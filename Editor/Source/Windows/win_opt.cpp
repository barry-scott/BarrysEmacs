//
// win_opt.cpp : implementation file
//

#include <emacs.h>
#include <win_emacs.h>
#include <win_registry.h>
#include <win_main_frame.h>
#include <win_doc.h>
#include <win_view.h>
#include <win_opt.h>
#include <win_toolbar.h>
#include <math.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

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

	return res;
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

	m_sys_scratch = get_device_name_translation( "sys$scratch" ).sdata();
	if( m_sys_scratch == "" )
		m_sys_scratch = s_str(get_tmp_path());
	m_emacs_journal = get_device_name_translation( "emacs$journal" ).sdata();
	if( m_emacs_journal == "" )
		m_emacs_journal = m_sys_scratch;
	m_emacs_checkpoint = get_device_name_translation( "emacs$checkpoint" ).sdata();
	if( m_emacs_checkpoint == "" )
		m_emacs_checkpoint = ".\\";
	m_emacs_library = get_device_name_translation( "emacs$library" ).sdata();
	if( m_emacs_library == "" )
		{
		m_emacs_library = theApp.m_pszHelpFilePath;
		m_emacs_library = m_emacs_library.Left( 
				m_emacs_library.ReverseFind( '\\' ) )
				 	+ "\\Library";
		}
	m_emacs_local_library = get_device_name_translation( "emacs$local_library" ).sdata();
	m_emacs_user_library = get_device_name_translation( "emacs$user" ).sdata();
	m_sys_login = get_device_name_translation( "sys$login" ).sdata();
	if( m_sys_login == "" )
		{
		EmacsString home( get_config_env("HOMEDRIVE") );
		home.append( get_config_env("HOMEPATH") );

		m_sys_login = home.data();
		}
	if( m_emacs_user_library == "" )
		{
		if( m_sys_login != "" )		
			{
			m_emacs_user_library = m_sys_login + "\\bemacs";
			}
		else
			{
			m_emacs_user_library = theApp.m_pszHelpFilePath;
			m_emacs_user_library = m_emacs_user_library.Left( 
					m_emacs_user_library.ReverseFind( '\\' ) )
						+ "\\UserLib";
			}
			
		}
	m_emacs_memory = get_config_env( u_str("emacs_memory") ).sdata();
	set_emacs_path();
	}

void CDirOpt::save_options()
	{
	theApp.WriteProfileString( "Environment", "Emacs_Path", m_emacs_path );
	theApp.WriteProfileString( "DeviceNames", "Sys$Login", m_sys_login );
	theApp.WriteProfileString( "DeviceNames", "Sys$Scratch", m_sys_scratch );
	theApp.WriteProfileString( "DeviceNames", "Emacs$Checkpoint", m_emacs_checkpoint );
	theApp.WriteProfileString( "DeviceNames", "Emacs$Journal", m_emacs_journal );
	theApp.WriteProfileString( "Environment", "Emacs_Memory", m_emacs_memory );
	theApp.WriteProfileString( "DeviceNames", "Emacs$User", m_emacs_user_library );
	theApp.WriteProfileString( "DeviceNames", "Emacs$Library", m_emacs_library );
	theApp.WriteProfileString( "DeviceNames", "Emacs$Local_Library", m_emacs_local_library );
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
//
void CColorComboBox::AddColorItem(COLORREF color)
	{
	// add a listbox item
	AddString((LPCTSTR) color);
		// Listbox does not have the LBS_HASSTRINGS style, so the
		//  normal listbox string is used to store an RGB color
	}

bool CColorComboBox::SetCurSelToColour(COLORREF colour)
	{
	for( int i=0; i<GetCount(); i++ )
		{
		if( colour == GetItemData( i ) )
			{
			SetCurSel( i );
			return true;
			}
		}

	return false;
	}

/////////////////////////////////////////////////////////////////////////////

const int COLOR_ITEM_HEIGHT( 20 );

void CColorComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
	{
	// all items are of fixed size
	// must use LBS_OWNERDRAWVARIABLE for this to work
	lpMIS->itemHeight = COLOR_ITEM_HEIGHT;
	}

void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
	{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	COLORREF cr = (COLORREF)lpDIS->itemData; // RGB in item data

	CRect rect( lpDIS->rcItem );
	rect.DeflateRect( 2, 2 );

	if (lpDIS->itemAction & ODA_DRAWENTIRE)
		{
		// Paint the color item in the color requested
		CBrush br(cr);
		pDC->FillRect(rect, &br);
		}

	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		{
		// item has been selected - hilite frame
		COLORREF crHilite = RGB(255-GetRValue(cr), 255-GetGValue(cr), 255-GetBValue(cr));
		CBrush br(crHilite);
		pDC->FrameRect(rect, &br);
		}

	if (!(lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & ODA_SELECT))
		{
		// Item has been de-selected -- remove frame
		CBrush br(cr);
		pDC->FrameRect(rect, &br);
		}
	}

int CColorComboBox::CompareItem(LPCOMPAREITEMSTRUCT lpCIS)
	{
	COLORREF cr1 = (COLORREF)lpCIS->itemData1;
	COLORREF cr2 = (COLORREF)lpCIS->itemData2;
	if (cr1 == cr2)
		return 0;       // exact match

	// first do an intensity sort, lower intensities go first
	int intensity1 = GetRValue(cr1) + GetGValue(cr1) + GetBValue(cr1);
	int intensity2 = GetRValue(cr2) + GetGValue(cr2) + GetBValue(cr2);
	if (intensity1 < intensity2)
		return -1;      // lower intensity goes first
	else if (intensity1 > intensity2)
		return 1;       // higher intensity goes second

	// if same intensity, sort by color (blues first, reds last)
	if (GetBValue(cr1) > GetBValue(cr2))
		return -1;
	else if (GetGValue(cr1) > GetGValue(cr2))
		return -1;
	else if (GetRValue(cr1) > GetRValue(cr2))
		return -1;
	else
		return 1;
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
	colours[COLOR_ITEM_SYNTAX_KEYWORD3] = syntax_keyword3_rendition;
	colours[COLOR_ITEM_SYNTAX_WORD] = syntax_word_rendition;
	colours[COLOR_ITEM_SYNTAX_STRING1] = syntax_string1_rendition;
	colours[COLOR_ITEM_SYNTAX_STRING2] = syntax_string2_rendition;
	colours[COLOR_ITEM_SYNTAX_STRING3] = syntax_string3_rendition;
	colours[COLOR_ITEM_SYNTAX_QUOTE] = syntax_quote_rendition;
	colours[COLOR_ITEM_SYNTAX_COMMENT1] = syntax_comment1_rendition;
	colours[COLOR_ITEM_SYNTAX_COMMENT2] = syntax_comment2_rendition;
	colours[COLOR_ITEM_SYNTAX_COMMENT3] = syntax_comment3_rendition;
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
	DDX_Control(pDX, IDC_OPTCOL_BACKGROUND, m_background);
	DDX_Control(pDX, IDC_OPTCOL_FOREGROUND, m_foreground);
	DDX_Control(pDX, IDC_OPTCOL_SAMPLE, m_sample_text);
	DDX_Control(pDX, IDC_OPTCOL_ITEMLIST, m_item_list);
	DDX_Check(pDX, IDC_OPTCOL_ITALIC, m_italic);
	DDX_Check(pDX, IDC_OPTCOL_UNDERLINE, m_underline);
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CColourOpt, CDialog)
	//{{AFX_MSG_MAP(CColourOpt)
	ON_LBN_SELCHANGE(IDC_OPTCOL_ITEMLIST, OnSelchangeOptcolItemlist)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_OPTCOL_DEFAULT, OnOptcolDefault)
	ON_BN_CLICKED(IDC_OPTCOL_CUSTOMISE, OnOptcolCustomise)
	ON_CBN_SELCHANGE(IDC_OPTCOL_FOREGROUND, OnSelchangeOptcolForeground)
	ON_CBN_SELCHANGE(IDC_OPTCOL_BACKGROUND, OnSelchangeOptcolBackground)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CColourOpt::save_options()
	{
	window_rendition = colours[COLOR_ITEM_NORMAL_TEXT].colourAsString();
	region_rendition = colours[COLOR_ITEM_HIGHLIGHT_TEXT].colourAsString();
	mode_line_rendition = colours[COLOR_ITEM_MODE_LINE].colourAsString();
	syntax_keyword1_rendition = colours[COLOR_ITEM_SYNTAX_KEYWORD1].colourAsString();
	syntax_keyword2_rendition = colours[COLOR_ITEM_SYNTAX_KEYWORD2].colourAsString();
	syntax_keyword3_rendition = colours[COLOR_ITEM_SYNTAX_KEYWORD3].colourAsString();
	syntax_word_rendition = colours[COLOR_ITEM_SYNTAX_WORD].colourAsString();
	syntax_string1_rendition = colours[COLOR_ITEM_SYNTAX_STRING1].colourAsString();
	syntax_string2_rendition = colours[COLOR_ITEM_SYNTAX_STRING2].colourAsString();
	syntax_string3_rendition = colours[COLOR_ITEM_SYNTAX_STRING3].colourAsString();
	syntax_quote_rendition = colours[COLOR_ITEM_SYNTAX_QUOTE].colourAsString();
	syntax_comment1_rendition = colours[COLOR_ITEM_SYNTAX_COMMENT1].colourAsString();
	syntax_comment2_rendition = colours[COLOR_ITEM_SYNTAX_COMMENT2].colourAsString();
	syntax_comment3_rendition = colours[COLOR_ITEM_SYNTAX_COMMENT3].colourAsString();
	user_1_rendition = colours[COLOR_ITEM_USER_1].colourAsString();
	user_2_rendition = colours[COLOR_ITEM_USER_2].colourAsString();
	user_3_rendition = colours[COLOR_ITEM_USER_3].colourAsString();
	user_4_rendition = colours[COLOR_ITEM_USER_4].colourAsString();
	user_5_rendition = colours[COLOR_ITEM_USER_5].colourAsString();
	user_6_rendition = colours[COLOR_ITEM_USER_6].colourAsString();
	user_7_rendition = colours[COLOR_ITEM_USER_7].colourAsString();
	user_8_rendition = colours[COLOR_ITEM_USER_8].colourAsString();

	EmacsWinColours regcolours;

	regcolours.windowText( colours[COLOR_ITEM_NORMAL_TEXT] );
	regcolours.highlightText( colours[COLOR_ITEM_HIGHLIGHT_TEXT] );
	regcolours.modeLine( colours[COLOR_ITEM_MODE_LINE] );
	regcolours.syntaxKeyword1( colours[COLOR_ITEM_SYNTAX_KEYWORD1] );
	regcolours.syntaxKeyword2( colours[COLOR_ITEM_SYNTAX_KEYWORD2] );
	regcolours.syntaxKeyword3( colours[COLOR_ITEM_SYNTAX_KEYWORD3] );
	regcolours.syntaxWord( colours[COLOR_ITEM_SYNTAX_WORD] );
	regcolours.syntaxString1( colours[COLOR_ITEM_SYNTAX_STRING1] );
	regcolours.syntaxString2( colours[COLOR_ITEM_SYNTAX_STRING2] );
	regcolours.syntaxString3( colours[COLOR_ITEM_SYNTAX_STRING3] );
	regcolours.syntaxQuote( colours[COLOR_ITEM_SYNTAX_QUOTE] );
	regcolours.syntaxComment1( colours[COLOR_ITEM_SYNTAX_COMMENT1] );
	regcolours.syntaxComment2( colours[COLOR_ITEM_SYNTAX_COMMENT2] );
	regcolours.syntaxComment3( colours[COLOR_ITEM_SYNTAX_COMMENT3] );
	regcolours.user1( colours[COLOR_ITEM_USER_1] );
	regcolours.user2( colours[COLOR_ITEM_USER_2] );
	regcolours.user3( colours[COLOR_ITEM_USER_3] );
	regcolours.user4( colours[COLOR_ITEM_USER_4] );
	regcolours.user5( colours[COLOR_ITEM_USER_5] );
	regcolours.user6( colours[COLOR_ITEM_USER_6] );
	regcolours.user7( colours[COLOR_ITEM_USER_7] );
	regcolours.user8( colours[COLOR_ITEM_USER_8] );

	EmacsWinRegistry reg;
	reg.displayColours( regcolours );
	reg.saveRegistrySettings();
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

static COLORREF fixed_colour_list[] =
	{
	RGB(255,128,128),
	RGB(255,0,0),
	RGB(128,64,64),
	RGB(128,0,0),
	RGB(64,0,0),
	RGB(0,0,0),

	RGB(255,255,128),
	RGB(255,255,0),
	RGB(255,128,64),
	RGB(255,128,0),
	RGB(128,64,0),
	RGB(128,128,0),

	RGB(128,255,128),
	RGB(128,255,0),
	RGB(0,255,0),
	RGB(0,128,0),
	RGB(0,64,0),
	RGB(128,128,64),

	RGB(0,255,128),
	RGB(0,255,64),
	RGB(0,128,128),
	RGB(0,128,64),
	RGB(0,64,64),
	RGB(128,128,128),

	RGB(128,255,255),
	RGB(0,255,255),
	RGB(0,64,128),
	RGB(0,0,255),
	RGB(0,0,128),
	RGB(64,128,128),

	RGB(0,128,255),
	RGB(0,128,192),
	RGB(128,128,255),
	RGB(0,0,160),
	RGB(0,0,64),
	RGB(192,192,192),

	RGB(255,128,192),
	RGB(128,128,192),
	RGB(128,0,64),
	RGB(128,0,128),
	RGB(64,0,64),
	RGB(255,128,255),

	RGB(255,128,255),
	RGB(255,0,255),
	RGB(255,0,128),
	RGB(128,0,255),
	RGB(64,0,128),
	RGB(255,255,255)
	};
const int num_fixed_colour_list( sizeof( fixed_colour_list )/sizeof( COLORREF ) );

void CColourOpt::fillInColours( CColorComboBox &list_box, COLORREF *custom_colours )
	{
	list_box.ResetContent();

	for( int i=0; i<num_fixed_colour_list; i++ )
		list_box.AddColorItem( fixed_colour_list[i] );

	for( int j=0; j<16; j++ )
		if( custom_colours[j] != RGB(255,255,255) )
			list_box.AddColorItem( custom_colours[j] );
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
	index = m_item_list.AddString( "Syntax keyword 3" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_KEYWORD3 );
	index = m_item_list.AddString( "Syntax word" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_WORD );

	index = m_item_list.AddString( "Syntax string 1" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_STRING1 );
	index = m_item_list.AddString( "Syntax string 2" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_STRING2 );
	index = m_item_list.AddString( "Syntax string 3" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_STRING3 );

	index = m_item_list.AddString( "Syntax quote" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_QUOTE );

	index = m_item_list.AddString( "Syntax comment 1" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_COMMENT1 );
	index = m_item_list.AddString( "Syntax comment 2" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_COMMENT2 );
	index = m_item_list.AddString( "Syntax comment 3" );
	m_item_list.SetItemData( index, COLOR_ITEM_SYNTAX_COMMENT3 );

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

	// Nothing selected
	cur_item = -1;
	m_item_list.SetCurSel( cur_item );

	//
	//	Fill in the colour list boxes
	//
	COLORREF custom_colours[16];
	getCustomColours( custom_colours );
	fillInColours( m_background, custom_colours );
	fillInColours( m_foreground, custom_colours );

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

	m_background.SetCurSelToColour( colours[cur_item].background );
	m_foreground.SetCurSelToColour( colours[cur_item].foreground );
	}



void CColourOpt::OnSelchangeOptcolForeground() 
	{
	int item = m_foreground.GetCurSel();
	if( item >= 0 )
		colours[cur_item].foreground = m_foreground.GetItemData( item );

	m_sample_text.Invalidate();
	}

void CColourOpt::OnSelchangeOptcolBackground() 
	{
	int item = m_background.GetCurSel();
	if( item >= 0 )
		colours[cur_item].background = m_background.GetItemData( item );

	m_sample_text.Invalidate();
	
	}

void CColourOpt::OnOptcolCustomise() 
	{
	CColorDialog color( colours[cur_item].foreground );
	getCustomColours( color.m_cc.lpCustColors );
	if( color.DoModal() == IDOK )
		{
		saveCustomColours( color.m_cc.lpCustColors );
		fillInColours( m_foreground, color.m_cc.lpCustColors );
		fillInColours( m_background, color.m_cc.lpCustColors );
		}
	}

void CColourOpt::getCustomColours( COLORREF *colours )
	{
	for( int index=0; index<16; index++ )
		{
		EmacsString indexStr( FormatString("%d") << index );
		colours[index] = theApp.GetProfileInt( "CustomColours", indexStr, RGB(255,255,255) );
		}
	}

void CColourOpt::saveCustomColours( COLORREF *colours )
	{
	for( int index=0; index<16; index++ )
		{
		EmacsString indexStr( FormatString("%d") << index );
		theApp.WriteProfileInt( "CustomColours", indexStr, colours[index] );
		}
	}

void CColourOpt::OnOptcolDefault() 
	{
	colours[COLOR_ITEM_MODE_LINE] = mode_line_rendition_default;
	colours[COLOR_ITEM_NORMAL_TEXT] = window_rendition_default;
	colours[COLOR_ITEM_HIGHLIGHT_TEXT] = region_rendition_default;
	colours[COLOR_ITEM_SYNTAX_KEYWORD1] = syntax_keyword1_rendition_default;
	colours[COLOR_ITEM_SYNTAX_KEYWORD2] = syntax_keyword2_rendition_default;
	colours[COLOR_ITEM_SYNTAX_KEYWORD3] = syntax_keyword3_rendition_default;
	colours[COLOR_ITEM_SYNTAX_WORD] = syntax_word_rendition_default;
	colours[COLOR_ITEM_SYNTAX_STRING1] = syntax_string1_rendition_default;
	colours[COLOR_ITEM_SYNTAX_STRING2] = syntax_string2_rendition_default;
	colours[COLOR_ITEM_SYNTAX_STRING3] = syntax_string3_rendition_default;
	colours[COLOR_ITEM_SYNTAX_QUOTE] = syntax_quote_rendition_default;
	colours[COLOR_ITEM_SYNTAX_COMMENT1] = syntax_comment1_rendition_default;
	colours[COLOR_ITEM_SYNTAX_COMMENT2] = syntax_comment2_rendition_default;
	colours[COLOR_ITEM_SYNTAX_COMMENT3] = syntax_comment3_rendition_default;
	colours[COLOR_ITEM_USER_1] = user_1_rendition_default;
	colours[COLOR_ITEM_USER_2] = user_2_rendition_default;
	colours[COLOR_ITEM_USER_3] = user_3_rendition_default;
	colours[COLOR_ITEM_USER_4] = user_4_rendition_default;
	colours[COLOR_ITEM_USER_5] = user_5_rendition_default;
	colours[COLOR_ITEM_USER_6] = user_6_rendition_default;
	colours[COLOR_ITEM_USER_7] = user_7_rendition_default;
	colours[COLOR_ITEM_USER_8] = user_8_rendition_default;

	int cur_item = (int)m_item_list.GetItemData( m_item_list.GetCurSel() ); 
	if( cur_item >= 0 )
		{
		m_italic = colours[cur_item].italic;
		m_underline = colours[cur_item].underline;
		// update the controls
		UpdateData( FALSE );

		m_background.SetCurSelToColour( colours[cur_item].background );
		m_foreground.SetCurSelToColour( colours[cur_item].foreground );
		}

	m_sample_text.Invalidate();
	}

void CDirOpt::OnOK()
	{
	CDialog::OnOK();
	}

/////////////////////////////////////////////////////////////////////////////
// CPrintOpt dialog

CPrintOpt::CPrintOpt( CDC &_print_dc, CWnd* pParent /*=NULL*/ )
	: CDialog(CPrintOpt::IDD, pParent)
	, reg()
	, print_dc( _print_dc )
	, view( (CWinemacsView *)theActiveView )
	{
	//{{AFX_DATA_INIT(CPrintOpt)
	m_header_format = "";
	m_footer_format = "";
	m_bottom_margin = float(0.0);
	m_left_margin = float(0.0);
	m_right_margin = float(0.0);
	m_top_margin = float(0.0);
	m_margin_units = _T("");
	m_borders = TRUE;
	m_line_numbers = FALSE;
	m_print_footer = TRUE;
	m_print_header = TRUE;
	m_print_in_colour = FALSE;
	m_print_syntax_colour = FALSE;
	m_header_font = _T("");
	m_footer_font = _T("");
	m_document_font = _T("");
	m_page_break_range = 0;
	m_wrap_long_lines = FALSE;
	m_page_size = _T("");
	m_usable_area = _T("");
	//}}AFX_DATA_INIT

	EmacsWinRegistry::units units = EmacsWinRegistry::measurementUnits();

	switch( units )
		{
	case EmacsWinRegistry::units__inch:
		twips_scaling = 1440.0;
		rounding_factor = 100.0;
		m_margin_units = "Specify margin dimensions in inches";
		units_string = "\"";

		page_width = double(print_dc.GetDeviceCaps( HORZRES )) / double(print_dc.GetDeviceCaps( LOGPIXELSX ));
		page_length = double(print_dc.GetDeviceCaps( VERTRES )) / double(print_dc.GetDeviceCaps( LOGPIXELSY ));
		break;

	case EmacsWinRegistry::units__mm:
		twips_scaling = 1440./25.4;
		rounding_factor = 10.0;
		m_margin_units = "Specify margin dimensions in millimetres";
		units_string = "mm";

		// can access the size directly in mm
		page_width = print_dc.GetDeviceCaps( HORZSIZE );
		page_length = print_dc.GetDeviceCaps( VERTSIZE );
		break;

	default:
		emacs_assert(false);
		}

	reg.loadRegistrySettings();
	options = reg.printOptions();

	load_options();
	}

void CPrintOpt::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPrintOpt)
	DDX_Text(pDX, IDC_PRINTER_HEADER_FORMAT, m_header_format);
	DDX_Text(pDX, IDC_PRINTER_FOOTER_FORMAT, m_footer_format);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_BOTTOM, m_bottom_margin);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_LEFT, m_left_margin);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_RIGHT, m_right_margin);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_TOP, m_top_margin);
	DDX_Text(pDX, IDC_PRINTER_MARGIN_UNITS, m_margin_units);
	DDX_Check(pDX, IDC_PRINTER_BORDERS, m_borders);
	DDX_Check(pDX, IDC_PRINTER_LINE_NUMBERS, m_line_numbers);
	DDX_Check(pDX, IDC_PRINT_FOOTER, m_print_footer);
	DDX_Check(pDX, IDC_PRINT_HEADER, m_print_header);
	DDX_Check(pDX, IDC_PRINT_COLOUR, m_print_in_colour);
	DDX_Check(pDX, IDC_PRINT_SYNTAX, m_print_syntax_colour);
	DDX_Text(pDX, IDC_PRINTER_HEADER_FONT, m_header_font);
	DDX_Text(pDX, IDC_PRINTER_FOOTER_FONT, m_footer_font);
	DDX_Text(pDX, IDC_PRINTER_DOCUMENT_FONT, m_document_font);
	DDX_Text(pDX, IDC_PRINTER_PAGE_BREAK_RANGE, m_page_break_range);
	DDV_MinMaxInt(pDX, m_page_break_range, 0, 60);
	DDX_Check(pDX, IDC_PRINTER_WRAP_LONG_LINES, m_wrap_long_lines);
	DDX_Text(pDX, IDC_PRINTER_PAGE_SIZE, m_page_size);
	DDX_Text(pDX, IDC_PRINTER_USABLE_AREA, m_usable_area);
	//}}AFX_DATA_MAP
	}

BEGIN_MESSAGE_MAP(CPrintOpt, CDialog)
	//{{AFX_MSG_MAP(CPrintOpt)
	ON_BN_CLICKED(IDC_PRINTER_SELECT_DOCUMENT_FONT, OnPrinterSelectDocumentFont)
	ON_BN_CLICKED(IDC_PRINTER_SELECT_HEADER_FONT, OnPrinterSelectHeaderFont)
	ON_BN_CLICKED(IDC_PRINTER_SELECT_FOOTER_FONT, OnPrinterSelectFooterFont)
	ON_BN_CLICKED(IDC_PRINTER_RESET_ALL, OnPrinterResetAll)
	//}}AFX_MSG_MAP
	ON_EN_KILLFOCUS(IDC_PRINTER_MARGIN_TOP, updateDialogSummary)
	ON_EN_KILLFOCUS(IDC_PRINTER_MARGIN_BOTTOM, updateDialogSummary)
	ON_EN_KILLFOCUS(IDC_PRINTER_MARGIN_LEFT, updateDialogSummary)
	ON_EN_KILLFOCUS(IDC_PRINTER_MARGIN_RIGHT, updateDialogSummary)
	ON_BN_CLICKED(IDC_PRINTER_LINE_NUMBERS, updateDialogSummary)
	ON_BN_CLICKED(IDC_PRINTER_BORDERS, updateDialogSummary)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPrintOpt message handlers

void CPrintOpt::OnOK()
	{
	if (!UpdateData(TRUE))
		{
		TRACE0("UpdateData failed during dialog termination.\n");
		// the UpdateData routine will set focus to correct item
		return;
		}

	if( (view->print_text_area.Height()/view->print_char_height) < 1 )
		{
		view->MessageBox
			(
			"Adjust the settings to allow atleast\n"
			"one line to be printed"
			"Print options validation problem",
			MB_OK
			);
		return;
		}

	if( (view->print_text_area.Width()/view->print_char_width) < 1 )
		{
		view->MessageBox
			(
			"Adjust the settings to allow atleast\n"
			"one character to be printed on each line"
			"Print options validation problem",
			MB_OK
			);
		return;
		}

	EndDialog(IDOK);
	}

static EmacsString fontDescription( const EmacsWinFont &font )
	{
	int ps_int = font.pointSize() / 10;
	int ps_frac = font.pointSize() % 10;

	if( ps_frac == 0 )
		return FormatString("%s  %dpt") << EmacsString( font.face() ) << ps_int;

	return FormatString("%s  %d.%dpt") << EmacsString( font.face() ) << ps_int << ps_frac;
	}

static EmacsString page_size_as_string( double size, double rounding_factor )
	{
	int int_size = int(size * rounding_factor);
	int frac_size = int_size % int( rounding_factor );
	int_size /= int( rounding_factor );

	if( frac_size == 0 )
		return FormatString("%d") << int_size;

	if( rounding_factor > 10.0 )
		// two digits
		return FormatString("%d.%02.2d") << int_size << frac_size;

	return  FormatString("%d.%1d") << int_size << frac_size;
	}

void CPrintOpt::load_options()
	{
	m_top_margin =		round_margin_value( (double)( options.topMargin() / twips_scaling ) );
	m_bottom_margin =	round_margin_value( (double)( options.bottomMargin() / twips_scaling ) );
	m_left_margin =		round_margin_value( (double)( options.leftMargin() / twips_scaling ) );
	m_right_margin =	round_margin_value( (double)( options.rightMargin() / twips_scaling ) );
	
	m_line_numbers =	options.lineNumbers();
	m_borders =		options.borders();
	m_print_header =	options.header();
	m_print_footer =	options.footer();
	m_print_syntax_colour = options.syntaxColour();
	m_print_in_colour =	options.inColour();
	m_wrap_long_lines =	options.alwaysWrapLongLines();

	m_header_format =	options.headerFormat().data();
	m_footer_format =	options.footerFormat().data();

	m_document_font =	fontDescription( options.printerDocumentFont() ).data();
	m_header_font =		fontDescription( options.printerHeaderFont() ).data();
	m_footer_font =		fontDescription( options.printerFooterFont() ).data();

	if( view->initialiseViewForPrinting( &print_dc, options ) )
		{
		view->tidyUpViewAfterPrinting( &print_dc );

		EmacsString descriptions;

		descriptions = FormatString("Page size %s%s x %s%s")
					<< page_size_as_string( page_width, rounding_factor ) << units_string
					<< page_size_as_string( page_length, rounding_factor ) << units_string;
		m_page_size = descriptions.sdata();

		descriptions = FormatString("Usable area is %d lines of %d characters")
					<< view->print_text_area.Height()/view->print_char_height
					<< view->print_text_area.Width()/view->print_char_width;
		m_usable_area = descriptions.sdata();
		}
	}

float CPrintOpt::round_margin_value( double margin )
	{
	double scaled_value = margin * rounding_factor;
	double ceil_value = ceil( scaled_value );
	double floor_value = floor( scaled_value );

	if( (ceil_value - scaled_value) <= (scaled_value - floor_value) )
		// round up
		return float( ceil_value/rounding_factor );
	else
		return float( floor_value/rounding_factor );
		// round down
	}


void CPrintOpt::updateDialogSummary()
	{
	// get the data out of the dialog
	if( UpdateData( TRUE ) )
		{
		// update the options objects
		save_options_from_dialog();
		// set the summary strings from the options
		load_options();

		// put the data back into the dialog
		UpdateData( FALSE );
		}
	}

void CPrintOpt::save_options_from_dialog()
	{
	options.topMargin	( (int)( m_top_margin    * twips_scaling ) );
	options.bottomMargin	( (int)( m_bottom_margin * twips_scaling ) );
	options.leftMargin	( (int)( m_left_margin   * twips_scaling ) );
	options.rightMargin	( (int)( m_right_margin  * twips_scaling ) );
	options.lineNumbers	( m_line_numbers != 0 );
	options.borders		( m_borders != 0 );
	options.header		( m_print_header != 0);
	options.footer		( m_print_footer != 0);
	options.syntaxColour	( m_print_syntax_colour != 0 );
	options.inColour	( m_print_in_colour != 0 );
	options.alwaysWrapLongLines( m_wrap_long_lines != 0 );

	options.headerFormat	( makeEmacsString( m_header_format ) );
	options.footerFormat	( makeEmacsString( m_footer_format ) );
	}

void CPrintOpt::save_options()
	{
	save_options_from_dialog();
	reg.printOptions( options );
	reg.saveRegistrySettings();
	}

void CPrintOpt::OnPrinterSelectDocumentFont() 
	{
	EmacsWinFont font( options.printerDocumentFont() );

	if( view->choosePrinterFont( print_dc, font ) )
		{
		options.printerDocumentFont( font );

		updateDialogSummary();
		}
	}

void CPrintOpt::OnPrinterSelectHeaderFont() 
	{
	EmacsWinFont font( options.printerHeaderFont() );

	if( view->choosePrinterFont( print_dc, font ) )
		{
		options.printerHeaderFont( font );

		updateDialogSummary();
		}
	}

void CPrintOpt::OnPrinterSelectFooterFont() 
	{
	EmacsWinFont font( options.printerFooterFont() );

	if( view->choosePrinterFont( print_dc, font ) )
		{
		options.printerFooterFont( font );

		updateDialogSummary();
		}
	}

void CPrintOpt::OnPrinterResetAll() 
	{
	// create a print options object with its defaults intact
	EmacsWinPrintOptions	default_options;

	// copy over the options
	options = default_options;

	// setup the dialog box variables from the options
	load_options();

	// update the dialog with the defaults
	UpdateData( FALSE );
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
		m_disk[i] = theApp.GetProfileInt( "FileParsing", disk, 0 );
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
		theApp.WriteProfileInt( "FileParsing", disk, m_disk[i] );
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

/////////////////////////////////////////////////////////////////////////////
// CLogNameOpt dialog


CLogNameOpt::CLogNameOpt(CWnd* pParent /*=NULL*/)
	: CDialog(CLogNameOpt::IDD, pParent)
	, sortFunction( SortByNameForward )
	{
	//{{AFX_DATA_INIT(CLogNameOpt)
	m_name = _T("");
	m_value = _T("");
	//}}AFX_DATA_INIT
	}


void CLogNameOpt::DoDataExchange(CDataExchange* pDX)
	{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogNameOpt)
	DDX_Control(pDX, IDC_LOGNAME_DELETE, m_delete_button);
	DDX_Control(pDX, IDC_LOGNAME_SET, m_set_button);
	DDX_Control(pDX, IDC_LOGNAME_LIST, m_list);
	DDX_Text(pDX, IDC_LOGNAME_NAME, m_name);
	DDX_Text(pDX, IDC_LOGNAME_VALUE, m_value);
	//}}AFX_DATA_MAP
	}


BEGIN_MESSAGE_MAP(CLogNameOpt, CDialog)
	//{{AFX_MSG_MAP(CLogNameOpt)
	ON_BN_CLICKED(IDC_LOGNAME_SET, OnLogNameSet)
	ON_BN_CLICKED(IDC_LOGNAME_DELETE, OnLogNameDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LOGNAME_LIST, OnItemchangedLogNameList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LOGNAME_LIST, OnColumnClickLogNameList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogNameOpt message handlers

void CLogNameOpt::OnLogNameSet() 
	{
	UpdateData( TRUE );

	CWinApp *app = AfxGetApp();
	HKEY hSecKey = app->GetSectionKey( "DeviceNames" );
	if( hSecKey == NULL )
		return;

	int retCode = RegSetValueEx
		(
		hSecKey,
		m_name,
		0,
		REG_SZ,
		(const unsigned char *)(const char *)m_value, m_value.GetLength()
		);
	if( retCode != ERROR_SUCCESS )
		AfxMessageBox("Failed to set value");

	RegCloseKey( hSecKey );

	LV_FINDINFO find;
	find.flags = LVFI_STRING;
	find.psz = m_name;
	find.lParam = 0;

	int pos = m_list.FindItem( &find );
	if( pos == -1 )
		{
		m_list.AddItem( cur_item, 0, m_name );
		m_list.AddItem( cur_item, 1, m_value );
		}
	else
		{
		m_list.SetItemText( pos, 1, m_value );
		}

	m_list.SortItems( sortFunction, (unsigned long)this );
	}

void CLogNameOpt::OnLogNameDelete() 
	{
	UpdateData( TRUE );

	CWinApp *app = AfxGetApp();
	HKEY hSecKey = app->GetSectionKey( "DeviceNames" );
	if( hSecKey == NULL )
		return;

	int retCode = RegDeleteValue( hSecKey, m_name );
	if( retCode != ERROR_SUCCESS )
		AfxMessageBox("Failed to delete the value");

	RegCloseKey( hSecKey );

	m_list.DeleteItem( cur_item );
	}

void CLogNameOpt::OnOK() 
	{
	// TODO: Add extra validation here
	
	CDialog::OnOK();
	}

BOOL CLogNameOpt::OnInitDialog() 
	{
	CDialog::OnInitDialog();
	BOOL status;
	status = m_list.AddColumn( "Logical Name", 0 );
	status = m_list.AddColumn( "Value", 1 );
	
	CWinApp *app = AfxGetApp();
	HKEY hSecKey = app->GetSectionKey( "DeviceNames" );
	if( hSecKey == NULL )
		return TRUE;

	int width_of_name_column = m_list.GetColumnWidth( 0 );
	int width_of_value_column = m_list.GetColumnWidth( 1 );
	for( int retCode = ERROR_SUCCESS, value_index=0;
		retCode == ERROR_SUCCESS;
			value_index++ )
		{
		char value_name[MAX_PATH];
		DWORD value_length = MAX_PATH;
		unsigned char data[MAX_PATH];
		DWORD data_length = MAX_PATH;
		DWORD value_type = 0;

		retCode = RegEnumValue
			(
			hSecKey, value_index,
			value_name, &value_length,
			NULL,
			&value_type,
			data, &data_length
			);
		if( retCode == ERROR_SUCCESS && value_type == REG_SZ )
			{
			m_list.AddItem( value_index, 0, value_name );
			int width = m_list.GetStringWidth( value_name );
			if( width_of_name_column < width )
				width_of_name_column = width;

			m_list.AddItem( value_index, 1, (char *)data );
			width = m_list.GetStringWidth( (char *)data );
			if( width_of_value_column < width )
				width_of_value_column = width;
			}
		}

	m_list.SetColumnWidth( 0, width_of_name_column + 10 );
	m_list.SetColumnWidth( 1, width_of_value_column + 30 );

	RegCloseKey( hSecKey );

	m_list.SortItems( sortFunction, (unsigned long)this );

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
	}


void CLogNameOpt::OnItemchangedLogNameList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if( pNMListView->uChanged&LVIF_STATE )
		{
		if( pNMListView->uNewState&LVIS_SELECTED )
			{
			cur_item = pNMListView->iItem;

			m_name = m_list.GetItemText( cur_item, 0 );
			m_value = m_list.GetItemText( cur_item, 1 );

			// update the controls
			UpdateData( FALSE );
			}
		}


	*pResult = 0;
	}

CString CLogNameOpt::GetItemString( LPARAM param, int subItem ) const
	{
	LV_FINDINFO find;
	find.flags = LVFI_PARAM;
	find.psz = NULL;
	find.lParam = param;

	int item = m_list.FindItem( &find );
	if( item < 0 )
		return CString();

	return m_list.GetItemText( item, subItem );
	}

int CLogNameOpt::SortByNameForward( LPARAM param1, LPARAM param2, LPARAM objPointer )
	{
	CLogNameOpt *obj = (CLogNameOpt *)objPointer;

	CString str1( obj->GetItemString( param1 ) );
	CString str2( obj->GetItemString( param2 ) );
	return str1.Compare( str2 );
	}

int CLogNameOpt::SortByNameReverse( LPARAM param1, LPARAM param2, LPARAM objPointer )
	{
	CLogNameOpt *obj = (CLogNameOpt *)objPointer;

	CString str1( obj->GetItemString( param1 ) );
	CString str2( obj->GetItemString( param2	 ) );
	return str2.Compare( str1 );
	}

int CLogNameOpt::SortByValueForward( LPARAM param1, LPARAM param2, LPARAM objPointer )
	{
	CLogNameOpt *obj = (CLogNameOpt *)objPointer;

	CString str1( obj->GetItemString( param1, 1 ) );
	CString str2( obj->GetItemString( param2, 1 ) );
	return str1.Compare( str2 );
	}

int CLogNameOpt::SortByValueReverse( LPARAM param1, LPARAM param2, LPARAM objPointer )
	{
	CLogNameOpt *obj = (CLogNameOpt *)objPointer;

	CString str1( obj->GetItemString( param1, 1 ) );
	CString str2( obj->GetItemString( param2, 1 ) );
	return str2.Compare( str1 );
	}

void CLogNameOpt::OnColumnClickLogNameList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	switch( pNMListView->iSubItem )
		{
	default:
	case 0:
		if( sortFunction == SortByNameForward )
			sortFunction = SortByNameReverse;
		else
			sortFunction = SortByNameForward;
		break;
	case 1:
		if( sortFunction == SortByValueForward )
			sortFunction = SortByValueReverse;
		else
			sortFunction = SortByValueForward;
		break;
		}

	m_list.SortItems( sortFunction, (unsigned long)this );

	*pResult = 0;
	}
/////////////////////////////////////////////////////////////////////////////
// COptionToolbar dialog

COptionToolbar::COptionToolbar( CMDIFrameWnd *_frame, CWnd* pParent /*=NULL*/)
	: CDialog(COptionToolbar::IDD, pParent)
	, frame( (CMainFrame *)_frame )
{
	reg.loadRegistrySettings();

	bars = reg.toolBars();

	//{{AFX_DATA_INIT(COptionToolbar)
	m_build		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_BUILD );
	m_case		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_CASE );
	m_edit		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_EDIT );
	m_file		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_FILE );
	m_macro		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_MACROS );
	m_region	= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_REGION );
	m_search	= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_SEARCH );
	m_tools		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_TOOLS );
	m_view		= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_VIEW );
	m_window	= frame->m_toolbars->bandIsVisible( ID_VIEW_TOOLBAR_WINDOW );
	//}}AFX_DATA_INIT
}


void COptionToolbar::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionToolbar)
	DDX_Check(pDX, IDC_TOOLBAR_BUILD, m_build);
	DDX_Check(pDX, IDC_TOOLBAR_CASE, m_case);
	DDX_Check(pDX, IDC_TOOLBAR_EDIT, m_edit);
	DDX_Check(pDX, IDC_TOOLBAR_FILE, m_file);
	DDX_Check(pDX, IDC_TOOLBAR_MACRO, m_macro);
	DDX_Check(pDX, IDC_TOOLBAR_REGION, m_region);
	DDX_Check(pDX, IDC_TOOLBAR_SEARCH, m_search);
	DDX_Check(pDX, IDC_TOOLBAR_TOOLS, m_tools);
	DDX_Check(pDX, IDC_TOOLBAR_VIEW, m_view);
	DDX_Check(pDX, IDC_TOOLBAR_WINDOW, m_window);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionToolbar, CDialog)
	//{{AFX_MSG_MAP(COptionToolbar)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionToolbar message handlers

void COptionToolbar::OnOK() 
	{
	if( !UpdateData( TRUE ) )
		return;

	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_BUILD,	m_build );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_CASE,	m_case );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_EDIT,	m_edit );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_FILE,	m_file );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_MACROS,	m_macro );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_REGION,	m_region );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_SEARCH,	m_search );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_TOOLS,	m_tools );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_VIEW,	m_view );
	frame->m_toolbars->showBand( ID_VIEW_TOOLBAR_WINDOW,	m_window );

	bars.isVisible( ID_VIEW_TOOLBAR_BUILD,		m_build );
	bars.isVisible( ID_VIEW_TOOLBAR_CASE,		m_case );
	bars.isVisible( ID_VIEW_TOOLBAR_EDIT,		m_edit );
	bars.isVisible( ID_VIEW_TOOLBAR_FILE,		m_file );
	bars.isVisible( ID_VIEW_TOOLBAR_MACROS,		m_macro );
	bars.isVisible( ID_VIEW_TOOLBAR_REGION,		m_region );
	bars.isVisible( ID_VIEW_TOOLBAR_SEARCH,		m_search );
	bars.isVisible( ID_VIEW_TOOLBAR_TOOLS,		m_tools );
	bars.isVisible( ID_VIEW_TOOLBAR_VIEW,		m_view );
	bars.isVisible( ID_VIEW_TOOLBAR_WINDOW,		m_window );

	reg.toolBars( bars );
	reg.saveRegistrySettings();

	CDialog::OnOK();
	}
