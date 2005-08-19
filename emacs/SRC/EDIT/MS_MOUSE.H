/*
 *	ms_mouse.h
 */
#define	INT_mouse		0x33

#define	MOUSE_reset		0
#define	MOUSE_show_cursor	1
#define	MOUSE_hide_cursor	2
#define	MOUSE_get_status	3
#define	MOUSE_set_position	4
#define	MOUSE_get_press		5
#define	MOUSE_get_release	6
#define	MOUSE_set_horz_limit	7
#define	MOUSE_set_vert_limit	8
#define	MOUSE_set_grapic_cur	9
#define	MOUSE_set_text_cur	10
#define	MOUSE_read_motion	11
#define	MOUSE_set_user_subr	12
#define	MOUSE_pen_em_on		13
#define	MOUSE_pen_em_off	14
#define	MOUSE_set_ratio		15
#define	MOUSE_cond_off		16
#define	MOUSE_set_double_speed	19
#define	MOUSE_swap_int_subr	20

#define	MOUSE_EV_CURSOR_CHANGE	0x01
#define	MOUSE_EV_LEFT_PRESSED	0x02
#define	MOUSE_EV_LEFT_RELEASED	0x04
#define	MOUSE_EV_RIGHT_PRESSED	0x08
#define	MOUSE_EV_RIGHT_RELEASED	0x10

#define	MOUSE_BUTTON_LEFT	0x01
#define	MOUSE_BUTTON_RIGHT	0x02

/* routines to interface to mouse */
int  mouse_reset(void);
void  mouse_show(int );
int  mouse_status(int *,int *,int *);
int  mouse_press(int ,int *,int *);
int  mouse_release(int ,int *,int *);
void  mouse_set_text(unsigned int ,unsigned int );
void  mouse_set_limits(int ,int ,int ,int );
void  mouse_double_speed(int );

void far  mouse_event_install( int );
int far  mouse_get_next_event( int far *, int far *, int far *, int far * );
