//
// win_view.cpp : implementation of the CWinemacsView class
//

#include <emacs.h>
#include <win_emacs.h>
#include <win_registry.h>

#include <win_doc.h>
#include <win_view.h>
#include <win_find.h>
#include <win_opt.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#define DEBUG_SCROLL_LINES 0

int display_scroll_bars = 1;
extern int vertical_bar_width;

CFindDlg *find_dlg;

const int WINDOW_BORDER( 1 );
const int V_SCROLL_RANGE( 10000 );
const int H_SCROLL_RANGE( 256 );

#define CARET_WIDTH	(term_cursor_mode ? (nCharWidth+1) : 2)
#define CARET_HEIGHT	nCharHeight
#define CARET_XPOS	((cursor_x*nCharWidth)+WINDOW_BORDER+(term_cursor_mode ? 0 : -1))
#define CARET_YPOS	(cursor_y*nCharHeight)

static int cursor_x;
static int cursor_y;



static unsigned short C0_map[32] =
	{
	0x2666,	// @ -> Diamond
	0x2401,	// A
	0x2192,	// B -> ^I HT
	0x2021,	// C -> ^L FF
	0x2518,	// D -> ^M CR
	0x2405,	// E
	0x2406,	// F
	0x2407,	// G
	0x2193,	// H -> NL
	0x2020,	// I -> ^K VT
	0x240a,	// J
	0x240b,	// K
	0x240c,	// L
	0x240d,	// M
	0x240e,	// N
	0x240f,	// O
	0x2410,	// P
	0x2411,	// Q
	0x2412,	// R
	0x2413,	// S
	0x2414,	// T
	0x2415,	// U
	0x2416,	// V
	0x2417,	// W
	0x2418,	// X
	0x2419,	// Y
	0x2248,	// Z -> Line Wrap
	0x241b,	// [
	0x241c,	// |
	0x241d,	// ]
	0x0387,	// ^ -> Center Dot
	0x2260	// _ ->  Line Tructate
	};

static char special_key[] = "\033__X";

struct conv_key_entry
	{
	void translate( bool ctrl, bool shift ) const;

	WORD key_code;
	bool lk201_keypad;
	bool is_enhanced_key;
	bool always_use_shift_translation;
	char *translation;
	char *shift_translation;
	char *ctrl_translation;
	char *ctrl_shift_translation;
	};

class conv_keys
	{
public:
	conv_keys( conv_key_entry *entries, int sizeof_entries )
		: m_entries( entries )
		, m_num_entries( sizeof_entries / sizeof( conv_key_entry ) )
		{}
	~conv_keys()
		{}

	bool findEntryAndTranslate( bool enh_key, UINT vk, UINT repcnt ) const;

private:
	const conv_key_entry *findEntry( bool enh_key, UINT vk ) const;

	conv_keys( const conv_keys & );
	conv_keys &operator=( const conv_keys & );

	const conv_key_entry *m_entries;
	const int m_num_entries;
	};


static conv_key_entry convert_entries[] =
	{
//	key_code	lk201  enh     always  translation      shift_tran      ctrl_tran       ctrl_shift_tran
//	--------        -----  ---     ------  -----------      ----------      ---------       ---------------
	VK_BACK,	false, false,  false,  "\x7f",		NULL,		"\033[85~",	NULL,
	VK_TAB,		false, false,  true,   "\t",		"\033\t",	"\t",		"\033\t",
	VK_CLEAR,	true,  false,  false,  "\033Ou",	NULL,		NULL,		NULL,
	VK_PAUSE,	false, false,  false,  "\033[34~",	"\033[134~",	"\033[84~",	"\033[184~",

	// kp0 to kp9 when num-lock is on
	VK_NUMPAD0,	true,  false,  false,  "\033Op",	NULL,		NULL,		NULL,
	VK_NUMPAD1,	true,  false,  false,  "\033Oq",	NULL,		NULL,		NULL,
	VK_NUMPAD2,	true,  false,  false,  "\033Or",	NULL,		NULL,		NULL,
	VK_NUMPAD3,	true,  false,  false,  "\033Os",	NULL,		NULL,		NULL,
	VK_NUMPAD4,	true,  false,  false,  "\033Ot",	NULL,		NULL,		NULL,
	VK_NUMPAD5,	true,  false,  false,  "\033Ou",	NULL,		NULL,		NULL,
	VK_NUMPAD6,	true,  false,  false,  "\033Ov",	NULL,		NULL,		NULL,
	VK_NUMPAD7,	true,  false,  false,  "\033Ow",	NULL,		NULL,		NULL,
	VK_NUMPAD8,	true,  false,  false,  "\033Ox",	NULL,		NULL,		NULL,
	VK_NUMPAD9,	true,  false,  false,  "\033Oy",	NULL,		NULL,		NULL,
	VK_DECIMAL,	true,  false,  false,  "\033On",	NULL,		NULL,		NULL,

	// kp0 to kp9 when num-lock is off
	VK_INSERT,	true,  false,  false,  "\033Op",	NULL,		NULL,		NULL,
	VK_END,		true,  false,  false,  "\033Oq",	NULL,		NULL,		NULL,
	VK_DOWN,	true,  false,  false,  "\033Or",	NULL,		NULL,		NULL,
	VK_NEXT,	true,  false,  false,  "\033Os",	NULL,		NULL,		NULL,
	VK_LEFT,	true,  false,  false,  "\033Ot",	NULL,		NULL,		NULL,
	// kp5 
	VK_RIGHT,	true,  false,  false,  "\033Ov",	NULL,		NULL,		NULL,
	VK_HOME,	true,  false,  false,  "\033Ow",	NULL,		NULL,		NULL,
	VK_UP,		true,  false,  false,  "\033Ox",	NULL,		NULL,		NULL,
	VK_PRIOR,	true,  false,  false,  "\033Oy",	NULL,		NULL,		NULL,
	VK_DELETE,	true,  false,  false,  "\033On",	NULL,		NULL,		NULL,

	VK_MULTIPLY,	true,  false,  false,  "\033OR",	NULL,		NULL,		NULL,
	VK_ADD,		true,  false,  false,  "\033Ol",	"\033Om",	NULL,		NULL,
	VK_SUBTRACT,	true,  false,  false,  "\033OS",	NULL,		NULL,		NULL,
	VK_DIVIDE,	true,  true,   false,  "\033OQ",	NULL,		NULL,		NULL,

	// function keys
	VK_F1,		false, false,  false,  "\033[17~",	"\033[117~",	"\033[67~",	"\033[167~",
	VK_F2,		false, false,  false,  "\033[18~",	"\033[118~",	"\033[68~",	"\033[168~",
	VK_F3,		false, false,  false,  "\033[19~",	"\033[119~",	"\033[69~",	"\033[169~",
	VK_F4,		false, false,  false,  "\033[20~",	"\033[120~",	"\033[70~",	"\033[170~",
	VK_F5,		false, false,  false,  "\033[21~",	"\033[121~",	"\033[71~",	"\033[171~",
	VK_F6,		false, false,  false,  "\033[23~",	"\033[123~",	"\033[73~",	"\033[173~",
	VK_F7,		false, false,  false,  "\033[24~",	"\033[124~",	"\033[74~",	"\033[174~",
	VK_F8,		false, false,  false,  "\033[25~",	"\033[125~",	"\033[75~",	"\033[175~",
	VK_F9,		false, false,  false,  "\033[26~",	"\033[126~",	"\033[76~",	"\033[176~",
	VK_F10,		false, false,  false,  "\033[28~",	"\033[128~",	"\033[78~",	"\033[178~",
	VK_F11,		false, false,  false,  "\033[29~",	"\033[129~",	"\033[79~",	"\033[179~",
	VK_F12,		false, false,  false,  "\033[31~",	"\033[131~",	"\033[81~",	"\033[181~",
	VK_F13,		false, false,  false,  "\033_I",	"\033[O",	NULL,		NULL,
	VK_F14,		false, false,  false,  "\033_J",	"\033[P",	NULL,		NULL,
	VK_F15,		false, false,  false,  "\033_K",	"\033[Q",	NULL,		NULL,
	VK_F16,		false, false,  false,  "\033_L",	"\033[R",	NULL,		NULL,
	VK_SCROLL,	false, false,  false,  "\033[33~",	"\033[133~",	"\033[83~","\033[183~",

	// enhanced keys
	VK_PRIOR,	false, true,   false,  "\033[5~",	"\033[105~",	"\033[55~",	"\033[155~",
	VK_NEXT,	false, true,   false,  "\033[6~",	"\033[106~",	"\033[56~",	"\033[156~",
	VK_END,		false, true,   false,  "\033[4~",	"\033[104~",	"\033[54~",	"\033[154~",
	VK_HOME,	false, true,   false,  "\033[1~",	"\033[101~",	"\033[51~",	"\033[151~",

	VK_LEFT,	false, true,   false,  "\033OD",	"\033[110~",	"\033[60~",	"\033[160~",
	VK_UP,		false, true,   false,  "\033OA",	"\033[107~",	"\033[57~",	"\033[157~",
	VK_RIGHT,	false, true,   false,  "\033OC",	"\033[109~",	"\033[59~",	"\033[159~",
	VK_DOWN,	false, true,   false,  "\033OB",	"\033[108~",	"\033[58~",	"\033[158~",

	VK_INSERT,	false, true,   true,  "\033[2~",	"\033[102~",	"\033[52~",	"\033[152~",
	VK_DELETE,	false, true,   true,  "\033[3~",	"\033[103~",	"\033[53~",	"\033[153~",

	// keypad keys when num_lock is off and no lk201 emulation
	VK_PRIOR,	false, false,  false,  "\033[5~",	"\033[105~",	"\033[55~",	"\033[155~",
	VK_NEXT,	false, false,  false,  "\033[6~",	"\033[106~",	"\033[56~",	"\033[156~",
	VK_END,		false, false,  false,  "\033[4~",	"\033[104~",	"\033[54~",	"\033[154~",
	VK_HOME,	false, false,  false,  "\033[1~",	"\033[101~",	"\033[51~",	"\033[151~",

	VK_LEFT,	false, false,  false,  "\033OD",	"\033[110~",	"\033[60~",	"\033[160~",
	VK_UP,		false, false,  false,  "\033OA",	"\033[107~",	"\033[57~",	"\033[157~",
	VK_RIGHT,	false, false,  false,  "\033OC",	"\033[109~",	"\033[59~",	"\033[159~",
	VK_DOWN,	false, false,  false,  "\033OB",	"\033[108~",	"\033[58~",	"\033[158~",

	VK_INSERT,	false, false,  true,  "\033[2~",	"\033[102~",	"\033[52~",	"\033[152~",
	VK_DELETE,	false, false,  true,  "\033[3~",	"\033[103~",	"\033[53~",	"\033[153~",

	VK_NUMLOCK,	true,  true,   false,  "\033OP",	NULL,		NULL,		NULL
	};

const conv_keys convert_keys( convert_entries, sizeof( convert_entries ) );

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView

IMPLEMENT_DYNCREATE(CWinemacsView, CView)

BEGIN_MESSAGE_MAP(CWinemacsView, CView)
	//{{AFX_MSG_MAP(CWinemacsView)
	ON_WM_HELPINFO()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_CHAR()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_OPTIONS_COLORS, OnOptionsColors)
	ON_COMMAND(ID_OPTIONS_DIRECTORIES, OnOptionsDirectories)
	ON_COMMAND(ID_OPTIONS_FILE_PARSING, OnOptionsFileParsing)
	ON_COMMAND(ID_OPTIONS_FONT, OnOptionsFont)
	ON_COMMAND(ID_OPTIONS_LOGNAMES, OnOptionsLogNames)
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_BUFFER_CASEBLINDSEARCH, OnUpdateBufferCaseblindsearch)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REPLACECASE, OnUpdateEditReplaceCase)
	ON_UPDATE_COMMAND_UI(ID_BUFFER_NARROWREGION, OnUpdateBufferNarrowregion)
	ON_UPDATE_COMMAND_UI(ID_BUFFER_SHOWWHITESPACE, OnUpdateBufferShowwhitespace)
	ON_UPDATE_COMMAND_UI(ID_BUFFER_WIDENREGION, OnUpdateBufferWidenregion)
	ON_UPDATE_COMMAND_UI(ID_BUFFER_WRAPLONGLINES, OnUpdateBufferWraplonglines)
	ON_UPDATE_COMMAND_UI(ID_BUILD_NEXTERROR, OnUpdateBuildNexterror)
	ON_UPDATE_COMMAND_UI(ID_BUILD_PREVIOUSERROR, OnUpdateBuildPreviouserror)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CLEAR, OnUpdateEditClear)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_FIND, OnUpdateSearchFind)
	ON_UPDATE_COMMAND_UI(ID_SEARCH_REPLACE, OnUpdateSearchReplace)
	ON_WM_VSCROLL()
	ON_UPDATE_COMMAND_UI(ID_MACROS_RECORD, OnUpdateMacrosRecord)
	ON_UPDATE_COMMAND_UI(ID_MACROS_RUN, OnUpdateMacrosRun)
	ON_UPDATE_COMMAND_UI(ID_MACROS_STOPRECORDING, OnUpdateMacrosStoprecording)
	ON_UPDATE_COMMAND_UI(ID_VIEW_READONLY, OnUpdateViewReadonly)
	ON_COMMAND(ID_VIEW_TOOLBAR_CHOOSER, OnViewToolbar)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEALL, OnUpdateFileSaveall)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_COMMAND(ID_FILE_PRINT, OnEmacsFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, OnEmacsFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView construction/destruction

// global pointer to the only view
CWinemacsView::CWinemacsView()
	: EmacsView()
	, print_window_group( NULL )
	, print_window( NULL )
	{
	vertical_bar_width = 2;

	for( int i=0; i<max_scroll_bars; i++ )
		{
		horiz_scroll[i] = NULL;
		vert_scroll[i] = NULL;
		}
	
	scroll_bar_region_data.header.dwSize = sizeof( scroll_bar_region_data.header);
	scroll_bar_region_data.header.iType = RDH_RECTANGLES;
	scroll_bar_region_data.header.nCount = 0;
	scroll_bar_region_data.header.nRgnSize = 0;
	scroll_bar_region_data.header.rcBound.top = 0;
	scroll_bar_region_data.header.rcBound.left = 0;
	scroll_bar_region_data.header.rcBound.bottom = 0;
	scroll_bar_region_data.header.rcBound.right = 0;

	memset( (void *)&scroll_bar_region_data.rects, 0, sizeof( scroll_bar_region_data.rects ) );

	t_il_mf = 0;
	t_il_ov = 10;

	term_deccrt = 1;

	theActiveView = this;
	nCharExtra = 25;
	last_window_size = SIZEICONIC;
        
	emacs_normal_font = NULL;
	emacs_italic_font = NULL;
//	emacs_special_font = NULL;
	have_hdc = 0;
	pDC = NULL;

	m_ignore_next_char = FALSE;

	have_focus = 0;
	old_font = 0;
	white_brush = 0;

	printer_doc_normal_font = NULL;
	printer_doc_italic_font = NULL;

	printer_header_font = NULL;
	printer_footer_font = NULL;

	printer_old_font = NULL;

	mouse_button_state = 0;
	}

//
// The following function scale there parameter from twips to the DC's units
//
static int ScaleWidth( int width, const CDC *pDC )
	{
	return (width * pDC->GetDeviceCaps(LOGPIXELSX)) / 1440;
	}

static int ScaleHeight( int height, const CDC *pDC )
	{
	return (height * pDC->GetDeviceCaps(LOGPIXELSY)) / 1440;
	}

CWinemacsView::~CWinemacsView()
	{
	delete emacs_normal_font;
	delete emacs_italic_font;

	if( theActiveView == this )
		theActiveView = NULL;
	}


/////////////////////////////////////////////////////////////////////////////
//  A view's OnInitialUpdate() overrideable function is called immediately
//  after the frame window is created, and the view within the frame
//  window is attached to its document.
void CWinemacsView::OnInitialUpdate()
	{
	white_brush = (HBRUSH)::GetStockObject( WHITE_BRUSH );

	new_font();
	ASSERT( emacs_normal_font != NULL );
	ASSERT( emacs_italic_font != NULL );

	/*                         
	 *	Restore the last position of the window
	 */
	CString buf = theApp.GetProfileString( "WindowPositions", "Window_1", "" );

	WINDOWPLACEMENT where;
	if( sscanf
		(
		buf,
		"%d %d %d %d %d %d %d %d %d",
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
			_dbg_msg( FormatString("SetWindowPlacement failed  %d") << ::GetLastError() );
			}
		}
//	else
//		((CMDIFrameWnd *)AfxGetApp()->m_pMainWnd)->MDIMaximize( theView->GetParent() );

	}


/////////////////////////////////////////////////////////////////////////////
// CWinemacsView drawing

typedef EmacsView *EmacsViewPtr;
void CWinemacsView::OnDraw(CDC* pDC)
	{
	Save<EmacsViewPtr> cur_view( &theActiveView );
	theActiveView = this;

//	CWinemacsDoc* pDoc = GetDocument();
	CSize line_size( nCharWidth*t_width, nCharHeight );

	EmacsLinePtr nullLine;
	for( int n = 0; n < t_length; )
		{
		CPoint pt( 0, n*nCharHeight );
		CRect this_line( pt, line_size );

		n++;

		if( pDC->RectVisible( this_line ) )
			t_update_line( nullLine, t_phys_screen[n], n );
		}
	}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView printing

//
//	The page will print looking like this
//
//	-------------------------------------------------
//	| Header line					|
//	-------------------------------------------------
//	| 1001 | Line 1001				|
//	| 1002 | Line 1002				|
//	| 1003 | Line 1003				|
//	| 1004 | Line 1004				|
//	| 1005 | Line 1005				|
//	-------------------------------------------------
//	| Footer line					|
//	-------------------------------------------------
//
//	The borders ("|" and "-" above) are optional
//	The line numbers are optional
//	The header line is only printed if its has a definition
//	The footer line is only printed if its has a definition
//


afx_msg void CWinemacsView::OnEmacsFilePrint()
	{
	theApp.GetEmacsPrinterSetup();

	CView::OnFilePrint();

	theApp.SaveEmacsPrinterSetup();
	}

afx_msg void CWinemacsView::OnEmacsFilePrintPreview()
	{
	theApp.GetEmacsPrinterSetup();

	CView::OnFilePrintPreview();

	theApp.SaveEmacsPrinterSetup();
	}


//
//	Used to format the header and footer strings for printer pages
//
//	%p	- page number
//	%P	- max page number
//	%b	- buffer name
//	%fn	- file name and type
//	%fp	- path to file
//	%ft	- time stamp of the file
//
EmacsString CWinemacsView::ExpandPrintFormatString( const EmacsString &format, CPrintInfo *pinfo )
	{
	EmacsString output;

	for( int pos=0; pos < format.length(); )
		{
		char ch( format[pos] ); pos++;
		if( ch != '%' )
			output.append( ch );
		else
		if( pos < format.length() )
			{
			ch = format[pos]; pos++;
			switch( ch )
				{
			case 'p':
				{
				int max_page = pinfo->GetMaxPage();
				int width=3;
				if( max_page > 99 )
					width = 3;
				else if( max_page > 9 )
					width = 2;
				else
					width = 1;

				EmacsString page_number = FormatString("%*d") << width << pinfo->m_nCurPage;
				output.append( page_number );
				break;
				}
			case 'P':
				{
				EmacsString page_number = FormatString("%d") << pinfo->GetMaxPage();
				output.append( page_number );
				break;
				}
			case 'b':
				output.append( bf_cur->b_buf_name );
				break;
			case 'f':
				{
				FileParse file;
				if( bf_cur->b_kind == FILEBUFFER )
					file.sys_parse( bf_cur->b_fname, EmacsString::null );

				if( pos < format.length() )
				{
				ch = format[pos]; pos++;
				switch( ch )
					{
				case 'n':
					if( bf_cur->b_kind == FILEBUFFER )
						{
						output.append( file.filename );
						output.append( file.filetype );
						}
					else
						output.append( bf_cur->b_buf_name );
					break;
				case 'p':
					if( bf_cur->b_kind == FILEBUFFER )
						{
						output.append( file.disk );
						output.append( file.path );
						}
					break;
				case 't':
					{
					struct tm *local_file_time = localtime( &bf_cur->b_file_time );
					char buffer[80];
					strftime( buffer, sizeof( buffer ), "%d-%b-%Y %H:%M", local_file_time );
					output.append( buffer );
					}
					break;
				default:
					break;
					}
				}
				}
				break;
			default:
				break;
				}
			}
		}

	return output;
	}

BOOL CWinemacsView::OnPreparePrinting( CPrintInfo* pInfo )
	{
	// default preparation
	return DoPreparePrinting(pInfo);
	}

static void setupPrinterAttribute
	(
	EmacsWinColour &attr,
	const SystemExpressionRepresentationGraphicRendition &str,
	bool syntax_colouring,
	bool in_colour
	)
	{
	EmacsWinColour a;
	if( syntax_colouring )
		{
		a = str;
		if( !in_colour )
			a.setColour( colour_black, colour_white );
		}

	attr = a;
	}

bool CWinemacsView::initialiseViewForPrinting( CDC *print_dc, EmacsWinPrintOptions &options )
	{
	//
	// Create the fonts
	//
	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );

	const EmacsWinFont &doc_font = options.printerDocumentFont();

	lf.lfHeight = doc_font.pointSize();
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = FW_NORMAL;
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = ANSI_CHARSET;
	lf.lfOutPrecision = OUT_DEVICE_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;
	strcpy( lf.lfFaceName, doc_font.face() );

	//
	//	CReate the font and select it into the DC
	//
	printer_old_font = NULL;
	printer_doc_normal_font = new CFont();
	printer_doc_italic_font = new CFont();

	if( printer_doc_normal_font == NULL
	||  printer_doc_italic_font == NULL )
		{
		delete printer_doc_normal_font;
		delete printer_doc_italic_font;

		return false;
		}

	lf.lfItalic = TRUE;
	printer_doc_italic_font->CreatePointFontIndirect( &lf, print_dc );

	lf.lfItalic = FALSE;
	printer_doc_normal_font->CreatePointFontIndirect( &lf, print_dc );

	//
	//	Create the header font
	//
	if( options.header() )
		{
		const EmacsWinFont &font = options.printerHeaderFont();
		printer_header_font = new CFont();
		if( printer_header_font == NULL )
			{
			delete printer_doc_normal_font;
			delete printer_doc_italic_font;

			return false;
			}

		lf.lfHeight = font.pointSize();
		strcpy( lf.lfFaceName, font.face() );

		printer_header_font->CreatePointFontIndirect( &lf, print_dc );
		}

	//
	//	Create the footer font
	//
	if( options.footer() )
		{
		const EmacsWinFont &font = options.printerFooterFont();
		printer_footer_font = new CFont();
		if( printer_footer_font == NULL )
			{
			delete printer_doc_normal_font;
			delete printer_doc_italic_font;
			delete printer_header_font;

			return false;
			}

		lf.lfHeight = font.pointSize();
		strcpy( lf.lfFaceName, font.face() );

		printer_footer_font->CreatePointFontIndirect( &lf, print_dc );
		}

	//
	//	Figure out the size of the fonts on this printer
	//
	TEXTMETRIC	print_textmetric;

	//
	// Document Font
	//
	printer_old_font = print_dc->SelectObject( printer_doc_normal_font );

	print_dc->GetTextMetrics( &print_textmetric );
	print_char_height = (int)(print_textmetric.tmExternalLeading + print_textmetric.tmHeight);
	print_char_width = print_textmetric.tmAveCharWidth;
	for( int i=0; i<MSCREENWIDTH; i++ )
		print_char_widths[i] = print_char_width;

	//
	// Header Font
	//
	int print_header_char_height = 0;
	if( options.header() )
		{
		print_dc->SelectObject( printer_header_font );

		print_dc->GetTextMetrics( &print_textmetric );
		print_header_char_height = (int)(print_textmetric.tmExternalLeading + print_textmetric.tmHeight);
		}	

	//
	// Footer Font
	//
	int print_footer_char_height = 0;
	if( options.footer() )
		{
		print_dc->SelectObject( printer_footer_font );

		print_dc->GetTextMetrics( &print_textmetric );
		print_footer_char_height = (int)(print_textmetric.tmExternalLeading + print_textmetric.tmHeight);
		}	

	if( printer_old_font != NULL )
		print_dc->SelectObject( printer_old_font );
	printer_old_font = NULL;

	//
	// calc the logical size of the page
	//
	print_whole_page.SetRect( 0, 0, print_dc->GetDeviceCaps(HORZRES), print_dc->GetDeviceCaps(VERTRES) );
	print_dc->DPtoLP( &print_whole_page );

	//
	// remove the margin from the whole page
	//
	print_whole_page.DeflateRect
		(
		ScaleWidth(  options.leftMargin(),   print_dc ),
		ScaleHeight( options.topMargin(),    print_dc ),
		ScaleWidth(  options.rightMargin(),  print_dc ),
		ScaleHeight( options.bottomMargin(), print_dc )
		);

	//
	//	The text area starts out as a copy of the whole page
	//
	print_text_area = print_whole_page;

	//
	// remove space for the borders, header and footer
	//
	if( options.borders() )
		{
		//
		//	Remove about 2mm from each edge
		//	thats 226 twips
		//
		print_text_area.DeflateRect
			(
			ScaleWidth( PRINT_BORDER_WIDTH, print_dc ),
			ScaleHeight( PRINT_BORDER_WIDTH, print_dc )
			);
		}


	print_header_area.SetRectEmpty();
	if( options.header() )
		{
		//
		//	Calculate the area needed for the header
		//
		print_header_area.SetRect
			(
			print_text_area.left, print_text_area.top,
			print_text_area.right, print_text_area.top + print_header_char_height
			);

		//
		// move the top of the text area down
		// to leave room for the header
		//
		print_text_area.top += print_header_area.Height() + ScaleHeight( PRINT_HEADER_EXTRA_SPACE, print_dc );
		if( options.borders() )
			print_text_area.top += ScaleHeight( PRINT_BORDER_LINE_WIDTH, print_dc );
		}

	print_footer_area.SetRectEmpty();
	if( options.footer() )
		{
		//
		//	Calculate the area needed for the footer
		//
		print_footer_area.SetRect
			(
			print_text_area.left, print_text_area.bottom - print_footer_char_height,
			print_text_area.right, print_text_area.bottom
			);

		//
		// move the bottom of the text area up
		// to leave room for the footer
		//
		print_text_area.bottom -= print_footer_area.Height() + ScaleHeight( PRINT_HEADER_EXTRA_SPACE, print_dc );
		if( options.borders() )
			print_text_area.bottom -= ScaleHeight( PRINT_BORDER_LINE_WIDTH, print_dc );
		}

	print_line_number_area.SetRectEmpty();
	if( options.lineNumbers() )
		{
		//
		//	remove space for 4 digits and a dividing line
		//
		print_line_number_area.SetRect
			(
			print_text_area.left,
			print_text_area.top,
			print_text_area.left + 4*print_char_width,
			print_text_area.bottom
			);
		print_text_area.left += print_line_number_area.Width() + ScaleWidth( PRINT_LINE_NUMBERS_EXTRA_SPACE, print_dc );
		if( options.borders() )
			print_text_area.left += ScaleWidth( PRINT_BORDER_LINE_WIDTH, print_dc );
		}

	return true;
	}

void CWinemacsView::OnBeginPrinting( CDC* pDC, CPrintInfo* pInfo )
	{
	EmacsWinRegistry reg;		reg.loadRegistrySettings();

	print_options = new EmacsWinPrintOptions( reg.printOptions() );


	pInfo->m_bContinuePrinting = TRUE;
	if( !initialiseViewForPrinting( pDC, *print_options ) )
		{
		pInfo->m_bContinuePrinting = FALSE;
		return;
		}

	//
	//	Fill in the attributes
	//

	// assume print in colour
	setupPrinterAttribute( print_attr_array[SYNTAX_DULL], window_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_MODELINE], mode_line_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_WORD], syntax_word_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_STRING1], syntax_string1_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_STRING2], syntax_string2_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_STRING3], syntax_string3_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_COMMENT1], syntax_comment1_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_COMMENT2], syntax_comment2_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_COMMENT3], syntax_comment3_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_KEYWORD1], syntax_keyword1_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_KEYWORD2], syntax_keyword2_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[SYNTAX_TYPE_KEYWORD3], syntax_keyword3_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+1], user_1_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+2], user_2_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+3], user_3_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+4], user_4_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+5], user_5_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+6], user_6_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+7], user_7_rendition, print_options->syntaxColour(), print_options->inColour() );
	setupPrinterAttribute( print_attr_array[LINE_ATTR_USER+8], user_8_rendition, print_options->syntaxColour(), print_options->inColour() );

	//
	//	Setup the Emacs windows objects required to support printing
	//
	print_window_group = new  EmacsWindowGroup( this );
	print_window = new EmacsWindow( *print_window_group );

	print_window->w_next = print_window;
	print_window->w_buf = bf_cur;
	print_window->unsetWindowDot();
	print_window->unsetWindowMark();
	print_window->unsetWindowStart();
	print_window->w_height = print_text_area.Height()/print_char_height;
	print_window->w_width = print_text_area.Width()/print_char_width;

	print_buffer_mode = bf_cur->b_mode;
	print_buffer_mode.md_highlight = 0;
	print_buffer_mode.md_display_eof = 0;
	if( print_options->alwaysWrapLongLines() )
		print_buffer_mode.md_wrap_lines = 1;

	//
	// fill in the page start array
	//
	int print_buffer_pos = 1;
	DisplayBody_t text_buffer[MSCREENWIDTH];
	DisplayAttr_t attr_buffer[MSCREENWIDTH];

	for( int page=1,line_num=1; page<=PRINT_MAX_PAGE; page++ )
		{
		print_page_pos[page] = print_buffer_pos;
		print_page_line_number[page] = line_num;

		for( int line=0; line<print_window->w_height; line++ )
			{
			if( print_buffer_pos >= bf_cur->unrestrictedSize() )
				break;

			text_cursor = text_buffer;
			attr_cursor = attr_buffer;
			columns_left = print_window->w_width;
			bool line_wrapped = false;
			print_buffer_pos = print_window->dump_line_from_buffer( true, print_buffer_pos, -1, -1, 1, NULL, NULL, line_wrapped, print_buffer_mode );

			if( print_buffer_pos < 0 )
				print_buffer_pos = -print_buffer_pos;

			if( !line_wrapped )
				line_num++;
			}

		if( print_buffer_pos >= bf_cur->unrestrictedSize() )
			break;
		}

	pInfo->SetMaxPage( page );
	}

void CWinemacsView::tidyUpViewAfterPrinting( CDC * /*print_dc*/ )
	{
	delete printer_doc_normal_font;	printer_doc_normal_font = NULL;
	delete printer_doc_italic_font;	printer_doc_italic_font = NULL;
	delete printer_header_font;	printer_header_font = NULL;
	delete printer_footer_font;	printer_footer_font = NULL;
	printer_old_font = NULL;
	}

void CWinemacsView::OnEndPrinting(CDC* pDC, CPrintInfo* /*pInfo*/)
	{
	delete print_options;		print_options = NULL;
	delete print_window;		print_window =  NULL;
	delete print_window_group;	print_window_group = NULL;

	tidyUpViewAfterPrinting( pDC );
	}

void CWinemacsView::OnPrepareDC(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
	{
	// can reposition the origin here if that is useful
	}

void CWinemacsView::OnPrint(CDC* pDC, CPrintInfo *pinfo )
	{
	if( !pinfo->m_bContinuePrinting )
		return;

	//
	//	setup the printer font and setup some limits
	//
	int old_tt_width = min( t_width, MSCREENWIDTH );
	int old_tt_length = t_length;

	printer_old_font = pDC->SelectObject( printer_doc_normal_font );

	t_width = print_window->w_width;
	t_length = print_window->w_height;

	//
	//	Print the header
	//
	if( print_options->header() )
		{
		EmacsString header( ExpandPrintFormatString( print_options->headerFormat(), pinfo ) );

		pDC->SelectObject( printer_header_font );

		pDC->ExtTextOut
		(
		print_header_area.left, print_header_area.top,
		ETO_CLIPPED, print_header_area, 
		header.sdata(), header.length(),
		NULL
		);

		// back to doc font
		pDC->SelectObject( printer_doc_normal_font );
		}


	//
	//	print the text of the buffer and any line numbers
	//
	int print_buffer_pos = print_page_pos[ pinfo->m_nCurPage ];
	int line_number = print_page_line_number[ pinfo->m_nCurPage ];

	bool print_the_line_number = print_buffer_pos <= 1 || bf_cur->char_at( print_buffer_pos - 1 ) == '\n';

	EmacsLinePtr line; 
	line.newLine();

	for( int line_on_page=0; line_on_page<print_window->w_height; line_on_page++ )
		{
		if( print_buffer_pos >= bf_cur->unrestrictedSize() )
			{
			pinfo->m_bContinuePrinting = FALSE;
			break;
			}

		text_cursor = line->line_body;
		attr_cursor = line->line_attr;
		columns_left = print_window->w_width;
		line->line_length = print_window->w_width;

		bool line_wrapped = false;
		print_buffer_pos = print_window->dump_line_from_buffer( true, print_buffer_pos, -1, -1, 1, NULL, NULL, line_wrapped, print_buffer_mode );
		if( print_buffer_pos < 0 )
			print_buffer_pos = -print_buffer_pos;

		line->line_length = text_cursor-line->line_body;

		print_line( *pDC, line, line_on_page );

		if( print_options->lineNumbers() && print_the_line_number )
			{
			char line_number_buffer[4];
			int n = line_number;
			line_number_buffer[3] = char(n%10 + '0'); n /= 10;
			line_number_buffer[2] = n == 0 ? ' ' : char(n%10 + '0'); n /= 10;
			line_number_buffer[1] = n == 0 ? ' ' : char(n%10 + '0'); n /= 10;
			line_number_buffer[0] = n == 0 ? ' ' : char(n%10 + '0'); n /= 10;

			line_number++;

			pDC->TextOut
			(
			print_line_number_area.left, print_line_number_area.top + line_on_page*print_char_height,
			line_number_buffer, 4
			);
			}

		print_the_line_number = print_buffer_pos > 1 && !line_wrapped;
		}

	//
	//	Print the footer
	//
	if( print_options->footer() )
		{
		EmacsString footer( ExpandPrintFormatString( print_options->footerFormat(), pinfo ) );

		pDC->SelectObject( printer_footer_font );

		pDC->ExtTextOut
		(
		print_footer_area.left, print_footer_area.top,
		ETO_CLIPPED, print_footer_area, 
		footer.sdata(), footer.length(),
		NULL
		);
		}


	//
	//	print all the borders
	//
	if( print_options->borders() )
		{
		LOGBRUSH log_black_brush;
		log_black_brush.lbStyle = BS_SOLID;		// a solid
		log_black_brush.lbColor = RGB( 0, 0, 0 );	// black brush
		log_black_brush.lbHatch = 0;			// hatch is ignored
		
		CPen border_drawing_pen
			(
			PS_SOLID|PS_GEOMETRIC,
			2,
			&log_black_brush
			);

		CPen *old_pen = pDC->SelectObject( &border_drawing_pen );
		CBrush black_brush; 

		pDC->MoveTo( print_whole_page.left, print_whole_page.top );
		pDC->LineTo( print_whole_page.right, print_whole_page.top );
		pDC->LineTo( print_whole_page.right, print_whole_page.bottom );
		pDC->LineTo( print_whole_page.left, print_whole_page.bottom );
		pDC->LineTo( print_whole_page.left, print_whole_page.top );

		if( print_options->header() )
			{
			pDC->MoveTo
				(
				print_whole_page.left,
				print_text_area.top - ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC )
				);
			pDC->LineTo
				(
				print_whole_page.right,
				print_text_area.top - ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC )
				);				
			}

		if( print_options->footer() )
			{
			pDC->MoveTo
				(
				print_whole_page.left,
				print_text_area.bottom + ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC )
				);
			pDC->LineTo
				(
				print_whole_page.right,
				print_text_area.bottom + ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC )
				);				
			}

		if( print_options->lineNumbers() )
			{
			int top = print_whole_page.top;
			if( print_options->header() )
				top = print_text_area.top - ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC );

			int bottom = print_whole_page.bottom;
			if( print_options->footer() )
				bottom = print_text_area.bottom + ScaleHeight( PRINT_BORDER_LINE_WIDTH/2, pDC );

			pDC->MoveTo
				(
				print_line_number_area.right + ScaleWidth( PRINT_BORDER_LINE_WIDTH/2, pDC ),
				top
				);
			pDC->LineTo
				(
				print_line_number_area.right + ScaleWidth( PRINT_BORDER_LINE_WIDTH/2, pDC ),
				bottom
				);				
			}

		pDC->SelectObject( old_pen );
		}


	t_width = old_tt_width;
	t_length = old_tt_length;

	pinfo->m_bContinuePrinting = print_buffer_pos < bf_cur->unrestrictedSize();

	if( printer_old_font != NULL )
		pDC->SelectObject( printer_old_font );
	}

void CWinemacsView::print_line( CDC &print_dc, EmacsLinePtr line, int line_on_page )
	{
	int start_col = 0;
	int end_col = 1;
	int last_col = line->line_length;
	while( start_col < last_col )
		{
		int last_attr = line->line_attr[start_col];
		while( end_col < last_col && last_attr == line->line_attr[end_col] )
			end_col++;

		EmacsWinColour *attr;
		if( last_attr&LINE_ATTR_USER )
			attr = &print_attr_array[last_attr&LINE_M_ATTR_USER];
		else
			attr = &print_attr_array[last_attr];

		COLORREF old_colour = print_dc.SetTextColor( attr->foreground );
		old_colour = print_dc.SetBkColor( attr->background );

		if( attr->italic )
			print_dc.SelectObject( printer_doc_italic_font );

		unsigned short unicode_line[MSCREENWIDTH];
		for( int col=start_col; col<end_col; col++ )
			if( line->line_body[ col ] < ' ' )
				unicode_line[ col ] = C0_map[ line->line_body[ col ] ];
			else
				unicode_line[ col ] = line->line_body[ col ];
		ExtTextOutW	// unicode ExtTextOut
			(
			print_dc.m_hDC,
			print_text_area.left + (start_col*print_char_width),
			print_text_area.top + (line_on_page*print_char_height),
			0, NULL,
			&unicode_line[start_col], end_col - start_col,
			&print_char_widths[0]
			);

		if( attr->italic )
			print_dc.SelectObject( printer_doc_normal_font );

		if( attr->underline )
			{
			CPen *old_pen = NULL;
			old_pen = print_dc.SelectObject( attr->pen );

			print_dc.MoveTo
			(
			print_text_area.left + (start_col*print_char_width),
			print_text_area.top + ((line_on_page+1)*print_char_height) - textmetric.tmDescent + 1
			);
			print_dc.LineTo
			(
			print_text_area.left + (end_col*print_char_width),
			print_text_area.top + ((line_on_page+1)*print_char_height) - textmetric.tmDescent + 1
			);

			if( old_pen != NULL )
				print_dc.SelectObject( old_pen );
			}

		start_col = end_col;
		end_col++;
		}

	COLORREF old_colour = print_dc.SetTextColor( RGB(0,0,0) );	// black
	old_colour = print_dc.SetBkColor( RGB(255,255,255) );		// on white
	}

int CWinemacsView::new_font()
	{
	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );

	lf.lfHeight = theApp.GetProfileInt( "Fonts", "TextFontSize", 9 );
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = theApp.GetProfileInt( "Fonts", "TextFontWeight", FW_NORMAL );
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = (unsigned char)theApp.GetProfileInt( "Fonts", "TextFontCharSet", ANSI_CHARSET );
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;

	CString face = theApp.GetProfileString( "Fonts", "TextFontName", "FixedSys" );
	if( face.GetLength() >= LF_FACESIZE )
		face = "FixedSys";
	strcpy( lf.lfFaceName, face );

	CFont *new_normal_font = new CFont;
	lf.lfItalic = FALSE;
	if( !new_normal_font->CreateFontIndirect( &lf ) )
		{
		_dbg_msg( FormatString("CreateFontIndirect failed %d") << GetLastError() );
		return 0;
		}

	CFont *new_italic_font = new CFont;
	lf.lfItalic = TRUE;
	if( !new_italic_font->CreateFontIndirect( &lf ) )
		{
		delete new_normal_font;
		_dbg_msg( FormatString("CreateFontIndirect failed %d") << GetLastError() );
		return 0;
		}

	lf.lfItalic = FALSE;

#if 0
	HFONT new_special_font = CreateFontW
		(
		emacs_lf.lfHeight,	// int nHeight,			// logical height of font 
		0,			// int nWidth,			// logical average character width 
		0,			// int nEscapement,		// angle of escapement 
		0,			// int nOrientation,		// base-line orientation angle 
		emacs_lf.lfWeight,	// int fnWeight,		// font weight 
		0,			// DWORD fdwItalic,		// italic attribute flag 
		0,			// DWORD fdwUnderline,		// underline attribute flag 
		0,			// DWORD fdwStrikeOut,		// strikeout attribute flag 
		DEFAULT_CHARSET,	// DWORD fdwCharSet,		// character set identifier 
		OUT_DEFAULT_PRECIS,	// DWORD fdwOutputPrecision,	// output precision 
		CLIP_DEFAULT_PRECIS,	// DWORD fdwClipPrecision,	// clipping precision 
		DEFAULT_QUALITY,	// DWORD fdwQuality,		// output quality 
		DEFAULT_PITCH,		// DWORD fdwPitchAndFamily,	// pitch and family 
		L"Lucinda Sans Unicode"	// LPCTSTR lpszFace		// pointer to typeface name string 
		);

	if( new_special_font == NULL )
		{
		delete new_normal_font;
		delete new_italic_font;
		_dbg_msg( FormatString("CreateFont special failed %d") << GetLastError() );
		return 0;
		}
#endif

	CDC *pDC = GetDC();

	delete emacs_normal_font;
	delete emacs_italic_font;
#if 0
	if( emacs_special_font )
		::DeleteObject( emacs_special_font );

	emacs_special_font = new_special_font;
#endif 
	emacs_normal_font = new_normal_font;
	emacs_italic_font = new_italic_font;

	old_font = pDC->SelectObject( emacs_normal_font );
	pDC->GetTextMetrics( &textmetric );
	pDC->SelectObject( old_font );
	ReleaseDC( pDC );

	nCharHeight = (int)(textmetric.tmExternalLeading + textmetric.tmHeight);
	nCharWidth = textmetric.tmAveCharWidth
		+ (textmetric.tmMaxCharWidth - textmetric.tmAveCharWidth)*nCharExtra/100;

	for( int i=0; i<MSCREENWIDTH; i++ )
		char_widths[i] = nCharWidth;

	if( have_focus )
		{
		HideCaret();
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		SetCaretPos( CPoint( CARET_XPOS, CARET_YPOS ) );
		ShowCaret();
		}

	t_length = 4;
	t_width = 50;

	CRect rect;
	GetClientRect( &rect );
	InvalidateRect( &rect );
	OnSize( SIZE_RESTORED, rect.Width(), rect.Height() );

	return 1;
	}

/////////////////////////////////////////////////////////////////////////////
// CWinemacsView diagnostics

#if defined(_DEBUG)
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


afx_msg BOOL CWinemacsView::OnSetCursor( CWnd* pWnd, UINT nHitTest, UINT message)
	{
	if( pWnd == this		// the views window
	&& nHitTest == HTCLIENT )	// in the client area
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

bool CWinemacsView::input_menu_string( UINT nID, bool do_input )
	{
	const EmacsString &id_string = menu_id_to_id_string( nID );
	if( id_string.isNull() )
		return false;

	if( do_input )
		{
		k_input_char_string( MENU_PREFIX, false );
		k_input_char_string( id_string.data(), false );
		}

	return true;
	}


BOOL CWinemacsView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
	{

	if( nCode == CN_COMMAND		// handle any COMMAND code
//	&& nID != ID_FILE_OPEN		// but must not handle File Open as CView must see that COMMAND
	&&	(  (nID >= 0x8000 && nID <= 0x8fff)
		|| (nID >= 0xE100 && nID <= 0xE9FF) )
	)
		{
		// see if the ID is know to the menu system
		if( input_menu_string( nID, pHandlerInfo == NULL ) )
			// o.k. all done
			return 1;
		// if the ID is one of the user defined then finish processing as well
		if( nID >= ID_USERMENU_FIRST && nID <= ID_USERMENU_LAST )
			return 1;
		}

	// pass on the the base class handler
	return CView::OnCmdMsg( nID, nCode, pExtra, pHandlerInfo );
	}

void CWinemacsView::OnStopExecution()
	{
	illegal_operation();
	}

static EmacsLinePtr empty_line_ptr;


void CWinemacsView::t_update_line(  EmacsLinePtr old_line, EmacsLinePtr new_line, int row )
	{
	if( new_line.isNull() )
		{
		if( empty_line_ptr.isNull() )
			empty_line_ptr.newLine();

		new_line = empty_line_ptr;
		}

	// only write the line if its changed or there is no old line
	if( !old_line.isNull()
	&& new_line->line_length == old_line->line_length
	&& memcmp( new_line->line_body, old_line->line_body, new_line->line_length*sizeof( DisplayBody_t ) ) == 0
	&& memcmp( new_line->line_attr, old_line->line_attr, new_line->line_length*sizeof( DisplayAttr_t ) ) == 0 )
		return;

	if( !t_update_begin() )
		return;

	if( t_width > new_line->line_length )
		{
		//
		//	Fill out the line with spaces
		//
		memset
		(
		new_line->line_body + new_line->line_length,
		' ',
		(t_width - new_line->line_length)*sizeof( DisplayBody_t )
		);
		// and an attribute of normal
		memset
		(
		new_line->line_attr + new_line->line_length,
		0,
		(t_width - new_line->line_length)*sizeof( DisplayAttr_t )
		);
		}


	int start_col = 0;
	int end_col = 1;
	while( start_col < t_width )
		{
		int last_attr = new_line->line_attr[start_col];
		while( end_col < t_width && last_attr == new_line->line_attr[end_col] )
			end_col++;

		EmacsWinColour *attr;
		if( last_attr&LINE_M_ATTR_HIGHLIGHT )
			attr = &attr_high;
		else if( last_attr&LINE_ATTR_USER )
			attr = &attr_array[last_attr&LINE_M_ATTR_USER];
		else
			attr = &attr_array[last_attr];

		COLORREF old_colour;	// used while debugging
		old_colour = pDC->SetTextColor( attr->foreground );
		old_colour = pDC->SetBkColor( attr->background );

		if( attr->italic )
			pDC->SelectObject( emacs_italic_font );

		unsigned short unicode_line[MSCREENWIDTH];
		for( int col=start_col; col<end_col; col++ )
			if( new_line->line_body[ col ] < ' ' )
				unicode_line[ col ] = C0_map[ new_line->line_body[ col ] ];
			else
				{
				int codepage = CP_ACP;
//				codepage = 1251;
				int rc = MultiByteToWideChar( codepage, 0, (const char *)&new_line->line_body[ col ], 1, &unicode_line[ col ], 1 );

				//unicode_line[ col ] = line->line_body[ col ];
				}
		ExtTextOutW	// unicode ExtTextOut
			(
			pDC->m_hDC,
			WINDOW_BORDER+(start_col*nCharWidth), (row-1)*nCharHeight,
			0, NULL,
			&unicode_line[start_col], end_col - start_col,
			&char_widths[0]
			);

#if 0
		bool special=false;
		for( col=start_col; col<end_col; col++ )
			if( new_line->line_body[ col ] < ' ' )
				{
				if( !special )
					{
					::SelectFont( pDC->m_hDC, emacs_special_font );
					special = true;			
					}
				unsigned char c0_ch = new_line->line_body[ col ];
				unsigned short sp_ch = C0_map[c0_ch];
				// use the control pictures
				::ExtTextOutW	// unicode ExtTextOut
					(
					pDC->m_hDC,
					WINDOW_BORDER+(col*nCharWidth), (row-1)*nCharHeight,
					0,	// options - none 
					NULL,	// rect - none
					&sp_ch, 1,
					NULL	// char widths - none
					);
				}
#endif
		if( /*special ||*/ attr->italic )
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

	t_update_end();
	}

bool CWinemacsView::t_update_begin()
	{
	/* cannot proceed if the font is not selected */	
	if( emacs_normal_font == NULL )
		return 0;

	if( have_hdc == 0 )
		{
		pDC = GetDC();
		emacs_setup_dc();
		HideCaret();

		attr_high = region_rendition;
		attr_array[SYNTAX_DULL] = window_rendition;
		attr_array[LINE_ATTR_MODELINE] = mode_line_rendition;
		attr_array[SYNTAX_WORD] = syntax_word_rendition;
		attr_array[SYNTAX_TYPE_STRING1] = syntax_string1_rendition;
		attr_array[SYNTAX_TYPE_STRING2] = syntax_string2_rendition;
		attr_array[SYNTAX_TYPE_STRING3] = syntax_string3_rendition;
		attr_array[SYNTAX_TYPE_COMMENT1] = syntax_comment1_rendition;
		attr_array[SYNTAX_TYPE_COMMENT2] = syntax_comment2_rendition;
		attr_array[SYNTAX_TYPE_COMMENT3] = syntax_comment3_rendition;
		attr_array[SYNTAX_TYPE_KEYWORD1] = syntax_keyword1_rendition;
		attr_array[SYNTAX_TYPE_KEYWORD2] = syntax_keyword2_rendition;
		attr_array[SYNTAX_TYPE_KEYWORD3] = syntax_keyword3_rendition;
		attr_array[LINE_ATTR_USER+1] = user_1_rendition;
		attr_array[LINE_ATTR_USER+2] = user_2_rendition;
		attr_array[LINE_ATTR_USER+3] = user_3_rendition;
		attr_array[LINE_ATTR_USER+4] = user_4_rendition;
		attr_array[LINE_ATTR_USER+5] = user_5_rendition;
		attr_array[LINE_ATTR_USER+6] = user_6_rendition;
		attr_array[LINE_ATTR_USER+7] = user_7_rendition;
		attr_array[LINE_ATTR_USER+8] = user_8_rendition;

		fixup_scroll_bars();
		}
	have_hdc++;    


	return 1;
	}

void CWinemacsView::t_update_end()
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

void CWinemacsView::t_topos ( int row, int column)
	{
	cursor_x = column-1;
	cursor_y = row-1;
	}

#if MEMMAP

bool CWinemacsView::t_window(  int n )
	{
	window_size = n;
	return true;
	} 
	
class EmacsCRgn : public CRgn
	{
public:
	EmacsCRgn()
		: CRgn()
		{
		CreateRectRgn( 0, 0, 0, 0 );
		}

	EmacsCRgn( RECT *rect )
		: CRgn()
		{
		CreateRectRgnIndirect( rect );
		}

	EmacsCRgn( RGNDATA *data )
		: CRgn()
		{
		data->rdh.nRgnSize = (data->rdh.nCount)*sizeof(RECT);
		RECT &bounds = data->rdh.rcBound;
		bounds.left = 65535;
		bounds.right = 0;
		bounds.top = 65535;
		bounds.bottom = 0;

		//
		//	If their are no rects then create an empty region and return
		//
		if( data->rdh.nCount == 0 )
			{
			CreateRectRgn( 0, 0, 0, 0 );
			return;
			}

		//
		//	Work out the bounding rectangle
		//
		RECT *rects = (RECT *)&data->Buffer[0];

		for( DWORD r=0; r<data->rdh.nCount; r++ )
			{
			if( rects[r].left < bounds.left )
				bounds.left = rects[r].left;
			if( rects[r].top < bounds.top )
				bounds.top = rects[r].top;
			if( rects[r].right > bounds.right )
				bounds.right = rects[r].right;
			if( rects[r].bottom > bounds.bottom )
				bounds.bottom = rects[r].bottom;
			}

		//
		//	Create the region
		//
		if( !CreateFromData
			(
			NULL,								// no XFORM
			sizeof( RGNDATAHEADER ) +
				(data->rdh.nCount)*sizeof(RECT),		// its this big
			data								// and its here
			) )
			{
			DWORD ml_err = GetLastError();
			_dbg_msg( FormatString("EmacsCRgn(RGNDATA) failed 0x%x") << ml_err );

			// prevent further crashes
			CreateRectRgn( 0, 0, 0, 0 );
			}
		}
	void PrintRegion( const char *title="");
	};

void EmacsCRgn::PrintRegion( const char *title )
	{
	struct __X__ {
		RGNDATA data;
		char buffer_space[4096];
		} rgndata;

	int count = GetRegionData( &rgndata.data, sizeof( rgndata ) );
	if( count == 0 )
		{
		_dbg_msg( "scroll_lines: Region data buffer too small" );
		return;
		}

	_dbg_msg( FormatString("scroll_lines: %s: Region rect count %d")
			<< title << rgndata.data.rdh.nCount );

	RECT *rects = (RECT *)&rgndata.data.Buffer[0];
	for( DWORD r=0; r<rgndata.data.rdh.nCount; r++ )
		{
		RECT &R = rects[r];
		_dbg_msg( FormatString("scroll_lines:  Rect %d( L:%d, R:%d, W:%d, T:%d, B:%d, H:%d )")
				<< r << R.left << R.right << R.right-R.left << R.top << R.bottom << R.bottom-R.top );
		}
	}

void PrintRect( const RECT &R, const char *title )
	{
	_dbg_msg( FormatString("scroll_lines: %s: Rect( L:%d, R:%d, W:%d T:%d, B:%d H:%d )")
				<< title << R.left << R.right << R.right-R.left << R.top << R.bottom << R.bottom-R.top );
	}

void CWinemacsView::scroll_lines( int scroll_by )
	{
	// scroll this area of the window
	CRect scroll_rect
		(
		0,						// left
		cursor_y * nCharHeight,				// top
		t_width * nCharWidth + 2*WINDOW_BORDER-1,	// right
		window_size * nCharHeight			// bottom
		);

	EmacsCRgn update_region;				// create null region
	RECT update_rect;

#if DEBUG_SCROLL_LINES
	update_region.PrintRegion( "update_region before" );
#endif

	// now scroll up or down by scroll_by amount
	pDC->ScrollDC( 0, scroll_by*nCharHeight, &scroll_rect, &scroll_rect, &update_region, &update_rect );
#if DEBUG_SCROLL_LINES
	PrintRect( scroll_rect, "scroll_rect" );
	PrintRect( update_rect, "update_rect" );
	update_region.PrintRegion( "update_region" );
#endif
	//
	// reduce the size of the scroll_rect to be the area of lines that
	// need to survive the scrolling
	//
	if( scroll_by < 0 )
		scroll_rect.DeflateRect
			(
			0,			// left
			0,			// top
			0,			// right
			-scroll_by*nCharHeight	// bottom
			);
	else
		scroll_rect.DeflateRect
			(
			0,			// left
			scroll_by*nCharHeight,	// top
			0,			// right
			0			// bottom
			);

	EmacsCRgn bounding_region( scroll_rect );			// create uninit region

#if DEBUG_SCROLL_LINES
	PrintRect( scroll_rect, "scroll_rect" );
	bounding_region.PrintRegion( "bounding_region" );
#endif


	// fixup_region is the piece of update_region that is inside the bounding_region
	EmacsCRgn fixup_region;
	fixup_region.CombineRgn( &update_region, &bounding_region, RGN_AND );
#if DEBUG_SCROLL_LINES
	fixup_region.PrintRegion( "fixup_region" );
#endif

	// scroll bars region is the area covered by scroll bars
	EmacsCRgn scroll_bar_region( (RGNDATA*)&scroll_bar_region_data );

#if DEBUG_SCROLL_LINES
	scroll_bar_region.PrintRegion( "scroll_bar_region" );
#endif

	fixup_region.CombineRgn( &fixup_region, &scroll_bar_region, RGN_DIFF );
#if DEBUG_SCROLL_LINES
	fixup_region.PrintRegion( "fixup_region sans scroll_bar_region" );
#endif

	RECT rgn_box_rect;
	DWORD type = fixup_region.GetRgnBox( &rgn_box_rect );
	if( type == NULLREGION )
		return;

	CSize line_size( nCharWidth*t_width, nCharHeight );

	EmacsLinePtr nullLine;
	for( int n = 0; n < t_length; )
		{

		CPoint pt( 0, n*nCharHeight );
		CRect this_line( pt, line_size );

		n++;

		if( fixup_region.RectInRegion( this_line ) )
			{
#if DEBUG_SCROLL_LINES
			EmacsString title( FormatString("this_list line %d is in fixup_region") << n );

			PrintRect( this_line, title.sdata() );
#endif
			t_update_line( nullLine, t_phys_screen[n-scroll_by], n );	// draw the line the hard way
			}
		}
#if DEBUG_SCROLL_LINES
	_dbg_msg("--------------------------\n");
#endif
	}

void CWinemacsView::t_insert_lines(  int n )
	{
	scroll_lines( n );
	}

void CWinemacsView::t_delete_lines(  int n )
	{
	scroll_lines( -n );
	}
#endif

void CWinemacsView::t_wipe_line ( int line)
	{
	if( !have_hdc )
		return;

	RECT rect;

	rect.top = line * nCharHeight;
	rect.bottom = (line+1) *nCharHeight ;
	rect.left = 0;
	rect.right = t_width * nCharWidth;

	pDC->FillRect( &rect, CBrush::FromHandle(white_brush) );
	}

void CWinemacsView::t_beep()
	{
	::MessageBeep( 0 );
	}

void CWinemacsView::t_reset( )
	{
	if( !t_update_begin() )
		return;

	CRect rect;
	GetClientRect( &rect );
	pDC->FillRect( &rect, CBrush::FromHandle(white_brush) );

	if( have_focus )
		{
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		}

	t_update_end();
	}

int init_char_terminal(const EmacsString &/*device*/ )
	{
	return 0;
	}
	
void CWinemacsView::t_change_attributes(  )
	{
	if( !t_update_begin() )
		return;

	if( have_focus )
		{
		::DestroyCaret();
		CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
		}

	t_update_end();
	}

void CWinemacsView::t_display_activity( unsigned char ch )
	{
	if( activity_indicator )
		{
		if( !t_update_begin() )
			return;

		pDC->SetTextColor( attr_array[SYNTAX_WORD].foreground );
		pDC->SetBkColor( attr_array[SYNTAX_WORD].background );

		pDC->ExtTextOut
		(
		WINDOW_BORDER, (t_length-1)*nCharHeight,
		0, NULL,
		s_str(&ch), 1,
		&char_widths[0]
		);

		t_update_end();
		}
	}

int init_gui_terminal( const EmacsString &/*device*/ )
	{
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
	Save<EmacsViewPtr> cur_view( &theActiveView );
	theActiveView = this;

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
		old_length = t_length;
		old_width = t_width;

		t_width = (cx - 2*WINDOW_BORDER)/nCharWidth - vertical_bar_width;
		if( t_width > MSCREENWIDTH )
			t_width = MSCREENWIDTH;
		t_length = cy/nCharHeight;
		if( t_length > MSCREENLENGTH )
			t_length = MSCREENLENGTH;
		if( t_length < 3 )
			t_length = 3;
		term_width = t_width;
		term_length = t_length;

		if( t_length != old_length
		|| t_width != old_width )
			t_geometry_change();
		}


		fixup_scroll_bars();
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

void CWinemacsView::OnChar(UINT ch, UINT nRepCnt, UINT /*nFlags*/)
	{
	for( UINT cnt=0; cnt < nRepCnt; cnt++ )
		{
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("Char ASCII_CODE(0x%X)") << ch );

		if( m_ignore_next_char )
			return;
		if( ch == VK_RETURN
		&& (last_key_down_flags&KF_EXTENDED) != 0
		&& keyboard_emulates_lk201_keypad != 0 )
			k_input_char_string( "\033OM", GetKeyState( VK_SHIFT ) < 0 );
		else if( ch == ' ' && GetKeyState( VK_CONTROL ) < 0 )
			k_input_char( '\000', false );
		else
			{
			k_input_char( ch, false );
			}
		}
	}

BOOL CWinemacsView::PreCreateWindow(CREATESTRUCT& cs) 
	{ 
	// Create a child window without the maximize button 
	cs.style |= WS_CLIPCHILDREN; 

	// Call the base-class version
	return CView::PreCreateWindow(cs);
	}

bool conv_keys::findEntryAndTranslate( bool enh_key, UINT vk, UINT /*repcnt*/ ) const
	{
	const conv_key_entry *entry = findEntry( enh_key, vk );
	if( entry == NULL )
		return false;

	bool ctrl = GetKeyState( VK_CONTROL ) < 0;
	bool shift = GetKeyState( VK_SHIFT ) < 0;

	entry->translate( ctrl, shift );

	return true;
	}

const conv_key_entry *conv_keys::findEntry( bool enhanced_key, UINT vk ) const
	{
	for( int i=0; i<m_num_entries; i++ )
		if( m_entries[i].is_enhanced_key == enhanced_key
		&&  m_entries[i].key_code == vk )
			{
			if( m_entries[i].lk201_keypad && keyboard_emulates_lk201_keypad == 0 )
				continue;

			return &m_entries[i];
			}

	return NULL;
	}

void conv_key_entry::translate( bool ctrl, bool shift ) const
	{
	if( input_mode == 0 && ctrl_shift_translation != NULL && ctrl && shift)
		{ theActiveView->k_input_char_string( ctrl_shift_translation, shift );
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("   KEY(0x%x): Control Shift") << key_code ); }

	else if( ctrl_translation != NULL && ctrl )
		{ theActiveView->k_input_char_string( ctrl_translation, shift );
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("   KEY(0x%x): Control") << key_code ); }

	else if( (input_mode == 0 || always_use_shift_translation) && shift_translation != NULL && shift )
		{ theActiveView->k_input_char_string( shift_translation, !always_use_shift_translation );
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("   KEY(0x%x): Shift") << key_code ); }

	else if( translation != NULL )
		{ theActiveView->k_input_char_string( translation, shift );
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("   KEY(0x%x): Translation") << key_code ); }
	else
		{
		special_key[3] = (unsigned char)key_code;
		theActiveView->k_input_char_string( special_key, shift );
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("   KEY(0x%x): Default") << key_code );
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

			convert_keys.findEntryAndTranslate( false, vk, 1 );
			}
		return 1;
		}
		
	return CWinApp::PreTranslateMessage( pMsg );
	}

void CWinemacsView::OnKeyDown(UINT vk, UINT nRepCnt, UINT flags)
	{
	if( dbg_flags&DBG_KEY )
		_dbg_msg( FormatString("KeyDown KEY_CODE(0x%X) flags: 0x%x") << vk << flags );

	last_key_down_flags = flags;

	m_ignore_next_char = convert_keys.findEntryAndTranslate
			( (flags&KF_EXTENDED) != 0, vk , nRepCnt );
	
	CView::OnKeyDown(vk, nRepCnt, flags);
	}

void CWinemacsView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
	{
	CView::OnKeyUp( nChar, nRepCnt, nFlags );
	m_ignore_next_char = FALSE;
	}


void CWinemacsView::OnOptionsColors()
	{
	CColourOpt colours;
	
	if( colours.DoModal() == IDOK )
		{
		colours.save_options();
	
		// redraw the screen in case the colours have changed
		screen_garbaged = 1;
		windows.do_dsp();
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
	LOGFONT lf;
	memset( &lf, 0, sizeof(lf) );

	CHOOSEFONT cf;
	memset( &cf, 0, sizeof(cf) );

	cf.lStructSize = sizeof(cf);
	cf.hwndOwner = m_hWnd;
	cf.hDC = NULL;
	cf.lpLogFont = &lf;
	cf.lpszStyle = NULL;

	cf.Flags = CF_SCREENFONTS;
	cf.Flags |= CF_FORCEFONTEXIST;

	cf.Flags |= CF_LIMITSIZE;
	cf.nSizeMin = 6;
	cf.nSizeMax = 36;

	cf.Flags |= CF_FIXEDPITCHONLY;
	cf.Flags |= CF_INITTOLOGFONTSTRUCT;

	lf.lfHeight = theApp.GetProfileInt( "Fonts", "TextFontSize", -9 );
	lf.lfWidth = 0;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfWeight = theApp.GetProfileInt( "Fonts", "TextFontWeight", FW_NORMAL );
	lf.lfItalic = FALSE;
	lf.lfUnderline = FALSE;
	lf.lfStrikeOut = FALSE;
	lf.lfCharSet = (unsigned char)theApp.GetProfileInt( "Fonts", "TextFontCharSet", ANSI_CHARSET );
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfQuality = PROOF_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_MODERN;

	CString face = theApp.GetProfileString( "Fonts", "TextFontName", "FixedSys" );
	if( face.GetLength() >= LF_FACESIZE )
		face = "FixedSys";

	strcpy( lf.lfFaceName, face );

	if( !ChooseFont( &cf ) )
		return;

	theApp.WriteProfileString( "Fonts", "TextFontName", lf.lfFaceName );
	theApp.WriteProfileInt( "Fonts", "TextFontSize", lf.lfHeight );
	theApp.WriteProfileInt( "Fonts", "TextFontWeight", lf.lfWeight );
	theApp.WriteProfileInt( "Fonts", "TextFontCharSet", lf.lfCharSet );

	new_font();
	}

bool CWinemacsView::choosePrinterFont( CDC &print_dc, EmacsWinFont &font ) 
	{
	//
	//	Fill in the structures used by ChooseFont
	//
	CHOOSEFONT cf;
	LOGFONT lf;

	//
	// setup choosefont structure
	//
	memset( &cf, 0, sizeof(cf) );

	cf.lStructSize	= sizeof(cf);
	cf.hwndOwner	= m_hWnd;
	cf.hDC		= print_dc.m_hDC;
	cf.lpLogFont	= &lf;
	cf.lpszStyle	= NULL;

	cf.Flags	= CF_PRINTERFONTS;
	cf.Flags	|= CF_FORCEFONTEXIST;
	cf.Flags	|= CF_FIXEDPITCHONLY;
	cf.Flags	|= CF_LIMITSIZE;
	cf.Flags	|= CF_INITTOLOGFONTSTRUCT;

	cf.nSizeMin	= 6;
	cf.nSizeMax	= 72;

	//
	//	setup logfont structure
	//
	memset( &lf, 0, sizeof(lf) );

	strcpy( lf.lfFaceName, font.face() );

	lf.lfHeight	= font.fontHeight();
	lf.lfWeight	= FW_NORMAL;

	//
	//	Have the user select the font and size
	//
	BOOL status = ChooseFont( &cf );
	if( !status )
		return false;

	//
	//	Save the selection in the registry
	//
	font.pointSize( cf.iPointSize );
	font.fontHeight( lf.lfHeight );
	font.face( lf.lfFaceName );

	return true;
	}

// work structure to queue on a timeout
class Synchronise_files_work_item : public EmacsWorkItem
	{
public:
	virtual void workAction(void)
		{
		synchronise_files();
		}
	};
static Synchronise_files_work_item synchronise_files_request;

extern SystemExpressionRepresentationIntBoolean synchronise_buffers_on_focus;

void CWinemacsView::OnSetFocus(CWnd* pOldWnd)
	{
	CView::OnSetFocus(pOldWnd);

	CreateSolidCaret( CARET_WIDTH, CARET_HEIGHT );
	SetCaretPos( CPoint( CARET_XPOS, CARET_YPOS ) );
	ShowCaret();

	have_focus = 1;

	theActiveView = this;

	if( synchronise_buffers_on_focus )
		synchronise_files_request.addItem();
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

void CWinemacsView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	CScrollBarHorizontal *hsb = (CScrollBarHorizontal *)pScrollBar;
	hsb->OnHScroll( nSBCode, nPos, pScrollBar );
	}	

void CWinemacsView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
	{
	CScrollBarVertical *hsb = (CScrollBarVertical *)pScrollBar;
	hsb->OnVScroll( nSBCode, nPos, pScrollBar );
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
	// only handle mouse up if we have processed a mouse down
	if( (mouse_button_state & MOUSE_BUTTON_1) != 0 )
		{
		mouse_button_state &= ~MOUSE_BUTTON_1;
		OnMouseButton( nFlags, point, 3 );
		}
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
	// only handle mouse up if we have processed a mouse down
	if( (mouse_button_state & MOUSE_BUTTON_3) != 0 )
		{
		mouse_button_state &= ~MOUSE_BUTTON_3;
		OnMouseButton( nFlags, point, 7 );
		}
//	CView::OnMButtonUp(nFlags, point);
	}

void CWinemacsView::OnMButtonDown(UINT nFlags, CPoint point)
	{
	mouse_button_state |= MOUSE_BUTTON_2;
	OnMouseButton( nFlags, point, 4 );
//	CView::OnMButtonDown(nFlags, point);
	}

void CWinemacsView::OnMButtonDblClk(UINT nFlags, CPoint point) 
	{
	mouse_button_state |= MOUSE_BUTTON_2;
	OnMouseButton( nFlags, point, 4 );
//	CView::OnMButtonDblClk(nFlags, point);
	}

void CWinemacsView::OnMButtonUp(UINT nFlags, CPoint point)
	{
	// only handle mouse up if we have processed a mouse down
	if( (mouse_button_state & MOUSE_BUTTON_2) != 0 )
		{
		mouse_button_state &= ~MOUSE_BUTTON_2;
		OnMouseButton( nFlags, point, 5 );
		}
//	CView::OnMButtonUp(nFlags, point);
	}

void CWinemacsView::OnMouseButton( UINT /*nFlags*/, CPoint point, int button )
	{
	/*
	 *	The mouse control sequence is
	 *	CSI event ; ; y ; x &w
	 */
	if( mouse_enable )
		{
		bool shift = GetKeyState( VK_SHIFT ) < 0;

		int mouse_x, mouse_y;
		mapCoordinatedToEmacs( point.x, point.y, mouse_x, mouse_y );

		if( mouse_x <= 0 )
			mouse_x = 1;
		if( mouse_x > t_width )
			mouse_x = t_width;

		last_mouse_x = mouse_x;
		last_mouse_y = mouse_y;

		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("MouseButton X: %d  Y: %d  Button: 0x%x")
				<< mouse_x << mouse_y << button );

		// if a mouse button is pressed...
		if( mouse_button_state != 0 )
			// capture the mouse
			SetCapture();
		else
			// other wise free up the mouse
			ReleaseCapture();

		theActiveView->k_input_char_string
			(
			EmacsString(FormatString("\x1b[%d;%d;%d;%d&w")
				<< button << mouse_button_state << mouse_y << mouse_x).data(),
			shift
			);
		}
	}

void CWinemacsView::OnMouseMove(UINT /*nFlags*/, CPoint point) 
	{
	/*
	 *	The mouse control sequence is
	 *	CSI event ; ; y ; x &W
	 */
	if( mouse_enable&MOUSE_MOTION_EVENTS	// enabled and
	&& mouse_button_state != 0 )		// a button down
		{
		bool shift = GetKeyState( VK_SHIFT ) < 0;

		int mouse_x, mouse_y;
		mapCoordinatedToEmacs( point.x, point.y, mouse_x, mouse_y );
		if( mouse_x < 0 )
			mouse_x = 0;
		if( mouse_x > t_width )
			mouse_x = t_width;
		if( mouse_y < 0 )
			mouse_y = 0;

		if( last_mouse_x != mouse_x
		|| last_mouse_y != mouse_y )
			{
			theActiveView->k_input_char_string
				(
				EmacsString(FormatString("\x1b[%d;%d;%d;%d&w")
					<< 0 << mouse_button_state << mouse_y << mouse_x).data(),
				shift
				);
			last_mouse_x = mouse_x;
			last_mouse_y = mouse_y;
			}
		}
	}


CDocument* CWinemacsApp::OpenDocumentFile(LPCSTR lpszFileName)
	{
	ui_open_file_name = lpszFileName;
	theActiveView->k_input_char_string( MENU_PREFIX "fr", false );

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
		theActiveView->windows.do_dsp();
		}
		
	}

void CWinemacsView::activateNextView()
	{
	CDocument *doc = GetDocument();

	// count the number of views on the document
	POSITION pos = doc->GetFirstViewPosition();
	while( pos != NULL )
		{
		CView* view = doc->GetNextView( pos );
		if( view == this )
			break;
		}   

	if( pos == NULL )
		pos = doc->GetFirstViewPosition();

	CView* view = doc->GetNextView( pos );
	CMDIChildWnd *mdi_child = (CMDIChildWnd *)view->GetParent();
	mdi_child->MDIActivate();
	}

void CWinemacsView::activatePreviousView()
	{
	CDocument *doc = GetDocument();

	// count the number of views on the document
	enum { MAX_POS=1024 };
	POSITION all_pos[MAX_POS];
	int pos_index = -1;
	int this_pos = -1;

	POSITION pos = doc->GetFirstViewPosition();
	while( pos != NULL && pos_index < MAX_POS )
		{
		pos_index++;
		all_pos[pos_index] = pos;

		CView* view = doc->GetNextView( pos );
		if( view == this )
			this_pos = pos_index;
		}   

	int prev_pos = this_pos - 1;
	if( prev_pos < 0 )
		// the last slot used
		prev_pos = pos_index;

	CView* view = doc->GetNextView( all_pos[prev_pos] );
	CMDIChildWnd *mdi_child = (CMDIChildWnd *)view->GetParent();
	mdi_child->MDIActivate();
	}

void CWinemacsView::fixup_scroll_bars()
	{
	int window_index = 0;
	int num_rect_used = 0;

	if( display_scroll_bars )
		{
# if DBG_DISPLAY
		if( dbg_flags & DBG_DISPLAY )
			_dbg_msg("fixup_scroll_bars: end");
# endif

		EmacsWindow *w = windows.windows;

		int x = 0;
		int y = 0;

		EmacsWindow *old_window = windows.currentWindow();
		EmacsBufferRef old( bf_cur );

		// while windows and not the minibuffer window and there are scroll bars left
		while( w != NULL && w->w_next != NULL && window_index < max_scroll_bars )
			{
			if( vert_scroll[window_index] == NULL )
				{
				CScrollBarVertical *sb = new CScrollBarVertical( windows );
				vert_scroll[window_index] = sb;
				sb->Create( WS_CHILD|WS_VISIBLE|SBS_VERT, CRect( 10, 10, 100, 20 ), this, 32000 );
				sb->SetScrollRange( 0, V_SCROLL_RANGE-1, FALSE );
				}

			if( horiz_scroll[window_index] == NULL )
				{
				CScrollBarHorizontal *sb = new CScrollBarHorizontal( windows );
				horiz_scroll[window_index] = sb;
				sb->Create( WS_CHILD|WS_VISIBLE|SBS_HORZ, CRect( 10, 10, 20, 100 ), this, 32000 );
				sb->SetScrollRange( 1, H_SCROLL_RANGE, FALSE );
				}

			if( w->w_height > 5 )
				{
				RECT &v = scroll_bar_region_data.rects[num_rect_used];
				num_rect_used++;

				v.left = WINDOW_BORDER + nCharWidth*(x + w->w_width);
				v.top = WINDOW_BORDER + nCharHeight*(y+1) - 1;			// one row down
				v.right = v.left + nCharWidth*vertical_bar_width;
				v.bottom = v.top + nCharHeight*(w->w_height-1-2);		// one from the bottom

				vert_scroll[window_index]->MoveWindow
					(
					WINDOW_BORDER + nCharWidth*(x + w->w_width),
					WINDOW_BORDER + nCharHeight*(y+1) - 1,	// one row down
					nCharWidth*vertical_bar_width,
					nCharHeight*(w->w_height-1-2)		// one from the bottom
					);
				vert_scroll[window_index]->win = w;
				w->set_win();
				vert_scroll[window_index]->setPosition();
				vert_scroll[window_index]->ShowWindow( SW_SHOW );
				}
			else
				vert_scroll[window_index]->ShowWindow( SW_HIDE );

			if( w->w_width > 20 )
				{
				RECT &h = scroll_bar_region_data.rects[num_rect_used];
				num_rect_used++;

				h.left = WINDOW_BORDER + nCharWidth*(x + w->w_width - 10);
				h.top = WINDOW_BORDER + nCharHeight*(y + w->w_height - 1) - 1;
				h.right = h.left + nCharWidth*10;
				h.bottom = h.top + nCharHeight;

				horiz_scroll[window_index]->MoveWindow
					(
					WINDOW_BORDER + nCharWidth*(x + w->w_width - 10),
					WINDOW_BORDER + nCharHeight*(y + w->w_height - 1) - 1,
					nCharWidth*10,
					nCharHeight
					);
				horiz_scroll[window_index]->win = w;
				horiz_scroll[window_index]->setPosition();
				horiz_scroll[window_index]->ShowWindow( SW_SHOW );
				}
			else
				horiz_scroll[window_index]->ShowWindow( SW_HIDE );

			//
			// adjust x and y
			//
			if( w->w_right != NULL )
				// next is beside this one
				x += w->w_width + vertical_bar_width;
			else
				// next is below this one
				x = 0, y += w->w_height;
			// step to the next scroll_bar
			window_index++;

			// step to the next window
			w = w->w_next;
			}

		old_window->set_win();
		old.set_bf();
		}

	// complete the region data
	scroll_bar_region_data.header.nCount = num_rect_used;

	// unmanage all the scroll bars that are not in use
	while( window_index < max_scroll_bars )
		{
		if( vert_scroll[window_index] != NULL )
			vert_scroll[window_index]->ShowWindow( SW_HIDE );
		if( horiz_scroll[window_index] != NULL )
			horiz_scroll[window_index]->ShowWindow( SW_HIDE );
		// step to next
		window_index++;
		}
	}

/////////////////////////////////////////////////////////////////////////////
// CScrollBarVertical

CScrollBarVertical::CScrollBarVertical( EmacsWindowGroup &_group )
	: CScrollBar()
	, win( NULL )
	, group( _group )
	{ }

CScrollBarVertical::~CScrollBarVertical()
	{ }


BEGIN_MESSAGE_MAP(CScrollBarVertical, CScrollBar)
	//{{AFX_MSG_MAP(CScrollBarVertical)
	ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollBarVertical message handlers
/////////////////////////////////////////////////////////////////////////////
// CScrollBarHorizontal

CScrollBarHorizontal::CScrollBarHorizontal( EmacsWindowGroup &_group )
	: CScrollBar()
	, win( NULL )
	, group( _group )
	{ }

CScrollBarHorizontal::~CScrollBarHorizontal()
	{ }


BEGIN_MESSAGE_MAP(CScrollBarHorizontal, CScrollBar)
	//{{AFX_MSG_MAP(CScrollBarHorizontal)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScrollBarHorizontal message handlers

void CScrollBarHorizontal::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
	{
	if( win == NULL )
		return;

	int scroll = win->w_horizontal_scroll;

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

	win->w_horizontal_scroll = scroll;
	win->w_user_horizontal_scrolled = true;	// tell auto scroll its the scroll bars being clicked on

	setPosition();

	cant_1line_opt = 1;
	redo_modes = 1;

	group.do_dsp();
	}

void CScrollBarVertical::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* /*pScrollBar*/) 
	{
	int scroll = 0;       

	EmacsWindow *old_window = theActiveView->windows.currentWindow();
	win->set_win();

	switch( nSBCode )
		{
	case SB_LINEUP:
		scroll = -2; break;
	case SB_LINEDOWN:
		scroll = 1; break;
	case SB_PAGEUP:
		scroll = - win->w_height * 4 / 5; break;
	case SB_PAGEDOWN:
		scroll = win->w_height * 4 / 5; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		{
		int n = (int)((float)bf_cur->num_characters() * (float)nPos / (float)V_SCROLL_RANGE);
		if( n < 1 )
			n = 1;
		if( n > bf_cur->num_characters() )
			n = bf_cur->num_characters() + 1;
		if( n != (bf_cur->num_characters() + 1) )
			set_dot( scan_bf('\n', n, -1) );
		}
		break;
	case SB_TOP:
		set_dot( 1 ); break;
	case SB_BOTTOM:
		set_dot( bf_cur->num_characters() ); break;
	default:
		break;
		}

	if( scroll )
		{
		// turn off the shift state before scrolling
		gui_input_shift_state( false );

		set_dot( scan_bf( '\n', dot, scroll ) );
		win->setWindowStart( scan_bf( '\n', win->getWindowStart(), scroll ) );
		}

	setPosition();

	old_window->set_win();

	int old_scroll_step = scroll_step;
	scroll_step = 1;
		group.do_dsp();
	scroll_step = old_scroll_step;
	}

void CScrollBarVertical::setPosition()
	{
	if( win == NULL )
		return;

	/* if the position of the scroll bar needs updating then do it */
	int n = (int)((float)V_SCROLL_RANGE * (float)dot / (float)(bf_cur->num_characters()+1));
	if( GetScrollPos() != (int)n )
		SetScrollPos( (int)n, TRUE );
	}

void CScrollBarHorizontal::setPosition()
	{
	if( win == NULL )
		return;

	/* if the position of the scroll bar needs updating then do it */
	if( GetScrollPos() != win->w_horizontal_scroll )
		SetScrollPos( win->w_horizontal_scroll, TRUE );
	}

BOOL CWinemacsView::OnHelpInfo(HELPINFO* pHelpInfo) 
	{
	//
	// if this is context sensitive help for the window
	// return false to allow the F1 key to be passed on
	// to the key down code. Required by NT4.0 changes.
	//
	if( pHelpInfo->iContextType == HELPINFO_WINDOW )
		return FALSE;

	return CView::OnHelpInfo( pHelpInfo );
	}

BOOL CWinemacsView::OnMouseWheel(UINT nFlags, short zDelta, CPoint point) 
	{
	//
	// CSI mode ; delta ; X ; y #w
	// where
	//	delta is always positive
	//
	//	x and y are char positions with 1,1 being the
	//	top left corner of the active window
	//
	//	mode is encoded as a bit mask
	//	bit-0	0 - delta positive, 1 - delta negative
	//	bit-1	reserved - always 0
	//	bit-2	1 - shift is down
	//	bit-3	1 - control is down
	//
	int mode = nFlags&(8|4);
	int delta = zDelta/120;
	if( delta < 0 )
		{
		delta = -delta;
		mode |= 1;
		}

	int mouse_x = (point.x - WINDOW_BORDER)/nCharWidth + 1;
	if( mouse_x <= 0 )
		mouse_x = 1;
	if( mouse_x > t_width )
		mouse_x = t_width;
	int mouse_y = point.y/nCharHeight + 1;


	if( dbg_flags&DBG_KEY )
		_dbg_msg( FormatString("MouseWheel X: %d  Y: %d  mode: %d  delta: %d")
			<< mouse_x << mouse_y << mode << delta );

	EmacsString mw( FormatString("\033[%d;%d;%d;%d#w")
		<< mode << delta << mouse_x << mouse_y );
	k_input_char_string( mw.sdata(), (nFlags&4) != 0 );

	return TRUE;
	}

void CWinemacsView::OnOptionsLogNames() 
	{
	CLogNameOpt log_names;

	//
	// the dialog takes care of itself.
	// no need to check the return code
	//
	log_names.DoModal();
	}

bool CWinemacsView::mapCoordinatedToWindows( int em_x, int em_y, long &win_x, long &win_y )
	{
	win_x = (em_x-1) * nCharWidth + WINDOW_BORDER ;
	win_y = (em_y-1) * nCharHeight;
	return true;
	}

bool CWinemacsView::mapCoordinatedToEmacs( long win_x, long win_y, int &em_x, int &em_y )
	{
	em_x = (win_x - WINDOW_BORDER + (nCharWidth/2))/nCharWidth + 1;
	em_y = win_y/nCharHeight + 1;
	return true;
	}

void CWinemacsView::OnFileOpen()
	{
	theActiveView->k_input_char_string( MENU_PREFIX "fo", false );
	}


//------------------
void CWinemacsView::OnUpdateEditClear(CCmdUI* pCmdUI)
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mark.isSet() );
	}

void CWinemacsView::OnUpdateSearchFind(CCmdUI* pCmdUI)
	{
	pCmdUI->Enable( find_dlg == NULL );
	}
 
void CWinemacsView::OnUpdateSearchReplace(CCmdUI* pCmdUI)
	{
	pCmdUI->Enable( find_dlg == NULL );
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
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mark.isSet() );
	}

void CWinemacsView::OnUpdateEditCut(CCmdUI* pCmdUI)
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mark.isSet() );
	}

void CWinemacsView::OnUpdateFileSave(CCmdUI* pCmdUI)
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_modified > 0 && bf_cur->b_kind == FILEBUFFER );
	}

void CWinemacsView::OnUpdateBufferCaseblindsearch(CCmdUI* pCmdUI) 
	{
	pCmdUI->SetCheck( bf_cur != NULL && bf_cur->b_mode.md_foldcase != 0 );
	}

void CWinemacsView::OnUpdateEditReplaceCase(CCmdUI* pCmdUI) 
	{
	pCmdUI->SetCheck( replace_case );
	}

void CWinemacsView::OnUpdateBufferNarrowregion(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mark.isSet() );
	}

void CWinemacsView::OnUpdateBufferShowwhitespace(CCmdUI* pCmdUI) 
	{
	pCmdUI->SetCheck( bf_cur != NULL && bf_cur->b_mode.md_displaynonprinting != 0 );
	}

void CWinemacsView::OnUpdateBufferWidenregion(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->isRestricted() );
	}

void CWinemacsView::OnUpdateBufferWraplonglines(CCmdUI* pCmdUI) 
	{
	pCmdUI->SetCheck( bf_cur != NULL && bf_cur->b_mode.md_wrap_lines != 0 );
	}

void CWinemacsView::OnUpdateBuildNexterror(CCmdUI* pCmdUI) 
	{
	// always enable this item
	pCmdUI->Enable( true );
	}

void CWinemacsView::OnUpdateBuildPreviouserror(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( error_messages_available() );
	}

void CWinemacsView::OnUpdateEditDelete(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( bf_cur != NULL && bf_cur->b_mark.isSet() );
	}

void CWinemacsView::OnUpdateMacrosRecord(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( !remembering );
	}

void CWinemacsView::OnUpdateMacrosRun(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( !key_mem.isNull() );
	}

void CWinemacsView::OnUpdateMacrosStoprecording(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( remembering );
	}

void CWinemacsView::OnUpdateViewReadonly(CCmdUI* pCmdUI) 
	{
	pCmdUI->SetCheck( bf_cur != NULL && bf_cur->b_mode.md_readonly );
	}

void CWinemacsView::OnViewToolbar() 
	{
	COptionToolbar toolbar_dlg( (CMDIFrameWnd *)AfxGetApp()->m_pMainWnd );

	toolbar_dlg.DoModal();
	}

void CWinemacsView::OnUpdateFileSaveall(CCmdUI* pCmdUI) 
	{
	pCmdUI->Enable( mod_exist() );
	}
