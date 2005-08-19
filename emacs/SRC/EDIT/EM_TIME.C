/*
 *	Em_Time.c - emacs time services
 *	Copyright 1993 Barry A. Scott
 */
#include <emacs.h>

/*forward*/ static void timer_interrupt( void *param );
/*forward*/ void process_timer_interrupts( void );
/*forward*/ int schedule_procedure( void );


GLOBAL SAVRES struct queue timer_list_head;

/*
 *	Routine to field a Timer interrupt.
 *		( It just wakes up the scheduler )
 */
static void timer_interrupt( void *PNOTUSED(param) )
	{
	/*
	 *	Wake up the Scheduler
	 */
	interlock_inc( &input_pending );
	interlock_inc( &timer_interrupt_occurred );
	}

/*
 *	Routine, called by the scheduler, to process timer interrupts
 */
void process_timer_interrupts( void )
	{
	time_t current_time;		/* Absolute time */
	struct timer_entry *p;

	interlock_dec( &input_pending );
	interlock_dec( &timer_interrupt_occurred );

	/*
	 *	Scan the timer list for expired timer entries
	 */
	current_time = time( 0 );
	while( !queue_empty(&timer_list_head) )
		{
		p = (struct timer_entry *)timer_list_head.next;

		/*
		 *	The timer list is sorted by due-time, check_ if
		 *	we have processed all the due timer entries.
		 */
		if( p->tim_due_time > current_time )
			break;
		/*
		 *	This item is due: Unlink it from the list
		 */
		queue_remove( timer_list_head.next );

		/*
		 *	Call its procedure
		 */
		if( p->tim_procedure_to_call != NULL )
			execute_bound_saved_environment( p->tim_procedure_to_call );
		/*
		 * Now just throw away the timer entry. It has been used
		 */
		free( p );
		}

	/*
	 *	Reschedule the timer interrupt
	 */
	if( queue_empty( &timer_list_head ) )
		return;

	p = (struct timer_entry *)timer_list_head.next;
	time_add_request( (p->tim_due_time - current_time)*1000, 'S', timer_interrupt, NULL );
	}

/*
 *
 *	MLisp procedure to schedule the repeated execution of other MLisp
 *	procedures: ( schedule-procedure u_str("procedure-name") seconds )
 *
 */
int schedule_procedure( void )
	{
	struct bound_name *procedure;	/* Procedure to schedule */
	int reschedule_interval;			/* Rescheduling Interval */
	int i;
	struct queue *e;
	struct timer_entry *p = NULL;
	struct timer_entry *q;

	/*
	 *	Get the procedure to Reschedule
	 */
	i = getword( mac_names_ref, u_str(": procedure to schedule ") );
	if( i < 0 )
		return 0;
	procedure = mac_bodies[i];

	/*
	 *	Get the ReScheduling Interval
	 */
	reschedule_interval = getnum( u_str(": interval (in seconds) ") );
	if( reschedule_interval < 0 )
		{
		error( u_str("bad interval.") );
		return 0;
		}
	/*
	 *	search down the Timer List and see if this procedure is on it
	 */
	e = timer_list_head.next;
	while( e != &timer_list_head )
		{
		p = (struct timer_entry *)e;

		if( p->tim_procedure_to_call == procedure )
			{
			/*
			 *	Found it - Unlink it from the List
			 */
			queue_remove( e );
			break;
			}
		e = e->next;
		}

	/*
	 *	If the interval is 0, we are cancelling a timer request
	 */
	if( reschedule_interval == 0 )
		{
		/*
		 *	If it existed, free the data structure
		 */
		if( e != &timer_list_head )
			free( e );
		return( 0 );
		}

	/*
	 *	If the Data Structure does not exist, create it
	 */
	if( e == &timer_list_head )
		{
		p = malloc_struct( timer_entry );
		if( p == 0 )
			{
			error( u_str("out of memory.") );
			return 0;
			}
		p->tim_queue.next = NULL;
		p->tim_queue.prev = NULL;
		p->tim_procedure_to_call = procedure;
		}

	/*
	 *	Set the new Rescheduling interval
	 */
	p->tim_reschedule_interval = reschedule_interval;

	/*
	 *	Insert into the Timer List
	 */
	p->tim_due_time = time( 0 ) + p->tim_reschedule_interval;
	q = (struct timer_entry *)timer_list_head.next;

	while( &q->tim_queue != &timer_list_head
	&& q->tim_due_time <= p->tim_due_time )
		q = (struct timer_entry *)q->tim_queue.next;
	queue_insert( q->tim_queue.prev, &p->tim_queue );

	/*
	 *	If this entry is at the top of the Timer List, set the
	 *	new Alarm time.
	 */
	if( timer_list_head.next == &p->tim_queue )
		{
		time_remove_requests( 'S' );
		time_add_request( p->tim_reschedule_interval*1000, 'S', timer_interrupt, NULL );
		}

	return 0;
	}

void init_scheduled_timeout(void)
	{
	if( timer_list_head.next == NULL )
		queue_init( &timer_list_head );
	}

void restore_scheduled_timeout(void)
	{
	/*
	 *	Reschedule the timer interrupts
	 */
	timer_interrupt_occurred = 0;
	if( !queue_empty( &timer_list_head ) )
		time_add_request
		( 
		(((struct timer_entry *)timer_list_head.next)->tim_due_time - time(0))*1000,
		'S',
		timer_interrupt, NULL
		);
	}
