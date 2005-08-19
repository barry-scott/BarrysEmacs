#include <term.h>

#ifdef unix
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

extern struct trmcontrol *tt;

int term_expand_c1 = 1;


/*
 *	Emacs output
 *
 *	all characters passed to t_putchar and t_printf are collected
 *	in the output_buffer. When the output_size is greater then
 *	the output_length less the output_overrun size the buffer is
 *	output.
 */

#define OUTPUT_MINIMUM 1
#define OUTPUT_OVERRUN 128
#define OUTPUT_LENGTH 512 + OUTPUT_OVERRUN

int output_size = 0;
unsigned char output_buffer[OUTPUT_LENGTH];

void t_flush( struct trmcontrol *tt )
	{
	if( output_size > 0 )
		write( tt->output_channel, &output_buffer, output_size );
	output_size = 0;
	}

#if 0
int check_term_output_size(int value, int v)
	{

	int status;
	volatile int max_qio_size;
	struct $itmlst_decl get_syi_list;

	$itmlst_init
	(
	itmlst = get_syi_list,
	(itmcod=syi$_maxbuf, bufadr=max_qio_size) );
	status = $getsyiw( itmlst = get_syi_list );
	if( ! status ) value = 0;

	if( value <= output_minimum )
		term_output_buffer_size = output_length - output_overrun
	else
		/*
		 *	The 38 is the over head on the IO in the TT driver
		 *	At this time it is TTY$C_WB_LENGTH = 38. See
		 *	TTYFDT.MAR and TTYDEF.SDL
		 */
		term_output_buffer_size =
			min( value, max_qio_size - 38 - output_overrun );

	/*
	 *	allocate a buffer of the correct size
	 */
	if( output_buffer != 0 ) efree( output_buffer );
	output_buffer = emalloc( term_output_buffer_size + output_overrun );
	output_size = 0;

	return 0;
	}
#endif

void t_print( struct trmcontrol *tt, unsigned char *str)
	{
	unsigned char *s;
	int ch;

	s = str;

	if( ! term_expand_c1 )
		{
		while( (ch = *s++) != 0 )
			{
			output_buffer[ output_size ] = (unsigned char)ch;
			output_size++;
			}
		}
	else
		{
		while( (ch = *s++) != 0 )
			{
			if( ch >= 128 && ch <= 128+31 )
				{
				output_buffer[ output_size ] = 0x1b;
				output_size++;

				ch = ch - 0x40;
				}

			output_buffer[ output_size ] = (unsigned char)ch;
			output_size++;
			}
		}

	if( output_size > term_output_buffer_size )
		t_flush(tt);
	}

void t_putchar(struct trmcontrol *tt, unsigned char ch)
	{
	/*
	 *	If there is no eightbit support on the terminal
	 *	and the character is C1 control fold onto a 7-bit code.
	 */
	if( term_expand_c1
	&& ch >= 128 && ch <= 128+31 )
		{
		output_buffer[ output_size ] = 0x1b;
		output_size++;

		ch = ch - 0x40;
		}

	output_buffer[ output_size ] = ch;
	output_size++;

	if( output_size > term_output_buffer_size )
		t_flush(tt);
	}

int term_init_device( struct trmcontrol *tt )
	{
        term_output_buffer_size = OUTPUT_LENGTH -OUTPUT_OVERRUN;
#if EMACS_USE_TERMIOS
	tt->t_cur_attributes.c_iflag |= IGNBRK;
	tt->t_cur_attributes.c_iflag &=	~(INLCR|ICRNL|IGNCR|ISTRIP);
	tt->t_cur_attributes.c_oflag &=	~(OPOST);
	tt->t_cur_attributes.c_lflag &= ~(ECHO|ICANON|ISIG|IEXTEN);

        tt->t_cur_attributes.c_cc[VMIN] = 6;
        tt->t_cur_attributes.c_cc[VTIME] = 1;

	tcsetattr( tt->input_channel, TCSADRAIN, &tt->t_cur_attributes );
#else
	tt->t_cur_attributes.c_iflag |= IGNBRK;
	tt->t_cur_attributes.c_iflag &=	~(INLCR|ICRNL|IGNCR);
	tt->t_cur_attributes.c_oflag &=	~(ONLCR|OCRNL|ONOCR|ONLRET);
	tt->t_cur_attributes.c_lflag &= ~(ECHO|ICANON|ISIG);

        tt->t_cur_attributes.c_cc[VMIN] = 1;
        tt->t_cur_attributes.c_cc[VTIME] = 0;

	ioctl( tt->input_channel, TIOCSETA, &tt->t_cur_attributes );
#endif
	return 1;
	}

void term_change_attributes( struct trmcontrol *tt )
	{
	term_init_device( tt );
	}

int term_input_enable( struct trmcontrol *PNOTUSED(tt) )
	{
	return 1;
	}

int term_input_disable( struct trmcontrol *PNOTUSED(tt) )
	{
	return 1;
	}

#if !defined(__FreeBSD__)
int baud_convert[CBAUD+1] =
	{ 0, 50, 75, 110, 134, 150, 200, 300, 600, 1200, 1800,
	2400, 4800, 9600, 19200, 38400 };
#endif

void term_select( struct trmcontrol *tt,
	unsigned char *mode, unsigned char *region, unsigned char *win)
	{
	if( !tt->t_attr_valid )
		{
#ifdef TIOCGWINSZ
                struct winsize ws;

                ioctl( tt->input_channel, TIOCGWINSZ, &ws );
                tt->t_width = ws.ws_col;
                tt->t_length = ws.ws_row;
                if( tt->t_width < 20 )
                        tt->t_width = 80;
                if( tt->t_length < 4 )
                        tt->t_length = 25;
#else
                tt->t_width = 80;
                tt->t_length = 25;
#endif
#ifdef EMACS_USE_TERMIOS
		tcgetattr( tt->input_channel, &tt->t_user_attributes );
		tcgetattr( tt->input_channel, &tt->t_cur_attributes );
#else
		ioctl( tt->input_channel, TIOCGETA, &tt->t_user_attributes );
		ioctl( tt->input_channel, TIOCGETA, &tt->t_cur_attributes );
#endif
		tt->t_attr_valid = 1;
		}

	mode_line_rendition_ref = mode;
	region_rendition_ref = region;
	window_rendition_ref = win;

	tt->t_baud_rate = 
#if defined( __FreeBSD__ )
		tt->t_cur_attributes.c_ospeed;
#else
		baud_convert[ tt->t_cur_attributes.c_cflag&CBAUD ];
#endif
	term_width = tt->t_width;
	term_length = tt->t_length;

	tt->t_il_mf = 0;
	tt->t_il_ov = 1;
	tt->t_ic_mf = 1;
	tt->t_ic_ov = 0;

	return;
	}

void term_check_for_input( struct trmcontrol *PNOTUSED(tt) )
	{
	return;
	}

void term_io_channels ( struct trmcontrol *tt, unsigned char *PNOTUSED(device) )
	{
	tt->input_channel = 0;
	tt->output_channel = 1;

	tt->t_io_putchar = t_putchar;
	tt->t_io_print = t_print;
	tt->t_io_flush = t_flush;
	tt->k_input_enable = term_input_enable;
	tt->k_input_disable = term_input_disable;
	tt->k_check_for_input = term_check_for_input;
	tt->t_select = term_select;
	tt->t_change_attributes = term_change_attributes;
	term_length = tt->t_length = 25;
	term_width = tt->t_width = 80;

	term_nopadding = 1;
	

	return;
	}

void term_restore_charactistics(struct trmcontrol *tt)
	{
#ifdef EMACS_USE_TERMIOS
        tcsetattr( tt->input_channel, TCSADRAIN, &tt->t_user_attributes );
#else
        ioctl( tt->input_channel, TIOCSETA, &tt->t_user_attributes );
#endif
	}
