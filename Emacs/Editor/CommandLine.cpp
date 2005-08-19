//
//	CommandLine.h
//
//	Copyright (c) 1997 Barry A. Scott
//
#include <emacs.h>

// transfer the command line from new_command_line to this object
EmacsCommandLine::EmacsCommandLine()
	: count(0)
	, no_more_qualifers( false )
	{
	for( int arg=0; arg<MAX_ARGUMENTS; arg++ )
		arguments[arg] = NULL;
	}

//
//	Take a Unix style command line
//

void EmacsCommandLine::setArguments( int argc, char **argv )
	{
	deleteArguments();
	no_more_qualifers = false;

	if( argc > MAX_ARGUMENTS )
		argc = MAX_ARGUMENTS;

	for( int arg = 0; arg < argc; arg++ )
		{
		EmacsString str( argv[arg] );
		addArgument( str );
		}
	}


void EmacsCommandLine::setArguments( const EmacsString &command_line )
	{
	deleteArguments();
	no_more_qualifers = false;

	bool skip_spaces = true;
	bool in_quote = false;

	EmacsString value;

	for( int pos=0, len=command_line.length(); pos < len; pos++ )
		{
		unsigned char ch = command_line[pos];

		if( ch == ' ' && !in_quote )
			{
			if( !skip_spaces )
				{
				skip_spaces = true;
				addArgument( value );
				value = EmacsString::null;
				}
			}
		else
			{
			skip_spaces = false;
			if( ch == '"' )
				in_quote = !in_quote;
			else
				value.append( ch );
			}
		}
	if( !value.isNull() )
		addArgument( value );
	}



void EmacsCommandLine::addArgument( EmacsString &value )
	{
	if( value == "--" )
		{
		no_more_qualifers = true;
		return;
		}

	bool is_qual = 
#ifndef __unix__
		value[0] == '/' ||
#endif
		value[0] == '+' ||
		value[0] == '-';
	
	arguments[count] = new EmacsArgument( is_qual && !no_more_qualifers, value );
	count++;
	}


EmacsCommandLine &EmacsCommandLine::operator=( EmacsCommandLine &new_command_line )
	{
	// must not assign to self
	emacs_assert( this != &new_command_line );

	deleteArguments();
	moveArguments( new_command_line );

	return *this;
	}

void EmacsCommandLine::deleteArgument( int arg )
	{
	emacs_assert( arg < count );

	delete arguments[arg];

	int i; for( i=arg; i<count; i++ )
		arguments[i] = arguments[i+1];

	arguments[i] = NULL;
	count--;
	}

void EmacsCommandLine::setArgument( int arg, const EmacsString &new_value, bool is_qual )
	{
	emacs_assert( arg < count );

 	arguments[arg]->arg_value = new_value;
	arguments[arg]->is_qualifier = is_qual;
	}

int EmacsCommandLine::argumentCount() const
	{
	return count;
	}

const EmacsArgument &EmacsCommandLine::argument( int n ) const
	{
	emacs_assert( n < count );
	emacs_assert( arguments[n] != NULL );

	return *arguments[n];
	}

void EmacsCommandLine::deleteArguments()
	{
	for( int arg=0; arg<MAX_ARGUMENTS; arg++ )
		{
		delete arguments[arg];
		arguments[arg] = NULL;
		}

	count = 0;
	}

void EmacsCommandLine::moveArguments( EmacsCommandLine &new_command_line )
	{
	emacs_assert( count == 0 );

	count = new_command_line.count;
	new_command_line.count = 0;

	for( int arg=0; arg<MAX_ARGUMENTS; arg++ )
		{
		arguments[arg] = new_command_line.arguments[arg];
		new_command_line.arguments[arg] = NULL;
		}
	}
