/*
 *	win_rtl.cpp
 *
 *	All the windows support functions for emacs live in here
 */
#include "stdafx.h"
#include "winemacs.h"

#include "mainfrm.h"
#include "winemdoc.h"
#include "winemvw.h"


int		win_emacs_quit;

#if !defined( _NT )
int GetLastError(void) { return 0; }

#endif

void _dbg_msg( unsigned char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = do_print( fmt, &argp, (unsigned char *)buf, sizeof( buf ) );
	buf[i] = '\0';

	MessageBox( NULL, buf, "Debug", MB_ICONINFORMATION );
	/* and log to debug terminal */
	DebugPrintf( "%s", buf );
	}

void DebugPrintf( char *fmt, ... )
	{
	char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	strcpy( buf, "Emacs: " );
	i = do_print( u_str(fmt), &argp, (unsigned char *)&buf[7], sizeof( buf )-7 );
	buf[i+7] = 0;

	OutputDebugString( buf );
	}

void wait_abit( void )
	{
	//	start the timer
	UINT status = theView->SetTimer( 'Z', 100, NULL );
	if( status == 0 )
	{
		TRACE("Failed to allocate timer\n");
		return;
	}

	theView->m_tick = 0;
	while( !theView->m_tick && input_pending == 0 )
		wait_for_activity();
	theView->KillTimer( 'Z' );	
	}

unsigned char *get_tmp_path(void)
	{
	static unsigned char tmp_path[MAXPATHLEN];

#if defined(_NT)
	DWORD len;

	len = GetTempPath( sizeof( tmp_path ), s_str(tmp_path));
#elif defined(_MSDOS)                                   
	tmp_path[0] = 0;
	GetTempFileName( 0, "z", 0, s_str( tmp_path ) );
	{ 
	unsigned char *p;

	p = _str_rchr( tmp_path, PATH_CH );
	if( p != NULL )
		*p = 0;
	}
#elif defined( vms )
	_str_cpy( tmp_buf, "sys$scratch:");
#else 
#error "Need a temp path"
#endif
	return tmp_path;
	}

unsigned char *get_config_env( unsigned char *name )
	{
	static unsigned char get_config_env_string[256];

	get_config_env_string[0] = '\0';

	CString value = AfxGetApp()->GetProfileString
		(
		"Environment", s_str(name),
		s_str(get_env( name ))
		);

	strcpy( s_str( get_config_env_string ), value );

	if( get_config_env_string[0] != '\0' )
		return get_config_env_string;

	return NULL;
	}

int put_config_env( unsigned char *name, unsigned char *value )
	{
	return AfxGetApp()->WriteProfileString
		(
		"Environment", s_str(name),
		value[0] == '\0' ? NULL : s_str(value)
		);
	}

char *get_device_name_translation( char *name )
	{
	static char get_device_env_string[256];
	CString str;

	get_device_env_string[0] = '\0';

	str = theApp.GetProfileString( "DeviceNames", s_str(name), "" );
	if( !str.IsEmpty() && str.GetLength() < sizeof( get_device_env_string ) )
		{
		strcpy( get_device_env_string, str );
		return get_device_env_string;
		}
	else
		return NULL;
	}

void invoke_debug(void)
	{
	return;
	}

int profile_emacs(void)
	{
	return no_value_command();
	}

int dump_histogram(void)
	{
	return no_value_command();
	}

#if defined(_NT)
#if !defined(MAX_USERNAME_LENGTH)
# define MAX_USERNAME_LENGTH 32
#endif

static char user_name_buf[MAX_USERNAME_LENGTH];
char *cuserid( char *str )
	{
	DWORD buf_size = sizeof( user_name_buf );
	if( !GetUserName( user_name_buf, &buf_size ) )
		return NULL;
	else
		return user_name_buf;
	}
#elif defined(_MSDOS)
char *cuserid( char * i )
	{
	return NULL;
	}
#else
#error "Need cuserid logic..."
#endif

int interlock_dec( volatile int *counter )
	{
	return InterlockedDecrement( (long *)counter );
	}

int interlock_inc( volatile int *counter )
	{
	return InterlockedIncrement( (long *)counter );
	}

void conditional_wake(void)
	{
	return;
	}

void get_user_full_name( unsigned char *users_name, unsigned char *users_full_name )
	{
#if defined(_NT)
	DWORD size = 16;
	users_full_name[0] = '\0';
	GetUserName( s_str(users_full_name), &size );
#endif
	}

void get_system_name( unsigned char *system_name )
	{
#if defined(_NT)
	DWORD size = MAX_COMPUTERNAME_LENGTH;
	system_name[0] = '\0';
	GetComputerName( s_str(system_name), &size );
#endif
	}

static int start_time; 
int elapse_time;

void fetch_elapse_time( expression * )
	{
	if( start_time == 0 )
		start_time = GetTickCount();

	elapse_time = GetTickCount() - start_time;
	}
