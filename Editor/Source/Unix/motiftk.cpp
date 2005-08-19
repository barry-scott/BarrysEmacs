// 
// Motif X-Window system widget toolkit
//
//	Nick Emery Jun-93
//	adapted from spm's X driver 7/85	MIT Project Athena
//

#include <emacs.h>

#ifdef XWINDOWS
#include <emacs_motif.h>
#include <Xm/SeparatoG.h>
#include <Xm/PushBG.h>
#include <Xm/LabelG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/FileSB.h>


//
// Data Types:
//

const int MAX_ARGS( 20 );

enum EmacsDialogResult
	{
	CB_CANCEL,
	CB_APPLY,
	CB_OK
	};

static const char * DIALOG_FORM = "form dialog";
static const char * DIALOG_FILE = "file selection dialog";
static const char * DIALOG_MESSAGE = "message dialog";

class EmacsDialog;
class EmacsPushButtonWidget;

//
//	points to the last ok or cancel button added to a dialog
//
static EmacsPushButtonWidget *exit_widget;

class EmacsWidget
	{
public:
	EmacsWidget( const EmacsString &name );
	virtual ~EmacsWidget();

	EmacsWidget *next;		// Next sibling in sequence
	Widget w;			// Widget ID
	EmacsString var;		// Name of variable for widget
	virtual int fetch() { return 0; }	// Fetch for widget callback
	virtual int store() { return 0; }	// Store from widget callback
	};

class EmacsPushButtonWidget : public EmacsWidget
	{
public:
	EmacsPushButtonWidget()
		: EmacsWidget( EmacsString::null )
		{ }
	virtual ~EmacsPushButtonWidget()
		{ };

	// the interface between X and this class
	static void button_press_cb( Widget w, void *d_, void *junk );
	// the individual button_press action
	virtual void button_press_action( EmacsDialog *d ) = 0;
	};

class EmacsPushButtonUserWidget : public EmacsPushButtonWidget
	{
public:
	EmacsPushButtonUserWidget( const EmacsString &keys )
		: EmacsPushButtonWidget()
		, push_action_keys( keys )
		{ }
	virtual ~EmacsPushButtonUserWidget()
		{ };
	virtual void button_press_action( EmacsDialog *d );
	EmacsString push_action_keys;	// keys to send when this button is pressed
	};

class EmacsPushButtonOkWidget : public EmacsPushButtonWidget
	{
public:
	EmacsPushButtonOkWidget()
		: EmacsPushButtonWidget()
		{
		exit_widget = this;
		}
	virtual ~EmacsPushButtonOkWidget()
		{ };
	virtual void button_press_action( EmacsDialog *d ) { button_press_ok_action( d ); }
	static void button_press_ok_action( EmacsDialog *d );
	};

class EmacsPushButtonCancelWidget : public EmacsPushButtonWidget
	{
public:
	EmacsPushButtonCancelWidget()
		: EmacsPushButtonWidget()
		{
		exit_widget = this;
		}
	virtual ~EmacsPushButtonCancelWidget()
		{ };
	virtual void button_press_action( EmacsDialog *d ) { button_press_cancel_action( d ); }
	static void button_press_cancel_action( EmacsDialog *d );
	};

class EmacsPushButtonApplyWidget : public EmacsPushButtonWidget
	{
public:
	EmacsPushButtonApplyWidget()
		: EmacsPushButtonWidget()
		{ }
	virtual ~EmacsPushButtonApplyWidget()
		{ };
	virtual void button_press_action( EmacsDialog *d ) { button_press_apply_action( d ); }
	static void button_press_apply_action( EmacsDialog *d );
	};

class EmacsToggleButtonWidget : public EmacsWidget
	{
public:
	EmacsToggleButtonWidget( const EmacsString &name )
		: EmacsWidget( name )
		{ }
	virtual ~EmacsToggleButtonWidget()
		{ };

	int fetch();
	int store();
	};

class EmacsScaleWidget : public EmacsWidget
	{
public:
	EmacsScaleWidget( const EmacsString &name )
		: EmacsWidget( name )
		{ }
	virtual ~EmacsScaleWidget()
		{ };

	int fetch();
	int store();
	};

class EmacsTextFieldWidget : public EmacsWidget
	{
public:
	EmacsTextFieldWidget( const EmacsString &name )
		: EmacsWidget( name )
		{ }
	virtual ~EmacsTextFieldWidget()
		{ };

	int fetch();
	int store();
	};

class EmacsLabelWidget : public EmacsWidget
	{
public:
	EmacsLabelWidget( const EmacsString &name )
		: EmacsWidget( name )
		{ }
	virtual ~EmacsLabelWidget()
		{ };

	int fetch();
	};

class EmacsFileSelectionWidget : public EmacsWidget
	{
public:
	EmacsFileSelectionWidget( const EmacsString &name1, const EmacsString &name2 )
		: EmacsWidget( name1 )
		, store_var( name2 )
		{ }
	virtual ~EmacsFileSelectionWidget()
		{ };

	EmacsString store_var;		// Name of variable for store widget

	int fetch();
	int store();
	};

class EmacsMessageBoxWidget : public EmacsWidget
	{
public:
	EmacsMessageBoxWidget( const EmacsString &name )
		: EmacsWidget( name )
		{ }
	virtual ~EmacsMessageBoxWidget()
		{ };

	int fetch();
	};

class EmacsDialog
	{
public:
	EmacsDialog( const EmacsString &name );
	~EmacsDialog();

	EmacsDialog *next;		// Next widget
	EmacsString name;		// Name of the dialog
	EmacsString apply_keys;		// Key sequence to generate for unsolicited apply
	const char *type;		// Type of dialog
	Widget widget;			// Widget for the dialog
	Widget dialog;			// Widget for the dialog shell
	int inuse;			// True if the dialog is in use
	int incomplete;			// True if the dialog is not complete
	EmacsDialogResult result;	// Result of the dialog pop-up
	EmacsWidget *children;		// Widget children of dialog
	};

//
// Own Storage:
//

static EmacsString no_mem( "No memory for %s \"%s\"" );
static EmacsString already_exists( "Motif widget \"%s\" already exists" );
static EmacsString motif_problem( "Probelm interacting with Motif for \"%s\"" );
static EmacsString doesnt_exist( "Dialog \"%s\" does not exists" );
static EmacsString inuse( "Dialog \"%s\" is in use" );
static EmacsString root_active( "Already building dialog \"%s\"" );
static EmacsString root_inactive( "Not building a dialog that can contain %s" );
static EmacsString must_be_var( "%s expects argument %d to be a variable name" );
static int x_pos = 0, y_pos = 0;
static EmacsDialog *dialogs = NULL;
static XmStringCharSet charset =( XmStringCharSet ) XmSTRING_DEFAULT_CHARSET;
static EmacsDialog *root_dialog;
static Widget root;
static int root_seq = 0;
int motif_insensitive = 0;

//
// External References:
//

extern int motif_not_running( void );

EmacsWidget::EmacsWidget( const EmacsString &name )
	: next(NULL)
	, w(0)
	, var( name )
	{ }

EmacsWidget::~EmacsWidget()
	{
	delete next;
	}

EmacsDialog::EmacsDialog( const EmacsString &_name )
	: next( dialogs )
	, name( _name )
	, apply_keys()
	, type(0)
	, widget(NULL)
	, dialog(NULL)
	, inuse(0)
	, incomplete(0)
	, result(CB_OK)
	, children(NULL)
	{
	dialogs = this;
	}

EmacsDialog::~EmacsDialog()
	{
	delete children;
	}



// Find a dialog in the list
static EmacsDialog *find_dialog( const EmacsString &name )
	{
	for( EmacsDialog *w = dialogs; w; w = w->next )
		if( name == w->name )
			return w;

	return NULL;
	}

static EmacsDialog *make_dialog( const EmacsString &name )
	{
	EmacsDialog *w = new EmacsDialog( name );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "dialog" << name );
		return NULL;
		}

	return w;
	}

static void destroy_dialog( const EmacsString &name )
	{
	EmacsDialog *w, *prev;

	for( prev = NULL, w = dialogs; w != NULL; prev = w, w = w->next )
		if( name == w->name )
			{
			if( prev == NULL )
				dialogs = w->next;
			else
				prev->next = w->next;
			delete w;
			}
	}

static void ok_cb( Widget PNOTUSED(w ), void *d_, void *PNOTUSED(junk ) )
	{
	EmacsPushButtonOkWidget::button_press_ok_action( (EmacsDialog *)d_ );
	}

static void cancel_cb( Widget PNOTUSED(w ), void *d_, void *PNOTUSED(junk ) )
	{
	EmacsPushButtonCancelWidget::button_press_cancel_action( (EmacsDialog *)d_ );
	}

static void apply_cb( Widget PNOTUSED(w ), void *d_, void *PNOTUSED(junk ) )
	{
	EmacsPushButtonApplyWidget::button_press_apply_action( (EmacsDialog *)d_ );
	}

void EmacsPushButtonOkWidget::button_press_ok_action( EmacsDialog *d )
	{
	if( d->inuse )
		{	
		d->incomplete = 0;
		d->result = CB_OK;
		d->inuse = 0;
		}
	else
		if( !d->apply_keys.isNull() )
			{
			int old_err = ml_err;

			for( EmacsWidget *p = d->children; p && !ml_err; p = p->next )
				p->store();

			theMotifGUI->input_char_string( d->apply_keys, false );
			ml_err = old_err;
			}

	XtUnmanageChild( d->widget );
	}

void EmacsPushButtonApplyWidget::button_press_apply_action( EmacsDialog *d )
	{
	if( d->inuse )
		{	
		d->incomplete = 0;
		d->result = CB_APPLY;
		return;
		}

	if( !d->apply_keys.isNull() )
		{
		int old_err = ml_err;
		EmacsWidget *p;

		theMotifGUI->input_char_string( d->apply_keys, false );
		for( p = d->children; p && !ml_err; p = p->next )
			p->store();
		ml_err = old_err;
		}
	}

void EmacsPushButtonCancelWidget::button_press_cancel_action( EmacsDialog *d )
	{
	if( d->inuse )
		{	
		d->incomplete = 0;
		d->result = CB_CANCEL;
		d->inuse = 0;
		}
	XtUnmanageChild( d->widget );
	}

void EmacsPushButtonUserWidget::button_press_action( EmacsDialog *d )
	{
	// do not distrub the 
	int old_err = ml_err;
	ml_err = 0;
	// save all the widget state
	for( EmacsWidget *p = d->children; p && !ml_err; p = p->next )
		p->store();
	// send the users keys
	theMotifGUI->input_char_string( push_action_keys, false );

	// restore the ml_err state
	ml_err = old_err;
	}

static void popdown_cb( Widget PNOTUSED(w ), void *d_, void *PNOTUSED(junk ) )
	{
	EmacsDialog *d = (EmacsDialog *)d_;
	if( d->inuse )
		{	
		d->incomplete = 0;
		d->result = CB_CANCEL;
		}
	}

// Create a named XmForm dialog shell
// (XmFormDialog name apply-keys width height <contents>*)
int motif_XmFormDialog( void )
	{
	EmacsDialog *w;
	ProgramNode *prg = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog != NULL )
		{
		error( FormatString( root_active ) << root_dialog->name );
		return 0;
		}
	if( check_args( 5, 1000000 ) )
		return 0;	

	if( !string_arg( 1 ) )
		return 0;
	if( find_dialog( ml_value.asString() ) )
		{
		error( FormatString( already_exists ) << ml_value.asString() );
		return 0;
		}

	int argc, width, height, x, y;
	Arg args[MAX_ARGS];

	if( (w = make_dialog( ml_value.asString() ) ) == NULL )
		return 0;
	w->type = DIALOG_FORM;
	if( !string_arg( 2 ) )
		{
		destroy_dialog( w->name );
		return 0;		
		}

	w->apply_keys = ml_value.asString();
	width = numeric_arg( 3 );
	height = numeric_arg( 4 );
	if( ml_err )
		return 0;

	Widget shell = theMotifGUI->application.shell->widget;
	x = shell->core.x;
	y = shell->core.y;
	x_pos = (x_pos + 1) & 7;
	y_pos = (y_pos + 1) & 3;
	XtSetArg( args[0], XmNx, x +( (x_pos + 1) * 10 ) ), argc = 1;
	XtSetArg( args[argc], XmNy, y +( (y_pos + 1) * 30 ) ), argc++;
	XtSetArg( args[argc], XmNwidth, width ), argc++;
	XtSetArg( args[argc], XmNheight, height ), argc++;		
	XtSetArg( args[argc], XmNautoUnmanage, 0 ), argc++;		
	XtSetArg( args[argc], XmNdefaultPosition, 0 ), argc++;		
	w->widget = XmCreateFormDialog( shell, w->name.sdataHack(), args, argc );
	if( w->widget == 0 )
		{
		error( FormatString( motif_problem ) << w->name );
		destroy_dialog( w->name );
		return 0;
		}

	w->dialog = XtParent( w->widget );

#if 0
	Widget b;
	int delta_l;

	XtAddCallback( w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w );
	XtSetArg( args[0], XmNtitle, w->name.sdata() ), argc = 1;
	XtSetArg( args[argc], XmNwindowGroup, XtUnspecifiedWindowGroup ), argc++;		
	XtSetValues( w->dialog, args, argc );

	// Create the OK button
	XtSetArg( args[0], XmNshowAsDefault, 1 );
	XtSetArg( args[1], XmNleftOffset, delta_l = 15 );
	XtSetArg( args[2], XmNbottomOffset, 12 );
	XtSetArg( args[3], XmNleftAttachment, XmATTACH_FORM );
	XtSetArg( args[4], XmNbottomAttachment, XmATTACH_FORM );
	XtSetArg( args[5], XmNwidth, 70 );

	b = XmCreatePushButtonGadget( w->widget, "OK", args, 6 );
	if( b == NULL )
		{
		error( FormatString( motif_problem ) << w->name );
		XtDestroyWidget( w->dialog );
		destroy_dialog( w->name );
		return 0;
		}

	XtManageChild( b );
	XtAddCallback( b, XmNactivateCallback, ok_cb, (XtPointer)w );
	XtSetArg( args[0], XmNdefaultButton, b );
	XtSetValues( w->widget, args, 1 );

	// Create the Apply button if required
	if( !w->apply_keys.isNull() )
		{
		XtSetArg( args[0], XmNleftOffset, delta_l = 110 );
		XtSetArg( args[1], XmNbottomOffset, 13 );
		XtSetArg( args[2], XmNleftAttachment, XmATTACH_FORM );
		XtSetArg( args[3], XmNbottomAttachment, XmATTACH_FORM );
		XtSetArg( args[4], XmNwidth, 70 );
		if( (b = XmCreatePushButtonGadget( w->widget, "Apply", args, 5 ) ) == NULL )
			{
			error( FormatString( motif_problem ) << w->name );
			XtDestroyWidget( w->dialog );
			destroy_dialog( w->name );
			return 0;
			}
		XtManageChild( b );
		XtAddCallback( b, XmNactivateCallback, apply_cb, (XtPointer)w );
		}

	// Create the Cancel button
	XtSetArg( args[0], XmNleftOffset, delta_l + 95 );
	XtSetArg( args[1], XmNbottomOffset, 13 );
	XtSetArg( args[2], XmNleftAttachment, XmATTACH_FORM );
	XtSetArg( args[3], XmNbottomAttachment, XmATTACH_FORM );
	XtSetArg( args[4], XmNwidth, 70 );

	b = XmCreatePushButtonGadget( w->widget, "Cancel", args, 5 );
	if( b == NULL )
		{
		error( FormatString( motif_problem ) << w->name );
		XtDestroyWidget( w->dialog );
		destroy_dialog( w->name );
		return 0;
		}
	XtManageChild( b );
	XtAddCallback( b, XmNactivateCallback, cancel_cb, (XtPointer)w );
#endif

	root = w->widget;
	root_dialog = w;
	exit_widget = NULL;
	for( int i = 5; i <= prg->p_nargs && !ml_err; i++ )
		eval_arg( i );

	if( exit_widget == NULL )
		error( FormatString( "At least one OK or CANCEL button is required in dialog %s" ) << w->name );

	root = NULL;
	root_dialog = NULL;

	if( ml_err )
		{
		XtDestroyWidget( w->dialog );
		destroy_dialog( w->name );
		return 0;
		}

	void_result();

	return 0;
	}

// Operate a dialog
int motif_XmOperateModalDialog( void )
	{
	EmacsWidget *p;

	if( !is_motif )
		return motif_not_running();

	EmacsString name = getstr( ": XmOperateModalDialog " );
	EmacsDialog *w = find_dialog( name );
	if( w == NULL )
		{
		error( FormatString( doesnt_exist ) << name );
		return 0;
		}

	if( w->inuse )
		{
		error( FormatString( inuse ) << name );
		return 0;
		}

	w->inuse = 1;
	for( p = w->children; p && !ml_err; p = p->next )
		p->fetch();
	if( !ml_err )
		{
		XtManageChild( w->widget );
		motif_insensitive = 1;
		for( w->incomplete = 1; w->incomplete; theActiveView->k_input_event( NULL, 0 ) )
			;
		motif_insensitive = 0;
		if( w->result != CB_CANCEL )
			for( p = w->children; p && !ml_err; p = p->next )
				p->store();
		}
	w->inuse = 0;
	void_result();
	ml_value = int( w->result );

	return 0;
	}

// Operate a dialog
int motif_XmOperateModelessDialog( void )
	{
	EmacsDialog *w;
	EmacsWidget *p;

	if( !is_motif )
		return motif_not_running();

	EmacsString name = getstr( ": XmOperateDialog " );
	if( !(w = find_dialog( name ) ) )
		{
		error( FormatString( doesnt_exist ) << name );
		return 0;
		}

	if( w->inuse )
		{
		error( FormatString( inuse ) << name );
		return 0;
		}

	for( p = w->children; p && !ml_err; p = p->next )
		p->fetch();
	if( !ml_err )
		XtManageChild( w->widget );
	void_result();
	ml_value = CB_CANCEL;

	return 0;
	}

// Destroy a dialog
int motif_XmDestroyDialog( void )
	{
	if( !is_motif )
		return motif_not_running();

	EmacsString name = getstr( ": XmDestroyDialog " );
	EmacsDialog *w;

	if( !(w = find_dialog( name ) ) )
		error( FormatString( doesnt_exist ) << name );
	else
		if( w->inuse )
			error( FormatString( inuse ) << name );
		else
			{
			XtDestroyWidget( w->dialog );
			destroy_dialog( name );
			void_result();
			}

	return 0;
	}

// list all dialogs
int motif_XmDumpDialogs( void )
	{
	EmacsDialog *p;
	EmacsBufferRef old( bf_cur );

	if( !is_motif )
		return motif_not_running();

	EmacsBuffer::scratch_bfn( "Dialog table", interactive() );
	bf_cur->ins_str( "Motif Dialogs\n"
			 "-------------\n\n" );
	for( p = dialogs; p; p = p->next )
		bf_cur->ins_cstr( FormatString("%s is a %s\n" ) << p->name << p->type );

	bf_cur->b_checkpointed = -1;
	bf_cur->b_modified = 0;
	set_dot( 1 );
	old.set_bf();
	theActiveView->window_on( bf_cur );

	return 0;
	}

// Create a label gadget
// (XmLabel x y label-text)
int motif_XmLabel( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a label" );
		return 0;
		}

	if( check_args( 3, 3 )
	|| !string_arg( 3 ) )
		return 0;

	XmString p = XmStringCreateLtoR( ml_value.asString().sdataHack(), charset );
	if( p == NULL )
		{
		error( FormatString( motif_problem ) << "label" );
		return 0;
		}

	int x, y;
	x = numeric_arg( 1 );
	if( !ml_err )
		 y = numeric_arg( 2 );
	if( ml_err )
		{
		XmStringFree( p );
		return 0;
		}

	Arg args[5];
	Widget b;
	int argc = 0;

	XtSetArg( args[argc], XmNlabelString, p ), argc++;
	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateLabelGadget( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XmStringFree( p );
		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "label" );
		XmStringFree( p );
		}

	return 0;
	}

// Fetch callback for toggle button
int EmacsToggleButtonWidget::fetch()
	{
	int value;
	if( fetch_var( var, &value, NULL ) )
		XmToggleButtonGadgetSetState( w, value != 0, 1 );

	return 0;
	}

// Store callback for toggle button
int EmacsToggleButtonWidget::store()
	{
	set_var( var, Expression( int(XmToggleButtonGadgetGetState( w )) ) );
	return 0;
	}

// Create a toggle button gadget
// (XmToggleButton x y label variable)
int motif_XmToggleButton( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a toggle button" );
		return 0;
		}

	if( check_args( 4, 4 ) )
		return 0;

	VariableName *var = cur_exec->arg(4)->name();
	if( var == NULL )
		{
		error( FormatString( must_be_var ) << "XmToggleButton" << 4 );
		return 0;
		}
	if( !string_arg( 3 ) )
		return 0;

	XmString p;

	if( (p = XmStringCreateLtoR( ml_value.asString().sdataHack(), charset ) ) == NULL )
		{
		error( FormatString( motif_problem ) << "toggle button" );
		return 0;
		}

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	if( ml_err )
		{
		XmStringFree( p );
		return 0;
		}

	EmacsToggleButtonWidget *w = new EmacsToggleButtonWidget( var->v_name );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "toggle button" );
		return 0;
		}

	Arg args[6];
	Widget b;
	int argc = 0;

	XtSetArg( args[argc], XmNlabelString, p ), argc++;
	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateToggleButtonGadget( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XmStringFree( p );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;
		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "toggle button" );
		XmStringFree( p );
		delete w;
		}

	return 0;
	}

// Create an empty radio box
// (XmRadioBox x y border-width <contents>)
int motif_XmRadioBox( void )
	{
	ProgramNode *p = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a radio box" );
		return 0;
		}

	if( check_args( 0, 10000 ) )
		return 0;

	int x = -1, y = -1, border = -1;
	switch( p->p_nargs )
		{
	case 3:	border = numeric_arg( 3 );
	case 2:	y = numeric_arg( 2 );
	case 1:	x = numeric_arg( 1 );
	default: ;
		}
	if( ml_err )
		return 0;

	Arg args[5];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( border >= 0 )
		XtSetArg( args[argc], XmNborderWidth, border > 0 ? 1 : 0 ), argc++;

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );

	b = XmCreateRadioBox( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		int i;
		Widget old_root = root;

		XtManageChild( b );

		root = b;
		for( i = 4; i <= p->p_nargs && !ml_err; i++ )
			eval_arg( i );
		root = old_root;
		void_result();
		ml_value = int( b );
		}
	else
		error( FormatString( motif_problem ) << "radio box" );

	return 0;
	}

// Create an empty check box
// (XmCheckBox x y border-width <contents>)
int motif_XmCheckBox( void )
	{
	ProgramNode *p = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a check box" );
		return 0;
		}

	if( check_args( 0, 10000 ) )
		return 0;

	int x = -1, y = -1, border = -1;
	switch( p->p_nargs )
		{
	case 3:	border = numeric_arg( 3 );
	case 2:	y = numeric_arg( 2 );
	case 1:	x = numeric_arg( 1 );
		}
	if( ml_err )
		return 0;

	Arg args[7];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( border >= 0 )
		XtSetArg( args[argc], XmNborderWidth, border > 0 ? 1 : 0 ), argc++;

	XtSetArg( args[argc], XmNentryClass, xmToggleButtonGadgetClass ), argc++;
	XtSetArg( args[argc], XmNisHomogeneous, 1 ), argc++;

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateRowColumn( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		int i;
		Widget old_root = root;

		XtManageChild( b );

		root = b;
		for( i = 4; i <= p->p_nargs && !ml_err; i++ )
			eval_arg( i );
		root = old_root;
		void_result();
		ml_value = int( b );
		}
	else
		error( FormatString( motif_problem ) << "check box" );

	return 0;
	}

// Fetch callback for scale
int EmacsScaleWidget::fetch()
	{
	int value;
	if( fetch_var( var, &value, NULL ) )
		XmScaleSetValue( w, value );

	return 0;
	}

// Store callback for scale
int EmacsScaleWidget::store()
	{
	int value;
	XmScaleGetValue( w, &value );
	set_var( var, Expression( value ) );
	return 0;
	}

// Create a scale Widget
// (XmScale x y title width height min max is_horz show variable)
int motif_XmScale( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a scale" );
		return 0;
		}
	if( check_args( 10, 10 ) )
		return 0;

	VariableName *var = cur_exec->arg(10)->name();
	if( var == NULL )
		{
		error( FormatString( must_be_var ) << "XmScale" << 10 );
		return 0;
		}
	if( !string_arg( 3 ) )
		return 0;

	XmString p;

	int titled = !ml_value.asString().isNull();
	if( titled && (p = XmStringCreateLtoR( ml_value.asString().sdataHack(), charset )) == NULL )
		{
		error( FormatString( motif_problem ) << "scale" );
		return 0;
		}

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	int width = numeric_arg( 4 );
	int height = numeric_arg( 5 );
	int mn = numeric_arg( 6 );
	int mx = numeric_arg( 7 );
	int is_horz = numeric_arg( 8 );
	int show = numeric_arg( 9 );
	if( ml_err )
		{
		if( titled ) XmStringFree( p );
		return 0;
		}

	EmacsScaleWidget *w = new EmacsScaleWidget( var->v_name );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "scale" );
		return 0;
		}

	Arg args[12];
	Widget b;
	int argc = 0;

	if( titled )
		XtSetArg( args[argc], XmNtitleString, p ), argc++;
	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNscaleWidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNscaleHeight, height ), argc++;
	if( mn >= 0 )
		XtSetArg( args[argc], XmNminimum, mn ), argc++;
	if( mx >= 0 )
		XtSetArg( args[argc], XmNmaximum, mx ), argc++;
	if( is_horz > 0 )
		XtSetArg( args[argc], XmNorientation, XmHORIZONTAL ), argc++;
	if( show >= 0 )
		XtSetArg( args[argc], XmNshowValue, show ), argc++;
	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateScale( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		if( titled ) XmStringFree( p );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;
		void_result();
		ml_value = int( b );
		}
	else
		{
		error( FormatString( motif_problem ) << "scale" );
		if( titled )
			XmStringFree( p );
		delete w;
		}

	return 0;
	}

// Fetch callback for text field
int EmacsTextFieldWidget::fetch()
	{
	EmacsString value;
	if( fetch_var( var, NULL, &value ) )
		XmTextFieldSetString( w, value.sdataHack() );

	return 0;
	}

// Store callback for text field
int EmacsTextFieldWidget::store()
	{
	char *value;
	value = XmTextFieldGetString( w );
	if( value )
		set_var( var, Expression( value ) );
	return 0;
	}

// Create a text field Widget
// (XmTextField x y columns resize variable)
int motif_XmTextField( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a text field" );
		return 0;
		}
	if( check_args( 5, 5 ) )
		return 0;

	VariableName *var = cur_exec->arg(5)->name();
	if( var == NULL )
		{
		error( FormatString( must_be_var ) << "XmTextField" << 5 );
		return 0;
		}

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	int columns = numeric_arg( 3 );
	int resize = numeric_arg( 4 );
	if( ml_err )
		return 0;


	EmacsTextFieldWidget *w = new EmacsTextFieldWidget( var->v_name );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "text field" );
		return 0;
		}

	Arg args[5];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( columns > 0 )
		XtSetArg( args[argc], XmNcolumns, columns ), argc++;
	if( resize > 0 )
		XtSetArg( args[argc], XmNresizeWidth, resize != 0 ), argc++;

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateTextField( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;
		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "text field" );
		delete w;
		}

	return 0;
	}

// Create a separator gadget
// (XmSeparator x y length is_horz separator-type)
int motif_XmSeparator( void )
	{
	ProgramNode *p = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a separator" );
		return 0;
		}

	if( check_args( 0, 5 ) )
		return 0;

	int x = -1, y = -1, len = -1, is_horz = 1, type = -1;
	switch( p->p_nargs )
		{
	case 5:	type = numeric_arg( 5 );
	case 4:	is_horz = numeric_arg( 4 );
	case 3:	len = numeric_arg( 3 );
	case 2:	y = numeric_arg( 2 );
	case 1:	x= numeric_arg( 1 );
		}
	if( ml_err )
		return 0;

	Arg args[9];
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( is_horz && len >= 0 )
		XtSetArg( args[argc], XmNwidth, len ), argc++;
	if( !is_horz )
		{
		if( len >= 0 )
			XtSetArg( args[argc], XmNheight, len ), argc++;
		XtSetArg( args[argc], XmNorientation, XmVERTICAL ), argc++;
		}
	if( type >= 0 )
		XtSetArg( args[argc], XmNseparatorType, type ), argc++;
		EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );

	Widget b = XmCreateSeparatorGadget( root, name.sdataHack(), args, argc );
	if( b == NULL )
		{
		error( FormatString( motif_problem ) << "separator" );
		return 0;
		}

	XtManageChild( b );
	void_result();
	ml_value = int( b );

	return 0;
	}

// Create an empty row/column
// (XmRowColumn x y width height border-width columns orientation pack)
int motif_XmRowColumn( void )
	{
	ProgramNode *p = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a row column" );
		return 0;
		}

	if( check_args( 8, 10000 ) )
		return 0;

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	int width = numeric_arg( 3 );
	int height = numeric_arg( 4 );
	int border = numeric_arg( 5 );
	int cols = numeric_arg( 6 );
	int orientation = numeric_arg( 7 );
	int pack = numeric_arg( 8 );
	if( ml_err )
		return 0;

	Arg args[10];
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNwidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNheight, height ), argc++;
	if( border >= 0 )
		XtSetArg( args[argc], XmNborderWidth, border > 0 ? 1 : 0 ), argc++;
	if( cols >= 0 )
		XtSetArg( args[argc], XmNnumColumns, cols ), argc++;
	if( orientation > 0 )
		XtSetArg( args[argc], XmNorientation, orientation ), argc++;
	if( pack >= 0 )
		XtSetArg( args[argc], XmNpacking, pack ), argc++;

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );

	Widget b = XmCreateRowColumn( root, name.sdataHack(), args, argc );
	if( b == NULL )
		{
		error( FormatString( motif_problem ) << "row column" );
		return 0;
		}

	Widget old_root = root;

	XtManageChild( b );

	root = b;
	for( int i = 9; i <= p->p_nargs && !ml_err; i++ )
		eval_arg( i );
	root = old_root;
	void_result();
	ml_value = int( b );

	return 0;
	}

// Create an empty row/column
// (XmFrame x y shadow-type <contents>)
int motif_XmFrame( void )
	{
	ProgramNode *p = cur_exec;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a frame" );
		return 0;
		}

	if( check_args( 3, 10000 ) )
		return 0;

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	int shadow = numeric_arg( 3 );
	if( ml_err )
		return 0;

	Arg args[6];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}
	if( shadow >= 0 )
		XtSetArg( args[argc], XmNshadowType, shadow ), argc++;

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	b = XmCreateFrame( root, name.sdataHack(), args, argc );
	if( b == NULL )
		{
		error( FormatString( motif_problem ) << "frame" );
		return 0;
		}

	Widget old_root = root;

	XtManageChild( b );

	root = b;
	for( int i = 4; i <= p->p_nargs && !ml_err; i++ )
		eval_arg( i );
	root = old_root;
	void_result();
	ml_value = int( b );

	return 0;
	}

// Fetch callback for updated label
int EmacsLabelWidget::fetch()
	{
	EmacsString value;
	if( !fetch_var( var, NULL, &value ) )
		return 0;

	XmString p;
	Arg args[1];

	if( (p = XmStringCreateLtoR( value.sdataHack(), charset ) ) == NULL )
		error( FormatString( motif_problem ) << "label" );
	else
		{
		XtSetArg( args[0], XmNlabelString, p );
		XtSetValues( w, args, 1 );
		XmStringFree( p );
		}

	return 0;
	}

// Create an updated  label gadget
// (XmUpdatedLabel x y variable)
int motif_XmUpdatedLabel( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a label" );
		return 0;
		}
	if( check_args( 3, 3 ) )
		return 0;

	VariableName *var = cur_exec->arg(3)->name();
	if( var == NULL )
		{
		error( FormatString( must_be_var ) << "XmUpdatedLabel" << 3 );
		return 0;
		}

	int x = numeric_arg( 1 );
	int y = numeric_arg( 2 );
	if( ml_err )
		return 0;

	Arg args[5];
	int argc = 0;

	EmacsLabelWidget *w = new EmacsLabelWidget( var->v_name );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "label" );
		return 0;
		}

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNtopOffset, y ), argc++;
		XtSetArg( args[argc], XmNtopAttachment, XmATTACH_FORM ), argc++;
		}

	EmacsString name( FormatString( "emacs_%d" ) << root_seq++ );
	Widget b = XmCreateLabelGadget( root, name.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;
		void_result();
		ml_value = int( b );
		}
	else
		error( FormatString( motif_problem ) << "label" );

	return 0;
	}

// Fetch callback for message box
int EmacsMessageBoxWidget::fetch()
	{
	EmacsString value;

	if( !fetch_var( var, NULL, &value ) )
		return 0;

	XmString p;

	if( (p = XmStringCreateLtoR( value.sdataHack(), charset ) ) == NULL )
		{
		error( FormatString( motif_problem ) << "message dialog" );
		return 0;
		}

	Arg args[1];

	XtSetArg( args[0], XmNmessageString, p );
	XtSetValues( w, args, 1 );
	XmStringFree( p );

	return 0;
	}

// Fetch callback for file selection box
int EmacsFileSelectionWidget::fetch()
	{
	EmacsString value;

	if( !fetch_var( var, NULL, &value ) )
		return 0;

	XmString p;

	if( (p = XmStringCreateLtoR( value.sdataHack(), charset ) ) == NULL )
		error( FormatString( motif_problem ) << "file selection dialog" );
	else
		{
		XmFileSelectionDoSearch( w, p );
		XmStringFree( p );
		}

	return 0;
	}

// Store callback for file selection box
int EmacsFileSelectionWidget::store()
	{
	char *value;
	XmString info = NULL;
	Arg args[1];
	  
	XtSetArg( args[0], XmNtextString, &info );
	XtGetValues( w, args, 1 );
	if( info != NULL )
		{
		XmStringGetLtoR( info, charset, &value );
		if( value )
			{
			set_var( store_var, Expression( value ) );
			XtFree( value );
			}
		}
	return 0;
	}

// Create a named XmFileSelectionBox dialog shell
// (XmFileSelectionDialog name apply-keys pattern-variable result-variable)
int motif_XmFileSelectionDialog( void )
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog != NULL )
		{
		error( FormatString( root_active ) << root_dialog->name );
		return 0;
		}
	if( check_args( 4, 4 ) )
		return 0;

	ProgramNode *pattern = cur_exec->arg(3);
	ProgramNode *result = cur_exec->arg(4);

	if( !string_arg( 1 ) )
		return 0;

	if( find_dialog( ml_value.asString() ) )
		{
		error( FormatString( already_exists ) << ml_value.asString() );
		return 0;
		}

	int argc, x, y;
	Arg args[MAX_ARGS];

	EmacsDialog *w = make_dialog( ml_value.asString() );
	if( w == NULL )
		return 0;

	w->type = DIALOG_FILE;
	void_result();
	if( !string_arg( 2 ) )
		{
		destroy_dialog( w->name );
		return 0;
		}

	w->apply_keys = ml_value.asString();

	XmString cancel = NULL, apply = NULL;

	Widget shell = theMotifGUI->application.shell->widget;
	x = shell->core.x;
	y = shell->core.y;
	x_pos = (x_pos + 1) & 7;
	y_pos = (y_pos + 1) & 3;
	XtSetArg( args[0], XmNx, x +( (x_pos + 1 ) * 10 ) ), argc = 1;
	XtSetArg( args[argc], XmNy, y +( (y_pos + 1 ) * 30 ) ), argc++;
	XtSetArg( args[argc], XmNautoUnmanage, 0 ), argc++;
	XtSetArg( args[argc], XmNdefaultPosition, 0 ), argc++;
	if( !w->apply_keys.isNull() )
		{
		cancel = XmStringCreateLtoR( "Cancel", charset );
		apply = XmStringCreateLtoR( "Apply", charset );
		}
	if( cancel != NULL && apply != NULL )
		{
		XtSetArg( args[argc], XmNcancelLabelString, apply ), argc++;
		XtSetArg( args[argc], XmNhelpLabelString, cancel ), argc++;
		}

	w->widget = XmCreateFileSelectionDialog( shell, w->name.sdataHack(), args, argc );
	if( w->widget == NULL )
		{
		error( FormatString( motif_problem ) << w->name );
		destroy_dialog( w->name );
		if( cancel ) XmStringFree( cancel );
		if( apply ) XmStringFree( apply );
		return 0;
		}

	w->dialog = XtParent( w->widget );
	XtAddCallback( w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w );
	XtAddCallback( w->widget, XmNokCallback, ok_cb, (XtPointer)w );
	if( cancel && apply )
		{
		XtAddCallback( w->widget, XmNcancelCallback, apply_cb, (XtPointer)w );
		XtAddCallback( w->widget, XmNhelpCallback, cancel_cb, (XtPointer)w );
		}
	else
		{
		Widget help;

		XtAddCallback( w->widget, XmNcancelCallback, cancel_cb, (XtPointer)w );
		if(( help = XmFileSelectionBoxGetChild( w->widget, XmDIALOG_HELP_BUTTON ) ) != NULL )
			XtDestroyWidget( help );
		}

	if( cancel )
		XmStringFree( cancel );
	if( apply )
		XmStringFree( apply );

	XtSetArg( args[0], XmNtitle, w->name.sdataHack() ), argc = 1;
	XtSetArg( args[argc], XmNwindowGroup, XtUnspecifiedWindowGroup ), argc++;		
	XtSetValues( w->dialog, args, argc );

	root = w->widget;
	root_dialog = w;

	VariableName *pattern_var = pattern->name();
	if( pattern_var == NULL )
		{
		error( FormatString( must_be_var ) << "XmFileSelectionDialog" << 2 );
		return 0;
		}

	VariableName *result_var = result->name();
	if( result_var == NULL )
		{
		error( FormatString( must_be_var ) << "XmFileSelectionDialog" << 3 );
		return 0;
		}

	EmacsFileSelectionWidget *fw = new EmacsFileSelectionWidget( pattern_var->v_name, result_var->v_name );
	if( fw == NULL )
		{
		error( FormatString( no_mem ) << "XmFileSelectionDialog" );
		return 0;
		}

	fw->next = root_dialog->children;
	fw->w = root;
	root_dialog->children = fw;

	root = NULL;
	root_dialog = NULL;

	if( ml_err )
		{
		XtDestroyWidget( w->dialog );
		destroy_dialog( w->name );
		return 0;
		}

	void_result();

	return 0;
	}

// Create a named XmMessage dialog shell
// (XmMessageDialog name dialog-type message-variable] [ok-text] [cancel-text] )
int motif_XmMessageDialog( void )
	{
	ProgramNode *mesg;
	int x = -1, y = -1;

	if( !is_motif )
		return motif_not_running();

	if( root_dialog != NULL )
		{
		error( FormatString( root_active ) << root_dialog->name );
		return 0;
		}

	if( check_args( 3, 5 ) )
		return 0;

	mesg = cur_exec->arg(3);

	EmacsString name;
	EmacsString ok_text("OK");
	EmacsString cancel_text("Cancel");
	
	if( !string_arg( 1 ) )
		return 0;
	name = ml_value.asString();

	switch( cur_exec->p_nargs )
		{
	case 5:	if( !string_arg( 5 ) )
			return 0;
		cancel_text = ml_value.asString();
	case 4:	if( !string_arg( 4 ) )
			return 0;
		ok_text = ml_value.asString();
	default:
		break;
		}
	if( find_dialog( name ) )
		{
		error( FormatString( already_exists ) << ml_value.asString() );
		return 0;
		}

	EmacsDialog *w = make_dialog( name );
	if( w == NULL )
		return 0;
	w->type = DIALOG_MESSAGE;
	void_result();
	int type = numeric_arg( 2 );
	if( ml_err )
		{
		destroy_dialog( w->name );
		return 0;
		}

	Widget shell = theMotifGUI->application.shell->widget;
	x = shell->core.x;
	y = shell->core.y;

	EmacsXtArgs args;
	args.setArg( XmNx, x + 100 );
	args.setArg( XmNy, y + 100 );
	args.setArg( XmNautoUnmanage, 0l );
	args.setArg( XmNdefaultPosition, 0l );
	if( type >= 0 )
		args.setArg( XmNdialogType, type );
	w->widget = XmCreateMessageDialog( shell, w->name.sdataHack(), args, args );
	if( w->widget == NULL )
		{
		error( FormatString( motif_problem ) << w->name );
		destroy_dialog( w->name );
		return 0;
		}

	w->dialog = XtParent( w->widget );
	XtAddCallback( w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w );
	XtAddCallback( w->widget, XmNokCallback, ok_cb, (XtPointer)w );

	Widget button = XmMessageBoxGetChild( w->widget, XmDIALOG_HELP_BUTTON );
	if( button != NULL )
		XtDestroyWidget( button );
	button = XmMessageBoxGetChild( w->widget, XmDIALOG_CANCEL_BUTTON );
	if( button != NULL )
		if( cancel_text.isNull() )
			XtDestroyWidget( button );
		else
			{
			XmString p = XmStringCreateLtoR( cancel_text.sdataHack(), charset );
			XtSetArg( args[0], XmNlabelString, p );
			XtSetValues( button, args, 1 );
			XmStringFree( p );
			XtAddCallback( w->widget, XmNcancelCallback, cancel_cb, (XtPointer)w );
			}

	button = XmMessageBoxGetChild( w->widget, XmDIALOG_OK_BUTTON );
	if( button != NULL && !ok_text.isNull() )
		{
		XmString p = XmStringCreateLtoR( ok_text.sdataHack(), charset );
		XtSetArg( args[0], XmNlabelString, p );
		XtSetValues( button, args, 1 );
		XmStringFree( p );
		}

	args.setArg( XmNtitle, w->name.sdataHack() );
	args.setArg( XmNwindowGroup, XtUnspecifiedWindowGroup );
	args.setValues( w->dialog );
	root = w->widget;
	root_dialog = w;

	VariableName *var = mesg->name();
	if( var == NULL )
		{
		error( FormatString( must_be_var ) << "XmMessageDialog" << 2 );
		return 0;
		}

	EmacsMessageBoxWidget *mw = new EmacsMessageBoxWidget( var->v_name );
	if( mw == NULL )
		{
		error( FormatString( no_mem ) << "XmMessageDialog" );
		return 0;
		}

	mw->next = root_dialog->children;
	mw->w = root;
	root_dialog->children = mw;

	root = NULL;
	root_dialog = NULL;
	if( ml_err )
		{
		XtDestroyWidget( w->dialog );
		destroy_dialog( w->name );
		}
	else
		void_result();

	return 0;
	}

// Return indication of existance of a dialog shell
int motif_XmIsDialog( void )
	{
	if( !is_motif )
		return motif_not_running();

	EmacsString name = getstr( ": XmIsDialog " );
	EmacsDialog *w = find_dialog( name );
	void_result();
	ml_value = int( w != NULL );

	return 0;
	}

//
//	(XmPushButton name keys x y width height)
//
int motif_XmPushButtonUser(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "a push button" );
		return 0;
		}

	if( check_args( 2, 6 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	EmacsString button_label = ml_value.asString();

	if( !string_arg( 2 ) )
		return 0;

	EmacsString action_keys = ml_value.asString();

	int x = -1, y = -1, width = -1, height = -1;

	switch( cur_exec->p_nargs )
		{
	case 6:	height = numeric_arg( 6 );
	case 5:	width = numeric_arg( 5 );
	case 4:	y = numeric_arg( 4 );
	case 3:	x = numeric_arg( 3 );
	default:
		break;
		}
	if( ml_err )
		return 0;

	EmacsPushButtonUserWidget *w = new EmacsPushButtonUserWidget( action_keys );
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "XmPushButton" );
		return 0;
		}

	Arg args[6];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNbottomOffset, y ), argc++;
		XtSetArg( args[argc], XmNbottomAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNwidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNheight, height ), argc++;

	b = XmCreatePushButtonGadget( root, button_label.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XtAddCallback( b, XmNactivateCallback, EmacsPushButtonWidget::button_press_cb, (XtPointer)root_dialog );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;
		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "toggle button" );
		delete w;
		}

	return 0;
	}

int motif_XmPushButtonOK(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "XmPushButtonOK" );
		return 0;
		}

	if( check_args( 1, 5 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	EmacsString button_label = ml_value.asString();

	int x = -1, y = -1, width = -1, height = -1;

	switch( cur_exec->p_nargs )
		{
	case 5:	height = numeric_arg( 5 );
	case 4:	width = numeric_arg( 4 );
	case 3:	y = numeric_arg( 3 );
	case 2:	x = numeric_arg( 2 );
	default:
		break;
		}
	if( ml_err )
		return 0;

	EmacsPushButtonOkWidget *w = new EmacsPushButtonOkWidget();
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "XmPushButtonOK" );
		return 0;
		}

	Arg args[6];
	Widget b;
	int argc = 1;

	XtSetArg( args[0], XmNshowAsDefault, 1 );
	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNbottomOffset, y ), argc++;
		XtSetArg( args[argc], XmNbottomAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNwidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNheight, height ), argc++;

	b = XmCreatePushButtonGadget( root, button_label.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XtAddCallback( b, XmNactivateCallback, EmacsPushButtonWidget::button_press_cb, (XtPointer)root_dialog );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;

		XtSetArg( args[0], XmNdefaultButton, b );
		XtSetValues( root_dialog->widget, args, 1 );

		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "XmPushButtonOK" );
		delete w;
		}

	return 0;
	}

int motif_XmPushButtonCancel(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "XmPushButtonCancel" );
		return 0;
		}

	if( check_args( 1, 5 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	EmacsString button_label = ml_value.asString();

	int x = -1, y = -1, width = -1, height = -1;

	switch( cur_exec->p_nargs )
		{
	case 5:	height = numeric_arg( 5 );
	case 4:	width = numeric_arg( 4 );
	case 3:	y = numeric_arg( 3 );
	case 2:	x = numeric_arg( 2 );
	default:
		break;
		}
	if( ml_err )
		return 0;

	EmacsPushButtonCancelWidget *w = new EmacsPushButtonCancelWidget();
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "XmPushButtonCancel" );
		return 0;
		}

	Arg args[6];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNbottomOffset, y ), argc++;
		XtSetArg( args[argc], XmNbottomAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNwidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNheight, height ), argc++;

	b = XmCreatePushButtonGadget( root, button_label.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XtAddCallback( b, XmNactivateCallback, EmacsPushButtonWidget::button_press_cb, (XtPointer)root_dialog );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;

		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "XmPushButtonCancel" );
		delete w;
		}

	return 0;
	}

int motif_XmPushButtonApply(void)
	{
	if( !is_motif )
		return motif_not_running();

	if( root_dialog == NULL )
		{
		error( FormatString( root_inactive ) << "XmPushButtonApply" );
		return 0;
		}

	if( check_args( 1, 5 ) )
		return 0;

	if( !string_arg( 1 ) )
		return 0;

	EmacsString button_label = ml_value.asString();

	int x = -1, y = -1, width = -1, height = -1;

	switch( cur_exec->p_nargs )
		{
	case 5:	height = numeric_arg( 5 );
	case 4:	width = numeric_arg( 4 );
	case 3:	y = numeric_arg( 3 );
	case 2:	x = numeric_arg( 2 );
	default:
		break;
		}
	if( ml_err )
		return 0;

	EmacsPushButtonApplyWidget *w = new EmacsPushButtonApplyWidget();
	if( w == NULL )
		{
		error( FormatString( no_mem ) << "XmPushButtonApply" );
		return 0;
		}

	Arg args[6];
	Widget b;
	int argc = 0;

	if( x >= 0 )
		{
		XtSetArg( args[argc], XmNleftOffset, x ), argc++;
		XtSetArg( args[argc], XmNleftAttachment, XmATTACH_FORM ), argc++;
		}
	if( y >= 0 )
		{
		XtSetArg( args[argc], XmNbottomOffset, y ), argc++;
		XtSetArg( args[argc], XmNbottomAttachment, XmATTACH_FORM ), argc++;
		}
	if( width >= 0 )
		XtSetArg( args[argc], XmNwidth, width ), argc++;
	if( height >= 0 )
		XtSetArg( args[argc], XmNheight, height ), argc++;

	b = XmCreatePushButtonGadget( root, button_label.sdataHack(), args, argc );
	if( b != NULL )
		{
		XtManageChild( b );
		XtAddCallback( b, XmNactivateCallback, EmacsPushButtonWidget::button_press_cb, (XtPointer)root_dialog );
		w->next = root_dialog->children;
		w->w = b;
		root_dialog->children = w;

		void_result();
		ml_value = int (b);
		}
	else
		{
		error( FormatString( motif_problem ) << "XmPushButtonApply" );
		delete w;
		}

	return 0;
	}

void EmacsPushButtonWidget::button_press_cb( Widget w, void *d_, void *PNOTUSED(junk) )
	{
	EmacsDialog *d = (EmacsDialog *)d_;

	int old_err = ml_err;
	EmacsWidget *p;
	EmacsPushButtonWidget *button = NULL;

	// save all the widget data
	// and find this widget
	for( p = d->children; p && !ml_err; p = p->next )
		{
		p->store();
		if( p->w == w )
			button = (EmacsPushButtonWidget *)p;
		}

	button->button_press_action( d );
	ml_err = old_err;
	}
#endif
