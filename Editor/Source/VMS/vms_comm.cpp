//     Copyright(c) 1982, 1983, 1984, 1985
//        Barry A. Scott and nick Emery
//

#include <emacs.h>
#include <algorithm>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


SystemExpressionRepresentationInt force_redisplay;
SystemExpressionRepresentationInt maximum_dcl_buffer_size( 10000 );
SystemExpressionRepresentationInt dcl_buffer_reduction( 500 );

#if defined( vms )

// empty routines to stub out UI- functions
int ui_add_menu(void) { return 0; }
int ui_edit_copy(void) { return 0; }
int ui_edit_paste(void) { return 0; }
int ui_file_open(void) { return 0; }
int ui_file_save_as(void) { return 0; }
int ui_find(void) { return 0; }
int ui_find_and_replace(void) { return 0; }
int ui_frame_maximize(void) { return 0; }
int ui_frame_minimize(void) { return 0; }
int ui_frame_restore(void) { return 0; }
int ui_frame_to_foreground(void) { return 0; }
int ui_list_menus(void) { return 0; }
int ui_remove_menu(void) { return 0; }
int ui_switch_buffer(void) { return 0; }
int ui_window_cascade(void) { return 0; }
int ui_window_maximize(void) { return 0; }
int ui_window_minimize(void) { return 0; }
int ui_window_restore(void) { return 0; }
int ui_window_tile_horz(void) { return 0; }
int ui_window_tile_vert(void) { return 0; }
int ui_add_to_recent_file_list(void) { return 0; }
int ui_win_exec (void) { return 0; }
int ui_win_help (void) { return 0; }
int ui_view_tool_bar(void) { return 0; }
int ui_view_status_bar(void) { return 0; }
int ui_open_file_readonly;
unsigned char ui_open_file_name[64];
unsigned char ui_save_as_file_name[64];
unsigned char ui_filter_file_list[256];
unsigned char ui_search_string[128];
unsigned char ui_replace_string[128];

#include <vms_comm.h>
#include <iodef.h>
#include <dvidef.h>
#include <msgdef.h>

char * clisetstr( void );
static void mail_box_AST_routine( ProcessChannel *chan );
static void termination_AST( EmacsProcess *dead_process );
int process_channel_interrupts( void );
static void close_channel( ProcessChannel *chan );
static int setup_channel( ProcessChannel *chan );
int start_DCL_process( void );
static EmacsProcess * get_process_arg( void );
static BoundName *get_procedure_arg( void );
int send_string_to_process( void );
int set_process_termination_proc( void );
int set_process_output_procedure( void );
int set_process_input_procedure( void );
int set_process_output_buffer( void );
int kill_process( void );
void proc_de_ref_buf( EmacsBuffer *b );
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
int VMS_load_averages( void );
int _spawn
        (
        char *cmd,
        char * in,
        char * out,
        int flags,
        char *name,
        unsigned int *id,
        int *status,
        unsigned char efn,
        void (*astadr)(void *), void *astprm,
        int reassign
        );
#if BROADCAST

int associate_mbx( void );
int deassign_mbx( void );
void reassign_mbx( void );
#endif
void init_vms( void );
void restore_vms( void );


static int terminating_process;    // Process terminated
unsigned int base_priority;

#if BROADCAST
EmacsProcess *broad_cast_process = 0;
#endif
Queue channel_list_head;
EmacsProcess *current_process = 0;
EmacsProcess **process_ptrs;
unsigned char **process_names;
int n_processes = 0;
int n_process_slots = 20;


static struct dsc$descriptor sysin_name =
{ 9, 0, 0, "SYS$INPUT" };

static struct dsc$descriptor sysout_name =
{ 10, 0, 0, "SYS$OUTPUT" };

static char sysin_text[64];
static struct dsc$descriptor sysin_trn = {sizeof(sysin_text),0,0,sysin_text};

static char sysout_text[64];
static struct dsc$descriptor sysout_trn = {sizeof(sysout_text),0,0,sysout_text};

extern void alter_brd_mbx();
extern unsigned char cli_name[64];
char *clisetstr( void )
{
    if( _str_icmp( cli_name, u_str("DCL") ) == 0 )
        return "$ set noon";
    else if( _str_icmp( cli_name, u_str("SHELL") ) == 0 )
        return "set -d";
    else
        return NULL;
}

//
//
//    Routine to service MailBox Attention ASTs
//    ( we just signal the higher level code and continue )
//
static void mail_box_AST_routine( ProcessChannel *chan )
{
    //
    //    Mark the channel as having interrupted
    //
    chan->chan_interrupt = 1;
    //
    //    Bump the various flags
    //
    interlock_inc( &pending_channel_io );
    //
    //    If we are hibernating, wake us up
    //
    conditional_wake();
}

//
//
//    Routine to handle termination ASTs
//
//
static void termination_AST( EmacsProcess *dead_process )
{
    dead_process->proc_state = PROC_K_DEAD;
    interlock_inc( &terminating_process );
    interlock_inc( &pending_channel_io );

    //
    //    If we are hibernating, wake us up
    //
    conditional_wake();
}
//
//
//    Routine to Process Channel Interrupts( called from Scheduler )
//
//
int process_channel_interrupts( void )
{
    int i;
    int work_done = 0;
    ProcessChannel *chan;
    int status;
    unsigned short int iosb[4];
    int savedcant_1line_opt;
    int savedcant_1win_opt;
    int savedredo_modes;
    EmacsProcess *saved_current_process;
    EmacsBuffer *saved_buffer;
    int loop_count;

    savedcant_1line_opt = cant_1line_opt;    // Be nice to refresher
    savedcant_1win_opt = cant_1win_opt;    // ditto
    savedredo_modes = redo_modes;        // ditto
    saved_current_process = current_process;
    saved_buffer = 0;
    //
    //    Process any terminating processes
    //
    while( terminating_process != 0 )
    {
        EmacsProcess *proc;
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
                    //
                    //    Call the term procedure
                    //
                    current_process = proc;
                    execute_bound_saved_environment( proc->proc_procedure );
                    if( force_redisplay )
                    {
                        do_dsp();
                        force_redisplay = 0;
                        savedcant_1win_opt = cant_1win_opt;
                        savedcant_1line_opt = cant_1line_opt;
                        savedredo_modes = redo_modes;
                    }
                }
                //
                //    Close open channels
                //
                close_channel( &proc->proc_input_channel );
                close_channel( &proc->proc_output_channel );
                //
                //    Kill the process entry
                //
                process_names[i] = 0;
                process_ptrs[i] = 0;

                for( j=i+1; j<n_processes; j++ )
                {
                    process_names[ j - 1] = process_names[ j];
                    process_ptrs[ j - 1] = process_ptrs[ j];
                }
                n_processes--;
                //
                //    Free the data structures
                //
                free( proc->proc_name );
                free( proc );
            }
        }
    }

    //
    //    Scan the list of channels to see which ones require attention
    //
    chan = (ProcessChannel *)channel_list_head.next;
    while( &chan->chan_queue != &channel_list_head )
    {
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
                loop_count = 3;
                // Enforce some Limit on this Loop!
                while( input_pending == 0 && loop_count > 0 )
                {
                    loop_count--;
                    status = sys$qiow
                        (
                        EFN_DO_NOT_WAIT_ON,
                        chan->chan_vms_channel,
                        IO$_READVBLK|IO$M_NOW,
                        (void *)&iosb,
                        NULL, NULL,
                        chan->chan_local.chan_buffer,
                        CHAN_BUFFER_SIZE,
                        0, 0, 0, 0
                        );
                    //
                    //    If error, assume there is no data to process
                    //
                    if( ! VMS_SUCCESS( status ) ) debug_invoke();
                    if( ! VMS_SUCCESS( iosb[0] ) )
                    {
                        if( iosb[0] != SS$_ENDOFFILE )
                            debug_invoke();
                        break;
                    }
                    //
                    //    Mung the input data( if necessary )
                    //
#if BROADCAST

                    if( chan->chan_brdcst_chan )
                    {
                        int ch;
                        unsigned char *p;
                        struct msg_record *msgbuf;

                        msgbuf = &chan->chan_local.chan_msg;

                        //
                        // If this is not a Broadcast then
                        // junk the data and try again. Bump the count
                        // to allow the unsolicit messages to be
                        // sweeped from the mailbox
                        //
                        if( msgbuf->msg_type != MSG$_TRMBRDCST )
                        {
                            loop_count++;
                            continue;
                        }
                        else
                        {
                            work_done = 1;
                            iosb[1] = iosb[1] - (sizeof(struct msg_record)-1);
                            p = msgbuf->msg_message;
                            while( p[0] == '\r'
                            || p[0] == '\n'
                            || p[0] == ctl('l') )
                            {p++; iosb[1]--;}
                            while( (ch = p[iosb[1] - 1] ) == '\r'
                            || ch == '\n' || ch == '\f' )
                                iosb[1]--;
                            // move the data up to the front of the buffer
                            memmove( chan->chan_local.chan_buffer, p, iosb[1] );
                        }
                    }
                    else
#endif
                        work_done = 1;

                    if( iosb[1] != CHAN_BUFFER_SIZE )
                    {
                        int add_trailing_LF;
                        unsigned char *s;
                        int n;

                        add_trailing_LF = 0;
                        s = chan->chan_local.chan_buffer;
                        n = iosb[1];

                        while( n > 0 )
                        {
                            n--;
                            if( s[0] == '\r' )
                            {
                                s[0] = '\n';
                                add_trailing_LF = -10000;
                            }
                            if( *s++ == '\n' )
                                add_trailing_LF++;
                        }
                        if( add_trailing_LF <= 0 )
                        {
                            s[0] = '\n';
                            iosb[1]++;
                        }
                    }
                    //
                    //    If there is a buffer associated with
                    //    this process output, make it current
                    //
                    if( chan->chan_buffer != 0 )
                    {
                        saved_buffer = bf_cur;
                        chan->chan_buffer ->set_bf();
                        set_dot( bf_cur->unrestrictedSize() + 1 );
                    }
                    //
                    //    If there is no procedure to call, assume
                    //    that we must stuff into Buffer
                    //
                    if( chan->chan_procedure == 0 )
                    {
                        //
                        //    If no buffer, get out of here!
                        //
                        if( chan->chan_buffer == 0 )
                            break;
                        //
                        //    Insert the Data
                        //
                        ins_cstr( chan->chan_local.chan_buffer, iosb[1] );
                        //
                        //    If buffer has overflowed
                        //    Delete start of buffer
                        //
                        if( (bf_cur->unrestrictedSize()) > chan->chan_maximum_buffer_size )
                        {
                            del_frwd( 1, chan->chan_buffer_reduction_size );
                            set_dot( bf_cur->unrestrictedSize() + 1 );
                        }
                    }// E_nd of Put Data into Buffer
                    else
                    {
                        //
                        //    Call the procedure
                        //
                        current_process = chan->chan_process;
                        chan->chan_chars_left = iosb[1];
                        chan->chan_local_buffer_has_data = 1;
                        execute_bound_saved_environment( chan->chan_procedure );
                        chan->chan_local_buffer_has_data = 0;
                        if( force_redisplay )
                        {
                            do_dsp();
                            force_redisplay = 0;
                            savedcant_1win_opt = cant_1win_opt;
                            savedcant_1line_opt = cant_1line_opt;
                            savedredo_modes = redo_modes;
                        }
                    }
                    // End of Call Procedure to deal with Data

                    //
                    //    If we had switched buffers, restore old one
                    //
                    if( saved_buffer != 0 )
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
                    chan->chan_reads_to_go_before_redisplay =
                        chan->chan_reads_to_go_before_redisplay - 1;
                    if( input_pending == 0
                    && (chan->chan_reads_to_go_before_redisplay <= 0) )
                    {
                        //
                        //    If it is the current buffer, we
                        //        always redisplay
                        //
                        if( bf_cur == chan->chan_buffer )
                        {
                            chan->chan_reads_to_go_before_redisplay = 1;
                            do_dsp();
                            savedcant_1line_opt = cant_1line_opt;
                            savedcant_1win_opt = cant_1win_opt;
                            savedredo_modes = redo_modes;
                        }// En_d of Current Buffer
                        //
                        //    If not the current buffer, we search
                        //    the list of windows to see if this
                        //    buffer is visible.
                        //
                        else
                        {
                            EmacsWindow *w;

                            w = windows;
                            while( w != 0 )
                            {
                                if( w->w_buf == chan->chan_buffer )
                                {
                                    //
                                    //    Buffer is visible,
                                    //    do the redisplay
                                    //
                                    chan->chan_reads_to_go_before_redisplay =
                                        chan->chan_num_reads_before_redisplay;
                                    do_dsp();
                                    savedcant_1line_opt = cant_1line_opt;
                                    savedcant_1win_opt = cant_1win_opt;
                                    savedredo_modes = redo_modes;
                                }
                                w = w->w_next;
                            }
                // En_d of search for window
                        }
                // En_d of Visible Buffer( Not Current )
                    }// En_d of Re-Display check_
                }// En_d of Read Data Loop

                //
                //    Set up a new Write attention AST
                //
                status = sys$qiow
                    (
                    EFN_DO_NOT_WAIT_ON,
                    chan->chan_vms_channel,
                    IO$_SETMODE|IO$M_WRTATTN,
                    (void *)&iosb,
                    NULL, NULL,
                    mail_box_AST_routine,
                    chan,
                    0, 0, 0, 0
                    );
                if( ! VMS_SUCCESS(status) ) debug_invoke();
                if( ! VMS_SUCCESS(iosb[0]) ) debug_invoke();
            }// En_d of Service Write Attention
            else
            {
                EmacsProcess *p;
                //
                //    Sub-Process wants data: notify its input procedure
                //
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
                        do_dsp();
                        force_redisplay = 0;
                        savedcant_1win_opt = cant_1win_opt;
                        savedcant_1line_opt = cant_1line_opt;
                        savedredo_modes = redo_modes;
                    }
                }    // En_d of Call Channel Input Procedure
            }    // En_d of Service Read Attention

            //
            //    Decrement pending Channel Interrupts
            //
            interlock_dec( &pending_channel_io );
            if( pending_channel_io != 0 )
                break;
            //
            //    If we exited the Loop with LoopCount zero,
            //    then we must do something about this Channel
            //    dominating things. We move the Channel to the
            //    the en_d of the Channel List and restart the
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

        }        // End of Scan for Interrupting Channels
        chan = (ProcessChannel *)chan->chan_queue.next;
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
//
//
//    Routine to close down a Channel
//
//
static void close_channel( ProcessChannel *chan )
{
    //
    //    If the chan is open, unlink it from the channel list and
    //        close the VMS I/O channel
    //
    if( chan->chan_channel_open )
    {
        if( chan->chan_queue.next != NULL )
            queue_remove( &chan->chan_queue );
        if( chan->chan_vms_channel != 0 )
            sys$dassgn( chan->chan_vms_channel );
        chan->chan_channel_open = 0;
        //
        //    There maybe outstanding IO on the channel. If so
        //    correct the pending channel io count
        //
        if( chan->chan_interrupt )
            interlock_dec( &pending_channel_io );
    }
}
//
//
//    Routine to Create a mailbox and set up the chan structure
//
//
static int setup_channel( ProcessChannel *chan )
{
    int code;
    int status;
    unsigned short int iosb[4];

    unsigned int unit=0;

    //
    //    If it is already set up, just return
    //
    if( chan->chan_channel_open )
        return 0;
    //
    //    Create a Mailbox for the channel
    //
    if( ! VMS_SUCCESS(sys$crembx
        (
        0,
        &chan->chan_vms_channel,
        CHAN_BUFFER_SIZE,
        CHAN_BUFFER_SIZE,
        DEFAULT_MAIL_BOX_PROTECTION,
        0, NULL
        )) )
            return -1;
    chan->chan_channel_open = 1;

    //
    //    Use $Getdvi to get its unit number( will be required later )
    //
    lib$getdvi
    (
    (code = DVI$_UNIT, &code),
    &chan->chan_vms_channel,
    NULL,
    &unit,
    NULL,
    NULL
    );

    chan->chan_mbx_unit = (unsigned short)unit;

    //
    //    Insert into the channel List
    //
    queue_insert( channel_list_head.prev, &chan->chan_queue );

    //
    //    Set up the appropriate attention AST request
    //
    status = sys$qiow
        (
        EFN_DO_NOT_WAIT_ON,
        chan->chan_vms_channel,
        (chan->chan_read_channel ?
                IO$_SETMODE | IO$M_WRTATTN
            :
                IO$_SETMODE | IO$M_READATTN),
        (void *)iosb,
        NULL, NULL,
        mail_box_AST_routine,
        chan,
        0, 0, 0, 0
        );
    if( ! VMS_SUCCESS(status) || ! VMS_SUCCESS(iosb[0]) )
    {
        debug_invoke();
        return -1;
    }
    //
    //    Return Success
    //
    return 0;
}
//
//
//    MLisp procedure to start up a sub-process running DCL in order
//        to later run commands in it.
//
//
int start_DCL_process( void )
{
    unsigned char *process_name;
    int i;
    EmacsProcess *proc;
    char sys_input[64];
    char sys_output[64];

#if BROADCAST
    int brd_cst_mbx;    // Assume not Broadcast
#endif
    process_name = getnbstr( u_str("process name: ") );
    if( process_name == 0 )
        return 0;
#if BROADCAST
    brd_cst_mbx = _str_cmp( process_name, u_str("broadcast-messages") ) == 0;
#endif

    //
    //    Verify that we do not have a duplicate process name
    //
#if BROADCAST

    if( brd_cst_mbx && broad_cast_process != 0 )
    {
        error( "duplicate broadcast process." );
        return 0;
    }
#endif
    for( i=0; i<n_processes; i++ )
        if( _str_cmp( process_names[i], process_name ) == 0 )
        {
            error( "duplicate process name." );
            return 0;
        }
    //
    //    Allocate the Process Structure
    //
    proc = EMACS_NEW EmacsProcess;

    //
    //    initialize the process structure
    //
    proc->proc_process_id = 0;
    proc->proc_state = PROC_K_RUNNING;
    proc->proc_time_state_was_entered = time( 0 );
    proc->proc_name = savestr( process_name );
    proc->proc_procedure = 0;

    proc->proc_output_channel.chan_process = proc;
    proc->proc_output_channel.chan_read_channel = 0;

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

    //
    //    Setup the Input and Output Channels
    //
    if( setup_channel( &proc->proc_input_channel ) != 0 )
    {
        error( "could not create input channel." );
        return 0;
    }
#if BROADCAST
    if( ! brd_cst_mbx )
#endif
        if( setup_channel( &proc->proc_output_channel ) != 0 )
        {
            error( "could not create output channel." );
            return 0;
        }
    //
    //    Insert this process in the list of known processes
    //
    if( n_processes == (n_process_slots - 1) )
    {
        //
        //    Out of room - reallocate the Process Lists
        //
        n_process_slots = n_process_slots + 20;
        process_names = (unsigned char **)realloc( process_names,
                    n_process_slots * sizeof(unsigned char *), malloc_type_star_star );
        process_ptrs = (EmacsProcess **)realloc( process_ptrs,
                    n_process_slots * sizeof(EmacsProcess *), malloc_type_star_star );
    }
    process_ptrs[n_processes] = proc;
    process_names[n_processes] = proc->proc_name;
    n_processes++;
    process_names[n_processes] = 0;
    process_ptrs[n_processes] = 0;

#if BROADCAST
    if( brd_cst_mbx )
    {
        //
        //    Setup the pointer to it for Associate and DisAssociate
        //    to find the Process structure
        //
        broad_cast_process = proc;
        proc->proc_input_channel.chan_brdcst_chan = 1;

        associate_mbx();

        //
        //    This is now the current process
        //
        current_process = proc;
        alter_brd_mbx( 1 );

        //
        //    All done!
        //
        return 0;
    }
#endif

    //
    //    Generate the Input, Output and error name descriptors
    //
    sprintfl( u_str(sys_output), sizeof( sys_output ),
            u_str( "_MBA%d:"),
            proc->proc_input_channel.chan_mbx_unit  );
    sprintfl( u_str(sys_input), sizeof( sys_input ),
            u_str( "_MBA%d:"),
            proc->proc_output_channel.chan_mbx_unit  );

    //
    //    Spawn the sub-process
    //
    if( ! VMS_SUCCESS(_spawn
        (
        clisetstr(),            // command line descriptor
        sys_input,            // input file spec
        sys_output,            // Output file spec
        1,                // flags: Nowait
        0,                // Process name
        &proc->proc_process_id,        // Process id spawned
        0,                // Completion status
        EFN_DO_NOT_WAIT_ON,        // Completion EFN
        (void (*)(void *))termination_AST,    // Completion ASTADR
        (void *)proc,            // Completion ASTPRM
        1                // Reassign after spawn )) )
    {
        termination_AST( proc );
        error( "could not spawn subprocess." );
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
//
//    Internal routine to get a Process argument
//
//
static EmacsProcess * get_process_arg( void )
{
    int index;
    index = BoundName::getword( "Process: " );

    if( index < 0 )
    {
        error( "No such process." );
        return 0;
    }
    return process_ptrs[index];
}

//
//
//    Internal Procedure to get an MLisp procedure argument
//
//
static BoundName * get_procedure_arg( void )
{
    int index;
    index = BoundName::getword( "Procedure: " );

    if( index >= 0
    && _str_cmp( mac_names[index], "novalue") != 0 )
        return mac_bodies[index];
    else
        return NULL;
}

//
//
//    MLisp procedure to send a string to a named process
//
//
int send_string_to_process( void )
{
    int status;
    unsigned short int iosb[4];
    EmacsProcess *proc;
    ProcessChannel *output_channel;
    unsigned char *cp;

    proc = get_process_arg();
    //
    //    Verify the destination process
    //
    if( proc == 0 )
        return 0;
    //
    //    If it exists, make sure we can send to it
    //
    if( proc->proc_state == PROC_K_DEAD )
    {
        error( "process is dead." );
        return 0;
    }
    output_channel = &proc->proc_output_channel;
    if( output_channel->chan_buffer != 0 )
    {
        error( "process input has a region attached to it." );
        return 0;
    }
    if( ! output_channel->chan_data_request )
    {
        error( "process not asking for input." );
        return 0;
    }
    output_channel->chan_data_request  = 0;
    //
    //    Change process state to 'RUNNING'
    //
    proc->proc_time_state_was_entered = time( 0 );
    BLOCKED_TO_RUNNING( proc );
    //
    //    Send the string and re-enable the Read Attention AST
    //
    cp = getstr( u_str("string: ") );
    if( cp != 0 )
    {
        status = sys$qiow
            (
            EFN_DO_NOT_WAIT_ON,
            output_channel->chan_vms_channel,
            IO$_WRITEVBLK | IO$M_NOW,
            (void *)&iosb,
            NULL, NULL,
            cp,
            _str_len( cp ),
            0,0,0,0
            );
        if( ! VMS_SUCCESS(status) ) debug_invoke();
        if( ! VMS_SUCCESS(iosb[0]) ) debug_invoke();
        status = sys$qiow
            (
            EFN_DO_NOT_WAIT_ON,
            output_channel->chan_vms_channel,
            IO$_SETMODE| IO$M_READATTN,
            (void *)&iosb,
            NULL, NULL,
            mail_box_AST_routine,
            output_channel,
            0,0,0,0
            );
        if( ! VMS_SUCCESS(status) ) debug_invoke();
        if( ! VMS_SUCCESS(iosb[0]) ) debug_invoke();
    }
    return 0;
}

//
//
//    MLisp procedure to set the process termination procedure
//
//
int set_process_termination_proc( void )
{
    EmacsProcess *proc;
    proc = get_process_arg();

    //
    //    Get the Procedure arg and set it
    //
    if( proc != 0 )
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
    EmacsProcess *proc;
    proc = get_process_arg();

    //
    //    Get the Procedure arg and set it
    //
    if( proc != 0 )
        proc->proc_input_channel.chan_procedure = get_procedure_arg();

    return 0;
}

//
//
//    MLisp procedure to set the process input procedure
//
int set_process_input_procedure( void )
{
    EmacsProcess *proc;

    proc = get_process_arg();

    //
    //    Get the Procedure arg and set it
    //
    if( proc != 0 )
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
    unsigned char *cp;
    EmacsBuffer *buf;
    EmacsProcess *proc;

    proc = get_process_arg();

    //
    //    Verify the Process
    //
    if( proc == 0 )
        return 0;
    //
    //    Get the Buffer
    //
    cp = EmacsBuffer::getescword( "Buffer: " );
    if( cp == 0 )
        return 0;
    if( (buf = find_bf( cp )) == 0 )
        buf = new_bf( cp );
    //
    //    Set the Process Buffer
    //
    proc->proc_input_channel.chan_buffer  = buf;
    return 0;
}

//
//
//    MLisp procedure to Kill a named process
//
//
int kill_process( void )
{
    EmacsProcess *proc;

    proc = get_process_arg();

    if( proc == 0 )
        return 0;

#if BROADCAST
    if( proc == broad_cast_process )
    {
        broad_cast_process = 0;
        proc->proc_state = PROC_K_DEAD;    // kill it off
        interlock_inc( &terminating_process );    // One more to terminate
        interlock_inc( &pending_channel_io );
        alter_brd_mbx( 0 );
        return 0;
    }
#endif

    //
    //    Do a DelPrc on the named process(Its termination message
    //        will arrive later)
    //
    if( proc->proc_process_id == 0 )
    {
        proc->proc_state = PROC_K_DEAD;
        interlock_inc( &terminating_process );
        interlock_inc( &pending_channel_io );
    }
    else
        sys$delprc( &proc->proc_process_id, NULL );
    return 0;
}

void proc_de_ref_buf( EmacsBuffer *b )
{
    int i;

    for( i=0; i<n_processes; i++ )
    {
        EmacsProcess *proc;
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
    EmacsProcess *proc;
    int i;

    for( i=0; i<n_processes; i++ )
    {
        proc = process_ptrs[i];
#if BROADCAST

        if( proc == broad_cast_process )
        {
            proc->proc_state = PROC_K_DEAD;// kill it off
            interlock_inc( &terminating_process );// One more to terminate
            interlock_inc( &pending_channel_io );
        }
#endif
        if( proc->proc_state != PROC_K_DEAD )
            sys$delprc( &proc->proc_process_id, NULL );
    }
}

int count_processes( void )
{
    return n_processes != 0;
}

//
//
//    MLisp procedure to suspend a named process
//
//
int pause_process( void )
{
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
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
    return 0;
}

//
//    MLisp procedure to Resume a named process
//
//
int resume_process( void )
{
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
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
    return 0;
}

//
//    MLisp procedure to Force Exit a named process
//
//
int force_exit_process( void )
{
    int code;
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
        return 0;    // Verify Process existance

    //
    //    Get the Exit Code
    //
    code = getnum( u_str("exit code: ") );
    //
    //    Do a Force Exit on the named process
    //    Note: In EUNICE, a -ve Force Exit code is
    //        taken to be a signal( once negated ),
    //        so you can send signals with this.
    //
    sys$forcex( &proc->proc_process_id, NULL, code );
    return 0;
}

//
//    Set the named process as current
//
//
int set_current_process( void )
{
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
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
    ml_value.exp_type = ISSTRING;
    if( current_process != 0 )
    {
        ml_value.exp_v.v_string = current_process->proc_name;
        ml_value.exp_int = _str_len( ml_value.exp_v.v_string );
    }
    else
    {
        ml_value.exp_v.v_string = u_str( "" );
        ml_value.exp_int = 0;
    }
    ml_value.exp_release = 0;
    return 0;
}

//
//
//    MLisp procedure to change a process name
//
//
int set_process_name( void )
{
    unsigned char *cp;
    int process_index;
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
        return 0;    // Verify process existance

    //
    //    Get the new name
    //
    cp = getnbstr( u_str("new name: ") );
    if( cp != 0 )
    {
        int i;

        //
        //    search for duplicate process name(and our
        //        process index)
        //
        for( i=0; i<n_processes; i++ )
        {
            if( _str_cmp( process_names[i], cp ) == 0 )
            {
                if( process_ptrs[i] != proc )
                    error( "duplicate process name." );
                return 0;
            }
            if( process_ptrs[i] == proc )
                process_index = i;
        }
        //
        //    The New name is unique, deallocate the old one
        //        and setup the new one
        //
        free( proc->proc_name );
        process_names[process_index] = proc->proc_name =
            savestr( cp );
    }
    return 0;
}

//
//
//    MLisp procedure to return any output from a process
//
//
int process_output( void )
{
    ProcessChannel *chan;
    EmacsProcess *proc;
    proc = get_process_arg();

    if( proc == 0 )
        return 0;    // Verify process existance

    //
    //    Make sure there is output to return
    //
    chan = &proc->proc_input_channel;
    if( ! chan->chan_local_buffer_has_data )
    {
        error( "no unprocessed data from this process." );
        return 0;
    }
    //
    //    Flag the data as having been snarfed
    //
    chan->chan_local_buffer_has_data = 0;
    //
    //    Return the data as a string
    //
    ml_value.exp_type = ISSTRING;
    ml_value.exp_v.v_string = chan->chan_local.chan_buffer;
    ml_value.exp_release = 0;
    ml_value.exp_int = chan->chan_chars_left;
    return 0;
}

//
//
//    MLisp procedure to put a list of processes and their states into
//        the buffer 'Process List'
//
//
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
    EmacsProcess *proc;
    char *cp;
    unsigned char line[100];
    int i;

    EmacsBufferRef old_buffer( bf_cur );

    EmacsBuffer::scratch_bfn( "Process list", interactive() );
    ins_str(u_str("Process                 State   Time      Buffer           In Proc         Out Proc\n"));
    ins_str(u_str("-------                 -----   ----      ------           -------         --------\n"));
//                  "..................... ....... ... ..... ............... ............... ...............
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
    old_buffer ->set_bf();
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
    ProcessChannel *chan;
    EmacsProcess *proc = get_process_arg();

    if( proc == 0 )
        return 0;    // Verify process existance

    ml_value.exp_type = ISINTEGER;

    //
    //    Keep looping in a scheduler loop until something happens
    //
    chan = &proc->proc_output_channel;

    while( ! chan->chan_data_request )
    {
        if( input_pending != 0 )
        {
            if( timer_interrupt_occurred != 0 )
                process_timer_interrupts();
            else
                ml_value.exp_int = 0;
            return 0;
        }

        if( pending_channel_io != 0 )
        {
            process_channel_interrupts();
        }
        else
            wait_for_activity();
    }

    ml_value.exp_int = 1;
    return 0;
}
//
//    MLisp routine to return the current Load Averages as a string
//
static int VLAR( struct dsc$descriptor *str )
{
    static unsigned char buf[132];

     ml_value.exp_type = ISSTRING;
    ml_value.exp_v.v_string = buf;
    ml_value.exp_release = 0;
    ml_value.exp_int = std::min( sizeof( buf )-1, str->dsc$w_length );

    memcpy( buf, str->dsc$a_pointer, ml_value.exp_int );

    return 1;
}

int VMS_load_averages( void )
{
    lib$show_timer( 0, 0, VLAR );
    return 0;
}
#if BROADCAST

static unsigned char term_name_text[129];
static unsigned char mbx_name_text[30];
static struct dsc$descriptor term_name, mbx_name;

//
//    The next two routines associate a Mailbox with the users terminal
//
int associate_mbx( void )
{
    ProcessChannel *chan;
    EmacsProcess *proc;

    proc = broad_cast_process;

    //
    //    Only go on if there is process and IO is to a terminal
    //
    if( proc == 0 || ! term_is_terminal )
        return 0;

    chan = &proc->proc_output_channel;

    //
    //    If it is already set up, just return
    //
    if( chan->chan_channel_open )
        return 0;

    sprintfl( mbx_name_text, sizeof(mbx_name_text ),
        u_str( "_MBA%d:"),
        proc->proc_input_channel.chan_mbx_unit  );
    DSC_SZ( mbx_name, mbx_name_text );

    if( (get_log(u_str("TT"), term_name_text) ) == 0 )
    {
        error( "Could not setup terminal." );
        return 0;
    }

    DSC_SZ( term_name, term_name_text );

    //
    //    Do the assign with mailbox to tt
    //
    reassign_mbx();

    chan->chan_channel_open = 1;

    //
    //    Insert into the channel List
    //
    queue_insert( channel_list_head.prev, &chan->chan_queue );

    return 0;
}
#endif
int _spawn
    (
    char * sz_cmd,
    char * sz_in,
    char * sz_out,
    int flags,
    char *sz_name,
    unsigned int *id,
    int *status,
    unsigned char efn,
    void (*astadr)(void *), void *astprm,
    int reassign
    )
{
    struct dsc$descriptor cmd, in, out, name, cli, prompt;
    char *sz_prompt;
    int ret;
#if BROADCAST
    int resp;
    resp = deassign_mbx();
#endif

    DSC_SZ( cmd, sz_cmd );
    DSC_SZ( in, sz_in );
    DSC_SZ( out, sz_out );
    DSC_SZ( cli, cli_name );
    if( sz_name )
    { DSC_SZ( name, sz_name ); }
    if( reassign )
    {
        lib$set_logical( &sysin_name, &in );
        lib$delete_logical( &sysout_name );
    }
    if( _str_icmp( cli_name, u_str("MCR") ) == 0 )
        sz_prompt = "(emacs)> ";
    else if( _str_icmp( cli_name, u_str("SHELL") ) == 0 )
        sz_prompt = "(emacs)% ";
    else
        sz_prompt = "(emacs)$ ";
    DSC_SZ( prompt, sz_prompt );

    ret = lib$spawn
        (
        &cmd,
        &in,
        &out,
        &flags,
        sz_name ? &name : NULL,
        id,
        status,
        &efn,
        astadr,
        astprm,
        &prompt,
        &cli
        );
#if BROADCAST
    if( reassign && resp )
        reassign_mbx();
#endif
    if( reassign )
    {
        lib$set_logical( &sysin_name, &sysin_trn );
        lib$set_logical( &sysout_name, &sysout_trn );
    }
    return ret;
}

#if BROADCAST

static int brdcstmbx_assigned;

int deassign_mbx( void )
{
    ProcessChannel *chan;
    int resp;
    resp = 0;

    if( broad_cast_process != 0 && term_is_terminal && brdcstmbx_assigned )
    {
        alter_brd_mbx( 0 );
        chan = &broad_cast_process->proc_output_channel;
        resp = sys$dassgn( chan->chan_vms_channel );
    }
    brdcstmbx_assigned = 0;
    return resp;
}

void reassign_mbx( void )
{
    ProcessChannel *chan;

    if( broad_cast_process != NULL && term_is_terminal && ! brdcstmbx_assigned )
    {
        alter_brd_mbx( 1 );
        chan = &broad_cast_process->proc_output_channel;
        sys$assign
        (
        &term_name,
        &chan->chan_vms_channel,
        0,
        &mbx_name
        );
        brdcstmbx_assigned = 1;
    }
}
#endif
//
//
//    initialize the VMS-specific world
//
//
void init_vms( void )
{
    // translate $input, and $output
    sys$trnlog
    (
    &sysin_name,
    &sysin_trn.dsc$w_length,
    &sysin_trn,
    NULL, NULL, NULL
    );
    sys$trnlog
    (
    &sysout_name,
    &sysout_trn.dsc$w_length,
    &sysout_trn,
    NULL, NULL, NULL
    );

    //
    //    Get the various process quotas so we can create
    //        sub-processes correctly
    //
    base_priority = 4;

    //
    //    initialize the Process Lists
    //
    if( process_ptrs == NULL )
    {
        process_ptrs = malloc( n_process_slots * sizeof( EmacsProcess * ), malloc_type_star_star );
        process_ptrs[0] = 0;
    }
    if( process_names == NULL )
    {
        process_names = malloc( n_process_slots * sizeof( unsigned char * ), malloc_type_star_star );
        process_names[0] = 0;
    }

    if( channel_list_head.next == NULL )
        queue_init( &channel_list_head );

    lib$init_timer();
}

//
// Special routine for restore. Kills off all processes
// except a broadcast-message processor. This processor is
// re-started
//
void restore_vms( void )
{
    int i;
    EmacsProcess *proc;

    init_vms();


    //
    // Restore the sub-processes
    //
    // All processes are re-started. Note, however, that the context
    // of the process is that inherited from this incarnation rather
    // than that of the original process
    //
    for( i=0; i<n_processes; i++ )
    {
        proc = process_ptrs[i];

        // Close the channels first
        proc->proc_input_channel.chan_vms_channel  = 0;
        close_channel( &proc->proc_input_channel );
        proc->proc_output_channel.chan_vms_channel  = 0;
        close_channel( &proc->proc_output_channel );

        // Re-open them
        if( setup_channel( &proc->proc_input_channel ) != 0 )
        {
            error( "could not create input channel." );
            break;    // give up
        }
#if BROADCAST
        if( proc->proc_input_channel.chan_brdcst_chan  )
        {
            broad_cast_process = proc;
            associate_mbx();
            alter_brd_mbx( 1 );
        }
        else
#endif
        {
            char sys_input[64];
            char sys_output[64];

            if( setup_channel( &proc->proc_output_channel ) != 0 )
            {
                error( "could not create output channel." );
                termination_AST( proc );
                break;
            }
            sprintfl( u_str(sys_input), sizeof(sys_input ),
                u_str( "_MBA%d:"),
                proc->proc_input_channel.chan_mbx_unit  );
            sprintfl( u_str(sys_output), sizeof(sys_output ),
                u_str( "_MBA%d:"),
                proc->proc_output_channel.chan_mbx_unit  );
            if( ! (_spawn
                (
                clisetstr(),
                sys_input,            // input file spec
                sys_output,            // Output file spec
                1,                // flags: Nowait
                0,                // Process name
                &proc->proc_process_id,        // Process id spawned
                0,                // Completion status
                EFN_DO_NOT_WAIT_ON,        // Completion EFN
                (void (*)(void *))termination_AST,// Completion ASTADR
                proc,                // Completion ASTPRM
                1                // Reassign after spawn
                )) )
            {
                termination_AST( proc );
                error( "could not spawn sub-process." );
                break;
            }
        }
    }
}
#else
int start_dcl_process( void )
{
    return no_value_command();
}

int send_string_to_process( void )
{
    return no_value_command();
}

int set_process_termination_proc( void )
{
    return no_value_command();
}

int set_process_output_procedure( void )
{
    return no_value_command();
}

int set_process_input_procedure( void )
{
    return no_value_command();
}

int set_process_output_buffer( void )
{
    return no_value_command();
}

int kill_process( void )
{
    return no_value_command();
}

int set_process_name( void )
{
    return no_value_command();
}

int list_processes( void )
{
    return no_value_command();
}

int wait_for_process_input_request( void )
{
    return no_value_command();
}

int vms_load_averages( void )
{
    return no_value_command();
}

int set_current_process( void )
{
    return no_value_command();
}

int current_process_name( void )
{
    return no_value_command();
}

int process_output( void )
{
    return no_value_command();
}

int pause_process( void )
{
    return no_value_command();
}

int resume_process( void )
{
    return no_value_command();
}

void restore_vms( void )
{
    return;
}

int process_channel_interrupts( void )
{
    return 0;
}

int count_processes( void )
{
    return 0;
}
void proc_de_ref_buf( EmacsBuffer *b )
{
    return;
}

void kill_processes( void )
{
    return;
}

int force_exit_process( void )
{
    return 0;
}

void init_vms(void)
{
    return;
}
#endif
