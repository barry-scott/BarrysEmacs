/*	Copyright (c) 1982-1993
 *	Barry A. Scott and nick Emery
 */

/* Support routines for the undo facility */

#include <emacs.h>

/*forward*/ static struct undorec *new_undo(int kind, int dot, int len);
/*forward*/ void record_insert(int dot, int n, unsigned char *s);
/*forward*/ void record_delete(int dot, int n);
/*forward*/ int done_is_done( void );
/*forward*/ int undo_boundary( void );
/*forward*/ int undo_command( void );
/*forward*/ int undo_more( void );
/*forward*/ void undo_de_ref_buf( struct emacs_buffer *b );
/*forward*/ void init_undo( void );

/* The undo records */
static struct undorec undo_rq[UNDO_MAX_REC+1];

/* And the characters associated with them */
static unsigned char undo_cq[UNDO_MAX_CHAR+1];
static int fill_rq;
static int fill_cq;
static int n_undone;
static int n_chars_left;
static int last_undone_c;
static struct undorec *last_undone;
static struct undorec *last_undo_rec;

static struct undorec *new_undo (int kind, int dot, int len)
	{
	struct undorec *p;
	struct undorec *np;
	p = &undo_rq[ fill_rq ];
	fill_rq = (fill_rq + 1) % UNDO_MAX_REC;
	np = &undo_rq[fill_rq];
	np->undo_kind = UNDO_UNDOABLE;
	p->undo_kind = kind;
	p->undo_buffer = bf_cur;
	p->undo_dot = dot;
	p->undo_len = len;
	p->undo_modified = bf_modified;
	last_undo_rec = p;
	if( kind != UNDO_BOUNDARY )
		last_undone = 0;
	return p;
	}

void record_insert(int dot, int n, unsigned char *s)
	{
	if( bf_cur->b_journalling )
		journal_insert(dot, n, s);
	new_undo( UNDO_DELETE, dot, n);
	}

void record_delete(int dot, int n)
	{
	int i;

	if( bf_cur->b_journalling )
		journal_delete(dot, n);
	new_undo( UNDO_INSERT, dot, n );
	n_chars_left = n_chars_left - n;

	for( i=1; i<=n; i += 1 )
		{
		undo_cq[ fill_cq ] = char_at( dot );
		fill_cq = (fill_cq + 1) % UNDO_MAX_CHAR;
		dot++;
		}
	}

int done_is_done( void )
	{
	new_undo( UNDO_UNDOABLE, dot, 0 );
	return 0;
	}

int undo_boundary( void )
	{
	new_undo( UNDO_BOUNDARY, dot, 0 );
	return 0;
	}

int undo_command( void )
	{
	arg++;
	last_undone = last_undo_rec;
	n_chars_left = UNDO_MAX_CHAR;
	n_undone = 0;
	last_undone_c = fill_cq;
	return undo_more();
	}

int undo_more( void )
	{
	struct undorec *p;
	int n;
	int chars;
	int i;

	p = last_undone;
	n = 0;
	if( p == 0 )
		{
		error (u_str("Cannot undo more: changes have been made since the last undo"));
		return 0;
		}
	for(;;)
		{
		while( p->undo_kind != UNDO_BOUNDARY )
			{
			if( (p->undo_kind == UNDO_INSERT
				&& (n_chars_left = n_chars_left - p->undo_len) < 0)
			|| p->undo_kind == UNDO_UNDOABLE
			|| n_undone >= UNDO_MAX_REC )
				{
				error (u_str("Sorry, cannot undo that. What is done is done."));
				return 0;
				}
			n_undone++;
			n++;
			p--;
			if( p < &undo_rq[ 0 ] )
				p = &undo_rq[ UNDO_MAX_REC - 1 ];
			}
		n_undone++;
		n++;
		arg--;
		if( arg <= 0 )
			break;
		p--;
		if( p < &undo_rq[0] )
			p = &undo_rq[ UNDO_MAX_REC - 1 ];
		}
	p = last_undone;
	chars = last_undone_c;
	for( i=1; i<=n; i += 1 )
		{
		if( p->undo_buffer != 0 )
			{
			if( bf_cur != p->undo_buffer )
				set_bfp( p->undo_buffer );
			set_dot( p->undo_dot );
			switch( p->undo_kind )
			{
			case UNDO_BOUNDARY: break;
			case UNDO_DELETE:
				del_frwd( dot, p->undo_len );
				break;
			case UNDO_INSERT:
				{
				int len;
				len = p->undo_len;
				chars = chars -len;
				if( chars < 0 )
					{
					ins_cstr( &undo_cq[0], len + chars);
					len = -chars;
					chars = chars + UNDO_MAX_CHAR;
					}
				ins_cstr( &undo_cq[ chars ], len );
				break;
				}
			default:
				error (u_str("Something rotten in undo undo_kind is 0x%x"), p->undo_kind);
				return 0;
			}
			bf_modified = p->undo_modified;
			}
		p--;
		if( p < &undo_rq[ 0 ] )
			p = &undo_rq[ UNDO_MAX_REC - 1 ];
		}
	last_undone = p;
	last_undone_c = chars;
	return 0;
	}
void undo_de_ref_buf
	(
	struct emacs_buffer *b
	)
	{
	int i;
	for( i=0; i<=UNDO_MAX_REC - 1; i += 1 )
		if( undo_rq[i].undo_buffer == b )
			undo_rq[i].undo_buffer = 0;
	}
void init_undo( void )
	{
	done_is_done();
	}
/*end
 *eludom */
