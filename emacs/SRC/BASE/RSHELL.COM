$	!
$	!	rshell.com
$	!		- driver for rshell use into unix systems
$	!
$	!	p1..p4 - command to run
$	if hand_context_target_cpu .eqs. "ALPHA" -
	.and. hand_context_target_os .eqs. "OSF/1"
$	then
$		server	= "waster"
$		dev_username = "hand_bld"
$		work_username	= f$edit( f$getjpi(0,"username"), "trim,lowercase" )
$	endif
$	if hand_context_target_cpu .eqs. "MIPS" -
	.and. hand_context_target_os .eqs. "ULTRIX"
$	then
$		server	= "laxatv"
$		dev_username = "hand_bld"
$		work_username	= f$edit( f$getjpi(0,"username"), "trim,lowercase" )
$	endif
$
$	if hand_in_build
$	then
$		username = dev_username
$	else
$		username = work_username
$	endif
$	ctx	= f$edit( f$trnlog("hand$context"), "lowercase" )
$	write sys$output "RShell for ",username," on ",server,": ''p1' ''p2' ''p3' ''p4'"
$	define/proc/user sys$error sys$output
$	rshell/user="''username'" 'server' -
 ". .profile; hand workon emacs main ''ctx ''p1' ''p2' ''p3' ''p4'"
$	exit $status
