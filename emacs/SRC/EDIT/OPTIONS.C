/*	Copyright (c) 1982-1994
 *		Barry A. Scott and Nick Emery
 *
 * A random assortment of commands: help facilities, macros, key bindings
 * and package loading.
 */
#include <emacs.h>
#ifdef vms
#include <stsdef.h>
#include <lnmdef.h>
#endif

struct key_scan_history
	{
	struct key_scan_history *hist_prev;
	struct keymap *hist_this;
	};

/*forward*/ int change_directory( void );
/*forward*/ unsigned char *key_to_str( unsigned char *keys, int len );
/*forward*/ int describe_key( void );
/*forward*/ int local_binding_of( void );
/*forward*/ int global_binding_of( void );
/*forward*/ static int binding_of_inner(int local_bind);
/*forward*/ void scan_map( struct keymap *kmap, void (*proc)( struct bound_name *b, unsigned char *keys, int len, int range ), int fold_case );
/*forward*/ static void scan_map_inner( struct keymap *kmap, void (*proc)( struct bound_name *b, unsigned char *keys, int len, int range ), struct key_scan_history *history, unsigned char *keys, int len, int fold_case );
/*forward*/ static void describe1( struct bound_name *b, unsigned char *keys, int len, int range );
/*forward*/ int describe_bindings( void );
/*forward*/ int define_keyboard_macro( void );
/*forward*/ int define_string_macro( void );
/*forward*/ int bind_to_key( void );
/*forward*/ int local_bind_to_key( void );
/*forward*/ static int bind_to_key_inner(int local_bind);
/*forward*/ int remove_binding( void );
/*forward*/ int use_global_map( void );
/*forward*/ int use_local_map( void );
/*forward*/ static void initialize_local_map( void );
/*forward*/ static struct bound_name *autodefinekeymap( void );
/*forward*/ static void perform_bind( struct keymap **tbl, struct bound_name *name );
/*forward*/ int remove_local_binding( void );
/*forward*/ int remove_all_local_bindings( void );
/*forward*/ int execute_extended_command( void );
/*forward*/ int define_keymap_command( void );
/*forward*/ int auto_load( void );
/*forward*/ int execute_bound( struct bound_name *p );
/*forward*/ int dump_stack_trace( void );
/*forward*/ int check_trace_hook(unsigned char *val, struct variablename *v);
/*forward*/ int check_error_hook(unsigned char *val, struct variablename *v);
/*forward*/ int check_break_hook(unsigned char *val, struct variablename *v);
/*forward*/ int check_trace_mode(unsigned char *val, struct variablename *v);
/*forward*/ int error_handler( void );
/*forward*/ static int break_point( struct prognode *p, int larg );
/*forward*/ static int trace( struct prognode *p, int larg );
/*forward*/ static int breaktrace( struct prognode *p, struct bound_name *b, int larg );
/*forward*/ int set_break_point( void );
/*forward*/ int list_break_points( void );
/*forward*/ int decompile_current_line( void );
/*forward*/ int decompile_function( void );
/*forward*/ static void decompile_puts( unsigned char *, int );
/*forward*/ static void decompile_string( unsigned char *str, int len );
/*forward*/ static unsigned char *decompile( struct prognode *p, int larg, int indent, int depth );
/*forward*/ static void decompile_inner( struct prognode *p, int depth, int indent );
#if defined(vms) && defined(CALL_BACK)
/*forward*/ static int activate_handler( struct	chf$signal_array *sig, struct chf$mech_array *mech );
/*forward*/ static int call_back(int function, ...);
/*forward*/ static int activate_image( unsigned char *image, unsigned char *symbol,
	int (**reference)(int (*call_back)(int,...), void *, int *), unsigned char *file );
/*forward*/ static int activate_external_function( struct bound_name *b );
#endif
/*forward*/ int list_images( void );
/*forward*/ void init_opt( void );

#if defined(vms) && defined(CALL_BACK)
extern int lib$analyze_sdesc( struct dsc$descriptor *, unsigned short *, unsigned char **);
extern int lib$find_image_symbol( struct dsc$descriptor *, struct dsc$descriptor *,void * *);
extern int str$upcase(struct dsc$descriptor *,struct dsc$descriptor *);
extern int str$copy_r( struct dsc$descriptor *, unsigned short *, unsigned char * );
#endif

static struct prognode *trace_cur_exec;
static int trace_larg;

GLOBAL SAVRES struct bound_name *trace_proc;
GLOBAL SAVRES struct bound_name *break_proc;
GLOBAL SAVRES struct bound_name *error_proc;
GLOBAL SAVRES unsigned int last_auto_keymap;	/* Index of last auto keymap */
GLOBAL SAVRES unsigned int decompile_max;	/* Size of DecompileBuffer */
GLOBAL SAVRES unsigned char *decompile_buffer;	/* The Trace back buffer */
GLOBAL SAVRES unsigned int decompile_used;
GLOBAL SAVRES int *current_function_length;
GLOBAL SAVRES int *current_break_length;	/* Current function name ptrs */
GLOBAL SAVRES unsigned char **current_function;
GLOBAL SAVRES unsigned char **current_break;
GLOBAL SAVRES struct expression *call_back_result;
GLOBAL SAVRES unsigned char *call_back_str;
GLOBAL SAVRES unsigned int call_back_len;
static int trace_mode_count;

unsigned char sexpr_defun[] = "s-expr";



int change_directory( void )
	{
	unsigned char *nd;
	nd = getstr (u_str(": change-directory "));
	if( nd == 0 )
		return 0;
	if( chdir_and_set_global_record( nd ) < 0 )
		error (u_str("Cannot change to directory %s"), nd);
	return 0;
	}




/*
 * Given a sequence of keystrokes (at "keys" for "len" characters) return a
 * printable representation of them -- with ESCs for escapes, and similar rot
 */
unsigned char *key_to_str
	(
	unsigned char *keys,
	int len
	)
	{
	static unsigned char buf[80];
	int i;
	unsigned char *p;

	p = &buf[0];
	if( keys == 0 || len == 0 )
		return u_str("[Unknown keys]");

	for( i=1; i<=len; i += 1 )
		{
		unsigned char *value;
		int matched;
		unsigned char ch;

		matched = lookup_key_value( len+1-i, keys, &value );
		if( matched > 0
		&& p < &buf[ sizeof( buf ) - 1 - _str_len( value ) ] )
			{
			while( *value )
				*p++ = *value++;
			i = i + matched - 1;
			keys = keys + matched;
			}
		else
		{
		if( p > &buf[ sizeof( buf ) - 5 ] )
			return u_str("[long key sequence]");
		if( (ch = *keys++) == ctl('[') )
			{
			*p++ =  'E';
			*p++ =  'S';
			*p++ =  'C';
			}
		else
			{
			if( ch <= 31 )
				{
				*p++ = '^';
				*p++ = (unsigned char)(ch + '@');
				}
			else if( (ch >= 32 && ch <= 126)
			|| (ch >= 161 && ch <= 254) )
				*p++ =  ch;
			else if( ch == 127 )
				{
				*p++ = '^';
				*p++ = '?';
				}
			else
				{
				*p++ = '\\';
				*p++ = (unsigned char)(((ch>>6)&7) + '0');
				*p++ = (unsigned char)(((ch>>3)&7) + '0');
				*p++ = (unsigned char)((ch&7) + '0');
				}
			}
		}
		if( i < len )
			*p++ =  '-';
		}
	*p++ =  0;

	return &buf[0];
	}



int describe_key( void )
	{
	unsigned char *keyp;
	unsigned char key;
	struct bound_name **p;
	unsigned char *where_bound;

	where_bound = u_str("globally");
	keyp = get_key( current_global_map, u_str(": describe-key ") );
	if( keyp == 0 || err )
		{
		void_result ();
		return 0;
		}
	key = *keyp++;
	p = lookup_keys(bf_mode.md_keys, ml_value->exp_v.v_string, ml_value->exp_int);
	if( p != 0 && p[0] == 0 )
		p = 0;
	if( p != 0 )
		where_bound = u_str("locally");
	else
		p = lookup_keys( current_global_map, ml_value->exp_v.v_string, ml_value->exp_int);
	if( p == 0 || p[0] == 0 )
		error (u_str("%s is not bound to anything"),
			key_to_str( ml_value->exp_v.v_string, ml_value->exp_int ));
	else
		{
		unsigned char *mlisp;
		unsigned char *line;
		unsigned char *type;
		struct bound_name *b;

		line = malloc_ustr( 300 );
		if( line == 0 )
			return 0;

		b = p[0];
		if( b->b_proc_name == sexpr_defun )
			mlisp = decompile( b->b_bound.b_prog, 1, 0, 1 );
		else
			mlisp = 0;
		switch( b->b_binding )
		{
		case MACROBOUND:	type = u_str("macro"); break;
		case AUTOLOADBOUND:	type = u_str("autoloaded function"); break;
		case MLISPBOUND:	type = u_str("MLisp function"); break;
		case EXTERNALBOUND:	type = u_str("external function"); break;
		case KEYBOUND:		type = u_str("keymap"); break;
		default:		type = u_str("wired procedure");
		}
		sprintfl
		(
		line, 300, u_str("%s is %s bound to the %s \"%s\""),
		key_to_str( ml_value->exp_v.v_string, ml_value->exp_int ),
		where_bound,
		type,
		(( mlisp != 0 ) ?  mlisp : b->b_proc_name) );
		if( interactive )
			{
			message (u_str("%s"), line);
			free( line );
			void_result();
			}
		else
			{
			release_expr( ml_value );
			ml_value->exp_type = ISSTRING;
			ml_value->exp_v.v_string = line;
			ml_value->exp_release = 1;
			ml_value->exp_int = _str_len( line );
			}
		}
	return 0;
	}



int local_binding_of( void )
	{
	return binding_of_inner( 1 );
	}

int global_binding_of( void )
	{
	return binding_of_inner( 0 );
	}

static int binding_of_inner(int local_bind)
	{
	unsigned char *func_name;
	unsigned char *key;
	unsigned char *str;
	struct keymap *kmap;
	struct bound_name *b;
	struct bound_name **ref_b;

	if( local_bind )
		{
		kmap = bf_mode.md_keys;
		func_name = u_str(": local-binding-of ");
		}
	else
		{
		kmap = current_global_map;
		func_name = u_str(": global-binding-of ");
		}

	key = get_key( current_global_map, func_name );
	if( key == 0 || err )
		return 0;

	ref_b = lookup_keys( kmap, ml_value->exp_v.v_string, ml_value->exp_int );
	ml_value->exp_type = ISSTRING;
	ml_value->exp_release = 0;
	if( ref_b == 0 || ref_b[0] == 0 )
		{
		ml_value->exp_v.v_string = u_str("nothing");
		ml_value->exp_int = sizeof("nothing");
		return 0;
		}
	b = ref_b[0];
	if( b->b_proc_name == sexpr_defun )
		{
		str = decompile( b->b_bound.b_prog, 1, 0, 1 );
		if( str != 0 )
			{
			ml_value->exp_v.v_string = str;
			ml_value->exp_int = decompile_used;
			return 0;
			}
		}

	ml_value->exp_v.v_string = b->b_proc_name;
	ml_value->exp_int = b->b_proc_name_size;

	return 0;
	}



/*
 * Recursively scan a keymap tree. It gets passed a pointer to a map and a
 * function. For each bound_name the function is called with these
 * parameters: the bound_name, the keystrokes leading to it
 * (as a unsigned char * and
 * an int) and a count of the number of following keys that are bound to the
 * same bound_name. A run of equal bound_names in a keymap is only passed to
 * the procedure once.
 */
void scan_map
	(
	struct keymap *kmap,
	void (*proc)( struct bound_name *b, unsigned char *keys, int len, int range ),
	int fold_case
	)
	{
	unsigned char keys[512];
	if( kmap != 0 )
		scan_map_inner( kmap, proc, 0, &keys[0], 0, fold_case );
	}

static void scan_map_inner
	(
	struct keymap *kmap,
	void (*proc)( struct bound_name *b, unsigned char *keys, int len, int range ),
	struct key_scan_history *history,
	unsigned char *keys,
	int len,
	int fold_case
	)
	{
	struct key_scan_history hist;
	struct bound_name *b;
	int c, c2;

	hist.hist_prev = history;
	hist.hist_this = kmap;

	c = 0;
	while( c <= 255 )
		{
		c2 = c + 1;
		if( (b = get_keymap_binding( kmap, c)) != 0
		&& (! fold_case || ! isupper(c)
			|| (b != get_keymap_binding( kmap, (unsigned char)tolower((unsigned char)c) ))) )
			{
			keys[len] = (unsigned char)c;
			while( c2 < 256
			&& (get_keymap_binding( kmap, c2 ) == b) )
				c2++;
			proc( b, keys, len + 1, c2 - c );
			if( b->b_binding == KEYBOUND && b->b_bound.b_keymap != 0 )
				{
				struct key_scan_history *h;
				h = history;
				while( h != 0 && h->hist_this != kmap )
					h = h->hist_prev;
				if( h == 0 )
					scan_map_inner( b->b_bound.b_keymap, proc, &hist, keys, len + 1, fold_case );
				}
			}
		c = c2;
		}
	}

/* Helper function for DescribeBindings -- inserts one line of info for the
 * given bound_name */
static void describe1
	(
	struct bound_name *b,
	unsigned char *keys,
	int len,
	int range
	)
	{
	int indent;
	unsigned char *s;

	s = key_to_str( keys, len );
	indent = _str_len( s );
	ins_cstr( s, indent );
	if( range > 1 )
		{
		int k;

		keys[ len - 1 ] = (unsigned char)(keys[ len - 1 ] + range - 1);
		ins_cstr( u_str(".."), 2 );
		s = key_to_str( keys, len );
		k = _str_len( s );
		ins_cstr( s, k );
		indent = indent + k + 2;
		keys[len - 1] = (unsigned char)(keys[ len - 1 ] - range - 1);
		}
	ins_cstr( u_str("                                "), 32 - min( indent, 31 ) );
	if( b->b_proc_name == sexpr_defun )
		{
		unsigned char *str;
		str = decompile( b->b_bound.b_prog, 1, 0, 1 );
		if( str != 0 )
			ins_cstr( str, decompile_used );
		}
	else
		ins_cstr( b->b_proc_name, b->b_proc_name_size );
	ins_cstr( u_str("\n"), 1 );
	}



int describe_bindings( void )
	{
	unsigned char *s;
	struct keymap *local_map;
	local_map = bf_mode.md_keys;
	scratch_bfn( u_str("Help"), 1 );
	ins_str( u_str("Global Bindings (") );
	s = macro_address_to_name( current_global_map );
	if( s != 0 )
		ins_str( s );
	ins_str( u_str("):\nKey				Binding\n---				-------\n") );
	scan_map( current_global_map, describe1, 1 );
	if( local_map != 0 )
		{
		ins_str( u_str("\nLocal bindings (") );
		s = macro_address_to_name( local_map );
		if( s != 0 )
			ins_str( s );
		ins_str( u_str("):\n") );
		scan_map( local_map, describe1, 0 );
		}
	beginning_of_file();
	bf_modified = 0;
	return 0;
	}



int define_keyboard_macro( void )
	{
	unsigned char *name;
	if( remembering != 0 )
		{
		error (u_str("Not allowed to define a macro while remembering."));
		return 0;
		}
	if( mem_used <= 0 )
		{
		error (u_str("No keyboard macro defined."));
		return 0;
		}
	name = getnbstr( u_str(": define-keyboard-macro ") );
	if( name == 0 )
		return 0;
	def_mac( name, &key_mem[0], 0, mem_used, 0 );
	mem_used = 0;
	return 0;
	}



int define_string_macro( void )
	{
	unsigned char name[200];
	unsigned char *p;
	if( remembering != 0 )
		{
		error( u_str("Not allowed to define a macro while remembering.") );
		return 0;
		}
	p = getnbstr( u_str(": define-string-macro ") );
	if( p == 0 )
		return 0;
	_str_cpy( &name[0], p );
	p = getstr( u_str(": define-string-macro %s body: "), &name[0] );
	if( p == 0 )
		return 0;
	def_mac( &name[0], p, 0, _str_len( p ), 0 );
	return 0;
	}



int bind_to_key( void )
	{
	return bind_to_key_inner( 0 );
	}

int local_bind_to_key( void )
	{
	return bind_to_key_inner( 1 );
	}

static int bind_to_key_inner(int local_bind)
	{
	unsigned char *func_name;
	unsigned char *string;
	int i;
	unsigned char *c;
	struct keymap *k;
	struct bound_name *b = NULL;
	struct prognode *p;

	if( local_bind )
		func_name = u_str("local-bind-to-key");
	else
		func_name = u_str("bind-to-key");

	if( cur_exec == 0 )
		{
		i = getword( mac_names_ref,
				u_str(": %s name: "), func_name );
		if( i < 0 )
			return 0;

		b = mac_bodies[i];
		string = mac_names[i];
		}
	else
		{
		last_arg_used++;
		if( ! string_arg( last_arg_used + 1 ) )
			{
			error( u_str( "%s expects a string as argument 1" ), func_name );
			return 0;
			}

		string = ml_value->exp_v.v_string;
		if( string[0] == '(' )
			{
			p = parse_mlisp_line( string );
			if( p != 0 )
				{
				b = malloc_struct( bound_name );
				if( b == 0 )
					{
					lisp_free( p );
					return 0;
					}
				b->b_bound.b_prog = p;
				b->b_binding = MLISPBOUND;
				b->b_proc_name = sexpr_defun;
				b->b_proc_name_size = _str_len( sexpr_defun );
				b->b_active = 0;
				b->b_break = 0;
				}
			}
		else
			{
			b = 0;
			i = find_mac( string );
			if( i > 0 )
				b = mac_bodies[i];
			}
		}
	if( b == 0 )
		{
		error( u_str( "%s expects %s to be an MLisp function or expression"),
				func_name, string );
		return 0;
		}
	if( local_bind )
		{
		initialize_local_map();
		k = bf_mode.md_keys;
		}
	else
		k = current_global_map;

	c = get_key( k, u_str(": %s name: %s key: "), func_name, string );
	if( c == 0 )
		{
		if( b->b_proc_name == sexpr_defun )
			{
			lisp_free( b->b_bound.b_prog );
			free( b );
			}
		return 0;
		}

	perform_bind( &k, b );
	return 0;
	}



void free_sexpr_defun( struct bound_name *b )
	{
	if( b->b_proc_name == sexpr_defun )
		{
		lisp_free( b->b_bound.b_prog );
		free( b );
		}
	}



int remove_binding( void )
	{
	unsigned char *c;
	struct bound_name **ref_b;	/* bound_name */

	c = get_key( current_global_map, u_str(": remove-binding ") );
	if( c != 0 && ! err )
		{
		ref_b = lookup_keys( current_global_map, ml_value->exp_v.v_string, ml_value->exp_int );
		if( ref_b != 0 && ref_b[0] != 0 )
			{
			free_sexpr_defun( ref_b[0] );
			ref_b[0] = 0;
			}
		}
	void_result ();
	return 0;
	}



int use_global_map( void )
	{
	int i;
	i = getword( mac_names_ref, u_str(": use-global-map ") );
	if( i < 0 )
		return 0;
	if( mac_bodies[i]->b_binding  != KEYBOUND )
		error( u_str("%s is not a keymap."), mac_names[i] );
	else
		current_global_map = mac_bodies[i]->b_bound.b_keymap ;
	next_global_keymap = next_local_keymap = 0;
	return 0;
	}



int use_local_map( void )
	{
	int i;

	i = getword( mac_names_ref, u_str(": use-local-map ") );
	if( i < 0 )
		return 0;
	if( mac_bodies[i]->b_binding  != KEYBOUND )
		error( u_str("%s is not a keymap."), mac_names[i] );
	else
		bf_mode.md_keys = bf_cur->b_mode.md_keys  =
			mac_bodies[i]->b_bound.b_keymap ;
	next_global_keymap = next_local_keymap = 0;
	return 0;
	}

/* The following procedure is a horrible compatibility hack. It
 * is called to ensure that the local map exists and that the ESC and ^X
 * slots in it are non-empty. If they are empty, then they are forced to be
 * bound to keymaps. */
static void initialize_local_map( void )
	{
	if( bf_mode.md_keys == 0
	|| get_keymap_binding( bf_mode.md_keys, ctl('[') ) == 0
	|| get_keymap_binding( bf_mode.md_keys, ctl('X') ) == 0 )
		{
		unsigned char horrible_hack[2];
		horrible_hack[0] = ctl('[');
		horrible_hack[1] = 0;
		release_expr( ml_value );
		ml_value->exp_v.v_string = &horrible_hack[0];
		ml_value->exp_int = 2;
		if( bf_mode.md_keys == 0
		|| get_keymap_binding( bf_mode.md_keys, ctl('[') ) == 0 )
			perform_bind( &bf_mode.md_keys, 0 );
		horrible_hack[0] = ctl('X');
		ml_value->exp_v.v_string = &horrible_hack[0];
		ml_value->exp_int = 2;
		if( bf_mode.md_keys == 0
		|| get_keymap_binding( bf_mode.md_keys, ctl('X') ) == 0 )
			perform_bind( &bf_mode.md_keys, 0 );
		}
	}

static struct bound_name *autodefinekeymap( void )
	{
	unsigned char auto_name[40];

	if( n_macs >= MAXMACS )
		{
		error( u_str("Too many macro definitions.") );
		return 0;
		}

	do
		{
		sprintfl( auto_name, sizeof( auto_name ),
			u_str("~~auto-defined-keymap-%d"), last_auto_keymap );
		last_auto_keymap++;
		}
	while( find_mac( auto_name ) >= 0 );

	if( define_keymap( auto_name ) == 0 )
		return 0;

	return mac_bodies[ find_mac( auto_name ) ];
	}

static void perform_bind
	(
	struct keymap **tbl,
	struct bound_name *name
	)
	{
	unsigned char *p;
	int level;
	struct bound_name *b;
	struct keymap *k;
	int i;

	p = ml_value->exp_v.v_string;
	level = ml_value->exp_int;
	/*
	 *	If the keymap is missing completely
	 *	create a new one. The binding is not
	 *	important yet.
	 */
	if( tbl[0] == 0 )
		{
		b = autodefinekeymap();
		if( b == 0 )
			return;
		tbl[0] = b->b_bound.b_keymap;
		if( tbl == &bf_mode.md_keys )
			bf_cur->b_mode.md_keys  = bf_mode.md_keys;
		}

	/*
	 * init the keymap pointer
	 */
	k = tbl[0];
	b = 0;
	/*
	 * create the the keymap path if required and
	 * leave k pointing at the last keymap.
	 */
	for( i=0; i<=level-2; i += 1 )
		{
		unsigned char ch;
		ch = *p++;
		b = get_keymap_binding( k, ch );
		if( b == 0
		|| b->b_binding != KEYBOUND )
			{
			b = autodefinekeymap();
			if( b == 0 )
				return;
			add_keymap_binding( k, ch, b );
			}

		k = b->b_bound.b_keymap;
		}

	/* final bind the procedure to the final keymap */
	add_keymap_binding( k, *p, name );
	void_result();
	}



int remove_local_binding( void )
	{
	unsigned char *c;
	struct bound_name **ref_b;	/* bound_name */

	initialize_local_map();
	c = get_key( bf_mode.md_keys, u_str(": remove-local-binding ") );
	if( c != 0 && ! err )
		{
		ref_b = lookup_keys( bf_mode.md_keys, ml_value->exp_v.v_string, ml_value->exp_int );
		if( ref_b != 0 && ref_b[0] != 0 )
			{
			free_sexpr_defun( ref_b[0] );
			ref_b[0] = 0;
			}
		}
	void_result();
	return 0;
	}



int remove_all_local_bindings( void )
	{
	int i;
	struct keymap *m;

	if( (m = bf_mode.md_keys) != 0 )
		{
		if( m->k_type == KEYMAP_LONG_TYPE )
			{
			struct bound_name **ref_b;

			ref_b = &m->k_map.k_long->k_binding[0];
			for( i=0; i<=255; i += 1 )
				if( ref_b[i] != 0 )
					free_sexpr_defun( ref_b[i] );

			free( m->k_map.k_long );
			m->k_type = KEYMAP_SHORT_TYPE;
			m->k_map.k_short = malloc_struct( keymap_short );
			m->k_map.k_short->k_used  = 0;
			}
		else
			{
			struct bound_name **ref_b;

			ref_b = m->k_map.k_short->k_sbinding;
			for( i=0; i<=m->k_map.k_short->k_used  - 1; i += 1 )
				if( ref_b[i] != 0 )
					free_sexpr_defun( ref_b[i] );

			m->k_map.k_short->k_used  = 0;
			}
		}
	return 0;
	}



int execute_extended_command( void )
	{
	int larg;
	int ind;
	struct bound_name *p;
	int rv;

	larg = arg;
	arg = 1;
	ind = getword( mac_names_ref, u_str(": ") );
	arg = larg;
	if( ind < 0 )
		return 0;

	p = mac_bodies[ind];
	rv = execute_bound( p );
	if( interactive && ! err && ml_value->exp_type != ISVOID )
		switch( ml_value->exp_type )
		{
		case ISWINDOWS:
			message( u_str("MLisp function return windows") );
			break;
		case ISINTEGER:
			message( u_str("MLisp function returned %d" ), ml_value->exp_int);
			break;
		case ISSTRING:
			message( u_str("MLisp function returned \"%s\"" ), ml_value->exp_v.v_string);
			break;
		case ISMARKER:
			{
			struct marker *m;
			m = ml_value->exp_v.v_marker;
			if( m != 0 )
				{
				struct emacs_buffer *old;
				old = bf_cur;
				message( u_str("MLisp function returned Marker (\"%s\", %d)" ),
						m->m_buf->b_buf_name ,
						to_mark( m ) );
				set_bfp( old );
				}
			}
			break;
		default:
			error( u_str("MLisp function returned a bizarre result!") );
		}

	return rv;
	}



int define_keymap_command( void )
	{
	unsigned char *mapname;
	mapname = getnbstr( u_str(": define-keymap ") );
	if( mapname == 0 )
		return 0;

	define_keymap( mapname );
	return 0;
	}



int auto_load( void )
	{
	unsigned char *comname;
	unsigned char combuf[500];
	unsigned char *filename;
	comname = getnbstr( u_str(": autoload procedure ") );
	if( comname == 0 )
		return 0;
	_str_cpy( &combuf, comname );
	filename = getnbstr( u_str(": autoload procedure %s from file "), &combuf );
	if( filename == 0 )
		return 0;
	def_mac( combuf, filename, -1, 0, 0 );
	return 0;
	}

extern int in_trace;

/* execute whatever is bound to p */
int execute_bound
	(
	struct bound_name *p
	)
	{
	int rv;
	int larg;
	unsigned char *old_current_function;
	size_t old_current_function_length;

#if DBG_ALLOC_CHECK
	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	/*
	 * See if there is any input activity to process
	 */
	tt->k_check_for_input(tt);

	rv = 0;
	if( arg_state == no_arg )
		arg = 1;
	else if( arg_state == prepared_arg )
		arg_state = have_arg;
	larg = arg;
	release_expr( ml_value );
	ml_value = &global_value;
	global_value.exp_type = ISVOID;

	if( p->b_break && in_trace == 0 )
		{
		struct prognode dummy;
		struct prognode *old_cur_exec;

		old_cur_exec = cur_exec;
		*current_break = p->b_proc_name;
		*current_break_length = p->b_proc_name_size;
		if( cur_exec == 0 )
			{
			cur_exec = &dummy;
			dummy.p_proc = p;
			dummy.p_nargs = 0;
			dummy.p_active = 0;
			dummy.p_args[0].pa_int = 0;
			}
		break_point( cur_exec, larg );
		cur_exec = old_cur_exec;
		*current_break_length = 0;
		}

	switch( p->b_binding )
	{
	case MACROBOUND:
		{
		struct prognode *lcur_exec;
		lcur_exec = cur_exec;
		cur_exec = 0;
		do
			{
			exec_str( p->b_bound.b_body );
			larg--;
			}
		while( ! err && larg > 0 );
		cur_exec = lcur_exec;
		}
		break;
	case MLISPBOUND:
		{
		struct execution_stack parent;

		parent = execution_root;
		execution_root.es_prefix_argument = larg;
		execution_root.es_prefix_argument_provided = arg_state != no_arg;
		execution_root.es_cur_exec = cur_exec;
		execution_root.es_dyn_parent = &parent;
		arg_state = no_arg;
		old_current_function = *current_function;
		old_current_function_length = *current_function_length;
		if( in_trace == 0 )
			if( trace_mode != 0 )
				{
				int old_trace_mode;
				int current_trace_mode_count;
				int current_trace_into;

				trace( cur_exec, larg );
				old_trace_mode = trace_mode;
				current_trace_mode_count = trace_mode_count;
				current_trace_into = trace_into;
				*current_function = p->b_proc_name;
				*current_function_length = p->b_proc_name_size;
				if( current_trace_into == 0 )
					trace_mode = 0;
				rv = exec_prog( p->b_bound.b_prog );
				if( current_trace_into == 0
				&& current_trace_mode_count == trace_mode_count )
					trace_mode = old_trace_mode;
				}
			else
				{
				*current_function = p->b_proc_name;
				*current_function_length = p->b_proc_name_size;
				rv = exec_prog( p->b_bound.b_prog );
				}
		else
			rv = exec_prog( p->b_bound.b_prog );
		*current_function = old_current_function;
		*current_function_length = old_current_function_length;
		execution_root = parent;
		}
		break;
	case AUTOLOADBOUND:
		{
		int larg;
		enum arg_states lstate;

		larg = arg;
		lstate = arg_state;
		arg = 0;
		arg_state = no_arg;
		rv = execute_mlisp_file( p->b_bound.b_body, 0 );
		if( ! err && rv == 0 )
			if( p->b_binding == AUTOLOADBOUND )
				error( u_str("%s was supposed to be defined by autoloading %s, but it was not." ),
					p->b_proc_name, p->b_bound.b_body);
			else
				{
				arg = larg;
				arg_state = lstate;
				rv = execute_bound( p );
				}
		}
		break;

	case KEYBOUND:
		next_local_keymap = p->b_bound.b_keymap;
		break;

	case PROCBOUND:
		if( trace_mode != 0 && in_trace == 0 )
			trace( cur_exec, larg );
		rv = p->b_bound.b_proc();
		if( arg_state != prepared_arg )
			last_proc = p->b_bound.b_proc;
		if( dot < first_character )
			set_dot( first_character );
		if( dot > num_characters )
			set_dot( num_characters + 1 );
		break;

	case EXTERNALBOUND:
#if defined(CALL_BACK)
		if( trace_mode != 0 && in_trace == 0 )
			trace( cur_exec, larg );
		rv = activate_external_function( p );
#else
		error( u_str("external function not supported %s"), p->b_proc_name );
#endif
		break;
	default:
		_dbg_msg( u_str("Unknown binding type of %d in block %X"), p->b_binding, p );
	}
	if( p->b_binding != KEYBOUND && arg_state != prepared_arg )
		{
		arg_state = no_arg;
		arg = 1;
		}

#if DBG_ALLOC_CHECK
	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	return rv;
	}



/* Execute a bound procedure with the current execution environment saved */
int execute_bound_saved_environment
	(
	struct bound_name *p
	)
	{
	struct prognode *saved_cur_exec;	/* Saved cur_exec */
	struct expression saved_ml_value;	/* Saved ml_value */
	int saved_arg;
	int rv;				/* Saved arg */
	enum arg_states saved_arg_state;			/* Saved arg_state */

	/*
	 *	Save the old execution environment
	 */
	saved_cur_exec = cur_exec;
	cur_exec = 0;
	saved_ml_value = *ml_value;
	ml_value->exp_release = 0;
	saved_arg = arg;
	arg = 0;
	saved_arg_state = arg_state;
	arg_state = no_arg;
	/*
	 *	Execute the procedure
	 */
	rv = execute_bound( p );
	/*
	 *	Restore the old execution environment
	 */
	*ml_value = saved_ml_value;

	arg = saved_arg;
	arg_state = saved_arg_state;
	cur_exec = saved_cur_exec;
	return rv;
	}



/* Dump a stack trace to the stack trace buffer -- handles recursive calls
 *	(eg. from error()) */
int dump_stack_trace( void )
	{
	struct emacs_buffer *old;
	struct execution_stack *p;
	int set_window;

	static int dump_depth;
	old = bf_cur;
	set_window = wn_cur->w_buf == bf_cur;
	dump_depth++;
	if( dump_depth > 1 )
		return 0;
	scratch_bfn( u_str("Stack trace"), 1 );
	if( err )
		{
		ins_cstr( u_str("Message:	"), 11 );
		ins_str( minibuf_body );
		ins_cstr( u_str("\n"), 1 );
		}
	ins_cstr( u_str("Executing: "), 11 );
	print_expr( cur_exec, 1 );
	ins_cstr( u_str("\n"), 1 );

	p = &execution_root;
	while( p->es_dyn_parent != 0 && dump_depth <= 1 )
		{
		print_expr( p->es_cur_exec, 1 );
		ins_cstr( u_str("\n"), 1 );
		p = p->es_dyn_parent;
		}
	if( dump_depth > 1 )
		ins_str( u_str("** error during stack trace **\n" ) );
	else
		ins_str( u_str("--- bottom of stack ---\n") );
	set_dot( 1 );
	dump_depth = 0;
	bf_modified = 0;
	set_bfp( old );
	if( set_window )
		window_on( bf_cur );
	return 0;
	}



/* trace_mode support */
int check_trace_hook(unsigned char *val, struct variablename *v)
	{
	return check_is_proc( val, v, &trace_proc );
	}

int check_error_hook(unsigned char *val, struct variablename *v)
	{
	return check_is_proc( val, v, &error_proc );
	}

int check_break_hook(unsigned char * val, struct variablename *v)
	{
	return check_is_proc( val, v, &break_proc );
	}

int check_trace_mode(unsigned char * PNOTUSED(val), struct variablename *PNOTUSED(v))
	{
	trace_mode_count++;
	return 1;
	}

int error_handler( void )
	{
	int rv;
	rv = 0;
	if( in_trace == 0 && error_proc != 0 )
		{
		unsigned char msg[BUFFERSIZE];
		struct prognode dummy;
		struct prognode *old_cur_exec;

		old_cur_exec = cur_exec;
		memcpy( msg, error_message_text, sizeof msg );
		if( cur_exec == 0 )
			{
			cur_exec = &dummy;
			dummy.p_proc = 0;
			dummy.p_nargs = 0;
			dummy.p_active = 0;
			dummy.p_args[0].pa_int = 0;
			}
		rv = breaktrace( cur_exec, error_proc, arg );
		cur_exec = old_cur_exec;
		memcpy( error_message_text, msg, sizeof msg );
		}
	return rv;
	}

static int break_point
	(
	struct prognode *p,
	int larg
	)
	{
	if( p != 0 )
		return breaktrace( p, break_proc, larg );
	else
		return 0;
	}

static int trace
	(
	struct prognode *p,
	int larg
	)
	{
	if( p != 0 )
		{
		struct bound_name *n;

		n = p->p_proc;
		if( ! (n->b_break != 0 && break_proc != 0) )
			return breaktrace( p, trace_proc, larg );
		}
	return 0;
	}

static int breaktrace
	(
	struct prognode *p,
	struct bound_name *b,
	int larg
	)
	{
	int rv;
	rv = 0;
	if( b != 0 )
		{
		int old_trace_larg;
		struct prognode *old_trace_cur_exec;

		old_trace_larg = trace_larg;
		old_trace_cur_exec = trace_cur_exec;
		trace_cur_exec = p;
		trace_larg = larg;
		in_trace++;
		rv = execute_bound_saved_environment( b );
		in_trace--;
		trace_cur_exec = old_trace_cur_exec;
		trace_larg = old_trace_larg;
		}
	return rv;
	}



int set_break_point( void )
	{
	int i;

	i = getword( mac_names_ref, u_str(": breakpoint ") );
	if( i >= 0 )
		{
		int val;
		val = getnum( u_str(": breakpoint %s "), mac_names[i] );
		mac_bodies[i]->b_break  = val != 0;
		}
	return 0;
	}



int list_break_points( void )
	{
	struct emacs_buffer *old;
	int i;

	old = bf_cur;
	scratch_bfn( u_str("Breakpoint list"), interactive );
	ins_str( u_str("	Name\n	----\n") );
	for( i=0; i<=n_macs-1; i += 1 )
		if( mac_bodies[i]->b_break  )
			{
			ins_cstr( u_str("   "), 3 );
			ins_str( mac_names[i] );
			ins_cstr( u_str("\n"), 1 );
			}
	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );
	return 0;
	}



int decompile_current_line( void )
	{
	unsigned char *decompiled_line;
	if( trace_cur_exec == 0 )
		if( interactive )
			decompiled_line = u_str("");
		else
			decompiled_line = decompile( cur_exec, arg, 0, 1);
	else
		decompiled_line = decompile( trace_cur_exec, trace_larg, 0, 1 );

	if( decompiled_line == 0 )
		error( u_str("Not enough memory.") );
	else
		{
		ml_value->exp_type = ISSTRING;
		ml_value->exp_release = 0;
		ml_value->exp_int = decompile_used;
		ml_value->exp_v.v_string = decompiled_line;
		}
	return 0;
	}




static unsigned int current_line;
int decompile_function( void )
	{
	int i;
	struct bound_name *p;

	i = getword( mac_names_ref, u_str(": decompile-mlisp-function ") );
	if( i < 0 )
		return 0;
	p = mac_bodies[i];
	current_line = 0;
	if( p->b_binding != MLISPBOUND )
		{
		unsigned char *type;
		switch( p->b_binding )
		{
		case MACROBOUND:	type = u_str("a macro");
					break;
		case AUTOLOADBOUND:	type = u_str("an autoload function");
					break;
		case KEYBOUND:		type = u_str("a keymap");
					break;
		case PROCBOUND:		type = u_str("a wired-in procedure");
					break;
		case EXTERNALBOUND:	type = u_str("an external function");
					break;
		default:		type = u_str("not an MLisp function");
		}
		error( u_str("%s is %s" ), p->b_proc_name, type );
		}
	else
		{
		unsigned char *str;
		str = decompile( p->b_bound.b_prog, 1, 1, 3 );
		if( str == 0 )
			error( u_str("Out of memory") );
		else
			{
			int l;
			int old_dot;
			old_dot = dot;
			ins_cstr( u_str("(defun\n    ("), 12 );
			ins_cstr( p->b_proc_name, (l = p->b_proc_name_size) );
			ins_cstr( u_str("\n"), 1 );
			ins_cstr( str, decompile_used );
			ins_cstr( u_str("    )\n)\n"), 8 );
			set_dot( old_dot + (( current_line == 0 ) ?  0 : current_line + 12 + 1 + l) );
			}
		}

	return 0;
	}



static void decompile_put_str( unsigned char *str )
	{
	decompile_puts( str, _str_len( str ) );
	}

static void decompile_put_char( int ch )
	{
	unsigned char buf[1];

	buf[0] = (unsigned char)ch;
	decompile_puts( buf, 1 );
	}

static void decompile_put_int( int i )
	{
	unsigned char buf[32];
	sprintfl( buf, sizeof( buf ), u_str("%d"), i );
	decompile_puts( buf, _str_len( buf ) );
	}

static void decompile_puts( unsigned char *str, int size )
	{
	if( decompile_used + size > decompile_max - 100 )
		{
		decompile_max = decompile_max + (( size > BUFSIZ - 100 ) ?  size + BUFSIZ : BUFSIZ);
		decompile_buffer = realloc_ustr( decompile_buffer, decompile_max+1 );
		}
	if( decompile_buffer == 0 )
		{
		decompile_used = 0;
		decompile_max = 0;
		return;
		}
	memcpy( &decompile_buffer[decompile_used], str, size );
	decompile_used = decompile_used + size;
	}



static void decompile_string
	(
	unsigned char *str,
	int len
	)
	{
	int i;
	unsigned char buf[4];

	for( i=1; i<=len; i += 1 )
		{
		int ch;
		ch = *str++;
		if( ch < ' ' )
			{
			unsigned char *x;
			switch( ch )
			{
			case ctl('J'):	x = u_str("\\n"); break;
			case ctl('H'):	x = u_str("\\b"); break;
			case ctl('M'):	x = u_str("\\r"); break;
			case ctl('I'):	x = u_str("\\t"); break;
			case ctl('['):	x = u_str("\\e"); break;
			case 127:	x = u_str("\\^?"); break;
			default:
				{
				x = buf;
				buf[0] = '\\';
				buf[1] = '^';
				buf[2] = (unsigned char)(ch + '@');
				buf[3] = 0;
				}
			}

			decompile_put_str( x );
			}
		else
			if( ch == '\\' )
				decompile_put_char( '\\' );
			else
				decompile_put_char( ch );
		}
	}



static unsigned char *decompile
	(
	struct prognode *p,
	int larg,
	int indent,
	int depth
	)
	{
	/* Set up the string stream */
	if( p == 0 || p->p_proc == 0 )
		return u_str("");

	if( decompile_buffer == 0 )
		{
		decompile_buffer = malloc_ustr( BUFSIZ+1 );
		if( decompile_buffer == 0 )
			return 0;
		decompile_max = BUFSIZ;
		}
	decompile_buffer[0] = 0;
	decompile_used = 0;
	if( larg != 1 )
		{
		decompile_put_int( larg );
		}
	decompile_inner( p, depth, indent );
	if( indent )
		decompile_put_char( ctl('J') );

	decompile_buffer[ decompile_used ] = 0;
	return decompile_buffer;
	}

static void decompile_inner
	(
	struct prognode *p,
	int depth,
	int indent
	)
	{
	struct bound_name *n;
	int i;
	int lineargs;

	lineargs = 0;
	if( p == 0 )
		return;
	n = p->p_proc;
	if( p == trace_cur_exec )
		current_line = decompile_used;
	if( depth > 1 )
		if( n == &bound_number_node )
			{
			decompile_put_int( p->p_args[0].pa_int );
			return;
			}
		else if( n == &bound_string_node )
			{
			decompile_put_char( '"' );
			decompile_string( &p->p_args[1].pa_char, p->p_args[0].pa_int );
			decompile_put_char( '"' );
			return;
			}
		else if( n == &bound_variable_node )
			{
			decompile_put_str(p->p_args[0].pa_name->v_name  );
			return;
			}

	if( indent != 0 )
		for( i=1; i<=depth-1; i += 1 )
			decompile_put_str(u_str("    ") );
	decompile_put_char( '(' );
	decompile_put_str( n->b_proc_name );
	if( indent != 0 )
		for( i=0; i<=p->p_nargs - 1; i += 1 )
			{
			struct prognode *argp;
			struct bound_name *argn;

			argp = p->p_args[i].pa_node;
			argn = argp->p_proc;
			if( argn != &bound_number_node
			&& argn != &bound_string_node
			&& argn != &bound_variable_node )
				{
				lineargs = 1;
				break;
				}
			}
	for( i=0; i<=p->p_nargs - 1; i += 1 )
		{
		struct prognode *argp;
		struct bound_name *argn;
		int j;

		argp = p->p_args[i].pa_node;
		argn = argp->p_proc;
		decompile_put_char( lineargs ? ctl('J') : ' ' );
		if( lineargs
		&&	(argn == &bound_number_node ||
			argn == &bound_string_node ||
			argn == &bound_variable_node) )
			for( j=1; j<=depth; j += 1 )
				decompile_put_str( u_str("    ") );
		decompile_inner( p->p_args[i].pa_node, depth + 1, indent );
		}
	if( lineargs )
		{
		decompile_put_char( ctl('J') );
		for( i=1; i<=depth - 1; i += 1 )
			decompile_put_str( u_str("    ") );
		}
	decompile_put_char( ')' );
	}

/*
 * external-defun functions.
 *
 * These include the function to activate the users code, and the function
 * to process any call backs to EMACS that the user may want to make.
 */
#if defined(vms) && defined(CALL_BACK)
#include <em_user.h>

static int vms_to_c( struct dsc$descriptor *str )
	{
	unsigned short int len;
	unsigned char *addr;
	int resp;
	if( ! (resp = VMS_SUCCESS(lib$analyze_sdesc( str, &len, &addr ))) )
		{
		error( u_str("Error `%s' during EMACS call back"),
				fetch_os_error( resp ) );
		return resp;
		}
	if( call_back_len < len + 1 )
		{
		if( call_back_str != NULL )
			free( call_back_str );
		if( (call_back_str = malloc_ustr( len + 1)) == NULL )
			{
			call_back_len = 0;
			error( u_str("Out of memory during EMACS call back") );
			return SS$_INSFMEM;
			}
		else
			call_back_len++;
		}
	memcpy( call_back_str, addr, len );
	call_back_str[len] = 0;
	return SS$_NORMAL;
	}



static int activate_handler
	(
	struct chf$signal_array *sig,
	struct chf$mech_array *mech
	)
	{
	int cond;
	cond = sig->chf$l_sig_name;

	/* ignore info, success and warnings */
	if( (cond&1)
	|| (cond & STS$M_SEVERITY) == STS$K_WARNING )
		return SS$_CONTINUE;

#if defined(vax)
	mech->chf$l_mch_savr0 = cond;
#elif defined(alpha)
	mech->chf$q_mch_savr0 = cond;
#endif

	/* Make the faulting routine return the error status */
#if defined(vax)
	sys$unwind( &mech->chf$l_mch_depth, NULL );
#elif defined(__ALPHA)
	sys$unwind( (long int *)&mech->chf$q_mch_depth, NULL );
#else
	#error "what goes here?"
#endif

	return SS$_NORMAL;
	}

static int call_back (int function, ... )
	{
	int rv;
	va_list args;

	va_start( args, function );

	switch( function)
	{
	case EMACS__K_MESSAGE:
	/* 	message-text-desc */
		{
		struct prognode *old_cur_exec;
		old_cur_exec = cur_exec;
		if( ! err )
			{
			if( ! (rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) ) ) )
				return rv;
			cur_exec = 0;
			message( u_str("%s"), call_back_str );
			cur_exec = old_cur_exec;
			}
		return SS$_NORMAL;
		}

	case EMACS__K_ERROR:
	/* 	message-text-desc */
		{
		err = 0;
		if( ! ( rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) ) ) )
			return rv;
		error( u_str("%s"), call_back_str );
		return SS$_NORMAL;
		}
	case EMACS__K_SITFOR:
	/* 	tenths-of-seconds */
		{
		sit_for_inner( va_arg( args, int ) );
		return SS$_NORMAL;
		}
	case EMACS__K_ARG_COUNT:
	/* 	address of int to receive arg count */
		{
		int *p = va_arg( args, int * );

		*p = (( cur_exec != 0 ) ?  cur_exec->p_nargs : 0);
		return SS$_NORMAL;
		}
	case EMACS__K_CHECK_ARGS:
	/* 	min-args max-args */
		{
		int min_val = va_arg( args, int );
		int max_val = va_arg( args, int );
		if( check_args( min_val, max_val ) )
			return SS$_BADPARAM;
		else
			return SS$_NORMAL;
		}

	case EMACS__K_STRING_ARG:
	/* 	argument-number desc-to-store-string-in int to
	 *	receive string length */
		if( string_arg( va_arg( args, int ) ) == 0 )
			return 0;
		else
			{
			struct dsc$descriptor *dst;
			unsigned short int len;
			unsigned char *addr;
			dst = va_arg( args, struct dsc$descriptor * );

			len = (unsigned short)ml_value->exp_int;
			if( ! (rv = VMS_SUCCESS(str$copy_r
					(dst,
					&len,
					ml_value->exp_v.v_string ))) )
				error( u_str("Error `%s' reported by EMACS call back" ),
					fetch_os_error( rv ) );
			else
				{
				unsigned short int *p = va_arg( args, unsigned short int * );
				lib$analyze_sdesc( dst, &len, &addr );
				rv = SS$_NORMAL;

				*p = min( (unsigned short)ml_value->exp_int, len );
				}
			void_result();
			return rv;
			}

	case EMACS__K_NUMERIC_ARG:
	/* 	argument-number int to receive numeric value */
		{
		rv = numeric_arg( va_arg( args, int ) );
		if( err )
			return SS$_BADPARAM;
		else
			{
			int *p = va_arg( args, int * );
			*p = rv;
			void_result();
			return SS$_NORMAL;
			}
		}

	case EMACS__K_STRING_RESULT:
	/* 	desc-of-result */
		{
		unsigned char *ptr;
		unsigned char *addr;
		unsigned short int len;
		if( ! ( rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr )) ) )
			{
			error( u_str("Error `%s' during EMACS call back"),
				fetch_os_error( rv ) );
			return rv;
			}
		if( (ptr = malloc_ustr( len + 1 )) == NULL )
			{
			error( u_str("Out of memory during EMACS call back") );
			return SS$_INSFMEM;
			}
		memcpy( ptr, addr, len );
		ptr[len] = 0;
		release_expr( call_back_result );
		call_back_result->exp_type = ISSTRING;
		call_back_result->exp_int = len;
		call_back_result->exp_v.v_string = ptr;
		call_back_result->exp_release = 1;
		return SS$_NORMAL;
		}

	case EMACS__K_NUMERIC_RESULT:
	/* 	result	: signed long */
		{
		release_expr( call_back_result );
		call_back_result->exp_type = ISINTEGER;
		call_back_result->exp_int = va_arg( args, int );
		call_back_result->exp_v.v_string = 0;
		call_back_result->exp_release = 0;
		return SS$_NORMAL;
		}
	case EMACS__K_ALLOCATE:
	/* 	size-to-allocate int to hold buffer address */
		{
		void *mem;
		int size = va_arg( args, int );
		void **p = va_arg( args, void ** );

		if( (mem = malloc_ustr( size )) == NULL )
			{
			*p = NULL;
			return SS$_INSFMEM;
			}
		else
			{
			*p = mem;
			return SS$_NORMAL;
			}
		}

	case EMACS__K_REALLOCATE:
	/* 	old-buffer-address new-size int to hold buffer
	 *	address */
	{
	void *new_mem;
	void *old_mem = va_arg( args, void * );
	int size = va_arg( args, int );
	void **p = va_arg( args, void ** );
	if( (new_mem = realloc_ustr( old_mem, size )) == NULL )
		{
		*p = NULL;
		return SS$_INSFMEM;
		}
	else
		{
		*p = new_mem;
		return SS$_NORMAL;
		}
	}


	case EMACS__K_FREE:
	/* 	buffer-to-free */
		{
		free( va_arg( args, void * ) );
		return SS$_NORMAL;
		}

	case EMACS__K_BUFFER_SIZE:
	/* 	int to hold buffer size */
		{
		int *p = va_arg( args, int * );
		*p = num_characters + 1 - first_character;
		return SS$_NORMAL;
		}

	case EMACS__K_DOT:
	/* 	int to hold numeric-value of dot desc to return
	 *	buffer-name in int to hold buffer-name length! */
		{
		int *p_int = va_arg( args, int * );
		struct dsc$descriptor *p_str = va_arg( args, struct dsc$descriptor * );
		unsigned short int *p_size = va_arg( args, unsigned short int * );

		unsigned short int len;
		unsigned char * addr;
		unsigned short int maximum;
		*p_int = dot;
		len = _str_len( bf_cur->b_buf_name );
		if( ! (rv = VMS_SUCCESS(str$copy_r
				(
				p_str,
				&len,
				bf_cur->b_buf_name))) )
			return rv;
		lib$analyze_sdesc( p_str, &maximum, &addr );
		*p_size = min( len, maximum );
		return SS$_NORMAL;
		}

	case EMACS__K_MARK:
	/* 	int to hold numeric-value of dot desc to return
	 *	buffer-name in int to hold buffer-name length */
		{
		int *p_int = va_arg( args, int * );
		struct dsc$descriptor *p_str = va_arg( args, struct dsc$descriptor * );
		unsigned short int *p_size = va_arg( args, unsigned short int * );

		unsigned short int len;
		unsigned char *addr;
		int pos;
		unsigned short int maximum;
		struct marker *m;
		m = bf_cur->b_mark;
		rv = SS$_NORMAL;
		if( m == NULL )
			{
			pos = 0;
			error( u_str("No mark set in buffer \"%s\"!"), bf_cur->b_buf_name );
			rv = SS$_IVSSRQ;
			}
		else
			pos = to_mark( m );
		*p_int = pos;
		len = _str_len( bf_cur->b_buf_name );
		if( ! (rv = VMS_SUCCESS(str$copy_r
				(
				p_str,
				&len,
				bf_cur->b_buf_name))) )
			return rv;
		lib$analyze_sdesc( p_str, &maximum, &addr );
		p_str->dsc$w_length = min( len, maximum );
		return rv;
		}

	case EMACS__K_SET_MARK:
	/* 	true - set, false - unset */
		{
		if( va_arg( args, int ) )
			{
			if( bf_cur->b_mark == 0 )
				bf_cur->b_mark = new_mark();
			set_mark( bf_cur->b_mark, bf_cur, dot, 0 );
			cant_1line_opt = 1;
			}
		else
			un_set_mark_command();
		return SS$_NORMAL;
		}

	case EMACS__K_USE_BUFFER:
	/* 	buffer-name-desc */
		{
		if( ! (rv = vms_to_c( va_arg( args, struct dsc$descriptor * ) )) )
			return rv;
		set_bfn( call_back_str );
		return SS$_NORMAL;
		}

	case EMACS__K_GOTO_CHARACTER:
	/* 	position-to-goto */
		{
		int pos;
		pos = va_arg( args, int );
		if( pos < 1 )
			pos = 1;
		if( pos > num_characters )
			pos = num_characters + 1;
		set_dot( pos );
		return SS$_NORMAL;
		}

	case EMACS__K_INSERT_CHARACTER:
	/* 	char to insert */
		{
		unsigned char c = va_arg( args, unsigned char );
		ins_cstr( &c, 1 );
		return SS$_NORMAL;
		}

	case EMACS__K_INSERT_STRING:
	/* 	desc of string to insert */
		{
		unsigned short int len;
		unsigned char *addr;
		if( ! ( rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr ))) )
			{
			error( u_str("Error `%s' during EMACS call back"), fetch_os_error( rv ) );
			return rv;
			}
		ins_cstr( addr, len );
		return SS$_NORMAL;
		}

	case EMACS__K_DELETE_CHARACTERS:
	/* 	number of chars to delete, -iv means backwards */
		{
		int cnt;
		int repeat;
		int forw;
		forw = 1;
		cnt = va_arg( args, int );
		if( cnt < 0 )
			{
			cnt = -cnt;
			forw = 0;
			}
		if( (repeat = del_chars_in_buffer( dot, cnt, forw )) != 0 )
			if( forw )
				dot_right( repeat );
		if( ! forw )
			dot_left( cnt );
		return SS$_NORMAL;
		}

	case EMACS__K_FETCH_CHARACTER:
	/* 	position of required char int to hold char at dot */
		{
		int pos = va_arg( args, int );
		int *p = va_arg( args, int * );

		if( pos < num_characters && pos > 0 )
			*p = char_at( pos );
		else
			*p = -1;

		return SS$_NORMAL;
		}

	case EMACS__K_RETURN_STRING_TO_PARAM:
	/* 	argument-number string-desc */
		{
		unsigned short int len;
		unsigned char *addr;
		struct prognode *p;
		int argnum;
		argnum = va_arg( args, int ) - 1;

		if( ! (rv = VMS_SUCCESS(lib$analyze_sdesc( va_arg( args, struct dsc$descriptor * ), &len, &addr ))) )
			{
			error( u_str("Error `%s' during EMACS call back"), fetch_os_error (rv) );
			return rv;
			}
		if( cur_exec == 0
		|| argnum < 0
		|| argnum >= cur_exec->p_nargs )
			{
			error( u_str("Error, Argument number %d out of range"), argnum );
			return SS$_BADPARAM;
			}
		p = cur_exec->p_args[argnum].pa_node;
		if( p->p_proc != &bound_variable_node )
			{
			error( u_str("Emacs Call back expects argument %d to be a variable name."), argnum+1 );
			return SS$_BADPARAM;
			}
		if( len == 0 )
			addr = u_str("");
		perform_set( p->p_args[0].pa_name, 0, addr, 0, 0, len );
		return SS$_NORMAL;
		}

	case EMACS__K_RETURN_NUMBER_TO_PARAM:
	/* 	argument-number integer */
		{
		struct prognode *p;
		int argnum;
		argnum = va_arg( args, int ) - 1;

		if( cur_exec == 0
		|| argnum < 0
		|| argnum >= cur_exec->p_nargs )
			{
			error( u_str("Error, Argument number %d out of range"), argnum+1 );
			return SS$_BADPARAM;
			}
		p = cur_exec->p_args[argnum].pa_node;
		if( p->p_proc != &bound_variable_node )
			{
			error( u_str("Emacs Call back expects argument %d to be a variable name."), argnum+1 );
			return SS$_BADPARAM;
			}
		perform_set( p->p_args[0].pa_name, 0, 0, 0, 0, va_arg( args, int ) );
		return SS$_NORMAL;
		}

	case EMACS__K_BUFFER_EXTENT: /* s1, p1, s2, p2 */
		*va_arg( args, int *) = bf_s1;
		*va_arg( args, unsigned char * *) = bf_p1;
		*va_arg( args, int *) = bf_s2;
		*va_arg( args, unsigned char * *) = bf_p2;
		break;

	case EMACS__K_GAP_TO: /* position */
		gap_to( va_arg( args, int ) );
		break;

	case EMACS__K_GAP_ROOM: /* size */
		gap_room( va_arg( args, int ) );
		break;

	default:
		return SS$_BADPARAM;
	}

	return SS$_NORMAL;
	}

static int activate_image
	(
	unsigned char *image,
	unsigned char *symbol,
	int (**reference)(int (*call_back)(int,...), void *, int *),
	unsigned char *file
	)
	{
	int rv;
	struct dsc$descriptor imagename;
	struct dsc$descriptor symbolname;
	struct dsc$descriptor filename;
	struct vms_itm_3 set_logical[2];
	unsigned int attr =LNM$M_CONFINE;

	lib$establish( activate_handler );

	DSC_SZ( imagename, image );
	DSC_SZ( symbolname, symbol );
	DSC_SZ( filename, file );

	/* If the image is different from the filename define a logical * name
	 * to point at the file! */
	if( imagename.dsc$w_length != filename.dsc$w_length )
		{
		struct dsc$descriptor idesc;
		struct dsc$descriptor fdesc;
		struct dsc$descriptor tabnam = { 11,0,0,"LNM$PROCESS" };
		char istring[255];
		char fstring[255];

		idesc.dsc$w_length = imagename.dsc$w_length;
		idesc.dsc$b_dtype = 0;
		idesc.dsc$b_class = 0;
		idesc.dsc$a_pointer = istring;

		fdesc.dsc$w_length = filename.dsc$w_length;
		fdesc.dsc$b_dtype = 0;
		fdesc.dsc$b_class = 0;
		fdesc.dsc$a_pointer = fstring;

		/* upcase the strings to keep VMS happy */
		str$upcase( &fdesc, &filename );
		str$upcase( &idesc, &imagename );

		memset( set_logical, 0, sizeof( set_logical ) );
		set_logical[0].w_item_code = LNM$_STRING;
		set_logical[0].a_buf_addr = u_str(fstring);
		set_logical[0].w_length = fdesc.dsc$w_length;

		if( !VMS_SUCCESS(sys$crelnm
			(
			&attr,
			&tabnam,
			&idesc,
			NULL,
			set_logical
			)) )
			{
			error( u_str("Error defining logical name %s as %s"), image, file );
			return 0;
			}
		}
	rv = lib$find_image_symbol( &imagename, &symbolname, (void **)reference );
	if( ! VMS_SUCCESS(rv) )
		{
		error( u_str("Error while defining %s in image %s, %s" ),
				symbol, image, fetch_os_error( rv ) );
		return 0;
		}
	return 1;
	}

/*
 * Activate the required bit of code.
 *
 * check_ to see when the code was last merge activiated[ This ]is coded in the
 * extern_func block which is used to contain information about the external
 * function. Offset ef_restore_count is set to the value that was the
 * value of the is_restored variable when LIB$FIND_IMAGE_SYMBOL was last
 * called. If ef_restore_count isnt correct, then its time to re-activate
 * the code.
 *
 * The function is called, and the result accepted as a standrad VAX/VMS
 * return status code[ For ]success, EMACS continues to execute[ Otherwise ]
 * the return status is used to indicate that EMACS is to exit.
 */

static int activate_external_function( struct bound_name *b )
	{
	int rv;
	struct extern_func *ef;
	int isrest;
	struct expression *old_result;
	struct expression result;

	old_result = call_back_result;
	ef = b->b_bound.b_func;

	lib$establish( activate_handler );

	call_back_result = &result;
	result.exp_type = ISVOID;
	result.exp_int = 0;
	result.exp_v.v_string = NULL;
	result.exp_release = 0;
	if( ef->ef_restore_count != is_restored )
		if( ! (activate_image
			(
			images[ ef->ef_image_index ],
			ef->ef_funcname,
			&ef->ef_function,
			image_filenames[ ef->ef_image_index ])) )
				return 0;
		else
			ef->ef_restore_count = is_restored;
	if( image_context_lost )
		ef->ef_context = 0;
	isrest = is_restored;
	rv = ef->ef_function( call_back, ef->ef_context, &isrest );
	if( ! VMS_SUCCESS(rv) )
		{
		error( u_str("Function %s failed with status %s" ),
				b->b_proc_name, fetch_os_error( rv ) );
		void_result();
		}
	else
		{
		*ml_value = result;
		call_back_result = old_result;
		}
	return 0;
	}
#endif

int list_images( void )
	{
	struct emacs_buffer *old;

	if( image_context_lost )
		{
		error( u_str("Image contexts lost due to memory allocation failure") );
		return 0;
		}

	old = bf_cur;
	scratch_bfn( u_str("Image list"), interactive );

	if( image_count  <= 0 )
		ins_str( u_str("No images in use.\n") );
	else
		{
		int i;

		ins_str(u_str("   Image Name\t\t\t     Context Value  Filename\n"));
		ins_str(u_str("   ----- ----\t\t\t     ------- -----  --------\n"));

		for( i=0; ; i++ )
			{
			unsigned char line[52 + 255 + 1];
			if( images[i] == 0 ) break;
			sprintfl
			(
			line, sizeof( line ) - 1,
			u_str("   %-33s 0X%X     %s\n" ),
			images[i], image_contexts[i], image_filenames[i] );
			ins_str( line );
			}
		}
	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur);

	return 0;
	}

void init_opt( void )
	{
	struct expression *e;
	struct variablename *v;
	struct binding *b;

	v = lookup_variable( u_str("current-function") );
	if( v != 0 )
		{
		b = v->v_binding;
		e = b->b_exp;
		e->exp_int = 0;
		e->exp_v.v_string = u_str("");
		current_function = &e->exp_v.v_string;
		current_function_length = &e->exp_int;
		}
	v = lookup_variable( u_str("current-breakpoint") );
	if( v != 0 )
		{
		b = v->v_binding;
		e = b->b_exp;
		e->exp_int = 0;
		e->exp_v.v_string = u_str("");
		current_break = &e->exp_v.v_string;
		current_break_length = &e->exp_int;
		}
	v = lookup_variable( u_str("last-expression") );
	if( v != 0 )
		{
		b = v->v_binding;
		b->b_exp = &last_expression;
		b->b_is_system = 0;
		last_expression.exp_type = ISINTEGER;
		}
	}
