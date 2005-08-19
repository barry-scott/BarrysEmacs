//
//
//	getfile.cpp
//
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


SystemExpressionRepresentationIntBoolean ignore_version_numbers;

EmacsFileTable::EmacsFileTable()
	: EmacsStringTable( 1024, 1024 )
	{ }

EmacsFileTable::~EmacsFileTable()
	{ }

void EmacsFileTable::makeTable( EmacsString &prefix )
	{
	FileParse fab;

	emptyTable();

	//
	// if we can parse what we have then make that the prompt
	//
	if( fab.sys_parse( prefix, "" ) )
		{
		prefix = fab.result_spec;
		}

	/* need the 'file' with a wild * on the end */
	EmacsString wild_file = prefix;
	wild_file.append( "*" );

	/* Expand to a full path */
	int resp = fab.sys_parse( ALL_FILES, wild_file );
	if( !resp )
		{
		//
		// opss thats a bad path...
		// just return the current directory contents
		//
		resp = fab.sys_parse( ALL_FILES, EmacsString::null );
		}
	if( resp )
		{
		//
		// For each file that matches the filespec, save the name
		// away in the table. Make sure old entries in the table are
		// deallocated first
		//
		FileFind finder( fab.result_spec );

		for(;;)
			{
			EmacsString file( finder.next() );
			if( file.isNull() )
				break;

			static int file_value(1);

			//
			//	duplicate file names can be returned from
			//	samba mounted Unix disks on Windows systems
			//
			if( find( file ) == NULL )	// its not already in the table
				add( file, (void *)&file_value );
			}
		}
	}

//
// return true is the entry allows the get to finish
// for example if entry is a directory don't finish
// but if its a file do finish
//
bool EmacsFileTable::terminalEntry( const EmacsString &entry )
	{
	return !file_is_directory( entry );
	}

int EmacsFileTable::compareKeys( const EmacsString &string1, const EmacsString &string2 )
	{
	return file_name_compare->compare( string1, string2 );
	}

int EmacsFileTable::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
	{
	return file_name_compare->commonPrefix( string1, string2 );
	}

