//
//    em_stat.h
//
//    Copyright (c) 1996 Barry A. Scott
//     5-Feb-1996
//
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

class EmacsFileStat
{
private:
    struct stat stat_buf;

public:
    EmacsFileStat();
    virtual ~EmacsFileStat();

    bool stat( const char *file_name );
    bool stat( FILE *file );
    bool stat( int fd );

    bool is_regular();
    bool is_directory();

    const struct stat &data(void)
    {
        return stat_buf;
    }
};
