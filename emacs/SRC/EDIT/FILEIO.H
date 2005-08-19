/********************************************************************************************************************************/
/* Created  4-NOV-1992 02:17:05 by VAX SDL V3.2-12     Source: 18-OCT-1992 00:14:40 SCOTT_VMS_VAX_SRC_ROOT:[SRC.EDITOR]FILEIO_STR */
/********************************************************************************************************************************/
 
/*** MODULE emacs$fileio ***/
/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and Nick Emery                               */
/*                                                                          */
/*	Structures used by fileio.c                                         */
/*                                                                          */
struct automode
	{                       /* information for automatic mode recognition */
	unsigned char *a_pattern;           /* the pattern that the name must match */
	int a_len;				/* the length of the pattern string */
	struct bound_name *a_what;		/* what to do if we find it         */
	struct automode *a_next;		/* the next thing to try            */
	};

/* create modes                                                             */
#define FIO_STD 0
#define FIO_CKP 1
/* Open modes                                                               */
#define FIO_READ 0
#define FIO_WRITE 1
