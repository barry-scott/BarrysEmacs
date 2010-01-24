//
//    caseconv.cpp
//
//    Copyright (c) 1997-2010 Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


int case_word_invert( void );
int case_word_upper( void );
int case_word_lower( void );
int case_word_capitalize( void );

int case_region_invert( void );
int case_region_upper( void );
int case_region_lower( void );
int case_region_capitalize( void );

int case_string_invert( void );
int case_string_upper( void );
int case_string_lower( void );
int case_string_capitalize( void );

//
//    These class encapsulate the algorithms to decide if a char needs converting
//
class case_op
{
public:
    virtual ~case_op() {};
    virtual bool canTranslate( EmacsChar_t ch, bool is_first_letter ) = 0;
    virtual EmacsChar_t translate( EmacsChar_t ch, bool is_first_letter ) = 0;
};

class case_op_to_upper : public case_op 
{ 
public:
    virtual bool canTranslate( EmacsChar_t ch, bool is_first_letter )
    {
        return unicode_has_upper_translation( ch );
    }
    virtual EmacsChar_t translate( EmacsChar_t ch, bool is_first_letter )
    {
        return unicode_to_upper( ch );
    }
};

class case_op_to_lower : public case_op
{
public:
    virtual bool canTranslate( EmacsChar_t ch, bool is_first_letter )
    {
        return unicode_has_lower_translation( ch );
    }
    virtual EmacsChar_t translate( EmacsChar_t ch, bool is_first_letter )
    {
        return unicode_to_lower( ch );
    }
};

class case_op_capitalise : public case_op
{ 
public:
    virtual bool canTranslate( EmacsChar_t ch, bool is_first_letter )
    {
        if( is_first_letter )
        {
            if( unicode_is_title( ch ) )
                return false;
            else if( unicode_has_title_translation( ch ) )
                return true;
            else
                return false;

        }
        else if( unicode_is_lower( ch ) )
            return false;
        else if( unicode_has_lower_translation( ch ) )
            return true;
        else
            return false;
    }
    virtual EmacsChar_t translate( EmacsChar_t ch, bool is_first_letter )
    {
        if( is_first_letter )
        {
            if( unicode_has_title_translation( ch ) )
                return unicode_to_title( ch );
            else
                return ch;
        }
        else if( unicode_has_lower_translation( ch ) )
            return unicode_to_lower( ch );
        else
            return ch;
    }
};

class case_op_invert : public case_op
{
public:
    virtual bool canTranslate( EmacsChar_t ch, bool is_first_letter )
    {
        if( unicode_is_upper( ch ) && unicode_has_lower_translation( ch ) )
            return true;
        else
        if( unicode_is_lower( ch ) && unicode_has_upper_translation( ch ) )
            return true;
        else
            return false;
    }
    virtual EmacsChar_t translate( EmacsChar_t ch, bool is_first_letter )
    {
        if( unicode_is_upper( ch ) && unicode_has_lower_translation( ch ) )
            return unicode_to_lower( ch );
        else
        if( unicode_is_lower( ch ) && unicode_has_upper_translation( ch ) )
            return unicode_to_upper( ch );
        else
            return ch;
    }
};



//
// Perform a case translation on the region from character 'first' to character 'last'.
//
static void case_conversion( int first, int last, case_op &op )
{
    bool buffer_modified = false;

    bool is_first_letter = true;
    while( first < last )
    {
        EmacsChar_t c = bf_cur->char_at( first++ );

        if( !bf_cur->char_is( c, SYNTAX_WORD ) )
        {
            is_first_letter = true;
        }
        else
        {
            if( op.canTranslate( c, is_first_letter ) )
            {
                bf_cur->insert_at( first - 1, op.translate( c, is_first_letter ) );
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
    if( ml_err )
        return 0;

    // copy the string to allow an overwrite
    const EmacsString &value = ml_value.asString();
    EmacsString result;

    // dirty trick to modify the data inside the string
    const EmacsChar_t *p = value.unicode_data();

    for( int i=result.length(); i>0; i--, p++ )
    {
        EmacsChar_t c = *p;
        if( !bf_cur->char_is( c, SYNTAX_WORD ) )
        {
            result.append( c );
            is_first_letter = true;
        }
        else
        {
            if( op.canTranslate( c, is_first_letter ) )
                result.append( op.translate( c, is_first_letter ) );
            else
                result.append( c );

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
    arg = 1;        backward_word();
    cant_1line_opt = 1;

    int left = dot;
    arg = saved_arg;    forward_word();

    case_conversion( left, dot, op );

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
    case_conversion( left, right, op );

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
