/* prompt - Routines to prompt in the Mini-buffer and collect input */

/*
 *	COPYRIGHT (c© 1985 BY Nick W. Emery and Barry Scott
 */

/*
 * FACILITY:
 *	VAX/VMS EMACS, prompt facility
 *
 * ABSTRACT:
 *	This module contains routines that collect input prompted for
 *	in the Mini-buffer.
 *
 */


/* "Declarations" */

/*
 * SWITCHES:
 *
 *	NONE
 *
 * INCLUDE FILES:
 */
#include <emacs.h>

/*
 * TABLE OF CONTENTS:
 */
/*forward*/ int getword( unsigned char * *table_ref, unsigned char *s, ... );	/* fetch a completed word */
/*forward*/ static unsigned char *getnsword( unsigned char * *table, unsigned char ** (*table_maker)( unsigned char *, int * ), unsigned char *, va_list *argp );	/* fetch a non-space fill word */
/*forward*/ unsigned char *getescword( unsigned char * *table_ref, unsigned char *s, ... );	/* fetch an escape expanded word */
/*forward*/ unsigned char *getescfile( unsigned char *fmt, va_list *argp, int dirs, int vers );		/* fetch an expanded filename */
/*forward*/ static unsigned char ** get_file_list( unsigned char *file, int *dir_spec_len );	/* Make a filename table */
/*forward*/ unsigned char *getescdb( struct dbsearch *current_dbs, unsigned char *fmt, ... );	/* fetch an expanded DB searchlist key */
#ifdef DB
/*forward*/ static unsigned char ** globdb( unsigned char *key, int *ignore_len );	/* Make a key table */
/*forward*/ static int globdb_helper( int keylen, unsigned char *key, unsigned char * *rfs );	/* Make the key table */
#endif

/*
 * EQUATED SYMBOLS:
 */

#define lf 10				/* ASCII code for newline */
#define PREFIX_BUF_SIZE 300		/* Size of most buffers */

/*
 * OWN STORAGE:
 */

static struct dbsearch *dbs;		/* Database search list in use */
static int dbs_count;			/* Database counter */
static int savedlen = 0;		/* Length of current table string */
static unsigned char savedstr[MAXPATHLEN+1];	/* current table string */

static int glob_flag;			/* Version flag for get_file_list */
static int glob_first;			/* filename cache purge flag for get_file_list */

/*
 * EXTERNAL REFERENCES:
 */

GLOBAL SAVRES int ignore_version_numbers = 0;
GLOBAL SAVRES unsigned char *vecfiles [MAXFILES + 3];
GLOBAL SAVRES unsigned char *vec_db [MAXFILES + 3];
GLOBAL int expand;
GLOBAL int help;

extern unsigned char last_key_struck;	/* The last character struck */
extern int remove_help_window;		/* True if help windows removed after use */
extern int auto_help;			/* True if help automatically pops up on error */



/* "getword - Prompt for and get a completed word" */
int getword ( unsigned char * *table_ref, unsigned char *fmt, ... )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	Thie routine is used to fetch a words from the user. It
 *	prompts in the Mini-buffer with the string s, and expands and
 *	completes one of the words in the table passed.
 *
 * FORMAL PARAMETERS:
 *
 *	table_ref is the address of a location containing the address
 *	of the table
 *
 *	s is the prompt string material
 *
 * IMPLICIT INPUTS:
 *
 *	bf_cur is used to find the current buffer
 *
 *	wn_cur is used to find the current window
 *
 *	remove_help_window is used to decide what to do with help
 *	windows on exit.
 *
 *	cur_exec is used to decide if the call is interactive or not.
 *
 *	auto_help is used to decide if a help window should be popped
 *	upon errors.
 *
 *	bf_modified is used to set buffers as unmodified
 *
 * IMPLICIT OUTPUTS:
 *
 *	NONE
 *
 * ROUTINE VALUE:
 *
 *	The table index of thematches string is returned, or
 *	-1 to indicate that the operation as aborted
 *
 * SIDE EFFECTS:
 *
 *	help windows may be popped up. The keyboard processor is
 *	called, so anything could happen!
 */
	{
	unsigned char *endp;	/* End of string pointer */
	int maxok;		/* max legal chars from prefix */
	int matched;		/* Chars that didnt match */
	int len;		/* Length of current input */
	unsigned char * *table;	/* Pointer to table of strings */
	unsigned char *current_string;/* pointer to current word */

	int p;			/* The loop counter to scan table */
	int bestp = -1;		/* The best version of p */
	int nfound;		/* Number of matching words found */
	struct marker *old = 0;	/* old position of dot */

	unsigned char prefix[PREFIX_BUF_SIZE+1];/* the current prefix string */

	unsigned char *ptr;	/* A pointer to the current table entry */
	int side;
	int saved_windows = 0;	/* true if windows should be saved */

	int old_expand = expand;	/* Saved value of expand flag */
	int old_help = help;		/* Saved value of help flag */

	int longest_string;

	va_list argp;

	va_start( argp, fmt );

	/*
	 * Get words from the user until one is found to match
	 */
	prefix[0] = 0;
	prefix[PREFIX_BUF_SIZE] = 0;
	table = table_ref;

	/* find the longest string for making a nice display */
	for( longest_string=0, p=0; table[p] != NULL; p++ )
		{
		len = _str_len( table[p] );
		longest_string = max( longest_string, len );
		}
	longest_string += 2;

	for(;;)
		{
		help = expand = 0;
		nfound = 0;
		maxok = 0;
		current_string = br_getstr (1, prefix, fmt, &argp );
		prefix[0] = 0;
		if( current_string == 0 )
				break;
		
		len = _str_len (current_string);
		if( help == 0 )
			{
			/*
			 * Scan the table to see what matches we have.
			 */
			p = 0;
			while( (ptr = table[p]) != 0 )
				{
				unsigned char *src2;

				src2 = ptr;
				matched = len;
				endp = current_string;

				while( matched > 0
				&& endp[0] == src2[0] )
					{
					matched--;
					endp++;
					src2++;
						}
				if( matched != 0 )
					{
					if( (endp - current_string) > maxok )
						maxok = endp - current_string;
					}
				else
					{
					/*
					 * We have found a match. If its the first, then copy the
					 * match as the next prefix string.
					 * If its not the first, then shorten the prefix string to
					 * the bit that unambiguous
					 */

					nfound++;
					if( len > maxok ) maxok = len;
					if( nfound == 1 )
						memcpy( prefix, ptr, min (_str_len (ptr) + 1, PREFIX_BUF_SIZE) );
					else
						{
						int len;
						len = 0;

						while( len < PREFIX_BUF_SIZE
						&& prefix[len] == ptr[len] )
							len++;

						prefix[len] = 0;
						}
					bestp = p;
					if( ptr [len] == 0 )
						/*
						 * An exact match has been found, so lets exit
						 */
						{
						nfound = 1;
						break;
						}
					}
				p++;
				}
			}
		/*
		 * We have search all of the items specified. If we only have one item,
		 * then lets get and return it. Otherwise we will have to show some help
		 * or worse.
		 */
		if( nfound == 1 )
			break;

		bestp = -1;
		if( nfound <= 1
		|| _str_ncmp( prefix, current_string, len + 1 ) == 0)
			{
			/*
			 * The input string is now so way off that no prefix strings matches
			 * or the string is wrong in the later sections. Either way, the poor
			 * user needs some help
			 */
			if( ! interactive )
				{
				/*
				 * The error occured non-interactively, report the error
				 * directly and exit
				 */

				bestp = -1;
				error( u_str("\"%s\" %s"), current_string,
					nfound == 0 ?
						"doesn't make any sense to me."
					:
						"is ambiguous." );
				break;
				}
			if( auto_help == 0 && help == 0 )
				{
				/*
				 * The user recons to be a wizard, and has help-on-command-completion-error
				 * switches of, and he didnt ask for help. Just bell at him, and
				 * remove all the erroneous characters.
				 */
				ding ();

				if( nfound == 0 )
					{
					memcpy( prefix, current_string, maxok );

					prefix[maxok] = 0;
					}
				}
			else
				{
				/*
				 * Pop up a help window, and fill it full of stuff for the
				 * user to see.
				 */
				unsigned char *msg;

				if( old == 0 )
					{
					old = new_mark ();
					set_mark (old, bf_cur, 1, 0);
					if( remove_help_window != 0 && cur_exec == 0 )
						{
						saved_windows = 1;
						push_window_ring ();
						}
					}
				scratch_bfn( u_str( "Help" ), 1 );

				/*
				 * Work out what to prompt with next time round, and what
				 * message to spit at the user in the help buffer
				 */

				if( help != 0 )
					{
					memcpy(prefix, current_string, len+1);
					msg = u_str("Choose one of the following:\n");
					}
				else
					if( nfound > 1 )
						msg = u_str("Ambiguous, choose one of the following:\n");
					else
						{
						len = 0;
						msg = u_str("Please use one of the following words:\n");
						}
				ins_str (msg);

				/*
				 * Generate the help buffer full of suitable strings
				 */
				side = 2;	/* number of columns to test for room for */
				for( p=0; table[p] != NULL; p++ )
					{
					if( len <= 0
					|| _str_ncmp( table[p], current_string, len) == 0)
						{
						unsigned char buf[PREFIX_BUF_SIZE + 1];

						/* make sure the help fits in the windows */
						if( side*longest_string <= wn_cur->w_width )
							{
							/* there is room for this one and the next at the end */
							side++;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%-*s "), longest_string, table[p]);
							}
						else
							{
							/* room for just this one */
							side = 2;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%s\n"), table[p]);
							}
						ins_str (buf);
						}
					}
				/*
				 * Now that the help buffer is full of useful information,
				 * display it to the user
				 */
				beginning_of_file ();
				bf_modified = 0;
				}
			}
		}

	/*
	 * Everything is now complete, one way or another. So, lets tidy up any
	 * mess we made of the screen, spit out the text of the word chosen and
	 * get on with it
	 */
	if( old != NULL && old->m_buf != NULL )
		window_on (old->m_buf);

	if( saved_windows )
		pop_window_ring ();
	if( old != NULL )
		dest_mark (old);
	help = old_help;
	expand = old_expand;
	if( interactive && bestp >= 0 )
		{
		va_list argp;
		int i;

		va_start( argp, fmt );

		i = do_print( fmt, &argp, prefix, PREFIX_BUF_SIZE );
		prefix[i] = 0;

		message (u_str("%s%s"), prefix, table[bestp]);
		}
	return bestp;
	}				/* Of routine getword */


/* "getnsword - Get a word without space or tab filling" */
static unsigned char *getnsword
		(
		unsigned char **table,
		unsigned char ** (*table_maker)( unsigned char *, int * ),
		unsigned char *fmt,
		va_list *argp
		)
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine is used to fetch a completed word from the
 *	minibuffer after prompting with the supplied strings.
 *	table is the list of acceptable words. If it is zero,
 *	then table_maker is called as a routine to build the table,
 *	and the result is used as the table of words
 *
 * FORMAL PARAMETERS:
 *
 *	table is the address of the table of
 *	strings against which to match.
 *
 *	table_maker make the table
 *
 *	s is the address of the prompt string format
 *
 * IMPLICIT INPUTS:
 *
 *	bf_cur is used to find the current buffer
 *
 *	wn_cur is used to find the current window
 *
 *	remove_help_window is used to decide what to do with help
 *	windows on exit.
 *
 *	cur_exec is used to decide if the call is interactive or not.
 *
 *	auto_help is used to decide if a help window should be popped
 *	upon errors.
 *
 *	bf_modified is used to set buffers as unmodified
 *
 * IMPLICIT OUTPUTS:
 *
 *	NONE
 *
 * ROUTINE VALUE:
 *
 *	0 to indicate an error, or the address of the selected string
 *
 * SIDE EFFECTS:
 *
 *	NONE
 */
	{
	unsigned char *endp;		/* End of string pointer */
	int maxok;			/* max legal chars from prefix */
	int matched;			/* Chars that didnt match */
	int len;			/* Length of current input */
	unsigned char *current_string;	/* pointer to current word */

	int p;				/* The loop counter to scan table */
	int bestp = -1;			/* The best version of p */
	int nfound;			/* Number of matching words found */
	int force_exit = 0;		/* A loop counter */
	struct marker *old = 0;		/* old position of dot */

	unsigned char prefix[PREFIX_BUF_SIZE+1]; /* the current prefix string */

	unsigned char *ptr;		/* A pointer to the current table entry */
	int side;
	int saved_windows = 0;		/* true if windows should be saved */

	int old_expand = expand;	/* Saved value of expand flag */
	int old_help = help;		/* Saved value of help flag */

	int longest_string;

	/* find the longest string for making a nice display */
	for( longest_string=0, p=0; table != NULL && table[p] != NULL; p++ )
		{
		len = _str_len( table[p] );
		longest_string = max( longest_string, len );
		}
	longest_string += 2;

	/*
	 * Get words from the user until one is found to match
	 */
	prefix[0] = 0;
	prefix[PREFIX_BUF_SIZE] = 0;
	for(;;)
		{
		help = expand = 0;
		nfound = 0;
		maxok = 0;
		current_string = br_getstr (1, prefix, fmt, argp );
		prefix[0] = 0;
		if( current_string == 0 )
			break;

		/*
		 * Deal with expand-and-exit cases and non-interactive cases. These
		 * just return the typed value
		 */
		if( cur_exec != NULL || (!expand && !help) )
			{
			force_exit = 1;
			break;
			}

		/*
		 * Make the table if we have to make tables more than once
		 * [Note that the current_string is written back into]
		 */
		if( table_maker != NULL )
			{
			table = table_maker( current_string, &longest_string );
			longest_string +=2;
			}

		len = _str_len( current_string );

		/*
		 * expand the word as the user requested
		 */
		if( expand )
			{
			/*
			 * Scan the table to see what matches we have.
			 */
			p = 0;
			while( (ptr = table[p]) != NULL )
				{
				unsigned char *src2;

				src2 = ptr;
				matched = len;
				endp = current_string;

				while( matched > 0 && *endp == *src2 )
					{
					matched--;
					endp++;
					src2++;
					}
				if( matched != 0 )
					{
					if( endp - current_string > maxok )
						maxok = endp - current_string;
					}
				else
					{
					/*
					 * We have found a match. If its the first, then copy the
					 * match as the next prefix string.
					 * If its not the first, then shorten the prefix string to
					 * the bit that unambiguous
					 */

					nfound++;
					if( len > maxok ) maxok = len;
					if( nfound == 1 )
						memcpy( prefix, ptr, min (_str_len (ptr) + 1, PREFIX_BUF_SIZE));
					else
						{
						int len;
						len = 0;

						while( len < PREFIX_BUF_SIZE
						&& prefix[len] == ptr[len] )
							len++;

						prefix[len] = 0;
						}
					bestp = p;
					if( ptr[len] == 0 )
						/*
						 * An exact match has been found, so lets exit
						 */
						{
						nfound = 1;
						break;
						}
					}
				p++;
				}
			}
		/*
		 * We have searched all of the items specified.
		 * If we only have one item, then lets get and
		 * return it. Otherwise we will have to show some help
		 * or worse.
		 */
		if( nfound == 1 )
			break;

		bestp = -1;
		if( nfound <= 1
		|| _str_ncmp( current_string, prefix, len + 1 ) == 0)
			{
			/*
			 * The input string is now so way off that no prefix strings matches
			 * or the string is wrong in the later sections. Either way, the poor
			 * user needs some help
			 */
			if( ! interactive )
				{
				/*
				 * The error occured non-interactively,
				 * report the error directly and exit
				 */
				bestp = -1;
				error( u_str("\"%s\" %s"), current_string,
					nfound == 0 ?
						u_str("does not make any sense to me.")
					:
						u_str("is ambiguous."));
				break;
				}
			if( !auto_help && !help )
				{
				/*
				 * The user recons to be a wizard, and has help-on-command-completion-error
				 * switches of, and he didnt ask for help. Just bell at him, and
				 * remove all the erroneous characters.
				 */
				ding ();
				if( nfound == 0 )
					{
					memcpy( prefix, current_string, maxok);
					prefix[maxok] = 0;
					}
				}
			else
				{
				/*
				 * Pop up a help window, and fill it full of stuff
				 * for the user to see.
				 */
				unsigned char *msg;

				if( old == 0 )
					{
					old = new_mark ();
					set_mark (old, bf_cur, 1, 0);
					if( remove_help_window != 0 && cur_exec == 0 )
						{
						saved_windows = 1;
						push_window_ring ();
						}
					}
				scratch_bfn( u_str( "Help" ), 1 );

				/*
				 * Work out what to prompt with next time round, and
				 * what message to spit at the user in the help buffer
				 */
				if( help != 0 )
					{
					memcpy( prefix, current_string, len + 1 );
					msg = u_str("Choose one of the following:\n");
					}
				else
					if( nfound > 1 )
						msg = u_str("Ambiguous, choose one of the following:\n");
					else
						{
						len = 0;
						msg = u_str("Please use one of the following words:\n");
						}
				ins_str (msg);

				/*
				 * Generate the help buffer full of suitable strings
				 */
				side = 2;	/* number of columns to test for room for */
				for( p=0; table[p] != NULL; p++ )
					{
					if( len <= 0
					|| _str_ncmp( table[p], current_string, len) == 0)
						{
						unsigned char buf[PREFIX_BUF_SIZE + 1];

						/* make sure the help fits in the windows */
						if( side*longest_string <= wn_cur->w_width )
							{
							/* there is room for this one and the next at the end */
							side++;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%-*s "), longest_string, table[p]);
							}
						else
							{
							/* room for just this one */
							side = 2;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%s\n"), table[p]);
							}
						ins_str (buf);
						}
					}
				/*
				 * Now that the help buffer is full of useful information,
				 * display it to the user
				 */
				beginning_of_file ();
				bf_modified = 0;
				}
			}
		}

	/*
	 * Everything is now complete, one way or another. So, lets tidy up any
	 * mess we made of the screen, spit out the text of the word chosen and
	 * get on with it
	 */
	if( old != NULL && old->m_buf != 0 )
		window_on (old->m_buf);
	if( saved_windows )
		pop_window_ring ();
	if( old != NULL )
		dest_mark (old);
	help = old_help;
	expand = old_expand;
	if( (interactive && bestp >= 0) || force_exit )
		{
		int i;

		i = do_print( fmt, argp, prefix, PREFIX_BUF_SIZE );
		prefix[i] = 0;

		message (u_str("%s%s"), prefix,
			force_exit ? current_string : table[bestp] );
		}
	/*
	 * return the actual text that caused the exit, or zero for aborts
	 */
	if( (! interactive || force_exit) )
		return current_string;
	else
		if( current_string != 0 )
			return table[bestp];
		else
			return 0;
	}				/* Of routine getnsword */


/* "getescword - Get an escape filled word" */
unsigned char *getescword ( unsigned char * *table_ref, unsigned char *fmt, ... )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine uses getnsword to fetch a none space expanded
 *	word from the user
 *
 * FORMAL PARAMETERS:
 *
 *	table_ref is the address of a word containing the address of
 *	the table of words
 *
 *	s is the prompt format string
 *
 * IMPLICIT INPUTS:
 *
 *	NONE
 *
 * IMPLICIT OUTPUTS:
 *
 *	NONE
 *
 * ROUTINE VALUE:
 *
 *	The string selected by the user, or zero to indicate an abort
 *
 * SIDE EFFECTS:
 *
 *	NONE
 *-- */
	{
	va_list argp;

	va_start( argp, fmt );

	return getnsword (table_ref, 0, fmt, &argp );
	}

unsigned char * call_getescfile( unsigned char *fmt, ... )
	{
	va_list arg;

	va_start( arg, fmt );

	return getescfile(fmt, &arg, 0, 0);
	}
    
/* "getescfile - gets a filename which may be ESC expanded" */
unsigned char *getescfile( unsigned char *fmt, va_list *argp, int PNOTUSED(dirs), int vers )
/*
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine gets a filename from the user. A table of possible
 *	filenames is generated from which the user should select. If
 *	he wants a file that is not in the list, then he types in
 *	the string and it is returned.
 *
 * FORMAL PARAMETERS:
 *
 *	flag is used to indicate if version numbers should be expanded
 *
 *	s is the format string for the prompt
 *
 * IMPLICIT INPUTS:
 *
 *	NONE
 *
 * IMPLICIT OUTPUTS:
 *
 *	NONE
 *
 * ROUTINE VALUE:
 *
 *	0 to indicate that the user aborted the operation, or the
 *	address of the selected string
 *
 * SIDE EFFECTS:
 *
 *	NONE
 */
	{
	unsigned char **table = NULL;
	unsigned char *endp;		/* End of string pointer */
	int maxok;			/* max legal chars from prefix */
	int matched;			/* Chars that didnt match */
	int len;			/* Length of current input */
	int longest_string;		/* Prefix length to skip in help window */
	unsigned char *current_string;	/* pointer to current word */

	int p;				/* The loop counter to scan table */
	int bestp = -1;			/* The best version of p */
	int nfound;			/* Number of matching words found */
	int force_exit = 0;		/* A loop counter */
	struct marker *old = 0;		/* old position of dot */

	unsigned char prefix[PREFIX_BUF_SIZE+1]; /* the current prefix string */

	unsigned char *ptr;		/* A pointer to the current table entry */
	int side;
	int saved_windows = 0;		/* true if windows should be saved */

	int old_expand = expand;	/* Saved value of expand flag */
	int old_help = help;		/* Saved value of help flag */

	int old_flag = /* version flag saved value */
				glob_flag;
	int old_glob_first = /* global_first cache purger */
				glob_first;

	glob_flag = ignore_version_numbers ? 0 : vers;
	glob_first = 1;

	/*
	 * Get words from the user until one is found to match
	 */
	prefix[0] = 0;
	prefix[PREFIX_BUF_SIZE] = 0;
	for(;;)
		{
		help = expand = 0;
		nfound = 0;
		maxok = 0;
		current_string = br_getstr (1, prefix, fmt, argp );
		prefix[0] = 0;
		if( current_string == 0 )
			break;

		/*
		 * Deal with expand-and-exit cases and non-interactive cases. These
		 * just return the typed value
		 */
		if( cur_exec != NULL || (!expand && !help) )
			{
			force_exit = 1;
			break;
			}

		/*
		 * Make the table if we have to make tables more than once
		 * [Note that the current_string is written back into]
		 */
		table = get_file_list( current_string, &longest_string );
		longest_string += 2;

		len = _str_len( current_string );

		/*
		 * expand the word as the user requested
		 */
		if( expand )
			{
			/*
			 * Scan the table to see what matches we have.
			 */
			p = 0;
			while( (ptr = table[p]) != NULL )
				{
				unsigned char *src2;

				src2 = ptr;
				matched = len;
				endp = current_string;

				while( matched > 0 && *endp == *src2 )
					{
					matched--;
					endp++;
					src2++;
					}
				if( matched != 0 )
					{
					if( endp - current_string > maxok )
						maxok = endp - current_string;
					}
				else
					{
					/*
					 * We have found a match. If its the first, then copy the
					 * match as the next prefix string.
					 * If its not the first, then shorten the prefix string to
					 * the bit that unambiguous
					 */

					nfound++;
					if( len > maxok ) maxok = len;
					if( nfound == 1 )
						memcpy( prefix, ptr, min (_str_len (ptr) + 1, PREFIX_BUF_SIZE));
					else
						{
						int len;
						len = 0;

						while( len < PREFIX_BUF_SIZE
						&& prefix[len] == ptr[len] )
							len++;

						prefix[len] = 0;
						}
					bestp = p;
					if( ptr[len] == 0 )
						/*
						 * An exact match has been found, so lets exit
						 */
						{
						nfound = 1;
						break;
						}
					}
				p++;
				}
			if( nfound == 1 && file_is_directory( table[bestp] ) )
				continue;
			}
		/*
		 * We have searched all of the items specified.
		 * If we only have one item, then lets get and
		 * return it. Otherwise we will have to show some help
		 * or worse.
		 */
		if( nfound == 1 )
			break;

		bestp = -1;
		if( nfound <= 1
		|| _str_ncmp( current_string, prefix, len + 1 ) == 0)
			{
			/*
			 * The input string is now so way off that no prefix strings matches
			 * or the string is wrong in the later sections. Either way, the poor
			 * user needs some help
			 */
			if( ! interactive )
				{
				/*
				 * The error occured non-interactively,
				 * report the error directly and exit
				 */
				bestp = -1;
				error( u_str("\"%s\" %s"), current_string,
					nfound == 0 ?
						u_str("does not make any sense to me.")
					:
						u_str("is ambiguous."));
				break;
				}
			if( !auto_help && !help )
				{
				/*
				 * The user recons to be a wizard, and has help-on-command-completion-error
				 * switches of, and he didnt ask for help. Just bell at him, and
				 * remove all the erroneous characters.
				 */
				ding ();
				if( nfound == 0 )
					{
					memcpy( prefix, current_string, maxok);
					prefix[maxok] = 0;
					}
				}
			else
				{
				/*
				 * Pop up a help window, and fill it full of stuff
				 * for the user to see.
				 */
				unsigned char *msg;

				if( old == 0 )
					{
					old = new_mark ();
					set_mark (old, bf_cur, 1, 0);
					if( remove_help_window != 0 && cur_exec == 0 )
						{
						saved_windows = 1;
						push_window_ring ();
						}
					}
				scratch_bfn( u_str( "Help" ), 1 );

				/*
				 * Work out what to prompt with next time round, and
				 * what message to spit at the user in the help buffer
				 */
				if( help != 0 )
					{
					memcpy( prefix, current_string, len + 1 );
					msg = u_str("Choose one of the following:\n");
					}
				else
					if( nfound > 1 )
						msg = u_str("Ambiguous, choose one of the following:\n");
					else
						{
						len = 0;
						msg = u_str("Please use one of the following words:\n");
						}
				ins_str (msg);

				/*
				 * Generate the help buffer full of suitable strings
				 */
				side = 2;	/* number of columns to test for room for */
				for( p=0; table[p] != NULL; p++ )
					{
					if( len <= 0
					|| _str_ncmp( table[p], current_string, len) == 0)
						{
						unsigned char buf[PREFIX_BUF_SIZE + 1];

						/* make sure the help fits in the windows */
						if( side*longest_string <= wn_cur->w_width )
							{
							/* there is room for this one and the next at the end */
							side++;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%-*s "), longest_string, table[p]);
							}
						else
							{
							/* room for just this one */
							side = 2;
							sprintfl (buf, PREFIX_BUF_SIZE, u_str("%s\n"), table[p]);
							}
						ins_str (buf);
						}
					}
				/*
				 * Now that the help buffer is full of useful information,
				 * display it to the user
				 */
				beginning_of_file ();
				bf_modified = 0;
				}
			}
		}

	/*
	 * Everything is now complete, one way or another. So, lets tidy up any
	 * mess we made of the screen, spit out the text of the word chosen and
	 * get on with it
	 */
	if( old != NULL && old->m_buf != NULL )
		window_on (old->m_buf);
	if( saved_windows )
		pop_window_ring ();
	if( old != NULL )
		dest_mark (old);
	help = old_help;
	expand = old_expand;
	if( (interactive && bestp >= 0) || force_exit )
		{
		int i;

		i = do_print( fmt, argp, prefix, PREFIX_BUF_SIZE );
		prefix[i] = 0;

		message (u_str("%s%s"), prefix,
			force_exit ? current_string : table[bestp] );
		}
	glob_flag = old_flag;
	glob_first = ! glob_first ? 1 : old_glob_first;

	/*
	 * return the actual text that caused the exit, or zero for aborts
	 */
	if( (! interactive || force_exit) )
		return current_string;
	else
		if( current_string != 0 )
			return table[bestp];
		else
			return 0;
	}


#ifdef vms
/* "get_file_list - expand a filespec into a table of filenames" */
int qsort_strcmp( const void *arg1, const void *arg2 ) 
	{
	return strcmp( * ( char** ) arg1, * ( char** ) arg2 );
	}


static unsigned char ** get_file_list( unsigned char *file, int *max_string_len )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine checks that the current filename table still
 *	works for the typed in filename. If it does, then it returned
 *	having done nothing. Otherwise, it creates the filename table
 *	and caches it.
 *
 * FORMAL PARAMETERS:
 *
 *	file is the name of the filespec to check_
 *
 *	max_string_len is the location to fill with the length up to the
 *	end of the directory spec
 *
 * IMPLICIT INPUTS:
 *
 *	glob_first is used to see if the filename chache must be
 *	purged. A new file cache is created every time getescfile is
 *	used.
 *
 *	glob_flag is used to decide if version numbers must be
 *	expanded
 *
 * IMPLICIT OUTPUTS:
 *
 *	glob_flag is set to zero
 *
 * ROUTINE VALUE:
 *
 *	The address of the filename table generated
 *
 * SIDE EFFECTS:
 *
 *	Any old filename table is destroyed
 */
	{
	int resp;
	int count;			/* Loop counter */
	int len = _str_len(file);	/* The length of the file string */
	struct fab fab;
	int dir_spec_len;
	int i;

	/*
	 * All files have now been generated, so add the final 0 to end
	 * the table in the conventional way
	 */
	for( count = 0; count < MAXFILES; count++ )
		if( vecfiles[count] != NULL )
			free( vecfiles[count] );

	count = 0;
	memset( &vecfiles, 0, sizeof( vecfiles ) );

	*max_string_len = 0; /* start off with zero length */

 	init_fab( &fab );
#if 0
	/* if we can parse what we have then make that the prompt */
	if( sys_parse( file, u_str(""), &fab ) )
		{
		_str_cpy( file, fab.result_spec );
		if( !fab.file_case_sensitive )
			_str_lwr( file );
		}

	/* need the 'file' with a wild * on the end */
	_str_cpy( savedstr, file );
	_str_cat( savedstr, "*" );

	/* Expand to a full path */
	resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
	if( !resp )
		{
		/* opss thats a bad path...
		 * just return the current directory contents */
		savedstr[savedlen] = 0;
		resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
		}
	if( resp )
#else
	/* need the 'file' with a wild * on the end */
	_str_cpy( savedstr, file );
	_str_cat( savedstr, "*" );
	fab.result_spec = savestr( savedstr );

	dir_spec_len = 0;
	for( i=0; savedstr[i] != '\0'; i++ )
		if( savedstr[i] == ']'
		|| savedstr[i] == '>'
		|| savedstr[i] == ':' )
			dir_spec_len = i+1;
#endif

		{
		unsigned char *file;
		void *handle;
		int status;

		/*
		 * For each file that matches the filespec, save the name
		 * away in the table. Make sure old entries in the table are
		 * deallocated first
		 */
		handle = find_first_file( fab.result_spec, &file );
		status = handle != NULL;

		count = 0;
		while( status && count < MAXFILES )
			{
			unsigned char *ptr;	/* Pointer into allocated memory */
			int cpy;		/* Length of component to copy */

			/*
			 * Find where in the original string, the directory spec
			 * ended. The string to the left will be added as the directory
			 * spec for each file in the table. This is so that expanded
			 * logical names etc, do not cause any grief
			 */
		
			for( i=0; file[i] != '\0'; i++ )
				if( file[i] == ']'
				|| file[i] == '>'
				|| file[i] == ':' )
					cpy = i+1;

			/*
			 * Allocate space for the new string
			 */
			ptr = malloc_ustr( _str_len( file ) - cpy + dir_spec_len + 1 );
			if( ptr == NULL )
				break;

			/*
			 * Make up the new string, and copy it
			 */
			_str_ncpy( ptr, savedstr, dir_spec_len );
			_str_cpy( &ptr[dir_spec_len], &file[cpy] );

			vecfiles[count] = ptr;

			cpy = _str_len( ptr );
			if( cpy > *max_string_len )
				*max_string_len = cpy;

			count++;

#if 0
			if( count == 1 )
				{
				ptr = malloc( cpy + 1 + 3 );
				if( ptr == 0 )
					break;

				/*
				 * Make up the new string, and copy it
				 */
				_str_cpy( ptr, file );
				vecfiles[1] = ptr;

				/* find the end of the directory */
				ptr = _str_chr( ptr, ']' );
				if( ptr != NULL )
					/* fake up an file to go up a level */
					_str_cpy( ptr, "][-]" );

				cpy = _str_len( vecfiles[1] );
				if( cpy > *dir_spec_len )
					*dir_spec_len = cpy;

				count++;				
				}
#endif
			status = find_next_file( handle, &file );
			}
		if( handle != NULL )
			find_file_close( handle );
		}

	free_fab( &fab );

	qsort
	(
	(void *)vecfiles, (size_t)count, sizeof( unsigned char * ),
	qsort_strcmp
	);

	vecfiles[count] = 0;

	return vecfiles;
	}				/* Of routine get_file_list */
#else
/* "get_file_list - expand a filespec into a table of filenames" */
int qsort_strcmp( const void *arg1, const void *arg2 ) 
	{
	return strcmp( * ( char** ) arg1, * ( char** ) arg2 );
	}


static unsigned char ** get_file_list( unsigned char *file, int *dir_spec_len )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine checks that the current filename table still
 *	works for the typed in filename. If it does, then it returned
 *	having done nothing. Otherwise, it creates the filename table
 *	and caches it.
 *
 * FORMAL PARAMETERS:
 *
 *	file is the name of the filespec to check_
 *
 *	dir_spec_len is the location to fill with the length up to the
 *	end of the directory spec
 *
 * IMPLICIT INPUTS:
 *
 *	glob_first is used to see if the filename chache must be
 *	purged. A new file cache is created every time getescfile is
 *	used.
 *
 *	glob_flag is used to decide if version numbers must be
 *	expanded
 *
 * IMPLICIT OUTPUTS:
 *
 *	glob_flag is set to zero
 *
 * ROUTINE VALUE:
 *
 *	The address of the filename table generated
 *
 * SIDE EFFECTS:
 *
 *	Any old filename table is destroyed
 */
	{
	int resp;
	int count;			/* Loop counter */
	struct fab fab;

	/*
	 * All files have now been generated, so add the final 0 to end
	 * the table in the conventional way
	 */
	for( count = 0; count < MAXFILES; count++ )
		if( vecfiles[count] != NULL )
			free( vecfiles[count] );

	count = 0;
	memset( &vecfiles, 0, sizeof( vecfiles ) );

	*dir_spec_len = 0; /* start off with zero length */

	init_fab( &fab );
	/* if we can parse what we have then make that the prompt */
	if( sys_parse( file, u_str(""), &fab ) )
		{
		_str_cpy( file, fab.result_spec );
		if( file_is_directory( file )
		&& file[_str_len(file)-1] != PATH_CH )
			_str_cat( file, PATH_STR );
		if( !fab.file_case_sensitive )
			_str_lwr( file );
		}

	/* need the 'file' with a wild * on the end */
	_str_cpy( savedstr, file );
	_str_cat( savedstr, "*" );

	/* Expand to a full path */
	resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
	if( !resp )
		{
		/* opss thats a bad path...
		 * just return the current directory contents */
		savedstr[savedlen] = 0;
		resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
		}
	if( resp )
		{
		unsigned char *file;
		void *handle;
		int status;

		/*
		 * For each file that matches the filespec, save the name
		 * away in the table. Make sure old entries in the table are
		 * deallocated first
		 */
		handle = find_first_file( fab.result_spec, &file );
		status = handle != NULL;

		count = 0;
		while( status && count < MAXFILES )
			{
			unsigned char *ptr;	/* Pointer into allocated memory */
			int cpy;		/* Length of component to copy */

			/*
			 * Find where in the original string, the directory spec
			 * ended. The string to the left will be added as the directory
			 * spec for each file in the table. This is so that expanded
			 * logical names etc, do not cause any grief
			 */
			cpy = _str_len( file );
			if( cpy > *dir_spec_len )
				*dir_spec_len = cpy;

			/*
			 * Allocate space for the new string
			 */
			ptr = malloc_ustr( cpy + 1 );
			if( ptr == 0 )
				break;

			/*
			 * Make up the new string, and copy it
			 */
			_str_cpy( ptr, file );
			vecfiles[count] = ptr;

			count++;

			status = find_next_file( handle, &file );
			}
		if( handle != NULL )
			find_file_close( handle );
		}

	free_fab( &fab );

	qsort
	(
	(void *)vecfiles, (size_t)count, sizeof( unsigned char * ),
	qsort_strcmp
	);

	vecfiles[count] = 0;

	return vecfiles;
	}				/* Of routine get_file_list */
#endif


/* "getescdb - fetch an expanded db key" */
unsigned char *getescdb ( struct dbsearch *current_dbs, unsigned char *fmt, ... )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine is used to fetch a database search list key
 *	name. It enables full expansion of key names from the DB
 *	search list. Either one of the names can be used, or a
 *	completly new name generated
 *
 * FORMAL PARAMETERS:
 *
 *	dbs is the address of the database search list to use.
 *
 *	s is the address of the prompt string format
 *
 * IMPLICIT INPUTS:
 *
 *	NONE
 *
 * IMPLICIT OUTPUTS:
 *
 *	glob_first is altered as a table is generated
 *
 * ROUTINE VALUE:
 *
 *	The address of the selected string, or zero indicating that
 *	the user aborted the operations
 *
 * SIDE EFFECTS:
 *
 *	vecfiles is trashed, savedstr and savedlen are also trashed
 *-- */
	{
#ifdef DB
	struct dbsearch *old_dbs = dbs;		/* The current DBS */
	int old_glob_first =	/* global_first cache purger */
			glob_first;
	unsigned char *resp;		/* Result of helper function */
	va_list argp;

	va_start( argp, fmt );

	glob_first = 1;
	dbs = current_dbs;

	resp = getnsword (0, globdb, fmt, &argp );

	glob_first = !glob_first ? 1 : old_glob_first;
	dbs = old_dbs;

	return resp;
#else
		error( u_str("Not implemented") );
		return 0;
#endif
	}				/* Of routine getescdb */


#ifdef DB
/* "globdb - generate a table of db search list keys" */
static unsigned char ** globdb( unsigned char *key, int *ignore_len )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine actually generates the keyname list. it is
 *	called when expansion is required. It check_s the glob_first
 *	flag, and created a new list is required, otherwise, set
 *	matches the last string used, to see in the new string starts
 *	with the old. If it does, it just returns. Otherwise, it has
 *	to generate a new table.
 *
 * FORMAL PARAMETERS:
 *
 *	key is the address of the start of the key
 *
 *	ignore_len must be set to zero
 *
 * IMPLICIT INPUTS:
 *
 *	NONE
 *
 * IMPLICIT OUTPUTS:
 *
 *	savedstr contains the saved key prefix
 *
 *	savedlen shows the usage of savedstr
 *
 * ROUTINE VALUE:
 *
 *	NONE
 *
 * SIDE EFFECTS:
 *
 *	A new key table is generated
 */
	{
	int count;

	/*
	 * All files have now been generated, so add the final 0 to end
	 * the table in the conventional way
	 */
	for( count = 0; count < MAXFILES; count++ )
		if( vec_db[count] != NULL )
			free( vec_db[count] );

	count = 0;
	memset( &vec_db, 0, sizeof( vec_db ) );

	dbs_count = 0;

	/*
	 * The cache is invalid, so create a new table, trashing the
	 * old as we go along.
	 */
	_str_cpy( savedstr, key );
	_str_cat( savedstr, "*" );
	fetch_database_index( dbs, savedstr, globdb_helper);

	*ignore_len = 0;
	for( count = 0; vec_db[count] != NULL; count++ )
		{
		int len = _str_len( vec_db[count] );
		if( len > *ignore_len )
			*ignore_len = len;
		}

	qsort
	(
	(void *)vec_db, (size_t)dbs_count, sizeof( unsigned char * ),
	qsort_strcmp
	);

	return vec_db;
	}				/* Of routine globdb */

/* "globdb_helper - make each key entry in the key table" */
static int globdb_helper( int keylen, unsigned char *key, unsigned char * *PNOTUSED(rfs) )
/*++
 * FUNCTIONAL DESCRIPTION:
 *
 *	This routine is called one for each key found to match the
 *	starting string. Its job is to save the key in the table, and
 *	increase the counter. The key is converted to lower case as
 *	it is moved into dynamic memory.
 *
 * FORMAL PARAMETERS:
 *
 *	key is the string descriptor of the current key
 *
 * IMPLICIT INPUTS:
 *
 *	dbs_count is the location in vecfile where the current key
 *	should be located.
 *
 * IMPLICIT OUTPUTS:
 *
 *	dbs_count is increased by one as required.
 *
 * ROUTINE VALUE:
 *
 *	1 if the copy took place, otherwise, 0
 *
 * SIDE EFFECTS:
 *
 *	NONE
 */
	{
	unsigned char *ptr;

	/*
	 * First check that the table has not overflowed
	 */
	if( dbs_count >= MAXFILES )
		return 0;

	/*
	 * Allocate space for the new string
	 */
	if( (ptr = vec_db[dbs_count] = malloc_ustr( keylen+1 ) ) == NULL )
		return 0;

	/*
	 * Make up the new string, and copy it
	 */
	memcpy( ptr, key, keylen );
	ptr[keylen] = '\0';
	_str_lwr( ptr );

	dbs_count++;

	return 1;
	}				/* Of routine globdb_helper */
#endif
