//
//    os_vms.h
//        vms specific definitions
//
#define DB 1
#define XWINDOWS 1

#define SUBPROCESSES 1
#define BROADCAST 1
#define CALL_BACK 1
#define SAVE_ENVIRONMENT 1

#ifndef OS_NO_INCLUDES
# ifdef vms
#  if defined( load_library_lib )
#   include <target_library:lib.h>
#  endif
#  include <target_library:ssdef.h>
# endif
#endif

#define OS_NAME "VMS"
#include    <unixio.h>
#include    <unixlib.h>
#include    <file.h>
#include    <descrip.h>
#include <descrip.h>
#include <fab.h>
#include <nam.h>
#include <chfdef.h>

#define VMS_SUCCESS( status ) ((status&1) != 0)
#define O_BINARY    0
#define O_TEXT        0
#define unlink(file)    delete(file)
#define REAL_TTY    1
#define GetEnv( var ) u_str( getenv( s_str( var ) ) )
#define GetConfigEnv( var ) u_str( getenv( s_str( var ) ) )
#define INT long int
#define USE_HUGE_ALLOC 0
#define SIGNAL_PROTOTYPE
#define InputCheck    0
#define align_int (sizeof (int) - 1)
#define ALIGN _align(LONGWORD)

// debug flag bits
#define DBG_DUMPTOFILE    0x0001
#define DBG_ALLOC_CHECK    0x0002
#define DBG_EXEC    0x0004
#define DBG_KEY        0x0008
#define DBG_DISPLAY    0x0010
#define DBG_QUEUE    0x0020
#if !defined( _NT )
# define DBG_PROFILE    0x0100
#else
# define DBG_PROFILE    0
#endif
#define DBG_VECTOR    (0x0200&0)
#define    DBG_CALC_M_PRINT 0x0300

#define MEMMAP 0
#define INODE    0

#define PATH_CH    ']'
#define PATH_STR "]"
#define PATH_SEP '#'
#define VERS_CH ';'
#define VERS_STR ";"

#define MAXNAMLEN    (39+1+39+1+5)
#define MAXPATHLEN    255
#define ROOT_CHAR_POS    0    // root is "/"

#define BACKUP_EXTENSION ".BAK"
#define CHECKPOINT_EXTENSION ".CKP"

#define EMACS_INIT_PROFILE    u_str("emacs_library:emacs_profile.ml")
#define MEMORY_FILE_STR        u_str("emacs_memory:emacs.memory")
#define MEMORY_FILE_ARG        0
#define EMACS_PATH        u_str("emacs_path:")
#define    EMACS_TMP_PATH        u_str("sys$scratch:")
#define EMACS_DB_DEFAULT    u_str("SYS$DISK:[].TLB")

struct fscn_def {
    unsigned short int w_length;    // return length word
    unsigned short int w_item_code;    // item code value
    unsigned char *a_addr;        // return length pointer
} ;

struct vms_itm_3 {
    unsigned short int w_length;
    unsigned short int w_item_code;
    unsigned char *a_buf_addr;
    unsigned int *a_ret_len;
};

#define EFN_DO_NOT_WAIT_ON 0            // used when an EFN is require that is not waited on
#define EFN_SIT_FOR 1                   // used for sit-for
#define EFN_RESTART_MBX 2               // used for reading the restart mailbox message
#define EFN_TERM_IO 3                   // used for terminal IO
#define EFN_SUB_PROC_SEND 4             // used to send messages to subprocesses
#define EFN_SUB_PROC_REC 5              // used to receive a message from a subprocess
#define EFN_GETJPI 6                    // used to do a getjpi and wait
#define EFN_ACTIVITY_IO 7               // used to do activity indicator IO
#define EFN_EMACS_PAUSED 96             // event flag 96 = EMACS has paused
#define EFN_EMACS_EXITED 97             // event flag 97 = EMACS has exited
#define EFN_EMACS_RESUME 98             // event flag 98 = resume EMACS

#define cuserid decc$cuserid
#define DEFAULT_MAIL_BOX_PROTECTION 0xff0f

#define DSC_SZ( dsc, sz ) \
    dsc.dsc$w_length = _str_len( sz ); \
    dsc.dsc$b_dtype = 0; \
    dsc.dsc$b_class = 0; \
    dsc.dsc$a_pointer = s_str(sz)

extern int sys$setast( unsigned char enb );
extern int sys$setimr( unsigned int efn, unsigned int *daytim,
    void (*astadr)(unsigned int), unsigned int reqidt, unsigned int flags );
extern int sys$cantim( unsigned int reqidt, unsigned int acmode );
extern int sys$crembx( int prmflg, unsigned short *chan, int maxmsg, int bufquo, int promsk, int acmode, struct dsc$descriptor *lognam );
extern int sys$crelnm( unsigned int *attr, struct dsc$descriptor *tabnam,
     struct dsc$descriptor *lognam, unsigned char *acmode, struct vms_itm_3 *itmlst );
extern int sys$dclexh( void * );
extern int sys$canexh( void * );
extern int sys$delprc( unsigned int *, struct dsc$descriptor * );
extern int sys$unwind( long int *, void (*)(void) );
extern int sys$resume( unsigned int *, struct dsc$descriptor * );
extern int sys$suspnd( unsigned int *, struct dsc$descriptor *, int );
extern int sys$forcex( unsigned int *, struct dsc$descriptor *, int );
extern int sys$getmsg( unsigned int msgid, unsigned short *msglen, struct dsc$descriptor *bufadr,
        unsigned int flags, unsigned char *outadr );
extern int sys$trnlog
    (
    struct dsc$descriptor *lognam,
    unsigned short *rsllen, struct dsc$descriptor *rslbuf,
    char *table,
    char *acmode,
    int *dsbmsk
    );
extern int sys$parse( FileParse *, void (*)(FileParse *), void (*)(FileParse *) );
extern int sys$search( FileParse * );
extern lib$establish( int (*)( struct chf$signal_array *sig, struct chf$mech_array *mech ) );
extern int lib$set_logical( struct dsc$descriptor *, struct dsc$descriptor *);
extern int lib$getdvi( int *code, unsigned short *chan, struct dsc$descriptor *name,
    unsigned int *value, struct dsc$descriptor *resultstr, unsigned short *resultlen );
extern int lib$getjpi( int *code, unsigned int *pid, struct dsc$descriptor *name,
    unsigned int *value, struct dsc$descriptor *resultstr, unsigned short *resultlen );
extern int lib$attach( unsigned int *pid );
extern int sys$hiber(void);
extern int sys$wake( unsigned int *pid, struct dsc$descriptor *prcnam );
extern int lib$spawn
    (
    struct dsc$descriptor *cmd,
    struct dsc$descriptor *in,
    struct dsc$descriptor *out,
    int *flags,
    struct dsc$descriptor *name,
    unsigned int *id,
    int *status,
    unsigned char *efn,
    void (*astadr)(void *),
    void *astprm,
    struct dsc$descriptor *,
    struct dsc$descriptor *
    );

extern int sys$filescan( struct dsc$descriptor *srcstr, struct fscn_def *items, int *fflags );

extern int sys$assign( struct dsc$descriptor *devnam, unsigned short *chan, unsigned int acmode, struct dsc$descriptor *mbxnam );
extern int sys$dassgn( unsigned short chan );

extern int sys$qiow
    (
    int efn,
    short chan,
    int func,
    void *iosb,
    void (*astrtn)(void *),
    void *astprm,
    ...    // p1 ... p6
    );

extern int sys$qio
    (
    int efn,
    short chan,
    int func,
    void *iosb,
    void (*astrtn)(void *),
    void *astprm,
    ...    // p1 ... p6
    );

extern int lib$delete_logical( struct dsc$descriptor * );
extern int lib$show_timer(int, int, int (*)( struct dsc$descriptor *str ));
extern int lib$init_timer(void);
extern int lib$put_output( struct dsc$descriptor * );
extern int _spawn
    (
    char * sz_cmd,
    char * sz_in,
    char * sz_out,
    int flags,
    char *sz_name,
    unsigned int *id,
    int *status,
    unsigned char efn,
    void (*astadr)(void *), void *astprm,
    int reassign
    );
extern char *clisetstr( void );
#if BROADCAST
void reassign_mbx(void);
int deassign_mbx(void);
#endif
