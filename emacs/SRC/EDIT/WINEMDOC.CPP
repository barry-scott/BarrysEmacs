// winemdoc.cpp : implementation of the CWinemacsDoc class
//

#include "stdafx.h"
#include "winemacs.h"

#include "winemdoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinemacsDoc

IMPLEMENT_DYNCREATE(CWinemacsDoc, CDocument)

BEGIN_MESSAGE_MAP(CWinemacsDoc, CDocument)
	//{{AFX_MSG_MAP(CWinemacsDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinemacsDoc construction/destruction

CWinemacsDoc::CWinemacsDoc()
{
	// TODO: add one-time construction code here
}

CWinemacsDoc::~CWinemacsDoc()
{
}

BOOL CWinemacsDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsDoc serialization

void CWinemacsDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


/////////////////////////////////////////////////////////////////////////////
// CWinemacsDoc diagnostics

#ifdef _DEBUG
void CWinemacsDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWinemacsDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinemacsDoc commands
BOOL CWinemacsDoc::CanCloseFrame( CFrameWnd* pFrame )
	{
	// count the number of views on the document
	int num_views = 0;
	POSITION pos = GetFirstViewPosition();
	while( pos != NULL )
		{
		CView* pView = GetNextView( pos );

		num_views++;
		}   

	// can close one if there are more then 1 views
	return num_views > 1;
	}


