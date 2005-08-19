/*
 *	Fileserv.c
 *	Copyright 1993-1995 Barry A. Scott
 */
#include	<emacs.h>

#if defined(_WINDOWS)
# include <afxwin.h>
# include "win_incl.h"
# if defined(_NT)
#  define	get_file_attr( name, attr ) \
	((*attr = GetFileAttributes( name )) != (unsigned)-1)
#  define IS_SUBDIR(a) ((FILE_ATTRIBUTE_DIRECTORY&a) != 0)
# else
#  include	<dos.h>

#  define	get_file_attr( name, attr ) (_dos_getfileattr( name, attr ) == 0)
#  define IS_SUBDIR(a) (_A_SUBDIR&a)

# endif
#elif defined(_MSDOS)
#  include	<dos.h>

#  define	get_file_attr( name, attr ) (_dos_getfileattr( name, attr ) == 0)
#  define IS_SUBDIR(a) (_A_SUBDIR&a)

#elif defined(vms)
# define IS_SUBDIR(a) (1&a)
extern int get_file_attr( char *file, unsigned *attr );
#elif defined(unix)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
# define IS_SUBDIR(a) S_ISDIR(a)
static int get_file_attr( char *file, unsigned *attr )
	{
	struct stat st;

	if( stat( file, &st ) == 0 )
		{
		*attr = st.st_mode;
		return 1;
		}

	return 0;
	}
#if defined( __hpux )
char *getwd( char *buf )
{
return getcwd( buf, MAXPATHLEN );
}
#else
extern char *getwd( char * );
#endif

#elif macintosh
# define IS_SUBDIR(a) (1&a)
# define get_file_attr( file, attr ) 0
#else
# error	"Need file services support written for this plateform"
#endif

int file_is_directory( unsigned char *file )
	{
	unsigned attr = 0;

#if defined(_NT)
	attr = (unsigned)GetFileAttributes( s_str(file) );
	if( attr == (unsigned)-1 )
		return 0;
#else
	if( file[_str_len(file)-1] == PATH_CH )
		return 1;
	if( !get_file_attr( s_str(file), &attr ))
		return 0;
#endif

	return IS_SUBDIR(attr);
	}

void init_fab( struct fab *fab )
	{
	fab->disk = 0;
	fab->path = 0;
	fab->filename = 0;
	fab->filetype = 0;
	fab->result_spec = 0;
	}

void free_fab( struct fab *fab )
	{
	if( fab->disk != NULL )
		free( fab->disk );
	if( fab->path != NULL )
		free( fab->path );
	if( fab->filename != NULL )
		free( fab->filename );
	if( fab->filetype != NULL )
		free( fab->filetype );
	if( fab->result_spec != NULL )
		free( fab->result_spec );
	init_fab( fab );
	}

static unsigned char null[1] = {0};

#if defined(_MSDOS) || defined(_WINDOWS) || defined(_NT)
static int analyse_filespec( unsigned char *filespec, struct fab *fab )
	{
	unsigned char *sp, *p, *newstr;
	int len;
	unsigned char filespec_buf[MAXPATHLEN+1];
	int device_loop_max_iterations = 10;

	free_fab( fab );

	sp = filespec;

device_loop:
	if( (p = _str_chr( sp, ':' )) != NULL )
		{
		p++;
		len = p - sp - 1;
		newstr = realloc_ustr( fab->disk, len + 1 + 1 );
		if( newstr == NULL )
			return 0;
		fab->disk = newstr;
		_str_ncpy( newstr, sp, len );
		newstr[len] = '\0';

		/* 
		 * if there is a replacement string use it otherwise
		 * leave the device name as it is
		 */
		{
		char *new_value = get_device_name_translation( s_str(newstr) );
		char buf[MAXPATHLEN+1];                       
		if( new_value == NULL )              
			_str_cat( newstr, ":" );
		else
			{
			/* we are replacing the disk so zap any
			   left over disk */
			free( fab->disk );
			fab->disk = NULL;

			_str_cpy( buf, new_value );
			if( buf[strlen(buf)-1] != PATH_CH )
				strcat( buf, PATH_STR );

			/* add the rest of the file spec to the buffer */
			_str_cat( buf, p );
			/* copy into the main buffer */
			_str_cpy( filespec_buf, buf );
			/* setup the pointer to the file spec to convert */
			sp = filespec_buf;
			/* go do the analysis again */
			device_loop_max_iterations--;
			if( device_loop_max_iterations > 0 )
				goto device_loop;
			}
		}

		sp = p;
		}
	else
		{
		// is this a server address?
		if( _str_ncmp( sp, "\\\\", 2 ) == 0 )
			{
			if( (p = _str_chr( sp+2, PATH_CH )) != NULL )
				{
				int len = p - sp;
				fab->disk = malloc_ustr( len + 1 );
				_str_ncpy( fab->disk, sp, len );
				fab->disk[len] = '\0';

				sp = p;
				}
			}
		}

	if( (p = _str_rchr( sp, PATH_CH )) != NULL )
		{
		p++;
		len = p - sp;
		newstr = malloc_ustr( len + 1 );
		if( newstr == NULL )
			return 0;
		fab->path = newstr;
		_str_ncpy( newstr, sp, len );
		newstr[len] = '\0';

		sp = p;
		}

	if( (p = _str_chr( sp, '.' )) != NULL )
		{
		if( (len = p - sp) != 0 )
			{
			newstr = malloc_ustr( len + 1 );
			if( newstr == NULL )
				return 0;
			fab->filename = newstr;
			_str_ncpy( newstr, sp, len );
			newstr[len] = '\0';
			}
		fab->filetype = savestr( p );
		}
	else
		if( *sp != '\0' )
			fab->filename = savestr( sp );

	return 1;
	}

#ifdef __WATCOMC__
void _getdcwd( unsigned drive, char *buf, unsigned len )
	{
	union REGS r;
	struct SREGS s;

	strcpy( buf, "A:\\" );
	buf[0] = drive + 'A' - 1;

	s.ds = s.es = s.fs = s.gs = FP_SEG( &s );
	r.x.esi = FP_OFF( buf ) + 3;
	r.h.dl = drive;
	r.h.ah = 0x47;
	intdosx( &r, &r, &s );
	}
#endif

int sys_parse( unsigned char *filename, unsigned char *def, struct fab *f_fab )
	{
	struct fab d_fab;
	unsigned char *newstr;
	unsigned char fn_buf[MAXPATHLEN+1];

	init_fab( &d_fab );

	if( !analyse_filespec( filename, f_fab ) )
		return 0;
	if( !analyse_filespec( def, &d_fab ) )
		return 0;

	if( f_fab->disk == NULL )
		if( d_fab.disk != NULL )
			{
			f_fab->disk = d_fab.disk;
			d_fab.disk = 0;
			}
		else
			{
			unsigned drive;

#ifdef _NT
			drive = _getdrive();
#else
			_dos_getdrive( &drive );
#endif 
			f_fab->disk = malloc_ustr( 3 );
			f_fab->disk[0] = (unsigned char)('A' + drive-1);
			f_fab->disk[1] = ':';
			f_fab->disk[2] = '\0';
			}

	/*
	 *	Assume these features of the file system
	 */
	f_fab->file_case_sensitive = 0;
	f_fab->filename_maxlen = 8;
	f_fab->filetype_maxlen = 4;

	/* if its NT then find out what the file system supports */
#if defined(_NT)
	{
	char disk[2];
	int file_parsing_override;

	disk[0] = f_fab->disk[0];
	disk[1] = '\0';

	/* get the override parameter - defaults to let emacs decide */
	file_parsing_override = AfxGetApp()->GetProfileInt( "FileParsing", disk, 0 );

	switch( file_parsing_override )
	{
	case 2:	/* force 8.3 short names */
		f_fab->filename_maxlen = 8;
		f_fab->filetype_maxlen = 4;
		break;

	default:
	case 1:	/* force long names */
		f_fab->filename_maxlen = 255;
		f_fab->filetype_maxlen = 255;
		break;

	case 0:	/* let Emacs decide */
		{
		/* figure out the version of the system */
		OSVERSIONINFO os_info;
		os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

		/* this call may fail on 3.1 with old win32s */
		int is_new_windows = GetVersionEx( &os_info );
		DWORD status = GetLastError();
		if( is_new_windows )
			{
			switch( os_info.dwPlatformId )
			{
			default:
				/* unknown version of windows assume its *new* */
			case VER_PLATFORM_WIN32_WINDOWS:
				/* yep new windows - this is Win95 */
				break;
			case VER_PLATFORM_WIN32s:
				/* this is old windows - win 3.1 with win32s */
				is_new_windows = 0;
				break;
			case VER_PLATFORM_WIN32_NT:
				/* this is NT - figure out the version */
				/* looking for better trwn NT 3.1 */
				if( os_info.dwMajorVersion == 3 && os_info.dwMinorVersion == 1 )
					is_new_windows = 0;
				break;
			}
			}

		if( is_new_windows )
			{
			/*
			 *	On new windows we will trucst GetVolumeInformation to tell the truth
			 *	about file component lengths
			 */

			char root[4];
			DWORD serial_number;
			DWORD max_comp_len;
			char fs_name[32];
			DWORD fs_flags;

			sprintfl( u_str(root), sizeof(root), u_str("%s" PATH_STR), f_fab->disk );

			if( GetVolumeInformation
			(
			root,
			NULL, 0,
			&serial_number,
			&max_comp_len,
			&fs_flags,
			fs_name, sizeof( fs_name )
			) )
				{
				f_fab->filename_maxlen = (int)max_comp_len;
				f_fab->filetype_maxlen = (int)max_comp_len;
				}
			else
				{
				f_fab->filename_maxlen = 255;
				f_fab->filetype_maxlen = 255;
				}
			}
		else
			{
			/*
			 *	On old windows we will trust GetVolumeInformation just so far
			 *	then we figure out the limits
			 */

			char root[4];
			DWORD serial_number;
			DWORD max_comp_len;
			char fs_name[32];
			DWORD fs_flags;

			sprintfl( u_str(root), sizeof(root), u_str("%s" PATH_STR), f_fab->disk );

			if( GetVolumeInformation
			(
			root,
			NULL, 0,
			&serial_number,
			&max_comp_len,
			&fs_flags,
			fs_name, sizeof( fs_name )
			) )
				{
				/* seems that NT is always insensitive */
				/* f_fab->file_case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0; */

				if( strcmp( fs_name, "FAT" ) == 0 )
					{
					f_fab->filename_maxlen = 8;
					f_fab->filetype_maxlen = 4;
					}
				else
					/* assume all others do not need a special check */
					{
					f_fab->filename_maxlen = (int)max_comp_len;
					f_fab->filetype_maxlen = (int)max_comp_len;
					}
				}
			else
				{
				f_fab->filename_maxlen = 255;
				f_fab->filetype_maxlen = 255;
				}
			}
		}
		break;
	}
	}
#endif

	if( f_fab->path == NULL )
		{
		if( d_fab.path != NULL )
			{
			f_fab->path = d_fab.path;
			d_fab.path = 0;
			}
		else
			{
			unsigned char def_path[1+MAXPATHLEN+1];

			_getdcwd( f_fab->disk[0] & 0x1f, s_str(def_path), MAXPATHLEN+1 );
			if( def_path[_str_len(def_path)-1] != PATH_CH )
				_str_cat( def_path, PATH_STR );
			f_fab->path = savestr( &def_path[2] );
			if( f_fab->path == NULL )
				goto error_exit;
			}
		}

	/*
	 *	See if the filename is infact a directory.
	 *	If it is a directory move the filename on to
	 *	the end of the path and null filename.
	 */
	if( f_fab->filename != NULL )
		{
		unsigned char fullspec[MAXPATHLEN+1];

		sprintfl( fullspec, sizeof(fullspec), u_str("%.2s%s%.*s%.*s"),
			f_fab->disk, f_fab->path,
			f_fab->filename_maxlen, f_fab->filename,
			f_fab->filetype_maxlen,
				(f_fab->filetype ? f_fab->filetype : u_str("")) );

		/* get attributes */
		if( file_is_directory( fullspec ) )
			{
			/* need to merge the filename on to the path */
			unsigned char *newstr;
			int len = _str_len( fullspec ) + 1 + 1;

			newstr = malloc_ustr( len );
			if( newstr == NULL )
				goto error_exit;
			sprintfl( newstr, len, u_str("%s%.*s%.*s"),
				f_fab->path,
				f_fab->filename_maxlen, f_fab->filename,
				f_fab->filetype_maxlen,
					(f_fab->filetype ? f_fab->filetype : u_str("")) );

			free( f_fab->path );
			f_fab->path = newstr;
			free( f_fab->filename );
			f_fab->filename = 0;
			if( f_fab->filetype )
				{
				free( f_fab->filetype );
				f_fab->filetype = 0;
				}
			}
		}

	if( f_fab->filename == NULL )
		if( d_fab.filename != NULL )
			{
			f_fab->filename = d_fab.filename;
			d_fab.filename = 0;
			}
		else
			{
			newstr = savestr( null );
			if( newstr == NULL )
				goto error_exit;
			f_fab->filename = newstr;
			}

	if( f_fab->filetype == NULL )
		if( d_fab.filetype != NULL )
			{
			f_fab->filetype = d_fab.filetype;
			d_fab.filetype = 0;
			}
		else
			{
			newstr = savestr( null );
			if( newstr == NULL )
				goto error_exit;
			f_fab->filetype = newstr;
			}

	free_fab( &d_fab );

#if defined(_WINDOWS) || defined( _MSDOS )
	{
	FILE *desc;
	int len;

	sprintfl( fn_buf, sizeof(fn_buf), u_str("%s%sfilename.dat"), f_fab->disk, f_fab->path );

	desc = fopen( s_str(fn_buf), "r" );
	if( desc != NULL )
		{
		unsigned char line_buf[80];
		/* user the newline to make sure that whole filename and type matches */
		sprintfl( fn_buf, sizeof(fn_buf), u_str("%s%s\n"), f_fab->filename, f_fab->filetype );

		len = _str_len( fn_buf );
		if( len > 1 ) /* need more then just a newline! */
		while( fgets( s_str(line_buf), sizeof( line_buf ), desc ) != NULL )
			{
			unsigned char *p = _str_chr( line_buf, ' ' );

			if( p == NULL )
				continue;	/* bogus entry */

			/* if we have a match */
			if( _str_nicmp( p+1, fn_buf, len ) == 0 )
				{
				unsigned char *f_p, *t_p;
				int f_len, t_len;

				*p = '\0';
				f_p = line_buf;
				if( (t_p = _str_chr( line_buf, '.' )) != NULL )
					{
					t_len = _str_len( t_p );
					f_len = t_p - f_p;
					}
				else
					{
					t_len = 0;
					f_len = _str_len( f_p );
					}

				f_fab->filename = realloc_ustr( f_fab->filename, f_len+1 );
				if( f_fab->filename == NULL )
					{
					fclose( desc );
					goto error_exit;
					}
				_str_ncpy( f_fab->filename, f_p, f_len );
				f_fab->filename[f_len] = '\0';

				f_fab->filetype = realloc_ustr( f_fab->filetype, t_len+1 );
				if( f_fab->filetype == NULL )
					{
					fclose( desc );
					goto error_exit;
					}
				_str_ncpy( f_fab->filetype, t_p, t_len );
				f_fab->filetype[t_len] = '\0';
				break;
				}
			}
		fclose( desc );
		}
	}
#endif
	sprintfl( fn_buf, sizeof(fn_buf), u_str("%s%s%.*s%.*s"),
		f_fab->disk,
		f_fab->path,
		f_fab->filename_maxlen, f_fab->filename,
		f_fab->filetype_maxlen, f_fab->filetype );

	newstr = malloc_ustr( MAXPATHLEN+1 );
	if( newstr == NULL )
		return 0;
	if( _fullpath( s_str(newstr), s_str(fn_buf), MAXPATHLEN+1 ) != NULL )
		{
		f_fab->result_spec = newstr;
		if( newstr[_str_len(newstr)-1] != PATH_CH
		&& file_is_directory( newstr ) )
			_str_cat( newstr, PATH_STR );
		}
	else
		return 0;

	if( !f_fab->file_case_sensitive )
		_str_lwr( newstr );

	return 1;
error_exit:
	free_fab( &d_fab );
	return 0;
	}

#elif vms
int get_file_attr( char *file, unsigned *attr )
	{
	int len;

	if( file == NULL || file[0] == '\0' )
		{ *attr = IS_SUBDIR(-1); return 1; }
	len = _str_len( file );
	if( file[len-1] == ':' || file[len-1] == ']' )
		{ *attr = IS_SUBDIR(-1); return 1; }

	*attr = 0;
	return 1;
	}

int sys_parse( unsigned char *filename, unsigned char *def, struct fab *f_fab )
	{
	unsigned char fn_buf[NAM$C_MAXRSS+1];
	struct FAB vms_fab;
	struct NAM vms_nam;
	char esa[NAM$C_MAXRSS+1];
	char rsa[NAM$C_MAXRSS+1];
	int len;
	int status;
	unsigned char *p;

	/*
	 * copy the filename into fn_buf read for processing the directories.
	 * any [dir1][.dir2] is turned into [dir1.dir2]
	 * and [dir1][dir2] into [dir1.dir2]
	 */
	_str_cpy( fn_buf, filename );
	while( (p = _str_str( fn_buf, "][." )) != NULL )
		{
		/* found it so slid the rest of the filespec up */
		memmove( p, p+2, _str_len( p )+1 );
		}
	while( (p = _str_str( fn_buf, "][" )) != NULL )
		{
		/* found it so slid the rest of the filespec up */
		memmove( p, p+1, _str_len( p )+1 );
		*p = '.';
		}

	memset( &vms_fab, 0, sizeof( vms_fab ) );
	vms_fab.fab$b_bid = FAB$C_BID;
	vms_fab.fab$b_bln = FAB$C_BLN;
	vms_fab.fab$l_nam = &vms_nam;

	memset( &vms_nam, 0, sizeof( vms_nam ) );
	vms_nam.nam$b_bid = NAM$C_BID;
	vms_nam.nam$b_bln = NAM$C_BLN;

	/*
	 *	Assume these features of the file system
	 */
	f_fab->file_case_sensitive = 0;
	f_fab->filename_maxlen = 39;
	f_fab->filetype_maxlen = 40;

	vms_fab.fab$b_fns = _str_len( fn_buf );
	vms_fab.fab$l_fna = (char *)fn_buf;
	vms_fab.fab$b_dns = _str_len( def );
	vms_fab.fab$l_dna = (char *)def;

	vms_nam.nam$b_ess = NAM$C_MAXRSS;
	vms_nam.nam$l_esa = esa;

	vms_nam.nam$b_rss = NAM$C_MAXRSS;
	vms_nam.nam$l_rsa = rsa;

	vms_nam.nam$b_nop = NAM$M_SYNCHK;
	status = sys$parse( &vms_fab, 0, 0 );
	if( ! (status&1) )
		return 0;

	f_fab->result_spec = malloc_ustr( vms_nam.nam$b_esl + 1 );
	if( f_fab->result_spec == NULL )
		return 0;

	len = vms_nam.nam$b_esl;
	memcpy( f_fab->result_spec, vms_nam.nam$l_esa, len );

	/* trim off extranious "." and ";" that parse adds
	 * if the filename andtype are missing */
	if( f_fab->result_spec[ len-1 ] == ';' )
		len--;
	if( f_fab->result_spec[ len-1 ] == '.' )
		len--;

	f_fab->result_spec[ len ] = 0;

	_str_lwr( f_fab->result_spec );

	return 1;
	}

#elif defined(unix)
static int analyse_filespec( unsigned char *filespec, struct fab *fab )
	{
	unsigned char *sp, *p, *newstr;
	int len;
	unsigned char filespec_buf[MAXPATHLEN+1];
	int device_loop_max_iterations = 10;

	free_fab( fab );

	sp = filespec;

device_loop:
	if( (p = _str_chr( sp, ':' )) != NULL )
		{
		p++;
		len = p - sp - 1;
		newstr = realloc_ustr( fab->disk, len + 1 + 1 );
		if( newstr == NULL )
			return 0;
		fab->disk = newstr;
		_str_ncpy( newstr, sp, len );
		newstr[len] = '\0';

		{
		unsigned char *p = newstr;

		while( *p )
			if( *p == '$' )
				*p++ = '_';
			else
				p++;
		}

		{
		char buf[MAXPATHLEN+1], *new_res;

		new_res = (char *)get_config_env( newstr );
		if( new_res == NULL )
			_str_cat( newstr, ":" );
		else
			{
			/* we are replacing the disk so zap any
			   left over disk */
			free( fab->disk );
			fab->disk = NULL;

			_str_cpy( buf, new_res );
			if( buf[strlen(buf)-1] != PATH_CH )
				strcat( buf, PATH_STR );

			/* add the rest of the file spec to the buffer */
			_str_cat( buf, p );
			/* copy into the main buffer */
			_str_cpy( filespec_buf, buf );
			/* setup the pointer to the file spec to convert */
			sp = filespec_buf;
			/* go do the analysis again */
			device_loop_max_iterations--;
			if( device_loop_max_iterations > 0 )
				goto device_loop;
			}
		}

		sp = p;
		}

	if( (p = _str_rchr( sp, '/' )) != NULL )
		{
		p++;
		len = p - sp;
		newstr = malloc_ustr( len + 1 );
		if( newstr == NULL )
			return 0;
		fab->path = newstr;
		_str_ncpy( newstr, sp, len );
		newstr[len] = '\0';

		sp = p;
		}

	if( (p = _str_chr( sp, '.' )) != NULL )
		{
		if( (len = p - sp) != 0 )
			{
			newstr = malloc_ustr( len + 1 );
			if( newstr == NULL )
				return 0;
			fab->filename = newstr;
			_str_ncpy( newstr, sp, len );
			newstr[len] = '\0';
			}
		fab->filetype = savestr( p );
		}
	else
		if( *sp != '\0' )
			fab->filename = savestr( sp );

	return 1;
	}


/* input name in nm, absolute pathname output to buf.  returns -1 if the
 * pathname cannot be successfully converted (only happens if the
 * current directory cannot be found)
 */
int fullpath( unsigned char *nm, unsigned char * buf)
	{
	unsigned char *s, *d;
	unsigned char lnm[MAXPATHLEN+1];
	unsigned char def_path[1+MAXPATHLEN+1];

	def_path[0] = '\0';
	getwd( (char *)def_path );

	s = nm;
	d = lnm;
	while( (*d++ = *s) != 0 )
		if (*s++ == '$') {
			register unsigned char  *start = d;
			register int braces = *s == '{';
			register unsigned char  *value;
			while( (*d++ = *s) != 0 )
				if (braces ? *s == '}' : !isalnum (*s))
					break;
				else
					s++;
			*--d = 0;
			if( braces )
				value = get_env ( &start[1] );
			else
				value = get_env ( start );
			if (value != NULL) {
				for (d = start - 1; (*d++ = *value++) != 0;)
					;
				d--;
				if (braces && *s)
					s++;
			}
		}

	d = buf;
	s = def_path;
	nm = lnm;

	if (nm[0] == '~')				/* prefix ~ */
		if (nm[1] == PATH_CH)		/* ~/filename */
			if( (s = get_env("HOME")) != NULL )
				nm += 2;
			else
				s = def_path;
		else {						/* ~user/filename */
			for (s = nm; *s && *s != PATH_CH; s++);
			if (*s == 0)
				s = def_path;
			else {
				register struct passwd *pw;
				*s = 0;
				pw = (struct passwd *) getpwnam (s_str(nm + 1));
				if (pw == 0) {
					*s = PATH_CH;
					s = def_path;
				}
				else {
					nm = s + 1;
					s = u_str(pw->pw_dir);
				}
			}
		}

		while( (*d++ = *s++) != 0 )
				;

	*(d - 1) = PATH_CH;
	s = nm;
	if (*s == PATH_CH)
		d = &buf[0];
	while( (*d++ = *s++) != 0 )
		;

	*(d - 1) = PATH_CH;
	*d = '\0';
	d = buf;
	s = buf;
	while (*s)
		if ((*d++ = *s++) == PATH_CH && d > buf + 1) {
			register unsigned char  *t = d - 2;
			switch (*t)
			{
			case PATH_CH:		/* found // in the name */
				--d;
				break;
			case '.':
				switch (*--t) {
					case PATH_CH: /* found /./ in the name */
						d -= 2;
						break;
					case '.':
						if (*--t == PATH_CH) {/* found /../ */
							while (t > buf && *--t != PATH_CH);
							d = t + 1;
						}
						break;
				}
				break;
			}
		}
	if (*(d - 1) == PATH_CH)
		d--;
	*d = '\0';
	return 1;
	}

int sys_parse( unsigned char *filename, unsigned char *def, struct fab *f_fab )
	{
	struct fab d_fab;
	unsigned char *newstr;
	unsigned char fn_buf[MAXPATHLEN+1];

	init_fab( &d_fab );

	if( !analyse_filespec( filename, f_fab ) )
		return 0;
	if( !analyse_filespec( def, &d_fab ) )
		return 0;

	/*
	 *	Assume these features of the file system
	 */
	f_fab->file_case_sensitive = 1;
	f_fab->filename_maxlen = 128;
	f_fab->filetype_maxlen = 128;

	if( f_fab->path == NULL )
		{
		if( d_fab.path != NULL )
			{
			f_fab->path = d_fab.path;
			d_fab.path = 0;
			}
		else
			{
			unsigned char def_path[1+MAXPATHLEN+1];

			if( getwd( (char *)def_path ) )
				{
				if( def_path[_str_len(def_path)-1] != '/' )
					_str_cat( def_path, "/" );
				f_fab->path = savestr( def_path );
				if( f_fab->path == NULL )
					goto error_exit;
				}
			}
		}

	/*
	 *	See if the filename is infact a directory.
	 *	If it is a directory move the filename on to
	 *	the end of the path and null filename.
	 */
	if( f_fab->filename != NULL )
		{
		char fullspec[MAXPATHLEN+1];

		sprintfl( u_str(fullspec), sizeof(fullspec), u_str("%s%.*s%.*s"),
			f_fab->path,
			f_fab->filename_maxlen, f_fab->filename,
			f_fab->filetype_maxlen,
				(f_fab->filetype ? f_fab->filetype : u_str("")) );

		/* get attributes */
		if( file_is_directory( u_str(fullspec) ) )
			{
			/* need to merge the filename on to the path */
			unsigned char *newstr;

			newstr = malloc_ustr( _str_len( fullspec ) + 1 +
					_str_len( f_fab->filename ) + 1);
			if( newstr == NULL )
				goto error_exit;
			_str_cpy( newstr, fullspec );
			free( f_fab->path );
			f_fab->path = newstr;
			free( f_fab->filename );
			f_fab->filename = 0;
			if( f_fab->filetype )
				{
				free( f_fab->filetype );
				f_fab->filetype = 0;
				}
			}
		}

	if( f_fab->filename == NULL )
		if( d_fab.filename != NULL )
			{
			f_fab->filename = d_fab.filename;
			d_fab.filename = 0;
			}
		else
			{
			newstr = savestr( null );
			if( newstr == NULL )
				goto error_exit;
			f_fab->filename = newstr;
			}

	if( f_fab->filetype == NULL )
		if( d_fab.filetype != NULL )
			{
			f_fab->filetype = d_fab.filetype;
			d_fab.filetype = 0;
			}
		else
			{
			newstr = savestr( null );
			if( newstr == NULL )
				goto error_exit;
			f_fab->filetype = newstr;
			}

	free_fab( &d_fab );

	sprintfl( fn_buf, sizeof(fn_buf), u_str("%s%.*s%.*s"),
		f_fab->path,
		f_fab->filename_maxlen, f_fab->filename,
		f_fab->filetype_maxlen, f_fab->filetype );

	newstr = malloc_ustr( MAXPATHLEN+1 );
	if( newstr == NULL )
		return 0;
	f_fab->result_spec = newstr;
	if( !fullpath( fn_buf, newstr ) )
		goto error_exit;

	return 1;
error_exit:
	free_fab( &d_fab );
	return 0;
	}
#elif macintosh
int sys_parse( unsigned char *filename, unsigned char *def, struct fab *f_fab )
	{
	f_fab->result_spec = savestr( filename );
	return 1;
	}
#else
#error "Need sys_parse..."
#endif

int match_wild( unsigned char *candidate, unsigned char *pattern )
	{
	unsigned char *cp, *pp;		/* candidate and pattern pointers */
	unsigned char *scp, *spp;	/* saved cp and pp */
	unsigned char cch, pch;		/* candidate and pattern char */

	scp = spp = u_str("");		/* init to null string */

	cp = candidate;
	pp = pattern;

	for(;;)
		if( *pp )	/* while there is pattern chars left */
			{
			pch = *pp++;

			if( pch == '*' )
				{
				if( *pp == '\0' )/* pattern null after a * */
					return 1;
				scp = cp;/* save pointers for back tracking */
				spp = pp;
				continue;
				}
			cch = *cp++;
			if( cch == '\0' )/* if candidate exhausted match fails */
				break;

			if( pch == cch )
				continue;
			if( pch == '?' )
				continue;

			/* mismatch detected */
			if( *scp++ == '\0' )
				break;
			cp = scp;
			pp = spp;
			}
		else
			{
			if( *cp == '\0' )
				return 1;
			/* mismatch detected */
			if( *scp++ == '\0' )
				break;
			cp = scp;
			pp = spp;
			}
	return 0;
	}

#if defined( _NT )
struct find_data
	{
	int is_valid;				// clear by restore-environment
	WIN32_FIND_DATA find;
	HANDLE handle;
	int case_sensitive;
	unsigned char root_path[ MAX_PATH ];
	unsigned char full_filename[ MAX_PATH ];
	};

void *find_first_file( unsigned char *files, unsigned char **file )
	{
	struct find_data *f;
	int len;
	unsigned char *p;
	char file_name_buffer[ MAX_PATH ];

	len = (int)GetFullPathName( s_str(files), MAX_PATH, s_str(file_name_buffer), NULL );
	if( len == 0 )
		return NULL;

	f = malloc_struct( find_data );
	if( f == NULL )
		return NULL;

	f->is_valid = TRUE;
	_str_cpy( f->root_path, file_name_buffer );
	p = _str_chr( file_name_buffer, PATH_CH );
	if( p != NULL )
		p[1] = '\0';

	p = _str_rchr( f->root_path, PATH_CH );
	if( p != NULL )
		p[1] = '\0';

	f->case_sensitive = 0;

/* seems to always be case insensitive */
#if 0
	{
	int serial_number;
	int max_comp_len;
	char fs_name[32];
	int fs_flags;

	if(
		GetVolumeInformation
		(
		file_name_buffer,
		NULL, 0,
		&serial_number,
		&max_comp_len,
		&fs_flags,
		fs_name, sizeof( fs_name )
		) )
			f->case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0;
	}
#endif

	f->handle = FindFirstFile( s_str( files ), &f->find );
	if( f->handle == INVALID_HANDLE_VALUE )
		{
		free( f );
		return NULL;
		}

	_str_cpy( f->full_filename, f->root_path );
	_str_cat( f->full_filename, f->find.cFileName );
	if( f->find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
		_str_cat( f->full_filename, PATH_STR );
	if( !f->case_sensitive )
		_str_lwr( f->full_filename );
	*file = f->full_filename;

	return f;
	}

int find_next_file( void *handle, unsigned char **file )
	{
	struct find_data *f = (struct find_data *)handle;

	if( !f->is_valid )
		return 0;
	if( !FindNextFile( f->handle, &f->find ) )
		return 0;

	_str_cpy( f->full_filename, f->root_path );
	_str_cat( f->full_filename, f->find.cFileName );
	if( f->find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
		_str_cat( f->full_filename, PATH_STR );
	if( !f->case_sensitive )
		_str_lwr( f->full_filename );
	*file = f->full_filename;

	return 1;
	}

void find_file_close( void *handle )
	{
	struct find_data *f = (struct find_data *)handle;

	if( f->is_valid )
		FindClose( f->handle );
	free( handle );
	}
#elif defined( _WINDOWS) || defined( _MSDOS )
struct find_data
	{
	int is_valid;				// clear by restore-environment
	struct _find_t find;
	unsigned char root_path[ _MAX_PATH ];
	unsigned char full_filename[ _MAX_PATH ];
	};

void *find_first_file( unsigned char *files, unsigned char **file )
	{
	struct find_data *f;
	int status;
	unsigned char *p;

	/* defend against overflowing root_path */
	if( _str_len( files ) >= _MAX_PATH )
		return NULL;

	/* get the memory we need */
	f = malloc_struct( find_data );
	if( f == NULL )
		return NULL;

	f->is_valid = TRUE;
	/* work out the disk and path */
	f->root_path[0] = '\0';
	_fullpath( s_str(f->root_path), s_str(files), _MAX_PATH );
	p = _str_rchr( f->root_path, PATH_CH );
	if( p == NULL )
		{
		free( f );
		return NULL;
		}
	p[1] = '\0';

	f->find.attrib = 0;
	status = _dos_findfirst( s_str( files ), _A_SUBDIR, &f->find );
	if( status != 0 )
		{
		free( f );
		return NULL;
		}

	_str_cpy( f->full_filename, f->root_path );
	_str_cat( f->full_filename, f->find.name );
	if( f->find.attrib&_A_SUBDIR )
		_str_cat( f->full_filename, PATH_STR );
	_str_lwr( f->full_filename );
	*file = f->full_filename;

	return f;
	}

int find_next_file( void *handle, unsigned char **file )
	{
	struct find_data *f = (struct find_data *)handle;

	if( !f->is_valid )
		return 0;

	if( _dos_findnext( &f->find ) != 0 )
		return 0;

	_str_cpy( f->full_filename, f->root_path );
	_str_cat( f->full_filename, f->find.name );
	if( f->find.attrib&_A_SUBDIR )
		_str_cat( f->full_filename, PATH_STR );
	_str_lwr( f->full_filename );
	*file = f->full_filename;

	return 1;
	}

void find_file_close( void *handle )
	{
	struct find_data *f = (struct find_data *)handle;

	free( handle );
	}


#elif defined(vms)
struct find_data
	{
	int is_valid;				// clear by restore-environment
	unsigned int context;
	struct FAB vms_fab;
	struct NAM vms_nam;
	char esa[NAM$C_MAXRSS+1];
	char rsa[NAM$C_MAXRSS+1];
	char full_filename[NAM$C_MAXRSS+1];
	};

static void find_file_post_process( struct find_data *f )
	{
	unsigned char *p;

	memcpy( f->full_filename, f->rsa, f->vms_nam.nam$b_rsl );
	f->full_filename[ f->vms_nam.nam$b_rsl ] = 0;

	_str_lwr( u_str( f->full_filename ) );

	/* look for a directory and turn the return filespec from
	 * [dir1]dir2.dir;1 into [dir1][.dir2] */
	p = _str_str( f->full_filename, ".dir;1" );
	if( p != NULL )
		{
		*p = '\0';
		p = _str_rchr( f->full_filename, ']' );
		if( p != NULL )
			{
			/* slid up the directory name */
			memmove( p+1+2, p+1, _str_len( p+1 )+1 );
			/* insert the [. and ] */
			p[1] = '[';
			p[2] = '.';
			_str_cat( p, "]" );
			}
		}

	}

void *find_first_file( unsigned char *files, unsigned char **file )
	{
	struct find_data *f;
	int status;
	int len;

	/* defend against overflowing root_path */
	if( (len = _str_len( files )) >= NAM$C_MAXRSS )
		return NULL;

	/* get the memory we need */
	f = malloc_struct( find_data );
	if( f == NULL )
		return NULL;

	f->is_valid = TRUE;

	f->vms_fab.fab$b_bid = FAB$C_BID;
	f->vms_fab.fab$b_bln = FAB$C_BLN;
	f->vms_fab.fab$l_nam = &f->vms_nam;

	f->vms_fab.fab$b_fns = len;
	f->vms_fab.fab$l_fna = (char *)files;
	f->vms_fab.fab$b_dns = 4;
	f->vms_fab.fab$l_dna = "*.*;";

	f->vms_nam.nam$b_bid = NAM$C_BID;
	f->vms_nam.nam$b_bln = NAM$C_BLN;

	f->vms_nam.nam$b_ess = NAM$C_MAXRSS;
	f->vms_nam.nam$l_esa = f->esa;

	f->vms_nam.nam$b_rss = NAM$C_MAXRSS;
	f->vms_nam.nam$l_rsa = f->rsa;

	status = sys$parse( &f->vms_fab, 0, 0 );
	if( ! (status&1) )
		{
		free( f );
		return NULL;
		}

	status = sys$search( &f->vms_fab );
	if( !(status&1) )
		{
		free( f );
		return NULL;
		}

	find_file_post_process( f );

	*file = u_str( f->full_filename );

	return f;
	}

int find_next_file( void *handle, unsigned char **file )
	{
	struct find_data *f = handle;
	int status;

	if( !f->is_valid )
		return 0;

	status = sys$search( &f->vms_fab );
	if( !(status&1) )
		return 0;

	find_file_post_process( f );

	*file = u_str( f->full_filename );

	return 1;
	}

void find_file_close( void *handle )
	{
	struct find_data *f = handle;

	free( f );
	}

#elif defined( unix )

#if defined( _POSIX_VERSION )
#define struct_direct struct dirent
#else
#define struct_direct struct direct
extern DIR * opendir( const char *path );
extern struct_direct * readdir( DIR *fd );
extern int closedir( DIR *fd );

#endif

struct find_data
	{
	int is_valid;				/* clear by restore-environment */
	DIR *find;
	unsigned char root_path[ MAXPATHLEN+1 ];
	unsigned char match_pattern[ MAXPATHLEN+1 ];
	unsigned char full_filename[ MAXPATHLEN+1 ];
	};

void *find_first_file( unsigned char *files, unsigned char **file )
	{
	struct find_data *f;
	unsigned char *p;

	f = malloc_struct( find_data );
	if( f == NULL )
		return NULL;

	f->is_valid = 1;

	/* split into path and pattern */
	_str_cpy( f->root_path, files );
	p = _str_rchr( f->root_path, '/' );
	if( p == NULL )
		goto error_exit_free;

	p++;
	_str_cpy( f->match_pattern, p );
	*p = '\0';

	f->find = opendir( s_str(f->root_path) );
	if( f->find == NULL)
		goto error_exit_free;

	if( find_next_file( f, file ) )
		return f;

	closedir( f->find );

error_exit_free:
	free( f );
	return NULL;
	}

int find_next_file( void *handle, unsigned char **file )
	{
	struct find_data *f = handle;
	struct_direct *d;

	if( !f->is_valid )
		return 0;

	/* read entries looking for a match */
	while ((d = readdir(f->find)) != NULL)
		{
		if( d->d_ino == 0 )
		    continue;

		/* does this one match? */
		if( match_wild( u_str(d->d_name), f->match_pattern ) )
			{
			unsigned attr = 0;

			_str_cpy( f->full_filename, f->root_path );
			_str_cat( f->full_filename, d->d_name );

			get_file_attr( s_str(f->full_filename), &attr );
			if( S_ISDIR( attr ) )
				_str_cat( f->full_filename, PATH_STR );
			else if( !S_ISREG(attr) )
				continue;

			/* return success and the full path */
			*file = f->full_filename;
			return 1;
			}
		}
	return 0;
	}

void find_file_close( void *handle )
	{
	struct find_data *f = handle;
	if( f->is_valid )
		closedir( f->find );
	free( f );
	}

#elif macintosh
void *find_first_file( unsigned char *files, unsigned char **file )
	{
	return NULL;
	}
int find_next_file( void *handle, unsigned char **file )
	{
	return 0;
	}

void find_file_close( void *handle )
	{
	return;
	}
#else
#error "Need file searching for this OS"
#endif
