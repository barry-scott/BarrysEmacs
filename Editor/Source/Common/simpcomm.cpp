//
//     Copyright (c) 1982, 1983, 1984, 1985, 1986
//        Barry A. Scott and Nick Emery
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

enum DeleteToWhere_t
{
    ReplaceBuffer = 1,
    AppendToBuffer,
    PrependToBuffer
};

int self_insert( EmacsCharQqq_t c );
int self_insert_command( void );
static void del_to_buf( int n, DeleteToWhere_t where, int doit, const EmacsString &name );
static void line_move( int up, int n );
void insert_buffer( const EmacsString &name );
int left_marker( void );
int right_marker( void );
static int left_or_right_marker( int right );

// Strings
unsigned char *kill_buffer_str = u_str("Kill buffer");
unsigned char *no_mark_set_str = u_str("No mark set in this buffer \"%s\"");
unsigned char *mark_not_set_str = u_str("Mark not set");



int beginning_of_line( void )
{
    set_dot (scan_bf_for_lf (dot, -1));
    return 0;
}

int backward_character( void )
{
    int count = arg;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
    count = count * numeric_arg (1);

    dot_left (count);
    if( dot < bf_cur->first_character() )
    {
        set_dot( bf_cur->first_character() );
        error( "You are at the beginning of the buffer");
    }
    return 0;
}

int exit_emacs( void )
{
    //
    // See if the user wants to specify a command, fetch_ it, and save
    // it for transmission when EMACS really exits back to DCL
    //

    exit_emacs_dcl_command = "";

    if( arg_state == have_arg
    || (cur_exec != 0 && cur_exec->p_nargs >  0) )
    {
        EmacsString p = getstr(": exit-emacs ");

        exit_emacs_dcl_command = p;
    }

    return -1;
}

int delete_next_character( void )
{
    int repeat_count;
    int count = arg;

    if( input_mode == 1 && gui_input_mode_before_delete() )
        return 0;

    if( cur_exec != 0 && cur_exec->p_nargs >  0 )
        count = count * numeric_arg (1);

    if( (repeat_count = del_chars_in_buffer (dot, count, 1)) != 0 )
        dot_right (repeat_count);

    return 0;
}

int end_of_line( void )
{
    int ndot = scan_bf_for_lf( dot ,1 );

    if( dot != ndot )
    {
        set_dot (ndot);
        if( bf_cur->char_at (ndot - 1) == '\n' )
        {
            dot_left( 1 );
            if( dot < bf_cur->first_character() )
            set_dot( bf_cur->first_character() );
        }
    }
    return 0;
}

int forward_character( void )
{
    int count = arg;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    dot_right (count);

    if( dot > bf_cur->num_characters() + 1 )
    {
        set_dot (bf_cur->num_characters() + 1);
        error( "You are at the end of the buffer.");
    }
    return 0;
}

int illegal_operation( void )
{
    ml_err = 1;
    return 0;
}

int delete_previous_character( void )
{
    int count = arg;

    if( input_mode == 1 && gui_input_mode_before_delete() )
        return 0;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    del_chars_in_buffer (dot, count, 0);
    dot_left (count);

    return 0;
}

int newline_and_indent( void )
{
    int dc = cur_indent();

    self_insert( '\n' );
    to_col (dc);
    return 0;
}

int kill_to_end_of_line( void )
{
    int nd;
    int count = arg;
    bool merge = last_proc == kill_to_end_of_line;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    do
    {
        arg = 1;
        nd = dot;
        end_of_line ();
        nd = dot - nd;
        if( nd <= 0 )
            nd = -1;
        if( !merge )
            replace_to_buf (-nd, kill_buffer_str);
        else
            append_to_buf (-nd, kill_buffer_str);
        merge = true;
        count--;
    }
    while( count > 0 );

    return 0;
}

int redraw_display( void )
{
    screen_garbaged = 1;
    return 0;
}

int newline_command( void )
{
    self_insert( '\n' );
    return 0;
}

int current_line_command( void )
{
    ml_value = current_line_number();

    return 0;
}

int next_line( void )
{
    int count = arg;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    line_move( 0, count );
    return 0;
}

int previous_line( void )
{
    int count = arg;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    line_move( 1, count );
    return 0;
}

static void line_move( int up, int n )
{
    static int lastcol;

    int ndot;
    int col = 1;
    int lim = bf_cur->num_characters() + 1;

    if( n == 0 )
        return;

    if( n < 0 )
    {
        n = -n;
        up = ! up;
    }
    if( up )
        n = -n - 1;

    if( last_proc != next_line
    && last_proc != previous_line )
    lastcol = (track_eol != 0 && dot < lim && bf_cur->char_at (dot) == '\n') ? 9999 : cur_col();

    ndot = scan_bf_for_lf( dot, n );
    while( col < lastcol && ndot < lim )
    {
        n = bf_cur->char_at( ndot );
        if( n == '\n' )
            break;
        if( n == '\t' )
            col = ((col - 1) / bf_cur->b_mode.md_tabsize + 1) * bf_cur->b_mode.md_tabsize + 1;
        else
            if( control_character( n ) )
                col += ctl_arrow != 0 ?
                        (term_deccrt != 0
                        && (n == ctl('k')
                        || n == ctl('l')
                        || n == '\r'
                        || n == '\033') ) ?
                            1
                        :
                            2
                    :
                        4;
            else
                col++;

        ndot++;
    }

    set_dot( ndot );
    dot_col = col;
    col_valid = 1;
}

int newline_and_backup( void )
{
    int larg = arg;

    self_insert( '\n' );
    dot_left( larg );
    return 0;
}

int quote_character( void )
{
    int abbrev = bf_cur->b_mode.md_abbrevon;

    bf_cur->b_mode.md_abbrevon = 0;
    self_insert( get_char() );
    bf_cur->b_mode.md_abbrevon = abbrev;

    return 0;
}

int transpose_characters( void )
{
    if( dot >= 3 )
    {
        EmacsCharQqq_t c = bf_cur->char_at( dot - 1 );

        bf_cur->del_back( dot, 1 );
        bf_cur->insert_at( dot - 2, c );
    }

    return 0;
}

int argument_prefix( void )
{
    if( arg_state == no_arg )
    {
        arg = 4;
        argument_prefix_cnt = 1;
    }
    else
    {
        arg = arg * 4;
        argument_prefix_cnt++;
    }
    arg_state = prepared_arg;

    return 0;
}

static int x_region_to_buffer( const char *str, DeleteToWhere_t operation )
{
    if( !bf_cur->b_mark.isSet() )
    {
        error( mark_not_set_str);
        return 0;
    }

    EmacsString name = getnbstr( str );
    del_to_buf( bf_cur->b_mark.to_mark() - dot, operation, false, name );

    return 0;
}

int copy_region_to_buffer( void )
{
    return x_region_to_buffer( ": copy-region-to-buffer ", ReplaceBuffer );
}

int append_region_to_buffer( void )
{
    return x_region_to_buffer( ": append-region-to-buffer ", AppendToBuffer );
}

int prepend_region_to_buffer( void )
{
    return x_region_to_buffer( ": prepend-region-to-buffer ", PrependToBuffer );
}

int delete_to_kill_buffer( void )
{
    if( !bf_cur->b_mark.isSet() )
    {
        error( mark_not_set_str);
        return 0;
    }

    replace_to_buf( bf_cur->b_mark.to_mark() - dot, kill_buffer_str );

    return 0;
}

int yank_from_kill_buffer( void )
{
    int count = arg;
    int i;

    if( input_mode == 1 ) gui_input_mode_before_insert();

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg( 1 );

    for( i=1; i<=count ; ++i )
        insert_buffer( kill_buffer_str );
    return 0;
}

int minus_command( void )
{
    if( arg_state == have_arg && argument_prefix_cnt > 0 )
    {
        arg = -arg;
        argument_prefix_cnt = -1;
        arg_state = prepared_arg;
        return 0;
    }

    self_insert_command();

    return 0;
}

int meta_minus( void )
{
    argument_prefix_cnt = -1;
    arg = -arg;
    arg_state = prepared_arg;

    return 0;
}

int digit_command( void )
{
    if( arg_state == have_arg )
    {
        if( argument_prefix_cnt != 0 )
            arg = 0;

        if( arg < 0 || argument_prefix_cnt < 0 )
            arg = arg * 10 - (last_key_struck - '0');
        else
            arg = arg * 10 + (last_key_struck - '0');

        argument_prefix_cnt = 0;
        arg_state = prepared_arg;
        return 0;
    }

    self_insert_command();

    return 0;
}

int meta_digit (void)
{
    if( arg_state == have_arg )
    {
        if( argument_prefix_cnt != 0 )
            arg = 0;

        if( arg < 0 || argument_prefix_cnt < 0 )
            arg = arg * 10 - (last_key_struck - '0');
        else
            arg = arg * 10 + (last_key_struck - '0');

        argument_prefix_cnt = 0;
        arg_state = prepared_arg;
    }
    else
    {
        arg = last_key_struck - '0';
        argument_prefix_cnt = 0;
        arg_state = prepared_arg;
    }

    return 0;
}

int delete_white_space( void )
{
    int c;
    int p1;
    int p2;

    p1 = dot;
    p2 = bf_cur->num_characters();

    while( p1 <= p2 && ((c = bf_cur->char_at (p1)) == ' ' || c == '\t'))
        p1++;

    p2 = dot;
    do
        p2--;
    while( p2 >= bf_cur->first_character()
    && ((c = bf_cur->char_at (p2)) == ' ' || c == '\t') );

    set_dot (p2 + 1);
    if( (p1 = p1 - p2 - 1) > 0 )
        bf_cur->del_frwd (dot, p1);

    return 0;
}

int self_insert_command( void )
{
    return self_insert( last_key_struck );
}

int self_insert( EmacsCharQqq_t c )
{
    int p;
    int repeat_count = arg;

    arg = 1;

    if( input_mode == 1 ) gui_input_mode_before_insert();

    if( bf_cur->b_mode.md_abbrevon && ! bf_cur->char_is( c, SYNTAX_WORD )
    && (p = dot - 1) >= bf_cur->first_character()
    && bf_cur->char_at_is( p, SYNTAX_WORD ) )
        if( abbrev_expand () != 0 )
            return 0;

    do
    {
        if( c > ' '
        && ((p = dot) > bf_cur->num_characters() || bf_cur->char_at (p) == '\n') )
            if( p > bf_cur->first_character() && cur_col() > bf_cur->b_mode.md_rightmargin )
            {
                EmacsCharQqq_t bfc;

                if( bf_cur->b_mode.md_auto_fill_proc != 0 )
                  {
                    bf_cur->b_mode.md_auto_fill_proc->execute();
                    if( ml_value.exp_type() ==  ISINTEGER
                    && ml_value.asInt() == 0 )
                        return 0;
                }
                else
                {
                    while( (p = dot - 1) >= bf_cur->first_character())
                    {
                        bfc = bf_cur->char_at( p );
                        if( bfc == '\n' )
                        {
                            p = 0;
                            break;
                        }
                        if( !control_character( bfc ) )
                        {
                            dot_col--;
                            dot--;
                        }
                        else
                            dot_left (1);

                        if( (bfc == ' ' || bfc == '\t')
                        && cur_col() <= bf_cur->b_mode.md_rightmargin )
                            break;
                    }
                    if( p >= bf_cur->first_character() )
                    {
                        delete_white_space ();
                        arg = 1;
                        bf_cur->insert_at( dot, '\n' );
                        dot_right (1);
                        to_col (bf_cur->b_mode.md_leftmargin);
                        if( bf_cur->b_mode.md_prefixstring.isNull() )
                            bf_cur->ins_cstr( bf_cur->b_mode.md_prefixstring );
                    }
                    end_of_line ();
                }
            }
        if( bf_cur->b_mode.md_replace
        &&  bf_cur->char_at( dot ) !=  '\n'
        && c != '\n' )
        {
            bf_cur->del_frwd( dot, 1 );
            bf_cur->insert_at( dot, c );
            if( bf_cur->b_modified == 0 )
             {
                redo_modes = 1;
                cant_1line_opt = 1;
            }
            bf_cur->b_modified++;
        }
        else
            bf_cur->insert_at( dot, c );

        dot_right (1);
    }
    while( (repeat_count = repeat_count - 1) > 0 );

    return 0;
}

int set_mark_command( void )
{
    bool gui_input_mode = false;
    ProgramNode *p = cur_exec;
    if( p != NULL && p->p_nargs == 1 )
        gui_input_mode = numeric_arg( 1 ) != 0;

    bf_cur->set_mark( dot, 0, gui_input_mode );
    if( gui_input_mode )
        gui_input_shift_state( true );

    if( interactive() )
        message( "Mark set." );
    cant_1line_opt = 1;

    return 0;
}

int un_set_mark_command( void )
{
    bf_cur->unset_mark();

    cant_1line_opt = 1;

    return 0;
}

int left_marker( void )
{
    return left_or_right_marker( 0 );
}

int right_marker( void )
{
    return left_or_right_marker( 1 );
}

static int left_or_right_marker( int right )
{
    if( !eval_arg( 1 ) )
        return 0;

    if( ml_value.exp_type() != ISMARKER )
    {
        error( FormatString("%s expects its argument to be a marker") <<
            cur_exec->p_proc->b_proc_name );
        return 0;
    }

    int n = ml_value.asMarker()->get_mark();
    Marker *m = EMACS_NEW Marker( bf_cur, n, right );
    ml_value = m;

    return 0;
}

int exchange_dot_and_mark( void )
{
    int old_dot = dot;

    if( !bf_cur->b_mark.isSet() )
        error( FormatString(no_mark_set_str) << bf_cur->b_buf_name);
    else
    {
        set_dot( bf_cur->b_mark.to_mark() );
        bf_cur->set_mark( old_dot, 0, bf_cur->b_gui_input_mode_set_mark );
    }

    return 0;
}

int erase_region( void )
{
    if( !bf_cur->b_mark.isSet() )
        error( FormatString(no_mark_set_str) << bf_cur->b_buf_name);
    else
    {
        int n = bf_cur->b_mark.to_mark() - dot;

        if( n < 0 )
        {
            n = -n;
            dot_left (n);
        }
        bf_cur->del_frwd( dot, n );
    }

    return 0;
}

void replace_to_buf( int n, const EmacsString &name )
{
    del_to_buf( n, ReplaceBuffer, true, name );
}

void append_to_buf( int n, const EmacsString &name )
{
    del_to_buf( n, AppendToBuffer, true, name );
}

static void del_to_buf
    (
    int n,
    DeleteToWhere_t where,
    int del_doner,
    const EmacsString &name
    )
{
    int p = dot;
    EmacsBuffer *old = bf_cur;
    EmacsBuffer *kill = EmacsBuffer::find( name );

    if( kill == NULL )
        kill = EMACS_NEW EmacsBuffer( name );

    if( where == ReplaceBuffer )
        kill->erase_bf();

    if( n < 0 )
    {
        n = -n;
        p = p - n;
    }
    if( p < bf_cur->first_character() )
    {
        n = n + p - bf_cur->first_character();
        p = bf_cur->first_character();
    }
    if( p + n > bf_cur->num_characters() + 1 )
        n = bf_cur->num_characters() + 1 - p;

    if( n <= 0 )
        return;

    bf_cur->gap_to( p );
    kill->set_bf();
    switch( where )
    {
    case PrependToBuffer:
        // prepend to the start of the buffer to the start of the buffer
        set_dot( bf_cur->first_character() );
        bf_cur->ins_cstr( old->ref_char_at( p ), n );

        // leave dot at the start
        set_dot( bf_cur->first_character() );
        break;

    case ReplaceBuffer:
    case AppendToBuffer:
        // append to the end of the buffer
        set_dot( bf_cur->num_characters() + 1 );
        bf_cur->ins_cstr( old->ref_char_at( p ), n );

        // move dot to the end of the buffer
        set_dot( bf_cur->num_characters() + 1 );
        break;
    }

    old->set_bf();
    if( del_doner )
    {
        bf_cur->del_frwd( p, n );
        set_dot (p);
    }
}

// insert the contents of the named buffer at the current position
void insert_buffer ( const EmacsString &name )
{
    EmacsBuffer *who = EmacsBuffer::find(name);

    if( who == NULL )
    {
        error( FormatString("Non-existant buffer: \"%s\"") << name );
        return;
    }
    if( who == bf_cur )
    {
        error( "Inserting a buffer into itself.");
        return;
    }
    bf_cur->insert_buffer( who );
}



int move_to_comment_column( void )
{
    bf_cur->b_mode.md_leftmargin = cur_col() == 1 ? 1 : bf_cur->b_mode.md_commentcolumn;
    to_col( bf_cur->b_mode.md_leftmargin );

    return 0;
}



int widen_region( void )
{
    bf_cur->b_mode.md_headclip = 1;
    bf_cur->b_mode.md_tailclip = 0;
    cant_1win_opt = 1;

    return 0;
}

int narrow_region( void )
{
    if( !bf_cur->b_mark.isSet() )
        error( FormatString(no_mark_set_str) << bf_cur->b_buf_name );
    else
    {
        int lo = bf_cur->b_mark.to_mark();
        int hi = dot;

        if( hi < lo )
        {
            int t = hi;

            hi = lo;
            lo = t;
        }

        bf_cur->b_mode.md_headclip = lo;
        bf_cur->b_mode.md_tailclip = bf_cur->unrestrictedSize() + 1 - hi;
        cant_1win_opt = 1;
    }

    return 0;
}

int save_restriction( void )
{
    int rv;

    EmacsBuffer *b = bf_cur;
    EmacsBuffer *b2;

    Marker ml( bf_cur, bf_cur->b_mode.md_headclip, 0);
    Marker mh( bf_cur, bf_cur->unrestrictedSize() + 1 - bf_cur->b_mode.md_tailclip, 1);

    rv = progn_command();

    b2 = bf_cur;
    b->b_mode.md_headclip = ml.to_mark();
    b->b_mode.md_tailclip = bf_cur->unrestrictedSize() + 1 - mh.to_mark();
    if( dot < bf_cur->first_character() )
        set_dot( bf_cur->first_character() );
    if( dot > bf_cur->num_characters() )
        set_dot( bf_cur->num_characters() + 1 );
    if( bf_cur != b2 )
        b2->set_bf();
    cant_1win_opt = 1;

    return rv;
}

//
// del_chars_in_buffer deletes n characters in the current buffer starting
// at dot. It takes account of the state of replace-mode so that if you
// delete characters in the middle of a line, then they are replaced with
// a space. Characters deleted at the end of a line actually disappear.
//
int del_chars_in_buffer ( int position, int number_of_characters, int fwd )
{
    int replace = 0;

    if( bf_cur->b_mode.md_replace != 0 )
    {
        //
        // Its a replace mode delete.
        // Perform the replace mode check_s, and do the correct
        // combination of inserts and deletes
        //
        if( fwd != 0 )
        {
            int i;

            for( i=0; i<=number_of_characters - 1; i += 1 )
            {
                EmacsCharQqq_t ch = bf_cur->char_at( position + i );

                bf_cur->del_frwd( position + i, 1 );
                if( ch != '\n' )
                {
                    bf_cur->insert_at (position + i, ' ');
                    replace++;
                }
            }
            if( bf_cur->char_at( position + replace ) == '\n' )
            {
                bf_cur->del_frwd( position, replace );
                replace = 0;
            }
        }
        else
        {
            int repl = bf_cur->char_at(position) != '\n';

            for( int i=1; i<number_of_characters; i++ )
            {
                EmacsCharQqq_t ch = bf_cur->char_at(position - i);

                bf_cur->del_back( position - i + 1, 1 );
                if( repl && ch != '\n' )
                {
                    bf_cur->insert_at( position - i, ' ' );
                    replace++;
                }
            }
        }
    }
    else
        //
        // Its an insert mode delete, just use the
        //  usual delete routines
        //
        if( fwd != 0 )
            bf_cur->del_frwd( position, number_of_characters );
        else
            bf_cur->del_back( position, number_of_characters );

    return replace;
}
