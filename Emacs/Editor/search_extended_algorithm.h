//
//	saerch_advanced_algorithm.h
//
//	Copyright Barry A. Scott (c) 2002
//
#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include <list>
#include <map>

class RegularExpressionTerm;
class EmacsStringStream;
class RegularExpressionGroup;

//
//	The advanced search implementation
//
class SearchAdvancedAlgorithm : public SearchImplementation
	{
public:
	enum { MAX_GROUPS = 100 };	// the maximum number of meta-brackets in an RE -- ( )
	SearchAdvancedAlgorithm();
	virtual ~SearchAdvancedAlgorithm();

	void regNumberedGroup( int number, RegularExpressionGroup * );
	void regNamedGroup( const EmacsString &name, RegularExpressionGroup * );

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

private:
	bool is_compatible( EmacsSearch::sea_type type );

	void compile( const EmacsString &pattern, EmacsSearch::sea_type RE );

	int search( int n, int dot );
	int looking_at();
	void search_replace_once( const EmacsString &new_string );
private:
	int max_group_number;
	enum { MAX_REPEAT = 0x7fffffff };
	std::map<int,RegularExpressionGroup *> numbered_groups;
	std::map<EmacsString,RegularExpressionGroup *> named_groups;

	void compile_string( const EmacsString &pattern );
	void compile_expression( const EmacsString &pattern );

	RegularExpressionTerm *parse_re( EmacsStringStream &pattern );
	RegularExpressionTerm *parse_term( EmacsStringStream &pattern );
	RegularExpressionTerm *parse_repeat( RegularExpressionTerm *term, EmacsStringStream &pattern );
	RegularExpressionTerm *parse_repeated_char( EmacsChar_t ch, EmacsStringStream &pattern );
	void parse_min_max( EmacsStringStream &pattern, int &repeat_min, int &repeat_max );
	RegularExpressionTerm *parse_set( EmacsStringStream &pattern );
	RegularExpressionTerm *parse_group( EmacsStringStream &pattern );
	RegularExpressionTerm *parse_group_contents( EmacsStringStream &pattern );

	RegularExpressionTerm *expression;
	
	bool case_fold;		// true if case fold for this search is enabled

	// make private to prevent use
	SearchAdvancedAlgorithm( const SearchAdvancedAlgorithm &src );
	SearchAdvancedAlgorithm &operator=(const SearchAdvancedAlgorithm &src);
	};

//
//	Base class for all terms in a regular expression
//
class RegularExpressionTerm
	{
public:
	RegularExpressionTerm( SearchAdvancedAlgorithm &owner_ );
	virtual ~RegularExpressionTerm();

	virtual bool matchTerm( int pos, int &end_pos ) = 0;
	virtual bool matchExpression( int pos, int &end_pos );

	void setNextTerm( RegularExpressionTerm *next_term_ );

protected:
	SearchAdvancedAlgorithm &owner;
	RegularExpressionTerm *next_term;
	};

//
//	Term that matches a string exactly
//
class RegularExpressionString: public RegularExpressionTerm
	{
public:
	RegularExpressionString( SearchAdvancedAlgorithm &owner, const EmacsString &string);
	virtual ~RegularExpressionString();

	bool matchTerm( int pos, int &end_pos );

private:
	EmacsString term_string;
	};

//
//	Term that matches if the next char is in its char_set
//
class RegularExpressionCharSet: public RegularExpressionTerm
	{
public:
	RegularExpressionCharSet( SearchAdvancedAlgorithm &owner, const EmacsString &char_set, bool include_word_chars = false );
	virtual ~RegularExpressionCharSet();

	bool matchTerm( int pos, int &end_pos );

private:
	EmacsString char_set;
	bool include_word_chars;
	};


//
//	Term that matches if the next char is not in its char_set
//
class RegularExpressionNotCharSet: public RegularExpressionTerm
	{
public:
	RegularExpressionNotCharSet( SearchAdvancedAlgorithm &owner, const EmacsString &char_set, bool include_word_chars = false );
	virtual ~RegularExpressionNotCharSet();

	bool matchTerm( int pos, int &end_pos );

private:
	EmacsString char_set;
	bool include_word_chars;
	};


//
//	Match if expression matches, but do not move end_pos
//
class RegularExpressionPositiveLookAhead: public RegularExpressionTerm
	{
public:
	RegularExpressionPositiveLookAhead( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_);
	virtual ~RegularExpressionPositiveLookAhead();

	bool matchTerm( int pos, int &end_pos );

private:
	RegularExpressionTerm *expression;
	};

//
//	Match if expression does not matches, but do not move end_pos
//
class RegularExpressionNegativeLookAhead: public RegularExpressionTerm
	{
public:
	RegularExpressionNegativeLookAhead( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_);
	virtual ~RegularExpressionNegativeLookAhead();

	bool matchTerm( int pos, int &end_pos );

private:
	RegularExpressionTerm *expression;
	};


//
//	Base class for all types of repeated regular expressions
//
class RegularExpressionRepeat: public RegularExpressionTerm
	{
public:
	RegularExpressionRepeat( SearchAdvancedAlgorithm &owner, int min_repeats, int max_repeats, RegularExpressionTerm *repeat_term );
	virtual ~RegularExpressionRepeat();

	virtual bool matchTerm( int pos, int &end_pos );
	virtual bool matchExpression( int pos, int &end_pos ) = 0;

protected:
	int min_repeats;
	int max_repeats;
	RegularExpressionTerm *repeat_term;
	};

//
//	Term that repeats its repeat_term most possible times
//
class RegularExpressionRepeatMost: public RegularExpressionRepeat
	{
public:
	RegularExpressionRepeatMost( SearchAdvancedAlgorithm &owner, int min_repeats, int max_repeats, RegularExpressionTerm *repeat_term );
	virtual ~RegularExpressionRepeatMost();

	virtual bool matchExpression( int pos, int &end_pos );
private:
	bool matchExpressionMost( int pos, int &end_pos, int matches );
	};

//
//	Term that repeats its repeat_term least possible times
//
class RegularExpressionRepeatLeast: public RegularExpressionRepeat
	{
public:
	RegularExpressionRepeatLeast( SearchAdvancedAlgorithm &owner, int min_repeats, int max_repeats, RegularExpressionTerm *repeat_term );
	virtual ~RegularExpressionRepeatLeast();

	virtual bool matchExpression( int pos, int &end_pos );
	};


//
//	A group of terms allowing alternatives
//
class RegularExpressionAlternation: public RegularExpressionTerm
	{
public:
	RegularExpressionAlternation( SearchAdvancedAlgorithm &owner );
	virtual ~RegularExpressionAlternation();

	void addAlternative( RegularExpressionTerm *term );

	virtual bool matchTerm( int pos, int &end_pos );

protected:
	std::list<RegularExpressionTerm *> alternative_expressions;
	};

//
//	A group that can be back referenced
//
class RegularExpressionGroup: public RegularExpressionTerm
	{
public:
	RegularExpressionGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_ );
	virtual ~RegularExpressionGroup();

	virtual bool matchTerm( int pos, int &end_pos );

	const Marker &groupStart() const;
	const Marker &groupEnd() const;
private:
	RegularExpressionTerm *expression;
	Marker start_mark;
	Marker end_mark;
	};

//
//	A named group that can be back referenced
//
class RegularExpressionNamedGroup : public RegularExpressionGroup
	{
public:
	RegularExpressionNamedGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_, const EmacsString &group_name_ );
	virtual ~RegularExpressionNamedGroup();
private:

	const EmacsString group_name;
	};

//
//	A numbered group that can be back referenced
//
class RegularExpressionNumberedGroup : public RegularExpressionGroup
	{
public:
	RegularExpressionNumberedGroup( SearchAdvancedAlgorithm &owner, RegularExpressionTerm *expression_, int group_number_ );
	virtual ~RegularExpressionNumberedGroup();
private:
	int group_number;
	};


//
//	regular expression Asserts
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
//	back reference
//
class RegularExpressionBackReference : public RegularExpressionTerm
	{
public:
	RegularExpressionBackReference( SearchAdvancedAlgorithm &owner, int back_ref_ );
	virtual ~RegularExpressionBackReference();

	bool matchTerm( int pos, int &end_pos );

private:
	int back_ref;
	};

