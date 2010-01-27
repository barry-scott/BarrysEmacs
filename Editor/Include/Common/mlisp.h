//    Copyright (c) 1982-2010
//        Barry A. Scott
// Header file for dealing with values returned by mlisp functions

#include <em_user.h>

//
//    Classes defined in this header file
//
class EmacsMacroString;

class VariableName;
class Binding;
class Binding_list;

class ExecutionStack;
class ProgramNode;

class BoundNameInside;
class  BoundNameMacro;                  // public BoundNameInside
class  BoundNameAutoLoad;               // public BoundNameInside
class  BoundNameBuiltin;                // public BoundNameInside
class  BoundNameProcedure;              // public BoundNameInside
class  BoundNameKeymap;                 // public BoundNameInside
class  BoundNameExternalFunction;       // public BoundNameInside
class BoundName;
class  BoundNameNoDefine;               // public BoundName

//
//
//    a particular (name,value) binding
//
//
class Binding : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( Binding )
    // constructors used generally
    Binding();                          // bind to void
    Binding( const Expression &exp );   // bind to exp
    Binding( const Expression &exp, EmacsBuffer *buf ); // bind to exp on buffer
    virtual ~Binding();

    // constructors used to init emacs - see variable.c
    Binding( SystemExpression *exp, int is_default=0 );
    Binding( SystemExpression *exp, Binding *default_value );

private:
    friend void restore_var(void);
    Binding( Expression *exp );
public:
    Binding *b_inner;               // the next inner binding for the same name
    Expression *b_exp;              // The value held by this variable
    EmacsBufferRef b_local_to;      // The buffer that this binding is local to

    unsigned b_buffer_specific : 1; // True iff the variable is buffer specific
    unsigned b_is_default : 1;      // True iff this is the default value entry
};

//
//    Used for lambda binding in *defun
//
class Binding_list : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( Binding_list )
    Binding_list();
    virtual ~Binding_list();

    Expression bl_exp;
    Binding_list *bl_flink;
    int bl_arg_index;
};

class ExecutionStack
{
    // traceback/argument-evaluation stack for MLisp functions
public:
    ExecutionStack( int provided=0, int arg=0 );
    virtual ~ExecutionStack();

    ExecutionStack *es_dyn_parent;      // pointer to the dynamically enclosing parent of this execution frame
    ProgramNode *es_cur_exec;           // the expression being executed at this level

    EmacsString es_cur_name;            // name of the function whoes body is in  es_cur_exec

    int es_prefix_argument;             // The argument prefixed to this invocation
    int es_prefix_argument_provided;    // true iff there really was an argument
                                        // prefixed to this invocation.  If there
                                        // wasn't, then the value of PrefixArgument
                                        // will be 1
};

//
//    HACK * HACK * HACK * HACK * HACK
//
//    These OS specific classes belong else ware
//

class EmacsExternImageOsInfo
{
public:
    EmacsExternImageOsInfo();
    virtual ~EmacsExternImageOsInfo();

    virtual bool isLoaded() = 0;
    virtual bool load( const EmacsString &file ) = 0;
    virtual bool unload() = 0;
    virtual void *symbolValue( const EmacsString &symbol ) = 0;
};

extern EmacsExternImageOsInfo *make_EmacsExternImageOsInfo();

// external-defun function description
class EmacsExternImage : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsExternImage )
    EmacsExternImage( const EmacsString &image, const EmacsString &filename );
    ~EmacsExternImage();

    int ref_count;
    EmacsString ef_image;        // name of the image
    EmacsString ef_filename;    // name of the file that holds the image
    void *ef_context;        // holds a pointer for the images use

    EmacsExternImageOsInfo *ef_os_info;    // data required by the os_specific code

    static EmacsExternImage *find( const EmacsString &name )
    { return (EmacsExternImage *)name_table.find( name ); }

    static void add( const EmacsString &key, EmacsExternImage *value )
    { name_table.add( key, value ); }
    static EmacsExternImage *remove( const EmacsString &key )
    { return (EmacsExternImage *)name_table.remove( key ); }
    static EmacsExternImage *value( int index )
    { return (EmacsExternImage *)name_table.value( index ); }
    static int entries()
    { return name_table.entries(); }
    static EmacsStringTable name_table;
};


class EmacsExternFunction : public EmacsObject
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsExternFunction )
    EmacsExternFunction( const EmacsString &name, EmacsExternImage *image )
    : ef_funcname( name )
    , ef_image( image )
    , ef_c_function( NULL )
    , ef_cpp_function( NULL )
    , ef_restore_count( 0 )
    , ef_context( NULL )
    { }

    EmacsString ef_funcname;    // Name of function to be called
    EmacsExternImage *ef_image;
    EmacsCallBackStatus (*ef_c_function)(EmacsCallBackStatus (*call_back)(EmacsCallBackFunctions,...));    // The address of the routine to be called
    EmacsCallBackStatus (*ef_cpp_function)(EmacsCallBackInterface &call_back);    // The address of the routine to be called

    bool is_c_plus_plus;        // true if ef_function is C++ rather then C
    int ef_restore_count;        // The restore-environment count at last
                    // call to LIB$FIND_IMAGE_SYMBOL
    void *ef_context;        // This function's context
};

// The things that an executable symbol can be bound to
enum BoundType
{
    PROCBOUND,    // a wired-in procedure
    MACROBOUND,    // a macro (string)
    MLISPBOUND,    // an MLisp function
    AUTOLOADBOUND,    // a function to be autoloaded
    KEYBOUND,    // bound to a keymap
    EXTERNALBOUND    // bound to some external, run-time loaded code
};

class BoundNameInside :  public EmacsObject
{
protected:
    BoundNameInside();
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameInside )
    virtual ~BoundNameInside();
    virtual int execute(void) = 0;            // execute what is bound
    virtual int canDelete(void) const;        // true if you can delete this item
    virtual KeyMap *getKeyMap(void) const;        // return NULL or a KeyMap if there is one
    virtual EmacsMacroString getMacro(void) const;    // return NULL or a Macros string if there is one
    virtual ProgramNode *getProcedure(void) const;
    virtual const char *commandTypeName() const = 0;
private:
    int ref_count;
};

class BoundNameMacro : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameMacro )
    BoundNameMacro( EmacsMacroString macro_body );
    virtual ~BoundNameMacro();
    virtual int execute(void);            // execute what is bound
    virtual EmacsMacroString getMacro(void) const;    // return NULL or a Macros string if there is one
    virtual const char *commandTypeName() const;
private:
    EmacsString b_body;
};

class BoundNameAutoLoad : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameAutoLoad )
    BoundNameAutoLoad( const EmacsString &module_name );
    virtual ~BoundNameAutoLoad();
    virtual int execute(void);            // execute what is bound
    virtual const char *commandTypeName() const;
private:
    EmacsString b_module;
};

class BoundNameBuiltin : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameBuiltin )
    BoundNameBuiltin( int (*builtin_function)(void) );
    virtual ~BoundNameBuiltin();
    virtual int execute(void);            // execute what is bound
    virtual int canDelete(void) const;        // true if you can delete this item
    virtual const char *commandTypeName() const;
private:
    int (*b_builtin)(void);
};

class BoundNameProcedure : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameProcedure )
    BoundNameProcedure( ProgramNode *mlisp_body );
    virtual ~BoundNameProcedure();
    virtual int execute(void);            // execute what is bound
    virtual ProgramNode *getProcedure(void) const;
    virtual const char *commandTypeName() const;
private:
    ProgramNode *b_prog;
};

class BoundNameKeymap : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameKeymap )
    BoundNameKeymap( KeyMap *keymap );
    virtual ~BoundNameKeymap();
    virtual int execute(void);            // execute what is bound
    virtual KeyMap *getKeyMap(void) const;        // return the KeyMap
    virtual const char *commandTypeName() const;
private:
    KeyMap *b_keymap;
};

class BoundNameExternalFunction : public BoundNameInside
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameExternalFunction )
    BoundNameExternalFunction( EmacsExternFunction *external_function );
    virtual ~BoundNameExternalFunction();
    virtual int execute(void);    // execute what is bound
    virtual const char *commandTypeName() const;
private:
#if defined(CALL_BACK)
    int activate_external_function();
#endif
    EmacsExternFunction *b_func;
};

extern int rename_macro(void);

class EmacsMacroString : public EmacsString
{
public:
    EMACS_OBJECT_FUNCTIONS( EmacsMacroString )
    EmacsMacroString()
    : EmacsString()
    { }
    EmacsMacroString( const EmacsString &macro )
    : EmacsString( macro )
    { }

    EmacsMacroString &operator=( const char *str ) { EmacsString::operator=( str ); return *this; }
    EmacsMacroString &operator=( const unsigned char *str ) { EmacsString::operator=( str ); return *this; }
    EmacsMacroString &operator=( const EmacsString &str ) { EmacsString::operator=( str ); return *this; }
};

class FunctionNameTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( FunctionNameTable )
    FunctionNameTable( int init_size, int grow_amount )
    : EmacsStringTable( init_size, grow_amount )
    { }
    virtual ~FunctionNameTable()
    { }

    void add( const EmacsString &key, BoundName *value )
    { EmacsStringTable::add( key, value ); }
    BoundName *remove( const EmacsString &key )
    { return (BoundName *)EmacsStringTable::remove( key ); }
    BoundName *find( const EmacsString &key )
    { return (BoundName *)EmacsStringTable::find( key ); }
    BoundName *value( int index )
    { return (BoundName *)EmacsStringTable::value( index ); }
};

class BoundName : public EmacsObject
{
    friend int BoundNameAutoLoad::execute(void);

    // a name-procedure/macro binding
public:
    EMACS_OBJECT_FUNCTIONS( BoundName )
    BoundName( const EmacsString &name );
    BoundName( const EmacsString &name, const EmacsMacroString &macro_body );
    BoundName( const EmacsString &name, const EmacsString &module_name );
    BoundName( const EmacsString &name, int (*builtin_function)(void) );
     BoundName( const EmacsString &name, ProgramNode *mlisp_body );
    BoundName( const EmacsString &name, KeyMap *keymap );
    BoundName( const EmacsString &name, EmacsExternFunction *external_function );

    // add this BoundName to the list of known commands
    virtual void define(void);
    // find the index into the known command list
    static BoundName *find( const EmacsString &name )
    { return name_table.find( name ); }

    // return one of the keys in the table otherwise NULL
    static BoundName *get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static BoundName *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }
    static BoundName *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

private:
    int replaceInsideHelper(void);
public:
    void replaceInside( void );
    void replaceInside( const EmacsMacroString &macro_body );
    void replaceInside( const EmacsString &module_name );
    void replaceInside( ProgramNode *mlisp_body );
    void replaceInside( KeyMap *keymap );
    void replaceInside( EmacsExternFunction *external_function );
    void replaceInside( int (*builtin_function)(void) );

    // fetch the contents of the BoundName as differnent types
    KeyMap *getKeyMap(void) const;
    EmacsMacroString getMacro(void) const;
    ProgramNode *getProcedure(void) const;

    // find out if this BoundName is of a particular type
    bool IsAKeyMap(void) const;
    bool IsAMacro(void) const;
    bool IsAProcedure(void) const;


    int isBound(void) const;

    int canDelete(void) const;
    int execute(void);

    const char*commandTypeName() const;

    static int rename_macro(void);

    EmacsString b_proc_name;    // 04 the name to which this procedure or macro is bound

    unsigned b_active : 1;        // 0c true iff this (macro) is active --
                    // prevents recursive macro calls
    unsigned b_break : 1;        // 0e true iff execution of this fuinction should case a break point
private:
    BoundNameInside *implementation;    // the implementation of this Name

public:
    static FunctionNameTable name_table;
};

class BoundNameNoDefine : public BoundName
{
public:
    EMACS_OBJECT_FUNCTIONS( BoundNameNoDefine )
     BoundNameNoDefine( const EmacsString &name, ProgramNode *mlisp_body );
    BoundNameNoDefine( const EmacsString &name, int (*builtin_function)(void) );
};


//
//
//    Program Node
//
//
extern BoundNameNoDefine bound_number_node;
extern BoundNameNoDefine bound_string_node;
extern BoundNameNoDefine bound_expression_node;
extern BoundNameNoDefine bound_variable_node;

class MLispInputStream;
class ProgramNode : public EmacsObject
{
protected:
    EMACS_OBJECT_FUNCTIONS( ProgramNode )
    ProgramNode( BoundName *proc )
    : p_proc( proc )
    , p_active( 1 )
    , p_nargs( 0 )
    { }
public:
    virtual ~ProgramNode() { }

    virtual ProgramNode *arg( int n ) const = 0;
    virtual VariableName *name() const = 0;

    // a node in an MLisp (minimal lisp) program node
    BoundName *p_proc;        // The dude that executes this node
    unsigned p_active : 1;        // True iff this node is being executed.
    int p_nargs;            // The number of arguments to this node

    enum { MAXNODES = 512 };    // Max number of Mlisp nodes for one node


    static ProgramNode *parse_mlisp_line( const EmacsString &s );

    static int execute_mlisp_stream( MLispInputStream &input );
    static ProgramNode *parse_node( MLispInputStream &stream );
private:
    static void lisp_comment( MLispInputStream &stream );
    static EmacsString parse_name( MLispInputStream &stream );
    static ProgramNode *paren_node( MLispInputStream &stream );
    static ProgramNode *name_node( MLispInputStream &stream );
    static ProgramNode *number_node( MLispInputStream &stream );
    static ProgramNode *string_node( MLispInputStream &stream );

};

class ProgramNodeInt : public ProgramNode
{
public:
    EMACS_OBJECT_FUNCTIONS( ProgramNodeInt )
    ProgramNodeInt( int number )
    : ProgramNode( &bound_number_node )
    , pa_int( number )
    { }
    virtual ~ProgramNodeInt() { }

    virtual ProgramNode *arg( int ) const { return NULL; }
    virtual VariableName *name() const { return NULL; }

    int pa_int;
};

class ProgramNodeString : public ProgramNode
{
public:
    EMACS_OBJECT_FUNCTIONS( ProgramNodeString )
    ProgramNodeString( const EmacsString &str )
    : ProgramNode( &bound_string_node )
    , pa_string( str )
    { }

    virtual ~ProgramNodeString() { }

    virtual ProgramNode *arg( int ) const { return NULL; }
    virtual VariableName *name() const { return NULL; }

    EmacsString pa_string;
};

class ProgramNodeExpression : public ProgramNode
{
public:
    EMACS_OBJECT_FUNCTIONS( ProgramNodeExpression )
    ProgramNodeExpression( Expression &expr )
    : ProgramNode( &bound_expression_node )
    , pa_value( expr )
    { }

    virtual ~ProgramNodeExpression() { }

    virtual ProgramNode *arg( int ) const { return NULL; }
    virtual VariableName *name() const { return NULL; }

    Expression pa_value;
};

class ProgramNodeVariable : public ProgramNode
{
public:
    EMACS_OBJECT_FUNCTIONS( ProgramNodeVariable )
    ProgramNodeVariable( VariableName *name )
    : ProgramNode( &bound_variable_node )
    , pa_name( name )
    { }
    virtual ~ProgramNodeVariable() { }
    virtual VariableName *name() const { return pa_name; }
    virtual ProgramNode *arg( int ) const { return NULL; }

    VariableName *pa_name;
};

class ProgramNodeNode : public ProgramNode
{
public:
    EMACS_OBJECT_FUNCTIONS( ProgramNodeNode )
    ProgramNodeNode( BoundName *proc, int nargs );
    virtual ~ProgramNodeNode();

    virtual ProgramNode *arg( int n ) const { if( n < 1 || n > p_nargs ) return NULL; else return pa_node[n-1]; }
    virtual VariableName *name() const { return NULL; }

    ProgramNode **pa_node;
};

//
// the possible states that the
// prefix-argument scanning could be in
//
enum arg_states
{
    no_arg,
    have_arg,
    prepared_arg
};

class VariableNameTable : public EmacsStringTable
{
public:
    EMACS_OBJECT_FUNCTIONS( VariableNameTable )
    VariableNameTable( int init_size, int grow_amount )
    : EmacsStringTable( init_size, grow_amount )
    { }
    virtual ~VariableNameTable()
    { }

    void add( const EmacsString &key, VariableName *value )
    { EmacsStringTable::add( key, value ); }
    VariableName *remove( const EmacsString &key )
    {
        return (VariableName *)EmacsStringTable::remove( key );
    }
    VariableName *find( const EmacsString &key )
    {
        return (VariableName *)EmacsStringTable::find( key );
    }
    VariableName *value( int index )
    {
        return (VariableName *)EmacsStringTable::value( index );
    }
};

class VariableName : public EmacsObject
{
    // a name for a variable with a pointer
    EMACS_OBJECT_FUNCTIONS( VariableName )
    // to it's chain of interpretations.
public:
    VariableName( const EmacsString &name, Binding *binding );
    virtual ~VariableName();

    static VariableName *find( const EmacsString &name );

    // return one of the keys in the table otherwise NULL
    static VariableName *get_word_mlisp()
    {
        EmacsString result;
        return find( name_table.get_word_mlisp( result ) );
    }
    static VariableName *get_word_interactive( const EmacsString &prompt )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, EmacsString::null, result ) );
    }
    static VariableName *get_word_interactive( const EmacsString &prompt, const EmacsString &default_value )
    {
        EmacsString result;
        return find( name_table.get_word_interactive( prompt, default_value, result ) );
    }

    bool isBound() const
    {
        return v_binding != NULL;
    }

    bool declareGlobal();
    bool declareBufferSpecific();

    bool normalValue( Expression &value ) const;
    bool assignNormal( const Expression &new_value );
    bool defaultValue( Expression &value ) const;
    bool assignDefault( const Expression &new_value ) const;

    void pushBinding();
    void pushBinding( const Expression &value );
    void popBinding();

private:
    void define();

    Binding *resolve_buffer_specific() const;
public:
    const EmacsString v_name;    // the name of the variable

    static VariableNameTable name_table;

private:
    Binding *v_binding;        // the most recent binding of this variable
};
