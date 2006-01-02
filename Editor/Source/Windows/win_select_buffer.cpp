// win_select_buffer.cpp : implementation file
//

#include <emacs.h>

#include "win_emacs.h"
#include "win_select_buffer.h"


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

/////////////////////////////////////////////////////////////////////////////
// CBufferSelectDlg dialog

CBufferSelectDlg::CBufferSelectDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CBufferSelectDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CBufferSelectDlg)
    m_buffer_name = "";
    //}}AFX_DATA_INIT
    buffer_types_mask = 1<<FILEBUFFER;
}

void CBufferSelectDlg::InitParameters( const EmacsString &title, const EmacsString &buffer_name, const EmacsString &buffer_types )
{
    dialog_title = title;
    m_buffer_name = CString( buffer_name.sdata() );

    buffer_types_mask = 0;

    if( buffer_types.index( "file" ) >= 0 )
        buffer_types_mask |= 1<<FILEBUFFER;
    if( buffer_types.index( "scratch" ) >= 0 )
        buffer_types_mask |= 1<<SCRATCHBUFFER;
    if( buffer_types.index( "macro" ) >= 0 )
        buffer_types_mask |= 1<<MACROBUFFER;
}



BOOL CBufferSelectDlg::OnInitDialog()
{
    SetWindowText( dialog_title );
    CDialog::OnInitDialog();

    // empty the list
    m_buffer_combo.ResetContent();

    // add all the buffer names
    for( int index = 0; index<EmacsBuffer::name_table.entries(); index++ )
    {
        EmacsBuffer *b = EmacsBuffer::name_table.value( index );
        if( (buffer_types_mask & (1<<b->b_kind)) != 0 )
            m_buffer_combo.AddString( (LPCSTR)(b->b_buf_name.sdata()) );
    }

    // select the current buffer as the default
    m_buffer_combo.SelectString( -1, m_buffer_name );

    UpdateData( FALSE );

    return TRUE;
}

void CBufferSelectDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CBufferSelectDlg)
    DDX_Control(pDX, IDC_SWITCH_BUFFER, m_buffer_combo);
    DDX_CBString(pDX, IDC_SWITCH_BUFFER, m_buffer_name);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CBufferSelectDlg, CDialog)
    //{{AFX_MSG_MAP(CBufferSelectDlg)
    ON_CBN_DBLCLK(IDC_SWITCH_BUFFER, OnDblclkSwitchBuffer)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBufferSelectDlg message handlers

void CBufferSelectDlg::OnDblclkSwitchBuffer()
{
    OnOK();
}

void CBufferSelectDlg::OnOK()
{
    UpdateData();

    EndDialog( IDOK );
}
