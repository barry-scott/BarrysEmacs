//
//    win_registry.cpp
//
//    Copyright 1998 (c) Barry A. Scott
//
#include <emacs.h>

#include <win_emacs.h>
#include <win_registry.h>

//================================================================================
//
//    EmacsWinFontRegistryKeys
//
//================================================================================
class EmacsRegistryKeysBase
{
protected:
    EmacsRegistryKeysBase( const char *_key )
        : key( _key )
    { }
    ~EmacsRegistryKeysBase()
    { }

    const char    *key;
public:

    CString loadStr( const char *val_name, const char *default_value ) const
    {
        return theApp.GetProfileString( key, val_name, default_value );
    }
    int    loadInt( const char *val_name, int default_value ) const
    {
        return theApp.GetProfileInt( key, val_name, default_value );
    }
    bool    loadBool( const char *val_name, bool default_value ) const
    {
        return theApp.GetProfileInt( key, val_name, default_value ) != 0;
    }

    void saveStr( const char *val_name, const char *value ) const
    {
        theApp.WriteProfileString( key, val_name, value );
    }
    void saveInt( const char *val_name, int value ) const
    {
        theApp.WriteProfileInt( key, val_name, value );
    }
    void saveBool( const char *val_name, bool value ) const
    {
        theApp.WriteProfileInt( key, val_name, value );
    }
};

//----------------------------------------------------------------------
//
// EmacsWinFontRegistryKeys
//
//----------------------------------------------------------------------
class EmacsWinFontRegistryKeys : public EmacsRegistryKeysBase
{
public:
    EmacsWinFontRegistryKeys
        (
        const char *_key,
        const char *_val_font_name,
        const char *_val_font_height,
        const char *_val_point_size
        )
        : EmacsRegistryKeysBase( _key )
        , val_font_name( _val_font_name )
        , val_font_height( _val_font_height )
        , val_point_size( _val_point_size )
        , def_printer_font_name( "Courier New" )
        , def_printer_font_size( -12 )
        , def_printer_point_size( 90 )
    { }
    ~EmacsWinFontRegistryKeys() {}

    // key and value names
    const char    *val_font_name;
    const char    *val_font_height;
    const char    *val_point_size;

    const char    *def_printer_font_name;
    const int    def_printer_font_size;
    const int    def_printer_point_size;
};


//----------------------------------------------------------------------
//
// EmacsWinColoursRegistryKeys
//
//----------------------------------------------------------------------
class EmacsWinColoursRegistryKeys : public EmacsRegistryKeysBase
{
public:
    EmacsWinColoursRegistryKeys()
        : EmacsRegistryKeysBase("Colours")
        , val_window_text    ("WindowText")
        , val_highlight_text    ("HighlightText")
        , val_mode_line        ("ModeLine")
        , val_syntax_keyword1    ("syntax_keyword1")
        , val_syntax_keyword2    ("syntax_keyword2")
        , val_syntax_keyword3    ("syntax_keyword3")
        , val_syntax_word    ("syntax_word")
        , val_syntax_string1    ("syntax_string")
        , val_syntax_string2    ("syntax_string2")
        , val_syntax_string3    ("syntax_string3")
        , val_syntax_quote    ("syntax_quote")
        , val_syntax_comment1    ("syntax_comment")
        , val_syntax_comment2    ("syntax_comment2")
        , val_syntax_comment3    ("syntax_comment3")
        , val_user_1        ("user_1")
        , val_user_2        ("user_2")
        , val_user_3        ("user_3")
        , val_user_4        ("user_4")
        , val_user_5        ("user_5")
        , val_user_6        ("user_6")
        , val_user_7        ("user_7")
        , val_user_8        ("user_8")
    {}
    ~EmacsWinColoursRegistryKeys()
    {}

    EmacsWinColour loadColour( const char *val_name, const char *default_value ) const
    {
        return loadStr( val_name, default_value );
    }

    void saveColour( const char *val_name, const EmacsWinColour &colour ) const
    {
        saveStr( val_name, colour.colourAsString() );
    }

    const char    *key;
    const char    *val_window_text;
    const char    *val_highlight_text;
    const char    *val_mode_line;
    const char    *val_syntax_keyword1;
    const char    *val_syntax_keyword2;
    const char    *val_syntax_keyword3;
    const char    *val_syntax_word;
    const char    *val_syntax_string1;
    const char    *val_syntax_string2;
    const char    *val_syntax_string3;
    const char    *val_syntax_quote;
    const char    *val_syntax_comment1;
    const char    *val_syntax_comment2;
    const char    *val_syntax_comment3;
    const char    *val_user_1;
    const char    *val_user_2;
    const char    *val_user_3;
    const char    *val_user_4;
    const char    *val_user_5;
    const char    *val_user_6;
    const char    *val_user_7;
    const char    *val_user_8;
};

//----------------------------------------------------------------------
//
// EmacsWinPrintOptionsRegistryKeys
//
//----------------------------------------------------------------------
class EmacsWinPrintOptionsRegistryKeys :public EmacsRegistryKeysBase
{
public:
    EmacsWinPrintOptionsRegistryKeys()
        : EmacsRegistryKeysBase("Print")
        , document_keys( key,"document_font_name", "document_font_height", "document_point_size" )
        , header_keys( key, "header_font_name", "header_font_height", "header_point_size" )
        , footer_keys( key, "footer_font_name", "footer_font_height", "footer_point_size" )
        , val_margin_top("top_margin")
        , val_margin_bottom("bottom_margin")
        , val_margin_left("left_margin")
        , val_margin_right("right_margin")
        , val_line_numbers("line_numbers")
        , val_borders("borders")
        , val_header("print_header")
        , val_footer("print_footer")
        , val_syntax_colour("print_syntax_colour")
        , val_in_colour("print_in_colour")
        , val_always_wrap_long_lines("always_wrap_long_lines")
        , val_header_format("header_format")
        , val_footer_format("footer_format")
    {}
    ~EmacsWinPrintOptionsRegistryKeys()
    {}

    EmacsWinFontRegistryKeys    document_keys;
    EmacsWinFontRegistryKeys    header_keys;
    EmacsWinFontRegistryKeys    footer_keys;

    const char    *val_margin_top;
    const char    *val_margin_bottom;
    const char    *val_margin_left;
    const char    *val_margin_right;

    const char    *val_line_numbers;
    const char    *val_borders;
    const char    *val_header;
    const char    *val_footer;
    const char    *val_syntax_colour;
    const char    *val_in_colour;
    const char    *val_always_wrap_long_lines;

    const char    *val_header_format;
    const char    *val_footer_format;
};

//----------------------------------------------------------------------
//
// EmacsWinToolBarsRegistryKeys
//
//----------------------------------------------------------------------
struct toolbar_data_t
{
    UINT id;
    char *val_name;
    bool default_visibility;
};

static toolbar_data_t toolbar_data[] =
{
{ ID_VIEW_TOOLBAR_FILE,        "File",        true },
{ ID_VIEW_TOOLBAR_EDIT,        "Edit",        true },
{ ID_VIEW_TOOLBAR_MACROS,    "Macros",    true },
{ ID_VIEW_TOOLBAR_SEARCH,    "Search",    true },
{ ID_VIEW_TOOLBAR_VIEW,        "View",        true },
{ ID_VIEW_TOOLBAR_WINDOW,    "Window",    true },
{ ID_VIEW_TOOLBAR_CASE,        "Case",        true },
{ ID_VIEW_TOOLBAR_REGION,    "Region",    true },
{ ID_VIEW_TOOLBAR_TOOLS,    "Tools",    true },
{ ID_VIEW_TOOLBAR_BUILD,    "Build",    true }
};

static int num_toolbars( sizeof( toolbar_data )/sizeof( toolbar_data_t ) );

class EmacsWinToolBarsRegistryKeys :public EmacsRegistryKeysBase
{
public:
    EmacsWinToolBarsRegistryKeys()
        : EmacsRegistryKeysBase("ToolBars")
        , val_style("style")
    {}
    ~EmacsWinToolBarsRegistryKeys()
    {}

    static int idToIndex( UINT id );
    const char *val_style;
};

int EmacsWinToolBarsRegistryKeys::idToIndex( UINT id )
{
    for( int index=0; index<num_toolbars; index++ )
        if( id == toolbar_data[index].id )
            return index;

    emacs_assert(false);
    return 0;
}

//================================================================================
//
//    EmacsWinRegistry
//
//================================================================================

static EmacsWinColoursRegistryKeys        colours_keys;
static EmacsWinPrintOptionsRegistryKeys        print_keys;
static EmacsWinToolBarsRegistryKeys        toolbars_keys;

EmacsWinRegistry::EmacsWinRegistry()
    : m_print_options()
    , m_display_colours()
    , m_toolbars()
{ }

EmacsWinRegistry::~EmacsWinRegistry()
{ }

// read from registry
void EmacsWinRegistry::loadRegistrySettings()
{
    m_print_options.loadRegistrySettings( print_keys );
    m_display_colours.loadRegistrySettings( colours_keys );
    m_toolbars.loadRegistrySettings( toolbars_keys );
}

void EmacsWinRegistry::saveRegistrySettings()
{
    m_print_options.saveRegistrySettings( print_keys );
    m_display_colours.saveRegistrySettings( colours_keys );
    m_toolbars.saveRegistrySettings( toolbars_keys );
}

const EmacsWinPrintOptions &EmacsWinRegistry::printOptions()
{
    return m_print_options;
}

const EmacsWinPrintOptions &EmacsWinRegistry::printOptions( const EmacsWinPrintOptions &new_font )
{
    m_print_options = new_font;

    return m_print_options;
}


const EmacsWinColours &EmacsWinRegistry::displayColours()
{
    return m_display_colours;
}

const EmacsWinColours &EmacsWinRegistry::displayColours( const EmacsWinColours &new_colours )
{
    m_display_colours = new_colours;

    return m_display_colours;
}

const EmacsWinToolBarsOptions &EmacsWinRegistry::toolBars()
{
    return m_toolbars;
}

const EmacsWinToolBarsOptions &EmacsWinRegistry::toolBars( const EmacsWinToolBarsOptions &new_toolbars )
{
    m_toolbars = new_toolbars;

    return m_toolbars;
}

//================================================================================
//
//    EmacsWinFont object fully describes a font for use on windows
//
//================================================================================

EmacsWinFont::EmacsWinFont()
    : m_face    ( print_keys.document_keys.def_printer_font_name )
    , m_point_size    ( print_keys.document_keys.def_printer_point_size )
    , m_font_height    ( print_keys.document_keys.def_printer_font_size )
{ }

EmacsWinFont::EmacsWinFont( const EmacsWinFont &other )
    : m_face( other.m_face )
    , m_point_size( other.m_point_size )
    , m_font_height( other.m_font_height )
{ }

EmacsWinFont::~EmacsWinFont()
{ }

EmacsWinFont &EmacsWinFont::operator=( const EmacsWinFont &other )
{
    m_face = other.m_face;
    m_point_size = other.m_point_size;
    m_font_height = other.m_font_height;

    return *this;
}

void EmacsWinFont::loadRegistrySettings( const EmacsWinFontRegistryKeys &keys )
{
    face(        keys.loadStr( keys.val_font_name,    keys.def_printer_font_name ) );
    fontHeight( keys.loadInt( keys.val_font_height,    keys.def_printer_font_size ) );
    pointSize(  keys.loadInt( keys.val_point_size,    keys.def_printer_point_size ) );

    // enforce limit
    if( face().GetLength() >= LF_FACESIZE )
        face( keys.def_printer_font_name );
}

void EmacsWinFont::saveRegistrySettings( const EmacsWinFontRegistryKeys &keys )
{
    keys.saveStr( keys.val_font_name,    face() );
    keys.saveInt( keys.val_font_height,    fontHeight() );
    keys.saveInt( keys.val_point_size,    pointSize() );
}


//
//    Properties of a font
//
const CString &EmacsWinFont::face() const
{
    return m_face;
}

const CString &EmacsWinFont::face( const CString &new_face)
{
    m_face = new_face;

    return m_face;
}

const CString &EmacsWinFont::face( const char *new_face)
{
    m_face = new_face;
    return m_face;
}

int EmacsWinFont::pointSize() const
{
    return m_point_size;
}

int EmacsWinFont::pointSize( int new_point_size )
{
    m_point_size = new_point_size;
    return m_point_size;
}

int EmacsWinFont::fontHeight() const
{
    return m_font_height;
}

int EmacsWinFont::fontHeight( int new_font_height )
{
    m_font_height = new_font_height;
    return m_font_height;
}

//================================================================================
//
//    EmacsWinColour
//
//================================================================================

EmacsWinColour::EmacsWinColour()
    : foreground( colour_black )
    , background( colour_white )
    , underline( 0 )
    , italic( 0 )
    , pen( NULL )
{ }

EmacsWinColour::EmacsWinColour( const CString &str )
    : foreground( colour_black )
    , background( colour_white )
    , underline( 0 )
    , italic( 0 )
    , pen( NULL )
{
    assign( EmacsString( EmacsString::copy, (unsigned char *)(LPCTSTR)str, str.GetLength() ) );
}

EmacsWinColour::EmacsWinColour( const EmacsWinColour &other )
    : foreground( other.foreground )
    , background( other.background )
    , underline( other.underline )
    , italic( other.italic )
    , pen( NULL )
{
    if( underline )
        pen = new CPen( PS_SOLID, 0, foreground );
}


EmacsWinColour::~EmacsWinColour()
{
    delete pen;
}

EmacsWinColour & EmacsWinColour::operator=( const EmacsWinColour &other )
{
    delete pen;
    pen = NULL;

    foreground = other.foreground;
    background = other.background;
    underline = other.underline;
    italic = other.italic;

    if( underline )
        pen = new CPen( PS_SOLID, 0, foreground );

    return *this;
}

EmacsWinColour & EmacsWinColour::operator=( const SystemExpressionRepresentationGraphicRendition &_str )
{
    EmacsString str( _str.asString() );
    return assign( str );
}

EmacsWinColour & EmacsWinColour::operator=( const EmacsString &str )
{
    return assign( str );
}

EmacsWinColour & EmacsWinColour::operator=( const CString &str )
{
    return assign( EmacsString( EmacsString::copy, (unsigned char *)(LPCTSTR)str, str.GetLength() ) );
}

EmacsWinColour & EmacsWinColour::operator=( const char *str )
{
    return assign( str );
}

static COLORREF ansi_colours[16] =
{
    RGB(   0,   0,   0 ),    // 0 black
    RGB( 127,   0,   0 ),    // 4 red
    RGB(   0, 127,   0 ),    // 2 green
    RGB( 127, 127,   0 ),    // 6 brown
    RGB(   0,   0, 127 ),    // 1 blue
    RGB( 127,   0, 127 ),    // 5 magenta
    RGB(   0, 127, 127 ),    // 3 cyan
    RGB( 255, 255, 255 ),    // 7 ltgrey

    RGB(   0,   0,   0 ),    // 8 black
    RGB( 255,   0,   0 ),    // 12 lt red
    RGB(   0, 255,   0 ),    // 10 lt green
    RGB( 255, 255,   0 ),    // 14 yellow
    RGB(   0,   0, 255 ),    // 9 lt blue
    RGB( 255,   0, 255 ),    // 13 lt magenta
    RGB(   0, 255, 255 ),    // 11 lt cyan
    RGB( 255, 255, 255 )    // 15 white
};

EmacsWinColour & EmacsWinColour::assign( const EmacsString &str )
{
    int num;
    const unsigned char *p = str;
    int bold = 0;            // default to bold off
    int fg = 0, bg = 7;        // fg = black, bg = white
    int fg_r=0, fg_g=0, fg_b=0;
    int bg_r=0, bg_g=0, bg_b=0;
    int rgb_seen = 0;
    int reverse = 0;        // reverse off

    italic = 0;            // italic text off
    underline = 0;            // underline off

    do
    {
        // skip any separator char
        while( *p == ';' )
            p++;

        // collect the number
        num = 0;
        while( *p && unicode_is_digit( *p ) )
            num = num*10 + *p++ - '0';

        // figure out what the number represents
        if( num >= 30 && num <= 37 )
            fg = num - 30;
        else if( num >= 40 && num <= 47 )
            bg = num - 40;
        else if( num == 1 )
            bold = 1;
        else if( num == 4 )
            underline = 1;
        else if( num == 1 )
            bold = 1;
        else if( num == 5 )    // blink attribute intpreted at italic
            italic = 1;
        else if( num == 7 )
            reverse = 1;
        else if( num >= 6000 && num <= 6255 )
            rgb_seen = 1, bg_b = (num-6000)&255;
        else if( num >= 5000 )
            rgb_seen = 1, bg_g = (num-5000)&255;
        else if( num >= 4000 )
            rgb_seen = 1, bg_r = (num-4000)&255;
        else if( num >= 3000 )
            rgb_seen = 1, fg_b = (num-3000)&255;
        else if( num >= 2000 )
            rgb_seen = 1, fg_g = (num-2000)&255;
        else if( num >= 1000 )
            rgb_seen = 1, fg_r = (num-1000)&255;
    }
    while( *p );

    if( rgb_seen )
    {
        foreground = RGB( fg_r, fg_g, fg_b );
        background = RGB( bg_r, bg_g, bg_b );
    }
    else
    {
        // reverse means swap foreground and background colours
        if( reverse )
        {
            int colour = fg;
            fg = bg;
            bg = colour;
        }

        // now turn the attributes into the rendition value
        foreground = ansi_colours[fg+(bold<<3)];
        background = ansi_colours[bg];
    }

    if( pen != NULL )
    {
        delete pen;
        pen = NULL;
    }
    if( underline )
        pen = new CPen( PS_SOLID, 0, foreground );

    return *this;
}

EmacsString EmacsWinColour::colourAsString() const
{
    EmacsString string;
    string = FormatString("%d;%d;%d;%d;%d;%d") <<
        1000+GetRValue(foreground) <<
        2000+GetGValue(foreground) <<
        3000+GetBValue(foreground) <<
        4000+GetRValue(background) <<
        5000+GetGValue(background) <<
        6000+GetBValue(background);
    if( italic )
        string.append( ";5" );
    if( underline )
        string.append( ";4" );

    return string;
}


void EmacsWinColour::setColour( COLORREF fg, COLORREF bg )
{
    foreground = fg;
    background = bg;

    if( pen != NULL )
    {
        delete pen;
        pen = NULL;
    }

    if( underline )
        pen = new CPen( PS_SOLID, 0, foreground );
}



//================================================================================
//
//    EmacsWinColours
//
//================================================================================
EmacsWinColours::EmacsWinColours()
    : window_text()
    , highlight_text()
    , mode_line()
    , syntax_keyword1()
    , syntax_keyword2()
    , syntax_keyword3()
    , syntax_word()
    , syntax_string1()
    , syntax_string2()
    , syntax_string3()
    , syntax_quote()
    , syntax_comment1()
    , syntax_comment2()
    , syntax_comment3()
    , user_1()
    , user_2()
    , user_3()
    , user_4()
    , user_5()
    , user_6()
    , user_7()
    , user_8()
{ }

EmacsWinColours::EmacsWinColours( const EmacsWinColours &other )
    : window_text( other.window_text )
    , highlight_text( other.highlight_text )
    , mode_line( other.mode_line )
    , syntax_keyword1( other.syntax_keyword1 )
    , syntax_keyword2( other.syntax_keyword2 )
    , syntax_keyword3( other.syntax_keyword3 )
    , syntax_word( other.syntax_word )
    , syntax_string1( other.syntax_string1 )
    , syntax_string2( other.syntax_string2 )
    , syntax_string3( other.syntax_string3 )
    , syntax_quote( other.syntax_quote )
    , syntax_comment1( other.syntax_comment1 )
    , syntax_comment2( other.syntax_comment2 )
    , syntax_comment3( other.syntax_comment3 )
    , user_1( other.user_1 )
    , user_2( other.user_2 )
    , user_3( other.user_3 )
    , user_4( other.user_4 )
    , user_5( other.user_5 )
    , user_6( other.user_6 )
    , user_7( other.user_7 )
    , user_8( other.user_8 )
{ }

EmacsWinColours::~EmacsWinColours()
{ }


EmacsWinColours &EmacsWinColours::operator=( const EmacsWinColours &other )
{
    window_text = other.window_text;
    highlight_text = other.highlight_text;
    mode_line = other.mode_line;
    syntax_keyword1 = other.syntax_keyword1;
    syntax_keyword2 = other.syntax_keyword2;
    syntax_keyword3 = other.syntax_keyword3;
    syntax_word = other.syntax_word;
    syntax_string1 = other.syntax_string1;
    syntax_string2 = other.syntax_string2;
    syntax_string3 = other.syntax_string3;
    syntax_quote = other.syntax_quote;
    syntax_comment1 = other.syntax_comment1;
    syntax_comment2 = other.syntax_comment2;
    syntax_comment3 = other.syntax_comment3;
    user_1 = other.user_1;
    user_2 = other.user_2;
    user_3 = other.user_3;
    user_4 = other.user_4;
    user_5 = other.user_5;
    user_6 = other.user_6;
    user_7 = other.user_7;
    user_8 = other.user_8;

    return *this;
}

void EmacsWinColours::loadRegistrySettings( const EmacsWinColoursRegistryKeys &keys )
{
    windowText    ( keys.loadColour( keys.val_window_text,    window_rendition_default ) );
    highlightText    ( keys.loadColour( keys.val_highlight_text,    region_rendition_default ) );
    modeLine    ( keys.loadColour( keys.val_mode_line,        mode_line_rendition_default ) );
    syntaxKeyword1    ( keys.loadColour( keys.val_syntax_keyword1,    syntax_keyword1_rendition_default ) );
    syntaxKeyword2    ( keys.loadColour( keys.val_syntax_keyword2,    syntax_keyword2_rendition_default ) );
    syntaxKeyword3    ( keys.loadColour( keys.val_syntax_keyword3,    syntax_keyword3_rendition_default ) );
    syntaxWord    ( keys.loadColour( keys.val_syntax_word,    syntax_word_rendition_default ) );
    syntaxString1    ( keys.loadColour( keys.val_syntax_string1,    syntax_string1_rendition_default ) );
    syntaxString2    ( keys.loadColour( keys.val_syntax_string2,    syntax_string2_rendition_default ) );
    syntaxString3    ( keys.loadColour( keys.val_syntax_string3,    syntax_string3_rendition_default ) );
    syntaxQuote    ( keys.loadColour( keys.val_syntax_quote,    syntax_quote_rendition_default ) );
    syntaxComment1    ( keys.loadColour( keys.val_syntax_comment1,    syntax_comment1_rendition_default ) );
    syntaxComment2    ( keys.loadColour( keys.val_syntax_comment2,    syntax_comment2_rendition_default ) );
    syntaxComment3    ( keys.loadColour( keys.val_syntax_comment3,    syntax_comment3_rendition_default ) );
    user1        ( keys.loadColour( keys.val_user_1,        user_1_rendition_default ) );
    user2        ( keys.loadColour( keys.val_user_2,        user_2_rendition_default ) );
    user3        ( keys.loadColour( keys.val_user_3,        user_3_rendition_default ) );
    user4        ( keys.loadColour( keys.val_user_4,        user_4_rendition_default ) );
    user5        ( keys.loadColour( keys.val_user_5,        user_5_rendition_default ) );
    user6        ( keys.loadColour( keys.val_user_6,        user_6_rendition_default ) );
    user7        ( keys.loadColour( keys.val_user_7,        user_7_rendition_default ) );
    user8        ( keys.loadColour( keys.val_user_8,        user_8_rendition_default ) );
}

void EmacsWinColours::saveRegistrySettings( const EmacsWinColoursRegistryKeys &keys )
{
    keys.saveColour( keys.val_window_text,        windowText() );
    keys.saveColour( keys.val_highlight_text,    highlightText() );
    keys.saveColour( keys.val_mode_line,        modeLine() );
    keys.saveColour( keys.val_syntax_keyword1,    syntaxKeyword1() );
    keys.saveColour( keys.val_syntax_keyword2,    syntaxKeyword2() );
    keys.saveColour( keys.val_syntax_keyword3,    syntaxKeyword3() );
    keys.saveColour( keys.val_syntax_word,        syntaxWord() );
    keys.saveColour( keys.val_syntax_string1,    syntaxString1() );
    keys.saveColour( keys.val_syntax_string2,    syntaxString2() );
    keys.saveColour( keys.val_syntax_string3,    syntaxString3() );
    keys.saveColour( keys.val_syntax_quote,        syntaxQuote() );
    keys.saveColour( keys.val_syntax_comment1,    syntaxComment1() );
    keys.saveColour( keys.val_syntax_comment2,    syntaxComment2() );
    keys.saveColour( keys.val_syntax_comment3,    syntaxComment3() );
    keys.saveColour( keys.val_user_2,        user2() );
    keys.saveColour( keys.val_user_3,        user3() );
    keys.saveColour( keys.val_user_4,        user4() );
    keys.saveColour( keys.val_user_5,        user5() );
    keys.saveColour( keys.val_user_6,        user6() );
    keys.saveColour( keys.val_user_7,        user7() );
    keys.saveColour( keys.val_user_8,        user8() );
}


//
//    The colours as strings
//
EmacsWinColour EmacsWinColours::windowText() const
{
    return window_text;
}

EmacsWinColour EmacsWinColours::windowText( const EmacsWinColour &colour )
{
    window_text = colour;
    return window_text;
}

EmacsWinColour EmacsWinColours::highlightText() const
{
    return highlight_text;
}

EmacsWinColour EmacsWinColours::highlightText( const EmacsWinColour &colour )
{
    highlight_text = colour;
    return highlight_text;
}

EmacsWinColour EmacsWinColours::modeLine() const
{
    return mode_line;
}

EmacsWinColour EmacsWinColours::modeLine( const EmacsWinColour &colour )
{
    mode_line = colour;
    return mode_line;
}

EmacsWinColour EmacsWinColours::syntaxKeyword1() const
{
    return syntax_keyword1;
}

EmacsWinColour EmacsWinColours::syntaxKeyword1( const EmacsWinColour &colour )
{
    syntax_keyword1 = colour;
    return syntax_keyword1;
}

EmacsWinColour EmacsWinColours::syntaxKeyword2() const
{
    return syntax_keyword2;
}

EmacsWinColour EmacsWinColours::syntaxKeyword2( const EmacsWinColour &colour )
{
    syntax_keyword2 = colour;
    return syntax_keyword2;
}

EmacsWinColour EmacsWinColours::syntaxKeyword3() const
{
    return syntax_keyword3;
}

EmacsWinColour EmacsWinColours::syntaxKeyword3( const EmacsWinColour &colour )
{
    syntax_keyword3 = colour;
    return syntax_keyword3;
}

EmacsWinColour EmacsWinColours::syntaxWord() const
{
    return syntax_word;
}

EmacsWinColour EmacsWinColours::syntaxWord( const EmacsWinColour &colour )
{
    syntax_word = colour;
    return syntax_word;
}

EmacsWinColour EmacsWinColours::syntaxString1() const
{
    return syntax_string1;
}

EmacsWinColour EmacsWinColours::syntaxString1( const EmacsWinColour &colour )
{
    syntax_string1 = colour;
    return syntax_string1;
}

EmacsWinColour EmacsWinColours::syntaxString2() const
{
    return syntax_string2;
}

EmacsWinColour EmacsWinColours::syntaxString2( const EmacsWinColour &colour )
{
    syntax_string2 = colour;
    return syntax_string2;
}

EmacsWinColour EmacsWinColours::syntaxString3() const
{
    return syntax_string3;
}

EmacsWinColour EmacsWinColours::syntaxString3( const EmacsWinColour &colour )
{
    syntax_string3 = colour;
    return syntax_string3;
}

EmacsWinColour EmacsWinColours::syntaxQuote() const
{
    return syntax_quote;
}

EmacsWinColour EmacsWinColours::syntaxQuote( const EmacsWinColour &colour )
{
    syntax_quote = colour;
    return syntax_quote;
}

EmacsWinColour EmacsWinColours::syntaxComment1() const
{
    return syntax_comment1;
}

EmacsWinColour EmacsWinColours::syntaxComment1( const EmacsWinColour &colour )
{
    syntax_comment1 = colour;
    return syntax_comment1;
}

EmacsWinColour EmacsWinColours::syntaxComment2() const
{
    return syntax_comment2;
}

EmacsWinColour EmacsWinColours::syntaxComment2( const EmacsWinColour &colour )
{
    syntax_comment2 = colour;
    return syntax_comment2;
}

EmacsWinColour EmacsWinColours::syntaxComment3() const
{
    return syntax_comment3;
}

EmacsWinColour EmacsWinColours::syntaxComment3( const EmacsWinColour &colour )
{
    syntax_comment3 = colour;
    return syntax_comment3;
}

EmacsWinColour EmacsWinColours::user1() const
{
    return user_1;
}

EmacsWinColour EmacsWinColours::user1( const EmacsWinColour &colour )
{
    user_1 = colour;
    return user_1;
}

EmacsWinColour EmacsWinColours::user2() const
{
    return user_2;
}

EmacsWinColour EmacsWinColours::user2( const EmacsWinColour &colour )
{
    user_2 = colour;
    return user_2;
}

EmacsWinColour EmacsWinColours::user3() const
{
    return user_3;
}

EmacsWinColour EmacsWinColours::user3( const EmacsWinColour &colour )
{
    user_3 = colour;
    return user_3;
}

EmacsWinColour EmacsWinColours::user4() const
{
    return user_4;
}

EmacsWinColour EmacsWinColours::user4( const EmacsWinColour &colour )
{
    user_4 = colour;
    return user_4;
}

EmacsWinColour EmacsWinColours::user5() const
{
    return user_5;
}

EmacsWinColour EmacsWinColours::user5( const EmacsWinColour &colour )
{
    user_5 = colour;
    return user_5;
}

EmacsWinColour EmacsWinColours::user6() const
{
    return user_6;
}

EmacsWinColour EmacsWinColours::user6( const EmacsWinColour &colour )
{
    user_6 = colour;
    return user_6;
}

EmacsWinColour EmacsWinColours::user7() const
{
    return user_7;
}

EmacsWinColour EmacsWinColours::user7( const EmacsWinColour &colour )
{
    user_7 = colour;
    return user_7;
}

EmacsWinColour EmacsWinColours::user8() const
{
    return user_8;
}

EmacsWinColour EmacsWinColours::user8( const EmacsWinColour &colour )
{
    user_8 = colour;
    return user_8;
}

//================================================================================
//
//    EmacsWinColours
//
//================================================================================
EmacsWinPrintOptions::EmacsWinPrintOptions()
    : m_margin_top( 1440 )
    , m_margin_bottom( 1440 )
    , m_margin_left( 1440 )
    , m_margin_right( 1440 )
    , m_line_numbers( false )
    , m_print_borders( true )
    , m_print_header( true )
    , m_print_footer( true )
    , m_print_syntax_colour( true )
    , m_print_in_colour( false )
    , m_always_wrap_long_lines( true )
    , m_header_format( "File %fn" )
    , m_footer_format( "Page %p" )
    , m_printer_document_font()
    , m_printer_header_font()
    , m_printer_footer_font()
{ }

EmacsWinPrintOptions::EmacsWinPrintOptions( const EmacsWinPrintOptions &other )
    : m_margin_top( other.m_margin_top )
    , m_margin_bottom( other.m_margin_bottom )
    , m_margin_left( other.m_margin_left )
    , m_margin_right( other.m_margin_right )
    , m_line_numbers( other.m_line_numbers )
    , m_print_borders( other.m_print_borders )
    , m_print_header( other.m_print_header )
    , m_print_footer( other.m_print_footer )
    , m_print_syntax_colour( other.m_print_syntax_colour )
    , m_print_in_colour( other.m_print_in_colour )
    , m_always_wrap_long_lines( other.m_always_wrap_long_lines )
    , m_header_format( other.m_header_format )
    , m_footer_format( other.m_footer_format )
    , m_printer_document_font( other.m_printer_document_font )
    , m_printer_header_font( other.m_printer_header_font )
    , m_printer_footer_font( other.m_printer_footer_font )
{ }

EmacsWinPrintOptions::~EmacsWinPrintOptions()
{ }

EmacsWinPrintOptions &EmacsWinPrintOptions::operator=( const EmacsWinPrintOptions &other )
{
    m_margin_top = other.m_margin_top;
    m_margin_bottom = other.m_margin_bottom;
    m_margin_left = other.m_margin_left;
    m_margin_right = other.m_margin_right;
    m_line_numbers = other.m_line_numbers;
    m_print_borders = other.m_print_borders;
    m_print_header = other.m_print_header;
    m_print_footer = other.m_print_footer;
    m_print_syntax_colour = other.m_print_syntax_colour;
    m_print_in_colour = other.m_print_in_colour;
    m_always_wrap_long_lines = other.m_always_wrap_long_lines;
    m_header_format = other.m_header_format;
    m_footer_format = other.m_footer_format;

    m_printer_document_font = other.m_printer_document_font;
    m_printer_header_font = other.m_printer_header_font;
    m_printer_footer_font = other.m_printer_footer_font;

    return *this;
}


int EmacsWinPrintOptions::topMargin() const
{ return m_margin_top; }
int EmacsWinPrintOptions::topMargin( int value )
{ m_margin_top = value; return m_margin_top; }

int EmacsWinPrintOptions::bottomMargin() const
{ return m_margin_bottom; }
int EmacsWinPrintOptions::bottomMargin( int value )
{ m_margin_bottom = value; return m_margin_bottom; }

int EmacsWinPrintOptions::leftMargin() const
{ return m_margin_left; }
int EmacsWinPrintOptions::leftMargin( int value )
{ m_margin_left = value; return m_margin_left; }

int EmacsWinPrintOptions::rightMargin() const
{ return m_margin_right; }
int EmacsWinPrintOptions::rightMargin( int value )
{ m_margin_right = value; return m_margin_right; }

bool EmacsWinPrintOptions::lineNumbers() const
{ return m_line_numbers; }
bool EmacsWinPrintOptions::lineNumbers( bool value )
{ m_line_numbers = value; return m_line_numbers; }

bool EmacsWinPrintOptions::borders() const
{ return m_print_borders; }
bool EmacsWinPrintOptions::borders( bool value )
{ m_print_borders = value; return m_print_borders; }

bool EmacsWinPrintOptions::header() const
{ return m_print_header; }
bool EmacsWinPrintOptions::header( bool value )
{ m_print_header = value; return m_print_header; }

bool EmacsWinPrintOptions::footer() const
{ return m_print_footer; }
bool EmacsWinPrintOptions::footer( bool value )
{ m_print_footer = value; return m_print_footer; }

bool EmacsWinPrintOptions::syntaxColour() const
{ return m_print_syntax_colour; }
bool EmacsWinPrintOptions::syntaxColour( bool value )
{ m_print_syntax_colour = value; return m_print_syntax_colour; }

bool EmacsWinPrintOptions::inColour() const
{ return m_print_in_colour; }
bool EmacsWinPrintOptions::inColour( bool value )
{ m_print_in_colour = value; return m_print_in_colour; }

bool EmacsWinPrintOptions::alwaysWrapLongLines() const
{ return m_always_wrap_long_lines; }
bool EmacsWinPrintOptions::alwaysWrapLongLines( bool value )
{ m_always_wrap_long_lines = value; return m_always_wrap_long_lines; }

const EmacsString &EmacsWinPrintOptions::headerFormat() const
{ return m_header_format; }
const EmacsString &EmacsWinPrintOptions::headerFormat( const EmacsString &value )
{ m_header_format = value; return m_header_format; }

const EmacsString &EmacsWinPrintOptions::footerFormat() const
{ return m_footer_format; }
const EmacsString &EmacsWinPrintOptions::footerFormat( const EmacsString &value )
{ m_footer_format = value; return m_footer_format; }

const EmacsWinFont &EmacsWinPrintOptions::printerDocumentFont()
{
    return m_printer_document_font;
}

const EmacsWinFont &EmacsWinPrintOptions::printerDocumentFont( const EmacsWinFont &new_font )
{
    m_printer_document_font = new_font;

    return m_printer_document_font;
}

const EmacsWinFont &EmacsWinPrintOptions::printerHeaderFont()
{
    return m_printer_header_font;
}

const EmacsWinFont &EmacsWinPrintOptions::printerHeaderFont( const EmacsWinFont &new_font )
{
    m_printer_header_font = new_font;

    return m_printer_header_font;
}

const EmacsWinFont &EmacsWinPrintOptions::printerFooterFont()
{
    return m_printer_footer_font;
}

const EmacsWinFont &EmacsWinPrintOptions::printerFooterFont( const EmacsWinFont &new_font )
{
    m_printer_footer_font = new_font;

    return m_printer_footer_font;
}

void EmacsWinPrintOptions::loadRegistrySettings( const EmacsWinPrintOptionsRegistryKeys &keys )
{
    m_printer_document_font.loadRegistrySettings( keys.document_keys );
    m_printer_header_font.loadRegistrySettings( keys.header_keys );
    m_printer_footer_font.loadRegistrySettings( keys.footer_keys );

    topMargin    ( keys.loadInt( keys.val_margin_top,        topMargin() ) );
    bottomMargin    ( keys.loadInt( keys.val_margin_bottom,        bottomMargin() ) );
    leftMargin    ( keys.loadInt( keys.val_margin_left,        leftMargin() ) );
    rightMargin    ( keys.loadInt( keys.val_margin_right,        rightMargin() ) );

    lineNumbers    ( keys.loadBool( keys.val_line_numbers,        lineNumbers() ) );
    borders        ( keys.loadBool( keys.val_borders,        borders() ) );
    header        ( keys.loadBool( keys.val_header,        header() ) );
    footer        ( keys.loadBool( keys.val_footer,        footer() ) );
    syntaxColour    ( keys.loadBool( keys.val_syntax_colour,    syntaxColour() ) );
    inColour    ( keys.loadBool( keys.val_in_colour,        inColour() ) );
    alwaysWrapLongLines( keys.loadBool( keys.val_always_wrap_long_lines, alwaysWrapLongLines() ) );

    headerFormat    ( makeEmacsString( keys.loadStr( keys.val_header_format,        headerFormat() ) ) );
    footerFormat    ( makeEmacsString( keys.loadStr( keys.val_footer_format,        footerFormat() ) ) );
}

void EmacsWinPrintOptions::saveRegistrySettings( const EmacsWinPrintOptionsRegistryKeys &keys )
{
    m_printer_document_font.saveRegistrySettings( keys.document_keys );
    m_printer_header_font.saveRegistrySettings( keys.header_keys );
    m_printer_footer_font.saveRegistrySettings( keys.footer_keys );

    keys.saveInt( keys.val_margin_top,    topMargin() );
    keys.saveInt( keys.val_margin_bottom,    bottomMargin() );
    keys.saveInt( keys.val_margin_left,    leftMargin() );
    keys.saveInt( keys.val_margin_right,    rightMargin() );

    keys.saveInt( keys.val_line_numbers,    lineNumbers() );
    keys.saveInt( keys.val_borders,        borders() );
    keys.saveInt( keys.val_header,        header() );
    keys.saveInt( keys.val_footer,        footer() );
    keys.saveInt( keys.val_syntax_colour,    syntaxColour() );
    keys.saveInt( keys.val_in_colour,    inColour() );
    keys.saveInt( keys.val_always_wrap_long_lines, alwaysWrapLongLines() );

    keys.saveStr( keys.val_header_format,    headerFormat() );
    keys.saveStr( keys.val_footer_format,    footerFormat() );
}

//================================================================================
//
//    EmacsWinToolBars
//
//================================================================================
EmacsWinToolBarsOptions::EmacsWinToolBarsOptions()
{
    m_style = toolbar_style__mfc;

    for( int index=0; index<num_toolbars; index++ )
        m_visible[index] = false;
}

EmacsWinToolBarsOptions::EmacsWinToolBarsOptions( const EmacsWinToolBarsOptions &other )
{
    m_style = other.m_style;

    for( int index=0; index<num_toolbars; index++ )
        m_visible[index] = other.m_visible[index];
}

EmacsWinToolBarsOptions::~EmacsWinToolBarsOptions()
{ }

EmacsWinToolBarsOptions &EmacsWinToolBarsOptions::operator=( const EmacsWinToolBarsOptions &other )
{
    m_style = other.m_style;

    for( int index=0; index<num_toolbars; index++ )
        m_visible[index] = other.m_visible[index];

    return *this;
}

BOOL EmacsWinToolBarsOptions::isVisible( UINT id )
{
    return m_visible[EmacsWinToolBarsRegistryKeys::idToIndex( id )];
}

BOOL EmacsWinToolBarsOptions::isVisible( UINT id, BOOL visible )
{
    m_visible[EmacsWinToolBarsRegistryKeys::idToIndex( id )] = visible;

    return visible;
}

toolbar_style_t EmacsWinToolBarsOptions::toolbarStyle()
{
    return m_style;
}

toolbar_style_t EmacsWinToolBarsOptions::toolbarStyle( toolbar_style_t new_style )
{
    m_style = new_style;

    return m_style;
}

void EmacsWinToolBarsOptions::loadRegistrySettings( const EmacsWinToolBarsRegistryKeys &keys )
{
    emacs_assert( NUM_TOOLBARS >= num_toolbars );

    m_style = (toolbar_style_t)keys.loadInt( keys.val_style, toolbar_style__ie4 );
    switch( m_style )
    {
    case toolbar_style__ie4:
    case toolbar_style__mfc:
        break;
    default:
        m_style = toolbar_style__mfc;
    }

    for( int index=0; index<num_toolbars; index++ )
        m_visible[index] = keys.loadBool( toolbar_data[index].val_name, toolbar_data[index].default_visibility );
}

void EmacsWinToolBarsOptions::saveRegistrySettings( const EmacsWinToolBarsRegistryKeys &keys )
{
    emacs_assert( NUM_TOOLBARS >= num_toolbars );

    keys.saveInt( keys.val_style, m_style );

    for( int index=0; index<num_toolbars; index++ )
        keys.saveBool( toolbar_data[index].val_name, m_visible[index] != 0 );
}


//================================================================================
//
//    Find the unit of measurement in use by the user
//
//================================================================================

EmacsWinRegistry::units EmacsWinRegistry::measurementUnits()
{
    int measure = 0;

    HKEY hKey;
    long query_status = RegOpenKeyEx
        (
        HKEY_CURRENT_USER,
        _T("Control Panel\\International"),
        0,
        KEY_QUERY_VALUE,
        &hKey
        );
    if( query_status == ERROR_SUCCESS )
    {
        char measure_string[80]; measure_string[0] = '\0';
        DWORD measure_length= sizeof( measure_string );
        DWORD key_type = 0;

        query_status = RegQueryValueEx
            (
            hKey,
            _T("iMeasure"),
            NULL,
            &key_type,
            (unsigned char *)&measure_string[0],
            &measure_length
            );
        if( query_status == ERROR_SUCCESS )
            measure = atoi( measure_string );

        RegCloseKey( hKey );
    }

    switch( measure )
    {
    case 1:    // Old measurement system - inches
    default:
        return units__inch;
    case 0:    // Metric - mm
        return units__mm;
    }
}
