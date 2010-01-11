//
//     Copyright(c) 1982-2001
//        Barry A. Scott
//
//    display_line.cpp
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


//--------------------------------------------------------------------------------
//
//
//    EmacsLinePtr implementation
//
//
//--------------------------------------------------------------------------------

EmacsLinePtr::EmacsLinePtr()
    : m_line(NULL)
{}

EmacsLinePtr::EmacsLinePtr( EmacsLinePtr &ptr )
    : m_line( ptr.m_line )
{
    if( m_line != NULL )
        m_line->ref_count++;
}

EmacsLinePtr::~EmacsLinePtr()
{
    if( m_line )
    {
        m_line->ref_count--;
        if( m_line->ref_count == 0 )
            delete m_line;
    }
}

bool EmacsLinePtr::isNull() const
{
    return m_line == NULL;
}

EmacsLinePtr &EmacsLinePtr::operator=( EmacsLinePtr &in )
{
    if( in.m_line != NULL )
        in.m_line->ref_count++;
    if( m_line )
    {
        m_line->ref_count--;
        if( m_line->ref_count == 0 )
            delete m_line;
    }

    m_line = in.m_line;
    return *this;
}

//--------------------------------------------------------------------------------
//
//
//    EmacsLin implementation
//
//
//--------------------------------------------------------------------------------

EmacsLine *EmacsLinePtr::operator->()
{
    return m_line;
}

void EmacsLinePtr::copyLine( EmacsLinePtr &other )
{
    newLine();
    *m_line = *other.m_line;
}

void EmacsView::copyline(int row)
{
    setpos( row, 1 );
    t_desired_screen[ row ].copyLine( t_phys_screen[ row ] );
    t_desired_screen[ row ]->line_next = NULL;
}


void EmacsLinePtr::newLine()
{
    releaseLine();
    m_line = EMACS_NEW EmacsLine;
}

void EmacsLinePtr::releaseLine()
{
    if( m_line )
    {
        m_line->ref_count--;
        if( m_line->ref_count == 0 )
            delete m_line;
    }

    m_line = NULL;
}

bool EmacsLinePtr::operator==( const EmacsLinePtr &other ) const
{
    return m_line == other.m_line;
}

bool EmacsLinePtr::operator!=( const EmacsLinePtr &other ) const
{
    return m_line != other.m_line;
}

EmacsLine::EmacsLine()
    : line_next( NULL )
    , line_drawcost( 0 )
    , line_length( 0 )
    , _line_hash( 0 )
    , ref_count( 1 )
{ }

EmacsLine::~EmacsLine()
{
    emacs_assert( ref_count == 0 );
}

EmacsLine &EmacsLine::operator=( const EmacsLine &in )
{
    _line_hash = in._line_hash;
    line_next = in.line_next;
    line_drawcost = in.line_drawcost;
    line_length = in.line_length;
    memcpy( line_body, in.line_body, in.line_length*sizeof( DisplayBody_t ) );
    memcpy( line_attr, in.line_attr, in.line_length*sizeof( DisplayAttr_t ) );

    return *this;
}

//
// 'hashline' computes a hash value for a line, unless the hash value
// is already known. This hash code has a few important properties:
//    - it is independant of the number of leading and trailing spaces
//    - it will never be zero
//
// As a side effect, an estimate of the cost of redrawing the line is
// calculated
//
int EmacsLine::calcHashLine()
{
    DisplayAttr_t *a = line_attr;

    if( (a[0] & LINE_ATTR_MODELINE) != 0 )
    {
        line_drawcost = line_length;
        return -200;
    }

    int h = line_length;
    EmacsCharQqq_t *t = line_body;
    EmacsCharQqq_t *l = &line_body[ line_length ];

#if !MEMMAP
    while( --l > t && l[0] == ' ')
        ;
    while( t <= l && t[0] == ' ' && a[0] == 0 )
    { t++; a++; }
#endif
    line_drawcost = (l - t) + 1;

    while( t <= l )
        h = (h << 5) + h + *t++ + *a++;

    return h != 0 ? h : 1;
}
