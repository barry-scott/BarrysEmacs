$	!
$	!	cli_bld.com
$	!
$	!	make the DCL CLI interface image
$	!
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		link = "link/alpha"
$		alpha = "/alpha"
$		define/proc target_library alpha$library:
$	else
$		link = "link"
$		alpha = ""
$		define/proc target_library vax$library:
$	endif
$	if p1 .nes. "" then	goto	'p1
$ bli:	@base$src:bliss	images - emacs_cli
$ link:
$	if hand_context_target_cpu .eqs. "VAX"
$	then
$		create images$obj:cli.opt
! options
universal=-
	emacs$cli_define_table,-
	emacs$cli_list_tables,-
	emacs$cli$dcl_parse,-
	emacs$cli$present,-
	emacs$cli$get_value
$	else
$		create images$obj:cli.opt
symbol_vector=(-
	emacs$cli_define_table=procedure,-
	emacs$cli_list_tables=procedure,-
	emacs$cli$dcl_parse=procedure,-
	emacs$cli$present=procedure,-
	emacs$cli$get_value=procedure)
gsmatch=lequal,1,0
$	endif
$	@images$src:cli_lnk
