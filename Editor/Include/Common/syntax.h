//    Copyright (c) 1982-2020
//        Barry A. Scott
//
// Declarations having to do with Emacs syntax tables
// A syntax table contains an array of information, one entry per ASCII
// character.
//
#include <map>
#include <list>

// Syntax data for a char splits between alternatives and invariant
// types.
// Alternatives
//    Dull
//    Keywords 1,2,3
//    String
//    Comment
// Invariant
//    Word

enum Syntax_Alternatives
{
    Syntax_Alt_Dull,
    Syntax_Alt_Keyword1,
    Syntax_Alt_Keyword2,
    Syntax_Alt_Keyword3,
    Syntax_Alt_String,
    Syntax_Alt_Comment
};

enum Syntax_Invariant
{
    Syntax_Mask_Type = 0x07,
    Syntax_Mask_Word = 0x08,
    Syntax_Mask_Table = 0x30,
    Syntax_Mask_Reserved = 0xc0
};


//
// The syntax byte is divided into two groups:
// 1) bits below SYNTAX_FIRST_FREE used to color the display of data
// 2) bits from SYNTAX_FIRST_FREE and up used to hold state
//
// ?! Need one set of bits for the syntax table data and
// a second set for the marking of characters in the buffer.
//
// SYNTAX_PREFIX_QUOTE\|SYNTAX_BEGIN_PAREN\|SYNTAX_END_PAREN\|SYNTAX_FIRST_FREE\|SYNTAX_LAST_BIT
typedef int SyntaxKind_t;

#define B(n) (1 << (n))        // bit n

const SyntaxKind_t SYNTAX_DULL              (        0  );      // 00 a dull (punctuation) character
const SyntaxKind_t SYNTAX_WORD              (      B(1) );      // 02 a word character for ESC-F and friends

const SyntaxKind_t SYNTAX_STRING_SHIFT      (        2  );
const SyntaxKind_t SYNTAX_STRING_MASK       ( B(2)+B(3) );      // 0c its a string
const SyntaxKind_t SYNTAX_TYPE_STRING1      ( B(2)      );      // 04 its a string type 1
const SyntaxKind_t SYNTAX_TYPE_STRING2      (      B(3) );      // 08 its a string type 2
const SyntaxKind_t SYNTAX_TYPE_STRING3      ( B(2)+B(3) );      // 0c its a string type 3

const SyntaxKind_t SYNTAX_COMMENT_SHIFT     (        4  );
const SyntaxKind_t SYNTAX_COMMENT_MASK      ( B(4)+B(5) );      // 30 its a comment
const SyntaxKind_t SYNTAX_TYPE_COMMENT1     ( B(4)      );      // 10 its a comment type 1
const SyntaxKind_t SYNTAX_TYPE_COMMENT2     (      B(5) );      // 20 its a comment type 2
const SyntaxKind_t SYNTAX_TYPE_COMMENT3     ( B(4)+B(5) );      // 30 its a comment type 3

const SyntaxKind_t SYNTAX_KEYWORD_SHIFT     (        6  );
const SyntaxKind_t SYNTAX_KEYWORD_MASK      ( B(6)+B(7) );      // c0 its a keyword
const SyntaxKind_t SYNTAX_TYPE_KEYWORD1     ( B(6)      );      // 40 its a keyword type 1
const SyntaxKind_t SYNTAX_TYPE_KEYWORD2     (      B(7) );      // 80 its a keyword type 2
const SyntaxKind_t SYNTAX_TYPE_KEYWORD3     ( B(6)+B(7) );      // c0 its a keyword type 3

const SyntaxKind_t SYNTAX_TYPE_PROBLEM      (      B(8) );      // 100

const SyntaxKind_t SYNTAX_FIRST_FREE        (      B(9) );      // 200
const SyntaxKind_t SYNTAX_PREFIX_QUOTE      (      B(9) );      // 200 like \ in C
const SyntaxKind_t SYNTAX_BEGIN_PAREN       (     B(10) );      // 400 a begin paren: (<[
const SyntaxKind_t SYNTAX_END_PAREN         (     B(11) );      // 800 an end paren: )>]    end
const SyntaxKind_t SYNTAX_LAST_BIT          (     B(11) );

#undef B

const SyntaxKind_t SYNTAX_MULTI_CHAR_TYPES
    (
    SYNTAX_WORD|SYNTAX_KEYWORD_MASK|SYNTAX_COMMENT_MASK|SYNTAX_STRING_MASK|SYNTAX_TYPE_PROBLEM
    );

const int SYNTAX_PROP_CASE_FOLD_MATCH( 1 );     // case blind matching of this entry
const int SYNTAX_PROP_REGEX_MATCH( 2 );         // entry is a regular expression
const int SYNTAX_PROP_PAIRED( 4 );              // entry is a paired value (used for string)
const int SYNTAX_PROP_CHAR( 8 );                // string is single character long
typedef int SyntaxData_t;

typedef short int SyntaxCharData_t;

//
//    syntax_string contains the following information for
//    the various kinds of syntax.
//    Kind          Main        Match
//    begin Paren   "("         ")"
//    end paren     ")"         "("
//    keyword[12]   "keyword"   ""
//    comment       "//"        "\n"
//
class SyntaxString;
typedef std::list< SyntaxString > SyntaxStringList_t;

const int SYNTAX_STRING_SIZE( 31 );
class SyntaxString : public EmacsObject
{
public:
    SyntaxString();
    SyntaxString
        (
        int _kind, int _properties,
        const EmacsString &_main, const EmacsString &_match
        );
    SyntaxString( const SyntaxString &old );
    ~SyntaxString();
    EMACS_OBJECT_FUNCTIONS( SyntaxString )
    SyntaxString &operator=( const SyntaxString &old );

    int looking_at_main( int pos ) const;
    int looking_at_match( int pos ) const;

    bool first_is_word() const;
    bool last_is_word( const SyntaxTable &table ) const;

    SyntaxKind_t s_kind;
    int s_properties;
    SyntaxStringList_t s_alt_matching;

    EmacsString s_main_str;
    EmacsString s_match_str;
    EmacsSearch s_main_ere;

private:
    int looking_at_internal( int pos, const EmacsString &str, bool is_main ) const;
};


class SyntaxNameTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( SyntaxNameTable )
    SyntaxNameTable( int init_size, int grow_amount )
    : EmacsStringTable( init_size, grow_amount )
    { }
    virtual ~SyntaxNameTable()
    { }

    void add( const EmacsString &key, SyntaxTable *value )
    {
        EmacsStringTable::add( key, value );
    }
    SyntaxTable *remove( const EmacsString &key )
    {
        return (SyntaxTable *)EmacsStringTable::remove( key );
    }
    SyntaxTable *find( const EmacsString &key )
    {
        return (SyntaxTable *)EmacsStringTable::find( key );
    }
    SyntaxTable *value( int index )
    {
        return (SyntaxTable *)EmacsStringTable::value( index );
    }
};


class SyntaxTable : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( SyntaxTable )
    SyntaxTable( const EmacsString &name );
    SyntaxTable( const EmacsString &name, const SyntaxTable &other );
    virtual ~SyntaxTable();

    void q( void );

    static SyntaxTable *find( const EmacsString &name )
    {
        return name_table.find( name );
    }

    // return one of the keys in the table otherwise NULL
    static SyntaxTable * get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static SyntaxTable *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }

    static SyntaxTable *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

    // return one of the keys in the table otherwise NULL
    static EmacsString &get_esc_word_mlisp( EmacsString &result )
    {
        return name_table.get_esc_word_mlisp( result );
    }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
    {
        return name_table.get_esc_word_interactive( prompt, EmacsString::null, result );
    }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
    {
        return name_table.get_esc_word_interactive( prompt, default_value, result );
    }

    void modify_table( int type, int properties, const EmacsString &str1, const EmacsString &str2 );

    SyntaxKind_t getSyntaxKind( EmacsChar_t ch ) const;
    void eraseSyntaxKind( EmacsChar_t ch );

    bool hasSyntaxStrings( EmacsChar_t ch ) const;
    const SyntaxStringList_t &getSyntaxStrings( EmacsChar_t ch ) const;
    SyntaxStringList_t &getSyntaxStrings( EmacsChar_t ch );
    void eraseSyntaxStrings( EmacsChar_t ch );

    EmacsString s_name;
    static SyntaxNameTable name_table;
private:
    typedef std::map< EmacsChar_t, SyntaxKind_t >       EmacsCharToSyntaxKind_t;
    typedef std::map< EmacsChar_t, SyntaxStringList_t > EmacsCharToSyntaxString_t;

    EmacsCharToSyntaxKind_t     s_kind;
    EmacsCharToSyntaxString_t   s_strings;

    class SyntaxException
    {
    public:
    };

    class SyntaxErrorException : public SyntaxException
    {
    public:
    };

    class SyntaxMemoryException : public SyntaxException
    {
    public:
    };

    // handlers used by modify_table to deal with type of syntax entry

    // Dull type
    void modify_table_dull_type( const EmacsString &str1 );

    // range of characters type like word A-Z
    void modify_table_range_type( int type, int properties, const EmacsString &str1,
                                  void (SyntaxTable::*set_func)( int type, int properties, int ch ) );

    // paired strings like comments // to
    void modify_table_paired_type( int type, int properties, const EmacsString &str1, const EmacsString &str2  );

    void modify_table_set_simple_type( int type, int properties, int ch );
    void modify_table_set_paired_type( int type, int properties, int ch );

    // add any type of syntax string
    void add_syntax_string_to_table( int ch, const SyntaxString &str );

    // add a paired syntax string to the table
    void add_paired_syntax_string_to_table( int ch, const SyntaxString &str );
};
