//
//    Copyright (c) 1982-2002
//        Barry A. Scott
//

// string search routines

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );




void SearchGlobals::compile( const EmacsString &str, SearchGlobals::sea_type RE )
{
    switch( RE )
    {
    default:
    case sea_type__string:
        compile_string( str );
        break;

    case sea_type__RE_simple:
        compile_RE_simple( str );
        break;

    case sea_type__RE_advanced:
        compile_RE_advanced( str );
        break;
    }
}

//--------------------------------------------------------------------------------
//
// Compile the given string into internal format
//
//--------------------------------------------------------------------------------
void SearchGlobals::compile_string( const EmacsString &str )
{
    CompilerStringIterator pattern( str );

    if( pattern.atEnd() )
    {
        if( sea_expbuf[0] == CEOP )
            error( "Null search string" );
        return;
    }

    try
    {
        last_search_string = str;

        compiler_begin_compile();

        while( pattern.beforeEnd() )
            compiler_add_cchr( pattern.nextChar() );

        compiler_end_compile();
    }

    catch( CompilerError &ml_err )
    {
        compiler_begin_compile();
        compiler_end_compile();

        error( FormatString("Badly formed search string \"%s\" - %s") << last_search_string.asString() << ml_err.msg );
    }
}

//--------------------------------------------------------------------------------
//
// Compile the given advanced regular expression into internal format
//
//--------------------------------------------------------------------------------
void SearchGlobals::compile_RE_advanced( const EmacsString &str )
{
    CompilerStringIterator pattern( str );

    if( pattern.atEnd() )
    {
        if( sea_expbuf[0] == CEOP )
            error( "Null search string" );
        return;
    }

    try
    {
        unsigned char bracket[SearchGlobals::MAX_NBRA];

        unsigned char *bracketp = &bracket[0];

        sea_nbra = 0;
        last_search_string = str;

        compiler_begin_compile();

        while( pattern.beforeEnd() )
        {
            re_expr_cell_t c = pattern.nextChar();

            switch( c )
            {
            case '(':
                // One of the following is expected
                //
                // (?=...)
                // (?!...)
                // (?<=...)
                // (?<!...)
                // (??<option-list>)
                // (?:...)
                // (?P<name>...)
                // (?P=<name>)
                // (...)
                //

                if( pattern.equalTo( "?" ) )
                {
                    pattern.nextChar();
                    c = pattern.nextChar();
                    switch( c )
                    {
                    case '?':
                        do
                        {
                            c = pattern.nextChar();
                            switch( c )
                            {
                            case 'c':    compiler_add_single( SEA_SYN_COMMENT ); break;
                            case 'C':    compiler_add_single( SEA_SYN_NOCOMMENT ); break;
                            case 's':    compiler_add_single( SEA_SYN_STRING ); break;
                            case 'S':    compiler_add_single( SEA_SYN_NOSTRING ); break;
                            default:
                                throw CompilerError( "unknown option in \"(??)\"" );
                            }
                        }
                        while( c != ')' );
                        break;
                    default:
                        throw CompilerError( "unknonw option in \"(?)\"" );
                    }
                }
                else
                {
                    if( sea_nbra >= SearchGlobals::MAX_NBRA )
                        throw CompilerError( "too many \"(\"" );

                    *bracketp++ = (unsigned char)sea_nbra;
                    compiler_add_single( CBRA );
                    compiler_add_data( sea_nbra );
                    sea_nbra++;
                }
                break;

            case '|':
                if( bracketp > &bracket[0] )
                    throw CompilerError( "must close \"(\" before |" );

                compiler_new_alternative();
                break;

            case ')':
                if( bracketp <= &bracket[0] )
                    throw CompilerError( "\")\" without matching \"(\"" );

                compiler_add_single( CKET );
                bracketp--;
                compiler_add_data( bracketp[0] );
                break;

            case '\\':
                c = pattern.nextChar();
                switch( c )
                {

                case '<':    compiler_add_single( BDOT ); break;
                case '=':    compiler_add_single( EDOT ); break;
                case '>':    compiler_add_single( ADOT ); break;
                case '`':    compiler_add_single( BBUF ); break;
                case '\'':    compiler_add_single( EBUF ); break;
                case 'w':    compiler_add_repeatable( WORDC ); break;
                case 'W':    compiler_add_repeatable( NWORDC ); break;
                case 'b':    compiler_add_single( WBOUND ); break;
                case 'B':    compiler_add_single( NWBOUND ); break;
                case '1': case '2': case '3':
                case '4': case '5': case '6':
                case '7': case '8': case '9':
                    compiler_add_repeatable( CBACK );
                    compiler_add_data( (unsigned int)(c - '1') );
                    break;
                case 'd':
                {
                    compiler_add_ccl( CCL );
                    compiler_add_data( "0123456789" );
                    compiler_ccl_complete();
                }
                    break;
                case 'D':
                {
                    compiler_add_ccl( NCCL );
                    compiler_add_data( "0123456789" );
                    compiler_ccl_complete();
                }
                    break;
                case 's':
                {
                    compiler_add_ccl( CCL );
                    compiler_add_data( " \t" );
                    compiler_ccl_complete();
                }
                    break;
                case 'S':
                {
                    compiler_add_ccl( NCCL );
                    compiler_add_data( " \t" );
                    compiler_ccl_complete();
                }
                    break;
                default:
                    compiler_add_cchr( c );
                    break;
                }
                break;
            case '.':
                compiler_add_repeatable( CDOT );
                break;

            case '{':
                // expect one of
                // {m}
                // {m,n}
                // where m and n and decimal numbers
            {
                int range_min = 0;
                int range_max = 0;

                enum state_t { st_first_digit, st_more_min, st_max} state( st_first_digit );

                do
                {
                    c = pattern.nextChar();

                    switch( state )
                    {
                    case st_first_digit:
                        if( ! (c >= '0' && c <= '9') )
                            throw CompilerError( "expecting digit in min of {min,max}" );

                        range_min = c - '0';

                        state = st_more_min;
                        break;
                    case st_more_min:
                        if( c >= '0' && c <= '9' )
                        {
                            range_min = range_min*10 + (c - '0');
                        }
                        else if( c == ',' )
                            state = st_max;
                        else if( c == '}' )
                            break;
                        else
                            throw CompilerError( "expecting digit in min of {min,max}" );
                        break;
                    case st_max:
                        if( c >= '0' && c <= '9' )
                        {
                            range_max = range_max*10 + (c - '0');
                        }
                        else if( c == '}' )
                            break;
                        else
                            throw CompilerError( "expecting digit in max of {min,max}" );
                        break;
                    }
                }
                while( c != '}' );

                compiler_set_min_max( range_min, range_max );
            }
                break;

            case '?':
                switch( compiler_state )
                {
                case repeatable_re:
                    compiler_set_min_max( 0, 1 );
                    break;
                case repeated_re:
                    // need to set greed mode
                    compiler_state = single_re;
                    break;
                default:
                    throw CompilerError( "unexpected \"?\"" );
                }
                break;
            case '+':
                compiler_set_min_max( 1, 0 );
                break;

            case '*':
                compiler_set_min_max( 0, 0 );
                break;

            case '^':
                if( compiler_at_start() )
                    compiler_add_cchr( c );
                else
                    compiler_add_single( CIRC );
                break;
            case '$':
            {
                if( pattern.atEnd()
                || pattern.equalTo( "\\|" ) )
                    compiler_add_single( CDOL );
                else
                    compiler_add_cchr( c );
            }
                break;
            case '[':
            {
                c = pattern.nextChar();
                if( c == '^' )
                {
                    c = pattern.nextChar();
                    compiler_add_ccl( NCCL );
                }
                else
                    compiler_add_ccl( CCL );

                if( c == '-' )
                {
                    compiler_add_data( c );
                    c = pattern.nextChar();
                }

                while( c != ']' )
                {
                    compiler_add_data( c );
                    if( pattern.equalTo( "-" ) )
                    {
                        pattern.nextChar();
                        re_expr_cell_t range_end = pattern.nextChar();

                        if( range_end == ']' )
                        {
                            compiler_add_data( re_expr_cell_t( '-' ) );
                            break;
                        }

                        for( c++; c <= range_end; c++ )
                            compiler_add_data( c );
                    }

                    c = pattern.nextChar();
                }

                compiler_ccl_complete();
            }
                break;

            default:
                compiler_add_cchr( c );
            }
        }



            if( bracketp != &bracket[0] )
                throw CompilerError( "missing \"\\\\)\"" );

        compiler_end_compile();
    }

    catch( CompilerError &ml_err )
    {
        compiler_begin_compile();
        compiler_end_compile();

        error( FormatString("Badly formed search string \"%s\" - %s") << last_search_string.asString() << ml_err.msg );
    }
}

//--------------------------------------------------------------------------------
//
// Compile the given original regular expression into internal format
//
//--------------------------------------------------------------------------------
void SearchGlobals::compile_RE_simple( const EmacsString &str )
{
    CompilerStringIterator pattern( str );

    if( pattern.atEnd() )
    {
        if( sea_expbuf[0] == CEOP )
            error( "Null search string" );
        return;
    }

    try
    {
        unsigned char bracket[SearchGlobals::MAX_NBRA];
        int cclcnt;

        unsigned char *bracketp = &bracket[0];

        sea_nbra = 0;
        last_search_string = str;

        compiler_begin_compile();

        while( pattern.beforeEnd() )
        {
            re_expr_cell_t c = pattern.nextChar();

            switch( c )

            {
            case 92:
                c = pattern.nextChar();
                switch( c )
                {
                case '(':
                    if( sea_nbra >= SearchGlobals::MAX_NBRA )
                        throw CompilerError( "too many \"\\(\"" );

                    *bracketp++ = (unsigned char)sea_nbra;
                    compiler_add_single( CBRA );
                    compiler_add_data( sea_nbra );
                    sea_nbra++;
                    break;
                case '|':
                    if( bracketp > &bracket[0] )
                        throw CompilerError( "must close \"\\(\" before \\|" );

                    compiler_new_alternative();
                    break;
                case ')':
                    if( bracketp <= &bracket[0] )
                        throw CompilerError( "\"\\)\" without matching \"\\(\"" );

                    compiler_add_single( CKET );
                    bracketp--;
                    compiler_add_data( bracketp[0] );
                    break;
                case '<':    compiler_add_single( BDOT ); break;
                case '=':    compiler_add_single( EDOT ); break;
                case '>':    compiler_add_single( ADOT ); break;
                case '`':    compiler_add_single( BBUF ); break;
                case 39:    compiler_add_single( EBUF ); break;
                case 'w':    compiler_add_repeatable( WORDC ); break;
                case 'W':    compiler_add_repeatable( NWORDC ); break;
                case 'b':    compiler_add_single( WBOUND ); break;
                case 'B':    compiler_add_single( NWBOUND ); break;
                case 'c':    compiler_add_single( SEA_SYN_COMMENT ); break;
                case 'C':    compiler_add_single( SEA_SYN_NOCOMMENT ); break;
                case 's':    compiler_add_single( SEA_SYN_STRING ); break;
                case 'S':    compiler_add_single( SEA_SYN_NOSTRING ); break;
                case '1': case '2': case '3': case '4': case '5':
                    compiler_add_repeatable( CBACK );
                    compiler_add_data( (unsigned int)(c - '1') );
                    break;
                default:
                    compiler_add_cchr( c );
                    break;
                }
                break;
            case '.':
                compiler_add_repeatable( CDOT );
                break;
            case '*':
                if( compiler_state == repeatable_re )
                    compiler_set_min_max( 0, 0 );
                else
                    compiler_add_cchr( c );
                break;
            case '^':
                if( compiler_at_start() )
                    compiler_add_cchr( c );
                else
                    compiler_add_single( CIRC );
                break;
            case '$':
            {
                if( pattern.atEnd()
                || pattern.equalTo( "\\|" ) )
                    compiler_add_single( CDOL );
                else
                    compiler_add_cchr( c );
            }
                break;
            case '[':
            {
                c = pattern.nextChar();
                if( c == '^' )
                {
                    c = pattern.nextChar();
                    compiler_add_ccl( NCCL );
                }
                else
                    compiler_add_ccl( CCL );

                if( c == '-' )
                {
                    compiler_add_data( c );
                    c = pattern.nextChar();
                }

                while( c != ']' )
                {
                    compiler_add_data( c );
                    if( pattern.equalTo( "-" ) )
                    {
                        pattern.nextChar();
                        re_expr_cell_t range_end = pattern.nextChar();

                        if( range_end == ']' )
                        {
                            compiler_add_data( re_expr_cell_t( '-' ) );
                            break;
                        }

                        for( c++; c <= range_end; c++ )
                            compiler_add_data( c );
                    }

                    c = pattern.nextChar();
                }

                compiler_ccl_complete();
            }
                break;
            default:
                compiler_add_cchr( c );
            }


        }

        if( bracketp != &bracket[0] )
            throw CompilerError( "missing \"\\\\)\"" );

        compiler_end_compile();
    }

    catch( CompilerError &ml_err )
    {
        compiler_begin_compile();
        compiler_end_compile();

        error( FormatString("Badly formed search string \"%s\" - %s") << last_search_string.asString() << ml_err.msg );
    }
}

//--------------------------------------------------------------------------------
//
//    Compiler functions
//
//--------------------------------------------------------------------------------
void SearchGlobals::compiler_begin_compile()        // called to init the compiler
{
    sea_expbuf[0] = CEOP;
    compiler_expr_next = sea_expbuf;
    compiler_last_repeatable = compiler_expr_next;

    compiler_cur_alt = 0;

    sea_alternatives[ 0 ] = compiler_expr_next;
    sea_alternatives[ 1 ] = NULL;
}

void SearchGlobals::compiler_new_alternative()
{
    if( compiler_cur_alt >= MAX_NALTS )
        throw CompilerError( "too many alternatives" );

    // terminate old alternative expression
    compiler_add_single( CEOP );

    // more to next alternative
    compiler_cur_alt++;
    sea_alternatives[ compiler_cur_alt ] = compiler_expr_next;
    sea_alternatives[ compiler_cur_alt+1 ] = NULL;
}

// called when all the expr has been compiled
void SearchGlobals::compiler_end_compile()
{
    compiler_cur_alt++;
    compiler_add_data( CEOP );
    sea_alternatives[compiler_cur_alt] = NULL;
}

void SearchGlobals::compiler_add_cchr( unsigned char ch )
{
    compiler_add_repeatable( CCHR );
    compiler_add_data( ch );
}

void SearchGlobals::compiler_add_single( search_meta_chars type )
{
    compiler_state = single_re;
    compiler_add_data( re_expr_cell_t( type ) );
}

void SearchGlobals::compiler_add_repeatable( search_meta_chars type )
{
    compiler_state = repeatable_re;
    compiler_last_repeatable = compiler_expr_next;

    compiler_add_data( type );
    compiler_add_data( 1u );    // MIN
    compiler_add_data( 1u );    // MAX
}

void SearchGlobals::compiler_add_data( search_meta_chars data )
{
    compiler_add_data( re_expr_cell_t( data ) );
}

void SearchGlobals::compiler_add_data( const char *data )
{
    while( *data != '\0' )
        compiler_add_data( re_expr_cell_t( *data++ ) );
}

void SearchGlobals::compiler_add_data( unsigned int data )
{
    if( data > re_expr_cell_max )
        throw CompilerError( "min or max too large" );

    compiler_add_data( re_expr_cell_t( data ) );
}

void SearchGlobals::compiler_add_data( unsigned char data )
{
    if( compiler_expr_next >= &sea_expbuf[SearchGlobals::ESIZE] )
        throw CompilerError( "search string is too long" );

    *compiler_expr_next++ = data;
}

void SearchGlobals::compiler_set_min_max( unsigned int repeat_min, unsigned int repeat_max )
{
    if( compiler_state != repeatable_re )
        throw CompilerError( "internal error attempting to set min max when not repeatable" );

    if( repeat_min > re_expr_cell_max )
        throw CompilerError( "min too large" );
    if( repeat_max > re_expr_cell_max )
        throw CompilerError( "max too large" );

    if( repeat_max != 0
    && (repeat_min > repeat_max) )
        throw CompilerError( "min must be <= max" );

    compiler_last_repeatable[1] = re_expr_cell_t( repeat_min );
    compiler_last_repeatable[2] = re_expr_cell_t( repeat_max );

    compiler_state = repeated_re;
}

bool SearchGlobals::compiler_at_start()
{
    return sea_alternatives[ compiler_cur_alt-1 ] == compiler_expr_next;
}

// add CCL or NCCL
void SearchGlobals::compiler_add_ccl( search_meta_chars type )
{
    compiler_add_repeatable( type );
    compiler_add_data( 0u );            // place holder for CCL size
}

// add CCL or NCCL
void SearchGlobals::compiler_ccl_complete()
{
    compiler_last_repeatable[3] = re_expr_cell_t( (compiler_expr_next - compiler_last_repeatable) - 4 );
}

//--------------------------------------------------------------------------------
//
//    CompilerError
//
//--------------------------------------------------------------------------------

CompilerError::CompilerError( const char *_msg )
    : msg( _msg )
{}

CompilerError::~CompilerError()
{}

//--------------------------------------------------------------------------------
//
//    CompilerStringIterator
//
//--------------------------------------------------------------------------------
CompilerStringIterator::CompilerStringIterator( const EmacsString &_str )
    : str( _str )
    , pos( 0 )
{}

CompilerStringIterator::~CompilerStringIterator()
{}

// true if at the end of the string
bool CompilerStringIterator::atEnd() const
{
    return pos == str.length();
}

// true if before the end of the string
bool CompilerStringIterator::beforeEnd() const
{
    return pos < str.length();
}

unsigned char CompilerStringIterator::nextChar()
{
    if( atEnd() )
        throw CompilerError( "pattern incomplete" );

    return str[pos++];
}

// true if the string contains str next
bool CompilerStringIterator::equalTo( const EmacsString &next )
{
    int remaining = str.length() - pos;
    return remaining >= next.length() && str( pos, pos+str.length() ) == next;
}
