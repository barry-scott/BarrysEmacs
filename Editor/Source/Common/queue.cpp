#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#if defined( _NT )
#   pragma warning( disable: 4201 )
#   include <windows.h>
#   pragma warning( default: 4201 )
#endif

QueueImplementation::QueueImplementation( bool is_header )
	: EmacsObject()
	{
	if( is_header )
		{
		// init the header
		_next = this;
		_prev = this;
		}
	else
		{
		// init the entry
		_next = NULL;
		_prev = NULL;
		}
	}

QueueImplementation::~QueueImplementation(void)
	{
	if( _next == NULL && _prev == NULL )
		return;

	if( _next == this && _prev == this )
		return;

	// still queue'd
//	emacs_assert(false);
	}

//
//	Init the QueueImplementation q as empty
//
void QueueImplementation::queue_init_header(void)
	{
	_next = this;
	_prev = this;
	}

void QueueImplementation::queue_init_entry(void)
	{
	_next = NULL;
	_prev = NULL;
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

void QueueImplementation::queue_lock(void)
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

void QueueImplementation::queue_unlock(void)
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
 *	Insert an element e into QueueImplementation q
 *	Return 1 if the QueueImplementation was empty
 */
int QueueImplementation::queue_insert( QueueImplementation *q )
	{
#if	DBG_QUEUE
	/*
	 * insist on the QueueImplementation having pointers in _next and _prev.
	 * If the header is NULL the QueueImplementation has not been initialised	
	 */
	if( q->_next == NULL || q->_prev == NULL )
		fatal_error( 400 );
	/*
	 * insist on the element have NULL _next and _prev.
	 * If they are pointers the element is on an other QueueImplementation
	 */
	if( _next != NULL || _prev != NULL )
		fatal_error( 401 );

	if( dbg_flags&DBG_QUEUE )
		q->queue_validate();
#endif

	queue_lock();

	_next = q->_next;
	_prev = q->_next->_prev;
	q->_next->_prev = this;
	q->_next = this;

	queue_unlock();

	return q->_next == q->_prev;
	}

/*
 *	remove this element from the QueueImplementation it is on
 *	and return that element
 */
QueueImplementation *QueueImplementation::queue_remove(void)
	{

	/* return NULL on already removed */
	if( _next == NULL || _next == this )
		return NULL;

#if	DBG_QUEUE
	if( dbg_flags&DBG_QUEUE )
		{
		queue_validate();
		}
#endif

	queue_lock();

	_prev->_next = _next;
	_next->_prev = _prev;

	queue_unlock();

	/* NULL element to catch bugs */
	_next = NULL;
	_prev = NULL;

	// return our self
	return this;
	}

bool QueueImplementation::queue_empty()
	{
#if	DBG_QUEUE
	/* check for a bad QueueImplementation */
	if( this == NULL || _next == NULL || _prev == NULL )
		fatal_error( 406 );

	if( dbg_flags&DBG_QUEUE )
		{
		queue_validate();
		}
#endif

	return _next == this;
	}

/*
 *	QueueImplementation validation is a tricky business that needs to
 *	tacked with care for the types of bugs that QueueImplementation
 *	system hit.
 *
 *	Double inserts will cause the QueueImplementation to have a loop
 *	in it. To detect this we use the technic of walking
 *	the QueueImplementation with two pointers. One fast the other slow.
 *	if they ever point to each other there is a loop.
 *	if the fast point reaches the starting QueueImplementation element
 *	there is no problem.
 *
 *	along the way each element is checked for sanity.
 */
#if	DBG_QUEUE
void QueueImplementation::queue_validate()
	{
	QueueImplementation *p1, *p2a, *p2;

	queue_lock();

	p1 = p2 = this;
	for(;;)
		{
		/* step p2 on twice */
		p2a = p2->_next;
		/* check linkages */
		if( p2a->_next->_prev != p2a )
			fatal_error( 407 );
		if( p2a->_prev->_next != p2a )
			fatal_error( 408 );
		p2 = p2a->_next;
		/* check linkages */
		if( p2->_next->_prev != p2 )
			fatal_error( 407 );
		if( p2->_prev->_next != p2 )
			fatal_error( 408 );

		/* Step P1 on once */
		p1 = p1->_next;
		if( p1 == this )
			break;	/* all done */
		/* check for loop */
		if( p1 == p2 )
			fatal_error( 409 );
		}
	
	queue_unlock();
	}
#endif

QueueHeaderImplementation::QueueHeaderImplementation()
	: QueueImplementation( true )
	{}

QueueHeaderImplementation::~QueueHeaderImplementation()
	{}

bool QueueHeaderImplementation::queueEntryIsHeader( const QueueImplementation *entry ) const
	{
	return (QueueImplementation *)entry == (QueueImplementation *)this;
	}

QueueIteratorImplementation::QueueIteratorImplementation( QueueHeaderImplementation &_header )
	: header( _header )
	, next_entry( header.queue_next() )
	, last_entry( NULL )
	{ }

QueueIteratorImplementation::~QueueIteratorImplementation()
	{ }

QueueImplementation *QueueIteratorImplementation::next()
	{
	if( !header.queueEntryIsHeader( next_entry ) )
		{
		last_entry = next_entry;
		next_entry = next_entry->queue_next();
		}
	else
		last_entry = NULL;

	return last_entry;
	}

QueueImplementation *QueueIteratorImplementation::value()
	{
	return last_entry;
	}
