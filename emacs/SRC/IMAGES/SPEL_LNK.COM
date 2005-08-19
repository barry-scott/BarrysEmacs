$	if f$search("target_library:splmsgref.obj" ) .nes. ""
$	then 
$		link/share=images$exe:emacs_spell_shr.exe -
			/nomap -
			/notraceback /nodebug 'p1 -
			images$obj:emacs_spell_shr, images$obj:spell.opt/opt
$	endif
