/*
 *	terminal control module for IBM PC
 */
#include <emacs.h>

#define peek_byte( addr )        (*(unsigned char _far *)addr)

static int kread = _KEYBRD_READ;
static int kready = _KEYBRD_READY;
static int kshiftstatus = _KEYBRD_SHIFTSTATUS;

static void wipeline( int );

unsigned char	Keyb_Type,Enable_Num_Lock,CC_Flag,Firmware;

#define DISP_MDA	1
#define DISP_CGA	2
#define DISP_EGA	4
#define DISP_VGA	8

static int disp_type = 0;

#define	KBT_simple	0
#define	KBT_enhanced	3
#define	KBT_lk250	2

INT cursor_mode = 0;
INT mouse_enable = 0;

static keyboard_type = 0;
static int keyboard_override = 0;
static int fix_cursor = 0;
static int cursor_y = 0;
/* static INT9_handler_loaded; */
static int curHL;
static unsigned short video_segment = 0xb800;
static short  *video_ptr = (short *)0xb8000;
static char  *bios_ptr = (char *)0x400;

static union REGS inregs, outregs;
extern int mouse_present;
INT mouse_x = 0, mouse_y = 0, mouse_shift = 0;

static unsigned char * held_char = ustr("");
static unsigned char special_key[] = "\033__X";
struct conv_keys
	{
	int	key_code;
	unsigned char * translation;
	};

unsigned char mouse_key[] = "\033&.";

static struct conv_keys *cur_convert;

static struct conv_keys FAR ibm_enh_convert[] =
	{
	0x0e08,		ustr( "\177" ),		/* <- */
	0x4de0,		ustr( "\033OC" ),	/* Right */
	0x4be0,		ustr( "\033OD" ),	/* Left */
	0x48e0,		ustr( "\033OA" ),	/* Up */
	0x50e0,		ustr( "\033OB" ),	/* Down */

	0x5e00,		ustr( "\033OP" ),	/* Ctrl-F1 - PF1 */
	0xe02f,		ustr( "\033OQ" ),
	0x372a,		ustr( "\033OR" ),
	0x4a2d,		ustr( "\033OS" ),	/* KP - - like LK201 KP Comma */
	0x4737,		ustr( "\033Ow" ),
	0x4838,		ustr( "\033Ox" ),
	0x4939,		ustr( "\033Oy" ),
	0x4b34,		ustr( "\033Ot" ),
	0x4c35,		ustr( "\033Ou" ),
	0x4d36,		ustr( "\033Ov" ),
	0x4e00+'+',	ustr( "\033Ol" ),	/* l or m KP + - Like KL201 KP Enter */
	0x4f31,		ustr( "\033Oq" ),
	0x5032,		ustr( "\033Or" ),
	0x5133,		ustr( "\033Os" ),
	0x5230,		ustr( "\033Op" ),	/* KP 0 */
	0x532e,		ustr( "\033On" ),

	0xe00d,		ustr( "\033OM" ),	/* Enter */

	0x52e0, ustr( "\033_A" ),	/* Ins */
	0x47e0, ustr( "\033_B" ),	/* Home */
	0x49e0, ustr( "\033_C" ),	/* PgUp */
	0x53e0, ustr( "\033_D" ),	/* Del */
	0x4fe0, ustr( "\033_E" ),	/* End */
	0x51e0, ustr( "\033_F" ),	/* PgDn */

	0x3b00, ustr( "\033_a" ),	/* F1 */
	0x3c00, ustr( "\033_b" ),	/* F2 */
	0x3d00, ustr( "\033_c" ),	/* F3 */
	0x3e00, ustr( "\033_d" ),	/* F4 */
	0x3f00, ustr( "\033_e" ),	/* F5 */
	0x4000, ustr( "\033_f" ),	/* F6 */
	0x4100, ustr( "\033_g" ),	/* F7 */
	0x4200, ustr( "\033_h" ),	/* F8 */
	0x4300, ustr( "\033_i" ),	/* F9 */
	0x4400, ustr( "\033_j" ),	/* F10 */
	0x5400, ustr( "\033_k" ),	/* S-F1 */
	0x5500, ustr( "\033_l" ),	/* S-F2 */
	0x5600, ustr( "\033_m" ),	/* S-F3 */
	0x5700, ustr( "\033_n" ),	/* S-F4 */
	0x5800, ustr( "\033_o" ),	/* S-F5 */
	0x5900, ustr( "\033_p" ),	/* S-F6 */
	0x5a00, ustr( "\033_q" ),	/* S-F7 */
	0x5b00, ustr( "\033_r" ),	/* S-F8 */
	0x5c00, ustr( "\033_s" ),	/* S-F9 */
	0x5454, ustr( "\033_t" ),	/* S-F10 */

	0x8500, ustr( "\033_G"),	/* F11 */
	0x8600, ustr( "\033_H"),	/* F12 */
	0x8700, ustr( "\033_M"),	/* S-F11 */
	0x8800, ustr( "\033_N"),	/* S-F12 */

	0,		0
	};

static void null_routine( void ) { return; }

static void update_begin( void )
	{
	if( mouse_present )
		mouse_show( 0 );
	}

static void update_end( void )
	{
	if( mouse_present )
		mouse_show( 1 );
	}

static void topos (int row, int column)
{
	union REGS r;

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		row += DBG_ROWS;
#endif

	cursor_y = row - 1;

	r.h.ah = VD_set_cur_pos;
	r.h.bh = 0;
	r.h.dh = (unsigned char)(row - 1);
	r.h.dl = (unsigned char)(column - 1);
	int86( INT_video, &r, &r );
};

#if _MSC_VER >= 700
#pragma check_pointer(off)
#endif

#if MEMMAP2
int window_size;

void window_range( int n )
	{
	window_size = n;
	}

void insert_lines( int n )
	{
	unsigned short _far *temp;

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		n += DBG_ROWS;
#endif

  	memmove
	(
	&video_ptr[ (cursor_y+n) * ScreenWidth ],
	&video_ptr[ cursor_y * ScreenWidth ],
	(window_size-n-cursor_y) * ScreenWidth * 2
	);
	}

void delete_lines( int n )
	{
	unsigned short _far *temp;

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		n += DBG_ROWS;
#endif

	memmove
	(
	&video_ptr[ cursor_y * ScreenWidth ],
	&video_ptr[ (cursor_y+n) * ScreenWidth ],
	(window_size-n-cursor_y) * ScreenWidth * 2
	);
#endif
	}
#endif

static unsigned char saved_video_mode;

static void wipeline (int line)
	{
	return;
	}

INT attr_text=0x07, attr_mode=0x70;
INT term_screen_length = 25;
INT term_screen_width = 80;
INT term_keyboard = 83;

#if	DBG_VECTOR
long int vector_copy[256];

void check_vector(void)
	{
	int i;
	long int *p;

	FP_SEG( p ) = 0;
	FP_OFF( p ) = 0;

	for( i=0; i<256; i++ )
		{
		if( vector_copy[i] != p[i] )
			printf("Old vector %x: %p New vector: %p\n",
				i, vector_copy[i], p[i] );
		}
	memcpy( vector_copy, 0, sizeof( vector_copy ) );
	}

#endif

static void init( int save_mode )
	{
	int old_length = tt.t_length;
	union REGS r;

#if	DBG_VECTOR
	memcpy( vector_copy, 0, sizeof( vector_copy ) );
#endif

	/*	Fetch old mode	*/
	r.h.ah = VD_get_mode;
	int86( INT_video, &r, &r );
	if( save_mode )
		saved_video_mode = r.h.al;

	if( r.h.al == 7 )
		{
		tt.t_length = 25;
		video_segment = 0xb000;
		}
	else
	{
	video_segment = 0xb800;

	if( (int)term_screen_length >= 43 )
		tt.t_length = 50;
	else
		tt.t_length = 25;

	/*	Set to video mode 3	*/
	r.h.ah = VD_set_mode;
	r.h.al = 3;
	int86( INT_video, &r, &r );
	if( tt.t_length == 50 )
		{
		union REGS r;

		r.x.ax = 0x1c00;
		r.x.cx = 7;
		int86( INT_video, &r, &r );
		if( r.h.al == 0x1c )
			{
			tt.t_length = 50;
			disp_type |= DISP_VGA;
			}
		else
			{
			r.h.ah = VD_alt_select;
			r.x.bx = 0xff10;
			int86( INT_video, &r, &r );

			if( (r.x.bx & 0xfefc) == 0)
				{
				tt.t_length = 43;
				disp_type |= DISP_EGA;
				}
			else
				{
				tt.t_length = 25;
				disp_type |= DISP_CGA;
				}
			}
		}

	if( tt.t_length != 25 )
		{
		/*	Load 8x8 font	*/
		r.h.ah = VD_char_gen;
		r.h.al = 0x12;
		r.h.bl = 0;
		int86( INT_video, &r, &r );
		/*	Load new printscreen??? */
		r.h.ah = VD_alt_select;
		r.h.al = 0;
		r.h.bl = VD_AS_alt_prt_screen;
		int86( INT_video, &r, &r );
		/*	Load cursor scan lines	*/
		r.h.ah = VD_set_cur_type;
		r.h.ch = (unsigned char)(cursor_mode ? 1 : 7);
		r.h.cl = 7;
		int86( INT_video, &r, &r );
		/*	patch bug in load cursor scan lines	*/
		if( disp_type&DISP_EGA && fix_cursor )
			{
			short _far * port_6845;
			port_6845 = (short _far *)&bios_ptr[0x63];
			outp( *port_6845, 10 );
			outp( *port_6845+1, cursor_mode ? 1 : 6 );
			}
		}
	else
		{
		/*	Load cursor scan lines	*/
		r.h.ah = VD_get_cur_pos;
		int86( INT_video, &r, &r );

		r.h.ah = VD_set_cur_type;
		if( cursor_mode )
			r.h.ch = 1;
		int86( INT_video, &r, &r );

		/*	patch bug in load cursor scan lines	*/
		if( disp_type&DISP_EGA && fix_cursor )
			{
			short _far * port_6845;
			port_6845 = (short _far *)&bios_ptr[0x63];
			outp( *port_6845, 10 );
			outp( *port_6845+1, cursor_mode ? 1 : 7 );
			}
		}
	}

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		tt.t_length -= DBG_ROWS;
#endif

	switch( term_keyboard )
	{
	case 250:
		keyboard_type = KBT_lk250; break;
	case 101: case 102:
		keyboard_type = KBT_enhanced; break;
	default:
	case 83: case 84:
		keyboard_type = KBT_simple; break;
	}

	if( !keyboard_override )
		{
		inregs.h.ah = 0x50;
		inregs.h.al = 0x01;
		int86( 0x15, &inregs, &outregs );
		if( outregs.x.ax == 0x1234 )
			keyboard_type = KBT_lk250;

		Enable_Num_Lock = 1;

		/*
		 * If bit 4 of the byte at 0x0040:0x0096 is set, the new
		 * keyboard is present.
		 */
		if( peek_byte( 0x00400096 ) & 0x10 )
			{
			Firmware = 1;
			kread = _NKEYBRD_READ;
			kready = _NKEYBRD_READY;
			kshiftstatus = _NKEYBRD_SHIFTSTATUS;
			keyboard_type = KBT_enhanced;
			}
		else
			keyboard_type = KBT_simple;
		}

	cur_convert = ibm_enh_convert;

	mouse_present = 0;
	if( mouse_enable )
		mouse_present = mouse_reset();

	if( mouse_present )
		{
		mouse_double_speed( 10 );

		switch( tt.t_length )
		{
		case 43:
			mouse_set_limits( 0, 640 - 1, 0, 8*43 - 1 );
			break;
		case 50:
			mouse_set_limits( 0, 640 - 1, 0, 8*50 - 1 );
			break;
		default:
			mouse_set_limits( 0, 640 - 1, 0, 8*25 - 1 );
		}
		mouse_show( 1 );
		}

	if( old_length != 0 && tt.t_length != old_length )
		{
		ChangeScreenHeight( tt.t_length - old_length );
		ScreenGarbaged = 1;
		}

	term_screen_length = tt.t_length;
	term_screen_width = tt.t_width;
	term_keyboard = keyboard_type == KBT_lk250 ? 250 :
			keyboard_type == KBT_enhanced ? 101 : 83;
	}

static void cleanup (void)
	{
	union REGS r;
/*	volatile unsigned int tick;
	int i;	*/

#ifdef	MOUSE
	mouse_present = mouse_reset();
#endif

#define tick_tock (*(unsigned int _far *)0x0040006c)

	if( saved_video_mode != 3 || tt.t_length != 25 )
		{
		r.h.ah = VD_set_mode;
		r.h.al = saved_video_mode;
		int86( INT_video, &r, &r );
		}

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		tt.t_length += DBG_ROWS;
#endif
	}

void ring_the_bell(void)
	{
	inregs.h.ah = 0x0e;
	inregs.h.al = '\007';
	inregs.h.bl = 0;
	int86( 0x10, &inregs, &outregs );
	}

struct TrmControl *TrmIBMPC (void) {
	static struct TrmControl tt;
	char *emacs_env;

	emacs_env = getenv("EMACS_OPTIONS");
	if( emacs_env != 0 )
		{
		char *s=emacs_env;

		while( s != NULL && *s != '\0' )
			{
			if( sscanf( s, "kboverride:%d", &keyboard_override ) == 0
			&&  sscanf( s, "fixcursor:%d",	&fix_cursor ) == 0 )
				{
				printf("Unknown EMACS_OPTIONS %s", s);
				exit(1);
				}			
			s = strchr( s, ',' );
			if( s != NULL )
				s++;
			}
		}

	keyboard_type = KBT_enhanced;

	tt.t_length = 0;
	tt.t_UpdateBegin = update_begin;
	tt.t_UpdateEnd = update_end;
	tt.t_init = init;
	tt.t_feep = ring_the_bell;
	tt.t_cleanup = cleanup;
	tt.t_wipeline = wipeline;
	tt.t_topos = topos;
	tt.t_reset = null_routine;
	tt.t_width = 80;
#if MEMMAP2
	tt.t_window = window_range;
	tt.t_inslines = insert_lines;
	tt.t_dellines = delete_lines;

	tt.t_ILmf = 0;
	tt.t_ILov = 10;
#endif

	return &tt;
};

/*
 * Read character.
 */
ttgetc(void)
{
	int ret_char;

	if( held_char[0] )
		ret_char = *held_char++;
	else
		{
		struct conv_keys *p;

		ret_char = 0;

		for(;;)
			{
#ifdef	MOUSE
			int x, y;

			if( mouse_present )
			{
			if( mouse_press( 0, &x, &y ) )
				ret_char = 'A';
			else if( mouse_press( 1, &x, &y ) )
				ret_char = 'B';
			else if( mouse_press( 2, &x, &y ) )
				ret_char = 'C';
			else if( mouse_release( 0, &x, &y ) )
				ret_char = 'a';
			else if( mouse_release( 1, &x, &y ) )
				ret_char = 'b';
			else if( mouse_release( 2, &x, &y ) )
				ret_char = 'c';
			if( ret_char )
				{
				mouse_shift = _bios_keybrd( kshiftstatus );
				mouse_shift = (mouse_shift&(KEY_sh_left|KEY_sh_right|KEY_sh_ctl|KEY_sh_alt));
				mouse_x = (INT)(x >> 3) + 1;
				mouse_y = (INT)(y >> 3) + 1;
#if	DBG_SCREEN
				if( dbg_flags&DBG_SCREEN )
					mouse_y -= DBG_ROWS;
#endif

				held_char = mouse_key;
				mouse_key[2] = (unsigned char)ret_char;

#if	DBG_KEY
				if( dbg_flags&DBG_KEY )
					debug
					(
					-1,
					ustr( "Mouse key:%x  Shift:%x  X:%d  Y:%d" ),
						ret_char, mouse_shift,
						(int)mouse_x, (int)mouse_y
					);
#endif

				return ttgetc();
				}
			}
#endif
			if( _bios_keybrd( kready ) )
				{
				/* read next char */
				ret_char = _bios_keybrd( kread );
#if	DBG_KEY
				if( dbg_flags&DBG_KEY )
					debug
					(
					-1,
				ustr( "Key: %x Shift: %x  keytype: %d  kread: %d" ),
					ret_char,
					_bios_keybrd( kshiftstatus ),
					keyboard_type, kread
					);
#endif

				break;
				}
			}

		/* see if this code need converting */
		for( p = cur_convert; p -> key_code ; p++ )
			if( p -> key_code == ret_char )
				{
				held_char = p -> translation;
				return ttgetc();
				}
		if( (ret_char&0xff) == 0 || (ret_char&0xff) == 0xe0 )
			{
			held_char = special_key;
			special_key[3] = (unsigned char)(ret_char>>8);
			return ttgetc();
			}
		}
	return( ret_char & 0xff );
}

/*
 *	tt_input_pending returns true if there is a character waiting
 *	to be read.
 */
int tt_input_pending(void)
	{
	if( held_char[0] )
		return 1;
	else
		return _bios_keybrd( kready );
	}

tt_cancel_active()
	{
	int ret;
	unsigned char _far *p;

/*	debug( 0, ustr("tt_cancel_active start\n") );	*/
#if defined( DOS386 )
	p = &bios_ptr[0x17];
#else
	FP_SEG( p ) = 0x40;
	FP_OFF( p ) = 0x17;
#endif

#define magic_shifts	(KEY_sh_left|KEY_sh_right|KEY_sh_alt)
	ret = (*p&magic_shifts) == magic_shifts;

/*	debug( 0, ustr("tt_cancel_active return %d\n"), ret );	*/

	return ret;
	}

void putline( int row, unsigned char *text, int color )
	{
	register union
		{
		unsigned short word;
		unsigned char byte[2];
		} x;
	register unsigned char *p;
	register unsigned short _far *video;
	unsigned short *temp;
	register int col;

#if	DBG_SCREEN
	if( dbg_flags&DBG_SCREEN )
		row += DBG_ROWS;
#endif

	x.byte[1]= (unsigned char)color;

#if	defined( DOS386 )
	video = &video_ptr[row*80];
#else
	FP_SEG(temp) = video_segment;
	FP_OFF(temp) = (row*80)*2 ;
	video = temp;
#endif

	for( p = text, col=0; col < 80; col++ )
		{
		x.byte[0] = *p++;
		*video++ = x.word;
		}
	}
