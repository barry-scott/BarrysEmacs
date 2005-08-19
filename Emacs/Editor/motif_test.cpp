#include <emacsutl.h>
#include <stdlib.h>
#include <string.h>
#include <emobject.h>
#include <emstring.h>

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
#include <Xm/RowColumnP.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/ScrollBar.h>
#include <Xm/MessageB.h>
#include <Xm/ToggleB.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/LabelG.h>
#include <Xm/CascadeBG.h>

#define write_scrolls_width 38
#define write_scrolls_height 38
#define write_scrolls_x_hot 0
#define write_scrolls_y_hot 0

const int MAX_CHILDREN( 100 );
const int MAX_LABEL( 100 );

void error( const EmacsString &msg )
	{
	_dbg_msg( msg );
	}

int vertical_bar_width = 2;

static unsigned char write_scrolls_bits[] =
   {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xff, 0x03,
   0x00, 0x00, 0x02, 0x00, 0x05, 0x00, 0x00, 0x01, 0x80, 0x09, 0x00, 0x80,
   0x00, 0xc0, 0xff, 0x00, 0x40, 0x00, 0x40, 0x20, 0x01, 0x50, 0x00, 0x40,
   0x10, 0x02, 0x78, 0x00, 0x40, 0xf0, 0x03, 0x50, 0x00, 0x40, 0x10, 0x00,
   0xa0, 0x00, 0x40, 0x10, 0x00, 0x40, 0x01, 0x40, 0x10, 0x00, 0xc0, 0x02,
   0x40, 0x10, 0x00, 0x40, 0x05, 0x40, 0x10, 0x00, 0x40, 0x0a, 0x40, 0x10,
   0x00, 0x40, 0x14, 0x40, 0x10, 0x00, 0x40, 0x28, 0x40, 0x10, 0x00, 0x40,
   0x30, 0x40, 0x10, 0x00, 0x40, 0x00, 0x40, 0x30, 0x00, 0x40, 0x00, 0x40,
   0x70, 0x00, 0x40, 0x00, 0x40, 0xf0, 0x07, 0x40, 0x00, 0x40, 0xf0, 0x07,
   0x40, 0x00, 0x40, 0xf0, 0x03, 0x40, 0x00, 0x40, 0xf0, 0x01, 0x40, 0x00,
   0x40, 0xf0, 0x00, 0x40, 0x00, 0x40, 0x70, 0x00, 0x20, 0x00, 0x40, 0x30,
   0x00, 0x18, 0x00, 0x20, 0x10, 0x00, 0xf0, 0xff, 0x3f, 0x18, 0x00, 0x80,
   0x00, 0x00, 0x0c, 0x00, 0x00, 0xff, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
   };

typedef int EmacsWindow;
typedef int EmacsLine;

const int MSCREENLENGTH( 50 );
const int MSCREENWIDTH( 200 );

int dbg_flags = DBG_DISPLAY;
int gargc;
unsigned char **gargv;

int number_zero(0);
int number_one(1);

//
//	Feature switches
//
#define TOOL_BAR_PRESENT 1
#define STATUS_BAR_PRESENT 1

//
//	Friendly way to handle the Args to Xt intensic functions
//
class EmacsXtArgs
	{
public:
	enum { max_args = 20 };
	EmacsXtArgs()
		: num_used(0)
		{ }
	~EmacsXtArgs()
		{ }

	void init()
		{ num_used = 0; }

	void setArg( String name, XtArgVal value )
		{
		emacs_assert( num_used < max_args );
		XtSetArg( args[num_used], name, value );
		num_used++;
		}		

	void setArg( String name, Widget value )
		{
		emacs_assert( num_used < max_args );
		XtSetArg( args[num_used], name, value );
		num_used++;
		}		

	void setArg( String name, XtPointer value )
		{
		emacs_assert( num_used < max_args );
		XtSetArg( args[num_used], name, value );
		num_used++;
		}		

	operator Arg *() { return args; }
	operator int()
		{
		int num = num_used;
		init();
		return num;
		}

	void setValues( Widget w )
		{
		XtSetValues( w, args, num_used );
		init();
		}
	void getValues( Widget w )
		{
		XtGetValues( w, args, num_used );
		init();
		}
private:
	int num_used;
	Arg args[max_args];
	};

//======================================================================//
//									//
//	Emacs MENU class						//
//									//
//======================================================================//
class EmacsMotifMenuButton;
class EmacsMotif_ActionItem
	{
public:
	EmacsMotif_ActionItem( const EmacsString &_keys ) : keys(_keys){}
	EmacsMotif_ActionItem( const EmacsMotif_ActionItem &in ) : keys( in.keys) {}
	EmacsMotif_ActionItem( void (*_callback)(Widget) ) {}
	EmacsMotif_ActionItem( Widget button, bool emulate_toggle ) {}
	EmacsMotif_ActionItem() {}
	~EmacsMotif_ActionItem() {}
	EmacsMotif_ActionItem &operator=( const EmacsMotif_ActionItem &in ) {}
	void queueAction() {}
	void setButton( Widget _button, bool _emulate_toggle )
		{
		button = _button;
		emulate_toggle = _emulate_toggle;
		}

	int doAction();
	EmacsString displayAction();
	bool isSeparator() const;
	bool isToggleButton() const { return true; }
	void setState();
private:
	EmacsString keys;
	void (*callback)(Widget);
	Widget button;
	bool emulate_toggle;		// true if the button is a toggle
	};

class EmacsMotifMenu
	{
public:
	EmacsMotifMenu( const EmacsString &name, int pos );
	virtual ~EmacsMotifMenu();
	EmacsMotifMenu *next;		// Chain of items
	EmacsString name;		// Name of the button
	int pos;			// menu order information
	Widget button;			// The button widget

	// dump the menu details
	virtual void dump( int level ) = 0;
	// update the state of a menu
	virtual void updateState();
	// make a button
	virtual EmacsMotifMenu *makeButton( const EmacsString &button_name, int pos, int mn,
				const EmacsMotif_ActionItem &action );
	// make a cascade
	virtual EmacsMotifMenu *makeCascade( const EmacsString &cascade_name, int pos, int mn );
	// make a separator
	virtual EmacsMotifMenu *makeSeparator( const EmacsString &cascade_name, int pos );

	virtual void removeMenuItem( int index );
	virtual void addMenuItem( const EmacsMotif_ActionItem &action, int index );
	};

class EmacsMotifMenuSeparator : public EmacsMotifMenu
	{
public:
	EmacsMotifMenuSeparator( const EmacsString &name, int pos );
	virtual ~EmacsMotifMenuSeparator();
	virtual void dump( int level );
	};

class EmacsMotifMenuButton : public EmacsMotifMenu
	{
public:
	EmacsMotifMenuButton( const EmacsString &name, int pos );
	virtual ~EmacsMotifMenuButton();

	EmacsMotif_ActionItem action;		// action to be used when button is pressed

	virtual void dump( int level );
	virtual void updateState();
	};

class EmacsMotifMenuCascade : public EmacsMotifMenu
	{
public:
	EmacsMotifMenuCascade( const EmacsString &name, int pos );
	virtual ~EmacsMotifMenuCascade();

	EmacsMotifMenu *children;	// Chain of items
	Widget menu;			// Menu pane
	int ccount;			// Count of the number of children present

	// make a button
	virtual EmacsMotifMenu *makeButton( const EmacsString &button_name, int pos, int mn,
				const EmacsMotif_ActionItem &action );
	// make a cascade
	virtual EmacsMotifMenu *makeCascade( const EmacsString &cascade_name, int pos, int mn );
	// make a separator
	virtual EmacsMotifMenu *makeSeparator( const EmacsString &cascade_name, int pos );

	virtual void removeMenuItem( int index );
	virtual void addMenuItem( const EmacsMotif_ActionItem &action, int index );

	virtual void dump( int level );
	virtual void updateState();
	};

//======================================================================//
//									//
//	Encapsulate the GCs that Emacs needs				//
//									//
//======================================================================//
class EmacsMotif_Application;

class EmacsMotif_Attributes
	{
public:
	EmacsMotif_Attributes()
		: gc(NULL)
		, special_gc(NULL)
		, underline(0)
		{ }
	~EmacsMotif_Attributes()
		{ }

	void create( EmacsMotif_Application &app, Pixel fg, Pixel bg, char *underline, Window win );

	GC gc;		// normal font
	GC special_gc;	// special font
	int underline;
	};

enum EmacsMotifCallback
	{
	EmacsMotifCallback_Expose = 1,
	EmacsMotifCallback_Resize,
	EmacsMotifCallback_Input,
	EmacsMotifCallback_ValueChangedScale,
	EmacsMotifCallback_ValueChangedButton,
	EmacsMotifCallback_Drag,
	EmacsMotifCallback_Activate
	};

class EmacsMotif_Application;

//
//	THe core of the widget support
//
class EmacsMotif_Core
	{
public:
	// create a widget
	EmacsMotif_Core( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, WidgetClass widget_class );
	// take control of a already existing widget
	EmacsMotif_Core( EmacsMotif_Application &app, Widget w );

	virtual ~EmacsMotif_Core();

	
	// call after creation but before XtRealize()
	virtual void preRealizeInit() = 0;
	// call after XtRealize()
	virtual void postRealizeInit() = 0;

	virtual void expose( XtPointer client_data, XtPointer call_data );
	virtual void resize( XtPointer client_data, XtPointer call_data );
	virtual void input( XtPointer client_data, XtPointer call_data );
	// for ScrollBar
	virtual void valueChanged( XtPointer client_data, XtPointer call_data );
	virtual void drag( XtPointer client_data, XtPointer call_data );
	// for push buttons
	virtual void activate( XtPointer client_data, XtPointer call_data );

	void manageWidget();
	void unmanageWidget();

	// destroy a child of this widget
	virtual bool destroyChildWidget( const EmacsString &name );

	// call all the preRealizeInit() function from this core class down
	void callPreRealizeInit();
	// call all the postRealizeInit() function from this core class down
	void callPostRealizeInit();

#if DBG_DISPLAY
	virtual void debugWidget( const char *label );	// hook for debugging
	void debugPrintWidgetTree( const char *label, int level=0 );
#endif
public:
	// setup the object's widget for use by this class
	void initCore();
	
	// install a handler on this object
	void install_handler( EmacsMotifCallback callback, XtPointer client_data = NULL );

	static void expose_handler( Widget w, XtPointer client_data, XtPointer call_data );
	static void resize_handler( Widget w, XtPointer client_data, XtPointer call_data );
#if 0
	static void input_handler( Widget w, XtPointer client_data, XtPointer call_data );
#else
	static void input_handler( Widget w, XEvent *e, String *params, Cardinal *num_params );
#endif
	static void value_changed_handler( Widget w, XtPointer client_data, XtPointer call_data );
	static void drag_handler( Widget w, XtPointer client_data, XtPointer call_data );
	static void activate_handler( Widget w, XtPointer client_data, XtPointer call_data );

	//
	//	Member variables
	//
	EmacsMotif_Application &application;
	Widget widget;
	EmacsMotif_Core *parent;
	EmacsMotif_Attributes attr;
	EmacsString widget_type;
	EmacsString widget_name;
	};

class EmacsMotif_ScrollBar : public EmacsMotif_Core
	{
public:
	EmacsMotif_ScrollBar( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int orientation );
	virtual ~EmacsMotif_ScrollBar();

	void preRealizeInit();
	void postRealizeInit();
	void position( int x, int y, int width, int height );
	};

class EmacsMotif_ScrollBarVertical : public EmacsMotif_ScrollBar
	{
public:
	EmacsMotif_ScrollBarVertical( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name );
	virtual ~EmacsMotif_ScrollBarVertical() { }

	void preRealizeInit();
	void postRealizeInit();

	// for ScrollBar
	virtual void valueChanged( XtPointer client_data, XtPointer call_data );
	virtual void drag( XtPointer client_data, XtPointer call_data );

	void setValue( int value, int max );
	
	EmacsWindow *win;
	};

class EmacsMotif_ScrollBarHorizontal : public EmacsMotif_ScrollBar
	{
public:
	EmacsMotif_ScrollBarHorizontal( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name );
	virtual ~EmacsMotif_ScrollBarHorizontal() { }

	void preRealizeInit();
	void postRealizeInit();

	// for ScrollBar
	virtual void valueChanged( XtPointer client_data, XtPointer call_data );
	virtual void drag( XtPointer client_data, XtPointer call_data );

	void setValue( int value );

	EmacsWindow *win;
	};

class EmacsMotif_Frame : public EmacsMotif_Core
	{
public:
	EmacsMotif_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name );
	virtual ~EmacsMotif_Frame();
	void preRealizeInit();
	void postRealizeInit();
	};

class EmacsMotif_DrawingArea : public EmacsMotif_Core
	{
public:
	EmacsMotif_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name );
	virtual ~EmacsMotif_DrawingArea();

	void preRealizeInit();
	void postRealizeInit();

	virtual void draw() = 0;
	};

class EmacsMotif_TextLabel : public EmacsMotif_DrawingArea
	{
public:
	EmacsMotif_TextLabel( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int _length=0 );
	~EmacsMotif_TextLabel();

	void update( const EmacsString &new_text );
	void draw();
	virtual void expose( XtPointer client_data, XtPointer call_data );
	void preRealizeInit();
	void postRealizeInit();
private:
	EmacsString text;
	int length;
	};

class EmacsMotif_TextLabelFrame : public EmacsMotif_Core
	{
public:
	EmacsMotif_TextLabelFrame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name );
	virtual ~EmacsMotif_TextLabelFrame();
	void preRealizeInit();
	void postRealizeInit();
	};


//==============================================================================//
//										//
//	The Emacs Motif Widget Tree Classes					//
//										//
//==============================================================================//


//------------------------------------------------------------------------------//
//										//
//	Status Bar								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_StatusIndicator_LineNumber : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_LineNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_LineNumber();

	void update();
	};

class EmacsMotif_StatusIndicator_ColumnNumber : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_ColumnNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_ColumnNumber();

	void update();
	};

class EmacsMotif_StatusIndicator_ReadOnly : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_ReadOnly( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_ReadOnly();

	void update();
	};

class EmacsMotif_StatusIndicator_OverStrike : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_OverStrike( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_OverStrike();

	void update();
	};

class EmacsMotif_StatusIndicator_RecordType : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_RecordType( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_RecordType();

	void update();
	};

class EmacsMotif_StatusIndicator_Message : public EmacsMotif_TextLabel
	{
public:
	EmacsMotif_StatusIndicator_Message( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusIndicator_Message();
	};

class EmacsMotif_StatusBar_Form : public EmacsMotif_Core
	{
public:
	EmacsMotif_StatusBar_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusBar_Form();

	void preRealizeInit();
	void postRealizeInit();

	void updateIndicators();

	// children widgets

	// frames for the indicators
	EmacsMotif_TextLabelFrame	line_number_frame;
	EmacsMotif_TextLabelFrame	column_number_frame;
	EmacsMotif_TextLabelFrame	read_only_frame;
	EmacsMotif_TextLabelFrame	over_strike_frame;
	EmacsMotif_TextLabelFrame	record_type_frame;

	// indicators
	EmacsMotif_StatusIndicator_LineNumber line_number;
	EmacsMotif_StatusIndicator_ColumnNumber column_number;
	EmacsMotif_StatusIndicator_ReadOnly read_only;
	EmacsMotif_StatusIndicator_OverStrike over_strike;
	EmacsMotif_StatusIndicator_RecordType record_type;

	EmacsMotif_StatusIndicator_Message message;
	};

class EmacsMotif_StatusBar_Frame : public EmacsMotif_Core
	{
public:
	EmacsMotif_StatusBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_StatusBar_Frame();

	void preRealizeInit();
	void postRealizeInit();

	// children widgets
	EmacsMotif_StatusBar_Form form;
	};

//------------------------------------------------------------------------------//
//										//
//	Tool Bar								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_ToolBar_Button : public EmacsMotif_Core
	{
public:
	EmacsMotif_ToolBar_Button
		(
		EmacsMotif_Application &app,
		EmacsMotif_Core &parent,
		const EmacsString &name,
		const EmacsString &label,
		const EmacsMotif_ActionItem &action
		);
	virtual ~EmacsMotif_ToolBar_Button();

	void preRealizeInit();
	void postRealizeInit();

	void activate( XtPointer client_data, XtPointer call_data );

	const EmacsString label;
	EmacsMotif_ActionItem action;
	};

class EmacsMotif_ToolBar_Separator : public EmacsMotif_Core
	{
public:
	EmacsMotif_ToolBar_Separator
		(
		EmacsMotif_Application &app,
		EmacsMotif_Core &parent,
		const EmacsString &name,
		int width
		);
	virtual ~EmacsMotif_ToolBar_Separator();

	void preRealizeInit();
	void postRealizeInit();
	};

class EmacsMotif_ToolBar_RowColumn : public EmacsMotif_Core
	{
public:
	EmacsMotif_ToolBar_RowColumn( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_ToolBar_RowColumn();

	void preRealizeInit();
	void postRealizeInit();

	// no children widgets
	// they are added dynamically
	};

class EmacsMotif_ToolBar_Frame : public EmacsMotif_Core
	{
public:
	EmacsMotif_ToolBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_ToolBar_Frame();

	void preRealizeInit();
	void postRealizeInit();

	// children widgets
	EmacsMotif_ToolBar_RowColumn row_column;
	};

//------------------------------------------------------------------------------//
//										//
//	Drawing Area								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_Screen_DrawingArea : public EmacsMotif_Core
	{
public:
	EmacsMotif_Screen_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_Screen_DrawingArea();

	void preRealizeInit();
	void postRealizeInit();

	void resize( XtPointer client_data, XtPointer call_data );
	void expose( XtPointer client_data, XtPointer call_data );
	void input( XtPointer client_data, XtPointer call_data );

	EmacsMotif_Attributes attr_array[256];
	EmacsMotif_Attributes cu_attr;
	EmacsMotif_Attributes rev_attr;
	EmacsMotif_Attributes curev_attr;
	};

class EmacsMotif_Screen_Frame : public EmacsMotif_Core
	{
public:
	EmacsMotif_Screen_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_Screen_Frame();

	void preRealizeInit();
	void postRealizeInit();

	EmacsMotif_Screen_DrawingArea drawing_area;
	};

//------------------------------------------------------------------------------//
//										//
//	Menu Bar								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_MenuBar : public EmacsMotif_Core
	{
public:
	EmacsMotif_MenuBar( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	~EmacsMotif_MenuBar();

	void preRealizeInit();
	void postRealizeInit();
	};



//------------------------------------------------------------------------------//
//										//
//	Main screen form								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_Screen_Form : public EmacsMotif_Core
	{
public:
	EmacsMotif_Screen_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_Screen_Form();

	void preRealizeInit();
	void postRealizeInit();

	// based on the users preference layout the main screen components
	void configureScreenGeometry(void);


#ifdef TOOL_BAR_PRESENT
	EmacsMotif_ToolBar_Frame	tool_bar;
#endif
	EmacsMotif_Screen_Frame		screen;
#ifdef STATUS_BAR_PRESENT
	EmacsMotif_StatusBar_Frame	status_bar;
#endif
	};


//------------------------------------------------------------------------------//
//										//
//	Main Window								//
//										//
//------------------------------------------------------------------------------//
class EmacsMotif_MainWindow : public EmacsMotif_Core
	{
public:
	EmacsMotif_MainWindow( EmacsMotif_Application &app, EmacsMotif_Core &parent );
	virtual ~EmacsMotif_MainWindow();

	void preRealizeInit();
	void postRealizeInit();

	EmacsMotif_MenuBar		menu_bar;
	EmacsMotif_Screen_Form		screen_form;
	};



// application shell
class EmacsMotif_Shell : public EmacsMotif_Core
	{
public:
	EmacsMotif_Shell( EmacsMotif_Application &app, Widget shell_widget );
	~EmacsMotif_Shell();

	void preRealizeInit();
	void postRealizeInit();

	EmacsMotif_MainWindow		main_window;
	};

// display
// application context

class EmacsMotif_Application
	{
public:
	static const char *emacs_class;

	enum { scroll_bar_min_width = 16 };

	struct _resources
		{
		Boolean display_scroll_bars;	// True if scroll bars are required
		Boolean display_status_bar;	// true if the status bar is required
		Boolean display_tool_bar;	// true if the tool bar is required
		char *normal_font_name;		// Font for normal text
		char *special_font_name;	// Font for display-non-printing and C1 controls text
#ifdef BOLDFONT
		char *bold_font_name;		// Font for bolded text
#endif
		int rows;			// Number of character rows in the window
		int columns;			// Number of character columns in the window
		Pixel n_fg;			// Foreground for normal text
		Pixel n_bg;			// Background for normal text
		char *n_underline;
		Pixel ml_fg;			// Foreground for mode line text
		Pixel ml_bg;			// Background for mode line text
		char *ml_underline;
		Pixel hl_fg;			// Foreground for highlighted text
		Pixel hl_bg;			// Background for highlighted text
		char *hl_underline;
		Pixel c_fg;			// Foreground for cursor
		Pixel c_bg;			// Background for cursor
		char *c_underline;
		Pixel word_fg;			// word foreground colour
		Pixel word_bg;			// word foreground colour
		char *word_underline;
		Pixel string_fg;		// string foreground colour
		Pixel string_bg;		// string foreground colour
		char *string_underline;
		Pixel comment_fg;		// comment foreground colour
		Pixel comment_bg;		// comment foreground colour
		char *comment_underline;
		Pixel kw1_fg;			// kw1 foreground colour
		Pixel kw1_bg;			// kw1 foreground colour
		char *kw1_underline;
		Pixel kw2_fg;			// kw2 foreground colour
		Pixel kw2_bg;			// kw2 foreground colour
		char *kw2_underline;
		Pixel kw3_fg;			// kw3 foreground colour
		Pixel kw3_bg;			// kw3 foreground colour
		char *kw3_underline;
		Pixel user1_fg;			// user 1 foreground colour
		Pixel user1_bg;
		char *user1_underline;
		Pixel user2_fg;			// user 2 foreground colour
		Pixel user2_bg;
		char *user2_underline;
		Pixel user3_fg;			// user 3 foreground colour
		Pixel user3_bg;
		char *user3_underline;
		Pixel user4_fg;			// user 4 foreground colour
		Pixel user4_bg;
		char *user4_underline;
		Pixel user5_fg;			// user 5 foreground colour
		Pixel user5_bg;
		char *user5_underline;
		Pixel user6_fg;			// user 6 foreground colour
		Pixel user6_bg;
		char *user6_underline;
		Pixel user7_fg;			// user 7 foreground colour
		Pixel user7_bg;
		char *user7_underline;
		Pixel user8_fg;			// user 8 foreground colour
		Pixel user8_bg;
		char *user8_underline;
		char *p_fg;			// Foreground for pointer
		char *p_bg;			// Background for pointer
		int p_shape;			// Pointer shape
		};
	typedef struct _resources Resources;
	EmacsMotif_Application();
	~EmacsMotif_Application();

 	bool initApplication( const EmacsString &display_location );

	Display *dpy;
	int dpy_fd;
	XtAppContext app_context;
	EmacsMotif_Shell *shell;

	Resources resources;

	XFontStruct *normalFontStruct;
	XFontStruct *specialFontStruct;
	XFontStruct *pointerFontStruct;

	Cursor arrowCursor;
	int cursors;

	int innerBorder;
	int fth;
	int ftw;
	int ftd;
	int pixelWidth;
	int pixelHeight;
	static void xt_error_handler( char *message );
	static void xt_warning_handler( char *message );
	static int x_error_handler( Display *dpy, XErrorEvent *err_event );
	};

class TerminalControl_GUI
	{
public:
	TerminalControl_GUI( const EmacsString &device );
	~TerminalControl_GUI();
//	void *operator new(size_t size);
//	void operator delete(void *p);

	//
	//	Screen control
	//
	void t_topos(int, int);	/* move the cursor to the indicated (row,column); (1,1) is the upper left */
	void t_reset();		/* reset terminal (screen is in unkown state, convert it to a known one) */
	bool t_update_begin();
	void t_update_end();
	void t_insert_lines(int);	/* insert n lines                   */
	void t_delete_lines(int);	/* delete n lines                   */
	void t_wipe_line(int);	/* erase to the end of the line     */
	bool t_window(void) { return true; }
	bool t_window(int);		/* set the screen window so that IDline operations only affect the first n lines of the screen */
	void t_display_activity(  unsigned char );
	void t_update_line(EmacsLine *oldl, EmacsLine *newl, int ln);	/* Routine to call to update a line */
	void t_change_attributes();	/* Routine to change attributes     */
	void t_beep();
	//
	//	Keyboard routines
	//
	void k_check_for_input();	/* check for any input */
	void t_wipe_screen();
	void t_flash();
	int k_input_event( unsigned char *, unsigned int );

	void keyboardInput( XKeyEvent *event );
	void mouseInput( XButtonEvent *event );
	void mouseMotionInput( XMotionEvent *event );
	void updateWindowTitle( void );
	void viewStatusBar( void );
	void viewToolBar( void );

	void input_char_string( const EmacsString &keys, bool shift );
public:
	int ToggleCursor();
	void write_region( int width, int height, int x, int y );
	int create_windows( const EmacsString &displayName );

	EmacsMotif_Application application;
	EmacsMotif_Screen_DrawingArea *drawing_area;

	void fixup_user_interface(void);
	void fixup_scroll_bars(void);
	void fixup_menus(void);
	void fixup_toolbars(void);

	enum { max_scroll_bars = 32 };

	EmacsMotif_ScrollBarVertical *vert_scroll[max_scroll_bars];
	EmacsMotif_ScrollBarHorizontal *horz_scroll[max_scroll_bars];

	EmacsMotif_ActionItem *action;

	int last_mouse_x;
	int last_mouse_y;
private:
	int mouse_button_state;		// bit mask of pressed buttons
	};

const char *icon_bits( (char *)write_scrolls_bits );
const int icon_width( write_scrolls_width );
const int icon_height( write_scrolls_height );
const int SB_SIZE( 100000 );
Display *dpy;
int dpy_fd;
int motif_iconic = 0;


// draw in the colours of the widgetstatic 

// Realise the emacs widgets
int TerminalControl_GUI::create_windows( const EmacsString &display_location )
	{
	if( !application.initApplication( display_location ) )
		return 0;

	drawing_area = &application.shell->main_window.screen_form.screen.drawing_area;

	application.shell->callPreRealizeInit();

	XtRealizeWidget( application.shell->widget );

	application.shell->callPostRealizeInit();

	return 1;
	}


extern int vertical_bar_width;

//------------------------------------------------------//
//							//
//   EmacsMotif_Core implementation			//
//							//
//------------------------------------------------------//
// create a widget
EmacsMotif_Core::EmacsMotif_Core( EmacsMotif_Application &app, EmacsMotif_Core &_parent, const EmacsString &name, WidgetClass widget_class )
	: application( app )
	, widget( NULL )
	, parent( &_parent )
	, widget_type( widget_class->core_class.class_name )
	, widget_name( name )
	{
	widget = XtCreateWidget( name, widget_class, parent->widget, NULL, 0 );

	// init the Core
	initCore();
	}

// take control of a already existing widget
EmacsMotif_Core::EmacsMotif_Core( EmacsMotif_Application &app, Widget w )
	: application( app )
	, widget( w )
	, parent( NULL )
	, widget_type( w->core.widget_class->core_class.class_name )
	, widget_name( w->core.name )
	{
	initCore();
	}

EmacsMotif_Core::~EmacsMotif_Core()
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("~EmacsMotif_Core");
#endif
	XtDestroyWidget( widget );
	}

// setup the object's widget for use by this class
void EmacsMotif_Core::initCore()
	{
	EmacsXtArgs args;
	args.setArg( XmNuserData, (XtPointer )this );
	args.setValues( widget );
	}

void EmacsMotif_Core::manageWidget()
	{
	XtManageChild( widget );
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("manageWidget");
#endif
	}
void EmacsMotif_Core::unmanageWidget()
	{
	XtUnmanageChild( widget );
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("unmanageWidget");
#endif
	}

//
//	Will find the first widget with the given name.
//	Attempting to delete two widgets at a time leads to a SEGV.
//	returns true if any where deleted.
//
bool EmacsMotif_Core::destroyChildWidget( const EmacsString &name )
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			if( core_object->widget_name == name )
				{
				delete core_object;
				return true;
				}
			}
		}

	return false;
	}

// call all the preRealizeInit() function from this core class down
void EmacsMotif_Core::callPreRealizeInit()
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->callPreRealizeInit();
			}
		}

	// call preRealizeInit on this widget
#if DBG_DISPLAY
	if( dbg_flags&DBG_DISPLAY )
		_dbg_msg( FormatString("calling preRealizeInit for %s %s")
			<< widget_type << widget_name );
#endif
	preRealizeInit();
	}

// call all the postRealizeInit() function from this core class down
void EmacsMotif_Core::callPostRealizeInit()
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->callPostRealizeInit();
			}
		}

	// call preRealizeInit on this widget
#if DBG_DISPLAY
	if( dbg_flags&DBG_DISPLAY )
		_dbg_msg( FormatString("calling postRealizeInit for %s %s")
			<< widget_type << widget_name );
#endif
	postRealizeInit();
	}

// call debugWidget for all widgets in the tree
void EmacsMotif_Core::debugPrintWidgetTree( const char *label, int level )
	{
	EmacsString label_and_level( FormatString("%*s %s") << level*2 << "" << label );
	debugWidget( label_and_level.sdata() );

	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->debugPrintWidgetTree( label, level+1 );
			}
		}
	}

// install a handler on this object
void EmacsMotif_Core::install_handler
	(
	EmacsMotifCallback callback,
	XtPointer client_data
	)
	{
	switch( callback )
		{
	case EmacsMotifCallback_Expose:
		XtAddCallback( widget, XmNexposeCallback, expose_handler, client_data );
		break;

	case EmacsMotifCallback_Resize:
		XtAddCallback( widget, XmNresizeCallback, resize_handler, client_data );
		break;

	case EmacsMotifCallback_Input:
#if 0
		// new motif works
		XtAddCallback( widget, XmNinputCallback, input_handler, client_data );
		break;
#else
		// old motif need this
		{
		XtActionsRec action;
		action.string = "emacs_input_handler";
		action.proc = input_handler;

		XtAppAddActions( application.app_context, &action, 1 );

		XtTranslations trans = XtParseTranslationTable
			(
			"<Key>:		emacs_input_handler()"	"\n"
			"<Motion>:	emacs_input_handler()"	"\n"
			"<BtnDown>:	emacs_input_handler()"	"\n"
			"<BtnUp>:	emacs_input_handler()"
			);
		XtOverrideTranslations( widget, trans );
		}
		break;
#endif
	case EmacsMotifCallback_ValueChangedButton:
		// toggle button value changed uses the activate semantics
		XtAddCallback( widget, XmNvalueChangedCallback, activate_handler, client_data );
		break;

	case EmacsMotifCallback_ValueChangedScale:
		XtAddCallback( widget, XmNvalueChangedCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNincrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNdecrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNpageDecrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNpageIncrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNtoTopCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNtoBottomCallback, value_changed_handler, client_data );
		break;

	case EmacsMotifCallback_Drag:
		XtAddCallback( widget, XmNdragCallback, drag_handler, client_data ); break;

	case EmacsMotifCallback_Activate:
		XtAddCallback( widget, XmNactivateCallback, activate_handler, client_data ); break;
	default:
		emacs_assert( false );
		}
	}

#if DBG_DISPLAY
void EmacsMotif_Core::debugWidget( const char *label )
	{
	_dbg_msg( FormatString( "%s %s(%s) M:%d R:%d x:%d y:%d w:%d h:%d")
		<< label
		<< widget_type << widget_name << XtIsManaged( widget ) << (XtIsRealized( widget ) != 0)
		<< widget->core.x << widget->core.y
		<< widget->core.width << widget->core.height );
	}
#endif

void EmacsMotif_Core::expose( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "expose" );
#endif
	}

void EmacsMotif_Core::resize( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "resize" );
#endif
	}

void EmacsMotif_Core::input( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "input" );
#endif
	}

void EmacsMotif_Core::valueChanged( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "valueChanged" );
#endif
	}

void EmacsMotif_Core::drag( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "drag" );
#endif
	}

void EmacsMotif_Core::activate( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "activate" );
#endif
	}


void EmacsMotif_Core::expose_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->expose( client_data, call_data );
	}

void EmacsMotif_Core::resize_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->resize( client_data, call_data );
	}

#if 0
void EmacsMotif_Core::input_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->input( client_data, call_data );
	}
#else
void EmacsMotif_Core::input_handler( Widget w, XEvent *e, String *, Cardinal * )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	XmDrawingAreaCallbackStruct call_data;
	call_data.reason = XmCR_INPUT;
	call_data.event = e;
	call_data.window = XtWindow( w );

	_this->input( NULL, &call_data );
	}
#endif
void EmacsMotif_Core::value_changed_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->valueChanged( client_data, call_data );
	}

void EmacsMotif_Core::drag_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->drag( client_data, call_data );
	}

void EmacsMotif_Core::activate_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	XtGetValues( w, args, args );

	emacs_assert( _this->widget == w );

	_this->activate( client_data, call_data );
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_Frame implementation			//
//							//
//------------------------------------------------------//
EmacsMotif_Frame::EmacsMotif_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmFrameWidgetClass )
	{ }

EmacsMotif_Frame::~EmacsMotif_Frame()
	{ }

void EmacsMotif_Frame::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNshadowType, XmSHADOW_IN );
	args.setArg( XmNborderWidth, 1 );

	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_Frame::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_DrawingArea implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_DrawingArea::EmacsMotif_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmDrawingAreaWidgetClass )
	{
	// make sure that we do not get swamped under motion events
	widget->core.widget_class->core_class.compress_motion = true;
	}

EmacsMotif_DrawingArea::~EmacsMotif_DrawingArea()
	{ }

void EmacsMotif_DrawingArea::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNborderWidth, 1 );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_DrawingArea::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_TextLabel implementation		//
//							//
//------------------------------------------------------//

EmacsMotif_TextLabel::EmacsMotif_TextLabel( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int _length )
	: EmacsMotif_DrawingArea( app, parent, name )
	, text()
	, length( _length )
	{ }

EmacsMotif_TextLabel::~EmacsMotif_TextLabel()
	{ }

void EmacsMotif_TextLabel::preRealizeInit()
	{
	install_handler( EmacsMotifCallback_Expose );
	install_handler( EmacsMotifCallback_Resize );

	EmacsXtArgs args;
	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
	args.setArg( XmNheight, (application.fth+4) );
	if( length != 0 )
		args.setArg( XmNwidth, (application.ftw*length) + 2 );	// optional width
	args.setValues( widget );
	XtManageChild( widget );

	manageWidget();
	}

void EmacsMotif_TextLabel::postRealizeInit()
	{
	}

void EmacsMotif_TextLabel::update( const EmacsString &new_text )
	{
	if( text != new_text )
		{
		text = new_text;
		draw();
		}
	}

void EmacsMotif_TextLabel::draw()
	{
	if( !XtIsRealized( widget ) )
		return;

	if( attr.gc == NULL )
		{
		Pixel fg, bg;
		EmacsXtArgs args;

		// draw in the colours of the widget
		args.setArg( XmNforeground, &fg );
		args.setArg( XmNbackground, &bg );
		args.getValues( widget );
		attr.create( application, fg, bg, "no", XtWindow( widget ) );
		}

	XDrawImageString
		(
		application.dpy, XtWindow( widget ), attr.gc,
		1, application.fth - application.ftd,
		text.sdata(), text.length()
		);
	}

void EmacsMotif_TextLabel::expose( XtPointer , XtPointer event_ )
	{
	XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "TextLabel expose" );
#endif

	switch( event->event->type )
		{
	case NoExpose:
		break;

	case GraphicsExpose:
	case Expose:
		// draw status text
		// _dbg_msg("status exposed...\n");
		draw();
		break;

	default:
		_dbg_msg( FormatString( "Unknown exposure event = %d" ) << event->event->type );
		break;
		}
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_TextLabelFrame implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_TextLabelFrame::EmacsMotif_TextLabelFrame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmFrameWidgetClass )
	{ }

EmacsMotif_TextLabelFrame::~EmacsMotif_TextLabelFrame()
	{ }

void EmacsMotif_TextLabelFrame::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNshadowType, XmSHADOW_IN );
	args.setArg( XmNborderWidth, 0l );

	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_TextLabelFrame::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_ScrollBar implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_ScrollBar::EmacsMotif_ScrollBar( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int orientation )
	: EmacsMotif_Core( app, parent, name, xmScrollBarWidgetClass )
	{
	EmacsXtArgs args;

	args.setArg( XmNorientation, orientation );
	args.setValues( widget );
	}

EmacsMotif_ScrollBar::~EmacsMotif_ScrollBar()
	{ }

void EmacsMotif_ScrollBar::preRealizeInit()
	{
	install_handler( EmacsMotifCallback_ValueChangedScale );
	install_handler( EmacsMotifCallback_Drag );

	manageWidget();
	}

void EmacsMotif_ScrollBar::postRealizeInit()
	{
	}

void EmacsMotif_ScrollBar::position( int x, int y, int width, int height )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBar::position X:%d Y:%d W:%d H:%d") << x << y << width << height );
#endif
	EmacsXtArgs args;
	args.setArg( XmNx, x );
	args.setArg( XmNy, y );
	args.setArg( XmNwidth, width );
	args.setArg( XmNheight, height );
	args.setValues( widget );

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "ScrollBar::position" );
#endif
	}

EmacsMotif_ScrollBarVertical::EmacsMotif_ScrollBarVertical( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_ScrollBar( app, parent, name, XmVERTICAL )
	{ }

void EmacsMotif_ScrollBarVertical::preRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::preRealizeInit();

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 11 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, 1 );
	args.setArg( XmNpageIncrement, 8 );	// should be 80% window size
	args.setValues( widget );
	}

void EmacsMotif_ScrollBarVertical::postRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::postRealizeInit();
	}

EmacsMotif_ScrollBarHorizontal::EmacsMotif_ScrollBarHorizontal( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_ScrollBar( app, parent, name, XmHORIZONTAL )
	{ }

void EmacsMotif_ScrollBarHorizontal::preRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::preRealizeInit();

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 256+1+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, 1 );
	args.setArg( XmNpageIncrement, 4 );	// should be tab-size
	args.setValues( widget );
	}

void EmacsMotif_ScrollBarHorizontal::postRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::postRealizeInit();
	}

	// for ScrollBar
void EmacsMotif_ScrollBarVertical::valueChanged( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarVert::valueChanged R:%d V:%d") << cbs->reason << cbs->value );
#endif

	}

void EmacsMotif_ScrollBarVertical::drag( XtPointer client_data, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarVert::drag R:%d V:%d") << cbs->reason << cbs->value );
#endif
	valueChanged( client_data, call_data );
	}

void EmacsMotif_ScrollBarVertical::setValue( int value, int max )
	{
	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 1+max+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, value );
	args.setValues( widget );
	}


	// for ScrollBar
void EmacsMotif_ScrollBarHorizontal::valueChanged( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarHoriz::valueChanged R:%d V:%d") << cbs->reason << cbs->value );
#endif
	}

void EmacsMotif_ScrollBarHorizontal::drag( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarHoriz::drag R:%d V:%d") << cbs->reason << cbs->value );
#endif
	}

void EmacsMotif_ScrollBarHorizontal::setValue( int value )
	{
	int max=256;

	while( max < value )
		max *= 2;

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, max+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, value );
	args.setValues( widget );
	}


void EmacsMotif_Attributes::create( EmacsMotif_Application &app, Pixel fg, Pixel bg, char *underline_text, Window win )
	{
	XGCValues gcv;

	gcv.fill_style = FillSolid;
	gcv.plane_mask = AllPlanes;
	gcv.line_style = LineSolid;
	gcv.foreground =  fg;
	gcv.background =  bg;

	long int bits = GCFont | GCForeground | GCBackground | GCFillStyle| GCPlaneMask;

	if( underline_text != NULL  )
		{
		int num_parsed;
		char keyword[80];
		int dash_value;

		keyword[0] = '\0';
		num_parsed = sscanf( underline_text,"%s %d", keyword, &dash_value  );

		if( strcmp("1",keyword ) == 0
		|| strcmp("true",keyword ) == 0
		|| strcmp("yes",keyword ) == 0  )
			{
			underline = 1;
			}
		else if( strcmp("dash",keyword ) == 0  )
			{
			bits |= GCLineStyle;
			underline = 1;
			gcv.line_style = LineOnOffDash;
			if( num_parsed > 1 && dash_value > 0 )
				{
				bits |= GCDashList;
				gcv.dashes = (char)dash_value;
				}
			}
		}

	gcv.font = app.normalFontStruct->fid;
	gc = XCreateGC( app.dpy, win, bits, &gcv );

	gcv.font = app.specialFontStruct->fid;
	special_gc = XCreateGC( app.dpy, win, bits, &gcv );
	}

//==============================================================================//
//										//
//	UI widgets								//
//										//
//==============================================================================//


//------------------------------------------------------------------------------//
//										//
//	Status Bar widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_StatusIndicator_LineNumber::EmacsMotif_StatusIndicator_LineNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "line_number_text", 7 )
	{ }

EmacsMotif_StatusIndicator_LineNumber::~EmacsMotif_StatusIndicator_LineNumber()
	{ }	


void EmacsMotif_StatusIndicator_LineNumber::update()
	{
	int value = 1234567;
	EmacsMotif_TextLabel::update( FormatString( "%07.7d" ) << value );
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_ColumnNumber::EmacsMotif_StatusIndicator_ColumnNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "column_number_text", 4 )
	{ }

EmacsMotif_StatusIndicator_ColumnNumber::~EmacsMotif_StatusIndicator_ColumnNumber()
	{ }	


void EmacsMotif_StatusIndicator_ColumnNumber::update()
	{
	int value = 1234;
	EmacsMotif_TextLabel::update( FormatString( "%04.4d" ) << value );
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_ReadOnly::EmacsMotif_StatusIndicator_ReadOnly( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "read_only_text", 4 )
	{ }

EmacsMotif_StatusIndicator_ReadOnly::~EmacsMotif_StatusIndicator_ReadOnly()
	{ }	


void EmacsMotif_StatusIndicator_ReadOnly::update()
	{
	EmacsMotif_TextLabel::update("READ");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_OverStrike::EmacsMotif_StatusIndicator_OverStrike( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "over_strike_text", 3 )
	{ }

EmacsMotif_StatusIndicator_OverStrike::~EmacsMotif_StatusIndicator_OverStrike()
	{ }	


void EmacsMotif_StatusIndicator_OverStrike::update()
	{
	EmacsMotif_TextLabel::update("OVR");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_RecordType::EmacsMotif_StatusIndicator_RecordType( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "record_type_text", 4 )
	{ }

EmacsMotif_StatusIndicator_RecordType::~EmacsMotif_StatusIndicator_RecordType()
	{ }	

void EmacsMotif_StatusIndicator_RecordType::update()
	{
	EmacsMotif_TextLabel::update("CRLF");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_Message::EmacsMotif_StatusIndicator_Message( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "message_text" )
	{ }

EmacsMotif_StatusIndicator_Message::~EmacsMotif_StatusIndicator_Message()
	{ }	


//--------------------------------------------------------------------------------
EmacsMotif_StatusBar_Form::EmacsMotif_StatusBar_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "status_bar_form", xmFormWidgetClass )
	, line_number_frame( app, *this, "line_number_frame" )
	, column_number_frame( app, *this, "column_number_frame" )
	, read_only_frame( app, *this, "read_only_frame" )
	, over_strike_frame( app, *this, "over_strike_frame" )
	, record_type_frame( app, *this, "record_type_frame" )
	, line_number( app, line_number_frame )
	, column_number( app, column_number_frame )
	, read_only( app, read_only_frame )
	, over_strike( app, over_strike_frame )
	, record_type( app, record_type_frame )
	, message( app, *this )
	{ }

EmacsMotif_StatusBar_Form::~EmacsMotif_StatusBar_Form()
	{ }

void EmacsMotif_StatusBar_Form::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_FORM );
	args.setArg( XmNrightAttachment, XmATTACH_WIDGET );
	args.setArg( XmNrightWidget, read_only_frame.widget );
	args.setValues( message.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -500 );
	args.setValues( record_type_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -400 );
	args.setValues( read_only_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -300 );
	args.setValues( over_strike_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -200 );
	args.setValues( line_number_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -100 );
	args.setValues( column_number_frame.widget );

	manageWidget();
	}

void EmacsMotif_StatusBar_Form::postRealizeInit()
	{
	EmacsXtArgs args;

	// now that the resizing has happened ask the size of the frames are adjust the offsets
	int spacing = 2;
	int offset = 0;

	offset -= column_number_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( column_number_frame.widget );
	
	offset -= line_number_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( line_number_frame.widget );

	offset -= over_strike_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( over_strike_frame.widget );

	offset -= record_type_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( record_type_frame.widget );

	offset -= read_only_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( read_only_frame.widget );
	}

void EmacsMotif_StatusBar_Form::updateIndicators()
	{
	line_number.update();
	column_number.update();
	read_only.update();
	over_strike.update();
	record_type.update();
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusBar_Frame::EmacsMotif_StatusBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "status_bar_frame", xmFrameWidgetClass )
	, form( app, *this )
	{ }

EmacsMotif_StatusBar_Frame::~EmacsMotif_StatusBar_Frame()
	{ }

void EmacsMotif_StatusBar_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_StatusBar_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Status Bar widgets							//
//										//
//------------------------------------------------------------------------------//

EmacsMotif_ToolBar_Button::EmacsMotif_ToolBar_Button
	(
	EmacsMotif_Application &app,
	EmacsMotif_Core &parent,
	const EmacsString &name,
	const EmacsString &_label,
	const EmacsMotif_ActionItem &_action
	)
#ifdef TOOLBAR_TOGGLE_BUTTONS
	: EmacsMotif_Core( app, parent, name, _action.isToggleButton() ? xmToggleButtonWidgetClass : xmPushButtonWidgetClass )
#else
	: EmacsMotif_Core( app, parent, name, xmPushButtonWidgetClass )
#endif
	, label( _label )
	, action( _action )
	{
	action.setButton( widget, action.isToggleButton() );

	if( label.first( '/' ) < 0
	&&  label.first( ':' ) < 0 )
		{
		XmString p = XmStringCreateSimple( label.sdataHack() );

		EmacsXtArgs args;

		args.setArg( XmNlabelString, p );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( widget );

		XmStringFree( p );
		}
	else
		{
		EmacsXtArgs args;

		Pixel fg;
		Pixel bg;

		// find out our fg and bg colours
		args.setArg( XmNforeground, &fg );
		args.setArg( XmNbackground, &bg );
		args.getValues( widget );

		EmacsString fullname( label );

		Pixmap pixmap = XmGetPixmap( XtScreen( parent.widget ), fullname.sdataHack(), fg, bg );

		if( pixmap == XmUNSPECIFIED_PIXMAP )
			{
			_dbg_msg( FormatString("Cannot load Pixmap from %s") << fullname );	
			}
		else
			{
			args.setArg( XmNlabelType, XmPIXMAP );
			args.setArg( XmNlabelPixmap, pixmap );
			}


#ifdef TOOLBAR_TOGGLE_BUTTONS
		if( action.isToggleButton() )
			{
			args.setArg( XmNindicatorOn, 0l );		// false
			args.setArg( XmNshadowThickness, 2 );
			args.setArg( XmNborderWidth, 0l );
			args.setArg( XmNshadowType, XmSHADOW_OUT );
			args.setArg( XmNmarginHeight, 0l );
			args.setArg( XmNmarginWidth, 0l );
			args.setArg( XmNwidth, 16 );
			}
#endif

		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( widget );
		}

#ifdef TOOLBAR_TOGGLE_BUTTONS
	if( action.isToggleButton() )
		install_handler( EmacsMotifCallback_ValueChangedButton );
	else
#endif
		install_handler( EmacsMotifCallback_Activate );
	}

EmacsMotif_ToolBar_Button::~EmacsMotif_ToolBar_Button()
	{ }

void EmacsMotif_ToolBar_Button::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Button::postRealizeInit()
	{ }

void EmacsMotif_ToolBar_Button::activate( XtPointer , XtPointer  )
	{
	action.queueAction();
	}

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_Separator::EmacsMotif_ToolBar_Separator
	(
	EmacsMotif_Application &app,
	EmacsMotif_Core &parent,
	const EmacsString &name,
	int width
	)
	: EmacsMotif_Core( app, parent, name, xmDrawingAreaWidgetClass )
	{
	EmacsXtArgs args;

//	Pixel fg;
//	Pixel bg;

	// find out our parents fg and bg colours
//	args.setArg( XmNforeground, &fg );
//	args.setArg( XmNbackground, &bg );
//	args.getValues( parent.widget );

	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
//	args.setArg( XmNforeground, fg );
//	args.setArg( XmNbackground, bg );
	args.setArg( XmNborderWidth, width/2 );
	args.setArg( XmNshadowThickness, 0l );
	args.setValues( widget );
	}


EmacsMotif_ToolBar_Separator::~EmacsMotif_ToolBar_Separator()
	{ }

void EmacsMotif_ToolBar_Separator::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Separator::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_RowColumn::EmacsMotif_ToolBar_RowColumn( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "tool_bar_rowcolumn", xmRowColumnWidgetClass )
	{ }

EmacsMotif_ToolBar_RowColumn::~EmacsMotif_ToolBar_RowColumn()
	{ }

void EmacsMotif_ToolBar_RowColumn::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNorientation, XmHORIZONTAL );
	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
	args.setValues( widget );
	manageWidget();
	}

void EmacsMotif_ToolBar_RowColumn::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_Frame::EmacsMotif_ToolBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "tool_bar_frame", xmFrameWidgetClass )
	, row_column( app, *this )
	{ }

EmacsMotif_ToolBar_Frame::~EmacsMotif_ToolBar_Frame()
	{ }

void EmacsMotif_ToolBar_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Drawing Area widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_Screen_DrawingArea::EmacsMotif_Screen_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_drawing_area", xmDrawingAreaWidgetClass )
	{
	install_handler( EmacsMotifCallback_Resize, NULL );
	install_handler( EmacsMotifCallback_Expose, NULL );
	install_handler( EmacsMotifCallback_Input, NULL );
	}

EmacsMotif_Screen_DrawingArea::~EmacsMotif_Screen_DrawingArea()
	{ }

void EmacsMotif_Screen_DrawingArea::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNforeground, application.resources.n_fg );
	args.setArg( XmNbackground, application.resources.n_bg );
	args.setArg( XmNmarginHeight, 0l );
	args.setArg( XmNmarginWidth, 0l );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_Screen_DrawingArea::postRealizeInit()
	{
	rev_attr.create( application, application.resources.n_bg, application.resources.n_fg, NULL, XtWindow( widget ) );
	cu_attr.create( application, application.resources.c_fg, application.resources.c_bg, NULL, XtWindow( widget ) );
	curev_attr.create( application, application.resources.c_bg, application.resources.c_fg, NULL, XtWindow( widget ) );
	}

//------------------------------------------------------------------------------
EmacsMotif_Screen_Frame::EmacsMotif_Screen_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_frame", xmFrameWidgetClass )
	, drawing_area( app, *this )
	{ }

EmacsMotif_Screen_Frame::~EmacsMotif_Screen_Frame()
	{ }

void EmacsMotif_Screen_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_Screen_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Main screen form								//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_Screen_Form::EmacsMotif_Screen_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_form", xmFormWidgetClass )
#ifdef TOOL_BAR_PRESENT
	, tool_bar( app, *this )
#endif
	, screen( app, *this )
#ifdef STATUS_BAR_PRESENT
	, status_bar( app, *this )
#endif
	{ }

EmacsMotif_Screen_Form::~EmacsMotif_Screen_Form()
	{ }

void EmacsMotif_Screen_Form::preRealizeInit()
	{
	configureScreenGeometry();
	manageWidget();
	}	

void EmacsMotif_Screen_Form::postRealizeInit()
	{
	configureScreenGeometry();
	}

void EmacsMotif_Screen_Form::configureScreenGeometry(void)
	{
	// the main work here is to setup the children

	EmacsXtArgs args;

	EmacsXtArgs drawing_area_args;

	if( application.resources.display_tool_bar )
		{
		tool_bar.manageWidget();

		// tool_bar setup
		args.setArg( XmNtopAttachment, XmATTACH_FORM );
		args.setArg( XmNleftAttachment, XmATTACH_FORM );
		args.setArg( XmNrightAttachment, XmATTACH_FORM );
		args.setArg( XmNbottomAttachment, XmATTACH_OPPOSITE_FORM );

		//
		// 38 is the magic number that allows
		// both Push Button and Toggle Buttons
		// to display there bit maps without truncation
		//
		args.setArg( XmNbottomOffset, -38 );

		args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( tool_bar.widget );

		drawing_area_args.setArg( XmNtopAttachment, XmATTACH_WIDGET );
		drawing_area_args.setArg( XmNtopWidget, tool_bar.widget );
		}
	else
		{
		drawing_area_args.setArg( XmNtopAttachment, XmATTACH_FORM );
		tool_bar.unmanageWidget();
		}

	if( application.resources.display_status_bar )
		{
		status_bar.manageWidget();

		// status_bar setup
		args.setArg( XmNtopAttachment, XmATTACH_OPPOSITE_FORM );
		args.setArg( XmNtopOffset, -(application.fth + 2*(2+2+2)) );
		args.setArg( XmNleftAttachment, XmATTACH_FORM );
		args.setArg( XmNrightAttachment, XmATTACH_FORM );
		args.setArg( XmNbottomAttachment, XmATTACH_FORM );
		args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setArg( XmNborderWidth, 1 );
		args.setValues( status_bar.widget );

		drawing_area_args.setArg( XmNbottomAttachment, XmATTACH_WIDGET );
		drawing_area_args.setArg( XmNbottomWidget, status_bar.widget );
		}
	else
		{
		status_bar.unmanageWidget();

		drawing_area_args.setArg( XmNbottomAttachment, XmATTACH_FORM );
		}

	// drawing_area setup
	drawing_area_args.setArg( XmNleftAttachment, XmATTACH_FORM );
	drawing_area_args.setArg( XmNrightAttachment, XmATTACH_FORM );
	drawing_area_args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
	drawing_area_args.setArg( XmNborderWidth, 1 );
	drawing_area_args.setArg( XmNtraversalOn, 1l );			// input here
	drawing_area_args.setValues( screen.widget );
	}
//------------------------------------------------------------------------------//
//										//
//	Main Window widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_MainWindow::EmacsMotif_MainWindow( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "main_window", xmMainWindowWidgetClass )
	, menu_bar( app, *this )
	, screen_form( app, *this )
	{ }

EmacsMotif_MainWindow::~EmacsMotif_MainWindow()
	{ }

void EmacsMotif_MainWindow::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNbackground, application.resources.n_bg );
	args.setArg( XmNwidth, application.pixelWidth );
	args.setArg( XmNheight, application.pixelHeight );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_MainWindow::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_MenuBar::EmacsMotif_MenuBar( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, XmCreateMenuBar( parent.widget, "menu_bar", NULL, 0 ) )
	{ }

EmacsMotif_MenuBar::~EmacsMotif_MenuBar()
	{ }

static EmacsMotifMenuCascade *menu_bar;
static EmacsMotifMenu *menu_cur;
TerminalControl_GUI *GUI;
void debug_menu_callback( Widget )
	{
//	GUI->application.shell->debugPrintWidgetTree( "click" );
	}

void EmacsMotif_MenuBar::preRealizeInit()
	{
	EmacsMotifMenu *item;

	menu_bar = new EmacsMotifMenuCascade( "menu_bar", 0 );
	menu_bar->menu = widget;

	// need one item on the menu bar otherwise it does not size properly
	item = menu_bar->makeCascade( "File", 10, 'F' );

	EmacsMotif_ActionItem action( debug_menu_callback );
	item->makeButton( "Debug", 10, 'D', action );
	manageWidget();
	}

void EmacsMotif_MenuBar::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_Shell::EmacsMotif_Shell( EmacsMotif_Application &app, Widget shell_widget )
	: EmacsMotif_Core( app, shell_widget )
	, main_window( app, *this )
	{ }

EmacsMotif_Shell::~EmacsMotif_Shell()
	{ }

void EmacsMotif_Shell::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNheightInc, application.fth );
	args.setArg( XmNwidthInc, application.ftw );
	args.setArg( XmNminHeight, application.fth * 10 );
	args.setArg( XmNminWidth, application.ftw * 30 );
	args.setArg( XmNmaxHeight, min( application.fth * MSCREENLENGTH, XDisplayHeight(application.dpy,0 )-30  ) );
	args.setArg( XmNmaxWidth, min( application.ftw * MSCREENWIDTH, XDisplayWidth(application.dpy,0 )-10  ) );

	args.setValues( widget );
	}

void EmacsMotif_Shell::postRealizeInit()
	{
	EmacsXtArgs args;

	/* Set application's window manager resources */
	Pixmap iconPixmap = XCreateBitmapFromData
		(
		application.dpy, XtWindow( widget ),
		(char *)write_scrolls_bits, write_scrolls_width, write_scrolls_height
		);
	if (iconPixmap != None)
		{
		args.setArg( XmNiconPixmap, iconPixmap );

		args.setValues( widget );
		}
	}

//------------------------------------------------------------------------------
EmacsMotif_Application::EmacsMotif_Application()
	: dpy(0)
	, app_context(NULL)
	, shell(NULL)
	, cursors(0)
	{ }

EmacsMotif_Application::~EmacsMotif_Application()
	{ }

const char *EmacsMotif_Application::emacs_class = "BEmacs";

 int motif_argc;
 char **motif_argv;
bool EmacsMotif_Application::initApplication( const EmacsString &display_location )
	{

	{
	const char *class_name = getenv("EMACS_CLASS_NAME");
	if( class_name != NULL )
		emacs_class = class_name;
	}

#define FGC( name, field, value ) \
		{ name, XtCForeground, XtRPixel, sizeof( Pixel ), XtOffset( Resources *, field ), XmRString, value}
#define BGC( name, field, value ) \
		{ name, XtCBackground, XtRPixel, sizeof( Pixel ), XtOffset( Resources *, field ), XmRString, value}
#define STR( name, field, value ) \
		{ name, XtCString, XtRString, sizeof(char * ), XtOffset( Resources *, field ), XmRString, value }

	static XtResource resource_descriptions[] =
		{
		{"displayScrollBars", "DisplayScrollBars", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_scroll_bars ), XmRImmediate,( caddr_t ) 1},
		{"displayStatusBar", "DisplayStatusBar", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_status_bar ), XmRImmediate,( caddr_t ) 1},
		{"displayToolBar", "DisplayToolBar", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_tool_bar ), XmRImmediate,( caddr_t ) 1},
		{"pointerForeground", "PointerForeground", XtRString, sizeof( char * ), XtOffset( Resources *, p_fg ), XtRString, "white"},
		{"pointerBackground", "PointerBackground", XtRString, sizeof( char * ), XtOffset( Resources *, p_bg ), XtRString, "black"},
		{"pointerShape", "PointerShape", XtRInt, sizeof( int ), XtOffset( Resources *, p_shape ), XmRImmediate,( caddr_t ) 0xFFFFFFFF},
		{"textFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, normal_font_name ), XmRString, "-dec-terminal-medium-r-normal--14-*-*-*-c-80-iso8859-1" },
		{"specialFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, special_font_name ), XmRString, "-dec-terminal-medium-r-normal--14-*-*-*-c-80-dec-dectech" },
#ifdef BOLDFONT
		{"boldFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, bold_font_name ), XmRString, "-dec-terminal-bold-r-normal--14-*-*-*-c-80-iso8859-1" },
#endif
		{"rows", "Rows", XtRInt, sizeof( int ), XtOffset( Resources *, rows ), XmRImmediate,( caddr_t ) 40},
		{"columns", "Columns", XtRInt, sizeof( int ), XtOffset( Resources *, columns ), XmRImmediate,( caddr_t ) 80},

		BGC("cursorForeground", c_fg, XtDefaultBackground),
		FGC("cursorBackground", c_bg, XtDefaultForeground),

		FGC("normalForeground", n_fg, "#000000"),
		BGC("normalBackground", n_bg, "#ffffff"),
		STR("normalUnderline", n_underline, "0"),

		FGC("modelineForeground", ml_fg, "#ffffff"),
		BGC("modelineBackground", ml_bg, "#0000ff"),
		STR("modelineUnderline", ml_underline, "0"),

		FGC("highlightForeground", hl_fg, "#ffffff"),
		BGC("highlightBackground", hl_bg, "#808080"),
		STR("highlightUnderline", hl_underline, "0"),

		FGC("wordForeground", word_fg, "#000000"),
		BGC("wordBackground", word_bg, "#ffffff"),
		STR("wordUnderline", word_underline, "0"),

		FGC("stringForeground", string_fg, "#008000"),
		BGC("stringBackground", string_bg, "#ffffff"),
		STR("stringUnderline", string_underline, "0"),

		FGC("commentForeground", comment_fg, "#008000"),
		BGC("commentBackground", comment_bg, "#ffffff"),
		STR("commentUnderline", comment_underline, "0"),

		FGC("keyword1Foreground", kw1_fg, "#0000ff"),
		BGC("keyword1Background", kw1_bg, "#ffffff"),
		STR("keyword1Underline", kw1_underline, "0"),

		FGC("keyword2Foreground", kw2_fg, "#ff0000"),
		BGC("keyword2Background", kw2_bg, "#ffffff"),
		STR("keyword2Underline", kw2_underline, "0"),

		FGC("keyword3Foreground", kw3_fg, "#ff00ff"),
		BGC("keyword3Background", kw3_bg, "#ffffff"),
		STR("keyword3Underline", kw3_underline, "0"),

		FGC("user1Foreground", user1_fg, "#ff0000"),
		BGC("user1Background", user1_bg, "#ffffff"),
		STR("user1Underline", user1_underline, "0"),

		FGC("user2Foreground", user2_fg, "#00ff00"),
		BGC("user2Background", user2_bg, "#ffffff"),
		STR("user2Underline", user2_underline, "0"),

		FGC("user3Foreground", user3_fg, "#0000ff"),
		BGC("user3Background", user3_bg, "#ffffff"),
		STR("user3Underline", user3_underline, "0"),

		FGC("user4Foreground", user4_fg, "#ffff00"),
		BGC("user4Background", user4_bg, "#ffffff"),
		STR("user4Underline", user4_underline, "0"),

		FGC("user5Foreground", user5_fg, "#ff00ff"),
		BGC("user5Background", user5_bg, "#ffffff"),
		STR("user5Underline", user5_underline, "0"),

		FGC("user6Foreground", user6_fg, "#00ffff"),
		BGC("user6Background", user6_bg, "#ffffff"),
		STR("user6Underline", user6_underline, "0"),

		FGC("user7Foreground", user7_fg, "#ffffff"),
		BGC("user7Background", user7_bg, "#c0c0c0"),
		STR("user7Underline", user7_underline, "0"),

		FGC("user8Foreground", user8_fg, "#ffffff"),
		BGC("user8Background", user8_bg, "#555555"),
		STR("user8Underline", user8_underline, "0")
		};

	XtToolkitInitialize();

	app_context = XtCreateApplicationContext();

	XtAppSetErrorHandler( app_context, xt_error_handler );
	XtAppSetWarningHandler( app_context, xt_warning_handler );
	XSetErrorHandler( x_error_handler );

	static String fallback_resources[] =
		{
		"*foreground: black",
		"*background: grey75",
		".displayToolBar:	true",
		".displayScrollBars:	true",
		".displayStatusBar:	true",
		".textFont: -adobe-courier-medium-r-normal--14-120-100-100-m-100-iso8859-1",
		//BEmacs.textFont: -dec-terminal-medium-r-normal--14-*-*-*-c-80-iso8859-1
		".specialFont: -dec-terminal-medium-r-normal--14-*-*-*-c-80-dec-dectech",
		//BEmacs.textFont: -dec-terminal-medium-r-normal--20-0-75-75-c-0-iso8859-1
		//BEmacs.specialFont: -dec-terminal-medium-r-normal--20-0-75-75-c-0-dec-dectech
		//#ifdef COLOR
		".normalForeground: black",
		".normalBackground: white",
		".normalUnderline: no",
		".modelineForeground: yellow",
		".modelineBackground: blue",
		".modelineUnderline: no",
		".highlightForeground: black",
		".highlightBackground: grey75",
		".highlightUnderline: no",
		//BEmacs.cursorForeground:
		//BEmacs.cursorBackground:
		//BEmacs.wordForeground:
		//BEmacs.wordBackground:
		//BEmacs.wordUnderline:
		".stringForeground: purple",
		".stringBackground: white",
		".stringUnderline: no",
		".commentForeground: darkgreen",
		".commentBackground: white",
		".commentUnderline: no",
		".keyword1Foreground: blue",
		".keyword1Background: white",
		".keyword1Underline: yes",
		".keyword2Foreground: purple",
		".keyword2Background: white",
		".keyword2Underline: yes",
		".keyword3Foreground: red",
		".keyword3Background: white",
		".keyword3Underline: yes",
		".user1Foreground: red",
		".user1Background: white",
		".user1Underline: yes",
		".user2Foreground: blue",
		".user2Background: white",
		".user2Underline: no",
		".user3Foreground: green",
		".user3Background: white",
		".user3Underline: no",
		".user4Foreground: red",
		".user4Background: white",
		".user4Underline: no",
		".user5Foreground: black",
		".user5Background: white",
		".user5Underline: no",
		".user6Foreground: black",
		".user6Background: white",
		".user6Underline: no",
		".user7Foreground: black",
		".user7Background: white",
		".user7Underline: no",
		".user8Foreground: black",
		".user8Background: white",
		".user8Underline: no",
		//
		//	Indicators
		//
		"*status_bar_form.?.?.background: white",

		//BEmacs.*.line_number_text.foreground: black
		//BEmacs.*.line_number_text.background: white
		//BEmacs.*.column_number_text.foreground: black
		//BEmacs.*.column_number_text.background: white
		//BEmacs.*.read_only_text.foreground: red
		//BEmacs.*.read_only_text.background: white
		//BEmacs.*.over_strike_text.foreground: blue
		//BEmacs.*.over_strike_text.background: white
		//BEmacs.*.tool_bar_rowcolumn.?.foreground: black
		"*tool_bar_rowcolumn.spacing: 0",


		//
		//	Give F10 back to Emacs as an usable key
		//
		".main_window.menu_bar.menuAccelerator: Alt<Key>F10",
		NULL
		};


	XtAppSetFallbackResources(app_context, fallback_resources);

	EmacsString name( "bemacs" );

	dpy = XtOpenDisplay( app_context, NULL, name.sdataHack(), emacs_class, NULL, 0, &motif_argc, motif_argv );
	if( dpy == NULL )
		{
		_dbg_msg( FormatString( "Cannot open X server at %s" ) <<display_location );
		return false;
		}

	dpy_fd = ConnectionNumber( dpy );

	Widget shell_widget = XtAppCreateShell( "emacs", emacs_class, applicationShellWidgetClass, dpy, NULL, 0 );

	// get the resources
	XtGetApplicationResources( shell_widget, (XtPointer)&resources, resource_descriptions, XtNumber(resource_descriptions), NULL, 0 );

	// Deal with the pointer shape
	if( (pointerFontStruct = XLoadQueryFont( dpy, "decw$cursor" ) ) == NULL )
		{
		_dbg_msg( FormatString( "Display %s is unable to open font: decw$cursor" ) << DisplayString(dpy ) );
		return false;
		}

	if( (unsigned)resources.p_shape != 0xFFFFFFFF )
		{
		XColor fgColor, bgColor;
		if( resources.p_shape & 1 )
			resources.p_shape &= ~1;
		XParseColor(dpy, DefaultColormap( dpy, DefaultScreen(dpy ) ), resources.p_fg, &fgColor );
		XParseColor(dpy, DefaultColormap( dpy, DefaultScreen(dpy ) ), resources.p_bg, &bgColor );
		arrowCursor = XCreateGlyphCursor( dpy, pointerFontStruct->fid, pointerFontStruct->fid,
			resources.p_shape, resources.p_shape + 1, &fgColor, &bgColor );
		cursors++;
		}
#  ifdef ACTIVITY
	watchCursor = XCreateGlyphCursor( dpy, pointerFontStruct->fid, pointerFontStruct->fid, 4, 5, &fgColor, &bgColor );
#  endif

	// Fetch the fonts and calculate their sizes
	if( (normalFontStruct = XLoadQueryFont( dpy, resources.normal_font_name ) ) == NULL )
		{
		_dbg_msg( FormatString( "Display %s is unable to open font: %s" ) << DisplayString(dpy) << resources.normal_font_name );
		return 0;
		}

	// assume we have the special font
	if( (specialFontStruct = XLoadQueryFont( dpy, resources.special_font_name ) ) == NULL )
		{
		specialFontStruct = normalFontStruct;
		_dbg_msg( FormatString( "Display %s is unable to open font: %s" ) << DisplayString(dpy ) << resources.normal_font_name );
		}

	fth = normalFontStruct->ascent + normalFontStruct->descent;
	// average width
	ftw = (normalFontStruct->max_bounds.width + normalFontStruct->min_bounds.width)/2;
	ftd = normalFontStruct->descent;
	innerBorder = 2;

	while( ftw*vertical_bar_width < scroll_bar_min_width )
		vertical_bar_width++;

	// default to 80 col by 24 lines
	pixelWidth = (80+vertical_bar_width) * ftw + innerBorder * 2;
	pixelHeight = 24 * fth + innerBorder * 2;

	shell = new EmacsMotif_Shell( *this, shell_widget );

	return true;
	}

void EmacsMotif_Application::xt_error_handler( char *message )
	{
	_dbg_msg( FormatString("Xt Error: %s") << message );
	debug_invoke();
	}

void EmacsMotif_Application::xt_warning_handler( char *message )
	{
	_dbg_msg( FormatString("Xt Warning: %s") << message );
	debug_invoke();
	}

int EmacsMotif_Application::x_error_handler( Display *dpy, XErrorEvent *err_event )
	{
	_dbg_msg( FormatString("X Error: %d") << err_event->error_code );

	char buf[BUFSIZ];
	XGetErrorText( dpy, err_event->error_code, buf, sizeof buf );
	_dbg_msg( FormatString("X Error: %s") << buf );

	debug_invoke();

	return 0;
	}

void _dbg_msg( const EmacsString &msg )
	{
	printf("%s\n", msg.sdata() );
	}

//ZZZ//
#ifdef __cplusplus
#define PNOTUSED(arg) /* arg */
#else
#define PNOTUSED(arg) arg
#endif

EmacsObject::EmacsObject()
	{ }

EmacsObject::~EmacsObject()
	{ }

void *EmacsObject::operator new( size_t size )
	{
	return malloc( size, malloc_type_char );
	}

void EmacsObject::operator delete( void *p )
	{
	if( p )
		free( p );
	}

#if DBG_ALLOC_CHECK
void *EmacsObject::operator new( size_t size, const char *, int )
	{
	return malloc( size, malloc_type_char );
	}
#endif

int EmacsObject::NextObjectNumber(0);


#undef malloc
#undef realloc
#undef free
# if DBG_ALLOC_CHECK
void *emacs_malloc( int size, enum malloc_block_type, const char *, int )
	{
	return malloc( size );
	}
void *emacs_realloc( void *p, int new_size, enum malloc_block_type, const char *, int )
	{
	return realloc( p, new_size );
	}
# else
void *emacs_malloc( int size, enum malloc_block_type )
	{
	return malloc( size );
	}
void *emacs_realloc( void *p, int new_size, enum malloc_block_type )
	{
	return realloc( p, new_size );
	}
# endif
void emacs_free( void *p )
	{
	free( p );
	}

void emacs_check_malloc_block( void * )
	{
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

void EmacsString::SaveEnvironment()
	{ }

void EmacsStringRepresentation::SaveEnvironment()
	{ }

void FormatString::SaveEnvironment()
	{ }

int emacs_stricmp( const unsigned char *, const unsigned char * )
	{
	return 0;
	}
EmacsString get_config_env( const EmacsString &name )
	{
	char *value = getenv( name );

	if( value != NULL )
		return value;

	return EmacsString::null;
	}


//
//
//	EmacsInitialisation
//
//
bool EmacsInitialisation::init_done = false;

EmacsInitialisation::EmacsInitialisation( const char *, const char *)
	{
	if( init_done )
		return;

	init_done = true;

	EmacsString::init();
	}

EmacsInitialisation::~EmacsInitialisation()
	{ }


int main( int argc, char **argv)
	{
	motif_argc = argc;
	motif_argv = argv;
	TerminalControl_GUI gui("");

	GUI = &gui;

	gui.create_windows( "localhost:0.0" );

	XtAppMainLoop( gui.application.app_context );
	return 0;
	}

TerminalControl_GUI::TerminalControl_GUI( const EmacsString & )
	{
	}

TerminalControl_GUI::~TerminalControl_GUI()
	{
	}

int initializing = 1;

void EmacsMotif_Screen_DrawingArea::expose( void * PNOTUSED(junk), void *event_ )
	{
	XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;
	initializing = 0;
	switch( event->event->type )
		{
	case NoExpose:
		_dbg_msg( "NoExpose" );
		break;

	case GraphicsExpose:
		_dbg_msg( "GraphicsExpose" );
	case Expose:
		_dbg_msg( "Expose" );
		break;

	default:
		_dbg_msg( FormatString( "Unknown exposure event = %d" ) << event->event->type );
		break;
		}
	}

int old_length = -1;
int old_width = -1;
// Handle a resize event from the drawing area widget
void EmacsMotif_Screen_DrawingArea::resize( void *PNOTUSED(junk ), void *PNOTUSED(event_ ) )
	{
	int width  = widget->core.width;
	int height = widget->core.height;

	if( height != application.pixelHeight || width != application.pixelWidth )
		{
		int newHeight = (height - application.innerBorder * 2 ) / application.fth;
		int newWidth = ((width - application.innerBorder * 2) / application.ftw) - vertical_bar_width;

# if DBG_DISPLAY
		if( dbg_flags&DBG_DISPLAY )
			_dbg_msg( FormatString("resize_cb: from height %d to %d, width %d to %d")
				<< old_length << newHeight << old_width << newWidth );
# endif
		old_width = newWidth;
		old_length = newHeight;

		if( newHeight < 5 )
			return;

		application.pixelWidth = (newWidth+vertical_bar_width) * application.ftw + application.innerBorder * 2;
		}
	}
void EmacsMotif_Screen_DrawingArea::input( void *PNOTUSED(junk ), void *event_ )
	{
	XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;

# if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "EmacsMotif_Screen_DrawingArea::input" );
# endif

	switch( event->event->type )
		{
	case KeyPress:
		_dbg_msg( "KeyPress" );
		break;

	case KeyRelease:
		_dbg_msg( "KeyRelease" );
		break;

	case ButtonPress:
	case ButtonRelease:
		_dbg_msg( "ButtonPress/ButtonRelease" );
		break;

	case MotionNotify:
//		_dbg_msg( "MotionNotify" );
		break;
	default:
		_dbg_msg( FormatString( "Unknown input event = %d" ) << event->event->type );
		break;
		}
	}

void EmacsMotifMenu::updateState()
	{ }

void EmacsMotifMenuButton::updateState()
	{
	}

void EmacsMotifMenuCascade::updateState()
	{
	}

void EmacsMotifMenu::addMenuItem( const EmacsMotif_ActionItem &, int )
	{
	error( FormatString( "Parent of button \"%s\" must be a cascade menu") << name );
	}

void EmacsMotifMenuCascade::addMenuItem( const EmacsMotif_ActionItem &action, int index )
	{
	}

EmacsMotifMenu::EmacsMotifMenu( const EmacsString &_name, int _pos )
	: next( NULL )
	, name( _name )
	, pos( _pos )
	, button( NULL )
	{ }

void EmacsMotifMenuCascade::dump( int level )
	{
	}
void EmacsMotifMenuButton::dump( int level )
	{
	}
void EmacsMotifMenuSeparator::dump( int level )
	{
	}

EmacsMotifMenu::~EmacsMotifMenu()
	{
	// delete the whole list of menu items
	delete next;
	if( button )
		XtDestroyWidget( button );
	}

EmacsMotifMenuButton::EmacsMotifMenuButton( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	, action()
	{ }

EmacsMotifMenuButton::~EmacsMotifMenuButton()
	{ }

EmacsMotifMenuCascade::EmacsMotifMenuCascade( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	, children( NULL )
	, menu( NULL )
	, ccount( 0 )
	{ }

EmacsMotifMenuCascade::~EmacsMotifMenuCascade()
	{
	// delete any children
	delete children;
	if( menu )
		XtDestroyWidget( menu );
	}

EmacsMotifMenuSeparator::EmacsMotifMenuSeparator( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	{ }

EmacsMotifMenuSeparator::~EmacsMotifMenuSeparator()
	{ }

EmacsMotifMenu *EmacsMotifMenu::makeButton( const EmacsString &, int, int, const EmacsMotif_ActionItem & )
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}

EmacsMotifMenu *EmacsMotifMenu::makeSeparator( const EmacsString &, int )
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}

EmacsMotifMenu *EmacsMotifMenu::makeCascade( const EmacsString &, int, int)
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}


/* Create a push button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeSeparator
	(
	const EmacsString &button_name, int button_pos
	)
	{
	Arg args[1];
	Widget button;
	EmacsMotifMenuSeparator *new_button = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error( FormatString("Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_button = new EmacsMotifMenuSeparator( button_name, button_pos );
	if( new_button == NULL )
		{
		error( FormatString("No memory to create button \"%s\"") << name );
		return NULL;
		}

	new_button->button = button = XmCreateSeparatorGadget( menu, new_button->name.sdataHack(), args, 0 );
	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > button_pos)
			break;
	menu_cur = cur;
	ccount++;
	new_button->next = cur;
	if( prev != NULL)
		prev->next = new_button;
	else
		children = new_button;

	XtManageChild( button );

	return new_button;
	}

static void menu_to_keys( Widget PNOTUSED(w), void */*button_*/, void * PNOTUSED(info))
	{
	}

/* Create a push button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeButton
	(
	const EmacsString &button_name, int button_pos, int mn,
	const EmacsMotif_ActionItem &action
	)
	{
	Widget button;
	EmacsMotifMenuButton *new_button = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error( FormatString("Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_button = new EmacsMotifMenuButton( button_name, button_pos );
	if( new_button == NULL )
		{
		error( FormatString("No memory to create button \"%s\"") << name );
		return NULL;
		}

	new_button->action = action;

	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > button_pos)
			break;
	menu_cur = cur;

	EmacsXtArgs args;
	if( mn != 0)
		args.setArg( XmNmnemonic, mn );

	if( action.isToggleButton() )
		{
		button = XmCreateToggleButtonGadget( menu, new_button->name.sdataHack(), args, args );
		XtAddCallback( button, XmNvalueChangedCallback, menu_to_keys, (XtPointer)new_button );
		}
	else
		{
		//
		// add space to the left so that menus without toggle buttons line up
		// with the toggle button menu items
		//
		args.setArg( XmNmarginLeft, 12 );

		button = XmCreatePushButtonGadget( menu, new_button->name.sdataHack(), args, args );
		XtAddCallback( button, XmNactivateCallback, menu_to_keys, (XtPointer)new_button );
		}

	new_button->button = button;
	new_button->action.setButton( button, false );

	ccount++;
	new_button->next = cur;
	if( prev != NULL)
		prev->next = new_button;
	else
		children = new_button;
	XtManageChild( button );

	return new_button;
	}

static Cardinal insert_pos( XmRowColumnRec *w)
	{
	Arg args[2];
	int count = 0;
	int i = 0;
	Widget *children, parent, prev;

	if( menu_cur == NULL)
		{
		parent = XtParent( w );
		XtSetArg( args[0], XmNnumChildren, &i );
		i = 0;
		XtGetValues( parent, args, 1 );
		}
	else
		{
		prev = menu_cur->button;
		parent = XtParent( w );
		XtSetArg( args[0], XmNnumChildren, &count );
		XtSetArg( args[1], XmNchildren, &children );
		XtGetValues( parent, args, 2 );

		for( i = 0; i < count; i++)
			if( children[i] == prev)
				break;
		}

	return i;
	}

/* Create a cascade button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeCascade( const EmacsString &cascade_name, int cascade_pos, int mn)
	{
	Arg args[2];
	int argc, is_help;
	Widget pulldown, cascade;
	EmacsMotifMenuCascade *new_cascade = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error(  FormatString( "Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_cascade = new EmacsMotifMenuCascade( cascade_name, cascade_pos );
	if( new_cascade == NULL )
		{
		error( FormatString( "No memory to create cascade button \"%s\"") << cascade_name );
		return NULL;
		}

	is_help = this == menu_bar && cascade_name.caseBlindCompare( "HELP" ) == 0;
	new_cascade->children = NULL;
	if( is_help)
		pos = 0x7FFFFFFF;

	new_cascade->ccount = 0;
	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > cascade_pos)
			break;
	menu_cur = cur;
	argc = 0;
	if( !is_help)
		{
		XtSetArg( args[0], XmNinsertPosition, insert_pos);
		argc++;
		}
	new_cascade->menu = pulldown = XmCreatePulldownMenu( menu, new_cascade->name.sdataHack(), args, argc );
	argc = 0;
	XtSetArg( args[0], XmNsubMenuId, pulldown );
	argc++;
	if( mn != 0)
		{
		XtSetArg( args[argc], XmNmnemonic, mn );
		argc++;
		}
	new_cascade->button = cascade = XmCreateCascadeButtonGadget( menu, new_cascade->name.sdataHack(), args, argc );
	if( is_help)
		{
		XtSetArg( args[0], XmNmenuHelpWidget, new_cascade->button );
		XtSetValues( menu, args, 1 );
		}
	ccount++;
	new_cascade->next = cur;
	if( prev != NULL)
		prev->next = new_cascade;
	else
		children = new_cascade;
	XtManageChild( cascade );

	return new_cascade;
	}

void EmacsMotifMenu::removeMenuItem( int )
	{
	error(  FormatString( "Parent of button \"%s\" must be a cascade menu") << name );
	}

void EmacsMotifMenuCascade::removeMenuItem( int )
	{
	}
