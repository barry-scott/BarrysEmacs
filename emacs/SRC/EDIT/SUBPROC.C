/* 	Copyright(c ) 1982-1995
 *		Barry A. Scott and nick Emery */

#include <emacs.h>

#if defined( _WINDOWS )
# include <win_incl.h>
#endif

#ifdef vms
# include <jpidef.h>
# include <iodef.h>
# include <dvidef.h>

#define tmpnam vms_tmpnam

static tmp_name_count = 1;
static char *vms_tmpnam( char *buffer )
	{
	int code = JPI$_PID;
	unsigned int pid = 0;
	lib$getjpi( &code, 0, 0, &pid, 0, 0 );

	sprintfl( buffer, sizeof(buffer), "sys$scratch:emacs_%x_%d.tmp", pid, tmp_name_count++ );
	return buffer;
	}


#endif

#ifdef unix
# ifdef SUBPROCESSES
#  include <signal.h>
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

/*forward*/ int check_enter_emacs(unsigned char *value, struct variablename *v);
/*forward*/ int check_exit_emacs(unsigned char *value, struct variablename *v);
/*forward*/ int check_leave_emacs(unsigned char *value, struct variablename *v);
/*forward*/ int check_return_to_emacs(unsigned char *value, struct variablename *v);
/*forward*/ int indent_c_procedure( void );
/*forward*/ int compile_it( void );
/*forward*/ int execute_monitor_command( void );
/*forward*/ int return_to_monitor( void );
/*forward*/ int pause_emacs( void );
/*forward*/ int filter_region( void );
/*forward*/ void filter_through(int n, unsigned char *command, ... );

#ifdef vms
/*forward*/ static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid);
#endif

#ifdef vms
static unsigned short int ichan;
static unsigned short int ochan;
static unsigned int subpid;

# if defined(SUBPROCESSES)
#  define	chk( a ) if( ! VMS_SUCCESS(a) ) return

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

#ifdef unix
# ifdef SUBPROCESSES
pid_t subproc_id;
# endif
#endif

GLOBAL SAVRES unsigned char compile_command[300];
GLOBAL SAVRES unsigned char debug_command[300];
GLOBAL SAVRES unsigned char filter_command[300];
GLOBAL SAVRES unsigned char execute_command[300];
GLOBAL SAVRES unsigned char cli_name[64];

#if defined( SUBPROCESSES )
# ifdef vms
/*forward*/ static void exec_bf( unsigned char *bufname, int display, unsigned char *input, int erase, ... );
/*forward*/ static void dcl_term(int return_from_monitor);
/*forward*/ static int cmd( unsigned char *fmt, ... );
/*forward*/ static int opt( unsigned char *buf );
/*forward*/ static void set_attn(int chan, int func, int (*rtn)(int), int par);
/*forward*/ static int mbx_ast(int must_read);
# endif
# ifdef unix
/*forward*/ static void exec_bf( unsigned char *bufname, int display, unsigned char *input, int erase, char *command, ... );
# endif
#if defined( _NT )
static void exec_bf
	(
	unsigned char *bufname,
	int display,
	unsigned char *input,
	int erase,
	...
	);
#endif
#endif

GLOBAL SAVRES int saved_buffer_count;
GLOBAL SAVRES struct emacs_buffer *saved_buffers[SAVED_BUFFER_MAX];
#ifdef _NT
static int disp_flag = 0;
static int insert_in_buffer = 0;
#endif

GLOBAL SAVRES unsigned char enter_emacs_hook[MLISP_HOOK_SIZE];
GLOBAL SAVRES unsigned char exit_emacs_hook[MLISP_HOOK_SIZE];
GLOBAL SAVRES unsigned char leave_emacs_hook[MLISP_HOOK_SIZE];
GLOBAL SAVRES unsigned char return_to_emacs_hook[MLISP_HOOK_SIZE];

GLOBAL SAVRES struct bound_name *enter_emacs_proc;
GLOBAL SAVRES struct bound_name *exit_emacs_proc;
GLOBAL SAVRES struct bound_name *leave_emacs_proc;
GLOBAL SAVRES struct bound_name *return_to_emacs_proc;

#ifdef vms
static int dcl_count = 0;
static unsigned int sub_pid;
unsigned int parent_pid;
#endif

int check_enter_emacs(unsigned char * value, struct variablename *v)
	{
	return check_is_proc( value, v, &enter_emacs_proc );
	}

int check_exit_emacs(unsigned char * value, struct variablename *v)
	{
	return check_is_proc( value, v, &exit_emacs_proc );
	}

int check_leave_emacs(unsigned char * value, struct variablename *v)
	{
	return check_is_proc( value, v, &leave_emacs_proc );
	}

int check_return_to_emacs(unsigned char * value, struct variablename *v)
	{
	return check_is_proc( value, v, &return_to_emacs_proc );
	}

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
			error( u_str("There is no parent to which to pass control") );
			return 0;
			}
		}
	wake_queued = 0;

	if( dcl_count != 0 )
		{
		kill_prc( 0, &sub_pid );
		sys$hiber();
		}
	/*
	 * See if the user wants to specify a command, fetch it, and send
	 * it down the restart mailbox
	 */
	if( arg_state == have_arg
	|| (! interactive && cur_exec != 0 && cur_exec->p_nargs > 0) )
		{
		static unsigned char last[300];
		static unsigned char *p;
		p = getstr( u_str(": pause-emacs ") );
		if( p == 0 )
			return 0;
		if( _str_len( p ) > 0 && _str_len( p ) < sizeof( last ) )
			_str_cpy( last, p );
		send_exit_message( last );
		}
	if( leave_emacs_proc != NULL )
		execute_bound_saved_environment( leave_emacs_proc );

	rst_dsp();
	journal_pause();

	if( ! VMS_SUCCESS(lib$attach( &parent_pid )) )
		error( u_str("Failed to attach to parent process") );
	complete_reinit = 1;
	init_dsp();
	start_async_io();
	journal_flush();
	fit_windows_to_screen_length();

	complete_reinit = 0;
	read_restart_message( 1 );
	err = 0;
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
	unsigned char *s;

	if( bf_cur->b_mark == 0 )
		{
		error( u_str("mark not set") );
		return 0;
		}
	s = getstr( u_str(": filter-region (through command) ") );
	if( s != NULL )
		{
		if( s[0] != '\0' )
			_str_cpy( filter_command, s );
		filter_through( to_mark( bf_cur->b_mark) - dot, filter_command, NULL );
		}
	return 0;
	}

/* pass the region starting at dot and extending for n characters through
 * the command. The old contents of the region is left in the kill
 * buffer */
void filter_through(int n, unsigned char *command, ... )
	{
#ifdef SUBPROCESSES
	unsigned char tempfile[MAXPATHLEN+1];
	unsigned char buf[265];	
	struct emacs_buffer *old;
	va_list argp;

	if( tmpnam( s_str( tempfile ) ) == NULL )
		{
		error( u_str("Unable to create temporary file") );
		return;
		}

	old = bf_cur;
	del_to_buf( n, /* replace buffer */ 0, /* and do it */ 1, u_str("Kill buffer") );

	set_bfn( u_str("Kill buffer") );
	write_file( tempfile, 1 );
	set_bfp( old );

	va_start( argp, command );

	_str_cpy( buf, command );
	for(;;)
		{
		unsigned char *arg_str = va_arg( argp, unsigned char * );

		if( arg_str == NULL )
			break;

		_str_cat( buf, " " );
		_str_cat( buf, arg_str );
		}
# ifdef vms
	exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, s_str (buf), NULL );
# endif
# ifdef unix
	exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, shell (), "-c", s_str (buf), NULL );
# endif
# ifdef _NT
	exec_bf( bf_cur->b_buf_name, 0, tempfile, 0, s_str (buf), NULL );
# endif
	if( bf_modified == 0 )
		{
		redo_modes = cant_1line_opt = 1;
		}
	bf_modified++;
	fio_delete( tempfile );
#endif
	}

#if defined( _NT )
static void exec_bf
	(
	unsigned char *bufname,
	int display,
	unsigned char *input,
	int erase,
	...
	)
	{
	va_list argp;
	unsigned char buf[256];
	unsigned char output[MAXPATHLEN+1];

	if( tmpnam( s_str( output ) ) == NULL )
		{
		error( u_str("Unable to create temporary file") );
		return;
		}
	
	va_start( argp, erase );

	if( saved_buffer_count >= SAVED_BUFFER_MAX - 2 )
		{
		error( u_str("not enough space to remember buffers") );
		return;
		}

	disp_flag = display;

	saved_buffers[saved_buffer_count] = bf_cur;
	saved_buffer_count++;

	set_bfn( bufname );

	if( interactive )
		window_on( bf_cur );

	if( erase )
		erase_bf( bf_cur );

	if( disp_flag )
		{
		message( u_str("starting up ...") );
		do_dsp( 1 );
		}

	/* use the prefered command interpreter */
	_str_cpy( buf, get_env( "COMSPEC" ) );
	_str_cat( buf, " /c" );
	for(;;)
		{
		unsigned char *arg_str = va_arg( argp, unsigned char * );

		if( arg_str == NULL )
			break;

		_str_cat( buf, " " );
		_str_cat( buf, arg_str );
		}

	_str_cat( buf, " <" ); _str_cat( buf, input );
	_str_cat( buf, " >" ); _str_cat( buf, output );

	PROCESS_INFORMATION ProcessInformation;
	STARTUPINFO si;

	si.cb = sizeof(STARTUPINFO);
	si.lpReserved = NULL;
	si.lpTitle = NULL;
	si.lpDesktop = NULL;
	si.dwX = si.dwY = si.dwXSize = si.dwYSize = si.dwFlags = 0L;
	si.wShowWindow = SW_HIDE;
	si.lpReserved2 = NULL;
	si.cbReserved2 = 0;

	if( CreateProcess
		(
		NULL,
		s_str(buf),
		NULL,
		NULL,
		FALSE, // Inherit handles
		0,
		NULL,
		NULL,
		&si,
		&ProcessInformation
		))
		{
		WaitForSingleObject( ProcessInformation.hThread, INFINITE );
		if( CloseHandle( ProcessInformation.hThread ) == FALSE )
			error( u_str("Failed to close process handle in exec_bf") );
		}
	else
		error( u_str("Failed to execute shell, error = %d\n"), GetLastError());


	/* insert the output file */
	read_file( output, 0, 0 );

	fio_delete( output );

	bf_modified = 0;

	saved_buffer_count--;
	if( interactive )
		window_on( saved_buffers[saved_buffer_count] );

	if( display )
		message( u_str("done.") );
	return;
	}
#endif

#ifdef vms
static int kill_prc(unsigned int *reason_for_exit, unsigned int *subpid)
	{
	/* don't want to kill ourselves! */
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
		error( u_str("Failed to send command to sub-process") );
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
			ins_cstr( buf, size+1 );
/*("do_dsp(1) from a F***ING AST routine!!!!!!!!")*/
			if( interactive )
				do_dsp( 1 );
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
	unsigned char *bufname,
	int display,
	unsigned char *input,
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
		error( u_str("not enough space to remember buffers") );
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
			error( u_str("cannot set up sub-process") );

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

		if(	! VMS_SUCCESS( _spawn
			(
			clisetstr(),
			sys_input_text,		/* input file spec */
			sys_output_text,	/* Output file spec */
			spawn_flags,		/* flags: Nowait */
			0,			/* Process name */
			&sub_pid,		/* Process id spawned */
			0,			/* Completion status */
			EFN_DO_NOT_WAIT_ON,	/* Completion EFN */
			(void (*)(void *))dcl_term,		/* Completion ASTADR */
			0,			/* Completion ASTPRM */
			1			/* Re-assign channel after use */
			)) )
			{
			sys$dassgn( ichan );
			sys$dassgn( ochan );
			error( u_str("cannot set up sub-process") );

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

	if( interactive )
		window_on( bf_cur );
	if( erase )
		erase_bf( bf_cur );
	if( disp_flag )
		{
		message( u_str("starting up ...") );
		do_dsp( 1 );
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
				wake_queued = 0;

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
	if( interactive )
		window_on( saved_buffers[saved_buffer_count] );

	if( display )
		message( u_str("done.") );
	return;
	}
#endif
#if defined( SUBPROCESSES ) && defined( unix )
char   *shell()
{		/* return the name of the users shell */
    static char *sh;
    if (!sh)
	sh = (char *) getenv("SHELL");
    if (!sh)
	sh = "sh";
    return sh;
}

/* Copy stuff from indicated file descriptor into the current
   buffer; return the number of characters read.  This routine is
   useful when reading from pipes and such.  */
static int ReadPipe( int fd, int display )
	{
	register int red = 0;
	register int  n;
	unsigned char buf[1000];
	if (display)
		message(u_str ("Starting up..."));
	if (display)
		do_dsp(1);

	for(;;)
		{
		while ((n = read(fd, buf, 1000)) < 0 && errno == EINTR)
			;
		if( n <= 0 )
			break;
		ins_cstr(buf, n);
		red += n;
		if (display)
			{
			message(u_str ("Chugging along..."));
			do_dsp(1);
			}
		}

	if (display)
		message(u_str ("Done!"));
	return red;
	}

/* execute a subprocess with the output being stuffed into the named
   buffer. exec_bf is called with the command as a list of strings as
   seperate arguments */
static void exec_bf
	(
	unsigned char *buffer,
	int display,
	unsigned char *input,
	int erase,
	char *command,
	...
	)
	{
	struct emacs_buffer  *old = bf_cur;
	int fd[2];
	long int sig_mask;
	char *args[512];
	int arg;
	va_list argp;

	/*
	*	Need to copy the args off of the stack
	*	as there is no reason for the stack order to
	*	match a char ** order. HP PA-Risc exposed this
	*	problem.
	*/
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

	set_bfn( buffer );
	if( interactive )
		window_on( bf_cur );
	if( erase )
		erase_bf( bf_cur );
	pipe( fd );
	/* turn off signal process until we are all done */
	sig_mask = sigblock( 1<<SIGCHLD );
	if( (subproc_id = vfork()) == 0 )
		{
		close( 0 );
		close( 1 );
		close( 2 );
		if( open( s_str( input ), 0 ) != 0 )
			{
			write( fd[1], "Couldn't open input file\n", 25 );
			_exit( -1 );
			}
		dup( fd[1] );
		dup( fd[1] );
		close( fd[1] );
		close( fd[0] );
		execvp( command, args );
		write( 1, "Couldn't execute the program!\n", 30 );
		_exit( -1 );
		}
	close( fd[1] );
	ReadPipe( fd[0], interactive && display );
	close( fd[0] );

	/* now enable the signal handler */
	signal( SIGCHLD, child_sig );
	sigsetmask( sig_mask );

	/* wait for the child to die */
	while( subproc_id )
		{
		sigpause( 1<<SIGCHLD );
		}
	if( interactive )
		window_on( old );
	}
#endif

#ifdef SUBPROCESSES
int indent_c_procedure( void )
	{
	int spos;
	int nest;
	int pos;
	nest = 0;
	pos = search( u_str("^}"), 1, dot - 3, 1 );
	if( pos <= 0 )
		{
		error( u_str("cannot find procedure boundary") );
		return 0;
		}
	spos = pos;
	pos = scan_bf( '\n', pos, 1 );
	while( spos > 1 )
		{
		unsigned char c;
		c = char_at( spos );
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
		filter_through( pos - dot, u_str("indent"), u_str("-st"), NULL );
		}
	else
		error( u_str("cannot find procedure boundary") );
	return 0;
	}

int compile_it( void )
	{
	unsigned char *com;

	if( saved_buffer_count >= SAVED_BUFFER_MAX - 2 )
		{
		error( u_str("not enough space to remember buffers") );
		return 0;
		}
	saved_buffers[saved_buffer_count] = bf_cur;
	saved_buffer_count++;
	if( mod_write() )
		{
		/*
		 * this test really shouldn't be done this way, all the prefix
		 * numeric argument stuff needs to be rationalized
		 */
		/*("arg_state stuff is naff")*/
		if( arg_state == have_arg || ! interactive )
			{
			com = getstr( u_str("compilation command: ") );
			if( com == 0 )
				{
				saved_buffer_count--;
				return 0;
				}
			if( com[0] != 0 )
				_str_cpy( compile_command, com );
# ifdef vms
			exec_bf( u_str("error log"), 1, u_str("NLA0:"), 1,
				compile_command, NULL );
# endif
# ifdef unix
			exec_bf( u_str("error log"), 1, u_str("/dev/null"), 1,
				shell (), "-c", compile_command, NULL );
# endif
			}
		else
# ifdef vms
			exec_bf( u_str("error log"), 1, u_str("NLA0:"), 1,
				u_str("MMS"), NULL );
# endif
# ifdef unix
			exec_bf( u_str("error log"), 1, u_str("/dev/null"), 1,
				"make", NULL );
# endif
		set_bfn( u_str("error log") );
		parse_erb( 1, bf_s1 + bf_s2 );
		saved_buffer_count--;
		set_bfp( saved_buffers[saved_buffer_count] );
		}
	return 0;
	}


int execute_monitor_command( void )
	{
	unsigned char *com;

	com = getstr( u_str("Command: ") );
	if( com == 0 )
		return 0;
	if( com[0] != 0 )
		_str_cpy( &execute_command[0], com );
# ifdef vms
	exec_bf( u_str("command execution"), 1, u_str("NLA0:"), 1,
		&execute_command[0], NULL );
# endif
# ifdef unix
	exec_bf( u_str("command execution"), 1, u_str("/dev/null"), 1,
		shell (), "-c", &execute_command[0], NULL );
# endif
	return 0;
	}

# ifdef vms
int return_to_monitor( void )
	{
	int resp;

	wake_queued = 0;

	if( dcl_count != 0 )
		{
		kill_prc( 0, &sub_pid );
		sys$hiber();
		}
	if( leave_emacs_proc != 0 )
		execute_bound_saved_environment( leave_emacs_proc );

	rst_dsp();
	if(	! ( resp = _spawn
		(
		clisetstr(),
		"sys$input",	/* input file spec */
		"sys$output",	/* Output file spec */
		0,			/* flags: None */
		0,			/* Process name */
		&sub_pid,			/* Process id spawned */
		0,			/* Completion status */
		EFN_SIT_FOR,	/* Completion EFN */
		0,			/* Completion ASTADR */
		0,			/* Completion ASTPRM */
		0			/* Donnot reassign MBX or IO channels */
		))
	)
		error( two_percent, u_str("Cannot set up sub-process" ),
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
# if defined( unix ) || defined( _WINDOWS )
int return_to_monitor( void )
	{
	return no_value_command();
	}
# endif
#endif
