/*
 *	unix_rtl.c for Emacs V6.0
 *	Copyright (c) 1993-1995 Barry A. Scott
 */

#include <emacs.h>
#include <sys/types.h>
#include <sys/time.h>
#include <limits.h>
#include <errno.h>
#include <unistd.h>
#include <pwd.h>
#include <fcntl.h>

#ifdef XWINDOWS
#include <X11/Intrinsic.h>
#endif

#ifdef __hpux
/* HP do not prototype select with the fd_set as the args */
#define Select( a, b, c, d, e ) select( a, (int *)b, (int *)c, (int *)d, e )
#else
#define Select( a, b, c, d, e ) select( a, b, c, d, e )
#endif

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
static unsigned char unix_path[MAXPATHLEN+1];
static unsigned char image_path[MAXPATHLEN+1];

static void process_args( int argc, char **argv );

static unsigned char restore_arg[64];
static unsigned char package_arg[64];
static int nodisplay_arg = 0;
static int the_argc;
static unsigned char *the_argv[20];

int main( int argc, char **argv )
	{
	unsigned char *p, *display;
	FILE *image;

	p = get_config_env( u_str("EMACS_DEBUG") );
	if( p != NULL )
		sscanf( s_str( p ), "%i", &dbg_flags );

	init_memory();

	_str_cpy( unix_path, u_str(getenv( "PATH" )) );
	p = unix_path;
	while( *p )
		if( *p == ':' )
			*p++ = PATH_SEP;
		else
			p++;

	image = fopenp( unix_path, u_str(argv[0]), image_path, 0, NULL );
	if( image == NULL )
		{
		_dbg_msg( u_str("Emacs is unable to find itself!\n") );
		return 100;
		}
	_str_cpy( image_path, fio_getname( image ) );
	fio_close( image );

	p = _str_rchr( image_path, PATH_CH );
	if( p == NULL )
		return 101;
	*p = '\0';

	_str_cpy( default_environment_file, "emacs$user:emacs_" OS_VERSION "_" CPU_TYPE ".emacs_environment" );

	process_args( argc, argv );

	/* record the start time */
	gettimeofday( &emacs_start_time, NULL );

	if( nodisplay_arg )
		display = NULL;
	else
		display = get_config_env (u_str("DISPLAY"));

	if( display != NULL )
		return emacs( the_argc, (unsigned char **)the_argv, restore_arg, u_str("gui"), display );
	else
		return emacs( the_argc, (unsigned char **)the_argv, restore_arg, u_str("char"), u_str("") );
	}

static void process_args( int argc, char **argv )
	{
	int arg;

	the_argc = 1;
	the_argv[0] = savestr(u_str("emacs"));
	the_argv[1] = NULL;

	for( arg=1; arg <argc; arg++ )
		{
		unsigned char *p = u_str( argv[arg] );

		while( *p )
			{
			while( isspace(*p) )
				p++;

			if( *p == '\0' )
				break;

			if( *p == '-' )
				{
				unsigned char *start = p;
				unsigned char *value = NULL;

				p++;
				while( *p && !isspace(*p) )
					{
					if( value == NULL && (*p == '=' || *p == ':') )
						value = p+1;
					p++;
					}
				{
				int key_length = value - start;
				int val_length = p - value;

				if( value == NULL )
					{
					value = p;
					key_length = value - start;
					val_length = 0;
					}
				else
					{
					key_length = value - start - 1;
					val_length = p - value;
					}

				if( key_length >= 2 && _str_nicmp( u_str("-restore"), start, key_length ) == 0 )
					{
					_str_ncpy( restore_arg, value, val_length );
					restore_arg[val_length] ='\0';
					}
				else if( key_length >= 2 && _str_nicmp( u_str("-package"), start, key_length ) == 0 )
					{
					_str_ncpy( package_arg, value, val_length );
					package_arg[val_length] ='\0';
					free( the_argv[0] );
					the_argv[0] = savestr( package_arg );
					}
				else if( key_length >= 3 && _str_nicmp( u_str("-nodisplay"), start, key_length ) == 0 )
					{
					nodisplay_arg = 1;
					}
				else if( (key_length == 9 && _str_nicmp( u_str("-geometry"), start, key_length ) == 0 )
				     || (key_length == 8 && _str_nicmp( u_str("-display"), start, key_length ) == 0 ) )
					{
					int length = _str_len( start );
					the_argv[the_argc] = malloc_ustr( length+1 );
					_str_ncpy( the_argv[the_argc], p, length );
					the_argv[the_argc][length] = '\0';
					the_argc++;
					p = u_str("");
					}
				else
					{
					printf( "Unknown option %.*s\n\r", key_length, start );
					exit(1);
					}
				}
				}
			else
				{
				int  length = _str_len( p );
				the_argv[the_argc] = malloc_ustr( length+1 );
				_str_ncpy( the_argv[the_argc], p, length );
				the_argv[the_argc][length] = '\0';
				the_argc++;
				p = u_str("");
				}
			}
		}
	}

#if !defined(XWINDOWS)
int init_gui_terminal( struct trmcontrol *tt, unsigned char *device )
	{
	return 0;
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
	extern int is_motif, dpy_fd;
	if (is_motif)
	    fd = dpy_fd;
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

void invoke_debug(void)
	{
	return;
	}

void _dbg_msg( unsigned char *fmt, ... )
	{
	va_list argp;

	int size;
	unsigned char buf[128];

	va_start( argp, fmt );

	size = do_print( fmt, &argp, buf, sizeof( buf )-1);
	buf[ size ] = 0;

 	fprintf( stdout,"\r%s\n\r", buf );
 	fflush( stdout );
	}

void start_unix_timer( int PNOTUSED(id) )
	{
	return;
	}

void stop_unix_timer( int PNOTUSED(id) )
	{
	return;
	}

int wait_for_activity(void)
	{
	unsigned char buf[128];
	int i;
	int size;

	size = tt->k_input_event( tt, buf, sizeof( buf ) );
	if( size >= 1 )
		{
		for( i=0; i<size; i++ )
			{
			keyboard_interrupt( tt, buf[i] );
			}
		return 1;
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

void get_user_full_name( unsigned char *users_name, unsigned char *users_full_name )
	{
	struct passwd *pw = getpwnam( s_str(users_name) );

	if( pw == NULL )
		users_full_name = NULL;
	else
		_str_cpy( users_full_name, pw->pw_gecos );
	}

void get_system_name( unsigned char *PNOTUSED(converted_system_name) )
	{
	return;
	}

void fatal_error( int code )
	{
	printf("\nFatal Error %d\n", code );
	return;
	}

int put_config_env( unsigned char *name, unsigned char *value )
	{
	char buf[128];

	sprintfl( u_str(buf), sizeof( buf ), u_str("%s=%s"), name, value );
	return putenv( buf );
	}

unsigned char *get_config_env( unsigned char *name )
	{
	char *value = getenv( s_str(name) );

	if( value != NULL )
		return u_str(value);
	
	if( _str_cmp( "emacs_path", name ) == 0 )
		return u_str( "emacs_user: emacs_library:" );

	if( _str_cmp( "emacs_user", name ) == 0 )
		return u_str( "HOME:/emacs" );

	if( _str_cmp( "emacs_library", name ) == 0 )
		return image_path;

	if( _str_cmp( "sys_login", name ) == 0 )
		return u_str( "HOME:/" );

	return NULL;
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

	for (i = 1; i <= fd_max; i++)
	    {
	    if( read_info[i].cb != NULL
	    && FD_ISSET( i, &readfds_resp ) )
		read_info[i].cb(read_info[i].param, &i, NULL);
	    if( write_info[i].cb != NULL
	    && FD_ISSET( i, &writefds_resp ) )
		write_info[i].cb(write_info[i].param, &i, NULL);
	    }

	if( FD_SET( fd, &readfds_resp ) )
	    return read (fd, buf, count);
	return 0;
	}

char *cuserid( char *PNOTUSED(str) )
	{
	return getlogin();
	}

/*
 *	Emacs server code
 */
#define SERVER_BUFFER_SIZE 16384
extern struct bound_name bound_string_node;

static char server_fifo[256];
static char client_fifo[256];

static XtInputId emacs_server_input_id;
static int emacs_server_read_fd = -1;
static int emacs_server_write_fd = -1;
static union
	{
	struct prognode node;
	unsigned char string[sizeof(struct prognode)+SERVER_BUFFER_SIZE];
	}
		emacs_server_command_string;

struct prognode emacs_server_command;

void emacs_server_worker( struct emacs_work_item *item );

struct emacs_work_item emacs_server_work_item =
	{
	{ NULL, NULL },
	emacs_server_worker,
	0, NULL
	};

void emacs_server_worker( struct emacs_work_item *item )
	{
	int client_command;
	int emacs_client_write_fd;

	if( item->num <= 0 )
		return;
	
	client_command = find_mac( u_str("UI-unix-client-command") );
	if( client_command < 0 )
		return;

	emacs_server_command.p_proc = mac_bodies[client_command];

	exec_prog( &emacs_server_command );

	/* this will broken until the client is synced up */
	emacs_client_write_fd = open( client_fifo, O_WRONLY );
	if( emacs_client_write_fd < 0 )
		return;

	switch( ml_value->exp_type )
	{
	default:
		/* send a 1 byte nul string */
		write( emacs_client_write_fd, "", 1 );
		break;
	case ISINTEGER:
		{
		char buf[20];
		sprintf( buf, "%d", ml_value->exp_int );
		write( emacs_client_write_fd, buf, strlen(buf) );
		}
		break;
	case ISSTRING:
		write( emacs_client_write_fd, ml_value->exp_v.v_string, ml_value->exp_int );
		break;
	}

	close( emacs_client_write_fd );
	}

void emacs_server_callback(XtPointer PNOTUSED(str), int *fd, XtInputId* PNOTUSED(id) )
	{
	char *buffer = s_str(&emacs_server_command_string.node.p_args[1].pa_char);
	int size = read( *fd, buffer, SERVER_BUFFER_SIZE - 1 );

	/* printf("emacs_server_callback read %d bytes\n", size ); */

	if( size > 0 )
		{
		emacs_server_command_string.node.p_args[0].pa_int = size;
		buffer[size] = '\0';

		emacs_server_work_item.num = size;
		if( emacs_server_work_item.header.next == NULL )
			work_add_item( &emacs_server_work_item );
		}
	}

extern XtInputId add_input (int channel, long int mask, XtInputCallbackProc input_request, struct process_blk *npb);
extern void remove_input (XtInputId id);

void start_emacs_server()
	{
	char *home_env = getenv("HOME");
	char *fifo_name = getenv("BEMACS_FIFO");

	if( emacs_server_read_fd >= 0 )
		return;

	if( home_env == NULL )
		return;

	/* setup the MLisp structures */
	emacs_server_command.p_nargs = 1;
	emacs_server_command.p_active = 0;
	emacs_server_command.p_args[0].pa_node = &emacs_server_command_string.node;

	emacs_server_command_string.node.p_proc = &bound_string_node;
	emacs_server_command_string.node.p_nargs = 0;
	emacs_server_command_string.node.p_active = 0;
	emacs_server_command_string.node.p_args[0].pa_int = 0;

	if( fifo_name == NULL )
		fifo_name = ".emacs_command";

	server_fifo[0] = '\0';
	if( fifo_name[0] != '/' )
		{
		strcpy( server_fifo, home_env );
		strcat( server_fifo, "/" );
		}
	strcat( server_fifo, fifo_name );

	strcpy( client_fifo, server_fifo );	
	strcat( client_fifo, "_response" );

	emacs_server_read_fd = open( server_fifo, O_RDONLY|O_NONBLOCK );
	if( emacs_server_read_fd < 0 )
		return;

	emacs_server_write_fd = open( server_fifo, O_WRONLY|O_NONBLOCK );
	if( emacs_server_write_fd < 0 )
		return;

	emacs_server_input_id = add_input( emacs_server_read_fd, XtInputReadMask, emacs_server_callback, NULL );
	}

void stop_emacs_server()
	{
	if( emacs_server_read_fd < 0 )
		return;

	remove_input( emacs_server_input_id );
	close( emacs_server_read_fd );
	close( emacs_server_write_fd );
	}


int elapse_time = 0;

void fetch_elapse_time( struct expression *e )
	{
	struct timeval now;

	gettimeofday( &now, NULL );

	/*
	 *	calculate the time since startup in mSec.
	 *	we ignore the usec part of the start time
	 */
	elapse_time = (int)(now.tv_sec - emacs_start_time.tv_sec);
	elapse_time *= 1000;
	elapse_time += (int)(now.tv_usec/1000);

	*e->exp_v.v_value = elapse_time;
	}
