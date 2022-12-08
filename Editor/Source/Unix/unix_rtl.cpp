//
//    unix_rtl.cpp for Emacs V7.0
//    Copyright (c) 1993-2003 Barry A. Scott
//

#include <emacs.h>
#include <sys/utsname.h>

#include <unixcomm.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <unistd.h>

#include <mem_man.h>

#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <pwd.h>
// #undef _POSIX_SOURCE
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <sys/select.h>

//#define exception math_exception
//#include <exception>
#include <math.h>

static struct timeval emacs_start_time;

static fd_set readfds, writefds, excepfds;
static fd_set readfds_resp, writefds_resp;

struct fd_info
{
    EmacsPollFdParam param;
    EmacsPollFdCallBack cb;
};

static struct fd_info read_info[FD_SETSIZE], write_info[FD_SETSIZE];
static int fd_max = 0;
static EmacsString unix_path;
EmacsString name_arg;

static void process_args( int argc, char **argv );
static bool isFileDescriptorOpen( int fd );

static EmacsString restore_arg;
static EmacsString package_arg;

int init_gui_terminal( const EmacsString & )
{
    return 0;
}

void UI_update_window_title( void )
{ }

static void( *timeout_handler )(void );
struct timeval timeout_time;

void time_schedule_timeout( void (*time_handle_timeout)( void ), int delta  )
{
    struct timezone tzp;
    gettimeofday( &timeout_time, &tzp  );

    timeout_time.tv_sec += delta/1000;
    timeout_time.tv_usec +=( delta%1000 )*1000;
    if( timeout_time.tv_usec > 1000000  )
    {
        timeout_time.tv_sec += 1;
        timeout_time.tv_usec -= 1000000;
    }
    timeout_handler = time_handle_timeout;
}

// stub for API used in unixcomm for pybemacs
void poll_process_fds()
{
}

void wait_abit(void)
{
    static struct timeval tmo = {0, 100000};
    fd_set rfds;
    int fd;
#if defined( SUBPROCESSES )
    extern fd_set process_fds;

    memcpy( &rfds, &process_fds, sizeof( fd_set ) );
#else
    FD_ZERO( &rfds );
#endif

    fd = 1<<0;
    FD_SET( fd, &rfds );

    if( select( FD_SETSIZE, &rfds, NULL, NULL, &tmo ) )
    {
        wait_for_activity();
    }
    return;
}

void debug_invoke(void)
{
    return;
}

void debug_SER(void)
{
    return;
}

void debug_exception(void)
{
    return;
}

void _dbg_msg( const EmacsString &msg )
{
    fprintf( stderr, "%s", msg.sdata() );
    if( msg[-1] != '\n' )
    {
        fprintf( stderr, "\n" );
    }
    fflush( stderr );
}

int win_emacs_quit = 0;

int wait_for_activity(void)
{
    unsigned char utf8_buf[128];
    int used = 0;

    for(;;)
    {
#if defined( SUBPROCESSES )
        if( child_changed )
        {
            change_msgs();
        }
#endif

        int size = theActiveView->k_input_event( utf8_buf, sizeof( utf8_buf )-used );
        if( size < 0 )
        {
            return -1;
        }

        used += size;
        if( used >= length_utf8_code_point( utf8_buf[0] ) )
        {
            break;
        }
    }

    EmacsChar_t uni_buf[128];
    int uni_len = length_utf8_to_unicode( used, utf8_buf );
    convert_utf8_to_unicode( utf8_buf, uni_len, uni_buf );

    if( uni_len >= 1 )
    {
        for( int i=0; i<uni_len; i++ )
        {
            theActiveView->k_input_char( uni_buf[i], false );
        }
        return 1;
    }

    if( win_emacs_quit )
    {
        win_emacs_quit = 0;
        return -1;
    }

    return 0;
}

int interlock_dec( volatile int *cell )
{
    (*cell)--;
    if( *cell == 0 )
    {
        return 0;
    }
    if( *cell < 0 )
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

int interlock_inc( volatile int *cell )
{
    (*cell)++;
    if( *cell == 0 )
    {
        return 0;
    }
    if( *cell < 0 )
    {
        return -1;
    }
    else
    {
        return 1;
    }
}

void conditional_wake(void)
{
    return;
}

EmacsString get_user_full_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid( uid );

    if( pw == NULL )
    {
        return EmacsString::null;
    }

    return EmacsString( pw->pw_gecos );
}

EmacsString users_login_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid( uid );

    if( pw == NULL )
    {
        return EmacsString::null;
    }

    return EmacsString( pw->pw_name );
}

EmacsString get_system_name()
{
    char system_name[256];
    if( gethostname( system_name, sizeof( system_name ) ) == 0 )
    {
        return EmacsString( system_name );
    }

    return EmacsString::null;
}

void fatal_error( int code )
{
    printf("\nFatal Error %d\n", code );
    exit( 1 );
}

int put_config_env( const EmacsString &name, const EmacsString &value )
{
    return setenv( name, value, 1 );
}

EmacsString image_path;

static void calcImagePath( EmacsString argv0 )
{
    unix_path = getenv( "PATH" );
    for( int i=0; i<unix_path.length(); i++ )
    {
        if( unix_path[i] == ':' )
        {
            unix_path[i] = PATH_SEP;
        }
    }

    {
        EmacsFile image;
        if( !image.fio_find_using_path( unix_path, argv0, EmacsString::null ) )
        {
            _dbg_msg( "Emacs is unable to find itself!\n" );
            return;
        }
        image_path = image.fio_getname();
    }

    // find the stat of the path and don't hide links
    struct stat stat_buf;
    if( lstat( (const char *)image_path, &stat_buf ) == 0
    // its a symbolic link
    && S_ISLNK( stat_buf.st_mode ) )
    {
        char link_path[MAXPATHLEN+1];
        // get the value of the link
        int size = readlink( image_path.sdata(), link_path, MAXPATHLEN );
        if( size != -1 )
        {
            link_path[size] = '\0';

            // printf( "The sym link is %s\n", link_path );
            // if its an absolute path just replace image_path
            if( link_path[0] == '/' )
            {
                image_path = link_path;
            }
            else
            {
                // add the link to the end of the image_path less the last file name
                int pos = image_path.last( PATH_CH );
                if( pos > 0 )
                {
                    image_path.remove( pos+1 );
                    image_path.append( link_path );
                }
            }
        }

        // printf( "New image path is %s\n", image_path.data() );
    }

    int pos = image_path.last( PATH_CH );
    if( pos < 0 )
    {
        return;
    }

    image_path.remove( pos );
}

void init_unix_environ( const char *argv0 )
{
    calcImagePath( argv0 );
}

EmacsString env_emacs_user;
#if defined( BEMACS_LIB_DIR )
EmacsString env_emacs_library( BEMACS_LIB_DIR );
#else
EmacsString env_emacs_library;
#endif

#if defined( BEMACS_DOC_DIR )
EmacsString env_emacs_doc( BEMACS_DOC_DIR );
#else
EmacsString env_emacs_doc;
#endif

EmacsString get_config_env( const EmacsString &name )
{
    char *value = getenv( name );

    if( value != NULL )
        return value;

    static EmacsString env_emacs_path(  "emacs_user: emacs_library:" );
    if( name == "emacs_path" )
        return env_emacs_path;

    if( name == "emacs_user" )
    {
        if( env_emacs_user.isNull() )
        {
            char *home = getenv( "HOME" );
            if( home != NULL )
            {
                env_emacs_user = FormatString("%s/bemacs") << home;
            }
        }
        return env_emacs_user;
    }

    if( name == "emacs_library" )
    {
        if( env_emacs_library.isNull() )
        {
            // only expect this code to run for macOS
            expand_and_default( FormatString( "%s/../emacs_library" ) << image_path, EmacsString::null, env_emacs_library );
        }
        return env_emacs_library;
    }

    if( name == "emacs_doc" )
    {
        if( env_emacs_doc.isNull() )
        {
            // only expect this code to run for macOS
            expand_and_default( FormatString( "%s/../documentation" ) << image_path, EmacsString::null, env_emacs_library );
        }
        return env_emacs_doc;
    }

    static EmacsString env_sys_login(  "HOME:/" );
    if( name == "sys_login" )
        return env_sys_login;

    return EmacsString::null;
}

EmacsPollFdId add_select_fd( int fd, long int mask, EmacsPollFdCallBack cb, EmacsPollFdParam p )
{
    EmacsPollFdId resp = 0;

    if( fd < FD_SETSIZE )
    {
        if( fd > fd_max )
        {
            fd_max = fd;
        }
        if( mask & EmacsPollInputReadMask )
        {
            read_info[fd].param = p;
            read_info[fd].cb = cb;
            FD_SET( fd, &readfds );
            resp = fd << 8;
        }
        if( mask & EmacsPollInputWriteMask )
        {
            write_info[fd].param = p;
            write_info[fd].cb = cb;
            FD_SET( fd, &writefds );
            resp |= fd << 16;
        }
    }
    else
    {
        fatal_error( 314 );
    }
    return resp;
}

void remove_select_fd( EmacsPollFdId id )
{
    int fd = 0;

    if( id & 0xff00 )
    {
        fd = (int)((id >> 8) & 0xff);
        read_info[fd].param = NULL;
        read_info[fd].cb = NULL;
        FD_CLR( fd, &readfds );
    }

    if( id & 0xff0000 )
    {
        fd = (int)((id >> 16) & 0xff);
        write_info[fd].param = NULL;
        write_info[fd].cb = NULL;
        FD_CLR( fd, &writefds );
    }

    if( fd == fd_max )
    {
        for( int i = fd_max; i >= 0; i-- )
        {
            if( read_info[fd].cb != NULL || write_info[fd].cb != NULL )
            {
                fd_max = i;
                break;
            }
            if( i < 0 )
            {
                fd_max = 0;
            }
        }
    }
}

int read_inputs( int fd, unsigned char *buf, unsigned int count )
{
    int status;

    do
    {
        memcpy( &readfds_resp, &readfds, sizeof( fd_set ) );
        memcpy( &writefds_resp, &writefds, sizeof( fd_set ) );
        FD_SET( fd, &readfds_resp );
        status = select( FD_SETSIZE, &readfds_resp, &writefds_resp, &excepfds, 0 );
    }
    while (status < 0 && errno == EINTR);

    //_dbg_msg( FormatString("read_inputs( %d, buf, %d ) => status = %d") << fd << count << status );

    for( int fd_scan = 1; fd_scan <= fd_max; fd_scan++ )
    {
        //_dbg_msg( FormatString("fd %d read %d write %d")
        //      << i << FD_ISSET( i, &readfds_resp ) << FD_ISSET( i, &writefds_resp ) );
        if( read_info[fd_scan].cb != NULL
        && FD_ISSET( fd_scan, &readfds_resp ) )
        {
            read_info[fd_scan].cb( read_info[fd_scan].param, fd_scan );
        }
        if( write_info[fd_scan].cb != NULL
        && FD_ISSET( fd_scan, &writefds_resp ) )
        {
            write_info[fd_scan].cb( write_info[fd_scan].param, fd_scan );
        }
    }

    if( FD_ISSET( fd, &readfds_resp ) )
    {
        //_dbg_msg( FormatString("read_inputs() calling read( %d, ... )") << fd );
        return read( fd, buf, count );
    }

    return 0;
}

void OutputDebugString( const char *message )
{
    printf( "Debug: %s\n", message );
}

extern void init_memory();

void EmacsInitialisation::os_specific_init()
{
    init_memory();
#ifdef SAVE_ENVIRONMENT
    //
    //    Create the save environment object at the earlest opertunity
    //
    EmacsSaveRestoreEnvironmentObject = EMACS_NEW EmacsSaveRestoreEnvironment;
#endif
}

#if 0
//
//    Emacs server code
//

static EmacsString server_fifo;
static EmacsString client_fifo;

static EmacsPollFdId emacs_server_input_id;
static int emacs_server_read_fd = -1;
static int emacs_server_write_fd = -1;

class EmacsServerWorkItem : public EmacsWorkItem
{
public:
    EmacsServerWorkItem()
    : EmacsWorkItem()
    , bytes_read(0)
    {}
    virtual ~EmacsServerWorkItem()
    {}

    // routine to read a command from the FIFO
    void readCommand( int fd );
private:
    virtual void workAction(void);

    // used to hold the size of the
    int bytes_read;
    // max read size
    enum { SERVER_BUFFER_SIZE = 16384 };
    // the command string that was read
    unsigned char command_string[SERVER_BUFFER_SIZE];
};

EmacsServerWorkItem emacs_server_work_item;

EmacsCommandLineServerWorkItem emacs_command_line_work_item;


extern TerminalControl_GUI *theMotifGUI;

void EmacsServerWorkItem::workAction()
{
    int emacs_client_write_fd;

    // quit if last read failed
    if( bytes_read <= 0 )
    {
        return;
    }

    // get focus if we need it
    theMotifGUI->getKeyboardFocus();

    void *sep = memchr( command_string, 0, bytes_read );
    if( sep != NULL )
    {
        int index = (char *)sep - (char *)((void *)&command_string[0]);

        emacs_command_line_work_item.newCommandLine
            (
            EmacsString( EmacsString::copy, command_string, index ),
            EmacsString( EmacsString::copy, command_string + index + 1, bytes_read - index - 1 )
            );
    }
    else
        emacs_command_line_work_item.newCommandLine
            (
            EmacsString( EmacsString::copy, command_string, bytes_read ),
            EmacsString( EmacsString::null )
            );

    // this will fail until the client is synced up
    emacs_client_write_fd = open( client_fifo, O_WRONLY|O_NONBLOCK );
    if( emacs_client_write_fd < 0 )
    {
        return;
    }

    // send a 1 byte <space> string
    write( emacs_client_write_fd, " ", 1 );
    close( emacs_client_write_fd );
}

bool send_exit_message( const EmacsString &command )
{
    // this will fail until the client is synced up
    int emacs_client_write_fd = open( client_fifo, O_WRONLY|O_NONBLOCK );
    if( emacs_client_write_fd < 0 )
    {
        return false;
    }

    // send a 1 byte header on the response string
    EmacsString response("R");    // R for response
    response.append( command );
    write( emacs_client_write_fd, response.data(), response.length() );
    close( emacs_client_write_fd );
    return true;
}

void emacs_server_callback(EmacsPollFdParam str, int *fd, EmacsPollFdId* id )
{
    emacs_server_work_item.readCommand( *fd );
}

void EmacsServerWorkItem::readCommand( int fd )
{
    bytes_read = read( fd, command_string, sizeof( command_string ) );
    if( bytes_read > 0 )
    {
        // and schedule the work item
        addItem();
    }
}

void start_emacs_server()
{
    const char *fifo_name = getenv("BEMACS_FIFO");

    if( emacs_server_read_fd >= 0 )
        return;

    if( fifo_name == NULL )
        fifo_name = ".bemacs/.emacs_command";

    if( fifo_name[0] != '/' )
    {
        const char *name = NULL;
        const char *home = getenv( "HOME" );
        if( home != NULL )
        {
            for( const char *p = home; *p; p++ )
            {
                if( *p == '/' )
                {
                    name = p;
                }
            }
        }
        else
        {
            struct passwd *pwd = getpwuid( geteuid() );
            if( pwd == NULL )
            {
                name = "default";
            }
            else
            {
                name = pwd->pw_name;
            }
        }

        server_fifo = "/tmp/";
        server_fifo.append( name );
        server_fifo.append( "/" );
    }
    server_fifo.append( fifo_name );

    client_fifo = server_fifo;
    client_fifo.append( "_response" );

    if( !name_arg.isNull() )
    {
        server_fifo.append( "_" );
        server_fifo.append( name_arg );
        client_fifo.append( "_" );
        client_fifo.append( name_arg );
    }

    emacs_server_read_fd = open( server_fifo, O_RDONLY|O_NONBLOCK );
    if( emacs_server_read_fd < 0 )
        return;

    emacs_server_write_fd = open( server_fifo, O_WRONLY|O_NONBLOCK );
    if( emacs_server_write_fd < 0 )
        return;


    emacs_server_input_id = add_to_select( emacs_server_read_fd, EmacsPollInputReadMask, emacs_server_callback, NULL );
}

void stop_emacs_server()
{
    if( emacs_server_read_fd < 0 )
    {
        return;
    }

    remove_input( emacs_server_input_id );
    close( emacs_server_read_fd );
    close( emacs_server_write_fd );
}

bool emacs_internal_init_done_event(void)
{
    start_emacs_server();

    return true;
}
#else
bool emacs_internal_init_done_event(void)
{
    return true;
}
#endif

static bool isFileDescriptorOpen( int fd )
{
    // get the close on exec flag
    int status = fcntl( fd, F_GETFD, 0 );
    // if that fails the FD is not open
    return status != -1;
}

EmacsDateTime EmacsDateTime::now(void)
{
    EmacsDateTime now;

    struct timeval t;
    gettimeofday(  &t, NULL );

    now.time_value = double( t.tv_usec ) / 1000000.0;
    now.time_value += double( t.tv_sec );

    return now;
}

EmacsString os_error_code( unsigned int code )
{
    const char *error_string = strerror( code );
    if( error_string == NULL )
    {
        return EmacsString( FormatString("Unix error code %d") << code );
    }

    return EmacsString( error_string );
}

void emacs_sleep( int milli_seconds )
{
    struct timespec request;
    request.tv_sec = milli_seconds/1000;        // seconds
    request.tv_nsec = (milli_seconds%1000)*1000000;    // convert milli to nano
    int rc = nanosleep( &request, NULL );
    if( rc == 0 )
    {
        return;
    }
    emacs_assert( errno == EINTR );
}
