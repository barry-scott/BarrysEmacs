/*module varthunk
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode(  nonexternal=long_relative, external=general  )
 *	) =
 *begin
 * 	Copyright(c ) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/*
 *	Copyright 1985 Barry A. Scott & Nick Emery
 *
 *	This module contains check_ and thunk routines for
 *	system variables.
 */
#include <emacs.h>


void fetch_buffer_names
	(
	struct emacs_array *a,
	int set_up,
	int index
	)
	{
	struct expression *e;

	if( set_up != 0 )
		{
		a->array_dimensions = 1;
		a->array_size[ 0 ] = n_buffers + 1;
		return;
		}
	if( index > n_buffers )
		{
		error( u_str("subscript error") );
		return;
		}
	e = a->array_expr[ 0 ];
	release_expr( e );

	/* index 0 is the number of buffer */
	if( index == 0 )
		{
		e->exp_type = ISINTEGER;
		e->exp_int = n_buffers;
		}
	else
		{
		e->exp_type = ISSTRING;
		e->exp_release = 1;
		e->exp_int = _str_len( buf_names[index - 1] );
		e->exp_v.v_string = savestr( buf_names[index - 1] );
		}
	}
