/*module lispfunc
 *	(
 *	ident	= 'V5.0-001',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *
 * 	Copyright (c) 1982, 1983, 1984, 1985, 1987
 *		Barry A. Scott and Nick Emery
 */

#include <emacs.h>
#include <time.h>

unsigned char *marker_or_string = u_str("region-to-string expects an integer of marker as its parameter");
unsigned char *no_mem_str = u_str ("Out of memory.");
unsigned char *unknown_str = u_str ("Unknown");

unsigned char parent_path[MAXPATHLEN+1];

static unsigned char converted_system_name[40];
static unsigned char users_full_name[35];
#ifdef vms
static unsigned char lognam[256];    /* terminal_names and terminal_idents are equivalence ordered */
static unsigned char *terminal_names[] = {
 u_str("vt05"),u_str("vk100"),u_str("vt173"),u_str("tq_bts"),
 u_str("tek401x"),u_str("ft1"),u_str("ft2"),u_str("ft3"),
 u_str("ft4"),u_str("ft5"),u_str("ft6"),u_str("ft7"),u_str("ft8"),
 u_str("la36"),u_str("la120"),u_str("la34"),u_str("la38"),
 u_str("la12"),u_str("la100"),u_str("lqp02"),u_str("la84"),
 u_str("vt52"),u_str("vt55"),u_str("vt100"),u_str("vt101"),
 u_str("vt102"),u_str("vt105"),u_str("vt125"),u_str("vt131"),
 u_str("vt132"),u_str("vt200_series"),u_str("pro_series"),
 u_str("unknown")};
static int terminal_idents[] = {
 1,2,3,4,10,16,17,18,19,20,21,22,23,32,33,34,35,36,
 37,38,39,64,65,96,97,98,99,100,101,102,110,111,0};
#endif

/*forward*/ static int get_tty_something( unsigned char *something );


int baud_rate_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = tt->t_baud_rate;
    return 0;
    }

int dot_val_command( void )
    {
    ml_value->exp_type = ISMARKER;
    ml_value->exp_v.v_marker = new_mark ();
    set_mark (ml_value->exp_v.v_marker, bf_cur, dot, 0);
    ml_value->exp_release = 1;
    return 0;
    }
int mark_val_command( void )
    {
    if( bf_cur->b_mark != 0 )
	{
	ml_value->exp_type = ISMARKER;
        ml_value->exp_v.v_marker = new_mark ();
        set_mark (ml_value->exp_v.v_marker, bf_cur, to_mark (bf_cur->b_mark), 0);
        ml_value->exp_release = 1;
	}
    else
	error (u_str ("no mark set in buffer \"%s\""), bf_cur->b_buf_name);

    return 0;
    }
int buf_size_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = num_characters + 1 - first_character;
    return 0;
    }
int cur_col_func_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = calc_col();
    return 0;
    }
int this_indent_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = cur_indent ();
    return 0;
    }
int bobp_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = dot <= first_character;
    return 0;
    }
int eobp_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = dot > num_characters;
    return 0;
    }
int bolp_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = dot <= first_character ||
        char_at (dot - 1) == '\n';
    return 0;
    }
int eolp_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = dot > num_characters ||
        char_at (dot) == '\n';
    return 0;
    }
int foll_char_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = ( dot > num_characters ? 0 : char_at (dot));

    return 0;
    }
int prev_char_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = ( dot <= first_character ? 0 : char_at (dot - 1));

    return 0;
    }
int fetch_last_key_struck_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = last_key_struck;
    return 0;
    }
int fetch_previous_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = previous_command;
    return 0;
    }
int recursion_depth_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = recursive_edit_depth;
    return 0;
    }
int nargs_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = execution_root.es_cur_exec != 0 ?
	execution_root.es_cur_exec->p_nargs : 0;

    return 0;
    }
int interactive_command( void )
    {
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = execution_root.es_cur_exec == 0;
    return 0;
    }
int current_buffer_name_command( void )
    {
    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = bf_cur->b_buf_name;
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len (ml_value->exp_v.v_string);
    return 0;
    }
int current_file_name_command( void )
    {
    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = bf_cur->b_fname != 0 ? bf_cur->b_fname : u_str("");
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len (ml_value->exp_v.v_string);

    return 0;
    }
int users_login_name_command( void )
    {
    if( users_name == NULL )
	if( (users_name = (unsigned char *)cuserid (0)) == NULL )
	    users_name = unknown_str;

    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = users_name;
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len (ml_value->exp_v.v_string);
    return 0;
    }

int users_full_name_command( void )
    {
    if( users_full_name[0] == 0 )
	{
	if( users_name == NULL )
	    if( (users_name = (unsigned char *)cuserid (0)) == NULL )
		_str_cpy( users_name, unknown_str );

	get_user_full_name( users_name, users_full_name );
	}

    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = users_full_name;
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len (ml_value->exp_v.v_string);

    return 0;
    }

int return_system_name_command( void )
    {
    if( converted_system_name[0] == 0 )
	get_system_name( converted_system_name );

    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = converted_system_name;
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len (ml_value->exp_v.v_string);

    return 0;
    }

int current_time( void )
    {
    time_t now = time(0);

    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = (unsigned char*)ctime (&now);
    ml_value->exp_v.v_string[24] = 0;
    ml_value->exp_release = 0;
    ml_value->exp_int = 24;

    if( interactive )
	message( u_str("%s"), ml_value->exp_v.v_string );

    return 0;
    }
int arg_command( void )
    {
    int i = numeric_arg(1);
    struct prognode *p = execution_root.es_cur_exec;
    struct execution_stack old;

    if( err )
	return 0;

    if( p == 0 || execution_root.es_dyn_parent == 0 )
     	{
	if( string_arg (2) )
	    {
	    last_arg_used = 0;
	    return get_tty_something (u_str ("string"));
	    }
	return 0;
    	}
    if( i > p->p_nargs || i <= 0 )
     	{
	error (u_str ("Bad argument index: (arg %d)"), i);
	return 0;
    	}
    old = execution_root;
    execution_root = *execution_root.es_dyn_parent;
    exec_prog( p->p_args[ i - 1 ].pa_node );
    execution_root = old;

    return 0;
    }

int dot_is_visible( void )
    {
    int windowtop = to_mark(wn_cur->w_start);

    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = dot >= windowtop
        && dot - (dot  >  num_characters) <
	scan_bf ('\n', windowtop, wn_cur->w_height - 1);

    return 0;
    }
int substr_command( void )
    {
    int pos = numeric_arg(2);
    int n = numeric_arg(3);
    unsigned char *p;

    if( string_arg (1) != 0 )
     	{
	if( pos < 0 )
	    pos = ml_value->exp_int + 1 + pos;

	if( pos <= 0 )
	    pos = 1;

	if( n < 0 )
	    {
	    n = ml_value->exp_int + n;
	    if( n < 0 )
		n = 0;
	    }
	if( pos + n - 1  >  ml_value->exp_int )
	    {
	    n = ml_value->exp_int + 1 - pos;
	    if( n < 0 )
		n = 0;
	    }
	p = malloc_ustr(n + 1);
	memcpy( p,  ml_value->exp_v.v_string + pos - 1, n );
	p[n] = 0;
	release_expr( ml_value );
	ml_value->exp_int = n;
	ml_value->exp_release = 1;
	ml_value->exp_v.v_string = p;
    	}

    return 0;
    }

int to_col_command( void )
    {
    int n = getnum(u_str(": to-col "));

    if( ! err )
	to_col (n);

    return 0;
    }
int char_to_string( void )
    {
    unsigned char *p;
    int n = getnum(u_str(": unsigned char-to-string "));

    release_expr (ml_value);
    ml_value->exp_type = ISSTRING;
    ml_value->exp_release = 1;
    ml_value->exp_int = 1;
    ml_value->exp_v.v_string = p = malloc_ustr (2);
    p[0] = (unsigned char)n;
    p[1] = 0;
    return 0;
    }

int string_to_char( void )
	{
	unsigned char *s = getstr(u_str(": string-to-unsigned char "));
	int the_char;

	/* must test s before release_expr */
	the_char = s != 0 ? s[0] : 0;

	/* invalidate s */
	release_expr (ml_value);

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = the_char;

	return 0;
	}

int insert_character( void )
    {
    self_insert ( (unsigned char)getnum (u_str(": insert-character ")));
    return 0;
    }

static int get_tty_something( unsigned char *something )
    {
    unsigned char *prompt1 = getstr( u_str(": get-tty-%s (prompt) "), something);
    struct prognode *lcur_exec = cur_exec;
    unsigned char *answer = NULL;

    if( prompt1 != 0 )
     	{
	int i;
	unsigned char prompt[512];

	_str_cpy (prompt, prompt1);
	cur_exec = 0;
	release_expr (ml_value);
	switch( something[0] )
	{
	case 's': 		/* get-tty-string */
	    answer = getstr (u_str ("%s"), prompt);
	    break;
	case 'b': 		/* get-tty-buffer */
	    i = getword (buf_names, prompt);
	    answer = i < 0 ? NULL : buf_names[i];
	    break;

	case 'c': 		/* get-tty-command */
	    i = getword (mac_names_ref, prompt);
	    answer = i < 0 ? NULL : mac_names[i];
	    break;

	case 'v': 		/* get-tty-variable */
	    i = getword (var_names, prompt);
	    answer =  i < 0 ? NULL : var_names[i];
	    break;

	case 'f':
	    answer = call_getescfile(u_str("%s"), prompt );
	    break;
	}

	cur_exec = lcur_exec;
	if( answer != NULL )
	    {
	    ml_value->exp_int = _str_len (answer);
	    ml_value->exp_v.v_string = savestr (answer);
	    ml_value->exp_type = ISSTRING;
	    ml_value->exp_release = 1;
	    }
	else
	    ml_value->exp_type = ISVOID;
    	}
    return 0;
    }
int get_tty_string( void )
    {
    return get_tty_something (u_str ("string"));
    }

int get_tty_command( void )
    {
    return get_tty_something (u_str ("command"));
    }

int get_tty_variable( void )
    {
    return get_tty_something (u_str("variable"));
    }

int get_tty_buffer( void )
    {
    return get_tty_something (u_str ("buffer"));
    }

int get_tty_file( void )
    {
    return get_tty_something (u_str ("file"));
    }

#define EXTEND_SIZE 30

static unsigned char **build_table (unsigned char *string)
    {
    unsigned char * *v;
    int siz;
    int tot;
    unsigned char *s;
    unsigned char *ss;

    while( *string != '\0' && isspace (*string))
	string++;
    if( *string == '\0' )
	{
	if( (v = (unsigned char **)malloc (sizeof( unsigned char *),malloc_type_star_star)) != 0 )
		{
		v[0] = NULL;
		return v;
		}
	error( no_mem_str );
	return 0;
	}
    if( (s = ss = savestr (string)) == 0 )
	{
	error (no_mem_str);
	return 0;
	}
    if( (v = (unsigned char **)malloc (EXTEND_SIZE * sizeof( unsigned char * ), malloc_type_star_star)) == 0 )
	{
	error (no_mem_str);
	return 0;
	}
    siz = 1;
    tot = EXTEND_SIZE;

    v[0] = ss;
    while( *ss != '\0')
	{
	if( isspace (*ss) )
		{
		*ss++ = '\0';

		while( *ss != '\0' && isspace (*ss))
			ss++;
		if( *ss != '\0' )
			{
			v[siz] = ss;
			siz++;
			if( siz >= tot )
				if( (v = (unsigned char **)realloc (v, (tot = tot + EXTEND_SIZE) * sizeof( unsigned char * ), malloc_type_star_star)) == 0 )
					{
					error (no_mem_str);
					free (s);
					return 0;
					}
			}
		}
	else
		ss++;
	}

    v[siz] = NULL;
    return v;
    }

static void destroy_table (unsigned char * *table)
    {
    if( table[0] != NULL )
	free (table[0]);
    free (table);
    }

int expand_from_string_table( void )
    {
    int i;
    unsigned char prompt[512];
    int exact = 0;
    unsigned char *prompt1 = getstr(u_str(": expand-from-string-table (prompt)"));
    unsigned char *str;
    struct prognode *lcur_exec = cur_exec;
    unsigned char **table;
    unsigned char *answer;

    if( prompt1 != NULL )
	{
	_str_cpy (prompt, prompt1);
	if( (str = getstr (
	    u_str (": expand-from-string-table (prompt) %s (table) "),
	    prompt1)) != NULL )
		{
		if( (table = build_table (str)) == 0 )
			return 0;

		if( arg_state == have_arg )
			exact = arg;
		else
			if( ! interactive && cur_exec->p_nargs > 2 )
				exact = getnum( u_str(": expand-string-from-table (flags) "));

		cur_exec = 0;
		release_expr (ml_value);

		if( exact )
			{
			i = getword (table, prompt);
			answer = i < 0 ? 0 : table[i];
			}
		else
			answer = getescword (table, prompt);
		cur_exec = lcur_exec;
		if( answer != 0 )
		    {
		    ml_value->exp_int = _str_len (answer);
		    ml_value->exp_v.v_string = savestr (answer);
		    ml_value->exp_type = ISSTRING;
		    ml_value->exp_release = 1;
		    }
		else
		    ml_value->exp_type = ISVOID;

		destroy_table (table);
		}
	}
    return 0;
    }

int get_tty_character( void )
    {
    struct prognode *lcur_exec = cur_exec;

    cur_exec = 0;
    ml_value->exp_type = ISINTEGER;
    ml_value->exp_int = term_is_terminal != 0 || mem_ptr != 0 ? get_char () : -1;
    cur_exec = lcur_exec;

    return 0;
    }
int concat_command( void )
    {
    string_arg (1);
    if( !err && cur_exec->p_nargs > 1 )
     	{
	unsigned char *p = malloc_ustr(100);
	int space = 100;
	int size = 0;
	int i = 1;

	do
	    {
	    if( size + ml_value->exp_int >= space )
		p = realloc_ustr(p, space = space + ml_value->exp_int + 100);

	    memcpy( p + size, ml_value->exp_v.v_string,  ml_value->exp_int );
	    size = size + ml_value->exp_int;
	    i++;
	    }
	while( i <= cur_exec->p_nargs && string_arg (i) != 0 );

	release_expr (ml_value);
	ml_value->exp_type = ISSTRING;
	ml_value->exp_int = size;
	ml_value->exp_release = 1;
	ml_value->exp_v.v_string = p;
	p[size] = 0;
    	}
    return 0;
    }


int region_to_string( void )
	{
	unsigned char *p;
	struct emacs_buffer *old_bf;
	int left;
	int right;

	old_bf = bf_cur;

	if( cur_exec != 0
	&& cur_exec->p_nargs > 0 )
		{
		if( check_args( 1, 2 ) )
			return 0;

		if( ! eval_arg( 1 ) )
			return 0;

		switch( ml_value->exp_type )
		{
		case ISINTEGER:
			left = ml_value->exp_int;
			break;
		case ISMARKER:
			left = to_mark( ml_value->exp_v.v_marker );
			break;
		default:
			{
			error( marker_or_string );
			return 0;
			}
		}

		if( cur_exec->p_nargs != 2 )
			right = dot;
		else
			{
			if( ! eval_arg( 2 ) )
				return 0;

			switch( ml_value->exp_type )
			{
			case ISINTEGER:
				right = ml_value->exp_int;
				break;
			case ISMARKER:
				{
				struct emacs_buffer *left_bf;

				left_bf = bf_cur;
				right = to_mark( ml_value->exp_v.v_marker );
				if( left_bf != bf_cur )
					{
					error( u_str("2nd marker must refer to buffer %s"),
						left_bf->b_buf_name );
					return 0;
					}
				}
				break;
			default:
				{
				error( marker_or_string );
				return 0;
				}
			}
			}
		if( left > right )
			{
			int swap;

			swap = right;
			right = left;
			left = swap;
			}
		}
	else
		{
		if( bf_cur->b_mark == 0 )
			{
			error (u_str("Mark not set"));
			return 0;
			}
		left = to_mark (bf_cur->b_mark);
		if( left <= dot )
			right = dot;
		else
			{
			right = left;
			left = dot;
			}
		}
	if( left <= bf_s1
	&& right > bf_s1 )
		gap_to (left);

	ml_value->exp_v.v_string = p =
		malloc_ustr ((ml_value->exp_int = right - left) + 1);

	memcpy
	(
	ml_value->exp_v.v_string,
	(void *)(left > bf_s1 ? &bf_p2[left] : &bf_p1[left]),
	ml_value->exp_int
	);
	p [ml_value->exp_int] = 0;
	ml_value->exp_release = 1;
	ml_value->exp_type = ISSTRING;

	if( old_bf != bf_cur )
		set_bfp( old_bf );

	return 0;
	}
int length_command( void )
    {
    if( string_arg (1) )
 	{
	release_expr (ml_value);
	ml_value->exp_type= ISINTEGER;
    	}
    return 0;
    }
int goto_character( void )
    {
    int n = getnum(u_str(": goto-character "));

    if( ! err )
     	{
	if( n < 1 )
	    n = 1;

	if( n > num_characters )
	    n = num_characters + 1;

	set_dot (n);
    	}
    return 0;
    }

int no_value_command( void )
    {
    return 0;
    }

int putenv_command( void )
	{
	unsigned char *vname = getnbstr(u_str(": putenv "));
	unsigned char *value;

	if( vname == NULL || vname[0] == '\0' )
		return 0;

	vname = savestr( vname );
	value = getstr( u_str(": putenv %s "), vname );
	if( value != NULL )
		put_config_env( vname, value );
	free( vname );
	return 0;
	}

extern int is_motif;
int getenv_command( void )
    {
    unsigned char *vname = getnbstr(u_str(": getenv "));
    unsigned char *value;

    if( vname == 0 )
	return 0;

	/* check_ on the special cases first, and do the right thing */
	if( _str_cmp (vname, u_str ("USER")) == 0 )
	    {
	    if( users_name == 0 )
		if( (users_name = (unsigned char *)cuserid (0)) == NULL )
		    users_name = unknown_str;
	    value = users_name;
	    }

#ifdef vms
	else if( _str_cmp (vname, u_str ("HOME")) == 0 )
		vname = u_str ("SYS$LOGIN");

	else if( _str_cmp (vname, u_str ("PATH")) == 0 )
	    value =  parent_path[0] != 0 ? parent_path : current_directory;

	else if( _str_cmp (vname, u_str ("TERM")) == 0 )
	    {
	    if (is_motif)
		value = u_str ("MOTIF");
	    else
		{
		int i;
		int term;

		term = tt->t_cur_attributes.b_type;
       	        i = 0;
		while( terminal_idents[i] != 0)
		    if( terminal_idents[i] == term )
		        break;
		    else
		        i++;

		if( terminal_idents[i] == 0 )
			{
			static unsigned char term_name_buf[32];

			sprintfl( term_name_buf, sizeof( term_name_buf ) - 1,
				u_str("term-id-%d"), term );
			value = term_name_buf;
			}
		else
			value = terminal_names[i];
	    	}
	    }
	else
	    {
            get_log (vname, lognam);
	    value = lognam;
	    }
#else
	else
		{
#ifdef __ultrix
		extern int is_motif;

		if (is_motif && _str_cmp (vname, u_str ("TERM")) == 0)
		    value = u_str ("MOTIF");
		else
#endif
		    value = get_config_env(vname);
		}
#endif

    if( value == NULL || value[0] == 0 )
     	{
	error (u_str ("There is no environment variable named %s"), vname);
	return 0;
    	}

    ml_value->exp_type = ISSTRING;
    ml_value->exp_release = 1;
    ml_value->exp_int = _str_len (value);
    ml_value->exp_v.v_string = savestr( value );

    return 0;
    }

void init_func( void )
    {
    return;
    }
