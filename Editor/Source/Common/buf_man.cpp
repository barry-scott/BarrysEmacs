//
//    buf_man.cpp
//    Copyright 1996 (c) Barry A. Scott
//

// buffer management commands

#include <emacs.h>
#include <string_map.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

SystemExpressionRepresentationEndOfLineStyle end_of_line_style_override( FIO_EOL__None );
SystemExpressionRepresentationBufferEndOfLineStyle buffer_end_of_line_style;

#if defined( __unix__ )
SystemExpressionRepresentationEndOfLineStyle default_end_of_line_style( FIO_EOL__StreamLF );

#elif defined( WIN32 )
SystemExpressionRepresentationEndOfLineStyle default_end_of_line_style( FIO_EOL__StreamCRLF );

#else
    #error must have a value for default_end_of_line_style
#endif

class SortedListOfEmacsStrings
{
public:
    SortedListOfEmacsStrings();
    ~SortedListOfEmacsStrings();

    void insert( const EmacsString &key, const EmacsString &value );

    void init_iterator() { position = header.queueFirst(); }
    const EmacsString *next_value();
private:
    class ListEntryForEmacsStrings : public QueueEntry<ListEntryForEmacsStrings>
    {
    public:
        ListEntryForEmacsStrings( const EmacsString &_key, const EmacsString &_value )
            : QueueEntry<ListEntryForEmacsStrings>()
            , key( _key )
            , value( _value )
        { }
        ~ListEntryForEmacsStrings() { };

        const EmacsString key;
        const EmacsString value;
    };

    QueueHeader<ListEntryForEmacsStrings> header;
    ListEntryForEmacsStrings *position;
};

SortedListOfEmacsStrings::SortedListOfEmacsStrings()
    : header()
    , position( NULL )
{ }

SortedListOfEmacsStrings::~SortedListOfEmacsStrings()
{
    while( !header.queue_empty() )
    {
        ListEntryForEmacsStrings *entry = header.queueFirst()->queueRemove();
        delete entry;
    }
}

void SortedListOfEmacsStrings::insert( const EmacsString &key, const EmacsString &value )
{
    ListEntryForEmacsStrings *entry = new ListEntryForEmacsStrings( key, value );

    QueueIterator<ListEntryForEmacsStrings> it( header );
    while( it.next() )
    {
        ListEntryForEmacsStrings *this_entry = it.value();
        if( this_entry->key > key )
        {
            entry->queueInsert( this_entry->queuePrev() );
            return;
        }
    }

    entry->queueInsert( header.queueLast() );
}

const EmacsString *SortedListOfEmacsStrings::next_value()
{
    if( header.queueEntryIsHeader( position ) )
        return NULL;

    const EmacsString *result = &position->value;

    position = position->queueNext();

    return result;
}

int list_buffers( void )
{
    SortedListOfEmacsStrings file_buffers;
    SortedListOfEmacsStrings other_buffers;

    EmacsBufferRef old( bf_cur );
    EmacsBuffer::scratch_bfn( "Buffer list", interactive() );
    bf_cur->ins_str(
"      Size  Type   Mode                Buffer                         File\n"
"      ----  ----   ----                ------                         ----\n" );
    EmacsBuffer *p = buffers;
    while( p != NULL )
    {
        ModeSpecific &mode = p->b_mode;

        EmacsString entry( FormatString("%10d%6s %c %c%c%c%c %-14s %-30s  %s\n" )
                            << (p->unrestrictedSize())
                            << (p->b_kind == FILEBUFFER ? "File" :
                                p->b_kind == MACROBUFFER ? "Macro" :
                                    "Scr")
                            << (p->b_modified != 0 ?  'M' : ' ')
                            << (mode.md_abbrevon ?  'A' : ' ')
                            << (p->b_checkpointed != -1 ?  'C' : ' ')
                            << (p->b_journalling && (p->b_kind == SCRATCHBUFFER ? int(journal_scratch_buffers) : 1) ?  'J' : ' ')
                            << (mode.md_replace ?  'R' : ' ')
                            << mode.md_modestring
                            << p->b_buf_name
                            << p->b_fname
                            );

        if( p->b_kind == FILEBUFFER )
        {
            file_buffers.insert( p->b_buf_name, entry );
        }
        else
        {
            other_buffers.insert( p->b_buf_name, entry );
        }

        p = p->b_next;
    }

    const EmacsString *value = NULL;

    file_buffers.init_iterator();
    while( (value = file_buffers.next_value()) != NULL )
    {
        bf_cur->ins_cstr( *value );
    }

    other_buffers.init_iterator();
    while( (value = other_buffers.next_value()) != NULL )
        bf_cur->ins_cstr( *value );

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

int delete_buffer_command( void )
{
    EmacsString buf_name;

    if( cur_exec == NULL )
        EmacsBuffer::get_esc_word_interactive( ": delete-buffer ", EmacsString::null, buf_name );
    else
        EmacsBuffer::get_esc_word_mlisp( buf_name );

    EmacsBuffer *b = EmacsBuffer::find( buf_name );
    if( b == NULL )
    {
        error( FormatString("Buffer \"%s\" does not exist") << buf_name );
        return 0;
    }
    if( b == minibuf )
    {
        error( FormatString("The Mini Buffer \"%s\" cannot be delete") << b->b_buf_name );
        return 0;
    }

    if( b->b_modified != 0 && interactive() )
    {
        if( !get_yes_or_no
            (
            0,
            FormatString("\"%s\" is modified, do you really want to delete it? ") << b->b_buf_name
            ) )
            return 0;
    }

    // delete the buffer now
    delete b;

    return 0;
}

int erase_buffer( void )
{
    bf_cur->erase_bf();
    return 0;
}



//--------------------------------------------------------------------------------
//
//    End of Line style (twined with RMS attribute)
//
//--------------------------------------------------------------------------------
static const char *end_of_line_style_names[] =
{
    "none",
    "binary",
    "crlf",
    "lf",
    NULL
};
static FIO_EOL_Attribute end_of_line_style_values[] =
{
    FIO_EOL__None,
    FIO_EOL__Binary,
    FIO_EOL__StreamCRLF,
    FIO_EOL__StreamLF,
};

StringMap<FIO_EOL_Attribute> end_of_line_style_attr( end_of_line_style_names, end_of_line_style_values );

void SystemExpressionRepresentationEndOfLineStyle::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value = new_value->asString();

    if( value.isNull() )
        return;

    value.toLower();

    if( !end_of_line_style_attr.map( value, exp_eol_attr ) )
        error( "Bad buffer end-of-line style specified" );
}

void SystemExpressionRepresentationEndOfLineStyle::fetch_value()
{
    exp_string = end_of_line_style_attr.map( exp_eol_attr );
}

//
// Buffer End of Line style
//
void SystemExpressionRepresentationBufferEndOfLineStyle::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationEndOfLineStyle::assign_value( new_value );
    bf_cur->b_eol_attribute = exp_eol_attr;
}

void SystemExpressionRepresentationBufferEndOfLineStyle::fetch_value()
{
    exp_eol_attr = bf_cur->b_eol_attribute;
    SystemExpressionRepresentationEndOfLineStyle::fetch_value();
}

//--------------------------------------------------------------------------------
//
//    Buffer Type
//
//--------------------------------------------------------------------------------
static const char *buffer_type_names[] =
{
    "file",
    "scratch",
    "macro",
    NULL
};

static buffer_types buffer_type_values[] =
{
    FILEBUFFER,
    SCRATCHBUFFER,
    MACROBUFFER
};

StringMap<buffer_types> buffer_types_map( buffer_type_names, buffer_type_values );

void SystemExpressionRepresentationBufferType::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );
    if( value.isNull() )
        return;
    buffer_types type_is;
    if( !buffer_types_map.map( value, type_is ) )
        error( "Bad buffer type specified" );
    else
    {
        if( bf_cur->b_kind != type_is )
        {
            bf_cur->b_fname = "";
            cant_1line_opt = redo_modes = 1;
        }
        bf_cur->b_kind = type_is;
    }
}

void SystemExpressionRepresentationBufferType::fetch_value()
{
    exp_string = buffer_types_map.map( bf_cur->b_kind );
}

void SystemExpressionRepresentationBufferFilename::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );
    if( value.isNull() )
        return;

    EmacsString fullname = save_abs( value );
    if( fullname.isNull() )
    {
        error( FormatString("\"%s\" is an illegal filename") << value );
        return;
    }

    bf_cur->b_kind = FILEBUFFER;
    bf_cur->b_fname = fullname;
    cant_1line_opt = redo_modes = 1;
}

void SystemExpressionRepresentationBufferFilename::fetch_value()
{
    if( bf_cur->b_kind == FILEBUFFER )
        exp_string = bf_cur->b_fname;
    else
        exp_string = "";
}

void SystemExpressionRepresentationBufferMacroName::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );
    if( value.isNull() )
        return;

    bf_cur->b_kind = MACROBUFFER;
    bf_cur->b_fname = value;
    cant_1line_opt = redo_modes = 1;
}

void SystemExpressionRepresentationBufferMacroName::fetch_value()
{
    if( bf_cur->b_kind == MACROBUFFER )
        exp_string = bf_cur->b_fname;
    else
        exp_string = "";
}

void SystemExpressionRepresentationBufferCheckpointable::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( (value&(~1)) != 0 )
        error( "Boolean value expected for current-buffer-checkpointable" );
    else
        if( value )
        {
            if( bf_cur->b_checkpointed < 0 )
                bf_cur->b_checkpointed = 0;
        }
        else
            bf_cur->b_checkpointed = -1;
    cant_1line_opt = redo_modes = 1;
}


void SystemExpressionRepresentationBufferCheckpointable::fetch_value()
{
    exp_int = bf_cur->b_checkpointed != -1;
}

void SystemExpressionRepresentationBufferName::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );
    if( value.isNull() )
        return;

    // check for a duplicate buffer name
    EmacsBuffer *b;
    if( (b = EmacsBuffer::find( value )) != NULL )
    {
        // no error if setting bf_Cur to its own name
        if( b != current_buffer_for_mlisp() )
            error( FormatString("Buffer name \"%s\" is already in use") << value );
        return;
    }


    current_buffer_for_mlisp()->rename( value );
    // remove from the name table
    b = EmacsBuffer::name_table.remove( current_buffer_for_mlisp()->b_buf_name );
    // change the name
    b->b_buf_name = value;
    // put the buffer back
    EmacsBuffer::name_table.add( value, b );
}

void SystemExpressionRepresentationBufferName::fetch_value()
{
    exp_string = current_buffer_for_mlisp()->b_buf_name;
}

void SystemExpressionRepresentationBufferKeymap::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );

    if( value.isNull() )
    {
        bf_cur->b_mode.md_keys = NULL;
        return;
    }

    BoundName *proc = BoundName::find( value );

    if( proc == NULL )
        error( FormatString("%s does not exist.") << value );
    if( proc->getKeyMap() == NULL )
        error( FormatString("%s is not a keymap.") << proc->b_proc_name );
    else
    {
        bf_cur->b_mode.md_keys = proc->getKeyMap();
        next_global_keymap = next_local_keymap = NULL;
    }
}

//
// The following are the fetch thunks for the above variables
//
void SystemExpressionRepresentationBufferKeymap::fetch_value()
{
    if( bf_cur->b_mode.md_keys != NULL )
        exp_string = bf_cur->b_mode.md_keys->k_name;
    else
        exp_string = EmacsString::null;
}
