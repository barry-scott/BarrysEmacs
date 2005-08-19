//
//	win_ui.cpp
//
//	This module contains all the UI-xxx functions for emacs.
//	THe implementation of the functions will be in other
//	modules.
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#include <win_emacs.h>
#include <win_registry.h>

#include <win_doc.h>
#include <win_view.h>
#include <win_find.h>
#include <win_select_buffer.h>
#include <win_opt.h>
#include <shellapi.h>

//
//	File handling
//
SystemExpressionRepresentationInt ui_open_file_readonly;
SystemExpressionRepresentationString ui_open_file_name;
SystemExpressionRepresentationString ui_save_as_file_name;
SystemExpressionRepresentationString ui_filter_file_list;

//
//	(UI-file-open window-title)
//
int ui_file_open(void)
	{
	unsigned char filter_file_list[1024];
	char open_file_name[256];
	char open_dir_name[256];
	open_file_name[0] = '\0';
	open_dir_name[0] = '\0';

	if( ui_filter_file_list.length() >= sizeof( filter_file_list ) )
		{
		error("UI-filter-file-list is too long");
		return 0;
		}

	if( ui_open_file_name.length() >= sizeof( open_file_name ) )
		{
		error("UI-open-file-name is too long");
		return 0;
		}

	if( ui_filter_file_list.isNull() )
		ui_filter_file_list =
			"All Files (*.*)\n*.*\n";

	const unsigned char *ip = ui_filter_file_list.data();
	unsigned char *op = filter_file_list;
	while( (*op = *ip) != 0 )
		{
		if( *ip =='\n' )
			*op = '\0';
		op++; ip++;
		}

	OPENFILENAME open_file;
	memset( (void *)&open_file, 0, sizeof( open_file ) );
	open_file.lStructSize = sizeof( open_file );
	open_file.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;
	open_file.lpstrFilter = s_str(filter_file_list);
	open_file.lpstrFile = s_str( open_file_name );
	open_file.nMaxFile = sizeof( open_file_name );
	open_file.lpstrFileTitle = NULL;
	open_file.lpstrTitle = "Emacs Open file";
	open_file.Flags = OFN_PATHMUSTEXIST;
	open_file.lpstrDefExt = NULL;

	if( ui_open_file_readonly != 0 )
		open_file.Flags |= OFN_READONLY;

	// seup the initial dir field
	int path_char_pos = ui_open_file_name.asString().last( PATH_CH );
	if( path_char_pos < 0 )
		{
		// no path in the file name
		_str_cpy( open_file_name, ui_open_file_name.data() );
		open_file.lpstrInitialDir = NULL;
		}
	else
		{
		// there is a path so split into two parts
		EmacsString path_part( ui_open_file_name.asString()( 0, path_char_pos ) );
		EmacsString name_part( ui_open_file_name.asString()( path_char_pos+1, ui_open_file_name.length() ) );

		_str_cpy( open_dir_name, path_part.data() );
		_str_cpy( open_file_name, name_part.data() );

		open_file.lpstrInitialDir = open_dir_name;
		}

	int result = GetOpenFileName( &open_file );

	if( result )
		{
		ui_open_file_name = open_file_name;
		chdir_and_set_global_record( u_str(".") );
		ui_open_file_readonly = (int)(open_file.Flags&OFN_READONLY) != 0;
		}

	ml_value = Expression( result );

	return 0;
	}

int ui_file_save_as(void)
	{
	static OPENFILENAME save_as_file;
	unsigned char filter_file_list[256];
	unsigned char file_dir[256];
	unsigned char file_name[256];

	const unsigned char *ip = ui_filter_file_list.data();
	unsigned char *op = filter_file_list;
	while( (*op = *ip) != 0 )
		{
		if( *ip =='\n' )
			*op = '\0';
		op++; ip++;
		}

	save_as_file.lStructSize = sizeof( save_as_file );
	save_as_file.hwndOwner = AfxGetApp()->m_pMainWnd->m_hWnd;
	save_as_file.lpstrFilter = s_str(filter_file_list);

	if( ui_save_as_file_name.length() > sizeof( file_dir )-1 )
		{
		error("UI-save-as-file-name is too long");
		return 0;
		}

	_str_cpy( file_dir, ui_save_as_file_name.data() );
	_str_cpy( file_name, "" );
	unsigned char *p = _str_rchr( file_dir, '\\' );
	if( p != NULL )
		{
		*p = '\0';
		_str_cpy( file_name, p+1 );
		}
	else
		{
		_str_cpy( file_name, file_dir );
		_str_cpy( file_dir, "" );
		}

	save_as_file.lpstrFile = s_str(file_name);
	save_as_file.nMaxFile = sizeof( file_name );
	save_as_file.lpstrInitialDir = s_str(file_dir);
	save_as_file.lpstrTitle = "Emacs Save file";
	save_as_file.Flags = OFN_PATHMUSTEXIST
			| OFN_OVERWRITEPROMPT
			| OFN_HIDEREADONLY;
	save_as_file.lpstrDefExt = NULL;

	int result = GetSaveFileName( &save_as_file );

	if( result )
		{
		ui_save_as_file_name = file_name;
		chdir_and_set_global_record( u_str(".") );
		}
	else
		{
		DWORD error_code = CommDlgExtendedError();
		if( error_code != 0 )
			_dbg_msg( FormatString("save as failed code %d") << error_code );
		}


	ml_value = Expression( result );

	return 0;
	}

int ui_edit_copy(void)
{
	HANDLE hData;
	LPSTR lData;
	unsigned char ch;
	int size, i, start, end;

	if( !bf_cur->b_mark.isSet() )
		return 0;

	start = dot;
	end = bf_cur->b_mark.to_mark();

	if( end < start )
		{
		start = end;
		end = dot;
		}

	size = end - start;
	for( i=start; i<end; i++ )
		if( bf_cur->char_at( i ) == '\n' )
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
		if( (ch = bf_cur->char_at( i )) == '\n' )
			{
			*lData++ = '\r';
			*lData++ = '\n';
			}
		else
			*lData++ = ch;
	*lData++ = '\0';

	::GlobalUnlock( hData );

	if( AfxGetApp()->m_pMainWnd->OpenClipboard() )
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

	if( !AfxGetApp()->m_pMainWnd->OpenClipboard() )
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

	if( input_mode == 1 ) gui_input_mode_before_insert();

	s = start = u_str(str);
	for(;;)
		{
		if( *s == '\r' || *s == '\0' )
			{
			int i = s - start;
			if( i )
				bf_cur->ins_cstr( u_str( start ), i );
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

int ui_select_buffer(void)
	{
	EmacsString title;
	EmacsString buffer_name;
	EmacsString buffer_types;

	if( check_args( 1, 3 ) )
		return 0;

	int n = cur_exec->p_nargs;
	if( string_arg( 1 ) )
		title = ml_value.asString();
	if( n >= 2 && string_arg( 2 ) )
		buffer_name = ml_value.asString();
	if( n == 3 && string_arg( 3 ) )
		buffer_types = ml_value.asString();
	if( n < 3 )
		buffer_types = "file,macro,scratch";
	if( n < 2 )
		buffer_name = bf_cur->b_buf_name.data();
	if( err )
		return 0;

	CBufferSelectDlg Dlg;

	Dlg.InitParameters( title, buffer_name, buffer_types );

	if( Dlg.DoModal() == IDOK )
		{
		ml_value = Expression( Dlg.m_buffer_name );
		}
	else
		error("UI-select-buffer cancelled by user");

	return 0;
	}

int ui_window_cascade(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_WINDOW_CASCADE );

	void_result();
	return 0;
	}

int ui_frame_state(void)
	{
	//
	// 	Get the current position of the frame
	//
	WINDOWPLACEMENT where;
	where.length = sizeof( where );

	if( AfxGetApp()->m_pMainWnd->GetWindowPlacement( &where ) )
		{
		switch( where.showCmd )
			{
		case SW_MAXIMIZE:	// and SW_SHOWMAXIMIZED
			ml_value = Expression( "maximized" );
			break;

		case SW_SHOWMINIMIZED:
		case SW_MINIMIZE:
		case SW_FORCEMINIMIZE:
			ml_value = Expression( "minimized" );
			break;

		case SW_NORMAL:	// and SW_SHOWNORMAL
		default:
			ml_value = Expression( "normal" );
			break;
			}
		}
	else
		{
		ml_value = Expression( "normal" );
		}	

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

int ui_frame_to_foreground(void)
	{
	AfxGetApp()->m_pMainWnd->SetForegroundWindow();

	void_result();
	return 0;
	}

int ui_window_next(void)
	{
	((CWinemacsView *)theActiveView)->activateNextView();

	void_result();
	return 0;
	}

int ui_window_previous(void)
	{
	((CWinemacsView *)theActiveView)->activatePreviousView();

	void_result();
	return 0;
	}

int ui_window_new(void)
	{
	AfxGetApp()->m_pMainWnd->SendMessage( WM_COMMAND, ID_WINDOW_NEW );

	void_result();
	return 0;
	}

int ui_window_maximize(void)
	{
	CWinemacsView *view = (CWinemacsView *)theActiveView;
	CMDIChildWnd *mdi_child = (CMDIChildWnd *)view->GetParent();
	mdi_child->MDIMaximize();

	void_result();
	return 0;
	}

int ui_window_minimize(void)
	{
	CWinemacsView *view = (CWinemacsView *)theActiveView;
	CMDIChildWnd *mdi_child = (CMDIChildWnd *)view->GetParent();
	mdi_child->SendMessage( WM_SYSCOMMAND, SC_ICON, 0l );

	void_result();
	return 0;
	}

int ui_window_restore(void)
	{
	CWinemacsView *view = (CWinemacsView *)theActiveView;
	CMDIChildWnd *mdi_child = (CMDIChildWnd *)view->GetParent();
	mdi_child->MDIRestore();

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

extern int display_scroll_bars;
int ui_view_scroll_bars(void)
	{
	display_scroll_bars = display_scroll_bars ? 0 : 1;
	screen_garbaged = 1;

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
	char *str;
	unsigned int num;
	}
		help_key_map[] =
	{
	"context", HELP_CONTEXT,
	"contents", HELP_CONTENTS,
	"setcontents", HELP_SETCONTENTS,
	"contextpopup", HELP_CONTEXTPOPUP,
	"key", HELP_KEY,
	"partialkey", HELP_PARTIALKEY,
	"command", HELP_COMMAND,
	"forcefile", HELP_FORCEFILE,
	"helponhelp", HELP_HELPONHELP,
	"quit", HELP_QUIT,
	NULL, 0,
	};

int ui_win_help(void)
	{
	int result;

	if( check_args( 3, 3 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	EmacsString help_file = ml_value.asString();

	struct string_int_map *p = help_key_map;

	if( !string_arg( 2 ) )
		{
		void_result();
		return 0;
		}

	EmacsString command = ml_value.asString();
	while( p->str != NULL )
		if( command == p->str )
			break;
		else
			p++;

	if( p->str == NULL )
		{
		error( FormatString("Unknown help command '%s'") << command );
		void_result();
		return 0;
		}

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
			{
			void_result();
			return 0;
			}
		dwData = (DWORD)(ml_value.asString().data());
		break;
	}

	// call the help engine
	result = WinHelp( AfxGetApp()->m_pMainWnd->m_hWnd, help_file.sdata(), p->num, dwData );

	if( !result )
		error( "WinHelp failed" );

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
	"minimized", SW_SHOWMINIMIZED,
	"maximized", SW_SHOWMAXIMIZED,
	"normal", SW_SHOWNORMAL,
	NULL, 0
	};

int ui_win_exec(void)
	{
	struct string_int_map *p = show_state_map;

 	if( !string_arg( 2 ) )
		return 0;

	EmacsString state = ml_value.asString();
	while( p->str != NULL )
		if( state == p->str )
			break;
		else
			p++;

	if( p->str == NULL )
		{
		error( FormatString("Unknown window show state '%s'") << state );
		return 0;
		}

	if( !string_arg( 1 ) )
		return 0;

	EmacsString command = ml_value.asString();
	WORD status = (WORD)WinExec( command.sdata(), p->num );
	if( status < 32 )
		error( FormatString("Failed to execute windows program %d") << status );

	void_result();
	return 0;
	}

//
//	(UI-windows-shell-execute operation file parameters directory show)
//

int ui_shell_execute(void)
	{
	// arg operation
 	if( !string_arg( 1 ) )
		return 0;

	EmacsString operation = ml_value.asString();

	// arg file
 	if( !string_arg( 2 ) )
		return 0;

	EmacsString file = ml_value.asString();

	// arg parameters
 	if( !string_arg( 3 ) )
		return 0;

	EmacsString parameters = ml_value.asString();

	// arg directory
 	if( !string_arg( 4 ) )
		return 0;

	EmacsString directory = ml_value.asString();

	// arg show
 	if( !string_arg( 5 ) )
		return 0;

	EmacsString show = ml_value.asString();

	struct string_int_map *p = show_state_map;

	while( p->str != NULL )
		if( show == p->str )
			break;
		else
			p++;

	if( p->str == NULL )
		{
		error( FormatString("Unknown window show state '%s'") << show );
		return 0;
		}

	// do the shell execute

	HINSTANCE status = ShellExecute
		(
		NULL,		// parent window
		operation,
		file,
		parameters,
		directory,
		p->num
		);
	if( int(status) < 32 )
		error( FormatString("Failed to execute windows program %d") << int(status) );

	void_result();
	return 0;
	}

int ui_add_to_recent_file_list(void)
	{
	if( !string_arg( 1 ) )
		return 0;
	EmacsString recent_file = ml_value.asString();

	AfxGetApp()->AddToRecentFileList( recent_file.sdata() );
	return 0;
	}

int ui_add_to_recent_directory_list(void)
	{
	if( !string_arg( 1 ) )
		return 0;
	EmacsString recent_dir = ml_value.asString();

#pragma message( "Crit: Need to implement recent dir list" )
//	AfxGetApp()->AddToRecentFileList( recent_file.sdata() );
	return 0;
	}

