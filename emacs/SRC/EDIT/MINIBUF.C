/*module minibuf
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode (nonexternal = long_relative, external = general)
 *	) =
 *begin
 *
 *  	Copyright(c) 1982, 1983, 1984, 1985, 1986, 1987
 *		Barry A. Scott and Nick Emery
 *
 * Functions to manipulate the minibuffer */

#include <emacs.h>

unsigned char *br_getstr( int breaksp, unsigned char *prefix, unsigned char *fmt, va_list *argp );

unsigned char *on_str = u_str("on");
unsigned char *off_str = u_str("off");
unsigned char *true_str = u_str("true");
unsigned char *false_str = u_str("false");
unsigned char *too_few_args_str = u_str("Too few arguments given to %s");

extern int gui_error;

static unsigned char buf[BUFFERSIZE];


/*  This is the same as sprintf(buf, ... ) except that it guards against
 *   buffer overflow
 */
unsigned char *sprintfl
	(
	unsigned char *buff,
	int len,
	unsigned char *fmt,
	...
	)
	{
	va_list argp;
	int i;

	va_start( argp, fmt );

	i = do_print (fmt, &argp, buff, len);
	buff[i] = 0;

	return &buff[0];
	}


/* dump an error message; called like printf */
void error (unsigned char *fmt, ...)
	{
	va_list argp;
	int i;
	int error_handled = 0;

	next_local_keymap = 0;
	next_global_keymap = 0;

	/*
	 * The first error message probably makes the
	 * most sense, so we suppress subsequent ones.
	 */

	if( err && minibuf_body != 0 )
		return;

	va_start( argp, fmt );
	i = do_print( fmt, &argp, error_message_text, sizeof( error_message_text ) );
	error_message_text[i] = 0;

	if( in_error_occurred == 0
	&&  (error_handled = error_handler()) == 0 )
		{
		_str_cpy( buf, error_message_text );

		if( term_is_terminal != 0 )
			{
			minibuf_body = buf;
#if defined(_WINDOWS)
			if( gui_error )
				win_error( error_message_text );
#endif
			}
		else
			fio_put(message_file, buf, _str_len (buf));
		}
	if( error_handled == 0 )
		err = 1;
	}


/* dump an informative message to the minibuf */
void message( unsigned char *fmt, ...)
	{
	va_list argp;
	int i;

	if( ! interactive || (err && minibuf_body != 0) )
		return;

	va_start( argp, fmt );
	i = do_print( fmt, &argp, buf, sizeof( buf ) - 1 );
	buf[i] = 0;

	if( term_is_terminal != 0 )
		minibuf_body = buf;
	else
		fio_put (message_file, buf, _str_len (buf));
	}


/* Converts a string to an integer */
int str_to_int (unsigned char *answer)
	{
	unsigned char *p = answer;
	int n = 0;
	int neg = 0;

	if( p == 0 )
		return 0;

	while( isspace( *p ) )
		p++;

	/* Allow on and true to mean 1 and off and false to mean 0 */
	if( p[0] >= 'A' )
		{
		int len = _str_len(answer);

		if( _str_ncmp (answer, on_str, len) == 0
		|| _str_ncmp (answer, true_str, len) == 0 )
			return 1;
		if( _str_ncmp (answer, off_str, len) == 0
		|| _str_ncmp ( answer, false_str, len) == 0 )
			return 0;
		}
	/* Convert from text to numeric */
	while( p[0] != 0 )
		{
		if( isdigit (p[0]) )
			n = (n * 10) + p[0] - '0';
		else
			if( p[0] == '-' )
				neg = ! neg;
			else
				if( ! isspace (p[0]) && p[0] != '+' )
					{
					error (u_str ("Malformed integer: \"%s\""), answer);
					return 0;
					}
		p = &p[1];
		}
	if( neg != 0 )
	    return -n;
	else
	    return n;
	}


/* read a number from the terminal with prompt string s */
int getnum (unsigned char * fmt, ...)
	{
	va_list argp;

	if( cur_exec != 0 )
		{
		/*
		 * We are being called from an MLisp-called
		 * function. Instead of prompting for a string
		 * we evaluate it from the arg list
		 */
		int larg = arg;
		enum arg_states larg_state = arg_state;
		int n;

		arg_state = no_arg;
		if( (last_arg_used = last_arg_used + 1) >= cur_exec->p_nargs )
			{
			error (too_few_args_str, cur_exec->p_proc->b_proc_name);
			return 0;
			}
		n = numeric_arg(last_arg_used + 1);
		arg = larg;
		arg_state = larg_state;
		return n;
		}


	va_start( argp, fmt );

	return str_to_int( br_getstr( 1, null_str, fmt, &argp ) );
	}


/* Read a string from the terminal with prompt string s */
unsigned char *getstr (unsigned char *fmt,...)
	{
	int old_help = help;
	int old_expand = expand;
	unsigned char *result;
	va_list argp;

	va_start( argp, fmt );

	result = br_getstr (0, null_str, fmt, &argp );

	help = old_help;
	expand = old_expand;

	return result;
	}


/* Read a string from the terminal with prompt string s, whitespace
 * will terminate it. */
unsigned char *getnbstr (unsigned char *fmt, ...)
	{
	int old_help = help;
	int old_expand = expand;
	unsigned char *result;
	va_list argp;

	va_start( argp, fmt );

	result = br_getstr (1, null_str, fmt, &argp );

	help = old_help;
	expand = old_expand;

	return result;
	}


/* Read a string from the terminal with prompt string s.
 *   Whitespace will break iff breaksp is true.
 *   The string 'prefix' behaves as though the user had typed that first.
 */
unsigned char *br_getstr
	(
	int breaksp,
	unsigned char *prefix,
	unsigned char *fmt,
	va_list *argp
	)
	{
	static unsigned char holdit[BUFFERSIZE + 1];
	static unsigned char lbuf[BUFFERSIZE + 1];

	int larg = arg;
	enum arg_states larg_state = arg_state;

	arg_state = no_arg;
	if( cur_exec != 0 )
		{
		/*
		 * We are being called from an MLisp-called
		 * function. Instead of prompting for a string
		 * we evaluate it from the arg list.
		 */
		last_arg_used++;
		if( last_arg_used >= cur_exec->p_nargs )
			{
			error(too_few_args_str, cur_exec->p_proc->b_proc_name);
			return 0;
			}
		if( string_arg (last_arg_used + 1) == 0
		|| ml_value->exp_type != ISSTRING )
			{
			error
			(
			u_str("%s expected %s to return a value." ),
			cur_exec->p_proc->b_proc_name,
			cur_exec->p_args[last_arg_used].pa_node->p_proc->b_proc_name
			);
			return 0;
			}
		arg = larg;
		arg_state = larg_state;

		if( err )
			return 0;

		if( ml_value->exp_v.v_string[ml_value->exp_int] != 0 )
			{
			int len;

   	       	        len =
			    ml_value->exp_int >= sizeof (holdit) ?
			        sizeof (holdit) - 1
			    :
			        ml_value->exp_int;

			memcpy( holdit, ml_value->exp_v.v_string, len );
			holdit[len] = 0;
			return holdit;
			}
		else
			return ml_value->exp_v.v_string;
		}
	else
	if( command_file != 0 )
	        /*
	         * get the string from the command file
	         */
		{
		int read_size;

		read_size = do_print( fmt, argp, lbuf, sizeof( lbuf ) );
		lbuf[read_size] = 0;

		read_size = fio_get_with_prompt(command_file, holdit, sizeof(holdit) - 1, lbuf);
		if( read_size <= 0 )
			{
			error(u_str("No more input available"));
			return 0;
			}

		holdit[read_size] = 0;
		return holdit;
		}
	else
		{
		/*
		 * get the string from the keyboard
		 */

		struct marker *olddot = new_mark();
		unsigned char *result = 0;
		struct keymap *outermap;
		unsigned char *outer_reset = reset_minibuf;
		unsigned char outer[BUFFERSIZE + 1];
		int outer_len;
		int outer_dot;
		int window_num = 0;
		struct window *w;
		struct window *last_w = NULL;
		int i;

		if( interactive )
			{
			int i;

			i = do_print( fmt, argp, lbuf, sizeof( lbuf ) );
			lbuf[i] = 0;

			reset_minibuf = minibuf_body = lbuf;
			}
		set_mark (olddot, bf_cur, dot, 0);

		w = windows;
		i = 0;
		while( w != 0 )
			{
			if( w == wn_cur )
				window_num = i;
			i++;
			last_w = w;
			w = w->w_next;
			}
		set_win (last_w);

		outermap = bf_mode.md_keys;
		bf_mode.md_keys = bf_cur->b_mode.md_keys =
			breaksp != 0 ?
			    minibuf_local_ns_map
			:
			    minibuf_local_map;
		next_global_keymap = next_local_keymap = 0;
		outer_len = bf_s1 + bf_s2;
		if( outer_len > BUFFERSIZE )
			outer_len = BUFFERSIZE;
		outer_dot = dot;
		dot = 1;
		while( dot <= outer_len )
			{
			outer[dot] = char_at(dot);
			dot++;
			}
		erase_bf (bf_cur);
		ins_str(prefix);

		minibuf_depth++;
		recursive_edit();
		minibuf_depth--;

		arg = larg;
		arg_state = larg_state;
		set_bfp(minibuf);
		bf_mode.md_keys = bf_cur->b_mode.md_keys = outermap;
		insert_at (bf_s1 + bf_s2 + 1, 0);
		set_dot (1);
		if( outer_len != 0 )
			ins_cstr( outer + 1, outer_len );
		set_dot (outer_dot);
		result = (( err ) ?  0 : ref_char_at (outer_len + 1));
		if( (reset_minibuf = outer_reset) != 0 )
			minibuf_body = reset_minibuf;
		else
			if( minibuf_body == lbuf )
				minibuf_body = u_str("");
		del_back (bf_s1 + bf_s2 + 1, bf_s1 + bf_s2 - outer_len);

	   	        w = windows;
		while( window_num != 0 && w != 0 )
			{
			window_num--;
			w = w->w_next;
			}
		if( window_num == 0 && w != 0 )
			{
			set_win (w);
			dot = to_mark (olddot);
			}
		else
			window_on (bf_cur);

		dot = to_mark (olddot);
		dest_mark (olddot);

		return result;
		}
	}


/* Get the name of a key. Alas, you can't type a control-G,
 * since that aborts the key name read. Returns-1 if aborted. */
unsigned char *get_key (struct keymap *kmap, unsigned char *fmt, ...)
	{
	unsigned char c;
	unsigned char *p;
	unsigned char *keys;
	int nkeys;
	struct bound_name *b;

	static unsigned char fake_it[100];
	static unsigned char lbuf[BUFFERSIZE];

	if( cur_exec != 0 )
		{
   	        /*
		 * Perform operation in Mock Lisp
		 */
		int larg = arg;
		enum arg_states larg_state = arg_state;

		arg_state = no_arg;
		eval_arg ((last_arg_used = last_arg_used + 1) + 1);
		arg = larg;
		arg_state = larg_state;
		if( err )
			return 0;

		if( ml_value->exp_type != ISSTRING )
			{
			fake_it[0] = (unsigned char)ml_value->exp_int;
			release_expr (ml_value);
			ml_value->exp_int = 1;

			ml_value->exp_type = ISSTRING;
			ml_value->exp_release = 0;
			ml_value->exp_v.v_string = fake_it;
			}
		if( ml_value->exp_int > sizeof (fake_it)
		|| convert_key_string (ml_value->exp_int, ml_value->exp_v.v_string,
		    &ml_value->exp_int, fake_it) == 0 )
			{
			error (u_str ("Key sequence too long"));
			return 0;
			}
		release_expr (ml_value);

		ml_value->exp_type = ISSTRING;
		ml_value->exp_release = 0;
		ml_value->exp_v.v_string = fake_it;

		return ml_value->exp_v.v_string;
		}
	if( interactive )
		{
		va_list argp;
		int i;

		va_start( argp, fmt );
		i = do_print( fmt, &argp, lbuf, sizeof( lbuf ) );
		lbuf[i] = 0;

		p = lbuf + i;
		}
	else
		p = lbuf;

	keys = fake_it;
	nkeys = 0;
	do
		{
		p[0] = 0;
		if( interactive )
		        {
			in_minibuf_body++;
			minibuf_body = lbuf;
			}
		if( (c = (unsigned char)get_char()) == '\007' )
			{
			error (u_str("Aborted."));
			in_minibuf_body = 0;
			return 0;
			}
		if( (nkeys = nkeys + 1) >= sizeof(fake_it) )
			{
			error (u_str("Key sequence too long"));
			return 0;
			}
		*keys++ = c;
		b = get_keymap_binding (kmap, c);
		if( kmap != 0 && b != 0 && b->b_binding == KEYBOUND )
			kmap = b->b_bound.b_keymap;
		else
			kmap = 0;

		if( c == '\033' )
			{
			*p++ = 'E';
			*p++ = 'S';
			*p++ = 'C';
			}
		else
			if( c < ' ' )
				{
				*p++ = '^';
				*p++ = (unsigned char)((c & 037) + 0100);
				}
			else
			        {
				*p++ = c;
				}
		if( kmap != 0 )
			{
			*p++ = '-';
			}
		}
	    while( kmap != 0 );
	p[0] = 0;
	if( interactive )
		minibuf_body = lbuf;
	else
		lbuf[0] = 0;
	in_minibuf_body = 0;
	ml_value->exp_int = nkeys;
	ml_value->exp_type = ISSTRING;
	ml_value->exp_release = 0;
	ml_value->exp_v.v_string = fake_it;

	return fake_it;
	}


int self_insert_and_exit( void )
	{
	self_insert_command();
	return -1;
	}

int error_and_exit( void )
	{
	error (u_str("Aborted."));
	return -1;
	}

int expand_and_exit( void )
	{
	expand = 1;
	return -1;
	}

int help_and_exit( void )
	{
	help = 1;
	return -1;
	}

/*
 *  Returns true if the user answers yes to a question.
 * The user will always be asked
 */
static unsigned char * boolean_words[] =
	{
	u_str("no"), u_str("yes"), u_str(""), 0
	};

int get_yes_or_no(int yes, unsigned char *prompt, ...)
	{
	unsigned char buf[200];
	int answer;
	int i;
	va_list argp;

	va_start( argp, prompt );
	i = do_print( prompt, &argp, buf, sizeof( buf ) );
       	buf[i] = '\0';

	{
	/* zap cur_exec to allow for interative prompts */
	struct prognode *old_cur_exec = cur_exec;
	cur_exec = NULL;

#if defined( _WINDOWS )
	answer = win_yes_or_no( buf, yes );
#else
	answer = getword(boolean_words, u_str( "%s" ), buf);
#endif
	cur_exec = old_cur_exec;
	}

	return answer == 0 || answer == 1 ? answer : yes;
	}
