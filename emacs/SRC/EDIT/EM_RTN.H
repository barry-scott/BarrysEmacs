/*
 *	Copyright 1986 Barry A. Scott and Nick W. Emery
 */
extern int emacs( int argc, unsigned char **argv,
	unsigned char *rest_fn, unsigned char *device, unsigned char *term_type );
extern void init_memory(void);
extern void init_queue_system(void);
extern int init_char_terminal( struct trmcontrol *tt, unsigned char *device );
extern int init_gui_terminal( struct trmcontrol *tt, unsigned char *device );
extern int init_file_terminal( struct trmcontrol *tt, unsigned char *device );
extern void fatal_error( int );
extern void invoke_debug(void);
extern int current_line_number(void);
extern unsigned char *br_getstr( int breaksp, unsigned char *prefix, unsigned char *fmt, va_list *argp );
extern unsigned char *sprintfl( unsigned char *buff, int len, unsigned char *fmt,...);
extern struct marker * new_mark(void);
extern struct marker * copy_mark( struct marker *dst, struct marker *src );
extern void set_mark( struct marker *m, struct emacs_buffer *b, int p, int right );
extern int set_mark_command( void );
extern int un_set_mark_command( void );
int exchange_dot_and_mark( void );
extern int to_mark(struct marker *m);
extern void dest_mark(struct marker *m);
extern void ins_cstr( unsigned char *s, int n );
extern void record_insert( int dot, int n, unsigned char *s );
extern void record_delete( int dot, int n );
extern int done_is_done( void );
extern int bin_setup(void);
extern int numeric_arg(int n);
extern int string_arg(int n);
extern struct windowring * window_arg(int n);
extern int eval_arg(int n);
extern void exec_str( unsigned char * );
extern int define_function(void);
extern int execute_mlisp_buffer( void );
extern int execute_mlisp_line( void );
extern int execute_mlisp_string( unsigned char * );
extern int execute_mlisp_file( unsigned char *fn, int missingOK );
extern int fetch_var (unsigned char *name, int *iresp, unsigned char **cresp);
extern void set_var (unsigned char *name, int iresp, unsigned char *cresp);
extern struct prognode *parse_mlisp_line( unsigned char *s );
extern int expand_and_default( unsigned char *, unsigned char *, unsigned char * );
extern void check_free(void *x);
extern void void_result( void );
extern struct variablename *lookup_variable( unsigned char *name );
extern struct bound_name **lookup_keys( struct keymap *kmap, unsigned char *keys, int len );
extern struct bound_name *get_keymap_binding( struct keymap *kmap, int c );
extern struct keymap *define_keymap( unsigned char *name );
extern void add_keymap_binding( struct keymap *kmap, int c, struct bound_name *proc );
extern void scan_map( struct keymap *kmap, void (*proc)( struct bound_name *b, unsigned char *keys, int len, int range ), int fold_case );
extern unsigned char *key_to_str( unsigned char *keys, int len );
extern int getword( unsigned char * *table_ref, unsigned char *s, ... );
extern unsigned char * call_getescfile( unsigned char *fmt, ... );
extern unsigned char *getescfile( unsigned char *fmt, va_list *argp, int dir, int vers );
extern unsigned char *getescword( unsigned char * *table_ref, unsigned char *s, ... );
extern int getnum( unsigned char *, ... );
extern int forward_word( void );
extern int backward_word( void );
extern int concat_command( void );
extern void re_init_keyboard( void );
void work_process_queue(void);
void work_add_item( struct emacs_work_item *work );
extern int illegal_operation( void );
extern int calc_col( void );
extern void set_bfp(struct emacs_buffer *p);
extern void set_bfn(unsigned char *name);
extern void scratch_bfn(unsigned char *name, int disp);
extern void erase_bf( struct emacs_buffer *b );
extern struct emacs_buffer * find_bf(unsigned char *name);
extern void kill_bfn(unsigned char *name);
extern void def_mac(unsigned char *s,void *body,int is_mlisp,int len,struct bound_name *prealloced_bound_name);
extern unsigned char *macro_address_to_name(struct keymap *addr);
extern int undo_boundary( void );
extern void error (unsigned char *fmt, ...);
extern unsigned char *fetch_os_error( int );
extern void message( unsigned char *fmt, ... );
extern int sit_for_inner( int );
extern void setprotocol( void );
extern unsigned char *get_key (struct keymap *kmap, unsigned char *fmt, ...);
extern int check_is_proc( unsigned char *, struct variablename *, struct bound_name ** );
extern int exec_prog( struct prognode * );
extern void perform_set( struct variablename *v, int arg, unsigned char *svalue, int setting_default, int is_array, int ivalue );
extern void do_dsp( int );
extern int checkpoint_everything( void );
extern int checkpoint_buffers( void );
extern int execute_bound( struct bound_name * );
extern int execute_bound_saved_environment( struct bound_name * );
extern struct expression *create_expression(int type, int val, unsigned char *string, int release);
extern struct emacs_array *create_array( int dims, ... );
extern void do_release( struct expression *e );
extern void lisp_free( struct prognode *p );
extern void free_sexpr_defun( struct bound_name * );
extern void print_expr( struct prognode *p, int depth );
extern void copy_expression( struct expression *from_e, struct expression *to_e, int is_system );
extern int process_channel_interrupts( void );
extern void set_activity_character( unsigned char );
extern void process_timer_interrupts( void );
extern int save_excursion_inner(int (*rtn)(void));
extern int recursive_edit( void );
extern void push_window_ring( void );
extern int widen_region( void );
extern void pop_window_ring( void );
extern int change_window_size( int );
extern void fit_windows_to_screen_length( void );
extern int chg_w_height( struct window *w, int delta, int dir );
extern int chg_w_width( struct window *w, int delta, int dir );
extern int scan_bf( unsigned char, int, int );
extern void set_win( struct window * );
extern void tie_win( struct window *w, struct emacs_buffer *b );
extern struct window *split_win( struct window * );
extern struct window *split_win_vert( struct window * );
extern int delete_other_windows( void );
extern void window_move( struct window *w, int down, int lots, int dottop );
extern void setpos( int, int );
extern void clearline( int );
extern void copyline( int );
extern void update_screen( int );
extern void ding( void );
extern int window_on( struct emacs_buffer * );
extern void del_win( struct window * );
extern void dest_window_ring( struct windowring * );
extern int find_mac( unsigned char *s );
extern int beginning_of_file(void);
extern void keyboard_interrupt( struct trmcontrol *, int );
extern void emacs_exit( int );
extern unsigned char * getstr(unsigned char *fmt,...);
extern unsigned char * getnbstr(unsigned char *fmt,...);
extern int del_chars_in_buffer( int position, int number_of_characters, int fwd );
extern void del_frwd(int n, int k);
extern void del_to_buf( int n, int where, int doit, unsigned char *name );
extern void insert_buffer( unsigned char *name );
extern int end_of_line( void );
extern void gap_to(int n);
extern int gap_room(int n);
extern int str_to_int( unsigned char * );
extern unsigned char *savestr( unsigned char *s );
extern int fio_access( unsigned char * );
extern int fio_delete( unsigned char * );
extern FILE *fio_create( unsigned char *, int, int, unsigned char *, int );
extern FILE *fio_open( unsigned char *, int, unsigned char *, int * );
extern int fio_get( FILE *, unsigned char *, int );
extern int fio_get_line( FILE *file, unsigned char *buf, int len );
extern int fio_get_with_prompt( FILE *rab, unsigned char *buffer, int size, unsigned char *prompt );
extern int fio_put( FILE *, unsigned char *, int );
extern int fio_split_put( FILE *, unsigned char *, int, unsigned char *, int, unsigned char * );
extern int fio_close( FILE * );
extern long int fio_size( FILE * );
extern time_t fio_modify_date( FILE *file );
extern time_t fio_file_modify_date( unsigned char *file );
extern int fio_access_mode( FILE *file );
extern int fio_file_access_mode( unsigned char *file );
extern unsigned char *fio_getname( FILE * );
extern long int fio_getpos( FILE * );
extern void fio_setpos( FILE *, long int );
extern int chdir_and_set_global_record( unsigned char * );
extern FILE *fopenp( unsigned char *path, unsigned char *fn, unsigned char *fnb, int mode, unsigned char *ex );
extern void proc_de_ref_buf( struct emacs_buffer *b );
extern void wind_de_ref_buf( struct emacs_buffer *b );
extern void undo_de_ref_buf( struct emacs_buffer *b );
extern void use_var_de_ref_buf( struct emacs_buffer *b );
extern int cur_indent( void );
extern void to_col( int );
extern int get_char( void );
extern void del_back(int n, int k);
extern void insert_at(int n, unsigned char c);
extern int abbrev_expand( void );
extern struct emacs_buffer * new_bf(unsigned char *name);
extern int progn_command( void );
extern int self_insert( unsigned char );
extern int self_insert_command( void );
extern void rst_dsp( void );
extern void init_dsp( void );
extern void start_async_io( void );
extern void stop_async_io( void );
extern int visit_file( unsigned char *fn, int createnew, int windowfiddle, unsigned char *dn );
extern int write_file( unsigned char *fn, int appendit );
extern int read_file( unsigned char *fn, int erase, int createnew );
extern int mod_write( void );
extern int process_keys( void );
extern int executepackage( unsigned char * );
extern int next_err( void );
extern int read_in_files( int, unsigned char ** );
extern void send_exit_message( unsigned char * );
extern int search( unsigned char *s, int n, int dot, int RE );
int region_around_match( int n );
extern void read_restart_message( int set_gargs );
extern void delete_errlog_list( void );
extern int parse_erb(int pos, int limit);
extern int mod_exist( void );
extern int count_processes( void );
extern void kill_processes( void );
extern void kill_checkpoint_files( void );
extern char *cuserid( char *str );
extern void journal_stop( struct emacs_buffer *b );
extern void journal_insert( int, int, unsigned char * );
extern void journal_delete( int, int );
extern void journal_flush( void );
extern void journal_pause( void );
extern void syntax_insert_update( int, int );
extern void syntax_delete_update( int, int );
extern void syntax_fill_in_array( int required );

#ifdef SAVE_ENVIRONMENT
extern void *emacs_malloc( int size, enum malloc_block_type );
extern void *emacs_realloc( void *p, int new_size, enum malloc_block_type );
#else
extern void *emacs_malloc( int size );
extern void *emacs_realloc( void *p, int new_size );
#endif
extern void emacs_free( void *p );
extern void emacs_check_malloc_block( void *p );

extern unsigned char *emacs_strlwr( unsigned char * );
#ifdef vms
extern unsigned char *emacs_strupr( unsigned char * );
#endif
extern int emacs_stricmp( const unsigned char *str1, const unsigned char *str2 );
extern int emacs_strnicmp( const unsigned char *str1, const unsigned char *str2, int maxchar );
extern unsigned char emacs_toupper( unsigned char ch );
extern unsigned char emacs_tolower( unsigned char ch );
extern void set_protocol( int );
extern void wait_abit(void);
extern unsigned char *lookup_key_name( unsigned char * );
extern int error_handler(void);
extern int convert_key_string( int inlen, unsigned char *inptr,	int *outlen, unsigned char *outptr );
extern unsigned char *save_abs( unsigned char * );
extern void not_implemented(void);
extern void fetch_last_phrase( struct expression *e );
/*
 *	check routines
 */
extern int check_buffer_alloc_size( int value, struct variablename *v );
extern int check_syntax_colouring( int value, struct variablename *v );
extern int check_syntax_array( int value, struct variablename *v );
extern int check_cursor_mode( int value, struct variablename *v );
extern int check_buffer_type( unsigned char *value, struct variablename *v );
extern int check_buffer_rms_attribute( unsigned char *value, struct variablename *v );
extern int check_default_rms_attribute( unsigned char *value, struct variablename *v );
extern int check_override_rms_attribute( unsigned char *value, struct variablename *v );
extern int check_file_name( unsigned char *value, struct variablename *v );
extern int check_backup_file_mode( unsigned char *value, struct variablename *v );
extern int check_macro_name( unsigned char *value, struct variablename *v );
extern int check_buffer_check( int value, struct variablename *v );
extern int check_buffer_name( unsigned char *value, struct variablename *v );
extern int check_enter_emacs(unsigned char *value, struct variablename *v);
extern int check_exit_emacs(unsigned char *value, struct variablename *v);
extern int check_leave_emacs(unsigned char *value, struct variablename *v);
extern int check_return_to_emacs(unsigned char *value, struct variablename *v);
extern int check_buffer_names( struct emacs_array *a, struct variablename *v);
extern int check_current_syntax_table( unsigned char *value, struct variablename *v );
extern int check_get_db_help_flags( int value, struct variablename *v );
extern int check_bool( int value, struct variablename *v );
extern int check_null( int value, struct variablename *v );
extern int check_read_only( int value, struct variablename *v );
extern int check_pos( int value, struct variablename *v );
extern void check_free(void *x);
extern int check_yes_no( unsigned char *s, ... );
extern int check_checkpoint( unsigned char *value, struct variablename *v );
extern int check_journal_frequency( int value, struct variablename *v );
extern int check_current_buffer_journal( int value, struct variablename *v );
extern int check_trace_hook(unsigned char *val, struct variablename *v);
extern int check_error_hook(unsigned char *val, struct variablename *v);
extern int check_break_hook(unsigned char *val, struct variablename *v);
extern int check_trace_mode(unsigned char *val, struct variablename *v);
extern int check_error_message_parser( unsigned char *value, struct variablename *v );
extern int check_protocol_mode( int value, struct variablename *v );
extern int check_screen_width( int width, struct variablename *v );
extern int check_screen_length( int length, struct variablename *v );
extern int check_display_bool( int value, struct variablename *v );
extern int check_mouse_mode( int value, struct variablename *v );
extern int check_graphic_rendition( unsigned char *value, struct variablename *v );
extern int check_args(int min, int max);
extern int check_tab_size( int value, struct variablename *v );
extern int check_default_right_margin( int value, struct variablename *v );
extern int check_default_left_margin( int value, struct variablename *v );
extern int check_right_margin( int value, struct variablename *v );
extern int check_left_margin( int value, struct variablename *v );
extern int check_current_abbrev_table( unsigned char *value, struct variablename *v );
extern int check_current_windows( struct windowring *wr, struct variablename *v );
extern int check_auto_fill_hook(unsigned char *val, struct variablename *v);
extern int check_default_auto_fill_hook (unsigned char *val, struct variablename *v);
extern int check_process_key_hook (unsigned char *val, struct variablename *v);
extern int check_cs_cvt_8bit(unsigned char *value, struct variablename *v);
extern int check_cs_cvt_csi(unsigned char *value, struct variablename *v);
extern int check_cs_par_char(unsigned char *value, struct variablename *v);
extern int check_cs_par_sep(unsigned char *value, struct variablename *v);
extern int check_cs_int_char(unsigned char *value, struct variablename *v);
extern int check_cs_fin_char(unsigned char *value, struct variablename *v);
extern int check_current_abbrev_table(unsigned char *value, struct variablename *v);
extern int check_window_size( int value, struct variablename *v );
extern int check_window_width( int value, struct variablename *v );
extern int check_current_syntax_table( unsigned char *value, struct variablename *v );
extern int check_window_column( int value, struct variablename *v );
extern int check_term_output_size( int value, struct variablename *v );

/*
 *	fetch routines
 */
extern void fetch_buffer_type( struct expression *e );
extern void fetch_buffer_rms_attribute( struct expression *e );
extern void fetch_default_rms_attribute( struct expression *e );
extern void fetch_rms_attribute_override( struct expression *e );
extern void fetch_file_name(struct expression *e);
extern void fetch_backup_file_mode(struct expression *e);
extern void fetch_macro_name(struct expression *e);
extern void fetch_buffer_check( struct expression *e );
extern void fetch_buffer_name( struct expression *e );
extern void fetch_current_keymap( struct expression *e );
extern void fetch_auto_fill_hook( struct expression *e );
extern void fetch_default_auto_fill_hook( struct expression *e );
extern void fetch_process_key_hook( struct expression *e );
extern void fetch_buffer_names( struct emacs_array *a, int set_up, int index );
extern int fetch_array_command( void );
extern void fetch_current_buffer_journal( struct expression *e );
extern void fetch_protocol_mode( struct expression *e );
extern void fetch_screen_width( struct expression *e );
extern void fetch_screen_length( struct expression *e );
extern int fetch_database_entry( void );
extern int fetch_help_database_entry( void );
extern void fetch_last_phrase( struct expression *e );
extern void fetch_current_windows( struct expression *e );
extern void fetch_current_abbrev_table( struct expression *e );
extern void fetch_last_search_string( struct expression *e );
extern void fetch_window_size( struct expression *e );
extern void fetch_window_width( struct expression *e );
extern void fetch_current_syntax_table( struct expression *e );
extern void fetch_window_column( struct expression *e );
void fetch_buffer_alloc_size( struct expression *e );
extern int erase_region(void);
extern int use_variables_of_buffer_command( void );
extern int write_current_file(void);
extern int no_value_command(void);
extern int write_named_file(unsigned char *);
extern int synchronise_files(void);
extern void search_replace_once( unsigned char *replace_string, int re );
extern int lookup_key_value(int len, unsigned char *chars, unsigned char **value);
extern int wait_for_activity(void);
extern int interlock_dec( volatile int * );
extern int interlock_inc( volatile int * );
extern void conditional_wake(void);
extern void _dbg_msg( unsigned char *str, ... );
extern void DebugPrintf( char *str, ... );
extern void define_variable( unsigned char *name, struct variablename *desc );
extern int get_yes_or_no( int yes, unsigned char *prompt, ... );

#ifndef get_config_env
extern unsigned char *get_config_env( unsigned char * );
#endif
extern int put_config_env( unsigned char *, unsigned char * );

#if defined( DB )
extern struct database *open_db( unsigned char *, int );
extern struct dbsearch *find_sl( unsigned char * );
extern void fetch_database_index( struct dbsearch *dbs, unsigned char *match, int (*helper)( int , unsigned char *, unsigned char * * ));
extern void close_db (struct database *);
extern unsigned char *getescdb( struct dbsearch *db, unsigned char *fmt, ... );
extern int put_db( unsigned char *, int, unsigned char *, int, struct database *);
extern int get_db( unsigned char *key, int keylen, unsigned char **cont, int *contlen, unsigned char *(*ins)(int,unsigned char *), struct database *db );
int get_db_help( unsigned char *key_spec, unsigned char * (*ins)(int, unsigned char *), struct database *db );
extern void free_db(struct database *db);
extern int index_db( unsigned char *match_string, int (*helper)( int , unsigned char *, unsigned char * * ), struct database *db );
#endif


#if defined(_WINDOWS)
extern int win_yes_or_no( unsigned char *buf, int yes );
extern void win_error( unsigned char *msg, ... );
#endif
extern void UI_update_window_title(void);
#if defined(_WINDOWS) || defined(_MSDOS)
char *get_device_name_translation( char *name );
#endif
#if defined(_MSDOS)
extern char *get_profile_string( char *, char * );
#endif
extern void emacs_heap_check(void);
extern void get_user_full_name( unsigned char *users_name, unsigned char *users_full_name );
extern void get_system_name( unsigned char *converted_system_name );

#ifdef vms
extern unsigned char *get_log( unsigned char *, unsigned char * );
extern void vms_set_term_is_terminal(unsigned char *);
extern int lib$signal( int, ... );
#endif

#ifdef XWINDOWS
extern int xwin__init( struct trmcontrol *, char * );
#endif
#ifdef unix
extern int ansi__init( struct trmcontrol *, unsigned char * );
extern void start_unix_timer( int );
extern void stop_unix_timer( int );
#ifdef SUBPROCESSES
extern void child_sig(int x);
extern int child_changed;
extern void change_msgs (void);
#endif

#endif
extern void fetch_elapse_time( struct expression *e );
