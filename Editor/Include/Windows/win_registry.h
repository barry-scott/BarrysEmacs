//
//    win_registry.h
//
//    Copyright 1998 (c) Barry A. Scott
//


//
// forward declarations
//
class EmacsWinRegistry;
class EmacsWinPrintOptions;

//
// classes referenced but not defined here
//
class EmacsWinFontRegistryKeys;        // defined in the implementation module
class EmacsWinColoursRegistryKeys;    // defined in the implementation module
class EmacsWinPrintOptionsRegistryKeys;    // defined in the implementation module
class EmacsWinToolBarsRegistryKeys;    // defined in the implementation module

//================================================================================
//
//    EmacsWinFont object fully describes a font for use on windows
//
//================================================================================
class EmacsWinFont
{
public:
    EmacsWinFont();
    EmacsWinFont( const EmacsWinFont &other );
    virtual ~EmacsWinFont();
    EmacsWinFont &operator=( const EmacsWinFont &other );

    //
    //    Properties of a font
    //
    const CString &face() const;
    const CString &face( const CString &new_face);
    const CString &face( const char *new_face );

    int pointSize() const;
    int pointSize( int new_point_size );

    int fontHeight() const;
    int fontHeight( int new_font_height );
protected:
    //
    // the following functions are for use by our friends
    //
    friend class EmacsWinPrintOptions;

    void loadRegistrySettings( const EmacsWinFontRegistryKeys &keys );
    void saveRegistrySettings( const EmacsWinFontRegistryKeys &keys );
private:
    CString        m_face;
    int        m_point_size;
    int        m_font_height;
};


//================================================================================
//
//    EmacsWinColour object holds all the colours being used on the screen
//
//================================================================================
const COLORREF colour_black( RGB( 0, 0, 0 ) );
const COLORREF colour_white( RGB( 255, 255, 255 ) );

class EmacsWinColour
{
public:
    EmacsWinColour();
    EmacsWinColour( const EmacsWinColour &other );
    EmacsWinColour( const CString & );
    virtual ~EmacsWinColour();

public:
    COLORREF foreground;
    COLORREF background;
    BOOL italic;
    BOOL underline;
    CPen *pen;
    EmacsWinColour & operator=( const EmacsString & );
    EmacsWinColour & operator=( const CString & );
    EmacsWinColour & operator=( const char * );
    EmacsWinColour & operator=( const SystemExpressionRepresentationGraphicRendition & );
    EmacsWinColour & operator=( const EmacsWinColour &other );

    EmacsString colourAsString() const;

    void setColour( COLORREF fg, COLORREF bg );
private:
    EmacsWinColour & assign( const EmacsString & );
};


class EmacsWinColours
{
public:
    EmacsWinColours();
    EmacsWinColours( const EmacsWinColours &other );
    virtual ~EmacsWinColours();
    EmacsWinColours &operator=( const EmacsWinColours &other );

    //
    //    The colours as strings
    //
    EmacsWinColour windowText() const;
    EmacsWinColour windowText( const EmacsWinColour &colour );
    EmacsWinColour highlightText() const;
    EmacsWinColour highlightText( const EmacsWinColour &colour );
    EmacsWinColour modeLine() const;
    EmacsWinColour modeLine( const EmacsWinColour &colour );
    EmacsWinColour syntaxKeyword1() const;
    EmacsWinColour syntaxKeyword1( const EmacsWinColour &colour );
    EmacsWinColour syntaxKeyword2() const;
    EmacsWinColour syntaxKeyword2( const EmacsWinColour &colour );
    EmacsWinColour syntaxKeyword3() const;
    EmacsWinColour syntaxKeyword3( const EmacsWinColour &colour );
    EmacsWinColour syntaxWord() const;
    EmacsWinColour syntaxWord( const EmacsWinColour &colour );
    EmacsWinColour syntaxString1() const;
    EmacsWinColour syntaxString1( const EmacsWinColour &colour );
    EmacsWinColour syntaxString2() const;
    EmacsWinColour syntaxString2( const EmacsWinColour &colour );
    EmacsWinColour syntaxString3() const;
    EmacsWinColour syntaxString3( const EmacsWinColour &colour );
    EmacsWinColour syntaxQuote() const;
    EmacsWinColour syntaxQuote( const EmacsWinColour &colour );
    EmacsWinColour syntaxComment1() const;
    EmacsWinColour syntaxComment1( const EmacsWinColour &colour );
    EmacsWinColour syntaxComment2() const;
    EmacsWinColour syntaxComment2( const EmacsWinColour &colour );
    EmacsWinColour syntaxComment3() const;
    EmacsWinColour syntaxComment3( const EmacsWinColour &colour );
    EmacsWinColour user1() const;
    EmacsWinColour user1( const EmacsWinColour &colour );
    EmacsWinColour user2() const;
    EmacsWinColour user2( const EmacsWinColour &colour );
    EmacsWinColour user3() const;
    EmacsWinColour user3( const EmacsWinColour &colour );
    EmacsWinColour user4() const;
    EmacsWinColour user4( const EmacsWinColour &colour );
    EmacsWinColour user5() const;
    EmacsWinColour user5( const EmacsWinColour &colour );
    EmacsWinColour user6() const;
    EmacsWinColour user6( const EmacsWinColour &colour );
    EmacsWinColour user7() const;
    EmacsWinColour user7( const EmacsWinColour &colour );
    EmacsWinColour user8() const;
    EmacsWinColour user8( const EmacsWinColour &colour );
protected:
    //
    // the following functions are for use by our friends
    //
    friend class EmacsWinRegistry;

    void loadRegistrySettings( const EmacsWinColoursRegistryKeys &keys );
    void saveRegistrySettings( const EmacsWinColoursRegistryKeys &keys );
private:
    EmacsWinColour window_text;
    EmacsWinColour highlight_text;
    EmacsWinColour mode_line;
    EmacsWinColour syntax_keyword1;
    EmacsWinColour syntax_keyword2;
    EmacsWinColour syntax_keyword3;
    EmacsWinColour syntax_word;
    EmacsWinColour syntax_string1;
    EmacsWinColour syntax_string2;
    EmacsWinColour syntax_string3;
    EmacsWinColour syntax_quote;
    EmacsWinColour syntax_comment1;
    EmacsWinColour syntax_comment2;
    EmacsWinColour syntax_comment3;
    EmacsWinColour user_1;
    EmacsWinColour user_2;
    EmacsWinColour user_3;
    EmacsWinColour user_4;
    EmacsWinColour user_5;
    EmacsWinColour user_6;
    EmacsWinColour user_7;
    EmacsWinColour user_8;
};

//================================================================================
//
//    EmacsWinPrintOptions
//
//================================================================================
class EmacsWinPrintOptions
{
public:

    EmacsWinPrintOptions();
    EmacsWinPrintOptions( const EmacsWinPrintOptions &other );
    virtual ~EmacsWinPrintOptions();
    EmacsWinPrintOptions &operator=( const EmacsWinPrintOptions &other );

    //
    //    Access and update functions
    //
    const EmacsWinFont &printerDocumentFont();
    const EmacsWinFont &printerDocumentFont( const EmacsWinFont &new_font );

    const EmacsWinFont &printerHeaderFont();
    const EmacsWinFont &printerHeaderFont( const EmacsWinFont &new_font );

    const EmacsWinFont &printerFooterFont();
    const EmacsWinFont &printerFooterFont( const EmacsWinFont &new_font );

    int        topMargin() const;
    int        topMargin( int value );
    int        bottomMargin() const;
    int        bottomMargin( int value );
    int        leftMargin() const;
    int        leftMargin( int value );
    int        rightMargin() const;
    int        rightMargin( int value );
    bool        lineNumbers() const;
    bool        lineNumbers( bool value );
    bool        borders() const;
    bool        borders( bool value );
    bool        header() const;
    bool        header( bool value );
    bool        footer() const;
    bool        footer( bool value );
    bool        syntaxColour() const;
    bool        syntaxColour( bool value );
    bool        inColour() const;
    bool        inColour( bool value );
    bool        alwaysWrapLongLines() const;
    bool        alwaysWrapLongLines( bool value );

    const EmacsString &headerFormat() const;
    const EmacsString &headerFormat( const EmacsString &value );
    const EmacsString &footerFormat() const;
    const EmacsString &footerFormat( const EmacsString &value );

protected:
    //
    // the following functions are for use by our friends
    //
    friend class EmacsWinRegistry;

    void loadRegistrySettings( const EmacsWinPrintOptionsRegistryKeys &keys );
    void saveRegistrySettings( const EmacsWinPrintOptionsRegistryKeys &keys );
private:
    int        m_margin_top;
    int        m_margin_bottom;
    int        m_margin_left;
    int        m_margin_right;

    bool        m_line_numbers;
    bool        m_print_borders;
    bool        m_print_header;
    bool        m_print_footer;
    bool        m_print_syntax_colour;
    bool        m_print_in_colour;
    bool        m_always_wrap_long_lines;

    EmacsString    m_header_format;
    EmacsString    m_footer_format;

    EmacsWinFont    m_printer_document_font;
    EmacsWinFont    m_printer_header_font;
    EmacsWinFont    m_printer_footer_font;
};

//================================================================================
//
//    EmacsWinToolBars
//
//================================================================================
enum toolbar_style_t
{
    toolbar_style__mfc = 0,
    toolbar_style__ie4 = 1
};

class EmacsWinToolBarsOptions
{
public:

    EmacsWinToolBarsOptions();
    EmacsWinToolBarsOptions( const EmacsWinToolBarsOptions &other );
    virtual ~EmacsWinToolBarsOptions();
    EmacsWinToolBarsOptions &operator=( const EmacsWinToolBarsOptions &other );

    BOOL isVisible( UINT id );
    BOOL isVisible( UINT id, BOOL visible );

    toolbar_style_t toolbarStyle();
    toolbar_style_t toolbarStyle( toolbar_style_t new_style );

protected:
    //
    // the following functions are for use by our friends
    //
    friend class EmacsWinRegistry;

    void loadRegistrySettings( const EmacsWinToolBarsRegistryKeys &keys );
    void saveRegistrySettings( const EmacsWinToolBarsRegistryKeys &keys );
private:
    toolbar_style_t    m_style;

    enum { NUM_TOOLBARS=20 };    // room for lots
    BOOL m_visible[NUM_TOOLBARS];
};

//================================================================================
//
//    EmacsWinRegistry is the interface to the windows registry.
//    Use objects of this class to read and write Emacs's settings
//    from the registry.
//
//================================================================================
class EmacsWinRegistry
{
public:
    EmacsWinRegistry();
    virtual ~EmacsWinRegistry();


    //
    //    Registry control
    //
    void loadRegistrySettings();
    void saveRegistrySettings();


    //
    //    Access and update functions
    //
    const EmacsWinPrintOptions &printOptions();
    const EmacsWinPrintOptions &printOptions( const EmacsWinPrintOptions &new_print_options );

    const EmacsWinColours &displayColours();
    const EmacsWinColours &displayColours( const EmacsWinColours &new_colours );

    const EmacsWinToolBarsOptions &toolBars();
    const EmacsWinToolBarsOptions &toolBars( const EmacsWinToolBarsOptions &new_toolbars );

    //
    //    Convenience functions
    //
    enum units { units__inch = 1, units__mm };
    static units measurementUnits();
private:
    EmacsWinPrintOptions    m_print_options;
    EmacsWinColours        m_display_colours;
    EmacsWinToolBarsOptions    m_toolbars;

    // these two are not implemented - just hidden to prevent compiler default being generated
    EmacsWinRegistry &operator=( const EmacsWinRegistry & );
    EmacsWinRegistry( const EmacsWinRegistry & );
};
