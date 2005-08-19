/*
 *	queue structure
 *
 *	queue next and prev are either NULL or pointers to
 *	queue structures.
 */
#if !defined( _EMACS_QUEUE_INCLUDE )
#define _EMACS_QUEUE_INCLUDE

struct queue
	{
	struct queue *next;
	struct queue *prev;
	};

extern void queue_lock( void );
extern void queue_unlock( void );
extern void queue_init( struct queue *q );
extern int queue_insert( struct queue *q, struct queue *e );
extern struct queue *queue_remove( struct queue *e );
extern int queue_empty( struct queue *q );
#if	DBG_QUEUE
extern void queue_validate( struct queue *q );
#endif
#define queue_remove_struct( s, e ) ((struct s *)queue_remove( e ))
#endif
