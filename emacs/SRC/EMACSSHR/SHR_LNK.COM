$	!
$	!	emacsshr_lnk.com
$	!
$		if p1 .eqs. "DEBUG"
$		then
$			where	= "obj"
$			link_opt = "/debug/trace"
$		else
$			where	= "exe"
$			link_opt = "/notrace/nodebug"
$		endif
$		link	/share=emacsshr$'where':emacsshr -
			/map=emacsshr$lis:emacsshr_'where' -
			'link_opt -
			emacsshr$obj:emacsshr,-
			emacsshr$obj:emacsshr_cld,-
			emacsshr$obj:em_mess,-
			emacsshr$obj:emacsshr.opt/opt
$
$		link	/exe=emacsshr$'where':emacs$emacs -
			/map=emacsshr$lis:emacs$emacs_'where' -
			'link_opt -
			emacsshr$obj:emacsshr,-
			emacsshr$obj:emacsshr_cld,-
			emacsshr$obj:em_mess
$
