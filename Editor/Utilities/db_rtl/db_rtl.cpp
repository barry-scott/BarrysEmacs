//
//    db_rtl.cpp    - run time support for the dbxxx utilities
//
#include <emacsutl.h>
#include <stdlib.h>
#include <emobject.h>
#include <emstring.h>
#include <fileserv.h>
#include <emstrtab.h>
#include <ndbm.h>

//static const char *THIS_FILE = __FILE__;

#if defined( SAVEENVIRONMENT )
void database::SaveEnvironment()
{ }

void FileParse::SaveEnvironment()
{ }

void DatabaseSearchList::SaveEnvironment()
{ }

void DatabaseSearchListTable::SaveEnvironment()
{ }
#endif

EmacsStringTable::EmacsStringTable( int , int )
{ }

EmacsString &EmacsStringTable::get_esc_word_mlisp( EmacsString &result )
{
    return result;
}

DatabaseSearchList::~DatabaseSearchList()
{ }
DatabaseSearchListTable DatabaseSearchList::name_table( 8, 8 );

EmacsString &EmacsStringTable::get_word_mlisp( EmacsString &result )
{ return result; }
void *EmacsStringTable::value( int  )
{
    return NULL;
}
void *EmacsStringTable::find( const EmacsString & )
{
    return NULL;
}
void EmacsStringTable::add( const EmacsString &, void * )
{ }
void *EmacsStringTable::remove( const EmacsString & )
{
    return NULL;
}


int expand_and_default( const EmacsString &nm, const EmacsString &def, EmacsString &buf)
{
    FileParse fab;

    if( fab.sys_parse( nm, def ) )
        buf = fab.result_spec;
    else
        buf = "";

    return 0;
}

int get_file_parsing_override(char const *,int x)
{
    return x;
}
