/*
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and Nick Emery
 */
#include <emacs.h>

/*
 *	Define an integer system variable
 */
#define Int(name, check, thunk, addr, def, is_def) \
	{ \
	static unsigned char n[] = name; \
	static struct expression w = { \
		0,		/* exp_int */ \
		{(unsigned char *)&addr},	/* exp_v */ \
		ISINTEGER,	/* exp_type */ \
		0		/* exp_release */ \
		}; \
	static struct binding x = { \
		0,		/* b_inner */ \
		&w,		/* b_exp */ \
		{(int (*)( int, struct variablename *))check},	/* b_rangecheck */ \
		{(int(*)(struct expression *))thunk},		/* b_thunk */ \
		{0},		/* b.b_default */ \
		1,		/* b_is_system */ \
		0,		/* b_buffer_specific */ \
		is_def		/* b_is_default */ \
		}; \
	static struct variablename y = { \
		n,		/* v_name */ \
		&x		/* v_binding */ \
		}; \
	x.b.b_default = def; \
	define_variable( n, &y ); \
	Def = &x; \
	}

/*
 *	Define an string system variable
 */
#define Str(name, check, thunk, addr, def, is_def) \
	{ \
	static unsigned char n[] = name; \
	static struct expression w = { \
		sizeof( addr ),	/* exp_int */ \
		{(unsigned char *)(&addr[0])},	/* exp_v */ \
		ISSTRING,	/* exp_type */ \
		0		/* exp_release */ \
		}; \
	static struct binding x = { \
		0,		/* b_inner */ \
		&w,		/* b_exp */ \
		{(int (*)( int, struct variablename *))check},	/* b_rangecheck */ \
		{(int(*)(struct expression *))thunk},	/* b_thunk */ \
		{0},		/* b.b_default */ \
		1,		/* b_is_system */ \
		0,		/* b_buffer_specific */ \
		is_def		/* b_is_default */ \
		}; \
	static struct variablename y = { \
		n,		/* v_name */ \
		&x		/* v_binding */ \
		}; \
	x.b.b_default = def; \
	define_variable( n, &y ); \
	Def = &x; \
	}

/*
 *	Define an window system variable
 */
#define Win(name, check, thunk, def, is_def) \
	{ \
	static unsigned char n[] = name; \
	static struct expression w = { \
		0,		/* exp_int */ \
		{0},		/* exp_v */ \
		ISWINDOWS,	/* exp_type */ \
		0		/* exp_release */ \
		}; \
	static struct binding x = { \
		0,		/* b_inner */ \
		&w,		/* b_exp */ \
		{(int(*)( int, struct variablename *))check},	/* b_rangecheck */ \
		{(int(*)(struct expression *))thunk},	/* b_thunk */ \
		{0},		/* b.b_default */ \
		1,		/* b_is_system */ \
		0,		/* b_buffer_specific */ \
		is_def		/* b_is_default */ \
		}; \
	static struct variablename y = { \
		n,		/* v_name */ \
		&x		/* v_binding */ \
		}; \
	x.b.b_default = def; \
	define_variable( n, &y ); \
	Def = &x; \
	}

/*
 *	Define an array system variable
 */
#define Arr(name, check, thunk, def, is_def) \
	{ \
	static unsigned char n[] = name; \
	static struct expression u; \
	static struct emacs_array v = { \
		1,				/* array_ref_count */ \
		1,				/* array_dimensions */ \
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* array_lower_bound */ \
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},	/* array_size */ \
		0,				/* array_total_size */ \
		{&u}				/* array_expr */ \
	}; \
	static struct expression w = { \
		0,		/* exp_int */ \
		{(unsigned char *)&v},	/* exp_v */ \
		ISARRAY,	/* exp_type */ \
		0		/* exp_release */ \
		}; \
	static struct binding x = { \
		0,		/* b_inner */ \
		&w,		/* b_exp */ \
		{(int(*)( int, struct variablename *))check},	/* b_rangecheck */ \
		{(int(*)(struct expression *))thunk},	/* b_thunk */ \
		{0},		/* b.b_default */ \
		1,		/* b_is_system */ \
		0,		/* b_buffer_specific */ \
		is_def		/* b_is_default */ \
		}; \
	static struct variablename y = { \
		n,		/* v_name */ \
		&x		/* v_binding */ \
		}; \
	x.b.b_default = def; \
	define_variable( n, &y ); \
	Def = &x; \
	}

#define INI_VAR_T_SIZE (148+250)

GLOBAL SAVRES VOLATILE struct variablename **var_desc;
GLOBAL SAVRES VOLATILE unsigned char **var_names;
GLOBAL SAVRES VOLATILE int n_vars = 0;
GLOBAL SAVRES int var_t_size = INI_VAR_T_SIZE;


static unsigned char none[1];

void restore_var(void)
	{
	struct binding *Def = NULL;

#include <varlist.h>
	}

void init_var(void)
	{
	var_desc = (struct variablename **)malloc( sizeof( struct variablename * ) * INI_VAR_T_SIZE, malloc_type_star_star );
	if( var_desc == NULL ) fatal_error( 1 );
	var_names = (unsigned char **)malloc( sizeof( unsigned char * ) * INI_VAR_T_SIZE, malloc_type_star_star );
	if( var_names == NULL ) fatal_error( 1 );

	restore_var();
	}

