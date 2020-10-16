// Copyright (c) 1985
//        Barry A. Scott and Nick Emery
//
// Copyright (c) 1986-2016
//        Barry A. Scott
//
// Declarations having to do with Emacs searching


// meta characters in the "compiled" form of a regular expression
#define CBRA               EmacsChar_t(   2 )  // \( -- begin bracket
#define CCHR               EmacsChar_t(   4 )  // a vanilla character
#define CDOT               EmacsChar_t(   6 )  // . -- match anything except a newline
#define CCL                EmacsChar_t(   8 )  // [...] -- character class
#define NCCL               EmacsChar_t(  10 )  // [^...] -- negated character class
#define CDOL               EmacsChar_t(  12 )  // $ -- matches the end of a line
#define CEOP               EmacsChar_t(  14 )  // The end of the pattern
#define CKET               EmacsChar_t(  16 )  // \) -- close bracket
#define CBACK              EmacsChar_t(  18 )  // \N -- backreference to the Nth bracketed string
#define CIRC               EmacsChar_t(  20 )  // ^ matches the beginning of a line
#define BBUF               EmacsChar_t(  22 )  // beginning of buffer \`
#define EBUF               EmacsChar_t(  24 )  // end of buffer \'
#define BDOT               EmacsChar_t(  26 )  // matches before dot \<
#define EDOT               EmacsChar_t(  28 )  // matches at dot \=
#define ADOT               EmacsChar_t(  30 )  // matches after dot \>
#define WORDC              EmacsChar_t(  32 )  // matches word character \w
#define NWORDC             EmacsChar_t(  34 )  // matches non-word characer \W
#define WBOUND             EmacsChar_t(  36 )  // matches word boundary \b
#define NWBOUND            EmacsChar_t(  38 )  // matches non-(word boundary) \B

// the following are only allowed when syntax_array is used
#define SEA_SYN_COMMENT    EmacsChar_t(  40 )   // only match inside a comment \c
#define SEA_SYN_NOCOMMENT  EmacsChar_t(  42 )   // only match outside a comment \C
#define SEA_SYN_STRING     EmacsChar_t(  44 )   // only match inside a string \s
#define SEA_SYN_NOSTRING   EmacsChar_t(  46 )   // only match outside a string \S

#define STAR               EmacsChar_t(   1 )   // * -- Kleene star, repeats the previous
                                                // RE as many times as possible; the value
                                                // ORs with the other operator types


class SearchSimpleAlgorithm : public SearchImplementation
{
public:
    enum { MAX_NBRA = 64 };         // the maximum number of meta-brackets in an RE -- \( \)
    enum { MAX_NALTS = 64 };        // the maximum number of \|'s

    SearchSimpleAlgorithm();
    virtual ~SearchSimpleAlgorithm();

private:
    bool is_compatible( EmacsSearch::sea_type type );

    void compile( const EmacsString &strp, EmacsSearch::sea_type RE );
    void setCaseFolding( bool ) { return; }

    int search( int n, int dot );
    int looking_at( int dot );
    int syntax_looking_at( int dot ) const;
    void place( int l1, int l2);
    int perform_replace( int query, EmacsSearch::sea_type RE);
    void search_replace_once( const EmacsString &new_string );

    int get_number_of_groups();
    int get_start_of_group( int group_number );
    int get_end_of_group( int group_number );
    int get_start_of_group( const EmacsString &group_name );
    int get_end_of_group( const EmacsString &group_name );

    int search_execute( int fflag, int addr);
    int search_advance( int lp, EmacsChar_t *ep, int syn_include, int syn_exclude );

    int backref( int i, int lp );
    int cclass( EmacsChar_t *char_set, EmacsChar_t c, int af );

    enum { ESIZE = 8192 };                          // the maximum size of an RE

    EmacsChar_t sea_expbuf[ ESIZE+4 ];              // The most recently compiled search string
    EmacsChar_t *sea_alternatives[ MAX_NALTS+1 ];   // The list of \| seperated alternatives
    Marker sea_bra_slist[ MAX_NBRA ];               // RE meta-bracket start list
    Marker sea_bra_elist[ MAX_NBRA ];               // RE meta-bracket end list
    int sea_loc1;                                   // The buffer position of the first character of the most recently found string
    int sea_loc2;                                   // The buffer position of the character following the most recently found string
    int sea_nbra;                                   // The number of meta-brackets in the most recently compiled RE
    bool sea_case_fold;                             // True if case blind compare

private:
    EmacsSearch::sea_type compiled_expression_type;

    // make private to prevent use
    SearchSimpleAlgorithm( const SearchSimpleAlgorithm &src );
    SearchSimpleAlgorithm &operator=(const SearchSimpleAlgorithm &src);
};
