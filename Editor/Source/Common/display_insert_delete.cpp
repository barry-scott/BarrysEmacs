//
//     Copyright(c) 1982-2001
//        Barry A. Scott
//
//    display_insert_delete.cpp
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if DBG_CALC_INS_DEL && 0
extern void emacs_sleep( int );
static void debugSleep()
{
    if( dbg_flags&DBG_CALC_INS_DEL )
        emacs_sleep( 200 );
}
#else
static void debugSleep() {}
#endif
//--------------------------------------------------------------------------------
//
//    executeInsertDelete
//
//--------------------------------------------------------------------------------
void EmacsView::executeInsertDelete()
{
#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
        dbg_dump_screen( "start of executeInsertDelete" );

#endif

    //
    // find the old lines that can be moved to the new line positions
    //

    // record which old line is updated to the new line
    int update_line_from[ MSCREENLENGTH + 1 ];

    for( int i=0; i<int(sizeof(update_line_from)/sizeof(update_line_from[0])); i++ )
        update_line_from[i] = 0;

    // find line that have not moved that match
    for( int line_num=1; line_num <= t_length; line_num++ )
    {
        if( !t_desired_screen[line_num].isNull()
        && !t_phys_screen[ line_num ].isNull()
        && t_phys_screen[ line_num ]->lineHash() == t_desired_screen[ line_num ]->lineHash() )
        {
            // record which line we update from
            update_line_from[ line_num ] = line_num;
        }
    }

    // start scanning the old lines for the first line
    int old_line_num_start_point = 1;
    for( int new_line_num=1; new_line_num <= t_length; new_line_num++ )
    {
        if( update_line_from[ new_line_num ] != 0 )
        {
            // we have already used up the old lines up to here
            old_line_num_start_point = new_line_num + 1;
        }
        else
        {
            int new_line_hash = 0;
            if( !t_desired_screen[new_line_num].isNull() )
                new_line_hash = t_desired_screen[new_line_num]->lineHash();

            // for each new line find a matching old line
            for( int old_line_num = old_line_num_start_point; old_line_num <= t_length; old_line_num++ )
            {
                if( update_line_from[ old_line_num ] != 0 )
                    break;

                // assume a match if the hash is the same
                if( !t_phys_screen[ old_line_num ].isNull()
                && t_phys_screen[ old_line_num ]->lineHash() == new_line_hash )
                {
                    // start next scan from the next line
                    old_line_num_start_point = old_line_num + 1;
                    // record which line we update from
                    update_line_from[ new_line_num ] = old_line_num;

                    break;    // stop look now a match has been found
                }
            }
        }

#if DBG_CALC_INS_DEL
        if( dbg_flags&DBG_CALC_INS_DEL )
        {
            _dbg_msg( FormatString( "[from old %2d to new %2d]" ) << update_line_from[ new_line_num ] << new_line_num );
        }
#endif
    }

    //
    // update the lines on the screen
    //

#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
        _dbg_msg( "executeInsertDelete: update pass 1" );
#endif
    // find the lines that need moving down out of the way of an over write
    // scan from bottom to top
    for( int new_line_num=t_length; new_line_num >= 1 ; new_line_num-- )
    {
        int old_line_num = update_line_from[ new_line_num ];

        // move old line into position
        if( old_line_num != 0 && old_line_num < new_line_num )
        {
            debugSleep();

            moveLine( old_line_num, new_line_num );
            updateLine( t_phys_screen[ old_line_num ], t_desired_screen[ new_line_num ], new_line_num );
        }
    }
#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
        _dbg_msg( "executeInsertDelete: update pass 2" );
#endif

    // update all the rest of the lines on the screen not covered by the move own pass
    // scan from top to bottom
    EmacsLinePtr empty;

    for( int new_line_num=1; new_line_num <= t_length; new_line_num++ )
    {
        int old_line_num = update_line_from[ new_line_num ];

        // no old line, just write the new one out
        if( old_line_num == 0 )
        {
            debugSleep();
            updateLine( empty, t_desired_screen[new_line_num], new_line_num );
        }
        // update changed new from old
        else if( new_line_num == old_line_num )
        {
            debugSleep();
            updateLine( t_phys_screen[old_line_num], t_desired_screen[new_line_num], new_line_num );
        }
        // move old line into position
        else if( old_line_num > new_line_num )
        {
            debugSleep();

            moveLine( old_line_num, new_line_num );
            updateLine( t_phys_screen[ old_line_num ], t_desired_screen[ new_line_num ], new_line_num );
        }

        // make the physical screen map contain the desired we just updated to
        t_phys_screen[new_line_num] = t_desired_screen[new_line_num];
        t_desired_screen[new_line_num].releaseLine();
    }

#if DBG_CALC_INS_DEL
    if( dbg_flags&DBG_CALC_INS_DEL )
        dbg_dump_screen( "end of executeInsertDelete" );
#endif
}
