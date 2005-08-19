#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

void error( const char *msg, const char *msg2 )
	{
	fprintf( stderr, "emclient: %s\n", msg );
	perror( msg2 );
	exit(1);
	}

char server_name[128];
char command[16384];
int command_size;

/*

	emclient [opt] arg ...

	opt
	none open the files named in arg
	+n open file at line n
	-e arg is an MLisp command to execute

*/

void parse_args( int argc, char ** argv );
void process_command();

int main( int argc, char ** argv )
	{
	parse_args( argc, argv );
	process_command();

	return 0;
	}

void parse_args( int argc, char **argv )
	{
	int arg;

	if( getcwd( command, 256 ) == NULL )
		error("getcwd","");

	command_size = strlen( command );

	for( arg=1; arg<argc; arg++ )
		{
		command[command_size++] = 1;
		strcpy( &command[command_size], argv[arg] );
		command_size += strlen( argv[arg] );
		}
	}

/*
//	Write the command to emacs
//	then read the response
*/
void process_command()
	{
	char server_fifo[256];
	char client_fifo[256];
	char *home_env = getenv("HOME");
	char *fifo_name = getenv("BEMACS_FIFO");
	struct stat stats;
	int size;
	int fd_command;
	int fd_response;

	if( fifo_name == NULL )
		fifo_name = ".emacs_command";

	server_fifo[0] = '\0';
	if( fifo_name[0] != '/' )
		{
		strcpy( server_fifo, home_env );
		strcat( server_fifo, "/" );
		}
	strcat( server_fifo, fifo_name );

	strcpy( client_fifo, server_fifo );	
	strcat( client_fifo, "_response" );

	if( stat( server_fifo, &stats ) == -1 )
		error( "cannot stat server fifo", server_fifo );
	if( !S_ISFIFO( stats.st_mode ) )
		error( "server not a FIFO", server_fifo );

	if( stat( client_fifo, &stats ) == -1 )
		error( "cannot stat client fifo", client_fifo );
	if( !S_ISFIFO( stats.st_mode ) )
		error( "client not a FIFO", client_fifo );

	fd_command = open( server_fifo, O_WRONLY|O_NONBLOCK );
	if( fd_command < 0 )
		error("open - no emacs server running", server_fifo);

	size = write( fd_command, command, command_size );
	if( size != command_size )
		error("write", server_fifo);

	close( fd_command );

	fd_response = open( client_fifo, O_RDONLY );
	if( fd_response < 0 )
		error("open - no emacs server running", client_fifo);

	size = read( fd_response, command, sizeof( command )-1 );
	if( size < 0 )
		error("read", client_fifo);
	command[size] = '\0';
	printf("%s\n",command);
	}
