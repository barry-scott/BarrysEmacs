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

int main(int argc, char **argv)
    {
    datum key;
    database * db;
    if (argc != 3) {
	printf ("Usage: %s database key\n", argv[0]);
	exit (1);
    }
    if ((db = open_db (u_str(argv[1]), 0)) == 0) {
	printf ("Data base not found\n");
	exit (1);
    }
    for (key = firstkey (db); key.dptr != 0; key = nextkey (key, db))
	if (strncmp ((char *)key.dptr, argv[2], key.dsize) == 0)
	    if (delete_key (key, db) < 0)
		{
		printf ("Database update failed\n");
		exit(2);
		}
    return 0;
    }
