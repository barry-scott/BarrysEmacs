//
//	Copyright (c) 1982-2002
//		Barry A. Scott
//

// string search routines

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );



SearchGlobals sea_glob;
unsigned char SearchGlobals::standard_trt[256];		/* the identity TRT */
unsigned char SearchGlobals::case_fold_trt[256];	/* folds upper to lower case */

/* If true then replace and query-replace will
 * modify the case conventions of the new
 * string to match those of the old. */
SystemExpressionRepresentationIntBoolean replace_case;
SystemExpressionRepresentationIntBoolean default_replace;

/* the last search string */
SystemExpressionRepresentationStringReadOnly last_search_string;

//
//
//	SearchGlobals implementation
//
//
SearchGlobals::SearchGlobals()
	: sea_loc1(0)
	, sea_loc2(0)
	, sea_nbra(0)
	, sea_trt(NULL)
	{ }

SearchGlobals::SearchGlobals( const SearchGlobals &src )
	: sea_loc1( src.sea_loc1 )
	, sea_loc2( src.sea_loc2 )
	, sea_nbra( src.sea_nbra)
	, sea_trt( src.sea_trt )
	{
	memcpy( sea_expbuf, src.sea_expbuf, sizeof( sea_expbuf ) );

	for( int i=0; i<MAX_NALTS; i++ )
		{
		if( src.sea_alternatives[i] != NULL )
			sea_alternatives[i] = &sea_expbuf[ src.sea_alternatives[i] - src.sea_expbuf ];
		else
			sea_alternatives[i] = NULL;
		}

	for( int j=0; j<MAX_NBRA; j++ )
		{
		sea_bra_slist[j] = src.sea_bra_slist[j];
		sea_bra_elist[j] = src.sea_bra_elist[j];
		}
	}

SearchGlobals::~SearchGlobals()
	{ }

SearchGlobals &SearchGlobals::operator=( const SearchGlobals &src )
	{
	sea_loc1 = src.sea_loc1;
	sea_loc2 = src.sea_loc2;
	sea_nbra = src.sea_nbra;
	sea_trt = src.sea_trt;

	memcpy( sea_expbuf, src.sea_expbuf, sizeof( sea_expbuf ) );

	for( int i=0; i<MAX_NALTS; i++ )
		{
		if( src.sea_alternatives[i] != NULL )
			sea_alternatives[i] = &sea_expbuf[ src.sea_alternatives[i] - src.sea_expbuf ];
		else
			sea_alternatives[i] = NULL;
		}

	for( int j=0; j<MAX_NBRA; j++ )
		{
		sea_bra_slist[j] = src.sea_bra_slist[j];
		sea_bra_elist[j] = src.sea_bra_elist[j];
		}

	return *this;
	}

/*
 * search for the n'th occurrence of string s in the current buffer,
 * starting at dot, leaving dot at the end (if forward) or beginning
 * (if reverse) of the found string returns. true or false
 * depending on whether or not the string was found
 */
int SearchGlobals::search( const EmacsString &s, int n, int dot, SearchGlobals::sea_type RE )
	{
	int pos = dot;
	sea_trt = bf_cur->b_mode.md_foldcase ? case_fold_trt : standard_trt;

	compile( s, RE );

	while( ! err && n != 0 )
		if( n < 0 )
			{
			if( pos <= bf_cur->first_character() )
				return 0;
			int matl = search_execute( 0, pos - 1);
			if( matl < 0 )
				return 0;
			n++;
			pos = sea_loc1;
			}
		else
			{
			if( pos > bf_cur->num_characters() )
				return 0;
			int matl = search_execute( 1, pos );
			if( matl < 0 )
				return 0;
			n--;
			pos = sea_loc1 + matl;
			}
	if( err )
		return -1;
	else
		return pos;
	}

int looking_at_helper( SearchGlobals::sea_type RE, const char *prompt )
	{
	/*
	 *	(looking-at str) is true iff we are
	 *	currently looking at the given RE
	 */

	EmacsString s;

	unsigned char **alt = sea_glob.sea_alternatives;

	sea_glob.sea_trt= bf_cur->b_mode.md_foldcase ? SearchGlobals::case_fold_trt : SearchGlobals::standard_trt;

	s = getstr( prompt );
	sea_glob.compile( s, RE );

	if( arg < 0 )
		{
		int resp;
		resp = sea_glob.search_execute( 0, dot-1 );
		ml_value = resp + sea_glob.sea_loc1 == dot;
		}
	else
		{
		while( *alt != 0 && ! err )
			{
			ml_value = sea_glob.search_advance( dot, *alt, 0, 0 );
			if( ml_value.asInt() != 0 )
				break;
			alt++;
			}
		sea_glob.sea_loc1 = dot;
		}

	return 0;
	}


int looking_at( void )
	{
	return looking_at_helper( SearchGlobals::sea_type__RE_simple, ": looking-at " );
	}

int advanced_looking_at( void )
	{
	return looking_at_helper( SearchGlobals::sea_type__RE_advanced, ": advanced-looking-at " );
	}

int search_reverse( void )
	{
	EmacsString str;
	int np;
	if( arg <= 0 )
		arg = 1;
	str = getstr("Reverse search for: ");
	np = sea_glob.search( str, -arg, dot, SearchGlobals::sea_type__string );
	if( np == 0 && ! err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int search_forward( void )
	{
	EmacsString str;
	int np;
	if( arg <= 0 )
		arg = 1;
	str = getstr("Search for: ");
	np = sea_glob.search( str, arg, dot, SearchGlobals::sea_type__string );
	if( np == 0 && ! err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int replace_string_command( void )
	{
	sea_glob.perform_replace( 0, SearchGlobals::sea_type__string );
	return 0;
	}

int query_replace_string( void )
	{
	sea_glob.perform_replace( 1, SearchGlobals::sea_type__string );
	return 0;
	}


int re_search_reverse_helper( SearchGlobals::sea_type RE )
	{
	int np;
	EmacsString str;
	if( arg <= 0 )
		arg = 1;
	str = getstr("Reverse RE search for: ");
	np = sea_glob.search( str, -arg, dot, RE );
	if( np == 0 && ! err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_search_reverse( void )
	{
	return re_search_reverse_helper( SearchGlobals::sea_type__RE_simple );
	}

int re_advanced_search_reverse( void )
	{
	return re_search_reverse_helper( SearchGlobals::sea_type__RE_advanced );
	}


int re_search_forward_helper( SearchGlobals::sea_type RE )
	{
	EmacsString str;
	int np;
	if( arg <= 0 )
		arg = 1;
	str = getstr("RE search for: ");
	np = sea_glob.search( str, arg, dot, RE );
	if( np == 0 && ! err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_search_forward( void )
	{
	return re_search_forward_helper( SearchGlobals::sea_type__RE_simple );
	}

int re_advanced_search_forward( void )
	{
	return re_search_forward_helper( SearchGlobals::sea_type__RE_advanced );
	}


int re_replace_string( void )
	{
	sea_glob.perform_replace( 0, SearchGlobals::sea_type__RE_simple);
	return 0;
	}

int re_advanced_replace_string( void )
	{
	sea_glob.perform_replace( 0, SearchGlobals::sea_type__RE_advanced );
	return 0;
	}

int re_query_replace_string( void )
	{
	sea_glob.perform_replace( 1, SearchGlobals::sea_type__RE_simple );
	return 0;
	}

int re_advanced_query_replace_string( void )
	{
	sea_glob.perform_replace( 1, SearchGlobals::sea_type__RE_advanced );
	return 0;
	}

int SearchGlobals::perform_replace( int query, SearchGlobals::sea_type RE )
	{
	//
	// perform either a query replace or a normal replace
	//
	int np;
	unsigned char c;

	int comma = 0;
	int replaced = 0;
	int olddot = dot;

	EmacsString old( FormatString("Old %s: ") << (RE == SearchGlobals::sea_type__string ? "string" : "pattern") );
	old = getstr( old );

	compile( old, RE );

	EmacsString new_string;
	if( err )
		return 0;

	new_string = getstr("New string: ");

	do
		{
		if( query )
			message( FormatString("Query-Replace \"%s\" with \"%s\"") << old << new_string );

		np = search( EmacsString::null, 1, dot, RE );
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
				if( sea_loc1 == sea_loc2 )
					dot_right( 1 );
				break;
			case ctl( 'G' ):
				break;
			case 'r':
				{
				Marker m( bf_cur, sea_loc1, 0 );

				message( "Invoke exit-emacs to resume query-replace" );

				save_excursion_inner( recursive_edit );

				set_dot( m.to_mark() );

				theActiveView->window_on( bf_cur );

				message( "Continuing with query-replace..." );
				}
				break;
			default:
				{
				message( "Help: ' ' ',' change; 'n' don't; '.'change, quit; '!' change-all; '^G' quit; 'r' edit" );
				c = '?';
				}
				break;
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
		if( interactive() )
			{
			void_result();
			message( FormatString("Replaced %d occurrences") << replaced );
			cant_1line_opt = 1;
			}
		else
			{
			ml_value = replaced;
			}
		}
	else
		error( FormatString("No replacements done for \"%s\"") << last_search_string.asString() );

	return 0;
	}

static int search_replace_helper( SearchGlobals::sea_type RE )
	{
	EmacsString replace_text;

	replace_text = getstr( "Replace with: " );

	sea_glob.search_replace_once( replace_text, RE );

	return 0;
	}

int replace_search_text_command()
	{
	return search_replace_helper( SearchGlobals::sea_type__string );
	}

int re_replace_search_text_command()
	{
	return search_replace_helper( SearchGlobals::sea_type__RE_simple );
	}

int re_advanced_replace_search_text_command()
	{
	return search_replace_helper( SearchGlobals::sea_type__RE_advanced );
	}

void SearchGlobals::search_replace_once( const EmacsString &new_string, SearchGlobals::sea_type RE )
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

	unsigned char lc;

	if( replace_case )
		{
		int i;

		flags.beg_of_str = 1;
		i = sea_loc1;
		flags.beg_of_word = i <= bf_cur->first_character() || !isalpha( bf_cur->char_at(i - 1) );
		while( i < sea_loc2 )
			{
			if( isalpha( lc = bf_cur->char_at( i ) ) )
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
					|| action == FIRST_ALL
					&& flags.beg_of_word )
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
	flags.beg_of_word = dot <= bf_cur->first_character() || ! isalpha( bf_cur->char_at( dot - 1 ) );
	for( int p = 0; p<new_string.length(); p++ )
		{
		lc = new_string[p];
		flags.last_prefix = flags.prefix;
		flags.prefix = 0;
		if( action != DO_NOTHING
		&& isalpha( lc ) )
			{
			if( islower( lc )
			&&	(action == UPPER
				|| action == FIRST_ALL
				&& flags.beg_of_word
				|| action == FIRST
				&& flags.beg_of_str) )
					lc = (unsigned char)toupper( lc );
			flags.beg_of_word = 0;
			flags.beg_of_str = 0;
			}
		else
			flags.beg_of_word = 1;
		if( lc == 92
		&& RE != SearchGlobals::sea_type__string
		&& !flags.last_prefix )
			flags.prefix = 1;
		else if( lc == '&'
		&& RE != SearchGlobals::sea_type__string
		&& ! flags.last_prefix )
			place( sea_loc1, sea_loc2 );
		else if( flags.last_prefix
		&& lc >= '1'
		&& lc < (unsigned char)(sea_nbra + '1') )
			place
			(
			sea_bra_slist[ lc - '1' ].to_mark(),
			sea_bra_elist[ lc - '1' ].to_mark()
			);
		else
			{
			bf_cur->insert_at( dot, lc );
			dot_right( 1 );
			}
		}

	if( sea_loc1 == sea_loc2 )
		dot_right( 1 );
	else
		{
		dot_left( sea_loc2 - sea_loc1 );
		bf_cur->del_back( sea_loc2, sea_loc2 - sea_loc1 );
		}
	}



//
// put dot and mark around the region matched by the n th parenthesised
// expression from the last search( n=0 = gtr the whole thing)
//
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
		error( "Out-of-bounds argument to region-around-match" );
		return 0;
		}
	if( n == 0 )
		{
		lo = sea_glob.sea_loc1;
		hi = sea_glob.sea_loc2;
		}
	else
		{
		lo = sea_glob.sea_bra_slist[ n - 1 ].to_mark();
		hi = sea_glob.sea_bra_elist[ n - 1 ].to_mark();
		}

	bf_cur->set_mark( lo, 0, false );
	set_dot( hi );

	return 0;
	}

/* Quote a string to inactivate reg-expr chars */
int quote_command( void )
	{
	EmacsString s;
	EmacsString result;

	s = getstr( ": quote " );
	for( int i=0; i<s.length(); i++ )
		{
		unsigned char ch = s[i];
		if( ch == '['
		|| ch == ']'
		|| ch == '*'
		|| ch == '+'
		|| ch == '{'
		|| ch == '.'
		|| ch == 92
		|| (ch == '^' && i == 0)
		|| (ch == '$' && s[0] == 0) )
			result.append( (unsigned char)92 );
		result.append( ch );
		}

	ml_value = result;

	return 0;
	}

/*
 * Compare two chars according to case-fold
 */
int char_compare_command( void )
	{
	if( check_args( 2, 2 ) )
		return 0;

	int a = numeric_arg( 1 );
	int b = numeric_arg( 2 );

	if( a < 0 || a > 256 || b < 0 || b > 256 )
		{
		error( "c= expects its arguments to be character values between 0 and 255" );
		return 0;
		}

	unsigned char *trt = sea_glob.sea_trt = bf_cur->b_mode.md_foldcase ? SearchGlobals::case_fold_trt : SearchGlobals::standard_trt;

	ml_value = trt[a] == trt[b];

	return 0;
	}


void init_srch( void )
	{
	unsigned i;

	/* initialize the search package, mostly just sets up translation tables */
	for( i=0; i<=255; i += 1 )
		SearchGlobals::standard_trt[i] = SearchGlobals::case_fold_trt[i] = (unsigned char)i;
	for( i='A'; i<='Z'; i += 1 )
		SearchGlobals::case_fold_trt[i] = (unsigned char)(i + ( 'a' - 'A' ));

	last_search_string = "";
	}


void SearchGlobals::place(int l1, int l2)
	{
	while( l1 < l2 )
		{
		bf_cur->insert_at( dot, bf_cur->char_at( l1 ) );
		dot_right( 1 );
		l1++;
		}
	}
