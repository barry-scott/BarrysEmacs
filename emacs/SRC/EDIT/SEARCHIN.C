/*			Copyright (c) 1985-1993
 *		Barry A. Scott and Nick Emery
 */
#include <emacs.h>

/*forward*/ int search_execute(int fflag, int addr);
/*forward*/ int search_advance( int lp, unsigned char *ep, int include, int exclude );
/*forward*/ static int backref(int i, int lp);
/*forward*/ static int cclass( unsigned char *char_set, int c, int af );

#define NL '\n'		/* Newlines character */

/*
 *	compile size and match width will be set to -1 if
 *	the size or width is not fixed.
 */
struct search_exp_info
	{
	int compile_size;	/* number of bytes in the compiled form */
	int match_width;	/* how many chars this can match */
	};

static struct search_exp_info search_exp_info[] =
	{
	{0,-1},	/* marks the end of the expression */
	{0,-1},	/* not used entry */
	{2,0},	/* CBRA		\( -- begin bracket */
	{2,0},	/* CBRA|STAR	\( -- begin bracket */
	{2,1},	/* CCHR		a vanilla character */
	{2,-1},	/* CCHR|STAR	a vanilla character */
	{1,1},	/* CDOT		. -- match anything except a newline */
	{1,-1},	/* CDOT|STAR	. -- match anything except a newline */
	{-1,1},	/* CCL		[...] -- character class */
	{-1,-1},/* CCL|STAR	[...] -- character class */
	{-1,1},	/* NCCL		[^...] -- negated character class */
	{-1,-1},/* NCCL|STAR	[^...] -- negated character class */
	{1,0},	/* CDOL		$ -- matches the end of a line */
	{1,-1},	/* CDOL|STAR	$ -- matches the end of a line */
	{1,-1},	/* CEOP		The end of the pattern */
	{1,-1},	/* CEOP|STAR	The end of the pattern */
	{2,0},	/* CKET		\) -- close bracket */
	{2,-1},	/* CKET|STAR	\) -- close bracket */
	{2,-1},	/* CBACK	\N -- backreference to the Nth bracketed string */
	{2,-1},	/* CBACK|STAR	\N -- backreference to the Nth bracketed string */
	{1,0},	/* CIRC		^ matches the beginning of a line */
	{1,0},	/* CIRC|STAR	^ matches the beginning of a line */
	{1,0},	/* BBUF		beginning of buffer \` */
	{1,0},	/* BBUF|STAR	beginning of buffer \` */
	{1,0},	/* EBUF		end of buffer \' */
	{1,0},	/* EBUF|STAR	end of buffer \' */
	{1,0},	/* BDOT		matches before dot \< */
	{1,0},	/* BDOT|STAR	matches before dot \< */
	{1,0},	/* EDOT		matches at dot \= */
	{1,0},	/* EDOT|STAR	matches at dot \= */
	{1,0},	/* ADOT		matches after dot \> */
	{1,0},	/* ADOT|STAR	matches after dot \> */
	{1,1},	/* WORDC	matches word character \w */
	{1,-1},	/* WORDC|STAR	matches word character \w */
	{1,1},	/* NWORDC	matches non-word characer \W */
	{1,-1},	/* NWORDC|STAR	matches non-word characer \W */
	{1,0},	/* WBOUND	matches word boundary \b */
	{1,0},	/* WBOUND|STAR	matches word boundary \b */
	{1,0},	/* NWBOUND	matches non-(word boundary) \B */
	{1,0},	/* NWBOUND|STAR	matches non-(word boundary) \B */
	{1,0},	/* SEA_SYN_COMMENT		only match inside a comment \c */
	{1,-1},	/* SEA_SYN_COMMENT|STAR		only match inside a comment \c */
	{1,0},	/* SEA_SYN_NOCOMMENT		only match outside a comment \C */
	{1,-1},	/* SEA_SYN_NOCOMMENT|STAR	only match outside a comment \C */
	{1,0},	/* SEA_SYN_STRING		only match inside a string \s */
	{1,-1},	/* SEA_SYN_STRING|STAR		only match inside a string \s */
	{1,0},	/* SEA_SYN_NOSTRING		only match outside a string \S */
	{1,-1}	/* SEA_SYN_NOSTRING|STAR	only match outside a string \S */
	};

/* check to see whether the most recently compiled regular expression
 *   matches the string starting at addr in the buffer.
 *   The search match is performed in the current buffer.
 *   fflag is true iff we are doing a forward search.
 */
int search_execute(int fflag, int addr)
	{
	int p1 = addr;
	unsigned char *trtp = sea_glob.sea_trt;
	int numchars = num_characters;
	int firstchar = first_character;
	unsigned char *cp = bf_p1 + p1;
	unsigned char *exp;
	int i;
	int inc = fflag ? 1 : -1;

	for( i=0; i<=MAX_NBRA-1; i++ )
		{
		if( sea_glob.sea_bra_slist[ i ] != NULL )
			dest_mark( sea_glob.sea_bra_slist[ i ] );
		sea_glob.sea_bra_slist[ i ] = NULL;

		if( sea_glob.sea_bra_elist[ i ] != NULL )
			dest_mark( sea_glob.sea_bra_elist[ i ] );
		sea_glob.sea_bra_elist[ i ] = NULL;
		}
	if( addr == 0 )
		return (-1);

	/* skip over any express elements that
	   have no effect on the matching */
	exp = sea_glob.sea_expbuf;
	while( search_exp_info[*exp].match_width == 0 )
		exp += search_exp_info[*exp].compile_size;

	if( exp[0] == CCHR
	&& sea_glob.sea_alternatives[1] == 0 )
		{
		#define SEARCH_FORWARD 1
		#define SEARCH_BACKWARD 0
		#define SEARCH_CASEFOLD 2
		#define SEARCH_USETRT 4

		unsigned char *s_g_e = sea_glob.sea_expbuf;
		int search_type = 0;
		unsigned char first_c = exp[1];	/* fast check for first character */
		unsigned char last_c = first_c;
		int last_c_offset=0;

		/*
		 *	Walk the express looking for the last char that
		 *	is a fixed offset from the first char.
		 */
		i = 0;
		while( search_exp_info[*exp].match_width >= 0 )
			{
			if( *exp == CCHR )
				{
				last_c_offset = i;
				last_c = exp[1];
				}
			if( search_exp_info[*exp].match_width == 1 )
				i++;
			if( search_exp_info[*exp].compile_size <= 0 )
				break;
			exp += search_exp_info[*exp].compile_size;
			}
		
		/*
		 *	The factors that effect the search algorithm
		 *	optimisations are
		 *
		 *	o	forward or backward search	bit 0
		 *	o	upcase char folding required	bit 1
		 *	o	char is greater then 127	bit 2
		 */
		first_c = trtp[ first_c ];
		last_c = trtp[ last_c ];
		if( fflag )
			search_type = SEARCH_FORWARD;
		if( bf_mode.md_foldcase )
			{
			/* Need a simple test for multinational letter */
			if( first_c > 127 || last_c > 127 )
				search_type |= SEARCH_USETRT;
			else if( (first_c >= 'a' && first_c <= 'z')
			&& (last_c >= 'a' && last_c <= 'z') )
				search_type |= SEARCH_CASEFOLD;
			else if( (first_c >= 'a' && first_c <= 'z')
			|| (last_c >= 'a' && last_c <= 'z') )
				search_type |= SEARCH_USETRT;
			}

		switch( search_type )
		{
		case SEARCH_FORWARD:
			if( numchars > bf_s1 )
				numchars = bf_s1;
			numchars = numchars - (p1 - 1);
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			if( --numchars >= last_c_offset )
			do
				{
				if( *cp++ == first_c
				&& char_at( p1+last_c_offset ) == last_c )
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++, --numchars >= last_c_offset );

			numchars = num_characters - (p1 - 1);
			cp = bf_p2 + p1;
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
				}
			if( --numchars >= last_c_offset )
			do
				{
				if( *cp++ == first_c
				&& char_at( p1+last_c_offset ) == last_c )
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++, --numchars >= last_c_offset );

			if( search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			return -1;

		case SEARCH_FORWARD|SEARCH_CASEFOLD:
			if( numchars > bf_s1 )
				numchars = bf_s1;
			numchars = numchars - (p1 - 1);
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
				}
			if( --numchars >= last_c_offset )
			do
				{
				/* match upper and lower case */
				if( (unsigned char)(*cp++ | 0x20) == first_c
				&& (unsigned char)(char_at( p1+last_c_offset )|0x20) == last_c)
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++, --numchars >= last_c_offset );

			numchars = num_characters - (p1 - 1);
			cp = bf_p2 + p1;
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			if( --numchars >= last_c_offset )
			do
				{
				if( (unsigned char)(*cp++ | 0x20) == first_c
				&& (unsigned char)(char_at( p1+last_c_offset )|0x20) == last_c)
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++,--numchars >= last_c_offset );

			if( search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			return -1;

		case SEARCH_FORWARD|SEARCH_USETRT:
			if( numchars > bf_s1 )
				numchars = bf_s1;
			numchars = numchars - (p1 - 1);
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			if( --numchars >= last_c_offset )
			do
				{
				if( trtp[*cp++] == first_c
				&& trtp[ char_at( p1+last_c_offset ) ] == last_c )
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++, --numchars >= last_c_offset );

			numchars = num_characters - (p1 - 1);
			cp = bf_p2 + p1;
			if( numchars == 0 && search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
				}
			if( --numchars >= last_c_offset )
			do
				{
				if( trtp [*cp++] == first_c
				&& trtp[ char_at( p1+last_c_offset ) ] == last_c )
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return sea_glob.sea_loc2 - sea_glob.sea_loc1;
						}

					if( err ) return -1;
					}
				}
			while( p1++, --numchars >= last_c_offset );

			if( search_advance( p1, s_g_e, 0, 0 ) )
				{
				sea_glob.sea_loc1 = p1;
				return sea_glob.sea_loc2 - sea_glob.sea_loc1;
				}
			return -1;

		case SEARCH_BACKWARD:
			do
				{
				if( char_at( p1 ) == first_c
				&& char_at( p1+last_c_offset ) == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
						}
					else
						if( err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;

		case SEARCH_BACKWARD|SEARCH_CASEFOLD:
			do
				{
				/* match upper and lower case */
				if( (unsigned char)(char_at( p1 ) | 0x20) == first_c
				&& (unsigned char)(char_at( p1+last_c_offset )|0x20) == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
						}
					else
						if( err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;

		case SEARCH_BACKWARD|SEARCH_USETRT:
			do
				{
				if( trtp[char_at (p1)] == first_c
				&& trtp[ char_at( p1+last_c_offset ) ] == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_glob.sea_loc1 = p1;
						return (sea_glob.sea_loc2 - sea_glob.sea_loc1);
						}
					else
						if( err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;
		default:
			break;
		}
		}
	else			/* regular algorithm */
		do
			{
			unsigned char **alt;
			alt = sea_glob.sea_alternatives;

			while( *alt != NULL )
				if( search_advance (p1, *alt, 0, 0) )
					{
					sea_glob.sea_loc1 = p1;
					return sea_glob.sea_loc2 - sea_glob.sea_loc1;
					}
				else
					{
					alt++;
					if( err )
						return -1;
					}
			p1 += inc;
			}
		while( p1 <= numchars && p1 >= firstchar );
	return -1;
	}



static unsigned char *syn_sea_error = u_str("search requires syntax-array to be enabled");

/* search_advance the match of the regular expression starting at ep along the
 *  string lp, simulates an NDFSA */
int search_advance
		(
		int lp,
		unsigned char *ep,
		int syn_include,
		int syn_exclude		
		)
	{
#define SYN_EXCLUDE	(syn_exclude && (syn_p[lp]&syn_exclude) != 0)
#define SYN_INCLUDE	(syn_include && (syn_p[lp]&syn_include) == 0)

	int curlp;
	unsigned char *trtp = sea_glob.sea_trt;
	unsigned char ch;
	unsigned char *ch_p = NULL;
	unsigned char *syn_p = NULL;
	int numchars = num_characters;
	int length = 0;
	int firstchar = first_character;
	int part;

	for( part=1; part<=2; part++ )
	{
	switch( part )
	{
	case 1:
		length = bf_s1;
		ch_p = bf_p1;
		syn_p = bf_cur->b_syntax.syntax_base;
		break;
	case 2:
		length = numchars;
		ch_p = bf_p2;
		syn_p = bf_cur->b_syntax.syntax_base + bf_gap;
		break;		
	}

/*	_m( 'advance_search at %d, length is %d', .lp, .length ); */
	while( (! err &&
		(lp <= length
		|| ((ch = *ep) & STAR) || ch == CKET || ch == EBUF )) )
		{
/*		_m( 'ep = %d, lp = %d char(.lp-1) = %d, char(.lp) = %d',
 *			.ep[0], .lp, .ch_p[ .lp-1 ], .ch_p[ .lp ] ); */

		if( (syn_include != 0 || syn_exclude != 0)
		&& bf_cur->b_syntax.syntax_valid <= lp )
			{
			syntax_fill_in_array( lp + 10000 );
			syn_p = bf_cur->b_syntax.syntax_base;
			if( part == 2 )
				syn_p += bf_gap;
			}

		switch( *ep++ )
		{
		case CCHR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( trtp[*ep++] != trtp[ch_p[lp]] )
				return 0;
			lp++;
			break;
		case CDOT:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (ch_p[lp] == NL) )
				return 0;
			lp++;
			break;
		case CDOL:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (ch_p[lp] != NL) )
				return 0;
			break;
		case CIRC:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ! (lp <= firstchar || char_at(lp-1) == NL) )
				return 0;
			break;
		case BBUF:
			if( (lp > firstchar) )
				return 0;
			break;
		case EBUF:
			if( (lp <= numchars) )
				return 0;
			break;
		case BDOT:
			if( (lp > dot) )
				return 0;
			break;
		case EDOT:
			if( (lp != dot) )
				return 0;
			break;
		case ADOT:
			if( (lp < dot) )
				return 0;
			break;
		case WORDC:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( char_is (ch_p[lp], SYNTAX_WORD) )
				lp++;
			else
				return 0;
			break;
		case NWORDC:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ( ! char_is (ch_p[lp], SYNTAX_WORD)) )
				lp++;
			else
				return 0;
			break;
		case WBOUND:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( !((lp <= firstchar || !char_is(char_at(lp-1), SYNTAX_WORD)) !=
				(lp > numchars || !char_is(ch_p[lp], SYNTAX_WORD))) )
				return 0;
			break;
		case NWBOUND:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ! ((lp <= firstchar ||  ! char_is (char_at(lp-1), SYNTAX_WORD)) ==
				(lp > numchars ||  ! char_is (ch_p[lp], SYNTAX_WORD))) )
				return 0;
			break;
		case SEA_SYN_COMMENT:
			if( !bf_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include ^= SYNTAX_COMMENT;
			syn_exclude &= ~SYNTAX_COMMENT;
			break;
		case SEA_SYN_NOCOMMENT:
			if( !bf_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include &= ~SYNTAX_COMMENT;
			syn_exclude ^= SYNTAX_COMMENT;
			break;
		case SEA_SYN_STRING:
			if( !bf_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include ^= SYNTAX_STRING;
			syn_exclude &= ~SYNTAX_STRING;
			break;
		case SEA_SYN_NOSTRING:
			if( !bf_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include &= ~SYNTAX_STRING;
			syn_exclude ^= SYNTAX_STRING;
			break;
		case CEOP:
			sea_glob.sea_loc2 = lp;
			return 1;
		case CCL:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (cclass( ep, ch_p[lp], 1 )) )
				{
				ep = &ep[ *ep ];
				lp++;
				}
			else
				return 0;
			break;
		case NCCL:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( cclass( ep, ch_p[lp], 0 ) )
				{
				ep = &ep[ *ep ];
				lp++;
				}
			else
				return 0;
			break;
		case CBRA:
			{
			struct marker **p;
			struct marker *m;

			p = &sea_glob.sea_bra_slist[*ep++ ];
			if( *p == NULL )
				m = *p = new_mark();
			else
				m = *p;
			set_mark( m, bf_cur, lp, 0 );
			}
			break;
		case CKET:
			{
			struct marker **p;
			struct marker *m;

			p = &sea_glob.sea_bra_elist[*ep++ ];
			if( *p == NULL )
				m = *p = new_mark();
			else
				m = *p;
			set_mark( m, bf_cur, lp, 0 );
			break;
			}
		case CBACK:
			ch = *ep++;
			if( (sea_glob.sea_bra_elist[ ch ] == 0) )
				{
				error (u_str("bad braces"));
				return 0;
				}
			if( backref(ch, lp) )
				{
				lp = lp +
					(to_mark( sea_glob.sea_bra_elist[ ch ] )
					- to_mark( sea_glob.sea_bra_slist[ ch ] ));
				}
			else
				return 0;
			break;

		case CBACK|STAR:
			ch = *ep++;
			if( (sea_glob.sea_bra_elist[ ch ] == 0) )
				{
				error (u_str("bad braces"));
				return 0;
				}
			curlp = lp;
			while( backref( ch, lp ) )
				{
				lp = lp -
					(to_mark( sea_glob.sea_bra_elist[ ch ] )
					- to_mark( sea_glob.sea_bra_slist[ ch ] ));
				}
			while( lp >= curlp )
				{
				if( search_advance(lp, ep, syn_include, syn_exclude) )
					return 1;
				lp = lp -
					(to_mark( sea_glob.sea_bra_elist[ ch ] )
					- to_mark( sea_glob.sea_bra_slist[ ch ] ));
				}
			break;

		case CDOT|STAR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;

			curlp = lp;
			while( lp <= numchars
			&& char_at(lp) != NL
			&& !SYN_EXCLUDE
			&& !SYN_INCLUDE )
				lp++;
			lp++;
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;

		case WORDC|STAR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			curlp = lp;
			while( lp <= numchars
			&& char_is (char_at(lp), SYNTAX_WORD) )
				lp++;
			lp++;
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;

		case NWORDC|STAR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			curlp = lp;
			while( lp <= numchars
			&&  ! char_is (char_at(lp), SYNTAX_WORD) )
				lp++;
			lp++;
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;

		case CCHR|STAR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			curlp = lp;
			ch = trtp[*ep];
			while( lp <= numchars
			&& trtp[char_at(lp)] == ch )
				lp++;
			lp++;
			ep++;
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;

		case CCL | STAR:
		case NCCL | STAR:
			curlp = lp;
			ch = (unsigned char)(ep[-1] == (CCL | STAR));
			while( lp <= numchars
			&& !SYN_EXCLUDE && !SYN_INCLUDE
			&& cclass( ep, char_at(lp), ch ) )
				lp++;
			lp++;
			ep = &ep[ *ep ];
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;

		default:
			error(u_str("Badly compiled pattern \"%s\" (Emacs internal error!)"),
					last_search_string);
			return 0;
		}
		}

	}
	if(( (ch = ep[0]) == CEOP || ch == CDOL ) )
		{
		sea_glob.sea_loc2 = lp;
		return 1;
		}
	return 0;
#undef SYN_EXCLUDE
#undef SYN_INCLUDE
	}



static int backref(int i, int lp)
	{
	int bp; int ep;

	bp = to_mark( sea_glob.sea_bra_slist[ i ] );
	ep = to_mark( sea_glob.sea_bra_elist[ i ] );
	while( lp <= num_characters && char_at(bp) == char_at(lp) )
		{
		bp++;
		lp++;
		if( bp >= ep )
			return 1;
		}
	return 0;
	}



static int cclass
		(
		unsigned char *char_set,
		int c,
		int af
		)
	{
	int n;
	unsigned char *trtp = sea_glob.sea_trt;

	n = *char_set++;
	while( (n = n - 1) != 0 )
		if( trtp[*char_set++] == trtp[c] )
			return af;
	return ! af;
	}
