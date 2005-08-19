/*module columns
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/* Routines to deal with column numbering */

#include <emacs.h>

/*forward*/ int calc_col(void);
/*forward*/ void to_col(int n);
/*forward*/ int cur_indent(void);

/* calculate the print column at dot */
int calc_col( void )
	{
	int p;
	int col;

        p = scan_bf( '\n', dot, -1 );
        col = 1;

	while( p < dot )
		{
		unsigned char c;

		c = char_at( p );
		if( c == '\t' )
			col = ((col - 1) / bf_mode.md_tabsize + 1) * bf_mode.md_tabsize + 1;
		else
			if( control_character( c ) )
				col = col +
					(( ctl_arrow ) ?
					(( term_deccrt &&
					(c == ctl ('L')
					 || c == ctl ('M')
					 || c == ctl ('K')
					 || c == ctl ('[')) ) ? 1 : 2) : 4);
		else
				col++;
		p++;
		}
	dot_col = col;
	col_valid = 2;
	return col;
	}
/* Insert tabs and spaces until we are out to at least column n */
void to_col(int n)
	{
	int col;
	int ncol;

	col = cur_col;
	if( col >= n )
		return;
	while( (ncol = ((col - 1) / bf_mode.md_tabsize + 1) * bf_mode.md_tabsize + 1)  <= n )
		{
		self_insert( '\t' );
		col = ncol;
		}
	while( col < n )
		{
		self_insert(' ');
		col++;
		}
	dot_col = col;
	col_valid = 3;
	}
/* Calculate the indentation of the current line */
int cur_indent( void )
	{
	int p;
	int col;
	int lim;

	p = scan_bf( '\n', dot, -1 );
	col = 1;
	lim = num_characters;
	while( p <= lim )
		{
		unsigned char c;

		c = char_at( p );
		if( c == '\t' )
			col = ((col - 1) / bf_mode.md_tabsize + 1)
				* bf_mode.md_tabsize + 1;
		else
			if( c == ' ' )
				col++;
			else
				break;
		p++;
		}
	return col;
	}
/*end
 *eludom */
