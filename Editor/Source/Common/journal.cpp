//
//    Copyright (c) 1987-2009
//    Barry A. Scott and nick Emery
//

//
//    Emacs file modification journalling
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#ifdef vms
#include <fscndef.h>
#include <descrip.h>
#endif
#ifdef __unix__
#include <unistd.h>
#include <fcntl.h>
#endif
#ifdef _NT
#include <io.h>
#endif

SystemExpressionRepresentationJournalFrequency journalling_frequency;
SystemExpressionRepresentationIntBoolean journal_scratch_buffers( 1 );
SystemExpressionRepresentationIntBoolean animate_journal_recovery;
unsigned char journal_fn[MAXPATHLEN+1];
int journal_records_written;

// work structure to queue on a timeout
class Journal_flush_work_item : public EmacsWorkItem
{
public:
    virtual void workAction(void)
    {
        set_activity_character( 'j' );
        EmacsBufferJournal::journal_flush();
        set_activity_character( 'b' );
    }
};
static Journal_flush_work_item journal_flush_request;

class JournalTimer : public EmacsTimer
{
public:
    JournalTimer() {}
    ~JournalTimer() {}

    virtual void timeOut();
    virtual EmacsString description() { return "JournalTimer"; }
};
JournalTimer journal_timer;

void JournalTimer::timeOut()
{
    journal_flush_request.addItem();
}

static void start_journal_timer( void )
{
    if( journalling_frequency == 0 )
        return;

    journal_timer.scheduleTimeOut( journalling_frequency );
}

static void stop_journal_timer( void )
{
    journal_timer.cancelTimeOut();
}


void SystemExpressionRepresentationJournalFrequency::assign_value( ExpressionRepresentation *new_value )
{
    enum{
        MIN_FREQ = 10,
        MAX_FREQ = 210
    };

    int value = new_value->asInt();

    if( value != 0
    && (value < MIN_FREQ || value > MAX_FREQ) )
    {
        error( FormatString("Journal frequency must be 0 or %d to %d seconds") <<
            MIN_FREQ << MAX_FREQ );
        return;
    }
    if( value == 0 )
    {
        EmacsBuffer *b;

        journalling_frequency = 0;
        stop_journal_timer();

        b = buffers;
        while( b != 0 )
        {
            if( b->b_journal != 0 )
            {
                delete b->b_journal;
                b->b_journal = NULL;
            }
            b = b->b_next;
        }
    }
    else
       {
       //
       //    Only check if journalling is being turn on. Changes
       //    of timer interval do not require the modified buffer
       //    checks.
       //
       if( exp_int == 0 )
    {
        EmacsBuffer *b;

        b = buffers;
        while( b != 0 )
        {
            if( b->b_journalling )
            {
                if( b->b_kind == FILEBUFFER && b->b_modified != 0 )
                {
                    error( FormatString("Journalling cannot be enabled on modified file buffer %s") <<
                        b->b_buf_name );
                    return;
                }
                else if( b->unrestrictedSize() != 0 )
                {
                    error( FormatString("Journalling cannot be enabled on non empty buffer %s") <<
                        b->b_buf_name );
                    return;
                }
            }
            b = b->b_next;
        }
    }

        exp_int = value;
        stop_journal_timer();    // Stop if its running
        start_journal_timer();    // Start at new interval
        }
}
void SystemExpressionRepresentationJournalFrequency::fetch_value()
{ }

void SystemExpressionRepresentationBufferJournalled::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );

    if( exp_int )
    {
        if( ! bf_cur->b_journalling
        && bf_cur->b_modified != 0 )
        {
            error( "Journalling cannot be enabled on a modified buffer - write out this buffer");
            return;
        }
        bf_cur->b_journalling = 1u;
    }
    else
    {
        bf_cur->b_journalling = 0;
        delete bf_cur->b_journal;
        bf_cur->b_journal = NULL;
    }

    cant_1line_opt = redo_modes = 1;
}

void SystemExpressionRepresentationBufferJournalled::fetch_value(void)
{
    exp_int = bf_cur->b_journalling;
}


EmacsBufferJournal::EmacsBufferJournal()
    : jnl_active(0)
    , jnl_open(0)
    , jnl_flush(0)
    , jnl_rab_inuse(0)
    , jnl_buf1_current(0)
    , jnl_file(NULL)
    , jnl_jname("")
    , jnl_used(0)        // records used in the current journal buf
    , jnl_record(0)        // last record written in the current journal buffer
{
    jnl_buf = NULL;
    memset( jnl_buf1, sizeof( jnl_buf1 ), 0 );
    memset( jnl_buf2, sizeof( jnl_buf2 ), 0 );
}

EmacsBufferJournal::~EmacsBufferJournal()
{
    if( jnl_file != NULL )
        fclose( jnl_file );
    // and delete the file as its not needed
    if( jnl_jname.length() > 0 )
        remove( jnl_jname );

}

EmacsBufferJournal *EmacsBufferJournal::journal_start( void )
{
    //
    //    First confirm that journalling is allowed on this buffer
    //
    if( journalling_frequency == 0      // globally disabled
    || ! bf_cur->b_journalling          // locally disabled
    //
    // This next test is a result of a the way that visit-file does a replace
    //
    || (bf_cur->b_kind == FILEBUFFER && bf_cur->b_fname.isNull())
    //
    // This next test enforces the scratch buffer journaling restriction
    //
    || (bf_cur->b_kind == SCRATCHBUFFER && !journal_scratch_buffers))
        return NULL;
    if( bf_cur->b_journal != 0 )        // already started
        return bf_cur->b_journal;
    if( bf_cur->b_modified != 0 )
    {
        error( FormatString("Cannot start a journal on modified buffer %s") << bf_cur->b_buf_name );
        return NULL;
    }

    //
    //    Allocate all the blocks required in one go
    //
    EmacsString p;
    union journal_record *open_record = NULL;

    EmacsBufferJournal *jnl = EMACS_NEW EmacsBufferJournal();

    for(;;)
    {
        if( jnl == NULL )
            break;

        jnl->jnl_active = 1;
        jnl->jnl_open = 1;
        jnl->jnl_buf1_current = 1;
        open_record = jnl->jnl_buf = &jnl->jnl_buf1[0];

#ifdef vms
        //
        //    If this is a file buffer use the filename to
        //    to make the journal file name. Otherwise use the
        //    buffer name to make a name from.
        //
    {
        unsigned char def_name_buf[MAXPATHLEN+1];

        if( bf_cur->b_kind == FILEBUFFER )
        {
            struct dsc$descriptor src;
            struct fscn_def items[2];

            memset( items, 0, sizeof( items ) );
            items[0].w_item_code = FSCN$_TYPE;

            DSC_SZ( src, bf_cur->b_fname );

            sys$filescan( &src, items, 0 );

            _str_cpy( def_name_buf, "emacs$journal:" );
            _str_ncat( def_name_buf, items[0].a_addr, min(39-14,items[0].w_length) );
            _str_cat( def_name_buf, "_emacs_journal;" );

            p = bf_cur->b_fname;
            open_record->jnl_open.jnl_type = JNL_FILENAME;
        }
        else
        {
            _str_cpy( def_name_buf, "emacs$journal:.buffer_emacs_journal" );

            p = concoct_filename( bf_cur->b_buf_name );
            open_record->jnl_open.jnl_type = JNL_BUFFERNAME;
        }

        expand_and_default( def_name_buf, p, jnl->jnl_jname );
        *_str_rchr( jnl->jnl_jname, ';' ) = '\0';
    }
#else
    //
    //    Create a unique journal file
    //
    {
        for( int i='a';; i++ )
        {
            FILE *file;

            //
            //    If this is a file buffer use the filename to
            //    to make the journal file name. Otherwise use the
            //    buffer name to make a name from.
            //
            EmacsString def_name;
            if( bf_cur->b_kind == FILEBUFFER )
            {
                def_name = "emacs$journal:.ej_";
                int last_char = def_name.length() - 1;
                def_name[last_char] = (unsigned char)i;

                p = bf_cur->b_fname;
                open_record->jnl_open.jnl_type = JNL_FILENAME;
            }
            else
            {
                def_name ="emacs$journal:.ej_";
                int last_char = def_name.length() - 1;
                def_name[last_char] = (unsigned char)i;

                p = concoct_filename( bf_cur->b_buf_name );
                open_record->jnl_open.jnl_type = JNL_BUFFERNAME;
            }

            expand_and_default( def_name, p, jnl->jnl_jname );

            // see if this file exist
            file = fopen( jnl->jnl_jname, "r" );

            // no then we have the file name we need
            if( file == NULL )
                break;

            // close and loop around
            fclose( file );

            if( i >= 'z' )
            {
                error( FormatString("Unable to create a unique journal filename tried %s last") <<
                    jnl->jnl_jname );
                break;
            }
        }
    }
#endif

        // w - write, b - binary, c - commit
        jnl->jnl_file = fopen( jnl->jnl_jname, "w" BINARY_MODE COMMIT_MODE );
        if( jnl->jnl_file == NULL )
            break;

        //
        //    setup the open record into the journal
        //
        open_record[0].jnl_open.jnl_version = JNL_VERSION;
        open_record[0].jnl_open.jnl_name_length = p.length()+1;
        _str_cpy( open_record[1].jnl_data.jnl_chars, p.data() );

        jnl->jnl_used = 1 + JNL_BYTE_TO_REC(open_record[0].jnl_open.jnl_name_length);
        jnl->jnl_record = 0;
        bf_cur->b_journal = jnl;

        return jnl;

    }

    //
    //    error recovery
    //
    delete jnl;

    error( FormatString("Unable to start journalling for buffer %s code %x") <<
            bf_cur->b_buf_name << errno );
    return NULL;
}

EmacsString EmacsBufferJournal::concoct_filename( EmacsString &in )
{
    const int FILE_NAME_SIZE(31);
    EmacsString out;
    unsigned char ch;
    int limit = min( in.length(), FILE_NAME_SIZE );

    int i; for( i=0; i<limit; i++ )
    {
        ch = in[i];
        if( ('a' <= ch && ch <= 'z')
        || ('A' <= ch && ch <= 'Z')
        || ('0' <= ch && ch <= '9')
        || ch == '$' || ch == '-' || ch == '_' )
            out.append(ch);
        else
            out.append('_');
    }
    out[FILE_NAME_SIZE] = 0;
    return out;
}


void EmacsBufferJournal::journal_pause( void )
{
    stop_journal_timer();
    flush_journals();
}

void EmacsBufferJournal::journal_flush( void )
{
    flush_journals();
    start_journal_timer();
}

void EmacsBufferJournal::flush_journals( void )
{
    EmacsBuffer *b;

    b = buffers;
    while( b != 0 )
    {
        EmacsBufferJournal *jnl;

        if( (jnl = b->b_journal) != NULL )
        {
            jnl->jnl_flush = 1;
            jnl->jnl_write_buffer();
        }
        b = b->b_next;
    }
}

//
//    Call this routine when Emacs exits to get rid of all the
//    out journal files.
//
void EmacsBufferJournal::journal_exit( void )
{
    EmacsBuffer *b;

    stop_journal_timer();

    b = buffers;
    while( b != 0 )
    {
        delete b->b_journal;
        b->b_journal = NULL;
        b = b->b_next;
    }
}

void EmacsBufferJournal::restore_journal( void )
{
    EmacsBuffer *b;

    b = buffers;
    while( b != 0 )
    {
        delete b->b_journal;
        b->b_journal = NULL;
        b = b->b_next;
    }
    stop_journal_timer();    // Stop if its running
    start_journal_timer();    // Start at new interval
}

void EmacsBufferJournal::journal_insert
    (
    int dot,                    // Location in buffer
    int len,                    // Length of insert
    const unsigned char *str    // data to insert
    )
{
    EmacsBufferJournal *jnl = bf_cur->b_journal;

    if( jnl == NULL )
    {
        if( ! journal_start() )
        {
            return;
        }
        else
        {
            jnl = bf_cur->b_journal;
        }
    }

    jnl->insertChars( dot, len, str );
}

void EmacsBufferJournal::insertChars
    (
    int dot,                    // Location in buffer
    int len,                    // Length of insert
    const unsigned char *str    // data to insert
    )
{
    union journal_record *in_rec;
    int free;
    int writing;
    int written;

#if DBG_JOURNAL
    if( dbg_flags&DBG_JOURNAL )
        validate_journal_buffer( jnl );
#endif
    written = 0;

    in_rec = &jnl_buf[jnl_record];
    if( in_rec->jnl_insert.jnl_type == JNL_INSERT
    && in_rec->jnl_insert.jnl_dot + in_rec->jnl_insert.jnl_insert_length == dot
    && (free = JNL_BUF_SIZE - jnl_used) > 0 )
    {
        writing = min( len, free*(int)JNL_BYTE_SIZE );
        memcpy
        (
        &in_rec[1].jnl_data.jnl_chars[in_rec->jnl_insert.jnl_insert_length],
        &str[ written ],
        writing
        );
        in_rec->jnl_insert.jnl_insert_length += writing;
        written = writing;
        jnl_used = jnl_record + 1 +
                JNL_BYTE_TO_REC( in_rec->jnl_insert.jnl_insert_length );
    }

    while( written < len )
    {
        //
        //    See if there is room in the buffer for the
        //    first byte of the insert. We need at least
        //    two slots. One for the insert record and
        //    one for the bytes of data.
        //
        free = JNL_BUF_SIZE - jnl_used - 1;
        if( free < 1 )
        {
            jnl_write_buffer();
            free = JNL_BUF_SIZE - jnl_used - 1;
        }
        in_rec = &jnl_buf[jnl_used];

        writing = min( len - written, free*(int)JNL_BYTE_SIZE );
        in_rec->jnl_insert.jnl_type = JNL_INSERT;
        in_rec->jnl_insert.jnl_dot = dot + written;
        in_rec->jnl_insert.jnl_insert_length = writing;

        memcpy( &in_rec[1].jnl_data.jnl_chars[0], &str[ written ], writing );

        written += writing;
        jnl_record = jnl_used;
        jnl_used += 1 + JNL_BYTE_TO_REC( writing );
    }
#if DBG_JOURNAL
    if( dbg_flags&DBG_JOURNAL )
        validate_journal_buffer( jnl );
#endif
}

void EmacsBufferJournal::find_previous_record()
{
    union journal_record *rec;
    int offset = 0;

    rec = &jnl_buf[ jnl_record ];
    rec->jnl_open.jnl_type = JNL_END;

    while( offset < jnl_record )
    {
        int add_to_offset = 0;

        rec = &jnl_buf[offset];

        switch( rec->jnl_open.jnl_type )
        {
        case JNL_FILENAME:
            add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
            break;

        case JNL_BUFFERNAME:
            add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
            break;

        case JNL_INSERT:
            add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
            break;

        case JNL_DELETE:
            add_to_offset = 1;
            break;

        case JNL_END:
            goto exit_loop;

        default:
            _dbg_msg( FormatString( "Journal find_prev loop unexpected type(0x%x): Type: %x\n") <<
                    offset <<
                    rec->jnl_open.jnl_type );
            debug_invoke();

            // patch the bad record out of the buffer
            rec->jnl_open.jnl_type = JNL_END;
            goto exit_loop;
        }
        if( offset + add_to_offset >= jnl_record )
            break;

        offset += add_to_offset;
    }
exit_loop:
    jnl_record = offset;
    jnl_used = offset;

    switch( rec->jnl_open.jnl_type )
    {
    case JNL_FILENAME:
        jnl_used = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
        break;

    case JNL_BUFFERNAME:
        jnl_used = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
        break;

    case JNL_INSERT:
        jnl_used += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
        break;

    case JNL_DELETE:
        jnl_used += 1;
        break;

    case JNL_END:
        break;

    default:
        _dbg_msg( FormatString( "Journal find prev switch unexpected type(0x%x): Type: %x\n") <<
                offset <<
                rec->jnl_open.jnl_type );
        debug_invoke();
        // patch the bad record out of the buffer
        rec->jnl_open.jnl_type = JNL_END;
        break;
    }

}

void EmacsBufferJournal::journal_delete
    (
    int dot,                // Location in buffer
    int len                    // length of delete
    )
{
    EmacsBufferJournal *jnl = bf_cur->b_journal;

    if( jnl == NULL )
    {
        if( ! journal_start() )
        {
            return;
        }
        else
        {
            jnl = bf_cur->b_journal;
        }
    }

    jnl->deleteChars( dot, len );
}

void EmacsBufferJournal::deleteChars( int dot, int len )
{
    union journal_record *del_rec;

#if DBG_JOURNAL
    if( dbg_flags&DBG_JOURNAL )
        validate_journal_buffer( jnl );
#endif

    //
    //    See if this delete merges with the last record
    //
    del_rec = &jnl_buf[jnl_record];

    switch( del_rec->jnl_open.jnl_type)
    {
    case JNL_DELETE:
        //
        //    See if this is a contiguous delete forward
        //
        if( del_rec->jnl_delete.jnl_del_dot == dot )
        {
            del_rec->jnl_delete.jnl_length += len;
#if DBG_JOURNAL
            if( dbg_flags&DBG_JOURNAL )
                validate_journal_buffer( jnl );
#endif
            return;
        }
        //
        //    See if this is a contiguous delete backward
        //
        if( del_rec->jnl_delete.jnl_del_dot == dot + len )
        {
            del_rec->jnl_delete.jnl_del_dot = dot;
            del_rec->jnl_delete.jnl_length += len;
#if DBG_JOURNAL
            if( dbg_flags&DBG_JOURNAL )
                validate_journal_buffer( jnl );
#endif
            return;
        }
        break;

    case JNL_INSERT:
        //
        //    See if this is a delete from the end of the
        //    last insert
        //
        if( del_rec->jnl_insert.jnl_dot + del_rec->jnl_insert.jnl_insert_length ==
            dot + len )
        {
            // this is delete that is shorter then the insert
            if( del_rec->jnl_insert.jnl_insert_length > len )
            {
                del_rec->jnl_insert.jnl_insert_length -= len;
                jnl_used = jnl_record + 1 +
                    JNL_BYTE_TO_REC( del_rec->jnl_insert.jnl_insert_length );
#if DBG_JOURNAL
                if( dbg_flags&DBG_JOURNAL )
                    validate_journal_buffer( jnl );
#endif
                return;
            }
            // This is an insert that is the same size as the delete.
            // Make this insert record go away and backup in the buffer
            //
            if( del_rec->jnl_insert.jnl_insert_length == len )
            {
                del_rec->jnl_open.jnl_type = JNL_END;
                find_previous_record();
#if DBG_JOURNAL
                if( dbg_flags&DBG_JOURNAL )
                    validate_journal_buffer( jnl );
#endif
                return;
            }
            // This is a delete that is longer then the insert.
            // Remove this insert record and issue a delete for
            // the remaining length
            //
            find_previous_record();
            journal_delete( dot, len - del_rec->jnl_insert.jnl_insert_length );
#if DBG_JOURNAL
            if( dbg_flags&DBG_JOURNAL )
                validate_journal_buffer( jnl );
#endif
            return;
        }
        break;
    default:
        // its not a delete or a insert
        break;
    }

    //
    //    See if there is room in the buffer for a
    //    delete record
    //
    if( JNL_BUF_SIZE - jnl_used < 1 )
        jnl_write_buffer();

    del_rec = &jnl_buf[jnl_used];

    del_rec->jnl_delete.jnl_type = JNL_DELETE;
    del_rec->jnl_delete.jnl_del_dot = dot;
    del_rec->jnl_delete.jnl_length = len;

    jnl_record = jnl_used;
    jnl_used += 1;

#if DBG_JOURNAL
    if( dbg_flags&DBG_JOURNAL )
        validate_journal_buffer( jnl );
#endif
    return;
}

void EmacsBufferJournal::jnl_write_buffer()
{
    union journal_record *buf;
    int status;

    if( jnl_file == NULL )
        return;

    buf = jnl_buf;

    if( jnl_used == 0
    && jnl_flush )
    {
        // flush the buffers
        fflush( jnl_file );
        // update the file info on disk
        close( dup( fileno( jnl_file ) ) );
        return;
    }

    //
    //    Tack on an END record if the buffer is not full
    //
    if( jnl_used != JNL_BUF_SIZE )
        buf[ jnl_used ].jnl_insert.jnl_type = JNL_END;

    //
    //    See if a flush is required after this write
    //
    journal_records_written++;

    //
    //    Write the journal records. The write_ast routine will
    //    check_ the flush flag.
    //
    status = fwrite( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, jnl_file );
    if( status != JNL_BUF_SIZE )
    {
        error( FormatString("error writing journal for %s status code %x") <<
            bf_cur->b_buf_name << errno );
        return;
    }

    //
    //    flush the buffers to the disk
    //
    // flush the buffers
    fflush( jnl_file );
    // update the file info on disk
    close( dup( fileno( jnl_file ) ) );

#if DBG_JOURNAL
    jnl_used = JNL_BUF_SIZE;
    if( !validate_journal_buffer( jnl ) )
    {
        // close the file
        fclose( jnl_file );
        jnl_file = NULL;
        _str_cpy( jnl_jname, "" );
    }
#endif

    //
    //    Switch to the other buffer
    //
    jnl_used = 0;
    jnl_record = 0;
    if( jnl_buf1_current )
    {
        jnl_buf1_current = 0;
        buf = jnl_buf2;
    }
    else
    {
        jnl_buf1_current = 1;
        buf = jnl_buf1;
    }
    jnl_buf = buf;

    //
    //    initialise the buffer to a known good state, empty
    //
    memset( buf, JNL_END, sizeof( jnl_buf1 ) );
}

int recover_journal( void )
{
    EmacsFileTable file_table;
    EmacsString journal_file;
    if( cur_exec == NULL )
        file_table.get_word_interactive( ": journal-recover (file) ", journal_file );
    else
        file_table.get_word_mlisp( journal_file );
    if( !ml_err )
        EmacsBufferJournal::recoverJournal( journal_file );

    return 0;
}

int EmacsBufferJournal::recoverJournal( const EmacsString &journal_file )
{
    union journal_record buf[JNL_BUF_SIZE];
    union journal_record *rec;
    EmacsString journal_filename;

    FILE *file = fopen( journal_file.sdata(), "r" BINARY_MODE );
    if( file == NULL )
    {
        error( FormatString("Cannot open journal file %s") << journal_file );
        return 0;
    }

    //
    //    Read the first record from the journal.
    //    It will be the file or buffer that is
    //    to be recovered.
    //
    int offset = 0;
    int status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );
    if( status == 0 || feof( file ) || ferror( file ) )
    {
        error( "Unable to read the first record from the journal" );
        return 0;
    }

    //
    //    Turn into an asciz string
    //
    rec = &buf[offset];

    if( rec->jnl_open.jnl_version != JNL_VERSION )
    {
        error( FormatString("This Emacs supports journal version %d but this journal file is version %d") <<
            JNL_VERSION << rec->jnl_open.jnl_version );
        return 0;
    }

    switch( rec->jnl_open.jnl_type )
    {
    case JNL_FILENAME:
    {
        journal_filename = rec[1].jnl_data.jnl_chars;
#ifdef vms
    {
        struct dsc$descriptor src;
        unsigned int version1;
        unsigned int version2;
        int i;
        struct fscn_def items[2];

        memset( items, 0, sizeof( items ) );
        items[0].w_item_code = FSCN$_VERSION;

        DSC_SZ( src, journal_filename );

        sys$filescan( &src, items, 0 );
        version1 = 0;
        for( i=1; i<=items[0].w_length - 1; i++ )
            version1 = version1*10 +
                items[0].a_addr[i] - '0';

        visit_file( u_str(";0"), 1, 1, &journal_filename[0] );

        DSC_SZ( src, bf_cur->b_fname );

        sys$filescan( &src, items, 0 );
        version2 = 0;
        for( i=1; i<=items[0].w_length - 1; i++ )
            version2 = version2*10 +
                items[0].a_addr[i] - '0';

        if( version1 != version2 )
        {
            error( "File to be recovered has been superseded by %s",
                bf_cur->b_fname );

            // tidy up and exit
            fclose( file );
            return 0;
        }

    }
#else
        visit_file( journal_filename, 1, 1, EmacsString::null );
#endif
        offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
    }
        break;

    case JNL_BUFFERNAME:
    {
        journal_filename = rec[1].jnl_data.jnl_chars;

        if( EmacsBuffer::find( journal_filename ) != NULL )
        {
            error( FormatString("Buffer already exists %s") << journal_filename );

            // tidy up
            fclose( file );
            return 0;
        }
        EmacsBuffer::set_bfn( EmacsString( journal_filename ) );
        theActiveView->window_on( bf_cur );
        offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
    }
        break;

    default:
    {
        error( FormatString("Format error in journal - first record is type %d") <<
            rec->jnl_open.jnl_type );

        // tidy up
        fclose( file );
        return 0;
    }
    }

    //
    //    Read all the blocks in the journal file
    //
    do
    {
        //
        //    Action all the records in the block
        //
        while( offset < JNL_BUF_SIZE )
        {
            if( animate_journal_recovery != 0 )
                sit_for_inner( 0 );

            rec = &buf[offset];
            switch( rec->jnl_open.jnl_type )
            {
            case JNL_INSERT:
                set_dot( rec->jnl_insert.jnl_dot );
                bf_cur->ins_cstr
                (
                rec[1].jnl_data.jnl_chars,
                rec->jnl_insert.jnl_insert_length
                );
                offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
                break;

            case JNL_DELETE:
                bf_cur->del_frwd
                (
                rec->jnl_delete.jnl_del_dot,
                rec->jnl_delete.jnl_length
                );
                offset += 1;
                break;

            case JNL_END:
                break;
            default:
                error( FormatString("Unexpected type of journal record %d encountered") <<
                    rec->jnl_open.jnl_type );
                break;
            }
            if( rec->jnl_open.jnl_type == JNL_END )
                break;
        }

        offset = 0;
        status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );
    }
    while( !(status != JNL_BUF_SIZE || feof( file ) || ferror( file )) );

    if( !feof( file ) )
        error( "Unable to read a record from the journal" );
    else
        message( FormatString("Recovery of %s completed") << journal_filename );

    //
    //    Tidy up and exit
    //
    fclose( file );

    return 0;
}

#if DBG_JOURNAL
int EmacsBufferJournal::validate_journal_buffer(void)
{
    union journal_record *rec;
    int offset = 0;

    while( offset < jnl_used )
    {
        rec = &jnl_buf[offset];

        switch( rec->jnl_open.jnl_type )
    {
        case JNL_FILENAME:
            offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
            break;

        case JNL_BUFFERNAME:
            offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
            break;

        case JNL_INSERT:
            offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
            break;

        case JNL_DELETE:
            offset += 1;
            break;

        case JNL_END:
            goto exit_loop;

        default:
            _dbg_msg( "Journal validate unexpected type(0x%x): Type: %x\n",
                    offset,
                    rec->jnl_open.jnl_type );
            debug_invoke();
            return 0;
    }
    }
exit_loop:
    return 1;
}
#endif
