// convert a pathname to an absolute one, if it is absolute already,
// it is returned in the buffer unchanged, otherwise leading "./"s
// will be removed, the name of the current working directory will be
// prepended, and "../"s will be resolved.
//
// In a moment of weakness, I have implemented the cshell ~ filename
// convention.    ~/foobar will have the ~ replaced by the home directory of
// the current user.  ~user/foobar will have the ~user replaced by the
// home directory of the named user.  This should really be in the kernel
// (or be replaced by a better kernel mechanism).  Doing file name
// expansion like this in a user-level program leads to some very
// distasteful non-uniformities.
//
// Another fit of dementia has led me to implement the expansion of shell
// environment variables.  $HOME/mbox is the same as ~/mbox.  If the
// environment variable a = "foo" and b = "bar" then:
//  $a    =>    foo
//  $a$b    =>    foobar
//  $a.c    =>    foo.c
//  xxx$a    =>    xxxfoo
//  ${a}!    =>    foo!
//

#include <emacs.h>

//extern "C" char *getwd( char * );
//extern "C" int fchdir( int fd );
#ifndef PATH_MAX
#define PATH_MAX 1023
#endif
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#ifdef _NT
#include <win_incl.h>
#include <direct.h>
#endif

#ifdef __unix__
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>
#endif

extern void UI_update_window_title( void );

int chdir_and_set_global_record( const EmacsString &buf );
EmacsString save_abs( const EmacsString &fn );
int expand_and_default( const EmacsString &fn, const EmacsString &dn, EmacsString &ou );
int cur_dir( void );
void init_abs( void );

EmacsString null_path("");
#ifdef __unix
EmacsString HOME_path;
EmacsString HOME_absolute_path;
#endif

#ifdef macintosh
int chdir( char *path )
{
    return 0;
}
#endif

//
// input name in nm, absolute pathname output to buf.  returns -1 if the
// pathname cannot be successfully converted (only happens if the
// current directory cannot be found)
//
int expand_and_default( const EmacsString &nm, const EmacsString &def, EmacsString & buf)
{
    FileParse fab;

    if( fab.sys_parse( nm, def ) )
        buf = fab.result_spec;
    else
        buf = EmacsString::null;

    return 0;
}


#ifdef __WATCOMC__
int _chdrive( int drive )
{
    union REGS r;

    r.h.dl = drive-1;
    r.h.ah = 0x0e;
    intdos( &r, &r );
    return 0;
}
#endif

// A chdir() that fiddles the global record
int chdir_and_set_global_record( const EmacsString &dirname )
{
    EmacsString path2;
    int ret;

    ret = expand_and_default( null_path, dirname, path2);
    if( ret < 0 )
        return ret;

#ifdef _MSDOS
    int drive = toupper( path2[0] ) - 'A' + 1;
    ret = _chdrive( drive );
    if( ret < 0 )
        return ret;
#endif
#ifdef _WINDOWS
    int len = path2.length();
    // leave a single PATH_CH
    if( len > (ROOT_CHAR_POS+1) )
        path2.remove( len );
#endif
    ret = chdir( path2 );
    if( ret < 0 )
        return ret;

    current_directory = path2;
#ifdef __unix
    if( path2 == HOME_absolute_path )
        current_directory = HOME_path;
    else if( path2.length() > HOME_absolute_path.length()    // long enough for HOME + "/"
    && path2.commonPrefix( HOME_absolute_path ) == HOME_absolute_path.length() // HOME is a prefix
    && path2[HOME_absolute_path.length()] == PATH_CH )    // ... an exact prefix
    {
        EmacsString friendly_path( HOME_path );
        friendly_path.append( path2( HOME_absolute_path.length(), path2.length() ) );

        current_directory = friendly_path;
    }
#endif
    UI_update_window_title();

    return 0;
}

//
// return a pointer to a copy of a file name that has been
// converted to absolute form.    This routine cannot return failure.
//
EmacsString save_abs( const EmacsString &fn )
{
    EmacsString buf;
    if( fn.isNull() )
        return EmacsString::null;

    if( expand_and_default( null_path, fn, buf ) < 0 )
        return fn;

    return buf;
}

void init_abs( void )
{
#ifdef __unix
    char *home = getenv("HOME");
    int cd_fd = open(".",O_RDONLY);
    if( home != NULL && cd_fd >= 0)
    {
        char home_dir[PATH_MAX+1];

        if( chdir( home ) >= 0 )
        {
            if( getcwd( home_dir, sizeof( home_dir ) ) != NULL )
            {
                HOME_path = home;
                HOME_absolute_path = home_dir;
            }
        }
    }
    fchdir( cd_fd );
    close( cd_fd );
#endif

    chdir_and_set_global_record( "." );
}
