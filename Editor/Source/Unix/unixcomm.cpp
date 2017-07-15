//
//    unixcomm.cpp
//
//    Copyright 1997 (c) Barry A. Scott
//
#include <emacs.h>
#include <em_stat.h>
#include <emacs_signal.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

extern int ptym_open( char *pts_name );
extern int ptys_open( int fdm, char *pts_name );

SystemExpressionRepresentationIntPositive maximum_dcl_buffer_size( 10000 );
SystemExpressionRepresentationIntPositive dcl_buffer_reduction( 500 );

#ifndef MAXFDS
# define MAXFDS 254
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <syslog.h>
//# ifndef _BSD
//#  define _BSD    // define this to get the wait() family of calls defined
//# endif
#include <sys/wait.h>
#ifdef _AIX
//# include <sys/m_wait.h>
# include <sys/select.h>
#endif

#if defined( __hpux )
# include <sys/pty.h>

# if !defined( _XPG4_EXTENDED )
// define openlog and syslog as it is missing on hpux 9
extern "C" void syslog(int priority, const char *message, ...);
extern "C" void openlog(const char *ident, int logopt, int facility);
# endif
#endif

#if defined( __linux__ ) || defined( __FreeBSD__ )
# define TIME_STAMP_STR "%d.%3.3d "
#else
# define TIME_STAMP_STR "%d.%03.3d "
#endif

#include <termios.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <unixcomm.h>

#if defined( SUBPROCESSES )

# if DBG_PROCESS && DBG_TMP
extern int elapse_time(void);
#  define Trace( s )  do { \
                        if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP ) { \
                            int t=elapse_time(); \
                            _dbg_msg( FormatString(TIME_STAMP_STR "%s") << t/1000 << t%1000 << (s) ); } \
                    } while(0)
# else
#  define Trace( s ) // do nothing
# endif

const int STOPPED(  1<<0 ); // 1
const int RUNNING(  1<<1 ); // 2
const int EXITED(   1<<2 ); // 4
const int SIGNALED( 1<<3 ); // 8
const int CHANGED(  1<<6 ); // 64

inline bool EmacsProcess::activeProcess()
{
    return bool( p_flag & (RUNNING|STOPPED) );
}

static EmacsProcess *get_process_arg()
{
    EmacsString name;
    if( cur_exec == NULL )
        EmacsProcess::name_table.get_word_interactive( "Process: ", name );
    else
        EmacsProcess::name_table.get_word_mlisp( name );

    EmacsProcess *proc = EmacsProcess::name_table.find( name );
    if(  proc == NULL )
    {
        error( "No such process." );
        return NULL;
    }

    return proc;
}

fd_set process_fds;                             // The set of subprocess fds
EmacsProcess *EmacsProcess::current_process;    // the one that we're current dealing with
int child_changed;                              // Flag when a child process has ceased to be
ProcessChannelInput *MPX_chan;

const char *SIG_names[] = {
    "",
    "Hanged",
    "Interrupted",
    "Quit*",
    "Illegal instruction*",
    "Trapped*",
    "Iot*",
    "EMT*",
    "FPE*",
    "Killed",
    "Bus error*",
    "Segment violation*",
    "Sys call error*",
    "Pipe write error",
    "Alarm clock",
    "Termination",
    "",
    "Stopped",
    "Stopped",
    "", "", "", "", "", "", ""
};

void stuff_buffer( ProcessChannelInput & );

EmacsString str_process( "Process: " );
EmacsString str_err_proc( "Cannot find the specified process" );
EmacsString str_is_blocked( "There is data already waiting to be send to the blocked process" );

EmacsProcess::EmacsProcess( const EmacsString &name, const EmacsString &_command)
: EmacsProcessCommon( name )
, chan_in()
, command( _command )
, term_proc( NULL )
, in_id(0)
, out_id(0)
, p_id(0)
, p_flag(0)
, p_reason(0)
, out_id_valid(0)
{
    if( maximum_dcl_buffer_size < 1000 )
        maximum_dcl_buffer_size = 10000;
    if( dcl_buffer_reduction > maximum_dcl_buffer_size - 500
    || dcl_buffer_reduction < 500 )
        dcl_buffer_reduction = 500;

//    proc_input_channel.chan_maximum_buffer_size = maximum_dcl_buffer_size;
//    proc_input_channel.chan_buffer_reduction_size = dcl_buffer_reduction;

    Trace( FormatString("EmacsProcess object created %s %s") << proc_name << command );
}

EmacsProcess::~EmacsProcess()
{
    Trace( FormatString("EmacsProcess object deleted %s %s") << proc_name << command );
}

ProcessChannelInput::ProcessChannelInput()
: ch_fd(-1)
, ch_ptr( NULL )
, ch_count( 0 )
, ch_buffer( NULL )
, ch_proc( NULL )
, ch_utf8_buffer_used( 0 )
{ }

ProcessChannelInput::~ProcessChannelInput()
{
    if( ch_fd >= 0 )
    {
        // close the PTY master side channel
        int status = close( ch_fd );
        Trace( FormatString("ProcessChannelInput::~ProcessChannelInput close(%d) => %d") << ch_fd << status );

        FD_CLR( ch_fd, &process_fds );
    }
}

ProcessChannelOutput::ProcessChannelOutput()
: ch_fd(-1)
, ch_count(0)
, ch_ccount(0)
, ch_buf( NULL )
, ch_data( NULL )
{ }

ProcessChannelOutput::~ProcessChannelOutput()
{
    if( ch_fd >= 0 )
    {
        int status = close( ch_fd );
        Trace( FormatString("ProcessChannelOutput::~ProcessChannelOutput close(%d) => %d") << ch_fd << status );
    }
    if( ch_buf )
        EMACS_FREE( ch_buf );
}

EmacsPollFdId add_to_select( int fd, long int mask, EmacsPollFdCallBack cb, EmacsProcess *npb )
{
    FD_SET( fd, &process_fds );

    return add_select_fd( fd, mask, cb, npb );
}

void remove_input( EmacsPollFdId id )
{
    remove_select_fd( id );
}


// Callback to handle an input request
void readProcessOutputHandler( EmacsPollFdParam p_, int fdp )
{
    EmacsProcess *p = (EmacsProcess *)p_;
    ProcessChannelInput &chan = p->chan_in;

    Trace( FormatString( "readProcessOutputHandler( Param p_, int fdp(%d) )") << fdp );

    int read_count = 5;
    int cc;
    do
    {
        cc = read( chan.ch_fd, &chan.ch_utf8_buffer[chan.ch_utf8_buffer_used], ProcessChannelInput::ch_buffer_size-chan.ch_utf8_buffer_used );
        Trace( FormatString( "readProcessOutputHandler read( %d, ... ) =>  %d errno %e" )
                            << chan.ch_fd << cc << errno );
        if( cc > 0 )
        {
            chan.ch_utf8_buffer_used += cc;

            // find out the usable bytes in buf
            int utf8_usable_length = 0;
            int unicode_length = length_utf8_to_unicode(
                        chan.ch_utf8_buffer_used, chan.ch_utf8_buffer,
                        ProcessChannelInput::ch_buffer_size,
                        utf8_usable_length );

            // convert to unicode
            convert_utf8_to_unicode( chan.ch_utf8_buffer, unicode_length, chan.ch_unicode_buffer );
            if( chan.ch_utf8_buffer_used > utf8_usable_length )
            {
                memmove( &chan.ch_utf8_buffer[0], &chan.ch_utf8_buffer[utf8_usable_length], chan.ch_utf8_buffer_used - utf8_usable_length );
            }

            chan.ch_utf8_buffer_used -= utf8_usable_length;

            Trace( FormatString( "readProcessOutputHandler unicode_buffer \"%r\"" )
                                << EmacsString( EmacsString::copy, chan.ch_unicode_buffer, unicode_length ) );

            chan.ch_ptr = chan.ch_unicode_buffer;
            chan.ch_count = unicode_length;
            stuff_buffer( chan );
        }
        else if( (cc == 0 && p->p_flag&(EXITED|SIGNALED) )  // end-of-file and process has exited
              || (cc < 0 && errno != EAGAIN) )              // and error and not told to do it again
        {
            // shutdown input and output
            if( p->in_id != 0 )
            {
                remove_input( p->in_id );
                p->in_id = 0;
            }
            if( p->out_id_valid )
            {
                remove_input( p->out_id );
                p->out_id = 0;
                p->out_id_valid = 0;
            }
            int status = close( chan.ch_fd );
            Trace( FormatString("readProcessOutputHandler close(%d) => %d errno %e") << chan.ch_fd << status << errno );
            FD_CLR( chan.ch_fd, &process_fds );

            chan.ch_fd = -1;
            return;
        }
    }
    while( cc > 0 && read_count-- > 0);
}

// Callback to handle an output request
void writeProcessInputHandler( EmacsPollFdParam p_, int fdp )
{
    EmacsProcess *p = (EmacsProcess *)p_;
    ProcessChannelOutput &chan = p->chan_out;

    Trace( FormatString("writeProcessInputHandler( Param p_, fdp %d ) ch_ccount %d ch_count %d")
        << fdp << chan.ch_ccount << chan.ch_count );
    if( chan.ch_ccount > 0 )
    {
        int cc = write( chan.ch_fd, "\004", 1 );
        Trace( FormatString("writeProcessInputHandler write( %d, ^D, 1 ) => %d errno %e") << chan.ch_fd << cc << errno );
        if( cc < 0 )
        {
            remove_input( p->out_id );
            p->out_id_valid = 0;
            return;
        }
        chan.ch_ccount = 0;
    }
    if( chan.ch_count > 0 )
    {
        int cc = write( chan.ch_fd, chan.ch_data, chan.ch_count );
        Trace( FormatString("writeProcessInputHandler write( %d, \"%*r\", %d ) => %d errno %e")
                            << chan.ch_fd << chan.ch_count << chan.ch_data << cc << cc << errno );
        if( cc > 0 )
        {
            chan.ch_data += cc;
            chan.ch_count -= cc;
        }
    }
    else
    {
        if( p->out_id_valid )
        {
            remove_input( p->out_id );
            p->out_id_valid = 0;
        }
        if( chan.ch_buf )
        {
            EMACS_FREE( chan.ch_buf );
            chan.ch_buf = NULL;
        }
    }
}



//
// Find the process which is connected to proc_name.
//
EmacsProcess *EmacsProcess::findProcess( const EmacsString &proc_name )
{
    EmacsProcess *p = EmacsProcessCommon::name_table.find( proc_name );
    if( p != NULL && p->activeProcess() )
        return p;

    return NULL;
}

//
// Get the first active process in the process list and assign to the current
// process
//
EmacsProcess *EmacsProcess::getNextProcess()
{
    for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
    {
        EmacsProcess *p = EmacsProcessCommon::name_table.value( index );
        if( p->activeProcess() )
            return p;
    }

    return NULL;
}

//
// Give a message that a process has changed and indicate why.  Dead processes
// are not removed until after a Display Processes command has been issued so
// that the user doesn't wonder where his process went in times of intense
// hacking
//
void change_msgs( void )
{
# if DBG_PROCESS
    if( dbg_flags&DBG_PROCESS )
        _dbg_msg( "change_msgs() starting\n" );
# endif

    int dodsp = 0, restore = 0;
    EmacsBufferRef old( bf_cur );
    int OldBufferIsVisible = (theActiveView->currentWindow()->w_buf == bf_cur );
    int old_cc = child_changed;
    int change_processed = 0;

    for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
    {
        EmacsProcess *p = EmacsProcessCommon::name_table.value( index );

# if DBG_PROCESS
        if( dbg_flags&DBG_PROCESS )
            _dbg_msg( FormatString("change_msgs() found \"%s\" p_flags=0x%x\n") << p->proc_name << p->p_flag );
# endif
        if( p->p_flag & CHANGED )
        {
            EmacsString status;

            p->p_flag &= ~CHANGED;
            change_processed ++;

            switch( p->p_flag & ( SIGNALED | EXITED ) )
            {
            case SIGNALED:
                status = FormatString("%s\n") << SIG_names[int(p->p_reason)];
                break;
            case EXITED:
                status = FormatString("Exited %d\n") << p->p_reason;
                break;
            }

            if( p->term_proc != NULL )
            {
                enum arg_states LArgState = arg_state;
                int larg = arg;

                arg_state = no_arg;
                MPX_chan = &p->chan_in;    // User will be able to get the output from process-output
                MPX_chan->ch_ptr = const_cast<EmacsChar_t *>( status.unicode_data() );
                MPX_chan->ch_count = status.length();
# if DBG_PROCESS
                if( dbg_flags&DBG_PROCESS )
                    _dbg_msg( FormatString("change_msgs() calling term_proc=%s proc_name=\"%s\" p_flags=0x%x\n")
                        << p->term_proc->b_proc_name << p->proc_name << p->p_flag );
# endif

                p->term_proc->execute();
                arg_state = LArgState;
                arg = larg;
                // remove refs to status's address.
                MPX_chan->ch_ptr = NULL;
                MPX_chan->ch_count = 0;
                MPX_chan = NULL;
                dodsp++;
                EmacsProcess::flushProcess( p );
            }
            else if( p->chan_in.ch_buffer != NULL )
            {
                p->chan_in.ch_buffer->set_bf();
                set_dot( bf_cur->unrestrictedSize() + 1 );
                bf_cur->ins_cstr( status );
                if( ( bf_cur->unrestrictedSize() ) > maximum_dcl_buffer_size )
                {
                    bf_cur->del_frwd( 1, dcl_buffer_reduction );
                    set_dot( bf_cur->unrestrictedSize() + 1 );
                }
                if( bf_cur->b_mark.isSet() )
                    bf_cur->set_mark( dot, 0, false );
                dodsp++;
                restore++;
            }
        }
    }

    // Update the screen if necessary
    if( dodsp )
    {
        if( restore )
        {
            old.set_bf();
            if( interactive() && OldBufferIsVisible )
                theActiveView->window_on( bf_cur );
        }
        theActiveView->do_dsp();
    }

    // Update the child_changed counter
    if( change_processed == 0 )
    {
        if( child_changed == old_cc )
            child_changed = 0;
    }
    else
        child_changed -= change_processed;
# if DBG_PROCESS
    if( dbg_flags&DBG_PROCESS )
        _dbg_msg( FormatString("change_msgs() done child_changed=%d change_processed=%d\n")
                << child_changed << change_processed );
# endif
}

//
// Send any pending output as indicated in the process block to the
// appropriate channel.
//
void send_chan( EmacsProcess *process )
{
    ProcessChannelOutput &output = process->chan_out;

    Trace( FormatString("send_chan( process %d) ch_ccount %d ch_count %d")
        << int(process->p_id) << output.ch_ccount << output.ch_count );
    if( output.ch_count == 0 && output.ch_ccount == 0 )
        ;
    else if( output.ch_ccount )
    {
        // write Ctrl-D to process
        int cc = write( output.ch_fd, "\004", 1 );
        Trace( FormatString("send_chan write( %d, ^D, 1 ) => %d errno %e") << output.ch_fd << cc << errno );
        if( cc >= 0 )
        {
            output.ch_ccount = 0;
        }
    }
    else
    {
        if( output.ch_count )
        {
            int cc = write( output.ch_fd, output.ch_data, output.ch_count );
            Trace( FormatString("send_chan write( %d, \"%*r\", %d ) => %d errno %e")
                                << output.ch_fd << EmacsString( EmacsString::copy, output.ch_data, output.ch_count ) << output.ch_count << cc << errno );
            if( cc > 0 )
            {
                output.ch_data += cc;
                output.ch_count -= cc;
            }
        }
    }
    if( !process->out_id_valid )
    {
        process->out_id = add_to_select( output.ch_fd, EmacsPollInputWriteMask, writeProcessInputHandler, process );
        process->out_id_valid = 1;
    }
}

// Output has been recieved from a process on "chan" and should be stuffed in
// the correct buffer
//
void stuff_buffer( ProcessChannelInput &chan )
{
    EmacsBufferRef old_buffer( bf_cur );
    static int lockout;
    int OldBufferIsVisible = theActiveView->currentWindow()->w_buf == bf_cur;

    // Check the lock
    if( lockout )
    {
        error( "On-output procedure asked for input" );
        return;
    }
    else
        lockout = 1;

    if( chan.ch_proc == NULL )
    {
        if( chan.ch_buffer == NULL )
        {
            error( "Process output available with no destination buffer" );
        }
        else
        {
            chan.ch_buffer->set_bf();
            set_dot( bf_cur->unrestrictedSize() + 1 );

            EmacsChar_t *p, *q;
            for( p = q = chan.ch_ptr; p < &chan.ch_ptr[chan.ch_count]; p++ )
            {
                if( *p == '\r' )
                {
                    if( p - q > 0 )
                    {
                        bf_cur->ins_cstr( q, p - q );
                    }
                    q = &p[1];
                }
            }
            if( p - q > 0 )
            {
                bf_cur->ins_cstr( q, p - q );
            }

            if( ( bf_cur->unrestrictedSize() ) > maximum_dcl_buffer_size )
            {
                bf_cur->del_frwd( 1, dcl_buffer_reduction );
                set_dot( bf_cur->unrestrictedSize() + 1 );
            }

            if( bf_cur->b_mark.isSet() )
            {
                bf_cur->set_mark( bf_cur->unrestrictedSize() + 1, 0, false );
            }

            theActiveView->do_dsp();
            old_buffer.set_bf();
            if( interactive() && OldBufferIsVisible )
            {
                theActiveView->window_on( bf_cur );
            }
        }
    }
    else
    {
        enum arg_states LArgState = arg_state;
        int larg = arg;

        arg_state = no_arg;
        MPX_chan = &chan;
        chan.ch_proc->execute();
        arg_state = LArgState;
        arg = larg;
        MPX_chan = NULL;    // a very short time only
    }

    chan.ch_count = 0;
    lockout = 0;
}

//
// Return a count of all active processes
//
int EmacsProcess::countProcesses()
{
    int count = 0;

    for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
    {
        EmacsProcess *p = EmacsProcessCommon::name_table.value( index );
        if( p->activeProcess() )
            count++;
    }

    return count;
}

int count_processes(void)
{
    return EmacsProcess::countProcesses();
}

//
// Flush a process but only if process is inactive
//
void EmacsProcess::flushProcess( EmacsProcess *process )
{
    Trace( FormatString("EmacsProcess::flushProcess %s") << process->proc_name );
    if( process->activeProcess() )
    {
        error( "Cannot flush an active process" );
        return;
    }

    if( process->in_id )
        remove_input( process->in_id );
    if( process->out_id_valid )
        remove_input( process->out_id );
    process->in_id = 0;
    process->out_id = 0;
    process->out_id_valid = 0;

    delete process;
}

//
// Kill off all active processes: usually done only to exit when user really
// insists
//
void EmacsProcess::killProcesses( void )
{
    for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
    {
        EmacsProcess *p = EmacsProcessCommon::name_table.value( index );
        if( p->activeProcess() )
# ifdef __hpux
            kill( p->p_id, SIGKILL );
# else
            killpg( p->p_id, SIGKILL );
# endif
    }
}

void kill_processes()
{
    EmacsProcess::killProcesses();
}

//
// Start up a new process by creating the process block and initializing
// things correctly
//
//
// Start up a subprocess with its standard input and output connected to
// a channel on the master side of a ptyf.  Also set its process group so we can kill it
// and set up its process block.  The process block is assumed to be pointed
// to by current_process.
//
extern char **environ;

bool EmacsProcess::startProcess( EmacsPosixSignal &sig_child )
{
    int channel;
    // Open the pty master side
    char pts_name[1024];
    channel = ptym_open( pts_name );
    const EmacsString ptyname( pts_name );

    Trace( FormatString("startProcess: ptyname %s channel %d") << ptyname << channel );
    if( channel < 0 )
    {
        error( "Cannot get a pseudo-terminal for the process\n" );
        return false;
    }
    if( channel > MAXFDS )
    {
        close( channel );
        error( "Too many processes already running" );
        return false;
    }

    unsetenv( "TERM" );
    setenv( "BEMACS_SHELL", "1", 1 );

    // Fork the child
    pid_t pid = fork();
    if( pid < 0 )
    {
        error( "Fork failed for process" );
        close( channel );
        return false;
    }

    if( pid == 0 )
    {
        // Handle child side of fork
        struct termios sg;

        EmacsPosixSignal sig_int( SIGINT ); sig_int.defaultSignalAction();
        EmacsPosixSignal sig_quit( SIGQUIT ); sig_quit.defaultSignalAction();

        sig_child.defaultSignalAction();
        sig_child.permitSignal();

        setsid();

        int newfd = ptys_open( channel, pts_name );
        if( newfd < 0 )
        {
            fprintf( stdout, "Cannot open pseudo-terminal %s reason %d\n", ptyname.sdata(), newfd );
            _exit( 1 );
        }

        // close the master side now we have the slave tty side open
//        close( channel );

//    why is this in here at all?
//        status = setpgrp();

        // close std in, out, ml_err
        close( STDIN_FILENO );
        close( STDOUT_FILENO );
        close( STDERR_FILENO );
        // dup the pty channel to std in, out, ml_err
        dup2( newfd, STDIN_FILENO );
        dup2( newfd, STDOUT_FILENO );
        dup2( newfd, STDERR_FILENO );
        // close off pty channel
        close( newfd );

        // become the controlling terminal
# if defined( TIOCSCTTY )
        ioctl( STDIN_FILENO, TIOCSCTTY, 0 );
#  elif defined( TIOCNOTTY )
        ioctl( STDIN_FILENO, TIOCNOTTY, 0 );
# endif

        // get the pty terminal attributes
        tcgetattr( STDIN_FILENO, &sg );

        // setup the attributes the way we want them
        sg.c_cc[VERASE] = 0xff;
        sg.c_cc[VKILL] = 0xff;

        sg.c_cc[VINTR] = 0177;
        sg.c_cc[VQUIT] = '\\' & 037;
        sg.c_cc[VSTART] = 'Q' & 037;
        sg.c_cc[VSTOP] = 'S' & 037;
        sg.c_cc[VEOF] = 'D' & 037;
# ifdef VBRK
        sg.c_cc[VBRK] = 0xff;
# endif
        sg.c_cc[VSUSP] = 0xff; //'Z' & 037;
# ifdef VDSUSP
        sg.c_cc[VDSUSP] = 0xff; //'Y' & 037;
# endif
# ifdef VREPRINT
        sg.c_cc[VREPRINT] = 'R' & 037;
# endif
# ifdef VDISCARD
        sg.c_cc[VDISCARD] = 'O' & 037;
# endif
# ifdef VWERASE
        sg.c_cc[VWERASE] = 'W' & 037;
# endif
# ifdef VLNEXT
        sg.c_cc[VLNEXT] = 'V' & 037;
# endif

        // turn off echo
        sg.c_lflag &= ~ECHO;

        tcsetattr( STDIN_FILENO, TCSANOW, &sg );

        int ld = 2;
        EmacsString shname( shell() );
        if( shname.length() >= 3 )
        {
            EmacsString shname_tail( shname( -3, -1 ) );
            if( shname_tail.caseBlindCompare( "csh" ) == 0 )
                ld = 0;
        }

# ifdef TIOCSETD
        int line_discipline = 0;
        ioctl( STDIN_FILENO, TIOCSETD, &line_discipline );
# endif

        if( ld == 0 )
        {
            // csh version
            execlp( shname, shname, "-f", "-c", command.sdata(), NULL );
        }
        else
        {
            execlp( shname, shname, "-c", command.sdata(), NULL );
        }

        write( STDOUT_FILENO, "Could not start the shell\n", 24 );
        _exit( 1 );
    }

    // Handle parent side of fork
    p_id = pid;
    p_flag = RUNNING;
    in_id = add_to_select( channel, EmacsPollInputReadMask, readProcessOutputHandler, this );
    out_id = add_to_select( channel, EmacsPollInputWriteMask, writeProcessInputHandler, this );
    out_id_valid = 1;

    chan_in.ch_fd = channel;
    chan_in.ch_ptr = NULL;
    chan_in.ch_count = 0;

    chan_out.ch_fd = channel;
    chan_out.ch_count = 0;
    chan_out.ch_ccount = 0;
    chan_out.ch_buf = NULL;

    term_proc = NULL;

    EmacsBuffer::set_bfn( proc_name );
    if( interactive() )
        theActiveView->window_on( bf_cur );

    chan_in.ch_buffer = bf_cur;
    chan_in.ch_proc = NULL;

    Trace( FormatString("startProcess pid=%d") << int(p_id) );
    return true;
}

void EmacsProcess::stopProcess()
{
    Trace( FormatString("stopProcess pid=%d") << int(p_id) );
    // see if there is nothing to do
    if( !activeProcess() )
        return;
# ifdef __hpux
    kill( p_id, SIGKILL );
# else
    killpg( p_id, SIGKILL );
# endif
}

//
// Emacs command to start up a default process: uses "Command Execution"
// buffer if one is not specified.  Also does default stuffing
//
int start_dcl_process(void)
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
    EmacsString process_command = getstr( "Command: " );
    EmacsProcess *proc = EMACS_NEW EmacsProcess( process_name, process_command );

    EmacsPosixSignal sig_child( SIGCHLD );
    // block now and release as the routine returns
    sig_child.blockSignal();

    // do the hard work of getting the process going
    if( !proc->startProcess( sig_child ) )
    {
        proc->stopProcess();

        delete proc;

        error( "Unable to start the process");

        return 0;
    }

    //
    //    This is now the current process
    //
    EmacsProcess::current_process = proc;

    //
    //    All done!
    //
    return 0;
}

//
// Internal Procedure to get an MLisp procedure argument
//
static BoundName * get_procedure_arg( const EmacsString &prompt )
{
    BoundName *procedure = getword( BoundName::, prompt );

    if( procedure == NULL || procedure->b_proc_name == "novalue" )
        return NULL;

    return procedure;
}

//
// Insert a filter-procedure between a process and emacs. This function
// should subsume the StartFilteredProcess function, but we should retain
// that one for compatibility I suppose...
//
int set_process_output_procedure( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    process->chan_in.ch_proc = get_procedure_arg( "On-output procedure: " );
    return 0;
}

//
// Insert a termination procedure to be called when the process terminates
//
int set_process_termination_proc( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    process->term_proc = get_procedure_arg( "On-termination procedure: ");

    // if the process has already terminated force this proc to get called
    if( process->p_flag & ( SIGNALED | EXITED ) )
    {
        process->p_flag |= CHANGED;
        child_changed++;

# if DBG_PROCESS
        if( dbg_flags&DBG_PROCESS )
            _dbg_msg( FormatString("set_process_termination_proc after exit (%s) \n")
                     << process->proc_name );
# endif
    }

    return 0;
}

//
// Sets the process name
//
int set_process_name( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    EmacsString new_name = getstr( "New name: " );
    if( new_name.isNull() )
        return 0;
    if( EmacsProcess::findProcess( new_name ) != NULL )
    {
        error( FormatString("A process named %s already exists") << new_name );
        return 0;
    }

    process->proc_name = new_name;

    return 0;
}

//
// Sets the process output buffer
//
int set_process_output_buffer( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    EmacsString new_name = getstr( "New buffer: " );
    if( new_name.isNull() )
        return 0;

    EmacsBuffer::set_bfn( new_name.isNull() ? EmacsString( "Command Execution" ) : new_name );
    if( interactive() )
        theActiveView->window_on( bf_cur );

    process->chan_in.ch_buffer = bf_cur;

    return 0;
}

//
// List the current processes.  After listing stopped or exited processes,
// flush them from the process list.
//
int list_processes(void)
{
    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Process list", interactive() );
    bf_cur->ins_str("Name                    Buffer                  Status           Command\n"
                    "----                    ------                  ------           -------\n" );

    EmacsPosixSignal sig( SIGCHLD );
    // block now and release as the routine returns
    sig.blockSignal();

    for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
    {
        EmacsProcess *p = EmacsProcessCommon::name_table.value( index );

        bf_cur->ins_cstr( FormatString("%-24s") << p->proc_name );
        bf_cur->ins_cstr( FormatString("%-24s") << p->chan_in.ch_buffer->b_buf_name );
        switch( p->p_flag & ( STOPPED | RUNNING | EXITED | SIGNALED ) )
        {
        case STOPPED:
            bf_cur->ins_cstr( FormatString("%-17s") << "Stopped" );
            break;
        case RUNNING:
            bf_cur->ins_cstr( FormatString("%-17s") << "Running" );
            break;
        case EXITED:
            bf_cur->ins_cstr( FormatString("Exited %-10s") << ( p->p_reason == 0 ? "" : "Abnormally" ) );
            break;
        case SIGNALED:
            bf_cur->ins_cstr( FormatString("%-17s") << SIG_names[int(p->p_reason)] );
            break;
        default:
            continue;
        }
        bf_cur->ins_cstr( FormatString("  %s\n") << p->command );

        if( p->term_proc != NULL )
            bf_cur->ins_cstr( FormatString("  Termination procedure: %s") << p->term_proc->b_proc_name );
        if( p->chan_in.ch_proc != NULL )
            bf_cur->ins_cstr( FormatString("  Input procedure: %s") << p->chan_in.ch_proc->b_proc_name );
        if( p->term_proc != NULL || p->term_proc != NULL )
            bf_cur->ins_cstr( "\n" );

        // if the process has died then flush it
        if( (p->p_flag & (EXITED | SIGNALED)) != 0 )
            EmacsProcess::flushProcess( p );
    }

    bf_cur->b_modified = 0;
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

static unsigned char *savestr( const unsigned char *s )
{
    if( s == NULL )
        // return a nul string
        return savestr( u_str("") );

    // copy the string
    size_t size = strlen( reinterpret_cast<const char *>( s ) ) + 1;
    unsigned char *ret = malloc_ustr( size );
    memcpy( ret, s, size );
    return ret;
}

//
// Send a string to the process as input
//
int send_string_to_process( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    EmacsString input_string = getstr( "String: " );
    ProcessChannelOutput &output = process->chan_out;

    if( output.ch_count || output.ch_ccount )
        error( str_is_blocked );

    output.ch_fd = process->chan_in.ch_fd;
    output.ch_ccount = 0;
    output.ch_count = input_string.length();
    if( output.ch_count <= 0 )
        error( "Attempt to send null string to process" );
    output.ch_data = output.ch_buf = savestr( input_string );

    send_chan( process );

    return 0;
}

//
// Get the current output which has been thrown at us and send it
// to the user as a string; this is only allowed if MPX_chan is non-null
// indicating that this has been indirectly called from stuff_buffer
//
int process_output( void )
{
    if( MPX_chan == NULL )
    {
        error( "process-output can only be called from an process output procedure" );
        return 0;
    }

    ml_value = EmacsString( EmacsString::copy, MPX_chan->ch_ptr, MPX_chan->ch_count );
    return 0;
}

//
// Send an signal to the specified process group
//
int sig_process( int signal )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    Trace( FormatString("sig_process name %s pid %d signal %d")
        << process->proc_name << int(process->p_id) << signal );

    //
    // We must update the process flag explicitly in the case of continuing a
    // process since no signal will come back
    //
    if( signal == SIGCONT )
    {
        long int sig_mask;

        sig_mask = sigblock( 1<<SIGCHLD );
        process->p_flag = (process->p_flag & ~STOPPED ) | RUNNING;
        sigsetmask( ~( 1<<SIGCHLD ) & sig_mask );
    }

    switch( signal )
    {
    case SIGINT:
    case SIGQUIT:
# ifdef TIOCFLUSH
    {
        int status = ioctl( process->chan_in.ch_fd, TIOCFLUSH, 0 );
        Trace( FormatString("sig_process ioctl(TIOCFLUSH) => %d proc %s") << status << process->proc_name );
    }
# endif
        process->chan_out.ch_count = 0;
        process->chan_out.ch_ccount = 0;
        break;
    }

    Trace( FormatString("sig_process %s to %s") << SIG_names[signal] << process->proc_name );
# ifdef __hpux
    kill( process->p_id, signal );
# else
    killpg( process->p_id, signal );
# endif

    return 0;
}

int force_exit_process( void )
{
    return sig_process( SIGINT );
}

int quit_process(void)
{
    return sig_process( SIGQUIT );
}

int kill_process( void )
{
    return sig_process( SIGKILL );
}

int pause_process( void )
{
    return sig_process( SIGTSTP );
}

int resume_process( void )
{
    return sig_process( SIGCONT );
}

int send_eof_to_process( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    ProcessChannelOutput &output = process->chan_out;
    if( output.ch_count || output.ch_ccount )
        error( str_is_blocked );

    output.ch_count = 0;
    output.ch_data = output.ch_buf = NULL;
    output.ch_ccount = 1;

    send_chan( process );

    return 0;
}

//
// Some useful functions on the process
//
int current_process_name( void )
{
    if( EmacsProcess::current_process == NULL )
        ml_value = EmacsString("");
    else
        ml_value = EmacsProcess::current_process->proc_name;

    return 0;
}

//
// Change the current-process to the one indicated.
//
int set_current_process( void )
{
    EmacsProcess *process = get_process_arg();
    if( process == NULL )
        return 0;

    EmacsProcess::current_process = process;

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
    EmacsProcess *process = EmacsProcess::findProcess( proc_name );
    if( process == NULL )
        ml_value = int(-1);
    else
        if( process->p_flag & RUNNING )
            ml_value = int(1);
        else
            ml_value = int(0);

    return 0;
}

// Get the process id
int process_id( void )
{
    EmacsString proc_name = getstr( "Process: " );
    EmacsProcess *process = EmacsProcess::findProcess( proc_name );

    if( process == NULL )
        ml_value = int( 0 );
    else
        ml_value = int( process->p_id );

    return 0;
}
//
// Initialize things on the multiplexed file.  This involves connecting the
// standard input to a channel on the  mpx file.  Someday we will be brave and
// close 0.
//
int set_process_input_procedure( void )
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

int process_channel_interrupts( void )
{
    return 0;
}

void proc_de_ref_buf( EmacsBuffer * )
{
    return;
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
#endif

#if defined( SUBPROCESSES ) || defined( EXEC_BF )
//
// Process a signal from a child process and make the appropriate change in
// the process block. Since signals are NOT queued, if two signals are
// received before this routine gets called, then only the first process in
// the process list will be handled.  We will try to get the MPX file stuff
// to help us out since it passes along signals from subprocesses.
//
class ChildSignalHandler : public EmacsPosixSignalHandler
{
public:
    ChildSignalHandler()
    : EmacsPosixSignalHandler( SIGCHLD )
    { }

    virtual ~ChildSignalHandler()
    { }

private:
    void signalHandler();
};

ChildSignalHandler child_sig;

extern int subproc_id;  // The process id of a subprocess started by the old subproc stuff.

void ChildSignalHandler::signalHandler()
{
    for(;;)
    {
        pid_t pid;
        int stat_loc;

        pid = waitpid( -1, &stat_loc, WUNTRACED | WNOHANG );

# if DBG_PROCESS
        if( dbg_flags&DBG_PROCESS )
            _dbg_msg( FormatString("waitpid => pid: %d, stat_loc: 0x%x\n")
                << int(pid) << stat_loc );
# endif
        if( pid <= 0 )
        {
            if( errno == EINTR )
            {
                errno = 0;
                continue;
            }
# if defined( SUBPROCESSES )
            if( pid == -1 )
            {
                if( EmacsProcess::current_process == NULL
                || !EmacsProcess::current_process->activeProcess() )
                {
                    EmacsProcess::current_process = EmacsProcess::getNextProcess();
                }
            }
# endif
            return;
        }
# if defined( EXEC_BF )
        if( pid == subproc_id )
        {
            // Take care of those subprocesses first
            subproc_id = 0;
            continue;
        }
# endif
# if defined( SUBPROCESSES )
        EmacsProcess *p = NULL;
        for( int index=0; index<EmacsProcessCommon::name_table.entries(); index++ )
        {
            p = EmacsProcessCommon::name_table.value( index );
            if( pid == p->p_id )
                break;
        }
        if( p == NULL )
            continue;        // We don't know who this is

#  if DBG_PROCESS
        if( dbg_flags&DBG_PROCESS )
            _dbg_msg( FormatString("Found emacs process 0x%x (%s)\n")
                     << (void *)p << p->proc_name );
#  endif
        if( WIFSTOPPED( stat_loc ) )
        {
            p->p_flag = STOPPED;
            p->p_reason = WSTOPSIG( stat_loc );

#  if DBG_PROCESS
            if( dbg_flags&DBG_PROCESS )
                _dbg_msg( "p_flags <= STOPPED\n" );
#  endif
        }
        else if( WIFEXITED( stat_loc ) )
        {
            p->p_flag = EXITED | CHANGED;
            child_changed++;
            p->p_reason = WEXITSTATUS( stat_loc );

#  if DBG_PROCESS
            if( dbg_flags&DBG_PROCESS )
                _dbg_msg( "p_flags <= EXITED | CHANGED\n" );
#  endif
        }
        else if( WIFSIGNALED( stat_loc ) )
        {
            p->p_flag = SIGNALED | CHANGED;
            child_changed++;
            p->p_reason = WTERMSIG( stat_loc );

#  if DBG_PROCESS
            if( dbg_flags&DBG_PROCESS )
                _dbg_msg( "p_flags <= SIGNALED | CHANGED\n" );
#  endif
        }
        if( EmacsProcess::current_process == NULL
        || !EmacsProcess::current_process->activeProcess() )
        {
            EmacsProcess::current_process = EmacsProcess::getNextProcess();
        }
# endif
    }
}

void init_subprocesses()
{
    child_sig.installHandler();
}

void restore_subprocesses( void )
{
    child_sig.installHandler();
}
#endif
