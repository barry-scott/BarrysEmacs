//
//    Em_Time.c - emacs time services
//    Copyright 1993-2000 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


void process_timer_interrupts( void );
int schedule_procedure( void );


QueueHeader<ProcTimerEntry> timer_list_head;

//
//    Routine to field a Timer interrupt.
//    It just wakes up the scheduler.
//
void ProcTimer::timeOut()
{
    TimerTrace( "timer_interrupt" );
    //
    //    Wake up the Scheduler
    //
    interlock_inc( &input_pending );
    interlock_inc( &timer_interrupt_occurred );
}

//
//    Routine, called by the scheduler, to process timer interrupts
//
void process_timer_interrupts( void )
{
    TimerTrace( "process_timer_interrupts" );

    interlock_dec( &input_pending );
    interlock_dec( &timer_interrupt_occurred );

    //
    //    Scan the timer list for expired timer entries
    //
    EmacsDateTime time_now( EmacsDateTime::now() );

    while( !timer_list_head.queueEmpty() )
    {
        ProcTimerEntry *p = timer_list_head.queueFirst();
        ProcTimer *timer = p->timer;

        //
        //    The timer list is sorted by due-time, check if
        //    we have processed all the due timer entries.
        //
        if( timer->dueTime() > time_now )
            break;
        //
        //    This item is due: Unlink it from the list and delete it.
        //
        delete p;

        //
        //    Call its procedure
        //
        TimerTrace("process_timer_interrupts found procedure");
        if( timer->tim_procedure_to_call != NULL )
        {
            TimerTrace( FormatString("process_timer_interrupts calling procedure %s")
                << timer->tim_procedure_to_call->b_proc_name );

            execute_bound_saved_environment( timer->tim_procedure_to_call );
        }
    }
}

//
//
//    MLisp procedure to schedule the repeated execution of other MLisp
//    procedures: ( schedule-procedure "procedure-name" seconds )
//
//
int schedule_procedure( void )
{
    //
    //    Get the procedure to Reschedule
    //
    BoundName *procedure = getword( BoundName::, ": procedure to schedule " );
    if( procedure == NULL )
        return 0;

    //
    //    Get the ReScheduling Interval
    //
    int reschedule_interval = getnum( ": interval (in seconds) " );
    if( reschedule_interval < 0 )
    {
        error( "bad interval." );
        return 0;
    }

    TimerTrace( FormatString("schedule_procedure proc %s, interval %d")
        << procedure->b_proc_name << reschedule_interval );

    //
    //    search down the Timer List and see if this procedure is on it
    //
    ProcTimerEntry *e = timer_list_head.queueFirst();
    while( !timer_list_head.queueEntryIsHeader( e ) )
    {
        if( e->timer->tim_procedure_to_call == procedure )
        {
            //
            //    Found it - Unlink it from the List
            //
            TimerTrace( FormatString("schedule_procedure removing proc %s")
                << procedure->b_proc_name );

            delete e->timer;
            delete e;
            break;
        }
        e = e->queueNext();
    }

    //
    //    If the interval is 0, we are cancelling a timer request
    //
    if( reschedule_interval == 0 )
        return 0 ;

    TimerTrace( FormatString("schedule_procedure new TimerEntry for proc %s")
        << procedure->b_proc_name );
    ProcTimer *t = EMACS_NEW ProcTimer( procedure, reschedule_interval );
    e = EMACS_NEW ProcTimerEntry( t );

    //
    //    Insert into the Timer List
    //
    t->scheduleTimeOut( reschedule_interval );

    QueueImplementation::queue_lock();

    // if this is the first element in the queue ask for a timeout
    if( timer_list_head.queueEmpty() )
    {
        e->queueInsert( timer_list_head.queueLast() );
    }
    else
    {
        ProcTimerEntry *cur = timer_list_head.queueFirst();

        while( !timer_list_head.queueEntryIsHeader( cur ) )
        {
            // does it go before this entry?
            if( e->timer->dueTime() < cur->timer->dueTime() )
                // yep break
                break;
            // the next item
            cur = cur->queueNext();
        }

        // insert before the cur item
        e->queueInsert( cur->queuePrev() );
    }

    QueueImplementation::queue_unlock();


    return 0;
}

void init_scheduled_timeout(void)
{ }

void restore_scheduled_timeout(void)
{
    TimerTrace( "restore_scheduled_timeout" );

}


ProcTimer::ProcTimer( BoundName *proc, time_t resched )
    : EmacsTimer()
    , tim_procedure_to_call( proc )
    , tim_reschedule_interval( resched )
{ }

ProcTimer::~ProcTimer()
{ }

EmacsString ProcTimer::description()
{
    EmacsString result( "ProcTimer for " );
    result.append( tim_procedure_to_call->b_proc_name );

    return result;
}

