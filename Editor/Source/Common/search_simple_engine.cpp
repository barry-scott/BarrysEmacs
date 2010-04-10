//
//            Copyright (c) 1985
//        Barry A. Scott and Nick Emery
//            Copyright (c) 1986-2010
//        Barry A. Scott
//
#include <emacs.h>
#include <search_simple_algorithm.h>
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


#define NL '\n'             // Newlines character

//
//    compile size and match width will be set to -1 if
//    the size or width is not fixed.
//
struct search_exp_info
{
    int compile_size;       // number of bytes in the compiled form
    int match_width;        // how many chars this can match
};

static struct search_exp_info search_exp_info[] =
{
{0,-1},     // marks the end of the expression
{0,-1},     // not used entry
{2,0},      // CBRA                     \( -- begin bracket
{2,0},      // CBRA|STAR                \( -- begin bracket
{2,1},      // CCHR                     a vanilla character
{2,-1},     // CCHR|STAR                a vanilla character
{1,1},      // CDOT                     . -- match anything except a newline
{1,-1},     // CDOT|STAR                . -- match anything except a newline
{-1,1},     // CCL                      [...] -- character class
{-1,-1},    // CCL|STAR                 [...] -- character class
{-1,1},     // NCCL                     [^...] -- negated character class
{-1,-1},    // NCCL|STAR                [^...] -- negated character class
{1,0},      // CDOL                     $ -- matches the end of a line
{1,-1},     // CDOL|STAR                $ -- matches the end of a line
{1,-1},     // CEOP                     The end of the pattern
{1,-1},     // CEOP|STAR                The end of the pattern
{2,0},      // CKET                     \) -- close bracket
{2,-1},     // CKET|STAR                \) -- close bracket
{2,-1},     // CBACK                    \N -- backreference to the Nth bracketed string
{2,-1},     // CBACK|STAR               \N -- backreference to the Nth bracketed string
{1,0},      // CIRC                     ^ matches the beginning of a line
{1,0},      // CIRC|STAR                ^ matches the beginning of a line
{1,0},      // BBUF                     beginning of buffer \`
{1,0},      // BBUF|STAR                beginning of buffer \`
{1,0},      // EBUF                     end of buffer \'
{1,0},      // EBUF|STAR                end of buffer \'
{1,0},      // BDOT                     matches before dot \<
{1,0},      // BDOT|STAR                matches before dot \<
{1,0},      // EDOT                     matches at dot \=
{1,0},      // EDOT|STAR                matches at dot \=
{1,0},      // ADOT                     matches after dot \>
{1,0},      // ADOT|STAR                matches after dot \>
{1,1},      // WORDC                    matches word character \w
{1,-1},     // WORDC|STAR               matches word character \w
{1,1},      // NWORDC                   matches non-word characer \W
{1,-1},     // NWORDC|STAR              matches non-word characer \W
{1,0},      // WBOUND                   matches word boundary \b
{1,0},      // WBOUND|STAR              matches word boundary \b
{1,0},      // NWBOUND                  matches non-(word boundary) \B
{1,0},      // NWBOUND|STAR             matches non-(word boundary) \B
{1,0},      // SEA_SYN_COMMENT          only match inside a comment \c
{1,-1},     // SEA_SYN_COMMENT|STAR     only match inside a comment \c
{1,0},      // SEA_SYN_NOCOMMENT        only match outside a comment \C
{1,-1},     // SEA_SYN_NOCOMMENT|STAR   only match outside a comment \C
{1,0},      // SEA_SYN_STRING           only match inside a string \s
{1,-1},     // SEA_SYN_STRING|STAR      only match inside a string \s
{1,0},      // SEA_SYN_NOSTRING         only match outside a string \S
{1,-1}      // SEA_SYN_NOSTRING|STAR    only match outside a string \S
};

// check to see whether the most recently compiled regular expression
//   matches the string starting at addr in the buffer.
//   The search match is performed in the current buffer.
//   fflag is true iff we are doing a forward search.
//
const int SEARCH_FORWARD( 1 );
const int SEARCH_BACKWARD( 0 );
const int SEARCH_CASEFOLD( 2 );
const int SEARCH_USETRT( 4 );

int SearchSimpleAlgorithm::search_execute( int fflag, int addr )
{
    int p1 = addr;
    EmacsChar_t *exp;
    int i;
    int inc = fflag ? 1 : -1;

    for( i=0; i<=SearchSimpleAlgorithm::MAX_NBRA-1; i++ )
    {
        sea_bra_slist[ i ].unset_mark();
        sea_bra_elist[ i ].unset_mark();
    }
    if( addr == 0 )
        return (-1);

    // skip over any express elements that
    // have no effect on the matching
    exp = sea_expbuf;
    while( search_exp_info[*exp].match_width == 0 )
        exp += search_exp_info[*exp].compile_size;

    // regular algorithm
    int numchars = bf_cur->num_characters();
    int firstchar = bf_cur->first_character();

    do
    {
        EmacsChar_t **alt;
        alt = sea_alternatives;

        while( *alt != NULL )
            if( search_advance (p1, *alt, 0, 0) )
            {
                sea_loc1 = p1;
                return sea_loc2 - sea_loc1;
            }
            else
            {
                alt++;
                if( ml_err )
                    return -1;
            }
        p1 += inc;
    }
    while( p1 <= numchars && p1 >= firstchar );

    return -1;
}

static unsigned char *syn_sea_error = u_str("search requires syntax-array to be enabled");

// search_advance the match of the regular expression starting at ep along the
//  string lp, simulates an NDFSA
int SearchSimpleAlgorithm::search_advance
    (
    int lp,
    EmacsChar_t *ep,
    int syn_include,
    int syn_exclude
    )
{
#define SYN_EXCLUDE    (syn_exclude && (b->syntax_at(lp)&syn_exclude) != 0)
#define SYN_INCLUDE    (syn_include && (b->syntax_at(lp)&syn_include) == 0)

    int curlp;
    EmacsChar_t ch;
    int numchars = bf_cur->num_characters();
    int firstchar = bf_cur->first_character();

    EmacsBuffer *b = bf_cur;

//    _m( 'advance_search at %d, length is %d', .lp, .length );
    while( (! ml_err &&
        (lp <= numchars
        || ((ch = *ep) & STAR) || ch == CKET || ch == EBUF )) )
    {
//        _m( 'ep = %d, lp = %d char(.lp-1) = %d, char(.lp) = %d',
//            .ep[0], .lp, .ch_p[ .lp-1 ], .ch_p[ .lp ] );

        if( (syn_include != 0 || syn_exclude != 0)
        && bf_cur->b_syntax.syntax_valid <= lp )
        {
            bf_cur->syntax_fill_in_array( lp + 10000 );
        }

        switch( *ep++ )
        {
        case CCHR:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( sea_case_fold )
            {
                if( unicode_casefold( *ep++ ) != unicode_casefold( b->char_at(lp) ) )
                    return 0;
            }
            else
            {
                if( *ep++ != b->char_at(lp) )
                    return 0;
            }
            lp++;
            break;

        case CDOT:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( (b->char_at(lp) == NL) )
                return 0;
            lp++;
            break;

        case CDOL:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( (b->char_at(lp) != NL) )
                return 0;
            break;

        case CIRC:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( ! (lp <= firstchar || bf_cur->char_at(lp-1) == NL) )
                return 0;
            break;

        case BBUF:
            if( (lp > firstchar) )
                return 0;
            break;

        case EBUF:
            if( (lp <= numchars) )
                return 0;
            break;

        case BDOT:
            if( (lp > dot) )
                return 0;
            break;

        case EDOT:
            if( (lp != dot) )
                return 0;
            break;

        case ADOT:
            if( (lp < dot) )
                return 0;
            break;

        case WORDC:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( bf_cur->char_at_is( lp, SYNTAX_WORD) )
                lp++;
            else
                return 0;
            break;

        case NWORDC:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( ( !bf_cur->char_at_is( lp, SYNTAX_WORD)) )
                lp++;
            else
                return 0;
            break;

        case WBOUND:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( !((lp <= firstchar || !bf_cur->char_at_is( lp-1, SYNTAX_WORD)) !=
                (lp > numchars || !bf_cur->char_at_is( lp, SYNTAX_WORD))) )
                return 0;
            break;

        case NWBOUND:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( ! ((lp <= firstchar ||  ! bf_cur->char_at_is (lp-1, SYNTAX_WORD)) ==
                (lp > numchars ||  ! bf_cur->char_at_is (lp, SYNTAX_WORD))) )
                return 0;
            break;

        case SEA_SYN_COMMENT:
            if( !bf_cur->b_mode.md_syntax_array )
            {
                error(syn_sea_error);
                return 0;
            }
            syn_include |= SYNTAX_COMMENT_MASK;
            syn_exclude &= ~SYNTAX_COMMENT_MASK;
            break;

        case SEA_SYN_NOCOMMENT:
            if( !bf_cur->b_mode.md_syntax_array )
            {
                error(syn_sea_error);
                return 0;
            }
            syn_include &= ~SYNTAX_COMMENT_MASK;
            syn_exclude |= SYNTAX_COMMENT_MASK;
            break;

        case SEA_SYN_STRING:
            if( !bf_cur->b_mode.md_syntax_array )
            {
                error(syn_sea_error);
                return 0;
            }
            syn_include |= SYNTAX_STRING_MASK;
            syn_exclude &= ~SYNTAX_STRING_MASK;
            break;

        case SEA_SYN_NOSTRING:
            if( !bf_cur->b_mode.md_syntax_array )
            {
                error(syn_sea_error);
                return 0;
            }
            syn_include &= ~SYNTAX_STRING_MASK;
            syn_exclude |= SYNTAX_STRING_MASK;
            break;

        case CEOP:
            sea_loc2 = lp;
            return 1;

        case CCL:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( (cclass( ep, b->char_at(lp), 1 )) )
            {
                ep = &ep[ *ep ];
                lp++;
            }
            else
                return 0;
            break;

        case NCCL:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            if( cclass( ep, b->char_at(lp), 0 ) )
            {
                ep = &ep[ *ep ];
                lp++;
            }
            else
                return 0;
            break;

        case CBRA:
            sea_bra_slist[*ep++ ].set_mark( bf_cur, lp, 0 );
            break;

        case CKET:
            sea_bra_elist[*ep++ ].set_mark( bf_cur, lp, 0 );
            break;

        case CBACK:
            ch = *ep++;
            if( sea_bra_elist[ ch ].m_buf == NULL )
            {
                error( "bad braces");
                return 0;
            }
            if( backref(ch, lp) )
            {
                lp += sea_bra_elist[ ch ].to_mark()
                    - sea_bra_slist[ ch ].to_mark();
            }
            else
                return 0;
            break;

        case CBACK|STAR:
            ch = *ep++;
            if( sea_bra_elist[ ch ].m_buf == NULL )
            {
                error( "bad braces");
                return 0;
            }
            curlp = lp;
            while( backref( ch, lp ) )
            {
                lp -=    sea_bra_elist[ ch ].to_mark()
                    - sea_bra_slist[ ch ].to_mark();
            }
            while( lp >= curlp )
            {
                if( search_advance(lp, ep, syn_include, syn_exclude) )
                    return 1;

                lp -=    sea_bra_elist[ ch ].to_mark()
                    - sea_bra_slist[ ch ].to_mark();
            }
            break;

        case CDOT|STAR:
            curlp = lp;
            while( lp <= numchars
            && bf_cur->char_at(lp) != NL )
                lp++;
            lp++;
            do
            {
                lp--;
                if( search_advance( lp, ep, syn_include, syn_exclude ) )
                    return 1;
            }
            while( lp > curlp );
            return 0;

        case WORDC|STAR:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            curlp = lp;
            while( lp <= numchars
            && bf_cur->char_at_is(lp, SYNTAX_WORD) )
                lp++;
            lp++;
            do
            {
                lp--;
                if( search_advance( lp, ep, syn_include, syn_exclude ) )
                    return 1;
            }
            while( lp > curlp );
            return 0;

        case NWORDC|STAR:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            curlp = lp;
            while( lp <= numchars
            &&  ! bf_cur->char_at_is( lp, SYNTAX_WORD) )
                lp++;
            lp++;
            do
            {
                lp--;
                if( search_advance( lp, ep, syn_include, syn_exclude ) )
                    return 1;
            }
            while( lp > curlp );
            return 0;

        case CCHR|STAR:
            if( SYN_EXCLUDE || SYN_INCLUDE )
                return 0;
            curlp = lp;
            if( sea_case_fold )
            {
                ch = unicode_casefold( *ep );
                while( lp <= numchars
                && unicode_casefold( bf_cur->char_at( lp ) ) == ch )
                    lp++;
            }
            else
            {
                ch = *ep;
                while( lp <= numchars
                && bf_cur->char_at( lp ) == ch )
                    lp++;
            }

            lp++;
            ep++;
            do
            {
                lp--;
                if( search_advance( lp, ep, syn_include, syn_exclude ) )
                    return 1;
            }
            while( lp > curlp );
            return 0;

        case CCL | STAR:
        case NCCL | STAR:
            curlp = lp;
            ch = (EmacsChar_t)(ep[-1] == (CCL | STAR));
            while( lp <= numchars
            && !SYN_EXCLUDE && !SYN_INCLUDE
            && cclass( ep, bf_cur->char_at(lp), ch ) )
                lp++;
            lp++;
            ep = &ep[ *ep ];
            do
            {
                lp--;
                if( search_advance( lp, ep, syn_include, syn_exclude ) )
                    return 1;
            }
            while( lp > curlp );
            return 0;

        default:
            error( FormatString("Badly compiled pattern \"%s\" (Emacs internal error!)") <<
                    last_search_string.asString() );
            return 0;
        }
    }

    if(( (ch = ep[0]) == CEOP || ch == CDOL ) )
    {
        sea_loc2 = lp;
        return 1;
    }
    return 0;
#undef SYN_EXCLUDE
#undef SYN_INCLUDE
}



int SearchSimpleAlgorithm::backref( int i, int lp )
{
    int bp = sea_bra_slist[ i ].to_mark();
    int ep = sea_bra_elist[ i ].to_mark();

    while( lp <= bf_cur->num_characters() && bf_cur->char_at( bp ) == bf_cur->char_at( lp ) )
    {
        bp++;
        lp++;
        if( bp >= ep )
            return 1;
    }

    return 0;
}



int SearchSimpleAlgorithm::cclass( EmacsChar_t *char_set, int c, int af )
{
    int n = *char_set++;

    if( sea_case_fold )
    {
        while( (n = n - 1) != 0 )
            if( unicode_casefold( *char_set++ ) == unicode_casefold( c ) )
                return af;
    }
    else
    {
        while( (n = n - 1) != 0 )
            if( *char_set++ == c )
                return af;
    }

    return !af;
}
