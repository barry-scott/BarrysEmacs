$	!
$	!	kit_bld.com
$	!
$	purge kit$exe:*.*
$	if hand_context_target_os .eqs. "VMS"
$	then
$		@kit$src:kit_bld_vms
$	endif
$	if hand_context_target_os .eqs. "ULTRIX"
$	then
$		@base$src:rshell "kit kit_bld_ultrix.ksh"
$	endif
$	if hand_context_target_os .eqs. "OSF/1"
$	then
$		@base$src:rshell "kit kit_bld_osf1.ksh"
$	endif
