/*	Copyright (c) 1986                                                  */
/*		Barry A. Scott and Nick Emery                               */

#define SAVED_BUFFER_MAX 100		/* maximum number of buffers to save in subprocess */
#define MAX_SYNTAX_TABLES 40		/* the maximum number of syntax tables */
#define MAX_ABBREV_TABLES 40		/* the maximum number of abbrev tables */
#define BUFFERSIZE 300			/* Maximum buffer size */
#define MLLINE_SIZE 300
#define MAXFILES 512			/* Maximum size of vecfiles */
#ifndef BUFSIZ
#define BUFSIZ 512
#endif

#ifdef SAVE_ENVIRONMENT
enum malloc_block_type
	{
	malloc_type_none = 0,
	malloc_type_char,
	malloc_type_star_star,
	malloc_type_struct_abbrevent,
	malloc_type_struct_abbrevtable,
	malloc_type_struct_automode,
	malloc_type_struct_binding,
	malloc_type_struct_binding_list,
	malloc_type_struct_bound_name,
	malloc_type_struct_database,
	malloc_type_struct_dbsearch,
	malloc_type_struct_emacs_array,
	malloc_type_struct_emacs_buffer,
	malloc_type_struct_emacs_line,
	malloc_type_struct_errblk,
	malloc_type_struct_expression,
	malloc_type_struct_extern_func,
	malloc_type_struct_find_data,
	malloc_type_struct_journal_block,
	malloc_type_struct_keymap,
	malloc_type_struct_keymap_long,
	malloc_type_struct_keymap_short,
	malloc_type_struct_marker,
	malloc_type_struct_process,
	malloc_type_struct_process_blk,
	malloc_type_struct_prognode,
	malloc_type_struct_syntax_string,
	malloc_type_struct_syntax_table,
	malloc_type_struct_time_request,
	malloc_type_struct_timer_entry,
	malloc_type_struct_trmcontrol,
	malloc_type_struct_variablename,
	malloc_type_struct_window,
	malloc_type_struct_windowring,
	malloc_type_struct_rendition_region,
	/* insert new types before this one */
	malloc_type_last
	};
#endif
