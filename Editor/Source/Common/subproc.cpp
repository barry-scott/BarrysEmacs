//     Copyright(c ) 1982-1998
//        Barry A. Scott

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

static int tmp_name_count = 1;

#if defined( _WINDOWS )
# include <win_incl.h>
//# include <emacs_process.h>
extern "C" { int _getpid(void); };

unsigned int getpid()
{
    return _getpid();
}

#endif

#ifdef __unix__
#include <unistd.h>
#endif

#ifdef vms
# include <jpidef.h>
# include <iodef.h>
# include <dvidef.h>

static char *emacs_tmpnam( char *buffer )
{
    int code = JPI$_PID;
    unsigned int pid = 0;
    lib$getjpi( &code, 0, 0, &pid, 0, 0 );

    sprintf( buffer, "sys$scratch:emacs_%x_%d.tmp", pid, tmp_name_count++ );
    return buffer;
}


#else

static EmacsString emacs_tmpnam()
{
    char *tmp_dir = getenv("TMP");
    if( tmp_dir == NULL )
        tmp_dir = getenv("TEMP");
    EmacsString tmp;
    if( tmp_dir == NULL )
        tmp = current_directory.asString();
    else
        tmp.append( tmp_dir );
    if( tmp[-1] != PATH_CH )
        tmp.append( PATH_CH );

    unsigned int pid = getpid();

    for( int attempt=0; attempt < 10; attempt++ )
    {
        EmacsString result = FormatString("%semacs_%x_%d.tmp")
            << tmp << pid << tmp_name_count++;
        if( !EmacsFile::fio_file_exists( result ) )
            return result;
    }

    return EmacsString::null;
}



#endif

#ifdef __unix__
# include <emacs_signal.h>
# ifdef SUBPROCESSES
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
char *shell(void);
# endif
#endif

int indent_c_procedure( void );
int execute_monitor_command( void );
int return_to_monitor( void );
int pause_emacs( void );
int filter_region( void );
void filter_through(int n, const EmacsString &command );

#ifdef vms
static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid);
#endif

#ifdef vms
static unsigned short int ichan;
static unsigned short int ochan;
static unsigned int subpid;

# if defined(SUBPROCESSES)
#  define    chk( a ) if( ! VMS_SUCCESS(a) ) return

unsigned char *two_percent = u_str( "%s: %s" );
unsigned char *log_in_out = u_str( "SYS$SYSTEM:LOGINOUT.EXE" );

static char sys_input_text[64];
static char sys_output_text[64];
static struct dsc$descriptor sys_input = {0,0,0,sys_input_text};
static struct dsc$descriptor sys_output = {0,0,0,sys_output_text};

static int terminated;

static int is_shell = 0;
static int is_dcl = 0;

static int spawn_flags = 1;

static unsigned int des_blk_cond;
static struct exit_handled_block
{
    int vms_use;
    int (*rtn)(unsigned int *,unsigned int *);
    int arg_count;
    unsigned int *cond_p;
    unsigned int *pid;
}
    des_blk =
{
    0,
    kill_prc,
    2,
    &des_blk_cond,
    &subpid
};
# endif
#endif

#ifdef __unix__
# ifdef SUBPROCESSES
pid_t subproc_id;
# endif
#endif

SystemExpressionRepresentationString compile_command;
SystemExpressionRepresentationString debug_command;
SystemExpressionRepresentationString filter_command;
SystemExpressionRepresentationString execute_command;
SystemExpressionRepresentationString cli_name;

#if defined( SUBPROCESSES )
# ifdef vms
static void exec_bf( const EmacsString &bufname, int display, const EmacsString &input, int erase, ... );
static void dcl_term(int return_from_monitor);
static int cmd( unsigned char *fmt, ... );
static int opt( unsigned char *buf );
static void set_attn(int chan, int func, int (*rtn)(int), int par);
static int mbx_ast(int must_read);
# endif
# ifdef __unix__
static void exec_bf( const EmacsString &bufname, int display, const EmacsString &input, int erase, char *command, ... );
# endif
#if defined( _NT )
static void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    ...
    );
#endif
#endif

int saved_buffer_count;
EmacsBuffer *saved_buffers[SAVED_BUFFER_MAX];

BoundName *enter_emacs_proc;
BoundName *exit_emacs_proc;
BoundName *leave_emacs_proc;
BoundName *return_to_emacs_proc;
#ifdef vms
static int dcl_count = 0;
static unsigned int sub_pid;
static int disp_flag;
static int insert_in_buffer;
#endif
unsigned int parent_pid;

#ifndef SUBPROCESSES
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
#endif

int pause_emacs( void )
{
#ifdef vms
    if( parent_pid == 0 )
    {
        int code = JPI$_OWNER;
        lib$getjpi( &code, 0, 0, &parent_pid, 0, 0 );
        if( parent_pid == 0 )
        {
            error( "There is no parent to which to pass control" );
            return 0;
        }
    }

    if( dcl_count != 0 )
    {
        kill_prc( 0, &sub_pid );
        sys$hiber();
    }
#endif


#if defined( __unix ) && defined( vms )
    //
    // See if the user wants to specify a command, fetch it, and send
    // it down the restart mailbox
    //
    if( arg_state == have_arg
    || (! interactive() && cur_exec != 0 && cur_exec->p_nargs > 0) )
    {
        EmacsString pause_command;
        pause_command = getstr( ": pause-emacs " );
    }
    else
        send_exit_message( EmacsString::null );
    if( leave_emacs_proc != NULL )
        execute_bound_saved_environment( leave_emacs_proc );

    if( !send_exit_message( pause_command ) )
        error("Unable to send response to bemacs client");
#endif
#ifdef vms
    rst_dsp();
    journal_pause();

    if( ! VMS_SUCCESS(lib$attach( &parent_pid )) )
        error( "Failed to attach to parent process" );
    complete_reinit = 1;
    init_dsp();
    start_async_io();
    journal_flush();
    fit_windows_to_screen_length();

    complete_reinit = 0;
    read_restart_message( 1 );
    ml_err = 0;
    if( return_to_emacs_proc != NULL )
        execute_bound_saved_environment( return_to_emacs_proc );
    if( ! touched_command_args )
        if( executepackage( gargv[0] ) == 0 )
            read_in_files( gargc, gargv );
#endif
    return 0;
}

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
void filter_through(int n, const EmacsString &command )
{
#ifdef SUBPROCESSES
    EmacsString tempfile( emacs_tmpnam() );
    if( tempfile.isNull() )
    {
        error( "Unable to create temporary file" );
        return;
    }

    EmacsBufferRef old( bf_cur );
    replace_to_buf( n, "Kill buffer" );

    EmacsBuffer::set_bfn( "Kill buffer" );
    bf_cur->write_file( tempfile, EmacsBuffer::CHECKPOINT_WRITE );
    old.set_bf();

# ifdef vms
    exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, command.data(), NULL );
# endif
# ifdef __unix__
    exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, shell (), "-c", command.data(), NULL );
# endif
# ifdef _NT
    exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, command.data(), NULL );
# endif

    if( bf_cur->b_modified == 0 )
    {
        redo_modes = cant_1line_opt = 1;
    }
    bf_cur->b_modified++;
    EmacsFile::fio_delete( tempfile );
#endif
}

#if defined( _NT)
static void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    ...
    )
{
    va_list argp;

    EmacsString output = emacs_tmpnam();
    if( output.isNull() )
    {
        error( "Unable to create temporary file" );
        return;
    }

    va_start( argp, erase );

    if( saved_buffer_count >= SAVED_BUFFER_MAX - 2 )
    {
        error( "not enough space to remember buffers" );
        return;
    }

    int disp_flag = display;

    saved_buffers[saved_buffer_count] = bf_cur;
    saved_buffer_count++;

    EmacsBuffer::set_bfn( bufname );

    if( interactive() )
        theActiveView->window_on( bf_cur );

    if( erase )
        bf_cur->erase_bf();

    if( disp_flag )
    {
        message( "Starting up ..." );
        theActiveView->do_dsp();
    }

    // use the prefered command interpreter
    EmacsString buf( getenv( "COMSPEC" ) );
    buf.append( " /c" );
    for(;;)
    {
        unsigned char *arg_str = va_arg( argp, unsigned char * );

        if( arg_str == NULL )
            break;

        buf.append( " " );
        buf.append( arg_str );
    }

    buf.append( " <" ); buf.append( input );
    buf.append( " >" ); buf.append( output );
    buf.append( " 2>&1" );


    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFO si;

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = NULL;
    si.lpTitle = NULL;
    si.lpDesktop = NULL;
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = si.dwFlags = 0L;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.lpReserved2 = NULL;
    si.cbReserved2 = 0;

    if( CreateProcess
        (
        NULL,
        (char *)buf.sdata(),
        NULL,
        NULL,
        FALSE, // Inherit handles
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &ProcessInformation
        ))
    {
        WaitForSingleObject( ProcessInformation.hThread, INFINITE );
        if( CloseHandle( ProcessInformation.hThread ) == FALSE )
            error( "Failed to close process handle in exec_bf" );
    }
    else
        error( FormatString("Failed to execute shell, error = %d\n") << GetLastError() );


    // insert the output file
    bf_cur->read_file( output, 0, 0 );

    EmacsFile::fio_delete( output );

    bf_cur->b_modified = 0;

    saved_buffer_count--;
    if( interactive() )
        theActiveView->window_on( saved_buffers[saved_buffer_count] );

    if( disp_flag )
        message( "Done." );
    return;
}
#endif

#ifdef vms
static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid)
{
    // don't want to kill ourselves!
    if( *subpid != 0 )
        sys$delprc( subpid, NULL );

    return 1;
}
#endif

#if defined( SUBPROCESSES ) && defined( vms )
static void dcl_term(int return_from_monitor)
{
    sys$dassgn( ichan );
    sys$dassgn( ochan );
    sys$canexh( (void *)&des_blk );
    sys$wake( NULL, NULL );
    dcl_count = 0;
    terminated = 1;
}

static int cmd(unsigned char *fmt, ... )
{
    va_list argp;
    int i;
    int status;
    unsigned char buf[BUFSIZ / 2];
    unsigned short int iosb[4];

    va_start( argp, fmt );

    i = do_print( fmt, &argp, buf, sizeof( buf ) );

    status = sys$qiow
        (
        EFN_SUB_PROC_SEND,
        ichan,
        IO$_WRITEVBLK,
        (void *)iosb,
        NULL, NULL,
        buf,
        i,
        0,0,0,0
        );
    if( ! VMS_SUCCESS(status) || ! VMS_SUCCESS(iosb[0]) )
    {
        error( "Failed to send command to sub-process" );
        return 0;
    }
    return 1;
}

static int opt( unsigned char *buf )
{
    int status;
    unsigned short int iosb[4];
    status = sys$qiow
        (
        EFN_SUB_PROC_REC,
        ochan,
        IO$_READVBLK,
        (void *)&iosb,
        NULL, NULL,
        buf,
        BUFSIZ,
        0,0,0,0
        );
    if( ! VMS_SUCCESS(status) || ! VMS_SUCCESS(iosb[0]) )
        return -1;

    buf[iosb[1]] = 0;

    return iosb[1];
}

static void set_attn(int chan, int func, int (*rtn)(int), int par)
{
    int status;
    unsigned short int iosb[4];
    status = sys$qiow
        (
        EFN_DO_NOT_WAIT_ON,
        chan,
        func,
        (void *)&iosb,
        NULL, NULL,
        rtn,
        par,
        0,0,0,0
        );
    if( VMS_SUCCESS(status) )
    {
        if( ! VMS_SUCCESS(iosb[0]) ) lib$signal( iosb[0] );
        return;
    }
    if( ! VMS_SUCCESS(status) ) lib$signal( status );
    return;
}

static int mbx_ast(int must_read)
{
    if( must_read )
    {
        unsigned char buf[BUFSIZ];
        int size;
        size = opt( buf );
        set_attn( ochan, IO$_SETMODE|IO$M_WRTATTN, mbx_ast, must_read );
        if( insert_in_buffer && size >= 0 )
        {
            buf[size] = '\n';
            bf_cur->ins_cstr( buf, size+1 );
            if( interactive() )
                do_dsp();
        }
    }
    else
    {
        terminated = 1;
        sys$wake( NULL, NULL );
    }
    return 1;
}

void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    ...
    )
{
    va_list argp;
    unsigned char buf[BUFSIZ / 2];
    int i;
    int code;

    va_start( argp, erase );

    is_shell = _str_icmp( cli_name, u_str("SHELL") ) == 0;
    is_dcl = _str_icmp( cli_name, u_str("DCL") ) == 0;

    insert_in_buffer = 0;
    if( saved_buffer_count >= SAVED_BUFFER_MAX - 2 )
    {
        error( "not enough space to remember buffers" );
        return;
    }
    disp_flag = display;
    if( dcl_count == 0 )
    {
        if(
            ! VMS_SUCCESS( sys$crembx
            (
            0,
            &ichan,
            BUFSIZ,
            BUFSIZ,
            DEFAULT_MAIL_BOX_PROTECTION,
            0, NULL
            ))
        ||
            ! VMS_SUCCESS( sys$crembx
            (
            0,
            &ochan,
            BUFSIZ,
            BUFSIZ,
            DEFAULT_MAIL_BOX_PROTECTION,
            0, NULL
            )) )
        {
            sys$dassgn( ichan );
            sys$dassgn( ochan );
            error( "cannot set up sub-process" );

            return;
        }

        sys_input.dsc$w_length = sizeof( sys_input_text );
        lib$getdvi( (code = DVI$_DEVNAM, &code), &ichan, NULL, NULL,
            &sys_input, &sys_input.dsc$w_length );
        sys_output.dsc$w_length = sizeof( sys_output_text );
        lib$getdvi( (code = DVI$_DEVNAM, &code), &ochan, NULL, NULL,
            &sys_output, &sys_output.dsc$w_length );

        sys_input_text[ sys_input.dsc$w_length ] = 0;
        sys_output_text[ sys_output.dsc$w_length ] = 0;

        if(    ! VMS_SUCCESS( _spawn
            (
            clisetstr(),
            sys_input_text,        // input file spec
            sys_output_text,    // Output file spec
            spawn_flags,        // flags: Nowait
            0,            // Process name
            &sub_pid,        // Process id spawned
            0,            // Completion status
            EFN_DO_NOT_WAIT_ON,    // Completion EFN
            (void (*)(void *))dcl_term,        // Completion ASTADR
            0,            // Completion ASTPRM
            1            // Re-assign channel after use
            )) )
        {
            sys$dassgn( ichan );
            sys$dassgn( ochan );
            error( "cannot set up sub-process" );

            return;
        }

        dcl_count++;
        set_attn( ochan, IO$_SETMODE|IO$M_WRTATTN, mbx_ast, 1 );
        if( is_dcl )
        {
            chk( cmd(u_str("!'f$verify(0)")) );
            chk( cmd(u_str("$ define/process tt NLA0:")) );
        }
        if( is_dcl )
            cmd( u_str("$ define/process sys$command %s"), input );
        sys$dclexh( (void *)&des_blk );
        set_attn( ochan, IO$_SETMODE|IO$M_WRTATTN, 0, 0 );
    }
    saved_buffers[saved_buffer_count] = bf_cur;
    saved_buffer_count++;

    set_bfn( bufname );

    if( interactive() )
        theActiveView->window_on( bf_cur );
    if( erase )
        bf_cur->erase_bf();
    if( disp_flag )
    {
        message( "Starting up ..." );
        do_dsp();
    }
    buf[0] = '\0';
    for(;;)
    {
        unsigned char *arg_str = va_arg( argp, unsigned char * );

        if( arg_str == NULL )
            break;

        if( buf[0] != '\0' )
            _str_cat( buf, " " );
        _str_cat( buf, arg_str );
    }

    set_attn( ochan, IO$_SETMODE|IO$M_WRTATTN, mbx_ast, 1 );
    if( (! is_shell && ! is_dcl)
    || cmd( (( is_shell ) ?  u_str("define sys$input %s")
        : u_str("$ define/process SYS$INPUT %s")), input ) )
    {
        terminated = 0;
        if( cmd( (( is_dcl ) ?  u_str("$%s") : u_str("%s")), buf ) )
        {
            int sent_exit;
            sent_exit = 0;
            insert_in_buffer = 1;
            set_attn( ichan, IO$_SETMODE|IO$M_READATTN, mbx_ast, 0 );
            while( ! terminated )
            {
                if( interrupt_key_struck && ! sent_exit )
                {
                    sent_exit = 1;
                    sys$forcex
                    (
                    &sub_pid,
                    NULL,
                    SS$_ABORT
                    );
                }
                if( timer_interrupt_occurred != 0 )
                    process_timer_interrupts();
                else if( pending_channel_io != 0 )
                    process_channel_interrupts();
                else
                    sys$hiber();
            }
        }
    }
    set_attn( ochan, IO$_SETMODE|IO$M_WRTATTN, 0, 0 );
    bf_modified = 0;

    saved_buffer_count--;
    if( interactive() )
        theActiveView->window_on( saved_buffers[saved_buffer_count] );

    if( display )
        message( "Done." );
    return;
}
#endif
#if defined( SUBPROCESSES ) && defined( __unix__ )
char   *shell()
{        // return the name of the users shell
    static char *sh;
    if (!sh)
    sh = (char *) getenv("SHELL");
    if (!sh)
    sh = "sh";
    return sh;
}

// Copy stuff from indicated file descriptor into the current
// buffer; return the number of characters read.  This routine is
// useful when reading from pipes and such.
static int ReadPipe( int fd, int display )
    {
    register int red = 0;
    register int  n;
    unsigned char buf[1000];
    if (display)
    message("Starting up...");
    if (display)
    theActiveView->do_dsp();
    while ((n = read(fd, buf, 1000)) > 0)
{
    bf_cur->ins_cstr(buf, n);
    red += n;
    if (display)
        {
        message("Chugging along...");
        theActiveView->do_dsp();
        }
}
    if (display)
    message("Done!");
    return red;
}

// execute a subprocess with the output being stuffed into the named
// buffer. exec_bf is called with the command as a list of strings as
// seperate arguments
static void exec_bf
    (
    const EmacsString &buffer,
    int display,
    const EmacsString &input,
    int erase,
    char *command,
    ...
    )
{
    EmacsBufferRef old( bf_cur );
    int     fd[2];
    char *args[100];
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
        char *arg_str = va_arg( argp, char * );

        if( arg_str == NULL )
            break;

        args[arg++] = arg_str;
    }
    args[arg] = NULL;

    EmacsBuffer::set_bfn( buffer );
    if( interactive() )
        theActiveView->window_on(bf_cur);
    if( erase )
        bf_cur->erase_bf();
    pipe(fd);

{
    EmacsPosixSignal sig( SIGCHLD );
    // block now and release as the block ends
    sig.blockSignal();

    if( (subproc_id = fork()) == 0 )
    {
        sig.permitSignal();
        sig.defaultSignalAction();

        close(0);
        close(1);
        close(2);

        if(open( input, 0) != 0)
        {
            write(fd[1], "Couldn't open input file\n", 25);
            _exit(-1);
        }

        dup(fd[1]);
        dup(fd[1]);
        close(fd[1]);
        close(fd[0]);
        execvp(command, args);
        write(1, "Couldn't execute the program!\n", 30);
        _exit(-1);
    }
    close(fd[1]);

    ReadPipe(fd[0], interactive() && display);
    close(fd[0]);
}

    while( subproc_id != 0 )
        sleep(1);

    if( interactive() && old.bufferValid())
        theActiveView->window_on( old.buffer() );
}
#endif

#ifdef SUBPROCESSES
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

    pos = scan_bf( '\n', pos, 1 );
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
        set_dot( scan_bf( '\n', spos, -1) );
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
        return 0;
    if( !com.isNull() )
        execute_command = com;

# ifdef vms
    exec_bf( "command execution", 1, "NLA0:", 1,
        execute_command.data(), NULL );
# endif
# ifdef __unix__
    exec_bf( "command execution", 1, "/dev/null", 1,
        shell (), "-c", execute_command.data(), NULL );
# endif
# ifdef _NT
    exec_bf( "Command execution", 1, "nul", 1, execute_command.data(), NULL );
# endif
    return 0;
}

# ifdef vms
int return_to_monitor( void )
{
    int resp;

    if( dcl_count != 0 )
    {
        kill_prc( 0, &sub_pid );
        sys$hiber();
    }
    if( leave_emacs_proc != 0 )
        execute_bound_saved_environment( leave_emacs_proc );

    rst_dsp();
    if(    ! ( resp = _spawn
        (
        clisetstr(),
        "sys$input",    // input file spec
        "sys$output",    // Output file spec
        0,            // flags: None
        0,            // Process name
        &sub_pid,            // Process id spawned
        0,            // Completion status
        EFN_SIT_FOR,    // Completion EFN
        0,            // Completion ASTADR
        0,            // Completion ASTPRM
        0            // Donnot reassign MBX or IO channels
        ))
    )
        error( two_percent, "Cannot set up sub-process",
            fetch_os_error( resp) );
    complete_reinit = 0;
    init_dsp();
    start_async_io();
    complete_reinit = 1;

    if( return_to_emacs_proc != 0 )
        execute_bound_saved_environment( return_to_emacs_proc );
    return 0;
}
# endif
# if defined( __unix__ ) || defined( _WINDOWS )
int return_to_monitor( void )
{
    return no_value_command();
}
# endif
#endif
