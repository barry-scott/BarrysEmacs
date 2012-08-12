// Copyright (c) 1985
//        Barry A. Scott and Nick Emery
//
// Copyright (c) 1986-2012
//        Barry A. Scott
//
// Declarations having to do with Emacs searching


// meta characters in the "compiled" form of a regular expression
#define CBRA                 2u    // \( -- begin bracket
#define CCHR                 4u    // a vanilla character
#define CDOT                 6u    // . -- match anything except a newline
#define CCL                  8u    // [...] -- character class
#define NCCL                10u    // [^...] -- negated character class
#define CDOL                12u    // $ -- matches the end of a line
#define CEOP                14u    // The end of the pattern
#define CKET                16u    // \) -- close bracket
#define CBACK               18u    // \N -- backreference to the Nth bracketed string
#define CIRC                20u    // ^ matches the beginning of a line
#define BBUF                22u    // beginning of buffer \`
#define EBUF                24u    // end of buffer \'
#define BDOT                26u    // matches before dot \<
#define EDOT                28u    // matches at dot \=
#define ADOT                30u    // matches after dot \>
#define WORDC               32u    // matches word character \w
#define NWORDC              34u    // matches non-word characer \W
#define WBOUND              36u    // matches word boundary \b
#define NWBOUND             38u    // matches non-(word boundary) \B

// the following are only allowed when syntax_array is used
#define SEA_SYN_COMMENT     40u     // only match inside a comment \c
#define SEA_SYN_NOCOMMENT   42u     // only match outside a comment \C
#define SEA_SYN_STRING      44u     // only match inside a string \s
#define SEA_SYN_NOSTRING    46u     // only match outside a string \S

#define STAR                 1u     // * -- Kleene star, repeats the previous
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

    int search( int n, int dot );
    int looking_at();
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
