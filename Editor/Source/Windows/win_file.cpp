//
//    windows File services.cpp
//    Copyright 1993-2018 Barry A. Scott
//
#include <emacs.h>
#include <em_stat.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

extern int get_file_parsing_override( const char *disk, int def_override );

static EmacsString convertShortPathToLongPath( const EmacsString &short_path );


FileNameCompareCaseSensitive file_name_compare_case_sensitive;
FileNameCompareCaseBlind file_name_compare_case_blind;
FileNameCompare *file_name_compare = &file_name_compare_case_blind;

#include    <win_incl.h>
#include    <direct.h>

bool isValidFilenameChar( EmacsChar_t ch )
{
    EmacsString invalid( "\\:/\000?<>*|\"" );
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
    WIN32_FIND_DATA m_find;
    HANDLE m_handle;
    bool m_case_sensitive;
};

class EmacsFileLocal : public EmacsFileImplementation
{
public:
    EmacsFileLocal( EmacsFile &parent, FIO_EOL_Attribute attr );
    virtual ~EmacsFileLocal();

    virtual EmacsString repr();
    virtual bool isOk()
    {
        return true;
    }
    virtual bool isRemoteFile()
    {
        return false;
    }
    virtual EmacsString lastError()
    {
        return EmacsString::null;
    }

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
    m_file = _fsopen( m_parent.result_spec, "w" BINARY_MODE SHARE_NONE );
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
        m_file = _fsopen( m_parent.result_spec, "a" BINARY_MODE SHARE_NONE );
        m_eol_attr = attr;
    }
    else
    {
        // open for read
        m_file = _fsopen( m_parent.result_spec, "r" BINARY_MODE SHARE_READ );
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
    DWORD attr = GetFileAttributesW( filename.utf16_data() );
    if( attr == (unsigned)-1 )
        return false;

    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
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

    DWORD attr = GetFileAttributesW( m_parent.result_spec.utf16_data() );
    if( attr == (unsigned)-1 )
    {
        return false;
    }

    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool EmacsFileLocal::fio_is_regular()
{
    TraceFile( FormatString("EmacsFileLocal[%d]::fio_is_regular parse_valid %d result_spec %s")
        << objectNumber() << m_parent.parse_is_valid() << m_parent.result_spec );

    if( m_parent.result_spec.isNull() )
    {
        return false;
    }

    DWORD attr = GetFileAttributesW( m_parent.result_spec.utf16_data() );
    if( attr == (unsigned)-1 )
    {
        return false;
    }

    TraceFile( FormatString("EmacsFileLocal[%d]::fio_is_regular attr 0x%x FILE_ATTRIBUTE_DEVICE=0x%x << FILE_ATTRIBUTE_DIRECTORY=0x%x")
        << objectNumber() << attr << FILE_ATTRIBUTE_DEVICE << FILE_ATTRIBUTE_DIRECTORY );

    // regular if not device or directory
    return (attr & (FILE_ATTRIBUTE_DEVICE|FILE_ATTRIBUTE_DIRECTORY)) == 0;
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
    TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path( '%s', '%s', '%s' ) m_parent EmacsFile[%d]")
        << objectNumber() <<  path << fn << ex << m_parent.objectNumber() );

    //
    // check for Node, device or directory specs.
    // also allows any logical name through
    // If present then just open the file stright.
    //
    if( fn.first( PATH_CH ) >= 0
    || fn.first( ':' ) >= 0 )
    {
        TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path can use fn as is %s")
            << objectNumber() << fn );

        EmacsFile fd( fn, ex );
        if( fd.fio_is_regular() )
        {
            TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path 1 using '%s'")
                << objectNumber() << fd.result_spec );

            m_parent.fio_set_filespec_from( fd );
            return true;
        }

        TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path not a regular file %s")
            << objectNumber() << fd.result_spec );
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
            TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path 2 using '%s'")
                << objectNumber() << fd.result_spec );

            m_parent.fio_set_filespec_from( fd );
            return true;
        }

        start = end;
        start++;
    }

    TraceFile( FormatString("EmacsFileLocal[%d]::fio_find_using_path not found")
        << objectNumber() );
    return false;
}

FileFindImplementation *EmacsFileLocal::factoryFileFindImplementation( bool return_all_directories )
{
    return EMACS_NEW FileFindLocal( m_parent, *this, return_all_directories );
}

EmacsFileImplementation *EmacsFileImplementation::factoryEmacsFileLocal( EmacsFile &file, FIO_EOL_Attribute attr )
{
    return EMACS_NEW EmacsFileLocal( file, attr );
}

static unsigned char null[1] = {0};

bool EmacsFile::parse_analyse_filespec( const EmacsString &filespec )
{
    EmacsString sp;
    int device_loop_max_iterations = 10;

    parse_init();

    sp = filespec;
    for( int i=0; i<sp.length(); i++ )
    {
        if( sp[i] == PATH_ALT_CH )
        {
            sp[i] = PATH_CH;
        }
    }

device_loop:
    int disk_end = sp.first(':');
    if( disk_end > 0 )
    {
        disk = sp( 0, disk_end );
        disk_end++;

        //
        // if there is a replacement string use it otherwise
        // leave the device name as it is
        //
        EmacsString new_value = get_config_env( disk );
        TraceFile( FormatString("EmacsFile::parse_analyse_filespec disk '%s' -> '%s'")
                    << disk << new_value );
        if( new_value.isNull() )
        {
            disk.append( ":" );
        }
        else
        {
            // we are replacing the disk so zap any left over disk
            disk = EmacsString::null;

            if( new_value[new_value.length()-1] != PATH_CH )
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
                goto device_loop;
        }
    }
    else
    {
        // default to no device found
        disk_end = 0;
        // treat server paths as devices
        if( sp.commonPrefix( "\\\\" ) == 2 )
        {
            int server_end = sp.index( PATH_CH, 2 );
            if( server_end > 0 )
            {
                disk = sp( 0, server_end );
                disk_end = server_end;
            }
        }
    }

    int path_end = sp.last( PATH_CH );
    if( path_end < 0 )
    {
        path_end = disk_end;
    }
    else
    {
        path_end++;
    }

    if( disk_end <= path_end )
    {
        path = sp( disk_end, path_end );    // extract the path
    }
    else
    {
        path = EmacsString::null;           // syntax error null the path
    }

    int filename_end = sp.last( '.', path_end );
    if( filename_end < 0 )
    {
        filename_end = sp.length();
    }

    filename = sp( path_end, filename_end );
    filetype = sp( filename_end, INT_MAX );

    return 1;
}

static EmacsString get_current_directory()
{
    // find out the size of the cwd
    DWORD w_len = GetCurrentDirectory( 0, NULL );
    wchar_t *w_buf = malloc_utf16( w_len+1 );
    w_len = GetCurrentDirectory( w_len, w_buf );

    EmacsString cwd( w_buf, w_len );
    emacs_free( w_buf );

    if( cwd[-1] != PATH_CH )
    {
        cwd.append( PATH_CH );
    }

    return cwd;
}

bool EmacsFile::parse_filename( const EmacsString &name, const EmacsString &def )
{
    TraceFile( FormatString("EmacsFile[%d]::parse_filename( '%s', '%s' )")
        << objectNumber() << name << def );

    parse_valid = false;

    if( !parse_analyse_filespec( name ) )
    {
        return false;
    }

    EmacsFile def_file;
    if( !def_file.parse_analyse_filespec( def ) )
    {
        return false;
    }

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 0;
    filename_maxlen = 8;
    filetype_maxlen = 4;

    if( disk.isNull() )
    {
        if( !def_file.disk.isNull() )
        {
            disk = def_file.disk;
        }
        else
        {
            // default to dev: or //server/service or cur dir
            EmacsFile cur_dir_fab( get_current_directory() );

            if( !cur_dir_fab.parse_is_valid() )
            {
                disk = EmacsString::null;
            }
            else
            {
                disk = cur_dir_fab.disk;
            }
        }
    }

    // if its NT then find out what the file system supports
    int file_parsing_override = 0;    // let emacs decide

    if( disk[1] != ':' )
    {
        // force long names for UNC paths
        filename_maxlen = 255;
        filetype_maxlen = 255;
    }
    else
    {
        // get the override parameter - defaults to let emacs decide
        char disk_name[2];
        disk_name[0] = (char)unicode_to_upper( disk[0] );
        disk_name[1] = '\0';
        file_parsing_override = get_file_parsing_override( disk_name, 0 );

        switch( file_parsing_override )
        {
        case 2:    // force 8.3 short names
            filename_maxlen = 8;
            filetype_maxlen = 4;
            break;

        default:
        case 1:    // force long names
            filename_maxlen = 255;
            filetype_maxlen = 255;
            break;

        case 0:    // let Emacs decide
        {
            //
            //    On new windows we will trust GetVolumeInformation to tell the truth
            //    about file component lengths
            //
            DWORD serial_number;
            DWORD max_comp_len;
            wchar_t fs_name[32];
            DWORD fs_flags;

            EmacsString root( FormatString("%s" PATH_STR) << disk );

            if( GetVolumeInformation
                (
                root.utf16_data(),
                NULL, 0,
                &serial_number,
                &max_comp_len,
                &fs_flags,
                fs_name, sizeof( fs_name )/sizeof(wchar_t)
                ) )
            {
                filename_maxlen = (int)max_comp_len;
                filetype_maxlen = (int)max_comp_len;
            }
            else
            {
                filename_maxlen = 255;
                filetype_maxlen = 255;
            }
            }
            break;
        }
    }

    if( path.isNull() )
    {
        if( !def_file.path.isNull() )
            path = def_file.path;
        else
        {
            EmacsFile cur_dir_fab( get_current_directory() );

            // only use the current dir if its on the same disk
            if( cur_dir_fab.parse_is_valid() && disk == cur_dir_fab.disk )
            {
                path = cur_dir_fab.path;
            }
            else
            {
                path = EmacsString("/");
            }
        }
    }

    //
    //    See if the filename is infact a directory.
    //    If it is a directory move the filename on to
    //    the end of the path and null filename.
    //
    if( !filename.isNull() )
    {
        EmacsString fullspec( FormatString("%.2s%s%.*s%.*s") <<
            disk << path <<
            filename_maxlen << filename <<
            filetype_maxlen << filetype );

        // get attributes
        if( fio_is_directory( fullspec ) )
        {
            // need to merge the filename on to the path
            path = FormatString("%s%.*s%.*s") <<
                path <<
                filename_maxlen << filename <<
                filetype_maxlen << filetype;

            filename = EmacsString::null;
            filetype = EmacsString::null;
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

    EmacsString fn_buf( FormatString("%s%s%.*s%.*s") <<
        disk <<
        path <<
        filename_maxlen << filename <<
        filetype_maxlen << filetype );

    char full_path_str[MAXPATHLEN+1 ];
    if( _fullpath( full_path_str, fn_buf, MAXPATHLEN+1 ) != NULL )
    {
        EmacsString full_path( full_path_str );
        if( full_path[-1] != PATH_CH
        && fio_is_directory( full_path ) )
        {
            full_path.append( PATH_STR );
        }
        else
        {
            if( fn_buf[-1] == '.' && full_path[-1] != '.' )
            {
                full_path.append( '.' );
            }
        }

        result_spec = convertShortPathToLongPath( full_path );
    }
    else
    {
        TraceFile( FormatString("EmacsFile[%d]::parse_filename false fn_buf '%s'")
            << objectNumber() << fn_buf );

        return false;
    }

    TraceFile( FormatString("EmacsFile[%d]::parse_filename true result_spec '%s'")
        << objectNumber() << result_spec );

    parse_valid = true;
    return true;
}

//
// walk the elements of the path converting potential short names into long names
//
static EmacsString convertShortPathToLongPath( const EmacsString &short_path )
{
    int start = 0;
    int end = short_path.first( PATH_CH );

    EmacsString long_path( short_path( start, end ) );

    while( end < short_path.length() )
    {
        start = end+1;

        end = short_path.index( PATH_CH, start );
        if( end < 0 )
            end = short_path.length();

        EmacsString part = short_path( start, end );

        if( part.first( '*' ) >= 0
        ||  part.first( '?' ) >= 0 )
        {
            // Do not expand wild cards - this is a name expander only
            long_path.append( PATH_CH );
            long_path.append( part);
        }
        else
        {
            EmacsString name_to_lookup( long_path );
            name_to_lookup.append( PATH_CH );
            name_to_lookup.append( part );

            WIN32_FIND_DATA find_data;
            HANDLE hFind = FindFirstFile( name_to_lookup.utf16_data(), &find_data );
            if( hFind == INVALID_HANDLE_VALUE )
            {
                // file was not found - use the part as is
                long_path.append( PATH_CH );
                long_path.append( part );
            }
            else
            {
                FindClose( hFind );
                EmacsString file_name( find_data.cFileName, wcslen(find_data.cFileName) );

                // use the long name as it appears on disk
                long_path.append( PATH_CH );
                long_path.append( file_name );
            }
        }
    }

    return long_path;
}


FileFindLocal::FileFindLocal( EmacsFile &files, EmacsFileLocal &local, bool return_all_directories )
: FileFindImplementation( files, return_all_directories )
, m_local( local )
, m_handle( INVALID_HANDLE_VALUE )
, m_case_sensitive( false )
{
    wchar_t file_name_buffer[ MAX_PATH ];
    DWORD len = GetFullPathName( files.result_spec.utf16_data(), MAX_PATH, file_name_buffer, NULL );
    if( len == 0 )
        return;

    // now its possible to get the first file
    m_state = first_time;

    m_root_path = EmacsString( file_name_buffer, len );
    int last_path_ch = m_root_path.last( PATH_CH );
    if( last_path_ch >= 0 )
    {
        m_match_pattern = m_root_path( last_path_ch+1, INT_MAX );
        m_root_path.remove( last_path_ch+1 );
    }
    if( !m_case_sensitive )
    {
        m_match_pattern.toLower();
    }
}

FileFindLocal::~FileFindLocal()
{
    if( m_handle )
        FindClose( m_handle );
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
    for(;;)
    {
        switch( m_state )
        {
        default:
        case all_done:
            return EmacsString::null;

        case first_time:
        {
            EmacsString files( m_root_path );
            files.append( "*" );
            m_handle = FindFirstFile( files.utf16_data(), &m_find );
            if( m_handle == INVALID_HANDLE_VALUE )
            {
                m_state = all_done;
                return EmacsString::null;
            }

            m_state = next_time;
        }
            break;

        case next_time:
            if( !FindNextFile( m_handle, &m_find ) )
            {
                m_state = all_done;
                return EmacsString::null;
            }
            break;
        }

        if( wcscmp( m_find.cFileName, L"." ) == 0
        || wcscmp( m_find.cFileName, L".." ) == 0 )
        {
            continue;
        }
        // return all directories if requested to do so
        if( m_return_all_directories
        &&  m_find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
        {
            break;
        }
        // if the name does match the pattern
        EmacsString file_name( m_find.cFileName, wcslen( m_find.cFileName ) );
        if( !m_case_sensitive )
        {
            file_name.toLower();
        }
        if( match_wild( file_name, m_match_pattern ) )
        {
            break;
        }
    }

    // return success and the full path
    m_full_filename = m_root_path;
    EmacsString file_name( m_find.cFileName, wcslen( m_find.cFileName ) );
    m_full_filename.append( file_name );

    if( m_find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY
    && m_full_filename[-1] != PATH_CH )
    {
        m_full_filename.append( PATH_STR );
    }

    return m_full_filename;
}
