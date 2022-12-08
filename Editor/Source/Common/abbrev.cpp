//
//    Copyright (c) 1982-1993
//        Barry A. Scott and Nick Emery
//    Copyright (c) 1994-2010
//        Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


static int hash_abbrev( const EmacsString &s );
static AbbrevEntry *lookup_abbrev( AbbrevTable *table, const EmacsString &name, int h );
static void define_abbrev_phrase( AbbrevTable *table, const EmacsString &abbrev, const EmacsString &phrase, BoundName *proc );
static void define_abbrev( AbbrevTable *table, const char *s, int hooked );
int define_global_abbrev( void );
int define_local_abbrev( void );
int define_hooked_global_abbrev( void );
int define_hooked_local_abbrev( void );
int test_abbrev_expand( void );
int abbrev_expand( void );
int use_abbrev_table( void );
int write_abbrev_file( void );
static int read_abbrevs( EmacsString s, int report );
int read_abbrev_file( void );
int quietly_read_abbrev_file( void );
void init_abbrev( void );


AbbrevNameTable AbbrevTable::name_table( 8, 8 );

AbbrevTable global_abbrev( "global" );
SystemExpressionRepresentationStringReadOnly last_phrase;

EmacsString two_percents("%s: %s");
EmacsString abbreviations(".abbreviations");

AbbrevEntry::AbbrevEntry()
: abbrev_next( NULL )           // the next pair in this chain
, abbrev_abbrev()               // the abbreviation
, abbrev_phrase()               // the expanded phrase
, abbrev_hash(0)                // abbrev_abbrev hashed
, abbrev_expansion_hook(NULL)   // BoundName
{ }


AbbrevTable::AbbrevTable( EmacsString name )
: abbrev_name( name )           // the name of this abbrev table
, abbrev_number_defined(0)      // the number of abbrevs defined in this abbrev table
{
    for(int i=0; i<ABBREVSIZE; i++ )
        abbrev_table[i] = NULL;

    name_table.add( name, this );
}

AbbrevTable::~AbbrevTable()
{
    name_table.remove( abbrev_name );
}

static int hash_abbrev( const EmacsString &s )
{
    int len = s.length();
    int h = 0;
    for( int i=0; i<len; i++ )
        h = h * 31 + s[i];
    return h&INT_MAX;       // make sure that h is positive
}

//
// look up an abbrev in the given table with the given name whose hash_abbrev is h.
//
static AbbrevEntry *lookup_abbrev
    (
    AbbrevTable *table,
    const EmacsString &name,
    int h
    )
{
    AbbrevEntry *p;

    p = table->abbrev_table[h % AbbrevTable::ABBREVSIZE ];
    while( p != 0
    &&    (p->abbrev_hash != h
        || p->abbrev_abbrev != name ) )
        p = p->abbrev_next;
    return p;
}

// in the given abbrev table define_abbrev the given abbreviation for the given phrase
static void define_abbrev_phrase
    (
    AbbrevTable *table,
    const EmacsString &abbrev,
    const EmacsString &phrase,
    BoundName *proc
    )
{
    AbbrevEntry *p;
    int h;

    h = hash_abbrev( abbrev );
    if( (p = lookup_abbrev( table, abbrev, h )) == NULL )
    {
        p = EMACS_NEW AbbrevEntry;
        p->abbrev_hash = h;
        p->abbrev_abbrev = abbrev;
        p->abbrev_next = table->abbrev_table[h % AbbrevTable::ABBREVSIZE ];
        table->abbrev_number_defined = table->abbrev_number_defined + 1;
        table->abbrev_table[h % AbbrevTable::ABBREVSIZE ] = p;
    }
    p->abbrev_phrase = phrase;
    p->abbrev_expansion_hook = proc;
}

static void define_abbrev
    (
    AbbrevTable *table,
    const char *s,
    int hooked
    )
{
    EmacsString abbrev;
    EmacsString phrase;
    BoundName *proc = NULL;

    if( cur_exec == NULL )
    {
        abbrev = get_nb_string_interactive( FormatString(": define-%s%s-abbrev ") <<
                (hooked ?  "hooked-" : "") << s );
        if( abbrev.isNull() )
            return;

        phrase = get_string_interactive( FormatString(": define-%s%s-abbrev %s phrase: ") <<
            (hooked ? "hooked-" : "") << s << abbrev );
        if( phrase.isNull() )
            return;

        if( hooked && (proc = BoundName::get_word_interactive( "Hooked to procedure: " )) == NULL )
            return;
    }
    else
    {
        abbrev = get_string_mlisp();
        if( abbrev.isNull() )
            return;

        phrase = get_string_mlisp();
        if( phrase.isNull() )
            return;

        if( hooked && (proc = BoundName::get_word_mlisp()) == NULL )
            return;
    }

    define_abbrev_phrase( table, abbrev, phrase, proc );
}

int define_global_abbrev( void )
{
    define_abbrev( &global_abbrev, "global", 0 );

    bf_cur->b_mode.md_abbrevon = 1;
    return 0;
}

int define_local_abbrev( void )
{
    if( bf_cur->b_mode.md_abbrev == 0 )
        error( "No abbrev table associated with this buffer.");
    else
        define_abbrev( bf_cur->b_mode.md_abbrev, "local", 0 );

    bf_cur->b_mode.md_abbrevon = 1;
    return 0;
}

int define_hooked_global_abbrev( void )
{
    define_abbrev( &global_abbrev, "global", 1 );
    return 0;
}

int define_hooked_local_abbrev( void )
{
    if( bf_cur->b_mode.md_abbrev == 0 )
        error( "No abbrev table associated with this buffer.");
    else
        define_abbrev( bf_cur->b_mode.md_abbrev, "local", 1);
    return 0;
}

int test_abbrev_expand( void )
{
    EmacsString abbrev;
    AbbrevEntry *p;

    abbrev = getnbstr( ": test-abbrev-expand " );
    p = 0;

    if( abbrev.isNull() )
        return 0;
    if( bf_cur->b_mode.md_abbrev != 0 )
        p = lookup_abbrev( bf_cur->b_mode.md_abbrev, abbrev, hash_abbrev( abbrev ) );
    if( p == 0 )
        p = lookup_abbrev( &global_abbrev, abbrev, hash_abbrev( abbrev ) );
    if( p == 0 )
        error( FormatString("Abbrev \"%s\" is not defined") << abbrev );
    else
    {
        message( FormatString("\"%s\" -> \"%s\" (%d)") <<
            p->abbrev_abbrev << p->abbrev_phrase << p->abbrev_hash );
        ml_value = Expression( p->abbrev_phrase );
    }
    return 0;
}

//
// called from self_insert to possibly expand the abbrev that preceeds dot
//
int abbrev_expand( void )
{
    int n = dot;
    int rv = 0;
    int upper_count = 0;

    for(;;)
    {
        n--;
        if( n < 1 )
            break;
        if( !bf_cur->char_at_is( n, SYNTAX_WORD ) )
        {
            n++;
            break;
        }
        if( unicode_is_upper( bf_cur->char_at( n ) ) )
            upper_count++;
        }

    bf_cur->gap_outside_of_range( n, dot );
    EmacsString phrase_original( EmacsString::copy, bf_cur->ref_char_at(n), dot - n );
    EmacsString phrase( phrase_original );
    phrase.toLower();

    AbbrevEntry *a;
    int h = hash_abbrev( phrase );
    if( (bf_cur->b_mode.md_abbrev == NULL
    || (a = lookup_abbrev( bf_cur->b_mode.md_abbrev, phrase, h )) == NULL)
       &&
       (a = lookup_abbrev( &global_abbrev, phrase, h )) == NULL )
        return 0;

    if( a->abbrev_expansion_hook != NULL )
    {
        last_phrase = a->abbrev_phrase;
        rv = execute_bound_saved_environment( a->abbrev_expansion_hook );
        last_phrase = EmacsString::null;
    }
    else
    {
        // turn off abbrev to prevent recursive expansion
        bf_cur->b_mode.md_abbrevon = 0;
        bf_cur->del_back( dot, phrase.length() );
        dot_left( phrase.length() );

        int pos;
        for( pos=0; pos < a->abbrev_phrase.length(); pos++ )
        {
            EmacsChar_t ch = a->abbrev_phrase[ pos ];
            if( unicode_is_lower( ch )
            && upper_count != 0
            &&    (pos == 0
                || (upper_count > 1 && (pos > 1 && unicode_is_space( a->abbrev_phrase[ pos-1 ] ))))
            )
                self_insert( unicode_to_upper( ch ) );
            else
                self_insert( ch );
        }

        // turn abbrev back on
        bf_cur->b_mode.md_abbrevon = 1;
    }
    return rv;
}

//
// select a named abbrev table for this buffer
// and turn on abbrev mode if it or the global
// abbrev table is non-empty
//
int use_abbrev_table( void )
{
    EmacsString name;
    getescword( AbbrevTable::, ": use-abbrev-table ", name );
    if( name.isNull() )
        return 0;
    AbbrevTable *p = AbbrevTable::find( name );
    if( p == NULL )
        p = EMACS_NEW AbbrevTable( name );

    bf_cur->b_mode.md_abbrev = p;

    if( p->abbrev_number_defined > 0
    || global_abbrev.abbrev_number_defined > 0 )
    {
        bf_cur->b_mode.md_abbrevon = 1;
        bf_cur->b_mode.md_abbrevon = 1;
        cant_1win_opt = 1;
    }

    return 0;
}



int list_abbreviation_tables( void )
{
    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Abbreviation table list", interactive() );

    bf_cur->ins_str( "  Table\n"
         "  -----\n" );

    for( int i=0; i<AbbrevTable::name_table.entries(); i++ )
    {
        bf_cur->ins_cstr( FormatString("  %s\n") << AbbrevTable::name_table.key( i ) );
    }

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

int dump_abbreviation_tables( void )
{
    AbbrevTable *table = getword( AbbrevTable::, ": dump-abbreviation-tables (table name) " );
    if( table == NULL )
        return 0;

    EmacsBufferRef old( bf_cur );

    EmacsBuffer::scratch_bfn( "Abbreviation table", interactive() );

    bf_cur->ins_cstr( FormatString("  Table: %s\n\n") << table->abbrev_name );

    bf_cur->ins_str(
            "  Abbreviation    Phrase                          Hook\n"
            "  ------------    ------                          ----\n" );

    for( int i=0; i<AbbrevTable::ABBREVSIZE; i++ )
    {
        AbbrevEntry *p = table->abbrev_table[i ];
        while( p != 0 )
        {
            const unsigned char *hook = u_str("");

            if( p->abbrev_expansion_hook != NULL )
                hook = p->abbrev_expansion_hook->b_proc_name ;

            bf_cur->ins_cstr( FormatString("  %-15s %-31s %s\n") <<
                                p->abbrev_abbrev << p->abbrev_phrase << hook );

            p = p->abbrev_next;
        }
    }

    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}


//
// write the given abbrev table to file f
//
static void write_abbrevs( EmacsFile &f, AbbrevTable *table )
{
    EmacsString header( FormatString("%s\n") << table->abbrev_name );
    f.fio_put( header );

    for( int i=0; i<AbbrevTable::ABBREVSIZE; i++ )
    {
        AbbrevEntry *p = table->abbrev_table[i];
        while( p != 0 )
        {
            EmacsString buf( FormatString(" %s    %s\n") <<
                        p->abbrev_abbrev <<
                        p->abbrev_phrase );
            f.fio_put( buf );
            p = p->abbrev_next;
        }
    }
}

int write_abbrev_file( void )
{
    EmacsString fn;
    if( cur_exec == NULL )
    {
        fn = get_string_interactive( ": write-abbrev-file " );
    }
    else
    {
        fn = get_string_mlisp();
    }
    if( fn.isNull() )
    {
        return 0;
    }

    EmacsFile f( fn, abbreviations );
    f.fio_create( FIO_STD, (FIO_EOL_Attribute)(int)default_end_of_line_style );
    if( !f.fio_is_open() )
    {
         error( FormatString( two_percents ) << fetch_os_error( errno ) << fn );
    }
    else
    {
        for( int i=0; i<AbbrevTable::name_table.entries(); i += 1 )
        {
            write_abbrevs( f, AbbrevTable::name_table.value( i ) );
        }
        f.fio_close();
    }

    return 0;
}

static int read_abbrevs
    (
    EmacsString s,
    int report
    )
{
    // QQQ - unicode - needs to read in unicode and process
    int len;
    unsigned char buf[500];
    unsigned char *p;
    unsigned char *phrase;
    EmacsString fullname;

    EmacsString name;
    if( cur_exec == NULL )
        name = get_string_interactive( s );
    else
        name = get_string_mlisp();
    if( name.isNull() )
        return 0;

    AbbrevTable *table = 0;
    EmacsFile f;
    if( !f.fio_find_using_path( EMACS_PATH, name, abbreviations ) || !f.fio_open() )
    {
        if( report )
        {
            error( FormatString(two_percents) << fetch_os_error( errno ) << name);
        }
        return 0;
    }

    while( (len = f.fio_get_line( buf, sizeof( buf )-1 )) > 0
    && ! ml_err )
    {
        buf[ len ] = 0;
        if( buf[0] != ' ' )
        {
            p = buf;
            while( p[0] != 0 )
                if( *p++ == '\n' )
                    p[-1] = 0;
            table = AbbrevTable::find( EmacsString( buf ) );
            if( table == NULL )
                table = EMACS_NEW AbbrevTable( EmacsString( buf ) );
        }
        else
            if( table != NULL )
            {
                p = &buf[1];
                while( p[0] != 0 && p[0] != '\t' )
                    p = &p[1];
                if( p[0] == 0 )
                {
                    error( FormatString("Improperly formatted abbrev file: %s") << name );
                    return 0;
                }
                *p++ = '\0';
                phrase = p;
                while( p[0] != 0 && p[0] != '\n' )
                    p = &p[1];
                p[0] = 0;
                define_abbrev_phrase( table, EmacsString(&buf[1]), EmacsString(phrase), 0 );
            }
    }
    f.fio_close();

    return 0;
}

int read_abbrev_file( void )
{
    return read_abbrevs( ": read-abbrev-file ", 1 );
}

int quietly_read_abbrev_file( void )
{
    return read_abbrevs( ": quietly-read-abbrev-file ", 0 );
}

void SystemExpressionRepresentationAbbrevTable::fetch_value()
{
    if( bf_cur->b_mode.md_abbrev != 0 )
        *this = bf_cur->b_mode.md_abbrev->abbrev_name;
    else
        *this = EmacsString::null;
}

void SystemExpressionRepresentationAbbrevTable::assign_value
    ( ExpressionRepresentation *new_value )
{
    AbbrevTable *p;

    const EmacsString &value = new_value->asString();
    if( value.isNull() )
    {
        error( "Illegal abbrev table name" );
        return;
    }

    p = AbbrevTable::find( value );
    if( p == NULL )
        p = EMACS_NEW AbbrevTable( value );

    bf_cur->b_mode.md_abbrev = p;
    if( p->abbrev_number_defined > 0
    || global_abbrev.abbrev_number_defined > 0 )
    {
        bf_cur->b_mode.md_abbrevon = 1;
        bf_cur->b_mode.md_abbrevon = 1;
        cant_1win_opt = 1;
    }
}
