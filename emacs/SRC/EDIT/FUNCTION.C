/*	Copyright (c) 1982-1993
 *		Barry A. Scott
 */
#include <emacs.h>

GLOBAL struct bound_name *mac_bodies[MAXMACS];
GLOBAL unsigned char *mac_names[MAXMACS];
GLOBAL SAVRES unsigned char **mac_names_ref = &mac_names[0];

extern int Expand_and_exit(void);
extern int help_and_exit(void);

void init_fncs2( void )
	{
	struct prognode *p;
	int rv;

	/*
	 *	Give us a way out of an emacs that failes to init keymaps
	 */
	p = parse_mlisp_line( u_str( "(bind-to-key \"exit-emacs\" \"\\^c\")" ) );
	if( p == 0 ) return;

	rv = exec_prog( p );
	lisp_free( p );
	}

/* Define all the funtion prototypes */
#define defproc(name,func) extern int func(void)
#define defprocset(name,func,blk) extern int func(void)
#define PART1 1
#define PART2 1
#include <funclist.h>

/* Define all the macros */
static void init_fncs_a( void )
	{
	union bound_ptr x;

#undef defproc
#define defproc(name,func) x.b_proc = func; def_mac( u_str(name), x.b_ptr, -5, 0, 0 )
#undef defprocset
#define defprocset(name,func,blk) x.b_proc = func; def_mac( u_str(name), x.b_ptr, -5, 0, &blk )

#undef PART2
#include <funclist.h>
	}

static void init_fncs_b( void )
	{
	union bound_ptr x;

#undef PART1
#define PART2
#include <funclist.h>
	}

void init_fncs( void )
	{
	init_fncs_a();
	init_fncs_b();
	}
