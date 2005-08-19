//
// Motif X-Window system pseudo-terminal driver for DEC Emacs
//
//	Nick Emery Jun-93
//	adapted from spm's X driver 7/85	MIT Project Athena
//

#include <emacs.h>


#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#ifdef XWINDOWS
#include <emacs_motif.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/ToggleB.h>
#include <Xm/LabelG.h>
#include <Xm/CascadeBG.h>

/*
 * Own Storage:
 */
static EmacsMotifMenuCascade *menu_bar;
static EmacsMotifMenu *menu_cur;
static int insert_incomplete;
static EmacsString selection;

const int MAX_CHILDREN( 100 );
const int MAX_LABEL( 100 );

const int CB_CANCEL( 0 );
const int CB_APPLY( 1 );
const int CB_OK( 2 );
const int CB_POPDOWN( 3 );

/*
 * External References:
 */

extern int error_and_exit( void );
extern int newline_command( void );
extern int motif_not_running( void );
extern int region_to_string( void );
extern int motif_iconic;

int ui_frame_state()
	{
	if( motif_iconic )
		{
		ml_value = Expression( "minimized" );
		}
	else
		{
		ml_value = Expression( "normal" );
		}	

	return 0;
	}

/* Iconify emacs */
int ui_frame_minimize()
	{
	Arg args[1];

	if( !is_motif)
		return motif_not_running();

	XtSetArg( args[0], XmNiconic, 1 );
	XtSetValues( theMotifGUI->application.shell->widget, args, 1 );
	motif_iconic++;

	return 0;
	}

/* Deiconify emacs */
int ui_frame_restore()
	{
	Arg args[1];

	if( !is_motif)
		return motif_not_running();

	XtSetArg( args[0], XmNiconic, 0 );
	XtSetValues( theMotifGUI->application.shell->widget, args, 1 );
	motif_iconic = 0;

	return 0;
	}

/* Raise emacs to the top of the window stack */
int ui_frame_to_foreground(void)
	{
	if( !is_motif)
		return motif_not_running();

	XRaiseWindow( theMotifGUI->application.dpy, XtWindow( theMotifGUI->application.shell->widget ) );
	return 0;
	}

static void insert_func
	(
	Widget PNOTUSED(w), void *PNOTUSED(junk), Atom *PNOTUSED(sel),
	Atom *PNOTUSED(type), void * value, unsigned long *length, int *PNOTUSED(format)
	)
	{
	bf_cur->ins_cstr( (unsigned char *)value, (int)*length );
	insert_incomplete = 0;
	}

static Atom selection_type = XA_PRIMARY;

/* Place contents of the primary selection to the current Emacs buffer at dot */
int ui_edit_paste()
	{
	if( !is_motif)
		return motif_not_running();

	if( input_mode == 1 ) gui_input_mode_before_insert();

	insert_incomplete = 1;

	EmacsString clipboard_name;

	if( cur_exec != NULL && cur_exec->p_nargs >= 1 )
		{
		if( !string_arg( 1 ) )
			return 0;
		clipboard_name = ml_value.asString();
		}

	selection_type = theMotifGUI->application.clipboardType( clipboard_name );
	//_dbg_msg( FormatString("ui_edit_paste: selection_type=%d") << int( selection_type ) );

	// use XA_CLIPBOARD for KDE/Gnome compatibility
	// use XA_PRIMARY for old middle click compatibility
	XtGetSelectionValue
	(
	theMotifGUI->application.shell->main_window.widget,
	selection_type, XA_STRING,
	insert_func, NULL,
	XtLastTimestampProcessed( theMotifGUI->application.dpy )
	);

	while( insert_incomplete)
		theActiveView->k_input_event( NULL, 0 );

	theActiveView->do_dsp();

	ml_value.release_expr();

	return 0;
	}

/* Convert string in selection to required type */
static Boolean cvt_proc
	(
	Widget PNOTUSED(w),
	Atom *sel, Atom *target, Atom *type_return,
	void **value_return, unsigned long *length_return, int *format_return
	)
	{
	// use XA_CLIPBOARD for KDE/Gnome compatibility
	// use XA_PRIMARY for old middle click compatibility
	if( *sel != selection_type || *target != XA_STRING)
		return FALSE;

	*type_return = XA_STRING;
	*value_return = (void *)selection.sdata();
	*length_return = selection.length();
	*format_return = 8;

	return TRUE;
	}

static void lose_proc( Widget PNOTUSED(w), Atom *sel)
	{
	// use XA_CLIPBOARD for KDE/Gnome compatibility
	// use XA_PRIMARY for old middle click compatibility
	if( *sel == selection_type )
		selection = EmacsString::null;
	}

static void done_proc( Widget PNOTUSED(w), Atom *PNOTUSED(sel), Atom *PNOTUSED(target) )
	{ }

/* Copy Emacs buffer to X server cut/paste buffer 0. */
int ui_edit_copy()
	{
	if( !is_motif)
		return motif_not_running();

	EmacsString clipboard_name;
	if( cur_exec != NULL && cur_exec->p_nargs >= 1 )
		{
		if( !string_arg( 1 ) )
			return 0;
		clipboard_name = ml_value.asString();
		}

	selection_type = theMotifGUI->application.clipboardType( clipboard_name );
	//_dbg_msg( FormatString("ui_edit_paste: selection_type=%d") << int( selection_type ) );

	selection = EmacsString::null;
	region_to_string();
	if( ml_err)
		selection = EmacsString::null;
	else
		{
		Boolean resp;
		selection = ml_value.asString();
		void_result();
		resp = XtOwnSelection
			(
			theMotifGUI->application.shell->main_window.widget,
			selection_type,
			XtLastTimestampProcessed( theMotifGUI->application.dpy ),
			cvt_proc,
			lose_proc,
			done_proc
			);
		}

	ml_value.release_expr();

	return 0;
	}

/* Store information about the menus into a buffer */
void EmacsMotifMenuSeparator::dump( int level )
	{
	to_col( level );

	EmacsString line( FormatString( "%-20s separator at position %d" ) << name << pos );
	bf_cur->ins_cstr( line );
	newline_command();
	}

void EmacsMotifMenuButton::dump( int level )
	{
	to_col( level );

	EmacsString line( FormatString( "%-20s button at position %d bound to %s" ) << name << pos << action.displayAction() );
	bf_cur->ins_cstr( line  );
	newline_command();
	}

void EmacsMotifMenuCascade::dump( int level )
	{
	to_col( level );

	EmacsString line( FormatString( "%-20s cascade at position %d" ) << name << pos );
	bf_cur->ins_cstr( line );
	newline_command();

	for( EmacsMotifMenu *cur = children; cur != NULL; cur = cur->next)
		cur->dump( level + 2 );
	}

/* Command interface to UI-dump-menu */
int motif_list_menus_command( void)
	{
	EmacsBufferRef old( bf_cur );

	if( !is_motif)
		return motif_not_running();

	EmacsBuffer::scratch_bfn( "Menu table", interactive() );
	bf_cur->ins_str( "Motif Menus\n" );
	bf_cur->ins_str( "-----------\n\n" );

	menu_bar->dump( 2 );

	bf_cur->b_checkpointed = -1;
	bf_cur->b_modified = 0;
	set_dot( 1 );
	old.set_bf();

	theActiveView->window_on( bf_cur );

	return 0;
	}

/* convert a menu push to a key sequence */
static void menu_to_keys( Widget PNOTUSED(w), void *button_, void * PNOTUSED(info))
	{
	EmacsMotifMenuButton *button = (EmacsMotifMenuButton *)button_;

	button->action.queueAction();
	}

static Cardinal insert_pos( XmRowColumnRec *w)
	{
	Arg args[2];
	int count = 0;
	int i = 0;
	Widget *children, parent, prev;

	if( menu_cur == NULL)
		{
		parent = XtParent( w );
		XtSetArg( args[0], XmNnumChildren, &i );
		i = 0;
		XtGetValues( parent, args, 1 );
		}
	else
		{
		prev = menu_cur->button;
		parent = XtParent( w );
		XtSetArg( args[0], XmNnumChildren, &count );
		XtSetArg( args[1], XmNchildren, &children );
		XtGetValues( parent, args, 2 );

		for( i = 0; i < count; i++)
			if( children[i] == prev)
				break;
		}

	return i;
	}

EmacsMotif_ActionItem::EmacsMotif_ActionItem()
	: keys( EmacsString::null )
	, proc( NULL )
	, var( NULL )
	, callback( NULL )
	, button( NULL )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( Widget _button, bool _emulate_toggle )
	: keys( EmacsString::null )
	, proc( NULL )
	, var( NULL )
	, callback( NULL )
	, button( _button )
	, emulate_toggle( _emulate_toggle )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( const EmacsString &_keys )
	: keys( _keys )
	, proc( NULL )
	, var( NULL )
	, callback( NULL )
	, button( NULL )
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( BoundName *_proc )
	: keys( EmacsString::null )
	, proc( _proc )
	, var( NULL )
	, callback( NULL )
	, button( NULL )
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( const EmacsString &_keys, VariableName *_var )
	: keys( _keys )
	, proc( NULL )
	, var( _var )
	, callback( NULL )
	, button( NULL )
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( BoundName *_proc, VariableName *_var )
	: keys( EmacsString::null )
	, proc( _proc )
	, var( _var )
	, callback( NULL )
	, button( NULL )
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( const EmacsMotif_ActionItem &in )
	: keys( in.keys )
	, proc( in.proc )
	, var( in.var )
	, callback( in.callback )
	, button( NULL )	// must not copy the button
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::EmacsMotif_ActionItem( void (*_callback)(Widget) )
	: keys( EmacsString::null )
	, proc( NULL )
	, var( NULL )
	, callback( _callback )
	, button( NULL )
	, emulate_toggle( false )
	{ }

EmacsMotif_ActionItem::~EmacsMotif_ActionItem()
	{ }

EmacsMotif_ActionItem &EmacsMotif_ActionItem::operator=( const EmacsMotif_ActionItem &in )
	{
	keys = in.keys;
	proc = in.proc;
	var = in.var;
	callback = in.callback;
	// must not copy the button
	return *this;
	}

bool EmacsMotif_ActionItem::isToggleButton() const
	{
	return var != NULL;
	}

void EmacsMotif_ActionItem::setButton( Widget _button, bool _emulate_toggle )
	{
	button = _button;
	emulate_toggle = _emulate_toggle;
	}

void EmacsMotif_ActionItem::queueAction()
	{
	theMotifGUI->action = this;

	//
	//	If this key was bound to "interrupt-key", purge the internal
	//	typeahead buffer and declare the interrupt
	//
	if( proc == &interrupt_block )
		theActiveView->k_interrupt_emacs();
	}
	
int EmacsMotif_ActionItem::doAction()
	{
	int rv=0;

	if( callback != NULL )
		{
		callback( button );
		theActiveView->do_dsp();
		}
	else if( proc != NULL )
		{
		// turn off the shift state
		gui_input_shift_state( false );
		// execute the procedure
		rv = execute_bound_saved_environment( proc );
		// update the display
		theActiveView->do_dsp();
		}
	else
		theMotifGUI->input_char_string( keys, false );

	return rv;
	}

void EmacsMotif_ActionItem::setState()
	{
	if( var == NULL )
		return;

	int new_state = 0;
	EmacsString errmsg;
	fetch_var( var, &new_state, NULL, errmsg );

	// set the buttons state...
	


#ifndef TOOLBAR_TOGGLE_BUTTONS
	if( emulate_toggle )
		{
		int shadow_type = -1;
		EmacsXtArgs args;
		args.setArg( XmNshadowType, &shadow_type );
		args.getValues( button );
		
		/* if( (new_state != 0) != (shadow_type == XmSHADOW_IN) ) */
			{
			EmacsXtArgs args;
			args.setArg( XmNshadowType, (new_state ? XmSHADOW_IN : XmSHADOW_OUT) );
			args.setValues( button );
			}
		}
	else
#endif
		{
		if( (new_state != 0) != (XmToggleButtonGetState( button ) != 0) )
			XmToggleButtonSetState( button, new_state != 0, false );
		}
	}

EmacsString EmacsMotif_ActionItem::displayAction()
	{
	EmacsString result;
	if( callback )
		result = "Builtin action";
	else if( proc )
		result = FormatString("(%s)") << proc->b_proc_name;
	else
		result = FormatString("\"%s\"") << key_to_str( keys );
	if( var != NULL )
		result.append( FormatString(" state variable %s") << var->v_name );

	return result;
	}

bool EmacsMotif_ActionItem::isSeparator() const
	{
	return keys == "-";
	}


void TerminalControl_GUI::fixup_menus(void)
	{
	menu_bar->updateState();
	}

void EmacsMotifMenu::updateState()
	{ }

void EmacsMotifMenuButton::updateState()
	{
	action.setState();
	}

void EmacsMotifMenuCascade::updateState()
	{
	for( EmacsMotifMenu *cur = children; cur != NULL; cur = cur->next)
		cur->updateState();
	}

EmacsMotifMenu::EmacsMotifMenu( const EmacsString &_name, int _pos )
	: next( NULL )
	, name( _name )
	, pos( _pos )
	, button( NULL )
	{ }

EmacsMotifMenu::~EmacsMotifMenu()
	{
	// delete the whole list of menu items
	delete next;
	if( button )
		XtDestroyWidget( button );
	}

EmacsMotifMenuButton::EmacsMotifMenuButton( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	, action()
	{ }

EmacsMotifMenuButton::~EmacsMotifMenuButton()
	{ }

EmacsMotifMenuCascade::EmacsMotifMenuCascade( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	, children( NULL )
	, menu( NULL )
	, ccount( 0 )
	{ }

EmacsMotifMenuCascade::~EmacsMotifMenuCascade()
	{
	// delete any children
	delete children;
	if( menu )
		XtDestroyWidget( menu );
	}

EmacsMotifMenuSeparator::EmacsMotifMenuSeparator( const EmacsString &name, int pos )
	: EmacsMotifMenu( name, pos )
	{ }

EmacsMotifMenuSeparator::~EmacsMotifMenuSeparator()
	{ }

EmacsMotifMenu *EmacsMotifMenu::makeButton( const EmacsString &, int, int, const EmacsMotif_ActionItem & )
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}

EmacsMotifMenu *EmacsMotifMenu::makeSeparator( const EmacsString &, int )
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}

EmacsMotifMenu *EmacsMotifMenu::makeCascade( const EmacsString &, int, int)
	{
	error( FormatString("Parent of button \"%s\" must be a cascade menu") << name );
	return NULL;
	}


/* Create a push button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeSeparator
	(
	const EmacsString &button_name, int button_pos
	)
	{
	Arg args[1];
	Widget button;
	EmacsMotifMenuSeparator *new_button = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error( FormatString("Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_button = new EmacsMotifMenuSeparator( button_name, button_pos );
	if( new_button == NULL )
		{
		error( FormatString("No memory to create button \"%s\"") << name );
		return NULL;
		}

	new_button->button = button = XmCreateSeparatorGadget( menu, new_button->name.sdataHack(), args, 0 );
	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > button_pos)
			break;
	menu_cur = cur;
	ccount++;
	new_button->next = cur;
	if( prev != NULL)
		prev->next = new_button;
	else
		children = new_button;

	XtManageChild( button );

	return new_button;
	}

/* Create a push button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeButton
	(
	const EmacsString &button_name, int button_pos, int mn,
	const EmacsMotif_ActionItem &action
	)
	{
	Widget button;
	EmacsMotifMenuButton *new_button = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error( FormatString("Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_button = new EmacsMotifMenuButton( button_name, button_pos );
	if( new_button == NULL )
		{
		error( FormatString("No memory to create button \"%s\"") << name );
		return NULL;
		}

	new_button->action = action;

	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > button_pos)
			break;
	menu_cur = cur;

	EmacsXtArgs args;
	if( mn != 0)
		args.setArg( XmNmnemonic, mn );

	if( action.isToggleButton() )
		{
		button = XmCreateToggleButtonGadget( menu, new_button->name.sdataHack(), args, args );
		XtAddCallback( button, XmNvalueChangedCallback, menu_to_keys, (XtPointer)new_button );
		}
	else
		{
		//
		// add space to the left so that menus without toggle buttons line up
		// with the toggle button menu items
		//
		args.setArg( XmNmarginLeft, 12 );

		button = XmCreatePushButtonGadget( menu, new_button->name.sdataHack(), args, args );
		XtAddCallback( button, XmNactivateCallback, menu_to_keys, (XtPointer)new_button );
		}

	new_button->button = button;
	new_button->action.setButton( button, false );

	ccount++;
	new_button->next = cur;
	if( prev != NULL)
		prev->next = new_button;
	else
		children = new_button;
	XtManageChild( button );

	return new_button;
	}

/* Create a cascade button in a menu pane */
EmacsMotifMenu *EmacsMotifMenuCascade::makeCascade( const EmacsString &cascade_name, int cascade_pos, int mn)
	{
	Arg args[2];
	int argc, is_help;
	Widget pulldown, cascade;
	EmacsMotifMenuCascade *new_cascade = NULL;
	EmacsMotifMenu *prev, *cur;

	if( ccount >= MAX_CHILDREN)
		{
		error(  FormatString( "Too many buttons in menu \"%s\"") << name );
		return NULL;
		}

	new_cascade = new EmacsMotifMenuCascade( cascade_name, cascade_pos );
	if( new_cascade == NULL )
		{
		error( FormatString( "No memory to create cascade button \"%s\"") << cascade_name );
		return NULL;
		}

	is_help = this == menu_bar && cascade_name.caseBlindCompare( "HELP" ) == 0;
	new_cascade->children = NULL;
	if( is_help)
		pos = 0x7FFFFFFF;

	new_cascade->ccount = 0;
	for( cur = children, prev = NULL; cur != NULL; prev = cur, cur = cur->next)
		if( cur->pos > cascade_pos)
			break;
	menu_cur = cur;
	argc = 0;
	if( !is_help)
		{
		XtSetArg( args[0], XmNinsertPosition, insert_pos);
		argc++;
		}
	new_cascade->menu = pulldown = XmCreatePulldownMenu( menu, new_cascade->name.sdataHack(), args, argc );
	argc = 0;
	XtSetArg( args[0], XmNsubMenuId, pulldown );
	argc++;
	if( mn != 0)
		{
		XtSetArg( args[argc], XmNmnemonic, mn );
		argc++;
		}
	new_cascade->button = cascade = XmCreateCascadeButtonGadget( menu, new_cascade->name.sdataHack(), args, argc );
	if( is_help)
		{
		XtSetArg( args[0], XmNmenuHelpWidget, new_cascade->button );
		XtSetValues( menu, args, 1 );
		}
	ccount++;
	new_cascade->next = cur;
	if( prev != NULL)
		prev->next = new_cascade;
	else
		children = new_cascade;
	XtManageChild( cascade );

	return new_cascade;
	}

void EmacsMotifMenu::removeMenuItem( int )
	{
	error(  FormatString( "Parent of button \"%s\" must be a cascade menu") << name );
	}

void EmacsMotifMenuCascade::removeMenuItem( int index )
	{
	if( string_arg( index ) == 0)
		return;

	EmacsString name = ml_value.asString();

	EmacsMotifMenu *cur, *prev;
	for( prev = NULL, cur = children; cur != NULL; prev = cur, cur = cur->next)
		if( name == cur->name )
			break;

	if( cur == NULL )
		{
		error( FormatString( "No such menu as \"%s\"") << name );
		return;
		}

	if( index < cur_exec->p_nargs )
		{
		cur->removeMenuItem( index+1 );
		return;
		}

	// Remove the entry from the siblings
	if( prev == NULL)
		children = cur->next;
	else
		prev->next = cur->next;

	// Destroy the EmacsMotifMenu tree
	cur->next = NULL;
	delete cur;
	}

/* Remove a menu and all of its submenu items from a menu */
int motif_remove_menu_command( void)
	{
	if( !is_motif)
		return motif_not_running();

	if( !check_args( 1, 1000000))
		menu_bar->removeMenuItem( 1 );

	return 0;
	}

void EmacsMotifMenu::addMenuItem( const EmacsMotif_ActionItem &, int )
	{
	error( FormatString( "Parent of button \"%s\" must be a cascade menu") << name );
	}

void EmacsMotifMenuCascade::addMenuItem( const EmacsMotif_ActionItem &action, int index )
	{
	int pos = numeric_arg( index );
	if( ml_err )
		return;
	if( string_arg( index + 1 ) == 0 )
		return;
	EmacsString name( ml_value.asString() );

	if( name.length() > MAX_LABEL)
		{
		error( FormatString( "The name of button \"%s\" is too long") << name );
		return;
		}

	int ampersand_pos = name.first( '&' );
	int mn = 0;
	if( ampersand_pos >= 0 && ampersand_pos+1 < name.length() )
		{
		name.remove( ampersand_pos, 1 );
		mn = name[ ampersand_pos ];
		}

	EmacsMotifMenu *cur;
	for( cur = children; cur != NULL; cur = cur->next)
		if( name == cur->name )
			break;
	if( (index+2) < cur_exec->p_nargs )
		{
		if( cur == NULL )
			cur = makeCascade( name, pos, mn );
		if( cur != NULL )
			cur->addMenuItem( action, index + 2 );
		}
	else
		{
		if( cur != NULL )
			error(  FormatString( "Button \"%s\" already exists") << name );
		else
			if( action.isSeparator() )
				cur = makeSeparator( name, pos );
			else
				cur = makeButton( name, pos, mn, action );
		}
	}

//
// Extended command to add a menu button
// (UI-add-menu-keys <keys> [<var>] <pos> <name> [<pos> <name>]*)
//
static unsigned char no_variable [] = "No variable exists with name \"%s\"";

int motif_add_menu_keys_command( void)
	{
	if( !is_motif )
		return motif_not_running();

	if( cur_exec->p_nargs & 1 )
		{
		// odd mean no <var>
		if( check_args( 3, 1000000 ) )
			return 0;
		if( !string_arg(1) )
			return 0;

		EmacsString keys( ml_value.asString() );

		menu_bar->addMenuItem( keys, 2 );
		}
	else
		{
		// even there is a var
		if( check_args( 3, 1000000 ) )
			return 0;
		if( !string_arg(1) )
			return 0;

		EmacsString keys( ml_value.asString() );
		if( !string_arg(2) )
			return 0;

		EmacsString varname( ml_value.asString() );

		VariableName *nm = VariableName::find( varname );
		if( nm == NULL )
			{
			error( FormatString( no_variable ) << varname );
			return 0;
			}

		menu_bar->addMenuItem( EmacsMotif_ActionItem( keys, nm ), 3 );
		}

	return 0;
	}

int motif_add_menu_proc_command( void)
	{
	if( !is_motif )
		return motif_not_running();

	int min = (cur_exec->p_nargs & 1) ? 3 : cur_exec->p_nargs + 1;
	if( !check_args( min, 1000000)
	&& string_arg(1) )
		{
		EmacsString proc_name( ml_value.asString() );
		BoundName *proc = BoundName::find( proc_name );
		if( proc == NULL )
			error( FormatString("UI-add-menu-procedure: expects arg 2 (%s) to be a procedure") << proc_name );
		else
			menu_bar->addMenuItem( proc, 2 );
		}
	return 0;
	}

/* Build the builtin menus */
void debug_menu_callback( Widget )
	{
	extern TerminalControl_GUI *theMotifGUI;

	theMotifGUI->application.shell->debugPrintWidgetTree( "click" );
	}

void xwin_menu_init( Widget menu_bar_widget)
	{
	EmacsMotifMenu *item;

	menu_bar = new EmacsMotifMenuCascade( "menu_bar", 0 );
	menu_bar->menu = menu_bar_widget;

	// need one item on the menu bar otherwise it does not size properly
	item = menu_bar->makeCascade( "File", 10, 'F' );

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		{
		EmacsMotif_ActionItem action( debug_menu_callback );
		item->makeButton( "Debug", 10, 'D', action );
		}
#endif
	}


//
//	(UI-add-tool-bar-button-keys <tool-bar> <name> <label> <keys> [<var>])
//
int motif_add_tool_bar_button_keys_command(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( check_args( 4, 5 ) )
		return 0;

	string_arg(1);
	EmacsString tool_bar_name( ml_value.asString() );

	if( tool_bar_name != "Global" )
		{
		error( "UI-add-tool-bar-button-keys: The tool bar must be named \"Global\"" );
		return 0;
		}

	string_arg(2);
	EmacsString resource_name( ml_value.asString() );
	if( resource_name.isNull() )
		{
		error( "UI-add-tool-bar-button-keys: arg 2 must not be null" );
		return 0;
		}

	string_arg(3);
	EmacsString button_label( ml_value.asString() );
	if( button_label.isNull() )
		{
		error( "UI-add-tool-bar-button-keys: arg 3 must not be null" );
		return 0;
		}

	string_arg(4);
	EmacsString keys( ml_value.asString() );
	if( keys.isNull() )
		{
		error( "UI-add-tool-bar-button-keys: arg 4 must not be null" );
		return 0;
		}

	if( ml_err )
		return 0;

	EmacsMotif_ToolBar_Button *button;

	if( cur_exec->p_nargs > 4 )
		{
		if( !string_arg(5) )
			return 0;

		EmacsString varname( ml_value.asString() );

		VariableName *nm = VariableName::find( varname );
		if( nm == NULL )
			{
			error( FormatString( no_variable ) << varname );
			return 0;
			}

		button = new EmacsMotif_ToolBar_Button
			(
			theMotifGUI->application,
			theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column,
			resource_name, button_label, EmacsMotif_ActionItem( keys, nm )
			);		
		}
	else
		button = new EmacsMotif_ToolBar_Button
			(
			theMotifGUI->application,
			theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column,
			resource_name, button_label, keys
			);		

	button->preRealizeInit();
	button->postRealizeInit();

	return 0;
	}

//
//	(UI-add-tool-bar-button-procedure <tool-bar> <name> <label> <proc>)
//
int motif_add_tool_bar_button_proc_command(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( check_args( 4, 4 ) )
		return 0;

	string_arg(1);
	EmacsString tool_bar_name( ml_value.asString() );

	if( tool_bar_name != "Global" )
		{
		error( "UI-add-tool-bar-button-procedure: The tool bar must be named \"Global\"" );
		return 0;
		}

	string_arg(2);
	EmacsString resource_name( ml_value.asString() );
	if( resource_name.isNull() )
		{
		error( "UI-add-tool-bar-button-procedure: arg 2 must not be null" );
		return 0;
		}

	string_arg(3);
	EmacsString button_label( ml_value.asString() );
	if( button_label.isNull() )
		{
		error( "UI-add-tool-bar-button-procedure: arg 3 must not be null" );
		return 0;
		}

	string_arg(4);
	EmacsString proc_name( ml_value.asString() );
	BoundName *proc =  BoundName::find( proc_name );
	if( proc == NULL )
		{
		error( FormatString("UI-add-tool-bar-button-procedure: arg 4 (%s) must be a procedure") << proc_name );
		return 0;
		}

	if( ml_err )
		return 0;

	EmacsMotif_ToolBar_Button *button =
		new EmacsMotif_ToolBar_Button
			(
			theMotifGUI->application,
			theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column,
			resource_name, button_label, proc
			);		

	button->preRealizeInit();
	button->postRealizeInit();

	return 0;
	}

int motif_remove_tool_bar_button_command(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( check_args( 2, 2 ) )
		return 0;

	string_arg(1);
	EmacsString tool_bar_name( ml_value.asString() );

	if( tool_bar_name != "Global" )
		{
		error( "UI-remove-tool-bar-button: The tool bar must be named \"Global\"" );
		return 0;
		}

	string_arg(2);
	EmacsString resource_name( ml_value.asString() );
	if( resource_name.isNull() )
		{
		error( "UI-remove-tool-bar-button: arg 2 must not be null" );
		return 0;
		}

	// return true if the button or separator was found
	ml_value = theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column.destroyChildWidget( resource_name );

	return 0;
	}

//
//	(UI-add-tool-bar-separator <tool-bar> <name> <width>)
//
int motif_add_tool_bar_separator_command(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( check_args( 2, 3 ) )
		return 0;

	string_arg(1);
	EmacsString tool_bar_name( ml_value.asString() );

	if( tool_bar_name != "Global" )
		{
		error( "UI-add-tool-bar-separator: The tool bar must be named \"Global\"" );
		return 0;
		}

	string_arg(2);
	EmacsString resource_name( ml_value.asString() );
	if( resource_name.isNull() )
		{
		error( "UI-add-tool-bar-separator: arg 2 must not be null" );
		return 0;
		}

	int width = 0;
	if( cur_exec->p_nargs >= 3 )
		width = numeric_arg(3);

	if( ml_err )
		return 0;

	EmacsMotif_ToolBar_Separator *separator =
		new EmacsMotif_ToolBar_Separator
			(
			theMotifGUI->application,
			theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column,
			resource_name, width
			);		

	separator->preRealizeInit();
	separator->postRealizeInit();

	return 0;
	}

void TerminalControl_GUI::fixup_toolbars(void)
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column.widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );

			EmacsString line;
			if( core_object->widget_type == "XmPushButton"
			||  core_object->widget_type == "XmToggleButton" )
				{
				EmacsMotif_ToolBar_Button *button = (EmacsMotif_ToolBar_Button *)core_object;

				button->action.setState();
				}
			}
		}
	}

int motif_list_tool_bars_command(void)
	{
	if( !is_motif)
		return motif_not_running();

	EmacsBufferRef old( bf_cur );

	EmacsBuffer::scratch_bfn( "Tool bars list", interactive() );

	bf_cur->ins_str("Tool bar: Global\n\n"
		"   Name                 Label           Bound keys\n"
		"   ----                 -----           ----------\n" );

	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( theMotifGUI->application.shell->main_window.screen_form.tool_bar.row_column.widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );

			EmacsString line;
			if( core_object->widget_type == "XmPushButton"
			||  core_object->widget_type == "XmToggleButton" )
				{
				EmacsMotif_ToolBar_Button *button = (EmacsMotif_ToolBar_Button *)core_object;

				if( button->label.length() > 15 )
					{
					line = FormatString( "   %-20s %-15s\n                                        %s" )
						<< button->widget_name
						<< button->label
						<< button->action.displayAction();
					}
				else
					{
					line = FormatString( "   %-20s %-15s %s" )
						<< button->widget_name
						<< button->label
						<< button->action.displayAction();
					}
				}
			else if( core_object->widget_type == "XmDrawingArea" )
				{
				EmacsMotif_ToolBar_Button *button = (EmacsMotif_ToolBar_Button *)core_object;

				line = FormatString( "   %-20s -- Separator --" )
						<< button->widget_name;
				}

			bf_cur->ins_cstr( line  );
			newline_command();
			}
		}

	bf_cur->b_checkpointed = -1;
	bf_cur->b_modified = 0;
	set_dot( 1 );
	old.set_bf();

	theActiveView->window_on( bf_cur );

	return 0;
	}

#endif
