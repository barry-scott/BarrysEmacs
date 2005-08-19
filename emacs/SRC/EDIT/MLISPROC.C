/* Copyright (c) 1982, 1983, 1984, 1985
 *	Barry A. Scott and Nick Emery
 * Mlisp Processing routines
 *
 * VMS Emacs MLisp (Mock/Minimal Lisp).
 */
#include <emacs.h>

#ifdef vms
#include <fscndef.h>
#include <descrip.h>
#endif

/*forward*/ static int exec_number( void );
/*forward*/ static int exec_string( void );
/*forward*/ int exec_prog( struct prognode *p );
/*forward*/ int progn_command( void );
/*forward*/ int lambda_command( void );
/*forward*/ int declare_global( void );
/*forward*/ int declare_buffer_specific( void );
/*forward*/ static void perform_declare(int buf_variable);
/*forward*/ void declare( struct variablename *v, int buf_variable );
/*forward*/ int is_bound_command( void );
/*forward*/ int error_occured( void );
/*forward*/ int prefix_argument_loop( void );
/*forward*/ int save_window_excursion( void );
/*forward*/ int save_excursion( void );
/*forward*/ int save_excursion_inner(int (*rtn)(void));
/*forward*/ int if_command( void );
/*forward*/ int while_command( void );
/*forward*/ int insert_string( void );
/*forward*/ int message_command( void );
/*forward*/ int send_string_to_terminal( void );
/*forward*/ int error_message( void );
/*forward*/ void do_release( struct expression *e );
/*forward*/ int star_define_function( void );
/*forward*/ int define_function( void );
/*forward*/ static int define_function_inner(struct bound_name *proc);
/*forward*/ int define_external_function( void );
/*forward*/ static struct binding *resolve_buffer_specific( struct binding *b );
/*forward*/ static struct binding *resolve_buffer_specific_assignment( struct binding *b, struct variablename *v, int default_required );
/*forward*/ static int exec_variable( void );
/*forward*/ void copy_expression( struct expression *from_e, struct expression *to_e, int is_system );
/*forward*/ int check_pos( int value, struct variablename *v );
/*forward*/ int check_bool( int value, struct variablename *v );
/*forward*/ int check_null( int value, struct variablename *v );
/*forward*/ int check_read_only( int value, struct variablename *v );
/*forward*/ int setq_command( void );
/*forward*/ int setq_default_command( void );
/*forward*/ static int do_setq(int Default);
/*forward*/ int set_command( void );
/*forward*/ int set_default_command( void );
/*forward*/ static int do_set(int Default);
/*forward*/ int setq_array_command( void );
/*forward*/ void perform_set( struct variablename *v, int arg, unsigned char *svalue, int setting_default, int is_array, int ivalue );
/*forward*/ int print_command( void );
/*forward*/ int print_default_command( void );
/*forward*/ static int do_print_command(int def);
/*forward*/ int provide_prefix_argument( void );
/*forward*/ int return_prefix_argument( void );
/*forward*/ void print_expr( struct prognode *p, int depth );
/*forward*/ void void_result( void );
/*forward*/ int array_command( void );
/*forward*/ static void dest_array( struct emacs_array *a );
/*forward*/ int fetch_array_command( void );
/*forward*/ static int array_index( unsigned int arg, struct emacs_array *a );
/*forward*/ struct emacs_array *create_array( int dims, ... );
/*forward*/ struct expression *create_expression(int type, int val, unsigned char *string, int release);
/*forward*/ int type_of_expression_command( void );
/*forward*/ int bounds_of_array_command( void );
/*forward*/ void init_lisp( void );


/* execute a number node */
static int exec_number( void )
	{
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = cur_exec->p_args[0].pa_int;
	return 0;
	}

static int exec_string( void )
	{
	ml_value->exp_type = ISSTRING;
	ml_value->exp_int = cur_exec->p_args[0].pa_int;
	ml_value->exp_v.v_string = &cur_exec->p_args[1].pa_char;
	return 0;
	}

int exec_prog( struct prognode *p )
	{
	struct prognode *old;
	unsigned int old_last_arg;
	struct bound_name *b;
	int rv;
	unsigned int was_active;

	if( err )
		return 0;

	old = cur_exec;
	old_last_arg = last_arg_used;
	rv = 0;

	release_expr( ml_value );
	ml_value = &global_value;
	global_value.exp_type = ISVOID;
	if( p == 0 )
		{
		if( old != 0 )
			{
			b = old->p_proc;
			error (u_str( "\"%s\" has not been defined yet." ),
					b->b_proc_name);
			}
		else
			error (u_str( "Attempt to execute an undefined MLisp function." ));
		return 0;
		}

	was_active = p->p_active;
	p->p_active = 1;
	cur_exec = p;
	last_arg_used = -1;
	rv = execute_bound( p->p_proc );
	p->p_active = was_active;
	cur_exec = old;
	last_arg_used = old_last_arg;

	/*
	 *	Copy ml_value into last_expression for the trace package to use
	 */
	if( trace_mode != 0 )
	if( ! (in_trace != 0) )
		{
		release_expr( &last_expression );
		if( ml_value->exp_type == ISVOID )
			{
			last_expression.exp_type = ISINTEGER;
			last_expression.exp_int = 0;
			}
		else
			copy_expression( ml_value, &last_expression, 0 );
		}

	return rv;
	}

GLOBAL SAVRES struct bound_name progn_block;
GLOBAL SAVRES struct bound_name lambda_block;

int progn_command( void )
	{
	struct prognode *p;
	int i;
	struct variablename *v;
	struct binding *b;
	int first_prog;
	int rv;
	int var;

	if( (p = cur_exec) == 0 )
		{
		error( u_str( "progn can only appear in mlisp statements" ) );
		return 0;
		}

	first_prog = p->p_nargs;
	/*
	 *	Bind the local variables
	 */
	for( var=0; var<=p->p_nargs - 1; var += 1 )
		{
		struct expression *e;
		struct prognode *pp;

		pp = p->p_args[var].pa_node;
		if( pp->p_proc != &bound_variable_node )
			{
			first_prog = var;
			break;
			}
		v = pp->p_args[0].pa_name;
		b = malloc_struct( binding );
		b->b_exp = e = malloc_struct( expression );
		e->exp_type = ISINTEGER;
		e->exp_int = 0;
		b->b_is_system = 0;
		b->b_is_default = 0;
		b->b_buffer_specific = 0;
		b->b.b_local_to = NULL;
		e->exp_v.v_string = NULL;
		b->b_thunk.thunk_int = NULL;
		b->b_rangecheck.check_int = NULL;
		b->b_inner = v->v_binding;
		v->v_binding = b;
		}
	rv = 0;
	i = first_prog;
	while( ! err && rv == 0 && quitting_emacs == 0
	&& i < p->p_nargs )
		{
		rv = exec_prog( p->p_args[i].pa_node );
		i++;
		}
	for( var=0; var<=first_prog - 1; var += 1 )
		{
		struct prognode *pp;

		pp = p->p_args[var].pa_node;
		v = pp->p_args[0].pa_name;
		b = v->v_binding;
		release_expr( b->b_exp );
		free( b->b_exp );
		b = b->b_inner;
		free( v->v_binding );
		v->v_binding = b;
		}

	return rv;
	}

/* Lambda binding function (DLK) */

int lambda_command( void )
	{
	struct prognode *p;
	struct binding_list *first_binding;
	int i;
	int arg_number;
	int rv;
	struct binding_list *last_binding;
	struct variablename *v;
	int first_prog;
	struct binding_list *b;

	if( (p = cur_exec) == 0 )
		{
		error (u_str( "lambda can only appear in mlisp statements" ));
		return 0;
		}

	/*
	 *	There must be at least one arg to this function
	 */
	if( p->p_nargs == 0 )
		{
		error (u_str( "Insufficient args to lambda" ));
		return 0;
		}

	first_binding = 0;
	last_binding = 0;
	arg_number = 0;
	rv = 0;

	/*
	 *	Run through the list of variables, binding them to the
	 *	evaluation of the appropriate argument to the MLisp function
	 *	but do not make the variables available to the MLisp world
	 *	until ALL the arguments are evaluated.
	 *
	 *	If there are insufficent arguments to match all the lambda
	 *	variables, we look to see if the variable is followed by
	 *	an MLisp expression[ If ]it is, we evaluate the expression
	 *	and set the variable to that value.
	 */
	p = p->p_args[0].pa_node;
	for( i=0; i<=p->p_nargs - 1; i += 1 )
		{
		struct prognode *pp;
		struct prognode *rootp;
		struct execution_stack old;

		/*
		 *	This had better be a variable (skip MLisp expressions)
		 */
		pp = p->p_args[i].pa_node;
		if( pp->p_proc == &bound_variable_node )
			{
			/*
			 *	If there are not enough arguments to the MLisp
			 *	function to satisfy all the lambda bindings
			 *	check_ to see if the variable is followed by an
			 *	MLisp expression[ If ]not, declare an error[ If ]so,
			 *	evaluate it and set the value of the new variable
			 *	to its result.
			 */
			rootp = execution_root.es_cur_exec;
			if( rootp == 0
			|| arg_number >= rootp->p_nargs )
				{
				/*
				 *	If it is a variable, you lose -- otherwise
				 *	evaluate it
				 */
				pp = p->p_args[i + 1].pa_node;
				if( i + 1 >= p->p_nargs
				|| pp->p_proc == &bound_variable_node )
					error (u_str( "Insufficent arguments and no default initializers" ));
				else
					{
					old = execution_root;
					execution_root = *execution_root.es_dyn_parent;
					exec_prog( p->p_args[i + 1].pa_node );
					execution_root = old;
					}
				}
			else
				{
				/*
				 *	Evaluate the appropriate argument to the MLisp
				 *	function
				 */
				old = execution_root;
				execution_root = *execution_root.es_dyn_parent;
				exec_prog( rootp->p_args[arg_number].pa_node );
				execution_root = old;
				}

			/*
			 *	Create a new binding for the lambda variable
			 */
			b = malloc_struct( binding_list );

			b->bl_binding.b_exp = malloc_struct( expression );
			b->bl_binding.b_thunk.thunk_int = NULL;
			b->bl_binding.b_rangecheck.check_int = NULL;
			b->bl_binding.b_is_system = 0;
			b->bl_binding.b_is_default = 0;
			b->bl_binding.b_buffer_specific = 0;

			b->bl_flink = 0;
			b->bl_arg_index = i;

			arg_number++;

			if( err )
				{
				/*
				 *	An error was encountered, bind this variable to 0
				 */
				struct expression *e;

				e = b->bl_binding.b_exp;
				e->exp_type = ISINTEGER;
				e->exp_int = 0;
				e->exp_v.v_string = 0;
				}
			else
				{
				/*
				 *	No errors so far, bind to the result of the arg
				 */
				copy_expression( ml_value, b->bl_binding.b_exp, 0);
				}
			/*
			 *	Insert this binding at the end of the binding List
			 */
			if( last_binding != 0 )
				last_binding->bl_flink = b;
			else
				first_binding = b;

			last_binding = b;
			}
		}
	/*
	 *	With all the arguments evaluated, we now loop through the
	 *	bindings, making the variables known to MLisp
	 */
	b = first_binding;
	while( b != 0 )
		{
		struct prognode *pp;
		pp = p->p_args[b->bl_arg_index].pa_node;
		v = pp->p_args[0].pa_name;
		b->bl_binding.b_inner = v->v_binding;
		v->v_binding = &b->bl_binding;
		b = b->bl_flink;
		}
	/*
	 *	Setup any Local variables that are declared
	 */
	p = cur_exec;
	first_prog = p->p_nargs;
	for( i=1; i<=p->p_nargs - 1; i += 1 )
		{
		struct expression *e;
		struct prognode *pp;

		pp = p->p_args[i].pa_node;
		if( pp->p_proc != &bound_variable_node )
			{
			first_prog = i;
			break;
			}
		v = p->p_args[i].pa_node->p_args[0].pa_name;
		b = malloc_struct( binding_list );
		b->bl_binding.b_exp = e = malloc_struct( expression );
		e->exp_type = ISINTEGER;
		e->exp_int = 0;
		b->bl_binding.b_is_system = 0;
		b->bl_binding.b_is_default = 0;
		b->bl_binding.b_buffer_specific = 0;
		b->bl_binding.b_thunk.thunk_int = NULL;
		b->bl_binding.b_rangecheck.check_int = NULL;
		e->exp_v.v_string = NULL;
		b->bl_binding.b_inner = v->v_binding;
		v->v_binding = &b->bl_binding;
		}
	/*
	 *	Loop through the rest of the arguments to lambda,
	 *	evaluating them (They are the MLisp expressions to execute
	 *	with the given lambda bindings).
	 */
	i = first_prog;
	while( ! err && rv == 0 && ! (quitting_emacs != 0)
	&& i < p->p_nargs )
		{
		rv = exec_prog( p->p_args[i].pa_node );
		i++;
		}
	/*
	 *	Destroy any local variables
	 */
	p = cur_exec;
	for( i=1; i<=first_prog - 1; i += 1 )
		{
		struct binding *b;
		struct prognode *pp;

		pp = p->p_args[i].pa_node;
		v = pp->p_args[0].pa_name;
		b = v->v_binding;
		release_expr( b->b_exp );
		free( b->b_exp );
		b = b->b_inner;
		free( v->v_binding );
		v->v_binding = b;
		}
	/*
	 *	Loop through the list of variables that were bound and
	 *	destroy the bindings
	 */
	b = first_binding;
	p = p->p_args[0].pa_node;
	while( b != 0 )
		{
		struct prognode *pp;
		struct binding_list *tmp;

		pp = p->p_args[b->bl_arg_index].pa_node;
		v = pp->p_args[0].pa_name;
		/*
		 *	If the variable is bound to a string, release the string
		 */
		release_expr( b->bl_binding.b_exp );
		free( b->bl_binding.b_exp );
		v->v_binding = b->bl_binding.b_inner;
		tmp = b;
		b = tmp->bl_flink;
		free( tmp );
		}

	/*
	 *	Return results of last exec_prog called
	 */
	return rv;
	}

int declare_global( void )
	{
	if( cur_exec == 0 )
		error (u_str( "declare-global can only appear in mlisp statements" ));
	else
		perform_declare( 0 );
	return 0;
	}

int declare_buffer_specific( void )
	{
	if( cur_exec == 0 )
		error (u_str( "declare-buffer-specific can only appear in mlisp statements" ));
	else
		perform_declare( 1 );
	return 0;
	}

static void perform_declare(int buf_variable)
	{
	struct prognode *p;
	struct prognode *pp;
	struct variablename *v;
	int i;

	p = cur_exec;
	for( i=0; i<=p->p_nargs - 1; i += 1 )
		{
		pp = p->p_args[i].pa_node;
		if( pp->p_proc == &bound_variable_node )
			{
			v = pp->p_args[0].pa_name;
			if( v->v_binding == 0 || buf_variable )
				declare( v, buf_variable );
			}
		else
			{
			error(u_str( "%s's argument %d should be a variable name" ),
				p->p_proc->b_proc_name, i + 1);
			return;
			}
		}
	}

void declare
	(
	struct variablename *v,
	int buf_variable
	)

	{
	struct expression *e;
	struct binding *b;
	if( v->v_binding == 0 || ! buf_variable )
		{
		b = malloc_struct( binding );
		b->b_exp = e = malloc_struct( expression );
		e->exp_type = ISINTEGER;
		e->exp_int = 0;
		e->exp_v.v_string = NULL;
		b->b_thunk.thunk_int = NULL;
		b->b_rangecheck.check_int = NULL;
		b->b.b_local_to = NULL;
		b->b_inner = v->v_binding;
		v->v_binding = b;
		b->b_is_system = 0;
		b->b_is_default = 0;
		b->b_buffer_specific = 0;
		}
	else
		{
		b = v->v_binding;
		while( b->b_inner != 0 )
			b = b->b_inner;
		if( b->b_is_system )
			return;
		}
	b->b_buffer_specific = buf_variable;
	b->b_is_default = buf_variable;
	}

int is_bound_command( void )
	{
	struct prognode *p;
	struct variablename *v;
	int i;

	if( (p = cur_exec) == 0 )
		{
		error( u_str( "is-bound can only appear in mlisp statements" ));
		return 0;
		}

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = 1;		/* assume is bound */
	for( i=0; i<=p->p_nargs - 1; i += 1 )
		{
		struct prognode *pp;

		pp = p->p_args[i].pa_node;
		if( pp->p_proc != &bound_variable_node )
			{
			error( u_str( "is-bound's arguments must be variable names" ) );
			return 0;
			}
		v = pp->p_args[0].pa_name;
		if( v->v_binding == 0 )
			{
			ml_value->exp_int = 0;	/* variable is not bound */
			break;
			}
		}

	return 0;
	}

int is_function_command( void )
	{
	int index;

	if( ! string_arg( 1 ) )
		return 0;

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = 0;

	index = find_mac( ml_value->exp_v.v_string );
	if( index < 0 )
		return 0;

	if( mac_bodies[ index ]->b_bound.b_body == 0 )
		return 0;

	ml_value->exp_int = 1;

	return 0;
	}

extern int in_error_occurred;
int error_occured( void )
	{
	int rv;
	in_error_occurred++;
	rv = progn_command();
	release_expr( ml_value );
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = err;
	err = 0;
	in_error_occurred--;
	return rv;
	}

int prefix_argument_loop( void )
	{
	unsigned int rv;
	int i;

	rv = 0;
	for( i=execution_root.es_prefix_argument - 1; i>=0; i-- )
		{
		if( err || rv != 0 )
			break;

		rv = progn_command();
		}

	return rv;
	}

int save_window_excursion( void )
	{
	unsigned int rv;

	push_window_ring();
	rv = save_excursion();
	pop_window_ring();

	return rv;
	}

int save_excursion( void )
	{
	return save_excursion_inner( progn_command );
	}

int save_excursion_inner(int (*rtn)(void))
	{
	struct marker *olddot;
	struct marker *oldmark;
	unsigned int buffer_visible;
	struct search_globals sea;
	unsigned char *sea_str;
	int rv;
	int c;
	int i;

	olddot = new_mark();
	oldmark = 0;
	buffer_visible = wn_cur->w_buf == bf_cur;

	/*
	 *	Save the search globals, but copy all the
	 *	markers that are there.
	 */
	sea = sea_glob;
	for( i=0; i<=MAX_NBRA-1; i += 1 )
		{
		if( sea_glob.sea_bra_slist[i] != 0 )
			sea_glob.sea_bra_slist[i] =
				copy_mark
				(
				new_mark(),
				sea_glob.sea_bra_slist[i]
				);

		if( sea_glob.sea_bra_elist[i] != 0 )
			sea_glob.sea_bra_elist[i] =
				copy_mark
				(
				new_mark(),
				sea_glob.sea_bra_elist[i]
				);
		}
	sea_str = last_search_string;
	last_search_string = savestr( sea_str );
	set_mark( olddot, bf_cur, dot, 0 );
	if( bf_cur->b_mark != 0 )
		{
		oldmark = new_mark();
		set_mark( oldmark, bf_cur, to_mark( bf_cur->b_mark ), 0 );
		}
	rv = rtn();
	if( olddot->m_buf != 0 )
		set_bfp( olddot->m_buf );
	if( buffer_visible )
		window_on( bf_cur );
	dot = to_mark( olddot );
	if( oldmark != 0 )
		{
		if( bf_cur->b_mark == 0 )
			bf_cur->b_mark = new_mark();
		set_mark( bf_cur->b_mark, bf_cur, to_mark( oldmark ), 0 );
		dest_mark( oldmark );
		}
	else
		if( bf_cur->b_mark != 0 )
			{
			dest_mark( bf_cur->b_mark );
			bf_cur->b_mark = 0;
			}

	dest_mark( olddot );

	/*
	 *	Destroy all the markers in the search globals
	 *	before overwritting with the original search
	 *	globals data.
	 */
	for( c=0; c<=MAX_NBRA-1; c += 1 )
		{
		if( sea_glob.sea_bra_slist[ c ] != 0 )
			dest_mark( sea_glob.sea_bra_slist[ c ] );

		if( sea_glob.sea_bra_elist[ c ] != 0 )
			dest_mark( sea_glob.sea_bra_elist[ c ] );
		}
	sea_glob = sea;
	free( last_search_string );
	last_search_string = sea_str;
	return rv;
	}

int if_command( void )
	{
	int i;

	if( check_args( 2, 0 ) )
		return 0;

	for( i=1; i<=cur_exec->p_nargs - 1; i += 2 )
		{
		if( err ) return 0;
		if( numeric_arg( i ) != 0 )
			return exec_prog( cur_exec->p_args[i].pa_node );
		}
	/*
	 *	check_ for dangling else expr, that is an odd number of
	 *	arguments to "if"
	 */
	if( (cur_exec->p_nargs & 1) != 0 )
		return exec_prog( cur_exec->p_args[cur_exec->p_nargs - 1].pa_node );
	else
		return 0;
	}

int while_command( void )
	{
	int rv;
	struct prognode **p;
	int nargs;
	struct prognode *test;
	int i;

	if( check_args( 2, 0 ) )
		return 0;
	if( err ) return 0;

	rv = 0;
	nargs = cur_exec->p_nargs - 1;

	p = (struct prognode **)&cur_exec->p_args[0];
	test = p[0];

	while( rv == 0
	&& quitting_emacs == 0
	&& (rv = exec_prog( test )) == 0
	&& ml_value->exp_type == ISINTEGER
	&& ml_value->exp_int != 0
	&& ! err )
		for( i=1; i<=nargs; i += 1 )
			if( rv != 0 || quitting_emacs != 0 )
				return 0;
			else
				rv = exec_prog( p[ i ] );
	return rv;
	}

int insert_string( void )
	{
	concat_command();
	if( err ) return 0;

	ins_cstr( ml_value->exp_v.v_string, ml_value->exp_int );
	return 0;
	}

int message_command( void )
	{
	concat_command();
	if( err )
		return 0;
	cur_exec = 0;
	if( ! err )
		message( u_str( "%s" ), ml_value->exp_v.v_string );
	void_result();
	return 0;
	}

int send_string_to_terminal( void )
	{
	unsigned char *s;
	s = getstr( u_str( ": send-string-to-terminal " ) );
#ifndef _WINDOWS
	if( s != 0 && term_is_terminal == 1)
		tt->t_io_printf( tt, u_str( "%s" ), s );
#endif
	void_result();
	return 0;
	}

int error_message( void )
	{
	concat_command();
	if( err )
		return 0;
	error( u_str( "%s" ), ml_value->exp_v.v_string );
	void_result();
	return 0;
	}

/* release the storage associated with an expression */
void do_release( struct expression *e )
	{
	if( e->exp_release )
		{
		switch( e->exp_type )
		{
		case ISSTRING:
			free( e->exp_v.v_string );
			e->exp_v.v_string = 0;
			break;

		case ISMARKER:
			dest_mark( e->exp_v.v_marker );
			e->exp_v.v_marker = 0;
			break;

		case ISWINDOWS:
			dest_window_ring( e->exp_v.v_windows );
			e->exp_v.v_windows = 0;
			break;

		case ISARRAY:
			dest_array( e->exp_v.v_array );
			e->exp_v.v_array = 0;
			break;
		default:
			break;
		}

		e->exp_release = 0;
		}
	}

int star_define_function( void )
	{
	return define_function_inner( &lambda_block );
	}

int define_function( void )
	{
	return define_function_inner( &progn_block );
	}

static int define_function_inner(struct bound_name *proc)
	{
	struct prognode *p;
	struct bound_name *b;
	int nargs;
	int i;

	if( check_args( 1, 0 ) )
		return 0;

	/*
	 *	As the arg list is being dismantled mark its
	 *	length as 0. This will result in an error from
	 *	the check_args( 1, 0 ) above if a defun is
	 *	processed twice.
	 */
	nargs = cur_exec->p_nargs;
	cur_exec->p_nargs = 0;

	for( i=0; i<=nargs - 1; i += 1 )
		if( (p = cur_exec->p_args[i].pa_node) != 0 )
			{
			cur_exec->p_args[i ].pa_node = 0;
			b = p->p_proc;
			switch( b->b_binding )
			{
			case MLISPBOUND:
				lisp_free( b->b_bound.b_prog );
				break;
			case MACROBOUND:
			case AUTOLOADBOUND:
				free( b->b_bound.b_body );
				b->b_binding = MLISPBOUND;
				break;
			default:
				{
				error( u_str( "\"%s\" is bound to a wired procedure and cannot be rebound!" ),
					b->b_proc_name );
				return 0;
				}
			}
			b->b_bound.b_prog = p;
			p->p_proc = proc;
			}
	return 0;
	}


static int find_image_index
	(
	unsigned char *image,
	unsigned char *filename
	)
	{
	int index;
	int i;

	/* search the existing images */
	index = image_count ;

	for( i=0; i<=image_count  - 1; i += 1 )
		if( images[ i ] == 0 )
			{
			index = i;
			break;
			}
		else
		if( _str_cmp( image, images[i] ) == 0 )
			{
			free( image_filenames[i] );
			image_filenames[i] = filename;
			return i;
			}

	/* It is a new image, allocate the next slot */
	if( index >= image_count  - 1 )
		/* allocate more space */
		if( image_count  == 0 )
			{
			images = (unsigned char **)malloc( 10 * sizeof( unsigned char * ), malloc_type_star_star );
			image_contexts = (void **)malloc( 10 * sizeof( void * ), malloc_type_star_star );
			image_filenames = (unsigned char **)malloc( 10 * sizeof( unsigned char * ), malloc_type_star_star );
			image_count  = 10;
			}
		else
			{
			image_count  = image_count  + 10;
			images = (unsigned char **)realloc( images, image_count  * 4, malloc_type_star_star );
			image_contexts = (void * *)realloc( image_contexts, image_count  * 4, malloc_type_star_star );
			image_filenames = (unsigned char **)realloc( image_filenames, image_count  * 4, malloc_type_star_star );
			}
	if( images == NULL
	|| image_contexts == NULL
	|| image_filenames == NULL )
		{
		error( u_str( "Out of memory. Lost all image names and contexts" ) );
		images = NULL;
		image_filenames = NULL;
		image_contexts = NULL;
		image_count  = 0;
		image_context_lost = 1;

		return -1;
		}
	else
		{
		images[index] = image;
		image_contexts[index] = NULL;
		images[index + 1] = NULL;
		image_filenames[index] = filename;

		return index;
		}
	}

int define_external_function( void )
	{
	unsigned char *image = NULL;
	unsigned char *symbol = NULL;
	unsigned char *filename = NULL;
	struct extern_func *body;

	/*
	 * check_ the argument count. There must be two args, but there may be
	 * three. The third allows you to map the symbol name in the image to
	 * a difference name in EMACS
	 */
	if( check_args( 2, 3 ) )
		return 0;

	/* Collect the file name */

	if( ! string_arg( 2 ) )
		goto other_error;
	if( (filename = savestr( ml_value->exp_v.v_string )) == NULL )
		goto out_of_memory;

	release_expr( ml_value );

	/* Collect the image symbol name if it exists */

	if( cur_exec->p_nargs > 2 )
		if( ! string_arg( 3 ) )
			goto other_error;
		else
			{
			if( (symbol = savestr( ml_value->exp_v.v_string) ) == NULL )
				goto out_of_memory;
			}
	else
		symbol = NULL;

	/* Collect the EMACS function name */

	if( ! string_arg( 1 ) )
		goto other_error;

	if( symbol == NULL )
		if( (symbol = savestr( ml_value->exp_v.v_string) ) == NULL )
			goto out_of_memory;

	/* Now work out the image name from the filename */
#ifdef vms
	{
	struct dsc$descriptor fdesc;
	struct fscn_def item[2];

	DSC_SZ( fdesc, filename );

	item[0].w_item_code = FSCN$_NAME;
	item[0].w_length = 0;
	item[0].a_addr = 0;
	item[1].w_item_code = 0;
	item[1].w_length = 0;
	item[1].a_addr = 0;

	if( ! VMS_SUCCESS(sys$filescan( &fdesc, item, 0 )) )
		{
		error( u_str( "$FILESCAN return an error status for file %s" ), filename );
		goto other_error;
		}

	image = malloc_ustr( item[0].w_length + 1 );
	if( image == NULL )
		goto out_of_memory;

	/* copy the image name out of the file spec */
	memcpy( image, item[0].a_addr, item[0].w_length );
	image[ item[0].w_length ] = 0;
	}
#else
	image = _str_rchr( filename, PATH_CH );
	if( image == NULL )
		image = savestr( filename );
	else
		image = savestr( image );	
	if( image == NULL )
		goto out_of_memory;
#endif
	/* Now, make the entry in the function name space, and release the
	 * extra */

	if( (body = malloc_struct( extern_func )) == NULL )
		goto out_of_memory;

	body->ef_funcname = symbol;
	body->ef_function = 0;
	body->ef_restore_count = is_restored - 1;
	body->ef_image_index = find_image_index( image, filename );
	body->ef_context = &image_contexts[ body->ef_image_index ];
	def_mac( ml_value->exp_v.v_string, body, -4, 0, 0 );
	void_result();
	return 0;

out_of_memory:
	error( u_str( "Out of memory in external-function!" ) );
other_error:
	if( symbol != 0 )
		free( symbol );
	if( image != 0 )
		free( image );
	if( filename != 0 )
		free( filename );
	void_result();
	return 0;
	}

/*
 *	(use-buffer-variables "buffer-name" s-expr)
 */

struct usevar *use_var_of_buffers_ptr;
struct emacs_buffer *use_variables_of_buffer;

void use_var_de_ref_buf(struct emacs_buffer *b)
	{
	struct usevar *p;

	p = use_var_of_buffers_ptr;
	while( p != 0 )
		{
		if( p->usevar_buf == b )
			p->usevar_buf = 0;
		p = p->usevar_next;
		}
	}

int use_variables_of_buffer_command( void )
	{
	struct usevar save_block;

	int rv;
	struct emacs_buffer *buf;

	if( check_args( 2, 2 ) )
		return 0;
	if( ! string_arg( 1 ) )
		return 0;

	buf = find_bf( ml_value->exp_v.v_string );
	if( buf == 0 )
		{
		error( u_str("Buffer %s does not exist"), ml_value->exp_v.v_string );
		return 0;
		}

	/*
	 *	Save the current use-buffer in the save block
	 *	and link it into the list.
	 */
	save_block.usevar_next = use_var_of_buffers_ptr;
	save_block.usevar_buf = use_variables_of_buffer;
	use_var_of_buffers_ptr = &save_block;

	use_variables_of_buffer = buf;

	rv = exec_prog( cur_exec->p_args[1].pa_node );

	use_var_of_buffers_ptr = save_block.usevar_next;
	use_variables_of_buffer = save_block.usevar_buf;

	return rv;
	}

static struct binding *resolve_buffer_specific
	(
	struct binding *b
	)
	{
	struct emacs_buffer *buf;

	if( use_variables_of_buffer != 0 )
		buf = use_variables_of_buffer;
	else
		buf = bf_cur;

	if( b != 0 && b->b_buffer_specific )
		{
		while( b != 0
		&& ! b->b_is_default
		&& b->b_buffer_specific
		&& b->b.b_local_to != buf )
			b = b->b_inner;

		if( b == 0 || ! b->b_buffer_specific )
			{
			error( u_str( "error resolving buffer-specific variable (internal error)" ) );
			invoke_debug();
			return 0;/* This should never happen! */
			}
		}

	return b;
	}

static struct binding *resolve_buffer_specific_assignment
	(
	struct binding *b,
	struct variablename *v,
	int default_required
	)
	{
	if( (b = resolve_buffer_specific( b )) == 0 )
		return 0;
	if( ! default_required && b->b_is_default )
		{
		struct expression *e;
		b = malloc_struct( binding  );
		b->b_exp = e = malloc_struct( expression );
		e->exp_type = ISINTEGER;
		e->exp_int = 0;
		b->b_is_system = 0;
		b->b_buffer_specific = 1;
		b->b_is_default = 0;
		if( use_variables_of_buffer != 0 )
			b->b.b_local_to = use_variables_of_buffer;
		else
			b->b.b_local_to = bf_cur;
		e->exp_v.v_string = NULL;
		b->b_thunk.thunk_int = NULL;
		b->b_rangecheck.check_int = NULL;
		b->b_inner = v->v_binding;
		v->v_binding = b;
		}

	return b;
	}

static int exec_variable( void )
	{
	struct variablename *v;
	struct binding *b;

	v = cur_exec->p_args[0].pa_name;
	if( (b = v->v_binding) == 0 )
		error( u_str( "Reference to an unbound variable: \"%s\"" ),
				v->v_name );
	else
		{
		if( b == 0
		||	(b->b_buffer_specific
			&& (b = resolve_buffer_specific( b )) == 0) )
			return 0;
		if( b->b_thunk.thunk_int != NULL )
			b->b_thunk.thunk_exp( b->b_exp, -1 );
		copy_expression( b->b_exp, ml_value, b->b_is_system );
		}
	return 0;
	}

void copy_expression
	(
	struct expression *from_e,
	struct expression *to_e,
	int is_system
	)
	{
	to_e->exp_type = from_e->exp_type;
	to_e->exp_int = from_e->exp_int;
	switch( to_e->exp_type )
	{
	case ISSTRING:
		to_e->exp_release = 1;
		to_e->exp_v.v_string = malloc_ustr( to_e->exp_int + 1 );
		memcpy
		(
		to_e->exp_v.v_string,
		from_e->exp_v.v_string,
		to_e->exp_int+ 1
		);
		if( is_system )
			to_e->exp_int = _str_len( to_e->exp_v.v_string );
		break;

	case ISINTEGER:
		to_e->exp_release = 0;
		to_e->exp_v.v_string = 0;
		if( is_system )
			to_e->exp_int = *(int *)(from_e->exp_v.v_string);
		break;

	case ISMARKER:
		to_e->exp_release = 1;
		to_e->exp_v.v_marker = copy_mark( new_mark(), from_e->exp_v.v_marker );
		break;

	case ISWINDOWS:
		to_e->exp_release = 1;
		to_e->exp_v.v_windows = from_e->exp_v.v_windows;
		from_e->exp_v.v_windows->wr_ref_count++;
		break;

	case ISARRAY:
		to_e->exp_release = 1;
		to_e->exp_v.v_array = from_e->exp_v.v_array;
		from_e->exp_v.v_array->array_ref_count++;
		break;
	default:
		error( u_str( "Variable has a bizarre type!" ) );
	}
	}

/*
 *	check_ routines of general use.
 */
int check_pos
	(
	int value,
	struct variablename *v
	)
	{
	if( value >= 0 ) return 1;

	error( u_str( "%s may only be set to a positive value" ), v->v_name );
	return 0;
	}

int check_bool
	(
	int value,
	struct variablename *v
	)
	{
	if( value == 0 || value == 1 ) return 1;

	error( u_str( "%s may only be set to a boolean value, 1 or 0" ), v->v_name );
	return 0;
	}

int check_null
	(
	int PNOTUSED(value),
	struct variablename *PNOTUSED(v)
	)
	{
	return 1;
	}

int check_read_only
	(
	int PNOTUSED(value),
	struct variablename *v
	)
	{
	error( u_str( "%s is a readonly variable" ), v->v_name );

	return 0;
	}

int setq_command( void )
	{
	return do_setq( 0 );
	}

int setq_default_command( void )
	{
	return do_setq( 1 );
	}

static int do_setq(int def_ault)
	{
	struct prognode *p;

	if( check_args( 2, 2 ) )
		return 0;

	p = cur_exec->p_args[0].pa_node;
	if( p->p_proc != &bound_variable_node )
		error( u_str( "setq expects its first argument to be a variable name." ) );
	else
		perform_set( p->p_args[0].pa_name, 2, 0, def_ault, 0, 0 );

	return 0;
	}

int set_command( void )
	{
	return do_set( 0 );
	}

int set_default_command( void )
	{
	return do_set( 1 );
	}

static int do_set(int def_ault)
	{
	int n;
	unsigned char *p;
	struct variablename *v;
	unsigned char *str;
	struct binding *b;

	n = getword( var_names,
	(( def_ault ) ?  u_str( ": set-default " ) : u_str( ": set " ) ));
	if( n < 0 )
		return 0;
	if( def_ault )
		str = u_str( ": set-default %s " );
	else
		str = u_str( ": set %s " );

	v = var_desc[n];
	b = v->v_binding;
	if( b != 0 && b->b_is_system
	&& b->b_exp->exp_type == ISINTEGER )
		p = getnbstr( str, v->v_name );
	else
		p = getstr( str, v->v_name );
	if( p == 0 )
		return 0;
	perform_set( v, 0, p, def_ault, 0, 0 );

	return 0;
	}

int setq_array_command( void )
	{
	struct prognode *p;

	if( check_args( 2, 0 ) )
		return 0;
	p = cur_exec->p_args[0].pa_node;
	if( p->p_proc != &bound_variable_node )
		error( u_str( "setq-array expects its first argument to be a variable name." ) );
	else
		perform_set( p->p_args[0].pa_name, 2, 0, 0, 1, 0 );
	return 0;
	}


/* Assign the arg th expression to v
 *	if arg eql 0 then the string 'svalue' will be used
 *	if arg is -1 then ivalue and svalue are used[ In ]which case
 *	if svalue is 0 then the expression is numeric, otherwise string.
 */
void perform_set
	(
	struct variablename *v,
	int arg,
	unsigned char *svalue,
	int setting_default,
	int is_array,
	int ivalue
	)
	{
	struct binding *b;
	struct emacs_array *a = NULL;
	struct expression *e;

	b = v->v_binding;
	if( setting_default && b == 0 )
		{
		declare( v, 0 );
		b = v->v_binding;
		}
	if( b == 0 )
		{
		error( u_str( "Attempt to set the unbound variable \"%s\"" ),
								v->v_name );
		return;
		}
	if( setting_default )
		{
		while( b->b_inner != 0 && ! b->b_is_default )
			b = b->b_inner;
		if( b->b_is_system && b->b.b_default != 0 )
			b = b->b.b_default;
		}

	if( b->b_is_system )
		{
		e = b->b_exp;
		switch( e->exp_type )
		{
		case ISARRAY:
			if( is_array )
				{
				invoke_debug();/* is there a check_ routine here for arrays */
				b->b_rangecheck.check_int( 0, v );
				}
			else
				{
				error( u_str( "Attempt to use set or setq on array \"%s\"" ), v->v_name );
				return;
				}
			break;

		case ISINTEGER:
			{
			int value;

			value = (( arg == 0 ) ?  str_to_int( svalue )
				: numeric_arg( 2 ));

			if( b->b_rangecheck.check_int( value, v ) )
				*(int *)(b->b_exp->exp_v.v_string) = value;
			break;
			}
		case ISSTRING:
			if( arg == 0 || string_arg( arg ) )
				{
				unsigned char *str;
				int len;
				if( arg != 0 )
					{
					str = ml_value->exp_v.v_string;
					len = ml_value->exp_int + 1;
					}
				else
					{
					str = svalue;
					len = _str_len( svalue ) + 1;
					}
				e = b->b_exp;
				len = min( e->exp_int, len );
				if( b->b_rangecheck.check_str( str, v ) )
					{
					memcpy( e->exp_v.v_string, str, len );
					e->exp_v.v_string[len - 1 ] = 0;
					}
				}
			break;

		case ISWINDOWS:
			if( arg != 0 )
				{
				if( window_arg( arg ) != 0 )
					b->b_rangecheck.check_win( ml_value->exp_v.v_windows, v );
				}
			else
				{
				error( u_str( "Internal error in perform_set" ) );
				invoke_debug();
				}
			break;

		default:
			error( u_str( "Internal error in perform_set" ) );
			invoke_debug();
		}
		cant_1win_opt = 1;
		bf_cur->b_mode = bf_mode;

		return;
		}

	if(	b == 0
	||	(b->b_buffer_specific
	&&	(b = resolve_buffer_specific_assignment( b, v, setting_default)) == 0) )
		return;

	if( arg == 0 )
		{
		e = b->b_exp;
		release_expr( e );
		if( svalue == 0 )
			{
			e->exp_type = ISINTEGER;
			e->exp_int = ivalue;
			}
		else
			{
			e->exp_type = ISSTRING;
			e->exp_release = 1;
			if( ivalue == 0 )
				{
				e->exp_int = _str_len( svalue );
				e->exp_v.v_string = savestr( svalue );
				}
			else
				{
				unsigned char *buf;
				buf = malloc_ustr( ivalue + 1 );
				e->exp_int = ivalue;
				e->exp_v.v_string = buf;
				memcpy( buf, svalue, ivalue );
				buf[ ivalue ] = 0;
				}
			}
		return;
		}

	e = b->b_exp;
	if( is_array )
		{
		unsigned int index;

		if( e->exp_type != ISARRAY )
			{
			error( u_str( "%s is expected to be an array." ), v->v_name );
			return;
			}

		a = e->exp_v.v_array;
		if( check_args( 2, 2 + a->array_dimensions ) )
			return;

		/* lock structure against deletion */
		a->array_ref_count = a->array_ref_count + 1;

		index = array_index( 2, a );
		if( err )
			{
			dest_array( a );
			return;
			}

		e = a->array_expr[index ];
		if( e == 0 )
			{
			e = malloc_struct( expression );
			if( e == 0 )
				{
				dest_array( a );
				return;
				}

			e->exp_int = 0;
			e->exp_type = ISINTEGER;
			e->exp_release = 0;
			e->exp_v.v_string = 0;
/*			e[ exp_refcnt ] = 1; */
			a->array_expr[index ] = e;
			}

		if( ! eval_arg( 2 + a->array_dimensions ) )
			{
			dest_array( a );/* unlock structure */
			return;
			}
		}
	else
		if( ! eval_arg( 2 ) )
			return;

	release_expr( e );
	e->exp_int = ml_value->exp_int;
	e->exp_type = ml_value->exp_type;
	switch( ml_value->exp_type )
	{
	case ISSTRING:
		if( ml_value->exp_release )
			{
			ml_value->exp_release = 0;
			e->exp_v.v_string = ml_value->exp_v.v_string;
			e->exp_release = 1;
			}
		else
			{
			e->exp_v.v_string = savestr( ml_value->exp_v.v_string );
			e->exp_release = 1;
			}
		break;

	case ISMARKER:
		if( ml_value->exp_release )
			{
			e->exp_v.v_marker = ml_value->exp_v.v_marker;
			ml_value->exp_release = 0;
			e->exp_release = 1;
			}
		else
			{
			e->exp_v.v_marker = copy_mark( new_mark(), ml_value->exp_v.v_marker );
			e->exp_release = 1;
			}
		break;

	case ISWINDOWS:
		e->exp_v.v_windows = ml_value->exp_v.v_windows;
		e->exp_v.v_windows->wr_ref_count++;
		e->exp_release = 1;
		break;

	case ISARRAY:
		if( is_array )
			error( u_str( "You cannot setq-array arrays into arrays." ) );
		else
			{
			/*
			 *	Copy the array and bump its referance count
			 */
			e->exp_v.v_array = ml_value->exp_v.v_array;
			e->exp_v.v_array->array_ref_count++;
			e->exp_release = 1;
			}
		break;

	case ISINTEGER:
		e->exp_v.v_string = 0;
		e->exp_release = 0;
		break;

	default:
		e->exp_type = ISVOID;
		e->exp_v.v_string = 0;
		e->exp_release = 0;
	}

	if( is_array )
		dest_array( a );	/* unlock structure */

	return;
	}

int print_command( void )
	{
	return do_print_command( 0 );
	}

int print_default_command( void )
	{
	return do_print_command( 1 );
	}

static int do_print_command(int def)
	{
	int n;
	struct binding *b;
	struct expression *e;
	unsigned char *str;

	n = getword( var_names,
		(( def ) ?  u_str( ": print-default ") : u_str( ": print " )) );
	if( n < 0 )
		return 0;

	b = var_desc[n]->v_binding;
	if( def )
		{
		str = u_str( "-default" );
		if( b != 0 )
			if( b->b_buffer_specific )
				while( b != 0 && b->b_is_default )
					b = b->b_inner;
			else
				b = b->b.b_default;
		}
	else
		{
		str = u_str( "" );

		if( b != 0 && b->b_buffer_specific )
			b = resolve_buffer_specific( b );
		}

	if( b == 0 )
		{
		error( u_str( "%s is not bound to a value." ), var_names[ n ] );
		return 0;
		}

	e = b->b_exp;
	switch( e->exp_type )
	{
	case ISINTEGER:
		if( b->b_thunk.thunk_int != NULL )
			b->b_thunk.thunk_int( e );
		message( u_str( ": print%s %s => %d"  ), str, var_names[n],
				(b->b_is_system ? *(int *)e->exp_v.v_string
				: e->exp_int));
		break;

	case ISSTRING:
		if( b->b_thunk.thunk_int != NULL )
			b->b_thunk.thunk_int( e );
		message( u_str( ": print%s %s => \"%s\""  ), str,
				var_names[n], e->exp_v.v_string);
		break;

	case ISMARKER:
		{
		struct marker *m;
		if( b->b_thunk.thunk_int != NULL )
			b->b_thunk.thunk_int( e );
		m = e->exp_v.v_marker;
		if( m != 0 && m->m_buf != 0 )
			{
			struct emacs_buffer *old;
			old = bf_cur;
			message( u_str( ": print%s %s = > Marker (\"%s\", %d)" ),
				str, var_names[n],
				m->m_buf->b_buf_name,
				to_mark( m ) );
			set_bfp( old );
			}
		else
			{
			message( u_str( ": print%s %s => marker for deleted buffer" ),
					str, var_names[n]);
			}
		}
		break;

	case ISWINDOWS:
		message( u_str( ": print%s %s => A set of windows"  ),
				str, var_names[n]);
		break;

	case ISARRAY:
		message( u_str( ": print%s %s => An array"  ),
				str, var_names[n]);
		break;

	default:
		error( u_str( ": print%s %s => Something very odd - internal error!"  ),
				str, var_names[n]);
		invoke_debug();
	}

	return 0;
	}

int provide_prefix_argument( void )
	{
	if( check_args( 2, 2 ) )
		return 0;
	arg = numeric_arg( 1 );
	arg_state = prepared_arg;
	if( err )
		return 0;
	else
		return exec_prog( cur_exec->p_args[1].pa_node );
	}

int return_prefix_argument( void )
	{
	arg = getnum( u_str( ": return-prefix-argument " ) );
	arg_state = prepared_arg;
	return 0;
	}

/* print out an MLisp expression( de-compile it ) into the current buffer */
void print_expr
	(
	struct prognode *p,
	int depth
	)

	{
	struct bound_name *n;
	if( p == 0 )
		{
		ins_str( u_str( "<< Command Level >>" ) );
		return;
		}
	n = p->p_proc;
	if( n == &bound_number_node )
		{
		unsigned char buf[50];
		sprintfl( &buf[0], sizeof( buf ),
			u_str( "%d" ), p->p_args );
		ins_str( &buf[0] );
		return;
		}
	if( n == &bound_string_node )
		{
		ins_cstr( u_str( "\"" ), 1 );
		ins_cstr( &p->p_args[1].pa_char, p->p_args[0].pa_int );
		ins_cstr( u_str( "\"" ), 1 );
		return;
		}
	if( n == &bound_variable_node )
		{
		ins_str( p->p_args[0].pa_name->v_name );
		return;
		}
	ins_cstr( u_str( "(" ), 1 );
	if( depth >= 0 )
		{
		int i;

		ins_str( n->b_proc_name );
		for( i=0; i<=p->p_nargs - 1; i += 1 )
			{
			ins_cstr( u_str( " " ), 1 );
			print_expr( p->p_args[i].pa_node, depth - 1 );
			}
		}
	ins_cstr( u_str( ")" ), 1 );
	}

/* Throw away any expression evaluation so that the current function returns
 * no value */
void void_result( void )
	{
	release_expr( ml_value );
	ml_value->exp_type = ISVOID;
	}

int array_command( void )
	{
	int i;
	unsigned int low_bound[ARRAY_MAX_DIMENSION];
	unsigned int size[ARRAY_MAX_DIMENSION];
	int dims;
	int low;
	int high;
	int total_size;
	struct emacs_array *array;

	if( check_args( 2, 2 * ARRAY_MAX_DIMENSION ) )
		return 0;

	total_size = 1;
	dims = cur_exec->p_nargs / 2;
	for( i=0; i<=dims - 1; i += 1 )
		{
		low = numeric_arg( 1 + i*2 );
		if( err )
			return 0;
		high = numeric_arg( 1 + i*2 + 1 );
		if( err )
			return 0;
		if( low > high )
			{
			error( u_str( "Array lower bound must be less then higher bound" ) );
			return 0;
			}
		low_bound[i] = low;
		size[i] = high - low + 1;
		total_size = total_size * size[i];
		}

	array = (struct emacs_array *)malloc( sizeof( struct emacs_array ) + total_size * 4, malloc_type_struct_emacs_array );
	if( array == 0 )
		{
		error( u_str( "Insufficent memory to allocate array of %d elements"  ),
				total_size);
		return 0;
		}

	array->array_ref_count = 1;
	array->array_dimensions = dims;
	array->array_total_size = total_size;

	/*
	 *	Copy the bound into the array structure
	 */
	memmove( array->array_lower_bound, &low_bound[0], dims*sizeof(int) );
	memmove( array->array_size, &size[0], dims*sizeof(int) );

	/*
	 *	start out with all expression pointers 0
	 *	because malloc zeros memory
	 */

	do_release( ml_value );
	ml_value->exp_release = 1;
	ml_value->exp_type = ISARRAY;
	ml_value->exp_int = 0;
	ml_value->exp_v.v_array = array;

	return 0;
	}

static void dest_array
	(
	struct emacs_array *a
	)
	{
	int i;
	struct expression **expr;

	a->array_ref_count = a->array_ref_count - 1;

	if( a->array_ref_count != 0 )
		return;

	expr = a->array_expr;

	for( i=0; i<=a->array_total_size - 1; i += 1 )
		{
		if( expr[0] != 0 )
			{
			do_release( expr[0] );
			free( expr[0] );
			}
		expr++;
		}

	free( a );
	}

int fetch_array_command( void )
	{
	struct prognode *p;
	struct binding *b;
	unsigned int index;
	struct variablename *v;
	struct expression *e;
	struct emacs_array *a;

	if( check_args( 2, 0 ) )
		return 0;

	p = cur_exec->p_args[0].pa_node;
	if( p->p_proc != &bound_variable_node )
		{
		error( u_str( "fetch-array expects its first argument to be a variable name." ) );
		return 0;
		}

	v = p->p_args[0].pa_name;
	if( (b = v->v_binding) == 0 )
		{
		error( u_str( "Reference to an unbound variable: \"%s\"" ), v->v_name );
		return 0;
		}

	if( b == 0
	||
		(b->b_buffer_specific
		&& (b = resolve_buffer_specific( b)) == 0) )
		return 0;
	e = b->b_exp;
	if( e->exp_type != ISARRAY )
		{
		error( u_str( "fetch-array expects to be called with an array variable" ) );
		return 0;
		}

	a = e->exp_v.v_array;

	/* set up for an array access to a system complex variable */
	if( b->b_thunk.thunk_arr != NULL )
		b->b_thunk.thunk_arr( a, 1 );

	if( check_args( 2, 1 + a->array_dimensions ) )
		return 0;

	/*	Lock the array struct */
	a->array_ref_count = a->array_ref_count + 1;

	index = array_index( 2, a );
	if( err )
		{
		dest_array( a );
		return 0;
		}

	if( b->b_thunk.thunk_arr != NULL )
		{
		b->b_thunk.thunk_arr( a, 0, index );
		index = 0;
		if( err )
			{
			dest_array( a );
			return 0;
			}
		}

	e = a->array_expr[index ];
	release_expr( ml_value );
	if( e == 0 )
		{
		/* return a value of 0 for uninitialised elements */
		ml_value->exp_type = ISINTEGER;
		ml_value->exp_int = 0;
		}
	else
		copy_expression( e, ml_value, 0 );

	dest_array( a );
	return 0;
	}
/*
 *	Return the array index value after check_ing bounds.
 *	returns -1 on error and geq 0 if o[ k ].
 */
static int array_index
	(
	unsigned int arg,
	struct emacs_array *a
	)
	{
	int i;
	int index;
	int subscript;
	int *low;
	int *size;

	low = a->array_lower_bound;
	size = a->array_size;
	index = 0;

	for( i=0; i<=a->array_dimensions - 1; i += 1 )
		{
		subscript = numeric_arg( arg + i );
		if( err )
			return 0;

		subscript = subscript - low[ i ];
		if( subscript < 0 || subscript >= size[ i ] )
			{
			error
			(
			u_str( "Array subscript error - subscript %d is %d, bounds are %d to %d" ),
			i + 1, subscript + low[i],
			low[i], low[i] + size[i] - 1 );
			return 0;
			}

		index = index * size[i] + subscript;
		}

	return index;
	}

/*
 *	This function returns an array structure filled in from
 *	the arguments.
 *
 *	starting at args are dims sizes followed by dims lower bounds
 * 	This version create 2D arrays only
*/
struct emacs_array *create_array
	(
	int dims,	/* number of dimensions */
	...		/* sizes and lower bounds... */
	)
	{
	int i;
	struct emacs_array *array;
	int total_size;
	va_list args;

	va_start( args, dims );
	total_size = 1;
	for( i=0; i<=dims - 1; i += 1 )
		total_size = total_size * va_arg( args, int );

	array = (struct emacs_array *)malloc
		(
		sizeof( struct emacs_array ) + total_size*sizeof( struct expression * ),
		malloc_type_struct_emacs_array
		);
	if( array == NULL )
		return 0;

	array->array_ref_count = 1;
	array->array_dimensions = dims;
	array->array_total_size = total_size;

	/* Copy the sizes and the lower bounds into the array structure */
	va_start( args, dims );
	for( i=0; i<dims; i++ )
		array->array_size[i] = va_arg( args, int );
	for( i=0; i<dims; i++ )
		array->array_lower_bound[i] = va_arg( args, int );

	/* init the expression pointers */
	/* which was done by malloc */

	return array;
	}

/*
 *	This function return an expression structure filled
 *	in from val, string and release
 */
struct expression *create_expression(int type, int val, unsigned char *string, int release)
	{
	struct expression *e;

	e = malloc_struct( expression );
	if( e == 0 )
		return 0;
	e->exp_type = type;
	e->exp_int = val;
	e->exp_v.v_string = string;
	e->exp_release = release;

	return e;
	}

/*
 *	type-of command
 *		returns a string describing the the type of the
 *		expression that is its parameter
 */
int type_of_expression_command( void )
	{
	int type;

	if( check_args( 1, 1 ) )
		return 0;
	if( ! eval_arg( 1 ) )
		return 0;

	type = ml_value->exp_type;
	release_expr( ml_value );

	switch( type )
	{
	case ISVOID: 	ml_value->exp_v.v_string = u_str( "void" ); break;
	case ISINTEGER: ml_value->exp_v.v_string = u_str( "integer" ); break;
	case ISSTRING: 	ml_value->exp_v.v_string = u_str( "string" ); break;
	case ISMARKER: 	ml_value->exp_v.v_string = u_str( "marker" ); break;
	case ISWINDOWS: ml_value->exp_v.v_string = u_str( "windows" ); break;
	case ISARRAY: 	ml_value->exp_v.v_string = u_str( "array" ); break;
	default:
		{
		error( u_str( "type-of-variable called with bizzare expression" ) );
		return 0;
		}
	}
	ml_value->exp_type = ISSTRING;
	ml_value->exp_release = 0;
	ml_value->exp_int = _str_len( ml_value->exp_v.v_string );

	return 0;
	}

int bounds_of_array_command( void )
	{
	int i;
	struct expression **e;
	struct emacs_array *array;
	struct emacs_array *bounds;
	int dims;

	if( check_args( 1, 1 ) )
		return 0;
	if( ! eval_arg( 1 ) )
		return 0;

	if( ml_value->exp_type != ISARRAY )
		{
		error( u_str( "bounds-of-array expects its argument to be an array" ) );
		return 0;
		}

	array = ml_value->exp_v.v_array;
	release_expr( ml_value );

	dims = array->array_dimensions;
	bounds = create_array
		(
		2,	/* dimensions */
		2,	/* first dimension size */
		dims + 1,/* second dimension size */
		1,	/* first low bound */
		0	/* second low bound */
		);

	e = bounds->array_expr;

	/* 1, 0 has the number of dimension in it */
	*e++ = create_expression( ISINTEGER, dims, 0, 0 );

	/* row 1 has the lower bounds in it */
	for( i=0; i<=dims - 1; i++ )
		*e++ = create_expression
			(
			ISINTEGER,
			array->array_lower_bound[i],
			0, 0
			);
	/* 2, 0 has the number of elements in the array */
	e[0] = create_expression( ISINTEGER, array->array_total_size, 0, 0 );
	e++;
	/* row 2 has the upper bounds in it */
	for( i=0; i<=dims - 1; i += 1 )
		*e++ = create_expression
			(
			ISINTEGER,
			array->array_lower_bound[i ] +
				array->array_size[i ] - 1,
			0, 0
			);

	ml_value->exp_type = ISARRAY;
	ml_value->exp_release = 1;
	ml_value->exp_v.v_array = bounds;

	return 0;
	}

/* Fetch a variable as either a string or a number */
static unsigned char no_variable [] = "No variable exists with name \"%s\"";
static unsigned char unbound_var [] = "\"%s\" is not bound to a value.";
static unsigned char fetch_type [] = "\"%s\" is %s and cannot be coerced to %s.";
static unsigned char int_type [] = "an integer";
static unsigned char str_type [] = "a string";
int fetch_var (unsigned char *name, int *iresp, unsigned char **cresp)
    {
    struct variablename *nm;
    struct binding *b;
    struct expression *e;
    int var;

    if ((nm = lookup_variable (name)) == NULL)
	{
	error (no_variable, name);
	return 0;
	}

    b = nm->v_binding;
    if (b != 0 && b->b_buffer_specific)
	b = resolve_buffer_specific (b);
    if( b == 0 )
	{
	error (unbound_var, name);
	return 0;
	}

    e = b->b_exp;
    switch (e->exp_type)
	{
	case ISINTEGER:
	    if (b->b_thunk.thunk_int != NULL)
		b->b_thunk.thunk_int (e);
	    var = b->b_is_system ? e->exp_v.v_string [0] : e->exp_int;
	    if (iresp)
		*iresp = var;
	    else
		{
		static unsigned char buf [20];
		sprintfl (buf, sizeof (buf), u_str ("%d"), var);
		*cresp = buf;
		}
	    break;

	case ISSTRING:
	    if (b->b_thunk.thunk_int != NULL)
		    b->b_thunk.thunk_int (e);
	    if (iresp)
		{
		*iresp = str_to_int (e->exp_v.v_string);
		if (err)
		    return 0;
		}
	    else
		*cresp = e->exp_v.v_string;
	    break;

    case ISMARKER:
	error (fetch_type, name, "a marker", iresp ? int_type : str_type);
	return 0;

    case ISWINDOWS:
	error (fetch_type, name, "a window", iresp ? int_type : str_type);
	return 0;

    case ISARRAY:
	error (fetch_type, name, "an array", iresp ? int_type : str_type);
	return 0;

    default:
	error (fetch_type, name, "unknown", iresp ? int_type : str_type);
	return 0;
	}

    return 1;
    }

/* Set a variable from the supplied values */
void set_var (unsigned char *name, int iresp, unsigned char *cresp)
    {
    struct variablename *v;
    struct binding *b;
    struct expression *e;

    if ((v = lookup_variable (name)) == NULL)
	{
	error (no_variable, name);
	return;
	}

    b = v->v_binding;
    if( b == 0 )
	{
	error (u_str ("Attempt to set the unbound variable \"%s\"" ), v->v_name);
	return;
	}

    if (b->b_is_system)
	{
	e = b->b_exp;
	switch (e->exp_type)
	    {
	    case ISINTEGER:
		{
		int value;

		value = cresp ? str_to_int (cresp) : iresp;
		if (err)
		    return;

		if (b->b_rangecheck.check_int (value, v))
			*(int *)(b->b_exp->exp_v.v_string) = value;
		break;
		}

	    case ISSTRING:
		{
		unsigned char *str, buf[20];
		if (cresp)
		    str = cresp;
		else
		    {
		    sprintfl (buf, sizeof (buf), u_str ("%d"), iresp);
		    str = buf;
		    }

		if (b->b_rangecheck.check_str (str, v))
		    {
		    int len = strlen (s_str (str));

		    memcpy (e->exp_v.v_string, str, len);
		    e->exp_v.v_string[len] = 0;
		    }
		break;
		}

	    case ISARRAY:
		error (fetch_type, name, iresp ? int_type : str_type, "an array");
		break;

	    case ISWINDOWS:
		error (fetch_type, name, iresp ? int_type : str_type, "a window");
		break;

	    default:
		error (fetch_type, name, iresp ? int_type : str_type, "unknown");
		break;
	    }
	cant_1win_opt = 1;
	bf_cur->b_mode = bf_mode;

	return;
	}

    if	(b == NULL || (b->b_buffer_specific && 
      (b = resolve_buffer_specific_assignment (b, v, 0)) == NULL))
	return;

    e = b->b_exp;
    release_expr (e);
    if (cresp == NULL)
	{
	e->exp_type = ISINTEGER;
	e->exp_int = iresp;
	}
    else
	{
	e->exp_type = ISSTRING;
	e->exp_release = 1;
	e->exp_int = _str_len (cresp);
	e->exp_v.v_string = savestr (cresp);
	}
    return;
    }

void init_lisp( void )
	{
	bound_number_node.b_bound.b_proc = exec_number;
	bound_number_node.b_proc_name = u_str( "execute-number" );
	bound_variable_node.b_bound.b_proc = exec_variable;
	bound_variable_node.b_proc_name = u_str( "execute-variable" );
	bound_string_node.b_bound.b_proc = exec_string;
	bound_string_node.b_proc_name = u_str( "execute-string" );
	bound_star_defun_node.b_bound.b_proc = star_define_function;
	bound_star_defun_node.b_proc_name = u_str( "*defun" );
	}
