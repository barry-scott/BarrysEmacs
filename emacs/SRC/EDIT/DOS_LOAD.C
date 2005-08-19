/*
 *	dos_load.c
 *		copyright (c) 1993 Barry A. Scott
 */

#include <emacs.h>
#include <conio.h>
#include <msdos.h>

extern void init_memory(void);
static void process_args( int argc, char **argv );

char image_location[64];
static char restore_arg[64];
static char package_arg[64];
static char the_argc;
static unsigned char *the_argv[20];

int main( int argc, char **argv )
	{
	cprintf( "\r%s\n\r", version_string );

	strcpy( image_location, argv[0] );

	init_memory();

	strcpy( default_environment_file, "emacs$user:emacsdos.env" );

	process_args( argc, argv );

	return emacs( the_argc, the_argv, restore_arg, "char", "" );
	}

void process_args( int argc, char **argv )
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

			if( *p == '/' )
				{
				unsigned char *start = p;
				unsigned char *value = NULL;

				p++;
				while( *p && !isspace(*p) && *p != '/' )
					{
					if( value == NULL && (*p == '=' || *p == ':') )
						value = p+1;
					p++;
					}
				if( value == NULL )
					value = p;

				{
				int key_length = value - start - 1;
				int val_length = p - value;

				if( key_length > 2 && _str_nicmp( u_str("/restore"), start, key_length ) == 0 )
					{
					_str_ncpy( restore_arg, value, val_length );
					restore_arg[val_length] ='\0';
					}
				else if( key_length > 2 && _str_nicmp( u_str("/package"), start, key_length ) == 0 )
					{
					_str_ncpy( package_arg, value, val_length );
					package_arg[val_length] ='\0';
					free( the_argv[0] );
					the_argv[0] = savestr( package_arg );
					}
				else
					{
					cprintf( "Unknown option %.*s\n\r", key_length, start );
					exit(1);
					}
				}
				}
			else
				{
				unsigned char *start = p;

				while( *p && !isspace( *p ) && *p != '/' )
					p++;

				{
				int length = p - start;
				the_argv[the_argc] = malloc_ustr( length+1 );
				_str_ncpy( the_argv[the_argc], start, length );
				the_argv[the_argc][length] = '\0';
				the_argc++;
				}
				}
			}
		}
	}

void _dbg_msg( unsigned char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = do_print( fmt, argp, (unsigned char *)buf, sizeof( buf ) );
	buf[i] = 0;

	cprintf( "DBG_MSG: %s\n\r", buf );
	}

void fatal_error( int code )
	{
	cprintf( "FATAL_ERROR: %d\n", code );

	exit(0);
	}

void invoke_debug(void)
	{
	return;
	}

int interlock_dec( volatile int *cell )
	{
	(*cell)++;
	if( *cell == 0 )
		return 0;
	if( *cell < 0 )
		return -1;
	else
		return 1;
	}

int interlock_inc( volatile int *cell )
	{
	(*cell)--;
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
	users_name[0] = '\0';
	users_full_name[0] = '\0';

	return;
	}

char *cuserid( char *str )
	{
	return NULL;
	}

void wait_abit(void)
	{
	union REGS target_regs, current_regs;
	long target, current;

	target_regs.h.ah = DOS_get_time;
	intdos( &target_regs, &target_regs );
	target = ((((target_regs.h.ch * 60l) + target_regs.h.cl) * 60l) +
			target_regs.h.dh) * 100l + target_regs.h.dl;
	target += 10;

	current_regs.h.ah = DOS_get_time;
	do
		{
		intdos( &current_regs, &current_regs );
		current = ((((current_regs.h.ch * 60l) + current_regs.h.cl)*60l)+
				current_regs.h.dh) * 100l + current_regs.h.dl;
		tt->k_check_for_input( tt );
		}
	while( current < target && !input_pending );
	}

int wait_for_activity(void)
	{
	tt->k_check_for_input( tt );

	return 0;
	}

void get_system_name( unsigned char *system_name )
	{
	system_name[0] = '\0';
	}

static FILE *emacs_ini;

char *get_profile_string( char *section, char *entry )
	{
	static int first_time = 0;
	static char buf[256];
	char section_buf[128]; int section_len;
	char look_for[128];
	int look_for_len;

	if( first_time != 0 )
		return NULL;
	first_time = 1;
	if( emacs_ini == NULL )
		emacs_ini = fio_open( ".ini", 0, image_location, NULL );
	first_time = 0;
	if( emacs_ini == NULL )
		return NULL;

	fseek( emacs_ini, SEEK_SET, 0l );

	strcpy( section_buf, "[" );
	strcat( section_buf, section );
	strcat( section_buf, "]" );
	section_len = strlen( section_buf );

	for(;;)
		{
		if( fgets( buf, sizeof( buf ), emacs_ini ) == NULL )
			return NULL;

		if( strnicmp( section_buf, buf, section_len ) == 0 )
			break;
		}

	strcpy( look_for, entry );
	strcat( look_for, "=" );
	look_for_len = strlen( look_for );

	for(;;)
		{
		int len;

		if( fgets( buf, sizeof( buf ), emacs_ini ) == NULL )
			return NULL;

		if( buf[0] == '[' )
			return NULL;

		len = strlen( buf );
		if( len > 1 )
			buf[len-1] = '\0';

		if( strnicmp( look_for, buf, look_for_len ) == 0 )
			return strchr( buf, '=' ) + 1;
		}
	}

int put_config_env( unsigned char *name, unsigned char *value )
	{
	if( emacs_ini )
		{
		fio_close( emacs_ini );
		emacs_ini = NULL;
		}
	return 0;
	}

unsigned char *get_config_env( unsigned char *entry )
	{
	static unsigned char buf[128];
	unsigned char *p = u_str( get_profile_string( "Environment", s_str( entry ) ) );
	if( p == NULL )
		return NULL;
	_str_cpy( buf, p );
	return buf;
	}

char *get_device_name_translation( char *name )
	{
	static char buf[128];
	char *p = get_profile_string( "DeviceNames", name );
	if( p == NULL )
		return NULL;
	_str_cpy( buf, p );
	return buf;
	}

/* empty routines to stub out UI- functions */
int ui_add_menu(void) { return 0; }
int ui_edit_copy(void) { return 0; }
int ui_edit_paste(void) { return 0; }
int ui_file_open(void) { return 0; }
int ui_file_save_as(void) { return 0; }
int ui_find(void) { return 0; }
int ui_find_and_replace(void) { return 0; }
int ui_frame_maximize(void) { return 0; }
int ui_frame_minimize(void) { return 0; }
int ui_frame_restore(void) { return 0; }
int ui_list_menus(void) { return 0; }
int ui_remove_menu(void) { return 0; }
int ui_switch_buffer(void) { return 0; }
int ui_add_to_recent_file_list(void) { return 0; }
int ui_window_cascade(void) { return 0; }
int ui_window_maximize(void) { return 0; }
int ui_window_minimize(void) { return 0; }
int ui_window_restore(void) { return 0; }
int ui_window_tile_horz(void) { return 0; }
int ui_window_tile_vert(void) { return 0; }
int ui_win_exec (void) { return 0; }
int ui_win_help (void) { return 0; }
int ui_view_tool_bar(void) { return 0; }
int ui_view_status_bar(void) { return 0; }
void UI_update_window_title(void) { return; }
int ui_open_file_readonly;
unsigned char ui_open_file_name[64];
unsigned char ui_save_as_file_name[64];
unsigned char ui_filter_file_list[256];
unsigned char ui_search_string[128];
unsigned char ui_replace_string[128];

int elapse_time;

void fetch_elapse_time( expression *e )
	{
	}
