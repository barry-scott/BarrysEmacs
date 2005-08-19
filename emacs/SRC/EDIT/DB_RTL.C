/*
 *	db_rtl.c	- run time support for the dbxxx utilities
 */
#include <stdlib.h>
#include <string.h>

#define u_str(s) ((unsigned char *)(s))
#define s_str(s) ((char *)(s))
#ifdef __cplusplus
#define PNOTUSED(arg) /* arg */
#else
#define PNOTUSED(arg) arg
#endif

int dbg_flags = 0;

enum malloc_block_type
	{
	malloc_type_char
	};

void *emacs_malloc (int len, enum malloc_block_type PNOTUSED(type))
    {
    return malloc (len);
    }

void emacs_free ( void *ptr )
    {
    free (ptr);
    }

int match_wild ( unsigned char *PNOTUSED(a), unsigned char *PNOTUSED(b) )
    {
    return 0;
    }

int expand_and_default( unsigned char *fn, unsigned char *dn, unsigned char *ou )
	{
	strcpy( s_str(ou), s_str(fn) );
	strcat( s_str(ou), s_str(dn) );
	return 0;
	}

unsigned char *savestr( unsigned char *s )
	{
	int len = strlen( s_str(s) );
	unsigned char *p = u_str(malloc( len+1 ));
	strcpy( s_str(p), s_str(s) );
	return p;
	}
