//
//    Copyright (c) 1982-2002
//        Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

int goto_window_at_x_y( void );
int move_dot_to_x_y( void );
void dest_window_ring( EmacsWindowRing *wr );
void push_window_ring( void );
void pop_window_ring( void );
int scan_bf_for_lf( int n, int k );

int vertical_bar_width = 1;

EmacsView *theActiveView;
EmacsWindowRing *pushed_windows = 0;
SystemExpressionRepresentationIntBoolean default_wrap_lines;
SystemExpressionRepresentationIntPositive scroll_step;
SystemExpressionRepresentationIntPositive scroll_step_horizontal( 40 );
SystemExpressionRepresentationIntBoolean automatic_horizontal_scroll( 1 );
SystemExpressionRepresentationIntBoolean quick_rd;
SystemExpressionRepresentationIntBoolean ctl_arrow;
SystemExpressionRepresentationString global_mode_string;
SystemExpressionRepresentationIntBoolean pop_up_windows( 1 );
SystemExpressionRepresentationIntPositive split_height_threshold( 20 );

static int mouse_x;             // The X screen coordinate of the mouse
static int mouse_y;             // The Y screen coordinate of the mouse

static int use_time;            // A counter used to set the time of last use
                                // of a window: for selecting the LRU window

// Move dot to the buffer and character corresponding to some absolute X
// and Y coordinate.
static int mouse_hit;
static EmacsWindow *mouse_win_; // The window corresponding to (mouse_x,mouse_y)
static int mouse_dot_;          // The character position corresponding to (mouse_x,mouse_y)

const int MOUSE_ON_MODELINE( 1 );
const int MOUSE_ON_VERTICAL( 2 );

#if 0
#define M_dbg_msg( msg ) _dbg_msg( msg )
#define M_dbg_fn_trace( msg ) _dbg_fn_trace t____( msg )

static EmacsString getWindowDescription( EmacsWindow *window )
{
    if( window != NULL )
        return window->getDescription();
    else
        return EmacsString( "<window NULL>" );
}

#else
#define M_dbg_msg( msg )
#define M_dbg_fn_trace( msg )
#endif


static void setMouseHitPosition( int pos, EmacsWindow *window )
{
    M_dbg_msg( FormatString( "setMouseHitPosition( %d, %s )" ) << pos << getWindowDescription( window ) );

    mouse_dot_ = pos;
    mouse_win_ = window;
}

int goto_window_at_x_y( void )
{
    int gui_mode_goto = 0;
    bool return_dot = false;
    if( cur_exec == NULL )
    {
        mouse_x = get_number_interactive( "X coordinate: " );    if( ml_err ) return 0;
        mouse_y = get_number_interactive( "Y coordinate: " );    if( ml_err ) return 0;
    }
    else
    {
        if( check_args( 2, 4 ) )
            return 0;
        mouse_x = numeric_arg( 1 );
        mouse_y = numeric_arg( 2 );
        // optional third arg
        if( cur_exec->p_nargs >= 3 )
            gui_mode_goto = numeric_arg( 3 );
        if( cur_exec->p_nargs >= 4 )
            return_dot = true;
    }
    if( ml_err )
        return 0;

    M_dbg_fn_trace( FormatString( "goto_window_at_x_y  X=%d, Y=%d GUI=%d ret_dot=%d" ) << mouse_x << mouse_y << gui_mode_goto << int(return_dot) );

    mouse_hit = 0;
    setMouseHitPosition( 0, NULL );

    cant_1line_opt = 1;;
    theActiveView->do_dsp();
    if( mouse_win_ == NULL )
        mouse_hit = -1;
    else
    {
        mouse_win_->set_win();
        //
        //    Do not set the dot if the mouse points to the
        //    mode line or vertical divider.
        //
        if( return_dot )
        {
            VariableName *var = cur_exec->arg(4)->name();
            Expression new_dot( EMACS_NEW Marker( bf_cur, mouse_dot_, 0 ) );
            set_var( var, new_dot );
        }
        else
        {
            if( mouse_hit == 0 )
            {
                if( gui_mode_goto )
                    gui_set_dot( mouse_dot_ );
                else
                    set_dot( mouse_dot_ );
            }
        }
    }

    M_dbg_msg( FormatString( "goto_window_at_x_y  mouse_hit=%d mouse_win=%s mouse_dot_=%d" )
                << mouse_hit << getWindowDescription(mouse_win_) << mouse_hit );

    ml_value = mouse_hit;

    return 0;
}

int window_dimensions(void)
{
    if( check_args( 0, 0 ) )
        return 0;

    // initial values
    int top = 1;
    int left = 1;

    // starting from the current window ...
    EmacsWindow *w = theActiveView->currentWindow();
    // move left adding window widths
    while( w->w_left != NULL )
    {
        w = w->w_left;
        left += w->w_width + vertical_bar_width;
    }
    // move upwards to the top adding window heights
    while( w->w_prev != NULL )
    {
        w = w->w_prev;
        if( w->w_left == NULL )
        {
            top += w->w_height;
        }
    }

    // back to the current window
    w = theActiveView->currentWindow();

    EmacsArray bounds( 1, 4 );

    bounds( 1 ) = top;            // top
    bounds( 2 ) = left;            // left
    bounds( 3 ) = top + w->w_height - 1;    // bottom
    bounds( 4 ) = left + w->w_width - 1;    // right

    ml_value = bounds;

    return 0;
}

int EmacsWindow::last_used_window_id( 0 );


EmacsWindow::EmacsWindow( EmacsWindowGroup &_group )
: w_next( NULL )
, w_prev( NULL )
, w_right( NULL )
, w_left( NULL )
, w_buf( NULL )
, w_height( 0 )
, w_width( 0 )
, w_lastuse( 0 )
, w_force( 0 )
, w_horizontal_scroll( 1 )
, w_user_horizontal_scrolled( false )
, w_window_id( ++last_used_window_id )
, w_start_()
, w_dot_()
, w_mark_()
, w_gui_input_mode_set_mark_( false )
, group( &_group )
{
}

EmacsWindow::EmacsWindow( EmacsWindow &w, EmacsWindowGroup &_group )
: w_next( NULL )
, w_prev( NULL )
, w_right( NULL )
, w_left( NULL )
, w_buf( w.w_buf )
, w_height( w.w_height )
, w_width( w.w_width)
, w_lastuse( w.w_lastuse )
, w_force( w.w_force )
, w_horizontal_scroll( w.w_horizontal_scroll )
, w_user_horizontal_scrolled( false )
, w_window_id( ++last_used_window_id )
, w_start_( w.w_start_ )
, w_dot_( w.w_dot_ )
, w_mark_( w.w_mark_ )
, w_gui_input_mode_set_mark_( false )
, group( &_group )
{
}

EmacsWindow::~EmacsWindow()
{
}

EmacsString EmacsWindow::getDescription() const
{
    return FormatString("<EmacsWindow id=%d>") << w_window_id;
}


//
//    Get and Set
//
void EmacsWindow::setWindowStart( int pos )
{
    M_dbg_msg( FormatString( "%s::setWindowStart( %d )" ) << getDescription() << pos );

    w_start_.set_mark( bf_cur, pos, 0 );
}

void EmacsWindow::unsetWindowStart()
{
    w_start_.unset_mark();
}

int EmacsWindow::getWindowStart()
{
    return w_start_.get_mark();
}

void EmacsWindow::setWindowDot( const Marker &mark )
{
    M_dbg_msg( FormatString("setWindowDot %s w_buf=%s w_mark=%s w_dot=%s <= %s")
            << getDescription()
            << w_buf->b_buf_name
            << getWindowMark().asString()
            << getWindowDot().asString()
            << mark.asString() );

    w_dot_ = mark;
}

void EmacsWindow::unsetWindowDot()
{
    M_dbg_msg( FormatString("unsetWindowDot %s w_buf=%s w_mark=%s w_dot=%s")
            << getDescription()
            << w_buf->b_buf_name
            << getWindowMark().asString()
            << getWindowDot().asString() );
    w_dot_.unset_mark();
}


const Marker &EmacsWindow::getWindowDot() const
{
    return w_dot_;
}

void display_break()
{
    return;
}

void EmacsWindow::setWindowMark( const Marker &mark, bool gui_input_mode_set_mark )
{
    M_dbg_msg( FormatString("setWindowMark  w_buf=%s w_mark=%s <= %s w_dot=%s gui=%d <= %d")
            << getDescription()
            << w_buf->b_buf_name
            << getWindowMark().asString()
            << mark.asString()
            << getWindowDot().asString()
            << int(w_gui_input_mode_set_mark_) << int(gui_input_mode_set_mark) );

    // break is second window, but not the minibuffer
    if( w_prev != NULL && w_next != NULL && mark.isSet() )
        display_break();

    w_mark_ = mark;
    w_gui_input_mode_set_mark_ = gui_input_mode_set_mark;
}

void EmacsWindow::unsetWindowMark()
{
    M_dbg_msg( FormatString("unsetWindowMark %s w_buf=%s w_mark=%s w_dot=%s gui=%d")
            << getDescription()
            << w_buf->b_buf_name
            << getWindowMark().asString()
            << getWindowDot().asString()
            << int(w_gui_input_mode_set_mark_) );

    w_mark_.unset_mark();
    w_gui_input_mode_set_mark_ = false;
}

const Marker &EmacsWindow::getWindowMark() const
{
    return w_mark_;
}

bool EmacsWindow::getWindowGuiInputModeSetMark() const
{
    return w_gui_input_mode_set_mark_;
}


// initialize the window system
void init_win()
{
    EmacsWindowGroup::init();
}

void EmacsWindowGroup::init( void )
{
}

void EmacsWindowGroup::restore(void)
{
    theActiveView->fit_windows_to_screen();
}

// set the current window by user
void EmacsWindow::set_win()
{
    // record the LRU window for the window
    w_lastuse = use_time++;

    set_win_internal();
}

void EmacsWindow::set_win_internal()
{
    M_dbg_fn_trace( "EmacsWindow::set_win_internal" );

    M_dbg_msg( FormatString("set_win_internal() dot=%d old_w=%s, new_w=%s") << dot << getWindowDescription(group->current_window) << getDescription() );

    int saved_input_mode = input_mode; input_mode = 0;

    // save buffer state of current windows buffer
    group->current_window->w_buf->saveGlobalState();

    M_dbg_msg( FormatString("set_win old %s w_buf=%s w_dot=%s w_mark=%s new w_buf")
        << getWindowDescription(group->current_window)
        << group->current_window->w_buf->b_buf_name
        << group->current_window->getWindowDot().asString()
        << group->current_window->getWindowMark().asString() );

    M_dbg_msg( FormatString("set_win new %s w_buf=%s w_dot=%s w_mark=%s new w_buf")
        << getDescription()
        << w_buf->b_buf_name
        << getWindowDot().asString()
        << getWindowMark().asString() );

    // switch to this buffer
    group->current_window = this;

    // restore this window's buffer's state
    w_buf->restoreGlobalState();

    // restore the mark from the window
    w_buf->b_mark = getWindowMark();
    w_buf->b_gui_input_mode_set_mark = getWindowGuiInputModeSetMark();

    cant_1win_opt = 1;

    input_mode = saved_input_mode;

    M_dbg_msg( FormatString("set_win_internal() done dot=%d") << dot );
}

EmacsWindow *EmacsWindow::split_win_horz()
{
    if( w_height <= 4 )
    {
        error( "You cannot have windows smaller than two lines high." );
        return this;
    }

    //
    //    Find the right most window from here
    //
    EmacsWindow *w = this;
    while( w->w_right != NULL )
        w = w->w_right;

    EmacsWindow *n = EMACS_NEW EmacsWindow( *group );
    n->w_prev = w;
    n->w_width = group->view->t_width;
    n->w_next = w->w_next;
    w->w_next = n;
    if( n->w_next != 0 )
        n->w_next->w_prev = n;
    n->w_height = w->w_height / 2;
    n->w_lastuse = 0;
    n->w_buf = w->w_buf;
    n->w_horizontal_scroll = w->w_horizontal_scroll;
    n->setWindowDot( Marker( n->w_buf, bf_cur == w->w_buf ? dot : w->getWindowDot().get_mark(), 0 ) );
    n->setWindowMark( w->getWindowMark(), w->getWindowGuiInputModeSetMark() );
    n->setWindowStart( w->getWindowStart() );
    while( w != 0 )
    {
        w->w_height = w->w_height - n->w_height;
        w = w->w_left;
    }

    cant_1win_opt = 1;

    return n;
}

EmacsWindow *EmacsWindow::split_win_vert()
{
    if( (w_width - vertical_bar_width) < 1 )
    {
        error( "You cannot have windows smaller than 1 column wide." );
        return this;
    }

    EmacsWindow *n = EMACS_NEW EmacsWindow( *group );
    n->w_force = 0;
    if( w_right != 0 )
        w_right->w_left = n;
    n->w_left = this;
    n->w_right = w_right;
    w_right = n;

    n->w_prev = this;
    n->w_next = w_next;
    w_next = n;
    if( n->w_next != 0 )
        n->w_next->w_prev = n;
    n->w_height = w_height;
    n->w_width = w_width / 2;
    w_width = w_width - n->w_width - vertical_bar_width;
    n->w_lastuse = 0;
    n->w_buf = w_buf;
    n->w_horizontal_scroll = w_horizontal_scroll;
    n->setWindowDot( Marker( n->w_buf, bf_cur == w_buf ? dot : getWindowDot().get_mark(), 0 ) );
    n->setWindowMark( getWindowMark(), getWindowGuiInputModeSetMark() );
    n->setWindowStart( getWindowStart() );

    cant_1win_opt = 1;

    return n;
}

// split the largest window, and return a pointer to it
EmacsWindow *EmacsWindowGroup::split_largest_window( void )
{
    EmacsWindow *bestw = NULL;
    int besth;
    besth = -1;

    EmacsWindow *w = windows;
    while( w != 0 )
    {
        if( w->w_height > besth )
        {
            besth = w->w_height;
            bestw = w;
        }
        w = w->w_next;
    }

    return bestw->split_win_horz();
}

// Delete the indicated window
void EmacsWindowGroup::del_win( EmacsWindow *w )
{
    if( w->w_next == NULL )    // Can't delete the last window -- it's the minibuf
        return;

    if( w == mouse_win_ )
        mouse_win_ = NULL;

    // last user window?
    if( w->w_next->w_next == NULL && w->w_prev == NULL )
    {
        // default the buffer in this window
        EmacsBuffer::set_bfn( "main" );

        w->unsetWindowDot();
        w->unsetWindowMark();
        w->unsetWindowStart();

        w->tie_win( bf_cur );
        w->w_horizontal_scroll = 1;
        w->set_win();

        return;
    }

    //
    //    See if this is a vertically split window.
    //    In which case adjust the width. Otherwise the
    //    heights need adjusting.
    //
    if( w->w_left != 0 || w->w_right != 0 )
    {
        //
        //    Combine vertical split windows
        //
        w->w_height = 0;        // Allow us to fall into the horizontal code
        if( w->w_right != NULL )
        {
            EmacsWindow *w2 = w->w_right;
            w2->w_left = w->w_left;
            w2->w_width = w2->w_width + w->w_width + vertical_bar_width;
            if( w->w_left != NULL )
                w->w_left->w_right = w2;
        }
        else if( w->w_left != NULL )
        {
            EmacsWindow *w2 = w->w_left;
            w2 = w->w_left;
            w2->w_right = w->w_right;
            w2->w_width = w2->w_width + w->w_width + vertical_bar_width;
            if( w->w_right != NULL )
                w->w_right->w_left = w2;
        }
    }

    //
    //    Combine horizontally split windows and unlink a vertically
    //    split window.
    //
    if( w->w_prev != NULL )
    {
        EmacsWindow *w2 = w->w_prev;
        w2->w_next = w->w_next;
        while( w2 != NULL )
        {
            w2->w_height = w2->w_height + w->w_height;
            w2 = w2->w_left;
        }
    }
    else
    {
        // what does this test for
        if( w->w_next == NULL )
            return;

        EmacsWindow *w2 = w->w_next;

        // only update windows if it point to us
        if( windows == w )
            windows = w->w_next;

        while( w2 != NULL )
        {
            w2->w_height = w2->w_height + w->w_height;
            w2 = w2->w_right;
        }
    }
    if( w->w_next != NULL )
        w->w_next->w_prev = w->w_prev;

    //
    //    If deleting the current window we need to pick a new
    //    current window.
    //
    if( w == current_window )
    {
        if( w->w_buf != NULL )
        {
            if( w->w_prev != 0 )
            {
                w->w_prev->set_win();
            }
            else
            {
                windows->set_win();
            }
        }
        else
        {
            current_window = w->w_next;
        }
    }

    //
    //    Free up the windows resources.
    //
    delete w;

    //
    //    Cannot assume that window struct is unchanged
    //
    cant_1win_opt = 1;
}

// tie a window to a buffer
void EmacsWindow::tie_win( EmacsBuffer *b )
{
    if( b == NULL || this == NULL || w_buf == b )
        return;

    M_dbg_msg( FormatString("EmacsWindow::tie_win( %s ) dot=%d b_ephemeral_dot=%d, window=%s")
        << b->b_buf_name << dot << b->b_ephemeral_dot << getDescription() );

    w_buf = b;
    w_force = 0;
    w_horizontal_scroll = 1;
    w_lastuse = use_time++;
    int newdot = b == bf_cur ? dot : b->b_ephemeral_dot;
    setWindowDot( Marker( b, newdot, 0 ) );

    // always unset the mark
    unsetWindowMark();
    // only set the mark in the window if it is set in the buffer
    if( b->b_mark.isSet() )
        setWindowMark( b->b_mark, b->b_gui_input_mode_set_mark );

    setWindowStart( 1 );
}

//
// Change the height of the pointed to window by delta; returns true iff
// the change succeeds. Chains forward if dir gtr 0, backward if dir lss 0 in
// attempting to find a suitable window.
//
int EmacsWindow::change_w_height( int delta, int dir )
{
    if( this == NULL )
        return 0;

    //
    //    Start from the left most window
    //
    EmacsWindow *w = this;
    while( w->w_left != 0)
        w = w->w_left;

    //
    //    Adjust this window
    //
    while( w != 0 )
        if( w->w_height + delta >= (( w->w_buf == minibuf ) ?  1 : 2)
        && (dir == 0 || w->w_buf != minibuf) )
        {
            cant_1win_opt = 1;

            //
            //    Adjust all the heights across the screen
            //
            do
            {
                w->w_height = w->w_height + delta;
                w = w->w_right;
            }
            while( w != 0 );

            return 1;
        }
        else
            switch( dir )
            {
            case 0:    return 0;
            case 1:
                while( w != 0 && w->w_right != 0 )
                    w = w->w_right;
                if( w != 0 )
                    w = w->w_next;
                break;
            case -1:
                w = w->w_prev;
                while( w != 0 && w->w_left != 0 )
                    w = w->w_left;
                break;
            }

    return 0;
}

//
// Change the width of the pointed to window by delta; returns true iff
// the change succeeds. Chains forward if dir gtr 0, backward if dir lss 0 in
// attempting to find a suitable window.
//
int EmacsWindow::change_w_width( int delta, int dir )
{
    EmacsWindow *w = this;

    //
    //    Adjust this window
    //
    while( w != 0 )
        if( w->w_width + delta >= 1 )
        {
            cant_1win_opt = 1;

            w->w_width = w->w_width + delta;

            return 1;
        }
        else
            switch( dir )
            {
            case 0:        return 0;
            case 1:        w = w->w_right; break;
            case -1:    w = w->w_left; break;
            }

    return 0;
}

// find the least recently used window; split if only one window
EmacsWindow *EmacsWindowGroup::LRU_win( void )
{
    EmacsWindow *bestw = NULL;
    int youngest = INT_MAX;
    int largest_height = 0;
    EmacsWindow *w = windows;
    while( w->w_next != NULL )
    {
        if( w->w_buf->unrestrictedSize() == 0 )
            return w;
        if( w->w_lastuse < youngest && w != current_window )
        {
            bestw = w;
            youngest = w->w_lastuse;
        }
        if( w->w_height > largest_height )
            largest_height = w->w_height;
        w = w->w_next;
    }
    if( view->t_length > 5 && (bestw == NULL || largest_height >= split_height_threshold) )
        bestw = split_largest_window();
    if( bestw == NULL )
        bestw = windows;
    return bestw;
}

//
// make sure that the current window is on the given buffer, either
// by picking the window that already contains it, the LRU window,
// or some brand new window
//
int EmacsWindowGroup::window_on( EmacsBuffer *bf )
{
    bool on_screen = false;
    EmacsWindow *w = current_window;
    if( w->w_buf != bf )
    {
        w = windows;
        while( w != NULL )
        {
            if( w->w_buf == bf )
            {
                on_screen = true;
                break;
            }
            w = w->w_next;
        }
    }
    if( w == NULL )
        w = pop_up_windows || current_window->w_next == NULL ? LRU_win() : current_window;

    w->tie_win( bf );
    w->set_win();

    return on_screen;
}

//
//    Window ring save and restore mechanism
//

// Copy a window and return a pointer to the new copy

EmacsWindowRing::EmacsWindowRing( EmacsWindowGroup &group )
    : windows( group )
    , wr_pushed( NULL )
    , wr_ref_count( 1 )
{ }

EmacsWindowRing::~EmacsWindowRing()
{
    emacs_assert( wr_ref_count == 0 );
}

void dest_window_ring( EmacsWindowRing *wr )
{
    wr->wr_ref_count = wr->wr_ref_count - 1;
    if( wr->wr_ref_count > 0 )
        return;

    delete wr;
}

void SystemExpressionRepresentationCurrentWindows::fetch_value()
{
    exp_windows = EMACS_NEW EmacsWindowRing( theActiveView->windows );
}

void SystemExpressionRepresentationCurrentWindows::assign_value( ExpressionRepresentation *new_value )
{
    EmacsWindowRing *wr = new_value->asWindows();

    theActiveView->set_current_windows( wr->windows );
}

void push_window_ring( void )
{
    EmacsWindowRing *wr = EMACS_NEW EmacsWindowRing( theActiveView->windows );

    // push ring
    wr->wr_pushed = pushed_windows;
    pushed_windows = wr;
}

void pop_window_ring( void )
{
    // install pushed windows
    EmacsWindowRing *wr = pushed_windows;
    if( wr == NULL )
        debug_invoke();

    pushed_windows = wr->wr_pushed;

    theActiveView->set_current_windows( wr->windows );

    dest_window_ring( wr );
}

void EmacsWindowGroup::set_current_windows( EmacsView *new_view, EmacsWindowGroup &from )
{
    deleteWindows();
    copyWindows( from );

    if( current_buffer.isSet() )
        current_buffer.m_buf->set_bf();

    if( bf_cur == NULL )
        windows->w_buf->set_bf();

    if( buffer_is_visible )
        window_on( bf_cur );

    fit_windows_to_screen( new_view );
}

//
// if the length of the screen has change adjust the windows so that
// they fill the screen
//
void EmacsWindowGroup::fit_windows_to_screen( EmacsView *new_view )
{
    view = new_view;

    int old_len;
    int old_wid = 0;
    EmacsWindow *w;

    old_len = 0;
    w = windows;
    while( w != 0 )
    {
        if( w->w_right == 0 )
            old_len = old_len + w->w_height;
        old_wid = w->w_width;    // taken from minibuffer window

        w = w->w_next;
    }

    //
    //    Go around the windows adjusting them to the new size
    //

    //
    //    If the page is longer add the new length to the windows above the
    //    minibuffer.
    //
    if( old_len < view->t_length )
    {

        int increase;

        increase = view->t_length - old_len;

        w = windows;
        while( w->w_next != 0 )
            w = w->w_next;
        w = w->w_prev;    // the window above the minibuffer

        do
        {
            w->w_height = w->w_height + increase;
            w = w->w_left;
        }
        while( w != 0 );

        screen_garbaged = 1;
    }

    //
    //    If the page is shorter reduce the height of the last windows until they
    //    fit. If this is not possible delete the last windows and try again.
    //
    if( old_len > view->t_length )
    {
        int decrease;

        decrease = old_len - view->t_length;

        w = windows;
        while( w->w_next != 0 )
            w = w->w_next;
        w = w->w_prev;    // the window above the minibuffer

        while( w->w_height - decrease < 2 )
        {
            w = w->w_prev;
            del_win( w->w_next );
        }

        do
        {
            w->w_height = w->w_height - decrease;
            w = w->w_left;
        }
        while( w != 0 );

        screen_garbaged = 1;
    }

    //
    //    If the width has increased then add the width to the right most window if each
    //    window row.
    //
    if( old_wid < view->t_width )
    {
        int increase;

        increase = view->t_width - old_wid;

        w = windows;
        while( w != 0 )
        {
            if( w->w_right == 0 )
                w->w_width = w->w_width + increase;
            w = w->w_next;
        }

        screen_garbaged = 1;
    }

    //
    //    If the width of the screen has decreased then reduce the size of the
    //    right most window until it fits. If the right most window is too small
    //    delete it and try again
    //
    if( old_wid > view->t_width )
    {
        int decrease;

        decrease = old_wid - view->t_width;

        w = windows;
        while( w != 0 )
        {
            if( w->w_right == 0 )
            {
                while( w->w_width - decrease <= 0 )
                {
                    w = w->w_left;
                    del_win( w->w_right );
                }
                w->w_width = w->w_width - decrease;
            }
            w = w->w_next;
        }

        screen_garbaged = 1;
    }
}

void EmacsWindowGroup::de_ref_buf( EmacsBuffer *b )
{
    QueueIterator<EmacsWindowGroup> it( header );
    while( it.next() )
        it.value()->derefBufferForOneWindowGroup( b );
}

void EmacsWindowGroup::derefBufferForOneWindowGroup( EmacsBuffer *b )
{
    EmacsWindow *w = windows;
    while( w != NULL )
        if( w->w_buf == b )
        {
            EmacsWindow *next = w->w_next;
            del_win( w );
            w = next;
        }
        else
            w = w->w_next;
}

void foo()
{
    return;
}


//
// full screen update -- called when absolutely nothing is known or
// many things have been fiddled with
//
int EmacsWindowGroup::full_upd( int &cant_ever_opt )
{
    M_dbg_fn_trace( FormatString( "EmacsWindowGroup::full_upd( %d )" ) << ++unique_count );

    EmacsWindow *cur_win = current_window;
    EmacsBuffer *hit_bf = current_window->w_buf;

    EmacsWindow *w = windows;
    int sline = 1;
    int scol = 1;
    int hits = 0;
    int slow = 0;

    while( w != NULL )
    {
        bool is_users_current_window = cur_win == w;

        w->set_win_internal();
        if( bf_cur == hit_bf )
            hits++;

        slow = slow || w->w_force != 0;

        int pos = w->getWindowStart();
        if( pos < bf_cur->first_character() )
            w->setWindowStart( bf_cur->first_character() );
        else if( pos > bf_cur->num_characters() + 1 )
            w->setWindowStart( bf_cur->num_characters() + 1 );

        pos = w->getWindowDot().to_mark();
        if( pos < bf_cur->first_character() )
            w->setWindowDot( Marker( w->w_buf, set_dot( bf_cur->first_character() ), 0 ) );
        else if( pos > bf_cur->num_characters() + 1 )
            w->setWindowDot( Marker( w->w_buf, set_dot( bf_cur->num_characters() + 1 ), 0 ) );

        int dumpstate = 0;

        while( dumpstate >= 0 )
        {
            int horizontal_scroll = w->w_horizontal_scroll;
            bool found_dot = w->dump_win( is_users_current_window, sline, scol, dumpstate == 0 );
            M_dbg_msg( FormatString("full_upd: w=%s sline=%d  w_start=%s dot=%d found_dot=%d dumpstate=%d")
                << w->getDescription() << sline << w->w_start_.asString() << dot << found_dot << dumpstate );

            if( found_dot )
            {
                // dot found

                // check if the horizontal_scroll moved
                if( horizontal_scroll != w->w_horizontal_scroll )
                    // dump the window again
                    w->dump_win( is_users_current_window, sline, scol, dumpstate == 0 );
                break;
            }
            if( w->w_next != NULL )
                foo();
            slow++;
            if( w->w_force )
            {
                if( dumpstate != 0 )
                    set_dot( w->getWindowStart() );
                else
                    set_dot( scan_bf_for_lf( w->getWindowStart(), w->w_height/2) );

                if( w != current_window )
                    w->setWindowDot( Marker( w->w_buf, dot, 0 ) );
                if( dumpstate != 0 )
                    w->w_force = 0;
                dumpstate++;
            }
            else
            {
                int next = 0;
                switch( dumpstate )
                {
                case 0:
                {
                    dumpstate++;
                    if( scroll_step > 0
                    && scroll_step < (w->w_height / 2) )
                    {
                        int old = w->getWindowStart();
                        next = scan_bf_for_lf
                            (
                            old,
                            (( old > dot ) ?
                                -scroll_step - 1
                             :
                                int(scroll_step)) );
                        if( dot < next )
                        {
                            next = scan_bf_for_lf
                                (
                                dot,
                                -(w->w_height / 2)
                                );
                        }
                    }
                    else
                        next = scan_bf_for_lf( dot, -(w->w_height/ 2) );
                }
                    break;
                case 1:
                {
                    next = scan_bf_for_lf( dot, -(w->w_height/ 2) );
                    dumpstate++;
                }
                    break;
                case 2:
                case 3:
                case 4:
                {
                    int old = w->getWindowStart();
                    next = scan_bf_for_lf( old, 1 );
                    if( old < next && next <= dot )
                    {
                        dumpstate = 5;
                        next = w->getWindowStart() + 50;
                    }
                    else
                        dumpstate++;

                }
                    break;
                case -1:
                    break;
                default:
                {
                    dumpstate++;
                    next = w->getWindowStart() + 50;
                    if( dumpstate > 10 )
                        dumpstate = -1;
                }
                }
                if( next <= dot )
                    w->setWindowStart( next );
                else
                    dumpstate = -1;
            }
        }

        w->w_force = 0;

        if( w->w_next != NULL )
            w->dump_mode( sline + w->w_height - 1, scol );
        if( w->w_right != 0 )
            scol += vertical_bar_width + w->w_width;
        else
        {
            scol = 1;
            sline += w->w_height;
        }

        w = w->w_next;
    }

    cant_ever_opt = hits > 1 && !quick_rd;

    // put back the current window
    cur_win->set_win_internal();

    M_dbg_msg( FormatString( "full_upd => slow=%d" ) << slow );

    return slow;
}

//
// Dump the mode line for window w on line n -- assumes the current buffer
// is the one associated with window w
//
static EmacsString dump_mode_percent( int pos, int length )
{
    if( pos <= 1 )
        return "Top";

    if( pos > length )
        return "Bottom";

    return FormatString("%2d%%") << int((pos - 1) * 100l / length);
}

//
// these two functions where added to work around bugs in
// GNU C++ 2.7.2.1 - expand inline once GNU gets their
// act together
//
static EmacsString dump_mode_hscroll( int horz_offset )
{
    if( horz_offset > 1 )
        return FormatString("<%d<") << horz_offset;

    return EmacsString::null;
}

void EmacsWindow::dump_mode( int line, int col )
{
    EmacsChar_t buf[MSCREENWIDTH+2];
    EmacsChar_t *p = buf;
    EmacsChar_t *buf_end = &buf[ w_width ];

    const EmacsChar_t *s = bf_cur->b_mode.md_modeformat.unicode_data();
    int recurse_depth = recursive_edit_depth - minibuf_depth;

    if( mouse_y == line
    && mouse_x >= col
    && mouse_x <= col + w_width )
    {
        mouse_hit |= MOUSE_ON_MODELINE;
        if( mouse_x >= col + w_width
        && mouse_x < col + w_width + vertical_bar_width
        && w_right != 0 )
            mouse_hit |= MOUSE_ON_VERTICAL;

        setMouseHitPosition( dot, this );
    }

    EmacsChar_t c;

    int tl = bf_cur->unrestrictedSize();
    int d = this == group->current_window ? dot : getWindowDot().to_mark();

    while( (c = *s++) != 0 )
        if( c != '%' )
        {
            if( p <= buf_end )
                *p++ = c;
        }
        else
        {
            EmacsString str;
            int width = 0;
            while( unicode_is_digit( c = *s++ ) )
                width = width * 10 + (c - '0');
            if( c == 0 )
                break;
            switch( c )
            {
            case 'a':
                str = bf_cur->b_mode.md_abbrevon ? ",Abbrev" : "";
                break;
            case 'b':
                str = bf_cur->b_buf_name;
                break;
            case 'c':
                str = checkpoint_frequency > 0
                    && bf_cur->b_checkpointed != -1 ? ",Checkpoint" : "";
                break;
            case 'f':
                str = bf_cur->b_fname.isNull() ? EmacsString("[No File]") : bf_cur->b_fname;
                break;
            case 'j':
                str = bf_cur->b_journalling
                    && journalling_frequency != 0 ? ",Journal" : "";
                break;
            case 'm':
                str = bf_cur->b_mode.md_modestring;
                break;
            case 'M':
                str = global_mode_string.asString();
                break;
            case 'r':
                str = bf_cur->b_mode.md_replace ? ",Overstrike" : "";
                break;
            case 'R':
                str = bf_cur->b_mode.md_readonly ? ",Readonly" : "";
                break;
            case '*':
                str = bf_cur->b_modified != 0 ? "*" : "";
                break;
            case '[':
                if( recurse_depth > 4 )
                    str = FormatString("[%d[") << recurse_depth;
                else
                    str = "[[[[" + (4 - recurse_depth);
                break;
            case ']':
                if( recurse_depth > 4 )
                    str = FormatString("]%d]") << recurse_depth;
                else
                    str = "]]]]" + (4 - recurse_depth);
                break;
            case 'p':
                str = dump_mode_percent( d, tl );
                break;
            case 'h':
                str = dump_mode_hscroll( w_horizontal_scroll );
                break;
            default:
                str.append( c );
            }
            if( str.length() )
            {
                for( int i=0; i<str.length(); i++ )
                {
                    if( p > buf_end )
                        break;
                    *p++ = str[i];
                    width = width - 1;
                    if( width == 0 )
                        break;
                }
            }

            for( int j=0; j<width; j++ )
            {
                if( p > buf_end )
                    break;
                *p++ = ' ';
            }
        }

    *p++ = 0;
    group->view->dump_str( false, buf, w_width, line, col, LINE_ATTR_MODELINE );
}

//
//    dump the indicated string
//    starting at line `line` and column `column` dump `limit` characters
//
void EmacsView::dump_str
    (
    bool is_users_current_window,
    const EmacsChar_t *s,
    int limit,
    int line, int column,
    int highlight
    )
{
    int col = 0;
    bool setcurs = false;
    if( minibuf_body.isPromptBody()
    && is_users_current_window
    && s == minibuf_body.getBody().unicode_data() )
        setcurs = true;

    // QQQ: s == mini... really compare pointers?
    if( activity_indicator && term_ansi && s == minibuf_body.getBody().unicode_data() )
    {
        setpos( line, 1 );
        dsputc( activity_character, 0 );
        dsputc( ' ', 0 );
        col = 2;
    }
    else
        setpos( line, column ); // setup things to update line line

    for( int i=0; i<limit; i++ )
    {
        int c;
        c = *s++;
        if( c == 0 )
            break;
        if( c == '\t' )    // expand tab to spaces
        {
            int new_col = min( (col / bf_cur->b_mode.md_tabsize + 1) *
                    bf_cur->b_mode.md_tabsize, limit );

            while( col < new_col )
            {
                dsputc( ' ', highlight );
                col++;
            }
        }
        else if( c < ' ' )
        {
            if( ctl_arrow )
            {
                if( term_deccrt
                && (c == ctl( 'L')
                || c == ctl( 'M')
                || c == ctl( 'K')
                || c == ctl( '[')) )
                {
                    col++;

                    if( col <= limit )
                        switch( c )
                        {
                        case ctl('L'):
                            dsputc( ctl('c'), highlight ); break;
                        case ctl('M'):
                            dsputc( ctl('d'), highlight ); break;
                        case ctl('K'):
                            dsputc( ctl('i'), highlight ); break;
                        case ctl('['):
                            dsputc( ctl('['), highlight ); break;
                        }
                }
                else
                {
                    col = col + 2;
                    if( col <= limit )
                    {
                        dsputc('^', highlight);
                        // need support of C1 control codes
                        dsputc( (( c < ' ' ) ?  (c & 0x1f) + 0x40 : '?' ), highlight );
                    }
                }
            }
            else
            {
                col = col + 4;
                if( col <= limit )
                {
                    dsputc( '\\', highlight );
                    dsputc( ((c>>6)&7) + '0', highlight);
                    dsputc( ((c>>3)&7) + '0', highlight);
                    dsputc( (c&7) + '0', highlight);
                }
            }
        }
        else
        {
            col++;
            if( col <= limit )
                dsputc( c, highlight );
        }
    }

    if( col > limit )
    {
        setpos( line, limit );
        dsputc( '$', highlight );
    }

    if( setcurs )
    {
        curs_y = line;
        curs_x = col > limit ? limit : col + 1;
    }
}

//
// dump the text from the indicated window on the indicated line;
// the current buffer must be the one tied to this window
//
bool EmacsWindow::dump_win( bool is_users_current_window, int line, int col, bool can_move )
{
    bool found_dot( false );

    // left needs to reflect the fact that the minibuf does not have a modeline
    int left = w_next != 0 ? w_height - 1 : w_height;

    int n = getWindowStart();

    if( can_move
    &&    ((n > bf_cur->first_character() && bf_cur->char_at( n - 1 ) != '\n')
        || n < bf_cur->first_character()) )
    {
        n = n < bf_cur->first_character() ? bf_cur->first_character() : scan_bf_for_lf( n, -1 );
        setWindowStart( n );
    }

    // if this window is the minibuffer
    if( w_next == 0 )
    {
        group->MB_line = line;
        group->view->clearline( line );

        if( !minibuf_body.haveBody() )
            return 0;

        if( n == 1 )
            group->view->dump_str( is_users_current_window, minibuf_body.getBody().unicode_data(), group->view->t_width, line, 1, 0 );

        bool dot_found_now = false;
        int dot_column = 0;
        bool line_wrapped = false;

        int next = dump_line_from_buffer
            (
            is_users_current_window,
            n,
            line,
            group->view->t_width - group->view->columns_left,
            group->view->t_width - group->view->columns_left,
            &dot_found_now,
            &dot_column,
            line_wrapped,
            bf_cur->b_mode
            );
        line++;
        left--;

        if( dot_found_now )
        {
            // dot found
            if( is_users_current_window )
            {
                group->one_line_start.set_mark( bf_cur, line_wrapped ? 1 : n, 0 );
                group->one_line_valid = !line_wrapped;
                group->one_line_line = line - 1;
            }

            found_dot = true;
        }

        n = next;
    }

    int last_col;
    if( w_right != 0 )
        last_col = col + w_width;
    else
        last_col = 0;

    for( int i=1; i<=left; i++ )
    {
        if( w_left == NULL )
            group->view->clearline( line );
        else
            group->view->setpos( line, col );

        bool dot_found_now = false;
        int dot_column = 0;
        bool line_wrapped = false;
        int next = dump_line_from_buffer( is_users_current_window, n, line, col, 1, &dot_found_now, &dot_column, line_wrapped, bf_cur->b_mode );
        if( last_col > 0 )
        {
            group->view->setpos( line, last_col );
            for( int bar_size=0; bar_size < vertical_bar_width; bar_size++ )
                group->view->dsputc( ' ', LINE_ATTR_MODELINE );
        }
        line++;
        if( dot_found_now )
        {
            if( this == group->current_window )
            {
                group->one_line_start.set_mark( bf_cur, line_wrapped ? 1 : n, 0 );
                group->one_line_valid = !line_wrapped;
                group->one_line_line = line - 1;
            }

            if( automatic_horizontal_scroll )
            {
                //
                // Based on what the user last did move dot or the horizontal scroll
                //
                if( w_user_horizontal_scrolled )
                {
                    // Move dot to be visible where the scroll bar is

                    int new_dot_column = w_horizontal_scroll + w_width/2;

                    if( dot_column < w_horizontal_scroll )
                    {
                        int col = 0;
                        do
                        {
                            if( bf_cur->char_at( dot ) == '\n'
                            || (dot-1) >= bf_cur->num_characters() )
                                break;

                            dot_right( 1 );
                            col = bf_cur->calculateColumn( dot );
                        }
                        while( col < new_dot_column );
                    }

                    if( dot_column > (w_horizontal_scroll + w_width - 2) )
                    {
                        int col = 0;
                        do
                        {
                            if( (dot-1) <= bf_cur->first_character()
                            || bf_cur->char_at( dot-1 ) == '\n' )
                                break;

                            dot_left( 1 );
                            col = bf_cur->calculateColumn( dot );
                        }
                        while( col > new_dot_column );
                    }
                }

                else
                {
                    // Move the scroll bar to show with dot

                    // need to make w_horizontal_scroll smaller?
                    if( dot_column < w_horizontal_scroll )
                    {
                        w_horizontal_scroll = dot_column - (scroll_step_horizontal-1);
                        if( w_horizontal_scroll < 1 )
                            w_horizontal_scroll = 1;

                        if( (scroll_step_horizontal%bf_cur->b_mode.md_tabsize) == 0 )
                            // round to a tab boundary
                            w_horizontal_scroll =
                                (w_horizontal_scroll-1)
                                    / bf_cur->b_mode.md_tabsize
                                        * bf_cur->b_mode.md_tabsize
                                            + 1;
                    }

                    // need to make w_horizontal_scroll bigger?
                    if( dot_column > (w_horizontal_scroll + w_width - 2) )
                    {
                        w_horizontal_scroll = dot_column + (scroll_step_horizontal-1) - (w_width-2);
                        if( w_horizontal_scroll < 1 )
                            w_horizontal_scroll = 1;

                        if( (scroll_step_horizontal%bf_cur->b_mode.md_tabsize) == 0 )
                            // round to a tab boundary
                            w_horizontal_scroll =
                                (w_horizontal_scroll-1)
                                    / bf_cur->b_mode.md_tabsize
                                        * bf_cur->b_mode.md_tabsize
                                            + 1;
                    }
                }

                w_user_horizontal_scrolled = false;
            }

            found_dot = true;
        }

        n = next;
    }

    return found_dot;
}

// Scan the current buffer for the k'th occurrence of character LF,
// starting at position n; k may be negative. Returns the position
// of the character following the one found
int scan_bf_for_lf( int n, int k )
{
    if( k > 0 )
        while( k != 0 )
        {
            n--;
            do
            {
                n++;
                if( n > bf_cur->num_characters() )
                    return n;
            }
            while( bf_cur->char_at( n ) != '\n' );

            k--;
            if( k != 0 )
                n++;
        }
    else
        while( k != 0 )
        {
            do
            {
                n--;
                if( n < bf_cur->first_character() )
                    return bf_cur->first_character();
            }
            while( bf_cur->char_at( n ) != '\n' );

            k++;
        }

    return n + 1;
}

//
// do a screen update, taking possible shortcuts into account
//
// Note: bf_cur may, or may not, be the same as current_window->w_buf
// depending on use of functions like temp-use-buffer
//
void EmacsWindowGroup::do_dsp()
{
    if( !term_is_terminal )
        return;

    // preserve bf_cur across update
    EmacsBuffer *old_buf = bf_cur;
    // need to save dot???

    current_window->w_buf->set_bf();

    int slow_update = 0;
    int cant_ever_opt = 0;


    int saved_input_mode = input_mode;
    input_mode = 0;

    if( screen_garbaged || ml_err || last_redisplay_paused )
    {
        cant_1win_opt = 1;
        last_redisplay_paused = 0;
    }

    if( cant_1win_opt )
        cant_1line_opt = redo_modes = 1;

    if( redo_modes )
        cant_1line_opt = 1;

    if( !cant_1line_opt
    && one_line_valid && ! one_line_start.m_modified
    && one_line_start.m_buf == current_window->w_buf )
    {
        int col = 1;
        int n = one_line_start.to_mark();

        if( current_window->w_left || current_window->w_right )
        {
            EmacsWindow *w = current_window->w_left;
            while( w )
            {
                col += w->w_width + vertical_bar_width;
                w = w->w_left;
            }

            //
            //    Get old line contents and clear out the
            //    bit that will get overwritten
            //
            view->copyline( one_line_line );
            view->setpos( one_line_line, col );
            memset( view->text_cursor, ' ', current_window->w_width );
            memset( view->attr_cursor, 0, current_window->w_width*sizeof(DisplayAttr_t) );
        }
        else
        {
            view->clearline( one_line_line );
        }

        if( minibuf_body.haveBody() && current_window->w_next == 0 )
        {
            if( n == 1 )
                view->dump_str( true, minibuf_body.getBody().unicode_data(), view->t_width, one_line_line, 1, 0 );
        }

        bool line_wrapped = false;
        bool cur_line_dumped = current_window->dump_line_from_buffer
            ( true, n, one_line_line, col, 1, NULL, NULL, line_wrapped, bf_cur->b_mode ) < 0;

        if( cur_line_dumped && !line_wrapped )
            goto update;    // we made it!

        if( !current_window->w_buf->b_mode.md_wrap_lines )
            slow_update = -1;
    }

    slow_update = 1;
    one_line_valid = 0;
    if( view->full_upd( cant_ever_opt ) != 0 )
        slow_update = 1;

update:
    view->update_screen( slow_update );
    if( ml_err )
    {
        ding();
        ml_err = 0;
    }

    cant_1line_opt = 0;
    cant_1win_opt = cant_ever_opt;
    redo_modes = 0;

    view->t_io_flush();

    input_mode = saved_input_mode;

    old_buf->set_bf();
}

//
// Dump one line from the current buffer starting at character n onto
// line sline; setting curs_x and curs_y if appropriate
//
static EmacsChar_t fake_c1_chars[32] =
{
    '*', '#', 't', 'f', 'c', 'l', 'o', '+',
    'n', 'v', '+', '+', '+', '+', '+', '~',
    '~', '-', '_', '_', '|', '|', '-', '-',
    '|', '<', '>', 'n', '=', 'L', '.', ' '
};

//
//    Returns index into buffer of the next char in the
//    buffer to be dumped
//    result is -ive if dot was found in the dumped line.
//    result is +ive otherwise
//
int EmacsWindow::dump_line_from_buffer
    (
    bool is_current_window,
    int line_start_n,
    int sline,
    int scolumn,
    int initial_column,
    bool *_found_dot,
    int *_dot_column,
    bool &_line_wrapped,
    ModeSpecific &_mode
    )
{
    return group->view->dump_line_from_buffer
        (
        this,
        is_current_window,
        line_start_n,
        sline,
        scolumn,
        initial_column,
        _found_dot,
        _dot_column,
        _line_wrapped,
        _mode
        );
}

int EmacsView::dump_line_from_buffer
    (
    EmacsWindow *window,
    bool is_current_window,
    int line_start_n,
    int sline,
    int scolumn,
    int initial_column,
    bool *_found_dot,
    int *_dot_column,
    bool &line_wrapped,
    ModeSpecific &mode
    )
{
    EmacsBuffer *buf = window->w_buf;
    int rr_start = 0; int rr_end = 0;    // limits of rendition region
    struct flags_bits
    {
        int display_non_printing : 1;
        bool found_dot : 1;
        int looking_for_mouse : 1;
        int dec_crt : 1;
        int syntax_colouring : 1;
    }
        flags;

    flags.dec_crt = term_deccrt;
    flags.display_non_printing = mode.md_displaynonprinting && flags.dec_crt;
    flags.syntax_colouring = mode.md_syntax_colouring;

    int first_column = window->w_horizontal_scroll;
    int term_width = window->w_width;
    int n = line_start_n;
    int col = initial_column;
    int lim = buf->unrestrictedSize() - mode.md_tailclip;
    flags.found_dot = 0;
    flags.looking_for_mouse =   mouse_y == sline    // on this line
                            &&  mouse_win_ == NULL  // don't look if found in winoow to the left
                            &&  mouse_x >= scolumn; // x is in this buffers columns
    if( flags.looking_for_mouse
    && mouse_x >= window->w_width + scolumn )
    {
        if( mouse_x >= window->w_width + scolumn
        && mouse_x < window->w_width + scolumn + vertical_bar_width
        && window->w_right != 0 )
        {
            mouse_hit = MOUSE_ON_VERTICAL;

            setMouseHitPosition( dot, window );
        }

        flags.looking_for_mouse = 0;
    }

    //
    //    update the syntax data if required
    //
    if( flags.syntax_colouring )
    {
        // required is atleast...
        // the start position
        // + the first_column offset
        // + the width of the screen
        // + 1 ??why + 1??
        // + 1024 to allow for truncated lines
        int required = n + first_column + t_width + 1 + 1024;
        if( required > buf->unrestrictedSize() )
            required = buf->unrestrictedSize();

        mode.md_syntax_array = 1;    // force syntax array
        // if there is a problem the md_syntax_array will be turned off
        if( !bf_cur->syntax_fill_in_array( required ) )
            flags.syntax_colouring = 0;
    }

    //
    //    find the value of the mark for this buffer
    //
    int mark = dot;

    //
    //    If allowed to highlight region and mark is set then set mark to
    //    the value of the mark. Note that if r_start and r_end are the
    //    same the region will not be highlight.
    //
    if( mode.md_highlight )
    {
        if( window->isCurrentWindow() )
        {
            // use the buffers mark
            M_dbg_msg( FormatString("dump_line_from_buffer( %s ) isCurrentWindow() sline=%d buf=%s b_mark=%s dot=%d w_mark=%s")
                        << window->getDescription() << sline
                        << buf->b_buf_name << buf->b_mark.asString()
                        << dot << window->getWindowMark().asString() );

            if( buf->b_mark.isSet() )
            {
                mark = buf->b_mark.to_mark();
            }
        }
        else
        {
            // use the saved mark
            M_dbg_msg( FormatString("dump_line_from_buffer( %s ) not isCurrentWindow() sline=%d buf=%s b_mark=%s dot=%d w_mark=%s")
                        << window->getDescription() << sline
                        << buf->b_buf_name << buf->b_mark.asString()
                        << dot << window->getWindowMark().asString() );

            if( window->getWindowMark().isSet() )
            {
                mark = window->getWindowMark().to_mark();
            }
        }
    }

    //
    //    Setup the region highlight stuff
    //
    int r_start     = min( dot, mark );
    int r_end       = max( dot, mark );
    int highlight   = r_start <= n && r_end > n ? LINE_M_ATTR_HIGHLIGHT : 0;

    // turn markers into ints
    RenditionRegion *rr = buf->b_rendition_regions;
    while( rr != NULL )
    {
        rr_start = rr->rr_start_mark.to_mark();
        rr_end = rr->rr_end_mark.to_mark();

        // if we start before this rr then use this rr as the current
        if( n < rr_start )
            break;
        // if we are in the middle of this rr set the colour
        if( n < rr_end )
        {
            highlight = rr->rr_colour;
            flags.syntax_colouring = 0;
            break;
        }

        rr = rr->rr_next;
    }
    if( rr == NULL )
    {
        rr_start = -1;
        rr_end = -1;
    }

    int dot_column = 0;

    bool wrap_lines = mode.md_wrap_lines != 0;
    EmacsChar_t c = 0;
    enum char_types_t { init_char, space_char, word_char, other_char } last_char_type = init_char;
    int word_start_col = 0;
    int word_start_pos = 0;
    bool do_found_dot_for_current_window = is_current_window;

    for(;;)
    {
#define    _if_wraped( column ) \
        if( column > (term_width + first_column) ) \
        { \
            if( wrap_lines ) \
            { \
                n -= 2; \
                goto dump_line_from_buffer_loop; \
            } \
        } \
        else \
            if( column > first_column )

        if( n == dot )
        {
            if( do_found_dot_for_current_window )
            {
                curs_x = t_width - columns_left + 1;
                curs_y = sline;
                dot_col = col;
                col_valid=4;

                if( curs_x > t_width )
                    curs_x = t_width;
                else if( curs_x < 1 )
                    curs_x = 1;
            }

            flags.found_dot = 1;
            dot_column = col;
        }

        //
        //    If we are at the start of the region turn on
        //    highlighting
        //
        if( n == r_start )
            highlight |= LINE_M_ATTR_HIGHLIGHT;

        //
        //    if we are at the end of the region turn off
        //    highlighting
        //
        if( n == r_end )
            highlight &= ~LINE_M_ATTR_HIGHLIGHT;

        // see if at the end of the rendition region
        if( n == rr_end )
        {
            // turn off the rr colour
            highlight &= LINE_M_ATTR_USER;
            // turn syntax colouring back on
            flags.syntax_colouring = mode.md_syntax_colouring;
            // move to the next rr
            rr = rr->rr_next;

            // setup the start and end
            if( rr == NULL )
            {
                rr_start = -1;
                rr_end = -1;
            }
            else
            {
                rr_start = rr->rr_start_mark.to_mark();
                rr_end = rr->rr_end_mark.to_mark();
            }
        }

        //
        // check for the start  of an rr - must be
        // done after the rr_end check
        //
        if( n == rr_start )
        {
            highlight = rr->rr_colour | (highlight&LINE_M_ATTR_HIGHLIGHT);
            // turn off syntax coloring
            flags.syntax_colouring = 0;
        }
        if( n > lim )
        {
            if( mode.md_display_eof
            && n - 1 == lim
            && flags.dec_crt )
                dsputc( ctl('@'), highlight );

            n++;
            c = ctl('J');
            goto dump_line_from_buffer_loop;
        }

        if( flags.looking_for_mouse
        && mouse_x < (scolumn + col - first_column) )
        {
            setMouseHitPosition( n-1, window );
            flags.looking_for_mouse = 0;
        }

        c = buf->char_at( n );
        if( flags.syntax_colouring  )
            highlight = (highlight&LINE_M_ATTR_HIGHLIGHT) | (buf->syntax_at( n ) & (SYNTAX_FIRST_FREE-1));

        n++;

        switch( c )
        {
        case ctl('J'):    // Newline
            if( flags.display_non_printing )
                if( col >= first_column - 1 )
                    dsputc( ctl('H'), highlight );

            goto dump_line_from_buffer_loop;

        case ctl('I'):    // TAB
        {
            int old_col = col;
            col = ((col - 1) / mode.md_tabsize + 1) * mode.md_tabsize + 1;

            if( flags.display_non_printing )
            {
                _if_wraped( old_col + 1 )
                    dsputc( ctl('B'), highlight );

                for( int i=old_col + 2; i<=col; i += 1 )
                    _if_wraped( i )
                        dsputc( ctl('^'), highlight );
            }
            else
            {
                for( int i=old_col + 1; i<=col; i += 1 )
                    _if_wraped( i )
                        dsputc( ' ', highlight );
            }

            // is this a change of char type?
            if( last_char_type != space_char            // the type changed
            && (col - first_column) < window->w_width ) // within the visible part of the window
            {
                // yes remember the place the change occured
                word_start_pos = n-1;
                word_start_col = col;
            }

            last_char_type = space_char;

            break;
        }

        default:
        {
            // figure out the type of the char
            char_types_t char_type;
            if( c == ' ' )
                char_type = space_char;
            else if( buf->char_is( c, SYNTAX_WORD ) )
                char_type = word_char;
            else
                char_type = other_char;

            // is this a change of char type?
            if( last_char_type != char_type                // the type changed
            && (col - first_column) < window->w_width )        // within the visible part of the window
            {
                // yes remember the place the change occured
                word_start_pos = n-1;
                word_start_col = col;
            }

            last_char_type = char_type;
        }

            if( (c >= ' ' && c <= '~')    // printing chars
            || (c >= 128+32 && c <= 254)
            || (c >= 256)
            )
            {
                col++;

                _if_wraped( col )
                    dsputc( c, highlight );
            }
            else
            // none printing control characters
            {
                if( mode.md_display_c1
                && c >= 128 && c <= 128+32 )
                {
                    col++;
                    c &= 0x1f;

                    _if_wraped( col )
                        dsputc( flags.dec_crt ? c : fake_c1_chars[c], highlight );
                }
                else
                if( ctl_arrow != 0 && (c & 0x80) == 0 )
                {
                    if( flags.dec_crt
                    &&
                        (c == ctl('L')
                        || c == ctl('M')
                        || c == ctl('K')
                        || c == ctl('[')
                        || c == ctl('I')) )
                    {
                        col++;

                        _if_wraped( col )
                            switch( c )
                            {
                            case ctl('L'):    dsputc( ctl('c'), highlight ); break;
                            case ctl('M'):    dsputc( ctl('d'), highlight ); break;
                            case ctl('K'):    dsputc( ctl('i'), highlight ); break;
                            case ctl('I'):    dsputc( ctl('b'), highlight ); break;
                            case ctl('['):    dsputc( ctl('{'), highlight ); break;
                            }
                    }
                    else
                    {
                        col = col + 2;
                        _if_wraped( col )
                        {
                            dsputc('^', highlight);
                            dsputc((( c < ' ' ) ? ( c & 0x1f) + 0x40 : '?'), highlight );
                        }
                    }
                }
                else
                {
                    col = col + 4;
                    _if_wraped( col )
                    {
                        dsputc( '\\', highlight);
                        dsputc( '0' + ((c>>6)&7), highlight);
                        dsputc( '0' + ((c>>3)&7), highlight);
                        dsputc( '0' + (c&7), highlight);
                    }
                }
            }
        }

#undef    _if_wraped
    }
dump_line_from_buffer_loop:

    if( flags.looking_for_mouse )
    {
        setMouseHitPosition( n-1, window );
    }

{
    //
    //    Only put a '$' or '\' at the end of the line if :-
    //
    //    The line is longer then "w_width"
    //    or the cursor is on column "w_width" + 1
    //
    if(    col - first_column > window->w_width        // a long line
    ||    dot_column - first_column >= window->w_width    // cursor off screen
    ||    c != ctl('J')                    // wrap line
    )
    {
        int col_for_end_marker = 1;
        if( wrap_lines                    // wrap lines
        && word_start_col > first_column )        // and something added to this line
        {
            n = word_start_pos;
            col_for_end_marker = col - word_start_col - 1;

            if( dot_column - first_column == window->w_width )    // cursor at edge screen
                col_for_end_marker++;                // the world is one column narrower in this case

            for( int i = col_for_end_marker; i>0; i-- )
            {
                text_cursor[-i] = ' ';
                attr_cursor[-1] = 0;
            }

            col = word_start_col;
        }

        highlight = r_start <= n && r_end > n ? LINE_M_ATTR_HIGHLIGHT : 0;
        if( flags.dec_crt )
            text_cursor[-col_for_end_marker] = (DisplayBody_t)(wrap_lines ? ctl('Z') : ctl('@'));
        else
            text_cursor[-col_for_end_marker] = (DisplayBody_t)(wrap_lines ? '\\' : '$');
        attr_cursor[-col_for_end_marker] = (DisplayAttr_t)highlight;

        if( wrap_lines )
            line_wrapped = true;
    }
}

    if( _found_dot != NULL )
        *_found_dot = flags.found_dot;
    if( _dot_column != NULL )
        *_dot_column = dot_column;

    return n;
}


//
//
//    EmacsWindowGroup
//
//
QueueHeader<EmacsWindowGroup> EmacsWindowGroup::header;

EmacsWindowGroup::~EmacsWindowGroup()
{
    deleteWindows();

    // get us out of the queue
    queueRemove();
}

EmacsWindowGroup::EmacsWindowGroup( EmacsView *_view )
    : windows( NULL )
    , view( _view )
    , one_line_start()
    , one_line_valid(false)
    , one_line_line( 0 )
    , MB_line( 0 )
    , current_window( NULL )
    , current_buffer()
    , buffer_is_visible( 0 )
{
    header.queueInsertAtTail( this );

    windows = EMACS_NEW EmacsWindow( *this );

    windows->w_height = 6;    // use a height that will split
    windows->w_width = view->t_width;
    windows->w_buf = bf_cur;
    windows->setWindowDot( Marker( bf_cur, 1, 0 ) );
    windows->unsetWindowMark();
    windows->setWindowStart( 1 );

    current_window = windows;
    windows->split_win_horz()->set_win();

    current_window->tie_win( minibuf );
    change_window_height( 1 - current_window->w_height);

    windows->set_win();

    fit_windows_to_screen( view );
}

EmacsWindowGroup::EmacsWindowGroup( EmacsWindowGroup &from )
    : view( from.view )
    , one_line_start()
    , one_line_valid(false)
    , one_line_line( 0 )
    , MB_line( 0 )
{
    header.queueInsertAtTail( this );

    copyWindows( from );
}

// an unusable window group needed for the save environment logic
EmacsWindowGroup::EmacsWindowGroup( SavedEmacsView & )
    : windows( NULL )
    , view( NULL )
    , one_line_start()
    , one_line_valid(false)
    , one_line_line( 0 )
    , MB_line( 0 )
    , current_window( NULL )
    , current_buffer()
    , buffer_is_visible( 0 )
{
    header.queueInsertAtTail( this );
}

EmacsWindowGroup &EmacsWindowGroup::operator=( EmacsWindowGroup &from )
{
    deleteWindows();
    copyWindows( from );
    return *this;
}

void EmacsWindowGroup::deleteWindows()
{
    while( windows )
    {
        EmacsWindow *next = windows->w_next;
        delete windows;
        windows = next;
    }
    one_line_start.unset_mark();
    current_buffer.unset_mark();
}

void EmacsWindowGroup::copyWindows( EmacsWindowGroup &from )
{
    // this is only required if save_environment is happening
    if( from.windows == NULL )
        return;

    windows = EMACS_NEW EmacsWindow( *from.windows, *this );
    current_buffer = from.current_buffer;
    current_window = windows;
    buffer_is_visible = from.buffer_is_visible;
    one_line_start = from.one_line_start;
    one_line_valid = from.one_line_valid;
    one_line_line = from.one_line_line;
    MB_line = from.MB_line;

    EmacsWindow *wcopy = windows;
    EmacsWindow *wfrom = from.windows->w_next;

    while( wfrom != NULL )
    {
        EmacsWindow *wnext = EMACS_NEW EmacsWindow( *wfrom, *this );
        wcopy->w_next = wnext;
        wnext->w_prev = wcopy;

        if( from.current_window == wfrom )
            current_window = wnext;

        wcopy = wnext;
        wfrom = wfrom->w_next;
    }

    //
    //    fix up the left and right pointers
    //
    wfrom = from.windows;
    wcopy = windows;
    while( wfrom != NULL )
    {
        if( wfrom->w_right != NULL )
            wcopy->w_right = wcopy->w_next;
        if( wfrom->w_left != NULL )
            wcopy->w_left = wcopy->w_prev;

        wfrom = wfrom->w_next;
        wcopy = wcopy->w_next;
    }
}

QueueHeader<EmacsView> EmacsView::header;

#if defined( _MSC_VER )
#pragma warning( disable: 4355 )
#endif
EmacsView::EmacsView()
: TerminalControl()
, QueueEntry<EmacsView>()
, curs_x( 1 )
, curs_y( 1 )
, current_line( -1 )
, columns_left( -1 )
, text_cursor( NULL )
, attr_cursor( NULL )
, window_size( 0 )
, windows( this )
{
    // allow us to be found
    header.queueInsertAtTail( this );
}

EmacsView::~EmacsView()
{
    // get us out of the queue
    queueRemove();
}

void EmacsView::dsputc( int c, int a )
{
    columns_left--;
    if( columns_left >= 0 )
    {
        *text_cursor++ = (DisplayBody_t)(c);
        *attr_cursor++ = (DisplayAttr_t)(a);
    }
}


    EmacsWindow *findWindowsById( int windows_id );


EmacsWindow *EmacsView::findWindowsById( int windows_id )
{
    EmacsWindow *w = windows.windows;
    while( w != NULL )
    {
        if( w->w_window_id == windows_id )
            return w;
        w = w->w_next;
    }

    return NULL;
}

void TerminalControl::k_input_scroll_set_horz( int window_id, int position )
{
    EmacsWindow *win = theActiveView->findWindowsById( window_id );
    if( win == NULL )
        return;

    win->w_horizontal_scroll = max( position, 0 );

    cant_1line_opt = 1;
    redo_modes = 1;

    theActiveView->windows.do_dsp();
}

void TerminalControl::k_input_scroll_change_horz( int window_id, int change )
{
    EmacsWindow *win = theActiveView->findWindowsById( window_id );
    if( win == NULL )
        return;

    win->w_horizontal_scroll = max( win->w_horizontal_scroll + change, 0 );

    cant_1line_opt = 1;
    redo_modes = 1;

    theActiveView->windows.do_dsp();
}

void TerminalControl::k_input_scroll_set_vert( int window_id, int position )
{
    EmacsWindow *win = theActiveView->findWindowsById( window_id );
    if( win == NULL )
        return;

    EmacsWindow *old_window = theActiveView->windows.currentWindow();
    win->set_win();

    int n = position+1;
    if( n < 1 )
        n = 1;
    if( n > bf_cur->num_characters() )
        n = bf_cur->num_characters() + 1;
    if( n != (bf_cur->num_characters() + 1) )
        set_dot( scan_bf_for_lf( n, -1 ) );

    old_window->set_win();

    int old_scroll_step = scroll_step;
    scroll_step = 1;
        theActiveView->windows.do_dsp();
    scroll_step = old_scroll_step;
}

void TerminalControl::k_input_scroll_change_vert( int window_id, int change )
{
    EmacsWindow *win = theActiveView->findWindowsById( window_id );
    if( win == NULL )
        return;

    EmacsWindow *old_window = theActiveView->windows.currentWindow();
    win->set_win();

    int scroll = 0;

    switch( change )
    {
    case +1:
        scroll += 1;
        break;

    case -1:
        scroll -= 2;
        break;

    case +2:
        scroll += win->w_height * 4 / 5;
        break;

    case -2:
        scroll -= win->w_height * 4 / 5;
        break;

    default:
        break;
    }

    if( scroll )
    {
        set_dot( scan_bf_for_lf( dot, scroll ) );
        win->setWindowStart( scan_bf_for_lf( win->getWindowStart(), scroll ) );
    }

    old_window->set_win();

    int old_scroll_step = scroll_step;
    scroll_step = 1;
        theActiveView->windows.do_dsp();
    scroll_step = old_scroll_step;
}
