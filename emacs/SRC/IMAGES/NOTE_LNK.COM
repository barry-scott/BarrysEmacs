$	if p1 .eqs. "DEBUG"
$	then
$		which	= "obj"
$		opt	= "/debug"
$	else
$		which	= "exe"
$		opt	= "/notrace/nodebug"
$	endif
$	if f$search( "target_library:notes$share.exe" ) .nes. ""
$	then
$	link	/share=images$'which':emacs_vaxnotes_shr.exe -
		/map=images$lis:emacs_vaxnotes_shr_'which'.map -
		/debug -
			images$obj:emacs_vaxnotes, -
			images$obj:notesmsg/select, -
			images$obj:notes.opt/option
$	endif
