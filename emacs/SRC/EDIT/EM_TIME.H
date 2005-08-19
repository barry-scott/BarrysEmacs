/*
 *	em_timer.h
 *	emacs V6.0
 *	Author: Barry A. Scott
 *	Copyright (c) 1993
 */
struct time_request
	{
	struct queue header;
	int id;				/* id used to find elements to remove */
	time_t delta;			/* interval in milli seconds to wait */
	void (*rtn)( void *param );	/* routine to call on time out */
	void *param;			/* its parameter */
	};

/* Timer queue structures */
struct timer_entry
	{					/* Timer List structure */
	struct queue tim_queue;
	struct bound_name *tim_procedure_to_call; /* Procedure */
	time_t tim_due_time;			/* time to call */
	time_t tim_reschedule_interval;		/* Delta seconds */
	};

/* routines to be used by platform independent code */
extern void time_add_request( time_t when, int id, void (*rtn)(void *param), void *param );
extern void time_remove_requests( int id );

/* routines that support the timer sub system */
extern void time_schedule_timeout( void (*time_handle_timeout)(void), int delta );
extern void time_cancel_timeout(void);
