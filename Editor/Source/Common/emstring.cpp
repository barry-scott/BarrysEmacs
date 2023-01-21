//
//    Copyright (c) 1995-2016 Barry A. Scott
//
#include <emacsutl.h>
#include <emobject.h>
#include <emstring.h>
#include <cassert>
#include <emunicode.h>

#include <iostream>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

//
//
//    EmacsString implementation
//
//
EmacsStringRepresentation *EmacsString::bad_value = (EmacsStringRepresentation *)0x4;

EmacsStringRepresentation *EmacsString::empty_string;

//
//    This routine contains all the initialisation
//    that must happen before any static objects are
//    init'ed
//
void EmacsString::init(void)
{
    // emacs strings rely on the empty_string being setup
    empty_string = EMACS_NEW EmacsStringRepresentation( keep, 0, 0, (EmacsChar_t *)"" );
    // empty_string must never go away
    empty_string->ref_count += 2;
    // other classes rely on null being setup
    const EmacsString *null_ref = &null;
    EmacsString *writeable_null_ref = (EmacsString *)null_ref;
    writeable_null_ref->_rep = empty_string;
}

const EmacsString EmacsString::null;

void EmacsString::q() const
{
    EmacsString qstr( *this );

    std::cout << "EmacsString length " << qstr.length() << " utf8 \"" << qstr.sdata() << "\"" << std::endl;
    std::cout << "            data " << std::hex;
    for( int i=0; i<qstr._rep->length; i++ )
    {
        std::cout << " 0x" << qstr._rep->data[i];
    }
    std::cout << std::dec << std::endl;
}

EmacsString::EmacsString( void )
: _rep( empty_string )
{
    check_for_bad_value( _rep );
    _rep->ref_count++;
}

#define char_strlen( str ) static_cast<int>( strlen( str ) )

EmacsString::EmacsString( const char *string )
: _rep( EMACS_NEW EmacsStringRepresentation( keep, 0, char_strlen( string ), (const unsigned char *)string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( const unsigned char *string )
: _rep( EMACS_NEW EmacsStringRepresentation( keep, 0, char_strlen( (const char *)string ), string ) )
{
    check_for_bad_value( _rep );
}

#if defined( WIN32 )
EmacsString::EmacsString( const wchar_t *string, int length )
: _rep( EMACS_NEW EmacsStringRepresentation( copy, 0, length, string ) )
{
    check_for_bad_value( _rep );
}
#endif

#if defined( PYBEMACS )
EmacsString::EmacsString( const Py::String &str )
: _rep( NULL )
{
    Py::ucs4string ucs4( str.as_ucs4string() );
    _rep = EMACS_NEW EmacsStringRepresentation( copy, 0, ucs4.size(), reinterpret_cast<const EmacsChar_t *>( ucs4.data() ) );
}
#endif

static int EmacsChar_strlen( const EmacsChar_t *string )
{
    const EmacsChar_t *p = string;

    while( *p != 0 )
        p++;

    return static_cast<int>( p - string );
}

EmacsString::EmacsString( const EmacsChar_t *string )
: _rep( EMACS_NEW EmacsStringRepresentation( copy, 0, EmacsChar_strlen( string ), string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( const EmacsChar_t *string, int length )
: _rep( EMACS_NEW EmacsStringRepresentation( copy, 0, length, string ) )
{
    check_for_bad_value( _rep );
}


EmacsString::EmacsString( enum string_type type )
: _rep( EMACS_NEW EmacsStringRepresentation( type, 0, 0, u_str("") ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const unsigned char *string )
: _rep( EMACS_NEW EmacsStringRepresentation( type, char_strlen( (const char *)string )+1, char_strlen( (const char *)string ), string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const unsigned char *string, int length )
: _rep( EMACS_NEW EmacsStringRepresentation( type, length+1, length, string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const EmacsChar_t *string )
: _rep( EMACS_NEW EmacsStringRepresentation( type, EmacsChar_strlen( string )+1, EmacsChar_strlen( string ), string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const EmacsChar_t *string, int length )
: _rep( EMACS_NEW EmacsStringRepresentation( type, length+1, length, string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const char *string )
: _rep( EMACS_NEW EmacsStringRepresentation( type, char_strlen( (const char *)string )+1, char_strlen( (const char *)string ), (unsigned char *)string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( enum string_type type, const char *string, int length )
: _rep( EMACS_NEW EmacsStringRepresentation( type, length+1, length, (unsigned char *)string ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( const std::string &string )
: _rep( EMACS_NEW EmacsStringRepresentation( copy, 0, string.length(), (unsigned char *)string.c_str() ) )
{
    check_for_bad_value( _rep );
}

EmacsString::EmacsString( const EmacsString &string )
: _rep( string._rep )
{
    check_for_bad_value( _rep );
    _rep->ref_count++;
}

EmacsString::EmacsString( EmacsStringRepresentation *rep )
: _rep( rep )
{
    check_for_bad_value( _rep );
}

EmacsString::~EmacsString()
{
    check_for_bad_value( _rep );
#if DBG_EMSTRING
    emacs_assert( _rep->ref_count > 0 );
#endif
    _rep->ref_count--;
    if( _rep->ref_count == 0 )
        delete _rep;
#if DBG_EMSTRING
    // if( this != &null )
        _rep = bad_value;
#endif
}

EmacsString &EmacsString::operator=( const EmacsString &string )
{
    check_for_bad_value( _rep );
    check_for_bad_value( string._rep );
    string._rep->ref_count++;

#if DBG_EMSTRING
    emacs_assert( _rep->ref_count > 0 );
#endif
    _rep->ref_count--;
    if( _rep->ref_count == 0 )
        delete _rep;

    _rep = string._rep;

    return *this;
}

int EmacsString::compare( const EmacsString &str2 ) const
{
    // is this a compare to self?
    if( _rep == str2._rep )
        return 0;    // yep then we match

    return unicode_strcmp( length(), unicode_data(), str2.length(), str2.unicode_data() );
}

int EmacsString::caseBlindCompare( const EmacsString &str2 ) const
{
    // is this a caseBlindCompare to self?
    if( _rep == str2._rep )
        return 0;    // yep then we match

    return unicode_stricmp( length(), unicode_data(), str2.length(), str2.unicode_data() );
}

EmacsString &EmacsString::append( int length, const EmacsChar_t *_data )
{
    if( _rep == NULL )
    {
        _rep = EMACS_NEW EmacsStringRepresentation( copy, string_growth_room, length, _data );
        check_for_bad_value( _rep );
    }
    else
    {
        copy_on_write();

        if( _rep->alloc_length < _rep->length + length + 1 )
        {
            _rep->alloc_length = _rep->length + length + string_growth_room;
            _rep->alloc_length |= 31;
            _rep->alloc_length++;

            _rep->data = reinterpret_cast<EmacsChar_t *>( EMACS_REALLOC( _rep->data, sizeof( EmacsChar_t )*(_rep->alloc_length), malloc_type_char ) );
        }
        memcpy( &_rep->data[ _rep->length ], _data, sizeof( EmacsChar_t )*length );
        _rep->length += length;
        _rep->data[ _rep->length ] = '\0';
    }

    check_for_bad_value( _rep );

    return *this;
}

EmacsString &EmacsString::insert( int pos, int length, const EmacsChar_t *_data )
{
    if( pos < 0 )
    {
        pos = _rep->length + pos;
        if( pos < 0 )
            pos = _rep->length;
    }
    if( pos > _rep->length )
        pos = _rep->length;

    if( _rep == NULL )
    {
        _rep = EMACS_NEW EmacsStringRepresentation( copy, string_growth_room, length, _data );
        check_for_bad_value( _rep );
    }
    else
    {
        copy_on_write();

        if( _rep->alloc_length < _rep->length + length + 1 )
        {
            _rep->alloc_length = _rep->length + length + string_growth_room;
            _rep->alloc_length |= 31;
            _rep->alloc_length++;

            _rep->data = reinterpret_cast<EmacsChar_t *>( EMACS_REALLOC( _rep->data, sizeof( EmacsChar_t )*(_rep->alloc_length), malloc_type_char ) );
        }

        memmove( &_rep->data[ pos + length ], &_rep->data[ pos ], sizeof( EmacsChar_t )*(_rep->length - pos) );
        memcpy( &_rep->data[ pos ], _data, sizeof( EmacsChar_t )*length );
        _rep->length += length;
        _rep->data[ _rep->length ] = '\0';
    }

    check_for_bad_value( _rep );

    return *this;
}

const EmacsChar_t *EmacsString::unicode_data() const
{
    return _rep->data;
}

#if defined( PYBEMACS )
Py::Object EmacsString::asPyString() const
{
    return Py::String( unicode_data(), length() );
}
#endif

EmacsString::operator const char *() const
{
    return (const char *)_rep->get_utf8_data();
}

EmacsString::operator const unsigned char *() const
{
    return _rep->get_utf8_data();
}

const char *EmacsString::sdata() const
{
    return (const char *)_rep->get_utf8_data();
}

const unsigned char *EmacsString::utf8_data() const
{
    return _rep->get_utf8_data();
}

int EmacsString::utf8_data_length() const
{
    _rep->get_utf8_data();
    return _rep->length_utf8_data;
}

#if defined(WIN32)
const wchar_t *EmacsString::utf16_data() const
{
    return _rep->get_utf16_data();
}

int EmacsString::utf16_data_length() const
{
    _rep->get_utf16_data();
    return _rep->length_utf16_data;
}
#endif

int EmacsString::length() const
{
    return _rep->length;
}

int EmacsString::first( char ch, int start_pos ) const
{
    return index( (EmacsChar_t)ch, start_pos );
}

int EmacsString::first( unsigned char ch, int start_pos ) const
{
    return index( (EmacsChar_t)ch, start_pos );
}

int EmacsString::index( char ch, int start_pos ) const        // find the first ch starting at pos
{
    return index( (EmacsChar_t)ch, start_pos );
}

int EmacsString::index( unsigned char ch, int start_pos ) const    // find the first ch starting at pos
{
    return index( char( ch ), start_pos );
}

int EmacsString::index( EmacsChar_t ch, int start_pos ) const        // find the first ch starting at pos
{
    // index of ch in string
    for( int i=start_pos; i<_rep->length; ++i )
        if( ch == _rep->data[ i ] )
            return i;

    return -1;
}

int EmacsString::index( const EmacsString &str, int start_pos ) const    // find the first str starting at pos
{
    if( str.isNull() )
        return -1;
    EmacsChar_t first_char = str[0];

    int last_pos = _rep->length - str._rep->length;
    int pos = start_pos;
    for(;;)
    {
        pos = index( first_char, pos );
        // cannot find the first char
        if( pos < 0 )
            return -1;
        // str cannot fit
        if( pos > last_pos )
            return -1;
        if( memcmp( str.unicode_data(), &unicode_data()[ pos ], sizeof( EmacsChar_t )*str.length()  ) == 0 )
            return pos;
        pos++;
    }
}

int EmacsString::last( EmacsChar_t ch, int start_pos ) const
{
    // index of ch in string
    if( _rep == 0 )
        return -1;

    // index of ch in string
    for( int i=_rep->length - 1; i>=start_pos; --i )
        if( ch == _rep->data[ i ] )
            return i;

    return -1;
}

bool EmacsString::startswith( const EmacsString &str ) const
{
    if( _rep->length < str._rep->length )
    {
        return 0;
    }

    return unicode_strcmp( str.length(), unicode_data(), str.length(), str.unicode_data() ) == 0;
}

bool EmacsString::endswith( const EmacsString &str ) const
{
    if( _rep->length < str._rep->length )
    {
        return 0;
    }

    int offset = _rep->length - str._rep->length;
    return unicode_strcmp( str.length(), unicode_data() + offset, str.length(), str.unicode_data() ) == 0;
}

//
//    returns the length of the prefix common to both strings
//
int EmacsString::commonPrefix( const EmacsString &str ) const
{
    int min_length = _rep->length;
    if( str._rep->length < min_length )
        min_length = str._rep->length;

    int prefix;
    for( prefix=0; prefix < min_length; prefix++ )
        if( _rep->data[prefix] != str._rep->data[prefix] )
            break;

    return prefix;
}

//
//    returns the length of the prefix common to both strings
//
int EmacsString::caseBlindCommonPrefix( const EmacsString &str ) const
{
    EmacsString us( *this );    us.toLower();
    EmacsString them( str );    them.toLower();

    return us.commonPrefix( them );
}


void EmacsString::remove( int position, int length )
{
    if( length == 0 )
        return;
    // remove the chars from position for length chars
    if( position > _rep->length )
        return;
    if( length > _rep->length )
        length = _rep->length;

    copy_on_write();

    if( position + length > _rep->length )
        length = _rep->length - position;

    memmove( (void *)&_rep->data[position], (void *)&_rep->data[position+length], (_rep->length - (position + length) + 1) * sizeof( EmacsChar_t ) );

    _rep->length -= length;
}

void EmacsString::remove( int position )
{
    // remove from position to the end of the string
    remove( position, INT_MAX );
}


void EmacsString::copy_on_write(void)
{
    // its writeable if this is the only reference
    if( _rep->ref_count == 1 && _rep->type == free )
    {
        if( _rep->utf8_data != NULL )
        {
            EMACS_FREE( _rep->utf8_data );
            _rep->utf8_data = NULL;
        }

#if defined(WIN32)
        if( _rep->utf16_data != NULL )
        {
            EMACS_FREE( _rep->utf16_data );
            _rep->utf16_data = NULL;
        }
#endif

        return;
    }
    // make a copy of the data
    EmacsStringRepresentation *new_rep = EMACS_NEW EmacsStringRepresentation( copy, 0, _rep->length, _rep->data );

    _rep->ref_count--;
    if( _rep->ref_count == 0 )
        delete _rep;

    _rep = new_rep;
}


EmacsString EmacsString::operator()( int first, int last ) const
{
    if( first < 0 )
        first = length() + first;
    if( first <= 0 )
        first = 0;
    else if( first > length() )
        first = length();

    if( last < 0 )
        last = length() + last;
    if( last <= 0 )
        last = 0;
    else if( last > length() )
        last = length();
    if( last > first )
        // return a substring
        return EmacsString( copy, &unicode_data()[ first ], last-first );

    return EmacsString::null;
}

EmacsString &EmacsString::toLower()
{
    copy_on_write();

    for( int i=0; i<_rep->length; i++ )
        if( unicode_is_upper( _rep->data[i] ) )
            // QQQ this is unicode safe
            _rep->data[i] = (EmacsChar_t)(_rep->data[i] + ('a' - 'A'));
    return *this;
}

EmacsString &EmacsString::toUpper()
{
    copy_on_write();

    for( int i=0; i<_rep->length; i++ )
        if( unicode_is_lower( _rep->data[i] ) )
            // QQQ this is unicode safe
            _rep->data[i] = (EmacsChar_t)(_rep->data[i] + ('A' - 'a'));
    return *this;
}

//================================================================================
//
//  EmacsStringRepresentation
//
//================================================================================
EmacsStringRepresentation::EmacsStringRepresentation
    (
    enum EmacsString::string_type _type,
    int _alloc_length,
    int _length,
    const unsigned char *_data
    )
: ref_count(1)
, type( _type )
, alloc_length( _alloc_length )
, length( length_utf8_to_unicode( _length, _data ) )
, data( NULL )
, utf8_data( NULL )
#if defined(WIN32)
, utf16_data( NULL )
#endif
{
    emacs_assert( _type != EmacsString::free );

    data = reinterpret_cast<EmacsChar_t *>( EMACS_MALLOC( sizeof( EmacsChar_t )*(length+1), malloc_type_char ) );
    convert_utf8_to_unicode( _data, length, data );
    data[ length ] = 0;
    type = EmacsString::free;
}

#if defined(WIN32)
EmacsStringRepresentation::EmacsStringRepresentation
    (
    enum EmacsString::string_type _type,
    int _alloc_length,
    int _length,
    const wchar_t *_data
    )
: ref_count(1)
, type( _type )
, alloc_length( _alloc_length )
, length( length_utf16_to_unicode( _length * sizeof( wchar_t ), reinterpret_cast<const unsigned char *>( _data ) ) )
, data( NULL )
, utf8_data( NULL )
, utf16_data( NULL )
{
    emacs_assert( _type != EmacsString::free );

    data = reinterpret_cast<EmacsChar_t *>( EMACS_MALLOC( sizeof( EmacsChar_t )*(length+1), malloc_type_char ) );
    convert_utf16_to_unicode( reinterpret_cast<const unsigned char *>( _data ), length, data );
    data[ length ] = 0;
    type = EmacsString::free;
}
#endif

EmacsStringRepresentation::EmacsStringRepresentation
    (
    enum EmacsString::string_type _type,
    int _alloc_length,
    int _length,
    const EmacsChar_t *_data
    )
: ref_count(1)
, type( _type )
, alloc_length( _alloc_length )
, length( _length )
, data( NULL )
, utf8_data( NULL )
#if defined(WIN32)
, utf16_data( NULL )
#endif
{
    alloc_length = length+1;
    alloc_length |= 15;
    alloc_length++;

    data = reinterpret_cast<EmacsChar_t *>( EMACS_MALLOC( sizeof( EmacsChar_t )*(alloc_length), malloc_type_char ) );
    for( int i=0; i< length; i++ )
        data[i] = _data[i];

    data[ length ] = 0;
    type = EmacsString::free;
}

EmacsStringRepresentation::~EmacsStringRepresentation()
{
    assert( ref_count == 0 );
    if( type == EmacsString::free )
        EMACS_FREE( data );
#if DBG_EMSTRING
    data = NULL;
#endif
    if( utf8_data != NULL )
    {
        EMACS_FREE( utf8_data );
        utf8_data = NULL;
    }
#if defined( WIN32 )
    if( utf16_data != NULL )
    {
        EMACS_FREE( utf16_data );
        utf16_data = NULL;
    }
#endif
}

const unsigned char *EmacsStringRepresentation::get_utf8_data()
{
    if( utf8_data == NULL )
    {
        length_utf8_data = length_unicode_to_utf8( length, data );
        utf8_data = malloc_ustr( length_utf8_data + 1 );
        convert_unicode_to_utf8( length, data, utf8_data );
        utf8_data[ length_utf8_data ] = 0;
    }

    return utf8_data;
}

#if defined(WIN32)
const wchar_t *EmacsStringRepresentation::get_utf16_data()
{
    if( utf16_data == NULL )
    {
        length_utf16_data = length_unicode_to_utf16( length, data );
        utf16_data = malloc_utf16( length_utf16_data + 1 );

        convert_unicode_to_utf16( length, data, reinterpret_cast<unsigned short *>( utf16_data ) );
        utf16_data[ length_utf16_data ] = 0;
    }

    return utf16_data;
}
#endif

//================================================================================
//
//    EmacsStringIterator
//
//================================================================================
EmacsStringIterator::EmacsStringIterator( const EmacsString &string, int index )
: m_string( string )
, m_index( index )
, m_remaining( string.length() - index )
{}

EmacsStringIterator::~EmacsStringIterator()
{}

bool EmacsStringIterator::next( EmacsChar_t &ch )
{
    if( m_remaining > 0 )
    {
        ch = m_string[ m_index ];
        m_index++;
        m_remaining--;
        return true;
    }

    return false;
}
