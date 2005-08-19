/*
 *	Emacs_rtl.c
 *
 *		Copyright (c) 1992 Barry A. Scott
 */
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <ctype.h>

int _dbg_fn_trace::callDepth()
	{
	return s_call_depth;
	}

int _dbg_fn_trace::s_call_depth(0);

_dbg_fn_trace::_dbg_fn_trace( const EmacsString &fn_name )
	: m_fn_name( fn_name )
	, m_call_depth( s_call_depth )
	{
	s_call_depth++;

	_dbg_msg( FormatString("Enter: %s()") << m_fn_name );	
	}

_dbg_fn_trace::~_dbg_fn_trace()
	{
	_dbg_msg( FormatString("Leave: %s()") << m_fn_name );	

	s_call_depth = m_call_depth;
	}

int emacs_stricmp( const unsigned char *str1, const unsigned char *str2 )
	{
	while( *str1 != '\0' && (toupper(*str1) == toupper(*str2)) )
		{ str1++; str2++; }

	if( *str1 == *str2 )
		return 0;
	if( toupper(*str1) < toupper(*str2) )
		return -1;
	else
		return 1;
	}

int emacs_strnicmp( const unsigned char *str1, const unsigned char *str2, int maxchar )
	{
	maxchar--;
	while(						/* while */
		maxchar > 0				/* inside the limit */
		&& *str1 != 0				/* and the string has not finished */
		&& (toupper(*str1) == toupper(*str2)) )	/* and there is a match */
			{ maxchar--; str1++; str2++; }	/* step on */

	/* figure out the result */
	if( *str1 == *str2 )
		return 0;
	if( toupper(*str1) < toupper(*str2) )
		return -1;
	else
		return 1;
	}

unsigned char *emacs_strlwr( unsigned char *str )
	{
	unsigned char *s = str;

	while( *s )
		{
		if( isupper( *s ) )
			*s = (unsigned char)tolower( *s );
		s++;
		}

	return str;
	}

unsigned char *emacs_strupr( unsigned char *str )
	{
	unsigned char *s = str;

	while( *s )
		{
		if( islower( *s ) )
			*s = (unsigned char)toupper( *s );
		s++;
		}

	return str;
	}

#ifdef vms
void _dbg_msg( EmacsString fmt, ... )
	{
	EmacsString buf;
	va_list argp;
	struct dsc$descriptor str;
	va_start( argp, fmt );

	do_print( fmt, &argp, buf );

	DSC_SZ( str, buf.sdata() );
	lib$put_output( &str );
	}
#endif

/* save a string in managed memory */
unsigned char * savestr( const unsigned char *s )
	{
	if( s == NULL )
		// return a nul string
		return savestr( u_str("") );

	// copy the string
	int size = _str_len( s ) + 1;
	unsigned char *ret = malloc_ustr( size );
	_str_cpy( ret, s );
	return ret;
	}
