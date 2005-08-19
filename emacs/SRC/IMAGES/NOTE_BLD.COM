$	!
$	!	vaxnotes_bld.com
$	!
$	!	make the notes interface image
$	!
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		link = "link/alpha"
$		alpha = "/alpha"
$		define/proc target_library alpha$library:
$	else
$		link = "link"
$		alpha = ""
$		define/proc target_library vax$library:
$	endif
$	if p1 .nes. "" then	goto	'p1
$ hlp:	library/help/create=key=32	images$exe:emacs$notes_help -
					images$src:emacs$notes_help
$ msg:	message 'alpha'	images$src:notesmsg.msg/obj=images$obj:/nolist
$ bli:	@base$src:bliss	images - emacs_vaxnotes
$ link:
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:notes.opt
! options
target_library:notes$share/share
universal=emacs$vaxnotes
gsmatch=lequal,1,0
$	else
$		create images$obj:notes.opt
! options
target_library:notes$share/share
symbol_vector=(emacs$vaxnotes=procedure)
gsmatch=lequal,1,0
$	endif
$	@images$src:vaxnotes_lnk debug
$	@images$src:vaxnotes_lnk
