//
//			Copyright (c) 1985-1993
//		Barry A. Scott and Nick Emery
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#define NL '\n'		/* Newlines character */

#if 0
#define SEA_DEBUG( expr ) do {_dbg_msg( expr ); } while(0)
#else
#define SEA_DEBUG( expr ) do { } while(0)
#endif

int SearchGlobals::search_execute(int fflag, int addr)
	{
	int p1 = addr;
	int i;
	int inc = fflag ? 1 : -1;

	for( i=0; i<=SearchGlobals::MAX_NBRA-1; i++ )
		{
		sea_bra_slist[ i ].unset_mark();
		sea_bra_elist[ i ].unset_mark();
		}
	if( addr == 0 )
		return -1;

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


static bool test_CCHR( SearchGlobals *sea, unsigned char *ep, int lp )
	{
	unsigned char *trtp = sea->sea_trt;

	return trtp[bf_cur->char_at(lp)] == trtp[*ep];
	}

static unsigned char * next_CCHR( unsigned char *ep )
	{
	ep++;
	return ep;
	}

static bool test_CDOT( SearchGlobals *, unsigned char *, int lp )
	{
	return bf_cur->char_at(lp) != NL;
	}

static unsigned char * next_CDOT( unsigned char *ep )
	{
	return ep;
	}

static bool test_CCL( SearchGlobals *sea, unsigned char *ep, int lp )
	{
	return sea->cclass( ep, bf_cur->char_at(lp) );
	}

static unsigned char * next_CCL( unsigned char *ep )
	{
	return &ep[ *ep ];
	}

static bool test_NCCL( SearchGlobals *sea, unsigned char *ep, int lp )
	{
	return !sea->cclass( ep, bf_cur->char_at(lp) );
	}

static unsigned char * next_NCCL( unsigned char *ep )
	{
	return &ep[ *ep ];
	}


static bool test_WORDC( SearchGlobals *, unsigned char *, int lp )
	{
	return bf_cur->char_at_is(lp, SYNTAX_WORD);
	}

static unsigned char * next_WORDC( unsigned char *ep )
	{
	return ep;
	}

static bool test_NWORDC( SearchGlobals *, unsigned char *, int lp )
	{
	return !bf_cur->char_at_is(lp, SYNTAX_WORD);
	}

static unsigned char * next_NWORDC( unsigned char *ep )
	{
	return ep;
	}


struct search_info_t
	{
	bool (*test_expr)( SearchGlobals *sea, unsigned char *ep, int lp );
	unsigned char * (*next_expr)( unsigned char *ep );
	}
		search_info[] =
{
	{ NULL, NULL },				// CEOP slot
	{ test_CCHR, next_CCHR },		// [R] a vanilla character 
	{ test_CDOT, next_CDOT },		// [R] . -- match anything except a newline 
	{ test_CCL, next_CCL },			// [R] [...] -- character class 
	{ test_NCCL, next_NCCL },		// [R] [^...] -- negated character class 
	{ NULL, NULL },				// [R] \N -- backreference to the Nth bracketed string 
	{ test_WORDC, next_WORDC },		// [R] matches word character \w 
	{ test_NWORDC, next_NWORDC }		// [R] matches non-word characer \W 
};

static unsigned char *syn_sea_error = u_str("search requires syntax-array to be enabled");

/* search_advance the match of the regular expression starting at ep along the
 *  string lp, simulates an NDFSA */
int SearchGlobals::search_advance
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
	int numchars = bf_cur->num_characters();
	int firstchar = bf_cur->first_character();

	EmacsBuffer *b = bf_cur;

/*	_m( 'advance_search at %d, length is %d', .lp, .length ); */
	while( !ml_err && lp <= numchars )
		{
/*		_m( 'ep = %d, lp = %d char(.lp-1) = %d, char(.lp) = %d',
 *			.ep[0], .lp, .ch_p[ .lp-1 ], .ch_p[ .lp ] ); */

		if( (syn_include != 0 || syn_exclude != 0)
		&& bf_cur->b_syntax.syntax_valid <= lp )
			{
			bf_cur->syntax_fill_in_array( lp + 10000 );
			}


		search_meta_chars command = (search_meta_chars)(*ep++);

		SEA_DEBUG( FormatString( "search_advance: cmd=%d lp=%d (%c)" ) << command << lp << bf_cur->char_at(lp) );

		switch( command )
			{
		case CCHR:
		case CDOT:
		case CCL:
		case NCCL:
		case WORDC:
		case NWORDC:
			{
			if( SYN_EXCLUDE || SYN_INCLUDE )
				return 0;

			bool (*test_function)( SearchGlobals *sea, unsigned char *ep, int lp ) = search_info[command].test_expr;
			unsigned char * (*next_function)( unsigned char *ep ) = search_info[command].next_expr;

			int range_min = *ep++;
			int range_max = *ep++;
			int matches = 0;

			curlp = lp;

			SEA_DEBUG( FormatString( "search_advance: range={%d,%d}" ) << range_min << range_max );


			while( lp <= numchars
			&& !SYN_EXCLUDE && !SYN_INCLUDE
			&& test_function( this, ep, lp ) )
				{
				matches++;
				lp++;

				// see if we have hit the max limit on matching
				if( range_max != 0 && matches == range_max )
					break;
				}

			SEA_DEBUG( FormatString( "search_advance: range={%d,%d} matches=%d" ) << range_min << range_max << matches );
			ep = next_function( ep );

			// check the minimum number of matches has been achieved
			if( matches < range_min )
				return 0;

			if( range_max != 0 )
				break;	// its a match - drop to end

			lp++;
			do
				{
				lp--;
				if( search_advance( lp, ep, syn_include, syn_exclude ) )
					return 1;
				}
			while( lp > curlp );
			return 0;
			}

		case CBACK:
			{
			int range_min = *ep++;
			int range_max = *ep++;
			int matches = 0;

			int back_ref = *ep++;

			if( sea_bra_elist[ back_ref ].m_buf == NULL )
				{
				error( "bad braces");
				return 0;
				}

			curlp = lp;
			while( backref( back_ref, lp ) )
				{
				matches++;
				lp += sea_bra_elist[ back_ref ].to_mark() - sea_bra_slist[ back_ref ].to_mark();

				// see if we have hit the max limit on matching
				if( range_max != 0 && matches == range_max )
					break;
				}

			// check the minimum number of matches has been achieved
			if( matches < range_min )
				return 0;

			if( range_max != 0 )
				break;	// its a match - drop to end

			while( lp >= curlp )
				{
				if( search_advance(lp, ep, syn_include, syn_exclude) )
					return 1;

				lp -=	sea_bra_elist[ back_ref ].to_mark() - sea_bra_slist[ back_ref ].to_mark();
				}
			}
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

		case CBRA:
			sea_bra_slist[*ep++ ].set_mark( bf_cur, lp, 0 );
			break;

		case CKET:
			sea_bra_elist[*ep++ ].set_mark( bf_cur, lp, 0 );
			break;

		default:
			error( FormatString("Badly compiled pattern \"%s\" (Emacs internal error!) command=%d") <<
					last_search_string.asString() << command );
			return 0;
			}
		}

	if( *ep == CEOP || *ep == CDOL )
		{
		sea_loc2 = lp;
		return 1;
		}
	return 0;

#undef SYN_EXCLUDE
#undef SYN_INCLUDE
	}



int SearchGlobals::backref(int i, int lp)
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



bool SearchGlobals::cclass( unsigned char *char_set, int c )
	{
	int n;
	unsigned char *trtp = sea_trt;

	n = *char_set++;
	while( (n = n - 1) != 0 )
		if( trtp[*char_set++] == trtp[c] )
			return true;
	return false;
	}


#if 0
// optimising search - but its not always right


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

int SearchGlobals::search_execute(int fflag, int addr)
	{
	int p1 = addr;
	int i;
	int inc = fflag ? 1 : -1;

	for( i=0; i<=SearchGlobals::MAX_NBRA-1; i++ )
		{
		sea_bra_slist[ i ].unset_mark();
		sea_bra_elist[ i ].unset_mark();
		}
	if( addr == 0 )
		return (-1);

	/* skip over any express elements that
	   have no effect on the matching */
	unsigned char *exp = sea_expbuf;
	while( search_exp_info[*exp].match_width == 0 )
		exp += search_exp_info[*exp].compile_size;

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

#endif
