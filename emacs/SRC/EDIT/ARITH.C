/*module arith
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/* functions to handle MLisp arithmetic */

#include <emacs.h>


/*forward*/ int check_args(int min, int max);
/*forward*/ int eval_arg(int n);
/*forward*/ struct windowring * window_arg(int n);
/*forward*/ int numeric_arg(int n);
/*forward*/ int string_arg(int n);
/*forward*/ int bin_setup( void );
/*forward*/ int add_command( void );
/*forward*/ int not_command( void );
/*forward*/ int subtract_command( void );
/*forward*/ int times_command( void );
/*forward*/ int divide_command( void );
/*forward*/ int mod_command( void );
/*forward*/ int shift_left_command( void );
/*forward*/ int shift_right_command( void );
/*forward*/ int and_command( void );
/*forward*/ int or_command( void );
/*forward*/ int xor_command( void );
/*forward*/ static int compare_setup( void );
/*forward*/ static void compare_return(int val);
/*forward*/ int equal_command( void );
/*forward*/ int not_equal_command( void );
/*forward*/ int less_command( void );
/*forward*/ int less_equal_command( void );
/*forward*/ int greater_command( void );
/*forward*/ int greater_equal_command( void );
/*forward*/ void init_arith( void );

/* check_ that we were given at least min and at most max arguments.
 * Returns true iff there was an error. */

int check_args(int min, int max)
	{
	struct prognode *p;

	p = cur_exec;
	if( err )
		return 1;

	if( p == 0 )
		{
		if( min != 0 || max != 0 )
			{
			error(u_str("No arguments provided to MLisp function!"));
			return 1;
			}

		return 0;
		}

	if( (min != 0 && p->p_nargs < min)
	||  (max != 0 && p->p_nargs > max) )
		{
		error (u_str("Too %s arguments to \"%s\""),
			(( p->p_nargs < min ) ?  u_str("few") : u_str("many")),
			p->p_proc->b_proc_name );
		return 1;
		}
	return 0;
	}

/* Evaluate the n th argument. Returns true if the evaluation was successful */
int eval_arg(int n)
	{
	struct prognode *p;
	struct prognode *arg;

	p = cur_exec;
	if( err )
		return 0;
	if( p == 0 || p->p_nargs < n )
		{
		error (u_str("Missing argument %d to %s"), n,
			(( p != 0 ) ?  p->p_proc->b_proc_name 
				: u_str("MLisp function")));
		return 0;
		}
	arg = p->p_args[n - 1].pa_node;
	exec_prog( arg );
	if( err )
		return 0;
	if( ml_value->exp_type == ISVOID )
		{
		error(u_str("\"%s\" did not return a value; \"%s\" was expecting it to."),
			arg->p_proc->b_proc_name ,
			p->p_proc->b_proc_name );
		return 0;
		}
	return 1;
	}

/* Evaluate and return the n th window argument */
struct windowring * window_arg(int n)
	{
	if( ! eval_arg( n ) )
		return 0;
	switch( ml_value->exp_type )
	{
	case ISWINDOWS:
		return ml_value->exp_v.v_windows;
	default:
		{
		error (u_str("windows argument expected."));
		return 0;
		}
	}
	}

/* Evaluate and return the n th numeric argument */
int numeric_arg(int n)
	{
	if( ! eval_arg( n ) )
		return 0;
	switch( ml_value->exp_type )
	{
	case ISINTEGER:
		return ml_value->exp_int;

	case ISSTRING: 	/* this is a cop-out */
		{
		unsigned char *p;
		int neg;

		p = ml_value->exp_v.v_string;
		neg = 0;

		while( isspace( p[0] ) )
			p = &p[1];

		if( p[0] == '+' || p[0] == '-' )
			{
			neg = p[0] == '-';
			p = &p[1];
			}
		while( isspace( p[0] ) )
			p = &p[1];
		n = 0;
		while( isdigit( p[0] ) || isspace( p[0] ) )
			{
			if( isdigit( p[0] ) )
				n = n * 10 + p[0]- '0';
			p = &p[1];
			}
		if( p[0] != 0 )
			error (u_str("String to integer conversion error: \"%s\""),
					ml_value->exp_v.v_string);
		if( neg )
			n = -n;
		release_expr( ml_value );
		ml_value->exp_type= ISINTEGER;
		return n;
		}

	case ISMARKER:
		{
		struct emacs_buffer *old;

		old = bf_cur;
		n = to_mark( ml_value->exp_v.v_marker );
		release_expr( ml_value );
		ml_value->exp_type = ISINTEGER;
		set_bfp( old );
		return n;
		}

	default:
		error (u_str("Numeric argument expected."));
	}

	return 0;
	}

/* Evaluate and return the n th string argument in ml_value (returns
 * true if all is well) */
int string_arg(int n)
	{
	if( ! eval_arg( n ) )
		return 0;
	switch( ml_value->exp_type )
	{
	case ISMARKER:
		{
		struct marker *m;
		struct emacs_buffer *b;

		m = ml_value->exp_v.v_marker;
		b = (( m != 0 ) ?  m->m_buf : 0);
		release_expr( ml_value );
		ml_value->exp_v.v_string = (( b != 0 ) ?  b->b_buf_name
			: u_str("<Bizarre marker>"));
		ml_value->exp_int = _str_len( ml_value->exp_v.v_string );
		ml_value->exp_type = ISSTRING;
		ml_value->exp_release = 0;
		return 1;
		}
	case ISINTEGER:
		{
		/* swine, using own again */
		static unsigned char buf[20];

		sprintfl( buf, sizeof( buf ),
			u_str("%d"), ml_value->exp_int );
		ml_value->exp_type = ISSTRING;
		ml_value->exp_int = _str_len( buf );
		ml_value->exp_v.v_string = buf;
		ml_value->exp_release = 0;
		return 1;
		}
	case ISSTRING:
		return 1;
	default:
		error (u_str("String argument expected."));
	}
	return 0;
	}

/* set up for a simple binary operator */
int bin_setup( void )
	{
	if( check_args( 1, 0 ) )
		return 0;
	return numeric_arg( 1 );
	}

int add_command( void )
	{
	int result;
	int i;

	result = bin_setup();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result = result + numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int not_command( void )
	{
	ml_value->exp_int = numeric_arg( 1 ) == 0;
	return 0;
	}

int subtract_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err )
		return 0;
	if( cur_exec->p_nargs == 1 )
		result = -result;
	else
	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result = result - numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int times_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result = result * numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int divide_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			{
			int denom;

			denom = numeric_arg( i );
			if( denom == 0 )
				error (u_str("Division by zero"));
			else
				result = result / denom;
			}
	ml_value->exp_int = result;
	return 0;
	}

int mod_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			{
			int denom;
			denom = numeric_arg( i );
			if( denom == 0 )
				error (u_str("Mod by zero"));
			else
				result = result % denom;
			}
	ml_value->exp_int = result;
	return 0;
	}

int shift_left_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result = result << numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int shift_right_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result = result >> numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int and_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result &= numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int or_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result |= numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}

int xor_command( void )
	{
	int result;
	int i;

	result = bin_setup ();
	if( err ) return 0;

	for( i=2; i<=cur_exec->p_nargs; i++ )
		if( err )
			break;
		else
			result ^= numeric_arg( i );
	ml_value->exp_int = result;
	return 0;
	}


static unsigned char *g_left_s;	/* left string operand to a comparison operator */
static int g_left_i;	/* left integer operand to a comparison operator */

/* Setup to do a comparison operator[ Comparison ]is
 * lexicographic if both operands are strings, numeric
 * otherwise */
static int compare_setup( void )
	{
	unsigned char *leftS;
	struct emacs_buffer *old;
	int leftI;

	old = bf_cur;
	if( ! eval_arg( 1 ) )
		return 0;
	leftI = ml_value->exp_int;
	switch( ml_value->exp_type )
	{
	case ISINTEGER:
		leftS = 0;
		break;
	case ISSTRING:
		if( ml_value->exp_release )
			{
			leftS = ml_value->exp_v.v_string;
			ml_value->exp_release = 0;
			}
		else
			leftS = savestr( ml_value->exp_v.v_string );
		break;
	case ISMARKER:
		{
		leftI = to_mark( ml_value->exp_v.v_marker );
		leftS = 0;
		release_expr( ml_value );
		set_bfp( old );
		break;
		}
	default:
		{
		error (u_str("Illegal operand to comparison operator"));
		return 0;
		}
	}

	if( ! eval_arg( 2 ) )
		{
		if( leftS != 0 )
			free( leftS );
		return 0;
		}
	if(	(ml_value->exp_type == ISINTEGER
		|| ml_value->exp_type == ISMARKER)
	&& leftS != 0 )
		{
		leftI = atoi( s_str(leftS) );
		free( leftS );
		leftS = 0;
		}
	if( leftS == 0 && ml_value->exp_type == ISSTRING )
		{
		ml_value->exp_int = atoi( s_str(ml_value->exp_v.v_string) );
		release_expr( ml_value );
		ml_value->exp_type = ISINTEGER;
		}
	if( ml_value->exp_type == ISMARKER )
		{
		int n;
		n = to_mark (ml_value->exp_v.v_marker);
		release_expr( ml_value );
		set_bfp( old );
		ml_value->exp_int = n;
		ml_value->exp_type = ISINTEGER;
		}
	g_left_s = leftS;
	g_left_i = leftI;
	return 1;
	}

static void compare_return(int val)
	{
	release_expr( ml_value );
	if( g_left_s != 0 )
		free( g_left_s );
	g_left_s = 0;
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = val;
	}

int equal_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp( g_left_s, ml_value->exp_v.v_string) == 0
		: g_left_i == ml_value->exp_int) );
	return 0;
	}

int not_equal_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp ( g_left_s, ml_value->exp_v.v_string) != 0
		: g_left_i != ml_value->exp_int) );
	return 0;
	}

int less_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp ( g_left_s, ml_value->exp_v.v_string) < 0
		: g_left_i < ml_value->exp_int) );
	return 0;
	}

int less_equal_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp ( g_left_s, ml_value->exp_v.v_string) <= 0
		: g_left_i <= ml_value->exp_int) );
	return 0;
	}

int greater_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp ( g_left_s, ml_value->exp_v.v_string) > 0
		: g_left_i > ml_value->exp_int) );
	return 0;
	}

int greater_equal_command( void )
	{
	if( compare_setup() )
		compare_return
		(
		(( g_left_s != 0 ) ?  _str_cmp ( g_left_s, ml_value->exp_v.v_string) >= 0
		: g_left_i >= ml_value->exp_int) );
	return 0;
	}

void init_arith( void )
	{
	return;
	}
