//
//  emacs_wxwidgets.h
//
//  Copyright 2008 Barry A. Scott
//
#include "wx/wx.h"

class TerminalControl_wxWidgets : public EmacsView
{
public:
    TerminalControl_wxWidgets( const EmacsString &device );
    virtual ~TerminalControl_wxWidgets();
    void *operator new(size_t size);
    void operator delete(void *p);

    //
    //    Screen control
    //
    virtual void t_topos(int, int);     // move the cursor to the indicated (row,column); (1,1) is the upper left
    virtual void t_reset();             // reset terminal (screen is in unknown state, convert it to a known one)
    virtual bool t_update_begin();
    virtual void t_update_end();
    virtual void t_insert_lines(int);   // insert n lines
    virtual void t_delete_lines(int);   // delete n lines
    virtual void t_wipe_line(int);      // erase to the end of the line
    virtual bool t_window(void);
    virtual bool t_window(int);         // set the screen window so that IDline operations only affect the first n lines of the screen
    virtual void t_display_activity(  unsigned char );
    virtual void t_update_line( EmacsLinePtr oldl, EmacsLinePtr newl, int ln );    // Routine to call to update a line
    virtual void t_change_attributes(); // Routine to change attributes
    virtual void t_beep();
    //
    //    Keyboard routines
    //
    virtual void k_check_for_input();   // check for any input
    virtual void t_wipe_screen();
    virtual void t_flash();
    virtual int k_input_event( unsigned char *, unsigned int );

    void keyboardInput();
    void mouseInput();
    void mouseMotionInput();
    void updateWindowTitle( void );
    void viewStatusBar( void );
    void viewToolBar( void );

    void input_char_string( const EmacsString &keys, bool shift );
public:
    void getKeyboardFocus();            // get keyboard focus and pop to top of Z order

    int ToggleCursor();
    void write_region( int width, int height, int x, int y );
    int create_windows( const EmacsString &displayName );

    void fixup_user_interface(void);
    void fixup_scroll_bars(void);
    void fixup_menus(void);
    void fixup_toolbars(void);

    enum { max_scroll_bars = 32 };

    int last_mouse_x;
    int last_mouse_y;
private:
    int mouse_button_state;             // bit mask of pressed buttons
};

extern TerminalControl_wxWidgets *theWxWidgetsGui;


class EmacsApp: public wxApp
{
public:
    bool OnInit();
    int OnExit();
};

DECLARE_APP(EmacsApp)

// Define a new canvas which can receive some events
class EmacsCanvas: public wxWindow
{
public:
    EmacsCanvas(wxFrame *frame);
    virtual ~EmacsCanvas();

    void OnPaint(wxPaintEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

private:
    wxMenu *m_popupMenu;

    DECLARE_EVENT_TABLE()
};

// Define a new frame
class MainWindow: public wxFrame
{
public:
    EmacsCanvas *canvas;
    MainWindow( wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style );
    virtual ~MainWindow();

    void OnCloseWindow( wxCloseEvent& event );
    void OnChar( wxKeyEvent& event );
    void OnPopup( wxCommandEvent& event );

    void Resize( void );

private:

    wxString m_searchString;
    wxString m_title;

    // Preferences
    void WritePreferences();
    void ReadPreferences();

    // Fonts
    void CreateFonts();
    wxFont *m_normalFont;
    wxFont *m_boldFont;
    wxFont *m_italicFont;

    DECLARE_EVENT_TABLE()
};

// Menu items
enum
{
    EMACS_ABOUT         = wxID_ABOUT,
    EMACS_EXIT          = wxID_EXIT,
    EMACS_PREVIOUS      = wxID_BACKWARD,
    EMACS_COPY          = wxID_COPY,
    EMACS_NEXT          = wxID_FORWARD,
    EMACS_NEXT_MATCH    = wxID_MORE,
    EMACS_BIGGER_TEXT   = wxID_ZOOM_IN,
    EMACS_SMALLER_TEXT  = wxID_ZOOM_OUT,
    EMACS_SEARCH        = wxID_FIND,
    EMACS_MINIMIZE      = wxID_ICONIZE_FRAME
};
