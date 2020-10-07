//
//    emacs_signal.cpp
//
#include <emacs.h>
#include <emacs_signal.h>

//
//
//    EmacsPosixSignalHandler
//
//
EmacsPosixSignalHandler *EmacsPosixSignalHandler::active_handlers[MAX_SIG_HANDLED];

EmacsPosixSignalHandler::EmacsPosixSignalHandler( int _sig_to_handle )
    : is_installed( false )
    , sig_to_handle( _sig_to_handle )
{
    emacs_assert( sig_to_handle < MAX_SIG_HANDLED );
}

EmacsPosixSignalHandler::~EmacsPosixSignalHandler()
{
    if( is_installed )
        removeHandler();
}

void EmacsPosixSignalHandler::installHandler()
{
    if( is_installed )
        return;

    struct sigaction action;
#if defined( __hpux__ ) && !defined(__GNUC__)
    action.sa_handler = (void (*)())signal_dispatcher;
#else
    action.sa_handler = signal_dispatcher;
#endif
    sigemptyset( &action.sa_mask );
    action.sa_flags = SA_NOCLDSTOP;

    int status = sigaction( sig_to_handle, &action, &previous_action );
    if( status != 0 )
    {
        _dbg_msg( FormatString("Failed to install signal handler for %d errno %d")
            << sig_to_handle << errno );
        return;
    }

    emacs_assert( active_handlers[sig_to_handle] == NULL );
    active_handlers[sig_to_handle] = this;

    is_installed = true;
}

void EmacsPosixSignalHandler::removeHandler()
{
    if( !is_installed )
        return;

    int status = sigaction( sig_to_handle, &previous_action, NULL );
    if( status != 0 )
        _dbg_msg( FormatString("Failed to remove signal handler for %d errno %d")
            << sig_to_handle << errno );

    emacs_assert( active_handlers[sig_to_handle] == this );

    active_handlers[sig_to_handle] = NULL;
    is_installed = false;
}

void EmacsPosixSignalHandler::signal_dispatcher( int the_signal )
{
    EmacsPosixSignalHandler *handler = active_handlers[ the_signal ];

    emacs_assert( handler != NULL );
    emacs_assert( handler->is_installed );

    handler->signalHandler();
}

//
//
//    EmacsPosixSignal
//
//
EmacsPosixSignal::EmacsPosixSignal( int _sig )
    : restore_signal( true )
    , sig( _sig )
{
    int status = sigprocmask( SIG_SETMASK, NULL, &signal_set );
    if( status != 0 )
        _dbg_msg( "Unable to get signal state" );
}

EmacsPosixSignal::~EmacsPosixSignal()
{
    if( restore_signal )
    {
        int status = sigprocmask( SIG_SETMASK, &signal_set, NULL );
        if( status != 0 )
            _dbg_msg( "Unable to restore signal state" );
    }
}

void EmacsPosixSignal::blockSignal()
{
    sigset_t block;
    sigemptyset( &block );
    sigaddset( &block, sig );

    int status = sigprocmask( SIG_BLOCK, &block, NULL );
    if( status != 0 )
        _dbg_msg( FormatString("Unable to block signal %d") << sig );
}

void EmacsPosixSignal::permitSignal()
{
    sigset_t unblock;
    sigemptyset( &unblock );
    sigaddset( &unblock, sig );

    int status = sigprocmask( SIG_UNBLOCK, &unblock, NULL );
    if( status != 0 )
        _dbg_msg( FormatString("Unable to block signal %d") << sig );
}

void EmacsPosixSignal::defaultSignalAction()
{
    struct sigaction signal_action;

    // set handler
    signal_action.sa_handler = SIG_DFL;
    // set mask
    sigemptyset( &signal_action.sa_mask );
    sigaddset( &signal_action.sa_mask, sig );
    // set flags
    signal_action.sa_flags = 0;

    int status = sigaction( sig, &signal_action, NULL );
    if( status != 0 )
        _dbg_msg( FormatString("Unable to set default action for signal %d") << sig );
}

void EmacsPosixSignal::ignoreSignalAction()
{
    struct sigaction signal_action;

    // set handler
    signal_action.sa_handler = SIG_IGN;
    // set mask
    sigemptyset( &signal_action.sa_mask );
    sigaddset( &signal_action.sa_mask, sig );
    // set flags
    signal_action.sa_flags = 0;

    int status = sigaction( sig, &signal_action, NULL );
    if( status != 0 )
        _dbg_msg( FormatString("Unable to set ignore action for signal %d") << sig );
}
