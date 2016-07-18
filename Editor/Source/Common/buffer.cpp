//    Copyright (c) 1982-2010
//    Barry A. Scott
//

// Buffer manipulation primitives

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if 0
#define M_dbg_msg( msg ) _dbg_msg( msg )
#define M_dbg_fn_trace( msg ) _dbg_fn_trace t____( msg )
#else
#define M_dbg_msg( msg ) (void)0
#define M_dbg_fn_trace( msg ) (void)0
#endif

void init_bf(void);

#if DBG_BUFFER
static void check_bf(void);
void check_markers( char * s, EmacsBuffer *b = bf_cur );
#endif

// The default values of several buffer-specific variables
SystemExpressionRepresentationString default_mode_format;
SystemExpressionRepresentationIntBoolean default_replace_mode;
SystemExpressionRepresentationIntBoolean default_fold_case;
SystemExpressionRepresentationDefaultRightMargin default_right_margin;
SystemExpressionRepresentationDefaultLeftMargin default_left_margin;
SystemExpressionRepresentationIntPositive default_comment_column;
SystemExpressionRepresentationDefaultTabSize default_tab_size;
SystemExpressionRepresentationIntBoolean default_indent_use_tab;
SystemExpressionRepresentationIntBoolean default_highlight;
SystemExpressionRepresentationIntBoolean default_display_nonprinting;
SystemExpressionRepresentationIntBoolean default_display_eof;
SystemExpressionRepresentationIntBoolean default_display_c1;
SystemExpressionRepresentationIntBoolean default_read_only_buffer;
BoundName *default_auto_fill_proc;
static const EmacsString readonlyerror("Read-only buffer %s cannot be modified");

int bf_journalling;
EmacsBuffer *buffers;    // root of the list of extant buffers
EmacsBuffer *minibuf;    // The minibuf


BufferNameTable EmacsBuffer::name_table( 32, 8 );

//--------------------------------------------------------------------------------
// routines to check the range of values to be stored in System variables
void SystemExpressionRepresentationDefaultTabSize::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value <= 64 )
        exp_int = value;
    else
        throw EmacsExceptionVariableTabOutOfRange();
}
void SystemExpressionRepresentationDefaultTabSize::fetch_value(void)
{ }

//--------------------------------------------------------------------------------
void SystemExpressionRepresentationTabSize::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value <= 64 )
        bf_cur->b_mode.md_tabsize = value;
    else
        throw EmacsExceptionVariableTabOutOfRange();
}
void SystemExpressionRepresentationTabSize::fetch_value(void)
{
    exp_int = bf_cur->b_mode.md_tabsize;
}

//--------------------------------------------------------------------------------
void SystemExpressionRepresentationBufferModified::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    bf_cur->b_modified = value;
}

void SystemExpressionRepresentationBufferModified::fetch_value(void)
{
    exp_int = bf_cur->b_modified;
}

//--------------------------------------------------------------------------------
void SystemExpressionRepresentationDefaultRightMargin::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value > default_left_margin )
        exp_int = value;
    else
        throw EmacsExceptionVariableGreaterThanRange( default_left_margin );

}
void SystemExpressionRepresentationDefaultRightMargin::fetch_value(void)
{ }

void SystemExpressionRepresentationDefaultLeftMargin::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value < default_right_margin )
        exp_int = value;
    else
        throw EmacsExceptionVariableLessThanRange( default_right_margin );
}
void SystemExpressionRepresentationDefaultLeftMargin::fetch_value(void)
{ }

void SystemExpressionRepresentationRightMargin::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value > bf_cur->b_mode.md_leftmargin )
        bf_cur->b_mode.md_rightmargin = value;
    else
        throw EmacsExceptionVariableGreaterThanRange( bf_cur->b_mode.md_leftmargin );
}
void SystemExpressionRepresentationRightMargin::fetch_value(void)
{
    exp_int = bf_cur->b_mode.md_rightmargin;
}

void SystemExpressionRepresentationLeftMargin::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value >= 1 && value < bf_cur->b_mode.md_rightmargin )
        bf_cur->b_mode.md_leftmargin = value;
    else
        throw EmacsExceptionVariableLessThanRange( bf_cur->b_mode.md_rightmargin );
}
void SystemExpressionRepresentationLeftMargin::fetch_value(void)
{
    exp_int = bf_cur->b_mode.md_leftmargin;
}

void SystemExpressionRepresentationIntBufferAllocSize::assign_value( ExpressionRepresentation *new_value )
{
    int value = new_value->asInt();

    if( value > bf_cur->allocatedSize() )
        bf_cur->gap_room( value - bf_cur->unrestrictedSize() );
}

void SystemExpressionRepresentationIntBufferAllocSize::fetch_value()
{
    exp_int = bf_cur->allocatedSize();
}

void SystemExpressionRepresentationDisplayBooleanHighlight::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationDisplayBoolean::assign_value( new_value );

    bf_cur->b_mode.md_highlight = exp_int;
}

void SystemExpressionRepresentationDisplayBooleanHighlight::fetch_value()
{
    exp_int = bf_cur->b_mode.md_highlight;
}

void SystemExpressionRepresentationDisplayBooleanNonPrinting::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationDisplayBoolean::assign_value( new_value );

    bf_cur->b_mode.md_displaynonprinting = exp_int;
}

void SystemExpressionRepresentationDisplayBooleanNonPrinting::fetch_value()
{
    exp_int = bf_cur->b_mode.md_displaynonprinting;
}

void SystemExpressionRepresentationDisplayBooleanEOF::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationDisplayBoolean::assign_value( new_value );

    bf_cur->b_mode.md_display_eof = exp_int;
}

void SystemExpressionRepresentationDisplayBooleanEOF::fetch_value()
{
    exp_int = bf_cur->b_mode.md_display_eof;
}

void SystemExpressionRepresentationDisplayBooleanC1::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationDisplayBoolean::assign_value( new_value );

    bf_cur->b_mode.md_display_c1 = exp_int;
}

void SystemExpressionRepresentationDisplayBooleanC1::fetch_value()
{
    exp_int = bf_cur->b_mode.md_display_c1;
}

void SystemExpressionRepresentationCommentColumn::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntPositive::assign_value( new_value );
    bf_cur->b_mode.md_commentcolumn = exp_int;
}
void SystemExpressionRepresentationCommentColumn::fetch_value()
{
    exp_int = bf_cur->b_mode.md_commentcolumn;
}

void SystemExpressionRepresentationAbbrevOn::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_abbrevon = exp_int;
}
void SystemExpressionRepresentationAbbrevOn::fetch_value()
{
    exp_int = bf_cur->b_mode.md_abbrevon;
}

void SystemExpressionRepresentationIndentUseTab::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_indent_use_tab = exp_int;
}
void SystemExpressionRepresentationIndentUseTab::fetch_value()
{
    exp_int = bf_cur->b_mode.md_indent_use_tab;
}

void SystemExpressionRepresentationFoldCase::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_foldcase = exp_int;
}
void SystemExpressionRepresentationFoldCase::fetch_value()
{
    exp_int = bf_cur->b_mode.md_foldcase;
}

void SystemExpressionRepresentationReplaceMode::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_replace = exp_int;
    redo_modes = 1;
}
void SystemExpressionRepresentationReplaceMode::fetch_value()
{
    exp_int = bf_cur->b_mode.md_replace;
}

void SystemExpressionRepresentationReadOnly::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_readonly = exp_int;
}
void SystemExpressionRepresentationReadOnly::fetch_value()
{
    exp_int = bf_cur->b_mode.md_readonly;
}

void SystemExpressionRepresentationWrapLines::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationIntBoolean::assign_value( new_value );
    bf_cur->b_mode.md_wrap_lines = exp_int;
}
void SystemExpressionRepresentationWrapLines::fetch_value()
{
    exp_int = bf_cur->b_mode.md_wrap_lines;
}

void SystemExpressionRepresentationModeString::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationString::assign_value( new_value );
    bf_cur->b_mode.md_modestring = exp_string;
}
void SystemExpressionRepresentationModeString::fetch_value()
{
    exp_string = bf_cur->b_mode.md_modestring;
}

void SystemExpressionRepresentationModeFormat::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationString::assign_value( new_value );
    bf_cur->b_mode.md_modeformat = exp_string;
}
void SystemExpressionRepresentationModeFormat::fetch_value()
{
    exp_string = bf_cur->b_mode.md_modeformat;
}

void SystemExpressionRepresentationPrefixString::assign_value( ExpressionRepresentation *new_value )
{
    SystemExpressionRepresentationString::assign_value( new_value );
    bf_cur->b_mode.md_prefixstring = exp_string;
}
void SystemExpressionRepresentationPrefixString::fetch_value()
{
    exp_string = bf_cur->b_mode.md_prefixstring;
}


// insert character c at position n in the current buffer
void EmacsBuffer::insert_at( int n, EmacsChar_t c )
{
    if( b_mode.md_readonly )
    {
        error( FormatString(readonlyerror) << b_buf_name );
        return;
    }
    if( n != b_size1 + 1 )
        gap_to( n );
    if( b_gap < 1 )
        if( gap_room( 1 ) )
            return;

    record_insert( n, 1, &c );

    b_size1++;
    b_gap--;
    *bf_cur->ref_char_at( n ) = c;
    if( c == '\n' )
        cant_1line_opt = 1;

    if( b_modified == 0 )
    {
        redo_modes = 1;
        cant_1line_opt = 1;
    }
    if( b_mode.md_syntax_array )
        syntax_insert_update( n, 1 );

    b_modified++;
}


void EmacsBuffer::ins_cstr( const EmacsString &s )
{
    ins_cstr( s.unicode_data(), s.length() );
}

// Insert the N character string S at dot.
void EmacsBuffer::ins_cstr( const unsigned char *s, int n )
{
    for( int i=0; i<n; ++i )
    {
        EmacsChar_t ch[1];
        ch[0] = s[i];
        ins_cstr( ch, 1 );
    }
}

void EmacsBuffer::ins_cstr( const EmacsChar_t *s, int n )
{
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
    if( n == 0 )
        return;

    if( b_mode.md_readonly )
    {
        error( FormatString(readonlyerror) << b_buf_name );
        return;
    }
    if( dot != b_size1 + 1 )
        gap_to( dot );
    if( b_gap < n )
        if( gap_room( n ) )
            return;

    record_insert( dot, n, s );

    // point to first EmacsChar_t in the gap
    memcpy( (void *)&b_base[b_size1], (void *)s, n*sizeof( EmacsChar_t ) );

    cant_1line_opt = 1;        // assume that there was a \n in the string

    b_size1 += n;
    b_gap -= n;

    if( b_mode.md_syntax_array )
        syntax_insert_update( dot, n );

    dot_right( n );

#if DBG_BUFFER
    if( b_gap < 0 )
    {
        error( "Internal Emacs error bf_cur->ins_cstr gap overrun!");
        debug_invoke();
    }
#endif
    if( b_modified == 0 )
    {
        redo_modes = 1;
        cant_1line_opt = 1;
    }
    b_modified++;

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
}

void EmacsBuffer::insert_buffer( EmacsBuffer *buf )
{
    if( buf->b_size1 > 0 )
        ins_cstr( buf->b_base, buf->b_size1 );
    if( buf->b_size2 > 0 )
        ins_cstr( buf->b_base + buf->b_size1 + buf->b_gap, buf->b_size2 );
}

// delete k characters forward from position n in the current buffer
void EmacsBuffer::del_frwd( int n, int k )
{
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
    if( b_mode.md_readonly )
    {
        error( FormatString(readonlyerror) << b_buf_name );
        return;
    }
    if( n != b_size1 + 1 )
        gap_to( n );
    if( k > b_size2 - b_mode.md_tailclip )
        k = b_size2 - b_mode.md_tailclip;
    if( k > 0 )
    {
        record_delete( n, k );
        if( n != dot || k > 1 || bf_cur->char_at( n ) == '\n' )
            cant_1line_opt = 1;
        if( b_modified == 0 )
            cant_1line_opt = 1;
        b_modified++;
        b_gap += k;
        b_size2 -= k;
        if( b_mode.md_syntax_array )
            syntax_delete_update( n, k );

        // adjust markers
        int lim = b_size1 + b_gap;
#if DBG_BUFFER
        if( dbg_flags&DBG_BUFFER )
            check_markers( "del_frwd" );
#endif
        QueueIterator<Marker> it( b_markset );

        while( it.next() )
        {
            Marker *m = it.value();

            if( m->m_pos > b_size1 && m->m_pos <= lim )
            {
                if( m->m_right )
                    m->m_pos = b_size1 + 1 + b_gap;
                else
                    m->m_pos = b_size1 + 1;
                m->m_modified = 1;
            }
        }

#if DBG_BUFFER
        if( dbg_flags&DBG_BUFFER )
            check_markers( "del_frwd" );
#endif
    }

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
}

// delete k characters backward from position n in the current buffer
void EmacsBuffer::del_back( int n, int k )
{
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
    if( b_mode.md_readonly )
    {
        error( FormatString(readonlyerror) << b_buf_name );
        return;
    }
    if( n != b_size1 + 1 )
        gap_to( n );
    if( k > b_size1 - b_mode.md_headclip + 1 )
        k = b_size1 - b_mode.md_headclip + 1;
    if( k > 0 )
    {
        if( n != dot || k > 1 || bf_cur->char_at( n - 1 ) == '\n' )
            cant_1line_opt = 1;
        b_gap = b_gap + k;
        record_delete( n - k, k );
        if( b_modified == 0 )
            cant_1line_opt = 1;
        b_modified++;
        b_size1 = b_size1 - k;
        if( b_mode.md_syntax_array )
            syntax_delete_update( n - k, k );

        // adjust markers
        int lim = b_size1 + b_gap;
#if DBG_BUFFER
        if( dbg_flags&DBG_BUFFER )
            check_markers( "del_back" );
#endif
        QueueIterator<Marker> it( b_markset );

        while( it.next() )
        {
            Marker *m = it.value();

            if( m->m_pos > b_size1 && m->m_pos <= lim )
            {
                if( m->m_right )
                    m->m_pos = b_size1 + 1 + b_gap;
                else
                    m->m_pos = b_size1 + 1;
                m->m_modified = 1;
            }
        }
#if DBG_BUFFER
        if( dbg_flags&DBG_BUFFER )
            check_markers( "del_back" );
#endif
    }

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
}


void EmacsBuffer::bufferExtent( EmacsChar_t *&p1, int &s1, EmacsChar_t *&p2, int &s2 )
{
    p1 = b_base - 1;
    s1 = b_size1;
    p2 = p1 + b_gap;
    s2 = b_size2;
}

// make sure that the gap is not inside the range
// move the gap to the right end if it is
void EmacsBuffer::gap_outside_of_range( int left, int right )
{
    if( left > b_size1+1        // all in part2
    || right < b_size1+1 )        // all in part1
        return;

    // move the gap out of the range
    gap_to( right );
}

// move the gap to position n
void EmacsBuffer::gap_to( int n )
{
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
    int old_s1 = b_size1;

    if( n < 0 )
        n = 0;
    if( n > b_size1 + b_size2 )
        n = b_size1 + b_size2 + 1;
    if( n == b_size1 + 1 )
        return;

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_markers( "gap_to" );
#endif

    if( n <= b_size1 )
    {
        // moving the gap left (into the first part)
        // which is moving data from part1 into part2
        EmacsChar_t *p2 = b_base + b_size1;
        EmacsChar_t *p1 = p2 + b_gap;
        // delt is the amount to decrease the size of size1 by
        int delt = b_size1 - (n-1);
        memmove( (void *)(p1 - delt), (void *)(p2 - delt), delt*sizeof(EmacsChar_t) );

        if( b_syntax.syntax_base != NULL )
        {
            // moving the gap left( into the first part )
            //    p1 = bf_p1+1 + b_size1 + b_gap;
            SyntaxCharData_t *p2 = b_syntax.syntax_base + b_size1;
            SyntaxCharData_t *p1 = p2 + b_gap;
            memmove( (void *)(p1 - delt), (void *)(p2 - delt), delt*sizeof(SyntaxCharData_t) );
        }

        b_size1 -= delt;
        b_size2 += delt;

        QueueIterator<Marker> it( b_markset );

        while( it.next() )
        {
            Marker *m = it.value();

            // adjust markers
            if( m->m_pos == b_size1 + 1
            && m->m_right )
            {
                m->m_pos = m->m_pos + b_gap;
                m->m_modified = 1;
            }
            else if( m->m_pos > b_size1 + 1
            && m->m_pos <= old_s1 + 1 )
            {
                m->m_pos = m->m_pos + b_gap;
                m->m_modified = 1;
            }
        }
    }
    else
    {
        // moving the gap right (into the second part)
        // which is moving data from part2 into part1
        EmacsChar_t *p1 = b_base + b_size1;
        EmacsChar_t *p2 = p1 + b_gap;
        // delt is the amount to increase the size of size1 by
        int delt = (n-1) - b_size1;

        memmove( (void *)p1, (void *)p2, delt*sizeof(EmacsChar_t) );

        if( b_syntax.syntax_base != NULL )
        {
            // moving the gap right( into the second part )
            SyntaxCharData_t *p1 = b_syntax.syntax_base + b_size1;
            SyntaxCharData_t *p2 = p1 + b_gap;
            memmove( (void *)p1, (void *)p2, delt*sizeof(SyntaxCharData_t) );
        }

        b_size1 += delt;
        b_size2 -= delt;

        QueueIterator<Marker> it( b_markset );

        while( it.next() )

        {
            // adjust markers
            Marker *m = it.value();

            if( m->m_pos >= old_s1 + 1 + b_gap
            && m->m_pos < b_size1 + 1 + b_gap )
            {
                m->m_pos = m->m_pos - b_gap;
                m->m_modified = 1;
            }
            else if( m->m_pos == b_size1 + 1 + b_gap
            && ! m->m_right )
            {
                m->m_pos = m->m_pos - b_gap;
                m->m_modified = 1;
            }
        }
    }

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
    {
        check_markers( "gap_to" );
        check_bf();
    }
#endif
}

// make sure that the gap in the current buffer is at least k characters wide
int EmacsBuffer::gap_room(int k)
{
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_bf();
#endif
    if( b_gap >= k )
        return 0;

    int old_gap = b_gap;
    int delt = k + BufferSizeIncrement;
    b_size += delt;

    if( b_base != NULL )
        b_base = (EmacsChar_t *)EMACS_REALLOC( (void *)b_base, b_size*sizeof(EmacsChar_t), malloc_type_char );

    if( b_base == NULL )
    {
        b_size = b_gap = b_size1 = b_size2 = 0;
        error( FormatString("Out of memory! Lost buffer %s") << b_buf_name );
        if( b_syntax.syntax_base != NULL )
        {
            EMACS_FREE( b_syntax.syntax_base );
            b_syntax.syntax_base = NULL;
        }
        return 1;
    }
    if( b_syntax.syntax_base != NULL )
    {
        b_syntax.syntax_base = (SyntaxCharData_t *)EMACS_REALLOC( (void *)b_syntax.syntax_base, (b_size + 1)*sizeof(SyntaxCharData_t), malloc_type_char );
        // I don't like the look of this!
        // b_syntax.syntax_valid = 0;
    }

    if( b_size2 != 0 )
    {
        EmacsChar_t *old_p2_start = b_base + b_size - b_size2 - delt;
        EmacsChar_t *new_p2_start = b_base + b_size - b_size2;

        memmove( (void *)new_p2_start, (void *)old_p2_start, b_size2*sizeof(EmacsChar_t) );

        if( b_syntax.syntax_base != NULL )
        {
            SyntaxCharData_t *old_p2_start = b_syntax.syntax_base + b_size - b_size2 - delt;
            SyntaxCharData_t *new_p2_start = b_syntax.syntax_base + b_size - b_size2;

            memmove( (void *)new_p2_start, (void *)old_p2_start, b_size2*sizeof(SyntaxCharData_t) );
            emacs_check_malloc_block( b_syntax.syntax_base );
        }
    }

    b_gap = b_gap + delt;

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_markers( "gap_room" );
#endif

    QueueIterator<Marker> it( b_markset );

    while( it.next() )
    {
        Marker *m = it.value();
        if( m->m_pos >= b_size1 + old_gap )     // was gtr before right
        {
            m->m_pos = m->m_pos + delt;
            m->m_modified = 1;
        }
    }

#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
    {
        check_markers( "gap_room" );
        check_bf();
    }
#endif
    return 0;
}

ModeSpecific::ModeSpecific()
: md_abbrev( NULL )
, md_syntax( global_syntax_table )
, md_rightmargin( (int)default_right_margin )
, md_leftmargin( (int)default_left_margin )
, md_commentcolumn( (int)default_comment_column )
, md_tabsize( default_tab_size )
, md_indent_use_tab( default_indent_use_tab )
, md_headclip( 1 )
, md_tailclip( 0 )
, md_keys( NULL )
, md_abbrevon( global_abbrev.abbrev_number_defined > 0 )
, md_foldcase( default_fold_case )
, md_replace( default_replace_mode )
, md_highlight( default_highlight )
, md_displaynonprinting( (int)default_display_nonprinting )
, md_display_eof( default_display_eof )
, md_display_c1( default_display_c1 )
, md_readonly( default_read_only_buffer )
, md_wrap_lines( default_wrap_lines )
, md_syntax_colouring( (int)default_syntax_colouring )
, md_syntax_array( default_syntax_array || default_syntax_colouring )
, md_modestring( "Normal" )
, md_modeformat( default_mode_format.asString() )
, md_prefixstring()
, md_auto_fill_proc( default_auto_fill_proc )
, md_process_key_proc( NULL )
{
}


ModeSpecific::~ModeSpecific()
{ }

ModeSpecific &ModeSpecific::operator=( ModeSpecific &from )
{
    md_abbrev               = from.md_abbrev;
    md_syntax               = from.md_syntax;
    md_rightmargin          = from.md_rightmargin;
    md_leftmargin           = from.md_leftmargin;
    md_commentcolumn        = (int)from.md_commentcolumn;
    md_tabsize              = from.md_tabsize;
    md_headclip             = from.md_headclip;
    md_tailclip             = from.md_tailclip;
    md_keys                 = from.md_keys;
    md_abbrevon             = (int)from.md_abbrevon;
    md_foldcase             = (int)from.md_foldcase;
    md_replace              = (int)from.md_replace;
    md_highlight            = (int)from.md_highlight;
    md_displaynonprinting   = (int)from.md_displaynonprinting;
    md_display_eof          = (int)from.md_display_eof;
    md_display_c1           = (int)from.md_display_c1;
    md_readonly             = (int)from.md_readonly;
    md_wrap_lines           = (int)from.md_wrap_lines;
    md_syntax_colouring     = (int)from.md_syntax_colouring;
    md_syntax_array         = (int)from.md_syntax_array;
    md_modestring           = from.md_modestring;
    md_modeformat           = from.md_modeformat;
    md_prefixstring         = from.md_prefixstring;
    md_auto_fill_proc       = from.md_auto_fill_proc;
    md_process_key_proc     = from.md_process_key_proc;

    return *this;
}

syntax_buffer_data::syntax_buffer_data( EmacsBuffer &_buffer )
: syntax_valid( 0 )
, syntax_update_credit( 0 )
, syntax_base( NULL )
, buffer( _buffer )
{}

syntax_buffer_data::~syntax_buffer_data()
{
    if( syntax_base != NULL )
        EMACS_FREE( syntax_base );
}

bool syntax_buffer_data::initBuffer()
{
    if( syntax_base == NULL )
    {
        syntax_base = (SyntaxCharData_t *)EMACS_MALLOC( (buffer.b_size + 1)*sizeof(SyntaxCharData_t), malloc_type_char );
        if( syntax_base == NULL )
            return false;

        // must have a 0 at the start of the array in all fields
        syntax_base[0].data = 0;
        syntax_base[0].outline_depth = 0;
        syntax_base[0].table_number = 0;
    }

    return true;
}


// create a buffer with the given name
EmacsBuffer::EmacsBuffer( const EmacsString &name )
: b_base(NULL)
, b_size1( 0 )
, b_gap( InitialBufferSize )
, b_size2( 0 )
, b_syntax(*this)
, b_line_valid( 0 )
, b_rendition_regions( NULL )
, b_buf_name( name )
, b_size( InitialBufferSize )
, b_ephemeral_dot( 1 )
, b_fname("")
, b_modified( 0 )
, b_backed_up( 0 )
, b_checkpointed( checkpoint_frequency ?  0 : -1 )
, b_checkpointfn("")
, b_next( NULL )
, b_markset()
, b_mark()
, b_gui_input_mode_set_mark( false )
, b_kind( SCRATCHBUFFER )
, b_mode()
, b_eol_attribute( (FIO_EOL_Attribute)(int)default_end_of_line_style )
, b_file_time( 0 )
, b_synch_file_time( 0 )
, b_file_access( 0 )
, b_synch_file_access( 0 )
, b_journal( 0 )
, b_journalling( journalling_frequency != 0 )
{
    b_base = (EmacsChar_t *)EMACS_MALLOC( b_size*sizeof(EmacsChar_t), malloc_type_char );
    if( b_base == NULL )
        b_size = 0;
        // out of memory -- give the error message when
        // we try to enlarge the buffer

    b_next = buffers;
    buffers = this;

    name_table.add( name, this );
}

// find a buffer with the given name -- returns NULL if no such buffer exists
EmacsBuffer *EmacsBuffer::find( const EmacsString &name )
{
    return name_table.find( name );
}

// set the current buffer to p
#ifdef DEBUG_SET_BF
void EmacsBuffer::__set_bf( const char *__file, int __line )
#else
void EmacsBuffer::set_bf()
#endif
{
    M_dbg_fn_trace( "EmacsBuffer::set_bf" );

#ifdef DEBUG_SET_BF
    _dbg_msg( FormatString( "set_bf(%s:%d): New \"%s\", b_ephemeral_dot=%d" ) << __file << __line << b_buf_name << b_ephemeral_dot );
#endif
    cant_1win_opt = 1;

    if( bf_cur != NULL )
        bf_cur->saveGlobalState();

    restoreGlobalState();

#ifdef DEBUG_SET_BF
    if( theActiveView != NULL )
{
    EmacsWindow *w = theActiveView->currentWindow();
    while( w->w_prev != NULL )
        w = w->w_prev;

    while( w != NULL )
    {
        _dbg_msg( FormatString( "set_bf (2) %s c=%d w_dot=%s" ) << w->getDescription() << int(w->isCurrentWindow()) << w->getWindowDot().asString() );
        w = w->w_next;
    }
}

#endif
}

bool EmacsBuffer::isRestricted() const
{
    return b_mode.md_headclip != 1 || b_mode.md_tailclip != 0;
}


void EmacsBuffer::saveGlobalState()
{
    M_dbg_fn_trace( "EmacsBuffer::saveGlobalState" );
    // only save the state if we are the current buffer
    if( this != bf_cur )
        return;

#ifdef DEBUG_SET_BF
    _dbg_msg( FormatString( "saveGlobalState: \"%s\", dot=%d" ) << b_buf_name << dot );
    if( theActiveView != NULL )
{
    EmacsWindow *w = theActiveView->currentWindow();
    while( w->w_prev != NULL )
        w = w->w_prev;

    while( w != NULL )
    {
        _dbg_msg( FormatString( "saveGlobalState: (1) %s c=%d w_dot=%s" ) << w->getDescription() << int(w->isCurrentWindow()) << w->getWindowDot().asString() );
        w = w->w_next;
    }
}

#endif

    if( dot < 1 )
        dot = 1;
    // coerse the dot to be within the buffer
    if( dot > (unrestrictedSize() + 1) )
        dot = (unrestrictedSize() + 1);

    b_ephemeral_dot = dot;

#ifdef DEBUG_SET_BF
    _dbg_msg( FormatString( "saveGlobalState: \"%s\", saved b_ephemeral_dot=%d" ) << b_buf_name << b_ephemeral_dot );
#endif

    if( theActiveView != NULL )
    {
        EmacsWindow *w = theActiveView->currentWindow();

        if( w != NULL && this == w->w_buf )
        {
            w->setWindowDot( Marker( this, dot, 0 ) );
#ifdef DEBUG_SET_BF
            _dbg_msg( FormatString( "saveGlobalState (2): \"%s\", saved %s w_dot=%s w_mark=%s" )
                << b_buf_name << w->getDescription() << w->getWindowDot().asString() << w->getWindowDot().asString() );
#endif
        }
    }

    cant_1line_opt = 1;
}

void EmacsBuffer::restoreGlobalState()
{
    M_dbg_fn_trace( "EmacsBuffer::restoreGlobalState" );
    bf_cur = this;

    // needed for gui input mode to be disabled on a switch
    dot = 0;

    EmacsWindow *w = NULL;
    if( theActiveView != NULL )
        w = theActiveView->currentWindow();

    if(w != NULL && this == w->w_buf)
    {
        set_dot( w->getWindowDot().to_mark() );
#ifdef DEBUG_SET_BF
        _dbg_msg( FormatString( "restoreGlobalState: from %s w_dot \"%s\", dot=%d" )  << w->getDescription() << b_buf_name << dot );
#endif
    }
    else
    {
        set_dot( b_ephemeral_dot );
#ifdef DEBUG_SET_BF
        _dbg_msg( FormatString( "restoreGlobalState: from b_ephemeral \"%s\", dot=%d" ) << b_buf_name << dot );
#endif
    }

#ifdef DEBUG_SET_BF
    if( theActiveView != NULL )
{
    EmacsWindow *w = theActiveView->currentWindow();
    while( w->w_prev != NULL )
        w = w->w_prev;

    while( w != NULL )
    {
        _dbg_msg( FormatString( "restoreGlobalState (3) %s c=%d w_dot=%s" )
                << w->getDescription() << int(w->isCurrentWindow()) << w->getWindowDot().asString() );
        w = w->w_next;
    }
}
#endif
}



// set the current buffer to the one named
void EmacsBuffer::set_bfn( const EmacsString &name )
{
    if( name.isNull() )
        return;

    EmacsBuffer *p = find( name );
    if( p == NULL )
        p = EMACS_NEW EmacsBuffer( name );

    p->set_bf();
}

void EmacsBuffer::scratch_bfn( const EmacsString &name, int disp)
{
    EmacsBuffer *p = find( name );
    if( p == NULL )
    {
        p = EMACS_NEW EmacsBuffer( name );
        p->b_checkpointed = -1;        // turn off checkpointing and
        p->b_journalling = 0;        // journalling
    }

    p->set_bf();

    if( disp )
        theActiveView->window_on( bf_cur );
    widen_region();
    bf_cur->erase_bf();
}

// change the name of this buffer
void EmacsBuffer::rename( const EmacsString &name )
{
    // remove from the name table
    name_table.remove( b_buf_name );
    // change the name
    b_buf_name = name;
    // put the buffer back
    name_table.add( b_buf_name, this );
}

EmacsBuffer::~EmacsBuffer(void)
{
    // do not let any of the following code find this buffer
    name_table.remove( b_buf_name );

    // remove pointers to this buffer from other objects
    EmacsBufferRef::markDeletedBuffer( this );
    EmacsWindowGroup::de_ref_buf( this );
    undo_de_ref_buf( this );

    b_journalling = 0;

    delete b_journal;
    if( b_base != NULL )
        EMACS_FREE( b_base );
    if( !b_checkpointfn.isNull() )
    {
        if( unlink_checkpoint_files )
            EmacsFile::fio_delete( b_checkpointfn );
    }

    EmacsBuffer *q = NULL;
    EmacsBuffer *p = buffers;
    while( p != NULL && p != this )
    {
        q = p;
        p = p->b_next;
    }
    if( q == 0 )
        buffers = b_next;
    else
        q->b_next = b_next;

    b_mark.unset_mark();

    QueueIterator<Marker> it( b_markset );

    while( it.next() )
        it.value()->unset_mark();

    if( bf_cur == this )
        theActiveView->currentWindow()->w_buf->set_bf();
}


// Erase the contents of a buffer
void EmacsBuffer::erase_bf()
{
    EmacsBufferRef old( bf_cur );

    set_bf();

    //
    //    Erasing a non file buffer kills off the journal.
    //    But as modifing the buffer writes a new
    //    journal journalling has to be disable over
    //    next few lines of code.
    //
    del_frwd( bf_cur->first_character(), bf_cur->num_characters() - bf_cur->first_character() + 1 );
    set_dot( bf_cur->first_character() );
    cant_1line_opt = 1;
    if( bf_cur->unrestrictedSize() == 0 )
    {
        b_modified = 0;
        if( bf_cur->b_kind != FILEBUFFER )
        {
            delete bf_cur->b_journal;
            bf_cur->b_journal = NULL;
        }
        bf_cur->b_syntax.syntax_valid = 0;
    }

    old.set_bf();
}

// set the marker of the buffer
void EmacsBuffer::set_mark( int pos, int right, bool gui )
{
    b_mark.set_mark( this, pos, right );
    b_gui_input_mode_set_mark = gui;

    // forward the set mark into the window if it is ours
    if( theActiveView != NULL )
    {
        EmacsWindow *w = theActiveView->currentWindow();
        if( w != NULL && w->w_buf == this )
            w->setWindowMark( b_mark, b_gui_input_mode_set_mark );
    }
}

// unset the marker of the buffer
void EmacsBuffer::unset_mark()
{
    b_mark.unset_mark();
    b_gui_input_mode_set_mark = false;

    // forward the unset mark into the window if it is ours
    if( theActiveView != NULL )
    {
        EmacsWindow *w = theActiveView->currentWindow();
        if( w != NULL && w->w_buf == this )
            w->unsetWindowMark();
    }
}

// initialize the buffer routines
void init_bf( void )
{
    default_mode_format = " %[%hBuffer: %b%* File: %f %M(%m%c%j%r%a%R) %p%]";
    default_tab_size = 8;
    default_indent_use_tab = 1;
    default_fold_case = 0;
    default_right_margin = 10000;
    default_left_margin = 1;
    default_comment_column = 33;
    default_replace_mode = 0;
    EmacsBuffer::set_bfn( "Minibuf" );
    minibuf = bf_cur;
    EmacsBuffer::set_bfn( "main" );
    bf_cur->b_kind = FILEBUFFER;
}

// Marker routines

// create a new marker
Marker::Marker( void )
    : m_buf( NULL )
    , m_pos( 0 )
    , m_right( 0 )
    , m_modified( 0 )
{ }

Marker::Marker( const Marker &src )
    : m_buf( NULL )
    , m_pos( 0 )
    , m_right( 0 )
    , m_modified( 0 )
{
    set_mark( src );
}

Marker::Marker( EmacsBuffer *b, int p, int right )
    : m_buf( NULL )
    , m_pos( 0 )
    , m_right( 0 )
    , m_modified( 0 )
{
    set_mark( b, p, right );
}

EmacsString Marker::asString() const
{
    if( isSet() )
    {
        return FormatString( "<Marker %s %s at %d>" )
            << (m_right ? "left" : "right")
            << m_buf->b_buf_name << get_mark();
    }
    else
    {
        return EmacsString( "<Marker unset>" );
    }
}


// delink a marker from a list of markers
void Marker::delink_mark(void)
{
    // get us out of the queue
    queueRemove();
    // lost the buffer pointer
    m_buf = NULL;
}



// destroy a marker
Marker::~Marker(void)
{
    delink_mark();
}

// set marker min buffer b at position p
void Marker::set_mark( const Marker &old_mark )
{
    set_mark( old_mark.m_buf, old_mark.get_mark(), old_mark.m_right );
}

void Marker::set_mark
    (
    EmacsBuffer *b,
    int p,
    int right
    )
{
    delink_mark();
    if( b == NULL )
        return;

#if DBG_BUFFER
    emacs_assert( p >= 1 );
    if( b == bf_cur )
        emacs_assert( p <= (bf_cur->unrestrictedSize() + 1) );
    else
        emacs_assert( p <= (b->b_size1 + b->b_size2 + 1) );
#endif

    //
    //    The call to error has been removed. It seems to only get called
    //    after the following type of code.
    //    (save-excursion (delete-buffer "bug") (pop-to-buffer "bug"))
    //
    if( p < 1 )        // error( "Bogus set_mark to %d", p),
        p = 1;
    m_buf = b;
    m_modified = 0;
    m_right = right;
    b->b_markset.queueInsertAtTail( this );

    m_modified = 0;
    if( b == bf_cur )
    {
        if( p < bf_cur->b_size1 + 1
        || (! right && p == bf_cur->b_size1 + 1) )
            m_pos = p;
        else
            m_pos = p + bf_cur->b_gap;
    }
    else
    {
        if( p < b->b_size1 + 1
        || (! right && p == b->b_size1 + 1) )
            m_pos = p;
        else
            m_pos = p + b->b_gap;
    }
#if DBG_BUFFER
    if( dbg_flags&DBG_BUFFER )
        check_markers( "set_mark" );
#endif
}

void Marker::unset_mark(void)
{
    delink_mark();
}

// copy the value of the source marker to the destination, handling all the
// nasty linking and delinking
Marker &Marker::operator=( const Marker &src )
{
    set_mark( src.m_buf, src.get_mark(), src.m_right );

    return *this;
}

// set bf_cur to the buffer indicated by the given marker and return
// the position( "dot", value ) within that buffer; returns 0 iff the
// marker wasn't set.
int Marker::to_mark(void) const
{
    if( m_buf == 0 )
        return 0;
    if( bf_cur != m_buf )
        m_buf->set_bf();
    if( m_pos > bf_cur->b_size1 + 1 )
        return m_pos - bf_cur->b_gap;
    else
        return m_pos;
}


// get the value of the marker without changing bf_cur
int Marker::get_mark(void) const
{
    if( m_buf == NULL )
        return 0;
    int pos;
    if( bf_cur == m_buf )
        if( m_pos > bf_cur->b_size1 + 1 )
            return m_pos - bf_cur->b_gap;
        else
            pos = m_pos;
    else
        if( m_pos > m_buf->b_size1 + 1 )
            pos = m_pos - m_buf->b_gap;
        else
            pos = m_pos;

#if DBG_BUFFER
    emacs_assert( pos >= 1 );
    if( m_buf == bf_cur )
        emacs_assert( pos <= (bf_cur->unrestrictedSize() + 1) );
    else
        emacs_assert( pos <= (m_buf->b_size1 + m_buf->b_size2 + 1) );
#endif

    return pos;
}

// Returns the number of the current line. Lines number from 1.
int current_line_number( void )
{
    int line_num;
    int n;

    line_num = 1;
    for( n=1; n<=dot - 1; n += 1 )
        if( bf_cur->char_at (n) == '\n' )
            line_num++;
    return line_num;
}

//======================================================================
//
//    EmacsBufferRef implementation
//
//======================================================================
EmacsBufferRef::EmacsBufferRef( EmacsBuffer *buf )
    : buffer_pointer( buf )
{
    header.queueInsertAtTail( this );
}

EmacsBufferRef::~EmacsBufferRef()
{
    queueRemove();
}



//
//    null out the refs to this buffer
//
void EmacsBufferRef::markDeletedBuffer( EmacsBuffer *buf )
{
    QueueIterator<EmacsBufferRef> it( header );

    while( it.next() )
    {
        EmacsBufferRef *ref = it.value();
        if( ref->buffer_pointer == buf )
            ref->buffer_pointer = NULL;
    }
}

QueueHeader<EmacsBufferRef> EmacsBufferRef::header;

#ifdef DEBUG_SET_BF
void EmacsBufferRef::__set_bf( const char *__file, int __line)
{
    if( buffer_pointer != NULL )
        buffer_pointer->__set_bf( __file, __line);
}
#else
void EmacsBufferRef::set_bf()
{
    if( buffer_pointer != NULL )
        buffer_pointer->set_bf();
}
#endif
//======================================================================
//
//    Debug routines
//
//======================================================================

#if DBG_BUFFER
static void check_bf( void )
{
#if DBG_ALLOC_CHECK
    emacs_heap_check();
#endif
}

void check_markers( char * s, EmacsBuffer *b )
{
    FormatString header("\n%s Buffer %s S1=%d GAP=%d, S2=%d b_mark=%X\n Marker   Position\tRight\tAllocated By\n");
    _dbg_msg( header << s << b->b_buf_name << b->b_size1 << b->b_gap << b->b_size2 << int(&b->b_mark) );

    QueueIterator<Marker> it( b->b_markset );

    while( it.next() )
    {
        Marker *m = it.value();
        const char *file = "";
        int line=0;
        const EmacsObject *object = NULL;
        m->objectAllocatedBy( file, line, object );

        _dbg_msg( FormatString(" %X %4.4d/%4.4d\t%4.4d\t%d     %s(%d) %s\n")
            << int(m) << m->m_pos << m->m_pos - bf_cur->b_gap << m->get_mark() << m->m_right
            << file << line << (object ? object->objectName() : "") );
    }
}
#endif
