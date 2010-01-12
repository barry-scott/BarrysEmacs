//
//    Copyright 1995-2009 Barry A. Scott
//
extern bool emacs_internal_init_done_event(void);
extern void gui_set_dot( int n );
extern void gui_input_shift_state( bool shift );
extern void gui_input_mode_set_dot( int n );
extern void gui_input_mode_before_insert();
extern bool gui_input_mode_before_delete();
extern int emacsMain( const EmacsString &rest_fn, const EmacsString &device, const EmacsString &term_type );
extern int parse_dbg_flags( const EmacsString &flags );
extern void init_memory(void);
extern void init_queue_system(void);
extern int init_char_terminal( const EmacsString &device );
extern int init_gui_terminal( const EmacsString &device );
extern int init_file_terminal( const EmacsString &device );
extern int current_line_number(void);
extern EmacsString br_get_string_interactive( int breaksp, const EmacsString &prefix, const EmacsString &fmt );
extern EmacsString br_get_string_mlisp();
extern int set_mark_command( void );
extern int un_set_mark_command( void );
int exchange_dot_and_mark( void );
extern void record_insert( int dot, int n, const EmacsChar_t *s );
extern void record_delete( int dot, int n );
extern int done_is_done( void );
extern int bin_setup(void);
extern int numeric_arg(int n);
extern int string_arg(int n);
extern EmacsWindowRing * window_arg(int n);
extern int eval_arg(int n);
extern void exec_str( const EmacsString & );
extern int define_function(void);
extern int execute_mlisp_buffer( void );
extern int execute_mlisp_line( void );
extern int execute_mlisp_string( const EmacsString & );
extern int execute_mlisp_file( const EmacsString &fn, int missingOK );
extern void dump_mlisp_stack(void);
extern int fetch_var( const EmacsString &name, int *, EmacsString * );
extern void set_var( const EmacsString &name, const Expression &e );
extern int fetch_var( VariableName *var, int *, EmacsString *, EmacsString &errmsg );
extern void set_var(  VariableName *var, const Expression &e );
extern int expand_and_default( const EmacsString &, const EmacsString &, EmacsString & );
extern void check_free(void *x);
extern void void_result( void );
extern BoundName **lookup_keys( KeyMap *kmap, EmacsString keys );
extern KeyMap *define_keymap( const EmacsString &name );
extern void scan_map( KeyMap *kmap, void (*proc)( BoundName *b, EmacsString &keys, int range ), int fold_case );
extern EmacsString key_to_str( const EmacsString &keys, bool replace_key_names=true );
extern int forward_word( void );
extern int backward_word( void );
extern int concat_command( void );
extern void re_init_keyboard( void );
void work_process_queue(void);
extern int illegal_operation( void );
extern int calc_col( void );
extern void def_mac(EmacsString s,void *body,int is_mlisp,int len,BoundName *prealloced_bound_name);
extern int undo_boundary( void );
extern void error( const EmacsString &fmt); //...
//extern void error( const char * fmt, ...);
extern EmacsString fetch_os_error( int );
extern void message( const EmacsString &fmt ); //...
extern int sit_for_inner( int );
extern void setprotocol( void );
extern int check_args( int, int );
extern int exec_prog( ProgramNode * );
extern void perform_set( VariableName *v, Expression &new_e, int setting_default );
extern int checkpoint_everything( void );
extern int checkpoint_buffers( void );
extern int execute_bound_saved_environment( BoundName * );
extern Expression *create_Expression(int type, int val, unsigned char *string, int release);
extern void do_release( Expression *e );
//extern void lisp_free( ProgramNode *p );
extern void free_sexpr_defun( BoundName * );
extern void print_expr( ProgramNode *p, int depth );
extern int process_channel_interrupts( void );
extern void set_activity_character( EmacsChar_t );
extern void process_timer_interrupts( void );
extern int save_excursion_inner(int (*rtn)(void));
extern int recursive_edit( void );
extern void push_window_ring( void );
extern int widen_region( void );
extern void pop_window_ring( void );
extern int scan_bf_for_lf( int, int );
extern int delete_other_windows( void );
extern void window_move( EmacsWindow *w, int down, int lots, int dottop );
extern void ding( void );
extern void dest_window_ring( EmacsWindowRing * );
extern int beginning_of_file(void);
extern void emacs_exit( int );
extern int del_chars_in_buffer( int position, int number_of_characters, int fwd );


extern void replace_to_buf( int n, const EmacsString &name );
extern void append_to_buf( int n, const EmacsString &name );

extern void insert_buffer( const EmacsString &name );
extern int end_of_line( void );
extern int str_to_int( const EmacsString & );
extern int chdir_and_set_global_record( const EmacsString & );
extern void undo_de_ref_buf( EmacsBuffer *b );
extern int cur_indent( void );
extern void to_col( int );
extern int get_char( void );
extern int abbrev_expand( void );
extern int progn_command( void );
extern int self_insert( EmacsChar_t );
extern int self_insert_command( void );
extern void rst_dsp( void );
extern void init_dsp( void );
extern void start_async_io( void );
extern void stop_async_io( void );
extern int visit_file( const EmacsString &fn, int createnew, int windowfiddle, const EmacsString &dn );
extern bool mod_write( void );
extern int write_this( const EmacsString &fname );
extern int process_keys( void );
extern int executepackage( const EmacsString & );
extern bool error_messages_available();
extern int read_in_files( int, unsigned char ** );
extern bool send_exit_message( const EmacsString & );
int region_around_match( int n );
extern void read_restart_message( int set_gargs );
extern void delete_errlog_list( void );
extern int parse_erb(int pos, int limit);
extern bool mod_exist( void );
extern int count_processes( void );
extern void kill_processes( void );
extern void kill_checkpoint_files( void );
extern void syntax_insert_update( int, int );
extern void syntax_delete_update( int, int );

extern void set_protocol( int );
extern void wait_abit(void);
extern int error_handler(void);
extern int convert_key_string( const EmacsString &input, EmacsString &output );
extern EmacsString save_abs( const EmacsString &  );
extern void not_implemented(void);

extern void check_free(void *x);
extern int check_yes_no( EmacsString &s );    //...
extern int check_args(int min, int max);

extern int fetch_array_command( void );
extern int fetch_database_entry( void );
extern int fetch_help_database_entry( void );

extern int erase_region(void);
extern int use_variables_of_buffer_command( void );
extern EmacsBuffer *current_buffer_for_mlisp();
extern int write_current_file(void);
extern int no_value_command(void);
extern int write_named_file(unsigned char *);
extern int synchronise_files(void);
extern int wait_for_activity(void);
extern int interlock_dec( volatile int * );
extern int interlock_inc( volatile int * );
extern void wake_main_thread(void);
extern void conditional_wake(void);
extern void DebugPrintf( const EmacsString &str );    //...
extern int get_yes_or_no( int yes, const EmacsString &prompt );    //...

#ifndef get_config_env
extern EmacsString get_config_env( const EmacsString & );
#endif
extern int put_config_env( const EmacsString &, const EmacsString & );

#if defined(_WINDOWS)
extern int win_yes_or_no( int yes, const EmacsString &prompt );
extern void win_error( EmacsString msg );    //...
#endif
#if defined(_WINDOWS) || defined(_MSDOS)
EmacsString get_device_name_translation( const EmacsString &name );
#endif
#if defined(_MSDOS)
extern char *get_profile_string( char *, char * );
#endif
extern void emacs_heap_check(void);
extern EmacsString get_user_full_name();
extern EmacsString users_login_name();
extern EmacsString get_system_name();

#ifdef vms
extern unsigned char *get_log( unsigned char *, unsigned char * );
extern void vms_set_term_is_terminal(unsigned char *);
extern int lib$signal( int, ... );
#endif

#ifdef XWINDOWS
extern int xwin__init( TerminalControl *, char * );
extern int motif_yes_or_no( int yes, const EmacsString &prompt );
extern void motif_message_box( const EmacsString &message );
#endif
#ifdef __unix__
extern int ansi__init( TerminalControl *, unsigned char * );
extern void start_unix_timer( int );
extern void stop_unix_timer( int );
# ifdef SUBPROCESSES
extern int child_changed;
extern void change_msgs (void);
# endif
#endif

unsigned char *savestr( const unsigned char *s );

//int do_print( const unsigned char *, va_list *, unsigned char *, int );
//int do_print( const EmacsString , va_list *, EmacsString & );
