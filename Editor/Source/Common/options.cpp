//    Copyright (c) 1982-1994
//        Barry A. Scott and Nick Emery
//
// A random assortment of commands: help facilities, macros, key bindings
// and package loading.
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

struct key_scan_history
{
    struct key_scan_history *hist_prev;
    KeyMap *hist_this;
};

int change_directory( void );
int describe_key( void );
int local_binding_of( void );
int global_binding_of( void );
static int binding_of_inner(int local_bind);
void scan_map( KeyMap *kmap, void (*proc)( BoundName *b, EmacsString &keys, int range ), int fold_case );
static void scan_map_inner( KeyMap *kmap, void (*proc)( BoundName *b, EmacsString &keys, int range ), struct key_scan_history *history, EmacsString keys, int fold_case );
static void describe1( BoundName *b, EmacsString &keys, int range );
int describe_bindings( void );
int define_keyboard_macro( void );
int define_string_macro( void );
int bind_to_key( void );
int local_bind_to_key( void );
static int bind_to_key_inner(int local_bind);
int remove_binding( void );
int use_global_map( void );
int use_local_map( void );
static void initialize_local_map( void );
static BoundName *autodefinekeymap( void );
static void perform_bind( KeyMap **tbl, BoundName *name );
int remove_local_binding( void );
int remove_all_local_bindings( void );
int execute_extended_command( void );
int define_keymap_command( void );
int auto_load( void );
int dump_stack_trace( void );
int error_handler( void );
static int break_point( ProgramNode *p, int larg );
static int trace( ProgramNode *p, int larg );
static int breaktrace( ProgramNode *p, BoundName *b, int larg );
int set_break_point( void );
int list_break_points( void );
int decompile_current_line( void );
int decompile_function( void );
static EmacsString decompile( ProgramNode *p, int larg, int indent, int depth );
static void decompile_inner( ProgramNode *p, int depth, int indent );

static ProgramNode *trace_cur_exec;
static int trace_larg;

BoundName *trace_proc;
BoundName *break_proc;
BoundName *error_proc;
unsigned int last_auto_keymap;    // Index of last auto keymap
static EmacsString decompile_buffer;    // The Trace back buffer
SystemExpressionRepresentationStringReadOnly current_function;
SystemExpressionRepresentationStringReadOnly current_break;
static int trace_mode_count;

const EmacsString sexpr_defun( "s-expr" );

int change_directory( void )
{
    EmacsDirectoryTable dir_table;
    EmacsString new_dir;
    getescword( dir_table., ": change-directory ", new_dir );

    // try the change
    if( chdir_and_set_global_record( new_dir ) < 0 )
        error( FormatString("Cannot change to directory %s") << new_dir );

    return 0;
}

//
// Given a sequence of keystrokes (at "keys" for "len" characters) return a
// printable representation of them -- with ESCs for escapes, and similar rot
//
EmacsString key_to_str( const EmacsString &keys, bool replace_key_names )
{
    EmacsString buf;

    if( keys.isNull() )
        return "[Unknown keys]";

    for( int i=0; i<keys.length(); i++ )
    {
        EmacsString value;
        int matched = 0;
        unsigned char ch;

        if( replace_key_names )
            matched = PC_key_names.keyNameOfValue( keys( i, INT_MAX ), value );
        if( matched > 0 )
        {
            buf.append( value );
            i = i + matched - 1;
        }
        else
    {
        if( buf.length() > 100 )
            return u_str("[long key sequence]");
        if( (ch = keys[i]) == ctl('[') )
        {
            buf.append( "ESC" );
        }
        else
        {
            if( ch <= 31 )
            {
                buf.append( '^' );
                buf.append( (unsigned char)(ch + '@') );
            }
            else if( (ch >= 32 && ch <= 126)
            || (ch >= 161 && ch <= 254) )
                buf.append( ch );
            else if( ch == 127 )
            {
                buf.append( "^?" );
            }
            else
            {
                buf.append( '\\' );
                buf.append( (unsigned char)(((ch>>6)&7) + '0') );
                buf.append( (unsigned char)(((ch>>3)&7) + '0') );
                buf.append( (unsigned char)((ch&7) + '0') );
            }
        }
    }
        if( i < (keys.length()-1) )
            buf.append( '-' );
    }

    return buf;
}



int describe_key( void )
{
    const char *where_bound = "globally";

    EmacsString keyp = get_key( current_global_map, ": describe-key " );
    if( keyp.isNull() || ml_err )
    {
        void_result();
        return 0;
    }

    BoundName **p = lookup_keys( bf_cur->b_mode.md_keys, keyp );
    if( p != NULL && p[0] == NULL )
        p = NULL;
    if( p != NULL )
        where_bound = "locally";
    else
        p = lookup_keys( current_global_map, keyp );

    if( p == NULL || p[0] == NULL )
    {
        error( FormatString("%s is not bound to anything") << key_to_str( keyp ) );
        return 0;
    }

    EmacsString mlisp;
    BoundName *b = p[0];

    if( b->b_proc_name == sexpr_defun )
        mlisp = decompile( b->getProcedure(), 1, 0, 1 );

    const char *type = b->commandTypeName();
    // skip the first "a " or "an "
    while( *type != ' ' )
        type++;
    type++;
    EmacsString line( FormatString("%s is %s bound to the %s \"%s\"") <<
                key_to_str( keyp ) <<
                where_bound <<
                type <<
                (!mlisp.isNull() ? mlisp : b->b_proc_name)
            );

    if( interactive() )
    {
        message( line );
        void_result();
    }
    else
        ml_value = line;

    return 0;
}


int local_binding_of( void )
{
    return binding_of_inner( 1 );
}

int global_binding_of( void )
{
    return binding_of_inner( 0 );
}

static int binding_of_inner(int local_bind)
{
    unsigned char *func_name;
    KeyMap *kmap;
    BoundName *b;
    BoundName **ref_b;

    if( local_bind )
    {
        kmap = bf_cur->b_mode.md_keys;
        func_name = u_str(": local-binding-of ");
    }
    else
    {
        kmap = current_global_map;
        func_name = u_str(": global-binding-of ");
    }

    EmacsString key = get_key( current_global_map, func_name );
    if( key.isNull() || ml_err )
        return 0;

    ref_b = lookup_keys( kmap, ml_value.asString() );
    if( ref_b == NULL || ref_b[0] == NULL )
    {
        ml_value = EmacsString("nothing");
        return 0;
    }

    b = ref_b[0];
    if( b->b_proc_name == sexpr_defun )
    {
        EmacsString str = decompile( b->getProcedure(), 1, 0, 1 );
        if( !str.isNull() )
        {
            ml_value = str;

            return 0;
        }
    }

    ml_value = b->b_proc_name;

    return 0;
}



//
// Recursively scan a keymap tree. It gets passed a pointer to a map and a
// function. For each bound_name the function is called with these
// parameters: the bound_name, the keystrokes leading to it
// (as a unsigned char * and
// an int) and a count of the number of following keys that are bound to the
// same bound_name. A run of equal bound_names in a keymap is only passed to
// the procedure once.
//
void scan_map
    (
    KeyMap *kmap,
    void (*proc)( BoundName *b, EmacsString &keys, int range ),
    int fold_case
    )
{
    EmacsString keys;
    if( kmap != 0 )
        scan_map_inner( kmap, proc, 0, keys, fold_case );
}

static void scan_map_inner
    (
    KeyMap *kmap,
    void (*proc)( BoundName *b, EmacsString &keys, int range ),
    struct key_scan_history *history,
    EmacsString keys,        // get a copy not a ref - important
    int fold_case
    )
{
    struct key_scan_history hist;
    BoundName *b;
    int c, c2;

    hist.hist_prev = history;
    hist.hist_this = kmap;

    int last_char = keys.length();
    keys.append( (unsigned char)0 );

    c = 0;
    while( c <= 255 )
    {
        c2 = c + 1;
        if( (b = kmap->getBinding( c )) != NULL
        && (! fold_case || ! isupper(c)
            || (b != kmap->getBinding( (unsigned char)tolower((unsigned char)c) ))) )
        {
            keys[last_char] = (unsigned char)c;

            while( c2 < 256
            && (kmap->getBinding( c2 ) == b) )
                c2++;
            proc( b, keys, c2 - c );
            if( b->getKeyMap() != NULL )
            {
                struct key_scan_history *h;
                h = history;
                while( h != 0 && h->hist_this != kmap )
                    h = h->hist_prev;
                if( h == 0 )
                    scan_map_inner( b->getKeyMap(), proc, &hist, keys, fold_case );
            }
        }
        c = c2;
    }
}

// Helper function for DescribeBindings -- inserts one line of info for the
// given bound_name
static void describe1
    (
    BoundName *b,
    EmacsString &keys,
    int range
    )
{
    EmacsString s;
    int len = keys.length();

    s = key_to_str( keys, arg == 1 );
    bf_cur->ins_cstr( s );

    int indent = s.length();

    if( range > 1 )
    {
        keys[ len - 1 ] = (unsigned char)(keys[ len - 1 ] + range - 1);
        bf_cur->ins_cstr( "..", 2 );
        s = key_to_str( keys, arg == 1 );
        bf_cur->ins_cstr( s );
        indent = indent + s.length() + 2;
        keys[len - 1] = (unsigned char)(keys[ len - 1 ] - range - 1);
    }

    bf_cur->ins_cstr( "                                ", 32 - min( indent, 31 ) );
    if( b->b_proc_name == sexpr_defun )
    {
        EmacsString str = decompile( b->getProcedure(), 1, 0, 1 );
        bf_cur->ins_cstr( str );
    }
    else
        bf_cur->ins_cstr( b->b_proc_name );

    bf_cur->ins_cstr( "\n", 1 );
}


int describe_bindings( void )
{
    KeyMap *local_map;
    local_map = bf_cur->b_mode.md_keys;
    EmacsBuffer::scratch_bfn( EmacsString("Help"), 1 );

    bf_cur->ins_str("Global Bindings (" );
    bf_cur->ins_cstr( current_global_map->k_name );
    bf_cur->ins_str("):\nKey                Binding\n"
        "---                -------\n" );

    scan_map( current_global_map, describe1, 1 );

    if( local_map != 0 )
    {
        bf_cur->ins_str( "\nLocal bindings (" );
        bf_cur->ins_cstr( local_map->k_name );
        bf_cur->ins_str( "):\n" );

        scan_map( local_map, describe1, 0 );
    }

    beginning_of_file();
    bf_cur->b_modified = 0;
    return 0;
}



static EmacsString msg_not_allowed("Not allowed to define a macro while remembering.");
int define_keyboard_macro( void )
{
    if( remembering != 0 )
    {
        error( msg_not_allowed );
        return 0;
    }
    if( key_mem.isNull() )
    {
        static EmacsString msg( "No keyboard macro defined.");
        error( msg );
        return 0;
    }

    EmacsString name = getnbstr( u_str(": define-keyboard-macro ") );

    BoundName *cmd = BoundName::find( name );
    if( cmd != NULL )
        cmd->replaceInside( key_mem );
    else
        cmd = EMACS_NEW BoundName( name, key_mem );

    return 0;
}



int define_string_macro( void )
{
    if( remembering != 0 )
    {
        error( msg_not_allowed );
        return 0;
    }

    EmacsString name( getnbstr( EmacsString(": define-string-macro ") ) );
    if( name.isNull() )
        return 0;

    EmacsMacroString body( getstr( FormatString(": define-string-macro %s body: ") << name ) );

    BoundName *cmd = BoundName::find( name );;
    if( cmd != NULL )
        cmd->replaceInside( body );
    else
        cmd = EMACS_NEW BoundName( name, body );

    return 0;
}



int bind_to_key( void )
{
    return bind_to_key_inner( 0 );
}

int local_bind_to_key( void )
{
    return bind_to_key_inner( 1 );
}

static int bind_to_key_inner(int local_bind)
{
    EmacsString string;
    BoundName *b = NULL;

    const char *func_name = local_bind ? "local-bind-to-key" : "bind-to-key";

    if( cur_exec == NULL )
    {
        b = BoundName::get_word_interactive( FormatString(": %s name: ") << func_name );
        if( b == NULL )
            return 0;

        string = b->b_proc_name;
    }
    else
    {
        last_arg_used++;
        if( ! string_arg( last_arg_used + 1 ) )
        {
            error( FormatString("%s expects a string as argument 1") << func_name );
            return 0;
        }

        string = ml_value.asString();
        if( string.isNull() )
        {
            error( FormatString("%s given a null string as argument 1") << func_name );
            return 0;
        }

        if( string[0] == '(' )
        {
            ProgramNode *p = ProgramNode::parse_mlisp_line( string );
            if( p != 0 )
            {
                b = EMACS_NEW BoundNameNoDefine( sexpr_defun, p );
                if( b == NULL )
                {
                    delete p;
                    return 0;
                }
            }
        }
        else
        {
            b = BoundName::find( string );
        }
    }
    if( b == NULL )
    {
        error( FormatString("%s expects %s to be an MLisp function or expression") <<
                func_name << string );
        return 0;
    }

    KeyMap *k;
    if( local_bind )
    {
        initialize_local_map();
        k = bf_cur->b_mode.md_keys;
    }
    else
        k = current_global_map;

    EmacsString c = get_key( k, FormatString(": %s name: %s key: ") << func_name << string );
    if( c.isNull() )
    {
        free_sexpr_defun( b );
        return 0;
    }

    perform_bind( &k, b );
    return 0;
}



void free_sexpr_defun( BoundName *b )
{
    if( b != NULL && b->b_proc_name == sexpr_defun )
        delete b;
}

int remove_binding( void )
{
    BoundName **ref_b;    // bound_name

    EmacsString c = get_key( current_global_map, u_str(": remove-binding ") );
    if( c.length() > 0 && ! ml_err )
    {
        ref_b = lookup_keys( current_global_map, ml_value.asString() );
        if( ref_b != NULL && ref_b[0] != NULL )
        {
            free_sexpr_defun( ref_b[0] );
            ref_b[0] = NULL;
        }
    }
    void_result ();
    return 0;
}



int use_global_map( void )
{
    BoundName *proc = getword( BoundName::, EmacsString(": use-global-map ") );
    if( proc == NULL )
        return 0;

    if( proc->getKeyMap() == NULL )
        error( FormatString("%s is not a keymap.") << proc->b_proc_name );
    else
        current_global_map = proc->getKeyMap();

    next_global_keymap = next_local_keymap = NULL;

    return 0;
}



int use_local_map( void )
{
    BoundName *proc = getword( BoundName::, ": use-local-map " );
    if( proc == NULL )
        return 0;

    if( proc->getKeyMap() == NULL )
        error( FormatString("%s is not a keymap.") << proc->b_proc_name );
    else
        bf_cur->b_mode.md_keys = proc->getKeyMap();

    next_global_keymap = next_local_keymap = NULL;

    return 0;
}

// The following procedure is a horrible compatibility hack. It
// is called to ensure that the local map exists and that the ESC and ^X
// slots in it are non-empty. If they are empty, then they are forced to be
// bound to keymaps.
static void initialize_local_map( void )
{
    if( bf_cur->b_mode.md_keys == NULL
    || bf_cur->b_mode.md_keys->getBinding( ctl('[') ) == NULL
    || bf_cur->b_mode.md_keys->getBinding( ctl('X') ) == NULL )
    {
        EmacsString esc_key; esc_key.append( char( ctl('[') ) );
        ml_value = esc_key;
        if( bf_cur->b_mode.md_keys == NULL
        || bf_cur->b_mode.md_keys->getBinding( ctl('[') ) == NULL )
            perform_bind( &bf_cur->b_mode.md_keys, 0 );

        EmacsString ctrl_x_key; ctrl_x_key.append( char( ctl('X') ) );
        ml_value = ctrl_x_key;
        if( bf_cur->b_mode.md_keys == NULL
        || bf_cur->b_mode.md_keys->getBinding( ctl('X') ) == NULL )
            perform_bind( (KeyMap **)&bf_cur->b_mode.md_keys, 0 );
    }
}

static BoundName *autodefinekeymap( void )
{
    EmacsString auto_name;

    do
    {
        auto_name = FormatString("~~auto-defined-keymap-%d") << last_auto_keymap;
        last_auto_keymap++;
    }
    while( BoundName::find( auto_name ) != NULL );

    if( define_keymap( auto_name ) == NULL )
        return 0;

    return BoundName::find( auto_name );
}

static void perform_bind
    (
    KeyMap **tbl,
    BoundName *name
    )
{
    BoundName *b;
    KeyMap *k;

    EmacsString p;

    try
    {
        switch( ml_value.exp_type() )
        {
        case ISINTEGER:
            p.append( char( ml_value.asInt() ) );
            break;
        default:
            p = ml_value.asString();
            break;
        }
    }
    catch(...)
    {
        error("Bind expecting an integer or a string");
        return;
    }

    //
    //    If the keymap is missing completely
    //    create a new one. The binding is not
    //    important yet.
    //
    if( tbl[0] == NULL )
    {
        b = autodefinekeymap();
        if( b == NULL )
            return;
        tbl[0] = b->getKeyMap();
    }

    //
    // init the keymap pointer
    //
    k = tbl[0];
    b = NULL;
    //
    // create the the keymap path if required and
    // leave k pointing at the last keymap.
    //
    int level = p.length();
    int i;
    for( i=0; i<=level-2; i++ )
    {
        unsigned char ch;
        ch = p[i];
        b = k->getBinding( ch );
        if( b == NULL || b->getKeyMap() == NULL )
        {
            b = autodefinekeymap();
            if( b == NULL )
                return;
            k->addBinding( ch, b );
        }

        k = b->getKeyMap();
    }

    // final bind the procedure to the final keymap
    k->addBinding( p[i], name );

    void_result();
}

int remove_local_binding( void )
{

    initialize_local_map();
    EmacsString c = get_key( bf_cur->b_mode.md_keys, u_str(": remove-local-binding ") );
    if( !c.isNull() && ! ml_err )
    {
        BoundName **ref_b = lookup_keys( bf_cur->b_mode.md_keys, ml_value.asString() );
        if( ref_b != NULL && ref_b[0] != NULL )
        {
            free_sexpr_defun( ref_b[0] );
            ref_b[0] = NULL;
        }
    }

    void_result();

    return 0;
}



int remove_all_local_bindings( void )
{
    KeyMap *k = bf_cur->b_mode.md_keys;
    if( k != NULL )
        k->removeAllBindings();

    return 0;
}

int execute_extended_command( void )
{
    static const EmacsString cmd_prompt( ": " );
    int larg = arg;
    arg = 1;
    BoundName *p = NULL;
    if( cur_exec == NULL )
        p = BoundName::get_word_interactive( cmd_prompt );
    else
        p = BoundName::get_word_mlisp();
    arg = larg;
    if( p == NULL )
        return 0;

    int rv = p->execute();

    if( interactive() && ! ml_err && ml_value.exp_type() != ISVOID )
        switch( ml_value.exp_type() )
        {
        case ISWINDOWS:
            message( "MLisp function return windows" );
            break;
        case ISINTEGER:
            message( FormatString("MLisp function returned %d") << ml_value.asInt() );
            break;
        case ISSTRING:
            message( FormatString("MLisp function returned \"%s\"") << ml_value.asString() );
            break;
        case ISMARKER:
        {
            Marker *m = ml_value.asMarker();
            if( m != NULL )
            {
                message( FormatString("MLisp function returned Marker (\"%s\", %d)") <<
                        m->m_buf->b_buf_name <<
                        m->get_mark() );
            }
        }
            break;
        default:
            error( "MLisp function returned a bizarre result!" );
        }

    return rv;
}



int define_keymap_command( void )
{
    EmacsString mapname = getnbstr( ": define-keymap " );

    define_keymap( mapname );
    return 0;
}



int auto_load( void )
{
    EmacsString comname = getnbstr( ": autoload procedure " );
    EmacsString filename( getnbstr( FormatString(": autoload procedure %s from file ") << comname ) );

    BoundName *cmd = BoundName::find( comname );
    if( cmd != NULL )
        cmd->replaceInside( filename );
    else
        cmd = EMACS_NEW BoundName( comname, filename );

    return 0;
}

extern int in_trace;

BoundName *active_boundname = NULL;


//
//    protect the current function name from a throw expection
//
class SaveCurrentFunction
{
public:
    SaveCurrentFunction() : saved_value( current_function.asString() ) { }
    ~SaveCurrentFunction() { current_function = saved_value; }
private:
    const EmacsString saved_value;
};


int BoundName::execute(void)
{
#if DBG_ALLOC_CHECK
    if( dbg_flags&DBG_ALLOC_CHECK )
        emacs_heap_check();
#endif

    //
    // See if there is any input activity to process
    //
    theActiveView->k_check_for_input();

    if( arg_state == no_arg )
        arg = 1;
    else if( arg_state == prepared_arg )
        arg_state = have_arg;

    // start with a void expression
    ml_value.release_expr();

    active_boundname = this;

    if( b_break && in_trace == 0 )
    {
        int larg( arg );
        ProgramNodeNode dummy( this, 0 );
        Save<ProgramNode *> old_cur_exec( &cur_exec );

        current_break = b_proc_name;
        if( cur_exec == NULL )
            cur_exec = &dummy;

        break_point( cur_exec, larg );
        arg = larg;
    }


    if( implementation == NULL )
    {
        error( FormatString("%s has not been defined yet.") << b_proc_name );
        return 0;
    }

    int rv = 0;

    try{
        //
        // only save the current-function name if we are not
        // in trace at the moment and the bound name is a procedure
        //
        if( in_trace == 0 && implementation->getProcedure() != NULL )
        {
            // save the current-function for MLisp procedures
            SaveCurrentFunction old_current_function;
            current_function = b_proc_name;

            rv = implementation->execute();
        }
        else
            rv = implementation->execute();
    }
    catch( EmacsException e )
    {
        if( !ml_err )
            error("Unknown exception thrown");
    }

    return rv;
}

int BoundNameMacro::execute(void)
{
    Save<ProgramNode *> lcur_exec( &cur_exec );

    cur_exec = NULL;

    int larg = arg;

    do
    {
        exec_str( b_body );
        larg--;
    }
    while( ! ml_err && larg > 0 );

    if( arg_state != prepared_arg )
    {
        arg_state = no_arg;
        arg = 1;
    }

    return 0;
}

int BoundNameProcedure::execute(void)
{
    int rv = 0;

    ExecutionStack stack_element( arg_state != no_arg, arg );

    arg_state = no_arg;

    if( in_trace == 0 )
    {
        if( trace_mode != 0 )
        {
            trace( cur_exec, arg );

            int old_trace_mode = trace_mode;
            int current_trace_mode_count = trace_mode_count;
            int current_trace_into = trace_into;

            if( current_trace_into == 0 )
                trace_mode = int(0);

            rv = exec_prog( b_prog );

            if( current_trace_into == 0
            && current_trace_mode_count == trace_mode_count )
                trace_mode = old_trace_mode;
        }
        else
        {
            rv = exec_prog( b_prog );
        }

    }
    else
        rv = exec_prog( b_prog );


    if( arg_state != prepared_arg )
    {
        arg_state = no_arg;
        arg = 1;
    }

    return rv;
}

int BoundNameAutoLoad::execute(void)
{
    BoundName *container = active_boundname;    // we need to know who we are contained inside of
    BoundNameInside *us = container->implementation;// save us
    container->implementation = NULL;        // prevent us being deleted

    emacs_assert( us == this );

    int larg = arg;
    enum arg_states lstate = arg_state;

    arg = 0;
    arg_state = no_arg;
    int rv = execute_mlisp_file( b_module, 0 );
    if( ! ml_err && rv == 0 )
        if( container->implementation == us )
        {
            error( FormatString("%s was supposed to be defined by autoloading %s, but it was not.") <<
                container->b_proc_name << b_module );
            us = NULL;    // don't delete us
        }
        else
        {
            arg = larg;
            arg_state = lstate;
            rv = container->execute();
        }

    if( arg_state != prepared_arg )
    {
        arg_state = no_arg;
        arg = 1;
    }

    if( us != NULL )
        delete us;    // YES this is not 100% C++
    return rv;
}

int BoundNameKeymap::execute(void)
{
    next_local_keymap = b_keymap;

    return 0;
}

int BoundNameBuiltin::execute(void)
{
    if( trace_mode != 0 && in_trace == 0 )
    {
        Save<int> larg( &arg );

        trace( cur_exec, arg );
    }


    int rv = b_builtin();

    if( arg_state != prepared_arg )
        last_proc = b_builtin;

    if( dot < bf_cur->first_character() )
        set_dot( bf_cur->first_character() );
    if( dot > bf_cur->num_characters() )
        set_dot( bf_cur->num_characters() + 1 );

    if( arg_state != prepared_arg )
    {
        arg_state = no_arg;
        arg = 1;
    }

    return rv;
}

int BoundNameExternalFunction::execute(void)
{
    if( arg_state == no_arg )
        arg = 1;
    else if( arg_state == prepared_arg )
        arg_state = have_arg;

#if defined(CALL_BACK)
    if( trace_mode != 0 && in_trace == 0 )
        trace( cur_exec, arg );
    int rv = activate_external_function();
#else
    error( FormatString("external function not supported %s") << active_boundname->b_proc_name );
    int rv = 0;
#endif
    if( arg_state != prepared_arg )
    {
        arg_state = no_arg;
        arg = 1;
    }

    return rv;
}

// Execute a bound procedure with the current execution environment saved
int execute_bound_saved_environment( BoundName *p )
{
    Save<ProgramNode *> saved_cur_exec( &cur_exec );    // Saved cur_exec
    Save<Expression> saved_ml_value( &ml_value );       // Saved ml_value
    Save<int> saved_arg( &arg );                        // Saved arg
    Save<arg_states> saved_arg_state( &arg_state );     // Saved arg_state

    cur_exec = 0;
    arg = 0;
    arg_state = no_arg;

    //
    //    Execute the procedure
    //
    int rv = p->execute();
    return rv;
}



void dump_mlisp_stack(void)
{
    if( ml_err )
        bf_cur->ins_cstr( FormatString("Message:  %s\n") << error_message_text.asString() );

    bf_cur->ins_cstr( FormatString("Executing %s: ") << current_function.asString() );
    print_expr( cur_exec, 1 );
    bf_cur->ins_str( "\n" );

    ExecutionStack *p = execution_root;
    while( p != NULL && p->es_dyn_parent != NULL )
    {
        bf_cur->ins_cstr( FormatString("          %s: ") << p->es_dyn_parent->es_cur_name );
        print_expr( p->es_cur_exec, 1 );
        bf_cur->ins_str( "\n" );
        p = p->es_dyn_parent;
    }
}

// Dump a stack trace to the stack trace buffer -- handles recursive calls
int dump_stack_trace( void )
{
    EmacsBufferRef old( bf_cur );
    bool set_window = theActiveView->currentWindow()->w_buf == bf_cur;

    EmacsBuffer::scratch_bfn( "Stack trace", 1 );

    dump_mlisp_stack();

    set_dot( 1 );

    bf_cur->b_modified = 0;
    old.set_bf();
    if( set_window )
        theActiveView->window_on( bf_cur );
    return 0;
}


void SystemExpressionRepresentationBoundName::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString name = new_value->asString();

    if( name.isNull() )
        *procedure = NULL;
    else
    {
        BoundName *proc = BoundName::find( name );
        if( proc == NULL )
        {
            error( FormatString("%s has not been define yet") << name );
            return;
        }
        *procedure = proc;
    }
}

void SystemExpressionRepresentationBoundName::fetch_value(void)
{
    if( *procedure == NULL )
        exp_string = "";
    else
        exp_string = (*procedure)->b_proc_name;
}

void SystemExpressionRepresentationProcessKeyHook::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString name = new_value->asString();

    if( name.isNull() )
        bf_cur->b_mode.md_process_key_proc = NULL;
    else
    {
        BoundName *proc = BoundName::find( name );
        if( proc == NULL )
        {
            error( FormatString("%s has not been define yet") << name );
            return;
        }
        bf_cur->b_mode.md_process_key_proc = proc;
    }
}

void SystemExpressionRepresentationProcessKeyHook::fetch_value(void)
{
    if( bf_cur->b_mode.md_process_key_proc == NULL )
        exp_string = "";
    else
        exp_string = bf_cur->b_mode.md_process_key_proc->b_proc_name;
}

void SystemExpressionRepresentationAutoFillHook::assign_value( ExpressionRepresentation *new_value )
{
    EmacsString name = new_value->asString();

    if( name.isNull() )
        bf_cur->b_mode.md_auto_fill_proc = NULL;
    else
    {
        BoundName *proc = BoundName::find( name );
        if( proc == NULL )
        {
            error( FormatString("%s has not been define yet") << name );
            return;
        }
        bf_cur->b_mode.md_auto_fill_proc = proc;
    }
}

void SystemExpressionRepresentationAutoFillHook::fetch_value(void)
{
    if( bf_cur->b_mode.md_auto_fill_proc == NULL )
        exp_string = "";
    else
        exp_string = bf_cur->b_mode.md_auto_fill_proc->b_proc_name;
}

int error_handler( void )
{
    int rv;
    rv = 0;
    if( in_trace == 0 && error_proc != 0 )
    {
        ProgramNode *old_cur_exec = cur_exec;
        EmacsString msg = error_message_text.asString();

        if( cur_exec == NULL )
        {
            ProgramNodeNode dummy( NULL, 0 );
            cur_exec = &dummy;
        }

        rv = breaktrace( cur_exec, error_proc, arg );

        cur_exec = old_cur_exec;
        error_message_text = msg;
    }

    return rv;
}

static int break_point
    (
    ProgramNode *p,
    int larg
    )
{
    if( p != NULL )
        return breaktrace( p, break_proc, larg );
    else
        return 0;
}

static int trace
    (
    ProgramNode *p,
    int larg
    )
{
    if( p != 0 )
    {
        BoundName *n;

        n = p->p_proc;
        // do not call the trace hook if the
        // break hook will have been called already
        if( ! (n->b_break && break_proc != 0) )
            return breaktrace( p, trace_proc, larg );
    }
    return 0;
}

static int breaktrace
    (
    ProgramNode *p,
    BoundName *b,
    int larg
    )
{
    int rv;
    rv = 0;
    if( b != 0 )
    {
        int old_trace_larg;
        ProgramNode *old_trace_cur_exec;

        old_trace_larg = trace_larg;
        old_trace_cur_exec = trace_cur_exec;
        trace_cur_exec = p;
        trace_larg = larg;
        in_trace++;
        rv = execute_bound_saved_environment( b );
        in_trace--;
        trace_cur_exec = old_trace_cur_exec;
        trace_larg = old_trace_larg;
    }
    return rv;
}



int set_break_point( void )
{
    BoundName *proc = getword( BoundName::, ": breakpoint " );
    if( proc != NULL )
    {
        int val = getnum( FormatString(": breakpoint %s ") << proc->b_proc_name );
        proc->b_break = val != 0;
    }
    return 0;
}



int list_break_points( void )
{
    EmacsBufferRef old( bf_cur );
    EmacsBuffer::scratch_bfn( "Breakpoint list", interactive() );

    bf_cur->ins_str( "    Name\n    ----\n");

    for( int index=0; index<BoundName::name_table.entries(); index++ )
    {
        BoundName *proc = BoundName::name_table.value( index );
        if( proc->b_break  )
        {
            bf_cur->ins_cstr( u_str("   "), 3 );
            bf_cur->ins_cstr( proc->b_proc_name );
            bf_cur->ins_cstr( u_str("\n"), 1 );
        }
    }
    bf_cur->b_modified = 0;
    set_dot( 1 );
    old.set_bf();
    theActiveView->window_on( bf_cur );
    return 0;
}



int decompile_current_line( void )
{
    EmacsString decompiled_line;
    if( trace_cur_exec == 0 )
        if( interactive() )
            decompiled_line = "";
        else
            decompiled_line = decompile( cur_exec, arg, 0, 1);
    else
        decompiled_line = decompile( trace_cur_exec, trace_larg, 0, 1 );

    if( decompiled_line.isNull() )
        error( "Not enough memory." );
    else
        ml_value = decompiled_line;

    return 0;
}

static unsigned int current_line;
int decompile_function( void )
{
    BoundName *p = getword( BoundName::, ": decompile-mlisp-function " );
    if( p == NULL )
        return 0;

    current_line = 0;

    if( p->getProcedure() == NULL )
    {
        const char *type = p->commandTypeName();
        error( FormatString("%s is %s") << p->b_proc_name << type );
    }
    else
    {
        EmacsString str = decompile( p->getProcedure(), 1, 1, 3 );

        int old_dot = dot;

        bf_cur->ins_cstr( u_str("(defun\n    ("), 12 );
        bf_cur->ins_cstr( p->b_proc_name );
        bf_cur->ins_cstr( u_str("\n"), 1 );
        bf_cur->ins_cstr( str );
        bf_cur->ins_cstr( u_str("    )\n)\n"), 8 );

        set_dot( old_dot + (current_line == 0 ?  0 : current_line + 12 + 1 + p->b_proc_name.length() ) );
    }

    return 0;
}



static void decompile_put_str( EmacsString str )
{
    decompile_buffer.append( str );
}

static void decompile_put_char( int ch )
{
    decompile_buffer.append( (unsigned char)ch );
}

static void decompile_put_int( int i )
{
    decompile_buffer.append( FormatString("%d") << i );
}


static void decompile_string( EmacsString &str )
{
    unsigned char buf[4];

    int len = str.length();

    for( int i=0; i<len; i++ )
    {
        int ch;
        ch = str[i];
        if( ch < ' ' )
        {
            unsigned char *x;
            switch( ch )
        {
            case ctl('J'):    x = u_str("\\n"); break;
            case ctl('H'):    x = u_str("\\b"); break;
            case ctl('M'):    x = u_str("\\r"); break;
            case ctl('I'):    x = u_str("\\t"); break;
            case ctl('['):    x = u_str("\\e"); break;
            case 127:    x = u_str("\\^?"); break;
            default:
            {
                x = buf;
                buf[0] = '\\';
                buf[1] = '^';
                buf[2] = (unsigned char)(ch + '@');
                buf[3] = 0;
            }
        }

            decompile_put_str( x );
        }
        else
            if( ch == '\\' )
                decompile_put_char( '\\' );
            else
                decompile_put_char( ch );
    }
}



static EmacsString decompile
    (
    ProgramNode *p,
    int larg,
    int indent,
    int depth
    )
{
    // Set up the string stream
    if( p == NULL )
        return "";

    decompile_buffer = "";

    if( larg != 1 )
    {
        decompile_put_int( larg );
    }
    decompile_inner( p, depth, indent );
    if( indent )
        decompile_put_char( ctl('J') );

    return decompile_buffer;
}

static void decompile_inner
    (
    ProgramNode *p,
    int depth,
    int indent
    )
{
    int lineargs = 0;

    if( p == NULL )
        return;

    BoundName *n = p->p_proc;

    // If this is the point that execution has reached recode the position
    if( p == trace_cur_exec )
        current_line = decompile_buffer.length();

    if( depth > 1 )
        if( n == &bound_number_node )
        {
            decompile_put_int( ((ProgramNodeInt *)p)->pa_int );
            return;
        }
        else if( n == &bound_string_node )
        {
            decompile_put_char( '"' );
            decompile_string( ((ProgramNodeString *)p)->pa_string );
            decompile_put_char( '"' );
            return;
        }
        else if( n == &bound_variable_node )
        {
            decompile_put_str( p->name()->v_name );
            return;
        }

    int i;

    if( indent != 0 )
        for( i=1; i<=depth-1; i += 1 )
            decompile_put_str(u_str("    ") );
    decompile_put_char( '(' );
    decompile_put_str( n->b_proc_name );
    if( indent != 0 )
        for( i=1; i<=p->p_nargs; i++ )
        {
            ProgramNode *argp =p->arg(i);
            BoundName *argn = argp->p_proc;

            if( argn != &bound_number_node
            && argn != &bound_string_node
            && argn != &bound_variable_node )
            {
                lineargs = 1;
                break;
            }
        }
    for( i=1; i<=p->p_nargs; i++ )
    {
        ProgramNode *argp;
        BoundName *argn;

        argp = p->arg(i);
        argn = argp->p_proc;
        decompile_put_char( lineargs ? ctl('J') : ' ' );
        if( lineargs
        &&    (argn == &bound_number_node ||
            argn == &bound_string_node ||
            argn == &bound_variable_node) )
            for( int j=1; j<=depth; j++ )
                decompile_put_str( u_str("    ") );
        decompile_inner( p->arg(i), depth + 1, indent );
    }
    if( lineargs )
    {
        decompile_put_char( ctl('J') );
        for( i=1; i<depth; i++ )
            decompile_put_str( u_str("    ") );
    }
    decompile_put_char( ')' );
}

void record_keystoke_history( const EmacsString &keys, BoundName *proc )
{
    EmacsString bufname = keystroke_history_buffer.asString();
    if( !bufname.isNull() )
    {
        // save the message in the named buffer

        EmacsBufferRef old( bf_cur );

        EmacsBuffer *p = EmacsBuffer::find( bufname );
        if( p == NULL )
        {
            p = EMACS_NEW EmacsBuffer( bufname );
            p->b_checkpointed = -1;        // turn off checkpointing and
            p->b_journalling = 0;        // journalling
        }

        p->set_bf();
        set_dot( bf_cur->num_characters() + 1 );

        EmacsString k_name( key_to_str( keys ) );
        EmacsString p_name;
        if( proc == NULL )
            bf_cur->ins_cstr( FormatString("%s is not in the keymaps\n") << k_name );
        else
        {
            if( proc->b_proc_name == sexpr_defun )
                p_name = decompile( proc->getProcedure(), 1, 0, 1 );
            else
                p_name = proc->b_proc_name;

            bf_cur->ins_cstr( FormatString("%s invoked %s\n") << k_name << p_name );
        }

        old.set_bf();
    }
}
