/* 
 * Motif X-Window system pseudo-terminal driver for DEC Emacs
 *
 * Dynamic key translation table parser
 */

#include <emacs.h>

#ifdef XWINDOWS
# include <sys/time.h>
# include <X11/X.h>
# include <X11/Intrinsic.h>
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/IntrinsicP.h>
# include <X11/StringDefs.h>
# include <X11/keysym.h> 
# include <Xm/Xm.h>
# include <Xm/MainW.h>
# include <Xm/PushB.h>
# include <Xm/CascadeB.h>
# include <Xm/RowColumn.h>
# include <Xm/Frame.h>
# include <Xm/DrawingA.h>
# include <Xm/BulletinB.h>
# include <Xm/ScrollBar.h>

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#include <motif_keys.h>


EmacsMotifKeySymMap::EmacsMotifKeySymMap()
	: buffer( NULL )
	, get_ptr( NULL )
	, buffer_end( NULL )
	, linenr( 1 )
	, line_start( NULL )
	{ }

EmacsMotifKeySymMap::~EmacsMotifKeySymMap()
	{ }

const unsigned char *EmacsMotifKeySymMap::lookup( KeySym k, int modifiers ) const
	{
	for( int i = 0; i < map_entries; i++ )
		{
		if( k == map[i].key_code )
			{
			const unsigned char *p = NULL;
			if( modifiers & Mod1Mask )
				p = map[i].enh_translation;
			else if( input_mode == 0 && modifiers & ControlMask && modifiers & ShiftMask )
				p = map[i].ctrl_shift_translation;
			else if( input_mode == 0 && modifiers & ShiftMask )
				p = map[i].shift_translation;
			else if( modifiers & ControlMask )
				p = map[i].ctrl_translation;
			else
				p = map[i].translation;
			if( p == NULL )
				p = map[i].translation;
			return p;
			}
		}

	return NULL;
	}

int EmacsMotifKeySymMap::kf_getch()
	{
	if( get_ptr >= buffer_end )
		return -1;

	return *get_ptr++;
	}


int EmacsMotifKeySymMap::kf_getns()
	{
	int c;
	while( (c = kf_getch()) != -1)
		{
		if( c != ' ' &&  c != '\t' )
			return c;
		}
	return -1;
	}


void EmacsMotifKeySymMap::kf_ungetch()
	{
	get_ptr--;
	assert( get_ptr > buffer );
	}


/* \? sequence - return escaped character if ? is valid else return -1 */
static int escaped_char( int c )
	{
	switch( c )
		{
	case 'e':	return 033;
	case 't':	return 011;
	case 'r':	return 015;
	case 'n':	return 012;
	case '\\':
	case '\'':
	case '"':
			return c;
	default:
			return -1;
		}
	}

int EmacsMotifKeySymMap::gettoken( int &integer_result, EmacsString &string_result )
	{
	int c, b, intv, intp;
	EmacsString ds;

	c = kf_getns();
	//
	//  Next token must be one of ...
	//	- an integer
	//	- a name
	//	- a quoted character
	//	- a quoted string
	//	- valid punctuation
	//

	if( c == '\n' ||  c == ',' )
		{
		/* === Valid Punctuation === */

		return c;
		}

	if (c == '\'')
		{
		/* === Quoted Character === */

		if ((c = kf_getch()) < 0)
			return c;
		if (c == '\'')
			return -2;

		if (c == '0')
			{
			if ((c = kf_getch()) < 0) return c;

			if (c == '\'')
				return -2;
			if (c == 'x' || c == 'X')
				{
				for ( intv = 0, intp = 0 ; intp < 8 ; intp++ )
					{
					if ((c = kf_getch()) < 0) return c;

					if      ('0' <= c && c <= '9')
						intv = (intv<<4) | (c-'0');
					else if ('a' <= c && c <= 'f')
						intv = (intv<<4) | (c-'a'+10);
					else if ('A' <= c && c <= 'F')
						intv = (intv<<4) | (c-'A'+10);
					else 	{
						kf_ungetch();
						break;
						}
					}
				if ((intp & 1) || (intp < 2))
					return -2;
				b = intv;
				}
			else 	{
				kf_ungetch();
				for ( intv = 0, intp = 0 ; intp < 3 ; intp++ ) 	{
					if ((c = kf_getch()) < 0)
						return c;
					if ('0' <= c && c <= '7')
						intv = (intv<<3) | (c-'0');
					else 	{
						kf_ungetch();
						break;
						}
					}
				b = intv;
				}
			}
		else if (c == '\\')
			{
			if ((c = kf_getch()) < 0)
				return c;
			if ((b = escaped_char(c)) == -1)
				return -2;
			}
		else
			b = c;

		if ((c = kf_getch()) < 0)
			return c;
		if (c != '\'')
			return -2;

		integer_result = b;
		return 'I';
		}

	if ( c == '\"')
		{
		/* === Quoted String === */

		for ( int qstate = 0 ; (c = kf_getch()) >= 0 ; )
			{
			switch (qstate)
				{
			case 0:
				if (c == '\"')
					{
					string_result = ds;
					return 'S';
					}
				else if (c == '\\')
					qstate =  1;
				else
					ds.append( c );
				break;

			case 1:
				if ('0' <= c && c <= '7')
				 	{
					intv = c-'0';
					intp = 1;
					qstate = 2;
					}
				else 	{
					if ((b = escaped_char(c)) >= 0)
						ds.append( b );
					else
					 	{
						ds.append('\\');
						ds.append( c );
						}
					qstate = 0;
					}
				break;

			case 2:
				if (c == 'x' || c == 'X') 	{
					intv = 0;
					intp = 0;
					qstate = 4;
					}
				else 	{
					kf_ungetch();
					qstate = 3;
					}
				break;

			case 3:
				if ('0' <= c && c <= '7')
				 	{
					intv = (intv<<3) | (c-'0');
					if (++intp == 3)
						{
						ds.append( intv );
						qstate = 0;
						}
					}
				else 	{
					kf_ungetch();
					ds.append( intv );
					qstate = 0;
					}
				break;

			case 4:
				if      ('0' <= c && c <= '9') b = c-'0';
				else if ('a' <= c && c <= 'f') b = c-'a'+10;
				else if ('A' <= c && c <= 'F') b = c-'A'+10;
				else 	{
					kf_ungetch();
					if ((intp & 1) || (intp < 2))
						return -2;
					b = -1;
					}
				if (b >= 0) 	{
					intv = (intv<<4) | b;
					if (++intp == 8) b = -1;
					}
				if (b < 0) 	{
					ds.append( intv );
					qstate = 0;
					}
				break;
				}
			}
		return -1;
		}

	if( isalpha( c ) )
		{
		/* === Name (Alpha followed by alphanumerics) === */
		ds.append( c );
		while ((c = kf_getch()) >= 0  &&  (c == '_' || isalnum(c)))
			ds.append( c );

		if( c < 0 )
			return c;

		kf_ungetch();

		string_result = ds;
		if ( string_result == "NULL" )
		 	{
			string_result = EmacsString::null;
			return 'S';
			}

		return 'N';
		}

	return -9;
	}


int EmacsMotifKeySymMap::getentry()
	{
	char tr[12];
	int ir[12];
	EmacsString sr[12];
	int result;

	for ( int i = 0; i < 12 ; i++ )
		{
		ir[i] = -1;
		result = gettoken( ir[i], sr[i]);
		tr[i] = (char)result;
		if( result < 0 )
			break;
# if DBG_KEY
		if( dbg_flags&DBG_KEY && dbg_flags&DBG_VERBOSE )
			_dbg_msg( FormatString("Token[%c] Num=%d 0x%08X \"%s\" ") << tr[i] << ir[i] << ir[i] << sr[i] );
# endif
		if (tr[i] == 'I')
			sr[i] = FormatString("%08X") << ir[i];
		}

	if( result < 0 )
		return result;

	if( (tr[0] != 'N' && tr[0] != 'I')
	|| tr[1] != ','
	|| tr[2] != 'S'
	|| tr[3] != ','
	|| tr[4] != 'S'
	|| tr[5] != ','
	|| tr[6] != 'S'
	|| tr[7] != ','
	|| tr[8] != 'S'
	|| tr[9] != ','
	|| tr[10] != 'S'
	|| tr[11] != '\n' )
		return -2;

	struct keysym_map &e = map[map_entries];

	e.key_code = KeySym(ir[0]);
	_str_cpy( e.translation, sr[2].sdata() );
	_str_cpy( e.enh_translation, sr[4].sdata() );
	_str_cpy( e.shift_translation, sr[6].sdata() );
	_str_cpy( e.ctrl_translation, sr[8].sdata() );
	_str_cpy( e.ctrl_shift_translation, sr[10].sdata() );
	if (tr[0] == 'N')
	 	{
		e.key_code = XStringToKeysym(sr[0]);
		if( e.key_code == 0
		&& sr[0].commonPrefix("XK_") == 3)
			e.key_code = XStringToKeysym(sr[0].sdata()+3);


		if( e.key_code == 0 )
			_dbg_msg( FormatString("Keysym %s is not known") << sr[0] );

# if DBG_KEY
		if( dbg_flags&DBG_KEY )
			_dbg_msg( FormatString("Looking up %s ...... result = %x") << sr[0] << int(e.key_code) );
# endif
		}

	map_entries++;

	return 0;
	}

bool EmacsMotifKeySymMap::init()
	{
	EmacsFile key_file;

	if( !key_file.fio_open_using_path( EMACS_PATH, "emacs.xkeys", 0, "" ) )
		{
		_dbg_msg( FormatString("Emacs X11 keysym file cannot be found in the emacs path: %s") << "emacs.xkeys" );
		return false;
		}

	unsigned char file_data[32768];

	if( key_file.fio_size() > int(sizeof( file_data )) )
		{
		_dbg_msg( FormatString("Emacs X11 keysym file is too big: %s") << key_file.fio_getname() );
		return false;
		}

	int file_data_size = key_file.fio_get( file_data, sizeof( file_data ) );
	if( file_data_size < 0 )
		{
		_dbg_msg( FormatString("Emacs X11 keysym file read error: %s") << key_file.fio_getname() );
		return false;
		}

	//
	//	Setup the pointers for the parser
	//
	buffer = file_data;
	get_ptr = file_data;
	buffer_end = file_data + file_data_size;


	//
	//	Fill up the keysym array
	//
	while( map_entries < max_map_entries )
		{
		int c = 1;
		// Skip leading whitespace and any comments following

		while( c > 0 )
			{
			switch( c = kf_getns() )
				{
			case '#':
				while ( (c = kf_getch()) != '\n' )
					if (c < 0)
						break;
				linenr++;
				break;
			case '\n':
				linenr++;
				break;
			case -1:
				break;
			default:
				kf_ungetch();
				c = 0;
				break;
				}
			}

		if (c < 0)
			break;

		line_start = get_ptr;

		int n = getentry();
		if (n < -1)
			{
			int line_length_parsed = get_ptr - line_start;
			_dbg_msg( FormatString("Error in '%s' line %d\n'..%.*s..'\n   %*s^ ")
					<< key_file.fio_getname() << linenr
					<< line_length_parsed << line_start
					<< line_length_parsed << "" );

			// skip the rest of the line
			while( (c = kf_getch()) >= 0 )
				if( c == '\n' )
					{
					n = 0;	// don't exit
					break;
					}
			}
		if( n < 0 )
			break;

		linenr++;
		}

	return true;
	}
#endif	    /* ifdef XWINDOWS */
