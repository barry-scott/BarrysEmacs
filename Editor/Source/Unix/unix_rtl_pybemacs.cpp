//
//    unix_rtl.cpp for Emacs V8.0
//    Copyright (c) 1993-2010 Barry A. Scott
//

#include <emacs.h>
#include <sys/utsname.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

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

static struct timeval emacs_start_time;

const int TIMER_TICK_VALUE( 50 );
static void( *timeout_handler )(void );
struct timeval timeout_time;

void time_schedule_timeout( void( *time_handle_timeout )(void ), const EmacsDateTime &time  )
{
    struct timezone tzp;
    gettimeofday( &timeout_time, &tzp  );
    double delta = time.asDouble();

    timeout_time.tv_sec += int(delta);
    timeout_time.tv_usec += int(delta*1000000)%1000000;
    if( timeout_time.tv_usec > 1000000  )
    {
        timeout_time.tv_sec += 1;
        timeout_time.tv_usec -= 1000000;
    }
    timeout_handler = time_handle_timeout;
}

void time_cancel_timeout(void)
{
    timeout_time.tv_sec = 0;
    timeout_time.tv_usec = 0;
    timeout_handler = NULL;
}

void wait_abit(void)
{
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
    if( dbg_flags&DBG_SYSLOG )
    {
        syslog( LOG_DEBUG, "%s", msg.sdata() );
    }
    else
    {
        fprintf( stderr,"%s", msg.sdata() );
        if( msg[-1] != '\n' )
            fprintf( stderr, "\n" );
        fflush( stderr );
    }
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
    printf( "\nFatal Error %d\n", code );
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


int elapse_time()
{
    struct timeval now;
    gettimeofday( &now, NULL );

    //
    //    calculate the time since startup in mSec.
    //    we ignore the usec part of the start time
    //    (assuming its 0)
    //
    int elapse_time = (int)(now.tv_sec - emacs_start_time.tv_sec);
    elapse_time *= 1000;
    elapse_time += (int)(now.tv_usec/1000);

    return elapse_time;
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

    gettimeofday( &emacs_start_time, NULL );
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

EmacsString EmacsDateTime::asString(void) const
{
    double int_part, frac_part;

    frac_part = modf( time_value, &int_part );
    frac_part *= 1000.0;

    time_t clock = int( int_part );
    int milli_sec = int( frac_part );

    struct tm *tm = localtime( &clock );

    return FormatString("%4d-%2d-%2d %2d:%2d:%2d.%3.3d")
        << tm->tm_year + 1900 << tm->tm_mon + 1 << tm->tm_mday
        << tm->tm_hour << tm->tm_min << tm->tm_sec << milli_sec;
}

EmacsString os_error_code( unsigned int code )
{
    const char *error_string = strerror( code );
    if( error_string == NULL )
        return EmacsString( FormatString("Unix error code %d") << code );
    else
        return EmacsString( error_string );
}

#undef NDEBUG
#include <assert.h>
void _emacs_assert( const char *exp, const char *file, unsigned line )
{
#if defined( __FreeBSD__ )
    // freebsd assert order
    __assert( "unknown", file, line, exp );

#elif defined( __GNUC__ ) && __GNUC__ >= 3
    // unix assert order
    __assert( exp, file, line );

#else
    // unix assert order
    __assert( file, line, exp );

#endif
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
