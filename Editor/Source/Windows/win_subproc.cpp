//
//     Copyright(c ) 1982-2009
//        Barry A. Scott

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );
# if defined( _WIN32 )
#  include <win_incl.h>
#  include <process.h>

extern EmacsString emacs_tmpnam();

void init_subprocesses()
{
}

int return_to_monitor( void )
{
    return no_value_command();
}

int pause_emacs( void )
{
    return 0;
}

void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    const char *command,
    ...
    )
{
    EmacsBufferRef old( bf_cur );
    va_list argp;

    EmacsFile output( emacs_tmpnam() );
    if( !output.parse_is_valid() )
    {
        error( "Unable to create temporary file" );
        return;
    }

    va_start( argp, command );

    int disp_flag = display;

    EmacsBuffer::set_bfn( bufname );
    if( interactive() )
    {
        theActiveView->window_on( bf_cur );
    }
    if( erase )
    {
        bf_cur->erase_bf();
    }

    if( disp_flag )
    {
        message( "Starting up ..." );
        theActiveView->do_dsp();
    }

    // use the prefered command interpreter
    EmacsString buf( getenv( "COMSPEC" ) );
    buf.append( " /c" );
    for(;;)
    {
        unsigned char *arg_str = va_arg( argp, unsigned char * );

        if( arg_str == NULL )
            break;

        buf.append( " " );
        buf.append( arg_str );
    }

    buf.append( " <" ); buf.append( input );
    buf.append( " >" ); buf.append( output.fio_getname() );
    buf.append( " 2>&1" );

    PROCESS_INFORMATION ProcessInformation;
    STARTUPINFO si;

    si.cb = sizeof(STARTUPINFO);
    si.lpReserved = NULL;
    si.lpTitle = NULL;
    si.lpDesktop = NULL;
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = si.dwFlags = 0L;
    si.wShowWindow = SW_HIDE;
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.lpReserved2 = NULL;
    si.cbReserved2 = 0;

    if( CreateProcess
        (
        NULL,
        const_cast<wchar_t *>( buf.utf16_data() ),
        NULL,
        NULL,
        FALSE, // Inherit handles
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &ProcessInformation
        ))
    {
        WaitForSingleObject( ProcessInformation.hThread, INFINITE );
        if( CloseHandle( ProcessInformation.hThread ) == FALSE )
            error( "Failed to close process handle in exec_bf" );
    }
    else
    {
        error( FormatString("Failed to execute shell, error = %d\n") << GetLastError() );
    }

    // insert the output file
    bf_cur->read_file( output, 0, 0 );
    output.fio_delete();

    bf_cur->b_modified = 0;

    if( disp_flag )
    {
        message( "Done." );
    }
}
# endif
