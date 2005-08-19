/*module errlog
 *	(
 *	ident = 'V5.0 Emacs',
 *	addressing_mode (nonexternal=long_relative, external=general)
 *	) =
 *begin
 * 	Copyright (c) 1982, 1983, 1984, 1985, 1986, 1987
 *		Barry A. Scott and Nick Emery */

#include <emacs.h>
#include <errlog.h>

/*forward*/ void delete_errlog_list( void );
/*forward*/ int check_error_message_parser( unsigned char *value, struct variablename *v );
/*forward*/ int parse_erb(int pos, int limit);
/*forward*/ int next_err( void );
/*forward*/ int prev_err( void );


/* Parse all of the error messages found in a region */
int parse_error_messages_in_region( void )
	{
	int left;
	int right;
	right = dot;
	if( bf_cur->b_mark == 0 )
		error( u_str("mark not set.") );
	else
		{
		left = to_mark( bf_cur->b_mark );
		if( left > right )
			{
			right = left;
			left = dot;
			}
		parse_erb( left, right );
		}
	return 0;
	}

int next_error( void )
	{
	next_err();
	return 0;
	}

int previous_error( void )
	{
	prev_err();
	return 0;
	}

unsigned char error_file_name[256];
int error_line_number;
int error_start_position;
GLOBAL SAVRES unsigned char error_message_parser_string[80] = "error-message-parser";

GLOBAL SAVRES struct bound_name *error_message_parser;
GLOBAL SAVRES struct errblk *errors;	/* the list of all error messages */
GLOBAL SAVRES struct errblk *thiserr;	/* the current error */


/* delete the error list */
void delete_errlog_list( void )
	{
	struct errblk *e;

	while( errors != NULL)
		{
		e = errors;
		dest_mark (e->e_mess);
		dest_mark(e->e_text);
		errors = e->e_next;
		free( e );
		}
	thiserr = NULL;
	}


/* check_ setting of error-message-parser */
int check_error_message_parser( unsigned char *value, struct variablename *v )
    {
    return check_is_proc (value, v, &error_message_parser);
    }


/* Parse error messages from the current buffer from character pos to limit */
int parse_erb (int pos, int limit)
	{
	struct emacs_buffer *erb = bf_cur;
	int old_dot;
	int rv = 0;
	int lo;
	int hi;
	unsigned char *fn;

	/* remove the last set of error markers */
	delete_errlog_list ();
	/* check that there is a parser to call */
	if( error_message_parser == NULL )
		error (u_str("No error message parser for parse-error-message-in-region."));
	else
		{
		/* Set up the buffer in which the parse is to take place */
		set_bfp (erb);
		old_dot = dot;
		dot = pos;
		/* Narrow the buffer on the reion to be parsed */
		lo = bf_mode.md_headclip;
		hi = bf_mode.md_tailclip;
		bf_cur->b_mode.md_headclip =
		    bf_mode.md_headclip = pos;
		bf_cur->b_mode.md_tailclip =
		    bf_mode.md_tailclip=
			bf_s1 + bf_s2 + 1 - limit;
		/*
		 * Invoke the scanning routine supplied by the user.
		 */
		while( !interrupt_key_struck && !err && rv == 0 )
			{
			/* select one to the error buffer */
			set_bfp (erb);
			/* Apply the users function */
			rv = execute_bound_saved_environment(error_message_parser);
			/* check that he output everything required */
			if( err
			|| error_start_position <= 0
			|| error_line_number <= 0
			|| error_file_name[0] == 0 )
				break;
			/* Visit the required file and create the markers */
			fn = savestr (error_file_name);
			if( visit_file (fn, 0, 0, u_str("")) == 0 )
				{
				free (fn);
				break;
				}
			free (fn);
			if( errors != NULL )
				{
				thiserr->e_next = malloc_struct( errblk );
				thiserr->e_next->e_prev = thiserr;
				thiserr = thiserr->e_next;
				}
			else
				{
				errors = thiserr = malloc_struct( errblk );
				thiserr->e_prev = NULL;
				}
			thiserr->e_next = NULL;
			thiserr->e_mess = new_mark();
			thiserr->e_text = new_mark();
			set_mark (thiserr->e_mess, erb, error_start_position, 0);
			set_mark (thiserr->e_text, bf_cur,
			    (error_line_number == 1 ? 1 : scan_bf ('\n', 1, error_line_number - 1)),
			    0 );
			}

		/* Restore the error message buffer to it original glory */
		set_bfp (erb);
		bf_cur->b_mode.md_headclip =
		    bf_mode.md_headclip = lo;
		bf_cur->b_mode.md_tailclip =
		    bf_mode.md_tailclip = hi;
		dot = old_dot;
		thiserr = NULL;
		}
	return errors != 0;
	}


/* move to the next error message in the log */
int next_err( void )
	{
	int n;

	if( errors == 0 )
		{
		error (u_str("No errors!"));
		return 0;
		}

	if( thiserr == 0 )
		thiserr = errors;
	else
		{
		thiserr = thiserr->e_next;
		if( thiserr == 0 )
			{
			error (u_str("No more errors..."));
			return 0;
			}
		}
	n = to_mark (thiserr->e_mess);
	window_on (bf_cur);
	set_dot (n);
	set_mark (wn_cur->w_start, bf_cur, dot, 0 );
	n = to_mark (thiserr->e_text);
	window_on (bf_cur);
	set_dot (n);

	return 1;
	}


/* move to the next error message in the log */
int prev_err( void )
	{
	int n;

	if( errors == 0 )
		{
		error (u_str("No errors!"));
		return 0;
		}

	if( thiserr == 0 )
		thiserr = errors;
	else
		{
		thiserr = thiserr->e_prev;
		if( thiserr == 0 )
			{
			error (u_str("No more errors..."));
			return 0;
			}
		}
	n = to_mark (thiserr->e_mess);
	window_on (bf_cur);
	set_dot (n);
	set_mark (wn_cur->w_start, bf_cur, dot, 0 );
	n = to_mark (thiserr->e_text);
	window_on (bf_cur);
	set_dot (n);

	return 1;
	}
