/*module macros
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and Nick Emery */

/* Stuff to do with the manipulation of macros.
 * For silly historical reasons, several routines that should be here
 * are actually in options. (eg. the command level callers). */

#include <emacs.h>


/*forward*/ int find_mac( unsigned char *s );
/*forward*/ void def_mac( unsigned char *s, void *body, int isMLisp, int len, struct bound_name *x );
/*forward*/ int edit_macro( void );
/*forward*/ int define_buffer_macro( void );
/*forward*/ int rename_macro( void );
/*forward*/ static unsigned char *save_macro_str( unsigned char *s, int len );
/*forward*/ unsigned char *macro_address_to_name(struct keymap *addr);
/*forward*/ void init_macros( void );

unsigned int *new_names;	/* *bound_name
 * points into the list of bound macro names; used for init_ialization */

/* Find the index of the named macro or command; -(index of where the
 * name should have been if it isn't found)-1. */
int find_mac( unsigned char *s )
	{
	int hi; int lo; int mid;
	unsigned char *s1;
	unsigned char *s2;

	lo = 0;
	hi = n_macs - 1;

	/* test only required for the first call */
	if( hi < 0 )
		return -1;

	while( lo <= hi )
		{
		mid = (lo + hi) >> 1;
		s1 = s;
		s2 = mac_names[mid];
		while( *s1 == *s2 )
			{
			if( *s1 == 0 )
				return mid;
			s1++; s2++;
			}
		if( *s1 < *s2 )
			hi = mid - 1;
		else
			lo = mid + 1;
		}
	return - lo - 1;
	}
/* define the named macro to have the given body
 * (or mlisp proc) */
void def_mac
	(
	unsigned char *s,
	void *p_body,
	int is_mlisp,
	int len,
	struct bound_name *prealloced_bound_name
	)
	{
	union bound_ptr body;
	struct bound_name *p;
	int i;

	body.b_ptr = p_body;
	i = find_mac( s );

	if( i < 0 )
		{
		struct bound_name **bp;
		unsigned char **np;
		struct bound_name **be;

		if( n_macs >= MAXMACS )
			{
			error (u_str("Too many macro definitions."));
			return;
			}

		np = &mac_names[n_macs];
		bp = &mac_bodies[n_macs];
		n_macs++;
		be = &mac_bodies[-i - 1];

		np[1] = NULL;	/* sneaky */
		while( bp > be )
			{
			np[0] = np[-1];
			bp[0] = bp[-1];
			np--; bp--;
			}
		if( prealloced_bound_name )
			p = bp[0] = prealloced_bound_name;
		else
			p = bp[0] = malloc_struct( bound_name );
		if( is_mlisp == -5 )
			p->b_proc_name = np[0] = s;
		else
			p->b_proc_name = np[0] = savestr( s );
		p->b_proc_name_size = _str_len( p->b_proc_name );
		p->b_active = 0;
		p->b_break = 0;
		}
	else
		{
		p = mac_bodies[i];
		switch( p->b_binding )
		{
		case PROCBOUND:
			error (u_str("%s is already bound to a wired procedure!"), s );
			return;

		case MLISPBOUND:
			lisp_free( p->b_bound.b_prog );
			break;

		case KEYBOUND:
			{
			struct emacs_buffer *b;
			b = buffers;
			while( b != 0 )
				{
				if( b->b_mode.md_keys  == p->b_bound.b_keymap )
					b->b_mode.md_keys  = 0;
				b = b->b_next;
				}
			if( current_global_map == p->b_bound.b_keymap )
				current_global_map = global_map;
			if( bf_mode.md_keys == p->b_bound.b_keymap )
				bf_mode.md_keys = 0;
			next_local_keymap = next_global_keymap = 0;
			free( p->b_bound.b_keymap );
			break;
			}
		case EXTERNALBOUND:
			free( p->b_bound.b_func->ef_funcname  );
			free( p->b_bound.b_func );
			break;

		default:
			free( p->b_bound.b_body );
		};
		}
	switch( is_mlisp )
	{
	case -1:
		p->b_binding = AUTOLOADBOUND;
		p->b_bound.b_body = savestr( body.b_body );
		break;
	case  -2:
		p->b_binding = KEYBOUND;
		p->b_bound.b_keymap = body.b_keymap;
		break;
	case  -5:
		p->b_binding = PROCBOUND;
		p->b_bound.b_keymap = body.b_keymap;
		break;
	case  0:
		p->b_binding = MACROBOUND;
		p->b_bound.b_body = save_macro_str( body.b_body, len);
		break;
	case  -3:
		p->b_binding = 0;
		p->b_bound.b_body = 0;
		break;
	case  -4:
		p->b_binding = EXTERNALBOUND;
		p->b_bound.b_func = body.b_func;
		break;
	default:
		p->b_binding = MLISPBOUND;
		p->b_bound.b_prog = body.b_prog;
		break;
	}
	}

int edit_macro( void )
	{
	struct bound_name *p;
	int i;

	i = getword( mac_names_ref, u_str(": edit-macro ") );

	if( i < 0 )
		return 0;

	p = mac_bodies[i];
	if( p->b_binding != MACROBOUND )
		error (u_str("%s is a procedure, not a macro!"),
			p->b_proc_name );
	else
		{
		int n;
		unsigned char c;
		unsigned char *cp;

		set_bfn (u_str("Macro edit"));
		erase_bf( bf_cur );
		if( bf_cur->b_fname )
			free( bf_cur->b_fname );
		bf_cur->b_fname = savestr( p->b_proc_name );
		bf_cur->b_kind = MACROBUFFER;
		window_on( bf_cur );

		cp = p->b_bound.b_body;
		n = 1;

		while( (c = *cp++) != 0 )
			{
			if( c == 0x80 )
				c = *cp++, c--;
			insert_at( n, c );
			n++;
			}
		bf_modified = 0;
		beginning_of_file();
		}
	return 0;
	}
int define_buffer_macro( void )
	{
	if( bf_cur->b_kind != MACROBUFFER
	|| bf_cur->b_buf_name == NULL
	|| bf_cur->b_fname == NULL )
		{
		error (u_str("This buffer does not contain a named macro."));
		return 0;
		}
	gap_to( bf_s1 + bf_s2 + 1 ); /* ignoring our abstract data type hiding */

	def_mac( bf_cur->b_fname, bf_p1 + 1, 0, bf_s1, 0 );

	bf_modified = 0;
	
	return 0;
	}
/* MLisp function to rename Macros */

int rename_macro( void )
	{
	struct bound_name *from_body;
	struct bound_name *to_body;
	unsigned char *to_name;
	int i;

	i = getword( mac_names_ref, u_str(": rename-mlisp-procedure (old name) "));

	/*
	 *	Get all the rename info
	 */
	if( i < 0 )
		return 0;
	from_body = mac_bodies[i];
	to_name = getstr(
u_str(": rename-mlisp-procedure (old name) %s to (new name) "),
			mac_names[i]);
	if( to_name == NULL )
		return 0;
	/*
	 *	If we are renaming into an active bound_name, deallocate the
	 * old bound data structure
	 */
	if( (i = find_mac( to_name )) >= 0 )
		{
		to_body = mac_bodies[i];
		if( to_body->b_binding == MLISPBOUND )
			lisp_free( to_body->b_bound.b_prog );
		else
			if( to_body->b_binding == MACROBOUND
			|| to_body->b_binding == AUTOLOADBOUND )
				free( to_body->b_bound.b_body );
			else
				{
				error(u_str("\"%s\" is bound to a wired procedure and cannot be redefined"), to_name);
				return 0;
				}
		}
	else
		{
		def_mac( to_name, 0, -3, 0, 0 );
		if( (i = find_mac( to_name )) < 0 )
			{
			error (u_str("definition bogosity for function %s"), to_name );
			return 0;
			}
		to_body = mac_bodies[i];
		}
	/*
	 *	Now copy the binding information
	 */
	to_body->b_bound = from_body->b_bound;
	to_body->b_binding = from_body->b_binding;
	to_body->b_active = from_body->b_active;
	to_body->b_break = from_body->b_break;
	to_body->b_proc_name_size = from_body->b_proc_name_size;
	/*
	 *	Declare the From body to be unbound
	 */
	from_body->b_binding = MLISPBOUND;
	from_body->b_bound.b_proc = 0;
	from_body->b_active = 0;
	from_body->b_break = 0;
	return 0;
	}
static unsigned char *save_macro_str
	(
	unsigned char *s,
	int len
	)
	{
	unsigned char *ret;
	unsigned char *p;
	unsigned int size;
	int i;

	size = len + 1;
	for( i=0; i<=len - 1; i += 1 )
		if( s[i] == 0 || s[i] == 0x80 )
			size++;

	p = ret = malloc_ustr( size );

	for( i=0; i<=len - 1; i += 1 )
		if( s[0] == 0 || s[0] == 0x80 )
			{
			*p++ =  0x80;
			*p++ = (unsigned char)(*s++ + 1);
			}
		else
			*p++ =  *s++;

	*p++ =  0;

	return ret;
	}

unsigned char *macro_address_to_name(struct keymap *addr)
	{
	struct bound_name *b;
	int i;

	if( addr != 0 )
		{
		for( i=0; i<=n_macs-1; i += 1 )
			{
			b = mac_bodies[i];
			if( b->b_bound.b_keymap == addr )
				return b->b_proc_name;
			}
		}
	return 0;
	}

void init_macros( void )
	{
	return;
	}
