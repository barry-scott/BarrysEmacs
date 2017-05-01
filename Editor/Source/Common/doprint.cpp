//    Copyright (c) 1984-2017
//      Barry A. Scott

//
//    doprint.c
//
//    Barry A. Scott         8-Aug-1984
//
#include <emacsutl.h>
#include <emobject.h>
#include <emstring.h>
#include <emexcept.h>
#include <climits>

#if !defined( INT64_MIN )
#define INT64_MIN -9223372036854775808
#endif

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

inline bool control_character( int c)
{
    if( c >= 0x0000 && c <= 0x001f )
        return true;
    if( c == 0x007f )
        return true;
    if( c >= 0x0080 && c <= 0x009f )
        return true;

    return false;
}

//
//  The only format characters used by emacs are
//      %s    - string
//      %d    - decimal number
//      %x    - hexadecimal
//      %o    - octal
//      %-ns  - fixed field width string
//      %p    - pointer in hexadecimal
//      %r    - repr of string
//

//
//
//    FormatString implementation
//
//
FormatString::FormatString( EmacsString _format )
: format( _format )
, result()
, next_arg_type( argNone )
, next_width_type( argNone )
, next_precision_type( argNone )
, format_char( 0 )
, pad_char( ' ' )
, width( 0 )
, precision( INT_MAX )
, left_justify( 0 )
, next_format_char_index( 0 )
, intArg( 0 )
, stringArg()
{
    process_format();
}

FormatString::operator const EmacsString () const
{
    if( next_arg_type != argNone )
    {
        throw EmacsInternalError( "FormatString - not enough arguments" );
    }

    return result;
}

void FormatString::setNextIntArg( long long v )
{
    if( next_width_type == argInt )
    {
        next_width_type = argNone;
        width = v;
    }

    else if( next_precision_type == argInt )
    {
        next_precision_type = argNone;
        precision = v;
        return;
    }

    else if( next_arg_type == argInt )
    {
        intArg = v;

        process_format();

        return;
    }
}


FormatString &FormatString::operator <<( const void *v )
{
    if( next_arg_type == argInt )
    {
        intArg = reinterpret_cast<int64_t>( v );

        process_format();

        return *this;
    }

//    throw EmacsInternalError( "FormatString - int arg not expected" );
    return *this;
}

FormatString &FormatString::operator <<( const EmacsString *v )
{
    return operator << (*v);
}

FormatString &FormatString::operator <<( const EmacsString &v )
{
    if( next_width_type == argInt
    && next_precision_type == argInt
    && next_arg_type == argInt )
        throw EmacsInternalError( "FormatString - string arg not expected" );

    if( next_arg_type == argString )
    {
        stringArg = v;

        process_format();
    }

    return *this;
}

FormatString &FormatString::operator <<( const char *v )
{
    if( next_width_type == argInt
    && next_precision_type == argInt
    && next_arg_type == argInt )
    {
        throw EmacsInternalError( "FormatString - string arg not expected" );
    }

    if( next_arg_type == argString )
    {
        stringArg = v;

        process_format();
    }

    return *this;
}

FormatString &FormatString::operator <<( const EmacsChar_t *v )
{
    if( next_width_type == argInt
    && next_precision_type == argInt
    && next_arg_type == argInt )
    {
        throw EmacsInternalError( "FormatString - string arg not expected" );
    }

    if( next_arg_type == argString )
    {
        stringArg = EmacsString( EmacsString::copy, v );

        process_format();
    }

    return *this;
}

EmacsChar_t FormatString::next_format_char()
{
    if( next_format_char_index >= format.length() )
        return 0;

    return format[next_format_char_index++];
}

extern EmacsString os_error_code( unsigned int code );

void FormatString::process_format()
{
    // finish off the last arg found
    if( next_arg_type != argNone )
    {
        switch( format_char )
        {
        case 'c':
            put( EmacsChar_t( intArg ) );
            break;

        case 'C':    // ensure are in a printable char
            {
                EmacsChar_t ch = EmacsChar_t( intArg );
                if( control_character( ch ) )
                    ch = '.';
                put( ch );
            }
            break;

        case 's':
            print_string( stringArg );
            break;

        case 'r':
            print_repr( stringArg );
            break;

        case 'd':
            print_decimal( intArg );
            break;

        case 'D':
            print_decimal( intArg );
            break;

        case 'o':
            print_octal( intArg );
            break;

        case 'O':
            print_octal( intArg );
            break;

        case 'x':
            {
                uint64_t val = static_cast<uint64_t>( intArg );

                if( width == 0 )
                    width = 4;
                print_hexadecimal( val );
            }
            break;

        case 'X':
            {
                uint64_t val = static_cast<uint64_t>( intArg );

                if( width == 0 )
                    width = 8;
                print_hexadecimal( val );
            }
            break;

        case 'p':
            {
                uint64_t  val = static_cast<uint64_t>( intArg );
                width = sizeof( uint64_t )*2;
                print_hexadecimal( val );
            }
            break;

        case 'e':
            {
                // errno value
                EmacsString str( strerror( static_cast<int>( intArg ) ) );
                print_string( str );
            }
            break;

        case 'E':
            {
                // win32 error code
                EmacsString str( os_error_code( static_cast<int>( intArg ) ) );
                print_string( str );
                break;
            }
        default:
            throw EmacsInternalError( "FormatString - unknown format char" );
        }
    }

    // reset variables ready for next format arg
    next_width_type = argNone;
    next_precision_type = argNone;
    next_arg_type = argNone;

    pad_char = ' ';
    width = 0;
    precision = INT_MAX;

    left_justify = 0;

    // process upto the next arg
    EmacsChar_t ch;
    while( (ch = next_format_char()) > 0 )
    {
        if( ch != '%' )
        {
            put( ch );
            continue;
        }

        ch = next_format_char();

        if( ch == '-' )
        {
            ch = next_format_char();
            left_justify = 1;
        }

        if( ch == '*' )
        {
            next_width_type = argInt;
            ch = next_format_char();
        }
        else
        {
            if( ch == '0' )
            {
                pad_char = '0';
            }
            while( ch >= '0' && ch <= '9' )
            {
                width = width*10 + ch - '0';
                ch = next_format_char();
            }
        }

        if( ch == '.' )
        {
            ch = next_format_char();
            if( ch == '*' )
            {
                next_precision_type = argInt;
                ch = next_format_char();
            }
            else
            {
                while( ch >= '0' && ch <= '9' )
                {
                    precision = precision*10 + ch - '0';
                    ch = next_format_char();
                }
            }
        }
        else
        {
            precision = 9999;
        }

        format_char = ch;
        switch( ch )
        {
        case 0:    // end of format string
            return;
        case '%':
            put(  '%' );
            continue;
        case 's':
        case 'r':
            next_arg_type = argString;
            return;
        case 'd':
        case 'D':
        case 'o':
        case 'O':
        case 'x':
        case 'X':
        case 'p':
        case 'c':
        case 'C':
        case 'e':
        case 'E':
            next_arg_type = argInt;
            return;

        default:
            throw EmacsInternalError( "FormatString - unknown format char" );
        }
    }
}

void FormatString::print_string( const EmacsString &str )
{
    if( !left_justify )
    {
        for( int str_len = str.length(); str_len < width; width-- )
            put( ' ' );
    }

    int limit = std::min( precision, str.length() );

    EmacsString s2( str( 0, limit ) );
    put( s2.unicode_data(), s2.length() );

    width -= limit;
    while( width-- > 0 )
        put( ' ' );
}

void FormatString::print_repr( const EmacsString &arg )
{
    EmacsString repr;
    for( int i=0; i<arg.length(); ++i )
    {
        EmacsChar_t ch = arg[i];
        switch( ch )
        {
        case 7:
            repr.append( "\\a" ); break;
        case 8:
            repr.append( "\\b" ); break;
        case 9:
            repr.append( "\\t" ); break;
        case 10:
            repr.append( "\\n" ); break;
        case 13:
            repr.append( "\\r" ); break;
        case 27:
            repr.append( "\\e" ); break;
        default:
            if( ch < 32
            || (ch > 126 && ch < 192) )
            {
                repr.append( FormatString("\\%03.3o") << ch );
            }
            else
            {
                repr.append( ch );
            }
        }
    }

    print_string( repr );
}


void FormatString::put( EmacsChar_t c )
{
    result.append( c );
}

void FormatString::put( const EmacsChar_t *str, unsigned int len )
{
    result.append( len, str );
}

void FormatString::print_decimal( int64_t n )
{
    EmacsChar_t digits[12];

    if( n == INT64_MIN )
    {
        EmacsString value( "-9223372036854775808" );
        put( value.unicode_data(), value.length() );
        return;
    }

    if( n < 0 )
    {
        put( '-' );
        n = -n;
    }

    int i = 0;
    do
    {
        digits[i] = (char) ((n % 10) + '0');
        n = n / 10;
        i++;
    }
    while( n );

    int w = width;
    if( w && w > i )
    {
        for(; w > i; w-- )
            put( pad_char );
    }

    while( i-- )
        put( digits[i] );
}

void FormatString::print_hexadecimal( uint64_t n )
{
    EmacsChar_t buf[sizeof( uint64_t )*2];

    int w = width;
    for( int i=w-1; i >= 0; i--)
    {
        buf[i] = (EmacsChar_t)((n&0xf) >= 10 ? (n&0xf) + 'a' - 10 : (n&0xf) + '0');
        n >>= 4;
    }
    put( buf, w );
}

//
//  print a number "n" in octal into buffer "buf"
//
void FormatString::print_octal( int64_t n )
{
    unsigned char digits[24];

    if( n == INT64_MIN )
    {
        EmacsString value( "-1000000000000000000000" );
        put( value.unicode_data(), value.length() );
        return;
    }

    if( n < 0 )
    {
        put( '-' );
        n = -n;
    }

    int i = 0;
    do
    {
        digits[i] = (char) ((n % 8) + '0');
        n = n / 8;
        i++;
    }
    while( n );

    int w = width;
    if( w && w > i )
    {
        for(; w > i; w-- ) put( pad_char );
    }

    while( i-- )
        put( digits[i] );
}
