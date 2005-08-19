//
// 	Copyright(c) 1982-2001
//		Barry A. Scott
//
//	display_magic_square.cpp
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

void print_matrix( void );

//	1   2   3   4   ....	Each Mij represents the minimum cost of
//    +---+---+---+---+-----	rearranging the first i lines to map onto
//  1 |   |   |   |   |		the first j lines (the j direction
//    +---+---+---+---+-----	represents the desired contents of a line,
//  2 |   |  \| ^ |   |		i the current contents).  The algorithm
//    +---+---\-|-+---+-----	used is a dynamic programming one, where
//  3 |   | <-+Mij|   |		M[i,j] = min( M[i-1,j],
//    +---+---+---+---+-----		      M[i,j-1]+redraw cost for j,2
//  4 |   |   |   |   |			      M[i-1,j-1]+the cost of
//    +---+---+---+---+-----			converting line i to line j);
//  . |   |   |   |   |		Line i can be converted to line j by either
//  .				just drawing j, or if they match, by moving
//  .				line i to line j (with insert/delete line)
//
struct emacs_msquare
	{
	int msq_cost;
	int msq_fromi;
	int msq_fromj;
	};
static struct emacs_msquare msquare[MSCREENLENGTH + 1][MSCREENLENGTH + 1];

void EmacsView::calc_matrix( void )
	{
	int re_draw_cost;
	int idcost;
	int leftcost;
	int i;

	int cost = 0;
	int movecost = 0;
	for( i=0; i<=t_length; i++ )
		{
		struct emacs_msquare *p = &msquare[i][0];
//		M[i, i,  cost ] = 0;
		p[i].msq_cost = 0;
		msquare[0][i].msq_cost = cost + movecost;
//		M[i, 0,  msq_cost ] = movecost;
		p[0].msq_cost = movecost;
		msquare[0][i].msq_fromi = 0;
//		M[0, i,  msq_fromj ] = M[i, i,  msq_fromj ] = i - 1;
		msquare[0][i].msq_fromj = i - 1;
		p[i].msq_fromj = i - 1;
//		M[i, 0,  msq_fromi ] = M[i, i,  msq_fromi ] = i - 1;
		p[0].msq_fromi = i - 1;
		p[i].msq_fromi = i - 1;
//		M[i, 0,  msq_fromj ] = 0;
		p[0].msq_fromj = 0;
		movecost = movecost + t_il_mf *  (t_length - i) + t_il_ov;
		if( !t_desired_screen[ i + 1 ].isNull() )
			cost = cost + t_desired_screen[ i + 1 ]->line_drawcost;
		}

	int fidcost = t_il_mf * (t_length + 1) + t_il_ov;
	for( i=1; i<=t_length; i++ )
		{
		emacs_msquare *p = &msquare[i][0];
		fidcost = fidcost - t_il_mf;
		idcost = fidcost;
		for( int j=1; j<=t_length; j++ )
			{
			cost = !t_desired_screen[j].isNull() ? t_desired_screen[j]->line_drawcost : 0;
			re_draw_cost = cost;
			if( !t_phys_screen[i].isNull() 
			&& !t_desired_screen[j].isNull()
			&& t_phys_screen[i]->lineHash() == t_desired_screen[j]->lineHash() )
				cost = 0;
//			idcost = tt[ t_il_mf ]* ( WindowSize - i + 1 ) + tt[ t_il_ov ];
			p++;
//			p = &M[i, j];
//			movecost = M[i - 1, j,  msq_cost ] + (j == WindowSize ? 0 : idcost);
			movecost = p[ -MSCREENLENGTH - 1].msq_cost
				+ (j == t_length ? 0 : idcost);

//			leftcost = M[i, j - 1,  msq_cost ]
			leftcost = p[-1].msq_cost + (i == t_length ? 0 : idcost) + re_draw_cost;
			if( leftcost < movecost )
				{
				movecost = leftcost;
				p[0].msq_fromi = i;
				p[0].msq_fromj = j - 1;
				}
			else
				{
				p[0].msq_fromi = i - 1;		/* now using movecost for */
				p[0].msq_fromj = j;		/* the minumum cost. */
				}

//			cost += M[i - 1, j - 1,  msq_cost ];
			cost += p[ -MSCREENLENGTH - 2].msq_cost;
			if( cost < movecost )
				{
				movecost = cost;
				p[0].msq_fromi = i - 1;
				p[0].msq_fromj = j - 1;
				}
			p[0].msq_cost = movecost;
			}
		}

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		print_matrix();
#endif
	}


//--------------------------------------------------------------------------------
//
//	calc_insert_delete
//
//--------------------------------------------------------------------------------
#if 1
void EmacsView::calc_insert_delete( int i, int j, int /* inserts_desired */ )
	{
	while( j>0 )
		{
		emacs_msquare *p = &msquare[i][j];
		int ni = p->msq_fromi;
		int nj = p->msq_fromj;

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		{
		_dbg_msg( FormatString( "calc_insert_delete:                               at %di,%dj next %dni,%dnj" )
			<< i << j << ni << nj );
		}
#endif

		// is a scroll-down needed
		if( nj == j && j > 1 )
			{
			// remember the low edge
			int window_lower_edge = i;

			while( msquare[ni][nj].msq_fromj == nj )
				{
				ni = msquare[ni][nj].msq_fromi;
				}

			// figure out the upper edge
			int delta = i-ni;

			// update the lines that match in the scroll zone
			// and figure out the end of the zone
			while( msquare[ni][nj].msq_fromj == msquare[ni][nj].msq_fromi + delta )
				{
				int nni = msquare[ni][nj].msq_fromi;
				int nnj = msquare[ni][nj].msq_fromj;

				ni = nni;
				nj = nnj;
				}

			int window_upper_edge = ni+1;

			int line;
			for( line=window_lower_edge; line >= window_upper_edge+delta; line-- )
				appendUpdateLineCommand( line-delta >= 1 ? line-delta : 0, line );

			for( ; line >= window_upper_edge; line-- )
				appendUpdateLineCommand( 0, line );

			appendScrollDownCommand( window_upper_edge, window_lower_edge, delta );

			if( line == 0 )
				break;

			// continue from the line above the top of the scroll region
			i = window_upper_edge - 1;
			j = window_upper_edge - 1;

#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				{
				_dbg_msg( FormatString( "calc_insert_delete:                               at %di,%dj after scroll down" )
							<< i << j );
				}
#endif
			}

		// is a scroll-up needed
		else if( ni == i && i > 1 )
			{
			// remember the low edge
			int window_low_edge = i;

			// figure out the offset of the scroll-up
			while( msquare[ni][nj].msq_fromi == ni )
				{
				nj = msquare[ni][nj].msq_fromj;
				}
			// figure out the upper edge
			int delta = j-nj;

			// update the line exposed
			for( int line=window_low_edge; line > window_low_edge-delta; line-- )
				appendUpdateLineCommand( 0, line );

			// update the lines that match in the scroll zone
			// and figure out the end of the zone
			while( msquare[ni][nj].msq_fromi == msquare[ni][nj].msq_fromj + delta )
				{
				appendUpdateLineCommand( ni, nj );
				int nni = msquare[ni][nj].msq_fromi;
				int nnj = msquare[ni][nj].msq_fromj;

				ni = nni;
				nj = nnj;
				}
			int window_upper_edge = nj+1;

			// insert some lines
			appendScrollUpCommand( window_upper_edge, window_low_edge, delta );

			// continue from the line above the top of the scroll region
			i = window_upper_edge - 1;
			j = window_upper_edge - 1;
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				{
				_dbg_msg( FormatString( "calc_insert_delete:                               at %di,%dj after scroll up" )
							<< i << j );
				}
#endif
			}

		// update this line
		else
			{
			appendUpdateLineCommand( i, j );

			i = ni;
			j = nj;
			}
		}
	}

#else
//
// calculate and perform the optimal sequence of insertions/deletions
// given the matrix M from routine calc_matrix
//
void EmacsView::calc_insert_delete( int i, int j, int inserts_desired )
	{
#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		{
		indent_level++;
		_dbg_msg( FormatString( "%scalc_insert_delete( %d, %d, %d ) begin" )
			<< indent() << i << j << inserts_desired );
		}
#endif

	struct emacs_msquare *p = &msquare[i][j];
	if( i > 0 || j > 0 )
		{
		int ni = p->msq_fromi;
		int nj = p->msq_fromj;
		if( ni == i )
			{
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "%scalc_insert_delete: ni == i              ni=%d, nj=%d" )
					<< indent() << ni << nj );
#endif

			if( i != window_size )
				calc_insert_delete( ni, nj, inserts_desired + 1 );
			else
				calc_insert_delete( ni, nj, 0 );

			inserts_desired = 0;
			if( input_pending )
				{
				if( t_phys_screen[j] != t_desired_screen[j] )
					release_line( t_phys_screen[j] );
				t_phys_screen[j] = NULL;
				release_line( t_desired_screen[j] );
				t_desired_screen[j] = NULL;
				last_redisplay_paused = 1;
				}
			else
				{
				updateLine( NULL, t_desired_screen[j], j );
				if( t_phys_screen[j] != t_desired_screen[j] )
					release_line( t_phys_screen[j] );
				t_phys_screen[j] = t_desired_screen[j];
				t_desired_screen[j] = NULL;
				}
			}
		else
		if( nj == j )
			{
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "%scalc_insert_delete: nj == j              ni=%d, nj=%d" )
					<< indent() << ni << nj );
#endif

			if( j != window_size )
				{
				int dlc = 1;
				while( ni != 0 )
					{
					if( msquare[ni][nj].msq_fromj == nj )
						{
						dlc++;
						ni = msquare[ni][nj].msq_fromi;
						}
					else
						break;
					}

				appendScrollDownCommand( i - dlc, dlc );
				}
			calc_insert_delete( ni, nj, 0 );
			}
		else
			{
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "%scalc_insert_delete: ni != i && nj != j   ni=%d, nj=%d" )
					<< indent() << ni << nj );
#endif

			int done_early = 0;

			EmacsLine *old = t_phys_screen[i];
			if( old == t_desired_screen[i] )
				t_desired_screen[i] = NULL;
			t_phys_screen[i] = NULL;

			//
			// The following hack and all following lines
			// involving the variable 'DoneEarly' cause the
			// bottom line of the screen to be redisplayed
			// before any others if it has changed and it
			// would be redrawn in-place. This is purely
			// for Emacs, people using this package for
			// other things might want to lobotomize this
			// section.
			//
			if( i == t_length
			&& j == t_length
			&& old != t_phys_screen[j] )
				{
				done_early = 1;
				updateLine( old, t_desired_screen[j], j );
				}

			calc_insert_delete( ni, nj, 0 );

			if( input_pending && ! done_early )
				{
				if( t_phys_screen[j] != old )
					release_line( t_phys_screen[j] );
				if( t_desired_screen[j] != old
				&& t_desired_screen[j] != t_phys_screen[j] )
					release_line( t_desired_screen[j] );
				t_phys_screen[j] = old;
				t_desired_screen[j] = NULL;
				last_redisplay_paused = 1;
				}
			else
				{
				if( ! done_early )
					updateLine( old, t_desired_screen[j], j );
				if( t_phys_screen[j] != t_desired_screen[j] )
					release_line( t_phys_screen[j] );
				if( old != t_desired_screen[j]
				&& old != t_phys_screen[j] )
					release_line( old );
				t_phys_screen[j] = t_desired_screen[j];
				t_desired_screen[j] = NULL;
				}
			}
		}

	if( inserts_desired != 0 )
		{
		appendScrollUpCommand( j, inserts_desired );
		}

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		{
		_dbg_msg( FormatString( "%scalc_insert_delete( %d, %d, %d ) done" )
			<< indent() << i << j << inserts_desired );
		indent_level--;
		}
#endif
	}
#endif

/* Debugging routines -- called from sdb only */

/* print out the insert/delete cost matrix */
void print_matrix( void )
	{
	int i,j;
	struct emacs_msquare *p;

	_dbg_msg("print_matrix:");

	{
	EmacsString line("   ");
	for( j=0; j<=theActiveView->t_length; j++ )
		{
		EmacsString str( FormatString("         %2dj") << j );
		line.append( str );
		}
	_dbg_msg( line );
	}
	for( i=0; i<=theActiveView->t_length; i++ )
		{
		EmacsString line( FormatString("%2di ") << i );
		for( j=0; j<=theActiveView->t_length; j++ )
			{
			p = &msquare[ i][ j ];

			EmacsString str( FormatString("%4d[%2d,%2d]%c")
				<< p->msq_cost << p->msq_fromi << p->msq_fromj
				<< (( p->msq_fromi < i && p->msq_fromj < j ) ?  92
					: (( p->msq_fromi < i ) ?  '^'
					: (( p->msq_fromj < j ) ?  '<'
					: ' '))));
			line.append( str );
			}
		_dbg_msg( line );
		}
	_dbg_msg( "print_matrix done\n" );
	}

//--------------------------------------------------------------------------------
//
//	executeInsertDelete
//
//--------------------------------------------------------------------------------
void EmacsView::executeInsertDelete()
	{
#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		dbg_dump_screen( "start of executeInsertDelete" );

#endif

	num_screen_update_commands = 0;

	calc_matrix();
	calc_insert_delete( t_length, t_length, 0 );
	executeStoredScreenUpdateCommands();

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		dbg_dump_screen( "end of executeInsertDelete" );
#endif
	}

//--------------------------------------------------------------------------------
//
//	executeStoredScreenUpdateCommands
//
//--------------------------------------------------------------------------------
void EmacsView::executeStoredScreenUpdateCommands( void )
	{
	//for( int i=num_screen_update_commands-1; i>=0; i-- )
	for( int i=0; i < num_screen_update_commands; i++ )
		{
		screen_update_command_t &command = screen_update_commands[i];
		switch( command.idc_command )
			{
		case screen_update_command_t::idc_update_line:
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "execute: updateLine at %d from old line %d" )
					<< command.idc_new_line << command.idc_old_line );
#endif
			updateLine( t_phys_screen[command.idc_old_line], t_desired_screen[command.idc_new_line], command.idc_new_line );
			break;

		case screen_update_command_t::idc_scroll_up:
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "execute: scroll up %d lines between %d - %d" )
					<< command.idc_amount << command.idc_upper_edge << command.idc_lower_edge );
#endif
			t_window( command.idc_lower_edge );
			t_topos( command.idc_upper_edge, 1 );
			t_delete_lines( command.idc_amount );
			break;

		case screen_update_command_t::idc_scroll_down:
#if DBG_CALC_INS_DEL
			if( dbg_flags&DBG_CALC_INS_DEL )
				_dbg_msg( FormatString( "execute: scroll down %d lines between %d - %d" )
					<< command.idc_amount << command.idc_upper_edge << command.idc_lower_edge );
#endif
			t_window( command.idc_lower_edge );
			t_topos( command.idc_upper_edge, 1 );
			t_insert_lines( command.idc_amount );
			break;

		default:
			emacs_assert( "Bad insert delete command entry" );
			}
		}

	num_screen_update_commands = 0;

	for( int line=1; line < t_length; line++ )
		{
		t_phys_screen[line] = t_desired_screen[line];
		t_desired_screen[line].releaseLine();
		}
	}

//--------------------------------------------------------------------------------
//
//	appendUpdateLineCommand
//
//--------------------------------------------------------------------------------
void EmacsView::appendUpdateLineCommand( int old_line, int new_line )
	{
	emacs_assert( old_line >= 0 && old_line <= t_length );
	emacs_assert( new_line > 0  && new_line <= t_length );

	screen_update_command_t &command = screen_update_commands[num_screen_update_commands];
	num_screen_update_commands++;

	command.idc_command = screen_update_command_t::idc_update_line;
	command.idc_old_line = old_line;
	command.idc_new_line = new_line;

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		_dbg_msg( FormatString( "appendUpdateLineCommand at %d from old line %d" )
				<< command.idc_new_line << command.idc_old_line );
#endif
	}

//--------------------------------------------------------------------------------
//
//	appendScrollUpCommand
//
//--------------------------------------------------------------------------------
void EmacsView::appendScrollUpCommand( int upper_edge, int lower_edge, int amount )
	{
	emacs_assert( upper_edge > 0 && upper_edge < t_length );
	emacs_assert( lower_edge > 0 && lower_edge <= t_length );
	emacs_assert( upper_edge < lower_edge );	// remember top line is 1 bottom is t_length (24 say) 
	emacs_assert( amount > 0 && amount <= lower_edge-upper_edge );

	screen_update_command_t &command = screen_update_commands[num_screen_update_commands];
	num_screen_update_commands++;

	command.idc_command = screen_update_command_t::idc_scroll_up;
	command.idc_upper_edge = upper_edge;
	command.idc_lower_edge = lower_edge;
	command.idc_amount = amount;
#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		_dbg_msg( FormatString( "appendScrollUpCommand %d lines between %d - %d" )
				<< command.idc_amount << command.idc_upper_edge << command.idc_lower_edge );
#endif
	}

//--------------------------------------------------------------------------------
//
//	appendScrollDownCommand
//
//--------------------------------------------------------------------------------
void EmacsView::appendScrollDownCommand( int upper_edge, int lower_edge, int amount )
	{
	emacs_assert( upper_edge > 0 && upper_edge < t_length );
	emacs_assert( lower_edge > 0 && lower_edge <= t_length );

#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		{
		// the next assert is called wuth upper and lower the same...
		emacs_assert( upper_edge < lower_edge );	// remember top line is 1 bottom is t_length (24 say) 
		// this next asserts blows with an amount of 0 some time.
		// ignoring this assert does not seem to lead to any problems
		emacs_assert( amount > 0 && amount <= lower_edge-upper_edge );
		}
#endif

	if( upper_edge == lower_edge )
		return;

	screen_update_command_t &command = screen_update_commands[num_screen_update_commands];
	num_screen_update_commands++;

	command.idc_command = screen_update_command_t::idc_scroll_down;
	command.idc_upper_edge = upper_edge;
	command.idc_lower_edge = lower_edge;
	command.idc_amount = amount;
#if DBG_CALC_INS_DEL
	if( dbg_flags&DBG_CALC_INS_DEL )
		_dbg_msg( FormatString( "appendScrollDownCommand %d lines between %d - %d" )
					<< command.idc_amount << command.idc_upper_edge << command.idc_lower_edge );
#endif
	}

