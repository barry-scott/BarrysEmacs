$	!
$	!		emacsshut.com
$	!
$	!	Use @EMACSSHUT to get help
$	!
$	if f$locate( "HELP", p1 ) .ne. f$length( p1 ) then	goto	help
$	system		= f$locate( "SYSTEM", p1 ) .ne. f$length( p1 )
$	set noon
$	oldpriv = ""
$	exec	= ""
$	if .not. system then	goto 20$
$	required_priv = "CMKRNL,SYSNAM"
$	oldpriv = f$setprv (required_priv)
$	if .not. f$privilege (required_priv) then exit %X24
$	deassign	= "deassign/system"
$	exec		= "/exec"
$
$	install	= "$install/command_mode"
$	index	= 0
$	images	=	"emacs$exe:emacs,emacs$share:emacsshr," + -
			"emacs$exe:emacs$emacs," + -
			"emacs$share:emacs_sys_shr," + -
			"emacs$share:emacs_cms_shr," + -
			"emacs$share:emacs_cli_shr," + -
			"emacs$share:emacs_vaxnotes_shr"
$ 10$:
$	image	= f$element( index, ",", images )
$	index	= index + 1
$	if image .eqs. "," then	goto	20$
$	if f$file( image + ".exe", "known" ) then	install remove 'image'
$	goto	10$
$ 20$:
$	deassign	emacs$exe
$	deassign	emacs$library
$	deassign	emacs$help
$	deassign	emacs$book
$	deassign	emacs$share
$	deassign	emacs$info
$	deassign	emacs$memory
$	deassign	emacs$path
$	deassign	emacs$environment
$	deassign	emacs$journal
$	deassign	emacs$checkpoint
$
$	if system then	deassign 'exec'	emacsshr
$	if system then	deassign 'exec'	emacs$emacs
$	if oldpriv .eqs. "" then	exit
$	oldpriv = f$setprv (oldpriv)
$	exit
$ help:
$	type	sys$input

	This command  procedure  is designed to be used to remove
	the   logical  names  that  are  required  to  run  Emacs
	successfully.   For   privileged   users,  it  will  also
	deinstall Emacs images.

	The command procedure takes the following parameter:

	P1      Must  by  the  word SYSTEM, or NONE. If SYSTEM is
		specified,   then   the   Emacs   image  will  be
		deinstalled,   and  the  logical  names  will  be
		removed from the system logical name table.

Example:

		@sys$manager:emacsshut system

$	exit
