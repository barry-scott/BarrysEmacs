//
//	Copyright( c ) 1982-1996
//		Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


/*forward*/ int execute_mlisp_buffer( void );
/*forward*/ int execute_mlisp_line( void );
/*forward*/ int execute_mlisp_file( const EmacsString &fn, int missingOK );
/*forward*/ int execute_mlisp_file_command( void );

//
//
//	Input handling base class
//
//
class MLispInputStream
	{
public:
	MLispInputStream();
	virtual ~MLispInputStream();

	// return the next char from the stream
	int operator()();

	// push back one character
	void pushBack( int c ) { peekc = c; }

	// look at the next char without reading
	int peek();

	// returns true if the stream was constructed
	virtual bool isOk();

	// return the current line number
	int currentLine() const { return line_number; }
protected:
	virtual int readCharacter() = 0;
private:
	enum { PEEK_EMPTY = -2 };	
	int peekc;
	int line_number;
	};


MLispInputStream::MLispInputStream()
	: peekc( PEEK_EMPTY )
	, line_number(1)
	{ }

MLispInputStream::~MLispInputStream()
	{ }

int MLispInputStream::peek()
	{
	if( peekc == PEEK_EMPTY )
		pushBack( readCharacter() );

	return peekc;
	}

bool MLispInputStream::isOk()
	{
	return true;
	}

int MLispInputStream::operator()()
	{
	int temp;

	if( peekc != PEEK_EMPTY )
		{
		temp = peekc;
		peekc = PEEK_EMPTY;
		}
	else
		{
		temp = readCharacter();
		if( temp == '\n' )
			line_number++;
		}

	return temp;
	}

//
//
//	mlisp buffer input stream
//
//
class MLispBufferInputStream : public MLispInputStream
	{
public:
	MLispBufferInputStream();
	virtual ~MLispBufferInputStream();

	int position();
private:
	virtual int readCharacter();

	int bufpos;
	};


MLispBufferInputStream::MLispBufferInputStream()
	: bufpos( bf_cur->first_character() )
	{ }

MLispBufferInputStream::~MLispBufferInputStream()
	{ }

int MLispBufferInputStream::position()
	{
	return bufpos;
	}

int MLispBufferInputStream::readCharacter( void )
	{
	unsigned char c;

	if( bufpos > bf_cur->num_characters() )
		// return end of file
		return -1;

	c = bf_cur->char_at( bufpos );
	bufpos++;

	return c;
	}

//
//
//	MLisp line input stream
//
//
class MLispStringInputStream : public MLispInputStream
	{
public:
	MLispStringInputStream( const EmacsString &_string );
	virtual ~MLispStringInputStream();

	virtual bool isOk();
protected:
	// used by library input stream
	void setString( const EmacsString &_string );
private:
	virtual int readCharacter();

	int pos;
	EmacsString string;
	};


MLispStringInputStream::MLispStringInputStream( const EmacsString &_string )
	: pos( 0 )
	, string( _string )
	{ }

void MLispStringInputStream::setString( const EmacsString &_string )
	{
	pos = 0;
	string = _string;
	}

bool MLispStringInputStream::isOk()
	{
	return !string.isNull();
	}

MLispStringInputStream::~MLispStringInputStream()
	{ }

int MLispStringInputStream::readCharacter( void )
	{
	if( pos < string.length() )
		return string[pos++];
	else
		return -1;
	}

//
//
//	MLisp file input stream
//
//
class MLispFileInputStream : public MLispInputStream, public EmacsFile
	{
public:
	MLispFileInputStream( const EmacsString &fn );
	virtual ~MLispFileInputStream();

	virtual bool isOk();
private:
	virtual int readCharacter();

	int pos;
	EmacsString string;

	int file_size;
	enum { BUF_SIZE = 65536 };
	unsigned char f_buffer[BUF_SIZE];
	unsigned char *file_pointer;
	};


MLispFileInputStream::MLispFileInputStream( const EmacsString &fn )
	: EmacsFile()	// VC 4.1 bug - do not put fopen here
	, file_size( 0 )
	, file_pointer( NULL )
	{
	fio_open_using_path( EMACS_PATH, fn, FIO_READ, ".ml" );
	}

bool MLispFileInputStream::isOk()
	{
	return fio_is_open();
	}

MLispFileInputStream::~MLispFileInputStream()
	{ }

int MLispFileInputStream::readCharacter( void )
	{
	if( !fio_is_open() )
		return -1;
	if( file_size == 0 )
		{
		file_size = fio_get( f_buffer, BUF_SIZE );
		file_pointer = f_buffer;
		}
	if( file_size > 0 )
		{
		file_size--;
		return *file_pointer++;
		}
	return -1;
	}


//
//
//	MLisp library input stream
//
//
class MLispLibraryInputStream : public MLispStringInputStream
	{
public:
	MLispLibraryInputStream( const EmacsString &fn );
	virtual ~MLispLibraryInputStream();

	bool isOk();
private:
	bool is_ok;
	};


MLispLibraryInputStream::MLispLibraryInputStream( const EmacsString &fn )
	: MLispStringInputStream( EmacsString::null )
	, is_ok( false )
	{
#if defined( DB )
	DatabaseSearchList *dbs;

	/*
	 *	If the search list exists...
	 */
	if( (dbs = DatabaseSearchList::find( "MLisp-library" )) != NULL )
		{
#ifdef vms
		EmacsString cp( fn );
		int fnl = cp.length();
		/* lose the trailing '.ml' if it is present */
		if( cp( fnl-3, 3 ) == ".ml" )
			cp.remove( fnl-3 );
#else
		EmacsString cp( fn );
		if( cp.first('.') < 0 )
			cp.append( ".ml" );
#endif

		/* look for the key in the db */
		for( int i=0; i<=dbs->dbs_size - 1; i += 1 )
			{
			EmacsString value;
			if( dbs->dbs_elements[i]->get_db( cp, value ) >= 0 )
				{
				setString( value );
				is_ok = true;
				break;
				}
			}
		}
#endif
	}

bool MLispLibraryInputStream::isOk()
	{
	return is_ok;
	}

MLispLibraryInputStream::~MLispLibraryInputStream()
	{ }

//
//
//	Binding_list implementation
//
//
Binding_list::Binding_list()
	: bl_exp()
	, bl_flink(NULL)
	, bl_arg_index(0)
	{ }

Binding_list::~Binding_list()
	{ }



/* name of the current file being parsed */
static EmacsString interactive_filename("-interactive-");
static EmacsString cur_mlisp_file( interactive_filename );

/*
 * ParseNode parses an MLisp program node and returns a pointer to it.
 * 0 is returned if the parse was unsuccessful. Getc is a function to be
 * called to fetch the next character -- it should return -1 on errors.
 */
ProgramNode *ProgramNode::parse_node( MLispInputStream &input )
	{
	int c;
	/*
	 *	Trim all spaces and comments
	 */
	for(;;)
		{
		do
			c = input();
		while( c >= 0 && isspace( c ) );

		if( c != ';' )
			break;
		lisp_comment( input );
		}

	if( c == '(' )
		return paren_node( input );
	if( c < 0 || c == ')' )
		{
		input.pushBack( c );
		return 0;
		}
	if( c == '"' )
		return string_node( input );
	input.pushBack( c );
	if(c == '\'' || c == '-' || isdigit( c ) )
		return number_node( input );
	else
		return name_node( input );
	}

/* LispComment handles lisp style comments */
void ProgramNode::lisp_comment( MLispInputStream &input )
	{
#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Comment: line %d\n") << input.currentLine() );
#endif

	int c;
	do
		c = input();
	while( c > 0 && c != '\n' );
	}

/* ParseName parses a name from the MLisp input stream */
EmacsString ProgramNode::parse_name( MLispInputStream &input )
	{
	EmacsString name;
	int c;

	do
		c = input();
	while( isspace( c ) );

	while( c > 0
	&& ! isspace( c )
	&& c != '(' && c != ')' && c != ';' )
		{
		name.append( (unsigned char)c );
		c = input();
		}
	input.pushBack( c );

#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Parse name: line %d name %s\n") << input.currentLine() << name );
#endif

	return name;
	}

/* parse a parenthesised node WITH SPECIAL CODE FOR *defun( DLK )
 * In order to get a lisp-style defun syntax, we have
 * to hack the reader to recognize a special case
 * -( defun function-name(var1 var2 ..[ varN ] ) ...
 * and turn it into
 * -( *defun (function-name(novalue var1 ..[ varN ] ) */

ProgramNode *ProgramNode::paren_node( MLispInputStream &input )
	{
	int c;
	int nargs = 0;
	ProgramNode *args[MAXNODES];	/* alas, yet another hard-wired limitation! */
	ProgramNodeNode *p;
	BoundName *who;
	ProgramNodeNode *converted_defun = NULL;

	EmacsString name = parse_name( input );

	if( name.isNull() )
		return 0;
	who = BoundName::find( name );
	if( who == NULL )
		who = EMACS_NEW BoundName( name );

#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Parse node: line %d ( name %s\n") << input.currentLine() << name );
#endif

	/*
	 *	If we are doing a 'defun', check_ for the special case mentioned
	 *	at the introduction to this function( defun fun(var1 var2 .[ varN ])... )
	 *	If we are doing just such a defun, convert it to the *defun form.
	 */
	if( who == &bound_defun_node )
		{
		/*
		 *	Skip blanks and any comments
		 */
		for(;;)
			{
			do
				c = input();
			while( c >= 0 && isspace( c ) );

			if( c != ';' )
				break;
			lisp_comment( input );
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
			input.pushBack( c );
		else
			{
			/*
			 *	This is it, the *defun you have all been waiting for!!
			 */
			input.pushBack( c );	// Push back the 1st character

			/*
			 *	Setup who with the function name( Same code as above )
			 */
			name = parse_name( input );
			if( name.isNull() )
				{
				return 0;
				}
			who = BoundName::find( name );
			if( who == NULL )
				who = EMACS_NEW BoundName( name );

			/*
			 *	Now scan for the open paren of the arg list
			 *	THERE HAD BETTER BE ONE, OR THIS IS AN error
			 */
			do
				c = input();
			while( c >= 0 && isspace( c ) );
			if( c != '(' )
				{
				error( "Syntax error in new-style defun -- incorrect arg list" );
				return 0;
				}

			/*
			 *	Gather up the variables as an argument list
			 */
			while( ! ml_err
			&& (args[ nargs ] = parse_node( input )) != 0 )
				{
				nargs++;
				if( nargs >= MAXNODES )
					{
					int i;

					error( "MLisp expression is too complex" );
					for( i=0; i<=nargs - 1; i += 1 )
						delete args[ i ];
					return 0;
					}
				}
			// what does this do?
			if( input.peek() == ')' )
				input();

			p = EMACS_NEW ProgramNodeNode( BoundName::find( "novalue" ), nargs );
			memcpy( p->pa_node, args, nargs*sizeof( ProgramNode * ) );

			/*
			 *	Construct a ProgramNode for the *defun and save it
			 */
			converted_defun = EMACS_NEW ProgramNodeNode( &bound_star_defun_node, 1 );

			/*
			 *	Save the arg list prognode for later
			 */
			converted_defun->pa_node[0] = p;
			nargs = 1;
			/* Rest of the evaluation will be from arg1 */
			}
		}

	while( ! ml_err
	&& (args[ nargs ] = parse_node( input )) != 0 )
		{
		if( nargs >= MAXNODES )
			{
			int i;

			error( FormatString("MLisp expression is too complex in %s") << cur_mlisp_file );
			for( i=0; i<=nargs - 1; i += 1 )
				delete args[ i ];
			return 0;
			}
		nargs++;
		}

	if( input.peek() == ')' )
		input();
	else if( input.peek() < 0 )
		error( FormatString("Unexpected EOF possible parens mismatched in %s") << cur_mlisp_file );

	p = EMACS_NEW ProgramNodeNode( who, nargs );
	memcpy( p->pa_node, args, nargs * sizeof( ProgramNode * ) );


#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Parse node: line %d %s )\n") << input.currentLine() << name );
#endif

	/*
	 *	If this is a converted Defun, fix up the pointers and return it
	 */
	if( converted_defun != NULL )
		{
		p->pa_node[0] = converted_defun->pa_node[0];
		converted_defun->pa_node[0] = p;
		return converted_defun;
		}
	/*
	 *	Otherwise return p
	 */
	return p;
	}

unsigned char msg_imp_char[] = "'%s' is an improper character constant.";

ProgramNode *ProgramNode::number_node( MLispInputStream &input )
	{
	int n;
	int c;

	c = input();
	n = 0;

	if( c == '\'' )
		{
		unsigned char buf[30];
		buf[ 0 ] = (unsigned char)input();
		do
			{
			c = input();
			n++;
			buf[ n ] = (unsigned char)c;
			}
		while( n < 29
		&& (c != '\'' || n == 1 && buf[0] == '\\') );

		buf[ n ] = 0;
		if( n > 10 )
			{
			error( FormatString(msg_imp_char) << buf );
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
							error( FormatString(msg_imp_char) << buf );
							goto number_block;
							}
						}
					else
						error( FormatString(msg_imp_char) << buf );
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
			c = input();
			}
		if( c == '0' )
			base = 8;
		while( isdigit( c ) )
			{
			n = n * base + c - '0';
			c = input();
			}
		if( neg )
			n = -n;
		input.pushBack( c );
		}
number_block:

#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Parse number: line %d number %d\n") << input.currentLine() << n );
#endif

	return EMACS_NEW ProgramNodeInt( n );

	}

ProgramNode *ProgramNode::string_node( MLispInputStream &input )
	{
	int c;
	EmacsString buf;

	while( (c = input()) > 0 )
		{
		if( c != '\\' )
			{
			if( c == '"'
			&& (c = input()) != '"' )
				{
				input.pushBack( c );
				break;
				}
			}
		else
		switch( c = input() )
			{
		case 'n': c = 10; break;	/* LF */
		case 'b': c =  8; break;	/* BS */
		case 't': c =  9; break;	/* TAB */
		case 'r': c = 13; break;	/* CR */
		case 'e': c = 27; break;	/* ESC */
		case '^':
			{
			c = input();
			if( c == '?' )
				c = 127	/* DEL */;
			else
				c = c & 037;
			break;
			}
		case '(':	// LK201 key names
		case '[':	// PC key names
			{
			EmacsString key_name;

			int final_char = c == '(' ? ')' : ']';
			c = input();
			while( c != final_char )
				{
				if( c == 0 || c == '"' || c < 0 )
					{
					error( "Closing \"\" missing in keyname escape sequence" );
					return 0;
					}
				key_name.append( (unsigned char)c );
				c = input();
				}

			EmacsString translation
				(
				final_char == ')' ?
					LK201_key_names.valueOfKeyName( key_name ) :
					PC_key_names.valueOfKeyName( key_name )
				);
			if( translation.isNull() )
				{
				error( FormatString("Unknown keyname %s in string") << key_name );
				return 0;
				}

			buf.append( translation );
			continue;
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
				&& '0' <= (c = input())
				&& c <= '7' );

				if( cnt > 0 )
					input.pushBack( c );
				c = nc;
				}
			}
			}
		if( c < 0 )
			{
			error( "Unterminated string constant" );
			return 0;
			}
		buf.append( (unsigned char)c );
		}

#if DBG_ML_PARSE
        if( dbg_flags&DBG_ML_PARSE )
                _dbg_msg( FormatString("Parse string: line %d string %s\n") << input.currentLine() << buf );
#endif

	return EMACS_NEW ProgramNodeString( buf );
	}


int execute_mlisp_buffer( void )
	{
	int rv;

	MLispBufferInputStream buffer_stream;

	rv = ProgramNode::execute_mlisp_stream( buffer_stream );
	if( ml_err )
		set_dot( buffer_stream.position() - 1 );
	return rv;
	}


ProgramNode *ProgramNode::parse_mlisp_line( const EmacsString &s )
	{
	if( s.isNull() )
		return NULL;
	
	MLispStringInputStream input_stream( s );

	return parse_node( input_stream );
	}

int execute_mlisp_string( const EmacsString &mlisp_command )
	{
	MLispStringInputStream input_stream( mlisp_command );

	int rv = ProgramNode::execute_mlisp_stream( input_stream );

	return rv;
	}

int execute_mlisp_line( void )
	{
	EmacsString line( getstr( ": execute-mlisp-line " ) );
	int rv = execute_mlisp_string( line );

	if( !ml_err && interactive() )
		{
		if( ml_value.exp_type() == ISINTEGER )
			message( FormatString("%s => %d") << line << ml_value.asInt() );
		else
		if( ml_value.exp_type() == ISSTRING )
				message( FormatString("%s => \"%s\"") << line << ml_value.asString() );

		ml_value = Expression();
		}

	return rv;
	}
	

int ProgramNode::execute_mlisp_stream( MLispInputStream &input )
	{
	int rv = 0;

	while( input.peek() >= 0
	&& rv == 0
	&& ! ml_err )
		{
		ProgramNode *p = ProgramNode::parse_node( input );
		if( p == NULL )
			break;

		rv = exec_prog( p );
		delete p;

		int c;
		do
			c = input();
		while( c >= 0 && isspace( c ) );

		input.pushBack( c );
		}

	return rv;
	}

/*
 *	Enhance execute-mlisp-file and load to scan the text libs
 *	in the search list 'procedures'.
 *	Barry A. Scott	30-Oct-1982
 */
int execute_mlisp_file( const EmacsString &fn, int missingOK )
	{
	if( fn.isNull() )
		return 0;

#if DBG_EXECFILE
        if( dbg_flags&DBG_EXECFILE )
                _dbg_msg( FormatString("execute-mlisp-file >> %s\r\n") << fn );
#endif
	
	Save<EmacsString> saved_cur_mlisp_file( &cur_mlisp_file );
	cur_mlisp_file = fn;

	int rv = 0;

	{
	MLispFileInputStream file_stream( fn );

	if( file_stream.isOk() )
		rv = ProgramNode::execute_mlisp_stream( file_stream );
	else
		{
		MLispLibraryInputStream lib_stream( fn );

		if( lib_stream.isOk() )
			rv = ProgramNode::execute_mlisp_stream( lib_stream );
		else
			if( !missingOK )
				error( FormatString("Cannot read %s") << fn );
		}
	}


#if DBG_EXECFILE
        if( dbg_flags&DBG_EXECFILE )
                _dbg_msg( FormatString("execute-mlisp-file << %s\r\n") << fn );
#endif

	return rv;
	}

int execute_mlisp_file_command( void )
	{
	EmacsString s = getstr( ": execute-mlisp-file " );
	if( s.isNull() )
		return 0;
	return execute_mlisp_file( s, 0 );
	}

VariableName *VariableName::find( const EmacsString &name )
	{
	return name_table.find( name );
	}

/*
 * define a varible name given the string name
 * and a pointer to the descriptor record
 */
void VariableName::define()
	{
	name_table.add( v_name, this );
	}

/*
 * parse a name token in an MLisp program
 */
ProgramNode *ProgramNode::name_node( MLispInputStream &input )
	{
	EmacsString name = parse_name( input );
	if( name.isNull() )
		return 0;

	VariableName *v = VariableName::find( name );
	if( v == NULL )
		{
		v = EMACS_NEW VariableName( name, NULL );
		}

	return EMACS_NEW ProgramNodeVariable( v );
	}

ProgramNodeNode::ProgramNodeNode( BoundName *proc, int nargs )
	: ProgramNode( proc )
	, pa_node( (ProgramNode **)EMACS_MALLOC( sizeof(ProgramNode *) * nargs, malloc_type_star_star ) )
	{
	p_nargs = nargs;

	for( int i=0; i<p_nargs; i++ )
		pa_node[i] = NULL;
	}

ProgramNodeNode::~ProgramNodeNode()
	{
	for( int i=0; i<p_nargs; i++ )
		delete pa_node[i];
	EMACS_FREE( pa_node );
	}
