//
//    emacs_init.cpp
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

bool EmacsInitialisation::init_done = false;
time_t EmacsInitialisation::most_recent_built_module = 0;

EmacsString version_string;

extern EmacsDateTime emacs_start_time;

EmacsInitialisation::EmacsInitialisation( const char *date_and_time, const char * /*file_name*/ )
{
    char month_str[80];
    struct tm time_parts;
    memset( &time_parts, 0, sizeof( time_parts ) );

    time_parts.tm_isdst = -1;    // mktime must work out if DST is active or not

    sscanf( date_and_time, "%s %d %d %d:%d:%d",
                month_str, &time_parts.tm_mday, &time_parts.tm_year,
                &time_parts.tm_hour, &time_parts.tm_min, &time_parts.tm_sec );
    time_parts.tm_year -= 1900;

    if( strcmp( month_str, "Jan" ) == 0 ) time_parts.tm_mon = 0;
    else if( strcmp( month_str, "Feb" ) == 0 ) time_parts.tm_mon = 1;
    else if( strcmp( month_str, "Mar" ) == 0 ) time_parts.tm_mon = 2;
    else if( strcmp( month_str, "Apr" ) == 0 ) time_parts.tm_mon = 3;
    else if( strcmp( month_str, "May" ) == 0 ) time_parts.tm_mon = 4;
    else if( strcmp( month_str, "Jun" ) == 0 ) time_parts.tm_mon = 5;
    else if( strcmp( month_str, "Jul" ) == 0 ) time_parts.tm_mon = 6;
    else if( strcmp( month_str, "Aug" ) == 0 ) time_parts.tm_mon = 7;
    else if( strcmp( month_str, "Sep" ) == 0 ) time_parts.tm_mon = 8;
    else if( strcmp( month_str, "Oct" ) == 0 ) time_parts.tm_mon = 9;
    else if( strcmp( month_str, "Nov" ) == 0 ) time_parts.tm_mon = 10;
    else if( strcmp( month_str, "Dec" ) == 0 ) time_parts.tm_mon = 11;
    else
        emacs_assert(0);
    time_t build_time = mktime( &time_parts );

    if( build_time > most_recent_built_module )
    {
        // remember the newest modules date and time
        most_recent_built_module = build_time;
    }
#if 0
{
    printf("           Module time string:     %s\n", date_and_time );
    printf("                   Build time: %s", ctime( &build_time ) );
    printf("Most recent built module time: %s", ctime( &most_recent_built_module ) );
}
#endif

    if( !init_done )
    {
        init_done = true;

        // allow for OS quirks
        os_specific_init();

        // get emacs memory system up and running
        init_memory();

        // create the null string
        EmacsString::init();

        emacs_start_time = EmacsDateTime::now();
    }
}

EmacsInitialisation::~EmacsInitialisation()
{ }

void EmacsInitialisation::setup_version_string()
{
    const char *build_time_string = ctime( &most_recent_built_module );
    // build a version string with that data and time in it
    version_string = EMACS_VERSION " of Emacs Copyright Barry A. Scott (c) ";
    version_string.append( EmacsString( EmacsString::copy, u_str(build_time_string), strlen( build_time_string )-1 ) );
}
