//
//
//    mem_man.cpp
//    Emacs V7.0
//    Copyright 1995 Barry A. Scott
//
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );
#include <mem_man.h>

#include <new>
#include <iostream>

//
//    functions
//
static void queue_init( struct queue *q );
static int queue_insert( struct queue *q, struct queue *e );
static struct queue *queue_remove( struct queue *e );
#if    DBG_QUEUE
static void queue_validate( struct queue *q );
#endif
#if    DBG_ALLOC_CHECK
void emacs_heap_check_entry( struct heap_entry *h );
#endif

#define DBG_MEM_BREAK 1
#if    DBG_MEM_BREAK
//
//    debug hooks
//
int debug_mem_sequence_number;    // sequence numbver of object to break on
void *debug_mem_address;    // address of object to break on
void debug_mem_break()
{
    return;
}
#endif

//
//    variables
//
int allocation_sequence_number = 0;
long int bytes_allocated;
SystemExpressionRepresentationIntReadOnly is_restored;

EmacsString default_environment_file;

struct queue all_emacs_memory;

#if DBG_ALLOC_CHECK
unsigned char guard_pattern[GUARD_SIZE] = { 'G','G','G','G','G','G','G','G','G','G','G','G','G','G','G','G' };
#endif

#ifdef LOOK_ASIDE
struct queue look_aside_lists[look_aside_max/look_aside_granularity];
int look_aside_max_queue_length[look_aside_max/look_aside_granularity];
int look_aside_cur_queue_length[look_aside_max/look_aside_granularity];
#endif
#ifdef SAVE_ENVIRONMENT
static struct heap_entry *last_heap_entry;
#endif

//
//
//    EmacsObject implementation
//
//
int EmacsObject::NextObjectNumber(1);

EmacsObject::EmacsObject()
    : ObjectNumber( NextObjectNumber++ )
{
#ifdef SAVE_ENVIRONMENT
    if( last_heap_entry != NULL
    && last_heap_entry->user_type == malloc_type_emacs_object
    && (unsigned char *)this >= &last_heap_entry->user_data[0]
    && (unsigned char *)this < &last_heap_entry->user_data[last_heap_entry->user_size] )
    {
        last_heap_entry->theObject = this;
        last_heap_entry = NULL;
    }
#endif
}

EmacsObject::~EmacsObject()
{ }

void *EmacsObject::operator new(size_t size)
{
    return emacs_malloc
        (
        size
        , malloc_type_emacs_object
#if DBG_ALLOC_CHECK
        , THIS_FILE, __LINE__
#endif
        );
}

#ifdef __has_array_new__
void *EmacsObject::operator new [](size_t )
{
    // must not use this interface as it cannot tag the block
    // as required for save env to work
    emacs_assert( false );
    throw std::bad_alloc();
}

#if DBG_ALLOC_CHECK
// allocate an vector of expressions
void *Expression::operator new [](size_t size, const char *fileName, int lineNumber)
{
    return emacs_malloc
        (
        size
        , malloc_type_emacs_object_Expression_Vector
        , fileName, lineNumber
        );
}
#else
// allocate an vector of expressions
void *Expression::operator new [](size_t size)
{
    return emacs_malloc
        (
        size
        , malloc_type_emacs_object_Expression_Vector
        );
}
#endif

void EmacsObject::operator delete [](void *p)
{
    emacs_free( p );
}
#endif

void EmacsObject::operator delete(void *p)
{
    emacs_free( p );
}

#if DBG_ALLOC_CHECK
void *EmacsObject::operator new(size_t size, const char *fileName, int lineNumber)
{
    return emacs_malloc
        (
        size
        , malloc_type_emacs_object
        , fileName, lineNumber
        );
}

#ifdef __has_array_new__
void *EmacsObject::operator new [](size_t size, const char *fileName, int lineNumber)
{
    return emacs_malloc
        (
        size
        , malloc_type_emacs_object
        , fileName, lineNumber
        );
}
#endif
#endif

#if DBG_ALLOC_CHECK
bool EmacsObject::isHeapObject() const
{
    // if this is a heap block then here is where it starts
    heap_entry *h = (struct heap_entry *)((char *)this-offsetof(struct heap_entry,user_data));

    // these tests should be all that is required
    if( h->user_type == malloc_type_emacs_object
#ifdef SAVE_ENVIRONMENT
    &&  h->theObject == this
#endif
    &&  memcmp( guard_pattern, h->front_guard, GUARD_SIZE ) == 0
    &&  memcmp( guard_pattern, &h->user_data[h->user_size], GUARD_SIZE ) == 0 )
        return true;

    return false;
}

void EmacsObject::objectAllocatedBy( const char *&file, int &line, const EmacsObject *&object ) const
{
    line = 0;
    file = "";
    object = NULL;

    struct queue *entry = all_emacs_memory.next;

    while( (void *)entry != (void *)&all_emacs_memory )
    {
        struct heap_entry *h = (struct heap_entry *)entry;

        if( (void *)this >= (void *)&h->user_data[ GUARD_SIZE ]
        && (void *)this < (void *)&h->user_data[ GUARD_SIZE + h->user_size ] )
        {
            file = h->fileName;
            line = h->lineNumber;
#ifdef SAVE_ENVIRONMENT
            object = h->theObject;
#endif
            break;
        }

        entry = entry->next;
    }
}
#endif

void init_memory( void )
{
    //
    // guard size must ensure that the offsetof(struct heap_entry, user_data)
    // is a multiple of sizeof(double) to ensure that double variables are
    // aligned correctly
    //
    if( offsetof( struct heap_entry, user_data )%sizeof(double) != 0 )
    {
        std::cerr << "offsetof( struct heap_entry, user_data ) " << offsetof( struct heap_entry, user_data ) << std::endl;
        std::cerr << "sizeof( double ) " << sizeof( double ) << std::endl;

        emacs_assert( offsetof( struct heap_entry, user_data )%sizeof(double) == 0 );
    }

    if( all_emacs_memory.next == NULL )
    {
        init_queue_system();
        queue_init( &all_emacs_memory );

#ifdef LOOK_ASIDE
        for( int i=0; i<(look_aside_max/look_aside_granularity); i++ )
            queue_init( &look_aside_lists[i] );
#endif
    }
    return;
}

#if DBG_ALLOC_CHECK
void emacs_check_malloc_block( void *p )
{
    struct heap_entry *h;

    h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

    emacs_heap_check_entry( h );
}
#else
void emacs_check_malloc_block( void * )
{ }
#endif

#if DBG_ALLOC_CHECK
void emacs_heap_check_entry( struct heap_entry *h )
{
    if( memcmp( guard_pattern, h->front_guard, GUARD_SIZE ) != 0 )
        _dbg_msg( FormatString( "Corrupt front guard band in {%d}%X at %s(%d)") <<
            h->sequence_number << int(h) << h->fileName << h->lineNumber );
    if( memcmp( guard_pattern, &h->user_data[h->user_size], GUARD_SIZE ) != 0 )
        _dbg_msg( FormatString( "Corrupt back guard band in {%d}%X at %s(%d)") <<
            h->sequence_number << int(h) << h->fileName << h->lineNumber );
}

void emacs_heap_check(void)
{
    struct queue *entry;

#if    DBG_QUEUE
    queue_validate( &all_emacs_memory );
#endif
    entry = all_emacs_memory.next;
    while( (void *)entry != (void *)&all_emacs_memory )
    {
        emacs_heap_check_entry( (struct heap_entry *)entry );
        entry = entry->next;
    }
    return;
}
#endif

//
//
//    emacs_malloc
//
//
int malloc_count = 0;
void *emacs_malloc
    (
    int size
    , enum malloc_block_type type
#if DBG_ALLOC_CHECK
    , const char *fileName
    , int lineNumber
#endif
    )
{
    malloc_count++;

    struct heap_entry *h;

#if DBG_ALLOC_CHECK
    int old_dbg_flags = dbg_flags;

    if( dbg_flags&DBG_ALLOC_CHECK )
        emacs_heap_check();
#endif

#ifdef LOOK_ASIDE
    int alloc_size = (size+look_aside_granularity_mask)&~look_aside_granularity_mask;
    if( alloc_size < look_aside_max )
    {
        int index = alloc_size/look_aside_granularity;
        h = (struct heap_entry *)queue_remove( look_aside_lists[index].next );
        if( h == NULL )
            h = (struct heap_entry *)malloc( sizeof( struct heap_entry ) + alloc_size );
        else
            look_aside_cur_queue_length[index]--;
    }
    else
#endif
        h = (struct heap_entry *)malloc( sizeof( struct heap_entry ) + alloc_size );
    if( h == NULL )
    {
        printf("Emacs error: malloc failed to allocate %d bytes\n", size );
        printf("             emacs is using a total of %ld bytes\n", bytes_allocated );
        return NULL;
    }

    h->q.next = NULL;
    h->q.prev = NULL;
    h->user_size = size;
    if( type <= malloc_type_none || type >= malloc_type_last )
#if DBG_ALLOC_CHECK
        _dbg_msg( FormatString( "Bad malloc block type of %d in emacs_malloc at %s(%d)") << type << h->fileName << h->lineNumber );
#else
        _dbg_msg( FormatString( "Bad malloc block type of %d in emacs_malloc") << type );
#endif
    h->user_type = type;

#if DBG_ALLOC_CHECK
    h->lost_entry = false;
#endif

#ifdef SAVE_ENVIRONMENT
    if( type == malloc_type_emacs_object )
        last_heap_entry = h;
#endif
    bytes_allocated += size;

#if DBG_ALLOC_CHECK
    h->fileName = fileName;
    h->lineNumber = lineNumber;
    h->sequence_number = ++allocation_sequence_number;
    if( h->sequence_number == debug_mem_sequence_number )
        debug_mem_break();
#endif

#if DBG_ALLOC_CHECK
    memcpy( h->front_guard, guard_pattern, GUARD_SIZE );
    memcpy( &h->user_data[size], guard_pattern, GUARD_SIZE );

    if( ! (dbg_flags&DBG_ALLOC_CHECK) )
        dbg_flags &= ~DBG_QUEUE;
#endif
    queue_insert( all_emacs_memory.prev, &h->q );

#if DBG_ALLOC_CHECK
    dbg_flags = old_dbg_flags;
#endif

    // zero out all blocks except char types
    if( type != malloc_type_char )
        memset( &h->user_data[0], 0, size );

#if    DBG_MEM_BREAK
    if( ((void *)&h->user_data[0]) == debug_mem_address )
        debug_mem_break();
#endif
    return (void *)&h->user_data[0];
}

//
//
//    emacs_free
//
//
void emacs_free( void *p )
{
    struct heap_entry *h;
#if DBG_ALLOC_CHECK
    int old_dbg_flags = dbg_flags;
#endif

#if    DBG_MEM_BREAK
    if( p == debug_mem_address )
        debug_mem_break();
#endif
    h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

#if DBG_ALLOC_CHECK
    emacs_heap_check_entry( h );
    if( ! (dbg_flags&DBG_ALLOC_CHECK) )
        dbg_flags &= ~DBG_QUEUE;
#endif
    queue_remove( &h->q );

#if DBG_ALLOC_CHECK
    dbg_flags = old_dbg_flags;
#endif

    bytes_allocated -= h->user_size;

#if DBG_ALLOC_CHECK
    memset( &h->user_data[0], 0xdd, (unsigned int)h->user_size );
#endif

#if DBG_ALLOC_CHECK
    if( dbg_flags&DBG_ALLOC_CHECK )
        emacs_heap_check();
#endif

#ifdef LOOK_ASIDE
    int alloc_size = int((h->user_size+look_aside_granularity_mask)&~look_aside_granularity_mask);
    if( alloc_size < look_aside_max )
    {
        int index = alloc_size/look_aside_granularity;
        queue_insert( look_aside_lists[index].prev, &h->q );
        look_aside_cur_queue_length[index]++;
        if( look_aside_cur_queue_length[index] > look_aside_max_queue_length[index] )
            look_aside_max_queue_length[index] = look_aside_cur_queue_length[index];

    }
    else
#endif
        free( (void *)h );
}

//
//
//    emacs_realloc
//
//
void *emacs_realloc
    (
    void *p,
    int size
    , enum malloc_block_type type
#if DBG_ALLOC_CHECK
    , const char *fileName
    , int lineNumber
#endif
    )
{
    struct heap_entry *h;
#if DBG_ALLOC_CHECK
    int old_dbg_flags = dbg_flags;
#endif
    long int old_size;

    if( p == NULL )
        return emacs_malloc
            (
            size
            , type
#if DBG_ALLOC_CHECK
            , fileName
            , lineNumber
#endif
            );

    h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

#if DBG_ALLOC_CHECK
    emacs_heap_check_entry( h );
    if( ! (dbg_flags&DBG_ALLOC_CHECK) )
        dbg_flags &= ~DBG_QUEUE;
#endif
    queue_remove( &h->q );
#if DBG_ALLOC_CHECK
    dbg_flags = old_dbg_flags;
#endif

#if DBG_ALLOC_CHECK
    if( dbg_flags&DBG_ALLOC_CHECK )
        emacs_heap_check();
#endif

    old_size = h->user_size;
    bytes_allocated -= old_size;

    int alloc_size = (size+look_aside_granularity_mask)&~look_aside_granularity_mask;
#ifdef LOOK_ASIDE
    int old_alloc_size = int((old_size+look_aside_granularity_mask)&~look_aside_granularity_mask);
    if( old_alloc_size < look_aside_max )
    {
        // nothing to do if the old and new come from the same slot
        if( old_alloc_size != alloc_size )
        {
            // different slots move from one to another
            void *new_p = emacs_malloc
                    (
                    alloc_size,
                    h->user_type
#if DBG_ALLOC_CHECK
                    , fileName
                    , lineNumber
#endif
                    );
            if( new_p == NULL )
                return NULL;
            struct heap_entry *new_h = (struct heap_entry *)((char *)new_p-offsetof(struct heap_entry,user_data));

            queue_remove( &new_h->q );

            memcpy( new_h->user_data, h->user_data, (size_t)old_size );

            int old_index = old_alloc_size/look_aside_granularity;
            queue_insert( look_aside_lists[old_index].prev, &h->q );

            look_aside_cur_queue_length[old_index]++;
            if( look_aside_cur_queue_length[old_index] > look_aside_max_queue_length[old_index] )
                look_aside_max_queue_length[old_index] = look_aside_cur_queue_length[old_index];

            h = new_h;
        }
    }
    else
#endif
        h = (struct heap_entry *)realloc( h, sizeof( struct heap_entry ) + alloc_size );
    if( h == NULL )
        return NULL;

#if DBG_ALLOC_CHECK
    if( h->user_type != type )
        _dbg_msg( FormatString( "realloc attempted to change block type from %d to %d at %s(%d)") <<
            h->user_type << type << fileName << lineNumber );
#endif

    h->user_size = size;
    bytes_allocated += size;

#if DBG_ALLOC_CHECK
    h->fileName = fileName;
    h->lineNumber = lineNumber;
    h->sequence_number = ++allocation_sequence_number;
    if( h->sequence_number == debug_mem_sequence_number )
        debug_mem_break();
#endif

#if DBG_ALLOC_CHECK
    memcpy( h->front_guard, guard_pattern, GUARD_SIZE );
    memcpy( &h->user_data[size], guard_pattern, GUARD_SIZE );

    if( ! (dbg_flags&DBG_ALLOC_CHECK) )
        dbg_flags &= ~DBG_QUEUE;
#endif
    queue_insert( all_emacs_memory.prev, &h->q );

#if DBG_ALLOC_CHECK
    dbg_flags = old_dbg_flags;

    if( dbg_flags&DBG_ALLOC_CHECK )
        emacs_heap_check();
#endif

    // zero out all blocks except char types
    if( type != malloc_type_char && size > old_size )
        memset( &h->user_data[old_size], 0, size-(int)old_size );

    return (void *)&h->user_data[0];
}

int dump_memory_statistics( void )
{
    const int MAX_SLOTS( 128 );
    const int SLOT_SIZE( look_aside_granularity );

    struct queue *entry;
    int mem_size[MAX_SLOTS+1];
    int index;

    // init the array of results
    memset( &mem_size[0], 0, sizeof( mem_size ) );

    // scan the allocated memory and collate the sizes
    entry = all_emacs_memory.next;
    while( (void *)entry != (void *)&all_emacs_memory )
    {
        struct heap_entry *heap = (struct heap_entry *)entry;

        index = (int)(heap->user_size/SLOT_SIZE);
        if(  index > MAX_SLOTS )
            index = MAX_SLOTS;
        mem_size[index]++;

        entry = entry->next;
    }

    EmacsBuffer::scratch_bfn( "Memory usage statistics", interactive() );
    bf_cur->ins_str( u_str("Size    Count\n----    -----\n"));

    for( index=0; index<MAX_SLOTS-1; index++ )
    {
        if( mem_size[index] != 0 )
            bf_cur->ins_cstr( FormatString("%d    %d\n" ) << ((index+1)*SLOT_SIZE) << mem_size[index] );
    }

    bf_cur->ins_cstr( FormatString(">%d    %d\n") << (MAX_SLOTS*SLOT_SIZE) << mem_size[MAX_SLOTS] );

#ifdef LOOK_ASIDE
    bf_cur->ins_str( u_str("Look aside list details\n") );
    bf_cur->ins_str( u_str("Size    Current/Maximum\n") );
    for( index=0; index<(look_aside_max/look_aside_granularity); index++ )
    {
        bf_cur->ins_cstr( FormatString( "%d    %d/%d\n" )
                << index*look_aside_granularity
                << look_aside_cur_queue_length[index]
                << look_aside_max_queue_length[index] );
    }
#endif

    return 0;

}



//
//    from the old C queue.c file
//

//
//    Init the queue q as empty
//
static void queue_init( struct queue *q )
{
#if    DBG_QUEUE
    // insist on queue headers being NULL
    if( q->next != NULL || q->prev != NULL )
        fatal_error( 405 );
#endif

    q->next = q;
    q->prev = q;
}


//
//    Insert an element e into queue q
//    Return 1 if the queue was empty
//
static int queue_insert( struct queue *q, struct queue *e )
{
#if    DBG_QUEUE
    //
    // insist on the queue having pointers in next and prev.
    // If the header is NULL the queue has not been initialised
    //
    if( q->next == NULL || q->prev == NULL )
        fatal_error( 400 );
    //
    // insist on the element have NULL next and prev.
    // If they are pointers the element is on an other queue
    //
    if( e->next != NULL || e->prev != NULL )
        fatal_error( 401 );

    if( dbg_flags&DBG_QUEUE )
    {
        queue_validate( q );
    }
#endif

    QueueImplementation::queue_lock();

    e->next = q->next;
    e->prev = q->next->prev;
    q->next->prev = e;
    q->next = e;

    QueueImplementation::queue_unlock();

    return q->next == q->prev;
}

//
//    remove element e from the queue it is on
//    and return that element
//
static struct queue *queue_remove( struct queue *e )
{
#if    DBG_QUEUE
    // Check for a NULL queue - no init done or already removed
    if( e == NULL || e->next == NULL || e->prev == NULL )
        fatal_error( 402 );

    if( dbg_flags&DBG_QUEUE )
    {
        queue_validate( e );
    }
#endif

    // return NULL on queue empty
    if( e->next == e )
        return NULL;

    QueueImplementation::queue_lock();

    e->prev->next = e->next;
    e->next->prev = e->prev;

    QueueImplementation::queue_unlock();

    // NULL element to catch bugs
    e->next = NULL;
    e->prev = NULL;

    // return the element e
    return e;
}

//
//    queue validation is a tricky business that needs to
//    tacked with care for the types of bugs that queue
//    system hit.
//
//    Double inserts will cause the queue to have a loop
//    in it. To detect this we use the technic of walking
//    the queue with two pointers. One fast the other slow.
//    if they ever point to each other there is a loop.
//    if the fast point reaches the starting queue element
//    there is no problem.
//
//    along the way each element is checked for sanity.
//
#if    DBG_QUEUE
static void queue_validate( struct queue *q )
{
    struct queue *p1, *p2a, *p2;

    QueueImplementation::queue_lock();

    p1 = p2 = q;
    for(;;)
    {
        // step p2 on twice
        p2a = p2->next;
        // check linkages
        if( p2a->next->prev != p2a )
            fatal_error( 407 );
        if( p2a->prev->next != p2a )
            fatal_error( 408 );
        p2 = p2a->next;
        // check linkages
        if( p2->next->prev != p2 )
            fatal_error( 407 );
        if( p2->prev->next != p2 )
            fatal_error( 408 );

        // Step P1 on once
        p1 = p1->next;
        if( p1 == q )
            break;    // all done
        // check for loop
        if( p1 == p2 )
            fatal_error( 409 );
    }

    QueueImplementation::queue_unlock();
}
#endif

#if DBG_ALLOC_CHECK
#include <iostream>
#endif

int last_sequence_count = INT_MAX;
void dump_memory_since()
{
#if DBG_ALLOC_CHECK
    char buf[1024];

    sprintf( buf, "Dump memory from sequence %d to %d", last_sequence_count, allocation_sequence_number );
    std::cout << buf << std::endl;

    struct queue *entry = all_emacs_memory.next;
    while( (void *)entry != (void *)&all_emacs_memory )
    {
        struct heap_entry *heap = (struct heap_entry *)entry;

        if( heap->sequence_number >= last_sequence_count )
        {
            switch( heap->user_type )
            {
            case malloc_type_emacs_object:
                sprintf
                (
                buf, "{%d} %s(%d) class %s size %d\n",
                    heap->sequence_number,
                    heap->fileName, heap->lineNumber,
                    ((EmacsObject *)heap->user_data)->objectName(),
                    int(heap->user_size)
                );
                break;
            default:
                sprintf
                (
                buf, "{%d} %s(%d) type %d size %d",
                    heap->sequence_number,
                    heap->fileName, heap->lineNumber,
                    heap->user_type,
                    int(heap->user_size)
                );
                break;
           }
           std::cout << buf << std::endl;

        }

        entry = entry->next;
    }

    std::cout << "Dump memory done ---------------------------------------" << std::endl;
    last_sequence_count = allocation_sequence_number;
#endif
}
