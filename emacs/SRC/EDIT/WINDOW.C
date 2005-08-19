/*
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and Nick Emery
 */
#include <emacs.h>

/*forward*/ int goto_window_at_x_y( void );
/*forward*/ int move_dot_to_x_y( void );
/*forward*/ void init_win( void );
/*forward*/ void set_win( struct window *w );
/*forward*/ struct window *split_win( struct window *w );
/*forward*/ struct window *split_largest_window( void );
/*forward*/ void del_win( struct window *w );
/*forward*/ void tie_win( struct window *w, struct emacs_buffer *b );
/*forward*/ int chg_w_height( struct window *w, int delta, int dir );
/*forward*/ static struct window *LR_uwin( void );
/*forward*/ int window_on( struct emacs_buffer *bf );
/*forward*/ static struct window *copy_window( struct window *w );
/*forward*/ struct windowring *copy_current_windows( void );
/*forward*/ void dest_window_ring( struct windowring *wr );
/*forward*/ void fetch_current_windows( struct expression *e );
/*forward*/ int check_current_windows( struct windowring *wr, struct variablename *v );
/*forward*/ void push_window_ring( void );
/*forward*/ void pop_window_ring( void );
/*forward*/ static void set_current_windows( struct windowring *wr );
/*forward*/ void fit_windows_to_screen_length( void );
/*forward*/ void wind_de_ref_buf( struct emacs_buffer *b );
/*forward*/ int full_upd( void );
/*forward*/ static void dump_mode( struct window *w, int line, int col );
/*forward*/ static void dump_str( unsigned char *s, int limit, int line, int column, int highlight );
/*forward*/ static int dump_win( struct window *w, int line, int col, int can_move );
/*forward*/ int scan_bf( unsigned char c, int n, int k );
/*forward*/ void do_dsp(int saveminibuf_body);
/*forward*/ int dump_bfl( int line_start_n, int sline, int scolumn, struct window *w, int initial_column );

#define	dsputc( c, a ) \
		do { \
		columns_left--; \
		if( columns_left >= 0 ) \
			{ \
			text_cursor[0] = (unsigned char)(c); \
			text_cursor++; \
			attr_cursor[0] = (unsigned char)(a); \
			attr_cursor++; \
			} \
		break; \
		} while( must_be_zero != 0 )

GLOBAL SAVRES struct queue saved_windows;
GLOBAL SAVRES struct windowring *pushed_windows = 0;
GLOBAL SAVRES struct window *windows;
GLOBAL SAVRES struct window *wn_cur;
GLOBAL SAVRES int G_saveminibuf_body;
GLOBAL SAVRES int in_minibuf_body;
GLOBAL SAVRES int default_wrap_lines;
GLOBAL SAVRES int scroll_step;
GLOBAL SAVRES int quick_rd;
GLOBAL SAVRES int ctl_arrow;
GLOBAL SAVRES unsigned char global_mode_string[MODESTRING_SIZE];
GLOBAL SAVRES int pop_up_windows = 1;
GLOBAL SAVRES int split_height_threshhold = 20;

static int line_wrapped;
static GLOBAL SAVRES int mouse_x;	/* The X screen coordinate of the mouse */
static GLOBAL SAVRES int mouse_y;	/* The Y screen coordinate of the mouse */
static GLOBAL SAVRES struct window *mouse_win;
					/* The window corresponding to (mouse_x,mouse_y) */
static GLOBAL SAVRES int mouse_dot;	/* The character position corresponding to (mouse_x,mouse_y) */

GLOBAL SAVRES struct marker *oneL_start; /* starting character position of the line
				 * containing dot -- used when doing the one
				 * line redisplay optimization. */
static int oneL_valid;		/* True iff OneLStart points at something valid */
static int oneL_line;		/* the display line which contains dot */
static int MB_line;			/* The line on which the minibuf starts */
static int use_time;		/* A counter used to set the time of last use
				 * of a window: for selecting the LRU window */

/* Move dot to the buffer and character corresponding to some absolute X
 * and Y coordinate. */
static int mouse_hit;

#define MOUSE_ON_MODELINE 1
#define MOUSE_ON_VERTICAL 2

int goto_window_at_x_y( void )
	{
	mouse_x = getnum (u_str("X coordinate: "));
	if( ! err )
		mouse_y = getnum (u_str("Y coordinate: "));
	if( ! err )
		{
		mouse_hit = 0;
		mouse_win = 0;
		cant_1line_opt = 1;;
		do_dsp( 1 );
		if( mouse_win == 0 )
			mouse_hit = -1;
		else
			{
			set_win( mouse_win );
			/*
			 *	Do not set the dot if the mouse points to the
			 *	mode line or vertical divider.
			 */
			if( mouse_hit == 0 )
				set_dot( mouse_dot );
			}

		release_expr( ml_value );
		ml_value->exp_type = ISINTEGER;
		ml_value->exp_int = mouse_hit;
		}
	return 0;
	}

/* Move dot to the buffer and character corresponding to some absolute X
 * and Y coordinate. */
int move_dot_to_x_y( void )
	{
	mouse_x = getnum (u_str("X coordinate: "));
	if( ! err )
		mouse_y = getnum (u_str("Y coordinate: "));
	if( ! err )
		{
		mouse_hit = 0;
		mouse_win = 0;
		cant_1line_opt = 1;;
		do_dsp( 1 );
		if( mouse_win == 0 || mouse_hit != 0 )
			error( u_str("The mouse is not pointing at a part of a buffer"));
		else
			{
			set_win( mouse_win );
			set_dot( mouse_dot );
			}
		}
	return 0;
	}

/* initialize the window system */
void init_win( void )
	{
	struct window *w;

	windows = w = malloc_struct( window );

	set_dot( 1 );
	w->w_height = 6;	/* use a height that will split */
	w->w_width = tt->t_width;
	w->w_next = 0;
	w->w_prev = 0;
	w->w_left = 0;
	w->w_right = 0;
	w->w_dot = new_mark();
	set_mark( w->w_dot, bf_cur, 1, 0 );
	w->w_start = new_mark();
	set_mark( w->w_start, bf_cur, 1, 0 );
	w->w_force = 0;
	w->w_horizontal_scroll = 1;
	w->w_buf = bf_cur;
	wn_cur = w;
	term_width = tt->t_width;
	set_win( split_win( w ) );
	tie_win( wn_cur, minibuf );
	change_window_size( 1 - wn_cur->w_height);
	set_win( w );
	oneL_start = new_mark();
	oneL_valid = 0;

	queue_init( &saved_windows );

	fit_windows_to_screen_length();
	}

void restore_win(void)
	{
	fit_windows_to_screen_length();
	}

/* set the current window */
void set_win( struct window *w )
	{
	if( w == 0 )
		return;

	w->w_lastuse = use_time++;
	set_bfp( w->w_buf );
	wn_cur = w;
	bf_cur = 0;
	cant_1win_opt = 1;
	set_bfp( w->w_buf );
	}

struct window *split_win( struct window *w )
	{
	struct window *n;
	struct emacs_buffer *old;

	old = bf_cur;
	if( w->w_height <= 4 )
		{
		error( u_str("You cannot have windows smaller than two lines high.") );
		return w;
		}
	/*
	 *	Find the right most window from here
	 */
	while( w->w_right != 0 )
		w = w->w_right;

	n = malloc_struct( window );
	n->w_prev = w;
	n->w_force = 0;
	n->w_left = 0;
	n->w_right = 0;
	n->w_width = tt->t_width;
	n->w_next = w->w_next;
	w->w_next = n;
	if( n->w_next != 0 )
		n->w_next->w_prev = n;
	n->w_height = w->w_height / 2;
	n->w_dot = new_mark();
	n->w_lastuse = 0;
	n->w_buf = w->w_buf;
	n->w_start = new_mark();
	n->w_horizontal_scroll = w->w_horizontal_scroll;
	set_mark( n->w_dot, n->w_buf,
		(( bf_cur == w->w_buf ) ?  dot : to_mark( w->w_dot )), 0 );
	set_mark( n->w_start, n->w_buf, to_mark( w->w_start), 0 );
	while( w != 0 )
		{
		w->w_height = w->w_height - n->w_height;
		w = w->w_left;
		}
	set_bfp( old );
	cant_1win_opt = 1;
	return n;
	}

struct window *split_win_vert( struct window *w )
	{
	struct window *n;
	struct emacs_buffer *old;

	old = bf_cur;

	if( w->w_width <= 3 )
		{
		error( u_str("You cannot have windows smaller than 1 column wide.") );
		return w;
		}

	n = malloc_struct( window );
	n->w_force = 0;
	if( w->w_right != 0 )
		w->w_right->w_left = n;
	n->w_left = w;
	n->w_right = w->w_right;
	w->w_right = n;

	n->w_prev = w;
	n->w_next = w->w_next;
	w->w_next = n;
	if( n->w_next != 0 )
		n->w_next->w_prev = n;
	n->w_height = w->w_height;
	n->w_width = w->w_width / 2;
	w->w_width = w->w_width - n->w_width - 1;
	n->w_dot = new_mark();
	n->w_lastuse = 0;
	n->w_buf = w->w_buf;
	n->w_start = new_mark();
	n->w_horizontal_scroll = w->w_horizontal_scroll;
	set_mark( n->w_dot, n->w_buf,
		(( bf_cur == w->w_buf ) ?  dot : to_mark( w->w_dot )), 0 );
	set_mark( n->w_start, n->w_buf, to_mark( w->w_start), 0 );
	set_bfp( old );
	cant_1win_opt = 1;

	return n;
	}

/* split the largest window, and return a pointer to it */
struct window *split_largest_window( void )
	{
	struct window *w;
	struct window *bestw = NULL;
	int besth;
	besth = -1;
	w = windows;
	while( w != 0 )
		{
		if( w->w_height > besth )
			{
			besth = w->w_height;
			bestw = w;
			}
		w = w->w_next;
		}
	return split_win( bestw );
	}

/* Delete the indicated window */
void del_win( struct window *w )
	{
	struct window *w2;

	if( w->w_next == 0 )	/* Can't delete the last window -- it's the minibuf */
		return;

	if( w == mouse_win )
		mouse_win = 0;

	/* last user window? */
	if( w->w_next->w_next == 0 && w->w_prev == 0 )
		{
		/* default the buffer in this window */
		if( n_buffers > 1 )
			set_bfn( buf_names[1] );
		else
			set_bfn( u_str("main") );

		if( w->w_dot != 0 )
			{
			dest_mark( w->w_dot);
			w->w_dot = 0;
			}
		if( w->w_start != 0 )
			{
			dest_mark( w->w_start);
			w->w_start = 0;
			}
		w->w_dot = new_mark();
		w->w_start = new_mark();
		tie_win( w, bf_cur );
		w->w_horizontal_scroll = 1;
		set_win( w );
		return;
		}

	/*
	 *	See if this is a vertically split window.
	 *	In which case adjust the width. Otherwise the
	 *	heights need adjusting.
	 */
	if( w->w_left != 0 || w->w_right != 0 )
		{
		/*
		 *	Combine vertical split windows
		 */
		w->w_height = 0;	/* Allow us to fall into the horizontal code */
		if( w->w_right != 0 )
			{
			w2 = w->w_right;
			w2->w_left = w->w_left;
			w2->w_width = w2->w_width + w->w_width + 1;
			if( w->w_left )
				{
				w2 = w->w_left;
				w2->w_right = w->w_right;
				}
			}
		else if( w->w_left != 0 )
			{
			w2 = w->w_left;
			w2->w_right = w->w_right;
			w2->w_width = w2->w_width + w->w_width + 1;
			if( w->w_right )
				{
				w2 = w->w_right;
				w2->w_left = w->w_left;
				}
			}
		}

	/*
	 *	Combine horizontally split windows and unlink a vertically
	 *	split window.
	 */
	if( w->w_prev != 0 )
		{
		w2 = w->w_prev;
		w2->w_next = w->w_next;
		while( w2 != 0 )
			{
			w2->w_height = w2->w_height + w->w_height;
			w2 = w2->w_left;
			}
		}
	else
		{
		if( w->w_next == 0 )
			return;

		w2 = windows = w->w_next;
		while( w2 != 0 )
			{
			w2->w_height = w2->w_height + w->w_height;
			w2 = w2->w_right;
			}
		}
	if( w->w_next != 0 )
		w->w_next->w_prev = w->w_prev;

	/*
	 *	If deleting the current window we need to pick a new
	 *	current window.
	 */
	if( w == wn_cur )
		if( w->w_buf != 0 )

			if( w->w_prev != 0 )
				set_win( w->w_prev );
			else
				set_win( windows );
		else
			wn_cur = w->w_next;

	/*
	 *	Free up the windows resources.
	 */
	dest_mark( w->w_dot );
	dest_mark( w->w_start );
	free( w );

	/*
	 *	Cannot assume that window struct is unchanged
	 */
	cant_1win_opt = 1;
	}

/* tie a window to a buffer */
void tie_win( struct window *w, struct emacs_buffer *b )
	{
	int newdot;

	if( b == 0 || w == 0 || w->w_buf == b )
		return;
	w->w_buf = b;
	w->w_force = 0;
	w->w_lastuse = use_time++;
	newdot = (( b == bf_cur ) ?  dot : b->b_ephemeral_dot);
	set_mark( w->w_dot, b, newdot, 0 );
	set_mark( w->w_start, b, 1, 0 );
	}

/*
 * Change the height of the pointed to window by delta; returns true iff
 * the change succeeds. Chains forward if dir gtr 0, backward if dir lss 0 in
 * attempting to find a suitable window.
 */
int chg_w_height( struct window *w, int delta, int dir )
	{
	if( w == 0 ) return 0;

	/*
	 *	Start from the left most window
	 */
	while( w->w_left != 0) w = w->w_left;

	/*
	 *	Adjust this window
	 */
	while( w != 0 )
		if( w->w_height + delta >= (( w->w_buf == minibuf ) ?  1 : 2)
		&& (dir == 0 || w->w_buf != minibuf) )
			{
			cant_1win_opt = 1;

			/*
			 *	Adjust all the heights across the screen
			 */
			do
				{
				w->w_height = w->w_height + delta;
				w = w->w_right;
				}
			while( w != 0 );

			return 1;
			}
		else
			switch( dir )
			{
			case 0:	return 0;
			case 1:
				while( w != 0 && w->w_right != 0 )
					w = w->w_right;
				if( w != 0 )
					w = w->w_next;
				break;
			case -1:
				w = w->w_prev;
				while( w != 0 && w->w_left != 0 )
					w = w->w_left;
				break;
			}

	return 0;
	}

/*
 * Change the width of the pointed to window by delta; returns true iff
 * the change succeeds. Chains forward if dir gtr 0, backward if dir lss 0 in
 * attempting to find a suitable window.
 */
int chg_w_width( struct window *w, int delta, int dir )
	{
	/*
	 *	Adjust this window
	 */
	while( w != 0 )
		if( w->w_width + delta >= 1 )
			{
			cant_1win_opt = 1;

			w->w_width = w->w_width + delta;

			return 1;
			}
		else
			switch( dir )
			{
			case 0:		return 0;
			case 1:		w = w->w_right; break;
			case -1:	w = w->w_left; break;
			}

	return 0;
	}

/* find the least recently used window; split if only one window */
static struct window *LR_uwin( void )
	{
	struct window *w;
	struct window *bestw;
	int youngest;
	int largest_height;

	bestw = 0;
	youngest = INT_MAX;
	largest_height = 0;
	w = windows;
	while( w->w_next != 0 )
		{
		if(	(( w->w_buf == bf_cur ) ?
				bf_s1 + bf_s2
			:
				w->w_buf->b_size1 + w->w_buf->b_size2)
			== 0 )
			return w;
		if( w->w_lastuse < youngest && w != wn_cur )
			{
			bestw = w;
			youngest = w->w_lastuse;
			}
		if( w->w_height > largest_height )
			largest_height = w->w_height;
		w = w->w_next;
		}
	if( bestw == 0 || largest_height >= split_height_threshhold )
		bestw = split_largest_window();
	return bestw;
	}

/*
 * make sure that the current window is on the given buffer, either
 * by picking the window that already contains it, the LRU window,
 * or some brand new window
 */
int window_on( struct emacs_buffer *bf )
	{
	int on_screen;
	struct window *w;

	on_screen = 0;
	w = wn_cur;
	if( w->w_buf != bf )
		{
		w = windows;
		while( w != 0 )
			{
			if( w->w_buf == bf )
				{
				on_screen = 1;
				break;
				}
			w = w->w_next;
			}
		}
	if( w == 0 )
		w = pop_up_windows || wn_cur->w_next == NULL ? LR_uwin() : wn_cur;
	tie_win( w, bf );
	set_win( w );

	return on_screen;
	}

/*
 *	Window ring save and restore mechanism
 */

/* Copy a window and return a pointer to the new copy */

static struct window *copy_window( struct window *w )
	{
	struct window *new_w;

	new_w = malloc_struct( window );

	new_w->w_next = 0;
	new_w->w_prev = 0;
	new_w->w_left = 0;
	new_w->w_right = 0;
	new_w->w_buf = w->w_buf;
	new_w->w_height = w->w_height;
	new_w->w_width = w->w_width;
	new_w->w_lastuse = w->w_lastuse;
	new_w->w_force = w->w_force;
	new_w->w_horizontal_scroll = w->w_horizontal_scroll;
	if( w->w_dot != 0 )
		copy_mark( new_w->w_dot = new_mark(), w->w_dot );
	else
		new_w->w_dot = 0;
	if( w->w_start != 0 )
		copy_mark( new_w->w_start = new_mark(), w->w_start );
	else
		new_w->w_start = 0;

	return new_w;
	}

struct windowring *copy_current_windows( void )
	{
	struct windowring *wr;
	struct window *w;
	struct window *wcopy;
	struct window *wnext;

	wr = malloc_struct( windowring );
	wr->wr_queue.next = NULL;
	wr->wr_queue.prev = NULL;
	wr->wr_windows = wcopy = copy_window( windows );
	wr->wr_bf_cur = new_mark();
	set_mark( wr->wr_bf_cur, bf_cur, 1, 0 );
	wr->wr_wn_cur = wcopy;
	wr->wr_buffervisible = wn_cur->w_buf == bf_cur;
	wr->wr_ref_count = 1;
	wr->wr_pushed = 0;

	/* insert in queue */
	queue_insert( &saved_windows, &wr->wr_queue );

	w = windows->w_next;
	while( w != 0 )
		{
		wnext = copy_window( w );
		wcopy->w_next = wnext;
		wnext->w_prev = wcopy;
		if( wn_cur == w )
			wr->wr_wn_cur = wnext;

		wcopy = wnext;
		w = w->w_next;
		}

	/*
	 *	fix up the left and right pointers
	 */
	w = windows;
	wcopy = wr->wr_windows;
	while( w != 0 )
		{
		if( w->w_right != 0 ) wcopy->w_right = wcopy->w_next;
		if( w->w_left != 0 ) wcopy->w_left = wcopy->w_prev;

		w = w->w_next;
		wcopy = wcopy->w_next;
		}

	return wr;
	}

void dest_window_ring( struct windowring *wr )
	{
	struct window *w;

	wr->wr_ref_count = wr->wr_ref_count - 1;
	if( wr->wr_ref_count > 0 )
		return;

	/* remove from queue */
	queue_remove( &wr->wr_queue );

	dest_mark( wr->wr_bf_cur );

	w = wr->wr_windows;
	while( w != 0 )
		{

		struct window *wnext;

		wnext = w->w_next;

		dest_mark( w->w_dot );
		dest_mark( w->w_start );
		free( w );
		w = wnext;
		}

	free( wr );
	}

void fetch_current_windows( struct expression *e )
	{
	e->exp_v.v_windows = copy_current_windows();
	e->exp_int = 0;
	e->exp_release = 1;
	}

int check_current_windows( struct windowring *wr, struct variablename *PNOTUSED(v) )
	{
	set_current_windows( wr );
	return 1;
	}

void push_window_ring( void )
	{
	struct windowring *wr;

	wr = copy_current_windows();

	/* push ring */
	wr->wr_pushed = pushed_windows;
	pushed_windows = wr;
	}

void pop_window_ring( void )
	{
	struct windowring *wr;

	/* install pushed windows */
	wr = pushed_windows;
	if( wr == 0 )
		invoke_debug();
	pushed_windows = wr->wr_pushed;

	set_current_windows( wr );
	dest_window_ring( wr );
	}

static void set_current_windows( struct windowring *wr )
	{
	struct window *w;
	struct window *wcopy;
	struct window *wnext;

	/* destroy current windows */
	w = windows;
	while( w != 0 )
		{
		struct window *wnext;

		wnext = w->w_next;

		dest_mark( w->w_dot );
		dest_mark( w->w_start );
		free( w );
		w = wnext;
		}

	/* install a copy of the windows from the ring */
	wn_cur = windows = wcopy = copy_window( wr->wr_windows );
	w = wr->wr_windows->w_next;
	while( w != 0 )
		{
		wnext = copy_window( w );
		wcopy->w_next = wnext;
		wnext->w_prev = wcopy;
		if( wr->wr_wn_cur == w )
			wn_cur = wnext;
		wcopy = wnext;
		w = w->w_next;
		}

	/*
	 *	fix up the left and right pointers
	 */
	wcopy = windows;
	w = wr->wr_windows;
	while( w != 0 )
		{
		if( w->w_right != 0 ) wcopy->w_right = wcopy->w_next;
		if( w->w_left != 0 ) wcopy->w_left = wcopy->w_prev;

		w = w->w_next;
		wcopy = wcopy->w_next;
		}

	if( wr->wr_bf_cur->m_buf != 0 )
		set_bfp( wr->wr_bf_cur->m_buf );

	/* check_ for deleted buffers in the windows */
	w = windows;
	while( w->w_next != 0 )
		{

		struct window *wn;
		wn = w->w_next;

		if( w->w_buf == 0 )
			del_win( w );

		w = wn;
		}

	if( bf_cur == 0 )
		set_bfp( windows->w_buf );

	if( wr->wr_buffervisible != 0 )
		window_on( bf_cur );

	fit_windows_to_screen_length();
	}

/*
 * if the length of the screen has change adjust the windows so that
 * they fill the screen
 */
void fit_windows_to_screen_length( void )
	{
	int old_len;
	int old_wid = 0;
	struct window *w;

	old_len = 0;
	w = windows;
	while( w != 0 )
		{
		if( w->w_right == 0 )
			old_len = old_len + w->w_height;
		old_wid = w->w_width;	/* taken from minibuffer window */

		w = w->w_next;
		}

	/*
	 *	Go around the windows adjusting them to the new size
	 */

	/*
	 *	If the page is longer add the new length to the windows above the
	 *	minibuffer.
	 */
	if( old_len < tt->t_length )
		{

		int increase;

		increase = tt->t_length - old_len;

		w = windows;
		while( w->w_next != 0 )
			w = w->w_next;
		w = w->w_prev;	/* the window above the minibuffer */

		do
			{
			w->w_height = w->w_height + increase;
			w = w->w_left;
			}
		while( w != 0 );

		screen_garbaged = 1;
		}

	/*
	 *	If the page is shorter reduce the height of the last windows until they
	 *	fit. If this is not possible delete the last windows and try again.
	 */
	if( old_len > tt->t_length )
		{

		int decrease;

		decrease = old_len - tt->t_length;

		w = windows;
		while( w->w_next != 0 )
			w = w->w_next;
		w = w->w_prev;	/* the window above the minibuffer */

		while( w->w_height - decrease < 2 )
			{
			w = w->w_prev;
			del_win( w->w_next );
			}

		do
			{
			w->w_height = w->w_height - decrease;
			w = w->w_left;
			}
		while( w != 0 );

		screen_garbaged = 1;
		}

	/*
	 *	If the width has increased then add the width to the right most window if each
	 *	window row.
	 */
	if( old_wid < tt->t_width )
		{

		int increase;

		increase = tt->t_width - old_wid;

		w = windows;
		while( w != 0 )
			{
			if( w->w_right == 0 )
				w->w_width = w->w_width + increase;
			w = w->w_next;
			}

		screen_garbaged = 1;
		}

	/*
	 *	If the width of the screen has decreased then reduce the size of the
	 *	right most window until it fits. If the right most window is too small
	 *	delete it and try again
	 */
	if( old_wid > tt->t_width )
		{

		int decrease;

		decrease = old_wid - tt->t_width;

		w = windows;
		while( w != 0 )
			{
			if( w->w_right == 0 )
				{
				while( w->w_width - decrease <= 0 )
					{
					w = w->w_left;
					del_win( w->w_right );
					}
				w->w_width = w->w_width - decrease;
				}
			w = w->w_next;
			}

		screen_garbaged = 1;
		}
	}

void wind_de_ref_buf( struct emacs_buffer *b )
	{
	struct queue *q;
	struct windowring *wr;
	struct window *w;

	q = saved_windows.next;
	while( (void *)q != (void *)&saved_windows )
		{
		wr = (struct windowring *)q;

		w = wr->wr_windows;
		while( w->w_next != 0 )
			{
			if( w->w_buf == b )
				{
				dest_mark( w->w_dot );
				w->w_dot = 0;
				dest_mark( w->w_start );
				w->w_start = 0;
				w->w_buf = 0;
				}
			w = w->w_next;
			}

		q = q->next;
		}
	}

/* full screen update -- called when absolutely nothing is known or
 * many things have been fiddled with */
int full_upd( void )
	{
	struct emacs_buffer *keep_bf;
	struct emacs_buffer *hit_bf;
	struct window *w;
	int sline;
	int scol;
	int hits;
	int slow;
	int ldot = 0;
	int dumpstate;

	keep_bf = bf_cur;
	hit_bf = wn_cur->w_buf;
	w = windows;
	sline = 1;
	scol = 1;
	hits = 0;
	slow = 0;

	while( w != 0 )
		{
		int pos;

		set_bfp( w->w_buf );
		if( bf_cur == hit_bf )
			hits++;
		slow = slow || w->w_force != 0;

		dumpstate = 0;
		if( w != wn_cur )
			{
			ldot = dot;
			set_dot( to_mark( w->w_dot ) );
			}

		pos = to_mark( w->w_start );
		if( pos < first_character )
			set_mark( w->w_start, w->w_buf, first_character, 0 );
		else if( pos > num_characters + 1 )
			set_mark( w->w_start, w->w_buf, num_characters + 1, 0 );

		pos = to_mark( w->w_dot );
		if( pos < first_character )
			set_mark( w->w_dot, w->w_buf, (ldot = set_dot( first_character )), 0 );
		else if( pos > num_characters + 1 )
			set_mark( w->w_dot, w->w_buf, (ldot = set_dot( num_characters + 1 )), 0 );

		while( dumpstate >= 0
		&& dump_win( w, sline, scol, dumpstate == 0) != 0 )
			{
			slow++;
			if( w->w_force != 0 )
				{
				set_dot
				(
				(( dumpstate != 0 ) ?  to_mark( w->w_start)
				: scan_bf( '\n', to_mark( w->w_start), w->w_height / 2) )
				);
				if( w != wn_cur )
					set_mark( w->w_dot, w->w_buf, dot, 0 );
				if( dumpstate != 0 )
					w->w_force = 0;
				dumpstate++;
				}
			else
				{
				int old;
				int next = 0;
				switch( dumpstate )
				{
				case 0:
					{
					dumpstate++;
					if( scroll_step > 0
					&& scroll_step < (w->w_height / 2) )
						{
						old = to_mark( w->w_start );
						next = scan_bf
							(
							'\n', old,
							(( old > dot ) ?
								-scroll_step - 1
							 :
								scroll_step) );
						if( dot < next )
							{
							next = scan_bf
								(
								'\n',
								dot,
								-(w->w_height / 2)
								);
							}
						}
					else
						next = scan_bf( '\n', dot, -(w->w_height/ 2) );
					}
					break;
				case 1:
					{
					next = scan_bf( '\n', dot, -(w->w_height/ 2) );
					dumpstate++;
					}
					break;
				case 2:
					{
					next = scan_bf( '\n', (old = to_mark( w->w_start )), 1 );
					if( old < next && next <= dot )
						{
						dumpstate++;
						next = to_mark( w->w_start ) + 50;
						if( dumpstate > 10 )
							dumpstate = -1;
						}
					}
					break;
				case -1:
					break;
				default:
					{
					dumpstate++;
					next = to_mark( w->w_start ) + 50;
					if( dumpstate > 10 )
						dumpstate = -1;
					}
				}
				if( next <= dot )
					set_mark( w->w_start, w->w_buf, next, 0 );
				else
					dumpstate = -1;
				}
			}
		if( w != wn_cur )
			set_dot( ldot );
		w->w_force = 0;

		if( redo_modes != 0 && w->w_next != 0 )
			dump_mode( w, sline + w->w_height - 1, scol );
		if( w->w_right != 0 )
			scol += 1 + w->w_width;
		else
			{
			scol = 1;
			sline += w->w_height;
			}

		w = w->w_next;
		}

	cant_ever_opt = hits > 1 && quick_rd == 0;
	set_bfp( keep_bf );
	return slow;
	}

/*
 * Dump the mode line for window w on line n -- assumes the current buffer
 * is the one associated with window w
 */
static void dump_mode( struct window *w, int line, int col )
	{
	unsigned char buf[MSCREENWIDTH+2];
	unsigned char *buf_end;
	unsigned char tbuf[20];
	unsigned char *p;
	unsigned char *s;
	unsigned char *str;
	unsigned char c;
	int width;
	int recurse_depth;

#define mode_c( c ) do {if( p > buf_end ) goto dump_mode_loop_done; *p++ = c;} while( must_be_zero != 0 )

	p = buf;
	buf_end = &buf[ w->w_width ];

	s = bf_mode.md_modeformat;
	recurse_depth = recursive_edit_depth - minibuf_depth;

	if( mouse_y == line
	&& mouse_x >= col
	&& mouse_x <= col + w->w_width )
		{
		mouse_hit |= MOUSE_ON_MODELINE;
		if( mouse_x == col + w->w_width
		&& w->w_right != 0 )
			mouse_hit |= MOUSE_ON_VERTICAL;

		mouse_win = w;
		mouse_dot = dot;
		}

	while( (c = *s++) != 0 )
		if( c == '%' )
			{
			str = 0;
			width = 0;
			while( isdigit( c = *s++ ) )
				width = width * 10 + (c - '0');
			switch( c )
			{
			case 0:
				goto dump_mode_loop_done;
			case 'a':
				str = (( bf_mode.md_abbrevon ) ?  u_str(",Abbrev" ) : u_str(""));
				break;
			case 'b':
				str = bf_cur->b_buf_name;
				break;
			case 'c':
				str = (( checkpoint_frequency > 0
					&& bf_cur->b_checkpointed != -1 ) ?  u_str(",checkpoint") : u_str(""));
				break;
			case 'f':
				str = (( bf_cur->b_fname != 0 ) ?
					(bf_cur->b_fname[0] != '_' ?
						bf_cur->b_fname : &bf_cur->b_fname[1])
					:
						u_str("[No File]"));
				break;
			case 'j':
				str =	(( bf_cur->b_journalling
					&& journalling_frequency != 0 ) ?  u_str(",Journal") : u_str(""));
				break;
			case 'm':
				str = bf_mode.md_modestring;
				break;
			case 'M':
				str = &global_mode_string[0];
				break;
			case 'r':
				str = (( bf_mode.md_replace ) ?  u_str(",Overstrike") : u_str(""));
				break;
			case 'R':
				str = (( bf_mode.md_readonly ) ?  u_str(",Readonly") : u_str(""));
				break;
			case '*':
				str = (( bf_modified != 0 ) ?  u_str("*") : u_str(""));
				break;
			case 'p':
				{
				int tl;
				int d;
				tl = bf_s1 + bf_s2;
				d = (( w == wn_cur ) ?  dot : to_mark( w->w_dot ));
				if( d <= 1 )
					str = u_str("Top");
				else
					if( d > tl )
						str = u_str("Bottom");
					else
						{
						sprintfl( tbuf, sizeof( tbuf ), u_str("%2d%%"),
							(int)((d - 1) * 100l / tl) );
						str = tbuf;
						}
				}
				break;
			case 'h':
				{

				int left;

				left = w->w_horizontal_scroll;

				if( left > 1 )
					{
					sprintfl( &tbuf[0], sizeof( tbuf ), u_str("<%d<"), left );
					str = &tbuf[0];
					}
				else
					str = u_str("");
				}
				break;
			case '[':
				if( recurse_depth > 4 )
					{
					sprintfl( &tbuf[0], sizeof( tbuf ), u_str("[%d["), recurse_depth );
					str = &tbuf[0];
					}
				else
					str = (u_str("[[[[") + 4) - recurse_depth;
				break;
			case ']':
				if( recurse_depth > 4 )
					{
					sprintfl( &tbuf[0], sizeof( tbuf ), u_str("]%d]"), recurse_depth );
					str = &tbuf[0];
					}
				else
					str = (u_str("]]]]") + 4) - recurse_depth;
				break;
			default:
				mode_c( c );
			}
			if( str != 0 )
				{
				int i;

				while( str[0] != 0 )
					{
					mode_c( *str++ );
					width = width - 1;
					if( width == 0 )
						break;
					}
				for( i=1; i<=width; i += 1 )
					mode_c(' ');
				}
			}
		else
			mode_c( c );

dump_mode_loop_done:
	*p++ = 0;
	dump_str( &buf[0], w->w_width, line, col, LINE_ATTR_MODELINE );
#undef mode_c
	}

/*
 *	dump the indicated string
 *	starting at line `line` and column `column` dump `limit` characters
 */
static void dump_str
	(
	unsigned char *s,
	int limit,
	int line, int column,
	int highlight
	)
	{
	int col;
	int setcurs;
	int new_col;
	int i;

	col = 0;
	setcurs = s == minibuf_body && in_minibuf_body;
	if( activity_indicator && term_ansi && s == minibuf_body )
		{
		setpos( line, 1 );
		dsputc( activity_character, 0 );
		dsputc( ' ', 0 );
		col = 2;
		}
	else
		setpos( line, column ); /* setup things to update line line */

	for( i=1; i<=limit; i += 1 )
		{
		int c;
		c = *s++;
		if( c == 0 )
			break;
		if( c == '\t' )	/* expand tab to spaces */
			{
			new_col = min( (col / bf_mode.md_tabsize + 1) *
					bf_mode.md_tabsize, limit );

			while( col < new_col )
				{
				dsputc( ' ', highlight );
				col++;
				}
			}
		else if( c < ' ' )
			{
			if( ctl_arrow )
				{
				if( term_deccrt
				&& (c == ctl( 'L')
				|| c == ctl( 'M')
				|| c == ctl( 'K')
				|| c == ctl( '[')
				|| c == ctl( 'I')) )
					{
					col++;

					if( col <= limit )
						switch( c )
						{
						case ctl('L'):
							dsputc( ctl('c'), highlight ); break;
						case ctl('M'):
							dsputc( ctl('d'), highlight ); break;
						case ctl('K'):
							dsputc( ctl('i'), highlight ); break;
						case ctl('I'):
							dsputc( ctl('b'), highlight ); break;
						case ctl('['):
							dsputc( ctl('{'), highlight ); break;
						}
					}
				else
					{
					col = col + 2;
					if( col <= limit )
						{
						dsputc('^', highlight);
						/* need suport of C1 control codes */
						dsputc( (( c < ' ' ) ?  (c & 0x1f) + 0x40 : '?' ), highlight );
						}
					}
				}
			else
				{
				col = col + 4;
				if( col <= limit )
					{
					dsputc( '\\', highlight );
					dsputc( ((c>>6)&7) + '0', highlight);
					dsputc( ((c>>3)&7) + '0', highlight);
					dsputc( (c&7) + '0', highlight);
					}
				}
			}
		else
			{
			col++;
			if( col <= limit )
				dsputc( c, highlight );
			}
		}
	if( col > limit )
		{
		setpos( line, limit );
		dsputc( '$', highlight );
		}
	if( setcurs )
		{
		curs_y = line;
		curs_x = (( col > limit ) ?  limit : col + 1);
		}
	}

/*
 * dump the text from the indicated window on the indicated line;
 * the current buffer must be the one tied to this window
 */
static int dump_win
	(
	struct window *w,
	int line, int col,
	int can_move
	)
	{
	int last_col;
	int left;
	int n;
	int misseddot;
	int next;
	int i;

	misseddot = 1;

	/* left needs to reflect the fact that the minibuf does not have a modeline */
	left = w->w_next != 0 ? w->w_height - 1 : w->w_height;

	n = to_mark( w->w_start );

	if( can_move
	&&	((n > first_character && char_at( n - 1 ) != '\n')
		|| n < first_character) )
		{
		n = n < first_character ? first_character : scan_bf( '\n', n, -1 );
		set_mark( w->w_start, w->w_buf, n, 0 );
		}

	/* if this window is the minibuffer */
	if( w->w_next == 0 )
		{
		MB_line = line;
		if( G_saveminibuf_body || minibuf_body == 0 )
			return 0;
		clearline( line );
		if( n == 1 )
			dump_str( minibuf_body, tt->t_width, line, 1, 0 );
		next = dump_bfl( n, line, tt->t_width - columns_left, w, tt->t_width - columns_left );
		line++;
		left--;

		if( next < 0 )
			{
			if( w == wn_cur )
				{
				set_mark( oneL_start, bf_cur, line_wrapped ? 1 : n, 0 );
				oneL_valid = !line_wrapped;
				oneL_line = line - 1;
				}
			n = -next;
			misseddot = 0;
			}
		else
			n = next;
		}

	if( w->w_right != 0 )
		last_col = col + w->w_width;
	else
		last_col = 0;

	for( i=1; i<=left; i += 1 )
		{
		if( w->w_left == 0 )
			clearline( line );
		else
			setpos( line, col );
		next = dump_bfl( n, line, col, w, 1 );
		if( last_col > 0 )
			{
			setpos( line, last_col );
			dsputc( ' ', LINE_ATTR_MODELINE );
			}
		line++;
		if( next < 0 )
			{
			if( w == wn_cur )
				{
				set_mark( oneL_start, bf_cur, line_wrapped ? 1 : n, 0 );
				oneL_valid = !line_wrapped;
				oneL_line = line - 1;
				}
			next = -next;
			misseddot = 0;
			}
		n = next;
		}

	return misseddot;
	}

/* Scan the current buffer for the k'th occurrence of character c,
 * starting at position n; k may be negative[ Returns ]the position
 * of the character following the one found */
int scan_bf
	(
	unsigned char c,
	int n,
	int k
	)
	{
	if( k > 0 )
		while( k != 0 )
			{
			n--;
			do
				{
				n++;
				if( n > num_characters )
					return n;
				}
			while( char_at( n ) != c );

			k--;
			if( k != 0 )
				n++;
			}
	else
		while( k != 0 )
			{
			do
				{
				n--;
				if( n < first_character )
					return first_character;
				}
			while( char_at( n ) != c );

			k++;
			}

	return n + 1;
	}

/*
 * do a screen update, taking possible shortcuts into account
 */
void do_dsp(int saveminibuf_body)
	{
	int slow_update;
	int doneminibuf_body;

	slow_update = 0;
	doneminibuf_body = 0;

	if( ! term_is_terminal )
		return;

	G_saveminibuf_body = saveminibuf_body && wn_cur->w_next == 0;

	if( screen_garbaged || err
	|| (last_redisplay_paused && !in_minibuf_body) )
		{
		cant_1win_opt = 1;
		last_redisplay_paused = 0;
		}
	if( cant_1win_opt )
		cant_1line_opt = redo_modes = 1;

	if( !cant_1line_opt
	&& oneL_valid && ! oneL_start->m_modified
	&& oneL_start->m_buf == bf_cur )
		{
		int n;
		int col;

		n = to_mark( oneL_start );
		if( wn_cur->w_left || wn_cur->w_right )
			{
			struct window *w;

			col = 1;
			w = wn_cur->w_left;
			while( w )
				{
				col += w->w_width + 1;
				w = w->w_left;
				}

			/*
			 *	Get old line contents and clear out the
			 *	bit that will get overwritten
			 */
			copyline( oneL_line );
			setpos( oneL_line, col );
			memset( text_cursor, ' ', wn_cur->w_width );
			memset( attr_cursor, 0, wn_cur->w_width );
			}
		else
			{
			col = 1;
			clearline( oneL_line );
			}

		if( minibuf_body != 0 && wn_cur->w_next == 0 )
			{
			if( n == 1 )
				dump_str( minibuf_body, tt->t_width, oneL_line, 1, 0 );
			doneminibuf_body = 1;
			}

		if( dump_bfl( n, oneL_line, col, wn_cur, 1 ) < 0 && line_wrapped == 0 )
			goto update;	/* we made it ! */
		else
			if( !bf_mode.md_wrap_lines )
				slow_update = -1;
		}

	doneminibuf_body = 1;
	slow_update = 1;
	oneL_valid = 0;
	if( full_upd() != 0 )
		slow_update = 1;
update:
	if( minibuf_body != 0
	&& ( ! G_saveminibuf_body || minibuf_body[0] != 0) )
		{
		if( doneminibuf_body == 0 )
			{
			clearline( MB_line );
			dump_str( minibuf_body, tt->t_width, MB_line, 1, 0 );
			}
		if( reset_minibuf != 0 )
			{
			minibuf_body = reset_minibuf;
			if( reset_minibuf[0] == 0 )
				reset_minibuf = 0;
			}
		else
			minibuf_body = minibuf_body[0] != 0 ? u_str("") : 0;
		}

	update_screen( slow_update );
	if( err )
		{
		ding();
		err = 0;
		}

	cant_1line_opt = 0;
	cant_1win_opt = cant_ever_opt;

	tt->t_io_flush( tt );
	}

/*
 * Dump one line from the current buffer starting at character n onto
 * line sline; setting curs_x and curs_y if appropriate
 */
static unsigned char fake_c1_chars[32] = 
	{
	'*', '#', 't', 'f', 'c', 'l', 'o', '+',
	'n', 'v', '+', '+', '+', '+', '+', '~',
	'~', '-', '_', '_', '|', '|', '-', '-',
	'|', '<', '>', 'n', '=', 'L', '.', ' '
	};

int dump_bfl
		(
		int line_start_n,
		int sline,
		int scolumn,
		struct window *w,
		int initial_column
		)
	{
	int n;
	int col;
	int lim;
	unsigned char c;
	int mark;
	int term_width;
	int first_column;
	int highlight;
	int dot_column;
	int r_start; int r_end;		/* limits of region */
	int rr_start = 0; int rr_end = 0;	/* limits of rendition region */
	struct flags_bits
		{
		int display_non_printing : 1;
		int missed_dot : 1;
		int looking_for_mouse : 1;
		int dec_crt : 1;
		int syntax_colouring : 1;
		}
			flags;
	unsigned char *syn_p1 = NULL, *syn_p2 = NULL;
	struct rendition_region *rr;

	flags.dec_crt = term_deccrt;
	flags.display_non_printing = bf_mode.md_displaynonprinting && flags.dec_crt;
	flags.syntax_colouring = bf_mode.md_syntax_colouring;

	first_column = w->w_horizontal_scroll;
	term_width = w->w_width;
	n = line_start_n;
	col = initial_column;
	lim = bf_s1 + bf_s2 - bf_mode.md_tailclip;
	flags.missed_dot = 1;
	flags.looking_for_mouse =	mouse_y == sline
				&&	mouse_win == 0
				&&	mouse_x >= scolumn;
	if( flags.looking_for_mouse
	&& mouse_x >= w->w_width + scolumn )
		{
		if( mouse_x == w->w_width + scolumn
		&& w->w_right != 0 )
			{
			mouse_hit = MOUSE_ON_VERTICAL;
			mouse_win = w;
			mouse_dot = dot;
			}

		flags.looking_for_mouse = 0;
		}

	/*
	 *	update the syntax data if required
	 */
	if( flags.syntax_colouring )
		{
		int required = n + tt->t_width + 1;
		if( required > bf_s1+bf_s2 )
			required = bf_s1+bf_s2;

		bf_mode.md_syntax_array = 1;	/* force syntax array */
		syntax_fill_in_array( required );
		syn_p1 = bf_cur->b_syntax.syntax_base;
		syn_p2 = syn_p1 + bf_gap;
		}

	/*
	 *	find the value of the mark for this buffer
	 */
	mark = dot;

	/*
	 *	If allowed to highlight region and mark is set then set mark to
	 *	the value of the mark. Note that if r_start and r_end are the
	 *	same the region will not be highlight.
	 */
	if( bf_mode.md_highlight
	&& bf_cur->b_mark != 0 )
		mark = to_mark( bf_cur->b_mark );

	/*
	 *	Setup the region highlight stuff
	 */
	r_start = min( dot, mark );
	r_end	= max( dot, mark );
	highlight =	r_start <= n && r_end > n ? LINE_M_ATTR_HIGHLIGHT : 0;

	/* turn markers into ints */
	rr = bf_cur->b_rendition_regions;
	while( rr != NULL )
		{
		rr_start = to_mark( rr->rr_start_mark );
		rr_end = to_mark( rr->rr_end_mark );

		/* if we start before this rr then use this rr as the current */
		if( n < rr_start )
			break;
		/* if we are in the middle of this rr set the colour */
		if( n < rr_end )
			{
			highlight |= rr->rr_colour;
			flags.syntax_colouring = 0;
			break;
			}

		rr = rr->rr_next;
		}
	if( rr == NULL )
		{
		rr_start = -1;
		rr_end = -1;
		}

	dot_column = 0;

	for(;;)
		{
#define	_if_wraped( column ) \
		if( column > (term_width + first_column) ) \
			{ \
			if( bf_mode.md_wrap_lines ) \
				{ \
				n = n - 2; \
				goto dump_bfl_loop; \
				} \
			} \
		else \
			if( column >= first_column )

		if( n == dot )
			{
			if( w == wn_cur && ! G_saveminibuf_body )
				{
				curs_x = tt->t_width - columns_left + 1;
				curs_y = sline;
				dot_col = col;
				col_valid=4;

				if( curs_x > tt->t_width )
					curs_x = tt->t_width;
				else if( curs_x < 1 )
					curs_x = 1;
				}

			flags.missed_dot = 0;
			dot_column = col;
			}

		/*
		 *	If we are at the start of the region turn on
		 *	highlighting
		 */
		if( n == r_start )
			highlight |= LINE_M_ATTR_HIGHLIGHT;

		/*
		 *	if we are at the end of the region turn off
		 *	highlighting
		 */
		if( n == r_end )
			highlight &= ~LINE_M_ATTR_HIGHLIGHT;

		/* see if at the end of the rendition region */
		if( n == rr_end )
			{
			/* turn off the rr colour */
			highlight &= LINE_M_ATTR_HIGHLIGHT;
			/* turn syntax colouring back on */
			flags.syntax_colouring = bf_mode.md_syntax_colouring;
			/* move to the next rr */
			rr = rr->rr_next;

			/* setup the start and end */
			if( rr == NULL )
				{
				rr_start = -1;
				rr_end = -1;
				}
			else
				{
				rr_start = to_mark( rr->rr_start_mark );
				rr_end = to_mark( rr->rr_end_mark );
				}
			}

		/*
		 * check for the start  of an rr - must be
		 * done after the rr_end check
		 */
		if( n == rr_start )
			{
			/* turn off any syntax colours */
			highlight &= LINE_M_ATTR_HIGHLIGHT;
			/* turn on the range colour */
			highlight |= rr->rr_colour;
			/* turn off syntax coloring */
			flags.syntax_colouring = 0;
			}
		if( n > lim )
			{
			if( bf_mode.md_display_eof
			&& n - 1 == lim
			&& flags.dec_crt )
				dsputc( ctl('@'), highlight );

			n++;
			c = ctl('J');
			goto dump_bfl_loop;
			}

		if( flags.looking_for_mouse
		&& mouse_x < (scolumn + col - first_column) )
			{
			mouse_win = w;
			mouse_dot = n-1;
			flags.looking_for_mouse = 0;
			}

		c = char_at( n );
		if( flags.syntax_colouring  )
			_if_wraped( col )
				highlight = (highlight&LINE_M_ATTR_HIGHLIGHT) | syntax_at( n );
		n++;
		switch( c )
		{
		case ctl('J'):	/* Newline */
			if( flags.display_non_printing )
				if( col >= first_column - 1 )
					dsputc( ctl('H'), highlight );

			goto dump_bfl_loop;

		case ctl('I'):	/* TAB */
			{
			int old_col;

			old_col = col;
			col = ((col - 1) / bf_mode.md_tabsize + 1) *
						bf_mode.md_tabsize + 1;

			if( flags.display_non_printing )
				{
				int i;
				_if_wraped( old_col + 1 )
					dsputc( ctl('B'), highlight );

				for( i=old_col + 2; i<=col; i += 1 )
					_if_wraped( i )
						dsputc( ctl('^'), highlight );
				}
			else
				{
				int i;

				for( i=old_col + 1; i<=col; i += 1 )
					_if_wraped( i )
						dsputc( ' ', highlight );
				}
			break;
			}

		default:
			if( (c >= ' ' && c <= '~')
			|| (c >= 128+32 && c <= 254) )	/* printing chars */
				{
				col++;

				_if_wraped( col )
					dsputc( c, highlight );
				}
		 	else
			/* none printing control characters */
			{
			if( bf_mode.md_display_c1
			&& c >= 128 && c <= 128+32 )
				{
				col++;
				c &= 0x1f;

				_if_wraped( col )
					dsputc( flags.dec_crt ? c : fake_c1_chars[c], highlight );
				}
			else
			if( ctl_arrow != 0 && (c & 0x80) == 0 )
				{
				if( flags.dec_crt
				&&
					(c == ctl('L')
					|| c == ctl('M')
					|| c == ctl('K')
					|| c == ctl('[')
					|| c == ctl('I')) )
					{
					col++;

					_if_wraped( col )
						switch( c )
						{
						case ctl('L'):	dsputc( ctl('c'), highlight ); break;
						case ctl('M'):	dsputc( ctl('d'), highlight ); break;
						case ctl('K'):	dsputc( ctl('i'), highlight ); break;
						case ctl('I'):	dsputc( ctl('b'), highlight ); break;
						case ctl('['):	dsputc( ctl('{'), highlight ); break;
						}
					}
				else
					{
					col = col + 2;
					_if_wraped( col )
						{
						dsputc('^', highlight);
						dsputc((( c < ' ' ) ? ( c & 0x1f) + 0x40 : '?'), highlight );
						}
					}
				}
			else
				{
				col = col + 4;
				_if_wraped( col )
					{
					dsputc( '\\', highlight);
					dsputc( '0' + ((c>>6)&7), highlight);
					dsputc( '0' + ((c>>3)&7), highlight);
					dsputc( '0' + (c&7), highlight);
					}
				}
			}
		}
#undef	_if_wraped
		}
dump_bfl_loop:

	if( flags.looking_for_mouse )
		{
		mouse_win = w;
		mouse_dot = n-1;
		}

	if( sline > 0 )
		{
		/*
		 *	Only put a '$' or '\' at the end of the line if :-
		 *
		 *	The line is longer then  "w_width"
		 *	or the cursor is on column  "w_width" + 1
		*/
		line_wrapped = 0;
		if(	col - first_column > term_width		/* a long line */
		||	dot_column - first_column >= term_width /* cursor off screen */
		||	c != ctl('J')				/* wrap lines */ )
			{
			setpos( sline, scolumn + w->w_width - 1 );
			highlight = r_start <= n && r_end > n ? LINE_M_ATTR_HIGHLIGHT : 0;
			if( flags.dec_crt )
				dsputc( (bf_mode.md_wrap_lines ? ctl('Z') : ctl('`')), highlight );
			else
				dsputc( (bf_mode.md_wrap_lines ? '\\' : '$'), highlight );

			if( bf_mode.md_wrap_lines )
				line_wrapped = 1;
			}
		}

	if( ! flags.missed_dot )
		n = -n;

	return n;
	}
