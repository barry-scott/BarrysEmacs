// finddlg.cpp : implementation file
//

#include <stdafx.h>
#include <winemacs.h>
#include <finddlg.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

unsigned char ui_search_string[128];
unsigned char ui_replace_string[128];

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

CFindDlg::CFindDlg(CWnd* pParent, UINT id)
	: CDialog(id, pParent)
{
	//{{AFX_DATA_INIT(CFindDlg)
	m_search_for = s_str(ui_search_string);
	m_re_search = TRUE;
	m_match_case = bf_mode.md_foldcase == 0;
	//}}AFX_DATA_INIT
}

BOOL CFindDlg::CreateModeless( UINT id )
	{
	return Create( id, NULL );
	}

BOOL CFindDlg::OnInitDialog()
	{      
	CDialog::OnInitDialog();

	SetDefID( IDC_FIND_NEXT );
	GotoDlgCtrl( GetDlgItem( IDC_FIND_TEXT ) );

	return 0;
	}

void CFindDlg::PostNcDestroy(void)
	{
	delete this;
	find_dlg = NULL;
	}
	        
void CFindDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDlg)
	DDX_Text(pDX, IDC_FIND_TEXT, m_search_for);
	DDX_Check(pDX, IDC_FIND_RE_SEARCH, m_re_search);
	DDX_Check(pDX, IDC_FIND_MATCH_CASE, m_match_case);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFindDlg, CDialog)
	//{{AFX_MSG_MAP(CFindDlg)
	ON_BN_CLICKED(IDC_FIND_NEXT, OnClickedFindNext)
	ON_BN_CLICKED(IDC_FIND_PREV, OnClickedFindPrev)
	ON_BN_CLICKED(IDC_FIND_MATCH_CASE, OnClickedFindMatchCase)
	ON_BN_CLICKED(IDC_FIND_RE_SEARCH, OnClickedFindReSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindDlg message handlers

void CFindDlg::OnCancel()
{
	UpdateData();

	const char *p = m_search_for;
	_str_cpy( ui_search_string, p );
	DestroyWindow();
}

void CFindDlg::OnClickedFindNext()
{
	m_search_direction = 1;
	FindString();
}

void CFindDlg::OnClickedFindPrev()
{
	m_search_direction = -1;
	FindString();
}

void CFindDlg::FindString(void)
{
	UpdateData();

	if( bf_cur->b_mark != NULL )
		if( (m_search_direction > 0 && dot < to_mark( bf_cur->b_mark ))
		|| (m_search_direction < 0 && dot > to_mark( bf_cur->b_mark )) )
			exchange_dot_and_mark();
			
	un_set_mark_command();

	if( !m_search_for.IsEmpty() )
		{
		int old_case_fold = bf_mode.md_foldcase;

		bf_mode.md_foldcase = m_match_case == 0;

		const char *p = m_search_for;
		int np = search( u_str(p), m_search_direction, dot, m_re_search );
		bf_mode.md_foldcase = old_case_fold;
		m_string_found = np > 0;
		if( m_string_found )
			{
			region_around_match( 0 );
			}
		else
			{
			un_set_mark_command();
			}
		do_dsp(0);
		}
}

void CFindDlg::OnClickedFindReSearch()
{
	CheckDlgButton( IDC_FIND_RE_SEARCH, !IsDlgButtonChecked( IDC_FIND_RE_SEARCH ) );
}

void CFindDlg::OnClickedFindMatchCase()
{
	CheckDlgButton( IDC_FIND_MATCH_CASE, !IsDlgButtonChecked( IDC_FIND_MATCH_CASE ) );
}

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg dialog

CFindReplaceDlg::CFindReplaceDlg(CWnd* pParent, UINT id)
	: CFindDlg(pParent, id)
{                
	//{{AFX_DATA_INIT(CFindReplaceDlg)
	m_replace_with = s_str(ui_replace_string);
	//}}AFX_DATA_INIT
	m_string_found = FALSE;
}

void CFindReplaceDlg::DoDataExchange(CDataExchange* pDX)
{
	CFindDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindReplaceDlg)
	DDX_Control(pDX, IDC_FIND_REPLACE_ALL, m_replace_all);
	DDX_Control(pDX, IDC_FIND_REPLACE, m_replace);
	DDX_Text(pDX, IDC_FIND_REPLACE_WITH, m_replace_with);
	//}}AFX_DATA_MAP
}

void CFindReplaceDlg::PostNcDestroy(void)
	{
	delete this;
	find_dlg = NULL;
	}
	        

BEGIN_MESSAGE_MAP(CFindReplaceDlg, CFindDlg)
	//{{AFX_MSG_MAP(CFindReplaceDlg)
	ON_BN_CLICKED(IDC_FIND_NEXT, OnClickedFindNext)
	ON_BN_CLICKED(IDC_FIND_PREV, OnClickedFindPrev)
	ON_BN_CLICKED(IDC_FIND_REPLACE, OnClickedFindReplace)
	ON_BN_CLICKED(IDC_FIND_REPLACE_ALL, OnClickedFindReplaceAll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg message handlers

void CFindReplaceDlg::OnCancel()
{
	UpdateData();

	const char *p = m_search_for;
	_str_cpy( ui_search_string, p );

	p = m_replace_with;
	_str_cpy( ui_replace_string, p );

	DestroyWindow();
}

void CFindReplaceDlg::OnClickedFindNext()
{
	CFindDlg::OnClickedFindNext();
	ReplaceButtons();
}

void CFindReplaceDlg::OnClickedFindPrev()
{
	CFindDlg::OnClickedFindPrev();
	ReplaceButtons();
}

void CFindReplaceDlg::ReplaceButtons()
{
	GetDlgItem( IDC_FIND_REPLACE )->EnableWindow(m_string_found);
	GetDlgItem( IDC_FIND_REPLACE_ALL )->EnableWindow(m_string_found);
}

void CFindReplaceDlg::OnClickedFindReplace()
{
	UpdateData();

	un_set_mark_command();

	ReplaceString();
	do_dsp(0);

	/* disable tbe replace buttons till the next search */
	GetDlgItem( IDC_FIND_REPLACE )->EnableWindow(0);
	GetDlgItem( IDC_FIND_REPLACE_ALL )->EnableWindow(0);
}

void CFindReplaceDlg::OnClickedFindReplaceAll()
{
	UpdateData();

	un_set_mark_command();
	
	do
		{
		ReplaceString();
		do_dsp(0);
		FindString();
		do_dsp(0);
		}
	while( m_string_found );

	/* disable tbe replace buttons till the next search */
	GetDlgItem( IDC_FIND_REPLACE )->EnableWindow(0);
	GetDlgItem( IDC_FIND_REPLACE_ALL )->EnableWindow(0);
}

void CFindReplaceDlg::ReplaceString(void)
	{
	set_dot( sea_glob.sea_loc2 );
	const char *p = m_replace_with;
	search_replace_once( u_str(p), m_re_search );
	}
