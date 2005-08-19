/*	Copyright (c) 1982-1995
 *	Barry A. Scott and nick Emery
 */

/* Buffer manipulation primitives */

#include <emacs.h>

/*forward*/ int check_tab_size( int value, struct variablename *v );
/*forward*/ int check_default_right_margin( int value, struct variablename *v );
/*forward*/ int check_default_left_margin( int value, struct variablename *v );
/*forward*/ int check_right_margin( int value, struct variablename *v );
/*forward*/ int check_left_margin( int value, struct variablename *v );
/*forward*/ void insert_at(int n, unsigned char c);
/*forward*/ void ins_cstr( unsigned char *s, int n );
/*forward*/ void del_frwd(int n, int k);
/*forward*/ void del_back(int n, int k);
/*forward*/ void gap_to(int n);
/*forward*/ int gap_room(int n);
/*forward*/ struct emacs_buffer * new_bf(unsigned char *name);
/*forward*/ struct emacs_buffer * find_bf(unsigned char *name);
/*forward*/ void set_bfp(struct emacs_buffer *p);
/*forward*/ void set_bfn(unsigned char *name);
/*forward*/ void scratch_bfn(unsigned char * name, int disp);
/*forward*/ void kill_bfn(unsigned char *name);
/*forward*/ void erase_bf(struct emacs_buffer *b);
/*forward*/ void init_bf(void);
/*forward*/ struct marker * new_mark(void);
/*forward*/ static void delink_mark(struct marker *m);
/*forward*/ void dest_mark(struct marker *m);
/*forward*/ void set_mark( struct marker *m, struct emacs_buffer *b, int p, int right );
/*forward*/ struct marker *copy_mark( struct marker *dst, struct marker *src );
/*forward*/ int to_mark(struct marker *m);

#if DBG_BUFFER
/*forward*/ static void check_bf(void);
/*forward*/ static void check_markers(unsigned char * s);
#endif

GLOBAL SAVRES VOLATILE unsigned char *bf_p1;	/* b_base-1 */
GLOBAL SAVRES VOLATILE unsigned char *bf_p2;	/* b_base+gap-1 (used to reference characters in the second part) */
GLOBAL SAVRES VOLATILE int bf_s1;		/* b_size1 */
GLOBAL SAVRES VOLATILE int bf_s2;		/* b_size2 */
GLOBAL SAVRES VOLATILE int bf_gap;		/* b_gap */
GLOBAL SAVRES VOLATILE int bf_modified;	/* b_modified */
GLOBAL SAVRES VOLATILE struct modespecific bf_mode;	/* b_mode */
GLOBAL SAVRES unsigned char bf_auto_fill_hook[MLISP_HOOK_SIZE];	/* b_AutoFillHook */
GLOBAL SAVRES unsigned char bf_process_key_hook[MLISP_HOOK_SIZE];	/* b_AutoFillHook */
GLOBAL SAVRES int bf_journalling;
GLOBAL SAVRES struct emacs_buffer *buffers;	/* root of the list of extant buffers */
GLOBAL SAVRES struct emacs_buffer *minibuf;	/* The minibuf */

/* The default values of several buffer-specific variables */
GLOBAL SAVRES unsigned char default_mode_format[MODELINE_SIZE];
GLOBAL SAVRES int default_replace_mode;
GLOBAL SAVRES int default_fold_case;
GLOBAL SAVRES int default_right_margin;
GLOBAL SAVRES int default_left_margin;
GLOBAL SAVRES int default_comment_column;
GLOBAL SAVRES int default_tab_size;
GLOBAL SAVRES int default_highlight;
GLOBAL SAVRES int default_display_nonprinting;
GLOBAL SAVRES int default_display_eof;
GLOBAL SAVRES int default_display_c1;
GLOBAL SAVRES int default_read_only_buffer;
GLOBAL SAVRES struct bound_name *default_auto_fill_proc;
static unsigned char *readonlyerror = u_str("Read-only buffer %s cannot be modified");



/* routines to check_ the range of values to be stored in System variables */
int check_tab_size
	(
	int value,
	struct variablename *v
	)
	{
	int rv;
	rv = 1 <= value && value <= 64;

	if( rv == 0 )
		error (u_str("%s should have a value between 1 and 64"),
			v->v_name);

	return rv;
	}

int check_default_right_margin
	(
	int value,
	struct variablename *v
	)
	{
	int rv;
	rv = value >= 1 && value > default_left_margin;

	if( rv == 0 )
		error (u_str("%s should be positive and greater than %d"),
				v->v_name, default_left_margin);

	return rv;
	}

int check_default_left_margin
	(
	int value,
	struct variablename *v
	)
	{
	int rv;
	rv = value >= 1 && value < default_right_margin;

	if( rv == 0 )
		error (u_str("%s should be positive and less than %d"),
				v->v_name, default_right_margin);

	return rv;
	}

int check_right_margin
	(
	int value,
	struct variablename *v
	)
	{
	int rv;
	rv = value >= 1 && value > bf_mode.md_leftmargin;

	if( rv == 0 )
		error (u_str("%s should be positive and greater than %d"),
				v->v_name, bf_mode.md_leftmargin);

	return rv;
	}

int check_left_margin
	(
	int value,
	struct variablename *v
	)
	{
	int rv;
	rv = value >= 1 && value < bf_mode.md_rightmargin;

	if( rv == 0 )
		error (u_str("%s should be positive and less than %d"),
				v->v_name, bf_mode.md_rightmargin);

	return rv;
	}

int buf_alloc_size_var;

int check_buffer_alloc_size
	(
	int value,
	struct variablename *PNOTUSED(v)
	)
	{
	if( value > bf_s1+bf_s2+bf_gap )
		gap_room( value - (bf_s1 + bf_s2) );

	return 0;
	}

void fetch_buffer_alloc_size( struct expression *e )
	{
	buf_alloc_size_var = e->exp_int = bf_s1+bf_s2+bf_gap;
	}

/* insert character c at position n in the current buffer */
void insert_at (int n, unsigned char c)
	{
	if( bf_mode.md_readonly )
		{
		error( readonlyerror, bf_cur->b_buf_name );
		return;
		}
	if( n != bf_s1 + 1 )
		gap_to( n );
	if( bf_gap < 1 )
		if( gap_room( 1 ) )
			return;
	bf_s1++;
	if( (bf_p1[ bf_s1 ] = c) == '\n' )
		cant_1line_opt = 1;
	bf_gap--;
	bf_p2--;

	record_insert( n, 1, &c );

	if( bf_modified == 0 )
		{
		redo_modes = 1;
		cant_1line_opt = 1;
		}
	if( bf_mode.md_syntax_array )
		syntax_insert_update( n, 1 );

	bf_modified++;
	}

/* Insert the N character string S at dot. */
void ins_cstr
	(
	unsigned char *s,
	int n
	)
	{
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	if( n == 0 )
		return;

	if( bf_mode.md_readonly )
		{
		error( readonlyerror, bf_cur->b_buf_name );
		return;
		}
	if( dot != bf_s1 + 1 )
		gap_to( dot );
	if( bf_gap < n )
		if( gap_room( n ) )
			return;

	record_insert( dot, n, s );

	/* point to first unsigned char in the gap */
	memcpy( &bf_p1[bf_s1 + 1], s, n );

	cant_1line_opt = 1;		/* assume that there was a \n in the string */

	bf_s1 = bf_s1 + n;
	bf_p2 = bf_p2 - n;
	bf_gap = bf_gap -n;

	if( bf_mode.md_syntax_array )
		syntax_insert_update( dot, n );

	dot_right( n );

#if DBG_BUFFER
	if( bf_gap < 0 )
		{
		error (u_str("Internal Emacs error ins_cstr gap overrun!"));
		invoke_debug();
		}
#endif
	if( bf_modified == 0 )
		{
		redo_modes = 1;
		cant_1line_opt = 1;
		}
	bf_modified++;

#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	}

/* delete k characters forward from position n in the current buffer */
void del_frwd (int n, int k)
	{
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	if( bf_mode.md_readonly )
		{
		error( readonlyerror, bf_cur->b_buf_name );
		return;
		}
	if( n != bf_s1 + 1 )
		gap_to( n );
	if( k > bf_s2 - bf_mode.md_tailclip )
		k = bf_s2 - bf_mode.md_tailclip;
	if( k > 0 )
		{
		bf_gap = bf_gap + k;
		record_delete( n, k );
		if( n != dot || k > 1 || char_at( n ) == '\n' )
			cant_1line_opt = 1;
		if( bf_modified == 0 )
			cant_1line_opt = 1;
		bf_modified++;
		bf_s2 = bf_s2 - k;
		bf_p2 = bf_p2 + k;
		if( bf_mode.md_syntax_array )
			syntax_delete_update( n, k );
		{		/* adjust markers */

		struct marker *m;
		int lim;
		lim = bf_s1 + bf_gap;
#if DBG_BUFFER 
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("del_frwd") );
#endif
		m = bf_cur->b_markset;
		while( m != 0 )
			{
			if( m->m_pos > bf_s1 && m->m_pos <= lim )
				{
				if( m->m_right )
					m->m_pos = bf_s1 + 1 + bf_gap;
				else
					m->m_pos = bf_s1 + 1;
				m->m_modified = 1;
				}
			m = m->m_next;
			}
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("del_frwd") );
#endif
		}
		}
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	}

/* delete k characters backward from position n in the current buffer */
void del_back(int n, int k)
	{
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	if( bf_mode.md_readonly )
		{
		error( readonlyerror, bf_cur->b_buf_name );
		return;
		}
	if( n != bf_s1 + 1 )
		gap_to( n );
	if( k > bf_s1 - bf_mode.md_headclip + 1 )
		k = bf_s1 - bf_mode.md_headclip + 1;
	if( k > 0 )
		{
		if( n != dot || k > 1 || char_at( n - 1 ) == '\n' )
			cant_1line_opt = 1;
		bf_gap = bf_gap + k;
		record_delete( n - k, k );
		if( bf_modified == 0 )
			cant_1line_opt = 1;
		bf_modified++;
		bf_p2 = bf_p2 + k;
		bf_s1 = bf_s1 - k;
		if( bf_mode.md_syntax_array )
			syntax_delete_update( n - k, k );
		{		/* adjust markers */

		struct marker *m;
		int lim;
		lim = bf_s1 + bf_gap;
		m = bf_cur->b_markset;
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("del_back") );
#endif
		while( m != 0 )
			{
			if( m->m_pos > bf_s1 && m->m_pos <= lim )
				{
				if( m->m_right )
					m->m_pos = bf_s1 + 1 + bf_gap;
				else
					m->m_pos = bf_s1 + 1;
				m->m_modified = 1;
				}
			m = m->m_next;
			}
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("del_back") );
#endif
		}
		}
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	}

/* move the gap to position n */
void gap_to(int n)
	{
	VOLATILE unsigned char *p1;
	VOLATILE unsigned char *p2;
	VOLATILE unsigned char *lim;
	int delt;
	int old_s1;
	struct marker *m;

#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	old_s1 = bf_s1;
	m = bf_cur->b_markset;
	if( n < 0 )
		n = 0;
	if( n > bf_s1 + bf_s2 )
		n = bf_s1 + bf_s2 + 1;
	if( n == bf_s1 + 1 )
		return;
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("gap_to") );
#endif

	if( n <= bf_s1 )
		{
		/* moving the gap left( into the first part )
		 *	p1 = bf_p1+1 + bf_s1 + bf_gap; */
		p2 = bf_p1 + 1 + bf_s1;
		p1 = p2 + bf_gap;
		lim = bf_p1 + n;
		delt = p2 - lim;
		memmove( p1 - delt, p2 - delt, delt );

		if( bf_cur->b_syntax.syntax_base != NULL )
			{
			/* moving the gap left( into the first part )
			 *	p1 = bf_p1+1 + bf_s1 + bf_gap; */
			p2 = bf_cur->b_syntax.syntax_base + 1 + bf_s1;
			p1 = p2 + bf_gap;
			memmove( p1 - delt, p2 - delt, delt );
			}

		bf_s1 = bf_s1 - delt;
		bf_s2 = bf_s2 + delt;
		while( m != 0 )
			{
			/* adjust markers */
			if( m->m_pos == bf_s1 + 1
			&& m->m_right )
				{
				m->m_pos = m->m_pos + bf_gap;
				m->m_modified = 1;
				}
			else if( m->m_pos > bf_s1 + 1
			&& m->m_pos <= old_s1 + 1 )
				{
				m->m_pos = m->m_pos + bf_gap;
				m->m_modified = 1;
				}
			m = m->m_next;
			}
		}
	else
		{
		/* moving the gap right( into the second part ) */
		p1 = bf_p1 + 1 + bf_s1;
		p2 = p1 + bf_gap;
		lim = bf_p2 + n;
		delt = p2 - lim;	/* delt lss 0 */
		if( delt >= 0 ) invoke_debug();
		memmove( p1, p2, -delt );

		if( bf_cur->b_syntax.syntax_base != NULL )
			{
			/* moving the gap right( into the second part ) */
			p1 = bf_cur->b_syntax.syntax_base + 1 + bf_s1;
			p2 = p1 + bf_gap;
			memmove( p1, p2, -delt );
			}

		bf_s1 = bf_s1 - delt;
		bf_s2 = bf_s2 + delt;
		while( m != 0 )
			{		/* adjust markers */
			if( m->m_pos >= old_s1 + 1 + bf_gap
			&& m->m_pos < bf_s1 + 1 + bf_gap )
				{
				m->m_pos = m->m_pos - bf_gap;
				m->m_modified = 1;
				}
			else if( m->m_pos == bf_s1 + 1 + bf_gap
			&& ! m->m_right )
				{
				m->m_pos = m->m_pos - bf_gap;
				m->m_modified = 1;
				}
			m = m->m_next;
			}
		}

#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		{
		check_markers( u_str("gap_to") );
		check_bf();
		}
#endif
	}

/* make sure that the gap in the current buffer is at least k characters wide */
int gap_room(int k)
	{
	struct emacs_buffer *b;
	unsigned char *p1;
	unsigned char *p2;
	unsigned char *lim;
	struct marker *m;
	int old_gap;

#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_bf();
#endif
	b = bf_cur;
	if( bf_gap >= k )
		return 0;
	old_gap = bf_gap;
	b->b_size = b->b_size + k + 2000;
	if( b->b_base != NULL )
		b->b_base = realloc_ustr( b->b_base, b->b_size );
	if( b->b_base == NULL )
		{
		bf_p1 = bf_p2 = NULL;
		b->b_size = b->b_gap = bf_gap = bf_s1 = bf_s2 = 0;
		error( u_str("Out of memory! Lost buffer %s"), b->b_buf_name );
		if( b->b_syntax.syntax_base )
			{
			free( b->b_syntax.syntax_base );
			b->b_syntax.syntax_base = NULL;
			}
		return 1;
		}
	if( b->b_syntax.syntax_base != NULL )
		{
		b->b_syntax.syntax_base = realloc_ustr( b->b_syntax.syntax_base, b->b_size + 1);
		/* I don't like the look of this! */
		/* b->b_syntax.syntax_valid = 0; */
		}

	bf_p1 = b->b_base- 1;
	p1 = b->b_base + b->b_size;
	lim = b->b_base + bf_s1 + bf_gap;
	p2 = lim + bf_s2;

	while( lim < p2 )
		{
		p1--; p2--;
		p1[0] = p2[0];
		}

	if( b->b_syntax.syntax_base != NULL )
		{
		p1 = b->b_syntax.syntax_base + b->b_size;
		lim = b->b_syntax.syntax_base + bf_s1 + bf_gap;
		p2 = lim + bf_s2;

		while( lim < p2 )
			{
			p1--; p2--;
			p1[0] = p2[0];
			}
		}

	bf_gap = bf_gap + (p1 - p2);
	bf_p2 = bf_p1 + bf_gap;

#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		check_markers( u_str("gap_room") );
#endif
	m = b->b_markset;
	while( m != 0 )
		{
		if( m->m_pos >= bf_s1 + old_gap	/* was gtr before right */ )
			{
			m->m_pos = m->m_pos + (bf_gap - old_gap);
			m->m_modified = 1;
			}
		 m = m->m_next;
		 }
#if DBG_BUFFER
	if( dbg_flags&DBG_BUFFER )
		{
		check_markers( u_str("gap_room") );
		check_bf();
		}
#endif
	return 0;
	}

/* create a buffer with the given name */
struct emacs_buffer * new_bf
	(
	unsigned char *name
	)
	{
	struct emacs_buffer *b;
	struct modespecific *mode;

	b = malloc_struct( emacs_buffer );
	b->b_buf_name = savestr( name );
	b->b_size = 2000;
	b->b_base = malloc_ustr( b->b_size );
	if( b->b_base == 0 )
		b->b_size = 0;
		/* out of memory -- give the error message when
		 * we try to enlarge the buffer */
	b->b_syntax.syntax_base = NULL;
	b->b_syntax.syntax_valid = 0;
	b->b_syntax.syntax_update_credit = 0;
	b->b_rendition_regions = NULL;
	b->b_line_valid = 0;
	b->b_line_numbers = NULL;
	b->b_fname = 0;
	b->b_kind = SCRATCHBUFFER;
	b->b_modified = 0;
	b->b_backed_up = 0;
	b->b_ephemeral_dot = 1;
	b->b_checkpointed = checkpoint_frequency ?  0 : -1;
	b->b_checkpointfn = 0;
	b->b_size1 = b->b_size2 = 0;
	b->b_gap = b->b_size;
	b->b_next = buffers;
	b->b_markset = 0;
	b->b_mark = 0;
	b->b_journalling = journalling_frequency != 0;
	b->b_journal = 0;
	b->b_rms_attribute = default_rms_attribute;
	b->b_file_time = 0;
	b->b_file_access = 0;
	b->b_synch_file_time = 0;
	b->b_synch_file_access = 0;
	mode = &b->b_mode;
	mode->md_keys = 0;
	_str_cpy( mode->md_modestring, u_str("Normal") );
	mode->md_prefixstring[0] = 0;
	mode->md_abbrev = 0;
	mode->md_syntax_colouring = default_syntax_colouring;
	mode->md_syntax_array = default_syntax_array || default_syntax_colouring;
 	mode->md_tailclip = 0;
	mode->md_headclip = 1;
	mode->md_syntax = &global_syntax_table;
	mode->md_wrap_lines = default_wrap_lines;
	mode->md_abbrevon = global_abbrev.abbrev_number_defined > 0;
	_str_cpy( mode->md_modeformat, default_mode_format );
	b->b_auto_fill_proc = default_auto_fill_proc;
	b->b_process_key_proc = 0;
	mode->md_foldcase = default_fold_case;
	mode->md_replace = default_replace_mode;
	mode->md_display_eof = default_display_eof;
	mode->md_display_c1 = default_display_c1;
	mode->md_readonly = default_read_only_buffer;
	mode->md_rightmargin = default_right_margin;
	mode->md_leftmargin = default_left_margin;
	mode->md_commentcolumn = default_comment_column;
	mode->md_tabsize = default_tab_size;
	mode->md_highlight = default_highlight;
	mode->md_displaynonprinting = default_display_nonprinting;
	buffers = b;

	buf_names[ n_buffers ] = b->b_buf_name;
	n_buffers++;
	buf_name_free--;
	if( buf_name_free <= 0 )
		{
		buf_names = (unsigned char **) realloc( buf_names, ((2 * n_buffers)+1) * sizeof( unsigned char * ), malloc_type_star_star);
		buf_name_free = n_buffers;
		}
	buf_names[ n_buffers ] = NULL;
	return b;
	}

/* find a buffer with the given name -- returns NULL if no such buffer exists */
struct emacs_buffer * find_bf
	(
	unsigned char *name
	)
	{
	struct emacs_buffer *b;
	b = buffers;
	while( b != 0 && _str_cmp( name, b->b_buf_name ) != 0 )
		b = b->b_next;
	return b;
	}

/* set the current buffer to p */
void set_bfp
	(
	struct emacs_buffer *p
	)
	{
	struct emacs_buffer *c;
	struct window *w;

	c = bf_cur;
	w = wn_cur;
	cant_1win_opt = 1;
	if( c != 0 )
		{
		if( w != 0 && c == w->w_buf )
			set_mark( w->w_dot, c, dot, 0 );
		c->b_size1 = bf_s1;
		if( c->b_modified != bf_modified )
			{
			c->b_modified = bf_modified;
			cant_1line_opt = 1;
			}
		c->b_size2 = bf_s2;
		c->b_gap = bf_gap;
		c->b_ephemeral_dot = dot;
		}
	bf_cur = p;
	bf_modified = p->b_modified;
	bf_mode = p->b_mode;
	bf_s1 = p->b_size1;
	bf_s2 = p->b_size2;
	bf_gap = p->b_gap;
	bf_p1 = p->b_base - 1;
	bf_p2 = bf_p1 + bf_gap;
	set_dot((( w != 0 && p == w->w_buf ) ?  to_mark( w->w_dot ) : p->b_ephemeral_dot) );
	}

/* set the current buffer to the one named */
void set_bfn( unsigned char *name )
	{
	struct emacs_buffer *p;

	if( name == 0 )
		return;
	p = find_bf( name );
	if( p == 0 )
		p = new_bf( name );
	set_bfp( p );
	}

void scratch_bfn(unsigned char * name, int disp)
	{
	struct emacs_buffer *p;
	if( name == 0 )
		return;
	p = find_bf( name );
	if( p == 0 )
		{
		p = new_bf( name );
		p->b_checkpointed = -1;	/* turn off checkpointing and */
		p->b_journalling = 0;		/* journalling */
		}
	set_bfp( p );

	if( disp ) window_on( bf_cur );
	widen_region();
	erase_bf( bf_cur );
	}

/* Kill a buffer and its contents etc */
void kill_bfn(unsigned char *name)
	{
	int i;
	struct emacs_buffer *b;
	struct emacs_buffer *p;
	struct emacs_buffer *q;
	struct marker *m;
	struct window *w;

	w = windows;
	if( (b = find_bf( name )) == 0 )
		return;

	/*
	 *	delete all the rendition regions
	 */
	{
	struct rendition_region *cur, *next;

	for( cur=b->b_rendition_regions; cur != NULL; cur = next )
		{
		next = cur->rr_next;

		dest_mark( cur->rr_start_mark );
		dest_mark( cur->rr_end_mark );

		free( cur );
		}

	b->b_rendition_regions = NULL;
	}

	b->b_journalling = 0;
	if( b->b_journal != 0 )
		journal_stop( b );
	if( b->b_base != 0 )
		free( b->b_base );
	if( b->b_fname != 0 )
		free( b->b_fname );
	if( b->b_checkpointfn != 0 )
		{
		if( unlink_checkpoint_files != 0 )
			fio_delete (b->b_checkpointfn);
		free( b->b_checkpointfn );
		}
	q = 0;
	p = buffers;
	while( p != 0 && p != b )
		{
		q = p;
		p = p->b_next;
		}
	if( q == 0 )
		buffers = b->b_next;
	else
		q->b_next = b->b_next;
	for( i=0; i<n_buffers; i++ )
		if( _str_cmp( name, buf_names[ i ] ) == 0 )
			{
			free( buf_names[i] );
			break;
			}
	for( ; i<n_buffers; i++ )
		buf_names[i] = buf_names[i + 1];
	n_buffers--;
	buf_names[ n_buffers ] = 0;
	buf_name_free++;

	/* kill of any marker in the buffer */
	if( b->b_mark != 0 )
		dest_mark( b->b_mark );

	/* make all marker in the marker set NULL */
	m = b->b_markset;
	while( m != 0 )
		{
		m->m_buf = 0;
		m = m->m_next;
		}
	while( w != 0 )
		if( w->w_buf == b )
			{
			struct window *nxt;
			nxt = w->w_next;
			del_win( w );
			w = nxt;
			}
		else
			w = w->w_next;

	if( bf_cur == b )
		set_bfp( wn_cur->w_buf );

	free( b );
	}

/* Erase the contents of a buffer */
void erase_bf( struct emacs_buffer *b )
	{
	struct emacs_buffer *old;
	old = bf_cur;

	set_bfp( b );

	/*
	 *	delete all the rendition regions
	 */
	{
	struct rendition_region *cur, *next;

	for( cur=b->b_rendition_regions; cur != NULL; cur = next )
		{
		next = cur->rr_next;

		dest_mark( cur->rr_start_mark );
		dest_mark( cur->rr_end_mark );

		free( cur );
		}

	b->b_rendition_regions = NULL;
	}

	/*
	 *	Erasing a non file buffer kills off the journal.
	 *	But as modifing the buffer writes a new
	 *	journal journalling has to be disable over
	 *	next few lines of code.
	 */
	del_frwd( first_character, num_characters - first_character + 1 );
	set_dot( first_character );
	cant_1line_opt = 1;
	if( bf_s1 + bf_s2 == 0 )
		{
		bf_modified = 0;
		if( bf_cur->b_kind != FILEBUFFER )
			journal_stop( b );
		bf_cur->b_syntax.syntax_valid = 0;
		}
	set_bfp( old );
	}

/* initialize the buffer routines */
void init_bf( void )
	{
	buf_name_free = 300;
	buf_names = (unsigned char **)malloc( (buf_name_free+1) * sizeof(unsigned char *), malloc_type_star_star);
	n_buffers = 0;
	_str_cpy( default_mode_format,
		u_str(" %[%hBuffer: %b%* File: %f %M(%m%c%j%r%a%R) %p%]") );
	default_tab_size = 8;
	default_fold_case = 0;
	default_right_margin = 10000;
	default_left_margin = 1;
	default_comment_column = 33;
	default_replace_mode = 0;
	set_bfn( u_str("Minibuf") );
	minibuf = bf_cur;
	set_bfn( u_str("main") );
	bf_cur->b_kind = FILEBUFFER;
	}

/* save a string in managed memory */
unsigned char * savestr
	(
	unsigned char *s
	)
	{
	unsigned char *ret;
	int size;
	size = (( s != 0 ) ?  _str_len( s ) : 0) + 1;
	ret = malloc_ustr( size );
	if( s != 0 )
		_str_cpy( ret, s );
	else
		ret[0] = '\0';
	return ret;
	}



/* Marker routines */

/* create a new marker */
struct marker *new_mark( void )
	{
	struct marker *m;
	m = malloc_struct( marker );
	m->m_buf = 0;
	m->m_pos = 0;
	m->m_modified = 0;
	m->m_right = 0;
	m->m_next = 0;
	m->m_prev = 0;
	return m;
	}

/* delink a marker from a list of markers */
static void delink_mark
	(
	struct marker *m
	)
	{
	if( m == 0 || m->m_buf == 0 )
		return;
	if( m->m_prev != 0 )
		m->m_prev->m_next = m->m_next;
	else
		if( m->m_buf != 0 )
			m->m_buf->b_markset = m->m_next;
	if( m->m_next != 0 )
		m->m_next->m_prev = m->m_prev;
	m->m_next = 0;
	m->m_prev = 0;
	m->m_buf = 0;
	}

/* destroy a marker */
void dest_mark
	(
	struct marker *m
	)
	{
	if( m == 0 )
		return;
	delink_mark( m );
	free( m );
	}

/* set marker min buffer b at position p */
void set_mark
	(
	struct marker *m,
	struct emacs_buffer *b,
	int p,
	int right
	)
	{
	if( m == 0 )
		{
		error( u_str("Uninitialized marker!") );
		return;
		}
	delink_mark( m );
	if( b == 0 )
		{
		error( u_str("Attempt to set_mark on a NULL buffer!") );
		return;
		}
	/*
	 *	The call to error has been removed. It seems to only get called
	 *	after the following type of code.
	 *	(save-excursion (delete-buffer "bug") (pop-to-buffer "bug"))
	 */
	if( p < 1 )		/* error(u_str("Bogus set_mark to %d"), p), */
		p = 1;
	m->m_buf = b;
	m->m_modified = 0;
	m->m_right = right;
	m->m_next = b->b_markset;
	m->m_prev = 0;
	if( m->m_next != 0 )
		m->m_next->m_prev = m;
	b->b_markset = m;
	m->m_modified = 0;
	if( b == bf_cur )
		{
		if( p < bf_s1 + 1
		||	(! right && p == bf_s1 + 1) )
			m->m_pos = p;
		else
			m->m_pos = p + bf_gap;
		}
	else
		{
		if( p < b->b_size1 + 1
		||	(! right && p == b->b_size1 + 1) )
			m->m_pos = p;
		else
			m->m_pos = p + b->b_gap;
		}
	}

/* copy the value of the source marker to the destination, handling all the
 * nasty linking and delinking */
struct marker * copy_mark
	(
	struct marker *dst,
	struct marker *src
	)
	{
	set_mark( dst, src->m_buf, 1, 0 );
	dst->m_pos = src->m_pos;
	dst->m_right = src->m_right;
	return dst;
	}

/* set bf_cur to the buffer indicated by the given marker and return
 * the position( "dot", value ) within that buffer; returns 0 iff the
 * marker wasn't set. */
int to_mark
	(
	struct marker *m
	)
	{
	if( m == 0 || m->m_buf == 0 )
		return 0;
	if( bf_cur != m->m_buf )
		set_bfp( m->m_buf );
	if( m->m_pos > bf_s1 + 1 )
		return m->m_pos - bf_gap;
	else
		return m->m_pos;
	}

/* Returns the number of the current line. Lines number from 1. */
int current_line_number( void )
	{
	int line_num;
	int n;

	line_num = 1;
	for( n=1; n<=dot - 1; n += 1 )
		if( char_at (n) == '\n' )
			line_num++;
	return line_num;
	}

#if DBG_BUFFER
static void check_bf( void )
	{
	/*
	 *	check_ the BF set of variables
	 */
	if( bf_p1 + bf_gap != bf_p2 )
		invoke_debug();
#if DBG_ALLOC_CHECK
	emacs_heap_check();
#endif
	}

static void check_markers(unsigned char * s)
	{
	return;
/*
	struct marker *m;

	_dbg_msg( u_str("%s Buffer %s S1=%d GAP=%d, S2=%d\n\r Marker Position Right"),
		s, bf_cur->b_buf_name, bf_s1, bf_gap, bf_s2 );
	m = bf_cur->b_markset;
	while( m != 0 )
		{
		_dbg_msg( u_str(" %.6x %.4d/%.4d %d"),
			m, m->m_pos, m->m_pos - bf_gap, m->m_right);
		m = m->m_next;
		}
*/
	}
#endif
