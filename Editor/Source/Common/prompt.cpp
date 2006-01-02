// prompt - Routines to prompt in the Mini-buffer and collect input

//
//    COPYRIGHT (c© 1985 BY Nick W. Emery and Barry Scott
//

//
// FACILITY:
//    VAX/VMS EMACS, prompt facility
//
// ABSTRACT:
//    This module contains routines that collect input prompted for
//    in the Mini-buffer.
//
//


// "Declarations"

//
// SWITCHES:
//
//    NONE
//
// INCLUDE FILES:
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


//
// TABLE OF CONTENTS:
//
int getword( EmacsString * *table_ref, const EmacsString &s );    // fetch a completed word
static EmacsString getnsword( EmacsString * *table, EmacsString ** (*table_maker)( const EmacsString &, int * ),
    const EmacsString & );    // fetch a non-space fill word
EmacsString getescword( EmacsString * *table_ref, const EmacsString &s );    // fetch an escape expanded word
EmacsString getescfile( const EmacsString fmt, va_list *argp, int dirs, int vers );        // fetch an expanded filename
static EmacsString ** get_file_list( EmacsString file, int *dir_spec_len );    // Make a filename table
EmacsString getescdb( struct dbsearch *current_dbs, const EmacsString fmt, ... );    // fetch an expanded DB searchlist key
#ifdef DB
static EmacsString ** globdb( const EmacsString &key, int *ignore_len );    // Make a key table
static int globdb_helper( const EmacsString &key, unsigned char * *rfs );    // Make the key table
#endif

//
// EQUATED SYMBOLS:
//

#define lf 10                // ASCII code for newline
#define PREFIX_BUF_SIZE 300        // Size of most buffers

//
// OWN STORAGE:
//

static struct dbsearch *dbs;        // Database search list in use
static int dbs_count;            // Database counter
static EmacsString savedstr;        // current table string

static int glob_flag;            // Version flag for get_file_list
static int glob_first;            // filename cache purge flag for get_file_list

//
// EXTERNAL REFERENCES:
//

int ignore_version_numbers = 0;
EmacsString *vecfiles [MAXFILES + 3];
EmacsString *vec_db [MAXFILES + 3];
int expand;
int help;



// "getword - Prompt for and get a completed word"
int getword ( EmacsString * *table_ref, const EmacsString &prompt )
//++
// FUNCTIONAL DESCRIPTION:
//
//    Thie routine is used to fetch a words from the user. It
//    prompts in the Mini-buffer with the string s, and expands and
//    completes one of the words in the table passed.
//
// FORMAL PARAMETERS:
//
//    table_ref is the address of a location containing the address
//    of the table
//
//    s is the prompt string material
//
// IMPLICIT INPUTS:
//
//    bf_cur is used to find the current buffer
//
//    wn_cur is used to find the current window
//
//    remove_help_window is used to decide what to do with help
//    windows on exit.
//
//    cur_exec is used to decide if the call is interactive or not.
//
//    auto_help is used to decide if a help window should be popped
//    upon errors.
//
//    bf_modified is used to set buffers as unmodified
//
// IMPLICIT OUTPUTS:
//
//    NONE
//
// ROUTINE VALUE:
//
//    The table index of thematches string is returned, or
//    -1 to indicate that the operation as aborted
//
// SIDE EFFECTS:
//
//    help windows may be popped up. The keyboard processor is
//    called, so anything could happen!
//
{
    int maxok;        // max legal chars from prefix
    int matched;        // Chars that didnt match
    int len;        // Length of current input
    EmacsString * *table;    // Pointer to table of strings
    EmacsString current_string;// pointer to current word

    int p;            // The loop counter to scan table
    int bestp = -1;        // The best version of p
    int nfound;        // Number of matching words found
    Marker *old = NULL;    // old position of dot

    EmacsString prefix;    // the current prefix string

    EmacsString *ptr;    // A pointer to the current table entry
    int side;
    int saved_windows = 0;    // true if windows should be saved

    int old_expand = expand;    // Saved value of expand flag
    int old_help = help;        // Saved value of help flag

    int longest_string;

    //
    // Get words from the user until one is found to match
    //
    prefix = "";
    table = table_ref;

    // find the longest string for making a nice display
    for( longest_string=0, p=0; table[p] != NULL; p++ )
        longest_string = max( longest_string, table[p]->length() );
    longest_string += 2;

    for(;;)
    {
        help = expand = 0;
        nfound = 0;
        maxok = 0;
        current_string = br_getstr (1, prefix, prompt );
        prefix = "";
//        if( current_string == 0 )
//                break;

        len = current_string.length();
        if( !help )
        {
            //
            // Scan the table to see what matches we have.
            //
            p = 0;
            while( (ptr = table[p]) != NULL && !prefix.isNull() )
            {
                matched = current_string.commonPrefix( *ptr );
                if( matched < current_string.length() )
                {
                    if( matched > maxok )
                        maxok = matched;
                }
                else
                {
                    //
                    // We have found a match. If its the first, then copy the
                    // match as the next prefix string.
                    // If its not the first, then shorten the prefix string to
                    // the bit that unambiguous
                    //

                    nfound++;
                    if( matched > maxok )
                        maxok = matched;
                    if( nfound == 1 )
                        prefix = *ptr;
                    else
                        prefix.remove( prefix.commonPrefix( *ptr ) );

                    bestp = p;
                    if( ptr->length() == current_string.length() )
                        //
                        // An exact match has been found, so lets exit
                        //
                    {
                        nfound = 1;
                        break;
                    }
                }
                p++;
            }
        }
        //
        // We have search all of the items specified. If we only have one item,
        // then lets get and return it. Otherwise we will have to show some help
        // or worse.
        //
        if( nfound == 1 )
            break;

        bestp = -1;

//        if( nfound == 0
//        || prefix.commonPrefix( current_string ) == len )
        {
            //
            // The input string is now so way off that no prefix strings matches
            // or the string is wrong in the later sections. Either way, the poor
            // user needs some help
            //
            if( !interactive() )
            {
                //
                // The error occurred non-interactively, report the error
                // directly and exit
                //
                bestp = -1;
                error( FormatString("\"%s\" %s") << current_string <<
                    (nfound == 0 ?
                        "does not make any sense to me."
                    :
                        "is ambiguous.") );
                break;
            }
            if( !auto_help && !help )
            {
                //
                // The user recons to be a wizard, and has help-on-command-completion-error
                // switches off, and he did not ask for help. Just bell at him, and
                // remove all the erroneous characters.
                //
                ding ();

                if( nfound == 0 )
                    prefix = current_string( 0, maxok );
            }
            else
            {
                //
                // Pop up a help window, and fill it full of stuff for the
                // user to see.
                //
                unsigned char *msg;

                if( old == NULL )
                {
                    old = EMACS_NEW Marker( bf_cur, 1, 0 );
                    if( remove_help_window && cur_exec == NULL )
                    {
                        saved_windows = 1;
                        push_window_ring();
                    }
                }

                EmacsBuffer::scratch_bfn( "Help", 1 );

                //
                // Work out what to prompt with next time round, and what
                // message to spit at the user in the help buffer
                //

                if( help )
                {
                    prefix = current_string( 0, maxok );
                    msg = u_str("Choose one of the following:\n");
                }
                else
                    if( nfound > 1 )
                        msg = u_str("Ambiguous, choose one of the following:\n");
                    else
                    {
                        len = 0;
                        msg = u_str("Please use one of the following words:\n");
                    }
                ins_str (msg);

                //
                // Generate the help buffer full of suitable strings
                //
                side = 2;    // number of columns to test for room for
                for( p=0; table[p] != NULL; p++ )
                {
                    if( maxok <= 0
                    || current_string.commonPrefix( *table[p] ) == maxok )
                    {
                        EmacsString buf;

                        // make sure the help fits in the windows
                        if( side*longest_string <= wn_cur->w_width )
                        {
                            // there is room for this one and the next at the end
                            side++;
                            buf = FormatString("%-*s ") << longest_string << *table[p];
                        }
                        else
                        {
                            // room for just this one
                            side = 2;
                            buf = *table[p]; buf.append( "\n" );
                        }
                        ins_cstr( buf );
                    }
                }
                //
                // Now that the help buffer is full of useful information,
                // display it to the user
                //
                beginning_of_file ();
                bf_modified = 0;
            }
        }
    }

    //
    // Everything is now complete, one way or another. So, lets tidy up any
    // mess we made of the screen, spit out the text of the word chosen and
    // get on with it
    //
    if( old != NULL && old->m_buf != NULL )
        theActiveView->window_on (old->m_buf);

    if( saved_windows )
        pop_window_ring ();
    if( old != NULL )
        delete old;
    help = old_help;
    expand = old_expand;
    if( interactive() && bestp >= 0 )
        message( FormatString("%s%s") << prompt << *table[bestp] );

    return bestp;
}                // Of routine getword


// "getnsword - Get a word without space or tab filling"
static EmacsString getnsword
        (
        EmacsString **table,
        EmacsString ** (*table_maker)( const EmacsString &, int * ),
        const EmacsString &prompt
        )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine is used to fetch a completed word from the
//    minibuffer after prompting with the supplied strings.
//    table is the list of acceptable words. If it is zero,
//    then table_maker is called as a routine to build the table,
//    and the result is used as the table of words
//
// FORMAL PARAMETERS:
//
//    table is the address of the table of
//    strings against which to match.
//
//    table_maker make the table
//
//    s is the address of the prompt string format
//
// IMPLICIT INPUTS:
//
//    bf_cur is used to find the current buffer
//
//    wn_cur is used to find the current window
//
//    remove_help_window is used to decide what to do with help
//    windows on exit.
//
//    cur_exec is used to decide if the call is interactive or not.
//
//    auto_help is used to decide if a help window should be popped
//    upon errors.
//
//    bf_modified is used to set buffers as unmodified
//
// IMPLICIT OUTPUTS:
//
//    NONE
//
// ROUTINE VALUE:
//
//    0 to indicate an error, or the address of the selected string
//
// SIDE EFFECTS:
//
//    NONE
//
{
    int maxok;            // max legal chars from prefix
    int matched;            // Chars that didnt match
    int len;            // Length of current input
    EmacsString current_string;    // pointer to current word

    int p;                // The loop counter to scan table
    int bestp = -1;            // The best version of p
    int nfound;            // Number of matching words found
    int force_exit = 0;        // A loop counter
    Marker *old = NULL;        // old position of dot

    EmacsString prefix;        // the current prefix string

    EmacsString *ptr;        // A pointer to the current table entry
    int side;
    int saved_windows = 0;        // true if windows should be saved

    int old_expand = expand;    // Saved value of expand flag
    int old_help = help;        // Saved value of help flag

    int longest_string;

    // find the longest string for making a nice display
    for( longest_string=0, p=0; table != NULL && table[p] != NULL; p++ )
        longest_string = max( longest_string, table[p]->length() );
    longest_string += 2;

    //
    // Get words from the user until one is found to match
    //
    prefix = "";

    for(;;)
    {
        help = expand = 0;
        nfound = 0;
        maxok = 0;
        current_string = br_getstr (1, prefix, prompt );
        prefix = "";
//        if( current_string == 0 )
//            break;

        //
        // Deal with expand-and-exit cases and non-interactive cases. These
        // just return the typed value
        //
        if( cur_exec != NULL || (!expand && !help) )
        {
            force_exit = 1;
            break;
        }

        //
        // Make the table if we have to make tables more than once
        // [Note that the current_string is written back into]
        //
        if( table_maker != NULL )
        {
            table = table_maker( current_string, &longest_string );
            longest_string +=2;
        }

        len = current_string.length();

        //
        // expand the word as the user requested
        //
        if( expand )
        {
            //
            // Scan the table to see what matches we have.
            //
            p = 0;
            while( (ptr = table[p]) != NULL )
            {
                matched = current_string.commonPrefix( *ptr );
                if( matched < current_string.length() )
                {
                    if( matched > maxok )
                        maxok = matched;
                }
                else
                {
                    //
                    // We have found a match. If its the first, then copy the
                    // match as the next prefix string.
                    // If its not the first, then shorten the prefix string to
                    // the bit that unambiguous
                    //

                    nfound++;
                    if( len > maxok )
                        maxok = len;
                    if( nfound == 1 )
                        prefix = *ptr;
                    else
                        prefix.remove( prefix.commonPrefix( *ptr ) );

                    bestp = p;
                    if( ptr->length() == current_string.length() )
                        //
                        // An exact match has been found, so lets exit
                        //
                    {
                        nfound = 1;
                        break;
                    }
                }
                p++;
            }
        }
        //
        // We have searched all of the items specified.
        // If we only have one item, then lets get and
        // return it. Otherwise we will have to show some help
        // or worse.
        //
        if( nfound == 1 )
            break;

        bestp = -1;
//        if( nfound <= 1
//        || _str_ncmp( current_string, prefix, len + 1 ) == 0)
        {
            //
            // The input string is now so way off that no prefix strings matches
            // or the string is wrong in the later sections. Either way, the poor
            // user needs some help
            //
            if( !interactive() )
            {
                //
                // The error occured non-interactively,
                // report the error directly and exit
                //
                bestp = -1;
                error( FormatString("\"%s\" %s") << current_string <<
                    (nfound == 0 ?
                        "does not make any sense to me."
                    :
                        "is ambiguous.") );
                break;
            }
            if( !auto_help && !help )
            {
                //
                // The user recons to be a wizard, and has help-on-command-completion-error
                // switches of, and he didnt ask for help. Just bell at him, and
                // remove all the erroneous characters.
                //
                ding ();
                if( nfound == 0 )
                    prefix = current_string( 0, maxok );
            }
            else
            {
                //
                // Pop up a help window, and fill it full of stuff
                // for the user to see.
                //
                unsigned char *msg;

                if( old == NULL )
                {
                    old = EMACS_NEW Marker( bf_cur, 1, 0 );
                    if( remove_help_window != 0 && cur_exec == 0 )
                    {
                        saved_windows = 1;
                        push_window_ring ();
                    }
                }
                EmacsBuffer::scratch_bfn( "Help", 1 );

                //
                // Work out what to prompt with next time round, and
                // what message to spit at the user in the help buffer
                //
                if( help != 0 )
                {
                    prefix = current_string( 0, maxok );
                    msg = u_str("Choose one of the following:\n");
                }
                else
                    if( nfound > 1 )
                        msg = u_str("Ambiguous, choose one of the following:\n");
                    else
                    {
                        len = 0;
                        msg = u_str("Please use one of the following words:\n");
                    }
                ins_str (msg);

                //
                // Generate the help buffer full of suitable strings
                //
                side = 2;    // number of columns to test for room for
                for( p=0; table[p] != NULL; p++ )
                {
                    if( maxok <= 0
                    || current_string.commonPrefix( *table[p] ) == maxok )
                    {
                        EmacsString buf;

                        // make sure the help fits in the windows
                        if( side*longest_string <= wn_cur->w_width )
                        {
                            // there is room for this one and the next at the end
                            side++;
                            buf = FormatString("%-*s ") << longest_string << *table[p];
                        }
                        else
                        {
                            // room for just this one
                            side = 2;
                            buf = FormatString("%s\n") << *table[p];
                        }
                        ins_cstr( buf );
                    }
                }
                //
                // Now that the help buffer is full of useful information,
                // display it to the user
                //
                beginning_of_file ();
                bf_modified = 0;
            }
        }
    }

    //
    // Everything is now complete, one way or another. So, lets tidy up any
    // mess we made of the screen, spit out the text of the word chosen and
    // get on with it
    //
    if( old != NULL && old->m_buf != 0 )
        theActiveView->window_on (old->m_buf);
    if( saved_windows )
        pop_window_ring ();
    if( old != NULL )
        delete old;
    help = old_help;
    expand = old_expand;
    if( interactive() && bestp >= 0 || force_exit )
        message (FormatString("%s%s") << prompt <<
            (force_exit ? current_string : *table[bestp]) );

    //
    // return the actual text that caused the exit, or zero for aborts
    //
    if( (! interactive() || force_exit) )
        return current_string;
    else
        if( current_string.length() > 0 )
            return *table[bestp];
        else
            return EmacsString();
}                // Of routine getnsword


// "getescword - Get an escape filled word"
EmacsString getescword( EmacsString * *table_ref, const EmacsString &prompt )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine uses getnsword to fetch a none space expanded
//    word from the user
//
// FORMAL PARAMETERS:
//
//    table_ref is the address of a word containing the address of
//    the table of words
//
//    s is the prompt format string
//
// IMPLICIT INPUTS:
//
//    NONE
//
// IMPLICIT OUTPUTS:
//
//    NONE
//
// ROUTINE VALUE:
//
//    The string selected by the user, or zero to indicate an abort
//
// SIDE EFFECTS:
//
//    NONE
//
{
    return getnsword (table_ref, NULL, prompt );
}

// "getescfile - gets a filename which may be ESC expanded"
EmacsString getescfile( const EmacsString &prompt, int PNOTUSED(dirs), int vers )
//
// FUNCTIONAL DESCRIPTION:
//
//    This routine gets a filename from the user. A table of possible
//    filenames is generated from which the user should select. If
//    he wants a file that is not in the list, then he types in
//    the string and it is returned.
//
// FORMAL PARAMETERS:
//
//    flag is used to indicate if version numbers should be expanded
//
//    s is the format string for the prompt
//
// IMPLICIT INPUTS:
//
//    NONE
//
// IMPLICIT OUTPUTS:
//
//    NONE
//
// ROUTINE VALUE:
//
//    0 to indicate that the user aborted the operation, or the
//    address of the selected string
//
// SIDE EFFECTS:
//
//    NONE
//
{
    EmacsString **table = NULL;
    int maxok;            // max legal chars from prefix
    int matched;            // Chars that didnt match
    int len;            // Length of current input
    int longest_string = 0;        // Prefix length to skip in help window
    EmacsString current_string;    // pointer to current word

    int p;                // The loop counter to scan table
    int bestp = -1;            // The best version of p
    int nfound;            // Number of matching words found
    int force_exit = 0;        // A loop counter
    Marker *old = 0;        // old position of dot

    EmacsString prefix; // the current prefix string

    EmacsString *ptr;        // A pointer to the current table entry
    int side;
    int saved_windows = 0;        // true if windows should be saved

    int old_expand = expand;    // Saved value of expand flag
    int old_help = help;        // Saved value of help flag

    int old_flag = glob_flag;    // version flag saved value
    int old_glob_first = glob_first;// global_first cache purger

    glob_flag = ignore_version_numbers ? 0 : vers;
    glob_first = 1;

    //
    // Get words from the user until one is found to match
    //
    prefix = "";
    for(;;)
    {
        help = expand = 0;
        nfound = 0;
        maxok = 0;
        current_string = br_getstr (1, prefix, prompt );
        prefix = "";
        if( current_string.isNull() )
            break;

        //
        // Deal with expand-and-exit cases and non-interactive cases. These
        // just return the typed value
        //
        if( cur_exec != NULL || (!expand && !help) )
        {
            force_exit = 1;
            break;
        }

        //
        // Make the table if we have to make tables more than once
        // [Note that the current_string is written back into]
        //
        table = get_file_list( current_string, &longest_string );
        longest_string += 2;

        len = current_string.length();

        //
        // expand the word as the user requested
        //
        if( expand )
        {
            //
            // Scan the table to see what matches we have.
            //
            p = 0;
            while( (ptr = table[p]) != NULL )
            {
                matched = current_string.commonPrefix( *ptr );
                if( matched < current_string.length() )
                {
                    if( matched > maxok )
                        maxok = matched;
                }
                else
                {
                    //
                    // We have found a match. If its the first, then copy the
                    // match as the next prefix string.
                    // If its not the first, then shorten the prefix string to
                    // the bit that unambiguous
                    //

                    nfound++;
                    if( len > maxok ) maxok = len;
                    if( nfound == 1 )
                        prefix = *ptr;
                    else
                        prefix.remove( prefix.commonPrefix( *ptr ) );

                    bestp = p;
                    if( ptr->length() == current_string.length() )
                        //
                        // An exact match has been found, so lets exit
                        //
                    {
                        nfound = 1;
                        break;
                    }
                }
                p++;
            }
            if( nfound == 1 && file_is_directory( *table[bestp] ) )
                continue;
        }
        //
        // We have searched all of the items specified.
        // If we only have one item, then lets get and
        // return it. Otherwise we will have to show some help
        // or worse.
        //
        if( nfound == 1 )
            break;

        bestp = -1;
//        if( nfound <= 1
//        || _str_ncmp( current_string, prefix, len + 1 ) == 0)
        {
            //
            // The input string is now so way off that no prefix strings matches
            // or the string is wrong in the later sections. Either way, the poor
            // user needs some help
            //
            if( ! interactive() )
            {
                //
                // The error occured non-interactively,
                // report the error directly and exit
                //
                bestp = -1;
                error( FormatString("\"%s\" %s") << current_string <<
                    (nfound == 0 ?
                        "does not make any sense to me."
                    :
                        "is ambiguous.") );
                break;
            }
            if( !auto_help && !help )
            {
                //
                // The user recons to be a wizard, and has help-on-command-completion-error
                // switches of, and he didnt ask for help. Just bell at him, and
                // remove all the erroneous characters.
                //
                ding ();
                if( nfound == 0 )
                    prefix = current_string( 0, maxok );
            }
            else
            {
                //
                // Pop up a help window, and fill it full of stuff
                // for the user to see.
                //
                unsigned char *msg;

                if( old == NULL )
                {
                    old = EMACS_NEW Marker( bf_cur, 1, 0 );
                    if( remove_help_window != 0 && cur_exec == 0 )
                    {
                        saved_windows = 1;
                        push_window_ring ();
                    }
                }
                EmacsBuffer::scratch_bfn( "Help", 1 );

                //
                // Work out what to prompt with next time round, and
                // what message to spit at the user in the help buffer
                //
                if( help != 0 )
                {
                    prefix = current_string( 0, maxok );
                    msg = u_str("Choose one of the following:\n");
                }
                else
                    if( nfound > 1 )
                        msg = u_str("Ambiguous, choose one of the following:\n");
                    else
                    {
                        len = 0;
                        msg = u_str("Please use one of the following words:\n");
                    }
                ins_str (msg);

                //
                // Generate the help buffer full of suitable strings
                //
                side = 2;    // number of columns to test for room for
                for( p=0; table[p] != NULL; p++ )
                {
                    if( len <= 0
                    || current_string.commonPrefix( *table[p] ) == maxok )
                    {
                        EmacsString buf;

                        // make sure the help fits in the windows
                        if( side*longest_string <= wn_cur->w_width )
                        {
                            // there is room for this one and the next at the end
                            side++;
                            buf = FormatString("%-*s ") << longest_string << *table[p];
                        }
                        else
                        {
                            // room for just this one
                            side = 2;
                            buf = FormatString("%s\n") << *table[p];
                        }
                        ins_cstr( buf );
                    }
                }
                //
                // Now that the help buffer is full of useful information,
                // display it to the user
                //
                beginning_of_file ();
                bf_modified = 0;
            }
        }
    }

    //
    // Everything is now complete, one way or another. So, lets tidy up any
    // mess we made of the screen, spit out the text of the word chosen and
    // get on with it
    //
    if( old != NULL && old->m_buf != NULL )
        theActiveView->window_on (old->m_buf);
    if( saved_windows )
        pop_window_ring ();
    if( old != NULL )
        delete old;
    help = old_help;
    expand = old_expand;
    if( interactive() && bestp >= 0 || force_exit )
        message( FormatString("%s%s") << prompt << (force_exit ? current_string : *table[bestp]) );

    glob_flag = old_flag;
    glob_first = !glob_first ? 1 : old_glob_first;

    //
    // return the actual text that caused the exit, or zero for aborts
    //
    if( !interactive() || force_exit )
        return current_string;
    else
        if( current_string.length() > 0 )
            return *table[bestp];
        else
            return null_str;
}


#ifdef vms
// "get_file_list - expand a filespec into a table of filenames"
int qsort_strcmp( const void *arg1, const void *arg2 )
{
    return strcmp( * ( char** ) arg1, * ( char** ) arg2 );
}


static EmacsString ** get_file_list( EmacsString file, int *max_string_len )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine checks that the current filename table still
//    works for the typed in filename. If it does, then it returned
//    having done nothing. Otherwise, it creates the filename table
//    and caches it.
//
// FORMAL PARAMETERS:
//
//    file is the name of the filespec to check_
//
//    max_string_len is the location to fill with the length up to the
//    end of the directory spec
//
// IMPLICIT INPUTS:
//
//    glob_first is used to see if the filename chache must be
//    purged. A new file cache is created every time getescfile is
//    used.
//
//    glob_flag is used to decide if version numbers must be
//    expanded
//
// IMPLICIT OUTPUTS:
//
//    glob_flag is set to zero
//
// ROUTINE VALUE:
//
//    The address of the filename table generated
//
// SIDE EFFECTS:
//
//    Any old filename table is destroyed
//
{
    int resp;
    int count;            // Loop counter
    int len = _str_len(file);    // The length of the file string
    FileParse fab;
    int dir_spec_len;
    int i;

    //
    // All files have now been generated, so add the final 0 to end
    // the table in the conventional way
    //
    for( count = 0; count < MAXFILES; count++ )
        if( vecfiles[count] != NULL )
            free( vecfiles[count] );

    count = 0;
    memset( &vecfiles, 0, sizeof( vecfiles ) );

    *max_string_len = 0; // start off with zero length

     init_fab( &fab );
#if 0
    // if we can parse what we have then make that the prompt
    if( sys_parse( file, u_str(""), &fab ) )
    {
        _str_cpy( file, fab.result_spec );
        if( !fab.file_case_sensitive )
            _str_lwr( file );
    }

    // need the 'file' with a wild * on the end
    _str_cpy( savedstr, file );
    _str_cat( savedstr, "*" );

    // Expand to a full path
    resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
    if( !resp )
    {
        // opss thats a bad path...
        // just return the current directory contents
        savedstr[savedlen] = 0;
        resp = sys_parse( savedstr, u_str(ALL_FILES), &fab );
    }
    if( resp )
#else
    // need the 'file' with a wild * on the end
    _str_cpy( savedstr, file );
    _str_cat( savedstr, "*" );
    fab.result_spec = savestr( savedstr );

    dir_spec_len = 0;
    for( i=0; savedstr[i] != '\0'; i++ )
        if( savedstr[i] == ']'
        || savedstr[i] == '>'
        || savedstr[i] == ':' )
            dir_spec_len = i+1;
#endif

    {
        unsigned char *file;
        void *handle;
        int status;

        //
        // For each file that matches the filespec, save the name
        // away in the table. Make sure old entries in the table are
        // deallocated first
        //
        handle = find_first_file( fab.result_spec, &file );
        status = handle != NULL;

        count = 0;
        while( status && count < MAXFILES )
        {
            unsigned char *ptr;    // Pointer into allocated memory
            int cpy;        // Length of component to copy

            //
            // Find where in the original string, the directory spec
            // ended. The string to the left will be added as the directory
            // spec for each file in the table. This is so that expanded
            // logical names etc, do not cause any grief
            //

            for( i=0; file[i] != '\0'; i++ )
                if( file[i] == ']'
                || file[i] == '>'
                || file[i] == ':' )
                    cpy = i+1;

            //
            // Allocate space for the new string
            //
            ptr = malloc_ustr( _str_len( file ) - cpy + dir_spec_len + 1 );
            if( ptr == NULL )
                break;

            //
            // Make up the new string, and copy it
            //
            _str_ncpy( ptr, savedstr, dir_spec_len );
            _str_cpy( &ptr[dir_spec_len], &file[cpy] );

            vecfiles[count] = ptr;

            cpy = _str_len( ptr );
            if( cpy > *max_string_len )
                *max_string_len = cpy;

            count++;

#if 0
            if( count == 1 )
            {
                ptr = malloc( cpy + 1 + 3 );
                if( ptr == 0 )
                    break;

                //
                // Make up the new string, and copy it
                //
                _str_cpy( ptr, file );
                vecfiles[1] = ptr;

                // find the end of the directory
                ptr = _str_chr( ptr, ']' );
                if( ptr != NULL )
                    // fake up an file to go up a level
                    _str_cpy( ptr, "][-]" );

                cpy = _str_len( vecfiles[1] );
                if( cpy > *dir_spec_len )
                    *dir_spec_len = cpy;

                count++;
            }
#endif
            status = find_next_file( handle, &file );
        }
        if( handle != NULL )
            find_file_close( handle );
    }

    qsort
    (
    (void *)vecfiles, (size_t)count, sizeof( unsigned char * ),
    qsort_strcmp
    );

    vecfiles[count] = 0;

    return vecfiles;
}                // Of routine get_file_list
#else
// "get_file_list - expand a filespec into a table of filenames"
int qsort_strcmp( const void *arg1, const void *arg2 )
{
    EmacsString *str1 = (EmacsString *)arg1;
    EmacsString *str2 = (EmacsString *)arg2;

    return str1->compare( *str2 );
}


static EmacsString ** get_file_list( EmacsString file, int *longest_filename )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine checks that the current filename table still
//    works for the typed in filename. If it does, then it returned
//    having done nothing. Otherwise, it creates the filename table
//    and caches it.
//
// FORMAL PARAMETERS:
//
//    file is the name of the filespec to check_
//
//    longest_filename is the length of the longest filename
//
// IMPLICIT INPUTS:
//
//    glob_first is used to see if the filename chache must be
//    purged. A new file cache is created every time getescfile is
//    used.
//
//    glob_flag is used to decide if version numbers must be
//    expanded
//
// IMPLICIT OUTPUTS:
//
//    glob_flag is set to zero
//
// ROUTINE VALUE:
//
//    The address of the filename table generated
//
// SIDE EFFECTS:
//
//    Any old filename table is destroyed
//
{
    int resp;
    int count;            // Loop counter
    int len = file.length();    // The length of the file string

    //
    // All files have now been generated, so add the final 0 to end
    // the table in the conventional way
    //
    for( count = 0; count < MAXFILES; count++ )
        delete vecfiles[count];

    count = 0;
    memset( &vecfiles, 0, sizeof( vecfiles ) );

    *longest_filename = 0; // start off with zero length

    FileParse fab;
    // if we can parse what we have then make that the prompt
    if( fab.sys_parse( file, "" ) )
    {
        file = fab.result_spec;
        if( file_is_directory( file )
        && file[file.length()-1] != PATH_CH )
            file.append( PATH_STR );
        if( !fab.file_case_sensitive )
            file.toLower();
    }

    // need the 'file' with a wild * on the end
    savedstr = file;
    savedstr.append( "*" );

    // Expand to a full path
    resp = fab.sys_parse( savedstr, ALL_FILES );
    if( !resp )
    {
        // opss thats a bad path...
        // just return the current directory contents
        savedstr = "";
        resp = fab.sys_parse( savedstr, ALL_FILES );
    }
    if( resp )
    {
        unsigned char *next_file;
        void *handle;
        int status;

        //
        // For each file that matches the filespec, save the name
        // away in the table. Make sure old entries in the table are
        // deallocated first
        //
        handle = find_first_file( fab.result_spec, &next_file );
        status = handle != NULL;

        count = 0;
        while( status && count < MAXFILES )
        {
            EmacsString *ptr;    // Pointer into allocated memory

            ptr = EMACS_NEW EmacsString( next_file );
            if( ptr == NULL )
                break;

            //
            // Find where in the original string, the directory spec
            // ended. The string to the left will be added as the directory
            // spec for each file in the table. This is so that expanded
            // logical names etc, do not cause any grief
            //
            if( ptr->length() > *longest_filename )
                *longest_filename = ptr->length();

            vecfiles[count] = ptr;

            count++;

            status = find_next_file( handle, &next_file );
        }
        if( handle != NULL )
            find_file_close( handle );
    }

    qsort
    (
    (void *)vecfiles, (size_t)count, sizeof( EmacsString * ),
    qsort_strcmp
    );

    vecfiles[count] = NULL;

    return vecfiles;
}                // Of routine get_file_list
#endif


// "getescdb - fetch an expanded db key"
EmacsString getescdb ( struct dbsearch *current_dbs, const EmacsString &prompt )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine is used to fetch a database search list key
//    name. It enables full expansion of key names from the DB
//    search list. Either one of the names can be used, or a
//    completly new name generated
//
// FORMAL PARAMETERS:
//
//    dbs is the address of the database search list to use.
//
//    s is the address of the prompt string format
//
// IMPLICIT INPUTS:
//
//    NONE
//
// IMPLICIT OUTPUTS:
//
//    glob_first is altered as a table is generated
//
// ROUTINE VALUE:
//
//    The address of the selected string, or zero indicating that
//    the user aborted the operations
//
// SIDE EFFECTS:
//
//    vecfiles is trashed, savedstr and savedlen are also trashed
//
{
#ifdef DB
    struct dbsearch *old_dbs = dbs;        // The current DBS
    int old_glob_first = glob_first;    // global_first cache purger
    EmacsString resp;            // Result of helper function

    glob_first = 1;
    dbs = current_dbs;

    resp = getnsword (0, globdb, prompt );

    glob_first = !glob_first ? 1 : old_glob_first;
    dbs = old_dbs;

    return resp;
#else
    error( "Not implemented" );
    return 0;
#endif
}                // Of routine getescdb


#ifdef DB
// "globdb - generate a table of db search list keys"
static EmacsString ** globdb( const EmacsString &key, int *ignore_len )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine actually generates the keyname list. it is
//    called when expansion is required. It check_s the glob_first
//    flag, and created a new list is required, otherwise, set
//    matches the last string used, to see in the new string starts
//    with the old. If it does, it just returns. Otherwise, it has
//    to generate a new table.
//
// FORMAL PARAMETERS:
//
//    key is the address of the start of the key
//
//    ignore_len must be set to zero
//
// IMPLICIT INPUTS:
//
//    NONE
//
// IMPLICIT OUTPUTS:
//
//    savedstr contains the saved key prefix
//
//    savedlen shows the usage of savedstr
//
// ROUTINE VALUE:
//
//    NONE
//
// SIDE EFFECTS:
//
//    A new key table is generated
//
{
    int count;
    int len = key.length();    // The length of the file string

    //
    // All files have now been generated, so add the final 0 to end
    // the table in the conventional way
    //
    for( count = 0; count < MAXFILES; count++ )
        delete vec_db[count];

    count = 0;
    memset( &vec_db, 0, sizeof( vec_db ) );

    dbs_count = 0;

    //
    // The cache is invalid, so create a new table, trashing the
    // old as we go along.
    //
    savedstr = key;
    savedstr.append( "*" );
    fetch_database_index( dbs, savedstr, globdb_helper);

    *ignore_len = 0;
    for( count = 0; vec_db[count] != NULL; count++ )
    {
        int len = vec_db[count]->length();
        if( len > *ignore_len )
            *ignore_len = len;
    }

    qsort
    (
    (void *)vec_db, (size_t)dbs_count, sizeof( EmacsString * ),
    qsort_strcmp
    );

    return vec_db;
}                // Of routine globdb

// "globdb_helper - make each key entry in the key table"
static int globdb_helper( const EmacsString &key, unsigned char * * )
//++
// FUNCTIONAL DESCRIPTION:
//
//    This routine is called one for each key found to match the
//    starting string. Its job is to save the key in the table, and
//    increase the counter. The key is converted to lower case as
//    it is moved into dynamic memory.
//
// FORMAL PARAMETERS:
//
//    key is the string descriptor of the current key
//
// IMPLICIT INPUTS:
//
//    dbs_count is the location in vecfile where the current key
//    should be located.
//
// IMPLICIT OUTPUTS:
//
//    dbs_count is increased by one as required.
//
// ROUTINE VALUE:
//
//    1 if the copy took place, otherwise, 0
//
// SIDE EFFECTS:
//
//    NONE
//
{
    EmacsString *ptr;

    //
    // First check that the table has not overflowed
    //
    if( dbs_count >= MAXFILES )
        return 0;

    //
    // Allocate space for the new string
    //
    if( (ptr = vec_db[dbs_count] = EMACS_NEW EmacsString( key )) == NULL )
        return 0;

    ptr->toLower();

    dbs_count++;

    return 1;
}                // Of routine globdb_helper
#endif
