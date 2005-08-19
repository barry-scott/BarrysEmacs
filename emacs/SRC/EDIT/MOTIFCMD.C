/* 
 * Motif X-Window system pseudo-terminal driver for DEC Emacs
 *
 *	Nick Emery Jun-93
 *	adapted from spm's X driver 7/85	MIT Project Athena
 */

#include <emacs.h>

#ifdef XWINDOWS
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/RowColumn.h>
#include <Xm/RowColumnP.h>
#include <Xm/Separator.h>
#include <Xm/SeparatoG.h>

/*
 * Data Types:
 */

/*
 * Data structures for menus
 */

struct any
    {
    int type;				/* Type of this structure */
    union any_menu *next;		/* Chain of items */
    unsigned char *name;		/* Name of the button */
    int pos;				/* menu order information */
    Widget button;			/* The button widget */
    };

struct button
    {
    int type;				/* Type of this structure */
    union any_menu *next;		/* Chain of items */
    unsigned char *name;		/* Name of the button */
    int pos;				/* menu order information */
    Widget button;			/* The button widget */
    /* Above fields are common in all meny types */

    unsigned char *keys;		/* Key sequence to use when button is pressed */
    void (*callback)(struct button *);	/* Key sequence to use when button is pressed */
    };

struct cascade
    {
    int type;			    /* Type of this structure */
    union any_menu *next;	    /* Chain of items */
    unsigned char *name;	    /* Name of the button */
    int pos;			    /* menu order information */
    Widget button;		    /* Cascade button */    
    /* Above fields are common in all meny types */

    union any_menu *children;	    /* Chain of items */
    Widget menu;		    /* Menu pane */
    int ccount;			    /* Count of the number of children present */
    };

union any_menu
    {
    struct cascade cascade;
    struct button button;
    struct any any;
    };

/*
 * Own Storage:
 */

Widget file;
static union any_menu menu_bar;
static union any_menu *menu_cur;
static int insert_incomplete;
static unsigned char *selection = NULL;
static unsigned int selection_len;

#define MAX_CHILDREN 100
#define MAX_LABEL 100

#define BUTTON 0
#define CASCADE 1
#define SEPARATOR 2

static char *names[] = 
    {
    "button",
    "cascade menu",
    "separator"
    };

#define CB_CANCEL 0
#define CB_APPLY 1
#define CB_OK 2
#define CB_POPDOWN 3

/*
 * External References:
 */

extern Display *dpy;
extern Widget app_shell, main_window;

extern void input_char_string (char *key);
extern int xwin_readsocket (struct trmcontrol *tt, unsigned char *bufp, unsigned int numchars);
extern int error_and_exit (void);
extern int newline_command (void);
extern int motif_not_running (void);
extern region_to_string (void);
extern int motif_iconic;
extern int is_motif;

/* Iconify emacs */
int xterm_iconify ()
    {
    Arg args[1];

    if (!is_motif)
	return motif_not_running ();

    XtSetArg (args[0], XmNiconic, 1);
    XtSetValues (app_shell, args, 1);
    motif_iconic++;
    return 0;
    }

/* Deiconify emacs */
int xterm_deiconify ()
    {
    Arg args[1];

    if (!is_motif)
	return motif_not_running ();

    XtSetArg (args[0], XmNiconic, 0);
    XtSetValues (app_shell, args, 1);
    motif_iconic = 0;
    return 0;
    }

/* Raise emacs to the top of the window stack */
int xterm_raise ()
    {
    if (!is_motif)
	return motif_not_running ();

    XRaiseWindow (dpy, XtWindow (app_shell));
    return 0;
    }

static void insert_func
	(
	Widget PNOTUSED(w), void *PNOTUSED(junk), Atom *PNOTUSED(sel),
	Atom *PNOTUSED(type), void * value, unsigned long *length, int *PNOTUSED(format)
	)
    {
    ins_cstr ((unsigned char *)value, (int)*length);
    insert_incomplete = 0;
    }

/* Place contents of the primary selection to the current Emacs buffer at dot */
int xterm_to_buffer()
    {
    if (!is_motif)
	return motif_not_running ();

    insert_incomplete = 1;
    XtGetSelectionValue (main_window, XA_PRIMARY, XA_STRING, insert_func, NULL, XtLastTimestampProcessed (dpy));
    while (insert_incomplete)
	xwin_readsocket (0, NULL, 0);
    return 0;
    }

/* Convert string in selection to required type */
static Boolean cvt_proc (Widget PNOTUSED(w), Atom *sel, Atom *target, Atom *type_return, void **value_return,
  unsigned long *length_return, int *format_return)
    {
    if (*sel != XA_PRIMARY || *target != XA_STRING)
	return FALSE;
    *type_return = XA_STRING;
    *value_return = (void *)selection;
    *length_return = selection_len;
    *format_return = 8;

    return TRUE;
    }

static void lose_proc (Widget PNOTUSED(w), Atom *sel)
    {
    if (*sel == XA_PRIMARY)
	{
	free (selection);
	selection = NULL;
	}
    return;
    }

static void done_proc (Widget PNOTUSED(w), Atom *PNOTUSED(sel), Atom *PNOTUSED(target))
    {
    return;
    }

/* Copy Emacs buffer to X server cut/paste buffer 0. */
int buffer_to_xterm()
    {
    if (!is_motif)
	return motif_not_running ();

    if (selection)
	free (selection);
    region_to_string ();
    if (err)
	selection = NULL;
    else
	{
	Boolean resp;
	selection = ml_value->exp_v.v_string;
	selection_len = ml_value->exp_int;
	ml_value->exp_release = 0;
	void_result ();
	resp = XtOwnSelection (main_window, XA_PRIMARY, XtLastTimestampProcessed (dpy), cvt_proc, lose_proc, done_proc);
	}
    return 0;	
    }

/* Quit callback */
static void quit_cb (struct button *PNOTUSED(button))
    {
    quitting_emacs = 1;
    }

/* Store information about the menus into a buffer */
static void dump_menu (union any_menu *item, int level)
    {
    unsigned char line[100];
    to_col (level);

    sprintfl (line, sizeof (line), u_str ("%s %s at position %d"), item->any.name, names[item->any.type], item->any.pos);
    ins_str (line);
    switch (item->any.type)
	{
	case BUTTON:
	    sprintfl (line, sizeof (line), u_str (" bound to \"%s\""), key_to_str (item->button.keys, _str_len (item->button.keys)));
	    ins_str (line);
	    newline_command ();
	    break;

	case SEPARATOR:
	    newline_command ();
	    break;

	case CASCADE:
	    newline_command ();
	    if (item->cascade.children)
		dump_menu (item->cascade.children, level + 2);
	    break;
	default:
	    newline_command ();
	    break;
	}

    if (item->any.next != NULL)
	dump_menu (item->any.next, level);
    }

/* Command interface to UI-dump-menu */
int dump_menu_command (void)
    {
    struct emacs_buffer *old = bf_cur;

    if (!is_motif)
	return motif_not_running ();

    scratch_bfn (u_str ("Menu table"), interactive);
    ins_str (u_str ("MOTIF Menus\n"));
    ins_str (u_str ("-----------\n\n"));
    dump_menu (&menu_bar, 2);
    bf_cur->b_checkpointed = -1;
    bf_modified = 0;
    set_dot (1);
    set_bfp (old);
    window_on (bf_cur);

    return 0;
    }

/* convert a menu push to a key sequence */
static void menu_to_keys (Widget PNOTUSED(w), void *button_, void * PNOTUSED(info))
    {
    struct button *button = (struct button *)button_;
    if (button->callback)
	(*button->callback)(button);
    input_char_string (s_str (button->keys));
    }

static Cardinal insert_pos (XmRowColumnWidgetRec *w)
    {
    Arg args[2];
    int count = 0, i;
    Widget *children, parent, prev;

    if (menu_cur == NULL)
	{
	parent = XtParent (w);
	XtSetArg (args[0], XmNnumChildren, &i);
	i = 0;
	XtGetValues (parent, args, 1);
	}
    else
	{
	prev = menu_cur->any.button;
	parent = XtParent (w);
	XtSetArg (args[0], XmNnumChildren, &count);
	XtSetArg (args[1], XmNchildren, &children);
	XtGetValues (parent, args, 2);
	for (i = 0; i < count; i++)
	    if (children[i] == prev)
		break;
	}
    return i;
    }

/* Create a push button in a menu pane */
static union any_menu *mkbutton (union any_menu *parent, unsigned char *name, int pos, int mn, unsigned char *keys,
  void (*callback)(struct button *))
    {
    int argc;
    Arg args[1];
    Widget button;
    struct button *new_button = NULL;
    union any_menu *prev, *cur;

    if (parent->any.type != CASCADE)
	error (u_str ("Parent of button \"%s\" must be a cascade menu"), name);
    else if (parent->cascade.ccount >= MAX_CHILDREN)
	error (u_str ("Too many buttons in menu \"%s\""), parent->cascade.name);
    else if( (new_button = (struct button *)malloc (sizeof (struct button), malloc_type_char)) != NULL )
	{
	new_button->type = BUTTON;
	if ((new_button->name = savestr (name)) == NULL)
	    {
	    free (new_button);
	    error (u_str ("No memory to create button \"%s\""), name);
	    return NULL;
	    }
	if ((new_button->keys = savestr (keys)) == NULL)
	    {
	    free (new_button->name);
	    free (new_button);
	    error (u_str ("No memory to create button \"%s\""), name);
	    return NULL;
	    }	    
	new_button->pos = pos;
	new_button->callback = callback;
	for (cur = parent->cascade.children, prev = NULL; cur != NULL; prev = cur, cur = cur->any.next)
	    if (cur->any.pos > pos)
		break;
	menu_cur = cur;
	if (keys[0] == '-' && keys[1] == 0)
	    {
	    new_button->button = button = XmCreateSeparatorGadget (parent->cascade.menu, s_str (name), args, 0);
	    new_button->type = SEPARATOR;
	    }
	else
	    {
	    argc = 0;
	    if (mn != 0)
		XtSetArg (args[argc], XmNmnemonic, mn), argc++;
	    new_button->button = button = XmCreatePushButtonGadget (parent->cascade.menu, s_str (name), args, argc);
	    XtAddCallback (button, XmNactivateCallback, menu_to_keys, (XtPointer)new_button);
	    }
	parent->cascade.ccount++;
	new_button->next = cur;
	if (prev != NULL)
	    prev->any.next = (union any_menu *)new_button;
	else
	    parent->cascade.children = (union any_menu *)new_button;
	XtManageChild (button);
	}
    else
	error (u_str ("No memory to create button \"%s\""), name);

    return (union any_menu *)new_button;
    }

/* Create a cascade button in a menu pane */
static union any_menu *mkcascade (union any_menu *parent, unsigned char *name, int pos, int mn)
    {
    Arg args[2];
    int argc, is_help;
    Widget pulldown, cascade;
    struct cascade *new_cascade = NULL;
    union any_menu *prev, *cur;

    if (parent->any.type != CASCADE)
	error (u_str ("Parent of button \"%s\" must be a cascade menu"), name);
    else if (parent->cascade.ccount >= MAX_CHILDREN)
	error (u_str ("Too many buttons in menu \"%s\""), parent->cascade.name);
    else if( (new_cascade = (struct cascade *)malloc (sizeof (struct cascade), malloc_type_char)) != NULL )
	{
	new_cascade->type = CASCADE;
	if ((new_cascade->name = savestr (name)) == NULL)
	    {
	    free (new_cascade);
	    error (u_str ("No memory to create cascade button \"%s\""), name);
	    return NULL;
	    }
	is_help = parent == &menu_bar && _str_icmp (new_cascade->name, u_str ("HELP")) == 0;
	new_cascade->children = NULL;
	if (is_help)
	    pos = 0x7FFFFFFF;
	new_cascade->pos = pos;
	new_cascade->ccount = 0;
	for (cur = parent->cascade.children, prev = NULL; cur != NULL; prev = cur, cur = cur->any.next)
	    if (cur->any.pos > pos)
		break;
	menu_cur = cur;
	argc = 0;
	if (!is_help)
	    XtSetArg (args[0], XmNinsertPosition, insert_pos), argc++;
	new_cascade->menu = pulldown = XmCreatePulldownMenu (parent->cascade.menu, s_str (name), args, argc);
	argc = 0;
	XtSetArg (args[0], XmNsubMenuId, pulldown), argc++;
	if (mn != 0)
	    XtSetArg (args[argc], XmNmnemonic, mn), argc++;
	new_cascade->button = cascade = XmCreateCascadeButtonGadget (parent->cascade.menu, s_str (name), args, argc);
	if (is_help)
	    {
	    XtSetArg (args[0], XmNmenuHelpWidget, new_cascade->button);
	    XtSetValues (parent->cascade.menu, args, 1);
	    }
	parent->cascade.ccount++;
	new_cascade->next = cur;
	if (prev != NULL)
	    prev->any.next = (union any_menu *)new_cascade;
	else
	    parent->cascade.children = (union any_menu *)new_cascade;
	XtManageChild (cascade);
	}
    else
	error (u_str ("No memory to create cascade button \"%s\""), name);

    return (union any_menu *)new_cascade;
    }

/* Remove a menu button and all of its subordinates */
static void rmmenustruct (union any_menu *item, int all)
    {
    union any_menu *next;

    do
	{
	next = item->any.next;
	switch (item->any.type)
	    {
	    case CASCADE:
		if (item->cascade.children != NULL)
		    rmmenustruct (item->cascade.children, 1);
		free (item->cascade.name);
		break;

	    case BUTTON:
		free (item->button.name);
		free (item->button.keys);
		break;

	    case SEPARATOR:
		free (item->button.name);
		break;

	    default:
		_dbg_msg (u_str ("Unknown type %d for menu \"%s\""), item->any.type, item->any.name);
		return;
	    }
	free (item);
	item = next;
	}
    while (all && next != NULL);
    }

/* Remove a menu and all of its submeny items from a menu */
int motif_remove_menu_command (void)
    {
    struct prognode *p = cur_exec;
    int i;
    union any_menu *menu, *cur, *prev;

    if (!is_motif)
	return motif_not_running ();

    if (!check_args (1, 1000000))
	{
	/* Traverse the menu tree to find the root button to be terminated */
	for (i = 1, cur = &menu_bar; i <= p->p_nargs; i++)
	    {
	    unsigned char *name;

	    menu = cur;
	    if (string_arg (i) == 0)
		return 0;
	    name = ml_value->exp_v.v_string;
	    if (menu->any.type != CASCADE)
		{
		error (u_str ("Parent of button \"%s\" must be a cascade menu"), name);
		return 0;
		}
	    for (prev = NULL, cur = menu->cascade.children; cur != NULL; prev = cur, cur = cur->any.next)
		if (_str_cmp (name, cur->any.name) == 0)
		    break;
	    if (cur == NULL)
		{
		error (u_str ("No such menu as \"%s\""), name);
		return 0;
		}
	    release_expr (ml_value);
	    }
	/* Remove the entry from the siblings */
	if (prev == NULL)
	    menu->cascade.children = cur->any.next;
	else
	    prev->any.next = cur->any.next;
	/* Destroy the wigets
	if (cur->any.type == CASCADE)
	    XtDestroyWidget (cur->cascade.menu);
	*/
	XtDestroyWidget (cur->any.button);
	/* Destroy the any_menu tree */
	rmmenustruct (cur, 0);
	}
    return 0;    
    }

/* Extended command to add a menu button */
int motif_add_menu_command (void)
    {
    struct prognode *p = cur_exec;
    int min, i;
    unsigned char *keys;
    union any_menu *menu, *cur;;

    if (!is_motif)
	return motif_not_running ();

    min = (p->p_nargs & 1) ? 3 : cur_exec->p_nargs + 1;
    if (!check_args (min, 1000000))
	if (string_arg (1))
	    {
	    keys = savestr (ml_value->exp_v.v_string);
	    release_expr (ml_value);

	    /* Build the buttons */
	    for (i = 2, menu = &menu_bar; i <= p->p_nargs; i += 2, menu = cur)
		{
		int pos;
		unsigned char name[MAX_LABEL], *s, *d, mn;

		pos = numeric_arg (i);
		if (err || string_arg (i + 1) == 0)
		    {
		    free (keys);
		    return 0;
		    }		    
		if (ml_value->exp_int > MAX_LABEL)
		    {
		    free (keys);
		    error (u_str ("The name of button \"%s\" is too long"), ml_value->exp_v.v_string);
		    return 0;		    
		    }
		for (s = ml_value->exp_v.v_string, d = name, mn = 0; *s; s++)
		    if (s[0] == '&' && s[1] != 0)
			mn = s[1];
		    else
			*d++ = s[0];
		*d = 0;
		release_expr (ml_value);
		if (menu->any.type != CASCADE)
		    {
		    free (keys);
		    error (u_str ("Parent of button \"%s\" must be a cascade menu"), name);
		    return 0;
		    }
		for (cur = menu->cascade.children; cur != NULL; cur = cur->any.next)
		    if (_str_cmp (name, cur->any.name) == 0)
			break;
		if (i >= p->p_nargs - 2)
		    {
		    if (cur != NULL)
			{
			error (u_str ("Button \"%s\" already exists"), name);
			free (keys);
			release_expr (ml_value);
			return 0;
			}
		    else
			cur = mkbutton (menu, name, pos, mn, keys, NULL);
		    }
		else
		    if (cur == NULL)
			cur = mkcascade (menu, name, pos, mn);
		if (cur == NULL)
		    {
		    free (keys);
		    return 0;
		    }
		}
	    }
    return 0;    
    }

/* Build the builtin menus */
void xwin_menu_init (Widget menu_bar_widget)
    {
    union any_menu *item, *item1;

    menu_bar.cascade.type = CASCADE;
    menu_bar.cascade.name = u_str ("menu_bar");
    menu_bar.cascade.menu = menu_bar_widget;
    menu_bar.cascade.ccount = 0;

    /* File menu */
    item = mkcascade (&menu_bar, u_str ("File"), 10, 'F');
    mkbutton (item, u_str ("New..."), 10, 'N', u_str ("\202Fn"), NULL);
    mkbutton (item, u_str ("Open..."), 20, 'O', u_str ("\202Fo"), NULL);
    mkbutton (item, u_str ("Include..."), 30, 'I', u_str ("\202Fi"), NULL);
    mkbutton (item, u_str ("Sep"), 40, 0, u_str ("-"), NULL);
    mkbutton (item, u_str ("Save"), 50, 'S', u_str ("\202Fs"), NULL);
    mkbutton (item, u_str ("Save As..."), 60, 'A', u_str ("\202Fa"), NULL);
    mkbutton (item, u_str ("Save All"), 70, 'v', u_str ("\202Fw"), NULL);
    mkbutton (item, u_str ("Sep1"), 80, 0, u_str ("-"), NULL);
    mkbutton (item, u_str ("Quit"), 90, 'Q', u_str ("\202Fq"), quit_cb);
    mkbutton (item, u_str ("Exit"), 100, 'E', u_str ("\202Fe"), NULL);

    /* Edit menu */
    item = mkcascade (&menu_bar, u_str ("Edit"), 20, 'E');
    mkbutton (item, u_str ("Undo"), 10, 'U', u_str ("\202Eu"), NULL);
    mkbutton (item, u_str ("Undo..."), 15, 'n', u_str ("\202Em"), NULL);
    mkbutton (item, u_str ("Sep"), 20, 0, u_str ("-"), NULL);
    mkbutton (item, u_str ("Cut"), 30, 't', u_str ("\202Et"), NULL);
    mkbutton (item, u_str ("Copy"), 40, 'C', u_str ("\202Ec"), NULL);
    mkbutton (item, u_str ("Paste"), 50, 'P', u_str ("\202Ep"), NULL);
    mkbutton (item, u_str ("Clear"), 60, 'e', u_str ("\202Ee"), NULL);
    mkbutton (item, u_str ("Set Mark"), 60, 'S', u_str ("\202Es"), NULL);

    /* Navigate menu */
    item = mkcascade (&menu_bar, u_str ("Navigate"), 30, 'N');
    item1 = mkcascade (item, u_str ("Find"), 10, 'F');
    mkbutton (item1, u_str ("Next"), 10, 'N', u_str ("\202Nfn"), NULL);
    mkbutton (item1, u_str ("Previous"), 10, 'P', u_str ("\202Nfp"), NULL);
    mkbutton (item, u_str ("Search..."), 20, 'S', u_str ("\202Ns"), NULL);
    mkbutton (item, u_str ("Replace..."), 30, 'R', u_str ("\202Nr"), NULL);
    mkbutton (item, u_str ("Sep"), 40, 0, u_str ("-"), NULL);
    item1 = mkcascade (item, u_str ("Advance To"), 50, 'A');
    mkbutton (item1, u_str ("Next Page"), 10, 'P', u_str ("\202Nap"), NULL);
    mkbutton (item1, u_str ("End of File"), 20, 'F', u_str ("\202Nae"), NULL);
    mkbutton (item1, u_str ("Sep"), 30, 0, u_str ("-"), NULL);
    mkbutton (item1, u_str ("Next Line"), 40, 'L', u_str ("\202Nal"), NULL);
    mkbutton (item1, u_str ("Next Sentence"), 50, 'S', u_str ("\202Nas"), NULL);
    mkbutton (item1, u_str ("Next Paragraph"), 60, 'g', u_str ("\202Nag"), NULL);
    item1 = mkcascade (item, u_str ("Backup To"), 50, 'B');
    mkbutton (item1, u_str ("Previous Page"), 10, 'P', u_str ("\202Npp"), NULL);
    mkbutton (item1, u_str ("Beginning of File"), 20, 'F', u_str ("\202Npb"), NULL);
    mkbutton (item1, u_str ("Sep"), 30, 0, u_str ("-"), NULL);
    mkbutton (item1, u_str ("Previous Line"), 40, 'L', u_str ("\202Npl"), NULL);
    mkbutton (item1, u_str ("Previous Sentence"), 50, 'S', u_str ("\202Nps"), NULL);
    mkbutton (item1, u_str ("Previous Paragraph"), 60, 'g', u_str ("\202Npg"), NULL);

    /* View menu */
    item = mkcascade (&menu_bar, u_str ("View"), 40, 'V');
    mkbutton (item, u_str ("Split Window"), 10, 'S', u_str ("\202V2"), NULL);
    mkbutton (item, u_str ("Delete Window"), 20, 'D', u_str ("\202Vd"), NULL);
    mkbutton (item, u_str ("One Window"), 30, 'O', u_str ("\202Vo"), NULL);
    mkbutton (item, u_str ("Pop Buffer"), 30, 'P', u_str ("\202Vp"), NULL);
    mkbutton (item, u_str ("Switch Buffer"), 30, 'B', u_str ("\202Vs"), NULL);
    mkbutton (item, u_str ("Refresh"), 40, 'R', u_str ("\202Vr"), NULL);

    /* Help menu */
    item = mkcascade (&menu_bar, u_str ("Help"), 99999, 'H');
    mkbutton (item, u_str ("On Commands"), 10, 'C', u_str ("\202Hc"), NULL);
    mkbutton (item, u_str ("On Variables"), 20, 'V', u_str ("\202Ho"), NULL);
    mkbutton (item, u_str ("On Word"), 25, 'W', u_str ("\202Hw"), NULL);
    mkbutton (item, u_str ("Apropos"), 30, 'A', u_str ("\202Ha"), NULL);
    mkbutton (item, u_str ("Sep"), 40, 0, u_str ("-"), NULL);
    mkbutton (item, u_str ("Help"), 50, 'H', u_str ("\202Hh"), NULL);
    mkbutton (item, u_str ("Sep1"), 60, 0, u_str ("-"), NULL);
    mkbutton (item, u_str ("On Version"), 70, 0, u_str ("\202Hv"), NULL);
    }
#endif
