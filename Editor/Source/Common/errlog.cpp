//module errlog
//    (
//    ident = 'V5.0 Emacs',
//    addressing_mode (nonexternal=long_relative, external=general)
//    ) =
//begin
//     Copyright (c) 1982, 1983, 1984, 1985, 1986, 1987
//        Barry A. Scott and Nick Emery

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <errlog.h>

void delete_errlog_list( void );
int parse_erb(int pos, int limit);
int next_err( void );
int prev_err( void );


// Parse all of the error messages found in a region
int parse_error_messages_in_region( void )
{
    int left;
    int right;
    right = dot;
    if( !bf_cur->b_mark.isSet() )
        error( "Mark not set." );
    else
    {
        left = bf_cur->b_mark.to_mark();
        if( left > right )
        {
            right = left;
            left = dot;
        }
        parse_erb( left, right );
    }
    return 0;
}

int next_error( void )
{
    next_err();
    return 0;
}

int previous_error( void )
{
    prev_err();
    return 0;
}

SystemExpressionRepresentationString error_file_name;
SystemExpressionRepresentationIntPositive error_line_number;
SystemExpressionRepresentationIntPositive error_start_position;

BoundName *error_message_parser;
ErrorBlock *errors;    // the list of all error messages
ErrorBlock *thiserr;    // the current error

ErrorBlock::ErrorBlock( EmacsBuffer *erb, int erp, EmacsBuffer *b, int p)
    : e_mess( erb, erp )
    , e_text( b, p )
    , e_next( NULL )
    , e_prev( NULL )
{ }

ErrorBlock::~ErrorBlock()
{ }


bool error_messages_available()
{
    return errors != NULL;
}

// delete the error list
void delete_errlog_list( void )
{
    while( errors != NULL)
    {
        ErrorBlock *e = errors;
        errors = e->e_next;
        delete e;
    }
    thiserr = NULL;
}


// Parse error messages from the current buffer from character pos to limit
int parse_erb( int pos, int limit )
{
    EmacsBuffer *erb = bf_cur;
    int rv = 0;
    int lo;
    int hi;

    // remove the last set of error markers
    delete_errlog_list ();

    // check that there is a parser to call
    if( error_message_parser == NULL )
    {
        error( "No error message parser for parse-error-message-in-region.");
        return 0;
    }

    // Set up the buffer in which the parse is to take place
    erb->set_bf();

    Save<int> old_dot( &dot );

    dot = pos;
    // Narrow the buffer on the reion to be parsed
    lo = bf_cur->b_mode.md_headclip;
    hi = bf_cur->b_mode.md_tailclip;
    bf_cur->b_mode.md_headclip =
        bf_cur->b_mode.md_headclip = pos;
    bf_cur->b_mode.md_tailclip =
        bf_cur->b_mode.md_tailclip=
        bf_cur->unrestrictedSize() + 1 - limit;

    //
    // Invoke the scanning routine supplied by the user.
    //
    while( !interrupt_key_struck && !ml_err && rv == 0 )
    {
        // select one to the error buffer
        erb->set_bf();
        // Apply the users function
        rv = execute_bound_saved_environment(error_message_parser);
        // check that he output everything required
        if( ml_err
        || error_start_position <= 0
        || error_line_number <= 0
        || error_file_name.isNull() )
            break;
        // Visit the required file and create the markers
        if( visit_file( error_file_name.asString(), 0, 0, EmacsString::null ) == 0 )
        {
            break;
        }
        int pos = error_line_number == 1 ? 1 : scan_bf_for_lf( 1, error_line_number - 1 );
        ErrorBlock *new_eb = EMACS_NEW ErrorBlock
                (
                erb, error_start_position,
                bf_cur, pos
                );
        if( errors != NULL )
        {
            thiserr->e_next = new_eb;
            thiserr->e_next->e_prev = thiserr;
            thiserr = thiserr->e_next;
        }
        else
        {
            errors = thiserr = new_eb;
        }
    }

    // Restore the error message buffer to it original glory
    erb->set_bf();
    bf_cur->b_mode.md_headclip = lo;
    bf_cur->b_mode.md_tailclip = hi;
    thiserr = NULL;

    return errors != NULL;
}


// move to the next error message in the log
int next_err( void )
{
    int n;

    if( errors == 0 )
    {
        error( "No errors!");
        return 0;
    }

    if( thiserr == 0 )
        thiserr = errors;
    else
    {
        thiserr = thiserr->e_next;
        if( thiserr == 0 )
        {
            error( "No more errors...");
            return 0;
        }
    }
    n = thiserr->e_mess.to_mark();
    theActiveView->window_on (bf_cur);
    set_dot (n);
    theActiveView->currentWindow()->setWindowStart( dot );
    n = thiserr->e_text.to_mark();
    theActiveView->window_on( bf_cur );
    set_dot( n );

    return 1;
}


// move to the next error message in the log
int prev_err( void )
{
    int n;

    if( errors == 0 )
    {
        error( "No errors!");
        return 0;
    }

    if( thiserr == 0 )
        thiserr = errors;
    else
    {
        thiserr = thiserr->e_prev;
        if( thiserr == 0 )
        {
            error( "No more errors..." );
            return 0;
        }
    }
    n = thiserr->e_mess.to_mark();
    theActiveView->window_on( bf_cur );
    set_dot( n );
    theActiveView->currentWindow()->setWindowStart( dot );
    n = thiserr->e_text.to_mark();
    theActiveView->window_on( bf_cur );
    set_dot( n );

    return 1;
}
