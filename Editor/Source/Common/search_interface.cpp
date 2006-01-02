//
//    search_pointer.cpp
//
//    Copyright Barry A. Scott (c) 2002
//
#include <emacs.h>

#include <search_simple_algorithm.h>
#include <search_extended_algorithm.h>


SearchImplementation::SearchImplementation()
    : ref_count( 1 )
{}

SearchImplementation::~SearchImplementation()
{
    emacs_assert( ref_count == 0 );
}

EmacsSearch::EmacsSearch()
    : pointer( NULL )
{}

EmacsSearch::EmacsSearch( EmacsSearch &other )
    : pointer( other.pointer )
{
    if( pointer != NULL )
        pointer->ref_count++;
}

EmacsSearch::~EmacsSearch()
{
    if( pointer != NULL )
    {
        pointer->ref_count--;
        if( pointer->ref_count == 0 )
        {
            delete pointer;
            pointer = NULL;
        }
    }
}

SearchImplementation *EmacsSearch::operator->()
{
    return pointer;
}

EmacsSearch &EmacsSearch::operator=( EmacsSearch &other )
{
    if( other.pointer != NULL )
        other.pointer->ref_count++;

    if( pointer != NULL )
    {
        pointer->ref_count--;
        if( pointer->ref_count == 0 )
        {
            delete pointer;
            pointer = NULL;
        }
    }

    pointer = other.pointer;
    return *this;
}

void EmacsSearch::compile( const EmacsString &pattern, EmacsSearch::sea_type RE )
{
    if( pattern.isNull() )
    {
        // use existing pattern
        if( pointer == NULL )
        {
            error("null search string");
            return;
        }
        return;
    }

    if( pointer != NULL && !pointer->is_compatible( RE ) )
    {
        pointer->ref_count--;

        if( pointer->ref_count == 0 )
            delete pointer;

        pointer = NULL;
    }

    if( pointer == NULL )
    {
        switch( RE )
        {
        case sea_type__string:
        case sea_type__RE_simple:
            pointer = new SearchSimpleAlgorithm;
            break;
        case sea_type__RE_extended:
            pointer = new SearchAdvancedAlgorithm;
            break;
        default:
            error( "Unsupported search type" );
            return;
        }
    }

    pointer->compile( pattern, RE );
}

int EmacsSearch::search( const EmacsString &s, int n, int dot, EmacsSearch::sea_type RE )
{
    compile( s, RE );
    if( ml_err )
        return 0;

    if( pointer != NULL )
        return pointer->search( n, dot );
    else
        return 0;
}

int EmacsSearch::search( int n, int dot )
{
    if( pointer != NULL )
        return pointer->search( n, dot );
    else
        return 0;
}

int EmacsSearch::looking_at( const EmacsString &s, EmacsSearch::sea_type RE )
{
    compile( s, RE );

    if( ml_err )
        return 0;

    if( pointer != NULL )
        return pointer->looking_at();
    else
        return 0;
}

void EmacsSearch::search_replace_once( const EmacsString &new_string )
{
    if( pointer != NULL )
        pointer->search_replace_once( new_string );
    else
        error( "search-replace-once called before a search");
}

int EmacsSearch::get_number_of_groups()
{
    if( pointer == NULL )
        return -1;

    return pointer->get_number_of_groups();
}

int EmacsSearch::get_start_of_group( int group_number )
{
    if( pointer == NULL )
        return -1;

    return pointer->get_start_of_group( group_number );
}

int EmacsSearch::get_end_of_group( int group_number )
{
    if( pointer == NULL )
        return -1;

    return pointer->get_end_of_group( group_number );
}
