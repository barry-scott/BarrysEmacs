/*
 *	timer.c
 *	emacs V6.0
 *	Author: Barry A. Scott
 *	Copyright (c) 1993
 */
#include <emacs.h>

static void time_handle_timeout( void );

#define TIMER_INTERVAL 100

struct queue timer_queue;
static int time_since_last_timeout;

void init_timer(void)
	{
	queue_init( &timer_queue );
	}

void restore_timer(void)
	{
	if( !queue_empty( &timer_queue ) )
		time_schedule_timeout( time_handle_timeout, TIMER_INTERVAL );
	}

static void time_handle_timeout( void )
	{
	struct time_request *cur;

	time_since_last_timeout += TIMER_INTERVAL;

#if DBG_TIME
	if( dbg_flags&DBG_TIME )
		printf( "time_handle_timeout: time_since_last_timeout = %d\n", time_since_last_timeout );
#endif

	queue_lock();

	/* walk the entire queue */
	cur = (struct time_request *)timer_queue.next;
	while( &cur->header != &timer_queue )
		{
		/* find the next here as cur may be deallocated */
		struct time_request *next = (struct time_request *)cur->header.next;

#if DBG_TIME
		if( dbg_flags&DBG_TIME )
			printf( "time_handle_timeout: checking id %c delta %d\n", cur->id, cur->delta );
#endif
		/* remove cur if it matches the ID */
		if( cur->delta <= time_since_last_timeout )
			{
			/* if this is not the last element then
			 * update the delta in next */
			if( &next->header != &timer_queue )
				next->delta += cur->delta - time_since_last_timeout;
			time_since_last_timeout = 0;

#if DBG_TIME
			if( dbg_flags&DBG_TIME )
				printf( "time_handle_timeout: calling %x( %x ) id %c\n",
					cur->rtn, cur->param, cur->id );
#endif
			/* call the timeout routine */
			cur->rtn( cur->param );
			/* free the block */
			free( queue_remove( &cur->header ) );
			}
		else
			break;
		/* step on to the next element */
		cur = next;
		}	

	queue_unlock();

#if DBG_TIME
	if( dbg_flags&DBG_TIME )
		printf( "time_handle_timeout: queue processed\n" );
#endif

	/* only ask for timeout if something is in the queue */
	if( !queue_empty( &timer_queue ) )
		{
#if DBG_TIME
		if( dbg_flags&DBG_TIME )
			printf( "time_handle_timeout: queue not empty schedule timeout\n" );
#endif
		time_schedule_timeout( time_handle_timeout, TIMER_INTERVAL );
		}
	}

void time_add_request( time_t delta, int id, void (*rtn)(void *param), void *param )
	{
	struct time_request *req = malloc_struct( time_request );
	struct time_request *cur;
	time_t time;

	if( req == NULL )
		return;

#if DBG_TIME
	if( dbg_flags&DBG_TIME )
		printf( "time_add_request: after %d call %x( %x ) id %c\n",
			delta, rtn, param, id );
#endif

	req->header.next = NULL;
	req->header.prev = NULL;
	req->id = id;
 	req->rtn = rtn;
	req->param = param;

	queue_lock();

	time = 0;
	cur = (struct time_request *)timer_queue.next;
	while( &cur->header != &timer_queue )
		{
		if( delta < time+cur->delta )
			break;
		time += cur->delta;
		cur = (struct time_request *)cur->header.next;
		}

	req->delta = delta - time;

	/* if this is the first element in the queue ask for a timeout */
	if( queue_empty( &timer_queue ) )
		{
#if DBG_TIME
		if( dbg_flags&DBG_TIME )
			printf( "time_add_request: queue was empty schedule timeout\n" );
#endif
		time_since_last_timeout = 0;
		time_schedule_timeout( time_handle_timeout, TIMER_INTERVAL );
		}

	if( &cur->header != &timer_queue )
		{
		/* the next entries will be called at req->delta + cur->delta
		 * so adjust cur to take req into account */
		cur->delta -= delta;
		}

	queue_insert( cur->header.next, &req->header );

	queue_unlock();
	}

void time_remove_requests( int id )
	{
	struct time_request *cur;

	queue_lock();

#if DBG_TIME
	if( dbg_flags&DBG_TIME )
		printf( "time_remove_requests: id %c\n", id );
#endif
	/* walk the entire queue */
	cur = (struct time_request *)timer_queue.next;
	while( &cur->header != &timer_queue )
		{
		/* find the next here as cur may be deallocated */
		struct time_request *next = (struct time_request *)cur->header.next;

		/* remove cur if it matches the ID */
		if( cur->id == id )
			{
#if DBG_TIME
			if( dbg_flags&DBG_TIME )
				printf( "time_remove_requests: removing %x( %x ) id %c\n",
					cur->rtn, cur->param, id );
#endif
			/* if this is not the last element then
			 * update the delta in next */
			if( &next->header != &timer_queue )
				next->delta += cur->delta;
			/* free the block */
			free( queue_remove( &cur->header ) );
			}
		/* step on to the next element */
		cur = next;
		}

	queue_unlock();

	/* if the queue is now empty cancel the timeout */
	if( queue_empty( &timer_queue ) )
		{
#if DBG_TIME
		if( dbg_flags&DBG_TIME )
			printf( "time_remove_requests: queue empty cancel timeout\n" );
#endif
		time_cancel_timeout();
		}
	}
