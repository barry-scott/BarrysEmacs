//
//
//    file_name_compare.cpp
//
//    Copyright (c) 2000 Barry A. Scott
//
//


#include    <emacsutl.h>
#include    <emobject.h>
#include    <emstring.h>
#include    <em_stat.h>
#include    <fileserv.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

int match_wild( const EmacsString &candidate, const EmacsString &pattern )
{
    static EmacsChar_t null_str[1] = {0};

    const EmacsChar_t *cp, *pp;        // candidate and pattern pointers
    const EmacsChar_t *scp, *spp;    // saved cp and pp
    EmacsChar_t cch, pch;        // candidate and pattern char

    scp = spp = null_str;

    cp = candidate.unicode_data();
    pp = pattern.unicode_data();

    for(;;)
        if( *pp )    // while there is pattern chars left
        {
            pch = *pp++;

            if( pch == '*' )
            {
                if( *pp == '\0' )// pattern null after a *
                    return 1;

                scp = cp;// save pointers for back tracking
                spp = pp;
                continue;
            }
            cch = *cp++;
            if( cch == '\0' )// if candidate exhausted match fails
                break;

            if( pch == cch )
                continue;

            if( pch == '?' )
                continue;

            // mismatch detected
            if( *scp++ == '\0' )
                break;

            cp = scp;
            pp = spp;
        }
        else
        {
            if( *cp == '\0' )
                return 1;

            // mismatch detected
            if( *scp++ == '\0' )
                break;

            cp = scp;
            pp = spp;
        }

    return 0;
}

//
//    Implementation
//
FileNameCompare::~FileNameCompare()
{
}

FileNameCompareCaseSensitive::~FileNameCompareCaseSensitive()
{
}

bool FileNameCompareCaseSensitive::isEqual( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.compare( string2 ) == 0;
}

int FileNameCompareCaseSensitive::compare( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.compare( string2 );
}

int FileNameCompareCaseSensitive::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.commonPrefix( string2 );
}

FileNameCompareCaseBlind::~FileNameCompareCaseBlind()
{
}

bool FileNameCompareCaseBlind::isEqual( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.caseBlindCompare( string2 ) == 0;
}

int FileNameCompareCaseBlind::compare( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.caseBlindCompare( string2 );
}

int FileNameCompareCaseBlind::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.caseBlindCommonPrefix( string2 );
}

