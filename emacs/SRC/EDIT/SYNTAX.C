/*	Copyright (c) 1982-1993
 *		Barry A. Scott and nick Emery
 *
 * Emacs routines to deal with syntax tables
 */
#include <emacs.h>

/*forward*/ static struct syntax_table *locate_syntax_table( unsigned char *name, int create );
/*forward*/ int use_syntax_table( void );
/*forward*/ int modify_syntax_table( void );
/*forward*/ static void modify_syntax_table_inner( int type, unsigned char *str1, unsigned char *str2 );
/*forward*/ int modify_syntax_entry( void );
/*forward*/ static int paren_scan(int stop_at_newline, int forw);
/*forward*/ int backward_paren_bl( void );
/*forward*/ int backward_paren( void );
/*forward*/ int forward_paren_bl( void );
/*forward*/ int forward_paren( void );
/*forward*/ int dump_syntax_table( void );
/*forward*/ int list_syntax_tables( void );
/*forward*/ void fetch_current_syntax_table( struct expression *e );
/*forward*/ int check_current_syntax_table( unsigned char *value, struct variablename *v );
/*forward*/ int syntax_loc( void );
/*forward*/ static int scan_comment( int i, struct syntax_string *comment );
/*forward*/ static int syntax_looking_at( int pos, int len, unsigned char *str );
/*forward*/ int current_syntax_entity( void );
/*forward*/ void init_syntax( void );
/*forward*/ void syntax_insert_update( int, int );
/*forward*/ void syntax_delete_update( int, int );
/*forward*/ static void syntax_update_buffer( int pos, int len );

GLOBAL SAVRES unsigned char *syntax_table_names[MAX_SYNTAX_TABLES];
GLOBAL SAVRES struct syntax_table *syntax_tables[MAX_SYNTAX_TABLES];
GLOBAL SAVRES struct syntax_table global_syntax_table;
GLOBAL SAVRES int number_of_syntax_tables;

#ifdef _DEBUG
unsigned char *_debug_ref_syntax_at( int n, unsigned char *syn_p1, unsigned char *syn_p2 )
	{
	if( n < 0 || n > bf_s1+bf_s2 )
		invoke_debug();
	return n > bf_s1 ? &syn_p2[n] : &syn_p1[n];
	}
#endif

/*
 * given the name of a syntax table, return a
 * pointer to it. If it does not exist, create it
 */
static struct syntax_table *locate_syntax_table
	(
	unsigned char *name, int create
	)
	{
	int i = 0;
	struct syntax_table *p;

	if( name == 0 || name[0] == 0 )
		return 0;
	while( i < number_of_syntax_tables )
		if( _str_cmp( syntax_table_names[i], name ) == 0 )
			return syntax_tables[ i ];
		else
			i++;
	if( number_of_syntax_tables >= MAX_SYNTAX_TABLES )
		{
		error (u_str("Too many syntax tables!"));
		return 0;
		}
	if( !create )
		{
		error( u_str("No such syntax tables as %s"), name );
		return 0;
		}

	p = malloc_struct( syntax_table );
	syntax_tables[ number_of_syntax_tables ] = p;
	for( i=0; i<256; i++ )
		{
		struct syntax_string *old_entry, **new_entry;

		p->s_table[i].s_kind = global_syntax_table.s_table[i].s_kind;
		old_entry = global_syntax_table.s_table[i].s_strings;
		new_entry = &p->s_table[i].s_strings;
		*new_entry = NULL;

		while( old_entry != NULL )
			{
			*new_entry = malloc_struct( syntax_string );
			**new_entry = *old_entry;
			old_entry = old_entry->s_next;
			new_entry = &(*new_entry)->s_next;
			}
		*new_entry = NULL;
		}
	syntax_table_names[ number_of_syntax_tables ] = p->s_name = savestr( name );
	number_of_syntax_tables++;

	return p;
	}

/*
 * select a named syntax table for this buffer
 * and turn on syntax mode if it or the global
 * syntax table is non-empty
 */
int use_syntax_table( void )
	{
	struct syntax_table *p;

	p = locate_syntax_table(
		getescword( syntax_table_names, u_str(": use-syntax-table ") ),
		1 );
	if( p == 0 )
		return 0;
	bf_cur->b_mode.md_syntax = bf_mode.md_syntax = p;
	bf_cur->b_syntax.syntax_valid = 0;
	return 0;
	}


/*
 *	new style syntax table modification interface
 *
 *	This command can be called in one of the
 *	following ways:
 *
 *	(modify-syntax-table "word" <char-set>)
 *	(modify-syntax-table "dull" <char-set>)
 *	(modify-syntax-table "comment" <comment-start-string> <comment-end-string>)
 *	(modify-syntax-table "paren" <open-paren> <close-paren>)
 *	(modify-syntax-table "prefix" <char-set>)
 *	(modify-syntax-table "string" <char-set>)
 *	(modify-syntax-table "keyword-1" <string>)
 *	(modify-syntax-table "keyword-2" <string>)
 */
static unsigned char *syntax_type_names[] =
	{
	u_str("comment"),
	u_str("dull"),
	u_str("keyword-1"),
	u_str("keyword-2"),
	u_str("paren"),
	u_str("prefix"),
	u_str("string"),
	u_str("word"),
	NULL
	};
static int syntax_type_values[] =
	{
	SYNTAX_COMMENT,
	SYNTAX_DULL,
	SYNTAX_KEYWORD1,
	SYNTAX_KEYWORD2,
	SYNTAX_BEGIN_PAREN,
	SYNTAX_PREFIX_QUOTE,
	SYNTAX_STRING,
	SYNTAX_WORD
	};

static unsigned char *modify_syntax_table_prompt_str1[] =
	{
	u_str(" (comment-begin) "),
	u_str(" (character-set) "),
	u_str(" (keyword) "),
	u_str(" (keyword) "),
	u_str(" (open-paren) "),
	u_str(" (character-set) "),
	u_str(" (character-set) "),
	u_str(" (character-set) ")
	};
static unsigned char *modify_syntax_table_prompt_str2[] =
	{
	u_str(" (comment-end) "),
	u_str(""),
	u_str(""),
	u_str(""),
	u_str(" (close-paren) "),
	u_str(""),
	u_str(""),
	u_str("")
	};

unsigned char *modify_syntax_table_prompt =
	u_str(": modify-syntax-table %s%s%s");

int modify_syntax_table( void )
	{
	unsigned char *type_name;
	unsigned char *str1=NULL, *str2=NULL;
	int i;

	type_name = getescword( syntax_type_names, modify_syntax_table_prompt, "(type) ","", "" );
	if( type_name == NULL || type_name[0] == '\0' )
		return 0;

	for( i=0; syntax_type_names[i] != NULL; i++ )
		if( _str_cmp( type_name, syntax_type_names[i] ) == 0 )
			break;
	if( syntax_type_names[i] == NULL )
		{
		error(u_str("modify-syntax-table - unknown type of %s"), type_name);
		return 0;
		}
	str1 = getstr( modify_syntax_table_prompt,
			type_name, modify_syntax_table_prompt_str1[i], "" );
	if( str1 == NULL || str1[0] == '\0' )
		return 0;

	str1 = savestr( str1 );
	if( str1 == NULL )
		return 0;

	if( syntax_type_values[i] == SYNTAX_COMMENT
	|| syntax_type_values[i] == SYNTAX_BEGIN_PAREN )
		{
		str2 = getstr( modify_syntax_table_prompt,
			type_name, str1, modify_syntax_table_prompt_str2[i] );
		if( str2 == NULL || str2[0] == '\0' )
			{
			free( str1 );
			return 0;
			}
		}
	else
		str2 = u_str("");

	modify_syntax_table_inner( syntax_type_values[i], str1, str2 );

	free( str1 );
	return 0;
	}

static void modify_syntax_table_inner( int type, unsigned char *str1, unsigned char *str2 )
	{
	int ch = 0;
	struct syntax_string *syn_str = NULL;

	switch( type )
	{
	case SYNTAX_DULL:
		/*
		 * mark the chars are not interesting
		 * and remove the s_strings.
		 */
		while( *str1 != '\0' )
			{
			unsigned char c;
			int lim;

			c = *str1++;
			if( *str1 != '-' )
				lim = c;
			else
				if( *++str1 != '\0' )
					lim = *str1++;
				else
					goto syntax_error;
			for( ch=c; ch<=lim; ch++ )
				{
				struct syntax_string *cur, *next;

				/* this test prevents a recursive loop */
				if( bf_mode.md_syntax->s_table[ch].s_kind != SYNTAX_DULL )
					{
					bf_mode.md_syntax->s_table[ch].s_kind = SYNTAX_DULL;
				
					cur = bf_mode.md_syntax->s_table[ch].s_strings;
					bf_mode.md_syntax->s_table[ch].s_strings = NULL;

					while( cur != NULL )
						{
						next = cur->s_next;

						/* remove the other paren */
						if( cur->s_kind == SYNTAX_BEGIN_PAREN
						|| cur->s_kind == SYNTAX_END_PAREN )
							modify_syntax_table_inner( SYNTAX_DULL, cur->s_match_str, u_str("") );
				
						free( cur );
						cur = next;
						}
					}
				}
			}		
		break;

	case SYNTAX_WORD:
	case SYNTAX_PREFIX_QUOTE:
	case SYNTAX_STRING:
		while( *str1 != '\0' )
			{
			unsigned char c;
			int lim;

			c = *str1++;
			if( *str1 != '-' )
				lim = c;
			else
				if( *++str1 != '\0' )
					lim = *str1++;
				else
					goto syntax_error;
			for( ch=c; ch<=lim; ch++ )
				bf_mode.md_syntax->s_table[ch].s_kind |= type;
			}		
		break;

	case SYNTAX_BEGIN_PAREN:
		/* Need a end paren entry to match up with the begin paren */
		modify_syntax_table_inner( SYNTAX_END_PAREN, str2, str1 );

	/* all the following types end up building the same syntax_strings entry */
	case SYNTAX_END_PAREN:
	case SYNTAX_KEYWORD1:
	case SYNTAX_KEYWORD2:
	case SYNTAX_COMMENT:
		if( _str_len( str1 ) > (sizeof( syn_str->s_main_str )-1)
		|| _str_len( str2 ) > (sizeof( syn_str->s_match_str )-1) )
			goto syntax_error;

		syn_str = malloc_struct( syntax_string );
		if( syn_str == NULL )
			goto memory_error;

		syn_str->s_kind = type;
		syn_str->s_next = NULL;
		syn_str->s_main_len = _str_len( str1 );
		_str_cpy( syn_str->s_main_str, str1 );
		syn_str->s_match_len = _str_len( str2 );
		_str_cpy( syn_str->s_match_str, str2 );

		ch = str1[0];
		bf_mode.md_syntax->s_table[ch].s_kind |= type;

		/* insert this entry into the s_strings list */
		{
		struct syntax_string **ptr, *cur;

		ptr = &bf_mode.md_syntax->s_table[ch].s_strings;
		for(;;)
			{
			cur = *ptr;
		
			/* if we at the end of list just add here */
			if( cur == NULL )
				{
				*ptr = syn_str;
				break;
				}

			/* if this is the same type as a prevous entry replace it */
			if( cur->s_kind == syn_str->s_kind
			&& _str_cmp( cur->s_main_str, syn_str->s_main_str ) == 0 )
				{
				*ptr = syn_str;
				syn_str->s_next = cur->s_next;
				free( cur );
				break;
				}
			ptr = &cur->s_next;
			}
		}
		break;
	}
	return;

memory_error:
	error(u_str("modify-syntax-table - out of memory!"));
	return;
syntax_error:
	error(u_str("modify-syntax-table - syntax error"));
	return;
	}

/* old style syntax table modification interface */
int modify_syntax_entry( void )
	{
	unsigned char *p, str1[3], str2[3];

	if( (p = getstr( u_str(": modify-syntax-entry "))) != 0 )
		{
		if( _str_len( p ) < 5 )
			goto syntax_error;

		switch( p[0] )
		{
		case ' ':
		case '-':	modify_syntax_table_inner( SYNTAX_DULL, &p[5], u_str("") );
				break;
		case 'w':	modify_syntax_table_inner( SYNTAX_WORD, &p[5], u_str("") );
				break;
		case '(':	str1[0] = p[5];	str2[0] = p[1];
				str1[1] = '\0'; str2[1] = '\0';
				modify_syntax_table_inner( SYNTAX_BEGIN_PAREN, str1, str2 );
				break;
		case ')':	str2[0] = p[5];	str1[0] = p[1];
				str2[1] = '\0'; str1[1] = '\0';
				modify_syntax_table_inner( SYNTAX_BEGIN_PAREN, str1, str2 );
				break;
		case '"':	str1[0] = p[5];
				str1[1] = '\0';
				modify_syntax_table_inner( SYNTAX_PAIRED_QUOTE, str1, u_str("") );
				break;
		case '\\':	str1[0] = p[5];
				str1[1] = '\0';
				modify_syntax_table_inner( SYNTAX_PREFIX_QUOTE, str1, u_str("") );
				break;
		default:
			goto syntax_error;
		}

		/* if this is comment... */
		if( p[2] == '{' || p[3] == '}' )
			{
			int i;

			/* start off with null begin and end comment strings */
			str1[0] = '\0';
			str2[0] = '\0';

			/*
			 * search for an existing comment entry to get the
			 * other string from. Note that the old style syntax
			 * table only works with a single comment defined
			 */
			for( i=0; i<256; i++ )
				if( char_is( i, SYNTAX_COMMENT ) )
					{
					struct syntax_string *cur;

					cur = bf_mode.md_syntax->s_table[i].s_strings;
					while( cur )
						{
						if( cur->s_kind == SYNTAX_COMMENT )
						if( cur->s_main_len <= 2
						&& cur->s_match_len <= 2 )
							{
							_str_cpy( str1, cur->s_main_str );
							_str_cpy( str2, cur->s_match_str );
							}

						cur = cur->s_next;
						}
					break;
					}
			if( p[2] == '{' )
				{
				str1[0] = p[5];
				str1[1] = p[4];
				str1[2] = '\0';
				modify_syntax_table_inner( SYNTAX_COMMENT, str1, str2 );
				}
			else
				{
				str2[0] = p[5];
				str2[1] = p[4];
				str2[2] = '\0';
				modify_syntax_table_inner( SYNTAX_COMMENT, str1, str2 );
				}
			}
		}
	return 0;

syntax_error:
	error( u_str("Bogus modify-syntax-table directive.  [TP{}Cc]") );
	return 0;
	}

/*
 * Primitive function for paren matching. Leaves dot at enclosing left
 * paren, or at top of buffer if none. Stops at a zero-level newline if
 * StopAtNewline is set. Returns (to MLisp) 1 if it finds
 * a match, 0 if not
 * Bugs: does not correctly handle comments (it will never really handle them
 * correctly...
 */
static int paren_scan(int stop_at_newline, int forw)
	{
	int paren_level;
	unsigned char c;
	int p_c;
	unsigned char parenstack[200];
	unsigned int in_string = 0;
	unsigned char matching_quote = 0;
	struct syntax_table *s;
	unsigned int k;
	int on_on = 1;
	int start;

	paren_level = getnum( u_str("Paren Level: ") );
	s = bf_mode.md_syntax;
	start = (( forw ) ? (dot + 1) : dot);

	if( paren_level >= sizeof( parenstack ) )
		{
		error( u_str("paren level to be, limit is %d"),
				sizeof( parenstack ) );
		return 0;
		}

	memset( parenstack, 0, sizeof parenstack );

	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = 0;
	if( stop_at_newline )
		{
		int p1; int p2; int dp;

		p1 = dot - 1;
		if( forw )
			{
			p2 = num_characters;
			dp = 1;
			}
		else
			{
			p2 = first_character;
			dp = -1;
			}

		while( p1 != p2
		&&
			(
			(c = char_at( p1 )) == ' '
			|| c == '\t'
			|| c == '\n' ) )
			{
			set_dot( p1 + 1 );
			p1 = p1 + dp;
			}
		}

	if( bf_mode.md_syntax_array )
		{
		unsigned char *syn_p1 = NULL, *syn_p2 = NULL;

		/* brute force - calc the whole buffer */
		syntax_fill_in_array( num_characters );
		syn_p1 = bf_cur->b_syntax.syntax_base;
		syn_p2 = syn_p1 + bf_gap;

		while( on_on && ! err )
			{
			int sc;

			if( forw )
				{
				if( dot > num_characters )
					return 0;
				dot_right( 1 );
				}
			if( dot > 2 )
				p_c = char_at( dot - 2 );
			else
				{
				p_c = 0;
				if( dot <= first_character )
					return 0;
				}
			c = char_at( dot - 1 );
			k = s->s_table[c].s_kind;
			sc = syntax_at( dot - 1 );

			if( stop_at_newline && c == '\n' && paren_level == 0 )
				return 0;
			if( (sc&(SYNTAX_COMMENT|SYNTAX_STRING)) == 0
			&& (k&(SYNTAX_END_PAREN|SYNTAX_BEGIN_PAREN)) )
				{
				if( (forw == 0) == ((k & SYNTAX_END_PAREN) != 0) )
					{
					paren_level++;
					if( paren_level == sizeof( parenstack ) )
						error( u_str("Too many unmatched parenthesis") );
					else
						{
						struct syntax_string *cur = s->s_table[c].s_strings;

						while( cur != NULL )
							{
							if( cur->s_kind & (SYNTAX_BEGIN_PAREN|SYNTAX_END_PAREN)
							&& cur->s_match_len == 1 )
								{
								parenstack[ paren_level ] = cur->s_match_str[0];
								break;
								}
							cur = cur->s_next;
							}
						}
					}
				else
					{
					if( paren_level > 0
					&& parenstack[ paren_level ] != 0
					&& parenstack[ paren_level ] != c )
						error( u_str("Parenthesis mismatch.") );
					paren_level--;
					}

				if( paren_level < 0
				|| (paren_level == 0 && ! stop_at_newline) )
					on_on = 0;
				}
			if( ! forw )
				dot_left( 1 );
			}
		}
	else
		{
		while( on_on && ! err )
			{
			if( forw )
				{
				if( dot > num_characters )
					return 0;
				dot_right( 1 );
				}
			if( dot > 2 )
				p_c = char_at( dot - 2 );
			else
				{
				p_c = 0;
				if( dot <= first_character )
					return 0;
				}
			c = char_at( dot - 1 );
			k = s->s_table[c].s_kind;
			if( s->s_table[p_c].s_kind&SYNTAX_PREFIX_QUOTE )
				k = SYNTAX_WORD;
			if( ((! in_string) || (c == matching_quote))
			&& (k&SYNTAX_PAIRED_QUOTE) )
				{
				in_string = ! in_string;
				matching_quote = c;
				}

			if( stop_at_newline && c == '\n' && paren_level == 0 )
				return 0;
			if( ! in_string
			&& (k&(SYNTAX_END_PAREN|SYNTAX_BEGIN_PAREN)) )
				{
				if( (forw == 0) == ((k & SYNTAX_END_PAREN) != 0) )
					{
					paren_level++;
					if( paren_level == sizeof( parenstack ) )
						error( u_str("Too many unmatched parenthesis") );
					else
						{
						struct syntax_string *cur = s->s_table[c].s_strings;

						while( cur != NULL )
							{
							if( cur->s_kind & (SYNTAX_BEGIN_PAREN|SYNTAX_END_PAREN)
							&& cur->s_match_len == 1 )
								{
								parenstack[ paren_level ] = cur->s_match_str[0];
								break;
								}
							cur = cur->s_next;
							}
						}
					}
				else
					{
					if( paren_level > 0
					&& parenstack[ paren_level ] != 0
					&& parenstack[ paren_level ] != c )
						error( u_str("Parenthesis mismatch.") );
					paren_level--;
					}

				if( paren_level < 0
				|| (paren_level == 0 && ! stop_at_newline) )
					on_on = 0;
				}
			if( ! forw )
				dot_left( 1 );
			}
		}

	ml_value->exp_int = 1;

	return 0;
	}

/*
 * Primitive function for lisp indenting. Searches backward till it finds
 * the matching left paren, or a line that begins with zero paren-balance.
 * Returns the paren level at termination to mlisp.
 */
int backward_paren_bl( void )
	{
	return paren_scan( 1, 0 );
	}

/* Searches backward until it find the matching left paren */
int backward_paren( void )
	{
	return paren_scan( 0, 0 );
	}

int forward_paren_bl( void )
	{
	return paren_scan( 1, 1 );
	}

/* Searches forward until it find the matching left paren */
int forward_paren( void )
	{
	return paren_scan( 0, 1 );
	}

/* Function to dump syntax table to buffer in human-readable format */
int dump_syntax_table( void )
	{
	struct syntax_table *p;
	int i;
	int j;
	struct emacs_buffer *old;
	unsigned char line[100];
	unsigned char line2[50];
	unsigned char line3[50];

	old = bf_cur;

	p = locate_syntax_table(
		getescword( syntax_table_names, u_str(": dump-syntax-table ") ),
		0 );
	if( p == 0 )
		return 0;
	scratch_bfn( u_str("Syntax table"), interactive );
	sprintfl( line, sizeof( line ), u_str("Syntax table: %s\n"), p->s_name );
	ins_str( line );
	ins_str( u_str("Character	Syntatic Characteristics\n") );
	ins_str( u_str("----------------------------------------\n") );

	i = 0;
	while( i < 256 )
		{
		struct syntax_string *cur;

		j = i;
		while( j < 255
		&& p->s_table[i].s_kind == p->s_table[j+1].s_kind
		&& p->s_table[i].s_strings == NULL
		&& p->s_table[j+1].s_strings == NULL )
			j++;

		if( control_character( i ) )
			sprintfl( line2, 20, u_str("'\\%o'"), i );
		else
			sprintfl( line2, 20, u_str("'%c'"), i);
		if( i != j )
			if( control_character( j ) )
				sprintfl( line2 + _str_len( line2 ), 20, u_str("-'\\%o'"), j );
			else
				sprintfl( line2 + _str_len( line2 ), 20, u_str("-'%c'"), j );

		line3[0] = '\0';
		if( p->s_table[i].s_kind == SYNTAX_DULL )
			_str_cpy( line3, "Dull" );
		if( p->s_table[i].s_kind&SYNTAX_WORD )
			_str_cpy( line3, "Word" );
		if( p->s_table[i].s_kind&SYNTAX_BEGIN_PAREN )
			{
			if( line3[0] != '\0' ) _str_cat( line3, ", " );
			_str_cat(line3, "Open paren");
			}
		if( p->s_table[i].s_kind&SYNTAX_END_PAREN )
			{
			if( line3[0] != '\0' ) _str_cat( line3, ", " );
			_str_cat(line3, "Close paren");
			}
		if( p->s_table[i].s_kind&SYNTAX_COMMENT )
			{
			if( line3[0] != '\0' ) _str_cat( line3, ", " );
			_str_cat(line3, "Comment");
			}
		if( p->s_table[i].s_kind&SYNTAX_PAIRED_QUOTE )
			{
			if( line3[0] != '\0' ) _str_cat( line3, ", " );
			_str_cat(line3, "Paired quote");
			}
		if( p->s_table[i].s_kind&SYNTAX_PREFIX_QUOTE )
			{
			if( line3[0] != '\0' ) _str_cat( line3, ", " );
			_str_cat(line3, "Prefix quote");
			}

		sprintfl
		(
		line, sizeof( line ) - 1,
		u_str("%13s   %s\n"),
		line2, line3
		);
		ins_str( line );

		cur = p->s_table[i].s_strings;
		while( cur != NULL )
			{
			switch( cur->s_kind )
			{
			case SYNTAX_COMMENT:
				sprintfl
				(
				line3, sizeof( line3 ),
				u_str( "Comment between \"%s\" and \"%s\"" ),
				cur->s_main_str, cur->s_match_str
				);
				break;

			case SYNTAX_BEGIN_PAREN:
				sprintfl
				(
				line3, sizeof( line3 ),
				u_str( "Matched paren \"%s\" and \"%s\"" ),
				cur->s_main_str, cur->s_match_str
				);
				break;

			case SYNTAX_END_PAREN:
				sprintfl
				(
				line3, sizeof( line3 ),
				u_str( "Matched paren \"%s\" and \"%s\"" ),
				cur->s_match_str, cur->s_main_str
				);
				break;

			case SYNTAX_KEYWORD1:
				sprintfl
				(
				line3, sizeof( line3 ),
				u_str( "Keyword type 1 \"%s\"" ),
				cur->s_main_str
				);
				break;

			case SYNTAX_KEYWORD2:
				sprintfl
				(
				line3, sizeof( line3 ),
				u_str( "Keyword type 2 \"%s\"" ),
				cur->s_main_str
				);
				break;
			default:
				line3[0] = '\0';
			}
			if( line3[0] != '\0' )
				{
				sprintfl
				(
				line, sizeof( line ) - 1,
				u_str("%13s   %s\n"),
				"", line3
				);
				ins_str( line );
				}
			cur = cur->s_next;
			}

		i = j + 1;
		}
	bf_cur->b_checkpointed = -1;
	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );

	return 0;
	}

int list_syntax_tables( void )
	{
	int i;
	struct emacs_buffer *old;

	old = bf_cur;

	scratch_bfn( u_str("Syntax Table list"), interactive );

	ins_str( u_str("  Name\n  ----\n") );
	for( i=0; i<number_of_syntax_tables; i++ )
		if( syntax_table_names[i] != NULL )
			{
			unsigned char buf[90];
			sprintfl( buf, sizeof( buf ),
				u_str("  %s\n"), syntax_table_names[i] );
			ins_str( buf );
			}

	bf_modified = 0;
	set_dot( 1 );
	set_bfp( old );
	window_on( bf_cur );

	return 0;
	}

GLOBAL SAVRES int default_syntax_array;
int check_syntax_array
	(
	int value,
	struct variablename *v
	)
	{
	if( !check_bool( value, v ) )
		return 0;

	/* force colouring off is array is turned off */
	if( !value )
		bf_mode.md_syntax_colouring = 0;

	return 1;
	}

GLOBAL SAVRES int default_syntax_colouring;
int check_syntax_colouring
	(
	int value,
	struct variablename *v
	)
	{
	if( !check_bool( value, v ) )
		return 0;

	/* force array on when colouring is requested */
	if( value )
		bf_mode.md_syntax_array = 1;

	return 1;
	}

unsigned char current_buffer_syntax[MLISP_HOOK_SIZE];
void fetch_current_syntax_table( struct expression *e )
	{
	_str_cpy( e->exp_v.v_string, bf_mode.md_syntax->s_name  );
	}

int check_current_syntax_table
	(
	unsigned char *value,
	struct variablename *PNOTUSED(v)
	)
	{
	if( _str_len( value ) == 0 )
		{
		error( u_str("Illegal syntax table name") );
		return 0;
		}
	else
		bf_cur->b_mode.md_syntax  = bf_mode.md_syntax =
			locate_syntax_table( value, 0 );
	return 1;
	}


int is_dull = 0;
int is_word = 1;
int is_str = 2;
int is_quote_character = 3;
int is_comment = 4;

int syntax_loc( void )
	{
	unsigned char c;
	unsigned char m;
	int i;

	if( bf_mode.md_syntax_array )
		{
		unsigned char *syn_p1 = NULL, *syn_p2 = NULL;

		syntax_fill_in_array( dot+1 );
		syn_p1 = bf_cur->b_syntax.syntax_base;
		syn_p2 = syn_p1 + bf_gap;

		i = syntax_at( dot );
		if( (i&SYNTAX_COMMENT) != 0 )
			return is_comment;
		if( (i&SYNTAX_STRING) != 0 )
			return is_str;
		if( (i&(SYNTAX_WORD|SYNTAX_KEYWORD1|SYNTAX_KEYWORD2)) != 0 )
			return is_word;
		return is_dull;
		}

	for( i=1; i<=dot - 1; i += 1 )
		{
		c = char_at( i );
		if( char_is( c, SYNTAX_PAIRED_QUOTE ) )
			{
			i++;
			m = c;
			while( i < dot )
				{
				c = char_at( i );
				if( char_is( c, SYNTAX_PREFIX_QUOTE ) )
					i++;
				else
					if( char_is( c, SYNTAX_PAIRED_QUOTE )
					&& c == m )
						break;
				i++;
				}
			if( i >= dot )
				return is_str;
			}
		else
		if( char_is( c, SYNTAX_PREFIX_QUOTE ) )
			{
			i++;
			if( i >= dot )
				return is_quote_character;
			}
		else
		if( char_is( c, SYNTAX_COMMENT ) )
			{
			struct syntax_string *cur = bf_mode.md_syntax->s_table[c].s_strings;
			while( cur != NULL )
				{
				if( cur->s_kind == SYNTAX_COMMENT
				&& syntax_looking_at( i, cur->s_main_len, cur->s_main_str ) )
					{
					i = scan_comment( i, cur );
					if( i >= dot )
						return is_comment;
					break;
					}
				cur = cur->s_next;
				}
			}
		}
	if( dot <= first_character || dot > num_characters )
		return is_dull;
	return
		char_is( char_at( dot ), SYNTAX_WORD )
		&& char_is( char_at( dot - 1 ), SYNTAX_WORD ) ? is_word : is_dull;
	}

static int scan_comment( int i, struct syntax_string *comment )
	{
	unsigned char c;

	while( i < dot )
		{
		c = char_at( i );
		if( c == comment->s_match_str[0]
		&& syntax_looking_at( i, comment->s_match_len, comment->s_match_str ) )
			return i+1;

		i++;
		}
	return i;
	}

/*
 *	return true if the string is as pos in the buffer
 */
static int syntax_looking_at( int pos, int len, unsigned char *str )
	{
	int i;

	/* see if there is room for the string to fit in the buffer */
	if( pos+len-1 > bf_s1+bf_s2 )
		return 0;

	/*
	 * match last to first on the basis that doing that is
	 * cause a mismatch fastest
	 */
	len--;
	for( i=len, pos+=len; i>=0; i--, pos-- )
		if( char_at( pos ) != str[i] )
			return 0;

	/* matched */
	return 1;
	}

int current_syntax_entity( void )
	{
	ml_value->exp_type = ISINTEGER;
	ml_value->exp_int = syntax_loc();
	ml_value->exp_release = 0;

	return 0;
	}

void init_syntax( void )
	{
	int i;
	static struct syntax_table_entry dull_c =
		{
		SYNTAX_DULL, NULL
		};
	static struct syntax_table_entry word_c =
		{
		SYNTAX_WORD, NULL
		};

	syntax_tables[0] = &global_syntax_table;
	syntax_table_names[0] = global_syntax_table.s_name =
					u_str("global-syntax-table");
	number_of_syntax_tables = 1;
	for( i=0; i<=255; i++ )
		global_syntax_table.s_table[i] =
			isalnum( i ) ? word_c : dull_c;
	}

/*
 *	The following two routines are passed enough information to
 *	allow for an incremental update of the syntax array.
 *
 *	Initial we just update the syntax valid indicator.
 */
void syntax_insert_update( int dot, int len )
	{
	struct syntax_buffer_data *s = &bf_cur->b_syntax;

	if( dot < s->syntax_valid )
		{
		if( s->syntax_update_credit > 0 )
			{
			s->syntax_update_credit--;
			s->syntax_valid += len;
			syntax_update_buffer( dot, len );
			}
		else
			{
			cant_1line_opt = 1;
			s->syntax_valid = dot;
			}
		}
	else
		cant_1line_opt = 1;

	if( dot < bf_cur->b_line_valid )
		bf_cur->b_line_valid = dot;
	}

void syntax_delete_update( int dot, int len )
	{
	struct syntax_buffer_data *s = &bf_cur->b_syntax;

	if( dot < s->syntax_valid )
		{
		if( s->syntax_update_credit > 0 )
			{
			s->syntax_update_credit--;
			s->syntax_valid -= len;
			syntax_update_buffer( dot, 0 );
			}
		else
			{
			cant_1line_opt = 1;
			s->syntax_valid = dot;
			}
		}
	else
		cant_1line_opt = 1;

	if( dot < bf_cur->b_line_valid )
		bf_cur->b_line_valid = dot;
	}

/*
 *	Fill in the syntax array as far as the 'required' offset
 */
enum syn_states
	{
	st_simple, st_string, st_comment
	};

void syntax_fill_in_array( int required )
	{
	struct syntax_buffer_data *s = &bf_cur->b_syntax;
	struct syntax_string *comment = NULL;
	enum syn_states state = st_simple;
	unsigned char *syn_p1, *syn_p2;
	int pos;
	struct syntax_table_entry *string_entry = NULL;

	if( !bf_mode.md_syntax_array )
		return;

	s->syntax_update_credit = 4;

	if( s->syntax_base == NULL )
		{
		s->syntax_base = malloc_ustr( bf_cur->b_size + 1 );
		if( s->syntax_base == NULL )
			return;
		/* must have a 0 at the start of the array */
		s->syntax_base[0] = 0;
		}

	/* emacs_check_malloc_block( bf_cur->b_syntax_base ); */

	if( required > bf_s1+bf_s2 )
		required = bf_s1 + bf_s2;
	if( required <= s->syntax_valid )
		return;

	syn_p1 = s->syntax_base;
	syn_p2 = syn_p1 + bf_gap;

	/*
	 * backup to a dull character
	 * as that ensures that we can
	 * figure out the rest of the syntax
	 * unambiguosly
	 */
	for( pos=s->syntax_valid-1-SYNTAX_STRING_SIZE; pos > 1; pos-- )
		{
		if( syntax_at(pos) == SYNTAX_DULL )
			break;
		}

	if( pos < 1 )
		pos = 1;

	for( ; pos<=required; pos++ )
		{
		unsigned char c = char_at(pos);
		struct syntax_table_entry *entry = &bf_mode.md_syntax->s_table[c];

		/* emacs_check_malloc_block( bf_cur->b_syntax_base ); */

		switch( state )
		{
		case st_simple:
			if( entry->s_kind&SYNTAX_COMMENT )
				{
				comment = entry->s_strings;
				while( comment != NULL )
					{
					if( comment->s_kind == SYNTAX_COMMENT
					&& syntax_looking_at( pos, comment->s_main_len, comment->s_main_str ) )
						{
						int i;

						state = st_comment;

						for( i=0; i<comment->s_main_len; i++, pos++ )
							*ref_syntax_at(pos) = SYNTAX_COMMENT;
						pos--;
						goto for_loop_end;
						}
					comment = comment->s_next;
					}
				}
			if( (syntax_at(pos-1)&SYNTAX_WORD) == 0
			&& entry->s_kind&(SYNTAX_KEYWORD1|SYNTAX_KEYWORD2) )
				{
				struct syntax_string *keyword = NULL;

				keyword = entry->s_strings;
				while( keyword != NULL )
					{
					if( keyword->s_kind&(SYNTAX_KEYWORD1|SYNTAX_KEYWORD2)
					&& !char_is( char_at(pos+keyword->s_main_len), SYNTAX_WORD )
					&& syntax_looking_at( pos, keyword->s_main_len, keyword->s_main_str ) )
						{
						int i;

						for( i=0; i<keyword->s_main_len; i++, pos++ )
							*ref_syntax_at(pos) = (unsigned char)keyword->s_kind;
						pos--;
						break;
						}
					keyword = keyword->s_next;
					}
				/* if we found a keyword then continue to the next char */
				if( keyword != NULL )
					goto for_loop_end;
				}
			
			if( entry->s_kind&SYNTAX_WORD )
				*ref_syntax_at(pos) = SYNTAX_WORD;
			else if( entry->s_kind&SYNTAX_PAIRED_QUOTE )
				{
				*ref_syntax_at(pos) = SYNTAX_STRING;
				state = st_string;
				string_entry = entry;
				}
			else
				*ref_syntax_at(pos) = SYNTAX_DULL;
			break;

		case st_string:
			*ref_syntax_at(pos) = SYNTAX_STRING;
			if( entry->s_kind == SYNTAX_PREFIX_QUOTE )
				{
				pos++;
				*ref_syntax_at(pos) = SYNTAX_STRING;
				}
			else if( entry == string_entry )
				state = st_simple;
			break;

		case st_comment:
			*ref_syntax_at(pos) = SYNTAX_COMMENT;
			if( c == comment->s_match_str[0]
			&& syntax_looking_at( pos, comment->s_match_len, comment->s_match_str ) )
				{
				int i;

				state = st_simple;

				for( i=0; i<comment->s_match_len; i++, pos++ )
					*ref_syntax_at(pos) = SYNTAX_COMMENT;
				pos--;
				}
			break;
		default:
			invoke_debug();
		}
		for_loop_end: ;
		}

	/* emacs_check_malloc_block( bf_cur->b_syntax_base ); */

	s->syntax_valid = required;
	}

static void syntax_update_buffer( int pos, int len )
	{
	struct syntax_buffer_data *s = &bf_cur->b_syntax;
	struct syntax_string *comment = NULL;
	enum syn_states state = st_simple;
	unsigned char *syn_p1, *syn_p2;
	struct syntax_table_entry *string_entry = NULL;
	int required = pos + len;

	if( !bf_mode.md_syntax_array
	|| s->syntax_base == NULL )
		return;

	syn_p1 = s->syntax_base;
	syn_p2 = syn_p1 + bf_gap;

	/*
	 * backup to a dull character
	 * as that ensures that we can
	 * figure out the rest of the syntax
	 * unambiguosly
	 */
	for( pos=pos-1-SYNTAX_STRING_SIZE; pos > 1; pos-- )
		{
		if( syntax_at(pos) == SYNTAX_DULL )
			break;
		}

	if( pos < 1 )
		pos = 1;

	if( s->syntax_valid > bf_s1+bf_s2 )
		s->syntax_valid = bf_s1+bf_s2;

	for( ; pos<=s->syntax_valid; pos++ )
		{
		unsigned char c = char_at(pos);
		struct syntax_table_entry *entry = &bf_mode.md_syntax->s_table[c];

		if( c == '\n' )
			cant_1line_opt = 1;

		/* emacs_check_malloc_block( bf_cur->b_syntax_base ); */

		switch( state )
		{
		case st_simple:
			/* done if processed all thats required */
			if( pos > required
			/*
			 * and the old syntax was not a special type
			 * that requires further work to validate
			 */
			&& (syntax_at( pos )&(SYNTAX_COMMENT|SYNTAX_STRING
					|SYNTAX_KEYWORD1|SYNTAX_KEYWORD2)) == 0 )
				return;
			if( entry->s_kind&SYNTAX_COMMENT )
				{
				comment = entry->s_strings;
				while( comment != NULL )
					{
					if( comment->s_kind == SYNTAX_COMMENT
					&& syntax_looking_at( pos, comment->s_main_len, comment->s_main_str ) )
						{
						int i;

						state = st_comment;

						for( i=0; i<comment->s_main_len; i++, pos++ )
							*ref_syntax_at(pos) = SYNTAX_COMMENT;
						pos--;
						goto for_loop_end;
						}
					comment = comment->s_next;
					}
				}
			if( (syntax_at(pos-1)&SYNTAX_WORD) == 0
			&& entry->s_kind&(SYNTAX_KEYWORD1|SYNTAX_KEYWORD2) )
				{
				struct syntax_string *keyword = NULL;

				keyword = entry->s_strings;
				while( keyword != NULL )
					{
					if( keyword->s_kind&(SYNTAX_KEYWORD1|SYNTAX_KEYWORD2)
					&& !char_is( char_at(pos+keyword->s_main_len), SYNTAX_WORD )
					&& syntax_looking_at( pos, keyword->s_main_len, keyword->s_main_str ) )
						{
						int i;

						for( i=0; i<keyword->s_main_len; i++, pos++ )
							*ref_syntax_at(pos) = (unsigned char)keyword->s_kind;
						pos--;
						break;
						}
					keyword = keyword->s_next;
					}
				/* if we found a keyword then continue to the next char */
				if( keyword != NULL )
					goto for_loop_end;
				}
			
			if( entry->s_kind&SYNTAX_WORD )
				*ref_syntax_at(pos) = SYNTAX_WORD;
			else if( entry->s_kind&SYNTAX_PAIRED_QUOTE )
				{
				*ref_syntax_at(pos) = SYNTAX_STRING;
				state = st_string;
				string_entry = entry;
				}
			else
				*ref_syntax_at(pos) = SYNTAX_DULL;
			break;

		case st_string:
			*ref_syntax_at(pos) = SYNTAX_STRING;
			if( entry->s_kind == SYNTAX_PREFIX_QUOTE )
				{
				pos++;
				*ref_syntax_at(pos) = SYNTAX_STRING;
				}
			else if( entry == string_entry )
				state = st_simple;
			break;

		case st_comment:
			*ref_syntax_at(pos) = SYNTAX_COMMENT;
			if( c == comment->s_match_str[0]
			&& syntax_looking_at( pos, comment->s_match_len, comment->s_match_str ) )
				{
				int i;

				state = st_simple;

				for( i=0; i<comment->s_match_len; i++, pos++ )
					*ref_syntax_at(pos) = SYNTAX_COMMENT;
				pos--;
				}
			break;
		default:
			invoke_debug();
		}
		for_loop_end: ;
		}
	}
