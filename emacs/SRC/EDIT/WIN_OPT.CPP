// win_opt.cpp : implementation file
//

#include "stdafx.h"
#include "winemacs.h"
#include "winemdoc.h"
#include "winemvw.h"
#include "win_opt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

class CDirDialog : public CFileDialog
{
public:
	CDirDialog( BOOL bOpen );

protected:
	virtual BOOL OnInitDialog();

};

CDirDialog::CDirDialog( BOOL bOpen ) : CFileDialog( bOpen )
	{
	}
//
//	Lose the file name part of the dialog box
//	to turn this into a directory chooser.
//
BOOL CDirDialog::OnInitDialog()
	{
	BOOL res = CFileDialog::OnInitDialog();

	CWnd *control;

	control = GetDlgItem( 1152 );
//	control->EnableWindow( FALSE );

	control = GetDlgItem( 1120 );	
//	control->EnableWindow( FALSE );

	control = GetDlgItem( 1136 );	
//	control->EnableWindow( FALSE );

	return 1;
	}


/////////////////////////////////////////////////////////////////////////////
// CDirOpt dialog

CDirOpt::CDirOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CDirOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDirOpt)
	m_emacs_journal = "";
	m_emacs_library = "";
	m_emacs_local_library = "";
	m_emacs_user_library = "";
	m_sys_login = "";
	m_sys_scratch = "";
	m_emacs_path = "";
	m_emacs_checkpoint = "";
	m_emacs_memory = _T("");
	//}}AFX_DATA_INIT

	m_sys_scratch = get_device_name_translation( "sys$scratch" );
	if( m_sys_scratch == "" )
		m_sys_scratch = s_str(get_tmp_path());
	m_emacs_journal = get_device_name_translation( "emacs$journal" );
	if( m_emacs_journal == "" )
		m_emacs_journal = m_sys_scratch;
	m_emacs_checkpoint = get_device_name_translation( "emacs$checkpoint" );
	if( m_emacs_checkpoint == "" )
		m_emacs_checkpoint = ".\\";
	m_emacs_library = get_device_name_translation( "emacs$library" );
	if( m_emacs_library == "" )
		{
		m_emacs_library = AfxGetApp()->m_pszHelpFilePath;
		m_emacs_library = m_emacs_library.Left( 
				m_emacs_library.ReverseFind( '\\' ) )
				 	+ "\\Library";
		}
	m_emacs_local_library = get_device_name_translation( "emacs$local_library" );
	m_emacs_user_library = get_device_name_translation( "emacs$user" );
	if( m_emacs_user_library == "" )
		{
		m_emacs_user_library = AfxGetApp()->m_pszHelpFilePath;
		m_emacs_user_library = m_emacs_user_library.Left( 
				m_emacs_user_library.ReverseFind( '\\' ) )
				 	+ "\\UserLib";
		}
	m_sys_login = get_device_name_translation( "sys$login" );
	m_emacs_memory = get_config_env( u_str("emacs_memory") );
	set_emacs_path();
}

void CDirOpt::save_options()
{
	AfxGetApp()->WriteProfileString( "Environment", "Emacs_Path", m_emacs_path );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Sys$Login", m_sys_login );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Sys$Scratch", m_sys_scratch );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Emacs$Checkpoint", m_emacs_checkpoint );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Emacs$Journal", m_emacs_journal );
	AfxGetApp()->WriteProfileString( "Environment", "Emacs_Memory", m_emacs_memory );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Emacs$User", m_emacs_user_library );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Emacs$Library", m_emacs_library );
	AfxGetApp()->WriteProfileString( "DeviceNames", "Emacs$Local_Library", m_emacs_local_library );
}

void CDirOpt::set_emacs_path()
{
	m_emacs_path = ".";
	if( m_emacs_user_library != "" )
		m_emacs_path += ";emacs$user:";
	if( m_emacs_local_library != "" )
		m_emacs_path += ";emacs$local_library:";
	if( m_emacs_library != "" )
		m_emacs_path += ";emacs$library:";
}

void CDirOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	if( !pDX->m_bSaveAndValidate )
		set_emacs_path();

	//{{AFX_DATA_MAP(CDirOpt)
	DDX_Text(pDX, IDC_OPTDIR_EMACS_JOURNAL, m_emacs_journal);
	DDV_MaxChars(pDX, m_emacs_journal, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_LIB, m_emacs_library);
	DDV_MaxChars(pDX, m_emacs_library, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_LOCAL_LIB, m_emacs_local_library);
	DDV_MaxChars(pDX, m_emacs_local_library, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_USER, m_emacs_user_library);
	DDV_MaxChars(pDX, m_emacs_user_library, 63);
	DDX_Text(pDX, IDC_OPTDIR_SYS_LOGIN, m_sys_login);
	DDV_MaxChars(pDX, m_sys_login, 63);
	DDX_Text(pDX, IDC_OPTDIR_SYS_SCRATCH, m_sys_scratch);
	DDV_MaxChars(pDX, m_sys_scratch, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_PATH, m_emacs_path);
	DDV_MaxChars(pDX, m_emacs_path, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_CHECKPOINT, m_emacs_checkpoint);
	DDV_MaxChars(pDX, m_emacs_checkpoint, 63);
	DDX_Text(pDX, IDC_OPTDIR_EMACS_MEMORY, m_emacs_memory);
	DDV_MaxChars(pDX, m_emacs_memory, 63);
	//}}AFX_DATA_MAP

	if( pDX->m_bSaveAndValidate )
		set_emacs_path();
}

BEGIN_MESSAGE_MAP(CDirOpt, CDialog)
	//{{AFX_MSG_MAP(CDirOpt)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE, OnClickedOptdirBrowse1)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE2, OnClickedOptdirBrowse2)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE3, OnClickedOptdirBrowse3)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE4, OnClickedOptdirBrowse4)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE5, OnClickedOptdirBrowse5)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE6, OnClickedOptdirBrowse6)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE7, OnClickedOptdirBrowse7)
	ON_BN_CLICKED(IDC_OPTDIR_BROWSE8, OnClickedOptdirBrowse8)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDirOpt message handlers

void CDirOpt::OnClickedOptdirBrowse( CString &str )
{                                  
	if( !UpdateData( TRUE ) )
		return;

	char file_name[64];
	if( str == "" )
		strcpy( file_name, "." );
	else
		strcpy( file_name, str );

	int len = strlen( file_name );
	if( len > 0
	&& file_name[len-1] == '\\' )
		file_name[len-1] = '\0';

	CDirDialog get_dir( TRUE );
	
	strcpy( get_dir.m_ofn.lpstrFile, "pick.one" );
	get_dir.m_ofn.Flags |= OFN_HIDEREADONLY|OFN_NOCHANGEDIR|OFN_PATHMUSTEXIST;
	                                        
	get_dir.m_ofn.lpstrTitle = "Select Directory";
	get_dir.m_ofn.lpstrInitialDir = file_name;
	                                      
	if( get_dir.DoModal() == IDOK )
		{
		CString newstr = get_dir.GetPathName();

		str = newstr.Left( newstr.ReverseFind( '\\' )+1 );

		UpdateData( FALSE );
		}
}

void CDirOpt::OnClickedOptdirBrowse1()
	{
	OnClickedOptdirBrowse( m_sys_login );
	}

void CDirOpt::OnClickedOptdirBrowse2()
	{
	OnClickedOptdirBrowse( m_sys_scratch );
	}

void CDirOpt::OnClickedOptdirBrowse3()
	{
	OnClickedOptdirBrowse( m_emacs_user_library );
	}

void CDirOpt::OnClickedOptdirBrowse4()
	{
	OnClickedOptdirBrowse( m_emacs_local_library );
	}

void CDirOpt::OnClickedOptdirBrowse5()
	{
	OnClickedOptdirBrowse( m_emacs_library );
	}

void CDirOpt::OnClickedOptdirBrowse6()
	{
	OnClickedOptdirBrowse( m_emacs_journal );
	}

void CDirOpt::OnClickedOptdirBrowse7()
	{
	OnClickedOptdirBrowse( m_emacs_checkpoint );
	}

void CDirOpt::OnClickedOptdirBrowse8()
	{
	OnClickedOptdirBrowse( m_emacs_memory );
	}

/////////////////////////////////////////////////////////////////////////////
// CColourOpt dialog

CColourOpt::CColourOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CColourOpt::IDD, pParent)
	, cur_item(-1)
{
	//{{AFX_DATA_INIT(CColourOpt)
	m_italic = FALSE;
	m_underline = FALSE;
	//}}AFX_DATA_INIT
	colours[COLOR_ITEM_MODE_LINE] = mode_line_rendition;
	colours[COLOR_ITEM_NORMAL_TEXT] = window_rendition;
	colours[COLOR_ITEM_HIGHLIGHT_TEXT] = region_rendition;
	colours[COLOR_ITEM_SYNTAX_KEYWORD1] = syntax_keyword1_rendition;
	colours[COLOR_ITEM_SYNTAX_KEYWORD2] = syntax_keyword2_rendition;
	colours[COLOR_ITEM_SYNTAX_WORD] = syntax_word_rendition;
	colours[COLOR_ITEM_SYNTAX_STRING] = syntax_string_rendition;
	colours[COLOR_ITEM_SYNTAX_QUOTE] = syntax_quote_rendition;
	colours[COLOR_ITEM_SYNTAX_COMMENT] = syntax_comment_rendition;
	colours[COLOR_ITEM_USER_1] = user_1_rendition;
	colours[COLOR_ITEM_USER_2] = user_2_rendition;
	colours[COLOR_ITEM_USER_3] = user_3_rendition;
	colours[COLOR_ITEM_USER_4] = user_4_rendition;
	colours[COLOR_ITEM_USER_5] = user_5_rendition;
	colours[COLOR_ITEM_USER_6] = user_6_rendition;
	colours[COLOR_ITEM_USER_7] = user_7_rendition;
	colours[COLOR_ITEM_USER_8] = user_8_rendition;
}

void CColourOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CColourOpt)
	DDX_Control(pDX, IDC_OPTCOL_SAMPLE, m_sample_text);
	DDX_Control(pDX, IDC_OPTCOL_ITEMLIST, m_item_list);
	DDX_Check(pDX, IDC_OPTCOL_ITALIC, m_italic);
	DDX_Check(pDX, IDC_OPTCOL_UNDERLINE, m_underline);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CColourOpt, CDialog)
	//{{AFX_MSG_MAP(CColourOpt)
	ON_BN_CLICKED(IDC_OPTCOL_BACKGROUND, OnClickedOptcolBackground)
	ON_BN_CLICKED(IDC_OPTCOL_FOREGROUND, OnClickedOptcolForeground)
	ON_LBN_SELCHANGE(IDC_OPTCOL_ITEMLIST, OnSelchangeOptcolItemlist)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColourOpt::save_options()
{
	colours[COLOR_ITEM_MODE_LINE].GetColor( mode_line_rendition );
	colours[COLOR_ITEM_NORMAL_TEXT].GetColor( window_rendition );
	colours[COLOR_ITEM_HIGHLIGHT_TEXT].GetColor( region_rendition );
	colours[COLOR_ITEM_SYNTAX_KEYWORD1].GetColor( syntax_keyword1_rendition );
	colours[COLOR_ITEM_SYNTAX_KEYWORD2].GetColor( syntax_keyword2_rendition );
	colours[COLOR_ITEM_SYNTAX_WORD].GetColor( syntax_word_rendition );
	colours[COLOR_ITEM_SYNTAX_STRING].GetColor( syntax_string_rendition );
	colours[COLOR_ITEM_SYNTAX_QUOTE].GetColor( syntax_quote_rendition );
	colours[COLOR_ITEM_SYNTAX_COMMENT].GetColor( syntax_comment_rendition );
	colours[COLOR_ITEM_USER_1].GetColor( user_1_rendition );
	colours[COLOR_ITEM_USER_2].GetColor( user_2_rendition );
	colours[COLOR_ITEM_USER_3].GetColor( user_3_rendition );
	colours[COLOR_ITEM_USER_4].GetColor( user_4_rendition );
	colours[COLOR_ITEM_USER_5].GetColor( user_5_rendition );
	colours[COLOR_ITEM_USER_6].GetColor( user_6_rendition );
	colours[COLOR_ITEM_USER_7].GetColor( user_7_rendition );
	colours[COLOR_ITEM_USER_8].GetColor( user_8_rendition );

	AfxGetApp()->WriteProfileString( "Colours", "WindowText", s_str(window_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "HighlightText", s_str(region_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "ModeLine", s_str(mode_line_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_keyword1", s_str(syntax_keyword1_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_keyword2", s_str(syntax_keyword2_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_word", s_str(syntax_word_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_string", s_str(syntax_string_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_quote", s_str(syntax_quote_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "Syntax_comment", s_str(syntax_comment_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_1", s_str(user_1_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_2", s_str(user_2_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_3", s_str(user_3_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_4", s_str(user_4_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_5", s_str(user_5_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_6", s_str(user_6_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_7", s_str(user_7_rendition) );
	AfxGetApp()->WriteProfileString( "Colours", "USER_8", s_str(user_8_rendition) );
}
/////////////////////////////////////////////////////////////////////////////
// CColourOpt message handlers

HBRUSH CColourOpt::OnCtlColor( CDC *pDC, CWnd *pWnd, UINT nCtlColor )
	{
	// if an item is selected
	if( cur_item >= 0
	// and this is a color change for a static control
	&& nCtlColor == CTLCOLOR_STATIC
	// and its the sample text control
	&& pWnd->GetDlgCtrlID() == IDC_OPTCOL_SAMPLE )
		{
		pDC->SetTextColor( colours[cur_item].foreground );
		pDC->SetBkColor( colours[cur_item].background );
		
		return ::CreateSolidBrush( colours[cur_item].background );
		}

	return CDialog::OnCtlColor( pDC, pWnd, nCtlColor );
	}

BOOL CColourOpt::OnInitDialog()
{
	CDialog::OnInitDialog();

	// empty the list
	m_item_list.ResetContent();

	// add all the item names
	int index = m_item_list.AddString( "Mode line" );
	m_item_list.SetItemData( index, COLOR_ITEM_MODE_LINE );
	index = m_item_list.AddString( "Normal text" );
	m_item_list.SetItemData( index, COLOR_ITEM_NORMAL_TEXT );
	index = m_item_list.AddString( "Highlight text" );
	m_item_list.SetItemData( index, COLOR_ITEM_HIGHLIGHT_TEXT );
	index = m_item_list.AddString( "Syntax keyword 1" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_KEYWORD1 );
	index = m_item_list.AddString( "Syntax keyword 2" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_KEYWORD2 );
	index = m_item_list.AddString( "Syntax word" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_WORD );
	index = m_item_list.AddString( "Syntax string" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_STRING );
	index = m_item_list.AddString( "Syntax quote" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_QUOTE );
	index = m_item_list.AddString( "Syntax comment" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_COMMENT );
	index = m_item_list.AddString( "User 1" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_1 );
	index = m_item_list.AddString( "User 2" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_2 );
	index = m_item_list.AddString( "User 3" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_3 );
	index = m_item_list.AddString( "User 4" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_4 );
	index = m_item_list.AddString( "User 5" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_5 );
	index = m_item_list.AddString( "User 6" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_6 );
	index = m_item_list.AddString( "User 7" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_7 );
	index = m_item_list.AddString( "User 8" );
	m_item_list.SetItemData( index, COLOR_ITEM_USER_8 );

	// nothing selected
	cur_item = -1;
	m_item_list.SetCurSel( cur_item );

	//
	//	THese controls are turned back on when a selection is made
	//
	CWnd *control = GetDlgItem( IDC_OPTCOL_FOREGROUND );
	control->EnableWindow( FALSE );
	control = GetDlgItem( IDC_OPTCOL_BACKGROUND );
	control->EnableWindow( FALSE );
	control = GetDlgItem( IDC_OPTCOL_ITALIC );
	control->EnableWindow( FALSE );
	control = GetDlgItem( IDC_OPTCOL_UNDERLINE );
	control->EnableWindow( FALSE );


	return TRUE;
}

void CColourOpt::OnOK()
{
	if( cur_item >= 0 )
		{
		// get the value of the controls
		UpdateData( TRUE );
		colours[cur_item].underline = m_underline;
		colours[cur_item].italic = m_italic;
		}
	CDialog::OnOK();
}

void CColourOpt::OnSelchangeOptcolItemlist()
{
	if( cur_item >= 0 )
		{
		// get the value of the controls
		UpdateData( TRUE );
		colours[cur_item].underline = m_underline;
		colours[cur_item].italic = m_italic;
		}
	else
		{
		CWnd *control = GetDlgItem( IDC_OPTCOL_FOREGROUND );
		control->EnableWindow( TRUE );
		control = GetDlgItem( IDC_OPTCOL_BACKGROUND );
		control->EnableWindow( TRUE );
		control = GetDlgItem( IDC_OPTCOL_ITALIC );
		control->EnableWindow( TRUE );
		control = GetDlgItem( IDC_OPTCOL_UNDERLINE );
		control->EnableWindow( TRUE );
		}

	cur_item = (int)m_item_list.GetItemData( m_item_list.GetCurSel() ); 
	m_sample_text.Invalidate();
	m_italic = colours[cur_item].italic;
	m_underline = colours[cur_item].underline;
	// update the controls
	UpdateData( FALSE );
}

void CColourOpt::OnClickedOptcolBackground()
{
	if( cur_item >= 0 )
		{
		CColorDialog bg_color( colours[cur_item].background );

		if( bg_color.DoModal() == IDOK )
			colours[cur_item].background = bg_color.GetColor();

		m_sample_text.Invalidate();
		}
}

void CColourOpt::OnClickedOptcolForeground()
{
	if( cur_item >= 0 )
		{
		CColorDialog fg_color( colours[cur_item].foreground );

		if( fg_color.DoModal() == IDOK )
			colours[cur_item].foreground = fg_color.GetColor();

		m_sample_text.Invalidate();
		}
}


void CDirOpt::OnOK()
{
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
// CPrintOpt dialog

CPrintOpt::CPrintOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CPrintOpt::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPrintOpt)
	m_header_format = "";
	m_font_name = "";
	m_font_size = 0;
	m_footer_format = "";
	m_bottom_margin = float(0.0);
	m_left_margin = float(0.0);
	m_right_margin = float(0.0);
	m_top_margin = float(0.0);
	//}}AFX_DATA_INIT

	char measure_string[80];
	long measure_length= sizeof( measure_string );

	int measure;
	if( RegQueryValue
		(
		HKEY_CURRENT_USER,
		"Control Panel\\International\\iMeasure",
		&measure_string[0],
		&measure_length
		) == ERROR_SUCCESS )
			measure = atoi( measure_string );
	else
		measure = 0;

	twips_scaling = measure == 0 ? 1440./25.4 : 1440.;

	load_options();
}

void CPrintOpt::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintOpt)
	DDX_Text(pDX, IDC_PRINT_HEADER_FORMAT, m_header_format);
	DDX_Text(pDX, IDC_PRINTER_FONT_NAME, m_font_name);
	DDX_Text(pDX, IDC_PRINTER_FONT_SIZE, m_font_size);
	DDV_MinMaxInt(pDX, m_font_size, 6, 128);
	DDX_Text(pDX, IDC_PRINTER_FOOTER_FORMAT, m_footer_format);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_BOTTOM, m_bottom_margin);
	DDV_MinMaxFloat(pDX, m_bottom_margin, float(0.), float(999.));
	DDX_Text(pDX, IDC_PRINTER_MARGIN_LEFT, m_left_margin);
	DDV_MinMaxFloat(pDX, m_left_margin, float(0.), float(999.));
	DDX_Text(pDX, IDC_PRINTER_MARGIN_RIGHT, m_right_margin);
	DDV_MinMaxFloat(pDX, m_right_margin, float(0.), float(999.));
	DDX_Text(pDX, IDC_PRINTER_MARGIN_TOP, m_top_margin);
	DDV_MinMaxFloat(pDX, m_top_margin, float(0.), float(999.));
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPrintOpt, CDialog)
	//{{AFX_MSG_MAP(CPrintOpt)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintOpt message handlers

void CPrintOpt::OnOK()
{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
}

void CPrintOpt::load_options()
{
	m_top_margin = (float)(AfxGetApp()->GetProfileInt( "Print", "margin_top", 1440 )/twips_scaling);
	m_bottom_margin = (float)(AfxGetApp()->GetProfileInt( "Print", "margin_bottom", 1440 )/twips_scaling);
	m_left_margin = (float)(AfxGetApp()->GetProfileInt( "Print", "margin_left", 1440 )/twips_scaling);
	m_right_margin = (float)(AfxGetApp()->GetProfileInt( "Print", "margin_right", 1440 )/twips_scaling);

	// read in twips and convert to Points
	m_font_size = AfxGetApp()->GetProfileInt( "Print", "font_size", 9*20 )/20;
	m_font_name = AfxGetApp()->GetProfileString( "Print", "font_name", "Courier New" );

	m_header_format = AfxGetApp()->GetProfileString( "Print", "header_format", "File: %f" );
	m_footer_format = AfxGetApp()->GetProfileString( "Print", "footer_format", "Page %p" );
}

void CPrintOpt::save_options()
{
	AfxGetApp()->WriteProfileInt( "Print", "margin_top", (int)(m_top_margin*twips_scaling) );
	AfxGetApp()->WriteProfileInt( "Print", "margin_bottom", (int)(m_bottom_margin*twips_scaling) );
	AfxGetApp()->WriteProfileInt( "Print", "margin_left", (int)(m_left_margin*twips_scaling) );
	AfxGetApp()->WriteProfileInt( "Print", "margin_right", (int)(m_right_margin*twips_scaling) );

	// Save font size in twips
	AfxGetApp()->WriteProfileInt( "Print", "font_size", m_font_size*20 );
	AfxGetApp()->WriteProfileString( "Print", "font_name", m_font_name );

	AfxGetApp()->WriteProfileString( "Print", "header_format", m_header_format );
	AfxGetApp()->WriteProfileString( "Print", "footer_format", m_footer_format );
}
/////////////////////////////////////////////////////////////////////////////
// COptionFilenameParsing dialog


COptionFilenameParsing::COptionFilenameParsing(CWnd* pParent /*=NULL*/)
	: CDialog(COptionFilenameParsing::IDD, pParent)
{
	char disk[2];
	disk[0] = 0;
	disk[1] = 0;

	for( int i=0; i<26; i++ )
		{
		disk[0] = char('A' + i);
		m_disk[i] = AfxGetApp()->GetProfileInt( "FileParsing", disk, 0 );
		}

	//{{AFX_DATA_INIT(COptionFilenameParsing)
	//}}AFX_DATA_INIT
}

void COptionFilenameParsing::save_options(void)
{
	char disk[2];
	disk[0] = 0;
	disk[1] = 0;

	for( int i=0; i<26; i++ )
		{
		disk[0] = char('A' + i);
		AfxGetApp()->WriteProfileInt( "FileParsing", disk, m_disk[i] );
		}
}


void COptionFilenameParsing::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_A, m_disk[0]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_B, m_disk[1]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_C, m_disk[2]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_D, m_disk[3]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_E, m_disk[4]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_F, m_disk[5]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_G, m_disk[6]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_H, m_disk[7]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_I, m_disk[8]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_J, m_disk[9]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_K, m_disk[10]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_L, m_disk[11]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_M, m_disk[12]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_N, m_disk[13]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_O, m_disk[14]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_P, m_disk[15]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_Q, m_disk[16]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_R, m_disk[17]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_S, m_disk[18]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_T, m_disk[19]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_U, m_disk[20]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_V, m_disk[21]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_W, m_disk[22]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_X, m_disk[23]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_Y, m_disk[24]);
	DDX_CBIndex(pDX, IDC_FILE_PARSE_DISK_Z, m_disk[25]);
//{{AFX_DATA_MAP(COptionFilenameParsing)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionFilenameParsing, CDialog)
	//{{AFX_MSG_MAP(COptionFilenameParsing)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionFilenameParsing message handlers
