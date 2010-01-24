// Copyright (c) 1982-1995
//    Barry A. Scott and Nick Emery
// Mlisp Processing routines
//
// VMS Emacs MLisp (Mock/Minimal Lisp).
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#ifdef vms
#include <fscndef.h>
#include <descrip.h>
#endif

static int exec_number( void );
static int exec_string( void );
int exec_prog( ProgramNode *p );
int progn_command( void );
int lambda_command( void );
int declare_global( void );
int declare_buffer_specific( void );
static void perform_declare(int buf_variable);
int is_bound_command( void );
int error_occured( void );
int prefix_argument_loop( void );
int save_window_excursion( void );
int save_excursion( void );
int save_excursion_inner(int (*rtn)(void));
int if_command( void );
int while_command( void );
int insert_string( void );
int message_command( void );
int send_string_to_terminal( void );
int error_message( void );
void do_release( Expression *e );
int star_define_function( void );
int define_function( void );
static int define_function_inner(BoundName *proc);
int define_external_function( void );
static int exec_variable( void );
int setq_command( void );
int setq_default_command( void );
static int do_setq(int Default);
int set_command( void );
int set_default_command( void );
static int do_set(int Default);
int setq_array_command( void );
void perform_set( VariableName *v, Expression &new_e, int setting_default );
static void perform_set_array( VariableName *v );
int print_command( void );
int print_default_command( void );
static int do_print_command(int def);
int provide_prefix_argument( void );
int return_prefix_argument( void );
void print_expr( ProgramNode *p, int depth );
void void_result( void );
int array_command( void );
int fetch_array_command( void );
int type_of_Expression_command( void );
void init_lisp( void );

int stack_current_depth = 0;
SystemExpressionRepresentationStackMaxDepth stack_maximum_depth;

//--------------------------------------------------------------------------------
// routines to check the range of values to be stored in System variables
void SystemExpressionRepresentationStackMaxDepth::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value < 10 )
        throw EmacsExceptionVariableLessThanRange( 10 );
    if( value > 750 )
        throw EmacsExceptionVariableGreaterThanRange( 750 );

    exp_int = value;
}

void SystemExpressionRepresentationStackMaxDepth::fetch_value(void)
{ }

// execute a number node
static int exec_number( void )
{
    ml_value = ((ProgramNodeInt *)cur_exec)->pa_int;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString( "Number: %d\n") << ml_value.asInt() );
#endif

    return 0;
}

static int exec_string( void )
{
    ml_value = ((ProgramNodeString *)cur_exec)->pa_string;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString( "String: %s\n") << ml_value.asString() );
#endif

    return 0;
}

static int exec_expression( void )
{
    ml_value = ((ProgramNodeExpression *)cur_exec)->pa_value;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( "Expression: -*-\n" );
#endif

    return 0;
}

int exec_prog( ProgramNode *p )
{
    ProgramNode *old;
    unsigned int old_last_arg;
    int rv;
    unsigned int was_active;

    if( current_global_map != NULL )
        if( current_global_map->k_name.length() > 80 )
            debug_invoke();

    if( stack_current_depth >= stack_maximum_depth )
        error( FormatString("stack depth %d exceeded") << stack_maximum_depth );

    if( ml_err )
        return 0;

    old = cur_exec;
    old_last_arg = last_arg_used;
    rv = 0;

    ml_value.release_expr();

    if( p == NULL
    || p->p_proc == NULL
    || !p->p_proc->isBound()
    )
    {
        if( p && p->p_proc )
        {
            error( FormatString("\"%s\" has not been defined yet.") <<
                    p->p_proc->b_proc_name);
        }
        else
            error( "Attempt to execute an undefined MLisp function.");
        return 0;
    }

    stack_current_depth++;

    was_active = p->p_active;
    p->p_active = 1;
    cur_exec = p;
    last_arg_used = -1;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString( "ExecProg: %s >>> %s\n") <<
            (old ? old->p_proc->b_proc_name.sdata() : "--Top Level--") <<
            p->p_proc->b_proc_name.sdata() );
#endif

    rv = p->p_proc->execute();

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString( "ExecProg: %s <<< %s\n") <<
            (old ? old->p_proc->b_proc_name.sdata() : "--Top Level--") <<
            p->p_proc->b_proc_name.sdata() );
#endif

    p->p_active = was_active;
    cur_exec = old;
    last_arg_used = old_last_arg;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        switch( ml_value.exp_type() )
        {
        case ISVOID:
            _dbg_msg( "ml_value: ISVOID\n" );
            break;
        case ISINTEGER:
            _dbg_msg( FormatString("ml_value: ISINTEGER %d\n") << ml_value.asInt() );
            break;
        case ISSTRING:
            _dbg_msg( FormatString("ml_value: ISSTRING %s\n") << ml_value.asString() );
            break;
        case ISMARKER:
            _dbg_msg( FormatString("ml_value: ISMARKER %d %s\n") << ml_value.asInt() << ml_value.asString() );
            break;
        case ISWINDOWS:
            _dbg_msg( "ml_value: ISWINDOWS\n" );
            break;
        case ISARRAY:
            _dbg_msg( "ml_value: ISARRAY\n" );
            break;
        }
#endif


    //
    //    Copy ml_value into last_expression for the trace package to use
    //
    if( trace_mode && in_trace == 0 )
    {
        last_expression.release_expr();
        if( ml_value.exp_type() == ISVOID )
            last_expression = int(0);
        else
            last_expression = ml_value;
    }

    stack_current_depth--;

    return rv;
}

BoundNameNoDefine progn_block( "progn", progn_command );
BoundNameNoDefine lambda_block( "lamdba", lambda_command );

int progn_command( void )
{
    ProgramNode *p = cur_exec;

    if( p == NULL )
    {
        error( "progn can only appear in mlisp statements" );
        return 0;
    }

    // check for an empty progn
    if( p->p_nargs == 0 )
        return 0;

    int first_prog = p->p_nargs;

    //
    //    Bind the local variables
    //
    int var;
    for( var=1; var<=p->p_nargs; var++ )
    {
        if( p->arg(var)->p_proc != &bound_variable_node )
        {
            first_prog = var;
            break;
        }

        VariableName *v = p->arg(var)->name();
        v->pushBinding();
    }
    int rv = 0;
    int i = first_prog;
    while( ! ml_err && rv == 0 && quitting_emacs == 0
    && i <= p->p_nargs )
    {
        rv = exec_prog( p->arg(i) );
        i++;
    }
    for( var=1; var<first_prog; var++ )
    {
        VariableName *v = p->arg(var)->name();
        v->popBinding();
    }

    return rv;
}

// Lambda binding function

int lambda_command( void )
{
    ProgramNode *p;
    VariableName *v;
    int first_prog;
    Binding_list *b;

    if( (p = cur_exec) == NULL )
    {
        error( "lambda can only appear in mlisp statements");
        return 0;
    }

    //
    //    There must be at least one arg to this function
    //
    if( p->p_nargs == 0 )
    {
        error( "Insufficient args to lambda");
        return 0;
    }

    Binding_list *first_binding = NULL;
    Binding_list *last_binding = NULL;
    int arg_number = 1;
    int rv = 0;

    //
    //    Run through the list of variables, binding them to the
    //    evaluation of the appropriate argument to the MLisp function
    //    but do not make the variables available to the MLisp world
    //    until ALL the arguments are evaluated.
    //
    //    If there are insufficent arguments to match all the lambda
    //    variables, we look to see if the variable is followed by
    //    an MLisp expression. If it is, we evaluate the expression
    //    and set the variable to that value.
    //
    p = p->arg(1);
    int i;
    for( i=1; i<=p->p_nargs; i++ )
    {
        //
        //    This had better be a variable (skip MLisp expressions)
        //
        ProgramNode *pp = p->arg(i);
        if( pp->p_proc == &bound_variable_node )
        {
            //
            //    If there are not enough arguments to the MLisp
            //    function to satisfy all the lambda bindings
            //    check to see if the variable is followed by an
            //    MLisp expression. If not, declare an error. If so,
            //    evaluate it and set the value of the new variable
            //    to its result.
            //
            ProgramNode *rootp = execution_root->es_cur_exec;
            if( rootp == NULL
            || arg_number > rootp->p_nargs )
            {
                //
                //    If it is a variable, you lose -- otherwise
                //    evaluate it
                //
                pp = p->arg(i + 1);
                if( i + 1 > p->p_nargs
                || pp->p_proc == &bound_variable_node )
                    error( "Insufficent arguments and no default initializers");
                else
                {
                    ExecutionStack *old = execution_root;
                    execution_root = execution_root->es_dyn_parent;
                    exec_prog( pp );
                    execution_root = old;
                }
            }
            else
            {
                //
                //    Evaluate the appropriate argument to the MLisp
                //    function
                //
                ExecutionStack *old = execution_root;
                execution_root = execution_root->es_dyn_parent;
                exec_prog( rootp->arg( arg_number ) );
                execution_root = old;
            }

            //
            //    Create a new binding for the lambda variable
            //
            b = EMACS_NEW Binding_list;

            b->bl_arg_index = i;

            arg_number++;

            if( ml_err )
            {
                //
                //    An error was encountered, bind this variable to 0
                //
                b->bl_exp = int(0);
            }
            else
            {
                //
                //    No errors so far, bind to the result of the arg
                //
                b->bl_exp = ml_value;
            }

            //
            //    Insert this binding at the end of the binding List
            //
            if( last_binding != NULL )
                last_binding->bl_flink = b;
            else
                first_binding = b;

            last_binding = b;
        }
    }

    //
    //    With all the arguments evaluated, we now loop through the
    //    bindings, making the variables known to MLisp
    //
    b = first_binding;
    while( b != NULL )
    {
        ProgramNode *pp = p->arg( b->bl_arg_index );
        v = pp->name();
        v->pushBinding( b->bl_exp );

        b = b->bl_flink;
    }

    //
    //    Setup any Local variables that are declared
    //
    p = cur_exec;
    first_prog = p->p_nargs;
    for( i=2; i<=p->p_nargs; i++ )
    {
        ProgramNode *pp = p->arg( i );
        if( pp->p_proc != &bound_variable_node )
        {
            first_prog = i;
            break;
        }
        v = p->arg( i )->name();
        v->pushBinding();
    }

    //
    //    Loop through the rest of the arguments to lambda,
    //    evaluating them (They are the MLisp expressions to execute
    //    with the given lambda bindings).
    //
    i = first_prog;
    while( ! ml_err && rv == 0 && ! (quitting_emacs != 0)
    && i <= p->p_nargs )
    {
        rv = exec_prog( p->arg( i ) );
        i++;
    }

    //
    //    Destroy any local variables
    //
    p = cur_exec;
    for( i=2; i<first_prog; i++ )
    {
        ProgramNode *pp = p->arg( i );
        v = pp->name();
        v->popBinding();
    }

    //
    //    Loop through the list of variables that were bound and
    //    destroy the bindings
    //
    b = first_binding;
    p = p->arg( 1 );
    while( b != NULL )
    {
        ProgramNode *pp = p->arg( b->bl_arg_index );
        v = pp->name();
        v->popBinding();

        Binding_list *tmp = b;
        b = tmp->bl_flink;
        delete tmp;
    }

    //
    //    Return results of last exec_prog called
    //
    return rv;
}

int declare_global( void )
{
    if( cur_exec == 0 )
        error( "declare-global can only appear in mlisp statements");
    else
        perform_declare( 0 );
    return 0;
}

int declare_buffer_specific( void )
{
    if( cur_exec == 0 )
        error( "declare-buffer-specific can only appear in mlisp statements");
    else
        perform_declare( 1 );
    return 0;
}

static void perform_declare(int buf_variable)
{
    ProgramNode *p = cur_exec;
    for( int i=1; i<=p->p_nargs; i++ )
    {
        ProgramNode *pp = p->arg( i );
        if( pp->p_proc == &bound_variable_node )
        {
            VariableName *v = pp->name();
            if( buf_variable )
                v->declareBufferSpecific();
            else
                v->declareGlobal();
        }
        else
        {
            error( FormatString("%s's argument %d should be a variable name") <<
                p->p_proc->b_proc_name << i );
            return;
        }
    }
}

int is_bound_command( void )
{
    ProgramNode *p;

    if( (p = cur_exec) == 0 )
    {
        error( "is-bound can only appear in mlisp statements");
        return 0;
    }

    ml_value = Expression( int(1) );    // assume is bound
    for( int i=1; i<=p->p_nargs; i++ )
    {
        ProgramNode *pp;

        pp = p->arg( i );
        if( pp->p_proc != &bound_variable_node )
        {
            error( "is-bound's arguments must be variable names" );
            return 0;
        }
        VariableName *v = pp->name();
        if( !v->isBound() )
        {
            ml_value = Expression( int(0) );    // variable is not bound
            break;
        }
    }

    return 0;
}

int is_function_command( void )
{
    if( ! string_arg( 1 ) )
        return 0;


    BoundName *proc = BoundName::find( ml_value.asString() );
    if( proc == NULL )
    {
        ml_value = int(0);
        return 0;
    }

    ml_value = proc->isBound();

    return 0;
}

extern int in_error_occurred;
int error_occured( void )
{
    int rv;
    in_error_occurred++;
    rv = progn_command();
    ml_value = Expression( ml_err );
    ml_err = 0;
    in_error_occurred--;
    return rv;
}

int prefix_argument_loop( void )
{
    unsigned int rv;
    int i;

    rv = 0;
    for( i=execution_root->es_prefix_argument - 1; i>=0; i-- )
    {
        if( ml_err || rv != 0 )
            break;

        rv = progn_command();
    }

    return rv;
}

int save_window_excursion( void )
{
    unsigned int rv;

    push_window_ring();
    rv = save_excursion();
    pop_window_ring();

    return rv;
}

int save_excursion( void )
{
    return save_excursion_inner( progn_command );
}

int save_excursion_inner(int (*rtn)(void))
{
    bool buffer_visible = theActiveView->currentWindow()->w_buf == bf_cur;

    EmacsSearch sea( sea_glob );
    EmacsString sea_str = last_search_string.asString();

    Marker olddot( bf_cur, dot, 0 );
    Marker oldmark( bf_cur->b_mark );
    bool old_gui_input_mode = bf_cur->b_gui_input_mode_set_mark;

    int rv = rtn();

    if( olddot.isSet() )
        olddot.m_buf->set_bf();
    if( buffer_visible )
        theActiveView->window_on( bf_cur );
    if( olddot.isSet() )
        dot = olddot.to_mark();

    if( oldmark.isSet() )
        bf_cur->set_mark( oldmark.to_mark(), 0, old_gui_input_mode );
    else
        bf_cur->unset_mark();

    sea_glob = sea;

    last_search_string = sea_str;

    return rv;
}

int if_command( void )
{
    if( check_args( 2, 0 ) )
        return 0;

    for( int i=1; i<=cur_exec->p_nargs - 1; i += 2 )
    {
        if( ml_err )
            return 0;
        if( numeric_arg( i ) != 0 )
            return exec_prog( cur_exec->arg( i+1 ) );
    }
    //
    //    check for dangling else expr, that is an odd number of
    //    arguments to "if"
    //
    if( (cur_exec->p_nargs & 1) != 0 )
        return exec_prog( cur_exec->arg( cur_exec->p_nargs ) );
    else
        return 0;
}

int while_command( void )
{
    if( check_args( 2, 0 ) )
        return 0;
    if( ml_err ) return 0;

    int rv = 0;
    int nargs = cur_exec->p_nargs - 1;

    ProgramNode **p = ((ProgramNodeNode *)cur_exec)->pa_node;
    ProgramNode *test = p[0];

    while( rv == 0
    && quitting_emacs == 0
    && (rv = exec_prog( test )) == 0
    && ml_value.exp_type() == ISINTEGER
    && ml_value.asInt() != 0
    && ! ml_err )
        for( int i=1; i<=nargs; i += 1 )
            if( rv != 0 || quitting_emacs != 0 )
                return 0;
            else
                rv = exec_prog( p[i] );
    return rv;
}

int insert_string( void )
{
    concat_command();
    if( ml_err )
        return 0;

    if( input_mode == 1 ) gui_input_mode_before_insert();

    bf_cur->ins_cstr( ml_value.asString() );
    return 0;
}

int message_command( void )
{
    concat_command();
    if( ml_err )
        return 0;

    cur_exec = NULL;
    if( ! ml_err )
        message( ml_value.asString() );

    void_result();
    return 0;
}

int send_string_to_terminal( void )
{
    EmacsString s;

    if( cur_exec == NULL )
        s = get_string_interactive( ": send-string-to-terminal " );
    else
        s = get_string_mlisp();
#ifndef _WINDOWS
    if( !s.isNull() && term_is_terminal == 1)
        theActiveView->t_io_print( s.data() );
#endif
    void_result();
    return 0;
}

int error_message( void )
{
    concat_command();
    if( ml_err )
        return 0;

    error( ml_value.asString() );

    void_result();
    return 0;
}

int star_define_function( void )
{
    return define_function_inner( &lambda_block );
}

int define_function( void )
{
    return define_function_inner( &progn_block );
}

static int define_function_inner(BoundName *proc)
{

    if( check_args( 1, 0 ) )
        return 0;

    //
    //    As the arg list is being dismantled mark its
    //    length as 0. This will result in an error from
    //    the check_args( 1, 0 ) above if a defun is
    //    processed twice.
    //
    int nargs = cur_exec->p_nargs;

    for( int i=1; i<=nargs; i++ )

    {
        ProgramNode *p = cur_exec->arg( i );
        if( p != NULL )
        {
            ((ProgramNodeNode *)cur_exec)->pa_node[ i-1 ] = NULL;

            // replaceInside will generate an error if the replace will fail
            p->p_proc->replaceInside( p );
            if( ml_err )
                break;

            p->p_proc = proc;
        }
    }

    cur_exec->p_nargs = 0;

    return 0;
}


int define_external_function( void )
{

    //
    // check the argument count. There must be two args, but there may be
    // three. The third allows you to map the symbol name in the image to
    // a difference name in EMACS
    //
    if( check_args( 2, 3 ) )
        return 0;

    // Collect the file name

    if( ! string_arg( 2 ) )
    {
        void_result();
        return 0;
    }

    EmacsString filename( ml_value.asString() );

    EmacsString symbol;
    // Collect the image symbol name if it exists
    if( cur_exec->p_nargs > 2 )
    {
        if( ! string_arg( 3 ) )
        {
            void_result();
            return 0;
        }
        else
        {
            symbol = ml_value.asString();
        }
    }
    // Collect the EMACS function name
    if( ! string_arg( 1 ) )
    {
        void_result();
        return 0;
    }

    if( symbol.isNull() )
        symbol = ml_value.asString();

    // Now work out the image name from the filename
    EmacsString image;
#ifdef vms
{
    struct dsc$descriptor fdesc;
    struct fscn_def item[2];

    DSC_SZ( fdesc, filename );

    item[0].w_item_code = FSCN$_NAME;
    item[0].w_length = 0;
    item[0].a_addr = 0;
    item[1].w_item_code = 0;
    item[1].w_length = 0;
    item[1].a_addr = 0;

    if( ! VMS_SUCCESS(sys$filescan( &fdesc, item, 0 )) )
    {
        error( "$FILESCAN return an error status for file %s", filename );

        void_result();
        return 0;
    }

    image = malloc_ustr( item[0].w_length + 1 );
    if( image == NULL )
    {
        error( "Out of memory in external-function!" );
        void_result();

        return 0;
    }

    // copy the image name out of the file spec
    memcpy( image, item[0].a_addr, item[0].w_length );
    image[ item[0].w_length ] = 0;
}
#else
{
    int path_index = filename.last( PATH_CH );

    if( path_index >= 0 )
        image = filename( path_index+1, INT_MAX );
    else
        image = filename;
}
#endif


    // Now, make the entry in the function name space, and release the extra
    EmacsExternImage *extern_image = EmacsExternImage::find( image );
    if( extern_image == NULL )
        extern_image = EMACS_NEW EmacsExternImage( image, filename );

    EmacsExternFunction *body = EMACS_NEW EmacsExternFunction( symbol, extern_image );
    if( extern_image == NULL || body == NULL )
    {
        error( "Out of memory in external-function!" );
        void_result();

        return 0;
    }

    EMACS_NEW BoundName( ml_value.asString(), body );

    void_result();

    return 0;
}

//
//    (use-buffer-variables "buffer-name" s-expr)
//
EmacsBufferRef *use_variables_of_buffer= NULL;

int use_variables_of_buffer_command( void )
{
    if( check_args( 2, 2 ) )
        return 0;
    if( ! string_arg( 1 ) )
        return 0;

    EmacsBuffer *buf = EmacsBuffer::find( ml_value.asString() );
    if( buf == NULL )
    {
        error( FormatString("Buffer %s does not exist") << ml_value.asString() );
        return 0;
    }

    //
    //    Save the current use-buffer in the save block
    //    and link it into the list.
    //
    EmacsBufferRef *save_block = use_variables_of_buffer;
    EmacsBufferRef var_of_buf( buf );

    use_variables_of_buffer = &var_of_buf;

    int rv = exec_prog( cur_exec->arg( 2 ) );

    use_variables_of_buffer = save_block;

    return rv;
}

EmacsBuffer *current_buffer_for_mlisp()
{
    if( use_variables_of_buffer != NULL && use_variables_of_buffer->bufferValid() )
        return use_variables_of_buffer->buffer();
    else
        return bf_cur;
}

static int exec_variable( void )
{
    VariableName *v = cur_exec->name();

    if( !v->normalValue( ml_value ) )
    {
        error( FormatString("Reference to an unbound variable: \"%s\"") << v->v_name );
        return 0;
    }

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        try
        {
            _dbg_msg( FormatString( "Variable: \"%s\" value \"%s\"\n")
                << v->v_name << ml_value.asString() );
        }
        catch(...)
        {
            _dbg_msg( FormatString( "Variable: \"%s\"\n") << v->v_name );
        }
#endif

    return 0;
}

int setq_command( void )
{
    return do_setq( 0 );
}

int setq_default_command( void )
{
    return do_setq( 1 );
}

static int do_setq(int def_ault)
{
    ProgramNode *p;

    if( check_args( 2, 2 ) )
        return 0;

    p = cur_exec->arg( 1 );
    if( p->p_proc != &bound_variable_node )
    {
        error( "setq expects its first argument to be a variable name." );
        return 0;
    }

    if( ! eval_arg( 2 ) )
        return 0;

    perform_set( p->name(), ml_value, def_ault );

    return 0;
}

int set_command( void )
{
    return do_set( 0 );
}

int set_default_command( void )
{
    return do_set( 1 );
}

static int do_set(int def_ault)
{
    VariableName *v;
    EmacsString prompt;

    if( cur_exec == NULL )
    {
        if( def_ault )
            prompt = ": set-default ";
        else
            prompt = ": set ";

        v = VariableName::get_word_interactive( prompt );
    }
    else
        v = VariableName::get_word_mlisp();
    if( v == NULL )
        return 0;

    EmacsString p;
    if( cur_exec != NULL )
        p = get_string_mlisp();
    else
    {
        if( def_ault )
            prompt = FormatString(": set-default %s ") << v->v_name;
        else
            prompt = FormatString(": set %s ") << v->v_name;

        Expression value;

        if( v->normalValue( value ) && value.exp_type() == ISINTEGER )
            p = get_nb_string_interactive( prompt );
        else
            p = get_string_interactive( prompt );
    }

    Expression e( p );
    perform_set( v, e, def_ault );

    return 0;
}

int setq_array_command( void )
{
    if( check_args( 2, 0 ) )
        return 0;

    ProgramNode *p = cur_exec->arg( 1 );
    if( p->p_proc != &bound_variable_node )
    {
        error( "setq-array expects its first argument to be a variable name." );
        return 0;
    }

    perform_set_array( p->name() );

    return 0;
}


//
// Assign the arg th expression to v
//    if arg eql 0 then the string 'svalue' will be used
//    if arg is -1 then ivalue and svalue are used. In which case
//    if svalue is 0 then the expression is numeric, otherwise string.
//
void perform_set
    (
    VariableName *v,
    Expression &new_e,
    int setting_default
    )
{
#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        try{
            _dbg_msg( FormatString( "perform_set: \"%s\" <= \"%s\"\n") << v->v_name << new_e.asString() );
        }
        catch(...)
        {
            _dbg_msg( FormatString( "perform_set: \"%s\"\n") << v->v_name );
        }
    }
#endif

    try
    {
        bool assigned = false;
        if( setting_default )
            assigned = v->assignDefault( new_e );
        else
            assigned = v->assignNormal( new_e );
        if( !assigned )
        {
            error( FormatString("Attempt to set the unbound variable \"%s\"") << v->v_name );
            return;
        }
    }
    catch( EmacsExceptionVariableTabOutOfRange )
    {
        error( FormatString("%s should have a value between 1 and 64") << v->v_name );
    }
    catch( EmacsExceptionVariableGreaterThanRange e )
    {
        error( FormatString("%s should be positive and greater than %d") <<
                v->v_name << e.range );
    }
    catch( EmacsExceptionVariableLessThanRange e )
    {
        error( FormatString("%s should be positive and less than %d") <<
                v->v_name << e.range );
    }
    catch( EmacsExceptionVariable )
    {
        error( FormatString("%s cannot be assigned to") << v->v_name );
    }
    return;
}

static void perform_set_array( VariableName *v )
{
    Expression e;
    if( !v->normalValue( e ) )
    {
        error( FormatString("Attempt to set the unbound variable \"%s\"") << v->v_name );
        return;
    }

    unsigned int index;

    if( e.exp_type() != ISARRAY )
    {
        error( FormatString("%s is expected to be an array.") << v->v_name );
        return;
    }

    EmacsArray a( e.asArray() );
    if( check_args( 2, 2 + a.dimensions() ) )
        return;

    index = a.array_index( 2 );
    if( ml_err )
        return;

    if( !eval_arg( 2 + a.dimensions() ) )
        return;

    a.setValue( index, ml_value );

    return;
}

int print_command( void )
{
    return do_print_command( 0 );
}

int print_default_command( void )
{
    return do_print_command( 1 );
}

static int do_print_command(int def)
{
    EmacsString prompt;
    if( def )
        prompt = ": print-default ";
    else
        prompt = ": print ";

    VariableName *v = getword( VariableName::, prompt  );
    if( v == NULL )
        return 0;

    bool have_value;
    Expression value;
    EmacsString str;

    if( def )
    {
        str = "-default";

        have_value = v->defaultValue( value );
    }
    else
    {
        // leave str null
        have_value = v->normalValue( value );
    }

    if( !have_value )
    {
        error( FormatString("%s is not bound to a value.") << v->v_name );
        return 0;
    }

    switch( value.exp_type() )
    {
    case ISINTEGER:
        message( FormatString(": print%s %s => %d") << str << v->v_name << value.asInt() );
        break;

    case ISSTRING:
        message( FormatString(": print%s %s => \"%s\"") << str << v->v_name << value.asString() );
        break;

    case ISMARKER:
    {
        Marker *m = value.asMarker();

        if( m != NULL && m->m_buf != NULL )
        {
            message( FormatString(": print%s %s = > Marker (\"%s\", %d)") <<
                str << v->v_name <<
                m->m_buf->b_buf_name <<
                m->get_mark() );
        }
        else
            message( FormatString(": print%s %s => marker for deleted buffer") << str << v->v_name);
    }
        break;

    case ISWINDOWS:
        message( FormatString(": print%s %s => A set of windows") << str << v->v_name );
        break;

    case ISARRAY:
        message( FormatString(": print%s %s => An array") << str << v->v_name );
        break;

    default:
        error( FormatString(": print%s %s => Something very odd - internal error!") << str << v->v_name );
    }

    return 0;
}

int provide_prefix_argument( void )
{
    if( check_args( 2, 2 ) )
        return 0;
    arg = numeric_arg( 1 );
    arg_state = prepared_arg;
    if( ml_err )
        return 0;
    else
        return exec_prog( cur_exec->arg( 2 ) );
}

int return_prefix_argument( void )
{
    arg = getnum( ": return-prefix-argument " );
    arg_state = prepared_arg;
    return 0;
}

// print out an MLisp expression( de-compile it ) into the current buffer
void print_expr
    (
    ProgramNode *p,
    int depth
    )

{
    BoundName *n;
    if( p == NULL )
    {
        bf_cur->ins_str( u_str( "<< Command Level >>" ) );
        return;
    }
    n = p->p_proc;
    if( n == &bound_number_node )
    {
        bf_cur->ins_cstr( FormatString("%d") << ((ProgramNodeInt *)p)->pa_int );
        return;
    }
    if( n == &bound_string_node )
    {
        bf_cur->ins_cstr( u_str( "\"" ), 1 );
        bf_cur->ins_cstr( ((ProgramNodeString *)p)->pa_string );
        bf_cur->ins_cstr( u_str( "\"" ), 1 );
        return;
    }
    if( n == &bound_variable_node )
    {
        bf_cur->ins_cstr( p->name()->v_name );
        return;
    }
    bf_cur->ins_cstr( u_str( "(" ), 1 );
    if( depth >= 0 )
    {
        int i;

        bf_cur->ins_cstr( n->b_proc_name );
        for( i=1; i<=p->p_nargs; i++ )
        {
            bf_cur->ins_cstr( u_str( " " ), 1 );
            print_expr( p->arg( i ), depth - 1 );
        }
    }
    bf_cur->ins_cstr( u_str( ")" ), 1 );
}

// Throw away any expression evaluation so that the current function returns
// no value
void void_result( void )
{
    ml_value = Expression();
}

int array_command( void )
{
    if( check_args( 2, 2 * EmacsArray::ARRAY_MAX_DIMENSION ) )
        return 0;

    EmacsArray array;

    int dims = cur_exec->p_nargs / 2;

    for( int i=0; i<dims; i++ )
    {
        int low = numeric_arg( 1 + i*2 );
        if( ml_err )
            return 0;
        int high = numeric_arg( 1 + i*2 + 1 );
        if( ml_err )
            return 0;
        if( low > high )
        {
            error( "Array lower bound must be less then higher bound" );
            return 0;
        }

        array.addDimension( low, high );
    }

    array.create();

    // return created array
    ml_value = array;

    return 0;
}

int fetch_array_command( void )
{
    if( check_args( 2, 0 ) )
        return 0;

    ProgramNode *p = cur_exec->arg( 1 );
    if( p->p_proc != &bound_variable_node )
    {
        error( "fetch-array expects its first argument to be a variable name." );
        return 0;
    }

    VariableName *v = p->name();
    Expression e;
    if( !v->normalValue( e ) )
    {
        error( FormatString("Reference to an unbound variable: \"%s\"" ) << v->v_name );
        return 0;
    }

    if( e.exp_type() != ISARRAY )
    {
        error( "fetch-array expects to be called with an array variable" );
        return 0;
    }

    // copy array to lock it
    EmacsArray a( e.asArray() );

    // set up for an array access to a system complex variable
    if( check_args( 2, 1 + a.dimensions() ) )
        return 0;

    unsigned int index = a.array_index( 2 );
    if( ml_err )
        return 0;

    ml_value = a.getValue( index );

    // compatibilty with V6.0 Emacs behaviour
    if( ml_value.exp_type() == ISVOID )
        ml_value = int(0);

    return 0;
}

//
//    Return the array index value after check_ing bounds.
//    returns with ml_err set on error
//
int EmacsArray::array_index( unsigned int arg )
{
    int i;
    int index;
    int subscript;
    int *low;
    int *size;

    low = array->lower_bound;
    size = array->size;
    index = 0;

    for( i=0; i<=array->dimensions - 1; i += 1 )
    {
        subscript = numeric_arg( arg + i );
        if( ml_err )
            return 0;

        subscript = subscript - low[ i ];
        if( subscript < 0 || subscript >= size[ i ] )
        {
            error
            (
            FormatString("Array subscript error - subscript %d is %d, bounds are %d to %d") <<
                (i + 1) << (subscript + low[i]) <<
                low[i] << (low[i] + size[i] - 1)
            );
            return 0;
        }

        index = index * size[i] + subscript;
    }

    return index;
}


//
//    type-of command
//        returns a string describing the the type of the
//        expression that is its parameter
//
int type_of_Expression_command( void )
{
    int type;

    if( check_args( 1, 1 ) )
        return 0;
    if( ! eval_arg( 1 ) )
        return 0;

    type = ml_value.exp_type();

    switch( type )
{
    case ISVOID:     ml_value = EmacsString( "void" ); break;
    case ISINTEGER: ml_value = EmacsString( "integer" ); break;
    case ISSTRING:     ml_value = EmacsString( "string" ); break;
    case ISMARKER:     ml_value = EmacsString( "marker" ); break;
    case ISWINDOWS: ml_value = EmacsString( "windows" ); break;
    case ISARRAY:     ml_value = EmacsString( "array" ); break;
    default:
    {
        error( "type-of-variable called with bizzare expression" );
        return 0;
    }
}

    return 0;
}

int EmacsArray::bounds_of_array_command( void )
{
    if( check_args( 1, 1 ) )
        return 0;
    if( ! eval_arg( 1 ) )
        return 0;

    if( ml_value.exp_type() != ISARRAY )
    {
        error( "bounds-of-array expects its argument to be an array" );
        return 0;
    }

    EmacsArray &array = ml_value.asArray();

    int dims = array.dimensions();

    EmacsArray bounds(
            1, 2,        // first dimension range
            0, dims        // second dimension size
            );

    // 1, 0 has the number of dimension in it
    bounds( 1, 0 ) = dims;

    // row 1 has the lower bounds in it
    for( int i=1; i<=dims; i++ )
        bounds( 1, i ) = array.array->lower_bound[i];

    // 2, 0 has the number of elements in the array
    bounds( 2, 0 ) = array.array->total_size;
    // row 2 has the upper bounds in it
    for( int j=1; j<=dims; j++ )
        bounds( 2, j ) = array.array->lower_bound[j] + array.array->size[j] - 1;

    ml_value = bounds;

    return 0;
}

// Fetch a variable as either a string or a number
static unsigned char no_variable [] = "No variable exists with name \"%s\"";
static unsigned char unbound_var [] = "\"%s\" is not bound to a value.";
static unsigned char fetch_type [] = "\"%s\" is %s and cannot be coerced to %s.";
static unsigned char int_type [] = "an integer";
static unsigned char str_type [] = "a string";

int fetch_var( const EmacsString &name, int *iresp, EmacsString *cresp )
{
    VariableName *nm = VariableName::find( name );

    if( nm == NULL )
    {
        error( FormatString(no_variable) << name );
        return 0;
    }

    EmacsString errmsg;
    if( fetch_var( nm, iresp, cresp, errmsg ) )
        return 1;

    error( errmsg );
    return 0;
}

int fetch_var( VariableName *nm, int *iresp, EmacsString *cresp, EmacsString &errmsg )
{
    Expression value;

    if( !nm->normalValue( value ) )
    {
        error( FormatString(unbound_var) << nm->v_name );
        return 0;
    }

    switch( value.exp_type() )
    {
    case ISINTEGER:
    {
        int var = value.asInt();
        if (iresp)
            *iresp = var;
        else
        {
            EmacsString number( FormatString("%d") << var );
            *cresp = number;
        }
    }
        break;

    case ISSTRING:
        if (iresp)
        {
            *iresp = value.asInt();
            if (ml_err)
                return 0;
        }
        else
            *cresp = value.asString();
        break;

    case ISMARKER:
        errmsg = FormatString(fetch_type) << nm->v_name << "a marker" << (iresp ? int_type : str_type);
        return 0;

    case ISWINDOWS:
        errmsg = FormatString(fetch_type) << nm->v_name << "a window" << (iresp ? int_type : str_type);
        return 0;

    case ISARRAY:
        errmsg = FormatString(fetch_type) << nm->v_name << "an array" << (iresp ? int_type : str_type);
        return 0;

    default:
        errmsg = FormatString(fetch_type) << nm->v_name << "unknown" << (iresp ? int_type : str_type);
        return 0;
    }

    return 1;
}

// Set a variable from the supplied values
void set_var( const EmacsString &name, const Expression &e )
{
    VariableName *v = VariableName::find( name );
    if( v == NULL)
    {
        error( FormatString(no_variable) << name);
        return;
    }

    set_var( v, e );
}

void set_var( VariableName *v, const Expression &e )
{
    if( !v->assignNormal( e ) )
        error( FormatString("Attempt to set the unbound variable \"%s\"") << v->v_name);

}

void SystemExpressionRepresentationPrefixArgument::assign_value( ExpressionRepresentation *new_value )
{
    execution_root->es_prefix_argument = new_value->asInt();
}

void SystemExpressionRepresentationPrefixArgumentProvided::assign_value( ExpressionRepresentation * )
{
    throw EmacsExceptionVariableReadOnly();
}

void SystemExpressionRepresentationPrefixArgument::fetch_value(void)
{
    if( execution_root )
        exp_int = execution_root->es_prefix_argument;
    else
        exp_int = int(0);
}

void SystemExpressionRepresentationPrefixArgumentProvided::fetch_value(void)
{
    if( execution_root )
        exp_int = execution_root->es_prefix_argument_provided;
    else
        exp_int = int(0);
}

ExecutionStack::ExecutionStack(int provided, int arg )
: es_dyn_parent( execution_root )
, es_cur_exec( cur_exec )
, es_cur_name( current_function.asString() )
, es_prefix_argument( arg )
, es_prefix_argument_provided( provided )
{
    execution_root = this;
}

ExecutionStack::~ExecutionStack()
{
    emacs_assert( this == execution_root );
    execution_root = es_dyn_parent;
}


BoundNameNoDefine bound_number_node( "execute-number", exec_number );
BoundNameNoDefine bound_string_node( "execute-string", exec_string );
BoundNameNoDefine bound_expression_node( "execute-expression", exec_expression );
BoundNameNoDefine bound_variable_node( "execute-variable", exec_variable );
BoundNameNoDefine bound_star_defun_node( "*defun", star_define_function );

void init_lisp( void )
{
    last_expression = int(0);
    stack_maximum_depth = int(100);
}
