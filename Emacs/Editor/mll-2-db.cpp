/*
 *	mll-2-db.c
 *	Copyright (c) 1994 Barry A. Scott
 *	Date: 28-Mar-1994
 */
#include <emacsutl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef __unix__
# include <unistd.h>
#endif
#include <emobject.h>
#include <emstring.h>
#include <emstrtab.h>
#include <ndbm.h>

char *buf;
size_t buf_size;
int verbose(0);

int main(int argc,char **argv)
	{
	FILE *in;
	size_t size;

	if( argc > 1 && strcmp( argv[1], "-i" ) == 0 )
		{
		verbose = 1; argc--; argv++;
		}
	if( argc < 3 )
		{
		printf("Usage: mll-2-db [-i] <mll-file> <database>\n");
		exit(EXIT_FAILURE);
		}

	in = fopen( argv[1], "r" );
	if( in == NULL )
		{
		perror( argv[1] );
		exit(EXIT_FAILURE);
		}
	fseek( in, 0l, SEEK_END );
	size = (size_t)ftell( in );
	fseek( in, 0l, SEEK_SET );

	buf = new char[size];
	if( buf == NULL )
		{
		printf("Failed to allocate %d bytes\n", buf_size );
		exit(EXIT_FAILURE);
		}

	buf_size = fread( buf, sizeof( char ), (size_t)size, in );
	if( buf_size > size )
		{
		printf( "fread overflowed buf!\n");
		exit(EXIT_FAILURE);
		}


	database db;
	if( !db.open_db( argv[2], 0) )
		{
		printf ("Data base %s not found\n", argv[2]);
		exit (EXIT_FAILURE);
		}
	
	int status;
	int line_start=0;
	int section_start=0;
	int module_start=0;
	int module_length=0;
	int num_updates(0);
	size_t pos;

	for( pos=0; pos < buf_size; pos++ )
	    if( buf[pos] == '\n' )
		{
		if( buf[line_start] == '['
		&& buf[pos-1] == ']' )
		    {
		    if( section_start != 0 )
			{
			if( verbose )
				printf("Adding %.*s (%d bytes)\n", module_length, &buf[module_start], line_start-section_start );
			status = db.put_db
				(
				EmacsString( EmacsString::copy, u_str(&buf[module_start]), module_length ),
				u_str(&buf[section_start]), line_start-section_start
				);
			if( status < 0 )
				{
				printf ("Database update failed\n");
				exit(EXIT_FAILURE);
				}

			num_updates++;
			}

		    module_start = line_start + 1;
		    module_length = pos - 1 - module_start;
		    section_start = pos + 1;
		    }

		line_start=pos+1;
		}

	if( verbose )
		printf("Adding %.*s (%d bytes)\n", module_length, &buf[module_start], line_start-section_start );
	status = db.put_db
		(
		EmacsString( EmacsString::copy, u_str(&buf[module_start]), module_length ),
		u_str(&buf[section_start]), pos-section_start
		);
	if( status < 0 )
		{
		printf ("Database update failed\n");
		exit(EXIT_FAILURE);
		}

	num_updates++;

	printf( "Updated %s with %d modules.\n", argv[2], num_updates );

	return EXIT_SUCCESS;
	}
