//
//    search.h
//
//    Copyright Barry A. Scott (c) 2002-2016
//

class SearchImplementation;

class EmacsSearch
{
public:
    enum sea_type
    {
        sea_type__string = 0,
        sea_type__RE_simple,
        sea_type__RE_extended,
        sea_type__RE_syntax     // simplified extended for use in syntax tables
    };

    EmacsSearch();
    EmacsSearch( EmacsSearch &other );
    ~EmacsSearch();

    EmacsSearch &operator=( EmacsSearch &other );

    int search( const EmacsString &s, int n, int dot, sea_type RE );
    int search( int n, int dot );
    int looking_at( const EmacsString &s, sea_type RE );
    void compile( const EmacsString &strp, sea_type RE );

    void search_replace_once( const EmacsString &new_string );

    int get_number_of_groups();
    int get_start_of_group( int group_number );
    int get_end_of_group( int group_number );
    int get_start_of_group( const EmacsString &group_name );
    int get_end_of_group( const EmacsString &group_name );


private:
    SearchImplementation *operator->();
    SearchImplementation *m_search_implementation;
};

class SearchImplementation
{
public:
    SearchImplementation();
    virtual ~SearchImplementation();
    virtual bool is_compatible( EmacsSearch::sea_type type ) = 0;

    virtual int search( int n, int dot ) = 0;
    virtual int looking_at( int dot ) = 0;
    virtual void compile( const EmacsString &strp, EmacsSearch::sea_type RE ) = 0;

    virtual void search_replace_once( const EmacsString &new_string ) = 0;

    virtual int get_number_of_groups() = 0;
    virtual int get_start_of_group( int group_number ) = 0;
    virtual int get_end_of_group( int group_number ) = 0;
    virtual int get_start_of_group( const EmacsString &group_name ) = 0;
    virtual int get_end_of_group( const EmacsString &group_name ) = 0;


private:
    friend class EmacsSearch;
    int ref_count;

    SearchImplementation( const SearchImplementation & );
    SearchImplementation &operator=( const SearchImplementation & );
};

