//module minibuf
//    (
//    ident    = 'V5.0 Emacs',
//    addressing_mode (nonexternal = long_relative, external = general)
//    ) =
//begin
//
//      Copyright(c) 1982, 1983, 1984, 1985, 1986, 1987
//        Barry A. Scott and Nick Emery
//
// Functions to manipulate the minibuffer

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


EmacsString on_str("on");
EmacsString off_str("off");
EmacsString true_str("true");
EmacsString false_str("false");
EmacsString too_few_args_str("Too few arguments given to %s");

extern int gui_error;

// dump an error message; called like printf

SystemExpressionRepresentationString error_messages_buffer;

void error( const EmacsString &text )
{
    int error_handled = 0;

    next_local_keymap = 0;
    next_global_keymap = 0;

#if DBG_ML_ERROR
        if( dbg_flags&DBG_ML_ERROR )
                _dbg_msg( FormatString("ML Error: %s\n") << text );
#endif

    //
    // The first error message probably makes the
    // most sense, so we suppress subsequent ones.
    //
    if( ml_err && minibuf_body.haveBody() )
        return;

    error_message_text = text;

    if( in_error_occurred == 0
    &&  (error_handled = error_handler()) == 0 )
    {
        if( term_is_terminal != 0 )
        {
            minibuf_body.setMessageBody( text );
#if defined(_WINDOWS)
            if( gui_error )
                win_error( text );
#endif
        }
        else
            message_file.fio_put( text.data(),text.length() );
    }
    if( error_handled == 0 )
        ml_err = 1;

    if( !error_messages_buffer.asString().isNull() )
    {
        // save the message in the named buffer

        EmacsBufferRef old( bf_cur );

        EmacsBuffer *p = EmacsBuffer::find( error_messages_buffer.asString() );
        if( p == NULL )
        {
            p = EMACS_NEW EmacsBuffer( error_messages_buffer.asString() );
            p->b_checkpointed = -1;     // turn off checkpointing and
            p->b_journalling = 0;       // journalling
        }

        p->set_bf();
        set_dot( bf_cur->num_characters() + 1 );

        time_t now = time(0);

        EmacsString now_string( EmacsString::copy, (unsigned char *)ctime( &now ), 24 );

        bf_cur->ins_cstr( FormatString("%shandled error at %s\n")
            << EmacsString(in_error_occurred ? "  " : "un" )
            << now_string );
        dump_mlisp_stack();

        bf_cur->ins_str( "\n" );

        old.set_bf();
    }
}


// dump an informative message to the minibuf
void message( const EmacsString &text )
{
    if( !interactive() || (ml_err && minibuf_body.haveBody()) )
        return;

    // make sure that the buffer is not overflowed by very long messages
    if( term_is_terminal != 0 )
        minibuf_body.setMessageBody( text );
    else
        message_file.fio_put( text.data(), text.length() );
}


// Converts a string to an integer
int str_to_int (const EmacsString &answer)
{
    int index = 0;
    int len = answer.length();
    int n = 0;
    int neg = 0;

    while( index < len && isspace( answer[index] ) )
        index++;

    // Allow on and true to mean 1 and off and false to mean 0
    if( index < len && answer[index] >= 'A' )
    {
        EmacsString keyword( answer( index, len ) );

        if( keyword.commonPrefix( on_str ) == keyword.length()
        || keyword.commonPrefix( true_str ) == keyword.length() )
            return 1;
        if( keyword.commonPrefix( off_str ) == keyword.length()
        || keyword.commonPrefix( false_str ) == keyword.length() )
            return 0;
    }
    // Convert from text to numeric
    for( ; index < len; index++ )
    {
        if( isdigit( answer[index] ) )
            n = (n * 10) + answer[index] - '0';
        else if( answer[ index ] == '-' )
                neg = !neg;
        else if( !isspace( answer[ index ] ) && answer[ index ] != '+' )
        {
            error( FormatString("Malformed integer: \"%s\"") << answer );
            return 0;
        }
    }

    if( neg != 0 )
        return -n;
    else
        return n;
}


int getnum( const EmacsString &prompt )
{
    if( cur_exec == NULL )
        return get_number_interactive( prompt );
    else
        return get_number_mlisp();
}

// read a number from the terminal with prompt string s
int get_number_mlisp()
{
    emacs_assert( cur_exec != 0 );

    int larg = arg;
    enum arg_states larg_state = arg_state;
    int n;

    arg_state = no_arg;
    last_arg_used++;
    if( last_arg_used >= cur_exec->p_nargs )
    {
        error( FormatString(too_few_args_str) << cur_exec->p_proc->b_proc_name );
        return 0;
    }
    n = numeric_arg(last_arg_used + 1);
    arg = larg;
    arg_state = larg_state;
    return n;
}

int get_number_interactive( const EmacsString &prompt )
{
    emacs_assert( cur_exec == 0 );

    return str_to_int( br_get_string_interactive( 1, EmacsString::null, prompt ) );
}


EmacsString getstr( const EmacsString &prompt )
{
    if( cur_exec == NULL )
        return get_string_interactive( prompt );
    else
        return get_string_mlisp();
}

EmacsString getnbstr( const EmacsString &prompt )
{
    if( cur_exec == NULL )
        return get_nb_string_interactive( prompt );
    else
        return get_string_mlisp();
}

// Read a string from the terminal with prompt string s
EmacsString get_string_interactive( const EmacsString &prompt )
{
    return get_string_interactive( prompt, EmacsString::null );
}

EmacsString get_string_interactive( const EmacsString &prompt, const EmacsString &default_value )
{
    Save<int> old_help( &help );
    Save<int> old_expand( &expand );

    return br_get_string_interactive( 0, default_value, prompt );
}

// Read a string from the terminal with prompt string s, whitespace
// will terminate it.
EmacsString get_nb_string_interactive( const EmacsString &prompt )
{
    return get_nb_string_interactive( prompt, EmacsString::null );
}

EmacsString get_nb_string_interactive( const EmacsString &prompt, const EmacsString &default_value )
{
    Save<int> old_help( &help );
    Save<int> old_expand( &expand );

    return br_get_string_interactive( 1, default_value, prompt );
}


EmacsString get_string_mlisp()
{
    return br_get_string_mlisp();
}


//
// Read a string from the terminal with prompt string s.
//   Whitespace will break iff breaksp is true.
//   The string 'prefix' behaves as though the user had typed that first.
//
EmacsString br_get_string_interactive( int breaksp, const EmacsString &prefix, const EmacsString &prompt )
{
    int larg = arg;
    enum arg_states larg_state = arg_state;

    arg_state = no_arg;
    emacs_assert( cur_exec == NULL );

    if( command_file.fio_is_open() )
    {
        //
        // get the string from the command file
        //
        unsigned char lbuf[BUFFERSIZE + 1];
        int read_size;

        read_size = command_file.fio_get_with_prompt( lbuf, sizeof(lbuf), prompt.data() );
        if( read_size <= 0 )
        {
            error("No more input available");
            return EmacsString::null;
        }

        return EmacsString( EmacsString::copy, lbuf, read_size );
    }

    //
    // get the string from the keyboard
    //
    unsigned char *result = 0;
    KeyMap *outermap;
    int outer_dot;
    int window_num = 0;
    EmacsWindow *last_w = NULL;

    Save<MiniBufferBody> saved_minibuf_body( &minibuf_body );
    minibuf_body.setPromptBody( prompt );

    Marker olddot( bf_cur, dot, 0 );

    EmacsWindow *w = theActiveView->windows.windows;
    int i = 0;
    while( w != NULL )
    {
        if( w == theActiveView->currentWindow() )
            window_num = i;
        i++;
        last_w = w;
        w = w->w_next;
    }
    last_w->set_win();

    outermap = bf_cur->b_mode.md_keys;

    if( breaksp != 0 )
        bf_cur->b_mode.md_keys = minibuf_local_ns_map;
    else
        bf_cur->b_mode.md_keys = minibuf_local_map;

    next_global_keymap = next_local_keymap = 0;

    outer_dot = dot;
    bf_cur->gap_outside_of_range( 1, bf_cur->unrestrictedSize()+1 );
    EmacsString outer( EmacsString::copy, bf_cur->ref_char_at(1), bf_cur->unrestrictedSize() );
    bf_cur->erase_bf();
    bf_cur->ins_cstr(prefix);

    minibuf_depth++;
    recursive_edit();
    minibuf_depth--;

    arg = larg;
    arg_state = larg_state;
    minibuf->set_bf();
    bf_cur->b_mode.md_keys = outermap;
    bf_cur->insert_at (bf_cur->unrestrictedSize() + 1, 0);
    set_dot (1);
    bf_cur->ins_cstr( outer );
    set_dot( outer_dot );
    result = ml_err ? 0 : bf_cur->ref_char_at( outer.length() + 1 );
    bf_cur->del_back (bf_cur->unrestrictedSize() + 1, bf_cur->unrestrictedSize() - outer.length() );

    w = theActiveView->windows.windows;
    while( window_num != 0 && w != 0 )
    {
        window_num--;
        w = w->w_next;
    }
    if( window_num == 0 && w != 0 )
    {
        w->set_win();
    }
    else
        theActiveView->window_on( bf_cur );

    set_dot( olddot.to_mark() );

    if( result == NULL )
        throw EmacsExceptionUserInputAbort();
    return EmacsString( result );
}


EmacsString br_get_string_mlisp()
{
    int larg = arg;
    enum arg_states larg_state = arg_state;

    arg_state = no_arg;
    emacs_assert( cur_exec != NULL );

    last_arg_used++;
    if( last_arg_used >= cur_exec->p_nargs )
    {
        error( FormatString(too_few_args_str) << cur_exec->p_proc->b_proc_name);
        return EmacsString::null;
    }
    if( !string_arg (last_arg_used + 1)
    || ml_value.exp_type() != ISSTRING )
    {
        error
        (
        FormatString("%s expected %s to return a value.") <<
            cur_exec->p_proc->b_proc_name <<
            cur_exec->arg( last_arg_used+1 )->p_proc->b_proc_name
        );
        return EmacsString::null;
    }
    arg = larg;
    arg_state = larg_state;

    if( ml_err )
        return EmacsString::null;

    return ml_value.asString();
}


EmacsString get_key( KeyMap *kmap, const EmacsString &prompt )
{
    if( cur_exec == NULL )
        return get_key_interactive( kmap, prompt );
    else
        return get_key_mlisp();
}

// Get the name of a key. Alas, you can't type a control-G,
// since that aborts the key name read. Returns-1 if aborted.
EmacsString get_key_interactive( KeyMap *kmap, const EmacsString &prompt )
{
    emacs_assert( cur_exec == 0 );

    EmacsString keys;

    // save the mini buffer state for restore on exit
    Save<MiniBufferBody> saved_minibuf_body( &minibuf_body );

    EmacsString minibuf_text;
    minibuf_text = prompt;

    do
    {
        minibuf_body.setPromptBody( minibuf_text );

        unsigned char c;
        if( (c = (unsigned char)get_char()) == '\007' )
        {
            error( "Aborted.");
            return EmacsString::null;
        }

        keys.append( c );

        BoundName *b = kmap->getBinding( c );
        kmap = NULL;
        if( b != NULL )
            kmap = b->getKeyMap();    // get NULL or a KeyMap

        if( c == '\033' )
        {
            minibuf_text.append( "ESC" );
        }
        else
            if( c < ' ' )
            {
                minibuf_text.append( '^' );
                minibuf_text.append( (unsigned char)((c & 037) + 0100) );
            }
            else
                    {
                minibuf_text.append( c );
            }
        if( kmap != NULL )
        {
            minibuf_text.append( '-' );
        }
    }
    while( kmap != NULL );

    ml_value = keys;

    return keys;
}

EmacsString get_key_mlisp()
{
    emacs_assert( cur_exec != 0 );

    int larg = arg;
    enum arg_states larg_state = arg_state;

    arg_state = no_arg;
    last_arg_used++;
    eval_arg( last_arg_used + 1 );
    arg = larg;
    arg_state = larg_state;
    if( ml_err )
        return EmacsString::null;

    EmacsString output;
    if( ml_value.exp_type() != ISSTRING )
    {
        output.append( (unsigned char)ml_value.asInt() );
    }
    else
    if( convert_key_string( ml_value.asString(), output ) == 0 )
    {
        error( "Key sequence too long" );
        return EmacsString::null;
    }

    ml_value = output;
    return output;
}

int error_and_exit( void )
{
    error( "Aborted.");
    return -1;
}

int expand_and_exit( void )
{
    expand = 1;
    return -1;
}

int help_and_exit( void )
{
    help = 1;
    return -1;
}

//
//  Returns true if the user answers yes to a question.
// The user will always be asked
//
class BooleanWordsNameTable : public EmacsStringTable
{
public:
    BooleanWordsNameTable();
    virtual ~BooleanWordsNameTable();
};

BooleanWordsNameTable boolean_words;

BooleanWordsNameTable::BooleanWordsNameTable()
    : EmacsStringTable( 2, 1 )
{
    static int zero(0);
    static int one(1);
    static int minus_one(-1);
    add( "no", &zero );    // false
    add( "yes", &one );    // true
    add( "", &minus_one );    // default
}

BooleanWordsNameTable::~BooleanWordsNameTable()
{ }

int get_yes_or_no( int yes, const EmacsString &prompt )
{
    int answer;
#if defined( _WINDOWS )
    answer = win_yes_or_no( yes, prompt );
#else

#if defined( XWINDOWS )
    if( is_motif )
        answer = motif_yes_or_no( yes, prompt );
    else
#endif
    {
        Save<ProgramNode *> lcur_exec( &cur_exec );
        cur_exec = NULL;

        EmacsString word;
        boolean_words.get_word_interactive( prompt, word );

        void *ptr_answer = boolean_words.find( word );
        answer = *(int *)ptr_answer;
    }
#endif
    return answer == 0 || answer == 1 ? answer : yes;
}


//--------------------------------------------------------------------------------
//
//    MiniBufferBody
//
//--------------------------------------------------------------------------------
MiniBufferBody::MiniBufferBody()
    : body_type( no_body)
    , body()
{}

MiniBufferBody::~MiniBufferBody()
{}

MiniBufferBody::MiniBufferBody( const MiniBufferBody &other )
    : body_type( other.body_type )
    , body( other.body )
{}

MiniBufferBody &MiniBufferBody::operator=( const MiniBufferBody &other )
{
    body_type = other.body_type;
    body = other.body;

    return *this;
}

void MiniBufferBody::setMessageBody( const EmacsString &body_ )
{
    body_type = message_body;
    body = body_;
}

void MiniBufferBody::setPromptBody( const EmacsString &body_ )
{
    body_type = prompt_body;
    body = body_;
}

void MiniBufferBody::clearMessageBody()
{
    if( body_type == message_body )
        body_type = no_body;

    // leave the body for debugging otherwise might be nice to set to null
}

const EmacsString& MiniBufferBody::getBody() const
{
    if( haveBody() )
        return body;
    else
        return EmacsString::null;
}

bool MiniBufferBody::haveBody() const
{
    return body_type != no_body;
}

bool MiniBufferBody::isPromptBody() const
{
    return body_type == prompt_body;
}
