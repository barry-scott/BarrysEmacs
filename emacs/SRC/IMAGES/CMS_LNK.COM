$	if f$search("target_library:cmsshr") .nes. ""
$	then
$	link/share=images$exe:emacs_cms_shr.exe -
		/nomap -
		/notraceback /nodebug 'p1 -
		images$obj:emacs_cms, images$obj:cms.opt/opt
$	endif
