//    Copyright (c) 1982-1995
//        Barry A. Scott
//
// Definitions for Unix Emacs Abbrev mode
// An abbrev table contains an array of pointers to abbrev entries.  When a
// word is to be looked up in a abbrev table it is hashed to a long value and
// that value is taken mod the array size to get the head of the appropriate
// chain.  The chain is scanned for an entry whose hash matches (comparing
// hash values is faster than comparins strings) and whose string matches.
//

class AbbrevNameTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( AbbrevNameTable )
    AbbrevNameTable( int init_size, int grow_amount )
        : EmacsStringTable( init_size, grow_amount )
    { }
    virtual ~AbbrevNameTable()
    { }

    void add( const EmacsString &key, AbbrevTable *value )
    { EmacsStringTable::add( key, value ); }
    AbbrevTable *remove( const EmacsString &key )
    { return (AbbrevTable *)EmacsStringTable::remove( key ); }
    AbbrevTable *find( const EmacsString &key )
    { return (AbbrevTable *)EmacsStringTable::find( key ); }
    AbbrevTable *value( int index )
    { return (AbbrevTable *)EmacsStringTable::value( index ); }
};

class AbbrevEntry : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( AbbrevEntry )
    AbbrevEntry();
    AbbrevEntry *abbrev_next;        // the next pair in this chain
    EmacsString abbrev_abbrev;        // the abbreviation
    EmacsString abbrev_phrase;        // the expanded phrase
    int abbrev_hash;            // abbrev_abbrev hashed
    BoundName *abbrev_expansion_hook;    // the command that will be executed when this abbrev is expanded
};

// a table of abbreviations and their expansions
class AbbrevTable : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( AbbrevTable )
    AbbrevTable( EmacsString name );
    ~AbbrevTable();

    static AbbrevTable *find( const EmacsString &name )
    { return name_table.find( name ); }

    // return one of the keys in the table otherwise NULL
    static AbbrevTable *get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static AbbrevTable *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }
    static AbbrevTable *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

    // return one of the keys in the table otherwise NULL
    static EmacsString &get_esc_word_mlisp( EmacsString &result )
    { return name_table.get_esc_word_mlisp( result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, EmacsString::null, result ); }
    static EmacsString &get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
    { return name_table.get_esc_word_interactive( prompt, default_value, result ); }

    EmacsString abbrev_name;        // the name of this abbrev table
    int abbrev_number_defined;        // the number of abbrevs defined in this abbrev table
    enum {ABBREVSIZE = 87};
    AbbrevEntry *abbrev_table [ABBREVSIZE];    // the array of pointers to chains of name pairs

    static AbbrevNameTable name_table;
};
