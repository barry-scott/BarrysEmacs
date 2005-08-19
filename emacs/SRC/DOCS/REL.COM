$	!
$	!	emacs_rel.com
$	!
$	create		docs$obj:emacs051.rnt
$	dsrp		docs$src:emacs051	/nooutput -
			/inter=docs$obj:emacs051
$	dsrp/contents	docs$obj:emacs051	/output=docs$obj:
$	dsrp		docs$src:emacs051	/output=docs$exe:
