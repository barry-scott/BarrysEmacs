//
//			Copyright (c) 1985-1993
//		Barry A. Scott and Nick Emery
//
#include <emacs.h>
#include <search_simple_algorithm.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


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
const int SEARCH_FORWARD( 1 );
const int SEARCH_BACKWARD( 0 );
const int SEARCH_CASEFOLD( 2 );
const int SEARCH_USETRT( 4 );

int SearchSimpleAlgorithm::search_execute(int fflag, int addr)
	{
	int p1 = addr;
	unsigned char *exp;
	int i;
	int inc = fflag ? 1 : -1;

	for( i=0; i<=SearchSimpleAlgorithm::MAX_NBRA-1; i++ )
		{
		sea_bra_slist[ i ].unset_mark();
		sea_bra_elist[ i ].unset_mark();
		}
	if( addr == 0 )
		return (-1);

	/* skip over any express elements that
	   have no effect on the matching */
	exp = sea_expbuf;
	while( search_exp_info[*exp].match_width == 0 )
		exp += search_exp_info[*exp].compile_size;

#if 0
	unsigned char *trtp = sea_trt;

	if( exp[0] == CCHR
	&& sea_alternatives[1] == 0 )
		{
		unsigned char *s_g_e = sea_expbuf;
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
		if( bf_cur->b_mode.md_foldcase )
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
			{
			// assume that we are starting in the S1 region
			unsigned char *cp = bf_cur->ref_char_at( p1 );
			// number of chars to look for the first char in
			int numchars = bf_cur->num_characters() - last_c_offset;
			if( numchars > bf_cur->b_size1 )
				numchars = bf_cur->b_size1 - last_c_offset;

			numchars -= p1 - 1;

			// look in S1
			if( --numchars >= 0 )
				do
					{
					if( *cp == first_c
					&& cp[ last_c_offset ] == last_c )
						{
						if( search_advance( p1, s_g_e, 0, 0 ) )
							{
							sea_loc1 = p1;
							return sea_loc2 - sea_loc1;
							}

						if( ml_err ) return -1;
						}
					cp++; p1++;
					}
				while( --numchars >= 0 );

			// look over the gap
			int limit = bf_cur->b_size1;
			if( limit > bf_cur->num_characters() )
				limit = bf_cur->num_characters();

			while( p1 <= limit )
				{
				if( search_advance( p1, s_g_e, 0, 0 ) )
					{
					sea_loc1 = p1;
					return sea_loc2 - sea_loc1;
					}
				p1++;
				}

			// look in S2 region
			numchars = bf_cur->num_characters() - (p1 - 1) - last_c_offset;
			cp = bf_cur->ref_char_at( p1 );

			if( --numchars >= 0 )
				do
					{
					if( *cp == first_c
					&& cp[ last_c_offset ] == last_c )
						{
						if( search_advance( p1, s_g_e, 0, 0 ) )
							{
							sea_loc1 = p1;
							return sea_loc2 - sea_loc1;
							}

						if( ml_err ) return -1;
						}
					cp++; p1++;
					}
				while( --numchars >= 0 );

			return -1;
			}

		case SEARCH_FORWARD|SEARCH_CASEFOLD:
			{
			// assume that we are starting in the S1 region
			unsigned char *cp = bf_cur->ref_char_at( p1 );
			// number of chars to look for the first char in
			int numchars = bf_cur->num_characters() - last_c_offset;
			if( numchars > bf_cur->b_size1 )
				numchars = bf_cur->b_size1 - last_c_offset;

			numchars -= p1 - 1;

			// look in S1 for match
			if( --numchars >= last_c_offset )
				do
					{
					/* match upper and lower case */
					if( (unsigned char)(*cp | 0x20) == first_c
					&& (unsigned char)(cp[last_c_offset]|0x20) == last_c)
						{
						if( search_advance( p1, s_g_e, 0, 0 ) )
							{
							sea_loc1 = p1;
							return sea_loc2 - sea_loc1;
							}

						if( ml_err ) return -1;
						}
					cp++; p1++;
					}
				while( --numchars >= 0 );

			// look over the gap
			int limit = bf_cur->b_size1;
			if( limit > bf_cur->num_characters() )
				limit = bf_cur->num_characters();

			while( p1 <= limit )
				{
				if( search_advance( p1, s_g_e, 0, 0 ) )
					{
					sea_loc1 = p1;
					return sea_loc2 - sea_loc1;
					}
				p1++;
				}

			// look in S2 region
			numchars = bf_cur->num_characters() - (p1 - 1) - last_c_offset;
			cp = bf_cur->ref_char_at( p1 );

			if( --numchars >= 0 )
			do
				{
				if( (unsigned char)(*cp | 0x20) == first_c
				&& (unsigned char)(cp[ last_c_offset ]|0x20) == last_c)
					{
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_loc1 = p1;
						return sea_loc2 - sea_loc1;
						}

					if( ml_err ) return -1;
					}
				cp++; p1++;
				}
			while( --numchars >= 0 );

			return -1;
			}

		case SEARCH_FORWARD|SEARCH_USETRT:
			{
			// assume that we are starting in the S1 region
			unsigned char *cp = bf_cur->ref_char_at( p1 );
			// number of chars to look for the first char in
			int numchars = bf_cur->num_characters() - last_c_offset;
			if( numchars > bf_cur->b_size1 )
				numchars = bf_cur->b_size1 - last_c_offset;

			numchars -= p1 - 1;

			// look in S1 for match
			if( --numchars >= 0 )
				do
					{
					if( trtp[*cp] == first_c
					&& trtp[ cp[last_c_offset] ] == last_c )
						{
						if( search_advance( p1, s_g_e, 0, 0 ) )
							{
							sea_loc1 = p1;
							return sea_loc2 - sea_loc1;
							}

						if( ml_err ) return -1;
						}
					cp++; p1++;
					}
				while( --numchars >= 0 );

			// look over the gap
			int limit = bf_cur->b_size1;
			if( limit > bf_cur->num_characters() )
				limit = bf_cur->num_characters();

			while( p1 <= limit )
				{
				if( search_advance( p1, s_g_e, 0, 0 ) )
					{
					sea_loc1 = p1;
					return sea_loc2 - sea_loc1;
					}
				p1++;
				}

			// look in S2 region
			numchars = bf_cur->num_characters() - (p1 - 1) - last_c_offset;
			cp = bf_cur->ref_char_at( p1 );

			if( --numchars >= 0 )
				do
					{
					if( trtp [*cp] == first_c
					&& trtp[ cp[ last_c_offset ] ] == last_c )
						{
						if( search_advance( p1, s_g_e, 0, 0 ) )
							{
							sea_loc1 = p1;
							return sea_loc2 - sea_loc1;
							}

						if( ml_err ) return -1;
						}
					cp++; p1++;
					}
				while( --numchars >= 0 );

			// not found
			return -1;
			}

		case SEARCH_BACKWARD:
			{
			int firstchar = bf_cur->first_character();

			do
				{
				if( bf_cur->char_at( p1 ) == first_c
				&& bf_cur->char_at( p1+last_c_offset ) == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_loc1 = p1;
						return (sea_loc2 - sea_loc1);
						}
					else
						if( ml_err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;
			}

		case SEARCH_BACKWARD|SEARCH_CASEFOLD:
			{
			int firstchar = bf_cur->first_character();

			do
				{
				/* match upper and lower case */
				if( (unsigned char)(bf_cur->char_at( p1 ) | 0x20) == first_c
				&& (unsigned char)(bf_cur->char_at( p1+last_c_offset )|0x20) == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_loc1 = p1;
						return (sea_loc2 - sea_loc1);
						}
					else
						if( ml_err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;
			}

		case SEARCH_BACKWARD|SEARCH_USETRT:
			{
			int firstchar = bf_cur->first_character();

			do
				{
				if( trtp[bf_cur->char_at (p1)] == first_c
				&& trtp[ bf_cur->char_at( p1+last_c_offset ) ] == last_c )
					if( search_advance( p1, s_g_e, 0, 0 ) )
						{
						sea_loc1 = p1;
						return (sea_loc2 - sea_loc1);
						}
					else
						if( ml_err ) return -1;
				p1--;
				}
			while( p1 >= firstchar );
			return -1;
			}

		default:
			break;
			}
		}
#endif

	// regular algorithm
	int numchars = bf_cur->num_characters();
	int firstchar = bf_cur->first_character();

	do
		{
		unsigned char **alt;
		alt = sea_alternatives;

		while( *alt != NULL )
			if( search_advance (p1, *alt, 0, 0) )
				{
				sea_loc1 = p1;
				return sea_loc2 - sea_loc1;
				}
			else
				{
				alt++;
				if( ml_err )
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
int SearchSimpleAlgorithm::search_advance
		(
		int lp,
		unsigned char *ep,
		int syn_include,
		int syn_exclude		
		)
	{
#define SYN_EXCLUDE	(syn_exclude && (b->syntax_at(lp)&syn_exclude) != 0)
#define SYN_INCLUDE	(syn_include && (b->syntax_at(lp)&syn_include) == 0)

	int curlp;
	unsigned char *trtp = sea_trt;
	unsigned char ch;
	int numchars = bf_cur->num_characters();
	int firstchar = bf_cur->first_character();

	EmacsBuffer *b = bf_cur;

/*	_m( 'advance_search at %d, length is %d', .lp, .length ); */
	while( (! ml_err &&
		(lp <= numchars
		|| ((ch = *ep) & STAR) || ch == CKET || ch == EBUF )) )
		{
/*		_m( 'ep = %d, lp = %d char(.lp-1) = %d, char(.lp) = %d',
 *			.ep[0], .lp, .ch_p[ .lp-1 ], .ch_p[ .lp ] ); */

		if( (syn_include != 0 || syn_exclude != 0)
		&& bf_cur->b_syntax.syntax_valid <= lp )
			{
			bf_cur->syntax_fill_in_array( lp + 10000 );
			}

		switch( *ep++ )
			{
		case CCHR:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( trtp[*ep++] != trtp[b->char_at(lp)] )
				return 0;
			lp++;
			break;
		case CDOT:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (b->char_at(lp) == NL) )
				return 0;
			lp++;
			break;
		case CDOL:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (b->char_at(lp) != NL) )
				return 0;
			break;
		case CIRC:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ! (lp <= firstchar || bf_cur->char_at(lp-1) == NL) )
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
			if( bf_cur->char_at_is( lp, SYNTAX_WORD) )
				lp++;
			else
				return 0;
			break;
		case NWORDC:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ( !bf_cur->char_at_is( lp, SYNTAX_WORD)) )
				lp++;
			else
				return 0;
			break;
		case WBOUND:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( !((lp <= firstchar || !bf_cur->char_at_is( lp-1, SYNTAX_WORD)) !=
				(lp > numchars || !bf_cur->char_at_is( lp, SYNTAX_WORD))) )
				return 0;
			break;
		case NWBOUND:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( ! ((lp <= firstchar ||  ! bf_cur->char_at_is (lp-1, SYNTAX_WORD)) ==
				(lp > numchars ||  ! bf_cur->char_at_is (lp, SYNTAX_WORD))) )
				return 0;
			break;
		case SEA_SYN_COMMENT:
			if( !bf_cur->b_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include |= SYNTAX_COMMENT_MASK;
			syn_exclude &= ~SYNTAX_COMMENT_MASK;
			break;
		case SEA_SYN_NOCOMMENT:
			if( !bf_cur->b_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include &= ~SYNTAX_COMMENT_MASK;
			syn_exclude |= SYNTAX_COMMENT_MASK;
			break;
		case SEA_SYN_STRING:
			if( !bf_cur->b_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include |= SYNTAX_STRING_MASK;
			syn_exclude &= ~SYNTAX_STRING_MASK;
			break;
		case SEA_SYN_NOSTRING:
			if( !bf_cur->b_mode.md_syntax_array )
				{
				error(syn_sea_error);
				return 0;
				}
			syn_include &= ~SYNTAX_STRING_MASK;
			syn_exclude |= SYNTAX_STRING_MASK;
			break;
		case CEOP:
			sea_loc2 = lp;
			return 1;
		case CCL:
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;
			if( (cclass( ep, b->char_at(lp), 1 )) )
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
			if( cclass( ep, b->char_at(lp), 0 ) )
				{
				ep = &ep[ *ep ];
				lp++;
				}
			else
				return 0;
			break;
		case CBRA:
			sea_bra_slist[*ep++ ].set_mark( bf_cur, lp, 0 );
			break;
		case CKET:
			sea_bra_elist[*ep++ ].set_mark( bf_cur, lp, 0 );
			break;
		case CBACK:
			ch = *ep++;
			if( sea_bra_elist[ ch ].m_buf == NULL )
				{
				error( "bad braces");
				return 0;
				}
			if( backref(ch, lp) )
				{
				lp += sea_bra_elist[ ch ].to_mark()
					- sea_bra_slist[ ch ].to_mark();
				}
			else
				return 0;
			break;

		case CBACK|STAR:
			ch = *ep++;
			if( sea_bra_elist[ ch ].m_buf == NULL )
				{
				error( "bad braces");
				return 0;
				}
			curlp = lp;
			while( backref( ch, lp ) )
				{
				lp -=	sea_bra_elist[ ch ].to_mark()
					- sea_bra_slist[ ch ].to_mark();
				}
			while( lp >= curlp )
				{
				if( search_advance(lp, ep, syn_include, syn_exclude) )
					return 1;

				lp -=	sea_bra_elist[ ch ].to_mark()
					- sea_bra_slist[ ch ].to_mark();
				}
			break;

		case CDOT|STAR:
			curlp = lp;
			while( lp <= numchars
			&& bf_cur->char_at(lp) != NL )
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
			&& bf_cur->char_at_is(lp, SYNTAX_WORD) )
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
			&&  ! bf_cur->char_at_is( lp, SYNTAX_WORD) )
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
			&& trtp[bf_cur->char_at(lp)] == ch )
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
			&& cclass( ep, bf_cur->char_at(lp), ch ) )
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
			error( FormatString("Badly compiled pattern \"%s\" (Emacs internal error!)") <<
					last_search_string.asString() );
			return 0;
			}
		}

	if(( (ch = ep[0]) == CEOP || ch == CDOL ) )
		{
		sea_loc2 = lp;
		return 1;
		}
	return 0;
#undef SYN_EXCLUDE
#undef SYN_INCLUDE
	}



int SearchSimpleAlgorithm::backref(int i, int lp)
	{
	int bp; int ep;

	bp = sea_bra_slist[ i ].to_mark();
	ep = sea_bra_elist[ i ].to_mark();
	while( lp <= bf_cur->num_characters() && bf_cur->char_at(bp) == bf_cur->char_at(lp) )
		{
		bp++;
		lp++;
		if( bp >= ep )
			return 1;
		}

	return 0;
	}



int SearchSimpleAlgorithm::cclass( unsigned char *char_set, int c, int af )
	{
	int n;
	unsigned char *trtp = sea_trt;

	n = *char_set++;
	while( (n = n - 1) != 0 )
		if( trtp[*char_set++] == trtp[c] )
			return af;
	return ! af;
	}
