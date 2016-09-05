//
//    search_extended_algorithm.h
//
//    Copyright Barry A. Scott (c) 2002-2016
//
#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include <list>
#include <map>

class RegularExpressionTerm;
class EmacsStringStream;
class RegularExpressionGroupStart;
class RegularExpressionGroupEnd;

//
//    The advanced search implementation
//
class SearchAdvancedAlgorithm : public SearchImplementation
{
    friend class SyntaxString;

public:
    enum { MAX_GROUPS = 100 };    // the maximum number of meta-brackets in an RE -- ( )
    SearchAdvancedAlgorithm();
    virtual ~SearchAdvancedAlgorithm();

    void regNumberedGroup( int number, RegularExpressionGroupStart * );
    void regNamedGroup( const EmacsString &name, RegularExpressionGroupStart * );

    // used by any term that needs to match literal strings
    bool matchLiteralString( const EmacsString &string, int pos, int &end_pos );

    bool isCaseFolding() const;
    EmacsChar_t caseFold( EmacsChar_t ch ) const;
    void setCaseFolding( bool enabled );

    int get_number_of_groups();
    int get_start_of_group( int group_number );
    int get_end_of_group( int group_number );
    int get_start_of_group( const EmacsString &group_name );
    int get_end_of_group( const EmacsString &group_name );

    int syntax_looking_at( int dot );

private:
    bool is_compatible( EmacsSearch::sea_type type );

    void compile( const EmacsString &pattern, EmacsSearch::sea_type RE );

    int search( int n, int dot );
    int looking_at( int dot );
    void search_replace_once( const EmacsString &new_string );
private:
    int m_max_group_number;
    enum { MAX_REPEAT = 0x7fffffff };
    std::map<int, RegularExpressionGroupStart *> m_numbered_groups;
    std::map<EmacsString, RegularExpressionGroupStart *> m_named_groups;

    void compile_string( const EmacsString &pattern );
    void compile_expression( const EmacsString &pattern );
    void compile_for_syntax( const EmacsString &pattern );

    RegularExpressionTerm *parse_re( EmacsStringStream &pattern );
    RegularExpressionTerm *parse_term( EmacsStringStream &pattern );
    RegularExpressionTerm *parse_repeat( RegularExpressionTerm *term, EmacsStringStream &pattern );
    RegularExpressionTerm *parse_repeated_char( EmacsChar_t ch, EmacsStringStream &pattern );
    void parse_min_max( EmacsStringStream &pattern, int &repeat_min, int &repeat_max );
    RegularExpressionTerm *parse_set( EmacsStringStream &pattern );
    RegularExpressionTerm *parse_group( EmacsStringStream &pattern );
    RegularExpressionTerm *parse_group_contents( EmacsStringStream &pattern );
    RegularExpressionTerm *parse_syntax_match( EmacsStringStream &pattern );

    RegularExpressionTerm *m_expression;

    bool m_case_fold;        // true if case fold for this search is enabled

    // make private to prevent use
    SearchAdvancedAlgorithm( const SearchAdvancedAlgorithm &src );
    SearchAdvancedAlgorithm &operator=(const SearchAdvancedAlgorithm &src);
};

//
//    Base class for all terms in a regular expression
//
class RegularExpressionTerm
{
public:
    RegularExpressionTerm( SearchAdvancedAlgorithm &owner_ );
    virtual ~RegularExpressionTerm();

    virtual bool matchTerm( int pos, int &end_pos ) = 0;
    virtual bool matchExpression( int pos, int &end_pos );
    virtual bool isStringExpression() { return false; }
    virtual EmacsChar_t stringExpressionFirstChar() { return 0; }

    void setNextTerm( RegularExpressionTerm *next_term );
    void appendTerm( RegularExpressionTerm *next_term );

    virtual bool isStringTerm() { return false; }

protected:
    SearchAdvancedAlgorithm &m_owner;
    RegularExpressionTerm *m_next_term;
};

//
//    Term that matches a string exactly
//
class RegularExpressionString: public RegularExpressionTerm
{
public:
    RegularExpressionString( SearchAdvancedAlgorithm &owner, const EmacsString &string);
    virtual ~RegularExpressionString();

    bool matchTerm( int pos, int &end_pos );

    virtual bool isStringTerm() { return true; }

private:
    EmacsString m_term_string;
};

//
//    Term that matches if the next char is in its char_set
//
class RegularExpressionCharSet: public RegularExpressionTerm
{
public:
    RegularExpressionCharSet( SearchAdvancedAlgorithm &owner, const EmacsString &char_set, bool include_word_chars = false );
    virtual ~RegularExpressionCharSet();

    bool matchTerm( int pos, int &end_pos );

private:
    EmacsString m_char_set;
    bool m_include_word_chars;
};


//
//    Term that matches if the next char is not in its char_set
//
class RegularExpressionNotCharSet: public RegularExpressionTerm
{
public:
    RegularExpressionNotCharSet(
        SearchAdvancedAlgorithm &owner,
        const EmacsString &char_set,
        bool include_word_chars = false );
    virtual ~RegularExpressionNotCharSet();

    bool matchTerm( int pos, int &end_pos );

private:
    EmacsString m_char_set;
    bool m_include_word_chars;
};


//
//    Match if expression matches, but do not move end_pos
//
class RegularExpressionPositiveLookAhead: public RegularExpressionTerm
{
public:
    RegularExpressionPositiveLookAhead(
        SearchAdvancedAlgorithm &owner,
        RegularExpressionTerm *expression_);
    virtual ~RegularExpressionPositiveLookAhead();

    bool matchTerm( int pos, int &end_pos );

private:
    RegularExpressionTerm *m_expression;
};

//
//    Match if expression does not matches, but do not move end_pos
//
class RegularExpressionNegativeLookAhead: public RegularExpressionTerm
{
public:
    RegularExpressionNegativeLookAhead(
        SearchAdvancedAlgorithm &owner,
        RegularExpressionTerm *expression_);
    virtual ~RegularExpressionNegativeLookAhead();

    bool matchTerm( int pos, int &end_pos );

private:
    RegularExpressionTerm *m_expression;
};


//
//    Base class for all types of repeated regular expressions
//
class RegularExpressionRepeat: public RegularExpressionTerm
{
public:
    RegularExpressionRepeat(
        SearchAdvancedAlgorithm &owner,
            int min_repeats, int max_repeats,
            RegularExpressionTerm *repeat_term );
    virtual ~RegularExpressionRepeat();

    virtual bool matchTerm( int pos, int &end_pos );
    virtual bool matchExpression( int pos, int &end_pos ) = 0;

protected:
    int m_min_repeats;
    int m_max_repeats;
    RegularExpressionTerm *m_repeat_term;
};

//
//    Term that repeats its repeat_term most possible times
//
class RegularExpressionRepeatMost: public RegularExpressionRepeat
{
public:
    RegularExpressionRepeatMost(
        SearchAdvancedAlgorithm &owner,
        int min_repeats, int max_repeats,
        RegularExpressionTerm *repeat_term );
    virtual ~RegularExpressionRepeatMost();

    virtual bool matchExpression( int pos, int &end_pos );
private:
    bool matchExpressionMost( int pos, int &end_pos, int matches );
};

//
//    Term that repeats its repeat_term least possible times
//
class RegularExpressionRepeatLeast: public RegularExpressionRepeat
{
public:
    RegularExpressionRepeatLeast(
        SearchAdvancedAlgorithm &owner,
        int min_repeats, int max_repeats,
        RegularExpressionTerm *repeat_term );
    virtual ~RegularExpressionRepeatLeast();

    virtual bool matchExpression( int pos, int &end_pos );
};


//
//    A group of terms allowing alternatives
//
class RegularExpressionAlternation: public RegularExpressionTerm
{
public:
    RegularExpressionAlternation( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionAlternation();

    void addAlternative( RegularExpressionTerm *term );

    virtual bool matchTerm( int pos, int &end_pos );

protected:
    std::list<RegularExpressionTerm *> m_alternative_expressions;
};

//
//    A group that can be back referenced
//
class RegularExpressionGroupStart: public RegularExpressionTerm
{
public:
    friend class RegularExpressionGroupEnd;
    RegularExpressionGroupStart(
        SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionGroupStart();

    virtual bool matchTerm( int pos, int &end_pos );

    const Marker &groupStart() const;
    const Marker &groupEnd() const;
private:
    int m_start_pos;
    Marker m_start_mark;
    Marker m_end_mark;
};

//
//    A group that can be back referenced
//
class RegularExpressionGroupEnd: public RegularExpressionTerm
{
public:
    RegularExpressionGroupEnd(
        SearchAdvancedAlgorithm &owner,
        RegularExpressionGroupStart &group_start_ );
    virtual ~RegularExpressionGroupEnd();

    virtual bool matchTerm( int pos, int &end_pos );

private:
        RegularExpressionGroupStart &m_group_start;
};

//
//    A named group that can be back referenced
//
class RegularExpressionNamedGroup : public RegularExpressionGroupStart
{
public:
    RegularExpressionNamedGroup(
            SearchAdvancedAlgorithm &owner,
            const EmacsString &group_name_ );
    virtual ~RegularExpressionNamedGroup();
private:

    const EmacsString m_group_name;
};

//
//    A numbered group that can be back referenced
//
class RegularExpressionNumberedGroup : public RegularExpressionGroupStart
{
public:
    RegularExpressionNumberedGroup(
        SearchAdvancedAlgorithm &owner,
        int group_number_ );
    virtual ~RegularExpressionNumberedGroup();
private:
    int m_group_number;
};


//
//    regular expression Asserts
//
class RegularExpressionWordBoundary : public RegularExpressionTerm
{
public:
    RegularExpressionWordBoundary( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionWordBoundary();

    bool matchTerm( int pos, int &end_pos );
};

class RegularExpressionNotWordBoundary : public RegularExpressionTerm
{
public:
    RegularExpressionNotWordBoundary( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionNotWordBoundary();

    bool matchTerm( int pos, int &end_pos );
};

class RegularExpressionWordStart : public RegularExpressionTerm
{
public:
    RegularExpressionWordStart( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionWordStart();

    bool matchTerm( int pos, int &end_pos );
};

class RegularExpressionWordEnd : public RegularExpressionTerm
{
public:
    RegularExpressionWordEnd( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionWordEnd();

    bool matchTerm( int pos, int &end_pos );
};

class RegularExpressionBeginningOfLine : public RegularExpressionTerm
{
public:
    RegularExpressionBeginningOfLine( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionBeginningOfLine();

    bool matchTerm( int pos, int &end_pos );
};

class RegularExpressionEndOfLine : public RegularExpressionTerm
{
public:
    RegularExpressionEndOfLine( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionEndOfLine();

    bool matchTerm( int pos, int &end_pos );
};

//
//    back reference
//
class RegularExpressionBackReference : public RegularExpressionTerm
{
public:
    RegularExpressionBackReference(
        SearchAdvancedAlgorithm &owner,
        int back_ref_ );
    virtual ~RegularExpressionBackReference();

    bool matchTerm( int pos, int &end_pos );

private:
    int m_back_ref;
};

class RegularExpressionSyntaxMatch : public RegularExpressionTerm
{
public:
    RegularExpressionSyntaxMatch( SearchAdvancedAlgorithm &owner );
    virtual ~RegularExpressionSyntaxMatch();

    bool matchTerm( int pos, int &end_pos );

    void addAnyOf( SyntaxKind_t mask, SyntaxKind_t value );
    void addNoneOf( SyntaxKind_t mask, SyntaxKind_t value );

private:
    typedef std::pair<SyntaxKind_t, SyntaxKind_t> SyntaxMatchPair_t;
    typedef std::list<SyntaxMatchPair_t> SyntaxMatchList_t;

    SyntaxMatchList_t   m_any_of;
    SyntaxMatchList_t   m_none_of;
};
