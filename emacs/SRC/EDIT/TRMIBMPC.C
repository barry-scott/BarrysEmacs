/*
 *	terminal control module for IBM PC
 */
#include	<emacs.h>
#include	<ibmbios.h>
#ifdef MOUSE
#include	<ms_mouse.h>
#endif
#include	<i86.h>
#include	<conio.h>

static void check_for_input(struct trmcontrol *tt);

static unsigned char peek_byte( seg, off )
	{
	unsigned char __far *addr = MK_FP( seg, off );

	return addr[0];
	}

static unsigned int peek_uint32( seg, off )
	{
	unsigned int __far *addr = MK_FP( seg, off );

	return addr[0];
	}

static int kread = _KEYBRD_READ;
static int kready = _KEYBRD_READY;
static int kshiftstatus = _KEYBRD_SHIFTSTATUS;

static void wipeline( struct trmcontrol *, int );

#define DISP_MDA	1
#define DISP_CGA	2
#define DISP_EGA	4
#define DISP_VGA	8

static int disp_type = 0;

#define	KBT_simple	0
#define	KBT_enhanced	3
#define	KBT_lk250	2

static keyboard_type = 0;
static int keyboard_override = 0;
static int fix_cursor = 0;
static int cursor_y = 0;

static unsigned short video_segment = 0xb800;

unsigned short *video_ptr = (unsigned short *)(0xb8000);
unsigned char *bios_ptr = (unsigned char *)(0x400);

static union REGS inregs, outregs;
int mouse_present;
int mouse_x = 0, mouse_y = 0, mouse_shift = 0;

static unsigned char special_key[] = "\033__X";
struct conv_keys
	{
	int	key_code;
	unsigned char * translation;
	};

unsigned char mouse_key[32];

static struct conv_keys *cur_convert;

static struct conv_keys ibm_kb_convert[] =
	{
	0x0e08,		u_str( "\177" ),		/* <- */
	0x4d00,		u_str( "\033OC" ),	/* Right */
	0x4b00,		u_str( "\033OD" ),	/* Left */
	0x4800,		u_str( "\033OA" ),	/* Up */
	0x5000,		u_str( "\033OB" ),	/* Down */

	0x3700+'*',	u_str( "\033OS" ),	/* KP * - like LK201 PF4 */
	0x4545,		u_str( "\033OQ" ),
	0x4646,		u_str( "\033OR" ),
	0x4737,		u_str( "\033Ow" ),
	0x4838,		u_str( "\033Ox" ),
	0x4939,		u_str( "\033Oy" ),
	0x4a00+'-',	u_str( "\033Om" ),	/* KP - - like LK201 KP Comma */
	0x4b34,		u_str( "\033Ot" ),
	0x4c35,		u_str( "\033Ou" ),
	0x4d36,		u_str( "\033Ov" ),
	0x4e00+'+',	u_str( "\033Ol" ),	/* KP + - Like KL201 KP Enter */
	0x4f31,		u_str( "\033Oq" ),
	0x5032,		u_str( "\033Or" ),
	0x5133,		u_str( "\033Os" ),
	0x5230,		u_str( "\033Op" ),	/* KP 0 */
	0x532e,		u_str( "\033On" ),

	0x5200, u_str( "\033_A" ),	/* Ins */
	0x4700, u_str( "\033_B" ),	/* Home */
	0x4900, u_str( "\033_C" ),	/* PgUp */
	0x5300, u_str( "\033_D" ),	/* Del */
	0x4f00, u_str( "\033_E" ),	/* End */
	0x5100, u_str( "\033_F" ),	/* PgDn */

	0x3b00, u_str( "\033[17~" ),	/* F1 */
	0x3c00, u_str( "\033[18~" ),	/* F2 */
	0x3d00, u_str( "\033[19~" ),	/* F3 */
	0x3e00, u_str( "\033[20~" ),	/* F4 */
	0x3f00, u_str( "\033[21~" ),	/* F5 */
	0x4000, u_str( "\033[23~" ),	/* F6 */
	0x4100, u_str( "\033[24~" ),	/* F7 */
	0x4200, u_str( "\033[25~" ),	/* F8 */
	0x4300, u_str( "\033[26~" ),	/* F9 */
	0x4400, u_str( "\033[28~" ),	/* F10 */
	0x5400, u_str( "\033[117~" ),	/* S-F1 */
	0x5500, u_str( "\033[118~" ),	/* S-F2 */
	0x5600, u_str( "\033[119~" ),	/* S-F3 */
	0x5700, u_str( "\033[120~" ),	/* S-F4 */
	0x5800, u_str( "\033[121~" ),	/* S-F5 */
	0x5900, u_str( "\033[123~" ),	/* S-F6 */
	0x5a00, u_str( "\033[124~" ),	/* S-F7 */
	0x5b00, u_str( "\033[125~" ),	/* S-F8 */
	0x5c00, u_str( "\033[126~" ),	/* S-F9 */
	0x5454, u_str( "\033[128~" ),	/* S-F10 */

	0,		0
	};

static struct conv_keys ibm_enh_convert[] =
	{
	0x0e08,		u_str( "\177" ),	/* <- */
	0x4de0,		u_str( "\033OC" ),	/* Right */
	0x4be0,		u_str( "\033OD" ),	/* Left */
	0x48e0,		u_str( "\033OA" ),	/* Up */
	0x50e0,		u_str( "\033OB" ),	/* Down */

	0x5e00,		u_str( "\033OP" ),	/* Ctrl-F1 - PF1 */
	0xe02f,		u_str( "\033OQ" ),
	0x372a,		u_str( "\033OR" ),
	0x4a2d,		u_str( "\033OS" ),	/* KP - - like LK201 KP Comma */
	0x4737,		u_str( "\033Ow" ),
	0x4838,		u_str( "\033Ox" ),
	0x4939,		u_str( "\033Oy" ),
	0x4b34,		u_str( "\033Ot" ),
	0x4c35,		u_str( "\033Ou" ),
	0x4d36,		u_str( "\033Ov" ),
	0x4e00+'+',	u_str( "\033Ol" ),	/* l or m KP + - Like KL201 KP Enter */
	0x4f31,		u_str( "\033Oq" ),
	0x5032,		u_str( "\033Or" ),
	0x5133,		u_str( "\033Os" ),
	0x5230,		u_str( "\033Op" ),	/* KP 0 */
	0x532e,		u_str( "\033On" ),

	0xe00d,		u_str( "\033OM" ),	/* Enter */

	0x52e0, u_str( "\033[2~" ),	/* Ins */
	0x47e0, u_str( "\033[1~" ),	/* Home */
	0x49e0, u_str( "\033[5~" ),	/* PgUp */
	0x53e0, u_str( "\033[3~" ),	/* Del */
	0x4fe0, u_str( "\033[4~" ),	/* End */
	0x51e0, u_str( "\033[6~" ),	/* PgDn */

	0x3b00, u_str( "\033[17~" ),	/* F1 */
	0x3c00, u_str( "\033[18~" ),	/* F2 */
	0x3d00, u_str( "\033[19~" ),	/* F3 */
	0x3e00, u_str( "\033[20~" ),	/* F4 */
	0x3f00, u_str( "\033[21~" ),	/* F5 */
	0x4000, u_str( "\033[23~" ),	/* F6 */
	0x4100, u_str( "\033[24~" ),	/* F7 */
	0x4200, u_str( "\033[25~" ),	/* F8 */
	0x4300, u_str( "\033[26~" ),	/* F9 */
	0x4400, u_str( "\033[28~" ),	/* F10 */
	0x5400, u_str( "\033[117~" ),	/* S-F1 */
	0x5500, u_str( "\033[118~" ),	/* S-F2 */
	0x5600, u_str( "\033[119~" ),	/* S-F3 */
	0x5700, u_str( "\033[120~" ),	/* S-F4 */
	0x5800, u_str( "\033[121~" ),	/* S-F5 */
	0x5900, u_str( "\033[123~" ),	/* S-F6 */
	0x5a00, u_str( "\033[124~" ),	/* S-F7 */
	0x5b00, u_str( "\033[125~" ),	/* S-F8 */
	0x5c00, u_str( "\033[126~" ),	/* S-F9 */
	0x5d00, u_str( "\033[128~" ),	/* S-F10 */
	0x5e00, u_str( "\033OP" ),	/* C-F1 */
	0x5f00, u_str( "\033[68~" ),	/* C-F2 */
	0x6000, u_str( "\033[69~" ),	/* C-F3 */
	0x6100, u_str( "\033[70~" ),	/* C-F4 */
	0x6200, u_str( "\033[71~" ),	/* C-F5 */
	0x6300, u_str( "\033[73~" ),	/* C-F6 */
	0x6400, u_str( "\033[74~" ),	/* C-F7 */
	0x6500, u_str( "\033[75~" ),	/* C-F8 */
	0x6600, u_str( "\033[76~" ),	/* C-F9 */
	0x6700, u_str( "\033[78~" ),	/* C-F10 */

	0x8500, u_str( "\033[29~"),	/* F11 */
	0x8600, u_str( "\033[31~"),	/* F12 */
	0x8700, u_str( "\033[129~"),	/* S-F11 */
	0x8800, u_str( "\033[131~"),	/* S-F12 */
	0x8900, u_str( "\033[79~"),	/* C-F11 */
	0x8a00, u_str( "\033[81~"),	/* C-F12 */

	0x84e0,	u_str("\033[55~"),	/* Ctrl-PgUp */
	0x76e0,	u_str("\033[56~"),	/* Ctrl-PgDn */
	0x75e0,	u_str("\033[54~"),	/* Ctrl-End, */
	0x77e0,	u_str("\033[51~"),	/* Ctrl-Home */
	0x73e0,	u_str("\033[60~"),	/* Ctrl-Left */
	0x8de0,	u_str("\033[57~"),	/* Ctrl-Up */
	0x74e0,	u_str("\033[59~"),	/* Ctrl-Right */
	0x91e0,	u_str("\033[58~"),	/* Ctrl-Down */

	0,		0
	};

static struct conv_keys lk250_convert[] =
	{
	0x0e08,		u_str( "\177" ),		/* <- */
	0x5d00,		u_str( "\033OC" ),	/* Right */
	0x5c00,		u_str( "\033OD" ),	/* Left */
	0x5b00,		u_str( "\033OA" ),	/* Up */
	0x5e00,		u_str( "\033OB" ),	/* Down */

	0x6a00,		u_str( "\033OP" ),	/* PF1 */
	0x6b00,		u_str( "\033OQ" ),
	0x6c00,		u_str( "\033OR" ),
	0x6d00,		u_str( "\033OS" ),	/* PF4 */

	0x4737,		u_str( "\033Ow" ),
	0x4838,		u_str( "\033Ox" ),
	0x4939,		u_str( "\033Oy" ),
	0x4a00+'-',	u_str( "\033Om" ),	/* KP - - like LK201 KP Comma */
	0x4b34,		u_str( "\033Ot" ),
	0x4c35,		u_str( "\033Ou" ),
	0x4d36,		u_str( "\033Ov" ),
	0x4e00+'+',	u_str( "\033Ol" ),	/* KP + - Like KL201 KP Enter */
	0x4f31,		u_str( "\033Oq" ),
	0x5032,		u_str( "\033Or" ),
	0x5133,		u_str( "\033Os" ),
	0x5230,		u_str( "\033Op" ),	/* KP 0 */
	0x532e,		u_str( "\033On" ),

	0x6900,		u_str( "\033OM" ),	/* Enter */

	0x5600, u_str( "\033_A" ),	/* Ins */
	0x5500, u_str( "\033_B" ),	/* Home */
	0x5900, u_str( "\033_C" ),	/* PgUp */
	0x5700, u_str( "\033_D" ),	/* Del */
	0x5800, u_str( "\033_E" ),	/* End */
	0x5A00, u_str( "\033_F" ),	/* PgDn */

	0x3b00, u_str( "\033[17~" ),	/* F1 */
	0x3c00, u_str( "\033[18~" ),	/* F2 */
	0x3d00, u_str( "\033[19~" ),	/* F3 */
	0x3e00, u_str( "\033[20~" ),	/* F4 */
	0x3f00, u_str( "\033[21~" ),	/* F5 */
	0x4000, u_str( "\033[23~" ),	/* F6 */
	0x4100, u_str( "\033[24~" ),	/* F7 */
	0x4200, u_str( "\033[25~" ),	/* F8 */
	0x4300, u_str( "\033[26~" ),	/* F9 */
	0x4400, u_str( "\033[28~" ),	/* F10 */
	0x5f00, u_str( "\033[29~" ),	/* F11 */
	0x6000, u_str( "\033[31~" ),	/* F12 */
	0x6100, u_str( "\033_m" ),	/* F13 */
	0x6200, u_str( "\033_n" ),	/* F14 */
	0x6300, u_str( "\033_o" ),	/* F15 */
	0x6400, u_str( "\033_p" ),	/* F16 */
	0x6500, u_str( "\033_q" ),	/* F17 */
	0x6600, u_str( "\033_r" ),	/* F18 */
	0x6700, u_str( "\033_s" ),	/* F19 */
	0x5400, u_str( "\033_t" ),	/* F20 */

	0,		0
	};

#define RGB( r,g,b ) ((r<<16)|(g<<8)|b)
typedef int COLORREF;

struct attr
	{
	COLORREF fg;
	COLORREF bg;
	unsigned char colour;
	};
static struct attr attr_array[256], attr_high;

/* Use this mask to restrict the RGB values to the
 * set that the VGA text mode will support */
#define COLOUR_MASK RGB(192,192,192)

COLORREF colours[16] =
	{
	RGB(   0,   0,   0 )&COLOUR_MASK,	/* 0 black */
	RGB(   0,   0, 127 )&COLOUR_MASK,	/* 1 blue */
	RGB(   0, 127,   0 )&COLOUR_MASK,	/* 2 green */
	RGB(   0, 127, 127 )&COLOUR_MASK,	/* 3 cyan */
	RGB( 127,   0,   0 )&COLOUR_MASK,	/* 4 red */
	RGB( 127,   0, 127 )&COLOUR_MASK,	/* 5 magenta */
	RGB( 127, 127,   0 )&COLOUR_MASK,	/* 6 brown */
	RGB( 127, 127, 127 )&COLOUR_MASK,	/* 7 ltgrey */
	RGB(   0,   0,   0 )&COLOUR_MASK,	/* 8 black */
	RGB(   0,   0, 255 )&COLOUR_MASK,	/* 9 lt blue */
	RGB(   0, 255,   0 )&COLOUR_MASK,	/* 10 lt green */
	RGB(   0, 255, 255 )&COLOUR_MASK,	/* 11 lt cyan */
	RGB( 255,   0,   0 )&COLOUR_MASK,	/* 12 lt red */
	RGB( 255,   0, 255 )&COLOUR_MASK,	/* 13 lt magenta */
	RGB( 255, 255,   0 )&COLOUR_MASK,	/* 14 yellow */
	RGB( 255, 255, 255 )&COLOUR_MASK	/* 15 white */
	};

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

	attr->fg &= COLOUR_MASK;
	attr->bg &= COLOUR_MASK;

	fg = 0;
	bg = 0;
	for( num=15; num>=0; num-- )
		{
		if( colours[num] == attr->fg )
			fg = num;
		if( colours[num] == attr->bg )
			bg = num;
		}
	attr->colour = (bg<<4) | fg;
	}

static void parse_all_renditions(void)
	{
	parse_rendition( &attr_high, region_rendition );
	parse_rendition( &attr_array[SYNTAX_DULL], window_rendition );
	parse_rendition( &attr_array[LINE_ATTR_MODELINE], mode_line_rendition );
	parse_rendition( &attr_array[SYNTAX_WORD], syntax_word_rendition );
	parse_rendition( &attr_array[SYNTAX_STRING], syntax_string_rendition );
	parse_rendition( &attr_array[SYNTAX_COMMENT], syntax_comment_rendition );
	parse_rendition( &attr_array[SYNTAX_KEYWORD1], syntax_keyword1_rendition );
	parse_rendition( &attr_array[SYNTAX_KEYWORD2], syntax_keyword2_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+1], user_1_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+2], user_2_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+3], user_3_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+4], user_4_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+5], user_5_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+6], user_6_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+7], user_7_rendition );
	parse_rendition( &attr_array[LINE_ATTR_USER+8], user_8_rendition );
	}

static void null_routine( struct trmcontrol *tt ) { return; }
static void null_int_routine( struct trmcontrol *tt, int i) { return; }
static void null_select
	( struct trmcontrol *tt, unsigned char *a, unsigned char *b, unsigned char *c)
	{
	return;
	}

static int update_depth = -1;
static void update_begin( struct trmcontrol *tt )
	{
	update_depth++;
	if( update_depth == 0 )
		{
		parse_all_renditions();
#ifdef MOUSE
		if( mouse_present )
			mouse_show( 0 );
#endif
		}
	}

static void update_end( struct trmcontrol *tt )
	{
	if( update_depth == 0 )
		{
#ifdef MOUSE
		if( mouse_present )
			mouse_show( 1 );
#endif
		}
	update_depth--;
	}

static void topos (struct trmcontrol *tt,int row, int column)
	{
	union REGS r;

	cursor_y = row - 1;

	r.h.ah = VD_set_cur_pos;
	r.h.bh = 0;
	r.h.dh = (unsigned char)(row - 1);
	r.h.dl = (unsigned char)(column - 1);
	int386( INT_video, &r, &r );
	}

int window_size;

void window_range( struct trmcontrol *tt,int n )
	{
	window_size = n;
	}

void insert_lines( struct trmcontrol *tt,int n )
	{
	memmove
	(
	&video_ptr[ (cursor_y+n) * tt->t_width ],
	&video_ptr[ cursor_y * tt->t_width ],
	(window_size-n-cursor_y) * tt->t_width * 2
	);
	}

void delete_lines( struct trmcontrol *tt,int n )
	{
	memmove
	(
	&video_ptr[ cursor_y * tt->t_width ],
	&video_ptr[ (cursor_y+n) * tt->t_width ],
	(window_size-n-cursor_y) * tt->t_width * 2
	);
	}

static unsigned char saved_video_mode;

static void wipeline (struct trmcontrol *tt,int line)
	{
	tt->t_update_line( tt, NULL, NULL, line );
	}

static void wipescreen( struct trmcontrol *tt )
	{
	int line;

	for( line=1; line<=tt->t_length; line++ )
		wipeline( tt, line );
	}

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

static void init( struct trmcontrol *tt )
	{
	int old_length = tt->t_length;
	union REGS r;
	struct SREGS s;
	int mode;

	if( saved_video_mode != 0 )
		return;

#if	DBG_VECTOR
	memcpy( vector_copy, 0, sizeof( vector_copy ) );
#endif

	/*	Fetch old mode	*/
	r.h.ah = VD_get_mode;
	int386( INT_video, &r, &r );
	if( saved_video_mode == 0 )
		saved_video_mode = r.h.al;

	if( r.h.al == 7 )
		{
		tt->t_length = 25;
		video_segment = 0xb000;
		disp_type = DISP_MDA;
		}
	else
	{
	/* allow for modes that are card specific */
	char *mode_str = get_config_env( "video_mode" );
	int width = 80;
	int height = tt->t_length;

	mode = 3;
	/* parse to get the numbers, but fail back to normal */
	if( mode_str != NULL
	&& sscanf( mode_str, "%d %d %d", &mode, &width, &height ) < 1 )
		{
		mode = 3;
		width = 80;
		height = tt->t_length;
		}

	tt->t_length = height;
	tt->t_width = width;

	video_segment = 0xb800;

	if( mode == 3 )
		if( tt->t_length >= 43 )
			tt->t_length = 50;
		else
			tt->t_length = 25;

	/*	Set to video mode	*/
	r.h.ah = VD_set_mode;
	r.h.al = (unsigned char)mode;
	int386( INT_video, &r, &r );

	/* setup the disp_type variable */

	/* get the VGA save/restore buffer size */
	r.x.eax = 0x1c00;
	r.x.ecx = 7;
	int386( INT_video, &r, &r );
	if( r.h.al == 0x1c )
		/* its a VGA */
		disp_type = DISP_VGA;
	else
		{
		/* EGA/VGA return video information */
		r.h.ah = VD_alt_select;
		r.x.ebx = 0xff10;
		int386( INT_video, &r, &r );

		if( (r.x.ebx & 0xfefc) == 0)
			/* its an EGA */
			disp_type = DISP_EGA;
		else
			/* its a CGA */
			disp_type = DISP_CGA;
		}

	if( mode == 3 )
	    {
	    if( tt->t_length == 50 )
		switch( disp_type )
		{
		case DISP_VGA:
			tt->t_length = 50; break;
		case DISP_EGA:
			tt->t_length = 43; break;
		case DISP_MDA:
		case DISP_CGA:
		default:
			tt->t_length = 25;
		}

	    if( tt->t_length != 25 )
		{
		/*	Load 8x8 font	*/
		r.h.ah = VD_char_gen;
		r.h.al = 0x12;
		r.h.bl = 0;
		int386( INT_video, &r, &r );
		/*	Load new printscreen??? */
		r.h.ah = VD_alt_select;
		r.h.al = 0;
		r.h.bl = VD_AS_alt_prt_screen;
		int386( INT_video, &r, &r );
		/*	Load cursor scan lines	*/
		r.h.ah = VD_set_cur_type;
		r.h.ch = (unsigned char)(term_cursor_mode ? 1 : 7);
		r.h.cl = 7;
		int386( INT_video, &r, &r );
		/*	patch bug in load cursor scan lines	*/
		if( disp_type&DISP_EGA && fix_cursor )
			{
			short * port_6845;
			port_6845 = (short *)&bios_ptr[0x63];

			outp( *port_6845, 10 );
			outp( *port_6845+1, term_cursor_mode ? 1 : 6 );
			}
		}
	    else
		{
		/*	Load cursor scan lines	*/
		r.h.ah = VD_get_cur_pos;
		int386( INT_video, &r, &r );

		r.h.ah = VD_set_cur_type;
		if( term_cursor_mode )
			r.h.ch = 1;
		int386( INT_video, &r, &r );

		/*	patch bug in load cursor scan lines	*/
		if( disp_type&DISP_EGA && fix_cursor )
			{
			short * port_6845;

			port_6845 = (short *)&bios_ptr[0x63];

			outp( *port_6845, 10 );
			outp( *port_6845+1, term_cursor_mode ? 1 : 7 );
			}
		}
	    }
	else
	    {
	    r.h.ah = VD_get_mode;
	    int386( INT_video, &r, &r );
	    tt->t_width = (int)(unsigned)r.h.ah;
	    r.x.eax = 0x1130;
	    r.h.bh = 2;
	    segread( &s );
	    int386x( INT_video, &r, &r, &s );
	    tt->t_length = ((int)(unsigned)r.h.dl);
	    if( disp_type&DISP_VGA )
		tt->t_length++;
	    }
	}

	if( disp_type&(DISP_EGA|DISP_VGA) )
		{
		inregs.h.ah = VD_set_palette;	/* Set palette registers */
		inregs.h.al = 3;		/* Toggle intensify/Blink */
		inregs.h.bl = 0;		/* 0 = intensity */
		int386( INT_video, &inregs, &outregs );
		}

	video_ptr = (unsigned short *)(video_segment<<4);

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
		int386( 0x15, &inregs, &outregs );
		if( outregs.x.eax == 0x1234 )
			keyboard_type = KBT_lk250;

		/*
		 * If bit 4 of the byte at 0x0040:0x0096 is set, the new
		 * keyboard is present.
		 */
		if( peek_byte( 0x0040, 0x0096 ) & 0x10 )
			{
			kread = _NKEYBRD_READ;
			kready = _NKEYBRD_READY;
			kshiftstatus = _NKEYBRD_SHIFTSTATUS;
			if( keyboard_type != KBT_lk250 )
				keyboard_type = KBT_enhanced;
			}
		else
			if( keyboard_type != KBT_lk250 )
				keyboard_type = KBT_simple;
		}

	switch( keyboard_type )
	{
	case KBT_lk250:		cur_convert = lk250_convert;	break;
	case KBT_enhanced:	cur_convert = ibm_enh_convert;	break;
	default:		cur_convert = ibm_kb_convert;	break;
	}

#ifdef MOUSE
	/* turn off the mouse in the case of a re init */
	mouse_present = mouse_reset();
	if( !mouse_enable )
		mouse_present = 0;

	if( mouse_present )
		{
		mouse_double_speed( 10 );

		if( mode == 3 || mode == 7 )
			switch( tt->t_length )
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
		else
			mouse_set_limits( 0, 640 - 1, 0, 400 - 1 );

		mouse_set_text( 0xffff, 0x7700 );

		mouse_show( 1 );
		}
#endif

	if( old_length != 0 && tt->t_length != old_length )
		{
		tt->t_geometry_change(tt);
		}

	term_keyboard = keyboard_type == KBT_lk250 ? 250 :
			keyboard_type == KBT_enhanced ? 101 : 83;
	tt->t_wipe_screen( tt );
	}

static void cleanup (struct trmcontrol *tt)
	{
	union REGS r;

#ifdef	MOUSE
	mouse_present = mouse_reset();
#endif

	if( saved_video_mode != 0 )
		{
		r.h.ah = VD_set_mode;
		r.h.al = saved_video_mode;
		int386( INT_video, &r, &r );
		saved_video_mode = 0;

		if( disp_type&(DISP_EGA|DISP_VGA) )
			{
			inregs.h.ah = VD_set_palette;	/* Set palette registers */
			inregs.h.al = 3;		/* Toggle intensify/Blink */
			inregs.h.bl = 0;
			int386( INT_video, &inregs, &outregs );
			}
		}

	if( keyboard_type == KBT_lk250 )
		{
		inregs.h.ah = 0x50;
		inregs.h.al = 0x00;
		int386( 0x15, &inregs, &outregs );
		}

	topos( tt, tt->t_length, 1 );
	printf( "\n\n%s\n\n", version_string );
	}

static void change_attributes( struct trmcontrol *tt )
	{
	tt->t_cleanup(tt);
	tt->t_init(tt);
	}

void ring_the_bell(struct trmcontrol *tt)
	{
	inregs.h.ah = 0x0e;
	inregs.h.al = '\007';
	inregs.h.bl = 0;
	int386( 0x10, &inregs, &outregs );
	}

static void input_string( struct trmcontrol *tt, unsigned char *str )
	{
	while( *str )
		tt->k_input_char( tt, *str++ );
	}


#define ONE_DAY (24*65536)
static unsigned int due_tick_count;
static void (*timeout_handler)(void);
void time_schedule_timeout( void (*time_handle_timeout)(void), int delta )
	{
	timeout_handler = time_handle_timeout;
	
	due_tick_count = peek_uint32( 0x40, 0x6c );
	due_tick_count += (delta+27)*10/549;	/* turn into DOS clock ticks */
	due_tick_count %= ONE_DAY;
	}

void time_cancel_timeout(void)
	{
	timeout_handler = NULL;
	}

/*
 *	tt_input_pending returns true if there is a character waiting
 *	to be read.
 */
#define CHECK_FOR_ACTIVITY_INTERVAL 100
static void check_for_input(struct trmcontrol *tt)
	{
	static check_counter = 0;
	struct conv_keys *p;
	int ret_char;

	check_counter++;
	check_counter %= CHECK_FOR_ACTIVITY_INTERVAL;
	if( check_counter != 0 )
		return;

	ret_char = 0;

	if( timeout_handler != NULL )
		{
		unsigned int cur_tick_count = peek_uint32( 0x40, 0x6c );

		/* if the timeout has occurred */
		if( (cur_tick_count - due_tick_count)%ONE_DAY < (ONE_DAY/2) )
			{
			void (*handler)(void) = timeout_handler;
			timeout_handler = NULL;
			handler();
			}
		}

#ifdef	MOUSE
	{
	int x, y;

	if( mouse_present )
		{
		int button = 0;
		if( mouse_press( 0, &x, &y ) )
			button = 2;
		else if( mouse_press( 1, &x, &y ) )
			button = 6;
		else if( mouse_press( 2, &x, &y ) )
			button = 4;
		else if( mouse_release( 0, &x, &y ) )
			button = 3;
		else if( mouse_release( 1, &x, &y ) )
			button = 7;
		else if( mouse_release( 2, &x, &y ) )
			button = 5;
		if( button )
			{
			mouse_x = (INT)(x >> 3) + 1;
			mouse_y = (INT)(y >> 3) + 1;

			sprintfl( s_str(mouse_key), sizeof(mouse_key), "\x1b[%d;%d;%d;%d&w",
				button, 0, mouse_y, mouse_x );
			input_string( tt, mouse_key );
#if	DBG_KEY
			if( dbg_flags&DBG_KEY )
				debug
				(
				-1,
				u_str( "Mouse key:%x  Shift:%x  X:%d  Y:%d" ),
					ret_char, mouse_shift,
					(int)mouse_x, (int)mouse_y
				);
#endif

			return;
			}
		}
	}
#endif

	if( _bios_keybrd( kready ) )
		{
		int shift_state = _bios_keybrd( kshiftstatus );
		/* read next char */
		ret_char = _bios_keybrd( kread );
#if	DBG_KEY
		if( dbg_flags&DBG_KEY )
			debug
			(
			-1,
			u_str( "Key: %x Shift: %x  keytype: %d  kread: %d" ),
			ret_char,
			_bios_keybrd( kshiftstatus ),
			keyboard_type, kread
			);
#endif

		/* see if this code need converting */
		for( p = cur_convert; p -> key_code ; p++ )
			if( p -> key_code == ret_char )
				{
				input_string( tt, p -> translation );
				return;
				}
		if( (ret_char&0xff) == 0 || (ret_char&0xff) == 0xe0 )
			{
			special_key[3] = (unsigned char)(ret_char>>8);
			input_string( tt, special_key );
			return;
			}
		/* check for Control-Space */
		if( ret_char == 0x3920 && shift_state&4 )
			ret_char = 0;
		tt->k_input_char( tt, ret_char );
		}
	}

static void display_activity( struct trmcontrol *tt, unsigned char it )
	{
	video_ptr[(tt->t_length-1)*tt->t_width] = 
		attr_array[SYNTAX_WORD].colour<<8 | it;
	}

static struct emacs_line empty_line =
	{
	0,	 		/* hash */
	0,			/* next */
	0,			/* drawcost */
	0,			/* length */
	0,			/* highlighted */
	};

static void update_line( struct trmcontrol *tt, struct emacs_line *old, struct emacs_line *new, int row )
	{
	register union
		{
		unsigned short word;
		unsigned char byte[2];
		} x;
	register unsigned char *p;
	register unsigned short *video;
	int start_col, end_col, last_attr;

	if( new == NULL )
		{
		new = &empty_line;
		}

	/* only write the line if its changed or there is no old line */
	if( old != NULL &&
	new->line_length == old->line_length
	&& memcmp( new->line_body, old->line_body, new->line_length ) == 0
	&& memcmp( new->line_attr, old->line_attr, new->line_length ) == 0 )
		return;

	tt->t_update_begin(tt);

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

		last_attr = new->line_attr[start_col];
		while( end_col < tt->t_width && last_attr == new->line_attr[end_col] )
			end_col++;

		if( last_attr&LINE_M_ATTR_HIGHLIGHT )
			attr = &attr_high;
		else if( last_attr&LINE_ATTR_USER )
			attr = &attr_array[last_attr&LINE_M_ATTR_USER];
		else
			attr = &attr_array[last_attr];

		x.byte[1]= attr->colour;

		video = &video_ptr[(row-1)*tt->t_width + start_col];
		p = &new->line_body[start_col];
		for( ;start_col < end_col; start_col++ )
			{
			x.byte[0] = *p++;
			*video++ = x.word;
			}

		end_col++;
		}

	update_end(tt);
	}

void null_printf( struct trmcontrol *tt, unsigned char *fmt, ... )
	{
	return;
	}

int init_gui_terminal( struct trmcontrol *tt, unsigned char *device )
	{
	return 0;
	}

int init_char_terminal( struct trmcontrol *tt, unsigned char *device )
	{
	char *emacs_env;

	emacs_env = get_profile_string( "Colours", "WindowText" );
	if( emacs_env != NULL )
		_str_cpy( window_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "ModeLine" );
	if( emacs_env != NULL )
		_str_cpy( mode_line_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "HighlightText" );
	if( emacs_env != NULL )
		_str_cpy( region_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "Syntax_keyword1" );
	if( emacs_env != NULL )
		_str_cpy( syntax_keyword1_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "Syntax_keyword2" );
	if( emacs_env != NULL )
		_str_cpy( syntax_keyword2_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "Syntax_word" );
	if( emacs_env != NULL )
		_str_cpy( syntax_word_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "Syntax_string" );
	if( emacs_env != NULL )
		_str_cpy( syntax_string_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "Syntax_comment" );
	if( emacs_env != NULL )
		_str_cpy( syntax_comment_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_1" );
	if( emacs_env != NULL )
		_str_cpy( user_1_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_2" );
	if( emacs_env != NULL )
		_str_cpy( user_2_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_3" );
	if( emacs_env != NULL )
		_str_cpy( user_3_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_4" );
	if( emacs_env != NULL )
		_str_cpy( user_4_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_5" );
	if( emacs_env != NULL )
		_str_cpy( user_5_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_6" );
	if( emacs_env != NULL )
		_str_cpy( user_6_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_7" );
	if( emacs_env != NULL )
		_str_cpy( user_7_rendition, emacs_env );

	emacs_env = get_profile_string( "Colours", "User_8" );
	if( emacs_env != NULL )
		_str_cpy( user_8_rendition, emacs_env );

	emacs_env = get_config_env("EMACS_OPTIONS");
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

	switch( keyboard_type )
	{
	case 250:		keyboard_type = KBT_lk250; break;
	case 101:
	case 102:		keyboard_type = KBT_enhanced; break;
	case 83:
	case 84:
	default:		keyboard_type = KBT_simple; break;
	}

	tt->t_update_begin = update_begin;
	tt->t_update_end = update_end;
	tt->t_update_line = update_line;
	tt->t_init = init;
	tt->t_beep = ring_the_bell;
	tt->t_cleanup = cleanup;
	tt->t_wipe_line = wipeline;
	tt->t_topos = topos;
	tt->t_reset = null_routine;
	tt->t_io_flush = null_routine;
	tt->t_io_printf = null_printf;
	tt->t_width = 80;
	tt->t_length = 25;
	tt->t_insert_mode = null_int_routine;
	tt->t_highlight_mode = null_int_routine;
	tt->k_input_enable = null_routine;
	tt->k_input_disable = null_routine;
	tt->k_check_for_input = check_for_input;
	tt->t_select = null_select;
	tt->t_window = window_range;
	tt->t_inslines = insert_lines;
	tt->t_dellines = delete_lines;
	tt->t_wipe_screen = wipescreen;
	tt->t_change_attributes = change_attributes;
	tt->t_display_activity = display_activity;
	tt->t_il_mf = 0;
	tt->t_il_ov = 10;

	return 1;
	}
