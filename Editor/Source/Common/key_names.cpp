//
//    key_names.cpp
//
//    Copyright (c) 1997 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

key_name_entry::key_name_entry( const char *n, const char *v )
: name( n )
, value( v )
, compressed_value()
{}

key_name_entry::key_name_entry( const key_name_entry &from )
: name( from.name )
, value( from.value )
, compressed_value()
{}

#if defined( PYBEMACS )
key_name PC_key_names;
#else
static key_name_entry PC_key_name_entries[] =
{
    // This list must be kept i sorted order
    key_name_entry("csi",                           "\033[" ),
    key_name_entry("ctrl-backspace",                "\033[85~" ),
    key_name_entry("ctrl-delete",                   "\033[53~" ),
    key_name_entry("ctrl-down",                     "\033[58~" ),
    key_name_entry("ctrl-end",                      "\033[54~" ),
    key_name_entry("ctrl-f1",                       "\033[67~" ),
    key_name_entry("ctrl-f10",                      "\033[78~" ),
    key_name_entry("ctrl-f11",                      "\033[79~" ),
    key_name_entry("ctrl-f12",                      "\033[81~" ),
    key_name_entry("ctrl-f2",                       "\033[68~" ),
    key_name_entry("ctrl-f3",                       "\033[69~" ),
    key_name_entry("ctrl-f4",                       "\033[70~" ),
    key_name_entry("ctrl-f5",                       "\033[71~" ),
    key_name_entry("ctrl-f6",                       "\033[73~" ),
    key_name_entry("ctrl-f7",                       "\033[74~" ),
    key_name_entry("ctrl-f8",                       "\033[75~" ),
    key_name_entry("ctrl-f9",                       "\033[76~" ),
    key_name_entry("ctrl-home",                     "\033[51~" ),
    key_name_entry("ctrl-insert",                   "\033[52~" ),
    key_name_entry("ctrl-left",                     "\033[60~" ),
    key_name_entry("ctrl-mouse-wheel-neg",          "\033[9#w"),
    key_name_entry("ctrl-mouse-wheel-pos",          "\033[8#w"),
    key_name_entry("ctrl-page-down",                "\033[56~" ),
    key_name_entry("ctrl-page-up",                  "\033[55~" ),
    key_name_entry("ctrl-pause",                    "\033[84~" ),
    key_name_entry("ctrl-print-screen",             "\033[82~" ),
    key_name_entry("ctrl-right",                    "\033[59~" ),
    key_name_entry("ctrl-scroll-lock",              "\033[83~" ),
    key_name_entry("ctrl-shift-delete",             "\033[153~" ),
    key_name_entry("ctrl-shift-down",               "\033[158~" ),
    key_name_entry("ctrl-shift-end",                "\033[154~" ),
    key_name_entry("ctrl-shift-f1",                 "\033[167~" ),
    key_name_entry("ctrl-shift-f10",                "\033[178~" ),
    key_name_entry("ctrl-shift-f11",                "\033[179~" ),
    key_name_entry("ctrl-shift-f12",                "\033[181~" ),
    key_name_entry("ctrl-shift-f2",                 "\033[168~" ),
    key_name_entry("ctrl-shift-f3",                 "\033[169~" ),
    key_name_entry("ctrl-shift-f4",                 "\033[170~" ),
    key_name_entry("ctrl-shift-f5",                 "\033[171~" ),
    key_name_entry("ctrl-shift-f6",                 "\033[173~" ),
    key_name_entry("ctrl-shift-f7",                 "\033[174~" ),
    key_name_entry("ctrl-shift-f8",                 "\033[175~" ),
    key_name_entry("ctrl-shift-f9",                 "\033[176~" ),
    key_name_entry("ctrl-shift-home",               "\033[151~" ),
    key_name_entry("ctrl-shift-insert",             "\033[152~" ),
    key_name_entry("ctrl-shift-left",               "\033[160~" ),
    key_name_entry("ctrl-shift-mouse-wheel-neg",    "\033[13#w"),
    key_name_entry("ctrl-shift-mouse-wheel-pos",    "\033[12#w"),
    key_name_entry("ctrl-shift-page-down",          "\033[156~" ),
    key_name_entry("ctrl-shift-page-up",            "\033[155~" ),
    key_name_entry("ctrl-shift-pause",              "\033[184~" ),
    key_name_entry("ctrl-shift-print-screen",       "\033[182~" ),
    key_name_entry("ctrl-shift-right",              "\033[159~" ),
    key_name_entry("ctrl-shift-scroll-lock",        "\033[183~" ),
    key_name_entry("ctrl-shift-up",                 "\033[157~" ),
    key_name_entry("ctrl-up",                       "\033[57~" ),
    key_name_entry("delete",                        "\033[3~" ),
    key_name_entry("down",                          "\033[B" ),
    key_name_entry("end",                           "\033[4~" ),
    key_name_entry("f1",                            "\033[17~" ),
    key_name_entry("f10",                           "\033[28~" ),
    key_name_entry("f11",                           "\033[29~" ),
    key_name_entry("f12",                           "\033[31~" ),
    key_name_entry("f2",                            "\033[18~" ),
    key_name_entry("f3",                            "\033[19~" ),
    key_name_entry("f4",                            "\033[20~" ),
    key_name_entry("f5",                            "\033[21~" ),
    key_name_entry("f6",                            "\033[23~" ),
    key_name_entry("f7",                            "\033[24~" ),
    key_name_entry("f8",                            "\033[25~" ),
    key_name_entry("f9",                            "\033[26~" ),
    key_name_entry("home",                          "\033[1~" ),
    key_name_entry("insert",                        "\033[2~" ),
    key_name_entry("kp-divide",                     "\033OQ" ),
    key_name_entry("kp-dot",                        "\033On" ),
    key_name_entry("kp-enter",                      "\033OM" ),
    key_name_entry("kp-minus",                      "\033OS" ),
    key_name_entry("kp-multiple",                   "\033OR" ),
    key_name_entry("kp-plus",                       "\033Ol" ),
    key_name_entry("kp0",                           "\033Op" ),
    key_name_entry("kp1",                           "\033Oq" ),
    key_name_entry("kp2",                           "\033Or" ),
    key_name_entry("kp3",                           "\033Os" ),
    key_name_entry("kp4",                           "\033Ot" ),
    key_name_entry("kp5",                           "\033Ou" ),
    key_name_entry("kp6",                           "\033Ov" ),
    key_name_entry("kp7",                           "\033Ow" ),
    key_name_entry("kp8",                           "\033Ox" ),
    key_name_entry("kp9",                           "\033Oy" ),
    key_name_entry("left",                          "\033[D" ),
    key_name_entry("menu",                          "\202" ),
    key_name_entry("mouse",                         "\033[&w" ),
    key_name_entry("mouse-1-down",                  "\033[2&w" ),
    key_name_entry("mouse-1-up",                    "\033[3&w" ),
    key_name_entry("mouse-2-down",                  "\033[4&w" ),
    key_name_entry("mouse-2-up",                    "\033[5&w" ),
    key_name_entry("mouse-3-down",                  "\033[6&w" ),
    key_name_entry("mouse-3-up",                    "\033[7&w" ),
    key_name_entry("mouse-4-down",                  "\033[8&w" ),
    key_name_entry("mouse-4-up",                    "\033[9&w" ),
    key_name_entry("mouse-wheel",                   "\033[#w"),
    key_name_entry("mouse-wheel-neg",               "\033[1#w"),
    key_name_entry("mouse-wheel-pos",               "\033[0#w"),
    key_name_entry("mouse-motion",                  "\033[901~"),   // QQQ what is the real sequence?
    key_name_entry("num-lock",                      "\033OP" ),
    key_name_entry("page-down",                     "\033[6~" ),
    key_name_entry("page-up",                       "\033[5~" ),
    key_name_entry("pause",                         "\033[34~" ),
    key_name_entry("print-screen",                  "\033[32~" ),
    key_name_entry("right",                         "\033[C" ),
    key_name_entry("scroll-lock",                   "\033[33~" ),
    key_name_entry("shift-delete",                  "\033[103~" ),
    key_name_entry("shift-down",                    "\033[108~" ),
    key_name_entry("shift-end",                     "\033[104~" ),
    key_name_entry("shift-f1",                      "\033[117~" ),
    key_name_entry("shift-f10",                     "\033[128~" ),
    key_name_entry("shift-f11",                     "\033[129~" ),
    key_name_entry("shift-f12",                     "\033[131~" ),
    key_name_entry("shift-f2",                      "\033[118~" ),
    key_name_entry("shift-f3",                      "\033[119~" ),
    key_name_entry("shift-f4",                      "\033[120~" ),
    key_name_entry("shift-f5",                      "\033[121~" ),
    key_name_entry("shift-f6",                      "\033[123~" ),
    key_name_entry("shift-f7",                      "\033[124~" ),
    key_name_entry("shift-f8",                      "\033[125~" ),
    key_name_entry("shift-f9",                      "\033[126~" ),
    key_name_entry("shift-home",                    "\033[101~" ),
    key_name_entry("shift-insert",                  "\033[102~" ),
    key_name_entry("shift-kp-plus",                 "\033Om" ),
    key_name_entry("shift-left",                    "\033[110~" ),
    key_name_entry("shift-mouse-wheel-neg",         "\033[5#w"),
    key_name_entry("shift-mouse-wheel-pos",         "\033[4#w"),
    key_name_entry("shift-page-down",               "\033[106~" ),
    key_name_entry("shift-page-up",                 "\033[105~" ),
    key_name_entry("shift-pause",                   "\033[134~" ),
    key_name_entry("shift-print-screen",            "\033[132~" ),
    key_name_entry("shift-right",                   "\033[109~" ),
    key_name_entry("shift-scroll-lock",             "\033[133~" ),
    key_name_entry("shift-tab",                     "\033[900~" ),  // QQQ does this exist on a konsole?
    key_name_entry("shift-up",                      "\033[107~" ),
    key_name_entry("ss3",                           "\033O" ),
    key_name_entry("up",                            "\033[A" )
};
key_name PC_key_names( PC_key_name_entries, sizeof( PC_key_name_entries ) );
#endif

key_name::key_name( key_name_entry *_key_names, int sizeof_key_names )
: name_to_keys()
, keys_to_name()
, name_to_compressed_keys()
, compressed_keys_to_name()
{
    for( size_t i=0; i<sizeof_key_names/sizeof( key_name_entry ); ++i )
    {
        addMapping( _key_names[i].name, _key_names[i].value );
    }
}

key_name::key_name()
: name_to_keys()
, keys_to_name()
, name_to_compressed_keys()
, compressed_keys_to_name()
{
}

key_name::~key_name()
{
}

void key_name::addMapping( const EmacsString &name, const EmacsString &keys )
{
    EmacsString compressed_keys;

    name_to_keys[ name ] = keys;
    keys_to_name[ keys ] = name;
}

void key_name::buildCompressedMapping()
{
    // build the compressed map on demand
    if( name_to_compressed_keys.empty() )
    {
        for( KeysMapping_t::const_iterator i = name_to_keys.begin();
                i != name_to_keys.end();
                    ++i )
        {
            EmacsString compressed_keys;
            convert_key_string( i->second, compressed_keys );
            name_to_compressed_keys[ i->first ] = compressed_keys;
            compressed_keys_to_name[ compressed_keys ] = i->first;
        }
    }
}

EmacsString key_name::valueOfKeyName( const EmacsString &key )
{
    buildCompressedMapping();

    KeysMapping_t::const_iterator i = name_to_compressed_keys.find( key );
    if( i != name_to_compressed_keys.end() )
    {
        return i->second;
    }

    return EmacsString::null;
}

//
//    Look for the longest key value that matches at the start of the
//    chars vector.
//
int key_name::keyNameOfValue( const EmacsString &chars, EmacsString &name )
{
    buildCompressedMapping();

    int longest_match_length = 0;
    KeysMapping_t::const_iterator best_match = name_to_compressed_keys.end();
    KeysMapping_t::const_iterator i = name_to_compressed_keys.begin();

    while( i != name_to_compressed_keys.end() )
    {
        EmacsString compressed_keys( i->second );
        int match_length = compressed_keys.commonPrefix( chars );

        if( compressed_keys.length() == match_length
        && match_length > longest_match_length )
        {
            best_match = i;
            longest_match_length = match_length;
        }
        ++i;
    }

    if( best_match == name_to_compressed_keys.end() )
    {
        return 0;
    }

    name = best_match->first;
    return best_match->second.length();
}
