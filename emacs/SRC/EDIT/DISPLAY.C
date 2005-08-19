/*
 * 	Copyright(c ) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery
 */
/****************************************************************

			 /-------------\
			/		\
		       /		 \
		      /			  \
		      |	  XXXX	   XXXX	  |
		      |	  XXXX	   XXXX	  |
		      |	  XXX	    XXX	  |
		      \		X	  /
		       --\     XXX     /--
			| |    XXX    | |
			| |	      | |
			| I I I I I I I |
			|  I I I I I I	|
			 \	       /
			  --	     --
			    \-------/
		    XXX			   XXX
		   XXXXX		  XXXXX
		   XXXXXXXXX	     XXXXXXXXXX
			  XXXXX	  XXXXX
			     XXXXXXX
			  XXXXX	  XXXXX
		   XXXXXXXXX	     XXXXXXXXXX
		   XXXXX		  XXXXX
		    XXX			   XXX

			    **************
			    *   BEWARE 	 *
			    **************

			All ye who enter here:
		    Most of the code in this module
		       is twisted beyond belief.

			   Tread carefully.

		    If you think you understand it,
			     You Do not,
			    So Look Again.

 ****************************************************************/

#include <emacs.h>

#ifdef vms
#include <ttdef.h>
#endif

/*forward*/ int check_protocol_mode( int value, struct variablename *v );
/*forward*/ void fetch_protocol_mode( struct expression *e );
/*forward*/ int check_screen_width( int width, struct variablename *v );
/*forward*/ void fetch_screen_width( struct expression *e );
/*forward*/ int check_screen_length( int length, struct variablename *v );
/*forward*/ void fetch_screen_length( struct expression *e );
/*forward*/ int check_display_bool( int value, struct variablename *v );
/*forward*/ int check_graphic_rendition( unsigned char *value, struct variablename *v );
/*forward*/ struct emacs_line * newline( void );
/*forward*/ static void release_line( struct emacs_line *p );
/*forward*/ void setpos(int row, int col);
/*forward*/ void clearline(int row);
/*forward*/ void copyline(int row);
/*forward*/ static void hashline( struct emacs_line *p );
/*forward*/ static void calc_matrix( void );
/*forward*/ static void calc_insert_delete(int i, int j, int inserts_desired);
/*forward*/ void generic_updateline( struct emacs_line *old_line, struct emacs_line *new_line, int ln );
/*forward*/ void update_screen(int slow_update);
/*forward*/ void ding( void );
/*forward*/ int sit_for( void );
/*forward*/ int sit_for_inner(int num_chars);
/*forward*/ void init_display( void );
#if DBG_CALC_M_PRINT
void print_matrix( void );
#endif

GLOBAL SAVRES struct emacs_line *phys_screen[MSCREENLENGTH + 1];
				/* the current( physical ) screen */
GLOBAL SAVRES struct emacs_line *desired_screen[MSCREENLENGTH + 1];
				/* the desired( virtual ) screen */

struct trmcontrol *active_terminal;
GLOBAL SAVRES int protocol_mode = {1};
int last_redisplay_paused;
int screen_garbaged;	/* set to 1 iff screen content is uncertain. */
int RDdebug;		/* line redraw debug selectone */
int IDdebug;		/* line insertion/deletion debug */
int curs_x;		/* X and Y coordinates of the cursor */
int curs_y;		/* between updates. */
int current_line;	/* current line for writing to the virtual screen. */
int columns_left;	/* number of columns left on the current line
			 * of the virtual screen. */
unsigned char *text_cursor;
unsigned char *attr_cursor;
				/* pointer into a line object, indicates where
				 * to put the next character */
GLOBAL SAVRES struct emacs_line *free_lines;	/* free space list */
GLOBAL SAVRES struct emacs_line *NL_scratch;


static int window_size;		/* the number of lines on which line ID
				operations should be done */
#if !MEMMAP
static int check_for_input;	/* -ve iff UpdateLine should bother checking for input */
#endif

/* the following macros are used to access terminal specific routines.
 * Really, no one outside of display.b32 should be using them, except for
 * the init_ialize/cleanup routines */


#define HASH_RESERVE -12345


int check_protocol_mode
	(
	int value,
	struct variablename *v
	)
	{
	if( !check_display_bool( value, v ) )
		return 0;

	protocol_mode = value;
	set_protocol( protocol_mode );

	return 0;
	}

void fetch_protocol_mode
	(
	struct expression *e
	)
	{
#ifdef vms
	protocol_mode = (tt->t_cur_attributes.v_char_1 & (TT$M_TTSYNC | TT$M_HOSTSYNC)) != 0;
#else
	protocol_mode = 0;
#endif
	*e->exp_v.v_value = protocol_mode;
	}

int check_screen_width
	(
	int width,
	struct variablename *PNOTUSED(v)
	)
	{
	if( width < 32 || width > MSCREENWIDTH )
		error( u_str("terminal-width must be > 32 and <= %d"), MSCREENWIDTH );
	else
		{
		term_width = width;
		tt->t_width = width;
		if( tt->t_change_width != 0 )
			tt->t_change_width( tt, width );
		set_protocol( protocol_mode );
		fit_windows_to_screen_length();
		screen_garbaged = 1;
		}
	return 0;
	}

void fetch_screen_width
	(
	struct expression *e
	)
	{
	*e->exp_v.v_value = tt->t_width;
	}

int check_cursor_mode
	(
	int type,
	struct variablename *v
	)
	{
	if( !check_display_bool( type, v ) )
		return 0;

	term_cursor_mode = type;
	tt->t_change_attributes( tt );

	return 0;
	}

int check_mouse_mode
	(
	int type,
	struct variablename *v
	)
	{
	if( !check_pos( type, v ) )
		return 0;

	mouse_enable = type;
	tt->t_change_attributes( tt );

	return 0;
	}

int check_screen_length
	(
	int length,
	struct variablename *PNOTUSED(v)
	)
	{
	if( length < 3 || length > MSCREENLENGTH )
		error( u_str("terminal-length must be > 3 and <= %d"), MSCREENLENGTH );
	else
		{
		term_length = length;
		tt->t_length = length;
		set_protocol( protocol_mode );
		fit_windows_to_screen_length();
		screen_garbaged = 1;
		}
	return 0;
	}

void fetch_screen_length
	(
	struct expression *e
	)
	{
	*e->exp_v.v_value = tt->t_length;
	}

int check_display_bool
	(
	int value,
	struct variablename *v
	)
	{
	int rv;

	rv = value == 1 || value == 0;

	if( ! rv )
		error( u_str("%s may only be set to a boolean value, 1 or 0"),
				v->v_name );

	cant_1line_opt = 1;
	cant_1win_opt = 1;

	return rv;
	}

int check_graphic_rendition
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	unsigned char *p;
	int c;

	p = value;
	if( p == 0 )
		return 0;

	while( *p != '\0' )
		if( !((c = *p++) == ';' || (c >= '0' && c <= '9') ))
			{
			error( u_str("graphic-rendition must be 0-9 and ;") );
			return 0;
			}

	cant_1line_opt = 1;
	cant_1win_opt = 1;
	redo_modes = 1;

	return 1;
	}

/* 'newline' returns a pointer to a new line object, either from the
 *  free list or from the general unix pool
 */
struct emacs_line * newline( void )
	{
	struct emacs_line *p;

	p = free_lines;

	if( p != 0 )
		{
		free_lines = p->line_next;
		if( p->line_hash != HASH_RESERVE )
			{
			invoke_debug();		/* bug in display code */
			free_lines = 0;

			return newline();
			}
		}
	else
		{
		static int leakage;
		p = malloc_struct( emacs_line );
		if( p == 0 )
			invoke_debug();
		leakage++;
		if( leakage > 150 )
			tt->t_io_printf( tt, u_str("*****Display core leakage!") );
		}
	p->line_length = 0;
	p->line_hash = 0;

	return p;
	}

/* 'release_line' returns a line object to the free list */
static void release_line
	(
	struct emacs_line *p
	)
	{
	if( p != 0 )
		{
		if( p->line_hash == HASH_RESERVE )
			{
			tt->t_io_printf( tt, u_str("Bogus re-release!") );
			tt->t_io_flush( tt );
			/* abort(); */
			invoke_debug();
			return;
			}
		p->line_next = free_lines;
		p->line_hash = HASH_RESERVE;
		free_lines = p;
		}
	}

/* 'setpos' positions the cursor at position(row,col ) in the virtual screen */
void setpos(int row, int col)
	{
	struct emacs_line *p;
	int n;

	if( current_line >= 0
	&&  (p = desired_screen[ current_line ], p->line_length)
		<= (n = tt->t_width - columns_left) )
		p->line_length = (( columns_left > 0 ) ?  n : tt->t_width);
	if( desired_screen[ row ] == 0 )
		desired_screen[ row ] = newline();
	p = desired_screen[ row ];
	p->line_hash = 0;
	while( p->line_length + 1 < col )
		{
		p->line_body[ p->line_length ] = ' ';
		p->line_attr[ p->line_length ] = 0;
		p->line_length++;
		}
	p->line_length = col - 1;
	current_line = row;
	columns_left = tt->t_width + 1 - col;
	text_cursor = &p->line_body[ col - 1 ];
	attr_cursor = &p->line_attr[ col - 1 ];
	}
/*
 * 'clearline' positions the cursor at the beginning of the
 *  indicated line and clears the line( in the image)
 */
void clearline(int row)
	{
	struct emacs_line *p;

	setpos( row, 1 );
	p = desired_screen[row];
	p->line_length = 0;
	}
void copyline(int row)
	{
	struct emacs_line *old_line;
	struct emacs_line *new_line;

	setpos( row, 1 );
	old_line = phys_screen[ row ];
	if( old_line == 0 ) return;

	new_line = desired_screen[ row ];
	*new_line = *old_line;
	new_line->line_next= NULL;
	}

/*
 * 'hashline' computes a hash value for a line, unless the hash value
 * is already known. This hash code has a few important properties:
 *	- it is independant of the number of leading and trailing spaces
 *	- it will never be zero
 *
 * As a side effect, an estimate of the cost of redrawing the line is
 * calculated
 */
static void hashline
	(
	struct emacs_line *p
	)
	{
	unsigned char *t;
	unsigned char *a;
	unsigned char *l;
	int h;

	if( p == 0 || p->line_hash != 0 )
		{
		if( p != 0 && p->line_hash == HASH_RESERVE )
			tt->t_io_printf( tt, u_str("****Free line in screen") );
		return;
		}
	a = p->line_attr;

	if( (a[0] & LINE_ATTR_MODELINE) != 0 )
		{
		p->line_drawcost = p->line_length;
		p->line_hash = -200;
		return;
		}
	h = 0;
	t = p->line_body;
	l = &p->line_body[ p->line_length ];

	while( --l > t && l[0] == ' ')
		;
	while( t <= l && t[0] == ' ' && a[0] == 0 )
		{ t++; a++; }

	p->line_drawcost = (l - t) + 1;

	while( t <= l )
		h = (h << 5) + h + *t++ + *a++;

	p->line_hash = h != HASH_RESERVE && h != 0 ? h : 1;
	}

/*	1   2   3   4   ....	Each Mij represents the minimum cost of
      +---+---+---+---+-----	rearranging the first i lines to map onto
    1 |   |   |   |   |		the first j lines (the j direction
      +---+---+---+---+-----	represents the desired contents of a line,
    2 |   |  \| ^ |   |		i the current contents).  The algorithm
      +---+---\-|-+---+-----	used is a dynamic programming one, where
    3 |   | <-+Mij|   |		M[i,j] = min( M[i-1,j],
      +---+---+---+---+-----		      M[i,j-1]+redraw cost for j,2
    4 |   |   |   |   |			      M[i-1,j-1]+the cost of
      +---+---+---+---+-----			converting line i to line j);
    . |   |   |   |   |		Line i can be converted to line j by either
    .				just drawing j, or if they match, by moving
    .				line i to line j (with insert/delete line)
*/

static struct emacs_msquare msquare[MSCREENLENGTH + 1][MSCREENLENGTH + 1];

static void calc_matrix( void )
	{
	struct emacs_line *line;
	struct emacs_msquare *p;
	int movecost;
	int cost;
	int re_draw_cost;
	int idcost;
	int leftcost;
	int fidcost;/*double; */
	int i;

	cost = 0;
	movecost = 0;
	for( i=0; i<=tt->t_length; i++ )
		{
		p = &msquare[i][0];
/*		 M[i, i,  cost ] = 0; */
		p[i].msq_cost = 0;
		msquare[0][i].msq_cost = cost + movecost;
/*		 M[i, 0,  msq_cost ] = movecost; */
		p[0].msq_cost = movecost;
		msquare[0][i].msq_fromi = 0;
/*		 M[0, i,  msq_fromj ] = M[i, i,  msq_fromj ] = i - 1; */
		msquare[0][i].msq_fromj = p[i].msq_fromj = i - 1;
/*		 M[i, 0,  msq_fromi ] = M[i, i,  msq_fromi ] = i - 1; */
		p[0].msq_fromi = p[i].msq_fromi = i - 1;
/*		 M[i, 0,  msq_fromj ] = 0; */
		p[0].msq_fromj = 0;
		movecost = movecost +
			tt->t_il_mf *  (tt->t_length - i) + tt->t_il_ov;
		if( (line = desired_screen[ i + 1 ]) != 0 )
			cost = cost + line->line_drawcost;
		}
	fidcost = tt->t_il_mf * (window_size + 1) + tt->t_il_ov;
	for( i=1; i<=window_size; i++ )
		{
		int j;

		p = &msquare[i][0];
		fidcost = fidcost - tt->t_il_mf;
		idcost = fidcost;
		for( j=1; j<=window_size; j++ )
			{
			p++;
/*			 p = &M[i, j]; */
			cost = (line = desired_screen[j]) != 0 ? line->line_drawcost : 0;
			re_draw_cost = cost;
			if( phys_screen[i] != 0 && line != 0
			&& phys_screen[i]->line_hash  == line->line_hash )
				cost = 0;
/*	idcost = tt[ t_il_mf ]* ( WindowSize - i + 1 ) + tt[ t_il_ov ];
 *	movecost = M[i - 1, j,  msq_cost ]+ ( j eql WindowSize ? 0	: idcost ); */
			movecost = p[ -MSCREENLENGTH - 1].msq_cost
				+ (j == window_size ? 0 : idcost);
			p[0].msq_fromi = i - 1;		/* now using movecost for */
			p[0].msq_fromj = j;		/* the minumum cost. */

/*			leftcost = M[i, j - 1,  msq_cost ] */
			if( (leftcost = p[-1].msq_cost +
				(i == window_size ? 0 : idcost) +
				 re_draw_cost)
			<
				movecost )
				{
				movecost = leftcost;
				p[0].msq_fromi = i;
				p[0].msq_fromj = j - 1;
				}
/*	cost += M[i - 1, j - 1,  msq_cost ]; */
			cost = cost + p[ -MSCREENLENGTH - 2].msq_cost;
			if( cost < movecost )
				{
				movecost = cost;
				p[0].msq_fromi = i - 1;
				p[0].msq_fromj = j - 1;
				}
			p[0].msq_cost = movecost;
			}
		}

#if DBG_CALC_M_PRINT
	if( dbg_flags&DBG_CALC_M_PRINT )
		print_matrix();
#endif
	}

#if MEMMAP
#define update_line( old_line, new_line, ln ) \
	tt->t_update_line( tt, old_line, new_line, ln )
#else
void update_line
	(
	struct emacs_line *old_line,
	struct emacs_line *new_line,
	int ln
	)
	{
	tt->t_update_line( tt, old_line, new_line, ln );
	check_for_input--;
	if( check_for_input < 0
	&& input_pending == 0 )
		{
		tt->t_io_flush( tt );
		check_for_input = 1;	/* baud_rate / 2400; */
		}
	}
#endif
/*
 * calculate and perform the optimal sequence of insertions/deletions
 * given the matrix M from routine calc_matrix
 */
static void calc_insert_delete(int i, int j, int inserts_desired)
	{
	int ni;
	int nj;
	struct emacs_msquare *p;

	p = &msquare[i][j];
	if( i > 0 || j > 0 )
		{
		ni = p->msq_fromi;
		nj = p->msq_fromj;
		if( ni == i )
			{
			calc_insert_delete( ni, nj, i != window_size ? inserts_desired + 1 : 0 );
			inserts_desired = 0;
			if( input_pending )
				{
				if( phys_screen[j] != desired_screen[j] )
					release_line( phys_screen[j] );
				phys_screen[j] = 0;
				release_line( desired_screen[j] );
				desired_screen[j] = 0;
				last_redisplay_paused = 1;
				}
			else
				{
				update_line( 0, desired_screen[j], j );
				if( phys_screen[j] != desired_screen[j] )
					release_line( phys_screen[j] );
				phys_screen[j] = desired_screen[j];
				desired_screen[j] = 0;
				}
			}
		else
		if( nj == j )
			{
			if( j != window_size )
				{
				int nni;
				int dlc;

				dlc = 1;
				while( ni != 0 )
					{
					p = &msquare[ ni][ nj ];
					nni = p[0].msq_fromi;
					if( p[0].msq_fromj == nj )
						{
						dlc++;
						ni = nni;
						}
					else
						break;
					}
				tt->t_topos( tt, i - dlc + 1, 1 );
				tt->t_dellines( tt, dlc );
				}
			calc_insert_delete( ni, nj, 0 );
			}
		else
			{
			struct emacs_line *old_line;
			int done_early;
			done_early = 0;
			old_line = phys_screen[i];
			if( old_line == desired_screen[i] )
				desired_screen[i] = 0;
			phys_screen[i] = 0;

			/* The following hack and all following lines
			 * involving the variable 'DoneEarly' cause the
			 * bottom line of the screen to be redisplayed
			 * before any others if it has changed and it
			 * would be redrawn in-place. This is purely
			 * for Emacs, people using this package for
			 * other things might want to lobotomize this
			 * section. */
			if( i == tt->t_length && j == tt->t_length
			&& old_line != phys_screen[j] )
				{
				done_early = 1;
				update_line( old_line, desired_screen[j], j );
				}
			calc_insert_delete( ni, nj, 0 );
			if( input_pending && ! done_early )
				{
				if( phys_screen[j] != old_line )
					release_line( phys_screen[j] );
				if( desired_screen[j] != old_line
				&& desired_screen[j] != phys_screen[j] )
					release_line( desired_screen[j] );
				phys_screen[j] = old_line;
				desired_screen[j] = 0;
				last_redisplay_paused = 1;
				}
			else
				{
				if( ! done_early )
					update_line( old_line, desired_screen[j], j );
				if( phys_screen[j] != desired_screen[j] )
					release_line( phys_screen[j] );
				if( old_line != desired_screen[j]
				&& old_line != phys_screen[j] )
					release_line( old_line );
				phys_screen[j] = desired_screen[j];
				desired_screen[j] = 0;
				}
			}
		}
	if( inserts_desired != 0 )
		{
		tt->t_topos( tt, j + 1, 1 );
		tt->t_inslines( tt, inserts_desired );
		}
	}

#if 0
/*
 * modify current screen line 'old' to match desired line 'new',
 * the old line is at position ln. Each line
 * is scanned and partitioned into 4 regions:
 *
 *	     <osp><----m1-----><-od--><----m2----->
 *   old:    "     Twas brillig and the slithy toves"
 *   new:    "        Twas brillig where a slithy toves"
 *	     <-nsp--><----m1-----><-nd--><----m2----->
 *
 *	nsp, osp	- number of leading spaces on each line
 *	m1		- length of a leading matching sequence
 *	m2		- length of a trailing matching sequence
 *	nd, od		- length of the differing sequences
 */
void generic_updateline
	(
	struct emacs_line *old,
	struct emacs_line *new,
	int ln
	)
	{
	unsigned char *op;
	unsigned char *np;
	unsigned char *ol;
	unsigned char *nl;
	int osp;
	int nsp;
	int m1;
	int m2;
	int od;
	int nd;
	int oldHL;
	int newHL;
	int t;

	if( old == new )
		return;
	if( old != 0 )
		{
		op = &old->line_body[0];
		ol = &old->line_body[ old->line_length ];
		oldHL = old->line_attr[0];
		}
	else
		{
		op = u_str( "" );
		ol = op;
		oldHL = 0;
		}
	if( new != 0 )
		{
		np = &new->line_body[0];
		nl = &new->line_body[ new->line_length ];
		newHL = new->line_highlighted;
		}
	else
		{
		np = u_str( "" );
		nl = np;
		newHL = 0;
		}
	osp = nsp = m1 = m2 = od = od = 0;

/* calculate the magic parameters */
	if( newHL == oldHL )
		{
		while( (ol = &ol[-1] , ol[0]) == ' ' && ol >= op );
		while( (nl = &nl[-1] , ol[0]) == ' ' && nl >= np );
		while( op[0] == ' ' && op <= ol )
			{
			op = &op[1];
			osp++;
			}
		while( np[0] == ' ' && np <= nl )
			{
			np = &np[1];
			nsp++;
			}
		while( op[0] == np[0] && op <= ol && np <= nl )
			{
			op = &op[1];
			np = &np[1];
			m1++;
			}
		while( ol[0] == nl[0] && op <= ol && np <= nl )
			{
			op = &op[-1];
			np = &np[-1];
			m2++;
			}
		}
	else
		{
		ol = &ol[-1];
		nl = &nl[-1];
		osp = 0;
		while( np[0] == ' ' && np < nl )
			{
			np = &np[1];
			nsp++;
			}
		}
	od = &ol[0] - &op[0] + 1;
	nd = &nl[0] - &np[0] + 1;

/* forget matches which would be expensive to capitalize on */
	if( m1 != 0 || m2 != 0 )
		{
		int c0; int c1; int c2; int c3;

		c0 = nsp + m1 + m2;
		if( (c1 = nsp - osp) != 0 )
			c1 = (( c1 < 0 ) ?  -c1 * tt->t_ic_mf : c1) +
					tt->t_ic_ov;
		if( (c3 = nd - od) != 0 )
			c3 = (( c3 < 0 ) ?  -c3 * tt->t_ic_mf : 0) +
					 tt->t_ic_ov;
		if( (c2 = (nsp + nd) - (osp + od)) != 0 )
			c2 = (( c2 < 0 ) ?  -c2 * tt->t_ic_mf : 0) +
					tt->t_ic_ov;
		c3 = c3 + c1;
		c1 = c1 + m2;
		c2 = c2 + m1;
		if( m2 != 0
		&&	(c0 < c2
			&& c0 < c3 || c1 < c2
			&& c1 < c3) )
			{
			nd = nd + m2;
			od = od + m2;
			ol = ol + m2;
			nl = nl + m2;
			m2 = 0;
			}
		if( m1 != 0
		&&	(c0 < c1
			&& c0 < c3 || c2 < c1
			&& c2 < c3) )
			{
			nd = nd + m1;
			od = od + m1;
			np = np - m1;
			op = op - m1;
			m1 = 0;
			}
		}
	if( RDdebug
	&& (m1 || m2 || nd || od) != 0 )
		{
		tt->t_io_printf( tt, u_str("%2d nsp=%2d osp=%2d m1=%2d nd=%2d od=%2d m2=%2d" ),
				ln, nsp, osp, m1, nd, od, m2);
		}
	tt->t_highlight_mode( tt, newHL );
	if( newHL != oldHL )
		{
		tt->t_topos( tt, ln, 1 );
		tt->t_wipe_line( tt, 1 );
		}
	if( m1 == 0 )
		{
		if( m2 == 0 )
			{
			if( od == 0 && nd == 0 )
				return;
			tt->t_topos( tt, ln, (t = min( nsp, osp )) + 1 );
			tt->t_insert_mode( tt, 0 );
			if( nsp > osp )
				tt->t_blanks( tt, nsp - osp );
			tt->t_write_chars( tt, np, nl );
			if( nsp + nd < osp + od )
				tt->t_wipe_line( tt, 0 );
			}
		else
			{
			/* m1 eql 0 and m2 neq 0 and ( nd neq 0 or od neq 0 ) */
			t = (nsp + nd) - (osp + od);
			tt->t_topos( tt, ln, min( nsp, osp ) + 1 );
			if( nsp > osp )
				np = np - (nsp - osp);
			if( t >= 0 )
				{
				if( nl - t >= np )
					{
					tt->t_insert_mode( tt, 0 );
					tt->t_write_chars( tt, np, nl - t );
					}
				if( t > 0 )
					{
					tt->t_insert_mode( tt, 1 );
					tt->t_write_chars( tt, nl - t + 1, nl );
					}
				}
			else
				{
				tt->t_insert_mode( tt, 0 );
				tt->t_write_chars( tt, np, nl );
				tt->t_delete_chars( tt, -t );
				}
			}
		}
	else
		{			/* m1 neq 0 */

		int lsp;
		lsp = osp;
		if( nsp < osp )
			{
			tt->t_topos( tt, ln, 1 );
			tt->t_delete_chars( tt, osp - nsp );
			lsp = nsp;
			}
		if( m2 == 0 )
			{
			if( nd == 0 && od == 0 )
				{
				if( nsp > osp )
					{
					tt->t_topos( tt, ln, 1 );
					tt->t_insert_mode( tt, 1 );
					tt->t_blanks( tt, nsp - osp );
					}
				return;
				}
			tt->t_topos( ln, lsp + m1 + 1 );
			tt->t_insert_mode( 0 );
			tt->t_write_chars( np, nl );
			if( nd < od )
				tt->t_wipe_line( tt, 0 );
			if( nsp > osp )
				{
				tt->t_topos( tt, ln, 1 );
				tt->t_insert_mode( tt, 1 );
				tt->t_blanks( tt, nsp - osp );
				}
			}
		else
			{
			/* m1 neq 0 and m2 neq 0 and ( nd neq 0 or od neq 0 ) */
			tt->t_topos( tt, ln, lsp + m1 + 1 );
			t = nd - od;
			if( nd > 0 && od > 0 )
				{
				tt->t_insert_mode( tt, 0 );
				tt->t_write_chars( tt, np, np + min( nd, od ) - 1 );
				}
			if( nd < od )
				tt->t_delete_chars( tt, od - nd );
			else
				if( nd > od )
					{
					tt->t_insert_mode( tt, 1 );
					tt->t_write_chars( tt, np + od, nl );
					}
			if( nsp > osp )
				{
				tt->t_topos( tt, ln, 1 );
				tt->t_insert_mode( tt, 1 );
				tt->t_blanks( tt, nsp - osp );
				}
			}
		}
	}
#endif

void update_screen(int slow_update)
	{
	int n; int c;
	struct emacs_line *p;

	tt->t_update_begin( tt );

#if !MEMMAP
	check_for_input = 999;
#endif
	if( screen_garbaged )
		{
		tt->t_reset( tt );
		screen_garbaged = 0;
		for( n=0; n<=tt->t_length; n++ )
			{
			release_line( phys_screen[n] );
			phys_screen[n] = 0;
			}
		}
	tt->t_highlight_mode( tt, 0 );

	if( current_line >= 0
	&& (p = desired_screen[ current_line ], p->line_length) <=
			tt->t_width - columns_left )
		p->line_length =
			(( columns_left > 0 ) ?
				tt->t_width - columns_left
			:
				tt->t_width);
	current_line = -1;

	if( tt->t_il_ov == MISSINGFEATURE )
		slow_update = 0;

	if( slow_update )
		{
		for( n=1; n<=tt->t_length; n++ )
			{
			if( desired_screen[n] == 0 )
				desired_screen[n] = phys_screen[n];
			else
				hashline( desired_screen[n] );
			hashline( phys_screen[n] );
			}
		c = 0;
		n = tt->t_length;
		while( n >= 1 && c <= 2 )
			{
			if( phys_screen[n] != desired_screen[n]
			&& phys_screen[n] != 0
			&& desired_screen[n]->line_hash  !=
					phys_screen[n]->line_hash  )
				c++;
			n--;
			}
		if( c <= 2 )
			slow_update = 0;
		else
			{
			if( tt->t_window != 0 )
				{
				n = tt->t_length;
				while( n >= 1
				&&	(phys_screen[n] == desired_screen[n]
					|| (phys_screen[n] != 0
					&& desired_screen[n]->line_hash  ==
						phys_screen[n]->line_hash )) )
					n--;

				window_size = n;
				tt->t_window( tt, n );
				}
			else
				window_size = tt->t_length;

			calc_matrix();
#if !MEMMAP
			check_for_input = 1;	/* baud_rate / 2400; */
#endif
			calc_insert_delete( tt->t_length, tt->t_length, 0 );
			}
		}
	if( ! slow_update )	/* fast update */
		{
		for( n=1; n<=tt->t_length; n++ )
			if( desired_screen[n] != 0 )
				{
				update_line( phys_screen[n], desired_screen[n], n );
				if( phys_screen[n] != desired_screen[n] )
					release_line( phys_screen[n] );
				phys_screen[n] = desired_screen[n];
				desired_screen[n] = 0;
				}
		}
	tt->t_highlight_mode( tt, 0 );
	if( input_pending == 0 )
		tt->t_topos( tt, curs_y, curs_x );

	tt->t_update_end( tt );
	}

GLOBAL SAVRES int visible_bell;		/* If true and the terminal will support it
					 * then the screen will flash instead of
					 * feeping when an error occurs */
GLOBAL SAVRES int black_on_white;

/* DJH common routine for a feep */
void ding( void )
	{
	if( visible_bell && tt->t_flash != NULL )
		tt->t_flash( tt );
	else
		tt->t_beep( tt );
	}

/* DLK routine to make the cursor sit for n/10 secs */
int sit_for( void )
	{
	return sit_for_inner( getnum(u_str(": sit-for ")) );
	}
int sit_for_inner(int num_chars)
	{
	if( input_pending != 0 )
		return 0;

	if( term_is_terminal )
		{
		do_dsp( 1 );	/* Make the screen correct */
		tt->t_insert_mode( tt, 0 );
		}
	while( num_chars != 0 && input_pending == 0 )
		{
		wait_abit();

		num_chars--;
		}
	return 0;
	}
/* init_ialize the teminal package */
void init_display( void )
	{
	RDdebug = 0;		/* line redraw debug selectone */
	IDdebug = 0;		/* line insertion/deletion debug */
	curs_x = 1;		/* X and Y coordinates of the cursor */
	curs_y = 1;		/* between updates. */
	current_line = -1;	/* current line for writing to the virtual
				screen. */
	columns_left = -1;	/* number of columns left on the current line
				of the virtual screen. */
	}

#if DBG_CALC_M_PRINT
/* Debugging routines -- called from sdb only */

/* print out the insert/delete cost matrix */
void print_matrix( void )
	{
	int i,j;
	struct emacs_msquare *p;
	FILE *f;

	f = fopen("calc_m.lis","a" );
	if( f == NULL )
		return;

	for( i=0; i<=tt->t_length; i++ )
		{
		for( j=0; j<=tt->t_length; j++ )
			{
			p = &msquare[ i][ j ];
			fprintf( f, "%4d%c",
				p->msq_cost,
				(( p->msq_fromi < i && p->msq_fromj < j ) ?  92
				: (( p->msq_fromi < i ) ?  '^'
				: (( p->msq_fromj < j ) ?  '<'
				: ' '))));
			}
		fprintf( f, "\n" );
		}
	fprintf( f, "\n" );
	fclose( f );
	}
#endif

