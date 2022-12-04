//
//    Copyright (c) 1995-2010 Barry A. Scott
//
//    fio.c
//
#include <emacs.h>
#include <emunicode.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <em_stat.h>

#if defined( __WATCOMC__ ) || defined( __unix__ )
# include <unistd.h>
# define _S_IWRITE    S_IWRITE
#endif

#if defined( _MSDOS ) || defined( _NT ) || defined( _WINDOWS )
# include <io.h>
# include <share.h>
# if defined( __WATCOMC__ )
#  define SHARE_READ , SH_DENYWR
#  define SHARE_NONE , SH_DENYRW
# else
#  define SHARE_READ , _SH_DENYWR
#  define SHARE_NONE , _SH_DENYRW
# endif
# define fopen _fsopen
#endif

#ifndef SHARE_READ
# define SHARE_READ
# define SHARE_NONE
#endif

EmacsFileImplementation::EmacsFileImplementation( FileParse &fab )
: m_fab( fab )
{ }

EmacsFileImplementation::~EmacsFileImplementation()
{ }


EmacsFile::EmacsFile( const EmacsString &filename, const EmacsString &def, FIO_EOL_Attribute attr )
: fab()
, impl( EMACS_NEW EmacsFileLocal( fab, attr ) )
{
    fab.sys_parse( filename, def );
}

EmacsFile::EmacsFile( const EmacsString &filename, FIO_EOL_Attribute attr )
: fab()
, impl( EMACS_NEW EmacsFileLocal( fab, attr ) )
{
    fab.sys_parse( filename, EmacsString::null );
}

EmacsFile::EmacsFile( FIO_EOL_Attribute attr )
: fab()
, impl( EMACS_NEW EmacsFileLocal( fab, attr ) )
{ }

EmacsFile::~EmacsFile()
{
    delete impl;
}

EmacsFileLocal::EmacsFileLocal( FileParse &fab, FIO_EOL_Attribute attr )
: EmacsFileImplementation( fab )
, m_file( NULL )
, m_eol_attr( attr )
, m_encoding_attr( FIO_Encoding_None )
, m_convert_size( 0 )
, m_convert_buffer( new unsigned char[ CONVERT_BUFFER_SIZE ] )
{
}

EmacsFileLocal::~EmacsFileLocal()
{
    if( m_file != NULL && m_file != stdin )
    {
        fclose( m_file );
    }

    delete [] m_convert_buffer;
}

//
//    check that the file exists and has read or read and write
//    access allowed
//
int EmacsFileLocal::fio_access()
{
    if( m_fab.is_valid() )
    {
        //  6 means read and write, 4 means read
        int r = access( m_fab.result_spec, 6 );
        if( r == 0 )
            return 1;

        r = access( m_fab.result_spec, 4 );
        if( r == 0 )
            return -1;
    }

    return 0;
}

bool EmacsFileLocal::fio_file_exists()
{
    int r = -1;         // assume not valid
    if( m_fab.is_valid() )
    {
        r = access( m_fab.result_spec, 0 );
    }

    return r != -1;     // true if the file exists
}

int EmacsFileLocal::fio_delete()
{
    int r = 1;          // assume not valid
    if( m_fab.is_valid() )
    {
        r = unlink( m_fab.result_spec );
    }

    return r;
}

bool EmacsFileLocal::fio_create
    (
    const EmacsString &name,
    FIO_CreateMode mode,
    const EmacsString &defnam,
    FIO_EOL_Attribute attr
    )
{
    m_fab.sys_parse( name, defnam );
    m_file = fopen( m_fab.result_spec, "w" BINARY_MODE SHARE_NONE );

    m_eol_attr = attr;

    return m_file != NULL;
}

bool EmacsFileLocal::fio_open
    (
    const EmacsString &name,
    bool eof,
    const EmacsString &defnam,
    FIO_EOL_Attribute attr
    )
{
    m_fab.sys_parse( name, defnam );

    if( !file_is_regular( m_fab.result_spec ) )
        return false;

    if( eof )
    {
        // open for append
        m_file = fopen( m_fab.result_spec, "a" BINARY_MODE SHARE_NONE );
        m_eol_attr = attr;
    }
    else
        // open for read
        m_file = fopen( m_fab.result_spec, "r" BINARY_MODE SHARE_READ );

    return m_file != NULL;
}

int EmacsFileLocal::fio_get( unsigned char *buf, int len )
{
    int status = fread( buf, 1, len, m_file );
    if( ferror( m_file ) )
    {
        return -1;
    }

    if( status == 0 && feof( m_file ) )
    {
        return 0;
    }

    return get_fixup_buffer( buf, status );
}

int EmacsFileLocal::fio_get_line( unsigned char *buf, int len )
{
    fgets( s_str(buf), len, m_file );
    if( ferror( m_file ) )
        return -1;
    if( feof( m_file ) )
        return 0;

    return get_fixup_buffer( buf, strlen( (const char *)buf ) );
}

int EmacsFileLocal::fio_get_with_prompt( unsigned char *buf, int len, const unsigned char * /*prompt*/ )
{
    int status = fread( buf, 1, len, m_file );
    if( ferror( m_file ) )
        return -1;
    if( status == 0 && feof( m_file ) )
        return -1;
    return status;
}

int EmacsFileLocal::fio_put( const EmacsString &str )
{
    return fio_put( str.utf8_data(), str.utf8_data_length() );
}

int EmacsFileLocal::fio_put( const unsigned char *buf , int len )
{
    int written_length = 0;
    switch( m_eol_attr )
    {
    case FIO_EOL__StreamCR:
    {
        // find each LF and output the text followed by a CR
        const unsigned char *from = buf;
        const unsigned char *end = &buf[len];

        while( from < end )
        {
            const unsigned char *to = (const unsigned char *)memchr( from, '\n', end - from );
            if( to == NULL )
            {
                int status = fwrite( from, 1, end-from, m_file );
                if( ferror( m_file ) )
                    return -1;

                written_length += status;
                break;
            }

            int status = fwrite( from, 1, to-from, m_file );
            if( ferror( m_file ) )
                return -1;

            written_length += status;

            status = fwrite( "\r", 1, 1, m_file );
            if( ferror( m_file ) )
                return -1;

            written_length += status;
            from = &to[1];
        }
    }
        break;

    case FIO_EOL__StreamCRLF:
    {
        // find each LF and output the text followed by a cR/LF
        const unsigned char *from = buf;
        const unsigned char *end = &buf[len];

        while( from < end )
        {
            const unsigned char *to = (const unsigned char *)memchr( from, '\n', end - from );
            if( to == NULL )
            {
                int status = fwrite( from, 1, end-from, m_file );
                if( ferror( m_file ) )
                    return -1;
                written_length += status;
                break;
            }

            int status = fwrite( from, 1, to-from, m_file );
            if( ferror( m_file ) )
                return -1;
            written_length += status;

            status = fwrite( "\r\n", 1, 2, m_file );
            if( ferror( m_file ) )
                return -1;
            written_length += status;
            from = &to[1];
        }
    }
        break;


    case FIO_EOL__Binary:
        // simply output as is
    case FIO_EOL__StreamLF:
    {
        // simply output as is
        int status = fwrite( buf, 1, len, m_file );
        if( ferror( m_file ) )
            return -1;
        written_length += status;
    }
        break;

    default:
        // cannot happen
        emacs_assert(false);
    }

    return written_length;
}

#ifdef vms
int EmacsFileLocal::fio_split_put
    (
    const unsigned char *buf1, int len1,
    const unsigned char *buf2, int len2
    )
{
    int status1 = fio_put( buf1, len1 );
    if( status1 < 0 )
        return -1;

    int status2 = fio_put( buf2, len2 );
    if( status2 < 0 )
        return -1;

    return status1 + status2;
}
#endif

//--------------------------------------------------------------------------------
//
//  Unicode file io API
//
//--------------------------------------------------------------------------------
int EmacsFileLocal::fio_get( EmacsChar_t *buf, int len )
{
    if( m_convert_size < CONVERT_BUFFER_SIZE )
    {
        int size = fio_get( m_convert_buffer + m_convert_size, CONVERT_BUFFER_SIZE - m_convert_size );
        if( size <= 0 )
            return size;

        m_convert_size += size;
    }

    if( m_encoding_attr == FIO_Encoding_UTF_8 )
    {
        int utf8_usable_len = 0;
        int unicode_len = length_utf8_to_unicode(
                m_convert_size, m_convert_buffer,   // convert these bytes
                len,                                // upto this number of unicode chars
                utf8_usable_len );                  // and return the number of bytes required

        // convert
        convert_utf8_to_unicode( m_convert_buffer, unicode_len, buf );

        // remove converted chars from the buffer
        m_convert_size -= utf8_usable_len;
        memmove( m_convert_buffer, m_convert_buffer+utf8_usable_len, m_convert_size );

        return unicode_len;
    }
    else
    {
        int utf16_usable_len = 0;
        int unicode_len = length_utf16_to_unicode(
                m_convert_size, m_convert_buffer,   // convert these bytes
                len,                                // upto this number of unicode chars
                utf16_usable_len );                 // and return the number of bytes required

        // convert
        convert_utf16_to_unicode( m_convert_buffer, unicode_len, buf );

        // remove converted chars from the buffer
        m_convert_size -= utf16_usable_len;
        memmove( m_convert_buffer, m_convert_buffer+utf16_usable_len, m_convert_size );

        return unicode_len;
    }
}

int EmacsFileLocal::fio_put( const EmacsChar_t *buf, int len )
{
    int written_length = 0;
    while( len > 0 )
    {
        int unicode_usable_length = 0;
        int utf8_length = length_unicode_to_utf8( len, buf, CONVERT_BUFFER_SIZE, unicode_usable_length );

        convert_unicode_to_utf8( unicode_usable_length, buf, m_convert_buffer );
        int bytes_written = fio_put( m_convert_buffer, utf8_length );
        if( bytes_written <= 0 )
            return bytes_written;

        written_length += unicode_usable_length;
        len -= unicode_usable_length;
        buf += unicode_usable_length;
    }

    return written_length;
}

//--------------------------------------------------------------------------------

void EmacsFileLocal::fio_flush()
{
    fflush( m_file );
}

bool EmacsFileLocal::fio_close()
{
    int status = fclose( m_file );
    m_file = NULL;

    return status == 0;
}

long int EmacsFileLocal::fio_size()
{
    long int cur_pos, end_of_file_pos;

    // find the current position
    cur_pos = ftell( m_file );

    // seek to the end of the file
    if( fseek( m_file, 0l, SEEK_END ) == 0 )
    {
        // the current position is the size of the file
        end_of_file_pos = ftell( m_file );
    }
    else
    {
        _dbg_msg( "fseek failed!" );
        end_of_file_pos = 0l;
    }

    // seek back to the orginal position
    fseek( m_file, cur_pos, SEEK_SET );

    return end_of_file_pos;
}

const EmacsString &EmacsFileLocal::fio_getname()
{
    return m_fab.result_spec;
}

time_t EmacsFileLocal::fio_modify_date()
{
    EmacsFileStat s;

    if( !s.stat( m_file ) )
        return 0;

    return s.data().st_mtime;
}

time_t EmacsFileLocal::fio_file_modify_date()
{
    EmacsFileStat s;

    if( !s.stat( m_fab.result_spec ) )
        return 0;

    return s.data().st_mtime;
}

// return true if the file mode is read only for this use
int EmacsFileLocal::fio_access_mode()
{
    EmacsFileStat s;

    s.stat( m_file );

    return (s.data().st_mode&_S_IWRITE) == 0;
}

template <typename T> static FIO_EOL_Attribute detectEolType( unsigned char *ch_buf, int ch_buf_len )
{
    T *buf = reinterpret_cast<T *>( ch_buf );
    int len = ch_buf_len/sizeof( T );

    //
    //    this makes a snap judgement of the buffers record type
    //
    int nl_index = -1;
    int cr_index = -1;
    for( int i=0; i<len; ++i )
    {
        if( nl_index < 0 && buf[i] == '\n' )
        {
            nl_index = i;
        }

        if( cr_index < 0 && buf[i] == '\r' )
        {
            cr_index = i;
        }

        if( nl_index >= 0 && cr_index >= 0 )
        {
            break;
        }
    }

    if( nl_index > 0 && (cr_index == nl_index-1) )
    {
        return FIO_EOL__StreamCRLF;
    }

    if( nl_index >= 0 )
    {
        return FIO_EOL__StreamLF;
    }

    if( cr_index >= 0 )
    {
        return FIO_EOL__StreamCR;
    }

    return FIO_EOL__None;
}

template <typename T> static int stripCr( unsigned char *ch_buf, int ch_buf_len )
{
    T *buf = reinterpret_cast<T *>( ch_buf );
    int len = ch_buf_len/sizeof( T );

    // strip CR's from the buf
    T *end = &buf[ len ];
    T *put = buf;
    T *get = put;

    while( get < end )
    {
        T c = *get++;
        if( c != '\r' )
        {
            *put++ = c;
        }
    }

    // return the length in bytes
    return (put - buf)*sizeof( T );
}

template <typename T> static void replaceCrWithNl( unsigned char *ch_buf, int ch_buf_len )
{
    // strip CR's from the buf

    T *buf = reinterpret_cast<T *>( ch_buf );
    int len = ch_buf_len/sizeof( T );

    for( int i=0; i<len; ++i )
    {
        if( buf[i] == '\r' )
        {
            buf[i] = '\n';
        }
    }
}

int EmacsFileLocal::get_fixup_buffer( unsigned char *buf, int len )
{
    if( m_encoding_attr == FIO_Encoding_None && len >= 2 )
    {
        // look for the utf-16 BOM
        if( buf[0] == 0xff && buf[1] == 0xfe )
        {
            m_encoding_attr = FIO_Encoding_UTF_16_LE;
            len -= 2;
            memmove( buf, buf+2, len );
        }
        else if( buf[0] == 0xfe && buf[1] == 0xff )
        {
            m_encoding_attr = FIO_Encoding_UTF_16_BE;
            len -= 2;
            memmove( buf, buf+2, len );
        }
        else
        {
            m_encoding_attr = FIO_Encoding_UTF_8;
        }
    }

    // convert to native - assuming little endian
    if( m_encoding_attr == FIO_Encoding_UTF_16_BE )
    {
        for( int i=0; i<len; i += 2 )
        {
            std::swap( buf[i], buf[i+1] );
        }
    }

    if( m_eol_attr == FIO_EOL__None )
    {
        if( m_encoding_attr == FIO_Encoding_UTF_8 )
        {
            m_eol_attr = detectEolType<unsigned char>( buf, len );
        }
        else
        {
            m_eol_attr = detectEolType<unsigned short>( buf, len );
        }
    }

    switch( m_eol_attr )
    {
    default:
        // default never gets hit...
        emacs_assert( false );

    case FIO_EOL__None:
        // no CR or LF in the buf so return the orginal len
        return len;

    case FIO_EOL__Binary:
        // no change required
        return len;

    case FIO_EOL__StreamLF:
        // no change required
        return len;

    case FIO_EOL__StreamCR:
    {
        if( m_encoding_attr == FIO_Encoding_UTF_8 )
        {
            replaceCrWithNl<unsigned char>( buf, len );
        }
        else
        {
            replaceCrWithNl<unsigned short>( buf, len );
        }
        return len;
    }

    case FIO_EOL__StreamCRLF:
    {
        if( m_encoding_attr == FIO_Encoding_UTF_8 )
        {
            return stripCr<unsigned char>( buf, len );
        }
        else
        {
            return stripCr<unsigned short>( buf, len );
        }
    }
    }
}
