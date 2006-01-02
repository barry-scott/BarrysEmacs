//
//
//    win_com_support.cpp
//
//    Copyright (c) 2000 Barry A. Scott
//

#include <emacs.h>

#include <win_emacs.h>

#undef read

//--ATL Support------------------------------------------------------------------------------

#define _ATL_APARTMENT_THREADED

#include <atlbase.h>

#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
//#include <statreg.cpp>
#endif

//--ATL Support------------------------------------------------------------------------------
//#include <atlimpl.cpp>
//--ATL Support------------------------------------------------------------------------------

// You may derive a class from CComModule and use it if you want to
// override something, but do not change the name of _Module.
class CExeModule : public CComModule
{
public:
    LONG Unlock();
    DWORD dwThreadID;
};

CExeModule _Module;

#include <atlcom.h>

LONG CExeModule::Unlock()
{
    LONG l = CComModule::Unlock();
    if (l == 0)
    {
#if _WIN32_WINNT >= 0x0400
        if (CoSuspendClassObjects() == S_OK)
            PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
#else
            PostThreadMessage(dwThreadID, WM_QUIT, 0, 0);
#endif
    }
    return l;
}

#include "ICommandLineImpl.h"
#include "ExternalInclude/BarrysEmacsUUID.c"

BEGIN_OBJECT_MAP(ObjectMap)
OBJECT_ENTRY(CLSID_CommandLine, CCommandLine)
END_OBJECT_MAP()

extern void revoke_rot_registration(void);


unsigned __stdcall CWinemacsApp::atl_thread_mainline( void *parm )
{
    CWinemacsApp *app = (CWinemacsApp *)parm;

    // Init COM in this thread
    CoInitialize( NULL );

    DebugPrintf( "atl_thread_mainline: starting" );

    MSG msg;

    // force msg-q to be created just in case, NOP otherwise
    PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

    // init ATL
    _Module.Init(ObjectMap, AfxGetInstanceHandle());
    _Module.dwThreadID = GetCurrentThreadId();
    HANDLE this_thread = GetCurrentThread();

    if( !app->qual_unregserver )
    {
        DebugPrintf( "Registering Class objects\n" );

        HRESULT hRes = _Module.RegisterClassObjects( CLSCTX_LOCAL_SERVER, REGCLS_SINGLEUSE  );

        DebugPrintf( FormatString( "_Module.RegisterClassObjects => 0x%x\n" ) << hRes );
    }

    //
    // When a server application is launched stand-alone, it is a good idea
    //  to update the system registry in case it has been damaged.
    //
    if( app->qual_unregserver )
    {
        _Module.UpdateRegistryFromResource(IDR_ATLMFC, FALSE);
        _Module.UnregisterServer();
    }
    else
    {
        _Module.UpdateRegistryFromResource(IDR_ATLMFC, TRUE);
        _Module.RegisterServer(TRUE);
    }

    // quit if all the work is done
    if( app->qual_unregserver || app->qual_regserver )
    {
        DebugPrintf( "atl_thread_mainline: regserver exiting" );
        return 0;
    }

    // msg to myself to do work
    //PostThreadMessage(GetCurrentThreadId(), WM_USER+1, 0, 0);

    // msg-pump
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // this was my message -- time to do my work
        if (msg.hwnd == NULL && msg.message == WM_USER+1)
        {
            // do my work here, CCI, work, work, release, etc
            //  if this thread is doing long process, you need to break that into smaller chunks,
            //   and post another user message to process further, that way you don't block the
            //   messages which need to be processed.

            // when done,
            PostQuitMessage(0);
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }


    //
    //    Clean up the world
    //
    revoke_rot_registration();

    _Module.RevokeClassObjects();
    _Module.Term();

    CoUninitialize();

    DebugPrintf( "atl_thread_mainline: normal exiting" );

    return 0;
}
