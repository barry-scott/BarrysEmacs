/*
 *	Copyright (c) 1987-1993
 *	Barry A. Scott and nick Emery
 */

/*
 *	Emacs file modification journalling
 */
#include <emacs.h>

#ifdef vms
#include <fscndef.h>
#include <descrip.h>
#endif

#ifdef unix
#include <unistd.h>
#include <fcntl.h>
#endif

/*forward*/ int check_journal_frequency( int value, struct variablename *v );
/*forward*/ int check_current_buffer_journal( int value, struct variablename *v );
/*forward*/ void fetch_current_buffer_journal( struct expression *e );

/*forward*/ static void start_journal_timer( void );
/*forward*/ static void stop_journal_timer( void );

/*forward*/ static int journal_start( void );
/*forward*/ static unsigned char *concoct_filename( unsigned char *in );
/*forward*/ void journal_stop( struct emacs_buffer *b );
/*forward*/ void journal_pause( void );
/*forward*/ void journal_flush( void );
/*forward*/ static void flush_journals( void );
/*forward*/ void journal_exit( void );
/*forward*/ void restore_journal( void );

/*forward*/ void journal_insert( int dot, /* Location in buffer */ int len, /* Length of insert */ unsigned char *str/* data to insert */ );
/*forward*/ void journal_delete( int dot, /* Location in buffer */ int len/* length of delete */ );
/*forward*/ static void jnl_write_buffer( struct journal_block *jnl );
/*forward*/ int jnl_write_ast( FILE *rab );
/*forward*/ int recover_journal( void );

#if DBG_JOURNAL
/*forward*/ static int validate_journal_buffer(struct journal_block *jnl);
#endif

GLOBAL SAVRES int journalling_frequency;
GLOBAL SAVRES int journal_scratch_buffers = 1;
GLOBAL SAVRES int animate_journal_recovery;
unsigned char journal_fn[MAXPATHLEN+1];
int journal_records_written;

int check_journal_frequency( int value, struct variablename *PNOTUSED(v) )
	{
#define MIN_FREQ 10
#define MAX_FREQ 210

	if( value != 0
	&& (value < MIN_FREQ || value > MAX_FREQ) )
		{
		error( u_str( "Journal frequency must be 0 or %d to %d seconds"),
			MIN_FREQ, MAX_FREQ );
		return 0;
		}
	if( value == 0 )
		{
		struct emacs_buffer *b;

		journalling_frequency = 0;
		stop_journal_timer();

		b = buffers;
		while( b != 0 )
			{
			if( b->b_journal != 0 )
				journal_stop( b );
			b = b->b_next;
			}
		}
	else
	   {
	   /*
	    *	Only check if journalling is being turn on. Changes
	    *	of timer interval do not require the modified buffer
	    *	checks.
	    */
	   if( journalling_frequency == 0 )
		{
		struct emacs_buffer *b;

		bf_cur->b_modified = bf_modified;	/* make the loop simpler */

		b = buffers;
		while( b != 0 )
			{
			if( b->b_journalling )
			if( b->b_kind == FILEBUFFER && b->b_modified != 0 )
				{
				error( u_str("Journalling cannot be enabled on modified file buffer %s" ),
					b->b_buf_name );
				return 0;
				}
			else if( b->b_size1 + b->b_size2 != 0 )
				{
				error( u_str("Journalling cannot be enabled on non empty buffer %s"),
					b->b_buf_name );
				return 0;
				}
			b = b->b_next;
			}
		}
	    journalling_frequency = value;
	    stop_journal_timer();	/* Stop if its running */
	    start_journal_timer();	/* Start at new interval */
	    }
	return 0;
	}

int check_current_buffer_journal( int value, struct variablename *v )
	{
	if( (value & ~1) != 0 )
		{
		error( u_str("Boolean value expected") );
		return 0;
		}
	if( value )
		{
		if( ! bf_cur->b_journalling
		&& bf_modified != 0 )
			{
			error( u_str("Journalling cannot be enabled on a modified buffer - write out this buffer" ));
			return 0;
			}
		bf_cur->b_journalling = 1;
		}
	else
		{
		bf_cur->b_journalling = 0;
		journal_stop( bf_cur );
		}

	cant_1line_opt = redo_modes = 1;

	return 0;
	}

void fetch_current_buffer_journal( struct expression *e )
	{
	*e->exp_v.v_value = bf_cur->b_journalling;
	}

/* Work routine for journal time outs */
static void work_flush_journal( struct emacs_work_item *PNOTUSED(work) )
	{
	set_activity_character( 'j' );
	journal_flush();
	set_activity_character( 'b' );
	}

/* work structure to queue on a timeout */
static struct emacs_work_item journal_flush_request =
	{
	{ NULL, NULL },
	work_flush_journal, 0, NULL
	};

static void journal_timeout( void *PNOTUSED(param) )
	{
#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		{
		time_t now = time(0);
		printf("journal_timeout %s", ctime( &now ) );
		}
#endif

	work_add_item( &journal_flush_request );
	}

static void start_journal_timer( void )
	{
	if( journalling_frequency == 0 )
		return;
	
#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		{
		time_t now = time(0);
		printf("start_journal_timer %s\n", ctime( &now ) );
		}
#endif

	time_add_request( journalling_frequency*1000, 'J', journal_timeout, NULL );
	}

static void stop_journal_timer( void )
	{
	time_remove_requests( 'J' );
#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		{
		time_t now = time(0);
		printf("stop_journal_timer %s\n", ctime( &now ) );
		}
#endif
	}

static int journal_start( void )
	{
	struct journal_block *jnl;
	union journal_record *open_record;
	unsigned char *p;
	int i;

	/*
	 *	First confirm that journalling is allowed on this buffer
	 */
	if( journalling_frequency == 0		/* globally disabled */
	|| ! bf_cur->b_journalling		/* locally disabled */
	/*
	 * This next test is a result of a the way that visit-file does a replace
	 */
	|| (bf_cur->b_kind == FILEBUFFER && bf_cur->b_fname == 0)
	/*
	 * This next test enforces the scratch buffer journaling restriction
	 */
	|| (bf_cur->b_kind == SCRATCHBUFFER && !journal_scratch_buffers))
		return 0;
	if( bf_cur->b_journal != 0	/* already started */ )
		return 1;
	if( bf_modified != 0 )
		{
		error( u_str( "Cannot start a journal on modified buffer %s"), bf_cur->b_buf_name );
		return 0;
		}

	/*
	 *	Allocate all the blocks required in one go
	 */
	jnl = malloc_struct( journal_block );
	if( jnl == 0 )
		goto journal_errors;

	memset( jnl, 0, sizeof( struct journal_block ) );
	open_record = jnl->jnl_buf = &jnl->jnl_buf1[0];
	jnl->jnl_active = 1;
	jnl->jnl_open = 1;
	jnl->jnl_buf1_current = 1;

#ifdef vms
	/*
	 *	If this is a file buffer use the filename to
	 *	to make the journal file name. Otherwise use the
	 *	buffer name to make a name from.
	 */
	{
	unsigned char def_name_buf[MAXPATHLEN+1];

	if( bf_cur->b_kind == FILEBUFFER )
		{
		struct dsc$descriptor src;
		struct fscn_def items[2];

		memset( items, 0, sizeof( items ) );
		items[0].w_item_code = FSCN$_TYPE;

		DSC_SZ( src, bf_cur->b_fname );

		sys$filescan( &src, items, 0 );

		_str_cpy( def_name_buf, "emacs$journal:" );
		_str_ncat( def_name_buf, items[0].a_addr, min(39-14,items[0].w_length) );
		_str_cat( def_name_buf, "_emacs_journal;" );

		p = bf_cur->b_fname;
		open_record->jnl_open.jnl_type = JNL_FILENAME;
		}
	else
		{
		_str_cpy( def_name_buf, "emacs$journal:.buffer_emacs_journal" );

		p = concoct_filename( bf_cur->b_buf_name );
		open_record->jnl_open.jnl_type = JNL_BUFFERNAME;
		}

	expand_and_default( def_name_buf, p, jnl->jnl_jname );
	*_str_rchr( jnl->jnl_jname, ';' ) = '\0';
	}
#else
	/*
	 *	Create a unique journal file
	 */
	for( i='a';; i++ )
		{
		unsigned char def_name_buf[MAXPATHLEN+1];
		FILE *file;

		/*
		 *	If this is a file buffer use the filename to
		 *	to make the journal file name. Otherwise use the
		 *	buffer name to make a name from.
		 */
		if( bf_cur->b_kind == FILEBUFFER )
			{
			_str_cpy( def_name_buf, "emacs$journal:.ej_" );
			def_name_buf[_str_len(def_name_buf)-1] = (unsigned char)i;

			p = bf_cur->b_fname;
			open_record->jnl_open.jnl_type = JNL_FILENAME;
			}
		else
			{
			_str_cpy( def_name_buf, "emacs$journal:.ej_" );
			def_name_buf[_str_len(def_name_buf)-1] = (unsigned char)i;

			p = concoct_filename( bf_cur->b_buf_name );
			open_record->jnl_open.jnl_type = JNL_BUFFERNAME;
			}

		expand_and_default( def_name_buf, p, jnl->jnl_jname );
		
		/* see if this file exist */
		file = fopen( s_str(jnl->jnl_jname), "r" );

		/* no then we have the file name we need */
		if( file == NULL )
			break;

		/* close and loop around */
		fclose( file );

		if( i >= 'z' )
			{
			error( u_str("Unable to create a unique journal filename tried %s last"),
				jnl->jnl_jname );
			goto journal_errors;
			}
		}
#endif

	/* w - write, b - binary, c - commit */
	jnl->jnl_file = fopen( s_str(jnl->jnl_jname), "w" BINARY_MODE COMMIT_MODE );
	if( jnl->jnl_file == NULL )
		goto journal_errors;

	/*
	 *	setup the open record into the journal
	 */
	open_record[0].jnl_open.jnl_version = JNL_VERSION;
	open_record[0].jnl_open.jnl_name_length = _str_len( p )+1;
	_str_cpy( open_record[1].jnl_data.jnl_chars, p );

	jnl->jnl_used = 1 + JNL_BYTE_TO_REC(open_record[0].jnl_open.jnl_name_length);
	jnl->jnl_record = 0;
	bf_cur->b_journal = jnl;

	return 1;

journal_errors:
	/*
	 *	error recovery
	 */
	if( jnl != 0 )
		{
		if( jnl->jnl_file )
			fclose( jnl->jnl_file );
		free( jnl );
		}

	error( u_str("Unable to start journalling for buffer %s code %x"),
			bf_cur->b_buf_name, errno );
	return 0;
	}

static unsigned char *concoct_filename( unsigned char *in )
	{
#define FILE_NAME_SIZE 31
	static unsigned char out[FILE_NAME_SIZE+1];
	unsigned char ch;
	int i;

	for( i=0; i<=FILE_NAME_SIZE-1; i += 1 )
		{
		ch = in[i];
		if( ('a' <= ch && ch <= 'z')
		|| ('A' <= ch && ch <= 'Z')
		|| ('0' <= ch && ch <= '9')
		|| ch == '$' || ch == '-' || ch == '_' )
			out[i] = ch;
		else if( ch == 0 )
			{
			out[i] = 0;
			return out;
			}
		else
			out[i] = '_';
		}
	out[FILE_NAME_SIZE] = 0;
	return out;
	}

void journal_stop( struct emacs_buffer *b )
	{
	struct journal_block *jnl;

	jnl = b->b_journal;
	if( jnl == 0 )
		return;

	/* close the file */
	if( jnl->jnl_file )
		fclose( jnl->jnl_file );

	/* and delete it as its not needed */
	if( jnl->jnl_jname[0] != '\0' )
		remove( s_str(jnl->jnl_jname) );

	/* free the memory */
	free( jnl );

	b->b_journal = 0;
	}

void journal_pause( void )
	{
	stop_journal_timer();
	flush_journals();
	}

void journal_flush( void )
	{
	flush_journals();
	start_journal_timer();
	}

static void flush_journals( void )
	{
	struct emacs_buffer *b;

#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		{
		time_t now = time(0);
		printf("flush_journals %s", ctime( &now ) );
		}
#endif

	b = buffers;
	while( b != 0 )
		{
		struct journal_block *jnl;

		if( (jnl = b->b_journal) != NULL )
			{
			jnl->jnl_flush = 1;
			jnl_write_buffer( jnl );
			}
		b = b->b_next;
		}
	}

/*
 *	Call this routine when Emacs exits to get rid of all the
 *	out journal files.
 */
void journal_exit( void )
	{
	struct emacs_buffer *b;

	stop_journal_timer();

	b = buffers;
	while( b != 0 )
		{
		if( b->b_journal != 0 )
			journal_stop( b );
		b = b->b_next;
		}
	}

void restore_journal( void )
	{
	struct emacs_buffer *b;

	b = buffers;
	while( b != 0 )
		{
		if( b->b_journal != 0 )
			journal_stop( b );
		b = b->b_next;
		}
	stop_journal_timer();	/* Stop if its running */
	start_journal_timer();	/* Start at new interval */
	}

void journal_insert
	(
	int dot,				/* Location in buffer */
	int len,				/* Length of insert */
	unsigned char *str			/* data to insert */
	)
	{
	union journal_record *in_rec;
	int free;
	int writing;
	int written;
	struct journal_block *jnl;

	jnl = bf_cur->b_journal;
	if( jnl == 0 )
		if( ! journal_start() )
			return;
		else
			jnl = bf_cur->b_journal;

#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		validate_journal_buffer( jnl );
#endif
	written = 0;

	in_rec = &jnl->jnl_buf[jnl->jnl_record];
	if( in_rec->jnl_insert.jnl_type == JNL_INSERT
	&& in_rec->jnl_insert.jnl_dot + in_rec->jnl_insert.jnl_insert_length == dot
	&& (free = JNL_BUF_SIZE - jnl->jnl_used) > 0 )
		{
		writing = min( len, free*(int)JNL_BYTE_SIZE );
		memcpy
		(
		&in_rec[1].jnl_data.jnl_chars[in_rec->jnl_insert.jnl_insert_length],
		&str[ written ],
		writing
		);
		in_rec->jnl_insert.jnl_insert_length += writing;
		written = writing;
		jnl->jnl_used = jnl->jnl_record + 1 +
				JNL_BYTE_TO_REC( in_rec->jnl_insert.jnl_insert_length );
		}

	while( written < len )
		{
		/*
		 *	See if there is room in the buffer for the
		 *	first byte of the insert. We need at least
		 *	two slots. One for the insert record and
		 *	one for the bytes of data.
		 */
		free = JNL_BUF_SIZE - jnl->jnl_used - 1;
		if( free < 1 )
			{
			jnl_write_buffer( jnl );
			free = JNL_BUF_SIZE - jnl->jnl_used - 1;
			}
		in_rec = &jnl->jnl_buf[jnl->jnl_used];

		writing = min( len - written, free*(int)JNL_BYTE_SIZE );
		in_rec->jnl_insert.jnl_type = JNL_INSERT;
		in_rec->jnl_insert.jnl_dot = dot + written;
		in_rec->jnl_insert.jnl_insert_length = writing;

		memcpy( &in_rec[1].jnl_data.jnl_chars[0], &str[ written ], writing );

		written += writing;
		jnl->jnl_record = jnl->jnl_used;
		jnl->jnl_used += 1 + JNL_BYTE_TO_REC( writing );
		}
#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		validate_journal_buffer( jnl );
#endif
	}

static void find_previous_record( void )
	{
	union journal_record *rec;
	struct journal_block *jnl;
	int offset = 0;

	jnl = bf_cur->b_journal;
	rec = &jnl->jnl_buf[ jnl->jnl_record ];
	rec->jnl_open.jnl_type = JNL_END;
	
	while( offset < jnl->jnl_record )
		{
		int add_to_offset = 0;

		rec = &jnl->jnl_buf[offset];
 
		switch( rec->jnl_open.jnl_type )
		{
		case JNL_FILENAME:
			add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
			break;

		case JNL_BUFFERNAME:
			add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
			break;

		case JNL_INSERT:
			add_to_offset = 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
			break;

		case JNL_DELETE:
			add_to_offset = 1;
			break;

		case JNL_END:
			goto exit_loop;

		default:
			_dbg_msg( u_str("Journal find_prev loop unexpected type(0x%x): Type: %x\n"),
					offset,
					rec->jnl_open.jnl_type );
			invoke_debug();

			/* patch the bad record out of the buffer */
			rec->jnl_open.jnl_type = JNL_END;
			goto exit_loop;
		}
		if( offset + add_to_offset >= jnl->jnl_record )
			break;

		offset += add_to_offset;
		}
exit_loop:
	jnl->jnl_record = offset;
	jnl->jnl_used = offset;

	switch( rec->jnl_open.jnl_type )
	{
	case JNL_FILENAME:
		jnl->jnl_used = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
		break;

	case JNL_BUFFERNAME:
		jnl->jnl_used = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
		break;

	case JNL_INSERT:
		jnl->jnl_used += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
		break;

	case JNL_DELETE:
		jnl->jnl_used += 1;
		break;

	case JNL_END:
		break;

	default:
		_dbg_msg( u_str("Journal find prev switch unexpected type(0x%x): Type: %x\n"),
				offset,
				rec->jnl_open.jnl_type );
		invoke_debug();
		/* patch the bad record out of the buffer */
		rec->jnl_open.jnl_type = JNL_END;
		break;
	}

	}

void journal_delete
	(
	int dot,				/* Location in buffer */
	int len					/* length of delete */
	)
	{
	union journal_record *del_rec;
	struct journal_block *jnl;

	jnl = bf_cur->b_journal;
	if( jnl == 0 )
		if( ! journal_start() )
			return;
		else
			jnl = bf_cur->b_journal;

#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		validate_journal_buffer( jnl );
#endif

	/*
	 *	See if this delete merges with the last record
	 */
	del_rec = &jnl->jnl_buf[jnl->jnl_record];

	switch( del_rec->jnl_open.jnl_type)
	{
	case JNL_DELETE:
		/*
		 *	See if this is a contiguous delete forward
		 */
		if( del_rec->jnl_delete.jnl_del_dot == dot )
			{
			del_rec->jnl_delete.jnl_length += len;
#if DBG_JOURNAL
			if( dbg_flags&DBG_JOURNAL )
				validate_journal_buffer( jnl );
#endif
			return;
			}
		/*
		 *	See if this is a contiguous delete backward
		 */
		if( del_rec->jnl_delete.jnl_del_dot == dot + len )
			{
			del_rec->jnl_delete.jnl_del_dot = dot;
			del_rec->jnl_delete.jnl_length += len;
#if DBG_JOURNAL
			if( dbg_flags&DBG_JOURNAL )
				validate_journal_buffer( jnl );
#endif
			return;
			}
		break;

	case JNL_INSERT:
		/*
		 *	See if this is a delete from the end of the
		 *	last insert
		 */
		if( del_rec->jnl_insert.jnl_dot + del_rec->jnl_insert.jnl_insert_length ==
			dot + len )
			{
			/* this is delete that is shorter then the insert */
			if( del_rec->jnl_insert.jnl_insert_length > len )
				{
				del_rec->jnl_insert.jnl_insert_length -= len;
				jnl->jnl_used = jnl->jnl_record + 1 +
					JNL_BYTE_TO_REC( del_rec->jnl_insert.jnl_insert_length );
#if DBG_JOURNAL
				if( dbg_flags&DBG_JOURNAL )
					validate_journal_buffer( jnl );
#endif
				return;
				}
			/* This is an insert that is the same size as the delete.
			 * Make this insert record go away and backup in the buffer
			 */
			if( del_rec->jnl_insert.jnl_insert_length == len )
				{
				del_rec->jnl_open.jnl_type = JNL_END;
				find_previous_record();
#if DBG_JOURNAL
				if( dbg_flags&DBG_JOURNAL )
					validate_journal_buffer( jnl );
#endif
				return;
				}
			/* This is a delete that is longer then the insert.
			 * Remove this insert record and issue a delete for
			 * the remaining length
			 */
			find_previous_record();
			journal_delete( dot, len - del_rec->jnl_insert.jnl_insert_length );
#if DBG_JOURNAL
			if( dbg_flags&DBG_JOURNAL )
				validate_journal_buffer( jnl );
#endif
			return;
			}
		break;
	default:
		break;
	}

	/*
	 *	See if there is room in the buffer for a
	 *	delete record
	 */
	if( JNL_BUF_SIZE - jnl->jnl_used < 1 )
		jnl_write_buffer( jnl );

	del_rec = &jnl->jnl_buf[jnl->jnl_used];

	del_rec->jnl_delete.jnl_type = JNL_DELETE;
	del_rec->jnl_delete.jnl_del_dot = dot;
	del_rec->jnl_delete.jnl_length = len;

	jnl->jnl_record = jnl->jnl_used;
	jnl->jnl_used += 1;

#if DBG_JOURNAL
	if( dbg_flags&DBG_JOURNAL )
		validate_journal_buffer( jnl );
#endif
	return;
	}

static void jnl_write_buffer( struct journal_block *jnl )
	{
	union journal_record *buf;
	int status;

	if( jnl->jnl_file == NULL )
		return;

	buf = jnl->jnl_buf;

	if( jnl->jnl_used == 0
	&& jnl->jnl_flush )
		{
		/* flush the buffers */
		fflush( jnl->jnl_file );
		/* update the file info on disk */
		close( dup( fileno( jnl->jnl_file ) ) );
		return;
		}

	/*
	 *	Tack on an END record if the buffer is not full
	 */
	if( jnl->jnl_used != JNL_BUF_SIZE )
		buf[ jnl->jnl_used ].jnl_insert.jnl_type = JNL_END;

	/*
	 *	See if a flush is required after this write
	 */
	journal_records_written++;

	/*
	 *	Write the journal records. The write_ast routine will
	 *	check_ the flush flag.
	 */
	status = fwrite( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, jnl->jnl_file );
	if( status != JNL_BUF_SIZE )
		{
		error( u_str("error writing journal for %s status code %x"),
			bf_cur->b_buf_name, errno );
		return;
		}

	/*
	 *	flush the buffers to the disk
	 */
	/* flush the buffers */
	fflush( jnl->jnl_file );
	/* update the file info on disk */
	close( dup( fileno( jnl->jnl_file ) ) );

#if DBG_JOURNAL
	jnl->jnl_used = JNL_BUF_SIZE;
	if( !validate_journal_buffer( jnl ) )
		{
		/* close the file */
		fclose( jnl->jnl_file );
		jnl->jnl_file = NULL;
		_str_cpy( jnl->jnl_jname, "" );
		}
#endif

	/*
	 *	Switch to the other buffer
	 */
	jnl->jnl_used = 0;
	jnl->jnl_record = 0;
	if( jnl->jnl_buf1_current )
		{
		jnl->jnl_buf1_current = 0;
		buf = jnl->jnl_buf2;
		}
	else
		{
		jnl->jnl_buf1_current = 1;
		buf = jnl->jnl_buf1;
		}
	jnl->jnl_buf = buf;

	/*
	 *	initialise the buffer to a known good state, empty
	 */
	memset( buf, JNL_END, sizeof( jnl->jnl_buf1 ) );
	}

int recover_journal( void )
	{
	int status;
	FILE *file;
	union journal_record buf[JNL_BUF_SIZE];
	int offset;
	union journal_record *rec;
	unsigned char *journal_file;
	unsigned char journal_filename[MAXPATHLEN+1];

	journal_file = getescfile (u_str("Journal file: "), NULL, 0, 1);
	if( journal_file == 0
	|| journal_file[0] == 0 )
		return 0;

	file = fopen( s_str(journal_file), "r" BINARY_MODE );
	if( file == NULL )
		{
		error( u_str("Cannot open journal file %s"), journal_file );
		return 0;
		}

	/*
	 *	Read the first record from the journal.
	 *	It will be the file or buffer that is
	 *	to be recovered.
	 */
	offset = 0;
	status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );
	if( status == 0 || feof( file ) || ferror( file ) )
		{
		error( u_str("Unable to read the first record from the journal") );
		goto journal_recover;
		}

	/*
	 *	Turn into an asciz string
	 */
	rec = &buf[offset];

	if( rec->jnl_open.jnl_version != JNL_VERSION )
		{
		error( u_str("This Emacs support journal version %d but this journal file is version %d"),
			JNL_VERSION, rec->jnl_open.jnl_version );
		goto journal_recover;
		}

	switch( rec->jnl_open.jnl_type )
	{
	case JNL_FILENAME:
		_str_cpy( journal_filename, rec[1].jnl_data.jnl_chars );
#ifdef vms
		{
		struct dsc$descriptor src;
		unsigned int version1;
		unsigned int version2;
		int i;
		struct fscn_def items[2];

		memset( items, 0, sizeof( items ) );
		items[0].w_item_code = FSCN$_VERSION;

		DSC_SZ( src, journal_filename );

		sys$filescan( &src, items, 0 );
		version1 = 0;
		for( i=1; i<=items[0].w_length - 1; i++ )
			version1 = version1*10 +
				items[0].a_addr[i] - '0';

		visit_file( u_str(";0"), 1, 1, &journal_filename[0] );

		DSC_SZ( src, bf_cur->b_fname );

		sys$filescan( &src, items, 0 );
		version2 = 0;
		for( i=1; i<=items[0].w_length - 1; i++ )
			version2 = version2*10 +
				items[0].a_addr[i] - '0';

		if( version1 != version2 )
			{
			error( u_str("File to be recovered has been superseded by %s"),
				bf_cur->b_fname );
			goto journal_recover;
			}

		}
#else
		visit_file( journal_filename, 1, 1, u_str("") );
#endif
		offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
		break;

	case JNL_BUFFERNAME:
		_str_cpy( journal_filename, rec[1].jnl_data.jnl_chars );

		if( find_bf( journal_filename ) != 0 )
			{
			error( u_str("Buffer already exists %s"), journal_filename );
			goto journal_recover;
			}
		set_bfn( journal_filename );
		window_on( bf_cur );
		offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
		break;

	default:
		{
		error( u_str("Format error in journal - first record is type %d"),
			rec->jnl_open.jnl_type );
		goto journal_recover;
		}
	}

	/*
	 *	Read all the blocks in the journal file
	 */
	do
		{
		/*
		 *	Action all the records in the block
		 */
		while( offset < JNL_BUF_SIZE )
			{
			if( animate_journal_recovery )
				sit_for_inner( 0 );

			rec = &buf[offset];
			switch( rec->jnl_open.jnl_type )
			{
			case JNL_INSERT:
				set_dot( rec->jnl_insert.jnl_dot );
				ins_cstr
				(
				rec[1].jnl_data.jnl_chars,
				rec->jnl_insert.jnl_insert_length
				);
				offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
				break;

			case JNL_DELETE:
				del_frwd
				(
				rec->jnl_delete.jnl_del_dot,
				rec->jnl_delete.jnl_length
				);
				offset += 1;
				break;

			case JNL_END:
				goto exit_loop;
			default:
				error( u_str("Unexpected type of journal record %d encountered"),
					rec->jnl_open.jnl_type );
				goto journal_recover;
			}
			}
exit_loop:
		offset = 0;
		status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );
		}
	while( !(status != JNL_BUF_SIZE || feof( file ) || ferror( file )) );

	message( u_str("Recovery of %s completed"), journal_filename );

journal_recover:
	/*
	 *	Tidy up and exit
	 */
	fclose( file );

	return 0;
	}

#if DBG_JOURNAL
static int validate_journal_buffer(struct journal_block *jnl)
	{
	union journal_record *rec;
	int offset = 0;

	while( offset < jnl->jnl_used )
		{
		rec = &jnl->jnl_buf[offset];

		switch( rec->jnl_open.jnl_type )
		{
		case JNL_FILENAME:
			offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
			break;

		case JNL_BUFFERNAME:
			offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
			break;

		case JNL_INSERT:
			offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
			break;

		case JNL_DELETE:
			offset += 1;
			break;

		case JNL_END:
			goto exit_loop;

		default:
			_dbg_msg( u_str("Journal validate unexpected type(0x%x): Type: %x\n"),
					offset,
					rec->jnl_open.jnl_type );
			invoke_debug();
			return 0;
		}
		}
exit_loop:
	return 1;
	}

#endif
