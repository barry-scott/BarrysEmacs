//
//    Copyright (c) 2010-2016 Barry A. Scott
//
#if defined( UNIT_TEST )
typedef unsigned int EmacsChar_t;

#include <cstdlib>
#include <iostream>
#include <iomanip>

#else
#include <emacsutl.h>

#endif

#include <emunicode.h>

#include <algorithm>

#include <em_unicode_data.h>
#include <iostream>

EmacsCharToCharMap_t __to_upper;
EmacsCharToCharMap_t __to_lower;
EmacsCharToCharMap_t __to_title;
EmacsCharToCharMap_t __casefold;

EmacsCharCategorySet_t __alphabetic;
EmacsCharCategorySet_t __numeric;

EmacsCharCategorySet_t __is_upper;
EmacsCharCategorySet_t __is_lower;
EmacsCharCategorySet_t __is_title;
EmacsCharCategorySet_t __is_space;
EmacsCharCategorySet_t __is_mlisp_space;

void init_unicode()
{
    for( struct unicode_category *p = unicode_init_numeric; p->code_point != 0; ++p )
    {
        __numeric.insert( p->code_point );
    }
    for( struct unicode_category *p = unicode_init_alphabetic; p->code_point != 0; ++p )
    {
        __alphabetic.insert( p->code_point );
    }
    for( struct unicode_category *p = unicode_init_is_upper; p->code_point != 0; ++p )
    {
        __is_upper.insert( p->code_point );
    }
    for( struct unicode_category *p = unicode_init_is_lower; p->code_point != 0; ++p )
    {
        __is_lower.insert( p->code_point );
    }
    for( struct unicode_category *p = unicode_init_is_title; p->code_point != 0; ++p )
    {
        __is_title.insert( p->code_point );
    }
    for( struct unicode_data *p = unicode_init_to_upper; p->code_point != 0; ++p )
    {
        __to_upper[ p->code_point ] = p->replacement;
    }
    for( struct unicode_data *p = unicode_init_to_lower; p->code_point != 0; ++p )
    {
        __to_lower[ p->code_point ] = p->replacement;
    }
    for( struct unicode_data *p = unicode_init_to_title; p->code_point != 0; ++p )
    {
        __to_title[ p->code_point ] = p->replacement;
    }
    for( struct unicode_data *p = unicode_init_casefold; p->code_point != 0; ++p )
    {
        __casefold[ p->code_point ] = p->replacement;
    }
    for( struct unicode_category *p = unicode_init_is_space; p->code_point != 0; ++p )
    {
        __is_space.insert( p->code_point );
        __is_mlisp_space.insert( p->code_point );
    }
    // emacs treats TAB as a space
    __is_space.insert( '\t' );

    // mlisp treat all the following as spaces
    __is_mlisp_space.insert( '\t' );
    __is_mlisp_space.insert( '\n' );
    __is_mlisp_space.insert( '\v' );
    __is_mlisp_space.insert( '\f' );
    __is_mlisp_space.insert( '\r' );
}

bool unicode_is_glyph( EmacsChar_t code_point )
{
    // is it a control char?
    if( code_point < 32 )
    {
        return false;
    }

    // is it DEL
    if( code_point == 0x7f )
    {
        return false;
    }

    // is it a control char?
    if( code_point >= 0x80 && code_point <= 0x9f )
    {
        return false;
    }

    // is it reserved for surigate pairs (UTF-16 encoding)
    if( code_point >= 0xdc00 && code_point <= 0xdfff )
    {
        return false;
    }

    // is it private use area
    if( code_point >= 0xe000 && code_point <= 0xf8ff )
    {
        return false;
    }

    if( code_point >= 0xf0000 && code_point <= 0x10fffd )
    {
        return false;
    }

    if( code_point > unicode_max_code_point )
    {
        return false;
    }

    return true;
}

bool unicode_is_digit( EmacsChar_t code_point )
{
    return code_point >= '0' && code_point <= '9';
}

bool unicode_is_numeric( EmacsChar_t code_point )
{
    return __numeric.count( code_point ) != 0;
}

EmacsCharCategorySet_t::const_iterator getNumericBegin()
{
    return __numeric.begin();
}

EmacsCharCategorySet_t::const_iterator getNumericEnd()
{
    return __numeric.end();
}

bool unicode_is_alphabetic( EmacsChar_t code_point )
{
    return __alphabetic.count( code_point ) != 0;
}

EmacsCharCategorySet_t::const_iterator getAlphabeticBegin()
{
    return __alphabetic.begin();
}

EmacsCharCategorySet_t::const_iterator getAlphabeticEnd()
{
    return __alphabetic.end();
}

bool unicode_is_mlisp_space( EmacsChar_t code_point )
{
    return __is_mlisp_space.count( code_point ) > 0;
}

bool unicode_is_space( EmacsChar_t code_point )
{
    return __is_space.count( code_point ) > 0;
}

bool unicode_is_upper( EmacsChar_t code_point )
{
    return __is_upper.count( code_point ) > 0;
}

bool unicode_has_upper_translation( EmacsChar_t code_point )
{
    return __to_upper.count( code_point ) > 0;
}

EmacsChar_t unicode_to_upper( EmacsChar_t code_point )
{
    EmacsCharToCharMap_t::iterator i = __to_upper.find( code_point );
    if( i == __to_upper.end() )
        return code_point;
    else
        return i->second;
}

bool unicode_is_lower( EmacsChar_t code_point )
{
    return __is_lower.count( code_point ) > 0;
}

bool unicode_has_lower_translation( EmacsChar_t code_point )
{
    return __to_lower.count( code_point ) > 0;
}

EmacsChar_t unicode_to_lower( EmacsChar_t code_point )
{
    EmacsCharToCharMap_t::iterator i = __to_lower.find( code_point );
    if( i == __to_lower.end() )
        return code_point;
    else
        return i->second;
}

bool unicode_is_title( EmacsChar_t code_point )
{
    return __is_title.count( code_point ) > 0;
}

bool unicode_has_title_translation( EmacsChar_t code_point )
{
    return __to_title.count( code_point ) > 0;
}

EmacsChar_t unicode_to_title( EmacsChar_t code_point )
{
    EmacsCharToCharMap_t::iterator i = __to_title.find( code_point );
    if( i == __to_title.end() )
        return code_point;
    else
        return i->second;
}

bool unicode_is_casefold( EmacsChar_t code_point )
{
    return __casefold.count( code_point ) > 0;
}

EmacsChar_t unicode_casefold( EmacsChar_t code_point )
{
    EmacsCharToCharMap_t::iterator i = __casefold.find( code_point );
    if( i == __casefold.end() )
        return code_point;
    else
        return i->second;
}

//
//   UCS-4 range (hex.)           UTF-8 octet sequence (binary)
//   0000 0000-0000 007F   0xxxxxxx
//   0000 0080-0000 07FF   110xxxxx 10xxxxxx
//   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
//   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//   0400 0000-7FFF FFFF   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//
int length_utf8_code_point( unsigned char first_byte )
{
    if( (first_byte&0x80) == 0x00 ) return 1;
    if( (first_byte&0xe0) == 0xc0 ) return 2;
    if( (first_byte&0xf0) == 0xe0 ) return 3;
    if( (first_byte&0xf8) == 0xf0 ) return 4;
    if( (first_byte&0xfc) == 0xf8 ) return 5;
    if( (first_byte&0xfe) == 0xfc ) return 6;

    // encoding error
    return 1;
}

int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data )
{
    int length = 0;
    int remaining = utf8_length;
    int i=0;
    while( remaining > 0 )
    {
        unsigned char ch = utf8_data[i];
        int code_point_len = length_utf8_code_point( ch );

        i += code_point_len;
        remaining -= code_point_len;
        ++length;
    }

    return length;
}

int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data, int unicode_limit, int &utf8_usable_length )
{
    int length = 0;
    int remaining = utf8_length;
    int i=0;
    while( remaining > 0 && length < unicode_limit )
    {
        unsigned char ch = utf8_data[i];
        int code_point_len = length_utf8_code_point( ch );
        if( code_point_len > remaining )
        {
            break;
        }

        i += code_point_len;
        remaining -= code_point_len;
        ++length;
    }

    utf8_usable_length = i;
    return length;
}

void convert_utf8_to_unicode( const unsigned char *utf8_data, int unicode_length, EmacsChar_t *unicode_data )
{
    while( unicode_length-- > 0 )
    {
        unsigned char ch = *utf8_data++;
        if( (ch&0x80) == 0 )
        {
            *unicode_data++ = ch;
        }
        else if( (ch&0xe0) == 0xc0 )
        {
            *unicode_data++ = ((ch&0x1f) <<  6) |
                                ((utf8_data[0])&0x3f);
            utf8_data += 1;
        }
        else if( (ch&0xf0) == 0xe0 )
        {
            *unicode_data++ = ((ch&0x0f) << 12) |
                                ((utf8_data[0]&0x3f) << 6 ) |
                                ((utf8_data[1]&0x3f) );
            utf8_data += 2;
        }
        else if( (ch&0xf8) == 0xf0 )
        {
            *unicode_data++ = ((ch&0x07) << 18) |
                                ((utf8_data[0]&0x3f) << 12 ) |
                                ((utf8_data[1]&0x3f) << 6 ) |
                                ((utf8_data[2]&0x3f) );
            utf8_data += 3;
        }
        else if( (ch&0xfc) == 0xf8 )
        {
            *unicode_data++ = ((ch&0x03) << 24) |
                                ((utf8_data[0]&0x3f) << 18 ) |
                                ((utf8_data[1]&0x3f) << 12 ) |
                                ((utf8_data[2]&0x3f) << 6 ) |
                                ((utf8_data[3]&0x3f) );
            utf8_data += 4;
        }
        else if( (ch&0xfe) == 0xfc )
        {
            *unicode_data++ = ((ch&0x01) << 30) |
                                ((utf8_data[0]&0x3f) << 24 ) |
                                ((utf8_data[1]&0x3f) << 18 ) |
                                ((utf8_data[2]&0x3f) << 12 ) |
                                ((utf8_data[3]&0x3f) << 6 ) |
                                ((utf8_data[4]&0x3f) );
            utf8_data += 5;
        }
        else
        {
            // Error in the encoding
            *unicode_data++ = ch;
        }
    }
}

int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data )
{
    int length = 0;

    for( int i=0; i<unicode_length; --unicode_length )
    {
        int uni_ch = *unicode_data++;
        if( uni_ch <= 0x0000007f )
        {
            length += 1;
        }
        else if( uni_ch <= 0x000007ff )
        {
            length += 2;
        }
        else if( uni_ch <= 0x0000ffff )
        {
            length += 3;
        }
        else if( uni_ch <= 0x001fffff )
        {
            length += 4;
        }
        else if( uni_ch <= 0x03ffffff )
        {
            length += 5;
        }
        else if( uni_ch <= 0x7fffffff )
        {
            length += 6;
        }
        else
        {
            // Error in the encoding
            length += 1;
        }
    }
    return length;
}

int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, int utf8_limit, int &unicode_usable_length )
{
    int length = 0;

    for( int i=0; i<unicode_length; ++i )
    {
        int next_char_length = 0;
        int uni_ch = *unicode_data++;
        if( uni_ch <= 0x0000007f )
        {
            next_char_length = 1;
        }
        else if( uni_ch <= 0x000007ff )
        {
            next_char_length = 2;
        }
        else if( uni_ch <= 0x0000ffff )
        {
            next_char_length = 3;
        }
        else if( uni_ch <= 0x001fffff )
        {
            next_char_length = 4;
        }
        else if( uni_ch <= 0x03ffffff )
        {
            next_char_length = 5;
        }
        else if( uni_ch <= 0x7fffffff )
        {
            next_char_length = 6;
        }
        else
        {
            // Error in the encoding
            next_char_length = 1;
        }

        if( (length + next_char_length) <= utf8_limit )
        {
            length += next_char_length;
        }
        else
        {
            unicode_usable_length = i;
            return length;
        }
    }
    unicode_usable_length = unicode_length;
    return length;
}

void convert_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, unsigned char *utf8_data )
{
    for( int i=0; i<unicode_length; --unicode_length )
    {
        int uni_ch = *unicode_data++;
        if( uni_ch <= 0x0000007f )
        {
            *utf8_data++ = uni_ch&0xff;
        }
        else if( uni_ch <= 0x000007ff )
        {
            *utf8_data++ = 0xc0 | ((uni_ch>>6)&0x1f);
            *utf8_data++ = 0x80 | ((uni_ch)&0x3f);
        }
        else if( uni_ch <= 0x0000ffff )
        {
            *utf8_data++ = 0xe0 | ((uni_ch>>12)&0x0f);
            *utf8_data++ = 0x80 | ((uni_ch>>6)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch)&0x3f);
        }
        else if( uni_ch <= 0x001fffff )
        {
            *utf8_data++ = 0xf0 | ((uni_ch>>18)&0x07);
            *utf8_data++ = 0x80 | ((uni_ch>>12)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>6)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch)&0x3f);
        }
        else if( uni_ch <= 0x03ffffff )
        {
            *utf8_data++ = 0xf8 | ((uni_ch>>24)&0x03);
            *utf8_data++ = 0x80 | ((uni_ch>>18)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>12)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>6)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch)&0x3f);
        }
        else if( uni_ch <= 0x7fffffff )
        {
            *utf8_data++ = 0xfc | ((uni_ch>>30)&0x01);
            *utf8_data++ = 0x80 | ((uni_ch>>24)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>18)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>12)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch>>6)&0x3f);
            *utf8_data++ = 0x80 | ((uni_ch)&0x3f);
        }
        else
        {
            // Error in the encoding
            *utf8_data++ = uni_ch&0xff;
        }
    }
}

//
//  UCS-4 range (hex.)      UTF-16 word sequence (binary)
//  0000 0000-0000 D7FF     xxxxxxxx-xxxxxxxx (AS-IS)
//  0000 D800-0000 DFFF     (reserved for UTF-16 encoded form)
//  0000 E000-0000 EFFF     xxxxxxxx-xxxxxxxx (AS-IS)
//  0001 0000-0010 FFFF     110110xx-xxxxxxxx 110110yy yyyyyyyy
//                          10 bit x is ((USC-4)-0x10000) >> 10)) & 0x3fff
//                          10 bit y is ((USC-4)-0x10000) & 0x3fff
//
int length_utf16_to_unicode( int data_length, const unsigned char *data )
{
    int utf16_length = data_length/sizeof( unsigned short );
    const unsigned short *utf16_data = reinterpret_cast<const unsigned short *>( data );

    int length = 0;
    int remaining = utf16_length;
    int i=0;
    while( remaining > 0 )
    {
        unsigned short ch = utf16_data[i];

        if( (ch <= 0xd7ff) && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch >= 0xe000) && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch >= 0xd800 && ch <= 0xdfff) && remaining >= 2 )
        {
            i += 2;
            remaining -= 2;
            ++length;
        }
        else
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
    }

    return length;
}

int length_utf16_to_unicode( int data_length, const unsigned char *data, int unicode_limit, int &utf16_usable_length )
{
    int utf16_length = data_length/sizeof( unsigned short );
    const unsigned short *utf16_data = reinterpret_cast<const unsigned short *>( data );

    int length = 0;
    int remaining = utf16_length;
    int i=0;
    while( remaining > 0 && length < unicode_limit )
    {
        unsigned short ch = utf16_data[i];

        if( (ch <= 0xd7ff) && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch >= 0xe000) && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch >= 0xd800 && ch <= 0xdfff) && remaining >= 2 )
        {
            i += 2;
            remaining -= 2;
            ++length;
        }
        else
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
    }

    utf16_usable_length = i*sizeof( unsigned short );
    return length;
}

void convert_utf16_to_unicode( const unsigned char *data, int unicode_length, EmacsChar_t *unicode_data )
{
    const unsigned short *utf16_data = reinterpret_cast<const unsigned short *>( data );

    while( unicode_length-- > 0 )
    {
        unsigned short ch = *utf16_data++;
        if( (ch <= 0xd7ff) )
        {
            *unicode_data++ = ch;
        }
        else if( (ch >= 0xe000) )
        {
            *unicode_data++ = ch;
        }
        else if( (ch >= 0xd800 && ch <= 0xdfff) && unicode_length >= 1 )
        {
            int high = ch;
            int low = *utf16_data++;
            unicode_length--;

            *unicode_data++ = ((high << 10)&0x3ff) + (low&0x3ff) + 0x10000;
        }
        else
        {
            // Error in the encoding
            *unicode_data++ = ch;
        }
    }
}

int length_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data )
{
    int length = 0;

    for( int i=0; i<unicode_length; --unicode_length )
    {
        int uni_ch = *unicode_data++;
        if( uni_ch >= 0x10000 )
        {
            length += 2;
        }
        else
        {
            length += 1;
        }
    }
    return length;
}

int length_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data, int utf16_limit, int &unicode_usable_length )
{
    int length = 0;

    for( int i=0; i<unicode_length; ++i )
    {
        int next_char_length = 0;
        int uni_ch = *unicode_data++;

        if( uni_ch >= 0x10000 )
        {
            next_char_length += 2;
        }
        else
        {
            next_char_length += 1;
        }

        if( (length + next_char_length) <= utf16_limit )
        {
            length += next_char_length;
        }
        else
        {
            unicode_usable_length = i;
            return length;
        }
    }

    unicode_usable_length = unicode_length;
    return length;
}

void convert_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data, unsigned short *utf16_data )
{
    for( int i=0; i<unicode_length; --unicode_length )
    {
        EmacsChar_t uni_ch = *unicode_data++;
        if( uni_ch <= 0x10000 )
        {
            *utf16_data++ = uni_ch&0xffff;
        }
        else
        {
            *utf16_data++ = 0xd800 | ((uni_ch>>10)&0x3ff);
            *utf16_data++ = 0xdc00 | (uni_ch&0x3fff);
        }
    }
}

//
//  unicode string operations
//
int unicode_strcmp( int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 )
{
    for( int common_length = std::min( len1, len2 ); common_length > 0; --common_length )
    {
        EmacsChar_t ch1 = *str1++;
        EmacsChar_t ch2 = *str2++;

        if( ch1 != ch2 )
        {
            if( ch1 < ch2 )
                return -1;
            else
                return 1;
        }
    }
    if( len1 == len2 )
        return 0;
    if( len1 < len2 )
        return -1;
    else
        return 1;
}

int unicode_stricmp( int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 )
{
    // QQQ Need case blind compare
    return unicode_strcmp( len1, str1, len2, str2 );
}

#if defined( UNIT_TEST )

int pass_count = 0;
int fail_count = 0;

const int buf_size = 32;
const int guard_size = 16;
const int total_size = guard_size + buf_size + guard_size;

const int unicode_poison = 0x7ededede;
const unsigned char utf8_poison = 0xde;

EmacsChar_t guard_unicode_buffer[ total_size ];
unsigned char guard_utf8_buffer[ total_size ];
EmacsChar_t *unicode_buffer = &guard_unicode_buffer[ guard_size ];
unsigned char *utf8_buffer = &guard_utf8_buffer[ guard_size ];


void test_code_point_length( const char *title, unsigned char first_byte, int expected_length )
{
    bool failed = false;
    std::cout << "test_code_point_length: " << title << " ------------------------------------------------------------" << std::endl;

    int code_point_length = length_utf8_code_point( first_byte );
    if( code_point_length != expected_length )
    {
        std::cout << "FAILURE: code_point_length not as expected " << code_point_length << "!=" << expected_length << std::endl;
        failed = true;
    }

    if( failed )
        fail_count++;
    else
        pass_count++;
}

void test_utf8_unicode( const char *title, int str_length, const char *str, int expected_length )
{
    bool failed = false;
    std::cout << "test_utf8_unicode: " << title << " ------------------------------------------------------------" << std::endl;

    //--------------------------------------------------------------------------------
    int unicode_length = length_utf8_to_unicode( str_length,  reinterpret_cast<const unsigned char *>( str ) );
    std::cout << "    length_utf8_to_unicode expected " << expected_length << " -> actual " << unicode_length << std::endl;
    if( unicode_length != expected_length )
    {
        std::cout << "FAILURE: unicode_length not as expected" << std::endl;
        failed = true;
    }

    for( int i=0; i<total_size; i++ )
    {
        guard_unicode_buffer[i] = unicode_poison;
    }

    std::cout << "    convert_unicode_to_utf8..." << std::endl;
    convert_utf8_to_unicode( reinterpret_cast<const unsigned char *>( str ), unicode_length, unicode_buffer );

    for( int i=0; i<unicode_length; i++ )
    {
        std::cout << "        unicode_buffer[" << i << "] = 0x" << std::hex << unicode_buffer[i] << std::dec << std::endl;
    }

    for( int i=0; i<guard_size; i++ )
    {
        if( guard_unicode_buffer[i] != unicode_poison )
        {
            std::cout << "FAILURE: guard_unicode_buffer overwritten at index " << i << std::endl;
            failed = true;
        }
    }
    for( int i=guard_size+expected_length; i<total_size; i++ )
    {
        if( guard_unicode_buffer[i] != unicode_poison )
        {
            std::cout << "FAILURE: guard_unicode_buffer overwritten at index " << i << std::endl;
            failed = true;
        }
    }
    if( failed )
        fail_count++;
    else
        pass_count++;
}

void test_unicode_utf8( const char *title, int unicode_length, const EmacsChar_t *unicode_buffer, int expected_length, const char *expected_str )
{
    bool failed = false;

    std::cout << "test_unicode_utf8: " << title << " ------------------------------------------------------------" << std::endl;

    int utf8_length = length_unicode_to_utf8( unicode_length, unicode_buffer );
    std::cout << "    length_unicode_to_utf8 expected << " << expected_length << " -> actual " << utf8_length << std::endl;
    if( utf8_length != expected_length )
    {
        std::cout << "FAILURE: utf8_length not as expected" << std::endl;
        failed = false;
    }

    std::cout << "    convert_unicode_to_utf8..." << std::endl;

    for( int i=0; i<total_size; i++ )
    {
        guard_utf8_buffer[i] = utf8_poison;
    }

    convert_unicode_to_utf8( unicode_length, unicode_buffer, utf8_buffer );

    for( int i=0; i<utf8_length; i++ )
    {
        std::cout << "        utf8_buffer[" << i << "] = 0x" << std::hex << (int)utf8_buffer[i] << std::dec << std::endl;
        if( (unsigned char)(expected_str[i]) != utf8_buffer[i] )
        {
            std::cout << "FAILURE: utf8_buffer index " << i << " expected " << std::hex << (int)(unsigned char)(expected_str[i]) << " actual " << (int)utf8_buffer[i] << std::dec << std::endl;
            failed = true;
        }
    }

    for( int i=0; i<guard_size; i++ )
    {
        if( guard_utf8_buffer[i] != utf8_poison )
        {
            std::cout << "FAILURE: guard_utf8_buffer overwritten at index " << i << std::endl;
            failed = true;
        }
    }
    for( int i=guard_size+expected_length; i<total_size; i++ )
    {
        if( guard_utf8_buffer[i] != utf8_poison )
        {
            std::cout << "FAILURE: guard_utf8_buffer overwritten at index " << i << std::endl;
            failed = true;
        }
    }
    if( failed )
        fail_count++;
    else
        pass_count++;
}

void test_fio_usage( const char *utf8_data, int unicode_limit, int expected_unicode_length, int expected_utf8_useble_length )
{
    bool failed = false;
    int utf8_usable_length = 0;
    int utf8_length = 0;
    for( const char *p=utf8_data; *p != 0; ++p )
    {
        utf8_length++;
    }
    int unicode_length = length_utf8_to_unicode( utf8_length, (unsigned char *)utf8_data, unicode_limit, utf8_usable_length );

    std::cout << "test_fio_usage utf8_data";

    for( int i=0; i<utf8_length; i++ )
    {
        std::cout << " " << std::hex << ((unsigned int)utf8_data[i]&0xff) << std::dec;
    }

    std::cout
        << " unicode_length expected " << expected_unicode_length << " -> actual " << unicode_length
        << " utf8_usable_length expected " << expected_utf8_useble_length << " -> actual " << utf8_usable_length
        << std::endl;

    if( unicode_length != expected_unicode_length )
    {
        std::cout << "FAILURE: unicode_length(" << unicode_length << ") != expected_unicode_length(" << expected_unicode_length << ")" << std::endl;
        failed = true;
    }

    if( utf8_usable_length != expected_utf8_useble_length )
    {
        std::cout << "FAILURE: utf8_usable_length(" << utf8_usable_length << ") != expected_utf8_useble_length(" << expected_utf8_useble_length << ")" << std::endl;
        failed = true;
    }

    if( failed )
        fail_count++;
    else
        pass_count++;
}

int main( int, char ** )
{
    test_code_point_length( "0x41", 0x41, 1 );
    test_code_point_length( "0xc0", 0xc0, 2 );
    test_code_point_length( "0ce0", 0xe0, 3 );
    test_code_point_length( "0xf0", 0xf0, 4 );
    test_code_point_length( "0xf8", 0xf8, 5 );
    test_code_point_length( "0xfc", 0xfc, 6 );
    test_code_point_length( "0xff", 0xff, 1);
    test_code_point_length( "0x10", 0x10, 1 );
    test_code_point_length( "0xff", 0xff, 1);

    test_utf8_unicode( "str_1", 3, "abc", 3 );
    test_unicode_utf8( "str_2", 3, unicode_buffer, 3, "abc" );
    test_utf8_unicode( "str_3", 5, "[\xe2\x82\xac]", 3 );
    test_unicode_utf8( "str_4", 3, unicode_buffer, 5, "[\xe2\x82\xac]" );

    unicode_buffer[0] = 0x00000001;
    test_unicode_utf8( "str 0x00000001", 1, unicode_buffer, 1, "\x01" );
    test_utf8_unicode( "str 0x00000001", 1, "\x01", 1 );

    unicode_buffer[0] = 0x00000002;
    test_unicode_utf8( "str 0x00000002", 1, unicode_buffer, 1, "\x02" );
    test_utf8_unicode( "str 0x00000002", 1, "\x02", 1 );

    unicode_buffer[0] = 0x00000004;
    test_unicode_utf8( "str 0x00000004", 1, unicode_buffer, 1, "\x04" );
    test_utf8_unicode( "str 0x00000004", 1, "\x04", 1 );

    unicode_buffer[0] = 0x00000008;
    test_unicode_utf8( "str 0x00000008", 1, unicode_buffer, 1, "\x08" );
    test_utf8_unicode( "str 0x00000008", 1, "\x08", 1 );

    unicode_buffer[0] = 0x00000010;
    test_unicode_utf8( "str 0x00000010", 1, unicode_buffer, 1, "\x10" );
    test_utf8_unicode( "str 0x00000010", 1, "\x10", 1 );

    unicode_buffer[0] = 0x00000020;
    test_unicode_utf8( "str 0x00000020", 1, unicode_buffer, 1, "\x20" );
    test_utf8_unicode( "str 0x00000020", 1, "\x20", 1 );

    unicode_buffer[0] = 0x00000040;
    test_unicode_utf8( "str 0x00000040", 1, unicode_buffer, 1, "\x40" );
    test_utf8_unicode( "str 0x00000040", 1, "\x40", 1 );

    unicode_buffer[0] = 0x0000007f;
    test_unicode_utf8( "str 0x0000007f", 1, unicode_buffer, 1, "\x7f" );
    test_utf8_unicode( "str 0x0000007f", 1, "\x7f", 1 );

    unicode_buffer[0] = 0x00000080;
    test_unicode_utf8( "str 0x00000080", 1, unicode_buffer, 2, "\xc2\x80" );
    test_utf8_unicode( "str 0x00000080", 2, "\xc2\x80", 1 );

    unicode_buffer[0] = 0x00000100;
    test_unicode_utf8( "str 0x00000100", 1, unicode_buffer, 2, "\xc4\x80" );
    test_utf8_unicode( "str 0x00000100", 2, "\xc4\x80", 1 );

    unicode_buffer[0] = 0x000007ff;
    test_unicode_utf8( "str 0x000007ff", 1, unicode_buffer, 2, "\xdf\xbf" );
    test_utf8_unicode( "str 0x000007ff", 2, "\xdf\xbf", 1 );

    unicode_buffer[0] = 0x00000800;
    test_unicode_utf8( "str 0x00000800", 1, unicode_buffer, 3, "\xe0\xa0\x80" );
    test_utf8_unicode( "str 0x00000800", 3, "\xe0\xa0\x80", 1 );

    unicode_buffer[0] = 0x0000ffff;
    test_unicode_utf8( "str 0x0000ffff", 1, unicode_buffer, 3, "\xef\xbf\xbf" );
    test_utf8_unicode( "str 0x0000ffff", 3, "\xef\xbf\xbf", 1 );

    unicode_buffer[0] = 0x00010000;
    test_unicode_utf8( "str 0x00010000", 1, unicode_buffer, 4, "\xf0\x90\x80\x80" );
    test_utf8_unicode( "str 0x00010000", 4, "\xf0\x90\x80\x80", 1 );

    unicode_buffer[0] = 0x001fffff;
    test_unicode_utf8( "str 0x001fffff", 1, unicode_buffer, 4, "\xf7\xbf\xbf\xbf" );
    test_utf8_unicode( "str 0x001fffff", 4, "\xf7\xbf\xbf\xbf", 1 );

    unicode_buffer[0] = 0x00200000;
    test_unicode_utf8( "str 0x00200000", 1, unicode_buffer, 5, "\xf8\x88\x80\x80\x80" );
    test_utf8_unicode( "str 0x00200000", 5, "\xf8\x88\x80\x80\x80", 1 );

    unicode_buffer[0] = 0x03ffffff;
    test_unicode_utf8( "str 0x03ffffff", 1, unicode_buffer, 5, "\xfb\xbf\xbf\xbf\xbf" );
    test_utf8_unicode( "str 0x03ffffff", 5, "\xfb\xbf\xbf\xbf\xbf", 1 );

    unicode_buffer[0] = 0x04000000;
    test_unicode_utf8( "str 0x04000000", 1, unicode_buffer, 6, "\xfc\x84\x80\x80\x80\x80" );
    test_utf8_unicode( "str 0x04000000", 6, "\xfc\x84\x80\x80\x80\x80", 1 );

    unicode_buffer[0] = 0x7fffffff;
    test_unicode_utf8( "str 0x7fffffff", 1, unicode_buffer, 6, "\xfd\xbf\xbf\xbf\xbf\xbf" );
    test_utf8_unicode( "str 0x7fffffff", 6, "\xfd\xbf\xbf\xbf\xbf\xbf", 1 );

    test_fio_usage( "\xe2\x82\xac", 1, 1, 3 );
    test_fio_usage( "[\xe2\x82\xac]", 3, 3, 5 );
    test_fio_usage( "[\xe2\x82\xac]", 2, 2, 4 );
    test_fio_usage( "[\xe2\x82", 2, 1, 1 );

    if( fail_count > 0 )
        std::cout << "FAILURE: " << fail_count << " tests failed" << std::endl;

    std::cout << "Passed: " << pass_count << " tests passed" << std::endl;
    return 0;
}
#endif
