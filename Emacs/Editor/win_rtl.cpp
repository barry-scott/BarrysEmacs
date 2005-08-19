/*
 *	win_rtl.cpp
 *
 *	All the windows support functions for emacs live in here
 */
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <win_emacs.h>
#include <win_registry.h>

#include <win_main_frame.h>
#include <win_doc.h>
#include <win_view.h>
#include <math.h>


int		win_emacs_quit;
int		is_windows_nt;

#if !defined( _NT )
int GetLastError(void) { return 0; }

#endif


void _dbg_msg( const EmacsString &msg )
	{
	if( !(dbg_flags&DBG_NO_DBG_MSG) )
		{
		EmacsString mbox_msg( msg );
		mbox_msg.append("\nDo you wish to see further Debug messages?");

		int button = MessageBox( NULL, mbox_msg, "Debug", MB_ICONINFORMATION|MB_YESNO|MB_DEFBUTTON1 );
		if( button == IDNO )
			dbg_flags |= DBG_NO_DBG_MSG;
		}

	int call_depth = _dbg_fn_trace::callDepth();
	EmacsString msg2;

	if( call_depth < 40 )
		{
		while( call_depth > 0 )
			{
			msg2.append( "| " );
			call_depth--;
			}
		}
	else
		msg2.append( FormatString( "[%d] | " ) << call_depth );

	msg2.append( msg );
	msg2.append("\n");
	// and log to debug terminal
	DebugPrintf( msg2 );
	if( dbg_flags&DBG_DUMPTOFILE )
		{
		static HANDLE file = CreateFile
					( 
					"emacs.debug",
					GENERIC_WRITE,
					FILE_SHARE_READ,
					0,
					OPEN_ALWAYS,
					0,
					0
					);
		static DWORD pos_status( SetFilePointer( file, 0, 0, FILE_END ) );
		DWORD bytes_written = 0;
		if( file != INVALID_HANDLE_VALUE )
			WriteFile( file, msg2.sdata(), msg2.length(), &bytes_written, NULL );
		}		
	}

void DebugPrintf( const EmacsString &text )
	{
	EmacsString buf( FormatString( "Emacs(T:0x%x): %s" ) << GetCurrentThreadId() << text );

	if( buf[-1] != '\n' )
		buf.append( "\n" );

	OutputDebugString( buf );
	}

void wait_abit( void )
	{
	//	start the timer
	UINT status = theApp.m_pMainWnd->SetTimer( 'Z', 100, NULL );
	if( status == 0 )
		{
		TRACE("Failed to allocate timer\n");
		return;
		}

	theApp.m_tick = false;
	while( !theApp.m_tick && input_pending == 0 )
		wait_for_activity();
	theApp.m_pMainWnd->KillTimer( 'Z' );	
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

EmacsString get_config_env( const EmacsString &name )
	{
	CString value = AfxGetApp()->GetProfileString
		(
		"Environment", name,
		getenv( name )
		);

	if( !value.IsEmpty() )
		return value;

	return "";
	}

int put_config_env( const EmacsString &name, const EmacsString &value )
	{
	return AfxGetApp()->WriteProfileString
		(
		"Environment", name,
		value.length() ==0 ? NULL : value.sdata()
		);
	}

EmacsString get_device_name_translation( const EmacsString &name )
	{
	return theApp.GetProfileString( "DeviceNames", name, "" );
	}

int get_file_parsing_override( const char *disk, int def_override )
	{
	return AfxGetApp()->GetProfileInt( "FileParsing", disk, def_override );
	}

void debug_invoke(void)
	{ return; }

void debug_exception(void)
	{ return; }

#if DBG_SER
void debug_SER(void)
	{ return; }
#endif

#if defined(_NT)
#if !defined(MAX_USERNAME_LENGTH)
# define MAX_USERNAME_LENGTH 32
#endif


EmacsString users_login_name()
	{
	char user_name_buf[MAX_USERNAME_LENGTH];
	DWORD buf_size = sizeof( user_name_buf );

	if( !GetUserName( user_name_buf, &buf_size ) )
		return EmacsString::null;
	else
		return EmacsString( user_name_buf );
	}

#elif defined(_MSDOS)
char *user_login_name( char * i )
	{
	return NULL;
	}
#else
#error "Need cuserid logic..."
#endif

EmacsString get_user_full_name()
	{
#if defined(_NT)
	char users_full_name[MAX_USERNAME_LENGTH];
	DWORD size = sizeof( users_full_name );
	users_full_name[0] = '\0';
	GetUserName( users_full_name, &size );
	return EmacsString( users_full_name );
#endif
	}

EmacsString get_system_name()
	{
#if defined(_NT)
	char system_name[MAX_COMPUTERNAME_LENGTH+1];
	DWORD size = MAX_COMPUTERNAME_LENGTH;
	system_name[0] = '\0';
	GetComputerName( system_name, &size );
	return EmacsString( system_name );
#endif
	}


int interlock_dec( volatile int *counter )
	{
	return InterlockedDecrement( (long *)counter );
	}

int interlock_inc( volatile int *counter )
	{
	return InterlockedIncrement( (long *)counter );
	}

extern DWORD main_thread_id;
void wake_main_thread(void)
	{
	PostThreadMessage( main_thread_id, WM_USER+2, 0, 0);
	}

void conditional_wake(void)
	{
	return;
	}

EmacsString::EmacsString( const CString &string )
	: _rep( EMACS_NEW EmacsStringRepresentation( copy, 0, string.GetLength(), (unsigned char *)(LPCTSTR)string ) )
	{
	check_for_bad_value( _rep );
	}


EmacsString &EmacsString::operator=( const CString &string )
	{
	check_for_bad_value( _rep );
	_rep->ref_count--;
	if( _rep->ref_count == 0 )
		delete _rep;

	_rep = EMACS_NEW EmacsStringRepresentation( copy, 0, string.GetLength(), (const unsigned char *)(LPCTSTR)string );
	check_for_bad_value( _rep );
	return *this;
	}

EmacsString::operator const CString() const
	{
	return CString( sdata(), length() );
	}


int elapse_time()
	{
	return GetTickCount();
	}

EmacsDateTime EmacsDateTime::now(void)
	{
	EmacsDateTime now;

	SYSTEMTIME sys_time;
	FILETIME file_time;

	GetSystemTime( &sys_time );
	SystemTimeToFileTime( &sys_time, &file_time );

	// a file time is in 100nS units
	now.time_value = double( file_time.dwHighDateTime );
	now.time_value *= 65536;
	now.time_value *= 65536;
	now.time_value += double( file_time.dwLowDateTime );
	now.time_value /= 10000000.0;
	return now;
	}

EmacsString EmacsDateTime::asString(void) const
	{
	double time( time_value );

	time *= 10000000.0;
	double low_16 = fmod( time, double(65536) );
	time = time / 65536;
	double high_16 = fmod( time, double(65536) );
	time = time / 65536;

	FILETIME file_time;
	file_time.dwLowDateTime = int(high_16) << 16;
	file_time.dwLowDateTime |= int(low_16);
	file_time.dwHighDateTime = int(time);

	FILETIME local_file_time;
	FileTimeToLocalFileTime( &file_time, &local_file_time );

	SYSTEMTIME sys_time;
	FileTimeToSystemTime( &local_file_time, &sys_time );

	return FormatString("%4d-%2d-%2d %2d:%2d:%2d.%3.3d")
		<< sys_time.wYear << sys_time.wMonth << sys_time.wDay
		<< sys_time.wHour << sys_time.wMinute << sys_time.wSecond << sys_time.wMilliseconds;
	}
