//
//    os_pybemacs.h
//
#include    <CXX/WrapPython.h>
#include    <CXX/Objects.hxx>

#define DB 1

#if _MSC_VER == 1200
#pragma warning( disable: 4710 )
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
#define DBG_FILE            (0x00800000&DBG___ON)   // EmacsFile debug
#define DBG_VERBOSE         (0x20000000&DBG___ON)   // for more of one of the other types of debug info
#define DBG_TMP             (0x40000000&DBG___ON)   // for temporary debug situations

#define MEMMAP      1
#define INODE       0
#define MOUSE       1

const char PATH_CH( '\\' );
#define PATH_STR "\\"
const char PATH_ALT_CH( '/' );
#define PATH_ALT_STR "/"
const char PATH_SEP( ';' );

#if defined(_NT)
const int MAXNAMLEN( _MAX_PATH );
const int MAXPATHLEN( _MAX_PATH );
#else
const int MAXNAMLEN( 8+1+3 );
const int MAXPATHLEN( _MAX_PATH );
#endif

const int ROOT_CHAR_POS( 2 );    // root is "x:\\"

#define BACKUP_EXTENSION ".bak"
#define CHECKPOINT_EXTENSION ".ckp"

#define EMACS_INIT_PROFILE  EmacsString("emacs_profile.ml")
#define MEMORY_FILE_STR     get_config_env("emacs_memory")
const int MEMORY_FILE_ARG( 0 );
#define EMACS_PATH          get_config_env("emacs_path")
extern EmacsString get_tmp_path(void);
#define EMACS_TMP_PATH      get_tmp_path()
#ifdef MLL_DB
#define EMACS_DB_DEFAULT    ".mll"
#else
#define EMACS_DB_DEFAULT    ".db"
#endif

#define TEXT_MODE "t"
#define BINARY_MODE "b"
#define COMMIT_MODE "c"

#define    ALL_FILES "*"

#define EXPORT_SYMBOL __declspec( dllexport )

//
//    typedefs for fundemental types
//
typedef Py_UCS4 EmacsChar_t;    // Py_UNICODE is 2 bytes on windows so use Py_UCS4

# include <io.h>
# include <share.h>
# define SHARE_READ , _SH_DENYWR
# define SHARE_NONE , _SH_DENYRW
// # define fopen _fsopen
