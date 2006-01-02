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
#define JNL_VERSION 2

#define INT long int

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
    unsigned char jnl_chars[ sizeof(INT) * 4 ];
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

#define JNL_BUF_SIZE 64        // this yields a 1k buffer

int main( int argc, char **argv )
{
    int status;
    FILE *file;
    union journal_record buf[JNL_BUF_SIZE];
    int offset;
    long file_offset;
    union journal_record *rec;
    unsigned char *journal_file;
    unsigned char journal_filename[MAXPATHLEN+1];

    file = fopen(argv[1], "rb");
    if( file == NULL )
        return 1;
    //
    //    Read the first record from the journal.
    //    It will be the file or buffer that is
    //    to be recovered.
    //
    offset = 0;
    file_offset = 0l;
    status = fread( buf, JNL_BYTE_SIZE, JNL_BUF_SIZE, file );

    printf("Block offset: 0x%lx (0x%x)\n", file_offset, status );

    if( status == 0 || feof( file ) || ferror( file ) )
    {
        printf( "Unable to read the first record from the journal");
        goto journal_recover;
    }

    rec = &buf[offset];

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
            rec = &buf[offset];
            switch( rec->jnl_open.jnl_type )
        {
            case JNL_FILENAME:
        printf(" JNL_FILENAME(0x%x): Version: %ld File: %s\n", offset,
                    rec->jnl_open.jnl_version,
                    rec[1].jnl_data.jnl_chars );
                offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
                break;

            case JNL_BUFFERNAME:
        printf(" JNL_BUFFERNAME(0x%x): Version: %ld Buf: %s\n", offset,
                    rec->jnl_open.jnl_version,
                    rec[1].jnl_data.jnl_chars );
                offset = 1 + JNL_BYTE_TO_REC( rec->jnl_open.jnl_name_length );
                break;

            case JNL_INSERT:
        printf(" JNL_INSERT(0x%x): Dot:%ld Size:%ld\n", offset,
                    rec->jnl_insert.jnl_dot,
                    rec->jnl_insert.jnl_insert_length
                    );
    {
        int len = (int)rec->jnl_insert.jnl_insert_length;
        unsigned char *p = rec[1].jnl_data.jnl_chars;
        int i;

        for( i=0; i<len; i++ )
            if( p[i] < ' ' )
                printf("%1.1x", p[i]>>4);
            else
                printf("_");
        printf("\n");
        for( i=0; i<len; i++ )
            if( p[i] < ' ' )
                printf("%1.1x", p[i]&0xf);
            else
                printf("%c", p[i]);
        printf("\n");
    }

                offset += 1 + JNL_BYTE_TO_REC( rec->jnl_insert.jnl_insert_length );
                break;

            case JNL_DELETE:
        printf(" JNL_DELETE(0x%x): Dot:%ld Size:%ld\n", offset,
                    rec->jnl_delete.jnl_del_dot,
                    rec->jnl_delete.jnl_length
                    );
                offset += 1;
                break;

            case JNL_END:
        printf(" JNL_END(0x%x):\n", offset);
                goto exit_loop;
            default:
        printf( ("Unexpected type(0x%x): Type: %lx\n"),
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
        printf("Block offset: 0x%lx (0x%x)\n", file_offset, status );
    }
    while( !(status != JNL_BUF_SIZE || feof( file ) || ferror( file )) );

    printf("Recovery completed");

journal_recover:
    //
    //    Tidy up and exit
    //
    fclose( file );

    return 0;
}
