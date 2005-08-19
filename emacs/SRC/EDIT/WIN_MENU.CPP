/*
 * 	win_menu.cpp
 *		windows menu commands
 */                 
#include "stdafx.h"    
#include "resource.h"
#include "winemdoc.h"
#include "winemvw.h"

#define MENU_DEPTH 9
class menu_args
{
public:
	menu_args();
public:      
	int num_menus;
	CString menu_names[MENU_DEPTH];
	int menu_pos[MENU_DEPTH];
	int menu_id;
public:
	void collect_args( int first_string );   
	void add_menu( int depth, CMenu *menu );
	void remove_menu( int depth, CMenu *menu );
};

struct menu_id_map
{
	unsigned int id;
	unsigned char *string;
};

static struct menu_id_map emacs_menu_id_map[] =
{
	ID_EDIT_COPY, u_str("ec"),
	ID_EDIT_CUT, u_str("ex"),
	ID_EDIT_CLEAR, u_str("er"),
	ID_EDIT_PASTE, u_str("ev"),
	ID_SEARCH_FIND, u_str("sf"),
	ID_SEARCH_REPLACE, u_str("sr"),
	ID_FILE_OPEN, u_str("fo"),
	ID_FILE_SAVE, u_str("fs"),
	ID_FILE_SAVE_AS, u_str("fa"),
	0, NULL
};

static struct menu_id_map user_menu_id_map[ID_USERMENU_LAST-ID_USERMENU_FIRST+1] =
{
0, NULL
};
static unsigned int next_menu_id = ID_USERMENU_FIRST;

unsigned char *menu_id_to_id_string( unsigned int id )
	{
	struct menu_id_map *p;

	/* if its a user menu id its an easy lookup */
	if( id >= ID_USERMENU_FIRST && id <= ID_USERMENU_LAST )
		return user_menu_id_map[id-ID_USERMENU_FIRST].string;
	
	/* hunt though the emacs menu ids */
	p = emacs_menu_id_map;
	while( p->string != NULL )
		if( p->id == id)
			return p->string;
		else
			p++;
	return NULL;
	}

unsigned int menu_id_string_to_id( unsigned char *id_string )
	{
	struct menu_id_map *p;
	int i;

	/* see if this is a emacs defined menu item */
	p = emacs_menu_id_map;
	while( p->string != NULL )
		if( _str_cmp( p->string, id_string ) == 0 )
			return p->id;
		else
			p++;

	/* see if its one of the users defined menus */
	p = user_menu_id_map;
	for( i=0; i<(ID_USERMENU_LAST-ID_USERMENU_FIRST+1); i++, p++ )
	     	if( p->string != NULL
		&& _str_cmp( p->string, id_string ) == 0 )
			return p->id;


	/* see if we have run out of space */
	if( next_menu_id > ID_USERMENU_LAST )
		return 0;

	/* add this id string to the table */
	user_menu_id_map[next_menu_id-ID_USERMENU_FIRST].string = savestr( id_string );
	user_menu_id_map[next_menu_id-ID_USERMENU_FIRST].id = next_menu_id;

	return next_menu_id++;
	}

menu_args::menu_args()
	{
	num_menus = 0;
	menu_id = 0;
	}

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
void menu_args::collect_args( int add )
	{
	int arg = 1;
	int last_arg = cur_exec->p_nargs;

	if( add )
		check_args( 3, 1+(MENU_DEPTH*2) );
	else
		check_args( 1, 1+MENU_DEPTH );

	if( err )
		return;
		
	if( add )
		{
		menu_id = -1;

		if( string_arg( 1 ) )
			menu_id = menu_id_string_to_id( ml_value->exp_v.v_string );

		if( menu_id == 0 )
			error(u_str("There are no free menu IDs left") );
		arg++;
		}

	while( !err && arg <= last_arg )
		{
		if( add )
			menu_pos[num_menus] = numeric_arg( arg++ );

		if( string_arg( arg++ ) )
			menu_names[num_menus] = s_str(ml_value->exp_v.v_string);

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
		if( strcmp( name_buf, menu_names[depth-1] ) == 0 )
			{ found = pos; break; }
		if( strcmp( name_buf, "&Help" ) == 0 )
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
			 (strcmp(menu_names[depth-1], "-") == 0 ? MF_SEPARATOR : MF_STRING),
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
		if( strcmp( name_buf, menu_names[depth-1] ) == 0 )
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

		int num_items = menu->GetMenuItemCount();
		if( num_items == 0 )
			menu->RemoveMenu( found, MF_BYPOSITION );
		}
	else
		{
		int id = menu->GetMenuItemID( pos );

		if( id >= ID_USERMENU_FIRST && id <= ID_USERMENU_LAST )
			menu->RemoveMenu( found, MF_BYPOSITION );
		}
	}               
	
//
//	(UI-add-menu pos "id" "name1" "name2" ... "name9")
//
int ui_add_menu(void)
	{
	menu_args args;

	args.collect_args(1);
	if( err )
		return 0;
		
	CWnd *wnd = AfxGetApp()->m_pMainWnd;
	CMenu *menu = wnd->GetMenu();

	args.add_menu( 1, menu );

	wnd->DrawMenuBar();

	return 0;
	}

//
//	(Windows-remove-menu "name1" "name2" ... "name9")
//
int ui_remove_menu(void)
	{
	menu_args args;

	args.collect_args(0);
	if( err )
		return 0;
		
	CWnd *wnd = AfxGetApp()->m_pMainWnd;
	CMenu *menu = wnd->GetMenu();

	wnd->DrawMenuBar();
	return 0;
	}

static int list_menu( CMenu *menu, int indent )
	{
	int num_items = menu->GetMenuItemCount();

	for( int pos=0; pos<num_items; pos++ )
		{
		char name_buf[64];
		int len = menu->GetMenuString( pos, name_buf, sizeof( name_buf )-1, MF_BYPOSITION );
		int id = menu->GetMenuItemID( pos );

		unsigned char buf[100];
		unsigned char *id_string = menu_id_to_id_string( id );
		if( id_string != NULL )
			sprintfl( buf, sizeof( buf ), u_str(" \\(menu)%s\t"), id_string );
		else
			sprintfl( buf, sizeof( buf ), u_str(" %d\t\t"), id );
		ins_str( buf );
		sprintfl( buf, sizeof( buf ), u_str("%*s%s\n"), indent, "", name_buf );
		ins_str( buf );

		CMenu *sub_menu = menu->GetSubMenu( pos );
		if( sub_menu )
			list_menu( sub_menu, indent+2 );
		}	 

	return 1;
	}

int ui_list_menus(void)
	{
	struct emacs_buffer *old;

	old = bf_cur;
	scratch_bfn( u_str("Menu list"), interactive );
	ins_str( u_str( " ID\t\tMenus Name\n"
			" --\t\t----------\n" ) );

	CWnd *wnd = AfxGetApp()->m_pMainWnd;
	CMenu *menu = wnd->GetMenu();

	list_menu( menu, 2 );

	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );                                  

	return 0;
	}
