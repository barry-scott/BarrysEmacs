/*	Copyright (c) 1982-1994
 *	Barry A. Scott and nick Emery */

/* window management commands */

#include <emacs.h>
#ifdef vms
#include <fab.h>
#endif

/*forward*/ int list_buffers( void );
/*forward*/ int delete_other_windows( void );
/*forward*/ int split_current_window( void );
/*forward*/ int kill_buffer_command( void );
/*forward*/ int switch_to_buffer( void );
/*forward*/ int pop_to_buffer( void );
/*forward*/ int temp_use_buffer( void );
/*forward*/ int erase_buffer( void );
/*forward*/ int use_old_buffer( void );
/*forward*/ static int str_table_match( unsigned char **table, unsigned char *string, int siz );
/*forward*/ int check_buffer_type( unsigned char *value, struct variablename *v );
/*forward*/ static int check_type_name( unsigned char *value, struct variablename *v, enum buffer_types typ );
/*forward*/ int check_file_name( unsigned char *value, struct variablename *v );
/*forward*/ int check_macro_name( unsigned char *value, struct variablename *v );
/*forward*/ int check_buffer_check( int value, struct variablename *v );
/*forward*/ int check_buffer_name( unsigned char *value, struct variablename *v );
/*forward*/ void fetch_buffer_type( struct expression *e );
/*forward*/ static void fetch_type_name( struct expression *e, int typ );
/*forward*/ void fetch_file_name(struct expression *e);
/*forward*/ void fetch_macro_name(struct expression *e);
/*forward*/ void fetch_buffer_check( struct expression *e );
/*forward*/ void fetch_buffer_name( struct expression *e );
/*forward*/ void fetch_current_keymap( struct expression *e );
/*forward*/ void fetch_auto_fill_hook( struct expression *e );
/*forward*/ void fetch_default_auto_fill_hook( struct expression *e );
/*forward*/ void fetch_process_key_hook( struct expression *e );
/*forward*/ int delete_window( void );
/*forward*/ int next_window( void );
/*forward*/ int previous_window( void );
/*forward*/ int shrink_window( void );
/*forward*/ int enlarge_window( void );
/*forward*/ int change_window_size(int delta);
/*forward*/ int narrow_window( void );
/*forward*/ int widen_window( void );
/*forward*/ static int change_window_width(int delta);
/*forward*/ static void window_horizontal_move( struct window *w, int add_columns, int abs_column );
/*forward*/ int scroll_one_column_left( void );
/*forward*/ int scroll_one_columnr_ight( void );
/*forward*/ int column_to_left_of_window( void );
/*forward*/ void window_move( struct window *w, int down, int lots, int dottop );
/*forward*/ int scroll_one_line_up( void );
/*forward*/ int scroll_one_line_down( void );
/*forward*/ int next_page( void );
/*forward*/ int previous_page( void );
/*forward*/ int line_to_top_of_window( void );
/*forward*/ int page_next_window( void );
/*forward*/ void window_size_thunk( struct expression *e );
/*forward*/ void window_column_thunk( struct expression *e );
/*forward*/ void init_wn_man( void );

GLOBAL SAVRES int rms_attribute_override = 0;
GLOBAL SAVRES int default_rms_attribute
#ifdef vms
	= FAB$C_VAR
#endif
	;

int list_buffers( void )
	{
	struct emacs_buffer *old;
	struct emacs_buffer *p;

	old = bf_cur;
	scratch_bfn( u_str("Buffer list"), interactive );
	ins_str( u_str(
"   Size  Type   Buffer                         Mode               File\n"
"   ----  ----   ------                         ----               ----\n"  ) );
	p = buffers;
	while( p != NULL )
		{
		unsigned char line[300];
		struct modespecific *mode;
		mode = &p->b_mode;
		sprintfl
		(
		line, sizeof( line ),
		u_str("%7d%6s %c %-30s %c%c%c%c %-14s %s\n" ),
			p->b_size1+ p->b_size2,
			p->b_kind == FILEBUFFER ? u_str("File") :
			p->b_kind == MACROBUFFER ? u_str("Macro") :
				u_str("Scr"),
			(p->b_modified != 0 ?  'M' : ' '),
			p->b_buf_name,
			(mode->md_abbrevon ?  'A' : ' '),
			(p->b_checkpointed != -1 ?  'C' : ' '),
			(p->b_journalling && (p->b_kind == SCRATCHBUFFER ? journal_scratch_buffers : 1) ?  'J' : ' '),
			(mode->md_replace ?  'R' : ' '),
			&mode->md_modestring,
			(p->b_fname != 0 ?  p->b_fname : u_str("")) );
		ins_str( &line[0] );
		p = p->b_next;
		}
	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );
	return 0;
	}



int delete_other_windows( void )
	{
	struct window *w, *next;
	w = windows;
	while( w != NULL )
		{
		next = w->w_next;
		if( w != wn_cur )
			del_win( w );
		w = next;
		}
	return 0;
	}



int split_current_window( void )
	{
	set_win( split_win( wn_cur ) );
	return 0;
	}

int split_current_window_vertically( void )
	{
	set_win( split_win_vert( wn_cur ) );
	return 0;
	}



int kill_buffer_command( void )
	{
	unsigned char *bn;
	struct emacs_buffer *b;

	bn = getescword( buf_names, u_str(": delete-buffer ") );
	if( bn == 0 )
		return 0;
	if( (b = find_bf( bn )) == 0 )
		{
		error( u_str("Buffer \"%s\" does not exist"), bn );
		return 0;
		}
	if( b == minibuf )
		{
		error( u_str("The Mini Buffer \"%s\" cannot be delete"), bn );
		return 0;
		}
	bn = savestr( bn );	/* bn will be zapped by getnbstr... */
	if( b->b_modified != 0 && interactive )
		{
		if( !get_yes_or_no
		(
		0,
		u_str("\"%s\" is modified, do you really want to delete it? "),
		bn
		) )
			{
			free( bn );
			return 0;
			}
		}
	proc_de_ref_buf( b );
	wind_de_ref_buf( b );
	undo_de_ref_buf( b );
	use_var_de_ref_buf( b );

	kill_bfn( bn );

	done_is_done();
	free( bn );
	return 0;
	}

int switch_to_buffer( void )
	{
	set_bfn( getescword( buf_names, u_str("Buffer: ") ) );
	tie_win( (wn_cur->w_next != NULL ? wn_cur : windows), bf_cur );
	wn_cur->w_horizontal_scroll = 1;

	return 0;
	}

int pop_to_buffer( void )
	{
	set_bfn( getescword( buf_names, u_str(": pop-to-buffer ") ) );
	if( window_on( bf_cur ) == 0 )
		wn_cur->w_horizontal_scroll = 1;
	return 0;
	}

int temp_use_buffer( void )
	{
	set_bfn( getescword( buf_names, u_str(": temp-use-buffer ") ) );
	return 0;
	}

int erase_buffer( void )
	{
	erase_bf( bf_cur );
	return 0;
	}



int use_old_buffer( void )
	{
	int bfn;
	bfn = getword( buf_names, u_str("Buffer: ") );
	if( bfn >= 0 )
		{
		set_bfn( buf_names[bfn] );
		tie_win
		(
		(( wn_cur->w_next != 0 ) ?  wn_cur : windows),
		bf_cur );
		}
	return 0;
	}

static int str_table_match
	(
	unsigned char **table,
	unsigned char *string,
	int siz
	)
	{
	unsigned char **p;
	int i;

	p = table;
	_str_lwr( string );

	for( i=0; ; i += 1 )
		{
		if( p[i] == 0 )
			break;
		if( _str_ncmp( p[i], string, siz ) == 0 )
			return i;
		}

	return -1;
	}



/*
 * The following routines make up the range check_ thunks for various buffer
 * specific values. They implement the setting of the following list of
 * variables:
 *
 *	current-buffer-type		check_buffer_type
 *	current-buffer-file-name	check_file_name
 *	current-buffer-macro-name	check_macro_name
 *	checkpoint-current-buffer	check_buffer_check
 *	current-buffer-name		check_buffer_name
 */

unsigned char *rms_attributes[] =
	{
	u_str("none"),
	u_str("fixed"),
	u_str("variable"),
	u_str("variable-fixed-control"),
	u_str("stream"),
	u_str("stream-lf"),
	u_str("stream-cr"),
	0
	};

int check_buffer_rms_attribute
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	int type_is;
	if( value == 0 )
		return 0;
	type_is = str_table_match( rms_attributes, value, _str_len( value ) );
	if( type_is < 0 )
		error( u_str("Bad buffer RMS attribute specified") );
	else
#ifdef vms
		if( type_is != FAB$C_VAR
		&& type_is != FAB$C_STM
		&& type_is != FAB$C_STMLF
		&& type_is != FAB$C_STMCR )
			error( u_str( "RMS attribute not supported" ) );
		else
#endif
			bf_cur->b_rms_attribute = type_is;
	return 0;
	}


int check_default_rms_attribute
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	int type_is;
	if( value == 0 )
		return 0;
	type_is = str_table_match( rms_attributes, value, _str_len( value ) );
	if( type_is < 0 )
		error( u_str("Bad default RMS attribute specified") );
	else
#ifdef vms
		if( type_is != FAB$C_VAR
		&& type_is != FAB$C_STM
		&& type_is != FAB$C_STMLF
		&& type_is != FAB$C_STMCR )
			error( u_str( "RMS attribute not supported" ) );
		else
#endif
			default_rms_attribute = type_is;

	return 0;
	}

int check_override_rms_attribute
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	int type_is;
	if( value == 0 )
		return 0;
	type_is = str_table_match( rms_attributes, value, _str_len( value ) );
	if( type_is < 0 )
		error( u_str("Bad override RMS attribute specified") );
	else
#ifdef vms
		if( type_is != 0
		&& type_is != FAB$C_VAR
		&& type_is != FAB$C_STM
		&& type_is != FAB$C_STMLF
		&& type_is != FAB$C_STMCR )
			error( u_str( "RMS attribute not supported" ) );
		else
#endif
			rms_attribute_override = type_is;

	return 0;
	}

unsigned char *typeslist[] =
	{
	u_str("file"),
	u_str("scratch"),
	u_str("macro"),
	0
	};

int check_buffer_type( unsigned char *value, struct variablename *PNOTUSED(v) )
	{
	int type_is;
	if( value == 0 )
		return 0;
	type_is = str_table_match( typeslist, value, _str_len( value ) );
	if( type_is < 0 )
		error( u_str("Bad buffer type specified") );
	else
		{
		if( bf_cur->b_kind != type_is
		&& bf_cur->b_fname != 0 )
			{
			free( bf_cur->b_fname );
			bf_cur->b_fname = 0;
			cant_1line_opt = redo_modes = 1;
			}
		bf_cur->b_kind = type_is == 0 ? FILEBUFFER : type_is == 1 ? SCRATCHBUFFER : MACROBUFFER;
		}
	return 0;
	}

static int check_type_name
	(
	unsigned char *value,
	struct variablename *v,
	enum buffer_types typ
	)
	{
	unsigned char fullname[257];
	if( value == 0 )
		return 0;
	if( typ == FILEBUFFER )
		{
		_str_cpy( &fullname[0], save_abs( value ) );
		if( _str_len( &fullname[0] ) <= 0 )
			{
			error( u_str("\"%s\" is an illegal filename"), value );
			return 0;
			}
		}
	else
		_str_ncpy( fullname, value, 255 );
	fullname[256] = 0;
	if( bf_cur->b_fname != 0 )
		free( bf_cur->b_fname );
	bf_cur->b_kind = typ;
	bf_cur->b_fname = savestr( fullname );
	cant_1line_opt = redo_modes = 1;

	return 0;
	}



int check_file_name( unsigned char *value, struct variablename *v )
	{
	return check_type_name( value, v, FILEBUFFER );
	}

int check_macro_name( unsigned char *value, struct variablename *v )
	{
	return check_type_name( value, v, MACROBUFFER );
	}

int check_buffer_check
	(
	int value,
	struct variablename *PNOTUSED(v)
	)
	{
	if( (value&(~1)) != 0 )
		error( u_str("Boolean value expected for current-buffer-checkpointable") );
	else
		if( value )
			{
			if( bf_cur->b_checkpointed < 0 )
				bf_cur->b_checkpointed = 0;
			}
		else
			bf_cur->b_checkpointed = -1;
	cant_1line_opt = redo_modes = 1;

	return 0;
	}



int check_buffer_name
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	struct emacs_buffer *b;
	int i;

	if( value == 0 )
		return 0;
	if( (b = find_bf( value )) != 0 )
		{
		if( b != bf_cur )
			error( u_str("Buffer name \"%s\" is already in use"), value );
		return 0;
		}
	for( i=0; i<=n_buffers-1; i += 1 )
		if( _str_cmp( buf_names[i], bf_cur->b_buf_name ) == 0 )
			{
			buf_names[i] = savestr( value );
			free( bf_cur->b_buf_name );
			bf_cur->b_buf_name = buf_names[i];
			cant_1line_opt = redo_modes = 1;
			break;
			}

	return 0;
	}



/*
 * The following are the fetch thunks for the above variables
 */
unsigned char current_buffer_rms_attrib_str[32];
unsigned char rms_attribute_override_str[32];
unsigned char default_rms_attribute_str[32];

void fetch_buffer_rms_attribute
	(
	struct expression *e
	)
	{
	unsigned char *p;
	p = rms_attributes[ bf_cur->b_rms_attribute ];
	_str_cpy( e->exp_v.v_string, p );
	}

void fetch_default_rms_attribute
	(
	struct expression *e
	)
	{
	unsigned char *p;
	p = rms_attributes[ default_rms_attribute ];
	_str_cpy( e->exp_v.v_string, p );
	}

void fetch_rms_attribute_override
	(
	struct expression *e
	)
	{
	unsigned char *p;
	p = rms_attributes[ rms_attribute_override ];
	_str_cpy( e->exp_v.v_string, p );
	}

unsigned char current_buffer_type[8];
void fetch_buffer_type( struct expression *e )
	{
	unsigned char *p;
	p = typeslist[ bf_cur->b_kind ];
	_str_cpy( e->exp_v.v_string, p );
	}

unsigned char typnamvar[80];
static void fetch_type_name( struct expression *e, int typ )
	{
	if( typ == bf_cur->b_kind && bf_cur->b_fname != 0 )
		_str_ncpy( e->exp_v.v_string, bf_cur->b_fname, e->exp_int-1 );
	else
		e->exp_v.v_string[ 0 ] = 0;
	}

void fetch_file_name(struct expression *e)
	{
	fetch_type_name( e, FILEBUFFER );
	}

void fetch_macro_name(struct expression *e)
	{
	fetch_type_name( e, MACROBUFFER );
	}

int bufcheckvar;
void fetch_buffer_check(struct expression *e)
	{
	*e->exp_v.v_value = (bf_cur->b_checkpointed != -1);
	}

unsigned char bufnamevar[80];
void fetch_buffer_name(struct expression *e)
	{
	_str_ncpy( e->exp_v.v_string, bf_cur->b_buf_name, e->exp_int-1 );
	}

unsigned char current_buffer_keymap[80];
void fetch_current_keymap(struct expression *e)
	{
	unsigned char *s;

	s = macro_address_to_name( bf_cur->b_mode.md_keys  );

	if( s != 0 )
		{
		_str_ncpy( e->exp_v.v_string, s, e->exp_int-1 );
		return;
		}

	e->exp_v.v_string[0] = 0;

	return;
	}

void fetch_auto_fill_hook( struct expression *e )
	{
	struct bound_name *b;
	b = bf_cur->b_auto_fill_proc;
	if( b != 0 )
		_str_ncpy( e->exp_v.v_string, b->b_proc_name, e->exp_int-1 );
	else
		e->exp_v.v_string[ 0 ] = 0;
 	return;
	}

void fetch_default_auto_fill_hook( struct expression *e )
	{
	struct bound_name *b;

	b = default_auto_fill_proc;
	if( b != 0 )
		_str_ncpy( e->exp_v.v_string, b->b_proc_name, e->exp_int-1 );
	else
		e->exp_v.v_string[ 0 ] = 0;
 	return;
	}



void fetch_process_key_hook( struct expression *e )
	{
	struct bound_name *b;
	b = bf_cur->b_process_key_proc;
	if( b != 0 )
		_str_ncpy( e->exp_v.v_string, b->b_proc_name, e->exp_int-1 );
	else
		e->exp_v.v_string[ 0 ] = 0;

	return;
	}



int delete_window( void )
	{
	del_win( wn_cur );
	set_bfp( wn_cur->w_buf );
	return 0;
	}

int left_window( void )
	{
	struct window *w;
	int count = arg;
	int i;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	w = wn_cur;

	for( i=1; i<=count; i += 1 )
		if( w->w_left != 0 )
			w = w->w_left;
		else
			{
			error( u_str("There are no windows to the left of the current window") );
			break;
			}
	set_win( w );

	return 0;
	}

int right_window( void )
	{
	struct window *w;
	int count = arg;
	int i;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	w = wn_cur;
	for( i=1; i<=count; i += 1 )
		if( w->w_right != 0 )
			w = w->w_right;
		else
			{
			error( u_str("There are no windows to the right the current window") );
			break;
			}
	set_win( w );

	return 0;
	}

int down_window( void )
	{
	struct window *w;
	int horiz_pos;
	int count = arg;
	int i;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	w = wn_cur;

	/*
	 *	Move to left edge of screen
	 */
	horiz_pos = 0;
	while( w->w_left != 0 )
		{
		w = w->w_left;
		horiz_pos++;
		}
	/*
	 *	Move down the desired number of windows
	 */
	for( i=1; i<=count; i += 1 )
		{
		while( w->w_right != 0 )
			w = w->w_right;

		if( w->w_next->w_next != 0 )
			w = w->w_next;
		else
			{
			error( u_str("There are no windows below the current window") );
			break;
			}
		}
	/*
	 *	Move over to the same horizontal position
	 */
	while( horiz_pos > 0 && w->w_right != 0 )
		{
		w = w->w_right;
		horiz_pos--;
		}
	set_win( w );

	return 0;
	}

int up_window( void )
	{
	struct window *w;
	int horiz_pos;
	int count = arg;
	int i;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	w = wn_cur;

	/*
	 *	Move to left edge of screen
	 */
	horiz_pos = 0;
	while( w->w_left != 0 )
		{
		w = w->w_left;
		horiz_pos++;
		}
	/*
	 *	Move up the desired number of windows
	 */
	for( i=1; i<=count; i += 1 )
		{
		if( w->w_prev != 0 )
			w = w->w_prev;
		else
			{
			error( u_str("There are no windows above current window") );
			break;
			}
		while( w->w_left != 0 )
			w = w->w_left;
		}
	/*
	 *	Move over to the same horizontal position
	 */
	while( horiz_pos > 0 && w->w_right != 0 )
		{
		w = w->w_right;
		horiz_pos--;
		}
	set_win( w );

	return 0;
	}

int next_window( void )
	{
	set_win( (( wn_cur->w_next != 0 ) ?  wn_cur->w_next : windows) );
	if( wn_cur->w_next == 0 && reset_minibuf == 0 )
		next_window();
	return 0;
	}

int previous_window( void )
	{
	struct window *w;
	w = wn_cur->w_prev;
	if( w == 0 )
		{
		w = windows;
		while( w->w_next != 0 )
			w = w->w_next;
		}
	set_win( w );
	if( wn_cur->w_next == 0 && reset_minibuf == 0 )
		previous_window();
	return 0;
	}



int shrink_window( void )
	{
	int delta;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		delta = -arg * numeric_arg (1);
	else
		delta = -arg;

	return change_window_size( delta );
	}

int enlarge_window( void )
	{
	int delta;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		delta = arg * numeric_arg (1);
	else
		delta = arg;

	return change_window_size( delta );
	}

int change_window_size(int delta)
	{
	struct window *w;

	if( wn_cur->w_height + delta < (( wn_cur->w_buf == minibuf ) ?  1 : 2) )
		goto win_change_error;

	w = wn_cur;
	while( w->w_right != 0 )
		w = w->w_next;
	if( chg_w_height( w->w_next, -delta, 1 ) == 0 )
		{
		w = wn_cur;
		while( w->w_left != 0 )
			w = w->w_prev;
		if( chg_w_height( w->w_prev, -delta, -1 ) == 0 )
			goto win_change_error;
		}


	if( chg_w_height( wn_cur, delta, 0 ) == 0 )
		error( u_str("Emacs bug -- window size change.") );
	return 0;
win_change_error:
	error( u_str("Cannot change window size") );
	return 0;
	}



int narrow_window( void )
	{
	return change_window_width( -arg );
	}

int widen_window( void )
	{
	return change_window_width( arg );
	}

static int change_window_width(int delta)
	{
	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		delta = delta * numeric_arg (1);

	if( wn_cur->w_width + delta < 1
	||	(
		chg_w_width( wn_cur->w_right, -delta, 1 ) == 0
		&& chg_w_width( wn_cur->w_left, -delta, -1 ) == 0 ) )
			error( u_str("Cannot change window size") );
	else
		if( chg_w_width( wn_cur, delta, 0 ) == 0 )
			error( u_str("Emacs bug -- window size change.") );
	return 0;
	}

static void window_horizontal_move
	(
	struct window *w,
	int add_columns,
	int abs_column
	)
	{
	if( abs_column != 0 )
		w->w_horizontal_scroll = abs_column;
	if( add_columns != 0 )
		w->w_horizontal_scroll =
				w->w_horizontal_scroll + add_columns;
	if( w->w_horizontal_scroll < 1 )
		w->w_horizontal_scroll = 1;
	cant_1win_opt = 1;
	}

int scroll_one_column_left( void )
	{
	window_horizontal_move( wn_cur, -arg, 0 );
	return 0;
	}

int scroll_one_column_right( void )
	{
	window_horizontal_move( wn_cur, arg, 0 );
	return 0;
	}

int column_to_left_of_window( void )
	{
	window_horizontal_move( wn_cur, 0, calc_col() );
	return 0;
	}

void window_move
	(
	struct window *w,
	int down,
	int lots,
	int dottop
	)
	{
	int n;
	int pos;
	n = arg;

	if( n < 0 )
		{
		down = ! down;
		n = -n;
		}
	if( lots != 0 )
		n = n * (w->w_height * 4 / 5);
	if( down != 0 )
		n = -n - 1;
	if( dottop != 0 )
		{
		n = -1;
		pos = dot;
		}
	else
		pos = to_mark( w->w_start );
	if( n != 0 )
		set_mark( w->w_start, w->w_buf, scan_bf( '\n', pos, n), 0 );
	w->w_force = w->w_force + 1;
	cant_1line_opt = 1;
	}

int scroll_one_line_up( void )
	{
	window_move( wn_cur, 0, 0, 0 );
	return 0;
	}

int scroll_one_line_down( void )
	{
	window_move( wn_cur, 1, 0, 0 );
	return 0;
	}

int next_page( void )
	{
	window_move( wn_cur, 0, 1, 0 );
	return 0;
	}

int previous_page( void )
	{
	window_move( wn_cur, 1, 1, 0 );
	return 0;
	}

int line_to_top_of_window( void )
	{
	window_move( wn_cur, 0, 0, 1 );
	return 0;
	}

int page_next_window( void )
	{
	struct window *w;
	int down;

	w = wn_cur->w_next;
	down = arg_state == have_arg;

	if( w == 0 || w->w_next == 0 )
		w = windows;
	if( w == wn_cur )
		error( u_str("there is no other window") );
	else
		{
		set_bfp( w->w_buf );
		if( argument_prefix_cnt == 1 && arg == 4 )
			arg = 1;
		window_move( w, down, 1, 0 );
		set_bfp( wn_cur->w_buf );
		}
	return 0;
	}

int window_size_variable;
void fetch_window_size( struct expression *e )
	{
	*e->exp_v.v_value = wn_cur->w_height;
	}

int check_window_size( int value, struct variablename *PNOTUSED(v) )
	{
	int delta;

	if( err )
		return 0;

	delta = value - wn_cur->w_height;
	if( delta != 0 )
		if( delta > 0 )
			{
			int i;

			/*
			 * expanding takes a little care do it
			 * we do it 1 line at a time
			 */
			err = 1; /* stop the error message being over written */
			for( i=1; i<=delta; i += 1 )
				change_window_size( 1 );
			err = 0;	/* we never get an error expanding */
			}
		else
			/* shrink can be done directly */
			change_window_size( delta );

	return 0;
	}

int window_width_variable;
void fetch_window_width( struct expression *e )
	{
	*e->exp_v.v_value = wn_cur->w_width;
	}

int check_window_width( int value, struct variablename *v )
	{
	int delta;
	int old_arg;

	if( err
	|| ! check_pos( value, v ) )
		return 0;

	old_arg = arg;
	arg = 1;

	delta = value - wn_cur->w_width;
	if( delta != 0 )
		if( delta > 0 )
			{
			int i;

			/*
			 * expanding takes a little care to do.
			 * we do it 1 line at a time
			 */
			err = 1; /* stop the error message being over written */
			for( i=1; i<=delta; i += 1 )
				change_window_width( 1 );

			err = 0;	/* we never get an error expanding */
			}
		else
			/* shrink can be done directly */
			change_window_width( delta );

	arg = old_arg;
	return 0;
	}

void fetch_window_column( struct expression *e )
	{
	*e->exp_v.v_value = wn_cur->w_horizontal_scroll;
	}

int check_window_column( int value, struct variablename *v )
	{
	if( check_pos( value, v ) )
		wn_cur->w_horizontal_scroll = value;

	return 0;
	}

/*
 *	(apply-colour-to-region start end colour)
 *
 *	First remove, shrink or split any existing regions
 *	that overlap the new region.
 *
 *	Add the new region if the color is positive,
 *	a color or 0 removes color.
 *	the regions are maintained in order of start
 *	position.
 */
void apply_colour_to_region( int start, int end, int colour )
	{
	struct rendition_region *rr, **rr_prev, *new_rr;

	/* make sure the args are in order */
	if( start > end )
		{
		int tmp = start;
		start = end;
		end = tmp;
		}

/* printf("Apply Colour %d to Region %d,%d \n", colour, start, end); */
	/* turn markers into ints */
	rr = bf_cur->b_rendition_regions;
	while( rr )
		{
		rr->rr_start_pos = to_mark( rr->rr_start_mark );
		rr->rr_end_pos = to_mark( rr->rr_end_mark );
/* printf("RR 0x%8.8x %d,%d %d\n", rr, rr->rr_start_pos, rr->rr_end_pos, rr->rr_colour ); */
		rr = rr->rr_next;
		}

	/* remove overlaps */
	rr_prev = &bf_cur->b_rendition_regions;
	rr = *rr_prev;
	while( rr != NULL )
		{
		/*
		 * if this region extends an existing region
		 * of the same colour
		 */
		if( rr->rr_colour == (LINE_ATTR_USER|colour) )
			{
			if( end == rr->rr_start_pos )
				{
				set_mark( rr->rr_start_mark, bf_cur, start, 0 );
				cant_1win_opt = 1;
				return;
				}
			if( start == rr->rr_end_pos )
				{
				set_mark( rr->rr_end_mark, bf_cur, end, 1 );
				cant_1win_opt = 1;
				return;
				}
			}

		/* the ne1w region is before this one */
		if( rr->rr_start_pos > end )
			break;


		/* the new region is after this one */
		if( rr->rr_end_pos <= start )
			{
			rr_prev = &rr->rr_next;
			rr = rr->rr_next;
			continue;
			}

		/* there is some overlap */
		if( start > rr->rr_start_pos )
			if( end < rr->rr_end_pos )
				{
				/* split this region */
				int split_start = rr->rr_start_pos;
				int split_colour = rr->rr_colour & ~LINE_ATTR_USER;

				apply_colour_to_region( split_start, start, split_colour );
				apply_colour_to_region( start, end, colour );
				return;
				}
			else
				{
				/* trim off the tail of the old region */
				set_mark( rr->rr_end_mark, bf_cur, start, 0 );
				rr->rr_end_pos = start;
				rr_prev = &rr->rr_next;
				rr = rr->rr_next;
				continue;
				}
		else
			{
			/* trim off the head of the old region */
			set_mark( rr->rr_start_mark, bf_cur, end, 0 );
			rr->rr_start_pos = end;
			}

		/* see if this region vanished */
		if( rr->rr_start_pos >= rr->rr_end_pos )
			{
			/* free the markers */
			dest_mark( rr->rr_start_mark );
			dest_mark( rr->rr_end_mark );

			/* delink from the chain */
			*rr_prev = rr->rr_next;
			/* free the rr */
			free( rr );
			/* move on to the next */
			rr = *rr_prev;
			}
		else
			/* the new region is before this one */
			break;
		}
	
	/* check for a valid colour */
	if( colour <= 0 || colour > 8 )
		return;

	new_rr = malloc_struct( rendition_region );
	if( new_rr == NULL )
		return;

	new_rr->rr_start_mark = new_mark();
	new_rr->rr_end_mark = new_mark();
	
	if( new_rr->rr_start_mark == NULL || new_rr->rr_end_mark == NULL )
		{
		free( new_rr );
		if( new_rr->rr_start_mark != NULL )
			free( new_rr->rr_start_mark );
		if( new_rr->rr_end_mark != NULL )
			free( new_rr->rr_end_mark );
		return;
		}

	/* start is left hand marker */
	set_mark( new_rr->rr_start_mark, bf_cur, start, 0 );
	/* end if right hand marker */
	set_mark( new_rr->rr_end_mark, bf_cur, end, 1 );
	/* set the color */
	new_rr->rr_colour = LINE_ATTR_USER |colour;
	/* link the new rr into the list */
	new_rr->rr_next = *rr_prev;
	*rr_prev = new_rr;

	cant_1win_opt = 1;
	return;
	}

int apply_colour_to_region_cmd( void )
	{
	int start, end, colour;

	/* colect the three args */
	if( check_args( 3, 3 ) )
		return 0;

	start = numeric_arg( 1 );
	end = numeric_arg( 2 );
	colour = numeric_arg( 3 );
	if( err )
		return 0;

	apply_colour_to_region( start, end, colour );
	return 0;
	}

void init_wn_man( void )
	{
	return;
	}
