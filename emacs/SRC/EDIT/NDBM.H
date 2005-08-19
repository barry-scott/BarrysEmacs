/*	Copyright (c) 1982, 1983, 1984, 1985 */
/*		Barry A. Scott and nick Emery */

#if defined( vms )
struct database
    {
    int db_rdonly;
    int db_reopen;
    unsigned char *db_name;
    int db_dirf;
    int db_pagf;
    int db_datf;
    };
#elif !defined( DB_MLL )
#define	PBLKSIZ	4096		/* Page block size */
#define	DBLKSIZ	4096		/* directory block size */
#define	BYTESIZ	8		/* bits per byte */

struct database
    {
    int db_rdonly;
    int db_reopen;
    unsigned char *db_name;
    int db_dirf;
    int db_pagf;
    int db_datf;
    long bitno;
    long maxbno;
    long blkno;
    long hmask;
    long oldpagb;
    long olddirb;
    unsigned char pagbuf[PBLKSIZ];
    unsigned char dirbuf[DBLKSIZ];
    unsigned char *dirnm, *datnm, *pagnm;
    };
typedef struct database database;

extern database * lastdatabase;

struct datum
    {
    unsigned char   *dptr;
    int dsize;
    long val1, val2;
    };
typedef struct datum datum;

extern struct database *open_db(unsigned char *,int );
extern void close_db(struct database *);
extern void free_db(struct database *);
extern long forder(struct datum ,struct database *);
extern struct datum fetch(struct datum ,struct database *);
extern int delete_key(struct datum ,struct database *);
extern int store(struct datum ,struct database *);
extern struct datum firstkey(struct database *);
extern struct datum nextkey(struct datum ,struct database *);
extern struct datum firsthash(long ,struct database *);
extern struct datum makdatum(unsigned char *,int );
extern long hashinc(long ,struct database *);
extern long calchash(struct datum );
extern int put_db(unsigned char *,int ,unsigned char *,int ,struct database *);
extern int get_db(unsigned char *,int ,unsigned char **,int *,unsigned char *(*)(int ,unsigned char *),struct database *);
extern int get_db_help(unsigned char *,unsigned char *(*)(int ,unsigned char *),struct database *);
extern int index_db(unsigned char *,int (*)(int ,unsigned char *,unsigned char **),struct database *);

#else
struct database_entry
	{
	unsigned char *dbe_name;
	long int dbe_position;
	int dbe_length;
	};

struct database
	{
	int db_rdonly;
	int db_reopen;
	unsigned char *db_name;
	int db_dirf;
	FILE *db_file;
	size_t db_data_length;
	unsigned char *db_data_pointer;
	int db_num_entries;
	struct database_entry *db_entries;
	};
#endif

/* maximum number of components in a database search list */
#define SEARCH_LEN 10
struct dbsearch
	{                       /* a database search list */
	int dbs_size;			/* number of components */
	struct dbsearch *dbs_next;          /* the next search list */
	unsigned char *dbs_name;            /* the name of this search list*/
	struct database *dbs_elements [SEARCH_LEN]; /* the components */
	/* each is a database from the ndbms package */
	};
