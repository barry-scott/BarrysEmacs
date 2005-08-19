/****************************************************************************

    PROGRAM: win_cmd.c

    PURPOSE: Input template for Windows applications

    FUNCTIONS:

	WinMain() - calls initialization function, processes message loop
	InitApplication() - initializes window data and registers window
	InitInstance() - saves instance hndle and creates main window
	MainWndProc() - processes messages
	About() - processes messages for "About" dialog box

****************************************************************************/

#include <win_incl.h>
#include <emacs.h>
#include <emacswin.h>

/*
 *	Forward routine declarations
 */
void emacs_cls_WM_INITMENUPOPUP(HWND hwnd, HMENU hMenu, int item, BOOL fSystemMenu);
int emacs_cls_WM_COMMAND(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);

LRESULT CALLBACK EXPORT win_switch_buffer( HWND hdlg, UINT message, WPARAM wParam, LPARAM lParam );
void EmacsSetupDC( HWND hwnd, HDC hDC );
void EmacsResetDC(HDC hDC);
LRESULT CALLBACK EXPORT about_dlg_proc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );

LRESULT CALLBACK EXPORT find_dlg_proc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
int find_cls_WM_COMMAND( HWND hdlg, int id, HWND hwndCtl, UINT codeNotify );

LRESULT CALLBACK EXPORT replace_dlg_proc( HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam );
DWORD do_about( void );
void do_switch_buffer( void );

/*
 *	Open and Save file stuff
 */
OPENFILENAME open_file;
unsigned char open_file_name[64];

OPENFILENAME save_as_file;
unsigned char save_as_name[64];

/*
 *	Search stuff
 */
static unsigned char last_gui_search_string[128];
static unsigned char last_gui_replace_string[128];

HWND h_find_dlg, h_replace_dlg;
int is_find_replace;

/*
 *	Emacs interface vaiables
 */
int emacs_quit = 0;

static cursor_x, cursor_y;

#define CARET_WIDTH	(cursor_mode ? (nCharWidth+1) : 2)
#define CARET_HEIGHT	nCharHeight
#define CARET_XPOS	((cursor_x*nCharWidth)-1+WINDOW_BORDER)
#define CARET_YPOS	(cursor_y*nCharHeight)
#define EmacsWIN_STYLE	WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_VSCROLL


void emacs_cls_WM_INITMENUPOPUP(HWND hwnd, HMENU hMenu, int item, BOOL fSystemMenu)
	{
	switch( item )
	{
	case MENU_EDIT:
		if( OpenClipboard( hwnd ) )
			{
			if( IsClipboardFormatAvailable( CF_TEXT )
			|| IsClipboardFormatAvailable( CF_OEMTEXT ) )
				EnableMenuItem( hMenu, IDM_PASTE, MF_ENABLED );
			else
				EnableMenuItem( hMenu, IDM_PASTE, MF_GRAYED );
			CloseClipboard();
			}
		break;
	case MENU_FILE:
		if( bf_modified != 0 )
			EnableMenuItem( hMenu, IDM_FILE_SAVE, MF_ENABLED );
		else
			EnableMenuItem( hMenu, IDM_FILE_SAVE, MF_GRAYED );
	}

	FORWARD_WM_INITMENUPOPUP(hwnd, hMenu, item, fSystemMenu, DefWindowProc);
	}

int emacs_cls_WM_COMMAND(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
	{
	switch( id )
	{
	case IDM_ABOUT:
		do_about();
		break;

	case IDM_QUIT:
		input_char_string( u_str("\x18\x03") );
		break;

	case IDM_UNDO:
		input_char_string( u_str("\x18\x15") );
		break;

	case IDM_WRITE_FILE_EXIT:
		input_char_string( u_str("\x18\x06") );
		break;

	case IDM_SAVE_ALL:
		input_char_string( u_str("\x18\x0d") );
		break;

	case IDM_FIND:
		if( h_replace_dlg )
			SendMessage( h_replace_dlg, WM_CLOSE, 0, 0 );
		if( !h_find_dlg )
			{
			DLGPROC dlg;

			dlg = (DLGPROC)MakeProcInstance
				(
				(FARPROC)find_dlg_proc,
				emacs_hinst
				);
			is_find_replace = 0;
			h_find_dlg = CreateDialog
				(
				emacs_hinst,
				MAKEINTRESOURCE( DLG_FIND ),
				hwnd,
				WATCOM_DLGPROC (DLGPROC)MakeProcInstance
					(
					WATCOM_DLGPROC find_dlg_proc,
					emacs_hinst
					)
				);
			}
		break;

	case IDM_REPLACE:
		if( h_find_dlg )
			SendMessage( h_find_dlg, WM_CLOSE, 0, 0 );
		if( !h_replace_dlg )
			{
			is_find_replace = 1;
			h_replace_dlg = CreateDialog
				(
				emacs_hinst,
				MAKEINTRESOURCE( DLG_REPLACE ),
				hwnd,
				WATCOM_DLGPROC (DLGPROC)MakeProcInstance( (FARPROC)find_dlg_proc, emacs_hinst )
				);
			}
		break;

	case IDM_SPLITWINDOW:
		input_char_string( u_str("\x18" "2") );
		break;

	case IDM_ONEWINDOW:
		input_char_string( u_str("\x18" "1") );
		break;

	case IDM_DELWINDOW:
		input_char_string( u_str("\x18" "d") );
		break;

	case IDM_SWITCHBUFFER:
		do_switch_buffer();
		break;

	case IDM_FILE_SAVE:
		write_current_file();
		break;

	case IDM_FILE_OPEN:
		{
		open_file.lStructSize = sizeof( open_file );
		open_file.hwndOwner = hwnd;
		open_file.lpstrFilter =
				"C/C++ Source code\0*.c;*.h;*.cpp;*.rc;*.def\0"
				"MLisp source code\0*.ml;*.mlp,*.key\0"
				"Text Files\0*.txt\0"
				"Ini Files\0*.ini;*.sys;*.bat;*.btm\0"
				"All Files (*.*)\0*.*\0";
		open_file_name[0] = '\0';
		open_file.lpstrFile = s_str( open_file_name );
		open_file.nMaxFile = sizeof( open_file_name );
		open_file.lpstrFileTitle = NULL;
		open_file.lpstrInitialDir = NULL;
		open_file.lpstrTitle = "Emacs Open file";
		open_file.Flags = OFN_PATHMUSTEXIST;
		open_file.lpstrDefExt = NULL;

		if( !GetOpenFileName( &open_file ) )
			break;

		_str_lwr( open_file_name );
		chdir_and_set_global_record( u_str(".") );
		visit_file( open_file_name, 1, 1, u_str("") );
		}
		break;

	case IDM_FILE_SAVE_AS:
		{
		save_as_file.lStructSize = sizeof( save_as_file );
		save_as_file.hwndOwner = hwnd;
		save_as_file.lpstrFilter =
				"Text Files\0*.txt;*.doc;readme*.*\0"
				"C Source code\0*.c;*.h;*.rc;*.def\0"
				"Emacs source code\0*.ml;*.mlp\0"
				"Ini Files\0*.ini;*.sys;*.bat;*.btm\0";
		if( bf_cur->b_fname != NULL )
			_str_cpy( save_as_name, bf_cur->b_fname );
		else
			save_as_name[0] = '\0';

		save_as_file.lpstrFile = s_str( save_as_name );
		save_as_file.nMaxFile = sizeof( save_as_name );
		save_as_file.lpstrFileTitle = NULL;
		save_as_file.lpstrInitialDir = NULL;
		save_as_file.lpstrTitle = "Emacs Save file";
		save_as_file.Flags = OFN_PATHMUSTEXIST
				| OFN_OVERWRITEPROMPT;
		save_as_file.lpstrDefExt = ".txt";

		if( !GetOpenFileName( &save_as_file ) )
			break;

		_str_lwr( open_file_name );
		chdir_and_set_global_record( u_str(".") );
		write_named_file( save_as_name );
		}
		break;

	case IDM_SELECTFONT:
		{
		char buf[20];

		if (! ChooseFont(&emacs_cf))
			return FALSE;

		_fstrcpy( buf, emacs_cf.lpLogFont->lfFaceName );
		WritePrivateProfileString
		( "Emacs", "TextFontName", buf, "emacs060.ini" );
		wsprintf( buf, "%d", emacs_cf.lpLogFont->lfHeight );
		WritePrivateProfileString
		( "Emacs", "TextFontSize", buf, "emacs060.ini" );

		emacs_win_new_font();
		}
		break;

	case IDM_SETMARK:
		set_mark_command();
		break;

	case IDM_CUT:
	case IDM_COPY:
		{
		HANDLE hData;
		LPSTR lData;
		unsigned char ch;
		int size, i, start, end;

		if (bf_cur -> b_mark == 0)
			return TRUE;

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

		hData = GlobalAlloc( GHND, size+1 );
		if( !hData ) return TRUE;

		if( !(lData = GlobalLock( hData ) ) )
			{
			GlobalFree( hData );
			return TRUE;
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

		GlobalUnlock( hData );

		if( OpenClipboard( hwnd ) )
			{
			EmptyClipboard();
			SetClipboardData( CF_TEXT, hData );
			CloseClipboard();

			if( id == IDM_CUT )
				erase_region();
			}
		else
			GlobalFree( hData );
		}
 		break;

	case IDM_PASTE:
		{
		HANDLE h;
		LPSTR str;
		unsigned char *s, *start;

		if( OpenClipboard( hwnd ) )
		{
		h = GetClipboardData( CF_TEXT );
		if( !h )
			{
			CloseClipboard();
			break;
			}
		str = GlobalLock( h );
		if( !str )
			{
			CloseClipboard();
			break;
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
		GlobalUnlock( h );
		CloseClipboard();
		}
		break;
		}
	case IDM_CLEAR:
		erase_region();
		break;

	case IDM_PARSEERROR:
		input_char_string( u_str("\033xparse-errors\r") );
		break;

	case IDM_NEXTERROR:
		input_char_string( u_str("\033xnext-error\r") );
		break;

	case IDM_PREVERROR:
		input_char_string( u_str("\033xprevious-error\r") );
		break;

	case IDM_MAKE:
		if( compile_command[0] )
			{
			int status;

			status = WinExec( s_str(compile_command), SW_SHOW );
			if( status < 31 )
				{
				char buf[80];
				sprintf( buf, "WinExec error %d", status );
				MessageBox( NULL, buf, "Error", MB_ICONINFORMATION );
				}
			}
		break;

	case IDM_DEBUG:
		if( debug_command[0] )
			{
			int status;

			status = WinExec( s_str(debug_command), SW_SHOW );
			if( status < 31 )
				{
				char buf[80];
				sprintf( buf, "WinExec error %d", status );
				MessageBox( NULL, buf, "Error", MB_ICONINFORMATION );
				}
			}
		break;

	default:
		FORWARD_WM_COMMAND(hwnd, id, hwndCtl, codeNotify, DefWindowProc);
		break;
	}
	do_dsp(1);

	return FALSE;
	}

unsigned char buf_name[256];

void do_switch_buffer(void)
	{
	DialogBox
		(
		emacs_hinst,
		MAKEINTRESOURCE( DLG_SWITCHBUFFER ),
		emacs_hwnd,
		WATCOM_DLGPROC (DLGPROC)MakeProcInstance((FARPROC)win_switch_buffer,emacs_hinst)
		);	
	}

LRESULT CALLBACK EXPORT win_switch_buffer
	(
	HWND hdlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	int i;
	LPARAM status;

	switch (message)
	{
	case WM_INITDIALOG:
		status = SendDlgItemMessage( hdlg, CB_RESETCONTENT, ID_SWITCHBUFFER, 0, 0L );

		for( i=0; i<n_buffers; i++ )
			status = SendDlgItemMessage( hdlg, ID_SWITCHBUFFER, CB_ADDSTRING,
				0, (LPARAM) (LPCSTR) buf_names[i] );
		return (TRUE);

	case WM_LBUTTONDBLCLK:
		wParam = IDOK;
		/* fall into WM_COMMAND */
	case WM_COMMAND:
		if (wParam == IDOK)
			{
			buf_name[0] = '\0';
			status = GetDlgItemText( hdlg, ID_SWITCHBUFFER,
				(LPSTR)buf_name, sizeof( buf_name ) );
			if( buf_name[0] != '\0' )
				{
				set_bfn( buf_name );
				tie_win(wn_cur->w_next ? wn_cur : windows, bf_cur);
				}
			EndDialog(hdlg, TRUE);
			return (TRUE);
			}
		else if( wParam == IDCANCEL )
			{
			EndDialog(hdlg, FALSE);
			return TRUE;
			}
		break;
	}
	return FALSE;
	}


LRESULT CALLBACK EXPORT find_dlg_proc
	(
	HWND hdlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText( hdlg, ID_FIND, s_str(last_gui_search_string) );
		CheckDlgButton( hdlg, ID_MATCH_CASE, bf_mode.md_foldcase == 0 );
		CheckDlgButton( hdlg, ID_RE_SEARCH, 1 );
		CheckDlgButton( hdlg, ID_INC_SEARCH, 0 );
		if( is_find_replace )
			{
			SetDlgItemText( hdlg, ID_REPLACE_WITH, s_str(last_gui_replace_string) );
			EnableWindow( GetDlgItem( hdlg, ID_REPLACE ), 0 );
			EnableWindow( GetDlgItem( hdlg, ID_REPLACE_ALL ), 0 );
			}
		return TRUE;

	HANDLE_MSG( hdlg, WM_COMMAND, find_cls_WM_COMMAND );

	case WM_CLOSE:
		EndDialog( hdlg, 0 );
		h_find_dlg = 0;
		h_replace_dlg = 0;

		un_set_mark_command();
		break;
	}

	return FALSE;
	}

int find_cls_WM_COMMAND( HWND hdlg, int id, HWND hwndCtl, UINT codeNotify )
	{
	int re, match_case, incremental, direction;
	int np;

	direction = 1;

	switch( id )
	{
	case ID_FIND_PREV:
		direction = -1;
	case IDOK:	
		/*
		 *	Fetch the values from the dialog
		 */
		if( bf_cur->b_mark != NULL )
			if( (direction > 0 && dot < to_mark( bf_cur->b_mark ))
			|| (direction < 0 && dot > to_mark( bf_cur->b_mark )) )
				exchange_dot_and_mark();
			
		last_gui_search_string[0] = '\0';
		GetDlgItemText
		(
		hdlg, ID_FIND,
		(LPSTR)last_gui_search_string,
		sizeof( last_gui_search_string )
		);
		/* make sure that the replace string is saved even if we do
		   not use it here */
		if( is_find_replace )
			{
			last_gui_replace_string[0] = '\0';
			GetDlgItemText
			(
			hdlg, ID_REPLACE_WITH,
			(LPSTR)last_gui_replace_string,
			sizeof( last_gui_replace_string )
			);
			}
		re = (int)IsDlgButtonChecked( hdlg, ID_RE_SEARCH );
		match_case = (int)IsDlgButtonChecked( hdlg, ID_MATCH_CASE );
		incremental = (int)IsDlgButtonChecked( hdlg, ID_INC_SEARCH );


		un_set_mark_command();

		if( last_gui_search_string[0] != '\0' )
			{
			int old_case_fold = bf_mode.md_foldcase;

			bf_mode.md_foldcase = match_case == 0;
			np = search( last_gui_search_string, direction, dot, re );
			bf_mode.md_foldcase = old_case_fold;
			if( np > 0 )
				{
				region_around_match( 0 );
				do_dsp(0);
				if( is_find_replace )
					{
					EnableWindow( GetDlgItem( hdlg, ID_REPLACE ), 1 );
					EnableWindow( GetDlgItem( hdlg, ID_REPLACE_ALL ), 1 );
					}
				}
			else
				{
				un_set_mark_command();
				if( is_find_replace )
					{
					EnableWindow( GetDlgItem( hdlg, ID_REPLACE ), 0 );
					EnableWindow( GetDlgItem( hdlg, ID_REPLACE_ALL ), 0 );
					}
				}
			}

		return TRUE;

	case IDCANCEL:
		EndDialog( hdlg, 0 );
		h_find_dlg = 0;
		h_replace_dlg = 0;
		return TRUE;

	case ID_REPLACE:
		{
		un_set_mark_command();
		last_gui_replace_string[0] = '\0';
		GetDlgItemText
		(
		hdlg, ID_REPLACE_WITH,
		(LPSTR)last_gui_replace_string,
		sizeof( last_gui_replace_string )
		);
		re = (int)IsDlgButtonChecked( hdlg, ID_RE_SEARCH );
		set_dot( sea_glob.sea_loc2 );
		search_replace_once( last_gui_replace_string, re );
		EnableWindow( GetDlgItem( hdlg, ID_REPLACE ), 0 );

		do_dsp(0);
		}
		break;

	case ID_REPLACE_ALL:
		break;

	case ID_RE_SEARCH:
	case ID_INC_SEARCH:
	case ID_MATCH_CASE:
		{
		int value;

		value = (int)IsDlgButtonChecked( hdlg, id );
		CheckDlgButton( hdlg, id, value == 0 );
		}
		break;
	}

	return FALSE;
	}

/****************************************************************************

	FUNCTION: about_dlg_proc(HWND, unsigned, WORD, LONG)

	PURPOSE:  Processes messages for "About" dialog box

	MESSAGES:

	WM_INITDIALOG - initialize dialog box
	WM_COMMAND	- Input received

****************************************************************************/

extern int bytes_allocated;

LRESULT CALLBACK EXPORT about_dlg_proc
	(
	HWND hDlg,
	UINT message,
	WPARAM wParam,
	LPARAM lParam
	)
	{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt( hDlg, ID_ALLOC, (WORD)(bytes_allocated/1024l), 0 );
		SetDlgItemText( hDlg, ID_VERSION, "Version " EMACS_VERSION );
		return (TRUE);

	case WM_COMMAND:
		if (wParam == IDOK)
		{
		EndDialog(hDlg, TRUE);
		return (TRUE);
		}
		break;
	}
	return (FALSE);
	}

DWORD do_about( void )
	{
	DialogBox
	(
	emacs_hinst,
	MAKEINTRESOURCE( DLG_ABOUT ),
	emacs_hwnd,
	WATCOM_DLGPROC (DLGPROC)about_dlg_proc
	);

	return 0l;
	}

BOOL CALLBACK EXPORT yes_no_dlg_proc
	(
	HWND hDlg,
	unsigned message,
	WORD wParam,
	LONG lParam
	)
	{
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemText( hDlg, ID_CODE, (LPSTR)lParam );
		return (TRUE);

	case WM_COMMAND:
		if (wParam == IDYES || wParam == IDNO)
		{
		EndDialog(hDlg, wParam == IDYES);
		return (TRUE);
		}
		break;
	}
	return (FALSE);
	}

int win_yes_or_no( unsigned char *prompt, int def_yes )
	{
	int result;
	UINT what = MB_YESNO|MB_ICONQUESTION;

	if( !def_yes )
		/* true if no is the default */
		what |= MB_DEFBUTTON2;
	result = MessageBox( emacs_hwnd, s_str(prompt), "Emacs", what );

	return result == IDYES;
	}

void win_error( unsigned char *msg, ... )
	{
	MessageBox( NULL, s_str(msg), "Error", MB_ICONSTOP );
	}

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
int ui_open_file_readonly;
unsigned char ui_open_file_name[64];
unsigned char ui_save_as_file_name[64];
unsigned char ui_filter_file_list[256];
unsigned char ui_search_string[128];
unsigned char ui_replace_string[128];
