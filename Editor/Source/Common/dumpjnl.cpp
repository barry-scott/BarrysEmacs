#include <stdio.h>
#define MAXPATHLEN 64

//    Copyright (c) 1987
//        Barry A. Scott and Nick Emery

//    Definition for the journal subsystem

#define JNL_END 0                     // end of block
#define JNL_FILENAME 1                // name of file being journalled
#define JNL_BUFFERNAME 2              // name of buffer being journalled
#define JNL_INSERT 3                  // insert text
#define JNL_DELETE 4                  // delete text
#define JNL_VERSION 3

#define INT int

typedef int EmacsChar_t;

struct jnl_open
{
    INT jnl_type;
    INT jnl_version;
    INT jnl_name_length;
};

struct jnl_insert
{
    INT jnl_type;
    INT jnl_dot;
    INT jnl_insert_length;
};

struct jnl_delete
{
    INT jnl_type;
    INT jnl_del_dot;
    INT jnl_length;
};

struct jnl_data
{
    EmacsChar_t jnl_chars[ 16/sizeof(EmacsChar_t) ];
};

#define JNL_BYTE_SIZE sizeof( struct jnl_data )
int JNL_BYTE_TO_REC( int n )
{
    int numrec =  ((n)+JNL_BYTE_SIZE-1)/JNL_BYTE_SIZE;
    return numrec;
}

union journal_record
{
    struct jnl_open jnl_open;
    struct jnl_insert jnl_insert;
    struct jnl_delete jnl_delete;
    struct jnl_data jnl_data;
};

#define JNL_BUF_SIZE 64        // this yields a 1k buffer

char *fromUnicode( EmacsChar_t *unicode, int length )
{
    static char charbuf[1024];

    for( int i=0; i<length; ++i )
    {
        charbuf[i] = static_cast<char>( unicode[i] );
    }
    charbuf[length] = 0;

    return charbuf;
}

int main( int argc, char **argv )
{
    if( argc != 2 )
    {
        return 1;
    }

    FILE *file = fopen(argv[1], "rb");
    if( file == NULL )
    {
        return 1;
    }

    //
    //    Read the first record from the journal.
    //    It will be the file or buffer that is
    //    to be recovered.
    //
    int offset = 0;
    long file_offset = 0l;

    union journal_record buf[JNL_BUF_SIZE];

    size_t status = fread( (unsigned char *)buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );

    printf("Block offset: 0x%lx (0x%zx)\n", file_offset, status );

    if( status == 0 || ferror( file ) )
    {
        printf( "Unable to read the first record from the journal");
        return 0;
    }

    union journal_record *rec = &buf[offset];

    //
    //    Read all the blocks in the journal file
    //
    do
    {
        //
        //    Action all the records in the block
        //
        while( offset < JNL_BUF_SIZE )
        {
            printf( "offset: %d\n", offset );
            rec = &buf[offset];

            switch( rec->jnl_open.jnl_type )
            {
            case JNL_FILENAME:
                printf(" JNL_FILENAME(0x%x): Version: %d File: %s\n", offset,
                    rec->jnl_open.jnl_version,
                    fromUnicode( rec[1].jnl_data.jnl_chars, rec->jnl_open.jnl_name_length ) );
                offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length*sizeof( EmacsChar_t ) );
                break;

            case JNL_BUFFERNAME:
                printf(" JNL_BUFFERNAME(0x%x): Version: %d Buf: %s\n", offset,
                    rec->jnl_open.jnl_version,
                    fromUnicode( rec[1].jnl_data.jnl_chars, rec->jnl_open.jnl_name_length ) );
                offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length*sizeof( EmacsChar_t ) );
                break;

            case JNL_INSERT:
                printf(" JNL_INSERT(0x%x): Dot:%d Size:%d\n", offset,
                    rec->jnl_insert.jnl_dot,
                    rec->jnl_insert.jnl_insert_length
                    );
                {
                    int len = (int)rec->jnl_insert.jnl_insert_length;
                    int *p = rec[1].jnl_data.jnl_chars;
                    int i;

                    for( i=0; i<len; i++ )
                        if( p[i] < ' ' )
                            printf("%1.1x", p[i]>>4);
                        else
                            printf("%c", p[i]);
                    printf("\n");
                    for( i=0; i<len; i++ )
                        if( p[i] < ' ' )
                            printf("%1.1x", p[i]&0xf);
                        else
                            printf("-");
                    printf("\n");
                }

                offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length*sizeof(EmacsChar_t) );
                break;

            case JNL_DELETE:
                printf(" JNL_DELETE(0x%x): Dot:%d Size:%d\n", offset,
                    rec->jnl_delete.jnl_del_dot,
                    rec->jnl_delete.jnl_length
                    );
                offset += 1;
                break;

            case JNL_END:
                printf(" JNL_END(0x%x):\n", offset);
                goto exit_loop;

            default:
                printf( ("Unexpected type(0x%x): Type: %x\n"),
                    offset,
                    rec->jnl_open.jnl_type );
                offset += 1;
                break;
            }
        }
exit_loop:
        offset = 0;
        file_offset += JNL_BYTE_SIZE*JNL_BUF_SIZE;
        status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );
        printf("Block offset: 0x%lx (0x%zx)\n", file_offset, status );
    }
    while( !(status != JNL_BUF_SIZE || feof( file ) || ferror( file )) );

    printf("Recovery completed");

    //
    //    Tidy up and exit
    //
    fclose( file );

    return 0;
}
