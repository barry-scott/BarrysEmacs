/*
 * 	Copyright(c) 1982, 1983, 1984, 1985. 1987
 *		Barry A. Scott and Nick Emery
 */

/* Simple data base manager routines. */

#include <emacs.h>


#define GROW 10			/* Size by which to grow search list names */
#define READONLY 1		/* Read only access */
#define OPEN 2			/* remain open access */

/*forward*/ int extend_database_search_list( void );
/*forward*/ int fetch_database_entry( void );
/*forward*/ int fetch_help_database_entry( void );
/*forward*/ int put_database_entry( void );
/*forward*/ int list_databases( void );
/*forward*/ int remove_database( void );
/*forward*/ void init_db( void );
/*forward*/ void restore_db( void );

GLOBAL SAVRES int get_db_help_flags = 7;	/* All information required */

#ifdef DB
/*forward*/ struct dbsearch *find_sl(unsigned char *name);
/*forward*/ static unsigned char *insertion_func (int n, unsigned char *p);
/*forward*/ void fetch_database_index( struct dbsearch *dbs, unsigned char *match,
			int (*helper)( int , unsigned char *, unsigned char * * ));

GLOBAL SAVRES struct dbsearch *dbroot;
GLOBAL SAVRES int db_count = 0;
GLOBAL SAVRES unsigned char **db_search_lists = 0;
GLOBAL SAVRES int db_spaceleft = 0;

unsigned char *no_such_db_str =  u_str("No such database search list \"%s\"");
unsigned char *no_help_db_str = u_str("help entry \"%s\" not found in \"%s\"");
unsigned char *db_empty_str = u_str("Empty database search list \"%s\"");
unsigned char *key_not_found_str = u_str("Entry \"%s\" not found in \"%s\"");


struct dbsearch *find_sl (unsigned char *name)
	{
	struct dbsearch *p = dbroot;

	while( p != 0)
		if( _str_cmp (p->dbs_name, name) == 0 )
			return p;
		else
			p = p->dbs_next;

	return NULL;
	}


int extend_database_search_list( void )
	{
	unsigned char *name;
	unsigned char buf[512];
	unsigned char *content;
	struct dbsearch *p;
	int i;
	struct database *db;
	int reopen = 0;

	name = getescword( db_search_lists, u_str(": extend-database-search-list (list) ") );
	if( name == NULL )
		return 0;

	p = find_sl (name);
	if( p == NULL )
		{
		p = malloc_struct(dbsearch);
		p->dbs_name = savestr(name);
		p->dbs_next = dbroot;
		dbroot = p;
		p->dbs_size = 0;
		if( db_spaceleft <= 1 )
			{
			db_search_lists = (unsigned char **)realloc
				(
				db_search_lists,
				(db_count + db_spaceleft + GROW) * sizeof( unsigned char *),
				malloc_type_star_star
				);
			db_spaceleft += GROW;
			}
		if( db_search_lists != NULL )
			{
			db_search_lists[db_count] = p->dbs_name;
			db_count++;
			db_search_lists[db_count] = NULL;
			db_spaceleft--;
			}
		}
	sprintfl (buf, sizeof (buf),
			u_str( ": extend-database-search-list (list) %s (database) " ),
			p->dbs_name);

        content = getescfile (buf, NULL, 0, 1);
	if( content == NULL )
		return 0;

	expand_and_default (content, EMACS_DB_DEFAULT, buf);
	content = buf;

	i = 0;
	while( i < p->dbs_size)
		{
		struct database *dbx = p->dbs_elements[i];
		if( _str_cmp( content, dbx->db_name) == 0 )
			return 0;
		i++;
		}
	if( p->dbs_size == SEARCH_LEN )
		{
		error( u_str("Too many components in search list") );
		return 0;
		}
	if( arg_state == have_arg )
		reopen = arg;
	else
		if( ! interactive && cur_exec->p_nargs > 2 )
			reopen = getnum( u_str(": extend-database-search-list (flags) "));

	db = open_db( content, reopen & READONLY );
	if( db == 0 )
		error( u_str("Cannot find database \"%s\""), content );
	else
		{
#ifndef unix
                db->db_reopen = (reopen & OPEN) == 0;
#endif
		i = p->dbs_size;
		while( i > 0)
			{
			p->dbs_elements[i] = p->dbs_elements[i-1];
			i--;
			}
		p->dbs_elements[0] = db;
		p->dbs_size = p->dbs_size + 1;
		if( (reopen & OPEN) == 0 )
			close_db (db);
		}

	return 0;
	}



/* function for inserting text into a buffer -- given a size returns a
 * pointer to a region of size characters */
static unsigned char *insertion_func (int n, unsigned char *p)
	{
	/* insert the text */
	ins_cstr( p, n );
	/* free the buffer */
	free( p );
	/* tell NDBM code to not copy or free the buffer */
	return NULL;
	}

/* fetch an entry from a database into the current buffer */
int fetch_database_entry( void )
	{
	unsigned char *dbname = getescword(db_search_lists,u_str(": fetch-database-entry (list) "));
	struct dbsearch *dbs;
	int i;
	unsigned char *key;
	unsigned char *content;
	int keylen;
	int contentlen;

	if( dbname == 0 )
		return 0;

	dbs = find_sl (dbname);
	if( dbs == 0 )
		{
		error (no_such_db_str, dbname);
		return 0;
		}
	key = getescdb(dbs, u_str(": fetch-database-entry (list) %s (entry) "),
	    dbs->dbs_name);
	if( key == NULL )
		return 0;

	keylen = _str_len (key);
	i = 0;
	while( i < dbs->dbs_size)
		{
		if( get_db( key, keylen, &content, &contentlen, insertion_func,
		   dbs->dbs_elements[i] ) >= 0 )
                        break;
		i++;
		}
	cant_1line_opt = 1;
	if( i > dbs->dbs_size )
		error( key_not_found_str, key, dbname );

	return 0;
	}


/* fetch an entry from a help database into the current buffer */
int fetch_help_database_entry( void )
	{
	unsigned char *dbname = getescword(db_search_lists,
					u_str(": fetch-help-database-entry (list) "));
	struct dbsearch *dbs;
	int i;
	unsigned char *keys;

	if( dbname == 0 )
		return 0;

	dbs = find_sl (dbname);
	if( dbs == 0 )
		{
		error(no_such_db_str, dbname);
		return 0;
		}
	if( (keys = getstr (u_str (": fetch_-help-database-entry (list) %s (topic) "),
	    dbs->dbs_name)) == 0 )
		return 0;

	i = 0;
	while( i < dbs->dbs_size)
		{
		if( get_db_help (keys, insertion_func, dbs->dbs_elements[i]) >= 0 )
			break;
		i++;
		}
	cant_1line_opt = 1;
	if( i >= dbs->dbs_size )
		error(no_help_db_str, keys, dbname);
	return 0;
	}



/* Put the contents of the current buffer into a database */
int put_database_entry( void )
	{
	unsigned char *dbname = getescword(db_search_lists, u_str(": put-database-entry (list)"));
	struct dbsearch *dbs;
	struct database *dbx;
	unsigned char *key;

	if( dbname == 0 )
		return 0;

	dbs = find_sl (dbname);
	if( dbs == 0 )
		{
		error (no_such_db_str, dbname);
		return 0;
		}
	if( dbs->dbs_size <= 0 )
		{
		error (db_empty_str, dbs->dbs_name);
		return 0;
		}
	dbx = dbs->dbs_elements[0];
	if( dbx->db_rdonly )
		{
		error (u_str("\"%s\" is a read-only database."), dbs->dbs_name);
		return 0;
		}
	key = getescdb (dbs, u_str(": put-database-entry (list) %s (entry) "), dbs->dbs_name);
	if( key == 0 )
		return 0;

	gap_to (bf_s1 + bf_s2 + 1);
	if( put_db (key, _str_len (key), ref_char_at (1), bf_s1 + bf_s2, dbx) < 0 )
		error (u_str("Database put failed"));
	return 0;
	}



/* List the names and contents of all database search lists */
int list_databases( void )
	{
	struct dbsearch *p;
	int i;
	struct emacs_buffer *old = bf_cur;

	scratch_bfn( u_str("Database list"), interactive );

	p = dbroot;
	while( p != 0)
		{
		ins_str (p->dbs_name); ins_cstr (u_str (":\n"), 2);
		i = 0;
		while( i < p->dbs_size)
			{
			struct database *db = p->dbs_elements[i];

			ins_cstr (u_str ("    "), 4);
			ins_str (db->db_name);
			if( db->db_rdonly || ! db->db_reopen )
				{
				ins_cstr (u_str ("    ("), 5);
				if( db->db_rdonly )
					{
					ins_cstr (u_str ("read only"), 9);
					if( ! db->db_reopen )
						ins_cstr (u_str (", "), 2);
					}
				if( ! db->db_reopen )
					ins_cstr (u_str ("open"), 4);
				ins_cstr (u_str (")"), 1);
				}
			ins_cstr (u_str ("\n"), 1);
			i++;
			}
		ins_cstr (u_str ("\n"), 1);
		p = p->dbs_next;
		}
	bf_modified = 0;
	bf_cur->b_checkpointed = -1;
	set_dot (1);
	set_bfp (old);
	window_on (bf_cur);
	return 0;
	}



int remove_database( void )
	{
	unsigned char *name;
	int content;
	struct dbsearch *p;
	unsigned char **entries;
	int i;

	name = getescword (db_search_lists, u_str(": remove-database (list) "));
	if( name == 0 )
		return 0;

	p = find_sl (name);
	if( p == 0 )
		{
		error(no_such_db_str, name);
		return 0;
		}
	if( p->dbs_size <= 0 )
		error(db_empty_str, p->dbs_name);
	else
		if( (entries = (unsigned char **)malloc((p->dbs_size + 1) * sizeof( unsigned char * ), malloc_type_star_star)) == 0 )
			error (u_str ("Out of memory in remove-database"));
		else
			{
			i = 0;
			while( i < p->dbs_size)
				{
				struct database *dbx = p->dbs_elements[i];
				entries[i] =  dbx->db_name;
				i++;
				}
			entries[p->dbs_size] = 0;
			content = getword(entries,
			    u_str( ": remove-database (list) %s (database) "), p->dbs_name);
			if( content >= 0 )
				{
				free_db(p->dbs_elements[content]);
				i = content;
				while( i < p->dbs_size - 1)
					{
					p->dbs_elements[i] = p->dbs_elements[i + 1];
					i++;
					}
				p->dbs_size--;
				}
			free(entries);
			}
	return 0;
	}


/* fetch an index of a databases entries */
void fetch_database_index ( struct dbsearch *dbs, unsigned char *match,
	int (*helper)( int , unsigned char *, unsigned char * * ))
	{
	int i;
	unsigned char string[512];

	_str_cpy( string, match );
#ifdef vms
	_str_upr( string );
#endif
	_str_cat( string, "*" );

	i = 0;
	while( i < dbs->dbs_size)
		{
		index_db( string, helper, dbs->dbs_elements[i] );
		i++;
		}
	}



/* init_ialization for when EMACS is started up */
void init_db( void )
	{
	db_search_lists = (unsigned char **)malloc(GROW * sizeof( unsigned char * ), malloc_type_star_star);
	db_spaceleft = GROW;
	if( db_search_lists != 0 )
		db_search_lists[0] = NULL;
	}

/* initialization for when EMACS starts up with a restore environment */
void restore_db( void )
	{
	int i;
	struct dbsearch *p;

	p = dbroot;
	while( p != 0)
		{
		i = 0;
		while( i < p->dbs_size)
			{
			struct database *dbx = p->dbs_elements[i];
#if defined MLL_DB || defined vms
                        dbx->db_dirf = 0;
#else
                        dbx->db_dirf = -1;
                        dbx->db_pagf = -1;
                        dbx->db_datf = -1;
#endif
			i++;
			}
		p = p->dbs_next;
		}
	}
#else
int extend_database_search_list( void )
	{
	return no_value_command();
	}

int fetch_database_entry( void )
	{
	return no_value_command();
	}

int fetch_help_database_entry( void )
	{
	return no_value_command();
	}

int put_database_entry( void )
	{
	return no_value_command();
	}

int list_databases( void )
	{
	return no_value_command();
	}

int remove_database( void )
	{
	return no_value_command();
	}

void init_db( void )
	{
	return;
	}

void restore_db( void )
	{
	return;
	}

#endif
