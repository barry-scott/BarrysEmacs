#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main( int argc, char **argv )
	{
	FILE *f;
	time_t t;

	if( argc < 2 )
		return 1;

	f = fopen( argv[1], "w" );
	fprintf( f, "#include <emacs.h>\n" );
	fprintf( f, "time_t build_time = %lu;\n", time( &t ) );
	fprintf( f, "unsigned char version_string[80] =\n" );
	fprintf( f, " EMACS_VERSION \" of Emacs Copyright BArry A. Scott (c) \" __DATE__ \" \" __TIME__;\n");
	fclose( f );
	return 0;
	}
