/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and nick Emery                               */
/* Declarations having to do with Emacs syntax tables                       */
/* A syntax table contains an array of information, one entry per ASCII     */
/* character.                                                               */

#define	char_is( ch, prop ) ((bf_mode.md_syntax->s_table[(ch)].s_kind & (prop)) == (prop))

#ifdef _DEBUG
extern unsigned char *_debug_ref_syntax_at( int n, unsigned char *syn_p1, unsigned char *syn_p2 );
#define syntax_at( n ) *_debug_ref_syntax_at( (n), syn_p1, syn_p2 )
#define	ref_syntax_at( n ) _debug_ref_syntax_at( (n), syn_p1, syn_p2 )
#else
#define syntax_at( n ) ((unsigned char)( (n) > bf_s1 ? syn_p2[(n)] : syn_p1[(n)]))
#define	ref_syntax_at( n ) ((unsigned char *)( (n) > bf_s1 ? &syn_p2[(n)] : &syn_p1[(n)]))
#endif

#define SYNTAX_DULL		0		/* a dull (punctuation) character   */
#define SYNTAX_WORD		1		/* a word character for ESC-F and friends */
#define SYNTAX_PAIRED_QUOTE	2		/* like " or ' in C                 */
#define SYNTAX_STRING SYNTAX_PAIRED_QUOTE	/* its a string */
#define SYNTAX_COMMENT		4		/* its a comment */
#define	SYNTAX_KEYWORD1		8
#define	SYNTAX_KEYWORD2		16
#define	SYNTAX_FIRST_FREE	32
#define SYNTAX_PREFIX_QUOTE	32		/* like \ in C                      */
#define SYNTAX_BEGIN_PAREN	64		/* a begin paren: (<[               */
#define SYNTAX_END_PAREN	128		/* an end paren: )>]	end         */
#define	SYNTAX_LAST_BIT		128

/*
 *	syntax_string contains the following information for
 *	the various kinds of syntax.
 *	Kind		Main		Match
 *	begin Paren	"("		")"
 *	end paren	")"		"("
 *	keyword[12]	"keyword"	""
 *	comment		"//"		"\n"
 */
#define	SYNTAX_STRING_SIZE	31
struct syntax_string
	{
	int s_kind;
	struct syntax_string *s_next;

	int s_main_len;
	unsigned char s_main_str[SYNTAX_STRING_SIZE+1];

	int s_match_len;
	unsigned char s_match_str[SYNTAX_STRING_SIZE+1];
	};


struct syntax_table_entry
	{
	int s_kind;
	struct syntax_string *s_strings;
	};

struct syntax_table
	{
	struct syntax_table_entry s_table [256];
	unsigned char *s_name;
	};

struct syntax_buffer_data
	{
	unsigned char *syntax_base;	/*  points to the beginning of the syntax
					 *  data for the buffer
					 *  maps 1-to-1 on the b_base array */
	int syntax_valid;		/*  The number of valid positions in the
					 *  syntax array */
	int syntax_update_credit;	/*  number of times the array may be updated */
	};
