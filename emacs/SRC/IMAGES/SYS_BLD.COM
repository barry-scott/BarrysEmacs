$	!
$	!	sys_bld.com
$	!
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		link = "link/alpha"
$		alpha = "/alpha"
$		macro = "macro/migrat"
$		define/proc target_library alpha$library:
$	else
$		link = "link"
$		alpha = ""
$		macro = "macro"
$		define/proc target_library vax$library:
$	endif
$	if p1 .nes. "" then	goto	'p1
$ setpriv:	macro	images$src:setpriv /obj=images$obj: /nolist 'p2
$ tables:	@base$src:bliss images - emacs_sys_tables
$ sys:		@base$src:bliss images - emacs_sys
$ link:
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:sys.opt
!
!	options
!
gsmatch=lequal,1,0
universal=-
	emacs$sys$fao,-
	emacs$sys$filescan,-
	emacs$sys$getmsg,-
	emacs$sys$setprn,-
	emacs$sys$getdvi,-
	emacs$sys$getjpi,-
	emacs$sys$getqui,-
	emacs$sys$getsyi,-
	emacs$sys$sndjbc,-
        emacs$sys$setuai,-
        emacs$sys$getuai,-
	emacs$sys$setprv
$	else
$		create images$obj:sys.opt
symbol_vector=(-
	emacs$sys$fao=procedure,-
	emacs$sys$filescan=procedure,-
	emacs$sys$getmsg=procedure,-
	emacs$sys$setprn=procedure,-
	emacs$sys$getdvi=procedure,-
	emacs$sys$getjpi=procedure,-
	emacs$sys$getqui=procedure,-
	emacs$sys$getsyi=procedure,-
	emacs$sys$sndjbc=procedure,-
        emacs$sys$setuai=procedure,-
        emacs$sys$getuai=procedure,-
	emacs$sys$setprv=procedure)
$	endif
$	@images$src:sys_lnk debug
$	@images$src:sys_lnk
