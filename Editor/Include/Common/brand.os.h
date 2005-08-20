//
//	os.h
//
//	This module contains all the macros, definitions, include, etc
//	that it takes to make the main code OS independent.
//
//	OS is chosen on the following symbol
//
//	vms		- VAX/VMS
//	unix		- any old unix
//	macintosh	- the Apple Macintosh
//	_MSDOS		- MS DOS
//	windows		- MS Windows
//
//	Features are conditional on the following symbols
//
//	DB		- database management
//	MEMMAP		- directly write the display hardware
//	REAL_TTY	- there is a real terminal as I/O device
//	MOUSE		- has a mouse device
//	INODE		- file system has unix like inodes
//	subprocesses	- Support subprocesses
//	ALIGN_ACCESS	- True if the hardware only supports aligned memory
//	USER_ID		- A system with usernames and the like
//	NETWORK_ID	- A system with a network address and name
//	XTERM		- X interface
//
//
#define EMACS_VERSION	"%(maturity)s%(major)s.%(minor)s-%(revision)s%(wc_state)s"

#ifndef RC_INVOKED
//********************* Include files *********************

# ifndef OS_NO_INCLUDES
#endif

#ifndef RC_INVOKED
#  include	<stdio.h>
#  include	<string>
#  include	<stdlib.h>
#  include	<stddef.h>
#  include	<time.h>
#  include	<stdarg.h>
#  include	<ctype.h>
#  include	<errno.h>
#  include	<limits.h>


#  ifdef macintosh
#   include	<fcntl.h>
#   define O_TEXT	0
#  endif
# endif

# if defined( __unix ) || defined( __unix__ ) || defined( _AIX ) || defined( __APPLE_CC__ )
#  if !defined( __unix__ )
#   define __unix__
#  endif
# endif

# if defined( vms )
#  include <os_vms.h>

#  elif defined( WIN32 )
#  include <os_win.h>

#  elif defined( _MSDOS )
#  include <os_msdos.h>

#  elif defined( macintosh )
#  include <os_mac.h>

#  elif defined( __unix__ )
#  include <os_unix.h>

# else
#  error need support for this platform
# endif

// if this is NT then get rid of the _MSDOS symbol
# ifdef _NT
#  ifdef _MSDOS
#   undef _MSDOS
#  endif
# endif

# ifndef align_int
#  define align_int (sizeof( long int )-1)
# endif
#endif

#ifndef TEXT_MODE
# define TEXT_MODE
#endif
#ifndef BINARY_MODE
# define BINARY_MODE
#endif
#ifndef COMMIT_MODE
# define COMMIT_MODE
#endif
#ifndef ALL_FILES
# define ALL_FILES "*.*"
#endif

extern void debug( int row, const EmacsString &text );
void debug_bpt(void);

#ifdef _DEBUG
// assert macro based on <assert.h> assert 
extern void _emacs_assert(const char *, const char *, unsigned);
#define emacs_assert(exp) (void)( (exp) || (_emacs_assert(#exp, __FILE__, __LINE__), 0) )
#else
#define emacs_assert(exp) ((void)0)
#endif

#ifndef __has_bool__
enum { false=0, true=1 };
typedef int bool;
#endif
