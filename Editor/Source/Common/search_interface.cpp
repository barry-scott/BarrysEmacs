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
: m_search_implementation( NULL )
{}

EmacsSearch::EmacsSearch( EmacsSearch &other )
: m_search_implementation( other.m_search_implementation )
{
    if( m_search_implementation != NULL )
        m_search_implementation->ref_count++;
}

EmacsSearch::~EmacsSearch()
{
    if( m_search_implementation != NULL )
    {
        m_search_implementation->ref_count--;
        if( m_search_implementation->ref_count == 0 )
        {
            delete m_search_implementation;
            m_search_implementation = NULL;
        }
    }
}

SearchImplementation *EmacsSearch::operator->()
{
    return m_search_implementation;
}

EmacsSearch &EmacsSearch::operator=( EmacsSearch &other )
{
    if( other.m_search_implementation != NULL )
        other.m_search_implementation->ref_count++;

    if( m_search_implementation != NULL )
    {
        m_search_implementation->ref_count--;
        if( m_search_implementation->ref_count == 0 )
        {
            delete m_search_implementation;
            m_search_implementation = NULL;
        }
    }

    m_search_implementation = other.m_search_implementation;
    return *this;
}

void EmacsSearch::compile( const EmacsString &pattern, EmacsSearch::sea_type RE )
{
    if( pattern.isNull() )
    {
        // use existing pattern
        if( m_search_implementation == NULL )
        {
            error("null search string");
            return;
        }
        return;
    }

    if( m_search_implementation != NULL && !m_search_implementation->is_compatible( RE ) )
    {
        m_search_implementation->ref_count--;

        if( m_search_implementation->ref_count == 0 )
            delete m_search_implementation;

        m_search_implementation = NULL;
    }

    if( m_search_implementation == NULL )
    {
        switch( RE )
        {
        case sea_type__string:
        case sea_type__RE_simple:
            m_search_implementation = new SearchSimpleAlgorithm;
            break;
        case sea_type__RE_extended:
        case sea_type__RE_syntax:
            m_search_implementation = new SearchAdvancedAlgorithm;
            break;
        default:
            error( "Unsupported search type" );
            return;
        }
    }

    m_search_implementation->compile( pattern, RE );
}

int EmacsSearch::search( const EmacsString &s, int n, int dot, EmacsSearch::sea_type RE )
{
    compile( s, RE );
    if( ml_err )
        return 0;

    if( m_search_implementation != NULL )
        return m_search_implementation->search( n, dot );
    else
        return 0;
}

int EmacsSearch::search( int n, int dot )
{
    if( m_search_implementation != NULL )
        return m_search_implementation->search( n, dot );
    else
        return 0;
}

int EmacsSearch::looking_at( const EmacsString &s, EmacsSearch::sea_type RE )
{
    compile( s, RE );

    if( ml_err )
        return 0;

    if( m_search_implementation != NULL )
        return m_search_implementation->looking_at( dot );
    else
        return 0;
}

int EmacsSearch::syntax_looking_at( int pos )
{
    if( m_search_implementation != NULL )
    {
        return m_search_implementation->syntax_looking_at( dot );
    }
    else
    {
        return 0;
    }
}

void EmacsSearch::search_replace_once( const EmacsString &new_string )
{
    if( m_search_implementation != NULL )
        m_search_implementation->search_replace_once( new_string );
    else
        error( "search-replace-once called before a search");
}

int EmacsSearch::get_number_of_groups()
{
    if( m_search_implementation == NULL )
        return -1;

    return m_search_implementation->get_number_of_groups();
}

int EmacsSearch::get_start_of_group( int group_number )
{
    if( m_search_implementation == NULL )
        return -1;

    return m_search_implementation->get_start_of_group( group_number );
}

int EmacsSearch::get_end_of_group( int group_number )
{
    if( m_search_implementation == NULL )
        return -1;

    return m_search_implementation->get_end_of_group( group_number );
}
