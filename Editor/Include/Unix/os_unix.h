//
//    os_unix.h
//
#include    <sys/param.h>
#include    <dirent.h>

#define DB 1
#define XWINDOWS 1
#define SUBPROCESSES 1
#define SAVE_ENVIRONMENT

//#define _POSIX_SOURCE 1        // we are a posix app

#define KEYBOARD_TYPE_DEFAULT 0
#define KEYBOARD_TYPE_PC 1
#define KEYBOARD_TYPE_HP 2
#define KEYBOARD_TYPE_OLD_HP 3

#define    TOOLBAR_TOGGLE_BUTTONS    1    // defined if this unix supports tool bar toggle buttons
#define OS_NAME "unix"
#if defined( __osf__ )
# define OS_VERSION "osf1"
# define CPU_TYPE "AXP"

#elif defined( __FreeBSD__ )
# define OS_VERSION "FreeBSD"
# define CPU_TYPE "i386"
# define KEYBOARD_TYPE KEYBOARD_TYPE_PC

#elif defined( __OpenBSD__ )
# define OS_VERSION "OpenBSD"
# define CPU_TYPE "i386"
# define KEYBOARD_TYPE KEYBOARD_TYPE_PC

#elif defined( __APPLE_CC__ )
# define OS_VERSION "Mac OS X"
# define CPU_TYPE "i386"
# define KEYBOARD_TYPE KEYBOARD_TYPE_PC

# if defined( __WXMAC__ )
#  undef SUBPROCESSES
#  undef SAVE_ENVIRONMENT
#  undef XWINDOWS
# endif

#elif defined( __linux__ )
# define OS_VERSION "linux"
# define CPU_TYPE "i386"
# define KEYBOARD_TYPE KEYBOARD_TYPE_PC

#elif defined( __hpux )
# define OS_VERSION "hpux"
# if defined( __hppa )
#  define CPU_TYPE "pa_risc"
#  define KEYBOARD_TYPE KEYBOARD_TYPE_HP
# else
#  define CPU_TYPE "m68k"
#  define KEYBOARD_TYPE KEYBOARD_TYPE_OLD_HP
# endif
//# undef    TOOLBAR_TOGGLE_BUTTONS    // not working on HP-UX
# define getws getcwd

#elif defined( _AIX )
# define OS_VERSION "aix"
# define CPU_TYPE "ppc"
# define KEYBOARD_TYPE KEYBOARD_TYPE_HP
//#define getws getcwd

#else
#error Which unix is this?
#endif

#ifdef __GNUC__
#define __has_bool__ 1
// GNU C++ has array new BUT we have to avoid it as the code generator crashes in related code.
#define __has_array_new__ 1
#include <typeinfo>
#endif


#if !defined(KEYBOARD_TYPE)
#define KEYBOARD_TYPE KEYBOARD_TYPE_DEFAULT
#endif

# define O_BINARY 0
# define REAL_TTY 1

# define MAXFDS 64

#ifndef MAXNAMLEN
# define MAXNAMLEN MAXPATHLEN
#endif

// debug flag bits
#define DBG__OFF            0x00000000
#define DBG___ON            0xffffffff

#define DBG_DUMPTOFILE      (0x00000001&DBG___ON)
#define DBG_ALLOC_CHECK     (0x00000002&DBG__OFF)
#define DBG_EXEC            (0x00000004&DBG___ON)
#define DBG_KEY             (0x00000008&DBG___ON)
#define DBG_DISPLAY         (0x00000010&DBG___ON)
#define DBG_QUEUE           (0x00000020&DBG__OFF)
#define DBG_BUFFER          (0x00000040&DBG__OFF)
#define DBG_PROFILE         (0x00000100&DBG___ON)
#define DBG_VECTOR          (0x00000200&DBG__OFF)
#define DBG_CALC_INS_DEL    (0x00000400&DBG___ON)
#define DBG_EXECFILE        (0x00000800&DBG___ON)
const int DBG_NO_DBG_MSG    (0x00001000&DBG___ON);  // always defined
#define DBG_SYNTAX          (0x00002000&DBG__OFF)
#define DBG_EMSTRING        (0x00004000&DBG___ON)
#define DBG_SER             (0x00008000&DBG__OFF)   // debug system expression representation
#define DBG_ML_PARSE        (0x00010000&DBG__OFF)   // ML parsing debug messages
#define DBG_PROCESS         (0x00020000&DBG___ON)   // process handling code
#define DBG_TIMER           (0x00040000&DBG___ON)   // timer code
#define DBG_ML_ERROR        (0x00080000&DBG___ON)   // MLisp errors
#define DBG_EXT_SEARCH      (0x00100000&DBG__OFF)   // Extended search
#define DBG_SYSLOG          (0x10000000&DBG___ON)   // redirect dbg_msg to syslog
#define DBG_VERBOSE         (0x20000000&DBG___ON)   // for more of one of the other types of debug info
#define DBG_TMP             (0x40000000&DBG___ON)   // for temporary debug situations

# define MEMMAP 1
# define INODE    1

# define PATH_CH    '/'
# define PATH_STR "/"
# define PATH_SEP ' '
# define ALL_FILES "*"

# define ROOT_CHAR_POS    0    // root is "/"

#define BACKUP_EXTENSION ".bak"
#define CHECKPOINT_EXTENSION ".ckp"

#define EMACS_INIT_PROFILE    u_str("emacs_profile.ml")
#define MEMORY_FILE_STR        u_str("emacs_memory:.emacs_mem")
#define MEMORY_FILE_ARG        0
#define EMACS_PATH        get_config_env("emacs_path")
#define    EMACS_TMP_PATH        u_str("/tmp")
#define    EMACS_DB_DEFAULT    u_str("")

#ifndef XWINDOWS
class EmacsProcess;
typedef unsigned long XtInputId;
typedef EmacsProcess *XtPointer;
typedef void (*XtInputCallbackProc) (XtPointer, int *, XtInputId * );
#define XtInputReadMask 1
#define XtInputWriteMask 2
#endif

extern void OutputDebugString( const char *message );
#ifdef XWINDOWS
extern int is_motif;
#endif
