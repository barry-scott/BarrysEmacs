//
//    Copyright (c) 1995-2010 Barry A. Scott
//

//
//
//    EmacsString
//
//
#include <emacsutl.h>

#include <string.h>

class EmacsString;
class EmacsStringRepresentation;


class EmacsString : public EmacsObject
{
    friend class EmacsInitialisation;
public:
    EMACS_OBJECT_FUNCTIONS( EmacsString )
    enum string_type
    {
        copy,       // make a copy of the string and free on delete
        free,       // free the malloc'ed string
        keep        // do not free the string
    };

    static const EmacsString null;      // always the empty string

    EmacsString( void );
    EmacsString( const char *string );
    EmacsString( const unsigned char *string );
    EmacsString( const wchar_t *string );
    EmacsString( const wchar_t *string, int length );

    EmacsString( enum string_type type );
    EmacsString( enum string_type type, const char *string );
    EmacsString( enum string_type type, const char *string, int length );
    EmacsString( enum string_type type, const unsigned char *string );
    EmacsString( enum string_type type, const unsigned char *string, int length );
    EmacsString( enum string_type type, const EmacsChar_t *string );
    EmacsString( enum string_type type, const EmacsChar_t *string, int length );

    EmacsString( const std::string &string );
    EmacsString( const EmacsString &string );

    EmacsString( EmacsStringRepresentation *rep );

    virtual ~EmacsString();

    EmacsString &operator=( const char *string )
        { return operator=( EmacsString( string ) ); }
    EmacsString &operator=( const unsigned char *string )
        { return operator=( EmacsString( string ) ); }
    EmacsString &operator=( const EmacsString &string );

    //
    //    relational operators
    //
    bool operator==( const EmacsString &str2 ) const
        { return compare( str2 ) == 0; }
    bool operator==( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) == 0; }
    bool operator==( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) == 0; }

    bool operator!=( const EmacsString &str2 ) const
        { return compare( str2 ) != 0; }
    bool operator!=( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) != 0; }
    bool operator!=( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) != 0; }

    bool operator>( const EmacsString &str2 ) const
        { return compare( str2 ) > 0; }
    bool operator>( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) > 0; }
    bool operator>( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) > 0; }

    bool operator>=( const EmacsString &str2 ) const
        { return compare( str2 ) >= 0; }
    bool operator>=( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) >= 0; }
    bool operator>=( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) >= 0; }

    bool operator<( const EmacsString &str2 ) const
        { return compare( str2 ) < 0; }
    bool operator<( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) < 0; }
    bool operator<( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) < 0; }

    bool operator<=( const EmacsString &str2 ) const
        { return compare( str2 ) <= 0; }
    bool operator<=( const char *str2 ) const
        { return compare( EmacsString( str2 ) ) <= 0; }
    bool operator<=( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ) <= 0; }

    //
    //    casts
    //
    operator const unsigned char *();
    operator const char *();

    //
    //    description of the data
    //
    const EmacsChar_t *unicode_data() const; // unsigned char data

    const unsigned char *data() const;  // unsigned char data
    const char *sdata() const;          // signed char data
                                        // these two function give unsafe access to the inside of representation

    int length() const;
    int isNull() const
        { return length() == 0; }

    //
    //    searching primitives
    //
    int first( char ch, int start_pos=0 ) const;                // index of first ch in string
    int first( unsigned char ch, int start_pos=0 ) const;       // index of first ch in string
    int first( EmacsChar_t ch, int start_pos=0 ) const;         // index of first ch in string

    int last( char ch, int start_pos=0 ) const                  // index of last ch in string
    {
        return last( (EmacsChar_t)ch, start_pos );
    }
    int last( unsigned char ch, int start_pos=0 ) const         // index of last ch in string
    {
        return last( (EmacsChar_t)ch, start_pos );
    }
    int last( EmacsChar_t ch, int start_pos=0 ) const;          // index of last ch in string

    int index( char ch, int start_pos=0 ) const;                // find the first ch starting at pos
    int index( unsigned char ch, int start_pos=0 ) const;       // find the first ch starting at pos
    int index( EmacsChar_t ch, int start_pos=0 ) const;         // find the first ch starting at pos
    int index( const EmacsString &str, int start_pos=0 ) const; // find the first str starting at pos

    int commonPrefix( const EmacsString &str ) const;           // length of common prefix case sensitive
    int caseBlindCommonPrefix( const EmacsString &str ) const;  // length of common prefix case blind

    //
    //    string modifiers
    //
    void remove( int position, int length );                    // remove the chars from position for length chars
    void remove( int position );                                // remove from position to the end of the string

    enum { string_growth_room=32 };                             // amount of space to allow for growth

    EmacsString &append( char ch )
        { return append( EmacsString( copy, &ch, 1 ) ); }
    EmacsString &append( unsigned char ch )
        { return append( EmacsString( copy, &ch, 1 ) ); }
    EmacsString &append( int ch )
        { return append( (EmacsChar_t)ch ); }
    EmacsString &append( EmacsChar_t ch )
    {
        EmacsChar_t str[1];
        str[0] = ch;
        return append( 1, str );
    }

    EmacsString &append( const char *str )
        { return append( EmacsString( str ) ); }
    EmacsString &append( const unsigned char *str )
        { return append( EmacsString( str ) ); }
    EmacsString &append( int len, const char *str )
        { return append( EmacsString( copy, str, len ) ); }
    EmacsString &append( int len, const unsigned char *str )
        { return append( EmacsString( copy, str, len ) ); }
    EmacsString &append( const EmacsString &str )
        { return append( str.length(), str.unicode_data() ); }

    EmacsString &append( int length, const EmacsChar_t *data );

    EmacsString &insert( int pos, char ch )
        { return insert( pos, EmacsString( copy, &ch, 1 ) ); }
    EmacsString &insert( int pos, unsigned char ch )
        { return insert( pos, EmacsString( copy, &ch, 1 ) ); }
    EmacsString &insert( int pos, EmacsChar_t ch )
        { return insert( pos, ch ); }
    EmacsString &insert( int pos, int ch )
        {
            EmacsChar_t str[1];
            str[0] = ch;
            return insert( pos, 1, str );
        }

    EmacsString &insert( int pos, const char *str )
        { return insert( pos, EmacsString( str ) ); }
    EmacsString &insert( int pos, const unsigned char *str )
        { return insert( pos, EmacsString( str ) ); }
    EmacsString &insert( int pos, const EmacsString &str )
        { return insert( pos, str.length(), str.unicode_data() ); }

    EmacsString &insert( int pos, int length, const EmacsChar_t *data );

    EmacsString operator ()( int first, int last ) const;       // return a substring

    EmacsString &toLower();
    EmacsString &toUpper();

    inline EmacsChar_t operator[]( int index ) const;           // read only
    inline EmacsChar_t &operator[]( int index );                // read write

    int compare( const char *str2 ) const
        { return compare( EmacsString( str2 ) ); }
    int compare( const unsigned char *str2 ) const
        { return compare( EmacsString( str2 ) ); }
    int compare( const EmacsString &str2 ) const;

    int caseBlindCompare( const char *str2 ) const
        { return caseBlindCompare( EmacsString( str2 ) ); }
//    int caseBlindCompare( const EmacsChar_t *str2 ) const
//        { return caseBlindCompare( EmacsString( str2 ) ); }
    int caseBlindCompare( const EmacsString &str2 ) const;

private:
    static void init();

    void copy_on_write();

    EmacsStringRepresentation *_rep;

    static EmacsStringRepresentation *empty_string;

    static EmacsStringRepresentation *bad_value;
    inline void check_for_bad_value( EmacsStringRepresentation *rep );

public:
    void q() const;   // print string for debug
};

class EmacsStringRepresentation : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsStringRepresentation )
    EmacsStringRepresentation
        (
        enum EmacsString::string_type _type,
        int _alloc_length,
        int _length,
        const unsigned char *_data
        );
    EmacsStringRepresentation
        (
        enum EmacsString::string_type _type,
        int _alloc_length,
        int _length,
        const EmacsChar_t *_data
        );
    EmacsStringRepresentation
        (
        enum EmacsString::string_type _type,
        int _alloc_length,
        int _length,
        const wchar_t *_data
        );
    virtual ~EmacsStringRepresentation();

    friend class EmacsString;

private:
    const unsigned char *get_utf8_data();

    int ref_count;
    enum EmacsString::string_type type;
    int alloc_length;
    int length;
    EmacsChar_t *data;
    unsigned char *utf8_data;
};

inline void EmacsString::check_for_bad_value( EmacsStringRepresentation *rep )
{
    if( rep->ref_count < 1 )
        debug_invoke();
    if( rep == bad_value )
        debug_invoke();

//
//    if( _rep->length > 0 )
//        emacs_assert( _rep->data[0] <= 255 );
}

inline EmacsChar_t EmacsString::operator[]( int index ) const
{
    // allow negative index to be relative to the end of the string
    if( index < 0 )
        index += _rep->length;

    emacs_assert( index >= 0 );
    emacs_assert( index < _rep->length );

    return _rep->data[index];
}

inline EmacsChar_t &EmacsString::operator[]( int index )
{
    // assume that the only reason to return a ref is to write to the buffer
    copy_on_write();

    // allow negative index to be relative to the end of the string
    if( index < 0 )
        index += _rep->length;
    emacs_assert( index >= 0 );
    emacs_assert( index < _rep->length );

    return _rep->data[index];
}

class FormatString : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( FormatString )
    FormatString( EmacsString _format );
    ~FormatString() {}

    operator const EmacsString() const;

    FormatString &operator <<( int v );
    FormatString &operator <<( const EmacsString & );
    FormatString &operator <<( const EmacsString * );
    FormatString &operator <<( const char * );
    FormatString &operator <<( const EmacsChar_t * );

private:
    enum arg_type { argNone, argString, argInt };

    void process_format();
    EmacsChar_t next_format_char();

    void put( EmacsChar_t );
    void put( const EmacsChar_t *chars, unsigned int len );

    void print_decimal( long int );
    void print_hexadecimal( long int );
    void print_octal( long int );
    void print_string( const EmacsString &str );

    EmacsString format;
    EmacsString result;

    arg_type next_arg_type;
    arg_type next_width_type;
    arg_type next_precision_type;
    EmacsChar_t format_char;
    EmacsChar_t pad_char;

    int width;
    int precision;
    int left_justify;

    int next_format_char_index;

    int intArg;
    EmacsString stringArg;
    // keep some compilers for complaining (C Set++)
    FormatString( const FormatString &other );
};

class EmacsStringIterator
{
public:
    EmacsStringIterator( const EmacsString &string, int index=0 );
    virtual ~EmacsStringIterator();

    bool next( char &ch );
private:
    const EmacsString string;
    int index;
    int remaining;
};
