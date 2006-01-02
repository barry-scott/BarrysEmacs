// win_find.cpp : implementation file
//

#include <emacs.h>
#include <win_emacs.h>
#include <win_find.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

SystemExpressionRepresentationString ui_search_string;
SystemExpressionRepresentationString ui_replace_string;

/////////////////////////////////////////////////////////////////////////////
// CFindDlg dialog

CFindDlg::CFindDlg(CWnd* pParent, UINT id)
    : CDialog(id, pParent)
{
    //{{AFX_DATA_INIT(CFindDlg)
    m_search_for = s_str( ui_search_string.data() );
    m_re_search = TRUE;
    m_match_case = bf_cur->b_mode.md_foldcase == 0;
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
    ui_search_string = p;
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

    if( bf_cur->b_mark.isSet() )
        if( (m_search_direction > 0 && dot < bf_cur->b_mark.to_mark())
        ||  (m_search_direction < 0 && dot > bf_cur->b_mark.to_mark()) )
            exchange_dot_and_mark();

    bf_cur->set_mark( dot, 0, false );

    if( !m_search_for.IsEmpty() )
    {
        int old_case_fold = bf_cur->b_mode.md_foldcase;

        bf_cur->b_mode.md_foldcase = m_match_case == 0;

        EmacsString p( m_search_for );
        int np = sea_glob.search( p, m_search_direction, dot, m_re_search ? EmacsSearch::sea_type__RE_extended : EmacsSearch::sea_type__string );
        bf_cur->b_mode.md_foldcase = old_case_fold;
        m_string_found = np > 0;
        if( m_string_found )
        {
            region_around_match( 0 );
        }
        else
        {
            bf_cur->set_mark( dot, 0, false );
        }
        theActiveView->do_dsp();
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
    m_replace_with = s_str(ui_replace_string.data());
    m_replace_case = replace_case;
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
    DDX_Check(pDX, IDC_FIND_REPLACE_CASE, m_replace_case);
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
    ON_BN_CLICKED(IDC_FIND_REPLACE_FIND, OnClickedFindReplaceFind)
    ON_BN_CLICKED(IDC_FIND_REPLACE_CASE, OnClickedFindReplaceCase)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindReplaceDlg message handlers

void CFindReplaceDlg::OnCancel()
{
    UpdateData();

    const char *p = m_search_for;
    ui_search_string = p;

    p = m_replace_with;
    ui_replace_string = p;

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
    GetDlgItem( IDC_FIND_REPLACE_FIND )->EnableWindow(m_string_found);

    if( !m_string_found )
        if( m_search_direction )
            GotoDlgCtrl( GetDlgItem( IDC_FIND_NEXT ) );
        else
            GotoDlgCtrl( GetDlgItem( IDC_FIND_PREV ) );
}

void CFindReplaceDlg::OnClickedFindReplace()
{
    UpdateData();

    bf_cur->set_mark( dot, 0, false );

    ReplaceString();
    theActiveView->do_dsp();

    m_string_found = false;

    // disable tbe replace buttons till the next search
    ReplaceButtons();
}

void CFindReplaceDlg::OnClickedFindReplaceAll()
{
    UpdateData();

    bf_cur->set_mark( dot, 0, false );

    do
    {
        ReplaceString();
        theActiveView->do_dsp();
        FindString();
        theActiveView->do_dsp();
    }
    while( m_string_found );

    // disable tbe replace buttons till the next search
    ReplaceButtons();
}

void CFindReplaceDlg::ReplaceString(void)
{
    if( sea_glob.get_number_of_groups() < 0 )
        return;

    int old_replace_case = replace_case;

    replace_case = m_replace_case != 0;

    set_dot( sea_glob.get_end_of_group( 0 ) );
    EmacsString p( m_replace_with );
    sea_glob.search_replace_once( p );

    replace_case = old_replace_case;
}

void CFindReplaceDlg::OnClickedFindReplaceFind()
{
    UpdateData();

    bf_cur->set_mark( dot, 0, false );

    ReplaceString();
    FindString();

    // disable tbe replace buttons till the next search
    ReplaceButtons();
}

void CFindReplaceDlg::OnClickedFindReplaceCase()
{
    CheckDlgButton( IDC_FIND_REPLACE_CASE, !IsDlgButtonChecked( IDC_FIND_REPLACE_CASE ) );
}
