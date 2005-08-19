$	!
$	!	cms_bld.com
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
$	@base$src:cc images - emacs_cms - /standard=vaxc
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:cms.opt
target_library:cmsshr/share
universal=emacs$cms
gsmatch=lequal,1,0
$	else
$		create images$obj:cms.opt
target_share:cmsshr/share
symbol_vector=emacs$cms=procedure
gsmatch=lequal,1,0
$	endif
$	@images$src:cms_lnk
