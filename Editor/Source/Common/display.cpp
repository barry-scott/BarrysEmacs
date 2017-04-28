//
//     Copyright(c) 1982-2001
//        Barry A. Scott
//
//    display.cpp
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#ifdef vms
#include <ttdef.h>
#endif

void generic_updateline( EmacsLine *old_line, EmacsLine *new_line, int ln );
void ding( void );
int sit_for( void );
int sit_for_inner(int num_chars);
void init_display( void );


SystemExpressionRepresentationTermProtocolMode protocol_mode( 1 );
int last_redisplay_paused;
int screen_garbaged;        // set to 1 iff screen content is uncertain.
int curs_x;                 // X and Y coordinates of the cursor
int curs_y;                 // between updates.
int current_line;           // current line for writing to the virtual screen.
int columns_left;           // number of columns left on the current line
                            // of the virtual screen.
DisplayBody_t *text_cursor;
DisplayAttr_t *attr_cursor; // pointer into a line object, indicates where
                            // to put the next character


#if !MEMMAP
static int check_for_input;    // -ve iff UpdateLine should bother checking for input
#endif


// initialize the teminal package
void init_display( void )
{
    curs_x = 1;        // X and Y coordinates of the cursor
    curs_y = 1;        // between updates.
    current_line = -1;    // current line for writing to the virtual screen.
    columns_left = -1;    // number of columns left on the current line of the virtual screen.
}


void SystemExpressionRepresentationTermProtocolMode::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );

    set_protocol( exp_int );
}

void SystemExpressionRepresentationTermProtocolMode::fetch_value()
{
#ifdef vms
    protocol_mode = (tt->t_cur_attributes.v_char_1 & (TT$M_TTSYNC | TT$M_HOSTSYNC)) != 0;
#else
    protocol_mode = 0;
#endif
}

void SystemExpressionRepresentationScreenWidth::assign_value( ExpressionRepresentation *new_value )
{
    int width = new_value->asInt();
    if( width < 32 || width > MSCREENWIDTH )
        error( FormatString("terminal-width must be > 32 and <= %d") << MSCREENWIDTH );
    else
    {
        term_width = width;
        theActiveView->t_width = width;
        theActiveView->t_change_width( width );
        set_protocol( protocol_mode );
        theActiveView->fit_windows_to_screen();
        screen_garbaged = 1;
    }
}

void SystemExpressionRepresentationScreenWidth::fetch_value()
{
    exp_int = theActiveView->t_width;
}

void SystemExpressionRepresentationScreenLength::assign_value( ExpressionRepresentation *new_value )
{
    int length = new_value->asInt();

    if( length < 3 || length > MSCREENLENGTH )
        error( FormatString("terminal-length must be > 3 and <= %d") << MSCREENLENGTH );
    else
    {
        term_length = length;
        theActiveView->t_length = length;
        set_protocol( protocol_mode );
        theActiveView->fit_windows_to_screen();
        screen_garbaged = 1;
    }
}

void SystemExpressionRepresentationScreenLength::fetch_value(void)
{
    exp_int = theActiveView->t_length;
}

void SystemExpressionRepresentationDisplayBoolean::assign_value( ExpressionRepresentation *new_value )
{
    // use base class boolean check
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );

    cant_1line_opt = 1;
    cant_1win_opt = 1;

    theActiveView->t_change_attributes();
}

void SystemExpressionRepresentationGraphicRendition::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value( new_value->asString() );
    const unsigned char *p = value.data();
    int c;

    while( *p != '\0' )
        if( !((c = *p++) == ';' || (c >= '0' && c <= '9') ))
        {
            error( "graphic-rendition must be 0-9 and ;" );
            return;
        }

    SystemExpressionRepresentationString::assign_value( new_value );

    cant_1line_opt = 1;
    cant_1win_opt = 1;
    redo_modes = 1;
}

//
// If true and the terminal will support it
// then the screen will flash instead of
// feeping when an error occurs
//
SystemExpressionRepresentationIntBoolean visible_bell;
SystemExpressionRepresentationIntBoolean black_on_white;

// DJH common routine for a feep
void ding( void )
{
    if( visible_bell )
        theActiveView->t_flash();
    else
        theActiveView->t_beep();
}

// DLK routine to make the cursor sit for n/10 secs
int sit_for( void )
{
    return sit_for_inner( getnum( ": sit-for " ) );
}

int sit_for_inner(int num_chars)
{
    if( input_pending != 0 )
        return 0;

    if( term_is_terminal )
    {
        theActiveView->do_dsp();    // Make the screen correct
        theActiveView->t_insert_mode( 0 );
    }

    while( num_chars != 0 && input_pending == 0 )
    {
        wait_abit();

        num_chars--;
    }

    return 0;
}


//--------------------------------------------------------------------------------
//
//
//    EmacsView implementation
//
//
//--------------------------------------------------------------------------------

// 'setpos' positions the cursor at position(row,col ) in the virtual screen
void EmacsView::setpos(int row, int col)
{
    if( current_line >= 0 )
    {
        EmacsLinePtr p = t_desired_screen[ current_line ];
        int n = t_width - columns_left;

        if( p->line_length <= n )
        {
            if( columns_left > 0 )
                p->line_length = n;
            else
                p->line_length = t_width;
        }
    }

    if( t_desired_screen[ row ].isNull() )
        t_desired_screen[ row ].newLine();

    EmacsLinePtr p = t_desired_screen[ row ];
    p->invalidateHash();

    while( p->line_length + 1 < col )
    {
        p->line_body[ p->line_length ] = ' ';
        p->line_attr[ p->line_length ] = 0;
        p->line_length++;
    }

    p->line_length = col - 1;

    current_line = row;
    columns_left = t_width + 1 - col;
    text_cursor = &p->line_body[ col - 1 ];
    attr_cursor = &p->line_attr[ col - 1 ];
}

//
// 'clearline' positions the cursor at the beginning of the
//  indicated line and clears the line( in the image)
//
void EmacsView::clearline(int row)
{
    setpos( row, 1 );
    t_desired_screen[row]->line_length = 0;
}

//--------------------------------------------------------------------------------
//
//    update_screen
//
//--------------------------------------------------------------------------------
void EmacsView::update_screen( int slow_update )
{
    t_update_begin();

#if !MEMMAP
    check_for_input = 999;
#endif
    if( screen_garbaged )
    {
        t_reset();
        screen_garbaged = 0;
        for( int n=0; n<=t_length; n++ )
        {
            t_phys_screen[n].releaseLine();
        }
    }

    if( current_line >= 0
    && t_desired_screen[ current_line ]->line_length <= t_width - columns_left )
        t_desired_screen[ current_line ]->line_length =
            (columns_left > 0 ?
                t_width - columns_left
            :
                t_width);
    current_line = -1;

    if( t_il_ov == MISSINGFEATURE )
        slow_update = 0;

    if( slow_update )
    {
        int n;
        for( n=1; n<=t_length; n++ )
        {
            if( t_desired_screen[n].isNull() )
                t_desired_screen[n] = t_phys_screen[n];
        }

        int c = 0;
        n = t_length;
        while( n >= 1 && c <= 2 )
        {
            if( t_phys_screen[n] != t_desired_screen[n]
            && !t_phys_screen[n].isNull()
            && t_desired_screen[n]->lineHash() != t_phys_screen[n]->lineHash()  )
                c++;
            n--;
        }
        if( c <= 2 )
            slow_update = 0;
        else
        {
#if !MEMMAP
            check_for_input = 1;    // baud_rate / 2400;
#endif
            executeInsertDelete();
        }
    }

    if( !slow_update )    // fast update
    {
        int n;
        for( n=1; n<=t_length; n++ )
            if( !t_desired_screen[n].isNull() )
            {
                updateLine( t_phys_screen[n], t_desired_screen[n], n );
                t_phys_screen[n] = t_desired_screen[n];
                t_desired_screen[n].releaseLine();
            }
    }

    if( input_pending == 0 )
        t_topos( curs_y, curs_x );

    t_update_end();
}


//--------------------------------------------------------------------------------
//
//    updateLine
//
//--------------------------------------------------------------------------------

#if MEMMAP
void EmacsView::updateLine
    (
    EmacsLinePtr old_line,
    EmacsLinePtr new_line,
    int ln
    )
{
#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
    {
        const EmacsChar_t nul( 0 );

        const int dump_size = 30;
        const EmacsChar_t *old_line_start = &nul;
        int old_line_len = 0;
        if( !old_line.isNull() )
        {
            old_line_start = old_line->line_body;
            old_line_len = old_line->line_length;
        }
        const EmacsChar_t *new_line_start = &nul;
        int new_line_len = 0;
        if( !new_line.isNull() )
        {
            new_line_start = new_line->line_body;
            new_line_len = new_line->line_length;
        }

        _dbg_msg( FormatString( "        updateLine(   %3d:'%.*s'%*s,    %3d:'%.*s'%*s, %d )" )
            << old_line_len
                << std::min( old_line_len, dump_size ) << old_line_start
                    << std::max( dump_size-old_line_len, 0 ) << ""
            << new_line_len
                << std::min( new_line_len, dump_size ) << new_line_start
                    << std::max( dump_size-new_line_len, 0 ) << ""
            << ln
            );
    }
#endif
    t_update_line( old_line, new_line, ln );
}

void EmacsView::moveLine
    (
    int old_line_num,
    int new_line_num
    )
{
#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
    {
        const EmacsChar_t nul( 0 );
        EmacsLinePtr &old_line = t_phys_screen[ old_line_num ];
        EmacsLinePtr &new_line = t_desired_screen[ new_line_num ];

        const int dump_size = 30;
        const EmacsChar_t *old_line_start = &nul;
        int old_line_len = 0;
        if( !old_line.isNull() )
        {
            old_line_start = old_line->line_body;
            old_line_len = old_line->line_length;
        }
        const EmacsChar_t *new_line_start = &nul;
        int new_line_len = 0;
        if( !new_line.isNull() )
        {
            new_line_start = new_line->line_body;
            new_line_len = new_line->line_length;
        }

        _dbg_msg( FormatString( "        move line( %2d %3d:'%.*s'%*s, %2d %3d:'%.*s'%*s )" )
            << old_line_num
            << old_line_len
                << std::min( old_line_len, dump_size ) << old_line_start
                    << std::max( dump_size-old_line_len, 0 ) << ""
            << new_line_num
            << new_line_len
                << std::min( new_line_len, dump_size ) << new_line_start
                    << std::max( dump_size-new_line_len, 0 ) << ""
            );
    }
#endif

    t_move_line( old_line_num, new_line_num );
}

#else
void EmacsView::updateLine
    (
    EmacsLinePtr old_line,
    EmacsLinePtr new_line,
    int ln
    )
{
    t_update_line( old_line, new_line, ln );
    check_for_input--;
    if( check_for_input < 0
    && input_pending == 0 )
    {
        t_io_flush();
        check_for_input = 1;    // baud_rate / 2400;
    }
}
#endif

//--------------------------------------------------------------------------------
//
//    dbg_dump_screen
//
//--------------------------------------------------------------------------------
#ifdef DBG_CALC_INS_DEL
void EmacsView::dbg_dump_screen( const char *title )
{
    _dbg_msg( FormatString("Dump of t_phys_screen and t_desired_screen: %s") << title );
    for( int ln=1; ln<=t_length; ln++ )
    {
        const EmacsChar_t nul( 0 );
        const int dump_size = 30;
        const EmacsChar_t *old_line_start = &nul;
        int old_line_len = 0;
        int old_line_hash = 0;

        if( !t_phys_screen[ln].isNull() )
        {
            old_line_start = t_phys_screen[ln]->line_body;
            old_line_len = t_phys_screen[ln]->line_length;
            old_line_hash = t_phys_screen[ln]->lineHash();
        }

        const EmacsChar_t *new_line_start = &nul;
        int new_line_len = 0;
        int new_line_hash = 0;

        if( !t_desired_screen[ln].isNull() )
        {
            new_line_start = t_desired_screen[ln]->line_body;
            new_line_len = t_desired_screen[ln]->line_length;
            new_line_hash = t_desired_screen[ln]->lineHash();
        }

        _dbg_msg( FormatString( "   %2d [%3d(0x%8.8x):'%.*s'%*s, %3d(0x%8.8x):'%.*s'%*s]" )
            << ln
            << old_line_len << old_line_hash
                << std::min( old_line_len, dump_size ) << old_line_start
                    << std::max( dump_size-old_line_len, 0 ) << ""
            << new_line_len << new_line_hash
                << std::min( new_line_len, dump_size ) << new_line_start
                    << std::max( dump_size-new_line_len, 0 ) << ""
            );
    }
}
#endif
