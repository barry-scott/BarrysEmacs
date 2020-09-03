//
//    Copyright (c) 2020
//        Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#if defined( SPELL_CHECKER )
#include <hunspell.hxx>

static Hunspell *checker = NULL;

int spell_check_init(void)
{
    EmacsString lang( getnbstr( ": spell-check-init (language) " ) );
    // look for the affix and dictionary files
    EmacsString dic_filename;
    expand_and_default( lang, "/usr/share/myspell/en_US.dic", dic_filename );

    if( EmacsFile::fio_access( dic_filename ) == 0 )
    {
        error( FormatString("Cannot find required spell checker dictionary %s") << dic_filename );
    }

    EmacsString aff_filename;
    expand_and_default( lang, "/usr/share/myspell/en_US.aff", aff_filename );

    if( EmacsFile::fio_access( aff_filename ) == 0 )
    {
        error( FormatString("Cannot find required spell checker affices %s") << aff_filename );
    }

    if( checker != NULL )
    {
        delete checker;
    }
    checker = new Hunspell( aff_filename, dic_filename );

    return 0;
}

int spell_check_word(void)
{
    if( checker == NULL )
    {
        error( "spell-check-init has not been called" );
        return 0;
    }

    EmacsString word( getnbstr( ": spell-check-word " ) );
    std::string std_word( word );

    bool ok = checker->spell( std_word );

    ml_value = int(ok);
    return 0;
}

int spell_check_suggestions(void)
{
    if( checker == NULL )
    {
        error( "spell-check-init has not been called" );
        return 0;
    }

    EmacsString word( get_string_mlisp() );
    if( ml_err )
        return 0;

    std::string std_word( word );

    std::vector<std::string> suggestions = checker->suggest( std_word );

    EmacsArray result( 0, suggestions.size()+1 );
    result( 0 ) = suggestions.size();

    std::vector<std::string>::iterator it = suggestions.begin();
    int index=1;
    while( it != suggestions.end() )
    {
        result( index++ ) = EmacsString( *it++ );
    }

    ml_value = result;

    return 0;
}

int get_tty_spelling(void)
{
    if( checker == NULL )
    {
        error( "spell-check-init has not been called" );
        return 0;
    }

    EmacsString prompt;
    EmacsString word;

    if( cur_exec == NULL )
    {
        prompt = get_string_interactive( ": get-tty-spelling (prompt) " );
        word = get_string_interactive( ": get-tty-spelling (word) " );
    }
    else
    {
        if( check_args( 2, 2 ) )
            return 0;

        prompt = get_string_mlisp();
        if( ml_err )
            return 0;

        word = get_string_mlisp();
        if( ml_err )
            return 0;
    }

    std::string std_word( word );

    std::vector<std::string> suggestions = checker->suggest( std_word );

    EmacsStringTable table;

    if( suggestions.size() == 0 )
    {
        table.add( word, NULL );
    }
    else
    {
        std::vector<std::string>::iterator it = suggestions.begin();
        while( it != suggestions.end() )
        {
            table.add( EmacsString( *it++ ), NULL );
        }

        word = suggestions.front();
    }

    Save<ProgramNode *> lcur_exec( &cur_exec );
    cur_exec = NULL;

    EmacsString answer;
    answer = table.get_esc_word_interactive( prompt, word, answer );

    ml_value = answer;

    return 0;
}

int spell_check_add_word(void)
{
    if( checker == NULL )
    {
        error( "spell-check-init has not been called" );
        return 0;
    }

    return 0;
}

#endif
