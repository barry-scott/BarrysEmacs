/****************************************************************************

    PROGRAM: win_load.c

    PURPOSE: Input template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance hndle and creates main window
	MainWndProc() - processes messages

****************************************************************************/

#include <win_incl.h>
#include <emacs.h>
#include <emacswin.h>

/*
 *	Forward routine declarations
 */
BOOL InitApplication( HANDLE hInstance );
int ShutdownApplication( void );
BOOL InitInstance( HANDLE hInstance, int nCmdShow );
LRESULT CALLBACK EXPORT MainWndProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam );

/*
 *	Application stuff
 */
char szAppName[6] = "Emacs";
char szClassName[32] = "Emacs";

MSG		emacs_win_msg;
HANDLE		emacs_hinst;
HFONT		emacs_hfont;
int		n_init_show_state;
HWND		emacs_hwnd;
extern int	emacs_quit;
int		is_windows_nt;

/****************************************************************************

	FUNCTION: WinMain(HANDLE, HANDLE, LPSTR, int)

	PURPOSE: calls initialization function, processes message loop

****************************************************************************/

#if !defined( _NT )
int GetLastError(void) { return 0; }
#endif

void _dbg_msg( unsigned char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = do_print( fmt, argp, (unsigned char *)buf, sizeof( buf ) );
	buf[i] = 0;

	OutputDebugString( buf );

	MessageBox( NULL, buf, "Debug", MB_ICONINFORMATION );
	}

#ifdef __WATCOM__
/*debug*/
static char LifeClass[32]="LifeClass";

long FAR PASCAL WindowProc( HWND win_handle, unsigned msg,
				     WORD wparam, LONG lparam )
/**************************************************************

    Handle messages for the main application window.
*/
{
	return DefWindowProc( win_handle, msg, wparam, lparam );
}

static BOOL FirstInstance( HANDLE this_inst )
/********************************************

    Register window class for the application,
    and do any other application initialization.
*/
{
    WNDCLASS	wc;
    BOOL	rc;

    sprintf( LifeClass,"LifeClass%d", this_inst );

    wc.style = CS_HREDRAW+CS_VREDRAW;
    wc.lpfnWndProc = (LPVOID) WindowProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = this_inst;
    wc.hIcon = 0;
    wc.hCursor = LoadCursor( NULL, IDC_ARROW );
    wc.hbrBackground = GetStockObject( WHITE_BRUSH );
    wc.lpszMenuName = "LifeMenu";
    wc.lpszClassName = LifeClass;
    rc = RegisterClass( &wc );
    return( rc );

}
static BOOL AnyInstance( HANDLE this_inst, int cmdshow )
/*******************************************************

    Do work required for every instance of the application:
    create the window, initialize data.
*/
{
    int	screen_x, screen_y;
    HWND WinHandle;

    screen_x = GetSystemMetrics( SM_CXSCREEN );
    screen_y = GetSystemMetrics( SM_CYSCREEN );

    WinHandle = CreateWindow(
	LifeClass,		/* class */
	"Life", 		/* caption */
	WS_OVERLAPPEDWINDOW,	/* style */
	screen_x / 8,		/* init. x pos */
	screen_y / 8,		/* init. y pos */
	3 * screen_x / 4,	/* init. x size */
	3 * screen_y / 4,	/* init. y size */
	NULL,			/* parent window */
	NULL,			/* menu handle */
	this_inst,		/* program handle */
	NULL			/* create parms */
	);
		    
    if( !WinHandle ) return( FALSE );
return 1;
}
/*debug*/
#endif

#ifdef __WATCOM__
int __argc = 1;
unsigned char *__argv[] = { u_str("emacs"), NULL };
#else
extern int __argc;
extern unsigned char **__argv;
#endif

SINT APIENTRY WinMain
	(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	SINT nCmdShow
	)
	{
#ifdef __WATCOM__
	FirstInstance( hInstance );
	AnyInstance( hInstance, nCmdShow );
#endif

#if !defined( _NT )
	if( hPrevInstance )
		return 0;
#endif

#ifdef __WINDOWS_386__
	sprintf(szClassName,"%s:%d",szAppName,hInstance);
#else
	if( !hPrevInstance )
#endif
		if( !InitApplication( hInstance ) )
			return FALSE;

	if( !InitInstance( hInstance, nCmdShow ) )
		return FALSE;

	emacs( __argc, __argv, NULL, u_str("gui"), u_str("") );

#if	DBG_PROFILE
	if( DBG_PROFILE&dbg_flags )
		{
		ProfStop();
		ProfFinish();
		}
#endif

	return ShutdownApplication();
	}

int ShutdownApplication( void )
	{
	WINDOWPLACEMENT where;

	/*
	 *	Save the last position of the window
	 */
	where.length = sizeof( where );
	if( GetWindowPlacement( emacs_hwnd, &where ) )
		{
		char buf[128];

		wsprintf
		( buf, "%d %d %d %d %d %d %d %d %d",
		where.showCmd,
		where.ptMinPosition.x, where.ptMinPosition.y,
		where.ptMaxPosition.x, where.ptMaxPosition.y,
		where.rcNormalPosition.left, where.rcNormalPosition.top, 
		where.rcNormalPosition.right, where.rcNormalPosition.bottom
		);
		WritePrivateProfileString( "Emacs", "placement", buf, "emacs060.ini" );
		}
	else
		{
		_dbg_msg( u_str("GetWindowPlacement failed  %d"), GetLastError() );
		WritePrivateProfileString( "Emacs", "placement", NULL, "emacs060.ini" );
		}

	/* kill off any timer resource used by journaling */
	stop_journal_timer();

	DestroyWindow( emacs_hwnd );

	DeleteFont( emacs_hfont );

	return 0;
	}

/****************************************************************************

	FUNCTION: InitApplication(HANDLE)

	PURPOSE: Initializes window data and registers window class

****************************************************************************/
BOOL InitApplication( HANDLE hInstance )
	{
	WNDCLASS  wc, wc2;
	BOOL class;
	DWORD version;

	emacs_hinst = hInstance;

	memset( &wc, 0, sizeof( WNDCLASS ) );

	wc.style = CS_DBLCLKS;		    /* double-click messages */
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = 0; /*LoadIcon(hInstance, "EMACS");*/
	wc.hCursor = LoadCursor(NULL, IDC_IBEAM);
	wc.hbrBackground = GetStockBrush(WHITE_BRUSH);
	wc.lpszMenuName = "EmacsMenu";
	wc.lpszClassName = szClassName;

	class = RegisterClass(&wc);
	if( class == 0 )
		{
		_dbg_msg( u_str("RegisterClass failed %d"), GetLastError() );
		return 0;
		}

	if( !GetClassInfo( emacs_hinst, szClassName, &wc2 ) )
		_dbg_msg( u_str("FAiled to retrive the emacs class info") );

	version = GetVersion();
#if defined(_NT) || defined( __WINDOWS_386__ )
	is_windows_nt = (version&0x80000000) == 0;
#define	WINDOWS_NUMBER "Win32"
#else
	is_windows_nt = 0;
#define	WINDOWS_NUMBER "Win16"
#endif
	sprintf( s_str(operating_system_name), "Windows" );
	sprintf( s_str(operating_system_version), "V%d.%d %s",
		 (int)(version&0xff), (int)((version>>8)&0xff), is_windows_nt ? "NT" : WINDOWS_NUMBER );	

	return class;
	}

/****************************************************************************

	FUNCTION:  InitInstance(HANDLE, int)

	PURPOSE:  Saves instance handle

****************************************************************************/

BOOL InitInstance( HANDLE hInstance, int nCmdShow )
	{
	n_init_show_state = nCmdShow;

	return TRUE;
	}

/****************************************************************************

	FUNCTION: MainWndProc(HWND, unsigned, WORD, LONG)

	PURPOSE:  Processes messages

****************************************************************************/

LRESULT CALLBACK EXPORT MainWndProc
	(
	HWND h_wnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	if( emacs_hwnd != 0
	&& emacs_hwnd != h_wnd )
		{
		invoke_debug();
		return FALSE;
		}
	switch (msg)
	{
	case WM_COMMAND_LINE:
		message( u_str("CMD: %s"), (char *)lParam );
		break;

	HANDLE_MSG( h_wnd, WM_INITMENUPOPUP, emacs_cls_WM_INITMENUPOPUP );
	HANDLE_MSG( h_wnd, WM_COMMAND, emacs_cls_WM_COMMAND );
	HANDLE_MSG( h_wnd, WM_SIZE, emacs_cls_WM_SIZE );
	HANDLE_MSG( h_wnd, WM_SETFOCUS, emacs_cls_WM_SETFOCUS );
	HANDLE_MSG( h_wnd, WM_KILLFOCUS, emacs_cls_WM_KILLFOCUS );
	HANDLE_MSG( h_wnd, WM_LBUTTONDOWN, emacs_cls_WM_LBUTTONDOWN );
	HANDLE_MSG( h_wnd, WM_LBUTTONUP, emacs_cls_WM_LBUTTONUP );
	HANDLE_MSG( h_wnd, WM_LBUTTONDBLCLK, emacs_cls_WM_LBUTTONDBLCLK );
	HANDLE_MSG( h_wnd, WM_RBUTTONDOWN, emacs_cls_WM_RBUTTONDOWN );
	HANDLE_MSG( h_wnd, WM_RBUTTONUP, emacs_cls_WM_RBUTTONUP );
	HANDLE_MSG( h_wnd, WM_RBUTTONDBLCLK, emacs_cls_WM_RBUTTONDBLCLK );
	HANDLE_MSG( h_wnd, WM_MBUTTONDOWN, emacs_cls_WM_MBUTTONDOWN );
	HANDLE_MSG( h_wnd, WM_MBUTTONUP, emacs_cls_WM_MBUTTONUP );
	HANDLE_MSG( h_wnd, WM_MBUTTONDBLCLK, emacs_cls_WM_MBUTTONDBLCLK );
	HANDLE_MSG( h_wnd, WM_KEYDOWN, emacs_cls_WM_KEYDOWN );
	HANDLE_MSG( h_wnd, WM_KEYUP, emacs_cls_WM_KEYUP );
	HANDLE_MSG( h_wnd, WM_SYSKEYDOWN, emacs_cls_WM_SYSKEYDOWN );
	HANDLE_MSG( h_wnd, WM_SYSKEYUP, emacs_cls_WM_SYSKEYUP );
	HANDLE_MSG( h_wnd, WM_CHAR, emacs_cls_WM_CHAR );
/*	HANDLE_MSG( h_wnd, WM_HSCROLL, emacs_cls_WM_HSCROLL );	*/
	HANDLE_MSG( h_wnd, WM_VSCROLL, emacs_cls_WM_VSCROLL );
	HANDLE_MSG( h_wnd, WM_PAINT, emacs_cls_WM_PAINT );
	HANDLE_MSG( h_wnd, WM_CLOSE, emacs_cls_WM_CLOSE );
	default:
		TranslateMessage( &emacs_win_msg );
		return DefWindowProc(h_wnd, msg, wParam, lParam);
	}

	return 0l;
	}


/*
 *	This function is called when Emacs needs to wait for an event
 */
static process_event( int can_wait )
	{
	int status;

#if	DBG_PROFILE
	if( DBG_PROFILE&dbg_flags )
		ProfStop();
#endif

	if( can_wait )
		status = GetMessage( &emacs_win_msg, NULL, 0, 0 );
	else
		status = PeekMessage( &emacs_win_msg, NULL, 0, 0, 1 );

#if	DBG_PROFILE
	if( DBG_PROFILE&dbg_flags )
		ProfStart();
#endif
	if( !status )
		return -1;

	if( (h_find_dlg == 0 || !IsDialogMessage( h_find_dlg, &emacs_win_msg ) )
	&& (h_replace_dlg == 0 || !IsDialogMessage( h_replace_dlg, &emacs_win_msg ) ))
		{
		DispatchMessage( &emacs_win_msg );
		}

	if( emacs_quit )
		return -1;
	return 0;
	}

int wait_for_activity(void)
	{
	return process_event( 1 );
	}

#define CHECK_FOR_ACTIVITY_INTERVAL 100
void check_for_activity(void)
	{
	static check_counter = 0;
	check_counter++;
	check_counter %= CHECK_FOR_ACTIVITY_INTERVAL;
	if( check_counter == 0 )
		while( process_event( 0 ) == 0 )
			;
	}

BOOL CALLBACK EXPORT error_dlg_proc
	(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	)
	{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt( hDlg, ID_CODE, (WORD)lParam, 0 );
		return (TRUE);

	case WM_COMMAND:
		if (wParam == IDOK)
		{
		EndDialog(hDlg, TRUE);
		return (TRUE);
		}
		break;
	}
	return (FALSE);
	}

void fatal_error( int code )
	{
	char buf[128];
	sprintf( buf, "fatal_error: %d", code );
	OutputDebugString( buf );

	DialogBoxParam
	(
	emacs_hinst,
	MAKEINTRESOURCE( DLG_ERROR ),
	emacs_hwnd,
	WATCOM_DLGPROC (DLGPROC)error_dlg_proc,
	code
	);

	exit(0);
	}

void wait_abit( void )
	{
	long target, current;

	target = GetCurrentTime();
	target += 100l;	/* 100ms/counter */

	do
		current = GetCurrentTime();
	while( current < target );
	}

unsigned char *get_tmp_path(void)
	{
	static unsigned char tmp_path[MAXPATHLEN];

#if defined(_NT)
	DWORD len;

	len = GetTempPath( sizeof( tmp_path ), s_str(tmp_path));
#elif defined(_MSDOS)                                   
	tmp_path[0] = 0;
	GetTempFileName( 0, "z", 0, s_str( tmp_path ) );
	{ 
	unsigned char *p;

	p = _str_rchr( tmp_path, PATH_CH );
	if( p != NULL )
		*p = 0;
	}
#elif defined( vms )
	_str_cpy( tmp_buf, "sys$scratch:");
#else 
#error "Need a temp path"
#endif
	return tmp_path;
	}         

unsigned char *get_config_env( unsigned char *name )
	{
	static unsigned char get_config_env_string[256];
	unsigned char *p;

	get_config_env_string[0] = '\0';

	GetPrivateProfileString
	(
	"Environment", s_str(name),
	s_str(get_env( name )),
	s_str(get_config_env_string), sizeof( get_config_env_string )-1,
	"emacs060.ini"
	);

	p = get_config_env_string;
	while( *p )
		if( *p == '|' )
			*p++ = ';';
		else
			p++;

	if( get_config_env_string[0] != '\0' )
		return get_config_env_string;
	else
		return NULL;
	}

void invoke_debug(void)
	{
	return;
	}

int profile_emacs(void)
	{
	return no_value_command();
	}

int dump_histogram(void)
	{
	return no_value_command();
	}

#if defined(_NT)
#if !defined(MAX_USERNAME_LENGTH)
# define MAX_USERNAME_LENGTH 32
#endif

static char user_name_buf[MAX_USERNAME_LENGTH];
char *cuserid( char *str )
	{
	DWORD buf_size = sizeof( user_name_buf );
	if( !GetUserName( user_name_buf, &buf_size ) )
		return NULL;
	else
		return user_name_buf;
	}
#elif defined(_MSDOS)
char *cuserid( char * i )
	{
	return NULL;
	}
#else
#error "Need cuserid logic..."
#endif

void interlock_dec( int *counter )
	{
	(*counter)--;
	}

void interlock_inc( int *counter )
	{
	(*counter)++;
	}

void conditional_wake(void)
	{
	return;
	}

void get_user_full_name( unsigned char *users_name, unsigned char *users_full_name )
	{
#if defined(_NT)
	DWORD size = 16;
	users_full_name[0] = '\0';
	GetUserName( s_str(users_full_name), &size );
#endif
	}

void get_system_name( unsigned char *system_name )
	{
#if defined(_NT)
	DWORD size = MAX_COMPUTERNAME_LENGTH;
	system_name[0] = '\0';
	GetComputerName( s_str(system_name), &size );
#endif
	}

int put_config_env( unsigned char *name, unsigned char *value )
	{
	return WritePrivateProfileString
		(
		"Environment", s_str(name),
		value[0] == '\0' ? NULL : s_str(value),
		"emacs060.ini"
		);
	}

char *get_device_name_translation( char *name )
	{
	static char get_device_env_string[256];

	get_device_env_string[0] = '\0';

	if( GetPrivateProfileString
	(
	"DeviceNames", s_str(name),
	"",
	get_device_env_string, sizeof( get_device_env_string )-1,
	"emacs060.ini"
	) > 0 )
		return get_device_env_string;
	else
		return NULL;
	}
