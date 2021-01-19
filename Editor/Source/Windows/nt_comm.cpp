//
//     Copyright(c) 1994-2021 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

SystemExpressionRepresentationIntPositive maximum_shell_buffer_size( 10000 );
SystemExpressionRepresentationIntPositive shell_buffer_reduction( 500 );

#include <nt_comm.h>

#ifdef EMACS_PYTHON_EXTENSION
#include <emacs_python_interface.h>
#endif

int process_channel_interrupts( void );
static EmacsProcess * get_process_arg( void );
static BoundName *get_procedure_arg( void );
int send_string_to_process( void );
int set_process_termination_proc( void );
int set_process_output_procedure( void );
int set_process_input_procedure( void );
int set_process_output_buffer( void );
int kill_process( void );
void kill_processes( void );
int count_processes( void );
int pause_process( void );
int resume_process( void );
int force_exit_process( void );
int set_current_process( void );
int current_process_name( void );
int set_process_name( void );
int process_output( void );
int list_processes( void );
int wait_for_process_input_request( void );
int vms_load_averages( void );
void init_vms( void );
void restore_vms( void );

volatile int terminating_process;    // Process terminated
unsigned int base_priority;

QueueHeader<ProcessChannel> channel_list_head;
EmacsProcess *current_process = NULL;

#pragma warning( disable : 4355 )

EmacsProcess::EmacsProcess( const EmacsString &name )
    : EmacsProcessCommon( name )
    , proc_output_channel( this )
    , proc_input_channel( this )
    , proc_nt_session()
    , proc_procedure( NULL )
    , proc_time_state_was_entered( time(0) )
    , proc_state( BLOCKED_FOR_INPUT )
{
    //
    //    initialize the process structure
    //
    proc_output_channel.chan_read_channel = 0;
    proc_output_channel.chan_data_request = 1;

    proc_input_channel.chan_read_channel = 1;
    proc_input_channel.chan_num_reads_before_redisplay = 1;
    proc_input_channel.chan_reads_to_go_before_redisplay = 1;

    if( maximum_shell_buffer_size < 1000 )
        maximum_shell_buffer_size = 10000;
    if( shell_buffer_reduction > maximum_shell_buffer_size - 500
    || shell_buffer_reduction < 500 )
        shell_buffer_reduction = 500;

    proc_input_channel.chan_maximum_buffer_size = maximum_shell_buffer_size;
    proc_input_channel.chan_buffer_reduction_size = shell_buffer_reduction;
}

EmacsProcess::~EmacsProcess()
{ }

ProcessChannel::ProcessChannel( EmacsProcess *owner )
: chan_process( owner )
, chan_buffer( NULL )
, chan_procedure( NULL )
, chan_end_of_data_mark()
, chan_chars_left( 0 )
, chan_num_reads_before_redisplay( 0 )
, chan_reads_to_go_before_redisplay( 0 )
, chan_maximum_buffer_size( 0 )
, chan_buffer_reduction_size( 0 )
, chan_interrupt( 0 )
, chan_read_channel( 0 )
, chan_channel_open( 0 )
, chan_local_buffer_has_data( 0 )
, chan_data_request( 0 )
, chan_nt_event( 0 )
, chan_data_buffer( NULL )
{ }

ProcessChannel::~ProcessChannel()
{ }

bool EmacsProcess::nt_console(false);
HANDLE EmacsProcess::con_in_handle;
HANDLE EmacsProcess::con_out_handle;

//
//
//    Routine to Process Channel Interrupts( called from Scheduler )
//
//
int process_channel_interrupts( void )
{
    if( pending_channel_io == 0 )
        return 0;

    int work_done = 0;
    EmacsProcess *saved_current_process = current_process;
    EmacsBuffer *saved_buffer = NULL;

    int savedcant_1line_opt = cant_1line_opt;    // Be nice to refresher
    int savedcant_1win_opt = cant_1win_opt;    // ditto
    int savedredo_modes = redo_modes;        // ditto

    //
    //    Process any terminating processes
    //
    while( terminating_process != 0 )
    {
        interlock_dec( &terminating_process );
        interlock_dec( &pending_channel_io );

        for( int i=0; i<EmacsProcess::name_table.entries(); i++ )
        {
            EmacsProcess *proc = EmacsProcess::name_table.value(i);
            if( proc->proc_state == EmacsProcess::DEAD )
            {
                work_done = 1;

                if( saved_current_process == proc )
                    saved_current_process = 0;
                if( proc->proc_procedure != 0 )
                {
                    //
                    //    Call the term procedure
                    //
                    current_process = proc;
                    execute_bound_saved_environment( proc->proc_procedure );
                    if( force_redisplay )
                    {
                        theActiveView->do_dsp();
                        force_redisplay = 0;
                        savedcant_1win_opt = cant_1win_opt;
                        savedcant_1line_opt = cant_1line_opt;
                        savedredo_modes = redo_modes;
                    }
                }

                proc->stopProcess();

                // kill off the process
                delete proc;
            }
        }
    }

    //
    //    Scan the list of channels to see which ones require attention
    //
    QueueIterator<ProcessChannel> it( channel_list_head );
    while( it.next() )
    {
        ProcessChannel *chan = it.value();
        //
        //    Service all Interrupting Channels
        //
        if( chan->chan_interrupt )
        {
            //
            //    Turn Off the interrupt Flag
            //
            chan->chan_interrupt = 0;
            //
            //    Service read/write channel
            //
            if( chan->chan_read_channel )
            {
                //
                //    Read Channel, Get the data
                //
            {
                //
                //    Mung the input data( if necessary )
                //
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
                            // remove the CR
                            memmove( &s[0], &s[1], n );
                            n--;
                        }
                        else
                        {
                            // change CR to NL
                            s[0] = '\n';
                        }
                    s++;
                }
            }

                //
                //    If there is a buffer associated with
                //    this process output, make it current
                //
                if( chan->chan_buffer.bufferValid() )
                {
                    saved_buffer = bf_cur;
                    chan->chan_buffer.set_bf();
                    set_dot( bf_cur->unrestrictedSize() + 1 );
                }
                //
                //    If there is no procedure to call, assume
                //    that we must stuff into Buffer
                //
                if( chan->chan_procedure == NULL )
                {
                    //
                    //    If no buffer, get out of here!
                    //
                    if( !chan->chan_buffer.bufferValid() )
                        break;
                    //
                    //    Insert the Data
                    //
                    bf_cur->ins_cstr( chan->chan_data_buffer, chan->chan_chars_left );
                    //
                    //    If buffer has overflowed
                    //    Delete start of buffer
                    //
                    if( (bf_cur->unrestrictedSize()) > chan->chan_maximum_buffer_size )
                    {
                        bf_cur->del_frwd( 1, chan->chan_buffer_reduction_size );
                        set_dot( bf_cur->unrestrictedSize() + 1 );
                    }
                }// End of Put Data into Buffer
                else
                {
                    //
                    //    Call the procedure
                    //
                    current_process = chan->chan_process;
                    chan->chan_local_buffer_has_data = 1;
                    execute_bound_saved_environment( chan->chan_procedure );
                    chan->chan_local_buffer_has_data = 0;
                    if( force_redisplay )
                    {
                        theActiveView->do_dsp();
                        force_redisplay = 0;
                        savedcant_1win_opt = cant_1win_opt;
                        savedcant_1line_opt = cant_1line_opt;
                        savedredo_modes = redo_modes;
                    }
                }// End of Call Procedure to deal with Data

                //
                //    If we had switched buffers, restore old one
                //
                if( saved_buffer != NULL )
                {
                    //
                    //    Set the Mark
                    //
                    bf_cur->set_mark( dot, 0, false );
                    //
                    //    Go back to original buffer
                    //
                    saved_buffer->set_bf();
                }
                //
                //    Deal with possible re-displaying
                //    (We take any possible excuse to
                //    not redisplay)
                //
                chan->chan_reads_to_go_before_redisplay--;
                if( input_pending == 0
                && (chan->chan_reads_to_go_before_redisplay <= 0) )
                {
                    //
                    //    If it is the current buffer, we
                    //        always redisplay
                    //
                    if( bf_cur == chan->chan_buffer.buffer() )
                    {
                        chan->chan_reads_to_go_before_redisplay = 1;
                        theActiveView->do_dsp();
                        savedcant_1line_opt = cant_1line_opt;
                        savedcant_1win_opt = cant_1win_opt;
                        savedredo_modes = redo_modes;
                    }// End of Current Buffer
                    //
                    //    If not the current buffer, we search
                    //    the list of windows to see if this
                    //    buffer is visible.
                    //
                    else
                    {
                        EmacsWindow *w;

                        w = theActiveView->windows.windows;
                        while( w != 0 )
                        {
                            if( w->w_buf == chan->chan_buffer.buffer() )
                            {
                                //
                                //    Buffer is visible,
                                //    do the redisplay
                                //
                                chan->chan_reads_to_go_before_redisplay =
                                    chan->chan_num_reads_before_redisplay;
                                theActiveView->do_dsp();
                                savedcant_1line_opt = cant_1line_opt;
                                savedcant_1win_opt = cant_1win_opt;
                                savedredo_modes = redo_modes;
                            }
                            w = w->w_next;
                        }// End of search for window
                    }// End of Visible Buffer( Not Current )
                }// End of Re-Display check_
            }// End of Read Data Loop

                //
                //    Allow another read
                //
                SetEvent( chan->chan_nt_event );
            }// End of Service Write Attention
            else
            {
                EmacsProcess *p;

                //
                //    Sub-Process wants data: notify its input procedure
                //
                chan->chan_data_request = 1;
                p = chan->chan_process;
                p->proc_time_state_was_entered = time( 0 );
                p->RUNNING_TO_BLOCKED( EmacsProcess::BLOCKED_FOR_INPUT );
                if( chan->chan_procedure != 0 )
                {
                    current_process = chan->chan_process;
                    execute_bound_saved_environment( chan->chan_procedure );
                    if( force_redisplay )
                    {
                        theActiveView->do_dsp();
                        force_redisplay = 0;
                        savedcant_1win_opt = cant_1win_opt;
                        savedcant_1line_opt = cant_1line_opt;
                        savedredo_modes = redo_modes;
                    }
                }    // End of Call Channel Input Procedure
            }    // End of Service Read Attention

            //
            //    Decrement pending Channel Interrupts
            //
            interlock_dec( &pending_channel_io );
            if( pending_channel_io == 0 )
                break;

#if 0
            //
            //    If we exited the Loop with LoopCount zero,
            //    then we must do something about this Channel
            //    dominating things. We move the Channel to the
            //    the End of the Channel List and restart the
            //    Interrupting Channel scan from the beginning
            //    of the List
            //
            if( loop_count > 0 )
            {
                Queue *it;

                queue_validate( &channel_list_head );
                it = queue_remove( &chan->chan_queue );

                queue_validate( &channel_list_head );
                queue_insert( channel_list_head.prev, it );
            }
#endif
        }        // End of Scan for Interrupting Channels
        chan = chan->queueNext();
    }
    //
    //    Restore the redisplay optimization variables
    //
    cant_1line_opt = savedcant_1line_opt;
    cant_1win_opt = savedcant_1win_opt;
    redo_modes = savedredo_modes;
    //
    //    Restore the Current Process
    //
    current_process = saved_current_process;

    return work_done;
}


int EmacsProcess::startProcess( EmacsString &error_detail )
{
    DWORD status;


    //
    //    Insert both channels into the channel List
    //
    channel_list_head.queueInsertAtTail( &proc_input_channel );
    channel_list_head.queueInsertAtTail( &proc_output_channel );


    //
    //    See if this is one of the special process names
    //
#ifdef EMACS_PYTHON_EXTENSION
#if EMACS_PYTHON_EXTENSION_THREADED
    if( proc_name == "Python Console" )
    {
        EmacsThread *thread_object = new EmacsPythonThread( this );
        if( !proc_nt_session.createThreadSession( thread_object, error_detail ) )
        {
            delete thread_object;
            return 0;
        }
    }
    else
#endif
#endif
    {
        if( cli_name.isNull() )
        {
            error_detail = "cli-name is not set";
            return 0;
        }

        if( !nt_console )
        {
            int i;
            HWND con_w = NULL;
            HWND act_w;
            HWND fore_w;

            char con_name[64];

            // remember the active window as we are about to lose focus
            act_w = GetActiveWindow();
            fore_w = GetForegroundWindow();

            // allocate a console. It will become that active window
            nt_console = AllocConsole() == TRUE;
            if( !nt_console )
            {
                error_detail = "Unable to allocate a console";
                return 0;
            }

            // set the title is a unique string
            sprintf( con_name, "Emacs Console (0x%x) about to be hidden...", GetCurrentProcessId() );
            status = SetConsoleTitleA( con_name );

            for( i=0; i<100; i++ )
            {
                con_w = FindWindowA( "ConsoleWindowClass", con_name );
                if( con_w != NULL )
                    break;
#if DBG_TMP
//                _dbg_msg("Console window does not exist yet. waiting...");
#endif
                Sleep( 10 );
            }
            if( con_w == NULL )
                _dbg_msg("Cannot find console window");
#if DBG_TMP
//            if( i != 0 )
//                _dbg_msg( FormatString("Found console window after %dms") << 10*i );
#endif
            if( con_w != 0 )
            {
                // we do not want to see you!
                status = ShowWindow( con_w, SW_HIDE );

                // change the title to reflect that its hidden
                sprintf( con_name, "Emacs Console (0x%x) Hidden.",
                    GetCurrentProcessId() );
                status = SetConsoleTitleA( con_name );

                // if we had an active window then  restore it
                if( act_w != 0 )
                    SetActiveWindow( act_w );
                if( fore_w != 0 )
                    SetForegroundWindow( fore_w );
            }

            // as alloc console does not setup the std handles we do it here
            con_in_handle = CreateFile
                (
                L"CONIN$",
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
                L"CONOUT$",
                GENERIC_READ|GENERIC_WRITE,
                FILE_SHARE_READ|FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL
                );
            status = SetStdHandle( STD_OUTPUT_HANDLE, con_out_handle );
            status = SetStdHandle( STD_ERROR_HANDLE, con_out_handle );
        }

        //
        //    Start a process
        //
        if( !proc_nt_session.createProcessSession( error_detail ) )
            return 0;
    }

    if( !proc_nt_session.connectSession( this, error_detail ) )
        return 0;

    return 1;
}

void EmacsProcess::stopProcess(void)
{
    proc_input_channel.queue_remove();
    proc_output_channel.queue_remove();

    proc_nt_session.deleteSession();
}


//
//
//    MLisp procedure to start up a sub-process running DCL in order
//        to later run commands in it.
//
//
int start_dcl_process( void )
{
    EmacsString process_name = getnbstr( "Process name: " );

    //
    //    Verify that we do not have a duplicate process name
    //
    if( EmacsProcess::name_table.find( process_name ) != NULL )
    {
        error( "Duplicate process name." );
        return 0;
    }

    //
    //    Allocate the Process Structure
    //
    EmacsProcess *proc = EMACS_NEW EmacsProcess( process_name );


    // do the hard work of getting the process going
    EmacsString error_detail;
    if( !proc->startProcess( error_detail ) )
    {
        proc->stopProcess();

        delete proc;

        error( FormatString("Unable to start the process %s: %s") << cli_name.asString() << error_detail );
        return 0;
    }

    //
    //    This is now the current process
    //
    current_process = proc;

    //
    //    All done!
    //
    return 0;
}

//
// Return the process' status
//    -1 - not an active process
//     0 - a stopped process
//     1 - a running process
//
int process_status(void)
{
    EmacsString proc_name = getstr( "Process: " );
    EmacsProcess *process = EmacsProcess::name_table.find( proc_name );
    if( process == NULL )
        ml_value = int(-1);
    else
        if( process->IS_RUNNING() )
            ml_value = int(1);
        else
            ml_value = int(0);

    return 0;
}

int process_end_of_output( void )
{
    EmacsString proc_name = getstr( ": process-end-of-output for process: " );
    EmacsProcess *process = EmacsProcess::name_table.find( proc_name );
    if( process == NULL )
    {
        error("process not found");
        return 0;
    }
    if( !process->proc_input_channel.chan_end_of_data_mark.isSet() )
    {
        error("process-end-of-output marker is not set");
        return 0;
    }

    Marker *m = EMACS_NEW Marker( process->proc_input_channel.chan_end_of_data_mark );
    ml_value = m;
    return 0;
}

//
//
//    Internal routine to get a Process argument
//
//
static EmacsProcess * get_process_arg( void )
{
    EmacsString name;
    if( cur_exec == NULL )
        EmacsProcess::name_table.get_word_interactive( "Process: ", name );
    else
        EmacsProcess::name_table.get_word_mlisp( name );

    EmacsProcess *proc = EmacsProcess::name_table.find( name );
    if( proc == NULL )
    {
        error( "No such process." );
        return 0;
    }
    return proc;
}

//
//
//    Internal Procedure to get an MLisp procedure argument
//
//
static BoundName * get_procedure_arg( void )
{
    BoundName *proc = getword( BoundName::, "Procedure: " );

    if( proc == NULL )
        return NULL;
    if( proc->b_proc_name == "novalue" )
        return NULL;
    return proc;
}

//
//
//    MLisp procedure to send a string to a named process
//
//
int send_string_to_process( void )
{
    EmacsProcess *proc = get_process_arg();
    //
    //    Verify the destination process
    //
    if( proc == NULL )
        return 0;
    //
    //    If it exists, make sure we can send to it
    //
    if( proc->proc_state == EmacsProcess::DEAD )
    {
        error( "process is dead." );
        return 0;
    }

    ProcessChannel *output_channel = &proc->proc_output_channel;
    if( output_channel->chan_buffer.bufferValid() )
    {
        error( "process input has a region attached to it." );
        return 0;
    }
    if( ! output_channel->chan_data_request )
    {
        error( "process not asking for input." );
        return 0;
    }

    //
    //    Change process state to 'RUNNING'
    //
    proc->proc_time_state_was_entered = time( 0 );
    proc->BLOCKED_TO_RUNNING();

    //
    //    Send the string and re-enable the Read Attention AST
    //
    EmacsString cp = getstr( "String: " );
    int len = cp.utf8_data_length();
    if( len > SHELL_BUFFER_SIZE )
    {
        error( "String too long");
        return 0;
    }
    memcpy( output_channel->chan_data_buffer, cp.utf8_data(), len );
    output_channel->chan_chars_left = len;

    // let the write thread run
    SetEvent( output_channel->chan_nt_event );

    output_channel->chan_data_request = 0;

    return 0;
}

//
//
//    MLisp procedure to set the process termination procedure
//
//
int set_process_termination_proc( void )
{
    EmacsProcess *proc = get_process_arg();

    //
    //    Get the Procedure arg and set it
    //
    if( proc != NULL )
        proc->proc_procedure = get_procedure_arg();

    return 0;
}

//
//
//    MLisp procedure to set the process output procedure
//
//
int set_process_output_procedure( void )
{
    EmacsProcess *proc = get_process_arg();
    int was_null;

    //
    //    Get the Procedure arg and set it
    //
    if( proc == NULL )
        return 0;

    // record the current state
    was_null = proc->proc_input_channel.chan_procedure == NULL
        && !proc->proc_input_channel.chan_buffer.bufferValid();
    proc->proc_input_channel.chan_procedure = get_procedure_arg();
    // if this is the first time there is somewhere for output to go
    // then release the reader
    if( was_null && proc->proc_input_channel.chan_procedure != NULL )
        SetEvent( proc->proc_input_channel.chan_nt_event );

    return 0;
}

//
//
//    MLisp procedure to set the process input procedure
//
int set_process_input_procedure( void )
{
    EmacsProcess *proc = get_process_arg();

    //
    //    Get the Procedure arg and set it
    //
    if( proc != NULL )
        proc->proc_output_channel.chan_procedure = get_procedure_arg();

    return 0;
}

//
//
//    MLisp procedure to set the process output buffer
//
//
int set_process_output_buffer( void )
{
    EmacsProcess *proc = get_process_arg();

    //
    //    Verify the Process
    //
    if( proc == NULL )
        return 0;
    //
    //    Get the Buffer
    //
    EmacsString cp;
    getescword( EmacsBuffer::, "Buffer: ", cp );
    if( cp.isNull() )
        return 0;

    EmacsBuffer *buf = EmacsBuffer::find( cp );
    if( buf == NULL )
        buf  = EMACS_NEW EmacsBuffer( cp );

    // record the current state
    int was_null = proc->proc_input_channel.chan_procedure == NULL
            && !proc->proc_input_channel.chan_buffer.bufferValid();
    //
    //    Set the Process Buffer
    //
    proc->proc_input_channel.chan_buffer.buffer( buf );

    // if this is the first time there is somewhere for output to go
    // then release the reader
    if( was_null && proc->proc_input_channel.chan_buffer.bufferValid() )
        SetEvent( proc->proc_input_channel.chan_nt_event );

    return 0;
}

//
//
//    MLisp procedure to Kill a named process
//
//
int kill_process( void )
{
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;

    //
    //    Do a DelPrc on the named process(Its termination message
    //        will arrive later)
    //
    if( !proc->proc_nt_session.initialised )
    {
        proc->proc_state = EmacsProcess::DEAD;
        interlock_inc( &terminating_process );
        interlock_inc( &pending_channel_io );
    }
    else
    {
        proc->proc_nt_session.deleteSession();
    }

    return 0;
}

void kill_processes( void )
{
    for( int i=0; i<EmacsProcess::name_table.entries(); i++ )
    {
        EmacsProcess *proc = EmacsProcess::name_table.value(i);

        if( proc->proc_state != EmacsProcess::DEAD )
        {
            proc->proc_nt_session.deleteSession();
        }
    }

    // lose that console window
    if( EmacsProcess::nt_console )
    {
        CloseHandle( EmacsProcess::con_in_handle );
        CloseHandle( EmacsProcess::con_out_handle );
        FreeConsole();
    }
}

int count_processes( void )
{
    return EmacsProcess::name_table.entries() != 0;
}

//
//
//    MLisp procedure to suspend a named process
//
//
int pause_process( void )
{
#if 0
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify process existance

    //
    //    Make sure it is not already Suspended
    //
    if( IS_PAUSED( proc ) )
    {
        error( "process is already paused." );
        return 0;
    }
    //
    //    Do a Suspnd on the named process and set its state to paused
    //
    sys$suspnd( &proc->proc_process_id, NULL, 0 );
    proc->proc_time_state_was_entered = time( 0 );
    RUNNING_TO_PAUSED( proc );
#endif

    return 0;
}

//
//    MLisp procedure to Resume a named process
//
//
int resume_process( void )
{
#if 0
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify process existance

    //
    //    Make sure it is not already Running
    //
    if( IS_RUNNING( proc ) )
    {
        error( "process was not paused." );
        return 0;
    }
    //
    //    Do a resume the named process and set its state to running
    //
    sys$resume( &proc->proc_process_id, NULL );
    proc->proc_time_state_was_entered = time( 0 );
    PAUSED_TO_RUNNING( proc );
#endif

    return 0;
}

//
//    MLisp procedure to Force Exit a named process
//
//
int force_exit_process( void )
{
#if 0
    int code;
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify Process existance

    //
    //    Get the Exit Code
    //
    code = getnum( "exit code: " );
    //
    //    Do a Force Exit on the named process
    //    Note: In EUNICE, a -ve Force Exit code is
    //        taken to be a signal( once negated ),
    //        so you can send signals with this.
    //
    sys$forcex( &proc->proc_process_id, NULL, code );
#endif

    return 0;
}

//
//    Set the named process as current
//
//
int set_current_process( void )
{
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify process existance

    //
    //    Just set CurrentProcess
    //
    current_process = proc;

    return 0;
}

//
//
//    MLisp procedure to return the Current Process Name
//
//
int current_process_name( void )
{
    //
    //    Just return the name string
    //
    if( current_process != NULL )
        ml_value = current_process->proc_name;
    else
        ml_value = EmacsString::null;

    return 0;
}

//
//
//    MLisp procedure to change a process name
//
//
int set_process_name( void )
{
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify process existance

    //
    //    Get the new name
    //
    EmacsString process_name = getnbstr( "new name: " );
    //
    //    search for duplicate process name(and our
    //        process index)
    //
    if( EmacsProcess::name_table.find( process_name ) != NULL )
    {
        error( "Duplicate process name." );
        return 0;
    }
    //
    //    The New name is unique, deallocate the old one
    //        and setup the new one
    //
    EmacsProcess::name_table.remove( proc->proc_name );
    proc->proc_name = process_name;
    EmacsProcess::name_table.add( process_name, proc );

    return 0;
}

//
//
//    MLisp procedure to return any output from a process
//
//
int process_output( void )
{
    EmacsProcess *proc = get_process_arg();

    if( proc == NULL )
        return 0;    // Verify process existance

    //
    //    Make sure there is output to return
    //
    ProcessChannel *chan = &proc->proc_input_channel;
    if( ! chan->chan_local_buffer_has_data )
    {
        error( "No unprocessed data from this process." );
        return 0;
    }
    //
    //    Flag the data as having been snarfed
    //
    chan->chan_local_buffer_has_data = 0;
    //
    //    Return the data as a string
    //
    ml_value = EmacsString( EmacsString::keep, chan->chan_data_buffer, chan->chan_chars_left );
    return 0;
}

//
//
//    MLisp procedure to put a list of processes and their states into
//        the buffer 'Process List'
//
//
static char *proc_states[] =
{
    "RUNNING",
    "PAUSED",
    "IN-WAIT",
    "PAUSED",
    "OUT-WAIT",
    "PAUSED",
    "DEAD"
};

int list_processes( void )
{
    EmacsBufferRef old_buffer( bf_cur );

    EmacsBuffer::scratch_bfn( "Process list", interactive() );
    bf_cur->ins_str("Process               State   Time      Buffer          In Proc         Out Proc\n"
                    "-------               -----   ----      ------          -------         --------\n");

    for( int i=0; i<EmacsProcess::name_table.entries(); i++ )
    {
        EmacsProcess *proc = EmacsProcess::name_table.value(i);

        char *cp = ctime( &proc->proc_time_state_was_entered );
        cp[3] = 0;      // isolate the day
        cp[16] = 0;     // isolate the time

        bf_cur->ins_cstr(
            FormatString("%-21s %-7s %3s %5s %-15s %-15s %-15s\n") <<
            proc->proc_name <<
            proc_states[ proc->proc_state ] <<
            cp <<
            &cp[11] <<
            (proc->proc_input_channel.chan_buffer.bufferValid() ?
                proc->proc_input_channel.chan_buffer.buffer()->b_buf_name
            :
                "[none]") <<
            (proc->proc_output_channel.chan_buffer.bufferValid() ?
                proc->proc_output_channel.chan_buffer.buffer()->b_buf_name
            :
                "[none]") <<
            (proc->proc_input_channel.chan_procedure != NULL ?
                proc->proc_input_channel.chan_procedure->b_proc_name
            :
                "[none]")
            );
    }

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old_buffer.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

//
//
//    MLisp procedure to wait for a process input request
//
//
int wait_for_process_input_request( void )
{
    EmacsProcess *proc = get_process_arg();

    if( proc == 0 )
        return 0;    // Verify process existance

    //
    //    Keep looping in a scheduler loop until something happens
    //
    ProcessChannel *chan = &proc->proc_output_channel;

    while( ! chan->chan_data_request )
    {
        if( input_pending != 0 )
        {
            if( timer_interrupt_occurred != 0 )
                process_timer_interrupts();
            else
                ml_value = int(0);
            return 0;
        }

        if( pending_channel_io != 0 )
        {
            process_channel_interrupts();
        }
        else
            wait_for_activity();
    }

    ml_value = 1;

    return 0;
}

int vms_load_averages( void )
{
    // use NT process stuff here
    return 0;
}

//
//
//    initialize the VMS-specific world
//
//
void init_vms( void )
{ }

//
// Special routine for restore. Kills off all processes
// except a broadcast-message processor. This processor is
// re-started
//
void restore_vms( void )
{
    init_vms();


    //
    // Restore the sub-processes
    //
    // All processes are re-started. Note, however, that the context
    // of the process is that inherited from this incarnation rather
    // than that of the original process
    //
}


EmacsThread::EmacsThread()
{}

EmacsThread::~EmacsThread()
{}
