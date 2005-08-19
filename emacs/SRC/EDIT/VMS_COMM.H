/*	Copyright (c) 1982, 1983, 1984, 1985 */
/*		Barry A. Scott and nick Emery */
/* Strcutures, and definitions for VMSCOMS.C */
/* sub-process strcutures and defintions */
#define CHAN_BUFFER_SIZE 256			/* Channel Local Buffer Size */

struct msg_record {
	short int msg_type;
	short int msg_unit;
	unsigned char msg_name_size;
	unsigned char msg_name [15];
	short int msg_size;
	unsigned char msg_message[1];
	};

struct channel
	{					/* I/O channel structure */
	struct queue chan_queue;
	struct process_blk *chan_process;			/* Process Block */
	struct emacs_buffer *chan_buffer;			/* Buf Pointer */
	struct bound_name *chan_procedure;			/* Proc Pointer */
	int chan_chars_left;			/* # Chars left */
	int chan_num_reads_before_redisplay;	/* do_dsp inhibit */
	int chan_reads_to_go_before_redisplay;	/*	" Counter */
	int chan_maximum_buffer_size;		/* Delete Thresh. */
	int chan_buffer_reduction_size;		/* Amount to delete */
	unsigned chan_interrupt : 1;		/* interrupt occured */
	unsigned chan_read_channel : 1;	/* 1=read, 0=write */
	unsigned chan_channel_open : 1;	/* Channel is open */
	unsigned chan_local_buffer_has_data : 1; /* Data in local buf */
	unsigned chan_data_request : 1;	/* R/W request */
	unsigned chan_brdcst_chan : 1;	/* 1=Broadcast channel */
	unsigned short int chan_vms_channel;	/* VMS I/O Chan. */
	unsigned short int chan_mbx_unit;	/* MailBox Unit */
	union chan_local
		{
		unsigned char chan_buffer [256]; /* Local Buffer */
		struct msg_record chan_msg;
		} chan_local;
	};

/* Process States */
/* These are carefully ordered so that the following macros work!! */
#define PROC_K_RUNNING 0		/* Process Running */
#define PROC_K_PAUSED 1			/* We Suspended it */
#define PROC_K_BLOCKED_FOR_INPUT 2	/* Input Wait */
#define PROC_K_PAUSED_INPUTBLOCK 3	/* " " (PAUSED) */
#define PROC_K_BLOCKED_FOR_OUTPUT 4	/* Output Wait */
#define PROC_K_PAUSED_OUTPUTBLOCK 5	/* " " (PAUSED) */
#define PROC_K_DEAD 6			/* It Exited */

#define	PAUSED_TO_RUNNING(proc) (proc->proc_state &= ~PROC_K_PAUSED)
#define	RUNNING_TO_PAUSED(proc) (proc->proc_state |= PROC_K_PAUSED)
#define	RUNNING_TO_BLOCKED(proc,reason) (proc->proc_state += PROC_K_##reason)
#define	BLOCKED_TO_RUNNING(proc) (proc->proc_state &= PROC_K_PAUSED)
#define	IS_RUNNING(proc) ((proc->proc_state & PROC_K_PAUSED) == 0)
#define	IS_PAUSED(proc) (proc->proc_state & PROC_K_PAUSED)
#define	IS_BLOCKED(proc) (proc->proc_state > PROC_K_PAUSED)

struct process_blk
	{	/* Process structure */
	unsigned char *proc_name;			/* Local Name */
	unsigned int proc_process_id;			/* Process ID */
	struct channel proc_output_channel;		/* Output Channel */
	struct channel proc_input_channel;		/* Input Channel */
	struct bound_name *proc_procedure;		/* Termination Proc */
	unsigned long proc_time_state_was_entered;	/* When it changed */
	unsigned char proc_state;			/* Process State */
	};
