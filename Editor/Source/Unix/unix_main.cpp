#include "unix_rtl.cpp"


static void process_args( int argc, char **argv );

int main( int argc, char **argv )
{
    if( !isFileDescriptorOpen( STDIN_FILENO ) )
    {
        FILE *file = freopen( "/dev/null", "r", stdin );
        assert( fileno(file) == STDIN_FILENO );
    }

    if( !isFileDescriptorOpen( STDOUT_FILENO ) )
    {
        FILE *file = freopen( "/dev/null", "w", stdout );
        assert( fileno(file) == STDOUT_FILENO );
    }

    if( !isFileDescriptorOpen( STDERR_FILENO ) )
    {
        FILE *file = freopen( "/dev/null", "w", stderr );
        assert( fileno(file) == STDERR_FILENO );
    }

    init_memory();

    init_unix_environ( argv[0] );

    // record the start time
    gettimeofday( &emacs_start_time, NULL );

    struct utsname uname_data;
    uname( &uname_data );

    EmacsString default_binding;
    default_binding.append( EmacsChar_t( 0x0010ff00 ) );    // '\U0010ff00'
    PC_key_names.addMapping( "default", default_binding );

    process_args( argc, argv );

    return emacsMain( EmacsString::null, u_str("char"), u_str("") );
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
