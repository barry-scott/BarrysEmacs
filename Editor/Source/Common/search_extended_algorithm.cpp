//
//    search_advanced_algorithm.cpp
//
//    Copyright (c) 2002-2016
//        Barry A. Scott
//

// string search routines

#include <emacs.h>
#include <search_extended_algorithm.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if DBG_EXT_SEARCH != 0
#define S_dbg_msg( msg )        _dbg_msg( msg )
#define S_dbg_fn_trace( msg )   _dbg_fn_trace t____( msg )
extern EmacsString syntax_bits_as_string( int syntax );

#else
#define S_dbg_msg( msg )        do { (void)0; } while( 0 )
#define S_dbg_fn_trace( msg )   do { (void)0; } while( 0 )
#define syntax_bits_as_string( arg ) arg
#endif

//
//
//    SearchAdvancedAlgorithm implementation
//
//
SearchAdvancedAlgorithm::SearchAdvancedAlgorithm()
: SearchImplementation()
, m_max_group_number( 0 )
, m_numbered_groups()
, m_named_groups()
, m_expression( NULL )
, m_case_fold( false )
{ }

SearchAdvancedAlgorithm::~SearchAdvancedAlgorithm()
{ }


bool SearchAdvancedAlgorithm::is_compatible( EmacsSearch::sea_type type )
{
    return type == EmacsSearch::sea_type__RE_extended;
}

int SearchAdvancedAlgorithm::search( int n, int dot )
{
    if( m_expression == NULL )
    {
        return 0;
    }

    // default the case folding to the users preference
    m_case_fold = bf_cur->b_mode.md_foldcase != 0;

    int end_pos = 0;
    if( n > 0 )
    {
        for( int pos=dot; pos <= bf_cur->num_characters(); pos++ )
            if( m_expression->matchExpression( pos, end_pos ) )
                return end_pos;
    }
    else
    {
        for( int pos=dot-1; pos >= bf_cur->first_character(); pos-- )
            if( m_expression->matchExpression( pos, end_pos ) )
                return pos;
    }

    // 0 means not found
    return 0;
}

bool SearchAdvancedAlgorithm::isCaseFolding() const
{
    return m_case_fold;
}

void SearchAdvancedAlgorithm::setCaseFolding( bool enabled )
{
    m_case_fold = enabled;
}

EmacsChar_t SearchAdvancedAlgorithm::caseFold( EmacsChar_t ch ) const
{
    if( m_case_fold )
    {
        return unicode_casefold( ch );
    }

    return ch;
}

int SearchAdvancedAlgorithm::syntax_looking_at( int pos )
{
    int end_pos = 0;
    if( m_expression->matchExpression( pos, end_pos ) )
    {
        return end_pos;
    }
    else
    {
        return 0;
    }
}

int SearchAdvancedAlgorithm::looking_at( int pos )
{
    int end_pos = 0;
    if( m_expression != NULL && m_expression->matchExpression( pos, end_pos ) )
    {
        ml_value = int(1);
    }
    else
    {
        ml_value = int(0);
    }

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
                case '\\':
                    i++;    // leave the pair of \ in the buffer
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
        unsigned int beg_of_str : 1;
        unsigned int beg_of_word : 1;
        unsigned int prefix : 1;
        unsigned int last_prefix : 1;
    } flags;

    if( replace_case )
    {
        flags.beg_of_str = 1u;
        flags.beg_of_word = match_start <= bf_cur->first_character() || !unicode_is_alphabetic( bf_cur->char_at( match_start - 1 ) );

        for( int i=match_start; i < match_end; i++ )
        {
            EmacsChar_t lc( bf_cur->char_at( i ) );

            if( unicode_is_alphabetic_numeric( lc ) )
            {
                if( unicode_is_upper( lc ) )
                {
                    if( flags.beg_of_str )
                    {
                        action = FIRST;
                    }
                    else
                    {
                        if( flags.beg_of_word && action != UPPER )
                        {
                            action = FIRST_ALL;
                        }
                        else
                        {
                            action = UPPER;
                        }
                    }
                }
                else
                {
                    if( action == UPPER
                    || (action == FIRST_ALL && flags.beg_of_word) )
                    {
                        action = DO_NOTHING;
                        break;
                    }
                }

                flags.beg_of_str = 0;
                flags.beg_of_word = 0;
            }
            else
            {
                flags.beg_of_word = 1u;
            }
        }
    }

    flags.beg_of_str = 1u;
    flags.prefix = 0;
    flags.beg_of_word = dot <= bf_cur->first_character() || ! unicode_is_alphabetic( bf_cur->char_at( dot - 1 ) );

    for( int p = 0; p<new_string.length(); p++ )
    {
        EmacsChar_t lc( new_string[p] );
        flags.last_prefix = flags.prefix;
        flags.prefix = 0;
        if( action != DO_NOTHING
        && unicode_is_alphabetic_numeric( lc ) )
        {
            if( unicode_is_lower( lc )
            &&  (action == UPPER
                || (action == FIRST_ALL && flags.beg_of_word)
                || (action == FIRST && flags.beg_of_str)) )
            {
                lc = unicode_to_upper( lc );
            }
            flags.beg_of_word = 0;
            flags.beg_of_str = 0;
        }
        else
        {
            flags.beg_of_word = 1u;
        }

        if( lc == '\\'
        && !flags.last_prefix )
        {
            flags.prefix = 1u;
        }
        else if( flags.last_prefix
        && lc >= '1' && lc <= '9' )
        {
            int group_number = lc - '0';
            if( group_number < 1 || group_number > m_max_group_number )
            {
                error( FormatString("group number %d is invalid - range is 1-%d")
                        << group_number << m_max_group_number );
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
    {
        dot_right( 1 );
    }
    else
    {
        dot_left( match_end - match_start );
        bf_cur->del_back( match_end, match_end - match_start );
    }
}

bool SearchAdvancedAlgorithm::matchLiteralString(
    const EmacsString &string, int pos, int &end_pos )
{
    // pos is where the first char must be
    int last_char_index = string.length() - 1;
    int last_char_pos = pos + last_char_index;
    if( last_char_pos > bf_cur->num_characters() )
    {
        return false;
    }

    if( isCaseFolding() )
    {
        // check first char
        if( caseFold( string[0] ) != caseFold( bf_cur->char_at( pos ) ) )
        {
            return false;
        }

        // check the last char
        if( caseFold( string[last_char_index] ) != caseFold( bf_cur->char_at( last_char_pos ) ) )
        {
            return false;
        }

        // check the middle
        for( int char_index=1, char_pos = pos+1; char_index < last_char_index; char_index++, char_pos++ )
        {
            if( caseFold( string[ char_index ] ) != caseFold( bf_cur->char_at( char_pos ) ) )
            {
                return false;
            }
        }
    }
    else
    {
        // check first char
        if( string[0] != bf_cur->char_at( pos ) )
        {
            return false;
        }

        // check the last char
        if( string[last_char_index] != bf_cur->char_at( last_char_pos ) )
        {
            return false;
        }

        // check the middle
        for( int char_index=1, char_pos = pos+1; char_index < last_char_index; char_index++, char_pos++ )
        {
            if( string[ char_index ] != bf_cur->char_at( char_pos ) )
            {
               return false;
            }
        }
    }

    // everything matches
    end_pos = last_char_pos + 1;
    return true;
}

void SearchAdvancedAlgorithm::regNumberedGroup( int number, RegularExpressionGroupStart *group )
{
    m_numbered_groups[ number ] = group;
}

void SearchAdvancedAlgorithm::regNamedGroup( const EmacsString &name, RegularExpressionGroupStart *group )
{
    m_named_groups[ name ] = group;
}

int SearchAdvancedAlgorithm::get_number_of_groups()
{
    return m_max_group_number;
}

int SearchAdvancedAlgorithm::get_start_of_group( int group_number )
{
    std::map<int,RegularExpressionGroupStart *>::iterator found_group;

    found_group = m_numbered_groups.find( group_number );
    if( found_group == m_numbered_groups.end() )
    {
        return 0;
    }

    return (*found_group).second->groupStart().to_mark();
}

int SearchAdvancedAlgorithm::get_end_of_group( int group_number )
{
    std::map<int,RegularExpressionGroupStart *>::iterator found_group;

    found_group = m_numbered_groups.find( group_number );
    if( found_group == m_numbered_groups.end() )
    {
        return 0;
    }

    return (*found_group).second->groupEnd().to_mark();
}

int SearchAdvancedAlgorithm::get_start_of_group( const EmacsString &group_name )
{
    std::map<EmacsString,RegularExpressionGroupStart *>::iterator found_group;

    found_group = m_named_groups.find( group_name );
    if( found_group == m_named_groups.end() )
    {
        return 0;
    }

    return (*found_group).second->groupStart().to_mark();
}

int SearchAdvancedAlgorithm::get_end_of_group( const EmacsString &group_name )
{
    std::map<EmacsString,RegularExpressionGroupStart *>::iterator found_group;

    found_group = m_named_groups.find( group_name );
    if( found_group == m_named_groups.end() )
    {
        return 0;
    }

    return (*found_group).second->groupEnd().to_mark();
}


//--------------------------------------------------------------------------------
//
// RegularExpressionTerm
//
//--------------------------------------------------------------------------------

RegularExpressionTerm::RegularExpressionTerm( SearchAdvancedAlgorithm &owner_ )
: m_owner( owner_ )
, m_next_term( NULL )
{}

RegularExpressionTerm::~RegularExpressionTerm()
{
    delete m_next_term;
}

void RegularExpressionTerm::setNextTerm( RegularExpressionTerm *next_term )
{
    emacs_assert( m_next_term == NULL );
    m_next_term = next_term;
}

void RegularExpressionTerm::appendTerm( RegularExpressionTerm *next_term )
{
    RegularExpressionTerm *end_of_list = this;
    while( end_of_list->m_next_term != NULL )
    {
        end_of_list = end_of_list->m_next_term;
    }
    end_of_list->m_next_term = next_term;
}

bool RegularExpressionTerm::matchExpression(int pos, int &end_pos )
{
    // try to match this term
    if( !matchTerm( pos, end_pos ) )
    {
        // no match all done
        return false;
    }

    // see if there is a next term
    if( m_next_term != NULL )
    {
        // match the rest of the m_expression from where this term reached
        return m_next_term->matchExpression( end_pos, end_pos );
    }

    // this term matches and no more terms to try
    return true;
}

//--------------------------------------------------------------------------------
//
// RegularExpressionString
//
//--------------------------------------------------------------------------------
RegularExpressionString::RegularExpressionString(
        SearchAdvancedAlgorithm &m_owner,
        const EmacsString &term_string )
: RegularExpressionTerm( m_owner )
, m_term_string( term_string )
{}

RegularExpressionString::~RegularExpressionString()
{}


bool RegularExpressionString::matchTerm( int pos, int &end_pos )
{
    S_dbg_msg( FormatString("RegularExpressionString::matchTerm() %r") << m_term_string );
    bool match = m_owner.matchLiteralString( m_term_string, pos, end_pos );

    S_dbg_msg( FormatString("RegularExpressionString::matchTerm() -> %d") << match );
    return match;
}


//--------------------------------------------------------------------------------
//
// RegularExpressionCharSet
//
//--------------------------------------------------------------------------------
RegularExpressionCharSet::RegularExpressionCharSet(
        SearchAdvancedAlgorithm &m_owner,
        const EmacsString &char_set,
        bool include_word_chars )
: RegularExpressionTerm( m_owner )
, m_char_set( char_set )
, m_include_word_chars( include_word_chars )
{}

RegularExpressionCharSet::~RegularExpressionCharSet()
{}


bool RegularExpressionCharSet::matchTerm( int pos, int &end_pos )
{
    if( pos > bf_cur->num_characters() )
    {
        return false;
    }

    EmacsChar_t ch( bf_cur->char_at( pos ) );

    // true if including word chars and its a word char
    if( m_include_word_chars && bf_cur->char_at_is( pos, SYNTAX_WORD ) )
    {
        end_pos = pos+1;
        return true;
    }

    // true if in set
    if( m_char_set.index( ch ) >= 0 )
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
RegularExpressionNotCharSet::RegularExpressionNotCharSet(
        SearchAdvancedAlgorithm &m_owner,
        const EmacsString &char_set,
        bool include_word_chars )
: RegularExpressionTerm( m_owner )
, m_char_set( char_set )
, m_include_word_chars( include_word_chars )
{}

RegularExpressionNotCharSet::~RegularExpressionNotCharSet()
{}


bool RegularExpressionNotCharSet::matchTerm( int pos, int &end_pos )
{
    EmacsChar_t ch( bf_cur->char_at( pos ) );

    if( pos > bf_cur->num_characters() )
    {
        return false;
    }

    // true if including word chars and its not a word char
    if( m_include_word_chars && !bf_cur->char_at_is( pos, SYNTAX_WORD ) )
    {
        end_pos = pos+1;
        return true;
    }

    // true if not in set
    if( m_char_set.index( ch ) < 0 )
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
        SearchAdvancedAlgorithm &m_owner,
        int min_repeats,
        int max_repeats,
        RegularExpressionTerm *repeat_term
        )
: RegularExpressionTerm( m_owner )
, m_min_repeats( min_repeats )
, m_max_repeats( max_repeats )
, m_repeat_term( repeat_term )
{}

RegularExpressionRepeat::~RegularExpressionRepeat()
{
    delete m_repeat_term;
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
        SearchAdvancedAlgorithm &m_owner,
        int min_repeats,
        int max_repeats,
        RegularExpressionTerm *repeat_term
        )
: RegularExpressionRepeat( m_owner, min_repeats, max_repeats, repeat_term )
{}

RegularExpressionRepeatMost::~RegularExpressionRepeatMost()
{}

bool RegularExpressionRepeatMost::matchExpression( int start_pos, int &end_pos )
{
    int matches = 0;
    int next_pos = start_pos;

    // match the min matches
    for( matches=0; matches<m_min_repeats; matches++ )
    {
        if( !m_repeat_term->matchExpression( next_pos, next_pos ) )
        {
            return false;
        }
    }

    return matchExpressionMost( next_pos, end_pos, matches );
}

bool RegularExpressionRepeatMost::matchExpressionMost( int start_pos, int &end_pos, int matches )
{
    if( matches < m_max_repeats && m_repeat_term->matchExpression( start_pos, end_pos ) )
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

        if( m_next_term )
        {
            if( m_next_term->matchExpression( end_pos, end_pos ) )
            {
                return true;
            }

            return m_next_term->matchExpression( start_pos, end_pos );
        }

        return true;
    }
    else
    {
        if( m_next_term )
        {
            return m_next_term->matchExpression( start_pos, end_pos );
        }

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
        SearchAdvancedAlgorithm &m_owner,
        int min_repeats,
        int max_repeats,
        RegularExpressionTerm *repeat_term
        )
: RegularExpressionRepeat( m_owner, min_repeats, max_repeats, repeat_term )
{}

RegularExpressionRepeatLeast::~RegularExpressionRepeatLeast()
{}

bool RegularExpressionRepeatLeast::matchExpression( int start_pos, int &end_pos )
{
    int matches = 0;
    int next_pos = start_pos;

    // match the min matches
    for( matches=0; matches<m_min_repeats; matches++ )
    {
        if( !m_repeat_term->matchExpression( next_pos, next_pos ) )
        {
            return false;
        }
    }

    if( m_next_term == NULL )
    {
        return true;
    }

    for(;;)
    {
        if( m_next_term->matchExpression( next_pos, end_pos ) )
        {
            return true;
        }

        if( matches >= m_max_repeats )
        {
            break;
        }

        if( !m_repeat_term->matchExpression( next_pos, next_pos ) )
        {
            return false;
        }
        else
        {
            matches++;
        }
    }

    return false;
}

//--------------------------------------------------------------------------------
//
// RegularExpressionAlternation
//
//--------------------------------------------------------------------------------
RegularExpressionAlternation::RegularExpressionAlternation( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
, m_alternative_expressions()
{}

RegularExpressionAlternation::~RegularExpressionAlternation()
{
    std::list<RegularExpressionTerm *>::iterator alternative = m_alternative_expressions.begin();

    while( alternative != m_alternative_expressions.end() )
    {
        RegularExpressionTerm *m_expression = *alternative;
        delete m_expression;
        ++alternative;
    }
}


void RegularExpressionAlternation::addAlternative( RegularExpressionTerm *term )
{
    m_alternative_expressions.push_back( term );
}


bool RegularExpressionAlternation::matchTerm( int start_pos, int &end_pos )
{
    std::list<RegularExpressionTerm *>::iterator alternative = m_alternative_expressions.begin();

    while( alternative != m_alternative_expressions.end() )
    {
        RegularExpressionTerm *m_expression = *alternative;
        if( m_expression->matchExpression( start_pos, end_pos ) )
            return true;
        ++alternative;
    }

    return false;
}

//--------------------------------------------------------------------------------
//
// RegularExpressionGroupStart
//
//--------------------------------------------------------------------------------

RegularExpressionGroupStart::RegularExpressionGroupStart( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
, m_start_pos( 0 )
, m_start_mark()
, m_end_mark()
{}

RegularExpressionGroupStart::~RegularExpressionGroupStart()
{}

bool RegularExpressionGroupStart::matchTerm( int pos, int &end_pos )
{
    end_pos = pos;
    m_start_pos = pos;
    return true;
}

const Marker &RegularExpressionGroupStart::groupStart() const
{
    return m_start_mark;
}

const Marker &RegularExpressionGroupStart::groupEnd() const
{
    return m_end_mark;
}

//--------------------------------------------------------------------------------
//
// RegularExpressionGroupEnd
//
//--------------------------------------------------------------------------------

RegularExpressionGroupEnd::RegularExpressionGroupEnd(
        SearchAdvancedAlgorithm &m_owner,
        RegularExpressionGroupStart &group_start )
: RegularExpressionTerm( m_owner )
, m_group_start( group_start )
{}

RegularExpressionGroupEnd::~RegularExpressionGroupEnd()
{}

bool RegularExpressionGroupEnd::matchTerm( int pos, int &end_pos )
{
    end_pos = pos;

    m_group_start.m_start_mark.set_mark( bf_cur, m_group_start.m_start_pos );
    m_group_start.m_end_mark.set_mark( bf_cur, end_pos );
    return true;
}

//--------------------------------------------------------------------------------
//
// RegularExpressionNamedGroup
//
//--------------------------------------------------------------------------------
RegularExpressionNamedGroup::RegularExpressionNamedGroup(
        SearchAdvancedAlgorithm &m_owner,
        const EmacsString &group_name )
: RegularExpressionGroupStart( m_owner )
, m_group_name( group_name )
{
    m_owner.regNamedGroup( m_group_name, this );
}

RegularExpressionNamedGroup::~RegularExpressionNamedGroup()
{
}

//--------------------------------------------------------------------------------
//
// RegularExpressionNumberedGroup
//
//--------------------------------------------------------------------------------
RegularExpressionNumberedGroup::RegularExpressionNumberedGroup(
        SearchAdvancedAlgorithm &m_owner,
        int group_number )
: RegularExpressionGroupStart( m_owner )
, m_group_number( group_number )
{
    m_owner.regNumberedGroup( m_group_number, this );
}

RegularExpressionNumberedGroup::~RegularExpressionNumberedGroup()
{
}

//--------------------------------------------------------------------------------
//
//    Match if m_expression matches, but do not move end_pos
//
//--------------------------------------------------------------------------------
RegularExpressionPositiveLookAhead::RegularExpressionPositiveLookAhead(
        SearchAdvancedAlgorithm &m_owner, RegularExpressionTerm *expression_)
: RegularExpressionTerm( m_owner )
, m_expression( expression_ )
{}

RegularExpressionPositiveLookAhead::~RegularExpressionPositiveLookAhead()
{
    delete m_expression;
}

bool RegularExpressionPositiveLookAhead::matchTerm( int pos, int & )
{
    int end_pos_not_used = 0;

    return m_expression->matchExpression( pos, end_pos_not_used );
}

//--------------------------------------------------------------------------------
//
//    Match if m_expression does not matches, but do not move end_pos
//
//--------------------------------------------------------------------------------
RegularExpressionNegativeLookAhead::RegularExpressionNegativeLookAhead(
        SearchAdvancedAlgorithm &m_owner, RegularExpressionTerm *expression_)
: RegularExpressionTerm( m_owner )
, m_expression( expression_ )
{}

RegularExpressionNegativeLookAhead::~RegularExpressionNegativeLookAhead()
{
    delete m_expression;
}

bool RegularExpressionNegativeLookAhead::matchTerm( int pos, int & )
{
    int end_pos_not_used = 0;

    return !m_expression->matchExpression( pos, end_pos_not_used );
}

//--------------------------------------------------------------------------------
//
//    assert matches at a word boundary
//
//--------------------------------------------------------------------------------
RegularExpressionWordBoundary::RegularExpressionWordBoundary( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
{}

RegularExpressionWordBoundary::~RegularExpressionWordBoundary()
{
}

bool RegularExpressionWordBoundary::matchTerm( int pos, int &end_pos )
{
    bool word_before_pos = false;    // assume not a word
    bool word_after_pos = false;    // assume not a word

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
//    assert matches if not at a word boundard
//
//--------------------------------------------------------------------------------
RegularExpressionNotWordBoundary::RegularExpressionNotWordBoundary( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
{}

RegularExpressionNotWordBoundary::~RegularExpressionNotWordBoundary()
{
}

bool RegularExpressionNotWordBoundary::matchTerm( int pos, int &end_pos )
{
    bool word_before_pos = false;    // assume not a word
    bool word_after_pos = false;    // assume not a word

    if( pos-1 >= bf_cur->first_character() )
    {
        word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );
    }

    if( pos <  bf_cur->num_characters() )
    {
        word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );
    }

    // true if before and after the same
    end_pos = pos;

    return word_before_pos == word_after_pos;
}


//--------------------------------------------------------------------------------
//
//    assert matches at a word start
//
//--------------------------------------------------------------------------------
RegularExpressionWordStart::RegularExpressionWordStart( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
{}

RegularExpressionWordStart::~RegularExpressionWordStart()
{
}

bool RegularExpressionWordStart::matchTerm( int pos, int &end_pos )
{
    bool word_before_pos = false;    // assume not a word
    bool word_after_pos = false;    // assume not a word

    if( pos-1 >= bf_cur->first_character() )
    {
        word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );
    }

    if( pos <  bf_cur->num_characters() )
    {
        word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );
    }

    // true if before not and after is
    end_pos = pos;

    return !word_before_pos && word_after_pos;
}

//--------------------------------------------------------------------------------
//
//    assert matches at a word end
//
//--------------------------------------------------------------------------------
RegularExpressionWordEnd::RegularExpressionWordEnd( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
{}

RegularExpressionWordEnd::~RegularExpressionWordEnd()
{
}

bool RegularExpressionWordEnd::matchTerm( int pos, int &end_pos )
{
    bool word_before_pos = false;    // assume not a word
    bool word_after_pos = false;    // assume not a word

    if( pos-1 >= bf_cur->first_character() )
    {
        word_before_pos = bf_cur->char_at_is( pos-1, SYNTAX_WORD );
    }

    if( pos <  bf_cur->num_characters() )
    {
        word_after_pos = bf_cur->char_at_is( pos, SYNTAX_WORD );
    }

    // true if before is and after not
    end_pos = pos;

    return word_before_pos && !word_after_pos;
}

//--------------------------------------------------------------------------------
//
//    assert matches at beginning of a line
//
//--------------------------------------------------------------------------------
RegularExpressionBeginningOfLine::RegularExpressionBeginningOfLine( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
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
//    assert matches at end of a line
//
//--------------------------------------------------------------------------------
RegularExpressionEndOfLine::RegularExpressionEndOfLine( SearchAdvancedAlgorithm &m_owner )
: RegularExpressionTerm( m_owner )
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
//    back reference
//
RegularExpressionBackReference::RegularExpressionBackReference( SearchAdvancedAlgorithm &m_owner, int back_ref )
: RegularExpressionTerm( m_owner )
, m_back_ref( back_ref )
{}

RegularExpressionBackReference::~RegularExpressionBackReference()
{}

bool RegularExpressionBackReference::matchTerm( int pos, int &end_pos )
{
    int group_start = m_owner.get_start_of_group( m_back_ref );
    int group_end = m_owner.get_end_of_group( m_back_ref );

    bf_cur->gap_outside_of_range( group_start, group_end );
    EmacsString group_contents( EmacsString::copy, bf_cur->ref_char_at( group_start ), group_end - group_start );

    return m_owner.matchLiteralString( group_contents, pos, end_pos );
}

//
//    syntax match
//
RegularExpressionSyntaxMatch::RegularExpressionSyntaxMatch( SearchAdvancedAlgorithm &owner )
: RegularExpressionTerm( owner )
, m_any_of()
, m_none_of()
, m_looking_at( false )

{
    S_dbg_msg( FormatString("c'tor m_any_of size %d") << m_any_of.size() );
    S_dbg_msg( FormatString("c'tor m_none_of size %d") << m_none_of.size() );
}

RegularExpressionSyntaxMatch::~RegularExpressionSyntaxMatch()
{ }

bool RegularExpressionSyntaxMatch::matchTerm( int pos, int &end_pos )
{
    if( pos > bf_cur->num_characters() )
    {
        return false;
    }

#if DBG_EXT_SEARCH!=0
    EmacsChar_t ch( bf_cur->char_at( pos ) );
#endif
    SyntaxKind_t syn( bf_cur->syntax_at( pos ) );
    S_dbg_msg( FormatString("matchTerm( %d ) ch \"%C\" syn 0x%x(%s)") << pos << ch << syn << syntax_bits_as_string( syn ) );

    if( !m_any_of.empty() )
    {
        bool any_of = false;
        for( SyntaxMatchList_t::iterator it = m_any_of.begin(); it != m_any_of.end(); ++it )
        {
            SyntaxKind_t mask = (*it).first;
            SyntaxKind_t value = (*it).second;

            S_dbg_msg( FormatString("matchTerm any_of syn 0x%x(%s) to mask 0x%x(%s) value0x%x(%s)")
                    << syn << syntax_bits_as_string( syn )
                    << mask << syntax_bits_as_string( mask )
                    << value << syntax_bits_as_string( value ) );

            if( (syn&mask) == value )
            {
                S_dbg_msg( "matchTerm any_of matched" );
                any_of = true;
                break;
            }
        }

        S_dbg_msg( FormatString("matchTerm any_of %d") << any_of );
        if( !any_of )
        {
            S_dbg_msg( "matchTerm return false !any_of" );
            return false;
        }
    }

    if( !m_none_of.empty() )
    {
        bool none_of = true;

        for( SyntaxMatchList_t::iterator it = m_none_of.begin(); it != m_none_of.end(); ++it )
        {
            SyntaxMatchPair_t &pair = *it;

            SyntaxKind_t mask = pair.first;
            SyntaxKind_t value = pair.second;

            S_dbg_msg( FormatString("matchTerm none_of syn 0x%x(%s) to mask 0x%x(%s) value0x%x(%s)")
                    << syn << syntax_bits_as_string( syn )
                    << mask << syntax_bits_as_string( mask )
                    << value << syntax_bits_as_string( value ) );

            if( (syn&mask) == value )
            {
                S_dbg_msg( "matchTerm none_of matched" );
                none_of = false;
                break;
            }
        }
        S_dbg_msg( FormatString("matchTerm none_of %d") << none_of );

        if( !none_of )
        {
            S_dbg_msg( "matchTerm return false !none_of" );
            return false;
        }
    }

    // at least 1 any_of and not none_of matched
    if( !m_looking_at )
    {
        end_pos = pos + 1;
    }

    S_dbg_msg( FormatString("matchTerm return true end_pos %d") << end_pos );
    return true;
}

void RegularExpressionSyntaxMatch::addAnyOf( SyntaxKind_t mask, SyntaxKind_t value )
{
    S_dbg_msg( FormatString("addAnyOf( mask 0x%x, value 0x%x )") << mask << value );
    m_any_of.push_back( std::make_pair( mask, value ) );
    S_dbg_msg( FormatString("addAnyOf size %d") << m_any_of.size() );
}

void RegularExpressionSyntaxMatch::addNoneOf( SyntaxKind_t mask, SyntaxKind_t value )
{
    S_dbg_msg( FormatString("addNoneOf( mask 0x%x, value 0x%x )") << mask << value );
    m_none_of.push_back( std::make_pair( mask, value ) );
    S_dbg_msg( FormatString("addNoneOf size %d") << m_none_of.size() );
}
void RegularExpressionSyntaxMatch::setLookingAt()
{
    m_looking_at = true;
}
