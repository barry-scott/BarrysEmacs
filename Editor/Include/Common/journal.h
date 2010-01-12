//    Copyright (c) 1987
//        Barry A. Scott and Nick Emery
//    Copyright (c) 1988-2010
//        Barry A. Scott

//    Definition for the journal subsystem

enum jnl_types
{
    JNL_END,                // end of block
    JNL_FILENAME,           // name of file being journalled
    JNL_BUFFERNAME,         // name of buffer being journalled
    JNL_INSERT,             // insert text
    JNL_DELETE              // delete text
};
const int JNL_VERSION( 2 );

struct jnl_open
{
    enum jnl_types jnl_type;
    int jnl_version;
    int jnl_name_length;
};

struct jnl_insert
{
    enum jnl_types jnl_type;
    int jnl_dot;
    int jnl_insert_length;
};

struct jnl_delete
{
    enum jnl_types jnl_type;
    int jnl_del_dot;
    int jnl_length;
};

struct jnl_data
{
    // just larger then the other jnl record types
    // - is this important?
    EmacsChar_t jnl_chars[ sizeof(int)*4/sizeof( EmacsChar_t ) ];
};

const int JNL_BYTE_SIZE( sizeof( struct jnl_data ) );
inline int JNL_BYTE_TO_REC( int n )
{
    return (n+JNL_BYTE_SIZE-1)/JNL_BYTE_SIZE;
}

union journal_record
{
    struct jnl_open     jnl_open;
    struct jnl_insert   jnl_insert;
    struct jnl_delete   jnl_delete;
    struct jnl_data     jnl_data;
};

const int JNL_BUF_SIZE( 64 );        // this yields a 1k buffer
class EmacsBufferJournal : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsBufferJournal )
    EmacsBufferJournal();
    virtual ~EmacsBufferJournal();

    static void journal_pause( void );
    static void journal_flush( void );
    static void journal_exit( void );
    static void restore_journal( void );

    static void journal_insert
        (
        int dot,                    // Location in buffer
        int len,                    // Length of insert
        const EmacsChar_t *str   // data to insert
        );
    static void journal_delete
        (
        int dot,                    // Location in buffer
        int len                     // length of delete
        );

    static int recoverJournal( const EmacsString &journal_file );

private:
    static EmacsBufferJournal *journal_start( void );
    static EmacsString concoct_filename( EmacsString &in );
    static void flush_journals( void );
    void find_previous_record( void );

    void insertChars
        (
        int dot,                    // Location in buffer
        int len,                    // Length of insert
        const EmacsChar_t *str   // data to insert
        );
    void deleteChars
        (
        int dot,                    // Location in buffer
        int len                     // length of delete
        );

    void jnl_write_buffer();

#if DBG_JOURNAL
    int validate_journal_buffer();
#endif

private:
    unsigned jnl_active : 1;
    unsigned jnl_open : 1;
    unsigned jnl_flush : 1;
    unsigned jnl_rab_inuse : 1;
    unsigned jnl_buf1_current : 1;
    FILE *jnl_file;
    EmacsString jnl_jname;
    int jnl_used;        // records used in the current journal buf
    int jnl_record;        // last record written in the current journal buffer
    union journal_record *jnl_buf;
    union journal_record jnl_buf1[ JNL_BUF_SIZE ];
    union journal_record jnl_buf2[ JNL_BUF_SIZE ];
};
