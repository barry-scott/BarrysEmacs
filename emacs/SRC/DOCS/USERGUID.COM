$	set default 'f$trnlnm("docs$obj",,0)
$	copy docs$src:emacs_user_guide.sdml docs$obj:
$	document/noprint/contents/index/device=blank/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_user_guide software.handbook ps
$	document/contents/index/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_user_guide cup.online bookreader
