//
//    glob_var.cpp
//
//    Copyright (c) 1982-1997
//        Barry A. Scott
//

#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


//// Symbols from the Header files
SystemExpressionRepresentationIntReadOnly is_false(0 == 1);
SystemExpressionRepresentationIntReadOnly is_true(0 == 0);
int arg;
enum arg_states arg_state;
EmacsBuffer *bf_cur;
EmacsBuffer *bf_prev;   // buffer before entering the minibuffer
int cant_1line_opt;
int cant_1win_opt;
int col_valid;
ProgramNode *cur_exec;
int dot;
int dot_col;
ExecutionStack *execution_root = NULL;
EmacsMacroString key_mem;
int last_arg_used;
EmacsChar_t last_key_struck;
SystemExpressionRepresentationStringReadOnly last_keys_struck;
MiniBufferBody minibuf_body;
SystemExpressionRepresentationIntReadOnly previous_command;
int recursive_edit_depth;
int redo_modes;
int remembering;
SystemExpressionRepresentationIntReadOnly this_command;
//
//// abspath.c
SystemExpressionRepresentationString current_directory;
SystemExpressionRepresentationString previous_directory;
//
//// keyboard.c
int quitting_emacs = 0;
//
//// lispfuncs.c
EmacsString users_name;
//
//// minibuf.c
SystemExpressionRepresentationStringReadOnly error_message_text;
    // error message text area
SystemExpressionRepresentationIntBoolean stack_trace_on_error;
    // if true, whenever an error is encountered
    // a stack trace will be dumped to the stack trace buffer
SystemExpressionRepresentationIntBoolean remove_help_window( 1 );
    // if true, then the help window will be
    // removed on exit of any command completion routines.
SystemExpressionRepresentationIntBoolean auto_help( 1 );
    // true iff ambiguous or misspelled words
    // should create a help window (DJH)
int minibuf_depth;

//// mlisp.c
int in_error_occurred = 0;
Expression ml_value;        // the value returned from the last evaluation
int ml_err;

// options.c
SystemExpressionRepresentationIntBoolean trace_into( 1 );
SystemExpressionRepresentationIntBoolean trace_mode( 0 );        // True if MLisp tracing is required
int in_trace;            // True if inside a trace functions
Expression last_expression;    // last expression
int (*last_proc)(void);

//// simplcoms.c
EmacsString exit_emacs_dcl_command;
SystemExpressionRepresentationIntBoolean track_eol( 1 );    // true iff ^n and ^p should stick with eol's
int argument_prefix_cnt;
//
//// trm_ansi.c
const char *mode_line_rendition_default = "1255;2255;3255;4000;5000;6255";
const char *region_rendition_default = "1255;2255;3255;4000;5000;6000";
const char *window_rendition_default = "";
const char *syntax_keyword1_rendition_default = "1000;2000;3255;4255;5255;6255";
const char *syntax_keyword2_rendition_default = "1255;2000;3000;4255;5255;6255";
const char *syntax_keyword3_rendition_default = "1255;2000;3000;4255;5255;6255";
const char *syntax_word_rendition_default = "";
const char *syntax_string1_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_string2_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_string3_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_quote_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_comment1_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_comment2_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *syntax_comment3_rendition_default = "1000;2128;3000;4255;5255;6255";
const char *user_1_rendition_default = "1255;2000;3000;4255;5255;6255";
const char *user_2_rendition_default = "1000;2255;3000;4255;5255;6255";
const char *user_3_rendition_default = "1000;2000;3255;4255;5255;6255";
const char *user_4_rendition_default = "1255;2255;3000;4255;5255;6255";
const char *user_5_rendition_default = "1255;2000;3255;4255;5255;6255";
const char *user_6_rendition_default = "1000;2255;3255;4255;5255;6255";
const char *user_7_rendition_default = "1255;2255;3255;4192;5192;6192";
const char *user_8_rendition_default = "1255;2255;3255;4085;5085;6085";


SystemExpressionRepresentationGraphicRendition mode_line_rendition("7");
SystemExpressionRepresentationGraphicRendition region_rendition("4");
SystemExpressionRepresentationGraphicRendition window_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_keyword1_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_keyword2_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_keyword3_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_word_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_string1_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_string2_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_string3_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_quote_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_comment1_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_comment2_rendition("");
SystemExpressionRepresentationGraphicRendition syntax_comment3_rendition("");
SystemExpressionRepresentationGraphicRendition user_1_rendition("");
SystemExpressionRepresentationGraphicRendition user_2_rendition("");
SystemExpressionRepresentationGraphicRendition user_3_rendition("");
SystemExpressionRepresentationGraphicRendition user_4_rendition("");
SystemExpressionRepresentationGraphicRendition user_5_rendition("");
SystemExpressionRepresentationGraphicRendition user_6_rendition("");
SystemExpressionRepresentationGraphicRendition user_7_rendition("");
SystemExpressionRepresentationGraphicRendition user_8_rendition("");
