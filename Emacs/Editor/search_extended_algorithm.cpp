//
//	search_advanced_algorithm.cpp
//
//	Copyright (c) 2002
//		Barry A. Scott
//

// string search routines

#include <emacs.h>
#include <search_extended_algorithm.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

//
//
//	SearchAdvancedAlgorithm implementation
//
//
SearchAdvancedAlgorithm::SearchAdvancedAlgorithm()
	: SearchImplementation()
	, max_group_number( 0 )
	, numbered_groups()
	, named_groups()
	, expression( NULL )
	, case_fold( false )
	{ }

SearchAdvancedAlgorithm::~SearchAdvancedAlgorithm()
	{ }


bool SearchAdvancedAlgorithm::is_compatible( EmacsSearch::sea_type type )
	{
	return type == EmacsSearch::sea_type__RE_extended;
	}

int SearchAdvancedAlgorithm::search( int n, int dot )
	{
	if( expression == NULL )
		return 0;


	// default the case folding to the users preference
	case_fold = bf_cur->b_mode.md_foldcase != 0;

	int end_pos = 0;
	if( n > 0 )
		{
		for( int pos=dot; pos <= bf_cur->num_characters(); pos++ )
			if( expression->matchExpression( pos, end_pos ) )
				return end_pos;
		}
	else
		{
		for( int pos=dot-1; pos >= bf_cur->first_character(); pos-- )
			if( expression->matchExpression( pos, end_pos ) )
				return pos;
		}

	// 0 means not found
	return 0;
	}

bool SearchAdvancedAlgorithm::isCaseFolding() const
	{
	return case_fold;
	}

void SearchAdvancedAlgorithm::setCaseFolding( bool enabled )
	{
	case_fold = enabled;
	}

EmacsChar_t SearchAdvancedAlgorithm::caseFold( EmacsChar_t ch ) const
	{
	if( case_fold )
		return case_fold_trt[ ch ];

	return ch;
	}

int SearchAdvancedAlgorithm::looking_at()
	{
	int end_pos = 0;
	if( expression != NULL && expression->matchExpression( dot, end_pos ) )
		ml_value = int(1);
	else
		ml_value = int(0);

	return 0;
	}

void SearchAdvancedAlgorithm::search_replace_once( const EmacsString &new_string_encoded )
	{
	EmacsString new_string( new_string_encoded );

	for( int i=0; i<new_string.length(); i++ )
		{
		if( new_string[i] == '\\' )
			{
			if( i+1 < new_string.length() )
				{
				switch( new_string[i+1] )
					{
				case 't':
					new_string.remove( i, 2 );
					new_string.insert( i, '\t' );
					break;
				case 'n':
					new_string.remove( i, 2 );
					new_string.insert( i, '\n' );
					break;
				default:
					break;
					}
				}
			}
		}

	enum rep_actions
	    {
	    DO_NOTHING, UPPER, FIRST, FIRST_ALL
	    }
		action = DO_NOTHING;

	int match_start = get_start_of_group( 0 );
	int match_end = get_end_of_group( 0 );

	struct flag_bits
	{
	int beg_of_str : 1;
	int beg_of_word : 1;
	int prefix : 1;
	int last_prefix : 1;
	} flags;

	if( replace_case )
		{
		flags.beg_of_str = 1;
		flags.beg_of_word = match_start <= bf_cur->first_character() || !isalpha( bf_cur->char_at( match_start - 1 ) );

		for( int i=match_start; i < match_end; i++ )
			{
			unsigned char lc = bf_cur->char_at( i );

			if( isalpha( lc ) )
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
			}
		}

	flags.beg_of_str = 1;
	flags.prefix = 0;
	flags.beg_of_word = dot <= bf_cur->first_character() || ! isalpha( bf_cur->char_at( dot - 1 ) );

	for( int p = 0; p<new_string.length(); p++ )
		{
		unsigned char lc = new_string[p];
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

		if( lc == '\\'
		&& !flags.last_prefix )
			flags.prefix = 1;

		else if( flags.last_prefix
		&& lc >= '1' && lc <= '9' )
			{
			int group_number = lc - '0';
			if( group_number < 1 || group_number > max_group_number )
				{
				error( FormatString("group number %d is invalid - range is 1-%d")
						<< group_number << max_group_number );
				return;
				}

			int group_start = get_start_of_group( group_number );
			int group_end = get_end_of_group( group_number );

			bf_cur->gap_outside_of_range( group_start, group_end );
			EmacsString group_contents( EmacsString::copy, bf_cur->ref_char_at(group_start), group_end - group_start );

			bf_cur->ins_cstr( group_contents );
			}
		else
			{
			bf_cur->insert_at( dot, lc );
			dot_right( 1 );
			}
		}

	if( match_start == match_end )
		dot_right( 1 );
	else
		{
		dot_left( match_end - match_start );
		bf_cur->del_back( match_end, match_end - match_start );
		}
	}

bool SearchAdvancedAlgorithm::matchLiteralString( const EmacsString &string, int pos, int &end_pos )
	{
	// pos is where the first char must be
	int last_char_index = string.length() - 1;
	int last_char_pos = pos + last_char_index;
	if( last_char_pos > bf_cur->num_characters() )
		return false;

	if( isCaseFolding() )
		{
		// check first char
		if( caseFold( string[0] ) != caseFold( bf_cur->char_at( pos ) ) )
			return false;

		// check the last char
		if( caseFold( string[last_char_index] ) != caseFold( bf_cur->char_at( last_char_pos ) ) )
			return false;

		// check the middle
		for( int char_index=1, char_pos = pos+1; char_index < last_char_index; char_index++, char_pos++ )
			if( caseFold( string[ char_index ] ) != caseFold( bf_cur->char_at( char_pos ) ) )
				return false;
		}
	else
		{
		// check first char
		if( string[0] != bf_cur->char_at( pos ) )
			return false;

		// check the last char
		if( string[last_char_index] != bf_cur->char_at( last_char_pos ) )
			return false;

		// check the middle
		for( int char_index=1, char_pos = pos+1; char_index < last_char_index; char_index++, char_pos++ )
			if( string[ char_index ] != bf_cur->char_at( char_pos ) )
				return false;
		}

	// everything matches
	end_pos = last_char_pos + 1;
	return true;
	}

void SearchAdvancedAlgorithm::regNumberedGroup( int number, RegularExpressionGroup *group )
	{
	numbered_groups[ number ] = group;
	}

void SearchAdvancedAlgorithm::regNamedGroup( const EmacsString &name, RegularExpressionGroup *group )
	{
	named_groups[ name ] = group;
	}

int SearchAdvancedAlgorithm::get_number_of_groups()
	{
	return max_group_number;
	}

int SearchAdvancedAlgorithm::get_start_of_group( int group_number )
	{
	std::map<int,RegularExpressionGroup *>::iterator found_group;

	found_group = numbered_groups.find( group_number );
	if( found_group == numbered_groups.end() )
		return 0;

	return (*found_group).second->groupStart().to_mark();
	}

int SearchAdvancedAlgorithm::get_end_of_group( int group_number )
	{
	std::map<int,RegularExpressionGroup *>::iterator found_group;

	found_group = numbered_groups.find( group_number );
	if( found_group == numbered_groups.end() )
		return 0;

	return (*found_group).second->groupEnd().to_mark();
	}

int SearchAdvancedAlgorithm::get_start_of_group( const EmacsString &group_name )
	{
	std::map<EmacsString,RegularExpressionGroup *>::iterator found_group;

	found_group = named_groups.find( group_name );
	if( found_group == named_groups.end() )
		return 0;

	return (*found_group).second->groupStart().to_mark();
	}

int SearchAdvancedAlgorithm::get_end_of_group( const EmacsString &group_name )
	{
	std::map<EmacsString,RegularExpressionGroup *>::iterator found_group;

	found_group = named_groups.find( group_name );
	if( found_group == named_groups.end() )
		return 0;

	return (*found_group).second->groupEnd().to_mark();
	}


//--------------------------------------------------------------------------------
//
// RegularExpressionTerm
//
//--------------------------------------------------------------------------------

RegularExpressionTerm::RegularExpressionTerm( SearchAdvancedAlgorithm &owner_ )
	: owner( owner_ )
	, next_term( NULL )
	{}

RegularExpressionTerm::~RegularExpressionTerm()
	{
	delete next_term;
	}

void RegularExpressionTerm::setNextTerm( RegularExpressionTerm *next_term_ )
	{
	emacs_assert( next_term == NULL );
	next_term = next_term_;
	}

bool RegularExpressionTerm::matchExpression( int pos, int &end_pos )
	{
	// try to match this term
	if( !matchTerm( pos, end_pos ) )
		// no match all done
		return false;

	// see if there is a next term
	if( next_term != NULL )
		// match the rest of the expression from where this term reached
		return next_term->matchExpression( end_pos, end_pos );

	// this term matches and no more terms to try
	return true;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionString
//
//--------------------------------------------------------------------------------
RegularExpressionString::RegularExpressionString( SearchAdvancedAlgorithm &owner, const EmacsString &term_string_ )
	: RegularExpressionTerm( owner )
	, term_string( term_string_ )
	{}

RegularExpressionString::~RegularExpressionString()
	{}


bool RegularExpressionString::matchTerm( int pos, int &end_pos )
	{
	return owner.matchLiteralString( term_string, pos, end_pos );
	}


//--------------------------------------------------------------------------------
//
// RegularExpressionCharSet
//
//--------------------------------------------------------------------------------
RegularExpressionCharSet::RegularExpressionCharSet( SearchAdvancedAlgorithm &owner, const EmacsString &char_set_, bool include_word_chars_ )
	: RegularExpressionTerm( owner )
	, char_set( char_set_ )
	, include_word_chars( include_word_chars_ )
	{}

RegularExpressionCharSet::~RegularExpressionCharSet()
	{}


bool RegularExpressionCharSet::matchTerm( int pos, int &end_pos )
	{
	unsigned char ch( bf_cur->char_at( pos ) );

	if( pos > bf_cur->num_characters() )
		return false;

	// true if including word chars and its a word char
	if( include_word_chars && bf_cur->char_at_is( pos, SYNTAX_WORD ) )
		{
		end_pos = pos+1;
		return true;
		}

	// true if in set
	if( char_set.index( ch ) >= 0 )
		{
		end_pos = pos+1;
		return true;
		}

	return false;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionNotCharSet
//
//--------------------------------------------------------------------------------
RegularExpressionNotCharSet::RegularExpressionNotCharSet
		( SearchAdvancedAlgorithm &owner, const EmacsString &char_set_, bool include_word_chars_ )
	: RegularExpressionTerm( owner )
	, char_set( char_set_ )
	, include_word_chars( include_word_chars_ )
	{}

RegularExpressionNotCharSet::~RegularExpressionNotCharSet()
	{}


bool RegularExpressionNotCharSet::matchTerm( int pos, int &end_pos )
	{
	unsigned char ch( bf_cur->char_at( pos ) );

	if( pos > bf_cur->num_characters() )
		return false;

	// true if including word chars and its not a word char
	if( include_word_chars && !bf_cur->char_at_is( pos, SYNTAX_WORD ) )
		{
		end_pos = pos+1;
		return true;
		}

	// true if not in set
	if( char_set.index( ch ) < 0 )
		{
		end_pos = pos+1;
		return true;
		}

	return false;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionRepeat
//
//--------------------------------------------------------------------------------
RegularExpressionRepeat::RegularExpressionRepeat
		(
		SearchAdvancedAlgorithm &owner,
		int min_repeats_,
		int max_repeats_,
		RegularExpressionTerm *repeat_term_
		)
	: RegularExpressionTerm( owner )
	, min_repeats( min_repeats_ )
	, max_repeats( max_repeats_ )
	, repeat_term( repeat_term_ )
	{}

RegularExpressionRepeat::~RegularExpressionRepeat()
	{
	delete repeat_term;
	}


bool RegularExpressionRepeat::matchTerm( int , int & )
	{
	emacs_assert( false );
	return false;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionRepeatMost
//
//--------------------------------------------------------------------------------
RegularExpressionRepeatMost::RegularExpressionRepeatMost
		(
		SearchAdvancedAlgorithm &owner,
		int min_repeats_,
		int max_repeats_,
		RegularExpressionTerm *repeat_term_
		)
	: RegularExpressionRepeat( owner, min_repeats_, max_repeats_, repeat_term_ )
	{}

RegularExpressionRepeatMost::~RegularExpressionRepeatMost()
	{}

bool RegularExpressionRepeatMost::matchExpression( int start_pos, int &end_pos )
	{
	int matches = 0;
	int next_pos = start_pos;

	// match the min matches
	for(matches=0; matches<min_repeats; matches++)
		{
		if( !repeat_term->matchExpression( next_pos, next_pos ) )
			return false;
		}

	return matchExpressionMost( next_pos, end_pos, matches );
	}

bool RegularExpressionRepeatMost::matchExpressionMost( int start_pos, int &end_pos, int matches )
	{
	if( matches < max_repeats && repeat_term->matchExpression( start_pos, end_pos ) )
		{
		// protect against a repeat_term that does not move pos
		if( end_pos > start_pos )
			{
			int final_end_pos = 0;
			if( matchExpressionMost( end_pos, final_end_pos, matches+1 ) )
				{
				end_pos = final_end_pos;
				return true;
				}
			}

		if( next_term )
			{
			if( next_term->matchExpression( end_pos, end_pos ) )
				return true;

			return next_term->matchExpression( start_pos, end_pos );
			}

		return true;
		}
	else
		{
		if( next_term )
			return next_term->matchExpression( start_pos, end_pos );

		end_pos = start_pos;
		return true;
		}
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionRepeatLeast
//
//--------------------------------------------------------------------------------
RegularExpressionRepeatLeast::RegularExpressionRepeatLeast
		(
		SearchAdvancedAlgorithm &owner,
		int min_repeats_,
		int max_repeats_,
		RegularExpressionTerm *repeat_term_
		)
	: RegularExpressionRepeat( owner, min_repeats_, max_repeats_, repeat_term_ )
	{}

RegularExpressionRepeatLeast::~RegularExpressionRepeatLeast()
	{}

bool RegularExpressionRepeatLeast::matchExpression( int start_pos, int &end_pos )
	{
	int matches = 0;
	int next_pos = start_pos;

	// match the min matches
	for(matches=0; matches<min_repeats; matches++)
		{
		if( !repeat_term->matchExpression( next_pos, next_pos ) )
			return false;
		}

	if( next_term == NULL )
		return true;

	for(;;)
		{
		if( next_term->matchExpression( next_pos, end_pos ) )
			return true;

		if( matches >= max_repeats )
			break;

		if( !repeat_term->matchExpression( next_pos, next_pos ) )
			return false;
		else
			matches++;
		}

	return false;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionAlternation
//
//--------------------------------------------------------------------------------
RegularExpressionAlternation::RegularExpressionAlternation( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	, alternative_expressions()
	{}

RegularExpressionAlternation::~RegularExpressionAlternation()
	{
	std::list<RegularExpressionTerm *>::iterator alternative = alternative_expressions.begin();

	while( alternative != alternative_expressions.end() )
		{
		RegularExpressionTerm *expression = *alternative;
		delete expression;
		++alternative;
		}
	}


void RegularExpressionAlternation::addAlternative( RegularExpressionTerm *term )
	{
	alternative_expressions.push_back( term );
	}


bool RegularExpressionAlternation::matchTerm( int start_pos, int &end_pos )
	{
	std::list<RegularExpressionTerm *>::iterator alternative = alternative_expressions.begin();

	while( alternative != alternative_expressions.end() )
		{
		RegularExpressionTerm *expression = *alternative;
		if( expression->matchExpression( start_pos, end_pos ) )
			return true;	
		++alternative;
		}

	return false;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionGroup
//
//--------------------------------------------------------------------------------

RegularExpressionGroup::RegularExpressionGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_ )
	: RegularExpressionTerm( owner )
	, expression( expression_ )
	, start_mark()
	, end_mark()
	{}

RegularExpressionGroup::~RegularExpressionGroup()
	{}

bool RegularExpressionGroup::matchTerm( int pos, int &end_pos )
	{
	if( expression->matchExpression( pos, end_pos ) )
		{
		start_mark.set_mark( bf_cur, pos );
		end_mark.set_mark( bf_cur, end_pos );
		return true;
		}

	return false;
	}

const Marker &RegularExpressionGroup::groupStart() const
	{
	return start_mark;
	}

const Marker &RegularExpressionGroup::groupEnd() const
	{
	return end_mark;
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionNamedGroup
//
//--------------------------------------------------------------------------------
RegularExpressionNamedGroup::RegularExpressionNamedGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_, const EmacsString &group_name_ )
	: RegularExpressionGroup( owner, expression_ )
	, group_name( group_name_ )
	{
	owner.regNamedGroup( group_name, this );
	}

RegularExpressionNamedGroup::~RegularExpressionNamedGroup()
	{
	}

//--------------------------------------------------------------------------------
//
// RegularExpressionNumberedGroup
//
//--------------------------------------------------------------------------------
RegularExpressionNumberedGroup::RegularExpressionNumberedGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_, int group_number_ )
	: RegularExpressionGroup( owner, expression_ )
	, group_number( group_number_ )
	{
	owner.regNumberedGroup( group_number, this );
	}

RegularExpressionNumberedGroup::~RegularExpressionNumberedGroup()
	{
	}

//--------------------------------------------------------------------------------
//
//	Match if expression matches, but do not move end_pos
//
//--------------------------------------------------------------------------------
RegularExpressionPositiveLookAhead::RegularExpressionPositiveLookAhead( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_)
	: RegularExpressionTerm( owner )
	, expression( expression_ )
	{}

RegularExpressionPositiveLookAhead::~RegularExpressionPositiveLookAhead()
	{
	delete expression;
	}

bool RegularExpressionPositiveLookAhead::matchTerm( int pos, int & )
	{
	int end_pos_not_used = 0;

	return expression->matchExpression( pos, end_pos_not_used );
	}

//--------------------------------------------------------------------------------
//
//	Match if expression does not matches, but do not move end_pos
//
//--------------------------------------------------------------------------------
RegularExpressionNegativeLookAhead::RegularExpressionNegativeLookAhead( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_)
	: RegularExpressionTerm( owner )
	, expression( expression_ )
	{}

RegularExpressionNegativeLookAhead::~RegularExpressionNegativeLookAhead()
	{
	delete expression;
	}

bool RegularExpressionNegativeLookAhead::matchTerm( int pos, int & )
	{
	int end_pos_not_used = 0;

	return !expression->matchExpression( pos, end_pos_not_used );
	}

//--------------------------------------------------------------------------------
//
//	assert matches at a word boundary
//
//--------------------------------------------------------------------------------
RegularExpressionWordBoundary::RegularExpressionWordBoundary( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionWordBoundary::~RegularExpressionWordBoundary()
	{
	}

bool RegularExpressionWordBoundary::matchTerm( int pos, int &end_pos )
	{
	bool word_before_pos = false;	// assume not a word
	bool word_after_pos = false;	// assume not a word

	if( pos-1 >= bf_cur->first_character() )
		word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );

	if( pos <  bf_cur->num_characters() )
		word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );

	// true if before and after are different
	end_pos = pos;

	return word_before_pos != word_after_pos;
	}



//--------------------------------------------------------------------------------
//
//	assert matches if not at a word boundard
//
//--------------------------------------------------------------------------------
RegularExpressionNotWordBoundary::RegularExpressionNotWordBoundary( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionNotWordBoundary::~RegularExpressionNotWordBoundary()
	{
	}

bool RegularExpressionNotWordBoundary::matchTerm( int pos, int &end_pos )
	{
	bool word_before_pos = false;	// assume not a word
	bool word_after_pos = false;	// assume not a word

	if( pos-1 >= bf_cur->first_character() )
		word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );

	if( pos <  bf_cur->num_characters() )
		word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );

	// true if before and after the same
	end_pos = pos;

	return word_before_pos == word_after_pos;
	}


//--------------------------------------------------------------------------------
//
//	assert matches at a word start
//
//--------------------------------------------------------------------------------
RegularExpressionWordStart::RegularExpressionWordStart( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionWordStart::~RegularExpressionWordStart()
	{
	}

bool RegularExpressionWordStart::matchTerm( int pos, int &end_pos )
	{
	bool word_before_pos = false;	// assume not a word
	bool word_after_pos = false;	// assume not a word

	if( pos-1 >= bf_cur->first_character() )
		word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );

	if( pos <  bf_cur->num_characters() )
		word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );

	// true if before not and after is
	end_pos = pos;

	return !word_before_pos && word_after_pos;
	}

//--------------------------------------------------------------------------------
//
//	assert matches at a word end
//
//--------------------------------------------------------------------------------
RegularExpressionWordEnd::RegularExpressionWordEnd( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionWordEnd::~RegularExpressionWordEnd()
	{
	}

bool RegularExpressionWordEnd::matchTerm( int pos, int &end_pos )
	{
	bool word_before_pos = false;	// assume not a word
	bool word_after_pos = false;	// assume not a word

	if( pos-1 >= bf_cur->first_character() )
		word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );

	if( pos <  bf_cur->num_characters() )
		word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );

	// true if before is and after not
	end_pos = pos;

	return word_before_pos && !word_after_pos;
	}

//--------------------------------------------------------------------------------
//
//	assert matches at beginning of a line
//
//--------------------------------------------------------------------------------
RegularExpressionBeginningOfLine::RegularExpressionBeginningOfLine( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionBeginningOfLine::~RegularExpressionBeginningOfLine()
	{
	}

bool RegularExpressionBeginningOfLine::matchTerm( int pos, int &end_pos )
	{
	if( pos <=  bf_cur->first_character() )
		{
		end_pos = pos;
		return true;
		}

	if( bf_cur->char_at( pos-1 ) == '\n' )
		{
		end_pos = pos;
		return true;
		}

	return false;		
	}



//--------------------------------------------------------------------------------
//
//	assert matches at end of a line
//
//--------------------------------------------------------------------------------
RegularExpressionEndOfLine::RegularExpressionEndOfLine( SearchAdvancedAlgorithm &owner )
	: RegularExpressionTerm( owner )
	{}

RegularExpressionEndOfLine::~RegularExpressionEndOfLine()
	{
	}

bool RegularExpressionEndOfLine::matchTerm( int pos, int &end_pos )
	{
	if( pos >  bf_cur->num_characters() )
		{
		end_pos = pos;
		return true;
		}

	end_pos = pos;
	return bf_cur->char_at( pos ) == '\n';
	}

//
//	back reference
//
RegularExpressionBackReference::RegularExpressionBackReference( SearchAdvancedAlgorithm &owner, int back_ref_ )
	: RegularExpressionTerm( owner )
	, back_ref( back_ref_ )
	{}

RegularExpressionBackReference::~RegularExpressionBackReference()
	{}

bool RegularExpressionBackReference::matchTerm( int pos, int &end_pos )
	{
	int group_start = owner.get_start_of_group( back_ref );
	int group_end = owner.get_end_of_group( back_ref );

	bf_cur->gap_outside_of_range( group_start, group_end );
	EmacsString group_contents( EmacsString::copy, bf_cur->ref_char_at(group_start), group_end - group_start );

	return owner.matchLiteralString( group_contents, pos, end_pos );
	}
