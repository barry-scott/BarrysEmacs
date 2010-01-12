//
//    Copyright (c) 1982-2010
//        Barry A. Scott
//

const int MSCREENWIDTH( 1024 );
const int MSCREENLENGTH( 512 );

//  If highlighted is true then mode line highlighting is done
//  If highlight_start is greater then zero then apply region
//  highlight to the characters between hightlight_start and end.
const int LINE_ATTR_MODELINE( SYNTAX_FIRST_FREE );          // 100
const int LINE_ATTR_USER( SYNTAX_FIRST_FREE<<1 );           // 200

const int LINE_M_ATTR_HIGHLIGHT( SYNTAX_LAST_BIT );         // 400
const int LINE_M_ATTR_USER( LINE_ATTR_USER|(15) );          // the 8 user colours

typedef EmacsChar_t DisplayBody_t;
typedef unsigned short DisplayAttr_t;

class EmacsLine : public EmacsObject
{
    friend class EmacsLinePtr;
public:
    EMACS_OBJECT_FUNCTIONS( EmacsLine )

    int lineHash()
    {
        if( _line_hash == 0 )
            _line_hash = calcHashLine();
        return _line_hash;
    }

    void invalidateHash()
    {
        _line_hash = 0;
    }

    // a line as it appears in a list of
    // lines (as in the physical and virtual display lists)
    EmacsLine *line_next;                       // pointer to the next line in a list of lines
    int line_drawcost;                          // the cost of redrawing this line
    int line_length;                            // the number of valid characters in the line
    DisplayBody_t line_body[ MSCREENWIDTH ];    // the actual text of the line
    DisplayAttr_t line_attr[ MSCREENWIDTH ];    // the actual attributes of the text
private:
    EmacsLine();
    ~EmacsLine();
    EmacsLine &operator=( const EmacsLine & );

    int calcHashLine();
    int _line_hash;                             // hash value for this line, 0 if not known
    int ref_count;
};

class EmacsLinePtr
{
public:
    EmacsLinePtr();
    EmacsLinePtr( EmacsLinePtr &ptr );
    ~EmacsLinePtr();

    EmacsLinePtr &operator=( EmacsLinePtr &in );
    EmacsLine *operator->();
    bool operator==( const EmacsLinePtr &other ) const;
    bool operator!=( const EmacsLinePtr &other ) const;
    bool isNull() const;
    void newLine();
    void releaseLine();
    void copyLine( EmacsLinePtr &other );
private:
    EmacsLine *m_line;
};
