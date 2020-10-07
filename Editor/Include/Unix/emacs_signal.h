
//
//    emacs_signal.h
//
#include <signal.h>

class EmacsPosixSignalHandler
{
public:
    EmacsPosixSignalHandler( int sig_to_handle );
    virtual ~EmacsPosixSignalHandler();

    void installHandler();
    void removeHandler();

protected:
    virtual void signalHandler() = 0;
private:

    bool is_installed;
    int sig_to_handle;
    struct sigaction previous_action;

    static void signal_dispatcher( int the_signal );
    enum {MAX_SIG_HANDLED = 30};
    static EmacsPosixSignalHandler *active_handlers[MAX_SIG_HANDLED];
};

//
//    Allow a signal to be manipulated
//
class EmacsPosixSignal
{
public:
    EmacsPosixSignal( int sig );
    virtual ~EmacsPosixSignal();
    void blockSignal();
    void permitSignal();
    void defaultSignalAction();
    void ignoreSignalAction();
    void doNotRestore() { restore_signal = false; }
    void doRestore() { restore_signal = true; }
private:
    bool restore_signal;    // true if the destructor must restore the signal state
    int sig;
    sigset_t signal_set;
};

