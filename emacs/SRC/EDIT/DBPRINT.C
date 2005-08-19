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

int main(int argc,char **argv)
    {
    register    database * db;
    char   *content;
    int     contentlen;

    if (argc != 3) {
	printf ("Usage: %s database key\n", argv[0]);
	exit (1);
    }
    if ((db = open_db (u_str(argv[1]), 1)) == 0) {
	printf ("Data base not found\n");
	exit (1);
    }
    if (get_db (u_str(argv[2]), strlen (argv[2]), (unsigned char **)&content, &contentlen, 0, db) < 0)
	printf ("Not found\n");
    else
	{
	write (1, content, contentlen);
	free (content);
	}
    return 0;
    }
