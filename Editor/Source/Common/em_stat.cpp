//
//    em_stat.cpp
//
//    Copyright (c) 1996 Barry A. Scott
//     5-Feb-1996
//

#include <emacsutl.h>
#include <em_stat.h>
#if defined( __unix__ ) || defined( __WATCOMC__ )
#include <unistd.h>
#endif


EmacsFileStat::EmacsFileStat()
{
    memset( &stat_buf, 0, sizeof( stat_buf ) );
}

EmacsFileStat::~EmacsFileStat()
{ }

bool EmacsFileStat::stat( const char *file_name )
{
    int status = ::stat( file_name, &stat_buf );
    return status == 0;
}

bool EmacsFileStat::stat( FILE *file )
{
    return stat( fileno( file ) );
}

bool EmacsFileStat::stat( int fd )
{
    int status = ::fstat( fd, &stat_buf );
    return status == 0;
}

bool EmacsFileStat::is_regular()
{
    return S_ISREG( stat_buf.st_mode );
}

bool EmacsFileStat::is_directory()
{
    return S_ISDIR( stat_buf.st_mode );
}
