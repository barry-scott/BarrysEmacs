$	!
$	!	mlisp_bld.com
$	!
$	set noverify
$	if hand_context_target_os .eqs. "MSDOS" -
	.or. hand_context_target_os .eqs. "NT"
$	then
$		create mlisp$obj:filename.dat
$	endif
$	@mlisp$src:mlisp_do_file mlisp emacslib.mll emacslib .mll
$	@mlisp$src:mlisp_do_file mlisp profile.ml emacs_profile .ml
$	@mlisp$src:mlisp_do_file mlisp emacsini.ml emacsinit .ml
