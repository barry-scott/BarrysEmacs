//
//  unix_file_remote.cpp
//  Copyright 2022-2023 Barry A. Scott
//
//  Implementation of EmacsFileRemote using libssh
//
#include <emacs.h>
#include <em_stat.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <unistd.h>
#include <fcntl.h>

#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <libssh/sftp.h>

void log_ssh_message( int priority, const char *function, const char *buffer, void *userdata )
{
    TraceFile( FormatString("libssh %d (%s): %s")
                << priority
                << function
                << buffer );
}

static EmacsString sftpError( sftp_session sftp )
{
    int code = sftp_get_error( sftp );

    switch( code )
    {
    case SSH_OK: return "No error";
    case SSH_ERROR: return "Error of some kind";
    case SSH_AGAIN: return "The nonblocking call must be repeated";
    case SSH_EOF: return "We have already a eof";
    case SSH_FX_EOF: return "end-of-file encountered";
    case SSH_FX_NO_SUCH_FILE: return "file does not exist";
    case SSH_FX_PERMISSION_DENIED: return "permission denied";
    case SSH_FX_FAILURE: return "generic failure";
    case SSH_FX_BAD_MESSAGE: return "garbage received from server";
    case SSH_FX_NO_CONNECTION: return "no connection has been set up";
    case SSH_FX_CONNECTION_LOST: return "there was a connection, but we lost it";
    case SSH_FX_OP_UNSUPPORTED: return "operation not supported by libssh yet";
    case SSH_FX_INVALID_HANDLE: return "invalid file handle";
    case SSH_FX_NO_SUCH_PATH: return "no such file or directory path exists";
    case SSH_FX_FILE_ALREADY_EXISTS: return "an attempt to create an already existing file or directory has been made";
    case SSH_FX_WRITE_PROTECT: return "write-protected filesystem";
    case SSH_FX_NO_MEDIA: return "no media was in remote drive";
    default:
        return FormatString("SFTP error code %d") << code;
    }
}


extern "C" {
    static void log_callback_trampoline( int priority, const char *function, const char *buffer, void *userdata )
    {
        // bounce into obj in userdata when we have a target
        TraceFile( FormatString("libssh %d (%s): %s")
                    << priority
                    << function
                    << buffer );
    }
};


class EmacsSshSession;

class EmacsSshSessionImpl
{
public:
    friend class EmacsSshSession;

    EmacsSshSessionImpl( const EmacsString &host, bool verbose=false )
    : ref_count( 1 )
    , m_session( ssh_new() )
    , m_is_connected( false )
    , m_host( host )
    , m_last_error()
    {
        if( m_session )
        {
            int log_level = verbose ? SSH_LOG_WARNING : SSH_LOG_NONE;
            ssh_options_set( m_session, SSH_OPTIONS_LOG_VERBOSITY, &log_level );
            ssh_options_set( m_session, SSH_OPTIONS_HOST, host.sdata() );
            ssh_set_log_callback( log_callback_trampoline );
        }
    }

    virtual ~EmacsSshSessionImpl()
    {
        emacs_assert( ref_count == 0 );

        if( m_is_connected )
        {
            ssh_disconnect( m_session );
        }

        if( m_session )
        {
            ssh_free( m_session );
        }
    }

    bool connect()
    {
        if( !m_session )
        {
            return false;
        }

        // connect to ssh server
        int rc = ssh_connect( m_session );
        if( rc != SSH_OK )
        {
            setLastError( rc );
            return false;
        }

        m_is_connected = true;

        // check that ssh server is known
        ssh_known_hosts_e state = ssh_session_is_known_server( m_session );
        if( state != SSH_KNOWN_HOSTS_OK )
        {
            setLastError( FormatString("server is not known code %d") << state );
            return false;
        }

        // authorize the user
        rc = ssh_userauth_publickey_auto( m_session, NULL, NULL );
        if( rc != SSH_AUTH_SUCCESS )
        {
            setLastError( FormatString("ssh user not authorized code %d") << rc );
            return false;
        }

        return true;
    }

    bool isOk()
    {
        return m_session && m_is_connected;
    }

    const EmacsString &lastError() const
    {
        return m_last_error;
    }

private:
    void setLastError( const EmacsString &msg )
    {
        m_last_error = msg;

        TraceFile( FormatString("EmacsSshSession.setLastError(msg) '%s'")
                    << m_last_error );
    }

    void setLastError( int code )
    {
        setLastError( FormatString("SSH error code %d: %s")
                            << code
                            << ssh_get_error( m_session ) );
    }

    // member vars
    int ref_count;
    ssh_session m_session;
    bool m_is_connected;
    EmacsString m_host;
    EmacsString m_last_error;
};

typedef std::map<EmacsString, EmacsSshSession> ssh_session_map_t;
ssh_session_map_t g_ssh_sessions;

class EmacsSshSession : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsSshSession )

    EmacsSshSession()
    : EmacsObject()
    , m_impl( NULL )
    { }

    EmacsSshSession( EmacsSshSession &other )
    : EmacsObject()
    , m_impl( other.m_impl )
    {
        m_impl->ref_count++;
    }

    EmacsSshSession( const EmacsString &host, bool verbose=false )
    : EmacsObject()
    , m_impl( NULL )
    {
        // see if there is an existing session to that host
        ssh_session_map_t::iterator pos = g_ssh_sessions.find( host );
        if( pos == g_ssh_sessions.end() )
        {
            m_impl = EMACS_NEW EmacsSshSessionImpl( host, verbose );
            g_ssh_sessions[ host ] = *this;
        }
        else
        {
            m_impl = pos->second.m_impl;
            m_impl->ref_count++;
        }
    }

    ~EmacsSshSession()
    {
        if( m_impl )
        {
            m_impl->ref_count--;
            if( m_impl->ref_count == 0 )
            {
                delete m_impl;
            }
        }
    }

    EmacsSshSession &operator=( EmacsSshSession &other )
    {
        if( m_impl )
        {
            if( m_impl == other.m_impl )
            {
                return *this;
            }
            m_impl->ref_count--;
            if( m_impl->ref_count == 0 )
            {
                delete m_impl;
            }
        }

        m_impl = other.m_impl;
        m_impl->ref_count++;

        return *this;
    }

    EmacsSshSessionImpl *operator->()
    {
        return m_impl;
    }

    operator ssh_session()
    {
        return m_impl->m_session;
    }

private:
    EmacsSshSessionImpl *m_impl;
};

//
//  function used from the debugger to list all the saved ssh sessions
//
void list_ssh_sessions(void)
{
    for( ssh_session_map_t::iterator it = g_ssh_sessions.begin(); it != g_ssh_sessions.end(); ++it )
    {
        TraceFile( FormatString("ssh_session '%s'") << it->first );
    }
}

//
//  shutdown all ssh sessions
//
void shutdown_sftp(void)
{
    TraceFile( "shutdown_sftp" );
    while( !g_ssh_sessions.empty() )
    {
        ssh_session_map_t::iterator it = g_ssh_sessions.begin();
        EmacsString remote_host( it->first );
        TraceFile( FormatString("shutdown_sftp shutdown '%s'") << remote_host );
        g_ssh_sessions.erase( it );
    }

    TraceFile( "shutdown_sftp done" );
}

// resource manager for sftp_attributes object
class EmacsSftpAttributes
{
public:
    explicit EmacsSftpAttributes( sftp_attributes attr )
    : m_file_attr( attr )
    { }

    ~EmacsSftpAttributes()
    {
        if( m_file_attr != NULL )
        {
            sftp_attributes_free( m_file_attr );
        }
    }

    bool isOk()
    {
        return m_file_attr != NULL;
    }

public:
    sftp_attributes m_file_attr;
};

//
//  an sftp session allows SFTP commands to be run
//
class EmacsSftpSession : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsSftpSession )

    EmacsSftpSession( EmacsSshSession &session )
    : EmacsObject()
    , m_ssh_session( session )
    , m_sftp_session( NULL )
    { }

    virtual ~EmacsSftpSession()
    {
        if( m_sftp_session != NULL )
        {
            sftp_free( m_sftp_session );
        }
    }

    operator sftp_session()
    {
        return m_sftp_session;
    }

    bool init()
    {
        m_sftp_session = sftp_new( m_ssh_session );
        if( m_sftp_session != NULL )
        {
            int rc = sftp_init( m_sftp_session );
            if( rc != SSH_OK )
            {
                setLastError( rc );
                sftp_free( m_sftp_session );
                m_sftp_session = NULL;
            }
        }
        return m_sftp_session != NULL;
    }

    bool isOk()
    {
        return m_sftp_session != NULL;
    }

    const EmacsString &lastError() const
    {
        return m_last_error;
    }

    // operations
    EmacsString cwd()
    {
        char *path = sftp_canonicalize_path( m_sftp_session, ".");
        if( path == NULL )
        {
            TraceFile( FormatString("EmacsSftpSession[%d]::cwd() sftp_canonicalize_path('.') error %s")
                        << objectNumber() << sftpError( m_sftp_session ) );
            return EmacsString( "." );
        }
        else
        {
            return EmacsString( path );
        }
    }

    EmacsSftpAttributes stat( const EmacsString &path )
    {
        EmacsSftpAttributes attr( sftp_stat( m_sftp_session, path ) );
        return attr;
    }

private:
    void setLastError( const EmacsString &msg )
    {
        TraceFile( FormatString("EmacsSftpSession[%d]::setLastError() '%s'")
                    << objectNumber() << m_last_error );

        m_last_error = msg;
    }

    void setLastError( int code )
    {
        setLastError( FormatString("SSH error code %d: %s")
                            << code
                            << ssh_get_error( m_sftp_session ) );
    }

    // member vars
    EmacsSshSession     m_ssh_session;
    sftp_session        m_sftp_session;
    EmacsString         m_last_error;
};

class EmacsSftpFile : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsSftpFile )

    EmacsSftpFile( EmacsSftpSession &session )
    : EmacsObject()
    , m_sftp_session( session )
    , m_file( NULL )
    , m_last_error()
    { }

    ~EmacsSftpFile()
    {
        if( is_open() )
        {
            close();
        }
    }

    bool is_open() const
    {
        return m_file != NULL;
    }

    bool create( const EmacsString &path )
    {
        m_file = sftp_open( m_sftp_session, path, O_WRONLY|O_TRUNC|O_CREAT, 0660);
        if( m_file == NULL )
        {
            setLastError( sftp_get_error( m_sftp_session ) );
            return false;
        }
        return true;
    }

    bool open( const EmacsString &path )
    {
        m_file = sftp_open( m_sftp_session, path, O_RDONLY, 0 );
        if( m_file == NULL )
        {
            setLastError( sftp_get_error( m_sftp_session ) );
            return false;
        }
        return true;
    }

    EmacsSftpAttributes stat( const EmacsString &path )
    {
        sftp_attributes q = sftp_stat( m_sftp_session, path );
        return EmacsSftpAttributes( q );
    }

    EmacsSftpAttributes fstat()
    {
        return EmacsSftpAttributes( sftp_fstat( m_file ) );
    }

    bool seek( uint64_t offset )
    {
        return 0 == sftp_seek64( m_file, offset);
    }

    ssize_t read( void *buf, size_t count )
    {
        return sftp_read( m_file, buf, count );
    }

    ssize_t write( const void *buf, size_t count )
    {
        return sftp_write( m_file, buf, count );
    }

    bool close()
    {
        int rc = sftp_close( m_file );
        m_file = NULL;
        return rc == SSH_NO_ERROR;
    }

    const EmacsString &lastError() const
    {
        return m_last_error;
    }

private:
    void setLastError( const EmacsString &msg )
    {
        TraceFile( FormatString("EmacsSftpFile.setLastError() '%s'")
                    << m_last_error );

        m_last_error = msg;
    }

    void setLastError( int code )
    {
        setLastError( FormatString("SFTP error code %d")
                    << code );
    }

private:
    EmacsSftpSession &m_sftp_session;
    sftp_file m_file;
    EmacsString m_last_error;
};


class EmacsFileRemote;

class FileFindRemote : public FileFindImplementation
{
public:
    EMACS_OBJECT_FUNCTIONS( FileFindRemote )

    FileFindRemote( EmacsFile &files, EmacsFileRemote &remote, bool return_all_directories );
    virtual ~FileFindRemote();

    EmacsString next();
    EmacsString repr();

private:
    EmacsFileRemote &m_remote;
    sftp_dir m_sftp_dir;
};

class EmacsFileRemote : public EmacsFileImplementation
{
public:
    EmacsFileRemote( EmacsFile &parent, FIO_EOL_Attribute attr );
    virtual ~EmacsFileRemote();

    virtual EmacsString repr();
    virtual bool isOk()
    {
        // not usable if has not connected
        return m_ssh_session->isOk();
    }
    virtual EmacsString lastError();

    virtual bool fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr );
    virtual bool fio_open( bool eof=false, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_find_using_path( const EmacsString &path, const EmacsString &fn, const EmacsString &ex )
    {
        TraceFile( "Unsupported EmacsFileRemote::fio_find_using_path() called" );
        return false;
    }
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        TraceFile( "Unsupported EmacsFileRemote::fio_open( FILE *, FIO_EOL_Attribute) called" );
        return false;
    }
    virtual bool fio_is_open()
    {
        return m_sftp_file.is_open();
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

    virtual bool
        fio_close();
    virtual void
        fio_flush();

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
        factoryFileFindImplementation( bool return_all_directories )
    {
        return EMACS_NEW FileFindRemote( m_parent, *this, return_all_directories );
    }

    // helpers
    virtual bool
        fio_is_directory( const EmacsString &path );
    virtual EmacsString
        fio_cwd();
    virtual EmacsString
        fio_home_dir()
    {
        return m_home_dir;
    }

private:
    friend class FileFindRemote;
    EmacsSshSession     m_ssh_session;
    EmacsSftpSession    m_sftp_session;
    EmacsSftpFile       m_sftp_file;

    EmacsString         m_home_dir;
};

EmacsFileImplementation *EmacsFileImplementation::factoryEmacsFileRemote( EmacsFile &file, FIO_EOL_Attribute attr )
{
    return EMACS_NEW EmacsFileRemote( file, attr );
}

EmacsFileRemote::EmacsFileRemote( EmacsFile &parent, FIO_EOL_Attribute attr )
: EmacsFileImplementation( parent, attr )
, m_ssh_session( parent.remote_host )
, m_sftp_session( m_ssh_session )
, m_sftp_file( m_sftp_session )
, m_home_dir()
{
    TraceFile( FormatString("EmacsFileRemote::EmacsFileRemote( '%s' )")
                << parent.repr() );

    // if not connected
    if( !m_ssh_session->isOk() )
    {
        m_ssh_session->connect();
    }

    // if connected setup sftp session
    if( m_ssh_session->isOk() )
    {
        m_sftp_session.init();
        if( m_sftp_session.isOk() )
        {
            m_home_dir = m_sftp_session.cwd();
            TraceFile( FormatString("EmacsFileRemote::EmacsFileRemote m_home_dir '%s'")
                        << m_home_dir );
        }
    }
}

EmacsFileRemote::~EmacsFileRemote()
{
    delete [] m_convert_buffer;
}

EmacsString EmacsFileRemote::repr()
{
    return FormatString("EmacsFileRemote %p:")
                << this;
}
EmacsString EmacsFileRemote::lastError()
{
    // if the session is not ok that is the error to return
    if( !m_ssh_session->isOk() )
    {
        return m_ssh_session->lastError();
    }

    // if there is a file error return that
    EmacsString last_error( m_sftp_file.lastError() );
    if( !last_error.isNull() )
    {
        return last_error;
    }

    // otherwise return the error from the sftp session
    return m_sftp_session.lastError();
}


//
//    check that the file exists and has read or read and write
//    access allowed
//
int EmacsFileRemote::fio_access()
{
    // assume read and write are allowed
    return 1;
}

bool EmacsFileRemote::fio_file_exists()
{
    // assume not valid

    if( m_parent.parse_is_valid() )
    {
        EmacsSftpAttributes attr( m_sftp_session.stat( m_parent.result_spec ) );
        return attr.isOk();
    }
    else
    {
        return false;
    }
}

int EmacsFileRemote::fio_delete()
{
    int r = sftp_unlink( m_sftp_session, m_parent.result_spec );
    return r == SSH_OK;
}

bool EmacsFileRemote::fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr )
{
    m_eol_attr = attr;

    if( !m_ssh_session->isOk() )
    {
        return false;
    }

    return m_sftp_file.create( m_parent.result_spec );
}

bool EmacsFileRemote::fio_open( bool eof, FIO_EOL_Attribute attr )
{
    if( !m_ssh_session->isOk() )
    {
        return false;
    }

    if( !fio_is_regular() )
    {
        return false;
    }

    m_eol_attr = attr;

    if( !m_sftp_file.open( m_parent.result_spec ) )
    {
        return false;
    }

    if( eof )
    {
        EmacsSftpAttributes attr( m_sftp_file.fstat() );
        if( !attr.isOk() )
        {
            return false;
        }

        // move to the end of the file
        if( !m_sftp_file.seek( attr.m_file_attr->size ) )
        {
            return false;
        }
    }

    return true;
}

int EmacsFileRemote::fio_get( unsigned char *buf, int len )
{
    int status = m_sftp_file.read( buf, len );
    if( status < 0 )
    {
        return -1;
    }

    return get_fixup_buffer( buf, status );
}

int EmacsFileRemote::fio_get_line( unsigned char *buf, int len )
{
    return -1;
}

int EmacsFileRemote::fio_get_with_prompt( unsigned char *buf, int len, const unsigned char * /*prompt*/ )
{
      return -1;
}

int EmacsFileRemote::fio_put( const EmacsString &str )
{
    return fio_put( str.utf8_data(), str.utf8_data_length() );
}

int EmacsFileRemote::fio_put( const unsigned char *buf , int len )
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
                int status = m_sftp_file.write( from, end-from );
                if( status < 0 )
                {
                    return -1;
                }

                written_length += status;
                break;
            }

            int status = m_sftp_file.write( from, to-from );
            if( status < 0 )
            {
                return -1;
            }

            written_length += status;

            status = m_sftp_file.write( "", 1 );
            if( status < 0 )
            {
                return -1;
            }

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
                int status = m_sftp_file.write( from, end-from );
                if( status < 0 )
                {
                    return -1;
                }
                written_length += status;
                break;
            }

            int status = m_sftp_file.write( from, to-from );
            if( status < 0 )
            {
                return -1;
            }
            written_length += status;

            status = m_sftp_file.write( "\r\n", 2 );
            if( status < 0 )
            {
                return -1;
            }
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
        int status = m_sftp_file.write( buf, len );
        if( status < 0 )
        {
            return -1;
        }
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
int EmacsFileRemote::fio_get( EmacsChar_t *buf, int len )
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

int EmacsFileRemote::fio_put( const EmacsChar_t *buf, int len )
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

void EmacsFileRemote::fio_flush()
{
    // not needed
}

bool EmacsFileRemote::fio_close()
{
    return m_sftp_file.close();
}

long int EmacsFileRemote::fio_size()
{
    if( m_sftp_file.is_open() )
    {
        EmacsSftpAttributes attr( m_sftp_file.fstat() );
        if( !attr.isOk() )
        {
            return 0;
        }
        return attr.m_file_attr->size;
    }
    else
    {
        EmacsSftpAttributes attr( m_sftp_file.stat( m_parent.result_spec ) );
        if( !attr.isOk() )
        {
            return 0;
        }
        return attr.m_file_attr->size;
    }
}

EmacsString EmacsFileRemote::fio_getname()
{
    EmacsString name( m_parent.remote_host );
    name.append( ":" );
    name.append( m_parent.result_spec );

    return name;
}

time_t EmacsFileRemote::fio_modify_date()
{
    EmacsSftpAttributes attr( m_sftp_file.fstat() );
    if( !attr.isOk() )
    {
        return 0;
    }

    if( attr.m_file_attr->mtime64 != 0 )
    {
        return attr.m_file_attr->mtime64;
    }
    else
    {
        return attr.m_file_attr->mtime;
    }
}

time_t EmacsFileRemote::fio_file_modify_date()
{
    EmacsSftpAttributes attr( m_sftp_session.stat( m_parent.result_spec ) );
    if( !attr.isOk() )
    {
        return 0;
    }

    if( attr.m_file_attr->mtime64 != 0 )
    {
        return attr.m_file_attr->mtime64;
    }
    else
    {
        return attr.m_file_attr->mtime;
    }
}

EmacsString EmacsFileRemote::fio_cwd()
{
    if( m_sftp_session.isOk() )
    {
        return m_sftp_session.cwd();
    }
    else
    {
        return ".";
    }
}

bool EmacsFileRemote::fio_is_directory( const EmacsString &filename )
{
    EmacsSftpAttributes attr( m_sftp_session.stat( filename ) );
    if( !attr.isOk() )
    {
        return false;
    }

    return S_ISDIR( attr.m_file_attr->permissions );
}

bool EmacsFileRemote::fio_is_directory()
{
    if( !m_parent.parse_is_valid() )
    {
        return false;
    }

    if( m_parent.result_spec[-1] == PATH_CH )
    {
        return true;
    }

    return fio_is_directory( m_parent.result_spec );
}

bool EmacsFileRemote::fio_is_regular()
{
    if( !m_parent.parse_is_valid() )
    {
        return false;
    }

    EmacsSftpAttributes attr( m_sftp_session.stat( m_parent.result_spec ) );
    if( !attr.isOk() )
    {
        return false;
    }

    return S_ISREG( attr.m_file_attr->permissions );
}

FileFindRemote::FileFindRemote( EmacsFile &files, EmacsFileRemote &remote, bool return_all_directories )
: FileFindImplementation( files, return_all_directories )
, m_remote( remote )
, m_sftp_dir( NULL )
{
    TraceFile( FormatString("FileFindRemote[%d]::FileFindRemote( EmacsFile[%d], EmacsFileRemote[%d], %d")
                << objectNumber() << files.objectNumber() << remote.objectNumber() << return_all_directories );
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

FileFindRemote::~FileFindRemote()
{
    TraceFile( FormatString("FileFindRemote[%d]::~FileFindRemote()")
                << objectNumber() );
    if( m_sftp_dir != NULL )
    {
        sftp_closedir( m_sftp_dir );
    }
}

EmacsString FileFindRemote::repr()
{
    return FormatString("FileFindRemote (%p) state %d root %s pattern %s full_filename %s")
            << this
            << m_state
            << m_root_path
            << m_match_pattern
            << m_full_filename;
}

EmacsString FileFindRemote::next()
{
    TraceFile( FormatString("FileFindRemote[%d]::next()")
                << objectNumber() );

    switch( m_state )
    {
    default:
    case all_done:
        TraceFile( FormatString("FileFindRemote[%d]::next() state all_done")
                    << objectNumber() );
        return EmacsString::null;

    case first_time:
        TraceFile( FormatString("FileFindRemote[%d]::next() state first_time")
                    << objectNumber() );
        if( m_match_pattern.isNull() )
        {
            m_state = all_done;
            TraceFile( FormatString("FileFindRemote[%d]::next() => '%s'")
                        << objectNumber() << m_root_path );
            return m_root_path;
        }

        {
        EmacsString path( m_root_path );
        path.remove( path.length() - 1 );

        m_sftp_dir = sftp_opendir( m_remote.m_sftp_session, path );
        }
        if( m_sftp_dir == NULL )
        {
            TraceFile( FormatString("FileFindRemote[%d]::next() sftp_opendir() error '%s'")
                        << objectNumber() << sftpError( m_remote.m_sftp_session ) );
            m_state = all_done;
            return EmacsString::null;
        }

        m_state = next_time;
        // fall into next_time

    case next_time:
    {
        TraceFile( FormatString("FileFindRemote[%d]::next() state next_time")
                    << objectNumber() );
        // read entries looking for a match
        while( !sftp_dir_eof( m_sftp_dir ) )
        {
            EmacsSftpAttributes dir( sftp_readdir( m_remote.m_sftp_session, m_sftp_dir ) );
            if( !dir.isOk() )
            {
                TraceFile( FormatString("FileFindRemote[%d]::next() sftp_readdir() done (error '%s')")
                            << objectNumber() << sftpError( m_remote.m_sftp_session ) );
                break;
            }
            TraceFile( FormatString("FileFindRemote[%d]::next() sftp_readdir() '%s'")
                        << objectNumber() << dir.m_file_attr->name );

            // do not return . and .. entries
            if( strcmp( dir.m_file_attr->name, "." ) == 0
            || strcmp( dir.m_file_attr->name, ".." ) == 0 )
            {
                continue;
            }

            m_full_filename = m_root_path;
            m_full_filename.append( dir.m_file_attr->name );

            bool is_dir = m_remote.fio_is_directory( m_full_filename );

            // always return directories
            if( m_return_all_directories && is_dir )
            {
                m_full_filename.append( PATH_STR );

                EmacsString result( m_files.remote_host );
                result.append( ":" );
                result.append( m_full_filename );

                TraceFile( FormatString("FileFindRemote[%d]::next() => '%s'")
                            << objectNumber() << result );
                return result;
            }

            // the name matches the pattern
            if( match_wild( dir.m_file_attr->name, m_match_pattern ) )
            {
                if( !m_return_all_directories && is_dir )
                {
                    m_full_filename.append( PATH_STR );
                }

                // return success and the full path
                EmacsString result( m_files.remote_host );
                result.append( ":" );
                result.append( m_full_filename );

                TraceFile( FormatString("FileFindRemote[%d]::next() => '%s'")
                            << objectNumber() << result );
                return result;
            }
        }
        m_state = all_done;
    }
        break;
    }

    TraceFile( FormatString("FileFindRemote[%d]::next() done => ''")
                << objectNumber() );
    return EmacsString::null;
}
