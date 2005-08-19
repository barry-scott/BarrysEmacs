$	!
$	!	cld_bld.com
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
$ cld:	set command 'alpha'	images$src:emacs_print/obj=images$obj: /nolist
$ link:
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:cld.opt
universal=print_options
$	else
$		create images$obj:cld.opt
symbol_vector=(print_options=data)
$	endif
$	@images$src:cld_lnk
