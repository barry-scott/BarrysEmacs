/* 	Copyright(c) 1994 Barry A. Scott */

#include <emacs.h>

GLOBAL SAVRES int force_redisplay;
GLOBAL SAVRES int maximum_dcl_buffer_size = 10000;
GLOBAL SAVRES int dcl_buffer_reduction = 500;

#define PIPE_ANONAMOUS 0

#include <nt_comm.h>

/*forward*/ int process_channel_interrupts( void );
/*forward*/ void stop_nt_process( struct process_blk *proc );
/*forward*/ int start_dcl_process( void );
/*forward*/ static struct process_blk * get_process_arg( void );
/*forward*/ static struct bound_name *get_procedure_arg( void );
/*forward*/ int send_string_to_process( void );
/*forward*/ int set_process_termination_proc( void );
/*forward*/ int set_process_output_procedure( void );
/*forward*/ int set_process_input_procedure( void );
/*forward*/ int set_process_output_buffer( void );
/*forward*/ int kill_process( void );
/*forward*/ void proc_de_ref_buf( struct emacs_buffer *b );
/*forward*/ void kill_processes( void );
/*forward*/ int count_processes( void );
/*forward*/ int pause_process( void );
/*forward*/ int resume_process( void );
/*forward*/ int force_exit_process( void );
/*forward*/ int set_current_process( void );
/*forward*/ int current_process_name( void );
/*forward*/ int set_process_name( void );
/*forward*/ int process_output( void );
/*forward*/ int list_processes( void );
/*forward*/ int wait_for_process_input_request( void );
/*forward*/ int vms_load_averages( void );
/*forward*/ void init_vms( void );
/*forward*/ void restore_vms( void );

volatile int terminating_process;	/* Process terminated */
unsigned int base_priority;

GLOBAL SAVRES struct queue channel_list_head;
GLOBAL SAVRES struct process_blk *current_process = 0;
GLOBAL SAVRES struct process_blk **process_ptrs;
GLOBAL SAVRES unsigned char **process_names;
GLOBAL SAVRES int n_processes = 0;
GLOBAL SAVRES int n_process_slots = 20;

extern unsigned char cli_name[64];

static BOOL nt_console;
static HANDLE con_in_handle, con_out_handle;

/*
 *
 *	Routine to Process Channel Interrupts( called from Scheduler )
 *
 */
int process_channel_interrupts( void )
	{
	int work_done = 0;
	struct channel *chan;
	int savedcant_1line_opt;
	int savedcant_1win_opt;
	int savedredo_modes;
	struct process_blk *saved_current_process;
	struct emacs_buffer *saved_buffer;

	savedcant_1line_opt = cant_1line_opt;	/* Be nice to refresher */
	savedcant_1win_opt = cant_1win_opt;	/* ditto */
	savedredo_modes = redo_modes;		/* ditto */
	saved_current_process = current_process;
	saved_buffer = 0;
	/*
	 *	Process any terminating processes
	 */
	while( terminating_process != 0 )
		{
		struct process_blk *proc;
		int i;

		interlock_dec( &terminating_process );
		interlock_dec( &pending_channel_io );

		for( i=0; i<n_processes; i++ )
			{
			int j;

			proc = process_ptrs[i];
			if( proc->proc_state == PROC_K_DEAD )
				{
				work_done = 1;

				if( saved_current_process == proc )
					saved_current_process = 0;
				if( proc->proc_procedure != 0 )
					{
					/*
					 *	Call the term procedure
					 */
					current_process = proc;
					execute_bound_saved_environment( proc->proc_procedure );
					if( force_redisplay )
						{
						do_dsp( 1 );
						force_redisplay = 0;
						savedcant_1win_opt = cant_1win_opt;
						savedcant_1line_opt = cant_1line_opt;
						savedredo_modes = redo_modes;
						}
					}

				stop_nt_process( proc );

				/*
				 *	Kill the process entry
				 */
				process_names[i] = 0;
				process_ptrs[i] = 0;

				for( j=i+1; j<n_processes; j++ )
					{
					process_names[ j - 1] = process_names[ j];
					process_ptrs[ j - 1] = process_ptrs[ j];
					}
				n_processes--;
				/*
				 *	Free the data structures
				 */
				free( proc->proc_name );
				free( proc );
				}
			}
		}

	/*
	 *	Scan the list of channels to see which ones require attention
	 */
	chan = (struct channel *)channel_list_head.next;
	while( &chan->chan_queue != &channel_list_head )
		{
		/*
		 *	Service all Interrupting Channels
		 */
		if( chan->chan_interrupt )
			{
			/*
			 *	Turn Off the interrupt Flag
			 */
			chan->chan_interrupt = 0;
			/*
			 *	Service read/write channel
			 */
			if( chan->chan_read_channel )
				{
				/*
				 *	Read Channel, Get the data
				 */
				{
				/*
				 *	Mung the input data( if necessary )
				 */
				work_done = 1;

				{
				unsigned char *s;
				int n;

				s = chan->chan_data_buffer;
				n = chan->chan_chars_left;

				while( n > 0 )
					{
					n--;
					if( s[0] == '\r' )
						if( n > 0 && s[1] == '\n' )
							{
							chan->chan_chars_left--;
							/* remove the CR */
							memmove( &s[0], &s[1], n );
							n--;
							}
						else
							{
							/* change CR to NL */
							s[0] = '\n';
							}
					s++;
					}
				}

				/*
				 *	If there is a buffer associated with
				 *	this process output, make it current
				 */
				if( chan->chan_buffer != 0 )
					{
					saved_buffer = bf_cur;
					set_bfp( chan->chan_buffer );
					set_dot( bf_s1 + bf_s2 + 1 );
					}
				/*
				 *	If there is no procedure to call, assume
				 *	that we must stuff into Buffer
				 */
				if( chan->chan_procedure == NULL )
					{
					/*
					 *	If no buffer, get out of here!
					 */
					if( chan->chan_buffer == NULL )
						break;
					/*
					 *	Insert the Data
					 */
					ins_cstr( chan->chan_data_buffer, chan->chan_chars_left );
					/*
					 *	If buffer has overflowed
					 *	Delete start of buffer
					 */
					if( (bf_s1 + bf_s2) > chan->chan_maximum_buffer_size )
						{
						del_frwd( 1, chan->chan_buffer_reduction_size );
						set_dot( bf_s1 + bf_s2 + 1 );
						}
					}/* End of Put Data into Buffer */
				else
					{
					/*
					 *	Call the procedure
					 */
					current_process = chan->chan_process;
					chan->chan_local_buffer_has_data = 1;
					execute_bound_saved_environment( chan->chan_procedure );
					chan->chan_local_buffer_has_data = 0;
					if( force_redisplay )
						{
						do_dsp( 1 );
						force_redisplay = 0;
						savedcant_1win_opt = cant_1win_opt;
						savedcant_1line_opt = cant_1line_opt;
						savedredo_modes = redo_modes;
						}
					}/* End of Call Procedure to deal with Data*/

				/*
				 *	If we had switched buffers, restore old one
				 */
				if( saved_buffer != NULL )
					{
					/*
					 *	If no mark exists, create one
					 */
					if( bf_cur->b_mark == NULL )
						bf_cur->b_mark = new_mark();
					/*
					 *	Set the Mark
					 */
					set_mark( bf_cur->b_mark, bf_cur, dot, 0 );
					/*
					 *	Go back to original buffer
					 */
					set_bfp( saved_buffer );
					}
				/*
				 *	Deal with possible re-displaying
				 *	(We take any possible excuse to
				 *	not redisplay)
				 */
				chan->chan_reads_to_go_before_redisplay--;
				if( input_pending == 0
				&& (chan->chan_reads_to_go_before_redisplay <= 0) )
					{
					/*
					 *	If it is the current buffer, we
					 *		always redisplay
					 */
					if( bf_cur == chan->chan_buffer )
						{
						chan->chan_reads_to_go_before_redisplay = 1;
						do_dsp( 1 );
						savedcant_1line_opt = cant_1line_opt;
						savedcant_1win_opt = cant_1win_opt;
						savedredo_modes = redo_modes;
						}/* End of Current Buffer
					 *
					 *	If not the current buffer, we search
					 *	the list of windows to see if this
					 *	buffer is visible.
					 */
					else
						{
						struct window *w;

						w = windows;
						while( w != 0 )
							{
							if( w->w_buf == chan->chan_buffer )
								{
								/*
								 *	Buffer is visible,
								 *	do the redisplay
								 */
								chan->chan_reads_to_go_before_redisplay =
									chan->chan_num_reads_before_redisplay;
								do_dsp( 1 );
								savedcant_1line_opt = cant_1line_opt;
								savedcant_1win_opt = cant_1win_opt;
								savedredo_modes = redo_modes;
								}
							w = w->w_next;
							}/* End of search for window */
						}/* End of Visible Buffer( Not Current ) */
					}/* End of Re-Display check_ */
				}/* End of Read Data Loop */

				/*
				 *	Allow another read
				 */
				SetEvent( chan->chan_nt_event );
				}/* End of Service Write Attention */
			else
				{
				struct process_blk *p;

				/*
				 *	Sub-Process wants data: notify its input procedure
				 */
				chan->chan_data_request = 1;
				p = chan->chan_process;
				p->proc_time_state_was_entered = time( 0 );
				RUNNING_TO_BLOCKED( p, BLOCKED_FOR_INPUT );
				if( chan->chan_procedure != 0 )
					{
					current_process = chan->chan_process;
					execute_bound_saved_environment( chan->chan_procedure );
					if( force_redisplay )
						{
						do_dsp( 1 );
						force_redisplay = 0;
						savedcant_1win_opt = cant_1win_opt;
						savedcant_1line_opt = cant_1line_opt;
						savedredo_modes = redo_modes;
						}
					}	/* End of Call Channel Input Procedure */
				}	/* End of Service Read Attention */

			/*
			 *	Decrement pending Channel Interrupts
			 */
			interlock_dec( &pending_channel_io );
			if( pending_channel_io == 0 )
				break;

#if 0
			/*
			 *	If we exited the Loop with LoopCount zero,
			 *	then we must do something about this Channel
			 *	dominating things. We move the Channel to the
			 *	the End of the Channel List and restart the
			 *	Interrupting Channel scan from the beginning
			 *	of the List
			 */
			if( loop_count > 0 )
				{
				struct queue *it;

				queue_validate( &channel_list_head );
				it = queue_remove( &chan->chan_queue );

				queue_validate( &channel_list_head );
				queue_insert( channel_list_head.prev, it );
				}
#endif
			}		/* End of Scan for Interrupting Channels */
		chan = (struct channel *)chan->chan_queue.next;
		}
	/*
	 *	Restore the redisplay optimization variables
	 */
	cant_1line_opt = savedcant_1line_opt;
	cant_1win_opt = savedcant_1win_opt;
	redo_modes = savedredo_modes;
	/*
	 *	Restore the Current Process
	 */
	current_process = saved_current_process;

	return work_done;
	}

#if PIPE_ANONAMOUS
int process_read_nt_pipe( struct channel *chan )
	{
	int status;

	while( chan->chan_thread_exit == 0 )
		{
		unsigned long bytes_moved;

		status = ReadFile
			(
			chan->chan_nt_pipe, chan->chan_data_buffer,
			1, &bytes_moved, NULL
			);
		if( !status )
			break;
		chan->chan_chars_left = 1;
		chan->chan_local_buffer_has_data = 1;
		chan->chan_interrupt = 1;
		interlock_inc( &pending_channel_io );

		/* wait for emacs to absorb the data */
		WaitForSingleObject( chan->chan_nt_semaphore, INFINITE );
		}

	return 0;
	}

int process_write_nt_pipe( struct channel *chan )
	{
	DWORD status;

	for(;;)
		{
		unsigned long bytes_moved;
		/* wait for emacs to generate the data */
		status = WaitForSingleObject( chan->chan_nt_semaphore, INFINITE );
		if( status != WAIT_OBJECT_0 )
			break;
		if( chan->chan_thread_exit )
			break;
		status = WriteFile
			(
			chan->chan_nt_pipe, chan->chan_data_buffer,
			chan->chan_chars_left, &bytes_moved, NULL
			);
		if( !status )
			break;
		chan->chan_chars_left = 1;
		chan->chan_local_buffer_has_data = 1;
		chan->chan_interrupt = 1;
		interlock_inc( &pending_channel_io );

		}

	return 0;
	}

static int semaphore_index = 0;

int start_nt_process( struct process_blk *proc )
	{
	int status;

	HANDLE p = GetCurrentProcess();
	HANDLE child_input, child_output;
	char semaphore_name[128];
	struct channel *ci = &proc->proc_input_channel;
	struct channel *co = &proc->proc_output_channel;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof( si ) ); 
	si.cb = sizeof( si );
	si.dwFlags = 0;

	status = CreatePipe( &ci->chan_nt_pipe, &child_output, NULL, 0 );
	if( !status )
		return 0;
	status = CreatePipe( &child_input, &co->chan_nt_pipe, NULL, 0 );
	if( !status )
		return 0;
	status = DuplicateHandle( p, child_input, p, NULL, 0, 1, 0 );
	if( !status )
		return 0;
	status = DuplicateHandle( p, child_output, p, NULL, 0, 1, 0 );
	if( !status )
		return 0;
	si.hStdInput = child_input;
	si.hStdOutput = child_output;
	si.hStdError = child_output;

	/* make up a unique name */
	semaphore_index++;
	sprintfl( semaphore_name, sizeof(semaphore_name), "Emacs-In-%x-%d", GetCurrentProcessId(), semaphore_index );
	ci->chan_nt_semaphore = CreateSemaphore( NULL, 1, 1, semaphore_name );
	if( ci->chan_nt_semaphore == NULL )
		return 0;
	sprintfl( semaphore_name, sizeof(semaphore_name), "Emacs-Out-%x-%d", GetCurrentProcessId(), semaphore_index );
	co->chan_nt_semaphore = CreateSemaphore( NULL, 0, 1, semaphore_name );
	if( co->chan_nt_semaphore == NULL )
		return 0;

	ci->chan_nt_thread =
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)process_read_nt_pipe,
				ci, 0, &ci->chan_nt_thread_id );
	if( ci->chan_nt_thread == NULL )
		return 0;
	co->chan_nt_thread =
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)process_write_nt_pipe,
				co, 0, &co->chan_nt_thread_id );
	if( co->chan_nt_thread == NULL )
		return 0;

	status = CreateProcess
		(
		NULL,	/* image name */
		"cmd /q",	/* command line */
		NULL,	/* process security */
		NULL,	/* thread security */
		1,	/* inherit */
		0,	/* create flags */
		NULL,	/* environment */
		NULL,	/* cur dir */
		&si,	/* startup information */
		&pi	/* process information */
		);

	CloseHandle( child_input );
	CloseHandle( child_output );

	/*
	 *	Insert both channels into the channel List
	 */
	queue_insert( channel_list_head.prev, &ci->chan_queue );
	queue_insert( channel_list_head.prev, &co->chan_queue );

	proc->proc_process_handle = pi.hProcess;

	return status;
	}

void stop_nt_process( struct process_blk *proc )
	{
	}
#else

int start_nt_process( struct process_blk *proc )
	{
	DWORD status;

	/*
	 *	Insert both channels into the channel List
	 */
	queue_insert( channel_list_head.prev, &proc->proc_input_channel.chan_queue );
	queue_insert( channel_list_head.prev, &proc->proc_output_channel.chan_queue );

	if( !nt_console )
		{
		int h1, i;
		HWND con_w, act_w;
		char con_name[64];

		/* remember the active window as we are about to lose focus */
		act_w = GetActiveWindow();

		/* allocate a console. It will become that active window */
		nt_console = AllocConsole();
		if( !nt_console )
			return 0;
		/* set the title is a unique string */
		sprintfl( u_str(con_name), sizeof(con_name), u_str("Emacs Console (0x%x) about to be hidden..."),
				GetCurrentProcessId() );
		status = SetConsoleTitle( con_name );

		for( i=0; i<100; i++ )
			{
			con_w = FindWindow( "ConsoleWindowClass", con_name );
			if( con_w != 0 )
				break;
			DebugPrintf("Console window does not exist yet. waiting...");

			Sleep( 10 );
			}
		if( con_w == 0 )
			_dbg_msg(u_str("Cannot find console window"));
#if 0
		if( i != 0 )
			_dbg_msg(u_str("Found console window after %dms"), 10*i );
#endif
		if( con_w != 0 )
			{
			/* we do not want to see you! */
			status = ShowWindow( con_w, SW_HIDE );

			/* change the title to reflect that its hidden */
			sprintfl( u_str(con_name), sizeof(con_name), u_str("Emacs Console (0x%x) Hidden."),
				GetCurrentProcessId() );
			status = SetConsoleTitle( con_name );		

			/* if we had an active window then  restore it */
			if( act_w != 0 )
				SetActiveWindow( act_w );
			}

		/* as alloc console does not setup the std handles we do it here */
		con_in_handle = CreateFile
			(
			"CONIN$",
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);
		status = SetStdHandle( STD_INPUT_HANDLE, con_in_handle );
		con_out_handle = CreateFile
			(
			"CONOUT$",
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_EXISTING,
			0,
			NULL
			);
		status = SetStdHandle( STD_OUTPUT_HANDLE, con_out_handle );
		status = SetStdHandle( STD_ERROR_HANDLE, con_out_handle );

		/* setup the CRTL I/O that Dave C's code wants to use */
		h1 = _open_osfhandle((long)con_in_handle, 0);
		status = dup2( h1, 0 );
		close( h1 );
		h1 = _open_osfhandle((long)con_out_handle, 0);
		status = dup2( h1, 1 );
		status = dup2( h1, 2 );
		close( h1 );
		}

	proc->proc_nt_session = CreateSession();
	if( proc->proc_nt_session == NULL )
		return 0;
	if( !ConnectSession( proc->proc_nt_session, proc ) )
		return 0;

	return 1;
	}

void stop_nt_process( struct process_blk *proc )
	{
	queue_remove( &proc->proc_input_channel.chan_queue );
	queue_remove( &proc->proc_output_channel.chan_queue );

	if( proc->proc_nt_session != NULL )
		DeleteSession( proc->proc_nt_session );
	}
#endif


/*
 *
 *	MLisp procedure to start up a sub-process running DCL in order
 *		to later run commands in it.
 *
 */
int start_dcl_process( void )
	{
	unsigned char *process_name;
	int i;
	struct process_blk *proc;

	process_name = getnbstr( u_str("Process name: ") );
	if( process_name == 0 )
		return 0;

	/*
	 *	Verify that we do not have a duplicate process name
	 */
	for( i=0; i<n_processes; i++ )
		if( _str_cmp( process_names[i], process_name ) == 0 )
			{
			error( u_str("Duplicate process name.") );
			return 0;
			}
	/*
	 *	Allocate the Process Structure
	 */
	proc = malloc_struct( process_blk );

	/*
	 *	initialize the process structure
	 */
	proc->proc_nt_session = 0;
	proc->proc_state = PROC_K_BLOCKED_FOR_INPUT;
	proc->proc_time_state_was_entered = time( 0 );
	proc->proc_name = savestr( process_name );
	proc->proc_procedure = 0;

	proc->proc_output_channel.chan_process = proc;
	proc->proc_output_channel.chan_read_channel = 0;
	proc->proc_output_channel.chan_data_request = 1;

	proc->proc_input_channel.chan_process = proc;
	proc->proc_input_channel.chan_read_channel = 1;
	proc->proc_input_channel.chan_num_reads_before_redisplay = 1;
	proc->proc_input_channel.chan_reads_to_go_before_redisplay = 1;
	proc->proc_input_channel.chan_read_channel = 1;

	if( maximum_dcl_buffer_size < 1000 )
		maximum_dcl_buffer_size = 10000;
	if( dcl_buffer_reduction > maximum_dcl_buffer_size - 500
	|| dcl_buffer_reduction < 500 )
		dcl_buffer_reduction = 500;

	proc->proc_input_channel.chan_maximum_buffer_size = maximum_dcl_buffer_size;
	proc->proc_input_channel.chan_buffer_reduction_size = dcl_buffer_reduction;

	/*
	 *	Insert this process in the list of known processes
	 */
	if( n_processes == (n_process_slots - 1) )
		{
		/*
		 *	Out of room - reallocate the Process Lists
		 */
		n_process_slots = n_process_slots + 20;
		process_names = (unsigned char **)realloc( process_names,
					n_process_slots * sizeof(unsigned char *), malloc_type_star_star );
		process_ptrs = (struct process_blk **)realloc( process_ptrs,
					n_process_slots * sizeof(struct process_blk *), malloc_type_star_star );
		}

	/* do the hard work of getting the process going */
	if( !start_nt_process( proc ) )
		{
		stop_nt_process( proc );

		error(u_str("Unable to start the process"));
		return 0;
		}

	/* all is good add this process to the table */
	process_ptrs[n_processes] = proc;
	process_names[n_processes] = proc->proc_name;
	n_processes++;
	process_names[n_processes] = 0;
	process_ptrs[n_processes] = 0;

	/*
	 *	This is now the current process
	 */
	current_process = proc;

	/*
	 *	All done!
	 */
	return 0;
	}

/*
 *
 *	Internal routine to get a Process argument
 *
 */
static struct process_blk * get_process_arg( void )
	{
	int index;
	index = getword( process_names, u_str("Process: ") );

	if( index < 0 )
		{
		error( u_str("No such process.") );
		return 0;
		}
	return process_ptrs[index];
	}

/*
 *
 *	Internal Procedure to get an MLisp procedure argument
 *
 */
static struct bound_name * get_procedure_arg( void )
	{
	int index;
	index = getword( mac_names_ref, u_str("Procedure: ") );

	if( index >= 0
	&& _str_cmp( mac_names[index], u_str("novalue")) != 0 )
		return mac_bodies[index];
	else
		return NULL;
	}

/*
 *
 *	MLisp procedure to send a string to a named process
 *
 */
int send_string_to_process( void )
	{
	struct process_blk *proc;
	struct channel *output_channel;
	unsigned char *cp;

	proc = get_process_arg();
	/*
	 *	Verify the destination process
	 */
	if( proc == NULL )
		return 0;
	/*
	 *	If it exists, make sure we can send to it
	 */
	if( proc->proc_state == PROC_K_DEAD )
		{
		error( u_str("process is dead.") );
		return 0;
		}
	output_channel = &proc->proc_output_channel;
	if( output_channel->chan_buffer != NULL )
		{
		error( u_str("process input has a region attached to it.") );
		return 0;
		}
	if( ! output_channel->chan_data_request )
		{
		error( u_str("process not asking for input.") );
		return 0;
		}
	/*
	 *	Change process state to 'RUNNING'
	 */
	proc->proc_time_state_was_entered = time( 0 );
	BLOCKED_TO_RUNNING( proc );
	/*
	 *	Send the string and re-enable the Read Attention AST
	 */
	cp = getstr( u_str("string: ") );
	if( cp != NULL )
		{
		int len = _str_len( cp );
		if( len > SHELL_BUFFER_SIZE )
			{
			error(u_str("string too long"));
			return 0;
			}
		memcpy( output_channel->chan_data_buffer, cp, len );
		output_channel->chan_chars_left = len;

		/* let the write thread run */
		SetEvent( output_channel->chan_nt_event );

		output_channel->chan_data_request = 0;
		}
	return 0;
	}

/*
 *
 *	MLisp procedure to set the process termination procedure
 *
 */
int set_process_termination_proc( void )
	{
	struct process_blk *proc;
	proc = get_process_arg();

	/*
	 *	Get the Procedure arg and set it
	 */
	if( proc != NULL )
		proc->proc_procedure = get_procedure_arg();
	return 0;
	}

/*
 *
 *	MLisp procedure to set the process output procedure
 *
 */
int set_process_output_procedure( void )
	{
	struct process_blk *proc = get_process_arg();
	int was_null;

	/*
	 *	Get the Procedure arg and set it
	 */
	if( proc == NULL )
		return 0;

	/* record the current state */
	was_null = proc->proc_input_channel.chan_procedure == NULL
		&& proc->proc_input_channel.chan_buffer == NULL;
	proc->proc_input_channel.chan_procedure = get_procedure_arg();
	/* if this is the first time there is somewhere for output to go
	   then release the reader */
	if( was_null && proc->proc_input_channel.chan_procedure != NULL )
		SetEvent( proc->proc_input_channel.chan_nt_event );

	return 0;
	}

/*
 *
 *	MLisp procedure to set the process input procedure
 */
int set_process_input_procedure( void )
	{
	struct process_blk *proc;

	proc = get_process_arg();

	/*
	 *	Get the Procedure arg and set it
	 */
	if( proc != NULL )
		proc->proc_output_channel.chan_procedure = get_procedure_arg();
	return 0;
	}

/*
 *
 *	MLisp procedure to set the process output buffer
 *
 */
int set_process_output_buffer( void )
	{
	unsigned char *cp;
	struct emacs_buffer *buf;
	struct process_blk *proc;
	int was_null;

	proc = get_process_arg();

	/*
	 *	Verify the Process
	 */
	if( proc == NULL )
		return 0;
	/*
	 *	Get the Buffer
	 */
	cp = getescword( buf_names, u_str("Buffer: "));
	if( cp == NULL )
		return 0;
	if( (buf = find_bf( cp )) == NULL )
		buf = new_bf( cp );

	/* record the current state */
	was_null = proc->proc_input_channel.chan_procedure == NULL
		&& proc->proc_input_channel.chan_buffer == NULL;
	/*
	 *	Set the Process Buffer
	 */
	proc->proc_input_channel.chan_buffer = buf;

	/* if this is the first time there is somewhere for output to go
	   then release the reader */
	if( was_null && proc->proc_input_channel.chan_buffer != NULL )
		SetEvent( proc->proc_input_channel.chan_nt_event );

	return 0;
	}

/*
 *
 *	MLisp procedure to Kill a named process
 *
 */
int kill_process( void )
	{
	struct process_blk *proc;

	proc = get_process_arg();

	if( proc == 0 )
		return 0;

	/*
	 *	Do a DelPrc on the named process(Its termination message
	 *		will arrive later)
	 */
	if( proc->proc_nt_session == NULL )
		{
		proc->proc_state = PROC_K_DEAD;
		interlock_inc( &terminating_process );
		interlock_inc( &pending_channel_io );
		}
	else
		{
		DeleteSession( proc->proc_nt_session );
		proc->proc_nt_session = NULL;
		}

	return 0;
	}

void proc_de_ref_buf( struct emacs_buffer *b )
	{
	int i;

	for( i=0; i<n_processes; i++ )
		{
		struct process_blk *proc;
		proc = process_ptrs[i];
		if( proc != 0 )
			{
			if( proc->proc_output_channel.chan_buffer == b )
				proc->proc_output_channel.chan_buffer = NULL;
			if( proc->proc_input_channel.chan_buffer == b )
				proc->proc_input_channel.chan_buffer = NULL;
			}
		}
	}

void kill_processes( void )
	{
	struct process_blk *proc;
	int i;

	for( i=0; i<n_processes; i++ )
		{
		proc = process_ptrs[i];
		if( proc->proc_state != PROC_K_DEAD )
			{
			DeleteSession( proc->proc_nt_session );
			proc->proc_nt_session = NULL;
			}
		}

	/* lose that console window */
	if( nt_console )
		{
		CloseHandle( con_in_handle );
		CloseHandle( con_out_handle );
		FreeConsole();
		}
	}

int count_processes( void )
	{
	return n_processes != 0;
	}

/*
 *
 *	MLisp procedure to suspend a named process
 *
 */
int pause_process( void )
	{
#if 0
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	/*
	 *	Make sure it is not already Suspended
	 */
	if( IS_PAUSED( proc ) )
		{
		error( u_str("process is already paused.") );
		return 0;
		}
	/*
	 *	Do a Suspnd on the named process and set its state to paused
	 */
	sys$suspnd( &proc->proc_process_id, NULL, 0 );
	proc->proc_time_state_was_entered = time( 0 );
	RUNNING_TO_PAUSED( proc );
#endif

	return 0;
	}

/*
 *	MLisp procedure to Resume a named process
 *
 */
int resume_process( void )
	{
#if 0
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	/*
	 *	Make sure it is not already Running
	 */
	if( IS_RUNNING( proc ) )
		{
		error( u_str("process was not paused.") );
		return 0;
		}
	/*
	 *	Do a resume the named process and set its state to running
	 */
	sys$resume( &proc->proc_process_id, NULL );
	proc->proc_time_state_was_entered = time( 0 );
	PAUSED_TO_RUNNING( proc );
#endif

	return 0;
	}

/*
 *	MLisp procedure to Force Exit a named process
 *
 */
int force_exit_process( void )
	{
#if 0
	int code;
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify Process existance */

	/*
	 *	Get the Exit Code
	 */
	code = getnum( u_str("exit code: ") );
	/*
	 *	Do a Force Exit on the named process
	 *	Note: In EUNICE, a -ve Force Exit code is
	 *		taken to be a signal( once negated ),
	 *		so you can send signals with this.
	 */
	sys$forcex( &proc->proc_process_id, NULL, code );
#endif

	return 0;
	}

/*
 *	Set the named process as current
 *
 */
int set_current_process( void )
	{
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	/*
	 *	Just set CurrentProcess
	 */
	current_process = proc;
	return 0;
	}

/*
 *
 *	MLisp procedure to return the Current Process Name
 *
 */
int current_process_name( void )
	{
	/*
	 *	Just return the name string
	 */
	ml_value->exp_type = ISSTRING;
	if( current_process != 0 )
		{
		ml_value->exp_v.v_string = current_process->proc_name;
		ml_value->exp_int = _str_len( ml_value->exp_v.v_string );
		}
	else
		{
		ml_value->exp_v.v_string = u_str( "" );
		ml_value->exp_int = 0;
		}
	ml_value->exp_release = 0;
	return 0;
	}

/*
 *
 *	MLisp procedure to change a process name
 *
 */
int set_process_name( void )
	{
	unsigned char *cp;
	int process_index = 0;
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	/*
	 *	Get the new name
	 */
	cp = getnbstr( u_str("new name: ") );
	if( cp != 0 )
		{
		int i;

		/*
		 *	search for duplicate process name(and our
		 *		process index)
		 */
		for( i=0; i<n_processes; i++ )
			{
			if( _str_cmp( process_names[i], cp ) == 0 )
				{
				if( process_ptrs[i] != proc )
					error( u_str("duplicate process name.") );
				return 0;
				}
			if( process_ptrs[i] == proc )
				process_index = i;
			}
		/*
		 *	The New name is unique, deallocate the old one
		 *		and setup the new one
		 */
		free( proc->proc_name );
		process_names[process_index] = proc->proc_name =
			savestr( cp );
		}
	return 0;
	}

/*
 *
 *	MLisp procedure to return any output from a process
 *
 */
int process_output( void )
	{
	struct channel *chan;
	struct process_blk *proc;
	proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	/*
	 *	Make sure there is output to return
	 */
	chan = &proc->proc_input_channel;
	if( ! chan->chan_local_buffer_has_data )
		{
		error( u_str("no unprocessed data from this process.") );
		return 0;
		}
	/*
	 *	Flag the data as having been snarfed
	 */
	chan->chan_local_buffer_has_data = 0;
	/*
	 *	Return the data as a string
	 */
	ml_value->exp_type = ISSTRING;
	ml_value->exp_v.v_string = chan->chan_data_buffer;
	ml_value->exp_release = 0;
	ml_value->exp_int = chan->chan_chars_left;
	return 0;
	}

/*
 *
 *	MLisp procedure to put a list of processes and their states into
 *		the buffer 'Process List'
 *
 */
static unsigned char *proc_states[] =
	{
	u_str( "RUNNING" ),
	u_str( "PAUSED" ),
	u_str( "IN-WAIT" ),
	u_str( "PAUSED" ),
	u_str( "OUT-WAIT" ),
	u_str( "PAUSED" ),
	u_str( "DEAD" )
	};

int list_processes( void )
	{
	struct emacs_buffer *old_buffer;
	struct process_blk *proc;
	char *cp;
	unsigned char line[100];
	int i;

	old_buffer = bf_cur;

	scratch_bfn( u_str("process list"), interactive );
	ins_str(u_str("Process               State   Time      Buffer          In Proc         Out Proc\n"));
	ins_str(u_str("-------               -----   ----      ------          -------         --------\n"));
/*	              "..................... ....... ... ..... ............... ............... ............... */
	for( i=0; i<n_processes; i++ )
		{
		proc = process_ptrs[i];
		cp = ctime( &proc->proc_time_state_was_entered );
		cp[3] = 0;
		cp[16] = 0;
		sprintfl
		(
		line, sizeof( line ),
		u_str( "%-21s %-7s %3s %5s %-15s %-15s %-15s\n" ),
		proc->proc_name,
		proc_states[ proc->proc_state ],
		cp,
		&cp[11],
		(proc->proc_input_channel.chan_buffer != NULL ?
			proc->proc_input_channel.chan_buffer->b_buf_name 
		:
			u_str( "[none]" )),
		(proc->proc_output_channel.chan_buffer != NULL ?
			proc->proc_output_channel.chan_buffer->b_buf_name 
		:
			u_str( "[none]" )),
		(proc->proc_input_channel.chan_procedure != NULL ?
			proc->proc_input_channel.chan_procedure->b_proc_name 
		:
			u_str( "[none]")) );
		ins_str( line );
		}

	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old_buffer );
	window_on( bf_cur );
	return 0;
	}

/*
 *
 *	MLisp procedure to wait for a process input request
 *
 */
int wait_for_process_input_request( void )
	{
	struct channel *chan;
	struct process_blk *proc = get_process_arg();

	if( proc == 0 )
		return 0;	/* Verify process existance */

	ml_value->exp_type = ISINTEGER;

	/*
	 *	Keep looping in a scheduler loop until something happens
	 */
	chan = &proc->proc_output_channel;

	while( ! chan->chan_data_request )
		{
		wake_queued = 0;

		if( input_pending != 0 )
			{
			if( timer_interrupt_occurred != 0 )
				process_timer_interrupts();
			else
				ml_value->exp_int = 0;
			return 0;
			}

		if( pending_channel_io != 0 )
			{
			process_channel_interrupts();
			}
		else
			wait_for_activity();
		}

	ml_value->exp_int = 1;
	return 0;
	}

int vms_load_averages( void )
	{
	// use NT process stuff here
	return 0;
	}

/*
 *
 *	initialize the VMS-specific world
 *
 */
void init_vms( void )
	{
	/*
	 *	initialize the Process Lists
	 */
	if( process_ptrs == NULL )
		{
		process_ptrs = (struct process_blk **)malloc( n_process_slots * sizeof( struct process_blk * ), malloc_type_star_star );
		process_ptrs[0] = 0;
		}
	if( process_names == NULL )
		{
		process_names = (unsigned char **)malloc( n_process_slots * sizeof( unsigned char * ), malloc_type_star_star );
		process_names[0] = 0;
		}

	if( channel_list_head.next == NULL )
		queue_init( &channel_list_head );
	}

/*
 * Special routine for restore. Kills off all processes
 * except a broadcast-message processor. This processor is
 * re-started
 */
void restore_vms( void )
	{
	int i;
	struct process_blk *proc;

	init_vms();


	/*
	 * Restore the sub-processes
	 *
	 * All processes are re-started. Note, however, that the context
	 * of the process is that inherited from this incarnation rather
	 * than that of the original process
	 */
	for( i=0; i<n_processes; i++ )
		{
		proc = process_ptrs[i];
		}
	}
