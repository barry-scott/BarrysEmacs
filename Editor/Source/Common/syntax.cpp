//
//    Copyright (c) 1982-2010
//        Barry A. Scott
//
// Emacs routines to deal with syntax tables
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#define DEBUG_SYNTAX 0

#include <iostream>

int use_syntax_table( void );
int modify_syntax_table( void );
int modify_syntax_entry( void );
static int paren_scan(int stop_at_newline, int forw);
int backward_paren_bl( void );
int backward_paren( void );
int forward_paren_bl( void );
int forward_paren( void );
int dump_syntax_table( void );
int list_syntax_tables( void );
int syntax_loc( void );
static int scan_comment( int i, const SyntaxString &comment );
int current_syntax_entity( void );
void init_syntax( void );
void syntax_insert_update( int, int );
void syntax_delete_update( int, int );

SyntaxNameTable SyntaxTable::name_table( 8, 8 );

SyntaxTable *global_syntax_table;

void init_syntax()
{
    global_syntax_table = EMACS_NEW SyntaxTable( "global-syntax-table" );
}

SyntaxTable::SyntaxTable( const EmacsString &name )
: s_name( name )
, s_kind()
, s_strings()
{
    name_table.add( name, this );

    EmacsCharCategorySet_t::const_iterator cur = getNumericBegin();
    EmacsCharCategorySet_t::const_iterator end = getNumericEnd();

    while( cur != end )
    {
        EmacsChar_t ch = *cur++;
        s_kind[ ch ] = SYNTAX_WORD;
    }
    cur = getAlphabeticBegin();
    end = getAlphabeticEnd();

    while( cur != end )
    {
        EmacsChar_t ch = *cur++;
        s_kind[ ch ] = SYNTAX_WORD;
    }
}

SyntaxTable::SyntaxTable( const EmacsString &name, const SyntaxTable &other )
: s_name( name )
, s_kind( other.s_kind )
, s_strings( other.s_strings )
{
    name_table.add( name, this );
}

SyntaxTable::~SyntaxTable()
{
    name_table.remove( s_name );
}

void SyntaxTable::q()
{
    std::cout << "s_kind size() " << std::dec << s_kind.size() << std::endl;
    EmacsCharToSyntaxKind_t::iterator i = s_kind.begin();
    while( i != s_kind.end() )
    {
        std::cout << std::hex << "  key=0x" << i->first << " val=0x" << i->second << std::endl;
        ++i;
    }
}

SyntaxKind_t SyntaxTable::getSyntaxKind( EmacsChar_t ch ) const
{
    SyntaxKind_t kind;
    EmacsCharToSyntaxKind_t::const_iterator i = s_kind.find( ch );
    if( i == s_kind.end() )
        kind = SYNTAX_DULL;
    else
        kind = i->second;

    return kind;
}

void SyntaxTable::eraseSyntaxKind( EmacsChar_t ch )
{
    EmacsCharToSyntaxKind_t::iterator i = s_kind.find( ch );
    if( i != s_kind.end() )
    {
        s_kind.erase( i );
    }
}

bool SyntaxTable::hasSyntaxStrings( EmacsChar_t ch ) const
{
    return s_strings.count( ch ) == 1;
}

const SyntaxStringList_t &SyntaxTable::getSyntaxStrings( EmacsChar_t ch ) const
{
    EmacsCharToSyntaxString_t::const_iterator i = s_strings.find( ch );
    if( i == s_strings.end() )
    {
        static SyntaxStringList_t null_list;
        return null_list;
    }
    else
    {
        return i->second;
    }
}

SyntaxStringList_t &SyntaxTable::getSyntaxStrings( EmacsChar_t ch )
{
    EmacsCharToSyntaxString_t::iterator i = s_strings.find( ch );
    if( i == s_strings.end() )
    {
        static SyntaxStringList_t null_list;
        return null_list;
    }
    else
    {
        return i->second;
    }
}

void SyntaxTable::eraseSyntaxStrings( EmacsChar_t ch )
{
    EmacsCharToSyntaxString_t::iterator i = s_strings.find( ch );
    if( i != s_strings.end() )
    {
        s_strings.erase( i );
    }
}

SyntaxString::SyntaxString()
: s_kind( 0 )
, s_properties( 0 )
, s_alt_matching()
, s_main_str()
, s_match_str()
{ }

SyntaxString::SyntaxString( int _kind, int _properties, const EmacsString &_main, const EmacsString &_match )
: s_kind( _kind )
, s_properties( _properties )
, s_alt_matching()
, s_main_str( _main )
, s_match_str( _match )
{
}

SyntaxString::~SyntaxString()
{
}

SyntaxString::SyntaxString( const SyntaxString &old )
{
    // use assignment code below rather then duplicate here
    operator=( old );
}

SyntaxString &SyntaxString::operator=( const SyntaxString &old )
{
    s_kind = old.s_kind;
    s_properties = old.s_properties;
    s_alt_matching.clear();         // QQQ - why clear() and not a copy from old?
    s_main_str = old.s_main_str;
    s_match_str = old.s_match_str;

    return *this;
}

//
// select a named syntax table for this buffer
// and turn on syntax mode if it or the global
// syntax table is non-empty
//
int use_syntax_table( void )
{
    EmacsString name;
    getescword( SyntaxTable::, ": use-syntax-table ", name );
    SyntaxTable *p = SyntaxTable::find( name );
    if( p == NULL )
        p = EMACS_NEW SyntaxTable( name, *global_syntax_table );

    bf_cur->b_mode.md_syntax = p;
    bf_cur->b_syntax.syntax_valid = 0;

    return 0;
}


//
//    new style syntax table modification interface
//
//    This command can be called in one of the
//    following ways:
//
//    (modify-syntax-table "word" <char-set>)
//    (modify-syntax-table "dull" <char-set>)
//    (modify-syntax-table "comment" <comment-start-string> <comment-end-string>)
//    (modify-syntax-table "paren" <open-paren> <close-paren>)
//    (modify-syntax-table "prefix" <char-set>)
//    (modify-syntax-table "string" <char-set>)
//    (modify-syntax-table "keyword-1" <string>)
//    (modify-syntax-table "keyword-2" <string>)
//    (modify-syntax-table "keyword-3" <string>)
//
struct ModifySyntaxData
{
    const char *name;
    int value;
    int properties;
    const char *prompt_str1;
    const char *prompt_str2;
};

static ModifySyntaxData modify_syntax_table_init_data[] =
{
{"comment",                 SYNTAX_TYPE_COMMENT1,   0,
                            " (comment-begin) ",    " (comment-end) "},
{"comment-1",               SYNTAX_TYPE_COMMENT1,   0,
                            " (comment-begin) ",    " (comment-end) "},
{"comment-2",               SYNTAX_TYPE_COMMENT2,   0,
                            " (comment-begin) ",    " (comment-end) "},
{"comment-3",               SYNTAX_TYPE_COMMENT3,   0,
                            " (comment-begin) ",    " (comment-end) "},
{"dull",                    SYNTAX_DULL,            0,
                            " (character-set) ",    ""},
{"keyword-1",               SYNTAX_TYPE_KEYWORD1,    0,
                            " (keyword) ",          ""},
{"keyword-1,case-fold",     SYNTAX_TYPE_KEYWORD1,    SYNTAX_PROP_CASE_FOLD_MATCH,
                            " (keyword) ",          ""},
{"keyword-2",               SYNTAX_TYPE_KEYWORD2,    0,
                            " (keyword) ",          ""},
{"keyword-2,case-fold",     SYNTAX_TYPE_KEYWORD2,    SYNTAX_PROP_CASE_FOLD_MATCH,
                            " (keyword) ",          ""},
{"keyword-3",               SYNTAX_TYPE_KEYWORD3,    0,
                            " (keyword) ",          ""},
{"keyword-3,case-fold",     SYNTAX_TYPE_KEYWORD3,    SYNTAX_PROP_CASE_FOLD_MATCH,
                            " (keyword) ",          ""},
{"paren",                   SYNTAX_BEGIN_PAREN,     0,
                            " (open-paren) ",       " (close-paren) "},
{"prefix",                  SYNTAX_PREFIX_QUOTE,    0,
                            " (character-set) ",    ""},
{"string",                  SYNTAX_TYPE_STRING1,    0,
                            " (character-set) ",    ""},
{"string,paired",           SYNTAX_TYPE_STRING1,    SYNTAX_PROP_PAIRED,
                            " (string-begin) ",     " (string-end) "},
{"string-1",                SYNTAX_TYPE_STRING1,    0,
                            " (character-set) ",    ""},
{"string-1,paired",         SYNTAX_TYPE_STRING1,    SYNTAX_PROP_PAIRED,
                            " (string-begin) ",     " (string-end) "},
{"string-2",                SYNTAX_TYPE_STRING2,    0,
                            " (character-set) ",    ""},
{"string-2,paired",         SYNTAX_TYPE_STRING2,    SYNTAX_PROP_PAIRED,
                            " (string-begin) ",     " (string-end) "},
{"string-3",                SYNTAX_TYPE_STRING3,    0,
                            " (character-set) ",    ""},
{"string-3,paired",         SYNTAX_TYPE_STRING3,    SYNTAX_PROP_PAIRED,
                            " (string-begin) ",     " (string-end) "},
{"word",                    SYNTAX_WORD,            0,
                            " (character-set) ",    ""},
{NULL,                      0,                      0,
                            NULL,                   NULL}
};

class ModifySyntaxTablePromptData : public EmacsStringTable
{
public:
    ModifySyntaxTablePromptData();

    ModifySyntaxData *get_word_mlisp();
    ModifySyntaxData *get_word_interactive( const EmacsString &prompt );
};

ModifySyntaxTablePromptData::ModifySyntaxTablePromptData()
: EmacsStringTable( 8, 1 )
{
    for( int i=0; modify_syntax_table_init_data[i].name != NULL; i++ )
    {
        ModifySyntaxData *d = &modify_syntax_table_init_data[i];

        add( d->name, d );
    }
}


ModifySyntaxData *ModifySyntaxTablePromptData::get_word_interactive( const EmacsString &prompt )
{
    EmacsString result;
    return (ModifySyntaxData *)find( EmacsStringTable::get_word_interactive( prompt, result ) );
}

ModifySyntaxData *ModifySyntaxTablePromptData::get_word_mlisp()
{
    EmacsString result;
    return (ModifySyntaxData *)find( EmacsStringTable::get_word_mlisp( result ) );
}

static ModifySyntaxTablePromptData modify_syntax_table_data;

EmacsString modify_syntax_table_prompt(": modify-syntax-table %s%s%s");

int modify_syntax_table( void )
{
    ModifySyntaxData *d = getword( modify_syntax_table_data.,
            FormatString(modify_syntax_table_prompt) << "(type) " << "" << "" );
    if( d == NULL )
    {
        error( "modify-syntax-table - unknown type" );
        return 0;
    }

    EmacsString str1 = getstr( FormatString(modify_syntax_table_prompt) <<
            d->name << d->prompt_str1 << "" );
    EmacsString str2("");

    if( d->prompt_str2[0] != '\0' )
    {
        str2 = getstr( FormatString(modify_syntax_table_prompt) <<
            d->name << str1 << d->prompt_str2 );
    }

    bf_cur->b_mode.md_syntax->modify_table( d->value, d->properties, str1, str2 );

    return 0;
}

void SyntaxTable::modify_table( int type, int properties, const EmacsString &str1, const EmacsString &str2 )
{
    try
    {
        switch( type )
        {
        case SYNTAX_DULL:
            modify_table_dull_type( str1 );
            break;

        case SYNTAX_TYPE_STRING1:
        case SYNTAX_TYPE_STRING2:
        case SYNTAX_TYPE_STRING3:
            if( (properties&SYNTAX_PROP_PAIRED) != 0 )
                modify_table_paired_type( type, 0, str1, str2 );
            else
                modify_table_range_type( type, str1, &SyntaxTable::modify_table_set_paired_type );
            break;

        case SYNTAX_WORD:
        case SYNTAX_PREFIX_QUOTE:
            modify_table_range_type( type, str1, &SyntaxTable::modify_table_set_simple_type );
            break;

        case SYNTAX_BEGIN_PAREN:
            // Need a end paren entry to match up with the begin paren
            modify_table
            (
            SYNTAX_END_PAREN, properties,
            str2, str1
            );
            modify_table_paired_type( type, properties, str1, str2 );
            break;

        // all the following types end up building the same syntax_strings entry
        case SYNTAX_END_PAREN:
        case SYNTAX_TYPE_KEYWORD1:
        case SYNTAX_TYPE_KEYWORD2:
        case SYNTAX_TYPE_KEYWORD3:
        case SYNTAX_TYPE_COMMENT1:
        case SYNTAX_TYPE_COMMENT2:
        case SYNTAX_TYPE_COMMENT3:
            modify_table_paired_type( type, properties, str1, str2 );
            break;

        default:
            error( "modify-syntax-table - internal error");
            break;
        }
    }
    catch( SyntaxTable::SyntaxErrorException & )
    {
        error( "modify-syntax-table - syntax error");
    }
    catch( SyntaxTable::SyntaxMemoryException & )
    {
        error( "modify-syntax-table - out of memory!");
    }
}

void SyntaxTable::modify_table_dull_type( const EmacsString &str1 )
    throw( SyntaxTable::SyntaxErrorException, SyntaxTable::SyntaxMemoryException )
{
    //
    // mark the chars are not interesting
    // and remove the s_strings.
    //
    int i = 0;
    while( i < str1.length() )
    {
        EmacsChar_t lim;

        EmacsChar_t c = str1[i++];

        if( i < str1.length() && str1[i] == '-' )
        {
            i++;
            if( i >= str1.length() )
                throw SyntaxErrorException();

            lim = str1[i++];
        }
        else
            lim = c;

        if( lim < c )
            throw SyntaxErrorException();

        for( int ch=c; ch<=lim; ch++ )
        {
            // this test prevents a recursive loop
            if( getSyntaxKind( ch ) != SYNTAX_DULL )
            {
                eraseSyntaxKind( ch );  // sets to DULL

                SyntaxStringList_t copy_of_strings_list( getSyntaxStrings( ch ) );
                eraseSyntaxStrings( ch );

                for( SyntaxStringList_t::iterator cur = copy_of_strings_list.begin();
                        cur != copy_of_strings_list.end();
                            ++cur )
                {
                    // remove the other paren
                    if( cur->s_kind == SYNTAX_BEGIN_PAREN
                    || cur->s_kind == SYNTAX_END_PAREN )
                        modify_table( SYNTAX_DULL, 0, cur->s_match_str, EmacsString::null );
                }
            }
        }
    }
}

void SyntaxTable::modify_table_set_simple_type( int type, int ch )
{
    s_kind[ ch ] = getSyntaxKind( ch ) | type;
}

void SyntaxTable::modify_table_set_paired_type( int type, int ch )
{
    EmacsString str1; str1.append( char( ch ) );
    modify_table_paired_type( type, 0, str1, str1 );
}

void SyntaxTable::modify_table_range_type( int type, const EmacsString &str1, void (SyntaxTable::*set_func)( int type, int ch ) )
    throw( SyntaxTable::SyntaxErrorException, SyntaxTable::SyntaxMemoryException )
{
    int ch = 0;

    int i=0;
    while( i < str1.length() )
    {
        EmacsChar_t lim;

        EmacsChar_t c = str1[i++];
        if( i < str1.length() && str1[i] == '-' )
        {
            i++;
            if( i >= str1.length() )
                throw SyntaxErrorException();

            lim = str1[i++];
        }
        else
            lim = c;

        if( lim < c )
            throw SyntaxErrorException();

        for( ch=c; ch<=lim; ch++ )
            (this->*set_func)( type, ch );
    }
}


void SyntaxTable::modify_table_paired_type( int type, int properties, const EmacsString &str1, const EmacsString &str2 )
    throw( SyntaxTable::SyntaxErrorException, SyntaxTable::SyntaxMemoryException )
{
    if( str1.isNull() )
        throw SyntaxErrorException();

    EmacsChar_t ch = str1[0];
    add_syntax_string_to_table( ch, SyntaxString( type, properties, str1, str2 ) );

    // if case fold and first char is a letter
    if( properties&SYNTAX_PROP_CASE_FOLD_MATCH
    && SearchImplementation::case_fold_trt[ch] == SearchImplementation::case_fold_trt[ch^0x20] )
    {
        SyntaxString *syn_str2 = EMACS_NEW SyntaxString( type, properties, str1, str2 );
        if( syn_str2 == NULL )
            throw SyntaxMemoryException();

        add_syntax_string_to_table( ch^0x20, SyntaxString( type, properties, str1, str2 ) );
    }
}

void SyntaxTable::add_syntax_string_to_table( int ch, const SyntaxString &syn_str )
{
    if( (syn_str.s_kind&SYNTAX_COMMENT_MASK) != 0
    || ((syn_str.s_kind&SYNTAX_STRING_MASK) != 0 && syn_str.s_properties == SYNTAX_PROP_PAIRED) )
    {
        add_paired_syntax_string_to_table( ch, syn_str );
        return;
    }

    s_kind[ ch ] = getSyntaxKind( ch ) | syn_str.s_kind;

    // insert this entry into the s_strings list
    for( SyntaxStringList_t::iterator cur = getSyntaxStrings( ch ).begin(), end=getSyntaxStrings( ch ).end();
            cur != end;
                ++cur )
    {
        // if this is the same type as a previous entry replace it
        if( cur->s_kind == syn_str.s_kind
        && cur->s_main_str == syn_str.s_main_str )
        {
            *cur = syn_str;
            return;
        }
    }

    if( s_strings.find( ch ) == s_strings.end() )
    {
        // add an empty list
        s_strings[ ch ] = SyntaxStringList_t();
    }

    // append to the end of the list
    EmacsCharToSyntaxString_t::iterator strings( s_strings.find( ch ) );
    strings->second.push_back( syn_str );
}

void SyntaxTable::add_paired_syntax_string_to_table( int ch, const SyntaxString &syn_str )
{
    s_kind[ ch ] = getSyntaxKind( ch ) | syn_str.s_kind;

    // insert this entry into the s_strings list
    for( SyntaxStringList_t::iterator cur = getSyntaxStrings( ch ).begin(), end=getSyntaxStrings( ch ).end();
            cur != end;
                ++cur )
    {
        // if its an exact duplicate comment entry free the new one
        if( cur->s_main_str == syn_str.s_main_str
        && cur->s_match_str == syn_str.s_match_str )
        {
            return;
        }

        // if its a duplicate main_str then walk the alt chain
        if( cur->s_main_str == syn_str.s_main_str )
        {
            for( SyntaxStringList_t::iterator alt = cur->s_alt_matching.begin();
                    alt != cur->s_alt_matching.end();
                        ++alt )
            {
                if( alt->s_match_str == syn_str.s_match_str )
                {
                    return;
                }
            }
            // add to alt chain
            cur->s_alt_matching.push_back( syn_str );
            return;
        }
    }

    if( s_strings.find( ch ) == s_strings.end() )
    {
        // add an empty list
        s_strings[ ch ] = SyntaxStringList_t();
    }

    // append to the end of the list
    EmacsCharToSyntaxString_t::iterator strings( s_strings.find( ch ) );
    strings->second.push_back( syn_str );
}

// old style syntax table modification interface
int modify_syntax_entry( void )
{
    static const EmacsString syntax_error( "Bogus modify-syntax-table directive.  [TP{}Cc]" );

    EmacsString p = getstr( ": modify-syntax-entry " );
    if( p.isNull() )
        return 0;

    if( p.length() < 5 )
    {
        error( syntax_error );
        return 0;
    }

    SyntaxTable &syntax = *bf_cur->b_mode.md_syntax;
    switch( p[0] )
    {
    case ' ':
    case '-':
        syntax.modify_table( SYNTAX_DULL, 0, p(5, INT_MAX ), "" );
        break;
    case 'w':
        syntax.modify_table( SYNTAX_WORD, 0, p(5, INT_MAX ), "" );
        break;
    case '(':
        syntax.modify_table( SYNTAX_BEGIN_PAREN, 0, p( 5, 6 ), p( 1, 2 ) );
        break;
    case ')':
        syntax.modify_table( SYNTAX_BEGIN_PAREN, 0, p( 1, 2 ), p( 5, 6 ) );
        break;
    case '"':
        syntax.modify_table( SYNTAX_TYPE_STRING1, 0, p( 5, 6 ), EmacsString::null );
        break;
    case '\\':
        syntax.modify_table( SYNTAX_PREFIX_QUOTE, 0, p( 5, 6 ), EmacsString::null );
        break;
    default:
        error( syntax_error );
        return 0;
    }

    // if this is comment...
    if( p[2] == '{' || p[3] == '}' )
    {
        // start off with null begin and end comment strings
        EmacsString str1;
        EmacsString str2;

        //
        // search for an existing comment entry to get the
        // other string from. Note that the old style syntax
        // table only works with a single comment defined
        //
        // QQQ - not unicode safe
        for( int i=0; i<256; i++ )
            if( bf_cur->char_is( i, SYNTAX_TYPE_COMMENT1 ) )
            {
                for( SyntaxStringList_t::iterator cur = syntax.getSyntaxStrings( i ).begin(),
                                                  end = syntax.getSyntaxStrings( i ).end();
                        cur != end;
                            ++cur )
                {
                    if( cur->s_kind == SYNTAX_TYPE_COMMENT1
                    && cur->s_main_str.length() <= 2
                    && cur->s_match_str.length() <= 2 )
                    {
                        str1 = cur->s_main_str;
                        str2 = cur->s_match_str;
                    }
                }
                break;
            }
        if( p[2] == '{' )
        {
            str1 = p( 5, 6 );
            str1.append( p[4] );
            syntax.modify_table( SYNTAX_TYPE_COMMENT1, 0, str1, str2 );
        }
        else
        {
            str2 = p( 5, 6 );
            str2.append( p[4] );
            syntax.modify_table( SYNTAX_TYPE_COMMENT1, 0, str1, str2 );
        }
    }

    return 0;
}

//
// Primitive function for paren matching. Leaves dot at enclosing left
// paren, or at top of buffer if none. Stops at a zero-level newline if
// StopAtNewline is set. Returns (to MLisp) 1 if it finds
// a match, 0 if not
// Bugs: does not correctly handle comments (it will never really handle them
// correctly...
//
static int paren_scan( int stop_at_newline, int forw )
{
    EmacsChar_t c;
    int p_c;
    EmacsChar_t parenstack[200];
    unsigned int in_string = 0;
    EmacsChar_t matching_quote = 0;
    unsigned int k;
    int on_on = 1;

    int paren_level = getnum( "Paren Level: " );
    SyntaxTable *s = bf_cur->b_mode.md_syntax;

    if( paren_level >= int(sizeof( parenstack )) )
    {
        error( FormatString("paren level to be, limit is %d") << sizeof( parenstack ) );
        return 0;
    }

    memset( parenstack, 0, sizeof parenstack );

    ml_value = int(0);

    if( stop_at_newline )
    {
        int p1; int p2; int dp;

        p1 = dot - 1;
        if( forw )
        {
            p2 = bf_cur->num_characters();
            dp = 1;
        }
        else
        {
            p2 = bf_cur->first_character();
            dp = -1;
        }

        while( p1 != p2
        &&
            (
            (c = bf_cur->char_at( p1 )) == ' '
            || c == '\t'
            || c == '\n' ) )
        {
            set_dot( p1 + 1 );
            p1 = p1 + dp;
        }
    }

    if( bf_cur->b_mode.md_syntax_array )
    {
        // brute force - calc the whole buffer
        bf_cur->syntax_fill_in_array( bf_cur->num_characters() );

        while( on_on && ! ml_err )
        {
            int sc;

            if( forw )
            {
                if( dot > bf_cur->num_characters() )
                    return 0;
                dot_right( 1 );
            }
            if( dot > 2 )
                p_c = bf_cur->char_at( dot - 2 );
            else
            {
                p_c = 0;
                if( dot <= bf_cur->first_character() )
                    return 0;
            }
            c = bf_cur->char_at( dot - 1 );
            k = s->getSyntaxKind( c );
            sc = bf_cur->syntax_at( dot - 1 );

            if( stop_at_newline && c == '\n' && paren_level == 0 )
                return 0;
            if( (sc&(SYNTAX_COMMENT_MASK|SYNTAX_STRING_MASK)) == 0
            && (k&(SYNTAX_END_PAREN|SYNTAX_BEGIN_PAREN)) )
            {
                if( (forw == 0) == ((k & SYNTAX_END_PAREN) != 0) )
                {
                    paren_level++;
                    if( paren_level == sizeof( parenstack ) )
                        error( "Too many unmatched parenthesis" );
                    else
                    {
                        for( SyntaxStringList_t::iterator cur = s->getSyntaxStrings( c ).begin(),
                                                          end = s->getSyntaxStrings( c ).end();
                                cur != end;
                                    ++cur )
                        {
                            if( cur->s_kind & (SYNTAX_BEGIN_PAREN|SYNTAX_END_PAREN)
                            && cur->s_match_str.length() == 1 )
                            {
                                parenstack[ paren_level ] = cur->s_match_str[0];
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if( paren_level > 0
                    && parenstack[ paren_level ] != 0
                    && parenstack[ paren_level ] != c )
                        error( "Parenthesis mismatch." );
                    paren_level--;
                }

                if( paren_level < 0
                || (paren_level == 0 && ! stop_at_newline) )
                    on_on = 0;
            }
            if( ! forw )
                dot_left( 1 );
        }
    }
    else
    {
        while( on_on && ! ml_err )
        {
            if( forw )
            {
                if( dot > bf_cur->num_characters() )
                    return 0;
                dot_right( 1 );
            }
            if( dot > 2 )
                p_c = bf_cur->char_at( dot - 2 );
            else
            {
                p_c = 0;
                if( dot <= bf_cur->first_character() )
                    return 0;
            }
            c = bf_cur->char_at( dot - 1 );
            k = s->getSyntaxKind( c );
            if( s->getSyntaxKind( p_c )&SYNTAX_PREFIX_QUOTE )
                k = SYNTAX_WORD;
            if( ((! in_string) || (c == matching_quote))
            && (k&SYNTAX_STRING_MASK) != 0 )
            {
                in_string = ! in_string;
                matching_quote = c;
            }

            if( stop_at_newline && c == '\n' && paren_level == 0 )
                return 0;
            if( ! in_string
            && (k&(SYNTAX_END_PAREN|SYNTAX_BEGIN_PAREN)) )
            {
                if( (forw == 0) == ((k & SYNTAX_END_PAREN) != 0) )
                {
                    paren_level++;
                    if( paren_level == sizeof( parenstack ) )
                        error( "Too many unmatched parenthesis" );
                    else
                    {
                        for( SyntaxStringList_t::iterator cur = s->getSyntaxStrings( c ).begin(),
                                                          end = s->getSyntaxStrings( c ).end();
                                cur != end;
                                    ++cur )
                        {
                            if( cur->s_kind & (SYNTAX_BEGIN_PAREN|SYNTAX_END_PAREN)
                            && cur->s_match_str.length() == 1 )
                            {
                                parenstack[ paren_level ] = cur->s_match_str[0];
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if( paren_level > 0
                    && parenstack[ paren_level ] != 0
                    && parenstack[ paren_level ] != c )
                        error( "Parenthesis mismatch." );
                    paren_level--;
                }

                if( paren_level < 0
                || (paren_level == 0 && ! stop_at_newline) )
                    on_on = 0;
            }
            if( !forw )
                dot_left( 1 );
        }
    }

    ml_value = 1;

    return 0;
}

//
// Primitive function for lisp indenting. Searches backward till it finds
// the matching left paren, or a line that begins with zero paren-balance.
// Returns the paren level at termination to mlisp.
//
int backward_paren_bl( void )
{
    return paren_scan( 1, 0 );
}

// Searches backward until it find the matching left paren
int backward_paren( void )
{
    return paren_scan( 0, 0 );
}

int forward_paren_bl( void )
{
    return paren_scan( 1, 1 );
}

// Searches forward until it find the matching left paren
int forward_paren( void )
{
    return paren_scan( 0, 1 );
}

// Function to dump syntax table to buffer in human-readable format
int dump_syntax_table( void )
{
    EmacsBufferRef old( bf_cur );

    SyntaxTable *p = getword( SyntaxTable::, ": dump-syntax-table " );
    if( p == NULL )
        return 0;

    EmacsBuffer::scratch_bfn( "Syntax table", interactive() );
    bf_cur->ins_cstr( FormatString("Syntax table: %s\n") << p->s_name );
    bf_cur->ins_str(
        "Character       Syntatic Characteristics\n"
        "----------------------------------------\n" );

    int i = 0;
    while( i < 65536 )
    {
        int j = i;
        while( j < 65535
        && p->getSyntaxKind( i ) == p->getSyntaxKind( j+1 )
        && !p->hasSyntaxStrings( i )
        && !p->hasSyntaxStrings( j+1 ) )
            j++;

        EmacsString char_range;
        if( i != j )
            if( control_character( i ) )
                if( control_character( j ) )
                    char_range = FormatString("'\\%o'-'\\%o'") << i << j;
                else
                    char_range = FormatString("'\\%o'-'%c'") << i << j;
            else
                char_range = FormatString("'%c'-'%c'") << i << j;
        else
            if( control_character( i ) )
                char_range = FormatString("'\\%o'") << i;
            else
                char_range = FormatString("'%c'") << i;


        EmacsString syntax_kinds;
        if( p->getSyntaxKind( i ) == SYNTAX_DULL )
            syntax_kinds = "Dull";
        if( p->getSyntaxKind( i )&SYNTAX_WORD )
            syntax_kinds = "Word";
        if( p->getSyntaxKind( i )&SYNTAX_BEGIN_PAREN )
        {
            if( !syntax_kinds.isNull() )
                syntax_kinds.append( ", " );
            syntax_kinds.append( "Open paren" );
        }
        if( p->getSyntaxKind( i )&SYNTAX_END_PAREN )
        {
            if( !syntax_kinds.isNull() )
                syntax_kinds.append( ", " );
            syntax_kinds.append( "Close paren" );
        }
        if( p->getSyntaxKind( i )&SYNTAX_COMMENT_MASK )
        {
            if( !syntax_kinds.isNull() )
                syntax_kinds.append( ", " );
            syntax_kinds.append( "Comment");
        }
        if( p->getSyntaxKind( i )&SYNTAX_STRING_MASK )
        {
            if( !syntax_kinds.isNull() )
                syntax_kinds.append( ", " );
            syntax_kinds.append( "String");
        }
        if( p->getSyntaxKind( i )&SYNTAX_PREFIX_QUOTE )
        {
            if( !syntax_kinds.isNull() )
                syntax_kinds.append( ", " );
            syntax_kinds.append( "Prefix quote");
        }

        bf_cur->ins_cstr( FormatString("%13s   %s\n") << char_range << syntax_kinds );

        EmacsString syntax_details;
        for( SyntaxStringList_t::iterator cur = p->getSyntaxStrings( i ).begin(),
                                          end = p->getSyntaxStrings( i ).end();
                cur != end;
                    ++cur )
        {
            switch( cur->s_kind )
            {
            case SYNTAX_TYPE_COMMENT1:
            case SYNTAX_TYPE_COMMENT2:
            case SYNTAX_TYPE_COMMENT3:
            {
                int type = cur->s_kind >> SYNTAX_COMMENT_SHIFT;
                syntax_details = FormatString("Comment type %d between \"%s\" and \"%s\"")
                                << type << cur->s_main_str << cur->s_match_str;

                for( SyntaxStringList_t::iterator alt = cur->s_alt_matching.begin();
                        alt != cur->s_alt_matching.end();
                            ++alt )
                {
                    syntax_details.append( FormatString(", \"%s\"") << alt->s_match_str );
                }
            }
                break;

            case SYNTAX_TYPE_STRING1:
            case SYNTAX_TYPE_STRING2:
            case SYNTAX_TYPE_STRING3:
            {
                int type = cur->s_kind >> SYNTAX_STRING_SHIFT;
                syntax_details = FormatString("String type %d between \"%s\" and \"%s\"")
                                << type << cur->s_main_str << cur->s_match_str;

                for( SyntaxStringList_t::iterator alt = cur->s_alt_matching.begin();
                        alt != cur->s_alt_matching.end();
                            ++alt )
                {
                    syntax_details.append( FormatString(", \"%s\"") << alt->s_match_str );
                }
            }
                break;

            case SYNTAX_BEGIN_PAREN:
                syntax_details = FormatString("Matched paren \"%s\" and \"%s\"")
                            << cur->s_main_str << cur->s_match_str;
                break;

            case SYNTAX_END_PAREN:
                syntax_details = FormatString("Matched paren \"%s\" and \"%s\"")
                            << cur->s_match_str << cur->s_main_str;
                break;

            case SYNTAX_TYPE_KEYWORD1:
            case SYNTAX_TYPE_KEYWORD2:
            case SYNTAX_TYPE_KEYWORD3:
            {
                int type = cur->s_kind >> SYNTAX_KEYWORD_SHIFT;
                syntax_details = FormatString("Keyword type %d \"%s\"")
                            << type << cur->s_main_str;
            }
                break;

            default:
                break;
            }
            if( !syntax_details.isNull() )
            {
                if( cur->s_properties&SYNTAX_PROP_REGEX_MATCH )
                    syntax_details.append( ", Regular expression");
                if( cur->s_properties&SYNTAX_PROP_CASE_FOLD_MATCH )
                    syntax_details.append( ", Case fold matched");
                bf_cur->ins_cstr( FormatString("%13s   %s\n") << "" << syntax_details );
            }
        }

        i = j + 1;
    }

    bf_cur->b_checkpointed = -1;
    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

int list_syntax_tables( void )
{
    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Syntax Table list", interactive() );

    bf_cur->ins_str("  Name\n  ----\n");

    for( int i=0; i<SyntaxTable::name_table.entries(); i++ )
        bf_cur->ins_cstr( FormatString("  %s\n") << SyntaxTable::name_table.key( i ) );

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

SystemExpressionRepresentationIntBoolean default_syntax_array;
SystemExpressionRepresentationIntBoolean default_syntax_colouring;

void SystemExpressionRepresentationSyntaxArray::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );

    if( exp_int )
        bf_cur->b_mode.md_syntax_array = 1;
    else
    {
        bf_cur->b_mode.md_syntax_array = 0;
        bf_cur->b_mode.md_syntax_colouring = 0;
    }
}

void SystemExpressionRepresentationSyntaxArray::fetch_value()
{
    exp_int = bf_cur->b_mode.md_syntax_array;
}

void SystemExpressionRepresentationSyntaxColouring::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );

    if( exp_int )
    {
        bf_cur->b_mode.md_syntax_array = 1;
        bf_cur->b_mode.md_syntax_colouring = 1;
    }
    else
        bf_cur->b_mode.md_syntax_colouring = 0;
}

void SystemExpressionRepresentationSyntaxColouring::fetch_value()
{
    exp_int = bf_cur->b_mode.md_syntax_array;
}

void SystemExpressionRepresentationBufferSyntaxTable::fetch_value()
{
    if( bf_cur->b_mode.md_syntax == NULL )
        exp_string = EmacsString::null;
    else
        exp_string = bf_cur->b_mode.md_syntax->s_name;
}

void SystemExpressionRepresentationBufferSyntaxTable::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value = new_value->asString();

    if( value.isNull() )
    {
        error( "Illegal syntax table name" );
        return;
    }
    SyntaxTable *p = SyntaxTable::find( value );
    if( p == NULL )
    {
        error( FormatString("No such syntax tables as %s") << value );
        return;
    }

    bf_cur->b_mode.md_syntax = p;
}


SystemExpressionRepresentationIntReadOnly is_dull( 0 );
SystemExpressionRepresentationIntReadOnly is_word( 1 );
SystemExpressionRepresentationIntReadOnly is_str( 2 );
SystemExpressionRepresentationIntReadOnly is_quote_character( 3 );
SystemExpressionRepresentationIntReadOnly is_comment( 4 );

int syntax_loc( void )
{
    EmacsChar_t c;
    EmacsChar_t m;
    int i;

    if( bf_cur->b_mode.md_syntax_array )
    {
        bf_cur->syntax_fill_in_array( dot+1 );

        i = bf_cur->syntax_at( dot );
        if( (i&SYNTAX_COMMENT_MASK) != 0 )
            return is_comment;
        if( (i&SYNTAX_STRING_MASK) != 0 )
            return is_str;
        if( (i&(SYNTAX_WORD|SYNTAX_KEYWORD_MASK)) != 0 )
            return is_word;
        return is_dull;
    }

    for( i=1; i<=dot - 1; i += 1 )
    {
        c = bf_cur->char_at( i );
        if( bf_cur->char_is( c, SYNTAX_STRING_MASK ) )
        {
            i++;
            m = c;
            while( i < dot )
            {
                c = bf_cur->char_at( i );
                if( bf_cur->char_is( c, SYNTAX_PREFIX_QUOTE ) )
                    i++;
                else
                    if( bf_cur->char_is( c, SYNTAX_STRING_MASK )
                    && c == m )
                        break;
                i++;
            }
            if( i >= dot )
                return is_str;
        }
        else
        if( bf_cur->char_is( c, SYNTAX_PREFIX_QUOTE ) )
        {
            i++;
            if( i >= dot )
                return is_quote_character;
        }
        else
        if( bf_cur->char_is( c, SYNTAX_COMMENT_MASK ) )
        {
            for( SyntaxStringList_t::iterator cur = bf_cur->b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                                              end = bf_cur->b_mode.md_syntax->getSyntaxStrings( c ).end();
                    cur != end;
                        ++cur )
            {
                if( (cur->s_kind&SYNTAX_COMMENT_MASK) != 0
                && cur->looking_at_main( i ) )
                {
                    i = scan_comment( i, *cur );
                    if( i >= dot )
                        return is_comment;
                    break;
                }
            }
        }
    }
    if( dot <= bf_cur->first_character() || dot > bf_cur->num_characters() )
        return is_dull;

    return
        bf_cur->char_at_is( dot, SYNTAX_WORD )
        && bf_cur->char_at_is( dot - 1, SYNTAX_WORD ) ? is_word : is_dull;
}

static int scan_comment( int i, const SyntaxString &comment )
{
    while( i < dot )
    {
        int len = comment.looking_at_match( i );
        if( len > 0 )
#ifdef _MSC_VER
            #pragma message("Crit: need to check the return value here for multi-byte comment ends")
#endif
            return i+1;

        i++;
    }
    return i;
}

bool SyntaxString::last_is_word( const SyntaxTable &table ) const
{
    EmacsChar_t last_char = s_main_str[-1];
    return (table.getSyntaxKind( last_char )&SYNTAX_WORD) != 0;
}

//
//    return true if the string is as pos in the buffer
//
int SyntaxString::looking_at_main( int pos ) const
{
    return looking_at_internal( pos, s_main_str );
}

int SyntaxString::looking_at_match( int pos ) const
{
    int len = looking_at_internal( pos, s_match_str );
    if( len > 0 )
        return len;

    for( SyntaxStringList_t::const_iterator alt = s_alt_matching.begin();
            alt != s_alt_matching.end();
                ++alt )
    {
        int len = alt->looking_at_internal( pos, alt->s_match_str );
        if( len > 0 )
            return len;
    }

    return 0;
}

int SyntaxString::looking_at_internal( int pos, const EmacsString &str ) const
{
    int len = str.length();

    switch( s_properties )
    {
    default:
    case 0:
    {
        // see if there is room for the string to fit in the buffer
        if( pos+len-1 > bf_cur->unrestrictedSize() )
            return 0;

        //
        // match last to first on the basis that doing that
        // will cause a mismatch fastest
        //

        pos += len-1;
        for( int i=len-1; i>=0; i--, pos-- )
            if( bf_cur->char_at( pos ) != str[i] )
                return 0;
    }
        break;

    case SYNTAX_PROP_CASE_FOLD_MATCH:
    {
        // see if there is room for the string to fit in the buffer
        if( pos+len-1 > bf_cur->unrestrictedSize() )
            return 0;

        //
        // match last to first on the basis that doing that
        // will cause a mismatch fastest
        //

        EmacsChar_t *trt = SearchImplementation::case_fold_trt;
        pos += len-1;
        for( int i=len-1; i>=0; i--, pos-- )
            if( trt[bf_cur->char_at( pos )] != trt[str[i]] )
                return 0;
    }
        break;

    case SYNTAX_PROP_REGEX_MATCH:
        return 0;

    case SYNTAX_PROP_CASE_FOLD_MATCH|SYNTAX_PROP_REGEX_MATCH:
        return 0;
    }

    // matched
    return len;
}

int current_syntax_entity( void )
{
    ml_value = syntax_loc();

    return 0;
}

//
//    The following two routines are passed enough information to
//    allow for an incremental update of the syntax array.
//
//    Initial we just update the syntax valid indicator.
//
void syntax_insert_update( int dot, int len )
{
    syntax_buffer_data *s = &bf_cur->b_syntax;

    if( dot < s->syntax_valid )
    {
        if( s->syntax_update_credit > 0 )
        {
            s->syntax_update_credit--;
            s->syntax_valid += len;
            bf_cur->syntax_update_buffer( dot, len );
        }
        else
        {
            cant_1line_opt = 1;
            s->syntax_valid = dot;
        }
    }
    else
        cant_1line_opt = 1;

    if( dot < bf_cur->b_line_valid )
        bf_cur->b_line_valid = dot;
}

void syntax_delete_update( int dot, int len )
{
    syntax_buffer_data *s = &bf_cur->b_syntax;

    if( dot < s->syntax_valid )
    {
        if( s->syntax_update_credit > 0 )
        {
            s->syntax_update_credit--;
            s->syntax_valid -= len;
            bf_cur->syntax_update_buffer( dot, 0 );
        }
        else
        {
            cant_1line_opt = 1;
            s->syntax_valid = dot;
        }
    }
    else
        cant_1line_opt = 1;

    if( dot < bf_cur->b_line_valid )
        bf_cur->b_line_valid = dot;
}

//
//    Fill in the syntax array as far as the 'required' offset
//
enum syn_states
{
    st_simple, st_string, st_comment
};

#if DEBUG_SYNTAX
static char *state_to_string( syn_states st )
{
    switch( st )
    {
    case st_simple:
        return "Simple ";
    case st_string:
        return "String ";
    case st_comment:
        return "Comment";
    }
}

static EmacsString syntax_bits_as_string( int syntax )
{
    EmacsString result;
    if( syntax == 0 )
        result = "Dull";
    if( syntax & SYNTAX_WORD )
        result.append( "W" );
    if( syntax & SYNTAX_STRING )
        switch( syntax & SYNTAX_STRING )
        {
        case SYNTAX_TYPE_STRING1:
            result.append( "S1" ); break;
        case SYNTAX_TYPE_STRING2:
            result.append( "S2" ); break;
        case SYNTAX_TYPE_STRING3:
            result.append( "S3" ); break;
        }
    if( syntax & SYNTAX_COMMENT )
        switch( syntax & SYNTAX_COMMENT )
        {
        case SYNTAX_TYPE_COMMENT1:
            result.append( "C1" ); break;
        case SYNTAX_TYPE_COMMENT2:
            result.append( "C2" ); break;
        case SYNTAX_TYPE_COMMENT3:
            result.append( "C3" ); break;
        }
    if( syntax & SYNTAX_KEYWORD )
        switch( syntax & SYNTAX_KEYWORD )
        {
        case SYNTAX_TYPE_KEYWORD1:
            result.append( "K1" ); break;
        case SYNTAX_TYPE_KEYWORD2:
            result.append( "K2" ); break;
        case SYNTAX_TYPE_KEYWORD3:
            result.append( "K3" ); break;
        }
    if( syntax & SYNTAX_PREFIX_QUOTE )
        result.append( "Q" );
    if( syntax & SYNTAX_BEGIN_PAREN )
        result.append( "(" );
    if( syntax & SYNTAX_END_PAREN )
        result.append( ")" );

    return result;
}
#endif

// return true if there is a error with initBuffer
bool EmacsBuffer::syntax_fill_in_array( int required )
{
    syntax_buffer_data *s = &b_syntax;
    enum syn_states state = st_simple;
    int pos;

    if( !b_mode.md_syntax_array )
        return true;

    s->syntax_update_credit = 4;

    // allocate and init the buffer if its not present
    if( !s->initBuffer() )
    {
        b_mode.md_syntax_array = 0;
        return false;
    }

    if( required > unrestrictedSize() )
        required = unrestrictedSize();
    if( required <= s->syntax_valid )
        return true;

    //
    // backup to a dull character
    // as that ensures that we can
    // figure out the rest of the syntax
    // unambiguosly
    //
    for( pos=s->syntax_valid-1-SYNTAX_STRING_SIZE; pos > 1; pos-- )
    {
        if( syntax_at(pos) == SYNTAX_DULL )
            break;
    }

    if( pos < 1 )
        pos = 1;

    SyntaxStringList_t::iterator comment;
    SyntaxStringList_t::iterator string;
    SyntaxStringList_t::iterator end;

    int limit = required;
    for( ; pos<=limit; pos++ )
    {
        EmacsChar_t c = char_at(pos);
        SyntaxKind_t kind = b_mode.md_syntax->getSyntaxKind( c );

        if( c == '\n' )
            cant_1line_opt = 1;

        // emacs_check_malloc_block( b_syntax_base );

#if DEBUG_SYNTAX
        _dbg_msg( FormatString("state: %s, pos: %d, s_kind: %s, c: %C, s: %s")
                << state_to_string( state )
                << pos
                << syntax_bits_as_string( kind )
                << c
                << syntax_bits_as_string( syntax_at( pos ) ) );
#endif
        switch( state )
        {
        case st_simple:
            if( pos > required
            && (syntax_at( pos )&SYNTAX_MULTI_CHAR_TYPES) == 0 )
                return true;

            if( kind&SYNTAX_COMMENT_MASK )
            {
                for( comment = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                         end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        comment != end;
                            ++comment )
                {
                    int len;
                    if( (comment->s_kind&SYNTAX_COMMENT_MASK) != 0
                    && (len = comment->looking_at_main( pos )) > 0 )
                    {
                        state = st_comment;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, comment->s_kind );

                        pos--;    // incremented at the top of the loop
                        goto for_loop_end;
                    }
                }
            }

            if( kind&SYNTAX_STRING_MASK )
            {
                for( string = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                        end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        string != end;
                            ++string )
                {
                    int len;
                    if( (string->s_kind&SYNTAX_STRING_MASK) != 0
                    && (len = string->looking_at_main( pos )) > 0 )
                    {
                        state = st_string;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, string->s_kind );

                        pos--;    // incremented at the top of the loop
                        goto for_loop_end;
                    }
                }
            }

            //
            // a keyword may start each time a word to nonword OR nonword to word boundary is crossed
            //
            if( kind&SYNTAX_KEYWORD_MASK
            && !(((kind&SYNTAX_WORD) != 0) && ((syntax_at( pos-1 )&SYNTAX_WORD) != 0)) )
            {
                bool found = false;
                for( SyntaxStringList_t::iterator keyword = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                                                      end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        keyword != end;
                            ++keyword )
                {
                    if( keyword->s_kind&SYNTAX_KEYWORD_MASK )
                    {
                        bool last_is_word = keyword->last_is_word( *b_mode.md_syntax );

                        if( pos + keyword->s_main_str.length() <= unrestrictedSize()
                        && !(last_is_word && char_at_is( pos+keyword->s_main_str.length(), SYNTAX_WORD ))
                        && keyword->looking_at_main( pos ) )
                        {
                            for( int i=0; i<keyword->s_main_str.length(); i++, pos++ )
                                set_syntax_at( pos, (SyntaxData_t)keyword->s_kind );
                            pos--;

                            found = true;
                            break;
                        }
                    }
                }
                // if we found a keyword then continue to the next char
                if( found )
                    goto for_loop_end;
            }

            if( kind&SYNTAX_WORD )
                set_syntax_at( pos, SYNTAX_WORD );
            else
                set_syntax_at( pos, SYNTAX_DULL );
            break;

        case st_string:
        {
            set_syntax_at( pos, string->s_kind );
            // if the next char is not quoted
            if( syntax_at_is( pos-1, SYNTAX_PREFIX_QUOTE ) )
                set_syntax_at( pos, string->s_kind );
            else
                if( char_at_is( pos, SYNTAX_PREFIX_QUOTE ) )
                    set_syntax_at( pos, string->s_kind|SYNTAX_PREFIX_QUOTE );
                else
                {
                    int len = string->looking_at_match( pos );
                    if( len > 0 )
                    {
                        state = st_simple;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, string->s_kind );
                        pos--;
                    }
                }
        }
            break;

        case st_comment:
        {
            set_syntax_at( pos, comment->s_kind );
            int len = comment->looking_at_match( pos );
            if( len > 0 )
            {
                state = st_simple;

                for( int i=0; i<len; i++, pos++ )
                    set_syntax_at( pos, comment->s_kind );
                pos--;
            }
        }
            break;
        default:
            debug_invoke();
        }
        for_loop_end: ;
    }

    // emacs_check_malloc_block( b_syntax_base );

    s->syntax_valid = limit;
    return true;
}

void EmacsBuffer::syntax_update_buffer( int pos, int len )
{
    syntax_buffer_data *s = &b_syntax;
    enum syn_states state = st_simple;
    int required = pos + len;

    if( !b_mode.md_syntax_array
    || s->syntax_base == NULL )
        return;

    if( s->syntax_valid > unrestrictedSize() )
        s->syntax_valid = unrestrictedSize();

    //
    // backup to a dull character
    // as that ensures that we can
    // figure out the rest of the syntax
    // unambiguosly
    //
    for( pos=pos-1-SYNTAX_STRING_SIZE; pos > 1; pos-- )
    {
        if( syntax_at(pos) == SYNTAX_DULL )
            break;
    }

    if( pos < 1 )
        pos = 1;

    SyntaxStringList_t::iterator comment;
    SyntaxStringList_t::iterator string;
    SyntaxStringList_t::iterator end;

    int limit = s->syntax_valid;
    for( ; pos<=limit; pos++ )
    {
        EmacsChar_t c = char_at( pos );
        SyntaxKind_t kind = b_mode.md_syntax->getSyntaxKind( c );

        if( c == '\n' )
            cant_1line_opt = 1;

        // emacs_check_malloc_block( b_syntax_base );

#if DEBUG_SYNTAX
        _dbg_msg( FormatString("state: %s, pos: %d, s_kind: %s, c: %C, s: %s")
                << state_to_string( state )
                << pos
                << syntax_bits_as_string( kind )
                << c
                << syntax_bits_as_string( syntax_at( pos ) ) );
#endif
        switch( state )
        {
        case st_simple:
            if( pos > required
            && (syntax_at( pos )&SYNTAX_MULTI_CHAR_TYPES) == 0 )
                return;

            if( kind&SYNTAX_COMMENT_MASK )
            {
                for( comment = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                         end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        comment != end;
                            ++comment )
                {
                    int len;
                    if( (comment->s_kind&SYNTAX_COMMENT_MASK) != 0
                    && (len = comment->looking_at_main( pos )) > 0 )
                    {
                        state = st_comment;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, comment->s_kind );

                        pos--;    // incremented at the top of the loop
                        goto for_loop_end;
                    }
                }
            }

            if( kind&SYNTAX_STRING_MASK )
            {
                for( string = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                        end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        string != end;
                            ++string )
                {
                    int len;
                    if( (string->s_kind&SYNTAX_STRING_MASK) != 0
                    && (len = string->looking_at_main( pos )) > 0 )
                    {
                        state = st_string;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, string->s_kind );

                        pos--;    // incremented at the top of the loop
                        goto for_loop_end;
                    }
                }
            }

            //
            // a keyword may start each time a word to nonword OR nonword to word boundary is crossed
            //
            if( (kind&SYNTAX_KEYWORD_MASK) != 0
            && !(((kind&SYNTAX_WORD) != 0) && ((syntax_at(pos-1)&SYNTAX_WORD) != 0)) )
            {
                bool found = false;
                for( SyntaxStringList_t::iterator keyword = b_mode.md_syntax->getSyntaxStrings( c ).begin(),
                                                      end = b_mode.md_syntax->getSyntaxStrings( c ).end();
                        keyword != end;
                            ++keyword )
                {
                    if( keyword->s_kind&SYNTAX_KEYWORD_MASK )
                    {
                        bool last_is_word = keyword->last_is_word( *b_mode.md_syntax );

                        if( pos + keyword->s_main_str.length() <= unrestrictedSize()
                        && !(last_is_word && char_at_is( pos+keyword->s_main_str.length(), SYNTAX_WORD ))
                        && keyword->looking_at_main( pos ) )
                        {
                            for( int i=0; i<keyword->s_main_str.length(); i++, pos++ )
                                set_syntax_at( pos, (EmacsChar_t)keyword->s_kind );
                            pos--;

                            found = true;
                            break;
                        }
                    }
                }
                // if we found a keyword then continue to the next char
                if( found )
                    goto for_loop_end;
            }

            if( kind&SYNTAX_WORD )
                set_syntax_at( pos, SYNTAX_WORD );
            else
                set_syntax_at( pos, SYNTAX_DULL );
            break;

        case st_string:
        {
            set_syntax_at( pos, string->s_kind );
            // if the next char is not quoted
            if( syntax_at_is( pos-1, SYNTAX_PREFIX_QUOTE ) )
                set_syntax_at( pos, string->s_kind );
            else
                if( char_at_is( pos, SYNTAX_PREFIX_QUOTE ) )
                    set_syntax_at( pos, string->s_kind|SYNTAX_PREFIX_QUOTE );
                else
                {
                    int len = string->looking_at_match( pos );
                    if( len > 0 )
                    {
                        state = st_simple;

                        for( int i=0; i<len; i++, pos++ )
                            set_syntax_at( pos, string->s_kind );
                        pos--;
                    }
                }
        }
            break;

        case st_comment:
        {
            set_syntax_at( pos, comment->s_kind );
            int len = comment->looking_at_match( pos );
            if( len > 0 )
            {
                state = st_simple;

                for( int i=0; i<len; i++, pos++ )
                    set_syntax_at( pos, comment->s_kind );
                pos--;
            }
        }
            break;
        default:
            debug_invoke();
        }
        for_loop_end: ;
    }
}
