// Motif X-Window parent pseudo-terminal driver for DEC Emacs
//
//    Nick Emery Jun-93
//    adapted from spm's X driver 7/85    MIT Project Athena
//
//    Graham R. Cobb 17-Sep-1993
//    Add XComposeStatus argument to XLookupString to allow compose
//    sequences to work.
//
#include <emacs.h>
#define exception math_exception
#include <math.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

int is_motif = 0;
#ifdef XWINDOWS
# include <emacs_motif.h>
# include <motif_keys.h>
# include <xwin_icn.h>

extern int vertical_bar_width;

extern void ui_frame_restore( void );

EmacsMotifKeySymMap keysym_map;

//
//    Motif View class
//
void UI_update_window_title(void);

//
// Own Storage:
//


// Processing data
static int VisibleX, VisibleY;
static int SavedX, SavedY;
static int CursorExists;
static int InUpdate;
static int flexlines;
static EmacsLinePtr empty_line_ptr;

int keyboard_type( KEYBOARD_TYPE );

// X-Windows data
const char *icon_bits( (char *)write_scrolls_bits );
const int icon_width( write_scrolls_width );
const int icon_height( write_scrolls_height );
const int SB_SIZE( 100000 );

int motif_iconic = 0;
static int initializing = 1;

TerminalControl_GUI *theMotifGUI;

TerminalControl_GUI::TerminalControl_GUI( const EmacsString &/*device*/ )
    : EmacsView()
    , action( NULL )
    , last_mouse_x( 0 )
    , last_mouse_y( 0 )
    , mouse_button_state( 0 )
{
    for( int i=0; i < max_scroll_bars; i++ )
    {
        vert_scroll[i] = NULL;
        horz_scroll[i] = NULL;
    }
}

TerminalControl_GUI::~TerminalControl_GUI()
{ }

//
// get keyboard focus and pop to top of Z order
//
void TerminalControl_GUI::getKeyboardFocus()
{
    if( initializing )
        return;

    //XRaiseWindow( theMotifGUI->application.dpy, XtWindow( theMotifGUI->application.shell->widget ) );

    EmacsXtArgs args;

    args.setArg( XmNiconic, (void *)&motif_iconic );
    args.getValues( theMotifGUI->application.shell->widget );
    if( motif_iconic )
    {
        EmacsXtArgs args;

        args.setArg( XmNiconic, XtArgVal(0) );
        args.setValues( theMotifGUI->application.shell->widget );
        motif_iconic = 0;
    }

{
    XSetInputFocus(
        application.dpy,    // display
        XtWindow( drawing_area->widget ),    // window
        RevertToNone,        // revert_to
        CurrentTime        // time
        );
}
}


//void TerminalControl_GUI::t_io_print( const unsigned char * )
//{
//    // do not expect to be called
//    emacs_assert(FALSE);
//}

// Display, or remove the cursor
int TerminalControl_GUI::ToggleCursor()
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( "ToggleCursor();" );
# endif

    if( VisibleX < 1 || VisibleX > t_width
    || VisibleY < 1 || VisibleY > t_length )
    {
        CursorExists = 0;
        return 0;
    }

    struct EmacsMotif_Attributes *attr;
    if( !t_phys_screen[VisibleY].isNull() && VisibleX-1 < t_phys_screen[VisibleY]->line_length )
    {
        int last_attr = t_phys_screen[VisibleY]->line_attr[VisibleX-1];

        if( CursorExists  )
        {
            if( last_attr & LINE_M_ATTR_HIGHLIGHT )
                attr = &drawing_area->attr_set.getAttr( LINE_M_ATTR_HIGHLIGHT );
            else if( last_attr & LINE_ATTR_MODELINE )
                attr = &drawing_area->attr_set.getAttr( LINE_ATTR_MODELINE );
            else if( last_attr&LINE_ATTR_USER )
                attr = &drawing_area->attr_set.getAttr( last_attr&LINE_M_ATTR_USER );
            else
                attr = &drawing_area->attr_set.getAttr( last_attr );
        }
        else
            attr = &drawing_area->cu_attr;

        if( attr->getNormalGc() == NULL )
        {
            attr = &drawing_area->attr_set.getAttr( SYNTAX_DULL );

            motif_message_box
            (
            FormatString("Bad last_attr value (0x%x) in ToggleCursor") << last_attr
            );
        }

        char ch( t_phys_screen[VisibleY]->line_body[VisibleX-1] );
        if( ch < ' ' )
        {
            ch += 96;

            XDrawImageString
                (
                application.dpy, XtWindow( drawing_area->widget ), attr->getSpecialGc(),
                application.innerBorder + (VisibleX - 1) * application.ftw,
                VisibleY * application.fth + application.innerBorder - application.ftd,
                &ch, 1
                );
        }
        else
            XDrawImageString
                (
                application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
                application.innerBorder + (VisibleX - 1) * application.ftw,
                VisibleY * application.fth + application.innerBorder - application.ftd,
                &ch, 1
                );

        if( attr->getUnderline()  )
        {
            XDrawLine
                (
                application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
                application.innerBorder + ((VisibleX - 1) * application.ftw),
                application.innerBorder + (VisibleY * application.fth) - application.ftd + 1,
                application.innerBorder + (VisibleX * application.ftw),
                application.innerBorder + (VisibleY * application.fth) - application.ftd + 1
                );
        }
    }
    else
    {
        attr = CursorExists ? &drawing_area->rev_attr : &drawing_area->curev_attr;
        XFillRectangle
            (
            application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
            (VisibleX-1) * application.ftw + application.innerBorder,
            (VisibleY-1) * application.fth + application.innerBorder,
            application.ftw,
            application.fth
            );
    }

# if DBG_DISPLAY && DBG_TMP
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_TMP  )
    {
        _dbg_msg( "XFlush in ToggleCursor" );
        XFlush( application.dpy );
    }
# endif

    CursorExists = !CursorExists;
    return 1;
}

// Move to position row, col, i.e. put the cursor there.
void TerminalControl_GUI::t_topos( int row, int col )
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "t_topos(%d, %d );" ) << row << col );
# endif

    curs_x = col;
    curs_y = row;
    if( InUpdate )
    {
        if( CursorExists )
            ToggleCursor();
        return;
    }

    if( (row == VisibleY ) &&( col == VisibleX ) )
    {
        if( !CursorExists )
            ToggleCursor();
    }
    else if( CursorExists )
    {
        ToggleCursor();
        VisibleX = col;
        VisibleY = row;
        ToggleCursor();
    }
    else
    {
        VisibleX = col;
        VisibleY = row;
        ToggleCursor();
    }
}
// Write a region to the screen
void TerminalControl_GUI::write_region( int width, int height, int x, int y )
{
    register int start_line;
    register int end_line;

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "write_region(%d, %d, %d, %d );" ) << width << height << x << y );
# endif
    //
    //    Convert pixels to char size
    //
    if( y < application.innerBorder )
        start_line = 1;
    else
        start_line =( y - application.innerBorder ) / application.fth + 1;

    if( y - application.innerBorder + height + application.fth - 1 < application.fth )
        end_line = 1;
    else
        end_line =( y - application.innerBorder +( height + application.fth - 1 ) ) / application.fth + 1;

    if( end_line > t_length )
        end_line = t_length;

    for( int row=start_line; row<=end_line; row++ )
        t_update_line( empty_line_ptr, t_phys_screen[row], row );
}
// Wipe a line from the screen, setting it to a background
void TerminalControl_GUI::t_wipe_line( int hlmod  )
{
    register int nCols;

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "wipe_line(%d );" ) << hlmod );
# endif

    if((curs_x < 1 ) ||( curs_x > t_width ) ||
      ( curs_y < 1 ) ||( curs_y > t_length ) )
        return;

    nCols = t_width - curs_x + 1;
    XClearArea
        (
        application.dpy,
        XtWindow( drawing_area->widget ),
        (curs_x - 1) * application.ftw + application.innerBorder,
        (curs_y - 1) * application.fth + application.innerBorder,
        application.ftw * nCols,
        application.fth,
        0
        );

    if( curs_y == VisibleY && VisibleX >= curs_x )
        CursorExists = 0;
    t_topos( curs_y, curs_x );
}
// Reset the entire screen to the backbround colour
void TerminalControl_GUI::t_wipe_screen()
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( "wipescreen();" );
# endif

    XClearWindow( application.dpy, XtWindow( drawing_area->widget ) );
    CursorExists = 0;
    if(!InUpdate )
        ToggleCursor();
}
// Reset the entire screen to the backbround colour
void TerminalControl_GUI::t_reset()
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "reset(0x%0x8);" ) << int(this) );
# endif

    t_wipe_screen();
}
// Flash the screen
void TerminalControl_GUI::t_flash()
{

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( "flash();" );
# endif

#if 0
    struct EmacsMotif_Attributes tmp_array[256];

    tmp_array[SYNTAX_DULL] = drawing_area->attr_array[SYNTAX_DULL];
    tmp_array[SYNTAX_WORD] = drawing_area->attr_array[SYNTAX_WORD];
    tmp_array[SYNTAX_TYPE_STRING1] = drawing_area->attr_array[SYNTAX_TYPE_STRING1];
    tmp_array[SYNTAX_TYPE_STRING2] = drawing_area->attr_array[SYNTAX_TYPE_STRING2];
    tmp_array[SYNTAX_TYPE_STRING3] = drawing_area->attr_array[SYNTAX_TYPE_STRING3];
    tmp_array[SYNTAX_TYPE_COMMENT1] = drawing_area->attr_array[SYNTAX_TYPE_COMMENT1];
    tmp_array[SYNTAX_TYPE_COMMENT2] = drawing_area->attr_array[SYNTAX_TYPE_COMMENT2];
    tmp_array[SYNTAX_TYPE_COMMENT3] = drawing_area->attr_array[SYNTAX_TYPE_COMMENT3];
    tmp_array[SYNTAX_TYPE_KEYWORD1] = drawing_area->attr_array[SYNTAX_TYPE_KEYWORD1];
    tmp_array[SYNTAX_TYPE_KEYWORD2] = drawing_area->attr_array[SYNTAX_TYPE_KEYWORD2];
    tmp_array[SYNTAX_TYPE_KEYWORD3] = drawing_area->attr_array[SYNTAX_TYPE_KEYWORD3];

    drawing_area->attr_array[SYNTAX_DULL] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_WORD] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_STRING1] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_STRING2] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_STRING3] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT1] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT2] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT3] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD1] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD2] = drawing_area->rev_attr;
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD3] = drawing_area->rev_attr;

    write_region
    (
    application.pixelWidth-application.innerBorder*2,
    application.pixelHeight-application.innerBorder*2,
    0, 0
    );

    drawing_area->attr_array[SYNTAX_DULL] = tmp_array[SYNTAX_DULL];
    drawing_area->attr_array[SYNTAX_WORD] = tmp_array[SYNTAX_WORD];
    drawing_area->attr_array[SYNTAX_TYPE_STRING1] = tmp_array[SYNTAX_TYPE_STRING1];
    drawing_area->attr_array[SYNTAX_TYPE_STRING2] = tmp_array[SYNTAX_TYPE_STRING2];
    drawing_area->attr_array[SYNTAX_TYPE_STRING3] = tmp_array[SYNTAX_TYPE_STRING3];
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT1] = tmp_array[SYNTAX_TYPE_COMMENT1];
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT2] = tmp_array[SYNTAX_TYPE_COMMENT2];
    drawing_area->attr_array[SYNTAX_TYPE_COMMENT3] = tmp_array[SYNTAX_TYPE_COMMENT3];
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD1] = tmp_array[SYNTAX_TYPE_KEYWORD1];
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD2] = tmp_array[SYNTAX_TYPE_KEYWORD2];
    drawing_area->attr_array[SYNTAX_TYPE_KEYWORD3] = tmp_array[SYNTAX_TYPE_KEYWORD3];

    write_region
    (
    application.pixelWidth-application.innerBorder*2,
    application.pixelHeight-application.innerBorder*2,
    0, 0
    );
#endif
}

void TerminalControl_GUI::fixup_user_interface()
{
    fixup_scroll_bars();
    fixup_menus();
    fixup_toolbars();
}

void TerminalControl_GUI::fixup_scroll_bars()
{
    int window_index = 0;

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        _dbg_msg("fixup_scroll_bars: begin");
# endif

    if( application.resources.display_scroll_bars )
    {
        EmacsWindow *w = windows.windows;

        int x = 0;
        int y = 0;

        EmacsWindow *old_window = windows.currentWindow();
        EmacsBufferRef old( bf_cur );

        // while windows and not the minibuffer window and there are scroll bars left
        while( w != NULL && w->w_next != NULL && window_index < max_scroll_bars )
        {
            if( vert_scroll[window_index] == NULL )
            {
                EmacsString name( FormatString("vert_scroll_%d") << window_index );
                vert_scroll[window_index] = new EmacsMotif_ScrollBarVertical
                    ( application, *drawing_area, name );

                vert_scroll[window_index]->preRealizeInit();
            }
            if( horz_scroll[window_index] == NULL )
            {
                EmacsString name( FormatString("horz_scroll_%d") << window_index );
                horz_scroll[window_index] = new EmacsMotif_ScrollBarHorizontal
                    ( application, *drawing_area, name );

                horz_scroll[window_index]->preRealizeInit();
            }

            if( w->w_height > 5 )
            {
//                vert_scroll[window_index]->manageWidget();
                vert_scroll[window_index]->position
                    (
                    application.innerBorder + application.ftw*(x + w->w_width),
                    application.innerBorder + application.fth*(y+1),
                    application.ftw*vertical_bar_width,
                    application.fth*(w->w_height-1-2)
                    );
                vert_scroll[window_index]->win = w;
                w->set_win();
                vert_scroll[window_index]->setValue( dot, bf_cur->num_characters() );
                vert_scroll[window_index]->manageWidget();
            }
            else
                vert_scroll[window_index]->unmanageWidget();

            if( w->w_width > 20 )
            {
//                horz_scroll[window_index]->manageWidget();
                horz_scroll[window_index]->position
                    (
                    application.innerBorder + application.ftw*(x + w->w_width - 10),
                    application.innerBorder + application.fth*(y + w->w_height - 1),
                    application.ftw*10,
                    application.fth
                    );
                horz_scroll[window_index]->win = w;
                horz_scroll[window_index]->setValue( w->w_horizontal_scroll );
                horz_scroll[window_index]->manageWidget();
            }
            else
                horz_scroll[window_index]->unmanageWidget();

            //
            // adjust x and y
            //
            if( w->w_right != NULL )
                // next is beside this one
                x += w->w_width + vertical_bar_width;
            else
                // next is below this one
                x = 0, y += w->w_height;
            // step to the next scroll_bar
            window_index++;

            // step to the next window
            w = w->w_next;
        }

        old_window->set_win();
        old.set_bf();
    }

    // unmanage all the scroll bars that are not in use
    while( window_index < max_scroll_bars )
    {
        if( vert_scroll[window_index] != NULL )
            vert_scroll[window_index]->unmanageWidget();
        if( horz_scroll[window_index] != NULL )
            horz_scroll[window_index]->unmanageWidget();
        // step to next
        window_index++;
    }

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        _dbg_msg("fixup_scroll_bars: end");
# endif
}

// Begin an update
bool TerminalControl_GUI::t_update_begin()
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        _dbg_msg( "update_begin();" );
# endif

    InUpdate++;
    if( InUpdate != 1 )
        return true;
    SavedX = curs_x;
    SavedY = curs_y;
    if( CursorExists )
        ToggleCursor();

    return true;
}

//
// Scan a buffer for the k'th occurrence of '\n',
// starting at position n.
//
int scan_nl(EmacsBuffer *buf, int n, int k )
{
    while( k != 0 )
    {
        n--;
        do
        {
            n++;
            if( n > buf->num_characters() )
                return n;
        }
        while( buf->char_at( n ) != '\n' );

        k--;
        if( k != 0 )
            n++;
    }

    return n + 1;
}

// End an update
void TerminalControl_GUI::t_update_end()
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        _dbg_msg( "update_end();" );
# endif

    InUpdate--;
    if( InUpdate != 0 )
        return;

    if( CursorExists )
        ToggleCursor();
    t_topos( SavedY, SavedX );

    InUpdate++;    // protect the following code

# ifdef STATUS_BAR_PRESENT
    //
    //    Update the status bar indicators
    //
    application.shell->main_window.screen_form.status_bar.form.updateIndicators();
# endif

    //
    // Recalculate the scrollbar position, and size
    //
    fixup_user_interface();

    XFlush( application.dpy );
    InUpdate--;    // remove protection

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        _dbg_msg( "update_end(); done" );
# endif
}

// What sections of the window will be modified from the UpdateDisplay
// routine is totally under soapplication.ftware control.  Any line with Y coordinate
// greater than flexlines will not change during an update.  This is really
// used only during dellines and inslines routines.

bool TerminalControl_GUI::t_window(void)
{
    return true;
}

bool TerminalControl_GUI::t_window( int n )
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "t_window(%d);" ) << n );
# endif

    if((n <= 0 ) ||( n > t_length ) )
        flexlines = t_length;
    else
        flexlines = n;

    return true;
}

// Insert n blank lines below the current line.
void TerminalControl_GUI::t_insert_lines( int n )
{
    register int topregion, bottomregion;
    register int length, newtop;

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "inslines(%d );" ) << n );
# endif

    if(curs_y > flexlines )
        return;
    if(CursorExists )
        ToggleCursor();

    topregion = curs_y;
    bottomregion = flexlines - n;
    newtop = curs_y + n;
    length =( bottomregion - topregion ) + 1;

    if((length > 0 ) &&( newtop <= flexlines ) )
        XCopyArea
        (
        application.dpy,
        XtWindow( drawing_area->widget ),
        XtWindow( drawing_area->widget ),
        drawing_area->attr_set.getAttr( SYNTAX_DULL ).getNormalGc(),
        0,
        (topregion-1)*application.fth + application.innerBorder,
        application.pixelWidth,
        length * application.fth,
        0,( newtop-1 ) * application.fth + application.innerBorder
        );

    newtop = min( newtop, flexlines );
    length = newtop - topregion;

    if(length > 0 )
        XFillRectangle
        (
        application.dpy,
        XtWindow( drawing_area->widget ),
        drawing_area->rev_attr.getNormalGc(),
        0,
        (topregion-1) * application.fth + application.innerBorder,
        application.pixelWidth, n * application.fth
        );

    if(!InUpdate )
        ToggleCursor();
}
// Delete n lines from the screen
void TerminalControl_GUI::t_delete_lines( int n )
{
# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_VERBOSE )
        _dbg_msg( FormatString( "delines(%d );" ) << n );
# endif

    if(curs_y > flexlines )
        return;

    if(CursorExists )
        ToggleCursor();
    if((curs_y + n ) > flexlines )
    {
        if(flexlines > curs_y )
            XFillRectangle
            (
            application.dpy,
            XtWindow( drawing_area->widget ),
            drawing_area->attr_set.getAttr( SYNTAX_DULL ).getNormalGc(),
            0,
            (curs_y - 1) * application.fth + application.innerBorder,
            application.pixelWidth,
            (flexlines - curs_y) * application.fth
            );
    }
    else
    {
        XCopyArea
        (
        application.dpy,
        XtWindow( drawing_area->widget ),
        XtWindow( drawing_area->widget ),
        drawing_area->attr_set.getAttr( SYNTAX_DULL ).getNormalGc(),
        0,
        (curs_y + n -1) * application.fth + application.innerBorder,
        application.pixelWidth,
        (flexlines + 1 - (curs_y + n)) * application.fth,
        0,
        (curs_y - 1) * application.fth + application.innerBorder
        );

        XFillRectangle
        (
        application.dpy,
        XtWindow( drawing_area->widget ),
        drawing_area->rev_attr.getNormalGc(),
        0,
        (flexlines - n) * application.fth + application.innerBorder,
        application.pixelWidth,
        n * application.fth
        );
    }

    if(!InUpdate )
        ToggleCursor();
}
// Update a single line of the display
void TerminalControl_GUI::t_update_line( EmacsLinePtr old_line, EmacsLinePtr new_line, int row )
{
    if( initializing )
        return;

    if( new_line.isNull() )
    {
        if( empty_line_ptr.isNull() )
            empty_line_ptr.newLine();

        new_line = empty_line_ptr;
    }

    int last_attr,start_col,end_col;

    // only write the line if its changed or there is no old line
    if( !old_line.isNull()
    && new_line->line_length == old_line->line_length
    && memcmp( new_line->line_body, old_line->line_body, new_line->line_length*sizeof( DisplayBody_t ) ) == 0
    && memcmp( new_line->line_attr, old_line->line_attr, new_line->line_length*sizeof( DisplayAttr_t ) ) == 0 )
        return;

    if( t_width > new_line->line_length )
    {
        //
        //    Fill out the line with spaces
        //
        memset
        (
        new_line->line_body + new_line->line_length,
        ' ',
        (t_width - new_line->line_length)*sizeof( DisplayBody_t )
        );
        // and an attribute of normal
        memset
        (
        new_line->line_attr + new_line->line_length,
        0,
        (t_width - new_line->line_length)*sizeof( DisplayAttr_t )
        );
    }

    DisplayBody_t new_line_text[MSCREENWIDTH];

    memcpy( new_line_text, new_line->line_body, MSCREENWIDTH*sizeof( DisplayBody_t ) );

    for( start_col=0; start_col<t_width; start_col++ )
        if( new_line_text[start_col] < ' ' )
            new_line_text[start_col] = ' ';

    start_col = 0;
    end_col = 1;
    while( start_col < t_width )
    {
        EmacsMotif_Attributes *attr;
        last_attr = new_line->line_attr[start_col];
        while( end_col < t_width && last_attr == new_line->line_attr[end_col] )
            end_col++;

        if( last_attr & LINE_M_ATTR_HIGHLIGHT )
            attr = &drawing_area->attr_set.getAttr( LINE_M_ATTR_HIGHLIGHT );
        else if( last_attr & LINE_ATTR_MODELINE )
            attr = &drawing_area->attr_set.getAttr( LINE_ATTR_MODELINE );
        else if( last_attr&LINE_ATTR_USER  )
            attr = &drawing_area->attr_set.getAttr( last_attr&LINE_M_ATTR_USER );
        else
            attr = &drawing_area->attr_set.getAttr( last_attr );
        if( attr->getNormalGc() == NULL )
        {
            attr = &drawing_area->attr_set.getAttr( SYNTAX_DULL );

            motif_message_box
            (
            FormatString("Bad last_attr value (0x%x) in t_update_line") << last_attr
            );
        }

        XDrawImageString
            (
            application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
            application.innerBorder + (start_col * application.ftw),
            application.innerBorder + (row * application.fth) - application.ftd,
            (char * )&new_line_text[start_col], end_col - start_col
            );
        if( attr->getUnderline()  )
        {
            int direction, ascent, descent;
            XCharStruct size;
            XTextExtents
            (
            application.normalFontStruct,
            (char * )&new_line_text[start_col], end_col - start_col,
            &direction, &ascent, &descent,
            &size
            );

            XDrawLine
            (
            application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
            application.innerBorder + (start_col * application.ftw),
            application.innerBorder + (row * application.fth) - application.ftd + 1,
            application.innerBorder + (start_col * application.ftw) + size.width,
            application.innerBorder + (row * application.fth) - application.ftd + 1
            );
        }

        for( int col = start_col; col<end_col; col++ )
            if( new_line->line_body[col] < ' ' )
            {
                char special = (char)new_line->line_body[col];
                special += 96;    // move from C0 to G0 special
                XDrawString
                    (
                    application.dpy, XtWindow( drawing_area->widget ), attr->getSpecialGc(),
                    application.innerBorder + (col * application.ftw),
                    application.innerBorder + (row * application.fth) - application.ftd,
                    &special, 1
                    );
            }
        start_col = end_col;
        end_col++;
    }
# if DBG_DISPLAY && DBG_TMP
    if( dbg_flags & DBG_DISPLAY && dbg_flags & DBG_TMP  )
    {
        _dbg_msg( "XFlush in t_update_line" );
        XFlush( application.dpy );
    }
# endif
}
// Ring the bell
void TerminalControl_GUI::t_beep()
{
    XBell( application.dpy, 0 );
}

// read an event from X-Windows

const int TIMER_TICK_VALUE( 50 );
static unsigned int due_tick_count;
static void( *timeout_handler )(void );
struct timeval timeout_time;

void time_schedule_timeout( void( *time_handle_timeout )(void ), const EmacsDateTime &when  )
{
//    TimerTrace( FormatString("time_schedule_timeout delta = %d") << delta );

    double int_part, frac_part;

    frac_part = modf( when.asDouble(), &int_part );
    frac_part *= 1000000.0;

    timeout_time.tv_sec = int( int_part );
    timeout_time.tv_usec = int( frac_part );

    timeout_handler = time_handle_timeout;
}

void time_cancel_timeout(void)
{
    TimerTrace( "time_cancel_timeout" );

    timeout_time.tv_sec = 0;
    timeout_time.tv_usec = 0;
    timeout_handler = NULL;
}

static XtIntervalId timer_id = 0;

void xwin_timeout_callback( XtPointer PNOTUSED(p), XtIntervalId *PNOTUSED(id ) )
{
    timer_id = 0;
    due_tick_count--;
}

static void timeout_check()
{
    if( timeout_handler != NULL  )
    {
        // if the timeout has occurred
        struct timeval now;
        struct timezone tzp;

        gettimeofday( &now, &tzp  );

        if( now.tv_sec > timeout_time.tv_sec
        ||( now.tv_sec == timeout_time.tv_sec && now.tv_usec > timeout_time.tv_usec  ) )
        //    due_tick_count <= 0  )
        {
            void( *handler )(void ) = timeout_handler;
            timeout_handler = NULL;
            handler();
        }
    }
}

extern int win_emacs_quit;

int TerminalControl_GUI::k_input_event( unsigned char *PNOTUSED(bufp), unsigned int PNOTUSED(numchars) )
{
    // set a timeout on the event processing of 50mS
    if( timer_id == 0 && timeout_handler != NULL )
        timer_id = XtAppAddTimeOut( application.app_context, TIMER_TICK_VALUE, xwin_timeout_callback, NULL  );

    XtInputMask mask = 0;
    do
    {
        XEvent e;
        XtAppPeekEvent( application.app_context, &e );

        XtAppProcessEvent( application.app_context, XtIMAll );
        mask = XtAppPending( application.app_context );
    }
    while( mask != 0  );

    if( action )
    {
        EmacsMotif_ActionItem *it = action;
        action = NULL;
        if( it->doAction() < 0 )
            return -1;
    }

    if( win_emacs_quit )
    {
        win_emacs_quit = 0;
        return -1;
    }

    if( child_changed  )
        change_msgs();

    timeout_check();

    return 0;
}
// Set the X-Windows pointer according to the emacs activity state
void xwin_activity( unsigned char PNOTUSED(ch ) )
{
# ifdef ACTIVITY
    XSetWindowAttributes xswa;

    if( cursors )
    {
        switch( ch )
        {
        case 'b': case 'j': case 'c':
            xswa.cursor = watchCursor;
            break;

        default:
            xswa.cursor = arrowCursor;
            break;
        }

        XChangeWindowAttributes(application.dpy, win, CWCursor, &xswa );
    }
# endif
}
//
// Input functions
//

// Feed a string into Emacs
void TerminalControl_GUI::input_char_string( const EmacsString &keys, bool shift )
{
        k_input_char_string( keys.data(), shift );
}

// Process a key stroke

void TerminalControl_GUI::keyboardInput( XKeyEvent *event )
{
    char keybuf[10];
    int nChar;
    KeySym k;
    static XComposeStatus compose_status; // Rely on default initialisation to 0

    k  = XLookupKeysym(event,0 );
# if DBG_KEY
    if( dbg_flags&DBG_KEY  )
        printf("Input: keycode %d state 0x%.4x keysym 0x%.4x ",
                event->keycode, event->state, (unsigned)k );
# endif
# ifdef KEYBOARD_TYPE_PC
    if( k == 0 && event->keycode == 84  )
        k = XK_KP_5;
# endif
    const unsigned char *keys = keysym_map.lookup( k, event->state );
    if( keys != NULL )
    {
# if DBG_KEY
        if( dbg_flags&DBG_KEY  )
            printf("converted to ESC-%s\n", &keys[1]  );
# endif
        input_char_string( keys, event->state & ShiftMask );
        return;
    }


    nChar = XLookupString(event, keybuf, sizeof( keybuf ), NULL, &compose_status );
    keybuf[nChar] = '\0';
# if DBG_KEY
    if( dbg_flags&DBG_KEY  )
        printf("is %d char 0x%.2x\n", nChar, keybuf[0] );
# endif
    if( nChar > 0  )
        k_input_char( keybuf[0], false );
}
// Process a mouse click
void TerminalControl_GUI::mouseInput( XButtonEvent *event )
{
    if( !mouse_enable  )
        return;

    // Calculate character cell position
    int column = (event->x - application.innerBorder + (application.ftw/2)) / application.ftw + 1;
    int line =   (event->y - application.innerBorder ) / application.fth + 1;

    if( event->type == ButtonPress )
        mouse_button_state |= 1<<event->button;
    else
    {
        // do not process a ButtonRelease if we have not seen the ButtonPress
        if( (mouse_button_state & (1<<event->button)) == 0 )
            return;

        mouse_button_state &= ~(1<<event->button);
    }

    int button;
    switch( event->button )
    {
    case Button1:
        button = event->type == ButtonPress ? 2 : 3;
        break;
    case Button2:
        button = event->type == ButtonPress ? 4 : 5;
        break;
    case Button3:
        button = event->type == ButtonPress ? 6 : 7;
        break;

    // Cygwin Xfree86 uses 4 and 5 to send the mouse wheel turns
    case Button4:
    case Button5:
    {
        if( event->type == ButtonPress )
        {
            int mode = 0;
            if( event->button == Button5 )
                mode |= 1;    // -ive wheel motion
            if( (event->state & ShiftMask) != 0 )
                mode |= 4;    // shift down
            if( (event->state & ControlMask) != 0 )
                mode |= 8;    // ctrl down

            EmacsString mouse( FormatString("\x1b[%d;%d;%d;%d#w")
                << mode << 1 << column << line );
            input_char_string( mouse, event->state & ShiftMask );
        }
        return;
    }
    default:
        _dbg_msg( FormatString( "Unexpected mouse button. event->button: %x" ) <<event->button );
        return;
    }


    last_mouse_x = column;
    last_mouse_y = line;
    EmacsString mouse( FormatString("\x1b[%d;%d;%d;%d&w")
        << button << event->state << line << column );
    //_dbg_msg( FormatString("Mouse Button: button=%d state=%d line=%d col=%d")
    //    << button << event->state << line << column );
    input_char_string( mouse, event->state & ShiftMask );
}

void TerminalControl_GUI::mouseMotionInput( XMotionEvent *event )
{
    if(( mouse_enable&MOUSE_MOTION_EVENTS ) == 0  )
        return;

    // only send a mouse motion if nothing else is in the input queue
    if( input_pending != 0 )
        return;

    // only send motion events if a button is pressed
    if( (event->state & (Button1MotionMask|Button2MotionMask|Button3MotionMask)) == 0 )
        return;

    // Calculate character cell position
    int column = (event->x - application.innerBorder + (application.ftw/2)) / application.ftw + 1;
    int line =   (event->y - application.innerBorder) / application.fth + 1;

    if( last_mouse_x != column
    ||  last_mouse_y != line )
    {
        last_mouse_x = column;
        last_mouse_y = line;
        EmacsString mouse( FormatString("\x1b[%d;%d;%d;%d&w") << 0 << event->state << line << column );
        input_char_string( mouse, event->state & ShiftMask );
    }
}


// work structure to queue on a timeout
class Synchronise_files_work_item : public EmacsWorkItem
{
public:
    virtual void workAction(void)
    {
        synchronise_files();
    }
};
static Synchronise_files_work_item synchronise_files_request;

extern SystemExpressionRepresentationIntBoolean synchronise_buffers_on_focus;


// Handle a keyboard or mouse input event from the drawing area widget
void EmacsMotif_Screen_DrawingArea::input( void *PNOTUSED(junk ), void *event_ )
{
    XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;
    extern int motif_insensitive;

# if DBG_DISPLAY
    if( dbg_flags & DBG_DISPLAY )
        debugWidget( "EmacsMotif_Screen_DrawingArea::input" );
# endif

    if( motif_insensitive )
        return;

    switch( event->event->type )
    {
    case KeyPress:
        theMotifGUI->keyboardInput( &event->event->xkey );
        break;

    case KeyRelease:
        break;

    case ButtonPress:
    case ButtonRelease:
        theMotifGUI->mouseInput( &event->event->xbutton );
        break;

    case MotionNotify:
        theMotifGUI->mouseMotionInput( &event->event->xmotion );
        break;

    case FocusIn:
        if( synchronise_buffers_on_focus )
            synchronise_files_request.addItem();
        break;

    default:
        _dbg_msg( FormatString( "Unknown input event = %d" ) << event->event->type );
        break;
    }

    if( screen_garbaged )
    {
        theActiveView->do_dsp();
        screen_garbaged = 0;
    }
}

// Handle an exposure event from the drawing area widget
void EmacsMotif_Screen_DrawingArea::expose( void * PNOTUSED(junk), void *event_ )
{
    XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;
    initializing = 0;
    switch( event->event->type )
    {
    case NoExpose:
        break;

    case GraphicsExpose:
    case Expose:
        if( motif_iconic )
            ui_frame_restore();
        theMotifGUI->write_region
            (
            event->event->xexpose.width,
            event->event->xexpose.height,
            event->event->xexpose.x,
            event->event->xexpose.y
            );
        break;

    default:
        _dbg_msg( FormatString( "Unknown exposure event = %d" ) << event->event->type );
        break;
    }

    if( screen_garbaged )
    {
        theActiveView->do_dsp();
        screen_garbaged = 0;
    }
}
// Handle a resize event from the drawing area widget
void EmacsMotif_Screen_DrawingArea::resize( void *PNOTUSED(junk ), void *PNOTUSED(event_ ) )
{
    int width  = widget->core.width;
    int height = widget->core.height;

    if( height != application.pixelHeight || width != application.pixelWidth )
    {
        int old_length = theActiveView->t_length;
        int old_width = theActiveView->t_width;

        int newHeight = (height - application.innerBorder * 2 ) / application.fth;
        int newWidth = ((width - application.innerBorder * 2) / application.ftw) - vertical_bar_width;

# if DBG_DISPLAY
        if( dbg_flags&DBG_DISPLAY )
            _dbg_msg( FormatString("resize_cb: from height %d to %d, width %d to %d")
                << old_length << newHeight << old_width << newWidth );
# endif
        if( newHeight < 5 )
            return;

        application.pixelWidth = (newWidth+vertical_bar_width) * application.ftw + application.innerBorder * 2;
        term_width = theActiveView->t_width = newWidth;
        term_length = theActiveView->t_length = newHeight;
        flexlines = theActiveView->t_length - 1;
        if( theActiveView->currentWindow() != NULL )
        {
            if(theActiveView->t_length != old_length || theActiveView->t_width != old_width )
            {
                theActiveView->fit_windows_to_screen();
                screen_garbaged = 1;
                if( attr_set.getAttr( SYNTAX_DULL ).getNormalGc() != NULL  )
                    theActiveView->do_dsp();
            }
        }
    }
}


int motif_not_running()
{
    error( "Not running in an X-Windows/MOTIF environment" );
    return 0;
}

//
// Widget manipulate functions
//


// draw in the colours of the widgetstatic

// Realise the emacs widgets
int TerminalControl_GUI::create_windows( const EmacsString &display_location )
{
    if( !application.initApplication( display_location ) )
        return 0;

    if( !keysym_map.init() )
        return 0;

    drawing_area = &application.shell->main_window.screen_form.screen.drawing_area;

    application.shell->callPreRealizeInit();

    XtRealizeWidget( application.shell->widget );

    application.shell->callPostRealizeInit();

    return 1;
}


void TerminalControl_GUI::t_change_attributes()
{
    // trim to the valid mouse modes
    mouse_enable = mouse_enable & (MOUSE_BUTTON_EVENTS|MOUSE_MOTION_EVENTS);

    // force MOUSE_MOTION_EVENTS off if MOUSE_BUTTON_EVENTS is not enabled
    if(( mouse_enable&MOUSE_BUTTON_EVENTS ) == 0  )
        mouse_enable = 0;
}

void TerminalControl_GUI::k_check_for_input()
{
    unsigned char buf[128];
    int i;
    int size;

    XtInputMask mask = 0;

    while( (mask = XtAppPending( application.app_context )) != 0  )
    {
        size = k_input_event( buf, sizeof( buf )  );
        if( size >= 1  )
        {
            for( i=0; i<size; i++  )
                k_input_char( buf[i], false  );
        }
    }

    timeout_check();
}

void TerminalControl_GUI::t_display_activity(unsigned char ch)
{
    EmacsMotif_Attributes *attr = &drawing_area->attr_set.getAttr( SYNTAX_DULL );
    XDrawImageString
        (
        application.dpy, XtWindow( drawing_area->widget ), attr->getNormalGc(),
        application.innerBorder,
        term_length * application.fth + application.innerBorder - application.ftd,
        (char *)&ch, 1
        );
}

// Initialization function
int init_gui_terminal( const EmacsString &displayName )
{
    TerminalControl_GUI *gui = new TerminalControl_GUI( displayName );

    theMotifGUI = gui;
    theActiveView = gui;
    theActiveView->t_il_mf = 1;
    theActiveView->t_il_ov = 1;
    theActiveView->t_ic_ov = MISSINGFEATURE;
    theActiveView->t_dc_ov = MISSINGFEATURE;
    theActiveView->t_baud_rate = 1000000;

    term_app_keypad=1;
    term_edit=1;
    term_eightbit=1;

    CursorExists = 0;
    InUpdate = 0;
    VisibleX = 1;
    VisibleY = 1;
    flexlines = theActiveView->t_length -2;
    is_motif = 1;

    if( !gui->create_windows( displayName ) )
        return false;

    gui->updateWindowTitle();

    return true;
}

void UI_update_window_title(void )
{
    if( theMotifGUI != NULL )
        theMotifGUI->updateWindowTitle();
}

extern EmacsString name_arg;
void TerminalControl_GUI::updateWindowTitle()
{
    char title[128];
    EmacsString home( getenv("HOME") );
    EmacsString cwd( current_directory.asString() );

    if( application.shell == NULL  )
        return;

    if( name_arg.isNull() )
    {
        strcpy( title, "Barry's Emacs V7 - "  );
    }
    else
    {
        strcpy( title, name_arg.sdata() );
        strcat( title, " - " );
    }
    if( cwd.commonPrefix( home ) == home.length() )
    {
        strcat( title, "~/"  );
        cwd.remove( 0, home.length() );
        if( cwd.length() > 0 && cwd[0] == '/'  )
            cwd.remove( 0, 1 );
        strcat( title, cwd );
    }
    else
        strcat( title, current_directory.sdata() );

    EmacsXtArgs args;
    args.setArg( XmNtitle, &title );
    args.setArg( XmNiconName, &title );

    args.setValues( application.shell->widget );
}

static int motif_yes;

static void yes_cb( Widget PNOTUSED(w), void *PNOTUSED(d_), void *PNOTUSED(junk) )
{
    motif_yes = 1;
}

static void no_cb( Widget PNOTUSED(w), void *PNOTUSED(d_), void *PNOTUSED(junk) )
{
    motif_yes = 0;
}

static XmStringCharSet charset = (XmStringCharSet)XmSTRING_DEFAULT_CHARSET;

int motif_yes_or_no( int yes, const EmacsString &prompt )
{
    EmacsMotif_Shell *shell = theMotifGUI->application.shell;

    int x = shell->widget->core.x;
    int y = shell->widget->core.y;

    EmacsXtArgs args;
    args.setArg( XmNx, x + 100 );
    args.setArg( XmNy, y + 100 );
    args.setArg( XmNautoUnmanage, 0l );
    args.setArg( XmNdefaultPosition, 0l );
    Widget w = XmCreateQuestionDialog( shell->widget, "Emacs", args, args );
    if( w == NULL )
        return 0;

    // add the call back to get the yes answer
    XtAddCallback( w, XmNokCallback, yes_cb, (XtPointer)w );

    // no help button for us
    Widget button = XmMessageBoxGetChild( w, XmDIALOG_HELP_BUTTON );
    XtDestroyWidget( button );

    // change "Cancel" to "No"
    Widget no_button = XmMessageBoxGetChild( w, XmDIALOG_CANCEL_BUTTON );
    XmString p = XmStringCreateLtoR( "No", charset );
    args.setArg( XmNlabelString, p );
    args.setArg( XmNshowAsDefault, yes ? 0 : 1 );
    args.setValues( no_button );
    XmStringFree( p );

    // add the call back to get the no answer
    XtAddCallback( w, XmNcancelCallback, no_cb, (XtPointer)w );

    Widget yes_button = XmMessageBoxGetChild( w, XmDIALOG_OK_BUTTON );

    p = XmStringCreateLtoR( "Yes", charset );
    args.setArg( XmNlabelString, p );
    args.setArg( XmNshowAsDefault, yes ? 1 : 0 );
    args.setValues( yes_button );
    XmStringFree( p );

    p = XmStringCreateLtoR( prompt.sdataHack(), charset );
    args.setArg( XmNwindowGroup, XtUnspecifiedWindowGroup );
    args.setArg( XmNdefaultButton, yes ? yes_button : no_button );
    args.setArg( XmNmessageString, p );
    args.setValues( w );
    XmStringFree( p );

    XtManageChild( w );

    extern int motif_insensitive;

    motif_insensitive = 1;
    motif_yes = -1;
    while( motif_yes < 0 )
        theActiveView->k_input_event( NULL, 0 );
    motif_insensitive = 0;

    XtUnmanageChild( w );

    XtDestroyWidget( w );

    return motif_yes;
}

void motif_message_box( const EmacsString &message )
{
    EmacsMotif_Shell *shell = theMotifGUI->application.shell;

    int x = shell->widget->core.x;
    int y = shell->widget->core.y;

    EmacsXtArgs args;
    args.setArg( XmNx, x + 100 );
    args.setArg( XmNy, y + 100 );
    args.setArg( XmNautoUnmanage, 0l );
    args.setArg( XmNdefaultPosition, 0l );
    Widget w = XmCreateQuestionDialog( shell->widget, "Emacs", args, args );
    if( w == NULL )
        return;

    // add the call back to get the yes answer
    XtAddCallback( w, XmNokCallback, yes_cb, (XtPointer)w );

    // no help button for us
    Widget button = XmMessageBoxGetChild( w, XmDIALOG_HELP_BUTTON );
    XtDestroyWidget( button );
    button = XmMessageBoxGetChild( w, XmDIALOG_CANCEL_BUTTON );
    XtDestroyWidget( button );

    Widget yes_button = XmMessageBoxGetChild( w, XmDIALOG_OK_BUTTON );

    XmString p = XmStringCreateLtoR( "Continue", charset );
    args.setArg( XmNlabelString, p );
    args.setArg( XmNshowAsDefault, 1 );
    args.setValues( yes_button );
    XmStringFree( p );

    p = XmStringCreateLtoR( message.sdataHack(), charset );
    args.setArg( XmNwindowGroup, XtUnspecifiedWindowGroup );
    args.setArg( XmNdefaultButton, yes_button );
    args.setArg( XmNmessageString, p );
    args.setValues( w );
    XmStringFree( p );

    XtManageChild( w );

    extern int motif_insensitive;

    motif_insensitive = 1;
    motif_yes = -1;
    while( motif_yes < 0 )
        theActiveView->k_input_event( NULL, 0 );
    motif_insensitive = 0;

    XtUnmanageChild( w );

    XtDestroyWidget( w );
}

int ui_view_status_bar(void)
{
    theMotifGUI->viewStatusBar();

    ml_value = int( theMotifGUI->application.resources.display_status_bar );

    return 0;
}

void TerminalControl_GUI::viewStatusBar()
{
    application.resources.display_status_bar = !application.resources.display_status_bar;

    application.shell->main_window.screen_form.configureScreenGeometry();
}

int ui_view_scroll_bars(void)
{
    theMotifGUI->application.resources.display_scroll_bars =
        !theMotifGUI->application.resources.display_scroll_bars;
    theActiveView->do_dsp();

    ml_value = int( theMotifGUI->application.resources.display_scroll_bars );

    return 0;
}

int ui_view_tool_bar(void)
{
    theMotifGUI->viewToolBar();

    ml_value = int( theMotifGUI->application.resources.display_tool_bar );

    return 0;
}

void TerminalControl_GUI::viewToolBar()
{
    application.resources.display_tool_bar = !application.resources.display_tool_bar;

    application.shell->main_window.screen_form.configureScreenGeometry();
}

#undef malloc
#undef free

void *TerminalControl_GUI::operator new(size_t size)
{ return malloc( size ); }

void TerminalControl_GUI::operator delete(void *p)
{ free( p ); }


#endif
