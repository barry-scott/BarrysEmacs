//
//    timer.c
//    emacs V6.0
//    Author: Barry A. Scott
//    Copyright (c) 1993-1997
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

QueueHeader<TimerEntry> timer_queue;

void restore_timer(void)
{
    TimerTrace("restore_timer");
    if( !timer_queue.queueEmpty() )
        time_schedule_timeout( EmacsTimer::handle_timeout, timer_queue.queueFirst()->timer->dueTime() );
}

#if DBG_TIMER
void dump_timer_queue( const char *title )
{
    TimerTrace( FormatString("Dump timer queue - %s") << title );
    QueueIterator<TimerEntry> it( timer_queue );
    while( it.next() )
    {
        TimerEntry *cur = it.value();
        EmacsTimer *timer = cur->timer;

        TimerTrace( FormatString("    At %s run %s") << timer->dueTime().asString() << timer->description() );
    }

    TimerTrace( "       -------------------" );
}
#endif


void EmacsTimer::handle_timeout( void )
{
    if( timer_queue.queue_empty() )
    {
        // if the queue is now empty cancel the timeout
        time_cancel_timeout();
        return;
    }

    EmacsDateTime current_time( EmacsDateTime::now() );

#if DBG_TIMER
    dump_timer_queue( "handle_timeout" );
#endif

    // walk the entire queue
    while( !timer_queue.queueEmpty() )
    {
        TimerEntry *cur = timer_queue.queueFirst();
        EmacsTimer *timer = cur->timer;

        // see if it time for this entry
        if( timer->dueTime() > current_time )
            // no, exit loop
            break;

        TimerTrace( FormatString("    handle_timeout calling handler %s") << timer->description() );

        // free the block
        delete cur;

        timer->is_scheduled = false;

        // call the timeout routine
        timer->timeOut();

    }

    // only ask for timeout if something is in the queue
    if( !timer_queue.queueEmpty() )
        time_schedule_timeout( handle_timeout, timer_queue.queueFirst()->timer->dueTime() );
}

void EmacsTimer::scheduleTimeOut( double interval )
{
    EmacsDateTime due_at( EmacsDateTime::now() );

    due_at += interval;
    scheduleTimeOut( due_at );
}

void EmacsTimer::scheduleTimeOut( const EmacsDateTime &time )
{
    // always cancel first
    if( timerIsScheduled() )
        cancelTimeOut();

    TimerEntry *req = EMACS_NEW TimerEntry( this );

    if( req == NULL )
        return;

    due_time = time;

    TimerTrace( FormatString("At %s run scheduleTimeOut id %s") << due_time.asString() << description() );

#if DBG_TIMER
    dump_timer_queue( "start of scheduleTimeOut" );
#endif

    QueueImplementation::queue_lock();

    // if this is the first element in the queue ask for a timeout
    if( timer_queue.queueEmpty() )
    {
        req->queueInsert( timer_queue.queueLast() );
    }
    else
    {
        TimerEntry *cur = timer_queue.queueFirst();

        while( !timer_queue.queueEntryIsHeader( cur ) )
        {
            // does it go before this entry?
            if( time < cur->timer->dueTime() )
                // yep break
                break;
            // the next item
            cur = cur->queueNext();
        }

        // insert before the cur item
        req->queueInsert( cur->queuePrev() );
    }

    QueueImplementation::queue_unlock();

    is_scheduled = true;

    if( req == timer_queue.queueFirst() )
    {
        time_cancel_timeout();
        time_schedule_timeout( handle_timeout, due_time );
    }
#if DBG_TIMER
    dump_timer_queue( "end of scheduleTimeOut" );
#endif
}

void EmacsTimer::cancelTimeOut()
{
    QueueImplementation::queue_lock();

    TimerTrace( FormatString("time_remove_requests id %s") << description() );

    TimerEntry *cur = timer_queue.queueFirst();

    while( !timer_queue.queueEntryIsHeader( cur ) )
    {
        // is it this entry?
        if( cur->timer == this )
        {
            TimerTrace( FormatString("time_remove_requests found %s") << cur->timer->description() );
            delete cur;
            break;
        }

        // the next item
        cur = cur->queueNext();
    }

    QueueImplementation::queue_unlock();

    // if the queue is now empty cancel the timeout
    time_cancel_timeout();

    if( !timer_queue.queueEmpty() )
    {
        EmacsTimer *timer = timer_queue.queueFirst()->timer;

        time_schedule_timeout( handle_timeout, timer->dueTime() );
    }

    is_scheduled = false;

#if DBG_TIMER
    dump_timer_queue( "end of time_remove_requests" );
#endif
}

EmacsTimer::EmacsTimer()
    : due_time()
    , is_scheduled(false)
{ }

EmacsTimer::~EmacsTimer()
{
    if( is_scheduled )
        cancelTimeOut();
}
