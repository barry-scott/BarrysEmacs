/* 
 * Motif X-Window system widget toolkit
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
#include <Xm/LabelG.h>
#include <Xm/PushBG.h>
#include <Xm/ToggleBG.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/TextF.h>
#include <Xm/SeparatoG.h>
#include <Xm/FileSB.h>
#include <Xm/MessageB.h>

/*
 * Data Types:
 */

#define MAX_ARGS 20

#define CB_CANCEL 0
#define CB_APPLY 1
#define CB_OK 2

#define DIALOG_FORM 0
#define DIALOG_FILE 1
#define DIALOG_MESSAGE 2

struct emacs_widget
    {
    struct emacs_widget *next;		    /* Next sibling in sequence */
    Widget w;				    /* Widget ID */
    unsigned char *var;			    /* Name of variable for widget */
    int (*fetch)(struct emacs_widget *);    /* Fetch for widget callback */
    int (*store)(struct emacs_widget *);    /* Store from widget callback */
    };

struct emacs_dialog
    {
    struct emacs_dialog *next;	    /* Next widget */
    unsigned char *name;	    /* Name of the dialog */
    unsigned char *apply_keys;	    /* Key sequence to generate for unsolicited apply */
    int type;			    /* Type of dialog */
    Widget widget;		    /* Widget for the dialog */
    Widget dialog;		    /* Widget for the dialog shell */
    int inuse;			    /* True if the dialog is in use */
    int incomplete;		    /* True if the dialog is not complete */
    int result;			    /* Result of the dialog pop-up */
    struct emacs_widget *children;  /* Widget children of dialog */
    };

/*
 * Own Storage:
 */

static unsigned char no_mem [] = "No memory for %s \"%s\"";
static unsigned char already_exists [] = "Motif widget \"%s\" already exists";
static unsigned char motif_problem [] = "Probelm interacting with Motif for \"%s\"";
static unsigned char doesnt_exist [] = "Dialog \"%s\" does not exists";
static unsigned char inuse [] = "Dialog \"%s\" is in use";
static unsigned char root_active [] = "Already building dialog \"%s\"";
static unsigned char root_inactive [] = "Not building a dialog that can contain %s";
static unsigned char must_be_var [] = "%s expects argument %d to be a variable name";
static int x_pos = 0, y_pos = 0;
static struct emacs_dialog *dialogs = NULL;
static XmStringCharSet charset = (XmStringCharSet) XmSTRING_DEFAULT_CHARSET;
static struct emacs_dialog *root_dialog = NULL;
static Widget root;
static int root_seq = 0;
int motif_insensitive = 0;
static char *dialog_names [] =
    {
    "form dialog",
    "file selection dialog",
    "message dialog"
    };
/*
 * External References:
 */

extern Widget app_shell;
extern int is_motif;
extern int xwin_readsocket (struct  trmcontrol *, unsigned char *, unsigned int);
extern int fetch_var (unsigned char *, int *, unsigned char **);
extern void set_var (unsigned char *name, int iresp, unsigned char *cresp);
extern void input_char_string (char *key);
extern int motif_not_running (void);

/* Find a dialog in the list */
static struct emacs_dialog *find_dialog (unsigned char *name)
    {
    struct emacs_dialog *w;

    for (w = dialogs; w; w = w->next)
	if (_str_cmp (name, w->name) == 0)
	    return w;

    return NULL;
    }

static struct emacs_dialog *make_dialog (unsigned char *name)
    {
    struct emacs_dialog *w = NULL;

    if ((w = (struct emacs_dialog *)malloc (sizeof (struct emacs_dialog), malloc_type_char)) == NULL)
	error (no_mem, "dialog", name);
    else
	if ((w->name = savestr (name)) == NULL)
	    {
	    error (no_mem, "dialog", name);
	    free (w);
	    }
	else
	    {
	    w->next = dialogs;
	    dialogs = w;
	    w->widget = NULL;
	    w->dialog = NULL;
	    w->inuse = 0;
	    w->incomplete = 0;
	    w->children = 0;
	    }
    return w;
    }

static void destroy_dialog (unsigned char *name)
    {
    struct emacs_dialog *w, *prev;
    struct emacs_widget *p, *q;

    for (prev = NULL, w = dialogs; w; prev = w, w = w->next)
	if (_str_cmp (name, w->name) == 0)
	    {
	    if (prev == NULL)
		dialogs = w->next;
	    else
		prev->next = w->next;
	    free (w->name);
	    if (w->apply_keys)
		free (w->apply_keys);
	    for (p = w->children; p; p = q)
		{
		if (p->var)
		    free (p->var);
		q = p->next;
		free (p);
		}
	    free (w);
	    }
    }

static void ok_cb (Widget PNOTUSED(w), void *d_, void *PNOTUSED(junk))
    {
    struct emacs_dialog *d = (struct emacs_dialog *)d_;
    if (d->inuse)
	{	
	d->incomplete = 0;
	d->result = CB_OK;
	d->inuse = 0;
	}
    else
	if (d->apply_keys)
	    {
	    int old_err = err;
	    struct emacs_widget *p;

	    input_char_string (s_str (d->apply_keys));
	    for (p = d->children; p && !err; p = p->next)
		if (p->store)
		    (*p->store)(p);
	    err = old_err;
	    }
    XtUnmanageChild (d->widget);
    }

static void apply_cb (Widget PNOTUSED(w), void *d_, void *PNOTUSED(junk))
    {
    struct emacs_dialog *d = (struct emacs_dialog *)d_;
    if (d->inuse)
	{	
	d->incomplete = 0;
	d->result = CB_APPLY;
	}
    else
	if (d->apply_keys)
	    {
	    int old_err = err;
	    struct emacs_widget *p;

	    input_char_string (s_str (d->apply_keys));
	    for (p = d->children; p && !err; p = p->next)
		if (p->store)
		    (*p->store)(p);
	    err = old_err;
	    }
    }

static void cancel_cb (Widget PNOTUSED(w), void *d_, void *PNOTUSED(junk))
    {
    struct emacs_dialog *d = (struct emacs_dialog *)d_;
    if (d->inuse)
	{	
	d->incomplete = 0;
	d->result = CB_CANCEL;
	d->inuse = 0;
	}
    XtUnmanageChild (d->widget);
    }

static void popdown_cb (Widget PNOTUSED(w), void *d_, void *PNOTUSED(junk))
    {
    struct emacs_dialog *d = (struct emacs_dialog *)d_;
    if (d->inuse)
	{	
	d->incomplete = 0;
	d->result = CB_CANCEL;
	}
    }

/* Create a named XmForm dialog shell */
int motif_mk_form_dialog (void)
    {
    struct emacs_dialog *w;
    struct prognode *prg = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root != NULL)
	error (root_active, root_dialog->name);
    else if (!check_args (5, 1000000))
	{
	if (string_arg (1) == 0)
	    return 0;
	if (find_dialog (ml_value->exp_v.v_string))
	    error (already_exists, ml_value->exp_v.v_string);
	else
	    {
	    int argc, width, height, x, y;
	    Arg args[MAX_ARGS];

	    if ((w = make_dialog (ml_value->exp_v.v_string)) == NULL)
		return 0;
	    w->type = DIALOG_FORM;
	    w->apply_keys = NULL;
	    release_expr (ml_value);
	    if (string_arg (2) == 0)
		{
		destroy_dialog (w->name);
		return 0;		
		}
	    if (*ml_value->exp_v.v_string != 0 &&
	      (w->apply_keys = savestr (ml_value->exp_v.v_string)) == NULL)
		{
		error (no_mem, "dialog", w->name);
		destroy_dialog (w->name);
		return 0;
		}
	    width = numeric_arg (3);
	    if (!err) height = numeric_arg (4);
	    if (!err)
		{
		x = app_shell->core.x;
		y = app_shell->core.y;
		x_pos = (x_pos + 1) & 7;
		y_pos = (y_pos + 1) & 3;
		XtSetArg (args[0], XmNx, x + ((x_pos + 1) * 10)), argc = 1;
		XtSetArg (args[argc], XmNy, y + ((y_pos + 1) * 30)), argc++;
		XtSetArg (args[argc], XmNwidth, width), argc++;
		XtSetArg (args[argc], XmNheight, height), argc++;		
		XtSetArg (args[argc], XmNautoUnmanage, 0), argc++;		
		XtSetArg (args[argc], XmNdefaultPosition, 0), argc++;		
		w->widget = XmCreateFormDialog (app_shell, s_str (w->name), args, argc);
		if (w->widget == 0)
		    {
		    error (motif_problem, w->name);
		    destroy_dialog (w->name);
		    }
		else
		    {
		    Widget b;
		    int delta_l;

		    w->dialog = XtParent (w->widget);
		    XtAddCallback (w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w);
		    XtSetArg (args[0], XmNtitle, w->name), argc = 1;
		    XtSetArg (args[argc], XmNwindowGroup, XtUnspecifiedWindowGroup), argc++;		
		    XtSetValues (w->dialog, args, argc);

		    /* Create the OK button */
		    XtSetArg (args[0], XmNshowAsDefault, 1);
		    XtSetArg (args[1], XmNleftOffset, delta_l = 15);
		    XtSetArg (args[2], XmNbottomOffset, 12);
		    XtSetArg (args[3], XmNleftAttachment, XmATTACH_FORM);
		    XtSetArg (args[4], XmNbottomAttachment, XmATTACH_FORM);
		    XtSetArg (args[5], XmNwidth, 70);
		    if( (b = XmCreatePushButtonGadget (w->widget, "OK", args, 6)) != NULL )
			{
			XtManageChild (b);
			XtAddCallback (b, XmNactivateCallback, ok_cb, (XtPointer)w);
			XtSetArg (args[0], XmNdefaultButton, b);
			XtSetValues (w->widget, args, 1);

			/* Create the Apply button if required */
			if (w->apply_keys != NULL)
			    {
			    XtSetArg (args[0], XmNleftOffset, delta_l = 110);
			    XtSetArg (args[1], XmNbottomOffset, 13);
			    XtSetArg (args[2], XmNleftAttachment, XmATTACH_FORM);
			    XtSetArg (args[3], XmNbottomAttachment, XmATTACH_FORM);
			    XtSetArg (args[4], XmNwidth, 70);
			    if ((b = XmCreatePushButtonGadget (w->widget, "Apply", args, 5)) == NULL)
				{
				error (motif_problem, w->name);
				XtDestroyWidget (w->dialog);
				destroy_dialog (w->name);
				return 0;
				}
			    XtManageChild (b);
			    XtAddCallback (b, XmNactivateCallback, apply_cb, (XtPointer)w);
			    }

			/* Create the Cancel button */
			XtSetArg (args[0], XmNleftOffset, delta_l + 95);
			XtSetArg (args[1], XmNbottomOffset, 13);
			XtSetArg (args[2], XmNleftAttachment, XmATTACH_FORM);
			XtSetArg (args[3], XmNbottomAttachment, XmATTACH_FORM);
			XtSetArg (args[4], XmNwidth, 70);
			if( (b = XmCreatePushButtonGadget (w->widget, "Cancel", args, 5)) != NULL )
			    {
			    int i;

			    XtManageChild (b);
			    XtAddCallback (b, XmNactivateCallback, cancel_cb, (XtPointer)w);
			    root = w->widget;
			    root_dialog = w;
			    for (i = 5; i <= prg->p_nargs && !err; i++)
				eval_arg (i);
			    root = NULL;
			    root_dialog = NULL;
			    if (err)
				{
				XtDestroyWidget (w->dialog);
				destroy_dialog (w->name);
				}
			    else
				void_result ();
			    }
			else
			    {
			    error (motif_problem, w->name);
			    XtDestroyWidget (w->dialog);
			    destroy_dialog (w->name);
			    }
			}
		    else
			{
			error (motif_problem, w->name);
			XtDestroyWidget (w->dialog);
			destroy_dialog (w->name);
			}
		    }
		}
	    }
	    
	}
    return 0;
    }

/* Operate a dialog */
int motif_display (void)
    {
    unsigned char *name;
    struct emacs_dialog *w;
    struct emacs_widget *p;

    if (!is_motif)
	return motif_not_running ();

    if( (name = getstr (u_str (": XmOperateDialog "))) != NULL )
	{
	if (!(w = find_dialog (name)))
	    error (doesnt_exist, name);
	else
	    if (w->inuse)
		error (inuse, name);
	    else
		if (w->apply_keys)
		    {
		    for (p = w->children; p && !err; p = p->next)
			if (p->fetch)
			    (*p->fetch)(p);
		    if (!err)
			XtManageChild (w->widget);
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = CB_CANCEL;
		    }
		else
		    {
		    w->inuse = 1;
    		    for (p = w->children; p && !err; p = p->next)
			if (p->fetch)
			    (*p->fetch)(p);
		    if (!err)
			{
			XtManageChild (w->widget);
			motif_insensitive = 1;
			for (w->incomplete = 1; w->incomplete; xwin_readsocket (0, NULL, 0));
			motif_insensitive = 0;
			if (w->result != CB_CANCEL)
			    for (p = w->children; p && !err; p = p->next)
				if (p->store)
				    (*p->store)(p);
			}
		    w->inuse = 0;
    		    void_result ();
		    ml_value->exp_type = ISINTEGER;
    		    ml_value->exp_int = (int)w->result;
		    }

	}
    return 0;
    }

/* Destroy a dialog */
int motif_destroy (void)
    {
    unsigned char *name;
    struct emacs_dialog *w;

    if (!is_motif)
	return motif_not_running ();

    if( (name = getstr (u_str (": XmDestroyDialog "))) != NULL )
	{
	if (!(w = find_dialog (name)))
	    error (doesnt_exist, name);
	else
	    if (w->inuse)
		error (inuse, name);
	    else
		{
		XtDestroyWidget (w->dialog);
		destroy_dialog (name);
		void_result ();
		}
	}
    return 0;
    }

/* list all dialogs */
int motif_dump (void)
    {
    struct emacs_dialog *p;
    struct emacs_buffer *old = bf_cur;
    unsigned char line[100];

    if (!is_motif)
	return motif_not_running ();

    scratch_bfn (u_str ("Dialog table"), interactive);
    ins_str (u_str ("MOTIF Dialogs\n"));
    ins_str (u_str ("-------------\n\n"));
    for (p = dialogs; p; p = p->next)
	{
	ins_str (p->name);
	sprintfl (line, sizeof (line), u_str (" is a %s\n"), dialog_names[p->type]);
	ins_str (line);
	}
    bf_cur->b_checkpointed = -1;
    bf_modified = 0;
    set_dot (1);
    set_bfp (old);
    window_on (bf_cur);

    return 0;
    }

/* Create a label gadget */
int motif_label (void)
    {
    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a label");
    else
        if (!check_args (3, 3))
	    if (string_arg (3))
		{
		XmString p;

		if ((p = XmStringCreateLtoR (s_str (ml_value->exp_v.v_string), charset)) == NULL)
		    error (motif_problem, "label");
		else
		    {
		    int x, y;
		    x = numeric_arg (1);
		    if (!err) y = numeric_arg (2);
		    if (err)
			XmStringFree (p);
		    else
			{
			Arg args[5];
			Widget b;
			int argc = 0;
			char name[20];

			XtSetArg (args[argc], XmNlabelString, p), argc++;
			if (x >= 0)
			    {
			    XtSetArg (args[argc], XmNleftOffset, x), argc++;
			    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
			    }
			if (y >= 0)
			    {
			    XtSetArg (args[argc], XmNtopOffset, y), argc++;
			    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
			    }
			sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
			if( (b = XmCreateLabelGadget (root, name, args, argc)) != NULL )
			    {
			    XtManageChild (b);
			    XmStringFree (p);
			    void_result ();
			    ml_value->exp_type = ISINTEGER;
			    ml_value->exp_int = (int)b;
			    }
			else
			    {
			    error (motif_problem, "label");
			    XmStringFree (p);
			    }
			}
		    }		
		}
    return 0;
    }

/* Fetch callback for toggle button */
static int toggle_button_fetch (struct emacs_widget *w)
    {
    int var;
    if (fetch_var (w->var, &var, NULL))
	{
	XmToggleButtonGadgetSetState (w->w, var != 0, 1);
	}
    return 0;
    }

/* Store callback for toggle button */
static int toggle_button_store (struct emacs_widget *w)
    {
    set_var (w->var, XmToggleButtonGadgetGetState (w->w), NULL);
    return 0;
    }

/* Create a toggle button gadget */
int motif_toggle_button (void)
    {
    struct prognode *prg;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a toggle button");
    else
        if (!check_args (4, 4))
	    {
	    prg = cur_exec->p_args[3].pa_node;
	    if (prg->p_proc != &bound_variable_node)
		error (must_be_var, "XmToggleButton", 4);
	    else if (string_arg (3))
		{
		struct emacs_widget *w;
		XmString p;

		if ((w = (struct emacs_widget *)malloc (sizeof (struct emacs_widget), malloc_type_char)) == NULL)
		    {
		    error (no_mem, "toggle button");
		    return 0;
		    }
		if ((p = XmStringCreateLtoR (s_str (ml_value->exp_v.v_string), charset)) == NULL)
		    error (motif_problem, "toggle button");
		else
		    {
		    int x, y;
			
		    x = numeric_arg (1);
		    if (!err) y = numeric_arg (2);
		    if (err)
			{
			XmStringFree (p);
			free (w);
			}
		    else
			{
			w->fetch = toggle_button_fetch;
			w->store = toggle_button_store;
			w->var = savestr (prg->p_args[0].pa_name->v_name);
			if (w->var == NULL)
			    {
			    error (no_mem, prg->p_args[0].pa_name->v_name);
			    XmStringFree (p);
			    free (w);
			    }
			else
			    {
			    Arg args[6];
			    Widget b;
			    int argc = 0;
			    char name[20];

			    XtSetArg (args[argc], XmNlabelString, p), argc++;
			    if (x >= 0)
				{
				XtSetArg (args[argc], XmNleftOffset, x), argc++;
				XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
				}
			    if (y >= 0)
				{
				XtSetArg (args[argc], XmNtopOffset, y), argc++;
				XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
				}
			    sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
			    if( (b = XmCreateToggleButtonGadget (root, name, args, argc)) != NULL )
				{
				XtManageChild (b);
				XmStringFree (p);
				w->next = root_dialog->children;
				w->w = b;
				root_dialog->children = w;
				void_result ();
				ml_value->exp_type = ISINTEGER;
				ml_value->exp_int = (int)b;
				}
			    else
				{
				error (motif_problem, "toggle button");
				XmStringFree (p);
				free (w);
				}
			    }
			}
		    }		
		}
	    }
    return 0;
    }

/* Create an empty radio box */
int motif_radio_box (void)
    {
    struct prognode *p = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a radio box");
    else
        if (!check_args (0, 10000))
	    {
	    int x , y, border;
	    x = p->p_nargs > 0 ? numeric_arg (1) : -1;
	    if (!err) y = p->p_nargs > 1 ? numeric_arg (2) : -1;
	    if (!err) border = p->p_nargs > 2 ? numeric_arg (3) : -1;
	    if (!err)
		{
		Arg args[5];
		Widget b;
		int argc = 0;
		char name[20];

		if (x >= 0)
		    {
		    XtSetArg (args[argc], XmNleftOffset, x), argc++;
		    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
		    }
		if (y >= 0)
		    {
		    XtSetArg (args[argc], XmNtopOffset, y), argc++;
		    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
		    }
		if (border >= 0)
		    XtSetArg (args[argc], XmNborderWidth, border > 0 ? 1 : 0), argc++;
		sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
		if( (b = XmCreateRadioBox (root, name, args, argc)) != NULL )
		    {
		    int i;
		    Widget old_root = root;

		    XtManageChild (b);

		    root = b;
		    for (i = 4; i <= p->p_nargs && !err; i++)
			eval_arg (i);
		    root = old_root;
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = (int)b;
		    }
		else
		    error (motif_problem, "radio box");
		}
	    }		
    return 0;
    }

/* Create an empty check box */
int motif_check_box (void)
    {
    struct prognode *p = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a check box");
    else
        if (!check_args (0, 10000))
	    {
	    int x , y, border;
	    x = p->p_nargs > 0 ? numeric_arg (1) : -1;
	    if (!err) y = p->p_nargs > 1 ? numeric_arg (2) : -1;
	    if (!err) border = p->p_nargs > 2 ? numeric_arg (3) : -1;
	    if (!err)
		{
		Arg args[7];
		Widget b;
		int argc = 0;
		char name[20];

		if (x >= 0)
		    {
		    XtSetArg (args[argc], XmNleftOffset, x), argc++;
		    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
		    }
		if (y >= 0)
		    {
		    XtSetArg (args[argc], XmNtopOffset, y), argc++;
		    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
		    }
		if (border >= 0)
		    XtSetArg (args[argc], XmNborderWidth, border > 0 ? 1 : 0), argc++;
		XtSetArg (args[argc], XmNentryClass, xmToggleButtonGadgetClass), argc++;
		XtSetArg (args[argc], XmNisHomogeneous, 1), argc++;
		sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
		if( (b = XmCreateRowColumn (root, name, args, argc)) != NULL )
		    {
		    int i;
		    Widget old_root = root;

		    XtManageChild (b);

		    root = b;
		    for (i = 4; i <= p->p_nargs && !err; i++)
			eval_arg (i);
		    root = old_root;
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = (int)b;
		    }
		else
		    error (motif_problem, "check box");
		}
	    }		
    return 0;
    }

/* Fetch callback for scale */
static int scale_fetch (struct emacs_widget *w)
    {
    int var;
    if (fetch_var (w->var, &var, NULL))
	{
	XmScaleSetValue (w->w, var);
	}
    return 0;
    }

/* Store callback for scale */
static int scale_store (struct emacs_widget *w)
    {
    int var;
    XmScaleGetValue (w->w, &var);
    set_var (w->var, var, NULL);
    return 0;
    }

/* Create a scale Widget */
int motif_scale (void)
    {
    struct prognode *prg;
    int titled;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a scale");
    else
        if (!check_args (10, 10))
	    {
	    prg = cur_exec->p_args[9].pa_node;
	    if (prg->p_proc != &bound_variable_node)
		error (must_be_var, "XmScale", 10);
	    else if (string_arg (3))
		{
		struct emacs_widget *w;
		XmString p;

		titled = ml_value->exp_int;
		if ((w = (struct emacs_widget *)malloc (sizeof (struct emacs_widget), malloc_type_char)) == NULL)
		    {
		    error (no_mem, "scale");
		    return 0;
		    }
		if (titled && (p = XmStringCreateLtoR (s_str (ml_value->exp_v.v_string), charset)) == NULL)
		    error (motif_problem, "scale");
		else
		    {
		    int x, y, mn, mx, is_horz, show, width, height;
			
		    x = numeric_arg (1);
		    if (!err) y = numeric_arg (2);
		    if (!err) width = numeric_arg (4);
		    if (!err) height = numeric_arg (5);
		    if (!err) mn = numeric_arg (6);
		    if (!err) mx = numeric_arg (7);
		    if (!err) is_horz = numeric_arg (8);
		    if (!err) show = numeric_arg (9);
		    if (err)
			{
			if (titled) XmStringFree (p);
			free (w);
			}
		    else
			{
			w->fetch = scale_fetch;
			w->store = scale_store;
			w->var = savestr (prg->p_args[0].pa_name->v_name);
			if (w->var == NULL)
			    {
			    error (no_mem, prg->p_args[0].pa_name->v_name);
			    if (titled) XmStringFree (p);
			    free (w);
			    }
			else
			    {
			    Arg args[12];
			    Widget b;
			    int argc = 0;
			    char name[20];

			    if (titled)
				XtSetArg (args[argc], XmNtitleString, p), argc++;
			    if (x >= 0)
				{
				XtSetArg (args[argc], XmNleftOffset, x), argc++;
				XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
				}
			    if (y >= 0)
				{
				XtSetArg (args[argc], XmNtopOffset, y), argc++;
				XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
				}
			    if (width >= 0)
				XtSetArg (args[argc], XmNscaleWidth, width), argc++;
			    if (height >= 0)
				XtSetArg (args[argc], XmNscaleHeight, height), argc++;
			    if (mn >= 0)
				XtSetArg (args[argc], XmNminimum, mn), argc++;
			    if (mx >= 0)
				XtSetArg (args[argc], XmNmaximum, mx), argc++;
			    if (is_horz > 0)
				XtSetArg (args[argc], XmNorientation, XmHORIZONTAL), argc++;
			    if (show >= 0)
				XtSetArg (args[argc], XmNshowValue, show), argc++;
			    sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
			    if( (b = XmCreateScale (root, name, args, argc)) != NULL )
				{
				XtManageChild (b);
				if (titled) XmStringFree (p);
				w->next = root_dialog->children;
				w->w = b;
				root_dialog->children = w;
				void_result ();
				ml_value->exp_type = ISINTEGER;
				ml_value->exp_int = (int)b;
				}
			    else
				{
				error (motif_problem, "scale");
				if (titled)XmStringFree (p);
				free (w);
				}
			    }
			}
		    }		
		}
	    }
    return 0;
    }

/* Fetch callback for text field */
static int text_field_fetch (struct emacs_widget *w)
    {
    unsigned char *var;
    if (fetch_var (w->var, NULL, &var))
	{
	XmTextFieldSetString (w->w, s_str (var));
	}
    return 0;
    }

/* Store callback for text field */
static int text_field_store (struct emacs_widget *w)
    {
    char *var;
    var = XmTextFieldGetString (w->w);
    if (var)
	set_var (w->var, 0, u_str (var));
    return 0;
    }

/* Create a text field Widget */
int motif_text_field (void)
    {
    struct prognode *prg;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a text field");
    else
        if (!check_args (5, 5))
	    {
	    prg = cur_exec->p_args[4].pa_node;
	    if (prg->p_proc != &bound_variable_node)
		error (must_be_var, "XmTextField", 5);
	    else
		{
		struct emacs_widget *w;

		if ((w = (struct emacs_widget *)malloc (sizeof (struct emacs_widget), malloc_type_char)) == NULL)
		    error (no_mem, "text field");
		else
		    {
		    int x, y, columns, resize;
			
		    x = numeric_arg (1);
		    if (!err) y = numeric_arg (2);
		    if (!err) columns = numeric_arg (3);
		    if (!err) resize = numeric_arg (4);
		    if (err)
			free (w);
		    else
			{
			w->fetch = text_field_fetch;
			w->store = text_field_store;
			w->var = savestr (prg->p_args[0].pa_name->v_name);
			if (w->var == NULL)
			    {
			    error (no_mem, prg->p_args[0].pa_name->v_name);
			    free (w);
			    }
			else
			    {
			    Arg args[5];
			    Widget b;
			    int argc = 0;
			    char name[20];

			    if (x >= 0)
				{
				XtSetArg (args[argc], XmNleftOffset, x), argc++;
				XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
				}
			    if (y >= 0)
				{
				XtSetArg (args[argc], XmNtopOffset, y), argc++;
				XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
				}
			    if (columns > 0)
				XtSetArg (args[argc], XmNcolumns, columns), argc++;
			    if (resize > 0)
				XtSetArg (args[argc], XmNresizeWidth, resize != 0), argc++;
			    sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
			    if( (b = XmCreateTextField (root, name, args, argc)) != NULL )
				{
				XtManageChild (b);
				w->next = root_dialog->children;
				w->w = b;
				root_dialog->children = w;
				void_result ();
				ml_value->exp_type = ISINTEGER;
				ml_value->exp_int = (int)b;
				}
			    else
				{
				error (motif_problem, "text field");
				free (w);
				}
			    }
			}
		    }		
		}
	    }
    return 0;
    }

/* Create a separator gadget */
int motif_separator (void)
    {
    struct prognode *p = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a separator");
    else
        if (!check_args (0, 5))
	    {
	    int x, y, len, is_horz, type;
	    x = p->p_nargs > 0 ? numeric_arg (1) : -1;
	    if (!err) y = p->p_nargs > 1 ? numeric_arg (2) : -1;
	    if (!err) len = p->p_nargs > 2 ? numeric_arg (3) : -1;
	    if (!err) is_horz = p->p_nargs > 3 ? numeric_arg (4) : 1;
	    if (!err) type = p->p_nargs > 4 ? numeric_arg (5) : -1;
	    if (!err)
		{
		Arg args[9];
		Widget b;
		int argc = 0;
		char name[20];

		if (x >= 0)
		    {
		    XtSetArg (args[argc], XmNleftOffset, x), argc++;
		    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
		    }
		if (y >= 0)
		    {
		    XtSetArg (args[argc], XmNtopOffset, y), argc++;
		    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
		    }
		if (is_horz && len >= 0)
		    XtSetArg (args[argc], XmNwidth, len), argc++;
		if (!is_horz)
		    {
		    if (len >= 0)
			XtSetArg (args[argc], XmNheight, len), argc++;
		    XtSetArg (args[argc], XmNorientation, XmVERTICAL), argc++;
		    }
		if (type >= 0)
		    XtSetArg (args[argc], XmNseparatorType, type), argc++;
		sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
		if( (b = XmCreateSeparatorGadget (root, name, args, argc)) != NULL )
		    {
		    XtManageChild (b);
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = (int)b;
		    }
		else
		    {
		    error (motif_problem, "separator");
		    }
		}
	    }		
    return 0;
    }

/* Create an empty row/column */
int motif_row_column (void)
    {
    struct prognode *p = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a row column");
    else
        if (!check_args (0, 10000))
	    {
	    int x , y, border, cols, is_horz, pack, width, height;
	    x = p->p_nargs > 0 ? numeric_arg (1) : -1;
	    if (!err) y = p->p_nargs > 1 ? numeric_arg (2) : -1;
	    if (!err) width = p->p_nargs > 2 ? numeric_arg (3) : -1;
	    if (!err) height = p->p_nargs > 3 ? numeric_arg (4) : -1;
	    if (!err) border = p->p_nargs > 4 ? numeric_arg (5) : -1;
	    if (!err) cols = p->p_nargs > 5 ? numeric_arg (6) : -1;
	    if (!err) is_horz = p->p_nargs > 6 ? numeric_arg (7) : -1;
	    if (!err) pack = p->p_nargs > 7 ? numeric_arg (8) : -1;
	    if (!err)
		{
		Arg args[10];
		Widget b;
		int argc = 0;
		char name[20];

		if (x >= 0)
		    {
		    XtSetArg (args[argc], XmNleftOffset, x), argc++;
		    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
		    }
		if (y >= 0)
		    {
		    XtSetArg (args[argc], XmNtopOffset, y), argc++;
		    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
		    }
		if (width >= 0)
		    XtSetArg (args[argc], XmNwidth, width), argc++;
		if (height >= 0)
		    XtSetArg (args[argc], XmNheight, height), argc++;
		if (border >= 0)
		    XtSetArg (args[argc], XmNborderWidth, border > 0 ? 1 : 0), argc++;
		if (cols >= 0)
		    XtSetArg (args[argc], XmNnumColumns, cols), argc++;
		if (is_horz > 0)
		    XtSetArg (args[argc], XmNorientation, XmHORIZONTAL), argc++;
		if (pack >= 0)
		    XtSetArg (args[argc], XmNpacking, pack), argc++;
		sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
		if( (b = XmCreateRowColumn (root, name, args, argc)) != NULL )
		    {
		    int i;
		    Widget old_root = root;

		    XtManageChild (b);

		    root = b;
		    for (i = 9; i <= p->p_nargs && !err; i++)
			eval_arg (i);
		    root = old_root;
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = (int)b;
		    }
		else
		    error (motif_problem, "row column");
		}
	    }		
    return 0;
    }

/* Create an empty row/column */
int motif_frame (void)
    {
    struct prognode *p = cur_exec;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a frame");
    else
        if (!check_args (0, 10000))
	    {
	    int x , y, shadow;
	    x = p->p_nargs > 0 ? numeric_arg (1) : -1;
	    if (!err) y = p->p_nargs > 1 ? numeric_arg (2) : -1;
	    if (!err) shadow = p->p_nargs > 2 ? numeric_arg (3) : -1;
	    if (!err)
		{
		Arg args[6];
		Widget b;
		int argc = 0;
		char name[20];

		if (x >= 0)
		    {
		    XtSetArg (args[argc], XmNleftOffset, x), argc++;
		    XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
		    }
		if (y >= 0)
		    {
		    XtSetArg (args[argc], XmNtopOffset, y), argc++;
		    XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
		    }
		if (shadow >= 0)
		    XtSetArg (args[argc], XmNshadowType, shadow), argc++;
		sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
		if( (b = XmCreateFrame (root, name, args, argc)) != NULL )
		    {
		    int i;
		    Widget old_root = root;

		    XtManageChild (b);

		    root = b;
		    for (i = 4; i <= p->p_nargs && !err; i++)
			eval_arg (i);
		    root = old_root;
		    void_result ();
		    ml_value->exp_type = ISINTEGER;
		    ml_value->exp_int = (int)b;
		    }
		else
		    error (motif_problem, "frame");
		}
	    }		
    return 0;
    }

/* Fetch callback for updated label */
static int label_fetch (struct emacs_widget *w)
    {
    unsigned char *var;
    if (fetch_var (w->var, NULL, &var))
	{
	XmString p;
	Arg args[1];

	if ((p = XmStringCreateLtoR (s_str (var), charset)) == NULL)
	    error (motif_problem, "label");
	else
	    {
	    XtSetArg (args[0], XmNlabelString, p);
	    XtSetValues (w->w, args, 1);
	    XmStringFree (p);
	    }
	}
    return 0;
    }

/* Create an updated  label gadget */
int motif_updated_label (void)
    {
    struct prognode *prg;

    if (!is_motif)
	return motif_not_running ();

    if (root == NULL)
	error (root_inactive, "a label");
    else
        if (!check_args (3, 3))
	    {
	    prg = cur_exec->p_args[2].pa_node;
	    if (prg->p_proc != &bound_variable_node)
		error (must_be_var, "XmUpdatedLabel", 5);
	    else
		{
		struct emacs_widget *w;

		if ((w = (struct emacs_widget *)malloc (sizeof (struct emacs_widget), malloc_type_char)) == NULL)
		    error (no_mem, "label");
		else
		    {
		    int x, y;

		    x = numeric_arg (1);
		    if (!err) y = numeric_arg (2);
		    if (err)
			free (w);
		    else
			{
			Arg args[5];
			Widget b;
			int argc = 0;
			char name[20];

			w->fetch = label_fetch;
			w->store = NULL;
			w->var = savestr (prg->p_args[0].pa_name->v_name);
			if (w->var == NULL)
			    {
			    error (no_mem, prg->p_args[0].pa_name->v_name);
			    free (w);
			    }
			else
			    {
			    if (x >= 0)
				{
				XtSetArg (args[argc], XmNleftOffset, x), argc++;
				XtSetArg (args[argc], XmNleftAttachment, XmATTACH_FORM), argc++;
				}
			    if (y >= 0)
				{
				XtSetArg (args[argc], XmNtopOffset, y), argc++;
				XtSetArg (args[argc], XmNtopAttachment, XmATTACH_FORM), argc++;
				}
			    sprintfl (u_str (&name[0]), sizeof (name), u_str ("emacs_%d"), root_seq++);
			    if( (b = XmCreateLabelGadget (root, name, args, argc)) != NULL )
				{
				XtManageChild (b);
				w->next = root_dialog->children;
				w->w = b;
				root_dialog->children = w;
				void_result ();
				ml_value->exp_type = ISINTEGER;
				ml_value->exp_int = (int)b;
				}
			    else
				error (motif_problem, "label");
			    }
			}
		    }
		}
	    }
    return 0;
    }

/* Fetch callback for message box */
static int mb_fetch (struct emacs_widget *w)
    {
    unsigned char *var;

    if (fetch_var (w->var, NULL, &var))
	if (*var != 0)
	    {
	    XmString p;

	    if ((p = XmStringCreateLtoR (s_str (var), charset)) == NULL)
		error (motif_problem, "message dialog");
	    else
		{
		Arg args[1];

		XtSetArg (args[0], XmNmessageString, p);
		XtSetValues (w->w, args, 1);
		XmStringFree (p);
		}
	    }
    return 0;
    }

/* Fetch callback for file selection box */
static int fsb_fetch (struct emacs_widget *w)
    {
    unsigned char *var;

    if (fetch_var (w->var, NULL, &var))
	if (*var != 0)
	    {
	    XmString p;

	    if ((p = XmStringCreateLtoR (s_str (var), charset)) == NULL)
		error (motif_problem, "file selection dialog");
	    else
		{
		XmFileSelectionDoSearch (w->w, p);
		XmStringFree (p);
		}
	    }
    return 0;
    }

/* Store callback for file selection box */
static int fsb_store (struct emacs_widget *w)
    {
    char *var;
    XmString info = NULL;
    Arg args[1];
   
    XtSetArg (args[0], XmNtextString, &info);
    XtGetValues (w->w, args, 1);
    if (info != NULL)
	{
	XmStringGetLtoR (info, charset, &var);
	if (var)
	    {
	    set_var (w->var, 0, u_str (var));
	    XtFree (var);
	    }
	}
    return 0;
    }

/* Setup the pseudo widgets for the file selection box */
static void pw_setup (struct prognode *prg, int (*func)(struct emacs_widget *), char *msg)
    {
    if (prg->p_proc != &bound_variable_node)
	error (must_be_var, msg, 3);
    else
	{
	struct emacs_widget *w;

	if ((w = (struct emacs_widget *)malloc (sizeof (struct emacs_widget), malloc_type_char)) == NULL)
	    error (no_mem, "file selection box");
	else
	    {
	    if (func == fsb_fetch || func == mb_fetch)
		{
		w->fetch = func;
		w->store = NULL;
		}
	    else
		{
		w->store = func;
		w->fetch = NULL;
		}
	    w->var = savestr (prg->p_args[0].pa_name->v_name);
	    if (w->var == NULL)
		{
		error (no_mem, prg->p_args[0].pa_name->v_name);
		free (w);
		}
	    else
		{
		w->next = root_dialog->children;
		w->w = root;
		root_dialog->children = w;
		}
	    }
	}
    }

/* Create a named XmFileSelectionBox dialog shell */
int motif_mk_file_dialog (void)
    {
    struct emacs_dialog *w;
    struct prognode *pattern, *result;

    if (!is_motif)
	return motif_not_running ();

    if (root != NULL)
	error (root_active, root_dialog->name);
    else if (!check_args (4, 4))
	{
	pattern = cur_exec->p_args[2].pa_node;
	result = cur_exec->p_args[3].pa_node;

	if (string_arg (1) == 0)
	    return 0;
	if (find_dialog (ml_value->exp_v.v_string))
	    error (already_exists, ml_value->exp_v.v_string);
	else
	    {
	    int argc, x, y;
	    Arg args[MAX_ARGS];

	    if ((w = make_dialog (ml_value->exp_v.v_string)) == NULL)
		return 0;
	    w->type = DIALOG_FILE;
	    w->apply_keys = NULL;
	    release_expr (ml_value);
	    if (string_arg (2) == 0)
		{
		destroy_dialog (w->name);
		return 0;
		}
	    if (*ml_value->exp_v.v_string != 0 &&
	      (w->apply_keys = savestr (ml_value->exp_v.v_string)) == NULL)
		{
		error (no_mem, "file selection dialog", w->name);
		destroy_dialog (w->name);
		}
	    else
		{
		XmString cancel = NULL, apply = NULL;

		x = app_shell->core.x;
		y = app_shell->core.y;
		x_pos = (x_pos + 1) & 7;
		y_pos = (y_pos + 1) & 3;
		XtSetArg (args[0], XmNx, x + ((x_pos + 1) * 10)), argc = 1;
		XtSetArg (args[argc], XmNy, y + ((y_pos + 1) * 30)), argc++;
		XtSetArg (args[argc], XmNautoUnmanage, 0), argc++;
		XtSetArg (args[argc], XmNdefaultPosition, 0), argc++;
		if (w->apply_keys)
		    if( (cancel = XmStringCreateLtoR ("Cancel", charset)) != NULL )
			if ((apply = XmStringCreateLtoR ("Apply", charset)) == NULL)
			    {
			    XmStringFree (cancel);
			    cancel = NULL;
			    }
		if (cancel && apply)
		    {
		    XtSetArg (args[argc], XmNcancelLabelString, apply), argc++;
		    XtSetArg (args[argc], XmNhelpLabelString, cancel), argc++;
		    }
		w->widget = XmCreateFileSelectionDialog (app_shell, s_str (w->name), args, argc);
		if (w->widget == 0)
		    {
		    error (motif_problem, w->name);
		    destroy_dialog (w->name);
		    if (cancel) XmStringFree (cancel);
		    if (apply) XmStringFree (apply);
		    }
		else
		    {
		    w->dialog = XtParent (w->widget);
		    XtAddCallback (w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w);
		    XtAddCallback (w->widget, XmNokCallback, ok_cb, (XtPointer)w);
		    if (cancel && apply)
			{
			XtAddCallback (w->widget, XmNcancelCallback, apply_cb, (XtPointer)w);
			XtAddCallback (w->widget, XmNhelpCallback, cancel_cb, (XtPointer)w);
			}
		    else
			{
			Widget help;

			XtAddCallback (w->widget, XmNcancelCallback, cancel_cb, (XtPointer)w);
			if( (help = XmFileSelectionBoxGetChild (w->widget, XmDIALOG_HELP_BUTTON)) != NULL )
			    XtDestroyWidget (help);
			}
		    if (cancel) XmStringFree (cancel);
		    if (apply) XmStringFree (apply);
		    XtSetArg (args[0], XmNtitle, w->name), argc = 1;
		    XtSetArg (args[argc], XmNwindowGroup, XtUnspecifiedWindowGroup), argc++;		
		    XtSetValues (w->dialog, args, argc);
		    root = w->widget;
		    root_dialog = w;
		    pw_setup (pattern, fsb_fetch, "XmFileSelectionDialog");
		    if (!err) pw_setup (result, fsb_store, "XmFileSelectionDialog");
		    root = NULL;
		    root_dialog = NULL;
		    if (err)
			{
			XtDestroyWidget (w->dialog);
			destroy_dialog (w->name);
			}
		    else
			void_result ();
		    }
		}
	    }
	}
    return 0;
    }

/* Create a named XmMessage dialog shell */
int motif_mk_message_dialog (void)
    {
    struct emacs_dialog *w;
    struct prognode *mesg;
    int x = -1, y = -1;

    if (!is_motif)
	return motif_not_running ();

    if (root != NULL)
	error (root_active, root_dialog->name);
    else if (!check_args (3, 3))
	{
	mesg = cur_exec->p_args[2].pa_node;

	if (string_arg (1) == 0)
	    return 0;
	if (find_dialog (ml_value->exp_v.v_string))
	    error (already_exists, ml_value->exp_v.v_string);
	else
	    {
	    int argc, type;
	    Arg args[MAX_ARGS];

	    if ((w = make_dialog (ml_value->exp_v.v_string)) == NULL)
		return 0;
	    w->type = DIALOG_MESSAGE;
	    w->apply_keys = NULL;
	    release_expr (ml_value);
	    type = numeric_arg (2);
	    if (err)
		destroy_dialog (w->name);
	    else
		{
		x = app_shell->core.x;
		y = app_shell->core.y;
		XtSetArg (args[0], XmNx, x + 10), argc = 1;
		XtSetArg (args[argc], XmNy, y + 30), argc++;
		XtSetArg (args[argc], XmNautoUnmanage, 0), argc++;
		XtSetArg (args[argc], XmNdefaultPosition, 0), argc++;
		w->widget = XmCreateMessageDialog (app_shell, s_str (w->name), args, argc);
		if (w->widget == 0)
		    {
		    error (motif_problem, w->name);
		    destroy_dialog (w->name);
		    }
		else
		    {
		    Widget button;

		    if (type >= 0)
			{
			XtSetArg (args[0], XmNdialogType, type);
			XtSetValues (w->widget, args, 1);
			}
		    w->dialog = XtParent (w->widget);
		    XtAddCallback (w->dialog, XmNpopdownCallback, popdown_cb, (XtPointer)w);
		    XtAddCallback (w->widget, XmNokCallback, ok_cb, (XtPointer)w);
		    if( (button = XmMessageBoxGetChild (w->widget, XmDIALOG_HELP_BUTTON)) != NULL )
			XtDestroyWidget (button);
		    if( (button = XmMessageBoxGetChild (w->widget, XmDIALOG_CANCEL_BUTTON)) != NULL )
			XtDestroyWidget (button);
		    XtSetArg (args[0], XmNtitle, w->name), argc = 1;
		    XtSetArg (args[argc], XmNwindowGroup, XtUnspecifiedWindowGroup), argc++;
		    XtSetValues (w->dialog, args, argc);
		    root = w->widget;
		    root_dialog = w;
		    pw_setup (mesg, mb_fetch, "XmMessageDialog");
		    root = NULL;
		    root_dialog = NULL;
		    if (err)
			{
			XtDestroyWidget (w->dialog);
			destroy_dialog (w->name);
			}
		    else
			void_result ();
		    }
		}
	    }
	}
    return 0;
    }

/* Return indication of existance of a dialog shell */
int motif_is_dialog (void)
    {
    struct emacs_dialog *w;
    unsigned char *name;
    if (!is_motif)
	return motif_not_running ();

    if( (name = getstr (u_str (": XmIsDialog "))) != NULL )
	{
	w = find_dialog (name);
	void_result ();
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = w != NULL;
	}
    return 0;
    }
#endif
