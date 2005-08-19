$	!
$	!	describe_bld.com
$	!
$	set noverify
$	if hand_context_target_os .eqs. "MSDOS" -
	.or. hand_context_target_os .eqs. "NT"
$	then
$		create describe$obj:filename.dat
$	endif
$	@mlisp$src:mlisp_do_file "describe" em_desc.mll emacsdesc .mll
$	@mlisp$src:mlisp_do_file "describe" qi_bliss.mll emacs_qinfo_bliss .mll
$	@mlisp$src:mlisp_do_file "describe" qi_cc.mll emacs_qinfo_vaxc .mll
$	@mlisp$src:mlisp_do_file "describe" qi_dcl.mll emacs_qinfo_dcl .mll
$	@mlisp$src:mlisp_do_file "describe" qi_vms.mll emacs_qinfo_system_services .mll
