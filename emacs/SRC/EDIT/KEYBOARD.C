/*	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/* keyboard manipulation primitives */

#include <emacs.h>


/*forward*/ struct bound_name *get_keymap_binding( struct keymap *kmap, int c );
/*forward*/ static struct bound_name **get_keymap_binding_address( struct keymap *kmap, int c );
/*forward*/ void add_keymap_binding( struct keymap *kmap, int c, struct bound_name *proc );
/*forward*/ struct keymap *define_keymap( unsigned char *name );
/*forward*/ int process_key( void );
/*forward*/ int get_char( void );
/*forward*/ struct bound_name **lookup_keys( struct keymap *kmap, unsigned char *keys, int len );
/*forward*/ int start_remembering( void );
/*forward*/ int stop_remembering( void );
/*forward*/ void exec_str( unsigned char *s );
/*forward*/ int execute_keyboard_macro( void );
/*forward*/ int push_back_character( void );
/*forward*/ int top_level_command( void );
/*forward*/ int recursive_edit( void );
/*forward*/ int recurse_depth( void );
/*forward*/ int interrupt_emacs( void );
/*forward*/ void keyboard_interrupt(struct trmcontrol *,int character);
/*forward*/ int convert_key_string_command( void );
/*forward*/ int convert_key_string( int inlen, /* length of input string */ unsigned char *inptr, /* address of input string */ int *outlen, /* return output length here */ unsigned char *outptr/* address of output string */ );
/*forward*/ void init_key( void );
/*forward*/ void re_init_keyboard( void );

/*
 *	Global Data definitions
 */

int wake_queued = 0;
GLOBAL SAVRES int activity_indicator;
GLOBAL SAVRES unsigned char *mem_ptr;
volatile int keyboard_input_pending;
volatile int input_pending;
volatile int timer_interrupt_occurred;
volatile int interrupt_key_struck;
volatile int pending_channel_io;
unsigned char activity_character = 'x';
struct bound_name interrupt_block;
GLOBAL SAVRES int checkpoint_frequency;
GLOBAL SAVRES int end_of_mac;
struct keymap *next_global_keymap;
struct keymap *next_mode_keymap;
struct keymap *next_local_keymap;
GLOBAL SAVRES struct keymap *current_global_map;
GLOBAL SAVRES struct keymap *global_map;
GLOBAL SAVRES struct keymap *minibuf_local_ns_map;
GLOBAL SAVRES struct keymap *minibuf_local_map;

GLOBAL SAVRES struct char_element char_cells[CHARACTER_QUEUE_SIZE];

SAVRES struct queue free_queue;
SAVRES struct queue push_back_queue;
SAVRES struct queue input_queue;
SAVRES struct queue emacs_work_queue;

/* A keyboard called procedure returns:
 *	 1 to continue interpretation
 *	 0 normally
 *	-1 to quit
 *	-2 if this proc has set arg & arg should be handled by next command
 */

static int keystrokes;		/* The number of keystrokes since the last checkpoint. */
static int can_checkpoint;	/* True iff we are allowed to checkpoint now. */

#define queue_remove_char(e) (struct char_element *)queue_remove(e)


/*
 *	routines to process keymaps
 */
struct bound_name *get_keymap_binding
	(
	struct keymap *kmap,
	int c
	)
	{
	struct keymap_short *smap;
	int i;

	switch(( kmap->k_type ) ) {
	case KEYMAP_SHORT_TYPE:
		{
		smap = kmap->k_map.k_short;
		for( i=0; i<=smap->k_used - 1; i += 1 )
			if(( smap->k_chars[i] == (unsigned char)c ) )
				return smap->k_sbinding[i];

		return 0;
		}
	case KEYMAP_LONG_TYPE:
		return kmap->k_map.k_long->k_binding[c];
	default:
		return 0;
	}
	}

static struct bound_name **get_keymap_binding_address
	(
	struct keymap *kmap,
	int c
	)
	{
	struct keymap_short *smap;
	int i;

	switch(( kmap->k_type ) ) {
	case KEYMAP_SHORT_TYPE:
		{
		smap = kmap->k_map.k_short;
		for( i=0; i<=smap->k_used - 1; i += 1 )
			if(( smap->k_chars[i] == (unsigned char)c ) )
				return &smap->k_sbinding[i];

		return 0;
		}
	case KEYMAP_LONG_TYPE:
		return &kmap->k_map.k_long->k_binding[c];
	default:
		return 0;
	}
	}

void add_keymap_binding
	(
	struct keymap *kmap,
	int c,
	struct bound_name *proc
	)
	{
	struct keymap_short *smap;
	int i;

	switch( kmap->k_type )
	{
	case KEYMAP_SHORT_TYPE:
		{
		smap = kmap->k_map.k_short;
		for( i=0; i<=smap->k_used - 1; i += 1 )
			if(( smap->k_chars[i] == (unsigned char)c ) )
				{
				if( smap->k_sbinding[i] != 0 )
					free_sexpr_defun( smap->k_sbinding[i] );
				smap->k_sbinding[i] = proc;
				return;
				}

		/* see if the short kmap is full */
		if(( smap->k_used < KEYMAP_SHORT_SIZE ) )
			{
			smap->k_chars[smap->k_used] = (unsigned char)c;
			smap->k_sbinding[smap->k_used] = proc;
			smap->k_used = smap->k_used + 1;
			return;
			}

		/* the kmap is full so allocate a long kmap */
		kmap->k_map.k_long = malloc_struct( keymap_long );
		if( kmap->k_map.k_long == NULL )
			{
			kmap->k_map.k_short = smap;
			return;
			}
		kmap->k_type = KEYMAP_LONG_TYPE;

		/* initialise long kmap prevent crash in save-env/restore-env */
		memset( kmap->k_map.k_long, 0, sizeof(struct keymap_long) );

		/* move bindings from short to long format */
		for( i=0; i<=KEYMAP_SHORT_SIZE - 1; i += 1 )
			add_keymap_binding
			(
			kmap,
			smap->k_chars[i],
			smap->k_sbinding[i]
			);

		free( smap );
		kmap->k_map.k_long->k_binding[c] = proc;
		break;
		}

	case KEYMAP_LONG_TYPE:
		{
		if( kmap->k_map.k_long->k_binding[c] != 0 )
			free_sexpr_defun( kmap->k_map.k_long->k_binding[c] );
		kmap->k_map.k_long->k_binding[c] = proc;
		break;
		}

	default:	;
	}
	}

struct keymap *define_keymap
	(
	unsigned char *name
	)
	{
	struct keymap *kmap;
	struct keymap_short *smap;

	kmap = malloc_struct( keymap );
	if( kmap == 0 ) return 0;
	kmap->k_type = KEYMAP_SHORT_TYPE;
	kmap->k_map.k_short = smap = malloc_struct( keymap_short );
	if( smap == 0 ) return 0;
	smap->k_used = 0;
	def_mac( name, kmap, -2, 0, 0 );

	return kmap;
	}


/* process_keys reads keystrokes and interprets them according to the
 *  given keymap and its inferior keymaps */

static struct keymap_short short_null_map;
static struct keymap null_map;

int process_keys( void )
	{
	struct keymap *kmap;
	unsigned char c;
	unsigned char *last_keys;

	last_keys = last_keys_struck;
	next_global_keymap = 0;
	next_local_keymap = 0;

	for(;;)
	 	{
#if DBG_ALLOC_CHECK
		/* test memory state at the heart of the emacs loop */
		if( dbg_flags&DBG_ALLOC_CHECK )
			emacs_heap_check();
#endif

		if( quitting_emacs != 0 )
			if( recursive_edit_depth != 0 )
				return 0;
			else
				quitting_emacs = 0;
		if( (next_global_keymap == 0) )
			{
			if( remembering )
				end_of_mac = mem_used;
			if( arg_state != have_arg
			&& mem_ptr == 0
			&& bf_cur != minibuf )
				undo_boundary();
			}
		if( bf_cur != minibuf )
			can_checkpoint = 1;
		{
		int ic;

		if( ((ic = get_char()) < 0) )
			{
			can_checkpoint = 0;
			return 0;
			}
		c = (unsigned char)ic;
		}
		can_checkpoint = 0;
		if(( last_keys < &last_keys_struck[LAST_KEYS_SIZE-1] ) )
			*last_keys++ = c;
		last_keys[0] = 0;

		if( (next_global_keymap == 0) )
			next_global_keymap = current_global_map;
		if( (next_local_keymap == 0) )
			next_local_keymap = bf_mode.md_keys;
		if( (wn_cur->w_buf != bf_cur) )
			set_bfp( wn_cur->w_buf );

		if( (kmap = next_local_keymap) != 0 )
			{
			struct bound_name *p;
			next_local_keymap = 0;
			if( (p = get_keymap_binding( kmap, c )) != 0 )
				{
				last_key_struck = c;
				if( (p->b_binding != KEYBOUND) )
					this_command = last_key_struck;
				if( (execute_bound( p ) < 0) )
					return 0;
				if( (arg_state != have_arg) )
					previous_command = this_command;
				/*
				 * if execute bound did something, start scanning a new
				 * key sequence.
				 */
				if( (next_local_keymap == 0) )
					{
					next_global_keymap = 0;
					last_keys = last_keys_struck;
					if(( bf_cur->b_process_key_proc != 0) )
						execute_bound_saved_environment( bf_cur->b_process_key_proc );
					continue;
					}
				}
			}
		if( (kmap = next_global_keymap) != 0 )
			{
			struct bound_name *p;
			struct keymap *lmap;

			lmap = next_local_keymap;
			next_global_keymap = 0;
			next_local_keymap = 0;
			if( (p = get_keymap_binding( kmap, c )) != 0 )
			 	{
				last_key_struck = c;
				if(( p->b_binding != KEYBOUND ) )
					this_command = last_key_struck;
				if(( p->b_binding == KEYBOUND || lmap == 0 ) )
					if( (execute_bound( p ) < 0) )
						return 0;
				if( (arg_state != have_arg) )
					previous_command = this_command;
				if( (next_local_keymap != 0) )
				 	{
					next_global_keymap = next_local_keymap;
					next_local_keymap = (( lmap != 0 ) ?  lmap : &null_map);
					}
				else
					{
					next_global_keymap = (( lmap != 0 ) ? &null_map : 0);
					next_local_keymap = lmap;
					last_keys = last_keys_struck;
					if(( bf_cur->b_process_key_proc != 0) )
						execute_bound_saved_environment( bf_cur->b_process_key_proc );
					}
				continue;
				}
			else
				{
				next_global_keymap = (( lmap != 0 ) ? &null_map : 0);
				next_local_keymap = lmap;
				}
			}
		if( next_local_keymap == 0 )
			{
			illegal_operation();
			last_keys = last_keys_struck;
			}
		else
			next_global_keymap = &null_map;
		}
	}


/*
 *	(setq key-processed (process-key (key-kmap) [key-result]))
 *
 *	process-key evals its first arg and expects a keymap as its
 *	result. The keymap is used to parse input key-result is set
 *	as the value, if any, returned by the function in the keymap.
 *	process-key returns a boolean value, true if the keymap was
 *	succesfully used. False otherwise.
 */
int process_key( void )
	{
	struct prognode *p;
	struct prognode *result_p = NULL;
	struct keymap *kmap;
	unsigned char c;
	unsigned char *last_keys;
	unsigned int rv;
	unsigned int key_parsed;

	p = cur_exec;
	last_keys = last_keys_struck;
	key_parsed = 1;
	if( ! term_is_terminal )
		error(u_str( "process-key can only be used with a terminal" ));

	if( check_args( 1, 2 ) != 0 )
		return 0;
	if( p->p_nargs >= 2 )
		{
		result_p = p->p_args[1].pa_node;
		if( result_p->p_proc != &bound_variable_node )
			{
			error( u_str( "process-key expects its 2nd argument to be a variable" ) );
			return( 0 );
			}
		}
	rv = exec_prog( p->p_args[0].pa_node );
	if( rv != 0 || err )
		return rv;

	if( next_local_keymap == 0 )
		{
		error(u_str( "process-key expects its 1st argument to be a keymap" ));
		return 0;
		}

	cur_exec = 0;
	cant_1line_opt = cant_1win_opt = 1;
	/*
	 *	While there are key maps process key strokes
	 */
	while( ( next_local_keymap != 0 ) )
		{
		struct bound_name *b;

		/* Save the current local kmap */
		kmap = next_local_keymap;

		/* get the get_char check_ for EOF conditions */
		{
		int ic;
		
		if( ((ic = get_char()) < 0) )
			{
			can_checkpoint = 0;
			break;
			}
		c = (unsigned char)ic;
		}
		if(( last_keys < &last_keys_struck[LAST_KEYS_SIZE-1] ) )
			*last_keys++ = c;
		last_keys[0] = 0;

		/*
		 *	Zero local kmap
		 *	find the function to call and call it
		 */
		next_local_keymap = 0;

		if( (b = get_keymap_binding( kmap, c )) != 0 )
			{
			last_key_struck = c;
			if( (b->b_binding != KEYBOUND) )
				this_command = last_key_struck;
			if( (execute_bound( b ) < 0) )
				break;
			if( arg_state != have_arg )
				previous_command = this_command;
			}
		else
			{
			/* parse error - return 0 */
			key_parsed = 0;
			break;
			}
		}

	/* see if there is a value from the function */
	if( p->p_nargs >= 2 && ! err )
		{
		struct variablename *v;
		struct binding *b;

		v = result_p->p_args[0].pa_name;
		b = v->v_binding;

		if( b == 0 )
			{
			error (u_str( "Attempt to set the unbound variable \"%s\"" ), v->v_name);
			return 0;
			}

		release_expr( b->b_exp);
		if(( ml_value->exp_type == ISVOID ) )
			perform_set( v, 0, 0, 0, 0, 0 );
		else
			copy_expression( ml_value, b->b_exp, b->b_is_system );
		}

	release_expr( ml_value );

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = key_parsed;
	cur_exec = p;
	redo_modes = cant_1line_opt = cant_1win_opt = 1;
	return 0;
	}


/* read a character from the keyboard; call the redisplay if needed */

static unsigned char parameter_chars[100];
static struct expression *parameter_expr;

int get_char( void )
	{
	int c = -1;
	int remember;
	int i;

	remember = remembering;

	{
	struct char_element *char_cell;
	{
	if( (char_cell = queue_remove_char( push_back_queue.next )) != NULL )
		{
		remember = 0;
		goto having_dequeued_a_char;
		}

	if( mem_ptr != 0 )
		{
		if( err || quitting_emacs )
			{
			mem_ptr = 0;
			return -1;
			}
		c = *mem_ptr++;
		if( c == 0x80 )
			{
			c = (*mem_ptr++) - 1;
			goto having_found_char;
			}
		if( c != 0 )
			goto having_found_char;

		/* run out of string so zap pointer */
		mem_ptr = 0;
		return -1;
		}

	if( input_pending == 0 )
		{
		set_activity_character( 'i' );

		do_dsp( 0 );
		if( checkpoint_frequency > 0
		&& can_checkpoint
		&& keystrokes > checkpoint_frequency )
			{
			checkpoint_everything();
			keystrokes = 0;
			}
		}

	keystrokes++;
	/*
	 *	This is the VMS EMACS scheduling loop -- we use
	 *		input_pending to count the number of things
	 *		(characters, timer interrupts, etc...)
	 *		that require attention and should be processed
	 *		BEFORE a redisplay occurs. We want the
	 *		sub-process channel input to be the lowest
	 *		priority item, though and we do not want it
	 *		to affect our decision to kill the redisplay
	 *		(This minimizes the impact of a lot of data
	 *		 coming into a buffer bound to a sub-process)
	 *		so we do not count pending_channel_io in
	 *		input_pending but process it separately once
	 *		everything else is done.
	 */
	for(;;)
		{
		wake_queued = 0;

		if( (char_cell = queue_remove_char( push_back_queue.next )) != NULL )
			{
			remember = 0;
			goto having_dequeued_a_char;
			}

		set_activity_character( 'i' );

		while( input_pending == 0
		&& queue_empty( &push_back_queue ) )
			{
			wake_queued = 0;

			if( pending_channel_io != 0 )
				{
				/*
				 *	If we really do something in here...
				 */
				if( process_channel_interrupts() )
					/*
					 *	then exit the loop.
					 */
					break;
				}

			interrupt_key_struck = 0;
			if( wait_for_activity() < 0 )
				return -1; 	/* we are quitting emacs */
			}

		set_activity_character( 'b' );

		/*
		 *	check for work other than fetching character input
		 */
		work_process_queue();

		while( timer_interrupt_occurred != 0 )
			process_timer_interrupts();

		/*
		 *	If we do not have any characters to process, do the
		 *		redisplay but do not touch the minibuffer
		 */
		if( (char_cell = queue_remove_char( push_back_queue.next )) != NULL )
			{
			remember = 0;
			goto having_dequeued_a_char;
			}

		if( (char_cell = queue_remove_char( input_queue.next )) != NULL )
			{
			interlock_dec( &input_pending );
			interlock_dec( &keyboard_input_pending );
			goto having_dequeued_a_char;
			}

		do_dsp( 1 );
		}
having_dequeued_a_char:	/* leave this block */
	;
	}

	switch( char_cell->ce_type )
	{
	case CE_TYPE_CHAR:
		c = char_cell->ce_char;
		break;
	case CE_TYPE_PAR_CHAR:
	case CE_TYPE_PAR_SEP:
	case CE_TYPE_FIN_CHAR:
		{
#define MAX_ARGS 32

		struct expression **e;
		struct emacs_array *a;
		unsigned char *last_p;
		unsigned char *p;
		struct expression *args[MAX_ARGS*2];

		int num_params;

		last_p = p = &parameter_chars[0];
		num_params = 0;

		while( char_cell->ce_type == CE_TYPE_PAR_SEP
		|| char_cell->ce_type == CE_TYPE_PAR_CHAR )
			{
			if( char_cell->ce_type == CE_TYPE_PAR_SEP )
				{
				if( num_params >= MAX_ARGS-1 )
					return get_char();

				*p++ = 0;
				args[ num_params ] = create_expression
						(
						ISSTRING,
						p - last_p,	/* length of string */
						savestr( last_p ),/* address of string */
						1		/* do not release */
						);
				*p++ = char_cell->ce_char;
				*p++ = 0;

				args[ num_params + MAX_ARGS ] = create_expression
						(
						ISSTRING,
						1,		/* length of string */
						savestr( p - 1 ),/* address of string */
						1		/* do not release */
						);
				last_p = p;
				num_params++;
				}
			else
				*p++ = char_cell->ce_char;

			/* insert at tail of queue */
			queue_insert( free_queue.prev, &char_cell->ce_queue );


			interlock_dec( &input_pending );
			interlock_dec( &keyboard_input_pending );
			if( (char_cell = queue_remove_char( input_queue.next )) == NULL )
				return get_char();	/* cannot happen error */
			}

		/*
		 *	The last parameter value
		 */
		*p++ = 0;
		args[ num_params ] = create_expression
				(
				ISSTRING,
				p - last_p,	/* length of string */
				savestr( last_p ),/* address of string */
				1		/* do not release */
				);

		a = create_array
			(
			2,	/* dimensions */
			2,	/* first dimension size */
			num_params + 1 + 1,/* second dimension size */
			1,
			0
			);
		if( a == 0 )
			return get_char();

		e = &a->array_expr[0];	/* first element */

		/* element 1,0 is the number of params */
		e[0] = create_expression( ISINTEGER, num_params+1, 0, 0 );
		e = &e[1];
		/* row 1 has the parameter strings */
		for( i=0; i<=num_params; i += 1 )
			{
			e[0] = args[i];
			e = &e[1];
			}
		e = &e[1];	/* skip 2,0 */

		/* row 2 has the separator strings */
		for( i=0; i<=num_params-1; i += 1 )
			{
			e[0] = args[MAX_ARGS + i];
			e = &e[1];
			}
		/* 2, .num_params is always zero */

		release_expr( parameter_expr );
		parameter_expr->exp_v.v_array = a;
		parameter_expr->exp_release = 1;

		c = char_cell->ce_char;
		break;
		}
	default:	invoke_debug();
	}

	/* insert at tail of queue */
	queue_insert( free_queue.prev, &char_cell->ce_queue );
having_found_char:	/* leave this block */
	;
	}

	if( c < 0 )
		return -1;

	/*
	 *	Have Character now remember it if nessesary
	 */
	if( remember )
		{
		if( c == 0 )
			{
			key_mem[ mem_used++ ] = 0x80;
			key_mem[ mem_used++ ] = 1;
			}
		else if( c == 0x80 )
			{
			key_mem[ mem_used++ ] = 0x80;
			key_mem[ mem_used++ ] = 0x81;
			}
		else
			key_mem[ mem_used++ ] = (unsigned char)c;

		if( mem_used >= MEMLEN )
			{
			error (u_str( "Keystroke memory overflow!" ));
			remembering = end_of_mac = mem_used = key_mem[0] = 0;
			}
		}

	set_activity_character( 'b' );

	return c;
	}


/* Given a keystroke sequence look up the bound_name that it is bound to */
struct bound_name **lookup_keys
	(
	struct keymap *kmap,
	unsigned char *keys,
	int len
	)
	{
	struct bound_name *b;

	while( kmap != 0 && (len = len - 1) >= 0 )
		{
		b = get_keymap_binding( kmap, keys[0] );
		if( len == 0 )
			return get_keymap_binding_address( kmap, keys[0] );
		keys++;
		if( b == 0 || b->b_binding != KEYBOUND )
			break;
		kmap = b->b_bound.b_keymap;
		}

	return 0;
	}

int start_remembering( void )
	{
	if( remembering )
		error (u_str( "Already remembering!" ));
	else
		{
		remembering = 1;
		mem_used = end_of_mac = 0;
		message(u_str( "remembering..." ));
		}
	return 0;
	}

int stop_remembering( void )
	{
	if( remembering )
	 	{
		remembering = 0;
		key_mem[ end_of_mac ] = 0;
		mem_used = end_of_mac;
		message(u_str( "Keyboard macro defined." ));
		}
	return 0;
	}

/* Execute the given command string */
void exec_str
	(
	unsigned char *s
	)
	{
	unsigned char *old;
	old = mem_ptr;
	mem_ptr = s;
	redo_modes = cant_1line_opt = cant_1win_opt = 1;
	process_keys();
	redo_modes = cant_1line_opt = cant_1win_opt = 1;
	mem_ptr = old;
	}

int execute_keyboard_macro( void )
	{
	if( remembering )
		error (u_str( "Sorry, you cannot call the keyboard macro while defining it." ));
	else if( mem_used == 0 )
		error (u_str( "No keyboard macro to execute." ));
	else
		{
		int i;

		i = arg;
		arg = 0;
		arg_state = no_arg;

		do
			{
			exec_str( &key_mem[0] );
			}
		while( !err && (i = i - 1) > 0 );
		}
	return 0;
	}

int push_back_character( void )
	{
	struct char_element *char_cell;
	unsigned char *n;
	int i;

	n = get_key( global_map, u_str( ": push-back-character " ) );
	if( ! err && n != 0 )
		for( i=ml_value->exp_int - 1; i>=0; i-- )
			{
			if( (char_cell = queue_remove_char( free_queue.next )) == NULL )
				return 0;
			char_cell->ce_char = n[i];
			char_cell->ce_type = CE_TYPE_CHAR;

			/* insert at head of queue */
			queue_insert( &push_back_queue, &char_cell->ce_queue );
			}
	return 0;
	}

int top_level_command( void )
	{
	quitting_emacs = 1;
	return 0;
	}

int recursive_edit( void )
	{
	struct prognode *oldp;

	oldp = cur_exec;

	if( term_is_terminal )
		{
		cur_exec = 0;
		recursive_edit_depth++;
		redo_modes = cant_1line_opt = cant_1win_opt = 1;

		process_keys ();

		recursive_edit_depth--;
		cur_exec = oldp;
		redo_modes = cant_1line_opt = cant_1win_opt = 1;
		}
	return 0;
	}

int recurse_depth( void )
	{
	release_expr( ml_value );

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = recursive_edit_depth;
	return 0;
	}


/*
 *	interrupt binding routine -- If a key is bound to this procedure
 *	then it is capable of causing an interrupt and when it
 *	invokes this procedure it will generate an Illegal Operation.
 */
int interrupt_emacs( void )
	{
	interrupt_key_struck = 0;
	return 0;
	}

/*
 *	Descriptor for "interrupt-key", so that keyboard_interrupt can
 *	determine if the key struck is bound to "interrupt-key".
 */
int debug_break_char = -1;	/* 29 == ^] */

GLOBAL SAVRES int cs_enabled;
GLOBAL SAVRES int cs_modified;
GLOBAL SAVRES int cs_cvt_f_keys;
GLOBAL SAVRES int cs_cvt_mouse;
GLOBAL SAVRES unsigned char cs_cvt_8bit_string[256];
GLOBAL SAVRES unsigned char cs_cvt_csi_string[256];
GLOBAL SAVRES unsigned char cs_par_char_string[256];
GLOBAL SAVRES unsigned char cs_par_sep_string[256];
GLOBAL SAVRES unsigned char cs_int_char_string[256];
GLOBAL SAVRES unsigned char cs_fin_char_string[256];

GLOBAL SAVRES unsigned char cs_attr[256];

static int cs_bits
	(
	unsigned char *chars,
	int bitmask
	)
	{
	int ch;
	unsigned char *p;
	int i;

	cs_modified++;

	/*
	 *	init this bit position to zero
	 */
	for( i=0; i<=255; i += 1 )
		cs_attr[i] &= ~bitmask;
	/*
	 *	set the bit for the chars specified
	 */
	p = &chars[0];
	while( (ch = (int)*p++) != 0 )
		cs_attr[ch] |= bitmask;

	return 1;
	}

int check_cs_cvt_8bit(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_CVT_8BIT );
	}

int check_cs_cvt_csi(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_CVT_CSI );
	}

int check_cs_par_char(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_PAR_CHAR );
	}

int check_cs_par_sep(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_PAR_SEP );
	}

int check_cs_int_char(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_INT_CHAR );
	}

int check_cs_fin_char(unsigned char *value, struct variablename *PNOTUSED(v))
	{
	return cs_bits( value, M_CS_FIN_CHAR );
	}


#define CSI_ST_NORMAL 0
#define CSI_ST_ESC 1
#define CSI_ST_CSI 2

struct char_element *_q_char( unsigned char value, int type )
	{
	struct char_element *char_cell;
	/*
	 *	queue the char
	 */
	if( (char_cell = queue_remove_char( free_queue.next )) != NULL )
		{
		/*
		 *	wake up get_char if this is the first char in the list
		 */
		char_cell->ce_char = value;
		char_cell->ce_type = (unsigned char)type;
		interlock_inc( &input_pending );
		interlock_inc( &keyboard_input_pending );

		/* insert at tail of queue */
		queue_insert( input_queue.prev, &char_cell->ce_queue );

		conditional_wake();

		return char_cell;
		}

	return 0;
	}

void keyboard_interrupt( struct trmcontrol *PNOTUSED(tt), int character )
	{
	struct char_element *char_cell;
	unsigned char ch;
	struct bound_name *proc = NULL;

	static int csi_state = CSI_ST_NORMAL;
	static unsigned char hold[100];
	static unsigned char *hold_put_ptr;

	ch = (unsigned char)character;
	proc = 0;

	if( character == debug_break_char )
		invoke_debug();

	while( cs_enabled )
		{
		switch( csi_state)
		{
		case CSI_ST_NORMAL:
			{
			if( cs_attr[ch]&M_CS_CVT_CSI )
				ch	= 0x9b;	/* CSI */

			switch( ch )
			{
			case 0x1b:
				{
				csi_state = CSI_ST_ESC;
				return;
				}
			case 0x9b:
				{
				csi_state = CSI_ST_CSI;
				hold_put_ptr = &hold[0];
				return;
				}
			default:	goto exit_loop;
			}
			break;
			}
		case CSI_ST_ESC:
			{
			if( cs_attr[ch]&M_CS_CVT_8BIT )
				{
				csi_state = CSI_ST_NORMAL;
				ch &= 0x1f;
				ch |= 0x80;	/* convert 7bit to 8bit */
				}
			else
				{
				_q_char( 0x1b, CE_TYPE_CHAR );
				csi_state = CSI_ST_NORMAL;
				goto exit_loop;
				}
			break;
			}
		case CSI_ST_CSI:
			{
			if( cs_attr[ch]&M_CS_PAR_SEP
			|| cs_attr[ch]&M_CS_PAR_CHAR
			|| cs_attr[ch]&M_CS_INT_CHAR )
				{
				if( hold_put_ptr >= &hold[sizeof(hold)-1] )
					{
					/*
					 * ignore chars on overflow
					 */
					csi_state = CSI_ST_NORMAL;
					return;
					}
				*hold_put_ptr++ = ch;
				return;
				}
			if( cs_attr[ch]&M_CS_FIN_CHAR )
				{
				int fin_char;

				if( ch == '~' && cs_cvt_f_keys )
					{
					int key_num;
					unsigned char *hold_get_ptr;

					hold_get_ptr = &hold[0];
					key_num = 0;
					while( hold_get_ptr != hold_put_ptr )
						{
						ch = *hold_get_ptr++;
						if( ch < '0' || ch > '9' )
							{
							_q_char( 0x9b, CE_TYPE_CHAR );
							goto process_f_keys;
							}
						key_num = key_num * 10 + ch - '0';
						}
					_q_char( 0x80, CE_TYPE_CHAR );
					_q_char( (unsigned char)(key_num + ' '), CE_TYPE_CHAR );
					csi_state = CSI_ST_NORMAL;
					return;
process_f_keys:
					;
					}
				else
				if( cs_cvt_mouse
				&& ch == 'w'
				&& &hold[0] != hold_put_ptr
				&& hold_put_ptr[-1] == '&' )
					{
					int event_num;
					unsigned char *hold_get_ptr;

					hold_get_ptr = &hold[0];
					event_num = 0;
					while( hold_get_ptr != hold_put_ptr )
						{
						ch = *hold_get_ptr++;
						if( ch < '0' || ch > '9' )
							break;
						event_num = event_num * 10 + ch - '0';
						}
					_q_char( 0x81, CE_TYPE_CHAR );
					hold_put_ptr--;/* lose the "&" */
					ch = (unsigned char)(event_num + 'A');
					csi_state = CSI_ST_NORMAL;
					}
				else
					_q_char( 0x9b, CE_TYPE_CHAR );

				{
				unsigned char par_char;
				unsigned char *hold_get_ptr;

				/*
				 *	Find out how many parameters are present
				 */
				fin_char = CE_TYPE_FIN_CHAR;
				hold_get_ptr = &hold[0];
				while( hold_get_ptr != hold_put_ptr )
					{
					par_char = *hold_get_ptr++;
					if( cs_attr[par_char]&M_CS_PAR_SEP )
						_q_char( par_char, CE_TYPE_PAR_SEP );
					else if( cs_attr[par_char]&M_CS_PAR_CHAR )
						_q_char( par_char, CE_TYPE_PAR_CHAR );
					else
						{
						_q_char( par_char, fin_char );
						fin_char = CE_TYPE_CHAR;
						}
					}
				}
				_q_char( ch, fin_char );
				csi_state = CSI_ST_NORMAL;
				return;
				}
			/* syntax error in escape sequence */
			{
			unsigned char *hold_get_ptr;

			_q_char( 0x9b, CE_TYPE_CHAR );
			hold_get_ptr = &hold[0];
			while( hold_get_ptr != hold_put_ptr )
				_q_char( *hold_get_ptr++, CE_TYPE_CHAR );
			csi_state = CSI_ST_NORMAL;
			goto exit_loop;
			}
			}
		}
		}
exit_loop:
	/*
	 *	Get the binding for this character (check_ both the
	 *		local and global key maps)
	 */

	/*
	 *	Get the Local Key kmap binding (if it is valid)
	 */
	if( bf_mode.md_keys != 0 )
		proc = get_keymap_binding( bf_mode.md_keys, ch );

	/*
	 *	If there was no Local Key kmap binding, use the Global Key kmap
	 */
	if( proc == 0 )
		proc = get_keymap_binding( current_global_map, ch );
	/*
	 *	If this key was bound to "interrupt-key", purge the internal
	 *	typeahead buffer and declare the interrupt
	 */
	if( proc == &interrupt_block )
		{
		illegal_operation();

		/*
		 *	purge type ahead
		 */
		while( (char_cell = queue_remove_char( input_queue.next )) != NULL )
			{
			/* insert at tail of queue */
			queue_insert( free_queue.prev, &char_cell->ce_queue );

			interlock_dec( &input_pending );
			interlock_dec( &keyboard_input_pending );
			}
		if( keyboard_input_pending != 0 )
			invoke_debug();
		interrupt_key_struck = 1;
		}

	/*
	 *	mark input pending
	 */
	_q_char( ch, CE_TYPE_CHAR );
	}


int convert_key_string_command( void )
	{
	int len;
	unsigned char *ptr;

	concat_command();
	if( err )
		return 0;

	ptr = malloc_ustr( ml_value->exp_int );
	if( ptr == 0 )
		return 0;

	if( ! convert_key_string
		(
		ml_value->exp_int,
		ml_value->exp_v.v_string,
		&len,
		ptr
		) )
		{
		error( u_str("error converting key string") );
		return 0;
		}

	release_expr( ml_value );	/* free up input string */

	ml_value->exp_int = len;	/* return converted string */
	ml_value->exp_release = 1;
	ml_value->exp_v.v_string = ptr;

	return 0;
	}

int convert_key_string
	(
	int inlen,				/* length of input string */
	unsigned char *inptr,	/* address of input string */
	int *outlen,	/* return output length here */
	unsigned char *outptr/* address of output string */
	)
	{
#define	_qq_char( value ) outptr[ len++ ] = value

	unsigned char ch;
	int len;
	int csi_state = CSI_ST_NORMAL;
	unsigned char hold[100];
	unsigned char *hold_get_ptr;
	unsigned char *hold_put_ptr;
	int i;

	csi_state = CSI_ST_NORMAL;
	hold_put_ptr = &hold[0];
	len = 0;

	/*
	 *	If nothing special is requested just return
	 *	the input as the result.
	 */
	if( ! cs_enabled )
		{
		memmove( outptr, inptr, inlen );
		outlen[0] = inlen;
		return 1;
		}

	for( i=0; i<=inlen - 1; i += 1 )
		{
		ch = inptr[ i ];
		for(;;)
			{
			switch( csi_state)
			{
			case CSI_ST_NORMAL:
				{
				if( cs_attr[ch]&M_CS_CVT_CSI )
					ch = 0x9b;	/* CSI */

				switch( ch )
				{
				case 0x1b:
					csi_state = CSI_ST_ESC;
					goto quit_loop_1;

				case 0x9b:
					csi_state = CSI_ST_CSI;
					hold_put_ptr = &hold[0];
					goto quit_loop_1;

				default:
					_qq_char( ch );
					goto quit_loop_1;
				}
				break;
				}
			case CSI_ST_ESC:
				{
				if( cs_attr[ch]&M_CS_CVT_8BIT )
					{
					csi_state = CSI_ST_NORMAL;
					ch += 0x40;	/* convert 7bit to 8bit */
					}
				else
					{
					_qq_char( 0x1b );
					_qq_char( ch );
					csi_state = CSI_ST_NORMAL;
					goto quit_loop_1;
					}
				break;
				}
			case CSI_ST_CSI:
				{
				if( cs_attr[ch]&M_CS_PAR_SEP
				|| cs_attr[ch]&M_CS_PAR_CHAR
				|| cs_attr[ch]&M_CS_INT_CHAR )
					{
					if( hold_put_ptr >= &hold[sizeof(hold)-1] )
						{
						return 0;
						}
					*hold_put_ptr++ = ch;
					goto quit_loop_1;
					}
				if( cs_attr[ch]&M_CS_FIN_CHAR )
					{
					if( ch == '~' && cs_cvt_f_keys )
						{
						int key_num;

						hold_get_ptr = &hold[0];
						key_num = 0;
						while( hold_get_ptr != hold_put_ptr )
							{
							ch = *hold_get_ptr++;
							if( ch < '0' || ch > '9' )
								{
								_qq_char( 0x9b );
								goto process_f_keys;
								}
							key_num = key_num * 10 + ch - '0';
							}
						_qq_char( 0x80 );
						_qq_char( (unsigned char)(key_num + ' ') );
						csi_state = CSI_ST_NORMAL;
						goto quit_loop_1;
				process_f_keys:
						;
						}
					else
					if( cs_cvt_mouse
					&& ch == 'w'
					&& &hold[0] != hold_put_ptr
					&& hold_put_ptr[-1] == '&' )
						{
						int event_num;

						hold_get_ptr = &hold[0];
						event_num = 0;
						while( hold_get_ptr != hold_put_ptr )
							{
							ch = *hold_get_ptr++;
							if( ch < '0' || ch > '9' )
								break;
							event_num = event_num * 10 + ch - '0';
							}
						_qq_char( 0x81 );
						hold_put_ptr = &hold_put_ptr[-1];
						_qq_char( (unsigned char)(event_num + 'A') );
						csi_state = CSI_ST_NORMAL;
						goto quit_loop_1;
						}
					else
						_qq_char( 0x9b );

					{
					unsigned char par_char;
					unsigned char *hold_get_ptr;
					/*
					 *	Find out how many parameters are present
					 */
					hold_get_ptr = &hold[0];
					while( hold_get_ptr != hold_put_ptr )
						{
						par_char = *hold_get_ptr++;
						if( ! (cs_attr[par_char]&M_CS_PAR_SEP)
						&&  ! (cs_attr[par_char]&M_CS_PAR_CHAR) )
							_qq_char( par_char );
						}
					}
					_qq_char( ch );
					csi_state = CSI_ST_NORMAL;
					goto quit_loop_1;
					}
				/* syntax error in escape sequence */
				_qq_char( 0x9b );
				hold_get_ptr = &hold[0];
				while( hold_get_ptr != hold_put_ptr )
					_qq_char( *hold_get_ptr++ );
				csi_state = CSI_ST_NORMAL;
				goto quit_loop_1;
				}
			}
			}
	quit_loop_1:
			;
		}

	/*
	 *	See if we got an incomplete key string.
	 *	If so return whats been collected up.
	 */
	if( csi_state != CSI_ST_NORMAL )
		{
		if( csi_state == CSI_ST_ESC )
			_qq_char( 0x1b );
		if( csi_state == CSI_ST_CSI )
			_qq_char( 0x9b );
		hold_get_ptr = &hold[0];
		while( hold_get_ptr != hold_put_ptr )
			_qq_char( *hold_get_ptr++ );
		}

	outlen[0] = len;

	return 1;
	}

void work_process_queue( void )
	{
	struct emacs_work_item *work;

	/* dispatch all the work items */
	while( (work = queue_remove_struct( emacs_work_item, emacs_work_queue.next )) != NULL )
		{
		interlock_dec( &input_pending );
		/* rtn is responsible for deallocting work */
		work->rtn( work );
		}
	}

void work_add_item( struct emacs_work_item *work )
	{
	/* if the queue has not been init ignore this request */
	if( emacs_work_queue.next == NULL )
		return;
	interlock_inc( &input_pending );
	queue_insert( emacs_work_queue.prev, &work->header );
	}

void init_key( void )
	{
	re_init_keyboard();

	checkpoint_frequency = 300;

	/*
	 * Create the initial global keymaps and set up the pointers
	 */
	global_map = define_keymap( u_str( "default-global-keymap" ) );
	minibuf_local_ns_map = define_keymap( u_str( "Minibuf-local-NS-map" ) );
	minibuf_local_map = define_keymap( u_str( "Minibuf-local-map" ) );
	}

void re_init_keyboard( void )
	{
	int i;
	struct variablename *v;
	struct binding *b;

	keyboard_input_pending = 0;

	null_map.k_type = KEYMAP_SHORT_TYPE;
	null_map.k_map.k_short = &short_null_map;

	input_queue.next = input_queue.prev = NULL;
	free_queue.next = free_queue.prev = NULL;
	push_back_queue.next = push_back_queue.prev = NULL;
	emacs_work_queue.next = emacs_work_queue.prev = NULL;

	queue_init( &input_queue );
	queue_init( &free_queue );
	queue_init( &push_back_queue );

	memset( &char_cells, 0, sizeof( char_cells ) );

	for( i=0; i<=CHARACTER_QUEUE_SIZE - 1; i += 1 )
		queue_insert( &free_queue, &char_cells[i].ce_queue );

	queue_init( &emacs_work_queue );

	/*
	 *	Find the control-sequence-parameters variable binding
	 */
	v = lookup_variable( u_str("control-string-parameters") );
	b = v->v_binding;	/* get the binding */
	parameter_expr = b->b_exp;	/* get expression */
	}
