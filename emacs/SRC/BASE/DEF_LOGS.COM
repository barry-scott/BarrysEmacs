$	define				emacs$bin	src_root:[bin]
$	'define'_vms_vax_specific	target_library	vax$library:
$	'define'_vms_alpha_specific	target_library	alpha$library:
