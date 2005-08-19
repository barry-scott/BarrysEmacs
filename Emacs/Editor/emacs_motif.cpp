//
//	emacs_motif.cpp
//
//	Copyright 1997-2001 (c) Barry A. Scott
//

#include <emacs.h>

#ifdef XWINDOWS
#include <emacs_motif.h>
#include <Xm/ToggleB.h>
#include <Xm/AtomMgr.h>
#include <Xm/Protocols.h>

#include <xwin_icn.h>

extern int vertical_bar_width;


//--------------------------------------------------------------//
//								//
//   EmacsXtArgs implementation					//
//   Friendly way to handle the Args to Xt intensic functions	//
//								//
//--------------------------------------------------------------//
EmacsXtArgs::EmacsXtArgs()
	: num_used(0)
	{ }

EmacsXtArgs::~EmacsXtArgs()
	{ }

void EmacsXtArgs::init()
	{ num_used = 0; }

void EmacsXtArgs::setArg( String name, XtArgVal value )
	{
	emacs_assert( num_used < max_args );
	XtSetArg( args[num_used], name, value );
	num_used++;
	}		

void EmacsXtArgs::setArg( String name, Widget value )
	{
	emacs_assert( num_used < max_args );
	XtSetArg( args[num_used], name, value );
	num_used++;
	}		

void EmacsXtArgs::setArg( String name, XtPointer value )
	{
	emacs_assert( num_used < max_args );
	XtSetArg( args[num_used], name, value );
	num_used++;
	}		

EmacsXtArgs::operator Arg *() { return args; }
EmacsXtArgs::operator int()
	{
	int num = num_used;
	init();
	return num;
	}

void EmacsXtArgs::setValues( Widget w )
	{
	XtSetValues( w, args, num_used );
	init();
	}

void EmacsXtArgs::getValues( Widget w )
	{
	XtGetValues( w, args, num_used );
	init();
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_Core implementation			//
//							//
//------------------------------------------------------//
// create a widget
EmacsMotif_Core::EmacsMotif_Core( EmacsMotif_Application &app, EmacsMotif_Core &_parent, const EmacsString &name, WidgetClass widget_class )
	: application( app )
	, widget( NULL )
	, parent( &_parent )
	, widget_type( widget_class->core_class.class_name )
	, widget_name( name )
	{
	widget = XtCreateWidget( name, widget_class, parent->widget, NULL, 0 );

	// init the Core
	initCore();
	}

// take control of a already existing widget
EmacsMotif_Core::EmacsMotif_Core( EmacsMotif_Application &app, Widget w )
	: application( app )
	, widget( w )
	, parent( NULL )
	, widget_type( w->core.widget_class->core_class.class_name )
	, widget_name( w->core.name )
	{
	initCore();
	}

EmacsMotif_Core::~EmacsMotif_Core()
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("~EmacsMotif_Core");
#endif
	XtDestroyWidget( widget );
	}

// setup the object's widget for use by this class
void EmacsMotif_Core::initCore()
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("setValues( XmNuserData, 0x%X ) <= 0x%X name=%s") << int(widget) << int(this) << widget_name );
#endif
	EmacsXtArgs args;
	args.setArg( XmNuserData, (XtPointer )this );
	args.setValues( widget );
	
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		{
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( widget );
		_dbg_msg( FormatString("getValues( XmNuserData, 0x%X ) => 0x%X )") << int(widget) << int(core_object) );
		}
#endif
	}

void EmacsMotif_Core::manageWidget()
	{
	XtManageChild( widget );
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("manageWidget");
#endif
	}
void EmacsMotif_Core::unmanageWidget()
	{
	XtUnmanageChild( widget );
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget("unmanageWidget");
#endif
	}

//
//	Will find the first widget with the given name.
//	Attempting to delete two widgets at a time leads to a SEGV.
//	returns true if any where deleted.
//
bool EmacsMotif_Core::destroyChildWidget( const EmacsString &name )
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			if( core_object->widget_name == name )
				{
				delete core_object;
				return true;
				}
			}
		}

	return false;
	}

// call all the preRealizeInit() function from this core class down
void EmacsMotif_Core::callPreRealizeInit()
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->callPreRealizeInit();
			}
		}

	// call preRealizeInit on this widget
#if DBG_DISPLAY
	if( dbg_flags&DBG_DISPLAY )
		_dbg_msg( FormatString("calling preRealizeInit for %s %s")
			<< widget_type << widget_name );
#endif
	preRealizeInit();
	}

// call all the postRealizeInit() function from this core class down
void EmacsMotif_Core::callPostRealizeInit()
	{
	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->callPostRealizeInit();
			}
		}

	// call preRealizeInit on this widget
#if DBG_DISPLAY
	if( dbg_flags&DBG_DISPLAY )
		_dbg_msg( FormatString("calling postRealizeInit for %s %s")
			<< widget_type << widget_name );
#endif
	postRealizeInit();
	}

// call debugWidget for all widgets in the tree
void EmacsMotif_Core::debugPrintWidgetTree( const char *label, int level )
	{
	EmacsString label_and_level( FormatString("%*s %s") << level*2 << "" << label );
	debugWidget( label_and_level.sdata() );

	// find all the children of this widget and recursively call them
	int num_children = 0;
	WidgetList children;

	EmacsXtArgs args;
	args.setArg( XmNnumChildren, &num_children );
	args.setArg( XmNchildren, &children );
	args.getValues( widget );

	for( int child=0; child<num_children; child++ )
		{
		Widget w = children[child];
		EmacsMotif_Core *core_object = NULL;
		args.setArg( XmNuserData, (XtPointer)&core_object );

#if DBG_DISPLAY
		if( dbg_flags & DBG_DISPLAY )
			{
			_dbg_msg( FormatString("debugPrintWidgetTree: getValues( 0x%X ) child %d") << int(w) << child );
			if( reinterpret_cast<unsigned int>( w->core.name ) > 0x1000 )
				_dbg_msg( FormatString("debugPrintWidgetTree: name=%s") << w->core.name );
			}
#endif
		args.getValues( w );

		if( core_object != NULL )
			{
			emacs_assert( core_object->widget == w );
			core_object->debugPrintWidgetTree( label, level+1 );
			}
		}
	}

// install a handler on this object
void EmacsMotif_Core::install_handler
	(
	EmacsMotifCallback callback,
	XtPointer client_data
	)
	{
	switch( callback )
		{
	case EmacsMotifCallback_Expose:
		XtAddCallback( widget, XmNexposeCallback, expose_handler, client_data );
		break;

	case EmacsMotifCallback_Resize:
		XtAddCallback( widget, XmNresizeCallback, resize_handler, client_data );
		break;

	case EmacsMotifCallback_Input:
#if 0
		// new motif works
		XtAddCallback( widget, XmNinputCallback, input_handler, client_data );
		break;
#else
		// old motif need this
		{
		XtActionsRec action;
		action.string = "emacs_input_handler";
		action.proc = input_handler;

		XtAppAddActions( application.app_context, &action, 1 );

		XtTranslations trans = XtParseTranslationTable
			(
			"<Key>:		emacs_input_handler()"	"\n"
			"<Motion>:	emacs_input_handler()"	"\n"
			"<BtnDown>:	emacs_input_handler()"	"\n"
			"<BtnUp>:	emacs_input_handler()"
			);
		XtOverrideTranslations( widget, trans );
		}
		break;
#endif
	case EmacsMotifCallback_ValueChangedButton:
		// toggle button value changed uses the activate semantics
		XtAddCallback( widget, XmNvalueChangedCallback, activate_handler, client_data );
		break;

	case EmacsMotifCallback_ValueChangedScale:
		XtAddCallback( widget, XmNvalueChangedCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNincrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNdecrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNpageDecrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNpageIncrementCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNtoTopCallback, value_changed_handler, client_data );
		XtAddCallback( widget, XmNtoBottomCallback, value_changed_handler, client_data );
		break;

	case EmacsMotifCallback_Drag:
		XtAddCallback( widget, XmNdragCallback, drag_handler, client_data ); break;

	case EmacsMotifCallback_Activate:
		XtAddCallback( widget, XmNactivateCallback, activate_handler, client_data ); break;

	case EmacsMotifCallback_WmProtocol:
		{
		Atom WM_DELETE_WINDOW = XmInternAtom( XtDisplay( widget ), "WM_DELETE_WINDOW", False );
		XmAddWMProtocolCallback( XtParent( widget ), WM_DELETE_WINDOW, wm_protocol_handler, widget ); break;
		}
	default:
		emacs_assert( false );
		}
	}

#if DBG_DISPLAY
void EmacsMotif_Core::debugWidget( const char *label )
	{
	_dbg_msg( FormatString( "%s %s(%s) M:%d R:%d x:%d y:%d w:%d h:%d")
		<< label
		<< widget_type << widget_name << XtIsManaged( widget ) << (XtIsRealized( widget ) != 0)
		<< widget->core.x << widget->core.y
		<< widget->core.width << widget->core.height );
	}
#endif

void EmacsMotif_Core::expose( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "expose" );
#endif
	}

void EmacsMotif_Core::resize( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "resize" );
#endif
	}

void EmacsMotif_Core::input( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "input" );
#endif
	}

void EmacsMotif_Core::valueChanged( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "valueChanged" );
#endif
	}

void EmacsMotif_Core::drag( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "drag" );
#endif
	}

void EmacsMotif_Core::activate( XtPointer , XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "activate" );
#endif
	}

void EmacsMotif_Core::wm_protocol( XtPointer  )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "wm_protocol" );
#endif
	}


void EmacsMotif_Core::expose_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->expose( client_data, call_data );
	}

void EmacsMotif_Core::resize_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->resize( client_data, call_data );
	}

#if 0
void EmacsMotif_Core::input_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->input( client_data, call_data );
	}
#else
void EmacsMotif_Core::input_handler( Widget w, XEvent *e, String *, Cardinal * )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	XmDrawingAreaCallbackStruct call_data;
	call_data.reason = XmCR_INPUT;
	call_data.event = e;
	call_data.window = XtWindow( w );

	_this->input( NULL, &call_data );
	}
#endif
void EmacsMotif_Core::value_changed_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->valueChanged( client_data, call_data );
	}

void EmacsMotif_Core::drag_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->drag( client_data, call_data );
	}

void EmacsMotif_Core::activate_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( w );

	emacs_assert( _this->widget == w );

	_this->activate( client_data, call_data );
	}

void EmacsMotif_Core::wm_protocol_handler( Widget w, XtPointer client_data, XtPointer call_data )
	{
	EmacsXtArgs args;
	EmacsMotif_Core *_this = NULL;
	args.setArg( XmNuserData, (XtPointer )&_this );
	args.getValues( (Widget)client_data );

	emacs_assert( XtParent( _this->widget ) == w );

	_this->wm_protocol( call_data );
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_Frame implementation			//
//							//
//------------------------------------------------------//
EmacsMotif_Frame::EmacsMotif_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmFrameWidgetClass )
	{ }

EmacsMotif_Frame::~EmacsMotif_Frame()
	{ }

void EmacsMotif_Frame::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNshadowType, XmSHADOW_IN );
	args.setArg( XmNborderWidth, 1 );

	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_Frame::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_DrawingArea implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_DrawingArea::EmacsMotif_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmDrawingAreaWidgetClass )
	{
	// make sure that we do not get swamped under motion events
	widget->core.widget_class->core_class.compress_motion = true;
	}

EmacsMotif_DrawingArea::~EmacsMotif_DrawingArea()
	{ }

void EmacsMotif_DrawingArea::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNborderWidth, 1 );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_DrawingArea::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_TextLabel implementation		//
//							//
//------------------------------------------------------//

EmacsMotif_TextLabel::EmacsMotif_TextLabel( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int _length )
	: EmacsMotif_DrawingArea( app, parent, name )
	, text()
	, length( _length )
	{ }

EmacsMotif_TextLabel::~EmacsMotif_TextLabel()
	{ }

void EmacsMotif_TextLabel::preRealizeInit()
	{
	install_handler( EmacsMotifCallback_Expose );
	install_handler( EmacsMotifCallback_Resize );

	EmacsXtArgs args;
	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
	args.setArg( XmNheight, (application.fth+4) );
	if( length != 0 )
		args.setArg( XmNwidth, (application.ftw*length) + 2 );	// optional width
	args.setValues( widget );
	XtManageChild( widget );

	manageWidget();
	}

void EmacsMotif_TextLabel::postRealizeInit()
	{
	}

void EmacsMotif_TextLabel::update( const EmacsString &new_text )
	{
	if( text != new_text )
		{
		text = new_text;
		draw();
		}
	}

void EmacsMotif_TextLabel::draw()
	{
	if( !XtIsRealized( widget ) )
		return;

	if( attr.getNormalGc() == NULL )
		{
		Pixel fg, bg;
		EmacsXtArgs args;

		// draw in the colours of the widget
		args.setArg( XmNforeground, &fg );
		args.setArg( XmNbackground, &bg );
		args.getValues( widget );
		attr.create( application, fg, bg, "no", XtWindow( widget ) );
		}

	XDrawImageString
		(
		application.dpy, XtWindow( widget ), attr.getNormalGc(),
		1, application.fth - application.ftd,
		text.sdata(), text.length()
		);
	}

void EmacsMotif_TextLabel::expose( XtPointer , XtPointer event_ )
	{
	XmDrawingAreaCallbackStruct *event = (XmDrawingAreaCallbackStruct *)event_;

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "TextLabel expose" );
#endif

	switch( event->event->type )
		{
	case NoExpose:
		break;

	case GraphicsExpose:
	case Expose:
		// draw status text
		// _dbg_msg("status exposed...\n");
		draw();
		break;

	default:
		_dbg_msg( FormatString( "Unknown exposure event = %d" ) << event->event->type );
		break;
		}
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_TextLabelFrame implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_TextLabelFrame::EmacsMotif_TextLabelFrame( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_Core( app, parent, name, xmFrameWidgetClass )
	{ }

EmacsMotif_TextLabelFrame::~EmacsMotif_TextLabelFrame()
	{ }

void EmacsMotif_TextLabelFrame::preRealizeInit()
	{
	EmacsXtArgs args;
	args.setArg( XmNshadowType, XmSHADOW_IN );
	args.setArg( XmNborderWidth, 0l );

	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_TextLabelFrame::postRealizeInit()
	{ }

//------------------------------------------------------//
//							//
//   EmacsMotif_ScrollBar implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_ScrollBar::EmacsMotif_ScrollBar( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name, int orientation )
	: EmacsMotif_Core( app, parent, name, xmScrollBarWidgetClass )
	{
	EmacsXtArgs args;

	args.setArg( XmNorientation, orientation );
	args.setValues( widget );
	}

EmacsMotif_ScrollBar::~EmacsMotif_ScrollBar()
	{ }

void EmacsMotif_ScrollBar::preRealizeInit()
	{
	install_handler( EmacsMotifCallback_ValueChangedScale );
	install_handler( EmacsMotifCallback_Drag );

	manageWidget();
	}

void EmacsMotif_ScrollBar::postRealizeInit()
	{
	}

void EmacsMotif_ScrollBar::position( int x, int y, int width, int height )
	{
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBar::position X:%d Y:%d W:%d H:%d") << x << y << width << height );
#endif
	EmacsXtArgs args;
	args.setArg( XmNx, x );
	args.setArg( XmNy, y );
	args.setArg( XmNwidth, width );
	args.setArg( XmNheight, height );
	args.setValues( widget );

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		debugWidget( "ScrollBar::position" );
#endif
	}

EmacsMotif_ScrollBarVertical::EmacsMotif_ScrollBarVertical( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_ScrollBar( app, parent, name, XmVERTICAL )
	{ }

EmacsMotif_ScrollBarVertical::~EmacsMotif_ScrollBarVertical()
	{ }

void EmacsMotif_ScrollBarVertical::preRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::preRealizeInit();

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 11 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, 1 );
	args.setArg( XmNpageIncrement, 8 );	// should be 80% window size
	args.setValues( widget );
	}

void EmacsMotif_ScrollBarVertical::postRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::postRealizeInit();
	}

EmacsMotif_ScrollBarHorizontal::EmacsMotif_ScrollBarHorizontal( EmacsMotif_Application &app, EmacsMotif_Core &parent, const EmacsString &name )
	: EmacsMotif_ScrollBar( app, parent, name, XmHORIZONTAL )
	{ }

EmacsMotif_ScrollBarHorizontal::~EmacsMotif_ScrollBarHorizontal()
	{ }

void EmacsMotif_ScrollBarHorizontal::preRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::preRealizeInit();

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 256+1+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, 1 );
	args.setArg( XmNpageIncrement, 4 );	// should be tab-size
	args.setValues( widget );
	}

void EmacsMotif_ScrollBarHorizontal::postRealizeInit()
	{
	// call base class init
	EmacsMotif_ScrollBar::postRealizeInit();
	}

	// for ScrollBar
void EmacsMotif_ScrollBarVertical::valueChanged( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarVert::valueChanged R:%d V:%d") << cbs->reason << cbs->value );
#endif

	int scroll = 0;

	EmacsWindow *old_window = theActiveView->windows.currentWindow();
	win->set_win();

	switch( cbs->reason )
		{
	case XmCR_INCREMENT:
		scroll += 1; break;

	case XmCR_DECREMENT:
		scroll -= 2; break;

	case XmCR_PAGE_INCREMENT:
		scroll += win->w_height * 4 / 5; break;

	case XmCR_PAGE_DECREMENT:
		scroll -= win->w_height * 4 / 5; break;

	case XmCR_TO_TOP:
		set_dot( 1 ); break;

	case XmCR_TO_BOTTOM:
		set_dot( bf_cur->num_characters() ); break;

	case XmCR_VALUE_CHANGED:
	case XmCR_DRAG:
		{
		int n = cbs->value;
		if( n < 1 )
			n = 1;
		if( n > bf_cur->num_characters() )
			n = bf_cur->num_characters() + 1;
		if( n != (bf_cur->num_characters() + 1) )
			set_dot( scan_bf('\n', n, -1) );
		}
		break;

		}

	if( scroll )
		{
		set_dot( scan_bf('\n', dot, scroll) );
		win->setWindowStart( scan_bf('\n', win->getWindowStart(), scroll) );
		}

	old_window->set_win();

	int old_scroll_step = scroll_step;
	scroll_step = 1;
		theActiveView->windows.do_dsp();
	scroll_step = old_scroll_step;
	}

void EmacsMotif_ScrollBarVertical::drag( XtPointer client_data, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarVert::drag R:%d V:%d") << cbs->reason << cbs->value );
#endif
	valueChanged( client_data, call_data );
	}

void EmacsMotif_ScrollBarVertical::setValue( int value, int max )
	{
	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, 1+max+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, value );
	args.setValues( widget );
	}


	// for ScrollBar
void EmacsMotif_ScrollBarHorizontal::valueChanged( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarHoriz::valueChanged R:%d V:%d") << cbs->reason << cbs->value );
#endif
	if( win == NULL )
		return;

	win->w_horizontal_scroll = cbs->value;

	cant_1line_opt = 1;
	redo_modes = 1;

	theActiveView->windows.do_dsp();
	}

void EmacsMotif_ScrollBarHorizontal::drag( XtPointer, XtPointer call_data )
	{
	XmScrollBarCallbackStruct *cbs = (XmScrollBarCallbackStruct *)call_data;
#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		_dbg_msg( FormatString("ScrollBarHoriz::drag R:%d V:%d") << cbs->reason << cbs->value );
#endif
	if( win == NULL )
		return;

	win->w_horizontal_scroll = cbs->value;

	cant_1line_opt = 1;
	redo_modes = 1;

	theActiveView->windows.do_dsp();
	}

void EmacsMotif_ScrollBarHorizontal::setValue( int value )
	{
	int max=256;

	while( max < value )
		max *= 2;

	EmacsXtArgs args;

	args.setArg( XmNminimum, 1 );
	args.setArg( XmNmaximum, max+10 );
	args.setArg( XmNsliderSize, 10 );
	args.setArg( XmNvalue, value );
	args.setValues( widget );
	}


//------------------------------------------------------//
//							//
//   EmacsMotif_Attributes implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_Attributes::EmacsMotif_Attributes()
	: gc(NULL)
	, special_gc(NULL)
	, underline(0)
	{ }

EmacsMotif_Attributes::~EmacsMotif_Attributes()
	{ }

GC EmacsMotif_Attributes::getNormalGc() const
	{ return gc; }

GC EmacsMotif_Attributes::getSpecialGc() const
	{ return special_gc; }

int EmacsMotif_Attributes::getUnderline() const
	{ return underline; }

void EmacsMotif_Attributes::create( EmacsMotif_Application &app, Pixel fg, Pixel bg, char *underline_text, Window win )
	{
	XGCValues gcv;
	memset( &gcv, 0, sizeof( gcv ) );

	gcv.fill_style = FillSolid;
	gcv.plane_mask = AllPlanes;
	gcv.line_style = LineSolid;
	gcv.foreground =  fg;
	gcv.background =  bg;

	long int bits = GCFont | GCForeground | GCBackground | GCFillStyle| GCPlaneMask;

	if( underline_text != NULL  )
		{
		int num_parsed;
		char keyword[80];
		int dash_value;

		keyword[0] = '\0';
		num_parsed = sscanf( underline_text,"%s %d", keyword, &dash_value  );

		if( strcmp("1",keyword ) == 0
		|| strcmp("true",keyword ) == 0
		|| strcmp("yes",keyword ) == 0  )
			{
			underline = 1;
			}
		else if( strcmp("dash",keyword ) == 0  )
			{
			bits |= GCLineStyle;
			underline = 1;
			gcv.line_style = LineOnOffDash;
			if( num_parsed > 1 && dash_value > 0 )
				{
				bits |= GCDashList;
				gcv.dashes = (char)dash_value;
				}
			}
		}

	gcv.font = app.normalFontStruct->fid;
	gc = XCreateGC( app.dpy, win, bits, &gcv );

	gcv.font = app.specialFontStruct->fid;
	special_gc = XCreateGC( app.dpy, win, bits, &gcv );
	}

//------------------------------------------------------//
//							//
//   EmacsMotif_AttributeSet implementation		//
//							//
//------------------------------------------------------//
EmacsMotif_AttributeSet::EmacsMotif_AttributeSet()
	{
	}

EmacsMotif_AttributeSet::~EmacsMotif_AttributeSet()
	{
	}

void EmacsMotif_AttributeSet::checkIndex( int index ) const
	{
	switch( index )
		{
	case SYNTAX_DULL:
	case LINE_M_ATTR_HIGHLIGHT:
	case SYNTAX_WORD:
	case SYNTAX_TYPE_STRING1:
	case SYNTAX_TYPE_STRING2:
	case SYNTAX_TYPE_STRING3:
	case SYNTAX_TYPE_COMMENT1:
	case SYNTAX_TYPE_COMMENT2:
	case SYNTAX_TYPE_COMMENT3:
	case SYNTAX_TYPE_KEYWORD1:
	case SYNTAX_TYPE_KEYWORD2:
	case SYNTAX_TYPE_KEYWORD3:
	case LINE_ATTR_MODELINE:
	case LINE_ATTR_USER+1:
	case LINE_ATTR_USER+2:
	case LINE_ATTR_USER+3:
	case LINE_ATTR_USER+4:
	case LINE_ATTR_USER+5:
	case LINE_ATTR_USER+6:
	case LINE_ATTR_USER+7:
	case LINE_ATTR_USER+8:
		break;
	default:
		{
		_dbg_msg( FormatString( "EmacsMotif_AttributeSet index=%d out of range 0 ... %d" )
			<< index << attr_array_size );
		fatal_error( 999 );
		}
		}
	}

EmacsMotif_Attributes &EmacsMotif_AttributeSet::getAttr( int index )
	{
	checkIndex( index );
	return attr_array[index];
	}

void EmacsMotif_AttributeSet::setAttr( int index, EmacsMotif_Attributes &attr )
	{
	checkIndex( index );
	attr_array[index] = attr;
	}

//==============================================================================//
//										//
//	UI widgets								//
//										//
//==============================================================================//


//------------------------------------------------------------------------------//
//										//
//	Status Bar widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_StatusIndicator_LineNumber::EmacsMotif_StatusIndicator_LineNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "line_number_text", 7 )
	{ }

EmacsMotif_StatusIndicator_LineNumber::~EmacsMotif_StatusIndicator_LineNumber()
	{ }	


void EmacsMotif_StatusIndicator_LineNumber::update()
	{
	int value = 1;
	if( bf_cur != NULL )
		for( int n=1; n<=dot - 1; n += 1 )
			if( bf_cur->char_at (n) == '\n' )
				value++;
	if( value > 9999999 )
		value = 9999999;
	EmacsMotif_TextLabel::update( FormatString( "%07.7d" ) << value );
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_ColumnNumber::EmacsMotif_StatusIndicator_ColumnNumber( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "column_number_text", 4 )
	{ }

EmacsMotif_StatusIndicator_ColumnNumber::~EmacsMotif_StatusIndicator_ColumnNumber()
	{ }	


void EmacsMotif_StatusIndicator_ColumnNumber::update()
	{
	int value = 1;
	if( bf_cur != NULL )
		value = cur_col();

	if( value > 9999 )
		value = 9999;
	EmacsMotif_TextLabel::update( FormatString( "%04.4d" ) << value );
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_ReadOnly::EmacsMotif_StatusIndicator_ReadOnly( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "read_only_text", 4 )
	{ }

EmacsMotif_StatusIndicator_ReadOnly::~EmacsMotif_StatusIndicator_ReadOnly()
	{ }	


void EmacsMotif_StatusIndicator_ReadOnly::update()
	{
	if( bf_cur != NULL && bf_cur->b_mode.md_readonly )
		EmacsMotif_TextLabel::update("READ");
	else
		EmacsMotif_TextLabel::update("    ");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_OverStrike::EmacsMotif_StatusIndicator_OverStrike( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "over_strike_text", 3 )
	{ }

EmacsMotif_StatusIndicator_OverStrike::~EmacsMotif_StatusIndicator_OverStrike()
	{ }	


void EmacsMotif_StatusIndicator_OverStrike::update()
	{
	if( bf_cur != NULL && bf_cur->b_mode.md_replace )
		EmacsMotif_TextLabel::update("OVR");
	else
		EmacsMotif_TextLabel::update("INS");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_RecordType::EmacsMotif_StatusIndicator_RecordType( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "record_type_text", 4 )
	{ }

EmacsMotif_StatusIndicator_RecordType::~EmacsMotif_StatusIndicator_RecordType()
	{ }	

void EmacsMotif_StatusIndicator_RecordType::update()
	{
	if( bf_cur != NULL )
		switch( bf_cur->b_rms_attribute )
			{
		case FIO_RMS__Binary:	// literal read and write no
			EmacsMotif_TextLabel::update("BIN ");
			break;
		case FIO_RMS__StreamCRLF:	// MS-DOS/Windows lines
			EmacsMotif_TextLabel::update("CRLF");
			break;
		case FIO_RMS__StreamCR:	// Machintosh lines
			EmacsMotif_TextLabel::update(" CR ");
			break;
		case FIO_RMS__StreamLF:// Unix lines
			EmacsMotif_TextLabel::update(" LF ");
			break;

		case FIO_RMS__None:		// not known
		default:
			EmacsMotif_TextLabel::update("UNKN");
			}
	else
		EmacsMotif_TextLabel::update("UNKN");
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusIndicator_Message::EmacsMotif_StatusIndicator_Message( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_TextLabel( app, parent, "message_text" )
	{ }

EmacsMotif_StatusIndicator_Message::~EmacsMotif_StatusIndicator_Message()
	{ }	


//--------------------------------------------------------------------------------
EmacsMotif_StatusBar_Form::EmacsMotif_StatusBar_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "status_bar_form", xmFormWidgetClass )
	, line_number_frame( app, *this, "line_number_frame" )
	, column_number_frame( app, *this, "column_number_frame" )
	, read_only_frame( app, *this, "read_only_frame" )
	, over_strike_frame( app, *this, "over_strike_frame" )
	, record_type_frame( app, *this, "record_type_frame" )
	, line_number( app, line_number_frame )
	, column_number( app, column_number_frame )
	, read_only( app, read_only_frame )
	, over_strike( app, over_strike_frame )
	, record_type( app, record_type_frame )
	, message( app, *this )
	{ }

EmacsMotif_StatusBar_Form::~EmacsMotif_StatusBar_Form()
	{ }

void EmacsMotif_StatusBar_Form::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_FORM );
	args.setArg( XmNrightAttachment, XmATTACH_WIDGET );
	args.setArg( XmNrightWidget, read_only_frame.widget );
	args.setValues( message.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -500 );
	args.setValues( record_type_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -400 );
	args.setValues( read_only_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -300 );
	args.setValues( over_strike_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -200 );
	args.setValues( line_number_frame.widget );

	args.setArg( XmNtopAttachment, XmATTACH_FORM );
	args.setArg( XmNbottomAttachment, XmATTACH_FORM );
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, -100 );
	args.setValues( column_number_frame.widget );

	manageWidget();
	}

void EmacsMotif_StatusBar_Form::postRealizeInit()
	{
	EmacsXtArgs args;

	// now that the resizing has happened ask the size of the frames are adjust the offsets
	int spacing = 2;
	int offset = 0;

	offset -= column_number_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( column_number_frame.widget );
	
	offset -= line_number_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( line_number_frame.widget );

	offset -= over_strike_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( over_strike_frame.widget );

	offset -= record_type_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( record_type_frame.widget );

	offset -= read_only_frame.widget->core.width + spacing;
	args.setArg( XmNleftAttachment, XmATTACH_OPPOSITE_FORM );
	args.setArg( XmNleftOffset, offset );
	args.setValues( read_only_frame.widget );
	}

void EmacsMotif_StatusBar_Form::updateIndicators()
	{
	line_number.update();
	column_number.update();
	read_only.update();
	over_strike.update();
	record_type.update();
	}

//--------------------------------------------------------------------------------
EmacsMotif_StatusBar_Frame::EmacsMotif_StatusBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "status_bar_frame", xmFrameWidgetClass )
	, form( app, *this )
	{ }

EmacsMotif_StatusBar_Frame::~EmacsMotif_StatusBar_Frame()
	{ }

void EmacsMotif_StatusBar_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_StatusBar_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Status Bar widgets							//
//										//
//------------------------------------------------------------------------------//

EmacsMotif_ToolBar_Button::EmacsMotif_ToolBar_Button
	(
	EmacsMotif_Application &app,
	EmacsMotif_Core &parent,
	const EmacsString &name,
	const EmacsString &_label,
	const EmacsMotif_ActionItem &_action
	)
#ifdef TOOLBAR_TOGGLE_BUTTONS
	: EmacsMotif_Core( app, parent, name, _action.isToggleButton() ? xmToggleButtonWidgetClass : xmPushButtonWidgetClass )
#else
	: EmacsMotif_Core( app, parent, name, xmPushButtonWidgetClass )
#endif
	, label( _label )
	, action( _action )
	{
	action.setButton( widget, action.isToggleButton() );

	if( label.first( '/' ) < 0
	&&  label.first( ':' ) < 0 )
		{
		XmString p = XmStringCreateSimple( label.sdataHack() );

		EmacsXtArgs args;

		args.setArg( XmNlabelString, p );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( widget );

		XmStringFree( p );
		}
	else
		{
		EmacsXtArgs args;

		Pixel fg;
		Pixel bg;

		// find out our fg and bg colours
		args.setArg( XmNforeground, &fg );
		args.setArg( XmNbackground, &bg );
		args.getValues( widget );

		EmacsString fullname;

		expand_and_default( label, "", fullname );

		Pixmap pixmap = XmGetPixmap( XtScreen( parent.widget ), fullname.sdataHack(), fg, bg );

		if( pixmap == XmUNSPECIFIED_PIXMAP )
			{
			error( FormatString("Cannot load Pixmap from %s") << fullname );	
			}
		else
			{
			args.setArg( XmNlabelType, XmPIXMAP );
			args.setArg( XmNlabelPixmap, pixmap );
			}


#ifdef TOOLBAR_TOGGLE_BUTTONS
		if( action.isToggleButton() )
			{
			args.setArg( XmNindicatorOn, 0l );		// false
			args.setArg( XmNshadowThickness, 2 );
			args.setArg( XmNborderWidth, 0l );
			args.setArg( XmNshadowType, XmSHADOW_OUT );
			args.setArg( XmNmarginHeight, 0l );
			args.setArg( XmNmarginWidth, 0l );
			args.setArg( XmNwidth, 16 );
			}
#endif

		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( widget );
		}

#ifdef TOOLBAR_TOGGLE_BUTTONS
	if( action.isToggleButton() )
		install_handler( EmacsMotifCallback_ValueChangedButton );
	else
#endif
		install_handler( EmacsMotifCallback_Activate );
	}

EmacsMotif_ToolBar_Button::~EmacsMotif_ToolBar_Button()
	{ }

void EmacsMotif_ToolBar_Button::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Button::postRealizeInit()
	{ }

void EmacsMotif_ToolBar_Button::activate( XtPointer , XtPointer  )
	{
	action.queueAction();
	}

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_Separator::EmacsMotif_ToolBar_Separator
	(
	EmacsMotif_Application &app,
	EmacsMotif_Core &parent,
	const EmacsString &name,
	int width
	)
	: EmacsMotif_Core( app, parent, name, xmDrawingAreaWidgetClass )
	{
	EmacsXtArgs args;

//	Pixel fg;
//	Pixel bg;

	// find out our parents fg and bg colours
//	args.setArg( XmNforeground, &fg );
//	args.setArg( XmNbackground, &bg );
//	args.getValues( parent.widget );

	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
//	args.setArg( XmNforeground, fg );
//	args.setArg( XmNbackground, bg );
	args.setArg( XmNborderWidth, width/2 );
	args.setArg( XmNshadowThickness, 0l );
	args.setValues( widget );
	}


EmacsMotif_ToolBar_Separator::~EmacsMotif_ToolBar_Separator()
	{ }

void EmacsMotif_ToolBar_Separator::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Separator::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_RowColumn::EmacsMotif_ToolBar_RowColumn( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "tool_bar_rowcolumn", xmRowColumnWidgetClass )
	{ }

EmacsMotif_ToolBar_RowColumn::~EmacsMotif_ToolBar_RowColumn()
	{ }

void EmacsMotif_ToolBar_RowColumn::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNorientation, XmHORIZONTAL );
	args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
	args.setValues( widget );
	manageWidget();
	}

void EmacsMotif_ToolBar_RowColumn::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_ToolBar_Frame::EmacsMotif_ToolBar_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "tool_bar_frame", xmFrameWidgetClass )
	, row_column( app, *this )
	{ }

EmacsMotif_ToolBar_Frame::~EmacsMotif_ToolBar_Frame()
	{ }

void EmacsMotif_ToolBar_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_ToolBar_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Drawing Area widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_Screen_DrawingArea::EmacsMotif_Screen_DrawingArea( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_drawing_area", xmDrawingAreaWidgetClass )
	{
	install_handler( EmacsMotifCallback_Resize, NULL );
	install_handler( EmacsMotifCallback_Expose, NULL );
	install_handler( EmacsMotifCallback_Input, NULL );
	}

EmacsMotif_Screen_DrawingArea::~EmacsMotif_Screen_DrawingArea()
	{ }

void EmacsMotif_Screen_DrawingArea::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNforeground, application.resources.n_fg );
	args.setArg( XmNbackground, application.resources.n_bg );
	args.setArg( XmNmarginHeight, 0l );
	args.setArg( XmNmarginWidth, 0l );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_Screen_DrawingArea::postRealizeInit()
	{
	rev_attr.create( application, application.resources.n_bg, application.resources.n_fg, NULL, XtWindow( widget ) );
	cu_attr.create( application, application.resources.c_fg, application.resources.c_bg, NULL, XtWindow( widget ) );
	curev_attr.create( application, application.resources.c_bg, application.resources.c_fg, NULL, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_DULL).create( application, application.resources.n_fg, application.resources.n_bg, application.resources.n_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_M_ATTR_HIGHLIGHT).create( application, application.resources.hl_fg, application.resources.hl_bg, application.resources.hl_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_WORD).create( application, application.resources.word_fg, application.resources.word_bg, application.resources.word_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_STRING1).create( application, application.resources.string1_fg, application.resources.string1_bg, application.resources.string1_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_STRING2).create( application, application.resources.string2_fg, application.resources.string2_bg, application.resources.string2_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_STRING3).create( application, application.resources.string3_fg, application.resources.string3_bg, application.resources.string3_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_COMMENT1).create( application, application.resources.comment1_fg, application.resources.comment1_bg, application.resources.comment1_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_COMMENT2).create( application, application.resources.comment2_fg, application.resources.comment2_bg, application.resources.comment2_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_COMMENT3).create( application, application.resources.comment3_fg, application.resources.comment3_bg, application.resources.comment3_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_KEYWORD1).create( application, application.resources.kw1_fg, application.resources.kw1_bg, application.resources.kw1_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_KEYWORD2).create( application, application.resources.kw2_fg, application.resources.kw2_bg, application.resources.kw2_underline, XtWindow( widget ) );
	attr_set.getAttr(SYNTAX_TYPE_KEYWORD3).create( application, application.resources.kw3_fg, application.resources.kw3_bg, application.resources.kw3_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_MODELINE).create( application, application.resources.ml_fg, application.resources.ml_bg, application.resources.ml_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+1).create( application, application.resources.user1_fg, application.resources.user1_bg, application.resources.user1_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+2).create( application, application.resources.user2_fg, application.resources.user2_bg, application.resources.user2_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+3).create( application, application.resources.user3_fg, application.resources.user3_bg, application.resources.user3_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+4).create( application, application.resources.user4_fg, application.resources.user4_bg, application.resources.user4_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+5).create( application, application.resources.user5_fg, application.resources.user5_bg, application.resources.user5_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+6).create( application, application.resources.user6_fg, application.resources.user6_bg, application.resources.user6_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+7).create( application, application.resources.user7_fg, application.resources.user7_bg, application.resources.user7_underline, XtWindow( widget ) );
	attr_set.getAttr(LINE_ATTR_USER+8).create( application, application.resources.user8_fg, application.resources.user8_bg, application.resources.user8_underline, XtWindow( widget ) );
	}

//------------------------------------------------------------------------------
EmacsMotif_Screen_Frame::EmacsMotif_Screen_Frame( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_frame", xmFrameWidgetClass )
	, drawing_area( app, *this )
	{ }

EmacsMotif_Screen_Frame::~EmacsMotif_Screen_Frame()
	{ }

void EmacsMotif_Screen_Frame::preRealizeInit()
	{
	manageWidget();
	}

void EmacsMotif_Screen_Frame::postRealizeInit()
	{ }

//------------------------------------------------------------------------------//
//										//
//	Main screen form								//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_Screen_Form::EmacsMotif_Screen_Form( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "screen_form", xmFormWidgetClass )
#ifdef TOOL_BAR_PRESENT
	, tool_bar( app, *this )
#endif
	, screen( app, *this )
#ifdef STATUS_BAR_PRESENT
	, status_bar( app, *this )
#endif
	{ }

EmacsMotif_Screen_Form::~EmacsMotif_Screen_Form()
	{ }

void EmacsMotif_Screen_Form::preRealizeInit()
	{
	configureScreenGeometry();
	manageWidget();
	}	

void EmacsMotif_Screen_Form::postRealizeInit()
	{
	configureScreenGeometry();
	}

void EmacsMotif_Screen_Form::configureScreenGeometry(void)
	{
	// the main work here is to setup the children

	EmacsXtArgs args;

	EmacsXtArgs drawing_area_args;

	if( application.resources.display_tool_bar )
		{
		tool_bar.manageWidget();

		// tool_bar setup
		args.setArg( XmNtopAttachment, XmATTACH_FORM );
		args.setArg( XmNleftAttachment, XmATTACH_FORM );
		args.setArg( XmNrightAttachment, XmATTACH_FORM );
		args.setArg( XmNbottomAttachment, XmATTACH_OPPOSITE_FORM );

		//
		// 38 is the magic number that allows
		// both Push Button and Toggle Buttons
		// to display there bit maps without truncation
		//
		args.setArg( XmNbottomOffset, -38 );

		args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setValues( tool_bar.widget );

		drawing_area_args.setArg( XmNtopAttachment, XmATTACH_WIDGET );
		drawing_area_args.setArg( XmNtopWidget, tool_bar.widget );
		}
	else
		{
		drawing_area_args.setArg( XmNtopAttachment, XmATTACH_FORM );
		tool_bar.unmanageWidget();
		}

	if( application.resources.display_status_bar )
		{
		status_bar.manageWidget();

		// status_bar setup
		args.setArg( XmNtopAttachment, XmATTACH_OPPOSITE_FORM );
		args.setArg( XmNtopOffset, -(application.fth + 2*(2+2+2)) );
		args.setArg( XmNleftAttachment, XmATTACH_FORM );
		args.setArg( XmNrightAttachment, XmATTACH_FORM );
		args.setArg( XmNbottomAttachment, XmATTACH_FORM );
		args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
		args.setArg( XmNtraversalOn, 0l );			// not a part of a TAB GROUP - no input here
		args.setArg( XmNborderWidth, 1 );
		args.setValues( status_bar.widget );

		drawing_area_args.setArg( XmNbottomAttachment, XmATTACH_WIDGET );
		drawing_area_args.setArg( XmNbottomWidget, status_bar.widget );
		}
	else
		{
		status_bar.unmanageWidget();

		drawing_area_args.setArg( XmNbottomAttachment, XmATTACH_FORM );
		}

	// drawing_area setup
	drawing_area_args.setArg( XmNleftAttachment, XmATTACH_FORM );
	drawing_area_args.setArg( XmNrightAttachment, XmATTACH_FORM );
	drawing_area_args.setArg( XmNshadowType, XmSHADOW_ETCHED_IN );
	drawing_area_args.setArg( XmNborderWidth, 1 );
	drawing_area_args.setArg( XmNtraversalOn, 1l );			// input here
	drawing_area_args.setValues( screen.widget );
	}
//------------------------------------------------------------------------------//
//										//
//	Main Window widgets							//
//										//
//------------------------------------------------------------------------------//
EmacsMotif_MainWindow::EmacsMotif_MainWindow( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, parent, "main_window", xmMainWindowWidgetClass )
	, menu_bar( app, *this )
	, screen_form( app, *this )
	{ }

EmacsMotif_MainWindow::~EmacsMotif_MainWindow()
	{ }

void EmacsMotif_MainWindow::preRealizeInit()
	{
	install_handler( EmacsMotifCallback_WmProtocol, NULL );

	EmacsXtArgs args;

	args.setArg( XmNbackground, application.resources.n_bg );
	args.setArg( XmNwidth, application.pixelWidth );
	args.setArg( XmNheight, application.pixelHeight );
	args.setValues( widget );

	manageWidget();
	}

void EmacsMotif_MainWindow::postRealizeInit()
	{ }

extern int win_emacs_quit;

void EmacsMotif_MainWindow::wm_protocol( XtPointer )
	{
	win_emacs_quit = 1;
	}



//------------------------------------------------------------------------------
EmacsMotif_MenuBar::EmacsMotif_MenuBar( EmacsMotif_Application &app, EmacsMotif_Core &parent )
	: EmacsMotif_Core( app, XmCreateMenuBar( parent.widget, "menu_bar", NULL, 0 ) )
	{ }

EmacsMotif_MenuBar::~EmacsMotif_MenuBar()
	{ }

extern void xwin_menu_init( Widget );

void EmacsMotif_MenuBar::preRealizeInit()
	{
	xwin_menu_init( widget );
	manageWidget();
	}

void EmacsMotif_MenuBar::postRealizeInit()
	{ }

//------------------------------------------------------------------------------
EmacsMotif_Shell::EmacsMotif_Shell( EmacsMotif_Application &app, Widget shell_widget )
	: EmacsMotif_Core( app, shell_widget )
	, main_window( app, *this )
	{ }

EmacsMotif_Shell::~EmacsMotif_Shell()
	{ }

void EmacsMotif_Shell::preRealizeInit()
	{
	EmacsXtArgs args;

	args.setArg( XmNheightInc, application.fth );
	args.setArg( XmNwidthInc, application.ftw );
	args.setArg( XmNminHeight, application.fth * 10 );
	args.setArg( XmNminWidth, application.ftw * 30 );
	args.setArg( XmNmaxHeight, min( application.fth * MSCREENLENGTH, XDisplayHeight(application.dpy,0 )-30  ) );
	args.setArg( XmNmaxWidth, min( application.ftw * MSCREENWIDTH, XDisplayWidth(application.dpy,0 )-10  ) );

	args.setArg( XmNdeleteResponse, XmDO_NOTHING );

	args.setValues( widget );
	}

void EmacsMotif_Shell::postRealizeInit()
	{
	EmacsXtArgs args;

	/* Set application's window manager resources */
	Pixmap iconPixmap = XCreateBitmapFromData
		(
		application.dpy, XtWindow( widget ),
		(char *)write_scrolls_bits, write_scrolls_width, write_scrolls_height
		);
	if (iconPixmap != None)
		{
		args.setArg( XmNiconPixmap, iconPixmap );

		args.setValues( widget );
		}
	}

//------------------------------------------------------------------------------
EmacsMotif_Application::EmacsMotif_Application()
	: dpy(0)
	, app_context(NULL)
	, shell(NULL)
	, cursors(0)
	{ }

EmacsMotif_Application::~EmacsMotif_Application()
	{ }

const char *EmacsMotif_Application::emacs_class = "BEmacs";

extern int motif_argc;
extern char *motif_argv[];
bool EmacsMotif_Application::initApplication( const EmacsString &display_location )
	{

	{
	const char *class_name = getenv("EMACS_CLASS_NAME");
	if( class_name != NULL )
		emacs_class = class_name;
	}

#define FGC( name, field, value ) \
		{ name, XtCForeground, XtRPixel, sizeof( Pixel ), XtOffset( Resources *, field ), XmRString, const_cast<char *>(value)}
#define BGC( name, field, value ) \
		{ name, XtCBackground, XtRPixel, sizeof( Pixel ), XtOffset( Resources *, field ), XmRString, const_cast<char *>(value)}
#define STR( name, field, value ) \
		{ name, XtCString, XtRString, sizeof(char * ), XtOffset( Resources *, field ), XmRString, const_cast<char *>(value) }

	static XtResource resource_descriptions[] =
		{
		{"displayScrollBars", "DisplayScrollBars", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_scroll_bars ), XmRImmediate,( caddr_t ) 1},
		{"displayStatusBar", "DisplayStatusBar", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_status_bar ), XmRImmediate,( caddr_t ) 1},
		{"displayToolBar", "DisplayToolBar", XtRBoolean, sizeof( Boolean ), XtOffset( Resources *, display_tool_bar ), XmRImmediate,( caddr_t ) 1},
		{"pointerForeground", "PointerForeground", XtRString, sizeof( char * ), XtOffset( Resources *, p_fg ), XtRString, const_cast<char *>("white")},
		{"pointerBackground", "PointerBackground", XtRString, sizeof( char * ), XtOffset( Resources *, p_bg ), XtRString, const_cast<char *>("black")},
		{"pointerShape", "PointerShape", XtRInt, sizeof( int ), XtOffset( Resources *, p_shape ), XmRImmediate,( caddr_t ) 0xFFFFFFFF},
		{"textFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, normal_font_name ), XmRString, const_cast<char *>("-dec-terminal-medium-r-normal--14-*-*-*-c-80-iso8859-1") },
		{"specialFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, special_font_name ), XmRString, const_cast<char *>("-dec-terminal-medium-r-normal--14-*-*-*-c-80-dec-dectech") },
#ifdef BOLDFONT
		{"boldFont", "Font", XtRString, sizeof(char * ), XtOffset( Resources *, bold_font_name ), XmRString, const_cast<char *>("-dec-terminal-bold-r-normal--14-*-*-*-c-80-iso8859-1") },
#endif
		{"rows", "Rows", XtRInt, sizeof( int ), XtOffset( Resources *, rows ), XmRImmediate,( caddr_t ) 40},
		{"columns", "Columns", XtRInt, sizeof( int ), XtOffset( Resources *, columns ), XmRImmediate,( caddr_t ) 80},

		BGC("cursorForeground", c_fg, XtDefaultBackground),
		FGC("cursorBackground", c_bg, XtDefaultForeground),

		FGC("normalForeground", n_fg, "#000000"),
		BGC("normalBackground", n_bg, "#ffffff"),
		STR("normalUnderline", n_underline, "0"),

		FGC("modelineForeground", ml_fg, "#ffffff"),
		BGC("modelineBackground", ml_bg, "#0000ff"),
		STR("modelineUnderline", ml_underline, "0"),

		FGC("highlightForeground", hl_fg, "#ffffff"),
		BGC("highlightBackground", hl_bg, "#808080"),
		STR("highlightUnderline", hl_underline, "0"),

		FGC("wordForeground", word_fg, "#000000"),
		BGC("wordBackground", word_bg, "#ffffff"),
		STR("wordUnderline", word_underline, "0"),

		FGC("stringForeground", string1_fg, "#008000"),
		BGC("stringBackground", string1_bg, "#ffffff"),
		STR("stringUnderline", string1_underline, "0"),

		FGC("string2Foreground", string2_fg, "#008000"),
		BGC("string2Background", string2_bg, "#ffffff"),
		STR("string2Underline", string2_underline, "0"),

		FGC("string3Foreground", string3_fg, "#008000"),
		BGC("string3Background", string3_bg, "#ffffff"),
		STR("string3Underline", string3_underline, "0"),

		FGC("commentForeground", comment1_fg, "#008000"),
		BGC("commentBackground", comment1_bg, "#ffffff"),
		STR("commentUnderline", comment1_underline, "0"),

		FGC("comment2Foreground", comment2_fg, "#008000"),
		BGC("comment2Background", comment2_bg, "#ffffff"),
		STR("comment2Underline", comment2_underline, "0"),

		FGC("comment3Foreground", comment3_fg, "#008000"),
		BGC("comment3Background", comment3_bg, "#ffffff"),
		STR("comment3Underline", comment3_underline, "0"),

		FGC("keyword1Foreground", kw1_fg, "#0000ff"),
		BGC("keyword1Background", kw1_bg, "#ffffff"),
		STR("keyword1Underline", kw1_underline, "0"),

		FGC("keyword2Foreground", kw2_fg, "#ff0000"),
		BGC("keyword2Background", kw2_bg, "#ffffff"),
		STR("keyword2Underline", kw2_underline, "0"),

		FGC("keyword3Foreground", kw3_fg, "#ff00ff"),
		BGC("keyword3Background", kw3_bg, "#ffffff"),
		STR("keyword3Underline", kw3_underline, "0"),

		FGC("user1Foreground", user1_fg, "#ff0000"),
		BGC("user1Background", user1_bg, "#ffffff"),
		STR("user1Underline", user1_underline, "0"),

		FGC("user2Foreground", user2_fg, "#00ff00"),
		BGC("user2Background", user2_bg, "#ffffff"),
		STR("user2Underline", user2_underline, "0"),

		FGC("user3Foreground", user3_fg, "#0000ff"),
		BGC("user3Background", user3_bg, "#ffffff"),
		STR("user3Underline", user3_underline, "0"),

		FGC("user4Foreground", user4_fg, "#ffff00"),
		BGC("user4Background", user4_bg, "#ffffff"),
		STR("user4Underline", user4_underline, "0"),

		FGC("user5Foreground", user5_fg, "#ff00ff"),
		BGC("user5Background", user5_bg, "#ffffff"),
		STR("user5Underline", user5_underline, "0"),

		FGC("user6Foreground", user6_fg, "#00ffff"),
		BGC("user6Background", user6_bg, "#ffffff"),
		STR("user6Underline", user6_underline, "0"),

		FGC("user7Foreground", user7_fg, "#ffffff"),
	      	BGC("user7Background", user7_bg, "#c0c0c0"),
		STR("user7Underline", user7_underline, "0"),

		FGC("user8Foreground", user8_fg, "#ffffff"),
		BGC("user8Background", user8_bg, "#555555"),
		STR("user8Underline", user8_underline, "0")
		};

	XtToolkitInitialize();

	app_context = XtCreateApplicationContext();

	XtAppSetErrorHandler( app_context, xt_error_handler );
	XtAppSetWarningHandler( app_context, xt_warning_handler );
	XSetErrorHandler( x_error_handler );

	static String fallback_resources[] =
		{
		"*foreground: black",
		"*background: grey75",
		".geometry: 100x40",
		".displayToolBar:	true",
		".displayScrollBars:	true",
		".displayStatusBar:	true",
		".textFont: -adobe-courier-medium-r-normal--14-120-100-100-m-100-iso8859-1",
		//BEmacs.textFont: -dec-terminal-medium-r-normal--14-*-*-*-c-80-iso8859-1
		".specialFont: -dec-terminal-medium-r-normal--14-*-*-*-c-80-dec-dectech",
		//BEmacs.textFont: -dec-terminal-medium-r-normal--20-0-75-75-c-0-iso8859-1
		//BEmacs.specialFont: -dec-terminal-medium-r-normal--20-0-75-75-c-0-dec-dectech
		//#ifdef COLOR
		".normalForeground: black",
		".normalBackground: white",
		".normalUnderline: no",
		".modelineForeground: yellow",
		".modelineBackground: blue",
		".modelineUnderline: no",
		".highlightForeground: black",
		".highlightBackground: grey75",
		".highlightUnderline: no",
		//BEmacs.cursorForeground:
		//BEmacs.cursorBackground:
		//BEmacs.wordForeground:
		//BEmacs.wordBackground:
		//BEmacs.wordUnderline:
		".stringForeground: purple",
		".stringBackground: white",
		".stringUnderline: no",
		".string2Foreground: purple",
		".string2Background: white",
		".string2Underline: no",
		".string3Foreground: purple",
		".string3Background: white",
		".string3Underline: no",
		".commentForeground: darkgreen",
		".commentBackground: white",
		".commentUnderline: no",
		".comment2Foreground: darkgreen",
		".comment2Background: white",
		".comment2Underline: no",
		".comment3Foreground: darkgreen",
		".comment3Background: white",
		".comment3Underline: no",
		".keyword1Foreground: blue",
		".keyword1Background: white",
		".keyword1Underline: yes",
		".keyword2Foreground: purple",
		".keyword2Background: white",
		".keyword2Underline: yes",
		".keyword3Foreground: red",
		".keyword3Background: white",
		".keyword3Underline: yes",
		".user1Foreground: red",
		".user1Background: white",
		".user1Underline: yes",
		".user2Foreground: blue",
		".user2Background: white",
		".user2Underline: no",
		".user3Foreground: green",
		".user3Background: white",
		".user3Underline: no",
		".user4Foreground: red",
		".user4Background: white",
		".user4Underline: no",
		".user5Foreground: black",
		".user5Background: white",
		".user5Underline: no",
		".user6Foreground: black",
		".user6Background: white",
		".user6Underline: no",
		".user7Foreground: black",
		".user7Background: white",
		".user7Underline: no",
		".user8Foreground: black",
		".user8Background: white",
		".user8Underline: no",
		//
		//	Indicators
		//
		"*status_bar_form.?.?.background: white",

		//BEmacs.*.line_number_text.foreground: black
		//BEmacs.*.line_number_text.background: white
		//BEmacs.*.column_number_text.foreground: black
		//BEmacs.*.column_number_text.background: white
		//BEmacs.*.read_only_text.foreground: red
		//BEmacs.*.read_only_text.background: white
		//BEmacs.*.over_strike_text.foreground: blue
		//BEmacs.*.over_strike_text.background: white
		//BEmacs.*.tool_bar_rowcolumn.?.foreground: black
		"*tool_bar_rowcolumn.spacing: 0",


		//
		//	Give F10 back to Emacs as an usable key
		//
		".main_window.menu_bar.menuAccelerator: Alt<Key>F10",
		NULL
		};


	XtAppSetFallbackResources(app_context, fallback_resources);

	EmacsString name( command_line_arguments.argument(0).value() );

	dpy = XtOpenDisplay( app_context, NULL, name.sdataHack(), emacs_class, NULL, 0, &motif_argc, motif_argv );
	if( dpy == NULL )
		{
		_dbg_msg( FormatString( "Cannot open X server at %s" ) <<display_location );
		return false;
		}

	dpy_fd = ConnectionNumber( dpy );

	Widget shell_widget = XtAppCreateShell( "emacs", emacs_class, applicationShellWidgetClass, dpy, NULL, 0 );

	// get the resources
	XtGetApplicationResources( shell_widget, (XtPointer)&resources, resource_descriptions, XtNumber(resource_descriptions), NULL, 0 );

	// Deal with the pointer shape
	if( (pointerFontStruct = XLoadQueryFont( dpy, "decw$cursor" ) ) == NULL )
		{
		_dbg_msg( FormatString( "Display %s is unable to open font: decw$cursor" ) << DisplayString(dpy ) );
		return false;
		}

	if( (unsigned)resources.p_shape != 0xFFFFFFFF )
		{
		XColor fgColor, bgColor;
		if( resources.p_shape & 1 )
			resources.p_shape &= ~1;
		XParseColor(dpy, DefaultColormap( dpy, DefaultScreen(dpy ) ), resources.p_fg, &fgColor );
		XParseColor(dpy, DefaultColormap( dpy, DefaultScreen(dpy ) ), resources.p_bg, &bgColor );
		arrowCursor = XCreateGlyphCursor( dpy, pointerFontStruct->fid, pointerFontStruct->fid,
			resources.p_shape, resources.p_shape + 1, &fgColor, &bgColor );
		cursors++;
		}
#  ifdef ACTIVITY
	watchCursor = XCreateGlyphCursor( dpy, pointerFontStruct->fid, pointerFontStruct->fid, 4, 5, &fgColor, &bgColor );
#  endif

	// Fetch the fonts and calculate their sizes
	if( (normalFontStruct = XLoadQueryFont( dpy, resources.normal_font_name ) ) == NULL )
		{
		_dbg_msg( FormatString( "Display %s is unable to open font: %s" ) << DisplayString(dpy) << resources.normal_font_name );
		return 0;
		}

	// assume we have the special font
	term_deccrt = true;
	if( (specialFontStruct = XLoadQueryFont( dpy, resources.special_font_name ) ) == NULL )
		{
		term_deccrt = false;
		specialFontStruct = normalFontStruct;
		_dbg_msg( FormatString( "Display %s is unable to open font: %s" ) << DisplayString(dpy ) << resources.normal_font_name );
		}

	fth = normalFontStruct->ascent + normalFontStruct->descent;
	// average width
	ftw = (normalFontStruct->max_bounds.width + normalFontStruct->min_bounds.width)/2;
	ftd = normalFontStruct->descent;
	innerBorder = 2;

	while( ftw*vertical_bar_width < scroll_bar_min_width )
		vertical_bar_width++;

	// default to 80 col by 24 lines
	pixelWidth = (80+vertical_bar_width) * ftw + innerBorder * 2;
	pixelHeight = 24 * fth + innerBorder * 2;

	shell = new EmacsMotif_Shell( *this, shell_widget );

#if DBG_DISPLAY
	if( dbg_flags & DBG_DISPLAY )
		shell->debugPrintWidgetTree( "initApplication", 0 );
#endif

	return true;
	}

void EmacsMotif_Application::xt_error_handler( char *message )
	{
	_dbg_msg( FormatString("Xt Error: %s") << message );
	debug_invoke();
	}

void EmacsMotif_Application::xt_warning_handler( char *message )
	{
	_dbg_msg( FormatString("Xt Warning: %s") << message );
	debug_invoke();
	}

int EmacsMotif_Application::x_error_handler( Display *dpy, XErrorEvent *err_event )
	{
	_dbg_msg( FormatString("X Error: %d") << err_event->error_code );

	char buf[BUFSIZ];
	XGetErrorText( dpy, err_event->error_code, buf, sizeof buf );
	_dbg_msg( FormatString("X Error: %s") << buf );

	debug_invoke();

	return 0;
	}

#endif
