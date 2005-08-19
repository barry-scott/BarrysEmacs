#pragma module emacs_cms "V4.1-015"

/*
 * 	Copyright © 1986
 *	Barry A. Scott and Nick W. Emery
 *
 * This module implements the EMACS <=> CMS interface layer in the
 * EMACS sharable image EMACS_CMS_SHR.EXE.
 *
 * CMS calls from EMACS through this interface have the following
 * format:
 *
 *	EMACS$CMS (CMS_COMMAND_ID, ARGS ...)
 *
 * The CMS_COMMAND_IDs are all universal symbols, and so can be
 * imported, as required, into the EMACS variable name space by
 * using the external-variable function.
 */

#include "edit$obj:em_user.h"
#include <descrip.h>
#include <stsdef.h>
#include <ssdef.h>

/*
 * Define the CMS command function codes. These are globalvalues so that
 * they can be made universal and imported into EMACS
 */

globaldef enum codes
    {
    cms$_set_library,		/* CMS SET LIBRARY directory */
    cms$_show_class,		/* CMS SHOW CLASS [class_name] */
    cms$_show_element,		/* CMS SHOW ELEMENT [element_name] */
    cms$_show_group,		/* CMS SHOW GROUP [group_name] */
    cms$_show_reservation,	/* CMS SHOW RESERVATION [generation] */
    cms$_fetch,			/* Fetch an element */
    cms$_reserve,		/* Reserve a single element */
    cms$_unreserve,		/* Unreserve an element */
    cms$_annotate,		/* Fetch an annotated listing */
    cms$_create_class,		/* Create a class */
    cms$_create_group,		/* Create a group */
    cms$_delete_class,		/* Deletes a class */
    cms$_delete_element,	/* Deletes an element */
    cms$_delete_group,		/* Deletes a group */
    cms$_insert_element,	/* Inserts an element into a group */
    cms$_insert_generation,	/* Inserts a generation into a class */
    cms$_insert_group,		/* Inserts a group into another group */
    cms$_replace,		/* Replace a reserved element */
    cms$_create_element,	/* Create a new element */
    cms$_copy_element,		/* Copy an element or group of elements */
    cms$_modify_class,		/* Modify attributes of a class */
    cms$_modify_element,	/* Modify attributes of an element */
    cms$_modify_group,		/* Modify attributes of a group */
    cms$_modify_library,	/* Modify attributes of a library */
    cms$_remove_element,	/* Remove an element from a group */
    cms$_remove_generation,	/* Remove a generation from a class */
    cms$_remove_group,		/* Remove a sub-group from a group */
    cms$_remark,		/* Enter a remark in the history */
    };

globalvalue CMS$_EOF, CMS$_STOPPED, CMS$_NORMAL;

/*
 * Define the EMACS callback macros.
 */

#define check_args(min, max) ((*call_back)\
	(EMACS__K_CHECK_ARGS, min, max))
#define arg_count(location) ((*call_back)\
	(EMACS__K_ARG_COUNT, location))
#define numeric_arg(offset, location) ((*call_back)\
	(EMACS__K_NUMERIC_ARG, offset, location))
#define string_arg(offset, location, size) ((*call_back)\
	(EMACS__K_STRING_ARG, offset, location, size))
#define error(string) ((*call_back)\
	(EMACS__K_ERROR, string))
#define malloc(size, address) ((*call_back)\
	(EMACS__K_ALLOCATE, size, address))
#define use_buffer(address) ((*call_back)\
	(EMACS__K_USE_BUFFER, address))
#define goto_character(number) ((*call_back)\
	(EMACS__K_GOTO_CHARACTER, number))
#define buffer_size(size) ((*call_back)\
	(EMACS__K_BUFFER_SIZE, size))
#define delete_characters(count) ((*call_back)\
	(EMACS__K_DELETE_CHARACTERS, count))
#define insert_string(str) ((*call_back)\
	(EMACS__K_INSERT_STRING, str))
#define insert_character(ch) ((*call_back)\
	(EMACS__K_INSERT_CHARACTER, ch))
#define dot(d, b, s) ((*call_back)\
	(EMACS__K_DOT, d, b, s))
#define return_string_to_param(arg, str) ((*call_back)\
	(EMACS__K_RETURN_STRING_TO_PARAM, arg, str))
#define GapTo(pos) ((*call_back)\
	(EMACS__K_GAP_TO, pos))
#define buffer_extent(s1,p1,s2,p2) ((*call_back)\
	(EMACS__K_BUFFER_EXTENT, s1, p1, s2, p2))

/*
 * Check for VMS errors
 */

#define failed(exp) (((exp) & STS$K_SUCCESS) == 0)
#define err(string) {static char text[] = string;\
    static $DESCRIPTOR(str,text);status = error (&str);}

/*
 * build an empty string descriptor
 */

#define descriptor(name, size) char name##_text[size];\
    struct dsc$descriptor name = \
    {size, DSC$K_DTYPE_T, DSC$K_CLASS_S, &name##_text}

/*
 * Macros for min and max
 */

#define min(a,b) ((a) > (b) ? (b) : (a))
#define max(a,b) ((a) > (b) ? (a) : (b))

/*
 * context offsets
 */

#define CTX_RESTORE 0			/* EMACS restore count */
#define CTX_CALLBACK 1			/* Address of CALL BACK ROUTINE */
#define CTX_CUR_RESTORE 2		/* Current restore value */
#define CTX_BASE 3			/* Base of replace buffer */
#define CTX_PTR 4			/* Current replace pointer */
#define CTX_BUF_SIZE 5			/* Current buffer size */
#define CTX_LDB 6			/* Start of LDB */
#define CTX_LDB_COUNT 7			/* In use count of LDB */
#define CTX_LDB_STATUS 7		/* Last CMS status return */
#define CTX_LDB_LIBRARY 8		/* Start of LDB library descriptor */
#define CTX_LDB_SIZE 50			/* Size of an LDB */
#define CTX_SIZE (CTX_LDB_SIZE + CTX_LDB)	/* Size of a CTX block */

#define MAX_LINE 512			/* Maximum fetch line size */
#define INDENT 20			/* output format offset */
#define STRING_SIZE 255			/* Size of all strings */

static char cms_list_text[] = "CMS list";
static $DESCRIPTOR (cms_list, cms_list_text);	/* List buffer name */
static char two_bl_lines[] = "\n\n";
static $DESCRIPTOR (two_blank_lines, two_bl_lines);	/* Two newlines */
static char spaces_text[] = "                     ";
static $DESCRIPTOR (spaces, spaces_text);	/* INDENT number of spaces */
static char comma_text[] = ", ";
static $DESCRIPTOR (comma, comma_text);	/* Comma separator */
static char null_text[] = "NLA0:";
static $DESCRIPTOR (null_dev, null_text);	/* Null device */

/* CMS message handling routines */
#define LINE_COUNT 0
#define CALL_BACK 1
int emit_one_line (str, params)
    struct dsc$descriptor *str;
    int *params;
	{
	int (*call_back)() = params[CALL_BACK];
	if (params[LINE_COUNT]++ == 1)
		error (str);
	return STS$K_WARNING;
	}

int cms_message (sig, mec, ldb)
    int *sig, *mec, *ldb;
	{
	int *p = &sig;
	int params[2];
	if (p[-1] >= 3)
		{
		params[0] = 0;
		params[1] = ldb[CTX_CALLBACK - CTX_LDB];
		sig [0] |= 1 << 16;
		sys$putmsg (sig, &emit_one_line, 0, &params);
		}
	return SS$_CONTINUE;
	}

static int null ()
	{
	return SS$_CONTINUE;
	}
extern int emacs$cms (call_back, context, restore)
    int (*call_back)();
    int **context;
    int *restore;
	{
	register int status;
	enum codes func;
	lib$establish (&cms_message);

	/* Allocate the memory for the context block if it is required */

	if (*context == 0)
		if (failed (status = malloc (CTX_SIZE * sizeof (int),
		    context)))
			return status;
		else
			(*context)[CTX_LDB_LIBRARY] = 0;

	/* Save the call_back address, and the current retore count */
	(*context)[CTX_CALLBACK] = call_back;
	(*context)[CTX_CUR_RESTORE] = *restore;

	/* Check that there are a reasonable number of args */
	if (! failed (status = check_args (1, 10)))
		if (! failed (status = numeric_arg (1, &func)))
			{

			/*
			 * Check that functions which require a CMS
			 * context block has one set up already.
			 */
			
			if (func != cms$_set_library)
				if (failed (status = cms_check_library (context)))
					return status;

			status = cms_call_function (context, func);
			}

	return status;
	}

/* Check a library for access */
int cms_check_library (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register int status = SS$_NORMAL;

	if ((*context)[CTX_LDB_LIBRARY] != 0)
		{
		if ((*context)[CTX_RESTORE] != (*context)[CTX_CUR_RESTORE])
			{
			status = cms$set_library (&(*context)[CTX_LDB],
			     &(*context)[CTX_LDB_LIBRARY], &cms_message);
			(*context)[CTX_RESTORE] = *(context)[CTX_CUR_RESTORE];
			}
		}
	else
		{
		err ("You must set up the CMS library before using this function");
		return STS$K_WARNING;
		}

	return status;
	}

/* Dispatch routine for CMS requests */
int cms_call_function (context,func)
    enum codes func;
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register int status = SS$_NORMAL;

	switch (func)
		{
		case cms$_set_library:
			status = cms_set_library (context);
			break;

		case cms$_show_class:
			{
			static char class_list_text[] = "Classes in DEC/CMS library ";
			static $DESCRIPTOR(class_list, class_list_text);
			extern int cms$show_class ();
			status = cms_show_basic (context, &class_list, &cms$show_class);
			break;
			}

		case cms$_show_element:
			status = cms_show_element (context);
			break;

		case cms$_show_group:
			{
			static char group_list_text[] = "Groups in DEC/CMS library ";
			static $DESCRIPTOR(group_list, group_list_text);
			extern int cms$show_group ();
			status = cms_show_basic (context, &group_list, &cms$show_group);
			break;
			}

		case cms$_show_reservation:
			status = cms_show_reservation (context);
			break;

		case cms$_fetch:
			status = cms_fetch (context);
			break;

		case cms$_reserve:
			status = cms_reserve (context);
			break;

		case cms$_unreserve:
			status = cms_unreserve (context);
			break;

		case cms$_annotate:
			status = cms_annotate (context);
			break;

		case cms$_create_class:
			{
			extern int cms$create_class ();
			status = cms_basic (context, &cms$create_class);
			break;
			}

		case cms$_create_group:
			{
			extern int cms$create_group ();
			status = cms_basic (context, &cms$create_group);
			break;
			}

		case cms$_delete_class:
			{
			extern int cms$delete_class ();
			status = cms_basic (context, &cms$delete_class);
			break;
			}

		case cms$_delete_element:
			{
			extern int cms$delete_element ();
			status = cms_basic (context, &cms$delete_element);
			break;
			}

		case cms$_delete_group:
			{
			extern int cms$delete_group ();
			status = cms_basic (context, &cms$delete_group);
			break;
			}

		case cms$_insert_element:
			{
			extern int cms$insert_element ();
			status = cms_insert_basic (context, &cms$insert_element);
			break;
			}

		case cms$_insert_generation:
			status = cms_insert_generation (context);
			break;

		case cms$_insert_group:
			{
			extern int cms$insert_group ();
			status = cms_insert_basic (context, &cms$insert_group);
			break;
			}

		case cms$_replace:
			status = cms_replace (context);
			break;

		case cms$_create_element:
			status = cms_create_element (context);
			break;

		case cms$_copy_element:
			status = cms_copy_element (context);
			break;

		case cms$_modify_class:
			{
			extern cms$modify_class ();
			status = cms_modify (context, &cms$modify_class);
			break;
			}

		case cms$_modify_element:
			status = cms_modify_element (context);
			break;

		case cms$_modify_group:
			{
			extern cms$modify_group ();
			status = cms_modify (context, &cms$modify_group);
			break;
			}

		case cms$_modify_library:
			status = cms_modify_library (context);
			break;

		case cms$_remove_element:
			{
			extern cms$remove_element ();
			status = cms_remove (context, &cms$remove_element);
			break;
			}

		case cms$_remove_generation:
			{
			extern cms$remove_generation ();
			status = cms_remove (context, &cms$remove_generation);
			break;
			}

		case cms$_remove_group:
			{
			extern cms$remove_group ();
			status = cms_remove (context, &cms$remove_group);
			break;
			}

		case cms$_remark:
			status = cms_remark (context);
			break;

		default:
			err ("Unknown CMS function code");
		}

	return status;
	}

/* Trim a string of trailing nulls */
cms_trim (desc)
    struct dsc$descriptor *desc;
	{
	register char *ptr = desc->dsc$a_pointer;
	register int i = desc->dsc$w_length - 1;
	while (i >= 0)
		if (ptr [i] != ' ')
			break;
		else
			i--;
	return i + 1;
	}

/* Fill in the header for an entry in CMS list */
cms_first_list (context, str)
    struct dsc$descriptor *str;
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int size;
	register int status;

	/*
	 * (temp-use-buffer "CMS list")
	 * (goto-charcter 1)
	 * (setq size (buffer-size))
	 * (provide-prefix-argument size (delete-next-character))
	 */

	if (!failed (status = use_buffer (&cms_list)))
		if (!failed (status = goto_character (1)))
			if (!failed (status = buffer_size (&size)))
				status = delete_characters (size);
	/*
	 * (insert-string "<str><name>")
	 */

	if (!failed (status))
		if (!failed (status = insert_string (str)))
			if (!failed (status = insert_string (&(*context)[CTX_LDB_LIBRARY])))
				status = insert_string (&two_blank_lines);
	return status;
	}
/* CMS SET LIBRARY LIB-NAME routine */
cms_set_library (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register int status;
	descriptor (libnam, STRING_SIZE);

	if (! failed (status = check_args (2, 2)))
		if (! failed (status = string_arg (2, &libnam,
		    &libnam.dsc$w_length)))
			{
 			(*context)[CTX_RESTORE] = (*context)[CTX_CUR_RESTORE];
			status = cms$set_library (&(*context)[CTX_LDB],
			    &libnam, &cms_message);
			}

	return status;
	}
/* CMS SHOW CLASS [class_name] */

/* Output for SHOW CLASS */
cms_basic_output (first, ldb, str, name_id, remark_id, read_only)
    int *first, *ldb, *name_id, *remark_id, *read_only;
    struct dsc$descriptor *str;
	{
	register int status = SS$_NORMAL;
	descriptor (desc, STRING_SIZE);
	int *ctx = &ldb[-CTX_LDB];
	register int (*call_back)() = ldb[CTX_CALLBACK - CTX_LDB];

	/*
	 * For the first envokation, select the buffer, clear it and fill
	 * in the header
	 */

	if (*first)
		status = cms_first_list (&ctx, str);

	/*
	 * fetch the name and the remark
	 */

	if (!failed (status))
		{
		register int i;
		cms$get_string (name_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		desc.dsc$w_length = max (INDENT, desc.dsc$w_length);
		insert_string (&desc);
		insert_character (' ');
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (remark_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		insert_string (&desc);
		insert_character ('\n');
		if (&read_only)
			{
			static char ro_text[] = "Read only\n";
			static $DESCRIPTOR (ro, ro_text);
			insert_string (&spaces);
			insert_string (&ro);
			}
		insert_character ('\n');
		}
	return status;
	}

cms_show_basic (context, str, rtn)
    struct dsc$descriptor *str;
    int (*rtn)(), **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	register struct dsc$descriptor *param = 0;
	descriptor (name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (1,2)))
		return status;

	/* Fetch the name if present */
	if (failed (status = arg_count (&argc)))
		return status;
	if (argc > 1)
		if (failed (status = string_arg (2, &name,
		    &name.dsc$w_length)))
			return status;
		else
			param = &name;

	/* Call CMS to provide the information */
	if (!failed (status = (*rtn) (&(*context)[CTX_LDB],
	    &cms_basic_output, str, param, cms_message)))
		status = goto_character (1);

	return status;
	}
/* Output for SHOW ELEMENT */
cms_element_output (first, ldb, context, element_id, remark_id,
    history_id, notes_id, position, concurrent, reference, group_id)
    int *first, *ldb, **context, *element_id, *remark_id, *history_id;
    int *notes_id, *position, *concurrent, *reference, *group_id;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register int status = SS$_NORMAL;
	int size;
	static char element_list_text[] = "Elements in DEC/CMS library ";
	static $DESCRIPTOR(element_list, element_list_text);
	descriptor (desc, STRING_SIZE);

	/*
	 * For the first invocation, select the buffer, clear it and fill
	 * in the header
	 */

	if (*first)
		status = cms_first_list (context, &element_list);

	/*
	 * fetch the element strings
	 */

	if (!failed (status))
		{
		register int i;
		cms$get_string (element_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		desc.dsc$w_length = max (INDENT, desc.dsc$w_length);
		insert_string (&desc);
		insert_character (' ');
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (remark_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		insert_string (&desc);
		insert_character ('\n');
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (history_id, &desc);
		if (desc.dsc$w_length = cms_trim (&desc))
			{
			static char history_text[] = "History = ";
			static $DESCRIPTOR (history, history_text);
			insert_string (&spaces);
			insert_string (&history);
			insert_string (&desc);
			insert_character ('\n');
			}
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (notes_id, &desc);
		if (desc.dsc$w_length = cms_trim (&desc))
			{
			static char notes_text[] = "Notes = ";
			static $DESCRIPTOR (notes, notes_text);
			insert_string (&spaces);
			insert_string (&notes);
			insert_string (&desc);
			insert_character ('\n');
			}
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (group_id, &desc);
		if (desc.dsc$w_length = cms_trim (&desc))
			{
			static char groups_text[] = "Groups = ";
			static $DESCRIPTOR (groups, groups_text);
			insert_string (&spaces);
			insert_string (&groups);
			insert_string (&desc);
			insert_character ('\n');
			}
		if (*concurrent || *reference)
			insert_string (&spaces);
		if (*concurrent)
			{
			static char conc_text[] = "Concurrency allowed";
			static $DESCRIPTOR (conc, conc_text);
			insert_string (&conc);
			}
		if (*reference)
			{
			static char ref_text[] = "Reference copy made";
			static $DESCRIPTOR (ref, ref_text);
			if (*concurrent)
				insert_string (&comma);
			insert_string (&ref);
			}
		if (*reference || *concurrent)
			insert_character ('\n');
		}
	insert_character ('\n');
	return status;
	}
cms_show_element (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	register struct dsc$descriptor *element = 0;
	descriptor (element_name, STRING_SIZE);
	int group = 0;
	/* Check that enough args were specified */
	if (failed (status = check_args (1,3)))
		return status;

	/* Fetch the element_name if present */
	if (failed (status = arg_count (&argc)))
		return status;
	if (argc > 1)
		if (failed (status = string_arg (2, &element_name,
		    &element_name.dsc$w_length)))
			return status;
		else
			element = &element_name;

	/* Fetch the group flag */
	if (argc > 2)
		if (failed (status = numeric_arg (3, &group)))
			return status;

	/* Call CMS to provide the information */
	if (!failed (status = cms$show_element (&(*context)[CTX_LDB],
	    &cms_element_output, context, element, &group, cms_message)))
		status = goto_character (1);

	return status;
	}
/* Output for SHOW RESERVATION */
cms_reservation_output (first, ldb, context, element_id, generation_id,
    time, user_id, remark_id, concurrent, merge_id, nonotes, nohistory, access)
    int *first, *ldb, **context, *element_id, *generation_id, *time, *user_id;
    int *remark_id, *concurrent, *merge_id, *nonotes, *nohistory, *access;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register int status = SS$_NORMAL;
	int size;
	static char reservation_list_text[] = "Reservations in DEC/CMS library ";
	static $DESCRIPTOR(reservation_list, reservation_list_text);
	descriptor (desc, STRING_SIZE);

	/*
	 * For the first invocation, select the buffer, clear it and fill
	 * in the header
	 */

	if (*first == 1)
		status = cms_first_list (context, &reservation_list);

	/* Ignore concurrency information */
	else
		if (*first == 3)
			return status;

	/*
	 * fetch the element strings
	 */

	if (!failed (status))
		{
		register int i;
		register struct dsc$descriptor *s;
		static char by_text[] = "By ";
		static $DESCRIPTOR (by, by_text);
		static char on_text[] = " on ";
		static $DESCRIPTOR (on, on_text);
		cms$get_string (element_id, &desc);
		i =
		desc.dsc$w_length = cms_trim (&desc);
		insert_string (&desc);
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (generation_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		insert_character ('(');
		insert_string (&desc);
		insert_character (')');
		insert_character (' ');
		i += desc.dsc$w_length + 1 + 1 + 1;
		while (i++ < INDENT + 1)
			insert_character (' ');
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (remark_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		insert_string (&desc);
		insert_character ('\n');
		desc.dsc$w_length = STRING_SIZE;
		cms$get_string (user_id, &desc);
		desc.dsc$w_length = cms_trim (&desc);
		insert_string (&spaces);
		insert_string (&by);
		insert_string (&desc);
		insert_string (&on);
		desc.dsc$w_length = 17;
		sys$asctim (&desc.dsc$w_length, &desc, time, 0);
		insert_string (&desc);
		insert_character ('\n');
		insert_string (&spaces);
		{
		static char con_all_text[] = "Concurrency allowed";
		static $DESCRIPTOR (con_all, con_all_text);
		static char con_dis_text[] = "Concurrency disallowed";
		static $DESCRIPTOR (con_dis, con_dis_text);
		static char res_dis_text[] = "Reservations disallowed";
		static $DESCRIPTOR (res_dis, res_dis_text);
		s = (*access == 0) ?
			&con_all :
		    (*access == 1) ?
			&con_dis :
			&res_dis;
		}
		insert_string (s);
		if (*concurrent)
			{
			static char conc_text[] = ", concurrent reservation";
			static $DESCRIPTOR (conc, conc_text);
			static char conr_text[] = ", concurrent replacement";
 			static $DESCRIPTOR (conr, conr_text);
			s = (*concurrent == -1) ? &conr : &conc;
			insert_string (s);
			}
		if (*nonotes)
			{
			static char non_text[] = ", notes suppressed";
			static $DESCRIPTOR (non, non_text);
			insert_string (&non);
			}
		if (*nohistory)
			{
			static char noh_text[] = ", history suppressed";
			static $DESCRIPTOR (noh, noh_text);
			insert_string (&noh);
			}
		insert_character ('\n');
		}
	insert_character ('\n');
	return status;
	}
cms_show_reservation (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	register struct dsc$descriptor *element = 0, *generation = 0, *user = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (generation_expr, STRING_SIZE);
	descriptor (user_name, 100);

	/* Check that enough args were specified */
	if (failed (status = check_args (1,4)))
		return status;

	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name if present */
	if (argc > 1)
		if (failed (status = string_arg (2, &element_name,
		    &element_name.dsc$w_length)))
			return status;
		else
			element = &element_name;

	/* Fetch the generation expression if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &generation_expr,
		    &generation_expr.dsc$w_length)))
			return status;
		else
			generation = &generation_expr;

	/* Fetch the nohistory flag */
	if (argc > 3)
		if (failed (status = string_arg (4, &user_name,
		    &user_name.dsc$w_length)))
			return status;
		else
			user = &user_name;

	/* Call CMS to provide the information */
	if (!failed (status = cms$show_reservations (&(*context)[CTX_LDB],
	    &cms_reservation_output, context, element, generation, user, cms_message)))
		status = goto_character (1);

	return status;
	}
/* Fetch the contents of an element , and insert it into the current buffer */
cms_fetch_lines (context, element, generation, nohistory, nonotes, gen_fetched)
    struct dsc$descriptor *element, *generation, *gen_fetched;
    int nohistory, nonotes, **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int fdb [CTX_SIZE];
	register int status, *ctx = *context;
	int d;
	descriptor (buf, 100);
	descriptor (line, MAX_LINE);

	fdb[CTX_RESTORE] = (*context)[CTX_RESTORE];
	fdb[CTX_CALLBACK] = call_back;
	fdb[CTX_CUR_RESTORE] = (*context)[CTX_CUR_RESTORE];

	/* Save dot for restoration */
	if (failed (status = dot (&d, &buf, &buf.dsc$w_length)))
		return status;

	/* Open the element */
	if (failed (status = cms$fetch_open (&fdb[CTX_LDB],
	    &ctx [CTX_LDB_LIBRARY], element, generation,
	    &nohistory, &nonotes, gen_fetched, &null)))
		{
		int params[2];
		int sig [] = {3 | (1 << 16), status, 1, element};
		params[0] = 1;
		params[1] = call_back;
		sys$putmsg (sig, &emit_one_line, 0, &params);
		return status;
		}

	gen_fetched->dsc$w_length = cms_trim (gen_fetched);

	/* Loop round and fetch all of the lines from the element */
	while (! failed (status = cms$fetch_get (&fdb[CTX_LDB],
	    &line, 0, 0, &null)))
		{
		line.dsc$w_length = cms_trim (&line);
		if (failed (status = insert_string (&line)))
			break;
		if (failed (status = insert_character ('\n')))
			break;
		line.dsc$w_length = MAX_LINE;
		}

	/* Close the fetch block. Ignore errors reported during fetch */
	cms$fetch_close (&fdb[CTX_LDB], &null);
	return goto_character (d);
	}
cms_fetch (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	int nohistory = 0, nonotes = 0;
	register struct dsc$descriptor *generation = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (generation_expr, STRING_SIZE);
	descriptor (gen_fetched, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,6)))
		return status;

	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the generation expression if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &generation_expr,
		    &generation_expr.dsc$w_length)))
			return status;
		else
			generation = &generation_expr;

	/* Fetch the nohistory flag if present */
	if (argc > 3)
		if (failed (status = numeric_arg (4, &nohistory)))
			return status;

	/* Fetch the nonotes flag if present */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &nonotes)))
			return status;

	/* Fetch the lines of the element and insert them into the current buffer */
	if (failed (status = cms_fetch_lines (context, &element_name, generation, &nohistory,
	    &nonotes, &gen_fetched)))
		return status;

	/* Poke the value of the fetched generation into the supplied parameter */
	if (argc > 5)
		status = return_string_to_param (6, &gen_fetched);

	return status;
	}
cms_reserve (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status, *ctx = *context;
	int nohistory = 0, nonotes = 0, concurrent = 1, reserve = 1;
	register struct dsc$descriptor *generation = 0, *remark = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (generation_expr, STRING_SIZE);
	descriptor (gen_fetched, STRING_SIZE);
	descriptor (remark_item, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,8)))
		return status;

	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the generation expression if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &generation_expr,
		    &generation_expr.dsc$w_length)))
			return status;
		else
			generation = &generation_expr;

	/* Fetch the remark if provided */
	if (argc > 3)
		if (failed (status = string_arg (4, &remark_item,
		    &remark_item.dsc$w_length)))
			return status;
		else
			remark = &remark_item;

	/* Fetch the nohistory flag if present */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &nohistory)))
			return status;

	/* Fetch the nonotes flag if present */
	if (argc > 5)
		if (failed (status = numeric_arg (6, &nonotes)))
			return status;

	/* Fetch the nonotes flag if present */
	if (argc > 6)
		if (failed (status = numeric_arg (7, &concurrent)))
			return status;

	/* Fetch the lines of the element and insert them into the current buffer */
	if (failed (status = cms_fetch_lines (context, &element_name, generation, &nohistory,
	    &nonotes, &gen_fetched)))
		return status;

	/* Reserve the element */
	if (failed (status = cms$fetch (&ctx [CTX_LDB], &element_name, remark,
	    generation, 0, &reserve, &nohistory, &nonotes, &concurrent,
	    &null_dev, &cms_message)))
		return status;

	/* Poke the value of the fetched generation into the supplied parameter */
	if (argc > 7)
		status = return_string_to_param (8, &gen_fetched);

	return status;
	}
cms_unreserve (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	int delete = 0;
	struct dsc$descriptor *remark = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,4)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Fetch the delete flag if present */
	if (argc > 3)
		if (failed (status = numeric_arg (4, &delete)))
			return status;

	/* Call CMS to provide the information */
	status = cms$unreserve (&(*context)[CTX_LDB],
	    &element_name, remark, 0, &delete, cms_message);

	return status;
	}
ann_output (first, ldb, context, element_id, output_id, eof_status)
    int *first, *ldb, **context, *element_id, *output_id, *eof_status;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	descriptor (buf, STRING_SIZE);

	/* Insert the element header for the first line */
	if (*first)
		{
		static char ann_text[] = "Annotated list for element ";
		static $DESCRIPTOR(ann, ann_text);
		
		cms$get_string(element_id, &buf);
		buf.dsc$w_length = cms_trim(&buf);
		insert_string (&ann);
		insert_string (&buf);
		insert_character ('\n');
		}

	/* At end of file, just plant a blank line */
	if (*eof_status == CMS$_EOF)
		insert_character ('\n');
	else
		{
		/* If there is a string, get it and insert it too */
		buf.dsc$w_length = STRING_SIZE;
		cms$get_string (output_id, &buf);
		buf.dsc$w_length = cms_trim(&buf);
		insert_string (&buf);
		insert_character ('\n');
		}
	return SS$_NORMAL;
	}
cms_annotate (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	int full = 0, d;
	struct dsc$descriptor *generation = 0, *merge = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (generation_name, STRING_SIZE);
	descriptor (merge_name, STRING_SIZE);
	descriptor (buf, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,5)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the generation if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &generation_name,
		    &generation_name.dsc$w_length)))
			return status;
		else
			generation = &generation_name;

	/* Fetch the merge generation if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &merge_name,
		    &merge_name.dsc$w_length)))
			return status;
		else
			merge = &merge_name;

	/* Fetch the full flag if present */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &full)))
			return status;

	err ("This service is unavailable due to a bug in CMS!");
	return status;

	/* Fetch dot for later restoration */
	if (failed (status = dot (&d, &buf, &buf.dsc$w_length)))
		return status;

	/* Call CMS to provide the information */
	status = cms$annotate (&(*context)[CTX_LDB],
	    &element_name, generation, merge, 0, &full, 0,
	    &ann_output, context, cms_message);

	/* Restore dot back again */
	goto_character (d);
	return status;
	}
cms_basic (context, routine)
    int **context, (*routine) ();
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	struct dsc$descriptor *remark = 0;
	descriptor (item_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,3)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the item_name */
	if (failed (status = string_arg (2, &item_name,
	    &item_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Call CMS to provide the information */
	status = (*routine) (&(*context)[CTX_LDB],
	    &item_name, remark, cms_message);

	return status;
	}
cms_insert_basic (context, routine)
    int **context, (*routine) ();
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	struct dsc$descriptor *remark = 0;
	int if_absent = 0;
	descriptor (item_name, STRING_SIZE);
	descriptor (target_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (3,5)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the item_name */
	if (failed (status = string_arg (2, &item_name,
	    &item_name.dsc$w_length)))
		return status;

	/* Fetch the target_name */
	if (failed (status = string_arg (3, &target_name,
	    &target_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Fetch the if_absent flag */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &if_absent)))
			return status;

	/* Call CMS to provide the information */
	status = (*routine) (&(*context)[CTX_LDB],
	    &item_name, &target_name, remark, &if_absent, cms_message);

	return status;
	}
cms_insert_generation (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc;
	register int status;
	struct dsc$descriptor *remark = 0, *generation = 0;
	int if_absent = 0, always = 0, supersede = 0;
	descriptor (item_name, STRING_SIZE);
	descriptor (target_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);
	descriptor (generation_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (3,8)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the item_name */
	if (failed (status = string_arg (2, &item_name,
	    &item_name.dsc$w_length)))
		return status;

	/* Fetch the target_name */
	if (failed (status = string_arg (3, &target_name,
	    &target_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Fetch the generation_expression if present */
	if (argc > 4)
		if (failed (status = string_arg (5, &generation_name,
		    &generation_name.dsc$w_length)))
			return status;
		else
			generation = &generation_name;

	/* Fetch the always flag */
	if (argc > 5)
		if (failed (status = numeric_arg (6, &always)))
			return status;

	/* Fetch the supersede flag */
	if (argc > 6)
		if (failed (status = numeric_arg (7, &always)))
			return status;

	/* Fetch the if_absent flag */
	if (argc > 7)
		if (failed (status = numeric_arg (8, &if_absent)))
			return status;

	/* Call CMS to provide the information */
	status = cms$insert_generation (&(*context)[CTX_LDB],
	    &item_name, &target_name, remark, generation, &always,
	    &supersede, &if_absent, cms_message);

	return status;
	}
cms_put_line (first, ldb, context, element_id, eof_status,
    sequence_flag, sequence_number)
    int *first, *ldb, **context, *element_id, *eof_status;
    int *sequence_flag, *sequence_number;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	register unsigned char *base = (*context)[CTX_BASE];
	register unsigned char *ptr = (*context)[CTX_PTR];
	register int size = (*context)[CTX_BUF_SIZE];
	register int status = SS$_NORMAL;
	struct dsc$descriptor line =
	    {0, DSC$K_DTYPE_T, DSC$K_CLASS_S, ptr};

	/* Check for multi elements */
	if (ptr == base && base == 0)
		{
		err ("Replaced only the first element of a multi-element request")
		return CMS$_STOPPED;
		}

	/* Check for EOF */
	if (ptr >= &base[size])
		{
		*eof_status = 1;
		cms$put_string (&line);
		(*context)[CTX_BASE] = 0;
		(*context)[CTX_PTR] = 0;
		(*context)[CTX_BUF_SIZE] = 0;
		return CMS$_NORMAL;
		}

	/* Find the end of the next line */
	while (ptr < &base[size] && *ptr != '\n')
		ptr++;

	/* Load up the lines descriptor and send the string */
	line.dsc$w_length = ptr - (unsigned char *)line.dsc$a_pointer;
	cms$put_string (&line);

	/* Skip over the linefeed and save away the pointer again */
	if (*ptr == '\n')
		ptr++;
	(*context)[CTX_PTR] = ptr;

	return CMS$_NORMAL;
	}

cms_replace (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	int reserve = 0, keep = 1;
	struct dsc$descriptor *remark = 0, *variant = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);
	descriptor (variant_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,6)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Fetch the variant if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &variant_name,
		    &variant_name.dsc$w_length)))
			return status;
		else
			variant = &variant_name;

	/* Fetch the reserve flag if present */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &reserve)))
			return status;

	/* Fetch the reserve flag if present */
	if (argc > 5)
		if (failed (status = numeric_arg (6, &keep)))
			return status;

	/*
	 * Make the current buffer have only one region and fetch the
	 * base address and size of that region. The GapTo is fairly
	 * expensive, but replaces will be done rarely so it does not
	 * really matter.
	 */

	GapTo (0x7FFFFFFF);
	buffer_extent (&size, &ptr, &junk, &junk);
	(*context)[CTX_BASE] = ptr;
	(*context)[CTX_PTR] = ptr+1;
	(*context)[CTX_BUF_SIZE] = size+1;

	/* Call CMS to provide the information */
	status = cms$replace (&(*context)[CTX_LDB],
	    &element_name, remark, variant, &reserve, &keep, 0,
	    &cms_put_line, context, cms_message);

	return status;
	}
cms_create_element (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	int reserve = 0, keep = 1, pos = 0, concurrent = 1, reference = 1;
	int *position = 0;
	struct dsc$descriptor *remark = 0, *history = 0, *notes = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);
	descriptor (history_name, STRING_SIZE);
	descriptor (notes_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,10)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Fetch the history if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &history_name,
		    &history_name.dsc$w_length)))
			return status;
		else
			history = &history_name;

	/* Fetch the notes if present */
	if (argc > 4)
		if (failed (status = string_arg (5, &notes_name,
		    &notes_name.dsc$w_length)))
			return status;
		else
			notes = &notes_name;

	/* Fetch the keep flag if present */
	if (argc > 5)
		if (failed (status = numeric_arg (6, &keep)))
			return status;

	/* Fetch the reserve flag if present */
	if (argc > 6)
		if (failed (status = numeric_arg (7, &reserve)))
			return status;

	/* Fetch the concurrent flag if present */
	if (argc > 7)
		if (failed (status = numeric_arg (8, &concurrent)))
			return status;

	/* Fetch the reference flag if present */
	if (argc > 8)
		if (failed (status = numeric_arg (9, &reference)))
			return status;

	/* Fetch the notes if present */
	if (argc > 9)
		if (failed (status = numeric_arg (10, &pos)))
			return status;
		else
			position = &pos;

	/*
	 * Make the current buffer have only one region and fetch the
	 * base address and size of that region. The GapTo is fairly
	 * expensive, but replaces will be done rarely so it does not
	 * really matter.
	 */

	GapTo (0x7FFFFFFF);
	buffer_extent (&size, &ptr, &junk, &junk);
	(*context)[CTX_BASE] = ptr;
	(*context)[CTX_PTR] = ptr+1;
	(*context)[CTX_BUF_SIZE] = size+1;

	/* Call CMS to provide the information */
	status = cms$create_element (&(*context)[CTX_LDB],
	    &element_name, remark, history, notes, position, &keep,
	    &reserve, &concurrent, &reference, 0,
	    &cms_put_line, context, cms_message);

	return status;
	}
cms_copy_element (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	struct dsc$descriptor *remark = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (new_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (3,4)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the element_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the new element name */
	if (failed (status = string_arg (3, &new_name,
	    &new_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			remark = &remark_name;

	/* Call CMS to provide the information */
	status = cms$copy_element (&(*context)[CTX_LDB],
	    &element_name, &new_name, remark, 0, cms_message);

	return status;
	}
cms_modify (context, rtn)
    int **context;
    int (*rtn)();
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	int *ro = 0, read_only = 0;
	struct dsc$descriptor *remark = 0, *new = 0, *new_rem = 0;
	descriptor (class_name, STRING_SIZE);
	descriptor (new_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);
	descriptor (new_remark, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,6)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the class_name */
	if (failed (status = string_arg (2, &class_name,
	    &class_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			if (remark_name.dsc$w_length)
				remark = &remark_name;

	/* Fetch the new name if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &new_name,
		    &new_name.dsc$w_length)))
			return status;
		else
			if (new_name.dsc$w_length)
				new = &new_name;

	/* Fetch the new remark if present */
	if (argc > 4)
		if (failed (status = string_arg (5, &new_remark,
		    &new_remark.dsc$w_length)))
			return status;
		else
			if (new_remark.dsc$w_length)
				new_rem = &new_remark;

	/* Fetch the new readonly flag if present */
	if (argc > 5)
		if (failed (status = numeric_arg (6, &read_only)))
			return status;
		else
			ro = &read_only;

	/* Call CMS to provide the information */
	status = (*rtn) (&(*context)[CTX_LDB],
	    &class_name, remark, new, new_rem, ro, cms_message);

	return status;
	}
cms_modify_element (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	int *pos = 0, position, *con = 0, concurrent, *ref = 0, reference;
	struct dsc$descriptor *remark = 0, *new = 0, *new_rem = 0;
	struct dsc$descriptor *history = 0, *notes = 0;
	descriptor (element_name, STRING_SIZE);
	descriptor (new_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);
	descriptor (new_remark, STRING_SIZE);
	descriptor (history_name, STRING_SIZE);
	descriptor (notes_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,10)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the class_name */
	if (failed (status = string_arg (2, &element_name,
	    &element_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			if (remark_name.dsc$w_length)
				remark = &remark_name;

	/* Fetch the new name if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &new_name,
		    &new_name.dsc$w_length)))
			return status;
		else
			if (new_name.dsc$w_length)
				new = &new_name;

	/* Fetch the new remark if present */
	if (argc > 4)
		if (failed (status = string_arg (5, &new_remark,
		    &new_remark.dsc$w_length)))
			return status;
		else
			if (new_remark.dsc$w_length)
				new_rem = &new_remark;

	/* Fetch the history if present */
	if (argc > 5)
		if (failed (status = string_arg (6, &history_name,
		    &history_name.dsc$w_length)))
			return status;
		else
			if (history_name.dsc$w_length)
				history = &history_name;

	/* Fetch the notes if present */
	if (argc > 6)
		if (failed (status = string_arg (7, &notes_name,
		    &notes_name.dsc$w_length)))
			return status;
		else
			if (notes_name.dsc$w_length)
				notes = &notes_name;

	/* Fetch the new concurrent flag if present */
	if (argc > 7)
		if (failed (status = numeric_arg (8, &concurrent)))
			return status;
		else
			con = &concurrent;

	/* Fetch the new reference flag if present */
	if (argc > 8)
		if (failed (status = numeric_arg (9, &reference)))
			return status;
		else
			ref = &reference;

	/* Fetch the new position flag if present */
	if (argc > 9)
		if (failed (status = numeric_arg (10, &position)))
			return status;
		else
			pos = &position;

	/* Call CMS to provide the information */
	status = cms$modify_element (&(*context)[CTX_LDB],
	    &element_name, remark, new, new_rem, history, notes, pos,
	    con, ref, cms_message);

	return status;
	}
cms_modify_library (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	struct dsc$descriptor *remark = 0;
	descriptor (remark_name, STRING_SIZE);
	descriptor (ref_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,3)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the reference name */
	if (failed (status = string_arg (2, &ref_name,
	    &ref_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 2)
		if (failed (status = string_arg (3, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			if (remark_name.dsc$w_length)
				remark = &remark_name;

	/* Call CMS to provide the information */
	status = cms$modify_library (&(*context)[CTX_LDB],
	     remark, &ref_name, cms_message);

	return status;
	}
cms_remove (context, rtn)
    int **context;
    int (*rtn)();
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	int if_present = 1;
	struct dsc$descriptor *remark = 0;
	descriptor (item_name, STRING_SIZE);
	descriptor (from_name, STRING_SIZE);
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (3,5)))
		return status;
	if (failed (status = arg_count (&argc)))
		return status;

	/* Fetch the item_name */
	if (failed (status = string_arg (2, &item_name,
	    &item_name.dsc$w_length)))
		return status;

	/* Fetch the from_name */
	if (failed (status = string_arg (3, &from_name,
	    &from_name.dsc$w_length)))
		return status;

	/* Fetch the remark if present */
	if (argc > 3)
		if (failed (status = string_arg (4, &remark_name,
		    &remark_name.dsc$w_length)))
			return status;
		else
			if (remark_name.dsc$w_length)
				remark = &remark_name;

	/* Fetch the new readonly flag if present */
	if (argc > 4)
		if (failed (status = numeric_arg (5, &if_present)))
			return status;

	/* Call CMS to provide the information */
	status = (*rtn) (&(*context)[CTX_LDB],
	    &item_name, &from_name, remark, &if_present, cms_message);

	return status;
	}
cms_remark (context)
    int **context;
	{
	register int (*call_back)() = (*context)[CTX_CALLBACK];
	int argc, junk, size, ptr;
	register int status;
	descriptor (remark_name, STRING_SIZE);

	/* Check that enough args were specified */
	if (failed (status = check_args (2,2)))
		return status;

	/* Fetch the remark if present */
	if (failed (status = string_arg (3, &remark_name,
	    &remark_name.dsc$w_length)))
		return status;

	/* Call CMS to provide the information */
	status = cms$remark (&(*context)[CTX_LDB],
	     &remark_name, cms_message);

	return status;
	}
