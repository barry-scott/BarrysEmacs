//module macros
//    (
//    ident    = 'V5.0 Emacs',
//    addressing_mode( nonexternal=long_relative, external=general )
//    ) =
//begin
//    Copyright (c) 1982, 1983, 1984, 1985
//        Barry A. Scott and Nick Emery

// Stuff to do with the manipulation of macros.
// For silly historical reasons, several routines that should be here
// are actually in options. (eg. the command level callers).
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );



int find_mac( const EmacsString s );
int edit_macro( void );
int define_buffer_macro( void );
int rename_macro( void );

unsigned int *new_names;    // bound_name points into the list of bound macro names; used for init_ialization

//
//    BoundNameInside implementation
//
BoundNameInside::BoundNameInside()
{ }

BoundNameInside::~BoundNameInside()
{ }

int BoundNameInside::canDelete() const
{
    return 1;
}

KeyMap *BoundNameInside::getKeyMap(void) const
{
    return NULL;
}

EmacsMacroString BoundNameInside::getMacro(void) const
{
    return EmacsString::null;
}

ProgramNode *BoundNameInside::getProcedure(void) const
{
    return NULL;
}

//
//    BoundNameMacro implementation
//
BoundNameMacro::BoundNameMacro( EmacsMacroString macro_body )
    : b_body( (const EmacsString &)macro_body )
{ }

BoundNameMacro::~BoundNameMacro()
{ }

EmacsMacroString BoundNameMacro::getMacro(void) const
{
    return b_body;
}

//
//    BoundNameAutoload implementation
//
BoundNameAutoLoad::BoundNameAutoLoad( const EmacsString &module_name )
    : b_module( module_name )
{ }

BoundNameAutoLoad::~BoundNameAutoLoad()
{ }

//
//    BoundNameBuiltin implementation
//
BoundNameBuiltin::BoundNameBuiltin( int (*builtin_function)(void) )
    : b_builtin( builtin_function )
{ }

BoundNameBuiltin::~BoundNameBuiltin()
{
    fatal_error(9999);    // cannot every delete a builtin command
}

int BoundNameBuiltin::canDelete(void) const    // true if you can delete this item
{
    return 0;    // cannot delete a builtin command
}

//
//    BoundNameProcedure implementation
//
BoundNameProcedure::BoundNameProcedure( ProgramNode *mlisp_body )
    : b_prog( mlisp_body )
{ }

BoundNameProcedure::~BoundNameProcedure()
{
    delete b_prog;
}

ProgramNode *BoundNameProcedure::getProcedure(void) const
{
    return b_prog;
}

//
//    BoundNameKeymap implementation
//

BoundNameKeymap::BoundNameKeymap( KeyMap *keymap )
    : b_keymap( keymap )
{ }

BoundNameKeymap::~BoundNameKeymap()
{
    EmacsBuffer *b = buffers;

    while( b != NULL )
    {
        if( b->b_mode.md_keys  == b_keymap )
            b->b_mode.md_keys  = NULL;
        b = b->b_next;
    }
    if( current_global_map == b_keymap )
        current_global_map = global_map;
    if( bf_cur->b_mode.md_keys == b_keymap )
        bf_cur->b_mode.md_keys = NULL;
    next_local_keymap = next_global_keymap = NULL;

    delete b_keymap;
}

KeyMap *BoundNameKeymap::getKeyMap(void) const
{
    return b_keymap;
}

//
//    BoundNameExternalFunction implementation
//
BoundNameExternalFunction::BoundNameExternalFunction( EmacsExternFunction *external_function )
    : b_func( external_function )
{ }

BoundNameExternalFunction::~BoundNameExternalFunction()
{
    delete b_func;
}

BoundName::BoundName( const EmacsString &name )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
 {
    implementation = NULL;
    define();
}

BoundName::BoundName( const EmacsString &name, const EmacsString &module )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameAutoLoad( module );
    define();
}

BoundName::BoundName( const EmacsString &name, const EmacsMacroString &macro )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameMacro( macro );
    define();
}

BoundName::BoundName( const EmacsString &name, int (*builtin_function)(void) )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameBuiltin( builtin_function );
    define();
}

BoundName::BoundName( const EmacsString &name, ProgramNode *mlisp_body )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameProcedure( mlisp_body );
    define();
}

BoundName::BoundName( const EmacsString &name, KeyMap *keymap )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameKeymap( keymap );
    define();
}

BoundName::BoundName( const EmacsString &name, EmacsExternFunction *external_function )
    : b_proc_name( name )
    , b_active( 0 )
    , b_break( 0 )
{
    implementation = EMACS_NEW BoundNameExternalFunction( external_function );
    define();
}

int BoundName::canDelete(void) const
{
    if( implementation && !implementation->canDelete() )
    {
        error( FormatString("%s is already bound to a wired procedure!") << b_proc_name );
        return 0;
    }

    return 1;
}

bool BoundName::IsAKeyMap(void) const
{
    if( implementation )
        return implementation->getKeyMap() != NULL;
    return false;
}

bool BoundName::IsAMacro(void) const
{
    if( implementation )
        return !implementation->getMacro().isNull();
    return false;
}

bool BoundName::IsAProcedure(void) const
{
    if( implementation )
        return implementation->getProcedure() != NULL;
    return false;
}

KeyMap *BoundName::getKeyMap(void) const
{
    if( implementation )
        return implementation->getKeyMap();
    return NULL;
}

EmacsMacroString BoundName::getMacro(void) const
{
    if( implementation )
        return implementation->getMacro();
    return EmacsString::null;
}

ProgramNode *BoundName::getProcedure(void) const
{
    if( implementation )
        return implementation->getProcedure();
    return NULL;
}

int BoundName::isBound(void) const
{
    return implementation != NULL;
}

int BoundName::replaceInsideHelper(void)
{
    if( implementation != NULL )
    {
        if( !canDelete() )
            return 0;

        delete implementation;
        implementation = NULL;

        return 1;
    }
    return 1;
}

void BoundName::replaceInside(void)
{
    replaceInsideHelper();
}

void BoundName::replaceInside( const EmacsMacroString &macro )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameMacro( macro );
}
void BoundName::replaceInside( const EmacsString &module )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameAutoLoad( module );
}
void BoundName::replaceInside( ProgramNode *mlisp_body )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameProcedure( mlisp_body );
}

void BoundName::replaceInside( KeyMap *keymap )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameKeymap( keymap );
}

void BoundName::replaceInside( EmacsExternFunction *external_function )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameExternalFunction( external_function );
}

void BoundName::replaceInside( int (*builtin_function)(void) )
{
    if( replaceInsideHelper() )
        implementation = EMACS_NEW BoundNameBuiltin( builtin_function );
}

BoundNameNoDefine::BoundNameNoDefine( const EmacsString &name, int (*builtin_function)(void) )
    : BoundName( EmacsString::null, builtin_function )
{
    b_proc_name = name;
}

BoundNameNoDefine::BoundNameNoDefine( const EmacsString &name, ProgramNode *mlisp_body )
    : BoundName( EmacsString::null, mlisp_body )
{
    b_proc_name = name;
}

//
//    commandTypeName
//
const char *BoundName::commandTypeName() const
{
    if( implementation != NULL )
        return implementation->commandTypeName();
    else
        return "an unbound procedure";
}

const char *BoundNameMacro::commandTypeName() const
{
    return "a macro";
}

const char *BoundNameAutoLoad::commandTypeName() const
{
    return "an autoload function";
}

const char *BoundNameBuiltin::commandTypeName() const
{
    return "a wired-in procedure";
}

const char *BoundNameProcedure::commandTypeName() const
{
    return "an MLisp procedure";
}

const char *BoundNameKeymap::commandTypeName() const
{
    return "a key map";
}

const char *BoundNameExternalFunction::commandTypeName() const
{
    return "an external function";
}


//
//    this define does nothing
//

// Find the index of the named macro or command; -(index of where the
// name should have been if it isn't found)-1.
void BoundName::define(void)
{
    if( b_proc_name.isNull() )
        return;

    if( name_table.find( b_proc_name ) != NULL )
        error( FormatString("Procedure %s is already defined!") << b_proc_name );
    else
        name_table.add( b_proc_name, this );
}

int edit_macro( void )
{
    BoundName *proc = getword( BoundName::, ": edit-macro " );

    if( proc == NULL )
        return 0;

    EmacsMacroString macro( proc->getMacro() );

    if( macro.isNull() )
    {
        error( FormatString("%s is a procedure, not a macro!") <<
            proc->b_proc_name );
        return 0;
    }

    EmacsBuffer::set_bfn("Macro edit");
    bf_cur->erase_bf();
    bf_cur->b_fname = proc->b_proc_name;
    bf_cur->b_kind = MACROBUFFER;
    theActiveView->window_on( bf_cur );

    bf_cur->ins_cstr( macro );
    bf_cur->b_modified = 0;
    beginning_of_file();

    return 0;
}

int define_buffer_macro( void )
{
    if( bf_cur->b_kind != MACROBUFFER
    || bf_cur->b_buf_name.isNull()
    || bf_cur->b_fname.isNull() )
    {
        error( "This buffer does not contain a named macro.");
        return 0;
    }
    bf_cur->gap_to( bf_cur->unrestrictedSize() + 1 ); // ignoring our abstract data type hiding

    BoundName *cmd = BoundName::find( bf_cur->b_fname );

    if( cmd != NULL )
        cmd->replaceInside( EmacsMacroString( EmacsString( EmacsString::copy, bf_cur->ref_char_at(1), bf_cur->unrestrictedSize()  ) ) );
    else
    {
        cmd = EMACS_NEW BoundName( bf_cur->b_fname,
            EmacsMacroString( EmacsString( EmacsString::copy, bf_cur->ref_char_at(1), bf_cur->unrestrictedSize()) ) );
    }

    bf_cur->b_modified = 0;

    return 0;
}

// MLisp function to rename Macros
int rename_macro( void )
{
    return BoundName::rename_macro();
}

int BoundName::rename_macro( void )
{
    BoundName *from_body = getword( BoundName::, ": rename-mlisp-procedure (old name) " );

    //
    //    Get all the rename info
    //
    if( from_body == NULL )
        return 0;

    EmacsString to_name;
    if( cur_exec == NULL )
        to_name = get_string_interactive( FormatString(": rename-mlisp-procedure (old name) %s to (new name) ") << from_body->b_proc_name );
    else
        to_name = get_string_mlisp();
    if( to_name.isNull() )
        return 0;

    //
    // If we are renaming into an active bound_name, deallocate the
    // old bound data structure
    //
    BoundName *to_body = BoundName::find( to_name );
    if( to_body != NULL )
    {
        if( !to_body->canDelete() )
        {
            error(FormatString("\"%s\" is bound to a wired procedure and cannot be redefined") << to_name);
            return 0;
        }
        to_body->replaceInside();
    }
    else
        to_body = EMACS_NEW BoundName( to_name );

    //
    //    Move the implementation over
    //
    to_body->implementation = from_body->implementation;
    from_body->implementation = 0;

    return 0;
}
