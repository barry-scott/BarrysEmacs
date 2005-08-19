/*** MODULE emacs_search ***/
/*			Copyright (c) 1985-1990 */
/*		Barry A. Scott and Nick Emery */
/* Declarations having to do with Emacs searching */
#define ESIZE 2000	/* the maximum size of an RE */
#define MAX_NBRA 32	/* the maximum number of meta-brackets in an */
/* RE -- \( \) */
#define MAX_NALTS 32	/* the maximum number of \|'s */
struct search_globals {
	unsigned char sea_expbuf [ESIZE+4];		/* The most recently compiled search string */
	unsigned char *sea_alternatives [MAX_NALTS];	/* The list of \| seperated alternatives */
	struct marker *sea_bra_slist [MAX_NBRA];	/* RE meta-bracket start list */
	struct marker *sea_bra_elist [MAX_NBRA];	/* RE meta-bracket end list */
	int sea_loc1;		/* The buffer position of the first */
				/* character of the most recently found string */
	int sea_loc2;		/* The buffer position of the character */
				/* following the most recently found string */
	unsigned char sea_nbra;	/* The number of meta-brackets int the most */
				/* recently compiled RE */
	unsigned char *sea_trt;	/* The current translation table */
	};

/* meta characters in the "compiled" form of a regular expression */
#define CBRA	2u	/* \( -- begin bracket */
#define CCHR	4u	/* a vanilla character */
#define CDOT	6u	/* . -- match anything except a newline */
#define CCL	8u	/* [...] -- character class */
#define NCCL	10u	/* [^...] -- negated character class */
#define CDOL	12u	/* $ -- matches the end of a line */
#define CEOP	14u	/* The end of the pattern */
#define CKET	16u	/* \) -- close bracket */
#define CBACK	18u	/* \N -- backreference to the Nth bracketed string */
#define CIRC	20u	/* ^ matches the beginning of a line */
#define BBUF	22u	/* beginning of buffer \` */
#define EBUF	24u	/* end of buffer \' */
#define BDOT	26u	/* matches before dot \< */
#define EDOT	28u	/* matches at dot \= */
#define ADOT	30u	/* matches after dot \> */
#define WORDC	32u	/* matches word character \w */
#define NWORDC	34u	/* matches non-word characer \W */
#define WBOUND	36u	/* matches word boundary \b */
#define NWBOUND	38u	/* matches non-(word boundary) \B */

/* the following are only allowed when syntax_array is used */
#define SEA_SYN_COMMENT 40u /* only match inside a comment \c */
#define SEA_SYN_NOCOMMENT 42u /* only match outside a comment \C */
#define SEA_SYN_STRING 44u /* only match inside a string \s */
#define SEA_SYN_NOSTRING 46u /* only match outside a string \S */

#define STAR	1u	/* * -- Kleene star, repeats the previous */
			/* REas many times as possible; the value */
			/* ORs with the other operator types */
