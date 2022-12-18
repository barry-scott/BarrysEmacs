//
//    unix_file_local.c
//    Copyright 1993-2004 Barry A. Scott
//
#include <emacs.h>
#include <em_stat.h>

#undef THIS_FILE
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

#if defined( _POSIX_VERSION )
# define struct_direct struct dirent
#else
# define struct_direct struct direct
#endif

bool isValidFilenameChar( EmacsChar_t ch )
{
    EmacsString invalid( "/\000" );
    return invalid.index( ch ) < 0;
}

class EmacsFileLocal;

class FileFindLocal : public FileFindImplementation
{
public:
    EMACS_OBJECT_FUNCTIONS( FileFindlocal )

    FileFindLocal( EmacsFile &files, EmacsFileLocal &local, bool return_all_directories );
    virtual ~FileFindLocal();

    EmacsString next();
    EmacsString repr();

private:
    EmacsFileLocal &m_local;
    DIR *m_find;
};

class EmacsFileLocal : public EmacsFileImplementation
{
public:
    EmacsFileLocal( EmacsFile &parent, FIO_EOL_Attribute attr );
    virtual ~EmacsFileLocal();

    virtual EmacsString repr();

    virtual bool fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr );
    virtual bool fio_open( bool eof=false, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        m_file = existing_file;
        m_eol_attr = attr;
        return true;
    }
    virtual bool fio_find_using_path( const EmacsString &path, const EmacsString &fn, const EmacsString &ex );
    virtual bool fio_is_open()
    {
        return m_file != NULL;
    }

    // Old 8bit chars
    virtual int fio_get( unsigned char *, int );
    virtual int fio_get_line( unsigned char *buf, int len );
    virtual int fio_get_with_prompt( unsigned char *buffer, int size, const unsigned char *prompt );

    virtual int fio_put( const unsigned char *, int );

    // Unicode chars
    virtual int fio_get( EmacsChar_t *, int );
    //int fio_get_line( EmacsChar_t *buf, int len );
    //int fio_get_with_prompt( EmacsChar_t *buffer, int size, const EmacsChar_t *prompt );

    virtual int fio_put( const EmacsChar_t *, int );

    virtual int fio_put( const EmacsString & );

    virtual bool fio_close();
    virtual void fio_flush();

    virtual long int
        fio_size();
    virtual time_t
        fio_modify_date();
    virtual EmacsString
        fio_getname();

    virtual int
        fio_access();
    virtual bool
        fio_file_exists();
    virtual int
        fio_delete();
    virtual time_t
        fio_file_modify_date();
    virtual bool
        fio_is_directory();
    virtual bool
        fio_is_regular();

    virtual FileFindImplementation *
        factoryFileFindImplementation( bool return_all_directories );

    // helpers
    virtual bool
        fio_is_directory( const EmacsString &filename );
    virtual EmacsString
        fio_cwd();
    virtual EmacsString
        fio_home_dir()
    {
        return m_home_dir;
    }

private:
    FILE *m_file;
    EmacsString m_home_dir;
};

FileFindImplementation *EmacsFileLocal::factoryFileFindImplementation( bool return_all_directories )
{
    return EMACS_NEW FileFindLocal( m_parent, *this, return_all_directories );
}

EmacsFileImplementation *EmacsFileImplementation::factoryEmacsFileLocal( EmacsFile &file, FIO_EOL_Attribute attr )
{
    return EMACS_NEW EmacsFileLocal( file, attr );
}

EmacsFileLocal::EmacsFileLocal( EmacsFile &parent, FIO_EOL_Attribute attr )
: EmacsFileImplementation( parent, attr )
, m_file( NULL )
, m_home_dir( get_config_env( "HOME" ) )
{
    if( m_home_dir.isNull() )
    {
        m_home_dir = fio_cwd();
    }
}

EmacsFileLocal::~EmacsFileLocal()
{
    if( m_file != NULL && m_file != stdin )
    {
        fclose( m_file );
    }

    delete [] m_convert_buffer;
}

EmacsString EmacsFileLocal::repr()
{
    return FormatString("EmacsFileLocal %p: m_file %p")
                << this << m_file;
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
    if( fio_is_open() )
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
            TraceFile( "fseek failed!" );
            end_of_file_pos = 0l;
        }

        // seek back to the orginal position
        fseek( m_file, cur_pos, SEEK_SET );

        return end_of_file_pos;
    }
    else
    {
        EmacsFileStat s;
        if( !s.stat( m_parent.result_spec ) )
        {
            return 0;
        }

        return s.data().st_size;
    }
}

EmacsString EmacsFileLocal::fio_getname()
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

EmacsString EmacsFileLocal::fio_cwd()
{
    char buf[65536];
    char *cwd = ::getcwd( buf, sizeof(buf) );
    if( cwd == NULL )
    {
        return EmacsString( "." );
    }

    return EmacsString( EmacsString::copy, cwd );
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

void EmacsFile::expand_tilda_path( const EmacsString &in_path, EmacsString &out_path )
{
    EmacsString expanded_in_path( in_path )

    TraceFileTmp( FormatString("expand_tilda_path( %s )") << in_path );

    int in_pos = 0;
    if( expanded_in_path.length() >= 2
    && expanded_in_path[0] == '~' )                 // prefix ~
    {
        if( expanded_in_path[1] == PATH_CH )        // ~/filename
        {
            in_pos = 2;
            out_path.append( m_impl->fio_home_dir() );
        }
        else
        {   // ~user/filename
            // QQQ does not work for EmacsFileRemote
            int path_char_pos = expanded_in_path.first( PATH_CH );
            if( path_char_pos < 0 )
            {
                out_path.append( m_impl->fio_cwd() );
            }
            else
            {
                EmacsString user( expanded_in_path( 1, path_char_pos ) );
                struct passwd *pw = getpwnam( user );
                if( pw == NULL )
                {
                    out_path.append( m_impl->fio_cwd() );
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
            out_path.append( m_impl->fio_cwd() );
        }
    }

    if( out_path.length() >= 1
    && out_path[-1] != PATH_CH )
    {
        out_path.append( PATH_CH );
    }

    out_path.append( expanded_in_path( in_pos, expanded_in_path.length() ) );

    TraceFileTmp( FormatString( "expand_tilda_path: out_path before => %s" ) << out_path );

    for( int pos=0; pos<out_path.length(); )
    {
        if( out_path[pos] == PATH_CH )
        {
            TraceFileTmp( FormatString( "expand_tilda_path: pos: %d, len: %d out_path => %s|%s" )
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
                TraceFileTmp( FormatString("expand_tilda_path: remove( %d, 1 )") << pos );
                out_path.remove( pos, 1 );
                break;

            case '.':
                if( (pos+2) < out_path.length() )
                    switch( out_path[pos+2] )
                    {
                    case PATH_CH:    // found /./ in the name
                        TraceFileTmp( FormatString("expand_tilda_path: remove( %d, 2 )") << pos );
                        out_path.remove( pos, 2 );
                        break;

                    case '.':
                        if( (pos+3) == out_path.length()
                        || ((pos+3) < out_path.length()
                            && out_path[pos+3] == PATH_CH) )
                        {
                            int remove = 3;
                            TraceFileTmp( "expand_tilda_path: found /../" );
                            // found /../
                            while( pos > 0 && out_path[pos-1] != PATH_CH )
                            {
                                pos--;
                                remove++;
                            }
                            if( pos > 0 )
                            { pos--; remove++; }
                            TraceFileTmp( FormatString("expand_tilda_path: remove( %d, %d ) => %s" )
                                << pos << remove << out_path( pos, pos + remove ) );
                            out_path.remove( pos, remove );
                            TraceFileTmp( FormatString("expand_tilda_path: found /../ %s")
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

    TraceFileTmp( FormatString( "expand_tilda_path: out_path after => %s" ) << out_path );
}

FileFindLocal::FileFindLocal( EmacsFile &files, EmacsFileLocal &local, bool return_all_directories )
: FileFindImplementation( files, return_all_directories )
, m_local( local )
, m_find( NULL )
{
    TraceFile( FormatString("FileFindLocal this %s") << repr() );
    TraceFile( FormatString("FileFindLocal m_files %s") << m_files.repr() );
    TraceFile( FormatString("FileFindLocal m_local %s") << m_local.repr() );

    if( !m_files.parse_is_valid() )
    {
        return;
    }

    m_root_path = m_files.result_spec;

    // now its possible to get the first file
    m_state = first_time;

    int last_path_ch = m_root_path.last( PATH_CH );
    if( last_path_ch >= 0 )
    {
        m_match_pattern = m_root_path( last_path_ch+1, INT_MAX );
        m_root_path.remove( last_path_ch+1 );
    }
}

FileFindLocal::~FileFindLocal()
{
    if( m_find )
    {
        closedir( m_find );
    }
}

EmacsString FileFindLocal::repr()
{
    return FormatString("FileFindLocal (%p) state %d root %s pattern %s full_filename %s")
            << this
            << m_state
            << m_root_path
            << m_match_pattern
            << m_full_filename;
}

EmacsString FileFindLocal::next()
{
    switch( m_state )
    {
    default:
    case all_done:
        return EmacsString::null;

    case first_time:
        if( m_match_pattern.isNull() )
        {
            m_state = all_done;
            return m_root_path;
        }

        m_find = opendir( m_root_path );
        if( m_find == NULL )
        {
            m_state = all_done;
            return EmacsString::null;
        }

        m_state = next_time;
        // fall into next_time

    case next_time:
    {
        // read entries looking for a match
        struct_direct *d;
        while( (d = readdir( m_find )) != NULL )
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

            m_full_filename = m_root_path;
            m_full_filename.append( d->d_name );

            bool is_dir = m_local.fio_is_directory( m_full_filename );

            // always return directories
            if( m_return_all_directories && is_dir )
            {
                m_full_filename.append( PATH_STR );
                return m_full_filename;
            }

            // the name matches the pattern
            if( match_wild( d->d_name, m_match_pattern ) )
            {
                if( !m_return_all_directories && is_dir )
                {
                    m_full_filename.append( PATH_STR );
                }

                // return success and the full path
                return m_full_filename;
            }
        }
        m_state = all_done;
    }
        break;
    }

    return EmacsString::null;
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
            path = m_impl->fio_cwd();
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
        if( m_impl->fio_is_directory( fullspec ) )
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

bool EmacsFile::parse_analyse_filespec( const EmacsString &filespec )
{
    TraceFile( FormatString("EmacsFile::parse_analyse_filespec[%d]( '%s' )")
                << objectNumber() << filespec );

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

#if defined( SFTP )
    // any "disk" here is a remote host
    // localhost:file.ext
    // switch to using remote parsing
    disk_end = sp.first(':');
    if( disk_end > 0 )
    {
        remote_host = sp( 0, disk_end );
        disk_end++;
        sp = sp( disk_end, INT_MAX );

        // switch to remote parsing
        TraceFile( FormatString("EmacsFile::parse_analyse_filespec[%d] switch to remote impl host '%s' filespec '%s'")
                    << objectNumber() << remote_host << filespec );

        FIO_EOL_Attribute eol_attr = m_impl->fio_get_eol_attribute();
        delete m_impl;
        m_impl = EmacsFileImplementation::factoryEmacsFileRemote( *this, eol_attr );
    }
#endif

    disk_end = 0;

    if( m_impl->fio_is_directory( sp ) )
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
            path_end = 0;
        }
        else
        {
            path_end++;
        }

        path = sp( 0, path_end );

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

// fio_find_using_path opens the file fn with the given IO mode using the given
// search path. It is used to to read in MLisp files via the
// executed-mlisp-file function.
bool EmacsFileLocal::fio_find_using_path
    (
    const EmacsString &path,
    const EmacsString &fn,
    const EmacsString &ex
    )
{
    //
    // check for Node, device or directory specs.
    // also allows any logical name through
    // If present then just open the file stright.
    //
    if( fn.first( PATH_CH ) >= 0
    || fn.first( ':' ) >= 0 )
    {
        // open the file
        EmacsFile fd( fn, ex );
        if( fd.fio_is_regular() )
        {
            m_parent.fio_set_filespec_from( fd );
            return true;
        }

        return false;
    }

    //
    // Otherwise, add the path onto the front of the filespec
    //
    int start = 0;
    int end = 0;

    while( start < path.length() )
    {
        end = path.index( PATH_SEP, start );
        if( end < 0 )
        {
            end = path.length();
        }

        EmacsString fnb( path( start, end ) );
        if( fnb[-1] != PATH_CH
        &&  fnb[-1] != ':' )
        {
            fnb.append( PATH_STR );
        }
        fnb.append( fn );

        EmacsFile fd( fnb, ex );
        if( fd.fio_is_regular() )
        {
            // move the FILE to *this
            m_parent.fio_set_filespec_from( fd );
            return true;
        }

        start = end;
        start++;
    }

    return false;
}
