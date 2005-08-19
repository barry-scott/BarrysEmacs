$	set default 'f$trnlnm("docs$obj",,0)
$	copy docs$src:emacs_prog_guide.sdml docs$obj:
$	document/noprint/contents/index/device=blank/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_prog_guide software.handbook ps
$	document/contents/index/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_prog_guide cup.online bookreader
