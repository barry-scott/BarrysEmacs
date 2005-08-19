/*
 *	unixcomm.h
 *
 *	Copyright (c) 1994 Barry A. Scott
 */
struct wh {
	int index;		/* Channel index */
	int count;		/* Count of remaining chars in data */
	int ccount;		/* Cound of control chars */
	unsigned char *buf;	/* Base buffer containing data */
	unsigned char *data;	/* Pointer to next segment to send */
};

/*
 * Structure records pertinent information about channels open.
 * There is one channel associated with each process.
 */
struct channel_blk
    {
    int ch_index;
    struct wh ch_outrec;		/* Output record */
    unsigned char *ch_ptr;		/* Pointer to next input character */
    short ch_count;			/* Count of characters remaining in buffer */
    struct emacs_buffer *ch_buffer;	/* Process is bound to this buffer */
    struct bound_name *ch_proc;		/* Procedure which gets called on output */
    };

/* Structure for information needed for each sub process started */

struct process_blk
    {
    struct channel_blk p_chan;		/* process i/o connected to channel */
    struct process_blk *next_process;	/* link to next process */
    unsigned char *p_name;		/* command that started process */
    unsigned char *p_pname;		/* Name of process */
    struct bound_name *term_proc;	/* Terminate procedure */
    XtInputId in_id;			/* Input event */
    XtInputId out_id;			/* Output event */
    pid_t p_id;				/* process id (group id as well */
    char p_flag;			/* RUNNING, STOPPED, etc */
    int p_reason;			/* signal causing p_flag */
    char out_id_valid;			/* True if out_id is valid */
    };

