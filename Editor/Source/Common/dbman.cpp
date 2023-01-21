//
//     Copyright(c) 1982, 1983, 1984, 1985. 1987
//        Barry A. Scott and Nick Emery
//     Copyright(c) 1988-2010
//

// Simple data base manager routines.

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#define GROW 10             // Size by which to grow search list names

int extend_database_search_list( void );
int fetch_database_entry( void );
int fetch_help_database_entry( void );
int put_database_entry( void );
int list_databases( void );
int remove_database( void );
void init_db( void );
void restore_db( void );

DatabaseSearchListTable DatabaseSearchList::name_table( 8, 8 );

SystemExpressionRepresentationIntPositive get_db_help_flags( 7 );    // All information required

#ifdef DB

unsigned char *no_such_db_str =     u_str("No such database search list \"%s\"");
unsigned char *no_help_db_str =     u_str("help entry \"%s\" not found in \"%s\"");
unsigned char *db_empty_str =       u_str("Empty database search list \"%s\"");
unsigned char *key_not_found_str =  u_str("Entry \"%s\" not found in \"%s\"");

DatabaseSearchList::DatabaseSearchList( EmacsString &name )
: dbs_name( name )
, dbs_size( 0 )
{
    for( int i=0; i<SEARCH_LEN; i++ )
        dbs_elements[i] = NULL;

    name_table.add( name, this );
}

DatabaseSearchList::~DatabaseSearchList()
{
    name_table.remove( dbs_name );
}

int extend_database_search_list( void )
{
    int access_flags = database::access_keepopen;

    EmacsString name;
    getescword( DatabaseSearchList::, ": extend-database-search-list (list) ", name );
    if( name.isNull() )
        return 0;

    DatabaseSearchList *p = DatabaseSearchList::find( name );
    if( p == NULL )
        p = EMACS_NEW DatabaseSearchList( name );

    EmacsString buf( FormatString( ": extend-database-search-list (list) %s (database) " ) << p->dbs_name );
    EmacsFileTable file_table;
    EmacsString db_filename;
    getescword( file_table., buf, db_filename );
    if( db_filename.isNull() )
        return 0;

    EmacsFile db_file( db_filename, EMACS_DB_DEFAULT );
    if( !db_file.parse_is_valid() )
    {
        error( FormatString("Cannot find DB file \"%s\"") << db_filename );
        return 0;
    }

    for( int i = 0; i < p->dbs_size; i++ )
    {
        database *dbx = p->dbs_elements[i];
        if( db_file.result_spec == dbx->db_name )
        {
            return 0;
        }
    }

    if( p->dbs_size == DatabaseSearchList::SEARCH_LEN )
    {
        error( "Too many components in search list" );
        return 0;
    }

    if( arg_state == have_arg )
    {
        access_flags = arg;
    }
    else if( !interactive() && cur_exec->p_nargs > 2 )
    {
        access_flags = getnum( u_str(": extend-database-search-list (flags) ") );
    }

    database *db = EMACS_NEW database;
    if( db == NULL || !db->open_db( db_file, access_flags & database::access_readonly, access_flags & database::access_may_create ) )
    {
        error( FormatString("Cannot find database \"%s\"") << db_file.result_spec );
        return 0;
    }

    db->db_access_keepopen = access_flags & database::access_keepopen;

    int i = p->dbs_size;
    while( i > 0)
    {
        p->dbs_elements[i] = p->dbs_elements[i-1];
        i--;
    }
    p->dbs_elements[0] = db;
    p->dbs_size++;

    if( !db->db_access_keepopen )
    {
        db->close_db();
    }

    return 0;
}

// fetch an entry from a database into the current buffer
int fetch_database_entry( void )
{
    EmacsString dbname;
    if( cur_exec == NULL )
        DatabaseSearchList::get_esc_word_interactive( ": fetch-database-entry (list) ", EmacsString::null, dbname );
    else
        DatabaseSearchList::get_esc_word_mlisp( dbname );

    if( dbname.isNull() )
        return 0;

    DatabaseSearchList *dbs = DatabaseSearchList::find( dbname );
    if( dbs == NULL )
    {
        error( FormatString(no_such_db_str) << dbname );
        return 0;
    }

    DatabaseEntryNameTable table( dbs );
    EmacsString key;
    getescword( table., FormatString(": fetch-database-entry (list) %s (entry) ") << dbs->dbs_name, key );
    if( key.isNull() )
        return 0;

    int i;
    for( i = 0; i < dbs->dbs_size; i++ )
    {
        EmacsString value;
        if( dbs->dbs_elements[i]->get_db( key, value ) >= 0 )
        {
            bf_cur->ins_cstr( value );
            break;
        }
    }

    cant_1line_opt = 1;
    if( i > dbs->dbs_size )
        error( FormatString(key_not_found_str) << key << dbname );

    return 0;
}


// fetch an entry from a help database into the current buffer
int fetch_help_database_entry( void )
{
    EmacsString dbname;
    if( cur_exec == NULL )
        DatabaseSearchList::get_esc_word_interactive( ": fetch-help-database-entry (list) ", EmacsString::null, dbname );
    else
        DatabaseSearchList::get_esc_word_mlisp( dbname );

    if( dbname.isNull() )
        return 0;

    DatabaseSearchList *dbs = DatabaseSearchList::find( dbname );
    if( dbs == 0 )
    {
        error( FormatString(no_such_db_str) << dbname );
        return 0;
    }

    EmacsString keys;
    if( cur_exec == NULL )
        keys = get_string_interactive( FormatString(": fetch-help-database-entry (list) %s (topic) ") << dbs->dbs_name );
    else
        keys = get_string_mlisp();

    if( keys.isNull() )
        return 0;

    int i;
    for( i = 0; i < dbs->dbs_size; i++ )
    {
        EmacsString value;
        if( dbs->dbs_elements[i]->get_db_help( keys, value ) >= 0 )
            break;
    }
    cant_1line_opt = 1;
    if( i >= dbs->dbs_size )
        error( FormatString(no_help_db_str) << keys << dbname );
    return 0;
}

// Put the contents of the current buffer into a database
int put_database_entry( void )
{
    EmacsString dbname;
    if( cur_exec == NULL )
        DatabaseSearchList::get_esc_word_interactive( ": put-database-entry (list) ", dbname );
    else
        DatabaseSearchList::get_esc_word_mlisp( dbname );

    if( dbname.isNull() )
        return 0;

    DatabaseSearchList *dbs = DatabaseSearchList::find( dbname );
    if( dbs == NULL )
    {
        error( FormatString(no_such_db_str) << dbname );
        return 0;
    }
    if( dbs->dbs_size <= 0 )
    {
        error( FormatString(db_empty_str) << dbs->dbs_name );
        return 0;
    }

    database *dbx = dbs->dbs_elements[0];
    if( dbx->db_is_readonly )
    {
        error( FormatString("\"%s\" is a read-only database.") << dbs->dbs_name );
        return 0;
    }

    DatabaseEntryNameTable table( dbs );
    EmacsString key;
    getescword( table., FormatString(": put-database-entry (list) %s (entry) ") << dbs->dbs_name, key );
    if( key.isNull() )
    {
        return 0;
    }

    bf_cur->gap_to( bf_cur->unrestrictedSize() + 1 );

    EmacsString content( EmacsString::copy, bf_cur->ref_char_at(1), bf_cur->unrestrictedSize() );
    if( dbx->put_db( key, content.utf8_data(), content.utf8_data_length() ) < 0 )
    {
        error( "Database put failed");
    }

    return 0;
}



// List the names and contents of all database search lists
int list_databases( void )
{
    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Database list", interactive() );

    for( int j=0; j<DatabaseSearchList::name_table.entries(); j++ )
    {
        DatabaseSearchList *p = DatabaseSearchList::name_table.value( j );

        bf_cur->ins_cstr( p->dbs_name ); bf_cur->ins_cstr( ":\n", 2 );
        for( int i = 0; i < p->dbs_size; i++ )
        {
            database *db = p->dbs_elements[i];

            bf_cur->ins_cstr( "    ", 4);
            bf_cur->ins_cstr( db->db_name );
            if( db->db_is_readonly || db->db_access_keepopen )
                bf_cur->ins_cstr( "    (read only, keep open)" );
            else if( db->db_is_readonly )
                bf_cur->ins_cstr( "    (read only)" );
            else if( db->db_access_keepopen )
                bf_cur->ins_cstr( "    (keep open)" );

            bf_cur->ins_cstr( "\n", 1 );
        }
        bf_cur->ins_cstr( "\n", 1 );
    }

    bf_cur->b_modified = 0;
    bf_cur->b_checkpointed = -1;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}


int remove_database( void )
{
    DatabaseSearchList *p = getword( DatabaseSearchList::, ": remove-database (list) " );
    if( p == NULL )
        return 0;

    if( p->dbs_size <= 0 )
    {
        error( FormatString(db_empty_str) << p->dbs_name );
        return 0;
    }

    EmacsStringTable table;
    for( int i = 0; i < p->dbs_size; i++ )
    {
        database *db = p->dbs_elements[i];
        table.add( db->db_name, db );
    }

    EmacsString name;
    if( cur_exec == NULL )
        table.get_word_interactive
            (
            FormatString(": remove-database (list) %s (database) ") << p->dbs_name,
            name
            );
    else
        table.get_word_mlisp( name );

    database *db = (database *)table.find( name );
    if( db != NULL )
    {
        int i;
        for( i = 0; i < p->dbs_size; i++ )
            if( p->dbs_elements[i] == db )
                break;
        delete db;

        for( ; i < p->dbs_size - 1; i++ )
            p->dbs_elements[i] = p->dbs_elements[i + 1];

        p->dbs_size--;
    }

    return 0;
}

// initialization for when EMACS starts up with a restore environment
void restore_db( void )
{
#if 0
    DatabaseSearchList *p = dbroot;
    while( p != NULL )
    {
        for( int i = 0; i < p->dbs_size; i++ )
        {
            database *dbx = p->dbs_elements[i];
#if defined MLL_DB || defined vms
            dbx->db_dirf = 0;
#else
            dbx->db_dirf = -1;
            dbx->db_pagf = -1;
            dbx->db_datf = -1;
#endif
        }
        p = p->dbs_next;
    }
#endif
}
#else
int extend_database_search_list( void )
{
    return no_value_command();
}

int fetch_database_entry( void )
{
    return no_value_command();
}

int fetch_help_database_entry( void )
{
    return no_value_command();
}

int put_database_entry( void )
{
    return no_value_command();
}

int list_databases( void )
{
    return no_value_command();
}

int remove_database( void )
{
    return no_value_command();
}

void init_db( void )
{
    return;
}

void restore_db( void )
{
    return;
}

#endif
