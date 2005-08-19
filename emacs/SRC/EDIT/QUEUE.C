#include <emacs.h>

#if defined( _NT )
#   pragma warning( disable: 4201 )
#   include <windows.h>
#   pragma warning( default: 4201 )
#endif

/*
 *	Init the queue q as empty
 */
void queue_init( struct queue *q )
	{
#if	DBG_QUEUE
	/* insist on queue headers being NULL */
	if( q->next != NULL || q->prev != NULL )
		fatal_error( 405 );
#endif

	q->next = q;
	q->prev = q;
	}

#ifdef _NT
static CRITICAL_SECTION global_queue_lock;
#endif
void init_queue_system()
	{
#ifdef _NT
	InitializeCriticalSection( &global_queue_lock );
#endif
	return;
	}


#ifdef VMS
volatile int queue_lock_count = -1;
#endif

void queue_lock(void)
	{
#if defined( VMS )
	int status = interlock_inc( &queue_lock_count );
	if( status == 0 )
		/* move from -1 to 0 only on the first lock */
		sys$setast( 0 );
#elif defined( _NT )
	EnterCriticalSection( &global_queue_lock );
#endif
	}

void queue_unlock(void)
	{
#if defined( VMS )
	int status = interlock_dec( &queue_lock_count );
	if( status < 0 )
		/* move from 0 to -1 only on last unlock */
		sys$setast( 1 );

	/*
	 *	Check for too many unlocks
	 *	note that on VAX the queue_lock_count is
	 *	treated as a short int.
	 */
	if( (int)((short)queue_lock_count) < -1 )
		fatal_error( 499 );
#elif defined( _NT )
	LeaveCriticalSection( &global_queue_lock );
#endif
	}

/*
 *	Insert an element e into queue q
 *	Return 1 if the queue was empty
 */
int queue_insert( struct queue *q, struct queue *e )
	{
#if	DBG_QUEUE
	/*
	 * insist on the queue having pointers in next and prev.
	 * If the header is NULL the queue has not been initialised	
	 */
	if( q->next == NULL || q->prev == NULL )
		fatal_error( 400 );
	/*
	 * insist on the element have NULL next and prev.
	 * If they are pointers the element is on an other queue
	 */
	if( e->next != NULL || e->prev != NULL )
		fatal_error( 401 );

	if( dbg_flags&DBG_QUEUE )
		{
		queue_validate( q );
		}
#endif

	queue_lock();

	e->next = q->next;
	e->prev = q->next->prev;
	q->next->prev = e;
	q->next = e;

	queue_unlock();

	return q->next == q->prev;
	}

/*
 *	remove element e from the queue it is on
 *	and return that element
 */
struct queue *queue_remove( struct queue *e )
	{
#if	DBG_QUEUE
	/* Check for a NULL queue - no init done or already removed */
	if( e == NULL || e->next == NULL || e->prev == NULL )
		fatal_error( 402 );

	if( dbg_flags&DBG_QUEUE )
		{
		queue_validate( e );
		}
#endif

	/* return NULL on queue empty */
	if( e->next == e )
		return NULL;

	queue_lock();

	e->prev->next = e->next;
	e->next->prev = e->prev;

	queue_unlock();

	/* NULL element to catch bugs */
	e->next = NULL;
	e->prev = NULL;

	/* return the element e */
	return e;
	}

int queue_empty( struct queue *q )
	{
#if	DBG_QUEUE
	/* check for a bad queue */
	if( q == NULL || q->next == NULL || q->prev == NULL )
		fatal_error( 406 );

	if( dbg_flags&DBG_QUEUE )
		{
		queue_validate( q );
		}
#endif

	return q->next == q;
	}

/*
 *	queue validation is a tricky business that needs to
 *	tacked with care for the types of bugs that queue
 *	system hit.
 *
 *	Double inserts will cause the queue to have a loop
 *	in it. To detect this we use the technic of walking
 *	the queue with two pointers. One fast the other slow.
 *	if they ever point to each other there is a loop.
 *	if the fast point reaches the starting queue element
 *	there is no problem.
 *
 *	along the way each element is checked for sanity.
 */
#if	DBG_QUEUE
void queue_validate( struct queue *q )
	{
	struct queue *p1, *p2a, *p2;

	queue_lock();

	p1 = p2 = q;
	for(;;)
		{
		/* step p2 on twice */
		p2a = p2->next;
		/* check linkages */
		if( p2a->next->prev != p2a )
			fatal_error( 407 );
		if( p2a->prev->next != p2a )
			fatal_error( 408 );
		p2 = p2a->next;
		/* check linkages */
		if( p2->next->prev != p2 )
			fatal_error( 407 );
		if( p2->prev->next != p2 )
			fatal_error( 408 );

		/* Step P1 on once */
		p1 = p1->next;
		if( p1 == q )
			break;	/* all done */
		/* check for loop */
		if( p1 == p2 )
			fatal_error( 409 );
		}
	
	queue_unlock();
	}
#endif
