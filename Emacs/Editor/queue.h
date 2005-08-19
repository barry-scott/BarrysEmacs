/*
 *	queue structure
 *
 *	queue next and prev are either NULL or pointers to
 *	queue structures.
 */
#if !defined( _EMACS_QUEUE_INCLUDE )
#define _EMACS_QUEUE_INCLUDE

class QueueImplementation : public EmacsObject
	{
public:
	EMACS_OBJECT_FUNCTIONS( QueueImplementation )
	QueueImplementation( bool is_header );
	virtual ~QueueImplementation();

	void queue_init_header(void);
	void queue_init_entry(void);

	bool queue_empty(void);

#if	DBG_QUEUE
	void queue_validate(void);
#endif

	static void queue_lock( void );
	static void queue_unlock( void );
	int queue_insert( QueueImplementation *q );
	QueueImplementation *queue_remove(void);
	QueueImplementation *queue_next(void) { return _next; }
	QueueImplementation *queue_prev(void) { return _prev; }

private:
	QueueImplementation *_next;
	QueueImplementation *_prev;
	};

template <class T>
class QueueEntry : public QueueImplementation
	{
public:
	QueueEntry() : QueueImplementation( false ) {}
	virtual ~QueueEntry() {}

	int queueInsert( T *q ) { return queue_insert( (QueueImplementation *)q ); }
	T *queueRemove(void) { return (T *)queue_remove(); }
	T *queueNext(void) { return (T *)queue_next(); }
	T *queuePrev(void) { return (T *)queue_prev(); }
	};

class QueueHeaderImplementation : public QueueImplementation
	{
public:
	QueueHeaderImplementation();
	virtual ~QueueHeaderImplementation();

	// true is the entry is this header
	bool queueEntryIsHeader( const QueueImplementation *entry ) const;
	};

template <class T>
class QueueHeader : public QueueHeaderImplementation
	{
public:
	QueueHeader() : QueueHeaderImplementation() {}
	virtual ~QueueHeader() {}

	int queueInsert( T *q ) { return queue_insert( q ); }
	// remove the first element
	T *queueRemoveFirst(void) { return (T *)queue_next()->queue_remove(); }
	// remove the last element
	T *queueRemoveLast(void) { return (T *)queue_prev()->queue_remove(); }
	// insert at head of queue
	void queueInsertAtHead( T *entry ) { entry->queue_insert( queue_next() ); }
	// insert at tail of queue
	void queueInsertAtTail( T *entry ) { entry->queue_insert( queue_prev() ); }


	bool queueEmpty(void) { return queue_empty(); }

	T *queueFirst(void) { return (T *)queue_next(); }
	T *queueLast(void) { return (T *)queue_prev(); }

	// true is the entry is this header
	bool queueEntryIsHeader( const T *entry ) const
		{
		return QueueHeaderImplementation::queueEntryIsHeader( entry );
		}
	};


class QueueIteratorImplementation
	{
public:
	QueueIteratorImplementation( QueueHeaderImplementation &_header );
	virtual ~QueueIteratorImplementation();

	//  move iterator on to the next entry and return a pointer to it
	QueueImplementation *next();
	QueueImplementation *value();
private:
	QueueHeaderImplementation &header;
	QueueImplementation *next_entry;
	QueueImplementation *last_entry;
	};

template <class T>
class QueueIterator : public QueueIteratorImplementation
	{
public:
	QueueIterator( QueueHeader<T> &_header )
		: QueueIteratorImplementation( _header )
		{ }
	virtual ~QueueIterator()
		{ }

	//  move iterator on to the next entry and return a pointer to it
	T *next() { return (T *)QueueIteratorImplementation::next(); }
	T *value() { return (T *)QueueIteratorImplementation::value(); }
	};
#endif
