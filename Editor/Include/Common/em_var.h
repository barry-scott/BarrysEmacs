//
//    Copyright 1986-2002 Barry A. Scott
//

extern EmacsString default_environment_file;
extern EmacsString version_string;
extern SystemExpressionRepresentationTermProtocolMode protocol_mode;
extern int complete_reinit;
extern bool touched_command_args;
extern volatile int timer_interrupt_occurred;
extern QueueHeader<ProcTimerEntry> timer_list_head;
extern volatile int pending_channel_io;
extern EmacsCommandLine command_line_arguments;
extern SystemExpressionRepresentationIntReadOnly is_restored;
extern int in_trace;
extern SystemExpressionRepresentationIntBoolean trace_mode;
extern SystemExpressionRepresentationIntBoolean trace_into;
extern SystemExpressionRepresentationInt input_mode;
extern Expression last_expression;
extern int (*last_proc)(void);
extern BoundNameNoDefine bound_star_defun_node;
extern BoundName bound_defun_node;
extern SystemExpressionRepresentationStringReadOnly last_keys_struck;
extern EmacsChar_t last_key_struck;
extern KeyMap *current_global_map;
extern KeyMap *global_map;
extern KeyMap *next_global_keymap;
extern KeyMap *next_local_keymap;
extern EmacsMacroString key_mem;
extern int macro_replay_next;
extern int recursive_edit_depth;
extern int minibuf_depth;
extern int remembering;
extern ProgramNode *cur_exec;
extern EmacsBuffer *bf_cur;
extern EmacsBuffer *bf_prev;
extern EmacsBuffer *minibuf;
extern MiniBufferBody minibuf_body;
extern KeyMap *minibuf_local_ns_map;
extern KeyMap *minibuf_local_map;
extern SystemExpressionRepresentationIntBoolean keyboard_emulates_lk201_keypad;
extern SystemExpressionRepresentationStringReadOnly error_message_text;
extern SystemExpressionRepresentationString error_messages_buffer;
extern SystemExpressionRepresentationString keystroke_history_buffer;
extern SystemExpressionRepresentationString previous_directory;
extern BoundName *error_message_parser;
extern SystemExpressionRepresentationGraphicRendition mode_line_rendition;
extern SystemExpressionRepresentationGraphicRendition region_rendition;
extern SystemExpressionRepresentationGraphicRendition window_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_keyword1_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_keyword2_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_keyword3_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_word_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_string1_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_string2_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_string3_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_quote_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_comment1_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_comment2_rendition;
extern SystemExpressionRepresentationGraphicRendition syntax_comment3_rendition;
extern SystemExpressionRepresentationGraphicRendition user_1_rendition;
extern SystemExpressionRepresentationGraphicRendition user_2_rendition;
extern SystemExpressionRepresentationGraphicRendition user_3_rendition;
extern SystemExpressionRepresentationGraphicRendition user_4_rendition;
extern SystemExpressionRepresentationGraphicRendition user_5_rendition;
extern SystemExpressionRepresentationGraphicRendition user_6_rendition;
extern SystemExpressionRepresentationGraphicRendition user_7_rendition;
extern SystemExpressionRepresentationGraphicRendition user_8_rendition;
extern EmacsWindowRing *pushed_windows;
extern EmacsView *theActiveView;
extern SystemExpressionRepresentationIntReadOnly this_command;
extern SystemExpressionRepresentationIntReadOnly previous_command;
extern int end_of_mac;
extern EmacsChar_t activity_character;
extern SystemExpressionRepresentationIntBoolean activity_indicator;
extern SystemExpressionRepresentationCtrlXSwap swap_ctrl_x_char;
extern int dot;
extern int dot_col;
extern int col_valid;
extern int screen_garbaged;
extern int arg;
extern enum arg_states arg_state;
extern int argument_prefix_cnt;
extern int last_arg_used;
extern int cant_1line_opt;
extern int cant_1win_opt;
extern int cant_ever_opt;
extern int redo_modes;
extern Expression ml_value;
extern int ml_err;
extern int bf_journalling;
extern ExecutionStack *execution_root;
extern SystemExpressionRepresentationIntPositive term_is_terminal;
extern SystemExpressionRepresentationDisplayBoolean term_cursor_mode;
extern SystemExpressionRepresentationScreenLength term_length;
extern SystemExpressionRepresentationIntPositive mouse_enable;
extern SystemExpressionRepresentationScreenWidth term_width;
extern SystemExpressionRepresentationIntBoolean term_ansi;
extern SystemExpressionRepresentationIntBoolean term_deccrt;
extern SystemExpressionRepresentationIntBoolean term_deccrt2;
extern SystemExpressionRepresentationIntBoolean term_deccrt3;
extern SystemExpressionRepresentationIntBoolean term_deccrt4;
extern SystemExpressionRepresentationIntBoolean term_avo;
extern SystemExpressionRepresentationIntBoolean term_app_keypad;
extern SystemExpressionRepresentationIntBoolean term_nopadding;
extern SystemExpressionRepresentationIntBoolean term_edit;
extern SystemExpressionRepresentationIntBoolean term_eightbit;
extern SystemExpressionRepresentationIntBoolean term_regis;
extern SystemExpressionRepresentationTermOutputSize term_output_buffer_size;
extern SystemExpressionRepresentationIntReadOnly keyboard_input_pending;
extern SystemExpressionRepresentationIntBoolean ignore_version_numbers;
extern int last_redisplay_paused;
extern int quitting_emacs;
extern BoundNameNoDefine bound_variable_node;
extern volatile int input_pending;
extern SystemExpressionRepresentationStringReadOnly last_search_string;
extern EmacsSearch sea_glob;
extern BoundName *default_auto_fill_proc;
extern BoundName *auto_execute_proc;
extern SystemExpressionRepresentationIntBoolean default_display_c1;
extern SystemExpressionRepresentationIntBoolean default_display_eof;
extern SystemExpressionRepresentationIntBoolean default_read_only_buffer;
extern SystemExpressionRepresentationIntBoolean default_display_nonprinting;
extern SystemExpressionRepresentationIntBoolean default_replace_mode;
extern SystemExpressionRepresentationIntBoolean default_fold_case;
extern SystemExpressionRepresentationDefaultRightMargin default_right_margin;
extern SystemExpressionRepresentationDefaultLeftMargin default_left_margin;
extern SystemExpressionRepresentationIntPositive default_comment_column;
extern SystemExpressionRepresentationDefaultTabSize default_tab_size;
extern SystemExpressionRepresentationIntBoolean default_indent_use_tab;
extern SystemExpressionRepresentationIntBoolean default_highlight;
extern SystemExpressionRepresentationIntBoolean default_syntax_colouring;
extern SystemExpressionRepresentationIntBoolean default_syntax_array;
extern SystemExpressionRepresentationString default_mode_format;
extern SystemExpressionRepresentationIntPositive checkpoint_frequency;
extern FileAutoMode *auto_list;
extern BoundName *checkpoint_proc;
extern SystemExpressionRepresentationIntBoolean force_redisplay;
extern SyntaxTable *global_syntax_table;
extern AbbrevTable global_abbrev;
extern EmacsBuffer *buffers;
extern volatile int interrupt_key_struck;
extern SystemExpressionRepresentationIntBoolean unlink_checkpoint_files;
extern SystemExpressionRepresentationIntBoolean ask_about_buffer_names;
extern SystemExpressionRepresentationIntReadOnly is_read_only;
extern SystemExpressionRepresentationIntReadOnly is_read_write;
extern SystemExpressionRepresentationIntReadOnly is_not_accessible;
extern SystemExpressionRepresentationString current_directory;
extern EmacsString exit_emacs_dcl_command;
extern SystemExpressionRepresentationIntBoolean track_eol;
extern SystemExpressionRepresentationIntBoolean ctl_arrow;
extern EmacsString users_name;
extern EmacsFile message_file;
extern EmacsFile command_file;
extern SystemExpressionRepresentationStackMaxDepth stack_maximum_depth;
extern SystemExpressionRepresentationIntBoolean stack_trace_on_error;
extern SystemExpressionRepresentationIntBoolean remove_help_window;
extern int expand;
extern int help;
extern int in_error_occurred;
extern SystemExpressionRepresentationIntBoolean auto_help;

extern SystemExpressionRepresentationJournalFrequency journalling_frequency;
extern SystemExpressionRepresentationIntBoolean journal_scratch_buffers;

extern SystemExpressionRepresentationEndOfLineStyle default_end_of_line_style;
extern SystemExpressionRepresentationBufferEndOfLineStyle buffer_end_of_line_style;
extern SystemExpressionRepresentationEndOfLineStyle end_of_line_style_override;

extern SystemExpressionRepresentationStringReadOnly last_phrase;
extern SystemExpressionRepresentationIntBoolean animate_journal_recovery;
extern SystemExpressionRepresentationIntBoolean black_on_white;
extern SystemExpressionRepresentationIntBoolean visible_bell;
extern SystemExpressionRepresentationBackupFileMode backup_file_mode;
extern SystemExpressionRepresentationBufferCheckpointable current_buffer_checkpointable;
extern SystemExpressionRepresentationBufferFilename current_buffer_filename;
extern SystemExpressionRepresentationString compile_command;
extern SystemExpressionRepresentationString debug_command;
extern SystemExpressionRepresentationString filter_command;
extern SystemExpressionRepresentationString execute_command;
extern SystemExpressionRepresentationIntBoolean cs_enabled;
extern int cs_modified;
extern SystemExpressionRepresentationIntBoolean cs_cvt_f_keys;
extern SystemExpressionRepresentationIntBoolean cs_cvt_mouse;
extern SystemExpressionRepresentationControlString cs_cvt_8bit_string;
extern SystemExpressionRepresentationControlString cs_cvt_csi_string;
extern SystemExpressionRepresentationControlString cs_par_char_string;
extern SystemExpressionRepresentationControlString cs_par_sep_string;
extern SystemExpressionRepresentationControlString cs_int_char_string;
extern SystemExpressionRepresentationControlString cs_fin_char_string;
extern SystemExpressionRepresentationArray cs_parameters;
extern int bufcheckvar;
extern SystemExpressionRepresentationString typnamvar;
extern SystemExpressionRepresentationString bufnamevar;
extern SystemExpressionRepresentationIntBufferAllocSize buffer_alloc_size;
extern SystemExpressionRepresentationIntBoolean pop_up_windows;
extern SystemExpressionRepresentationIntBoolean replace_case;
extern SystemExpressionRepresentationIntBoolean default_replace;
extern SystemExpressionRepresentationIntBoolean quick_rd;
extern SystemExpressionRepresentationIntBoolean silently_kill_processes;
extern SystemExpressionRepresentationIntPositive scroll_step;
extern SystemExpressionRepresentationIntPositive scroll_step_horizontal;
extern SystemExpressionRepresentationIntBoolean automatic_horizontal_scroll;
extern SystemExpressionRepresentationIntPositive split_height_threshold;
extern SystemExpressionRepresentationIntBoolean default_wrap_lines;
extern SystemExpressionRepresentationString global_mode_string;
extern SystemExpressionRepresentationString error_file_name;
extern SystemExpressionRepresentationIntPositive error_line_number;
extern SystemExpressionRepresentationIntPositive error_column_number;
extern SystemExpressionRepresentationIntPositive error_start_position;
extern SystemExpressionRepresentationIntReadOnly is_false;
extern SystemExpressionRepresentationIntReadOnly is_true;
extern SystemExpressionRepresentationIntReadOnly is_dull;
extern SystemExpressionRepresentationIntReadOnly is_word;
extern SystemExpressionRepresentationIntReadOnly is_str;
extern SystemExpressionRepresentationIntReadOnly is_quote_character;
extern SystemExpressionRepresentationIntReadOnly is_comment;
extern SystemExpressionRepresentationIntReadOnly is_problem;
extern SystemExpressionRepresentationIntPositive maximum_shell_buffer_size;
extern SystemExpressionRepresentationIntPositive shell_buffer_reduction;
extern SystemExpressionRepresentationString cli_name;
extern SystemExpressionRepresentationIntPositive get_db_help_flags;
extern int must_checkpoint_on_crash;
extern SystemExpressionRepresentationStringReadOnly ui_type;
extern SystemExpressionRepresentationStringReadOnly operating_system_name;
extern SystemExpressionRepresentationStringReadOnly operating_system_version;
extern SystemExpressionRepresentationStringReadOnly CPU_type;
#if defined(_WINDOWS)
extern int win_emacs_quit;
extern int is_windows_nt;
#endif
extern BoundNameNoDefine interrupt_block;
extern BoundNameNoDefine lambda_block;
extern BoundNameNoDefine progn_block;

extern SystemExpressionRepresentationString ui_filter_file_list;
extern SystemExpressionRepresentationString ui_open_file_name;
extern SystemExpressionRepresentationString ui_save_as_file_name;
extern SystemExpressionRepresentationString ui_search_string;
extern SystemExpressionRepresentationString ui_replace_string;
extern SystemExpressionRepresentationInt ui_open_file_readonly;
extern unsigned char cs_attr[256];
extern BoundName *trace_proc;
extern BoundName *break_proc;
extern BoundName *error_proc;
extern BoundName *user_interface_hook_proc;
extern unsigned int last_auto_keymap;    // Index of last auto keymap
extern SystemExpressionRepresentationStringReadOnly current_function;
extern SystemExpressionRepresentationStringReadOnly current_break;
extern Expression *call_back_result;
extern unsigned char *call_back_str;
extern unsigned int call_back_len;

extern BoundName *buffer_choose_name_proc;
extern BoundName *buffer_backup_filename_proc;
extern BoundName *buffer_file_loaded_proc;
extern BoundName *buffer_file_reloaded_proc;
extern BoundName *buffer_saved_as_proc;

extern BoundName *enter_emacs_proc;
extern BoundName *exit_emacs_proc;
extern BoundName *leave_emacs_proc;
extern BoundName *return_to_emacs_proc;
extern SystemExpressionRepresentationCurrentWindows current_windows;
#ifdef SUBPROCESSES
extern int saved_buffer_count;
extern EmacsBuffer *saved_buffers[SAVED_BUFFER_MAX];
#endif
