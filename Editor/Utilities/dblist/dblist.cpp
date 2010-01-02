#include <emacsutl.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
#include <unistd.h>
#else
#include <io.h>
#endif
#include <emobject.h>
#include <emstring.h>
#include <emstrtab.h>
#include <ndbm.h>

int main(int argc,char **argv)
{
    database db;
    database::datum key( db );
    char *ndb = NULL;
    register char  *p;
    int listlens = 0;
    int makeproc = 0;

    if (argc<2)
    {
        printf ("Usage: %s database [ -l ] [ -p newdatabase ]\n", argv[0]);
        exit (1);
    }
    if ( !db.open_db( argv[1], 1 ) )
    {
        printf ("Data base not found\n");
        exit (1);
    }
    if (argc > 2)
    {
        p = argv[2];
        while (*p)
            switch (*p++)
            {
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

    for( key = db.firstkey(); key.dptr != 0; key = db.nextkey( key) )
        if (makeproc)
        {
            EmacsString list_key( EmacsString::copy, key.dptr, key.dsize );
            EmacsString result;
            db.get_db ( list_key, result );
            printf ("dbadd %s \"%.*s\" <<\"</FoO ThE bAr/>\"\n%s",
                    ndb, key.dsize, key.dptr, result.data());
            if (result[-1] != '\n')
                printf("\n");
            printf ("</FoO ThE bAr/>\n");
        }
        else
            printf (listlens ? "%-12.*s %ld,%ld\n" : "%.*s\n",
                key.dsize, key.dptr, key.val1, key.val2);
    return 0;
}
