//module lispfunc
//    (
//    ident    = 'V5.0-001',
//    addressing_mode( nonexternal=long_relative, external=general )
//    ) =
//begin
//
//     Copyright (c) 1982, 1983, 1984, 1985, 1987
//        Barry A. Scott and Nick Emery
//     Copyright (c) 1988-2010
//        Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <time.h>

int get_tty_string( void );

unsigned char *marker_or_string = u_str("region-to-string expects an integer of marker as its parameter");
unsigned char *no_mem_str = u_str ("Out of memory.");
unsigned char *unknown_str = u_str ("Unknown");

EmacsString parent_path;

static EmacsString converted_system_name;
static EmacsString users_full_name;
#ifdef vms
static unsigned char lognam[256];    // terminal_names and terminal_idents are equivalence ordered
static unsigned char *terminal_names[] = {
 u_str("vt05"),u_str("vk100"),u_str("vt173"),u_str("tq_bts"),
 u_str("tek401x"),u_str("ft1"),u_str("ft2"),u_str("ft3"),
 u_str("ft4"),u_str("ft5"),u_str("ft6"),u_str("ft7"),u_str("ft8"),
 u_str("la36"),u_str("la120"),u_str("la34"),u_str("la38"),
 u_str("la12"),u_str("la100"),u_str("lqp02"),u_str("la84"),
 u_str("vt52"),u_str("vt55"),u_str("vt100"),u_str("vt101"),
 u_str("vt102"),u_str("vt105"),u_str("vt125"),u_str("vt131"),
 u_str("vt132"),u_str("vt200_series"),u_str("pro_series"),
 u_str("unknown")};
static int terminal_idents[] = {
 1,2,3,4,10,16,17,18,19,20,21,22,23,32,33,34,35,36,
 37,38,39,64,65,96,97,98,99,100,101,102,110,111,0};
#endif

int baud_rate_command( void )
    {
    ml_value = theActiveView->t_baud_rate;
    return 0;
    }

int dot_val_command( void )
    {
    ml_value = EMACS_NEW Marker( bf_cur, dot, 0 );
    return 0;
    }

int mark_val_command( void )
    {
    if( bf_cur->b_mark.isSet() )
{
    ml_value = EMACS_NEW Marker( bf_cur->b_mark );
}
    else
    error( FormatString("No mark set in buffer \"%s\"") << bf_cur->b_buf_name);

    return 0;
    }

int buf_size_command( void )
    {
    ml_value = bf_cur->num_characters() + 1 - bf_cur->first_character();
    return 0;
    }

int cur_col_func_command( void )
    {
    ml_value = calc_col();
    return 0;
    }

int this_indent_command( void )
    {
    ml_value = cur_indent();
    return 0;
    }

int bobp_command( void )
    {
    ml_value = dot <= bf_cur->first_character();
    return 0;
    }

int eobp_command( void )
    {
    ml_value = dot > bf_cur->num_characters();
    return 0;
    }

int bolp_command( void )
    {
    ml_value = dot <= bf_cur->first_character() || bf_cur->char_at (dot - 1) == '\n';
    return 0;
    }

int eolp_command( void )
    {
    ml_value = dot > bf_cur->num_characters() || bf_cur->char_at (dot) == '\n';
    return 0;
    }

int following_char_command( void )
    {
    ml_value = dot > bf_cur->num_characters() ? 0 : bf_cur->char_at (dot);
    return 0;
    }

int preceding_char_command( void )
    {
    ml_value = dot <= bf_cur->first_character() ? 0 : bf_cur->char_at (dot - 1);

    return 0;
    }

int fetch_last_key_struck_command( void )
    {
    ml_value = last_key_struck;
    return 0;
    }

int fetch_previous_command( void )
    {
    ml_value = previous_command;
    return 0;
    }

int recursion_depth_command( void )
    {
    ml_value = recursive_edit_depth;
    return 0;
    }

int nargs_command( void )
    {
    if( cur_exec == NULL )
    error( "nargs can only appear in mlisp statements" );
    if( execution_root == NULL )
    error( "nargs can only be called from within an MLisp procedure" );
    if( ml_err )
    return 0;
    if( execution_root->es_cur_exec != NULL )
        ml_value = execution_root->es_cur_exec->p_nargs;
    else
    ml_value = 0;

    return 0;
    }

int interactive_command( void )
    {
    if( execution_root == NULL || execution_root->es_cur_exec == NULL )
        ml_value = 1;
    else
    ml_value = 0;

    return 0;
    }

int current_buffer_name_command( void )
    {
    ml_value = bf_cur->b_buf_name;
    return 0;
    }

int current_file_name_command( void )
    {
    ml_value = bf_cur->b_fname;
    return 0;
    }

int users_login_name_command( void )
    {
    if( users_name.isNull() )
{
    EmacsString id = users_login_name();
    if( id.isNull() )
        users_name = unknown_str;
    else
        users_name = id;
}

    ml_value = users_name;
    return 0;
    }

int users_full_name_command( void )
    {
    if( users_full_name.isNull() )
{
    users_full_name = get_user_full_name();
}

    ml_value = users_full_name;
    return 0;
    }

int return_system_name_command( void )
    {
    if( converted_system_name.isNull() )
    get_system_name();

    ml_value = EmacsString( converted_system_name );

    return 0;
    }

int current_time( void )
    {
    time_t now = time(0);

    ml_value = EmacsString( EmacsString::copy, (unsigned char *)ctime (&now), 24 );

    if( interactive() )
    message( FormatString("%s") << ml_value.asString() );

    return 0;
    }

int arg_command( void )
    {
    if( cur_exec == NULL )
    error( "arg can only appear in mlisp statements" );
    if( execution_root == NULL )
    error( "arg can only be called from within an MLisp procedure" );
    int i = numeric_arg(1);
    if( ml_err )
    return 0;

    ProgramNode *p = execution_root->es_cur_exec;
    if( p == NULL )
     {
    if( string_arg (2) )
        {
        EmacsString prompt( ml_value.asString() );

        last_arg_used = 0;

        Save<ProgramNode *> lcur_exec( &cur_exec );
        cur_exec = NULL;

        EmacsString answer;
        answer = get_string_interactive( prompt, EmacsString::null );

        ml_value = answer;
        return 0;
        }
    return 0;
    }
    if( i > p->p_nargs || i < 1 )
     {
    error( FormatString("Bad argument index: (arg %d)") << i);
    return 0;
    }
    ExecutionStack *old = execution_root;
    execution_root = execution_root->es_dyn_parent;
    exec_prog( p->arg( i ) );
    execution_root = old;

    return 0;
    }

int dot_is_visible( void )
    {
    int windowtop = theActiveView->currentWindow()->getWindowStart();

    ml_value = dot >= windowtop
        && dot - (dot  >  bf_cur->num_characters()) <
    scan_bf_for_lf( windowtop, theActiveView->currentWindow()->w_height - 1 );

    return 0;
    }

int substr_command( void )
    {
    int pos = numeric_arg(2);
    int n = numeric_arg(3);

    if( string_arg (1) != 0 )
     {
    EmacsString str( ml_value.asString() );

    if( pos < 0 )
        pos = str.length() + 1 + pos;

    if( pos <= 0 )
        pos = 1;

    if( n < 0 )
        {
        n = str.length() + n;
        if( n < 0 )
        n = 0;
        }
    if( pos + n - 1 > str.length() )
        {
        n = str.length() + 1 - pos;
        if( n < 0 )
        n = 0;
        }

    // extract the sub string
    EmacsString sub_string( str( pos-1, pos-1+n ) );
    ml_value = sub_string;
    }

    return 0;
    }

// string-extract str start end
int string_extract( void )
{
    if( check_args( 3, 3 ) )
        return 0;

    if( !string_arg(1) )
        return 0;
    EmacsString str( ml_value.asString() );

    int start = numeric_arg(2);
    int end = numeric_arg(3);

    if( start < 0 )
    {
        start = str.length() + start;
        if( start < 0 )
            start = 0;
        if( end == 0 )
            end = str.length();
    }
    if( start > str.length() )
        start = str.length();

    if( end < 0 )
    {
        end = str.length() + end;
        if( end < 0 )
            end = 0;
    }
    if( end > str.length() )
        end = str.length();

    if( start > end )
    {
        int tmp = start;
        start = end;
        end = tmp;
    }

    // extract the sub string
    EmacsString sub_string( str( start, end ) );
    ml_value = sub_string;

    return 0;
}

// string-index-of-first "str" "find"
int string_index_of_first(void)
{
    if( check_args( 2, 2 ) )
        return 0;

    if( !string_arg( 1 ) )
        return 0;
    EmacsString string( ml_value.asString() );

    if( !string_arg( 2 ) )
        return 0;
    EmacsString find( ml_value.asString() );

    if( ml_err )
        return 0;

    int index = string.index( find, 0 );

    ml_value = index;

    return 0;
}

// string-index-of-last "str"
int string_index_of_last(void)
{
    if( check_args( 2, 2 ) )
        return 0;

    if( !string_arg( 1 ) )
        return 0;
    EmacsString string( ml_value.asString() );

    if( !string_arg( 2 ) )
        return 0;
    EmacsString find( ml_value.asString() );

    if( ml_err )
        return 0;

    int index = -1;
    int next;
    while( (next = string.index( find, index+1 )) >= 0 )
        index = next;

    ml_value = index;

    return 0;
}

// string-index-of-string "str" "find" pos
int string_index_of_string(void)
{
    if( check_args( 3, 3 ) )
        return 0;

    if( !string_arg( 1 ) )
        return 0;
    EmacsString string( ml_value.asString() );

    if( !string_arg( 2 ) )
        return 0;
    EmacsString find( ml_value.asString() );

    int index = numeric_arg( 3 );

    if( ml_err )
        return 0;

    if( index < 0 )
        index += string.length();

    if( index < 0
    || index >= string.length() )
    {
        ml_value = int(-1);
        //error( FormatString("index %d out of range in string-index-of-string") << index );
        return 0;
    }

    index = string.index( find, index );

    ml_value = index;

    return 0;
}


int to_col_command( void )
    {
    int n = getnum(": to-col ");

    if( ! ml_err )
    to_col (n);

    return 0;
    }
int char_to_string( void )
    {
    int n = getnum(": char-to-string ");

    EmacsChar_t str[1];
    str[0] = (EmacsChar_t)n;

    EmacsString string( EmacsString::copy, str, 1 );
    ml_value = string;

    return 0;
    }

int string_to_char( void )
{
    EmacsString s;
    if( cur_exec == NULL )
        s = get_string_interactive(": string-to-char ");
    else
        s = get_string_mlisp();

    // must test s before release_expr
    int the_char = s.length() > 0 ? s[0] : 0;

    ml_value = the_char;

    return 0;
}

int insert_character( void )
    {
    self_insert( (EmacsChar_t)getnum (": insert-character ") );
    return 0;
    }


static EmacsString get_tty_prompt_string( " (prompt) " );
static EmacsString get_tty_default_string( " (default) " );

static bool get_tty_prompt_and_default_value( const EmacsString &command_name, EmacsString &prompt, EmacsString &default_value )
{
    if( cur_exec == NULL )
    {
        EmacsString prompt_prompt( command_name ); prompt_prompt.append( get_tty_prompt_string );
        EmacsString prompt_default( command_name ); prompt_default.append( get_tty_default_string );

        prompt = get_string_interactive( prompt_prompt );
        default_value = get_string_interactive( prompt_default );
    }
    else
    {
        if( check_args( 1, 2 ) )
            return false;

        prompt = get_string_mlisp();
        if( ml_err )
            return false;
        if( cur_exec->p_nargs >= 2 )
        {
            default_value = get_string_mlisp();
            if( ml_err )
                return false;
        }
    }

    return true;
}

int get_tty_string( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-string", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsString answer;
    answer = get_string_interactive( prompt, default_value );

    ml_value = answer;
    return 0;
}

int get_tty_command( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-command", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    BoundName *b = BoundName::get_word_interactive( prompt, default_value );

    if( b != NULL )
        ml_value = b->b_proc_name;
    else
        ml_value = Expression();

    return 0;
}

int get_tty_variable( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-variable", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    VariableName *v = VariableName::get_word_interactive( prompt, default_value );

    if( v != NULL )
        ml_value = v->v_name;
    else
        ml_value = Expression();

    return 0;
}

int get_tty_buffer( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-string", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsString answer;
    EmacsBuffer::get_esc_word_interactive( prompt, default_value, answer );

    ml_value = answer;

    return 0;
}

int get_tty_file( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-file", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsFileTable file_table;
    EmacsString fn;

    // always interative as we just set cur_exec to NULL
    file_table.get_esc_word_interactive( prompt, default_value, fn );

    ml_value = fn;

    return 0;
}

int get_tty_directory( void )
{
    EmacsString prompt;
    EmacsString default_value;

    if( !get_tty_prompt_and_default_value( ": get-tty-directory", prompt, default_value ) )
        return 0;

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsDirectoryTable dir_table;
    EmacsString fn;
    if( cur_exec == NULL )
        dir_table.get_esc_word_interactive( prompt, default_value, fn );
    else
        dir_table.get_esc_word_mlisp( fn );

    ml_value = fn;

    return 0;
}

int expand_from_string_table( void )
{
    EmacsString prompt;
    EmacsString string;
    int exact = 0;
    EmacsString default_value;

    if( cur_exec == NULL )
    {
        prompt = get_string_interactive(": expand-from-string-table (prompt) ");
        string = get_string_interactive( FormatString(": expand-from-string-table (prompt) %s (table) ") << prompt);
        if( arg_state == have_arg )
            exact = arg;
    }
    else
    {
        if( check_args( 2, 4 ) )
            return 0;
        prompt = get_string_mlisp();
        string = get_string_mlisp();
        if( cur_exec->p_nargs > 2 )
            exact = getnum( ": expand-string-from-table (flags) " );
        if( cur_exec->p_nargs > 3 )
            default_value = get_string_mlisp();
    }

    EmacsStringTable table;
    int start=0;
    int end=0;
    while( end < string.length() )
    {
        if( string[end] == ' ' )
        {
            table.add( string( start, end ), NULL );
            start = end+1;
        }
        end++;
    }
    if( start != end )
        table.add( string( start, end ), NULL );


    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsString answer;
    if( exact )
        answer = table.get_word_interactive( prompt, default_value, answer );
    else
        answer = table.get_esc_word_interactive( prompt, default_value, answer );

    ml_value = answer;

    return 0;
}

int get_tty_character( void )
{
    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    ml_value = term_is_terminal != 0 || macro_replay_next >= 0 ? get_char () : -1;

    return 0;
}

int concat_command( void )
{
    string_arg( 1 );
    if( !ml_err && cur_exec->p_nargs > 1 )
    {
        EmacsString result;

        int i = 1;

        do
        {
            result.append( ml_value.asString() );
            i++;
        }
        while( i <= cur_exec->p_nargs && string_arg( i ) != 0 );

        ml_value = result;
    }

    return 0;
}


int region_to_string( void )
{
    int left;
    int right;

    EmacsBufferRef old_bf( bf_cur );

    if( cur_exec != 0
    && cur_exec->p_nargs > 0 )
    {
        if( check_args( 1, 2 ) )
            return 0;

        if( ! eval_arg( 1 ) )
            return 0;

        switch( ml_value.exp_type() )
        {
        case ISINTEGER:
            left = ml_value.asInt();
            break;
        case ISMARKER:
            left = ml_value.asMarker()->to_mark();
            break;
        default:
        {
            error( marker_or_string );
            return 0;
        }
        }

        if( cur_exec->p_nargs != 2 )
            right = dot;
        else
        {
            if( ! eval_arg( 2 ) )
                return 0;

            switch( ml_value.exp_type() )
            {
            case ISINTEGER:
                right = ml_value.asInt();
                break;
            case ISMARKER:
            {
                EmacsBuffer *left_bf;

                left_bf = bf_cur;
                right = ml_value.asMarker()->to_mark();
                if( left_bf != bf_cur )
                {
                    error( FormatString("2nd marker must refer to buffer %s") <<
                        left_bf->b_buf_name );
                    return 0;
                }
            }
                break;
            default:
            {
                error( marker_or_string );
                return 0;
            }
            }
        }
        if( left > right )
        {
            int swap = right;
            right = left;
            left = swap;
        }
    }
    else
    {
        if( !bf_cur->b_mark.isSet() )
        {
            error( "Mark not set");
            return 0;
        }
        left = bf_cur->b_mark.to_mark();
        if( left <= dot )
            right = dot;
        else
        {
            right = left;
            left = dot;
        }
    }

    bf_cur->gap_outside_of_range( left, right );

    EmacsString string( EmacsString::copy, bf_cur->ref_char_at(left), right - left );
    ml_value = string;

    if( old_bf.buffer() != bf_cur )
        old_bf.set_bf();

    return 0;
}

int length_command( void )
{
    if( string_arg(1) )
        ml_value = ml_value.asString().length();

    return 0;
}

int goto_character( void )
{
    int n = getnum(": goto-character ");

    if( ml_err )
        return 0;

    if( n < 1 )
        n = 1;

    if( n > bf_cur->num_characters() )
        n = bf_cur->num_characters() + 1;

    set_dot (n);

    return 0;
}

int no_value_command( void )
{
    return 0;
}

int putenv_command( void )
{
    EmacsString vname;
    if( cur_exec == NULL )
        vname = get_nb_string_interactive(": putenv ");
    else
        vname = get_string_mlisp();
    if( vname.isNull() )
        return 0;

    EmacsString value;
    if( cur_exec == NULL )
        value = get_string_interactive( FormatString(": putenv %s ") << vname );
    else
        value = get_string_mlisp();

#ifdef WIN32
    _putenv_s( vname.sdata(), value.sdata() );
#else
    setenv( vname.sdata(), value.sdata(), 1 );
#endif

    return 0;
}

int getenv_command( void )
{
    EmacsString vname;
    if( cur_exec == NULL )
    {
        vname = get_nb_string_interactive(": getenv ");
    }
    else
    {
        vname = get_string_mlisp();
    }
    if( vname.isNull() )
        return 0;

    EmacsString value;

    // check on the special cases first, and do the right thing
    if( vname == "USER" )
    {
        if( users_name.isNull() )
        {
            users_name = users_login_name();
            if( users_name.isNull() )
            {
                users_name = unknown_str;
            }
        }
        value = users_name;
    }

#ifdef vms
    else if( vname == "HOME" )
    {
        vname = u_str ("SYS$LOGIN");
    }

    else if( vname == "PATH" )
    {
        value =  parent_path.isNull() ? current_directory : parent_path;
    }

    else if( vname == "TERM" )
    {
        if (is_motif)
        {
            value = "MOTIF";
        }
        else
        {
            int term = tt->t_cur_attributes.b_type;
            int i = 0;

            while( terminal_idents[i] != 0)
            {
                if( terminal_idents[i] == term )
                {
                    break;
                }
                else
                {
                    i++;
                }

                if( terminal_idents[i] == 0 )
                {
                    static unsigned char term_name_buf[32];

                    sprintfl( term_name_buf, sizeof( term_name_buf ) - 1,
                        u_str("term-id-%d"), term );
                    value = term_name_buf;
                }
                else
                {
                    value = terminal_names[i];
                }
            }
        }
    }
    else
    {
        get_log (vname, lognam);
        value = lognam;
    }
#else
    else
    {
        value = get_config_env( vname );
    }
#endif

    if( value.isNull() )
    {
        error( FormatString("There is no environment variable named %s") << vname );
        return 0;
    }

    ml_value = value;

    return 0;
}

extern int elapse_time(void);

// fetch the number of milli seconds since emacs started up
void SystemExpressionRepresentationElapseTime::fetch_value()
{
    exp_int = elapse_time();
}

void SystemExpressionRepresentationElapseTime::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntReadOnly::assign_value( new_value );
}
