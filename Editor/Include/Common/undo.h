//    Copyright (c) 1982-2010
//        Barry A. Scott
//
// Definitions of objects used by the undo facility
// The events that can exist in the undo queue.
//
enum UNDO_type
{
    UNDO_BOUNDARY,              // A boundary between sets of undoable things
    UNDO_UNDOABLE,              // What's done is done -- some things can't be undone
    UNDO_DELETE,                // Delete characters to perform the undo
    UNDO_INSERT                 // Insert ....
};

struct undorec
{                               // A record of a single undo action
    UNDO_type undo_kind;        // the kind of action to be undone
    int undo_len;               // The extent of the undo (characters
                                // inserted or deleted)
    EmacsBuffer *undo_buffer;   // the buffer where the action takes place
    int undo_dot;               // Where dot is
    int undo_modified;          // buffer-is-modified count from the buffer
};

//
// The undo history consists of two circular queues, one of characters and
// one of UndoRecs.  When Uinsert recs are added to UndoRQ characters get
// added to UndoCQ.  The position of the characters can be reconstructed by
// subtracting len from the fill pointer.
//
const int UNDO_MAX_REC( 10000 );
const int UNDO_MAX_CHAR( 100003 );
