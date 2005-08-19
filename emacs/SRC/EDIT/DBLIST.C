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
    datum key;
    register    database * db;
    char *ndb;
    register char  *p;
    int     listlens = 0,
            makeproc = 0;

    if (argc<2) {
	printf ("Usage: %s database [ -l ] [ -p newdatabase ]\n", argv[0]);
	exit (1);
    }
    if ((db = open_db (u_str(argv[1]), 1)) == 0) {
	printf ("Data base not found\n");
	exit (1);
    }
    if (argc > 2) {
	p = argv[2];
	while (*p)
	    switch (*p++) {
		default: 
		    printf ("Bogus switch: -%c\n", *--p);
		    exit (1);
		case 'l': 
		    listlens++;
		    break;
		case 'p': 
		    makeproc++;
		    ndb = argc>3 ? argv[3] : argv[1];
		    break;
		case '-': 
		    break;
	    }
    }

    for (key = firstkey (db); key.dptr != 0; key = nextkey (key, db))
	if (makeproc) {
	    unsigned char buf[200];
	    unsigned char *ret;
	    int retlen;
	    strncpy ((char *)buf, (char *)key.dptr, key.dsize);
	    get_db (buf, key.dsize, &ret, &retlen, 0, db);
	    printf ("dbadd %s \"%.*s\" <<\"</FoO ThE bAr/>\"\n%.*s",
		    ndb, key.dsize, key.dptr, retlen, ret);
	    if (ret[retlen-1] != '\n') printf("\n");
	    printf ("</FoO ThE bAr/>\n");
	    free (ret);
	}
	else
	    printf (listlens ? "%-12.*s %d,%d\n" : "%.*s\n",
		    key.dsize, key.dptr, key.val1, key.val2);
    return 0;
    }
