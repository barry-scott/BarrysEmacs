//
//     Copyright (c) 1982-1996
//        Barry A, Scott
//

#include <emacsutl.h>
#include <em_stat.h>
#include <stdlib.h>
#include <emobject.h>
#include <emstring.h>
#include <emstrtab.h>
#include <ndbm.h>


# undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

#ifdef DB
# ifdef __unix__
#  include <unistd.h>
#  include <sys/ioctl.h>
# endif
# if defined(_NT)
#  include <io.h>
# endif
# include <sys/stat.h>
# include <fcntl.h>

extern int expand_and_default( const EmacsString &nm, const EmacsString &def, EmacsString & buf );
extern int match_wild( const EmacsString &, const EmacsString & );
extern int dbg_flags;

# if DBG_EXEC
void database::dbg_check_fildes( const char *title )
{
    int i;

    printf("\n\rDBG: check fildes - %s\n\r", title );
    printf("Database: %s\n\r", db_name.sdata() );
    printf("db_dirf: %d  %s\n\r", db_dirf, dirnm.sdata() );
    printf("db_pagf: %d  %s\n\r", db_pagf, pagnm.sdata() );
    printf("db_datf: %d  %s\n\r", db_datf, datnm.sdata() );

    printf("Fildes\tInode\tSize\n" );
    for( i=1; i<20; i++ )
    {
        EmacsFileStat s;
        bool result = s.stat( i );

        if( result )
            printf("%d\t%ld\t%ld\n\r", i, (long)s.data().st_ino, (long)s.data().st_size );
        else
            printf("%d\tclosed\t%d\n\r", i, errno );
    }
}
# endif

database * database::lastdatabase;

# define clrbuf( cp, n ) memset( cp, 0, n )

class byteOrderIndependantInt
{
public:
    void set_LSB_InLowAddress() { byte_order = LSB_InLowAddress; }
    void set_LSB_InHighAddress() { byte_order = LSB_InHighAddress; }
    bool has_LSB_InLowAddress() { return byte_order == LSB_InLowAddress; }
    bool has_LSB_InHighAddress() { return byte_order == LSB_InHighAddress; }
protected:
    enum ByteOrder
    {
        LSB_InLowAddress,    // so called little endian 80x86, VAX, Alpha
        LSB_InHighAddress        // so called big endian HP-PA, 68K
    };
    byteOrderIndependantInt( int wantLSB_InLowAddress )
        : byte_order( wantLSB_InLowAddress ? LSB_InLowAddress : LSB_InHighAddress )
    { }
    ~byteOrderIndependantInt()
    { }

    ByteOrder byte_order;
};

class shortInt : public byteOrderIndependantInt
{
public:
    shortInt( bool LSB_InLowAddress, unsigned char *base, int index=0 );
    int get( int index=0 );
    int set( int value, int index=0 );
    int operator []( int index ) { return get( index ); }
private:
    unsigned char *sp;
};

shortInt::shortInt( bool LSB_InLowAddress, unsigned char *base, int index )
    : byteOrderIndependantInt( LSB_InLowAddress )
    , sp( base + index*sizeof(short) )
{ }

int shortInt::get(int index)
{
    unsigned char *p = sp + index*sizeof(short);

    if( has_LSB_InLowAddress() )
    {
        int value = int(p[0]) << 0;
        value |= int(p[1]) << 8;

        return (short)value;
    }
    else
    {
        int value = int(p[1]) << 0;
        value |= int(p[0]) << 8;

        return (short)value;
    }
}

int shortInt::set( int value, int index )
{
    unsigned char *p = sp + index*sizeof(short);

    if( has_LSB_InLowAddress() )
    {
        p[0] = (unsigned char)(value>>0);
        p[1] = (unsigned char)(value>>8);
    }
    else
    {
        p[1] = (unsigned char)(value>>0);
        p[0] = (unsigned char)(value>>8);
    }

    return (short)value;
}

class longInt : public byteOrderIndependantInt
{
public:
    longInt( bool LSB_InLowAddress, unsigned char *base, int index=0 );
    int get( int index=0 );
    int set( int value, int index=0 );
    int operator []( int index ) { return get( index ); }
private:
    unsigned char *lp;
};

longInt::longInt( bool LSB_InLowAddress, unsigned char *base, int index )
    : byteOrderIndependantInt( LSB_InLowAddress )
    , lp( base + index*sizeof(long) )
{ }

int longInt::get(int index)
{
    unsigned char *p = lp + index*sizeof(long);

    if( has_LSB_InLowAddress() )
    {
        long value = p[0] << 0;
        value |= p[1] << 8;
        value |= p[2] << 16;
        value |= p[3] << 24;

        return value;
    }
    else
    {
        long value = p[3] << 0;
        value |= p[2] << 8;
        value |= p[1] << 16;
        value |= p[0] << 24;

        return value;
    }
}

int longInt::set( int value, int index )
{
    unsigned char *p = lp + index*sizeof(long);

    if( has_LSB_InLowAddress() )
    {
        p[0] = (unsigned char)(value>>0);
        p[1] = (unsigned char)(value>>8);
        p[2] = (unsigned char)(value>>16);
        p[3] = (unsigned char)(value>>24);
    }
    else
    {
        p[3] = (unsigned char)(value>>0);
        p[2] = (unsigned char)(value>>8);
        p[1] = (unsigned char)(value>>16);
        p[0] = (unsigned char)(value>>24);
    }

    return (long)value;
}

bool database::open_db( const EmacsString &file, int access )
{
    EmacsFileStat statb;
# if DBG_EXEC && 0
    if( dbg_flags&DBG_EXEC )
        _dbg_msg( FormatString("open_db( %s, %d )" << file.sdata() << access );
# endif

    db_name = file;
    expand_and_default( file, ".dir", dirnm );
    expand_and_default( file, ".pag", pagnm );
    expand_and_default( file, ".dat", datnm );

# if DBG_EXEC && 0
    _dbg_msg( FormatString("open_db() dirnm %s") << dirnm );
    _dbg_msg( FormatString("open_db() pagnm %s") << pagnm );
    _dbg_msg( FormatString("open_db() datnm %s") << datnm );
# endif

    return reopen_db( access );
}

bool database::reopen_db( int access )
{
# if DBG_EXEC && 0
    if( dbg_flags&DBG_EXEC )
        _dbg_msg("reopen_db()");
# endif

    oldpagb = -1;
    olddirb = -1;
    db_rdonly = access != 0;
    db_dirf = -1;
    db_pagf = -1;
    db_datf = -1;

    if( setup_db() < 0 )
        return false;

    return true;
}

void database::close_db()
{
    if( lastdatabase == this )
        lastdatabase = NULL;

    if( db_dirf > 0 )
        close( db_dirf );
    if( db_pagf > 0 )
        close( db_pagf );
    if( db_datf > 0 )
        close( db_datf );
    db_reopen = 1;
    db_dirf = -1;
    db_pagf = -1;
    db_datf = -1;
}

database::database()
    : db_name()
    , dirnm()
    , datnm()
    , pagnm()
    , databaseUsesLSB_byteOrdering( true )
{ }

database::~database()
{
    if( lastdatabase == this )
    {
        if( db_dirf > 0 )
            close( db_dirf );
        if( db_pagf > 0 )
            close( db_pagf );
        if( db_datf > 0 )
            close( db_datf );
        db_dirf = -1;
        db_pagf = -1;
        db_datf = -1;
        lastdatabase = NULL;
    }
}

int database::setup_db()
{
    if( lastdatabase == this )
        return 0;

    if( lastdatabase != NULL )
        lastdatabase->close_db();

    if( db_rdonly )
        db_datf = open( datnm, O_BINARY|O_RDONLY );
    else
    {
        db_datf = open( datnm, O_BINARY|O_RDWR );
        if( db_datf < 0 )
        {
            db_rdonly = 1;
            db_datf = open( datnm, O_BINARY|O_RDONLY );
        }
    }
    if( db_datf < 0 )
        return -1;

    db_pagf = open( pagnm, db_rdonly ? O_BINARY|O_RDONLY : O_BINARY|O_RDWR );
    if( db_pagf < 0 )
    {
        close( db_datf );
        return -1;
    }

    db_dirf = open( dirnm, db_rdonly ? O_BINARY|O_RDONLY : O_BINARY|O_RDWR );
    if( db_dirf < 0 )
    {
        close( db_pagf );
        close( db_datf );
        return -1;
    }

    EmacsFileStat statb;
    statb.stat( db_dirf );
    maxbno = statb.data().st_size * BYTESIZ - 1;

# if defined( F_SETFD )
    fcntl( db_dirf, F_SETFD, 1 );
    fcntl( db_pagf, F_SETFD, 1 );
    fcntl( db_datf, F_SETFD, 1 );
#  elif defined( FIOCLEX )
    ioctl( db_dirf, FIOCLEX, 0 );
    ioctl( db_pagf, FIOCLEX, 0 );
    ioctl( db_datf, FIOCLEX, 0 );
# endif
    lastdatabase = this;
    db_reopen = 0;
    return 0;
}

long database::forder( datum &key )
{
    long    hash;

    hash = key.calchash();
    for( hmask = 0;; hmask = ( hmask << 1 ) + 1 )
    {
        blkno = hash & hmask;
        bitno = blkno + hmask;
        if( getbit() == 0 )
            break;
    }
    return blkno;
}

database::datum database::fetch( datum &key )
{
    int i;
# if DBG_EXEC && 0
    if( dbg_flags&DBG_EXEC )
        _dbg_msg("fetch( %d, %s )\n\r", key, db_name.sdata() );
# endif

    ndbm_access( key.calchash() );
    for( i = 0;; i ++ )
    {
        datum item( *this, pagbuf, i );

        if( item.dptr == 0 )
            return item;

        if( key.cmpdatum( item ) == 0 )
            return item;
    }
}

int database::delete_key( datum &key )
{
    int i;

    if( db_rdonly )
        return -1;
    ndbm_access( key.calchash() );
    for( i = 0;; i ++ )
    {
        datum item( *this, pagbuf, i );

        if( item.dptr == 0 )
            return -1;

        if( key.cmpdatum( item ) == 0 )
        {
            delitem( pagbuf, i );
            break;
        }
    }

    if( setup_db() < 0 )
        return -1;

    lseek( db_pagf, blkno * PBLKSIZ, 0 );
    write( db_pagf, pagbuf, PBLKSIZ );

    return 0;
}

int database::store( datum &key )
{
    int i;
    unsigned char ovfbuf[PBLKSIZ];

    if( setup_db() < 0 )
        return -1;
    if( db_rdonly )
        return - 1;
    for(;;)
    {
        ndbm_access( key.calchash() );
        for( i = 0;; i ++ )
        {
            datum item( *this, pagbuf, i );

            if( item.dptr == 0 )
                break;

            if( key.cmpdatum( item ) == 0 )
            {
                delitem( pagbuf, i );
                break;
            }
        }

        i = additem( pagbuf, key );
        if( i >= 0 )
        {
            lseek( db_pagf, blkno * PBLKSIZ, 0 );
            write( db_pagf, pagbuf, PBLKSIZ );
            return 0;
        }

        // split
        if( key.dsize + 2*sizeof( long ) + 2 * sizeof( short ) >= PBLKSIZ )
            return -1;

        clrbuf( ovfbuf, PBLKSIZ );

        for( i = 0;; )
        {
            datum item( *this, pagbuf, i );
            if( item.dptr == 0 )
                break;

            if( item.calchash() & (hmask + 1) )
            {
                additem( ovfbuf, item );
                delitem( pagbuf, i );
                continue;
            }
            i ++;
        }

        lseek( db_pagf, blkno * PBLKSIZ, 0 );
        write( db_pagf, pagbuf, PBLKSIZ );

        lseek( db_pagf,( blkno + hmask + 1 ) * PBLKSIZ, 0 );
        write( db_pagf, ovfbuf, PBLKSIZ );

        ndbm_setbit();
    }
}

database::datum database::firstkey()
{
    return firsthash( 0L );
}

database::datum database::nextkey( datum &key )
{
    int i;
    datum bitem(*this);
    datum item(*this);
    long hash;
    int f;

    hash = key.calchash();
    ndbm_access( hash );
    f = 1;

    for( i = 0;; i ++ )
    {
        item = datum( *this, pagbuf, i );
        if( item.dptr == 0 )
            break;
        if( key.cmpdatum( item ) <= 0 )
            continue;
        if( f || bitem.cmpdatum( item ) < 0 )
        {
            bitem = item;
            f = 0;
        }

    }
    if( f == 0 )
        return bitem;

    hash = hashinc( hash );
    if( hash == 0 )
        return item;

    return firsthash( hash );
}

database::datum database::firsthash( long hash )
{
    int i;
    datum item(*this);
    datum bitem(*this);

    for(;;)
    {
        ndbm_access( hash );
        bitem = datum( *this, pagbuf, 0 );
        for( i = 0;; i ++ )
        {
            item = datum( *this, pagbuf, i );
            if( item.dptr == 0 )
                break;
            if( bitem.cmpdatum( item ) < 0 )
                bitem = item;
        }
        if( bitem.dptr != 0 )
            return bitem;
        hash = hashinc( hash );
        if( hash == 0 )
            return item;
    }
}

void database::ndbm_access( long hash )
{
    for( hmask = 0;; hmask = (hmask << 1) + 1 )
    {
        blkno = hash & hmask;
        bitno = blkno + hmask;
        if( getbit() == 0 )
            break;
    }
    if( blkno != oldpagb )
    {
        clrbuf( pagbuf, PBLKSIZ );

        setup_db();

        lseek( db_pagf, blkno * PBLKSIZ, 0 );
        read( db_pagf, pagbuf, PBLKSIZ );
        chkblk( pagbuf );

        oldpagb = blkno;
    }
}

int database::getbit()
{
    long bn;
    long b, i, n;

    if( bitno > maxbno )
        return 0;
    n = bitno % BYTESIZ;
    bn = bitno / BYTESIZ;
    i = bn % DBLKSIZ;
    b = bn / DBLKSIZ;
    if( b != olddirb )
    {
        clrbuf( dirbuf, DBLKSIZ );

        setup_db();

        lseek( db_dirf, ( long ) b * DBLKSIZ, 0 );
        read( db_dirf, dirbuf, DBLKSIZ );

        olddirb = b;
    }

    if( dirbuf[i] & (1 << n) )
        return 1;

    return 0;
}

int database::ndbm_setbit()
{
    long bn;
    long i, n, b;

    if( db_rdonly )
        return -1;

    if( bitno > maxbno )
    {
        maxbno = bitno;
        getbit();
    }

    n = bitno % BYTESIZ;
    bn = bitno / BYTESIZ;
    i = bn % DBLKSIZ;
    b = bn / DBLKSIZ;
    dirbuf[i] |= 1 << n;

    setup_db();

    lseek( db_dirf,( long ) b * DBLKSIZ, 0 );
    write( db_dirf, dirbuf, DBLKSIZ );

    return 0;
}

database::datum::datum( database &_db )
    : dptr( NULL)
    , dsize( 0)
    , val1( 0)
    , val2( 0)
    , db(_db)
{ }

database::datum::datum( database &_db, const datum &d )
    : db( _db )
{
    operator=( d );
}

database::datum &database::datum::operator=( const datum &d )
{
    dptr = d.dptr;
    dsize = d.dsize;
    val1 = d.val1;
    val2 = d.val2;

    return *this;
}

database::datum::datum( database &_db, unsigned char *buf, int n )
    : db( _db )
{
    shortInt sp( db.databaseUsesLSB_byteOrdering, buf );

    if( n < 0 || n >= sp[0] )
    {
        dptr = 0;
        dsize = 0;
        return;
    }

    int t = PBLKSIZ;
    if( n > 0 )
        t = sp[n + 1 - 1];

    longInt l_i_p( db.databaseUsesLSB_byteOrdering, buf + sp[n + 1] );
    val1 = l_i_p.get(0);
    val2 = l_i_p.get(1);

    dptr = buf + sp[n + 1] + 2*sizeof( long );
    dsize = t - sp[n + 1] - 2*sizeof( long );
}

int database::datum::cmpdatum( const datum &d2 )
{
    int n;
    register const unsigned char  *p1, *p2;

    n = dsize;
    if( n != d2.dsize )
        return n - d2.dsize;
    if( n == 0 )
        return 0;
    p1 = dptr;
    p2 = d2.dptr;
    do
    {
        if( *p1++ != *p2++ )
            return *--p1 - *--p2;
        n--;
    }
    while( n > 0 );

    return 0;
}

static int hitab[16] =
{
    61, 57, 53, 49, 45, 41, 37, 33,
    29, 25, 21, 17, 13, 9, 5, 1,
};
static long hltab[64]=
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

long database::hashinc( long hash )
{
    long    bit;

    hash &= hmask;
    bit = hmask + 1;
    for( ;; )
    {
        bit >>= 1;
        if( bit == 0 )
            return 0L;
        if( ( hash & bit ) == 0 )
            return hash | bit;
        hash &= ~bit;
    }
}

long database::datum::calchash()
{
    long hashl = 0;
    int hashi = 0;

    for( int i = 0; i < dsize; i++ )
    {
        int f = dptr[i];
        for( int j = 0; j < BYTESIZ; j += 4 )
        {
            hashi += hitab[f & 017];
            hashl += hltab[hashi & 63];
            f >>= 4;
        }
    }
    return hashl;
}

int database::delitem( unsigned char buf[database::PBLKSIZ], int n )
{
    shortInt sp( databaseUsesLSB_byteOrdering, buf );

    if( n < 0 || n >= sp[0] )
        return -1;

    int i1 = sp[n + 1];
    int i2 = PBLKSIZ;
    if( n > 0 )
        i2 = sp[ n + 1 - 1 ];
    int i3 = sp[ sp[0] + 1 - 1 ];
    if( i2 > i1 )
        while( i1 > i3 )
        {
            i1--;
            i2--;
            buf[i2] = buf[i1];
            buf[i1] = 0;
        }
    i2 -= i1;
    for( i1 = n + 1; i1 < sp[0]; i1++ )
        sp.set( sp[i1 + 1] + i2, i1 + 1 - 1 );
    sp.set( sp[0]-1, 0 );
    sp.set( 0, sp[0] + 1 );

    return 0;
}

int database::additem( unsigned char buf[database::PBLKSIZ], datum &item )
{
    shortInt sp( databaseUsesLSB_byteOrdering, buf );

    int i1 = PBLKSIZ;
    if( sp[0] > 0 )
        i1 = sp[sp[0] + 1 - 1];

    i1 -= item.dsize + 2*sizeof( long );
    int i2 = ( sp[0] + 2 ) * sizeof( short );
    if( i1 <= i2 )
        return -1;

    sp.set( i1, sp[0] + 1 );

    longInt l_i_p( databaseUsesLSB_byteOrdering, &buf[i1] );
    l_i_p.set( (int)item.val1, 0 );
    l_i_p.set( (int)item.val2, 1 );

    unsigned char *p = &buf[ i1 + 2*sizeof( long ) ];
    for( i2 = 0; i2 < item.dsize; i2++ )
        *p++ = item.dptr[i2];

    sp.set( sp[0]+1, 0 );

    return sp[0] - 1;
}


static bool chkblkHelper( shortInt &sp )
{
    int t = database::PBLKSIZ;
    int i;
    for( i = 0; i < sp[0]; i++ )
    {
        if( sp[i + 1] > t )
            return false;
        t = sp[i + 1];
    }
    if( t < ((sp[0] + 1) * (int)sizeof( short )) )
        return false;

    return true;
}

int database::chkblk( unsigned char buf[database::PBLKSIZ] )
{
    databaseUsesLSB_byteOrdering = true;
    shortInt sp( databaseUsesLSB_byteOrdering, buf );


    if( chkblkHelper( sp ) )
        return 0;

    sp.set_LSB_InHighAddress();
    databaseUsesLSB_byteOrdering = false;

    if( chkblkHelper( sp ) )
        return 0;

    return -1;
}

int database::put_db( const EmacsString &key, unsigned char *content, int contentlen )
{
    datum keyd(*this);
    keyd.dptr = key.data();
    keyd.dsize = key.length();

    datum value( fetch( keyd ) );

    keyd.val2 = contentlen;

    setup_db();

    keyd.val1 = value.dptr && value.val2 >= contentlen
        ? lseek( db_datf, value.val1, 0 )
        : lseek( db_datf, 0, 2 );

    if( store( keyd ) < 0 )
        return -1;

    write( db_datf, content, contentlen );

    return 0;
}

//
//    return
//        -1 failed
//        0 success
//        1 success user must free content buffer
//
int database::get_db
    (
    const EmacsString &key,
    EmacsString &result_value
    )
{
    int read_size;
    off_t lseek_val;

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
        dbg_check_fildes( "start of get_db" );
# endif

    datum value(*this);
    value.dptr = key.data();
    value.dsize = key.length();

    value = fetch( value );
    if( value.dptr == 0 )
        return -1;

    int contentlen = (int)value.val2;
    unsigned char *content = malloc_ustr( contentlen );

    setup_db();

    lseek_val = lseek( db_datf, value.val1, 0 );
    read_size = read( db_datf, content, contentlen );

    EmacsString new_value( EmacsString::copy, content, contentlen );
    EMACS_FREE( content );

# if DBG_EXEC
    if( dbg_flags&DBG_EXEC )
    {
        dbg_check_fildes( "after read in get_db" );
        printf("lseek = %d, read = %d\n\r", (int)lseek_val, read_size );
    }
# endif

# ifdef __osf__
    // assume this means that a bug in OSF/1 read code
    if( read_size == 0 )
    {
        setup_db( NULL );
        setup_db();

        memset( content, 0, contentlen );

        // repeat the read we want to do
        lseek_val = lseek( db_datf, value.val1, 0 );
        read_size = read( db_datf, content, contentlen );
    }
# endif

    if( read_size != contentlen )
        return -1;

    result_value = new_value;

    return 1;
}

int database::get_db_help
    (
    const EmacsString &,
    EmacsString &
    )
{
    return 0;
}

int database::index_db
    (
    const EmacsString &match_string,
    int( *helper )( const EmacsString &, unsigned char ** )
    )
{
    for( datum key( *this, firstkey() ); key.dptr != 0; key = nextkey( key ) )
    {
        EmacsString key_name( EmacsString::keep, key.dptr, key.dsize );
        if( match_wild( key_name, match_string ) )
            if( !helper( key_name, NULL ) )
                return 0;
    }
    return 1;
}

#endif
