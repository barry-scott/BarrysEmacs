/*
 * 	Copyright (c) 1982, 1983, 1984, 1985, 1986
 *		Barry A. Scott and Nick Emery
 */

#include <emacs.h>

/*forward*/ int self_insert(unsigned char c);
/*forward*/ int self_insert_command( void );
/*forward*/ void del_to_buf( int n, int where, int doit, unsigned char *name );
/*forward*/ static void line_move( int up, int n );
/*forward*/ void insert_buffer( unsigned char *name );
/*forward*/ int left_marker( void );
/*forward*/ int right_marker( void );
/*forward*/ static int left_or_right_marker(int right);

/* Strings */
unsigned char *kill_buffer_str = u_str("Kill buffer");
unsigned char *no_mark_set_str = u_str("No mark set in this buffer \"%s\"");
unsigned char *mark_not_set_str = u_str("Mark not set");



int beginning_of_line( void )
	{
	set_dot (scan_bf ('\n', dot, -1));
	return 0;
	}			/* Of BeginningOfLine */



int backward_character( void )
	{
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
	count = count * numeric_arg (1);

	dot_left (count);
	if( dot < first_character )
	 	{
		set_dot (first_character);
		error (u_str("You are at the beginning of the buffer"));
		}
	return 0;
	}			/* Of BackwardCharacter */



int exit_emacs( void )
	{
	/*
	 * See if the user wants to specify a command, fetch_ it, and save
	 *it for transmission when EMACS really exits back to DCL
	 */

	if( exit_emacs_dcl_command != 0 )
		{
		free (exit_emacs_dcl_command);
		exit_emacs_dcl_command = 0;
		}
	if( arg_state == have_arg
	|| (cur_exec != 0 && cur_exec->p_nargs >  0) )
		{
		unsigned char *p = getstr(u_str(": exit-emacs "));

		if( p == 0 )
			return 0;

		exit_emacs_dcl_command = savestr (p);
		}

	return -1;
	}			/* Of ExitEmacs */



int delete_next_character( void )
	{
	int repeat_count;
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs >  0 )
		count = count * numeric_arg (1);

	if( (repeat_count = del_chars_in_buffer (dot, count, 1)) != 0 )
		dot_right (repeat_count);

	return 0;
	}			/* Of DeleteNextCharacter */

int end_of_line( void )
	{
	int ndot = scan_bf( '\n', dot ,1 );

	if( dot != ndot )
		{
		set_dot (ndot);
		if( char_at (ndot - 1) == '\n' )
			{
			dot_left( 1 );
			if( dot < first_character )
			set_dot (first_character);
			}
		}
	return 0;
	}			/* Of end_of_line */



int forward_character( void )
	{
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	dot_right (count);

	if( dot > num_characters + 1 )
	 	{
		set_dot (num_characters + 1);
		error (u_str("You are at the end of the buffer."));
		}
	return 0;
	}			/* Of ForwardCharacter */



int illegal_operation( void )
	{
	err = 1;
	return 0;
	}			/* Of illegal_operation */



int delete_previous_character( void )
	{
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	del_chars_in_buffer (dot, count, 0);
	dot_left (count);

	return 0;
	}			/* Of DeletePreviousCharacter */



int newline_and_indent( void )
	{
	int dc = cur_indent();

	self_insert ('\n');
	to_col (dc);
	return 0;
	}			/* Of NewlineAndIndent */



int kill_to_end_of_line( void )
	{
	int nd;
	int count = arg;
	int merge = last_proc == kill_to_end_of_line;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	do
		{
		arg = 1;
		nd = dot;
		end_of_line ();
		nd = dot - nd;
		if( nd <= 0 )
			nd = -1;
		del_to_buf (-nd, merge, 1, kill_buffer_str);
		merge = 1;
		count--;
		}
	while( count > 0 );

	return 0;
	}			/* Of KillToEndOfLine */



int redraw_display( void )
	{
	screen_garbaged = 1;
	return 0;
	}			/* Of RedrawDisplay */



int newline_command( void )
	{
	self_insert ('\n');
	return 0;
	}			/* Of Newline_command */

int current_line_command( void )
	{
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = current_line_number();

	return 0;
	}			/* Of CurrentLine_command */

int next_line( void )
	{
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	line_move (0, count);
	return 0;
	}			/* Of NextLine */



int previous_line( void )
	{
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	line_move (1, count);
	return 0;
	}			/* Of PreviousLine */



static void line_move ( int up, int n )
	{
	static int lastcol;

	int ndot;
	int col = 1;
	int lim = num_characters + 1;

	if( n == 0 )
		return;

	if( n < 0 )
		{
		n = -n;
		up = ! up;
		}
	if( up )
		n = -n - 1;

	if( last_proc != next_line
	&& last_proc != previous_line )
	lastcol = ((track_eol != 0 && dot < lim && char_at (dot) == '\n') ? 9999 : cur_col );

	ndot = scan_bf ('\n', dot, n);
	while( col < lastcol && ndot < lim )
	 	{
		n = char_at (ndot);
		if( n == '\n' )
			break;
		if( n == '\t' )
			col = ((col - 1) / bf_mode.md_tabsize + 1) *
				bf_mode.md_tabsize + 1;
		else
			if( control_character(n) )
				col += ctl_arrow != 0 ?
						(term_deccrt != 0 
						&& (n == ctl('k')
						|| n == ctl('l')
						|| n == '\r'
						|| n == '\033') ) ?
							1
						:
							2
					:
						4;
			else
				col++;

		ndot++;
		}

	set_dot (ndot);
	dot_col = col;
	col_valid = 1;
	}			/* Of LineMove */



int newline_and_backup( void )
	{
	int larg = arg;

	self_insert( '\n' );
	dot_left (larg);
	return 0;
	}			/* Of NewlineAndBackup */



int quote_character( void )
	{
	int abbrev = bf_mode.md_abbrevon;

	bf_mode.md_abbrevon = 0;
	self_insert( (unsigned char)get_char () );
	bf_mode.md_abbrevon = abbrev;

	return 0;
	}			/* Of QuoteCharacter */



int transpose_characters( void )
	{
	if( dot >= 3 )
	 	{
		unsigned char c = char_at(dot - 1);

		del_back (dot, 1);
		insert_at (dot - 2, c);
		}

	return 0;
	}			/* Of TransposeCharacters */



int argument_prefix( void )
	{
	if( arg_state == no_arg )
	 	{
		arg = 4;
		argument_prefix_cnt = 1;
		}
	else
	 	{
		arg = arg * 4;
		argument_prefix_cnt++;
		}
	arg_state = prepared_arg;

	return 0;
	}			/* ArgumentPrefix */



static int x_region_to_buffer( unsigned char *str, int operation )
	{
	unsigned char *name;

	if( bf_cur->b_mark == 0 )
	 	{
		error( mark_not_set_str);
		return 0;
		}

	name = getnbstr (str);
	if( name != 0 )
		del_to_buf (to_mark (bf_cur->b_mark) - dot, operation, 0, name);

	return 0;
	}			/* Of PrependRegionToBuffer */



int copy_region_to_buffer( void )
	{
	return x_region_to_buffer (u_str(": copy-region-to-buffer "), 0);
	}

int append_region_to_buffer( void )
	{
	return x_region_to_buffer (u_str(": append-region-to-buffer "), 1);
	}

int prepend_region_to_buffer( void )
	{
	return x_region_to_buffer (u_str(": prepend-region-to-buffer "), -1);
	}



int delete_to_kill_buffer( void )
	{
	if( bf_cur->b_mark == 0 )
	 	{
		error( mark_not_set_str);
		return 0;
		}

	del_to_buf (to_mark (bf_cur->b_mark) - dot, 0, 1, kill_buffer_str);

	return 0;
	}			/* Of DeleteToKillBuffer */



int yank_from_kill_buffer( void )
	{
	int count = arg;
	int i;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg (1);

	for( i=1; i<=count ; i += 1 )
		insert_buffer (kill_buffer_str);
	return 0;
	}	   		   		   /* Of YankFromKillBuffer */

int minus_command( void )
	{
	if( arg_state == have_arg && argument_prefix_cnt > 0 )
	 	{
		arg = -arg;
		argument_prefix_cnt = -1;
		arg_state = prepared_arg;
		return 0;
		}

	self_insert_command();

	return 0;
	}			/* Of Minus */



int meta_minus( void )
	{
	argument_prefix_cnt = -1;
	arg = -arg;
	arg_state = prepared_arg;

	return 0;
	}			/* MetaMinus */



int digit_command( void )
	{
	if( arg_state == have_arg )
	 	{
		if( argument_prefix_cnt != 0 )
			arg = 0;

		if( arg < 0 || argument_prefix_cnt < 0 )
			arg = arg * 10 - (last_key_struck - '0');
		else
			arg = arg * 10 + last_key_struck - '0';

		argument_prefix_cnt = 0;
		arg_state = prepared_arg;
		return 0;
		}

	self_insert_command();

	return 0;
	}			/* Of Digit */



int meta_digit (void)
	{
	if( arg_state == have_arg )
	 	{
		if( argument_prefix_cnt != 0 )
			arg = 0;

		if( arg < 0 || argument_prefix_cnt < 0 )
			arg = arg * 10 - (last_key_struck - '0');
		else
			arg = arg * 10 + last_key_struck - '0';

		argument_prefix_cnt = 0;
		arg_state = prepared_arg;
		}
	else
	 	{
		arg = last_key_struck - '0';
		argument_prefix_cnt = 0;
		arg_state = prepared_arg;
		}

	return 0;
	}			/* Of MetaDigit */



int delete_white_space( void )
	{
	int c;
	int p1;
	int p2;

	p1 = dot;
	p2 = num_characters;

	while( p1 <= p2 && ((c = char_at (p1)) == ' ' || c == '\t'))
		p1++;

	p2 = dot;
	do
		p2--;
	while( p2 >= first_character
	&& ((c = char_at (p2)) == ' ' || c == '\t') );

	set_dot (p2 + 1);
	if( (p1 = p1 - p2 - 1) > 0 )
		del_frwd (dot, p1);

	return 0;
	}			/* Of  DeleteWhiteSpace */



int self_insert_command( void )
	{
	return self_insert( last_key_struck );
	}

int self_insert (unsigned char c)
	{
	int p;
	int repeat_count = arg;

	arg = 1;

	if( bf_mode.md_abbrevon && ! char_is (c, SYNTAX_WORD)
	&& (p = dot - 1) >= first_character
	&& char_is (char_at (p), SYNTAX_WORD) )
		if( abbrev_expand () != 0 )
			return 0;

	do
	 	{
		if( c > ' '
		&& ((p = dot) > num_characters || char_at (p) == '\n') )
			if( p > first_character && cur_col > bf_mode.md_rightmargin )
			 	{
				unsigned char bfc;

				if( bf_cur->b_auto_fill_proc != 0 )
			  		{
					execute_bound (bf_cur->b_auto_fill_proc);
					if( ml_value->exp_type ==  ISINTEGER
					&& ml_value->exp_int == 0 )
						return 0;
					}
				else
					{
					while( (p = dot - 1) >= first_character)
						{
						bfc = char_at (p);
						if( bfc == '\n' )
							{
							p = 0;
							break;
							}
						if( !control_character(bfc) )
							{
							dot_col--;
							dot--;
							}
						else
							dot_left (1);

						if( (bfc == ' ' || bfc == '\t')
						&& cur_col <= bf_mode.md_rightmargin )
							break;
						}
					if( p >= first_character )
						{
						delete_white_space ();
						arg = 1;
						insert_at (dot, '\n');
						dot_right (1);
						to_col (bf_mode.md_leftmargin);
						if( bf_mode.md_prefixstring[ 0 ] != 0 )
							ins_str (bf_mode.md_prefixstring);
						}
					end_of_line ();
					}
				}
		if( bf_mode.md_replace
		&&  char_at (dot) !=  '\n'
		&& c != '\n' )
			{
			del_frwd (dot, 1);
			insert_at (dot, c);
			if( bf_modified == 0 )
			 	{
				redo_modes = 1;
				cant_1line_opt = 1;
				}
			bf_modified++;
			}
		else
			insert_at (dot, c);

		dot_right (1);
		}
	while( (repeat_count = repeat_count - 1) > 0 );

	return 0;
	}



int check_auto_fill_hook(unsigned char *val, struct variablename *v)
	{
	return check_is_proc (val, v, &bf_cur->b_auto_fill_proc);
	}

int check_default_auto_fill_hook (unsigned char *val, struct variablename *v)
	{
	return check_is_proc (val, v, &default_auto_fill_proc);
	}

int check_process_key_hook (unsigned char *val, struct variablename *v)
	{
	return check_is_proc (val, v, &bf_cur->b_process_key_proc);
	}



int set_mark_command( void )
	{
	if( bf_cur->b_mark == 0 )
		bf_cur->b_mark = new_mark ();
	set_mark (bf_cur->b_mark, bf_cur, dot, 0);
	if( interactive )
		message (u_str("Mark set."));
	cant_1line_opt = 1;

	return 0;
	}			/* Of set_mark_command */



int un_set_mark_command( void )
	{
	if( bf_cur->b_mark != NULL )
	 	{
		dest_mark (bf_cur->b_mark);
		bf_cur->b_mark = 0;
		}
	cant_1line_opt = 1;

	return 0;
	}			/* Of UnSet_command */



int left_marker( void )  
	{
	return left_or_right_marker( 0 );
	}

int right_marker( void )  
	{
	return left_or_right_marker( 1 );
	}

static int left_or_right_marker(int right)
	{
	struct marker *m;
	int n;
	struct emacs_buffer *old;

	if( ! eval_arg( 1 ) )
		return 0;

	if( ml_value->exp_type != ISMARKER )
		{
		error( u_str("%s expects its argument to be a marker"),
			cur_exec->p_proc->b_proc_name );
		return 0;
		}
	old = bf_cur;
	m = new_mark();
	n = to_mark( ml_value->exp_v.v_marker );
	release_expr( ml_value );
	ml_value->exp_type = ISMARKER;
	set_mark( m, bf_cur, n, right );
	ml_value->exp_v.v_marker = m;
	ml_value->exp_release = 1;
	set_bfp( old );

	return 0;
	}



int exchange_dot_and_mark( void )
	{
	int old_dot = dot;

	if( bf_cur->b_mark == 0 )
		error( no_mark_set_str, bf_cur->b_buf_name);
	else
	 	{
		set_dot (to_mark (bf_cur->b_mark));
		set_mark (bf_cur->b_mark, bf_cur, old_dot, 0);
		}

	return 0;
	}			/* Of ExchangeDotAndMark */



int erase_region( void )
	{
	if( bf_cur->b_mark == 0 )
		error( no_mark_set_str, bf_cur->b_buf_name);
	else
	 	{
		int n = to_mark(bf_cur->b_mark)- dot;

		if( n < 0 )
			{
			n = -n;
			dot_left (n);
			}
		del_frwd (dot, n);
		}

	return 0;
	}



void del_to_buf
	(
	int n,
	int where,
	int doit,
	unsigned char *name
	)
	{
	int p = dot;
	struct emacs_buffer *old = bf_cur;
	struct emacs_buffer *kill = find_bf(name);

	if( kill == 0 )
		kill = new_bf (name);

	if( where == 0 )
		erase_bf (kill);

	if( n < 0 )
	 	{
		n = -n;
		p = p - n;
		}
	if( p < first_character )
	 	{
		n = n + p - first_character;
		p = first_character;
		}
	if( p + n  >  num_characters + 1 )
		n = num_characters + 1 - p;

	if( n <= 0 )
		return;

	gap_to (p);
	set_bfp (kill);
	set_dot (( where <= 0 ) ?  first_character : num_characters + 1);
	ins_cstr (old->b_base + old->b_size1 + old->b_gap, n);
	set_bfp (old);
	if( doit != 0 )
	 	{
		del_frwd (p, n);
		set_dot (p);
		}
	}			/* Of Del to buf */

/* insert the contents of the named buffer at the current position */
void insert_buffer ( unsigned char *name )
	{
	struct emacs_buffer *who = find_bf(name);

	if( who == 0 )
	 	{
		error (u_str ("Non-existant buffer: \"%s\""), name);
		return;
		}
	if( who == bf_cur )
	 	{
		error (u_str ("Inserting a buffer into itself."));
		return;
		}
	ins_cstr (who->b_base, who->b_size1);
	ins_cstr (who->b_base + who->b_size1 + who->b_gap, who->b_size2);
	}



int move_to_comment_column( void )
	{
	bf_cur->b_mode.md_leftmargin =
	bf_mode.md_leftmargin = cur_col == 1 ? 1 : bf_mode.md_commentcolumn;
	to_col (bf_mode.md_leftmargin);

	return 0;
	}			/* of MoveToCommentColumn */



int widen_region( void )
	{
	bf_cur->b_mode.md_headclip = bf_mode.md_headclip = 1;
	bf_cur->b_mode.md_tailclip = bf_mode.md_tailclip = 0;
	cant_1win_opt = 1;

	return 0;
	} 			/* of widen_region */



int narrow_region( void )
	{
	if( bf_cur->b_mark == 0 )
		error( no_mark_set_str, bf_cur->b_buf_name);
	else
	 	{
		int lo = to_mark(bf_cur->b_mark);
		int hi = dot;

		if( hi < lo )
			{
			int t = hi;

			hi = lo;
			lo = t;
			}

		bf_cur->b_mode.md_headclip = bf_mode.md_headclip = lo;
		bf_cur->b_mode.md_tailclip = bf_mode.md_tailclip = bf_s1 + bf_s2 + 1 - hi;
		cant_1win_opt = 1;
		}

	return 0;
	}



int save_restriction( void )
	{
	struct marker *ml = new_mark();
	struct marker *mh = new_mark();
	int rv;
	struct emacs_buffer *b = bf_cur;
	struct emacs_buffer *b2;

	set_mark (ml, bf_cur, bf_mode.md_headclip, 0);
	set_mark (mh, bf_cur, bf_s1 + bf_s2 + 1 - bf_mode.md_tailclip, 1);
	rv = progn_command ();
	b2 = bf_cur;
	b->b_mode.md_headclip = to_mark (ml);
	b->b_mode.md_tailclip = bf_s1 + bf_s2 + 1 - to_mark (mh);
	dest_mark (ml);
	dest_mark (mh);
	if( dot < first_character )
		set_dot (first_character);
	if( dot > num_characters )
		set_dot (num_characters + 1);
	if( bf_cur == b2 )
	 	{
		bf_mode.md_headclip = b->b_mode.md_headclip;
		bf_mode.md_tailclip = b->b_mode.md_tailclip;
		}
	else
		set_bfp (b2);
	cant_1win_opt = 1;

	return rv;
	}



/*
 * del_chars_in_buffer deletes n characters in the current buffer starting
 * at dot. It takes account of the state of replace-mode so that if you
 * delete characters in the middle of a line, then they are replaced with
 * a space. Characters deleted at the end of a line actually disappear.
 */

int del_chars_in_buffer ( int position, int number_of_characters, int fwd )
	{
	int replace = 0;

	if( bf_mode.md_replace != 0 )
	 	{
		/*
		 * Its a replace mode delete.
		 * Perform the replace mode check_s, and do the correct
		 * combination of inserts and deletes
		 */
		if( fwd != 0 )
			{
			int i;

			for( i=0; i<=number_of_characters - 1 ; i += 1 )
				{
				unsigned char ch = char_at(position + i);

				del_frwd (position + i, 1);
				if( ch != '\n' )
					{
					insert_at (position + i, ' ');
					replace++;
					}
				}
			if( char_at (position + replace) == '\n' )
			 	{
				del_frwd (position, replace);
				replace = 0;
				}
			}
		else
			{
			int i;
			int repl = char_at(position) != '\n';

			for( i=1; i<=number_of_characters ; i += 1 )
				{
				unsigned char ch = char_at(position - i);

				del_back (position - i + 1, 1);
				if( repl && ch != '\n' )
					{
					insert_at (position - i, ' ');
					replace++;
					}
				}
			}
		}
	else

	/*
	 * Its an insert mode delete, just use the
	 *  usual delete routines
	 */
		if( fwd != 0 )
			del_frwd (position, number_of_characters);
		else
			del_back (position, number_of_characters);

	return replace;
	}
