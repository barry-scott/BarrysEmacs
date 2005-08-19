/* 
 *	fileserv.h	- file services
 */
/* 
	On DOS a file is made up of 4 parts,
	diskname, path, filename and filetype
	Viz:-
		DISK:\PATH\FILE.TYPE
*/
struct fab
	{
	unsigned char	*disk;		/* disk: */
	unsigned char	*path;		/* /path/ */
	unsigned char	*filename;	/* name */
	unsigned char	*filetype;	/* .type */
	unsigned char	*result_spec;	/* full file spec with all fields filled in */
	int	wild;			/* true if any field is wild */
	int	filename_maxlen;	/* how long filename can be */
	int	filetype_maxlen;	/* how long filetype can be */
	int	file_case_sensitive;	/* true if case is important */
	};

void init_fab( struct fab * );
void free_fab( struct fab * );
int sys_parse( unsigned char *, unsigned char *, struct fab * );
int sys_search( unsigned char *, unsigned char *, struct fab * );
int match_wild( unsigned char *, unsigned char * );

void *find_first_file( unsigned char *files, unsigned char **file );
int find_next_file( void *handle, unsigned char **file );
void find_file_close( void *handle );
int file_is_directory( unsigned char *file );
