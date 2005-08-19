/*
 *	win_term.c
 *	Copyright (c) 1992-1993 Barry A. Scott
 */

#include <win_incl.h>
#include <emacs.h>
#include <emacswin.h>

/*
 *	Forward routine declarations
 */
void emacs_cls_WM_SIZE(HWND hwnd, UINT state, int cx, int cy);
void emacs_cls_WM_SETFOCUS(HWND hwnd, HWND hwndOldFocus);
void emacs_cls_WM_KILLFOCUS(HWND hwnd, HWND hwndNewFocus);
int emacs_cls_WM_LBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_LBUTTONUP( HWND hwnd, int x, int y, UINT flags );
int emacs_cls_WM_LBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_RBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_RBUTTONUP( HWND hwnd, int x, int y, UINT flags );
int emacs_cls_WM_RBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_MBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_MBUTTONUP( HWND hwnd, int x, int y, UINT flags );
int emacs_cls_WM_MBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
int emacs_cls_WM_BUTTON( int button, HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags );
int emacs_cls_WM_KEYDOWN( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
void emacs_cls_WM_KEYUP( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
void emacs_cls_WM_CHAR(HWND hwnd, UINT ch, int cRepeat);
int emacs_cls_WM_SYSKEYUP( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
int emacs_cls_WM_SYSKEYDOWN( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
/* void emacs_cls_WM_HSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos); */
void emacs_cls_WM_VSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos);
void emacs_cls_WM_PAINT(HWND hwnd);
void emacs_cls_WM_CLOSE(HWND hwnd);


int emacs_win_new_font(void);
void EmacsSetupDC( HWND hwnd, HDC hDC );
void EmacsResetDC(HDC hDC);

static void cleanup (struct trmcontrol * );
void win_update_line( struct trmcontrol *tt, struct emacs_line *old, struct emacs_line *new, int row );
void input_char_string( unsigned char * );

extern struct emacs_line *phys_screen[MSCREENLENGTH + 1];

WORD (WINAPI *SetSigHandler)
	(
	FARPROC handler,
	DWORD FAR *oldhandler,
	WORD FAR *oldtype,
	WORD type,
	WORD mb1
	);

/*
 *	Font selection stuff
 */
CHOOSEFONT emacs_cf;
static LOGFONT emacs_lf;
static char font_name[LF_FACESIZE];
static char szStyle[LF_FACESIZE];

/*
 *	keyboard handling variables
 */
static UINT last_key_down_flags;
static volatile int ctrl_break_hit = 0;

extern int n_init_show_state;

static int		have_hdc;
static int		have_focus;
static HDC		hDC;		/* display-context variable	*/
static TEXTMETRIC	textmetric;
static int		nCharHeight;
static int		nCharWidth;
static int		nCharExtra = 25;
static SINT		char_widths[MSCREENWIDTH];
#define	    Emacs_FONT	SYSTEM_FIXED_FONT	/* font used for display */
static HFONT		hOldFont;
static HBRUSH		white_brush;
static UINT		last_window_size = SIZEICONIC;
static int		last_v_scroll_pos;

static RECT		rClRect;
static RECT		rWnRect;

/*
 *	Emacs interface vaiables
 */
static int mouse_x, mouse_y, mouse_shift;
struct attr {
	COLORREF fg;
	COLORREF bg;
	};
static struct attr attr_text, attr_mode, attr_high;
int cursor_mode=0;

static cursor_x, cursor_y;

#define CARET_WIDTH	(cursor_mode ? (nCharWidth+1) : 2)
#define CARET_HEIGHT	nCharHeight
#define CARET_XPOS	((cursor_x*nCharWidth)-1+WINDOW_BORDER)
#define CARET_YPOS	(cursor_y*nCharHeight)

static char special_key[] = "\033__X";

#if !defined(_NT)
extern int GetLastError(void);
#endif

/* pull in the data tables used by the terminal emulation */
#include <win_term.dat>

#if DBG_DISPLAY
void check_display( void )
	{
	struct window *w = windows;
	int num_lines;
	int x;

	GetClientRect( emacs_hwnd, &rClRect );

	num_lines = (rClRect.bottom - rClRect.top - 2*WINDOW_BORDER)/nCharHeight - DBG_ROWS;
	if( num_lines != tt->t_length )
		fatal_error( 51 );

	x = 0;
	while( w )
		{
		x += w->w_height;
		w = w->w_next;
		}
	if( num_lines != x )
		fatal_error( 52 );
	}
#endif

static void parse_rendition( struct attr *attr, unsigned char *str )
	{
	int num;
	unsigned char *p = str;
	int bold = 0;			/* default to bold off */
	int reverse = 0;		/* reverse off */
	int fg = 0, bg = 7;		/* fg = black, bg = white */
	int fg_r=0, fg_g=0, fg_b=0;
	int bg_r=0, bg_g=0, bg_b=0;
	int rgb_seen = 0;

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
		attr->fg = RGB( fg_r, fg_g, fg_b );
		attr->bg = RGB( bg_r, bg_g, bg_b );
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
		attr->fg = colours[fg+(bold<<3)];
		attr->bg = colours[bg];
		}
	}

static void parse_all_renditions(void)
	{
	parse_rendition( &attr_text, window_rendition );
	parse_rendition( &attr_mode, mode_line_rendition );
	parse_rendition( &attr_high, region_rendition );
	}

WORD WINAPI EXPORT ctrl_break_handler( WORD a, WORD b )
	{
	ctrl_break_hit++;
	return 0;
	}

void emacs_cls_WM_SIZE(HWND hwnd, UINT state, int cx, int cy)
	{
	last_window_size = state;

	switch( state )
	{
	case SIZEFULLSCREEN:
	case SIZENORMAL:
		{
		int old_length;
		int old_width;
		old_length = tt->t_length;
		old_width = tt->t_width;

		GetClientRect( hwnd, &rClRect );
		tt->t_width = (rClRect.right - rClRect.left - 2*WINDOW_BORDER)/nCharWidth;
		if( tt->t_width > MSCREENWIDTH )
			tt->t_width = MSCREENWIDTH;
		tt->t_length = (rClRect.bottom - rClRect.top)/nCharHeight;
		if( tt->t_length > MSCREENLENGTH )
			tt->t_length = MSCREENLENGTH;
		if( tt->t_length < 3 )
			tt->t_length = 3;
		term_width = tt->t_width;
		term_length = tt->t_length;

		if( wn_cur != NULL )
			{
			if( tt->t_length != old_length
			|| tt->t_width != old_width )
				{
				tt->t_geometry_change(tt);
				screen_garbaged = 1;
				do_dsp(1);
				}
			}
#if DBG_DISPLAY
		if( dbg_flags&DBG_DISPLAY )
			check_display();
#endif
		}
		break;
	default:
		break;
	}
	}

void emacs_cls_WM_SETFOCUS(HWND hwnd, HWND hwndOldFocus)
	{
	/* create caret & display */
	have_focus = 1;

	CreateCaret( hwnd, NULL, CARET_WIDTH, CARET_HEIGHT );
	SetCaretPos( CARET_XPOS, CARET_YPOS );
	ShowCaret( hwnd );
	}

void emacs_cls_WM_KILLFOCUS(HWND hwnd, HWND hwndNewFocus)
	{
	if( have_focus )
		{
		HideCaret( hwnd );
		DestroyCaret();
		}

	have_focus = 0;
	}

int emacs_cls_WM_LBUTTONDOWN
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_LBUTTONDOWN-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

int emacs_cls_WM_LBUTTONUP
	( HWND hwnd, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_LBUTTONUP-WM_LBUTTONDOWN,
		hwnd, 0, x, y, flags );
	return 0;
	}

int emacs_cls_WM_LBUTTONDBLCLK
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_LBUTTONDBLCLK-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

int emacs_cls_WM_RBUTTONDOWN
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_RBUTTONDOWN-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

int emacs_cls_WM_RBUTTONUP
	( HWND hwnd, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_RBUTTONUP-WM_LBUTTONDOWN,
		hwnd, 0, x, y, flags );
	return 0;
	}

int emacs_cls_WM_RBUTTONDBLCLK
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_RBUTTONDBLCLK-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

int emacs_cls_WM_MBUTTONDOWN
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_MBUTTONDOWN-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

int emacs_cls_WM_MBUTTONUP
	( HWND hwnd, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_MBUTTONUP-WM_LBUTTONDOWN,
		hwnd, 0, x, y, flags );
	return 0;
	}

int emacs_cls_WM_MBUTTONDBLCLK
	( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags )
	{
	emacs_cls_WM_BUTTON( WM_MBUTTONDBLCLK-WM_LBUTTONDOWN,
		hwnd, fDoubleClick, x, y, flags );
	return 0;
	}

static int mouse_button_map[] =
	{
	2,	/* WM_LBUTTONDOWN */
	3,	/* WM_LBUTTONUP */
	2,	/* WM_LBUTTONDBLCLK */
	6,	/* WM_RBUTTONDOWN */
	7,	/* WM_RBUTTONUP */
	6,	/* WM_RBUTTONDBLCLK */
	4,	/* WM_MBUTTONDOWN */
	5,	/* WM_MBUTTONUP */
	4	/* WM_MBUTTONDBLCLK */
	};

int emacs_cls_WM_BUTTON
	(
	int button,
	HWND hwnd,
	BOOL fDoubleClick,
	int x, int y,
	UINT keyFlags
	)
	{
	unsigned char mouse[64];

	/*
	 *	The mouse control sequence is
	 *	CSI event ; ; y ; x &w
	 */
	if( mouse_enable )
		{
		mouse_x = (x - WINDOW_BORDER)/nCharWidth + 1;
		if( mouse_x <= 0 )
			mouse_x = 1;
		if( mouse_x > tt->t_width )
			mouse_x = tt->t_width;
		mouse_y = y/nCharHeight + 1;
		mouse_shift = keyFlags;
		sprintf( s_str(mouse), "\x1b[%d;%d;%d;%d&w", mouse_button_map[button], 0, mouse_y, mouse_x );
		input_char_string( mouse );
		}

	return 0;
	}

int emacs_cls_WM_KEYDOWN
	(
	HWND hwnd,
	UINT vk,
	BOOL fDown,
	int cRepeat,
	UINT flags
	)
	{
#if	DBG_KEY
	if( dbg_flags&DBG_KEY )
		debug( -2, u_str("Key: vk:%.4x fdown: %d rep:%d flags%.4x"),
		vk, fDown, cRepeat, flags );
#endif

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
/* not require as we now process kf_extended
		if( vk == VK_NUMLOCK || vk == VK_SCROLL )
			{
			BYTE key_state[256];

			GetKeyboardState( key_state );
			key_state[ VK_NUMLOCK ] = 1;
			key_state[ VK_SCROLL ] = 0;
			SetKeyboardState( key_state );
			}
 */
		return 0;
		}

	/* we are not going to handle this one so translate into a WM_CHAR */
	TranslateMessage( &emacs_win_msg );

	return 0;
	}

void emacs_cls_WM_KEYUP
	(
	HWND hwnd,
	UINT vk,
	BOOL fDown,
	int cRepeat,
	UINT flags
	)
	{
	TranslateMessage( &emacs_win_msg );
	return;
	}

int emacs_cls_WM_SYSKEYUP
	(
	HWND hwnd,
	UINT vk,
	BOOL fDown,
	int cRepeat,
	UINT flags
	)
	{
	if( vk != VK_F10 )
		{
		TranslateMessage( &emacs_win_msg );
		FORWARD_WM_SYSKEYUP(hwnd, vk, cRepeat, flags, DefWindowProc );
		}
	return 0;
	}

int emacs_cls_WM_SYSKEYDOWN
	(
	HWND hwnd,
	UINT vk,
	BOOL fDown,
	int cRepeat,
	UINT flags
	)
	{
	if( vk != VK_F10 )
		{
		TranslateMessage( &emacs_win_msg );
		FORWARD_WM_SYSKEYDOWN(hwnd, vk, cRepeat, flags, DefWindowProc );
		}
	else
		{
		if( convert[vk].shift_translation != NULL
		&& GetKeyState( VK_SHIFT ) < 0 )
			input_char_string( u_str(convert[vk].shift_translation) );
		else if( convert[vk].ctrl_translation != NULL
		&& GetKeyState( VK_CONTROL ) < 0 )
			input_char_string( u_str(convert[vk].ctrl_translation) );
		else if( convert[vk].translation != NULL )
			input_char_string( u_str(convert[vk].translation) );
		else
			{
			special_key[3] = (unsigned char)vk;
			input_char_string( u_str(special_key) );
			}
		}
	return 0;
	}

#ifndef KF_EXTENDED
#define KF_EXTENDED (1<<24)
#endif

void emacs_cls_WM_CHAR(HWND hwnd, UINT ch, int cRepeat)
	{
#if	DBG_KEY
	if( dbg_flags&DBG_KEY )
		debug( -1, u_str("Char: %x Rep=%d"), ch, cRepeat );
#endif
	if( ch == VK_RETURN && (last_key_down_flags&KF_EXTENDED) != 0 )
		input_char_string( u_str("\033OM") );
	else if( ch == ' ' && GetKeyState( VK_CONTROL ) < 0 )
		input_char_string( u_str("\200\200") );
	else
		{
		tt->k_input_char( tt, ch );
		}
	}

void emacs_cls_WM_HSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos)
	{ return; }

void emacs_cls_WM_VSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos)
	{
	int n;
	int scroll = 0;
	int old_scroll_step = scroll_step;

	switch( code )
	{
	case SB_LINEUP:
		scroll = -2; break;
	case SB_LINEDOWN:
		scroll = 1; break;
	case SB_PAGEUP:
		scroll = -wn_cur->w_height * 4 / 5; break;
	case SB_PAGEDOWN
:		scroll = wn_cur->w_height * 4 / 5; break;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		n = (int)((float)num_characters * (float)pos / (float)V_SCROLL_RANGE);
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
		SetScrollPos( hwnd, SB_VERT, last_v_scroll_pos, TRUE );
		}

	scroll_step = 1;
	do_dsp(0);
	scroll_step = old_scroll_step;

	return;
	}

void emacs_cls_WM_CLOSE( HWND hwnd )
	{
	/*
	 * tell ttgetc to return -1 which
	 * causes process keys to return
	 * which allows emacs to exit.
	 */
	emacs_quit = 1;
	}

void emacs_cls_WM_PAINT(HWND hwnd)
	{
	PAINTSTRUCT ps;			 /* paint structure */
	RECT rcUpdate;
	int n;

	hDC = BeginPaint (hwnd, &ps);
	have_hdc++;
	if( !IsIconic( hwnd ) )
		{
		EmacsSetupDC( hwnd, hDC );
		if( have_focus )
			{
			HideCaret( hwnd );
			DestroyCaret();
			}

		rcUpdate = ps.rcPaint;
		DPtoLP( hDC,(POINT *) &rcUpdate, 2 );

		/* calculate first and last lines to update */
		for( n = max (0, rcUpdate.top/nCharHeight);
			n < min (tt->t_length, 1+rcUpdate.bottom/nCharHeight); )
			{
			n++;
			win_update_line( tt, NULL, phys_screen[n], n );
			}

		if( have_focus )
			{
			CreateCaret( hwnd, NULL, CARET_WIDTH, CARET_HEIGHT );
			SetCaretPos( CARET_XPOS, CARET_YPOS );
			ShowCaret( hwnd );
			}
		EmacsResetDC( hDC );
		}
	have_hdc--;
	EndPaint(hwnd, &ps);
	}

void EmacsSetupDC( HWND hwnd, HDC hDC )
	{
	GetClientRect( hwnd, &rClRect );
	GetWindowRect( hwnd, &rWnRect );

	/* Set the background mode to opaque, and select the font. */
	SetBkMode( hDC, OPAQUE );
	hOldFont = SelectFont( hDC, emacs_hfont );
	}

void EmacsResetDC(HDC hDC)
	{
	SelectFont(hDC,hOldFont);
	}

static void update_begin(struct trmcontrol *c)
	{
	int n;

	/* if the position of the scroll bar needs updating then do it */
	n = (int)((float)V_SCROLL_RANGE * (float)dot / (float)(num_characters+1));
	if( last_v_scroll_pos != (int)n )
		{
		last_v_scroll_pos = (int)n;
		SetScrollPos( emacs_hwnd, SB_VERT, last_v_scroll_pos, TRUE );
		}

	if( have_hdc == 0 )
		{
		hDC = GetDC( emacs_hwnd );
		EmacsSetupDC( emacs_hwnd, hDC );
		HideCaret( emacs_hwnd );

		parse_all_renditions();
		}
	have_hdc++;
	}

static void update_end(struct trmcontrol *tt)
	{
	have_hdc--;
	if( have_hdc == 0 )
		{
		SetCaretPos( CARET_XPOS, CARET_YPOS );
		EmacsResetDC( hDC );
		ShowCaret( emacs_hwnd );
		ReleaseDC( emacs_hwnd, hDC );
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

void insert_lines( struct trmcontrol *tt, int n )
	{
	RECT scroll_rect;

	scroll_rect.top = cursor_y * nCharHeight;
	scroll_rect.bottom = window_size * nCharHeight;
	scroll_rect.left = 0;
	scroll_rect.right = tt->t_width * nCharWidth + 2*WINDOW_BORDER;

	ScrollDC( hDC, 0, n*nCharHeight, &scroll_rect, &scroll_rect, NULL, NULL );
	}

void delete_lines( struct trmcontrol *tt, int n )
	{
	RECT scroll_rect;

	scroll_rect.top = cursor_y * nCharHeight;
	scroll_rect.bottom = window_size * nCharHeight;
	scroll_rect.left = 0;
	scroll_rect.right = tt->t_width * nCharWidth;

	ScrollDC( hDC, 0, -n*nCharHeight, &scroll_rect, &scroll_rect, NULL, NULL );
	}
#endif

static void cleanup( struct trmcontrol *tt )
	{
	}

static void wipe_line (struct trmcontrol *tt, int line)
	{
	RECT rect;

	rect.top = line * nCharHeight;
	rect.bottom = (line+1) *nCharHeight ;
	rect.left = 0;
	rect.right = tt->t_width * nCharWidth;

	FillRect( hDC, &rect, white_brush );
	};

static void init( struct trmcontrol *tt )
	{
	}

void ring_the_bell(struct trmcontrol *tt )
	{
	MessageBeep( 0 );
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
	update_begin(tt);

	GetClientRect( emacs_hwnd, &rClRect );
	FillRect( hDC, &rClRect, white_brush );

	update_end(tt);
	return;
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

	i = do_print( fmt, argp, buf, sizeof( buf ) );

	_dbg_msg( u_str("%s"), buf );
	}

extern char szClassName[32];
extern void check_for_activity(void);

static void check_for_input( struct trmcontrol *tt )
	{
	check_for_activity();
	}

int init_char_terminal(struct trmcontrol *tt,unsigned char *device)
	{
	return 0;
	}

int init_gui_terminal(struct trmcontrol *tt,unsigned char *device)
	{
	unsigned char buf[128];
	WINDOWPLACEMENT where;

	GetPrivateProfileString( "Colours", "WindowText", "",
		s_str(window_rendition), sizeof( window_rendition )-1, "emacs060.ini" );
	GetPrivateProfileString( "Colours", "ModeLine", "7",
		s_str(mode_line_rendition), sizeof( mode_line_rendition )-1, "emacs060.ini" );
	GetPrivateProfileString( "Colours", "HighlightText", "7",
		s_str(region_rendition), sizeof( region_rendition )-1, "emacs060.ini" );

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
	tt->t_change_attributes = null_tt_routine;
	tt->t_init = init;
	tt->t_cleanup = cleanup;
	tt->t_beep = ring_the_bell;
	tt->t_wipe_line = wipe_line;
#if MEMMAP
	tt->t_window = window_range;
	tt->t_inslines = insert_lines;
	tt->t_dellines = delete_lines;

	tt->t_il_mf = 0;
	tt->t_il_ov = 10;
#endif
	tt->t_io_flush = null_void_routine;
	tt->t_io_printf = win_printf;
	tt->k_check_for_input = check_for_input;

	emacs_hwnd =
		CreateWindow
		(
		szClassName,
		"Emacs " EMACS_VERSION,
		EMACS_WIN_STYLE,	 /* horz & vert scroll bars */
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		emacs_hinst,
		NULL
		);

	if (!emacs_hwnd)
		{
		_dbg_msg( u_str("CreateWindowEx failed %d"), GetLastError() );
		return FALSE;
		}
	SetScrollRange( emacs_hwnd, SB_VERT, 0, V_SCROLL_RANGE-1, FALSE );

	emacs_cf.lStructSize = sizeof(emacs_cf);
	emacs_cf.hwndOwner = emacs_hwnd;
	emacs_cf.lpszStyle = szStyle;

	emacs_cf.Flags = CF_SCREENFONTS;
	emacs_cf.Flags |= CF_ANSIONLY;
	emacs_cf.Flags |= CF_FORCEFONTEXIST;

	emacs_cf.Flags |= CF_LIMITSIZE;
	emacs_cf.nSizeMin = 6;
	emacs_cf.nSizeMax = 36;

	emacs_cf.Flags |= CF_INITTOLOGFONTSTRUCT;
	emacs_cf.lpLogFont = &emacs_lf;

	emacs_lf.lfHeight = GetPrivateProfileInt( "Emacs", "TextFontSize", 9, "emacs060.ini" );
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
	GetPrivateProfileString( "Emacs", "TextFontName", "FixedSys", emacs_lf.lfFaceName, LF_FACESIZE, "emacs060.ini" );

	if( !emacs_win_new_font() )
		return 0;

	white_brush = GetStockBrush( WHITE_BRUSH );

	/*
	 *	Restore the last position of the window
	 */
	buf[0] = '\0';
	GetPrivateProfileString( "Emacs", "Placement", "", s_str(buf), sizeof( buf ), "emacs060.ini" );
	
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
		if( !SetWindowPlacement( emacs_hwnd, &where ) )
			{
			_dbg_msg( u_str("SetWindowPlacement failed  %d"), GetLastError() );
			ShowWindow( emacs_hwnd, n_init_show_state );
			}
		}
	else
		ShowWindow( emacs_hwnd, n_init_show_state );
	UpdateWindow( emacs_hwnd );

	return 1;
	}


/*
 *	tt_input_pending returns true if there is a character waiting
 *	to be read.
 */
#if !defined( VK_LSHIFT )
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5
#endif

int tt_cancel_active(void)
	{
	/* Left and Right Shift with ALT all at once */
	if( GetAsyncKeyState( VK_LSHIFT ) < 0
	&& GetAsyncKeyState( VK_RSHIFT ) < 0
	&& GetAsyncKeyState( VK_MENU ) < 0 )
		return 1;

	return 0;
	}

static struct emacs_line empty_line =
	{
	0,	 		/* hash */
	0,			/* next */
	0,			/* drawcost */
	0,			/* length */
	0,			/* highlighted */
	};

void win_update_line( struct trmcontrol *tt, struct emacs_line *old, struct emacs_line *new, int row )
	{
	int last_attr,start_col,end_col;

	if( new == NULL )
		new = &empty_line;

	/* only write the line if its changed or there is no old line */
	if( old != NULL &&
	new->line_length == old->line_length
	&& memcmp( new->line_body, old->line_body, new->line_length ) == 0
	&& memcmp( new->line_attr, old->line_attr, new->line_length ) == 0 )
		return;

	update_begin(tt);

	/*
	 *	Fill out the line with spaces
	 */
	memset
	(
	new->line_body + new->line_length,
	' ',
	tt->t_width - new->line_length
	);
	/* and an attribute of normal */
	memset
	(
	new->line_attr + new->line_length,
	0,
	tt->t_width - new->line_length
	);

	
	start_col = 0;
	end_col = 1;
	while( start_col < tt->t_width )
		{
		struct attr *attr;
		COLORREF old_colour;

		last_attr = new->line_attr[start_col];
		while( end_col < tt->t_width && last_attr == new->line_attr[end_col] )
			end_col++;

		switch( last_attr )
		{
		case LINE_M_ATTR_MODELINE:	attr = &attr_mode; break;
		case LINE_M_ATTR_HIGHLIGHT:	attr = &attr_high; break;
		default:
		case 0:				attr = &attr_text; break;
		}
		old_colour = SetTextColor( hDC, attr->fg );
		old_colour = SetBkColor( hDC, attr->bg );

		ExtTextOut
		(
		hDC, WINDOW_BORDER+(start_col*nCharWidth), (row-1)*nCharHeight,
		0, NULL,
		s_str(&new->line_body[start_col]), end_col - start_col,
		&char_widths[0]
		);
		start_col = end_col;
		end_col++;
		}

	update_end(tt);
	}

void RstDsp( void )
	{
	cleanup( tt );
	return;
	}

void InitDsp( void )
	{
#if	DBG_PROFILE
	if( !ProfInsChk() )
		dbg_flags &= ~DBG_PROFILE;
	if( DBG_PROFILE&dbg_flags )
		{
		ProfSetup( 64, 0 );
		ProfSampRate( 5, 4 );
		ProfClear();
		ProfStart();
		}
#endif

	screen_garbaged = 1;
	}


void input_char_string( unsigned char *keys )
	{
	int ch;

	while( (ch = *keys++ ) != 0 )
		{
		if( ch == '\200' )
			ch = *keys++ - '\200';
		tt->k_input_char( tt, ch );
		}
	}

int emacs_win_new_font(void)
	{
	HFONT newFont;
	int i;

	newFont = CreateFontIndirect( &emacs_lf );
	if( newFont == NULL )
		{
		_dbg_msg( u_str("CreateFontIndirect failed %d"), GetLastError() );
		return 0;
		}

	hDC = GetDC(emacs_hwnd);

	if( emacs_hfont )
		DeleteFont( emacs_hfont );

	emacs_hfont = newFont;
	hOldFont = SelectFont( hDC, emacs_hfont );
	GetTextMetrics( hDC, &textmetric );
	SelectFont( hDC, hOldFont );
	ReleaseDC( emacs_hwnd, hDC );
	nCharHeight = (int)(textmetric.tmExternalLeading + textmetric.tmHeight);
	nCharWidth = textmetric.tmAveCharWidth
		+ (textmetric.tmMaxCharWidth - textmetric.tmAveCharWidth)*nCharExtra/100;

	for( i=0; i<MSCREENWIDTH; i++ )
		char_widths[i] = nCharWidth;

	if( have_focus )
		{
		HideCaret( emacs_hwnd );
		DestroyCaret();
		CreateCaret( emacs_hwnd, NULL, CARET_WIDTH, CARET_HEIGHT );
		SetCaretPos( CARET_XPOS, CARET_YPOS );
		ShowCaret( emacs_hwnd );
		}

	emacs_cls_WM_SIZE( emacs_hwnd, last_window_size, 0, 0 );

	GetClientRect( emacs_hwnd, &rClRect );
	InvalidateRect( emacs_hwnd, &rClRect, TRUE );

	return 1;
	}

void set_activity_character( unsigned char it )
	{
	activity_character = it;

	update_begin(tt);

	SetTextColor( hDC, attr_text.fg );
	SetBkColor( hDC, attr_text.bg );
	ExtTextOut
	(
	hDC, WINDOW_BORDER+(0*nCharWidth), (term_length-1)*nCharHeight,
	0, NULL,
	s_str(&activity_character), 1,
	&char_widths[0]
	);

	update_end(tt);
	}

void win_update_window_title( void )
	{
	char new_title[256];

	strcpy( new_title, "Emacs " EMACS_VERSION " - " );
	_str_cat( new_title, current_directory );

	SetWindowText( emacs_hwnd, new_title );
	}
