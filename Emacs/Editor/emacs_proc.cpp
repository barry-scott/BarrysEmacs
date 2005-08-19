//
//	emacs_proc.cpp
//		copyright (c) 1995-1996 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <emacs_proc.h>

EmacsProcessNameTable EmacsProcessCommon::name_table( 8, 8 );

EmacsProcessCommon::EmacsProcessCommon( const EmacsString &name )
	: proc_name( name )
	{
	// this cast is not very nice!
	name_table.add( name, (EmacsProcess *)this );
	}

EmacsProcessCommon::~EmacsProcessCommon()
	{
	name_table.remove( proc_name );
	}

EmacsThreadCommon::EmacsThreadCommon()
	{}

EmacsThreadCommon::~EmacsThreadCommon()
	{}
