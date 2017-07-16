//
//    save_env.cpp
//
//    Copyright 1995-1998 Barry A. Scott
//

#include <emacs.h>

# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );
#include <mem_man.h>

extern SystemExpressionRepresentationIntBoolean complete_unique_choices;
extern SystemExpressionRepresentationIntBoolean confirm_expansion_choice;
extern SystemExpressionRepresentationString backup_filename_format;

#ifndef SAVE_ENVIRONMENT
int save_environment( void )
{
    return no_value_command();
}

int restore_environment( unsigned char *fn )
{
    return 0;
}
#else
//
//    save environment implementation
//

EmacsSaveRestoreEnvironment *EmacsSaveRestoreEnvironmentObject;

# include <errlog.h>
# if defined(SUBPROCESSES)
#  ifdef vms
#   include <vms_comm.h>
#  endif
#  ifdef __unix__
#   include <unixcomm.h>
#  endif
#  if defined(_NT)
#   pragma warning( disable: 4201 )
#   include <windows.h>
#   include <nt_comm.h>
#   pragma warning( default: 4201 )
#  endif
# endif

extern QueueHeader<TimerEntry> timer_queue;

# ifdef _ptr
#  undef _ptr
# endif

int debug_save_index;        // address of block being saved to break on
void *debug_save_ptr;        // address of block being saved to break on
int debug_restore_index;    // index of block being restored to break on
void *debug_restore_ptr;

# define _ptr( field ) EmacsSaveRestoreEnvironmentObject->_serialize_ptr( (void **)&field )
# define _ptr_i( p ) EmacsSaveRestoreEnvironmentObject->_serialize_ptr_indirect( (void **)&p )
# define _serialize_var( var ) EmacsSaveRestoreEnvironmentObject->_serialize_bytes( (byte *)&var, sizeof( var ) )
# define _serialize_object( obj ) serialize_EmacsObject( &(obj), sizeof( obj ) )

class MemManException
{
    public:
        MemManException( const char *message ) { error_message = message; }
        const char *error_message;
};
class MemManReadError : public MemManException
{
    public:
        MemManReadError() : MemManException( "Read error" ) { };
};
class MemManWriteError : public MemManException
{
    public:
        MemManWriteError() : MemManException( "Write error" ) { };
};
class MemManOutOfMemory : public MemManException
{
    public:
        MemManOutOfMemory() : MemManException( "Out of memory" ) { };
};

extern int allocation_sequence_number;


// a list of all the markers that where restored
QueueHeader<Marker> restored_markers;

struct ptr_info_t
{
    char operating_system_name[128];
    char version_string[128];

    void **ptr_vector;
    int max_user_size;
    int max_index;
    struct queue *first_heap_entry;
    struct heap_entry *low_addr_ptr;
    struct heap_entry *high_addr_ptr;
};
struct ptr_info_t ptr_info;
static unsigned char the_end_of_save_marker[] = "[*<*(The end of the saved environment)*>*]";

EmacsSaveRestoreEnvironment::EmacsSaveRestoreEnvironment()
    : EmacsObject()
    , serialize_file( NULL )
    , saving_environment(false)
    , allocation_sequence_number_at_construction( allocation_sequence_number )
{
}

EmacsSaveRestoreEnvironment::~EmacsSaveRestoreEnvironment()
{
}

int save_environment(void)
{
    if( EmacsSaveRestoreEnvironmentObject )
    {
        EmacsSaveRestoreEnvironmentObject->DoSave();
        return 0;
    }

    return no_value_command();
}

class SavedEmacsView : public QueueEntry<SavedEmacsView>
{
public:
    EMACS_OBJECT_FUNCTIONS( SavedEmacsView )

    SavedEmacsView( EmacsView *, QueueHeader<SavedEmacsView> &header );
    virtual ~SavedEmacsView();

    EmacsView *original_view;
    EmacsWindowGroup saved_group;
};

SavedEmacsView::SavedEmacsView( EmacsView *view, QueueHeader<SavedEmacsView> &header )
    : original_view( view )
    , saved_group( view->windows )
{
    // kill off the windows structure
    EmacsWindowGroup tmp( *this );    // required to keep HP CC quite
    original_view->windows = tmp;    // was a sngle line
    // insert into the queue
    header.queueInsertAtTail( this );
}

SavedEmacsView::~SavedEmacsView()
{
    // restore the windows into the view
    if( original_view != NULL )
        original_view->windows = saved_group;

    // get out of the queue
    queueRemove();
}


void SavedEmacsView::SaveEnvironment()
{
    // Force original_view to be null on a restore as it is not in the
    // restored data
    if( !EmacsSaveRestoreEnvironmentObject->savingEnvironment() )
        original_view = NULL;

    QueueImplementation::SaveEnvironment();
    saved_group.SaveEnvironment();
}

// used by both save and restore - must be a file scope variable
static QueueHeader<SavedEmacsView> saved_views;

# if DBG_BUFFER
extern void check_markers( char * s, EmacsBuffer *b = bf_cur );
# endif

void EmacsSaveRestoreEnvironment::DoSave(void )
{
# if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_markers( "DoSave start", bf_cur );
# endif
    int index = 1;
    struct queue *entry;
    EmacsString save_file_name;

    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., FormatString( ": save-environment (%s) " ) << default_environment_file, fn );

    expand_and_default( fn, default_environment_file, save_file_name );
# ifdef vms
    // lose the version number
    fn = _str_rchr( save_file_name, ';' );
    if( fn != NULL )
        *fn = '\0';
# endif
    serialize_file = fopen( save_file_name, "wb" );
    if( serialize_file == NULL )
    {
        error( "error creating file" );
        return;
    }

    saving_environment = true;

    // update the emacs_buffer from the globals like bf_p1
    bf_cur->set_bf();
    // free up all error log memory
    delete_errlog_list();

    //
    // save all the windowgroups from the emacs views
    //
{
    QueueIterator<EmacsView> it( EmacsView::header );
    while( it.next() )
    {
        EmacsView *view = it.value();

        EMACS_NEW SavedEmacsView( view, saved_views );
    }
}

# if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_markers( "DoSave after save view", bf_cur );
# endif
    try{
        //
        //    write out the emacs version information to allow
        //    for version checking on restore
        //
        strcpy( ptr_info.version_string, version_string );
        strcpy( ptr_info.operating_system_name, operating_system_name.asString() );

        //
        //    serialize the heap
        //
        ptr_info.max_user_size = 0;
        ptr_info.low_addr_ptr = (struct heap_entry *)all_emacs_memory.next;
        ptr_info.high_addr_ptr = ptr_info.low_addr_ptr;
        if( ptr_info.ptr_vector != NULL )
        {
            emacs_free( ptr_info.ptr_vector );
            ptr_info.ptr_vector = NULL;
        }

        // scan the allocated memory and set the ptr_index field
        entry = all_emacs_memory.next;
        while( (void *)entry != (void *)&all_emacs_memory )
        {
            struct heap_entry *heap = (struct heap_entry *)entry;

            heap->ptr_index = index++;
# if DBG_ALLOC_CHECK
            heap->ref_count = 0;
# endif
            if( (int)heap->user_size > ptr_info.max_user_size )
                ptr_info.max_user_size = (int)heap->user_size;

            if( heap < ptr_info.low_addr_ptr )
                ptr_info.low_addr_ptr = heap;
            if( heap > ptr_info.high_addr_ptr )
                ptr_info.high_addr_ptr = heap;

            entry = entry->next;
        }

        // allocate the ptr_vector
        ptr_info.ptr_vector = (void **)emacs_malloc
            (
            sizeof( struct heap_entry *) * (index+1), malloc_type_char
# if DBG_ALLOC_CHECK
            , THIS_FILE, __LINE__
# endif
            );
        if( ptr_info.ptr_vector == NULL )
            throw MemManOutOfMemory();

        // fill in the ptr_vector values
        index = 1;
        entry = all_emacs_memory.next;
        while( (void *)entry != (void *)&all_emacs_memory )
        {
            struct heap_entry *heap = (struct heap_entry *)entry;

            if( heap->user_data == debug_save_ptr )
                debug_invoke();

            if( index == debug_save_index )
                debug_invoke();

            ptr_info.ptr_vector[index] = heap;
            index++;

            entry = entry->next;
        }

        ptr_info.max_index = index;

        // all but char types need a scratch buffer
        conversion_buffer = (byte *)emacs_malloc
            (
            ptr_info.max_user_size, malloc_type_char
# if DBG_ALLOC_CHECK
            , THIS_FILE, __LINE__
# endif
            );
        if( conversion_buffer == NULL )
            throw MemManOutOfMemory();

        //==================================================
        //
        // start serialising the dayta
        //
        //==================================================

        // need to know the ptr_info value to perform the restore
        _serialize_var( ptr_info );

        //
        // scan the allocated memory and output to the save file
        // but do not output the two malloced heap blocks used
        // by save environment.
        //
        entry = all_emacs_memory.next;
        for( index = 1; index<ptr_info.max_index; index++ )
        {
            struct heap_entry *heap = (struct heap_entry *)entry;
            int size = (int)heap->user_size;

            if( heap->user_data == debug_save_ptr )
                debug_invoke();

            // ignore objects allocated before the EmacsSaveRestoreEnvironment object
//            if( heap->sequence_number > allocation_sequence_number_at_construction )
            {
                _serialize_bytes( (byte *)&size, sizeof( size ) );
                _serialize_bytes( (byte *)&heap->user_type, sizeof( heap->user_type ) );
# if DBG_ALLOC_CHECK
                _serialize_bytes( (byte *)&heap->fileName, sizeof( heap->fileName ) );
                _serialize_bytes( (byte *)&heap->lineNumber, sizeof( heap->lineNumber ) );
# endif
                switch( heap->user_type )
                {
                case malloc_type_char:
                    _serialize_bytes( (byte *)&heap->user_data, size );
                    break;

                case malloc_type_star_star:
                    // make a copy for the routines to fix up pointers in
                    memcpy( conversion_buffer, heap->user_data, size );
                    serialize_star_star( (void **)conversion_buffer, size );
                    _serialize_bytes( conversion_buffer, size );
                    break;

                case malloc_type_emacs_object:
                {
                    serialize_EmacsObject( (EmacsObject *)(heap->user_data), size );
                    ptrdiff_t object = (char *)heap->user_data - (char *)heap->theObject;
                    _serialize_bytes( (byte *)&object, sizeof( object ) );
                }
                    break;

                case malloc_type_emacs_object_Expression_Vector:
//#ifdef _has_array_new_
//                {
//                    // first int is the number of objects
//                    int num_objects = *(int *)(heap->user_data);
//                    // followed by the objects them selves
//                    Expression *objects = (Expression *)(heap->user_data);
//
//                    // sanity check
//                    emacs_assert( *num_objects * sizeof(Expression) + sizeof(int) == size );
//
//                    for( int i=0; i<*num_objects; i++ )
//                        serialize_EmacsObject( (EmacsObject *)(&objects[i]), sizeof( Expression ) );
//                }
//#else
                {
                    Expression *objects = (Expression *)(heap->user_data);
                    int num_objects = size / sizeof( Expression );
                    for( int i=0; i<num_objects; i++ )
                        serialize_EmacsObject( (EmacsObject *)(&objects[i]), sizeof( Expression ) );
                }
//#endif
                    break;

                default:
# ifndef __GNUC__
                    _dbg_msg( FormatString( "Do not know how to save block type %d") << heap->user_type );
# endif    // __GNUC__
                    break;
                }
            }

            entry = entry->next;
        }

        //
        // need the saved view information
        //
        _serialize_object( saved_views );

        //
        //    serialize the SAVRES globals
        //
        serialize_savres_globals();

        //
        // put down a special end of save marker
        // which restore will check for to confirm
        // that the restore process read all the bytes
        // that where written
        //
        _serialize_var( the_end_of_save_marker );
        fclose( serialize_file );


# if DBG_ALLOC_CHECK && 0
        //
        // scan the allocated memory and report any block that are not referenced
        //
        _dbg_msg( "Start of Unused heap block dump\n" );
        entry = all_emacs_memory.next;
        for( index = 1; index<ptr_info.max_index; index++ )
        {
            struct heap_entry *heap = (struct heap_entry *)entry;
            int size = (int)heap->user_size;

            // move on to the next entry now before an delete of the current entry
            entry = entry->next;

            if( heap->user_data == debug_save_ptr )
                debug_invoke();

            if( heap->ref_count == 0 && !heap->lost_entry )
            {
                heap->lost_entry = true;

                switch( heap->user_type )
                {
                case malloc_type_char:
                    _dbg_msg( FormatString("Unused CHAR index %d seq %d at %X (%s,%d)\n")
                        << index << heap->sequence_number
                        << int( heap->user_data )
                        << heap->fileName << heap->lineNumber );
                    //emacs_free( heap->user_data );
                    break;

                case malloc_type_star_star:
                    _dbg_msg( FormatString("Unused CHAR_STAR index %d seq %d at %X (%s,%d)\n")
                        << index << heap->sequence_number
                        << int( heap->user_data )
                        << heap->fileName << heap->lineNumber );
                    //emacs_free( heap->user_data );
                    break;

                case malloc_type_emacs_object:
                {
                    EmacsObject *it = (EmacsObject *)(heap->user_data);
                    _dbg_msg( FormatString("Unused OBJECT %s index %d seq %d at %X (%s,%d)\n")
                        << it->objectName()
                        << index << heap->sequence_number
                        << int( heap->user_data )
                        << heap->fileName << heap->lineNumber );
                    //delete it;
                }
                    break;

                default:
                    _dbg_msg( FormatString("Unused Unknown block index %d %X\n")
                        << int( heap->user_data ) );
                 }
            }
        }
# endif

        // clean up temporary heap blocks
        emacs_free( ptr_info.ptr_vector );
        ptr_info.ptr_vector = NULL;

        emacs_free( conversion_buffer );
        conversion_buffer = NULL;

        if( cur_exec == NULL )
            // iteractive - report the status
            message( FormatString("Environment saved in %s") << save_file_name );
        else
            // MLisp code return the file name
            ml_value = save_file_name;
    }
    catch( MemManException e )
    {
        error( e.error_message );
    }
    //
    // restore the views
    //
{
    QueueIterator<SavedEmacsView> it( saved_views );
    while( it.next() )
        // the destructor does the restore
        delete it.value();
}

}

//
//    Called early in the emacs initialisation process
//
int EmacsSaveRestoreEnvironment::canRestore( const EmacsString &rest_fn, EmacsString &full_name, EmacsString &reason )
{
    if( rest_fn.isNull() )
        return 0;

    expand_and_default( rest_fn, default_environment_file, full_name );

    serialize_file = fopen( full_name, "rb" );
    if( serialize_file == NULL )
        return 0;

    saving_environment = false;

    //
    //    read in the emacs version information to allow
    //    for version checking
    //
    try{
        _serialize_var( ptr_info );
    }
    catch( MemManException & )
    {
        fclose( serialize_file );
        return 0;
    }

    ptr_info.ptr_vector = NULL;

    if( strcmp( ptr_info.version_string, version_string ) != 0 )
    {
        reason = "the environment file does not match this version of Emacs.";

        fclose( serialize_file );
        serialize_file = NULL;
        return -1;
    }
    if( strcmp( ptr_info.operating_system_name, operating_system_name.asString() ) != 0 )
    {
        reason = "the environment file does not match Emacs for this operating system.";

        fclose( serialize_file );
        serialize_file = NULL;
        return -1;
    }

    return 1;
}

bool EmacsSaveRestoreEnvironment::DoRestore(void)
{
    int index = 1;
    struct queue *entry;

    if( serialize_file == NULL )
        return false;

    try{
        //
        //    serialize the heap
        //

        // allocate the ptr_vector
        ptr_info.ptr_vector = (void **)emacs_malloc
                (
                sizeof( struct heap_entry *) * (ptr_info.max_index+1), malloc_type_char
# if DBG_ALLOC_CHECK
                , THIS_FILE, __LINE__
# endif
                );
        if( ptr_info.ptr_vector == NULL )
            throw MemManOutOfMemory();

        ptr_info.max_user_size = 0; // not used on restore
        ptr_info.low_addr_ptr = (struct heap_entry *)all_emacs_memory.next;
        ptr_info.high_addr_ptr = ptr_info.low_addr_ptr;
        // start scanning from the first heap block we restore
        ptr_info.first_heap_entry = all_emacs_memory.prev;

        // read in all the heap blocks
        for( index=1; index<ptr_info.max_index; index++ )
        {
            int size;
            enum malloc_block_type type;
            void *ptr;
            struct heap_entry *h;

            // find out the size and type of this block
            _serialize_var( size );
            _serialize_var( type );

            if( index == debug_restore_index )
                debug_invoke();
# if DBG_ALLOC_CHECK
            char *fileName;
            int lineNumber;
            _serialize_bytes( (byte *)&fileName, sizeof( fileName ) );
            _serialize_bytes( (byte *)&lineNumber, sizeof(lineNumber ) );
            // alloc the block
            ptr = emacs_malloc( size, type, fileName, lineNumber );
# else
            // alloc the block
            ptr = emacs_malloc( size, type );
# endif

            if( ptr == NULL )
                throw MemManOutOfMemory();

            if( ptr == debug_restore_ptr )
                debug_invoke();

            // get its contents
            _serialize_bytes( (byte *)ptr, size );

            // set the ptr_index field
            h = (struct heap_entry *)((char *)ptr-offsetof(struct heap_entry,user_data));
            h->ptr_index = index;
            if( type == malloc_type_emacs_object )
            {
                ptrdiff_t object;
                _serialize_bytes( (byte *)&object, sizeof( object ) );
                h->theObject = (EmacsObject *)((char *)ptr + object);
            }
            // build up the ptr_vector
            ptr_info.ptr_vector[index] = ptr;
        }

        // scan the restored memory and fix up the points
        entry = ptr_info.first_heap_entry->next;
        while( (void *)entry != (void *)&all_emacs_memory )
        {
            struct heap_entry *heap = (struct heap_entry *)entry;
            int size = (int)heap->user_size;

            // make a copy for the routines to fix up pointers in
            switch( heap->user_type )
        {
            case malloc_type_char:
                // nothing to do
                break;
            case malloc_type_star_star:
                serialize_star_star( (void **)heap->user_data, size );
                break;
            case malloc_type_emacs_object:
                ((EmacsObject *)heap->user_data)->SaveEnvironment();
                break;
            case malloc_type_emacs_object_Expression_Vector:
            {
                Expression *objects = (Expression *)(heap->user_data);
                int num_objects = size / sizeof( Expression );
                for( int i=0; i<num_objects; i++ )
                    objects[i].SaveEnvironment();
            }
                break;
            default:
                _dbg_msg( FormatString( "Do not know how to restore block type %d") << heap->user_type );
        }

            entry = entry->next;
        }

        //
        // need the saved view information
        //
        _serialize_object( saved_views );

        //
        //    serialize the SAVRES globals
        //
        serialize_savres_globals();

        unsigned char tmp[sizeof( the_end_of_save_marker )];
        _serialize_var( tmp );
        if( memcmp( (void *)tmp, (void *)the_end_of_save_marker, sizeof( tmp ) ) != 0 )
        {
            _dbg_msg( "end of save marker mismatch" );
            return false;
        }

        fclose( serialize_file );

        //
        //    Need to fix up all the markers
        //
    {
        Marker *m;
        while( (m = restored_markers.queueRemoveFirst()) != NULL )
            m->m_buf->b_markset.queueInsertAtTail( m );
    }

        // update the globals like bf_p1 from the emacs_buffer
        EmacsBuffer *b = bf_cur;
        b->set_bf();

# if DBG_BUFFER
        if( dbg_flags&DBG_BUFFER )
            check_markers( "DoRestore", bf_cur );
# endif

        // say that we have restored an environment
        is_restored = is_restored + 1;

        //
        // restore the views
        //
    {
        QueueIterator<SavedEmacsView> saved_it( saved_views );
        QueueIterator<EmacsView> view_it( EmacsView::header );
        while( saved_it.next() )
        {
            // if there is aview to match what is saved the link them
            if( view_it.next() )
                saved_it.value()->original_view = view_it.value();

            // the destructor does the restore
            delete saved_it.value();
        }
    }


        return true;
    }
    catch( MemManException e )
    {
        error( e.error_message );
    }

    return false;
}

void EmacsSaveRestoreEnvironment::_serialize_bytes( byte *bytes, int length )
{
    int io_size;

    if( saving_environment )
        io_size = fwrite( bytes, 1, length, serialize_file );
    else
        io_size = fread( bytes, 1, length, serialize_file );

    if( io_size != length )
    {
        error(  saving_environment ? "error writing saved environment" : "error reading saved environment"  );
        if( saving_environment )
            throw MemManWriteError();
        else
            throw MemManReadError();
    }
}

//
// the ptr is the value from a (thing *) value that
// needs converting between a ptr_index and a real ptr
// for a save convert from ptr to ptr_index.
// for a restore convert from ptr_index to ptr
//
void EmacsSaveRestoreEnvironment::_serialize_ptr( void **ptr )
{
    // map 0 to NULL and NULL to 0
    if( *ptr == NULL )
        return;

    if( saving_environment )
    {
        struct heap_entry *h;
        int index;

        h = (struct heap_entry *)(((char *)(*ptr))-offsetof(struct heap_entry,user_data));
        // only process addreses inside the heap
        if( h < ptr_info.low_addr_ptr || h > ptr_info.high_addr_ptr )
            return;

        // find the index out of the heap_entry structure
        index = h->ptr_index;
        if( index < 1 || index > ptr_info.max_index )
            return;
        // confirm its a heap_entry by checking the reverse mapping
        if( ptr_info.ptr_vector[index] != h )
            return;

        *(int *)ptr = -index;
# if DBG_ALLOC_CHECK
        h->ref_count++;
# endif
    }
    else
    {
        int index = -*(int *)ptr;

        if( debug_restore_index == index )
            debug_invoke();

        if( index >= 1 && index <= ptr_info.max_index )
            *ptr = ptr_info.ptr_vector[index];

        if( debug_restore_ptr == ptr )
            debug_invoke();
    }
}

void EmacsSaveRestoreEnvironment::_serialize_ptr_indirect( void **ptr )
{
    if( saving_environment )
    {
        void *p = *ptr;

        _serialize_ptr( &p );
        _serialize_bytes( (byte *)&p, sizeof( void * ) );
    }
    else
    {
        _serialize_bytes( (byte *)ptr, sizeof( void * ) );
        _serialize_ptr( ptr );
    }
}

void EmacsSaveRestoreEnvironment::serialize_EmacsObject( EmacsObject *object, int size )
{
    if( saving_environment )
    {
        // save the body of the object
        memcpy( conversion_buffer, object, size );

        object->SaveEnvironment();

        _serialize_bytes( (byte *)object, size );

        // restore the body of the object
        memcpy( (void *)object, conversion_buffer, size );
    }
    else
    {
        _serialize_bytes( (byte *)object, size );

        object->SaveEnvironment();
    }
}

void EmacsSaveRestoreEnvironment::serialize_savres_globals(void)
{
    int i;

    // save all the MLisp command names
    _serialize_object( BoundName::name_table );

    _serialize_object( default_syntax_array );
    _serialize_object( default_syntax_colouring );

    _serialize_object( SyntaxTable::name_table );

    _serialize_object( timer_list_head );

    _serialize_object( timer_queue );

    _serialize_object( global_syntax_table );
    _serialize_object( global_abbrev );
    _serialize_object( last_expression );
    _serialize_object( progn_block );
    _serialize_object( lambda_block );
    _serialize_object( interrupt_block );

    _serialize_object( is_restored );
//    _serialize_object( EmacsWindowGroup::header );
    _ptr_i( pushed_windows );
//    _ptr_i( theActiveView );
    _serialize_object( default_wrap_lines );
    _serialize_object( scroll_step );
    _serialize_object( scroll_step_horizontal );
    _serialize_object( automatic_horizontal_scroll );
    _serialize_object( quick_rd );
    _serialize_object( ctl_arrow );
    _serialize_object( global_mode_string );
    _serialize_object( pop_up_windows );
    _serialize_object( split_height_threshold );
    _serialize_object( protocol_mode );
    _serialize_object( visible_bell );
    _serialize_object( black_on_white );
    _serialize_object( unlink_checkpoint_files );
    _serialize_object( ask_about_buffer_names );
    _serialize_object( unlink_checkpoint_files );
    _serialize_object( backup_filename_format );
    _serialize_object( backup_file_mode );
    _ptr_i( auto_list );
    _ptr_i( checkpoint_proc );

    _ptr_i( buffer_choose_name_proc );
    _ptr_i( buffer_backup_filename_proc );
    _ptr_i( buffer_file_loaded_proc );
    _ptr_i( buffer_saved_as_proc );

    _serialize_object( swap_ctrl_x_char );

    _serialize_var( arg );
    _serialize_var( arg_state );
    _ptr_i( bf_cur );
    _serialize_object( EmacsBuffer::name_table );
    _serialize_var( col_valid );
    _serialize_var( dot );
    _serialize_var( dot_col );
    _serialize_object( key_mem );
    _serialize_var( last_key_struck );
    _serialize_object( last_keys_struck );
    _ptr_i( last_proc );
//TBD    _ptr_i( minibuf_body );
    _serialize_object( previous_command );
    _serialize_var( remembering );
//TBD    _ptr_i( reset_minibuf );
    _serialize_object( this_command );
    _serialize_var( quitting_emacs );
    _serialize_object( error_message_text );
    _serialize_object( stack_trace_on_error );
    _serialize_object( remove_help_window );
    _serialize_object( auto_help );
    _serialize_object( complete_unique_choices );
    _serialize_object( confirm_expansion_choice );
    _ptr_i( ml_value );
    _serialize_var( ml_err );
    _serialize_object( trace_into );
    _serialize_object( trace_mode );
    _ptr_i( exit_emacs_dcl_command );
    _serialize_object( track_eol );
    _serialize_var( argument_prefix_cnt );
#if 0
    _serialize_object( mode_line_rendition );
    _serialize_object( region_rendition );
    _serialize_object( window_rendition );
    _serialize_object( syntax_keyword1_rendition );
    _serialize_object( syntax_keyword2_rendition );
    _serialize_object( syntax_keyword3_rendition );
    _serialize_object( syntax_word_rendition );
    _serialize_object( syntax_string1_rendition );
    _serialize_object( syntax_string2_rendition );
    _serialize_object( syntax_string3_rendition );
    _serialize_object( syntax_quote_rendition );
    _serialize_object( syntax_comment1_rendition );
    _serialize_object( syntax_comment2_rendition );
    _serialize_object( syntax_comment3_rendition );
    _serialize_object( user_1_rendition );
    _serialize_object( user_2_rendition );
    _serialize_object( user_3_rendition );
    _serialize_object( user_4_rendition );
    _serialize_object( user_5_rendition );
    _serialize_object( user_6_rendition );
    _serialize_object( user_7_rendition );
    _serialize_object( user_8_rendition );
#endif
    _serialize_object( force_redisplay );
    _serialize_object( maximum_shell_buffer_size );
    _serialize_object( shell_buffer_reduction );
# ifdef SUBPROCESSES
#  ifdef vms
    _ptr_i( current_process );
    _ptr_i( process_ptrs );
    _ptr_i( process_names );
    _serialize_object( n_processes );
    _serialize_object( n_process_slots );
#  endif
# endif
    _serialize_object( ignore_version_numbers );
    _serialize_object( replace_case );
    _serialize_object( default_replace );
    _ptr_i( last_search_string );
    _serialize_object( get_db_help_flags );
    _serialize_object( DatabaseSearchList::name_table );
    _serialize_object( silently_kill_processes );
    _serialize_object( journalling_frequency );
    _serialize_object( journal_scratch_buffers );
    _serialize_object( animate_journal_recovery );
    _serialize_object( activity_indicator );
    _serialize_object( input_mode );
    _serialize_object( checkpoint_frequency );
    _serialize_var( end_of_mac );
    _ptr_i( current_global_map );
    _ptr_i( global_map );
    _ptr_i( minibuf_local_ns_map );
    _ptr_i( minibuf_local_map );
    _serialize_object( cs_enabled );
    _serialize_var( cs_modified );
    _serialize_object( cs_cvt_f_keys );
    _serialize_object( cs_cvt_mouse );
    _serialize_object( cs_cvt_8bit_string );
    _serialize_object( cs_cvt_csi_string );
    _serialize_object( cs_par_char_string );
    _serialize_object( cs_par_sep_string );
    _serialize_object( cs_int_char_string );
    _serialize_object( cs_fin_char_string );
    _serialize_var( cs_attr );
    _ptr_i( trace_proc );
    _ptr_i( break_proc );
    _ptr_i( error_proc );
    _serialize_var( last_auto_keymap );    // Index of last auto keymap
    _ptr_i( current_function );
    _ptr_i( current_break );
    _ptr_i( call_back_result );
    _ptr_i( call_back_str );
    _serialize_var( call_back_len );
    _serialize_object( AbbrevTable::name_table );
    _ptr_i( last_phrase );
    _serialize_var( bf_journalling );
    _ptr_i( buffers );    // root of the list of extant buffers
    _ptr_i( minibuf );    // The minibuf
    _serialize_object( default_mode_format );
    _serialize_object( default_replace_mode );
    _serialize_object( default_fold_case );
    _serialize_object( default_right_margin );
    _serialize_object( default_left_margin );
    _serialize_object( default_comment_column );
    _serialize_object( default_tab_size );
    _serialize_object( default_indent_use_tab );
    _serialize_object( default_highlight );
    _serialize_object( default_display_nonprinting );
    _serialize_object( default_display_eof );
    _serialize_object( default_display_c1 );
    _serialize_object( default_read_only_buffer );
    _ptr_i( default_auto_fill_proc );
    _ptr_i( auto_execute_proc );
    _serialize_object( VariableName::name_table );
    _ptr_i( error_message_parser );
    _serialize_object( end_of_line_style_override );
    _serialize_object( default_end_of_line_style );
    _serialize_object( compile_command );
    _serialize_object( debug_command );
    _serialize_object( filter_command );
    _serialize_object( execute_command );
    _serialize_object( cli_name );
# ifdef SUBPROCESSES
    _serialize_var( saved_buffer_count );
    for( i=0; i<saved_buffer_count; i++ )
        _ptr_i( saved_buffers[i] );
# endif

    _ptr_i( enter_emacs_proc );
    _ptr_i( exit_emacs_proc );
    _ptr_i( leave_emacs_proc );
    _ptr_i( return_to_emacs_proc );
    _ptr_i( user_interface_hook_proc );
}

void EmacsSaveRestoreEnvironment::serialize_star_star( void **ptr, int size )
{
    int i;

    size /= sizeof( void * );

    for( i = 0; i<size; i++ )
        _serialize_ptr( &ptr[i] );
}

void EmacsObject::SaveEnvironment()
{
    const char *object_name = objectName();
    (void)strlen( object_name );    // called to stop a warning
    debug_invoke();
}

void EmacsSaveRestoreEnvironment::SaveEnvironment()
{
    // never change this object as its active during a save and a restore
    return;
}

void AbbrevEntry::SaveEnvironment()
{
    _ptr( abbrev_next );
    abbrev_abbrev.SaveEnvironment();
    abbrev_phrase.SaveEnvironment();
    _ptr( abbrev_expansion_hook );
}

void AbbrevTable::SaveEnvironment()
{
    int i;

    abbrev_name.SaveEnvironment();
    for( i=0; i<ABBREVSIZE; i++ )
        _ptr( abbrev_table[i] );
}

void Binding::SaveEnvironment()
{
    _ptr( b_inner );
    _ptr( b_exp );
    _ptr( b_local_to );
}

void Binding_list::SaveEnvironment()
{
    bl_exp.SaveEnvironment();
    _ptr( bl_flink );
}

void BoundName::SaveEnvironment()
{
    b_proc_name.SaveEnvironment();

    _ptr( implementation );
}

void BoundNameNoDefine::SaveEnvironment(void)
{
    BoundName::SaveEnvironment();
}

//-----------------------------------------------------
void BoundNameInside::SaveEnvironment()
{ }

void BoundNameAutoLoad::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();

    b_module.SaveEnvironment();
}

void BoundNameBuiltin::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();
}

void BoundNameExternalFunction::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();
}

void BoundNameKeymap::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();

    _ptr( b_keymap );
}

void BoundNameMacro::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();

    b_body.SaveEnvironment();
}

void BoundNameProcedure::SaveEnvironment()
{
    BoundNameInside::SaveEnvironment();

    _ptr( b_prog );
}
//-----------------------------------------------------

void database::SaveEnvironment()
{
    // do not save with open file handles
    close_db();

    db_name.SaveEnvironment();
# ifndef vms
    dirnm.SaveEnvironment();
    datnm.SaveEnvironment();
    pagnm.SaveEnvironment();
# endif
}

void DatabaseSearchList::SaveEnvironment()
{
    int i;

    dbs_name.SaveEnvironment();

    for( i=0; i<SEARCH_LEN; i++ )
        _ptr( dbs_elements[i] );
}

void EmacsArray::SaveEnvironment()
{
    _ptr( array );
}

void EmacsArray::Array::SaveEnvironment()
{
    _ptr( values );
}

void EmacsBuffer::SaveEnvironment()
{
    _ptr( b_base );
    _ptr( b_syntax.syntax_base );
    _ptr( b_rendition_regions );
    b_buf_name.SaveEnvironment();
    b_fname.SaveEnvironment();
    b_checkpointfn.SaveEnvironment();
    _ptr( b_next );
    if( !EmacsSaveRestoreEnvironmentObject->savingEnvironment() )
        b_markset.queue_init_header();
    b_mark.SaveEnvironment();
    b_mode.SaveEnvironment();
    _ptr( b_journal );
}

void ModeSpecific::SaveEnvironment()
{
    _ptr( md_abbrev );
    _ptr( md_syntax );
    _ptr( md_keys );
    md_modestring.SaveEnvironment();
    md_modeformat.SaveEnvironment();
    md_prefixstring.SaveEnvironment();
    _ptr( md_auto_fill_proc );
    _ptr( md_process_key_proc );
}

void EmacsLine::SaveEnvironment()
{
    _ptr( line_next );
}

# if defined(SUBPROCESSES)
void EmacsProcessCommon::SaveEnvironment()
{
    proc_name.SaveEnvironment();
}

void EmacsProcess::SaveEnvironment()
{
    EmacsProcessCommon::SaveEnvironment();
#  ifdef vms
    _ptr( proc_procedure );

    _ptr( proc_input_channel.chan_process );
    _ptr( proc_input_channel.chan_buffer );
    _ptr( proc_input_channel.chan_procedure );

    _ptr( proc_output_channel.chan_process );
    _ptr( proc_output_channel.chan_buffer );
    _ptr( proc_output_channel.chan_procedure );

    if( !saving_environment )
    {
        // restore_process will insert all the channels back
        // into the channel list queue
        ptr->proc_input_channel.chan_queue.next = NULL;
        ptr->proc_input_channel.chan_queue.prev = NULL;
        ptr->proc_output_channel.chan_queue.next = NULL;
        ptr->proc_output_channel.chan_queue.prev = NULL;
    }
#  endif
#  ifdef __unix__
    throw MemManException("do not know how to save an environment with processes in it");
#  endif
}
# endif

void EmacsString::SaveEnvironment(void)
{
    _ptr( _rep );
}

void EmacsMacroString::SaveEnvironment(void)
{
    EmacsString::SaveEnvironment();
}

void EmacsStringRepresentation::SaveEnvironment(void)
{
    _ptr( data );
}

//-----------------------------------------------------
void EmacsStringTable::SaveEnvironment(void)
{
    _ptr( keys );
    _ptr( values );
}

void AbbrevNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void BufferNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void DatabaseEntryNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void DatabaseSearchListTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void EmacsFileTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void EmacsDirectoryTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void EmacsProcessNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void FunctionNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void SyntaxNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}

void VariableNameTable::SaveEnvironment(void)
{
    EmacsStringTable::SaveEnvironment();
}
//-----------------------------------------------------

void EmacsWindow::SaveEnvironment()
{
    _ptr( w_next );
    _ptr( w_prev );
    _ptr( w_right );
    _ptr( w_left );
    _ptr( w_buf );
    w_dot_.SaveEnvironment();
    w_mark_.SaveEnvironment();
    w_start_.SaveEnvironment();
    _ptr( group );
}

void EmacsWindowGroup::SaveEnvironment()
{
    if( !EmacsSaveRestoreEnvironmentObject->savingEnvironment() )
    {
        // restore into header queue
        queue_init_entry();
        header.queueInsertAtTail( this );
    }

    _ptr( windows );
    _ptr( view );
    one_line_start.SaveEnvironment();
    _ptr( current_window );
    current_buffer.SaveEnvironment();
}

void EmacsWindowRing::SaveEnvironment()
{
    _ptr( wr_pushed );
}

void EmacsWorkItem::SaveEnvironment(void)
{
    QueueImplementation::SaveEnvironment();
}

// journal_flush_work_item
// synchronise_files_work_item

void ErrorBlock::SaveEnvironment()
{
    _ptr( e_mess );
    _ptr( e_text );
    _ptr( e_next );
    _ptr( e_prev );
}

//-----------------------------------------------------
void Expression::SaveEnvironment()
{
    _ptr( data );
}

void SystemExpression::SaveEnvironment(void)
{
    Expression::SaveEnvironment();
}
//-----------------------------------------------------


//-----------------------------------------------------
void ExpressionRepresentation::SaveEnvironment(void)
{ }

void ExpressionRepresentationArray::SaveEnvironment(void)
{
    exp_array.SaveEnvironment();
}

void ExpressionRepresentationInt::SaveEnvironment(void)
{ }

void ExpressionRepresentationMarker::SaveEnvironment(void)
{
    _ptr( exp_marker );
}

void ExpressionRepresentationString::SaveEnvironment(void)
{
    exp_string.SaveEnvironment();
}

void ExpressionRepresentationWindowRing::SaveEnvironment(void)
{
    _ptr( exp_windows );
}

void SystemExpressionRepresentationArray::SaveEnvironment(void)
{
    exp_array.SaveEnvironment();
}

void SystemExpressionRepresentationInt::SaveEnvironment(void)
{ }

void SystemExpressionRepresentationIntReadOnly::SaveEnvironment(void)
{ }

void SystemExpressionRepresentationIntPositive::SaveEnvironment(void)
{ }

void SystemExpressionRepresentationIntBoolean::SaveEnvironment(void)
{ }

void SystemExpressionRepresentationDisplayBoolean::SaveEnvironment(void)
{ }

void SystemExpressionRepresentationString::SaveEnvironment(void)
{
    exp_string.SaveEnvironment();
}

void SystemExpressionRepresentationStringReadOnly::SaveEnvironment(void)
{
    SystemExpressionRepresentationString::SaveEnvironment();
}

void SystemExpressionRepresentationWindowRing::SaveEnvironment(void)
{
    _ptr( exp_windows );
}

void EmacsExternImage::SaveEnvironment()
{
    emacs_assert(false);    // need to wriye this
}
void EmacsExternFunction::SaveEnvironment()
{
    ef_funcname.SaveEnvironment();
    _ptr( ef_context );
}

void FileAutoMode::SaveEnvironment()
{
    a_pattern.SaveEnvironment();
    _ptr( a_what );
    _ptr( a_next );
}

void FileParse::SaveEnvironment(void)
{
    disk.SaveEnvironment();
    path.SaveEnvironment();
    filename.SaveEnvironment();
    filetype.SaveEnvironment();
    result_spec.SaveEnvironment();
}

void FormatString::SaveEnvironment(void)
{
    format.SaveEnvironment();
    result.SaveEnvironment();
    stringArg.SaveEnvironment();
}

void EmacsBufferJournal::SaveEnvironment()
{
    _ptr( jnl_file );
    jnl_jname.SaveEnvironment();
    _ptr( jnl_buf );
}

void KeyMap::SaveEnvironment()
{
    k_name.SaveEnvironment();
    _ptr( k_map );
}

void KeyMapShort::SaveEnvironment()
{
    for( int i=0; i<k_used; i++ )
        _ptr( k_sbinding[i] );
}

void KeyMapLong::SaveEnvironment()
{
    for( int i=0; i<256; i++ )
        _ptr( k_binding[i] );
}

void Marker::SaveEnvironment()
{
    _ptr( m_buf );

    if( !EmacsSaveRestoreEnvironmentObject->savingEnvironment() )
    {
        queue_init_entry();
        // need to fix it up
        if( m_buf != NULL )
            restored_markers.queueInsertAtTail( this );
    }
}

# ifdef _NT
void ProcessChannel::SaveEnvironment(void)
{ }
# endif

void ProgramNode::SaveEnvironment(void)
{
    _ptr( p_proc );
}

void ProgramNodeInt::SaveEnvironment(void)
{
    ProgramNode::SaveEnvironment();
}

void ProgramNodeNode::SaveEnvironment(void)
{
    ProgramNode::SaveEnvironment();
    _ptr( pa_node );
}

void ProgramNodeString::SaveEnvironment(void)
{
    ProgramNode::SaveEnvironment();
    pa_string.SaveEnvironment();
}

void ProgramNodeExpression::SaveEnvironment(void)
{
    ProgramNode::SaveEnvironment();
    pa_value.SaveEnvironment();
}

void ProgramNodeVariable::SaveEnvironment(void)
{
    ProgramNode::SaveEnvironment();
    _ptr( pa_name );
}

void SyntaxTable::SaveEnvironment()
{
    for( int i=0; i<256; i++ )
        _ptr( s_table[i].s_strings );
    s_name.SaveEnvironment();
}

void SyntaxString::SaveEnvironment()
{
    _ptr( s_next );
}

void QueueImplementation::SaveEnvironment()
{
    _ptr( _next );
    _ptr( _prev );
}

void TimerEntry::SaveEnvironment()
{
    QueueImplementation::SaveEnvironment();

    _ptr( timer );
}

void ProcTimerEntry::SaveEnvironment()
{
    QueueImplementation::SaveEnvironment();

    _ptr( timer );
}


void EmacsTimer::SaveEnvironment()
{
}

void ProcTimer::SaveEnvironment()
{
    EmacsTimer::SaveEnvironment();

    _ptr( tim_procedure_to_call );
}

void VariableName::SaveEnvironment()
{
    // lose the const from v_name and save
    ((EmacsString &)v_name).SaveEnvironment();
    _ptr( v_binding );
}
//-------------------------------------------------------

void RenditionRegion::SaveEnvironment()
{
    _ptr( rr_next );
    rr_start_mark.SaveEnvironment();
    rr_end_mark.SaveEnvironment();
}
void EmacsView::SaveEnvironment()
{
    QueueImplementation::SaveEnvironment();

    windows.SaveEnvironment();

    for( int i=0; i<MSCREENLENGTH+1; i++ )
    {
        _ptr_i( t_phys_screen[i] );
        _ptr_i( t_desired_screen[i] );
    }
}

#endif
