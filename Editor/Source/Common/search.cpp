//
//	Copyright (c) 1982-2002
//		Barry A. Scott
//

// string search routines

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


EmacsSearch sea_glob;
EmacsChar_t SearchImplementation::standard_trt[256];	// the identity TRT
EmacsChar_t SearchImplementation::case_fold_trt[256];	// folds upper to lower case

//
// If true then replace and query-replace will
// modify the case conventions of the new
// string to match those of the old.
//
SystemExpressionRepresentationIntBoolean replace_case;
SystemExpressionRepresentationIntBoolean default_replace;

// the last search string
SystemExpressionRepresentationStringReadOnly last_search_string;

int looking_at_helper( EmacsSearch::sea_type RE, const char *prompt )
	{
	/*
	 *	(looking-at str) is true iff we are
	 *	currently looking at the given RE
	 */

	EmacsString s;
	s = getstr( prompt );

	sea_glob.looking_at( s, RE );

	return 0;
	}


int looking_at( void )
	{
	return looking_at_helper( EmacsSearch::sea_type__RE_simple, ": looking-at " );
	}

int ere_looking_at( void )
	{
	return looking_at_helper( EmacsSearch::sea_type__RE_extended, ": ere-looking-at " );
	}

int search_reverse( void )
	{
	EmacsString str;
	int np;
	if( arg <= 0 )
		arg = 1;
	str = getstr("Reverse search for: ");
	np = sea_glob.search( str, -arg, dot, EmacsSearch::sea_type__string );
	if( np == 0 && ! ml_err )
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
	np = sea_glob.search( str, arg, dot, EmacsSearch::sea_type__string );
	if( np == 0 && ! ml_err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}



int re_search_reverse_helper( EmacsSearch::sea_type RE )
	{
	int np;
	EmacsString str;
	if( arg <= 0 )
		arg = 1;
	if( RE == EmacsSearch::sea_type__RE_extended )
		str = getstr("Reverse ERE search for: ");
	else
		str = getstr("Reverse RE search for: ");
	np = sea_glob.search( str, -arg, dot, RE );
	if( np == 0 && ! ml_err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_search_reverse( void )
	{
	return re_search_reverse_helper( EmacsSearch::sea_type__RE_simple );
	}

int ere_search_reverse( void )
	{
	return re_search_reverse_helper( EmacsSearch::sea_type__RE_extended );
	}


int re_search_forward_helper( EmacsSearch::sea_type RE )
	{
	EmacsString str;
	int np;
	if( arg <= 0 )
		arg = 1;
	if( RE == EmacsSearch::sea_type__RE_extended )
		str = getstr("ERE search for: ");
	else
		str = getstr("RE search for: ");
	np = sea_glob.search( str, arg, dot, RE );
	if( np == 0 && ! ml_err )
		error(FormatString("Cannot find \"%s\"") << last_search_string.asString() );
	else
		if( np > 0 )
			set_dot( np );
	return 0;
	}

int re_search_forward( void )
	{
	return re_search_forward_helper( EmacsSearch::sea_type__RE_simple );
	}

int ere_search_forward( void )
	{
	return re_search_forward_helper( EmacsSearch::sea_type__RE_extended );
	}


int replace_string_helper( EmacsSearch::sea_type RE, const EmacsString &prompt )
	{
	//
	// perform either a query replace or a normal replace
	//
	int replaced = 0;
	int olddot = dot;

	EmacsString old;
	old = getstr( prompt );

	sea_glob.compile( old, RE );

	EmacsString new_string;
	if( ml_err )
		return 0;

	new_string = getstr("New string: ");

	int np = sea_glob.search( 1, dot );
	while( np > 0 )
		{
		set_dot( np );
		sea_glob.search_replace_once( new_string );
		replaced++;

		np = sea_glob.search( 1, dot );
#if 0
		// make sure the search actually moves on
		if( dot == np )
			{
			set_dot( dot + 1 );
			
			np = sea_glob.search( 1, dot );
			}
#endif
		}

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


int replace_string_command( void )
	{
	return replace_string_helper( EmacsSearch::sea_type__string, "Old string: " );
	}

int re_replace_string( void )
	{
	return replace_string_helper(  EmacsSearch::sea_type__RE_simple, "RE old pattern: " );
	}

int ere_replace_string( void )
	{
	return replace_string_helper(  EmacsSearch::sea_type__RE_extended, "ERE old pattern: " );
	}

static int search_replace_helper()
	{
	EmacsString replace_text;

	replace_text = getstr( "Replace with: " );

	sea_glob.search_replace_once( replace_text );

	return 0;
	}

int replace_search_text_command()
	{
	return search_replace_helper();
	}

int re_replace_search_text_command()
	{
	return search_replace_helper();
	}

//
// put dot and mark around the region matched by the n th parenthesised
// expression from the last search( n=0 = gtr the whole thing)
//
int region_around_match_command( void )
	{
	int n;

	n = getnum( u_str(": region-around-match ") );
	if( ml_err )
		return 0;
	region_around_match( n );
	return 0;
	}
	
int region_around_match( int n )
	{
	if( n < 0 || n > sea_glob.get_number_of_groups() )
		{
		error( "Out-of-bounds argument to region-around-match" );
		return 0;
		}

	bf_cur->set_mark( sea_glob.get_start_of_group( n ), 0, false );
	set_dot( sea_glob.get_end_of_group( n ) );

	return 0;
	}

//
// Quote a string to inactivate reg-expr chars
//
int quote_command( void )
	{
	EmacsString s;
	EmacsString result;

	s = getstr( ": quote " );
	for( int i=0; i<s.length(); i++ )
		{
		EmacsChar_t ch = s[i];
		if( ch == '['
		|| ch == ']'
		|| ch == '*'
		|| ch == '+'
		|| ch == '{'
		|| ch == '.'
		|| ch == 92
		|| (ch == '^' && i == 0)
		|| (ch == '$' && s[0] == 0) )
			result.append( (EmacsChar_t)92 );
		result.append( ch );
		}

	ml_value = result;

	return 0;
	}

//
// Compare two chars according to case-fold
//
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

	if( bf_cur->b_mode.md_foldcase )
		ml_value = SearchImplementation::case_fold_trt[a] == SearchImplementation::case_fold_trt[b];
	else
		ml_value = a == b;

	return 0;
	}


void init_srch( void )
	{
	unsigned i;

	//
	// initialize the search package, mostly just sets up translation tables
	//
	for( i=0; i<=255; i += 1 )
		SearchImplementation::standard_trt[i] = SearchImplementation::case_fold_trt[i] = (EmacsChar_t)i;
	for( i='A'; i<='Z'; i += 1 )
		SearchImplementation::case_fold_trt[i] = (EmacsChar_t)(i + ( 'a' - 'A' ));

	last_search_string = EmacsString::null;
	}
