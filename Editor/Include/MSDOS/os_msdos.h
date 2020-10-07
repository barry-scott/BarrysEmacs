//
//      os_msdos.h
//
#define BROADCAST 0
#define CHECK_POINTER_SUPPORT   0
// #define SUBPROCESSES 1
#define DB 1
#define SAVE_ENVIRONMENT

#  define OS_NAME "MS-DOS"

#  define INT long int
#  ifndef FAR
#   define FAR _far
#   define NEAR _near
#  endif
#  define HUGE_PTR _huge
#  define USE_HUGE_ALLOC 1
#  define CDECL _cdecl
#  define PASCAL _pascal
#  define SIGNAL_PROTOTYPE _far _cdecl

extern unsigned char *get_config_env( unsigned char *entry );

# define align_int (sizeof (int) - 1)

// debug flag bits
#define DBG_DUMPTOFILE  0x0001
#define DBG_ALLOC_CHECK 0x0002
#define DBG_EXEC        0x0004
#define DBG_KEY         0x0008
#define DBG_DISPLAY     0x0010
#define DBG_QUEUE       0x0020
#define DBG_BUFFER      0x0040
#define DBG_JOURNAL     0x0080
#if !defined( _NT )
# define DBG_PROFILE    0x0100
#else
# define DBG_PROFILE    0
#endif
#define DBG_VECTOR      (0x0200&0)
#define DBG_CALC_M_PRINT 0x0400

# define MEMMAP 1
# define INODE  0
# define MOUSE 1

# define PATH_CH '\\'
# define PATH_STR "\\"
# define PATH_SEP ';'

# define MAXNAMLEN      (8+1+3)
# define MAXPATHLEN     63
# define ROOT_CHAR_POS  2       // root is "x:\\"

# define checkpoint_extension ".CKP"

#define EMACS_INIT_PROFILE      u_str("emacs_profile.ml")
#define MEMORY_FILE_STR         get_config_env(u_str("emacs_memory"))
#define MEMORY_FILE_ARG         0
#define EMACS_PATH              get_config_env(u_str("emacs_path"))
extern unsigned char *get_tmp_path(void);
#define EMACS_TMP_PATH          get_tmp_path()
#ifdef MLL_DB
#define    EMACS_DB_DEFAULT    u_str(".mll")
#else
#define    EMACS_DB_DEFAULT    u_str("")
#endif
#define TEXT_MODE "t"
#define BINARY_MODE "b"
#define COMMIT_MODE "c"

#include <bios.h>
extern void start_dos_timer( int );
extern void stop_dos_timer( int );
