//    Copyright (c) 1982-1994
//    Barry A. Scott and nick Emery

// window management commands

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if 0
#define M_dbg_msg( msg ) _dbg_msg( msg )
#define M_dbg_fn_trace( msg ) _dbg_fn_trace t____( msg )
#else
#define M_dbg_msg( msg ) (void)0
#define M_dbg_fn_trace( msg ) (void)0
#endif

int delete_other_windows( void )
{
    EmacsWindow *w = theActiveView->windows.windows;
    while( w != NULL )
    {
        EmacsWindow *next = w->w_next;
        if( !w->isCurrentWindow() )
            theActiveView->windows.del_win( w );
        w = next;
    }
    return 0;
}



int split_current_window( void )
{
    theActiveView->currentWindow()->split_win_horz()->set_win();
    return 0;
}

int split_current_window_vertically( void )
{
    theActiveView->currentWindow()->split_win_vert()->set_win();
    return 0;
}



int switch_to_buffer( void )
{
    M_dbg_fn_trace( "switch_to_buffer" );

    EmacsString buf_name;

    if( cur_exec == NULL )
        EmacsBuffer::get_esc_word_interactive( "Buffer: ", EmacsString::null, buf_name );
    else
        EmacsBuffer::get_esc_word_mlisp( buf_name );

    EmacsBuffer::set_bfn( buf_name );

    if( theActiveView->currentWindow()->w_next != NULL )
        theActiveView->currentWindow()->tie_win( bf_cur);
    else
        theActiveView->windows.windows->tie_win( bf_cur );
    theActiveView->currentWindow()->w_horizontal_scroll = 1;

    return 0;
}

int pop_to_buffer( void )
{
    EmacsString buf_name;
    if( cur_exec == NULL )
        EmacsBuffer::get_esc_word_interactive( ": pop-to-buffer ", EmacsString::null, buf_name );
    else
        EmacsBuffer::get_esc_word_mlisp( buf_name );

    EmacsBuffer::set_bfn( buf_name );

    if( theActiveView->window_on( bf_cur ) == 0 )
        theActiveView->currentWindow()->w_horizontal_scroll = 1;
    return 0;
}

int temp_use_buffer( void )
{
    EmacsString buf_name;
    if( cur_exec == NULL )
        EmacsBuffer::get_esc_word_interactive( ": temp-use-buffer ", EmacsString::null, buf_name );
    else
        EmacsBuffer::get_esc_word_mlisp( buf_name );

    EmacsBuffer::set_bfn( buf_name );
    return 0;
}

int use_old_buffer( void )
{
    EmacsBuffer *b = getword( EmacsBuffer::, "Buffer: " );
    if( b != NULL )
    {
        b->set_bf();
        if( theActiveView->currentWindow()->w_next != NULL )
            theActiveView->currentWindow()->tie_win( bf_cur );
        else
            theActiveView->windows.windows->tie_win( bf_cur );
    }
    return 0;
}

int delete_window( void )
{
    theActiveView->windows.del_win( theActiveView->currentWindow() );

    theActiveView->currentWindow()->w_buf->set_bf();

    return 0;
}

int left_window( void )
{
    EmacsWindow *w;
    int count = arg;
    int i;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    w = theActiveView->currentWindow();

    for( i=1; i<=count; i += 1 )
        if( w->w_left != 0 )
            w = w->w_left;
        else
        {
            error( "There are no windows to the left of the current window" );
            break;
        }
    w->set_win();

    return 0;
}

int right_window( void )
{
    EmacsWindow *w;
    int count = arg;
    int i;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    w = theActiveView->currentWindow();
    for( i=1; i<=count; i += 1 )
        if( w->w_right != 0 )
            w = w->w_right;
        else
        {
            error( "There are no windows to the right the current window" );
            break;
        }
    w->set_win();

    return 0;
}

int down_window( void )
{
    EmacsWindow *w;
    int horiz_pos;
    int count = arg;
    int i;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    w = theActiveView->currentWindow();

    //
    //    Move to left edge of screen
    //
    horiz_pos = 0;
    while( w->w_left != 0 )
    {
        w = w->w_left;
        horiz_pos++;
    }
    //
    //    Move down the desired number of windows
    //
    for( i=1; i<=count; i += 1 )
    {
        while( w->w_right != 0 )
            w = w->w_right;

        if( w->w_next->w_next != 0 )
            w = w->w_next;
        else
        {
            error( "There are no windows below the current window" );
            break;
        }
    }
    //
    //    Move over to the same horizontal position
    //
    while( horiz_pos > 0 && w->w_right != 0 )
    {
        w = w->w_right;
        horiz_pos--;
    }
    w->set_win();

    return 0;
}

int up_window( void )
{
    EmacsWindow *w;
    int horiz_pos;
    int count = arg;
    int i;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        count = count * numeric_arg (1);

    w = theActiveView->currentWindow();

    //
    //    Move to left edge of screen
    //
    horiz_pos = 0;
    while( w->w_left != 0 )
    {
        w = w->w_left;
        horiz_pos++;
    }
    //
    //    Move up the desired number of windows
    //
    for( i=1; i<=count; i += 1 )
    {
        if( w->w_prev != 0 )
            w = w->w_prev;
        else
        {
            error( "There are no windows above current window" );
            break;
        }
        while( w->w_left != 0 )
            w = w->w_left;
    }
    //
    //    Move over to the same horizontal position
    //
    while( horiz_pos > 0 && w->w_right != 0 )
    {
        w = w->w_right;
        horiz_pos--;
    }
    w->set_win();

    return 0;
}

int next_window( void )
{
    if( theActiveView->currentWindow()->w_next != NULL )
        theActiveView->currentWindow()->w_next->set_win();
    else
        theActiveView->windows.windows->set_win();

    if( theActiveView->currentWindow()->w_next == NULL && minibuf_depth == 0 )
        next_window();

    return 0;
}

int previous_window( void )
{
    EmacsWindow *w = theActiveView->currentWindow();

    if( w->w_prev == NULL )
    {
        w = theActiveView->windows.windows;
        while( w->w_next != NULL )
            w = w->w_next;

        w->set_win();
    }
    else
        w->w_prev->set_win();


    if( theActiveView->currentWindow()->w_next == NULL && minibuf_depth == 0 )
        previous_window();

    return 0;
}



int shrink_window( void )
{
    int delta;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        delta = -arg * numeric_arg (1);
    else
        delta = -arg;

    return theActiveView->windows.change_window_height( delta );
}

int enlarge_window( void )
{
    int delta;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        delta = arg * numeric_arg (1);
    else
        delta = arg;

    return theActiveView->windows.change_window_height( delta );
}

int EmacsWindowGroup::change_window_height(int delta)
{
    EmacsWindow *w;

    if( currentWindow()->w_height + delta < (( currentWindow()->w_buf == minibuf ) ?  1 : 2) )
        goto win_change_error;

    w = current_window;
    while( w->w_right != 0 )
        w = w->w_next;
    if( w->w_next->change_w_height( -delta, 1 ) == 0 )
    {
        w = current_window;
        while( w->w_left != 0 )
            w = w->w_prev;
        if( w->w_prev->change_w_height( -delta, -1 ) == 0 )
            goto win_change_error;
    }


    if( current_window->change_w_height( delta, 0 ) == 0 )
        error( "Emacs bug -- window size change." );
    return 0;
win_change_error:
    error( "Cannot change window size" );
    return 0;
}


int EmacsWindowGroup::change_window_width(int delta)
{
    if( current_window->w_width + delta < 1
    ||    (
        current_window->w_right->change_w_width( -delta, 1 ) == 0
        && current_window->w_left->change_w_width( -delta, -1 ) == 0 ) )
            error( "Cannot change window size" );
    else
        if( current_window->change_w_width( delta, 0 ) == 0 )
            error( "Emacs bug -- window size change." );
    return 0;
}

int narrow_window( void )
{
    int delta = -arg;
    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        delta *= numeric_arg( 1 );

    return theActiveView->windows.change_window_width( delta );
}

int widen_window( void )
{
    int delta = arg;
    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        delta *= numeric_arg( 1 );

    return theActiveView->windows.change_window_width( delta );
}

static void window_horizontal_move
    (
    EmacsWindow *w,
    int add_columns,
    int abs_column
    )
{
    if( abs_column != 0 )
        w->w_horizontal_scroll = abs_column;
    if( add_columns != 0 )
        w->w_horizontal_scroll += add_columns;
    if( w->w_horizontal_scroll < 1 )
        w->w_horizontal_scroll = 1;
    cant_1win_opt = 1;
}

int scroll_one_column_left( void )
{
    window_horizontal_move( theActiveView->currentWindow(), -arg, 0 );
    return 0;
}

int scroll_one_column_right( void )
{
    window_horizontal_move( theActiveView->currentWindow(), arg, 0 );
    return 0;
}

int column_to_left_of_window( void )
{
    window_horizontal_move( theActiveView->currentWindow(), 0, calc_col() );
    return 0;
}

void window_move
    (
    EmacsWindow *w,
    bool down,
    bool lots,
    bool dottop
    )
{
    int n = arg;
    int pos;

    if( cur_exec != 0 && cur_exec->p_nargs > 0 )
        n *= numeric_arg( 1 );

    if( n < 0 )
    {
        down = !down;
        n = -n;
    }
    if( lots )
        n = n * (w->w_height * 4 / 5);
    if( down )
        n = -n - 1;
    if( dottop )
    {
        n = -1;
        pos = dot;
    }
    else
        pos = w->getWindowStart();
    if( n != 0 )
        w->setWindowStart( scan_bf( '\n', pos, n ) );
    w->w_force = w->w_force + 1;
    cant_1line_opt = 1;
}

int scroll_one_line_up( void )
{
    window_move( theActiveView->currentWindow(), false, false, false );
    return 0;
}

int scroll_one_line_down( void )
{
    window_move( theActiveView->currentWindow(), true, false, false );
    return 0;
}

int next_page( void )
{
    window_move( theActiveView->currentWindow(), false, true, false );
    return 0;
}

int previous_page( void )
{
    window_move( theActiveView->currentWindow(), true, true, false );
    return 0;
}

int line_to_top_of_window( void )
{
    window_move( theActiveView->currentWindow(), false, false, true );
    return 0;
}

int page_next_window( void )
{
    EmacsWindow *w = theActiveView->currentWindow()->w_next;
    bool down = arg_state == have_arg;

    if( w == NULL || w->w_next == NULL )
        w = theActiveView->windows.windows;

    if( w->isCurrentWindow() )
        error( "there is no other window" );
    else
    {
        w->w_buf ->set_bf();
        if( argument_prefix_cnt == 1 && arg == 4 )
            arg = 1;
        window_move( w, down, true, false );
        theActiveView->currentWindow()->w_buf ->set_bf();
    }

    return 0;
}

void SystemExpressionRepresentationWindowSize::fetch_value()
{
    exp_int = theActiveView->currentWindow()->w_height;
}

void SystemExpressionRepresentationWindowSize::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntPositive::assign_value( new_value );

    int delta = exp_int - theActiveView->currentWindow()->w_height;
    if( delta != 0 )
        if( delta > 0 )
        {
            int i;

            //
            // expanding takes a little care do it
            // we do it 1 line at a time
            //
            ml_err = 1; // stop the error message being over written
            for( i=1; i<=delta; i += 1 )
                theActiveView->windows.change_window_height( 1 );
            ml_err = 0;    // we never get an error expanding
        }
        else
            // shrink can be done directly
            theActiveView->windows.change_window_height( delta );
}

void SystemExpressionRepresentationWindowWidth::fetch_value()
{
    exp_int = theActiveView->currentWindow()->w_width;
}

void SystemExpressionRepresentationWindowWidth::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntPositive::assign_value( new_value );

    Save<int> old_arg( &arg );
    arg = 1;

    int delta = exp_int - theActiveView->currentWindow()->w_width;
    if( delta != 0 )
        if( delta > 0 )
        {
            //
            // expanding takes a little care to do.
            // we do it 1 line at a time
            //
            ml_err = 1; // stop the error message being over written
            for( int i=1; i<=delta; i += 1 )
                theActiveView->windows.change_window_width( 1 );

            ml_err = 0;    // we never get an error expanding
        }
        else
            // shrink can be done directly
            theActiveView->windows.change_window_width( delta );
}

void SystemExpressionRepresentationWindowColumn::fetch_value()
{
    exp_int = theActiveView->currentWindow()->w_horizontal_scroll;
}

void SystemExpressionRepresentationWindowColumn::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntPositive::assign_value( new_value );

    theActiveView->currentWindow()->w_horizontal_scroll = exp_int;
}

RenditionRegion::RenditionRegion( int start, int end, int colour )
    : rr_next(NULL)
    , rr_start_mark( bf_cur, start, 0 )
    , rr_end_mark( bf_cur, end, 0 )
    , rr_start_pos(0)
    , rr_end_pos(0)
    , rr_colour( colour )
{
    emacs_assert( colour >= (LINE_ATTR_USER|1) && colour <= (LINE_ATTR_USER|8) );
}

RenditionRegion::~RenditionRegion()
{ }


//
//    (apply-colour-to-region start end colour)
//
//    First remove, shrink or split any existing regions
//    that overlap the new region.
//
//    Add the new region if the color is positive,
//    a color or 0 removes color.
//    the regions are maintained in order of start
//    position.
//
void apply_colour_to_region( int start, int end, int colour )
{
    RenditionRegion *rr, **rr_prev, *new_rr;

    // make sure the args are in order
    if( start > end )
    {
        int tmp = start;
        start = end;
        end = tmp;
    }

    // turn markers into ints
    rr = bf_cur->b_rendition_regions;
    while( rr )
    {
        rr->rr_start_pos = rr->rr_start_mark.to_mark();
        rr->rr_end_pos = rr->rr_end_mark.to_mark();
        rr = rr->rr_next;
    }

    // remove overlaps
    rr_prev = &bf_cur->b_rendition_regions;
    rr = *rr_prev;
    while( rr != NULL )
    {
        // the new region is before this one
        if( rr->rr_start_pos > end )
            break;

        // the new region is after this one
        if( rr->rr_end_pos <= start )
        {
            rr_prev = &rr->rr_next;
            rr = rr->rr_next;
            continue;
        }

        // there is some overlap
        if( start > rr->rr_start_pos )
            if( end < rr->rr_end_pos )
            {
                // split this region
                int split_start = rr->rr_start_pos;
                int split_colour = rr->rr_colour & ~LINE_ATTR_USER;

                apply_colour_to_region( split_start, start, split_colour );
                apply_colour_to_region( start, end, colour );
                return;
            }
            else
            {
                // trim off the tail of the old region
                rr->rr_end_mark.set_mark( bf_cur, start, 0 );
                rr->rr_end_pos = start;
                rr_prev = &rr->rr_next;
                rr = rr->rr_next;
                continue;
            }
        else
        {
            // trim off the head of the old region
            rr->rr_start_mark.set_mark( bf_cur, end, 0 );
            rr->rr_start_pos = end;
        }

        // see if this region vanished
        if( rr->rr_start_pos >= rr->rr_end_pos )
        {
            // delink from the chain
            *rr_prev = rr->rr_next;
            // free the rr
            delete rr;
            // move on to the next
            rr = *rr_prev;
        }
        else
            // the new region is before this one
            break;
    }

    // check for a valid colour
    if( colour <= 0 || colour > 8 )
        return;

    new_rr = EMACS_NEW RenditionRegion( start, end, LINE_ATTR_USER|colour );
    if( new_rr == NULL )
        return;

    // link the new rr into the list
    new_rr->rr_next = *rr_prev;
    *rr_prev = new_rr;

    cant_1win_opt = 1;
}

int apply_colour_to_region_cmd( void )
{
    int start, end, colour;

    // colect the three args
    if( check_args( 3, 3 ) )
        return 0;

    start = numeric_arg( 1 );
    end = numeric_arg( 2 );
    colour = numeric_arg( 3 );
    if( ml_err )
        return 0;

    apply_colour_to_region( start, end, colour );
    return 0;
}
