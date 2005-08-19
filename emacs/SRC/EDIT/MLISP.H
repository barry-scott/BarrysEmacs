/*	Copyright (c) 1982, 1983, 1984, 1985 */
/*		Barry A. Scott and nick Emery */
/* Header file for dealing with values returned by mlisp functions */


#define MLISP_HOOK_SIZE 80	/* size of hook function names */
struct expression {	/* a value that can be returned from the */
/* evaluation of a MLisp expression */
	int exp_int;	/* an integer value */
	union exp_v_tag {
		unsigned char *v_string;	/* if IsString */
		struct marker *v_marker;	/* if ISMARKER */
		struct emacs_array *v_array;	/* if IsArray */
		struct windowring *v_windows;	/* if IsWindows */
		int *v_value;
		} exp_v;
	int exp_type;	/* the kind of expression we're dealing with */
	unsigned exp_release : 1;	/* true iff this fellow points to a string */
/* in managed memory */
	} ;
typedef struct expression expression;

struct variablename {	/* a name for a variable with a pointer */
/* to it's chain of interpretations. */
	unsigned char *v_name;	/* the name of the variable */
	struct binding *v_binding;	/* the most recent binding of this variable */
	} ;

union range_check
	{
	int (*check_int)( int, struct variablename *);
	int (*check_str)( unsigned char *, struct variablename *);
	int (*check_win)( struct windowring *, struct variablename *);
	int (*check_array)( struct emacs_array *, struct variablename *);
	};

union thunk_rtn
	{
	int (*thunk_int)( struct expression *);
	int (*thunk_exp)( struct expression *, int );
	int (*thunk_arr)( struct emacs_array *, int, ... );
	};

struct binding {	/* a particular (name,value) binding */
	struct binding *b_inner;	/* the next inner binding for the same name */
	struct expression *b_exp;	/* The value held by this variable */
	union range_check b_rangecheck;
	/* routine to check_ the range of values */
	/* stored in this binding when set(q) */
	union thunk_rtn b_thunk;	/* Code to run to fetch_ value of variable */
				/* and load it into the expression structure */
	union bind_union {
		struct emacs_buffer *b_local_to;	/* The buffer that this binding is local too */
		struct binding *b_default;	/* The default value for this system variable */
		} b;
	unsigned b_is_system : 1;	/* true iff this is a system variable, */
					/* in which case b_string points to the */
					/* variable, be it string or integer. */
	unsigned b_buffer_specific : 1;	/* True iff the variable is buffer specific */
	unsigned b_is_default : 1;	/* True iff this is the default value entry */
	unsigned char fill_1 [3];
	} ;
/* */
/*	Used for lambda binding in *defun */
/* */
struct binding_list
	{
	struct binding bl_binding;
	struct binding_list *bl_flink;
	int bl_arg_index;
	};

#define ARRAY_MAX_DIMENSION 10
struct emacs_array
	{
	int array_ref_count;	/* number of referances to this structure */
	int array_dimensions;
	int array_lower_bound [10];	/* value of the lower bound */
	int array_size [10];	/* number of elements in this dimension */
	int array_total_size;	/* total number of elements in the array */
	struct expression *array_expr [1];
/* more expressions go in here */
	};

/* The data types possible for MLisp values */
#define ISVOID 0
#define ISINTEGER 1
#define ISSTRING 2
#define ISMARKER 3
#define ISWINDOWS 4
#define ISARRAY 5
struct execution_stack {	/* traceback/argument-evaluation stack for */
/* MLisp functions */
	struct prognode *es_cur_exec;	/* the expression being executed at this level */
	struct execution_stack *es_dyn_parent;	/* pointer to the dynamically enclosing parent of this execution frame */
	int es_prefix_argument;	/* The argument prefixed to this invocation */
	int es_prefix_argument_provided;	/* true iff there really was an argument */
/* prefixed to this invocation.  If there */
/* wasn't, then the value of PrefixArgument */
/* will be 1 */
	} ;
/* external-defun function description */
struct extern_func {
	unsigned char *ef_funcname;	/* Name of function to be called */
	int ef_image_index;	/* index into Image vectors */
	int (*ef_function)(int (*call_back)(int,...), void *, int *);	/* The address of the routine to be called */
	int ef_restore_count;	/* The restore-environment count at last */
				/* call to LIB$FIND_IMAGE_SYMBOL */
	void *ef_context;	/* This function's context */
	};

union p_args_union
	{
	int pa_int;
	unsigned char pa_char;
	struct prognode *pa_node;
	struct variablename *pa_name;
	};

struct prognode {			/* a node in an MLisp (minimal lisp) program node */
	struct bound_name *p_proc;		/* The dude that executes this node */
	int p_nargs;			/* The number of arguments to this node */
	unsigned p_active : 1;		/* True iff this node is being executed. */
	unsigned p_fill : 15;
	union p_args_union p_args[1];	/* The actual arguments -- this is really */
					/* an extensible array (!!!) */
	} ;
/* The things that an executable symbol can be bound to */
#define PROCBOUND 0	/* a wired-in procedure */
#define MACROBOUND 1	/* a macro (string) */
#define MLISPBOUND 2	/* an MLisp function */
#define AUTOLOADBOUND 3	/* a function to be autoloaded */
#define KEYBOUND 4	/* bound to a keymap */
#define EXTERNALBOUND 5	/* bound to some external, run-time loaded code */
#define MAXMACS 5000	/* maximum number of macro slots to allocate */

union bound_ptr {
	void *b_ptr;
	unsigned char *b_body;	/* 00 body of the macro to which this name is bound */
	int (*b_proc)(void);		/* 00 pointer to the procedure to which this name is bound */
	struct prognode *b_prog;	/* 00 The MLisp program node to which this name is bound */
	struct keymap *b_keymap;	/* 00 The keymap to which this name is bound */
	struct extern_func *b_func;	/* The external function description to which this function is bound */
	};

struct bound_name {			/* a name-procedure/macro binding */
	union bound_ptr b_bound;	/* 00 pointer to stuff */
	unsigned char *b_proc_name;	/* 04 the name to which this procedure or macro is bound */
	size_t b_proc_name_size;	/* 0c Size of this function's name */
	int b_binding;			/* 08 The kind of thing this symbol is bound to */
	unsigned b_active : 1;		/* 0c true iff this (macro) is active -- */
					/* prevents recursive macro calls */
	unsigned b_break : 1;		/* 0e true iff execution of this fuinction should case a break point */
	};
/* the possible states that the */
/* prefix-argument scanning could be in */
enum arg_states
	{
	no_arg,
	have_arg,
	prepared_arg
	};

struct usevar
	{
	struct usevar *usevar_next;
	struct emacs_buffer *usevar_buf;
	};

