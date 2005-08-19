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

int main(int argc, char **argv)
	{
	database db;
	database::datum key( db );
	if (argc != 3)
		{
		printf ("Usage: %s database key\n", argv[0]);
		exit (1);
		}
	if( !db.open_db( argv[1], 0 ) )
		{
		printf ("Data base not found\n");
		exit (1);
		}
	for( key = db.firstkey(); key.dptr != 0; key = db.nextkey( key ) )
		if (strncmp ((char *)key.dptr, argv[2], key.dsize) == 0)
			if( db.delete_key( key ) < 0 )
				{
				printf ("Database update failed\n");
				exit(2);
				}
	return 0;
	}
