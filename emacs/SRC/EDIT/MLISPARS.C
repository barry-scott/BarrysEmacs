/*
 *	Copyright( c ) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery
 */

/* MLisp parsing routines */

/* Unix Emacs MLisp( Mock/Minimal Lisp ).
 * This atrocity is used for writing extensions to Emacs.
 *
 * MLisp is Lisp without the CONS function, and all that that implies.
 *( including the fact that MLisp programs are not MLisp data structures) !
 */

#include <emacs.h>

/*forward*/ static int next_char(int (*read_character)(void));
/*forward*/ static struct prognode *parse_node(int (*read_character)(void));
/*forward*/ static void lisp_comment(int (*read_character)(void));
/*forward*/ static unsigned char *parse_name(int (*read_character)(void));
/*forward*/ void lisp_free( struct prognode *p );
/*forward*/ static struct prognode *paren_node(int (*read_character)(void));
/*forward*/ static struct prognode *number_node(int (*read_character)(void));
/*forward*/ static struct prognode *string_node(int (*read_character)(void));
/*forward*/ static int getbufc( void );
/*forward*/ int execute_mlisp_buffer( void );
/*forward*/ static int get_line_char( void );
/*forward*/ int execute_mlisp_line( void );
/*forward*/ static int execute_mlisp_something(int (*read_character)(void));
/*forward*/ static int read_mlisp_file_character( void );
/*forward*/ static unsigned char *save_lib_text_pointer(int n, unsigned char *p);
/*forward*/ static int read_mlisp_lib_character( void );
/*forward*/ int execute_mlisp_file( unsigned char *fn, int missingOK );
/*forward*/ int execute_mlisp_file_command( void );
/*forward*/ struct variablename *lookup_variable( unsigned char *name );
/*forward*/ static int lookup_variable_inner( unsigned char *name );
/*forward*/ void define_variable( unsigned char *name, struct variablename *desc );
/*forward*/ static struct prognode *name_node(int (*read_character)(void));

extern struct bound_name bound_star_defun_node;

#define MAXNODES 512	/* Max number of Mlisp nodes for one node */

/* name of the current file being parsed */
static unsigned char *cur_mlisp_file = u_str("-interactive-");

static int peekc;	/* one character push-back */

static int next_char(int (*read_character)(void))
	{
	int temp;

	if( peekc != 0 )
		{
		temp = peekc;
		peekc = 0;
		return temp;
		}
	else
		return read_character();
	}

/*
 * ParseNode parses an MLisp program node and returns a pointer to it.
 * 0 is returned if the parse was unsuccessful. Getc is a function to be
 * called to fetch the next character -- it should return -1 on errors.
 */
static struct prognode *parse_node(int (*read_character)(void))
	{
	int c;
	/*
	 *	Trim all spaces and comments
	 */
	for(;;)
		{
		do
			c = next_char( read_character );
		while( c >= 0 && isspace( c ) );

		if( c != ';' ) break;
		lisp_comment( read_character );
		}

	if( c == '(' )
		return paren_node( read_character );
	if( c < 0 || c == ')' )
		{
		peekc = c;
		return 0;
		}
	if( c == '"' )
		return string_node( read_character );
	peekc = c;
	if(c == '\'' || c == '-' || isdigit( c ) )
		return number_node( read_character );
	else
		return name_node( read_character );
	}

/* LispComment handles lisp style comments */
static void lisp_comment(int (*read_character)(void))
	{
	int c;
	do
		c = next_char( read_character );
	while( c > 0 && c != '\n' );
	}

/* ParseName parses a name from the MLisp input stream */
static unsigned char *parse_name(int (*read_character)(void))
	{
	static unsigned char buf[200];

	unsigned char *p;
	int c;

	do
		c = next_char( read_character );
	while( isspace( c ) );
	p = &buf[ 0 ];
	while( c > 0
	&& ! isspace( c )
	&& c != '(' && c != ')' && c != ';' )
		{
		*p++ =  (unsigned char)c;
		c = next_char( read_character );
		}
	peekc = c;
	*p++ =  0;
	if( buf[ 0 ] != 0 )
		return &buf[ 0 ];
	else
		return 0;
	}

/* release the tree p */
void lisp_free( struct prognode *p )
	{
	int n;

	if( p == 0 || p->p_active )
		/* Punt freeing actively executing functions */
		return;
	for( n=0; n<=p->p_nargs - 1; n += 1 )
		lisp_free( p->p_args[n].pa_node );
	free( p );
	}

/* parse a parenthesised node WITH SPECIAL CODE FOR *defun( DLK )
 * In order to get a lisp-style defun syntax, we have
 * to hack the reader to recognize a special case
 * -( defun function-name(var1 var2 ..[ varN ] ) ...
 * and turn it into
 * -( *defun (function-name(novalue var1 ..[ varN ] ) */

static struct prognode *paren_node(int (*read_character)(void))
	{
	int c;
	int nargs = 0;
	struct prognode *args[MAXNODES];	/* alas, yet another hard-wired limitation! */
	unsigned char *name;
	struct prognode *p;
	struct bound_name *who;
	int ind;
	struct prognode *converted_defun;

	name = parse_name( read_character );
	converted_defun = 0;

	if( name == 0 )
		return 0;
	ind = find_mac( name );
	if( ind < 0 )
		{
		def_mac( name, 0, 1, 0, 0 );
		ind = find_mac( name );
		if( ind < 0 )
			{
			error( u_str( "definition bogosity, defining %s" ), name );
			return 0;
			}
		}
	who = mac_bodies[ ind ];
	/*
	 *	If we are doing a 'defun', check_ for the special case mentioned
	 *	at the introduction to this function( defun fun(var1 var2 .[ varN ])... )
	 * If we are doing just such a defun, convert it to the *defun form.
	 */
	if( who->b_bound.b_proc == define_function )
		{
		/*
		 *	Skip blanks and any comments
		 */
		for(;;)
			{
			do
				c = next_char( read_character );
			while( c >= 0 && isspace( c ) );

			if( c != ';' )
				break;
			lisp_comment( read_character );
			}
		/*
		 *	If the next character is any of ( ; ) double quote single quote - or digit,
		 *	we just push back the character and proceed normally
		 */
		if( c == '('
		|| c == ';'
		|| c < 0
		|| c == ')'
		|| c == '"'
		|| c == '\''
		|| c == '-'
		|| isdigit( c ) )
			peekc = c;
		else
			{
			/*
			 *	This is it, the *defun you have all been waiting for!!
			 */
			peekc = c;	/* Push back the 1st character
			 *
			 *	Construct a ProgNode for the *defun and save it
			 */
			converted_defun = malloc_struct( prognode );
			converted_defun->p_proc= &bound_star_defun_node;
			converted_defun->p_nargs= 1;
			/*
			 *	Setup who with the function name( Same code as above )
			 */
			name = parse_name( read_character );
			if( name == 0 )
				{
				free( converted_defun );
				return 0;
				}
			ind = find_mac( name );
			if( ind < 0 )
				{
				def_mac( name, 0, 1, 0, 0 );
				ind = find_mac( name );
				if( ind < 0 )
					{
					free( converted_defun );
					error( u_str( "definition bogosity, defining %s" ), name );
					return 0;
					}
				}
			who = mac_bodies[ ind ];
			/*
			 *	Now scan for the open paren of the arg list
			 *	THERE HAD BETTER BE ONE, OR THIS IS AN error
			 */
			do
				c = next_char( read_character );
			while( c >= 0 && isspace( c ) );
			if( c != '(' )
				{
				free( converted_defun );
				error( u_str( "Syntax error in new-style defun -- incorrect arg list" ) );
				return 0;
				}
			/*
			 *	Gather up the variables as an argument list
			 */
			while( ! err
			&& (args[ nargs ] = parse_node( read_character )) != 0 )
				{
				nargs++;
				if( nargs >= MAXNODES )
					{
					int i;

					error( u_str( "MLisp expression is too complex" ) );
					for( i=0; i<=nargs - 1; i += 1 )
						lisp_free( args[ i ] );
					return 0;
					}
				}
			if( peekc == ')' )
				peekc = 0;
			p = (struct prognode *)malloc( sizeof( struct prognode ) + (nargs - 1) * sizeof( void* ), malloc_type_struct_prognode );
			p->p_proc = mac_bodies[ find_mac( u_str("novalue") ) ];
			p->p_nargs = nargs;
			memcpy( p->p_args, args, nargs*sizeof( void* ) );

			/*
			 *	Save the arg list prognode for later
			 */
			converted_defun->p_args[0].pa_node = p;
			nargs = 1;
			/* Rest of the evaluation will be from arg1 */
			}
		}
	while( ! err
	&& (args[ nargs ] = parse_node( read_character )) != 0 )
		{
		if( nargs >= MAXNODES )
			{
			int i;

			error( u_str( "MLisp expression is too complex in %s" ), cur_mlisp_file );
			for( i=0; i<=nargs - 1; i += 1 )
				lisp_free( args[ i ] );
			return 0;
			}
		nargs++;
		}
	if( peekc == ')' )
		peekc = 0;
	if( peekc < 0 )
		error( u_str( "Unexpected EOF possible parens mismatched in %s" ), cur_mlisp_file );
	p = (struct prognode *)malloc( sizeof( struct prognode ) + (nargs - 1) * sizeof( void* ), malloc_type_struct_prognode );
	p->p_proc = who;
	p->p_nargs = nargs;
	memcpy( p->p_args, args, nargs*sizeof( void* ) );

	/*
	 *	If this is a converted Defun, fix up the pointers and return it
	 */
	if( converted_defun != 0 )
		{
		p->p_args[0] = converted_defun->p_args[0];
		converted_defun->p_args[0].pa_node = p;
		return converted_defun;
		}
	/*
	 *	Otherwise return p
	 */
	return p;
	}

unsigned char msg_imp_char[] = "'%s' is an improper character constant.";
extern struct bound_name bound_number_node;

static struct prognode *number_node(int (*read_character)(void))
	{
	struct prognode *p;
	int n;
	int c;

	p = malloc_struct( prognode );
	c = next_char( read_character );
	n = 0;

	{
	if( c == '\'' )
		{
		unsigned char buf[30];
		buf[ 0 ] = (unsigned char)next_char( read_character );
		do
			{
			c = next_char( read_character );
			n++;
			buf[ n ] = (unsigned char)c;
			}
		while( n < 29
		&& (c != '\'' || (n == 1 && buf[0] == '\\')) );

		buf[ n ] = 0;
		if( n > 10 )
			{
			error( msg_imp_char, buf );
			goto number_block;
			}
		if( n == 1 )
			n = buf[0];
		else
			{
			if( n == 2
			&& buf[0] == '^' )
				{
				if( (n = buf[1]) == '?' )
					n = '\177';
				else
					n &= 037;
				}
			else
				{
				if( n == 2
				&& buf[0] == '\\'
				&& ! isdigit( buf[1] ) )
					switch( buf[1] )
					{
					case 'n': n = 10; break;	/* LF */
					case 'b': n =  8; break;	/* BS */
					case 't': n =  9; break;	/* TAB */
					case 'r': n = 13; break;	/* CR */
					case 'e': n = 27; break;	/* ESC */
					default:  n = buf[1];
					}
				else
					{
					if( n > 1
					&& buf[0] == '\\' )
						{
						unsigned char *p;
						p = &buf[1];
						n = 0;
						while( isdigit( c = *p++ ) )
							n = n * 8 + c - '0';
						if( c != 0 )
							{
							error( msg_imp_char, buf );
							goto number_block;
							}
						}
					else
						error( msg_imp_char, buf );
					}
				}
			}
		}
	else
		{
		int neg;
		int base;
		neg = 0;
		base = 10;
		if( c == '-' )
			{
			neg++;
			c = next_char( read_character );
			}
		if( c == '0' )
			base = 8;
		while( isdigit( c ) )
			{
			n = n * base + c - '0';
			c = next_char( read_character );
			}
		if( neg )
			n = -n;
		peekc = c;
		}
number_block:
	;
	}

	p->p_proc = &bound_number_node;
	p->p_nargs = 0;
	p->p_args[0].pa_int = n;

	return p;
	}

extern struct bound_name bound_string_node;
static struct prognode *string_node(int (*read_character)(void))
	{
	unsigned char *p;
	int c;
	int i;
	unsigned char buf[300];
	struct prognode *r;

	p = buf;
	while( (c = next_char( read_character )) > 0 )
		{
		if( c == '\\' )
			switch( c = next_char( read_character ) )
			{
			case 'n': c = 10; break;	/* LF */
			case 'b': c =  8; break;	/* BS */
			case 't': c =  9; break;	/* TAB */
			case 'r': c = 13; break;	/* CR */
			case 'e': c = 27; break;	/* ESC */
			case '^':
				{
				c = next_char( read_character );
				if( c == '?' )
					c = 127	/* DEL */;
				else
					c = c & 037;
				break;
				}
			case '(':
				{
				unsigned char *kp;
				unsigned char key_name[32];
				kp = key_name;
				c = next_char( read_character );
				while( c != ')' )
					{
					if( c == 0 || c == '"'
					|| kp >= &key_name[sizeof( key_name ) - 1] )
						{
						error( u_str("Closing \")\" missing in keyname escape sequence") );
						return 0;
						}
					*kp++ =  (unsigned char)c;
					c = next_char( read_character );
					}
				*kp++ =  0;
				kp = lookup_key_name( key_name );
				if( kp == 0 )
					{
					error( u_str( "Unknown keyname %s in string" ), key_name );
					return 0;
					}
				/* move all but one of the chars */
				while( kp[1] )
					if( p < &buf[ sizeof( buf ) ] )
						*p++ =  *kp++;
				c = *kp; /* now get the last one */
				break;
				}
			default:
				{
				if( '0' <= c && c <= '7' )
					{
					int nc; int cnt;
					nc = 0;
					cnt = 3;
					do
						nc = nc * 8 + c - '0';
					while( (cnt = cnt - 1) > 0
					&& '0' <= (c = next_char( read_character ))
					&& c <= '7' );

					if( cnt > 0 )
						peekc = c;
					c = nc;
					}
				}
			}
		else
			if( c == '"'
			&& (c = next_char( read_character )) != '"' )
				{
				peekc = c;
				break;
				}
		if( p >= &buf[ sizeof( buf ) ] )
			{
			error( u_str( "Unterminated string constant" ) );
			return 0;
			}
		*p++ =  (unsigned char)c;
		}
	*p++ =  0;
	i = p - buf;
	r = (struct prognode *)malloc( sizeof( struct prognode ) + i, malloc_type_struct_prognode );
	r->p_proc = &bound_string_node;
	r->p_nargs = 0;
	r->p_args[0].pa_int = i - 1;
	memcpy(&r->p_args[1].pa_char, buf, i );

	return r;
	}


static int bufpos;
static unsigned char *getbuf_prefix;
static unsigned char *getbuf_suffix;

static int getbufc( void )
	{
	unsigned char c;
	if( bufpos == 0 )
		{
		if( (c = *getbuf_prefix++) != 0 )
			return c;
		else
			bufpos = first_character;
		}
	if( bufpos > num_characters )
		if(( c = getbuf_suffix[0] ) != 0 )
			{
			getbuf_suffix++;
			return (int)c;
			}
		else
			return -1;
	c = char_at( bufpos );
	bufpos++;
	return (int)c;
	}

int execute_mlisp_buffer( void )
	{
	int rv;
	bufpos = 0;
	getbuf_prefix = u_str( "(progn " );
	getbuf_suffix = u_str( "\n)" );
	rv = execute_mlisp_something( getbufc );
	if( err )
		set_dot( bufpos - 1 );
	return rv;
	}


static unsigned char ml_line[MLLINE_SIZE];
static int get_line_char( void )
	{
	int c;

	if( (c = ml_line[ bufpos ]) != 0 )
		{
		bufpos++;
		return c;
		}
	else
		return -1;
	}

struct prognode *parse_mlisp_line( unsigned char *s )
	{
	int line_size;

	if( s == 0 ) return 0;

	line_size = _str_len( s );
	if( line_size > (MLLINE_SIZE - 1) )
		{
		error( u_str( "string too long to execute" ) );
		return 0;
		}
	bufpos = 0;
	/* copy string an asciz null */
	memcpy( ml_line, s, line_size+1 );
	peekc = 0;

	return parse_node( get_line_char );
	}

int execute_mlisp_string( unsigned char *mlisp_command )
	{
	struct prognode *p;
	int rv;

	p = parse_mlisp_line( mlisp_command );
	if( p == 0 ) return 0;

	rv = exec_prog( p );
	lisp_free( p );

	return rv;
	}

int execute_mlisp_line( void )
	{
	int rv = execute_mlisp_string( getstr( u_str( ": execute-mlisp-line " ) ) );

	if( !err && interactive )
		{
		if( ml_value->exp_type == ISINTEGER )
			message( u_str( "%s => %d" ),
				ml_line, ml_value->exp_int );
		else
			if( ml_value->exp_type == ISSTRING )
				message( u_str( "%s => \"%s\"" ),
					ml_line, ml_value->exp_v.v_string );
		release_expr( ml_value );
		ml_value->exp_type= ISVOID;
		}

	return rv;
	}
	
extern struct bound_name bound_variable_node;

static int execute_mlisp_something(int (*read_character)(void))
	{
	struct prognode *p;
	int rv;

	peekc = 0;
	if( (p = parse_node( read_character )) == 0 )
		return 0;
	rv = exec_prog( p );
	lisp_free( p );
	return rv;
	}

static FILE *mlisp_in;
static int file_size;
static unsigned char *f_buffer;
static unsigned char *file_pointer;

static int read_mlisp_file_character( void )
	{
	if( mlisp_in == 0 )
		return -1;
	if( file_size == 0 )
		{
		file_size = fio_get( mlisp_in, f_buffer, BUFSIZ );
		file_pointer = f_buffer;
		}
	if( file_size > 0 )
		{
		file_size--;
		return *file_pointer++;
		}
	return -1;
	}


static int lib_text_size;		/* Number of characters left */
static unsigned char *lib_text;		/* Start of buffer holding chars */
static unsigned char *lib_text_pointer;	/* pointer to next to char to return */

#if defined( DB )
static unsigned char * save_lib_text_pointer(int n, unsigned char *p)
	{
	lib_text_size = n;
	lib_text = p;
	lib_text_pointer = p;
	return NULL;
	}
#endif

static int read_mlisp_lib_character( void )
	{
	if( lib_text == 0 )
		return -1;
	lib_text_size--;
	if( lib_text_size >= 0 )
		return *lib_text_pointer++;
	free( lib_text );
	lib_text = 0;
	return -1;
	}

/*
 *	Enhance execute-mlisp-file and load to scan the text libs
 *	in the search list 'procedures'.
 *	Barry A. Scott	30-Oct-1982
 */
int execute_mlisp_file( unsigned char *fn, int missingOK )
	{
	int c = -1;
	unsigned char fnb[BUFSIZ];
	FILE *old_mlisp_in;
	int old_file_size;
	unsigned char *old_f_buffer;
	unsigned char *old_file_pointer;
	unsigned char *old_lib_text;
	unsigned char *old_lib_text_pointer;
	unsigned char *old_cur_mlisp_file = cur_mlisp_file;
	int old_lib_text_size;
	int rv;
	int lib;

#if DBG_EXEC
        if( dbg_flags&DBG_EXEC )
                _dbg_msg( u_str("execute-mlisp-file %s\r\n"), fn);
#endif

	old_mlisp_in = mlisp_in;
	old_file_size = file_size;
	old_f_buffer = f_buffer;
	old_file_pointer = file_pointer;
	old_lib_text = lib_text;
	old_lib_text_pointer = lib_text_pointer;
	old_lib_text_size = lib_text_size;
	rv = 0;
	lib = 0;

	if( fn == 0 )
		return 0;
	mlisp_in = 0;
	file_size = 0;
	f_buffer = 0;
	file_pointer = 0;
	lib_text = 0;
	lib_text_pointer = 0;
	lib_text_size = 0;

	cur_mlisp_file = fn;

	{
	peekc = 0;
	mlisp_in = fopenp( EMACS_PATH, fn, fnb, FIO_READ, u_str( ".ml" ));
	if( mlisp_in == 0 )
		{
#if defined( DB )
		int i;
		struct dbsearch *dbs;

		/*
		 *	If the search list exists...
		 */
		if( (dbs = find_sl( u_str( "MLisp-library" ) )) != 0 )
			{
			int fnl;
			unsigned char *cp;
			int contlen;
			unsigned char *cont;

#ifdef vms
			cp = fn;
			fnl = _str_len( cp );
			/* lose the trailing '.ml' if it is present */
			if( cp[ fnl - 3 ] == '.'
			&& cp[ fnl - 2 ] == 'm'
			&& cp[ fnl - 1 ] == 'l' )
				fnl -= 3;
#else
			cp = fnb;
			_str_cpy( cp, fn );
			if( _str_chr( cp, '.' ) == NULL )
				_str_cat( cp, ".ml" );
			fnl = _str_len( cp );
#endif

			/* look for the key in the db */
			for( i=0; i<=dbs->dbs_size - 1; i += 1 )
				{
				get_db
				(
				cp, fnl,
				&cont, &contlen,
				save_lib_text_pointer,
				dbs->dbs_elements[i]
				);
				if( lib_text != NULL )
					{
					lib = 1;
					peekc = read_mlisp_lib_character();
					break;
					}
				}
			}
#endif
		if( lib == 0
		&& mlisp_in == 0
		&& ! missingOK )
			{
			error( u_str( "Cannot read %s" ), fn );
			goto execute_block;
			}
		}
	else
		{
		f_buffer = fnb;
		peekc = read_mlisp_file_character();
		}

	if( mlisp_in != 0
	|| lib != 0 )
		while( peekc >= 0
		&& rv == 0
		&& ! err )
			{
			struct prognode *p;
			if( lib != 0 )
				p = parse_node( read_mlisp_lib_character );
			else
				p = parse_node( read_mlisp_file_character );
			if( p == 0 )
				break;
			rv = exec_prog( p );
			lisp_free( p );
			if( lib != 0 )
				do
					c = read_mlisp_lib_character();
				while( c >= 0 && isspace( c ) );
			else
				if( mlisp_in != 0 )
					do
						c = read_mlisp_file_character();
					while( c >= 0 && isspace( c ) );
			peekc = c;
			}
execute_block:
	;
	}	/* of execute_block */

	if( mlisp_in != 0 )
		fio_close( mlisp_in );
	if( lib_text != 0 )
		{
		if( lib_text )
			free( lib_text );
		lib_text = 0;
		}
	mlisp_in = old_mlisp_in;
	file_size = old_file_size;
	f_buffer = old_f_buffer;
	file_pointer = old_file_pointer;
	lib_text = old_lib_text;
	lib_text_pointer = old_lib_text_pointer;
	lib_text_size = old_lib_text_size;
	cur_mlisp_file = old_cur_mlisp_file;

	return rv;
	}

int execute_mlisp_file_command( void )
	{
	unsigned char *s;
	s = getstr( u_str( ": execute-mlisp-file " ) );
	if( s == 0 )
		return 0;
	return execute_mlisp_file( s, 0 );
	}

struct variablename *lookup_variable( unsigned char *name )
	{
	int index = lookup_variable_inner( name );
	if( index >= 0 )
		return var_desc[index];
	else
		return NULL;
	}

static int lookup_variable_inner( unsigned char *name )
	{
	int hi; int lo; int mid;
	unsigned char *s1;
	unsigned char *s2;

	lo = 0;
	hi = n_vars - 1;

	/* test only required for the first call */
	if( hi < 0 )
		return -1;

	while( lo <= hi )
		{
		mid = (lo + hi) >> 1;
		s1 = name;
		s2 = var_names[mid];
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

/*
 * define a varible name given the string name
 * and a pointer to the descriptor record
 */
void define_variable( unsigned char *name, struct variablename *desc )
	{
	int index;

	if( n_vars > var_t_size - 1 )
		invoke_debug();

	if( n_vars == var_t_size - 1 )	/* enlarge the string table */
		{
		int new_size = var_t_size + 50;

		var_names = (unsigned char **)realloc( var_names, new_size * sizeof( void* ), malloc_type_star_star );
		var_desc = (struct variablename **)realloc( var_desc, new_size * sizeof( void* ), malloc_type_star_star );

		var_t_size = new_size;
		}

	index = lookup_variable_inner( name );
	if( index < 0 )
		{
		struct variablename **vp;
		unsigned char **np;
		struct variablename **ve;

		np = &var_names[n_vars];
		vp = &var_desc[n_vars];
		n_vars++;
		ve = &var_desc[-index - 1];
		np[1] = NULL;	/* sneaky */
		while( vp > ve )
			{
			np[0] = np[-1];
			vp[0] = vp[-1];
			np--; vp--;
			}
		np[0] = name;
		vp[0] = desc;
		}
	}

/*
 * parse a name token in an MLisp program
 */
static struct prognode *name_node(int (*read_character)(void))
	{
	struct prognode *p;
	unsigned char *name;
	struct variablename *v;

	p = malloc_struct( prognode );
	name = parse_name( read_character );
	if( name == 0 )
		return 0;

	v = lookup_variable( name );
	if( v == 0 )
		{
		v = malloc_struct( variablename );
		v->v_name = savestr( name );
		define_variable( v->v_name, v );
		v->v_binding = 0;
		}
	p->p_nargs= 0;
	p->p_args[0].pa_name = v;
	p->p_proc = &bound_variable_node;
	return p;
	}

int check_is_proc
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v),
	struct bound_name **proc
	)
	{
	int index;

	if( value == 0 )
		return 0;

	if( _str_len( value ) <= 0 )
		proc[0] = 0;
	else
		{
		index = find_mac( value );
		if( index < 0 )
			{
			error( u_str("%s has not been define yet"), value );
			return 0;
			}
		proc[0] = mac_bodies[index];
		}
	return 1;
	}

