/*module caseconv
 *	(
 *	ident	= 'V5.0 Emacs',
 *	addressing_mode( nonexternal=long_relative, external=general )
 *	) =
 *begin
 *	Copyright (c) 1982, 1983, 1984, 1985
 *		Barry A. Scott and nick Emery */

/* emacs routines to play with the case of words (invert; set upper; set
 * lower; capitalize) */

#include <emacs.h>

enum case_convert_mode { case_invert, case_upper, case_lower, case_capitalize };

/*forward*/ static void case_convertion( int first, int last, enum case_convert_mode mode );
/*forward*/ static int case_word(enum case_convert_mode mode);
/*forward*/ static int case_region(enum case_convert_mode mode);
/*forward*/ int case_word_invert( void );
/*forward*/ int case_word_upper( void );
/*forward*/ int case_word_lower( void );
/*forward*/ int case_word_capitalize( void );
/*forward*/ int case_region_invert( void );
/*forward*/ int case_region_upper( void );
/*forward*/ int case_region_lower( void );
/*forward*/ int case_region_capitalize( void );
/*forward*/ int case_string_invert( void );
/*forward*/ int case_string_upper( void );
/*forward*/ int case_string_lower( void );
/*forward*/ int case_string_capitalize( void );
/*forward*/ static int case_string(enum case_convert_mode mode);
/*forward*/ void init_case( void );

/* Perform a case translation on the region from character 'first' to
 * character 'last'.
 *	mode=0 =>  invert
 *	mode=1 =>  upper
 *	mode=2 =>  lower
 *	mode=3 =>  capitalize (upper case first letter; lower case rest)
 */
static void case_convertion
	(
	int first,
	int last,
	enum case_convert_mode mode
	)
	{
	unsigned char c;
	int firstlet;

	firstlet = 1;
	while( first < last )
		{
		c = char_at( first );
		first = first+1;
		if( ! char_is( c, SYNTAX_WORD ) )
			{
			firstlet = 1;
			if( c == '\n' )
				cant_1line_opt = 1;
			}
		else
			{
			if( isalpha( c ) )
				if( mode == case_invert
				|| (isupper( c )
					? mode == case_lower || (mode == case_capitalize && !firstlet)
					: mode == case_upper || (mode == case_capitalize && firstlet)) )
					{
					insert_at( first - 1, (unsigned char)(c^0x20) );
					del_frwd( first, 1 );
					}
			firstlet = 0;
			}
		}
	if( ! bf_modified )
		redo_modes = cant_1line_opt = 1;
	bf_modified++;
	}

static int case_word(enum case_convert_mode mode)
	{
	int olddot;
	int left;

	olddot = dot;
	left = arg;

	if( dot <= num_characters )
		dot_right( 1 );

	arg = 1;
	backward_word();
	cant_1line_opt = 1;
	arg = left;
	left = dot;
	forward_word();
	case_convertion( left, dot, mode );
	set_dot( olddot );
	return 0;
	}

static int case_region(enum case_convert_mode mode)
	{
	int left;
	int right;

	right = dot;
	if( bf_cur->b_mark == 0 )
		error( u_str("Mark not set.") );
	else
		{
		left = to_mark( bf_cur->b_mark );
		if( left > right )
			{
			right = left;
			left = dot;
			}
		case_convertion( left, right, mode );
		}
	return 0;
	}

int case_word_invert( void )
	{
	return case_word( case_invert );
	}
int case_word_upper( void )
	{
	return case_word( case_upper );
	}
int case_word_lower( void )
	{
	return case_word( case_lower );
	}
int case_word_capitalize( void )
	{
	return case_word( case_capitalize );
	}
int case_region_invert( void )
	{
	return case_region( case_invert );
	}
int case_region_upper( void )
	{
	return case_region( case_upper );
	}
int case_region_lower( void )
	{
	return case_region( case_lower );
	}
int case_region_capitalize( void )
	{
	return case_region( case_capitalize );
	}
int case_string_invert( void )
	{
	return case_string( case_invert );
	}
int case_string_upper( void )
	{
	return case_string( case_upper );
	}
int case_string_lower( void )
	{
	return case_string( case_lower );
	}
int case_string_capitalize( void )
	{
	return case_string( case_capitalize );
	}

static int case_string(enum case_convert_mode mode)
	{
	unsigned char c;
	unsigned char *p;
	int firstlet;
	int i;

	firstlet = 1;

	concat_command();
	if( err )
		return 0;

	p = ml_value->exp_v.v_string;
	for( i=1; i<=ml_value->exp_int; i += 1 )
		{
		c = p[0];
		if( ! char_is( c, SYNTAX_WORD ) )
			firstlet = 1;
		else
			{
			if( isalpha (c) )
				if( mode == case_invert
				|| (isupper( c )
					? mode == case_lower || (mode == case_capitalize && !firstlet)
					: mode == case_upper || (mode == case_capitalize && firstlet)))
					p[0] = (unsigned char)(c ^ 0x20);
			firstlet = 0;
			}
		p = &p[1];
		}
	return 0;
	}

void init_case( void )
	{
	return;
	}
