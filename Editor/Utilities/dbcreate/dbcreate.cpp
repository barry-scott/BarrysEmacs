#include <emacsutl.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
#include <unistd.h>
#include <limits.h>
#ifdef MAXPATHLEN
#define _MAX_PATH MAXPATHLEN
#else
#define _MAX_PATH PATH_MAX
#endif
#else
#include <io.h>
#endif
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 1        // we are a posix app
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <emobject.h>
#include <emstring.h>
#include <emstrtab.h>
#include <ndbm.h>

#ifdef S_IWUSR
#define DB_OPEN_MODE (S_IWUSR|S_IRUSR)
#else
#define DB_OPEN_MODE (S_IREAD|S_IWRITE)
#endif

int c_flag = 0;

int main(int argc,char **argv)
{
    char filename[_MAX_PATH+1];
    char *p;
    int file;
    if (argc < 2) {
        printf ("Usage: %s database [-c]\n", argv[0]);
        exit (1);
}
    if( argc == 3 && strcmp( argv[2], "-c" ) == 0 )
        c_flag = 1;

    database db;
    if( !c_flag
    && db.open_db( argv[1], 0 ) )
    {
        printf ("Data base found\n");
        exit (1);
    }

    strcpy( filename, argv[1] );
    p = filename + strlen( filename );
    strcpy( p, ".dir" );

    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, DB_OPEN_MODE );
    if( file == -1 )
    {
        printf ("Database create failed\n");
        perror(filename);
        exit(2);
    }

    // overcome problems with zero length files and poorly write Windows software
    // like InstallShield Express and older WinZip versions
    static unsigned char lots_of_zeros[database::DBLKSIZ];
    int size = write( file, lots_of_zeros, sizeof( lots_of_zeros ) );
    if( size != sizeof( lots_of_zeros ) )
    {
        printf ("Database write failed\n");
        perror(filename);
        exit(2);
    }

    close(file);

    strcpy( p, ".pag" );
    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, DB_OPEN_MODE );
    if( file == -1 )
    {
        printf ("Database create failed\n");
        perror(filename);
        exit(2);
    }
    size = write( file, lots_of_zeros, sizeof( lots_of_zeros ) );
    if( size != sizeof( lots_of_zeros ) )
    {
        printf ("Database write failed\n");
        perror(filename);
        exit(2);
    }

    close(file);

    strcpy( p, ".dat" );
    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, DB_OPEN_MODE );
    if( file == -1 )
    {
        printf ("Database create failed\n");
        perror(filename);
        exit(2);
    }
    close(file);

    return 0;
}
