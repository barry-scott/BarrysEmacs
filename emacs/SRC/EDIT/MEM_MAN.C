/*	Copyright (c) 1984-1995
 *		Barry A. Scott & Nick Emery
 */

/*
 * FACILITY:
 *	VAX EMACS
 *
 * ABSTRACT:
 *	This module contains the code to manage memory for the EMACS
 *	editor. This includes the implementation of malloc, realloc
 *	and free, and the routines to save and restore all malloced
 *	memory and selected data areas in a file. This means that the
 *	true state of EMACS can be saved for later restoration.
 *
 * ENVIRONMENT:
 *	User mode, not AST re-entrant
 *
 * AUTHOR: NW EMERY	CREATION DATE:  1-Jun-1984
 *
 * MODIFIED BY:
 *
 * Edit	Modifier	Date		Reason
 * 000	NW EMERY	 1-Jun-1984	Original
 * 001	NW EMERY	20-AUG-1984	Added support for memory saving
 * 003	NW EMERY	13-Nov-1984	Changed ident match
 *   4	Barry A. Scott	 4-Feb-1985	change ch$move to move_c_3
 * 005	Nick W. Emery	 1-May-1985	Fixed bug in realloc. Gets $$alox wrong
 *   6	Barry A. Scott	 3-Jul-1985	Add account and checking code for
 *					debugging emacs
 *   7	Nick W Emery	15-Aug-1988	Added malloc look aside lists
 */

#include <emacs.h>

#undef malloc
#undef free
#undef calloc
#undef realloc

long int bytes_allocated;
int is_restored;

unsigned char default_environment_file[MAXPATHLEN+1];

#define GUARD_SIZE 16
#if DBG_ALLOC_CHECK
unsigned char guard_pattern[GUARD_SIZE] = { 'G','G','G','G','G','G','G','G','G','G','G','G','G','G','G','G' };
#endif
struct heap_entry
	{
	struct queue q;
	long int user_size;	/* use long int to cause machine independant alignment */
#ifdef SAVE_ENVIRONMENT
	enum malloc_block_type user_type;
	int ptr_index;
#endif
	void (* users_pc)(void);
#if DBG_ALLOC_CHECK
	unsigned char front_guard[GUARD_SIZE];
#endif
	unsigned char user_data[GUARD_SIZE];
	};
struct queue all_emacs_memory;

#if DBG_ALLOC_CHECK
void emacs_heap_check_entry( struct heap_entry *h );
#endif

void init_memory( void )
	{
	if( all_emacs_memory.next == NULL )
		{
		init_queue_system();
		queue_init( &all_emacs_memory );
		}
	return;
	}

#if DBG_ALLOC_CHECK
void emacs_check_malloc_block( void *p )
	{
	struct heap_entry *h;

	h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

	emacs_heap_check_entry( h );
	}

void emacs_heap_check_entry( struct heap_entry *h )
	{
	if( memcmp( guard_pattern, h->front_guard, GUARD_SIZE ) != 0 )
		_dbg_msg( u_str("Corrupt front guard band in %X at PC %X"), h, h->users_pc );
	if( memcmp( guard_pattern, &h->user_data[h->user_size], GUARD_SIZE ) != 0 )
		_dbg_msg( u_str("Corrupt back guard band in %X at PC %X"), h, h->users_pc );
	}

void emacs_heap_check(void)
	{
	struct queue *entry;

	queue_validate( &all_emacs_memory );

	entry = all_emacs_memory.next;
	while( (void *)entry != (void *)&all_emacs_memory )
		{
		emacs_heap_check_entry( (struct heap_entry *)entry );
		entry = entry->next;
		}
	return;
	}
#endif

void *emacs_malloc
	(
	int size
#ifdef	SAVE_ENVIRONMENT
	, enum malloc_block_type type
#endif
	)
	{
	struct heap_entry *h;

#if DBG_ALLOC_CHECK
	int old_dbg_flags = dbg_flags;

	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	h = (struct heap_entry *)malloc( sizeof( struct heap_entry ) + size );
	if( h == NULL )
		return NULL;

	h->q.next = NULL;
	h->q.prev = NULL;
	h->user_size = size;
#ifdef	SAVE_ENVIRONMENT
	if( type <= malloc_type_none || type >= malloc_type_last )
		_dbg_msg( u_str("Bad malloc block type of %d in emacs_malloc"), type );
	h->user_type = type;
#endif

	bytes_allocated += size;

#if defined(_NT)
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( _M_I286 )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( vms ) && defined( vax )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( __mips )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( __alpha )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined(__WATCOMC__) || defined( __i386__ )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( macintosh )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( __hp9000s300 )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( __hp9000s700 )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#else
#	error "How do we get the PC on this machine"
#endif

#if DBG_ALLOC_CHECK
	memcpy( h->front_guard, guard_pattern, GUARD_SIZE );
	memcpy( &h->user_data[size], guard_pattern, GUARD_SIZE );

	if( ! (dbg_flags&DBG_ALLOC_CHECK) )
		dbg_flags &= ~DBG_QUEUE;
#endif
	queue_insert( all_emacs_memory.prev, &h->q );

#if DBG_ALLOC_CHECK
	dbg_flags = old_dbg_flags;
#endif

	/* zero out all blocks except char types */
	if( type != malloc_type_char ) 
		memset( &h->user_data[0], 0, size );

	return (void *)&h->user_data[0];
	}

void emacs_free( void *p )
	{
	struct heap_entry *h;
#if DBG_ALLOC_CHECK
	int old_dbg_flags = dbg_flags;
#endif

	h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

#if DBG_ALLOC_CHECK
	emacs_heap_check_entry( h );
	if( ! (dbg_flags&DBG_ALLOC_CHECK) )
		dbg_flags &= ~DBG_QUEUE;
#endif
	queue_remove( &h->q );

#if DBG_ALLOC_CHECK
	dbg_flags = old_dbg_flags;
#endif

	bytes_allocated -= h->user_size;

#if DBG_ALLOC_CHECK
	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	free( (void *)h );
	}

void *emacs_realloc
	(
	void *p,
	int size
#ifdef	SAVE_ENVIRONMENT
	, enum malloc_block_type type
#endif
	)
	{
	struct heap_entry *h;
#if DBG_ALLOC_CHECK
	int old_dbg_flags = dbg_flags;
#endif
	long int old_size;

	if( p == NULL )
		return emacs_malloc
			(
			size
#ifdef SAVE_ENVIRONMENT
			, type
#endif
			);

	h = (struct heap_entry *)((char *)p-offsetof(struct heap_entry,user_data));

#if DBG_ALLOC_CHECK
	emacs_heap_check_entry( h );
	if( ! (dbg_flags&DBG_ALLOC_CHECK) )
		dbg_flags &= ~DBG_QUEUE;
#endif
	queue_remove( &h->q );
#if DBG_ALLOC_CHECK
	dbg_flags = old_dbg_flags;
#endif

#if DBG_ALLOC_CHECK
	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	old_size = h->user_size;
	bytes_allocated -= old_size;

	h = (struct heap_entry *)realloc( h, sizeof( struct heap_entry ) + size );
	if( h == NULL )
		return NULL;

	if( h->user_type != type )
		_dbg_msg( u_str("realloc attempted to change block type from %d to %d"),
			h->user_type, type );
	h->user_size = size;
	bytes_allocated += size;
#if defined(_NT)
	h->users_pc = (void (*)(void))*(int *)(((char *)&p) - sizeof( long int ));
#elif defined( _M_I286 )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( vms ) && defined( vax )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( __mips )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined( __alpha )
	h->users_pc = (void (*)(void))*(long int *)(((char *)&size) - sizeof( long int ));
#elif defined(__WATCOMC__) || defined( __i386__ )
	h->users_pc = (void (*)(void))*(int *)(((char *)&p) - sizeof( long int ));
#elif defined( macintosh )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( __hp9000s300 )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#elif defined( __hp9000s700 )
	h->users_pc = (void (*)(void))*(int *)(((char *)&size) - sizeof( long int ));
#else
#	error "How do we get the PC on this machine"
#endif

#if DBG_ALLOC_CHECK
	memcpy( h->front_guard, guard_pattern, GUARD_SIZE );
	memcpy( &h->user_data[size], guard_pattern, GUARD_SIZE );

	if( ! (dbg_flags&DBG_ALLOC_CHECK) )
		dbg_flags &= ~DBG_QUEUE;
#endif
	queue_insert( all_emacs_memory.prev, &h->q );

#if DBG_ALLOC_CHECK
	dbg_flags = old_dbg_flags;

	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	/* zero out all blocks except char types */
	if( type != malloc_type_char && size > old_size )
		memset( &h->user_data[old_size], 0, size-(int)old_size );

	return (void *)&h->user_data[0];
	}

#if 0
void *emacs_calloc
	(
	int num,
	int size
#ifdef	SAVE_ENVIRONMENT
	, enum malloc_block_type type
#endif
	)
	{
	void *p;

#if DBG_ALLOC_CHECK
	if( dbg_flags&DBG_ALLOC_CHECK )
		emacs_heap_check();
#endif

	p = emacs_malloc
		(
		num*size
#ifdef SAVE_ENVIRONMENT
		, type
#endif
		);
	if( p != NULL )
		memset( p, 0, num*size );

	return p;
	}
#endif

int dump_memory_statistics( void )
	{
#define MAX_SLOTS 128
#define SLOT_SIZE 4

	unsigned char line[300];
	struct queue *entry;
	int mem_size[MAX_SLOTS+1];
	int index;

	/* init the array of results */
	memset( &mem_size[0], 0, sizeof( mem_size ) );

	/* scan the allocated memory and collate the sizes */
	entry = all_emacs_memory.next;
	while( (void *)entry != (void *)&all_emacs_memory )
		{
		struct heap_entry *heap = (struct heap_entry *)entry;

		index = (int)(heap->user_size/SLOT_SIZE);
		if(  index > MAX_SLOTS )
			index = MAX_SLOTS;
		mem_size[index]++;

		entry = entry->next;
		}

	scratch_bfn( u_str( "Memory usage statistics" ), interactive );
	ins_str( u_str("Size	Count\n----	-----\n"));

	for( index=0; index<MAX_SLOTS-1; index++ )
		{
		if( mem_size[index] != 0 )
			{
			sprintfl( line, sizeof(line),
				u_str( "%d	%d\n" ),
				(index+1)*SLOT_SIZE,
				mem_size[index] );
			ins_cstr( line, _str_len( line ) );
			}
		}

	sprintfl( line, sizeof(line), u_str(">%d	%d\n"),
		MAX_SLOTS*SLOT_SIZE, mem_size[MAX_SLOTS] );
	ins_cstr( line, _str_len( line ) );

	return 0;

#undef MAX_SLOTS
#undef SLOT_SIZE
	}

#ifdef SAVE_ENVIRONMENT
#if !defined( __cplusplus )
# include <setjmp.h>
#endif
# include <errlog.h>
# if defined(SUBPROCESSES)
#  ifdef vms
#   include <vms_comm.h>
#  endif
#  ifdef unix
#   ifdef XWINDOWS
#   include <X11/Intrinsic.h>
#   endif
#   include <unixcomm.h>
#  endif
#  if defined(_NT)
#   pragma warning( disable: 4201 )
#   include <windows.h>
#   include <nt_comm.h>
#   pragma warning( default: 4201 )
#  endif
# endif
extern struct queue timer_queue;
extern void mem_man_code_begin(void);
extern char *mem_man_cdata_begin;
extern int mem_man_idata_begin;
extern int mem_man_udata_begin;
extern void mem_man_code_end(void);
extern char *mem_man_cdata_end;
extern int mem_man_idata_end;
extern int mem_man_udata_end;

#ifdef _ptr
#undef _ptr
#endif
#define _ptr( field ) _serialize_ptr( (void **)&ptr->field )
#define _ptr_i( p ) _serialize_ptr_indirect( (void **)&p )
#define _serialize_var( var ) _serialize_bytes( (byte *)&var, sizeof( var ) )

#if defined( __cplusplus )

class EmacsException
	{
	public:
		EmacsException( const char *message ) { error_message = message; }
		const char *error_message;
	};
class MemManReadError : public EmacsException
	{
	public:
		MemManReadError() : EmacsException( "Read error" ) { };
	};
class MemManWriteError : public EmacsException
	{
	public:
		MemManWriteError() : EmacsException( "Write error" ) { };
	};
class MemManOutOfMemory : public EmacsException
	{
	public:
		MemManOutOfMemory() : EmacsException( "Out of memory" ) { };
	};

#else
static jmp_buf serialize_error_jmp;
#endif
typedef unsigned char byte;
static FILE *serialize_file = NULL;
static int saving_environment;

static void _serialize_bytes( byte *bytes, int length );
static void _serialize_ptr( void **ptr );
static void _serialize_ptr_indirect( void **ptr );
static void serialize_savres_globals(void);
static void serialize_star_star( void **ptr, int size );
static void serialize_struct_abbrevent( struct abbrevent *ptr, int size );
static void serialize_struct_abbrevtable( struct abbrevtable *ptr, int size );
static void serialize_struct_automode( struct automode *ptr, int size );
static void serialize_struct_binding( struct binding *ptr, int size );
static void serialize_struct_binding_list( struct binding_list *ptr, int size );
static void serialize_struct_bound_name( struct bound_name *ptr, int size );
static void serialize_struct_database( struct database *ptr, int size );
static void serialize_struct_dbsearch( struct dbsearch *ptr, int size );
static void serialize_struct_emacs_array( struct emacs_array *ptr, int size );
static void serialize_struct_emacs_buffer( struct emacs_buffer *ptr, int size );
static void serialize_struct_emacs_line( struct emacs_line *ptr, int size );
static void serialize_struct_errblk( struct errblk *ptr, int size );
static void serialize_struct_expression( struct expression *ptr, int size );
static void serialize_struct_extern_func( struct extern_func *ptr, int size );
static void serialize_struct_find_data( void *ptr, int size );
static void serialize_struct_journal_block( struct journal_block *ptr, int size );
static void serialize_struct_keymap( struct keymap *ptr, int size );
static void serialize_struct_keymap_long( struct keymap_long *ptr, int size );
static void serialize_struct_keymap_short( struct keymap_short *ptr, int size );
static void serialize_struct_marker( struct marker *ptr, int size );
#if defined(SUBPROCESSES)
static void serialize_struct_process_blk( struct process_blk *ptr, int size );
#endif
static void serialize_struct_prognode( struct prognode *ptr, int size );
static void serialize_struct_syntax_string( struct syntax_string *ptr, int size );
static void serialize_struct_syntax_table( struct syntax_table *ptr, int size );
static void serialize_struct_time_request( struct time_request *ptr, int size );
static void serialize_struct_timer_entry( struct timer_entry *ptr, int size );
static void serialize_struct_trmcontrol( struct trmcontrol *ptr, int size );
static void serialize_struct_variablename( struct variablename *ptr, int size );
static void serialize_struct_window( struct window *ptr, int size );
static void serialize_struct_windowring( struct windowring *ptr, int size );
static void serialize_struct_rendition_region( struct rendition_region *ptr, int size );

struct ptr_info
	{
	void **ptr_vector;
	int max_user_size;
	int max_index;
	struct queue *first_heap_entry;
	struct heap_entry *low_addr_ptr;
	struct heap_entry *high_addr_ptr;

	void *low_code_ptr, *high_code_ptr;
	ptrdiff_t relocation_code_offset;
	void *low_cdata_ptr, *high_cdata_ptr;
	ptrdiff_t relocation_cdata_offset;
	void *low_idata_ptr, *high_idata_ptr;
	ptrdiff_t relocation_idata_offset;
	void *low_udata_ptr, *high_udata_ptr;
	ptrdiff_t relocation_udata_offset;
	};
struct ptr_info ptr_info;
static unsigned char the_end_of_save_marker[] = "[*<*(THe end of the saved environment)*>*]";

int save_environment( void )
	{
	int index = 1;
	unsigned char *fn;
	struct queue *entry;
	unsigned char *buffer;
	unsigned char save_file_name[MAXPATHLEN];

	fn = call_getescfile( u_str(": save-environment (%s) "), default_environment_file );
	if( fn == NULL )
		return 0;

	expand_and_default( fn, default_environment_file, save_file_name );
#ifdef vms
	/* lose the version number */
	fn = _str_rchr( save_file_name, ';' );
	if( fn != NULL )
		*fn = '\0';
#endif
	serialize_file = fopen( s_str(save_file_name), "wb" );
	if( serialize_file == NULL )
		{
		error( u_str("error creating file") );
		return 0;
		}

#if !defined( __cplusplus )
	if( setjmp( serialize_error_jmp ) )
		{
		error( u_str("error saving data") );
		return 0;
		}
#else
	try	{
#endif
	saving_environment = 1;

	/* update the emacs_buffer from the globals like bf_p1 */
	set_bfp( bf_cur );
	/* free up all error log memory */
	delete_errlog_list();

	/*
	 *	write out the emacs version information to allow
	 *	for version checking on restore
	 */
	_serialize_bytes( operating_system_name, sizeof( operating_system_name ) );
	_serialize_bytes( version_string, sizeof( version_string ) );

	ptr_info.low_code_ptr = (void *)&mem_man_code_begin;
	ptr_info.high_code_ptr = (void *)&mem_man_code_end;
	ptr_info.low_idata_ptr = (void *)&mem_man_idata_begin;
	ptr_info.high_idata_ptr = (void *)&mem_man_idata_end;
	ptr_info.low_udata_ptr = (void *)&mem_man_udata_begin;
	ptr_info.high_udata_ptr = (void *)&mem_man_udata_end;
	ptr_info.low_cdata_ptr = (void *)mem_man_cdata_begin;
	ptr_info.high_cdata_ptr = (void *)mem_man_cdata_end;

	/*
	 *	serialize the heap
	 */
	ptr_info.max_user_size = 0;
	ptr_info.low_addr_ptr = (struct heap_entry *)all_emacs_memory.next;
	ptr_info.high_addr_ptr = ptr_info.low_addr_ptr;
	if( ptr_info.ptr_vector != NULL )
		{
		emacs_free( ptr_info.ptr_vector );
		ptr_info.ptr_vector = NULL;
		}

	/* scan the allocated memory and set the ptr_index field */
	entry = all_emacs_memory.next;
	while( (void *)entry != (void *)&all_emacs_memory )
		{
		struct heap_entry *heap = (struct heap_entry *)entry;

		heap->ptr_index = index++;

		if( (int)heap->user_size > ptr_info.max_user_size )
			ptr_info.max_user_size = (int)heap->user_size;

		if( heap < ptr_info.low_addr_ptr )
			ptr_info.low_addr_ptr = heap;
		if( heap > ptr_info.high_addr_ptr )
			ptr_info.high_addr_ptr = heap;

		entry = entry->next;
		}

	/* allocate the ptr_vector */
	ptr_info.ptr_vector = (void **)emacs_malloc( sizeof( struct heap_entry *) * (index+1), malloc_type_char );
	if( ptr_info.ptr_vector == NULL )
#if defined( __cplusplus )
		throw MemManOutOfMemory();
#else
		longjmp( serialize_error_jmp, 1 );
#endif

	/* fill in the ptr_vector values */
	index = 1;
	entry = all_emacs_memory.next;
	while( (void *)entry != (void *)&all_emacs_memory )
		{
		struct heap_entry *heap = (struct heap_entry *)entry;

		ptr_info.ptr_vector[index] = heap;
		index++;

		entry = entry->next;
		}

	ptr_info.max_index = index;

	/* all but char types need a scratch buffer */
	buffer = u_str( emacs_malloc( ptr_info.max_user_size, malloc_type_char ) );
	if( buffer == NULL )
#if defined( __cplusplus )
		throw MemManOutOfMemory();
#else
		{
		error(u_str("Out of memory in save-environment"));
		longjmp( serialize_error_jmp, 1 );
		}
#endif

	/* need to know the ptr_info value to perform the restore */
	_serialize_var( ptr_info );

	/*
	 * scan the allocated memory and output to the save file
	 * but do not output the two malloced heap blocks used
	 * by save environment.
	 */
	entry = all_emacs_memory.next;
	for( index = 1; index<ptr_info.max_index; index++ )
		{
		struct heap_entry *heap = (struct heap_entry *)entry;
		int size = (int)heap->user_size;

		_serialize_bytes( (byte *)&size, sizeof( size ) );
		_serialize_bytes( (byte *)&heap->user_type, sizeof( heap->user_type ) );

		if( heap->user_type == malloc_type_char )
			{
			_serialize_bytes( (byte *)&heap->user_data, size );
			}
		else
		{
		/* make a copy for the routines to fix up pointers in */
		memcpy( buffer, heap->user_data, size );
		switch( heap->user_type )
		{
		case malloc_type_star_star:
			serialize_star_star( (void **)buffer, size );
			break;
		case malloc_type_struct_abbrevent:
			serialize_struct_abbrevent( (struct abbrevent *)buffer, size );
			break;
		case malloc_type_struct_abbrevtable:
			serialize_struct_abbrevtable( (struct abbrevtable *)buffer, size );
			break;
		case malloc_type_struct_automode:
			serialize_struct_automode( (struct automode *)buffer, size );
			break;
		case malloc_type_struct_binding:
			serialize_struct_binding( (struct binding *)buffer, size );
			break;
		case malloc_type_struct_binding_list:
			serialize_struct_binding_list( (struct binding_list *)buffer, size );
			break;
		case malloc_type_struct_bound_name:
			serialize_struct_bound_name( (struct bound_name *)buffer, size );
			break;
		case malloc_type_struct_database:
			serialize_struct_database( (struct database *)buffer, size );
			break;
		case malloc_type_struct_dbsearch:
			serialize_struct_dbsearch( (struct dbsearch *)buffer, size );
			break;
		case malloc_type_struct_emacs_array:
			serialize_struct_emacs_array( (struct emacs_array *)buffer, size );
			break;
		case malloc_type_struct_emacs_buffer:
			serialize_struct_emacs_buffer( (struct emacs_buffer *)buffer, size );
			break;
		case malloc_type_struct_emacs_line:
			serialize_struct_emacs_line( (struct emacs_line *)buffer, size );
			break;
		case malloc_type_struct_errblk:
			serialize_struct_errblk( (struct errblk *)buffer, size );
			break;
		case malloc_type_struct_expression:
			serialize_struct_expression( (struct expression *)buffer, size );
			break;
		case malloc_type_struct_extern_func:
			serialize_struct_extern_func( (struct extern_func *)buffer, size );
			break;
		case malloc_type_struct_find_data:
			serialize_struct_find_data( (void *)buffer, size );
			break;
		case malloc_type_struct_journal_block:
			serialize_struct_journal_block( (struct journal_block *)buffer, size );
			break;
		case malloc_type_struct_keymap:
			serialize_struct_keymap( (struct keymap *)buffer, size );
			break;
		case malloc_type_struct_keymap_long:
			serialize_struct_keymap_long( (struct keymap_long *)buffer, size );
			break;
		case malloc_type_struct_keymap_short:
			serialize_struct_keymap_short( (struct keymap_short *)buffer, size );
			break;
		case malloc_type_struct_marker:
			serialize_struct_marker( (struct marker *)buffer, size );
			break;
#if defined(SUBPROCESSES)
		case malloc_type_struct_process_blk:
			serialize_struct_process_blk( (struct process_blk *)buffer, size );
			break;
#endif
		case malloc_type_struct_prognode:
			serialize_struct_prognode( (struct prognode *)buffer, size );
			break;
		case malloc_type_struct_syntax_string:
			serialize_struct_syntax_string( (struct syntax_string *)buffer, size );
			break;
		case malloc_type_struct_syntax_table:
			serialize_struct_syntax_table( (struct syntax_table *)buffer, size );
			break;
		case malloc_type_struct_time_request:
			serialize_struct_time_request( (struct time_request *)buffer, size );
			break;
		case malloc_type_struct_timer_entry:
			serialize_struct_timer_entry( (struct timer_entry *)buffer, size );
			break;
		case malloc_type_struct_trmcontrol:
			serialize_struct_trmcontrol( (struct trmcontrol *)buffer, size );
			break;
		case malloc_type_struct_variablename:
			serialize_struct_variablename( (struct variablename *)buffer, size );
			break;
		case malloc_type_struct_window:
			serialize_struct_window( (struct window *)buffer, size );
			break;
		case malloc_type_struct_windowring:
			serialize_struct_windowring( (struct windowring *)buffer, size );
			break;
		case malloc_type_struct_rendition_region:
			serialize_struct_rendition_region( (struct rendition_region *)buffer, size );
			break;

		default:
			_dbg_msg( u_str("Do not know how to save block type %d"), heap->user_type );
		}
		_serialize_bytes( buffer, size );
		}

		entry = entry->next;
		}

	/*
	 *	serialize the SAVRES globals
	 */	
	serialize_savres_globals();

	/* put down a special end of save marker
	 * which restore will check for to confirm
	 * that the restore process read all the bytes
	 * that where written
	 */
	_serialize_var( the_end_of_save_marker );
	fclose( serialize_file );

	message( u_str("Environment saved in %s"), save_file_name );

#if defined( __cplusplus )
		}
	catch( EmacsException e )
		{
		error( u_str(e.error_message) );
		return 0;
		}
#endif

	return no_value_command();
	}

int can_restore_environment( unsigned char *rest_fn, unsigned char *full_name )
	{
	if( rest_fn == NULL )
		return 0;

	expand_and_default( rest_fn, default_environment_file, full_name );
	if( fio_access( full_name ) == 0 )
		return 0;

	serialize_file = fopen( s_str(full_name), "rb" );
	if( serialize_file == NULL )
		return 0;

	saving_environment = 0;

	/*
	 *	read in the emacs version information to allow
	 *	for version checking
	 */
	{
	unsigned char file_operating_system_name[sizeof( operating_system_name ) ];
	unsigned char file_version_string[sizeof(version_string)];

	_serialize_bytes( file_operating_system_name, sizeof( operating_system_name ) );
	_serialize_bytes( file_version_string, sizeof( version_string ) );

	if( memcmp( file_version_string, version_string, sizeof(version_string) ) != 0 )
		{
		_dbg_msg( u_str("The restore file does not match this version of Emacs") );
		return -1;
		}
	if( memcmp( file_operating_system_name, operating_system_name, sizeof(operating_system_name) ) != 0 )
		{
		_dbg_msg( u_str("The restore file does not match Emacs for this operating system") );
		return -1;
		}
	}

	return 1;
	}

int restore_environment(void)
	{
	int index = 1;
	struct queue *entry;

	if( serialize_file == NULL )
		return 0;

#if !defined( __cplusplus )
	if( setjmp( serialize_error_jmp ) )
		return 0;
#else
	try	{
#endif

	/* Get the ptr_info data from the orginal memory image */
	_serialize_var( ptr_info );

	ptr_info.relocation_code_offset = 0;
	ptr_info.relocation_cdata_offset = 0;
	ptr_info.relocation_idata_offset = 0;
	ptr_info.relocation_udata_offset = 0;

	if( ptr_info.low_cdata_ptr != (void *)mem_man_cdata_begin )
		{
/*		_dbg_msg( u_str("Emacs cdata is in a new location of %X v. %X"),
			mem_man_cdata_begin, ptr_info.low_cdata_ptr ); */
		ptr_info.relocation_cdata_offset = mem_man_cdata_begin - ((char *)ptr_info.low_cdata_ptr);
		}
	if( ptr_info.low_idata_ptr != (void *)&mem_man_idata_begin )
		{
/*		_dbg_msg( u_str("Emacs idata is in a new location of %X v. %X"),
			&mem_man_idata_begin, ptr_info.low_idata_ptr ); */
		ptr_info.relocation_idata_offset = ((char *)&mem_man_idata_begin) - ((char *)ptr_info.low_idata_ptr);
		}
	if( ptr_info.low_udata_ptr != (void *)&mem_man_udata_begin )
		{
/*		_dbg_msg( u_str("Emacs udata is in a new location of %X v. %X"),
			&mem_man_udata_begin, ptr_info.low_udata_ptr ); */
		ptr_info.relocation_udata_offset = ((char *)&mem_man_udata_begin) - ((char *)ptr_info.low_udata_ptr);
		}
	if( ptr_info.low_code_ptr != (void *)&mem_man_code_begin      )
		{
/*		_dbg_msg( u_str("Emacs code is in a new location of %X v. %X"),
			 &emacs, ptr_info.low_code_ptr ); */
		ptr_info.relocation_code_offset = ((char *)&mem_man_code_begin) - ((char *)ptr_info.low_code_ptr);
		}

	/*
	 *	serialize the heap
	 */

	/* allocate the ptr_vector */
	ptr_info.ptr_vector = (void **)emacs_malloc( sizeof( struct heap_entry *) * (ptr_info.max_index+1), malloc_type_char );
	if( ptr_info.ptr_vector == NULL )
#if defined( __cplusplus )
		throw MemManOutOfMemory();
#else
		longjmp( serialize_error_jmp, 1 );
#endif

	ptr_info.max_user_size = 0; /* not used on restore */
	ptr_info.low_addr_ptr = (struct heap_entry *)all_emacs_memory.next;
	ptr_info.high_addr_ptr = ptr_info.low_addr_ptr;
	/* start scanning from the first heap block we restore */
	ptr_info.first_heap_entry = all_emacs_memory.prev;

	/* read in all the heap blocks */
	for( index=1; index<ptr_info.max_index; index++ )
		{
		int size;
		enum malloc_block_type type;
		void *ptr;
		struct heap_entry *h;

		/* find out the size and type of this block */
		_serialize_var( size );
		_serialize_var( type );

		/* alloc the block */
		ptr = emacs_malloc( size, type );
		if( ptr == NULL )
#if defined( __cplusplus )
		throw MemManOutOfMemory();
#else
			longjmp( serialize_error_jmp, 1 );
#endif

		/* get its contents */
		_serialize_bytes( (byte *)ptr, size );

		/* set the ptr_index field */
		h = (struct heap_entry *)((char *)ptr-offsetof(struct heap_entry,user_data));
		h->ptr_index = index;

		/* build up the ptr_vector */
		ptr_info.ptr_vector[index] = ptr;
		}

	/* scan the restored memory and fix up the points */
	entry = ptr_info.first_heap_entry->next;
	while( (void *)entry != (void *)&all_emacs_memory )
		{
		struct heap_entry *heap = (struct heap_entry *)entry;
		int size = (int)heap->user_size;

		/* make a copy for the routines to fix up pointers in */
		switch( heap->user_type )
		{
		case malloc_type_char:
			/* nothing to do */
			break;
		case malloc_type_star_star:
			serialize_star_star( (void **)heap->user_data, size );
			break;
		case malloc_type_struct_abbrevent:
			serialize_struct_abbrevent( (struct abbrevent *)heap->user_data, size );
			break;
		case malloc_type_struct_abbrevtable:
			serialize_struct_abbrevtable( (struct abbrevtable *)heap->user_data, size );
			break;
		case malloc_type_struct_automode:
			serialize_struct_automode( (struct automode *)heap->user_data, size );
			break;
		case malloc_type_struct_binding:
			serialize_struct_binding( (struct binding *)heap->user_data, size );
			break;
		case malloc_type_struct_binding_list:
			serialize_struct_binding_list( (struct binding_list *)heap->user_data, size );
			break;
		case malloc_type_struct_bound_name:
			serialize_struct_bound_name( (struct bound_name *)heap->user_data, size );
			break;
		case malloc_type_struct_database:
			serialize_struct_database( (struct database *)heap->user_data, size );
			break;
		case malloc_type_struct_dbsearch:
			serialize_struct_dbsearch( (struct dbsearch *)heap->user_data, size );
			break;
		case malloc_type_struct_emacs_array:
			serialize_struct_emacs_array( (struct emacs_array *)heap->user_data, size );
			break;
		case malloc_type_struct_emacs_buffer:
			serialize_struct_emacs_buffer( (struct emacs_buffer *)heap->user_data, size );
			break;
		case malloc_type_struct_emacs_line:
			serialize_struct_emacs_line( (struct emacs_line *)heap->user_data, size );
			break;
		case malloc_type_struct_errblk:
			serialize_struct_errblk( (struct errblk *)heap->user_data, size );
			break;
		case malloc_type_struct_expression:
			serialize_struct_expression( (struct expression *)heap->user_data, size );
			break;
		case malloc_type_struct_extern_func:
			serialize_struct_extern_func( (struct extern_func *)heap->user_data, size );
			break;
		case malloc_type_struct_find_data:
			serialize_struct_find_data( (void *)heap->user_data, size );
			break;
		case malloc_type_struct_journal_block:
			serialize_struct_journal_block( (struct journal_block *)heap->user_data, size );
			break;
		case malloc_type_struct_keymap:
			serialize_struct_keymap( (struct keymap *)heap->user_data, size );
			break;
		case malloc_type_struct_keymap_long:
			serialize_struct_keymap_long( (struct keymap_long *)heap->user_data, size );
			break;
		case malloc_type_struct_keymap_short:
			serialize_struct_keymap_short( (struct keymap_short *)heap->user_data, size );
			break;
		case malloc_type_struct_marker:
			serialize_struct_marker( (struct marker *)heap->user_data, size );
			break;
#if defined(SUBPROCESSES)
		case malloc_type_struct_process_blk:
			serialize_struct_process_blk( (struct process_blk *)heap->user_data, size );
			break;
#endif
		case malloc_type_struct_prognode:
			serialize_struct_prognode( (struct prognode *)heap->user_data, size );
			break;
		case malloc_type_struct_syntax_string:
			serialize_struct_syntax_string( (struct syntax_string *)heap->user_data, size );
			break;
		case malloc_type_struct_syntax_table:
			serialize_struct_syntax_table( (struct syntax_table *)heap->user_data, size );
			break;
		case malloc_type_struct_time_request:
			serialize_struct_time_request( (struct time_request *)heap->user_data, size );
			break;
		case malloc_type_struct_timer_entry:
			serialize_struct_timer_entry( (struct timer_entry *)heap->user_data, size );
			break;
		case malloc_type_struct_trmcontrol:
			serialize_struct_trmcontrol( (struct trmcontrol *)heap->user_data, size );
			break;
		case malloc_type_struct_variablename:
			serialize_struct_variablename( (struct variablename *)heap->user_data, size );
			break;
		case malloc_type_struct_window:
			serialize_struct_window( (struct window *)heap->user_data, size );
			break;
		case malloc_type_struct_windowring:
			serialize_struct_windowring( (struct windowring *)heap->user_data, size );
			break;
		case malloc_type_struct_rendition_region:
			serialize_struct_rendition_region( (struct rendition_region *)heap->user_data, size );
			break;
		default:
			_dbg_msg( u_str("Do not know how to restore block type %d"), heap->user_type );
		}

		entry = entry->next;
		}

	/*
	 *	serialize the SAVRES globals
	 */	
	serialize_savres_globals();

	{
	unsigned char tmp[sizeof( the_end_of_save_marker )];
	_serialize_var( tmp );
	if( memcmp( (void *)tmp, (void *)the_end_of_save_marker, sizeof( tmp ) ) != 0 )
		{
		_dbg_msg( u_str("end of save marker mismatch") );
		return 0;
		}
	}

	fclose( serialize_file );

	/* update the globals like bf_p1 from the emacs_buffer */
	{
	struct emacs_buffer *b = bf_cur;
	bf_cur = NULL;
	set_bfp( b );
	}

	/* say that we have restored an environment */
	is_restored++;

	return 1;
#if defined( __cplusplus )
		}
	catch( EmacsException e )
		{
		error( u_str(e.error_message) );
		return 0;
		}
#endif

	}


extern struct marker *oneL_start;

static void serialize_savres_globals(void)
	{
	int i;

	/* save all the MLisp command names */
	_serialize_var( n_macs );
	for( i=0; i<n_macs; i++ )
		{
		_ptr_i( mac_names[i] );
		_ptr_i( mac_bodies[i] );
		}

	_serialize_var( default_syntax_array );
	_serialize_var( default_syntax_colouring );

	_serialize_var( number_of_syntax_tables );
	for( i=0; i<number_of_syntax_tables; i++ )
		{
		_ptr_i( syntax_table_names[i] );
		_ptr_i( syntax_tables[i] );
		}

	_ptr_i( timer_list_head.next );
	_ptr_i( timer_list_head.prev );

	_ptr_i( timer_queue.next );
	_ptr_i( timer_queue.prev );

	if( saving_environment )
		{
		struct syntax_table tmp_syn;
		struct abbrevtable tmp_abb;
		expression tmp_exp;
		struct bound_name tmp_b;

		tmp_syn = global_syntax_table;
		serialize_struct_syntax_table( &tmp_syn, sizeof( tmp_syn ) );
		_serialize_bytes( (byte *)&tmp_syn, sizeof( tmp_syn ) );

		tmp_abb = global_abbrev;
		serialize_struct_abbrevtable( &tmp_abb, sizeof( tmp_abb ) );
		_serialize_bytes( (byte *)&tmp_abb, sizeof( tmp_abb ) );

		tmp_exp = global_value;
		serialize_struct_expression( &tmp_exp, sizeof( tmp_exp ) );
		_serialize_bytes( (byte *)&tmp_exp, sizeof( tmp_exp ) );

		tmp_exp = last_expression;
		serialize_struct_expression( &tmp_exp, sizeof( tmp_exp ) );
		_serialize_bytes( (byte *)&tmp_exp, sizeof( tmp_exp ) );

		tmp_b = progn_block;
		serialize_struct_bound_name( &tmp_b, sizeof( tmp_b ) );
		_serialize_bytes( (byte *)&tmp_b, sizeof( tmp_b ) );

		tmp_b = lambda_block;
		serialize_struct_bound_name( &tmp_b, sizeof( tmp_b ) );
		_serialize_bytes( (byte *)&tmp_b, sizeof( tmp_b ) );

		tmp_b = interrupt_block;
		serialize_struct_bound_name( &tmp_b, sizeof( tmp_b ) );
		_serialize_bytes( (byte *)&tmp_b, sizeof( tmp_b ) );
		}
	else
		{
		_serialize_bytes( (byte *)&global_syntax_table, sizeof( global_syntax_table ) );
		serialize_struct_syntax_table( &global_syntax_table, sizeof( global_syntax_table ) );

		_serialize_bytes( (byte *)&global_abbrev, sizeof( global_abbrev ) );
		serialize_struct_abbrevtable( &global_abbrev, sizeof( global_abbrev ) );

		_serialize_bytes( (byte *)&global_value, sizeof( global_value ) );
		serialize_struct_expression( &global_value, sizeof( global_value ) );

		_serialize_bytes( (byte *)&last_expression, sizeof( last_expression ) );
		serialize_struct_expression( &last_expression, sizeof( last_expression ) );

		_serialize_bytes( (byte *)&progn_block, sizeof( progn_block ) );
		serialize_struct_bound_name( &progn_block, sizeof( progn_block ) );

		_serialize_bytes( (byte *)&lambda_block, sizeof( lambda_block ) );
		serialize_struct_bound_name( &lambda_block, sizeof( lambda_block ) );

		_serialize_bytes( (byte *)&interrupt_block, sizeof( interrupt_block ) );
		serialize_struct_bound_name( &interrupt_block, sizeof( interrupt_block ) );
		}
	_serialize_var( is_restored );
	_ptr_i( saved_windows.next );
	_ptr_i( saved_windows.prev );
	_ptr_i( pushed_windows );
	_ptr_i( windows );
	_ptr_i( wn_cur );
	_ptr_i( oneL_start );
	_serialize_var( G_saveminibuf_body );
	_serialize_var( in_minibuf_body );
	_serialize_var( default_wrap_lines );
	_serialize_var( scroll_step );
	_serialize_var( quick_rd );
	_serialize_var( ctl_arrow );
	_serialize_var( global_mode_string );
	_serialize_var( pop_up_windows );
	_serialize_var( split_height_threshhold );
	for( i=0; i<MSCREENLENGTH+1; i++ )
		{
		_ptr_i( phys_screen[i] );
		_ptr_i( desired_screen[i] );
		}
	_serialize_var( protocol_mode );
	_ptr_i( free_lines );
	_ptr_i( NL_scratch );
	_serialize_var( visible_bell );
	_serialize_var( black_on_white );
	_serialize_var( unlink_checkpoint_files );
	_serialize_var( checkpoint_hook );
	_serialize_var( ask_about_buffer_names );
	_ptr_i( auto_list );
	_ptr_i( checkpoint_proc );

	_serialize_var( arg );
	_serialize_var( arg_state );
	_ptr_i( bf_cur );
	_serialize_var( buf_name_free );
	_ptr_i( buf_names );
	_serialize_var( col_valid );
	_serialize_var( dot );
	_serialize_var( dot_col );
	_serialize_var( key_mem );
	_serialize_var( last_key_struck );
	_serialize_var( last_keys_struck );
	_ptr_i( last_proc );
	_serialize_var( mem_used );
	_ptr_i( minibuf_body );
	_serialize_var( n_buffers );
	_serialize_var( n_macs );
	_serialize_var( previous_command );
	_serialize_var( remembering );
	_ptr_i( reset_minibuf );
	_serialize_var( this_command );
	_serialize_var( quitting_emacs );
	_serialize_var( error_message_text );
	_serialize_var( stack_trace_on_error );
	_serialize_var( remove_help_window );
	_serialize_var( auto_help );
	_ptr_i( ml_value );
	_serialize_var( err );
	_serialize_var( image_context_lost );
	_serialize_var( image_count  );
	_ptr_i( images );
	_ptr_i( image_contexts );
	_ptr_i( image_filenames );
	_serialize_var( trace_into );
	_serialize_var( trace_mode );
	_serialize_var( error_hook );
	_serialize_var( trace_hook );
	_serialize_var( break_hook );
	_ptr_i( exit_emacs_dcl_command );
	_serialize_var( track_eol );
	_serialize_var( argument_prefix_cnt );
#ifndef _NT
	_serialize_var( mode_line_rendition );
	_serialize_var( region_rendition );
	_serialize_var( window_rendition );
	_serialize_var( syntax_keyword1_rendition );
	_serialize_var( syntax_keyword2_rendition );
	_serialize_var( syntax_word_rendition );
	_serialize_var( syntax_string_rendition );
	_serialize_var( syntax_quote_rendition );
	_serialize_var( syntax_comment_rendition );
	_serialize_var( user_1_rendition );
	_serialize_var( user_2_rendition );
	_serialize_var( user_3_rendition );
	_serialize_var( user_4_rendition );
	_serialize_var( user_5_rendition );
	_serialize_var( user_6_rendition );
	_serialize_var( user_7_rendition );
	_serialize_var( user_8_rendition );
#endif
	_serialize_var( force_redisplay );
	_serialize_var( maximum_dcl_buffer_size );
	_serialize_var( dcl_buffer_reduction );
#ifdef SUBPROCESSES
#ifdef vms
	_ptr_i( current_process );
	_ptr_i( process_ptrs );
	_ptr_i( process_names );
	_serialize_var( n_processes );
	_serialize_var( n_process_slots );
#endif
#endif
	_serialize_var( ignore_version_numbers );
	for( i=0; i<MAXFILES+3; i++ )
		{
		_ptr_i( vecfiles[i] );
		_ptr_i( vec_db[i] );
		}
	_serialize_var( replace_case );
	_serialize_var( default_replace );
	_ptr_i( last_search_string );
	_serialize_var( get_db_help_flags );
	_ptr_i( dbroot );
	_serialize_var( db_count );
	_ptr_i( db_search_lists );
	_serialize_var( db_spaceleft );
	_serialize_var( silently_kill_processes );
	_serialize_var( journalling_frequency );
	_serialize_var( journal_scratch_buffers );
	_serialize_var( animate_journal_recovery );
	_serialize_var( activity_indicator );
	_ptr_i( mem_ptr );
	_serialize_var( checkpoint_frequency );
	_serialize_var( end_of_mac );
	_ptr_i( current_global_map );
	_ptr_i( global_map );
	_ptr_i( minibuf_local_ns_map );
	_ptr_i( minibuf_local_map );
	_serialize_var( cs_enabled );
	_serialize_var( cs_modified );
	_serialize_var( cs_cvt_f_keys );
	_serialize_var( cs_cvt_mouse );
	_serialize_var( cs_cvt_8bit_string );
	_serialize_var( cs_cvt_csi_string );
	_serialize_var( cs_par_char_string );
	_serialize_var( cs_par_sep_string );
	_serialize_var( cs_int_char_string );
	_serialize_var( cs_fin_char_string );
	_serialize_var( cs_attr );
	_ptr_i( trace_proc );
	_ptr_i( break_proc );
	_ptr_i( error_proc );
	_serialize_var( last_auto_keymap );	/* Index of last auto keymap */
	_serialize_var( decompile_max );	/* Size of DecompileBuffer */
	_ptr_i( decompile_buffer );	/* The Trace back buffer */
	_serialize_var( decompile_used );
	_ptr_i( current_function_length );
	_ptr_i( current_break_length );	/* Current function name ptrs */
	_ptr_i( current_function );
	_ptr_i( current_break );
	_ptr_i( call_back_result );
	_ptr_i( call_back_str );
	_serialize_var( call_back_len );
	_serialize_var( number_of_abbrev_tables );
	for( i=0; i<number_of_abbrev_tables; i++ )
		{
		_ptr_i( abbrev_table_names[i] );
		_ptr_i( abbrev_tables[i] );
		}
	_ptr_i( last_phrase );
	_serialize_var( bf_auto_fill_hook );
	_serialize_var( bf_process_key_hook );
	_serialize_var( bf_journalling );
	_ptr_i( buffers );	/* root of the list of extant buffers */
	_ptr_i( minibuf );	/* The minibuf */
	_serialize_var( default_mode_format );
	_serialize_var( default_replace_mode );
	_serialize_var( default_fold_case );
	_serialize_var( default_right_margin );
	_serialize_var( default_left_margin );
	_serialize_var( default_comment_column );
	_serialize_var( default_tab_size );
	_serialize_var( default_highlight );
	_serialize_var( default_display_nonprinting );
	_serialize_var( default_display_eof );
	_serialize_var( default_display_c1 );
	_serialize_var( default_read_only_buffer );
	_ptr_i( default_auto_fill_proc );
	_ptr_i( var_desc );
	_ptr_i( var_names );
	_serialize_var( n_vars );
	_serialize_var( var_t_size );
	_ptr_i( mac_names_ref );
	_serialize_var( error_message_parser_string );
	_ptr_i( error_message_parser );
	_serialize_var( rms_attribute_override );
	_serialize_var( default_rms_attribute );
	_serialize_var( compile_command );
	_serialize_var( debug_command );
	_serialize_var( filter_command );
	_serialize_var( execute_command );
	_serialize_var( cli_name );
#ifdef SUBPROCESSES
	_serialize_var( saved_buffer_count );
	for( i=0; i<saved_buffer_count; i++ )
		_ptr_i( saved_buffers[i] );
#endif

	_serialize_var( enter_emacs_hook );
	_serialize_var( exit_emacs_hook );
	_serialize_var( leave_emacs_hook );
	_serialize_var( return_to_emacs_hook );
	_ptr_i( enter_emacs_proc );
	_ptr_i( exit_emacs_proc );
	_ptr_i( leave_emacs_proc );
	_ptr_i( return_to_emacs_proc );

	}

static void serialize_star_star( void **ptr, int size )
	{
	int i;

	size /= sizeof( void * );

	for( i = 0; i<size; i++ )
		_serialize_ptr( &ptr[i] );
	}

static void serialize_struct_abbrevent( struct abbrevent *ptr, int PNOTUSED(size) )
	{
	_ptr( abbrev_next );
	_ptr( abbrev_abbrev );
	_ptr( abbrev_phrase );
	_ptr( abbrev_expansion_hook );
	}

static void serialize_struct_abbrevtable( struct abbrevtable *ptr, int PNOTUSED(size) )
	{
	int i;

	_ptr( abbrev_name );
	for( i=0; i<ABBREVSIZE; i++ )
		_ptr( abbrev_table[i] );
	}

static void serialize_struct_automode( struct automode *ptr, int PNOTUSED(size) )
	{
	_ptr( a_pattern );
	_ptr( a_what );
	_ptr( a_next );
	}
static void serialize_struct_binding( struct binding *ptr, int PNOTUSED(size) )
	{
	_ptr( b_inner );
	_ptr( b_exp );
	_ptr( b.b_local_to );
	}
static void serialize_struct_binding_list( struct binding_list *ptr, int PNOTUSED(size) )
	{
	_ptr( bl_flink );
	}
static void serialize_struct_bound_name( struct bound_name *ptr, int PNOTUSED(size) )
	{
	_ptr( b_bound.b_body );
	_ptr( b_proc_name );
	}
static void serialize_struct_database( struct database *ptr, int PNOTUSED(size) )
	{
	_ptr( db_name );
#ifndef vms
	_ptr( dirnm );
	_ptr( datnm );
	_ptr( pagnm );
#endif
	}
static void serialize_struct_dbsearch( struct dbsearch *ptr, int PNOTUSED(size) )
	{
	int i;
	_ptr( dbs_next );
	_ptr( dbs_name );
	
	for( i=0; i<SEARCH_LEN; i++ )
		_ptr( dbs_elements[i] );
	}
static void serialize_struct_emacs_array( struct emacs_array *ptr, int PNOTUSED(size) )
	{
	int i;
	for( i=0; i<ptr->array_total_size; i++ )
		_ptr( array_expr [i] );
	}
static void serialize_struct_emacs_buffer( struct emacs_buffer *ptr, int PNOTUSED(size) )
	{
	_ptr( b_base );
	_ptr( b_syntax.syntax_base );
	_ptr( b_line_numbers );
	_ptr( b_rendition_regions );
	_ptr( b_buf_name );
	_ptr( b_fname );
	_ptr( b_checkpointfn );
	_ptr( b_next );
	_ptr( b_markset );
	_ptr( b_mark );
	_ptr( b_mode.md_abbrev );
	_ptr( b_mode.md_syntax );
	_ptr( b_mode.md_keys );
	_ptr( b_auto_fill_proc );
	_ptr( b_process_key_proc );
	_ptr( b_journal );
	}
static void serialize_struct_emacs_line( struct emacs_line *ptr, int PNOTUSED(size) )
	{
	_ptr( line_next );
	}
static void serialize_struct_errblk( struct errblk *ptr, int PNOTUSED(size) )
	{
	_ptr( e_mess );
	_ptr( e_text );
	_ptr( e_next );
	_ptr( e_prev );
	}
static void serialize_struct_expression( struct expression *ptr, int PNOTUSED(size) )
	{
	_ptr( exp_v.v_string );
	}
static void serialize_struct_extern_func( struct extern_func *ptr, int PNOTUSED(size) )
	{
	_ptr( ef_funcname );
	_ptr( ef_context );
	}
static void serialize_struct_find_data( void *ptr, int size )
	{
	if( saving_environment )
		/* save as all zeros to clear the valid field */
		memset( ptr, 0, size );
	}
static void serialize_struct_journal_block( struct journal_block *ptr, int PNOTUSED(size) )
	{
	return; /* nothing to do */
	}
static void serialize_struct_keymap( struct keymap *ptr, int PNOTUSED(size) )
	{
	_ptr( k_map.k_short );
	}
static void serialize_struct_keymap_long( struct keymap_long *ptr, int PNOTUSED(size) )
	{
	int i;
	for( i=0; i<256; i++ )
		_ptr( k_binding[i] );
	}
static void serialize_struct_keymap_short( struct keymap_short *ptr, int PNOTUSED(size) )
	{
	int i;
	for( i=0; i<ptr->k_used; i++ )
		_ptr( k_sbinding[i] );
	}
static void serialize_struct_marker( struct marker *ptr, int PNOTUSED(size) )
	{
	_ptr( m_buf );
	_ptr( m_next );
	_ptr( m_prev );
	}

#if defined(SUBPROCESSES)
static void serialize_struct_process_blk( struct process_blk *ptr, int PNOTUSED(size) )
	{
#ifdef vms
	_ptr( proc_name );
	_ptr( proc_procedure );

	_ptr( proc_input_channel.chan_process );
	_ptr( proc_input_channel.chan_buffer );
	_ptr( proc_input_channel.chan_procedure );

	_ptr( proc_output_channel.chan_process );
	_ptr( proc_output_channel.chan_buffer );
	_ptr( proc_output_channel.chan_procedure );

	if( !saving_environment )
		{
		/* restore_process will insert all the channels back
		 * into the channel list queue */
		ptr->proc_input_channel.chan_queue.next = NULL;
		ptr->proc_input_channel.chan_queue.prev = NULL;
		ptr->proc_output_channel.chan_queue.next = NULL;
		ptr->proc_output_channel.chan_queue.prev = NULL;
		}
#endif
#ifdef unix
#if defined( __cplusplus )
	throw( EmacsException("do not know how save an environment with processes in it") );
#else
	error( u_str("do not know how save an environment with processes in it"));
	longjmp( serialize_error_jmp, 1 );
#endif
#endif
	}
#endif

extern struct bound_name bound_variable_node;

static void serialize_struct_prognode( struct prognode *ptr, int PNOTUSED(size) )
	{
	int i, limit;
	struct bound_name *proc = NULL;
	
	if( saving_environment )
		proc = ptr->p_proc;
	_ptr( p_proc );
	if( !saving_environment )
		proc = ptr->p_proc;

	limit = ptr->p_nargs;
	if( proc == &bound_variable_node )
		limit = 1;

	for( i=0; i<limit; i++ )
		_ptr( p_args[i].pa_name );
	}

static void serialize_struct_syntax_string( struct syntax_string *ptr, int PNOTUSED(size) )
	{
	_ptr( s_next );
	}
static void serialize_struct_syntax_table( struct syntax_table *ptr, int PNOTUSED(size) )
	{
	int i;

	for( i=0; i<256; i++ )
		_ptr( s_table[i].s_strings );
	_ptr( s_name );
	}
static void serialize_struct_time_request( struct time_request *ptr, int PNOTUSED(size) )
	{
	_ptr( header.next );
	_ptr( header.prev );
	_ptr( param );
	}
static void serialize_struct_timer_entry( struct timer_entry *ptr, int PNOTUSED(size) )
	{
	_ptr( tim_queue.next );
	_ptr( tim_queue.prev );
	_ptr( tim_procedure_to_call );
	}
static void serialize_struct_trmcontrol( struct trmcontrol *ptr, int PNOTUSED(size) )
	{
	_ptr( t_phys_screen );
	_ptr( t_desired_screen );
	}
static void serialize_struct_variablename( struct variablename *ptr, int PNOTUSED(size) )
	{
	_ptr( v_name );
	_ptr( v_binding );
	}
static void serialize_struct_window( struct window *ptr, int PNOTUSED(size) )
	{
	_ptr( w_next );
	_ptr( w_prev );
	_ptr( w_right );
	_ptr( w_left );
	_ptr( w_buf );
	_ptr( w_dot );
	_ptr( w_start );
	}
static void serialize_struct_windowring( struct windowring *ptr, int PNOTUSED(size) )
	{
	_ptr( wr_pushed );
	_ptr( wr_windows );
	_ptr( wr_wn_cur );
	_ptr( wr_bf_cur );
	}

static void serialize_struct_rendition_region( struct rendition_region *ptr, int PNOTUSED(size) )
	{
	_ptr( rr_next );
	_ptr( rr_start_mark );
	_ptr( rr_end_mark );
	}

static void _serialize_bytes( byte *bytes, int length )
	{
	int io_size;

	if( saving_environment )
		io_size = fwrite( bytes, 1, length, serialize_file );
	else
		io_size = fread( bytes, 1, length, serialize_file );

	if( io_size != length )
		{
		error( u_str( saving_environment ? "error writing" : "error reading" ) );
#if defined( __cplusplus )
	if( saving_environment )
		throw( MemManWriteError() );
	else
		throw( MemManReadError() );
#else
		longjmp( serialize_error_jmp, 1 );
#endif
		}
	}

/*
 * the ptr is the value from a (thing *) value that
 * needs converting between a ptr_index and a real ptr
 * for a save convert from ptr to ptr_index.
 * for a restore convert from ptr_index to ptr
 */
static void _serialize_ptr( void **ptr )
	{
	/* map 0 to NULL and NULL to 0 */
	if( *ptr == NULL )
		return;

	if( saving_environment )
		{
		struct heap_entry *h;
		int index;

		h = (struct heap_entry *)(((char *)(*ptr))-offsetof(struct heap_entry,user_data));
		/* only process addreses inside the heap */
		if( h < ptr_info.low_addr_ptr || h > ptr_info.high_addr_ptr )
			return;

		/* find the index out of the heap_entry structure */
		index = h->ptr_index;
		if( index < 1 || index > ptr_info.max_index )
			return;
		/* confirm its a heap_entry by checking the reverse mapping */
		if( ptr_info.ptr_vector[index] != h )
			return;

		*(int *)ptr = -index;		
		}
	else
		{
		int index = -*(int *)ptr;

		if( index >= 1 && index <= ptr_info.max_index )
			*ptr = ptr_info.ptr_vector[index];
		else if( *ptr > ptr_info.low_code_ptr && *ptr < ptr_info.high_code_ptr )
			*(char **)ptr += ptr_info.relocation_code_offset;
		else if( *ptr > ptr_info.low_cdata_ptr && *ptr < ptr_info.high_cdata_ptr )
			*(char **)ptr += ptr_info.relocation_cdata_offset;
		else if( *ptr > ptr_info.low_idata_ptr && *ptr < ptr_info.high_idata_ptr )
			*(char **)ptr += ptr_info.relocation_idata_offset;
		else if( *ptr > ptr_info.low_udata_ptr && *ptr < ptr_info.high_udata_ptr )
			*(char **)ptr += ptr_info.relocation_udata_offset;
		}
	}

static void _serialize_ptr_indirect( void **ptr )
	{
	if( saving_environment )
		{
		void *p = *ptr;

		_serialize_ptr( &p );
		_serialize_bytes( (byte *)&p, sizeof( void * ) );
		}
	else
		{
		_serialize_bytes( (byte *)ptr, sizeof( void * ) );
		_serialize_ptr( ptr );
		}
	}
#else
int save_environment( void )
	{
	return no_value_command();
	}

int restore_environment( unsigned char *fn )
	{
	return 0;
	}
#endif
