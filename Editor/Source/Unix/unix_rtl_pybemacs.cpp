//
//    unix_rtl_pybemacs.cpp for Emacs V8.0
//    Copyright (c) 1993-2010 Barry A. Scott
//

#include <emacs.h>
#include <sys/utsname.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if DBG_PROCESS && DBG_TMP
extern int elapse_time(void);
# define Trace( s )  do { \
                        if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP ) { \
                            int t=elapse_time(); \
                            _dbg_msg( FormatString("%d.%03.3d " "%s") << t/1000 << t%1000 << (s) ); } \
                    } while( false )
#else
# define Trace( s ) // do nothing
#endif

#include <syslog.h>

#include <mem_man.h>

#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <pwd.h>

#include <unistd.h>
#include <assert.h>
#include <time.h>

#include <fcntl.h>
#include <sys/stat.h>

#include <math.h>

extern EmacsString env_emacs_user;
extern EmacsString env_emacs_library;
extern EmacsString env_emacs_doc;

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

int interlock_dec( volatile int *cell )
{
    (*cell)--;
    if( *cell == 0 )
        return 0;
    if( *cell < 0 )
        return -1;
    else
        return 1;
}

int interlock_inc( volatile int *cell )
{
    (*cell)++;
    if( *cell == 0 )
        return 0;
    if( *cell < 0 )
        return -1;
    else
        return 1;
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
        return EmacsString::null;
    else
        return EmacsString( pw->pw_gecos );
}

EmacsString users_login_name()
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid( uid );

    if( pw == NULL )
        return EmacsString::null;

    return EmacsString( pw->pw_name );
}

EmacsString get_system_name()
{
    char system_name[256];
    if( gethostname( system_name, sizeof( system_name ) ) == 0 )
        return EmacsString( system_name );
    else
        return EmacsString::null;
}

void fatal_error( int code )
{
    printf( "\nBarry's Emacs Fatal Error %d\n", code );
    exit( 1 );
}

int put_config_env( const EmacsString &name, const EmacsString &value )
{
    return setenv( name.sdata(), value.sdata(), 1 );
}

EmacsString get_config_env( const EmacsString &name )
{
    char *value = getenv( name.sdata() );

    if( value != NULL )
        return value;

    static EmacsString env_emacs_path( "emacs_user: emacs_library:" );
    if( name == "emacs_path" )
        return env_emacs_path;

    if( name == "emacs_user" )
        return env_emacs_user;

    if( name == "emacs_library" )
        return env_emacs_library;

    if( name == "emacs_doc" )
        return env_emacs_doc;

    static EmacsString env_emacs_journal( "./" );
    if( name == "emacs_journal" )
        return env_emacs_journal;

    static EmacsString env_sys_login( "HOME:/" );
    if( name == "sys_login" )
        return env_sys_login;

    return EmacsString::null;
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

//
//    Emacs server code
//
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
        return EmacsString( FormatString("Unix error code %d") << code );
    else
        return EmacsString( error_string );
}

void emacs_sleep( int milli_seconds )
{
    struct timespec request;
    request.tv_sec = milli_seconds/1000;        // seconds
    request.tv_nsec = (milli_seconds%1000)*1000000;    // convert milli to nano
    int rc = nanosleep( &request, NULL );
    if( rc == 0 )
        return;
    emacs_assert( errno == EINTR );
}

#if defined( SUBPROCESSES )
#include "unixcomm.h"

static fd_set readfds;
static fd_set writefds;
static fd_set excepfds;
static fd_set readfds_resp;
static fd_set writefds_resp;

struct fd_info
{
    EmacsPollFdParam param;
    EmacsPollFdCallBack cb;
};

static struct fd_info read_info[FD_SETSIZE];
static struct fd_info write_info[FD_SETSIZE];
static int fd_max = -1;

EmacsPollFdId add_select_fd( int fd, long int mask, EmacsPollFdCallBack cb, EmacsPollFdParam p )
{
    Trace( FormatString("add_select_fd( %d, 0x%x, ...)") << fd << mask );
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
        Trace( FormatString("remove_select_fd: clear read fd %d") << fd );
    }

    if( id & 0xff0000 )
    {
        fd = (int)((id >> 16) & 0xff);
        write_info[fd].param = NULL;
        write_info[fd].cb = NULL;
        FD_CLR( fd, &writefds );
        Trace( FormatString("remove_select_fd: clear write fd %d") << fd );
    }

    if( fd == fd_max )
    {
        Trace( FormatString("remove_select_fd: find new fd_max %d") << fd_max );
        int i = fd_max;
        fd_max = -1;
        while( i >= 0 )
        {
            Trace( FormatString("remove_select_fd: is max %d?") << i );

            if( read_info[fd].cb != NULL || write_info[fd].cb != NULL )
            {
                fd_max = i;
                break;
            }
            --i;
        }
    }
    Trace( FormatString("remove_select_fd: done fd_max %d") << fd_max );
}

double poll_process_delay()
{
    // delay 60.0s if there are no processes to monitor
    return fd_max < 0 ? 60.0 : 1.0;
}

void poll_process_fds()
{
    // see if there are any fds to poll
    if( fd_max < 0 )
        return;

    int status;
    timeval timeout = { 0, 0 };
    do
    {
        memcpy( &readfds_resp, &readfds, sizeof( fd_set ) );
        memcpy( &writefds_resp, &writefds, sizeof( fd_set ) );
        status = select( FD_SETSIZE, &readfds_resp, &writefds_resp, &excepfds, &timeout );
    }
    while (status < 0 && errno == EINTR);

    for( int fd_scan = 1; fd_scan <= fd_max; fd_scan++ )
    {
        Trace( FormatString("poll_process_fds: fd_scan %d read %d write %d")
              << fd_scan << FD_ISSET( fd_scan, &readfds_resp ) << FD_ISSET( fd_scan, &writefds_resp ) );

        if( read_info[fd_scan].cb != NULL
        && FD_ISSET( fd_scan, &readfds_resp ) )
        {
            Trace("poll_process_fds: calling read cb");
            read_info[fd_scan].cb( read_info[fd_scan].param, fd_scan );
            Trace("poll_process_fds: read cb returned");
        }

        if( write_info[fd_scan].cb != NULL
        && FD_ISSET( fd_scan, &writefds_resp ) )
        {
            Trace("poll_process_fds: calling write cb");
            write_info[fd_scan].cb( write_info[fd_scan].param, fd_scan );
            Trace("poll_process_fds: write cb returned");
        }
    }
}
#endif
