/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and nick Emery                               */
/* Definitions of objects used by the undo facility                         */
/* The events that can exist in the undo queue.                             */
#define UNDO_BOUNDARY 0                 /* A boundary between sets of undoable things */
#define UNDO_UNDOABLE 1                 /* What's done is done -- some things can't be undone */
#define UNDO_DELETE 2                   /* Delete characters to perform the undo */
#define UNDO_INSERT 3                   /* Insert ....                      */
struct undorec {                        /* A record of a single undo action */
    int undo_kind;                /* the kind of action to be undone  */
    int undo_len;                 /* The extent of the undo (characters */
/* inserted or deleted)                                                     */
    struct emacs_buffer *undo_buffer;	/* the buffer where the action takes place */
    int undo_dot;                  /* Where dot is                     */
    int undo_modified;             /* buffer-is-modified count from the buffer */
    };
/* The undo history consists of two circular queues, one of characters and  */
/* one of UndoRecs.  When Uinsert recs are added to UndoRQ characters get   */
/* added to UndoCQ.  The position of the characters can be reconstructed by */
/* subtracting len from the fill pointer.                                   */
#define UNDO_MAX_REC 1000
#define UNDO_MAX_CHAR 10003
