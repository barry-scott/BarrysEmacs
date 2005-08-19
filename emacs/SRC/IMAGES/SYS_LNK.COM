$	if p1 .eqs. "DEBUG"
$	then
$		which	= "obj"
$		opt	= "/debug"
$	else
$		which	= "exe"
$		opt	= "/notrace/nodebug"
$	endif
$	link	/share=images$'which':emacs_sys_shr -
			/map=images$lis:emacs_sys_shr_'which' 'opt' -
			images$obj:emacs_sys, -
			images$obj:emacs_sys_tables, -
			images$obj:setpriv, -
			images$obj:sys.opt/opt
$	exit
