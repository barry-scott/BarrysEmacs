// switchdl.cpp : implementation file
//

#include "stdafx.h"
#include "winemacs.h"
#include "switchdl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSwitchDlg dialog

CSwitchDlg::CSwitchDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSwitchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSwitchDlg)
	m_buffer_name = "";
	//}}AFX_DATA_INIT
}

BOOL CSwitchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// empty the list
	m_buffer_combo.ResetContent();

	// add all the buffer names
	for( int i=0; i<n_buffers; i++ )
		m_buffer_combo.AddString( (LPCSTR) buf_names[i] );

	// select the current buffer as the default
	m_buffer_combo.SelectString( -1, s_str(bf_cur->b_buf_name) );

	return TRUE;
}

void CSwitchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSwitchDlg)
	DDX_Control(pDX, IDC_SWITCH_BUFFER, m_buffer_combo);
	DDX_CBString(pDX, IDC_SWITCH_BUFFER, m_buffer_name);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSwitchDlg, CDialog)
	//{{AFX_MSG_MAP(CSwitchDlg)
	ON_CBN_DBLCLK(IDC_SWITCH_BUFFER, OnDblclkSwitchBuffer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSwitchDlg message handlers

void CSwitchDlg::OnDblclkSwitchBuffer()
{
	OnOK();
}

void CSwitchDlg::OnOK()
{
	UpdateData();

	if( !m_buffer_name.IsEmpty() )
		{
		set_bfn( u_str( (const char *)m_buffer_name ) );
		tie_win(wn_cur->w_next ? wn_cur : windows, bf_cur);
		}

	EndDialog( IDOK );
}
