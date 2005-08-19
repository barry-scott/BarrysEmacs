/* 
 * Motif X-Window system pseudo-terminal driver for DEC Emacs
 *
 *	Nick Emery Jun-93
 *	adapted from spm's X driver 7/85	MIT Project Athena
 *
 *	Graham R. Cobb 17-Sep-1993
 *	Add XComposeStatus argument to XLookupString to allow compose
 *	sequences to work.
 */

#include <emacs.h>

int is_motif = 0;
#ifdef XWINDOWS
#include <sys/time.h>
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/keysym.h> 
#include <Xm/Xm.h>
#include <Xm/MainW.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/ScrollBar.h>
#include <xwin_icn.h>

void xwin_update_line (struct trmcontrol *tt,struct emacs_line *old_line, struct emacs_line *new_line, int row);
/*
 * Own Storage:
 */
static int number_one = 1;
static int number_zero = 0;

/* Processing data */
static int VisibleX, VisibleY;
static int SavedX, SavedY;
static int CursorExists;
static int InUpdate;
static int innerBorder;
static int flexlines;
static int pixelWidth, pixelHeight;
static struct emacs_line empty_line;

#define MOUSE_BUTTON_EVENTS 1
#define MOUSE_MOTION_EVENTS 2

/* keyboard conversion table */
struct conv_keys
    {
    KeySym key_code;
    char *translation;
    char *enh_translation;
    char *shift_translation;
    char *ctrl_translation;
    };

static struct conv_keys convert[] =
    {
#ifdef DXK_Remove
    {DXK_Remove,	"\033[3~",       NULL,        NULL,        "\033[53~"},
#endif
    {XK_BackSpace,	"\177",		"\177",	      "\177",	   "\177"},
#ifdef KEYBOARD_TYPE_PC
    {XK_Home,		"\033[1~",	 NULL,	      NULL,	   NULL},
    {XK_End,		"\033[4~",	 NULL,	      NULL,	   NULL},
    {XK_KP_Space,	"\033[3~",	 NULL,	      NULL,	   NULL},

    {XK_KP_F1,		"\033OP",	 NULL,	      NULL,	   NULL},
    {XK_KP_Multiply,	"\033OQ",        NULL,        NULL,        NULL},
    {XK_KP_Divide,	"\033OR",        NULL,        NULL,        NULL},
    {XK_KP_Subtract,	"\033OS",        NULL,        NULL,        NULL},
    {XK_KP_Add,		"\033Ol",	 NULL,        "\033Om",	   NULL},

    {XK_KP_Delete,	"\033On",        "\033On",    NULL,        NULL},
    {XK_KP_Insert,	"\033Op",        "\033Op",    NULL,        NULL},
    {XK_KP_End,		"\033Oq",        "\033Oq",    NULL,        NULL},
    {XK_KP_Down,	"\033Or",        "\033Or",    NULL,        NULL},
    {XK_KP_Page_Down,	"\033Os",        "\033Os",    NULL,        NULL},
    {XK_KP_Left,	"\033Ot",        "\033Ot",    NULL,        NULL},
    {XK_KP_Right,	"\033Ov",        "\033Ov",    NULL,        NULL},
    {XK_KP_Home,	"\033Ow",        "\033Ow",    NULL,        NULL},
    {XK_KP_Up,		"\033Ox",        "\033Ox",    NULL,        NULL},
    {XK_KP_Page_Up,	"\033Oy",        "\033Oy",    NULL,        NULL},

    {XK_F1,		"\033[17~",      "\033[17~",  "\033[117~", "\033[67~"},
    {XK_F2,		"\033[18~",      "\033[18~",  "\033[118~", "\033[68~"},
    {XK_F3,		"\033[19~",      "\033[19~",  "\033[119~", "\033[69~"},
    {XK_F4,		"\033[20~",      "\033[20~",  "\033[120~", "\033[70~"},
    {XK_F5,		"\033[21~",      "\033[21~",  "\033[121~", "\033[71~"},
    {XK_F6,		"\033[23~",      "\033[23~",  "\033[123~", "\033[73~"},
    {XK_F7,		"\033[24~",      "\033[24~",  "\033[124~", "\033[74~"},
    {XK_F8,		"\033[25~",      "\033[25~",  "\033[125~", "\033[75~"},
    {XK_F9,		"\033[26~",      "\033[26~",  "\033[126~", "\033[76~"},
    {XK_F10,		"\033[28~",      "\033[28~",  "\033[128~", "\033[78~"},
    {XK_F11,		"\033[29~",      "\033[29~",  "\033[129~", "\033[79~"},
    {XK_F12,		"\033[31~",      "\033[31~",  "\033[131~", "\033[81~"},
    {XK_F13,		"\033[32~",      "\033[32~",  "\033[132~", "\033[82~"},
    {XK_F14,		"\033[33~",      "\033[33~",  "\033[133~", "\033[83~"},
    {XK_F15,		"\033[34~",      "\033[34~",  "\033[134~", "\033[84~"},
#elif defined( KEYBOARD_TYPE_OLD_HP )
    {XK_KP_Multiply,	"\033OP",        NULL,        NULL,        NULL},
    {XK_KP_Divide,	"\033OQ",        NULL,        NULL,        NULL},
    {XK_KP_Add,		"\033OR",        NULL,        NULL,        NULL},
    {XK_KP_Subtract,	"\033OS",        NULL,        NULL,        NULL},
    {XK_KP_Tab,		"\033OM",        NULL,        NULL,        NULL},
    {XK_KP_Enter,	"\033Om",        "\033Om",    NULL,        NULL},
    {XK_KP_Separator,	"\033Ol",        "\033Ol",    NULL,        NULL},
    {XK_KP_Divide,	"",              NULL,        NULL,        NULL},

    {XK_F1,		"\033[17~",      "\033[17~",  "\033[117~", "\033[67~"},
    {XK_F2,		"\033[18~",      "\033[18~",  "\033[118~", "\033[68~"},
    {XK_F3,		"\033[19~",      "\033[19~",  "\033[119~", "\033[69~"},
    {XK_F4,		"\033[20~",      "\033[20~",  "\033[120~", "\033[70~"},
    {XK_Menu,		"\033[21~",      "\033[21~",  "\033[121~", "\033[71~"},
    {XK_F5,		"\033[23~",      "\033[23~",  "\033[123~", "\033[73~"},
    {XK_F6,		"\033[24~",      "\033[24~",  "\033[124~", "\033[74~"},
    {XK_F7,		"\033[25~",      "\033[25~",  "\033[125~", "\033[75~"},
    {XK_F8,		"\033[26~",      "\033[26~",  "\033[126~", "\033[76~"},
    {hpXK_ClearLine,	"\033[28~",      "\033[28~",  "\033[128~", "\033[78~"},
    {XK_Clear,		"\033[29~",      "\033[29~",  "\033[129~", "\033[79~"},
    {XK_F9,		"\033[31~",      "\033[31~",  "\033[131~", "\033[81~"},
    {XK_F10,		"\033[32~",      "\033[32~",  "\033[132~", "\033[82~"},
    {XK_F11,		"\033[33~",      "\033[33~",  "\033[133~", "\033[83~"},
    {XK_F12,		"\033[34~",      "\033[34~",  "\033[134~", "\033[84~"},
#elif defined( KEYBOARD_TYPE_NEW_HP )
    {XK_Num_Lock,	"\033OP",	 NULL,	      NULL,        NULL},
    {XK_KP_Divide,	"\033OQ",        NULL,        NULL,        NULL},
    {XK_KP_Multiply,	"\033OR",        NULL,        NULL,        NULL},
    {XK_KP_Subtract,	"\033OS",        NULL,        NULL,        NULL},
    {XK_KP_Add,		"\033Ol",        "\033Om",    NULL,        NULL},
    {XK_KP_Enter,	"\033OM",        NULL,        NULL,        NULL},

    {XK_F1,		"\033[17~",      "\033[17~",  "\033[117~", "\033[67~"},
    {XK_F2,		"\033[18~",      "\033[18~",  "\033[118~", "\033[68~"},
    {XK_F3,		"\033[19~",      "\033[19~",  "\033[119~", "\033[69~"},
    {XK_F4,		"\033[20~",      "\033[20~",  "\033[120~", "\033[70~"},
    {XK_F5,		"\033[21~",      "\033[21~",  "\033[121~", "\033[71~"},
    {XK_F6,		"\033[23~",      "\033[23~",  "\033[123~", "\033[73~"},
    {XK_F7,		"\033[24~",      "\033[24~",  "\033[124~", "\033[74~"},
    {XK_F8,		"\033[25~",      "\033[25~",  "\033[125~", "\033[75~"},
    {XK_F9,		"\033[26~",      "\033[26~",  "\033[126~", "\033[76~"},
    {XK_F10,		"\033[28~",      "\033[28~",  "\033[128~", "\033[78~"},
    {XK_F11,		"\033[29~",      "\033[29~",  "\033[129~", "\033[79~"},
    {XK_F12,		"\033[31~",      "\033[31~",  "\033[131~", "\033[81~"},
    {XK_Print,		"\033[32~",      "\033[32~",  "\033[132~", "\033[82~"},
    {XK_Scroll_Lock,	"\033[33~",      "\033[33~",  "\033[133~", "\033[83~"},
    {XK_Pause,		"\033[34~",      "\033[34~",  "\033[134~", "\033[84~"},
#else
    {XK_KP_F1,		"\033OP",        NULL,        NULL,        NULL},
    {XK_KP_F2,		"\033OQ",        NULL,        NULL,        NULL},
    {XK_KP_F3,		"\033OR",        NULL,        NULL,        NULL},
    {XK_KP_F4,		"\033OS",        NULL,        NULL,        NULL},
    {XK_KP_Equal,	"",              NULL,        NULL,        NULL},
    {XK_KP_Multiply,	"",              NULL,        NULL,        NULL},
    {XK_KP_Add,		"",              NULL,        NULL,        NULL},
    {XK_KP_Separator,	"\033Ol",        "\033Ol",    NULL,        NULL},
    {XK_KP_Subtract,	"\033Om",        "\033Om",    NULL,        NULL},
    {XK_KP_Divide,	"",              NULL,        NULL,        NULL},
    {XK_F1,		"\033[11~",      "\033[11~",  "\033[111~", "\033[61~"},
    {XK_F2,		"\033[12~",      "\033[12~",  "\033[112~", "\033[62~"},
    {XK_F3,		"\033[13~",      "\033[13~",  "\033[113~", "\033[63~"},
    {XK_F4,		"\033[14~",      "\033[14~",  "\033[114~", "\033[64~"},
    {XK_F5,		"\033[15~",      "\033[15~",  "\033[115~", "\033[65~"},
    {XK_F6,		"\033[17~",      "\033[17~",  "\033[117~", "\033[67~"},
    {XK_F7,		"\033[18~",      "\033[18~",  "\033[118~", "\033[68~"},
    {XK_F8,		"\033[19~",      "\033[19~",  "\033[119~", "\033[69~"},
    {XK_F9,		"\033[20~",      "\033[20~",  "\033[120~", "\033[70~"},
    {XK_F10,		"\033[21~",      "\033[21~",  "\033[121~", "\033[71~"},
    {XK_F11,		"\033[23~",      "\033[23~",  "\033[123~", "\033[73~"},
    {XK_F12,		"\033[24~",      "\033[24~",  "\033[124~", "\033[74~"},
    {XK_F13,		"\033[25~",      "\033[25~",  "\033[125~", "\033[75~"},
    {XK_F14,		"\033[26~",      "\033[26~",  "\033[126~", "\033[76~"},
    {XK_F15,		"\033[28~",      "\033[28~",  "\033[128~", "\033[78~"},
    {XK_F16,		"\033[29~",      "\033[29~",  "\033[129~", "\033[79~"},
    {XK_F17,		"\033[31~",      "\033[31~",  "\033[131~", "\033[81~"},
    {XK_F18,		"\033[32~",      "\033[32~",  "\033[132~", "\033[82~"},
    {XK_F19,		"\033[33~",      "\033[33~",  "\033[133~", "\033[83~"},
    {XK_F20,		"\033[34~",      "\033[34~",  "\033[134~", "\033[84~"},
#endif
    {XK_KP_Enter,	"\033OM",        NULL,        NULL,        NULL},
    {XK_Left,	   	"\033[D",        "\033OD",    NULL,        "\033[60~"},
    {XK_Up,	   	"\033[A",        "\033OA",    NULL,        "\033[57~"},
    {XK_Right,		"\033[C",        "\033OC",    NULL,        "\033[59~"},
    {XK_Down,		"\033[B",        "\033OB",    NULL,        "\033[58~"},
    {XK_KP_Decimal,	"\033On",        "\033On",    NULL,        NULL},
    {XK_KP_0,		"\033Op",        "\033Op",    NULL,        NULL},
    {XK_KP_1,		"\033Oq",        "\033Oq",    NULL,        NULL},
    {XK_KP_2,		"\033Or",        "\033Or",    NULL,        NULL},
    {XK_KP_3,		"\033Os",        "\033Os",    NULL,        NULL},
    {XK_KP_4,		"\033Ot",        "\033Ot",    NULL,        NULL},
    {XK_KP_5,		"\033Ou",        "\033Ou",    NULL,        NULL},
    {XK_KP_6,		"\033Ov",        "\033Ov",    NULL,        NULL},
    {XK_KP_7,		"\033Ow",        "\033Ow",    NULL,        NULL},
    {XK_KP_8,		"\033Ox",        "\033Ox",    NULL,        NULL},
    {XK_KP_9,		"\033Oy",        "\033Oy",    NULL,        NULL},
    {XK_Prior,		"\033[5~",       NULL,        NULL,        "\033[55~"},
    {XK_Next,		"\033[6~",       NULL,        NULL,        "\033[56~"},
    {XK_Select,		"\033[4~",       NULL,        NULL,        "\033[54~"},
    {XK_Menu,		"\033[29~",      NULL,        NULL,        NULL},
    {XK_Insert,		"\033[2~",       NULL,        NULL,        "\033[52~"},
    {XK_Find,		"\033[1~",       NULL,        NULL,        "\033[51~"},
    {XK_Help,		"\033[28~",      NULL,        NULL,        NULL}
    };

struct emacs_attr
	{
	GC gc;
	int underline;
	};

/* X-Windows data */
#define icon_bits write_scrolls_bits
#define icon_width write_scrolls_width
#define icon_height write_scrolls_height
#define SB_SIZE (100000)
Display *dpy;
int dpy_fd;
static Window win;
Widget app_shell, main_window, menu_bar, frame, drawing_area, vscroll;
static int fth, ftw, ftd;
static struct emacs_attr attr_array[256],cu_attr, rev_attr, curev_attr;
static Cursor arrowCursor;
static int cursors = 0;
int motif_iconic = 0;
static int initializing = 1;

XtAppContext app_context;

struct res
    {
    char *normal_font_name;	    /* Font for normal text */
#ifdef BOLDFONT
    char *bold_font_name;	    /* Font for bolded text */
#endif
    int rows;			    /* Number of character rows in the window */
    int columns;		    /* Number of character columns in the window */
    Pixel n_fg;			    /* Foreground for normal text */
    Pixel n_bg;			    /* Background for normal text */
    char *n_underline;
    Pixel ml_fg;		    /* Foreground for mode line text */
    Pixel ml_bg;		    /* Background for mode line text */
    char *ml_underline;
    Pixel hl_fg;		    /* Foreground for highlighted text */
    Pixel hl_bg;		    /* Background for highlighted text */
    char *hl_underline;
    Pixel c_fg;			    /* Foreground for cursor */
    Pixel c_bg;			    /* Background for cursor */
    char *c_underline;
    Pixel word_fg;		    /* word foreground colour */
    Pixel word_bg;		    /* word foreground colour */
    char *word_underline;
    Pixel string_fg;		    /* string foreground colour */
    Pixel string_bg;		    /* string foreground colour */
    char *string_underline;
    Pixel comment_fg;		    /* comment foreground colour */
    Pixel comment_bg;		    /* comment foreground colour */
    char *comment_underline;
    Pixel kw1_fg;		    /* kw1 foreground colour */
    Pixel kw1_bg;		    /* kw1 foreground colour */
    char *kw1_underline;
    Pixel kw2_fg;		    /* kw2 foreground colour */
    Pixel kw2_bg;		    /* kw2 foreground colour */
    char *kw2_underline;
    Pixel user1_fg;		    /* user 1 foreground colour */
    Pixel user1_bg;
    char *user1_underline;
    Pixel user2_fg;		    /* user 2 foreground colour */
    Pixel user2_bg;
    char *user2_underline;
    Pixel user3_fg;		    /* user 3 foreground colour */
    Pixel user3_bg;
    char *user3_underline;
    Pixel user4_fg;		    /* user 4 foreground colour */
    Pixel user4_bg;
    char *user4_underline;
    Pixel user5_fg;		    /* user 5 foreground colour */
    Pixel user5_bg;
    char *user5_underline;
    Pixel user6_fg;		    /* user 6 foreground colour */
    Pixel user6_bg;
    char *user6_underline;
    Pixel user7_fg;		    /* user 7 foreground colour */
    Pixel user7_bg;
    char *user7_underline;
    Pixel user8_fg;		    /* user 8 foreground colour */
    Pixel user8_bg;
    char *user8_underline;
    char *p_fg;			    /* Foreground for pointer */
    char *p_bg;			    /* Background for pointer */
    int p_shape;		    /* Pointer shape */
    };
static struct res resv;

static char emacs_class[] = "Emacs";

static XtResource resources[] =
    {
    {"pointerForeground", "PointerForeground", XtRString, sizeof (char *), XtOffset (struct res *, p_fg), XtRString, "white"},
    {"pointerBackground", "PointerBackground", XtRString, sizeof (char *), XtOffset (struct res *, p_bg), XtRString, "black"},
    {"pointerShape", "PointerShape", XtRInt, sizeof (int), XtOffset (struct res *, p_shape), XmRImmediate, (caddr_t) 0xFFFFFFFF},
    {"textFont", "Font", XtRString, sizeof(char *), XtOffset (struct res *, normal_font_name), XmRString, "-dec-terminal-medium-r-normal--14-*-*-*-c-80-iso8859-1" },
#ifdef BOLDFONT
    {"boldFont", "Font", XtRString, sizeof(char *), XtOffset (struct res *, bold_font_name), XmRString, "-dec-terminal-bold-r-normal--14-*-*-*-c-80-iso8859-1" },
#endif
    {"rows", "Rows", XtRInt, sizeof (int), XtOffset (struct res *, rows), XmRImmediate, (caddr_t) 40},
    {"columns", "Columns", XtRInt, sizeof (int), XtOffset (struct res *, columns), XmRImmediate, (caddr_t) 80},
    {"normalForeground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, n_fg), XmRString, XtDefaultForeground},
    {"normalBackground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, n_bg), XmRString, XtDefaultBackground},
    {"normalUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, n_underline), XmRString, "0" },
    {"modelineForeground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, ml_fg), XmRString, XtDefaultBackground},
    {"modelineBackground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, ml_bg), XmRString, XtDefaultForeground},
    {"modelineUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, ml_underline), XmRString, "0" },
    {"highlightForeground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, hl_fg), XmRString, XtDefaultBackground},
    {"highlightBackground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, hl_bg), XmRString, XtDefaultForeground},
    {"highlightUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, hl_underline), XmRString, "0" },
    {"cursorForeground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, c_fg), XmRString, XtDefaultBackground},
    {"cursorBackground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, c_bg), XmRString, XtDefaultForeground},
    {"wordForeground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, word_fg), XmRString, XtDefaultForeground},
    {"wordBackground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, word_bg), XmRString, XtDefaultBackground},
    {"wordUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, word_underline), XmRString, "0" },
    {"stringForeground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, string_fg), XmRString, XtDefaultForeground},
    {"stringBackground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, string_bg), XmRString, XtDefaultBackground},
    {"stringUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, string_underline), XmRString, "0" },
    {"commentForeground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, comment_fg), XmRString, XtDefaultForeground},
    {"commentBackground", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, comment_bg), XmRString, XtDefaultBackground},
    {"commentUnderline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, comment_underline), XmRString, "0" },
    {"keyword1Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, kw1_fg), XmRString, XtDefaultForeground},
    {"keyword1Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, kw1_bg), XmRString, XtDefaultBackground},
    {"keyword1Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, kw1_underline), XmRString, "0" },
    {"keyword2Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, kw2_fg), XmRString, XtDefaultForeground},
    {"keyword2Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, kw2_bg), XmRString, XtDefaultBackground},
    {"keyword2Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, kw2_underline), XmRString, "0" },
    {"user1Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user1_fg), XmRString, XtDefaultForeground},
    {"user1Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user1_bg), XmRString, XtDefaultBackground},
    {"user1Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user1_underline), XmRString, "0" },
    {"user2Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user2_fg), XmRString, XtDefaultForeground},
    {"user2Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user2_bg), XmRString, XtDefaultBackground},
    {"user2Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user2_underline), XmRString, "0" },
    {"user3Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user3_fg), XmRString, XtDefaultForeground},
    {"user3Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user3_bg), XmRString, XtDefaultBackground},
    {"user3Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user3_underline), XmRString, "0" },
    {"user4Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user4_fg), XmRString, XtDefaultForeground},
    {"user4Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user4_bg), XmRString, XtDefaultBackground},
    {"user4Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user4_underline), XmRString, "0" },
    {"user5Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user5_fg), XmRString, XtDefaultForeground},
    {"user5Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user5_bg), XmRString, XtDefaultBackground},
    {"user5Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user5_underline), XmRString, "0" },
    {"user6Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user6_fg), XmRString, XtDefaultForeground},
    {"user6Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user6_bg), XmRString, XtDefaultBackground},
    {"user6Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user6_underline), XmRString, "0" },
    {"user7Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user7_fg), XmRString, XtDefaultForeground},
    {"user7Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user7_bg), XmRString, XtDefaultBackground},
    {"user7Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user7_underline), XmRString, "0" },
    {"user8Foreground", XtCForeground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user8_fg), XmRString, XtDefaultForeground},
    {"user8Background", XtCBackground, XtRPixel, sizeof (Pixel), XtOffset (struct res *, user8_bg), XmRString, XtDefaultBackground},
    {"user8Underline", XtCString, XtRString, sizeof(char *), XtOffset (struct res *, user8_underline), XmRString, "0" }
    };

/*
 * External References:
 */

extern struct emacs_line *desired_screen[];
extern struct emacs_line *phys_screen[];
extern int curs_x, curs_y;

int xterm_deiconify ();
void xwin_menu_init (Widget);
/* Display, or remove the cursor */
static int ToggleCursor()
    {
    struct emacs_attr *attr;
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("ToggleCursor();"));
#endif

    if((VisibleX < 1) || (VisibleX > tt->t_width) || (VisibleY < 1) || (VisibleY > tt->t_length))
	{
	CursorExists = 0;
	return 0;
	}

    if (phys_screen[VisibleY] != NULL && VisibleX-1 < phys_screen[VisibleY]->line_length)
	{
	int last_attr = phys_screen[VisibleY]->line_attr[VisibleX-1];

	if( CursorExists )
	    {
	    if (last_attr & LINE_M_ATTR_HIGHLIGHT)
		attr = &attr_array[LINE_M_ATTR_HIGHLIGHT];
	    else if (last_attr & LINE_ATTR_MODELINE)
		attr = &attr_array[LINE_ATTR_MODELINE];
	    else if( last_attr&LINE_ATTR_USER )
		attr = &attr_array[last_attr&LINE_M_ATTR_USER];
	    else
		attr = &attr_array[last_attr];
	    }
	else
	    attr = &cu_attr;

	if( attr == NULL )
	    attr = &attr_array[SYNTAX_DULL];

	XDrawImageString (dpy, win, attr->gc, innerBorder +  (VisibleX - 1) * ftw, (VisibleY) * fth + innerBorder - ftd,
	    (char *)&phys_screen[VisibleY]->line_body[VisibleX-1], 1);
	if( attr->underline )
	    {
	    XDrawLine
	    (
	    dpy,win,attr->gc,
	    innerBorder + ((VisibleX - 1) * ftw), innerBorder + (VisibleY * fth) - ftd + 1,
	    innerBorder + (VisibleX * ftw), innerBorder + (VisibleY * fth) - ftd + 1
	    );
	    }
	}
    else
	{
	attr = CursorExists ? &rev_attr : &curev_attr;
	XFillRectangle (dpy, win, attr->gc, (VisibleX-1 ) * ftw + innerBorder,
	  (VisibleY-1 ) * fth + innerBorder, ftw, fth);
	}

    CursorExists = !CursorExists;
    return 1;
    }
/* Move to position row, col, i.e. put the cursor there. */
static
void topos (struct trmcontrol *PNOTUSED(tt), int row, int col)
    {
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("topos(%d, %d);"), row, col);
#endif

    curs_x = col;
    curs_y = row;
    if (InUpdate)
	{
	if (CursorExists)
	    ToggleCursor ();
	return;
	}

    if ((row == VisibleY) && (col == VisibleX))
	{
	if (!CursorExists)
	    ToggleCursor();
	}
    else if (CursorExists)
	{
	ToggleCursor ();
	VisibleX = col;
	VisibleY = row;
	ToggleCursor ();
	}
    else
	{
	VisibleX = col;
	VisibleY = row;
	ToggleCursor ();
	}
    }
/* Write a region to the screen */
static void writeregion (int PNOTUSED(width), int height, int PNOTUSED(x), int y)
    {
    register int start_line;
    register int end_line;
    register int row;

#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
        _dbg_msg (u_str("writeregion(%d, %d, %d, %d);"), width, height, x, y);
#endif
    /* 
     *	Convert pixels to char size
     */
    if (y < innerBorder)
	start_line = 1;
    else
	start_line = (y - innerBorder) / fth + 1;	

    if (y - innerBorder + height + fth - 1 < fth)
	end_line = 1;
    else
	end_line = (y - innerBorder + (height + fth - 1)) / fth + 1;

    if (end_line > tt->t_length)
	end_line = tt->t_length;

    for (row=start_line; row<=end_line; row++)
	xwin_update_line (tt,NULL,phys_screen[row], row);
    }
/* Wipe a line from the screen, setting it to a background */
static void wipe_line (struct trmcontrol *tt,int PNOTUSED(hlmod))
    {
    register int nCols;

#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("wipe_line(%d);"), hlmod);
#endif

    if((curs_x < 1) || (curs_x > tt->t_width) ||
      (curs_y < 1) || (curs_y > tt->t_length))
	return;

    nCols = tt->t_width - curs_x + 1;
    XClearArea (dpy, win, (curs_x - 1) * ftw + innerBorder,  (curs_y - 1) * fth + innerBorder, ftw * nCols, fth, 0);

    if ((curs_y == VisibleY) && (VisibleX >= curs_x))
	CursorExists = 0;
    topos(tt,curs_y, curs_x);
    }
/* Reset the entire screen to the backbround colour */
static void wipescreen (struct trmcontrol *PNOTUSED(tt))
    {
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("wipescreen();"));
#endif

    XClearWindow(dpy,win);
    CursorExists = 0;
    if(!InUpdate)
	ToggleCursor();
    }
/* Reset the entire screen to the backbround colour */
static void reset (struct trmcontrol *tt)
    {
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("reset(0x%0x8);"), tt);
#endif

    wipescreen(tt);
    }
/* Flash the screen */
static void flash(struct trmcontrol *PNOTUSED(tt))
    {
    struct emacs_attr tmp_array[256];
	
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("flash();"));
#endif

    tmp_array[SYNTAX_DULL] = attr_array[SYNTAX_DULL];
    tmp_array[SYNTAX_WORD] = attr_array[SYNTAX_WORD];
    tmp_array[SYNTAX_STRING] = attr_array[SYNTAX_STRING];
    tmp_array[SYNTAX_COMMENT] = attr_array[SYNTAX_COMMENT];
    tmp_array[SYNTAX_KEYWORD1] = attr_array[SYNTAX_KEYWORD1];
    tmp_array[SYNTAX_KEYWORD2] = attr_array[SYNTAX_KEYWORD2];

    attr_array[SYNTAX_DULL] = rev_attr;
    attr_array[SYNTAX_WORD] = rev_attr;
    attr_array[SYNTAX_STRING] = rev_attr;
    attr_array[SYNTAX_COMMENT] = rev_attr;
    attr_array[SYNTAX_KEYWORD1] = rev_attr;
    attr_array[SYNTAX_KEYWORD2] = rev_attr;

    writeregion(pixelWidth-innerBorder*2, pixelHeight-innerBorder*2,0,0);

    attr_array[SYNTAX_DULL] = tmp_array[SYNTAX_DULL];
    attr_array[SYNTAX_WORD] = tmp_array[SYNTAX_WORD];
    attr_array[SYNTAX_STRING] = tmp_array[SYNTAX_STRING];
    attr_array[SYNTAX_COMMENT] = tmp_array[SYNTAX_COMMENT];
    attr_array[SYNTAX_KEYWORD1] = tmp_array[SYNTAX_KEYWORD1];
    attr_array[SYNTAX_KEYWORD2] = tmp_array[SYNTAX_KEYWORD2];

    writeregion(pixelWidth-innerBorder*2, pixelHeight-innerBorder*2,0,0);	
    }
/* Begin an update */
static void update_begin(struct trmcontrol *PNOTUSED(tt))
    {	
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("update_begin();"));
#endif
 
    InUpdate = 1;
    SavedX = curs_x;
    SavedY = curs_y;
    if(CursorExists)
	ToggleCursor();
    }
/* Scan a buffer for the k'th occurrence of '\n',
 * starting at position n.
 */
int scan_nl(struct emacs_buffer *buf, int n, int k)
    {
    while (k != 0)
	{
	n--;
	do
	    {
	    n++;
	    if (n > (buf->b_size1 + buf->b_size2 - buf->b_mode.md_tailclip))
		return n;
	    }
	while ((n > buf->b_size1 ? (buf->b_base + buf->b_gap - 1)[n] : (buf->b_base - 1)[n]) != '\n');

	k--;
	if (k != 0)
	    n++;
	}

    return n + 1;
    }

/* End an update */
static void update_end(struct trmcontrol *tt)
    {	
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("update_end();"));
#endif

    InUpdate = 0;
    if(CursorExists)
	ToggleCursor();
    topos(tt,SavedY, SavedX);

    /*
     * Recalculate the scrollbar position, and size
     */

    if (wn_cur != NULL && wn_cur->w_buf != minibuf)
	{
	struct emacs_buffer *buf = wn_cur->w_buf;
	int bsz = buf->b_size1 + buf->b_size2 - buf->b_mode.md_tailclip;
/*	double estimated_lines = bsz / 35;
*/	int pos;
	int size;

/*	if (estimated_lines <= 0.0)
	    estimated_lines = 1.0;
*/
	if (bsz == 0)
	    {
	    pos = 1;
	    size = SB_SIZE;
	    }
	else
	    {
	    pos = (((wn_cur->w_start->m_pos - 1) * 100) / bsz) * (SB_SIZE / 100);
/*	    size = (int) (((double)((wn_cur->w_height - 1) * 100) / estimated_lines) * (double)(SB_SIZE / 100));
*/	    size = (int)((((double)(scan_nl (buf, wn_cur->w_start->m_pos, wn_cur->w_height - 1) - 1 - (wn_cur->w_start->m_pos - 1)) * 100.0) / ((double) bsz)) * (double)(SB_SIZE / 100));
	    }

	if (size < SB_SIZE / 100)
	    size = SB_SIZE / 100;

	XmScrollBarSetValues (vscroll, 
	    pos + 1, 
	    size,
	    10,
	    100,
	    0);   
	}

    XFlush(dpy);
    }
 
/* What sections of the window will be modified from the UpdateDisplay
 * routine is totally under software control.  Any line with Y coordinate
 * greater than flexlines will not change during an update.  This is really
 * used only during dellines and inslines routines.
 */
static void setflexlines (struct trmcontrol *tt,int n)
    {
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("setflexlines(%d);"), n);
#endif

    if((n <= 0) || (n > tt->t_length))
	flexlines = tt->t_length;
    else
	flexlines = n;
    }
/* Insert n blank lines below the current line. */ 
static void inslines (struct trmcontrol *PNOTUSED(tt),int n)
    {
    register int topregion, bottomregion;
    register int length, newtop;

#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("inslines(%d);"), n);
#endif
 
    if(curs_y > flexlines) 
	return;
    if(CursorExists) 
	ToggleCursor();
	    
    topregion = curs_y;
    bottomregion = flexlines - n;
    newtop = curs_y + n;
    length = (bottomregion - topregion) + 1;

    if((length > 0) && (newtop <= flexlines))
	XCopyArea(dpy, win, win, attr_array[SYNTAX_DULL].gc, 0, (topregion-1)* fth + innerBorder, pixelWidth, length * fth,
	  0, (newtop-1) * fth + innerBorder );

    newtop = min(newtop, flexlines);
    length = newtop - topregion;

    if(length > 0)
	XFillRectangle(dpy, win, rev_attr.gc, 0, (topregion-1) * fth + innerBorder, pixelWidth, n * fth);

    if(!InUpdate) 
	ToggleCursor();
    }
/* Delete n lines from the screen */
static void dellines (struct trmcontrol *PNOTUSED(tt),int n)
    {
#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("delines(%d);"), n);
#endif

    if(curs_y > flexlines) 
	return;
	    
    if(CursorExists) 
	ToggleCursor();
    if((curs_y + n) > flexlines)
	{
	if(flexlines > curs_y)
	    XFillRectangle(dpy, win, attr_array[SYNTAX_DULL].gc, 0, (curs_y-1) * fth + innerBorder, pixelWidth, 
	      (flexlines - curs_y) * fth);
	}
    else
	{
	XCopyArea(dpy, win, win, attr_array[SYNTAX_DULL].gc, 0, (curs_y + n -1 )* fth + innerBorder,
	  pixelWidth, (flexlines + 1 - (curs_y + n)) * fth, 0, (curs_y-1) * fth + innerBorder );

	XFillRectangle(dpy, win, rev_attr.gc, 0, (flexlines -n ) * fth + innerBorder, pixelWidth, n  * fth);
	}

    if(!InUpdate) 
	ToggleCursor();
    }
/* Update a single line of the display */
void xwin_update_line (struct trmcontrol *tt,struct emacs_line *old_line, struct emacs_line *new_line, int row)
    {
    int last_attr,start_col,end_col;

#if DBG_DISPLAY
    if (dbg_flags & DBG_DISPLAY)
	_dbg_msg (u_str("xwin_update_line(0x%08x, 0x%08x, %d);"), old_line, new_line, row);
#endif

    if (initializing)
	return;
 
    if (new_line == NULL)
	new_line = &empty_line;

    if (old_line == NULL)
	old_line = &empty_line;

    /* only write the line if its changed or there is no old_line line */
    if (old_line != NULL && new_line->line_length == old_line->line_length &&
      memcmp (new_line->line_body, old_line->line_body, new_line->line_length) == 0 &&
      memcmp (new_line->line_attr, old_line->line_attr, new_line->line_length) == 0 )
	return;

    /* fill out the line with spaces */
    memset (new_line->line_body + new_line->line_length, ' ', tt->t_width - new_line->line_length);
    /* and an attribute of normal */
    memset (new_line->line_attr + new_line->line_length, 0, tt->t_width - new_line->line_length);
	
    start_col = 0;
    end_col = 1;
    while (start_col < tt->t_width)
	{
	struct emacs_attr *attr;
	last_attr = new_line->line_attr[start_col];
	while (end_col < tt->t_width && last_attr == new_line->line_attr[end_col])
	    end_col++;

	if (last_attr & LINE_M_ATTR_HIGHLIGHT)
	    attr = &attr_array[LINE_M_ATTR_HIGHLIGHT];
	else if (last_attr & LINE_ATTR_MODELINE)
	    attr = &attr_array[LINE_ATTR_MODELINE];
	else if( last_attr&LINE_ATTR_USER )
	    attr = &attr_array[last_attr&LINE_M_ATTR_USER];
	else
	    attr = &attr_array[last_attr];
	if( attr == NULL )
	    attr = &attr_array[SYNTAX_DULL];

	XDrawImageString(dpy,win,attr->gc, innerBorder + (start_col * ftw), innerBorder + (row * fth) - ftd, 
		(char *)&new_line->line_body[start_col], end_col - start_col);
	if( attr->underline )
	    {
	    XDrawLine
	    (
	    dpy,win,attr->gc,
	    innerBorder + (start_col * ftw), innerBorder + (row * fth) - ftd + 1,
	    innerBorder + (end_col * ftw), innerBorder + (row * fth) - ftd + 1
	    );
	    }

	start_col = end_col;
	end_col++;
	}
    }
/* Ring the bell */
static void beep(struct trmcontrol *PNOTUSED(tt))
    {
    XBell(dpy,0); 
    }

/* read an event from X-Windows */

#define	TIMER_TICK_VALUE 50
static unsigned int due_tick_count;
static void (*timeout_handler)(void);
struct timeval timeout_time;

void time_schedule_timeout( void (*time_handle_timeout)(void), int delta )
	{
	struct timezone tzp;
	gettimeofday( &timeout_time, &tzp );

	timeout_time.tv_sec += delta/1000;
	timeout_time.tv_usec += (delta%1000)*1000;
	if( timeout_time.tv_usec > 1000000 )
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

static XtIntervalId timer_id = 0;

void xwin_timeout_callback(XtPointer PNOTUSED(p), XtIntervalId *PNOTUSED(id) )
	{
	timer_id = 0;
	due_tick_count--;
	}

static void timeout_check()
	{
	if( timeout_handler != NULL )
		{
		/* if the timeout has occurred */
		struct timeval now;
		struct timezone tzp;

		gettimeofday( &now, &tzp );

		if( now.tv_sec > timeout_time.tv_sec
		|| (now.tv_sec == timeout_time.tv_sec && now.tv_usec > timeout_time.tv_usec ))
		/*	due_tick_count <= 0 ) */
			{
			void (*handler)(void) = timeout_handler;
			timeout_handler = NULL;
			handler();
			}
		}
	}

int xwin_readsocket( struct trmcontrol *PNOTUSED(tt), unsigned char *PNOTUSED(bufp), unsigned int PNOTUSED(numchars))
	{
	/* set a timeout on the event processing of 50mS */
	if( timer_id == 0 && timeout_handler != NULL )
		timer_id = XtAppAddTimeOut( app_context, TIMER_TICK_VALUE, xwin_timeout_callback, NULL );
	XtAppProcessEvent (app_context, XtIMAll);

	if( child_changed )
		change_msgs ();

	timeout_check();

	return 0;
	}
/* Set the X-Windows pointer according to the emacs activity state */
void xwin_activity (unsigned char PNOTUSED(ch))
    {
#ifdef ACTIVITY
    XSetWindowAttributes xswa;

    if (cursors)
	{
	switch (ch)
	    {
	    case 'b': case 'j': case 'c':
		xswa.cursor = watchCursor; 
		break;

	    default:
		xswa.cursor = arrowCursor; 
		break;
	    }

	XChangeWindowAttributes(dpy, win, CWCursor, &xswa);
	}
#endif
    }
/*
 * Input functions
 */

/* Feed a string into Emacs */
void input_char_string (char *keys)
    {
    int ch;

    while ((ch = *keys++ ) != 0)
	{
	if (ch == '\200')
	    ch = *keys++ - '\200';
	keyboard_interrupt (tt,ch);
	}
    }
/* Process a key stroke */
#ifdef DXK_Remove
#define is_special(a) (a == DXK_Remove)
#elif defined( KEYBOARD_TYPE_NEW_HP )
#define is_special(a) (a == XK_BackSpace || a == XK_Num_Lock || a == XK_Scroll_Lock || a == XK_Pause)
#elif defined( KEYBOARD_TYPE_OLD_HP )
#define is_special(a) (a == hpXK_ClearLine || a == XK_Clear || a == XK_BackSpace)
#else
#define is_special(a) (a == XK_BackSpace)
#endif

static void Input (XKeyEvent *event)
    {
    char keybuf[10];
    int nChar,i;
    KeySym k;
    static XComposeStatus compose_status; /* Rely on default initialisation to 0 */

    k  = XLookupKeysym(event,0);
#if DBG_KEY
    if( dbg_flags&DBG_KEY )
	printf("Input: keycode %d state 0x%.4x keysym 0x%.4x ",
		event->keycode, event->state, k); 
#endif
#ifdef KEYBOARD_TYPE_PC
    if( k == 0 && event->keycode == 84 )
	k = XK_KP_5;
#endif
    if(IsCursorKey(k)
    || IsPFKey(k)
    || IsKeypadKey(k)
    || IsFunctionKey(k)
    || IsMiscFunctionKey(k)
    || is_special (k))
	{
	for (i = 0; i < sizeof (convert) / sizeof (struct conv_keys); i++)
	    {
	    if (k == convert[i].key_code)
		{
		char *p;
		p = event->state & ShiftMask ? convert[i].shift_translation :
		    event->state & ControlMask ? convert[i].ctrl_translation :
		    event->state & Mod1Mask ? convert[i].enh_translation :
		    convert[i].translation;
		if (p == NULL)
		    p = convert[i].translation;
#if DBG_KEY
		if( dbg_flags&DBG_KEY )
		    printf("converted to ESC-%s ", &p[1] ); 
#endif
		input_char_string (p);
		break;
		}
	    }
#if DBG_KEY
	if( dbg_flags&DBG_KEY )
	    printf("\n");
#endif
	}
    else	
	{
	
	nChar = XLookupString(event, keybuf, sizeof (keybuf), NULL, &compose_status);
	keybuf[nChar] = '\0';
#if DBG_KEY
	if( dbg_flags&DBG_KEY )
	    printf("is %d char 0x%.2x\n", nChar, keybuf[0]);
#endif
	if( nChar > 0 )
		keyboard_interrupt(tt,keybuf[0]);
	}
   }
/* Process a mouse click */
static void mouseInput(XButtonEvent *event)
    {
    char mouse[64];
    register int line,column; 
    register int button;

    if( !mouse_enable )
	return;

    /* Calculate character cell position */
    column = (event->x - innerBorder) / ftw + 1;
    line = (event->y - innerBorder) / fth + 1;

    switch (event->button)
	{
	case Button1: 
	    button = event->type == ButtonPress ? 2 : 3;
	    break;
	case Button2: 
	    button = event->type == ButtonPress ? 4 : 5;
	    break;
	case Button3:
	    button = event->type == ButtonPress ? 6 : 7;
	    break;
	default:
	    _dbg_msg (u_str("Unexpected mouse button. event->button: %x"),event->button);
	    break;
	}

    sprintfl( u_str(mouse), sizeof(mouse), u_str("\x1b[%d;%d;%d;%d&w"), button, event->state, line, column );
    input_char_string( mouse );
    }
/* Handle a keyboard or mouse input event from the drawing area widget */ 
static void input_cb (Widget PNOTUSED(w), void *PNOTUSED(junk), void *event_)
    {
    XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;
    extern int motif_insensitive;

    if (motif_insensitive)
	return;

    switch (event->event->type)
	{
	case KeyPress: 
	    Input(&event->event->xkey);
	    break;

	case KeyRelease:
	    break;

	case ButtonPress:
	case ButtonRelease:
	    mouseInput(&event->event->xbutton);
	    break;

	default:
	    _dbg_msg (u_str("Unknown input event = %d"), event->event->type);
	    break;
	}

    if (screen_garbaged)
	{
	do_dsp(0);
	screen_garbaged = 0;
	}
    }	

static void mouse_motion_handler
	(
	Widget PNOTUSED(w),
	XtPointer PNOTUSED(data),
	XEvent *event_,
	Boolean *PNOTUSED(continue_to_dispatch)
	)
    {
    XMotionEvent *event = (XMotionEvent *)event_;
    char mouse[64];
    int line, column; 

    if( (mouse_enable&MOUSE_MOTION_EVENTS) == 0 )
	return;

    /* Calculate character cell position */
    column = (event->x - innerBorder) / ftw + 1;
    line = (event->y - innerBorder) / fth + 1;

    sprintfl( u_str(mouse), sizeof(mouse), u_str("\x1b[%d;%d;%d;%d&W"), 0, event->state, line, column );
    input_char_string( mouse );
    }

/* Handle an exposure event from the drawing area widget */
static void exposure_cb (Widget PNOTUSED(w), void * PNOTUSED(junk), void *event_)
    {
    XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;
    initializing = 0;
    switch (event->event->type)
	{
	case NoExpose:
	    break;

	case GraphicsExpose:
	case Expose:
	    if (motif_iconic)
		xterm_deiconify ();
	    writeregion(event->event->xexpose.width,event->event->xexpose.height,event->event->xexpose.x, event->event->xexpose.y);
	    break;

	default:
	    _dbg_msg (u_str("Unknown exposure event = %d"), event->event->type);
	    break;
	}

    if (screen_garbaged)
	{
	do_dsp(0);
	screen_garbaged = 0;
	}
    }	
/* Handle a resize event from the drawing area widget */
static void resize_cb (Widget w, void *PNOTUSED(junk), void *PNOTUSED(event_))
    {
    int height;
    int width;

    width  = w->core.width;
    height = w->core.height;

    if (height != pixelHeight || width != pixelWidth)
	{
	int old_length;
	int old_width;
	int newHeight, newWidth;

	old_length = tt->t_length;
	old_width = tt->t_width;

	newHeight = (height - innerBorder * 2 ) / fth;
	newWidth = (width - innerBorder * 2 ) / ftw;
	pixelWidth = newWidth * ftw + innerBorder * 2;
	term_width = tt->t_width = newWidth;
	term_length = tt->t_length = newHeight;
	flexlines = tt->t_length - 1;
	if (wn_cur != NULL)
	    {
	    if(tt->t_length != old_length || tt->t_width != old_width)
		{
		fit_windows_to_screen_length();
		screen_garbaged = 1;
		if( attr_array[SYNTAX_DULL].gc != NULL )
			do_dsp(1);
		}
	    }
	}
    }	
/*
 * Empty functions required by the terminal handler
 */
static void null_int_routine (struct trmcontrol *PNOTUSED(tt),int PNOTUSED(mode))
    {
    return;
    }

static void null_void_routine (struct trmcontrol *PNOTUSED(tt))
    {
    return;
    }

static void null_select (struct trmcontrol *PNOTUSED(tt), unsigned char *PNOTUSED(a), unsigned char *PNOTUSED(b), unsigned char *PNOTUSED(c))
    {
    return;
    }

static void null_tt_routine( struct trmcontrol *PNOTUSED(tt) )
    {
    return;
    }

int motif_not_running ()
    {
    error (u_str ("Not running in an X-Windows/MOTIF environment"));
    return 0;
    }   

static void motif_err_handler (String PNOTUSED(msg))
    {
    return;
    }

static void one_line_cb (Widget PNOTUSED(w), void * direction, void *PNOTUSED(event_))
    {
    if (wn_cur && wn_cur->w_buf != minibuf)
	{
	window_move (wn_cur, *(int *)direction, 0, 0);
	do_dsp (0);
	}
    }

static void page_cb (Widget PNOTUSED(w), void * direction, void *PNOTUSED(event))
    {
    if (wn_cur && wn_cur->w_buf != minibuf)
	{
	window_move (wn_cur, *(int *)direction, 1, 0);
	do_dsp (0);
	}
    }

static void drag_cb (Widget PNOTUSED(w), void * PNOTUSED(direction), void *event_)
    {
    XmScrollBarCallbackStruct *event = (XmScrollBarCallbackStruct *)event_;
    if (wn_cur && wn_cur->w_buf != minibuf && wn_cur->w_start)
	{
	struct emacs_buffer *buf = wn_cur->w_buf;
	int pos;

	pos = (int) (((double)((buf->b_size1 + buf->b_size2)) * (double)(event->value)) / ((double) SB_SIZE));
	set_mark (wn_cur->w_start, buf, pos, 0);
	cant_1line_opt = 1;
	wn_cur->w_force++;
	do_dsp (0);
	}
    }
/*
 * Widget manipulate fuinctions
 */

static void create__attr( struct emacs_attr *attr, XGCValues *gcv, Pixel fg, Pixel bg, char *underline )
    {
    long int bits = GCFont | GCForeground | GCBackground | GCFillStyle| GCPlaneMask;

    gcv->foreground =  fg;
    gcv->background =  bg;

    if( underline != NULL )
	{
	int num_parsed;
	char keyword[80];
	int dash_value;

	keyword[0] = '\0';
	num_parsed = sscanf( underline,"%s %d", keyword, &dash_value );

	if( strcmp("1",keyword) == 0
	|| strcmp("true",keyword) == 0
	|| strcmp("yes",keyword) == 0 )
	    {
	    attr->underline = 1;
	    }
	else if( strcmp("dash",keyword) == 0 )
	    {
	    bits |= GCLineStyle;
	    attr->underline = 1;
	    gcv->line_style = LineOnOffDash;
	    if( num_parsed > 1 && dash_value > 0)
		{
		bits |= GCDashList;
		gcv->dashes = (char)dash_value;
		}
	    }
	}
	
    attr->gc = XCreateGC
	    (
	    dpy, win,
	    bits,
	    gcv
	    );
    }


/* Realise the emacs widgets */
static int create_windows (char *displayName)
    {
    Pixmap iconPixmap;
    XSetWindowAttributes xswa;
    XGCValues gcv;
    XFontStruct *normalFontStruct, *pointerFontStruct; 
    Arg args[10];
    unsigned int XXheight = 24, XXwidth = 80;

    XtToolkitInitialize ();
    app_context = XtCreateApplicationContext ();
    if (!(dpy = XtOpenDisplay (app_context, NULL, (char *)gargv[0], emacs_class, NULL, 0, &gargc, (char **)gargv)))
	{
	_dbg_msg (u_str("Can't open X server at %s"),displayName);
	return 0;
	}    
    dpy_fd = ConnectionNumber (dpy);
    app_shell = XtAppCreateShell ((char *)gargv[0], emacs_class, applicationShellWidgetClass, dpy, NULL, 0);

    XtGetApplicationResources(app_shell, (XtPointer)&resv, resources, XtNumber(resources), NULL, 0);
    XtAppSetWarningHandler (app_context, &motif_err_handler);

    /* Deal with the pointer shape */
    if ((pointerFontStruct = XLoadQueryFont (dpy, "decw$cursor")) == NULL)
	_dbg_msg (u_str("Display %s is unable to open font: decw$cursor"), DisplayString(dpy));
    else
	{
	if (resv.p_shape != 0xFFFFFFFF)
	    {
	    XColor fgColor, bgColor;
	    if (resv.p_shape & 1)
		resv.p_shape &= ~1;
	    XParseColor(dpy, DefaultColormap (dpy, DefaultScreen(dpy)), resv.p_fg, &fgColor);
	    XParseColor(dpy, DefaultColormap (dpy, DefaultScreen(dpy)), resv.p_bg, &bgColor);
	    arrowCursor = XCreateGlyphCursor (dpy, pointerFontStruct->fid, pointerFontStruct->fid, 
		resv.p_shape, resv.p_shape + 1, &fgColor, &bgColor);
	    cursors++;
	    }
#ifdef ACTIVITY
	watchCursor = XCreateGlyphCursor (dpy, pointerFontStruct->fid, pointerFontStruct->fid, 4, 5, &fgColor, &bgColor);
#endif
	}

    /* Fetch the fonts and calculate their sizes */
    if ((normalFontStruct = XLoadQueryFont (dpy, resv.normal_font_name)) == NULL)
	{
	_dbg_msg (u_str("Display %s is unable to open font: %s"), DisplayString(dpy), resv.normal_font_name);
	return 0;
	}

#ifdef BOLDFONT
    if ((boldFontStruct = XLoadQueryFont (dpy, resv.bold_font_name)) == NULL)
	{
	_dbg_msg (u_str("Display %s is unable to open bold font: %s"), DisplayString(dpy), resv.bold_font_name);
	boldFontStruct = normalFontStruct;
	}
#endif

    fth = normalFontStruct->ascent + normalFontStruct->descent;
    ftw = normalFontStruct->max_bounds.width;
    ftd = normalFontStruct->descent;
    innerBorder = 2;

    /* Set application's window manager resources */
    XtSetArg (args[0], XmNheightInc, fth);
    XtSetArg (args[1], XmNwidthInc, ftw);
    XtSetArg (args[2], XmNminHeight, fth * 10);
    XtSetArg (args[3], XmNminWidth, ftw * 30);
    XtSetArg (args[4], XmNmaxHeight, min( fth * MSCREENLENGTH, XDisplayHeight(dpy,0)-10 ));
    XtSetArg (args[5], XmNmaxWidth, min( ftw * MSCREENWIDTH, XDisplayWidth(dpy,0)-10 ));
    XtSetValues (app_shell, args, 6);

    UI_update_window_title();

    main_window = XmCreateMainWindow (app_shell, "shell", args, 0);
    XtManageChild (main_window);

    /* Create the menus */
    menu_bar = XmCreateMenuBar (main_window, "menuBar", args, 0); 
    XtManageChild (menu_bar);
    xwin_menu_init (menu_bar);

    /* Createthe frame and drawing area */
    XtSetArg (args[0], XmNbackground, resv.n_bg);
    frame = XmCreateFrame (main_window, "frame", args, 1);
    XtManageChild (frame);

    tt->t_length = XXheight = resv.rows;
    tt->t_width = XXwidth = resv.columns;

    pixelWidth = XXwidth * ftw + innerBorder * 2;
    pixelHeight = XXheight * fth + innerBorder * 2;

    XtSetArg (args[0], XmNwidth, pixelWidth);
    XtSetArg (args[1], XmNheight, pixelHeight);
    XtSetArg (args[2], XmNbackground, resv.n_bg);
    drawing_area =  XmCreateDrawingArea (frame, "screen", args, 3);

    XtManageChild (drawing_area);
    XtAddCallback (drawing_area, XmNexposeCallback, exposure_cb, NULL);
    XtAddCallback (drawing_area, XmNinputCallback, input_cb, NULL);
    XtAddCallback (drawing_area, XmNresizeCallback, resize_cb, NULL);

    /* Create the main verticle scroll bar */
    XtSetArg (args[0], XmNmaximum, SB_SIZE);
    vscroll = XmCreateScrollBar (main_window, "vScroll", args, 1);
    XtManageChild (vscroll);
    XtAddCallback (vscroll, XmNincrementCallback, one_line_cb, &number_zero);
    XtAddCallback (vscroll, XmNdecrementCallback, one_line_cb, &number_one);
    XtAddCallback (vscroll, XmNpageIncrementCallback, page_cb, &number_zero);
    XtAddCallback (vscroll, XmNpageDecrementCallback, page_cb, &number_one);
    XtAddCallback (vscroll, XmNdragCallback, drag_cb, &number_one);

    /* Set MainWindow areas and add tab groups */
    XmMainWindowSetAreas (main_window, menu_bar, NULL, NULL, vscroll, frame);

    /* realize the widgets */
    XtRealizeWidget (app_shell);

    /* Create the graphic contexts for drawing in the window */
    win = XtWindow (drawing_area);

    gcv.fill_style = FillSolid;
    gcv.plane_mask = AllPlanes;
    gcv.font = normalFontStruct->fid;
    gcv.line_style = LineSolid;

#define create_gc( gc, fg, bg ) \
    gcv.foreground =  resv.fg; \
    gcv.background =  resv.bg; \
    gc = XCreateGC (dpy, win, (GCFont | GCForeground | GCBackground | GCFillStyle| GCPlaneMask), &gcv)

#define create_attr( attr, name )  \
	create__attr( &attr, &gcv, resv.name##_fg, resv.name##_bg, resv.name##_underline );

    create_gc( rev_attr.gc, n_bg, n_fg );
    create_gc( cu_attr.gc, c_fg, c_bg );
    create_gc( curev_attr.gc, c_bg, c_fg );
    create_attr( attr_array[SYNTAX_DULL], n );
    create_attr( attr_array[LINE_M_ATTR_HIGHLIGHT], hl );
    create_attr( attr_array[SYNTAX_WORD], word );
    create_attr( attr_array[SYNTAX_STRING], string );
    create_attr( attr_array[SYNTAX_COMMENT], comment );
    create_attr( attr_array[SYNTAX_KEYWORD1], kw1 );
    create_attr( attr_array[SYNTAX_KEYWORD2], kw2 );
    create_attr( attr_array[LINE_ATTR_MODELINE], ml );
    create_attr( attr_array[LINE_ATTR_USER+1], user1 );
    create_attr( attr_array[LINE_ATTR_USER+2], user2 );
    create_attr( attr_array[LINE_ATTR_USER+3], user3 );
    create_attr( attr_array[LINE_ATTR_USER+4], user4 );
    create_attr( attr_array[LINE_ATTR_USER+5], user5 );
    create_attr( attr_array[LINE_ATTR_USER+6], user6 );
    create_attr( attr_array[LINE_ATTR_USER+7], user7 );
    create_attr( attr_array[LINE_ATTR_USER+8], user8 );
#undef create_gc
#undef create_attr

    /* Set up the pointer */
    xswa.cursor = arrowCursor; 
    XChangeWindowAttributes(dpy, win, CWCursor, &xswa);

    /* Set application's window manager resources */
    iconPixmap = XCreateBitmapFromData(dpy, XtWindow (app_shell), (char *)icon_bits, icon_width, icon_height);
    if (iconPixmap != None)
	{
	XtSetArg (args[0], XmNiconPixmap, iconPixmap);
	XtSetValues (app_shell, args, 1);
	}

    ToggleCursor();

    return 1;
    }

static int current_mouse_enable = 0;
static void change_attribute( struct trmcontrol *PNOTUSED(tt) )
    {
    /* trim to the valid mouse modes */
    mouse_enable &= MOUSE_BUTTON_EVENTS|MOUSE_MOTION_EVENTS;

    /* force MOUSE_MOTION_EVENTS off if MOUSE_BUTTON_EVENTS is not enabled */
    if( (mouse_enable&MOUSE_BUTTON_EVENTS) == 0 )
	mouse_enable = 0;

    /* see if anything has changed */
    if( current_mouse_enable == mouse_enable )
	return;

    /* if MOUSE_MOTION_EVENTS where enabled and are not required now turn them off */
    if( (current_mouse_enable&MOUSE_MOTION_EVENTS) != 0
    && (mouse_enable&MOUSE_MOTION_EVENTS) == 0 )
	XtRemoveEventHandler( drawing_area, ButtonMotionMask, False, mouse_motion_handler, NULL );

    /* if MOUSE_MOTION_EVENTS are required now trun them on */
    if( (mouse_enable&MOUSE_MOTION_EVENTS) != 0 )
	XtAddEventHandler( drawing_area, ButtonMotionMask, False, mouse_motion_handler, NULL );

    /* save the enable state */
    current_mouse_enable = mouse_enable;
    }

void xwin_check_for_input( struct trmcontrol *tt )
	{
	unsigned char buf[128];
	int i;
	int size;

	while( XPending(dpy) )
		{
		size = tt->k_input_event( tt, buf, sizeof( buf ) );
		if( size >= 1 )
			{
			for( i=0; i<size; i++ )
				tt->k_input_char( tt, buf[i] );
			}
		}

	timeout_check();
	}

void xwin_printf( struct trmcontrol *PNOTUSED(tt), unsigned char *fmt, ... )
	{
	unsigned char buf[128];
	int i;
	va_list argp;

	va_start( argp, fmt );

	i = do_print( fmt, &argp, buf, sizeof( buf ) );

	_dbg_msg( u_str("%s"), buf );
	}

void xwin_display_activity( struct trmcontrol *PNOTUSED(tt), unsigned char PNOTUSED(it) )
	{
	/* need to write the char placing code here */
	return;
	}

/* Initialization function */
int init_gui_terminal(struct trmcontrol *tt, unsigned char *displayName)
    {
    if( tt->t_version != TT_STRUCT_VERSION )
	{
	_dbg_msg (u_str("TT_STRUCT_VERSION is %d and should be %d"), tt->t_version, TT_STRUCT_VERSION);
	return 0;
	}

    tt->t_topos = topos;
    tt->t_reset = reset;
    tt->t_insert_mode = null_int_routine;
    tt->t_highlight_mode = null_int_routine;
    tt->t_inslines = inslines;	
    tt->t_dellines = dellines; 
    tt->t_blanks = null_int_routine;
    tt->t_init = null_tt_routine;
    tt->t_cleanup = null_tt_routine;
    tt->t_wipe_line = wipe_line;
    tt->t_wipe_screen = wipescreen;
    tt->t_delete_chars = null_int_routine;	
    tt->t_write_chars = NULL; /* writechars; */
    tt->t_window = setflexlines;
    tt->t_change_attributes = change_attribute;
    tt->t_flash = flash;
    tt->t_update_begin = update_begin;
    tt->t_update_line = xwin_update_line;
    tt->t_update_end = update_end;
    tt->t_select = null_select;
    tt->t_beep = beep;
    tt->t_width = 80;
    tt->t_length = 24;
    tt->t_io_flush = null_void_routine;
    tt->t_io_printf = xwin_printf;
    tt->t_display_activity = xwin_display_activity;

    tt->t_il_mf = 1;
    tt->t_il_ov = 1;
    tt->t_ic_ov = MISSINGFEATURE;
    tt->t_dc_ov = MISSINGFEATURE;
    tt->t_baud_rate = 1000000;
    tt->k_input_event = xwin_readsocket;
    tt->k_check_for_input = xwin_check_for_input;

    term_app_keypad=1;
    term_edit=1;
    term_eightbit=1;

    CursorExists = 0;
    InUpdate = 0;
    VisibleX = 1;
    VisibleY = 1;
    flexlines = tt->t_length -2;
    is_motif = 1;

    return create_windows( s_str(displayName) );
}

void UI_update_window_title(void)
	{
	Arg args[2];
	char title[128];
	char *home = getenv("HOME");
	char *cwd = s_str(current_directory);

	if( app_shell == NULL )
		return;

	strcpy( title, "Barry's Emacs - " );
	if( strncmp( home, cwd, strlen( home ) ) == 0 )
		{
		strcat( title, "~/" );
		cwd += strlen(home);
		if( *cwd == '/' )
			cwd++;
		strcat( title, cwd );
		}
	else
		strcat( title, s_str(current_directory) );

	XtSetArg (args[0], XmNtitle, &title);
	XtSetArg (args[1], XmNiconName, &title);

	XtSetValues (app_shell, args, 2);
	}
#endif
