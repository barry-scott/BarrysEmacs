//    Copyright (c) 1982-1995
//        Barry A. Scott

//
// Terminal control module for ANSI terminals
//
// Barry A. Scott     6-Feb-1984
//

#include <emacs.h>

#include <tt_unix.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );



//forward
const char SO( 016 );
const char SI( 017 );

void TerminalControl_CHAR::PAD( int n, float f )
    {
        if( !term_nopadding ) pad( n, f );
    }

void TerminalControl_CHAR::t_highlight_mode( int on )
{
    if (cur_hl == on)
        return;

    if( on )
        t_io_printf( "\233%sm", mode_line_rendition.sdata() );
    else
        t_io_printf( "\233%sm", window_rendition.sdata() );

    cur_hl = on;
}

static int cur_ins;
void TerminalControl_CHAR::t_insert_mode( int on )
{
    if( cur_ins == on ) return;

    if( on )
        t_io_print( u_str( "\2334h" ) );
    else
        t_io_print( u_str( "\2334l" ) );

    cur_ins = on;
}

void TerminalControl_CHAR::t_insert_lines( int n )
{
    if( !term_edit )
    {
        t_io_printf( "\233%d;%dr\233%d;1H",
            ansi_cur_y, window_size, ansi_cur_y);
        ansi_cur_x = 1;
        while (--n >= 0)
        {
            t_io_print( u_str( "\033M" ));
            PAD (1, 30.);
        }
        t_io_print( u_str( "\233r" ));
    }
    else
        t_io_printf( "\233%d;%dr\233%dL\233r",
            ansi_cur_y, window_size, n );
    ansi_cur_x = ansi_cur_y = 1;
}
void TerminalControl_CHAR::t_delete_lines( int n )
{
    if( !term_edit )
    {
        t_io_printf( "\233%d;%dr\233%dH",
                ansi_cur_y, window_size, window_size);
        ansi_cur_x = 1;
        ansi_cur_y = window_size;
        while (--n >= 0)
        {
            t_io_print( u_str( "\033E" ));
            PAD (1, 30.);        // [see above comment]
        }
        t_io_print( u_str( "\233r" ));
    }
    else
        t_io_printf( "\233%d;%dr\233%dM\233r",
            ansi_cur_y, window_size, n );
    ansi_cur_x = ansi_cur_y = 1;
}

static float baud_factor;
void TerminalControl_CHAR::pad( int n, float f)
{
    register int k = (int)(n * f * baud_factor);
    while (--k >= 0)
        t_io_putchar( 0);
}

void TerminalControl_CHAR::t_topos( int row, int column )
{
    ansi_topos( row, column, 0, 0 );
}

void TerminalControl_CHAR::ansi_topos
    (
    int row, int column,
    DisplayBody_t *cur_line,
    DisplayAttr_t *cur_line_attr
    )
{
    //
    //    This test check_ to see if we may have hit the
    //    edge of the hardware screen. If so the cursor
    //    is in fact in ansi_cur_x - 1. Except if the terminal
    //    is a VWS or a VTxxx in column 80 or 132.
    //
    //    The the above conditions cannot be determined do
    //    absolute cursor movement.
    //
    if( ansi_cur_x > t_width )
    {
        if( ansi_cur_y == row )
            if( column-1 )
                t_io_printf( "\015\233%dC", column-1 );
            else
                t_io_putchar( '\015');
        else
            t_io_printf( "\233%d;%dH", row, column );
        goto done;
    }
    if (ansi_cur_y == row)
    {
        int diff;
        //
        //    movement within a line
        //
        if( ansi_cur_x == column ) return;

        if( column == 1 )
        {
            t_io_putchar(  '\r' ); goto done;
        }
        if( column > ansi_cur_x )
        {
            // moving forward along the line
            diff = column - ansi_cur_x;

            if( cur_line == 0 )
            {
                if( diff == 1 )
                    t_io_print( u_str("\233C") );
                else
                    t_io_printf( "\233%dC", diff );
            }
            else
            {
                cur_line += ansi_cur_x - 1;
                cur_line_attr += ansi_cur_x - 1;
                switch( diff )
            {
                case 3:    ansi_putchar(  *cur_line++, *cur_line_attr++ );
                case 2:    ansi_putchar(  *cur_line++, *cur_line_attr++ );
                case 1:    ansi_putchar(  *cur_line++, *cur_line_attr++ );
                    break;
                default:
                    t_io_printf( "\233%dC", diff );
                    break;
            }
            }
        }
        else
        {
            // moving backward along the line
            diff = ansi_cur_x - column;
            switch( diff )
        {
            case 3:    t_io_putchar( '\010');
            case 2:    t_io_putchar( '\010');
            case 1:    t_io_putchar( '\010');
                break;
            default:
                t_io_printf( "\233%dD", diff );
                break;
        }
        }
        goto done;
    }
    if (ansi_cur_y + 1 == row && (column == 1 || column==ansi_cur_x))
    {
        if(column!=ansi_cur_x) t_io_putchar( 015);
        t_io_putchar( 012);
        goto done;
    }
    if (row == 1 && column == 1)
    {
        t_io_print( u_str( "\233H" ));
        goto done;
    }
    t_io_printf( "\233%d;%dH", row, column );
done:
    ansi_cur_x = column;
    ansi_cur_y = row;
}

void TerminalControl_CHAR::ansi_putchar( DisplayBody_t ch, DisplayAttr_t region_highlight )
{
    if( region_highlight != cur_region_highlight )
    {
        if (region_highlight & LINE_M_ATTR_HIGHLIGHT)
            t_io_printf( "\233;%sm", region_rendition.sdata() );
        else if (region_highlight & LINE_ATTR_MODELINE)
            t_io_printf( "\233;%sm", mode_line_rendition.sdata() );
        else
            t_io_printf( "\233%sm", window_rendition.sdata() );
        cur_region_highlight = region_highlight;
    }
    if( ch >= ' ' )
    {
        if( cur_special_graphics && ch > '^' && ch < '\177' )
        {
            cur_special_graphics = 0;
            t_io_putchar(  SI );
        }
        t_io_putchar(  ch );
    }
    else
    {
        if( !cur_special_graphics )
        {
            cur_special_graphics = 1;
            t_io_putchar(  SO );
        }
        t_io_putchar(  ch+0140 );
    }
}

static EmacsLinePtr empty_line_ptr;

void TerminalControl_CHAR::t_update_line( EmacsLinePtr old_line, EmacsLinePtr new_line, int ln )
{
    if( new_line.isNull() )
    {
        if( empty_line_ptr.isNull() )
            empty_line_ptr.newLine();

        new_line = empty_line_ptr;
    }

    if( old_line.isNull() )
    {
        if( empty_line_ptr.isNull() )
            empty_line_ptr.newLine();

        old_line = empty_line_ptr;
    }

    register DisplayBody_t *old_t_p;    // pointer to old lines text
    register DisplayBody_t *new_t_p;    // pointer to new lines text
    register DisplayAttr_t *old_a_p;    // pointer to old lines attr
    register DisplayAttr_t *new_a_p;    // pointer to new lines attr
    register int len;            // holds min( o_len, n_len )
    register int col;            // column that the software cursor is in
    register int diff;            // holds differance between n_len and o_len
    register DisplayBody_t *cur_line;    // hold a pointer to the start of the new lines text
    register DisplayAttr_t *cur_line_attr;    // hold a pointer to the start of the new lines text

    int n_len = len = new_line->line_length;
    int o_len = old_line->line_length;
    if( o_len == 0 && n_len == 0 )
        return;

    cur_line = new_t_p = &new_line->line_body[0];
    cur_line_attr = new_a_p = &new_line->line_attr[0];

    if( len > o_len )
        len = o_len;

    old_t_p = &old_line->line_body[0];
    old_a_p = &old_line->line_attr[0];

    // start in the first column
    col=1;

    //
    //    Work out the offset to the first change
    //
    while( col <= len )
    {
        if( *old_t_p != *new_t_p ) break;
        if( *old_a_p != *new_a_p ) break;

        col++; old_t_p++; new_t_p++; old_a_p++; new_a_p++;
    }

    diff = n_len - o_len;

    //
    //    If we have character editing see if it can be used
    //
    if( term_edit )
    //
    //    Only bother to use insert delete if the old and new lines
    //    are differant, but if the change from old to new is either
    //    to kill to end of line or insert a string at the end of the
    //    line do not use INSmode or DeleteChars.
    //
    if( diff != 0        // lines must have differant length
     && len != col-1    // differance must not be at the end of either line
     && (diff>0 ? diff : -diff ) < 9 // and the diff must be small
     )
    {
        ansi_topos( ln, col, cur_line, cur_line_attr );

        if( diff > 0 )
        {
            //
            //    Need to do an insert unsigned char here
            //
            ansi_cur_x += diff;    // move cur_x alone
            t_insert_mode(1);
            while( diff-- )
            {
                ansi_putchar(  *new_t_p++, *new_a_p++ );
                col++;
            }
            t_insert_mode(0);
        }
        else
        {
            //
            //    Need to do a delete unsigned char here
            //
            diff = -diff;
            t_io_printf( "\233%dP", diff );
            old_t_p += diff;
            old_a_p += diff;
        }
        diff = 0;       // stop after updating the changed part
        len = n_len;    // we can now process to the end of the new line
    }
    //
    //    Update the changed part of the line
    //
    for(; col <= len;
        col++, old_t_p++, new_t_p++, old_a_p++, new_a_p++ )
    {
        if( *old_t_p == *new_t_p && *old_a_p == *new_a_p )
            continue;

        ansi_topos( ln, col, cur_line, cur_line_attr );
        ansi_putchar(  *new_t_p, *new_a_p );
        ansi_cur_x++;
    }
    //
    //    If the old line is longer than the new erase to end of line
    //
    if( diff == 0 ) goto tidy_up_and_exit;
    if( diff < 0 )
    {
        //
        //    The new line is shorter then the old
        //
        ansi_topos( ln, col, cur_line, cur_line_attr  );

        switch( -diff )
    {
        case 1:    ansi_putchar(  ' ', 0 );
            ansi_cur_x++;
            break;
        default:t_io_print( u_str( "\233K" ));
            break;
    }
        goto tidy_up_and_exit;
    }
    //
    //    The new line is longer then the old so
    //    print the rest of the line.
    //
    //    But take into account topos's ability to deal with
    //    white space if the line is not highlighted.
    //
    while( diff )
    {
        // skip spaces and let topos cope with them
        for( ; diff; diff--, new_t_p++, new_a_p++, col++ )
            if( *new_t_p != ' ' || *new_a_p != 0 ) break;
        // move over to output the printing unsigned char
        if( diff )
            ansi_topos( ln, col, cur_line, cur_line_attr );
        // output chars until a space
        for(; diff; diff--, col++ )
        {
            if( *new_t_p == ' ' && *new_a_p == 0 ) break;

            ansi_putchar(  *new_t_p++, *new_a_p++ );
            ansi_cur_x++;
        }
    }
tidy_up_and_exit:
    if( cur_region_highlight )
    {
        t_io_printf( "\233%sm", window_rendition.sdata() );
        cur_region_highlight = 0;
    }
    if( cur_special_graphics )
    {
        cur_special_graphics = 0;
        t_io_putchar(  SI );
    }
}

void TerminalControl_CHAR::t_init()
{
    t_change_attributes();
    baud_factor = t_baud_rate/9600.0;
}

void TerminalControl_CHAR::t_reset()
{
    //
    //    Too reset terminal
    //        scroll-region        r
    //        graphic rendition    m
    //        Insert replace        4l
    //        Jump scroll        ?4l
    //        Wrap around        ?7l
    //        Clear screen        2J
    //        Select US chars as G0    (B
    //        Select Special graphics
    //            chars as G1    )0
    //        G0 as current chars    ^O
    //
    //        Origin mode set        ?6h     for EDIT
    //        Origin mode reset    ?6l     for NOEDIT
    //
    t_io_printf
    (
    "\233r\233m\2334l\233?4l\233?6%c\233?7l\2332J\033(B\033)0\017",
    term_edit ? 'h' : 'l'
    );
    PAD (1, 100.);
    window_size = t_length;
    cur_hl = 0; cur_ins = 0;
    ansi_cur_x = ansi_cur_y = 1;
}

void TerminalControl_CHAR::t_cleanup()
{
    t_highlight_mode( 0 );
    t_window( 0 );
    t_io_printf( "\233?6l\233%dH\233K", window_size );
    t_io_flush();
    term_restore_charactistics();
}

void TerminalControl_CHAR::t_wipe_line( int line )
{
    t_topos( line, 0 );
    t_io_print( u_str( "\233K" ));
    PAD (1, 3.);
}

bool TerminalControl_CHAR::t_window( int n )
{
    if (n <= 0 || n > t_length)
        n = t_length;
    window_size = n;

    return true;
}

void TerminalControl_CHAR::t_flash()
{
    t_io_print( black_on_white ? u_str( "\233?5l" ) : u_str( "\233?5h" ));
    PAD (1, 30.);
    t_io_print( black_on_white ? u_str( "\233?5h" ) : u_str( "\233?5l" ));
    PAD (1, 30.);
}

void TerminalControl_CHAR::t_change_width( int width )
{
    t_io_print( width > 80 ? u_str( "\233?3h" ) : u_str( "\233?3l" ));
    PAD (1, 130.);
}

void TerminalControl_CHAR::t_beep()
{
    t_io_putchar( 7 );    // bell
}

#ifdef __unix__
extern int read_inputs( int, unsigned char *, unsigned int );
int TerminalControl_CHAR::k_input_event( unsigned char *b, unsigned int s )
{
    return read_inputs( input_channel, b, s );
}
#endif

static int in_update;
bool TerminalControl_CHAR::t_update_begin()
{
    in_update = 1;
    return true;
}

void TerminalControl_CHAR::t_update_end()
{
    in_update = 0;
    t_io_flush();
}

void TerminalControl_CHAR::t_display_activity( unsigned char it )
{
    if( in_update )
        return;
    t_io_printf( "\033" "7" "\033[%d;1H%c\033" "8", t_length, it );
    t_io_flush();
}

TerminalControl_CHAR::TerminalControl_CHAR( const EmacsString &dev )
    : EmacsView()
    , ansi_cur_x(0)
    , ansi_cur_y(0)
    , window_size(0)
    , cur_region_highlight(0)
    , cur_special_graphics(0)
    , cur_hl(0)
{
    term_io_channels( dev );
}

TerminalControl_CHAR::~TerminalControl_CHAR()
{ }

int init_char_terminal( const EmacsString &device )
{
    theActiveView = new TerminalControl_CHAR( device );
    return 1;
}

#undef malloc
#undef free

void *TerminalControl_CHAR::operator new(size_t size)
{ return malloc( size ); }

void TerminalControl_CHAR::operator delete(void *p)
{ free( p ); }
