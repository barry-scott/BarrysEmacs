//
//    unixfile.c
//    Copyright 1993-2004 Barry A. Scott
//
#include <emacs.h>
#include <em_stat.h>

# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

FileNameCompareCaseSensitive file_name_compare_case_sensitive;
FileNameCompareCaseBlind file_name_compare_case_blind;
FileNameCompare *file_name_compare = &file_name_compare_case_sensitive;

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>

bool isValidFilenameChar( EmacsChar_t ch )
{
    EmacsString invalid( "/\000" );
    return invalid.index( ch ) < 0;
}


EmacsFileLocal::EmacsFileLocal( EmacsFile &parent, FIO_EOL_Attribute attr )
: EmacsFileImplementation( parent )
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
    if( m_parent.parse_is_valid() )
    {
        //  6 means read and write, 4 means read
        int r = access( m_parent.result_spec, 6 );
        if( r == 0 )
            return 1;

        r = access( m_parent.result_spec, 4 );
        if( r == 0 )
            return -1;
    }

    return 0;
}

bool EmacsFileLocal::fio_file_exists()
{
    int r = -1;         // assume not valid
    if( m_parent.parse_is_valid() )
    {
        r = access( m_parent.result_spec, 0 );
    }

    return r != -1;     // true if the file exists
}

int EmacsFileLocal::fio_delete()
{
    int r = 1;          // assume not valid
    if( m_parent.parse_is_valid() )
    {
        r = unlink( m_parent.result_spec );
    }

    return r;
}

bool EmacsFileLocal::fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr )
{
    m_file = fopen( m_parent.result_spec, "w" BINARY_MODE SHARE_NONE );
    m_eol_attr = attr;

    return m_file != NULL;
}

bool EmacsFileLocal::fio_open( bool eof, FIO_EOL_Attribute attr )
{
    if( !fio_is_regular() )
    {
        return false;
    }

    if( eof )
    {
        // open for append
        m_file = fopen( m_parent.result_spec, "a" BINARY_MODE SHARE_NONE );
        m_eol_attr = attr;
    }
    else
    {
        // open for read
        m_file = fopen( m_parent.result_spec, "r" BINARY_MODE SHARE_READ );
    }

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
    return m_parent.result_spec;
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

    if( !s.stat( m_parent.result_spec ) )
        return 0;

    return s.data().st_mtime;
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

bool EmacsFileLocal::fio_is_directory( const EmacsString &filename )
{
    EmacsFileStat s;
    if( s.stat( filename.sdata() ) )
    {
        return S_ISDIR( s.data().st_mode );
    }

    return false;
}

bool EmacsFileLocal::fio_is_directory()
{
    if( !m_parent.parse_is_valid() )
    {
        return false;
    }

    if( m_parent.result_spec[-1] == PATH_CH )
    {
        return true;
    }

    EmacsFileStat s;
    if( s.stat( m_parent.result_spec ) )
    {
        return s.is_directory();
    }

    return false;
}

bool EmacsFileLocal::fio_is_regular()
{
    if( !m_parent.parse_is_valid() )
    {
        return false;
    }

    EmacsFileStat s;
    if( s.stat( m_parent.result_spec ) )
    {
        return s.is_regular();
    }

    return false;
}

// input name in nm, absolute pathname output to buf.  returns -1 if the
// pathname cannot be successfully converted (only happens if the
// current directory cannot be found)
//
#if DBG_TMP && 0
# define Trace( s ) do { if( dbg_flags&DBG_TMP ) { _dbg_msg( s ); } } while(0)
#else
# define Trace( s ) // do nothing
#endif

static void expand_tilda_path( const EmacsString &in_path, EmacsString &out_path)
{
    EmacsString expanded_in_path( in_path )

    Trace( FormatString("expand_tilda_path( %s )") << in_path );

    char c_def_path[1+MAXPATHLEN+1];

    c_def_path[0] = '\0';
    getcwd( c_def_path, sizeof(c_def_path) );

    EmacsString def_path( c_def_path );

    int in_pos = 0;
    if( expanded_in_path.length() >= 2
    && expanded_in_path[0] == '~' )                 // prefix ~
    {
        if( expanded_in_path[1] == PATH_CH )        // ~/filename
        {
            char *value = getenv( "HOME" );
            if( value != NULL )
            {
                in_pos = 2;
                out_path.append( EmacsString( value ) );
            }
            else
            {
                out_path.append( def_path );
            }
        }
        else
        {   // ~user/filename
            int path_char_pos = expanded_in_path.first( PATH_CH );
            if( path_char_pos < 0 )
            {
                out_path.append( def_path );
            }
            else
            {
                EmacsString user( expanded_in_path( 1, path_char_pos ) );
                struct passwd *pw = getpwnam( user );
                if( pw == NULL )
                {
                    out_path.append( def_path );
                }
                else
                {
                    out_path.append( EmacsString( pw->pw_dir ) );
                }
                in_pos = path_char_pos;
            }
        }
    }
    else
    {
        if( expanded_in_path.length() >= 1
        && expanded_in_path[0] != '/' )
        {
            out_path.append( def_path );
        }
    }

    if( out_path.length() >= 1
    && out_path[-1] != PATH_CH )
    {
        out_path.append( PATH_CH );
    }

    out_path.append( expanded_in_path( in_pos, expanded_in_path.length() ) );

    Trace( FormatString( "expand_tilda_path: out_path before => %s" ) << out_path );

    for( int pos=0; pos<out_path.length(); )
    {
        if( out_path[pos] == PATH_CH )
        {
            Trace( FormatString( "expand_tilda_path: pos: %d, len: %d out_path => %s|%s" )
                << pos << out_path.length()
                << out_path( 0, pos )
                << out_path( pos, out_path.length() ) );

            if( (pos+1) >= out_path.length() )
            {
                break;
            }

            switch( out_path[pos+1] )
            {
            case PATH_CH:        // found // in the name
                Trace( FormatString("expand_tilda_path: remove( %d, 1 )") << pos );
                out_path.remove( pos, 1 );
                break;

            case '.':
                if( (pos+2) < out_path.length() )
                    switch( out_path[pos+2] )
                    {
                    case PATH_CH:    // found /./ in the name
                        Trace( FormatString("expand_tilda_path: remove( %d, 2 )") << pos );
                        out_path.remove( pos, 2 );
                        break;

                    case '.':
                        if( (pos+3) == out_path.length()
                        || ((pos+3) < out_path.length()
                            && out_path[pos+3] == PATH_CH) )
                        {
                            int remove = 3;
                            Trace( "expand_tilda_path: found /../" );
                            // found /../
                            while( pos > 0 && out_path[pos-1] != PATH_CH )
                            {
                                pos--;
                                remove++;
                            }
                            if( pos > 0 )
                            { pos--; remove++; }
                            Trace( FormatString("expand_tilda_path: remove( %d, %d ) => %s" )
                                << pos << remove << out_path( pos, pos + remove ) );
                            out_path.remove( pos, remove );
                            Trace( FormatString("expand_tilda_path: found /../ %s")
                                << out_path );
                        }
                        else
                        {
                            pos++;
                        }
                        break;

                    default:
                        pos++;
                    }

                else
                    pos++;
                break;

            default:
                pos++;
                break;
            }
        }

        else
        {
            pos++;
        }
    }

    Trace( FormatString( "expand_tilda_path: out_path after => %s" ) << out_path );
}


#if defined( _POSIX_VERSION )
# define struct_direct struct dirent
#else
# define struct_direct struct direct
#endif

class FileFindUnix : public FileFindInternal
{
public:
    FileFindUnix( const EmacsString &files, bool return_all_directories );
    virtual ~FileFindUnix();

    EmacsString next();
private:
    const EmacsString files;
    enum { first_time, next_time, all_done } state;
    EmacsString root_path;
    EmacsString match_pattern;
    EmacsString full_filename;

    DIR *find;
};

FileFind::FileFind( const EmacsString &files, bool return_all_directories )
{
    implementation = new FileFindUnix( files, return_all_directories );
}

FileFind::~FileFind()
{
    delete implementation;
}

EmacsString FileFind::next()
{
    if( implementation )
        return implementation->next();
    return EmacsString::null;
}


FileFindUnix::FileFindUnix( const EmacsString &_files, bool return_all_directories )
: FileFindInternal( return_all_directories )
, files( _files )
, state( all_done )    // assume all done
, root_path()
, match_pattern()
, full_filename()
, find( NULL )
{
    EmacsFile fab( files );
    if( !fab.parse_is_valid() )
    {
        return;
    }

    root_path = fab.result_spec;

    // now its possible to get the first file
    state = first_time;

    int last_path_ch = root_path.last( PATH_CH );
    if( last_path_ch >= 0 )
    {
        match_pattern = root_path( last_path_ch+1, INT_MAX );
        root_path.remove( last_path_ch+1 );
    }
}

FileFindUnix::~FileFindUnix()
{
    if( find )
    {
        closedir( find );
    }
}

EmacsString FileFindUnix::next()
{
    switch( state )
    {
    default:
    case all_done:
        return EmacsString::null;

    case first_time:
        if( match_pattern.isNull() )
        {
            state = all_done;
            return root_path;
        }

        find = opendir( root_path );
        if( find == NULL )
        {
            state = all_done;
            return EmacsString::null;
        }

        state = next_time;
        // fall into next_time

    case next_time:
    {
        // read entries looking for a match
        struct_direct *d;
        while( (d = readdir(find)) != NULL )
        {
            if( d->d_ino == 0 )
            {
                continue;
            }

            // do not return . and .. entries
            if( strcmp( d->d_name, "." ) == 0
            || strcmp( d->d_name, ".." ) == 0 )
            {
                continue;
            }

            full_filename = root_path;
            full_filename.append( d->d_name );

            EmacsFile fd( full_filename );
            bool is_dir = fd.fio_is_directory();

            // always return directories
            if( return_all_directories && is_dir )
            {
                full_filename.append( PATH_STR );
                return full_filename;
            }

            // the name matches the pattern
            if( match_wild( d->d_name, match_pattern ) )
            {
                if( !return_all_directories && is_dir )
                {
                    full_filename.append( PATH_STR );
                }

                // return success and the full path
                return full_filename;
            }
        }
        state = all_done;
    }
        break;
    }

    return EmacsString::null;
}

//  QQQ: copy of sys_parse for the new EmacsFile
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
}

bool EmacsFile::parse_filename( const EmacsString &name, const EmacsString &def )
{
    EmacsFile def_file;

    parse_valid = false;

    if( !parse_analyse_filespec( name ) )
    {
        return false;
    }

    if( !def_file.parse_analyse_filespec( def ) )
    {
        return false;
    }

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = true;
    filename_maxlen = 128;
    filetype_maxlen = 128;

    if( path.isNull() )
    {
        if( !def_file.path.isNull() )
        {
            path = def_file.path;
        }
        else
        {
            char def_path[1+MAXPATHLEN+1];
            char *r = getcwd( def_path, sizeof(def_path) );

            if( r )
            {
                path = def_path;
            }

            if( path[-1] != '/' )
            {
                path.append( "/" );
            }
        }
    }

    //
    //    See if the filename is infact a directory.
    //    If it is a directory move the filename on to
    //    the end of the path and null filename.
    //
    if( !filename.isNull()
    ||  !filetype.isNull() )
    {
        EmacsString fullspec( FormatString("%s%.*s%.*s") <<
            path <<
            filename_maxlen << filename <<
            filetype_maxlen << filetype );

        // get attributes
        if( impl->fio_is_directory( fullspec ) )
        {
            // need to merge the filename on to the path
            path = FormatString("%s%.*s%.*s") <<
                path <<
                filename_maxlen << filename <<
                filetype_maxlen << filetype;

            filename = EmacsString::null;
            filetype = EmacsString::null;

            if( path[-1] != '/' )
            {
                path.append( "/" );
            }
        }
    }

    if( filename.isNull() )
    {
        filename = def_file.filename;
    }

    if( filetype.isNull() )
    {
        filetype = def_file.filetype;
    }

    EmacsString fn_buf = FormatString("%s%.*s%.*s") <<
        path <<
        filename_maxlen << filename <<
        filetype_maxlen << filetype;

    expand_tilda_path( fn_buf, result_spec );
    parse_valid = true;
    return true;
}

// QQQ: copy of analyse_filespec for the new EmacsFile
bool EmacsFile::parse_analyse_filespec( const EmacsString &filespec )
{
    int device_loop_max_iterations = 10;

    parse_init();

    EmacsString sp = filespec;

    int disk_end;
    for(;;)
    {
        disk_end = sp.first(':');
        if( disk_end > 0 )
        {
            disk = sp( 0, disk_end );
            disk_end++;

            //
            // if there is a replacement string use it otherwise
            // leave the device name as it is
            //
            EmacsString new_value = get_config_env( disk );
            if( new_value.isNull() )
            {
                disk = EmacsString::null;
                disk_end = 0;
                break;
            }
            else
            {
                // we are replacing the disk so zap any
                // left over disk
                disk = EmacsString::null;

                if( new_value[-1] != PATH_CH )
                {
                    new_value.append( PATH_STR );
                }

                // add the rest of the file spec to the buffer
                new_value.append( sp( disk_end, INT_MAX ) );
                // setup the pointer to the file spec to convert
                sp = new_value;
                // go do the analysis again
                device_loop_max_iterations--;
                if( device_loop_max_iterations > 0 )
                {
                    continue;
                }
            }
        }
        else
        {
            disk_end = 0;
        }
        break;
    }

    // any "disk" here is a remote host
    // localhost:file.ext
    // switch to using remote parsing
    disk_end = sp.first(':');
    if( disk_end > 0 )
    {
        remote_host = sp( 0, disk_end );
        disk_end++;
        sp = sp( disk_end, INT_MAX );

        // QQQ Once we implement remote
        // delete impl
        // impl = EmacsFileRemote(*this)
    }
    else
    {
        disk_end = 0;
    }

    if( impl->fio_is_directory( sp ) )
    {
        // all of sp is a path
        path = sp;
        if( path[-1] != PATH_CH )
        {
            path.append( PATH_STR );
        }
    }
    else
    {
        int path_end = sp.last( PATH_CH );
        if( path_end < 0 )
        {
            path_end = disk_end;
        }
        else
        {
            path_end++;
        }

        path = sp( disk_end, path_end );

        int filename_end = sp.last( '.', path_end );
        if( filename_end < 0 )
        {
            filename_end = sp.length();
        }

        filename = sp( path_end, filename_end );
        filetype = sp( filename_end, INT_MAX );
    }
    return true;
}
