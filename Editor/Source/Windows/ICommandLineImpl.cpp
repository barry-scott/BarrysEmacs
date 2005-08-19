// ICommandLineImpl.cpp : Implementation of CEditorApp and DLL registration.

#include <emacs.h>

#include <win_emacs.h>

#if defined( _DEBUG ) || 1
#define	_DebugPrintf( str ) DebugPrintf( str )
#else
#define	_DebugPrintf( str )
#endif

#undef read

//--ATL Support------------------------------------------------------------------------------

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
//#include <statreg.cpp>
#endif

class CExeModule : public CComModule
	{
public:
	LONG Unlock();
	DWORD dwThreadID;
	};

extern CExeModule _Module;

#include <atlcom.h>
#include <oaidl.h>

//--ATL Support------------------------------------------------------------------------------


#include "ICommandLineImpl.h"

/////////////////////////////////////////////////////////////////////////////
//
BOOL EmacsIsEqualGUID(REFGUID rguid1, REFGUID rguid2)
{
   return (
	  ((PLONG) &rguid1)[0] == ((PLONG) &rguid2)[0] &&
	  ((PLONG) &rguid1)[1] == ((PLONG) &rguid2)[1] &&
	  ((PLONG) &rguid1)[2] == ((PLONG) &rguid2)[2] &&
	  ((PLONG) &rguid1)[3] == ((PLONG) &rguid2)[3]);
}


STDMETHODIMP CCommandLine::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_ICommandLine,
	};

	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (EmacsIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


extern DWORD main_thread_id;

class WindowsEmacsCommandLineServerWorkItem : public EmacsCommandLineServerWorkItem
	{
public:
	WindowsEmacsCommandLineServerWorkItem()
		: EmacsCommandLineServerWorkItem()
		, event( CreateEvent( NULL, TRUE, FALSE, NULL ) )	// Manual reset, not signalled event
		{ }

	virtual ~WindowsEmacsCommandLineServerWorkItem()
		{
		CloseHandle( event );
		}

	void waitUntilCommandProcessed()
		{
		_DebugPrintf( "WindowsEmacsCommandLineServerWorkItem::waitUntilCommandProcessed() waiting\n" );

		// kick main thread
		PostThreadMessage( main_thread_id, WM_USER+1, 0, 0);

		// wait till its done
		WaitForSingleObjectEx( event, INFINITE, FALSE );

		_DebugPrintf( "WindowsEmacsCommandLineServerWorkItem::waitUntilCommandProcessed() wait done\n" );
		}

private:
	virtual void workAction(void)
		{
		_DebugPrintf( "WindowsEmacsCommandLineServerWorkItem::workAction start\n" );
		EmacsCommandLineServerWorkItem::workAction();
		SetEvent( event );
		_DebugPrintf( "WindowsEmacsCommandLineServerWorkItem::workAction done\n" );
		}

	HANDLE event;
	};


STDMETHODIMP CCommandLine::CommandLine(BSTR _current_working_directory, BSTR _command_line)
{

	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	EmacsString current_working_directory( _current_working_directory );
	EmacsString command_line( _command_line );

	_DebugPrintf( FormatString("CCommandLine::CommandLine( %s, %s )\n")
			<< current_working_directory << command_line );

	WindowsEmacsCommandLineServerWorkItem item;
	
	item.newCommandLine( current_working_directory, command_line );

	item.waitUntilCommandProcessed();

	return S_OK;
}

DWORD registation_id = 0;
EmacsString bemacs_process_name;

STDMETHODIMP CCommandLine::SetProcessName(BSTR _process_name)
	{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())

	EmacsString process_name( _process_name );

	_DebugPrintf( FormatString("CCommandLine::SetProcessName( %s )\n") << process_name );

	// only set the name once
	if( registation_id != 0 )
		{
		_DebugPrintf( "CCommandLine::SetProcessName() - already registered\n" );
		return S_OK;
		}

	if( process_name.length() > 127 )
		{
		_DebugPrintf( "CCommandLine::SetProcessName() - name too long\n" );
		return E_INVALIDARG;
		}

	bemacs_process_name = process_name;

	IBindCtx *bind_ctx = NULL;

	HRESULT hr = CreateBindCtx( 0, &bind_ctx );
	if( FAILED( hr ) )
		return hr;

	ULONG eaten = 0;
	IMoniker *moniker = NULL;

	unsigned short moniker_string[128+7];
	wcscpy( moniker_string, L"bemacs:" );

	unsigned short *o = moniker_string;
	while( *o )
		o++;
	const char *i = process_name;
	while( (*o++ = *i++) != 0 )
		;

	hr = MkParseDisplayName( bind_ctx, moniker_string, &eaten, &moniker );
	if( FAILED( hr ) )
		{
		bind_ctx->Release();
		return hr;
		}

	bind_ctx->Release();

	IRunningObjectTable *rot = NULL;
	hr = GetRunningObjectTable( 0, &rot );
	if( FAILED( hr ) )
		{
		moniker->Release();
		return hr;
		}

	IUnknown *unknown = NULL;

	hr = QueryInterface( IID_IUnknown, (void **)&unknown );
	if( FAILED(hr) )
		{
		_DebugPrintf( FormatString("CCommandLine::SetProcessName: dispatch->QueryInterface() failed hr=%X\n") << hr );

		rot->Release();

		return hr;
		}

	hr = rot->Register( ROTFLAGS_REGISTRATIONKEEPSALIVE, unknown, moniker, &registation_id );
	if( FAILED(hr) )
		_DebugPrintf( FormatString("CCommandLine::SetProcessName: rot->Register() failed hr=%X\n") << hr );
	else
		_DebugPrintf( FormatString("CCommandLine::SetProcessName: rot->Register() => Id=%X\n") << registation_id );

	FILETIME filetime;
	CoFileTimeNow( &filetime );
	rot->NoteChangeTime( registation_id, &filetime );

	rot->Release();

	return hr;
	}

// called when emacs is about to exit
void revoke_rot_registration(void)
	{
	_DebugPrintf( FormatString("revoke_rot_registration() Id=%X\n") << registation_id );

	if( registation_id == 0 )
		// nothing to do
		return;

	IRunningObjectTable *rot = NULL;
	HRESULT hr = GetRunningObjectTable( 0, &rot );
	if( FAILED( hr ) )
		return;

	hr = rot->Revoke( registation_id );
	if( FAILED(hr) )
		{
		_DebugPrintf( FormatString("rot->Revoke() failed hr=%X\n") << hr );
		::MessageBox( NULL, "Failed to revoke ROT registration", "Bemacs Server error", MB_OK );
		}
	rot->Release();
	}

STDMETHODIMP CCommandLine::GetProcessId(long *id)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState())


	*id = GetCurrentProcessId();

	_DebugPrintf( FormatString("c:CCommandLine:GetProcessId() pid=%d\n") << *id );

	return S_OK;
}
