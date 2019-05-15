//
//     Copyright (c) 2019
//        Barry A, Scott
//

#include <emacsutl.h>
#include <em_stat.h>
#include <stdlib.h>
#include <emobject.h>
#include <emstring.h>
#include <emstrtab.h>
#include <ndbm.h>

# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

extern int expand_and_default( const EmacsString &nm, const EmacsString &def, EmacsString & buf );
extern int match_wild( const EmacsString &, const EmacsString & );
extern int dbg_flags;

database * database::lastdatabase;

static char *makeSqlString( const unsigned char *text, int size )
{
    char *str = (char *)EMACS_MALLOC( size, malloc_type_char );
    memcpy( str, text, size );
    return str;
}

static void freeSqlString( void *p )
{
    EMACS_FREE( p );
}

database::database()
: db_name()
, db_is_open( false )
, db_is_readonly( false )
, db_access_readonly( false )
, db_access_keepopen( false )
, db_access_may_create( false )
, db_filename()
, db_ptr( NULL )
, db_stmt_select_value( NULL )
, db_stmt_select_all_keys( NULL )
, db_stmt_insert_key_value( NULL )
, db_stmt_delete_key( NULL )
{ }

database::~database()
{
    close_db();
}

bool database::open_db( const EmacsString &file, bool readonly, bool may_create )
{
# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("open_db( %s, %d )") << file << readonly );
# endif

    db_name = file;
    expand_and_default( file, ".db", db_filename );

    db_access_readonly = readonly;
    db_access_may_create = may_create;

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: open_db( , %d )") << db_filename << readonly );
    }
# endif

    return reopen_db();
}

void database::close_db()
{
# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: close_db()") << db_filename );
    }
# endif

    if( lastdatabase == this )
    {
        lastdatabase = NULL;
    }

    int rc = sqlite3_finalize( db_stmt_select_value );
    db_stmt_select_value = NULL;
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_finalize( db_stmt_select_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_finalize( db_stmt_select_all_keys );
    db_stmt_select_all_keys = NULL;
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_finalize( db_stmt_select_all_keys ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_finalize( db_stmt_insert_key_value );
    db_stmt_insert_key_value = NULL;
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_finalize( db_stmt_insert_key_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_finalize( db_stmt_delete_key );
    db_stmt_delete_key = NULL;
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_finalize( db_stmt_delete_key ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_close_v2( db_ptr );
    db_ptr = NULL;
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_close_v2() rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    db_is_open = false;
}

bool database::reopen_db()
{
# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: reopen_db() db_is_open %d") << db_filename << db_is_open );
    }
# endif

    // open if it exists
    int rc = sqlite3_open_v2( db_filename, &db_ptr, db_access_readonly ? SQLITE_OPEN_READONLY : SQLITE_OPEN_READWRITE, NULL );
    if( rc == SQLITE_CANTOPEN )
    {
        if( db_access_readonly )
        {
            return false;
        }

        // try opening readonly
        rc = sqlite3_open_v2( db_filename, &db_ptr, SQLITE_OPEN_READONLY, NULL );
        if( rc == SQLITE_CANTOPEN && !db_access_may_create )
        {
            return false;
        }

        // try to create a new database file
        rc = sqlite3_open_v2( db_filename, &db_ptr, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL );
        if( rc == SQLITE_CANTOPEN )
        {
            return false;
        }

        // create the table
        sqlite3_stmt *create_table = NULL;
        rc = sqlite3_prepare_v3( db_ptr, "CREATE TABLE key_value (key TEXT PRIMARY KEY, value TEXT)",
                                -1, 0, &create_table, NULL );
        if( rc != SQLITE_OK )
        {
            _dbg_msg( FormatString("db %s: sqlite3_prepare_v2( CREATE TABLE ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
            return false;
        }
        rc = sqlite3_step( create_table );

        int rc2 = sqlite3_finalize( create_table );
        if( rc != SQLITE_DONE )
        {
            _dbg_msg( FormatString("db %s: sqlite3_step( CREATE TABLE ) rc %d: %s") << db_filename << rc << sqlite3_errstr( rc ) );
            return false;
        }
        if( rc2 != SQLITE_OK )
        {
            _dbg_msg( FormatString("db %s: sqlite3_step( CREATE TABLE ) rc %d: %s") << db_filename << rc2 << sqlite3_errstr( rc2 ) );
            return false;
        }
    }

    db_is_open = true;
    db_is_readonly = db_access_readonly;

    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_open_v2() rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
        return false;
    }

    rc = sqlite3_prepare_v3( db_ptr, "SELECT value FROM key_value WHERE key = ?",
                            -1, SQLITE_PREPARE_PERSISTENT, &db_stmt_select_value, NULL );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_prepare_v2( db_stmt_select_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
        return false;
    }

    rc = sqlite3_prepare_v3( db_ptr, "SELECT key FROM key_value where key LIKE ?",
                            -1, SQLITE_PREPARE_PERSISTENT, &db_stmt_select_all_keys, NULL );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_prepare_v2( db_stmt_select_all_keys ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
        return false;
    }

    rc = sqlite3_prepare_v3( db_ptr, "INSERT INTO key_value( key, value ) VALUES( ?, ? )\n"
                                     "    ON CONFLICT(key) DO UPDATE SET value=excluded.value",
                            -1, SQLITE_PREPARE_PERSISTENT, &db_stmt_insert_key_value, NULL );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_prepare_v2( db_stmt_insert_key_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
        return false;
    }

    rc = sqlite3_prepare_v3( db_ptr, "DELETE FROM key_value WHERE key = ?",
                            -1, SQLITE_PREPARE_PERSISTENT, &db_stmt_delete_key, NULL );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_prepare_v2( db_stmt_delete_key ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
        return false;
    }

    return true;
}

int database::put_db( const EmacsString &key, const unsigned char *content, int contentlen )
{
# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: put_db( %s, ... )") << db_filename << key );
    }
# endif

    char *key_str = makeSqlString( key.utf8_data(), key.utf8_data_length() );
    int rc = sqlite3_bind_text( db_stmt_insert_key_value, 1, key_str, key.utf8_data_length(), freeSqlString );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_bind_text( db_stmt_insert_key_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    char *value_str = makeSqlString( content, contentlen );
    rc = sqlite3_bind_text( db_stmt_insert_key_value, 2, value_str, contentlen, freeSqlString );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_bind_text( db_stmt_insert_key_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_step( db_stmt_insert_key_value );
    if( rc != SQLITE_DONE )
    {
        _dbg_msg( FormatString("db %s: sqlite3_step( db_stmt_insert_key_value ) rc %d: %s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    // reset ready for next use
    rc = sqlite3_reset( db_stmt_insert_key_value );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_reset( db_stmt_insert_key_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    return 0;
}

//
//    return
//        -1 failed
//        0 success
//        1 success user must free content buffer
//
int database::get_db
    (
    const EmacsString &key,
    EmacsString &result_value
    )
{
    bool found = false;
    assert( db_ptr != NULL );

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: get_db( %r ) start") << db_filename << key );
    }
# endif

    char *key_str = makeSqlString( key.utf8_data(), key.utf8_data_length() );
    int rc = sqlite3_bind_text( db_stmt_select_value, 1, key_str, key.utf8_data_length(), freeSqlString );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_bind_text( db_stmt_select_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    rc = sqlite3_step( db_stmt_select_value );
    if( rc == SQLITE_ROW )
    {
        const unsigned char *text = sqlite3_column_text( db_stmt_select_value, 0 );
        int size = sqlite3_column_bytes( db_stmt_select_value, 0 );

        EmacsString result( EmacsString::copy, text, size );
        result_value = result;

        found = true;
    }
    else if( rc != SQLITE_DONE )
    {
        _dbg_msg( FormatString("db %s: sqlite3_step( db_stmt_select_value ) rc %d: %s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    // reset ready for next use
    rc = sqlite3_reset( db_stmt_select_value );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_reset( db_stmt_select_value ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: get_db() done") << db_filename );
    }
# endif

    return found ? 0 : -1;
}

int database::del_db( const EmacsString &key )
{
    assert( db_ptr != NULL );

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: del_db( %r ) start") << db_filename << key );
    }
# endif

    char *key_str = makeSqlString( key.utf8_data(), key.utf8_data_length() );
    int rc = sqlite3_bind_text( db_stmt_delete_key, 1, key_str, key.utf8_data_length(), freeSqlString );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_bind_text( db_stmt_delete_key ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    bool found = false;

    rc = sqlite3_step( db_stmt_delete_key );
    if( rc == SQLITE_ROW )
    {
        found = true;
    }
    else if( rc != SQLITE_DONE )
    {
        _dbg_msg( FormatString("db %s: sqlite3_step( db_stmt_delete_key ) rc %d: %s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    // reset ready for next use
    rc = sqlite3_reset( db_stmt_delete_key );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_reset( db_stmt_delete_key ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: del_db() done") << db_filename );
    }
# endif

    return found ? 0 : -1;
}

int database::index_db
    (
    const EmacsString &key_prefix,
    void (*helper)( const EmacsString & )
    )
{
# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: index_db( %s )") << db_filename << key_prefix );
    }
# endif

    EmacsString like_pattern( key_prefix );
    like_pattern.append( "%" );

    char *like_pattern_str = (char *)EMACS_MALLOC( like_pattern.utf8_data_length(), malloc_type_char );
    memcpy( like_pattern_str, like_pattern.utf8_data(), like_pattern.utf8_data_length() );

    int rc = sqlite3_bind_text( db_stmt_select_all_keys, 1, like_pattern_str, like_pattern.utf8_data_length(), freeSqlString );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_bind_text( db_stmt_select_all_keys ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

    for(;;)
    {
        int rc = sqlite3_step( db_stmt_select_all_keys );
        if( rc == SQLITE_ROW )
        {
            const unsigned char *text = sqlite3_column_text( db_stmt_select_all_keys, 0 );
            int size = sqlite3_column_bytes( db_stmt_select_all_keys, 0 );

            EmacsString key_name( EmacsString::copy, text, size );
            _dbg_msg( FormatString("db %s: index_db key_name %s") << db_filename << key_name );
            helper( key_name );
        }
        else if( rc == SQLITE_DONE )
        {
            break;
        }
        else
        {
            _dbg_msg( FormatString("db %s: sqlite3_step( db_stmt_select_all_keys ) rc %d: %s") << db_filename << rc << sqlite3_errstr( rc ) );
            break;
        }
    }

    // reset ready for next use
    rc = sqlite3_reset( db_stmt_select_all_keys );
    if( rc != SQLITE_OK )
    {
        _dbg_msg( FormatString("db %s: sqlite3_reset( db_stmt_select_all_keys ) rc %d:%s") << db_filename << rc << sqlite3_errstr( rc ) );
    }

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        _dbg_msg( FormatString("db %s: index_db() done") << db_filename );
    }
# endif

    return 0;
}

// a left over from the past - not sure what it used to do
int database::get_db_help
    (
    const EmacsString &,
    EmacsString &
    )
{
    return 0;
}
