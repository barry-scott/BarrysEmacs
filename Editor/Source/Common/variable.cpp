//
//    Copyright (c) 1995-2002
//        Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );


extern SystemExpressionRepresentationIntBoolean complete_unique_choices;
extern SystemExpressionRepresentationIntBoolean confirm_expansion_choice;
extern SystemExpressionRepresentationIntBoolean ask_about_synchronise_for_none_modified_buffers;
extern SystemExpressionRepresentationIntBoolean synchronise_buffers_on_focus;
extern SystemExpressionRepresentationString backup_filename_format;
extern SystemExpressionRepresentationInt maximum_file_read_size;

#define VarRep( name, rep ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( rep ) ) ); }
#define Var0( name, exp_class ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( ser = EMACS_NEW SystemExpressionRepresentation##exp_class() ) ) ); }
#define Var( name, exp_class, addr ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( ser = EMACS_NEW SystemExpressionRepresentation##exp_class( addr ) ) ) ); }

#define DefVar( name, exp_class, addr ) \
{ varname = EMACS_NEW VariableName( name, b = Def = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( ser = EMACS_NEW SystemExpressionRepresentation##exp_class( addr ) ), int(1) ) ); }
#define DefVarRep( name, rep ) \
{ varname = EMACS_NEW VariableName( name, b = Def = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( rep ), int(1) ) ); }
#define VarWithDef( name, exp_class, addr ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( ser = EMACS_NEW SystemExpressionRepresentation##exp_class( addr ) ), Def ) ); }
#define VarWithDef0( name, exp_class ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( ser = EMACS_NEW SystemExpressionRepresentation##exp_class() ), Def ) ); }
#define VarWithDefRep( name, rep ) \
{ varname = EMACS_NEW VariableName( name, b = EMACS_NEW Binding( se = EMACS_NEW SystemExpression( rep ), Def ) ); }

#define INI_VAR_T_SIZE (148+250)

VariableNameTable VariableName::name_table( 4096, 1024 );




void restore_var(void)
{
    VariableName *varname = NULL;
    Binding *Def = NULL;
    Binding *b = NULL;
    SystemExpression *se = NULL;
    ExpressionRepresentation *ser = NULL;

#if defined( SUBPROCESS )
    //
    // here a a set of VMS specific names which now have
    // neutral or more generally applicable names
    //
    VarRep("maximum-DCL-buffer-size", &maximum_dcl_buffer_size)
    VarRep("DCL-buffer-reduction", &dcl_buffer_reduction)

    DefVarRep("default-buffer-RMS-record-attribute", &default_rms_attribute)
    VarRep("current-buffer-RMS-record-attribute", &buffer_rms_attribute)
    VarRep("override-RMS-record-attribute", &rms_attribute_override)
#endif
    VarRep("fetch-help-database-flags", &get_db_help_flags)

    // end of VMS inspired variable

#ifdef EMACS_PYTHON_EXTENSION
    VarRep("Python-flags", &python_flags)
#endif
    VarRep("UI-filter-file-list", &ui_filter_file_list)
    VarRep("UI-open-file-name", &ui_open_file_name)
    VarRep("UI-open-file-readonly", &ui_open_file_readonly)
    VarRep("UI-save-as-file-name", &ui_save_as_file_name)
    VarRep("UI-search-string", &ui_search_string)
    VarRep("UI-replace-string", &ui_replace_string)

#if defined( SUBPROCESS )
    VarRep("shell-buffer-reduction", &dcl_buffer_reduction)
    VarRep("maximum-shell-buffer-size", &maximum_dcl_buffer_size)
#endif

    DefVarRep("default-buffer-end-of-line-style", &default_end_of_line_style)
    VarRep("current-buffer-end-of-line-style", &buffer_end_of_line_style)
    VarRep("override-end-of-line-style", &end_of_line_style_override)

    VarRep("cli-name", &cli_name)
    VarRep("abbrev-expansion", &last_phrase)
    Var0("abbrev-mode", AbbrevOn)
    VarRep("activity-indicator", &activity_indicator)
    VarRep("animated-journal-recovery", &animate_journal_recovery)
    VarRep("ask-about-buffer-names", &ask_about_buffer_names)
    VarRep("ask-about-synchronise-for-none-modified-buffers", &ask_about_synchronise_for_none_modified_buffers)
    Var("auto-execute-hook", BoundName, &auto_execute_proc)
    DefVar("default-auto-fill-hook", BoundName, &default_auto_fill_proc)
    VarWithDef0("auto-fill-hook", AutoFillHook)
    VarRep("backup-file-mode", &backup_file_mode)
    VarRep("backup-filename-format", &backup_filename_format)
    VarRep("black-on-white", &black_on_white)
    Var("breakpoint-hook", BoundName, &break_proc)
    Var0("buffer-is-modified", BufferModified)
    Var0("buffer-names", BufferNames )
    DefVarRep("default-case-fold-search", &default_fold_case)
    VarWithDef0("case-fold-search", FoldCase)
    VarWithDefRep("checkpoint-frequency", &checkpoint_frequency)
    Var("checkpoint-hook", BoundName, &checkpoint_proc)
    DefVarRep("default-comment-column", &default_comment_column)
    VarWithDef0("comment-column", CommentColumn)
    VarRep("compile-it-command",  &compile_command)
    VarRep("confirm-expansion-choice", &confirm_expansion_choice );
    VarRep("control-string-convert-F-keys", &cs_cvt_f_keys)
    VarRep("control-string-convert-mouse", &cs_cvt_mouse)
    VarRep("control-string-convert-to-8-bit", &cs_cvt_8bit_string)
    VarRep("control-string-convert-to-CSI", &cs_cvt_csi_string)
    VarRep("control-string-final-characters", &cs_fin_char_string)
    VarRep("control-string-intermediate-characters", &cs_int_char_string)
    VarRep("control-string-parameters", &cs_parameters )
    VarRep("control-string-parameter-characters", &cs_par_char_string)
    VarRep("control-string-parameter-separators", &cs_par_sep_string)
    VarRep("control-string-processing", &cs_enabled)
    VarRep("current-breakpoint", &current_break)
    Var0("current-buffer-abbrev-table", AbbrevTable)
    Var0("current-buffer-allocation-size", IntBufferAllocSize)
    Var0("current-buffer-checkpointable", BufferCheckpointable)
    Var0("current-buffer-file-name", BufferFilename)
    Var0("current-buffer-journalled", BufferJournalled)
    Var0("current-buffer-macro-name", BufferMacroName)
    Var0("current-buffer-name", BufferName)
    Var0("current-buffer-syntax-table", BufferSyntaxTable)
    Var0("current-buffer-type", BufferType)
    VarRep("current-function", &current_function)
    Var0("current-local-keymap", BufferKeymap)
    Var0("current-windows", CurrentWindows)
    VarRep("ctlchar-with-^", &ctl_arrow)
    VarRep("debug-it-command", &debug_command)
    DefVarRep("default-display-C1-as-graphics", &default_display_c1)
    VarWithDef0("display-C1-as-graphics", DisplayBooleanC1)
    DefVarRep("default-display-end-of-file", &default_display_eof)
    VarWithDef0("display-end-of-file", DisplayBooleanEOF)
    DefVarRep("default-display-non-printing-characters", &default_display_nonprinting)
    VarWithDef0("display-non-printing-characters", DisplayBooleanNonPrinting)
    Var0("elapse-time", ElapseTime );
    Var("enter-emacs-hook", BoundName, &enter_emacs_proc)
    VarRep("error-file-name", &error_file_name)
    Var("error-hook", BoundName, &error_proc)
    VarRep("error-message", &error_message_text)
    VarRep("error-messages-buffer", &error_messages_buffer)
    Var("error-message-parser", BoundName, &error_message_parser)
    VarRep("error-line-number", &error_line_number)
    VarRep("error-start-position", &error_start_position)
    VarRep("execute-monitor-command", &execute_command)
    Var("exit-emacs-hook", BoundName, &exit_emacs_proc)
    VarRep("expansion-only-completes-unique-choices", &complete_unique_choices );
    VarRep("false", &is_false)
    VarRep("filter-region-command", &filter_command)

    Var("buffer-choose-name-hook", BoundName, &buffer_choose_name_proc )
    Var("buffer-backup-filename-hook", BoundName, &buffer_backup_filename_proc )
    Var("buffer-file-loaded-hook", BoundName, &buffer_file_loaded_proc )
    Var("buffer-saved-as-hook", BoundName, &buffer_saved_as_proc )

    VarRep("force-redisplay", &force_redisplay)
    VarRep("global-mode-string", &global_mode_string)
    VarRep("help-on-command-completion-error", &auto_help)
    DefVarRep("default-highlight-region", &default_highlight)
    VarWithDef0("highlight-region", DisplayBooleanHighlight)
    VarRep("highlight-region-graphic-rendition", &region_rendition)
    DefVarRep("default-indent-use-tab", &default_indent_use_tab)
    VarWithDef0("indent-use-tab", IndentUseTab)
    VarRep("input-mode", &input_mode );
    VarRep("is-comment", &is_comment)
    VarRep("is-dull", &is_dull)
    VarRep("is-quote", &is_quote_character)
    VarRep("is-string", &is_str)
    VarRep("is-word", &is_word)
    VarRep("journal-frequency", &journalling_frequency )
    VarRep("journal-scratch-buffers", &journal_scratch_buffers)
    VarRep("keyboard-input-available", &keyboard_input_pending )
    VarRep("keyboard-emulates-lk201-keypad", &keyboard_emulates_lk201_keypad )
    VarRep("keystroke-history-buffer", &keystroke_history_buffer )
{ EMACS_NEW VariableName( "last-expression", EMACS_NEW Binding( &last_expression ) ); }
    VarRep("last-keys-struck", &last_keys_struck)
    DefVarRep("default-left-margin", &default_left_margin)
    VarWithDef0("left-margin", LeftMargin)
    Var("leave-emacs-hook", BoundName, &leave_emacs_proc)
    VarRep("maximum-file-read-size", &maximum_file_read_size)
    DefVarRep("default-mode-line-format",  &default_mode_format)
    VarWithDef0("mode-line-format", ModeFormat)
    VarRep("mode-line-graphic-rendition", &mode_line_rendition)
    Var0("mode-string", ModeString)
    VarRep("mouse-enable", &mouse_enable)
    VarRep("not-accessible", &is_not_accessible)
    VarRep("omit-file-version-expansion", &ignore_version_numbers)
    VarRep("operating-system-name", &operating_system_name )
    VarRep("operating-system-version", &operating_system_version )
    VarRep("CPU-type", &CPU_type )
    VarRep("pop-up-windows", &pop_up_windows)
    Var0("prefix-argument", PrefixArgument)
    Var0("prefix-argument-provided", PrefixArgumentProvided)
    Var0("prefix-string",  PrefixString)
    VarRep("previous-directory", &previous_directory );
    Var0("process-key-hook", ProcessKeyHook)
    VarRep("quick-redisplay", &quick_rd)
    VarRep("read-only", &is_read_only)
    DefVarRep("default-read-only-buffer", &default_read_only_buffer)
    VarWithDef0("read-only-buffer", ReadOnly)
    VarRep("read-write", &is_read_write)
    VarRep("remove-help-window", &remove_help_window)
    VarRep("replace-case", &replace_case)
    DefVarRep("default-replace-mode", &default_replace)
    VarWithDef0("replace-mode", ReplaceMode)
    VarRep("restored-environment", &is_restored)
    Var("return-to-emacs-hook", BoundName, &return_to_emacs_proc)
    DefVarRep("default-right-margin", &default_right_margin)
    VarWithDef0("right-margin", RightMargin)
    VarRep("scroll-step", &scroll_step)
    VarRep("horizontal-scroll-step", &scroll_step_horizontal)
    VarRep("automatic-horizontal-scrolling", &automatic_horizontal_scroll)
    VarRep("search-string", &last_search_string)
    VarRep("silently-kill-processes", &silently_kill_processes)
    VarRep("split-height-threshold", &split_height_threshold)
    VarRep("stack-trace-on-error", &stack_trace_on_error)
    VarRep("stack-maximum-depth", &stack_maximum_depth)
    VarRep("swap-ctrl-x-char", &swap_ctrl_x_char)
    VarRep("synchronise-buffers-on-focus", &synchronise_buffers_on_focus)
    DefVarRep("default-syntax-colouring", &default_syntax_colouring)
    VarWithDef0("syntax-colouring", SyntaxColouring)
    DefVarRep("default-syntax-array", &default_syntax_array)
    VarWithDef0("syntax-array", SyntaxArray)
    VarRep("syntax-word-rendition", &syntax_word_rendition)
    VarRep("syntax-string-rendition", &syntax_string1_rendition)
    VarRep("syntax-string1-rendition", &syntax_string1_rendition)
    VarRep("syntax-string2-rendition", &syntax_string2_rendition)
    VarRep("syntax-string3-rendition", &syntax_string3_rendition)
    VarRep("syntax-comment-rendition", &syntax_comment1_rendition)
    VarRep("syntax-comment1-rendition", &syntax_comment1_rendition)
    VarRep("syntax-comment2-rendition", &syntax_comment1_rendition)
    VarRep("syntax-comment3-rendition", &syntax_comment1_rendition)
    VarRep("syntax-keyword1-rendition", &syntax_keyword1_rendition)
    VarRep("syntax-keyword2-rendition", &syntax_keyword2_rendition)
    VarRep("syntax-keyword3-rendition", &syntax_keyword3_rendition)
    DefVarRep("default-tab-size", &default_tab_size)
    VarWithDef0("tab-size", TabSize)
    VarRep("terminal-DEC-CRT-level-1", &term_deccrt)
    VarRep("terminal-DEC-CRT-level-2", &term_deccrt2)
    VarRep("terminal-DEC-CRT-level-3", &term_deccrt3)
    VarRep("terminal-DEC-CRT-level-4", &term_deccrt4)
    VarRep("terminal-advanced-video-option", &term_avo)
    VarRep("terminal-ansi-crt", &term_ansi)
    VarRep("terminal-application-keypad", &term_app_keypad)
    VarRep("terminal-cursor-type", &term_cursor_mode)
    VarRep("terminal-does-not-need-null-padding", &term_nopadding)
    VarRep("terminal-edit", &term_edit)
    VarRep("terminal-eightbit", &term_eightbit)
    VarRep("terminal-is-terminal", &term_is_terminal)
    VarRep("terminal-length", &term_length)
    VarRep("terminal-output-buffer-size", &term_output_buffer_size)
    VarRep("terminal-regis", &term_regis)
    VarRep("terminal-width", &term_width)
    VarRep("this-command", &this_command)
    Var("trace-hook", BoundName, &trace_proc)
    VarRep("trace-into", &trace_into)
    VarRep("trace-mode", &trace_mode)
    VarRep("track-eol-on-^N-^P", &track_eol)
    VarRep("true", &is_true)
    VarRep("user-colour-1", &user_1_rendition)
    VarRep("user-colour-2", &user_2_rendition)
    VarRep("user-colour-3", &user_3_rendition)
    VarRep("user-colour-4", &user_4_rendition)
    VarRep("user-colour-5", &user_5_rendition)
    VarRep("user-colour-6", &user_6_rendition)
    VarRep("user-colour-7", &user_7_rendition)
    VarRep("user-colour-8", &user_8_rendition)
    Var("user-interface-hook", BoundName, &user_interface_hook_proc)
    VarRep("unlink-checkpoint-files", &unlink_checkpoint_files)
    VarRep("visible-bell", &visible_bell)
    Var0("window-first-column", WindowColumn)
    VarRep("window-graphic-rendition", &window_rendition)
    Var0("window-size", WindowSize)
    Var0("window-width", WindowWidth)
    DefVarRep("default-wrap-long-lines", &default_wrap_lines)
    VarWithDef0("wrap-long-lines", WrapLines)
    VarRep("xon-mode", &protocol_mode)
}

void init_var(void)
{
    restore_var();
}
