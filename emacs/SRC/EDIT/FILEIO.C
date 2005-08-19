/* 	Copyright (c) 1982-1993
 *		Barry A. Scott and Nick Emery
 * File IO for Emacs
 */

#include <emacs.h>

#ifdef vms
#include <descrip.h>
#endif

/* Table of contents */

/*forward*/ static void do_auto( unsigned char *filename );
/*forward*/ int apply_auto_execute( void );
/*forward*/ int auto_execute( void );
/*forward*/ int unlink_file( void );
/*forward*/ int file_exists( void );
/*forward*/ int expand_file_name( void );
/*forward*/ int write_file_exit( void );
/*forward*/ int insert_file( void );
/*forward*/ int write_modified_files( void );
/*forward*/ int read_file_command( void );
/*forward*/ int write_current_file( void );
/*forward*/ int visit_file_command( void );
/*forward*/ int write_named_file_command( void );
/*forward*/ int write_named_file( unsigned char *fn );
/*forward*/ int append_to_file( void );
/*forward*/ int visit_file( unsigned char *fn, int createnew, int windowfiddle, unsigned char *dn );
/*forward*/ int read_file( unsigned char *fn, int erase, int createnew );
/*forward*/ static unsigned char *concoct_name( unsigned char *fn, unsigned char *extension );
/*forward*/ int write_file( unsigned char *fn, int appendit );
/*forward*/ FILE *fopenp( unsigned char *path, unsigned char *fn, unsigned char *fnb, int mode, unsigned char *ex );
/*forward*/ int mod_exist( void );
/*forward*/ int check_yes_no( unsigned char *s, ... );
/*forward*/ int mod_write( void );
/*forward*/ void kill_checkpoint_files( void );
/*forward*/ int check_checkpoint( unsigned char *value, struct variablename *v );
/*forward*/ int checkpoint_everything(void);
/*forward*/ int checkpoint_buffers(void);
/*forward*/ unsigned char *fetch_os_error( int error_code );
/*forward*/ int synchonize_files(void);
/*forward*/ void init_FIO( void );

/* Static strings including error messages */
static unsigned char ckp_ext[] = "EMACS$CHECKPOINT:.CKP";
static unsigned char defname[] = "EMACS$CHECKPOINT:CHECKPNT.CKP";
static unsigned char perror_str[] = "%s: %s";
static unsigned char null_file_spec[] = "You must specify a file name";

unsigned char null_str[1] = { 0 };

/* default backup_files based on support of file versions */
#define BACKUP_FILE_MODE_NONE 0
#define BACKUP_FILE_MODE_COPY 1
#define BACKUP_FILE_MODE_RENAME 2

int backup_file_mode =
#ifdef VERS_CH
	BACKUP_FILE_MODE_NONE;
#else
	BACKUP_FILE_MODE_COPY;
#endif

int is_not_accessible = 0;
int is_read_write = 1;
int is_read_only = -1;

GLOBAL SAVRES int unlink_checkpoint_files;
GLOBAL SAVRES unsigned char checkpoint_hook[MLISP_HOOK_SIZE] =
	{"checkpoint-buffers"};

/* If true (the default) Emacs will ask
 * instead of synthesizing a unique name in
 * the case where visit-file encounters a
 * conflict in generated buffer names. */
GLOBAL SAVRES int ask_about_buffer_names;

/* Various local variables */

GLOBAL SAVRES struct automode *auto_list;
GLOBAL SAVRES struct bound_name *checkpoint_proc;
static unsigned char wrote_file[MAXPATHLEN+1];
static unsigned char concoct_str[MAXPATHLEN+1];
static unsigned char expand_file_last_name[MAXPATHLEN+1];

static int write_errors;
static unsigned char *os_error_buf = 0;

/* Write file operation types */

#define APPEND_WRITE 2  	       	   /* Appends to a file */
#define CHECKPOINT_WRITE 1		   /* Writes a checkpoint file */
#define ORDINARY_WRITE 0		   /* Writes an ordinary file */
#define MIN_WRITEFILE ORDINARY_WRITE
#define MAX_WRITEFILE APPEND_WRITE
#define MAX_ERROR_MESSAGE 256		   /* Max size of error string */

int check_backup_file_mode
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	if( value == 0 )
		return 0;

	if( _str_cmp( "none", value ) == 0 )
		backup_file_mode = BACKUP_FILE_MODE_NONE;
	else if( _str_cmp( "copy", value ) == 0 )
		backup_file_mode = BACKUP_FILE_MODE_COPY;
	else if( _str_cmp( "rename", value ) == 0 )
		backup_file_mode = BACKUP_FILE_MODE_RENAME;
	else
		return 0;
	return 1;
	}

unsigned char backup_file_mode_str[8];
void fetch_backup_file_mode( struct expression *e )
	{
	switch( backup_file_mode )
	{
	default:
	case BACKUP_FILE_MODE_NONE: _str_cpy( e->exp_v.v_string, "none" ); break;
	case BACKUP_FILE_MODE_COPY: _str_cpy( e->exp_v.v_string, "copy" ); break;
	case BACKUP_FILE_MODE_RENAME: _str_cpy( e->exp_v.v_string, "rename" ); break;
	}
	}

int cur_dir( void )
	{
	ml_value -> exp_type = ISSTRING;
	ml_value -> exp_v.v_string = current_directory;
	ml_value -> exp_release = 0;
	ml_value -> exp_int = _str_len(ml_value -> exp_v.v_string);
	return 0;
	}

/* Perform the auto-execute action (if any) for the specified filename */

static void do_auto ( unsigned char *filename )
	{
	unsigned char fn[MAXNAMLEN+1];
	unsigned char *fn_in;
	struct automode *p;
	int saverr = err;

	err = 0;

	/*
	 * trim off the directory path at the front of the filename and
	 * and the version number from the end
	 */
	fn_in = _str_rchr( filename, PATH_CH );
	if( fn_in == NULL )
		_str_cpy( fn, filename );
	else
		_str_cpy( fn, fn_in+1 );

#ifdef VERS_CH
	fn_in = _str_chr( fn, VERS_CH );
	if( fn_in != NULL )
		*fn_in = '\0';
#endif

	fn_in = fn;
	while( *fn_in )
		if( isupper( *fn_in ) )
			*fn_in++ += 'a' - 'A';
		else
			fn_in++;

	/*
	 * Scan the list of autoexecutes. For each pattern that matches the
	 * supplied file name, execute the supplied function
	 */

	p = auto_list;
	while( p != 0)
		{
		if( match_wild (fn, p->a_pattern) )
			{
			execute_bound_saved_environment (p->a_what);
			break;
			}
		p = p->a_next;
		}

	err |= saverr;
	}


/* The apply-auto-execute function interface */
int apply_auto_execute( void )
	{
	unsigned char *file = getstr(u_str(": apply-auto-execute "));

	if( file != 0 )
		do_auto( file );

	return 0;
	}			/* of ApplyAutoExecute */


/* Set up an auto-execute - function interface */
int auto_execute( void )
	{
	int what = getword( mac_names_ref, u_str(": auto-execute ") );
	unsigned char *pattern;
	struct automode *p;

	if( what < 0 )
		return 0;

	pattern = getstr (u_str(": auto-execute %s when name matches "),
			mac_names[what]);

	if( pattern == 0 )
		return 0;

	if( (p = malloc_struct( automode )) == 0
	|| (pattern = savestr( pattern )) == 0 )
		{
		error (u_str ("Out of memory!"));
		return 0;
		}

	p->a_pattern = pattern;
	while( pattern[0] != 0)
		{
		if( pattern[0] >= 'A' && pattern[0] <= 'Z' )
			pattern[0] += 'a' - 'A';
		pattern++;
		}
	p->a_len = p->a_pattern - pattern;
	p->a_what = mac_bodies[what];
	p->a_next = auto_list;
	auto_list = p;

	return 0;
	}			/* of AutoExecute */


int list_auto_executes( void )
	{
	unsigned char line[150];
	struct emacs_buffer *old;
	unsigned char *what;
	struct automode *p;

	old = bf_cur;

	scratch_bfn( u_str("Auto Execute list"), interactive );

	ins_str( u_str("  Pattern               Function\n") );
	ins_str( u_str("  -------               --------\n") );

	p = auto_list;
	while( p != 0 )
		{
		int i;

		what = u_str("");
		for( i=0; i<=n_macs - 1; i += 1 )
			if( mac_bodies[ i ] == p->a_what )
				{
				what = mac_names[ i ];
				break;
				}

		sprintfl( line, sizeof( line ) - 1,
			u_str("  %-20s  %s\n"), p->a_pattern, what );
		ins_str( line );

		p = p->a_next;
		}
	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );

	return 0;
	}


/* Function interface to delete a file */
int unlink_file( void )
	{
	unsigned char *fn = getescfile(u_str(": unlink-file "),NULL,0,1);
	unsigned char fullname[MAXPATHLEN+1];

	if( fn == 0 )
		return 0;

	expand_and_default (fn, null_str, fullname);

	ml_value->exp_int = fio_delete (fullname) ? 0 : -1;
	ml_value->exp_type = ISINTEGER;
	return 0;
	}				/* Of UnlinkFile */


/* Function interface to check if a file exists */
int file_exists( void )
	{
	unsigned char *fn = getescfile(u_str(": file-exists "),NULL,0,1);
	unsigned char fullname[MAXPATHLEN+1];

	if( fn == 0 )
		return 0;

	expand_and_default (fn, null_str, fullname);

	ml_value->exp_int = fullname[0] == 0 ? is_not_accessible : fio_access (fullname);
	ml_value->exp_type = ISINTEGER;

	return 0;
	}				/* Of FileExists */

 
int expand_file_name( void )
	{
	static void *search_file_handle;
	unsigned char *fn = getescfile(u_str(": expand-file-name "),NULL,0,1);
	unsigned char *file;

	if( fn == 0 )
		return 0;

	if( _str_len (fn) > 0 )
		{
		unsigned char fullname[MAXPATHLEN+1];

		expand_and_default (fn, null_str, fullname);

		_str_cpy (expand_file_last_name, fullname);

		if( search_file_handle != NULL )
			find_file_close( search_file_handle );

		search_file_handle = find_first_file( fullname, &file );
		if( search_file_handle == NULL )
			{
			error (perror_str, fetch_os_error(errno), expand_file_last_name);
			return 0;
			}
		}
	else
		{
		if( search_file_handle == NULL )
			{
			error( u_str("expand-file has not been initialised") );
			return 0;
			}
		if( !find_next_file( search_file_handle, &file ) )
			{
			find_file_close( search_file_handle );
			search_file_handle = NULL;
			file = u_str("");
			}
		}

	ml_value->exp_v.v_string = file;
	ml_value->exp_type = ISSTRING;
	ml_value->exp_release = 0;
	ml_value->exp_int = _str_len( ml_value->exp_v.v_string );

	return 0;
	}				/* Of ExpandFileName */


/* Function interface to write all files, and exit Emacs */
int write_file_exit( void )
	{
	if( mod_write () != 0 )
		return -1;
	else
		return 0;	/* End of WriteFileExit */
	}

static void backup_buffer( unsigned char *fn )
	{
	/*
	 *	A backup file name has an "_" added to the front of the type
	 */
	struct fab fab;
	unsigned char type_name[MAXPATHLEN+1];

	init_fab( &fab );
	sys_parse( fn, null_str, &fab );

	/* start with the ._ */
	_str_cpy( type_name, "._" );
	/* then if there is any type add it to the end */
	if( _str_len( fab.filetype ) > 1 ) /* more then a "." */
		_str_cat( type_name, fab.filetype+1 );

	sys_parse( type_name, fn, &fab );

	/* delete the old backup file */
	if( remove( s_str(fab.result_spec ) ) != 0 && errno == EACCES )
		{
		error( u_str("Failed to delete %s"), fab.result_spec );
		return;
		}

	if( backup_file_mode == BACKUP_FILE_MODE_RENAME )
		{
		/*
		 * rename the old file to the backup file name
		 * but expect fn not to exist
		 */
		if( rename( s_str(fn), s_str(fab.result_spec) ) != 0 && errno != ENOENT )
			{
			error( u_str("Failed to rename %s to %s"), fn, fab.result_spec );
			return;
			}
		}
	else
		{
		/*
		 * copy the old file to the backup file name
		 */
		FILE *in, *out;
		int rms_attribute;
		int len;
		unsigned char buf[16384];

		/* Open the input file */
		if( (in = fio_open (fn, 0, 0, &rms_attribute)) == NULL )
			{
			if( errno != ENOENT )
				error( u_str("Failed to open for backup %s"), fn );
			return;
			}

		/* Create the output file */
		if( (out = fio_create(fab.result_spec,0,0,NULL,rms_attribute)) == NULL )
			{
			error( u_str("Failed to create for backup %s"), fab.result_spec );
			return;
			}
		/* copy all the input file to the output file */
		while( (len = fio_get( in, buf, sizeof(buf) )) > 0 )
			if( fio_put( out, buf, len ) < 0 )
				{
				error( u_str("Error writing while backing up to %s"), fab.result_spec );
				fio_close( in );
				fio_close( out );
				return;
				}

		fio_close( in );
		fio_close( out );
		if( len < 0 )
			{
			error( u_str("Error reading while backing up from %s"), fab.result_spec );
			return;
			}
		}
	}


/* Internal routine to write a buffer */
static int write_this( unsigned char *fname )
	{
	int rv = 0;
	unsigned char *fn;
	unsigned char fn_buf[MAXPATHLEN+1];

	if( fname == 0 )
		fn = bf_cur->b_fname;
	else
		fn = fname;
	if( fn == 0 )
		{
		error (u_str("No file name associated with buffer %s"),
			bf_cur->b_buf_name);
		return rv;
		}

	_str_cpy( fn_buf, fn );
	fn = fn_buf;

#ifdef VERS_CH
	{
	unsigned char *p;

	p = _str_chr( fn, VERS_CH );
	if( p != NULL )
		*p = '\0';
	}
#endif

	if( backup_file_mode != BACKUP_FILE_MODE_NONE
	&& bf_cur->b_file_time != 0 )
		{
		backup_buffer( fn );
		if( err )
			return -1;
		}


	if( write_file (fn, ORDINARY_WRITE) != 0 )
		{
		if( bf_cur->b_fname )
			free (bf_cur->b_fname);
		bf_cur->b_fname = savestr (wrote_file);
		journal_stop( bf_cur );
		rv = -1;
		}

	if( unlink_checkpoint_files != 0 )
		{
		if( !err
		&& bf_cur->b_checkpointfn != 0 )
			fio_delete (bf_cur->b_checkpointfn);

		if( bf_cur->b_checkpointed > 0 )
			bf_cur->b_checkpointed = 0;
		}

	return rv;
	}


/* Function interface to insert-file */
int insert_file( void )
	{
	int old_size;

	if( bf_mode.md_readonly )
		{
		/* call insert_at to generate the read only error message */
		insert_at( 1, 0 );
		return 0;
		}

	old_size = bf_s1 + bf_s2;

	if( read_file( getescfile( u_str("Insert file: "), NULL, 0, 1 ), 0, 0 ) != 0
	|| interrupt_key_struck != 0 )
		{
		if( bf_modified == 0 )
			{
			redo_modes = 1;
			cant_1line_opt = 1;
			}
		bf_modified++;
		}

	if( bf_mode.md_syntax_array )
		syntax_insert_update( dot, (bf_s1 + bf_s2) - old_size );

	return 0;
	}				/* Of InsertFile */


/* Function interface to write all modified files */
int write_modified_files( void )
	{
	mod_write ();
	return 0;
	}				/* Of WriteModifiedFiles */


int read_file_command( void )
	{
	unsigned char *fn = getescfile(u_str("read file: "),NULL,0,1);

	if( fn == 0 )
		return 0;

	read_file (fn, 1, 0);
	do_auto (fn);

	return 0;
	}				/* Of ReadFile_command */


/* Code to write the current file */
int write_current_file( void )
	{
	write_this (NULL);

	cant_1line_opt = 1;
	redo_modes = 1;

	return 0;
	}				/* Of WriteCurrentFile */


/* Function interface to visit_file */
int visit_file_command( void )
	{
	visit_file( getescfile (u_str("Visit file: "), NULL,0,1), 1, 1, null_str );

	return 0;
	}				/* Of VisitFile_command */


/* Function interface to write a specific file */
int write_named_file_command( void )
	{
	unsigned char *fn = getescfile( u_str("write file: "), NULL,0, 0 );
	return write_named_file( fn );
	}

int write_named_file( unsigned char *fn )
	{
	if( fn != 0 )
		{
		if( fn[0] == 0 )
			{
			if( bf_cur->b_fname == 0 )
				{
				error( null_file_spec );
				return 0;
				}
			else
				fn = 0;
			}

		/* Write the file, update the */
		if( write_this (fn) )
			{
			bf_cur->b_kind = FILEBUFFER;
			}

		cant_1win_opt = 1;
		}

	return 0;
	}			/* Of WriteNamedFile */


/* Appends to a file */
int append_to_file( void )
	{
	unsigned char *fn = getescfile(u_str(": append-to-file "),NULL,0,1);

	if( fn != 0 )
		/* if the length of the filename is zero, then complain */
		if( fn[0] == 0 )
			error( null_file_spec );
		else
			write_file (fn, APPEND_WRITE);

	return 0;
	}				/* Of AppendToFile */


/* Visit a file - all the internal logic for this wonderous function */
int visit_file ( unsigned char *fn, int createnew, int windowfiddle, unsigned char *dn )
	{
	unsigned char *p;
	int r_stat;
	unsigned char fullname[MAXPATHLEN+1];
	unsigned char bufname_text[MAXPATHLEN+1];
	unsigned char synthname[MAXPATHLEN+1];
	struct emacs_buffer *b;
	struct emacs_buffer *oldb = bf_cur;

	/* If no filename is supplied, just return */
	if( fn == 0 )
		return 0;

	expand_and_default (fn, dn, fullname);

	b = buffers;
	while( b != 0
	&& (b->b_fname == 0 || _str_cmp( fullname, b->b_fname ) != 0) )
		b = b->b_next;

	if( b != 0 )
		set_bfp (b);
	else
		{
		unsigned char *bufname;

		bufname = _str_rchr( fullname, PATH_CH );
		if( bufname == NULL
		|| _str_cmp( bufname, "." ) == 0 )
			bufname = u_str ("no-name");
		else
			{
			_str_cpy( bufname_text, bufname+1 );
			bufname = bufname_text;
#ifdef VERS_CH
			/* if the file name has a version number at the end strip it off */
			{
			unsigned char *p;
			p = _str_chr( bufname, VERS_CH );
			if( p != NULL )
				*p = '\0';
			}
#endif
			}

		/*
		 * Now, check_ to see if the buffer is in use, and perform
		 * appropriate actions
		 */
		if( find_bf( bufname ) != 0 )
			{
			if( interactive
			&& ask_about_buffer_names != 0 )
				{
				p = getstr( u_str("Buffer name %s is in use, type a new name or <CR> to clobber: "), bufname);
				if( p == 0 )
					return 0;
				if( p[0] != 0 )
					bufname = p;
				}
			else
				{
				/*
				 * The buffer exists, but ask-about-buffer-names is
				 * false. So, we have to construct a buffer name. We do
				 * this by taging <n> onto the end of the existing
				 * buffer name, where n is a number. Then try and
				 * find the resulting buffer. Do this until a buffer
				 * is not found
				 */
				int seq = 1;

				do
					{
					sprintfl( synthname, sizeof (synthname),
						u_str("%s<%d>"), bufname, seq);
					seq++;
					}
				while( find_bf( synthname ) != 0 );

				bufname = synthname;
				}
			}

		/* Create the buffer, and free up the old filename */
		set_bfn( bufname );

		if( bf_cur->b_fname != 0
		&& createnew != 0 )
			{
			free( bf_cur->b_fname );
			bf_cur->b_fname = 0;
			}

		/* Read in the file */
		r_stat = read_file( fullname, 1, createnew );

		/*
		 * When the read is interrupted, leave what has already been
		 * read in the appropriate buffer. Then make it a scratch buffer
		 * and causes it to not be popped up onto the screen
		 */
		if( interrupt_key_struck != 0 )
			{
			set_bfp( oldb );
			return 0;
			}

		/*
		 * check_ to see if a failure has occurred during the read. Process it
		 * as required by the various options
		 */
		if( r_stat == 0 )
			{
			if( createnew == 0 )
				{
				set_bfp (oldb);
				return 0;
				}
			}

		/*
		 * Fill in the rest of the buffer information
		 */
		bf_cur->b_kind = FILEBUFFER;
		if( bf_cur->b_checkpointfn != 0 )
			free (bf_cur->b_checkpointfn);
		bf_cur->b_checkpointfn = 0;
		bf_cur->b_checkpointed = checkpoint_frequency != 0 ? 0 : -1;
		}

	/*
	 * Put the newly created buffer in the window ring, and make
	 * sure that any auto-executes get executed
	 */

	if( windowfiddle != 0 )
		window_on (bf_cur);

	if( b == 0 )
		do_auto( fullname );

	return 1;
	}				/* Of visit_file */


/* Read a file into a buffer */
int read_file( unsigned char *fn, int erase, int createnew )
	{
	FILE *fd;
	int n;
	int i;
	int rms_attribute;
	int fsize;
	unsigned char *fnam;
	unsigned char fullname[MAXPATHLEN+1];

	if( fn == 0 )
		return 0;

	if( fn[0] == 0 )
		{
		error (null_file_spec);
		return 0;
		}

	/* allow * and % to be expanded */
	expand_and_default (fn, null_str, fullname);

	/* Open the file if possible */
	if( (fd = fio_open (fullname, 0, 0, &rms_attribute)) == NULL )
		{
		int saved_errno = errno;

		bf_cur->b_file_access = 0;
		bf_cur->b_file_time = 0;
		bf_cur->b_synch_file_access = 0;
		bf_cur->b_synch_file_time = 0;

		/* If creating a new file, do not report FNF error */
		if( createnew != 0
		&& errno == ENOENT )
			{
			cant_1line_opt = 1;
			redo_modes = 1;
			if( erase )
				erase_bf (bf_cur);

			error (u_str ("New file: %s"), fn);

			bf_cur->b_fname = savestr (fullname);
			bf_cur->b_kind = FILEBUFFER;
			}
		else
			error (perror_str, fetch_os_error (errno), fn);

		errno = saved_errno;
		return 0;
		}

	/*
	 * Read in the file into an appropriate environment
	 */
	cant_1line_opt = 1;
	redo_modes = 1;

	/* Erase the buffer if requested */
	if( erase )
		{
		/* if we are erasing turn off readonly mode */
		bf_mode.md_readonly = 0;
		erase_bf (bf_cur);
		bf_cur->b_rms_attribute = rms_attribute;
		}

	/*
	 * Set the gap to the correct place, and make sure there is
	 * enough room in it to contain the files contents
	 */
	gap_to (dot);
	if( erase )
		done_is_done ();

	if( gap_room( (fsize = (int)fio_size (fd)) + 2000) != 0 )
		{
		fio_close (fd);
		error (u_str ("No room for file %s"), fn);
		return 0;
		}

	/* Get the real filename */
	fnam = fio_getname (fd);
	if( erase )
		{
		bf_cur->b_synch_file_time =
			bf_cur->b_file_time =
				fio_modify_date( fd );
		bf_cur->b_synch_file_access =
			bf_cur->b_file_access =
				fio_access( fnam );
		bf_cur->b_mode.md_readonly = bf_cur->b_file_access < 0;
		}
	/*
	 * Read in the files contents
	 */
	n = 0;
	i = 0;

	if( fsize > 0 )
		while( ! err
		&& (i = fio_get (fd, &bf_p1[bf_s1 + 1 + n], fsize - n)) > 0 )
			{
			n += i;
			if( n > fsize - 1000 )
				gap_room (fsize = fsize + 2000);
			}

	/* Close  the file, and adjust the pointers */
	fio_close (fd);
	if( n > 0 )
		{
		if( !erase )
			record_insert( dot, n, &bf_p1[bf_s1 + 1] );
		bf_s1 = bf_s1 + n;
		bf_gap = bf_gap - n;
		bf_p2 = &bf_p2[-n];
		}

	/* Generate appropriate messages */
	if( n == 0
	&& ! err )
		message (u_str ("Empty file."));

	if( i == EOF )
		error (perror_str, fetch_os_error (errno), fn);

	if( erase )
		{
		if( bf_cur->b_checkpointfn != 0 )
			{
			free (bf_cur->b_checkpointfn);
			bf_cur->b_checkpointfn = 0;
			if( bf_cur->b_checkpointed > 0 )
				bf_cur->b_checkpointed = 0;
			}

		if( bf_cur->b_fname != NULL )
			free (bf_cur->b_fname);

		if( interrupt_key_struck )
			{
			bf_cur->b_kind = SCRATCHBUFFER;
			bf_cur->b_fname = NULL;
			}
		else
			{
			bf_cur->b_fname = savestr (fnam);
			bf_cur->b_kind = FILEBUFFER;
			}
		}

	/* return a success indication as required from the read */
	return i >= EOF && ! err;
	}


/* Concoct a checkpoint filename */
static unsigned char *concoct_name( unsigned char *fn, unsigned char *extension )
	{
	FILE *fd;

	expand_and_default (extension, fn, concoct_str);

	/* Create the file to check_ that it is valid, and get its real name */
	if( (fd = fio_create(concoct_str, 0, FIO_STD, 0, default_rms_attribute)) == NULL
	&& (fd = fio_create(defname, 0, FIO_STD, 0, default_rms_attribute )) == NULL )
			return defname;

	_str_cpy (concoct_str, fio_getname (fd));
	fio_close (fd);

	return concoct_str;
	}			/* Of concoct_name */


/*
 * Write the current buffer to the named file; returns true iff
 * successful. Appends to the file if AppendIt is append_write, does a checkpoint
 * style write if AppendIt is checkpoint_write.
 */
int write_file ( unsigned char *fn, int appendit )
	{
	FILE *fd=NULL;
	int nc = bf_s1 + bf_s2;

	wrote_file[0] = 0;

	if( fn == 0 )
		return 0;

	/* Open the file, and position to the correct place */
	switch( appendit )
	{
	case APPEND_WRITE:
		/*
		 * Open and existing file for appending, or if there is none,
		 * create a new file
		 */
		fd = fio_open (fn, 1, 0, 0);
		if( fd == NULL )
			fd = fio_create (fn, nc, 0, 0, 0);
		break;

	case CHECKPOINT_WRITE:
		fd = fio_create (fn, nc, 1, 0, bf_cur->b_rms_attribute );
		break;

	case ORDINARY_WRITE:
		fd = fio_create (fn, nc, 0, 0, bf_cur->b_rms_attribute );
		break;
	}

	/* Write out the contents of the buffer if the file was created */
	if( fd == 0 )
		{
		error (perror_str, fetch_os_error (errno), fn);
		return 0;
		}

	/*  Save the filespec */
	_str_cpy (wrote_file, fio_getname (fd));

#ifdef vms
	{
	int n;
	unsigned char *p;
	unsigned char *q;
	unsigned char *p2;
	unsigned char *q2;
	unsigned char *start;
	unsigned char *endp;

	/*  Copy the p1 region first */
	p = start = &bf_p1[1];
	endp = &start[bf_s1];

	while( p < endp
	&& ! err )
		{
		/*  Locate a single record as terminated with an LF */
		q = p;
		while( q < endp
		&& *q != '\n' )
			q++;

		/* Skip out if the loop crosses the gap */
		if( q >= endp )
			break;

		/* Put the record, and check_ for any failure */
		if( (n = fio_put (fd, p, q - p)) < 0 )
			{
			error (perror_str, fetch_os_error (errno), fn);
			fio_close (fd);
			return 0;
			}

		/* Update the pointers as required */
		p = &p[n];
		if( *p == '\n' )
			p++;
		}

	/* Deal with any cross gap records */
	p2 = &bf_p1[1 + bf_s1 + bf_gap];
	if( ! err
	&& p < endp )
		{
		unsigned char *full;

		/* Find the end of the split record */
		q2 = p2;
		while( q2 < &p2[bf_s2]
		&& q2[0] != '\n' )
			q2 = &q2[1];

		/*  Try to allocate a buffer for the split record */
		if( (full = malloc_ustr (q - p + q2 - p2)) == 0 )
			{
			/* No memory, move the gap to the start of the nxt record */
			gap_to (p - start);
			p2 = &bf_p1 [1 + bf_s1 + bf_gap];
			}
		else
			{
			/* Emit the split record */
			fio_split_put (fd, p, q - p, p2, q2 - p2, full);
			p2 = q2;
			if( p2[0] == '\n' )
				p2 = &p2[1];
			free (full);
			}
		}

	/* Emit the p2 region */
	endp = &bf_p1[1 + bf_s1 + bf_gap + bf_s2];
	while( p2 < endp
	&& ! err )
		{
		/* find the end of record boundary */
		q2 = p2;
		while( q2 < endp
		&& q2[0] != '\n' )
			q2 = &q2[1];

		/* Emit the record, and check_ for errors */
		if( (n = fio_put (fd, p2, q2 - p2)) < 0 )
			{
			error (perror_str, fetch_os_error (errno), fn);
			fio_close (fd);
			return 0;
			}

		/* Update the various pointers */
		p2 = &p2[n];
		if( p2[0] == '\n' )
			p2 = &p2[1];
		}
	}
#else
	if( bf_s1 )
		if( fio_put( fd, &bf_p1[1],bf_s1 ) < 0 )
			{
			error (perror_str, fetch_os_error (errno), fn);
			fio_close (fd);
			return 0;
			}
	if( bf_s2 )
		if( fio_put( fd, &bf_p1[1 + bf_s1 + bf_gap],bf_s2 ) < 0 )
			{
			error (perror_str, fetch_os_error (errno), fn);
			fio_close (fd);
			return 0;
			}
#endif

	fio_close (fd);

	/* Update the modified flag and checkpointing information */
	if( ! err )
		{
		/* if this is an ordinary write then we need to
		 * figure out the new modify time of the file
		 */
		if( appendit == ORDINARY_WRITE )
			{
			bf_modified = 0;
			bf_cur->b_synch_file_time =
				bf_cur->b_file_time =
					fio_file_modify_date( wrote_file );
			bf_cur->b_synch_file_access = 1; /* writable */
			}

		if( bf_cur->b_checkpointed > 0 )
			bf_cur->b_checkpointed = 0;
		if( appendit != CHECKPOINT_WRITE )
			message (u_str ("Wrote %s"), wrote_file);
		}

	return 1;
	}				/* write_file */


/* fopenp opens the file fn with the given IO mode using the given
 * search path. The actual file name is returned in fnb. It is used to
 * to read in MLisp files via the executed-mlisp-file function. */
FILE *fopenp
	(
	unsigned char *path,
	unsigned char *fn,
	unsigned char *fnb,
	int append,
	unsigned char *ex
	)
	{
	unsigned char *start, *end;

	/* Provide some defaults
	 * The search path */
	if( path == NULL )
		path = null_str;

	/* The filename */
	if( fn == NULL )
		fn = null_str;

	/* The extension */
	if( ex == NULL )
		ex = null_str;

	/*
	 * check for Node, device or directory specs. If present then
	 * just open the file stright
	 */
#ifdef unix
	if(_str_chr( fn, ':' ) != NULL)
		{
		/* Expand filenames containing a ``logical name'' */
		unsigned char fullname[MAXPATHLEN+1];

		expand_and_default (fn, null_str, fullname);
		
		return fio_open (fullname, append, ex, 0);
		}
	else
#endif
	if( _str_chr( fn, PATH_CH ) != NULL
#ifdef DISK_CH
	|| _str_chr( fn, DISK_CH ) != NULL
#endif
	)
		return fio_open (fn, append, ex, 0);

	/*
	 * work around bugs in calling this routine with
	 * static data that is destoryed. As is the case
	 * with get_config_env
	 */
	path = savestr( path );

	/*
	 * Otherwise, add the path onto the front of the filespec
	 */
	start = path;
	while( *start )
		{
		FILE *file;

		end = _str_chr( start, PATH_SEP );
		if( end == NULL )
			end = start + _str_len( start );

		sprintfl( fnb, MAXPATHLEN+1, u_str ("%.*s%s%s"),
			end-start, start,
			end[-1] == PATH_CH
#ifdef DISK_CH
			|| end[-1] == DISK_CH 
#endif
				? "" : PATH_STR,
			fn );
		file = fio_open (fnb, append, ex, 0);
		if( file != NULL )
			{
			free( path );
			return file;
			}
		start = end;
		if( *start )
			start++;
		}

	free( path );
	return NULL;
	}			/* Of fopenp */


/* Eeturns true if modified buffers exist */
int mod_exist( void )
	{
	struct emacs_buffer *b;

	set_bfp (bf_cur);
	b = buffers;
	while( b != 0)
		if( b->b_modified != 0
		&&  b->b_kind == FILEBUFFER )
			return 1;
		else
			b = b->b_next;

	return 0;
	}			/* Of mod_exist */


/* Write all modified buffers; return true iff OK */

int mod_write( void )
	{
	struct emacs_buffer *b;
	struct emacs_buffer *old = bf_cur;
	int write_errors = 0;

	/* force the global buffer data into the emacs_buffer structure */
	set_bfp( old );

	b = buffers;
	while( !err && b != NULL )
		{
		set_bfp (b);
		if( b->b_kind == FILEBUFFER
		&& b->b_modified != 0 )
			{
			set_bfp (b);
			if( write_this (NULL) == 0 )
				if( interactive
				|| get_yes_or_no
				   ( 0,
				   u_str("Cannot write buffer %s, can I ignore it? "),
				   b->b_buf_name
				   ) == 0 )
					write_errors++;
			}
		b = b->b_next;
		}

	set_bfp (old);

	return !err && write_errors == 0;
	}			/* Of mod_write */


/* Deletes all checkpoint file */
void kill_checkpoint_files( void )
	{
	struct emacs_buffer *b;

	if( unlink_checkpoint_files != 0 )
		{
		b = buffers;
		while( b != 0)
			{
			if( b->b_checkpointfn != 0 )
				{
				fio_delete (b->b_checkpointfn);
				b->b_checkpointfn = 0;
				}
			 b = b->b_next;
			 }
		}
	}			/* kill_checkpoint_files */


/* Set thunk for changing the checkpoint-hook function */
int check_checkpoint ( unsigned char *value, struct variablename *v )
	{
	return check_is_proc (value, v, &checkpoint_proc);
	}


/* Function that initiate checkpointing */
int checkpoint_everything(void)
	{
	unsigned char saved_activity_char = activity_character;
	int rv = 0;

	write_errors = 0;

	if( checkpoint_proc != 0 )
		{
		if( /* boundf == 0 || */ interactive )
			{
			if( activity_indicator != 0 && term_ansi != 0 )
				set_activity_character ('c');
			else
				message (u_str("checkpointing..."));

			do_dsp (0);
			}

		rv = execute_bound_saved_environment(checkpoint_proc);

		if( /* boundf == 0 || */ interactive )
			{
			if( write_errors == 0 || interrupt_key_struck != 0 )
				{
				if( interrupt_key_struck != 0 )
					message (u_str("checkpointing... interrupted."));
				else
					if( activity_indicator != 0 && term_ansi != 0 )
						set_activity_character (saved_activity_char);
					else
						message (u_str("checkpointing... done."));
				}
			else
				error (u_str("checkpointing... failed."));

/*			if( boundf == 0 ) */
				do_dsp (0);
			}
		}

	return rv;
	}			/* Of checkpoint_everything */


/* Function interface to the checkpoint-buffers default checkpoint action */
int checkpoint_buffers(void)
	{
	struct emacs_buffer *b;
	struct emacs_buffer *old = bf_cur;
	int modcnt;

	write_errors = 0;

	b = buffers;
	while( b != 0 && interrupt_key_struck == 0)
		{
		modcnt = b == bf_cur ? bf_modified : b->b_modified;

		if( b->b_checkpointed >= 0
		&& b->b_checkpointed < modcnt )
			{
			set_bfp (b);
			if( b->b_checkpointfn == 0 )
				b->b_checkpointfn =
					savestr( concoct_name(
						(( b->b_fname != 0 ) ?
							b->b_fname : b->b_buf_name),
						ckp_ext));

			write_errors |= write_file(b->b_checkpointfn, CHECKPOINT_WRITE) == 0;

			err = 0;
			b->b_checkpointed = bf_modified = modcnt;
			}

		b = b->b_next;
		}

	set_bfp (old);
	return 0;
	}			/* Of checkpointBuffers */


/* fetch_ the text of a VAX/VMS error code */
#ifdef vms
unsigned char *fetch_os_error ( int error_code )
	{
	struct dsc$descriptor bufd;
	unsigned short int len = MAX_ERROR_MESSAGE;

	if( os_error_buf == 0 )
		if( (os_error_buf = malloc_ustr (MAX_ERROR_MESSAGE)) == 0 )
			return u_str("error detected");

	bufd.dsc$w_length = MAX_ERROR_MESSAGE;
	bufd.dsc$b_dtype = 0;
	bufd.dsc$b_class = 0;
	bufd.dsc$a_pointer = (char *)os_error_buf;

	if( ! VMS_SUCCESS(sys$getmsg( (unsigned int)error_code, &len, &bufd, 1, 0)) )
		{
		sprintfl (os_error_buf, MAX_ERROR_MESSAGE,
			u_str("error %%X%08X detected"), error_code);
		len = _str_len (os_error_buf);
		}

	os_error_buf[len] = 0;

	if( os_error_buf[0] >= 'a' && os_error_buf[0] <= 'z' )
		os_error_buf[0] = os_error_buf[0] + 'A' - 'a';

	return os_error_buf;
	}			/* Of fetch_os_error */
#else
unsigned char *fetch_os_error ( int error_code )
	{
	if( os_error_buf == 0 )
		if( (os_error_buf = malloc_ustr (MAX_ERROR_MESSAGE+1)) == 0 )
			return u_str("Out of memory!");

	if (error_code > sys_nerr)
	    sprintfl (os_error_buf, MAX_ERROR_MESSAGE, u_str("Unknown error %d detected"), error_code);
	else
	    sprintfl (os_error_buf, MAX_ERROR_MESSAGE, u_str("%s"), sys_errlist[error_code]);

	return os_error_buf;
	}			/* Of fetch_os_error */
#endif

int synchronise_files(void)
	{
	struct emacs_buffer *b;
	struct emacs_buffer *old = bf_cur;

	/* force the global buffer data into the emacs_buffer structure */
	set_bfp( old );

	b = buffers;
	while( !err && b != NULL )
	    {
	    set_bfp (b);
	    if( b->b_kind == FILEBUFFER
	    && b->b_fname != NULL )
		{
		time_t new_time;
		int new_access;
#if defined( vms )
		/* for VMS use the  latest version of the file */
		unsigned char fname[256], *p;
		_str_cpy( fname, b->b_fname );
		/* strip the version */
		p = _str_rchr( fname, ';' );
		if( p != NULL ) *p = '\0';
		
#else
		unsigned char *fname = b->b_fname;
#endif

		new_time = fio_file_modify_date( fname );
		new_access = fio_access( fname );

		/* if has been deleted and used to exist */
		if( new_access == 0 && new_access != b->b_synch_file_access )
			{
			int delete_it;

			if( b->b_modified != 0 )
				delete_it = get_yes_or_no
					( 0,
					u_str("The file %s has been delete do you want to delete modified buffer %s?"),
					fname, b->b_buf_name
					);
			else
				delete_it = get_yes_or_no
					( 1,
					u_str("The file %s has been delete do you want to delete buffer %s?"),
					fname, b->b_buf_name
					);
			if( delete_it )
				{
				/* need to save the name as it will free'ed in kill_bfn */
				unsigned char *name = savestr( b->b_buf_name );

				/* find the next before we kill this buffer */
				b = b->b_next;

				/* kill off the buffer */
				kill_bfn( name );

				free( name );

				/* on to the next buffer */
				continue;
				}
			else
				{
				/* update the synch attribute variable */
				b->b_synch_file_access = new_access;
				}
			}

		/* see if the file attributes have changed since the last synch */
		if( b->b_synch_file_time != new_time
		|| b->b_synch_file_access != new_access )
			{
			int read_it;

			if( b->b_modified != 0 )
				read_it = get_yes_or_no
					( 0,
					u_str("For modified buffer %s the file %s has changed do you want to reload it?"),
					b->b_buf_name, fname
					);
			else
				read_it = get_yes_or_no
					( 1,
					u_str("The file %s has changed do you want to reload it?"),
					fname
					);

			if( read_it )
				{
				int old_dot = dot;

				b->b_mode.md_readonly = 0;

				read_file (fname, 1, 0);

				set_dot( old_dot );
				}
			else
				{
				/* update the synch attribute variables */
				b->b_synch_file_time = new_time;
				b->b_synch_file_access = new_access;
				}
			}
		}
	    b = b->b_next;
	    }

	set_bfp (old);

	if( wn_cur != NULL )
		do_dsp( 0 );

	return 0;
	}

/* fileio initialisation */
void init_fio( void )
	{
	int ckp = find_mac( checkpoint_hook );

	if( ckp >= 0 )
		checkpoint_proc = mac_bodies[ckp];

	ask_about_buffer_names = 1;
	}
