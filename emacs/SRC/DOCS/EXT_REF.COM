$	set default 'f$trnlnm("docs$obj",,0)
$	copy docs$src:emacs_ext_ref.sdml docs$obj:
$	document/noprint/contents/index/device=blank/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_ext_ref software.reference ps
$	document/contents/index/symbols=docs$src:emacs_symbols -
		docs$obj:emacs_ext_ref cup.online bookreader
