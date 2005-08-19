/*	Copyright (c) 1982, 1983, 1984, 1985                                */
/*		Barry A. Scott and nick Emery                               */
/* structure that defines a window                                          */

struct window {
/* windows are organized in a double linked list.  Each window has          */
/* its own value of 'dot' and is tied to some buffer                        */
    struct window *w_next;              /* next window                      */
    struct window *w_prev;              /* preceeding window                */
    struct window *w_right;             /* preceeding window                */
    struct window *w_left;              /* preceeding window                */
    struct emacs_buffer *w_buf;		/* buffer tied to this window       */
    struct marker *w_dot;		/* value of "dot" for this buffer   */
    struct marker *w_start;		/* start character position in the tied buffer of the display */
    int w_height;			/* number of screen lines devoted to this window; includes space for the mode line */
    int w_width;			/* number of columns used           */
    int w_lastuse;			/* sequence counter for LRU window finding */
    int w_force;			/* true iff the value of start MUST be used on the next redisplay */
    int w_horizontal_scroll;		/* horizontal offset into start  of winodw */
    } ;
struct windowring {
    struct queue wr_queue;		/* queue header */
    struct windowring *wr_pushed;       /* pointer to pushed ring           */
    struct window *wr_windows;          /* window ring                      */
    struct window *wr_wn_cur;           /* current window in this ring      */
    struct marker *wr_bf_cur;		/* current buffer in this ring      */
    int wr_ref_count;			/* number of referances to this ring */
    unsigned wr_buffervisible : 1;	/* true if wr_bf_cur is visible     */
    } ;
