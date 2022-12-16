//
//     Copyright(c) 1982-2017
//        Barry A. Scott

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

SystemExpressionRepresentationString compile_command;
SystemExpressionRepresentationString debug_command;
SystemExpressionRepresentationString filter_command;
SystemExpressionRepresentationString execute_command;
SystemExpressionRepresentationString cli_name;

int saved_buffer_count;
EmacsBuffer *saved_buffers[SAVED_BUFFER_MAX];

BoundName *enter_emacs_proc;
BoundName *exit_emacs_proc;
BoundName *leave_emacs_proc;
BoundName *return_to_emacs_proc;

extern void filter_through( int n, const EmacsString &command );

#if DBG_PROCESS && DBG_TMP
extern int elapse_time(void);
# define Trace( s )  do { \
                        if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP ) { \
                            int t=elapse_time(); \
                            _dbg_msg( FormatString("%d.%03.3d " "%s") << t/1000 << t%1000 << (s) ); } \
                    } while( false )
#else
# define Trace( s ) // do nothing
#endif

#if !defined( EXEC_BF )
int indent_c_procedure( void )
{
    return no_value_command();
}

int compile_it( void )
{
    return no_value_command();
}

int execute_monitor_command( void )
{
    return no_value_command();
}

int return_to_monitor( void )
{
    return no_value_command();
}

int pause_emacs( void )
{
    return 0;
}

void filter_through( int n, const EmacsString &command )
{
}
#endif


#if defined( EXEC_BF )
static int tmp_name_count = 1;

# ifdef __unix__
#  include <unistd.h>
# endif

#if defined( WIN32 )
#include <process.h>
#endif

# if defined( __unix__ ) || defined( WIN32 )
EmacsString emacs_tmpnam()
{
    char *tmp_dir = getenv("TMPDIR");
    if( tmp_dir == NULL )
        tmp_dir = getenv("TEMP");
    if( tmp_dir == NULL )
        tmp_dir = getenv("TMP");

    EmacsString tmp;
    if( tmp_dir == NULL || !EmacsFile( tmp_dir ).fio_file_exists() )
        tmp = current_directory.asString();
    else
        tmp.append( tmp_dir );

    if( tmp[-1] != PATH_CH )
        tmp.append( PATH_CH );

#if defined( WIN32 )
    unsigned int pid = _getpid();
#else
    unsigned int pid = getpid();
#endif
    for( int attempt=0; attempt < 10; attempt++ )
    {
        EmacsString result = FormatString("%semacs_%x_%d.tmp")
            << tmp << pid << tmp_name_count++;
        if( !EmacsFile( result ).fio_file_exists() )
            return result;
    }

    return EmacsString::null;
}
# endif

# ifdef __unix__
#  include <emacs_signal.h>
#  ifndef _BSD
#   define _BSD
#   include <sys/wait.h>
#   undef _BSD
#  else
#   include <sys/wait.h>
#  endif
#  include <ctype.h>
#  include <unistd.h>
#  include <fcntl.h>

const char *shell()
{   // return the name of the users shell
    static const char *sh = NULL;
    if( sh == NULL )
        sh = getenv( "SHELL" );
    if( sh == NULL )
        sh = "sh";
    return sh;
}
# endif

int indent_c_procedure( void );
int execute_monitor_command( void );
int return_to_monitor( void );
int pause_emacs( void );
int filter_region( void );
void filter_through(int n, const EmacsString &command );


unsigned int parent_pid;

#if defined( EXEC_BF )
extern void exec_bf( const EmacsString &bufname, int display, const EmacsString &input, int erase, const char *command, ... );
#endif

# if defined( __unix__ )
int pause_emacs( void )
{
#  if 0
    //
    // See if the user wants to specify a command, fetch it, and send
    // it down the restart mailbox
    //
    EmacsString pause_command;

    if( arg_state == have_arg
    || (! interactive() && cur_exec != 0 && cur_exec->p_nargs > 0) )
    {
        pause_command = getstr( ": pause-emacs " );
    }
    else
    {
        send_exit_message( EmacsString::null );
    }

    if( leave_emacs_proc != NULL )
    {
        execute_bound_saved_environment( leave_emacs_proc );
    }

    if( !send_exit_message( pause_command ) )
    {
        error("Unable to send response to bemacs client");
    }
#  endif

    return 0;
}
# endif

int filter_region( void )
{
    if( !bf_cur->b_mark.isSet() )
    {
        error( "Mark not set" );
        return 0;
    }
    EmacsString s = getstr( ": filter-region (through command) " );
    if( !s.isNull() )    // failed to read string
    {
        if( !s.isNull() )    // empty string
            filter_command = s;
        filter_through( bf_cur->b_mark.to_mark() - dot, filter_command.asString() );
    }
    return 0;
}

// pass the region starting at dot and extending for n characters through
// the command. The old contents of the region is left in the kill
// buffer
void filter_through( int n, const EmacsString &command )
{
    EmacsFile tempfile( emacs_tmpnam() );

    EmacsBufferRef old( bf_cur );
    replace_to_buf( n, "Kill buffer" );

    EmacsBuffer::set_bfn( "Kill buffer" );
    bf_cur->write_file( tempfile, EmacsBuffer::CHECKPOINT_WRITE );
    old.set_bf();

# if defined( vms )
    exec_bf( bf_cur->b_buf_name, 0, tempfile.result_spec, 0, "notused", command.data(), NULL );
# endif
# if defined( __unix__ )
    exec_bf( bf_cur->b_buf_name, 0, tempfile.result_spec, 0, shell(), "-c", command.utf8_data(), NULL );
# endif
# if defined( WIN32 )
    exec_bf( bf_cur->b_buf_name, 0, tempfile.result_spec, 0, "notused", command.utf8_data(), NULL );
# endif

    if( bf_cur->b_modified == 0 )
    {
        redo_modes = cant_1line_opt = 1;
    }
    bf_cur->b_modified++;

    tempfile.fio_delete();
}

# if defined( __unix__ )

// Copy stuff from indicated file descriptor into the current
// buffer; return the number of characters read.  This routine is
// useful when reading from pipes and such.
static void readPipe( int fd, int display )
{
    const int buf_size = 16 * 1024;

    if( display )
    {
        message("Starting up...");
        theActiveView->do_dsp();
    }

    unsigned char utf8_buf[ buf_size ];
    EmacsChar_t unicode_buf[ buf_size ];

    int utf8_buf_used = 0;
    for(;;)
    {
        Trace( FormatString("readPipe utf8_buf_used %d available %d") << utf8_buf_used << buf_size - utf8_buf_used );
        int n = read( fd, &utf8_buf[utf8_buf_used], buf_size - utf8_buf_used );
        Trace( FormatString("readPipe read() -> %d") << n );
        if( n == 0 )
        {
            Trace( "readPipe end-of-file reached" );
            break;
        }
        if( n <= 0 )
        {
            Trace( FormatString("readPipe read() -> errno %e") << errno );
            break;
        }

        utf8_buf_used += n;

        // find out the usable bytes in buf
        int utf8_usable_length = 0;
        int unicode_length = length_utf8_to_unicode(
                    utf8_buf_used, utf8_buf,
                    buf_size,
                    utf8_usable_length );
        Trace( FormatString("readPipe length_utf8_to_unicode() utf8_usable_length %d unicode_length %d") << utf8_usable_length << unicode_length );

        // convert to unicode
        convert_utf8_to_unicode( utf8_buf, unicode_length, unicode_buf );

        // insert unicode
        bf_cur->ins_cstr( unicode_buf, unicode_length );

        // move left over bytes to the start of the buffer
        if( utf8_usable_length > 0 )
        {
            memmove( &utf8_buf[0], &utf8_buf[utf8_usable_length], utf8_buf_used - utf8_usable_length );
            utf8_buf_used -= utf8_usable_length;
        }

        if( display )
        {
            message("Chugging along...");
            theActiveView->do_dsp();
        }
    }

    Trace( FormatString("readPipe() utf8_buf_used %d at exit") << utf8_buf_used );

    if( display )
    {
        message("Done!");
    }
}

// execute a subprocess with the output being stuffed into the named
// buffer. exec_bf is called with the command as a list of strings as
// seperate arguments
void exec_bf
    (
    const EmacsString &buffer,
    int display,
    const EmacsString &input,
    int erase,
    const char *command,
    ...
    )
{
    Trace( FormatString("exec_bf( %s, %d, \"%.20s\", %d, %s )") << buffer << display << input << erase << command );
    EmacsBufferRef old( bf_cur );
    int fd[2];
    const char *args[100];
    int arg;
    va_list argp;

    //
    //    Need to copy the args off of the stack
    //    as there is no reason for the stack order to
    //    match a char ** order. HP PA-Risc exposed this
    //    problem.
    //
    va_start( argp, command );

    args[0] = command;
    arg = 1;
    for(;;)
    {
        const char *arg_str = va_arg( argp, const char * );

        if( arg_str == NULL )
        {
            break;
        }

        args[arg++] = arg_str;
    }
    args[arg] = NULL;

    EmacsBuffer::set_bfn( buffer );
    if( interactive() )
    {
        theActiveView->window_on( bf_cur );
    }
    if( erase )
    {
        bf_cur->erase_bf();
    }
    pipe( fd );

    pid_t subproc_id = fork();
    Trace( FormatString("exec_bf() fork() => %d errno %e") << subproc_id << errno );
    if( subproc_id == 0 )
    {
        close( STDIN_FILENO );
        close( STDOUT_FILENO );
        close( STDERR_FILENO );

        if( open( input, 0 ) != 0 )
        {
            const char *msg = "Couldn't open input file\n";
            write( fd[1], msg, sizeof( msg ) );
            _exit( -1 );
        }

        dup( fd[1] );
        dup( fd[1] );
        close( fd[1] );
        close( fd[0] );
        execvp( command, (char **)args );
        const char *msg = "Couldn't execute the program!\n";
        write( fd[1], msg, sizeof( msg ) );
        _exit( -1 );
    }
    close( fd[1] );

    readPipe( fd[0], interactive() && display );
    close( fd[0] );

    int stat_loc = 0;
    pid_t rc = waitpid( subproc_id, &stat_loc, 0 );
    Trace( FormatString("exec_bf() waitpid() -> %d") << rc );

    if( interactive() && old.bufferValid() )
    {
        theActiveView->window_on( old.buffer() );
    }
    Trace( "exec_bf() done" );
}

int return_to_monitor( void )
{
    return no_value_command();
}
# endif

static EmacsString c_procedure_end("^}");
int indent_c_procedure( void )
{
    int pos = sea_glob.search( c_procedure_end, 1, dot - 3, EmacsSearch::sea_type__RE_simple );
    if( pos <= 0 )
    {
        error( "cannot find procedure boundary" );
        return 0;
    }

    int nest = 0;
    int spos = pos;

    pos = scan_bf_for_lf( pos, 1 );
    while( spos > 1 )
    {
        unsigned char c;
        c = bf_cur->char_at( spos );
        if( c == '}' )
            nest++;
        if( c == '{' )
        {
            nest--;
            if( nest <= 0 )
                break;
        }
        spos--;;
    }

    if( nest == 0 )
    {
        set_dot( scan_bf_for_lf( spos, -1) );
        filter_through( pos - dot, "indent -st" );
    }
    else
        error( "Cannot find procedure boundary" );

    return 0;
}

int execute_monitor_command( void )
{
    EmacsString com = getstr( "Command: " );
    if( com.isNull() )
    {
        return 0;
    }
    if( !com.isNull() )
    {
        execute_command = com;
    }

# if defined( vms )
    exec_bf( "command execution", 1, "NLA0:", 1,
                execute_command.data(), NULL );
# endif
# if defined( __unix__ )
    exec_bf( "command execution", 1, "/dev/null", 1,
                shell(), "-c", execute_command.utf8_data(), NULL );
# endif
# if defined( WIN32 )
    exec_bf( "Command execution", 1, "nul", 1, "unused",
                execute_command.utf8_data(), NULL );
# endif
    return 0;
}
#endif
