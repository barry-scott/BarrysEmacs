//    Copyright (c) 1982, 1983, 1984, 1985
//        Barry A. Scott and Nick Emery
//
//    Structures used by fileio.c
//

class FileAutoMode : public EmacsObject
{                       // information for automatic mode recognition
public:
    EMACS_OBJECT_FUNCTIONS( FileAutoMode )
    EmacsString a_pattern;        // the pattern that the name must match
    BoundName *a_what;        // what to do if we find it
    FileAutoMode *a_next;        // the next thing to try
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

class EmacsFileImplementation
{
public:
    EmacsFileImplementation( FileParse &fab );
    virtual ~EmacsFileImplementation();

    virtual bool fio_create( const EmacsString &filename, FIO_CreateMode, const EmacsString &def, FIO_EOL_Attribute ) = 0;
    virtual bool fio_open( const EmacsString &filename, bool eof, const EmacsString &def, FIO_EOL_Attribute attr=FIO_EOL__None ) = 0;
    virtual bool fio_open_using_path( const EmacsString &path, const EmacsString &fn, int append, const EmacsString &ex, FIO_EOL_Attribute attr=FIO_EOL__None ) = 0;
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr ) = 0;
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
    virtual int
        fio_access_mode() = 0;
    virtual const EmacsString &
        fio_getname() = 0;
    virtual FIO_EOL_Attribute
        fio_get_eol_attribute() = 0;
    virtual FIO_Encoding_Attribute
        fio_get_encoding_attribute() = 0;

    virtual int
        fio_access() = 0;
    virtual bool
        fio_file_exists() = 0;
    virtual int
        fio_delete() = 0;
    virtual time_t
        fio_file_modify_date() = 0;

protected:
    FileParse &m_fab;
};

class EmacsFileLocal: public EmacsFileImplementation
{
public:
    EmacsFileLocal( FileParse &fab, FIO_EOL_Attribute _attr=FIO_EOL__None );
    virtual ~EmacsFileLocal();

    virtual bool fio_create( const EmacsString &filename, FIO_CreateMode, const EmacsString &def, FIO_EOL_Attribute );
    virtual bool fio_open( const EmacsString &filename, bool eof, const EmacsString &def, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_open_using_path( const EmacsString &path, const EmacsString &fn, int append, const EmacsString &ex, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        m_file = existing_file;
        m_eol_attr = attr;
        return true;
    }
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
    virtual int
        fio_access_mode();
    virtual const EmacsString &
        fio_getname();
    virtual FIO_EOL_Attribute
        fio_get_eol_attribute() { return m_eol_attr; }
    virtual FIO_Encoding_Attribute
        fio_get_encoding_attribute() { return m_encoding_attr; }

    virtual int
        fio_access();
    virtual bool
        fio_file_exists();
    virtual int
        fio_delete();
    virtual time_t
        fio_file_modify_date();

private:
    int get_fixup_buffer( unsigned char *buf, int len );

    FILE *m_file;
    FIO_EOL_Attribute m_eol_attr;
    FIO_Encoding_Attribute m_encoding_attr;

    int m_convert_size;
    enum { CONVERT_BUFFER_SIZE = 1024 * 1024 };
    unsigned char *m_convert_buffer;
};

class EmacsFileRemote: public EmacsFileImplementation
{
    EmacsFileRemote( FileParse &fab, FIO_EOL_Attribute _attr=FIO_EOL__None );
    virtual ~EmacsFileRemote();

    virtual bool fio_create( const EmacsString &filename, FIO_CreateMode, const EmacsString &def, FIO_EOL_Attribute );
    virtual bool fio_open( const EmacsString &filename, bool eof, const EmacsString &def, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_open_using_path( const EmacsString &path, const EmacsString &fn, int append, const EmacsString &ex, FIO_EOL_Attribute attr=FIO_EOL__None );
    virtual bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr );
    virtual bool fio_is_open();

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

    virtual long int fio_size();
    virtual time_t fio_modify_date();
    virtual int fio_access_mode();
    virtual const EmacsString &fio_getname();
    virtual FIO_EOL_Attribute fio_get_eol_attribute();
    virtual FIO_Encoding_Attribute fio_get_encoding_attribute();

    virtual int fio_access();
    virtual bool fio_file_exists();
    virtual int fio_delete();
    virtual time_t fio_file_modify_date();

private:
};

class EmacsFile
{
public:
    EmacsFile( const EmacsString &filename, const EmacsString &def, FIO_EOL_Attribute _attr=FIO_EOL__None );
    EmacsFile( const EmacsString &filename, FIO_EOL_Attribute _attr=FIO_EOL__None );
    EmacsFile( FIO_EOL_Attribute _attr=FIO_EOL__None );
    virtual ~EmacsFile();

    bool fio_create( const EmacsString &name, FIO_CreateMode mode, const EmacsString &defname, FIO_EOL_Attribute attr )
    {
        return impl->fio_create( name, mode, defname, attr );
    }
    bool fio_open( const EmacsString &name, bool eof, const EmacsString &defname, FIO_EOL_Attribute attr=FIO_EOL__None )
    {
        return impl->fio_open( name, eof, defname, attr );
    }
    bool fio_open_using_path( const EmacsString &path, const EmacsString &fn, bool append, const EmacsString &ex, FIO_EOL_Attribute attr=FIO_EOL__None )
    {
        return impl->fio_open_using_path( path, fn, append, ex, attr );
    }
    bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        return impl->fio_open( existing_file, attr );
    }
    bool fio_is_open()
    {
        return impl->fio_is_open();
    }

    // Old 8bit chars
    int fio_get( unsigned char *buffer, int size )
    {
        return impl->fio_get( buffer, size );
    }
    int fio_get_line( unsigned char *buffer, int len )
    {
        return impl->fio_get_line( buffer, len );
    }
    int fio_get_with_prompt( unsigned char *buffer, int size, const unsigned char *prompt )
    {
        return impl->fio_get_with_prompt( buffer, size, prompt );
    }

    int fio_put( const unsigned char *buffer, int size )
    {
        return impl->fio_put( buffer, size );
    }

    // Unicode chars
    int fio_get( EmacsChar_t *buffer, int len )
    {
        return impl->fio_get( buffer, len );
    }
    //int fio_get_line( EmacsChar_t *buf, int len );
    //int fio_get_with_prompt( EmacsChar_t *buffer, int size, const EmacsChar_t *prompt );

    int fio_put( const EmacsChar_t *buffer, int len )
    {
        return impl->fio_put( buffer, len );
    }

    int fio_put( const EmacsString &str )
    {
        return impl->fio_put( str );
    }

    bool fio_close()
    {
        return impl->fio_close();
    }
    void fio_flush()
    {
        impl->fio_flush();
    }

    long int fio_size()
    {
        return impl->fio_size();
    }
    time_t fio_modify_date()
    {
        return impl->fio_modify_date();
    }
    int fio_access_mode()
    {
        return impl->fio_access_mode();
    }
    const EmacsString &fio_getname()
    {
        return impl->fio_getname();
    }
    FIO_EOL_Attribute fio_get_eol_attribute()
    {
        return impl->fio_get_eol_attribute();
    }
    FIO_Encoding_Attribute fio_get_encoding_attribute()
    {
        return impl->fio_get_encoding_attribute();
    }

    //
    //    Manipulate files by name
    //
    int fio_access()
    {
        return impl->fio_access();
    }
    bool fio_file_exists()
    {
        return impl->fio_file_exists();
    }
    int fio_delete()
    {
        return impl->fio_delete();
    }
    time_t fio_file_modify_date()
    {
        return impl->fio_file_modify_date();
    }

public:
    FileParse fab;

private:
    EmacsFileImplementation *impl;
};
