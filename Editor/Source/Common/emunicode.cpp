//
//    Copyright (c) 2010 Barry A. Scott
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

//
//   UCS-4 range (hex.)           UTF-8 octet sequence (binary)
//   0000 0000-0000 007F   0xxxxxxx
//   0000 0080-0000 07FF   110xxxxx 10xxxxxx
//   0000 0800-0000 FFFF   1110xxxx 10xxxxxx 10xxxxxx
//   0001 0000-001F FFFF   11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
//   0020 0000-03FF FFFF   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//   0400 0000-7FFF FFFF   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
//
int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data )
{
    int length = 0;
    int remaining = utf8_length;
    int i=0;
    while( remaining > 0 )
    {
        unsigned char ch = utf8_data[i];

        if( (ch&0x80) == 0x00 && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch&0xe0) == 0xc0 && remaining >= 2 )
        {
            i += 2;
            remaining -= 2;
            ++length;
        }
        else if( (ch&0xf0) == 0xe0 && remaining >= 3 )
        {
            i += 3;
            remaining -= 3;
            ++length;
        }
        else if( (ch&0xf8) == 0xf0 && remaining >= 4 )
        {
            i += 4;
            remaining -= 4;
            ++length;
        }
        else if( (ch&0xfc) == 0xf8 && remaining >= 5 )
        {
            i += 5;
            remaining -= 5;
            ++length;
        }
        else if( (ch&0xfe) == 0xfc && remaining >= 6 )
        {
            i += 6;
            remaining -= 6;
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

int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data, int unicode_limit, int &utf8_usable_length )
{
    int length = 0;
    int remaining = utf8_length;
    int i=0;
    while( remaining > 0 && length < unicode_limit )
    {
        unsigned char ch = utf8_data[i];

        if( (ch&0x80) == 0x00 && remaining >= 1 )
        {
            i += 1;
            remaining -= 1;
            ++length;
        }
        else if( (ch&0xe0) == 0xc0 && remaining >= 2 )
        {
            i += 2;
            remaining -= 2;
            ++length;
        }
        else if( (ch&0xf0) == 0xe0 && remaining >= 3 )
        {
            i += 3;
            remaining -= 3;
            ++length;
        }
        else if( (ch&0xf8) == 0xf0 && remaining >= 4 )
        {
            i += 4;
            remaining -= 4;
            ++length;
        }
        else if( (ch&0xfc) == 0xf8 && remaining >= 5 )
        {
            i += 5;
            remaining -= 5;
            ++length;
        }
        else if( (ch&0xfe) == 0xfc && remaining >= 6 )
        {
            i += 6;
            remaining -= 6;
            ++length;
        }
        else
        {
            break;
        }
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
                                (utf8_data[0])&0x3f;
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
            *unicode_data++ = ((ch&0x07) << 24) |
                                ((utf8_data[0]&0x3f) << 18 ) |
                                ((utf8_data[1]&0x3f) << 12 ) |
                                ((utf8_data[2]&0x3f) << 6 ) |
                                ((utf8_data[3]&0x3f) );
            utf8_data += 4;
        }
        else if( (ch&0xfe) == 0xfc )
        {
            *unicode_data++ = ((ch&0x07) << 30) |
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
            *utf8_data++ = uni_ch;
        }
        else if( uni_ch <= 0x000007ff )
        {
            *utf8_data++ = 0xc0 | (uni_ch>>6)&0x1f;
            *utf8_data++ = 0x80 | (uni_ch)&0x3f;
        }
        else if( uni_ch <= 0x0000ffff )
        {
            *utf8_data++ = 0xe0 | (uni_ch>>12)&0x0f;
            *utf8_data++ = 0x80 | (uni_ch>>6)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch)&0x3f;
        }
        else if( uni_ch <= 0x001fffff )
        {
            *utf8_data++ = 0xf0 | (uni_ch>>18)&0x07;
            *utf8_data++ = 0x80 | (uni_ch>>12)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>6)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch)&0x3f;
        }
        else if( uni_ch <= 0x03ffffff )
        {
            *utf8_data++ = 0xf8 | (uni_ch>>24)&0x03;
            *utf8_data++ = 0x80 | (uni_ch>>18)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>12)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>6)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch)&0x3f;
        }
        else if( uni_ch <= 0x7fffffff )
        {
            *utf8_data++ = 0xfc | (uni_ch>>30)&0x01;
            *utf8_data++ = 0x80 | (uni_ch>>24)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>18)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>12)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch>>6)&0x3f;
            *utf8_data++ = 0x80 | (uni_ch)&0x3f;
        }
        else
        {
            // Error in the encoding
            *utf8_data++ = uni_ch&0xff;
        }
    }
}

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
EmacsChar_t unicode_buffer[ buf_size ];
unsigned char utf8_buffer[ buf_size ];

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

    for( int i=0; i<buf_size; i++ )
    {
        unicode_buffer[i] = 0;
    }

    std::cout << "    convert_unicode_to_utf8..." << std::endl;
    convert_utf8_to_unicode( reinterpret_cast<const unsigned char *>( str ), unicode_length, unicode_buffer );

    for( int i=0; i<unicode_length; i++ )
    {
        std::cout << "        unicode_buffer[" << i << "] = 0x" << std::hex << unicode_buffer[i] << std::dec << std::endl;
    }

    for( int i=unicode_length; i<buf_size; i++ )
    {
        if( unicode_buffer[i] != 0 )
        {
            std::cout << "FAILURE: unicode_buffer overwritten at index " << i << std::endl;
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

    for( int i=0; i<buf_size; i++ )
        utf8_buffer[i] = 0;

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

    for( int i=utf8_length; i<buf_size; i++ )
    {
        if( utf8_buffer[i] != 0 )
        {
            std::cout << "FAILURE: utf8_buffer overwritten at index " << i << std::endl;
            failed = true;
        }
    }
    if( failed )
        fail_count++;
    else
        pass_count++;
}

void test_fio_usage()
{
    int utf8_usable_length = 0;
    int unicode_length = 0;

    //--------------------------------------------------------------------------------
    unicode_length = length_utf8_to_unicode( 3, (unsigned char *)("\xe2\x82\xac"), 1, utf8_usable_length );
    std::cout << "test_fio_usage"
        << " unicode_length(1) " << unicode_length
        << " utf8_usable_length(3) " << utf8_usable_length
        << std::endl;

    //--------------------------------------------------------------------------------
    unicode_length = length_utf8_to_unicode( 5, (unsigned char *)("[\xe2\x82\xac]"), 3, utf8_usable_length );
    std::cout << "test_fio_usage"
        << " unicode_length(3) " << unicode_length
        << " utf8_usable_length(5) " << utf8_usable_length
        << std::endl;

    //--------------------------------------------------------------------------------
    unicode_length = length_utf8_to_unicode( 5, (unsigned char *)("[\xe2\x82\xac]"), 2, utf8_usable_length );
    std::cout << "test_fio_usage"
        << " unicode_length(2) " << unicode_length
        << " utf8_usable_length(4) " << utf8_usable_length
        << std::endl;

    //--------------------------------------------------------------------------------
    unicode_length = length_utf8_to_unicode( 3, (unsigned char *)("[\xe2\x82"), 2, utf8_usable_length );
    std::cout << "test_fio_usage"
        << " unicode_length(1) " << unicode_length
        << " utf8_usable_length(1) " << utf8_usable_length
        << std::endl;
}

int main( int argc, char **argv )
{
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

    test_fio_usage();

    if( fail_count > 0 )
        std::cout << "FAILURE: " << fail_count << " tests failed" << std::endl;

    std::cout << "Passed: " << pass_count << " tests passed" << std::endl;    
    return 0;
}
#endif
