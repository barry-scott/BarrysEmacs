//
//    search_advanced_parser.cpp
//
//    Copyright (c) 2002-2006
//        Barry A. Scott
//
#include <emacs.h>
#include <search_extended_algorithm.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if DBG_EXT_SEARCH != 0
#define S_dbg_msg( msg )        do { if( dbg_flags&DBG_EXT_SEARCH ) _dbg_msg( msg ); } while( 0 )
#define S_dbg_fn_trace( msg )   _dbg_fn_trace t____( msg )
#else
#define S_dbg_msg( msg )        do { (void)0; } while( 0 )
#define S_dbg_fn_trace( msg )   do { (void)0; } while( 0 )
#endif

//
//    repeaters
//    ?
//    *
//    *?
//    +
//    +?
//    {n}
//    {n,}
//    {n,m}
//    {n,}?
//    {n,m}?
//
//
//    terms
//    ^
//    $
//    c        [c]
//    [...]
//    [^...]
//    .        [^\n]
//    (...)
//    |
//
//

void SearchAdvancedAlgorithm::compile( const EmacsString &pattern, EmacsSearch::sea_type RE )
{
    S_dbg_fn_trace( "SearchAdvancedAlgorithm::compile" );
    if( pattern.isNull() && m_expression != NULL )
        return;

    switch( RE )
    {
    case EmacsSearch::sea_type__string:
        compile_string( pattern );
        break;
    case EmacsSearch::sea_type__RE_extended:
        compile_expression( pattern );
        break;
    default:
        emacs_assert( false );
    }
}

void SearchAdvancedAlgorithm::compile_string( const EmacsString &pattern )
{
    delete m_expression;
    m_expression = new RegularExpressionString( *this, pattern );

    last_search_string = pattern;
}

//
//    Exceptions
//
class RegularExpressionSyntaxError
{
public:
    RegularExpressionSyntaxError( const EmacsString &reason_text_ )
        : reason_text( reason_text_ )
    {}
    virtual ~RegularExpressionSyntaxError()
    {}

    const EmacsString &reason() const
    {
        return reason_text;
    }
private:
    const EmacsString reason_text;
};


class EmacsStringStreamData
{
public:
    friend class EmacsStringStream;
    friend class EmacsStringStreamStringEnd;
    friend class EmacsStringStreamExpressionEnd ;

    EmacsStringStreamData( const EmacsString &string_ );
    ~EmacsStringStreamData();
private:
    int position;                       // position to read next char from
    const EmacsString string;           // the pattern
};


class EmacsStringStream
{
public:
    EmacsStringStream( EmacsStringStreamData &stream_data_ );
    EmacsStringStream( EmacsStringStream &stream_ );
    virtual ~EmacsStringStream();

    EmacsChar_t nextChar( bool quoted=false );      // return next char from pattern and move position
    EmacsChar_t peekNextChar( bool quoted=false );  // return following char, don't move position
    void undoNextChar();                            // undo the effect of the last nextChar()
    virtual bool atEnd( bool quoted=false ) = 0;    // at the end?
protected:
    EmacsStringStreamData &data;
};

// atEnd is true at the end of the string only
class EmacsStringStreamStringEnd : public EmacsStringStream
{
public:
    EmacsStringStreamStringEnd( EmacsStringStreamData &stream_data_ );
    EmacsStringStreamStringEnd( EmacsStringStream &stream_data_ );
    virtual ~EmacsStringStreamStringEnd();
    virtual bool atEnd( bool quoted=false );
};

// atEnd is true at end of string, when next char is "|" or ")"
class EmacsStringStreamExpressionEnd : public EmacsStringStream
{
public:
    EmacsStringStreamExpressionEnd( EmacsStringStreamData &stream_data_ );
    EmacsStringStreamExpressionEnd( EmacsStringStream &stream_data_ );
    virtual ~EmacsStringStreamExpressionEnd();
    virtual bool atEnd( bool quoted=false );
};

//--------------------------------------------------------------------------------
//
//    EmacsStringStream implementation
//
//--------------------------------------------------------------------------------
EmacsStringStreamData::EmacsStringStreamData( const EmacsString &string_ )
    : position( 0 )
    , string( string_ )
{}

EmacsStringStreamData::~EmacsStringStreamData()
{}

//--------------------------------------------------------------------------------
//
//    EmacsStringStream
//
//--------------------------------------------------------------------------------
EmacsStringStream::EmacsStringStream( EmacsStringStreamData &stream_data_ )
    : data( stream_data_ )
{}

EmacsStringStream::EmacsStringStream( EmacsStringStream &stream_ )
    : data( stream_.data )
{}

EmacsStringStream::~EmacsStringStream()
{}

// return next char from pattern and move position
EmacsChar_t EmacsStringStream::nextChar( bool quoted )
{
    S_dbg_fn_trace( "EmacsStringStream::nextChar" );

    if( atEnd( quoted ) )
        throw RegularExpressionSyntaxError( "expecting more characters in regular expression" );

    S_dbg_msg( FormatString( "next char %c" ) << data.string[ data.position ] );
    return data.string[ data.position++ ];
}

// return following char, don't move position
EmacsChar_t EmacsStringStream::peekNextChar( bool quoted )
{
    S_dbg_fn_trace( "EmacsStringStream::peekNextChar" );
    if( atEnd( quoted ) )
        throw RegularExpressionSyntaxError( "expecting more characters in regular expression" );

    S_dbg_msg( FormatString( "peek char %c" ) << data.string[ data.position ] );
    return data.string[ data.position ];
}

void EmacsStringStream::undoNextChar()
{
    emacs_assert( data.position > 0 );
    data.position--;
}


//--------------------------------------------------------------------------------
//
//
//    EmacsStringStreamStringEnd
//
//
//--------------------------------------------------------------------------------
// atEnd is true at the end of the string only
//
EmacsStringStreamStringEnd::EmacsStringStreamStringEnd( EmacsStringStreamData &stream_data_ )
    : EmacsStringStream( stream_data_ )
{}

EmacsStringStreamStringEnd::EmacsStringStreamStringEnd( EmacsStringStream &stream_ )
    : EmacsStringStream( stream_ )
{}

EmacsStringStreamStringEnd::~EmacsStringStreamStringEnd()
{}

bool EmacsStringStreamStringEnd::atEnd( bool )
{
    return data.position >= data.string.length();
}

// atEnd is true at end of string, when next char is "|" or ")"
EmacsStringStreamExpressionEnd::EmacsStringStreamExpressionEnd( EmacsStringStreamData &stream_data_ )
    : EmacsStringStream( stream_data_ )
{}

EmacsStringStreamExpressionEnd::EmacsStringStreamExpressionEnd( EmacsStringStream &stream_ )
    : EmacsStringStream( stream_ )
{}

EmacsStringStreamExpressionEnd::~EmacsStringStreamExpressionEnd()
{}

bool EmacsStringStreamExpressionEnd::atEnd( bool quoted )
{
    if( quoted )
    {
        return data.position >= data.string.length();
    }
    else
    {
        return data.position >= data.string.length()
            || data.string[data.position] == ')'
            || data.string[data.position] == '|';
    }
}


//--------------------------------------------------------------------------------
//
//
//    regular m_expression Parser
//
//
//--------------------------------------------------------------------------------
void SearchAdvancedAlgorithm::compile_expression( const EmacsString &pattern )
{
    delete m_expression;
    m_expression = NULL;

    try
    {
        EmacsStringStreamData data( pattern );
        EmacsStringStreamStringEnd stream( data );

        //
        // we claim group 0 for the whole match
        // and start the interior groups at 1
        //
        m_max_group_number = 1;
        RegularExpressionTerm *term = parse_group_contents( stream );
        if( !stream.atEnd() )
        {
            delete term;
            throw RegularExpressionSyntaxError("not all string parsed");
        }

        RegularExpressionGroupStart *group_start = new RegularExpressionNumberedGroup( *this, 0 );
        RegularExpressionGroupEnd *group_end = new RegularExpressionGroupEnd( *this, *group_start );

        group_start->setNextTerm( term );
        term->appendTerm( group_end );
        m_expression = group_start;


        last_search_string = pattern;
    }
    catch( RegularExpressionSyntaxError &e )
    {
        error( e.reason() );
    }
}


RegularExpressionTerm *SearchAdvancedAlgorithm::parse_re( EmacsStringStream &pattern )
{
    //
    //    parse the string into a list of terms
    //
    std::list<RegularExpressionTerm *> re_list;

    do
    {
        RegularExpressionTerm *re_item = parse_term( pattern );
        re_list.push_back( re_item );
    }
    while( !pattern.atEnd() );

    //
    // link all ther terms together
    //
    std::list<RegularExpressionTerm *>::iterator a = re_list.begin();
    std::list<RegularExpressionTerm *>::iterator b = re_list.begin();
    ++b;

    while( b != re_list.end() )
    {
        (*a)->appendTerm( *b );
        ++a;
        ++b;
    }

    return re_list.front();
}


RegularExpressionTerm *SearchAdvancedAlgorithm::parse_repeat( RegularExpressionTerm *term, EmacsStringStream &pattern )
{
    if( pattern.atEnd() )
        return term;

    EmacsChar_t peek_char = pattern.peekNextChar();

    if( peek_char == '?' )
    {
        pattern.nextChar();
        return new RegularExpressionRepeatLeast( *this, 0, 1, term );
    }
    else
    if( peek_char == '*' )
    {
        pattern.nextChar();
        if( !pattern.atEnd() && pattern.peekNextChar() == '?' )
        {
            pattern.nextChar();
            return new RegularExpressionRepeatLeast( *this, 0, MAX_REPEAT, term );
        }
        else
            return new RegularExpressionRepeatMost( *this, 0, MAX_REPEAT, term );
    }
    else
    if( peek_char == '+' )
    {
        pattern.nextChar();
        if( !pattern.atEnd() && pattern.peekNextChar() == '?' )
        {
            pattern.nextChar();
            return new RegularExpressionRepeatLeast( *this, 1, MAX_REPEAT, term );
        }
        else
            return new RegularExpressionRepeatMost( *this, 1, MAX_REPEAT, term );
    }
    else
    if( peek_char == '{' )
    {
        pattern.nextChar();

        int repeat_min = 0;
        int repeat_max = 0;
        parse_min_max( pattern, repeat_min, repeat_max );

        if( !pattern.atEnd() && pattern.peekNextChar() == '?' )
        {
            pattern.nextChar();
            return new RegularExpressionRepeatLeast( *this, repeat_min, repeat_max, term );
        }
        else
            return new RegularExpressionRepeatMost( *this, repeat_min, repeat_max, term );
    }
    else
        // no repeat return args
        return term;
}

//
//  parse the {} repeat, note: leading { has been consumed
//  valid syntax is
//  {[0-9]+}
//  {[0-9]+,}
//  {[0-9]+,[0-9]+}
//
void SearchAdvancedAlgorithm::parse_min_max( EmacsStringStream &pattern, int &repeat_min, int &repeat_max )
{
    if( pattern.atEnd() )
        throw RegularExpressionSyntaxError( "incomplete {} repeat term" );

    //
    // parse first mandatory
    //
    EmacsChar_t ch = pattern.nextChar();
    if( ch < '0' || ch > '9' )
        throw RegularExpressionSyntaxError( "{} repeat must start with a digit" );
    repeat_min = int( ch - '0' );
    while( !pattern.atEnd() )
    {
        ch = pattern.nextChar();
        if( ch < '0' || ch > '9' )
            break;
        repeat_min = repeat_min*10 + int( ch - '0' );
    }

    if( ch == '}' )
    {
        // it is {[0-9]+}
        repeat_max = repeat_min;
        return;
    }

    if( ch != ',' )
        throw RegularExpressionSyntaxError( FormatString( "%c unexpected in {} repeat" ) << ch );

    ch = pattern.nextChar();
    if( ch == '}' )
    {
        // it is {[0-9]+,}
        repeat_max = MAX_REPEAT;
        return;
    }

    // must be {[0-9]+,[0-9]+}

    while( ch != '}' )
    {
        if( ch < '0' || ch > '9' )
            throw RegularExpressionSyntaxError( FormatString( "%c unexpected in {} repeat" ) << ch );

        repeat_max = repeat_max * 10 + int( ch - '0' );

        ch = pattern.nextChar();
    }
}


static int hex_to_int(  EmacsChar_t ch )
{
    if( ch >= '0' && ch <= '9' )
        return ch - '0';
    if( ch >= 'a' && ch <= 'f' )
        return ch - 'a' + 10;
    if( ch >= 'A' && ch <= 'F' )
        return ch - 'A' + 10;

    throw RegularExpressionSyntaxError( FormatString("expecting \"%c\" to be a hexadecimal character") << ch );
}

RegularExpressionTerm *SearchAdvancedAlgorithm::parse_term( EmacsStringStream &pattern )
{
    EmacsChar_t ch = pattern.nextChar();

    // check for chars that are not allowed
    switch( ch )
    {
    case '?': case '*': case '+': case '{':
        throw RegularExpressionSyntaxError( FormatString( "%c unexpected" ) << ch );
    default:
        break;
    }

    // special case range of chars for a RegularExpressionString
    EmacsString simple_string;
    bool looking_for_simple_chars = true;
    while( looking_for_simple_chars )
    {
        bool is_simple_ch = true;
        bool is_repeat = false;

        // see if its not a simple char
        switch( ch )
        {
        case '\\':
            switch( pattern.peekNextChar( true ) )
            {
            case '\\':
            case '*':
            case '.':
            case '[':
            case '{':
            case '^':
            case '$':
            case '?':
            case '+':
            case ')':
                ch = pattern.nextChar( true );
                break;
            default:
                is_simple_ch = false;
                break;
            }
            break;
        case '[':
        case '(':
        case '.':
        case '^':
        case '$':
            is_simple_ch = false;
            break;
        case '?':
        case '+':
        case '*':
        case '{':
            is_repeat = true;
        default:
            break;
        }

        if( is_repeat )
        {
            // need to undo the read of repeat char
            pattern.undoNextChar();

            // if its a single char then use parse_repeat to return the term
            if( simple_string.length() == 1 )
                return parse_repeat( new RegularExpressionString( *this, simple_string ), pattern );

            // need to undo the read of the last char
            pattern.undoNextChar();

            // now remove the last char from the simple_string
            simple_string.remove( simple_string.length() - 1 );

            return new RegularExpressionString( *this, simple_string );
        }

        if( is_simple_ch )
        {
            // add the current char to the string
            simple_string.append( ch );

            // return what we have if its the end of the input
            if( pattern.atEnd() )
                return new RegularExpressionString( *this, simple_string );

            // get hold of the next char
            ch = pattern.nextChar();
        }
        else
        {
            if( simple_string.isNull() )
            {
                looking_for_simple_chars = false;
                break; // need to leave for(;;)
            }
            else
            {
                pattern.undoNextChar();
                return new RegularExpressionString( *this, simple_string );
            }
        }
    }

    switch( ch )
    {
    case '[':
        return parse_set( pattern );
    case '(':
        return parse_group( pattern );
    case '.':
        return parse_repeat( new RegularExpressionNotCharSet( *this, "\n" ), pattern );
    case '^':
        return new RegularExpressionBeginningOfLine( *this );
    case '$':
        if( pattern.atEnd() )
            return new RegularExpressionEndOfLine( *this );
        else
            return parse_repeated_char( ch, pattern );
    case '\\':
        {
        // the \ remove the special meaning of ) and | that the pattern may stop on
        EmacsStringStreamStringEnd raw_pattern( pattern );
        ch = raw_pattern.nextChar();

        if( ch >= 'a' && ch <= 'z'
        || ch >= 'A' && ch <= 'Z'
        || ch == '0' )
            switch( ch )
            {
            case '0':
                return parse_repeated_char( '\000', pattern );
            case 'a':
                return parse_repeated_char( '\007', pattern );
            case 'b':
                return new RegularExpressionWordBoundary( *this );
            case 'B':
                return new RegularExpressionNotWordBoundary( *this );
            case 'd':
                return parse_repeat( new RegularExpressionCharSet( *this, "0123456789" ), pattern );
            case 'D':
                return parse_repeat( new RegularExpressionNotCharSet( *this, "0123456789" ), pattern );
            case 'n':
                return parse_repeated_char( '\n', pattern );
            case 's':
                return parse_repeat( new RegularExpressionCharSet( *this, " \t" ), pattern );
            case 'S':
                return parse_repeat( new RegularExpressionNotCharSet( *this, " \t" ), pattern );
            case 't':
                return parse_repeated_char( '\t', pattern );
            case 'w':
                return parse_repeat( new RegularExpressionCharSet( *this, "", true ), pattern );
            case 'W':
                return parse_repeat( new RegularExpressionNotCharSet( *this, "", true ), pattern );
            case 'x':
                {
                int code = hex_to_int( pattern.nextChar() )*16;
                code += hex_to_int( pattern.nextChar() );
                return parse_repeated_char( (EmacsChar_t)code, pattern );
                }
            default:
                throw RegularExpressionSyntaxError( FormatString("reserved \"\\%c\" escape code") << ch );
            }
        else if( ch == '<' )
            return new RegularExpressionWordStart( *this );

        else if( ch == '<' )
            return new RegularExpressionWordEnd( *this );

        else if( ch >= '1' && ch <= '9' )
        {
            // back ref
            int back_ref = ch - '0';

            if( !pattern.atEnd() )
            {
                ch = pattern.peekNextChar();
                if( ch >= '0' && ch <= '9' )
                {
                    pattern.nextChar();
                    back_ref = back_ref*10 + (ch - '0');
                }
            }
            if( back_ref > m_max_group_number )
                throw RegularExpressionSyntaxError( FormatString("back reference \\%d invalid, only %d groups") << back_ref << m_max_group_number );

            return parse_repeat( new RegularExpressionBackReference( *this, back_ref ), pattern );
        }
        else
            // pass thru all other chars
            return parse_repeated_char( ch, pattern );
        }

    default:
        return parse_repeated_char( ch, pattern );
    }
}

RegularExpressionTerm *SearchAdvancedAlgorithm::parse_repeated_char( EmacsChar_t ch, EmacsStringStream &pattern )
{
    EmacsString ch_in_string;
    ch_in_string.append( ch );
    return parse_repeat( new RegularExpressionString( *this, ch_in_string ), pattern );
}

//
//    matches either
//    [set] or [^set] where set is one or more chars.
//    The first char in set can be ']'
//
RegularExpressionTerm *SearchAdvancedAlgorithm::parse_set( EmacsStringStream &pattern )
{
    // need a stream that will allow ) and | to be read
    EmacsStringStreamStringEnd raw_pattern( pattern );

    bool invert = false;
    bool include_word_chars = false;
    EmacsString char_set;

    EmacsChar_t ch = raw_pattern.nextChar();
    if( ch == '^' )
    {
        invert = true;
        ch = raw_pattern.nextChar();
    }

    // looping this ways deals with the case of ']' being allowed in the set
    do
    {
        if( ch == '\\' )
        {
            ch = raw_pattern.nextChar();
            switch( ch )
            {
            case 'w':
                include_word_chars = true;
                break;
            case 'd':
                char_set.append( "0123456789" );
                break;
            case 's':
                char_set.append( " \t" );
                break;
            case '-':
            case ']':
            case '\\':
                char_set.append( ch );
                break;
            case 'n':
                char_set.append( '\n' );
                break;
            case 't':
                char_set.append( '\n' );
                break;
            case '0':
                char_set.append( '\000' );
            default:
                throw RegularExpressionSyntaxError( FormatString("reserved [\"\\%c\"] escape code") << ch );
            }
        }
        else
        {
            if( raw_pattern.peekNextChar() == '-' )
            {
                EmacsChar_t low_ch;
                EmacsChar_t high_ch;

                // we have a range
                raw_pattern.nextChar();
                EmacsChar_t end_ch = raw_pattern.nextChar();
                if( end_ch < ch )
                {
                    low_ch = end_ch;
                    high_ch = ch;
                }
                else
                {
                    low_ch = ch;
                    high_ch = end_ch;
                }

                while( low_ch <= high_ch )
                {
                    char_set.append( low_ch );
                    low_ch++;
                }
            }
            else
                char_set.append( ch );
        }

        ch = raw_pattern.nextChar();
    }
    while( ch != ']' );

    if( invert )
        return parse_repeat( new RegularExpressionNotCharSet( *this, char_set, include_word_chars ), pattern );
    else
        return parse_repeat( new RegularExpressionCharSet( *this, char_set, include_word_chars ), pattern );
}

//
//    Figure out which sort of group this is
//
//    (?%...) - ... is a list of options
//    (?P<name>...) - group named name
//    (?P=name) - back ref to group named name
//    (?#...) - comment
//    (?=...) - look ahead positive assertion
//    (?!...) - look ahead negative assertion
//    (?<=...) - look behind positive assertion
//    (?<!...) - look behind negative assertion
//    (?:...) - group, but don't remember
//    Any other character following ? is reserved
//    (...) - numbered group
//
//
RegularExpressionTerm *SearchAdvancedAlgorithm::parse_group( EmacsStringStream &pattern )
{
    RegularExpressionTerm *term = NULL;
    EmacsStringStreamStringEnd raw_stream( pattern );

    EmacsChar_t ch = raw_stream.peekNextChar();
    if( ch == '?' )
    {
        raw_stream.nextChar();
        ch = raw_stream.nextChar();
        switch( ch )
        {
        case '#':
            // comment - skip its contents and return the next term
            while( raw_stream.peekNextChar() != ')' )
                raw_stream.nextChar();
            // now return a term
            return parse_term( pattern );
        case ':':
            // don't remember, but can repeat
            term = parse_group_contents( pattern );
            break;
        case '=':
            // look ahead, cannot repeat
            return new RegularExpressionPositiveLookAhead
                (
                *this,
                parse_group_contents( pattern )
                );
        case '!':
            // look ahead, cannot repeat
            return new RegularExpressionNegativeLookAhead
                (
                *this,
                parse_group_contents( pattern )
                );
        default:
            throw RegularExpressionSyntaxError( "reserved (? ) sequence" );
        }
    }
    else
    {
        int group_number = m_max_group_number;
        m_max_group_number++;
        RegularExpressionTerm *contents = parse_group_contents( pattern );

        RegularExpressionGroupStart * group_start = new RegularExpressionNumberedGroup( *this, group_number );
        RegularExpressionGroupEnd *group_end = new RegularExpressionGroupEnd( *this, *group_start );

        group_start->setNextTerm( contents );
        contents->appendTerm( group_end );
        term = group_start;
    }

    if( raw_stream.nextChar() != ')' )
    {
        delete term;
        throw RegularExpressionSyntaxError( "expecting group to finish with a \")\"" );
    }

    // now parse the repeat for the term
    return parse_repeat( term, pattern );
}

RegularExpressionTerm *SearchAdvancedAlgorithm::parse_group_contents( EmacsStringStream &pattern )
{
    EmacsStringStreamStringEnd raw_stream( pattern );
    EmacsStringStreamExpressionEnd group_parse_stream( pattern );

    RegularExpressionAlternation *alternation = NULL;

    for(;;)
    {
        RegularExpressionTerm *term = parse_re( group_parse_stream );

        if( raw_stream.atEnd() )
        {
            if( alternation == NULL )
                return term;

            alternation->addAlternative( term );
            return alternation;
        }

        switch( raw_stream.peekNextChar() )
        {
        case ')':
            if( alternation == NULL )
                return term;

            alternation->addAlternative( term );
            return alternation;

        case '|':
            if( alternation == NULL )
                alternation = new RegularExpressionAlternation( *this );
            alternation->addAlternative( term );

            raw_stream.nextChar();
            break;

        default:
            emacs_assert( false );
        }
    }
}
