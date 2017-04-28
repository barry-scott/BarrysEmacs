//
//
//    Emacs V7.0 emacs string table
//
//    Copyright 1995-1997 Barry A. Scott
//
#include <emacs.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

SystemExpressionRepresentationIntBoolean complete_unique_choices( 0 );
SystemExpressionRepresentationIntBoolean confirm_expansion_choice( 0 );

int expand = 0;
int help = 0;

unsigned char *msg_choose_one = u_str("Choose one of the following:\n");
unsigned char *msg_ambiguous_choose_one = u_str("Ambiguous, choose one of the following:\n");
unsigned char *msg_please_use_one = u_str("Please use one of the following words:\n");

EmacsStringTable::EmacsStringTable( int initial_size, int growth )
    : num_entries( 0 )
    , allocated_entries( initial_size )
    , grow_by( std::min( growth, 32 ) )
{
    keys = (EmacsString **)EMACS_MALLOC( allocated_entries * sizeof( EmacsString * ), malloc_type_star_star );
    values = (void **)EMACS_MALLOC( allocated_entries * sizeof( void * ), malloc_type_star_star );
}

EmacsStringTable::~EmacsStringTable()
{
    emptyTable();

    EMACS_FREE( keys );
    EMACS_FREE( values );
}

void EmacsStringTable::emptyTable()
{
    for( int index=0; index<num_entries; index++ )
    {
        delete keys[ index ];
        // prevent dangling references
        keys[index] = NULL;
        values[index] = NULL;
    }

    // mark as empty
    num_entries = 0;
}

int EmacsStringTable::findIndex( const EmacsString &key )
{
    int lo = 0;
    int hi = num_entries - 1;

    // test only required for the first call
    if( hi < 0 )
        return -1;

    while( lo <= hi )
    {
        int mid = (lo + hi) >> 1;
        int relation = compareKeys( key, *keys[mid] );
        if( relation == 0 )
            return mid;
        if( relation < 0 )
            hi = mid - 1;
        else
            lo = mid + 1;
    }

    return - lo - 1;
}

void EmacsStringTable::add( const EmacsString &key, void *value )
{
    // see if the table is full
    if( num_entries == allocated_entries )
    {
        // expand the table
        allocated_entries += grow_by;

        keys = (EmacsString **)EMACS_REALLOC( keys, allocated_entries * sizeof( EmacsString * ), malloc_type_star_star );
        values = (void **)EMACS_REALLOC( values, allocated_entries * sizeof( void * ), malloc_type_star_star );
    }

    int index = findIndex( key );

    emacs_assert( index < 0 );    // assert that the key is not found

    index = -index - 1;

    for( int entry=num_entries; entry >= index; entry-- )
    {
        keys[ entry ] = keys[ entry-1 ];
        values[ entry ] = values[ entry-1 ];
    }

    num_entries++;

    keys[ index ] = EMACS_NEW EmacsString( key );
    values[ index ] = value;
}

void *EmacsStringTable::remove( const EmacsString &key )
{
    int index = findIndex( key );
    if( index < 0 )
        return NULL;

    void *result = values[ index ];

    delete keys[ index ];

    num_entries--;
    for( ; index<num_entries; index++ )
    {
        keys[ index ] = keys[ index+1 ];
        values[ index ] = values[ index+1 ];
    }

    // prevent dangling references
    keys[index] = NULL;
    values[index] = NULL;

    return result;
}

void *EmacsStringTable::find( const EmacsString &key )
{
    int index = findIndex( key );
    if( index < 0 )
        return NULL;
    else
        return values[ index ];
}

// return the key at slot index
const EmacsString *EmacsStringTable::key( int index )
{
    emacs_assert( index >= 0 && index < num_entries );

    return keys[index];
}

// return the value at slot index
void *EmacsStringTable::value( int index )
{
    emacs_assert( index >= 0 && index < num_entries );

    return values[index];
}

// override to do things like case blind compare
int EmacsStringTable::compareKeys( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.compare( string2 );
}

int EmacsStringTable::commonPrefix( const EmacsString &string1, const EmacsString &string2 )
{
    return string1.commonPrefix( string2 );
}

// return the number of entries in the table that match prefix
// retrun in matchingString the longest common prefix of the matching
// entries. This can be longer or shorter then prefix.
int EmacsStringTable::match( const EmacsString &prefix, EmacsString &matchingString )
{
    if( prefix.isNull() )
    {
        matchingString = prefix;
        return num_entries;
    }

    // find a place in the table that has the first char of
    // prefix in its first place
    EmacsString first_char = prefix(0,1);

    int lo = 0;
    int hi = num_entries - 1;

    // test only required for the first call
    if( hi < 0 )
        return -1;

    int mid = 0;
    while( lo <= hi )
    {
        mid = (lo + hi) >> 1;
        EmacsString key_char = (*keys[mid])(0,1);
        int comp = compareKeys( first_char, key_char );
        if( comp == 0  )
            break;
        if( comp < 0 )
            hi = mid - 1;
        else
            lo = mid + 1;
    }

    // nothing found
    if( lo > hi )
    {
        matchingString = EmacsString::null;
        return num_entries;
    }

    // work out the number of entries that match the prefix

    lo = hi = mid;
    int longest_match = commonPrefix( prefix, *keys[mid] );
    while( lo > 0 )
    {
        lo--;

        int x = commonPrefix( prefix, *keys[lo] );
        if( x < longest_match )
        {
            lo++;
            break;
        }
        if( x > longest_match )
        {
            hi = lo;
            longest_match = x;
        }
    }

    while( hi < num_entries )
    {
        int x = commonPrefix( prefix, *keys[hi] );
        if( x < longest_match )
            break;
        if( x > longest_match )
        {
            lo = hi;
            longest_match = x;
        }

        hi++;
    }

    // see if only one match found with this prefix
    if( hi - lo == 1 && commonPrefix( prefix, *keys[lo] ) == prefix.length() )
    {
        matchingString = *keys[lo];
        return 1;
    }

    // Only if all the prefix was used...
    if( longest_match == prefix.length() )
    {
        // ...find the longest common prefix of the set of strings
        // between lo and hi-1
        longest_match = INT_MAX;

        hi--;
        for( mid=lo; mid < hi; mid++ )
        {
            int x = commonPrefix( *keys[mid], *keys[mid+1] );
            if( x < longest_match )
                longest_match = x;
        }
    }

    matchingString = (*keys[lo])( 0, longest_match );
    return hi-lo+1;
}


// return the next entry that contains str in the key
// start with index as zero
// no more matches once a NULL has been returned
const EmacsString *EmacsStringTable::apropos( const EmacsString &_str, int &index )
{
    emacs_assert( index >= 0 );

    EmacsString str( _str );
    str.toLower();

    while( index < num_entries )
    {
        EmacsString entry( *keys[index] );
        entry.toLower();
        if( strstr( entry.sdata(), str.sdata() ) != NULL )
            return keys[index++];

        index++;
    }

    return NULL;
}

void EmacsStringTable::makeTable( EmacsString & )
{ }

bool EmacsStringTable::terminalEntry( const EmacsString & )
{
    return true;
}

void EmacsStringTable::fillHelpBuffer( const EmacsString &prefix, int nfound )
{
    EmacsBuffer::scratch_bfn( "Help", 1 );

    //
    // Work out what to prompt with next time round, and what
    // message to spit at the user in the help buffer
    //
    const char *msg;
    if( help )
        msg = "Choose one of the following:\n";
    else
        if( nfound > 1 )
            msg = "Ambiguous, choose one of the following:\n";
        else
            msg = "Please use one of the following words:\n";

    bf_cur->ins_cstr( msg );

    // find the longest string for making a nice display
    int longest_string;
    int p;
    for( longest_string=0, p=0; p<num_entries; p++ )
        longest_string = std::max( longest_string, keys[p]->length() );
    longest_string += 2;

    int len = prefix.length();

    //
    // Generate the help buffer full of suitable strings
    //
    int side = 2;    // number of columns to test for room for
    for( p = 0; p < num_entries; p++ )
    {
        if( commonPrefix( prefix, *keys[p] ) >= len )
        {
            EmacsString buf;

            // make sure the help fits in the windows
            if( side*longest_string <= theActiveView->currentWindow()->w_width )
            {
                // there is room for this one and the next at the end
                side++;
                buf = FormatString("%-*s ") << longest_string << keys[p];
            }
            else
            {
                // room for just this one
                side = 2;
                buf = *keys[p]; buf.append( "\n" );
            }
            bf_cur->ins_cstr( buf );
        }
    }
    //
    // Now that the help buffer is full of useful information,
    // display it to the user
    //
    beginning_of_file ();
    bf_cur->b_modified = 0;
}

class save_windows
{
public:
    save_windows()
        : windows_pushed( false )
    { }
    ~save_windows()
    {
        if( windows_pushed )
            pop_window_ring();
    }
    void save()
    {
        push_window_ring();
        windows_pushed = true;
    }
private:
    bool windows_pushed;
};


// return one of the keys in the table otherwise NULL
EmacsString &EmacsStringTable::get_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
{
    Marker old;                         // old position of dot

    save_windows saved_windows;         // true if windows should be saved

    Save<int> old_expand( &expand );    // Saved value of expand flag
    Save<int> old_help( &help );        // Saved value of help flag

    //
    // Get words from the user until one is found to match
    //
    EmacsString current_string( default_value );

    while( quitting_emacs == 0 )
    {
        help = expand = 0;
        int nfound = 0;
        EmacsString initial_input = current_string;
        current_string = br_get_string_interactive( 1, initial_input, prompt );

        //
        // Make the table if we have to make tables more than once
        // [Note that the current_string is written back into]
        //
        makeTable( current_string );

        if( !help )
        {
            //
            // Scan the table to see what matches we have.
            //
            EmacsString longest_match;
            nfound = match( current_string, longest_match );

            // if there is an exact match set nfound to 1
            if( find( current_string ) != NULL )
            {
                if( !complete_unique_choices || !expand )
                    nfound = 1;
            }
            else
                if( current_string.length() < longest_match.length() )
                    current_string = longest_match;

            //
            // We have searched all of the items specified. If we only have one item,
            // then lets get and return it. Otherwise we will have to show some help
            // or worse.
            //
            if( nfound == 1 )
            {
                // if its an exact match return now
                if( terminalEntry( longest_match ) )
                {
                    current_string = longest_match;
                    break;
                }
            }
        }


        bool duplicate_expands = expand && current_string == initial_input;
        if( !auto_help && !help && !duplicate_expands )
        {
            //
            // The user recons to be a wizard, and has help-on-command-completion-error
            // switches off, and he did not ask for help. Just bell at him, and
            // remove all the erroneous characters.
            //
            ding();
        }
        else
        {
            //
            // Pop up a help window, and fill it full of stuff for the
            // user to see.
            //
            if( !old.isSet() )
            {
                old.set_mark( bf_cur, 1, 0 );
                if( remove_help_window && cur_exec == NULL )
                    saved_windows.save();
            }

            //
            // Scan the table to see what matches we have.
            //
            EmacsString longest_match;
            nfound = match( current_string, longest_match );

            fillHelpBuffer( longest_match, nfound );
        }
    }

    //
    // Everything is now complete, one way or another. So, lets tidy up any
    // mess we made of the screen, spit out the text of the word chosen and
    // get on with it
    //
    if( old.isSet() )
        theActiveView->window_on( old.m_buf );

    if( interactive() )
        message( FormatString("%s%s") << prompt << current_string );

    result = current_string;
    return result;
}

EmacsString &EmacsStringTable::get_word_mlisp( EmacsString &result )
{
    //
    // Get words from the user until one is found to match
    //
    EmacsString current_string = br_get_string_mlisp();

    //
    // Make the table if we have to make tables more than once
    // [Note that the current_string is written back into]
    //
    makeTable( current_string );

    // if there is an exact match set nfound to 1
    if( find( current_string ) != NULL )
    {
        result = current_string;
        return result;
    }

    //
    // The error occurred non-interactively, report the error
    // directly and exit
    //

    //
    // Scan the table to see what matches we have.
    //
    EmacsString matched_prefix;
    int nfound = match( current_string, matched_prefix );
    if( matched_prefix != current_string )
        nfound = 0;

    error( FormatString("\"%s\" %s") << current_string <<
        ( nfound == 0 ?
            "is not one of the available choices."
        :
            "is ambiguous.") );

    result = EmacsString::null;
    return result;
}


// return one of the keys, a new value otherwise NULL
EmacsString &EmacsStringTable::get_esc_word_interactive( const EmacsString &prompt, const EmacsString &default_value, EmacsString &result )
{
    Marker old;            // old position of dot

    save_windows saved_windows;    // true if windows should be saved

    Save<int> old_expand( &expand );// Saved value of expand flag
    Save<int> old_help( &help );    // Saved value of help flag

    //
    // Get words from the user until one is found to match
    //
    EmacsString current_string( default_value );    // what we have so far

    for(;;)
    {
        help = expand = 0;
        int nfound = 0;

        // what the user typed before calling br_get_string_interactive
        EmacsString initial_input( current_string );

        current_string = br_get_string_interactive( 1, initial_input, prompt );

        //
        // Deal with expand-and-exit cases and non-interactive cases. These
        // just return the typed value
        //
        if( cur_exec != NULL || (!expand && !help) )
        {
            EmacsString longest_match( current_string );

            //
            // Scan the table to see what matches we have.
            //
            nfound = match( current_string, longest_match );

            // is this the only match
            if( nfound == 1
            // and its logically the same as the user input
            && compareKeys( current_string, longest_match ) == 0 )
                // use the longest as it may have different case
                current_string = longest_match;

            break;
        }

        //
        // Make the table if we have to make tables more than once
        // [Note that the current_string is written back into]
        //
        makeTable( current_string );

        //
        // expand the word as the user requested
        //
        EmacsString longest_match( current_string );
        if( expand )
        {
            //
            // Scan the table to see what matches we have.
            //
            nfound = match( current_string, longest_match );

            // is this the only match
            if( nfound == 1
            // or did we find a longer prefix?
            || longest_match.length() > current_string.length() )
                current_string = longest_match;

        }

        //
        // We have searched all of the items specified.
        // If we only have one item, then lets get and
        // return it. Otherwise we will have to show some help
        // or worse.
        //
        if( nfound == 1 )
            if( find( current_string ) != NULL && terminalEntry( current_string ) )
            {
                if( !confirm_expansion_choice )
                    break;
                else
                    if( compareKeys( current_string, initial_input ) == 0 )
                        break;
                // go around again
            }
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
            error( FormatString("\"%s\" %s") << current_string <<
                (nfound == 0 ?
                    "is not one of the available choices."
                :
                    "is ambiguous.") );
            break;
        }

        bool duplicate_expands = expand && compareKeys( current_string, initial_input ) == 0;
        if( !auto_help && !help && !duplicate_expands )
        {
            //
            // The user recons to be a wizard, and has help-on-command-completion-error
            // switches off, and he did not ask for help. Just bell at him, and
            // remove all the erroneous characters.
            //
            ding ();
        }
        else
        {
            //
            // Pop up a help window, and fill it full of stuff
            // for the user to see.
            //

            if( !old.isSet() )
            {
                old.set_mark( bf_cur, 1, 0 );
                if( remove_help_window != 0 && cur_exec == 0 )
                    saved_windows.save();
            }

            if( !expand )
            {
                //
                // Scan the table to see what matches we have.
                //
                nfound = match( current_string, longest_match );
            }

            fillHelpBuffer( longest_match, nfound );
        }
    }

    //
    // Everything is now complete, one way or another. So, lets tidy up any
    // mess we made of the screen, spit out the text of the word chosen and
    // get on with it
    //
    if( old.isSet() )
        theActiveView->window_on( old.m_buf );

    if( interactive() )
        message( FormatString("%s%s") << prompt << current_string );

    //
    // return the actual text that caused the exit
    //
    result = current_string;
    return result;
}

EmacsString &EmacsStringTable::get_esc_word_mlisp( EmacsString &result )
{
    result = br_get_string_mlisp();
    return result;
}


#if 0
class CharTable : public EmacsStringTable
{
public:
    CharTable( int a, int b )
        : EmacsStringTable( a, b )
    { }
    virtual ~CharTable()
    { }

    void add( const EmacsString &key, char *value )
    {
        EmacsStringTable::add( key, value );
    }
    char *remove( const EmacsString &key )
    {
        return (char *)EmacsStringTable::remove( key );
    }
    char *find( const EmacsString &key )
    {
        return (char *)EmacsStringTable::find( key );
    }
};

void test_emacs_string_tables(void)
{
    CharTable tab( 2, 2 );

    tab.add( "barry", "dad" );
    tab.add( "alex", "son" );
    tab.add( "therese", "mum" );
    tab.add( "john", "friend with an H" );
    tab.add( "jon", "friend without an H" );

    int entries = tab.entries();

    const char *who = tab.find( "jon" );

    int index = 0;

    const EmacsString *what;
    do
    {
        what = tab.apropos( "e", index );
    }
    while( what != NULL );


    EmacsString match;

    int matches = tab.match( "j", match );
    matches = tab.match( "b", match );
    debug_invoke();
}
#endif
