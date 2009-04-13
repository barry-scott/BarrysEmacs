//    Copyright (c) 1982-2008
//        Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

// init the name table before declaring any static functions
FunctionNameTable BoundName::name_table( 5000, 1000 );

// define the "defun" command
BoundName bound_defun_node( "defun", define_function );


void init_fncs2( void )
{
    //
    //    Give us a way out of an emacs that fails to init keymaps
    //
    execute_mlisp_string( EmacsString("(bind-to-key \"exit-emacs\" \"\\^c\")") );
}

// Define all the funtion prototypes
extern int not_command(void);
extern int not_equal_command(void);
extern int mod_command(void);
extern int and_command(void);
extern int times_command(void);
extern int add_command(void);
extern int subtract_command(void);
extern int divide_command(void);
extern int less_command(void);
extern int shift_left_command(void);
extern int less_equal_command(void);
extern int equal_command(void);
extern int greater_command(void);
extern int greater_equal_command(void);
extern int shift_right_command(void);
# ifdef profile
extern int profile_emacs(void);
extern int dump_histogram(void);
# endif
# ifdef XWINDOWS
extern int motif_add_menu_keys_command(void);
extern int motif_add_menu_proc_command(void);
extern int ui_edit_copy(void);
extern int ui_frame_state(void);
extern int ui_frame_restore(void);
extern int ui_frame_to_foreground(void);
extern int motif_list_menus_command(void);
extern int ui_frame_minimize(void);
extern int ui_edit_paste(void);
extern int motif_remove_menu_command(void);
extern int ui_view_tool_bar(void);
extern int ui_view_scroll_bars(void);
extern int ui_view_status_bar(void);
extern int motif_add_tool_bar_button_keys_command(void);
extern int motif_add_tool_bar_button_proc_command(void);
extern int motif_add_tool_bar_separator_command(void);
extern int motif_remove_tool_bar_button_command(void);
extern int motif_list_tool_bars_command(void);
# endif
# if defined( _WINDOWS ) || defined( _MSDOS )
extern int ui_add_menu(void);
extern int ui_add_named_menu(void);
extern int ui_add_to_recent_file_list (void);
extern int ui_add_to_recent_directory_list (void);
extern int ui_edit_copy(void);
extern int ui_edit_paste(void);
extern int ui_file_open(void);
extern int ui_file_save_as(void);
extern int ui_find(void);
extern int ui_find_and_replace(void);
extern int ui_frame_state(void);
extern int ui_frame_maximize(void);
extern int ui_frame_minimize(void);
extern int ui_frame_restore(void);
extern int ui_frame_to_foreground(void);
extern int ui_list_menus(void);
extern int ui_remove_menu(void);
extern int ui_remove_named_menu(void);
extern int ui_select_buffer(void);
extern int ui_window_cascade(void);
extern int ui_window_new(void);
extern int ui_window_next(void);
extern int ui_window_previous(void);
extern int ui_window_maximize(void);
extern int ui_window_minimize(void);
extern int ui_window_restore(void);
extern int ui_window_tile_horz(void);
extern int ui_window_tile_vert(void);
extern int ui_win_exec(void);
extern int ui_shell_execute(void);
extern int ui_win_help(void);
extern int ui_view_tool_bar(void);
extern int ui_view_scroll_bars(void);
extern int ui_view_status_bar(void);
extern int ui_popup_menu(void);
# endif
# if defined( vms )
extern int vms_load_averages(void);
# endif

# ifdef XWINDOWS
extern int motif_XmCheckBox(void);
extern int motif_XmDestroyDialog(void);
extern int motif_XmDumpDialogs(void);
extern int motif_XmFileSelectionDialog(void);
extern int motif_XmFormDialog(void);
extern int motif_XmFrame(void);
extern int motif_XmIsDialog(void);
extern int motif_XmLabel(void);
extern int motif_XmMessageDialog(void);
extern int motif_XmOperateModalDialog(void);
extern int motif_XmOperateModelessDialog(void);
extern int motif_XmPushButtonApply(void);
extern int motif_XmPushButtonCancel(void);
extern int motif_XmPushButtonOK(void);
extern int motif_XmPushButtonUser(void);
extern int motif_XmRadioBox(void);
extern int motif_XmRowColumn(void);
extern int motif_XmScale(void);
extern int motif_XmSeparator(void);
extern int motif_XmTextField(void);
extern int motif_XmToggleButton(void);
extern int motif_XmUpdatedLabel(void);
# endif
extern int xor_command(void);
extern int append_region_to_buffer(void);
extern int append_to_file(void);
extern int apply_auto_execute(void);
extern int apply_colour_to_region_cmd(void);
extern int apropos(void);
extern int apropos_command(void);
extern int apropos_variable(void);
extern int arg_command(void);
extern int argc_command(void);
extern int argument_prefix(void);
extern int argv_command(void);
extern int argIsQualifier_command( void );
extern int array_command(void);
extern int auto_execute(void);
extern int auto_load(void);
extern int backward_paren_bl(void);
extern int backward_character(void);
extern int backward_paren(void);
extern int backward_word(void);
extern int baud_rate_command(void);
extern int beginning_of_file(void);
extern int beginning_of_line(void);
extern int beginning_of_window(void);
extern int bind_to_key(void);
extern int bobp_command(void);
extern int bolp_command(void);
extern int set_break_point(void);
extern int buf_size_command(void);
extern int char_compare_command(void);
extern int case_region_capitalize(void);
extern int case_region_invert(void);
extern int case_region_lower(void);
extern int case_region_upper(void);
extern int case_string_capitalize(void);
extern int case_string_invert(void);
extern int case_string_lower(void);
extern int case_string_upper(void);
extern int case_word_capitalize(void);
extern int case_word_invert(void);
extern int case_word_lower(void);
extern int case_word_upper(void);
extern int change_directory(void);
extern int char_to_string(void);
extern int checkpoint_everything(void);
extern int checkpoint_buffers(void);
extern int column_to_left_of_window(void);
extern int concat_command(void);
extern int convert_key_string_command (void);
extern int copy_region_to_buffer(void);
extern int current_buffer_name_command(void);
extern int cur_col_func_command(void);
extern int cur_dir(void);
extern int current_file_name_command(void);
extern int this_indent_command(void);
extern int current_line_command(void);
extern int current_process_name(void);
extern int current_syntax_entity(void);
extern int current_time(void);
extern int debug_emacs(void);
extern int declare_buffer_specific(void);
extern int declare_global(void);
extern int decompile_current_line(void);
extern int decompile_function(void);
extern int define_buffer_macro(void);
extern int define_global_abbrev(void);
extern int define_hooked_global_abbrev(void);
extern int define_hooked_local_abbrev(void);
extern int define_keyboard_macro(void);
extern int define_keymap_command(void);
extern int define_local_abbrev(void);
extern int define_string_macro(void);
extern int define_function(void);
extern int kill_buffer_command(void);
extern int delete_next_character(void);
extern int delete_next_word(void);
extern int delete_other_windows(void);
extern int delete_previous_character(void);
extern int delete_previous_word(void);
extern int delete_region_to_buffer(void);
extern int delete_to_kill_buffer(void);
extern int delete_white_space(void);
extern int delete_window(void);
extern int describe_bindings(void);
extern int describe_key(void);
extern int digit_command(void);
extern int dot_val_command(void);
extern int dot_is_visible(void);
extern int down_window(void);
extern int dump_abbreviation_tables (void);
extern int dump_memory_statistics(void);
extern int dump_stack_trace(void);
extern int dump_syntax_table(void);
extern int edit_macro(void);
extern int emacs_version(void);
extern int end_of_file(void);
extern int end_of_line(void);
extern int end_of_window(void);
extern int enlarge_window(void);
extern int eobp_command(void);
extern int eolp_command(void);
extern int erase_buffer(void);
extern int erase_region(void);
extern int error_and_exit(void);
extern int error_message(void);
extern int error_occured(void);
extern int exchange_dot_and_mark(void);
extern int execute_extended_command(void);
extern int execute_keyboard_macro(void);
extern int execute_mlisp_buffer(void);
extern int execute_mlisp_file_command(void);
extern int execute_mlisp_line(void);
extern int execute_monitor_command(void);
extern int exit_emacs(void);
extern int expand_and_exit(void);
extern int file_name_expand_and_default(void);
extern int file_is_a_directory_cmd(void);
extern int file_format_string_cmd(void);
extern int expand_file_name(void);
extern int expand_file_name_recursive(void);
extern int expand_from_string_table(void);
extern int extend_database_search_list(void);
extern int define_external_function(void);
extern int fetch_array_command(void);
extern int fetch_database_entry(void);
extern int fetch_help_database_entry(void);
extern int file_exists(void);
extern int filter_region(void);
extern int following_char_command(void);
extern int force_exit_process(void);
extern int forward_paren_bl(void);
extern int forward_character(void);
extern int forward_paren(void);
extern int forward_word(void);
extern int get_tty_buffer(void);
extern int get_tty_character(void);
extern int get_tty_command(void);
extern int get_tty_file(void);
extern int get_tty_directory(void);
extern int get_tty_string(void);
extern int get_tty_variable(void);
extern int getenv_command(void);
extern int global_binding_of(void);
extern int goto_character(void);
extern int goto_window_at_x_y (void);
extern int help_and_exit(void);
extern int if_command(void);
extern int illegal_operation(void);
extern int indent_c_procedure(void);
extern int insert_character(void);
extern int insert_file(void);
extern int insert_string(void);
extern int interactive_command(void);
extern int is_bound_command(void);
extern int is_function_command(void);
//    interrupt-key
extern int recover_journal(void);
extern int kill_buffer_command(void);
# if defined (SUBPROCESSES)
extern int kill_process(void);
# endif
extern int kill_to_end_of_line(void);
extern int fetch_last_key_struck_command(void);
extern int left_marker(void);
extern int left_window(void);
extern int length_command(void);
extern int line_to_top_of_window(void);
extern int list_abbreviation_tables (void);
extern int list_auto_executes (void);
extern int list_break_points(void);
extern int list_buffers(void);
extern int list_databases(void);
extern int list_images(void);
# if defined (SUBPROCESSES)
extern int list_processes(void);
# endif
extern int list_syntax_tables(void);
extern int execute_mlisp_file_command(void);
extern int local_bind_to_key(void);
extern int local_binding_of(void);
extern int looking_at(void);
extern int ere_looking_at(void);
extern int match_wild_command(void);
extern int mark_val_command(void);
extern int message_command(void);
extern int meta_digit(void);
extern int meta_minus(void);
extern int minus_command(void);
extern int modify_syntax_entry(void);
extern int modify_syntax_table(void);
extern int move_dot_to_x_y(void);
extern int move_to_comment_column(void);
extern int nargs_command(void);
extern int narrow_region(void);
extern int narrow_window(void);
extern int newline_command(void);
extern int newline_and_backup(void);
extern int newline_and_indent(void);
extern int next_error(void);
extern int next_line(void);
extern int next_page(void);
extern int next_window(void);
extern int no_value_command(void);
extern int page_next_window(void);
extern int parse_error_messages_in_region(void);
extern int pause_emacs(void);
# if defined (SUBPROCESSES)
extern int pause_process(void);
# endif
extern int pop_to_buffer(void);
extern int preceding_char_command(void);
extern int prefix_argument_loop(void);
extern int prepend_region_to_buffer(void);
extern int fetch_previous_command(void);
extern int previous_error(void);
extern int previous_line(void);
extern int previous_page(void);
extern int previous_window(void);
extern int print_command(void);
extern int print_default_command(void);
# if defined (__unix__) && defined (SUBPROCESSES)
extern int process_id(void);
# endif
extern int process_key(void);
# if defined (SUBPROCESSES)
extern int process_output(void);
extern int process_status(void);
# endif
extern int provide_prefix_argument(void);
extern int push_back_character(void);
extern int put_database_entry(void);
extern int putenv_command(void);
extern int quietly_read_abbrev_file(void);
# if defined (__unix__) && defined (SUBPROCESSES)
extern int quit_process(void);
# endif
extern int quote_command(void);
extern int quote_character(void);
extern int re_replace_search_text_command(void);
extern int re_replace_string(void);
extern int re_search_forward(void);
extern int re_search_reverse(void);
extern int ere_replace_string(void);
extern int ere_search_forward(void);
extern int ere_search_reverse(void);
extern int read_abbrev_file(void);
extern int read_file_command(void);
extern int recursion_depth_command(void);
extern int recursive_edit(void);
extern int redraw_display(void);
extern int region_around_match_command(void);
extern int region_to_string(void);
extern int remove_all_local_bindings(void);
extern int remove_binding(void);
extern int remove_database(void);
extern int remove_local_binding(void);
extern int rename_macro(void);
extern int replace_search_text_command(void);
extern int replace_string_command(void);
# if defined (SUBPROCESSES)
extern int resume_process(void);
# endif
extern int return_prefix_argument(void);
extern int return_to_monitor(void);
extern int right_marker(void);
extern int right_window(void);
extern int save_environment(void);
extern int save_excursion(void);
extern int save_restriction(void);
extern int save_window_excursion(void);
extern int schedule_procedure(void);
extern int scroll_one_column_left (void);
extern int scroll_one_column_right (void);
extern int scroll_one_line_down(void);
extern int scroll_one_line_up(void);
extern int search_forward(void);
extern int search_reverse(void);
extern int self_insert_command(void);
# if defined (__unix__) && defined (SUBPROCESSES)
extern int send_eof_to_process(void);
# endif
# if defined (SUBPROCESSES)
extern int send_string_to_process(void);
# endif
extern int send_string_to_terminal(void);
extern int set_command(void);
extern int set_default_command(void);
# if defined (SUBPROCESSES)
extern int set_current_process(void);
# endif
extern int set_mark_command(void);
# if defined (SUBPROCESSES)
extern int set_process_input_procedure(void);
extern int set_process_name(void);
extern int set_process_output_buffer(void);
extern int set_process_output_procedure(void);
extern int set_process_termination_proc(void);
#endif
extern int setq_command(void);
extern int setq_array_command(void);
extern int setq_default_command(void);
extern int shrink_window(void);
extern int sit_for(void);
extern int split_current_window(void);
extern int split_current_window_vertically(void);
extern int sprintf_cmd(void);
# if defined (SUBPROCESSES)
extern int start_dcl_process(void);
# endif
extern int start_remembering(void);
extern int stop_remembering(void);
extern int string_extract(void);
extern int string_index_of_first(void);
extern int string_index_of_last(void);
extern int string_index_of_string(void);
extern int string_to_char(void);
extern int synchronise_files(void);
extern int substr_command(void);
extern int switch_to_buffer(void);
extern int return_system_name_command(void);
extern int temp_use_buffer(void);
extern int test_abbrev_expand(void);
extern int to_col_command(void);
extern int top_level_command(void);
extern int transpose_characters(void);
extern int type_of_Expression_command(void);
extern int undo_command(void);
extern int undo_boundary(void);
extern int undo_more(void);
extern int unlink_file(void);
extern int un_set_mark_command(void);
extern int up_window(void);
extern int use_abbrev_table(void);
extern int use_global_map(void);
extern int use_local_map(void);
extern int use_old_buffer(void);
extern int use_syntax_table(void);
extern int use_variables_of_buffer_command(void);
extern int users_full_name_command(void);
extern int users_login_name_command(void);
extern int visit_file_command(void);
# if defined (SUBPROCESSES)
extern int wait_for_process_input_request(void);
# endif
extern int while_command(void);
extern int widen_region(void);
extern int widen_window(void);
extern int window_dimensions(void);
extern int write_abbrev_file(void);
extern int write_current_file(void);
extern int write_file_exit(void);
extern int write_modified_files(void);
extern int write_named_file_command(void);
extern int yank_buffer(void);
extern int yank_from_kill_buffer(void);
extern int or_command(void);

#ifdef EMACS_PYTHON_EXTENSION
int python_import_module(void);
int python_call_function(void);
int python_eval_string(void);
int python_exec_string(void);
#if !EMACS_PYTHON_EXTENSION_THREADED
int python_init(void);
#endif

#endif

//
// Define all the macros
//
static void init_fncs_a( void )
{
    EMACS_NEW BoundName( "!", not_command );
    EMACS_NEW BoundName( "!=", not_equal_command );
    EMACS_NEW BoundName( "%", mod_command );
    EMACS_NEW BoundName( "&", and_command );
    EMACS_NEW BoundName( "*", times_command );
    EMACS_NEW BoundName( "+", add_command );
    EMACS_NEW BoundName( "-", subtract_command );
    EMACS_NEW BoundName( "/", divide_command );
    EMACS_NEW BoundName( "<", less_command );
    EMACS_NEW BoundName( "<<", shift_left_command );
    EMACS_NEW BoundName( "<=", less_equal_command );
    EMACS_NEW BoundName( "=", equal_command );
    EMACS_NEW BoundName( ">", greater_command );
    EMACS_NEW BoundName( ">=", greater_equal_command );
    EMACS_NEW BoundName( ">>", shift_right_command );
# ifdef profile
    EMACS_NEW BoundName( "Profile-Emacs", profile_emacs );
    EMACS_NEW BoundName( "Profile-Emacs-Histogram", dump_histogram );
# endif
#ifdef EMACS_PYTHON_EXTENSION
#if !EMACS_PYTHON_EXTENSION_THREADED
    EMACS_NEW BoundName( "Python-initialise", python_init );
#endif
    EMACS_NEW BoundName( "Python-import", python_import_module );
    EMACS_NEW BoundName( "Python-call", python_call_function );
    EMACS_NEW BoundName( "Python-eval", python_eval_string );
    EMACS_NEW BoundName( "Python-exec", python_exec_string );
#endif
# ifdef XWINDOWS
    EMACS_NEW BoundName( "UI-add-menu-keys", motif_add_menu_keys_command );
    EMACS_NEW BoundName( "UI-add-menu-procedure", motif_add_menu_proc_command );
    EMACS_NEW BoundName( "UI-add-tool-bar-button-keys", motif_add_tool_bar_button_keys_command );
    EMACS_NEW BoundName( "UI-add-tool-bar-button-procedure", motif_add_tool_bar_button_proc_command );
    EMACS_NEW BoundName( "UI-add-tool-bar-separator", motif_add_tool_bar_separator_command );
    EMACS_NEW BoundName( "UI-edit-copy", ui_edit_copy );
    EMACS_NEW BoundName( "UI-edit-paste", ui_edit_paste );
    EMACS_NEW BoundName( "UI-frame-state", ui_frame_state );
    EMACS_NEW BoundName( "UI-frame-minimize", ui_frame_minimize );
    EMACS_NEW BoundName( "UI-frame-restore", ui_frame_restore );
    EMACS_NEW BoundName( "UI-frame-to-foreground", ui_frame_to_foreground );
    EMACS_NEW BoundName( "UI-list-menus", motif_list_menus_command );
    EMACS_NEW BoundName( "UI-list-tool-bars", motif_list_tool_bars_command );
    EMACS_NEW BoundName( "UI-remove-menu", motif_remove_menu_command );
    EMACS_NEW BoundName( "UI-remove-tool-bar-button", motif_remove_tool_bar_button_command );
    EMACS_NEW BoundName( "UI-view-scroll-bars", ui_view_scroll_bars );
    EMACS_NEW BoundName( "UI-view-status-bar", ui_view_status_bar );
    EMACS_NEW BoundName( "UI-view-tool-bar", ui_view_tool_bar );
# endif
# if defined( _WINDOWS ) || defined( _MSDOS )
    EMACS_NEW BoundName( "UI-add-menu", ui_add_menu );
    EMACS_NEW BoundName( "UI-add-named-menu", ui_add_named_menu );
    EMACS_NEW BoundName( "UI-add-to-recent-file-list", ui_add_to_recent_file_list );
    EMACS_NEW BoundName( "UI-add-to-recent-directory-list", ui_add_to_recent_directory_list );
    EMACS_NEW BoundName( "UI-edit-copy", ui_edit_copy );
    EMACS_NEW BoundName( "UI-edit-paste", ui_edit_paste );
    EMACS_NEW BoundName( "UI-file-open", ui_file_open );
    EMACS_NEW BoundName( "UI-file-save-as", ui_file_save_as );
    EMACS_NEW BoundName( "UI-find", ui_find );
    EMACS_NEW BoundName( "UI-find-and-replace", ui_find_and_replace );
    EMACS_NEW BoundName( "UI-frame-state", ui_frame_state );
    EMACS_NEW BoundName( "UI-frame-maximize", ui_frame_maximize );
    EMACS_NEW BoundName( "UI-frame-minimize", ui_frame_minimize );
    EMACS_NEW BoundName( "UI-frame-restore", ui_frame_restore );
    EMACS_NEW BoundName( "UI-frame-to-foreground", ui_frame_to_foreground );
    EMACS_NEW BoundName( "UI-list-menus", ui_list_menus );
    EMACS_NEW BoundName( "UI-remove-menu", ui_remove_menu );
    EMACS_NEW BoundName( "UI-remove-named-menu", ui_remove_named_menu );
    EMACS_NEW BoundName( "UI-select-buffer", ui_select_buffer );
    EMACS_NEW BoundName( "UI-window-cascade", ui_window_cascade );
    EMACS_NEW BoundName( "UI-window-maximize", ui_window_maximize );
    EMACS_NEW BoundName( "UI-window-minimize", ui_window_minimize );
    EMACS_NEW BoundName( "UI-window-new", ui_window_new );
    EMACS_NEW BoundName( "UI-window-next", ui_window_next );
    EMACS_NEW BoundName( "UI-window-previous", ui_window_previous );
    EMACS_NEW BoundName( "UI-window-restore", ui_window_restore );
    EMACS_NEW BoundName( "UI-window-tile-horizontal", ui_window_tile_horz );
    EMACS_NEW BoundName( "UI-window-tile-vertical", ui_window_tile_vert );
    EMACS_NEW BoundName( "UI-windows-execute-program", ui_win_exec  );
    EMACS_NEW BoundName( "UI-windows-shell-execute", ui_shell_execute  );
    EMACS_NEW BoundName( "UI-windows-help", ui_win_help  );
    EMACS_NEW BoundName( "UI-view-tool-bar", ui_view_tool_bar );
    EMACS_NEW BoundName( "UI-view-scroll-bars", ui_view_scroll_bars );
    EMACS_NEW BoundName( "UI-view-status-bar", ui_view_status_bar );
    EMACS_NEW BoundName( "UI-popup-menu", ui_popup_menu );
# endif
# if defined( vms )
    EMACS_NEW BoundName( "VMS-load-averages", vms_load_averages );
# endif
# ifdef XWINDOWS
    EMACS_NEW BoundName( "XmCheckBox", motif_XmCheckBox );
    EMACS_NEW BoundName( "XmDestroyDialog", motif_XmDestroyDialog );
    EMACS_NEW BoundName( "XmDumpDialogs", motif_XmDumpDialogs );
    EMACS_NEW BoundName( "XmFileSelectionDialog", motif_XmFileSelectionDialog );
    EMACS_NEW BoundName( "XmFormDialog", motif_XmFormDialog );
    EMACS_NEW BoundName( "XmFrame", motif_XmFrame );
    EMACS_NEW BoundName( "XmIsDialog", motif_XmIsDialog );
    EMACS_NEW BoundName( "XmLabel", motif_XmLabel );
    EMACS_NEW BoundName( "XmMessageDialog", motif_XmMessageDialog );
    EMACS_NEW BoundName( "XmOperateModalDialog", motif_XmOperateModalDialog );
    EMACS_NEW BoundName( "XmOperateModelessDialog", motif_XmOperateModelessDialog );
    EMACS_NEW BoundName( "XmPushButtonApply", motif_XmPushButtonApply );
    EMACS_NEW BoundName( "XmPushButtonCancel", motif_XmPushButtonCancel );
    EMACS_NEW BoundName( "XmPushButtonOK", motif_XmPushButtonOK );
    EMACS_NEW BoundName( "XmPushButtonUser", motif_XmPushButtonUser );
    EMACS_NEW BoundName( "XmRadioBox", motif_XmRadioBox );
    EMACS_NEW BoundName( "XmRowColumn", motif_XmRowColumn );
    EMACS_NEW BoundName( "XmScale", motif_XmScale );
    EMACS_NEW BoundName( "XmSeparator", motif_XmSeparator );
    EMACS_NEW BoundName( "XmTextField", motif_XmTextField );
    EMACS_NEW BoundName( "XmToggleButton", motif_XmToggleButton );
    EMACS_NEW BoundName( "XmUpdatedLabel", motif_XmUpdatedLabel );
# endif
    EMACS_NEW BoundName( "^", xor_command );
    EMACS_NEW BoundName( "append-region-to-buffer", append_region_to_buffer );
    EMACS_NEW BoundName( "append-to-file", append_to_file );
    EMACS_NEW BoundName( "apply-auto-execute", apply_auto_execute );
    EMACS_NEW BoundName( "apply-colour-to-region", apply_colour_to_region_cmd );
    EMACS_NEW BoundName( "apropos", apropos );
    EMACS_NEW BoundName( "apropos-command", apropos_command );
    EMACS_NEW BoundName( "apropos-variable", apropos_variable );
    EMACS_NEW BoundName( "arg", arg_command );
    EMACS_NEW BoundName( "argc", argc_command );
    EMACS_NEW BoundName( "argument-prefix", argument_prefix );
    EMACS_NEW BoundName( "argv", argv_command );
    EMACS_NEW BoundName( "arg-is-qualifier", argIsQualifier_command );
    EMACS_NEW BoundName( "array", array_command );
    EMACS_NEW BoundName( "auto-execute", auto_execute );
    EMACS_NEW BoundName( "autoload", auto_load );
    EMACS_NEW BoundName( "backward-balanced-paren-line", backward_paren_bl );
    EMACS_NEW BoundName( "backward-character", backward_character );
    EMACS_NEW BoundName( "backward-paren", backward_paren );
    EMACS_NEW BoundName( "backward-word", backward_word );
    EMACS_NEW BoundName( "baud-rate", baud_rate_command );
    EMACS_NEW BoundName( "beginning-of-file", beginning_of_file );
    EMACS_NEW BoundName( "beginning-of-line", beginning_of_line );
    EMACS_NEW BoundName( "beginning-of-window", beginning_of_window );
    EMACS_NEW BoundName( "bind-to-key", bind_to_key );
    EMACS_NEW BoundName( "bobp", bobp_command );
    EMACS_NEW BoundName( "bolp", bolp_command );
    EMACS_NEW BoundName( "bounds-of-array", (int (*)(void))EmacsArray::bounds_of_array_command );
    EMACS_NEW BoundName( "breakpoint", set_break_point );
    EMACS_NEW BoundName( "buffer-size", buf_size_command );
    EMACS_NEW BoundName( "c=", char_compare_command );
    EMACS_NEW BoundName( "case-region-capitalize", case_region_capitalize );
    EMACS_NEW BoundName( "case-region-invert", case_region_invert );
    EMACS_NEW BoundName( "case-region-lower", case_region_lower );
    EMACS_NEW BoundName( "case-region-upper", case_region_upper );
    EMACS_NEW BoundName( "case-string-capitalize", case_string_capitalize );
    EMACS_NEW BoundName( "case-string-invert", case_string_invert );
    EMACS_NEW BoundName( "case-string-lower", case_string_lower );
    EMACS_NEW BoundName( "case-string-upper", case_string_upper );
    EMACS_NEW BoundName( "case-word-capitalize", case_word_capitalize );
    EMACS_NEW BoundName( "case-word-invert", case_word_invert );
    EMACS_NEW BoundName( "case-word-lower", case_word_lower );
    EMACS_NEW BoundName( "case-word-upper", case_word_upper );
    EMACS_NEW BoundName( "change-directory", change_directory );
    EMACS_NEW BoundName( "char-to-string", char_to_string );
    EMACS_NEW BoundName( "checkpoint", checkpoint_everything );
    EMACS_NEW BoundName( "checkpoint-buffers", checkpoint_buffers );
    EMACS_NEW BoundName( "column-to-left-of-window", column_to_left_of_window );
    EMACS_NEW BoundName( "concat", concat_command );
    EMACS_NEW BoundName( "convert-key-string", convert_key_string_command  );
    EMACS_NEW BoundName( "copy-region-to-buffer", copy_region_to_buffer );
    EMACS_NEW BoundName( "current-buffer-name", current_buffer_name_command );
    EMACS_NEW BoundName( "current-column", cur_col_func_command );
    EMACS_NEW BoundName( "current-directory", cur_dir );
    EMACS_NEW BoundName( "current-file-name", current_file_name_command );
    EMACS_NEW BoundName( "current-indent", this_indent_command );
    EMACS_NEW BoundName( "current-line-number", current_line_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "current-process-name", current_process_name );
# endif
    EMACS_NEW BoundName( "current-syntax-entity", current_syntax_entity );
    EMACS_NEW BoundName( "current-time", current_time );
    EMACS_NEW BoundName( "debug-emacs", debug_emacs );
    EMACS_NEW BoundName( "declare-buffer-specific", declare_buffer_specific );
    EMACS_NEW BoundName( "declare-global", declare_global );
    EMACS_NEW BoundName( "decompile-current-line", decompile_current_line );
    EMACS_NEW BoundName( "decompile-mlisp-function", decompile_function );
    EMACS_NEW BoundName( "define-buffer-macro", define_buffer_macro );
    EMACS_NEW BoundName( "define-global-abbrev", define_global_abbrev );
    EMACS_NEW BoundName( "define-hooked-global-abbrev", define_hooked_global_abbrev );
    EMACS_NEW BoundName( "define-hooked-local-abbrev", define_hooked_local_abbrev );
    EMACS_NEW BoundName( "define-keyboard-macro", define_keyboard_macro );
    EMACS_NEW BoundName( "define-keymap", define_keymap_command );
    EMACS_NEW BoundName( "define-local-abbrev", define_local_abbrev );
    EMACS_NEW BoundName( "define-string-macro", define_string_macro );
    //    new BoundName( "defun", define_function );
    EMACS_NEW BoundName( "delete-buffer", kill_buffer_command );
    EMACS_NEW BoundName( "delete-next-character", delete_next_character );
    EMACS_NEW BoundName( "delete-next-word", delete_next_word );
    EMACS_NEW BoundName( "delete-other-windows", delete_other_windows );
    EMACS_NEW BoundName( "delete-previous-character", delete_previous_character );
    EMACS_NEW BoundName( "delete-previous-word", delete_previous_word );
    EMACS_NEW BoundName( "delete-region-to-buffer", delete_region_to_buffer );
    EMACS_NEW BoundName( "delete-to-killbuffer", delete_to_kill_buffer );
    EMACS_NEW BoundName( "delete-white-space", delete_white_space );
    EMACS_NEW BoundName( "delete-window", delete_window );
    EMACS_NEW BoundName( "describe-bindings", describe_bindings );
    EMACS_NEW BoundName( "describe-key", describe_key );
    EMACS_NEW BoundName( "digit", digit_command );
    EMACS_NEW BoundName( "dot", dot_val_command );
    EMACS_NEW BoundName( "dot-is-visible", dot_is_visible );
    EMACS_NEW BoundName( "down-window", down_window );
    EMACS_NEW BoundName( "dump-abbreviation-tables", dump_abbreviation_tables  );
    EMACS_NEW BoundName( "dump-memory-statistics", dump_memory_statistics );
    EMACS_NEW BoundName( "dump-stack-trace", dump_stack_trace );
    EMACS_NEW BoundName( "dump-syntax-table", dump_syntax_table );
    EMACS_NEW BoundName( "edit-macro", edit_macro );
    EMACS_NEW BoundName( "emacs-version", emacs_version );
    EMACS_NEW BoundName( "end-of-file", end_of_file );
    EMACS_NEW BoundName( "end-of-line", end_of_line );
    EMACS_NEW BoundName( "end-of-window", end_of_window );
    EMACS_NEW BoundName( "enlarge-window", enlarge_window );
    EMACS_NEW BoundName( "eobp", eobp_command );
    EMACS_NEW BoundName( "eolp", eolp_command );
    EMACS_NEW BoundName( "erase-buffer", erase_buffer );
    EMACS_NEW BoundName( "erase-region", erase_region );
    EMACS_NEW BoundName( "error-and-exit", error_and_exit );
    EMACS_NEW BoundName( "error-message", error_message );
    EMACS_NEW BoundName( "error-occurred", error_occured );
    EMACS_NEW BoundName( "exchange-dot-and-mark", exchange_dot_and_mark );
    EMACS_NEW BoundName( "execute-extended-command", execute_extended_command );
    EMACS_NEW BoundName( "execute-keyboard-macro", execute_keyboard_macro );
    EMACS_NEW BoundName( "execute-mlisp-buffer", execute_mlisp_buffer );
    EMACS_NEW BoundName( "execute-mlisp-file", execute_mlisp_file_command );
    EMACS_NEW BoundName( "execute-mlisp-line", execute_mlisp_line );
    EMACS_NEW BoundName( "execute-monitor-command", execute_monitor_command );
    EMACS_NEW BoundName( "exit-emacs", exit_emacs );
    EMACS_NEW BoundName( "expand-and-exit", expand_and_exit );
    EMACS_NEW BoundName( "expand-file-name", expand_file_name );
    EMACS_NEW BoundName( "expand-file-name-recursive", expand_file_name_recursive );
    EMACS_NEW BoundName( "expand-from-string-table", expand_from_string_table );
    EMACS_NEW BoundName( "extend-database-search-list", extend_database_search_list );
    EMACS_NEW BoundName( "external-function", define_external_function );
    EMACS_NEW BoundName( "fetch-array", fetch_array_command );
    EMACS_NEW BoundName( "fetch-database-entry", fetch_database_entry );
    EMACS_NEW BoundName( "fetch-help-database-entry", fetch_help_database_entry );
    EMACS_NEW BoundName( "file-exists", file_exists );
    EMACS_NEW BoundName( "file-name-expand-and-default", file_name_expand_and_default );
    EMACS_NEW BoundName( "file-is-a-directory", file_is_a_directory_cmd );
    EMACS_NEW BoundName( "file-format-string", file_format_string_cmd );
    EMACS_NEW BoundName( "filter-region", filter_region );
    EMACS_NEW BoundName( "following-char", following_char_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "force-exit-process", force_exit_process );
# endif
    EMACS_NEW BoundName( "forward-balanced-paren-line", forward_paren_bl );
    EMACS_NEW BoundName( "forward-character", forward_character );
    EMACS_NEW BoundName( "forward-paren", forward_paren );
    EMACS_NEW BoundName( "forward-word", forward_word );
    EMACS_NEW BoundName( "get-tty-buffer", get_tty_buffer );
    EMACS_NEW BoundName( "get-tty-character", get_tty_character );
    EMACS_NEW BoundName( "get-tty-command", get_tty_command );
    EMACS_NEW BoundName( "get-tty-directory", get_tty_directory );
    EMACS_NEW BoundName( "get-tty-file", get_tty_file );
    EMACS_NEW BoundName( "get-tty-string", get_tty_string );
    EMACS_NEW BoundName( "get-tty-variable", get_tty_variable );
    EMACS_NEW BoundName( "getenv", getenv_command );
    EMACS_NEW BoundName( "global-binding-of", global_binding_of );
    EMACS_NEW BoundName( "goto-character", goto_character );
    EMACS_NEW BoundName( "goto-window-at-x-y", goto_window_at_x_y  );
    EMACS_NEW BoundName( "help-and-exit", help_and_exit );
    EMACS_NEW BoundName( "if", if_command );
    EMACS_NEW BoundName( "illegal-operation", illegal_operation );
    EMACS_NEW BoundName( "indent-C-procedure", indent_c_procedure );
    EMACS_NEW BoundName( "insert-character", insert_character );
    EMACS_NEW BoundName( "insert-file", insert_file );
    EMACS_NEW BoundName( "insert-string", insert_string );
    EMACS_NEW BoundName( "interactive", interactive_command );
    EMACS_NEW BoundName( "is-bound", is_bound_command );
    EMACS_NEW BoundName( "is-function", is_function_command );
    //    interrupt-key
    EMACS_NEW BoundName( "journal-recover", recover_journal );
    EMACS_NEW BoundName( "kill-buffer", kill_buffer_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "kill-process", kill_process );
# endif
    EMACS_NEW BoundName( "kill-to-end-of-line", kill_to_end_of_line );
    //    new BoundNameset( "lambda", lambda_block );
    EMACS_NEW BoundName( "last-key-struck", fetch_last_key_struck_command );
    EMACS_NEW BoundName( "left-marker", left_marker );
    EMACS_NEW BoundName( "left-window", left_window );
    EMACS_NEW BoundName( "length", length_command );
    EMACS_NEW BoundName( "line-to-top-of-window", line_to_top_of_window );
    EMACS_NEW BoundName( "list-abbreviation-tables", list_abbreviation_tables  );
    EMACS_NEW BoundName( "list-auto-executes", list_auto_executes  );
    EMACS_NEW BoundName( "list-breakpoints", list_break_points );
    EMACS_NEW BoundName( "list-buffers", list_buffers );
    EMACS_NEW BoundName( "list-databases", list_databases );
    EMACS_NEW BoundName( "list-images", list_images );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "list-processes", list_processes );
# endif
    EMACS_NEW BoundName( "list-syntax-tables", list_syntax_tables );
    EMACS_NEW BoundName( "load", execute_mlisp_file_command );
    EMACS_NEW BoundName( "local-bind-to-key", local_bind_to_key );
    EMACS_NEW BoundName( "local-binding-of", local_binding_of );
    EMACS_NEW BoundName( "ere-looking-at", ere_looking_at );
    EMACS_NEW BoundName( "looking-at", looking_at );
    EMACS_NEW BoundName( "mark", mark_val_command );
    EMACS_NEW BoundName( "match-wild", match_wild_command );
    EMACS_NEW BoundName( "message", message_command );
    EMACS_NEW BoundName( "meta-digit", meta_digit );
    EMACS_NEW BoundName( "meta-minus", meta_minus );
    EMACS_NEW BoundName( "minus", minus_command );
    EMACS_NEW BoundName( "modify-syntax-entry", modify_syntax_entry );
    EMACS_NEW BoundName( "modify-syntax-table", modify_syntax_table );
    EMACS_NEW BoundName( "move-to-comment-column", move_to_comment_column );
    EMACS_NEW BoundName( "nargs", nargs_command );
    EMACS_NEW BoundName( "narrow-region", narrow_region );
    EMACS_NEW BoundName( "narrow-window", narrow_window );
    EMACS_NEW BoundName( "newline", newline_command );
    EMACS_NEW BoundName( "newline-and-backup", newline_and_backup );
    EMACS_NEW BoundName( "newline-and-indent", newline_and_indent );
    EMACS_NEW BoundName( "next-error", next_error );
    EMACS_NEW BoundName( "next-line", next_line );
    EMACS_NEW BoundName( "next-page", next_page );
    EMACS_NEW BoundName( "next-window", next_window );
    EMACS_NEW BoundName( "novalue", no_value_command );
    EMACS_NEW BoundName( "page-next-window", page_next_window );
    EMACS_NEW BoundName( "parse-error-messages-in-region", parse_error_messages_in_region );
    EMACS_NEW BoundName( "pause-emacs", pause_emacs );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "pause-process", pause_process );
# endif
    EMACS_NEW BoundName( "pop-to-buffer", pop_to_buffer );
    EMACS_NEW BoundName( "preceding-char", preceding_char_command );
    EMACS_NEW BoundName( "prefix-argument-loop", prefix_argument_loop );
    EMACS_NEW BoundName( "prepend-region-to-buffer", prepend_region_to_buffer );
    EMACS_NEW BoundName( "previous-command", fetch_previous_command );
    EMACS_NEW BoundName( "previous-error", previous_error );
    EMACS_NEW BoundName( "previous-line", previous_line );
    EMACS_NEW BoundName( "previous-page", previous_page );
    EMACS_NEW BoundName( "previous-window", previous_window );
    EMACS_NEW BoundName( "print", print_command );
    EMACS_NEW BoundName( "print-default", print_default_command );
# if defined (__unix__) && defined (SUBPROCESSES)
    EMACS_NEW BoundName( "process-id", process_id );
# endif
    EMACS_NEW BoundName( "process-key", process_key );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "process-output", process_output );
    EMACS_NEW BoundName( "process-status", process_status );
# endif
    //    new BoundNameset( "progn", progn_block );
    EMACS_NEW BoundName( "provide-prefix-argument", provide_prefix_argument );
    EMACS_NEW BoundName( "push-back-character", push_back_character );
    EMACS_NEW BoundName( "put-database-entry", put_database_entry );
    EMACS_NEW BoundName( "putenv", putenv_command );
    EMACS_NEW BoundName( "quietly-read-abbrev-file", quietly_read_abbrev_file );
# if defined (__unix__) && defined (SUBPROCESSES)
    EMACS_NEW BoundName( "quit-process", quit_process );
# endif
    EMACS_NEW BoundName( "quote", quote_command );
    EMACS_NEW BoundName( "quote-character", quote_character );
    EMACS_NEW BoundName( "re-replace-search-text", re_replace_search_text_command );
    EMACS_NEW BoundName( "re-replace-string", re_replace_string );
    EMACS_NEW BoundName( "re-search-forward", re_search_forward );
    EMACS_NEW BoundName( "re-search-reverse", re_search_reverse );
    EMACS_NEW BoundName( "ere-replace-string", ere_replace_string );
    EMACS_NEW BoundName( "ere-search-forward", ere_search_forward );
    EMACS_NEW BoundName( "ere-search-reverse", ere_search_reverse );
    EMACS_NEW BoundName( "read-abbrev-file", read_abbrev_file );
    EMACS_NEW BoundName( "read-file", read_file_command );
    EMACS_NEW BoundName( "recursion-depth", recursion_depth_command );
    EMACS_NEW BoundName( "recursive-edit", recursive_edit );
    EMACS_NEW BoundName( "redraw-display", redraw_display );
    EMACS_NEW BoundName( "region-around-match", region_around_match_command );
    EMACS_NEW BoundName( "region-to-string", region_to_string );
    EMACS_NEW BoundName( "remove-all-local-bindings", remove_all_local_bindings );
    EMACS_NEW BoundName( "remove-binding", remove_binding );
    EMACS_NEW BoundName( "remove-database", remove_database );
    EMACS_NEW BoundName( "remove-local-binding", remove_local_binding );
    EMACS_NEW BoundName( "rename-macro", rename_macro );
    EMACS_NEW BoundName( "replace-search-text", replace_search_text_command );
    EMACS_NEW BoundName( "replace-string", replace_string_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "resume-process", resume_process );
# endif
    EMACS_NEW BoundName( "return-prefix-argument", return_prefix_argument );
    EMACS_NEW BoundName( "return-to-monitor", return_to_monitor );
    EMACS_NEW BoundName( "right-marker", right_marker );
    EMACS_NEW BoundName( "right-window", right_window );
    EMACS_NEW BoundName( "save-environment", save_environment );
    EMACS_NEW BoundName( "save-excursion", save_excursion );
    EMACS_NEW BoundName( "save-restriction", save_restriction );
    EMACS_NEW BoundName( "save-window-excursion", save_window_excursion );
    EMACS_NEW BoundName( "schedule-procedure", schedule_procedure );
    EMACS_NEW BoundName( "scroll-one-column-left", scroll_one_column_left  );
    EMACS_NEW BoundName( "scroll-one-column-right", scroll_one_column_right  );
    EMACS_NEW BoundName( "scroll-one-line-down", scroll_one_line_down );
    EMACS_NEW BoundName( "scroll-one-line-up", scroll_one_line_up );
    EMACS_NEW BoundName( "search-forward", search_forward );
    EMACS_NEW BoundName( "search-reverse", search_reverse );
    EMACS_NEW BoundName( "self-insert", self_insert_command );
# if defined (__unix__) && defined (SUBPROCESSES)
    EMACS_NEW BoundName( "send-eof-to-process", send_eof_to_process );
# endif
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "send-string-to-process", send_string_to_process );
# endif
    EMACS_NEW BoundName( "send-string-to-terminal", send_string_to_terminal );
    EMACS_NEW BoundName( "set", set_command );
    EMACS_NEW BoundName( "set-default", set_default_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "set-current-process", set_current_process );
# endif
    EMACS_NEW BoundName( "set-mark", set_mark_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "set-process-input-procedure", set_process_input_procedure );
    EMACS_NEW BoundName( "set-process-name", set_process_name );
    EMACS_NEW BoundName( "set-process-output-buffer", set_process_output_buffer );
    EMACS_NEW BoundName( "set-process-output-procedure", set_process_output_procedure );
    EMACS_NEW BoundName( "set-process-termination-procedure", set_process_termination_proc );
# endif
    EMACS_NEW BoundName( "setq", setq_command );
    EMACS_NEW BoundName( "setq-array", setq_array_command );
    EMACS_NEW BoundName( "setq-default", setq_default_command );
    EMACS_NEW BoundName( "shrink-window", shrink_window );
    EMACS_NEW BoundName( "sit-for", sit_for );
    EMACS_NEW BoundName( "split-current-window", split_current_window );
    EMACS_NEW BoundName( "split-current-window-vertically", split_current_window_vertically );
    EMACS_NEW BoundName( "sprintf", sprintf_cmd );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "start-DCL-process", start_dcl_process );
    EMACS_NEW BoundName( "start-process", start_dcl_process );
# endif
    EMACS_NEW BoundName( "start-remembering", start_remembering );
    EMACS_NEW BoundName( "stop-remembering", stop_remembering );
    EMACS_NEW BoundName( "string-extract", string_extract );
    EMACS_NEW BoundName( "string-index-of-first", string_index_of_first );
    EMACS_NEW BoundName( "string-index-of-last", string_index_of_last );
    EMACS_NEW BoundName( "string-index-of-string", string_index_of_string );
    EMACS_NEW BoundName( "string-to-char", string_to_char );
    EMACS_NEW BoundName( "synchronise-files", synchronise_files );
    EMACS_NEW BoundName( "substr", substr_command );
    EMACS_NEW BoundName( "switch-to-buffer", switch_to_buffer );
    EMACS_NEW BoundName( "system-name", return_system_name_command );
    EMACS_NEW BoundName( "temp-use-buffer", temp_use_buffer );
    EMACS_NEW BoundName( "test-abbrev-expand", test_abbrev_expand );
    EMACS_NEW BoundName( "to-col", to_col_command );
    EMACS_NEW BoundName( "top-level", top_level_command );
    EMACS_NEW BoundName( "transpose-characters", transpose_characters );
    EMACS_NEW BoundName( "type-of-expression", type_of_Expression_command );
    EMACS_NEW BoundName( "undo", undo_command );
    EMACS_NEW BoundName( "undo-boundary", undo_boundary );
    EMACS_NEW BoundName( "undo-more", undo_more );
    EMACS_NEW BoundName( "unlink-file", unlink_file );
    EMACS_NEW BoundName( "unset-mark", un_set_mark_command );
    EMACS_NEW BoundName( "up-window", up_window );
    EMACS_NEW BoundName( "use-abbrev-table", use_abbrev_table );
    EMACS_NEW BoundName( "use-global-map", use_global_map );
    EMACS_NEW BoundName( "use-local-map", use_local_map );
    EMACS_NEW BoundName( "use-old-buffer", use_old_buffer );
    EMACS_NEW BoundName( "use-syntax-table", use_syntax_table );
    EMACS_NEW BoundName( "use-variables-of-buffer", use_variables_of_buffer_command );
    EMACS_NEW BoundName( "users-full-name", users_full_name_command );
    EMACS_NEW BoundName( "users-login-name", users_login_name_command );
    EMACS_NEW BoundName( "visit-file", visit_file_command );
# if defined (SUBPROCESSES)
    EMACS_NEW BoundName( "wait-for-process-input-request", wait_for_process_input_request );
# endif
    EMACS_NEW BoundName( "while", while_command );
    EMACS_NEW BoundName( "widen-region", widen_region );
    EMACS_NEW BoundName( "widen-window", widen_window );
    EMACS_NEW BoundName( "window-dimensions", window_dimensions );
    EMACS_NEW BoundName( "write-abbrev-file", write_abbrev_file );
    EMACS_NEW BoundName( "write-current-file", write_current_file );
    EMACS_NEW BoundName( "write-file-exit", write_file_exit );
    EMACS_NEW BoundName( "write-modified-files", write_modified_files );
    EMACS_NEW BoundName( "write-named-file", write_named_file_command );
    EMACS_NEW BoundName( "yank-buffer", yank_buffer );
    EMACS_NEW BoundName( "yank-from-killbuffer", yank_from_kill_buffer );
    EMACS_NEW BoundName( "|", or_command );
}

void init_fncs( void )
{
    init_fncs_a();
    //init_fncs_b();

    progn_block.define();
    lambda_block.define();
    interrupt_block.define();
}
