/* 	Copyright(c ) 1982, 1983, 1984, 1985, 1986, 1987
 *		Barry A, Scott and Nick Emery
 */
#include <emacs.h>

#ifdef DB
#if !defined(DB_MLL)
#ifdef unix
#include <unistd.h>
#include <sys/ioctl.h>
#endif
#ifdef __ultrix
#define POSIX
#endif
#include <sys/stat.h>
#include <fcntl.h>

#if DBG_EXEC
void dbg_check_fildes( database *db, char *title )
	{
	int i;

	printf("\n\rDBG: check fildes - %s\n\r", title);
	printf("Database: %s\n\r", db->db_name ); 
	printf("db_dirf: %d  %s\n\r", db->db_dirf, db->dirnm );
	printf("db_pagf: %d  %s\n\r", db->db_pagf, db->pagnm );
	printf("db_datf: %d  %s\n\r", db->db_datf, db->datnm );

	printf("Fildes\tInode\tSize");
	for( i=1; i<20; i++ )
		{
		struct stat stat;
		int result = fstat( i, &stat );

		if( result == 0 )
			printf("%d\t%d\t%d\n\r", i, stat.st_ino, stat.st_size );
		else
			printf("%d\tclosed\t%d\n\r", i, errno );
		}
	}
#endif

database * lastdatabase;

static long getlong( unsigned char *p );
static void putlong( unsigned char *p, long val );
static int setup_db( database * db );
static void ndbm_access( long  hash, database * db );
static int getbit( database * db );
static int ndbm_setbit( database * db );
static int cmpdatum( datum d1, datum d2 );
static int delitem( unsigned char buf[PBLKSIZ], int n );
static int additem( unsigned char buf[PBLKSIZ], datum item );
static int chkblk( unsigned char buf[PBLKSIZ] );
static unsigned char *DefaultSpacefunc(int n, unsigned char *p);

#define clrbuf( cp, n ) memset( cp, 0, n )

static long getlong( unsigned char *p )
    {
    long    xp;

    *((unsigned char *) & xp) = *p++;
    *(1 + (unsigned char *) & xp) = *p++;
    *(2 + (unsigned char *) & xp) = *p++;
    *(3 + (unsigned char *) & xp) = *p++;
    return xp;
    }

static void putlong( unsigned char *p, long val )
    {
    *(unsigned char *) p++ = *((unsigned char *) & val);
    *(unsigned char *) p++ = *(1 + (unsigned char *) & val);
    *(unsigned char *) p++ = *(2 + (unsigned char *) & val);
    *(unsigned char *) p++ = *(3 + (unsigned char *) & val);
    }

database *open_db( unsigned char *file, int access )
    {
    struct stat statb;
    register database *db = malloc_struct(database);
    unsigned char buf[MAXPATHLEN+1];
#if DBG_EXEC && 0
	if( dbg_flags&DBG_EXEC )
		_dbg_msg(u_str("open_db( %s, %d )\n\r"), file, access );
#endif

    if( db == NULL )
	return NULL;

    db->db_name = savestr( file );
    expand_and_default( file, u_str(".dir"), buf );	db->dirnm = savestr( buf );
    expand_and_default( file, u_str(".pag"), buf );	db->pagnm = savestr( buf );
    expand_and_default( file, u_str(".dat"), buf );	db->datnm = savestr( buf );

    db->oldpagb = -1;
    db->olddirb = -1;
    db->db_rdonly = access != 0;
    db->db_dirf = -1;
    db->db_pagf = -1;
    db->db_datf = -1;
    if (setup_db(db) < 0)
	{
	free(db->dirnm);
	free(db->pagnm);
	free(db->datnm);
	free(db);
	return 0;
	}
    fstat(db->db_dirf, &statb);
    db->maxbno = statb.st_size * BYTESIZ - 1;
    return(db);
    }

void close_db (struct database *PNOTUSED(db))
    {
    /* cannot allow this code to be used until reopen_db is written */
#if 0
    if (db->db_file)
	fio_close(db->db_file);
    db->db_file = NULL;
#endif
    }

void free_db(database * db)
    {
    if (db == 0)
	return;
    if (lastdatabase == db)
	{
	if (db->db_dirf > 0)
	    close(db->db_dirf);
	if (db->db_pagf > 0)
	    close(db->db_pagf);
	if (db->db_datf > 0)
	    close(db->db_datf);
	db->db_dirf = -1;
	db->db_pagf = -1;
	db->db_datf = -1;
	lastdatabase = 0;
	}
    free(db->dirnm);
    free(db->pagnm);
    free(db->datnm);
    free(db);
    return;
}

static int setup_db( database * db )
    {
    if (lastdatabase == db)
	return 0;
    if (lastdatabase)
	{
	if (lastdatabase->db_dirf > 0)
	    close(lastdatabase->db_dirf);
	if (lastdatabase->db_pagf > 0)
	    close(lastdatabase->db_pagf);
	if (lastdatabase->db_datf > 0)
	    close(lastdatabase->db_datf);
	lastdatabase->db_reopen = 1;
	lastdatabase->db_dirf = -1;
	lastdatabase->db_pagf = -1;
	lastdatabase->db_datf = -1;
	lastdatabase = NULL;
	}
    if (db==NULL)
	return -1;

    if (db->db_rdonly)
	db->db_dirf = open(s_str(db->dirnm), O_BINARY|O_RDONLY);
    else
	{
	db->db_dirf = open(s_str(db->dirnm), O_BINARY|O_RDWR);
	if (db->db_dirf < 0)
	    {
	    db->db_rdonly = 1;
	    db->db_dirf = open(s_str(db->dirnm), O_BINARY|O_RDONLY);
	    }
	}
    db->db_pagf = open(s_str(db->pagnm), db->db_rdonly ? O_BINARY|O_RDONLY : O_BINARY|O_RDWR);
    db->db_datf = open(s_str(db->datnm), db->db_rdonly ? O_BINARY|O_RDONLY : O_BINARY|O_RDWR);
    if (db->db_dirf < 0 || db->db_pagf < 0 || db->db_datf < 0)
	{
	close(db->db_dirf);
	close(db->db_pagf);
	close(db->db_datf);
	return -1;
	}
#if defined( F_SETFD )
    fcntl(db->db_dirf, F_SETFD, 1 );
    fcntl(db->db_pagf, F_SETFD, 1 );
    fcntl(db->db_datf, F_SETFD, 1 );
#elif defined( FIOCLEX )
    ioctl(db->db_dirf, FIOCLEX, 0);
    ioctl(db->db_pagf, FIOCLEX, 0);
    ioctl(db->db_datf, FIOCLEX, 0);
#endif
    lastdatabase = db;
    lastdatabase->db_reopen = 0;
    return 0;
    }

long forder(datum key, database * db )
    {
    long    hash;

    hash = calchash(key);
    for (db->hmask = 0;; db->hmask = (db->hmask << 1) + 1)
	{
	db->blkno = hash & db->hmask;
	db->bitno = db->blkno + db->hmask;
	if (getbit(db) == 0)
	    break;
	}
    return db->blkno;
    }

datum fetch(datum key, database *db)
    {
    register    i;
    datum item;

#if DBG_EXEC && 0
	if( dbg_flags&DBG_EXEC )
		_dbg_msg(u_str("fetch( %s, %d )\n\r"), key, db->db_name );
#endif

    ndbm_access(calchash(key), db);
    for (i = 0;; i ++)
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    return(item);
	if (cmpdatum(key, item) == 0)
	    {
	    return item;
	    }
	}
    }

int delete_key( datum key, database * db )
    {
    register i;
    datum item;

    if (db->db_rdonly)
	return -1;
    ndbm_access(calchash(key), db);
    for (i = 0;; i ++)	
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    return -1;
	if (cmpdatum(key, item) == 0)	
	    {
	    delitem(db->pagbuf, i);
	    break;
	    }
	}
    setup_db(db);
    lseek(db->db_pagf, db->blkno * PBLKSIZ, 0);
    write(db->db_pagf, db->pagbuf, PBLKSIZ);
    return 0;
    }

int store(datum key, database *db)
    {
    register i;
    datum item;
    unsigned char ovfbuf[PBLKSIZ];

    if (setup_db(db) < 0) 
	return -1;
    if (db->db_rdonly)
	return - 1;
loop: 
    ndbm_access(calchash(key), db);
    for (i = 0;; i ++)
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    break;
	if (cmpdatum(key, item) == 0)
	    {
	    delitem(db->pagbuf, i);
	    break;
	    }
	}
    i = additem(db->pagbuf, key);
    if (i < 0)
	goto split;
    lseek(db->db_pagf, db->blkno * PBLKSIZ, 0);
    write(db->db_pagf, db->pagbuf, PBLKSIZ);
    return 0;

split: 
    if (key.dsize + 2*sizeof (long) + 2 * sizeof (short) >= PBLKSIZ)
	return -1;
    clrbuf(ovfbuf, PBLKSIZ);
    for (i = 0;;)
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    break;
	if (calchash(item) & (db->hmask + 1))
	    {
	    additem(ovfbuf, item);
	    delitem(db->pagbuf, i);
	    continue;
	    }
	i ++;
	}
    lseek(db->db_pagf, db->blkno * PBLKSIZ, 0);
    write(db->db_pagf, db->pagbuf, PBLKSIZ);
    lseek(db->db_pagf,(db->blkno + db->hmask + 1) * PBLKSIZ, 0);
    write(db->db_pagf, ovfbuf, PBLKSIZ);
    ndbm_setbit(db);
    goto loop;
    }

datum firstkey(database *db)
    {
    return firsthash(0L, db);
    }

datum nextkey(datum key, database *db)
    {
    int i;
    datum item, bitem;
    long hash;
    int f;

    /* keep compler quite */
    bitem.val1 = 0;

    hash = calchash(key);
    ndbm_access(hash, db);
    f = 1;
    for (i = 0;; i ++)
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    break;
	if (cmpdatum(key, item) <= 0)
	    continue;
	if (f || cmpdatum(bitem, item) < 0)
	    {
	    bitem = item;
	    f = 0;
	    }
	}
    if (f == 0)
	return bitem;
    hash = hashinc(hash, db);
    if (hash == 0)
	return item;
    return firsthash(hash, db);
}

datum firsthash( long hash, database * db )
    {
    register i;
    datum item, bitem;

loop: 
    ndbm_access(hash, db);
    bitem = makdatum(db->pagbuf, 0);
    for (i = 0;; i ++)
	{
	item = makdatum(db->pagbuf, i);
	if (item.dptr == 0)
	    break;
	if (cmpdatum(bitem, item) < 0)
	    bitem = item;
	}
    if (bitem.dptr != 0)
	return bitem;
    hash = hashinc(hash, db);
    if (hash == 0)
	return item;
    goto loop;
    }

static void ndbm_access( long  hash, database * db )
    {
    for (db->hmask = 0;; db->hmask = (db->hmask << 1) + 1)
	{
	db->blkno = hash & db->hmask;
	db->bitno = db->blkno + db->hmask;
	if (getbit(db) == 0)
	    break;
	}
    if (db->blkno != db->oldpagb)
	{
	clrbuf(db->pagbuf, PBLKSIZ);
	setup_db(db);
	lseek(db->db_pagf, db->blkno * PBLKSIZ, 0);
	read(db->db_pagf, db->pagbuf, PBLKSIZ);
	chkblk(db->pagbuf);
	db->oldpagb = db->blkno;
	}
    }

static int getbit( database * db )
    {
    long bn;
    long b, i, n;

    if (db->bitno > db->maxbno)
	return 0;
    n = db->bitno % BYTESIZ;
    bn = db->bitno / BYTESIZ;
    i = bn % DBLKSIZ;
    b = bn / DBLKSIZ;
    if (b != db->olddirb)
	{
	clrbuf(db->dirbuf, DBLKSIZ);
	setup_db(db);
	lseek(db->db_dirf, (long) b * DBLKSIZ, 0);
	read(db->db_dirf, db->dirbuf, DBLKSIZ);
	db->olddirb = b;
	}
    if (db->dirbuf[i] & (1 << n))
	return 1;
    return 0;
}

static int ndbm_setbit( database * db )
    {
    long bn;
    long i, n, b;

    if (db->db_rdonly)
	return -1;
    if (db->bitno > db->maxbno)
	{
	db->maxbno = db->bitno;
	getbit(db);
	}
    n = db->bitno % BYTESIZ;
    bn = db->bitno / BYTESIZ;
    i = bn % DBLKSIZ;
    b = bn / DBLKSIZ;
    db->dirbuf[i] |= 1 << n;
    setup_db(db);
    lseek(db->db_dirf,(long) b * DBLKSIZ, 0);
    write(db->db_dirf, db->dirbuf, DBLKSIZ);
    return 0;
    }

datum makdatum( unsigned char *buf, int n )
    {
    register short *sp;
    register t;
    unsigned char *lp;
    datum item;

    sp = (short *) buf;
    if (n < 0 || n >= sp[0])
	goto null;
    t = PBLKSIZ;
    if (n > 0)
	t = sp[n + 1 - 1];
    lp = buf + sp[n + 1];
    item.val1 = getlong(lp);
    lp += sizeof( long );
    item.val2 = getlong(lp);
    lp += sizeof( long );
    item.dptr = lp;
    item.dsize = t - sp[n + 1] - 2*sizeof(long);
    return (item);

null: 
    item.dptr = 0;
    item.dsize = 0;
    return (item);
    }

static int cmpdatum( datum d1, datum d2 )
    {
    register n;
    register unsigned char  *p1, *p2;

    n = d1.dsize;
    if (n != d2.dsize)
	return n - d2.dsize;
    if (n == 0)
	return 0;
    p1 = d1.dptr;
    p2 = d2.dptr;
    do
	if (*p1++ != *p2++)
	    return *--p1 - *--p2;
    while (--n);
    return 0;
    }

int  hitab[16] =
    {
    61, 57, 53, 49, 45, 41, 37, 33,
    29, 25, 21, 17, 13, 9, 5, 1,
    };
long hltab[64]=
    {
    06100151277L, 06106161736L, 06452611562L, 05001724107L,
    02614772546L, 04120731531L, 04665262210L, 07347467531L,
    06735253126L, 06042345173L, 03072226605L, 01464164730L,
    03247435524L, 07652510057L, 01546775256L, 05714532133L,
    06173260402L, 07517101630L, 02431460343L, 01743245566L,
    00261675137L, 02433103631L, 03421772437L, 04447707466L,
    04435620103L, 03757017115L, 03641531772L, 06767633246L,
    02673230344L, 00260612216L, 04133454451L, 00615531516L,
    06137717526L, 02574116560L, 02304023373L, 07061702261L,
    05153031405L, 05322056705L, 07401116734L, 06552375715L,
    06165233473L, 05311063631L, 01212221723L, 01052267235L,
    06000615237L, 01075222665L, 06330216006L, 04402355630L,
    01451177262L, 02000133436L, 06025467062L, 07121076461L,
    03123433522L, 01010635225L, 01716177066L, 05161746527L,
    01736635071L, 06243505026L, 03637211610L, 01756474365L,
    04723077174L, 03642763134L, 05750130273L, 03655541561L,
    };

long hashinc( long hash, database * db )
    {
    long    bit;

    hash &= db->hmask;
    bit = db->hmask + 1;
    for (;;)
	{
	bit >>= 1;
	if (bit == 0)
	    return 0L;
	if ((hash & bit) == 0)
	    return hash | bit;
	hash &= ~bit;
	}
    }

long calchash( datum item )
    {
    register i, j, f;
    long hashl;
    int hashi;

    hashl = 0;
    hashi = 0;
    for (i = 0; i < item.dsize; i++)
	{
	f = item.dptr[i];
	for (j = 0; j < BYTESIZ; j += 4)
	    {
	    hashi += hitab[f & 017];
	    hashl += hltab[hashi & 63];
	    f >>= 4;
	    }
	}
    return hashl;
    }

static int delitem( unsigned char buf[PBLKSIZ], int n )
    {
    register short *sp;
    register i1, i2, i3;

    sp = (short *) buf;
    if (n < 0 || n >= sp[0])
	goto bad;
    i1 = sp[n + 1];
    i2 = PBLKSIZ;
    if (n > 0)
	i2 = sp[n + 1 - 1];
    i3 = sp[sp[0] + 1 - 1];
    if (i2 > i1)
	while (i1 > i3)
	    {
	    i1--;
	    i2--;
	    buf[i2] = buf[i1];
	    buf[i1] = 0;
	    }
    i2 -= i1;
    for (i1 = n + 1; i1 < sp[0]; i1++)
	sp[i1 + 1 - 1] = (short)(sp[i1 + 1] + i2);
    sp[0]--;
    sp[sp[0] + 1] = 0;
    return 0;

bad: 
    return -1;
    }

static int additem( unsigned char buf[PBLKSIZ], datum item )
    {
    register short *sp;
    register unsigned char *p;
    register i1, i2;

    sp = (short *) buf;
    i1 = PBLKSIZ;
    if (sp[0] > 0)
	i1 = sp[sp[0] + 1 - 1];
    i1 -= item.dsize + 2*sizeof(long);
    i2 = (sp[0] + 2) * sizeof (short);
    if (i1 <= i2)
	return -1;
    sp[sp[0] + 1] = (short)i1;
    p = &buf[i1];
    putlong(p, item.val1);
    p += sizeof(long);
    putlong(p, item.val2);
    p += sizeof(long);
    for (i2 = 0; i2 < item.dsize; i2++)
	{
	*p++ = item.dptr[i2];
	}
    sp[0]++;
    return sp[0] - 1;
    }

static int chkblk( unsigned char buf[PBLKSIZ] )
    {
    register short *sp;
    register int t, i;

    sp = (short *) buf;
    t = PBLKSIZ;
    for (i = 0; i < sp[0]; i++)
	{
	if (sp[i + 1] > t)
	    goto bad;
	t = sp[i + 1];
	}
    if (t < (int)((sp[0] + 1) * sizeof (short)))
	goto bad;
    return 0;

bad: 
    clrbuf(buf, PBLKSIZ);
    return -1;
    }

int put_db(unsigned char *key, int keylen, unsigned char *content, int contentlen, database *db)
    {
    datum keyd, value;
    keyd.dptr = key;
    keyd.dsize = keylen;
    value = fetch(keyd, db);
    keyd.val2 = contentlen;
    setup_db(db);
    keyd.val1 = value.dptr && value.val2 >= contentlen
	? lseek(db->db_datf, value.val1, 0)
	: lseek(db->db_datf, 0, 2);
    if (store(keyd, db) < 0)
	return -1;
    write(db->db_datf, content, contentlen);
    return 0;
    }

static unsigned char *DefaultSpacefunc(int PNOTUSED(n), unsigned char *PNOTUSED(p))
    {
    return NULL;
    }

/*
 *	return
 *		-1 failed
 *		0 success
 *		1 success user must free content buffer
 */
int get_db
  (
  unsigned char *key,
  int keylen,
  unsigned char **content,
  int *contentlen,
  unsigned char *(*spacefunc)( int, unsigned char * ),
  register database *db
  )
    {
    datum value;
    unsigned char *p;
    int read_size;
    off_t lseek_val;

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
	dbg_check_fildes( db, "start of get_db" );
#endif

    if (content==0 || contentlen==0) return 1;
    if (spacefunc == NULL)
	spacefunc = DefaultSpacefunc;
    value.dptr = key;
    value.dsize = keylen;
    value = fetch(value, db);
    if (value.dptr == 0)
	return - 1;
    *contentlen = (int)value.val2;
    *content = malloc_ustr( *contentlen );
    setup_db(db);
    lseek_val = lseek(db->db_datf, value.val1, 0);
    read_size = read(db->db_datf, *content, *contentlen);

#if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
	{
	dbg_check_fildes( db,"after read in get_db" );
	printf("lseek = %d, read = %d\n\r", lseek_val, read_size );
	}
#endif

#ifdef __osf__
    /* assume this means that a bug in OSF/1 read code */
    if( read_size == 0 )
	{
	setup_db( NULL );
	setup_db( db );	

	memset( *content, 0, *contentlen );

	/* repeat the read we want to do */
	lseek_val = lseek(db->db_datf, value.val1, 0);
	read_size = read(db->db_datf, *content, *contentlen);
	}
#endif

    if (read_size != *contentlen)
	return -1;
    p = spacefunc (*contentlen, *content);
    if (p != NULL)
	{
	memcpy (p, *content, *contentlen);
	free( *content );
	return 0;
	}
    return 1;
    }

int get_db_help (unsigned char *PNOTUSED(key_spec), unsigned char *(*PNOTUSED(ins))(int, unsigned char *), struct database *PNOTUSED(db))
    {
    return 0;
    }

int index_db (unsigned char *match_string, int (*helper)(int, unsigned char *, unsigned char **), struct database *db)
    {
    datum key;
    unsigned char *str = NULL;
    int len = 0;

    for (key = firstkey (db); key.dptr != 0; key = nextkey (key, db))
	{
	if (key.dsize > len)
	    {
	    if (str)
		free (str);
	    if ((str = malloc_ustr(len = max (key.dsize + 1, 20))) == NULL)
		return 0;
	    }
	memcpy (str, key.dptr, key.dsize);
	str[key.dsize] = '\0';
	if(match_wild (key.dptr, match_string))
	    if(!helper (key.dsize, key.dptr, NULL))
		return 0;
	}
    return 1;
    }

#else

struct database *open_db( unsigned char *, int );
void close_db (struct database *);
int put_db( unsigned char *, int, unsigned char *, int, struct database *);
int get_db( unsigned char *key, int keylen, unsigned char **cont, int *contlen, unsigned char * (*ins)(int,unsigned char *), struct database *db );
int get_db_help( unsigned char *key_spec, unsigned char * (*ins)(int, unsigned char *), struct database *db );
void free_db(struct database *db);
int index_db( unsigned char *match_string, int (*helper)( int , unsigned char *, unsigned char * * ), struct database *db );

struct database *open_db( unsigned char *filename, int access )
	{
	struct database *db = NULL;
	FILE *index = NULL;
	int i;

#if DBG_EXEC
	if( dbg_flags&DBG_EXEC )
		_dbg_msg(u_str("open_db( %s, %d )\n\r"), filename, access );
#endif

	/* alloc the basic database struct */
	db = malloc_struct( database );
	if( db == NULL )
		return NULL;
	memset( db, 0, sizeof( struct database ) );

	/* open up the database file */
	db->db_file = fio_open( filename, 0, u_str(".mll"), NULL );
	if( db->db_file == NULL )
		goto error_opening;

	/* save the database name */
	db->db_name = savestr( fio_getname( db->db_file ) );
	if( db->db_name == NULL )
		goto error_memory;

	/*
	 *	This algorithm takes into account the way that
	 *	some operating systems have extra characters in
	 *	a text file that the C RTL does not return.
	 *
	 *	Note that the length of an entry is not equal
	 *	to the difference between two fio_getpos results.
	 *	fio_getpos returns a number that is only usable
	 *	as input to fio_setpos.
	 */
	index = fio_open( u_str( ".mli" ), 0, db->db_name, NULL );
	if( index == NULL
	|| fio_modify_date( db->db_file ) > fio_modify_date( index ) )
		{
		int entry_length, line_length;
		unsigned char buf[1024];

		if( index != NULL )
			{
			fio_close( index );
			index = NULL;
			}

		/* build the index */
		entry_length = 0;
		while( fio_get_line( db->db_file, buf, sizeof(buf)-1 ) > 0 )
			{
			line_length = _str_len( buf );
			entry_length += line_length;
			if( buf[0] == '[' )
			{
			if( buf[line_length-2] == ']' )
				{
				struct database_entry *e;

				if( db->db_num_entries > 0 )
					{
					e = &db->db_entries[db->db_num_entries-1];
					e->dbe_length = (int)(entry_length - line_length);
					}

				entry_length = 0;

				buf[line_length-2] = '\0';
				db->db_num_entries++;
				db->db_entries = (struct database_entry *)realloc( db->db_entries,
					sizeof( struct database_entry ) * db->db_num_entries );
				if( db->db_entries == NULL )
					goto error_memory;

				e = &db->db_entries[db->db_num_entries-1];
				e->dbe_name = savestr( &buf[1] );
				if( e->dbe_name == NULL )
					goto error_memory;
				e->dbe_position = fio_getpos( db->db_file );
				}
			}
			}
		db->db_entries[db->db_num_entries-1].dbe_length = entry_length;

		/* write out the index */
		index = fio_create( u_str( ".mli" ), 0, 0, db->db_name, 0 );
		if( index == NULL )
			goto error_memory;

		for( i=0; i<db->db_num_entries; i++ )
			{
			struct database_entry *e = &db->db_entries[i];
			unsigned char buf[256];

			sprintfl( buf, sizeof( buf ),
				u_str("[%s %D %D]\n"),
					e->dbe_name, e->dbe_position, e->dbe_length );
			fio_put( index, buf, _str_len( buf ) );
			}

		fio_close( index );
		}
	else
		{
		unsigned char name[128];
		long int pos;
		int len;

		db->db_num_entries = 0;
		db->db_entries = NULL;

		/* read the index entries from the files */
		while( fscanf( index, " [%127s %ld %d]", &name, &pos, &len ) == 3 )
			{
			struct database_entry *e;
			db->db_num_entries++;
			db->db_entries = (struct database_entry *)realloc( db->db_entries,
				sizeof( struct database_entry ) * db->db_num_entries );
			if( db->db_entries == NULL )
				goto error_memory;

			e = &db->db_entries[ db->db_num_entries-1 ];
			e->dbe_name = savestr( name );
			if( e->dbe_name == NULL )
				goto error_memory;
			e->dbe_length = len;
			e->dbe_position = pos;
			}

		fio_close( index );
		}

	return db;
error_opening:
#if DBG_EXEC
	if( dbg_flags&DBG_EXEC )
		_dbg_msg(u_str("error opening db\n\r"));
#endif

error_memory:
	if( index )
		fio_close( index );
	if( db->db_name )
		free( db->db_name );
	if( db->db_entries )
		free( db->db_entries );
	if( db->db_data_pointer )
		free( db->db_data_pointer );
	if( db->db_file )
		fio_close( db->db_file );
	free( db );

	return NULL;
	}

void close_db (struct database *db)
	{
	/* cannot allow this code to be used until reopen_db is written */
#if 0
	if( db->db_file )
		fio_close( db->db_file );
	db->db_file = NULL;
#endif
	}

int put_db(
	unsigned char *key, int keylen,
	unsigned char *cont, int contlen,
	struct database *db
	)
	{
	return 1;
	}

int get_db
	(
	unsigned char *key, int keylen,
	unsigned char **cont, int *contlen,
	unsigned char * (*ins)(int,unsigned char *),
	struct database *db
	)
	{
	int i;
	unsigned char *p;

	for( i=0; i<db->db_num_entries; i++ )
	    if( _str_ncmp( key, db->db_entries[i].dbe_name, keylen ) == 0 )
		{
		fio_setpos( db->db_file, db->db_entries[i].dbe_position );
		*contlen = db->db_entries[i].dbe_length;
		*cont = malloc_ustr( *contlen );
		if( *cont == NULL )
			return 1;
		if( fio_get( db->db_file, *cont, *contlen ) <= 0 )
			return 1;

		p = ins( *contlen, *cont );
		if( p != NULL )
			memcpy( p, *cont, *contlen );

		return 0;
		}

	return 1;
	}

int get_db_help(
	unsigned char *key_spec,
	unsigned char *(*ins)(int, unsigned char *),
	struct database *db
	)
	{
	return 1;
	}

void free_db(struct database *db)
	{
	free( db->db_name );
	free( db->db_entries );
	if( db->db_data_pointer )
		free( db->db_data_pointer );
	if( db->db_file )
		fio_close( db->db_file );
	free( db );
	}

int index_db( unsigned char *match_string,
	int (*helper)( int , unsigned char *, unsigned char * * ),
	struct database *db )
	{
	int i;

	for( i=0; i<db->db_num_entries; i++ )
		{
		struct database_entry *e = &db->db_entries[i];

		if( match_wild( e->dbe_name, match_string ) )
			if( !helper( _str_len( e->dbe_name ), e->dbe_name, NULL ) )
				return 0;
		}

	return 1;
	}

#endif
#endif
