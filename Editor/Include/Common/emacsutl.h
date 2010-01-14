//
//    emacsutl.h
//    Copyright (c) 1996-2010 Barry A. Scott
//
//    Contains the mandatory definitions to configure emacs code
//
#ifndef __EMACSUTL_H__
#define __EMACSUTL_H__

// debug support
class EmacsString;
extern int dbg_flags;
extern void _dbg_msg( const EmacsString &msg );

#if defined( _MSC_VER )
// control the warning messages of the compiler
#pragma warning( disable : 4511 )    // "class" copy constructor could not be generated
#pragma warning( disable : 4512 )    // "class" assignment operator could not be generated
#pragma warning( disable : 4237 )    // nonstandard extension used : 'true' keyword is reserved for future use
#pragma warning( disable : 4514 )    // "func" : unreferenced inline function has been removed
#pragma warning( disable : 4711 )    // Function "x" selected for automatic inline expansion
#pragma warning( disable : 4290 )    // C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
#endif

class EmacsString;

#include <ctype.h>
#include <errno.h>
#include <os.h>
#include <string.h>

#include <em_gen.h>        // general constants used in emacs

#include <emunicode.h>

//
//    extern functions of global interest
//
extern void fatal_error( int );
extern void debug_invoke(void);
# if DBG_ALLOC_CHECK
extern void *emacs_malloc( int size, enum malloc_block_type, const char *fileName, int lineNumber );
extern void *emacs_realloc( void *p, int new_size, enum malloc_block_type, const char *fileName, int lineNumber );
# else
extern void *emacs_malloc( int size, enum malloc_block_type );
extern void *emacs_realloc( void *p, int new_size, enum malloc_block_type );
# endif
extern void emacs_free( void *p );
extern void emacs_check_malloc_block( void *p );


extern EmacsString get_config_env( const EmacsString & );

#define u_str( str ) ((unsigned char *)(str))
#define s_str( str ) ((char *)(str))

#define ctl( ch ) (ch & 0x9f)

#define _str_cat( a, b ) u_str( strcat( s_str( a ), s_str( b ) ) )
#define _str_chr( a, b ) u_str( strchr( s_str( a ), b ) )
#define _str_rchr( a, b ) u_str( strrchr( s_str( a ), b ) )
#define _str_cmp( a, b ) strcmp( s_str( a ), s_str( b ) )
#define _str_cpy( a, b ) u_str( strcpy( s_str( a ), s_str( b ) ) )
#define _str_ncmp( a, b, c ) strncmp( s_str( a ), s_str( b ), c )
#define _str_ncpy( a, b, c ) u_str( strncpy( s_str( a ), s_str( b ), c ) )
#define _str_ncat( a, b, c ) u_str( strncat( s_str( a ), s_str( b ), c ) )
#define _str_str( a, b ) u_str( strstr( s_str( a ), s_str( b ) ) )

# if DBG_ALLOC_CHECK
#  define    EMACS_MALLOC(a,t)      emacs_malloc(a,t,THIS_FILE,__LINE__)
#  define    EMACS_FREE(a)          emacs_free(a)

#  define    EMACS_REALLOC(a,b,t)   emacs_realloc(a,b,t,THIS_FILE,__LINE__)
# else
#  define    EMACS_MALLOC(a,t)      emacs_malloc(a,t)
#  define    EMACS_FREE(a)          emacs_free(a)

#  define    EMACS_REALLOC(a,b,t)   emacs_realloc(a,b,t)
# endif

#define realloc_ustr(a,b) ((unsigned char *)EMACS_REALLOC(a,b,malloc_type_char))
#define malloc_struct( s ) ((struct s *)EMACS_MALLOC( sizeof( struct s ), malloc_type_struct_##s ))
#define malloc_ustr( i ) ((unsigned char *)EMACS_MALLOC( i, malloc_type_char ))

#ifdef __cplusplus
# define PNOTUSED(arg) // arg
# ifdef min
#  undef min
# endif
inline int min( int a, int b )
{
    return a < b ? a : b;
}

# ifdef max
#  undef max
# endif
inline int max( int a, int b )
{
    return a > b ? a : b;
}
#else
# define PNOTUSED(arg) arg
# ifndef min
#  define min(a,b) ((a) < (b) ? (a) : (b))
# endif

# ifndef max
#  define max(a,b) ((a) > (b) ? (a) : (b))
# endif

#endif
//
//    Use this class's constructor to
//    initialise any subsystem required for
//    the correct construction of other global objects.
//
//
class EmacsInitialisation
{
public:
    EmacsInitialisation( const char *date_and_time, const char *file_name );
    virtual ~EmacsInitialisation();

    // hook for operating system specific init
    // in the very earliest stage of process startup
    static void os_specific_init();

    static void setup_version_string();

    static bool init_done;

    static time_t most_recent_built_module;
};


#endif // __EMACSUTL_H__
