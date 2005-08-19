/* copyright 1994 Barry A. Scott */

struct errblk
	{
	struct marker *e_mess;	/* points to the error message */
        struct marker *e_text;	/* points to the next compiler error */
        struct errblk *e_next;	/* the next error in the chain */
        struct errblk *e_prev;	/* the next error in the chain */
	};

