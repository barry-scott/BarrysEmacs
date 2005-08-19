/*	Copyright (c) 1982-1991                                             */
/*		Barry A. Scott and nick Emery                               */

#define MSCREENWIDTH 255
#define MSCREENLENGTH 100

/*  If highlighted is true then mode line highlighting is done              */
/*  If highlight_start is greater then zero then apply region               */
/*  highlight to the characters between hightlight_start and end.           */
#define LINE_ATTR_MODELINE		SYNTAX_FIRST_FREE
#define	LINE_ATTR_USER			(SYNTAX_FIRST_FREE<<1)

#define LINE_M_ATTR_HIGHLIGHT		SYNTAX_LAST_BIT
#define LINE_M_ATTR_USER		((SYNTAX_FIRST_FREE<<1)|((SYNTAX_FIRST_FREE<<1)-1)) 

struct emacs_line 
	{
	/* a line as it appears in a list of                                        */
	/* lines (as in the physical and virtual display lists)                     */
	int line_hash;               /* hash value for this line, 0 if not known  */
	struct emacs_line *line_next;     /* pointer to the next line in a list of lines  */
	int line_drawcost;          /* the cost of redrawing this line  */
	int line_length;            /* the number of valid characters in the line  */
	unsigned char line_body [MSCREENWIDTH];	/* the actual text of the line      */
	unsigned char line_attr [MSCREENWIDTH];	/* the actual attributes of the text */
	};

struct emacs_msquare
	{
	int msq_cost;
	int msq_fromi;
	int msq_fromj;
	};
