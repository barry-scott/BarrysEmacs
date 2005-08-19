// winemdoc.h : interface of the CWinemacsDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CWinemacsDoc : public CDocument
{
protected: // create from serialization only
	CWinemacsDoc();
	DECLARE_DYNCREATE(CWinemacsDoc)

// Attributes
public:

// Operations
public:

// Implementation
public:
	virtual ~CWinemacsDoc();
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual	BOOL OnNewDocument();
	virtual BOOL CanCloseFrame( CFrameWnd* pFrame );

// Generated message map functions
protected:
	//{{AFX_MSG(CWinemacsDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
