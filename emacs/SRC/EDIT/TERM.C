/*
 *	V5.0 Emacs
 */
#include <emacs.h>

extern struct emacs_line *phys_screen[MSCREENLENGTH + 1];
extern struct emacs_line *desired_screen[MSCREENLENGTH + 1];

#ifdef vms
#include <descrip.h>
#include <ttdef.h>
#include <tt2def.h>
#endif

static void rms_close(struct trmcontrol *tt);
static void rms_printf(struct trmcontrol *tt, unsigned char *fmt, ...);

/*forward*/ void init_dsp( void );
/*forward*/ void init_terminal( unsigned char *device_name, unsigned char *term_type );
/*forward*/ void startasyncIO( void );
/*forward*/ void rst_dsp( void );
/*forward*/ void set_protocol(int flag);
/*forward*/ void alter_brd_mbx(int flag);
/*forward*/ unsigned char *lookup_key_name(unsigned char *key);

/*forward*/ int check_term_output_size( int value, struct variablename *v );
/*forward*/ void set_activity_character(unsigned char ch);
/*forward*/ void display_activity_character( void *param );

int complete_reinit = 1;

int sys_version;
int term_is_terminal = 1;
int term_cursor_mode=0;
int mouse_enable=1;
int term_keyboard = 83;
int term_ansi=1;
int term_app_keypad=1;
int term_avo;
int term_deccrt;
int term_deccrt2;
int term_deccrt3;
int term_deccrt4;
int term_edit=1;
int term_eightbit=1;
int term_length;
int term_nopadding;
int term_output_buffer_size;
int term_regis;
int term_width;
struct trmcontrol *tt;

#ifdef vms
extern int lib$find_image_symbol
	( struct dsc$descriptor *a, struct dsc$descriptor *b, int * );

#include <em_mess.h>
#define EMACS__INCOMPTERM EMACS$_INCOMPTERM
#else
#define EMACS__INCOMPTERM 1
#endif

void init_dsp( void )
	{
	if( !term_is_terminal )
		return;

 	tt->t_select( tt, mode_line_rendition, region_rendition, window_rendition );
 	tt->t_init( tt );

 	tt->t_select( tt, mode_line_rendition, region_rendition, window_rendition );
 	tt->t_change_attributes( tt );

	term_width	= tt->t_width;
	term_length	= tt->t_length;

	screen_garbaged = 1;
	if( complete_reinit )
		{
	 	tt->t_reset( tt );
		if( tt->t_window != 0 )
		 	tt->t_window(tt, 0);
		}

#if BROADCAST
	reassign_mbx();
#endif
	}

void geometry_change_handler( struct trmcontrol *PNOTUSED(tt) )
	{
	if (wn_cur != NULL)
		{
		fit_windows_to_screen_length();
		screen_garbaged = 1;
		do_dsp(1);
		}
	}

/*
 *	Init a terminal and return the TT structure initialised.
 */
void init_terminal( unsigned char *term_type, unsigned char *device_name )
	{
	int status;

	/*
	 *	allocate the TrmControl structure
	 */
	tt = malloc_struct( trmcontrol );

	/*
	 *	The version number is check by the display handler.
	 */
	tt->t_version = TT_STRUCT_VERSION;

	tt->k_input_char = keyboard_interrupt;
	tt->t_update_line = NULL;
	tt->t_alloc_mem = emacs_malloc;
	tt->t_free_mem = emacs_free;
	tt->t_geometry_change = geometry_change_handler;
	tt->t_phys_screen = phys_screen;
	tt->t_desired_screen = desired_screen;

	/*
	 *	Emacs recognises three types of terminal
	 *	"file" - a batch processed stream
	 *	"char" - an ANSI character terminal
	 *	"gui"  - a graphical user interface
	 */
	if( _str_icmp( u_str("file"), term_type ) == 0 )
		{
		term_is_terminal = 0;
		status = init_file_terminal( tt, device_name );
		}
	else if( _str_icmp( u_str("char"), term_type ) == 0 )
		{
		term_is_terminal = 1;
		status = init_char_terminal( tt, device_name );
		}
	else if( _str_icmp( u_str("gui"), term_type ) == 0 )
		{
		term_is_terminal = 3;
		status = init_gui_terminal( tt, device_name );
		}
	else
		{
		_dbg_msg( u_str("Unknown terminal type %s"), term_type );
		status = 0;
		}
	if( ! status )
		{
#if defined(_WINDOWS)
		_dbg_msg( u_str("Failed to init in trm windows") );
#endif
		emacs_exit( EMACS__INCOMPTERM );
		}

	}

extern int default_rms_attribute;
extern FILE *message_file;
extern FILE *command_file;


static int rms_int_null( struct trmcontrol *PNOTUSED(tt) )
	{
	return 0;
	}

static void rms_void_null( struct trmcontrol *PNOTUSED(tt) )
	{
	return;
	}

static void rms_void_null_int( struct trmcontrol *PNOTUSED(tt), int PNOTUSED(x) )
	{
	return;
	}

int init_file_terminal( struct trmcontrol *tt, unsigned char *file )
	{
	/*
	 *	fake out terminal settings
	 */
	tt->t_length = 24;
	tt->t_width = 80;
	tt->k_input_enable = rms_int_null;
	tt->k_input_disable = rms_int_null;
	tt->t_change_attributes = rms_void_null;
	tt->t_change_width = rms_void_null_int;
	tt->t_cleanup = rms_close;
	tt->t_io_printf = rms_printf;
	tt->t_io_flush = rms_void_null;
	tt->k_check_for_input = rms_void_null;

	/*
	 *	open the (message "...") stream
	 */
#ifdef vms
	tt->t_context = message_file =
		fio_create( u_str("SYS$OUTPUT"), 0, 0, u_str("emacs.lis"), default_rms_attribute );
#else
	tt->t_context = message_file = stdout;
#endif
	if( tt->t_context == 0 )
		emacs_exit( errno );

	/*
	 *	open the get-tty- stream
	 */
	command_file = stdin;
	if( file != NULL && file[0] != '\0' )
		command_file = fio_open
			(
			file,
			0, 0, 0
			);
	tt->k_context = command_file;
	if( tt->k_context == 0 )
		emacs_exit( errno );

	return 1;
	}

static void rms_close(struct trmcontrol *tt)
	{
	if( (FILE *)tt->k_context != stdin )
		fio_close( (FILE *)tt->k_context );
	}

static void rms_printf(struct trmcontrol *tt, unsigned char *fmt, ...)
	{
	va_list argp;

	int size;
	unsigned char buf[128];

	va_start( argp, fmt );

	size = do_print (fmt, &argp, buf, sizeof( buf )-1);
	fio_put( (FILE *)tt->t_context, buf, size );
	}


static int async_io;
void start_async_io( void )
	{
        /*
	 *	Initialize VMS asynchronous Keyboard Input
	 */
	if( async_io ) return;

	re_init_keyboard();

	input_pending = timer_interrupt_occurred;

	if( tt->k_input_enable )
		tt->k_input_enable( tt );
	async_io = 1;
	}

void stop_async_io( void )
	{
	if( tt->k_input_disable )
		tt->k_input_disable( tt );
	async_io = 0;
	}


void rst_dsp( void )
	{
	if( ! term_is_terminal )
		return;

	stop_async_io();
#if BROADCAST
	deassign_mbx();
#endif

	if( tt->t_window )
		tt->t_window( tt, 0 );
	if( tt->t_cleanup != 0 )
		tt->t_cleanup(tt);
	else
		{
		tt->t_topos( tt, tt->t_length, 1 );
		tt->t_wipe_line( tt, 0 );
		}
	}

void set_protocol(int PNOTUSED(flag))
    	{
#ifdef vms
	if( flag )
		tt->t_cur_attributes.v_char_1 |= TT$M_TTSYNC | TT$M_HOSTSYNC;
	else
		tt->t_cur_attributes.v_char_1 &= ~(TT$M_TTSYNC | TT$M_HOSTSYNC);
#endif

	tt->t_change_attributes( tt );
	}


void alter_brd_mbx(int PNOTUSED(flag))
    	{
#ifdef vms
	if( flag )
		{
		tt->t_cur_attributes.v_char_1 |= TT$M_NOBRDCST;
		tt->t_cur_attributes.l_char_2 |= TT2$M_BRDCSTMBX;
		}
	else
		{
		tt->t_cur_attributes.v_char_1 =
				(tt->t_cur_attributes.v_char_1 & ~TT$M_NOBRDCST)
			|
				(tt->t_user_attributes.v_char_1 & TT$M_NOBRDCST);
		tt->t_cur_attributes.l_char_2 =
				(tt->t_cur_attributes.l_char_2 & ~TT2$M_BRDCSTMBX)
			|
				(tt->t_cur_attributes.l_char_2 & TT2$M_BRDCSTMBX);
		}

	tt->t_change_attributes( tt );
#endif
	}


struct key_name
	{
	unsigned char *name;
	unsigned char *value;
	unsigned char *compressed_value;
	};
static struct key_name key_names[] =
	{
	{u_str("comma"),	u_str("\033Ol"),	NULL},
	{u_str("csi"),		u_str("\033["),		NULL},
	{u_str("ctrl-del"),	u_str("\033[53~"),	NULL},
	{u_str("ctrl-do"),	u_str("\033[79~"),	NULL},
	{u_str("ctrl-down"),	u_str("\033[58~"),	NULL},
	{u_str("ctrl-end"),	u_str("\033[54~"),	NULL},
	{u_str("ctrl-f10"),	u_str("\033[71~"),	NULL},
	{u_str("ctrl-f11"),	u_str("\033[73~"),	NULL},
	{u_str("ctrl-f12"),	u_str("\033[74~"),	NULL},
	{u_str("ctrl-f13"),	u_str("\033[75~"),	NULL},
	{u_str("ctrl-f14"),	u_str("\033[76~"),	NULL},
	{u_str("ctrl-f17"),	u_str("\033[81~"),	NULL},
	{u_str("ctrl-f18"),	u_str("\033[82~"),	NULL},
	{u_str("ctrl-f19"),	u_str("\033[83~"),	NULL},
	{u_str("ctrl-f20"),	u_str("\033[84~"),	NULL},
	{u_str("ctrl-f6"),	u_str("\033[67~"),	NULL},
	{u_str("ctrl-f7"),	u_str("\033[68~"),	NULL},
	{u_str("ctrl-f8"),	u_str("\033[69~"),	NULL},
	{u_str("ctrl-f9"),	u_str("\033[70~"),	NULL},
	{u_str("ctrl-home"),	u_str("\033[51~"),	NULL},
	{u_str("ctrl-ins"),	u_str("\033[52~"),	NULL},
	{u_str("ctrl-left"),	u_str("\033[60~"),	NULL},
	{u_str("ctrl-pgdn"),	u_str("\033[56~"),	NULL},
	{u_str("ctrl-pgup"),	u_str("\033[55~"),	NULL},
	{u_str("ctrl-right"),	u_str("\033[59~"),	NULL},
	{u_str("ctrl-up"),	u_str("\033[57~"),	NULL},
	{u_str("do"),		u_str("\033[29~"),	NULL},
	{u_str("dot"),		u_str("\033On"),	NULL},
	{u_str("down"),		u_str("\033[B"),	NULL},
	{u_str("enter"),	u_str("\033OM"),	NULL},
	{u_str("f10"),		u_str("\033[21~"),	NULL},
	{u_str("f11"),		u_str("\033[23~"),	NULL},
	{u_str("f12"),		u_str("\033[24~"),	NULL},
	{u_str("f13"),		u_str("\033[25~"),	NULL},
	{u_str("f14"),		u_str("\033[26~"),	NULL},
	{u_str("f17"),		u_str("\033[31~"),	NULL},
	{u_str("f18"),		u_str("\033[32~"),	NULL},
	{u_str("f19"),		u_str("\033[33~"),	NULL},
	{u_str("f20"),		u_str("\033[34~"),	NULL},
	{u_str("f6"),		u_str("\033[17~"),	NULL},
	{u_str("f7"),		u_str("\033[18~"),	NULL},
	{u_str("f8"),		u_str("\033[19~"),	NULL},
	{u_str("f9"),		u_str("\033[20~"),	NULL},
	{u_str("find"),		u_str("\033[1~"),	NULL},
	{u_str("gold"),		u_str("\033OP"),	NULL},
	{u_str("help"),		u_str("\033[28~"),	NULL},
	{u_str("insert"),	u_str("\033[2~"),	NULL},
	{u_str("insert here"),	u_str("\033[2~"),	NULL},
	{u_str("kp0"),		u_str("\033Op"),	NULL},
	{u_str("kp1"),		u_str("\033Oq"),	NULL},
	{u_str("kp2"),		u_str("\033Or"),	NULL},
	{u_str("kp3"),		u_str("\033Os"),	NULL},
	{u_str("kp4"),		u_str("\033Ot"),	NULL},
	{u_str("kp5"),		u_str("\033Ou"),	NULL},
	{u_str("kp6"),		u_str("\033Ov"),	NULL},
	{u_str("kp7"),		u_str("\033Ow"),	NULL},
	{u_str("kp8"),		u_str("\033Ox"),	NULL},
	{u_str("kp9"),		u_str("\033Oy"),	NULL},
	{u_str("left"),		u_str("\033[D"),	NULL},
	{u_str("menu"),		u_str("\202"),		NULL},
	{u_str("minus"),	u_str("\033Om"),	NULL},
	{u_str("mouse"),	u_str("\033[&w"),	NULL},
	{u_str("mouse-1-down"),	u_str("\033[2&w"),	NULL},
	{u_str("mouse-1-up"),	u_str("\033[3&w"),	NULL},
	{u_str("mouse-2-down"),	u_str("\033[4&w"),	NULL},
	{u_str("mouse-2-up"),	u_str("\033[5&w"),	NULL},
	{u_str("mouse-3-down"),	u_str("\033[6&w"),	NULL},
	{u_str("mouse-3-up"),	u_str("\033[7&w"),	NULL},
	{u_str("mouse-4-down"),	u_str("\033[8&w"),	NULL},
	{u_str("mouse-4-up"),	u_str("\033[9&w"),	NULL},
	{u_str("next"),		u_str("\033[6~"),	NULL},
	{u_str("next screen"),	u_str("\033[6~"),	NULL},
	{u_str("pf1"),		u_str("\033OP"),	NULL},
	{u_str("pf2"),		u_str("\033OQ"),	NULL},
	{u_str("pf3"),		u_str("\033OR"),	NULL},
	{u_str("pf4"),		u_str("\033OS"),	NULL},
	{u_str("prev"),		u_str("\033[5~"),	NULL},
	{u_str("prev screen"),	u_str("\033[5~"),	NULL},
	{u_str("remove"),	u_str("\033[3~"),	NULL},
	{u_str("right"),	u_str("\033[C"),	NULL},
	{u_str("select"),	u_str("\033[4~"),	NULL},
	{u_str("ss3"),		u_str("\033O"),		NULL},
	{u_str("up"),		u_str("\033[A"),	NULL}
	};

#define KEY_NAME_SIZE (sizeof(key_names)/sizeof(struct key_name))


unsigned char *lookup_key_name(unsigned char *key)
	{
	int key_size;
	int hi; int lo;

	key_size = _str_len( key );

	lo = 0;
	hi = KEY_NAME_SIZE-1;

	for(;;)
		{
		int match;
		int mid;
		struct key_name *entry;

		mid = (lo + hi) / 2;
		entry = &key_names[ mid ];
		match = _str_icmp( key, entry->name );
		if( match == 0 )
			return entry->value;
		if( lo >= hi )
			break;
		if( match < 0 )
			hi = mid - 1;
		else
			lo = mid + 1;
		}

	return 0;
	}


static SAVRES int key_compressed_built = -1;

static void build_key_value_tables( void )
	{
	unsigned char output_buf[16];
	int i;

	for( i=0; i<=KEY_NAME_SIZE-1; i += 1 )
		{
		int outlen;
		struct key_name *entry;

		entry = &key_names[ i ];

		convert_key_string
		(
		_str_len( entry->value ), entry->value,
		&outlen, output_buf
		);
		entry->compressed_value = realloc_ustr( entry->compressed_value, outlen+1 );
		if( entry->compressed_value )
			{
			entry->compressed_value[0] = (unsigned char)outlen;
			memcpy( entry->compressed_value+1, output_buf, outlen );
			}
		}

#if 0
	/*
	 *	Sort the list into order
	 */
	for( i=1; i<=key_name_size-1; i += 1 )
		for( j=i; j<=key_name_size-1; j += 1 )
			{

			unsigned char *first;
			unsigned char *second;

			first = key_compressed_values[ j - 1 ];
			second = key_compressed_values[ j ];
			if( ch$gtr
				(
				first[0], first[1],
				second[0], second[1], 0xff ) )
				{
				key_compressed_values[ j ] = first;
				key_compressed_values[ j - 1 ] = second;
				first = key_compressed_names[ j - 1 ];
				second = key_compressed_names[ j ];
				key_compressed_names[ j ] = first;
				key_compressed_names[ j - 1 ] = second;
				}
			}
#endif
	key_compressed_built = cs_modified;
	}

/*
 *	Look for the longest key value that matches at the start of the
 *	chars vector. 
 */
int lookup_key_value(int len, unsigned char *chars, unsigned char **value)
	{
	static struct key_name worst_entry = { u_str(""), u_str(""), u_str("\0") };
	struct key_name *best_entry = &worst_entry;
	int i;
	
	if( cs_modified != key_compressed_built )
		build_key_value_tables();

	for( i=0; i<=KEY_NAME_SIZE-1; i += 1 )
		{
		struct key_name *entry;

		entry = &key_names[ i ];

		if( entry->compressed_value != NULL
		&& entry->compressed_value[0] > best_entry->compressed_value[0]
		&& (int)entry->compressed_value[0] <= len
		&& memcmp
		    (
		    &entry->compressed_value[1], chars,
		    entry->compressed_value[0]
		    ) == 0 )
			best_entry = entry;
		}

	*value = best_entry->name;
	return best_entry->compressed_value[0];
	}


int check_term_output_size( int PNOTUSED(value), struct variablename *PNOTUSED(v) )
	{
	return 0;
	}

void display_activity_character( void *PNOTUSED(param) )
	{
	if( !activity_indicator )
		return;

	tt->t_display_activity( tt, activity_character );
	}

void set_activity_character( unsigned char it )
	{
	if( !activity_indicator )
		return;

	/* if its a new  activity char... */
	if( activity_character != it )
		{
		activity_character = it;

		/* only display a new char if the timer gets to expire */
		time_remove_requests( 'A' );
		time_add_request( 1000, 'A', display_activity_character, NULL );
		}
	}
