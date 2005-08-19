//
//	caseconv.cpp
//
//	Copyright (c) 1997 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


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

//
//	These class encapsulate the algorithms to decide if a char needs converting
//
class case_op { public: virtual bool convert( bool is_upper, bool is_first_letter ) = 0; };

class case_op_to_upper : public case_op { public: virtual bool convert( bool is_upper, bool /*is_first_letter*/ )
	{ return !is_upper; } };
class case_op_to_lower : public case_op { public: virtual bool convert( bool is_upper, bool /*is_first_letter*/ )
	{ return is_upper; } };
class case_op_capitalise : public case_op { public: virtual bool convert( bool is_upper, bool is_first_letter )
	{ return is_first_letter ? !is_upper : is_upper; } };
class case_op_invert : public case_op { public: virtual bool convert( bool /*is_upper*/, bool /*is_first_letter*/ )
	{ return true; } };

//
// Perform a case translation on the region from character 'first' to character 'last'.
//
static void case_convertion( int first, int last, case_op &op )
	{
	bool buffer_modified = false;
	
	bool is_first_letter = true;
	while( first < last )
		{
		unsigned char c = bf_cur->char_at( first );	first++;

		if( !bf_cur->char_is( c, SYNTAX_WORD ) )
			{
			is_first_letter = true;
			}
		else
			{
			if( isalpha( c ) && op.convert( isupper( c ) != 0, is_first_letter ) )
				{
				bf_cur->insert_at( first - 1, (unsigned char)(c^0x20) );
				bf_cur->del_frwd( first, 1 );
				buffer_modified = true;
				}
			is_first_letter = false;
			}
		}

	if( buffer_modified )
		{
		redo_modes = cant_1line_opt = 1;

		bf_cur->b_modified++;
		}
	}

static int case_string( case_op &op )
	{
	bool is_first_letter = true;

	concat_command();
	if( err )
		return 0;

	// copy the string to allow an overwrite
	const EmacsString &value = ml_value.asString();
	EmacsString result( EmacsString::copy, value.data(), value.length() );

	// dirty trick to modify the data inside the string
	unsigned char *p = (unsigned char *)result.data();

	for( int i=result.length(); i>0; i--, p++ )
		{
		unsigned char c = *p;
		if( !bf_cur->char_is( c, SYNTAX_WORD ) )
			is_first_letter = true;
		else
			{
			if( isalpha( c ) && op.convert( isupper( c ) != 0, is_first_letter ) )
				*p = (unsigned char)(c ^ 0x20);

			is_first_letter = false;
			}
		}

	ml_value = Expression( result );

	return 0;
	}

static int case_word( case_op &op )
	{
	int olddot = dot;

	if( dot <= bf_cur->num_characters() )
		dot_right( 1 );

	int saved_arg = arg;
	arg = 1;		backward_word();
	cant_1line_opt = 1;

	int left = dot;
	arg = saved_arg;	forward_word();

	case_convertion( left, dot, op );

	set_dot( olddot );

	return 0;
	}

static int case_region( case_op &op )
	{
	if( !bf_cur->b_mark.isSet() )
		{
		error( "Mark not set." );
		return 0;
		}

	int right = dot;
	int left = bf_cur->b_mark.to_mark();
	if( left > right )
		{
		right = left;
		left = dot;
		}
	case_convertion( left, right, op );

	return 0;
	}

int case_word_invert( void ) { case_op_invert op; return case_word( op ); }
int case_word_upper( void ) { case_op_to_upper op; return case_word( op ); }
int case_word_lower( void ) { case_op_to_lower op; return case_word( op ); }
int case_word_capitalize( void ) { case_op_capitalise op; return case_word( op ); }

int case_region_invert( void ) { case_op_invert op; return case_region( op ); }
int case_region_upper( void ) { case_op_to_upper op; return case_region( op ); }
int case_region_lower( void ) { case_op_to_lower op; return case_region( op ); }
int case_region_capitalize( void ) { case_op_capitalise op; return case_region( op ); }

int case_string_invert( void ) { case_op_invert op; return case_string( op ); }
int case_string_upper( void ) { case_op_to_upper op; return case_string( op ); }
int case_string_lower( void ) { case_op_to_lower op; return case_string( op ); }
int case_string_capitalize( void ) { case_op_capitalise op; return case_string( op ); }
