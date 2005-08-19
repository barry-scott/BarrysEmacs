$	!
$	!	emacsshr_bld.com
$	!
$	if hand_context_target_os .nes. "VMS" then exit
$
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		link = "link/alpha"
$		alpha = "/alpha"
$		define/proc target_library alpha$library:
$		create emacsshr$obj:emacsshr.opt
target_library:decc$shr.exe/share
symbol_vector=(emacs$edit=procedure,emacs$utility=procedure,emacs$emacs=procedure)
gsmatch = lequal,1,0
$	else
$		link = "link"
$		alpha = ""
$		define/proc target_library vax$library:
$		create emacsshr$obj:emacsshr.opt
target_library:decc$shr.exe/share
universal = emacs$edit,emacs$utility,emacs$emacs
gsmatch = lequal,1,0
$	endif
$ emacsshr:	@base$src:bliss emacsshr - emacsshr
$ cld:		set comm 'alpha' emacsshr$src:emacsshr.cld -
				/obj=emacsshr$obj:emacsshr_cld
$ message:	message 'alpha' /nolist edit$src:em_mess /obj=emacsshr$obj:
$
$ link:
$	@emacsshr$src:emacsshr_lnk
$	@emacsshr$src:emacsshr_lnk debug
$	exit
