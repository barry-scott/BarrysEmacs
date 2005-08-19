//
//	key_names.cpp
//
//	Copyright (c) 1997 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

//
//	LK201 emulation on the PC Enhanced keyboard
//
//	LK201		PC		Codes
//	-----		--		-----
//	Find		Home		CSI 1 ~
//	Insert Here	Insert		CSI 2 ~
//	Remove		Delete		CSI 3 ~
//	Select		End		CSI 4 ~
//	Prev Screen	Page Up		CSI 5 ~
//	Next Screen	Page Down	CSI 6 ~
//
//	F6		F1		CSI 1 7 ~
//	F7		F2		CSI 1 8 ~
//	F8		F3		CSI 1 9 ~
//	F9		F4		CSI 2 0 ~
//	F10		F5		CSI 2 1 ~
//	F11 (ESC)	F6		CSI 2 3 ~
//	F12 (BS)	F7		CSI 2 4 ~
//	F13 (LF)	F8		CSI 2 5 ~
//	F14		F9		CSI 2 6 ~
//	Help		F10		CSI 2 8 ~
//	Do		F11		CSI 2 9 ~
//	F17		F12		CSI 3 1 ~
//	F18		Print Screen	CSI 3 2 ~
//	F19		Scroll Lock	CSI 3 3 ~
//	F20		Pause		CSI 3 4 ~
//

key_name_entry::key_name_entry( const char *n, const char *v )
		: name( n )
		, value( v )
		, compressed_value()
		{}

key_name_entry::key_name_entry( const key_name_entry &from )
		: name( from.name )
		, value( from.name )
		, compressed_value()
		{}

static key_name_entry LK201_key_name_entries[] =
	{
	key_name_entry("comma",				"\033Ol" ),
	key_name_entry("csi",				"\033[" ),
	key_name_entry("ctrl-del",			"\033[53~" ),
	key_name_entry("ctrl-do",			"\033[79~" ),
	key_name_entry("ctrl-down",			"\033[58~" ),
	key_name_entry("ctrl-end",			"\033[54~" ),
	key_name_entry("ctrl-f10",			"\033[71~" ),
	key_name_entry("ctrl-f11",			"\033[73~" ),
	key_name_entry("ctrl-f12",			"\033[74~" ),
	key_name_entry("ctrl-f13",			"\033[75~" ),
	key_name_entry("ctrl-f14",			"\033[76~" ),
	key_name_entry("ctrl-f17",			"\033[81~" ),
	key_name_entry("ctrl-f18",			"\033[82~" ),
	key_name_entry("ctrl-f19",			"\033[83~" ),
	key_name_entry("ctrl-f20",			"\033[84~" ),
	key_name_entry("ctrl-f6",			"\033[67~" ),
	key_name_entry("ctrl-f7",			"\033[68~" ),
	key_name_entry("ctrl-f8",			"\033[69~" ),
	key_name_entry("ctrl-f9",			"\033[70~" ),
	key_name_entry("ctrl-home",			"\033[51~" ),
	key_name_entry("ctrl-ins",			"\033[52~" ),
	key_name_entry("ctrl-left",			"\033[60~" ),
	key_name_entry("ctrl-pgdn",			"\033[56~" ),
	key_name_entry("ctrl-pgup",			"\033[55~" ),
	key_name_entry("ctrl-right",			"\033[59~" ),
	key_name_entry("ctrl-up",			"\033[57~" ),
	key_name_entry("do",				"\033[29~" ),
	key_name_entry("dot",				"\033On" ),
	key_name_entry("down",				"\033[B" ),
	key_name_entry("enter",				"\033OM" ),
	key_name_entry("f10",				"\033[21~" ),
	key_name_entry("f11",				"\033[23~" ),
	key_name_entry("f12",				"\033[24~" ),
	key_name_entry("f13",				"\033[25~" ),
	key_name_entry("f14",				"\033[26~" ),
	key_name_entry("f17",				"\033[31~" ),
	key_name_entry("f18",				"\033[32~" ),
	key_name_entry("f19",				"\033[33~" ),
	key_name_entry("f20",				"\033[34~" ),
	key_name_entry("f6",				"\033[17~" ),
	key_name_entry("f7",				"\033[18~" ),
	key_name_entry("f8",				"\033[19~" ),
	key_name_entry("f9",				"\033[20~" ),
	key_name_entry("find",				"\033[1~" ),
	key_name_entry("gold",				"\033OP" ),
	key_name_entry("help",				"\033[28~" ),
	key_name_entry("insert",			"\033[2~" ),
	key_name_entry("insert here",			"\033[2~" ),
	key_name_entry("kp0",				"\033Op" ),
	key_name_entry("kp1",				"\033Oq" ),
	key_name_entry("kp2",				"\033Or" ),
	key_name_entry("kp3",				"\033Os" ),
	key_name_entry("kp4",				"\033Ot" ),
	key_name_entry("kp5",				"\033Ou" ),
	key_name_entry("kp6",				"\033Ov" ),
	key_name_entry("kp7",				"\033Ow" ),
	key_name_entry("kp8",				"\033Ox" ),
	key_name_entry("kp9",				"\033Oy" ),
	key_name_entry("left",				"\033[D" ),
	key_name_entry("menu",				"\202" ),
	key_name_entry("minus",				"\033Om" ),
	key_name_entry("mouse",				"\033[&w" ),
	key_name_entry("mouse-1-down",			"\033[2&w" ),
	key_name_entry("mouse-1-up",			"\033[3&w" ),
	key_name_entry("mouse-2-down",			"\033[4&w" ),
	key_name_entry("mouse-2-up",			"\033[5&w" ),
	key_name_entry("mouse-3-down",			"\033[6&w" ),
	key_name_entry("mouse-3-up",			"\033[7&w" ),
	key_name_entry("mouse-4-down",			"\033[8&w" ),
	key_name_entry("mouse-4-up",			"\033[9&w" ),
	key_name_entry("mouse-wheel",			"\033[#w"),
	key_name_entry("mouse-wheel-neg",		"\033[1#w"),
	key_name_entry("mouse-wheel-pos",		"\033[0#w"),
	key_name_entry("next",				"\033[6~" ),
	key_name_entry("next screen",			"\033[6~" ),
	key_name_entry("pf1",				"\033OP" ),
	key_name_entry("pf2",				"\033OQ" ),
	key_name_entry("pf3",				"\033OR" ),
	key_name_entry("pf4",				"\033OS" ),
	key_name_entry("prev",				"\033[5~" ),
	key_name_entry("prev screen",			"\033[5~" ),
	key_name_entry("remove",			"\033[3~" ),
	key_name_entry("right",				"\033[C" ),
	key_name_entry("select",			"\033[4~" ),
	key_name_entry("ss3",				"\033O" ),
	key_name_entry("up",				"\033[A" )
	};

key_name LK201_key_names( LK201_key_name_entries, sizeof( LK201_key_name_entries ) );

static key_name_entry PC_key_name_entries[] =
	{
	key_name_entry("csi",				"\033[" ),
	key_name_entry("ctrl-backspace",		"\033[85~" ),
	key_name_entry("ctrl-delete",			"\033[53~" ),
	key_name_entry("ctrl-down",			"\033[58~" ),
	key_name_entry("ctrl-end",			"\033[54~" ),
	key_name_entry("ctrl-f1",			"\033[67~" ),
	key_name_entry("ctrl-f10",			"\033[78~" ),
	key_name_entry("ctrl-f11",			"\033[79~" ),
	key_name_entry("ctrl-f12",			"\033[81~" ),
	key_name_entry("ctrl-f2",			"\033[68~" ),
	key_name_entry("ctrl-f3",			"\033[69~" ),
	key_name_entry("ctrl-f4",			"\033[70~" ),
	key_name_entry("ctrl-f5",			"\033[71~" ),
	key_name_entry("ctrl-f6",			"\033[73~" ),
	key_name_entry("ctrl-f7",			"\033[74~" ),
	key_name_entry("ctrl-f8",			"\033[75~" ),
	key_name_entry("ctrl-f9",			"\033[76~" ),
	key_name_entry("ctrl-home",			"\033[51~" ),
	key_name_entry("ctrl-insert",			"\033[52~" ),
	key_name_entry("ctrl-left",			"\033[60~" ),
	key_name_entry("ctrl-mouse-wheel-neg",		"\033[9#w"),
	key_name_entry("ctrl-mouse-wheel-pos",		"\033[8#w"),
	key_name_entry("ctrl-page-down",		"\033[56~" ),
	key_name_entry("ctrl-page-up",			"\033[55~" ),
	key_name_entry("ctrl-pause",			"\033[84~" ),
	key_name_entry("ctrl-print-screen",		"\033[82~" ),
	key_name_entry("ctrl-right",			"\033[59~" ),
	key_name_entry("ctrl-scroll-lock",		"\033[83~" ),
	key_name_entry("ctrl-shift-delete",		"\033[153~" ),
	key_name_entry("ctrl-shift-down",		"\033[158~" ),
	key_name_entry("ctrl-shift-end",		"\033[154~" ),
	key_name_entry("ctrl-shift-f1",			"\033[167~" ),
	key_name_entry("ctrl-shift-f10",		"\033[178~" ),
	key_name_entry("ctrl-shift-f11",		"\033[179~" ),
	key_name_entry("ctrl-shift-f12",		"\033[181~" ),
	key_name_entry("ctrl-shift-f2",			"\033[168~" ),
	key_name_entry("ctrl-shift-f3",			"\033[169~" ),
	key_name_entry("ctrl-shift-f4",			"\033[170~" ),
	key_name_entry("ctrl-shift-f5",			"\033[171~" ),
	key_name_entry("ctrl-shift-f6",			"\033[173~" ),
	key_name_entry("ctrl-shift-f7",			"\033[174~" ),
	key_name_entry("ctrl-shift-f8",			"\033[175~" ),
	key_name_entry("ctrl-shift-f9",			"\033[176~" ),
	key_name_entry("ctrl-shift-home",		"\033[151~" ),
	key_name_entry("ctrl-shift-insert",		"\033[152~" ),
	key_name_entry("ctrl-shift-left",		"\033[160~" ),
	key_name_entry("ctrl-shift-mouse-wheel-neg",	"\033[13#w"),
	key_name_entry("ctrl-shift-mouse-wheel-pos",	"\033[12#w"),
	key_name_entry("ctrl-shift-page-down",		"\033[156~" ),
	key_name_entry("ctrl-shift-page-up",		"\033[155~" ),
	key_name_entry("ctrl-shift-pause",		"\033[184~" ),
	key_name_entry("ctrl-shift-print-screen",	"\033[182~" ),
	key_name_entry("ctrl-shift-right",		"\033[159~" ),
	key_name_entry("ctrl-shift-scroll-lock",	"\033[183~" ),
	key_name_entry("ctrl-shift-up",			"\033[157~" ),
	key_name_entry("ctrl-up",			"\033[57~" ),
	key_name_entry("delete",			"\033[3~" ),
	key_name_entry("down",				"\033[B" ),
	key_name_entry("end",				"\033[4~" ),
	key_name_entry("f1",				"\033[17~" ),
	key_name_entry("f10",				"\033[28~" ),
	key_name_entry("f11",				"\033[29~" ),
	key_name_entry("f12",				"\033[31~" ),
	key_name_entry("f2",				"\033[18~" ),
	key_name_entry("f3",				"\033[19~" ),
	key_name_entry("f4",				"\033[20~" ),
	key_name_entry("f5",				"\033[21~" ),
	key_name_entry("f6",				"\033[23~" ),
	key_name_entry("f7",				"\033[24~" ),
	key_name_entry("f8",				"\033[25~" ),
	key_name_entry("f9",				"\033[26~" ),
	key_name_entry("home",				"\033[1~" ),
	key_name_entry("insert",			"\033[2~" ),
	key_name_entry("kp-divide",			"\033OQ" ),
	key_name_entry("kp-dot",			"\033On" ),
	key_name_entry("kp-enter",			"\033OM" ),
	key_name_entry("kp-minus",			"\033OS" ),
	key_name_entry("kp-multiple",			"\033OR" ),
	key_name_entry("kp-plus",			"\033Ol" ),
	key_name_entry("kp0",				"\033Op" ),
	key_name_entry("kp1",				"\033Oq" ),
	key_name_entry("kp2",				"\033Or" ),
	key_name_entry("kp3",				"\033Os" ),
	key_name_entry("kp4",				"\033Ot" ),
	key_name_entry("kp5",				"\033Ou" ),
	key_name_entry("kp6",				"\033Ov" ),
	key_name_entry("kp7",				"\033Ow" ),
	key_name_entry("kp8",				"\033Ox" ),
	key_name_entry("kp9",				"\033Oy" ),
	key_name_entry("left",				"\033[D" ),
	key_name_entry("menu",				"\202" ),
	key_name_entry("mouse",				"\033[&w" ),
	key_name_entry("mouse-1-down",			"\033[2&w" ),
	key_name_entry("mouse-1-up",			"\033[3&w" ),
	key_name_entry("mouse-2-down",			"\033[4&w" ),
	key_name_entry("mouse-2-up",			"\033[5&w" ),
	key_name_entry("mouse-3-down",			"\033[6&w" ),
	key_name_entry("mouse-3-up",			"\033[7&w" ),
	key_name_entry("mouse-4-down",			"\033[8&w" ),
	key_name_entry("mouse-4-up",			"\033[9&w" ),
	key_name_entry("mouse-wheel",			"\033[#w"),
	key_name_entry("mouse-wheel-neg",		"\033[1#w"),
	key_name_entry("mouse-wheel-pos",		"\033[0#w"),
	key_name_entry("num-lock",			"\033OP" ),
	key_name_entry("page-down",			"\033[6~" ),
	key_name_entry("page-up",			"\033[5~" ),
	key_name_entry("pause",				"\033[34~" ),
	key_name_entry("print-screen",			"\033[32~" ),
	key_name_entry("right",				"\033[C" ),
	key_name_entry("scroll-lock",			"\033[33~" ),
	key_name_entry("shift-delete",			"\033[103~" ),
	key_name_entry("shift-down",			"\033[108~" ),
	key_name_entry("shift-end",			"\033[104~" ),
	key_name_entry("shift-f1",			"\033[117~" ),
	key_name_entry("shift-f10",			"\033[128~" ),
	key_name_entry("shift-f11",			"\033[129~" ),
	key_name_entry("shift-f12",			"\033[131~" ),
	key_name_entry("shift-f2",			"\033[118~" ),
	key_name_entry("shift-f3",			"\033[119~" ),
	key_name_entry("shift-f4",			"\033[120~" ),
	key_name_entry("shift-f5",			"\033[121~" ),
	key_name_entry("shift-f6",			"\033[123~" ),
	key_name_entry("shift-f7",			"\033[124~" ),
	key_name_entry("shift-f8",			"\033[125~" ),
	key_name_entry("shift-f9",			"\033[126~" ),
	key_name_entry("shift-home",			"\033[101~" ),
	key_name_entry("shift-insert",			"\033[102~" ),
	key_name_entry("shift-kp-plus",			"\033Om" ),
	key_name_entry("shift-left",			"\033[110~" ),
	key_name_entry("shift-mouse-wheel-neg",		"\033[5#w"),
	key_name_entry("shift-mouse-wheel-pos",		"\033[4#w"),
	key_name_entry("shift-page-down",		"\033[106~" ),
	key_name_entry("shift-page-up",			"\033[105~" ),
	key_name_entry("shift-pause",			"\033[134~" ),
	key_name_entry("shift-print-screen",		"\033[132~" ),
	key_name_entry("shift-right",			"\033[109~" ),
	key_name_entry("shift-scroll-lock",		"\033[133~" ),
	key_name_entry("shift-up",			"\033[107~" ),
	key_name_entry("ss3",				"\033O" ),
	key_name_entry("up",				"\033[A" )
	};
key_name PC_key_names( PC_key_name_entries, sizeof( PC_key_name_entries ) );

EmacsString key_name::valueOfKeyName( const EmacsString &key )
	{
	int lo = 0;
	int hi = num_key_names-1;

	for(;;)
		{
		int mid = (lo + hi) / 2;
		key_name_entry *entry = &key_names[ mid ];
		int match = key.compare( entry->name );

		if( match == 0 )
			return entry->value;
		if( lo >= hi )
			break;
		if( match < 0 )
			hi = mid - 1;
		else
			lo = mid + 1;
		}

	return EmacsString::null;
	}

void key_name::build_key_values( void )
	{
	for( int i=0; i<num_key_names; i++ )
		{
		key_name_entry *entry = &key_names[ i ];

		convert_key_string
			(
			entry->value,
			entry->compressed_value
			);
		}

	key_compressed_built = cs_modified;
	}

//
//	Look for the longest key value that matches at the start of the
//	chars vector. 
//
int key_name::keyNameOfValue( const EmacsString &chars, EmacsString &value)
	{
	static key_name_entry worst_entry( "", "" );
	key_name_entry *best_entry = &worst_entry;
	
	if( cs_modified != key_compressed_built )
		build_key_values();

	for( int i=0; i<num_key_names; i++ )
		{
		key_name_entry *entry = &key_names[ i ];

		if( entry->compressed_value.length() > best_entry->compressed_value.length()
		&& entry->compressed_value.commonPrefix( chars ) == entry->compressed_value.length() )
			best_entry = entry;
		}

	value = best_entry->name;

	return best_entry->compressed_value.length();
	}
