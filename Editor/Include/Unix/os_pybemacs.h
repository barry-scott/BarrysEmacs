//
//    os_pybemacs.h
//
#include    <CXX/WrapPython.h>
#include    <CXX/Objects.hxx>
#include    <sys/param.h>
#include    <dirent.h>

#define DB 1

//#define _POSIX_SOURCE 1        // we are a posix app

#ifdef __GNUC__
// GNU C++ has array new BUT we have to avoid it as the code generator crashes in related code.
#define __has_array_new__ 1
#include <typeinfo>
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
#define DBG_EXT_PARSER      (0x00100000&DBG__OFF)   // Extended search parser
#define DBG_EXT_SEARCH      (0x00200000&DBG__OFF)   // Extended search algorithm
#define DBG_OBJ_LOCK        (0x00400000&DBG__OFF)   // add in lockObject() code for debugging object lifetime problems
#define DBG_VERBOSE         (0x20000000&DBG___ON)   // for more of one of the other types of debug info
#define DBG_TMP             (0x40000000&DBG___ON)   // for temporary debug situations

#define MEMMAP              1
#define INODE               1

#define PATH_CH             '/'
#define PATH_STR            "/"
#define PATH_SEP            ' '
#define ALL_FILES           "*"

#define ROOT_CHAR_POS       0    // root is "/"

#define BACKUP_EXTENSION    ".bak"
#define CHECKPOINT_EXTENSION ".ckp"

#define EMACS_INIT_PROFILE  u_str("emacs_profile.ml")
#define MEMORY_FILE_STR     u_str("emacs_memory:.emacs_mem")
#define MEMORY_FILE_ARG     0
#define EMACS_PATH          get_config_env("emacs_path")
#define EMACS_TMP_PATH      u_str("/tmp")
#define EMACS_DB_DEFAULT    u_str("")

#define EXPORT_SYMBOL

//
//    typedefs for fundemental types
//
typedef Py_UNICODE EmacsChar_t;

#define SHARE_READ
#define SHARE_NONE
