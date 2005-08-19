/* convert a pathname to an absolute one, if it is absolute already,
   it is returned in the buffer unchanged, otherwise leading "./"s
   will be removed, the name of the current working directory will be
   prepended, and "../"s will be resolved.

   In a moment of weakness, I have implemented the cshell ~ filename
   convention.	~/foobar will have the ~ replaced by the home directory of
   the current user.  ~user/foobar will have the ~user replaced by the
   home directory of the named user.  This should really be in the kernel
   (or be replaced by a better kernel mechanism).  Doing file name
   expansion like this in a user-level program leads to some very
   distasteful non-uniformities.

   Another fit of dementia has led me to implement the expansion of shell
   environment variables.  $HOME/mbox is the same as ~/mbox.  If the
   environment variable a = "foo" and b = "bar" then:
	$a	=>	foo
	$a$b	=>	foobar
	$a.c	=>	foo.c
	xxx$a	=>	xxxfoo
	${a}!	=>	foo!

 */

#ifdef _NT
#include <win_incl.h>
#endif

#ifdef unix
#include <unistd.h>
#include <pwd.h>
#endif

#include <emacs.h>

/*forward*/ int chdir_and_set_global_record( unsigned char *buf );
/*forward*/ unsigned char *save_abs( unsigned char *fn );
/*forward*/ int expand_and_default( unsigned char *fn, unsigned char *dn, unsigned char *ou );
/*forward*/ int cur_dir( void );
/*forward*/ void init_abs( void );

unsigned char null_path[1];

#ifdef macintosh
int chdir( char *path )
	{
	return 0;
	}
#endif

/*
 * input name in nm, absolute pathname output to buf.  returns -1 if the
 * pathname cannot be successfully converted (only happens if the
 * current directory cannot be found)
 */
int expand_and_default( unsigned char *nm, unsigned char *def, unsigned char * buf)
	{
	struct fab fab;

	init_fab( &fab );

	if( def == NULL )
		def = null_path;
	
	if( sys_parse( nm, def, &fab ) )
		_str_cpy( buf, fab.result_spec );
	else
		*buf = 0;

	free_fab( &fab );

	return 0;
	}


#ifdef __WATCOMC__
int _chdrive( int drive )
	{
	union REGS r;

	r.h.dl = drive-1;
	r.h.ah = 0x0e;
	intdos( &r, &r );
	return 0;
	}
#endif

/* A chdir() that fiddles the global record */
int chdir_and_set_global_record( unsigned char *dirname )
	{
	int ret;
	unsigned char *p;
	unsigned char path1[MAXPATHLEN+1], path2[MAXPATHLEN+1];
	int len;

	for (p = path1; (*p++ = *dirname++) != 0; )
		;

	ret = expand_and_default( null_path, path1, path2);
	if( ret < 0 )
		return ret;

#ifdef _MSDOS
	{
	int drive;
	drive = toupper( path2[0] ) - 'A' + 1;
	ret = _chdrive( drive );
	if( ret < 0 )
		return ret;
	}
#endif
	len = _str_len(path2);
	/* leave a single PATH_CH */
#ifdef _WINDOWS
	if( len > (ROOT_CHAR_POS+1) )
		path2[len-1] = '\0';
#endif
	ret = chdir( s_str(path2) );
	if( ret < 0 )
		return ret;

	_str_cpy (current_directory, path2);

	UI_update_window_title();

	return 0;
	}

/* return a pointer to a copy of a file name that has been
   converted to absolute form.	This routine cannot return failure. */
unsigned char *save_abs( unsigned char *fn )
	{
	static unsigned char buf[MAXPATHLEN+1];
	if( fn == 0 )
		return 0;
	if( expand_and_default( null_path, fn, buf ) < 0 )
		return u_str(_str_cpy( buf, fn ));

	return buf;
	}

void init_abs( void )
	{
	chdir_and_set_global_record( u_str(".") );
	}
