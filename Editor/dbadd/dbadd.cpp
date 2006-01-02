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

//static const char *THIS_FILE = __FILE__;


const int BUF_SIZE( 500000 );

int main(int argc,char **argv)
{
    char *buf;
    int len = 0, n;
    if (argc != 3) {
    printf ("Usage: %s database key\n", argv[0]);
    exit (1);
    }
    database db;
    if( !db.open_db( argv[1], 0 ) )
{
    printf ("Data base not found %s\n", argv[1]);
    exit (1);
}
    buf = (char *)EMACS_MALLOC( BUF_SIZE, malloc_type_char );
    while ((n = read (0, buf + len, BUF_SIZE - len)) > 0)
    len += n;
    if (db.put_db (argv[2], u_str(buf), len ) < 0)
{
    printf ("Database update failed %s %s\n", argv[1], argv[2]);
    exit(2);
}
    return 0;
}
