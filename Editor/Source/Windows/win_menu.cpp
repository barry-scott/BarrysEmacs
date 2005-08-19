//
// 	win_menu.cpp
//		windows menu commands
//                 
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );
    
#include <win_emacs.h>
#include <win_registry.h>
#include <win_doc.h>
#include <win_view.h>
#include <win_menu.h>

EmacsMenuNameTable *menu_name_table = NULL;

class menu_args
	{
public:
	menu_args();
public:      
	int num_menus;
	enum { MENU_DEPTH = 9 };
	EmacsString menu_names[MENU_DEPTH];
	int menu_pos[MENU_DEPTH];
	int menu_id;
public:
	void add_menu( int depth, CMenu *menu );
	void remove_menu( int depth, CMenu *menu );

	void collect_add_args( int first_arg )
		{ collect_args( first_arg, true ); }
	void collect_remove_args( int first_arg )
		{ collect_args( first_arg, false ); }
private:
	void collect_args( int first_string, bool add );   
	};

class EmacsMenu
	{
protected:
	EmacsMenu()
		{}
	virtual ~EmacsMenu()
		{}
public:
	virtual void add_menu( menu_args &args )
		{
		args.add_menu( 1, menu() );
		}
	virtual void remove_menu( menu_args &args )
		{
		args.remove_menu( 1, menu() );
		}
	virtual CMenu *menu() = 0;
	};

class EmacsMainMenu : public EmacsMenu
	{
public:
	EmacsMainMenu()
		: EmacsMenu()
		{}
	virtual ~EmacsMainMenu()
		{}

	void add_menu( menu_args &args )
		{
		EmacsMenu::add_menu( args );
		AfxGetApp()->m_pMainWnd->DrawMenuBar();
		}

	void remove_menu( menu_args &args )
		{
		EmacsMenu::remove_menu( args );
		AfxGetApp()->m_pMainWnd->DrawMenuBar();
		}
private:
	CMenu *menu() 
		{
		return AfxGetApp()->m_pMainWnd->GetMenu();
		}

	};

class EmacsPopupMenu : public EmacsMenu
	{
public:
	EmacsPopupMenu()
		: EmacsMenu()
		, menu_( new CMenu() )
		{
		if( !menu_->CreatePopupMenu() )
			{
			error("CreatePopupMenu() failed");
			}
		}
	virtual ~EmacsPopupMenu()
		{}

private:
	virtual CMenu *menu() { return menu_; }
	CMenu *menu_;
	};

class EmacsModeSpecificMenu : public EmacsMenu
	{
public:
	EmacsModeSpecificMenu()
		: EmacsMenu()
		, menu_( new CMenu() )
		{
		if( !menu_->CreateMenu() )
			{
			error("CreateMenu() failed");
			}
		}
	virtual ~EmacsModeSpecificMenu()
		{}

private:
	virtual CMenu *menu() { return menu_; }
	CMenu *menu_;
	};




struct menu_id_map
	{
	unsigned int id;
	EmacsString string;
	};

class init_menu_id_map
	{
public:
	init_menu_id_map();
	~init_menu_id_map() { };

	struct menu_id_map map[100];
	};

init_menu_id_map emacs_menu_id_map;

init_menu_id_map::init_menu_id_map()
	{
	int i=0;
#define MapEntry( _id, _str ) map[i].id = _id; map[i].string = _str; i++;
	MapEntry( ID_BUFFER_CASEBLINDSEARCH,			"eS" );
	MapEntry( ID_EDIT_REPLACECASE,				"eR" );
	MapEntry( ID_BUFFER_DELETE_BUFFER,			"bd" );
	MapEntry( ID_BUFFER_ERASE_BUFFER,			"be" );
	MapEntry( ID_BUFFER_FILTERREGIONTHROUGHCOMMAND, 	"rf" );
	MapEntry( ID_BUFFER_LISTBUFFERS,			"bl" );
	MapEntry( ID_BUFFER_NARROWREGION, 			"rn" );
	MapEntry( ID_BUFFER_SHOWWHITESPACE,			"vw" );
	MapEntry( ID_BUFFER_SORTREGION, 			"rs" );
	MapEntry( ID_BUFFER_WIDENREGION, 			"rw" );
	MapEntry( ID_BUFFER_WRAPLONGLINES, 			"vl" );
	MapEntry( ID_BUILD_COMPILE, 				"bc" );
	MapEntry( ID_BUILD_NEXTERROR, 				"bn" );
	MapEntry( ID_BUILD_PREVIOUSERROR, 			"bp" );
	MapEntry( ID_EDIT_ADVANCED_CAPITALISE, 			"cc" );
	MapEntry( ID_EDIT_ADVANCED_DELETEWHITESPACE, 		"cw" );
	MapEntry( ID_EDIT_ADVANCED_INDENTREGION, 		"ri" );
	MapEntry( ID_EDIT_ADVANCED_INVERTCASE, 			"ci" );
	MapEntry( ID_EDIT_ADVANCED_LOWERCASE, 			"cl" );
	MapEntry( ID_EDIT_ADVANCED_TABIFYREGION, 		"rt" );
	MapEntry( ID_EDIT_ADVANCED_UNDENTREGION, 		"rI" );
	MapEntry( ID_EDIT_ADVANCED_UNTABIFYREGION, 		"rT" );
	MapEntry( ID_EDIT_ADVANCED_UPPERCASE, 			"cu" );
	MapEntry( ID_EDIT_GOTO,					"eg" );
	MapEntry( ID_EDIT_MARK,					"em" );
	MapEntry( ID_EDIT_COPY, 				"ec" );
	MapEntry( ID_EDIT_CUT, 					"ex" );
	MapEntry( ID_EDIT_DELETE, 				"ed" );
	MapEntry( ID_EDIT_PASTE, 				"ev" );
	MapEntry( ID_EDIT_UNDO, 				"eu" );
	MapEntry( ID_EDIT_SELECT_ALL, 				"ea" );
	MapEntry( ID_FILE_INSERTFILE, 				"fi" );
	MapEntry( ID_FILE_OPEN, 				"fo" );
	MapEntry( ID_FILE_OPENSELECTEDFILE,			"fO" );
	MapEntry( ID_FILE_REVERTTOSAVED, 			"fR" );
	MapEntry( ID_FILE_SAVE, 				"fs" );
	MapEntry( ID_FILE_SAVEALL,				"fl" );
	MapEntry( ID_FILE_SAVE_AS, 				"fa" );
	MapEntry( ID_FILE_SAVE_ENVIRONMENT, 			"fe" );
	MapEntry( ID_MACROS_RECORD, 				"mr" );
	MapEntry( ID_MACROS_RUN, 				"mp" );
	MapEntry( ID_MACROS_STOPRECORDING, 			"ms" );
	MapEntry( ID_SEARCH_FIND, 				"ef" );
	MapEntry( ID_SEARCH_REPLACE, 				"er" );
	MapEntry( ID_STOP_EXECUTION,				"se" );
	MapEntry( ID_TOOLS_EXECUTECOMMAND,			"te" );
	MapEntry( ID_TOOLS_GREP_IN_FILES,			"tg" );
	MapEntry( ID_TOOLS_GREP_IN_BUFFERS,			"tb" );
	MapEntry( ID_TOOLS_GREP_CURRENT_BUFFER,			"tc" );
	MapEntry( ID_TOOLS_SHELL, 				"ts" );
	MapEntry( ID_WINDOW_DELETEOTHERWINDOWS,			"wo" );
	MapEntry( ID_WINDOW_DELETETHISWINDOW,			"wt" );
	MapEntry( ID_WINDOW_SPLITHORIZONTAL, 			"wh" );
	MapEntry( ID_WINDOW_SPLITVERTICAL, 			"wv" );
	MapEntry( ID_WINDOW_SWITCHBUFFER,			"bs" );
//	"fr" used in OpenDocumentFile
	map[i].id = 0;
	emacs_assert( i < (sizeof(map)/sizeof(struct menu_id_map)) );
#undef MapEntry
	}

static struct menu_id_map user_menu_id_map[ID_USERMENU_LAST-ID_USERMENU_FIRST+1];
static unsigned int next_menu_id = ID_USERMENU_FIRST;

const EmacsString &menu_id_to_id_string( unsigned int id )
	{
	struct menu_id_map *p;

	/* if its a user menu id its an easy lookup */
	if( id >= ID_USERMENU_FIRST && id <= ID_USERMENU_LAST )
		return user_menu_id_map[id-ID_USERMENU_FIRST].string;
	
	/* hunt though the emacs menu ids */
	p = emacs_menu_id_map.map;
	while( !p->string.isNull() )
		if( p->id == id)
			return p->string;
		else
			p++;

	return EmacsString::null;
	}

unsigned int menu_id_string_to_id( const EmacsString &id_string )
	{
	struct menu_id_map *p;
	int i;

	/* see if this is a emacs defined menu item */
	p = emacs_menu_id_map.map;
	while( !p->string.isNull() )
		if( id_string == p->string )
			return p->id;
		else
			p++;

	/* see if its one of the users defined menus */
	p = user_menu_id_map;
	for( i=0; i<(ID_USERMENU_LAST-ID_USERMENU_FIRST+1); i++, p++ )
	     	if( id_string == p->string )
			return p->id;

	/* see if we have run out of space */
	if( next_menu_id > ID_USERMENU_LAST )
		return 0;

	/* add this id string to the table */
	user_menu_id_map[next_menu_id-ID_USERMENU_FIRST].string = id_string;
	user_menu_id_map[next_menu_id-ID_USERMENU_FIRST].id = next_menu_id;

	return next_menu_id++;
	}

menu_args::menu_args()
	: num_menus( 0 )
	, menu_id( 0 )
	{ }

//
//	collect the args for with the add menu or remove menu functions
//
//	for add menu the args are
//		menu-id [position name]*
//	where
//		menu-id is the two char ID string to bind the menu to
//		position is the place to insert this menu item
//		name is the name of this menu item
//		position and menu args are repeated in pairs to
//		 implement hierical memus.
//
//	for remove menu the args are
//		name [name]*
//
void menu_args::collect_args( int first_string, bool add )
	{
	int arg = first_string;
	int last_arg = cur_exec->p_nargs;

	if( add )
		check_args( first_string+2, first_string+(MENU_DEPTH*2) );
	else
		check_args( first_string, first_string+MENU_DEPTH );

	if( ml_err )
		return;
		
	if( add )
		{
		menu_id = -1;

		if( string_arg( arg++ ) )
			menu_id = menu_id_string_to_id( ml_value.asString() );

		if( menu_id == 0 )
			error( "There are no free menu IDs left" );
		}

	while( !ml_err && arg <= last_arg )
		{
		if( add )
			menu_pos[num_menus] = numeric_arg( arg++ );

		if( string_arg( arg++ ) )
			menu_names[num_menus] = ml_value.asString();

		num_menus++;
		}
	}

void menu_args::add_menu( int depth, CMenu *menu )
	{
	int num_items = menu->GetMenuItemCount();
	int help_item = num_items-1;
	int found = -1;

	// find the position of the menu for this level
	for( int pos=0; pos<num_items; pos++ )
		{
		char name_buf[32];
		int len = menu->GetMenuString( pos, name_buf, sizeof( name_buf )-1, MF_BYPOSITION );

		name_buf[len] = '\0';
		EmacsString name( name_buf );
		if( name == menu_names[depth-1] )
			{ found = pos; break; }
		if( name ==  "&Help" )
			help_item = pos;
		}

	if( found < 0 )
		{
		if( depth < num_menus )
			{
			CMenu *new_popup = new CMenu();
			if( new_popup == NULL )
				return;

			new_popup->CreateMenu();

			// new sub menu
			if( ! menu->InsertMenu
				(
				min( menu_pos[depth-1], (depth == 1 ? help_item : num_items) ), 
				MF_BYPOSITION|MF_ENABLED|MF_POPUP,
				(UINT)new_popup->m_hMenu,
				menu_names[depth-1]
				) )
					return;

			add_menu( depth+1, new_popup );
			}
		else
			{
			// new menu item
			menu->InsertMenu
				(
				min( menu_pos[depth-1], num_items ), 
				MF_BYPOSITION |
				 MF_ENABLED |
				 ((menu_names[depth-1] == "-") ? MF_SEPARATOR : MF_STRING),
				menu_id,
				menu_names[depth-1]
				);
			}
		return;
		}

	// found
	if( depth < num_menus )
		{
		// follow the sub menu down
		CMenu *old_popup = menu->GetSubMenu( found );
		if( old_popup == NULL )
			return;
		add_menu( depth+1, old_popup );
		}
	}

void menu_args::remove_menu( int depth, CMenu *menu )
	{
	int num_items = menu->GetMenuItemCount();
	int found = -1;

	// find the position of the menu for this level
	for( int pos=0; pos<num_items; pos++ )
		{
		char name_buf[32];
		int len = menu->GetMenuString( pos, name_buf, sizeof( name_buf )-1, MF_BYPOSITION );

		name_buf[len] = '\0';
		EmacsString name( name_buf );

		if( name == menu_names[depth-1] )
			{ found = pos; break; }
		}

	if( found < 0 )
		return;


	// found
	if( depth < num_menus )
		{
		// follow the sub menu down
		CMenu *old_popup = menu->GetSubMenu( found );
		if( old_popup == NULL )
			return;
		remove_menu( depth+1, old_popup );

		int num_items = old_popup->GetMenuItemCount();
		if( num_items == 0 )
			menu->RemoveMenu( found, MF_BYPOSITION );
		}
	else
		menu->RemoveMenu( found, MF_BYPOSITION );
	}               
	
static ui_add_menu_common( const EmacsString &menu_name, int first_arg );
static ui_remove_menu_common( const EmacsString &menu_name, int first_arg );


//
//	(UI-add-menu pos "id" "name1" "name2" ... "name9")
//
int ui_add_menu(void)
	{
	if( interactive() )
		{
		error( "UI-add-menu cannot be called interactively" );
		return 0;
		}

	return ui_add_menu_common( "Global", 1 );
	}

//
//	(Windows-remove-menu "name1" "name2" ... "name9")
//
int ui_remove_menu(void)
	{
	if( interactive() )
		{
		error( "UI-remove-menu cannot be called interactively" );
		return 0;
		}

	return ui_remove_menu_common( "Global", 1 );
	}

//
//	(UI-add-named-menu "menu-name" pos "id" "name1" "name2" ... "name9")
//
int ui_add_named_menu(void)
	{
	EmacsString menu_name;
	if( interactive() )
		{
		error( "UI-add-named-menu cannot be called interactively" );
		return 0;
		}

	if( !string_arg( 1 ) )
		return 0;

	menu_name = ml_value.asString();

	return ui_add_menu_common( menu_name, 2 );
	}

//
//	(UI-remove-menu "menu-name" "name1" "name2" ... "name9")
//
int ui_remove_named_menu(void)
	{
	EmacsString menu_name;
	if( interactive() )
		{
		error( "UI-remove-named-menu cannot be called interactively" );
		return 0;
		}

	if( string_arg( 1 ) )
		return 0;

	menu_name = ml_value.asString();

	return ui_remove_menu_common( menu_name, 2 );
	}



static EmacsMenu *find_menu( const EmacsString &menu_name )
	{
	EmacsMenu *menu = NULL;
	if( menu_name_table == NULL )
		{
		// initialise the menu table
		menu_name_table = new EmacsMenuNameTable( 8, 8 );

		if( menu_name_table == NULL )
			return NULL;

		menu_name_table->add( "Global", new EmacsMainMenu() );
		}

	menu = menu_name_table->find( menu_name );
	if( menu == NULL )
		{
		static const EmacsString popup_prefix( "Popup" );
		// create new menu
		if( menu_name.commonPrefix( popup_prefix ) == popup_prefix.length() )
			{
			// create a popup menu
			menu = new EmacsPopupMenu();
			menu_name_table->add( menu_name, menu );
			}
		else
			{
			// create a mode specific menu
			menu = new EmacsModeSpecificMenu();
			menu_name_table->add( menu_name, menu );
			}
		}

	return menu;
	}

static ui_add_menu_common( const EmacsString &menu_name, int first_arg )
	{
	EmacsMenu *menu = find_menu( menu_name );
	if( menu == NULL )
		return 0;

	menu_args args;

	args.collect_add_args( first_arg );
	if( ml_err )
		return 0;
		
	menu->add_menu( args );

	return 0;
	}

static ui_remove_menu_common( const EmacsString &menu_name, int first_arg )
	{
	EmacsMenu *menu = find_menu( menu_name );
	if( menu == NULL )
		return 0;

	menu_args args;

	args.collect_remove_args( first_arg );
	if( ml_err )
		return 0;
		
	menu->remove_menu( args );

	return 0;
	}

static int list_menu( CMenu *menu, int indent )
	{
	int num_items = menu->GetMenuItemCount();

	for( int pos=0; pos<num_items; pos++ )
		{
		char name_buf[64];
		int len = menu->GetMenuString( pos, name_buf, sizeof( name_buf )-1, MF_BYPOSITION );
		name_buf[len] = '\0';

		int id = menu->GetMenuItemID( pos );


		EmacsString buf;
		const EmacsString &id_string = menu_id_to_id_string( id );
		if( !id_string.isNull() )
			buf = FormatString( " \\(menu)%s\t" ) << id_string;
		else
			buf = FormatString(" %d\t\t") << id;
		bf_cur->ins_cstr( buf );
		buf = FormatString("%*s%s\n") << indent << "" << name_buf;
		bf_cur->ins_cstr( buf );

		CMenu *sub_menu = menu->GetSubMenu( pos );
		if( sub_menu )
			list_menu( sub_menu, indent+2 );
		}	 

	return 1;
	}

int ui_list_menus(void)
	{
	EmacsBufferRef old( bf_cur );

	EmacsBuffer::scratch_bfn( "Menu list", interactive() );

	bf_cur->ins_str( " ID\t\tMenus Name\n"
		" --\t\t----------\n" );

	CWnd *wnd = AfxGetApp()->m_pMainWnd;
	CMenu *menu = wnd->GetMenu();

	list_menu( menu, 2 );

	bf_cur->b_modified = 0;
	set_dot( 1 );
	old.set_bf();
	theActiveView->window_on( bf_cur );                                  

	return 0;
	}

//
//	(UI-popup-menu X Y)
//
int ui_popup_menu()
	{
	if( check_args( 2, 2 ) )
		return 0;

	EmacsMenu *popup = menu_name_table->find( "Popup" );

	if( popup ==  NULL )
		{
		error("No Popup menu defined");
		return 0;
		}

	int x = numeric_arg(1); if( ml_err ) return 0;
	int y = numeric_arg(2); if( ml_err ) return 0;

	CWinemacsView *view = (CWinemacsView *)theActiveView;

	CPoint menu_point;

	view->mapCoordinatedToWindows( x, y, menu_point.x, menu_point.y );
	view->ClientToScreen( &menu_point );

	if( !popup->menu()->TrackPopupMenu
		(
		TPM_LEFTALIGN|TPM_RIGHTBUTTON,
		menu_point.x, menu_point.y,
		view
		)
	)
		{
		error("TrackPopupMenu() failed"); return 0;
		}


	return 0;
	}
