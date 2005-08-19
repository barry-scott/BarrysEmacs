//
//	BEmacsComClient.cpp
//		Copyright (c) 1997-2002 Barry A. Scott
//
#define INITGUID
#include <windows.h>

#include <cguid.h>

#include <BarrysEmacs.h>

#include <tchar.h>

#include <resource.h>

void check( HRESULT hresult );

HRESULT EmacsCoGetObject( LPCWSTR pszName, BIND_OPTS *pBindOptions, REFIID riid, void **ppv );

void copyString( wchar_t *out, const char *in );


class DebugTrace
	{
public:
	DebugTrace( const wchar_t *prefix )
		: m_prefix( prefix )
		{
		initBuffer();
		}

	~DebugTrace()
		{
		}

	void add( const wchar_t *t )
		{
		wcscat( m_buffer, t );
		}

	void addLast( const wchar_t *t )
		{
		add( t );
		add( L"\n" );

		OutputDebugString( m_buffer );

		initBuffer();
		}

private:
	void initBuffer()
		{
		m_buffer[0] = 0;
		add( m_prefix );
		}

	const wchar_t *m_prefix;
	wchar_t m_buffer[4096];
	};


DebugTrace _t( L"BEmacs COM client: " );


int WINAPI WinMain
	(
	HINSTANCE hInstance,		// handle to current instance
	HINSTANCE /* hPrevInstance */,	// handle to previous instance
	LPSTR /* lpCmdLine */,		// pointer to command line
	int /* nCmdShow */		// show state of window
	)
	{
	_t.addLast( L"@#@version@#@" );

	HRESULT hresult = 0;
	DWORD rc = 0;

	hresult = CoInitialize(NULL);
	check( hresult );

	TCHAR current_directory[1024];
	rc = GetCurrentDirectory( sizeof( current_directory )/sizeof( TCHAR ), current_directory );
	if( rc == 0 )
		{
		// Oh we are on Windows 9X
		char current_directory_a[1024];
		rc = GetCurrentDirectoryA( sizeof( current_directory_a )/sizeof( char ), current_directory_a );
		if( rc == 0 )
			{
			_t.addLast( L"Failed to get current directory" );
			MessageBox( NULL, L"Failed to get current directory", L"BEmacs error", MB_OK|MB_ICONERROR );
			return 0;
			}

		copyString( current_directory, current_directory_a );
		}

	// load the prefix and strip leading spaces
	char tmp_char_buf_a[1024];
	TCHAR cmd_prefix_buf[1024];
	int cmd_prefix_len = LoadStringA( hInstance, IDS_CMD_PREFIX, &tmp_char_buf_a[0], sizeof(tmp_char_buf_a)/sizeof(tmp_char_buf_a[0]) );
	if( cmd_prefix_len == 0 )
		tmp_char_buf_a[0] = 0;
	copyString( cmd_prefix_buf, tmp_char_buf_a );
	LPTSTR cmd_prefix_str = cmd_prefix_buf;
	while( *cmd_prefix_str == ' ' )
		cmd_prefix_str++;

	// load the suffix and strip leading spaces
	TCHAR cmd_suffix_buf[1024];
	int cmd_suffix_len = LoadStringA( hInstance, IDS_CMD_SUFFIX, &tmp_char_buf_a[0], sizeof(tmp_char_buf_a)/sizeof(tmp_char_buf_a[0]) );
	if( cmd_suffix_len == 0 )
		tmp_char_buf_a[0] = 0;
	copyString( cmd_suffix_buf, tmp_char_buf_a );
	LPTSTR cmd_suffix_str = cmd_suffix_buf;
	while( *cmd_suffix_str == ' ' )
		cmd_suffix_str++;

	// get the comamnd line the user supplied
	LPTSTR cmd_user_cmd_line_str = GetCommandLine();
	TCHAR process_name[1024];
	int process_name_len = LoadStringA( hInstance, IDS_PROCNAME, &tmp_char_buf_a[0], sizeof(tmp_char_buf_a)/sizeof(tmp_char_buf_a[0]) );
	if( process_name_len == 0 )
		wcscpy( process_name, _T("main") );
	else
		copyString( process_name, tmp_char_buf_a );

	_t.add( L"GetCommandLine(): " );
	_t.addLast( cmd_user_cmd_line_str );


	//
	//	Lose the first arg - its the path to the image
	//
	if( *cmd_user_cmd_line_str == '"' )
		{
		cmd_user_cmd_line_str++;

		while( *cmd_user_cmd_line_str != 0 && *cmd_user_cmd_line_str != '"' )
			cmd_user_cmd_line_str++;
		if( *cmd_user_cmd_line_str == '"' )
			cmd_user_cmd_line_str++;
		}
	else
		{
		while( *cmd_user_cmd_line_str != 0 && *cmd_user_cmd_line_str != ' ' )
			cmd_user_cmd_line_str++;
		}

	while( *cmd_user_cmd_line_str == ' ' )
		cmd_user_cmd_line_str++;

	//
	//	Assemble the whole command line
	//	[<prefix> SPC] user-cmd-line [SPC <suffix>]
	//
	TCHAR command_line[4096];
	command_line[0] = 0;

	if( cmd_prefix_str[0] != 0 )
		{
		wcscat( command_line, cmd_prefix_str );
		wcscat( command_line, _T(" ") );
		}
	wcscat( command_line, cmd_user_cmd_line_str );
	if( cmd_suffix_str[0] != 0 )
		{
		wcscat( command_line, _T(" ") );
		wcscat( command_line, cmd_suffix_str );
		}

	_t.add( L"command_line: [" );
	_t.add( command_line );
	_t.addLast( L"]" );


	// look for name parameter its [-/]name[:=]proc-name

	// for now only find /name= at the start of the command line
	{
	LPTSTR command_line_ptr = command_line;

	while( *command_line_ptr != 0 )
		{
		if( wcsncmp( command_line_ptr, _T("/name="), 6 ) == 0
		||  wcsncmp( command_line_ptr, _T("-name="), 6 ) == 0
		||  wcsncmp( command_line_ptr, _T("/name:"), 6 ) == 0
		||  wcsncmp( command_line_ptr, _T("-name:"), 6 ) == 0 )
			{
			LPTSTR name_start_ptr = command_line_ptr;

			// skip /name=
			command_line_ptr += 6;
			
			LPTSTR procname_ptr = process_name;
			while( *command_line_ptr != 0 && *command_line_ptr != ' ' )
				{
				*procname_ptr++ = *command_line_ptr++;
				}
			*procname_ptr = 0;


			while( *command_line_ptr == ' ' )
				command_line_ptr++;

			// copy the rest of the command line down to remove the /name=foo [SPC]
			for(;;)
				{
				TCHAR ch = *command_line_ptr++;
				*name_start_ptr++ = ch;
				if( ch == 0 )
					break;
				}

			_t.add( L"command_line sans /name: [" );
			_t.add( command_line );
			_t.addLast( L"]" );

			break;
			}

		command_line_ptr++;
		}
	}

	// the moniker is bemacs:<proc-name>
	TCHAR moniker[128];

	wcscpy( moniker, _T("bemacs:") );
	wcscat( moniker, process_name );
	
	_t.add( L"moniker is [" );
	_t.add( moniker );
	_t.addLast( L"]" );


	BIND_OPTS bind_opts =
		{
		sizeof( bind_opts ),
		0,
		STGM_READWRITE,
		0
		};

	_t.addLast( L"Before EmacsCoGetObject" );
	
	IDispatch *pdisp;
	hresult = EmacsCoGetObject( moniker, &bind_opts, IID_IDispatch, (void **)&pdisp );
	check( hresult );

	_t.addLast( L"Before GetIDsOfNames" );

	OLECHAR FAR* szMember[] = { _TEXT("CommandLine"), _TEXT("GetProcessId") };
	DISPID dispid[] = { 0, 0 };

	hresult = pdisp->GetIDsOfNames
		(
		IID_NULL, szMember, sizeof( szMember )/sizeof( OLECHAR FAR * ),
		LOCALE_USER_DEFAULT, dispid
		);
	check( hresult );



	
	//--------------------------------------------------------------------------------
	// only W2K has the API to allow this proc to pass on the ability to be the
	// foreground window
	//
	HMODULE hUser32 = GetModuleHandleA( "user32" );
	typedef BOOL (_stdcall *AllowSetForegroundWindow_t)( DWORD) ;
	AllowSetForegroundWindow_t pAllowSetForegroundWindow = (AllowSetForegroundWindow_t)GetProcAddress( hUser32, "AllowSetForegroundWindow" );

	if( pAllowSetForegroundWindow != NULL )
		{
		_t.addLast( L"Before Invoke GetProcessId" );
		long pid = 0;

		DISPPARAMS dispParams = {NULL, NULL, 0, 0};
		VARIANT retValue;

		hresult = pdisp->Invoke
			(
			dispid[1],
			IID_NULL,
			LOCALE_USER_DEFAULT,
			DISPATCH_METHOD,
			&dispParams,
			&retValue, NULL, NULL
			);
		check( hresult );

		pid = retValue.lVal;

		BOOL allowed = pAllowSetForegroundWindow( pid );
		if( allowed )
			_t.addLast( L"After AllowSetForegroundWindow allowed" );
		else
			_t.addLast( L"After AllowSetForegroundWindow not allowed" );
		}


	//--------------------------------------------------------------------------------
	_t.addLast( L"Before Invoke CommandLine" );

	{
	VARIANTARG string_arg[2] =
		{
		{VT_BSTR, 0, 0, 0},
		{VT_BSTR, 0, 0, 0}
		};
	DISPPARAMS dispParams = {string_arg, NULL, 2, 0};
	string_arg[1].bstrVal = ::SysAllocString( current_directory );
	string_arg[0].bstrVal = ::SysAllocString( command_line );

	hresult = pdisp->Invoke
		(
		dispid[0],
		IID_NULL,
		LOCALE_USER_DEFAULT,
		DISPATCH_METHOD,
		&dispParams,
		NULL, NULL, NULL
		);
	check( hresult );
	}

	_t.addLast( L"Before pDisp Release" );
	pdisp->Release();

	_t.addLast( L"Before CoUninitialize" );
	CoUninitialize();


	_t.addLast( L"all done" );
	return 0;
	}

void check( HRESULT hresult )
	{
	if( SUCCEEDED(hresult) )
		return;

	void *msg = NULL;

	FormatMessage
	(
	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
	NULL,
	hresult,
	MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
	(LPTSTR) &msg,
	0,
	NULL
	);

	_t.add( L"check HRESULT failed: " );
	_t.addLast( (LPTSTR)msg );
	MessageBox( NULL, (LPTSTR)msg, _TEXT("Bemacs Error"), MB_OK );

	CoUninitialize();

	_t.addLast( L"ExitProcess(1)" );
	ExitProcess(1);
	}

//
//	CoGetObject is not available on Windows 95
//	Here is Emacs's private version of CoGetObject
//
HRESULT EmacsCoGetObject( LPCWSTR pszName, BIND_OPTS * /* pBindOptions */,  REFIID riid,  void **ppv )
	{
	HRESULT hr;

	_t.add( L"EmacsCoGetObject( " );
	_t.add( pszName );
	_t.addLast( L")" );

	//
	//	Create the bind context
	//
	_t.addLast( L"EmacsCoGetObject before CreateBindCtx" );
	IBindCtx *pbc = NULL;
	hr = CreateBindCtx( 0, &pbc );
	if( FAILED( hr ) )
		{
		_t.addLast( L"EmacsCoGetObject CreateBindCtx failed" );
		return hr;
		}
	//
	//	Parse the moniker
	//
	_t.addLast( L"EmacsCoGetObject before MkParseDisplayName" );
	ULONG eaten = 0;
	IMoniker *pmk = NULL;
	hr = MkParseDisplayName( pbc, pszName, &eaten, &pmk );
	if( FAILED( hr ) )
		{
		_t.addLast( L"EmacsCoGetObject MkParseDisplayName failed" );
		pbc->Release();
		return hr;
		}

	_t.addLast( L"EmacsCoGetObject before BindToObject" );
	hr = pmk->BindToObject( pbc, NULL, riid, ppv );
	if( SUCCEEDED( hr ) )
		_t.addLast( L"EmacsCoGetObject BindToObject succeeded" );
	else
		_t.addLast( L"EmacsCoGetObject BindToObject failed" );

	pbc->Release();
	pmk->Release();

	return hr;
	}

void copyString( wchar_t *out, const char *in )
	{
	for(;;)
		{
		char ch = *in++;
		*out++ = (wchar_t)ch;
		if( ch == 0 )
			break;
		}
	}
