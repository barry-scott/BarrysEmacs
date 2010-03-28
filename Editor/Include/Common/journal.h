//
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
const int JNL_VERSION( 3 );

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


const int JNL_NUM_CHARS_PER_RECORD( sizeof(int)*4/sizeof( EmacsChar_t ) );

struct jnl_data
{
    // just larger then the other jnl record types
    // - is this important?
    EmacsChar_t jnl_chars[ JNL_NUM_CHARS_PER_RECORD ];
};

const int JNL_BYTE_SIZE( sizeof( struct jnl_data ) );
inline int jnlCharsToRecords( int num_chars )
{
    return (num_chars + JNL_NUM_CHARS_PER_RECORD - 1)/JNL_NUM_CHARS_PER_RECORD;
}

inline int jnlRecordsToChars( int num_records )
{
    return (num_records * JNL_NUM_CHARS_PER_RECORD);
}

inline void jnlCharsCopy( EmacsChar_t *out, const EmacsChar_t *in, int len )
{
    memcpy( out, in, len * sizeof( EmacsChar_t ) );
}

union journal_record
{
    struct jnl_open     jnl_open;
    struct jnl_insert   jnl_insert;
    struct jnl_delete   jnl_delete;
    struct jnl_data     jnl_data;
};

const int JNL_BUF_NUM_RECORDS( 2048/JNL_BYTE_SIZE );
const int JNL_MAX_NAME_LENGTH( (JNL_BUF_NUM_RECORDS-1) * JNL_NUM_CHARS_PER_RECORD );

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
        const EmacsChar_t *str      // data to insert
        );
    static void journal_delete
        (
        int dot,                    // Location in buffer
        int len                     // length of delete
        );

    static int recoverJournal( const EmacsString &journal_file );

private:
    static EmacsBufferJournal *_journalStart( void );
    static EmacsString _concoctFilename( EmacsString &in );
    static void _flushJournals( void );
    void _findPreviousRecord( void );

    void insertChars
        (
        int dot,                    // Location in buffer
        int len,                    // Length of insert
        const EmacsChar_t *str      // data to insert
        );
    void deleteChars
        (
        int dot,                    // Location in buffer
        int len                     // length of delete
        );

    void jnlWriteBuffer();

#if DBG_JOURNAL
    int _validateJournalBuffer();
#endif

private:
    unsigned m_jnl_active : 1;
    unsigned m_jnl_open : 1;
    unsigned m_jnl_flush : 1;
    FILE *m_jnl_file;
    EmacsString m_jnl_jname;
    int m_jnl_used;           // records used in the current journal buf
    int m_jnl_record;         // last record written in the current journal buffer
    union journal_record m_jnl_buf[ JNL_BUF_NUM_RECORDS ];
};
