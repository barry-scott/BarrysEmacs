//
//	string_map.cpp
//
//	Copyright (c) 2000 Barry A. Scott
//
//
#include <emacs.h>
#include <string_map.h>

StringMapBase::StringMapBase( char **_strings, int *_values )
	: strings( _strings )
	, values( _values )
	{ }

StringMapBase::~StringMapBase()
	{ }


bool StringMapBase::map( const EmacsString &key, int &value )
	{
	char **p = strings;

	for( int i=0; ; i += 1 )
		{
		if( p[i] == NULL )
			break;
		if( key == p[i] )
			{
			value = values[i];
			return true;
			}
		}

	return false;
	}

EmacsString StringMapBase::map( int value )
	{
	for( int i=0; strings[i] != NULL; i++ )
		if( values[i] == value )
			return strings[i];
	return "Unknown";
	}
