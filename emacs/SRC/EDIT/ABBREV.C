/*	Copyright (c) 1982-1993
 *		Barry A. Scott and Nick Emery */

#include <emacs.h>

/*forward*/ void fetch_last_phrase( struct expression *e );
/*forward*/ static int hash_abbrev( unsigned char *s );
/*forward*/ static struct abbrevent *lookup_abbrev( struct abbrevtable *table, unsigned char *name, int h );
/*forward*/ static void define_abbrev_phrase( struct abbrevtable *table, unsigned char *abbrev, unsigned char *phrase, struct bound_name *proc );
static void define_abbrev( struct abbrevtable *table, unsigned char *s, int hooked );
/*forward*/ static struct abbrevtable *locate_abbrev( unsigned char *name );
/*forward*/ int define_global_abbrev( void );
/*forward*/ int define_local_abbrev( void );
/*forward*/ int define_hooked_global_abbrev( void );
/*forward*/ int define_hooked_local_abbrev( void );
/*forward*/ int test_abbrev_expand( void );
/*forward*/ int abbrev_expand( void );
/*forward*/ int use_abbrev_table( void );
/*forward*/ static void write_abbrevs( FILE *f, struct abbrevtable *table );
/*forward*/ int write_abbrev_file( void );
/*forward*/ static int read_abbrevs( unsigned char *s, int report );
/*forward*/ int read_abbrev_file( void );
/*forward*/ int quietly_read_abbrev_file( void );
/*forward*/ void fetch_current_abbrev_table( struct expression *e );
/*forward*/ int check_current_abbrev_table( unsigned char *value, struct variablename *v );
/*forward*/ void init_abbrev( void );


GLOBAL SAVRES struct abbrevtable global_abbrev;
GLOBAL SAVRES unsigned char * abbrev_table_names[MAX_ABBREV_TABLES];
GLOBAL SAVRES struct abbrevtable *abbrev_tables[MAX_ABBREV_TABLES];
GLOBAL SAVRES int number_of_abbrev_tables;
GLOBAL SAVRES unsigned char *last_phrase;
unsigned char current_buffer_abbrev[80];

unsigned char *	two_percents	= u_str("%s: %s");
unsigned char *	abbreviations	= u_str(".abbreviations");

void fetch_last_phrase
	(
	struct expression *e
	)
	{
	e->exp_v.v_string = last_phrase;
	e->exp_int = _str_len( last_phrase );
	}

static int hash_abbrev	/* hash_abbrev an abbrev string */
	(
	unsigned char *s
	)
	{
	int h;
	h = 0;
	while( s[0] != 0 )
		h = h * 31 + *s++;
	return h&INT_MAX;	/* make sure that h is positive */
	}
/*
 * look up an abbrev in the given table with the given name whose hash_abbrev is h.
 */
static struct abbrevent *lookup_abbrev
	(
	struct abbrevtable *table,
	unsigned char *name,
	int h
	)
	{
	struct abbrevent *p;

	p = table->abbrev_table[h % ABBREVSIZE ];
	while( p != 0
	&&	(p->abbrev_hash != h
		|| _str_cmp( name, p->abbrev_abbrev ) != 0) )
		p = p->abbrev_next;
	return p;
	}

/* in the given abbrev table define_abbrev the given abbreviation for the given phrase */
static void define_abbrev_phrase
	(
	struct abbrevtable *table,
	unsigned char *abbrev,
	unsigned char *phrase,
	struct bound_name *proc
	)
	{
	struct abbrevent *p;
	int h;

	h = hash_abbrev( abbrev );
	if( (p = lookup_abbrev( table, abbrev, h )) != 0 )
		free( p->abbrev_phrase );
	else
		{
		p = malloc_struct( abbrevent );
		p->abbrev_hash = h;
		p->abbrev_abbrev = savestr( abbrev );
		p->abbrev_next = table->abbrev_table[h % ABBREVSIZE ];
		table->abbrev_number_defined = table->abbrev_number_defined + 1;
		table->abbrev_table[h % ABBREVSIZE ] = p;
		}
	p->abbrev_phrase = savestr( phrase );
	p->abbrev_expansion_hook = proc;
	}

/*
 * given the name of an abbrev table, return a
 * pointer to it. If it does not exist, create it
 */
static struct abbrevtable *locate_abbrev
	(
	unsigned char *name
	)
	{
	struct abbrevtable *p;
	int i;

	if( name == 0 || name[0] == 0 )
		return 0;
	for( i=0; i<=number_of_abbrev_tables-1; i += 1 )
		if( _str_cmp( abbrev_table_names[i], name ) == 0 )
			return abbrev_tables[i];

	if( number_of_abbrev_tables >= MAX_ABBREV_TABLES )
		{
		error( u_str("Too many abbrev tables!") );
		return 0;
		}
	p = malloc_struct( abbrevtable );
	abbrev_tables[ number_of_abbrev_tables ] = p;
	abbrev_table_names[ number_of_abbrev_tables ] = p->abbrev_name =
				savestr( name );
	number_of_abbrev_tables++;
	p->abbrev_number_defined = 0;
	for( i=0; i<=ABBREVSIZE - 1; i += 1 )
		p->abbrev_table[i ] = 0;
	return p;
	}

static void define_abbrev
	(
	struct abbrevtable *table,
	unsigned char *s,
	int hooked
	)
	{
	unsigned char *abbrev;
	unsigned char *phrase;
	int hookinx;
	unsigned char s_abbrev[300];

	abbrev = getnbstr( u_str(": define-%s%s-abbrev "),
	            (( hooked ) ?  u_str("hooked-") : u_str("")), s );
	if( abbrev == 0 )
		return;

	_str_cpy( s_abbrev, abbrev );
	phrase = getstr( u_str(": define-%s%s-abbrev %s phrase: "),
		(( hooked ) ?  u_str("hooked-") : u_str("")),
		 s, &s_abbrev[0]);
	if( phrase == 0 )
		return;
	hookinx = -1;
	phrase = savestr( phrase );
	if( hooked
	&& (hookinx = getword( mac_names_ref, u_str("Hooked to procedure: "))) < 0 )
		return;
	define_abbrev_phrase( table, &s_abbrev[0], phrase,
		(( hookinx < 0 ) ?  NULL
			: mac_bodies[ hookinx ] ));
	}

int define_global_abbrev( void )
	{
	define_abbrev( &global_abbrev, u_str("global"), 0 );
	bf_cur->b_mode.md_abbrevon  = bf_mode.md_abbrevon = 1;
	return 0;
	}

int define_local_abbrev( void )
	{
	if( bf_mode.md_abbrev == 0 )
		error (u_str("No abbrev table associated with this buffer."));
	else
		define_abbrev( bf_mode.md_abbrev, u_str("local"), 0 );
	bf_cur->b_mode.md_abbrevon  = bf_mode.md_abbrevon = 1;
	return 0;
	}

int define_hooked_global_abbrev( void )
	{
	define_abbrev( &global_abbrev, u_str("global"), 1 );
	return 0;
	}

int define_hooked_local_abbrev( void )
	{
	if( bf_mode.md_abbrev == 0 )
		error (u_str("No abbrev table associated with this buffer."));
	else
		define_abbrev( bf_mode.md_abbrev, u_str("local"), 1);
	return 0;
	}

int test_abbrev_expand( void )
	{
	unsigned char *abbrev;
	struct abbrevent *p;

	abbrev = getnbstr (u_str(": test-abbrev-expand "));
	p = 0;

	if( abbrev == 0 )
		return 0;
	if( bf_mode.md_abbrev != 0 )
		p = lookup_abbrev( bf_mode.md_abbrev, abbrev, hash_abbrev( abbrev ) );
	if( p == 0 )
		p = lookup_abbrev( &global_abbrev, abbrev, hash_abbrev( abbrev ) );
	if( p == 0 )
		error( u_str("Abbrev \"%s\" is not defined"), abbrev );
	else
		{
		message( u_str("\"%s\" -> \"%s\" (%d)"),
			p->abbrev_abbrev, p->abbrev_phrase, p->abbrev_hash );
		release_expr( ml_value );
		ml_value->exp_type = ISSTRING;
		ml_value->exp_release = 1;
		ml_value->exp_int = _str_len( p->abbrev_phrase );
		ml_value->exp_v.v_string = savestr( p->abbrev_phrase );
		}
	return 0;
	}

/*
 * called from self_insert to possibly expand the abbrev that preceeds dot
 */
int abbrev_expand( void )
	{
	unsigned char *p;
	int n;
	struct abbrevent *a;
	unsigned char c;
	int h;
	int rv;
	int uccount;
	unsigned char buf[200];

	n = dot;
	rv = 0;
	uccount = 0;

	p = &buf[ + sizeof( buf ) - 1 ];
	p[0] = 0;
	while( (n = n - 1) >= 1 && char_is( c = char_at( n ), SYNTAX_WORD ) )
		{
		p = &p[-1];
		p[0] = c;
		if( isupper( c ) )
			{
			uccount++;
			p[0] += 'a' - 'A';
			}
		}
	h = hash_abbrev( p );
	if(	(bf_mode.md_abbrev == 0
		|| (a = lookup_abbrev( bf_mode.md_abbrev, p, h )) == 0)
	&&	(a = lookup_abbrev( &global_abbrev, p, h )) == 0 )
		return 0;

	if( a->abbrev_expansion_hook != 0 )
		{
		last_phrase = a->abbrev_phrase;
		rv = execute_bound_saved_environment( a->abbrev_expansion_hook );
		last_phrase = u_str("");
		}
	else
		{
		unsigned char ch;

		bf_mode.md_abbrevon = 0;
		del_back( dot, h = &buf[sizeof( buf )] - p - 1 );
		dot_left( h );
		p = a->abbrev_phrase;

		while( (ch = *p++) != 0 )
			if( (islower( ch )
			&& uccount != 0
			&&	(&p[-1] == a->abbrev_phrase
				|| (uccount > 1
					&& isspace( p[-2] )))) )
				self_insert( (unsigned char)toupper( ch ) );
			else
				self_insert( ch );
		bf_mode.md_abbrevon = 1;
		}
	return rv;
	}

/*
 * select a named abbrev table for this buffer
 * and turn on abbrev mode if it or the global
 * abbrev table is non-empty
 */
int use_abbrev_table( void )
	{
	struct abbrevtable *p;
	p = locate_abbrev( getnbstr( u_str(": use-abbrev-table ") ) );
	if( p == 0 )
		return 0;

	bf_cur->b_mode.md_abbrev  = bf_mode.md_abbrev = p;

	if( p->abbrev_number_defined > 0
	|| global_abbrev.abbrev_number_defined > 0 )
		{
		bf_cur->b_mode.md_abbrevon  = bf_mode.md_abbrevon = 1;
		cant_1win_opt = 1;
		}
	return 0;
	}



int list_abbreviation_tables( void )
	{
	unsigned char line[150];
	struct emacs_buffer *old;
	struct abbrevtable *at;
	int i;

	old = bf_cur;

	scratch_bfn( u_str("Abbreviation table list"), interactive );

	ins_str( u_str("  Table\n") );
	ins_str( u_str("  -----\n") );

	for( i=0; i<=number_of_abbrev_tables - 1; i += 1 )
		{
		at = abbrev_tables[i];
		sprintfl( line, sizeof( line ) - 1,
			u_str("  %s\n"), at->abbrev_name );
		ins_str( line );
		}

	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );

	return 0;
	}



int dump_abbreviation_tables( void )
	{
	unsigned char line[150];
	struct emacs_buffer *old;
	struct abbrevent *p;
	struct abbrevtable *table;
	int i;

	table = locate_abbrev( getnbstr( u_str(": dump-abbrev-table ") ) );
	if( table == 0 )
		return 0;

	old = bf_cur;

	scratch_bfn( u_str("Abbreviation table"), interactive );

	sprintfl( line, sizeof( line ) - 1,
		u_str("  Table: %s\n\n"), table->abbrev_name );
	ins_str( line );

	ins_str( u_str("  Abbreviation    Phrase                          Hook\n") );
	ins_str( u_str("  ------------    ------                          ----\n") );

	for( i=0; i<=ABBREVSIZE - 1; i += 1 )
		{
		p = table->abbrev_table[i ];
		while( p != 0 )
			{
			unsigned char *hook;

			hook = u_str("");
			if( p->abbrev_expansion_hook != 0 )
				hook = p->abbrev_expansion_hook->b_proc_name ;
			sprintfl( line, sizeof( line ) - 1,
				u_str("  %-15s %-31s %s\n"),
				p->abbrev_abbrev, p->abbrev_phrase, hook );
			ins_str( line );

			p = p->abbrev_next;
			}
		}

	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );

	return 0;
	}



static void write_abbrevs	/* write the given abbrev table to file f */
	(
	FILE *f,
	struct abbrevtable *table
	)
	{
	int i;
	unsigned char buf[500];
	struct abbrevent *p;

	fio_put( f, table->abbrev_name, _str_len( table->abbrev_name ) );
	for( i=0; i<=ABBREVSIZE - 1; i += 1 )
		{
		p = table->abbrev_table[i ];
		while( p != 0 )
			{
			sprintfl( buf, sizeof( buf ),
				u_str(" %s	%s"),
				p->abbrev_abbrev, p->abbrev_phrase );
			fio_put( f, buf, _str_len( buf ) );
			p = p->abbrev_next;
			}
		}
	}

int write_abbrev_file( void )
	{
	unsigned char *fn;
	FILE *f;
	int i;

	fn = getstr( u_str(": write-abbrev-file ") );
	if( fn == 0 )
		return 0;
	if( (f = fio_create( fn, 0, FIO_STD, abbreviations, default_rms_attribute ) ) == 0 )
		error( two_percents, fetch_os_error( errno ), fn );
	else
		{
		for( i=0; i<=number_of_abbrev_tables - 1; i += 1 )
			write_abbrevs( f, abbrev_tables[i] );
		fio_close( f );
		}
	return 0;
	}

static int read_abbrevs
	(
	unsigned char *s,
	int report
	)
	{
	unsigned char *name;
	FILE *f;
	int len;
	unsigned char buf[500];
	struct abbrevtable *table;
	unsigned char *p;
	unsigned char *phrase;
	unsigned char fullname[MAXPATHLEN+1];

	name = getstr( s );
	table = 0;
	if( name == 0 )
		return 0;

	if( (f = fopenp( EMACS_PATH, name, fullname, FIO_READ, abbreviations )) == 0 )
		{
		if( report )
			error( two_percents, fetch_os_error( errno ), name);
		return 0;
		}
	while( (len = fio_get_line( f, buf, sizeof( buf )-1 )) > 0
	&& ! err )
		{
		buf[ len ] = 0;
		if( buf[0] != ' ' )
			{
			p = buf;
			while( p[0] != 0 )
				if( *p == '\n' || *p == '\r' )
					*p = '\0';
				else
					p++;
			table = locate_abbrev( buf );
			}
		else
			if( table != 0 )
				{
				p = &buf[1];
				while( p[0] != 0 && p[0] != '\t' )
					p = &p[1];
				if( p[0] == 0 )
					{
					error( u_str("Improperly formatted abbrev file: %s"), name );
					return 0;
					}
				*p++ = '\0';
				phrase = p;
				while( p[0] != 0 && p[0] != '\n' && p[0] != '\r' )
					p = &p[1];
				p[0] = 0;
				define_abbrev_phrase( table, &buf[1], phrase, 0 );
				}
		}
	fio_close( f );
	return 0;
	}

int read_abbrev_file( void )
	{
	return read_abbrevs( u_str(": read-abbrev-file "), 1 );
	}

int quietly_read_abbrev_file( void )
	{
	return read_abbrevs( u_str(": quietly-read-abbrev-file "), 0 );
	}

void fetch_current_abbrev_table( struct expression *e )
	{
	_str_cpy( e->exp_v.v_string,
		(( bf_mode.md_abbrev != 0 ) ?
			bf_mode.md_abbrev->abbrev_name 
		:	u_str("")) );
	}

int check_current_abbrev_table
	(
	unsigned char *value,
	struct variablename *v
	)
	{
	struct abbrevtable *p;

	v = v;	/* touch unused arg */

	if( _str_len( value ) <= 0 )
		{
		error( u_str("Illegal abbrev table name") );
		return 0;
		}

	p = locate_abbrev( value );
	bf_cur->b_mode.md_abbrev  = bf_mode.md_abbrev = p;
	if( p->abbrev_number_defined > 0
	|| global_abbrev.abbrev_number_defined > 0 )
		{
		bf_cur->b_mode.md_abbrevon  = bf_mode.md_abbrevon = 1;
		cant_1win_opt = 1;
		}

	return 1;
	}

void init_abbrev( void )
	{
	last_phrase = u_str("");
	global_abbrev.abbrev_name = abbrev_table_names[0] = u_str("global");
	abbrev_tables[0] = &global_abbrev;
	number_of_abbrev_tables = 1;
	}
