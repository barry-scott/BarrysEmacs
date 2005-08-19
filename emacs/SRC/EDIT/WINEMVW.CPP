//
// winemvw.cpp : implementation of the CWinemacsView class
//

#include "stdafx.h"
#include "winemacs.h"

#include "winemdoc.h"
#include "winemvw.h"
#include "finddlg.h"
#include "switchdl.h"
#include "win_opt.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CFindDlg *find_dlg;

#define WINDOW_BORDER 1
#define V_SCROLL_RANGE 10000
#define H_SCROLL_RANGE 256

#define CARET_WIDTH	(term_cursor_mode ? (nCharWidth+1) : 2)
#define CARET_HEIGHT	nCharHeight
#define CARET_XPOS	((cursor_x*nCharWidth)+WINDOW_BORDER+(term_cursor_mode ? 0 : -1))
#define CARET_YPOS	(cursor_y*nCharHeight)

static cursor_x, cursor_y;

static COLORREF colours[16] =
	{
	RGB(   0,   0,   0 ),	/* 0 black */
	RGB( 127,   0,   0 ),	/* 4 red */
	RGB(   0, 127,   0 ),	/* 2 green */
	RGB( 127, 127,   0 ),	/* 6 brown */
	RGB(   0,   0, 127 ),	/* 1 blue */
	RGB( 127,   0, 127 ),	/* 5 magenta */
	RGB(   0, 127, 127 ),	/* 3 cyan */
	RGB( 255, 255, 255 ),	/* 7 ltgrey */

	RGB(   0,   0,   0 ),	/* 8 black */
	RGB( 255,   0,   0 ),	/* 12 lt red */
	RGB(   0, 255,   0 ),	/* 10 lt green */
	RGB( 255, 255,   0 ),	/* 14 yellow */
	RGB(   0,   0, 255 ),	/* 9 lt blue */
	RGB( 255,   0, 255 ),	/* 13 lt magenta */
	RGB(   0, 255, 255 ),	/* 11 lt cyan */
	RGB( 255, 255, 255 )	/* 15 white */
	};

C_color_attr::C_color_attr()
	{
	foreground = 0;
	background = 0;
	underline = 0;
	italic = 0;
	pen = NULL;
	}
C_color_attr::~C_color_attr()
	{
	if( pen != NULL )
		delete pen;
	}

C_color_attr & C_color_attr::operator=( unsigned char *str )
	{
	int num;
	unsigned char *p = str;
	int bold = 0;			/* default to bold off */
	int fg = 0, bg = 7;		/* fg = black, bg = white */
	int fg_r=0, fg_g=0, fg_b=0;
	int bg_r=0, bg_g=0, bg_b=0;
	int rgb_seen = 0;
	int reverse = 0;		/* reverse off */

	italic = 0;			/* italic text off */
	underline = 0;			/* underline off */

	do
		{
		/* skip any seperator char */
		while( *p == ';' )
			p++;

		/* collect the number */
		num = 0;
		while( *p && isdigit( *p ) )
			num = num*10 + *p++ - '0';

		/* figure out what the number represents */
		if( num >= 30 && num <= 37 )
			fg = num - 30;
		else if( num >= 40 && num <= 47 )
			bg = num - 40;
		else if( num == 1 )
			bold = 1;
		else if( num == 4 )
			underline = 1;
		else if( num == 1 )
			bold = 1;
		else if( num == 5 )	/* blink attribute intpreted at italic */
			italic = 1;
		else if( num == 7 )
			reverse = 1;
		else if( num >= 6000 && num <= 6255 )
			rgb_seen = 1, bg_b = (num-6000)&255;
		else if( num >= 5000 )
			rgb_seen = 1, bg_g = (num-5000)&255;
		else if( num >= 4000 )
			rgb_seen = 1, bg_r = (num-4000)&255;
		else if( num >= 3000 )
			rgb_seen = 1, fg_b = (num-3000)&255;
		else if( num >= 2000 )
			rgb_seen = 1, fg_g = (num-2000)&255;
		else if( num >= 1000 )
			rgb_seen = 1, fg_r = (num-1000)&255;
		}
	while( *p );

	if( rgb_seen )
		{
		foreground = RGB( fg_r, fg_g, fg_b );
		background = RGB( bg_r, bg_g, bg_b );
		}
	else
		{
		/* reverse means swap foreground and background colours */
		if( reverse )
			{
			int colour = fg;
			fg = bg;
			bg = colour;
			}

		/* now turn the attributes into the rendition value */
		foreground = colours[fg+(bold<<3)];
		background = colours[bg];
		}

	if( pen != NULL )
		{
		delete pen;
		pen == NULL;
		}
	if( underline )
		pen = new CPen( PS_SOLID, 0, foreground );

	return *this;
	}

void C_color_attr::GetColor( unsigned char *str )
	{
	sprintf( s_str( str ), "%d;%d;%d;%d;%d;%d",
		1000+GetRValue(foreground),
		2000+GetGValue(foreground),
		3000+GetBValue(foreground),
		4000+GetRValue(background),
		5000+GetGValue(background),
		6000+GetBValue(background) );
	if( italic )
		_str_cat( str, ";5" );
	if( underline )
		_str_cat( str, ";4" );
	}


static char special_key[] = "\033__X";

struct conv_keys
	{
	WORD key_code;
	char *translation;
	char *enh_translation;
	char *shift_translation;
	char *ctrl_translation;
	};

static struct conv_keys FAR convert[] =
	{
	0,		NULL,		NULL,		NULL,		NULL,		/* 00 */
	VK_LBUTTON,	NULL,		NULL,		NULL,		NULL,
	VK_RBUTTON,	NULL,		NULL,		NULL,		NULL,
	VK_CANCEL,	NULL,		NULL,		NULL,		NULL,
	VK_MBUTTON,	NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	VK_BACK,	"\x7f",		"\x7f",		NULL,		NULL,		/* 08 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	VK_CLEAR,	"\033Ou",	NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 10 */
	0,		NULL,		NULL,		NULL,		NULL,
	VK_MENU,	NULL,		NULL,		NULL,		NULL,
	VK_PAUSE,	"\033[34~",	"\033[34~",	"\033[134~",	"\033[84~",
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 18 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,/*VK_ESCAPE*/	NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 20 */
	VK_PRIOR,	"\033Oy",	"\033[5~",	NULL,		"\033[55~",
	VK_NEXT,	"\033Os",	"\033[6~",	NULL,		"\033[56~",
	VK_END,		"\033Oq",	"\033[4~",	NULL,		"\033[54~",
	VK_HOME,	"\033Ow",	"\033[1~",	NULL,		"\033[51~",
	VK_LEFT,	"\033Ot",	"\033OD",	NULL,		"\033[60~",
	VK_UP,		"\033Ox",	"\033OA",	NULL,		"\033[57~",
	VK_RIGHT,	"\033Ov",	"\033OC",	NULL,		"\033[59~",

	VK_DOWN,	"\033Or",	"\033OB",	NULL,		"\033[58~",	/* 28 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	VK_INSERT,	"\033Op",	"\033[2~",	NULL,		"\033[52~",
	VK_DELETE,	"\033On",	"\033[3~",	NULL,		"\033[53~",
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 30 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 38 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 40 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 48 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 50 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 58 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	VK_NUMPAD0,	"\033Op",	"\033Op",	NULL,		NULL,		/* 60 */
	VK_NUMPAD1,	"\033Oq",	"\033Oq",	NULL,		NULL,
	VK_NUMPAD2,	"\033Or",	"\033Or",	NULL,		NULL,
	VK_NUMPAD3,	"\033Os",	"\033Os",	NULL,		NULL,
	VK_NUMPAD4,	"\033Ot",	"\033Ot",	NULL,		NULL,
	VK_NUMPAD5,	"\033Ou",	"\033Ou",	NULL,		NULL,
	VK_NUMPAD6,	"\033Ov",	"\033Ov",	NULL,		NULL,
	VK_NUMPAD7,	"\033Ow",	"\033Ow",	NULL,		NULL,

	VK_NUMPAD8,	"\033Ox",	"\033Ox",	NULL,		NULL,		/* 68 */
	VK_NUMPAD9,	"\033Oy",	"\033Oy",	NULL,		NULL,
	VK_MULTIPLY,    "\033OR",	"\033OR",	NULL,		NULL,
	VK_ADD,		"\033Ol",	"\033Ol",	"\033Om",	NULL,
	VK_SEPARATOR,   NULL,		NULL,		NULL,		NULL,
	VK_SUBTRACT,    "\033OS",	"\033OS",	NULL,		NULL,
	VK_DECIMAL,	"\033On",	"\033On",	NULL,		NULL,
	VK_DIVIDE,	"\033OQ",	"\033OQ",	NULL,		NULL,

	VK_F1,		"\033[17~",	"\033[17~",	"\033[117~",	"\033[67~",	/* 70 */
	VK_F2,		"\033[18~",	"\033[18~",	"\033[118~",	"\033[68~",
	VK_F3,		"\033[19~",	"\033[19~",	"\033[119~",	"\033[69~",
	VK_F4,		"\033[20~",	"\033[20~",	"\033[120~",	"\033[70~",
	VK_F5,		"\033[21~",	"\033[21~",	"\033[121~",	"\033[71~",
	VK_F6,		"\033[23~",	"\033[23~",	"\033[123~",	"\033[73~",
	VK_F7,		"\033[24~",	"\033[24~",	"\033[124~",	"\033[74~",
	VK_F8,		"\033[25~",	"\033[25~",	"\033[125~",	"\033[75~",
	VK_F9,		"\033[26~",	"\033[26~",	"\033[126~",	"\033[76~",	/* 78 */
	VK_F10,		"\033[28~",	"\033[28~",	"\033[128~",	"\033[78~",

	VK_F11,		"\033[29~",	"\033[29~",	"\033[129~",	"\033[79~",
	VK_F12,		"\033[31~",	"\033[31~",	"\033[131~",	"\033[81~",
	VK_F13,		"\033_I",	"\033_I",	"\033[O",	NULL,
	VK_F14,		"\033_J",	"\033_J",	"\033[P",	NULL,
	VK_F15,		"\033_K",	"\033_K",	"\033[Q",	NULL,
	VK_F16,		"\033_L",	"\033_L",	"\033[R",	NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 80 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	0,		NULL,		NULL,		NULL,		NULL,		/* 88 */
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,
	0,		NULL,		NULL,		NULL,		NULL,

	VK_NUMLOCK,	"\033OP",	"\033OP",	NULL,		NULL,		/* 90 */
	VK_SCROLL,	"\033[33~",	"\033[33~",	"\033[133~",	"\033[83~"
	};
#define NUM_CONV_KEY	(sizeof( convert ) / sizeof( struct conv_keys))

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView

IMPLEMENT_DYNCREATE(CWinemacsView, CView)

BEGIN_MESSAGE_MAP(CWinemacsView, CView)
	//{{AFX_MSG_MAP(CWinemacsView)
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_STOP_EXECUTION, OnStopExecution)
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_KEYUP()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_OPTIONS_COLORS, OnOptionsColors)
	ON_COMMAND(ID_OPTIONS_DIRECTORIES, OnOptionsDirectories)
	ON_COMMAND(ID_OPTIONS_FONT, OnOptionsFont)
	ON_COMMAND(ID_SEARCH_FIND, OnSearchFind)
	ON_COMMAND(ID_SEARCH_REPLACE, OnSearchReplace)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FIND, OnUpdateSearchFind)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_REPLACE, OnUpdateSearchReplace)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_COMMAND(ID_FILE_SAVE_ENVIRONMENT, OnFileSaveEnvironment)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_DROPFILES()
	ON_COMMAND(ID_OPTIONS_FILE_PARSING, OnOptionsFileParsing)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView construction/destruction

// global pointer to the only view
CWinemacsView *theView;

CWinemacsView::CWinemacsView()
{
	theView = this;
	nCharExtra = 25;
	last_window_size = SIZEICONIC;
        
        emacs_normal_font = NULL;
        emacs_italic_font = NULL;
	have_hdc = 0;
	pDC = NULL;

	m_ignore_next_char = FALSE;

	have_focus = 0;
	old_font = 0;
	white_brush = 0;
	last_v_scroll_pos = -1;
	last_h_scroll_pos = 1;

	printer_font = NULL;
	printer_old_font = NULL;

	mouse_button_state = 0;
}

//
// The following function scale there parameter from twips to the DC's units
//
static void ScaleCPoint( CPoint *point, const CDC *pDC )
	{
	point->y = point->y * pDC->GetDeviceCaps(LOGPIXELSY) / 1440;
	point->x = point->x * pDC->GetDeviceCaps(LOGPIXELSX) / 1440;
	}

static void ScaleCSize( CSize *size, const CDC *pDC )
	{
	size->cy = size->cy * pDC->GetDeviceCaps(LOGPIXELSY) / 1440;
	size->cx = size->cx * pDC->GetDeviceCaps(LOGPIXELSX) / 1440;
	}

static void ScaleLogFont( LOGFONT *font, const CDC *pDC )
	{
	font->lfHeight = font->lfHeight * pDC->GetDeviceCaps(LOGPIXELSY) / 1440;
	}

CWinemacsView::~CWinemacsView()
{
	if( emacs_normal_font )
		delete emacs_normal_font;
	if( emacs_italic_font )
		delete emacs_italic_font;
}

/////////////////////////////////////////////////////////////////////////////
//  A view's OnInitialUpdate() overrideable function is called immediately
//  after the frame window is created, and the view within the frame
//  window is attached to its document.
void CWinemacsView::OnInitialUpdate()
	{              
	EnableScrollBarCtrl( SB_BOTH );
	SetScrollRange( SB_VERT, 0, V_SCROLL_RANGE-1, FALSE );
	SetScrollRange( SB_HORZ, 1, H_SCROLL_RANGE, FALSE );

	emacs_cf.lStructSize = sizeof(emacs_cf);
	emacs_cf.hwndOwner = m_hWnd;
	emacs_cf.lpszStyle = NULL; // szStyle;

	emacs_cf.Flags = CF_SCREENFONTS;
	emacs_cf.Flags |= CF_ANSIONLY;
	emacs_cf.Flags |= CF_FORCEFONTEXIST;
//	emacs_cf.Flags |= CF_NOSTYLESEL;

	emacs_cf.Flags |= CF_LIMITSIZE;
	emacs_cf.nSizeMin = 6;
	emacs_cf.nSizeMax = 36;

	emacs_cf.Flags |= CF_FIXEDPITCHONLY;

	emacs_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	emacs_cf.lpLogFont = &emacs_lf;

	emacs_lf.lfHeight = theApp.GetProfileInt( "Fonts", "TextFontSize", 9 );
	emacs_lf.lfWidth = 0;
	emacs_lf.lfEscapement = 0;
	emacs_lf.lfOrientation = 0;
	emacs_lf.lfWeight = FW_NORMAL;
	emacs_lf.lfItalic = FALSE;
	emacs_lf.lfUnderline = FALSE;
	emacs_lf.lfStrikeOut = FALSE;
	emacs_lf.lfCharSet = ANSI_CHARSET;
	emacs_lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	emacs_lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	emacs_lf.lfQuality = PROOF_QUALITY;
	emacs_lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	GetPrivateProfileString( "Fonts", "TextFontName", "FixedSys", emacs_lf.lfFaceName, LF_FACESIZE, theApp.m_pszProfileName );

	new_font();
	ASSERT( emacs_normal_font != NULL );
	ASSERT( emacs_italic_font != NULL );

	white_brush = (HBRUSH)::GetStockObject( WHITE_BRUSH );

	/*                         
	 *	Restore the last position of the window
	 */
	unsigned char buf[128];

	buf[0] = '\0';
	GetPrivateProfileString( "Window Positions", "Window_1", "", s_str(buf), sizeof( buf ), theApp.m_pszProfileName );

	WINDOWPLACEMENT where;
	if( sscanf
	( s_str(buf), "%d %d %d %d %d %d %d %d %d",
	&where.showCmd,
	&where.ptMinPosition.x, &where.ptMinPosition.y,
	&where.ptMaxPosition.x, &where.ptMaxPosition.y,
	&where.rcNormalPosition.left, &where.rcNormalPosition.top,
	&where.rcNormalPosition.right, &where.rcNormalPosition.bottom
	) == 9 )
		{
		where.length = sizeof( where );
		if( !SetWindowPlacement( &where ) )
			{
			_dbg_msg( u_str("SetWindowPlacement failed  %d"), ::GetLastError() );
			}
		}
	else
		((CMDIFrameWnd *)AfxGetApp()->m_pMainWnd)->MDIMaximize( theView->GetParent() );

	/*
	 *	Print related initialisation
	 */
	emacs_cf_print.lStructSize = sizeof(emacs_cf_print);
	emacs_cf_print.hwndOwner = m_hWnd;
	emacs_cf_print.lpszStyle = szStyle;

	emacs_cf_print.Flags = CF_PRINTERFONTS;
	emacs_cf_print.Flags |= CF_ANSIONLY;
	emacs_cf_print.Flags |= CF_FORCEFONTEXIST;
/*	emacs_cf_print.Flags |= CF_NOSTYLESEL;	*/

	emacs_cf_print.Flags |= CF_LIMITSIZE;
	emacs_cf_print.nSizeMin = 6;
	emacs_cf_print.nSizeMax = 36;

/*	emacs_cf_print.Flags |= CF_FIXEDPITCHONLY;	*/

	emacs_cf_print.Flags |= CF_INITTOLOGFONTSTRUCT;
	emacs_cf_print.lpLogFont = &emacs_lf_print;

	// height is in twips 9pt * 20
	emacs_lf_print.lfHeight = theApp.GetProfileInt( "Print", "font_size", -9*20 );
	emacs_lf_print.lfWidth = 0;
	emacs_lf_print.lfEscapement = 0;
	emacs_lf_print.lfOrientation = 0;
	emacs_lf_print.lfWeight = FW_NORMAL;
	emacs_lf_print.lfItalic = FALSE;
	emacs_lf_print.lfUnderline = FALSE;
	emacs_lf_print.lfStrikeOut = FALSE;
	emacs_lf_print.lfCharSet = ANSI_CHARSET;
	emacs_lf_print.lfOutPrecision = OUT_DEFAULT_PRECIS;
	emacs_lf_print.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	emacs_lf_print.lfQuality = PROOF_QUALITY;
	emacs_lf_print.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	GetPrivateProfileString( "Print", "Font_Name", "Courier New", emacs_lf_print.lfFaceName, LF_FACESIZE, theApp.m_pszProfileName );
	}


/////////////////////////////////////////////////////////////////////////////
// CWinemacsView drawing

void CWinemacsView::OnDraw(CDC* pDC)
{
	CWinemacsDoc* pDoc = GetDocument();
	CSize line_size( nCharWidth*tt->t_width, nCharHeight );

	for( int n = 0; n < tt->t_length; )
		{
		CPoint pt( 0, n*nCharHeight );
		CRect this_line( pt, line_size );

		n++;

		if( pDC->RectVisible( this_line ) )
			win_update_line( NULL, tt->t_phys_screen[n], n );
		}
}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView printing

BOOL CWinemacsView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

extern int dump_bfl(
		int line_start_n,
		int sline,
		int scolumn,
		struct window *w,
		int initial_column
		);

void CWinemacsView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
	{
	pInfo->m_bContinuePrinting = TRUE;
	int map_mode = pDC->GetMapMode();

//	pDC->SetMapMode( MM_ANISOTROPIC );
//	pDC->SetWindowExt( 1440, 1440 );
//	pDC->SetViewportExt( pDC->GetDeviceCaps(LOGPIXELSX), -pDC->GetDeviceCaps(LOGPIXELSY) );

	// get the lastest printer font and size
	emacs_lf_print.lfHeight = theApp.GetProfileInt( "Print", "font_size", -9*20 );
	GetPrivateProfileString( "Print", "Font_Name", "Courier New", emacs_lf_print.lfFaceName, LF_FACESIZE, theApp.m_pszProfileName );
	ScaleLogFont( &emacs_lf_print, pDC );

	printer_old_font = NULL;
	printer_font = new CFont();
	if( printer_font != NULL )
	    printer_font->CreateFontIndirect( &emacs_lf_print );

	if( printer_font )
		printer_old_font = pDC->SelectObject( printer_font );
	pDC->GetOutputTextMetrics( &print_textmetric );
	print_char_height = (int)(print_textmetric.tmExternalLeading + print_textmetric.tmHeight);
	print_char_width = print_textmetric.tmAveCharWidth;
//		+ (print_textmetric.tmMaxCharWidth - print_textmetric.tmAveCharWidth)*nCharExtra/100;
	if( printer_old_font )
		pDC->SelectObject( printer_old_font );


	// calc the logical size of the page
	CRect rect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
	pDC->DPtoLP(&rect);

	// find the top left margin
	print_top_left_margin = 
	CPoint( AfxGetApp()->GetProfileInt( "Print", "margin_left", 1440 ),
			(int)(AfxGetApp()->GetProfileInt( "Print", "margin_top", 1440 )) );
	ScaleCPoint( &print_top_left_margin, pDC );

	CSize bottom_right( AfxGetApp()->GetProfileInt( "Print", "margin_right", 1440 ),
			(int)(AfxGetApp()->GetProfileInt( "Print", "margin_bottom", 1440 )));
	ScaleCSize( &bottom_right, pDC );

	CSize margins = CSize(print_top_left_margin );
	margins += bottom_right;

	// remove the margins
	rect.right -= margins.cx;
	rect.bottom -= margins.cy;

	print_window.w_next = &print_window;
	print_window.w_prev = NULL;
	print_window.w_right = NULL;
	print_window.w_left = NULL;
	print_window.w_buf = bf_cur;
	print_window.w_dot = NULL;
	print_window.w_start = NULL;
	print_window.w_height = rect.Height()/print_char_height;
	print_window.w_width = rect.Width()/print_char_width;
	print_window.w_lastuse = 0;
	print_window.w_force = 0;
	print_window.w_horizontal_scroll = 0;

	// fill in the page start array
	int print_buffer_pos = 1;
	unsigned char text_buffer[256], attr_buffer[256];

	for( int page=1; page<=PRINT_MAX_PAGE; page++ )
		{
		print_page_pos[page] = print_buffer_pos;

		for( int line=0; line<print_window.w_height; line++ )
			{
			if( print_buffer_pos >= bf_s1+bf_s2 )
				break;

			text_cursor = text_buffer;
			attr_cursor = attr_buffer;
			columns_left = print_window.w_width;
			print_buffer_pos = dump_bfl( print_buffer_pos, -1, -1, &print_window, 1 );
			if( print_buffer_pos < 0 )
				print_buffer_pos = -print_buffer_pos;
			}

		if( print_buffer_pos >= bf_s1+bf_s2 )
			break;
		}
	pInfo->SetMaxPage( page );
	}

void CWinemacsView::OnEndPrinting(CDC* pDC, CPrintInfo* /*pInfo*/)
{
/*	if( printer_old_font != NULL )
		pDC->SelectObject( printer_old_font ); */
	if( printer_font != NULL )
		delete printer_font;
	printer_old_font = NULL;
	printer_font = NULL;
}

void CWinemacsView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
	{
	if( pInfo != NULL )
		{
		// set the mapping mode we need
//		pDC->SetMapMode( MM_ANISOTROPIC );
//		pDC->SetWindowExt( 1440, 1440 );
//		pDC->SetViewportExt( pDC->GetDeviceCaps(LOGPIXELSX), -pDC->GetDeviceCaps(LOGPIXELSY) );

		// move the origin by the top left margins
		pDC->SetWindowOrg( -print_top_left_margin.x, -print_top_left_margin.y );
		}
	}

void CWinemacsView::OnPrint(CDC* pDC, CPrintInfo *pinfo )
	{
	unsigned char text_buffer[256], attr_buffer[256];
	int old_tt_width = tt->t_width;
	int old_tt_length = tt->t_length;

	if( printer_font )
		printer_old_font = pDC->SelectObject( printer_font );

	pDC->GetOutputTextMetrics( &print_textmetric );

	tt->t_width = min(print_window.w_width, sizeof(text_buffer));
	tt->t_length = print_window.w_height;

	int print_buffer_pos = print_page_pos[pinfo->m_nCurPage];

	if( pinfo->m_bPreview )
		{
		// draw a box
		pDC->MoveTo(                                       -1, -1 );
		pDC->LineTo( print_window.w_width*print_char_width, -1 );
		pDC->LineTo( print_window.w_width*print_char_width, print_window.w_height*print_char_height );
		pDC->LineTo(                                       -1, print_window.w_height*print_char_height );
		pDC->LineTo(                                       -1, -1 );
		// add diagonal lines to it
//		pDC->LineTo( print_window.w_width*print_char_width-2, print_window.w_height*print_char_height );
//		pDC->MoveTo(                                       1, print_window.w_height*print_char_height );
//		pDC->LineTo( print_window.w_width*print_char_width-2, 1 );
		}

	for( int line=0; line<print_window.w_height; line++ )
		{
		if( print_buffer_pos >= bf_s1+bf_s2 )
			{
			pinfo->m_bContinuePrinting = FALSE;
			break;
			}

		memset( text_buffer, ' ', sizeof( text_buffer ) );
		text_cursor = text_buffer;
		attr_cursor = attr_buffer;
		columns_left = print_window.w_width;
		print_buffer_pos = dump_bfl( print_buffer_pos, -1, -1, &print_window, 1 );
		if( print_buffer_pos < 0 )
			print_buffer_pos = -print_buffer_pos;

		pDC->TextOut
		(
		0, line*print_char_height,
		s_str(&text_buffer[0]), text_cursor-&text_buffer[0]
		);
		}

	tt->t_width = old_tt_width;
	tt->t_length = old_tt_length;

	pinfo->m_bContinuePrinting = print_buffer_pos < bf_s1+bf_s2;

	if( printer_old_font )
		pDC->SelectObject( printer_old_font );
	}

int CWinemacsView::new_font()
	{
	int i;

	CFont *new_normal_font = new CFont;
	emacs_lf.lfItalic = FALSE;
	if( !new_normal_font->CreateFontIndirect( &emacs_lf ) )
		{
		_dbg_msg( u_str("CreateFontIndirect failed %d"), GetLastError() );
		return 0;
		}

	CFont *new_italic_font = new CFont;
	emacs_lf.lfItalic = TRUE;
	if( !new_italic_font->CreateFontIndirect( &emacs_lf ) )
		{
		delete new_normal_font;
		_dbg_msg( u_str("CreateFontIndirect failed %d"), GetLastError() );
		return 0;
		}

	emacs_lf.lfItalic = FALSE;

	CDC *pDC = GetDC();

	if( emacs_normal_font )
		delete emacs_normal_font;
	if( emacs_italic_font )
		delete emacs_italic_font;
 
	emacs_normal_font = new_normal_font;
	emacs_italic_font = new_italic_font;
	old_font = pDC->SelectObject( emacs_normal_font );
	pDC->GetOutputTextMetrics( &textmetric );
	pDC->SelectObject( old_font );
	ReleaseDC( pDC );

	nCharHeight = (int)(textmetric.tmExternalLeading + textmetric.tmHeight);
	nCharWidth = textmetric.tmAveCharWidth
		+ (textmetric.tmMaxCharWidth - textmetric.tmAveCharWidth)*nCharExtra/100;

	for( i=0; i<MSCREENWIDTH; i++ )
		char_widths[i] = nCharWidth;

	if( have_focus )
		{
		HideCaret();
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		SetCaretPos( CPoint( CARET_XPOS, CARET_YPOS ) );
		ShowCaret();
		}

	tt->t_length = 4;
	tt->t_width = 50;

	CRect rect;
	GetClientRect( &rect );
	InvalidateRect( &rect );
	OnSize( SIZE_RESTORED, rect.Width(), rect.Height() );

	return 1;
	}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView diagnostics

#ifdef _DEBUG
void CWinemacsView::AssertValid() const
{
	CView::AssertValid();
}

void CWinemacsView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWinemacsDoc* CWinemacsView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWinemacsDoc)));
	return (CWinemacsDoc*) m_pDocument;
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView message handlers


void input_char_string( unsigned char *keys )
	{
	int ch;

	while( (ch = *keys++ ) != 0 )
		{
		if( ch == '\200' )
			ch = *keys++ - '\200';
		keyboard_interrupt( tt, ch );
		}
	}

afx_msg BOOL CWinemacsView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message)
	{
	if( nHitTest == HTCLIENT )
		{
//		if( activity_character == 'i' )
			// back to the normal cursor
			SetCursor( theApp.normal_text_cursor );
//		else
			// display the wait cursor
//			SetCursor( theApp.busy_cursor );
		return TRUE;
		}

	return CView::OnSetCursor(pWnd, nHitTest, message);
	}

BOOL CWinemacsView::OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo)
	{
	if( nCode == CN_COMMAND && nID >= ID_USERMENU_FIRST && nID <= ID_USERMENU_LAST )
		{
		unsigned char *id_string = menu_id_to_id_string( nID );
		if( id_string != NULL )
			{
			input_char_string( u_str(MENU_PREFIX) );
			input_char_string( id_string );
			}
		return 1;
		}   

	return CView::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
	}

void CWinemacsView::OnStopExecution()
{
	illegal_operation();
}

void win_update_line( struct trmcontrol *tt, struct emacs_line *old_line, struct emacs_line *new_line, int row )
	{
	theView->win_update_line( old_line, new_line, row );
	}

static struct emacs_line empty_line =
	{
	0,	 		/* hash */
	0,			/* next */
	0,			/* drawcost */
	0,			/* length */
	0,			/* highlighted */
	};

void CWinemacsView::win_update_line( struct emacs_line *old_line, struct emacs_line *new_line, int row )
	{
	int last_attr,start_col,end_col;

	if( new_line == NULL )
		new_line = &empty_line;


	/* only write the line if its changed or there is no old line */
	if( old_line != NULL &&
	new_line->line_length == old_line->line_length
	&& memcmp( new_line->line_body, old_line->line_body, new_line->line_length ) == 0
	&& memcmp( new_line->line_attr, old_line->line_attr, new_line->line_length ) == 0 )
		return;

	if( !update_begin() )
		return;

	/*
	 *	Fill out the line with spaces
	 */
	memset
	(
	new_line->line_body + new_line->line_length,
	' ',
	tt->t_width - new_line->line_length
	);
	/* and an attribute of normal */
	memset
	(
	new_line->line_attr + new_line->line_length,
	0,
	tt->t_width - new_line->line_length
	);


	start_col = 0;
	end_col = 1;
	while( start_col < tt->t_width )
		{
		C_color_attr *attr;
		COLORREF old_colour;

		last_attr = new_line->line_attr[start_col];
		while( end_col < tt->t_width && last_attr == new_line->line_attr[end_col] )
			end_col++;

		if( last_attr&LINE_M_ATTR_HIGHLIGHT )
			attr = &attr_high;
		else if( last_attr&LINE_ATTR_USER )
			attr = &attr_array[last_attr&LINE_M_ATTR_USER];
		else
			attr = &attr_array[last_attr];
		old_colour = pDC->SetTextColor( attr->foreground );
		old_colour = pDC->SetBkColor( attr->background );

		if( attr->italic )
			pDC->SelectObject( emacs_italic_font );
		pDC->ExtTextOut
		(
		WINDOW_BORDER+(start_col*nCharWidth), (row-1)*nCharHeight,
		0, NULL,
		s_str(&new_line->line_body[start_col]), end_col - start_col,
		&char_widths[0]
		);
		if( attr->italic )
			pDC->SelectObject( emacs_normal_font );
		if( attr->underline )
			{
			pDC->SelectObject( attr->pen );
			pDC->MoveTo
			(
			WINDOW_BORDER+(start_col*nCharWidth),
			(row)*nCharHeight - textmetric.tmDescent + 1
			);
			pDC->LineTo
			(
			WINDOW_BORDER+(end_col*nCharWidth),
			(row)*nCharHeight - textmetric.tmDescent + 1
			);
			}
		start_col = end_col;
		end_col++;
		}

	update_end();
	}

static void update_begin(struct trmcontrol *tt)
	{
	theView->update_begin();
	}

int CWinemacsView::update_begin( void )
	{
	int n;

	/* cannot proceed if the font is not selected */	
	if( emacs_normal_font == NULL )
		return 0;

	/* if the position of the scroll bar needs updating then do it */
	n = (int)((float)V_SCROLL_RANGE * (float)dot / (float)(num_characters+1));
	if( last_v_scroll_pos != (int)n )
		{
		last_v_scroll_pos = (int)n;
		SetScrollPos( SB_VERT, last_v_scroll_pos, TRUE );
		}

	/* if the position of the scroll bar needs updating then do it */
	if( wn_cur != NULL && last_h_scroll_pos != wn_cur->w_horizontal_scroll )
		{
		last_h_scroll_pos = wn_cur->w_horizontal_scroll;
		SetScrollPos( SB_HORZ, last_h_scroll_pos, TRUE );
		}

	if( have_hdc == 0 )
		{
		pDC = GetDC();
		emacs_setup_dc();
		HideCaret();

		attr_high = region_rendition;
		attr_array[SYNTAX_DULL] = window_rendition;
		attr_array[LINE_ATTR_MODELINE] = mode_line_rendition;
		attr_array[SYNTAX_WORD] = syntax_word_rendition;
		attr_array[SYNTAX_STRING] = syntax_string_rendition;
		attr_array[SYNTAX_COMMENT] = syntax_comment_rendition;
		attr_array[SYNTAX_KEYWORD1] = syntax_keyword1_rendition;
		attr_array[SYNTAX_KEYWORD2] = syntax_keyword2_rendition;
		attr_array[LINE_ATTR_USER+1] = user_1_rendition;
		attr_array[LINE_ATTR_USER+2] = user_2_rendition;
		attr_array[LINE_ATTR_USER+3] = user_3_rendition;
		attr_array[LINE_ATTR_USER+4] = user_4_rendition;
		attr_array[LINE_ATTR_USER+5] = user_5_rendition;
		attr_array[LINE_ATTR_USER+6] = user_6_rendition;
		attr_array[LINE_ATTR_USER+7] = user_7_rendition;
		attr_array[LINE_ATTR_USER+8] = user_8_rendition;
 		}
	have_hdc++;    
	return 1;
	}

void update_end(struct trmcontrol *tt)
	{
	theView->update_end();
	}

void CWinemacsView::update_end( void )
	{
	have_hdc--;
	if( have_hdc == 0 )
		{
		SetCaretPos( CPoint( CARET_XPOS, CARET_YPOS ) );
		emacs_reset_dc();
		ShowCaret();
		// get rid of any special pens
		pDC->SelectStockObject( BLACK_PEN );
		ReleaseDC( pDC );
		}
	}

static void win_topos (struct trmcontrol *tt, int row, int column)
	{
	cursor_x = column-1;
	cursor_y = row-1;
	}

#if MEMMAP
int window_size;

void window_range( struct trmcontrol *tt, int n )
	{
	window_size = n;
	} 
	
static void insert_lines( struct trmcontrol *tt, int n )
	{
	theView->insert_lines( n );
	}

void CWinemacsView::insert_lines( int n )
	{
	RECT scroll_rect;

	scroll_rect.top = cursor_y * nCharHeight;
	scroll_rect.bottom = window_size * nCharHeight;
	scroll_rect.left = 0;
	scroll_rect.right = tt->t_width * nCharWidth + 2*WINDOW_BORDER;

	pDC->ScrollDC( 0, n*nCharHeight, &scroll_rect, &scroll_rect, NULL, NULL );
	}

static void delete_lines( struct trmcontrol *tt, int n )
	{
	theView->delete_lines( n );
	}

void CWinemacsView::delete_lines( int n )
	{
	RECT scroll_rect;

	scroll_rect.top = cursor_y * nCharHeight;
	scroll_rect.bottom = window_size * nCharHeight;
	scroll_rect.left = 0;
	scroll_rect.right = tt->t_width * nCharWidth;

	pDC->ScrollDC( 0, -n*nCharHeight, &scroll_rect, &scroll_rect, NULL, NULL );
	}
#endif

static void cleanup( struct trmcontrol *tt )
	{
	}

static void wipe_line (struct trmcontrol *tt, int line)
	{
	theView->wipe_line( line );
	}

void CWinemacsView::wipe_line(int line)
	{
	RECT rect;

	rect.top = line * nCharHeight;
	rect.bottom = (line+1) *nCharHeight ;
	rect.left = 0;
	rect.right = tt->t_width * nCharWidth;

	pDC->FillRect( &rect, CBrush::FromHandle(white_brush) );
	}

static void init( struct trmcontrol *tt )
	{
	}

void ring_the_bell( struct trmcontrol *tt )
	{
	::MessageBeep( 0 );
	}

static void null_select
	( struct trmcontrol *tt, unsigned char *a, unsigned char *b, unsigned char *c)
	{
	return;
	}

static void null_tt_routine( struct trmcontrol *tt )
	{
	return;
	}

static void win_reset( struct trmcontrol *tt )
	{
	theView->win_reset( tt );
	}

void CWinemacsView::win_reset( struct trmcontrol *tt )
	{
	if( !update_begin() )
		return;

	CRect rect;
	GetClientRect( &rect );
	pDC->FillRect( &rect, CBrush::FromHandle(white_brush) );

	if( have_focus )
		{
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		}

	update_end();
	}

static void null_int_routine( struct trmcontrol *tt, int mode )
	{
	return;
	}

static void null_void_routine(struct trmcontrol *tt)
	{
	return;
	}

void win_printf( struct trmcontrol *tt, unsigned char *fmt, ... )
	{
	unsigned char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = do_print( fmt, &argp, buf, sizeof( buf ) );

	_dbg_msg( u_str("%s"), buf );
	}
	
int init_char_terminal(struct trmcontrol *tt, unsigned char *device )
	{
	return 0;
	}
	
static void change_attributes( struct trmcontrol *tt )
	{
	theView->win_change_attributes();
	}

void CWinemacsView::win_change_attributes(void)
	{
	if( !update_begin() )
		return;

	if( have_focus )
		{
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		}

	update_end();
	}

static void win_display_activity( struct trmcontrol *tt, unsigned char ch )
	{
	theView->win_display_activity( ch );
	}

void CWinemacsView::win_display_activity( unsigned char ch )
	{
	if( activity_indicator )
		{
		if( !update_begin() )
			return;

		pDC->SetTextColor( attr_array[SYNTAX_WORD].foreground );
		pDC->SetBkColor( attr_array[SYNTAX_WORD].background );

		pDC->ExtTextOut
		(
		WINDOW_BORDER, (tt->t_length-1)*nCharHeight,
		0, NULL,
		s_str(&ch), 1,
		&char_widths[0]
		);

		update_end();
		}
	}

int init_gui_terminal(struct trmcontrol *tt, unsigned char *device )
	{
	tt->t_topos = win_topos;
	tt->t_reset = win_reset;
	tt->t_update_begin = update_begin;
	tt->t_update_end = update_end;
	tt->t_update_line = win_update_line;
	tt->t_select = null_select;
	tt->t_insert_mode = null_int_routine;
	tt->t_highlight_mode = null_int_routine;
	tt->t_blanks = null_int_routine;
	tt->t_wipe_screen = null_void_routine;
	tt->t_change_attributes = change_attributes;
	tt->t_init = init;
	tt->t_cleanup = cleanup;
	tt->t_beep = ring_the_bell;
	tt->t_wipe_line = wipe_line;
	tt->k_check_for_input = win_check_for_activity;
	tt->t_window = window_range;
	tt->t_inslines = insert_lines;
	tt->t_dellines = delete_lines;
	tt->t_display_activity = win_display_activity;

	tt->t_il_mf = 0;
	tt->t_il_ov = 10;

	tt->t_io_flush = null_void_routine;
	tt->t_io_printf = win_printf;

	// create a new (empty) document
	((CWinemacsApp *)(AfxGetApp()))->emacs_new_document();

	return 1;
	}

void CWinemacsView::emacs_setup_dc()
	{
	/* Set the background mode to opaque, and select the font. */
	pDC->SetBkMode( OPAQUE );
	ASSERT( emacs_normal_font != NULL );
	ASSERT( emacs_italic_font != NULL );
	old_font = pDC->SelectObject( emacs_normal_font );
	}

void CWinemacsView::emacs_reset_dc()
	{
	pDC->SelectObject(old_font);
	}


void CWinemacsView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	last_window_size = nType;
	// some times called before things are configured
	if( emacs_normal_font == NULL )
		return;

	// some calls to this routine contain zero sizes
        if( cx == 0 || cy == 0 )
        	return;

	switch( nType )
	{
	case SIZE_MAXIMIZED:
	case SIZE_RESTORED:
		{
		int old_length;
		int old_width;
		old_length = tt->t_length;
		old_width = tt->t_width;

		tt->t_width = (cx - 2*WINDOW_BORDER)/nCharWidth;
		if( tt->t_width > MSCREENWIDTH )
			tt->t_width = MSCREENWIDTH;
		tt->t_length = cy/nCharHeight;
		if( tt->t_length > MSCREENLENGTH )
			tt->t_length = MSCREENLENGTH;
		if( tt->t_length < 3 )
			tt->t_length = 3;
		term_width = tt->t_width;
		term_length = tt->t_length;

		if( tt->t_length != old_length
		|| tt->t_width != old_width )
			tt->t_geometry_change( tt );
		}
		break;  
	case SIZE_MINIMIZED:
	default:
		break;
	}
}

#ifndef KF_EXTENDED
#define KF_EXTENDED (1<<24)
#endif                            
UINT last_key_down_flags;

void CWinemacsView::OnChar(UINT ch, UINT nRepCnt, UINT nFlags)
{                         
	if( m_ignore_next_char )
		return;
	if( ch == VK_RETURN && (last_key_down_flags&KF_EXTENDED) != 0 )
		input_char_string( u_str("\033OM") );
	else if( ch == ' ' && GetKeyState( VK_CONTROL ) < 0 )
		input_char_string( u_str("\200\200") );
	else
		{
		keyboard_interrupt( tt, ch );
		}
}

BOOL CWinemacsApp::PreTranslateMessage( MSG *pMsg )
	{  
	UINT vk = pMsg->wParam;
	UINT flags = (UINT)pMsg->lParam;

	if( (pMsg->message == WM_SYSKEYUP || pMsg->message == WM_SYSKEYDOWN)
	&& vk == VK_F10 )
		{
		if( pMsg->message == WM_SYSKEYDOWN )
			{
			last_key_down_flags = flags;

			if( vk < NUM_CONV_KEY
			&& convert[vk].key_code )
				{
				if( convert[vk].key_code != vk )
					fatal_error( 76 );

		 		if( convert[vk].ctrl_translation != NULL
				&& GetKeyState( VK_CONTROL ) < 0 )
					input_char_string( u_str(convert[vk].ctrl_translation) );
				else if( convert[vk].enh_translation != NULL
				&& flags&KF_EXTENDED )
					input_char_string( u_str(convert[vk].enh_translation) );
				else if( convert[vk].shift_translation != NULL
				&& GetKeyState( VK_SHIFT ) < 0 )
					input_char_string( u_str(convert[vk].shift_translation) );
				else if( convert[vk].translation != NULL )
					input_char_string( u_str(convert[vk].translation) );
				else
					{
					special_key[3] = (unsigned char)vk;
					input_char_string( u_str(special_key) );
					}
				}
			}
		return 1;
		}
		
	return CWinApp::PreTranslateMessage( pMsg );
	}

void CWinemacsView::OnKeyDown(UINT vk, UINT nRepCnt, UINT flags)
{
	last_key_down_flags = flags;

	if( vk < NUM_CONV_KEY
	&& convert[vk].key_code )
		{
		if( convert[vk].key_code != vk )
			fatal_error( 76 );

 		if( convert[vk].ctrl_translation != NULL
		&& GetKeyState( VK_CONTROL ) < 0 )
			input_char_string( u_str(convert[vk].ctrl_translation) );
		else if( convert[vk].enh_translation != NULL
		&& flags&KF_EXTENDED )
			input_char_string( u_str(convert[vk].enh_translation) );
		else if( convert[vk].shift_translation != NULL
		&& GetKeyState( VK_SHIFT ) < 0 )
			input_char_string( u_str(convert[vk].shift_translation) );
		else if( convert[vk].translation != NULL )
			input_char_string( u_str(convert[vk].translation) );
		else
			{
			special_key[3] = (unsigned char)vk;
			input_char_string( u_str(special_key) );
			}
		m_ignore_next_char = TRUE;
		}
	
	CView::OnKeyDown(vk, nRepCnt, flags);
}

void CWinemacsView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CView::OnKeyUp(nChar, nRepCnt, nFlags);
	m_ignore_next_char = FALSE;
}


void CWinemacsView::OnEditCopy()
	{
	input_char_string( u_str( MENU_PREFIX "ec" ) );
	}


void CWinemacsView::OnEditCut()
	{
	input_char_string( u_str( MENU_PREFIX "ex" ) );
	}

void CWinemacsView::OnEditClear()
{
	input_char_string( u_str( MENU_PREFIX "er" ) );
}

void CWinemacsView::OnUpdateEditClear(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( bf_cur->b_mark != NULL );
}

void CWinemacsView::OnEditPaste()
	{
	input_char_string( u_str( MENU_PREFIX "ev" ) );
	}

void CWinemacsView::OnOptionsColors()
	{
	CColourOpt colours;
	
	if( colours.DoModal() == IDOK )
		{
		colours.save_options();
	
		// redraw the screen in case the colours have changed
		screen_garbaged = 1;
		do_dsp(0);
		}
	}

void CWinemacsView::OnOptionsDirectories()
	{
	CDirOpt dir_opt;
	
	if( dir_opt.DoModal() == IDOK )
		dir_opt.save_options();
	}

void CWinemacsView::OnOptionsFont()
	{
	char buf[20];

	if (! ChooseFont(&emacs_cf))
		return;

	WritePrivateProfileString
	( "Fonts", "TextFontName", emacs_cf.lpLogFont->lfFaceName, theApp.m_pszProfileName );
	wsprintf( buf, "%d", emacs_cf.lpLogFont->lfHeight );
	WritePrivateProfileString
	( "Fonts", "TextFontSize", buf, theApp.m_pszProfileName );

	new_font();
	}

void CWinemacsView::OnSearchFind()
{
	input_char_string( u_str( MENU_PREFIX "es" ) );
}

void CWinemacsView::OnSearchReplace()
{
	input_char_string( u_str( MENU_PREFIX "ef" ) );
}

void CWinemacsView::OnDropFiles(HDROP hDropInfo)
{
	char file_name[256];
	
	unsigned int num_files = ::DragQueryFile( hDropInfo, 0xffffffff, NULL, 0 );
	for( unsigned int i=0; i<num_files; i++ )
		{                                                        
		int len = ::DragQueryFile( hDropInfo, i, file_name, sizeof( file_name ) );
		file_name[len] = '\0';
		
		char command_line[300];
		strcpy( command_line, "(UI-drop-file \"" );
		char *op = &command_line[strlen(command_line)];
		char *ip = file_name;
		while( *ip )
			{
			*op++ = *ip;
			if( *ip++ == '\\' )
				*op++ = '\\';
			}
		*op = '\0';
		strcat( command_line, "\")" );

		execute_mlisp_string( u_str(command_line) );
		}
	::DragFinish(hDropInfo);
	do_dsp(0);
}


void CWinemacsView::OnUpdateSearchFind(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( find_dlg == NULL );
}

void CWinemacsView::OnUpdateSearchReplace(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( find_dlg == NULL );
}

static void work_synchronise_files( struct emacs_work_item *work )
	{
	synchronise_files();
	}

/* work structure to queue on a timeout */
static struct emacs_work_item synchronise_files_request =
	{
	{ NULL, NULL },
	work_synchronise_files, 0, NULL
	};

void CWinemacsView::OnSetFocus(CWnd* pOldWnd)
{
	CView::OnSetFocus(pOldWnd);

	CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
	SetCaretPos( CPoint( CARET_XPOS, CARET_YPOS ) );
	ShowCaret();

	have_focus = 1;

	if( synchronise_files_request.header.next == NULL )
		work_add_item( &synchronise_files_request );
}

void CWinemacsView::OnKillFocus(CWnd* pNewWnd)
{
	CView::OnKillFocus(pNewWnd);
	
	if( have_focus )
		{
		HideCaret();
		::DestroyCaret();
		}
	have_focus = 0;
}

void CWinemacsView::OnUpdateEditPaste(CCmdUI* pCmdUI)
{
	if( !OpenClipboard() )
		return;

	pCmdUI->Enable( IsClipboardFormatAvailable( CF_TEXT )
			|| IsClipboardFormatAvailable( CF_OEMTEXT ) );
	::CloseClipboard();
}

void CWinemacsView::OnUpdateEditCopy(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( bf_cur->b_mark != NULL );
}

void CWinemacsView::OnUpdateEditCut(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( bf_cur->b_mark != NULL );
}

void CWinemacsView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int scroll = wn_cur->w_horizontal_scroll;

	switch( nSBCode )
	{
	case SB_LINEUP:
		scroll -= 1; break;
	case SB_LINEDOWN:
		scroll += 1; break;
	case SB_PAGEUP:
		scroll -= 8; break;
	case SB_PAGEDOWN:
		scroll += 8; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		scroll = nPos; break;
	case SB_TOP:
		scroll = 1; break;
	case SB_BOTTOM:
		scroll = H_SCROLL_RANGE; break;
	}

	// prevent under flow of the horizontal offset
	if( scroll < 1 )
		scroll = 1;

	/* if the position of the scroll bar needs updating then do it */
	if( last_h_scroll_pos != scroll )
		{
		last_h_scroll_pos = scroll;
		SetScrollPos( SB_HORZ, last_h_scroll_pos, TRUE );
		}

	wn_cur->w_horizontal_scroll = scroll;

	cant_1line_opt = 1;
	redo_modes = 1;

	do_dsp(0);
}

void CWinemacsView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int scroll = 0;       
	int n;
	int old_scroll_step = scroll_step;

	switch( nSBCode )
	{
	case SB_LINEUP:
		scroll = -2; break;
	case SB_LINEDOWN:
		scroll = 1; break;
	case SB_PAGEUP:
		scroll = -wn_cur->w_height * 4 / 5; break;
	case SB_PAGEDOWN:
		scroll = wn_cur->w_height * 4 / 5; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		n = (int)((float)num_characters * (float)nPos / (float)V_SCROLL_RANGE);
		if (n < 1)
			n = 1;
		if (n > num_characters)
			n = num_characters + 1;
		if( n != (num_characters + 1) )
			set_dot( scan_bf('\n', n, -1) );
		break;
	case SB_TOP:
		set_dot( 1 ); break;
	case SB_BOTTOM:
		set_dot( num_characters ); break;
	}

	if( scroll )
		{
		set_dot( scan_bf('\n', dot, scroll) );
		set_mark(wn_cur->w_start, wn_cur->w_buf,
			scan_bf('\n', to_mark(wn_cur->w_start), scroll), 0);
		}

	/* if the position of the scroll bar needs updating then do it */
	n = (int)((float)V_SCROLL_RANGE * (float)dot / (float)(num_characters+1));
	if( last_v_scroll_pos != (int)n )
		{
		last_v_scroll_pos = (int)n;
		SetScrollPos( SB_VERT, last_v_scroll_pos, TRUE );
		}

	scroll_step = 1;
	do_dsp(0);
	scroll_step = old_scroll_step;
}

void CWinemacsView::OnFileOpen()
{
	input_char_string( u_str( MENU_PREFIX "fo" ) );
}

void CWinemacsView::OnFileSave()
{
	input_char_string( u_str( MENU_PREFIX "fs" ) );
}

void CWinemacsView::OnFileSaveAs()
{
	input_char_string( u_str( MENU_PREFIX "fa" ) );
}

void CWinemacsView::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable( bf_cur->b_modified );
}

void CWinemacsView::OnFileSaveEnvironment()
{
	input_char_string( u_str( MENU_PREFIX "fe" ) );
}

void CWinemacsView::OnLButtonDown(UINT nFlags, CPoint point)
{
	mouse_button_state |= MOUSE_BUTTON_1;
	OnMouseButton( nFlags, point, 2 );
//	CView::OnLButtonDown(nFlags, point);
}

void CWinemacsView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	mouse_button_state |= MOUSE_BUTTON_1;
	OnMouseButton( nFlags, point, 2 );
//	CView::OnLButtonDblClk(nFlags, point);
}

void CWinemacsView::OnLButtonUp(UINT nFlags, CPoint point)
{
	mouse_button_state &= ~MOUSE_BUTTON_1;
	OnMouseButton( nFlags, point, 3 );
//	CView::OnLButtonUp(nFlags, point);
}

void CWinemacsView::OnRButtonDown(UINT nFlags, CPoint point)
{
	mouse_button_state |= MOUSE_BUTTON_3;
	OnMouseButton( nFlags, point, 6 );
//	CView::OnRButtonDown(nFlags, point);
}

void CWinemacsView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	mouse_button_state |= MOUSE_BUTTON_3;
	OnMouseButton( nFlags, point, 6 );
//	CView::OnRButtonDblClk(nFlags, point);
}

void CWinemacsView::OnRButtonUp(UINT nFlags, CPoint point)
{
	mouse_button_state &= ~MOUSE_BUTTON_3;
	OnMouseButton( nFlags, point, 7 );
	CView::OnRButtonUp(nFlags, point);
}

void CWinemacsView::OnMouseButton( UINT nFlags, CPoint point, int button )
	{
	unsigned char mouse[64];

	/*
	 *	The mouse control sequence is
	 *	CSI event ; ; y ; x &w
	 */
	if( mouse_enable )
		{
		int mouse_x = (point.x - WINDOW_BORDER)/nCharWidth + 1;
		if( mouse_x <= 0 )
			mouse_x = 1;
		if( mouse_x > tt->t_width )
			mouse_x = tt->t_width;
		int mouse_y = point.y/nCharHeight + 1;

		sprintfl( mouse, sizeof(mouse), u_str("\x1b[%d;%d;%d;%d&w"), button, mouse_button_state, mouse_y, mouse_x );
		input_char_string( mouse );
		}
	}


void CWinemacsView::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	unsigned char mouse[64];

	/*
	 *	The mouse control sequence is
	 *	CSI event ; ; y ; x &W
	 */
	if( mouse_enable&2		// enabled and
	&& mouse_button_state != 0 )	// a button down
		{
		int mouse_x = (point.x - WINDOW_BORDER)/nCharWidth + 1;
		if( mouse_x <= 0 )
			mouse_x = 1;
		if( mouse_x > tt->t_width )
			mouse_x = tt->t_width;
		int mouse_y = point.y/nCharHeight + 1;

		sprintfl( mouse, sizeof(mouse), u_str("\x1b[%d;%d;%d;%d&W"), 0, mouse_button_state, mouse_y, mouse_x );
		input_char_string( mouse );
		}
//	CView::OnMouseMove(nFlags, point);
}

static void (*timeout_handle)(void);
void time_schedule_timeout( void (*time_handle_timeout)(void), int delta )
	{
	timeout_handle = time_handle_timeout;

	//	start the timer
	UINT status = theView->SetTimer( 'A', delta, NULL );
	if( status == 0 )
		{
		TRACE("Failed to allocate timer\n");
		return;
		}
	}

void time_cancel_timeout(void)
	{
	theView->KillTimer( 'A' );
	}

void CWinemacsView::OnTimer(UINT nIDEvent)
{
	if( nIDEvent == 'Z' )
		m_tick = 1;
	else
		if( timeout_handle )
			timeout_handle();			

	CView::OnTimer(nIDEvent);
}

extern "C" HANDLE _osfhnd[];

time_t fio_modify_date( FILE *file )
	{
	FILETIME filetime;

	int err = 0;
	if( !GetFileTime( _osfhnd[fileno(file)], NULL, NULL, &filetime ) )
		{
		err = GetLastError();
		return 0;
		}

	CTime c_time( filetime );
	time_t time = c_time.GetTime();

	return time;
	}

CDocument* CWinemacsApp::OpenDocumentFile(LPCSTR lpszFileName)
	{
	_str_cpy( ui_open_file_name, lpszFileName );
	input_char_string( u_str( MENU_PREFIX "fr" ) );

	return NULL;
	}


void CWinemacsView::OnOptionsFileParsing() 
{
	COptionFilenameParsing file_parsing;

	if( file_parsing.DoModal() == IDOK )
		{
		file_parsing.save_options();
	
		// redraw the screen in case the colours have changed
		screen_garbaged = 1;
		do_dsp(0);
		}
		
}
