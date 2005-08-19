#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef unix
#include <unistd.h>
#else
#include <io.h>
#endif
#include <ndbm.h>

#define u_str(s) ((unsigned char *)(s))

#define BUF_SIZE 500000

int main(int argc,char **argv)
{
    database * db;
    char *buf;
    int len = 0, n;
    if (argc != 3) {
	printf ("Usage: %s database key\n", argv[0]);
	exit (1);
    }
    if ((db = open_db (u_str(argv[1]), 0)) == 0) {
	printf ("Data base not found\n");
	exit (1);
    }
    buf = (char *)malloc( BUF_SIZE );
    while ((n = read (0, buf + len, BUF_SIZE - len)) > 0)
	len += n;
    if (put_db (u_str(argv[2]), strlen (argv[2]), u_str(buf), len, db) < 0)
	{
	printf ("Database update failed\n");
	exit(2);
	}
    return 0;
}
