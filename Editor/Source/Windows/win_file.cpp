//
//    windows File services.cpp
//    Copyright 1993-2018 Barry A. Scott
//
#include    <emacsutl.h>
#include    <emobject.h>
#include    <emstring.h>
#include    <em_stat.h>
#include    <fileserv.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


extern EmacsString get_device_name_translation( const EmacsString &name );
extern int get_file_parsing_override( const char *disk, int def_override );

static EmacsString convertShortPathToLongPath( const EmacsString &short_path );


FileNameCompareCaseSensitive file_name_compare_case_sensitive;
FileNameCompareCaseBlind file_name_compare_case_blind;
FileNameCompare *file_name_compare = &file_name_compare_case_blind;

#include    <win_incl.h>
#include    <direct.h>

bool isValidFilenameChar( EmacsChar_t ch )
{
    EmacsString invalid( "\\:/\000?<>*|\"" );
    return invalid.index( ch ) < 0;
}

int file_is_regular( const EmacsString &file )
{
    if( file.isNull() )
        return 0;

    return 1;
}

int file_is_directory( const EmacsString &file )
{
    DWORD attr = GetFileAttributes( file.utf16_data() );
    if( attr == (unsigned)-1 )
        return 0;

    return (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

FileParse::FileParse()
: disk()                    // disk: or //server/service
, path()                    // /path/
, filename()                // name
, filetype()                // .type
, result_spec()             // full file spec with all fields filled in
, wild( false )             // true if any field is wild
, filename_maxlen( 0 )      // how long filename can be
, filetype_maxlen( 0 )      // how long filetype can be
, file_case_sensitive( 0 )  // true if case is important
, valid( false )
{ }

void FileParse::init()
{
}

FileParse::~FileParse()
{ }

static unsigned char null[1] = {0};

int FileParse::analyse_filespec( const EmacsString &filespec )
{
    EmacsString sp;
    int device_loop_max_iterations = 10;

    init();

    sp = filespec;
    for( int i=0; i<sp.length(); i++ )
    {
        if( sp[i] == PATH_ALT_CH )
            sp[i] = PATH_CH;
    }

#ifndef _CONSOLE
device_loop:
#endif
    int disk_end = sp.first(':');
    if( disk_end > 0 )
    {
        disk = sp( 0, disk_end );
        disk_end++;

#ifdef _CONSOLE
        disk.append( ":" );
#else

        //
        // if there is a replacement string use it otherwise
        // leave the device name as it is
        //
        EmacsString new_value = get_device_name_translation( disk );

        if( new_value.isNull() )
        {
            disk.append( ":" );
        }
        else
        {
            // we are replacing the disk so zap any left over disk
            disk = EmacsString::null;

            if( new_value[new_value.length()-1] != PATH_CH )
            {
                new_value.append( PATH_STR );
            }

            // add the rest of the file spec to the buffer
            new_value.append( sp( disk_end, INT_MAX ) );
            // setup the pointer to the file spec to convert
            sp = new_value;
            // go do the analysis again

            device_loop_max_iterations--;
            if( device_loop_max_iterations > 0 )
                goto device_loop;
        }
#endif
    }
    else
    {
        // default to no device found
        disk_end = 0;
        // treat server paths as devices
        if( sp.commonPrefix( "\\\\" ) == 2 )
        {
            int server_end = sp.index( PATH_CH, 2 );
            if( server_end > 0 )
            {
                disk = sp( 0, server_end );
                disk_end = server_end;
            }
        }
    }

    int path_end = sp.last( PATH_CH );
    if( path_end < 0 )
        path_end = disk_end;
    else
        path_end++;

    if( disk_end <= path_end )
        path = sp( disk_end, path_end );    // extract the path
    else
        path = EmacsString::null;        // syntax error null the path

    int filename_end = sp.last( '.', path_end );
    if( filename_end < 0 )
        filename_end = sp.length();

    filename = sp( path_end, filename_end );
    filetype = sp( filename_end, INT_MAX );

    return 1;
}

static EmacsString get_current_directory()
{
    // find out the size of the cwd
    DWORD w_len = GetCurrentDirectory( 0, NULL );
    wchar_t *w_buf = malloc_utf16( w_len+1 );
    w_len = GetCurrentDirectory( w_len, w_buf );

    EmacsString cwd( w_buf, w_len );
    emacs_free( w_buf );

    if( cwd[-1] != PATH_CH )
    {
        cwd.append( PATH_CH );
    }

    return cwd;
}

bool FileParse::sys_parse( const EmacsString &name, const EmacsString &def )
{
    FileParse d_fab;
    EmacsString fn_buf;

    if( !analyse_filespec( name ) )
        return 0;
    if( !d_fab.analyse_filespec( def ) )
        return 0;

    if( disk.isNull() )
    {
        if( !d_fab.disk.isNull() )
            disk = d_fab.disk;
        else
        {
            // default to dev: or //server/service or cur dir
            EmacsString cur_dir( get_current_directory() );

            FileParse cur_dir_fab;

            if( !cur_dir_fab.analyse_filespec( cur_dir ) )
                disk = EmacsString::null;
            else
                disk = cur_dir_fab.disk;
        }
    }

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 0;
    filename_maxlen = 8;
    filetype_maxlen = 4;

    // if its NT then find out what the file system supports
    int file_parsing_override = 0;    // let emacs decide

    if( disk[1] != ':' )
    {
        // force long names for UNC paths
        filename_maxlen = 255;
        filetype_maxlen = 255;
    }
    else
    {
        // get the override parameter - defaults to let emacs decide
        char disk_name[2];
        disk_name[0] = (char)unicode_to_upper( disk[0] );
        disk_name[1] = '\0';
        file_parsing_override = get_file_parsing_override( disk_name, 0 );

        switch( file_parsing_override )
        {
        case 2:    // force 8.3 short names
            filename_maxlen = 8;
            filetype_maxlen = 4;
            break;

        default:
        case 1:    // force long names
            filename_maxlen = 255;
            filetype_maxlen = 255;
            break;

        case 0:    // let Emacs decide
        {
            // figure out the version of the system
            OSVERSIONINFO os_info;
            os_info.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

            // this call may fail on 3.1 with old win32s
            int is_new_windows = GetVersionEx( &os_info );
            if( is_new_windows )
            {
                switch( os_info.dwPlatformId )
            {
                default:
                    // unknown version of windows assume its *new*
                case VER_PLATFORM_WIN32_WINDOWS:
                    // yep new windows - this is Win95
                    break;
                case VER_PLATFORM_WIN32s:
                    // this is old windows - win 3.1 with win32s
                    is_new_windows = 0;
                    break;
                case VER_PLATFORM_WIN32_NT:
                    // this is NT - figure out the version
                    // looking for better trwn NT 3.1
                    if( os_info.dwMajorVersion == 3 && os_info.dwMinorVersion == 1 )
                        is_new_windows = 0;
                    break;
            }
            }

            if( is_new_windows )
            {
                //
                //    On new windows we will trust GetVolumeInformation to tell the truth
                //    about file component lengths
                //
                DWORD serial_number;
                DWORD max_comp_len;
                wchar_t fs_name[32];
                DWORD fs_flags;

                EmacsString root( FormatString("%s" PATH_STR) << disk );

                if( GetVolumeInformation
                    (
                    root.utf16_data(),
                    NULL, 0,
                    &serial_number,
                    &max_comp_len,
                    &fs_flags,
                    fs_name, sizeof( fs_name )/sizeof(wchar_t)
                    ) )
                {
                    filename_maxlen = (int)max_comp_len;
                    filetype_maxlen = (int)max_comp_len;
                }
                else
                {
                    filename_maxlen = 255;
                    filetype_maxlen = 255;
                }
            }
            else
            {
                //
                //    On old windows we will trust GetVolumeInformation just so far
                //    then we figure out the limits
                //
                DWORD serial_number;
                DWORD max_comp_len;
                wchar_t fs_name[32];
                DWORD fs_flags;

                EmacsString root( FormatString("%s" PATH_STR) << disk );

                if( GetVolumeInformation
                    (
                    root.utf16_data(),
                    NULL, 0,
                    &serial_number,
                    &max_comp_len,
                    &fs_flags,
                    fs_name, sizeof( fs_name )/sizeof( wchar_t )
                    ) )
                {
                    // seems that NT is always insensitive
                    // file_case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0;

                    if( wcscmp( fs_name, L"FAT" ) == 0 )
                    {
                        filename_maxlen = 8;
                        filetype_maxlen = 4;
                    }
                    else
                        // assume all others do not need a special check
                    {
                        filename_maxlen = (int)max_comp_len;
                        filetype_maxlen = (int)max_comp_len;
                    }
                }
                else
                {
                    filename_maxlen = 255;
                    filetype_maxlen = 255;
                }
            }
        }
            break;
        }
    }

    if( path.isNull() )
    {
        if( !d_fab.path.isNull() )
            path = d_fab.path;
        else
        {
            // default to path of cur dir
            EmacsString cur_dir( get_current_directory() );

            FileParse cur_dir_fab;

            // only use the current dir if its on the same disk
            if( cur_dir_fab.analyse_filespec( cur_dir ) && disk == cur_dir_fab.disk )
                path = cur_dir_fab.path;
            else
                path = EmacsString("/");
        }
    }

    //
    //    See if the filename is infact a directory.
    //    If it is a directory move the filename on to
    //    the end of the path and null filename.
    //
    if( !filename.isNull() )
    {
        EmacsString fullspec( FormatString("%.2s%s%.*s%.*s") <<
            disk << path <<
            filename_maxlen << filename <<
            filetype_maxlen << filetype );

        // get attributes
        if( file_is_directory( fullspec ) )
        {
            // need to merge the filename on to the path
            path = FormatString("%s%.*s%.*s") <<
                path <<
                filename_maxlen << filename <<
                filetype_maxlen << filetype;

            filename = EmacsString::null;
            filetype = EmacsString::null;
        }
    }

    if( filename.isNull() )
        filename = d_fab.filename;

    if( filetype.isNull() )
        filetype = d_fab.filetype;

    fn_buf = FormatString("%s%s%.*s%.*s") <<
        disk <<
        path <<
        filename_maxlen << filename <<
        filetype_maxlen << filetype;

    char full_path_str[MAXPATHLEN+1 ];
    if( _fullpath( full_path_str, fn_buf, MAXPATHLEN+1 ) != NULL )
    {
        EmacsString full_path( full_path_str );
        if( full_path[-1] != PATH_CH
        && file_is_directory( full_path ) )
            full_path.append( PATH_STR );
        else
            if( fn_buf[-1] == '.' && full_path[-1] != '.' )
                full_path.append( '.' );

        result_spec = convertShortPathToLongPath( full_path );
    }
    else
        return 0;

    return 1;
}

//
// walk the elements of the path converting potential short names into long names
//
static EmacsString convertShortPathToLongPath( const EmacsString &short_path )
{
    int start = 0;
    int end = short_path.first( PATH_CH );

    EmacsString long_path( short_path( start, end ) );

    while( end < short_path.length() )
    {
        start = end+1;

        end = short_path.index( PATH_CH, start );
        if( end < 0 )
            end = short_path.length();

        EmacsString part = short_path( start, end );

        if( part.first( '*' ) >= 0
        ||  part.first( '?' ) >= 0 )
        {
            // Do not expand wild cards - this is a name expander only
            long_path.append( PATH_CH );
            long_path.append( part);
        }
        else
        {
            EmacsString name_to_lookup( long_path );
            name_to_lookup.append( PATH_CH );
            name_to_lookup.append( part );

            WIN32_FIND_DATA find_data;
            HANDLE hFind = FindFirstFile( name_to_lookup.utf16_data(), &find_data );
            if( hFind == INVALID_HANDLE_VALUE )
            {
                // file was not found - use the part as is
                long_path.append( PATH_CH );
                long_path.append( part );
            }
            else
            {
                FindClose( hFind );
                EmacsString file_name( find_data.cFileName, wcslen(find_data.cFileName) );

                // use the long name as it appears on disk
                long_path.append( PATH_CH );
                long_path.append( file_name );
            }
        }
    }

    return long_path;
}


class FileFindWindowsNT : public FileFindInternal
{
public:
    FileFindWindowsNT( const EmacsString &files, bool return_all_directories );
    virtual ~FileFindWindowsNT();

    EmacsString next();
private:
    enum { first_time, next_time, all_done } state;
    WIN32_FIND_DATA find;
    HANDLE handle;
    int case_sensitive;
    EmacsString root_path;
    EmacsString match_pattern;
    EmacsString full_filename;
};

FileFind::FileFind( const EmacsFile &files, bool return_all_directories )
: impl( NULL )
{
    impl = new FileFindWindowsNT( files.result_spec, return_all_directories );
}

FileFind::~FileFind()
{
    delete impl;
}

EmacsString FileFind::next()
{
    if( impl )
    {
        return impl->next();
    }

    return EmacsString::null;
}


FileFindWindowsNT::FileFindWindowsNT( const EmacsString &_files, bool _return_all_directories )
: FileFindInternal( _return_all_directories )
, state( all_done )    // assume all done
, handle( INVALID_HANDLE_VALUE )
, case_sensitive(0)
, root_path()
, full_filename()
{
    wchar_t file_name_buffer[ MAX_PATH ];
    DWORD len = GetFullPathName( _files.utf16_data(), MAX_PATH, file_name_buffer, NULL );
    if( len == 0 )
        return;

    // now its possible to get the first file
    state = first_time;

    root_path = EmacsString( file_name_buffer, len );
    int last_path_ch = root_path.last( PATH_CH );
    if( last_path_ch >= 0 )
    {
        match_pattern = root_path( last_path_ch+1, INT_MAX );
        root_path.remove( last_path_ch+1 );
    }
    if( !case_sensitive )
        match_pattern.toLower();
}

FileFindWindowsNT::~FileFindWindowsNT()
{
    if( handle )
        FindClose( handle );
}

EmacsString FileFindWindowsNT::next()
{
    for(;;)
    {
        switch( state )
        {
        default:
        case all_done:
            return EmacsString::null;

        case first_time:
        {
            EmacsString files( root_path );
            files.append( "*" );
            handle = FindFirstFile( files.utf16_data(), &find );
            if( handle == INVALID_HANDLE_VALUE )
            {
                state = all_done;
                return EmacsString::null;
            }

            state = next_time;
        }
            break;

        case next_time:
            if( !FindNextFile( handle, &find ) )
            {
                state = all_done;
                return EmacsString::null;
            }
            break;
        }

        if( wcscmp( find.cFileName, L"." ) == 0
        || wcscmp( find.cFileName, L".." ) == 0 )
            continue;

        // return all directories if requested to do so
        if( return_all_directories
        &&  find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
            break;
        // if the name does match the pattern
        EmacsString file_name( find.cFileName, wcslen( find.cFileName ) );
        if( !case_sensitive )
            file_name.toLower();
        if( match_wild( file_name, match_pattern ) )
            break;
    }

    // return success and the full path
    full_filename = root_path;
    EmacsString file_name( find.cFileName, wcslen( find.cFileName ) );
    full_filename.append( file_name );
    if( find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY
    && full_filename[-1] != PATH_CH )
        full_filename.append( PATH_STR );

    return full_filename;
}
