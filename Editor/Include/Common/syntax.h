//    Copyright (c) 1982-1998
//        Barry A. Scott
// Declarations having to do with Emacs syntax tables
// A syntax table contains an array of information, one entry per ASCII
// character.


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
// THe syntax byte is divided into two groups:
// 1) bits below SYNTAX_FIRST_FREE used to color the display of data
// 2) bits from SYNTAX_FIRST_FREE and up used to hold state
//
// ?! Need one set of bits for the syntax table data and
// a second set for the marking of characters in the buffer.
//
// SYNTAX_PREFIX_QUOTE\|SYNTAX_BEGIN_PAREN\|SYNTAX_END_PAREN\|SYNTAX_FIRST_FREE\|SYNTAX_LAST_BIT

#define B(n) (1 << (n))        // bit n

const int SYNTAX_DULL        (        0  );        // 00 a dull (punctuation) character
const int SYNTAX_WORD        (      B(1) );        // 02 a word character for ESC-F and friends

const int SYNTAX_STRING_SHIFT    (        2  );
const int SYNTAX_STRING_MASK    ( B(2)+B(3) );        // 0c its a string
const int SYNTAX_TYPE_STRING1    ( B(2)      );        // 04 its a string type 1
const int SYNTAX_TYPE_STRING2    (      B(3) );        // 08 its a string type 2
const int SYNTAX_TYPE_STRING3    ( B(2)+B(3) );        // 0c its a string type 3

const int SYNTAX_COMMENT_SHIFT    (        4  );
const int SYNTAX_COMMENT_MASK    ( B(4)+B(5) );        // 30 its a comment
const int SYNTAX_TYPE_COMMENT1    ( B(4)      );        // 10 its a comment type 1
const int SYNTAX_TYPE_COMMENT2    (      B(5) );        // 20 its a comment type 2
const int SYNTAX_TYPE_COMMENT3    ( B(4)+B(5) );        // 30 its a comment type 3

const int SYNTAX_KEYWORD_SHIFT    (        6  );
const int SYNTAX_KEYWORD_MASK    ( B(6)+B(7) );        // c0 its a keyword
const int SYNTAX_TYPE_KEYWORD1    ( B(6)      );        // 40 its a keyword type 1
const int SYNTAX_TYPE_KEYWORD2    (      B(7) );        // 80 its a keyword type 2
const int SYNTAX_TYPE_KEYWORD3    ( B(6)+B(7) );        // c0 its a keyword type 3

const int SYNTAX_FIRST_FREE    (      B(8) );        // 100
const int SYNTAX_PREFIX_QUOTE    (      B(8) );        // 100 like \ in C
const int SYNTAX_BEGIN_PAREN    (      B(9) );        // 200 a begin paren: (<[
const int SYNTAX_END_PAREN    (     B(10) );        // 400 an end paren: )>]    end
const int SYNTAX_LAST_BIT    (     B(10) );

#undef B

const int SYNTAX_MULTI_CHAR_TYPES
    (
    SYNTAX_WORD|SYNTAX_KEYWORD_MASK|SYNTAX_COMMENT_MASK|SYNTAX_STRING_MASK
    );

const int SYNTAX_PROP_CASE_FOLD_MATCH( 1 );    // case blind matching of this entry
const int SYNTAX_PROP_REGEX_MATCH( 2 );        // entry is a regular expression
const int SYNTAX_PROP_PAIRED( 4 );        // entry is a paired value (used for string)

typedef int SyntaxData_t;
typedef int SyntaxDepth_t;
typedef int SyntaxTable_t;

struct SyntaxCharData_t
{
    unsigned int data        : 12;
    unsigned int outline_visible    :  4;
    unsigned int table_number    :  8;
    unsigned int outline_depth    :  8;
};

//
//    syntax_string contains the following information for
//    the various kinds of syntax.
//    Kind        Main        Match
//    begin Paren    "("        ")"
//    end paren    ")"        "("
//    keyword[12]    "keyword"    ""
//    comment        "//"        "\n"
//
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

    int s_kind;
    int s_properties;
    SyntaxString *s_next;
    SyntaxString *s_alt_matching;

    int s_main_len;
    EmacsChar_t s_main_str[SYNTAX_STRING_SIZE+1];

    int s_match_len;
    EmacsChar_t s_match_str[SYNTAX_STRING_SIZE+1];
private:
    int looking_at_internal( int pos, int len, const unsigned char *str ) const;
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
    { EmacsStringTable::add( key, value ); }
    SyntaxTable *remove( const EmacsString &key )
    { return (SyntaxTable *)EmacsStringTable::remove( key ); }
    SyntaxTable *find( const EmacsString &key )
    { return (SyntaxTable *)EmacsStringTable::find( key ); }
    SyntaxTable *value( int index )
    { return (SyntaxTable *)EmacsStringTable::value( index ); }
};

class SyntaxTable : public EmacsObject
{
public:
    struct entry
    {
        int s_kind;
        SyntaxString *s_strings;
    };

    EMACS_OBJECT_FUNCTIONS( SyntaxTable )
    SyntaxTable( const EmacsString &name );
    virtual ~SyntaxTable();

    static SyntaxTable *find( const EmacsString &name )
    { return name_table.find( name ); }

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
    { return name_table.get_esc_word_mlisp( result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, EmacsString::null, result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, default_value, result ); }

    void modify_table
        (
        int type, int properties,
        const EmacsString &str1, const EmacsString &str2
        );

    EmacsString s_name;
    struct entry s_table [256];

    static SyntaxNameTable name_table;
private:
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
    void modify_table_dull_type        // Dull type
        ( const EmacsString &str1 )
        throw( SyntaxErrorException, SyntaxMemoryException );

    void modify_table_range_type        // range of characters type like word A-Z
        ( int type, const EmacsString &str1, void (SyntaxTable::*set_func)( int type, int ch ) )
        throw( SyntaxErrorException, SyntaxMemoryException );

    void modify_table_paired_type        // paired strings like comments // to
        ( int type, int properties, const EmacsString &str1, const EmacsString &str2  )
        throw( SyntaxErrorException, SyntaxMemoryException );

    void modify_table_set_simple_type( int type, int ch );
    void modify_table_set_paired_type( int type, int ch );

    // add any type of syntax string
    void add_syntax_string_to_table( int ch, SyntaxString *str );

    // add a paired syntax string to the table
    void add_paired_syntax_string_to_table( int ch, SyntaxString *str );
};
