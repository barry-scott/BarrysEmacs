//
//	search.h
//
//	Copyright (c) 1985-2002 BArry A. Scott
//
//	Declarations having to do with Emacs searching 
//

//
// meta characters in the "compiled" form of a regular expression 
// Thoses marked [R] have a min, max range max of 0 means unlimited
//
enum search_meta_chars
	{
	CEOP = 0,		// [-] The end of the pattern 
	CCHR,			// [R] a vanilla character 
	CDOT,			// [R] . -- match anything except a newline 
	CCL,			// [R] [...] -- character class 
	NCCL,			// [R] [^...] -- negated character class 
	CBACK,			// [R] \N -- backreference to the Nth bracketed string 
	WORDC,			// [R] matches word character \w 
	NWORDC,			// [R] matches non-word characer \W 

	CBRA,			// [-] \( -- begin bracket 
	CDOL,			// [-] $ -- matches the end of a line 
	CKET,			// [-] \) -- close bracket 
	CIRC,			// [-] ^ matches the beginning of a line 
	BBUF,			// [-] beginning of buffer \` 
	EBUF,			// [-] end of buffer \' 
	BDOT,			// [-] matches before dot \< 
	EDOT,			// [-] matches at dot \= 
	ADOT,			// [-] matches after dot \> 
	WBOUND,			// [-] matches word boundary \b 
	NWBOUND,		// [-] matches non-(word boundary) \B 

	// the following are only allowed when syntax_array is used 
	SEA_SYN_COMMENT,	// [-] only match inside a comment \c 
	SEA_SYN_NOCOMMENT,	// [-] only match outside a comment \C 
	SEA_SYN_STRING,		// [-] only match inside a string \s 
	SEA_SYN_NOSTRING	// [-] only match outside a string \S 
	};

class SearchGlobals : public EmacsObject
	{
public:
	enum sea_type
		{
		sea_type__string = 0,
		sea_type__RE_simple,
		sea_type__RE_advanced
		};
	EMACS_OBJECT_FUNCTIONS( SearchGlobals )
	SearchGlobals();
	SearchGlobals( const SearchGlobals &src );
	virtual ~SearchGlobals();
	SearchGlobals &operator=(const SearchGlobals &src);

	void compile( const EmacsString &strp, sea_type RE );
	void compile_string( const EmacsString &strp );
	void compile_RE_simple( const EmacsString &strp );
	void compile_RE_advanced( const EmacsString &strp );

	int search( const EmacsString &s, int n, int dot, sea_type RE );
	void place(int l1, int l2);
	int perform_replace(int query, sea_type RE);
	void search_replace_once( const EmacsString &new_string, sea_type RE );

	int search_execute(int fflag, int addr);
	int search_advance( int lp, unsigned char *ep, int syn_include, int syn_exclude );

	int backref(int i, int lp);
	bool cclass( unsigned char *char_set, int c );

	typedef unsigned char re_expr_cell_t;
	enum { re_expr_cell_max = 255 };
private:
	re_expr_cell_t *compiler_expr_next;	// pointer into sea_expbuf
	re_expr_cell_t *compiler_last_repeatable;// pointer into sea_expbuf of last repeatable expr
	int compiler_cur_alt;

	enum compile_state_t { single_re, repeatable_re, repeated_re, greed_re };
	compile_state_t compiler_state;

	void compiler_begin_compile();		// called to init the compiler
	void compiler_new_alternative();
	void compiler_add_single( search_meta_chars type );
	void compiler_add_repeatable( search_meta_chars type );
	void compiler_add_cchr( re_expr_cell_t ch );
	void compiler_add_data( re_expr_cell_t data );
	void compiler_add_data( search_meta_chars data );
	void compiler_add_data( const char *data );
	void compiler_add_data( unsigned int data );
	void compiler_set_min_max( unsigned int repeat_min, unsigned int repeat_max );
	void compiler_end_compile();		// called when all the expr has been compiled
	bool compiler_at_start();		// true if at the start of an expression
	void compiler_add_ccl( search_meta_chars type ); // add CCL or NCCL
	void compiler_ccl_complete();		// fill in length

public:
	enum { ESIZE = 65536 };			// the maximum size of an RE 
	enum { MAX_NBRA = 64 };			// the maximum number of meta-brackets in an RE -- \( \) 
	enum { MAX_NALTS = 64 };		// the maximum number of \|'s 
	re_expr_cell_t sea_expbuf[ESIZE+4];	// The most recently compiled search string 
	re_expr_cell_t *sea_alternatives[MAX_NALTS+1];	// The list of \| seperated alternatives 
	Marker sea_bra_slist [MAX_NBRA];	// RE meta-bracket start list 
	Marker sea_bra_elist [MAX_NBRA];	// RE meta-bracket end list 
	int sea_loc1;				// The buffer position of the first character of the most recently found string 
	int sea_loc2;				// The buffer position of the character following the most recently found string 
	unsigned int sea_nbra;				// The number of meta-brackets int the most recently compiled RE 
	unsigned char *sea_trt;			// The current translation table

	static unsigned char standard_trt[256];	// the identity TRT 
	static unsigned char case_fold_trt[256];// folds upper to lower case 
	};

class CompilerError
	{
public:
	CompilerError( const char *_msg );
	~CompilerError();

	const char *msg;
	};

class CompilerStringIterator
	{
public:
	CompilerStringIterator( const EmacsString &str );
	~CompilerStringIterator();

	bool atEnd() const;		// true if at the end of the string
	bool beforeEnd() const;		// true if before the end of the string
	bool equalTo( const EmacsString &str ); // true if the string contains str next
	SearchGlobals::re_expr_cell_t nextChar();	// return the next char
private:
	EmacsString str;		// the string
	int pos;			// position to return next char from
	};
