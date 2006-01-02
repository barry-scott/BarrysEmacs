//
//    Fileserv.c
//    Copyright 1993 Barry A. Scott
//
#include    <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#if defined(_WINDOWS)
# include "win_incl.h"
# if defined(_NT)
#  define    get_file_attr( name, attr ) \
    ((*attr = GetFileAttributes( name )) != (unsigned)-1)
#  define IS_SUBDIR FILE_ATTRIBUTE_DIRECTORY
# else
#  include    <dos.h>

#  define    get_file_attr( name, attr ) (_dos_getfileattr( name, attr ) == 0)
#  define IS_SUBDIR _A_SUBDIR

# endif
#elif defined(_MSDOS)
#  include    <dos.h>

#  define    get_file_attr( name, attr ) (_dos_getfileattr( name, attr ) == 0)
#  define IS_SUBDIR _A_SUBDIR

#elif defined(vms)
# define IS_SUBDIR 1
extern int get_file_attr( char *file, unsigned *attr );
#elif defined(__unix__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
# define IS_SUBDIR 1
int get_file_attr( const EmacsString &file, unsigned *attr )
{
    struct stat st;

    if( stat( file, &st ) == 0 )
    {
        *attr = S_ISDIR(st.st_mode);
        return 1;
    }

    return 0;
}
#if defined( __hpux )
char *getwd( char *buf )
{
    return getcwd( buf, MAXPATHLEN );
}
#else
extern char *getwd( char * );
#endif

#elif macintosh
# define IS_SUBDIR 1
# define get_file_attr( file, attr ) 0
#else
# error    "Need file services support written for this plateform"
#endif

int file_is_directory( const EmacsString file )
{
    unsigned attr = 0;

#if defined(_NT)
    attr = (unsigned)GetFileAttributes( file );
    if( attr == (unsigned)-1 )
        return 0;
#else
    if( file[file.length()-1] == PATH_CH )
        return 1;
    if( !get_file_attr( file, &attr ))
        return 0;
#endif

    return (attr & IS_SUBDIR) != 0;
}

FileParse::FileParse()
    : disk()            // disk:
    , path()            // /path/
    , filename()            // name
    , filetype()            // .type
    , result_spec()            // full file spec with all fields filled in
    , wild(0)            // true if any field is wild
    , filename_maxlen(0)        // how long filename can be
    , filetype_maxlen(0)        // how long filetype can be
    , file_case_sensitive(0)    // true if case is important
{ }

void FileParse::init()
{
    disk = "";
    path = "";
    filename = "";
    filetype = "";
    result_spec = "";
}

FileParse::~FileParse()
{ }

static unsigned char null[1] = {0};

#if defined(_MSDOS) || defined(_WINDOWS) || defined(_NT)
int FileParse::analyse_filespec( const EmacsString &filespec )
{
    EmacsString sp;
    int device_loop_max_iterations = 10;

    init();

    sp = filespec;

device_loop:
    int disk_end = sp.first(':');
    if( disk_end > 0 )
    {
        disk = sp( 0, disk_end );
        disk_end++;

        //
        // if there is a replacement string use it otherwise
        // leave the device name as it is
        //
        EmacsString new_value = get_device_name_translation( disk );
        if( new_value.isNull() )
            disk.append( ":" );
        else
        {
            // we are replacing the disk so zap any
            // left over disk
            disk = "";

            if( new_value[new_value.length()-1] != PATH_CH )
                new_value.append( PATH_STR );

            // add the rest of the file spec to the buffer
            new_value.append( sp( disk_end, INT_MAX ) );
            // setup the pointer to the file spec to convert
            sp = new_value;
            // go do the analysis again
            device_loop_max_iterations--;
            if( device_loop_max_iterations > 0 )
                goto device_loop;
        }
    }
    else
        disk_end = 0;

    int path_end = sp.last( PATH_CH );
    if( path_end < 0 )
        path_end = disk_end;
    else
        path_end++;

    path = sp( disk_end, path_end );

    int filename_end = sp.index( '.', path_end );
    if( filename_end < 0 )
        filename_end = sp.length();

    filename = sp( path_end, filename_end );
    filetype = sp( filename_end, INT_MAX );

    return 1;
}

#ifdef __WATCOMC__
void _getdcwd( unsigned drive, char *buf, unsigned len )
{
    union REGS r;
    struct SREGS s;

    strcpy( buf, "A:\\" );
    buf[0] = drive + 'A' - 1;

    s.ds = s.es = s.fs = s.gs = FP_SEG( &s );
    r.x.esi = FP_OFF( buf ) + 3;
    r.h.dl = drive;
    r.h.ah = 0x47;
    intdosx( &r, &r, &s );
}
#endif

bool FileParse::sys_parse( const EmacsString &name, const EmacsString &def )
{
    FileParse d_fab;
    EmacsString fn_buf;

    if( !analyse_filespec( name ) )
        return 0;
    if( !d_fab.analyse_filespec( def ) )
        return 0;

    if( disk.isNull() )
        if( !d_fab.disk.isNull() )
            disk = d_fab.disk;
        else
        {
            unsigned drive;

#ifdef _NT
            drive = _getdrive();
#else
            _dos_getdrive( &drive );
#endif
            disk = "A:";
            disk[0] = (unsigned char)('A' + drive-1);
        }

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 0;
    filename_maxlen = 8;
    filetype_maxlen = 4;

    // if its NT then find out what the file system supports
#if defined(_NT)
{
    char disk[2];
    int file_parsing_override;

    disk[0] = disk[0];
    disk[1] = '\0';

    // get the override parameter - defaults to let emacs decide
    file_parsing_override = AfxGetApp()->GetProfileInt( "FileParsing", disk, 0 );

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
        DWORD status = GetLastError();
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
            //    On new windows we will trucst GetVolumeInformation to tell the truth
            //    about file component lengths
            //

            DWORD serial_number;
            DWORD max_comp_len;
            char fs_name[32];
            DWORD fs_flags;

            EmacsString root( FormatString("%s" PATH_STR) << disk );

            if( GetVolumeInformation
                (
                root,
                NULL, 0,
                &serial_number,
                &max_comp_len,
                &fs_flags,
                fs_name, sizeof( fs_name )
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
            char fs_name[32];
            DWORD fs_flags;

            EmacsString root( FormatString("%s" PATH_STR) << disk );

            if( GetVolumeInformation
                (
                root,
                NULL, 0,
                &serial_number,
                &max_comp_len,
                &fs_flags,
                fs_name, sizeof( fs_name )
                ) )
            {
                // seems that NT is always insensitive
                // file_case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0;

                if( strcmp( fs_name, "FAT" ) == 0 )
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
#ifdef old_nt
{
    DWORD serial_number;
    DWORD max_comp_len;
    char fs_name[32];
    DWORD fs_flags;

    EmacsString root( FormatString("%s" PATH_STR) << disk );

    if( GetVolumeInformation
        (
        root,
        NULL, 0,
        &serial_number,
        &max_comp_len,
        &fs_flags,
        fs_name, sizeof( fs_name )
        )
    )
    {
        EmacsString assume_vfat;
        assume_vfat = get_config_env( EmacsString("assume_vfat") );
        if( assume_vfat.isNull() )
            assume_vfat = "0";
        // seems that NT is always insensitive
        // file_case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0;

        if( strcmp( fs_name, "FAT" ) == 0 && !atoi(assume_vfat) )
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
#endif
#endif

    if( path.isNull() )
    {
        if( !d_fab.path.isNull() )
            path = d_fab.path;
        else
        {
            unsigned char def_path[1+MAXPATHLEN+1];

            _getdcwd( disk[0] & 0x1f, s_str(def_path), MAXPATHLEN+1 );
            if( def_path[_str_len(def_path)-1] != PATH_CH )
                _str_cat( def_path, PATH_STR );
            path = &def_path[2];
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

            filename = "";
            filetype = "";
        }
    }

    if( filename.isNull() )
        filename = d_fab.filename;

    if( filetype.isNull() )
        filetype = d_fab.filetype;

#if defined(_WINDOWS) || defined( _MSDOS )
{
    FILE *desc;

    fn_buf = FormatString("%s%sfilename.dat") << disk << path;

    desc = fopen( fn_buf, "r" );
    if( desc != NULL )
    {
        unsigned char line_buf[80];
        // user the newline to make sure that whole filename and type matches
        fn_buf = FormatString("%s%s\n") << filename << filetype;

        if( fn_buf.length() > 1 ) // need more then just a newline!
        while( fgets( s_str(line_buf), sizeof( line_buf ), desc ) != NULL )
        {
            unsigned char *p = _str_chr( line_buf, ' ' );

            if( p == NULL )
                continue;    // bogus entry

            // if we have a match
            if( emacs_stricmp( fn_buf, (p+1) ) == 0 )
            {
                unsigned char *f_p, *t_p;
                int f_len, t_len;

                *p = '\0';
                f_p = line_buf;
                if( (t_p = _str_chr( line_buf, '.' )) != NULL )
                {
                    t_len = _str_len( t_p );
                    f_len = t_p - f_p;
                }
                else
                {
                    t_len = 0;
                    f_len = _str_len( f_p );
                }

                filename = EmacsString( EmacsString::copy, f_p, f_len );
                filetype = EmacsString( EmacsString::copy, t_p, t_len );
                break;
            }
        }
        fclose( desc );
    }
}
#endif
    fn_buf = FormatString("%s%s%.*s%.*s") <<
        disk <<
        path <<
        filename_maxlen << filename <<
        filetype_maxlen << filetype;

    char full_path_str[MAXPATHLEN+1 ];
    if( _fullpath( full_path_str, fn_buf, MAXPATHLEN+1 ) != NULL )
    {
        if( full_path_str[strlen(full_path_str)-1] != PATH_CH
        && file_is_directory( full_path_str ) )
            strcat( full_path_str, PATH_STR );
        result_spec = full_path_str;
    }
    else
        return 0;

    if( !file_case_sensitive )
        result_spec.toLower();

    return 1;
}

#elif vms
int get_file_attr( char *file, unsigned *attr )
{
    int len;

    if( file == NULL || file[0] == '\0' )
        { *attr = IS_SUBDIR; return 1; }
    len = _str_len( file );
    if( file[len-1] == ':' || file[len-1] == ']' )
        { *attr = IS_SUBDIR; return 1; }

    *attr = 0;
    return 1;
}

int sys_parse( const unsigned char *filename, const unsigned char *def, FileParse *f_fab )
{
    unsigned char fn_buf[NAM$C_MAXRSS+1];
    FileParse vms_fab;
    struct NAM vms_nam;
    char esa[NAM$C_MAXRSS+1];
    char rsa[NAM$C_MAXRSS+1];
    int len;
    int status;
    unsigned char *p;

    //
    // copy the filename into fn_buf read for processing the directories.
    // any [dir1][.dir2] is turned into [dir1.dir2]
    // and [dir1][dir2] into [dir1.dir2]
    //
    _str_cpy( fn_buf, filename );
    while( (p = _str_str( fn_buf, "][." )) != NULL )
    {
        // found it so slid the rest of the filespec up
        memmove( p, p+2, _str_len( p )+1 );
    }
    while( (p = _str_str( fn_buf, "][" )) != NULL )
    {
        // found it so slid the rest of the filespec up
        memmove( p, p+1, _str_len( p )+1 );
        *p = '.';
    }

    memset( &vms_fab, 0, sizeof( vms_fab ) );
    vms_fab.fab$b_bid = FAB$C_BID;
    vms_fab.fab$b_bln = FAB$C_BLN;
    vms_fab.fab$l_nam = &vms_nam;

    memset( &vms_nam, 0, sizeof( vms_nam ) );
    vms_nam.nam$b_bid = NAM$C_BID;
    vms_nam.nam$b_bln = NAM$C_BLN;

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 0;
    filename_maxlen = 39;
    filetype_maxlen = 40;

    vms_fab.fab$b_fns = _str_len( fn_buf );
    vms_fab.fab$l_fna = (char *)fn_buf;
    vms_fab.fab$b_dns = _str_len( def );
    vms_fab.fab$l_dna = (char *)def;

    vms_nam.nam$b_ess = NAM$C_MAXRSS;
    vms_nam.nam$l_esa = esa;

    vms_nam.nam$b_rss = NAM$C_MAXRSS;
    vms_nam.nam$l_rsa = rsa;

    vms_nam.nam$b_nop = NAM$M_SYNCHK;
    status = sys$parse( &vms_fab, 0, 0 );
    if( ! (status&1) )
        return 0;

    result_spec = malloc_ustr( vms_nam.nam$b_esl + 1 );
    if( result_spec == NULL )
        return 0;

    len = vms_nam.nam$b_esl;
    memcpy( result_spec, vms_nam.nam$l_esa, len );

    // trim off extranious "." and ";" that parse adds
    // if the filename andtype are missing
    if( result_spec[ len-1 ] == ';' )
        len--;
    if( result_spec[ len-1 ] == '.' )
        len--;

    result_spec[ len ] = 0;

    _str_lwr( result_spec );

    return 1;
}

#elif defined(__unix__)
static int analyse_filespec( unsigned char *filespec, FileParse *fab )
{
    unsigned char *sp, *p, *newstr;
    int len;
    unsigned char filespec_buf[MAXPATHLEN+1];
    int device_loop_max_iterations = 10;

    free_fab( fab );

    sp = filespec;

device_loop:
    if( (p = _str_chr( sp, ':' )) != NULL )
    {
        p++;
        len = p - sp - 1;
        newstr = realloc_ustr( fab->disk, len + 1 + 1 );
        if( newstr == NULL )
            return 0;
        fab->disk = newstr;
        _str_ncpy( newstr, sp, len );
        newstr[len] = '\0';

    {
        unsigned char *p = newstr;

        while( *p )
            if( *p == '$' )
                *p++ = '_';
            else
                p++;
    }

    {
        char buf[MAXPATHLEN+1], *new_res;

        new_res = (char *)get_config_env( newstr );
        if( new_res == NULL )
            _str_cat( newstr, ":" );
        else
        {
            // we are replacing the disk so zap any
            // left over disk
            free( fab->disk );
            fab->disk = NULL;

            _str_cpy( buf, new_res );
            if( buf[strlen(buf)-1] != PATH_CH )
                strcat( buf, PATH_STR );

            // add the rest of the file spec to the buffer
            _str_cat( buf, p );
            // copy into the main buffer
            _str_cpy( filespec_buf, buf );
            // setup the pointer to the file spec to convert
            sp = filespec_buf;
            // go do the analysis again
            device_loop_max_iterations--;
            if( device_loop_max_iterations > 0 )
                goto device_loop;
        }
    }

        sp = p;
    }

    if( (p = _str_rchr( sp, '/' )) != NULL )
    {
        p++;
        len = p - sp;
        newstr = malloc_ustr( len + 1 );
        if( newstr == NULL )
            return 0;
        fab->path = newstr;
        _str_ncpy( newstr, sp, len );
        newstr[len] = '\0';

        sp = p;
    }

    if( (p = _str_chr( sp, '.' )) != NULL )
    {
        if( (len = p - sp) != 0 )
        {
            newstr = malloc_ustr( len + 1 );
            if( newstr == NULL )
                return 0;
            fab->filename = newstr;
            _str_ncpy( newstr, sp, len );
            newstr[len] = '\0';
        }
        fab->filetype = savestr( p );
    }
    else
        if( *sp != '\0' )
            fab->filename = savestr( sp );

    return 1;
}


// input name in nm, absolute pathname output to buf.  returns -1 if the
// pathname cannot be successfully converted (only happens if the
// current directory cannot be found)
//
int fullpath( unsigned char *nm, unsigned char * buf)
{
    unsigned char *s, *d;
    unsigned char lnm[MAXPATHLEN+1];
    unsigned char def_path[1+MAXPATHLEN+1];

    def_path[0] = '\0';
    getwd( (char *)def_path );

    s = nm;
    d = lnm;
    while( (*d++ = *s) != 0 )
        if (*s++ == '$') {
            register unsigned char  *start = d;
            register int braces = *s == '{';
            register unsigned char  *value;
            while( (*d++ = *s) != 0 )
                if (braces ? *s == '}' : !isalnum (*s))
                    break;
                else
                    s++;
            *--d = 0;
            if( braces )
                value = get_env ( &start[1] );
            else
                value = get_env ( start );
            if (value != NULL) {
                for (d = start - 1; (*d++ = *value++) != 0;)
                    ;
                d--;
                if (braces && *s)
                    s++;
        }
    }

    d = buf;
    s = def_path;
    nm = lnm;

    if (nm[0] == '~')                // prefix ~
        if (nm[1] == PATH_CH)        // ~/filename
            if (s = get_env("HOME"))
                nm += 2;
            else
                s = def_path;
        else {                        // ~user/filename
            for (s = nm; *s && *s != PATH_CH; s++);
            if (*s == 0)
                s = def_path;
            else {
                register struct passwd *pw;
                *s = 0;
                pw = (struct passwd *) getpwnam (s_str(nm + 1));
                if (pw == 0) {
                    *s = PATH_CH;
                    s = def_path;
            }
                else {
                    nm = s + 1;
                    s = u_str(pw->pw_dir);
            }
        }
    }

        while( (*d++ = *s++) != 0 )
                ;

    *(d - 1) = PATH_CH;
    s = nm;
    if (*s == PATH_CH)
        d = &buf[0];
    while( (*d++ = *s++) != 0 )
        ;

    *(d - 1) = PATH_CH;
    *d = '\0';
    d = buf;
    s = buf;
    while (*s)
        if ((*d++ = *s++) == PATH_CH && d > buf + 1) {
            register unsigned char  *t = d - 2;
            switch (*t)
        {
            case PATH_CH:        // found // in the name
                --d;
                break;
            case '.':
                switch (*--t) {
                    case PATH_CH: // found /./ in the name
                        d -= 2;
                        break;
                    case '.':
                        if (*--t == PATH_CH) {// found /../
                            while (t > buf && *--t != PATH_CH);
                            d = t + 1;
                    }
                        break;
            }
                break;
        }
    }
    if (*(d - 1) == PATH_CH)
        d--;
    *d = '\0';
    return 1;
}

int sys_parse( const unsigned char *filename, const unsigned char *def, FileParse *f_fab )
{
    FileParse d_fab;
    unsigned char *newstr;
    unsigned char fn_buf[MAXPATHLEN+1];

    init_fab( &d_fab );

    if( !analyse_filespec( filename, f_fab ) )
        return 0;
    if( !analyse_filespec( def, &d_fab ) )
        return 0;

    //
    //    Assume these features of the file system
    //
    file_case_sensitive = 1;
    filename_maxlen = 128;
    filetype_maxlen = 128;

    if( path == NULL )
    {
        if( d_fab.path != NULL )
        {
            path = d_fab.path;
            d_fab.path = 0;
        }
        else
        {
            unsigned char def_path[1+MAXPATHLEN+1];

            if( getwd( (char *)def_path ) )
            {
                if( def_path[_str_len(def_path)-1] != '/' )
                    _str_cat( def_path, "/" );
                path = savestr( def_path );
                if( path == NULL )
                    goto error_exit;
            }
        }
    }

    //
    //    See if the filename is infact a directory.
    //    If it is a directory move the filename on to
    //    the end of the path and null filename.
    //
    if( filename != NULL )
    {
        char fullspec[MAXPATHLEN+1];

        sprintf( fullspec, "%s%.*s%.*s",
            path,
            filename_maxlen, filename,
            filetype_maxlen,
                (filetype ? filetype : u_str("")) );

        // get attributes
        if( file_is_directory( u_str(fullspec) ) )
        {
            // need to merge the filename on to the path
            unsigned char *newstr;

            newstr = malloc_ustr( _str_len( fullspec ) + 1 +
                    _str_len( filename ) + 1);
            if( newstr == NULL )
                goto error_exit;
            _str_cpy( newstr, fullspec );
            free( path );
            path = newstr;
            free( filename );
            filename = 0;
            if( filetype )
            {
                free( filetype );
                filetype = 0;
            }
        }
    }

    if( filename == NULL )
        if( d_fab.filename != NULL )
        {
            filename = d_fab.filename;
            d_fab.filename = 0;
        }
        else
        {
            newstr = savestr( null );
            if( newstr == NULL )
                goto error_exit;
            filename = newstr;
        }

    if( filetype == NULL )
        if( d_fab.filetype != NULL )
        {
            filetype = d_fab.filetype;
            d_fab.filetype = 0;
        }
        else
        {
            newstr = savestr( null );
            if( newstr == NULL )
                goto error_exit;
            filetype = newstr;
        }

    free_fab( &d_fab );

    sprintf( (char *)fn_buf, "%s%.*s%.*s",
        path,
        filename_maxlen, filename,
        filetype_maxlen, filetype );

    newstr = malloc_ustr( MAXPATHLEN+1 );
    if( newstr == NULL )
        return 0;
    result_spec = newstr;
    if( !fullpath( fn_buf, newstr ) )
        goto error_exit;

    return 1;
error_exit:
    free_fab( &d_fab );
    return 0;
}
#elif macintosh
int sys_parse( const unsigned char *filename, const unsigned char *def, FileParse *f_fab )
{
    result_spec = savestr( filename );
    return 1;
}
#else
#error "Need sys_parse..."
#endif

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

#if defined( _NT )
struct find_data
{
    WIN32_FIND_DATA find;
    HANDLE handle;
    int case_sensitive;
    unsigned char root_path[ MAX_PATH ];
    unsigned char full_filename[ MAX_PATH ];
};

void *find_first_file( const unsigned char *files, unsigned char **file )
{
    struct find_data *f;
    int len;
    unsigned char *p;
    char file_name_buffer[ MAX_PATH ];

    len = (int)GetFullPathName( s_str(files), MAX_PATH, s_str(file_name_buffer), NULL );
    if( len == 0 )
        return NULL;

    f = EMACS_NEW find_data;
    if( f == NULL )
        return NULL;

    _str_cpy( f->root_path, file_name_buffer );
    p = _str_chr( file_name_buffer, PATH_CH );
    if( p != NULL )
        p[1] = '\0';

    p = _str_rchr( f->root_path, PATH_CH );
    if( p != NULL )
        p[1] = '\0';

    f->case_sensitive = 0;

// seems to always be case insensitive
#if 0
{
    int serial_number;
    int max_comp_len;
    char fs_name[32];
    int fs_flags;

    if(
        GetVolumeInformation
        (
        file_name_buffer,
        NULL, 0,
        &serial_number,
        &max_comp_len,
        &fs_flags,
        fs_name, sizeof( fs_name )
        ) )
            f->case_sensitive = (fs_flags&FS_CASE_SENSITIVE) != 0;
}
#endif

    f->handle = FindFirstFile( s_str( files ), &f->find );
    if( f->handle == INVALID_HANDLE_VALUE )
    {
        delete f;
        return NULL;
    }

    _str_cpy( f->full_filename, f->root_path );
    _str_cat( f->full_filename, f->find.cFileName );
    if( f->find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
        _str_cat( f->full_filename, PATH_STR );
    if( !f->case_sensitive )
        _str_lwr( f->full_filename );
    *file = f->full_filename;

    return f;
}

int find_next_file( void *handle, unsigned char **file )
{
    struct find_data *f = (struct find_data *)handle;

    if( !FindNextFile( f->handle, &f->find ) )
        return 0;

    _str_cpy( f->full_filename, f->root_path );
    _str_cat( f->full_filename, f->find.cFileName );
    if( f->find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY )
        _str_cat( f->full_filename, PATH_STR );
    if( !f->case_sensitive )
        _str_lwr( f->full_filename );
    *file = f->full_filename;

    return 1;
}

void find_file_close( void *handle )
{
    struct find_data *f = (struct find_data *)handle;

    FindClose( f->handle );
    delete f;
}
#elif defined( _WINDOWS) || defined( _MSDOS )
struct find_data
{
    struct _find_t find;
    unsigned char root_path[ _MAX_PATH ];
    unsigned char full_filename[ _MAX_PATH ];
};

void *find_first_file( const unsigned char *files, unsigned char **file )
{
    struct find_data *f;
    int status;
    unsigned char *p;

    // defend against overflowing root_path
    if( _str_len( files ) >= _MAX_PATH )
        return NULL;

    // get the memory we need
    f = EMACS_NEW find_data;
    if( f == NULL )
        return NULL;

    // work out the disk and path
    f->root_path[0] = '\0';
    _fullpath( s_str(f->root_path), s_str(files), _MAX_PATH );
    p = _str_rchr( f->root_path, PATH_CH );
    if( p == NULL )
    {
        delete f;
        return NULL;
    }
    p[1] = '\0';

    f->find.attrib = 0;
    status = _dos_findfirst( s_str( files ), _A_SUBDIR, &f->find );
    if( status != 0 )
    {
        delete f;
        return NULL;
    }

    _str_cpy( f->full_filename, f->root_path );
    _str_cat( f->full_filename, f->find.name );
    if( f->find.attrib&_A_SUBDIR )
        _str_cat( f->full_filename, PATH_STR );
    _str_lwr( f->full_filename );
    *file = f->full_filename;

    return f;
}

int find_next_file( void *handle, unsigned char **file )
{
    struct find_data *f = (struct find_data *)handle;

    if( _dos_findnext( &f->find ) != 0 )
        return 0;

    _str_cpy( f->full_filename, f->root_path );
    _str_cat( f->full_filename, f->find.name );
    if( f->find.attrib&_A_SUBDIR )
        _str_cat( f->full_filename, PATH_STR );
    _str_lwr( f->full_filename );
    *file = f->full_filename;

    return 1;
}

void find_file_close( void *handle )
{
    struct find_data *f = (struct find_data *)handle;

    delete handle;
}


#elif defined(vms)
struct find_data
{
    unsigned int context;
    FileParse vms_fab;
    struct NAM vms_nam;
    char esa[NAM$C_MAXRSS+1];
    char rsa[NAM$C_MAXRSS+1];
    char full_filename[NAM$C_MAXRSS+1];
};

static void find_file_post_process( struct find_data *f )
{
    unsigned char *p;

    memcpy( f->full_filename, f->rsa, f->vms_nam.nam$b_rsl );
    f->full_filename[ f->vms_nam.nam$b_rsl ] = 0;

    _str_lwr( u_str( f->full_filename ) );

    // look for a directory and turn the return filespec from
    // [dir1]dir2.dir;1 into [dir1][.dir2]
    p = _str_str( f->full_filename, ".dir;1" );
    if( p != NULL )
    {
        *p = '\0';
        p = _str_rchr( f->full_filename, ']' );
        if( p != NULL )
        {
            // slid up the directory name
            memmove( p+1+2, p+1, _str_len( p+1 )+1 );
            // insert the [. and ]
            p[1] = '[';
            p[2] = '.';
            _str_cat( p, "]" );
        }
    }

}

void *find_first_file( const unsigned char *files, unsigned char **file )
{
    struct find_data *f;
    int status;
    int len;

    // defend against overflowing root_path
    if( (len = _str_len( files )) >= NAM$C_MAXRSS )
        return NULL;

    // get the memory we need
    f = EMACS_NEW find_data;
    if( f == NULL )
        return NULL;

    f->vms_fab.fab$b_bid = FAB$C_BID;
    f->vms_fab.fab$b_bln = FAB$C_BLN;
    f->vms_fab.fab$l_nam = &f->vms_nam;

    f->vms_fab.fab$b_fns = len;
    f->vms_fab.fab$l_fna = (char *)files;
    f->vms_fab.fab$b_dns = 4;
    f->vms_fab.fab$l_dna = "*.*;";

    f->vms_nam.nam$b_bid = NAM$C_BID;
    f->vms_nam.nam$b_bln = NAM$C_BLN;

    f->vms_nam.nam$b_ess = NAM$C_MAXRSS;
    f->vms_nam.nam$l_esa = f->esa;

    f->vms_nam.nam$b_rss = NAM$C_MAXRSS;
    f->vms_nam.nam$l_rsa = f->rsa;

    status = sys$parse( &f->vms_fab, 0, 0 );
    if( ! (status&1) )
    {
        delete f;
        return NULL;
    }

    status = sys$search( &f->vms_fab );
    if( !(status&1) )
    {
        delete f;
        return NULL;
    }

    find_file_post_process( f );

    *file = u_str( f->full_filename );

    return f;
}

int find_next_file( void *handle, unsigned char **file )
{
    struct find_data *f = handle;
    int status;

    status = sys$search( &f->vms_fab );
    if( !(status&1) )
        return 0;

    find_file_post_process( f );

    *file = u_str( f->full_filename );

    return 1;
}

void find_file_close( void *handle )
{
    struct find_data *f = handle;

    delete f;
}

#elif defined( __unix__ )

#if defined( _POSIX )
#define struct_direct struct dirent
#else
#define struct_direct struct direct
#endif

extern DIR * opendir( const char *path );
extern struct_direct * readdir( DIR *fd );
extern int closedir( DIR *fd );

struct find_data
{
    DIR *find;
    unsigned char root_path[ MAXPATHLEN+1 ];
    unsigned char match_pattern[ MAXPATHLEN+1 ];
    unsigned char full_filename[ MAXPATHLEN+1 ];
};

void *find_first_file( const unsigned char *files, unsigned char **file )
{
    struct find_data *f;
    unsigned char *p;

    f = EMACS_NEW find_data;
    if( f == NULL )
        return NULL;

    // split into path and pattern
    _str_cpy( f->root_path, files );
    p = _str_rchr( f->root_path, '/' );
    if( p == NULL )
        goto error_exit_free;

    p++;
    _str_cpy( f->match_pattern, p );
    *p = '\0';

    f->find = opendir( s_str(f->root_path) );
    if( f->find == NULL)
        goto error_exit_free;

    if( find_next_file( f, file ) )
        return f;

    closedir( f->find );

error_exit_free:
    delete f;
    return NULL;
}

int find_next_file( void *handle, unsigned char **file )
{
    struct find_data *f = (struct find_data *)handle;
    struct_direct *d;

    // read entries looking for a match
    while ((d = readdir(f->find)) != NULL)
    {
        if( d->d_ino == 0 )
            continue;

        // does this one match?
        if( match_wild( u_str(d->d_name), f->match_pattern ) )
        {
            _str_cpy( f->full_filename, f->root_path );
            _str_cat( f->full_filename, d->d_name );

            if( file_is_directory( f->full_filename ) )
                _str_cat( f->full_filename, PATH_STR );

            // return success and the full path
            *file = f->full_filename;
            return 1;
        }
    }
    return 0;
}

void find_file_close( void *handle )
{
    struct find_data *f = (struct find_data *)handle;
    closedir( f->find );
    delete f;
}

#elif macintosh
void *find_first_file( const unsigned char *files, unsigned char **file )
{
    return NULL;
}
int find_next_file( void *handle, unsigned char **file )
{
    return 0;
}

void find_file_close( void *handle )
{
    return;
}
#else
#error "Need file searching for this OS"
#endif
