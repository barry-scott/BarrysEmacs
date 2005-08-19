/*
 *	Copyright (c) 1982, 1983, 1984, 1985, 1986
 *		Barry A. Scott and nick Emery
 */
#include <emacs.h>

/*forward*/ void emacs_exit(int code);
#ifdef vms
/*forward*/ void read_restart_message( int set_gargs );
/*forward*/ void send_exit_message( unsigned char *str );
#endif
/*forward*/ int debug_emacs( void );
/*forward*/ int argc_command( void );
/*forward*/ int argv_command( void );
/*forward*/ int emacs_version( void );
/*forward*/ int emacs( int argc, unsigned char **argv,
		unsigned char *rest_fn, unsigned char *device, unsigned char *term_type );
/*forward*/ int executepackage( unsigned char *package );
/*forward*/ int read_in_files( int argc, unsigned char **argv );



#ifdef vms
extern vms_restore_fail( int, unsigned char * );
extern vms_restoring_env( unsigned char * );
extern vms_debug_emacs( unsigned char * );
#endif

extern void journal_exit(void);
extern void init_timer(void);
extern void init_scheduled_timeout(void);
extern void restore_scheduled_timeout(void);
extern void init_dsp(void);	extern void init_memory(void);
extern void init_display(void);	extern void init_func(void);
extern void init_lisp(void);	extern void init_abs(void);
extern void init_srch(void);	extern void init_terminal(unsigned char *,unsigned char *);
extern void init_win(void);	extern void init_opt(void);
extern void restore_db(void);	extern void restore_vms(void);
extern void init_fncs(void);	extern void init_bf(void);
extern void init_display(void);	extern void init_abbrev(void);
extern void init_syntax(void);	extern void init_db(void);
extern void init_undo(void);
extern void init_macros(void);
extern void init_key(void);	extern void init_fio(void);
extern void init_fncs2(void);	extern void init_wn_man(void);
extern void init_meta(void);
extern void init_case(void);	extern void init_arith(void);
extern void init_var(void);	extern void restore_var(void);
extern void restore_journal(void);
extern int can_restore_environment(unsigned char *fn, unsigned char *buf);
extern int restore_environment(void);
extern void init_vms(void);	extern void restore_win(void);
extern void restore_timer(void);

/*
 *	This variable is used in code contained in macros
 *	that need a constant zero to for syntaxic reasons.
 *	Production code does not need this.
 */
#if !defined( must_be_zero )
int must_be_zero = 0;
#endif

int dbg_flags;

FILE *message_file;	/* FIO pointer to SYS$OUTPUT */
FILE *command_file;	/* FIO pointer to SYS$INPUT */
int gui_error = 0;	/* true if a message box isrequired for errors */

extern unsigned char parent_path[256];
GLOBAL SAVRES int silently_kill_processes;
unsigned char operating_system_name[32] = { OS_NAME };
unsigned char operating_system_version[32]
#ifdef OS_VERSION
	= { OS_VERSION }
#endif
	;

unsigned char CPU_type[32]
#ifdef CPU_TYPE
	= { CPU_TYPE }
#endif
	;

int must_checkpoint_on_crash;

int emacs_is_exiting;

void emacs_exit(int code)
	{
	emacs_is_exiting = 1;
	exit( code );
	}

int debug_emacs(void)
	{
	if( arg_state == have_arg
	|| (! interactive && cur_exec != 0 && cur_exec->p_nargs > 0) )
		{
		unsigned char cmd[256];
		unsigned char *p;

		p = getstr( u_str(": debug-emacs ") );
		if( p == 0 )
			return 0;

		_str_cpy( cmd, p );

#if DBG_ALLOC_CHECK
		if( _str_cmp( cmd, "check_heap" ) == 0 )
			emacs_heap_check();
		else
#endif
		if( _str_ncmp( cmd, "flags=", 6 ) == 0 )
			sscanf( s_str( &cmd[6] ), "%i", &dbg_flags );
#ifdef vms
		else
			vms_debug_emacs( cmd );
#endif
		}
	else
		invoke_debug();
	
	return 0;
 	}

extern struct bound_name *enter_emacs_proc;
extern struct bound_name *exit_emacs_proc;
extern unsigned char *exit_emacs_dcl_command;
#ifdef unix
extern void start_emacs_server(void);
#endif

GLOBAL int gargc;		/* global versions of argv envp and argc, for
				 * use by MLisp functions! */
GLOBAL unsigned char **gargv;
int touched_command_args;	/* true iff the user has touched the Unix
				 * command line arguments, this stops Emacs
				 * from doing the VisitFiles */

int argc_command( void )
	{				/* return the value of argc */
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = gargc;
	touched_command_args = 1;
	return 0;
	}

int argv_command( void )
	{				/* return the value of argv[i] */
	int n;
	n = getnum( u_str(": argv index: ") );
	if( ! err )
		if( n >= gargc || n < 0 )
			error( u_str("Argv cannot return the %d'th argument, there are only %d"),
				n, gargc );
		else
			{
			ml_value->exp_type = ISSTRING;
			ml_value->exp_v.v_string = gargv[ n ];
			ml_value->exp_int = _str_len( ml_value->exp_v.v_string );
			}
	touched_command_args = 1;
	return 0;
	}



int emacs_version( void )
	{
	ml_value->exp_type = ISSTRING;
	ml_value->exp_v.v_string = &version_string[0];
	ml_value->exp_int = _str_len( &version_string[0] );

	return 0;
	}



int emacs
	(
	int argc,
	unsigned char **argv,
	unsigned char *rest_fn,
	unsigned char *term_type,
	unsigned char *device
	)
	{
	unsigned char combuf[300];
	unsigned char memory[45];
	unsigned char *fn;
	int saved_err;
	FILE *args;
	int rv;


	{
	unsigned char *p;
	p = get_config_env( u_str("EMACS_DEBUG") );
	if( p != NULL )
		{
		sscanf( s_str( p ), "%i", &dbg_flags );

#if defined( _MSDOS ) && !defined( _WINDOWS ) && !defined( macintosh )
		if( dbg_flags != 0 )
			{
			printf( "Debug flags = %x\n", dbg_flags );
			(void)_bios_keybrd( _KEYBRD_READ );
			printf( "Emacs image loaded at address %.4x:0000\n", _psp );
			(void)_bios_keybrd( _KEYBRD_READ );
			}
#endif
		if( dbg_flags != 0 )
			debug( -3, u_str( "dbg_flags=%x" ), dbg_flags );
		}
	}

	/*
	 *	check to see if a restore operation is required and possible
	 */
	init_memory();
	rv = can_restore_environment( rest_fn, combuf );
	if( rv < 0 )
		{
#ifdef vms
		vms_restore_fail( 1, combuf );
#else
		_dbg_msg( u_str("Unable to restore environment file %s - please recreate it"), combuf );
#endif
		}
	if( rv > 0 )
		{
		int status;
		init_display();	/* " the core display system */
		init_func();	/* " lisp environment enquiry functions */
		init_lisp();	/* " the MLisp system */
		init_abs();	/* " the current directory name */
#ifdef vms
		vms_restoring_env( combuf );
#endif
		if( ((status = restore_environment())&1) != 1 )
			{
#ifdef vms
			vms_restore_fail( status, combuf );
#else
			_dbg_msg( u_str("Unable to restore environment file %s - please recreate it"), combuf );
			return 0;
#endif
			}
		gargc = argc;
		gargv = argv;
		restore_var();
		init_srch();		/* " the search commands */
		init_terminal( term_type, device );	/* Start the primary terminal IO system */
		init_dsp();
		restore_win();		/* " the window system */
		init_opt();		/* " commands that deal with options */
		restore_timer();
		restore_scheduled_timeout();
		restore_db();		/* Restart the database manager */
		restore_vms();		/* Restart the sub-process code */
		restore_journal();	/* Restore the journal system */
		}
	else
		{
		gargc = argc;
		gargv = argv;
		init_fncs();		/* initialise the key bindings */
		init_var();		/* " the variables */
		init_bf();		/* " the buffer system */
		init_timer();
		init_scheduled_timeout();
		init_display();		/* " the core display system */
		init_terminal( term_type, device );	/* Start the primary terminal IO system */
		init_dsp();
		init_win();		/* " the window system */
		init_wn_man();		/* " the window management commands */
		init_srch();		/* " the search commands */
		init_meta();		/* " the simple meta commands */
		init_abbrev();		/* " the abbrev system */
		init_syntax();		/* " the syntax table system */
		init_db();		/* " the data base manager */
		init_case();		/* " the case manipulation commands */
		init_arith();		/* " the arithmetic operators( for lisp) */
		init_undo();		/* " the undo facility */
		init_func();		/* " lisp environment enquiry functions */
		init_lisp();		/* " the MLisp system */
		init_abs();		/* " the current directory name */
		init_vms();		/* " VMS specific commands */
		init_opt();		/* " commands that deal with options */
		init_macros();		/* " the macro system and name bindings */
		/* The following calls MUST be after init_Macro */
		init_key();		/* " commands that deal with options */
		init_fio();		/* " the file IO system */
		current_global_map = global_map;
		init_fncs2();		/* Finish off init of functions */
		}

	saved_err = 0;
	args = 0;
	rv = 0;

#ifdef vms
	read_restart_message( 0 );
#endif
	start_async_io();

#ifdef unix
	start_emacs_server();
#endif

	must_checkpoint_on_crash = 1;

	if( is_restored == 0 )
		{
		gui_error = 1;
		rv = execute_mlisp_file( EMACS_INIT_PROFILE, 1 );
		gui_error = 0;
		}

	/* initialise the user interface for this platform */
	rv = execute_mlisp_file( u_str("emacs_default_user_interface"), 1 );

	/* do not run the enter-emacs-hook if we are building the default environment */
	if(( term_is_terminal || is_restored != 0)
	&& enter_emacs_proc != 0
	&& rv == 0 )
		rv = execute_bound_saved_environment( enter_emacs_proc );

	/* execute the start up package */
	if( rv == 0 )
		rv = executepackage( argv[0] );

	fn = MEMORY_FILE_STR;
	if( fn != NULL )
	    sprintfl( memory, sizeof( memory ), fn, MEMORY_FILE_ARG );
	if( rv == 0 && ! touched_command_args )
	    {
	    int done_any_visiting;

	    done_any_visiting = read_in_files( argc, argv );
	    if( ! done_any_visiting
	    && (args = fio_open( memory, FIO_READ, 0, 0)) != 0 )
		    while( fio_get_line( args, combuf, sizeof( combuf)) > 0 )
			    {
			    int i;
			    unsigned char *p;

			    p = &combuf[0];
			    while( *p != 0 && *p >= ' ' )
				    p++;
			    i = p[0];
			    *p++ = 0;
			    saved_err = saved_err || err;
			    err = 0;
			    visit_file( &combuf[0], 1, 1, u_str("") );
			    if( i == 1 )
				    {
				    i = 0;
				    while( '0' <= p[0] && p[0] <= '9' )
					    {
					    i = i * 10 + p[0] - '0';
					    p = &p[1];
					    }
				    if( i > first_character && i <= (num_characters+1) )
					    set_dot( i );
				    }
			    }
		    if( args != 0 )
			    fio_close( args );
		    }
	if( fn != NULL )
	    fio_delete( &memory[0] );

	err = 0;

	if( term_is_terminal )
		{
		int can_exit;
		do
			{
			if( rv == 0 )
				process_keys();
			can_exit = 1;
			rv = 0;
			if( mod_exist() )
				can_exit = get_yes_or_no( 0, u_str("Modified buffers exist, do you really want to exit? ") );

			if( can_exit
			&& ! silently_kill_processes
			&& count_processes() )
				can_exit = get_yes_or_no( 1, u_str("You have processes still on the prowl, shall I chase them down for you? " ) );

			}
		while( ! can_exit );
		}
	else
		/* execute sys$input as the command stream */
		rv = execute_mlisp_file( device, 0 );

	if( exit_emacs_proc != 0 && rv == 0 )
		rv = execute_bound_saved_environment( exit_emacs_proc );

	if( fn != NULL )
	    {
	    struct window *w;
	    args = 0;
	    w = windows;
	    while( ( w != 0 ) )
		{
		set_bfp( w->w_buf);
		if( bf_cur->b_fname != 0 )
			{
			if( args == 0 )
				{
				args = fio_create( &memory[0], 1, FIO_STD, 0, default_rms_attribute );
				if( args == 0 )
					break;
				}
			sprintfl( &combuf[0], sizeof( combuf ),
				u_str("%s\001%d\n"), bf_cur->b_fname,
					(( w == wn_cur ) ?  dot : to_mark( w->w_dot) ));
			fio_put( args, &combuf[0], _str_len( &combuf[0] ) );
			}
		w = w->w_next;
		}
	    }
	if( args )
		fio_close( args );

	kill_processes();
	/* check_ for an exit DCL command, and send it to the mailbox if
	 * required */
#ifdef vms
	if( exit_emacs_dcl_command != 0 )
		send_exit_message( exit_emacs_dcl_command );
#endif
	kill_checkpoint_files();
	journal_exit();

	rst_dsp();
	emacs_is_exiting = 1;
#ifdef vms
	return 1;
#else
	return EXIT_SUCCESS;
#endif
	}


int executepackage( unsigned char *package )
	{
	int i;
	int rv;
	rv = 0;
	/*
	 * If Emacs is invoked by any name other than 'emacs' we try to load a
	 * package by that name and then execute a command by that name; this
	 * is so that things like rmail and dired can be invoked from the
	 * DCL command line.
	 */
	if( package != 0 && _str_cmp( package, u_str("emacs")) != 0 )
		{
		unsigned char buf[300];
		unsigned char buf1[300];
		unsigned char *p;

		sprintfl( buf, sizeof( buf ), u_str("%s.mlp"), package );
		p = u_str(_str_rchr( package, PATH_CH ));
		if( p == NULL )
			p = package;
		else
			p++;
		sprintfl( buf1, sizeof( buf1 ), u_str("%s-com"), p );
		/*
		 * See if the function is defined already
		 */
		if( (i = find_mac( &buf1[0] )) >= 0 )
			return execute_bound_saved_environment( mac_bodies[i] );
		/* as the func is not defined load the .MLP file and try again */
		if( (rv = execute_mlisp_file( &buf[0], 1)) != 0
		|| err
		|| (i = find_mac( buf1)) < 0 )
			{
			err = 0;
			minibuf_body = 0;
			}
		else
			return execute_bound_saved_environment( mac_bodies[i] );
		}
	return rv;
	}

int read_in_files
	(
	int argc,
	unsigned char * *argv
	)
	{
	int done_any_visiting;
	int saved_err;
	int i;

	done_any_visiting = 0;
	saved_err = 0;

	if( ! touched_command_args )
		for( i=1; i<=argc - 1; i += 1 )
			{
			saved_err = saved_err || err;
			err = 0;
			visit_file( argv[i], 1, 1,
				(( parent_path[0] == 0 ) ?  u_str("") : &parent_path[0]) );
			done_any_visiting = 1;
			}
	touched_command_args = 1;
	err = err || saved_err;
	return done_any_visiting;
	}

void debug
	(
#ifdef MEMMAP
	int row,
#else
	int PNOTUSED(row), 
#endif
	unsigned char *fmt, ...
	)
	{
	static struct emacs_line line;
	va_list arg_ptr;

	va_start( arg_ptr, fmt );
	line.line_length =
		do_print( fmt, &arg_ptr, line.line_body, sizeof( line.line_body )-1 );

#if MEMMAP
	if( tt && tt->t_update_line )
		tt->t_update_line( tt, NULL, &line, row );
	else
		_dbg_msg( u_str("%.*s\n"), line.line_length, line.line_body );
#endif
#if	DBG_DUMPTOFILE
	if( (dbg_flags&DBG_DUMPTOFILE) != 0 )
	{
	static FILE *dbg_dump_file;

	line.line_body[ line.line_length ] = '\n'; line.line_length++;
	line.line_body[ line.line_length ] = '\0';

	if( dbg_dump_file == NULL )
		{
                unsigned char *s;
                s = get_config_env( u_str("EMACS_DBGFILE") );
		if( s != NULL )
			dbg_dump_file = fopen( s_str(s), "w" );
		}

	if( dbg_dump_file != NULL )
		{
		fwrite( line.line_body, line.line_length, 1, dbg_dump_file );
		fflush( dbg_dump_file );
		}
	}
#endif
	}
