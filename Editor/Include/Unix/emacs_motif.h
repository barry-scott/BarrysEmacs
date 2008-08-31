//
//    emacs_motif.h
//
//    Copyright 1997-2001 (c) Barry A. Scott
//
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
#include <Xm/RowColumnP.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/Form.h>
#include <Xm/DrawingA.h>
#include <Xm/BulletinB.h>
#include <Xm/ScrollBar.h>
#include <Xm/MessageB.h>

//
//    Feature switches
//
#define TOOL_BAR_PRESENT 1
#define STATUS_BAR_PRESENT 1

//
//    Friendly way to handle the Args to Xt intensic functions
//
class EmacsXtArgs
{
public:
    enum { max_args = 20 };
    EmacsXtArgs();
    ~EmacsXtArgs();

    void init();

    void setArg( String name, XtArgVal value );
    void setArg( String name, Widget value );
    void setArg( String name, XtPointer value );

    operator Arg *();
    operator int();

    void setValues( Widget w );
    void getValues( Widget w );
private:
    int num_used;
    Arg args[max_args];
};

//======================================================================//
//                                                                      //
//    Emacs MENU class                                                  //
//                                                                      //
//======================================================================//
class EmacsMotifMenuButton;
class EmacsMotif_ActionItem
{
public:
    EmacsMotif_ActionItem( const EmacsString &_keys );
    EmacsMotif_ActionItem( BoundName *_proc );
    EmacsMotif_ActionItem( const EmacsString &_keys, VariableName *_var );
    EmacsMotif_ActionItem( BoundName *_proc, VariableName *_var );
    EmacsMotif_ActionItem( const EmacsMotif_ActionItem &in );
    EmacsMotif_ActionItem( void (*_callback)(Widget) );
    EmacsMotif_ActionItem( Widget button, bool emulate_toggle );
    EmacsMotif_ActionItem();
    ~EmacsMotif_ActionItem();
    EmacsMotif_ActionItem &operator=( const EmacsMotif_ActionItem &in );
    void queueAction();
    void setButton( Widget button, bool emulate_toggle );
    int doAction();
    EmacsString displayAction();
    bool isSeparator() const;
    bool isToggleButton() const;
    void setState();
private:
    EmacsString keys;
    BoundName *proc;
    VariableName *var;
    void (*callback)(Widget);
    Widget button;
    bool emulate_toggle;        // true if the button is a toggle
};

class EmacsMotifMenu
{
public:
    EmacsMotifMenu( const EmacsString &name, int pos );
    virtual ~EmacsMotifMenu();
    EmacsMotifMenu *next;        // Chain of items
    EmacsString name;        // Name of the button
    int pos;            // menu order information
    Widget button;            // The button widget

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

    EmacsMotif_ActionItem action;        // action to be used when button is pressed

    virtual void dump( int level );
    virtual void updateState();
};

class EmacsMotifMenuCascade : public EmacsMotifMenu
{
public:
    EmacsMotifMenuCascade( const EmacsString &name, int pos );
    virtual ~EmacsMotifMenuCascade();

    EmacsMotifMenu *children;    // Chain of items
    Widget menu;            // Menu pane
    int ccount;            // Count of the number of children present

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
//                                    //
//    Encapsulate the GCs that Emacs needs                //
//                                    //
//======================================================================//
class EmacsMotif_Application;

class EmacsMotif_Attributes
{
public:
    EmacsMotif_Attributes();
    ~EmacsMotif_Attributes();

    void create( EmacsMotif_Application &app, Pixel fg, Pixel bg, char *underline, Window win );

    GC getNormalGc() const;
    GC getSpecialGc() const;
    int getUnderline() const;
private:
    GC gc;        // normal font
    GC special_gc;    // special font
    int underline;
};

class EmacsMotif_AttributeSet
{
public:
    EmacsMotif_AttributeSet();
    ~EmacsMotif_AttributeSet();
    EmacsMotif_Attributes &getAttr( int index );
    void setAttr( int index, EmacsMotif_Attributes &attr );
private:
    void checkIndex( int index ) const;
    enum {attr_array_size = SYNTAX_LAST_BIT<<1};
    EmacsMotif_Attributes attr_array[ attr_array_size ];
};


enum EmacsMotifCallback
{
    EmacsMotifCallback_Expose = 1,
    EmacsMotifCallback_Resize,
    EmacsMotifCallback_Input,
    EmacsMotifCallback_ValueChangedScale,
    EmacsMotifCallback_ValueChangedButton,
    EmacsMotifCallback_Drag,
    EmacsMotifCallback_Activate,
    EmacsMotifCallback_WmProtocol
};

class EmacsMotif_Application;

//
//    THe core of the widget support
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
    // for WM procotol
    virtual void wm_protocol( XtPointer call_data );

    void manageWidget();
    void unmanageWidget();

    // destroy a child of this widget
    virtual bool destroyChildWidget( const EmacsString &name );

    // call all the preRealizeInit() function from this core class down
    void callPreRealizeInit();
    // call all the postRealizeInit() function from this core class down
    void callPostRealizeInit();

#if DBG_DISPLAY
    virtual void debugWidget( const char *label );    // hook for debugging
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
    static void wm_protocol_handler( Widget w, XtPointer client_data, XtPointer call_data );

    //
    //    Member variables
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
    virtual ~EmacsMotif_ScrollBarVertical();

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
    virtual ~EmacsMotif_ScrollBarHorizontal();

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
//                                        //
//    The Emacs Motif Widget Tree Classes                    //
//                                        //
//==============================================================================//


//------------------------------------------------------------------------------//
//                                        //
//    Status Bar                                //
//                                        //
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
    EmacsMotif_TextLabelFrame    line_number_frame;
    EmacsMotif_TextLabelFrame    column_number_frame;
    EmacsMotif_TextLabelFrame    read_only_frame;
    EmacsMotif_TextLabelFrame    over_strike_frame;
    EmacsMotif_TextLabelFrame    record_type_frame;

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
//                                        //
//    Tool Bar                                //
//                                        //
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
//                                        //
//    Drawing Area                                //
//                                        //
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

    EmacsMotif_AttributeSet attr_set;
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
//                                        //
//    Menu Bar                                //
//                                        //
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
//                                        //
//    Main screen form                                //
//                                        //
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
    EmacsMotif_ToolBar_Frame    tool_bar;
#endif
    EmacsMotif_Screen_Frame        screen;
#ifdef STATUS_BAR_PRESENT
    EmacsMotif_StatusBar_Frame    status_bar;
#endif
};


//------------------------------------------------------------------------------//
//                                        //
//    Main Window                                //
//                                        //
//------------------------------------------------------------------------------//
class EmacsMotif_MainWindow : public EmacsMotif_Core
{
public:
    EmacsMotif_MainWindow( EmacsMotif_Application &app, EmacsMotif_Core &parent );
    virtual ~EmacsMotif_MainWindow();

    void preRealizeInit();
    void postRealizeInit();

    // override WM procotol call back
    void wm_protocol( XtPointer call_data );

    EmacsMotif_MenuBar        menu_bar;
    EmacsMotif_Screen_Form        screen_form;
};



// application shell
class EmacsMotif_Shell : public EmacsMotif_Core
{
public:
    EmacsMotif_Shell( EmacsMotif_Application &app, Widget shell_widget );
    ~EmacsMotif_Shell();

    void preRealizeInit();
    void postRealizeInit();

    EmacsMotif_MainWindow        main_window;
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
        Boolean display_scroll_bars;    // True if scroll bars are required
        Boolean display_status_bar;     // true if the status bar is required
        Boolean display_tool_bar;       // true if the tool bar is required
        char *normal_font_name;         // Font for normal text
        char *special_font_name;        // Font for display-non-printing and C1 controls text
#ifdef BOLDFONT
        char *bold_font_name;           // Font for bolded text
#endif
        int rows;               // Number of character rows in the window
        int columns;            // Number of character columns in the window
        Pixel n_fg;             // Foreground for normal text
        Pixel n_bg;             // Background for normal text
        char *n_underline;
        Pixel ml_fg;            // Foreground for mode line text
        Pixel ml_bg;            // Background for mode line text
        char *ml_underline;
        Pixel hl_fg;            // Foreground for highlighted text
        Pixel hl_bg;            // Background for highlighted text
        char *hl_underline;
        Pixel c_fg;             // Foreground for cursor
        Pixel c_bg;             // Background for cursor
        char *c_underline;
        Pixel word_fg;          // word foreground colour
        Pixel word_bg;          // word foreground colour
        char *word_underline;
        Pixel string1_fg;       // string foreground colour
        Pixel string1_bg;       // string foreground colour
        char *string1_underline;
        Pixel string2_fg;       // string foreground colour
        Pixel string2_bg;       // string foreground colour
        char *string2_underline;
        Pixel string3_fg;       // string foreground colour
        Pixel string3_bg;       // string foreground colour
        char *string3_underline;
        Pixel comment1_fg;      // comment foreground colour
        Pixel comment1_bg;      // comment foreground colour
        char *comment1_underline;
        Pixel comment2_fg;      // comment foreground colour
        Pixel comment2_bg;      // comment foreground colour
        char *comment2_underline;
        Pixel comment3_fg;      // comment foreground colour
        Pixel comment3_bg;      // comment foreground colour
        char *comment3_underline;
        Pixel kw1_fg;           // kw1 foreground colour
        Pixel kw1_bg;           // kw1 foreground colour
        char *kw1_underline;
        Pixel kw2_fg;           // kw2 foreground colour
        Pixel kw2_bg;           // kw2 foreground colour
        char *kw2_underline;
        Pixel kw3_fg;           // kw3 foreground colour
        Pixel kw3_bg;           // kw3 foreground colour
        char *kw3_underline;
        Pixel user1_fg;         // user 1 foreground colour
        Pixel user1_bg;
        char *user1_underline;
        Pixel user2_fg;         // user 2 foreground colour
        Pixel user2_bg;
        char *user2_underline;
        Pixel user3_fg;         // user 3 foreground colour
        Pixel user3_bg;
        char *user3_underline;
        Pixel user4_fg;         // user 4 foreground colour
        Pixel user4_bg;
        char *user4_underline;
        Pixel user5_fg;         // user 5 foreground colour
        Pixel user5_bg;
        char *user5_underline;
        Pixel user6_fg;         // user 6 foreground colour
        Pixel user6_bg;
        char *user6_underline;
        Pixel user7_fg;         // user 7 foreground colour
        Pixel user7_bg;
        char *user7_underline;
        Pixel user8_fg;         // user 8 foreground colour
        Pixel user8_bg;
        char *user8_underline;
        char *p_fg;             // Foreground for pointer
        char *p_bg;             // Background for pointer
        int p_shape;            // Pointer shape
    };
    typedef struct _resources Resources;
    EmacsMotif_Application();
    ~EmacsMotif_Application();

     bool initApplication( const EmacsString &display_location );

    Atom clipboardType( const EmacsString &name );

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
private:
    Atom default_clipboard;    // the default selection we use as the clipboard
};

class TerminalControl_GUI : public EmacsView
{
public:
    TerminalControl_GUI( const EmacsString &device );
    virtual ~TerminalControl_GUI();
    void *operator new(size_t size);
    void operator delete(void *p);

    //
    //    Screen control
    //
    virtual void t_topos(int, int);     // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_reset();             // reset terminal (screen is in unkown state, convert it to a known one)
    virtual bool t_update_begin();
    virtual void t_update_end();
    virtual void t_insert_lines(int);   // insert n lines
    virtual void t_delete_lines(int);   // delete n lines
    virtual void t_wipe_line(int);      // erase to the end of the line
    virtual bool t_window(void);
    virtual bool t_window(int);         // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_display_activity(  unsigned char );
    virtual void t_update_line(EmacsLinePtr oldl, EmacsLinePtr newl, int ln);    // Routine to call to update a line
    virtual void t_change_attributes(); // Routine to change attributes
    virtual void t_beep();
    //
    //    Keyboard routines
    //
    virtual void k_check_for_input();   // check for any input
    virtual void t_wipe_screen();
    virtual void t_flash();
    virtual int k_input_event( unsigned char *, unsigned int );

    void keyboardInput( XKeyEvent *event );
    void mouseInput( XButtonEvent *event );
    void mouseMotionInput( XMotionEvent *event );
    void updateWindowTitle( void );
    void viewStatusBar( void );
    void viewToolBar( void );

    void input_char_string( const EmacsString &keys, bool shift );
public:
    void getKeyboardFocus();            // get keyboard focus and pop to top of Z order

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
    int mouse_button_state;             // bit mask of pressed buttons
};

extern TerminalControl_GUI *theMotifGUI;
