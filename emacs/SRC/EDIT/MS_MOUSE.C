/*
 *	ms_mouse.c
 */
#include	<dos.h>
#include	<ms_mouse.h>
#include	<msdos.h>

/*
 *	Reset the mouse and return true if the mouse exists
 */
int mouse_reset(void)
	{
	union REGS regs;
	struct SREGS sregs;

	segread( &sregs );

	/*	find out if the mouse software is installed	*/
	regs.h.ah = DOS_get_vector;
	regs.h.al = INT_mouse;
	intdosx( &regs, &regs, &sregs );

	/*	check for an interrupt service routine	*/
	if( regs.x.ebx == 0 && sregs.es == 0 ) return 0;

	/*	do a mouse reset	*/
	regs.x.eax = MOUSE_reset;
	int386( INT_mouse, &regs, &regs );

	return (regs.x.eax != 0);
	}

/*
 *	either hide or show the mouse
 */
void mouse_show( int state )
	{
	union REGS regs;

	if( state )
		regs.x.eax = MOUSE_show_cursor;
	else
		regs.x.eax = MOUSE_hide_cursor;
	int386( INT_mouse, &regs, &regs );
	}

/*
 *	get the mouse status
 */
void mouse_status( int *button, int *x, int *y )
	{
	union REGS regs;

	regs.x.eax = MOUSE_get_status;
	int386( INT_mouse, &regs, &regs );

	if( regs.x.ebx != 0 )
		{
		*x = regs.x.ecx & 0xffff;
		*y = regs.x.edx & 0xffff;
		*button = regs.x.ebx & 0xffff;
		}
	else
		*button = 0;
	}
/*
 *	get the button press info
 */
int mouse_press( int button, int *x, int *y )
	{
	union REGS regs;

	regs.x.eax = MOUSE_get_press;
	regs.x.ebx = button;
	int386( INT_mouse, &regs, &regs );
	if( regs.x.ebx != 0 )
		{
		*x = regs.x.ecx&0xffff;
		*y = regs.x.edx&0xffff;
		return 1;
		}
	else
		return 0;
	}

/*
 *	get the button press info
 */
int mouse_release( int button, int *x, int *y )
	{
	union REGS regs;

	regs.x.eax = MOUSE_get_release;
	regs.x.ebx = button;
	int386( INT_mouse, &regs, &regs );
	if( regs.x.ebx != 0 )
		{
		*x = regs.x.ecx&0xffff;
		*y = regs.x.edx&0xffff;
		return 1;
		}
	else
		return 0;
	}

/*
 *	set the text cursor
 */
void mouse_set_text( unsigned int screen_mask, unsigned int cursor_mask )
	{
	union REGS regs;

	regs.x.eax = MOUSE_set_text_cur;
	regs.x.ebx = 0;
	regs.x.ecx = screen_mask;
	regs.x.edx = cursor_mask;
	int386( INT_mouse, &regs, &regs );
	}

void mouse_set_limits( int min_x, int max_x, int min_y, int max_y )
	{
	union REGS regs;

	regs.x.eax = MOUSE_set_horz_limit;
	regs.x.ebx = 0;
	regs.x.ecx = min_x;
	regs.x.edx = max_x;
	int386( INT_mouse, &regs, &regs );

	regs.x.eax = MOUSE_set_vert_limit;
	regs.x.ebx = 0;
	regs.x.ecx = min_y;
	regs.x.edx = max_y;
	int386( INT_mouse, &regs, &regs );
	}

void mouse_double_speed( int threshold )
	{
	union REGS regs;

	regs.x.eax = MOUSE_set_double_speed;
	regs.x.ebx = 0;
	regs.x.ecx = 0;
	regs.x.edx = threshold == 0 ? 10000 : threshold;
	int386( INT_mouse, &regs, &regs );
	}

/*
 *	The following routines, variables and structures where compiled
 *	into .ASM and hand crafted into the required shape. See mouse_ev.asm
 *	for the result.
 */

#ifdef	MAKING_ASM_SOURCE
struct mouse_event
	{
	int mask;	/* condition mask */
	int button;	/* Button press info */
	int x, y;	/* Associated position */
	};
static struct mouse_event mouse_events[32];
static int event_put, event_get;
extern far mouse_event_handler();

void far mouse_event_handler( unsigned eax, unsigned ebx, unsigned ecx, unsigned edx )
	{
	event_put++;
	event_put &= 31;

	mouse_events[ event_put ].mask = eax;
	mouse_events[ event_put ].button = ebx;
	mouse_events[ event_put ].x = ecx;
	mouse_events[ event_put ].y = edx;
	}

union REGS mouse_event_regs;

void mouse_event_install( int enable )
	{
	char *p = &mouse_event_handler;

	mouse_event_regs.x.eax = MOUSE_swap_int_subr;
	mouse_event_regs.x.ebx = FP_SEG( p );
	mouse_event_regs.x.ecx = enable;
	mouse_event_regs.x.edx = FP_OFF( p );

	int386( INT_mouse, &mouse_event_regs, &mouse_event_regs );	
	}


int mouse_get_next_event( int *mask, int *button, int *x, int *y )
	{
	if( event_get == event_put )
		return 0;

	event_put++;
	event_put &= 31;

	*mask	= mouse_events[ event_put ].mask;
	*button = mouse_events[ event_put ].button;
	*x	= mouse_events[ event_put ].x;
	*y	= mouse_events[ event_put ].y;

	return 1;
	}
#endif
