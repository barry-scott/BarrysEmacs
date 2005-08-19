$	!
$	!	spell_bld.com
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
$	@base$src:bliss images - emacs_spell_shr
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:spell.opt
target_library:splmsgref
universal=emacs$spell
gsmatch=lequal,1,0
$	else
$		create images$obj:spell.opt
target_library:splmsgref
symbol_vector=emacs$spell=procedure
gsmatch=lequal,1,0
$	endif
$	@images$src:spell_lnk
