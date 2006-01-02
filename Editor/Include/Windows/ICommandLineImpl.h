// ICommandLineImpl.h: Definition of the CCommandLine class
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICOMMANDLINEIMPL_H__AD1696A2_C999_11D3_9BA6_00500445EFA8__INCLUDED_)
#define AFX_ICOMMANDLINEIMPL_H__AD1696A2_C999_11D3_9BA6_00500445EFA8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"       // main symbols
#include "ExternalInclude/BarrysEmacs.h"

/////////////////////////////////////////////////////////////////////////////
// CCommandLine

class CCommandLine :
    public IDispatchImpl<ICommandLine, &IID_ICommandLine, &LIBID_winemacs>,
    public ISupportErrorInfo,
    public CComObjectRoot,
    public CComCoClass<CCommandLine,&CLSID_CommandLine>
{
public:
    CCommandLine() {}
BEGIN_COM_MAP(CCommandLine)
    COM_INTERFACE_ENTRY(IDispatch)
    COM_INTERFACE_ENTRY(ICommandLine)
    COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CCommandLine)

DECLARE_REGISTRY(CCommandLine, _T("BarryScottEmacs.CommandLine.1"), _T("BarryScottEmacs.CommandLine"), IDS_COMMANDLINE_DESC, THREADFLAGS_BOTH)
// ISupportsErrorInfo
    STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// ICommandLine
public:
    STDMETHOD(GetProcessId)(long *id);
    STDMETHOD(SetProcessName)(BSTR process_name);
    STDMETHOD(CommandLine)(BSTR current_working_directory,  BSTR command_line);

};

#endif // !defined(AFX_ICOMMANDLINEIMPL_H__AD1696A2_C999_11D3_9BA6_00500445EFA8__INCLUDED_)
