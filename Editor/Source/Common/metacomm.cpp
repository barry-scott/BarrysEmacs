//module metacomm
//    (
//    ident    = 'V5.0 Emacs',
//    addressing_mode( nonexternal=long_relative, external=general )
//    ) =
//begin
//
//     Copyright (c) 1982, 1983, 1984, 1985, 1986, 1987
//         Barry A. Scott and Nick Emery
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );



int delete_region_to_buffer( void );
int yank_buffer( void );
int beginning_of_file( void );
int end_of_file( void );
int end_of_window( void );
int beginning_of_window( void );
static int skip_over(int punct, int inc, int dot);
static int word_operation(int direction, int del);
int forward_word( void );
int backward_word( void );
int delete_next_word( void );
int delete_previous_word( void );
static void apropos_helper( BoundName *b, EmacsString &keys, int range );
int apropos( void );
int apropos_variable( void );


const int skip_over_punct( 1 );        // SkipOver codes
const int skip_over_words( 0 );

int delete_region_to_buffer( void )
{
    EmacsString fn;

    if( cur_exec == NULL )
        EmacsBuffer::get_esc_word_interactive( ": delete-region-to-buffer ", EmacsString::null, fn );
    else
        EmacsBuffer::get_esc_word_mlisp( fn );

    if( !fn.isNull() )
    {
        if( !bf_cur->b_mark.isSet() )
        {
            error( "Mark not set" );
        }
        else
        {
            replace_to_buf( bf_cur->b_mark.to_mark() - dot, fn );
        }
    }
    return 0;
}

int yank_buffer( void )
{
    EmacsString fn;
    getescword( EmacsBuffer::, ": yank-buffer ", fn );

    if( !fn.isNull() )
    {
        if( input_mode == 1 ) gui_input_mode_before_insert();

        insert_buffer( fn );
    }
    return 0;
}

int beginning_of_file( void )
{
    set_dot( bf_cur->first_character() );
    return 0;
}            // of beginning_of_file

int end_of_file( void )
{
    set_dot( bf_cur->num_characters() + 1 );
    return 0;
}            // Of EndOfFile

int end_of_window( void )
{
    set_dot( scan_bf_for_lf( theActiveView->currentWindow()->getWindowStart(), theActiveView->currentWindow()->w_height - 2) );
    end_of_line();
    return 0;
}            // Of EndOfWindow

int beginning_of_window( void )
{
    set_dot( theActiveView->currentWindow()->getWindowStart() );
    return 0;
}            // Of BeginningOfWindow

static int skip_over(int punct, int inc, int dot)
{
    int n;

    if( inc < 0 )
        dot--;

    n = dot;

    if( punct == skip_over_words )
    {
        if( inc == 1 )
        {
            while( dot <= bf_cur->num_characters()
            && bf_cur->char_at_is( dot, SYNTAX_WORD ) )
                dot++;
        }
        else
        {
            while( dot >= bf_cur->first_character()
            && bf_cur->char_at_is( dot, SYNTAX_WORD ) )
                dot--;
        }
    }
    else
    {
        if( inc == 1 )
        {
            while( dot <= bf_cur->num_characters()
            && !bf_cur->char_at_is( dot, SYNTAX_WORD ) )
                dot++;
        }
        else
        {
            while( dot >= bf_cur->first_character()
            && !bf_cur->char_at_is( dot, SYNTAX_WORD ) )
                dot--;
        }
    }
    return dot - n;
}

static int word_operation(int direction, int del)
{
    int inc; int n;
    int count = arg;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg( 1 );

    do
     {
        inc = direction;
        n = skip_over( skip_over_punct, inc, dot );
        n = n + skip_over( skip_over_words, inc, dot + n );
        if( n == 0 )
            return 0;
        if( direction < 0 && del )
        {
            del_chars_in_buffer( dot, -n, 0);
            dot_left( -n );
        }
        else
            if( del )
             {
                int num;
                num = del_chars_in_buffer( dot, n, 1 );
                if( num != 0 )
                    dot_right( num );
            }
            else
                dot_right( n );
        count--;
    }
    while( count > 0 && ! ml_err );

    return 0;
}

int forward_word( void )
{
    return word_operation( 1, 0 );
}

int backward_word( void )
{
    return word_operation( -1, 0 );
}

int delete_next_word( void )
{
    return word_operation( 1, 1 );
}

int delete_previous_word( void )
{
    return word_operation( -1, 1 );
}

static BoundName *apropos_target;
static EmacsString apropos_keys;

static void apropos_helper
    (
    BoundName *b,
    EmacsString &keys,
    int range
    )
{
    if( b != apropos_target )
        return;

    EmacsString s = key_to_str( keys );
    if( apropos_keys.length() > 0 )
        apropos_keys.append(", ");
    apropos_keys.append( s );

    if( range > 1 )
    {
        int len = keys.length();
        keys[len - 1] = (EmacsChar_t)(keys[len - 1] + range - 1);
        apropos_keys.append( ".." );
        s = key_to_str( keys );
        apropos_keys = s;

//        keys[len - 1] = (unsigned char)(keys[len - 1] - range - 1);
    }
}

static void apropos_command_inner( const EmacsString &keyword, KeyMap *local_map );
static void apropos_variable_inner( const EmacsString &keyword );

int apropos( void )
{
    EmacsString keyword;
    EmacsBufferRef old( bf_cur );
    keyword = getnbstr( ": apropos keyword: " );
    if( keyword.isNull() )
        return 0;

    EmacsBuffer::scratch_bfn( "Help", 1 );

    bf_cur->ins_cstr("Commands\n--------\n");
    apropos_command_inner( keyword, old.buffer()->b_mode.md_keys );
    bf_cur->ins_cstr("\nVariables\n---------\n");
    apropos_variable_inner( keyword );

    set_dot( 1 );
    bf_cur->b_modified = 0;
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

int apropos_command()
{
    EmacsString keyword;
    EmacsBufferRef old( bf_cur );
    keyword = getnbstr( ": apropos-command keyword: " );
    if( keyword.isNull() )
        return 0;

    EmacsBuffer::scratch_bfn( "Help", 1 );

    apropos_command_inner( keyword, old.buffer()->b_mode.md_keys );

    set_dot( 1 );
    bf_cur->b_modified = 0;
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

static void apropos_command_inner( const EmacsString &keyword, KeyMap *local_map )
{
    int index = 0;
    const EmacsString *word = NULL;

    while( (word = BoundName::name_table.apropos( keyword, index )) != NULL )
    {
        apropos_keys = "";
        apropos_target = BoundName::find( *word );

        scan_map( current_global_map, apropos_helper, true );
        scan_map( local_map, apropos_helper, true );

        EmacsString buf;
        if( !apropos_keys.isNull() )
            buf = FormatString("%-30s(%s)\n") << word << apropos_keys;
        else
            buf = FormatString("%s\n") << word;

        bf_cur->ins_cstr( buf );
    }
}

int apropos_variable( void )
{
    EmacsString keyword;
    EmacsBufferRef old( bf_cur );
    keyword = getnbstr( u_str(": apropos-variable keyword: ") );
    if( keyword.isNull() )
        return 0;

    EmacsBuffer::scratch_bfn( "Help", 1 );

    apropos_variable_inner( keyword );

    set_dot( 1 );
    bf_cur->b_modified = 0;
    old.set_bf();
    theActiveView->window_on( bf_cur );

    return 0;
}

static void apropos_variable_inner( const EmacsString &keyword )
{
    int index = 0;
    const EmacsString *word = NULL;

    while( (word = VariableName::name_table.apropos( keyword, index )) != NULL )
    {
        bf_cur->ins_cstr( *word ); bf_cur->ins_cstr( "\n" );
    }
}
