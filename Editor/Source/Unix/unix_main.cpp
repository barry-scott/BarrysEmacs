#include "unix_rtl.cpp"


static void process_args( int argc, char **argv );

static bool term_is_file( false );

int main( int argc, char **argv )
{
    if( !isFileDescriptorOpen( STDIN_FILENO ) )
    {
        freopen( "/dev/null", "r", stdin );
    }

    if( !isFileDescriptorOpen( STDOUT_FILENO ) )
    {
        freopen( "/dev/null", "w", stdout );
    }

    if( !isFileDescriptorOpen( STDERR_FILENO ) )
    {
        freopen( "/dev/null", "w", stderr );
    }

    init_memory();

    init_unix_environ( argv[0] );

    // record the start time
    gettimeofday( &emacs_start_time, NULL );

    struct utsname uname_data;
    uname( &uname_data );

    process_args( argc, argv );

    if( term_is_file )
    {
        return emacsMain( EmacsString::null, u_str("file"), u_str("") );
    }
    else
    {
        // EMACS_TERM_DEVICE is mostly useful when debugging
        // so that Emacs and the debugged have separate terminals
        EmacsString term_device = get_config_env( "EMACS_TERM_DEVICE" );
        return emacsMain( EmacsString::null, u_str("char"), term_device );
    }
}

static void process_args( int argc, char **argv )
{
    command_line_arguments.setArguments( argc, argv );
    command_line_arguments.setArgument( 0, "emacs", false );

    int arg = 1;

    while( arg<command_line_arguments.argumentCount() )
    {
        const EmacsArgument &argument( command_line_arguments.argument( arg ) );

        if( argument.isQualifier() )
        {
            EmacsString str( argument.value() );
            EmacsString key_string;
            EmacsString val_string;

            int equal_pos = str.first( '=' );
            if( equal_pos > 0 )
            {
                key_string = str( 1, equal_pos );
                val_string = str( equal_pos+1, str.length() );
            }
            else
            {
                key_string = str( 1, str.length() );
            }

            bool delete_this_arg = true;

            if( key_string.commonPrefix( "package" ) > 2 )
            {
                package_arg = val_string;
                command_line_arguments.setArgument( 0, package_arg, false );
            }
            else if( key_string == "noterm" )
            {
                term_is_file = true;
            }
            else
            {
                delete_this_arg = false;
            }

            if( delete_this_arg )
            {
                command_line_arguments.deleteArgument( arg );
            }
            else
            {
                arg++;
            }
        }
        else
        {
            arg++;
        }
    }
}

int ui_frame_to_foreground(void)
{
    return 0;
}
