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

class EmacsFile
{
public:
    EmacsFile( FIO_EOL_Attribute _attr = FIO_EOL__None );
    virtual ~EmacsFile();

    bool fio_create( const EmacsString &, int, FIO_CreateMode, const EmacsString &, FIO_EOL_Attribute );
    bool fio_open( const EmacsString &, int, const EmacsString &, FIO_EOL_Attribute attr=FIO_EOL__None );
    bool fio_open_using_path( const EmacsString &path, const EmacsString &fn, int append, const EmacsString &ex, FIO_EOL_Attribute attr=FIO_EOL__None );
    bool fio_open( FILE *existing_file, FIO_EOL_Attribute attr )
    {
        m_file = existing_file;
        m_attr = attr;
        return true;
    }
    bool fio_is_open()
    {
        return m_file != NULL;
    }

    // Old 8bit chars
    int fio_get( unsigned char *, int );
    int fio_get_line( unsigned char *buf, int len );
    int fio_get_with_prompt( unsigned char *buffer, int size, const unsigned char *prompt );

    int fio_put( const unsigned char *, int );

#ifdef vms
    int fio_split_put( const unsigned char *, int, const unsigned char *, int );
#endif

    // Unicode chars
    int fio_get( EmacsChar_t *, int );
    //int fio_get_line( EmacsChar_t *buf, int len );
    //int fio_get_with_prompt( EmacsChar_t *buffer, int size, const EmacsChar_t *prompt );

    int fio_put( const EmacsChar_t *, int );

    bool fio_close();

    long int fio_size();
    time_t fio_modify_date();
    int fio_access_mode();
    const EmacsString &fio_getname();
    FIO_EOL_Attribute fio_get_eol_attribute() { return m_attr; }

    //
    //    Manipulate files by name
    //
    static int fio_access( const EmacsString & );
    static bool fio_file_exists( const EmacsString & );
    static int fio_delete( const EmacsString & );
    static time_t fio_file_modify_date( const EmacsString &file );

private:
    int get_fixup_buffer( unsigned char *buf, int len );
    int get_fixup_buffer( EmacsChar_t *buf, int len );


    EmacsString m_full_file_name;
    FILE *m_file;
    FIO_EOL_Attribute m_attr;

    int m_convert_size;
    unsigned char m_convert_buffer[64];
};
