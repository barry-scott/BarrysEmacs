//
//    em_timer.h
//    
//    Copyright (c) 1993-2010 Barry A. Scott
//
class EmacsDateTime
{
public:
    EmacsDateTime()
    : time_value( 0.0 )
    { }
    EmacsDateTime( const EmacsDateTime &dueTime )
    : time_value( dueTime.time_value )
    { }
    EmacsDateTime( double interval )
    : time_value( now().time_value + interval )
    { }

    virtual ~EmacsDateTime() { };

    EmacsDateTime &operator =( const EmacsDateTime &time )
    {
        time_value = time.time_value;
        return *this;
    }

    EmacsDateTime &operator +=( double interval )
    { 
        time_value += interval;
        return *this;
    }

    bool operator ==( const EmacsDateTime &time ) const
    {
        return time_value == time.time_value;
    }

    bool operator !=( const EmacsDateTime &time ) const
    {
        return time_value != time.time_value;
    }
    bool operator  <( const EmacsDateTime &time ) const
    {
        return time_value  < time.time_value;
    }
    bool operator <=( const EmacsDateTime &time ) const
    {
        return time_value <= time.time_value;
    }
    bool operator  >( const EmacsDateTime &time ) const
    {
        return time_value  > time.time_value;
    }
    bool operator >=( const EmacsDateTime &time ) const
    { 
        return time_value >= time.time_value;
    }

    EmacsString asString() const;
    double asDouble() const { return time_value; }

    static EmacsDateTime now();         // implement in OS specfic code
private:
    double time_value;                  // in seconds
};

class EmacsTimer : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsTimer )

    EmacsTimer();
    virtual ~EmacsTimer();

    void scheduleTimeOut( double interval );
    void scheduleTimeOut( const EmacsDateTime &time );

    void cancelTimeOut();

    bool timerIsScheduled() { return is_scheduled; }
    const EmacsDateTime &dueTime() { return due_time; }

    // User must supply the time out routine in a derived class
    virtual void timeOut() = 0;
    // User must supply a string for debugging code to print
    virtual EmacsString description() = 0;

    static void handle_timeout( void );
private:
    EmacsDateTime due_time;
    bool is_scheduled;
};


// Timer queue structures
class ProcTimer : public EmacsTimer
{
public:
    EMACS_OBJECT_FUNCTIONS( ProcTimer )

    ProcTimer( BoundName *proc, time_t interval );
    virtual ~ProcTimer();

    virtual void timeOut();
    virtual EmacsString description();

    BoundName *tim_procedure_to_call;    // Procedure
    time_t tim_reschedule_interval;        // Delta seconds
};

class TimerEntry : public QueueEntry<TimerEntry>
{
public:
    EMACS_OBJECT_FUNCTIONS( TimerEntry )

    TimerEntry( EmacsTimer *_timer ) : timer( _timer ) { };
    virtual ~TimerEntry() { queueRemove(); }

    EmacsTimer *timer;
};

class ProcTimerEntry : public QueueEntry<ProcTimerEntry>
{
public:
    EMACS_OBJECT_FUNCTIONS( ProcTimerEntry )
    ProcTimerEntry( ProcTimer *_timer ) : timer( _timer ) { };
    virtual ~ProcTimerEntry() { queueRemove(); }

    ProcTimer *timer;
};


// routines that support the timer sub system
extern void time_schedule_timeout( void (*time_handle_timeout)(void), const EmacsDateTime &due_time );
extern void time_cancel_timeout(void);
extern void time_call_timeout_handler();

#if DBG_TIMER
#define TimerTrace( s ) do \
{ \
    if( dbg_flags&DBG_TIMER ) \
        _dbg_msg( FormatString("%s: %s") << EmacsDateTime::now().asString() << (s) ); \
} while( must_be_zero )
#else
#define TimerTrace( s ) // do nothing
#endif
