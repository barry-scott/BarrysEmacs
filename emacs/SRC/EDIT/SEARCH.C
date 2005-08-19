/*
 *	Copyright (c) 1982-1994
 *		Barry A. Scott and nick Emery */

/* string search routines */

#include <emacs.h>


/*forward*/ int search( unsigned char *s, int n, int dot, int RE );
/*forward*/ int looking_at( void );
/*forward*/ int search_reverse( void );
/*forward*/ int search_forward( void );
/*forward*/ int replace_string_command( void );
/*forward*/ int query_replace_string( void );
/*forward*/ int re_search_reverse( void );
/*forward*/ int re_search_forward( void );
/*forward*/ int re_replace_string( void );
/*forward*/ int re_query_replace_string( void );
/*forward*/ static int perform_replace(int query, int RE);
/*forward*/ void search_replace_once( unsigned char *replace, int RE );
/*forward*/ int region_around_match_command( void );
/*forward*/ int region_around_match( int n );
/*forward*/ int quote_command( void );
/*forward*/ int char_compare_command( void );
/*forward*/ void init_srch( void );
/*forward*/ static void compile( unsigned char *strp, int RE );
/*forward*/ static void place(int l1, int l2);

static unsigned char standard_trt[256];	/* the identity TRT */
static unsigned char case_fold_trt[256];	/* folds upper to lower case */

GLOBAL SAVRES struct search_globals sea_glob;
	/* If true then replace and query-replace will
	 * modify the case conventions of the new
	 * string to match those of the old. */
GLOBAL SAVRES int replace_case;
GLOBAL SAVRES int default_replace;
	/* the last search string */
GLOBAL SAVRES unsigned char *last_search_string;

extern int search_execute(int fflag, int addr);
extern int search_advance( int lp, unsigned char *ep, int include, int exclude );


/*
 * search for the n th occurrence of string s in the current buffer,
 * starting at dot, leaving dot at the end (if forward) or beginning
 * (if reverse) of the found string returns. true or false
 * depending on whether or not the string was found
 */
int search
	(
	unsigned char *s,
	int n,
	int dot,
	int RE
	)
	{
	int pos;
	int matl;
	unsigned char *trt;

	pos = dot;
	trt = sea_glob.sea_trt = bf_mode.md_foldcase ? case_fold_trt : standard_trt;
	if( s == 0 )
		return -1;
	compile( s, RE );
	while( ! err && n != 0 )
		if( n < 0 )
			{
			if( pos <= first_character )
				return 0;
			if( (matl = search_execute( 0, pos - 1)) < 0 )
				return 0;
			n++;
			pos = sea_glob.sea_loc1;
			}
		else
			{
			if( pos > num_characters )
				return 0;
			if( (matl = search_execute( 1, pos )) < 0 )
				return 0;
			n--;
			pos = sea_glob.sea_loc1 + matl;
			}
	if( err )
		return -1;
	else
		return pos;
	}

int looking_at( void )
	{
	/*
	 *	(looking-at str) is true iff we are
	 *	currently looking at the given RE
	 */

	unsigned char **alt;
	unsigned char *s;

	s = getstr(u_str(": looking-at "));
	alt = sea_glob.sea_alternatives;

	sea_glob.sea_trt= ( bf_mode.md_foldcase ) ?  case_fold_trt : standard_trt;
	if( s == 0 )
		return 0;
	compile( s, 1 );
	release_expr( ml_value );
	ml_value->exp_int = 0;
	ml_value->exp_type = ISINTEGER;
	if( arg < 0 )
		{
		int resp;
		resp = search_execute( 0, dot );
		ml_value->exp_int = resp + sea_glob.sea_loc1 == dot;
		}
	else
		{
		while( *alt != 0 && ! err )
			{
			if( (ml_value->exp_int = search_advance( dot, *alt, 0, 0 )) != 0 )
				break;
			alt++;
			}
		sea_glob.sea_loc1 = dot;
		}
	return 0;
	}


int search_reverse( void )
	{
	int np;
	if( arg <= 0 )
		arg = 1;
	np = search( getstr(u_str("reverse search for: ")), -arg, dot, 0 );
	if( np == 0 && ! err )
		error( u_str("Cannot find \"%s\""), last_search_string );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int search_forward( void )
	{
	int np;
	if( arg <= 0 )
		arg = 1;
	np = search( getstr(u_str("search for: ")), arg, dot, 0 );
	if( np == 0 && ! err )
		error( u_str("Cannot find \"%s\""), last_search_string );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int replace_string_command( void )
	{
	perform_replace( 0, 0 );
	return 0;
	}

int query_replace_string( void )
	{
	perform_replace( 1, 0 );
	return 0;
	}


int re_search_reverse( void )
	{
	int np;
	if( arg <= 0 )
		arg = 1;
	np = search( getstr(u_str("Reverse RE search for: ")), -arg, dot, 1 );
	if( np == 0 && ! err )
		error( u_str("Cannot find \"%s\""), last_search_string );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_search_forward( void )
	{
	int np;
	if( arg <= 0 )
		arg = 1;
	np = search( getstr(u_str("RE search for: ")), arg, dot, 1 );
	if( np == 0 && ! err )
		error( u_str("Cannot find \"%s\""), last_search_string );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_replace_string( void )
	{
	perform_replace( 0, 1 );
	return 0;
	}

int re_query_replace_string( void )
	{
	perform_replace( 1, 1 );
	return 0;
	}


static int perform_replace(int query, int RE)
	{
	/*
	 * perform either a query replace or a normal replace
	 */
	unsigned char *old;
	unsigned char *temp_new;
	unsigned char new_string[1000];
	int np;
	int comma;
	unsigned char c;
	int len;
	int replaced;
	int olddot;

	old = getstr( u_str("Old %s: "),
		(( RE ) ?  u_str("pattern") : u_str("string")) );
	comma = 0;
	replaced = 0;
	olddot = dot;

	if( old == 0
	|| (compile( old, RE ), err)
	|| (temp_new = getstr(u_str("new string: "))) == 0 )
		return 0;
	len = min( sizeof( new_string ) - 1, _str_len( temp_new ));
	memcpy( new_string, temp_new, len );
	new_string[len] = 0;
	if( query )
		message( u_str("Query-Replace mode") );

	do
	{
	np = search( u_str(""), 1, dot, RE );
	if( np <= 0 )
		break;
	set_dot( np );
	comma = 0;
	do
		{
		switch( (c = (unsigned char)( query ? get_char() : ' ')) )
		{
		case ' ':
		case '!':
		case '.':
		case ',':
			if (!comma)
			    {
			    search_replace_once( new_string, RE );
			    replaced++;
			    }
			if( c == '!' )
				query = 0;
			if( c == '.' )
				c = ctl( 'G' );
			break;
		case '\033':
			c = ctl( 'G' );
			break;
		case 'n':
		case '\177':
			if( sea_glob.sea_loc1 == sea_glob.sea_loc2 )
				dot_right( 1 );
			break;
		case ctl( 'G' ):
			break;
		case 'r':
			{
			struct marker *m;
			m = new_mark();
			set_mark( m, bf_cur, sea_glob.sea_loc1, 0 );

			message( u_str("Invoke exit-emacs to resume query-replace") );

			save_excursion_inner( recursive_edit );

			set_dot( to_mark( m ) );
			dest_mark( m );
			window_on( bf_cur );

			message( u_str("Continuing with query-replace...") );
			}
			break;
		default:
			{
			message( u_str("Options: ' ' ','=>change; 'n'=>don't; '.'=>change, quit; '^G'=>quit; 'r'=>edit") );
			c = '?';
			}
		}

		if( c == ',' )
			comma = 1;
		}
	while( c == '?' || c == ',' );
	}
	while( c != ctl( 'G' ) );

	set_dot( olddot );

	if( replaced != 0 )
		{
		if( interactive )
			{
			void_result();
			message( u_str("Replaced %d occurrences"), replaced );
			cant_1line_opt = 1;
			}
		else
			{
			release_expr( ml_value );
			ml_value->exp_int = replaced;
			ml_value->exp_type = ISINTEGER;
			}
		}
	else
		error( u_str("No replacements done for \"%s\""), last_search_string );

	return 0;
	}

static int search_replace_helper( int RE )
	{
	unsigned char *replace_text = getstr( u_str("replace with: ") );
	if( replace_text != NULL )
		search_replace_once( replace_text, RE );
	return 0;
	}

int replace_search_text_command()
	{
	return search_replace_helper( 0 );
	}

int re_replace_search_text_command()
	{
	return search_replace_helper( 1 );
	}

void search_replace_once( unsigned char *new_string, int RE )
	{
	enum rep_actions
	    {
	    DO_NOTHING, UPPER, FIRST, FIRST_ALL
	    }
		action = DO_NOTHING;

	struct flag_bits
	{
	int beg_of_str : 1;
	int beg_of_word : 1;
	int prefix : 1;
	int last_prefix : 1;
	} flags;

	unsigned char *p;
	unsigned char lc;

	if( replace_case )
		{
		int i;

		flags.beg_of_str = 1;
		i = sea_glob.sea_loc1;
		flags.beg_of_word = i <= first_character
			|| ! isalpha( char_at(i - 1) );
		while( i < sea_glob.sea_loc2 )
			{
			if( isalpha( lc = char_at( i ) ) )
				{
				if( isupper( lc ) )
					{
					if( flags.beg_of_str )
						action = FIRST;
					else
						if( flags.beg_of_word && action != UPPER )
							action = FIRST_ALL;
						else
							action = UPPER;
					}
				else
					if( action == UPPER
					|| (action == FIRST_ALL
					&& flags.beg_of_word) )
						{
						action = DO_NOTHING;
						break;
						}
				flags.beg_of_str = 0;
				flags.beg_of_word = 0;
				}
			else
				flags.beg_of_word = 1;
			i++;
			}
		}

	flags.beg_of_str = 1;
	flags.prefix = 0;
	flags.beg_of_word = dot <= first_character
		|| ! isalpha( char_at( dot - 1 ) );
	p = new_string;
	while( (lc = *p++) != 0 )
		{
		flags.last_prefix = flags.prefix;
		flags.prefix = 0;
		if( action != DO_NOTHING
		&& isalpha( lc ) )
			{
			if( islower( lc )
			&&	(action == UPPER
				|| (action == FIRST_ALL
					&& flags.beg_of_word)
				|| (action == FIRST
					&& flags.beg_of_str)) )
					lc = (unsigned char)toupper( lc );
			flags.beg_of_word = 0;
			flags.beg_of_str = 0;
			}
		else
			flags.beg_of_word = 1;
		if( lc == 92 && RE && !flags.last_prefix )
			flags.prefix = 1;
		else if( lc == '&' && RE && ! flags.last_prefix )
			place( sea_glob.sea_loc1, sea_glob.sea_loc2 );
		else if( flags.last_prefix
		&& lc >= '1'
		&& lc < (unsigned char)(sea_glob.sea_nbra + '1') )
			place
			(
			to_mark( sea_glob.sea_bra_slist[ lc - '1' ] ),
			to_mark( sea_glob.sea_bra_elist[ lc - '1' ] )
			);
		else
			{
			insert_at( dot, lc );
			dot_right( 1 );
			}
		}

	if( sea_glob.sea_loc1 == sea_glob.sea_loc2 )
		dot_right( 1 );
	else
		{
		dot_left( sea_glob.sea_loc2 - sea_glob.sea_loc1 );
		del_back( sea_glob.sea_loc2, sea_glob.sea_loc2 - sea_glob.sea_loc1 );
		}
	}



/* put dot and mark around the region matched by the n th parenthesised
 * expression from the last search( n=0 = gtr the whole thing) */
int region_around_match_command( void )
	{
	int n;

	n = getnum( u_str(": region-around-match ") );
	if( err )
		return 0;
	region_around_match( n );
	return 0;
	}
	
int region_around_match( int n )
	{
	int lo;
	int hi;

	if( n < 0 || n > (int)sea_glob.sea_nbra )
		{
		error( u_str("Out-of-bounds argument to region-around-match") );
		return 0;
		}
	if( n == 0 )
		{
		lo = sea_glob.sea_loc1;
		hi = sea_glob.sea_loc2;
		}
	else
		{
		lo = to_mark( sea_glob.sea_bra_slist[ n - 1 ] );
		hi = to_mark( sea_glob.sea_bra_elist[ n - 1 ] );
		}
	set_dot( lo );
	set_mark_command();
	set_dot( hi );
	return 0;
	}

/* Quote a string to inactivate reg-expr chars */
int quote_command( void )
	{
	unsigned char *p;
	unsigned char *cp;
	unsigned char *s;
	unsigned char ch;
	int size;

	s = getstr( u_str(": quote ") );

	if( s == 0 )
		return 0;
	size = _str_len( s );

	cp = s;
	while( (ch = *cp++) != 0 )
		if( ch == '['
		|| ch == ']'
		|| ch == '*'
		|| ch == '.'
		|| ch == 92
		|| (ch == '^' && &cp[-1] == s)
		|| (ch == '$' && cp[0] == 0) )
			size++;
	p = malloc_ustr( size + 1 );
	cp = p;
	while( (ch = *s++) != 0 )
		{
		if( ch == '['
		|| ch == ']'
		|| ch == '*'
		|| ch == '.'
		|| ch == 92
		|| (ch == '^' && &cp[-1] == p)
		|| (ch == '$' && s[0] == 0) )
			*cp++ = 92;
		*cp++ = ch;
		}

	cp[0] = 0;
	release_expr( ml_value );
	ml_value->exp_type = ISSTRING;
	ml_value->exp_int = size;
	ml_value->exp_release = 1;
	ml_value->exp_v.v_string = p;
	return 0;
	}

/*
 * Compare two chars according to case-fold
 */
int char_compare_command( void )
	{
	unsigned char *trt;
	int a; int b;

	a = bin_setup();
	b = numeric_arg( 2 );

	if( a > 256 || b > 256 )
		{
		error( u_str( "c= expects its arguments to be character values between 0 and 255" ) );
		return 0;
		}

	trt = sea_glob.sea_trt = bf_mode.md_foldcase ? case_fold_trt : standard_trt;
	release_expr( ml_value );
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = (trt[a] == trt[b]);
	return 0;
	}


void init_srch( void )
	{
	unsigned i;

	/* initialize the search package, mostly just sets up translation tables */
	for( i=0; i<=255; i += 1 )
		standard_trt[i] = case_fold_trt[i] = (unsigned char)i;
	for( i='A'; i<='Z'; i += 1 )
		case_fold_trt[i] = (unsigned char)(i + ( 'a' - 'A' ));

	last_search_string = malloc_ustr( 1 );
	last_search_string[0] = 0;
	}


/* Compile the given regular expression into a[secret] internal format */
static void compile
	(
	unsigned char *strp,
	int RE
	)
	{
	unsigned char c;
	unsigned char *ep;
	unsigned char *lastep;
	unsigned char bracket[MAX_NBRA];
	unsigned char *bracketp;
	int cclcnt;
	unsigned char **alt;

	alt = sea_glob.sea_alternatives;

	ep = sea_glob.sea_expbuf;
	*alt++ = ep;
	bracketp = &bracket[0];
	if( strp[0] == 0 )
		{
		if( ep[0] == 0 )
			error( u_str("null search string") );
		return;
		}
	sea_glob.sea_nbra = 0;
	if( last_search_string != 0 )
		free( last_search_string );
	last_search_string = savestr( strp );
	lastep = 0;
	{	/* leave this block to return a compile error */
	for(;;)
		{
		if( ep >= &sea_glob.sea_expbuf[ESIZE] )
			goto cerror;
		c = *strp++;
		if( c == 0 )
			{
			if( bracketp != &bracket[0] )
				goto cerror;
			*ep++ = CEOP;
			*alt++ = 0;
			return;
			}
		if( c != '*' )
			lastep = ep;
		if( ! RE )
			{
			*ep++ = CCHR;
			*ep++ = c;
			}
		else
			switch( c)
			{
			case 92:
				switch( c = *strp++ )
				{
				case '(':
					if( sea_glob.sea_nbra >= MAX_NBRA )
						goto cerror;
					*bracketp++ = sea_glob.sea_nbra;
					*ep++ = CBRA;
					*ep++ = sea_glob.sea_nbra;
					sea_glob.sea_nbra++;
					break;
				case '|':
					if( bracketp > &bracket[0] )
						goto cerror;
					/* Alas! */
					*ep++ = CEOP;
					*alt++ = ep;
					break;
				case ')':
					if( bracketp <= &bracket[0] )
						goto cerror;
					*ep++ = CKET;
					bracketp = &bracketp[-1];
					*ep++ = bracketp[0];
					break;
				case '<':	*ep++ = BDOT; break;
				case '=':	*ep++ = EDOT; break;
				case '>':	*ep++ = ADOT; break;
				case '`':	*ep++ = BBUF; break;
				case 39:	*ep++ = EBUF; break;
				case 'w':	*ep++ = WORDC; break;
				case 'W':	*ep++ = NWORDC; break;
				case 'b':	*ep++ = WBOUND; break;
				case 'B':	*ep++ = NWBOUND; break;
				case 'c':	*ep++ = SEA_SYN_COMMENT; break;
				case 'C':	*ep++ = SEA_SYN_NOCOMMENT; break;
				case 's':	*ep++ = SEA_SYN_STRING; break;
				case 'S':	*ep++ = SEA_SYN_NOSTRING; break;
				case '1': case '2': case '3': case '4': case '5':
					*ep++ = CBACK;
					*ep++ = (unsigned char)(c - '1');
					break;
				default:
					*ep++ = CCHR;
					if( c == 0 )
						goto cerror;
					*ep++ = c;
					break;
				}
				break;
			case '.':
				*ep++ = CDOT; break;
			case '*':
				if( lastep == 0
				|| lastep[0] == CBRA
				|| lastep[0] == CKET
				|| lastep[0] == CIRC
				|| (BBUF <= lastep[0] && lastep[0] <= ADOT)
				|| (lastep[0] & STAR)
				|| lastep[0] > NWORDC )
					{
					*ep++ = CCHR;
					*ep++ = c;
					}
				else
					lastep[0] |= STAR;
				break;
			case '^':
				if( ep != &sea_glob.sea_expbuf[0]
				&& ep[-1] != CEOP )
					{
					*ep++ = CCHR;
					*ep++ = c;
					}
				else
					*ep++ = CIRC;
				break;
			case '$':
				if( strp[0] != 0
				&& (strp[0] != 92 || strp[1] != '|' ) )
					{
					*ep++ = CCHR;
					*ep++ = c;
					}
				else
					*ep++ = CDOL;
				break;
			case '[':
				*ep++ = CCL;
				*ep++ = 0;
				cclcnt = 1;
				if(( c = *strp++ ) == '^' )
					{
					c = *strp++;
					ep[-2] = NCCL;
					}
				do
					{
					if( c == '\0' )
						goto cerror;
					if( c == '-' && ep[-1] != 0 )
						{
						if(( c = *strp++ ) == ']' )
							{
							*ep++ = '-';
							cclcnt++;
							break;
							}
						while( ep[-1] < c )
							{
							/* Ridiculous
							  This should
							  be reflected
							  in the
							  compiled
							  form */
							ep[0] = (unsigned char)(ep[-1] + 1);
							ep++;
							cclcnt++;
							if( ep >= &sea_glob.sea_expbuf[ESIZE ] )
								goto cerror;
							}
						}
					*ep++ = c;
					cclcnt++;
					if( ep >= &sea_glob.sea_expbuf[ESIZE ] )
						goto cerror;
					}
				while( (c = *strp++) != ']' );

				lastep[1] = (unsigned char)cclcnt;
				break;

			default:
				{
				*ep++ = CCHR;
				*ep++ = c;
				}
			}
		}
	}
cerror:
	sea_glob.sea_expbuf[0 ] = 0;
	sea_glob.sea_nbra = 0;
	error( u_str("Badly formed search string \"%s\""), last_search_string );
	}


static void place(int l1, int l2)
	{
	while( l1 < l2 )
		{
		insert_at( dot, char_at( l1 ) );
		dot_right( 1 );
		l1++;
		}
	}
void fetch_last_search_string( struct expression *e )
	{
	release_expr( e );
	e->exp_type = ISSTRING;
	e->exp_int = _str_len( last_search_string );
	e->exp_v.v_string = savestr( last_search_string );
	e->exp_release = 1;
	}
