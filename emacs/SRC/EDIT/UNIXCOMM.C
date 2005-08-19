/* copyright (c) 1993-1995 Barry A. Scott */

#include <emacs.h>

GLOBAL SAVRES int force_redisplay;
GLOBAL SAVRES int maximum_dcl_buffer_size = 10000;
GLOBAL SAVRES int dcl_buffer_reduction = 500;

#if defined( SUBPROCESSES )

# include <unistd.h>
# include <fcntl.h>
# include <stdio.h>
# include <signal.h>
# include <errno.h>
# include <sys/param.h>
# include <sys/ioctl.h>
#if defined( __hpux )
#  include <sys/pty.h>
#endif
# 
# if !defined( _BSD ) && !defined( __FreeBSD__ )
#  define _BSD
#  include <sys/wait.h>
#  undef _BSD
# else
#  include <sys/wait.h>
# endif
# if defined( __ultrix )
#  include <sgtty.h>
#  define sgttyb termios
#  define tcgetattr( h, sg ) ioctl(h, TIOCGETP, sg);
#  define tcsetattr( h, sg ) ioctl(h, TIOCSETP, sg);
# else
#  include <termios.h>
# endif
# include <sys/time.h>
# include <sys/stat.h>
# ifdef XWINDOWS
#  include <X11/Intrinsic.h>
#  include <unixcomm.h>
extern int is_motif;
extern XtAppContext app_context;



#  define STOPPED		(1<<0)
#  define RUNNING		(1<<1)
#  define EXITED		(1<<2)
#  define SIGNALED		(1<<3)
#  define CHANGED		(1<<6)

#  define active_process(x) ((x!=NULL)&& (x->p_flag&(RUNNING|STOPPED)))

extern int errno;

int process_fds[] = {0, 0};		/* The set of subprocess fds */
struct process_blk *process_list;	/* all existing processes */
struct process_blk *current_process;	/* the one that we're current dealing with */
int child_changed;			/* Flag when a child process has ceased to be */
static unsigned char cbuffer[BUFSIZ];	/* Used for reading mpx file */
struct channel_blk *MPX_chan;
extern int subproc_id;			/* The process id of a subprocess started by the old subproc stuff. */
extern char **environ;

char *SIG_names[] = {
    "",
    "Hanged",
    "Interrupted",
    "Quit*",
    "Illegal instruction*",
    "Trapped*",
    "Iot*",
    "EMT*",
    "FPE*",
    "Killed",
    "Bus error*",
    "Segment violation*",
    "Sys call error*",
    "Pipe write error",
    "Alarm clock",
    "Termination",
    "",
    "Stopped",
    "Stopped",
    "", "", "", "", "", "", ""
    };

extern char *shell();
struct process_blk *get_next_process();
void stuff_buffer(register struct channel_blk *);
int flush_process(register struct process_blk *);
extern XtInputId add_select_fd (int, long int, XtInputCallbackProc, XtPointer);
extern void remove_select_fd (XtInputId);

unsigned char str_process [] = "Process: ";
unsigned char str_err_proc [] = "Cannot find the specified process";
unsigned char str_is_blocked [] = "There is data already waiting to be send to the blocked process";
XtInputId add_input (int channel, long int mask, XtInputCallbackProc input_request, struct process_blk *npb)
    {
    if (channel < sizeof (int) * CHAR_BIT)
	process_fds[0] |= 1<<channel;
    else
	process_fds[1] |= 1<<(channel-(sizeof (int) * CHAR_BIT));
#  ifdef XWINDOWS
    if (is_motif)
	{
	/* this is the way to pass the mask in without an error or warning */
	union xt_arg
		{
		long l;
		XtPointer p;
		} xt_arg;
	xt_arg.l = mask;
	return XtAppAddInput (app_context, channel, xt_arg.p, input_request, (XtPointer) npb);
	}
#  endif
    else
    return add_select_fd (channel, mask, input_request, npb);
    }
void remove_input (XtInputId id)
    {
#  ifdef XWINDOWS
    if (is_motif)
	XtRemoveInput (id);
    else
#  endif
    remove_select_fd (id);
    }
/* locate the next pty to use and open the master side */
static char *pty(int *ptyv)
    {
    struct stat stb;
    static char name_p[24];
    static char name_t[24];
    int lastNameChar = 9;
    int on = 1, i;
    int tty_d;

    _str_cpy(name_p, u_str ("/dev/ptypX"));
    _str_cpy(name_t, u_str ("/dev/ttypX"));
    for (;;)
	{
	name_p[lastNameChar] = '0';
	name_t[lastNameChar] = '0';

	if (stat(name_p, &stb) < 0)
	    return 0;
	if (stat(name_t, &stb) < 0)
	    return 0;
	for (i = 0; i < 16; i++)
	    {
	    name_p[lastNameChar] = "0123456789abcdef"[i];
	    name_t[lastNameChar] = "0123456789abcdef"[i];
	    *ptyv = open(name_p, 2);	
	    if (*ptyv >= 0)
		{
		tty_d = open(name_t, 2);
		if ( (tty_d >=0))
		    {
#ifdef TIOCREMOTE
		    ioctl(*ptyv, TIOCREMOTE, &on);
#endif
		    ioctl(*ptyv, FIONBIO, &on);
		    close(tty_d);
		    return name_t;
		    }
		else
		    close(*ptyv);
		}
	    }
	name_p[lastNameChar-1]++;		
	name_t[lastNameChar-1]++;
	}
    }
/* Callback to handle an input request */
void input_request (void *p_, int *PNOTUSED(fdp), XtInputId *PNOTUSED(id))
    {
    struct process_blk *p = (struct process_blk *)p_;
    int cc;
    struct channel_blk *chan = &p->p_chan;

    cc = read(chan->ch_index, cbuffer, sizeof (cbuffer));
    if (cc > 0) 
	{
	chan->ch_ptr = cbuffer;
	chan->ch_count = cc;
	stuff_buffer (chan);
	}
    else if (cc <= 0)
	{
	remove_input (p->in_id);
	if (p->out_id_valid)
	    {
	    remove_input (p->out_id);
	    p->out_id_valid = 0;
	    }
	close(chan->ch_index);
	if (chan->ch_index < sizeof (int) * CHAR_BIT)
	    process_fds[0] &= ~(1<<chan->ch_index);
	else
	    process_fds[1] &= ~(1<<(chan->ch_index-(sizeof (int) * CHAR_BIT)));
	}
    }
/* Callback to handle an output request */
void output_request (void *p_, int *PNOTUSED(fdp), XtInputId *PNOTUSED(id))
    {
    struct process_blk *p = (struct process_blk *)p_;
    int cc;
    struct channel_blk *chan = &p->p_chan;

    if (chan->ch_outrec.ccount) 
	{
	cc = write(chan->ch_index, "\004", 1);
	if (cc < 0)
	    return;
	chan->ch_outrec.ccount = 0;
	}
    if (chan->ch_outrec.count)
	{
	cc = write(chan->ch_index, chan->ch_outrec.data, chan->ch_outrec.count);
	if (cc > 0)
	    {
	    chan->ch_outrec.data += cc;
	    chan->ch_outrec.count -= cc;
	    }
	}
    else
	{
	if (p->out_id_valid)
	    {
	    remove_input (p->out_id);
	    p->out_id_valid = 0;
	    }
	if (chan->ch_outrec.buf)
	    {
	    free (chan->ch_outrec.buf);
	    chan->ch_outrec.buf = NULL;
	    }
	}
    }
/* Start up a subprocess with its standard input and output connected to 
 * a channel on the master side of a ptyf.  Also set its process group so we can kill it
 * and set up its process block.  The process block is assumed to be pointed
 * to by current_process.
 */
int create_process (struct process_blk *npb)
    {
    unsigned char *command = npb->p_name;
    int channel;
    int newfd;
    pid_t pid;
    long sig_mask;
    char *ptyname;
    char **p, **term_is = NULL, **termcap_is = NULL, *old_term, *old_termcap;

    /* Open the pty master side */
    ptyname = pty(&channel);
    if (ptyname == 0)
	{
	error(u_str ("Cannot get a pseudo-terminal for the process\n"));
	return(-1);
	}
    if (channel > MAXFDS)
	{
	close (channel);
	error (u_str ("Too many processes already running"));
	}

    /* Find and nobble TERM and TERMCAP */
    for (p = environ; *p && (term_is == NULL || termcap_is == NULL); p++)
	if (_str_ncmp ("TERM=", *p, 5) == 0)
	    term_is = p;
	else if (_str_ncmp ("TERMCAP=", *p, 8) == 0)
	    termcap_is = p;
    if (term_is)
	{
	old_term = *term_is;
	*term_is = "TERM=unknown";
	}
    if (termcap_is)
	{
	old_termcap = *termcap_is;
	*termcap_is = "NOTERMCAP=none";
	}
    /* Fork the child */
    if ((pid = vfork()) < 0)
	{
	error(u_str ("Fork failed for process"));
	close(channel);
	return - 1;
	}

    if (pid == 0)
	{
	/* Handle child side of fork */
	char *shname = shell();
	struct termios sg;
	int ld;

	tcgetattr( 0, &sg );
	close(channel);
	sig_mask = sigblock(0);
    	sigsetmask(~(1<<SIGCHLD) & sig_mask);
	signal(SIGCHLD, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
#ifdef TIOCNOTTY
	{
	int f;
	if ((f = open("/dev/tty", 2)) >= 0)
	    {
	    ioctl(f, TIOCNOTTY, 0);
	    close(f);
	    }
	}
#endif

	if ((newfd = open(ptyname, 2)) < 0)
	    {
	    fprintf(stdout, "Cannot open pseudo-terminal %s\n", ptyname);
	    _exit(1);
	    }

#ifdef _POSIX_VERSION
	setpgid(0, getpid());
#else
	setpgrp();
#endif
	dup2(newfd, 0); dup2(newfd, 1); dup2(newfd, 2);close(newfd);

	sg.c_cc[VERASE] = 0xff;
	sg.c_cc[VKILL] = 0xff;

	sg.c_cc[VINTR] = 0177;
	sg.c_cc[VQUIT] = '\\' & 037;
	sg.c_cc[VSTART] = 'Q' & 037;
	sg.c_cc[VSTOP] = 'S' & 037;
	sg.c_cc[VEOF] = 'D' & 037;
#ifdef VBRK
	sg.c_cc[VBRK] = 0xff;
#endif
	sg.c_cc[VSUSP] = 'Z' & 037;
#  ifdef VDSUSP
	sg.c_cc[VDSUSP] = 'Y' & 037;
#  endif
#  ifdef VREPRINT
	sg.c_cc[VREPRINT] = 'R' & 037;
#  endif
#  ifdef VDISCARD
	sg.c_cc[VDISCARD] = 'O' & 037;
#  endif
#  ifdef VWERASE
	sg.c_cc[VWERASE] = 'W' & 037;
#  endif
#  ifdef VLNEXT
	sg.c_cc[VLNEXT] = 'V' & 037;
#  endif

	tcsetattr( 0, TCSANOW, &sg );

	ld = 2;
	if (_str_icmp(u_str(shname+_str_len(shname)-3), u_str ("csh")) == 0)
	    ld = 0;
#ifdef TIOCSETD
	ioctl(0, TIOCSETD, &ld);
#endif
	if (_str_icmp(u_str(shname+_str_len(shname)-3), u_str ("csh")) == 0)
	    execlp(shname, shname, "-f", "-c", command, 0);
	else
	    execlp(shname, shname, "-c", command, 0);
	write(1, "Could not start the shell\n", 24);
	_exit(1);
	}

    /* Unnobble the TERM and TERMCAP entries */
    if (term_is)
	*term_is = old_term;
    if (termcap_is)
	*termcap_is = old_termcap;

    /* Handle parent side of fork */
    npb->p_id = pid;
    npb->p_flag = RUNNING;
    npb->in_id = add_input (channel, XtInputReadMask, input_request, npb);
    npb->out_id = add_input (channel, XtInputWriteMask, output_request, npb);
    npb->out_id_valid = 1;
    npb->p_chan.ch_index = channel;
    npb->p_chan.ch_ptr = NULL;
    npb->p_chan.ch_count = 0;
    npb->p_chan.ch_outrec.index = channel;
    npb->p_chan.ch_outrec.count = 0;
    npb->p_chan.ch_outrec.ccount = 0;
    npb->p_chan.ch_outrec.buf = NULL;
    npb->term_proc = NULL;
    return 0;
    }

/* Process a signal from a child process and make the appropriate change in 
 * the process block. Since signals are NOT queued, if two signals are
 * received before this routine gets called, then only the first process in
 * the process list will be handled.  We will try to get the MPX file stuff
 * to help us out since it passes along signals from subprocesses.
 */
void child_sig(int PNOTUSED(x))
    {
    pid_t pid;
    union wait w;
    register struct process_blk	*p;

loop: 
    pid = wait3(&w.w_status, WUNTRACED | WNOHANG, 0);
    if (pid <= 0)
	{
	if (errno == EINTR)
	    {
	    errno = 0;
	    goto loop;
	    }
	if (pid == -1) {
	    if (!active_process(current_process))
		current_process = get_next_process();
	    }
	signal(SIGCHLD, child_sig);
	return;
	}
    if (pid == subproc_id)
	{
	/* Take care of those subprocesses first */
	subproc_id = 0;		
	goto loop;
	}
    for (p = process_list; p != NULL; p = p->next_process)
	if (pid == p->p_id)
	    break;
    if (p == NULL)
	goto loop;		/* We don't know who this is */

    if (WIFSTOPPED(w))
	{
	p->p_flag = STOPPED;
	p->p_reason = w.w_stopsig;
	}
    else
	if (WIFEXITED(w))
	    {
	    p->p_flag = EXITED | CHANGED;
	    child_changed++;
	    p->p_reason = w.w_retcode;
	    }
	else
	    if (WIFSIGNALED(w))	
		{
		p->p_flag = SIGNALED | CHANGED;
		child_changed++;
		p->p_reason = w.w_termsig;
		}
    if (!active_process(current_process))
	current_process = get_next_process();
    goto loop;
    }
/*
 * Find the process which is connected to proc_name.
 */
struct process_blk *find_process (register unsigned char *proc_name)
    {
    register struct process_blk *p;

    if (proc_name == NULL)
	return (NULL);
    for (p = process_list; p != NULL; p = p->next_process)
	{
	if (!active_process(p))
	    continue;
	if (_str_icmp(p->p_pname, proc_name) == 0)
	    break;
	}
    return(p);
    }
/*
 * Get the first active process in the process list and assign to the current
 * process
 */
struct process_blk *get_next_process()
    {
    register struct process_blk *p;

    for (p = process_list; p != NULL && !active_process(p); p = p->next_process);
    return (p);
    }
/* Give a message that a process has changed and indicate why.  Dead processes
 * are not removed until after a Display Processes command has been issued so
 * that the user doesn't wonder where his process went in times of intense
 * hacking
 */
void change_msgs(void)
    {
    register struct process_blk	*p, *np;
    register int dodsp = 0, restore = 0;
    register struct emacs_buffer *old = bf_cur;
    unsigned char line[50];
    int OldBufferIsVisible = (wn_cur->w_buf == bf_cur);
    int old_cc = child_changed, change_processed;

    for (p = process_list, change_processed = 0; p != NULL; p = np)
	{
	np = p->next_process;
	if (p->p_flag & CHANGED)
	    {
	    line[0] = '\0';
	    p->p_flag &= ~CHANGED;
	    change_processed ++;
	    if (p->term_proc != NULL)
		{
		enum arg_states LArgState = arg_state;
		int larg = arg;

		arg_state = no_arg;
		switch (p->p_flag & (SIGNALED | EXITED))
		    {
		    case SIGNALED: 
			sprintfl (line, sizeof line, u_str ("%s\n"), SIG_names[p->p_reason]);
			break;
		    case EXITED: 
			sprintfl(line, sizeof line, u_str ("Exited %d\n"), p->p_reason);
			break;
		    }
		MPX_chan = &p->p_chan;	/* User will be able to get the output for */
		MPX_chan->ch_ptr = line;
		MPX_chan->ch_count = _str_len (line);
		execute_bound(p->term_proc);
		arg_state = LArgState;
		arg = larg;
		MPX_chan = NULL;	/* a very short time only */
		dodsp++;
		flush_process (p);
		}
	    else if (p->p_chan.ch_buffer != NULL)
		{
		switch (p->p_flag & (SIGNALED | EXITED))
		    {
		    case SIGNALED: 
			sprintfl(line, sizeof line, u_str ("%s\n"), SIG_names[p->p_reason]);
			break;
		    case EXITED: 
			if (p->p_reason != 0)
			    sprintfl(line, sizeof line, u_str ("Exited with status %d\n"), p->p_reason);
			break;
		    }
		set_bfp(p->p_chan.ch_buffer);
		set_dot(bf_s1 + bf_s2 + 1);
		ins_str(line);
		if ((bf_s1 + bf_s2) > maximum_dcl_buffer_size)
		    {
		    del_frwd (1, dcl_buffer_reduction);
		    set_dot (bf_s1 + bf_s2 + 1);
		    }
		if (bf_cur->b_mark == NULL)
		    bf_cur->b_mark = new_mark();
		set_mark(bf_cur->b_mark, bf_cur, dot, 0);
		dodsp++;
		restore++;
		}
	    }
	}

    /* Update the screen if necessary */
    if (dodsp)
	{
	if (restore)
	    {
	    set_bfp (old);
	    if (interactive && OldBufferIsVisible)
		window_on (bf_cur);
	    }
	do_dsp(1);
	}

    /* Update the child_changed counter */
    if (change_processed == 0)
	{
	if (child_changed == old_cc)
	    child_changed = 0;
	}
    else
	child_changed -= change_processed;
    }
/*
 * Send any pending output as indicated in the process block to the 
 * appropriate channel.
 */
int send_chan(register struct process_blk *process)
    {
    register struct wh *output;

    output = &process->p_chan.ch_outrec;
    if (output->count == 0 && output->ccount == 0)
	;
    else if (output->ccount) 
	{
	if (write(output->index, "\004", 1) >= 0) 
	    {
	    output->ccount = 0;
	    }
	}
    else
	{
	if (output->count)
	    {
	    int cc = write(output->index, output->data, output->count);

	    if (cc > 0) 
		{
		output->data += cc;
		output->count -= cc;
		}
	    }
	}
    if (!process->out_id_valid)
	{
	process->out_id = add_input (output->index, XtInputWriteMask, output_request, process);
	process->out_id_valid = 1;
	}
    return 0;
    }
/* Output has been recieved from a process on "chan" and should be stuffed in
 * the correct buffer
 */
void stuff_buffer(register struct channel_blk *chan)
    {
    struct emacs_buffer *old_buffer = bf_cur;
    static int lockout;
    int OldBufferIsVisible = (wn_cur->w_buf == bf_cur);

    /* Check the lock */
    if (lockout)
	{
	error(u_str ("On-output procedure asked for input"));
	return;
	}
    else
	lockout = 1;

    if (chan->ch_proc == NULL)
	{
	if (chan->ch_buffer == NULL)
	    error(u_str ("Process output available with no destination buffer"));
	else
	    {
	    register unsigned char *p, *q;
	    
	    set_bfp (chan->ch_buffer);
	    set_dot (bf_s1 + bf_s2 + 1);
	    for (p = q = chan->ch_ptr; p < &chan->ch_ptr[chan->ch_count]; p++)
		if (*p == '\r')
		    {
		    if (p - q > 0)
			ins_cstr (q, p - q);
		    q = &p[1];
		    }
	    if (p - q > 0)
		ins_cstr (q, p - q);
	    if ((bf_s1 + bf_s2) > maximum_dcl_buffer_size)
		{
		del_frwd(1, dcl_buffer_reduction);
		set_dot (bf_s1 + bf_s2 + 1);
		}
	    if (bf_cur->b_mark == NULL)
		bf_cur->b_mark = new_mark();
	    set_mark(bf_cur->b_mark, bf_cur, dot, 0);
	    do_dsp(1);
	    set_bfp(old_buffer);
	    if (interactive && OldBufferIsVisible)
		window_on(bf_cur);
	    }
	}
    else
	{
	enum arg_states LArgState = arg_state;
	int larg = arg;

	arg_state = no_arg;
	MPX_chan = chan;
	execute_bound (chan->ch_proc);
	arg_state = LArgState;
	arg = larg;
	MPX_chan = NULL;	/* a very short time only */
	}
    chan->ch_count = 0;
    lockout = 0;
    }
/* 
 * Return a count of all active processes
 */
int count_processes()
    {
    register struct process_blk *p;
    register count = 0;

    for (p = process_list; p != NULL; p = p->next_process)
	if (active_process(p))
	    count++;
    return (count);
    }
/*
 * Flush a process but only if process is inactive
 */
int flush_process(register struct process_blk *process)
    {
    struct process_blk *p, *lp;
    struct channel_blk *chan;

    if (active_process(process))
	{
	error(u_str ("Cannot flush an active process"));
	return 0;
	}

    for (lp = NULL, p = process_list; (p != NULL) && (p != process); lp = p, p = p->next_process)
	;
    if (p != process)
	{
	error(str_err_proc);
	return 0;
	}
    if (lp == NULL)
	process_list = process->next_process;
    else
	lp->next_process = process->next_process;

    chan = &process->p_chan;

    remove_input (p->in_id);
    if (p->out_id_valid)
	remove_input (p->out_id);

    /* close the PTY master side channel */
    close(chan->ch_index);

    if (chan->ch_index < sizeof (int) * CHAR_BIT)
	process_fds[0] &= ~(1<<chan->ch_index);
    else
	process_fds[1] &= ~(1<<(chan->ch_index-(sizeof (int) * CHAR_BIT)));

    if (process->p_chan.ch_outrec.buf)
	{
	free (process->p_chan.ch_outrec.buf);
	process->p_chan.ch_outrec.buf = NULL;
	}
    if (process->p_pname)
	free (process->p_pname);
    if (process->p_name)
	free (process->p_name);
    free(process);
    return 0;
    }
/*
 * Kill off all active processes: usually done only to exit when user really
 * insists
 */
void kill_processes(void)
    {
    register struct process_blk *p;

    for (p = process_list; p != NULL; p = p->next_process)
	if (active_process(p))
#ifdef __hpux
	    kill(p->p_id, SIGKILL);
#else
	    killpg (p->p_id, SIGKILL);
#endif
    }
/*
 * Start up a new process by creating the process block and initializing 
 * things correctly
 */
int start_process(register unsigned char *com, register unsigned char *buf, int proc)
    {
    long sig_mask;
    struct process_blk *new_process;

    if (com == 0)
	return (0);
    new_process = (struct process_blk *) malloc_struct (process_blk);
    if (new_process == NULL)
	{
	error(u_str ("Out of memory while creating process"));
	return 0;
	}
    sig_mask = sigblock(1<<SIGCHLD);
    new_process->next_process = process_list;
    new_process->p_name = com;
    process_list = new_process;
    if (create_process(new_process) < 0) 
	{
	/* job was not started, so undo */
	flush_process(new_process);
	sigsetmask(~(1<<SIGCHLD) & sig_mask);
	return 0;
        }
    set_bfn (buf == NULL ? u_str ("command execution") : buf);
    if (interactive)
	window_on(bf_cur);
    new_process->p_chan.ch_buffer = bf_cur;
    new_process->p_pname = savestr (new_process->p_chan.ch_buffer->b_buf_name);
    new_process->p_chan.ch_proc = (proc < 0 ? NULL : mac_bodies[proc]);
    sigsetmask(~(1<<SIGCHLD) & sig_mask);
    current_process = new_process;
    return 0;
    }
/*
 * Emacs command to start up a default process: uses "Command Execution"
 * buffer if one is not specified.  Also does default stuffing
 */
int start_dcl_process()
    {
    register unsigned char *com;
    register char unsigned  *buf = getstr(u_str ("Process name: "));;

    if (buf == NULL)
	return 0;
    if (*buf == '\0')
	buf = NULL;
    else
	buf = savestr (buf);
    com = getstr(u_str ("Command: "));
    if (com == NULL || *com == '\0')
	{
	error (u_str ("No command specified for process to execute"));
	return 0;
	}
    com = savestr (com);
    start_process (com, buf, -1);
    if (buf)
	free (buf);
    return 0;
    }
/*
 * Internal Procedure to get an MLisp procedure argument
 */
static struct bound_name * get_procedure_arg(unsigned char *prompt )
    {
    int index;
    index = getword( mac_names_ref, prompt);

    if( index >= 0 && _str_cmp( mac_names[index], u_str("novalue")) != 0 )
	return mac_bodies[index];
    else
	return 0;
    }
/*
 * Insert a filter-procedure between a process and emacs. This function
 * should subsume the StartFilteredProcess function, but we should retain
 * that one for compatibility I suppose...
 */
int set_process_output_procedure( void )
    {
    register unsigned char *proc_name = getstr(str_process);
    register struct process_blk	*process;

    if ((process = find_process(proc_name)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    process->p_chan.ch_proc = get_procedure_arg (u_str ("On-output procedure: "));
    return (0);
    }
/*
 * Insert a termination procedure to be called when the process terminates
 */
int set_process_termination_proc( void )
    {
    register unsigned char *proc_name = getstr(str_process);
    register struct process_blk	*process;

    if ((process = find_process(proc_name)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    process->term_proc = get_procedure_arg (u_str ("On-termination procedure: "));
    return (0);
    }
/*
 * Sets the process name
 */
int set_process_name( void )
    {
    register unsigned char *new_name, *proc_name = getstr(str_process);
    register struct process_blk	*process, *np;

    if ((process = find_process(proc_name)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    new_name = getstr(u_str ("New name: "));
    if (new_name == NULL || *new_name == '\0')  
	return 0;
    if ((np = find_process (new_name)) != NULL)
	{
	error(u_str ("A process with the new name already exists"));
	return 0;
	}
    if (process->p_pname)
	free (process->p_pname);
    process->p_pname = savestr (new_name);
    return (0);
    }
/*
 * Sets the process output buffer
 */
int set_process_output_buffer( void )
    {
    register unsigned char *new_name, *proc_name = getstr(str_process);
    register struct process_blk	*process;

    if ((process = find_process(proc_name)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    new_name = getstr(u_str ("New buffer: "));
    if (new_name == NULL)  
	return 0;
    set_bfn (*new_name =='\0' ? u_str ("command execution") : new_name);
    if (interactive)
	window_on(bf_cur);
    process->p_chan.ch_buffer = bf_cur;
    return (0);
    }
/*
 * List the current processes.  After listing stopped or exited processes,
 * flush them from the process list.
 */
int list_processes()
    {
    register struct emacs_buffer *old = bf_cur;
    register struct process_blk *p;
    unsigned char line[150];
    long sig_mask;

    scratch_bfn( u_str("process list"), interactive );
    ins_str(u_str ("Name                    Buffer			Status		   Command\n----                    ------			------		   -------\n"));
    sig_mask = sigblock(1<<SIGCHLD);
    for (p = process_list; p != NULL; p = p->next_process)
	{
	sprintfl(line, sizeof line, u_str ("%-24s"), p->p_pname);
	ins_str(line);
	sprintfl(line, sizeof line, u_str ("%-24s"), p->p_chan.ch_buffer->b_buf_name);
	ins_str(line);
	switch (p->p_flag & (STOPPED | RUNNING | EXITED | SIGNALED))
	    {
	    case STOPPED: 
		sprintfl(line, sizeof line, u_str ("%-17s"), "Stopped");
		break;
	    case RUNNING: 
		sprintfl(line, sizeof line, u_str ("%-17s"), "Running");
		break;
	    case EXITED: 
		sprintfl(line, sizeof line, u_str ("Exited %-10s"), (p->p_reason == 0 ? "" : "Abnormally"));
		flush_process(p);
		break;
	    case SIGNALED: 
		sprintfl(line, sizeof line, u_str ("%-17s"), SIG_names[p->p_reason]);
		flush_process(p);
		break;
	    default: 
		continue;
	    }
	ins_str(line);
	sprintfl(line, sizeof line, u_str ("  %-32s\n"), p->p_name);
	ins_str(line);
	}
    sigsetmask(~(1<<SIGCHLD) & sig_mask);
    bf_modified = 0;
    set_bfp(old);
    window_on(bf_cur);
    return 0;
    }
/* Send a string to the process as input */
int send_string_to_process( void )
    {
    register unsigned char *proc_name = getstr(str_process);
    register unsigned char *input_string;
    register struct process_blk *process;
    register struct wh *output;

    if ((process = find_process(proc_name)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    input_string = getstr(u_str ("String: "));
    if (input_string == 0)  
	return 0;
    output = &process->p_chan.ch_outrec;
    if (output->count || output->ccount)
	error(str_is_blocked);
    output->index = process->p_chan.ch_index;
    output->ccount = 0;
    output->count = _str_len(input_string);
    if (output->count <= 0)
	error(u_str ("Attempt to send null string to process"));
    output->data = output->buf = savestr (input_string);
    send_chan(process);
    return 0;
    }
/* 
 * Get the current output which has been thrown at us and send it
 * to the user as a string; this is only allowed if MPX_chan is non-null
 * indicating that this has been indirectly called from stuff_buffer
 */
int process_output( void )
    {
    if (MPX_chan == NULL)
	{
	error(u_str ("process-output can only be called from an process outpur procedure"));
	return 0;
	}

    ml_value->exp_type = ISSTRING;
    ml_value->exp_release = 1;
    ml_value->exp_int = MPX_chan->ch_count;
    ml_value->exp_v.v_string = malloc_ustr(ml_value->exp_int + 1);
    _str_ncpy(ml_value->exp_v.v_string, MPX_chan->ch_ptr, ml_value->exp_int);
    ml_value->exp_v.v_string[ml_value->exp_int] = '\0';
    return 0;
    }
/* Send an signal to the specified process group */
int sig_process(int signal)
    {
    register struct process_blk *process;
    register unsigned char *buf = getstr(str_process);

    if ((process = find_process(buf)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}

    /*
     * We must update the process flag explicitly in the case of continuing a 
     * process since no signal will come back
     */

    if (signal == SIGCONT)
	{
	long sig_mask;

	sig_mask = sigblock(1<<SIGCHLD);
	process->p_flag = (process->p_flag & ~STOPPED) | RUNNING;
	sigsetmask(~(1<<SIGCHLD) & sig_mask);
	}

    switch (signal)
	{
	case SIGINT: 
	case SIGQUIT:
#ifdef TIOCFLUSH
	  ioctl(process->p_chan.ch_index, TIOCFLUSH, 0);
#endif
	  process->p_chan.ch_outrec.count = 0;
	  process->p_chan.ch_outrec.ccount = 0;
	  break;
	}
#ifdef __hpux
    kill(process->p_id, signal);
#else
    killpg(process->p_id, signal);
#endif
    return 0;
    }

int force_exit_process( void )
    {
    return(sig_process(SIGINT));
    }

int quit_process()
    {
    return (sig_process(SIGQUIT));
    }

int kill_process( void )
    {
    return (sig_process(SIGKILL));
    }

int pause_process( void )
    {
    return (sig_process(SIGTSTP));
    }

int resume_process( void )
    {
    return (sig_process(SIGCONT));
    }

int send_eof_to_process(void)
    {
    register struct process_blk	*process;
    register unsigned char *buf = getstr(str_process);
    register struct wh *output;

    if ((process = find_process(buf)) == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    output = &process->p_chan.ch_outrec;
    if (output->count || output->ccount)
	error(str_is_blocked);

    output->index = process->p_chan.ch_index;
    output->count = 0;
    output->data = output->buf = NULL;
    output->ccount = 1;
    send_chan(process);

    return 0;
    }
/* Some useful functions on the process */
int current_process_name( void )
    {
    ml_value->exp_type = ISSTRING;
    ml_value->exp_v.v_string = (current_process ? current_process->p_pname : u_str (""));
    ml_value->exp_release = 0;
    ml_value->exp_int = _str_len( ml_value->exp_v.v_string );
    return 0;
    }
/*
 * Change the current-process to the one indicated.
 */
int set_current_process( void )
    {
    register unsigned char *p_name = getstr(str_process);
    register struct process_blk *process;

    process = find_process(p_name);
    if (process == NULL)
	{
	error(str_err_proc);
	return 0;
	}
    current_process = process;
    return 0;
    }
/*
 * Return the process' status
 *    -1 - not an active process
 *     0 - a stopped process
 *     1 - a running process
 */
int process_status()
    {
    register unsigned char *p_name = getstr(str_process);
    register struct process_blk	*process;

    ml_value->exp_type = ISINTEGER;
    process = find_process(p_name);
    if (process == NULL)
	ml_value->exp_int = -1;
    else
	if (process->p_flag & RUNNING)
	    ml_value->exp_int = 1;
	else
	    ml_value->exp_int = 0;
    return 0;
    }
/* Get the process id */
int process_id(void)
    {
    register unsigned char *p_name = getstr(str_process);
    register struct process_blk	*process;

    ml_value->exp_type = ISINTEGER;
    process = find_process(p_name);
    if (process == NULL)
	ml_value->exp_int = 0;
    else
	ml_value->exp_int = (int)process->p_id;
    return 0;
    }
/*
 * Initialize things on the multiplexed file.  This involves connecting the
 * standard input to a channel on the  mpx file.  Someday we will be brave and
 * close 0.
 */
void init_vms ()
	{
	signal(SIGCHLD, child_sig);
	}

int set_process_input_procedure( void )
	{
	return no_value_command();
	}

int wait_for_process_input_request( void )
	{
	return no_value_command();
	}

int vms_load_averages( void )
	{
	return no_value_command();
	}

void restore_vms( void )
	{
	signal(SIGCHLD, child_sig);
	}

int process_channel_interrupts( void )
	{
	return 0;
	}

void proc_de_ref_buf( struct emacs_buffer *PNOTUSED(b) )
	{
	return;
	}
# endif
#else
int start_dcl_process( void )
	{
	return no_value_command();
	}

int send_string_to_process( void )
	{
	return no_value_command();
	}

int set_process_termination_proc( void )
	{
	return no_value_command();
	}

int set_process_output_procedure( void )
	{
	return no_value_command();
	}

int set_process_input_procedure( void )
	{
	return no_value_command();
	}

int set_process_output_buffer( void )
	{
	return no_value_command();
	}

int kill_process( void )
	{
	return no_value_command();
	}

int set_process_name( void )
	{
	return no_value_command();
	}

int list_processes( void )
	{
	return no_value_command();
	}

int wait_for_process_input_request( void )
	{
	return no_value_command();
	}

int vms_load_averages( void )
	{
	return no_value_command();
	}

int set_current_process( void )
	{
	return no_value_command();
	}

int current_process_name( void )
	{
	return no_value_command();
	}

int process_output( void )
	{
	return no_value_command();
	}

int pause_process( void )
	{
	return no_value_command();
	}

int resume_process( void )
	{
	return no_value_command();
	}

void restore_vms( void )
	{
	return;
	}

int process_channel_interrupts( void )
	{
	return 0;
	}

int count_processes( void )
	{
	return 0;
	}
void proc_de_ref_buf( struct emacs_buffer *PNOTUSED(b) )
	{
	return;
	}

void kill_processes( void )
	{
	return;
	}

int force_exit_process( void )
	{
	return 0;
	}

void init_vms(void)
	{
	return;
	}
#endif

int ui_open_file_readonly;
unsigned char ui_open_file_name[64];
unsigned char ui_save_as_file_name[64];
unsigned char ui_filter_file_list[256];
unsigned char ui_search_string[128];
unsigned char ui_replace_string[128];
