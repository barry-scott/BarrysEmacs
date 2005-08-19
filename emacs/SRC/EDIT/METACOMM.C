/*module metacomm
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *
 * 	Copyright (c) 1982, 1983, 1984, 1985, 1986, 1987
 * 		Barry A. Scott and Nick Emery
 */

#include <emacs.h>


/*forward*/ int delete_region_to_buffer( void );
/*forward*/ int yank_buffer( void );
/*forward*/ int beginning_of_file( void );
/*forward*/ int end_of_file( void );
/*forward*/ int end_of_window( void );
/*forward*/ int beginning_of_window( void );
/*forward*/ static int skip_over(int punct, int inc, int dot);
/*forward*/ static int word_operation(int direction, int del);
/*forward*/ int forward_word( void );
/*forward*/ int backward_word( void );
/*forward*/ int delete_next_word( void );
/*forward*/ int delete_previous_word( void );
/*forward*/ static void apropos_helper( struct bound_name *b, unsigned char *keys, int len, int range );
/*forward*/ int apropos( void );
/*forward*/ int apropos_variable( void );
/*forward*/ void init_meta( void );


#define skip_over_punct 1		/* SkipOver codes */
#define skip_over_words 0

int delete_region_to_buffer( void )
	{
	unsigned char *fn;

	fn = getescword( buf_names, u_str(": delete-region-to-buffer ") );

	if( fn != 0 )
		if( bf_cur->b_mark == 0 )
			error( u_str("Mark not set") );
		else
			del_to_buf( to_mark(bf_cur->b_mark) - dot, 0, 1, fn );

	return 0;
	}			/* Of DeleteRegionToBuffer */



int yank_buffer( void )
	{
	unsigned char *fn;

	fn =getescword( buf_names, u_str(": yank-buffer ") );

	if( fn != 0 )
		insert_buffer( fn );

	return 0;
	}			/* of Of YankBuffer */



int beginning_of_file( void )
	{
	set_dot( first_character );
	return 0;
	}			/* of beginning_of_file */



int end_of_file( void )
	{
	set_dot( num_characters + 1 );
	return 0;
	}			/* Of EndOfFile */



int end_of_window( void )
	{
	set_dot( scan_bf( '\n', to_mark(wn_cur->w_start), wn_cur->w_height - 2) );
	end_of_line();
	return 0;
	}			/* Of EndOfWindow */



int beginning_of_window( void )
	{
	set_dot( to_mark(wn_cur->w_start) );
	return 0;
	}			/* Of BeginningOfWindow */



static int skip_over(int punct, int inc, int dot)
	{
	int n;

	if( inc < 0 )
		dot--;

	n = dot;

	if( punct == skip_over_words )
		{
		if( inc == 1 )
			{
			while( dot <= num_characters
			&& char_is( char_at( dot ), SYNTAX_WORD ) )
				dot++;
			}
		else
			{
			while( dot >= first_character
			&& char_is( char_at( dot ), SYNTAX_WORD ) )
				dot--;
			}
		}
	else
		{
		if( inc == 1 )
			{
			while( dot <= num_characters
			&& ! char_is( char_at( dot ), SYNTAX_WORD ) )
				dot++;
			}
		else
			{
			while( dot >= first_character
			&& ! char_is( char_at( dot ), SYNTAX_WORD ) )
				dot--;
			}
		}
	return dot - n;
	}

static int word_operation(int direction, int del)
	{
	int inc; int n;
	int count = arg;

	if( cur_exec != 0 && cur_exec->p_nargs > 0 )
		count = count * numeric_arg( 1 );

	do
	 	{
		inc = direction;
		n = skip_over( skip_over_punct, inc, dot );
		n = n + skip_over( skip_over_words, inc, dot + n );
		if( n == 0 )
			return 0;
		if( direction < 0 && del )
			{
			del_chars_in_buffer( dot, -n, 0);
			dot_left( -n );
			}
		else
			if( del )
			 	{
				int num;
				num = del_chars_in_buffer( dot, n, 1 );
				if( num != 0 )
					dot_right( num );
				}
			else
				dot_right( n );
		count--;
		}
	while( count > 0 && ! err );

	return 0;
	}               
	
int forward_word( void )
	{
	return word_operation( 1, 0 );
	}

int backward_word( void )
	{
	return word_operation( -1, 0 );
	}

int delete_next_word( void )
	{
	return word_operation( 1, 1 );
	}

int delete_previous_word( void )
	{
	return word_operation( -1, 1 );
	}

static struct bound_name *apropos_target;
static unsigned char *apropos_pointer;

static void apropos_helper
	(
	struct bound_name *b,
	unsigned char *keys,
	int len,
	int range
	)
	{
	int k;
	unsigned char *s;
	if( b != apropos_target )
		return;

	s = key_to_str( keys, len );
	k = _str_len( s );
	_str_cpy( apropos_pointer, u_str(", ") );
	_str_cpy( apropos_pointer + 2, s );
	apropos_pointer = apropos_pointer + k + 2;
	if( range > 1 )
		{
		keys[len - 1] = (unsigned char)(keys[len - 1] + range - 1);
		_str_cpy( apropos_pointer, u_str("..") );
		s = key_to_str( keys, len );
		k = _str_len( s );
		_str_cpy( apropos_pointer + 2, s );
		apropos_pointer = apropos_pointer + k + 2;
		keys[len - 1] = (unsigned char)(keys[len - 1] - range - 1);
		}
	apropos_pointer[0] = 0;
	}

int apropos( void )
	{				/* $? */

	unsigned char *keyword;
	int keyword_length;
	int i;
	struct emacs_buffer *old;
	unsigned char buf[4000];

	old = bf_cur;
	keyword = getnbstr( u_str(": apropos keyword: ") );
	if( keyword == 0 )
		return 0;
	keyword_length = _str_len( keyword );

	scratch_bfn( u_str("Help"), 1 );

	i = 0;
	while( mac_names[i] != 0 )
		{
		if( _str_str( mac_names[i], keyword ) != 0 )
			{
			unsigned char keys[3000];

			keys[0] = 0;
			apropos_pointer = &keys[0];
			apropos_target = mac_bodies[i];
			scan_map( current_global_map, apropos_helper, 1 );
			scan_map( old->b_mode.md_keys , apropos_helper, 1 );
			sprintfl( buf, sizeof( buf )-1,
					(( keys[0] != 0 ) ?  u_str("%-30s(%s)\n")
					: u_str("%s\n" )),
					mac_names[i], &keys[2]);
			ins_str( &buf[0] );
			}
		i++;
		}
	set_dot( 1 );
	bf_modified = 0;
	set_bfp( old );
	window_on( bf_cur );
	return 0;
	}
int apropos_variable( void )
	{				/* ^X-? */

	unsigned char *keyword;
	int keyword_length;
	int i;
	struct emacs_buffer *old;
	unsigned char buf[4000];

	old = bf_cur;
	keyword = getnbstr( u_str(": apropos-variable keyword: ") );
	if( keyword == 0 )
		return 0;
	keyword_length = _str_len( keyword );
	scratch_bfn( u_str("Help"), 1 );
	i = 0;
	while( var_names[i] != 0 )
		{
		if( _str_str( var_names[i], keyword ) != 0 )
			{
			sprintfl( buf, sizeof( buf )-1,
				u_str("%s\n"), var_names[i] );
			ins_str( &buf[0] );
			}
		i++;
		}
	set_dot( 1 );
	bf_modified = 0;
	set_bfp( old );
	window_on( bf_cur );
	return 0;
	}

void init_meta( void )
	{
	return;
	}
