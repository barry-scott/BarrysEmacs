/*
 *	Emacs_rtl.c
 *
 *		Copyright (c) 1992 Barry A. Scott
 */
#include <emacs.h>
#include <ctype.h>


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

#if 0
int load_ctype_please( int a )
	{
	return isdigit(a);
	}
#endif

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

#ifdef vms
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

void _dbg_msg( unsigned char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;
	struct dsc$descriptor str;
	va_start( argp, fmt );

	i = do_print( fmt, &argp, (unsigned char *)buf, sizeof( buf ) );
	buf[i] = 0;

	DSC_SZ( str, buf );
	lib$put_output( &str );
	}
#endif
