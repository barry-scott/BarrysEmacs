//
//     Copyright(c ) 1982-2009
//        Barry A. Scott

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#  include <jpidef.h>
#  include <iodef.h>
#  include <dvidef.h>

static char *emacs_tmpnam( char *buffer )
{
    int code = JPI$_PID;
    unsigned int pid = 0;
    lib$getjpi( &code, 0, 0, &pid, 0, 0 );

    sprintf( buffer, "sys$scratch:emacs_%x_%d.tmp", pid, tmp_name_count++ );
    return buffer;
}

static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid);
static unsigned short int ichan;
static unsigned short int ochan;
static unsigned int subpid;

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

static void exec_bf( const EmacsString &bufname, int display, const EmacsString &input, int erase, ... );
static void dcl_term(int return_from_monitor);
static int cmd( unsigned char *fmt, ... );
static int opt( unsigned char *buf );
static void set_attn(int chan, int func, int (*rtn)(int), int par);
static int mbx_ast(int must_read);

static int dcl_count = 0;
static unsigned int sub_pid;
static int disp_flag;
static int insert_in_buffer;

int pause_emacs( void )
{
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
# endif
{
# if defined( __unix ) && defined( vms )
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
        send_exit_message( EmacsString::null );

    if( leave_emacs_proc != NULL )
        execute_bound_saved_environment( leave_emacs_proc );

    if( !send_exit_message( pause_command ) )
        error("Unable to send response to bemacs client");
# endif

# ifdef vms
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
# endif
    return 0;
}
# ifdef vms
static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid)
{
    // don't want to kill ourselves!
    if( *subpid != 0 )
        sys$delprc( subpid, NULL );

    return 1;
}

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
# endif
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
