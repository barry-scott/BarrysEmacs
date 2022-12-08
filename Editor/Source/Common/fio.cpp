//
//    Copyright (c) 1995-2010 Barry A. Scott
//
//    fio.c
//
#include <emacs.h>
#include <emunicode.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <em_stat.h>

EmacsFileImplementation::EmacsFileImplementation( EmacsFile &parent )
: EmacsObject()
, m_parent( parent )
{ }

EmacsFileImplementation::~EmacsFileImplementation()
{ }

EmacsFile::EmacsFile( const EmacsString &filename, const EmacsString &def, FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, impl( 0 )
{
    impl = EMACS_NEW EmacsFileLocal( *this, attr );
    parse_filename( filename, def );
}

EmacsFile::EmacsFile( const EmacsString &filename, FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, impl( 0 )
{
    impl = EMACS_NEW EmacsFileLocal( *this, attr );
    parse_filename( filename, EmacsString::null );
}

EmacsFile::EmacsFile( FIO_EOL_Attribute attr )
: remote_host()
, disk()
, path()
, filename()
, filetype()
, result_spec()
, wild( false )
, filename_maxlen( 0 )
, filetype_maxlen( 0 )
, file_case_sensitive( false )
, parse_valid( false )
, impl( 0 )
{
    impl = EMACS_NEW EmacsFileLocal( *this, attr );
}

EmacsFile::~EmacsFile()
{
    delete impl;
}

bool EmacsFile::parse_is_valid()
{
    return parse_valid;
}

