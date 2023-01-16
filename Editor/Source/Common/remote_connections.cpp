//
//  remote_connections.cpp
//
//  Copyright 2023 Barry A. Scott
//
#include <emacs.h>
#include <em_stat.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if defined( SFTP )
extern int list_ssh_connections(void);
extern bool close_ssh_connection( const EmacsString &host );

int list_remote_connections()
{
    return list_ssh_connections();
}

int close_remote_connection()
{
    EmacsString host;

    if( cur_exec == NULL )
    {
        host = get_string_interactive( ": close-remote-connection (host) " );
    }
    else
    {
        host = get_string_mlisp();
        if( ml_err )
        {
            return 0;
        }
    }

    if( close_ssh_connection( host ) )
    {
        ml_value = FormatString("closed ssh connection to %s") << host;
        return 0;
    }
    else
    {
        error( FormatString("remote ssh connection to %s not found") << host );
        return 0;
    }
}

#else
int list_remote_connections()
{
    error("remote connections are not supported");
    return 0;
}

int close_remote_connection()
{
    error("remote connections are not supported");
    return 0;
}
#endif
