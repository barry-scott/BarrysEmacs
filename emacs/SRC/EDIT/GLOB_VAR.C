/*	Copyright (c) 1982, 1983, 1984, 1985
		Barry A. Scott and nick Emery */

/*
 * global_variables.c. Define all of the global storage for the
 * VAX/VMS EMACS editor
 */

/*
 * Define the PSECT into which the data that is saved and restored in
 * an EMACS environment file is located
 */

#include <emacs.h>

#define NOSAVE

/* Allocate the correct sapce in the mac_names Psects */
unsigned char *func_names [MAXMACS + 1];
struct bound_name *func_bodies [MAXMACS + 1];

/* Symbols from the Header files */
GLOBAL NOSAVE int is_false = (0 == 1);
GLOBAL NOSAVE int is_true = (0 == 0);
GLOBAL SAVRES int arg;
GLOBAL SAVRES enum arg_states arg_state;
GLOBAL SAVRES struct emacs_buffer *bf_cur;
GLOBAL SAVRES int buf_name_free;
GLOBAL SAVRES unsigned char **buf_names;
GLOBAL NOSAVE int cant_1line_opt;
GLOBAL NOSAVE int cant_1win_opt;
GLOBAL NOSAVE int cant_ever_opt;
GLOBAL SAVRES int col_valid;
GLOBAL NOSAVE struct prognode *cur_exec;
GLOBAL SAVRES int dot;
GLOBAL SAVRES int dot_col;
GLOBAL NOSAVE struct execution_stack execution_root;
GLOBAL SAVRES unsigned char key_mem [MEMLEN+2];
GLOBAL NOSAVE int last_arg_used;
GLOBAL SAVRES unsigned char last_key_struck;
GLOBAL SAVRES unsigned char last_keys_struck[LAST_KEYS_SIZE];
GLOBAL SAVRES int (*last_proc)(void);
GLOBAL SAVRES int mem_used;
GLOBAL SAVRES unsigned char *minibuf_body;
GLOBAL SAVRES int n_buffers;
GLOBAL SAVRES int n_macs;
GLOBAL NOSAVE short ospeed;
GLOBAL SAVRES int previous_command;
GLOBAL NOSAVE int recursive_edit_depth;
GLOBAL NOSAVE int redo_modes;
GLOBAL SAVRES int remembering;
GLOBAL SAVRES unsigned char *reset_minibuf;
GLOBAL SAVRES int this_command;

/* abspath.c */
GLOBAL NOSAVE unsigned char current_directory[MAXPATHLEN+1];

/* keyboard.c */
GLOBAL SAVRES int quitting_emacs = 0;

/* lispfuncs.c */
GLOBAL NOSAVE unsigned char *users_name;

/* minibuf.c */
GLOBAL SAVRES unsigned char error_message_text[BUFFERSIZE];	/* error message text area */
GLOBAL SAVRES int stack_trace_on_error;	/* if true, whenever an error is encountered
					   a stack trace will be dumped to the stack
					   trace buffer */
GLOBAL SAVRES int remove_help_window = 1;	/* if true, then the help window will be
        	                        	   removed on exit of any command completion
                	                	   routines. */
GLOBAL SAVRES int auto_help = 1;	/* true iff ambiguous or misspelled words
					   should create a help window (DJH) */
GLOBAL NOSAVE int minibuf_depth;

/* mlisp.c */
GLOBAL NOSAVE struct bound_name bound_number_node;
GLOBAL NOSAVE struct bound_name bound_string_node;
GLOBAL NOSAVE struct bound_name bound_variable_node;
GLOBAL NOSAVE struct bound_name bound_star_defun_node;
GLOBAL NOSAVE int in_error_occurred = 0;
GLOBAL SAVRES expression *ml_value;		/* the value returned from the last
						   evaluation */
GLOBAL SAVRES expression global_value;	/* The thing that ml_value usually points to */
GLOBAL SAVRES int err;

/* options.c */
GLOBAL SAVRES int image_context_lost = 0;
GLOBAL SAVRES int image_count  = 0;
GLOBAL SAVRES unsigned char **images;
GLOBAL SAVRES void **image_contexts;
GLOBAL SAVRES unsigned char **image_filenames;
GLOBAL SAVRES int trace_into = 1;
GLOBAL SAVRES int trace_mode = 0;		/* True if MLisp tracing is required */
GLOBAL NOSAVE int in_trace;		/* True if inside a trace functions */
GLOBAL SAVRES unsigned char error_hook [MLISP_HOOK_SIZE];
GLOBAL SAVRES unsigned char trace_hook [MLISP_HOOK_SIZE];
GLOBAL SAVRES unsigned char break_hook [MLISP_HOOK_SIZE];
GLOBAL SAVRES expression last_expression; /* last expression */

/* simplcoms.c */
GLOBAL SAVRES unsigned char *exit_emacs_dcl_command = 0;
GLOBAL SAVRES int track_eol = 1;		/* true iff ^n and ^p should stick with
						   eol's */
GLOBAL SAVRES int argument_prefix_cnt;

/* trmterm.c */
GLOBAL NOSAVE unsigned char *UP;
GLOBAL NOSAVE unsigned char *BC;
GLOBAL NOSAVE unsigned char PC_;
GLOBAL NOSAVE unsigned char *tbuf, *combuf;

/* trm_ansi.c */
GLOBAL SAVRES unsigned char mode_line_rendition [43+1] = {'7',0};
GLOBAL SAVRES unsigned char region_rendition [43+1] = {'1',0};
GLOBAL SAVRES unsigned char window_rendition [43+1] = {0};
GLOBAL SAVRES unsigned char syntax_keyword1_rendition[43+1];
GLOBAL SAVRES unsigned char syntax_keyword2_rendition[43+1];
GLOBAL SAVRES unsigned char syntax_word_rendition[43+1];
GLOBAL SAVRES unsigned char syntax_string_rendition[43+1];
GLOBAL SAVRES unsigned char syntax_quote_rendition[43+1];
GLOBAL SAVRES unsigned char syntax_comment_rendition[43+1];
GLOBAL SAVRES unsigned char user_1_rendition[43+1];
GLOBAL SAVRES unsigned char user_2_rendition[43+1];
GLOBAL SAVRES unsigned char user_3_rendition[43+1];
GLOBAL SAVRES unsigned char user_4_rendition[43+1];
GLOBAL SAVRES unsigned char user_5_rendition[43+1];
GLOBAL SAVRES unsigned char user_6_rendition[43+1];
GLOBAL SAVRES unsigned char user_7_rendition[43+1];
GLOBAL SAVRES unsigned char user_8_rendition[43+1];
