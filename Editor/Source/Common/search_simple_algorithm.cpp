//
//    Copyright (c) 1982-2010
//        Barry A. Scott
//

// string search routines

#include <emacs.h>
#include <search_simple_algorithm.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

//
//
//    SearchSimpleAlgorithm implementation
//
//
SearchSimpleAlgorithm::SearchSimpleAlgorithm()
: SearchImplementation()
, sea_loc1( 0 )
, sea_loc2( 0 )
, sea_nbra( 0 )
, sea_case_fold( false )
{ }

SearchSimpleAlgorithm::~SearchSimpleAlgorithm()
{ }


bool SearchSimpleAlgorithm::is_compatible( EmacsSearch::sea_type type )
{
    return type == EmacsSearch::sea_type__string || type == EmacsSearch::sea_type__RE_simple;
}


//
// search for the n'th occurrence of string s in the current buffer,
// starting at dot, leaving dot at the end (if forward) or beginning
// (if reverse) of the found string returns. true or false
// depending on whether or not the string was found
//
int SearchSimpleAlgorithm::search( int n, int dot )
{
    int pos = dot;
    sea_case_fold = bf_cur->b_mode.md_foldcase;

    while( ! ml_err && n != 0 )
        if( n < 0 )
        {
            if( pos <= bf_cur->first_character() )
                return 0;
            int matl = search_execute( 0, pos - 1);
            if( matl < 0 )
                return 0;
            n++;
            pos = sea_loc1;
        }
        else
        {
            if( pos > bf_cur->num_characters() )
                return 0;
            int matl = search_execute( 1, pos );
            if( matl < 0 )
                return 0;
            n--;
            pos = sea_loc1 + matl;
        }

    if( ml_err )
        return -1;
    else
        return pos;
}

int SearchSimpleAlgorithm::looking_at( int pos )
{
    EmacsChar_t **alt = sea_alternatives;

    sea_case_fold = bf_cur->b_mode.md_foldcase;

    if( arg < 0 )
    {
        int resp = search_execute( 0, pos-1 );
        ml_value = resp + sea_loc1 == pos;
    }
    else
    {
        while( *alt != 0 && ! ml_err )
        {
            ml_value = search_advance( pos, *alt, 0, 0 );
            if( ml_value.asInt() != 0 )
                break;
            alt++;
        }

        sea_loc1 = dot;
    }

    return 0;
}

void SearchSimpleAlgorithm::search_replace_once( const EmacsString &new_string )
{
    enum rep_actions
        {
        DO_NOTHING, UPPER, FIRST, FIRST_ALL
        }
        action = DO_NOTHING;

    struct flag_bits
{
    unsigned int beg_of_str : 1;
    unsigned int beg_of_word : 1;
    unsigned int prefix : 1;
    unsigned int last_prefix : 1;
} flags;

    EmacsChar_t lc;

    if( replace_case )
    {
        int i;

        flags.beg_of_str = 1u;
        i = sea_loc1;
        flags.beg_of_word = i <= bf_cur->first_character() || !unicode_is_alphabetic( bf_cur->char_at(i - 1) );
        while( i < sea_loc2 )
        {
            if( unicode_is_alphabetic( lc = bf_cur->char_at( i ) ) )
            {
                if( unicode_is_upper( lc ) )
                {
                    if( flags.beg_of_str )
                        action = FIRST;
                    else
                        if( flags.beg_of_word && action != UPPER )
                            action = FIRST_ALL;
                        else
                            action = UPPER;
                }
                else
                    if( action == UPPER
                    || (action == FIRST_ALL && flags.beg_of_word) )
                    {
                        action = DO_NOTHING;
                        break;
                    }
                flags.beg_of_str = 0;
                flags.beg_of_word = 0;
            }
            else
                flags.beg_of_word = 1u;
            i++;
        }
    }

    flags.beg_of_str = 1u;
    flags.prefix = 0;
    flags.beg_of_word = dot <= bf_cur->first_character() || ! unicode_is_alphabetic( bf_cur->char_at( dot - 1 ) );
    for( int p = 0; p<new_string.length(); p++ )
    {
        lc = new_string[p];
        flags.last_prefix = flags.prefix;
        flags.prefix = 0;
        if( action != DO_NOTHING
        && unicode_is_alphabetic( lc ) )
        {
            if( unicode_is_lower( lc )
            &&  (action == UPPER
                || (action == FIRST_ALL && flags.beg_of_word)
                || (action == FIRST && flags.beg_of_str)) )
            {
                lc = (EmacsChar_t)unicode_to_upper( lc );
            }
            flags.beg_of_word = 0;
            flags.beg_of_str = 0;
        }
        else
            flags.beg_of_word = 1u;
        if( lc == 92
        && compiled_expression_type != EmacsSearch::sea_type__string
        && !flags.last_prefix )
            flags.prefix = 1u;
        else if( lc == '&'
        && compiled_expression_type != EmacsSearch::sea_type__string
        && ! flags.last_prefix )
            place( sea_loc1, sea_loc2 );
        else if( flags.last_prefix
        && lc >= '1'
        && lc < (EmacsChar_t)(sea_nbra + '1') )
            place
            (
            sea_bra_slist[ lc - '1' ].to_mark(),
            sea_bra_elist[ lc - '1' ].to_mark()
            );
        else
        {
            bf_cur->insert_at( dot, lc );
            dot_right( 1 );
        }
    }

    if( sea_loc1 == sea_loc2 )
        dot_right( 1 );
    else
    {
        dot_left( sea_loc2 - sea_loc1 );
        bf_cur->del_back( sea_loc2, sea_loc2 - sea_loc1 );
    }
}


// Compile the given regular expression into a secret internal format
void SearchSimpleAlgorithm::compile( const EmacsString &str, EmacsSearch::sea_type RE )
{
    int strp = 0;
    EmacsChar_t c;
    EmacsChar_t *lastep;
    EmacsChar_t bracket[MAX_NBRA];
    EmacsChar_t *bracketp;
    int cclcnt;

    compiled_expression_type = RE;

    EmacsChar_t **alt;

    alt = sea_alternatives;

    EmacsChar_t *ep = sea_expbuf;
    *alt++ = ep;
    bracketp = &bracket[0];
    if( str.isNull() )
    {
        if( ep[0] == 0 )
            error( "Null search string" );
        return;
    }
    sea_nbra = 0;
    last_search_string = str;
    lastep = 0;
{    // leave this block to return a compile error
    for(;;)
    {
        if( ep >= &sea_expbuf[ESIZE] )
            goto cerror;
        if( strp >= str.length() )
        {
            if( bracketp != &bracket[0] )
                goto cerror;
            *ep++ = CEOP;
            *alt++ = 0;
            return;
        }
        c = str[strp++];
        if( c != '*' )
            lastep = ep;
        if( RE == EmacsSearch::sea_type__string )
        {
            *ep++ = CCHR;
            *ep++ = c;
        }
        else
            switch( c)
            {
            case 92:
                if( strp >= str.length() )
                    goto cerror;

                switch( c = str[strp++] )
                {
                case '(':
                    if( sea_nbra >= MAX_NBRA )
                        goto cerror;
                    *bracketp++ = (EmacsChar_t)sea_nbra;
                    *ep++ = CBRA;
                    *ep++ = (EmacsChar_t)sea_nbra;
                    sea_nbra++;
                    break;
                case '|':
                    if( bracketp > &bracket[0] )
                        goto cerror;
                    // Alas!
                    *ep++ = CEOP;
                    *alt++ = ep;
                    break;
                case ')':
                    if( bracketp <= &bracket[0] )
                        goto cerror;
                    *ep++ = CKET;
                    bracketp = &bracketp[-1];
                    *ep++ = bracketp[0];
                    break;
                case '<':    *ep++ = BDOT; break;
                case '=':    *ep++ = EDOT; break;
                case '>':    *ep++ = ADOT; break;
                case '`':    *ep++ = BBUF; break;
                case 39:    *ep++ = EBUF; break;
                case 'w':    *ep++ = WORDC; break;
                case 'W':    *ep++ = NWORDC; break;
                case 'b':    *ep++ = WBOUND; break;
                case 'B':    *ep++ = NWBOUND; break;
                case 'c':    *ep++ = SEA_SYN_COMMENT; break;
                case 'C':    *ep++ = SEA_SYN_NOCOMMENT; break;
                case 's':    *ep++ = SEA_SYN_STRING; break;
                case 'S':    *ep++ = SEA_SYN_NOSTRING; break;
                case '1': case '2': case '3': case '4': case '5':
                    *ep++ = CBACK;
                    *ep++ = (EmacsChar_t)(c - '1');
                    break;
                default:
                    *ep++ = CCHR;
                    *ep++ = c;
                    break;
                }
                break;
            case '.':
                *ep++ = CDOT; break;
            case '*':
                if( lastep == 0
                || lastep[0] == CBRA
                || lastep[0] == CKET
                || lastep[0] == CIRC
                || (BBUF <= lastep[0] && lastep[0] <= ADOT)
                || (lastep[0] & STAR)
                || lastep[0] > NWORDC )
                {
                    *ep++ = CCHR;
                    *ep++ = c;
                }
                else
                    lastep[0] |= STAR;
                break;
            case '^':
                if( ep != &sea_expbuf[0]
                && ep[-1] != CEOP )
                {
                    *ep++ = CCHR;
                    *ep++ = c;
                }
                else
                    *ep++ = CIRC;
                break;
            case '$':
            {
                int remaining_length = str.length() - strp;

                if( remaining_length == 0
                || (remaining_length >= 2 && str[strp] == 92 && str[strp+1] == '|'))
                    *ep++ = CDOL;
                else
                {
                    *ep++ = CCHR;
                    *ep++ = c;
                }
            }
                break;
            case '[':
                *ep++ = CCL;
                *ep++ = 0;
                cclcnt = 1;
                if( strp >= str.length() )
                    goto cerror;
                c = str[strp++];
                if( c == '^' )
                {
                    if( strp >= str.length() )
                        goto cerror;
                    c = str[strp++];
                    ep[-2] = NCCL;
                }

                do
                {
                    if( c == '-' && ep[-1] != 0 )
                    {
                        if( strp >= str.length() )
                            goto cerror;
                        c = str[strp++];
                        if( c == ']' )
                        {
                            *ep++ = '-';
                            cclcnt++;
                            break;
                        }
                        while( ep[-1] < c )
                        {
                            // This should be reflected in the compiled form
                            ep[0] = (EmacsChar_t)(ep[-1] + 1);
                            ep++;
                            cclcnt++;
                            if( ep >= &sea_expbuf[ESIZE ] )
                                goto cerror;
                        }
                    }
                    *ep++ = c;
                    cclcnt++;
                    if( ep >= &sea_expbuf[ESIZE ] )
                        goto cerror;
                    if( strp >= str.length() )
                        goto cerror;
                    c = str[strp++];
                }
                while( c != ']' );

                lastep[1] = (EmacsChar_t)cclcnt;
                break;

            default:
            {
                *ep++ = CCHR;
                *ep++ = c;
            }
            }
    }
}
cerror:
    sea_expbuf[0 ] = 0;
    sea_nbra = 0;
    error(FormatString("Badly formed search string \"%s\"") << last_search_string.asString() );
}


void SearchSimpleAlgorithm::place(int l1, int l2)
{
    while( l1 < l2 )
    {
        bf_cur->insert_at( dot, bf_cur->char_at( l1 ) );
        dot_right( 1 );
        l1++;
    }
}

int SearchSimpleAlgorithm::get_number_of_groups()
{
    return sea_nbra;
}

int SearchSimpleAlgorithm::get_start_of_group( int group_number )
{
    if( group_number < 0 || group_number > get_number_of_groups() )
        return -1;


    if( group_number == 0 )
        return sea_loc1;
    else
        return sea_bra_slist[ group_number - 1 ].to_mark();
}

int SearchSimpleAlgorithm::get_end_of_group( int group_number )
{
    if( group_number < 0 || group_number > get_number_of_groups() )
        return -1;


    if( group_number == 0 )
        return sea_loc2;
    else
        return sea_bra_elist[ group_number - 1 ].to_mark();
}

int SearchSimpleAlgorithm::get_start_of_group( const EmacsString & )
{
    return 0;
}

int SearchSimpleAlgorithm::get_end_of_group( const EmacsString & )
{
    return 0;
}
