//
//     Copyright (c) 1982-2016
//        Barry A. Scott
//
// File IO for Emacs
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#ifdef vms
#include <descrip.h>
#endif

// Table of contents

static void do_auto( const EmacsString &filename );
int apply_auto_execute( void );
int auto_execute( void );
int unlink_file( void );
int file_exists( void );
int expand_file_name( void );
int write_file_exit( void );
int insert_file( void );
int write_modified_files( void );
int read_file_command( void );
int write_current_file( void );
int visit_file_command( void );
int write_named_file_command( void );
int write_named_file( EmacsString &fn );
int append_to_file( void );
EmacsString makeBufferName( EmacsString &fullname, EmacsBuffer *existing_buffer );
int visit_file( const EmacsString &fn, int createnew, int windowfiddle, const EmacsString &dn );
static EmacsString concoct_name( const EmacsString &fn, const EmacsString &extension );
bool mod_exist( void );
int check_yes_no( const EmacsString &s, ... );
bool mod_write( void );
void kill_checkpoint_files( void );
int checkpoint_everything(void);
int checkpoint_buffers(void);
EmacsString fetch_os_error( int error_code );
int synchronise_files(void);
EmacsString file_format_string( const EmacsString &format, const EmacsString &filename );
EmacsString file_format_string( const EmacsString &format, const FileParse &fab );
bool file_read_veto( const EmacsString &filename );

// Static strings including error messages
static EmacsString ckp_ext( "EMACS_CHECKPOINT:.CKP" );
static EmacsString defname( "EMACS_CHECKPOINT:CHECKPNT.CKP" );
static EmacsString perror_str( "%s: %s" );
static EmacsString null_file_spec( "You must specify a file name" );

// default backup_files based on support of file versions
const int BACKUP_FILE_MODE_NONE = 0;
const int BACKUP_FILE_MODE_COPY = 1;
const int BACKUP_FILE_MODE_RENAME = 2;

SystemExpressionRepresentationIntBoolean ask_about_synchronise_for_none_modified_buffers;
SystemExpressionRepresentationBackupFileMode backup_file_mode
#ifdef VERS_CH
    ( BACKUP_FILE_MODE_NONE );
#else
    ( BACKUP_FILE_MODE_COPY );
#endif
SystemExpressionRepresentationString backup_filename_format;

SystemExpressionRepresentationIntReadOnly is_not_accessible( 0 );
SystemExpressionRepresentationIntReadOnly is_read_write( 1 );
SystemExpressionRepresentationIntReadOnly is_read_only( -1 );
SystemExpressionRepresentationInt maximum_file_read_size( 10*1024*1024 );

SystemExpressionRepresentationIntBoolean unlink_checkpoint_files;

BoundName *auto_execute_proc;

// If true (the default) Emacs will ask
// instead of synthesizing a unique name in
// the case where visit-file encounters a
// conflict in generated buffer names.
SystemExpressionRepresentationIntBoolean ask_about_buffer_names( 1 );

//
//    true if synchronise buffers should be called on aquiring focus
//
SystemExpressionRepresentationIntBoolean synchronise_buffers_on_focus( 1 );

// Various local variables

FileAutoMode *auto_list;
BoundName *checkpoint_proc;

BoundName *buffer_choose_name_proc;
BoundName *buffer_backup_filename_proc;
BoundName *buffer_file_loaded_proc;
BoundName *buffer_saved_as_proc;

static EmacsString wrote_file;
static int write_errors;

void SystemExpressionRepresentationBackupFileMode::assign_value( ExpressionRepresentation *new_value )
{
    const EmacsString &value = new_value->asString();
    if( value.isNull() )
    {
        return;
    }

    if( value == "none" )
    {
        exp_int = BACKUP_FILE_MODE_NONE;
    }

    else if( value == "copy" )
    {
        exp_int = BACKUP_FILE_MODE_COPY;
    }

    else if( value == "rename" )
    {
        exp_int = BACKUP_FILE_MODE_RENAME;
    }
    else
    {
        throw EmacsExceptionTextError( "Unknown backup file mode" );
    }
}

void SystemExpressionRepresentationBackupFileMode::fetch_value()
{
    switch( exp_int )
    {
    default:
    case BACKUP_FILE_MODE_NONE:    exp_string = "none"; break;
    case BACKUP_FILE_MODE_COPY:    exp_string = "copy"; break;
    case BACKUP_FILE_MODE_RENAME:    exp_string = "rename"; break;
    }
}

int cur_dir( void )
{
    ml_value = current_directory.asString();
    return 0;
}

bool callProc( BoundName *proc, const EmacsString &str )
{
    if( proc != NULL
    && proc->isBound() )
    {
        ProgramNodeNode prog_node( proc, 1 );
        // must new the ProgramNodeString as its deleted via the NodeNode d'tor
        prog_node.pa_node[0] = new ProgramNodeString( str );

        exec_prog( &prog_node );

        return true;
    }

    return false;
}

// Perform the auto-execute action (if any) for the specified filename

static void do_auto( const EmacsString &filename )
{
    EmacsString fn;
    int saverr = ml_err;

    ml_err = 0;

    //
    // trim off the directory path at the front of the filename and
    // and the version number from the end
    //
    int fn_in = filename.last( PATH_CH );
    if( fn_in < 0 )
    {
        fn = filename;
    }
    else
    {
        fn = filename( fn_in+1, INT_MAX );
    }

#ifdef VERS_CH
    fn_in = fn.first( VERS_CH );
    if( fn_in > 0 )
    {
        fn.remove( fn_in );
    }
#endif

    fn.toLower();

    //
    // Scan the list of autoexecutes. For each pattern that matches the
    // supplied file name, execute the supplied function
    //
    bool match_found = false;

    for( FileAutoMode *p = auto_list; p != NULL; p = p->a_next )
        if( match_wild( fn, p->a_pattern ) )
        {
            match_found = true;
            execute_bound_saved_environment( p->a_what );
            break;
        }

    if( !match_found && auto_execute_proc != NULL )
    {
        // run the auto_execute_hook
        execute_bound_saved_environment( auto_execute_proc );
    }

    ml_err |= saverr;
}


// The apply-auto-execute function interface
int apply_auto_execute( void )
{
    EmacsString file;
    if( cur_exec == NULL )
    {
        file = get_string_interactive(": apply-auto-execute ");
    }
    else
    {
        file = get_string_mlisp();
    }

    if( !file.isNull() )
    {
        do_auto( file );
    }

    return 0;
}   // of ApplyAutoExecute


// Set up an auto-execute - function interface
int auto_execute( void )
{
    BoundName *what;
    if( cur_exec == NULL )
    {
        what = BoundName::get_word_interactive( ": auto-execute " );
    }
    else
    {
        what = BoundName::get_word_mlisp();
    }

    if( what == NULL )
    {
        return 0;
    }

    EmacsString pattern;
    if( cur_exec == NULL )
    {
        pattern = get_string_interactive( FormatString(": auto-execute %s when name matches ") <<
            what->b_proc_name );
    }
    else
    {
        pattern = get_string_mlisp();
    }

    if( pattern.isNull() )
    {
        return 0;
    }

    FileAutoMode *p = EMACS_NEW FileAutoMode;
    if( p == NULL )
    {
        error( "Out of memory!" );
        return 0;
    }

//    pattern.toLower();

    p->a_pattern = pattern;
    p->a_what = what;
    p->a_next = auto_list;
    auto_list = p;

    return 0;
}   // of AutoExecute


int list_auto_executes( void )
{
    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Auto Execute list", interactive() );

    bf_cur->ins_cstr( "  Pattern               Function\n"
                      "  -------               --------\n" );

    for( FileAutoMode *p = auto_list; p != NULL; p = p->a_next )
    {
        bf_cur->ins_cstr( FormatString("  %-20s  %s\n") << p->a_pattern << p->a_what->b_proc_name );
    }

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

int match_wild_command(void)
{
    if( check_args( 2, 2 ) )
    {
        return 0;
    }

    EmacsString candidate = get_string_mlisp();
    EmacsString pattern = get_string_mlisp();

    ml_value = match_wild( candidate, pattern );

    return 0;
}



// Function interface to delete a file
int unlink_file( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    if( cur_exec == NULL )
    {
        file_table.get_word_interactive( ": unlink-file ", fn );
    }
    else
    {
        file_table.get_word_mlisp( fn );
    }
    if( fn.isNull() )
    {
        return 0;
    }

    EmacsString fullname;
    expand_and_default (fn, EmacsString::null, fullname);

    ml_value = Expression( EmacsFile::fio_delete( fullname ) ? 0 : -1 );
    return 0;
}

// Function interface to check if a file exists
int file_exists( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., ": file-exists ", fn );
    EmacsString fullname;

    if( fn.isNull() )
    {
        ml_value = Expression( is_not_accessible );
    }
    else
    {
        expand_and_default (fn, EmacsString::null, fullname);

        if( fullname.isNull() )
        {
            ml_value = Expression( is_not_accessible );
        }
        else
        {
            ml_value = Expression( EmacsFile::fio_access( fullname ) );
        }
    }

    return 0;
}

class FileFindRecursiveInternal : public FileFind
{
public:
    FileFindRecursiveInternal( const EmacsString &_name )
    : FileFind( _name, true )
    , inner( NULL )
    { }
    virtual ~FileFindRecursiveInternal()
    { }

    FileFindRecursiveInternal *inner;
};

class FileFindRecursive
{
public:
    FileFindRecursive( const EmacsString &name )
    : original_name( name )
    , finder( new FileFindRecursiveInternal( name ) )
    { }

    virtual ~FileFindRecursive()
    {
        delete finder;
    }
    virtual EmacsString next();
private:
    EmacsString original_name;
    FileFindRecursiveInternal *finder;
};

EmacsString FileFindRecursive::next()
{
    for(;;)
    {
        EmacsString file( finder->next() );
        if( file.isNull() )
        {
            FileFindRecursiveInternal *inner = finder->inner;
            if( inner == NULL )
            {
                return file;
            }

            delete finder;
            finder = inner;

            // spin around and get another file
        }
        else
        {
            // is it a dir?
            if( file[-1] == PATH_CH )
            {
                // yes find on the inner
                EmacsString fullname;
                expand_and_default( file, original_name, fullname );
                FileFindRecursiveInternal *new_finder = new FileFindRecursiveInternal( fullname );
                new_finder->inner = finder;
                finder = new_finder;

                // spin around and get another file
            }
            else
            {
                // no so return the file
                return file;
            }
        }
    }
}

int file_name_expand_and_default(void)
{
    EmacsFileTable file_table;
    EmacsString fn, def;
    getescword( file_table., ": file-name-expand-and-default (filename) ", fn );
    getescword( file_table., ": file-name-expand-and-default (default-filename) ", def );

    EmacsString fullname;
    expand_and_default (fn, def, fullname);

    ml_value = fullname;
    return 0;
}

//
//    format string contains the following escapes
//
//    device:\path1\path2\path3\filename.filetype
//    %<n>ps - path split           : sets the divide between head and tail of the path //
//                                  : at position <n>                                   //
//    %pa    - path all             : device:\path1\path2\path3\                        //
//    %ph    - path head            : device:\path1\path2\                              //
//    %pt    - path tail            : path3\                                            //
//    %pv    - path device          : device:                                           //
//    %pc    - path char            : PATH_CH                                           //
//    %pd    - path directory       : \path1\path2\path3\                               //
//    %fn    - filename             : filename                                          //
//    %ft    - filetype             : .filetype                                         //
//    %fa    - filename and type    : filename.filetype                                 //
//
//    Path split is needs to take into acount what happends if there are insufficent
//    path parts to set the split at <n>. THe rule is that head + tail == the full path
//
//

//
//    if the split_point is:-
//        +ive count from the start of the path
//        -ive count from the end of the path
//    0 means all the path is in the head, null tail
//    if there are less then count parts then if
//    +ive put all in the head. if -ive put it all in the tail
//
//    the head is always the root
//    windows - x:\                                                                     //
//    unix    - /                                                                       //
//
static void file_format_string_path_split
    (
    const FileParse &fab, int split_point,
    EmacsString &head, EmacsString &tail
    )
{
    // find the root position
    EmacsString fullpath = fab.disk;
    fullpath.append( fab.path );

    if( split_point == 0 )
    {
        return;
    }

    // count the path parts
    int pos = 0;
    int num_path_parts = -1;

    while( (pos = head.index( PATH_CH, pos+1 )) >= 0 )
    {
        num_path_parts++;
    }

    if( split_point < 0 )
    {
        split_point = num_path_parts + split_point;

        if( split_point <= 0 )
        {
            split_point = 1;
        }
    }
    else
    {
        if( split_point > num_path_parts )
        {
            split_point = num_path_parts;
        }
    }

    // find index of split_point path fragment
    pos = -1;
    while( (pos = head.index( PATH_CH, pos+1 )) >= 0 && split_point > 0)
    {
        split_point--;
    }

    // return the parts
    head = fullpath( 0, pos+1 );
    tail = fullpath( pos+1, fullpath.length() );
}

EmacsString file_format_string( const EmacsString &format, const EmacsString &filename )
{
    FileParse fab;
    fab.sys_parse( filename, EmacsString::null );

    return file_format_string( format, fab );
}

EmacsString file_format_string( const EmacsString &format, const FileParse &fab )
{
    EmacsString result;

    EmacsString path_head( fab.disk );      // the full path
    path_head.append( fab.path );

    EmacsString path_tail;                  // null

    EmacsStringIterator format_iterator( format );

    char ch = 0;
    while( format_iterator.next( ch ) )
    {
        if( ch != '%' )
        {
            result.append( ch );
        }
        else
        {
            char format_ch_1 = 0;
            int format_arg = 0;
            bool negative_arg = false;

            for(;;)
            {
                if( !format_iterator.next( format_ch_1 ) )
                {
                    error("file-format-string format too short");
                    return EmacsString::null;
                }

                if( format_ch_1 >= '0' && format_ch_1 <= '9' )
                {
                    format_arg = format_arg*10 + format_ch_1-'0';
                }

                else if( !negative_arg && format_ch_1 == '-' )
                {
                    negative_arg = true;
                }
                else
                {
                    break;
                }
            }

            if( negative_arg )
            {
                format_arg = -format_arg;
            }

            switch( format_ch_1 )
            {
            case '%':
                result.append( '%' );
                break;

            case 'p':    // path operations
            {
                char format_ch_2 = 0;
                if( !format_iterator.next( format_ch_2 ) )
                {
                    error("file-format-string format string too short");
                    return EmacsString::null;
                }

                switch( format_ch_2 )
                {
                case 'c':
                    result.append( PATH_CH );
                    break;
                case 'a':
                    result.append( fab.disk );
                    result.append( fab.path);
                    break;
                case 'h':
                    result.append( path_head );
                    break;
                case 't':
                    result.append( path_tail );
                    break;
                case 'v':
                    result.append( fab.disk );
                    break;
                case 'd':
                    result.append( fab.path);
                    break;
                case 's':
                    file_format_string_path_split( fab, format_arg, path_head, path_tail );
                    break;
                default:
                    error(FormatString("file-format-string unknown format type \"%%%c%c\"") << format_ch_1 << format_ch_2 );
                    return EmacsString::null;
                }
            }
                break;

            case 'f':    // file operations
            {
                char format_ch_2 = 0;
                if( !format_iterator.next( format_ch_2 ) )
                {
                    error("file-format-string format string too short");
                    return EmacsString::null;
                }

                switch( format_ch_2 )
                {
                case 'n':
                    result.append( fab.filename );
                    break;
                case 't':
                    if( format_arg >= 0 )
                        result.append( fab.filetype( format_arg, fab.filetype.length() ) );
                    else
                        result.append( fab.filetype( 0, format_arg ) );
                    break;
                case 'a':
                    result.append( fab.filename );
                    result.append( fab.filetype);
                    break;
                default:
                    error(FormatString("file-format-string unknown format type \"%%%c%c\"") << format_ch_1 << format_ch_2 );
                    return EmacsString::null;
                }
            }
                break;

            default:
                error(FormatString("file-format-string unknown format type \"%%%c\"") << format_ch_1 );
                return EmacsString::null;
            }
        }
    }

    return result;
}


int file_format_string_cmd(void)
{
    EmacsString format;
    EmacsString filename;

    if( cur_exec == NULL )
    {
        EmacsFileTable file_table;
        format = get_string_interactive( ": file-format-string (format) ");
        getescword( file_table., ": file-format-string (filename) ", filename );
    }
    else
    {
        if( check_args( 2, 2 ) )
            return 0;
        format = get_string_mlisp();
        filename = get_string_mlisp();
    }

    EmacsString result = file_format_string( format, filename );

    ml_value = result;

    return 0;
}


int file_is_a_directory_cmd(void)
{
    EmacsFileTable file_table;
    EmacsString fn;

    getescword( file_table., ": file-is-a-directory ", fn );

    EmacsString fullname;
    expand_and_default( fn, EmacsString::null, fullname );

    if( !fullname.isNull()
    && file_is_directory( fullname ) )
    {
        ml_value = int(true);
    }
    else
    {
        ml_value = int(false);
    }

    return 0;
}

int expand_file_name( void )
{
    static FileFind *search_file_handle;
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., ": expand-file-name ", fn );

    if( fn.length() > 0 )
    {
        EmacsString fullname;

        expand_and_default (fn, EmacsString::null, fullname);

        delete search_file_handle;
        search_file_handle = new FileFind( fullname );
        if( search_file_handle == NULL )
        {
            error( "No Mem" );
            return 0;
        }
    }

    if( search_file_handle == NULL )
    {
        error( "expand-file-name has not been initialised" );
        return 0;
    }

    EmacsString file( search_file_handle->next() );
    if( file.isNull() )
    {
        delete search_file_handle;
        search_file_handle = NULL;
    }

    ml_value = file;

    return 0;
}

int expand_file_name_recursive( void )
{
    static FileFindRecursive *search_file_handle;
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., ": expand-file-name-recursive ", fn );

    if( fn.length() > 0 )
    {
        EmacsString fullname;

        expand_and_default (fn, EmacsString::null, fullname);
        if( fullname[-1] == PATH_CH )
        {
            error( "No filename only a directory given" );
            return 0;
        }

        delete search_file_handle;
        search_file_handle = new FileFindRecursive( fullname );
        if( search_file_handle == NULL )
        {
            error( "No Mem" );
            return 0;
        }
    }

    if( search_file_handle == NULL )
    {
        error( "expand-file-name-recursive has not been initialised" );
        return 0;
    }

    EmacsString file( search_file_handle->next() );
    if( file.isNull() )
    {
        delete search_file_handle;
        search_file_handle = NULL;
    }

    ml_value = file;

    return 0;
}

// Function interface to write all files, and exit Emacs
int write_file_exit( void )
{
    if( mod_write () != 0 )
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

static void backup_buffer( EmacsString &fn )
{
    //
    //    A backup file name has an "_" added to the front of the type
    //
    FileParse fab;
    fab.sys_parse( fn, EmacsString::null );

    //BoundName *proc = BoundName::find( "create-backup-filename" );

    bool use_builtin_rule = true;
    EmacsString string( fab.result_spec );
    if( callProc( buffer_backup_filename_proc, string ) )
    {
        try
        {
            fab.sys_parse( ml_value.asString(), fn );
            use_builtin_rule = false;
        }
        catch( EmacsExceptionExpressionNotString )
        {
            use_builtin_rule = true;
        }
    }

    if( use_builtin_rule )
    {
        // use backup format to generate the backup filename
        EmacsString backup_filename = file_format_string( backup_filename_format.asString(), fab );
        EmacsString original_filename = fab.result_spec;

        fab.sys_parse( backup_filename, fn );

        if( original_filename == fab.result_spec )
        {
            error( "Backup filename is the same as the original filename" );
            return;
        }
    }

    // delete the old backup file
    if( remove( fab.result_spec ) != 0 && errno == EACCES )
    {
        error( FormatString("Failed to delete %s") << fab.result_spec );
        return;
    }

    if( backup_file_mode == BACKUP_FILE_MODE_RENAME )
    {
        //
        // rename the old file to the backup file name
        // but expect fn not to exist
        //
        if( rename( fn, fab.result_spec ) != 0 && errno != ENOENT )
        {
            error( FormatString("Failed to rename %s to %s") << fn << fab.result_spec );
            return;
        }
    }
    else
    {
        //
        // copy the old file to the backup file name
        //
        EmacsFile in( FIO_EOL__Binary );
        EmacsFile out( FIO_EOL__Binary );
        unsigned char buf[16384];

        // Open the input file
        if( !in.fio_open( fn, 0, EmacsString::null ) )
        {
            if( errno != ENOENT )
                error( FormatString("Failed to open file for backup %s") << fn );
            return;
        }

        // Create the output file
        if( !out.fio_create( fab.result_spec, 0, FIO_STD, EmacsString::null, FIO_EOL__Binary ) )
        {
            error( FormatString("Failed to create file for backup %s") <<fab.result_spec );
            return;
        }
        // copy all the input file to the output file
        int len;
        while( (len = in.fio_get( buf, sizeof(buf) )) > 0 )
            if( out.fio_put( buf, len ) < 0 )
            {
                error( FormatString("Error writing while backing up to %s") << fab.result_spec );
                return;
            }

        if( len < 0 )
        {
            error( FormatString("Error reading while backing up from %s") << fab.result_spec );
            return;
        }
    }
}

// Internal routine to write a buffer
int write_this( const EmacsString &fname )
{
    int rv = 0;
    EmacsString fn;

    if( fname.isNull() )
    {
        fn = bf_cur->b_fname;
    }
    else
    {
        fn = fname;
    }
    if( fn.isNull() )
    {
        error( FormatString("No file name associated with buffer %s") << bf_cur->b_buf_name);
        return rv;
    }

#ifdef VERS_CH
    {
    // lose any version numbers
    int pos = fn.first( VERS_CH );
    if( pos >=0 )
    {
        fn.remove( pos+1, INT_MAX );
    }
    }
#endif

    if( backup_file_mode != BACKUP_FILE_MODE_NONE
    && bf_cur->b_file_time != 0 )
    {
        backup_buffer( fn );
        if( ml_err )
        {
            return -1;
        }
    }

    if( bf_cur->write_file( fn, EmacsBuffer::ORDINARY_WRITE ) != 0 )
    {
        bf_cur->b_fname = wrote_file;
        delete bf_cur->b_journal;
        bf_cur->b_journal = NULL;
        rv = -1;
    }

    if( unlink_checkpoint_files != 0 )
    {
        if( !ml_err
        && bf_cur->b_checkpointfn.length() > 0 )
        {
            EmacsFile::fio_delete( bf_cur->b_checkpointfn );
        }

        if( bf_cur->b_checkpointed > 0 )
        {
            bf_cur->b_checkpointed = 0;
        }
    }

    return rv;
}


// Function interface to insert-file
int insert_file( void )
{
    int old_size;

    if( bf_cur->b_mode.md_readonly )
    {
        // call insert_at to generate the read only error message
        bf_cur->insert_at( 1, 0 );
        return 0;
    }

    old_size = bf_cur->unrestrictedSize();

    EmacsFileTable file_table;
    EmacsString fn;
    if( cur_exec == NULL )
    {
        file_table.get_word_interactive( ": insert-file ", fn );
    }
    else
    {
        file_table.get_word_mlisp( fn );
    }

    EmacsString fullname;
    expand_and_default( fn, EmacsString::null, fullname );

    if( file_read_veto( fullname ) )
    {
        return 0;
    }

    if( bf_cur->read_file( fn, 0, 0 ) != 0
    || interrupt_key_struck != 0 )
    {
        if( bf_cur->b_modified == 0 )
        {
            redo_modes = 1;
            cant_1line_opt = 1;
        }
        bf_cur->b_modified++;
    }

    if( bf_cur->b_mode.md_syntax_colouring )
    {
        syntax_insert_update( dot, (bf_cur->unrestrictedSize()) - old_size );
    }

    return 0;
}                // Of InsertFile


// Function interface to write all modified files
int write_modified_files( void )
{
    mod_write();
    return 0;
}


int read_file_command( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., ": read-file ", fn );

    if( fn.isNull() )
    {
        return 0;
    }

    EmacsString fullname;
    expand_and_default( fn, EmacsString::null, fullname );

    if( file_read_veto( fullname ) )
    {
        return 0;
    }

    bf_cur->read_file( fullname, 1, 0 );
    if( !callProc( buffer_file_loaded_proc, bf_cur->b_buf_name ) )
    {
        do_auto( fn );
    }

    return 0;
}                // Of ReadFile_command


// Code to write the current file
int write_current_file( void )
{
    write_this( EmacsString::null );

    cant_1line_opt = 1;
    redo_modes = 1;

    return 0;
}                // Of WriteCurrentFile


// Function interface to visit_file
int visit_file_command( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table.,  ": visit-file ", fn );

    visit_file( fn, 1, 1, EmacsString::null );

    return 0;
}                // Of VisitFile_command


// Function interface to write a specific file
int write_named_file_command( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    if( cur_exec == NULL )
    {
        EmacsString default_filename;

        // use the file buffer last filename
        if( bf_cur->b_kind == FILEBUFFER )
        {
            default_filename = bf_cur->b_fname;
        }
        // if all else fails use the buffer name
        if( default_filename.isNull() )
        {
            default_filename = bf_cur->b_buf_name;
        }
        file_table.get_esc_word_interactive( ": write-named-file ", default_filename, fn );
    }
    else
    {
        file_table.get_esc_word_mlisp( fn );
    }

    return write_named_file( fn );
}

int write_named_file( EmacsString &fn )
{
    if( fn.isNull()
    && bf_cur->b_fname.isNull() )
    {
        error( null_file_spec );
        return 0;
    }

    // Write the file, update the
    if( write_this( fn ) )
    {
        bf_cur->b_kind = FILEBUFFER;

        if( !callProc( buffer_saved_as_proc, bf_cur->b_buf_name ) )
        {
            bf_cur->rename( makeBufferName( bf_cur->b_fname, bf_cur ) );
        }
    }

    cant_1win_opt = 1;

    return 0;
}   // Of WriteNamedFile

// Appends to a file
int append_to_file( void )
{
    EmacsFileTable file_table;
    EmacsString fn;
    getescword( file_table., ": append-to-file ", fn );

    if( fn.isNull() )
    {
        error( null_file_spec );
    }
    else
    {
        bf_cur->write_file( fn, EmacsBuffer::APPEND_WRITE );
    }

    return 0;
}

EmacsString makeBufferName( EmacsString &fullname, EmacsBuffer *existing_buffer )
{
    EmacsString bufname;

    bool use_builtin_rule = true;

    //BoundName *proc = BoundName::find( "create-buffer-name-from-filename" );
    if( callProc( buffer_choose_name_proc, fullname ) )
    {
        //
        // this was coded to use a try catch block to get the string
        // but GNU C++ cannot compile the code
        //
        if( !ml_err && ml_value.exp_type() == ISSTRING )
        {
            bufname = ml_value.asString();
            use_builtin_rule = false;
        }
        else
        {
            use_builtin_rule = true;
        }
   }

    if( use_builtin_rule )
    {
        int last_part = fullname.last( PATH_CH );
        if( last_part > 0 )
        {
            bufname = fullname( last_part+1, INT_MAX );
        }
        else
        {
            bufname = fullname;
        }

        if( bufname == "." || bufname.isNull() )
        {
            bufname = u_str ("no-name");
        }
        else
        {
#ifdef VERS_CH
            // if the file name has a version number at the end strip it off
            int index = bufname.first( VERS_CH );
            if( index > 0 )
                bufname.remove( index );
#endif
        }
    }

    //
    // Now, check to see if the buffer is in use, and perform
    // appropriate actions. Allow buffer's name to set to its current
    // value.
    //
    EmacsBuffer *named_buffer = EmacsBuffer::find( bufname );
    if( named_buffer != NULL && named_buffer != existing_buffer )
    {
        if( interactive()
        && ask_about_buffer_names != 0 )
        {
            EmacsString p;
            if( cur_exec == NULL )
            {
                p = get_string_interactive(
                    FormatString("Buffer name %s is in use, type a new name or <Enter> to reuse: ")
                                    << bufname );
            }
            else
            {
                p = get_string_mlisp();
            }
            if( !p.isNull() )
            {
                bufname = p;
            }
        }
        else
        {
            //
            // The buffer exists, but ask-about-buffer-names is
            // false. So, we have to construct a buffer name. We do
            // this by taging <n> onto the end of the existing
            // buffer name, where n is a number. Then try and
            // find the resulting buffer. Do this until a buffer
            // is not found
            //
            int seq = 1;
            EmacsString synthname;

            do
            {
                synthname = FormatString("%s<%d>") << bufname << seq;
                seq++;
            }
            while( EmacsBuffer::find( synthname ) != NULL );

            bufname = synthname;
        }
    }

    return bufname;
}

// Visit a file - all the internal logic for this wonderous function

int visit_file( const EmacsString &fn, int createnew, int windowfiddle, const EmacsString &dn )
{
    int r_stat;
    EmacsString fullname;
    EmacsBufferRef oldb( bf_cur );

    // If no filename is supplied, just return
    if( fn.isNull() )
    {
        return 0;
    }

    expand_and_default( fn, dn, fullname );

    if( file_is_directory( fullname ) )
    {
        error( FormatString("visit-file cannot open directory %s") << fullname );
        return 0;
    }

    EmacsBuffer *b = buffers;
    while( b != NULL && file_name_compare->compare( b->b_fname, fullname ) != 0 )
    {
        b = b->b_next;
    }

    if( b != NULL )
    {
        b->set_bf();

        if( windowfiddle )
        {
            theActiveView->window_on( bf_cur );
        }

        return 1;
    }

    //
    //    Check the limits
    //
    if( file_read_veto( fullname ) )
    {
        return 1;
    }

    EmacsString bufname = makeBufferName( fullname, NULL );

    if( bufname.isNull() )
    {
        return 0;
    }

    // Create the buffer, and free up the old filename
    EmacsBuffer::set_bfn( bufname );

    bf_cur->b_fname = EmacsString::null;

    // Read in the file
    r_stat = bf_cur->read_file( fullname, 1, createnew );

    //
    // When the read is interrupted, leave what has already been
    // read in the appropriate buffer. Then make it a scratch buffer
    // and causes it to not be popped up onto the screen
    //
    if( interrupt_key_struck != 0 )
    {
        oldb.set_bf();
        return 0;
    }

    //
    // check to see if a failure has occurred during the read. Process it
    // as required by the various options
    //
    if( r_stat == 0 )
    {
        if( createnew == 0 )
        {
            oldb.set_bf();
            return 0;
        }
    }

    //
    // Fill in the rest of the buffer information
    //
    bf_cur->b_kind = FILEBUFFER;
    bf_cur->b_checkpointfn = EmacsString::null;
    bf_cur->b_checkpointed = checkpoint_frequency ? 0 : -1;

    //
    // Put the newly created buffer in the window ring, and make
    // sure that any auto-executes get executed
    //
    if( windowfiddle )
    {
        theActiveView->window_on (bf_cur);
    }

    if( !callProc( buffer_file_loaded_proc, bf_cur->b_buf_name ) )
    {
        do_auto( fullname );
    }

    return 1;
}                // Of visit_file


// Read a file into a buffer
int EmacsBuffer::read_file( const EmacsString &fn, int erase, int createnew )
{
    EmacsString fullname;

    if( fn.isNull() )
    {
        error( null_file_spec );
        return 0;
    }

    if( file_is_directory( fn ) )
    {
        error( FormatString("read-file cannot open directory %s") << fn );
        return 0;
    }

    // allow * and % to be expanded
    expand_and_default( fn, EmacsString::null, fullname );

    EmacsFile fd;

    // Open the file if possible
    if( !fd.fio_open( fullname, 0, EmacsString::null ) )
    {
        int saved_errno = errno;

        if( erase )
        {
            b_file_access = 0;
            b_file_time = 0;
            b_synch_file_access = 0;
            b_synch_file_time = 0;
        }

        // If creating a new file, do not report FNF error
        if( createnew != 0
        && errno == ENOENT )
        {
            cant_1line_opt = 1;
            redo_modes = 1;
            if( erase )
            {
                // Remove any buffer restriction
                b_mode.md_headclip = 1;
                b_mode.md_tailclip = 0;

                // erase the contents
                erase_bf();
            }

            error( FormatString("New file: %s") << fn );

            b_fname = fullname;
            b_kind = FILEBUFFER;
        }
        else
        {
            error( FormatString(perror_str) << fetch_os_error(errno) << fn );
        }

        errno = saved_errno;
        return 0;
    }

    //
    // Read in the file into an appropriate environment
    //
    cant_1line_opt = 1;
    redo_modes = 1;

    // Erase the buffer if requested
    if( erase )
    {
        // Remove any buffer restriction
        b_mode.md_headclip = 1;
        b_mode.md_tailclip = 0;

        // turn off read only and then erase
        b_mode.md_readonly = false;

        // erase the contents
        erase_bf();
    }

    //
    // Set the gap to the correct place, and make sure there is
    // enough room in it to contain the files contents
    //
    gap_to( dot );

    int fsize = int( fd.fio_size() ) + 2000;
    if( gap_room( fsize ) != 0 )
    {
        fd.fio_close();
        error( FormatString("No room for file %s") << fn);
        return 0;
    }

    // Get the real filename
    EmacsString fnam( fd.fio_getname() );
    if( erase )
    {
        b_synch_file_time = b_file_time = fd.fio_modify_date();
        b_synch_file_access = b_file_access = EmacsFile::fio_access( fnam );
        b_mode.md_readonly = b_file_access < 0;
    }
    //
    // Read in the files contents
    //
    int n = 0;
    int i = 0;

    if( fsize > 0 )
    {
        while( ! ml_err
        && (i = fd.fio_get( b_base + dot - 1 + n, fsize - n)) > 0 )
        {
            n += i;
            if( n > fsize - 1000 )
            {
                fsize += 2000;
                gap_room( fsize );
            }
        }
    }

    if( erase )
    {
        b_eol_attribute = fd.fio_get_eol_attribute();
    }

    // Close  the file, and adjust the pointers
    fd.fio_close();
    if( n > 0 )
    {
        // first record an insert before changing the buffer size
        if( !erase )
        {
            record_insert( dot, n, b_base + b_size1 );
        }
        b_size1 += + n;
        b_gap -= n;

        // notify the syntax code of an insert after the buffer size is changed
        if( !erase )
        {
            syntax_insert_update( dot, n );
        }
    }

    // Generate appropriate messages
    if( n == 0
    && ! ml_err )
    {
        message( "Empty file." );
    }

    if( i == EOF )
    {
        error( FormatString(perror_str) << fetch_os_error(errno) << fnam );
    }

    if( erase )
    {
        b_checkpointfn = EmacsString::null;
        if( b_checkpointed > 0 )
        {
            b_checkpointed = 0;
        }

        if( interrupt_key_struck )
        {
            b_kind = SCRATCHBUFFER;
            b_fname = EmacsString::null;
        }
        else
        {
            b_fname = fnam;
            b_kind = FILEBUFFER;
        }
    }

    // return a success indication as required from the read
    return i >= EOF && ! ml_err;
}


// Concoct a checkpoint filename
static EmacsString concoct_name( const EmacsString &fn, const EmacsString &extension )
{
    EmacsString result;

    expand_and_default( extension, fn, result );

    EmacsFile fd;
    // Create the file to check that it is valid, and get its real name
    if( !fd.fio_create( result, 0, FIO_STD, EmacsString::null, (FIO_EOL_Attribute)(int)default_end_of_line_style )
    && !fd.fio_create( defname, 0, FIO_STD, EmacsString::null, (FIO_EOL_Attribute)(int)default_end_of_line_style ) )
    {
        return defname;
    }

    result = fd.fio_getname();
    fd.fio_close();

    return result;
}   // Of concoct_name


//
// Write the current buffer to the named file; returns true iff
// successful. Appends to the file if AppendIt is append_write, does a checkpoint
// style write if AppendIt is checkpoint_write.
//
int EmacsBuffer::write_file( const EmacsString &fn, EmacsBuffer::WriteFileOperation_t appendit )
{
    int nc = unrestrictedSize();

    wrote_file = EmacsString::null;

    if( fn.isNull() )
        return 0;

    if( b_eol_attribute == FIO_EOL__None )
    {
        b_eol_attribute = default_end_of_line_style;
    }

    FIO_EOL_Attribute write_eol_attribute( b_eol_attribute );
    if( end_of_line_style_override != FIO_EOL__None )
    {
        write_eol_attribute = end_of_line_style_override;
    }

    EmacsFile fd;
    // Open the file, and position to the correct place
    switch( appendit )
    {
    case APPEND_WRITE:
        //
        // Open an existing file for appending, or if there is none,
        // create a new file
        //
        if( !fd.fio_open( fn, 1, EmacsString::null, write_eol_attribute ) )
        {
            fd.fio_create( fn, nc, FIO_STD, EmacsString::null, write_eol_attribute );
        }
        break;

    case CHECKPOINT_WRITE:
        fd.fio_create( fn, nc, FIO_CKP, EmacsString::null, write_eol_attribute );
        break;

    case ORDINARY_WRITE:
        fd.fio_create( fn, nc, FIO_STD, EmacsString::null, write_eol_attribute );
        b_eol_attribute = write_eol_attribute;
        break;
    }

    // Write out the contents of the buffer if the file was created
    if( !fd.fio_is_open() )
    {
        error( FormatString(perror_str) << fetch_os_error(errno) << fn);
        return 0;
    }

    //  Save the filespec
    wrote_file = fd.fio_getname();

#ifdef vms
{
    int n;
    unsigned char *p;
    unsigned char *q;
    unsigned char *p2;
    unsigned char *q2;
    unsigned char *start;
    unsigned char *endp;

    //  Copy the p1 region first
    p = start = &bf_p1[1];
    endp = &start[bf_s1];

    while( p < endp
    && ! ml_err )
    {
        //  Locate a single record as terminated with an LF
        q = p;
        while( q < endp
        && *q != '\n' )
            q++;

        // Skip out if the loop crosses the gap
        if( q >= endp )
            break;

        // Put the record, and check_ for any failure
        if( (n = fio_put (fd, p, q - p)) < 0 )
        {
            error( FormatString(perror_str) << fetch_os_error(errno) << fn);
            fio_close (fd);
            return 0;
        }

        // Update the pointers as required
        p = &p[n];
        if( *p == '\n' )
            p++;
    }

    // Deal with any cross gap records
    p2 = &bf_p1[1 + bf_s1 + bf_gap];
    if( ! ml_err
    && p < endp )
    {
        unsigned char *full;

        // Find the end of the split record
        q2 = p2;
        while( q2 < &p2[bf_s2]
        && q2[0] != '\n' )
            q2 = &q2[1];

        //  Try to allocate a buffer for the split record
        if( (full = malloc_ustr (q - p + q2 - p2)) == 0 )
        {
            // No memory, move the gap to the start of the nxt record
            gap_to (p - start);
            p2 = &bf_p1 [1 + bf_s1 + bf_gap];
        }
        else
        {
            // Emit the split record
            fio_split_put (fd, p, q - p, p2, q2 - p2, full);
            p2 = q2;
            if( p2[0] == '\n' )
                p2 = &p2[1];
            free (full);
        }
    }

    // Emit the p2 region
    endp = &bf_p1[1 + bf_s1 + bf_gap + bf_s2];
    while( p2 < endp
    && ! ml_err )
    {
        // find the end of record boundary
        q2 = p2;
        while( q2 < endp
        && q2[0] != '\n' )
            q2 = &q2[1];

        // Emit the record, and check_ for errors
        if( (n = fio_put (fd, p2, q2 - p2)) < 0 )
        {
            error (perror_str, fetch_os_error (errno), fn);
            fio_close (fd);
            return 0;
        }

        // Update the various pointers
        p2 = &p2[n];
        if( p2[0] == '\n' )
            p2 = &p2[1];
    }
}
#else
    if( b_size1 > 0
    && fd.fio_put( b_base, b_size1 ) < 0 )
    {
        error( FormatString(perror_str) << fetch_os_error(errno) << fn);
        fd.fio_close();
        return 0;
    }

    if( b_size2 > 0
    && fd.fio_put( b_base + b_size1 + b_gap, b_size2 ) < 0 )
    {
        error( FormatString(perror_str) << fetch_os_error(errno) << fn);
        fd.fio_close();
        return 0;
    }
#endif

    fd.fio_close();

    // Update the modified flag and checkpointing information
    if( ! ml_err )
    {
        // if this is an ordinary write then we need to
        // figure out the new modify time of the file
        //
        if( appendit == ORDINARY_WRITE )
        {
            b_modified = 0;
            b_synch_file_time = b_file_time = EmacsFile::fio_file_modify_date( wrote_file );
            b_synch_file_access = 1; // writable
        }

        if( b_checkpointed > 0 )
        {
            b_checkpointed = 0;
        }
        if( appendit != CHECKPOINT_WRITE )
        {
            message( FormatString("Wrote %s") << wrote_file );
        }
    }

    return 1;
}                // write_file


// fio_open_using_path opens the file fn with the given IO mode using the given
// search path. The actual file name is returned in fnb. It is used to
// to read in MLisp files via the executed-mlisp-file function.
bool EmacsFile::fio_open_using_path
    (
    const EmacsString &path,
    const EmacsString &fn,
    int append,
    const EmacsString &ex,
    FIO_EOL_Attribute _attr
    )
{
    //
    // check for Node, device or directory specs.
    // also allows any logical name through
    // If present then just open the file stright.
    //
    if( fn.first( PATH_CH ) >= 0
    || fn.first( ':' ) >= 0 )
    {
        return fio_open( fn, append, ex, _attr );
    }

    //
    // Otherwise, add the path onto the front of the filespec
    //
    int start = 0;
    int end = 0;

    while( start < path.length() )
    {
        end = path.index( PATH_SEP, start );
        if( end < 0 )
        {
            end = path.length();
        }

        EmacsString fnb( path( start, end ) );
        if(fnb[-1] != PATH_CH
        && fnb[-1] != ':' )
        {
            fnb.append( PATH_STR );
        }
        fnb.append( fn );

        fio_open( fnb, append, ex );
        if( fio_is_open() )
        {
            return true;
        }

        start = end;
        start++;
    }

    return false;
}   // Of fio_open_using_path


// Returns true if modified buffers exist
bool mod_exist( void )
{
    if( bf_cur == NULL )
    {
        return 0;
    }

    bf_cur->set_bf();
    EmacsBuffer *b = buffers;
    while( b != 0)
    {
        if( b->b_modified != 0
        &&  b->b_kind == FILEBUFFER )
        {
            return true;
        }
        else
        {
            b = b->b_next;
        }
    }

    return false;
}   // Of mod_exist


// Write all modified buffers; return true iff OK

bool mod_write( void )
{
    EmacsBufferRef old( bf_cur );
    int write_errors = 0;

    // force the global buffer data into the emacs_buffer structure
    old.set_bf();

    EmacsBuffer *b = buffers;
    while( !ml_err && b != NULL )
    {
        b->set_bf();
        if( b->b_kind == FILEBUFFER
        && b->b_modified != 0 )
        {
            b->set_bf();
            if( write_this( EmacsString::null ) == 0 )
            {
                if( interactive()
                || get_yes_or_no
                        ( 0,
                        FormatString("Cannot write buffer %s, can I ignore it? ") << b->b_buf_name
                        ) == 0 )
                {
                    write_errors++;
                }
            }
        }
        b = b->b_next;
    }

    old.set_bf();

    return !ml_err && write_errors == 0;
}   // Of mod_write


// Deletes all checkpoint file
void kill_checkpoint_files( void )
{
    if( unlink_checkpoint_files != 0 )
    {
        EmacsBuffer *b = buffers;
        while( b != NULL )
        {
            if( b->b_checkpointfn.length() > 0 )
            {
                EmacsFile::fio_delete( b->b_checkpointfn );
                b->b_checkpointfn = EmacsString::null;
            }

            b = b->b_next;
        }
    }
}

// Function that initiate checkpointing
int checkpoint_everything(void)
{
    EmacsChar_t saved_activity_char = activity_character;
    int rv = 0;

    write_errors = 0;

    if( checkpoint_proc != 0 )
    {
        if( interactive() )
        {
            if( activity_indicator != 0 && term_ansi != 0 )
            {
                set_activity_character ('c');
            }
            else
            {
                message ("Checkpointing...");
            }

            theActiveView->do_dsp();
        }

        rv = execute_bound_saved_environment(checkpoint_proc);

        if( interactive() )
        {
            if( write_errors == 0 || interrupt_key_struck != 0 )
            {
                if( interrupt_key_struck != 0 )
                {
                    message( "Checkpointing... interrupted." );
                }
                else
                {
                    if( activity_indicator != 0 && term_ansi != 0 )
                    {
                        set_activity_character( saved_activity_char );
                    }
                    else
                    {
                        message( "Checkpointing... done." );
                    }
                }
            }
            else
            {
                error( "Checkpointing... failed.");
            }

            theActiveView->do_dsp();
        }
    }

    return rv;
}


// Function interface to the checkpoint-buffers default checkpoint action
int checkpoint_buffers(void)
{
    EmacsBufferRef old( bf_cur );

    write_errors = 0;

    EmacsBuffer *b = buffers;
    while( b != 0 && interrupt_key_struck == 0)
    {
        int modcnt = b->b_modified;

        if( b->b_checkpointed >= 0
        && b->b_checkpointed < modcnt )
        {
            b->set_bf();
            if( b->b_checkpointfn.isNull() )
                b->b_checkpointfn =
                    concoct_name(
                        b->b_fname.length() > 0 ? b->b_fname : b->b_buf_name,
                        ckp_ext);

            write_errors |= b->write_file( b->b_checkpointfn, EmacsBuffer::CHECKPOINT_WRITE ) == 0;

            ml_err = 0;
            b->b_modified = modcnt;
            b->b_checkpointed = modcnt;
        }

        b = b->b_next;
    }

    old.set_bf();
    return 0;
}


// fetch_ the text of a VAX/VMS error code
#ifdef vms
EmacsString fetch_os_error( int error_code )
{
    struct dsc$descriptor bufd;
    unsigned short int len = MAX_ERROR_MESSAGE;

    if( os_error_buf == 0 )
        if( (os_error_buf = malloc_ustr (MAX_ERROR_MESSAGE)) == 0 )
            return u_str("error detected");

    bufd.dsc$w_length = MAX_ERROR_MESSAGE;
    bufd.dsc$b_dtype = 0;
    bufd.dsc$b_class = 0;
    bufd.dsc$a_pointer = (char *)os_error_buf;

    if( ! VMS_SUCCESS(sys$getmsg( (unsigned int)error_code, &len, &bufd, 1, 0)) )
    {
        sprintfl (os_error_buf, MAX_ERROR_MESSAGE,
            u_str("error %%X%08X detected"), error_code);
        len = _str_len (os_error_buf);
    }

    os_error_buf[len] = 0;

    if( os_error_buf[0] >= 'a' && os_error_buf[0] <= 'z' )
        os_error_buf[0] = os_error_buf[0] + 'A' - 'a';

    return os_error_buf;
}
#else
EmacsString fetch_os_error ( int error_code )
{
    char *error_str = strerror( error_code );
    if( error_str != NULL )
    {
        return error_str;
    }
    else
    {
        return FormatString("Unknown error %d detected") << error_code;
    }
}
#endif

int synchronise_files(void)
{
    int synched_buffers_deleted( 0 );
    int synched_buffers_reloaded( 0 );
    EmacsBufferRef old( bf_cur );

    // force the global buffer data into the emacs_buffer structure
    old.set_bf();

    EmacsBuffer *b = buffers;
    while( !ml_err && b != NULL )
    {
        b->set_bf();
        if( b->b_kind == FILEBUFFER
        && b->b_fname.length() > 0 )
            {
            EmacsString fname( b->b_fname );

#if defined( vms )
            // for VMS use the  latest version of the file
            int index = fname.last( ';' );
            // strip the version
            if( index >= 0 )
            {
                fname.remove( index );
            }
#endif

            time_t new_time = EmacsFile::fio_file_modify_date( fname );
            int new_access = EmacsFile::fio_access( fname );

            // if has been deleted and used to exist
            if( new_access == 0 && new_access != b->b_synch_file_access )
            {
                int delete_it = 1;

                if( b->b_modified != 0 )
                {
                    delete_it = get_yes_or_no
                        ( 0,
                        FormatString("The file %s has been delete do you want to delete modified buffer %s?") <<
                            fname << b->b_buf_name
                        );
                }
                else
                {
                    if( ask_about_synchronise_for_none_modified_buffers )
                    {
                        delete_it = get_yes_or_no
                            ( 1,
                            FormatString("The file %s has been delete do you want to delete buffer %s?") <<
                                fname << b->b_buf_name
                            );
                    }
                }

                if( delete_it )
                {
                    synched_buffers_deleted++;

                    message(FormatString("Deleting buffer synchronising %s...") << b->b_buf_name );
                    if( theActiveView != NULL && theActiveView->currentWindow() != NULL )
                    {
                        theActiveView->do_dsp();    // Make the screen correct
                    }

                    // need to save the name as it will be free'ed in kill_bfn
                    EmacsString name( b->b_buf_name );

                    EmacsBuffer *to_kill = b;
                    // find the next before we kill this buffer
                    b = b->b_next;

                    // kill off the buffer
                    delete to_kill;

                    // on to the next buffer
                    continue;
                }
                else
                {
                    // update the synch attribute variable
                    b->b_synch_file_access = new_access;
                }
            }

            // see if the file attributes have changed since the last synch
            if( b->b_synch_file_time != new_time )
            {
                int read_it = 1;

                if( b->b_modified != 0 )
                {
                    read_it = get_yes_or_no
                        ( 0,
                        FormatString("For modified buffer %s the file %s has changed do you want to reload it?") <<
                            b->b_buf_name << fname
                        );
                }
                else
                {
                    if( ask_about_synchronise_for_none_modified_buffers )
                    {
                        read_it = get_yes_or_no
                            ( 1,
                            FormatString("The file %s has changed do you want to reload it?") <<
                                fname
                            );
                    }
                }

                if( read_it )
                {
                    synched_buffers_reloaded++;

                    int old_dot = dot;

                    // allow the buffer to be over written
                    b->b_mode.md_readonly = 0;

                    message(FormatString("Reading buffer to synchronise %s...") << b->b_buf_name );
                    if( theActiveView != NULL && theActiveView->currentWindow() != NULL )
                    {
                        theActiveView->do_dsp();    // Make the screen correct
                    }

                    // the do_dsp may change buffers under us
                    b->set_bf();
                    b->read_file( fname, 1, 0 );

                    set_dot( old_dot );
                }
                else
                {
                    // update the synch attribute variables
                    b->b_synch_file_time = new_time;
                    b->b_synch_file_access = new_access;
                }
            }
            else if( b->b_synch_file_access != new_access )
            {
                b->b_synch_file_access = new_access;
                b->b_file_access = new_access;
                b->b_mode.md_readonly = new_access < 0;
            }
        }

    b = b->b_next;
    }

    // carefully return to the old buffer
    if( old.bufferValid() )
    {
        // o.k. its still around use it
        old.set_bf();
    }
    else
    {
        // default to main to prevent the posibilty of ending up in the Minibuffer
        EmacsBuffer::set_bfn( "main" );
    }

    if( synched_buffers_reloaded > 0 || synched_buffers_deleted > 0 )
    {
        EmacsString message_text( "synchonise-files " );

        switch( synched_buffers_deleted )
        {
        case 0:
            break;
        case 1:
            message_text.append( "deleted 1 buffer" );
            break;
        default:
            message_text.append( FormatString("deleted %d buffers") << synched_buffers_deleted );
            break;
        }

        if( synched_buffers_reloaded > 0 && synched_buffers_deleted > 0 )
        {
            message_text.append( " and " );
        }

        switch( synched_buffers_reloaded )
        {
        case 0:
            break;
        case 1:
            message_text.append( "reloaded 1 buffer" );
            break;
        default:
            message_text.append( FormatString("reloaded %d buffers") << synched_buffers_reloaded );
            break;
        }

        message( message_text );
    }

    if( theActiveView != NULL && theActiveView->currentWindow() != NULL )
    {
        theActiveView->do_dsp();
    }

    return 0;
}

bool file_read_veto( const EmacsString &filename )
{
    // only bother if there is a size limit
    if( maximum_file_read_size.asInt() == 0 )
    {
        return false;
    }

    EmacsFile fd;

    // Open the file if possible
    if( !fd.fio_open( filename, 0, EmacsString::null ) )
    {
        return false;
    }

    //
    //    Check the size is within the read limit
    //
    int file_size = int( fd.fio_size() );
    if( file_size > maximum_file_read_size.asInt() )
    {
        error( FormatString( "maximum file size %d exceeded. %d bytes in %s" )
            << maximum_file_read_size << file_size << filename );
        return true;
    }

    return false;
}
