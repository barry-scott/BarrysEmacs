//
//
//    getfile.cpp
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
    emptyTable();

    {
    EmacsFile fab( prefix );

    //
    // if we can parse what we have then make that the prompt
    //
    if( fab.parse_is_valid() )
    {
        prefix = fab.fio_getname();
    }
    else
    {
        prefix = EmacsString::null;
    }
    }

    // need the 'file' with a wild * on the end
    EmacsString wild_file = prefix;
    wild_file.append( "*" );

    EmacsFile *fab = EMACS_NEW EmacsFile( wild_file );
    if( fab->parse_is_valid() )
    {
        //
        // For each file that matches the filespec, save the name
        // away in the table. Make sure old entries in the table are
        // deallocated first
        //
        FileFind finder( fab );

        for(;;)
        {
            EmacsString file( finder.next() );
            if( file.isNull() )
                break;

            static int file_value(1);

            //
            //    duplicate file names can be returned from
            //    samba mounted Unix disks on Windows systems
            //
            if( find( file ) == NULL )    // its not already in the table
            {
                add( file, (void *)&file_value );
            }
        }
    }
    else
    {
        delete fab;
    }
}

//
// return true is the entry allows the get to finish
// for example if entry is a directory don't finish
// but if its a file do finish
//
bool EmacsFileTable::terminalEntry( const EmacsString &entry )
{
    return !EmacsFile( entry ).fio_is_directory();
}

int EmacsFileTable::compareKeys( const EmacsString &string1, const EmacsString &string2 )
{
    return file_name_compare->compare( string1, string2 );
}

int EmacsFileTable::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
{
    return file_name_compare->commonPrefix( string1, string2 );
}

