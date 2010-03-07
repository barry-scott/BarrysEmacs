//
//    emclient.cpp
//
//    Copyright (c) 1997-2010 Barry A. Scott
//
#ifdef __GNUC__
#include <typeinfo>
#endif
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>

#include <emacsutl.h>
#include <emobject.h>
#include <emstring.h>
#include <em_stat.h>

#include <pwd.h>


bool opt_debug( false );
#define debug( msg ) do { if( opt_debug ) {std::cerr << msg << std::endl;} } while(0)
//#define debug( msg )

EmacsString realPathToBemacs( const char *argv0 );

std::ostream &operator<<( std::ostream &s, const EmacsString &string )
{
#ifdef OLD_STDCXX
    if( s.opfx() )
#else
    std::ostream::sentry se(s);
    if( se )
#endif
    {
        s.write( string.sdata(), string.length() );
    }

    return s;
}


void error( const char *msg )
{
    std::cerr << "bemacs: " << msg << std::endl;
    exit(1);
}

void error( const char *msg, const EmacsString &msg2 )
{
    int rc = errno;
    std::cerr << "bemacs: " << msg << std::endl;
    std::cerr << "- " << msg2 << ": " << strerror( rc ) << std::endl;
    exit(1);
}

char command[16384];
int command_size;
char response[16384];
char *name_arg = NULL;
bool wait_qual = false;

//
//
//    bemacs [opt] arg ...
//
//    opt
//    none open the files named in arg
//    +n open file at line n
//    -e arg is an MLisp command to execute
//
//

void parse_args( int argc, char ** argv );
bool process_command();
void make_dir( const EmacsString &dir );
void make_fifo( const EmacsString &fifo );

int new_server_argc = 1;
char *new_server_argv[32];

const int TIMEOUT_STARTUP( 30 );

int main( int argc, char ** argv )
{
    opt_debug = getenv("BEMACS_CLIENT_DEBUG") != NULL;
    debug( "Starting emclient" );

    parse_args( argc, argv );

    debug("main: calling process_command()");
    if( !process_command() )
    {
        debug("main: process_command() failed");
        char *display_env = getenv("DISPLAY");
        bool do_not_fork = display_env == NULL || display_env[0] == '\0';
        pid_t child = 0;
        if( !do_not_fork )
        {
            std::cerr << "Starting new bemacs server" << std::endl;

            child = fork();
        }


        if( child == 0 )
        {
            EmacsString app( realPathToBemacs( argv[0] ) ); app.append("_server");
            if( do_not_fork )
            {
                argv[0] = const_cast<char *>( app.sdata() );
                execvp( argv[0], argv );
            }
            else
            {
                new_server_argv[0] = const_cast<char *>( app.sdata() );
                execvp( new_server_argv[0], new_server_argv );
            }

            error("Cannot exec", app );
        }
        else
        {
            int i=TIMEOUT_STARTUP;
            while( i > 0 )
            {
                i--;
                sleep(1);

                debug("main: calling process_command() after fork()");
                if( process_command() )
                    break;
            }
            if( i == 0 )
                std::cerr << "Timed out waiting for the bemacs server to start up" << std::endl;
        }
    }

    return 0;
}

void parse_args( int argc, char **argv )
{
    if( getcwd( command, 256 ) == NULL )
        error("getcwd","");

    command_size = strlen( command );
    command[command_size++] = 0;

    for( int arg=1; arg<argc; arg++ )
    {
        if( strncmp( argv[arg], "-name=", 6 ) == 0 )
        {
            name_arg = &argv[arg][6];

            new_server_argv[new_server_argc] = argv[arg];
            new_server_argc++;
            continue;
        }
        else if( strcmp( argv[arg], "-wait" ) == 0 )
        {
            wait_qual = true;

            new_server_argv[new_server_argc] = argv[arg];
            new_server_argc++;
        }
        else if( strncmp( argv[arg], "-re", 3 ) == 0
        ||strncmp( argv[arg], "-nore", 5 ) == 0 )
        {
            new_server_argv[new_server_argc] = argv[arg];
            new_server_argc++;
            continue;
        }

        if( arg != 1 )
            command[command_size++] = '\x00';
        strcpy( &command[command_size], argv[arg] );
        command_size += strlen( argv[arg] );
    }
}

//
//    Write the command to emacs
//    then read the response
//
bool process_command()
{
    EmacsString server_fifo;

    {
    const char *fifo_name = getenv("BEMACS_FIFO");
    if( fifo_name == NULL )
        fifo_name = ".bemacs/.emacs_command";
    if( fifo_name[0] != '/' )
    {
        const char *name = NULL;
        const char *home = getenv( "HOME" );
        if( home != NULL )
        {
            for( const char *p = home; *p; p++ )
                if( *p == '/' )
                    name = p;
        }
        else
        {
            struct passwd *pwd = getpwuid( geteuid() );
            if( pwd == NULL )
                name = "default";
            else
                name = pwd->pw_name;
        }

        server_fifo = "/tmp/";
        server_fifo.append( name );
        server_fifo.append( "/" );
    }

    server_fifo.append( fifo_name );
    }


    EmacsString client_fifo( server_fifo );
    client_fifo.append( "_response" );

    if( name_arg != NULL )
    {
        server_fifo.append( "_" );
        server_fifo.append( name_arg );
        client_fifo.append( "_" );
        client_fifo.append( name_arg );
    }

    int last_part = server_fifo.last('/');
    EmacsString fifo_dir( server_fifo( 0, last_part ) );

    debug( "server_fifo is " << server_fifo );
    debug( "client_fifo is " << client_fifo );
    debug( "fifo_dir is " << fifo_dir );

    make_dir( fifo_dir );

    make_fifo( server_fifo );
    make_fifo( client_fifo );

    errno = 0;    // reset errno to zero to cleanup debug messages

    debug("open( " << server_fifo << " ) ...");
    int fd_command = open( server_fifo, O_WRONLY|O_NONBLOCK );
    debug("... open() ) => " << fd_command << " errno=" << strerror( errno ) );
    if( fd_command < 0 )
        return false;    // start up a new emacs

    debug("open( " << client_fifo << " ) ...");
    int fd_response = open( client_fifo, O_RDONLY|O_NDELAY );
    debug("... open() => " << fd_response << " errno=" << strerror( errno ) );
    if( fd_response < 0 )
        error("open - no bemacs server running", client_fifo);

    debug("write( " << fd_command << ", command, " << command_size << " ) ...");
    ssize_t size = write( fd_command, command, command_size );
    debug("... write() => " << size );
    if( size != command_size )
        error("write", server_fifo);

    debug("close( " << fd_command << " )...");
    int close_status = close( fd_command );
    debug("... close() => " << close_status << " errno=" << strerror( errno ) );

    //
    //    make sure that an ack is seen before the response.
    //    it seems that on HP-UX you can send a bunch of responses
    //    back on a fifo that does not have a listener.
    //
    bool seen_ack = false;
    for(;;)
    {
        debug("read( " << fd_response << ", command, " << sizeof( command )-1 << " ) ...");
        size = read( fd_response, command, sizeof( command )-1 );
        debug("... read() => " << size << " errno=" << strerror(errno));
        if( size > 0 )
        {
            debug( "read response: " << int(command[0]) << "(" << command[0] << ") seen_ack=" << seen_ack );
            if( command[0] == ' ' )
            {
                seen_ack = true;
                if( !wait_qual )
                    break;
            }
            else if( seen_ack )
            {
                if( command[0] == 'R' )
                    // found the response
                    break;
            }
        }
        if( size < 0 && errno != EAGAIN )
        {
            error("read", client_fifo);
            break;
        }
        sleep(1);
    }

    if( size > 1 )
    {
        command[size] = '\0';
        printf("%s\n",command+1);
    }

    return true;    // command sent to new emacs
}

//
// operates like the command mkdir -p
//
void make_dir( const EmacsString &final_dir )
{
    debug( "checking for dir " << final_dir );

    int index = final_dir.index( '/', 1 );

    for(;;)
    {
        EmacsString dir( final_dir( 0, index ) );

        debug( "index is " << index << " dir is " << dir );

        EmacsFileStat stats;

        if( stats.stat( dir ) )
        {
            if( !S_ISDIR( stats.data().st_mode ) )
                error( "Not a directory", dir );
        }
        else
        {
            int rc = mkdir( dir, S_IRUSR|S_IWUSR|S_IXUSR|S_IRGRP|S_IXGRP );
            if( rc != 0 )
                error( "Cannot mkdir", dir );
            std::cerr << "Made missing directory " << dir << std::endl;
        }
        if( index >= final_dir.length() )
            break;

        index = final_dir.index( '/', index+1 );
        if( index < 0 )
            index = final_dir.length();
    }
}

void make_fifo( const EmacsString &fifo )
{
    EmacsFileStat stats;

    off_t bytes_of_data = 0;
    if( stats.stat( fifo.sdata() ) )
    {
        if( !S_ISFIFO( stats.data().st_mode ) )
            error( "Not a FIFO", fifo );

        bytes_of_data = stats.data().st_size;
        if( bytes_of_data == 0 )
            return;
    }

    std::cerr << "Replacing fifo " << fifo << " contains " << (int)bytes_of_data << " bytes of data." << std::endl;
    remove( fifo.sdata() );

    int rc = mkfifo( fifo.sdata(), S_IRUSR|S_IWUSR );
    if( rc != 0 )
        error( "Cannot mkfifo", fifo );
    if( bytes_of_data == 0 )
        std::cerr << "Made fifo " << fifo << std::endl;
}

EmacsString realPathToBemacs( const char *argv0 )
{
    //
    //    first figure out if its a simple name which we search the path for
    //    or contains a / which we use directly
    //
    EmacsString prog( argv0 );

    if( prog.first( '/' ) == -1 )
    {
        // need to find in the path
        char *path_ = getenv( "PATH" );
        if( path_ == NULL )
            error( "PATH variable not set" );

        EmacsString path( path_ ); path.append(':');

        int start = 0;
        int end   = path.first(':');

        while( end != -1 )
        {
            EmacsString path_part( path( start, end ) );
            debug( "realPathToBemacs: path( " << start << ", " << end << " ) => " << path_part );

            start = end + 1;
            end = path.index( ':', start );

            path_part.append( '/' );
            path_part.append( prog );

            char out_path_buffer[MAXPATHLEN];

            debug( "realPathToBemacs: path_part " << path_part );
            if( realpath( path_part.sdata(), out_path_buffer ) != NULL )
            {
                debug( "realPathToBemacs: realpath 1 " << out_path_buffer );

                struct stat stats;
                if( stat( out_path_buffer, &stats ) == 0 && S_ISREG( stats.st_mode ) )
                    if( access( out_path_buffer, X_OK ) == 0 )
                        return EmacsString( out_path_buffer );
            }
        }
    }
    char out_path_buffer[MAXPATHLEN];

    if( realpath( prog.sdata(), out_path_buffer ) == NULL )
        error( "Failed to find realpath of", prog );

    debug( "realPathToBemacs: realpath 2 " << out_path_buffer );

    return EmacsString( out_path_buffer );
}
