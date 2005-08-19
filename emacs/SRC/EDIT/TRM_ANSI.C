/*	Copyright (c) 1982-1995
		Barry A. Scott and nick Emery */

/*
 * Terminal control module for ANSI terminals
 *
 * Barry A. Scott	 6-Feb-1984
 */

#include <term.h>
#include <syntax.h>

extern int black_on_white;

/*forward*/
static void ansi_hl_mode (struct trmcontrol *tt,int on);
static void ansi_ins_mode(struct trmcontrol *tt, int on );
static void ansi_insert_lines (struct trmcontrol *tt,int n);
static void ansi_delete_lines (struct trmcontrol *tt,int n);
static void ansi_write_chars( struct trmcontrol *tt,unsigned char *start, unsigned char *end );
static void ansi_blanks (struct trmcontrol *tt,int n);
static void pad(struct trmcontrol *tt,int n,float f);
static void ansi_topos_ext( struct trmcontrol *tt,int row, int column );
static void ansi_topos
	(
	struct trmcontrol *tt,int row, int column,
	unsigned char *cur_line,
	unsigned char *cur_line_attr
	);
static void ansi_putchar( struct trmcontrol *tt,unsigned char ch, int region_highlight );
static void ansi_update_line( struct trmcontrol *tt,struct emacs_line *old, struct emacs_line *new_line, int ln );
static void ansi_init_trm( struct trmcontrol *tt );
static void ansi_reset (struct trmcontrol *);
static void ansi_cleanup (struct trmcontrol *);
static void ansi_wipe_line (struct trmcontrol *tt,int);
static void ansi_wipe_screen (struct trmcontrol *tt);
static void ansi_window (struct trmcontrol *tt,int n);
static void ansi_feep (struct trmcontrol *tt);
static void ansi_change_width (struct trmcontrol *tt,int width);
static void t_printf( struct trmcontrol *tt, unsigned char *fmt, ... );



#define	SO 016
#define SI 017

#define PAD( a, b ) do { if( !term_nopadding ) pad( tt, a, b ); } while( 1 == 0 )

unsigned char *mode_line_rendition_ref;
unsigned char *window_rendition_ref;
unsigned char *region_rendition_ref;


static int	ansi_cur_x, ansi_cur_y;
static int	window_size;

static cur_region_highlight, cur_special_graphics;

static cur_hl;
static void ansi_hl_mode (struct trmcontrol *tt,int on)
	{
	if (cur_hl == on)
		return;
	
	if( on )
		tt->t_io_printf( tt, u_str( "\233%sm" ), mode_line_rendition_ref );
	else
		tt->t_io_printf( tt, u_str( "\233%sm" ), window_rendition_ref );

	cur_hl = on;
	}

static cur_ins;
static void ansi_ins_mode( struct trmcontrol *tt, int on )
	{
	if( cur_ins == on ) return;

	if( on )
		tt->t_io_print( tt, u_str( "\2334h" ) );
	else
		tt->t_io_print( tt, u_str( "\2334l" ) );

	cur_ins = on;
	}

static void ansi_insert_lines (struct trmcontrol *tt,int n)
	{
	if( !term_edit )
		{
		tt->t_io_printf( tt, u_str( "\233%d;%dr\233%d;1H" ),
			ansi_cur_y, window_size, ansi_cur_y);
		ansi_cur_x = 1;
		while (--n >= 0)
			{
			tt->t_io_print( tt, u_str( "\033M" ));
			PAD (1, 30.);
			}
		tt->t_io_print( tt, u_str( "\233r" ));
		}
	else
		tt->t_io_printf( tt, u_str( "\233%d;%dr\233%dL\233r" ),
			ansi_cur_y, window_size, n );
	ansi_cur_x = ansi_cur_y = 1;
	}
static void ansi_delete_lines (struct trmcontrol *tt,int n)
	{
	if( !term_edit )
		{
		tt->t_io_printf( tt, u_str( "\233%d;%dr\233%dH" ),
				ansi_cur_y, window_size, window_size);
		ansi_cur_x = 1;
		ansi_cur_y = window_size;
		while (--n >= 0)
			{
			tt->t_io_print( tt, u_str( "\033E" ));
			PAD (1, 30.);		/* [see above comment] */
			}
		tt->t_io_print( tt, u_str( "\233r" ));
		}
	else
		tt->t_io_printf( tt, u_str( "\233%d;%dr\233%dM\233r" ),
			ansi_cur_y, window_size, n );
	ansi_cur_x = ansi_cur_y = 1;
	}

static void ansi_write_chars( struct trmcontrol *tt, unsigned char *start, unsigned char *end )
	{
	while (start <= end)
		{
		tt->t_io_putchar( tt, *start++);
		ansi_cur_x++;
		}
	}

static void ansi_blanks (struct trmcontrol *tt, int n)
	{
	while (--n >= 0)
		{
		tt->t_io_putchar( tt, ' ');
		ansi_cur_x++;
		}
	}

static float baud_factor;
static void pad(struct trmcontrol *tt, int n,float f)
	{
	register int k = (int)(n * f * baud_factor);
	while (--k >= 0)
		tt->t_io_putchar( tt, 0);
	}

static void ansi_topos_ext( struct trmcontrol *tt, int row, int column )
	{
	ansi_topos( tt, row, column, 0, 0 );
	}

static void ansi_topos
	(
	struct trmcontrol *tt,
	int row, int column,
	register unsigned char *cur_line,
	register unsigned char *cur_line_attr
	)
	{
	/*
	 *	This test check_ to see if we may have hit the
	 *	edge of the hardware screen. If so the cursor
	 *	is in fact in ansi_cur_x - 1. Except if the terminal
	 *	is a VWS or a VTxxx in column 80 or 132.
	 *
	 *	The the above conditions cannot be determined do
	 *	absolute cursor movement.
	 */
	if( ansi_cur_x > tt->t_width )
		{
		if( ansi_cur_y == row )
			if( column-1 )
				tt->t_io_printf( tt, u_str( "\015\233%dC" ), column-1 );
			else
				tt->t_io_putchar( tt, '\015');
		else
			tt->t_io_printf( tt, u_str( "\233%d;%dH" ), row, column );
		goto done;
		}
	if (ansi_cur_y == row)
		{
		int diff;
		/*
		 *	movement within a line
		 */
		if( ansi_cur_x == column ) return;

		if( column == 1 )
			{
			tt->t_io_putchar( tt,  '\r' ); goto done;
			}
		if( column > ansi_cur_x )
			{
			/* moving forward along the line */
			diff = column - ansi_cur_x;

			if( cur_line == 0 )
				{
				if( diff == 1 )
					tt->t_io_printf( tt, u_str( "\233C" ));
				else
					tt->t_io_printf( tt, u_str( "\233%dC" ), diff );
				}
			else
				{
				cur_line += ansi_cur_x - 1;
				cur_line_attr += ansi_cur_x - 1;
				switch( diff )
				{
				case 3:	ansi_putchar( tt,  *cur_line++, *cur_line_attr++ );
				case 2:	ansi_putchar( tt,  *cur_line++, *cur_line_attr++ );
				case 1:	ansi_putchar( tt,  *cur_line++, *cur_line_attr++ );
					break;
				default:tt->t_io_printf( tt, u_str( "\233%dC" ), diff );
					break;
				}
				}
			}
		else
			{
			/* moving backward along the line */
			diff = ansi_cur_x - column;
			switch( diff )
			{
			case 3:	tt->t_io_putchar( tt, '\010');
			case 2:	tt->t_io_putchar( tt, '\010');
			case 1:	tt->t_io_putchar( tt, '\010');
				break;
			default:tt->t_io_printf( tt, u_str( "\233%dD" ), diff );
				break;
			}
			}
		goto done;
		}
	if (ansi_cur_y + 1 == row && (column == 1 || column==ansi_cur_x))
		{
		if(column!=ansi_cur_x) tt->t_io_putchar( tt, 015);
		tt->t_io_putchar( tt, 012);
		goto done;
		}
	if (row == 1 && column == 1)
		{
		tt->t_io_print( tt, u_str( "\233H" ));
		goto done;
		}
	tt->t_io_printf( tt, u_str( "\233%d;%dH" ), row, column );
done:
	ansi_cur_x = column;
	ansi_cur_y = row;
	}
static void ansi_putchar( struct trmcontrol *tt, unsigned char ch, int region_highlight )
	{
	if( region_highlight != cur_region_highlight )
		{
		if (region_highlight & LINE_M_ATTR_HIGHLIGHT)
			tt->t_io_printf( tt, u_str( "\233;%sm" ), region_rendition_ref );
		else if (region_highlight & LINE_ATTR_MODELINE)
			tt->t_io_printf( tt, u_str( "\233;%sm" ), mode_line_rendition_ref );
		else
			tt->t_io_printf( tt, u_str( "\233%sm" ), window_rendition_ref );
		cur_region_highlight = region_highlight;
		}
	if( ch >= ' ' )
		{
		if( cur_special_graphics && ch > '^' && ch < '\177' )
			{
			cur_special_graphics = 0;
			tt->t_io_putchar( tt,  SI );
			}
		tt->t_io_putchar( tt,  ch );
		}
	else
		{
		if( !cur_special_graphics )
			{
			cur_special_graphics = 1;
			tt->t_io_putchar( tt,  SO );
			}
		tt->t_io_putchar( tt,  ch+0140 );
		}
	}

static struct emacs_line empty_line;

static void ansi_update_line( struct trmcontrol *tt, struct emacs_line *old, struct emacs_line *new_line, int ln )
	{
	register unsigned char *old_t_p;	/* pointer to old lines text */
	register unsigned char *new_t_p;	/* pointer to new lines text */
	register unsigned char *old_a_p;	/* pointer to old lines attr */
	register unsigned char *new_a_p;	/* pointer to new lines attr */
	register int len;	/* holds min( o_len, n_len ) */
	register int col;	/* column that the software cursor is in */
	register int diff;	/* holds differance between n_len and o_len */
	register unsigned char *cur_line;/* hold a pointer to the start of the new
				   lines text */
	register unsigned char *cur_line_attr;/* hold a pointer to the start of the new
				   lines text */
	int o_len, n_len;	/* the length of the lines */

	if( old == 0 )	old = &empty_line;
	if( new_line == 0 )	new_line = &empty_line;

	n_len = len = new_line->line_length;
	o_len = old->line_length;
	if( o_len == 0 && n_len == 0 ) return;

	cur_line = new_t_p = &new_line->line_body[0];
	cur_line_attr = new_a_p = &new_line->line_attr[0];

	if( len > o_len ) len = o_len;

	old_t_p = &old->line_body[0];
	old_a_p = &old->line_attr[0];

	/* start in the first column */
	col=1;

	/*
	 *	Work out the offset to the first change
	 */
	while( col <= len )
		{
		if( *old_t_p != *new_t_p ) break;
		if( *old_a_p != *new_a_p ) break;

		col++; old_t_p++; new_t_p++; old_a_p++; new_a_p++;
		}
	
	diff = n_len - o_len;

	/*
	 *	If we have character editing see if it can be used
	 */
	if( term_edit )
	/* 
	 *	Only bother to use insert delete if the old and new lines
	 *	are differant, but if the change from old to new is either
	 *	to kill to end of line or insert a string at the end of the
	 *	line do not use INSmode or DeleteChars.
	 */
	if( diff != 0		/* lines must have differant length */
	 && len != col-1	/* differance must not be at the end of either line */
	 && (diff>0 ? diff : -diff ) < 9 /* and the diff must be small */
	 )
		{
		ansi_topos( tt, ln, col, cur_line, cur_line_attr );

		if( diff > 0 )
			{
			/*
			 *	Need to do an insert unsigned char here
			 */
			ansi_cur_x += diff;	/* move cur_x alone */
			ansi_ins_mode(tt,1);
			while( diff-- )
				{
				ansi_putchar( tt,  *new_t_p++, *new_a_p++ );
				col++;
				}
			ansi_ins_mode(tt,0);
			}
		else
			{
			/*
			 *	Need to do a delete unsigned char here
			 */
			diff = -diff;
			tt->t_io_printf( tt, u_str( "\233%dP" ), diff );
			old_t_p += diff;
			old_a_p += diff;
			}
		diff = 0;	/* stop after updating the changed part */
		len = n_len;	/* we can now process to the end of the new
				   line */
		}
	/*
	 *	Update the changed part of the line
	 */
	for(; col <= len;
		col++, old_t_p++, new_t_p++, old_a_p++, new_a_p++ )
		{
		if( *old_t_p == *new_t_p && *old_a_p == *new_a_p )
			continue;

		ansi_topos( tt, ln, col, cur_line, cur_line_attr );
		ansi_putchar( tt,  *new_t_p, *new_a_p );
		ansi_cur_x++;
		}
	/*
	 *	If the old line is longer than the new erase to end of line
	 */
	if( diff == 0 ) goto tidy_up_and_exit;
	if( diff < 0 )
		{
		/*
		 *	The new line is shorter then the old
		 */
		ansi_topos( tt, ln, col, cur_line, cur_line_attr  );

		switch( -diff )
		{
		case 1:	ansi_putchar( tt,  ' ', 0 );
			ansi_cur_x++;
			break;
		default:tt->t_io_print( tt, u_str( "\233K" ));
			break;
		}
		goto tidy_up_and_exit;
		}
	/*
	 *	The new line is longer then the old so
	 *	print the rest of the line.
	 *
	 *	But take into account topos's ability to deal with
	 *	white space if the line is not highlighted.
	 */
	while( diff )
		{
		/* skip spaces and let topos cope with them */
		for( ; diff; diff--, new_t_p++, new_a_p++, col++ )
			if( *new_t_p != ' ' || *new_a_p != 0 ) break;
		/* move over to output the printing unsigned char */
		if( diff )
			ansi_topos( tt, ln, col, cur_line, cur_line_attr );
		/* output chars until a space */
		for(; diff; diff--, col++ )
			{
			if( *new_t_p == ' ' && *new_a_p == 0 ) break;

			ansi_putchar( tt,  *new_t_p++, *new_a_p++ );
			ansi_cur_x++;
			}
		}
tidy_up_and_exit:
	if( cur_region_highlight )
		{
		tt->t_io_printf( tt, u_str( "\233%sm" ), window_rendition_ref );
		cur_region_highlight = 0;
		}
	if( cur_special_graphics )
		{
		cur_special_graphics = 0;
		tt->t_io_putchar( tt,  SI );
		}
	}

static void ansi_init_trm( struct trmcontrol *tt )
	{
	int status;
	status = term_init_device( tt );
	if( (status&1) == 0 )
		return;

	baud_factor = tt->t_baud_rate/9600.0;
	}

static void ansi_reset ( struct trmcontrol *tt )
	{
	/*
	 *	Too reset terminal
	 *		scroll-region		r
	 *		graphic rendition	m
	 *		Insert replace		4l
	 *		Jump scroll		?4l
	 *		Wrap around		?7l
	 *		Clear screen		2J
	 *		Select US chars as G0	(B
	 *		Select Special graphics
	 *			chars as G1	)0
	 *		G0 as current chars	^O
	 *
	 *		Origin mode set		?6h	 for EDIT
	 *		Origin mode reset	?6l	 for NOEDIT
	 */
	tt->t_io_printf
	(
	tt, u_str( "\233r\233m\2334l\233?4l\233?6%c\233?7l\2332J\033(B\033)0\017" ),
	term_edit ? 'h' : 'l'
	);
	PAD (1, 100.);
	window_size = tt->t_length;
	cur_hl = 0; cur_ins = 0;
	ansi_cur_x = ansi_cur_y = 1;
	}

static void ansi_cleanup (struct trmcontrol *tt)
	{
	ansi_hl_mode (tt,0);
	ansi_window (tt,0);
	tt->t_io_printf( tt, u_str( "\233?6l\233%dH\233K" ), window_size );
	tt->t_io_flush(tt);
	term_restore_charactistics(tt);
	}

static void ansi_wipe_line (struct trmcontrol *tt, int line)
	{
	ansi_topos_ext( tt, line, 0 );
	tt->t_io_print( tt, u_str( "\233K" ));
	PAD (1, 3.);
	}

static void ansi_wipe_screen (struct trmcontrol *tt)
	{
	tt->t_io_print( tt, u_str( "\2332J" ));
	PAD (1, 72.);
	}

static void ansi_window (struct trmcontrol *tt, int n)
	{
	if (n <= 0 || n > tt->t_length)
		n = tt->t_length;
	window_size = n;
	}

static void ansi_feep (struct trmcontrol *tt)
	{
	tt->t_io_print( tt, black_on_white ? u_str( "\233?5l" ) : u_str( "\233?5h" ));
	PAD (1, 30.);
	tt->t_io_print( tt, black_on_white ? u_str( "\233?5h" ) : u_str( "\233?5l" ));
	PAD (1, 30.);
	}

static void ansi_change_width (struct trmcontrol *tt, int width)
	{
	tt->t_io_print( tt, width > 80 ? u_str( "\233?3h" ) : u_str( "\233?3l" ));
	PAD (1, 130.);
	}

static void ansi_beep(struct trmcontrol *tt)
	{
	tt->t_io_putchar( tt,  7 );	/* bell */
	}

#ifdef unix
extern int read_inputs( int, unsigned char *, unsigned int );
static int ansi_read( struct trmcontrol *tt, unsigned char *b, unsigned int s )
	{
	return read_inputs( tt->input_channel, b, s );
	}
#endif

static in_update;
static void ansi_update_begin(struct trmcontrol *PNOTUSED(tt))
	{
	in_update = 1;
	}

static void ansi_update_end(struct trmcontrol *tt)
	{
	in_update = 0;
	tt->t_io_flush( tt );
	}

static void ansi_display_activity( struct trmcontrol *tt, unsigned char it )
	{
	if( in_update )
		return;
	tt->t_io_printf(tt, u_str("\033" "7" "\033[%d;1H%c\033" "8"),
		tt->t_length, it );
	tt->t_io_flush( tt );
	}

int init_char_terminal( struct trmcontrol *tt, unsigned char *dev )
	{
	term_io_channels( tt, dev );

	tt->t_update_begin = ansi_update_begin;
	tt->t_update_end = ansi_update_end;

	tt->t_beep = ansi_beep;
	tt->t_insert_mode = ansi_ins_mode;
	tt->t_highlight_mode = ansi_hl_mode;
	tt->t_inslines = ansi_insert_lines;
	tt->t_dellines = ansi_delete_lines;
	tt->t_blanks = ansi_blanks;
	tt->t_init = ansi_init_trm;
	tt->t_cleanup = ansi_cleanup;
	tt->t_wipe_line = ansi_wipe_line;
	tt->t_wipe_screen = ansi_wipe_screen;
	tt->t_topos = ansi_topos_ext;
	tt->t_reset = ansi_reset;
	tt->t_delete_chars = NULL;
	tt->t_write_chars = ansi_write_chars;
	tt->t_window = ansi_window;
	tt->t_change_width = ansi_change_width;
	tt->t_update_line = ansi_update_line;
	tt->t_flash = ansi_feep;
	tt->t_io_printf = t_printf;
#ifdef unix
	tt->k_input_event = ansi_read;
#endif
	tt->t_display_activity = ansi_display_activity;
	return 1;
	}

static void t_printf( struct trmcontrol *tt, unsigned char *fmt, ... )
	{
	va_list argp;

	int size;
	unsigned char buf[128];

	va_start( argp, fmt );

	size = do_print ( fmt, &argp, buf, sizeof( buf )-1);
	buf[ size ] = 0;
	tt->t_io_print( tt, buf );
	}
