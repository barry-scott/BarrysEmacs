/*
 *	os_unix.h
 */
#include	<sys/param.h>
#include	<dirent.h>

#define HELPGOESAWAY		/* True if you want help to be allowed to go away */
#define DB 1
#define XWINDOWS 1
#define SUBPROCESSES 1
#define SAVE_ENVIRONMENT

#define OS_NAME "unix"
#if defined( __osf__ )
# define OS_VERSION "osf1"
# define CPU_TYPE "AXP"
#elif defined( __FreeBSD__ )
# define OS_VERSION "FreeBSD"
# define CPU_TYPE "i386"
# define EMACS_USE_TERMIOS 1
#elif defined( __hpux )
# define OS_VERSION "hpux"
# if defined( __hppa )
#  define CPU_TYPE "pa_risc"
# else
#  define CPU_TYPE "m68k"
# endif
# define EMACS_USE_TERMIOS 1
/*# define KEYBOARD_TYPE_OLD_HP 1*/
# define KEYBOARD_TYPE_NEW_HP 1
extern int sys_nerr;
extern char *sys_errlist[];
# define getws getcwd
#elif defined( __linux )
# define OS_VERSION "linux"
# define CPU_TYPE "i386"
# define EMACS_USE_TERMIOS 1
extern int sys_nerr;
extern char *sys_errlist[];
#else
# define OS_VERSION "ultrix"
# define CPU_TYPE "MIPS"
#endif

#if defined( __FreeBSD__ ) || defined( __linux )
# define KEYBOARD_TYPE_PC
#endif

#define O_BINARY 0
#define REAL_TTY 1

#define get_env( var ) u_str( getenv( s_str( var ) ) )
#define MAXFDS 64

#ifndef MAXNAMLEN
# define MAXNAMLEN MAXPATHLEN
#endif

#if defined( __FreeBSD__ )
# define KEYBOARD_TYPE_PC
#endif

#define O_BINARY 0
#define REAL_TTY 1

#define get_env( var ) u_str( getenv( s_str( var ) ) )
#define MAXFDS 64

#ifndef MAXNAMLEN
# define MAXNAMLEN MAXPATHLEN
#endif

/* debug flag bits */
#define DBG_DUMPTOFILE		(0x0001&0)
#define DBG_ALLOC_CHECK		(0x0002&0)
#define DBG_EXEC		(0x0004&0)
#define DBG_KEY			(0x0008&0)
#define DBG_DISPLAY		(0x0010&0)
#define DBG_QUEUE		(0x0020&0)
#define DBG_BUFFER		(0x0040&0)
#define DBG_JOURNAL		(0x0080&0)
#define DBG_PROFILE		(0x0100&0)
#define DBG_VECTOR		(0x0200&0)
#define	DBG_CALC_M_PRINT	(0x0400&0)
#define DBG_TIME		(0x0800&0)

#define MEMMAP 0
#define INODE	1

#define PATH_CH	'/'
#define PATH_STR "/"
#define PATH_SEP ' '
#define ALL_FILES "*"

#define ROOT_CHAR_POS	0	/* root is "/" */

#define NETWORK_ID
extern unsigned char SysnameHacking[20];
#define SystemName ((unsigned char *) sprintf(SysnameHacking, "%s", sysname ()))

#define BACKUP_EXTENSION ".bak"
#define CHECKPOINT_EXTENSION ".ckp"

#define EMACS_INIT_PROFILE	u_str("emacs_profile.ml")
#define MEMORY_FILE_STR		u_str("emacs_memory:.emacs_mem")
#define MEMORY_FILE_ARG		0
#define EMACS_PATH		u_str(get_config_env(u_str ("emacs_path")))
#define	EMACS_TMP_PATH		u_str("/tmp")
#define	EMACS_DB_DEFAULT	u_str("")

#ifndef XWINDOWS
struct process_blk;
typedef unsigned long XtInputId;
typedef void (*XtInputCallbackProc) (struct process_blk *, int *);
# define XtInputReadMask 1
# define XtInputWriteMask 2
#endif

#if defined( __hpux ) && defined( __cplusplus )
#define volatile		/* not supported in the HP C++ compiler */
#endif
