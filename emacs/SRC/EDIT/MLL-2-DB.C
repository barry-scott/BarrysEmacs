/*
 *	mll-2-db.c
 *	Copyright (c) 1994 Barry A. Scott
 *	Date: 28-Mar-1994
 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef unix
# include <unistd.h>
#endif
#include <ndbm.h>

#define u_str(s) ((unsigned char *)(s))

char *buf;
size_t buf_size;

int main(int argc,char **argv)
	{
	FILE *in;
	database * db;
	size_t size;

	if( argc < 3 )
		{
		printf("Usage: mll-2-db <mll-file> <database>\n");
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

	buf = (char *)malloc( size );
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


	if ((db = open_db (u_str(argv[2]), 0)) == 0)
		{
		printf ("Data base %s not found\n", argv[2]);
		exit (EXIT_FAILURE);
		}
	
	{
	int status;
	int line_start=0;
	int section_start=0;
	int module_start=0;
	int module_length=0;
	size_t pos;

	for( pos=0; pos < buf_size; pos++ )
	    if( buf[pos] == '\n' )
		{
		if( buf[line_start] == '['
		&& buf[pos-1] == ']' )
		    {
		    if( section_start != 0 )
			{
			printf("Adding %.*s (%d bytes)\n", module_length, &buf[module_start], line_start-section_start );
			status = put_db
				(
				u_str(&buf[module_start]), module_length,
				u_str(&buf[section_start]), line_start-section_start,
				db
				);
			if( status < 0 )
				{
				printf ("Database update failed\n");
				exit(EXIT_FAILURE);
				}
			}

		    module_start = line_start + 1;
		    module_length = pos - 1 - module_start;
		    section_start = pos + 1;
		    }

		line_start=pos+1;
		}

	printf("Adding %.*s (%d bytes)\n", module_length, &buf[module_start], line_start-section_start );
	status = put_db
		(
		u_str(&buf[module_start]), module_length,
		u_str(&buf[section_start]), pos-section_start,
		db
		);
	if( status < 0 )
		{
		printf ("Database update failed\n");
		exit(EXIT_FAILURE);
		}
	}

	return EXIT_SUCCESS;
	}
