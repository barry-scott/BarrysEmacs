/*	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/* structure that defines a marker
 *   A marker is conceptually a (buffer,position) pair.  m_buf
 *   indicates which buffer is marked, and m_pos indicates which
 *   position is marked.  All markers for a particular buffer are
 *   chained together by m_next, rooted at b_markset.  The
 *   interpretation if m_pos is rather odd: it is the index from the
 *   beginning of the allocated area (b_base) of the marked position
 *   -- it is not the character number of the marked position.  This
 *   interpretation causes markers to be invariant over insertions and
 *   deletions, the only things that affect them are gap motions,
 *   which should be far less frequent.
 */
struct marker
	{
	struct emacs_buffer *m_buf;	/*  0 the buffer that this marker refers to */
	int m_pos;			/*  4 the position in the buffer of the character referred to */
	struct marker *m_next;		/*  8 the next marker that is chained to the same buffer */
	struct marker *m_prev;		/* 0c back pointer in marker chain */
	unsigned m_right : 1;		/* 10 true iff this is a right handed marker */
	unsigned m_modified : 1;	/* 10 true iff this marker has been modified since it was set */
	};

#define MODESTRING_SIZE 80
#define MODELINE_SIZE 256               /* same as MScreenWidth rounded to longwords */
struct modespecific
	{
	struct abbrevtable *md_abbrev;	/* the abbrev table in use in this buffer */
	struct syntax_table *md_syntax;	/* the syntax table in use in this buffer */
	int md_rightmargin;		/* Right margin for auto-newline */
	int md_leftmargin;		/* Left margin for auto-newline */
	int md_commentcolumn;		/* Comment column for auto-newline */
	int md_tabsize;			/* Size of one tab stop. 8 is the default */
	int md_headclip;		/* The number of characters clipped off the head of the buffer by restrict-region +1 */
	int md_tailclip;		/* The number of characters clipped off the tail of the buffer by restrict-region */
	struct keymap *md_keys;		/* Keys that are bound local to this buffer (stuff like $J) */
	int md_abbrevon;		/* true iff abbrev mode has been enabled for this buffer */
	int md_foldcase;		/* true iff case folded comparisons are to be done */
	int md_replace;			/* true iff we replace instead of insert characters */
	int md_highlight;		/* true if this buffers region should be highlighted */
	int md_displaynonprinting;	/* true if non printing chars are to shown up as printable chars */
	int md_display_eof;		/* true if the EOF diamond is to be shown */
	int md_display_c1;		/* true if the C1 characters in the buffer are to be show as graphics */
	int md_readonly;		/* true if the buffer is read-only */
	int md_wrap_lines;		/* true if long lines should be wrapped for this buffer */
	int md_syntax_colouring;	/* true if syntax colouring is enabled */
	int md_syntax_array;		/* true if the sytax array methods are to be used */
	unsigned char md_modestring [MODESTRING_SIZE];	/* The commentary string that appears in the modeline of each window */
	unsigned char md_modeformat [MODELINE_SIZE];	/* The format of the mode line for this buffer */
	unsigned char md_prefixstring [20];		/* The auto-newline prefix string */
	};

	/* the "kinds" of stuff that can be in a buffer */
enum buffer_types
	{
	FILEBUFFER,		/* Contains info from a file
				 * (WriteModifiedFiles will dump it) */
	SCRATCHBUFFER,		/* Scratch stuff -- automatically generated
				 * by emacs for stuff like ^X^B */
	MACROBUFFER		/* contains the body of a macro, in which
				 * case the file name is actually the macro name */
	};

struct line_number
	{
	int position;
	int line;
	};

struct line_numbers
	{
	int num_entries;
	struct line_number line_data[1];
	};

struct rendition_region
	{
	struct rendition_region *rr_next;
	struct marker *rr_start_mark, *rr_end_mark;
	int rr_start_pos, rr_end_pos;
	int rr_colour;
	};

struct emacs_buffer
	{
	/* An Emacs buffer is maintained as a single block of storage that
	 * contains all of the text involved (eg. the entire contents of a
	 * file, we're depending on the paging system to do a lot of work
	 * for us).  This block is divided into two parts, which when
	 * concatenated form one long text string.  The gap in the middle is
	 * to allow insertions and deletions to be performed without
	 * repeated copying of the entire buffer contents.  "dot" will not
	 * necessarily be positioned at the gap, but if any insertions or
	 * deletions are to be done around "dot" then the gap must be moved.
	 *
	 * |<---------------size---------------------------------->|
	 * |<---b_size1--->|<---b_gap--->|<---b_size2--->|
	 * ^--b_base
	 */
	unsigned char *b_base;		/*  0 points to the beginning of the */
					/*    block of storage used to hold the */
					/*    text in the buffer */
	struct syntax_buffer_data b_syntax;
	int b_line_valid;		/*  c The highwater mark for the line number
					 *    calculations */
	struct line_numbers *b_line_numbers;
	struct rendition_region *b_rendition_regions;
	unsigned char *b_buf_name;	/*  4 the name of this buffer */
	int b_size;			/*  8 the number of characters in the */
					/*    block pointed to by b_base.  Not */
					/*    all of the characters in the */
					/*    block may be valid */
	int b_size1;			/*  c the number of characters in the */
					/*    first part of the block */
	int b_gap;			/* 10 the number of characters in the */
					/*    gap between the two parts */
	int b_size2;			/* 14 the number of characters in the */
					/*    second part of the block */
	int b_ephemeral_dot;		/* 18 The value that dot had the last time that */
					/*    this buffer was visible in a window or */
					/*    accessed in any way */
	unsigned char *b_fname;		/* 1c the name of the file associated */
					/*    with this buffer */
	int b_modified;			/* 20 true iff this buffer has been */
					/*    modified since it was last */
					/*    written */
	int b_backed_up;		/* 24 true iff this buffer has been been backed */
					/*    up (if you write to its associated file */
					/*    and it hasn't been backed up, then a */
					/*    backup will be made) */
	int b_checkpointed;		/* 28 the value of b_modified at the last */
					/*    check_point.  Since b_modified is actually */
					/*    a count of the number of changes made */
					/*    (which gets zeroed when the file is */
					/*    written), deciding whether or not to */
					/*    check_point is done on the basis of the */
					/*    difference between b_modified and */
					/*    b_checkpointed */
	unsigned char *b_checkpointfn;	/* 2c file name used for check_pointing this buffer */
	struct emacs_buffer *b_next;	/* 30 the next buffer in the chain of */
					/*    extant buffers */
	struct marker *b_markset;	/* 34 the markers that refer to this buffer */
	struct marker *b_mark;		/* 38 The distinguished mark (set by */
					/*    ^@) for this buffer */
	enum buffer_types b_kind;	/* 3c The kind of thing in this buffer */
	struct modespecific b_mode;	/* 40 The mode specific information for this buffer */
	struct bound_name *b_auto_fill_proc;/* The command that will be executed */
					/* when the right margin is passed */
	struct bound_name *b_process_key_proc;	/* Process key proc */
	int b_rms_attribute;		/* the RMS attributes of the file in this buffer */
	time_t b_file_time;		/* the modification time of the file */
	time_t b_synch_file_time;	/* the modification time of the file at last synch */
	int b_file_access;		/* file access */
	int b_synch_file_access;	/* synch file access */
	struct journal_block *b_journal;
	int b_journalling : 1;		/* True if journalling */
	};
