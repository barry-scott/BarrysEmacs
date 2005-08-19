//
//
//	file_name_compare.cpp
//
//	Copyright (c) 2000 Barry A. Scott
//
//


#include	<emacsutl.h>
#include	<emobject.h>
#include	<emstring.h>
#include	<em_stat.h>
#include	<fileserv.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


//
//	Implementation
//
bool FileNameCompareCaseSensitive::isEqual( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.compare( string2 ) == 0;
	}

int FileNameCompareCaseSensitive::compare( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.compare( string2 );
	}

int FileNameCompareCaseSensitive::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.commonPrefix( string2 );
	}

bool FileNameCompareCaseBlind::isEqual( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.caseBlindCompare( string2 ) == 0;
	}

int FileNameCompareCaseBlind::compare( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.caseBlindCompare( string2 );
	}

int FileNameCompareCaseBlind::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
	{
	return string1.caseBlindCommonPrefix( string2 );
	}

