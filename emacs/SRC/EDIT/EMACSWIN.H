/*
 *	Emacs windows header file
 */
#define WINDOW_BORDER		0
#define EMACS_WIN_STYLE	WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_VSCROLL
#define V_SCROLL_RANGE		10000

#define	WM_COMMAND_LINE		WM_USER+100

#define	MENU_FILE		0
#define	MENU_EDIT		1
#define	MENU_SEARCH		2

#define IDM_FIRST_ID		100
/*	File menu	*/
#define IDM_SELECTFONT		100
#define	IDM_QUIT		101
#define	IDM_FILE_OPEN		102
#define	IDM_FILE_SAVE		103
#define	IDM_FILE_SAVE_AS	104
#define	IDM_WRITE_FILE_EXIT	105
#define	IDM_SAVE_ALL		106

/*	Edit menu	*/
#define	IDM_UNDO		107
#define	IDM_CUT			108
#define	IDM_COPY		109
#define	IDM_PASTE		110
#define	IDM_CLEAR		111
#define	IDM_SETMARK		112

/*	Search menu	*/
#define	IDM_FIND		113
#define	IDM_REPLACE		114

/*	Window menu	*/
#define	IDM_SPLITWINDOW		115
#define	IDM_ONEWINDOW		116
#define	IDM_DELWINDOW		117
#define	IDM_SWITCHBUFFER	118

/*	User Menu	*/
#define	IDM_PARSEERROR		119
#define	IDM_NEXTERROR		120
#define	IDM_PREVERROR		121
#define	IDM_MAKE		122
#define	IDM_DEBUG		123
#define	IDM_EXECUTE		124

/*	Help menu	*/
#define IDM_ABOUT		125
#define	IDM_HELP		126

#include <emacsdlg.h>

EXTERN void emacs_cls_WM_INITMENUPOPUP(HWND hwnd, HMENU hMenu, int item, BOOL fSystemMenu);
EXTERN int emacs_cls_WM_COMMAND(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
EXTERN void emacs_cls_WM_SIZE(HWND hwnd, UINT state, int cx, int cy);
EXTERN void emacs_cls_WM_SETFOCUS(HWND hwnd, HWND hwndOldFocus);
EXTERN void emacs_cls_WM_KILLFOCUS(HWND hwnd, HWND hwndNewFocus);
EXTERN int emacs_cls_WM_LBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_LBUTTONUP( HWND hwnd, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_LBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_RBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_RBUTTONUP( HWND hwnd, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_RBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_MBUTTONDOWN( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_MBUTTONUP( HWND hwnd, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_MBUTTONDBLCLK( HWND hwnd, BOOL fDoubleClick, int x, int y, UINT flags );
EXTERN int emacs_cls_WM_BUTTON( int button, HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags );
EXTERN int emacs_cls_WM_KEYDOWN( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
EXTERN void emacs_cls_WM_KEYUP( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
EXTERN void emacs_cls_WM_CHAR(HWND hwnd, UINT ch, int cRepeat);
EXTERN int emacs_cls_WM_SYSKEYUP( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
EXTERN int emacs_cls_WM_SYSKEYDOWN( HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags );
EXTERN void emacs_cls_WM_VSCROLL(HWND hwnd, HWND hwndCtl, UINT code, int pos);
EXTERN void emacs_cls_WM_PAINT(HWND hwnd);
EXTERN void emacs_cls_WM_CLOSE(HWND hwnd);
EXTERN void invoke_debug(void);
EXTERN int find_cls_WM_COMMAND( HWND hdlg, int id, HWND hwndCtl, UINT codeNotify );
EXTERN void input_char_string( unsigned char * );
EXTERN WORD WINAPI EXPORT ctrl_break_handler( WORD a, WORD b );
extern int emacs_win_new_font(void);

extern HWND h_find_dlg;
extern HWND h_replace_dlg;
extern HFONT emacs_hfont;
extern HWND emacs_hwnd;
extern int emacs_quit;
extern MSG emacs_win_msg;
extern char szAppName[6];
extern CHOOSEFONT emacs_cf;
extern HANDLE emacs_hinst;
