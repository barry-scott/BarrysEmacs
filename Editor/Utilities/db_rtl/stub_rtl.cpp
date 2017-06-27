//
//    stub_rtl.cpp    - run time support for the dbxxx utilities
//
#include <emacsutl.h>
#include <stdlib.h>
#include <string>
#include <emobject.h>
#include <emstring.h>
#include <fileserv.h>
#include <emstrtab.h>
#include <ndbm.h>
#include <assert.h>

int dbg_flags = 0;

EmacsObject::EmacsObject()
{ }

EmacsObject::~EmacsObject()
{ }

#undef malloc
#undef realloc
#undef free
void *EmacsObject::operator new( size_t size )
{
    return malloc( size );
}

void EmacsObject::operator delete( void *p )
{
    if( p )
        free( p );
}

#if DBG_ALLOC_CHECK
void *EmacsObject::operator new( size_t size, const char *, int )
{
    return malloc( size );
}
#endif

int EmacsObject::NextObjectNumber(0);

# if DBG_ALLOC_CHECK
void *emacs_malloc( int size, enum malloc_block_type, const char *, int )
{
    return malloc( size );
}
void *emacs_realloc( void *p, int new_size, enum malloc_block_type, const char *, int )
{
    return realloc( p, new_size );
}
# else
void *emacs_malloc( int size, enum malloc_block_type )
{
    return malloc( size );
}
void *emacs_realloc( void *p, int new_size, enum malloc_block_type )
{
    return realloc( p, new_size );
}
# endif
void emacs_free( void *p )
{
    free( p );
}

void emacs_check_malloc_block( void * )
{
    return;
}

void debug_invoke(void)
{
    return;
}

void debug_SER(void)
{
    return;
}

void debug_exception(void)
{
    return;
}

#ifdef SAVEENVIRONMENT
void EmacsString::SaveEnvironment()
{ }

void EmacsStringRepresentation::SaveEnvironment()
{ }

void FormatString::SaveEnvironment()
{ }

void EmacsStringTable::SaveEnvironment()
{ }
#endif

EmacsString get_config_env( const EmacsString &name )
{
    char *value = getenv( name.sdata() );

    if( value != NULL )
        return value;

    return EmacsString::null;
}

EmacsString get_device_name_translation( const EmacsString &name )
{
    return EmacsString::null;
}


//
//
//    EmacsInitialisation
//
//
bool EmacsInitialisation::init_done = false;
time_t EmacsInitialisation::most_recent_built_module = 0;

EmacsInitialisation::EmacsInitialisation( const char *, const char * )
{
    if( init_done )
        return;

    init_done = true;

    EmacsString::init();
}

EmacsInitialisation::~EmacsInitialisation()
{ }

void _dbg_msg( const EmacsString &msg )
{
    fprintf( stdout,"%s", msg.sdata() );
    if( msg[-1] != '\n' )
        fprintf( stdout, "\n" );
    fflush( stdout );
}

void EmacsStringTable::makeTable( EmacsString & )
{ }

bool EmacsStringTable::terminalEntry( const EmacsString & )
{
    return true;
}

EmacsStringTable::~EmacsStringTable()
{ }

EmacsString &EmacsStringTable::get_word_interactive(EmacsString const &, EmacsString const &, EmacsString &x)
{
    return x;
}

EmacsString &EmacsStringTable::get_esc_word_interactive(EmacsString const &, EmacsString const &, EmacsString &x)
{
    return x;
}

int EmacsStringTable::compareKeys( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.compare( string2 );
}

int EmacsStringTable::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.commonPrefix( string2 );
}

EmacsString os_error_code( unsigned int code )
{
    return EmacsString( FormatString( "Win32 error code: 0x%x" ) << code );
}
