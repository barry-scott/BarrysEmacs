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
    // work around bug in GCC/G++ 2.7.2 libg++ 2.7.1 that gets
    // the size of the stat buffer wrong
    union u
    {
        struct stat stat_buf;
        char padding[512];
    }
        uu;
public:
    EmacsFileStat();
    virtual ~EmacsFileStat();

    bool stat( const char *file_name );
    bool stat( FILE *file );
    bool stat( int fd );

    const struct stat &data(void)
    { return uu.stat_buf; }
};
