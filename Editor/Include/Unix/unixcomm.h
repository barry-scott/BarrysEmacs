//
//    unixcomm.h
//
//    Copyright (c) 1994 Barry A. Scott
//
#include <emacs_proc.h>    // for EmacsProcessCommon

typedef unsigned int EmacsPollFdId;
typedef void *EmacsPollFdParam;
typedef void (*EmacsPollFdCallBack)( EmacsPollFdParam param, int fd );
enum EmacsPollMask
{
    EmacsPollInputReadMask = 1,
    EmacsPollInputWriteMask = 2,
    EmacsPollInputReadWriteMask = 3
};

class ProcessChannelOutput
{
public:
    ProcessChannelOutput();
    ~ProcessChannelOutput();
    int ch_fd;              // Channel index
    int ch_count;           // Count of remaining chars in data
    bool ch_send_eof;       // Cound of control chars
    unsigned char *ch_buf;  // Base buffer containing data
    unsigned char *ch_data; // Pointer to next segment to send
};

//
// Structure records pertinent information about channels open.
// There is one channel associated with each process.
//
class ProcessChannelInput
{
public:
    ProcessChannelInput( EmacsProcess * );
    ~ProcessChannelInput();

    int ch_fd;
    EmacsChar_t *ch_ptr;        // Pointer to next input character
    int ch_count;               // Count of characters remaining in buffer
    EmacsBufferRef ch_buffer;   // Process is bound to this buffer
    Marker ch_end_of_data_mark; // set to the end of the inserted data
    BoundName *ch_proc;         // Procedure which gets called on output
    EmacsProcess *ch_process;   // The process that owns this input channels

    enum { ch_buffer_size = 16 * 1024 };
    // reading into here
    int ch_utf8_buffer_used;
    unsigned char ch_utf8_buffer[ch_buffer_size];
    // convert into here
    EmacsChar_t ch_unicode_buffer[ch_buffer_size];

    void handleReceivedInput();
};

// Structure for information needed for each sub process started

class EmacsPosixSignal;
class EmacsProcess : public EmacsProcessCommon
{
public:
    EmacsProcess( const EmacsString &name, const EmacsString &command );
    virtual ~EmacsProcess();

    bool startProcess( EmacsPosixSignal &sig );
    void stopProcess();
    static void flushProcess( EmacsProcess *process );
    static EmacsProcess *findProcess( const EmacsString &name );
    static EmacsProcess *getNextProcess(void);
    static int countProcesses(void);
    inline bool activeProcess(void);
    static void killProcesses( void );

    static EmacsProcess *current_process;   // the one that we are current dealing with

    EMACS_OBJECT_FUNCTIONS( EmacsProcess )
    ProcessChannelInput chan_in;            // input channel
    ProcessChannelOutput chan_out;          // output channel
    EmacsString command;                    // command that started process
    BoundName *term_proc;                   // Terminate procedure
    EmacsPollFdId in_id;                    // Input event
    EmacsPollFdId out_id;                   // Output event
    pid_t p_id;                             // process id (group id as well
    char p_flag;                            // RUNNING, STOPPED, etc
    char p_reason;                          // signal causing p_flag
    bool out_id_valid;                      // True if out_id is valid
};

extern const char *shell(void);

extern EmacsPollFdId add_to_select( int fd, long int mask, EmacsPollFdCallBack input_request, EmacsProcess *npb );
extern void remove_input( EmacsPollFdId id );

extern EmacsPollFdId add_select_fd( int fd, long int mask, EmacsPollFdCallBack cb, EmacsPollFdParam p );
extern void remove_select_fd( EmacsPollFdId id );
