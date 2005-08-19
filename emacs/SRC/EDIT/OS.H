/*
 *	os.h
 *
	This module contains all the macros, definitions, include, etc
	that it takes to make the main code OS independent.

	OS is chosen on the following symbol

	vms		- VAX/VMS
	unix		- any old unix
	macintosh	- the Apple Macintosh
	_MSDOS		- MS DOS
	windows		- MS Windows

	Features are conditional on the following symbols

	DB		- database management
	MEMMAP		- directly write the display hardware
	REAL_TTY	- there is a real terminal as I/O device
	MOUSE		- has a mouse device
	INODE		- file system has unix like inodes
	OneEmacsPerTty	- Only one Emacs for each terminal
	subprocesses	- Support subprocesses
	ALIGN_ACCESS	- True if the hardware only supports aligned memory
	USER_ID		- A system with usernames and the like
	NETWORK_ID	- A system with a network address and name
	XTERM		- X interface
 *
 */
#define EMACS_VERSION	"X6.1.27"

#ifndef RC_INVOKED
/********************** Include files **********************/
#ifdef MSDOS
#ifndef _MSDOS
#define _MSDOS
#endif
#endif


# ifndef OS_NO_INCLUDES

#  include	<stdio.h>
#  include	<string.h>
#  include	<stdlib.h>
#  include	<stddef.h>
#  include	<time.h>
#  include	<stdarg.h>
#  include	<ctype.h>
#  include	<errno.h>
#  include	<limits.h>
#  include	<assert.h>
#  if defined( _MSDOS ) || defined ( _NT )
#   include	<direct.h>
#   include	<io.h>
#   include	<dos.h>
#   include	<fcntl.h>
#   include	<sys/types.h>
#   include	<sys/stat.h>

#ifndef __WATCOMC__
#define access	   _access
#define chmod	   _chmod
#define chsize	   _chsize
#define close	   _close
#define creat	   _creat
#define dup	   _dup
#define dup2	   _dup2
#define eof	   _eof
#define filelength _filelength
#define isatty	   _isatty
#define locking    _locking
#define lseek	   _lseek
#define mktemp	   _mktemp
#define open	   _open
#define read	   _read
#define setmode    _setmode
#define sopen	   _sopen
#define tell	   _tell
#define umask	   _umask
#define unlink	   _unlink
#define write	   _write
#endif

#  endif
#  ifdef macintosh
#   include	<fcntl.h>
#   define O_TEXT	0
#  endif
# endif

#if defined( __unix ) || defined( __unix__ )
# if !defined( unix )
#  define unix
# endif
#endif

# if defined( vms )
#  include <os_vms.h>
# elif defined( _WINDOWS ) || defined( _NT )
#   include <os_win.h>
# elif defined( _MSDOS )
#   include <os_msdos.h>
# elif defined( macintosh )
#  include <os_mac.h>
# elif defined( unix )
#  include <os_unix.h>
# else
#  error need support for this platform
# endif

/* if this is NT then get rid of the _MSDOS symbol */
# ifdef _NT
#  ifdef _MSDOS
#   undef _MSDOS
#  endif
#  ifdef _MSDOS
#   undef _MSDOS
#  endif
# endif

# ifndef align_int
#  define align_int (sizeof( long int )-1)
# endif
#endif

#ifndef _WINDOWS
#ifndef ALIGN
#define ALIGN
#endif
#ifndef FAR
#define FAR
#endif
#ifndef NEAR
#define NEAR
#endif
#ifndef HUGE_PTR
#define HUGE_PTR
#endif
#ifndef CDECL
#define CDECL
#endif          
#endif

#ifndef TEXT_MODE
#define TEXT_MODE
#endif
#ifndef BINARY_MODE
#define BINARY_MODE
#endif
#ifndef COMMIT_MODE
#define COMMIT_MODE
#endif
#ifndef ALL_FILES
#define ALL_FILES "*.*"
#endif

#ifdef __cplusplus
extern "C" {
#endif
void debug( int row, unsigned char *fmt, ... );
void debug_bpt(void);

int do_print( unsigned char *, va_list *, unsigned char *, int );

#ifdef __cplusplus
}
#endif
