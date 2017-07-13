//
//     Copyright(c ) 1982-2009
//        Barry A. Scott

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );
# if defined( _WINDOWS )
#  include <win_incl.h>
//# include <emacs_process.h>
extern "C" { int _getpid(void); };

unsigned int getpid()
{
    return _getpid();
}

# endif
# if defined( _NT )
static void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    ...
    );
# endif

# if defined( _WINDOWS )
int return_to_monitor( void )
{
    return no_value_command();
}
# endif
# if defined( _NT)
static void exec_bf
    (
    const EmacsString &bufname,
    int display,
    const EmacsString &input,
    int erase,
    ...
    )
{
    va_list argp;

    EmacsString output = emacs_tmpnam();
    if( output.isNull() )
    {
        error( "Unable to create temporary file" );
        return;
    }

    va_start( argp, erase );

    if( saved_buffer_count >= SAVED_BUFFER_MAX - 2 )
    {
        error( "not enough space to remember buffers" );
        return;
    }

    int disp_flag = display;

    saved_buffers[saved_buffer_count] = bf_cur;
    saved_buffer_count++;

    EmacsBuffer::set_bfn( bufname );

    if( interactive() )
        theActiveView->window_on( bf_cur );

    if( erase )
        bf_cur->erase_bf();

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
    buf.append( " >" ); buf.append( output );
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
        (char *)buf.sdata(),
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
        error( FormatString("Failed to execute shell, error = %d\n") << GetLastError() );


    // insert the output file
    bf_cur->read_file( output, 0, 0 );

    EmacsFile::fio_delete( output );

    bf_cur->b_modified = 0;

    saved_buffer_count--;
    if( interactive() )
        theActiveView->window_on( saved_buffers[saved_buffer_count] );

    if( disp_flag )
        message( "Done." );
    return;
}
# endif
