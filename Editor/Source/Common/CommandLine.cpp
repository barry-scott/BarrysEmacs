//
//    CommandLine.h
//
//    Copyright (c) 1997-2010 Barry A. Scott
//
#include <emacs.h>

// transfer the command line from new_command_line to this object
EmacsCommandLine::EmacsCommandLine()
: m_count(0)
, m_no_more_qualifers( false )
{
    for( int arg=0; arg<MAX_ARGUMENTS; arg++ )
    {
        m_arguments[ arg ] = NULL;
    }
}

//
//    Take a Unix style command line
//
void EmacsCommandLine::setArguments( int argc, char **argv )
{
    deleteArguments();
    m_no_more_qualifers = false;

    if( argc > MAX_ARGUMENTS )
        argc = MAX_ARGUMENTS;

    for( int arg = 0; arg < argc; arg++ )
    {
        EmacsString str( argv[ arg ] );
        addArgument( str );
    }
}

void EmacsCommandLine::setArguments( const EmacsString &command_line )
{
    deleteArguments();
    m_no_more_qualifers = false;

    EmacsString value;

    for( int pos=0, len=command_line.length(); pos < len; pos++ )
    {
        EmacsChar_t ch = command_line[ pos ];

        if( ch == 0 )
        {
            addArgument( value );
            value = EmacsString::null;
        }
        else
        {
            value.append( ch );
        }
    }

    if( !value.isNull() )
    {
        addArgument( value );
    }
}

void EmacsCommandLine::addArgument( const EmacsString &value )
{
    if( value == "--" )
    {
        m_no_more_qualifers = true;
        return;
    }

    bool is_qual =
#ifndef __unix__
        value[0] == '/' ||
#endif
        value[0] == '+' ||
        value[0] == '-';

    m_arguments[ m_count ] = new EmacsArgument( is_qual && !m_no_more_qualifers, value );
    m_count++;
}

EmacsCommandLine &EmacsCommandLine::operator=( const EmacsCommandLine &new_command_line )
{
    // must not assign to self
    if( this == &new_command_line )
        return *this;

    deleteArguments();
    moveArguments( new_command_line );

    return *this;
}

void EmacsCommandLine::deleteArgument( int arg )
{
    emacs_assert( arg < m_count );

    delete m_arguments[ arg ];

    int i; for( i=arg; i<m_count; i++ )
        m_arguments[i] = m_arguments[i+1];

    m_arguments[i] = NULL;
    m_count--;
}

void EmacsCommandLine::setArgument( int arg, const EmacsString &new_value, bool is_qual )
{
    emacs_assert( arg < m_count );

    m_arguments[ arg ]->m_arg_value = new_value;
    m_arguments[ arg ]->m_is_qualifier = is_qual;
}

int EmacsCommandLine::argumentCount() const
{
    return m_count;
}

const EmacsArgument &EmacsCommandLine::argument( int n ) const
{
    emacs_assert( n < m_count );
    emacs_assert( m_arguments[n] != NULL );

    return *m_arguments[n];
}

void EmacsCommandLine::deleteArguments()
{
    for( int arg=0; arg<m_count; arg++ )
    {
        delete m_arguments[ arg ];
        m_arguments[ arg ] = NULL;
    }

    m_count = 0;
}

void EmacsCommandLine::moveArguments( const EmacsCommandLine &other )
{
    emacs_assert( this != &other );
    emacs_assert( m_count == 0 );

    m_count = other.m_count;

    for( int arg=0; arg<m_count; arg++ )
    {
        m_arguments[ arg ] = new EmacsArgument( *other.m_arguments[ arg ] );
    }
}
