//
//    Copyright (c) 1982-2019
//
//        Barry A. Scott
//

#if defined( vms )
class database : public EmacsObject
{
    EMACS_OBJECT_FUNCTIONS( database )
    int db_rdonly;
    int db_reopen;
    EmacsString db_name;
    int db_dirf;
    int db_pagf;
    int db_datf;
};
#elif defined( DB_MLL )
struct database_entry
{
    unsigned char *dbe_name;
    int dbe_position;
    int dbe_length;
};

database
{
    int db_rdonly;
    int db_reopen;
    EmacsString db_name;
    int db_dirf;
    FILE *db_file;
    size_t db_data_length;
    unsigned char *db_data_pointer;
    int db_num_entries;
    struct database_entry *db_entries;
};
#elif defined( DB_SQLITE3 )
// sqlite3 version
#include <sqlite3.h>

class database : public EmacsObject
{
public:
    typedef int datum;  // place holder

    EMACS_OBJECT_FUNCTIONS( database )
    database();
    virtual ~database();

    // access flags
    enum {
        access_readonly = 1,
        access_keepopen = 2,    // used by dbman
        access_may_create = 4
    };

    int put_db( const EmacsString &key, const unsigned char *value, int length );
    int get_db( const EmacsString &key, EmacsString &value );
    int del_db( const EmacsString &key );
    int get_db_help( const EmacsString &key, EmacsString &value );
    int index_db( const EmacsString &, void (*)( const EmacsString & ) );
    bool open_db( const EmacsString &name, bool readonly, bool may_create );
    void close_db();

private:
    // functions
    bool reopen_db();

public:
    // data
    EmacsString db_name;
    // state
    bool db_is_open;
    bool db_is_readonly;
    // desired access
    bool db_access_readonly;
    bool db_access_keepopen;    // used by dbman
    bool db_access_may_create;

private:
    EmacsString db_filename;

    // the database
    sqlite3 *db_ptr;

    // prepared statements
    sqlite3_stmt *db_stmt_select_value;
    sqlite3_stmt *db_stmt_select_all_keys;
    sqlite3_stmt *db_stmt_insert_key_value;
    sqlite3_stmt *db_stmt_delete_key;

    // data
    static database *lastdatabase;
};

#else
// ndbm version
class database : public EmacsObject
{
public:
    enum { PBLKSIZ = 4096 };    // Page block size
    enum { DBLKSIZ = 4096 };    // directory block size
    enum { BYTESIZ = 8 };       // bits per byte

    // access flags
    enum {
        access_readonly = 1,
        access_keepopen = 2,    // used by dbman
        access_may_create = 4
    };

    class datum
    {
    public:
        datum( database & );
        datum( database &, unsigned char *,int );
        datum( database &, const datum &);
        datum &operator=( const datum & );

        int calchash();
        int cmpdatum( const datum &other );
        const EmacsString asString() const;

        const unsigned char *dptr;
        int dsize;
        int val1, val2;
    private:
        database &db;
    };

    friend class datum;

    friend class Entry;
public:
    EMACS_OBJECT_FUNCTIONS( database )
    database();
    virtual ~database();

    int put_db( const EmacsString &key, const unsigned char *value, int length );
    int get_db( const EmacsString &key, EmacsString &value );
    int del_db( const EmacsString &key );
    int get_db_help( const EmacsString &key,EmacsString &value );
    int index_db( const EmacsString &, void (*)( const EmacsString & ) );
    bool open_db( const EmacsString &name, bool readonly, bool may_create );
    void close_db();

    datum firstkey();
    datum nextkey( datum & );
    int delete_key( datum & );

    // data
    EmacsString db_name;
    // state
    bool db_is_open;
    bool db_is_readonly;
    // desired access
    bool db_access_readonly;
    bool db_access_keepopen;    // used by dbman
    bool db_access_may_create;

private:
    // functions
    bool reopen_db();
    int forder( datum & );
    datum fetch( datum & );
    int store( datum & );
    datum firsthash( int );
    int hashinc( int );
    int setup_db();
    static int getlong( unsigned char *p );
    static void putlong( unsigned char *p, int val );
    static short getshort( unsigned char *p );
    static void putshort( unsigned char *p, short val );
    void ndbm_access( int hash );
    int getbit();
    int ndbm_setbit();
    int delitem( unsigned char buf[PBLKSIZ], int n );
    int additem( unsigned char buf[PBLKSIZ], datum &item );
    int chkblk( unsigned char buf[PBLKSIZ] );

#if DBG_EXEC
    void dbg_check_fildes( const char *title );
#endif

private:
    // data
    static database *lastdatabase;

    int db_dirf;
    int db_pagf;
    int db_datf;
    int bitno;
    int maxbno;
    int blkno;
    int hmask;
    int oldpagb;
    int olddirb;
    unsigned char pagbuf[PBLKSIZ];
    unsigned char dirbuf[DBLKSIZ];
    EmacsString dirnm;
    EmacsString datnm;
    EmacsString pagnm;

    bool databaseUsesLSB_byteOrdering;
};
#endif

class DatabaseSearchList;

class DatabaseSearchListTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( DatabaseSearchListTable )
    DatabaseSearchListTable( int init_size, int grow_amount )
    : EmacsStringTable( init_size, grow_amount )
    { }
    virtual ~DatabaseSearchListTable()
    { }

    void add( const EmacsString &key, DatabaseSearchList *value )
    {
        EmacsStringTable::add( key, value );
    }
    DatabaseSearchList *remove( const EmacsString &key )
    {
        return (DatabaseSearchList *)EmacsStringTable::remove( key );
    }
    DatabaseSearchList *find( const EmacsString &key )
    {
        return (DatabaseSearchList *)EmacsStringTable::find( key );
    }
    DatabaseSearchList *value( int index )
    {
        return (DatabaseSearchList *)EmacsStringTable::value( index );
    }
};

// maximum number of components in a database search list
class DatabaseSearchList : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( DatabaseSearchList )
    DatabaseSearchList( EmacsString &name );
    virtual ~DatabaseSearchList();

    static DatabaseSearchList *find( const EmacsString &name )
    {
        return name_table.find( name );
    }

    // return one of the keys in the table otherwise NULL
    static DatabaseSearchList *get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static DatabaseSearchList *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }
    static DatabaseSearchList *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

    // return one of the keys in the table otherwise NULL
    static EmacsString &get_esc_word_mlisp( EmacsString &result )
    {
        return name_table.get_esc_word_mlisp( result );
    }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
    {
        return name_table.get_esc_word_interactive( prompt, EmacsString::null, result );
    }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
    {
        return name_table.get_esc_word_interactive( prompt, default_value, result );
    }

    enum { SEARCH_LEN = 10 };
    EmacsString dbs_name;
    int dbs_size;                           // number of components
    database *dbs_elements [SEARCH_LEN];    // the components - each is a database from the ndbms package

    static DatabaseSearchListTable name_table;
};
