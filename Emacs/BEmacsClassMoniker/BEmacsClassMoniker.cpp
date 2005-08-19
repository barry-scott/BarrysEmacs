// BEmacsClassMoniker.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To merge the proxy/stub code into the object DLL, add the file 
//      dlldatax.c to the project.  Make sure precompiled headers 
//      are turned off for this file, and add _MERGE_PROXYSTUB to the 
//      defines for the project.  
//
//      If you are not running WinNT4.0 or Win95 with DCOM, then you
//      need to remove the following define from dlldatax.c
//      #define _WIN32_WINNT 0x0400
//
//      Further, if you are running MIDL without /Oicf switch, you also 
//      need to remove the following define from dlldatax.c.
//      #define USE_STUBLESS_PROXY
//
//      Modify the custom build rule for BEmacsClassMoniker.idl by adding the following 
//      files to the Outputs.
//          BEmacsClassMoniker_p.c
//          dlldata.c
//      To build a separate proxy/stub DLL, 
//      run nmake -f BEmacsClassMonikerps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>
#include "BEmacsClassMoniker.h"
#include "dlldatax.h"

#include "BEmacsClassMoniker_i.c"
#include "BemacsMoniker.h"
#include <BarrysEmacs.h>

#ifdef _MERGE_PROXYSTUB
extern "C" HINSTANCE hProxyDll;
#endif

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_BemacsMoniker, CBemacsMoniker)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    lpReserved;
#ifdef _MERGE_PROXYSTUB
    if (!PrxDllMain(hInstance, dwReason, lpReserved))
        return FALSE;
#endif
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        _Module.Init(ObjectMap, hInstance, &LIBID_BEMACSCLASSMONIKERLib);
        DisableThreadLibraryCalls(hInstance);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
        _Module.Term();
    return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllCanUnloadNow() != S_OK)
        return S_FALSE;
#endif
    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
#ifdef _MERGE_PROXYSTUB
    if (PrxDllGetClassObject(rclsid, riid, ppv) == S_OK)
        return S_OK;
#endif
    return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	//::MessageBox( NULL, "BEmacs Moniker is being registered", "BEmacs Moniker Setup", MB_OK );

	char dll_file_name[256];
	GetModuleFileName( _Module.m_hInst, dll_file_name, sizeof( dll_file_name ) );

	char *p = dll_file_name;
	char *file_name_start = p;
	while( *p )
		if( *p++ == '\\' )
			file_name_start = p;

	strcpy( file_name_start, "BEmacsServer.exe" );


	PROCESS_INFORMATION proc_info;
	STARTUPINFO start_info;

	start_info.cb = sizeof( start_info );

	start_info.lpReserved = 0;
	start_info.lpDesktop = 0;
	start_info.lpTitle = 0;
	start_info.dwX = 0;
	start_info.dwY = 0;
	start_info.dwXSize = 0;
	start_info.dwYSize = 0;
	start_info.dwXCountChars = 0;
	start_info.dwYCountChars = 0;
	start_info.dwFillAttribute = 0;
	start_info.dwFlags = 0;
	start_info.wShowWindow = 0;
	start_info.cbReserved2 = 0;
	start_info.lpReserved2 = 0;
	start_info.hStdInput = 0;
	start_info.hStdOutput = 0;
	start_info.hStdError = 0;

	BOOL status = ::CreateProcess
		(
		dll_file_name,
		"emacs /regserver",
		NULL,
		NULL,
		false,
		0,
		NULL,
		NULL,
		&start_info,
		&proc_info
		);
	if( status )
		::WaitForSingleObject( proc_info.hProcess, INFINITE );
	else
		::MessageBox( NULL, dll_file_name, "Failed to register BEmacsServer", MB_OK );

#ifdef _MERGE_PROXYSTUB
    HRESULT hRes = PrxDllRegisterServer();
    if (FAILED(hRes))
        return hRes;
#endif
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	//::MessageBox( NULL, "BEmacs Moniker is being unregistered", "BEmacs Moniker uninstall", MB_OK );

	char dll_file_name[256];
	GetModuleFileName( _Module.m_hInst, dll_file_name, sizeof( dll_file_name ) );

	char *p = dll_file_name;
	char *file_name_start = p;
	while( *p )
		if( *p++ == '\\' )
			file_name_start = p;

	strcpy( file_name_start, "BEmacsServer.exe" );


	PROCESS_INFORMATION proc_info;
	STARTUPINFO start_info;

	start_info.cb = sizeof( start_info );

	start_info.lpReserved = 0;
	start_info.lpDesktop = 0;
	start_info.lpTitle = 0;
	start_info.dwX = 0;
	start_info.dwY = 0;
	start_info.dwXSize = 0;
	start_info.dwYSize = 0;
	start_info.dwXCountChars = 0;
	start_info.dwYCountChars = 0;
	start_info.dwFillAttribute = 0;
	start_info.dwFlags = 0;
	start_info.wShowWindow = 0;
	start_info.cbReserved2 = 0;
	start_info.lpReserved2 = 0;
	start_info.hStdInput = 0;
	start_info.hStdOutput = 0;
	start_info.hStdError = 0;

	BOOL status = ::CreateProcess
		(
		dll_file_name,
		"emacs /unregserver",
		NULL,
		NULL,
		false,
		0,
		NULL,
		NULL,
		&start_info,
		&proc_info
		);
	if( status )
		::WaitForSingleObject( proc_info.hProcess, INFINITE );
	else
		::MessageBox( NULL, dll_file_name, "Failed to unregister BEmacsServer", MB_OK );

#ifdef _MERGE_PROXYSTUB
    PrxDllUnregisterServer();
#endif
    return _Module.UnregisterServer(TRUE);
}
