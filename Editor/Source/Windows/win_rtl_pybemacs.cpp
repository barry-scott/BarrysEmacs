//
//    win_rtl_pybemacs.cpp
//
//    All the windows support functions for emacs live in here
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

extern EmacsString env_emacs_user;
extern EmacsString env_emacs_library;
extern EmacsString env_emacs_doc;

int win_emacs_quit;
int is_windows_nt;

unsigned long main_thread_id;

void EmacsInitialisation::os_specific_init()
{
}

void _dbg_msg( const EmacsString &msg )
{
    int call_depth = _dbg_fn_trace::callDepth();
    EmacsString msg2;

    if( call_depth < 40 )
    {
        while( call_depth > 0 )
        {
            msg2.append( "| " );
            call_depth--;
        }
    }
    else
    {
        msg2.append( FormatString( "[%d] | " ) << call_depth );
    }

    msg2.append( msg );
    if( msg[-1] != '\n' )
    {
         msg2.append( '\n' );
    }
    // and log to debug terminal

    fwrite( msg2.sdata(), sizeof( char ), strlen( msg2.sdata() ), stderr );
}

void DebugPrintf( const EmacsString &text )
{
    EmacsString buf( text );

    if( buf[-1] != '\n' )
        buf.append( "\n" );

    static FILE *debug_file = NULL;

    if( debug_file == NULL )
    {
        char *filename = getenv( "BEMACS_DEBUG_LOG" );
        if( filename != NULL )
            debug_file = fopen( filename, "w" );
    }

    if( debug_file != NULL )
    {
        fputs( buf.sdata(), debug_file );
        fflush( debug_file );
    }
}

void emacs_sleep( int milli_seconds )
{
    //Sleep( milli_seconds );
}

EmacsString get_tmp_path(void)
{
    wchar_t w_buf[MAXPATHLEN];
    int w_len = GetTempPath( MAXPATHLEN, w_buf);
    return EmacsString( w_buf, w_len );
}

EmacsString get_config_env( const EmacsString &name )
{
    char *value = getenv( name.sdata() );

    if( value != NULL )
        return value;

    static EmacsString env_emacs_path( "emacs_user:;emacs_library:" );
    if( name == "emacs_path" )
        return env_emacs_path;

    if( name == "emacs_user" )
        return env_emacs_user;

    if( name == "emacs_library" )
        return env_emacs_library;

    if( name == "emacs_doc" )
        return env_emacs_doc;

    static EmacsString env_emacs_journal( ".\\" );
    if( name == "emacs_journal" )
        return env_emacs_journal;

    static EmacsString env_sys_login( "HOME:/" );
    if( name == "sys_login" )
        return env_sys_login;

    return EmacsString::null;
}

int put_config_env( const EmacsString &name, const EmacsString &value )
{
    //putenv( name.sdata(), value.sdata() );
    return 0;
}

EmacsString get_device_name_translation( const EmacsString &name )
{
    return get_config_env( name );
}

int get_file_parsing_override( const char *disk, int def_override )
{
    return def_override;
}

void debug_invoke(void)
{
    return;
}

void debug_exception(void)
{
    return;
}


void fatal_error( int code )
{
    printf("\nFatal Error %d\n", code );
    exit(1);
}

#if DBG_SER
void debug_SER(void)
{ return; }
#endif

#if defined(WIN32)
#if !defined(MAX_USERNAME_LENGTH)
# define MAX_USERNAME_LENGTH 32
#endif


EmacsString users_login_name()
{
    wchar_t w_buf[MAX_USERNAME_LENGTH];
    DWORD w_len = sizeof( w_buf )/sizeof( wchar_t );

    if( !GetUserName( w_buf, &w_len ) )
    {
        return EmacsString::null;
    }
    else
    {
        return EmacsString( w_buf, w_len );
    }
}

#elif defined(_MSDOS)
char *user_login_name( char * i )
{
    return NULL;
}
#else
#error "Need cuserid logic..."
#endif

EmacsString get_user_full_name()
{
    return users_login_name();
}

EmacsString get_system_name()
{
    wchar_t w_buf[MAX_COMPUTERNAME_LENGTH+1];
    DWORD w_len = MAX_COMPUTERNAME_LENGTH;
    w_buf[0] = '\0';
    if( !GetComputerName( w_buf, &w_len ) )
    {
        return EmacsString::null;
    }
    else
    {
        return EmacsString( w_buf, w_len );
    }
}

int interlock_dec( volatile int *counter )
{
    return InterlockedDecrement( (long *)counter );
}

int interlock_inc( volatile int *counter )
{
    return InterlockedIncrement( (long *)counter );
}

void conditional_wake(void)
{
    return;
}

EmacsDateTime EmacsDateTime::now(void)
{
    EmacsDateTime now;

    static double epoch = 0;

    if( epoch == 0 )
    {
        SYSTEMTIME s_epoch;
        s_epoch.wYear = 1970;
        s_epoch.wMonth = 1;
        s_epoch.wDayOfWeek = 0;
        s_epoch.wDay = 1;
        s_epoch.wHour = 0;
        s_epoch.wMinute = 0;
        s_epoch.wSecond = 0;
        s_epoch.wMilliseconds = 0;

        FILETIME f_epoch;

        SystemTimeToFileTime( &s_epoch, &f_epoch );

        // a file time is in 100nS units
        epoch = double( f_epoch.dwHighDateTime );
        epoch *= 65536;
        epoch *= 65536;
        epoch += double( f_epoch.dwLowDateTime );
        epoch /= 10000000.0;
    }

    FILETIME file_time;
    GetSystemTimeAsFileTime( &file_time );

    // a file time is in 100nS units
    now.time_value = double( file_time.dwHighDateTime );
    now.time_value *= 65536;
    now.time_value *= 65536;
    now.time_value += double( file_time.dwLowDateTime );
    now.time_value /= 10000000.0;

    // convert to python compatible time
    now.time_value -= epoch;

    return now;
}

EmacsString os_error_code( unsigned int code )
{
    char msg[1024];

    int size = FormatMessage
        (
        FORMAT_MESSAGE_FROM_SYSTEM,
        NULL,
        code,
        MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
        (LPTSTR) &msg,
        sizeof( msg ) - 1,
        NULL
        );
    if( size == 0 )
        return EmacsString( FormatString( "Win32 error code: 0x%x" ) << code );

    // lose a trailing \r\n
    if( size > 0 && msg[size-1] == '\n' )
        size--;
    if( size > 0 && msg[size-1] == '\r' )
        size--;

    msg[size] = 0;

    return EmacsString( FormatString( "%s (0x%x)" ) << EmacsString(msg) << code );
}

double poll_process_delay()
{
    return 0.0;
}
