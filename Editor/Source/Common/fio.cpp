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

EmacsFileImplementation::EmacsFileImplementation( EmacsFile &parent, FIO_EOL_Attribute attr )
: EmacsObject()
, m_parent( parent )
, m_eol_attr( attr )
, m_encoding_attr( FIO_Encoding_None )
, m_convert_size( 0 )
, m_convert_buffer( new unsigned char[ CONVERT_BUFFER_SIZE ] )
{ }

EmacsFileImplementation::~EmacsFileImplementation()
{ }

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

int EmacsFileImplementation::get_fixup_buffer( unsigned char *buf, int len )
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

EmacsFile::EmacsFile( const EmacsString &filename, const EmacsString &def, FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, m_impl( 0 )
{
    TraceFile( FormatString("EmacsFile[%d]::EmacsFile( '%s', '%s', %d )")
                    << objectNumber() << filename << def << attr );

    m_impl = EmacsFileImplementation::factoryEmacsFileLocal( *this, attr );
    parse_filename( filename, def );

    TraceFile( FormatString("EmacsFile[%d]::%s")
                    << objectNumber() << repr() );
}

EmacsFile::EmacsFile( const EmacsString &filename, FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, m_impl( 0 )
{
    TraceFile( FormatString("EmacsFile[%d]::EmacsFile( '%s', %d )")
                    << objectNumber() << filename << attr );

    m_impl = EmacsFileImplementation::factoryEmacsFileLocal( *this, attr );
    parse_filename( filename, EmacsString::null );

    TraceFile( FormatString("EmacsFile[%d]::%s")
                    << objectNumber() << repr() );
}

EmacsFile::EmacsFile( FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, m_impl( 0 )
{
    TraceFile( FormatString("EmacsFile[%d]::EmacsFile( %d )")
                    << objectNumber() << attr );

    m_impl = EmacsFileImplementation::factoryEmacsFileLocal( *this, attr );
}

EmacsFile::~EmacsFile()
{
    TraceFile( FormatString("EmacsFile[%d]::~EmacsFile()")
                    << objectNumber() );
    delete m_impl;
}

EmacsString EmacsFile::repr()
{
    return FormatString("EmacsFile[%d]: parse_valid %d remote_host '%s' path '%s' name '%s' type '%s' result_spec '%s'")
                    << objectNumber() << parse_valid << remote_host << path << filename << filetype << result_spec;
}

void EmacsFile::parse_init()
{
    remote_host = EmacsString::null;
    disk = EmacsString::null;
    path = EmacsString::null;
    filename = EmacsString::null;
    filetype = EmacsString::null;
    result_spec = EmacsString::null;

    parse_valid = false;
}

bool EmacsFile::parse_is_valid()
{
    return parse_valid;
}

// transfer the m_file from other to this
void EmacsFile::fio_set_filespec_from( EmacsFile &other )
{
    remote_host = other.remote_host;
    disk = other.disk;
    path = other.path;
    filename = other.filename;
    filetype = other.filetype;
    result_spec = other.result_spec;

    parse_valid = other.parse_valid;

#if defined(SFTP)
    if( !remote_host.isNull() )
    {
        FIO_EOL_Attribute eol_attr = other.fio_get_eol_attribute();
        delete m_impl;
        m_impl = EmacsFileImplementation::factoryEmacsFileRemote( *this, eol_attr );
    }
#endif
}

FileFind::FileFind( EmacsFile *files, bool return_all_directories )
: EmacsObject()
, m_files( files )
, m_impl( NULL )
{
    m_impl = files->factoryFileFindImplementation( return_all_directories );
}

FileFind::~FileFind()
{
    delete m_impl;
    delete m_files;
}

const EmacsString &FileFind::matchPattern() const
{
    return m_impl->matchPattern();
}


EmacsString FileFind::next()
{
    if( m_impl )
    {
        return m_impl->next();
    }

    return EmacsString::null;
}

EmacsString FileFind::repr()
{
    if( m_impl )
    {
        return FormatString("FileFind %p impl %s") << this << m_impl->repr();
    }

    return FormatString("FileFind %p impl NULL") << this;
}
