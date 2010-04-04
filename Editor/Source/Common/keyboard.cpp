//
//    Copyright (c) 1982-2010
//        Barry A. Scott
//

// keyboard manipulation primitives

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


KeyMap *define_keymap( const EmacsString &name );
int process_key( void );
int get_char( void );
int start_remembering( void );
int stop_remembering( void );
void exec_str( const EmacsString &s );
int execute_keyboard_macro( void );
int push_back_character( void );
int top_level_command( void );
int recursive_edit( void );
int recurse_depth( void );
int interrupt_emacs( void );
int convert_key_string_command( void );
int convert_key_string( const EmacsString &input, EmacsString &output );
void init_key( void );
void re_init_keyboard( void );

extern void record_keystoke_history( const EmacsString &keys, BoundName *proc );

//
//    Global Data definitions
//
SystemExpressionRepresentationInt input_mode;
SystemExpressionRepresentationIntBoolean activity_indicator;
EmacsMacroString macro_replay_body;
int macro_replay_next = -1;
SystemExpressionRepresentationIntReadOnly keyboard_input_pending;
SystemExpressionRepresentationString keystroke_history_buffer;
volatile int input_pending;
volatile int timer_interrupt_occurred;
volatile int interrupt_key_struck;
volatile int pending_channel_io;
EmacsChar_t activity_character = 'x';
BoundNameNoDefine interrupt_block( "interrupt-key", interrupt_emacs );
SystemExpressionRepresentationIntPositive checkpoint_frequency;
int end_of_mac;
KeyMap *next_global_keymap;
KeyMap *next_mode_keymap;
KeyMap *next_local_keymap;
KeyMap *current_global_map;
KeyMap *global_map;
KeyMap *minibuf_local_ns_map;
KeyMap *minibuf_local_map;

CharElement char_cells[ CHARACTER_QUEUE_SIZE ];

QueueHeader<CharElement> free_queue;
QueueHeader<CharElement> push_back_queue;
QueueHeader<CharElement> input_queue;
bool EmacsWorkItem::enabled = false;
QueueHeader<EmacsWorkItem> EmacsWorkItem::work_queue;
int EmacsWorkItem::queued_while_disabled = 0;

// A keyboard called procedure returns:
//     1 to continue interpretation
//     0 normally
//    -1 to quit
//    -2 if this proc has set arg & arg should be handled by next command
//

static int keystrokes;        // The number of keystrokes since the last checkpoint.
static int can_checkpoint;    // True iff we are allowed to checkpoint now.

static bool default_key_set( false );
static EmacsChar_t default_key;

KeyMap::KeyMap( const EmacsString &name )
: k_name( name )
, k_default_binding( NULL )
, k_binding()
{
}

KeyMap::~KeyMap()
{
    removeAllBindings();
}

void KeyMap::q() const
{
    std::cout << "KeyMap " << k_name.sdata() << std::endl;
    if( k_default_binding != NULL )
    {
        std::cout << "    default: " << k_default_binding->b_proc_name.sdata() << std::endl;
    }

    for( EmacsCharToBoundName_t::const_iterator it=k_binding.begin();
        it != k_binding.end();
            ++it )
    {
        EmacsChar_t key = it->first;
        BoundName *b = it->second;

        std::cout << "    key " << std::hex << key << ": " << b->b_proc_name.sdata() << std::endl;
    }
}

//
//    routines to process keymaps
//
BoundName *KeyMap::getBinding( EmacsChar_t c )
{
    EmacsCharToBoundName_t::iterator i = k_binding.find( c );
    if( i == k_binding.end() )
        return k_default_binding;
    else
        return i->second;
}

void KeyMap::addBinding( EmacsChar_t c, BoundName *proc )
{
    if( !default_key_set )
    {
        EmacsString default_value( PC_key_names.valueOfKeyName( "default" ) );
        if( default_value.length() == 1 )
        {
            default_key = default_value[0];
            default_key_set = true;
        }
    }

    removeBinding( c );

    if( default_key_set && c == default_key )
    {
        k_default_binding = proc;
    }
    else
    {
        k_binding[ c ] = proc;
    }
}

void KeyMap::removeBinding( EmacsChar_t c )
{
    if( default_key_set && c == default_key )
    {
        free_sexpr_defun( k_default_binding );
        k_default_binding = NULL;
    }
    else
    {
        EmacsCharToBoundName_t::iterator i = k_binding.find( c );
        if( i != k_binding.end() )
        {
            free_sexpr_defun( i->second );
            k_binding.erase( i );
        }
    }
}

void KeyMap::removeAllBindings()
{
    if( k_default_binding != NULL )
    {
            free_sexpr_defun( k_default_binding );
            k_default_binding = NULL;
    }

    while( !k_binding.empty() )
    {
        EmacsCharToBoundName_t::iterator i( k_binding.begin() );
        free_sexpr_defun( i->second );
        k_binding.erase( i );
    }
}

KeyMap *define_keymap( const EmacsString &name )
{
    KeyMap *kmap;

    kmap = EMACS_NEW KeyMap( name );
    if( kmap == NULL )
        return 0;

    BoundName *cmd = BoundName::find( name );
    if( cmd != NULL )
        cmd->replaceInside( kmap );
    else
        cmd = EMACS_NEW BoundName( name, kmap );

    return kmap;
}

// process_keys reads keystrokes and interprets them according to the
//  given keymap and its inferior keymaps

static KeyMap null_map( "--null keymap--" );

int process_keys( void )
{
    EmacsString keys_struck;

    next_global_keymap = NULL;
    next_local_keymap = NULL;

    for(;;)
    {
#if DBG_ALLOC_CHECK
        // test memory state at the heart of the emacs loop
        if( dbg_flags&DBG_ALLOC_CHECK )
            emacs_heap_check();
#endif

        if( quitting_emacs != 0 )
        {
            if( recursive_edit_depth != 0 )
            {
                return 0;
            }
            else
            {
                quitting_emacs = 0;
            }
        }

        if( (next_global_keymap == 0) )
        {
            if( remembering )
                end_of_mac = key_mem.length();
            if( arg_state != have_arg
            && macro_replay_next < 0
            && bf_cur != minibuf )
                undo_boundary();
        }

        if( bf_cur != minibuf )
            can_checkpoint = 1;

        int ic = get_char();

        if( ic < 0 )
        {
            can_checkpoint = 0;
            return 0;
        }

        EmacsChar_t c = (EmacsChar_t)ic;

        can_checkpoint = 0;
        keys_struck.append( c );

        if( next_global_keymap == NULL )
            next_global_keymap = current_global_map;
        if( next_local_keymap == NULL )
            next_local_keymap = bf_cur->b_mode.md_keys;
        if( theActiveView->currentWindow()->w_buf != bf_cur )
            theActiveView->currentWindow()->w_buf->set_bf();

        KeyMap *kmap = next_local_keymap;
        if( kmap != NULL )
        {
            BoundName *p = kmap->getBinding( c );
            next_local_keymap = NULL;
            if( p != NULL )
            {
                last_key_struck = c;
                if( !p->IsAKeyMap() )
                {
                    this_command = last_key_struck;
                    last_keys_struck = keys_struck;
                    keys_struck = "";

                    minibuf_body.clearMessageBody();

                    record_keystoke_history( last_keys_struck.asString(), p );
                }

                //std::cout << "process_keys:272 char 0x" << std::hex << c << std::dec << " execute " << p->b_proc_name.sdata() << std::endl;
                if( p->execute() < 0 )
                    return 0;

                if( arg_state != have_arg )
                    previous_command = int(this_command);
                //
                // if execute bound does something, start scanning a new
                // key sequence.
                //
                if( next_local_keymap == NULL )
                {
                    next_global_keymap = NULL;
                    keys_struck = "";
                    if(( bf_cur->b_mode.md_process_key_proc != NULL) )
                    {
                        execute_bound_saved_environment( bf_cur->b_mode.md_process_key_proc );
                    }
                    continue;
                }
            }
        }

        kmap = next_global_keymap;
        if( kmap != NULL )
        {
            KeyMap *lmap = next_local_keymap;

            next_global_keymap = NULL;
            next_local_keymap = NULL;

            BoundName *p = kmap->getBinding( c );
            if( p != 0 )
             {
                last_key_struck = c;
                if( !p->IsAKeyMap() )
                {
                    this_command = last_key_struck;
                    last_keys_struck = keys_struck;
                    keys_struck = "";

                    minibuf_body.clearMessageBody();

                    record_keystoke_history( last_keys_struck.asString(), p );
                }
                if( p->IsAKeyMap() || lmap == NULL )
                {
                    //std::cout << "process_keys:318 char 0x" << std::hex << c << std::dec << " execute " << p->b_proc_name.sdata() << std::endl;
                    if( p->execute() < 0 )
                        return 0;
                }
                if( arg_state != have_arg )
                    previous_command = int(this_command);
                if( next_local_keymap != NULL )
                 {
                    next_global_keymap = next_local_keymap;
                    next_local_keymap = lmap != NULL ? lmap : &null_map;
                }
                else
                {
                    if( lmap != NULL )
                        next_global_keymap = &null_map;
                    else
                        next_global_keymap = NULL;
                    next_local_keymap = lmap;
                    if( bf_cur->b_mode.md_process_key_proc != NULL )
                        execute_bound_saved_environment( bf_cur->b_mode.md_process_key_proc );
                }
                continue;
            }
            else
            {
                if( lmap != NULL )
                    next_global_keymap = &null_map;
                else
                    next_global_keymap = NULL;
                next_local_keymap = lmap;
            }
        }

        if( next_local_keymap == NULL )
        {
            illegal_operation();
            keys_struck = "";
            record_keystoke_history( last_keys_struck.asString(), NULL );
        }
        else
            next_global_keymap = &null_map;
    }
}


//
//    (setq key-processed (process-key (key-kmap) [key-result]))
//
//    process-key evals its first arg and expects a keymap as its
//    result. The keymap is used to parse input key-result is set
//    as the value, if any, returned by the function in the keymap.
//    process-key returns a boolean value, true if the keymap was
//    succesfully used. False otherwise.
//
int process_key( void )
{
    if( ! term_is_terminal )
        error( "process-key can only be used with a terminal");

    if( check_args( 1, 2 ) != 0 )
        return 0;

    ProgramNode *p = cur_exec;

    ProgramNode *result_p = NULL;
    if( p->p_nargs >= 2 )
    {
        result_p = p->arg(2);
        if( result_p->p_proc != &bound_variable_node )
        {
            error( "process-key expects its 2nd argument to be a variable" );
            return 0;
        }
    }

    unsigned int rv = exec_prog( p->arg(1) );
    if( rv != 0 || ml_err )
        return rv;

    if( next_local_keymap == 0 )
    {
        error( "process-key expects its 1st argument to be a keymap");
        return 0;
    }

    cur_exec = NULL;
    cant_1line_opt = cant_1win_opt = 1;

    //
    //    While there are key maps process key strokes
    //
    unsigned int key_parsed = 1;
    EmacsString keys_struck;

    while( next_local_keymap != NULL )
    {
        // Save the current local kmap
        KeyMap *kmap = next_local_keymap;

        // get the get_char check for EOF conditions

        int ic = get_char();
        if( ic < 0 )
        {
            can_checkpoint = 0;
            break;
        }
        EmacsChar_t c = (EmacsChar_t)ic;

        keys_struck.append( c );
        last_keys_struck = keys_struck;

        //
        //    Zero local kmap
        //    find the function to call and call it
        //
        next_local_keymap = NULL;

        BoundName *b = kmap->getBinding( c );

        if( b != NULL )
        {
            last_key_struck = c;
            if( !b->IsAKeyMap() )
                this_command = last_key_struck;

            //std::cout << "process_key:445 char 0x" << std::hex << c << std::dec << " execute " << b->b_proc_name.sdata() << std::endl;
            if( b->execute() < 0 )
                break;

            if( arg_state != have_arg )
                previous_command = int(this_command);
        }
        else
        {
            // parse error - return 0
            key_parsed = 0;
            break;
        }
    }

    // see if there is a value from the function
    if( p->p_nargs >= 2 && ! ml_err )
    {
        VariableName *v = result_p->name();

        if( !v->assignNormal( ml_value ) )
        {
            error( FormatString("Attempt to set the unbound variable \"%s\"") << v->v_name);
            return 0;
        }
    }

    ml_value = Expression( key_parsed );
    cur_exec = p;
    redo_modes = cant_1line_opt = cant_1win_opt = 1;

    return 0;
}




// read a character from the keyboard; call the redisplay if needed

static EmacsChar_t parameter_chars[100];

static int _get_char( void )
{
    int c = -1;
    int i;

    int remember = remembering;

#if DBG_KEY && DBG_TMP
    if( dbg_flags&DBG_KEY && dbg_flags&DBG_TMP )
    {
        _dbg_msg( "get_char: start" );
        {
        QueueIterator<CharElement> next( push_back_queue );
        CharElement *it;
        while( (it = next.next()) != NULL )
            _dbg_msg( FormatString("push_back_queue: %C(0x%x)") << it->ce_char << it->ce_char );
        }
        {
        QueueIterator<CharElement> next( input_queue );
        CharElement *it;
        while( (it = next.next()) != NULL )
            _dbg_msg( FormatString("input_queue: %C(0x%x)") << it->ce_char << it->ce_char );
        }
    }
#endif

    {
    CharElement *char_cell;
    {
#if DBG_QUEUE
    push_back_queue.queue_validate();
#endif

    if( (char_cell = push_back_queue.queueRemoveFirst()) != NULL )
    {
        remember = 0;
        goto having_dequeued_a_char;
    }

    if( macro_replay_next >= 0 )
    {
        if( macro_replay_next >= macro_replay_body.length() )
        {
            // run out of string so zap pointer
            macro_replay_next = -1;
            return -1;
        }
        if( ml_err || quitting_emacs )
        {
            macro_replay_next = -1;
            return -1;
        }
        c = macro_replay_body[ macro_replay_next++ ];
        goto having_found_char;
    }

    if( input_pending == 0 )
    {
        set_activity_character( 'i' );

        theActiveView->do_dsp();
        if( checkpoint_frequency > 0
        && can_checkpoint
        && keystrokes > checkpoint_frequency )
        {
            checkpoint_everything();
            keystrokes = 0;
        }
    }

    keystrokes++;

    //
    //    This is the EMACS scheduling loop -- we use
    //        input_pending to count the number of things
    //        (characters, timer interrupts, etc...)
    //        that require attention and should be processed
    //        BEFORE a redisplay occurs. We want the
    //        sub-process channel input to be the lowest
    //        priority item, though and we do not want it
    //        to affect our decision to kill the redisplay
    //        (This minimizes the impact of a lot of data
    //         coming into a buffer bound to a sub-process)
    //        so we do not count pending_channel_io in
    //        input_pending but process it separately once
    //        everything else is done.
    //
    for(;;)
    {
#if    DBG_QUEUE
        push_back_queue.queue_validate();
#endif
        if( (char_cell = push_back_queue.queueRemoveFirst()) != NULL )
        {
            remember = 0;
            goto having_dequeued_a_char;
        }

        set_activity_character( 'i' );

        while( input_pending == 0
        && push_back_queue.queue_empty() )
        {
            //
            //    If we really do something in here...
            //
#if defined( SUBPROCESSES )
            if( process_channel_interrupts() )
                //
                //    then exit the loop.
                //
                break;
#endif

            interrupt_key_struck = 0;
            if( wait_for_activity() < 0 )
                return -1;     // we are quitting emacs

            // check for timeouts
            time_call_timeout_handler();

#if    DBG_QUEUE
            push_back_queue.queue_validate();
#endif
        }

        set_activity_character( 'b' );

        //
        //    check for work other than fetching character input
        //
        EmacsWorkItem::processQueue();

        while( timer_interrupt_occurred != 0 )
            process_timer_interrupts();

        //
        //    If we do not have any characters to process, do the
        //        redisplay but do not touch the minibuffer
        //
#if    DBG_QUEUE
        push_back_queue.queue_validate();
#endif
        if( (char_cell = push_back_queue.queueRemoveFirst()) != NULL )
        {
            remember = 0;
            goto having_dequeued_a_char;
        }

#if    DBG_QUEUE
        input_queue.queue_validate();
#endif
        if( (char_cell = input_queue.queueRemoveFirst()) != NULL )
        {
            interlock_dec( &input_pending );
            if( char_cell->ce_type == CE_TYPE_DO_DSP )
            {
                // just do_dsp();
                char_cell->ce_type = CE_TYPE_FREE_CELL;
                free_queue.queueInsertAtTail( char_cell );
            }
            else
            {
                goto having_dequeued_a_char;
            }
        }

        theActiveView->do_dsp();
    }
having_dequeued_a_char:    // leave this block
    ;
    }

    switch( char_cell->ce_type )
    {
    case CE_TYPE_CHAR:
        c = char_cell->ce_char;
        break;

    // passed in by the pybemacs UI
    case CE_TYPE_PARM_LIST_FIN_CHAR:
    {
        int num_params = char_cell->ce_all_params.size();

        EmacsArray a( 0, num_params + 1 );

        // element 1,0 is the number of params
        a( 0 ) = num_params+1;

        // row 1 has the parameter strings
        for( int i=0; i<num_params; i++ )
        {
            a( i+1 ) = char_cell->ce_all_params[i];
        }

        cs_parameters.replace( a );

        c = char_cell->ce_char;
        break;

    }

    case CE_TYPE_PAR_CHAR:
    case CE_TYPE_PAR_SEP:
    case CE_TYPE_FIN_CHAR:
    {
        const int MAX_ARGS( 32 );

        Expression args[2][MAX_ARGS];

        EmacsChar_t *p = &parameter_chars[0];
        EmacsChar_t *last_p = p;
        int num_params = 0;

        while( char_cell->ce_type == CE_TYPE_PAR_SEP
        || char_cell->ce_type == CE_TYPE_PAR_CHAR )
        {
            if( char_cell->ce_type == CE_TYPE_PAR_SEP )
            {
                if( num_params >= MAX_ARGS-1 )
                    return get_char();

                args[0][ num_params ] = EmacsString
                        (
                        EmacsString::copy,
                        last_p,                 // address of string
                        p - last_p              // length of string
                        );
                args[1][ num_params ] = EmacsString
                        (
                        EmacsString::copy,
                        &char_cell->ce_char,    // address of string
                        1                       // length of string
                        );
                last_p = p;
                num_params++;
            }
            else
                *p++ = char_cell->ce_char;

            // insert at tail of queue
            char_cell->ce_type = CE_TYPE_FREE_CELL;
            free_queue.queueInsertAtTail( char_cell );
#if    DBG_QUEUE
            free_queue.queue_validate();
#endif

            interlock_dec( &input_pending );
            if( (char_cell = input_queue.queueRemoveFirst()) == NULL )
                return get_char();              // cannot happen error
        }

        //
        //    The last parameter value
        //
        args[0][ num_params ] = EmacsString
                (
                EmacsString::copy,
                last_p,             // address of string
                p - last_p          // length of string
                );

        {
        EmacsArray a
            (
            1, 2,                   // first dimension size
            0, num_params + 1       // second dimension size
            );

        // element 1,0 is the number of params
        a( 1, 0 ) = num_params+1;
        // row 1 has the parameter strings
        for( i=0; i<=num_params; i++ )
            a( 1, i+1 ) = args[0][i];

        // row 2 has the separator strings
        for( i=0; i<num_params; i++ )
            a( 2, i+1 ) = args[1][i];

        // 2, .num_params is always zero

        cs_parameters.replace( a );
        }

        c = char_cell->ce_char;
        break;
    }

    case CE_TYPE_FREE_CELL:
        _dbg_msg( "Found a free char cell in get_char" );
        debug_invoke();
        break;

    default:
        _dbg_msg( "Found an unknown char cell in get_char" );
        debug_invoke();
    }

    if( input_mode == 1 )
        gui_input_shift_state( char_cell->ce_shift );

    // insert at tail of queue
    char_cell->ce_type = CE_TYPE_FREE_CELL;
    free_queue.queueInsertAtTail( char_cell );
#if DBG_QUEUE
    free_queue.queue_validate();
#endif
having_found_char:    // leave this block
    ;
    }

#if DBG_KEY && DBG_TMP
    if( dbg_flags&DBG_KEY && dbg_flags&DBG_TMP )
        _dbg_msg( FormatString("get_char: return %C(0x%x)") << c << c );
#endif
    if( c < 0 )
        return -1;

    //
    //    Have Character now remember it if nessesary
    //
    if( remember )
    {
        key_mem.append( (EmacsChar_t)c );

        if( key_mem.length() >= MEMLEN )
        {
            error( "Keystroke memory overflow!");
            remembering = end_of_mac = 0;
            key_mem = "";
        }
    }

    set_activity_character( 'b' );

    // set true if there is any input_pending events
    keyboard_input_pending = input_queue.queueEmpty() ? 0 : 1;

    return c;
}

int get_char( void )
{
    int ch = _get_char();
#if DBG_KEY && DBG_TMP
    if( dbg_flags&DBG_KEY && dbg_flags&DBG_TMP )
        _dbg_msg( FormatString("get_char() return %C(0x%x)") << ch << ch );
#endif
    return ch;
}

// Given a keystroke sequence look up the bound_name that it is bound to
BoundName *lookup_boundname_keys( KeyMap *kmap, EmacsString keys )
{
    for( int key_index=0; key_index<keys.length(); ++key_index )
    {
        if( kmap == NULL )
            break;

        BoundName *b = kmap->getBinding( keys[ key_index ] );
        if( b == NULL )
            break;

        if( key_index == (keys.length()-1) )
            return b;

        // get the next keymap
        kmap = b->getKeyMap();
    }

    return NULL;
}

KeyMap *lookup_keymap_keys( KeyMap *kmap, EmacsString keys )
{
    for( int key_index=0; key_index < (keys.length()-1); ++key_index )
    {
        if( kmap == NULL )
            return NULL;

        BoundName *b = kmap->getBinding( keys[ key_index ] );
        if( b == NULL )
            break;

        // get the next keymap
        kmap = b->getKeyMap();
    }

    return kmap;
}

int start_remembering( void )
{
    if( remembering )
        error( "Already remembering!" );
    else
    {
        remembering = 1;
        end_of_mac = 0;
        key_mem = "";
        message( "Remembering..." );
    }

    return 0;
}

int stop_remembering( void )
{
    if( remembering )
     {
        remembering = 0;
        // remove the keystrokes that caused stop_remembering to be invoked
        key_mem.remove( end_of_mac, INT_MAX );
        message( "Keyboard macro defined." );
    }
    return 0;
}

// Execute the given command string
void exec_str( const EmacsString &s )
{
    Save<EmacsMacroString> old_replay_body( &macro_replay_body );
    Save<int> old_replay_next( &macro_replay_next );

    macro_replay_body = s;
    macro_replay_next = 0;

    redo_modes = cant_1line_opt = cant_1win_opt = 1;
    process_keys();
    redo_modes = cant_1line_opt = cant_1win_opt = 1;
}

int execute_keyboard_macro( void )
{
    if( remembering )
        error( "Sorry, you cannot call the keyboard macro while defining it.");
    else if( key_mem.isNull() )
        error( "No keyboard macro to execute.");
    else
    {
        int i;

        i = arg;
        arg = 0;
        arg_state = no_arg;

        do
        {
            exec_str( key_mem );
        }
        while( !ml_err && (i = i - 1) > 0 );
    }
    return 0;
}

int push_back_character( void )
{
    EmacsString n( get_key( global_map, ": push-back-character " ) );
    if( ! ml_err && !n.isNull() )
        for( int i=n.length() - 1; i>=0; i-- )
        {
            CharElement *char_cell = free_queue.queueRemoveFirst();
            if( char_cell == NULL )
                return 0;

            char_cell->ce_char = n[i];
            char_cell->ce_type = CE_TYPE_CHAR;

            // insert at head of queue
            char_cell->queue_insert( &push_back_queue );
        }
    return 0;
}

int top_level_command( void )
{
    quitting_emacs = 1;
    return 0;
}

int recursive_edit( void )
{
    ProgramNode *oldp;

    oldp = cur_exec;

    if( term_is_terminal )
    {
        cur_exec = 0;
        recursive_edit_depth++;
        redo_modes = cant_1line_opt = cant_1win_opt = 1;

        process_keys();

        recursive_edit_depth--;
        cur_exec = oldp;
        redo_modes = cant_1line_opt = cant_1win_opt = 1;
    }
    return 0;
}

//
//    interrupt binding routine -- If a key is bound to this procedure
//    then it is capable of causing an interrupt and when it
//    invokes this procedure it will generate an Illegal Operation.
//
int interrupt_emacs( void )
{
    interrupt_key_struck = 0;
    return 0;
}

//
//    Descriptor for "interrupt-key", so that keyboard_interrupt can
//    determine if the key struck is bound to "interrupt-key".
//
int debug_break_char = -1;    // 29 == ^]

SystemExpressionRepresentationArray cs_parameters;
SystemExpressionRepresentationIntBoolean cs_enabled;
int cs_modified;
SystemExpressionRepresentationIntBoolean cs_cvt_f_keys;
SystemExpressionRepresentationIntBoolean cs_cvt_mouse;
SystemExpressionRepresentationControlString cs_cvt_8bit_string( M_CS_CVT_8BIT );
SystemExpressionRepresentationControlString cs_cvt_csi_string( M_CS_CVT_CSI );
SystemExpressionRepresentationControlString cs_par_char_string( M_CS_PAR_CHAR );
SystemExpressionRepresentationControlString cs_par_sep_string( M_CS_PAR_SEP );
SystemExpressionRepresentationControlString cs_int_char_string( M_CS_INT_CHAR );
SystemExpressionRepresentationControlString cs_fin_char_string( M_CS_FIN_CHAR );

unsigned char cs_attr[256];

void SystemExpressionRepresentationControlString::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString value = new_value->asString();

    cs_modified++;

    //
    //    init this bit position to zero
    //
    for( int i=0; i<=255; i += 1 )
        cs_attr[i] &= ~mask;

    //
    //    set the bit for the chars specified
    //
    for( int i=0; i<value.length(); i++ )
    {
        EmacsChar_t index = value[i];
        if( index < 256 )
            cs_attr[ value[i] ] |= mask;
    }
}

void SystemExpressionRepresentationControlString::fetch_value()
{ }

enum csi_states
{
    CSI_ST_NORMAL,
    CSI_ST_ESC,
    CSI_ST_CSI
};

CharElement *_q_char( EmacsChar_t value, CE_TYPE_type type, bool shift )
{
    CharElement *char_cell;
    //
    //    queue the char
    //
    if( (char_cell = free_queue.queueRemoveFirst()) != NULL )
    {
        //
        //    wake up get_char if this is the first char in the list
        //
        char_cell->set( value, type, shift );
        interlock_inc( &input_pending );

        // insert at tail of queue
        input_queue.queueInsertAtTail( char_cell );

        conditional_wake();

        return char_cell;
    }

    return 0;
}

CharElement *_q_char( EmacsChar_t value, CE_TYPE_type type, bool shift, std::vector<int> all_params )
{
    CharElement *char_cell;
    //
    //    queue the char
    //
    if( (char_cell = free_queue.queueRemoveFirst()) != NULL )
    {
        //
        //    wake up get_char if this is the first char in the list
        //
        char_cell->set( value, type, shift, all_params );
        interlock_inc( &input_pending );

        // insert at tail of queue
        input_queue.queueInsertAtTail( char_cell );

        conditional_wake();

        return char_cell;
    }

    return 0;
}

void TerminalControl::k_input_char( int character, bool shift )
{
    if( character == -1 )
    {
        // the special call do_dsp signature
        _q_char( 0, CE_TYPE_DO_DSP, false );
        return;
    }

    if( !k_input_is_enabled )
        return;

    static csi_states csi_state( CSI_ST_NORMAL );
    static EmacsChar_t hold[100];
    static EmacsChar_t *hold_put_ptr;

    EmacsChar_t ch( character );

    // do the Ctrl-X swapping
    if( ch == ctl('X') )
        ch = (EmacsChar_t)swap_ctrl_x_char;
    else
    if( ch == (EmacsChar_t)swap_ctrl_x_char )
        ch = ctl('X');

    if( character == debug_break_char )
        debug_invoke();

    while( cs_enabled )
    {
        switch( csi_state )
        {
        case CSI_ST_NORMAL:
        {
            if( cs_attr[ch]&M_CS_CVT_CSI )
                ch = 0x9b;    // CSI

            switch( ch )
            {
            case 0x1b:
            {
                csi_state = CSI_ST_ESC;
                return;
            }
            case 0x9b:
            {
                csi_state = CSI_ST_CSI;
                hold_put_ptr = &hold[0];
                return;
            }
            default:
                goto exit_loop;
            }
            break;
        }

        case CSI_ST_ESC:
        {
            if( cs_attr[ch]&M_CS_CVT_8BIT )
            {
                csi_state = CSI_ST_NORMAL;
                ch &= 0x1f;
                ch |= 0x80;    // convert 7bit to 8bit
            }
            else
            {
                _q_char( 0x1b, CE_TYPE_CHAR, shift );
                csi_state = CSI_ST_NORMAL;
                goto exit_loop;
            }
            break;
        }

        case CSI_ST_CSI:
        {
            if( cs_attr[ch]&M_CS_PAR_SEP
            || cs_attr[ch]&M_CS_PAR_CHAR
            || cs_attr[ch]&M_CS_INT_CHAR )
            {
                if( hold_put_ptr >= &hold[sizeof(hold)-1] )
                {
                    //
                    // ignore chars on overflow
                    //
                    csi_state = CSI_ST_NORMAL;
                    return;
                }
                *hold_put_ptr++ = ch;
                return;
            }
            if( cs_attr[ch]&M_CS_FIN_CHAR )
            {
                CE_TYPE_type fin_char;

                if( ch == '~' && cs_cvt_f_keys )
                {
                    int key_num;
                    EmacsChar_t *hold_get_ptr;

                    hold_get_ptr = &hold[0];
                    key_num = 0;
                    while( hold_get_ptr != hold_put_ptr )
                    {
                        ch = *hold_get_ptr++;
                        if( ch < '0' || ch > '9' )
                        {
                            _q_char( 0x9b, CE_TYPE_CHAR, shift );
                            goto process_f_keys;
                        }
                        key_num = key_num * 10 + ch - '0';
                    }
                    {
                    int key_bank = key_num/50;
                    key_num %= 50;
                    if( key_bank != 0 )
                        key_bank++;
                    _q_char( (EmacsChar_t)(0x80+key_bank), CE_TYPE_CHAR, shift );
                    _q_char( (EmacsChar_t)(key_num + ' '), CE_TYPE_CHAR, shift );
                    }
                    csi_state = CSI_ST_NORMAL;

#if    DBG_QUEUE
                    input_queue.queue_validate();
                    free_queue.queue_validate();
#endif
                    return;
process_f_keys:
                    ;
                }
                else
                if( cs_cvt_mouse
                && ch == 'w'
                && &hold[0] != hold_put_ptr
                && hold_put_ptr[-1] == '&' )
                {
                    int event_num;
                    EmacsChar_t *hold_get_ptr;

                    hold_get_ptr = &hold[0];
                    event_num = 0;
                    while( hold_get_ptr != hold_put_ptr )
                    {
                        ch = *hold_get_ptr++;
                        if( ch < '0' || ch > '9' )
                            break;
                        event_num = event_num * 10 + ch - '0';
                    }
                    _q_char( 0x81, CE_TYPE_CHAR, shift );
                    hold_put_ptr--;// lose the "&"
                    ch = (EmacsChar_t)(event_num + 'A');
                    csi_state = CSI_ST_NORMAL;
                }
                else
                // mouse wheel code
                if( cs_cvt_mouse
                && ch == 'w'
                && &hold[0] != hold_put_ptr
                && hold_put_ptr[-1] == '#' )
                {
                    int event_num;
                    EmacsChar_t *hold_get_ptr;

                    hold_get_ptr = &hold[0];
                    event_num = 0;
                    while( hold_get_ptr != hold_put_ptr )
                    {
                        ch = *hold_get_ptr++;
                        if( ch < '0' || ch > '9' )
                            break;
                        event_num = event_num * 10 + ch - '0';
                    }
                    _q_char( 0x81, CE_TYPE_CHAR, shift );
                    hold_put_ptr--;// lose the "&"
                    ch = (EmacsChar_t)(event_num + 'M');
                    csi_state = CSI_ST_NORMAL;
                }
                else
                    _q_char( 0x9b, CE_TYPE_CHAR, shift );

                {
                EmacsChar_t par_char;
                EmacsChar_t *hold_get_ptr;

                //
                //    Find out how many parameters are present
                //
                fin_char = CE_TYPE_FIN_CHAR;
                hold_get_ptr = &hold[0];
                while( hold_get_ptr != hold_put_ptr )
                {
                    par_char = *hold_get_ptr++;
                    if( cs_attr[par_char]&M_CS_PAR_SEP )
                        _q_char( par_char, CE_TYPE_PAR_SEP, shift );
                    else if( cs_attr[par_char]&M_CS_PAR_CHAR )
                        _q_char( par_char, CE_TYPE_PAR_CHAR, shift );
                    else
                    {
                        _q_char( par_char, fin_char, shift );
                        fin_char = CE_TYPE_CHAR;
                    }
                }
                }
                _q_char( ch, fin_char, shift );
                csi_state = CSI_ST_NORMAL;
#if    DBG_QUEUE
                input_queue.queue_validate();
                free_queue.queue_validate();
#endif
                return;
            }
            // syntax error in escape sequence
            {
            EmacsChar_t *hold_get_ptr;

            _q_char( 0x9b, CE_TYPE_CHAR, shift );
            hold_get_ptr = &hold[0];
            while( hold_get_ptr != hold_put_ptr )
                _q_char( *hold_get_ptr++, CE_TYPE_CHAR, shift );
            csi_state = CSI_ST_NORMAL;
            goto exit_loop;
            }
        }
        }
    }
exit_loop:
    //
    //    Get the binding for this character (check both the
    //        local and global key maps)
    //

    //
    //    Get the Local Key kmap binding (if it is valid)
    //
    BoundName *proc = NULL;
    if( bf_cur->b_mode.md_keys != NULL )
        proc = bf_cur->b_mode.md_keys->getBinding( ch );

    //
    //    If there was no Local Key kmap binding, use the Global Key kmap
    //
    if( proc == NULL )
        proc = current_global_map->getBinding( ch );
    //
    //    If this key was bound to "interrupt-key", purge the internal
    //    typeahead buffer and declare the interrupt
    //
    if( proc == &interrupt_block )
        k_interrupt_emacs();

    //
    //    mark input pending
    //
    _q_char( ch, CE_TYPE_CHAR, shift );

#if    DBG_QUEUE
    input_queue.queue_validate();
    free_queue.queue_validate();
#endif
}

#include <iostream>

void TerminalControl::k_input_mouse( const EmacsString &keys, bool shift, const std::vector<int> &all_params )
{
    for( int i=0; i<keys.length()-1; i++ )
    {
        _q_char( keys[0], CE_TYPE_CHAR, shift );
    }
    _q_char( keys[-1], CE_TYPE_PARM_LIST_FIN_CHAR, shift, all_params );
}

void TerminalControl::k_interrupt_emacs()
{
    illegal_operation();

    //
    //    purge type ahead
    //
#if    DBG_QUEUE
    input_queue.queue_validate();
    free_queue.queue_validate();
#endif
    CharElement *char_cell;
    while( (char_cell = input_queue.queueRemoveFirst()) != NULL )
    {
        // insert at tail of queue
        free_queue.queueInsertAtTail( char_cell );

        interlock_dec( &input_pending );
    }

    interrupt_key_struck = 1;

#if    DBG_QUEUE
    input_queue.queue_validate();
    free_queue.queue_validate();
#endif
}

void TerminalControl::k_input_char_string( const unsigned char *keys, bool shift )
{
    int ch;

    while( (ch = *keys++ ) != 0 )
    {
        if( ch == '\200' )
            ch = *keys++ - '\200';
        k_input_char( ch, shift );
    }
}

int convert_key_string_command( void )
{
    concat_command();
    if( ml_err )
        return 0;

    EmacsString input( ml_value.asString() );
    EmacsString output;

    if( !convert_key_string( input, output ) )
    {
        error( "error converting key string" );
        return 0;
    }

    ml_value = output;

    return 0;
}

//
//    Convert the keys in input into output
//
int convert_key_string( const EmacsString &input, EmacsString &output )
{
#define    _qq_char( value ) output.append( (EmacsChar_t)(value) )

    EmacsChar_t ch;
    int len;
    int csi_state = CSI_ST_NORMAL;
    EmacsChar_t hold[100];
    EmacsChar_t *hold_get_ptr;
    EmacsChar_t *hold_put_ptr;
    int i;

    csi_state = CSI_ST_NORMAL;
    hold_put_ptr = &hold[0];
    len = 0;

    //
    //    If nothing special is requested just return
    //    the input as the result.
    //
    if( !cs_enabled )
    {
        output = input;
        return 1;
    }

    for( i=0; i<=input.length() - 1; i += 1 )
    {
        ch = input[ i ];
        for(;;)
        {
            switch( csi_state)
            {
            case CSI_ST_NORMAL:
            {
                if( cs_attr[ch]&M_CS_CVT_CSI )
                    ch = 0x9b;    // CSI

                switch( ch )
                {
                case 0x1b:
                    csi_state = CSI_ST_ESC;
                    goto quit_loop_1;

                case 0x9b:
                    csi_state = CSI_ST_CSI;
                    hold_put_ptr = &hold[0];
                    goto quit_loop_1;

                default:
                    _qq_char( ch );
                    goto quit_loop_1;
                }
                break;
            }
            case CSI_ST_ESC:
            {
                if( cs_attr[ch]&M_CS_CVT_8BIT )
                {
                    csi_state = CSI_ST_NORMAL;
                    ch += 0x40;    // convert 7bit to 8bit
                }
                else
                {
                    _qq_char( 0x1b );
                    _qq_char( ch );
                    csi_state = CSI_ST_NORMAL;
                    goto quit_loop_1;
                }
                break;
            }
            case CSI_ST_CSI:
            {
                if( cs_attr[ch]&M_CS_PAR_SEP
                || cs_attr[ch]&M_CS_PAR_CHAR
                || cs_attr[ch]&M_CS_INT_CHAR )
                {
                    if( hold_put_ptr >= &hold[sizeof(hold)-1] )
                    {
                        return 0;
                    }
                    *hold_put_ptr++ = ch;
                    goto quit_loop_1;
                }
                if( cs_attr[ch]&M_CS_FIN_CHAR )
                {
                    if( ch == '~' && cs_cvt_f_keys )
                    {
                        int key_num;

                        hold_get_ptr = &hold[0];
                        key_num = 0;
                        while( hold_get_ptr != hold_put_ptr )
                        {
                            ch = *hold_get_ptr++;
                            if( ch < '0' || ch > '9' )
                            {
                                _qq_char( 0x9b );
                                goto process_f_keys;
                            }
                            key_num = key_num * 10 + ch - '0';
                        }

                        {
                        int key_bank = key_num/50;
                        key_num %= 50;
                        if( key_bank != 0 )
                            key_bank++;
                        _qq_char( 0x80+key_bank );
                        _qq_char( (EmacsChar_t)(key_num + ' ') );

                        csi_state = CSI_ST_NORMAL;
                        goto quit_loop_1;
                        }

                    process_f_keys:
                        ;
                    }
                    else
                    if( cs_cvt_mouse
                    && ch == 'w'
                    && &hold[0] != hold_put_ptr
                    && hold_put_ptr[-1] == '&' )
                    {
                        int event_num;

                        hold_get_ptr = &hold[0];
                        event_num = 0;
                        while( hold_get_ptr != hold_put_ptr )
                        {
                            ch = *hold_get_ptr++;
                            if( ch < '0' || ch > '9' )
                                break;
                            event_num = event_num * 10 + ch - '0';
                        }
                        _qq_char( 0x81 );
                        hold_put_ptr = &hold_put_ptr[-1];
                        _qq_char( (EmacsChar_t)(event_num + 'A') );
                        csi_state = CSI_ST_NORMAL;
                        goto quit_loop_1;
                    }
                    else
                    // mouse wheel code
                    if( cs_cvt_mouse
                    && ch == 'w'
                    && &hold[0] != hold_put_ptr
                    && hold_put_ptr[-1] == '#' )
                    {
                        int event_num;

                        hold_get_ptr = &hold[0];
                        event_num = 0;
                        while( hold_get_ptr != hold_put_ptr )
                        {
                            ch = *hold_get_ptr++;
                            if( ch < '0' || ch > '9' )
                                break;
                            event_num = event_num * 10 + ch - '0';
                        }
                        _qq_char( 0x81 );
                        hold_put_ptr = &hold_put_ptr[-1];
                        _qq_char( (EmacsChar_t)(event_num + 'M') );
                        csi_state = CSI_ST_NORMAL;
                        goto quit_loop_1;
                    }
                    else
                    {
                        _qq_char( 0x9b );
                    }

                    EmacsChar_t par_char;
                    EmacsChar_t *hold_get_ptr;
                    //
                    //    Find out how many parameters are present
                    //
                    hold_get_ptr = &hold[0];
                    while( hold_get_ptr != hold_put_ptr )
                    {
                        par_char = *hold_get_ptr++;
                        if( ! (cs_attr[par_char]&M_CS_PAR_SEP)
                        &&  ! (cs_attr[par_char]&M_CS_PAR_CHAR) )
                            _qq_char( par_char );
                    }

                    _qq_char( ch );
                    csi_state = CSI_ST_NORMAL;
                    goto quit_loop_1;
                }

                // syntax error in escape sequence
                _qq_char( 0x9b );
                hold_get_ptr = &hold[0];
                while( hold_get_ptr != hold_put_ptr )
                    _qq_char( *hold_get_ptr++ );

                csi_state = CSI_ST_NORMAL;
                goto quit_loop_1;
            }
            }
        }
    quit_loop_1:
            ;
    }

    //
    //    See if we got an incomplete key string.
    //    If so return whats been collected up.
    //
    if( csi_state != CSI_ST_NORMAL )
    {
        if( csi_state == CSI_ST_ESC )
            _qq_char( 0x1b );

        if( csi_state == CSI_ST_CSI )
            _qq_char( 0x9b );

        hold_get_ptr = &hold[0];
        while( hold_get_ptr != hold_put_ptr )
            _qq_char( *hold_get_ptr++ );
    }

    return 1;
}

void EmacsWorkItem::processQueue( void )
{
    if( !enabled )
        return;

    EmacsWorkItem *work;

    // dispatch all the work items
    while( (work = work_queue.queueRemoveFirst()) != NULL )
    {
        interlock_dec( &input_pending );

        work->workAction();
    }
}

void EmacsWorkItem::addItem()
{
    // if the queue has not been init ignore this request
    if( work_queue.queueFirst() == NULL )
        return;

    // if already queued then exit
    if( queueNext() != NULL )
        return;

    if( enabled )
        interlock_inc( &input_pending );
    else
        queued_while_disabled++;

    work_queue.queueInsertAtTail( this );
}

bool EmacsWorkItem::enableWorkQueue( bool enable )
{
    bool result = enabled;

    enabled = enable;

    //_dbg_msg( FormatString("enableWorkQueue( %d ) queued_while_disabled=%d") << enable << queued_while_disabled );
    if( enabled )
        while( queued_while_disabled > 0 )
        {
            interlock_inc( &input_pending );
            queued_while_disabled--;
        }
    return result;
}

void init_key( void )
{
    re_init_keyboard();

    checkpoint_frequency = 300;

    //
    // Create the initial global keymaps and set up the pointers
    //
    global_map = define_keymap( "default-global-keymap"  );
    minibuf_local_ns_map = define_keymap( "Minibuf-local-NS-map"  );
    minibuf_local_map = define_keymap( "Minibuf-local-map" );
}

void re_init_keyboard( void )
{
    keyboard_input_pending = 0;

    // see if its the first time
    if( free_queue.queue_empty() )
    {
        for( int i=0; i<CHARACTER_QUEUE_SIZE; i++ )
            char_cells[i].queue_insert( &free_queue );
    }
    else
        // its not the first time
    {
        CharElement *char_cell;
        while( !input_queue.queue_empty() )
        {
            char_cell = input_queue.queueRemoveFirst();
            char_cell->ce_type = CE_TYPE_FREE_CELL;
            free_queue.queueInsertAtTail( char_cell );
        }
        while( !push_back_queue.queue_empty() )
        {
            char_cell = push_back_queue.queueRemoveFirst();
            char_cell->ce_type = CE_TYPE_FREE_CELL;
            free_queue.queueInsertAtTail( char_cell );
        }
    }
}
