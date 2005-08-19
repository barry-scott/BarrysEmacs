/*	Copyright (c) 1986                                                  */
/*		Barry A. Scott and Nick Emery                               */

const int SAVED_BUFFER_MAX( 100 );		/* maximum number of buffers to save in subprocess */
const int MAX_SYNTAX_TABLES( 40 );		/* the maximum number of syntax tables */
const int MAX_ABBREV_TABLES( 40 );		/* the maximum number of abbrev tables */
const int BUFFERSIZE( 300 );			/* Maximum buffer size */
const int MLLINE_SIZE( 300 );
const int MAXFILES( 512 );			/* Maximum size of vecfiles */
#ifndef BUFSIZ
#define BUFSIZ 512
#endif

//
//	Even if save_environment is not required these symbols are.
//

enum malloc_block_type
	{
	malloc_type_none = 0,
	malloc_type_char,
	malloc_type_star_star,
	malloc_type_emacs_object,
	malloc_type_emacs_object_Expression_Vector,
	/* insert new types before this one */
	malloc_type_last
	};
