//
//    os_mac.h
//
#define BROADCAST 0
#define CHECK_POINTER_SUPPORT   0
// #define SUBPROCESSES 1
#define DB 1

#define OS_NAME "Macintosh"

extern unsigned char *get_config_env( unsigned char * );

// debug flag bits
#define DBG_DUMPTOFILE    0x0001
#define DBG_ALLOC_CHECK    0x0002
#define DBG_EXEC    0x0004
#define DBG_KEY        0x0008
#define DBG_DISPLAY    0x0010
#define DBG_QUEUE    0x0020
#define DBG_BUFFER    0x0040
#if !defined( _NT )
# define DBG_PROFILE    0x0100
#else
# define DBG_PROFILE    0
#endif
#define DBG_VECTOR    (0x0200&0)
#define DBG_CALC_M_PRINT 0x0400

# define MEMMAP 1
# define INODE    0
#define MOUSE 1

struct _stat
{
    time_t st_mtime;
    long int st_size;
    int st_mode;
#define _S_IFDIR 0x10
#define _S_IWRITE 0x0
    int st_dev;
};
extern int _stat( char *filename, struct _stat *buffer );
extern int _fstat( int fileno, struct _stat *buffer );

# define PATH_CH ':'
# define PATH_STR ":"
# define PATH_SEP '#'

# define MAXNAMLEN      (31)
# define MAXPATHLEN     (255)
# define ROOT_CHAR_POS  0       // root is "x:\\"

#define BACKUP_EXTENSION        ".BAK"
#define CHECKPOINT_EXTENSION    ".CKP"

#define EMACS_INIT_PROFILE      u_str("emacs_profile.ml")
#define MEMORY_FILE_STR         get_config_env(u_str("Emacs Memory"))
#define MEMORY_FILE_ARG         0
#define EMACS_PATH              get_config_env(u_str("emacs_path"))
extern unsigned char *get_tmp_path(void);
#define    EMACS_TMP_PATH       get_tmp_path()
#define    EMACS_DB_DEFAULT     u_str(".mll")

#define TEXT_MODE
#define BINARY_MODE
#define COMMIT_MODE
