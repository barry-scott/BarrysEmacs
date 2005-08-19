/*	Copyright 1985-1992
 *		Barry A. Scott & Nick Emery
 *
 *	macros required for bliss to access Emacs data structures
 */
#define u_str( str ) ((unsigned char *)(str))
#define s_str( str ) ((char *)(str))

#define char_at( n ) ((unsigned char)( (n) > bf_s1 ? bf_p2[(n)] : bf_p1[(n)]))
#define	ref_char_at( n ) ((unsigned char *)( (n) > bf_s1 ? &bf_p2[(n)] : &bf_p1[(n)]))

#define	first_character bf_mode.md_headclip
#define	num_characters (bf_s1+bf_s2-bf_mode.md_tailclip)
#define	interactive (mem_ptr == 0 && cur_exec == 0)

#define	release_expr(e) \
	{ \
	struct expression *_expr_; \
	_expr_ = (e); \
	if( _expr_ != 0 \
	&& _expr_->exp_release ) \
		do_release (e); \
	}

#include <ctype.h>

#if defined( _DEBUG ) && 0
 extern must_be_zero;
#else
#ifdef _MSC_VER
#pragma warning( disable:4127 )
#endif
# define must_be_zero 0
#endif
/*
 * cur_col returns the current print column number for dot, which may
 * have to be calculated
 */
#define cur_col (col_valid ? dot_col : calc_col())

/*
 * dot should ONLY be given a value by calling set_dot(new) -- it ensures
 * that all associated bookkeeping is done.
 */
#define set_dot(n) (col_valid = 0, dot = (n))

/*
 * dot should be moved left or right using the following macros -- they
 * attempt (or will, eventually) to keep dot_col valid.  They do not check
 * the new valid of dot: you have to do that.
 */
#define dot_right(n) do {col_valid = 0; dot = dot + (n);} while( must_be_zero != 0 )
#define dot_left(n) do {col_valid = 0; dot = dot - (n);} while( must_be_zero != 0 )

#define control_character(c) ((c & 0x7F) < ' ' || (c & 0x7f) == 0x7F)

#define ins_str( insstr_str ) \
    do { \
    unsigned char *insstr_s = (insstr_str); \
    ins_cstr( insstr_s, _str_len( insstr_s ) ); \
    break; \
    } while( must_be_zero != 0 )

#define ctl( ch ) (ch & 0x9f)

#define	_str_len( a ) strlen( s_str( a ) )
#define _str_cat( a, b ) strcat( s_str( a ), s_str( b ) )
#define _str_chr( a, b ) u_str(strchr( s_str( a ), b ))
#define _str_rchr( a, b ) u_str(strrchr( s_str( a ), b ))
#define _str_cmp( a, b ) strcmp( s_str( a ), s_str( b ) )
#define _str_icmp( a, b ) emacs_stricmp( a, b )
#define _str_cpy( a, b ) strcpy( s_str( a ), s_str( b ) )
#define _str_ncmp( a, b, c ) strncmp( s_str( a ), s_str( b ), c )
#define _str_nicmp( a, b, c ) emacs_strnicmp( a, b, c )
#define _str_ncpy( a, b, c ) strncpy( s_str( a ), s_str( b ), c )
#define _str_ncat( a, b, c ) strncat( s_str( a ), s_str( b ), c )
#define	_str_str( a, b ) strstr( s_str( a ), s_str( b ) )
#define _str_lwr( a ) emacs_strlwr( a )
#define _str_upr( a ) emacs_strupr( a )

#ifdef SAVE_ENVIRONMENT
#if defined(malloc)
#undef malloc
#undef free
#undef calloc
#undef cfree
#endif
#define	malloc(a,t)	emacs_malloc(a,t)
#define	free(a)		emacs_free(a)

#define	realloc(a,b,t)	emacs_realloc(a,b,t)
#define realloc_ustr(a,b) ((unsigned char *)emacs_realloc(a,b,malloc_type_char))

#define malloc_struct( s ) ((struct s *)malloc( sizeof( struct s ), malloc_type_struct_##s ))
#define malloc_ustr( i ) ((unsigned char *)malloc( i, malloc_type_char ))
#else
#define	malloc(a,t)	emacs_malloc(a)
#define	free(a)		emacs_free(a)

#define	realloc(a,b,t)	emacs_realloc(a,b)
#define realloc_ustr(a,b) ((unsigned char *)emacs_realloc(a,b))

#define malloc_struct( s ) ((struct s *)malloc( sizeof( struct s ) ) )
#define malloc_ustr( i ) ((unsigned char *)malloc( i ))
#endif

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif

#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifdef __cplusplus
#define PNOTUSED(arg)
#else
#define PNOTUSED(arg) arg
#endif
