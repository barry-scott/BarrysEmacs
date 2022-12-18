//    Copyright (c) 1982, 1983, 1984, 1985
//        Barry A. Scott and Nick Emery
//
//    Structures used by fileio.c
//

#if DBG_FILE
# define TraceFile( msg ) do { if( dbg_flags&DBG_FILE ) { _dbg_msg( msg ); } } while(0);
#else
# define TraceFile( msg ) // no nothing
#endif

// TraceFileTmp for expand_tilda_path
#if DBG_TMP && 0
# define TraceFileTmp( msg ) do { if( dbg_flags&DBG_TMP ) { TraceFile( msg ); } } while(0)
#else
# define TraceFileTmp( msg ) // do nothing
#endif


class FileAutoMode : public EmacsObject
{                               // information for automatic mode recognition
public:
    EMACS_OBJECT_FUNCTIONS( FileAutoMode )
    EmacsString a_pattern;      // the pattern that the name must match
    BoundName *a_what;          // what to do if we find it
    FileAutoMode *a_next;       // the next thing to try
};

// create modes
enum FIO_CreateMode
{
    FIO_STD,
    FIO_CKP
};

// Open modes
enum FIO_OpenMode
{
    FIO_READ,
    FIO_WRITE
};

enum FIO_EOL_Attribute
{
    FIO_EOL__None,                      // not known
    FIO_EOL__Binary,                    // literal read and write no
    FIO_EOL__StreamCRLF,                // MS-DOS/Windows lines
    FIO_EOL__StreamCR,                  // Machintosh lines
    FIO_EOL__StreamLF                   // Unix lines
};

enum FIO_Encoding_Attribute
{
    FIO_Encoding_None,                  // Unknown
    FIO_Encoding_UTF_8,                 // UTF-8
    FIO_Encoding_UTF_16_BE,             // UTF-16 big endian
    FIO_Encoding_UTF_16_LE              // UTF-16 little endian
};

class EmacsFile;
class FileFindImplementation;

class EmacsFileImplementation : public EmacsObject
{
public:
    friend class EmacsFile;

    EMACS_OBJECT_FUNCTIONS( EmacsFile )
    EmacsFileImplementation( EmacsFile &parent, FIO_EOL_Attribute attr );
    virtual ~EmacsFileImplementation();

    virtual EmacsString repr() = 0;

    virtual bool fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr ) = 0;
    virtual bool fio_open( bool eof=false, FIO_EOL_Attribute attr=FIO_EOL__None ) = 0;
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr ) = 0;
    virtual bool fio_find_using_path( const EmacsString &path, const EmacsString &fn, const EmacsString &ex ) = 0;
    virtual bool fio_is_open() = 0;

    // Old 8bit chars
    virtual int fio_get( unsigned char *, int ) = 0;
    virtual int fio_get_line( unsigned char *buf, int len ) = 0;
    virtual int fio_get_with_prompt( unsigned char *buffer, int size, const unsigned char *prompt ) = 0;

    virtual int fio_put( const unsigned char *, int ) = 0;

    // Unicode chars
    virtual int fio_get( EmacsChar_t *, int ) = 0;
    //int fio_get_line( EmacsChar_t *buf, int len ) = 0;
    //int fio_get_with_prompt( EmacsChar_t *buffer, int size, const EmacsChar_t *prompt ) = 0;

    virtual int fio_put( const EmacsChar_t *, int ) = 0;
    virtual int fio_put( const EmacsString & ) = 0;

    virtual bool fio_close() = 0;
    virtual void fio_flush() = 0;

    virtual long int
        fio_size() = 0;
    virtual time_t
        fio_modify_date() = 0;
    virtual EmacsString
        fio_getname() = 0;
    virtual FIO_EOL_Attribute
        fio_get_eol_attribute() { return m_eol_attr; }
    virtual FIO_Encoding_Attribute
        fio_get_encoding_attribute() { return m_encoding_attr; }
    virtual int
        fio_access() = 0;
    virtual bool
        fio_file_exists() = 0;
    virtual int
        fio_delete() = 0;
    virtual time_t
        fio_file_modify_date() = 0;
    virtual bool
        fio_is_directory() = 0;
    virtual bool
        fio_is_regular() = 0;

    // return a specific implementation of EmacsFileImplementation
    static EmacsFileImplementation *factoryEmacsFileLocal( EmacsFile &file, FIO_EOL_Attribute attr );
#if defined( SFTP )
    static EmacsFileImplementation *factoryEmacsFileRemote( EmacsFile &file, FIO_EOL_Attribute attr );
#endif
    virtual FileFindImplementation *
        factoryFileFindImplementation( bool return_all_directories ) = 0;

    // helpers
    virtual bool
        fio_is_directory( const EmacsString &filename ) = 0;
    virtual EmacsString
        fio_cwd() = 0;
    virtual EmacsString
        fio_home_dir() = 0;

protected:
    EmacsFile &m_parent;
    FIO_EOL_Attribute m_eol_attr;
    FIO_Encoding_Attribute m_encoding_attr;

protected:
    int get_fixup_buffer( unsigned char *buf, int len );

    int m_convert_size;
    enum { CONVERT_BUFFER_SIZE = 1024 * 1024 };
    unsigned char *m_convert_buffer;
};

class EmacsFile : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsFile )

    EmacsFile( const EmacsString &filename, const EmacsString &def, FIO_EOL_Attribute _attr=FIO_EOL__None );
    EmacsFile( const EmacsString &filename, FIO_EOL_Attribute _attr=FIO_EOL__None );
    EmacsFile( FIO_EOL_Attribute _attr=FIO_EOL__None );
    virtual ~EmacsFile();

    EmacsString repr();

    bool parse_is_valid();

    bool fio_create( FIO_CreateMode mode, FIO_EOL_Attribute attr )
    {
        return m_impl->fio_create( mode, attr );
    }
    bool fio_open( bool eof=false, FIO_EOL_Attribute attr=FIO_EOL__None )
    {
        return m_impl->fio_open( eof, attr );
    }
    bool fio_find_using_path( const EmacsString &path, const EmacsString &fn, const EmacsString &ex )
    {
        return m_impl->fio_find_using_path( path, fn, ex );
    }
    bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        return m_impl->fio_open( existing_file, attr );
    }
    bool fio_is_open()
    {
        return m_impl->fio_is_open();
    }

    void fio_set_filespec_from( EmacsFile &other );

    // Old 8bit chars
    int fio_get( unsigned char *buffer, int size )
    {
        return m_impl->fio_get( buffer, size );
    }
    int fio_get_line( unsigned char *buffer, int len )
    {
        return m_impl->fio_get_line( buffer, len );
    }
    int fio_get_with_prompt( unsigned char *buffer, int size, const unsigned char *prompt )
    {
        return m_impl->fio_get_with_prompt( buffer, size, prompt );
    }

    int fio_put( const unsigned char *buffer, int size )
    {
        return m_impl->fio_put( buffer, size );
    }

    // Unicode chars
    int fio_get( EmacsChar_t *buffer, int len )
    {
        return m_impl->fio_get( buffer, len );
    }
    //int fio_get_line( EmacsChar_t *buf, int len );
    //int fio_get_with_prompt( EmacsChar_t *buffer, int size, const EmacsChar_t *prompt );

    int fio_put( const EmacsChar_t *buffer, int len )
    {
        return m_impl->fio_put( buffer, len );
    }
    int fio_put( const EmacsString &str )
    {
        return m_impl->fio_put( str );
    }

    bool fio_close()
    {
        return m_impl->fio_close();
    }
    void fio_flush()
    {
        m_impl->fio_flush();
    }

    long int fio_size()
    {
        return m_impl->fio_size();
    }
    time_t fio_modify_date()
    {
        return m_impl->fio_modify_date();
    }
    EmacsString fio_getname()
    {
        return m_impl->fio_getname();
    }
    FIO_EOL_Attribute fio_get_eol_attribute()
    {
        return m_impl->fio_get_eol_attribute();
    }
    FIO_Encoding_Attribute fio_get_encoding_attribute()
    {
        return m_impl->fio_get_encoding_attribute();
    }

    //
    //    Manipulate files by name
    //
    int fio_access()
    {
        return m_impl->fio_access();
    }
    bool fio_file_exists()
    {
        return m_impl->fio_file_exists();
    }
    int fio_delete()
    {
        return m_impl->fio_delete();
    }
    time_t fio_file_modify_date()
    {
        return m_impl->fio_file_modify_date();
    }
    bool fio_is_directory()
    {
        return m_impl->fio_is_directory();
    }
    bool fio_is_directory( const EmacsString &filename )
    {
        return m_impl->fio_is_directory( filename );
    }
    bool fio_is_regular()
    {
        return m_impl->fio_is_regular();
    }

    FileFindImplementation *factoryFileFindImplementation( bool return_all_directories )
    {
        return m_impl->factoryFileFindImplementation( return_all_directories );
    }

public:
    EmacsString remote_host;    // host:
    EmacsString disk;           // disk:
    EmacsString path;           // /path/
    EmacsString filename;       // name
    EmacsString filetype;       // .type
    EmacsString result_spec;    // full file spec with all fields filled in

    bool wild;                  // true if any field is wild
    int filename_maxlen;        // how long filename can be
    int filetype_maxlen;        // how long filetype can be
    int file_case_sensitive;    // true if case is important

private:
    bool parse_filename( const EmacsString &filename, const EmacsString &def );
    void parse_init();
    bool fio_is_regular( const EmacsString &filename );
    bool parse_analyse_filespec( const EmacsString &filespec );
    void expand_tilda_path( const EmacsString &in_path, EmacsString &out_path );

    bool parse_valid;

private:
    EmacsFileImplementation *m_impl;
};
