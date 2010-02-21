int main( int argc, char **argv )
{
    char *debug_string = getenv( "EMACS_DEBUG" );
    if( debug_string != NULL )
        dbg_flags = parse_dbg_flags( debug_string );

    {
    bool open_fds[256];
    if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP )
        for( int fd=0; fd<256; fd ++ )
            open_fds[fd] = isFileDescriptorOpen( fd );

#ifdef __hpux
    openlog( "bemacs", LOG_PID|LOG_NDELAY, LOG_LOCAL2 );
#else
    openlog( "bemacs", LOG_PID|LOG_NDELAY, LOG_USER );
#endif

    if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP )
        for( int fd=0; fd<256; fd ++ )
            if( open_fds[fd] )
                _dbg_msg( FormatString("File descriptor %d is open at startup") << fd );
    }

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

    unix_path = getenv( "PATH" );
    for( int i=0; i<unix_path.length(); i++ )
        if( unix_path[i] == ':' )
            unix_path[i] = PATH_SEP;

    {
    EmacsFile image;
    image.fio_open_using_path( unix_path, argv[0], 0, "" );
    if( !image.fio_is_open() )
    {
        _dbg_msg( "Emacs is unable to find itself!\n" );
        return 100;
    }
    image_path = image.fio_getname();
    }

    // find the stat of the path and don't hide links
    struct stat stat_buf;
    if( lstat( (const char *)image_path, &stat_buf ) == 0
    // its a symbolic link
    && (stat_buf.st_mode&S_IFMT) == 0120000 )
    {
        unsigned char link_path[MAXPATHLEN+1];
        // get the value of the link
        int size = readlink( (const char *)image_path, (char *)link_path, MAXPATHLEN );
        if( size != -1 )
        {
            link_path[size] = '\0';

            // printf( "The sym link is %s\n", link_path );
            // if its an absolute path just replace image_path
            if( link_path[0] == '/' )
                image_path = link_path;
            else
            {
                // add the link to the end of the image_path less the last file name
                int pos = image_path.last( PATH_CH );
                if( pos > 0 )
                {
                    image_path.remove( pos+1 );
                    image_path.append( link_path );
                }
            }
        }

        // printf( "New image path is %s\n", image_path.data() );
    }

    int pos = image_path.last( PATH_CH );
    if( pos < 0 )
        return 101;
    image_path.remove( pos );

    // record the start time
    gettimeofday( &emacs_start_time, NULL );

    struct utsname uname_data;
    int status = uname( &uname_data );

    default_environment_file = "emacs_user:emacs7_" OS_VERSION "_";
    if( status != -1 )
    {
        default_environment_file.append( uname_data.release );
        default_environment_file.append( "_" );
    }
    default_environment_file.append( CPU_TYPE ".emacs_environment" );
    restore_arg = default_environment_file;

    process_args( argc, argv );

#ifdef XWINDOWS
    EmacsString display;
    if( !nodisplay_arg )
        display = get_config_env( "DISPLAY" );

    int exit_code;
    if( !display.isNull() )
        exit_code = emacsMain( restore_arg, u_str("gui"), display );
    else
#endif
        exit_code = emacsMain( restore_arg, u_str("char"), u_str("") );

    closelog();

    return exit_code;
}

static void process_args( int argc, char **argv )
{
    command_line_arguments.setArguments( argc, argv );
    command_line_arguments.setArgument( 0, "emacs", false );

    motif_argv[motif_argc++] = "emacs";

    int arg = 1;

    while( arg<command_line_arguments.argumentCount() )
    {
        EmacsArgument argument( command_line_arguments.argument( arg ) );

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

            if( key_string.commonPrefix( "restore" ) > 2 )
                restore_arg = val_string;
            else if( key_string.commonPrefix( "norestore" ) > 4 )
                restore_arg = EmacsString::null;
            else if( key_string.commonPrefix( "package" ) > 2 )
            {
                package_arg = val_string;
                command_line_arguments.setArgument( 0, package_arg, false );
            }
            else if( key_string.commonPrefix( "name" ) > 3 )
            {
                name_arg = val_string;
            }
            else if( key_string.commonPrefix("nodisplay") > 3 )
            {
                nodisplay_arg = 1;
            }
            else if( str == "display" )
            {
                motif_argv[motif_argc++] = "-display";
                command_line_arguments.deleteArgument( arg );
                if( arg < command_line_arguments.argumentCount() )
                {
                    motif_argv[motif_argc++] = strdup( command_line_arguments.argument(arg).value().sdata() );
                }
                else
                    delete_this_arg = false;
            }
            else if( str == "geometry" )
            {
                motif_argv[motif_argc++] = "-geometry";
                command_line_arguments.deleteArgument( arg );
                if( arg < command_line_arguments.argumentCount() )
                {
                    motif_argv[motif_argc++] = strdup( command_line_arguments.argument(arg).value().sdata() );
                }
                else
                    delete_this_arg = false;
            }
            else
                delete_this_arg = false;

            if( delete_this_arg )
                command_line_arguments.deleteArgument( arg );
            else
                arg++;
        }
        else
            arg++;
    }
}
