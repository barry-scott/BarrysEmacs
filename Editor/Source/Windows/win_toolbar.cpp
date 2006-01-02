//
//    win_toolbar.h
//        Copyright (c) 1998 Barry A. Scott
//
#include <emacs.h>
#include <win_emacs.h>
#include <win_toolbar.h>
#include <win_registry.h>
#include <win_main_frame.h>
#include <afxcmn.h>

//================================================================================
//
//    static toolbar data
//
//================================================================================

// toolbar buttons - IDs are command buttons
static TBBUTTON tbButtons_old_toolbar[] =
        {
        { 0, ID_STOP_EXECUTION, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        { 1, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 2, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        { 3, ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 4, ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 5, ID_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
        { 6, ID_FILE_PRINT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
        { 7, ID_APP_ABOUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
        };
static TBBUTTON tbButtons_file[] =
{
{ 0, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_FILE_SAVEALL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
{ 3, ID_FILE_PRINT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_edit[] =
{
{ 0, ID_EDIT_CUT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_EDIT_COPY, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_EDIT_PASTE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
{ 3, ID_EDIT_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_macros[] =
{
{ 0, ID_MACROS_RECORD, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_MACROS_STOPRECORDING, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_MACROS_RUN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_build[] =
{
{ 0, ID_BUILD_COMPILE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_BUILD_NEXTERROR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_BUILD_PREVIOUSERROR, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
};

static TBBUTTON tbButtons_case[] =
{
{ 0, ID_EDIT_ADVANCED_CAPITALISE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_EDIT_ADVANCED_UPPERCASE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_EDIT_ADVANCED_LOWERCASE,  TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 3, ID_EDIT_ADVANCED_INVERTCASE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_region[] =
{
{ 0, ID_EDIT_ADVANCED_INDENTREGION, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_EDIT_ADVANCED_UNDENTREGION, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
};

static TBBUTTON tbButtons_search[] =
{
{ 0, ID_SEARCH_REPLACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_BUFFER_CASEBLINDSEARCH, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_view[] =
{
{ 0, ID_BUFFER_SHOWWHITESPACE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_BUFFER_WRAPLONGLINES, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_window[] =
{
{ 0, ID_WINDOW_CASCADE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_WINDOW_TILE_HORZ, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_WINDOW_TILE_VERT, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 0, 0, TBSTATE_ENABLED, TBSTYLE_SEP, 0L, 0},
{ 3, ID_WINDOW_SPLITHORIZONTAL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 4, ID_WINDOW_SPLITVERTICAL, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 5, ID_WINDOW_DELETEOTHERWINDOWS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 6, ID_WINDOW_DELETETHISWINDOW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

static TBBUTTON tbButtons_tools[] =
{
{ 0, ID_TOOLS_GREP_IN_FILES, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 1, ID_TOOLS_GREP_IN_BUFFERS, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0},
{ 2, ID_TOOLS_GREP_CURRENT_BUFFER, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0L, 0}
};

//================================================================================
//
//    EmacsWinToolBars
//
//================================================================================
EmacsWinToolBars::EmacsWinToolBars( CMainFrame &_frame )
    : frame( _frame )
{ }

EmacsWinToolBars::~EmacsWinToolBars()
{ }

//================================================================================
//
//    EmacsWinReBarToolBars
//
//================================================================================
struct ToolbarInfoReBar
{
    DWORD toolbar_id;    // the tool bar ID used to save and restore state
    TBBUTTON *buttons;    // the buttons in this toolbar
    int num_buttons;    // now big buttons is
    int bitmap_id;        // the resource ID of the bitmap
    int bitmap_size;    // the number of button images in the bitmap
};

ToolbarInfoReBar toolbar_info_rebar[] =
{
{ ID_VIEW_TOOLBAR_FILE,  tbButtons_file,   sizeof( tbButtons_file   )/sizeof( TBBUTTON ), IDR_TOOLBAR_FILE,   4},
{ ID_VIEW_TOOLBAR_EDIT,  tbButtons_edit,   sizeof( tbButtons_edit   )/sizeof( TBBUTTON ), IDR_TOOLBAR_EDIT,   4},
{ ID_VIEW_TOOLBAR_CASE,  tbButtons_case,   sizeof( tbButtons_case   )/sizeof( TBBUTTON ), IDR_TOOLBAR_CASE,   4},
{ ID_VIEW_TOOLBAR_MACROS,tbButtons_macros, sizeof( tbButtons_macros )/sizeof( TBBUTTON ), IDR_TOOLBAR_MACROS, 3},
{ ID_VIEW_TOOLBAR_SEARCH,tbButtons_search, sizeof( tbButtons_search )/sizeof( TBBUTTON ), IDR_TOOLBAR_SEARCH, 2},
{ ID_VIEW_TOOLBAR_VIEW,  tbButtons_view,   sizeof( tbButtons_view   )/sizeof( TBBUTTON ), IDR_TOOLBAR_VIEW,   2},
{ ID_VIEW_TOOLBAR_REGION,tbButtons_region, sizeof( tbButtons_region )/sizeof( TBBUTTON ), IDR_TOOLBAR_REGION, 2},
{ ID_VIEW_TOOLBAR_BUILD, tbButtons_build,  sizeof( tbButtons_build  )/sizeof( TBBUTTON ), IDR_TOOLBAR_BUILD,  3},
{ ID_VIEW_TOOLBAR_TOOLS, tbButtons_tools,  sizeof( tbButtons_tools  )/sizeof( TBBUTTON ), IDR_TOOLBAR_TOOLS,  1},
{ ID_VIEW_TOOLBAR_WINDOW,tbButtons_window, sizeof( tbButtons_window )/sizeof( TBBUTTON ), IDR_TOOLBAR_WINDOW, 7}
};
const int num_toolbars_rebar( sizeof( toolbar_info_rebar )/sizeof( ToolbarInfoReBar ) );


EmacsWinReBarToolBars::EmacsWinReBarToolBars( CMainFrame &_frame )
    : EmacsWinToolBars( _frame )
{ }

EmacsWinReBarToolBars::~EmacsWinReBarToolBars()
{ }

bool EmacsWinReBarToolBars::loadToolBars()
{
    if (!m_wndReBar.Create(&frame) )
    {
        TRACE0("Failed to create rebar\n");
        return false;      // fail to create
    }

    EmacsWinRegistry reg;        reg.loadRegistrySettings();

    EmacsWinToolBarsOptions bars( reg.toolBars() );

    for( int toolbar=0; toolbar < num_toolbars_rebar; toolbar++ )
    {
        ToolbarInfoReBar &tbi = toolbar_info_rebar[toolbar];
        if( !m_wndToolBars[toolbar].CreateEx
            (
            &frame,
            TBSTYLE_FLAT | TBSTYLE_TRANSPARENT ,
            WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
            CRect(0, 0, 0, 0),
            tbi.bitmap_id
            )
        )
        {
            TRACE("Failed to createEx toolbar\n");
            return false;        // fail to create
        }

        CToolBarCtrl &ctrl = m_wndToolBars[toolbar].GetToolBarCtrl();
        if( ctrl.AddBitmap( tbi.bitmap_size, tbi.bitmap_id ) == -1 )
        {
            TRACE("Failed to AddBitmap toolbar\n");
            return false;        // fail to create
        }

        if( ctrl.AddButtons( tbi.num_buttons, tbi.buttons ) == -1 )
        {
            TRACE("Failed to AddButton toolbar\n");
            return false;        // fail to create
        }

        REBARBANDINFO rbBand;
        memset( &rbBand, 0, sizeof( REBARBANDINFO ) );
        rbBand.cbSize = sizeof(REBARBANDINFO);
        rbBand.fMask =     RBBIM_STYLE
                | RBBIM_CHILD
                | RBBIM_CHILDSIZE
                | RBBIM_ID
                | RBBIM_IDEALSIZE
                | RBBIM_LPARAM
                | RBBIM_SIZE;

        rbBand.fStyle = RBBS_GRIPPERALWAYS
                | RBBS_FIXEDBMP;

        rbBand.hwndChild = m_wndToolBars[toolbar].m_hWnd;
        rbBand.wID = tbi.toolbar_id;
        rbBand.lParam = tbi.toolbar_id;

        CSize size;
        CControlBar* pTemp = DYNAMIC_DOWNCAST(CControlBar, &m_wndToolBars[toolbar]);
        if (pTemp != NULL)
        {
            size = pTemp->CalcFixedLayout(FALSE, pTemp->GetBarStyle() & CBRS_ORIENT_HORZ);
        }
        else
        {
            CRect rect;
            m_wndToolBars[toolbar].GetWindowRect(&rect);
            size = rect.Size();
        }

        //WINBUG: COMCTL32.DLL is off by 4 pixels in its sizing logic.  Whatever
        //  is specified as the minimum size, the system rebar will allow that band
        //  to be 4 actual pixels smaller!  That's why we add 4 to the size here.
        rbBand.cxMinChild = size.cx + (Emacs_AfxGetComCtlVersion() < VERSION_IE401 ? 4 : 0);
        rbBand.cyMinChild = size.cy;
        rbBand.cxIdeal = rbBand.cxMinChild;
        rbBand.cx = rbBand.cxMinChild;

        if( !m_wndReBar.GetReBarCtrl().InsertBand( unsigned(-1), &rbBand ) )
        {
            TRACE("Failed to InsertBand toolbar\n");
            return false;
        }

        m_wndToolBars[toolbar].SetBarStyle
            (
            m_wndToolBars[toolbar].GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY
            );

        showBand( tbi.toolbar_id, bars.isVisible( tbi.toolbar_id ) );
    }

    return true;
}

void EmacsWinReBarToolBars::saveToolBarsState()
{
    frame.SaveBarState("BarStateV2ReBar");
}

void EmacsWinReBarToolBars::loadToolBarsState()
{
    frame.LoadBarState("BarStateV2ReBar");
}

BOOL EmacsWinReBarToolBars::bandIsVisible( UINT id )
{
    CReBarCtrl &ctrl = m_wndReBar.GetReBarCtrl();
    int index = ctrl.IDToIndex( id );
    if( index == -1 )
        return FALSE;

    REBARBANDINFO info;
    memset( &info, 0, sizeof( info ) );
    info.cbSize = sizeof( info );
    info.fMask = RBBIM_ID|RBBIM_STYLE|RBBIM_LPARAM|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_SIZE;

    if( !ctrl.GetBandInfo( index, &info ) )
        return FALSE;


    OutputDebugString( EmacsString(FormatString("wID: %x, index: %d\n") << info.wID << index) );
    OutputDebugString( EmacsString(FormatString("   fStyle: %x\n") << info.fStyle) );
    OutputDebugString( EmacsString(FormatString("   cx: %d, cxMinChild: %d\n") << info.cx << info.cxMinChild) );


    return (info.fStyle & RBBS_HIDDEN  ) == 0;
}


void EmacsWinReBarToolBars::showBand( UINT id, BOOL visible )
{
    CReBarCtrl &ctrl = m_wndReBar.GetReBarCtrl();

    int index = ctrl.IDToIndex( id );
    if( index == -1 )
        return;

    ctrl.ShowBand( index, visible );
}


//================================================================================
//
//    EmacsWinMfcToolBars
//
//================================================================================
struct ToolbarInfoMfc
{
    DWORD toolbar_id;    // the tool bar ID used to save and restore state
    TBBUTTON *buttons;    // the buttons in this toolbar
    int num_buttons;    // now big buttons is
    int bitmap_id;        // the resource ID of the bitmap
    int bitmap_size;    // the number of button images in the bitmap
};

ToolbarInfoMfc toolbar_info_mfc[] =
{
    // First ID is the one used in pre 7.0.63 versions of bemacs
    // If it is chenged then bemacs crashes inside an assert in MFC
    // when upgrading.
    // so... DON'T change this id
{ ID_VIEW_TOOLBAR,  tbButtons_old_toolbar, sizeof(  tbButtons_old_toolbar )/sizeof( TBBUTTON ), IDR_MAINFRAME, 8},
    // the rest of te ID's match the id codes used in the menu items
    // which allows us to toggle them on and off
{ ID_VIEW_TOOLBAR_FILE,  tbButtons_file, sizeof(  tbButtons_file )/sizeof( TBBUTTON ), IDR_TOOLBAR_FILE, 4},
{ ID_VIEW_TOOLBAR_EDIT,  tbButtons_edit, sizeof(  tbButtons_edit )/sizeof( TBBUTTON ), IDR_TOOLBAR_EDIT, 4},
{ ID_VIEW_TOOLBAR_MACROS,  tbButtons_macros, sizeof(  tbButtons_macros )/sizeof( TBBUTTON ), IDR_TOOLBAR_MACROS, 3},
{ ID_VIEW_TOOLBAR_SEARCH,  tbButtons_search, sizeof(  tbButtons_search )/sizeof( TBBUTTON ), IDR_TOOLBAR_SEARCH, 2},
{ ID_VIEW_TOOLBAR_VIEW,  tbButtons_view, sizeof(  tbButtons_view )/sizeof( TBBUTTON ), IDR_TOOLBAR_VIEW, 2},
{ ID_VIEW_TOOLBAR_WINDOW,  tbButtons_window, sizeof(  tbButtons_window )/sizeof( TBBUTTON ), IDR_TOOLBAR_WINDOW, 7},
{ ID_VIEW_TOOLBAR_CASE,  tbButtons_case, sizeof(  tbButtons_case )/sizeof( TBBUTTON ), IDR_TOOLBAR_CASE, 4},
{ ID_VIEW_TOOLBAR_REGION,  tbButtons_region, sizeof(  tbButtons_region )/sizeof( TBBUTTON ), IDR_TOOLBAR_REGION, 2},
{ ID_VIEW_TOOLBAR_TOOLS,  tbButtons_tools, sizeof(  tbButtons_tools )/sizeof( TBBUTTON ), IDR_TOOLBAR_TOOLS, 1},
{ ID_VIEW_TOOLBAR_BUILD, tbButtons_build, sizeof( tbButtons_build )/sizeof( TBBUTTON ), IDR_TOOLBAR_BUILD, 3}
};
const int num_toolbars_mfc( sizeof( toolbar_info_mfc )/sizeof( ToolbarInfoMfc ) );


EmacsWinMfcToolBars::EmacsWinMfcToolBars( CMainFrame &_frame )
    : EmacsWinToolBars( _frame )
{ }

EmacsWinMfcToolBars::~EmacsWinMfcToolBars()
{ }

bool EmacsWinMfcToolBars::loadToolBars()
{
    EmacsWinRegistry reg;        reg.loadRegistrySettings();

    EmacsWinToolBarsOptions bars( reg.toolBars() );

    for( int toolbar=0; toolbar < num_toolbars_mfc; toolbar++ )
    {
        ToolbarInfoMfc &tbi = toolbar_info_mfc[toolbar];

        // force the old toolbar to be invisible
        bool visible = tbi.toolbar_id == ID_VIEW_TOOLBAR ? false : bars.isVisible( tbi.toolbar_id ) != 0;
        if( !m_wndToolBars[toolbar].Create
            (
            &frame,
            WS_CHILD|CBRS_TOP|CBRS_HIDE_INPLACE| (visible ? WS_VISIBLE : 0),
            tbi.toolbar_id
            )
        )
        {
            TRACE("Failed to create toolbar\n");
            return false;        // fail to create
        }

        m_wndToolBars[toolbar].GetToolBarCtrl().AddBitmap( tbi.bitmap_size, tbi.bitmap_id );
        m_wndToolBars[toolbar].GetToolBarCtrl().AddButtons( tbi.num_buttons, tbi.buttons );

        m_wndToolBars[toolbar].EnableDocking(CBRS_ALIGN_ANY);

        frame.DockControlBar( &m_wndToolBars[toolbar] );

        m_wndToolBars[toolbar].SetBarStyle( m_wndToolBars[toolbar].GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY );
    }

    return true;
}

void EmacsWinMfcToolBars::saveToolBarsState()
{
    frame.SaveBarState("BarStateV2");
}

void EmacsWinMfcToolBars::loadToolBarsState()
{
    frame.LoadBarState("BarStateV2");
}

BOOL EmacsWinMfcToolBars::bandIsVisible( UINT id )
{
    return (frame.GetControlBar( id )->GetStyle() & WS_VISIBLE) != 0;
}

void EmacsWinMfcToolBars::showBand( UINT id, BOOL visible )
{
    frame.ShowControlBar( frame.GetControlBar( id ), visible, FALSE );
}
