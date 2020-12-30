#include    <unistd.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <errno.h>
#include    <fcntl.h>
#include    <grp.h>
#include    <string.h>

#if defined( __linux__ )
static const char *pty_first_letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
#else
static const char *pty_first_letters = "pqrstuvwxyzPQRST";
#endif
static const char *pty_second_letters = "0123456789abcdef";

int ptym_open( char *pts_name )
{
    strcpy( pts_name, "/dev/ptyXY" );
    // array index: 0123456789 (for references in following code)
    for( const char *ptr1 = pty_first_letters; *ptr1 != 0; ptr1++ )
    {
        pts_name[8] = *ptr1;
        for( const char *ptr2 = pty_second_letters; *ptr2 != 0; ptr2++ )
        {
            pts_name[9] = *ptr2;

            // try to open master
            int fdm = open( pts_name, O_RDWR|O_NONBLOCK );
            if( fdm < 0 )
            {
                if( errno == ENOENT )   // different from EIO
                    return -1;          // out of pty devices
                else
                    continue;           // try next pty device
            }

            pts_name[5] = 't';          // change "pty" to "tty"
            return fdm;                 // got it, return fd of master
        }
    }

    return -1;                          // out of pty devices
}

int ptys_open( int fdm, char *pts_name )
{
    struct group *grptr = getgrnam( "tty" );
    int gid = grptr != NULL ? grptr->gr_gid : -1;

    // following two functions don't work unless we're root
    chown(pts_name, getuid(), gid);
    chmod(pts_name, S_IRUSR | S_IWUSR | S_IWGRP);

    int fds = open(pts_name, O_RDWR|O_NONBLOCK);
    if( fds < 0 )
    {
        close( fdm );
        return -1;
    }

    return fds;
}
