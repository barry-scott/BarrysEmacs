/*
 *	unix_rtl.cpp for Emacs V7.0
 *	Copyright (c) 1993-2003 Barry A. Scott
 */

#include <emacs.h>
#include <sys/utsname.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <syslog.h>

#include <mem_man.h>

#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <pwd.h>
// #undef _POSIX_SOURCE
#include <unistd.h>
#include <assert.h>
#include <time.h>

#ifdef XWINDOWS
#include <emacs_motif.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#ifdef __hpux
#include <symlink.h>

// define openlog and syslog as it is missing on hpux 9
extern "C" void syslog(int priority, const char *message, ...);
extern "C" void openlog(const char *ident, int logopt, int facility);
extern "C" void closelog(void);
#endif

#if defined( __hpux ) && !defined( _XPG4_EXTENDED )
/* HP-UX 9 does not prototype select with the fd_set as the args */
#define Select( a, b, c, d, e ) select( a, (int *)b, (int *)c, (int *)d, e )
#else
#define Select( a, b, c, d, e ) select( a, b, c, d, e )
#endif
#ifdef _AIX
#include <sys/select.h>
#endif

//#define exception math_exception
//#include <exception>
#include <math.h>


static struct timeval emacs_start_time;

static fd_set readfds, writefds, excepfds;
static fd_set readfds_resp, writefds_resp;
struct fd_info
    {
    XtPointer param;			/* Parameter for function */
    XtInputCallbackProc cb;		/* Fucntion to call for event */
    };

static struct fd_info read_info[MAXFDS], write_info[MAXFDS];
static int fd_max = 0;
static EmacsString unix_path;
static EmacsString image_path;
EmacsString name_arg;

static void process_args( int argc, char **argv );
static bool isFileDescriptorOpen( int fd );

static EmacsString restore_arg;
static EmacsString package_arg;
static int nodisplay_arg = 0;

int main( int argc, char **argv )
	{
	char *debug_string = getenv( "EMACS_DEBUG" );
	if( debug_string != NULL )
		dbg_flags = parse_dbg_flags( debug_string );

	{
	bool open_fds[256];
	if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP )
		for( int fd=0; fd<256; fd ++ )
			open_fds[fd] = isFileDescriptorOpen( fd );

#ifdef __hpux
	openlog( "bemacs", LOG_PID|LOG_NDELAY, LOG_LOCAL2 );
#else
	openlog( "bemacs", LOG_PID|LOG_NDELAY, LOG_USER );
#endif

	if( dbg_flags&DBG_PROCESS && dbg_flags&DBG_TMP )
		for( int fd=0; fd<256; fd ++ )
			if( open_fds[fd] )
				_dbg_msg( FormatString("File descriptor %d is open at startup") << fd );
	}

	if( !isFileDescriptorOpen( STDIN_FILENO ) )
		{
		FILE *file = freopen( "/dev/null", "r", stdin );
		assert( fileno(file) == STDIN_FILENO );
		}

	if( !isFileDescriptorOpen( STDOUT_FILENO ) )
		{
		FILE *file = freopen( "/dev/null", "w", stdout );
		assert( fileno(file) == STDOUT_FILENO );
		}

	if( !isFileDescriptorOpen( STDERR_FILENO ) )
		{
		FILE *file = freopen( "/dev/null", "w", stderr );
		assert( fileno(file) == STDERR_FILENO );
		}

	init_memory();

	unix_path = getenv( "PATH" );
	for( int i=0; i<unix_path.length(); i++ )
		if( unix_path[i] == ':' )
			unix_path[i] = PATH_SEP;

	{
	EmacsFile image;
	image.fio_open_using_path( unix_path, argv[0], 0, "" );
	if( !image.fio_is_open() )
		{
		_dbg_msg( "Emacs is unable to find itself!\n" );
		return 100;
		}
	image_path = image.fio_getname();
	}

	// find the stat of the path and don't hide links
	struct stat stat_buf;
	if( lstat( (const char *)image_path, &stat_buf ) == 0
	// its a symbolic link
	&& (stat_buf.st_mode&S_IFMT) == 0120000 )
		{
		unsigned char link_path[MAXPATHLEN+1];
		// get the value of the link
		int size = readlink( (const char *)image_path, (char *)link_path, MAXPATHLEN );
		if( size != -1 )
			{
			link_path[size] = '\0';

			// printf( "The sym link is %s\n", link_path );
			// if its an absolute path just replace image_path
			if( link_path[0] == '/' )
				image_path = link_path;
			else
				{
				// add the link to the end of the image_path less the last file name
				int pos = image_path.last( PATH_CH );
				if( pos > 0 )
					{
					image_path.remove( pos+1 );
					image_path.append( link_path );
					}
				}
			}

		// printf( "New image path is %s\n", image_path.data() );
		}

	int pos = image_path.last( PATH_CH );
	if( pos < 0 )
		return 101;
	image_path.remove( pos );

	// record the start time
	gettimeofday( &emacs_start_time, NULL );

	struct utsname uname_data;
	int status = uname( &uname_data );

	default_environment_file = "emacs$user:emacs7_" OS_VERSION "_";
	if( status != -1 )
		{
		default_environment_file.append( uname_data.release );
		default_environment_file.append( "_" );
		}
	default_environment_file.append( CPU_TYPE ".emacs_environment" );
	restore_arg = default_environment_file;

	process_args( argc, argv );

#ifdef XWINDOWS
	EmacsString display;
	if( !nodisplay_arg )
		display = get_config_env( "DISPLAY" );

	int exit_code;
	if( !display.isNull() )
		exit_code = emacsMain( restore_arg, u_str("gui"), display );
	else
#endif
		exit_code = emacsMain( restore_arg, u_str("char"), u_str("") );

	closelog();

	return exit_code;
	}

int motif_argc = 0;
char *motif_argv[32];

static void process_args( int argc, char **argv )
	{
	command_line_arguments.setArguments( argc, argv );
	command_line_arguments.setArgument( 0, "emacs", false );

	motif_argv[motif_argc++] = "emacs";

	int arg = 1;

	while( arg<command_line_arguments.argumentCount() )
		{
		EmacsArgument argument( command_line_arguments.argument( arg ) );

		if( argument.isQualifier() )
			{
			EmacsString str( argument.value() );
			EmacsString key_string;
			EmacsString val_string;

			int equal_pos = str.first( '=' );
			if( equal_pos > 0 )
				{
				key_string = str( 1, equal_pos );
				val_string = str( equal_pos+1, str.length() );
				}
			else
				{
				key_string = str( 1, str.length() );
				}

			bool delete_this_arg = true;

			if( key_string.commonPrefix( "restore" ) > 2 )
				restore_arg = val_string;
			else if( key_string.commonPrefix( "norestore" ) > 4 )
				restore_arg = EmacsString::null;
			else if( key_string.commonPrefix( "package" ) > 2 )
				{
				package_arg = val_string;
				command_line_arguments.setArgument( 0, package_arg, false );
				}
			else if( key_string.commonPrefix( "name" ) > 3 )
				{
				name_arg = val_string;
				}
			else if( key_string.commonPrefix("nodisplay") > 3 )
				{
				nodisplay_arg = 1;
				}
			else if( str == "display" )
				{
				motif_argv[motif_argc++] = "-display";
				command_line_arguments.deleteArgument( arg );
				if( arg < command_line_arguments.argumentCount() )
					{
					motif_argv[motif_argc++] = strdup( command_line_arguments.argument(arg).value().sdata() );
					}
				else
					delete_this_arg = false;
				}
			else if( str == "geometry" )
				{
				motif_argv[motif_argc++] = "-geometry";
				command_line_arguments.deleteArgument( arg );
				if( arg < command_line_arguments.argumentCount() )
					{
					motif_argv[motif_argc++] = strdup( command_line_arguments.argument(arg).value().sdata() );
					}
				else
					delete_this_arg = false;
				}
			else
				delete_this_arg = false;

			if( delete_this_arg )
				command_line_arguments.deleteArgument( arg );
			else
				arg++;
			}
		else
			arg++;
		}
	}

#if !defined(XWINDOWS)
int init_gui_terminal( const EmacsString & )
	{
	return 0;
	}

void UI_update_window_title( void )
	{ }

const int TIMER_TICK_VALUE( 50 );
static unsigned int due_tick_count;
static void( *timeout_handler )(void );
struct timeval timeout_time;

void time_schedule_timeout( void( *time_handle_timeout )(void ), int delta  )
	{
	struct timezone tzp;
	gettimeofday( &timeout_time, &tzp  );

	timeout_time.tv_sec += delta/1000;
	timeout_time.tv_usec +=( delta%1000 )*1000;
	if( timeout_time.tv_usec > 1000000  )
			{
			timeout_time.tv_sec += 1;
			timeout_time.tv_usec -= 1000000;
			}
	timeout_handler = time_handle_timeout;
	}

void time_cancel_timeout(void)
	{
	timeout_time.tv_sec = 0;
	timeout_time.tv_usec = 0;
	timeout_handler = NULL;
	}
#endif

void wait_abit(void)
	{
	static struct timeval tmo = {0, 100000};
	fd_set rfds;
	int fd;
#ifdef SUBPROCESSES
	extern fd_set process_fds;

	memcpy( &rfds, &process_fds, sizeof( fd_set ) );
#else
	FD_ZERO( &rfds );
#endif
	{
#ifdef XWINDOWS
	if (is_motif)
	    fd = theMotifGUI->application.dpy_fd;
	else
#endif
	    fd = 1<<0;
	}
	FD_SET( fd, &rfds );

	if (Select (MAXFDS, &rfds, NULL, NULL, &tmo))
	    {
	    wait_for_activity ();
	    }
#if defined( SUBPROCESSES )
	    if( child_changed )
		change_msgs ();
#endif
	return;
	}

void debug_invoke(void)
	{
	return;
	}

void debug_SER(void)
	{
	return;
	}

void debug_exception(void)
	{
	return;
	}

void _dbg_msg( const EmacsString &msg )
	{
	if( dbg_flags&DBG_SYSLOG )
		{
		syslog( LOG_DEBUG, "%s", msg.sdata() );
		}
	else
		{
		fprintf( stderr,"%s", msg.sdata() );
		if( msg[-1] != '\n' )
			fprintf( stderr, "\n" );
		fflush( stderr );
		}
	}

int win_emacs_quit = 0;

int wait_for_activity(void)
	{
	unsigned char buf[128];

	int size = theActiveView->k_input_event( buf, sizeof( buf ) );
	if( size < 0 )
		return -1;
	if( size >= 1 )
		{
		for( int i=0; i<size; i++ )
			{
			theActiveView->k_input_char( buf[i], false );
			}
		return 1;
		}

	if( win_emacs_quit )
		{
		win_emacs_quit = 0;
		return -1;
		}

	return 0;
	}

int interlock_dec( volatile int *cell )
	{
	(*cell)--;
	if( *cell == 0 )
		return 0;
	if( *cell < 0 )
		return -1;
	else
		return 1;
	}

int interlock_inc( volatile int *cell )
	{
	(*cell)++;
	if( *cell == 0 )
		return 0;
	if( *cell < 0 )
		return -1;
	else
		return 1;
	}

void conditional_wake(void)
	{
	return;
	}

EmacsString get_user_full_name()
	{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid( uid );

	if( pw == NULL )
		return EmacsString::null;
	else
		return EmacsString( pw->pw_gecos );
	}

EmacsString users_login_name()
	{
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid( uid );

	if( pw == NULL )
		return EmacsString::null;

	return EmacsString( pw->pw_name );
	}

EmacsString get_system_name()
	{
	char system_name[256];
	if( gethostname( system_name, sizeof( system_name ) ) == 0 )
		return EmacsString( system_name );
	else
		return EmacsString::null;
	}

void fatal_error( int code )
	{
	printf("\nFatal Error %d\n", code );
	exit(1);
	}

int put_config_env( const EmacsString &name, const EmacsString &value )
	{
	EmacsString buf;
	buf.append( name );
	buf.append( "=" );
	buf.append( value );

	return putenv( buf.sdataHack() );
	}

EmacsString get_config_env( const EmacsString &name )
	{
	char *value = getenv( name );

	if( value != NULL )
		return value;
	
	static EmacsString env_emacs_path(  "emacs_user: emacs_library:" );
	if( name == "emacs_path" )
		return env_emacs_path;

	static EmacsString env_emacs_user(  "HOME:/bemacs" );
	if( name == "emacs_user" )
		return env_emacs_user;

	static EmacsString env_emacs_library(  image_path );
	if( name == "emacs_library" )
		return env_emacs_library;

	static EmacsString env_sys_login(  "HOME:/" );
	if( name == "sys_login" )
		return env_sys_login;

	return EmacsString::null;
	}

XtInputId add_select_fd (int fd, long int mask, XtInputCallbackProc cb, XtPointer p)
    {
    XtInputId resp = 0;

    if (fd < MAXFDS)
	{
	if (fd > fd_max)
	    fd_max = fd;
	if (mask & XtInputReadMask)
	    {
	    read_info[fd].param = p;
	    read_info[fd].cb = cb;
	    FD_SET( fd, &readfds );
	    resp = fd << 8;
	    }
	if (mask & XtInputWriteMask)
	    {
	    write_info[fd].param = p;
	    write_info[fd].cb = cb;
	    FD_SET( fd, &writefds );
	    resp |= fd << 16;
	    }
	}
    return resp;
    }

void remove_select_fd (XtInputId id)
    {
    int fd = 0, i;

    if (id & 0xff00)
	{
	fd = (int)((id >> 8) & 0xff);
	read_info[fd].param = NULL;
	read_info[fd].cb = NULL;
        FD_CLR( fd, &readfds );
	}

    if (id & 0xff0000)
	{
	fd = (int)((id >> 16) & 0xff);
	write_info[fd].param = NULL;
	write_info[fd].cb = NULL;
        FD_CLR( fd, &writefds );
	}

    if (fd == fd_max)
	{
	for (i = fd_max; i >= 0; i--)
	    if (read_info[fd].cb != NULL || write_info[fd].cb != NULL)
		{
		fd_max = i;
		break;
		}
	if (i < 0)
	    fd_max = 0;
	}
    }
int read_inputs (int fd, unsigned char *buf, unsigned int count)
	{
	int i, status;

	do
	    {
	    memcpy( &readfds_resp, &readfds, sizeof( fd_set ) );
	    memcpy( &writefds_resp, &writefds, sizeof( fd_set ) );
	    FD_SET( fd, &readfds_resp );
	    status = Select (MAXFDS, &readfds_resp, &writefds_resp, &excepfds, 0);
	    }
	while (status < 0 && errno == EINTR);

//	_dbg_msg( FormatString("read_inputs( %d, buf, %d ) => status = %d") << fd << count << status ); 

	for (i = 1; i <= fd_max; i++)
	    {
//	    _dbg_msg( FormatString("fd %d read %d write %d")
//			<< i << FD_ISSET( i, &readfds_resp ) << FD_ISSET( i, &writefds_resp ) );
	    if( read_info[i].cb != NULL
	    && FD_ISSET( i, &readfds_resp ) )
		read_info[i].cb(read_info[i].param, &i, NULL);
	    if( write_info[i].cb != NULL
	    && FD_ISSET( i, &writefds_resp ) )
		write_info[i].cb(write_info[i].param, &i, NULL);
	    }

	if( FD_ISSET( fd, &readfds_resp ) )
	    {
//	    _dbg_msg( FormatString("read_inputs() calling read( %d, ... )") << fd );
	    return read (fd, buf, count);
	    }

	return 0;
	}

void OutputDebugString( const char *message )
	{
	printf( "Debug: %s\n", message );
	}


int elapse_time()
	{
	struct timeval now;
	gettimeofday( &now, NULL );

	/*
	 *	calculate the time since startup in mSec.
	 *	we ignore the usec part of the start time
	 *	(assuming its 0)
	 */
	int elapse_time = (int)(now.tv_sec - emacs_start_time.tv_sec);
	elapse_time *= 1000;
	elapse_time += (int)(now.tv_usec/1000);

	return elapse_time;
	}


extern void init_memory();

void EmacsInitialisation::os_specific_init()
	{
	init_memory();
#ifdef SAVE_ENVIRONMENT
	//
	//	Create the save environment object at the earlest opertunity
	//
	EmacsSaveRestoreEnvironmentObject = EMACS_NEW EmacsSaveRestoreEnvironment;
#endif
	}

//
//	Emacs server code
//

static EmacsString server_fifo;
static EmacsString client_fifo;

static XtInputId emacs_server_input_id;
static int emacs_server_read_fd = -1;
static int emacs_server_write_fd = -1;

class EmacsServerWorkItem : public EmacsWorkItem
	{
public:
	EmacsServerWorkItem()
		: EmacsWorkItem(),
		bytes_read(0)
		{}
	virtual ~EmacsServerWorkItem()
		{}

	// routine to read a command from the FIFO
	void readCommand( int fd );
private:
	virtual void workAction(void);

	// used to hold the size of the 
	int bytes_read;
	// max read size
	enum { SERVER_BUFFER_SIZE = 16384 };
	// the command string that was read
	unsigned char command_string[SERVER_BUFFER_SIZE];
	};

EmacsServerWorkItem emacs_server_work_item;

EmacsCommandLineServerWorkItem emacs_command_line_work_item;


extern TerminalControl_GUI *theMotifGUI;

void EmacsServerWorkItem::workAction()
	{
	int emacs_client_write_fd;

	// quit if last read failed
	if( bytes_read <= 0 )
		return;

	// get focus if we need it
	theMotifGUI->getKeyboardFocus();

	void *sep = memchr( command_string, 0, bytes_read );
	if( sep != NULL )
		{
		int index = (char *)sep - (char *)((void *)&command_string[0]);

		emacs_command_line_work_item.newCommandLine
			(
			EmacsString( EmacsString::copy, command_string, index ),
			EmacsString( EmacsString::copy, command_string + index + 1, bytes_read - index - 1 )
			);
		}
	else
		emacs_command_line_work_item.newCommandLine
			(
			EmacsString( EmacsString::copy, command_string, bytes_read ),
			EmacsString( EmacsString::null )
			);
		
	// this will fail until the client is synced up
	emacs_client_write_fd = open( client_fifo, O_WRONLY|O_NONBLOCK );
	if( emacs_client_write_fd < 0 )
		return;
	// send a 1 byte <space> string
	write( emacs_client_write_fd, " ", 1 );
	close( emacs_client_write_fd );
	}

bool send_exit_message( const EmacsString &command )
	{
	// this will fail until the client is synced up
	int emacs_client_write_fd = open( client_fifo, O_WRONLY|O_NONBLOCK );
	if( emacs_client_write_fd < 0 )
		return false;
	// send a 1 byte header on the response string
	EmacsString response("R");	// R for response
	response.append( command );
	write( emacs_client_write_fd, response.data(), response.length() );
	close( emacs_client_write_fd );
	return true;
	}

void emacs_server_callback(XtPointer PNOTUSED(str), int *fd, XtInputId* PNOTUSED(id) )
	{
	emacs_server_work_item.readCommand( *fd );
	}

void EmacsServerWorkItem::readCommand( int fd )
	{
	bytes_read = read( fd, command_string, sizeof( command_string ) );
	if( bytes_read > 0 )
		{
		// and schedule the work item
		addItem();
		}
	}


extern XtInputId add_to_select( int fd, long int mask, XtInputCallbackProc input_request, EmacsProcess *npb );
extern void remove_input( XtInputId id );

void start_emacs_server()
	{
	char *fifo_name = getenv("BEMACS_FIFO");

	if( emacs_server_read_fd >= 0 )
		return;

	if( fifo_name == NULL )
		fifo_name = ".bemacs/.emacs_command";

	if( fifo_name[0] != '/' )
		{
		char *name = NULL;
		char *home = getenv( "HOME" );
		if( home != NULL )
			{
			for( char *p = home; *p; p++ )
				if( *p == '/' )
					name = p;
			}
		else
			{
			struct passwd *pwd = getpwuid( geteuid() );
			if( pwd == NULL )
				name = "default";
			else
				name = pwd->pw_name;
			}

		server_fifo = "/tmp/";
		server_fifo.append( name );
		server_fifo.append( "/" );
		}
	server_fifo.append( fifo_name );

	client_fifo = server_fifo;
	client_fifo.append( "_response" );

	if( !name_arg.isNull() )
		{
		server_fifo.append( "_" );
		server_fifo.append( name_arg );
		client_fifo.append( "_" );
		client_fifo.append( name_arg );
		}

	emacs_server_read_fd = open( server_fifo, O_RDONLY|O_NONBLOCK );
	if( emacs_server_read_fd < 0 )
		return;

	emacs_server_write_fd = open( server_fifo, O_WRONLY|O_NONBLOCK );
	if( emacs_server_write_fd < 0 )
		return;


	emacs_server_input_id = add_to_select( emacs_server_read_fd, XtInputReadMask, emacs_server_callback, NULL );
	}

void stop_emacs_server()
	{
	if( emacs_server_read_fd < 0 )
		return;

	remove_input( emacs_server_input_id );
	close( emacs_server_read_fd );
	close( emacs_server_write_fd );
	}

bool emacs_internal_init_done_event(void)
	{
	start_emacs_server();

	return true;
	}

static bool isFileDescriptorOpen( int fd )
	{
	// get the close on exec flag
	int status = fcntl( fd, F_GETFD, 0 );
	// if that fails the FD is not open
	return status != -1;
	}

EmacsDateTime EmacsDateTime::now(void)
	{
	EmacsDateTime now;


	struct timeval t;
	gettimeofday(  &t, NULL );

	now.time_value = double( t.tv_usec ) / 1000000.0;
	now.time_value += double( t.tv_sec );

	return now;
	}

EmacsString EmacsDateTime::asString(void) const
	{
	double int_part, frac_part;

	frac_part = modf( time_value, &int_part );
	frac_part *= 1000.0;

	time_t clock = int( int_part );
	int milli_sec = int( frac_part );

	struct tm *tm = localtime( &clock );

	return FormatString("%4d-%2d-%2d %2d:%2d:%2d.%3.3d")
		<< tm->tm_year + 1900 << tm->tm_mon + 1 << tm->tm_mday
		<< tm->tm_hour << tm->tm_min << tm->tm_sec << milli_sec;
	}

EmacsString os_error_code( unsigned int code )
	{
	const char *error_string = strerror( code );
	if( error_string == NULL )
		return EmacsString( FormatString("Unix error code %d") << code );
	else
		return EmacsString( error_string );
	}

#ifdef _DEBUG
void _emacs_assert( const char *exp, const char *file, unsigned line )
	{
#if defined( __FreeBSD__ )
	// freebsd assert order
	__assert( "unknown", file, line, exp );

#elif defined( __GNUC__ ) && __GNUC__ >= 3
	// unix assert order
	__assert( exp, file, line );

#else
	// unix assert order
	__assert( file, line, exp );

#endif
	}
#endif


void emacs_sleep( int milli_seconds )
	{
	struct timespec request;
	request.tv_sec = milli_seconds/1000;		// seconds
	request.tv_nsec = (milli_seconds%1000)*1000000;	// convert milli to nano
	int rc = nanosleep( &request, NULL );
	if( rc == 0 )
		return;
	emacs_assert( errno == EINTR );
	}
