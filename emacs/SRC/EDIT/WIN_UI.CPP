//
//	win_ui.cpp
//
//	This module contains all the UI-xxx functions for emacs.
//	THe implementation of the functions will be in other
//	modules.
//

#include "stdafx.h"
#include "winemacs.h"

#include "winemdoc.h"
#include "winemvw.h"
#include "finddlg.h"
#include "switchdl.h"
#include "win_opt.h"


//
//	File handling
//
int ui_open_file_readonly;
unsigned char ui_open_file_name[64];
unsigned char ui_save_as_file_name[64];
unsigned char ui_filter_file_list[256] = {
		"C/C++ Source code\n*.c;*.h;*.cpp;*.rc;*.def\n"
		"MLisp source code\n*.ml;*.mlp,*.key\n"
		"Text Files\n*.txt\n"
		"Ini Files\n*.ini;*.sys;*.bat;*.btm\n"
		"All Files (*.*)\n*.*\n" };

//
//	(UI-file-open window-title)
//
int ui_file_open(void)
	{
	static OPENFILENAME open_file;
	unsigned char filter_file_list[256];

	unsigned char *ip = ui_filter_file_list;
	unsigned char *op = filter_file_list;
	while( (*op = *ip) != 0 )
		{
		if( *ip =='\n' )
			*op = '\0';
		op++; ip++;
		}

	open_file.lStructSize = sizeof( open_file );
	open_file.hwndOwner = theView->m_hWnd;
	open_file.lpstrFilter = s_str(filter_file_list);
	ui_open_file_name[0] = '\0';
	open_file.lpstrFile = s_str( ui_open_file_name );
	open_file.nMaxFile = sizeof( ui_open_file_name );
	open_file.lpstrFileTitle = NULL;
	open_file.lpstrInitialDir = ".";
	open_file.lpstrTitle = "Emacs Open file";
	open_file.Flags = OFN_PATHMUSTEXIST;
	open_file.lpstrDefExt = NULL;

	int result = GetOpenFileName( &open_file );

	if( result )
		{
		_str_lwr( ui_open_file_name );
		chdir_and_set_global_record( u_str(".") );
		ui_open_file_readonly = (int)(open_file.Flags&OFN_READONLY) != 0;
		}

	release_expr( ml_value );
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = result;

	return 0;
	}

int ui_file_save_as(void)
	{
	static OPENFILENAME save_as_file;
	unsigned char filter_file_list[256];
	unsigned char file_dir[256];
	unsigned char file_name[256];

	unsigned char *ip = ui_filter_file_list;
	unsigned char *op = filter_file_list;
	while( (*op = *ip) != 0 )
		{
		if( *ip =='\n' )
			*op = '\0';
		op++; ip++;
		}

	save_as_file.lStructSize = sizeof( save_as_file );
	save_as_file.hwndOwner = theView->m_hWnd;
	save_as_file.lpstrFilter = s_str(filter_file_list);

	_str_cpy( file_dir, ui_save_as_file_name );
	_str_cpy( file_name, "" );
	unsigned char *p = _str_rchr( ui_save_as_file_name, '\\' );
	if( p != NULL )
		{
		*p = '\0';
		_str_cpy( file_name, p+1 );
		}

	save_as_file.lpstrFile = s_str(file_name);
	save_as_file.nMaxFile = sizeof( file_name );
	save_as_file.lpstrInitialDir = s_str(file_dir);
	save_as_file.lpstrTitle = "Emacs Save file";
	save_as_file.Flags = OFN_PATHMUSTEXIST
			| OFN_OVERWRITEPROMPT;
	save_as_file.lpstrDefExt = ".txt";

	int result = GetSaveFileName( &save_as_file );

	if( result )
		{
		_str_cpy( ui_save_as_file_name, file_name );
		_str_lwr( ui_save_as_file_name );
		chdir_and_set_global_record( u_str(".") );
		}
	else
		{
		DWORD error_code = CommDlgExtendedError();
		if( error_code != 0 )
			_dbg_msg( u_str("save as failed code %d"), error_code );
		}

	release_expr( ml_value );
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = result;

	return 0;
	}

int ui_edit_copy(void)
{
	HANDLE hData;
	LPSTR lData;
	unsigned char ch;
	int size, i, start, end;

	if (bf_cur -> b_mark == 0)
		return 0;

	start = dot;
	end = to_mark( bf_cur -> b_mark );

	if( end < start )
		{
		start = end;
		end = dot;
		}

	size = end - start;
	for( i=start; i<end; i++ )
		if( char_at( i ) == '\n' )
			size++;

	hData = ::GlobalAlloc( GHND|GMEM_DDESHARE, size+1 );
	if( !hData )
		return 0;

	if( (lData = (LPSTR)::GlobalLock( hData ) ) == NULL )
		{
		::GlobalFree( hData );
		return 0;
		}

	for( i=start; i<end; i++ )
		if( (ch = char_at( i )) == '\n' )
			{
			*lData++ = '\r';
			*lData++ = '\n';
			}
		else
			*lData++ = ch;
	*lData++ = '\0';

	::GlobalUnlock( hData );

	if( theView->OpenClipboard() )
		{
		::EmptyClipboard();
		::SetClipboardData( CF_TEXT, hData );
		::CloseClipboard();
		}
	else
		::GlobalFree( hData );

	void_result();
	return 0;
	}

int ui_edit_paste(void)
{
	HANDLE h;
	LPSTR str;
	unsigned char *s, *start;

	if( !theView->OpenClipboard() )
		return 0;

	h = ::GetClipboardData( CF_TEXT );
	if( !h )
		{
		::CloseClipboard();
		return 0;
		}
	str = (LPSTR)::GlobalLock( h );
	if( !str )
		{
		::CloseClipboard();
		return 0;
		}

	s = start = u_str(str);
	for(;;)
		{
		if( *s == '\r' || *s == '\0' )
			{
			int i = s - start;
			if( i )
				ins_cstr( u_str( start ), i );
			if( *s == '\0' )
				break;
			else
				s++; /* skip CR */
			start = s;
			}
		s++;
		}
	::GlobalUnlock( h );
	::CloseClipboard();

	void_result();

	return 0;
	}

int ui_find(void)
	{
	if( find_dlg == NULL )
		{
		find_dlg = new CFindDlg;
		find_dlg->CreateModeless( CFindDlg::IDD );
		}

	void_result();
	return 0;
	}

int ui_find_and_replace(void)
	{
	if( find_dlg == NULL )
		{
		find_dlg = new CFindReplaceDlg;
		find_dlg->CreateModeless( CFindReplaceDlg::IDD );
		}

	void_result();
	return 0;
	}

int ui_switch_buffer(void)
	{
	CSwitchDlg Dlg;
	Dlg.DoModal();

	void_result();
	return 0;
	}

int ui_window_cascade(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_WINDOW_CASCADE );

	void_result();
	return 0;
	}

int ui_frame_maximize(void)
	{
	AfxGetApp()->m_pMainWnd->ShowWindow( SW_MAXIMIZE );

	void_result();
	return 0;
	}

int ui_frame_minimize(void)
	{
	AfxGetApp()->m_pMainWnd->ShowWindow( SW_MINIMIZE );

	void_result();
	return 0;
	}

int ui_frame_restore(void)
	{
	AfxGetApp()->m_pMainWnd->ShowWindow( SW_RESTORE );

	void_result();
	return 0;
	}

int ui_window_maximize(void)
	{
	((CMDIFrameWnd *)AfxGetApp()->m_pMainWnd)->MDIMaximize( theView->GetParent() );

	void_result();
	return 0;
	}

int ui_window_minimize(void)
	{
	theView->GetParent()->SendMessage( WM_SYSCOMMAND, SC_ICON, 0l );

	void_result();
	return 0;
	}

int ui_window_restore(void)
	{
	((CMDIFrameWnd *)AfxGetApp()->m_pMainWnd)->MDIRestore( theView->GetParent() );

	void_result();
	return 0;
	}

int ui_window_tile_horz(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_WINDOW_TILE_HORZ );

	void_result();
	return 0;
	}

int ui_window_tile_vert(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_WINDOW_TILE_VERT );

	void_result();
	return 0;
	}

int ui_view_tool_bar(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_VIEW_TOOLBAR );

	void_result();
	return 0;
	}

int ui_view_status_bar(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_VIEW_STATUS_BAR );

	void_result();
	return 0;
	}

//
//	Interface onto the windows help system
//
//	(UI-windows-help "Help-file" "Cmd" "Data")
//

struct string_int_map {
	unsigned char *str;
	unsigned int num;
	}
		help_key_map[] =
	{
	u_str("context"), HELP_CONTEXT,
	u_str("contents"), HELP_CONTENTS,
	u_str("setcontents"), HELP_SETCONTENTS,
	u_str("contextpopup"), HELP_CONTEXTPOPUP,
	u_str("key"), HELP_KEY,
	u_str("partialkey"), HELP_PARTIALKEY,
	u_str("command"), HELP_COMMAND,
	u_str("forcefile"), HELP_FORCEFILE,
	u_str("helponhelp"), HELP_HELPONHELP,
	u_str("quit"), HELP_QUIT,
	NULL, 0,
	};

int ui_win_help(void)
	{
	int result;

	if( check_args( 3, 3 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	unsigned char *help_file = savestr( ml_value->exp_v.v_string );

	struct string_int_map *p = help_key_map;

	if( !string_arg( 2 ) )
		goto error_exit;

	while( p->str != NULL )
		if( _str_cmp( p->str, ml_value->exp_v.v_string ) == 0 )
			break;
		else
			p++;

	if( p->str == NULL )
		{
		error( u_str("Unknown help command '%s'"), ml_value->exp_v.v_string );
		goto error_exit;
		}

	{
	DWORD dwData = 0;
	switch( p->num )
	{
		// commands that need dwData of 0l
	case HELP_CONTENTS:
	case HELP_FORCEFILE:
	case HELP_HELPONHELP:
	case HELP_QUIT:
		dwData = 0l;
		break;

		// commands that need an integer
	case HELP_CONTEXT:
	case HELP_SETCONTENTS:
	case HELP_CONTEXTPOPUP:
		dwData = numeric_arg( 3 );
		break;

		// commands that need a string
	case HELP_KEY:
	case HELP_PARTIALKEY:
	case HELP_COMMAND:
		if( !string_arg( 3 ) )
			goto error_exit;
		dwData = (DWORD)(ml_value->exp_v.v_string);
		break;
	}

	// call the help engine
	result = WinHelp( theView->m_hWnd, s_str(help_file), p->num, dwData );
	}
	if( !result )
		error( u_str("WinHelp failed") );
error_exit:
	free( help_file );

	void_result();
	return 0;
	}

//
//	Execute a windows program
//
//	(UI-windows-execute "command line" "show")
//
struct string_int_map show_state_map[] =
	{
	u_str("minimized"), SW_SHOWMINIMIZED,
	u_str("maximized"), SW_SHOWMAXIMIZED,
	u_str("normal"), SW_SHOWNORMAL,
	NULL, 0
	};

int ui_win_exec(void)
	{
	struct string_int_map *p = show_state_map;

 	if( !string_arg( 2 ) )
		return 0;

	while( p->str != NULL )
		if( _str_cmp( p->str, ml_value->exp_v.v_string ) == 0 )
			break;
		else
			p++;

	if( p->str == NULL )
		{
		error( u_str("Unknown window show state '%s'"),
			ml_value->exp_v.v_string );
		return 0;
		}

	if( !string_arg( 1 ) )
		return 0;

	WORD status = (WORD)WinExec( s_str( ml_value->exp_v.v_string ), p->num );
	if( status < 32 )
		error( u_str("Failed to execute windows program %d"), status );

	void_result();
	return 0;
	}

int ui_add_to_recent_file_list(void)
	{
	if( !string_arg( 1 ) )
		return 0;
	AfxGetApp()->AddToRecentFileList( s_str(ml_value->exp_v.v_string) );
	return 0;
	}
