#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#if defined unix || defined __unix
#include <unistd.h>
#include <limits.h>
#if defined( _POSIX_PATH_MAX )
#define _MAX_PATH _POSIX_PATH_MAX
#elif defined( MAXPATHLEN )
#define _MAX_PATH MAXPATHLEN
#else
#define _MAX_PATH 255
#endif
#else
#include <io.h>
#endif
#define POSIX 1
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ndbm.h>

#define u_str(s) ((unsigned char *)(s))
#define BUF_SIZE 500000

int c_flag = 0;

int main(int argc,char **argv)
{
    database * db;
    char filename[_MAX_PATH+1];
    char *p;
    int file;
    if (argc < 2) {
	printf ("Usage: %s database [-c]\n", argv[0]);
	exit (1);
    }
    if( argc == 3 && strcmp( argv[2], "-c" ) == 0 )
	c_flag = 1;

    if( !c_flag && ((db = open_db (u_str(argv[1]), 0)) != NULL) )
	{
	printf ("Data base found\n");
	exit (1);
	}
    
    strcpy( filename, argv[1] );
    p = filename + strlen( filename );
    strcpy( p, ".dir" );
    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, S_IWRITE|S_IREAD ); 
    if( file == -1 )
	{
	printf ("Database create failed\n");
	perror(filename);
	exit(2);
	}
    close(file);

    strcpy( p, ".pag" );
    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, S_IWRITE|S_IREAD ); 
    if( file == -1 )
	{
	printf ("Database create failed\n");
	perror(filename);
	exit(2);
	}
    close(file);

    strcpy( p, ".dat" );
    file = open( filename, O_CREAT|O_TRUNC|O_RDWR, S_IWRITE|S_IREAD ); 
    if( file == -1 )
	{
	printf ("Database create failed\n");
	perror(filename);
	exit(2);
	}
    close(file);

    return 0;
}
