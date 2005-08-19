/*	Copyright (c) 1987                                                  */
/*		Barry A. Scott and Nick Emery                               */

/*	Definition for the journal subsystem                                */

enum jnl_types
	{
	JNL_END,			/* end of block                     */
	JNL_FILENAME,			/* name of file being journalled    */
	JNL_BUFFERNAME,			/* name of buffer being journalled  */
	JNL_INSERT,			/* insert text                      */
	JNL_DELETE			/* delete text                      */
	};
#define JNL_VERSION 2

struct jnl_open
	{
	enum jnl_types jnl_type;
	int jnl_version;
	int jnl_name_length;
	};

struct jnl_insert
	{
	enum jnl_types jnl_type;
	int jnl_dot;
	int jnl_insert_length;
	};

struct jnl_delete
	{
	enum jnl_types jnl_type;
	int jnl_del_dot;
	int jnl_length;
	};

struct jnl_data
	{
	unsigned char jnl_chars[ sizeof(int) * 4 ];
	};

#define JNL_BYTE_SIZE sizeof( struct jnl_data )
#define JNL_BYTE_TO_REC( n ) (((n)+JNL_BYTE_SIZE-1)/JNL_BYTE_SIZE)

union journal_record
	{
	struct jnl_open jnl_open;
	struct jnl_insert jnl_insert;
	struct jnl_delete jnl_delete;
	struct jnl_data jnl_data;
	};

#define JNL_BUF_SIZE 64		/* this yields a 1k buffer */
struct journal_block
	{
        unsigned jnl_active : 1;
        unsigned jnl_open : 1;
        unsigned jnl_flush : 1;
        unsigned jnl_rab_inuse : 1;
        unsigned jnl_buf1_current : 1;
	FILE *jnl_file;
	unsigned char jnl_jname [MAXPATHLEN+1];
	int jnl_used;		/* records used in the current journal buf */
	int jnl_record;		/* last record written in the current journal buffer */
	union journal_record *jnl_buf;
	union journal_record jnl_buf1 [JNL_BUF_SIZE];
	union journal_record jnl_buf2 [JNL_BUF_SIZE];
	};
