//    Copyright (c) 1982-2010
//        Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

static bool shift_state( false );

void gui_input_shift_state( bool shift )
{
    shift_state = shift;
#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("Shift is %s") << (shift ? "Down" : "Up") );
#endif
}


void gui_set_dot( int n )
{
#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    _dbg_msg( FormatString("GUI_set_dot in %s to %d from %d") << bf_cur->b_buf_name << n << dot );
#endif
    // force shift_state to on
    Save<bool> saved_shift_state( &shift_state );
    shift_state = true;

    // set dot knowing that the region will
    // be set if nessesary
    set_dot( n );
}

void gui_input_mode_set_dot( int n )
{
    // do nothing if set_dot set dot to 0
    if( dot == 0 )
        return;
    // do nothing if dot is not moving
    if( dot == n )
        return;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("Setting dot in %s to %d from %d") << bf_cur->b_buf_name << n << dot );
#endif

    if( shift_state && !bf_cur->b_mark.isSet() )
    {
        bf_cur->set_mark( dot, 0, true );
        cant_1line_opt = 1;
#if DBG_EXEC
        if( dbg_flags&DBG_EXEC )
            _dbg_msg( FormatString("   set_mark( %s, %d )") << bf_cur->b_buf_name << dot );
#endif
    }
    else if( !shift_state && bf_cur->b_mark.isSet() && bf_cur->b_gui_input_mode_set_mark )
    {
        bf_cur->unset_mark();
#if DBG_EXEC
        if( dbg_flags&DBG_EXEC )
            _dbg_msg( "   unset_mark()" );
#endif
    }
}


void gui_input_mode_before_insert()
{
#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("Before insert in %s dot is %d") << bf_cur->b_buf_name << dot );
#endif
    if( bf_cur->b_mark.isSet() && bf_cur->b_gui_input_mode_set_mark )
    {
        erase_region();
#if DBG_EXEC
        if( dbg_flags&DBG_EXEC )
            _dbg_msg( "   erase_region()" );
#endif
    }
}


bool gui_input_mode_before_delete()
{
#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("Before delete in %s dot is %d") << bf_cur->b_buf_name << dot );
#endif
    bool result = false;

    if( bf_cur->b_mark.isSet()            // have a mark
    && bf_cur->b_gui_input_mode_set_mark )        // owned by GUI input
    {
        if( (bf_cur->b_mark.to_mark() - dot) != 0 )    // with a region that is not empty
        {
            erase_region();
#if DBG_EXEC
            if( dbg_flags&DBG_EXEC )
                _dbg_msg( "   erase_region()" );
#endif
            result = true;    // we did the delete
        }

        // force the mark to be unset
        bf_cur->unset_mark();
    }

    // always clear the shift state when deleting
    shift_state = false;

    return result;
}
