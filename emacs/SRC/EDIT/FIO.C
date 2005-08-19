/*
 *	fio.c
 */

#include	<emacs.h>

#ifndef macintosh
# include <sys/types.h>
# include <sys/stat.h>
#endif
#if defined( __WATCOMC__ ) || defined( unix )
# define _S_IWRITE	S_IWRITE
#endif
#if defined( _MSDOS ) || defined( _NT ) || defined( _WINDOWS )
# include <share.h>
# if defined( __WATCOMC__ )
#  define SHARE_READ , SH_DENYWR
#  define SHARE_NONE , SH_DENYRW
# else
#  define SHARE_READ , _SH_DENYWR
#  define SHARE_NONE , _SH_DENYRW
# endif
# define fopen _fsopen
#endif
#if defined( unix ) || defined( __WATCOMC__ )
#include <unistd.h>
# define _fstat fstat
# define _stat stat
#endif
#ifndef SHARE_READ
# define SHARE_READ
# define SHARE_NONE
#endif


/* forward */
int fio_access( unsigned char * );
int fio_delete( unsigned char * );
FILE *fio_create( unsigned char *, int, int, unsigned char *, int );
FILE *fio_open( unsigned char *, int, unsigned char *, int * );
int fio_get( FILE *, unsigned char *, int );
int fio_get_with_prompt( FILE *rab, unsigned char *buffer, int size, unsigned char *prompt );
int fio_put( FILE *, unsigned char *, int );
int fio_split_put( FILE *, unsigned char *, int, unsigned char *, int, unsigned char * );
int fio_close( FILE * );
long int fio_size( FILE * );
unsigned char *fio_getname( FILE * );

static unsigned char last_file_name[MAXPATHLEN+1];

/*
 *	check that the file exists and has read or read and write
 *	access allowed
 */
int fio_access( unsigned char *filename )
	{
	int r;

	/*  6 means read and write, 4 means read */
	r = access( s_str( filename ), 6 );
	if( r == 0 )
		return 1;

	r = access( s_str( filename ), 4 );
	if( r == 0 )
		return -1;
	
	return 0;
	}

int fio_delete( unsigned char *filename )
	{
	int r;

	expand_and_default( filename, u_str(""), last_file_name );
	r = unlink( s_str( last_file_name ) );

	return r;
	}

FILE *fio_create( unsigned char *name, int PNOTUSED(size), int PNOTUSED(mode), unsigned char *defnam, int PNOTUSED(attr) )
	{
	FILE *file;

	expand_and_default( name, defnam, last_file_name );
	file = fopen( s_str(last_file_name), "w" TEXT_MODE SHARE_NONE );

	return file;
	}

FILE *fio_open( unsigned char *name, int eof, unsigned char *defnam, int *attr )
	{
	FILE *file;

	if( attr != NULL )
		*attr = 4;	/* force it to "stream" type */

	expand_and_default( name, defnam, last_file_name );

	if( file_is_directory( last_file_name ) )
		return NULL;

	if( eof )
		/* open for append */
		file = fopen( s_str(last_file_name), "a" TEXT_MODE SHARE_NONE );
	else
		/* open for read */
		file = fopen( s_str(last_file_name), "r" TEXT_MODE SHARE_READ );

	return file;
	}

int fio_get( FILE *file, unsigned char *buf, int len )
	{
	int status;
	status = fread( buf, 1, len, file );
	if( ferror( file ) )
		return -1;
	if( status == 0 && feof( file ) )
		return 0;
	return status;	
	}

int fio_get_line( FILE *file, unsigned char *buf, int len )
	{
	fgets( s_str(buf), len, file );
	if( ferror( file ) )
		return -1;
	if( feof( file ) )
		return 0;
	return _str_len( buf );
	}

int fio_get_with_prompt( FILE *file, unsigned char *buf, int len, unsigned char *PNOTUSED(prompt) )
	{
	int status;
	status = fread( buf, 1, len, file );
	if( ferror( file ) )
		return -1;
	if( status == 0 && feof( file ) )
		return -1;
	return status;	
	}

int fio_put( FILE *file, unsigned char *buf , int len )
	{
	int status;
	status = fwrite( buf, 1, len, file );
	if( ferror( file ) )
		return -1;
	return status;
	}

int fio_split_put
	(
	FILE *file,
	unsigned char *buf1, int len1,
	unsigned char *buf2, int len2,
	unsigned char *PNOTUSED(zzz)
	)
	{
	int status;

	status = fwrite( buf1, 1, len1, file );
	if( ferror( file ) )
		return -1;
	status += fwrite( buf2, 1, len2, file );
	if( ferror( file ) )
		return -1;
	return status;
	}

int fio_close( FILE *file )
	{
	return fclose( file );
	}

void fio_setpos( FILE *file, long int pos )
	{
	int status = fseek( file, pos, SEEK_SET );
	if( status != 0 )
		{
		status = errno;
		}
	}

long int fio_getpos( FILE *file )
	{
	return ftell( file );
	}

long int fio_size( FILE *file )
	{
	long int cur_pos, end_of_file_pos;

	/* find the current position */
	cur_pos = ftell( file );

	/* seek to the end of the file */
	if( fseek( file, 0l, SEEK_END ) == 0 )
		/* the current position is the size of the file */
		end_of_file_pos = ftell( file );
	else
		{
		_dbg_msg( u_str("fseek failed!") );
		end_of_file_pos = 0l;
		}

	/* seek back to the orginal position */
	fseek( file, cur_pos, SEEK_SET );

	return end_of_file_pos;
	}

unsigned char *fio_getname( FILE *PNOTUSED(file) )
	{
	return last_file_name;
	}

#if !defined(_NT)
time_t fio_modify_date( FILE *file )
	{
	struct _stat s;

	_fstat( fileno(file), &s );

	return s.st_mtime;
	}

time_t fio_file_modify_date( unsigned char *file )
	{
	struct _stat s;

	if( _stat( s_str(file), &s ) == 0 )
		return s.st_mtime;
	else
		return 0;
	}
#else
time_t fio_file_modify_date( unsigned char *file )
	{
	time_t time;
	int rms_attribute;
	FILE *fd;

	if( (fd = fio_open( file, 0, 0, &rms_attribute)) == NULL )
		return 0;

	time = fio_modify_date( fd );
	fio_close( fd );

	return time;
	}
#endif

/* return true if the file mode is read only for this use */
int fio_access_mode( FILE *file )
	{
	struct _stat stat;

	_fstat( fileno(file), &stat );

	return (stat.st_mode&_S_IWRITE) == 0;
	}
