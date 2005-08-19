$	!
$	!	edit_make.com
$	!
$	ver = f$ver()
$	if hand_context_target_os .eqs. "ULTRIX" -
	.or. hand_context_target_os .eqs. "OSF/1"
$	then
$		if hand_context_target_type .eqs. "PRODUCTION"
$		then
$			@base$src:rshell "edit unix_mak pro"
$		else
$			@base$src:rshell "edit unix_mak dbg"
$		endif
$	endif
$	if hand_context_target_cpu .eqs. "VAX" -
	.and. hand_context_target_os .eqs. "VMS"
$	then
$		mms/noignore/description=edit$com:emacs.mms/norules/macro=("alpha=")
$	endif
$	if hand_context_target_cpu .eqs. "ALPHA" -
	.and. hand_context_target_os .eqs. "VMS"
$	then
$		mms/noignore/description=edit$com:emacs.mms/norules/macro=("alpha=/alpha")
$	endif
