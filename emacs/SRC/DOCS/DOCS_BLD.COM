$	!
$	!	docs_bld.com
$	!
$	if hand_context_target_os .eqs. "VMS" -
	.and. hand_context_target_cpu .eqs. "VAX"
$	then
$		old_ver	= f$verify(0'build_debug')
$		w	= "write sys$output
$		define	= "define"
$		set default 'f$trnlnm("docs$obj",,0)
$		@docs$src:build_user_guide
$		@docs$src:build_prog_guide
$		@docs$src:build_mlisp_ref
$		@docs$src:build_ext_ref
$		old_ver	= f$verify( old_ver )
$	else
$		copy emacs_main_vms_vax_obj_root:[obj.docs]*.* docs$obj:/log
$	endif
