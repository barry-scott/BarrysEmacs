//
//    os_win.h
//

// control the warning messages of the compiler
#define BROADCAST 0
#define CHECK_POINTER_SUPPORT   0
#define SUBPROCESSES 1
#define DB 1
#define CALL_BACK 1
#define SAVE_ENVIRONMENT

// only turn on Python if its a MSVC60 build
//#if _MSC_VER == 1200
#define EMACS_PYTHON_EXTENSION 1        // compile in python extension support
#define EMACS_PYTHON_EXTENSION_THREADED 0    // Using the threaded version if true
//#endif
#define OS_NAME "Windows"

#if _MSC_VER == 1200
#pragma warning( disable: 4710 )
#endif

// debug flag bits
#define DBG_DUMPTOFILE        (0x00000001)
#define DBG_ALLOC_CHECK        (0x00000002&0)
#define DBG_EXEC        (0x00000004)
#define DBG_KEY            (0x00000008)
#define DBG_DISPLAY        (0x00000010)
#define DBG_QUEUE        (0x00000020&0)
#define DBG_BUFFER        (0x00000040&0)
#if !defined( _NT )
# define DBG_PROFILE        (0x00000100)
#else
# define DBG_PROFILE        (0)
#endif
#define DBG_VECTOR        (0x00000200&0)
#define    DBG_CALC_INS_DEL    (0x00000400)
#define    DBG_EXECFILE        (0x00000800)
const int DBG_NO_DBG_MSG    (0x00001000);    // always defined
#define    DBG_SYNTAX        (0x00002000&0)
#define    DBG_EMSTRING        (0x00004000&0)
#define    DBG_SER            (0x00008000&0)    // debug system expression representation
#define    DBG_ML_PARSE        (0x00010000&0)    // ML parsing debug messages
#define    DBG_PROCESS        (0x00020000)    // process handling code
#define    DBG_TIMER        (0x00040000)    // timer code
#define    DBG_ML_ERROR        (0x00080000)    // MLisp errors
#define    DBG_VERBOSE        (0x20000000)    // for more of one of the other types of debug info
#define    DBG_TMP            (0x40000000)    // for temporary debug situations
#define MEMMAP    1
#define INODE    0
#define MOUSE    1

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

#define EMACS_INIT_PROFILE    EmacsString("emacs_profile.ml")
#define MEMORY_FILE_STR        get_config_env("emacs_memory")
const int MEMORY_FILE_ARG( 0 );
#define EMACS_PATH        get_config_env("emacs_path")
extern unsigned char *get_tmp_path(void);
#define    EMACS_TMP_PATH        get_tmp_path()
#ifdef MLL_DB
#define    EMACS_DB_DEFAULT    ".mll"
#else
#define    EMACS_DB_DEFAULT    ""
#endif

#define TEXT_MODE "t"
#define BINARY_MODE "b"
#define COMMIT_MODE "c"

#define    ALL_FILES "*"
