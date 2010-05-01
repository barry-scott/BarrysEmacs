//
//    unixfile.c
//    Copyright 1993-2004 Barry A. Scott
//
#include    <emacsutl.h>
#include    <emobject.h>
#include    <emstring.h>
#include    <em_stat.h>
#include    <fileserv.h>


# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

FileNameCompareCaseSensitive file_name_compare_case_sensitive;
FileNameCompareCaseBlind file_name_compare_case_blind;
FileNameCompare *file_name_compare = &file_name_compare_case_sensitive;

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
const int IS_SUBDIR( 1 );

int get_file_attr( const EmacsString &filename, unsigned *attr )
{
    EmacsFileStat s;
    if( s.stat( filename.sdata() ) )
    {
        *attr = S_ISDIR(s.data().st_mode);
        return 1;
    }

    return 0;
}

int file_is_directory( const EmacsString &file )
{
    unsigned attr = 0;

    if( file.isNull() )
        return 0;
    if( file[-1] == PATH_CH )
        return 1;
    if( !get_file_attr( file, &attr ))
        return 0;

    return (attr & IS_SUBDIR) != 0;
}

int file_is_regular( const EmacsString &filename )
{
    if( filename.isNull() )
        return 0;

    EmacsFileStat s;
    if( s.stat( filename.sdata() ) )
        return S_ISREG( s.data().st_mode );

    return 0;
}

FileParse::FileParse()
: disk()                    // disk:
, path()                    // /path/
, filename()                // name
, filetype()                // .type
, result_spec()             // full file spec with all fields filled in
, wild( 0 )                 // true if any field is wild
, filename_maxlen( 0 )      // how long filename can be
, filetype_maxlen( 0 )      // how long filetype can be
, file_case_sensitive( 0 )  // true if case is important
{ }

void FileParse::init()
{
    disk = EmacsString::null;
    path = EmacsString::null;
    filename = EmacsString::null;
    filetype = EmacsString::null;
    result_spec = EmacsString::null;
}

FileParse::~FileParse()
{ }

int FileParse::analyse_filespec( const EmacsString &filespec )
{
    EmacsString sp;
    int device_loop_max_iterations = 10;

    init();

    sp = filespec;

    int disk_end;
    for(;;)
    {
        disk_end = sp.first(':');
        if( disk_end > 0 )
        {
            disk = sp( 0, disk_end );
            disk_end++;

            //
            // if there is a replacement string use it otherwise
            // leave the device name as it is
            //
            EmacsString new_value = get_config_env( disk );
            if( new_value.isNull() )
            {
                disk = EmacsString::null;
                disk_end = 0;
                break;
            }
            else
            {
                // we are replacing the disk so zap any
                // left over disk
                disk = EmacsString::null;

                if( new_value[-1] != PATH_CH )
                    new_value.append( PATH_STR );

                // add the rest of the file spec to the buffer
                new_value.append( sp( disk_end, INT_MAX ) );
                // setup the pointer to the file spec to convert
                sp = new_value;
                // go do the analysis again
                device_loop_max_iterations--;
                if( device_loop_max_iterations > 0 )
                    continue;
            }
        }
        else
        {
            disk_end = 0;
        }
        break;
    }

    if( file_is_directory( sp ) )
    {
        // all of sp is a path
        path = sp;
        if( path[-1] != PATH_CH )
            path.append( PATH_STR );
    }
    else
    {
        int path_end = sp.last( PATH_CH );
        if( path_end < 0 )
            path_end = disk_end;
        else
            path_end++;

        path = sp( disk_end, path_end );

        int filename_end = sp.last( '.', path_end );
        if( filename_end < 0 )
            filename_end = sp.length();

        filename = sp( path_end, filename_end );
        filetype = sp( filename_end, INT_MAX );
    }

    return 1;
}


// input name in nm, absolute pathname output to buf.  returns -1 if the
// pathname cannot be successfully converted (only happens if the
// current directory cannot be found)
//
#if DBG_TMP && 0
#define Trace( s ) do { if( dbg_flags&DBG_TMP ) _dbg_msg( s ); } while(0)
#else
#define Trace( s ) // do nothing
#endif
static int fullpath( const EmacsString &in_path, EmacsString &out_path)
{
    EmacsString expanded_in_path( in_path )

    Trace( FormatString("fullpath( %s )") << in_path );

    char c_def_path[1+MAXPATHLEN+1];

    c_def_path[0] = '\0';
    getcwd( c_def_path, sizeof(c_def_path) );

    EmacsString def_path( c_def_path );

    int in_pos = 0;
    if( expanded_in_path.length() >= 2
    && expanded_in_path[0] == '~' )                 // prefix ~
    {
        if( expanded_in_path[1] == PATH_CH )        // ~/filename
        {
            char *value = getenv( "HOME" );
            if( value != NULL )
            {
                in_pos = 2;
                out_path.append( EmacsString( value ) );
            }
            else
            {
                out_path.append( def_path );
            }
        }
        else
        {                        // ~user/filename
            int path_char_pos = expanded_in_path.first( PATH_CH );
            if( path_char_pos < 0 )
            {
                out_path.append( def_path );
            }
            else
            {
                struct passwd *pw;
                EmacsString user( expanded_in_path( 1, path_char_pos ) );
                pw = getpwnam( user );
                if( pw == NULL )
                {
                    out_path.append( def_path );
                }
                else
                {
                    out_path.append( EmacsString( pw->pw_dir ) );
                }
                in_pos = path_char_pos;
            }
        }
    }
    else
    if( expanded_in_path.length() >= 1
    && expanded_in_path[0] != '/' )
    {
        out_path.append( def_path );
    }

    if( out_path.length() >= 1
    && out_path[-1] != PATH_CH )
    {
        out_path.append( PATH_CH );
    }

    out_path.append( expanded_in_path( in_pos, expanded_in_path.length() ) );

    Trace( FormatString( "fullpath: out_path before => %s" ) << out_path );

    for( int pos=0; pos<out_path.length(); )
    {
        if( out_path[pos] == PATH_CH )
        {
            Trace( FormatString( "fullpath: pos: %d, len: %d out_path => %s|%s" )
                << pos << out_path.length()
                << out_path( 0, pos )
                << out_path( pos, out_path.length() ) );

            if( (pos+1) >= out_path.length() )
                break;

            switch( out_path[pos+1] )
            {
            case PATH_CH:        // found // in the name
                Trace( FormatString("fullpath: remove( %d, 1 )") << pos );
                out_path.remove( pos, 1 );
                break;

            case '.':
                if( (pos+2) < out_path.length() )
                    switch( out_path[pos+2] )
                    {
                    case PATH_CH:    // found /./ in the name
                        Trace( FormatString("fullpath: remove( %d, 2 )") << pos );
                        out_path.remove( pos, 2 );
                        break;

                    case '.':
                        if( (pos+3) == out_path.length()
                        || ((pos+3) < out_path.length()
                            && out_path[pos+3] == PATH_CH) )
                        {
                            int remove = 3;
                            Trace( "fullpath: found /../" );
                            // found /../
                            while( pos > 0 && out_path[pos-1] != PATH_CH )
                            {
                                pos--;
                                remove++;
                            }
                            if( pos > 0 )
                            { pos--; remove++; }
                            Trace( FormatString("fullpath: remove( %d, %d ) => %s" )
                                << pos << remove << out_path( pos, pos + remove ) );
                            out_path.remove( pos, remove );
                            Trace( FormatString("fullpath: found /../ %s")
                                << out_path );
                        }
                        else
                        {
                            pos++;
                        }
                        break;

                    default:
                        pos++;
                    }

                else
                    pos++;
                break;

            default:
                pos++;
                break;
            }
        }

        else
        {
            pos++;
        }
    }

    Trace( FormatString( "fullpath: out_path after => %s" ) << out_path );
    return 1;
}

bool FileParse::sys_parse( const EmacsString &name, const EmacsString &def )
{
    FileParse d_fab;

    if( !analyse_filespec( name ) )
        return false;

    if( !d_fab.analyse_filespec( def ) )
        return false;

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 1;
    filename_maxlen = 128;
    filetype_maxlen = 128;

    if( path.isNull() )
    {
        if( !d_fab.path.isNull() )
            path = d_fab.path;
        else
        {
            char def_path[1+MAXPATHLEN+1];

            if( getcwd( def_path, sizeof(def_path) ) )
                path = def_path;

            if( path[-1] != '/' )
                path.append( "/" );
        }
    }

    //
    //    See if the filename is infact a directory.
    //    If it is a directory move the filename on to
    //    the end of the path and null filename.
    //
    if( !filename.isNull()
    ||  !filetype.isNull() )
    {
        EmacsString fullspec( FormatString("%s%.*s%.*s") <<
            path <<
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

            if( path[-1] != '/' )
                path.append( "/" );
        }
    }

    if( filename.isNull() )
        filename = d_fab.filename;

    if( filetype.isNull() )
        filetype = d_fab.filetype;

    EmacsString fn_buf = FormatString("%s%.*s%.*s") <<
        path <<
        filename_maxlen << filename <<
        filetype_maxlen << filetype;

    if( !fullpath( fn_buf, result_spec ) )
        return false;

    return true;
}

int match_wild( const EmacsString &candidate, const EmacsString &pattern )
{
    const unsigned char *cp, *pp;        // candidate and pattern pointers
    const unsigned char *scp, *spp;    // saved cp and pp
    unsigned char cch, pch;        // candidate and pattern char

    scp = spp = u_str("");        // init to null string

    cp = candidate.data();
    pp = pattern.data();

    for(;;)
        if( *pp )    // while there is pattern chars left
        {
            pch = *pp++;

            if( pch == '*' )
            {
                if( *pp == '\0' )// pattern null after a *
                    return 1;
                scp = cp;// save pointers for back tracking
                spp = pp;
                continue;
            }
            cch = *cp++;
            if( cch == '\0' )// if candidate exhausted match fails
                break;

            if( pch == cch )
                continue;
            if( pch == '?' )
                continue;

            // mismatch detected
            if( *scp++ == '\0' )
                break;
            cp = scp;
            pp = spp;
        }
        else
        {
            if( *cp == '\0' )
                return 1;
            // mismatch detected
            if( *scp++ == '\0' )
                break;
            cp = scp;
            pp = spp;
        }
    return 0;
}

#if defined( _POSIX_VERSION )
# define struct_direct struct dirent
#else
# define struct_direct struct direct
#endif

class FileFindUnix : public FileFindInternal
{
public:
    FileFindUnix( const EmacsString &files, bool return_all_directories );
    virtual ~FileFindUnix();

    EmacsString next();
private:
    const EmacsString files;
    enum { first_time, next_time, all_done } state;
    EmacsString root_path;
    EmacsString match_pattern;
    EmacsString full_filename;

    DIR *find;
};

FileFind::FileFind( const EmacsString &files, bool return_all_directories )
{
    implementation = new FileFindUnix( files, return_all_directories );
}

FileFind::~FileFind()
{
    delete implementation;
}

EmacsString FileFind::next()
{
    if( implementation )
        return implementation->next();
    return EmacsString::null;
}


FileFindUnix::FileFindUnix( const EmacsString &_files, bool return_all_directories )
    : FileFindInternal( return_all_directories )
    , files( _files )
    , state( all_done )    // assume all done
    , root_path()
    , match_pattern()
    , full_filename()
    , find(NULL)
{
    FileParse fab;

    if( fab.sys_parse( files, EmacsString::null ) )
        root_path = fab.result_spec;
    else
        return;

    // now its possible to get the first file
    state = first_time;

    int last_path_ch = root_path.last( PATH_CH );
    if( last_path_ch >= 0 )
    {
        match_pattern = root_path( last_path_ch+1, INT_MAX );
        root_path.remove( last_path_ch+1 );
    }
}

FileFindUnix::~FileFindUnix()
{
    if( find )
        closedir( find );
}

EmacsString FileFindUnix::next()
{
    switch( state )
    {
    default:
    case all_done:
        return EmacsString::null;

    case first_time:
        if( match_pattern.isNull() )
        {
            state = all_done;
            return root_path;
        }

        find = opendir( root_path );
        if( find == NULL )
        {
            state = all_done;
            return EmacsString::null;
        }

        state = next_time;
        // fall into next_time

    case next_time:
    {
        // read entries looking for a match
        struct_direct *d;
        while( (d = readdir(find)) != NULL )
        {
            if( d->d_ino == 0 )
                continue;

            // do not return . and .. entries
            if( strcmp( d->d_name, "." ) == 0
            || strcmp( d->d_name, ".." ) == 0 )
                continue;

            full_filename = root_path;
            full_filename.append( d->d_name );

            // always return directories
            if( return_all_directories
            && file_is_directory( full_filename ) )
            {
                full_filename.append( PATH_STR );
                return full_filename;
            }

            // the name matches the pattern
            if( match_wild( d->d_name, match_pattern ) )
            {
                if( !return_all_directories        // optimise the next call away if possible
                && file_is_directory( full_filename ) )
                    full_filename.append( PATH_STR );

                // return success and the full path
                return full_filename;
            }
        }
        state = all_done;
    }
        break;
    }

    return EmacsString::null;
}
