[sys$adjstk]
SYS$ADJSTK (acmode, adjust, newadr)			     [System service]
int acmode, *newadr;
short adjust;
/* Adjusts an outer mode stack pointer */
[$adjstk]
$ADJSTK (acmode, adjust, newadr)			     [System service]
! Adjusts an outer mode stack pointer
[sys$adjwsl]
SYS$ADJWSL (pagcnt, wsetlm)				     [System service]
int pagcnt, *wsetlm;
/* Adjusts working set limit */
[$adjwsl]
$ADJWSL (pagcnt, wsetlm)				     [System service]
! Adjusts the working set limit
[sys$alloc]
SYS$ALLOC (devnam, phylen, phybuf, acmode)		     [System service]
struct dsc$descriptor *devnam, *phybuf;
int acmode;
short *phylen;
/* Allocates a device */
[$alloc]
$ALLOC (devnam, phylen, phybuf, acmode)			     [System service]
! Allocates a device
[sys$ascefc]
SYS$ASCEFC (efn, name, prot, perm)			     [System service]
int efn, prot, perm;
struct dsc$descriptor *name;
/* Associates a common event flag cluster */
[$ascefc]
$ASCEFC (efn, name, prot, perm)				     [System service]
! Associates a common event flag cluster
[sys$asctim]
SYS$ASCTIM (timlen, timbuf, timadr, cvtflg)		     [System service]
short *timlen, timadr [2], cvtflg;
struct dsc$descriptor *timbuf;
/* Converts a binary format time to an ASCII string */
[$asctim]
$ASCTIM (timlen, timbuf, timadr, cvtflg)		     [System service]
! Converts a binary format time to an ASCII string
[sys$assign]
SYS$ASSIGN (devnam, chan, acmode, mbxnam)		     [System service]
struct dsc$descriptor *devnam, *mbxnam;
short *chan;
int acmode;
/* Assigns an I/O channel */
[$assign]
$ASSIGN (devnam, chan, acmode, mbxnam)			     [System service]
! Assigns an I/O channel
[sys$bintim]
SYS$BINTIM (timbuf, timadr)				     [System service]
struct dsc$descriptor *timbuf;
int timadr [2];
/* Converts an ASCII string into a binary time value */
[$bintim]
$BINTIM (timbuf, timadr)				     [System service]
! Converts an ASCII string into a binary time va
[sys$brdcst]
SYS$BRDCST (msgbuf, devnam, flags, carcon)		     [System service]
struct dsc$descriptor *msgbuf, *devnam;
int flags, carcon;
/* Brodcasts a message to a terminal */
[$brdcst]
$BRDCST (msgbuf, devnam, flags, carcon)			     [System service]
! Brodcasts a message to a terminal
[sys$cancel]
SYS$CANCEL (chan)					     [System service]
short chan;
/* Cancels the I/O requests queue to a channel */
[$cancel]
$CANCEL (chan)						     [System service]
! Cancels the I/O requests queue to a channel
[sys$canexh]
SYS$CANEXH (desblk)					     [System service]
struct EXH *desblk;
/* Cancels an exit handler */
[$canexh]
$CANEXH (desblk)					     [System service]
! Cancels an exit handler
[sys$cantim]
SYS$CANTIM (reqidt, acmode)				     [System service]
int reqidt, acmode;
/* Cancels a timer request */
[$cantim]
$CANTIM (reqidt, acmode)				     [System service]
! Cancels a timer request
[sys$canwak]
SYS$CANWAK (pidadr, prcnam)				     [System service]
int *pidadr;
struct dsc$descriptor *prcnam;
/* Cancals a scheduled wake-up request */
[$canwak]
$CANWAK (pidadr, prcnam)				     [System service]
! Cancals a scheduled wake-up request
[sys$clref]
SYS$CLREF (efn)						     [System service]
int efn;
/* Clears an event flag */
[$clref]
$CLREF (efn)						     [System service]
! Clears an event flag
[sys$cmexec]
SYS$CMEXEC (routin, arglst)				     [System service]
int (*routin)(), *arglst;
/* Changes execution mode to Executive Mode */
[$cmexec]
$CMEXEC (routin, arglst)				     [System service]
! Changes execution mode to Executive Mode
[sys$cmkrnl]
SYS$CMKRNL (routin, arglst)				     [System service]
int (*routin)(), *arglst;
/* Changes the execution mode to kernel */
[$cmkrnl]
$CMKRNL (routin, arglst)				     [System service]
! Changes the execution mode to kernel
[sys$cntreg]
SYS$CNTREG (pagcnt, retadr, acmode, region)		     [System service]
int pagcnt, retadr [2], acmode, region;
/* Contracts the program or control region of a process */
[$cntreg]
$CNTREG (pagcnt, retadr, acmode, region)		     [System service]
! Contracts the program or control region of a process
[sys$crelog]
SYS$CRELOG (tblflg, lognam, eqlnam, acmode)		     [System service]
struct dsc$descriptor *lognam, *eqlnam;
int tblflg, acmode;
/* Creates a logical name */
[$crelog]
$CRELOG (tblflg, lognam, eqlnam, acmode)		     [System service]
! Creates a logical name
[sys$crembx]
SYS$CREMBX (prmflg, chan, maxmsg, bufquo, promsk, acmode, lognam)
int prmflg, maxmsg, bufquo, promsk, acmode;
struct dsc$descriptor lognam
short *chan;
/* Creates a mailbox and assigns a channel */
[$crembx]
$CREMBX (prmflg, chan, maxmsg, bufquo, promsk, acmode, lognam)
! Creates a mailbox and assigns a channel
[sys$creprc]
SYS$CREPRC (pidadr, image, input, output, error, prvadr, quota, prcnam, baspri,
    uic, mbxunt, stsflg)
int *pidadr, prvadr [2], *quota[], baspri, uic, stsflg;
struct dsc$desriptor *image, *input, *output, *error, *prcnam;
/* Creates a process */
[$creprc]
$CREPRC (pidadr, image, input, output, error, prvadr, quota, prcnam, baspri,
    uic, mbxunt, stsflg)
! Creates a process
[sys$cretva]
SYS$CRETVA (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Creates some virtual address space */
[$cretva]
$CRETVA (inadr, retadr, acmode)				     [System service]
! Creates some virtual address space
[sys$crmpsc]
SYS$CRMPSC (inadr, retadr, acmode, flags, gsdnam, ident, relpag, chan, pagcnt,
    vbn, prot, pfc)
int inadr [2], retadr [2], acmode, flags, ident [2], relpag, pagcnt;
int vbn, prot, pfc;
struct dsc$descriptor *dsgnam;
short chan;
/* Creates and maps a section */
[$crmpsc]
$CRMPSC (inadr, retadr, acmode, flags, gsdnam, ident, relpag, chan, pagcnt,
    vbn, prot, pfc)
! Creates and maps a section
[sys$dacefc]
SYS$DACEFC (efn)					     [System service]
int efn;
/* Disassociates from a common event flag cluster */
[$dacefc]
$DACEFC (efn)						     [System service]
! Disassociates from a common event flag cluster
[sys$dalloc]
SYS$DALLOC (devnam, acmode)				     [System service]
struct dsc$descriptor *devnam;
int acmode;
/* Deallocates a device */
[$dalloc]
$DALLOC (devnam, acmode)				     [System service]
! Deallocates a device
[sys$dassgn]
SYS$DASSGN (chan)					     [System service]
short chan;
/* Deassigns a channel */
[$dassgn]
$DASSGN (chan)						     [System service]
! Deassigns a channel
[sys$dclast]
SYS$DCLAST (astadr, astprm, acmode)			     [System service]
int (*astadr)(), *astprm, acmode;
/* Delares an AST */
[$dclast]
$DCLAST (astadr, astprm, acmode)			     [System service]
! Delares an AST
[sys$dclcmh]
SYS$DCLCMH (addres, prvhnd, type)			     [System service]
int (*addres)(), *prvhnd, type;
/* Declares a change mode or compatibility mode handler */
[$dclcmh]
$DCLCMH (addres, prvhnd, type)				     [System service]
! Declares a change mode or compatibility mode handler
[sys$dclexh]
SYS$DCLEXH (desblk)					     [System service]
struct EXH *desexh;
/* Declares an exit handler */
[$dclexh]
$DCLEXH (desblk)					     [System service]
! Declares an exit handler
[sys$dellog]
SYS$DELLOG (tblflg, lognam, acmode)			     [System service]
int tblflg, lognam;
struct dsc$descriptor *lognam;
/* Deletes a logical name */
[$dellog]
$DELLOG (tblflg, lognam, acmode)			     [System service]
! Deletes a logical name
[sys$delmbx]
SYS$DELMBX (chan)					     [System service]
short chan;
/* Deletes a mailbox */
[$delmbx]
$DELMBX (chan)						     [System service]
! Deletes a mailbox
[sys$delprc]
SYS$DELPRC (pidadr, prcnam)				     [System service]
int *pidadr;
struct dsc$descriptor *prcnam;
/* Deletes a process */
[$delprc]
$DELPRC (pidadr, prcnam)				     [System service]
! Deletes a process
[sys$deltva]
SYS$DELTVA (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Deletes some virtual address space */
[sys$deq]
SYS$DEQ (lkid, valblk, acmode, flags)			     [System service]
int lkid, valblk [], acmode, flags;
/* Dequeues a lock request */
[$deq]
$DEQ (lkid, valblk, acmode, flags)			     [System service]
! Dequeues a lock request
[$deltva]
$DELTVA (inadr, retadr, acmode)				     [System service]
! Deletes some virtual address space
[sys$dgblsc]
SYS$DGBLSC (flags, gsdnam, ident)			     [System service]
struct dsc$descriptor *gsdnam;
int flags, ident [2];
/* Deletes a global section */
[$dgblsc]
$DGBLSC (flags, gsdnam, ident)				     [System service]
! Deletes a global section
[sys$dismou]
SYS$DISMOU (devnam, flags)				     [System service]
struct dsc$descriptor *devnam;
int flags;
/* Dismounts a volume */
[$dismou]
$DISMOU (devnam, flags)					     [System service]
! Dismounts a volume
[sys$dlcefc]
SYS$DLCEFC (name)					     [System service]
struct dsc$descriptor *name;
/* Deletes a common event flag cluster */
[$dlcefc]
$DLCEFC (name)						     [System service]
! Deletes a common event flag cluster
[sys$enq]
SYS$ENQ (efn, lkmode, lksb, flags, resnam, parid, astadr,    [System service]
    astprm, blkast, acmode)
int efn, lkmode, lksb [], flags, parid;
int (*astadr)(), astprm, (*blkast)(), acmode;
struct dsc$descriptor *resnam;
/* Queues a lock request */
[$enq]
$ENQ (efn, lkmode, lksb, flags, resnam, parid, astadr,       [System service]
    astprm, blkast, acmode)
! Queues a lock request
[sys$enqw]
SYS$ENQW (efn, lkmode, lksb, flags, resnam, parid, astadr,   [System service]
    astprm, blkast, acmode)
int efn, lkmode, lksb [], flags, parid;
int (*astadr)(), astprm, (*blkast)(), acmode;
struct dsc$descriptor *resnam;
/* Queues a lock request and waits for grant */
[$enqw]
$ENQW (efn, lkmode, lksb, flags, resnam, parid, astadr,	     [System service]
    astprm, blkast, acmode)
! Queues a lock request and waits for grant
[sys$exit]
SYS$EXIT (code)						     [System service]
int code;
/* Exits an image */
[$exit]
$EXIT (code)						     [System service]
! Exits an image
[sys$expreg]
SYS$EXPREG (pagcnt, retadr, acmode, region)		     [System service]
int pagcnt, retadr [2], acmode, region;
/* Expands the control or program region */
[$expreg]
$EXPREG (pagcnt, retadr, acmode, region)		     [System service]
! Expands the control or program region
[sys$fao]
SYS$FAO (ctrstr, outlen, outbuf, p1, ...) 		     [System service]
struct dsc$descriptor *ctrstr, *outbuf;
short *outlen;
/* Fomats a control string and a set of arguments */
[$fao]
$FAO (ctrstr, outlen, outbuf, p1, ...)	 		     [System service]
! Fomats a control string and a set of arguments
[sys$faol]
SYS$FAOL (ctrstr, outlen, outbuf, prmlst)		     [System service]
struct dsc$descriptor *ctrstr, *outbuf;
int prmlst [];
short *outlen;
/* Formats a control and arguments list */
[$faol]
$FAOL (ctrstr, outlen, outbuf, prmlst)			     [System service]
! Formats a control and arguments list
[sys$forcex]
SYS$FORCEX (pidadr, prcnam, code)			     [System service]
int *pidadr; code;
struct dsc$descriptor *prcnam;
/* Forces a image to exit */
[$forcex]
$FORCEX (pidadr, prcnam, code)				     [System service]
! Forces a image to exit
[sys$getchn]
SYS$GETCHN (chan, prilen, pribuf, scdlen, scdbuf)	     [System service]
struct dsc$descriptor *pribuf, *scdbuf;
short *prilen, *scdlen, chan;
/* Gets information about an I/O channel */
[$getchn]
$GETCHN (chan, prilen, pribuf, scdlen, scdbuf)		     [System service]
! Gets information about an I/O channel
[sys$getdev]
SYS$GETDEV (devnam, prilen, pribuf, scdlen, scdbuf)	     [System service]
struct dsc$descriptor *devnam, *pribuf, *scdbuf;
short *prilen, scdlen;
/* Obtains information about a device */
[$getdev]
$GETDEV (devnam, prilen, pribuf, scdlen, scdbuf)	     [System service]
! Obtains information about a device
[sys$getdvi]
SYS$GETDVI (efn, chan, devnam, itmlst, iosb, astadr, 	     [System service]
    astprm, nullarg)
int efn, itmlst [], (*astadr)(), astprm, nullarg;
short chan, iosb [4];
struct dsc$descriptor *devnam;
/* Returns information about a device of volume */
[$getdvi]
$GETDVI (efn, chan, devnam, itmlst, iosb, astadr, 	     [System service]
    astprm, nullarg)
! Returns information about a device of volume
[sys$getjpi]
SYS$GETJPI (efn, pidadr, prcnam, itmlst, iosb, astadr,        [System service]
    astprm)
int efn, *pidadr, itmlst [], (*astadr)(), astprm;
short iosb [4];
struct dsc$descriptor *prcnam;
/* Returns information about a process */
[$getjpi]
$GETJPI (efn, pidadr, prcnam, itmlst, iosb, astadr, astprm)  [System service]
! Returns information about a process
[sys$getmsg]
SYS$GETMSG (msgid, msglen, bufadr, flags, outadr)	     [System service]
int msgid, flags;
short *msglen;
char outadr [4];
struct dsc$desctriptor *bufadr;
/* Return the text associated with a message number */
[$getmsg]
$GETMSG (msgid, msglen, bufadr, flags, outadr)		     [System service]
! Return the text associated with a message number
[sys$getsyi]
SYS$GETSYI (efn, nul1, nul2, itmlst, iosb, astadr, astprm)   [System service]
int efn, itmlst [], (*astadr)(), astprm;
short iosb [4];
/* Returns system wide information */
[$getsyi]
$GETSYI (efn, nul1, nul2, itmlst, iosb, astadr, astprm)      [System service]
! Returns system wide information
[sys$gettim]
SYS$GETTIM (timadr)					     [System service]
int timadr [2];
/* Fetch the current system time */
[$gettim]
$GETTIM (timadr)					     [System service]
! Fetch the current system time
[sys$lckpag]
SYS$LCKPAG (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Lock pages of memory */
[$lckpag]
$LCKPAG (inadr, retadr, acmode)				     [System service]
! Lock pages of memory
[sys$lkwset]
SYS$LKWSET (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Lock pages into the working set */
[$lkwset]
$LKWSET (inadr, retadr, acmode)				     [System service]
! Lock pages into the working set
[sys$mgblsc]
SYS$MGBLSC (inadr, retadr, acmode, flags, gsdnam, ident,     [System service]
    relpag)
int inadr [2], retadr [2], acmode, flags, ident [2], relpag;
struct dsc$descriptor *gsdnam;
/* Map a globl section */
[$mgblsc]
$MGBLSC (inadr, retadr, acmode, flags, gsdnam, ident,	     [System service]
    relpag)
! Map a globl section
[sys$mount]
SYS$MOUNT (itmlst)					     [System service]
int itmlst [];
/* Mounts a volume */
[$mount]
$MOUNT (itmlst)						     [System service]
! Mounts a volume
[sys$numtim]
SYS$NUMTIM (timbuf, timadr)				     [System service]
short timbuf [7];
int timadr [2];
/* Converts a binary time to a numeric time */
[$numtim]
$NUMTIM (timbuf, timadr)				     [System service]
! Converts a binary time to a numeric time
[sys$hiber]
SYS$HIBER () 						     [System service]
/* Sets the process hibernating */
[$hiber]
$HIBER () 						     [System service]
! Sets the process hibernating
[sys$purgws]
SYS$PURGWS (inadr)					     [System service]
int inadr [2];
/* Purges the working set between the specified addresses */
[$purgws]
$PURGWS (inadr)						     [System service]
! Purges the working set between the specified addresses
[sys$putmsg]
SYS$PUTMSG (msgvec, actrtn, facnam, actprm)		     [System service]
int msgvec [], (*actrtn)(), actprm;
struct dsc$descriptor *facnam;
/* Displays an error message */
[$putmsg]
$PUTMSG (msgvec, actrtn, facnam, actprm)		     [System service]
! Displays an error message
[sys$qio]
SYS$QIO (efn, chan, func, iosb, astadr, astprm,		     [System service]
    p1, p2, p3, p4, p5, p6)
int efn, func (*astadr)(), astprm;
int p1, p2, p3, p4, p5, p6;
short chan, iosb [4];
/* Queues an I/O request */
[$qio]
$QIO (efn, chan, func, iosb, astadr, astprm,		     [System service]
    p1, p2, p3, p4, p5, p6)
! Queues an I/O request
[sys$qiow]
SYS$QIOW (efn, chan, func, iosb, astadr, astprm,	     [System service]
    p1, p2, p3, p4, p5, p6)
int efn, func (*astadr)(), astprm;
int p1, p2, p3, p4, p5, p6;
short chan, iosb [4];
/* Queues an I/O request and waits for completion */
[$qiow]
$QIOW (efn, chan, func, iosb, astadr, astprm,		     [System service]
    p1, p2, p3, p4, p5, p6)
! Queues an I/O request and waits for completion
[sys$readef]
SYS$READEF (efn, state)					     [System service]
int efn, *state;
/* Reads an event flag */
[$readef]
$READEF (efn, state)					     [System service]
! Reads an event flag
[sys$resume]
SYS$RESUME (pidadr, prcnam)				     [System service]
int *pidadr;
struct dsc$descriptor *prcnam;
/* Resumes a suspended process */
[$resume]
$RESUME (pidadr, prcnam)				     [System service]
! Resumes a suspended process
[sys$schdwk]
SYS$SCHDWK (pidadr, prcnam, daytim, reptim)		     [System service]
int *pidadr, daytim [2], reptim [2];
struct dsc$descriptor *prcnam;
/* Schedules a wake-up request */
[$schdwk]
$SCHDWK (pidadr, prcnam, daytim, reptim)		     [System service]
! Schedules a wake-up request
[sys$setast]
SYS$SETAST (enbflg)					     [System service]
int enbflg;
/* Sets AST enabled/disabled */
[$setast]
$SETAST (enbflg)					     [System service]
! Sets AST enabled/disabled
[sys$setef]
SYS$SETEF (efn)						     [System service]
int efn;
/* Sets an event flag */
[$setef]
$SETEF (efn)						     [System service]
! Sets an event flag
[sys$setexv]
SYS$SETEXV (vector, addres, acmode, prvhnd)		     [System service]
int (*addres)(), *(*prvhnd)(), vector, acmode;
/* Sets an exception vector */
[$setexv]
$SETEXV (vector, addres, acmode, prvhnd)		     [System service]
! Sets an exception vector
[sys$setime]
SYS$SETIME (timadr)					     [System service]
int timadr [2];
/* Sets the system time */
[$setime]
$SETIME (timadr)					     [System service]
! Sets the system time
[sys$setimr]
SYS$SETIMR (efn, daytim, astadr, reqidt)		     [System service]
int (*astadr)(), daytim [2], astprm, efn;
/* Sets a timer */
[$setimr]
$SETIMR (efn, daytim, astadr, reqidt)			     [System service]
! Sets a timer
[sys$setpra]
SYS$SETPRA (astadr, acmode)				     [System service]
int (*astadr)(), acmode;
/* Sets a power fail recovery AST */
[$setpra]
$SETPRA (astadr, acmode)				     [System service]
! Sets a power fail recovery AST
[sys$setpri]
SYS$SETPRI (pidadr, prcnam, pri, prvpri)		     [System service]
int *pidadr, *prvpri, pri;
struct dsc$descriptor *prcnam;
/* Set a processes priority */
[$setpri]
$SETPRI (pidadr, prcnam, pri, prvpri)			     [System service]
! Set a processes priority
[sys$setprn]
SYS$SETPRN (prcnam)					     [System service]
struct dsc$descriptor *prcnam;
/* Changes the current process name */
[$setprn]
$SETPRN (prcnam)					     [System service]
! Changes the current process name
[sys$setprt]
SYS$SETPRT (inadr, retadr, acmode, prot, prvprt)	     [System service]
int inadr [2], retadr [2], acmode, prot, *prvprt;
/* Changes the protection on pages of memory */
[$setprt]
$SETPRT (inadr, retadr, acmode, prot, prvprt)		     [System service]
! Changes the protection on pages of memory
[sys$setprv]
SYS$SETPRV (enbflg, prvadr, prmflg, prvprt)		     [System service]
int prvadr [2], *prvprt, *enbflg, prmflg;
/* Sets the current process' privileges */
[$setprv]
$SETPRV (enbflg, prvadr, prmflg, prvprt)		     [System service]
! Sets the current process' privileges
[sys$setrwm]
SYS$SETRWM (watflg)					     [System service]
int watflg;
/* Sets resource wait mode */
[$setrwm]
$SETRWM (watflg)					     [System service]
! Sets resource wait mode
[sys$setsfm]
SYS$SETSFM (enbflg)					     [System service]
int enbflg;
/* Sets system service exception failure mode */
[$setsfm]
$SETSFM (enbflg)					     [System service]
! Sets system service exception failure mode
[sys$setswm]
SYS$SETSWM (swpflg)					     [System service]
int swpflg;
/* Sets process swap mode */
[$setswm]
$SETSWM (swpflg)					     [System service]
! Sets process swap mode
[sys$sndacc]
SYS$SNDACC (msgbuf, chan)				     [System service]
struct dsc$descriptor *msgbuf;
short chan;
/* Sends a message to the accounts manager */
[$sndacc]
$SNDACC (msgbuf, chan)					     [System service]
! Sends a message to the accounts manager
[sys$snderr]
SYS$SNDERR (msgbuf)					     [System service]
struct dsc$descriptor *msgbuf;
/* Sends a message to the error logger */
[$snderr]
$SNDERR (msgbuf)					     [System service]
! Sends a message to the error logger
[sys$sndopr]
SYS$SNDOPR (msgbuf, chan)				     [System service]
struct dsc$descriptor *msgbuf;
short chan;
/* Sends a message to the operator */
[$sndopr]
$SNDOPR (msgbuf, chan)					     [System service]
! Sends a message to the operator
[sys$sndsmb]
SYS$SNDSMB (msgbuf, chan)				     [System service]
struct dsc$descriptor *msgbuf;
short chan;
/* Sends a message to the symbiont manager */
[$sndsmb]
$SNDSMB (msgbuf, chan)					     [System service]
! Sends a message to the symbiont manager
[sys$suspnd]
SYS$SUSPND (pidadr, prcnam)				     [System service]
int *pidadr;
struct dc$descriptor *prcnam;
/* Suspends the designated process */
[$suspnd]
$SUSPND (pidadr, prcnam)				     [System service]
! Suspends the designated process
[sys$trnlog]
SYS$TRNLOG (lognam, rsllen, rslbuf, table, acmode, dsbmsk)   [System service]
struct dsc$descriptor *lognam, *rslbuf;
int table, acmode, dsbmsk;
short *rsllen;
/* Translates a logical name */
[$trnlog]
$TRNLOG (lognam, rsllen, rslbuf, table, acmode, dsbmsk)	     [System service]
! Translates a logical name
[sys$ulkpag]
SYS$ULKPAG (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Unlocks a page from memory */
[$ulkpag]
$ULKPAG (inadr, retadr, acmode)				     [System service]
! Unlocks a page from memory
[sys$ulwset]
SYS$ULWSET (inadr, retadr, acmode)			     [System service]
int inadr [2], retadr [2], acmode;
/* Unlocks a page from the working set */
[$ulwset]
$ULWSET (inadr, retadr, acmode)				     [System service]
! Unlocks a page from the working set
[sys$unwind]
SYS$UNWIND (depadr, newpc)				     [System service]
int *depadr (*newpc)();
/* Unwinds the call stack */
[$unwind]
$UNWIND (depadr, newpc)					     [System service]
! Unwinds the call stack
[sys$updsec]
SYS$UPDSEC (inadr, retadr, acmode, updflg, efn, iosb, 	     [System service]
    astadr, astprm)
int inadr [2], retadr [2], acmode, updflg, efn;
int (*astadr)(), astprm;
short iosb [4];
/* Updates a section file on disk */
[$updsec]
$UPDSEC (inadr, retadr, acmode, updflg, efn, iosb, 	     [System service]
    astadr, astprm)
! Updates a section file on disk
[sys$waitfr]
SYS$WAITFR (efn)					     [System service]
int efn;
/* Wait for a single event flag */
[$waitfr]
$WAITFR (efn)						     [System service]
! Wait for a single event flag
[sys$wake]
SYS$WAKE (pidadr, prcnam)				     [System service]
int *pidadr;
struct dsc$descriptor *prcnam;
/* Wake the nominated process */
[$wake]
$WAKE (pidadr, prcnam)					     [System service]
! Wake the nominated process
[sys$wfland]
SYS$WFLAND (efn, mask)					     [System service]
int efn, mask;
/* Wait for logical and of event flags */
[$wfland]
$WFLAND (efn, mask)					     [System service]
! Wait for logical and of event flags
[sys$wflor]
SYS$WFLOR (efn, mask)					     [System service]
int efn, mask;
/* Wait for logical or of event flags */
[$wflor]
$WFLOR (efn, mask)					     [System service]
! Wait for logical or of event flags */
