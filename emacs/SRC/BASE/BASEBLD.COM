$	!
$	!	basebld.com	for Emacs
$	!
$	!	p1	- version number
$	!	p2	- operating system type - not used
$	!	p3	- optional step to continue from
$	!
$	phase	= "call do_phase"
$
$	!
$	!	P3 contains the range of steps to build.
$	!	Start with "start_step" and stop after building
$	!	"end_step". The global build_done is set true to
$	!	make steps after "end_step" do nothing.
$	!
$	!	WARNING: This mechanism does not cope with restarts!
$	!
$	build_done	== 0
$	start_step	= f$element( 0, "-", p3 ) - "-"
$	end_step	= f$element( 1, "-", p3 ) - "-"
$	!
$	!	restart on crash
$	!
$	if $restart then if batch$restart .nes. "" then goto	'batch$restart
$	!
$	!	start_step is the name of the component to continue build from
$	!
$	if start_step .nes. "" then	goto	'start_step'
$
$	!
$	!	Build the components of Emacs.
$	!
$	goto	delete		!  the default starting point
$
$ fetch:	phase	fetch		delete	src_root:[src.*]*.*;*
$		phase	fetch		hand60 fetch comp *
$ delete:	phase	delete		call delete_all
$ nodelete:
$ ident:	phase	ident		hand60 set ident *
$ emacsshr:	phase	emacsshr	"@emacsshr$com:emacsshr_bld"
$ edit:		phase	edit		"@edit$com:edit_bld"
$ images:	phase	images		"@images$com:images_bld"
$ mlisp:	phase	mlisp		"@mlisp$com:mlisp_bld"
$ describe:	phase	describe	"@describe$com:describe_bld"
$ language:	phase	language	"@language$com:lang_bld"
$!cindent:	phase	cindent		"@cindent$com:cindent_bld"
$!info:		phase	info		"@info$com:info_bld"
$!docs:		phase	docs		"@docs$com:docs_bld"
$ kit:		phase	kit		"@kit$com:kit_bld"
$
$ build_end:
$	exit
$	!
$	!	delete phase
$	!
$ delete_all:
$	subroutine
$	!
$	!	Delete all the result files from the area
$	!
$	del	= "call delete_files"
$	del	dev$sym:*.*.*
$	del	dev$exe:*.*.*
$	del	obj_root:[obj.*]*.*.*
$	del	obj_root:[lis.*]*.*.*
$	exit
$	endsubroutine
$ delete_files:
$	subroutine
$	if f$search( p1 ) .nes. "" then	delete 'p1'
$	exit
$	endsubroutine
$
$	!
$	!	Execute a phase of the build
$	!
$	!	p1	- phase name
$	!	p2,p3...- command to execute for this phase
$ do_phase:
$	subroutine	!'f$verify(0)
$	if p1 .nes. end_step .and. build_done then	exit
$	if p1 .eqs. end_step then	build_done	== 1
$	set noon
$	name = hand_context_target_os+" "+hand_context_target_cpu+" "+p1
$	set proc/name="''f$extract( 0, 15, name )'"
$	set on
$	if end_step .nes. ""
$	then
$		set restart_value='p1'-'end_step'
$	else
$		set restart_value='p1'
$	endif
$	a	= f$fao( "  Build step !AS at !%D  ", p1, 0 )
$	s1	= (80-f$length( a ))/2
$	s2	= 80-f$length( a )-s1
$	b	= f$fao( "!#*%!AS!#*%", s1, a, s2 )
$	write	sys$output	b
$	write	sys$error	b
$	start_cpu = f$getjpi(0,"cputim")
$	set verify
$	set default missing:[missing]
$	'p2' 'p3' 'p4' 'p5' 'p6' 'p7' 'p8'
$	ver	= 'f$verify(0)
$	end_cpu	= f$getjpi(0,"cputim")
$	used	= end_cpu - start_cpu
$	sec_used	= used / 100
$	frac_used	= used - (sec_used * 100)
$	a	= f$fao( "  Build step !AS used !SL.!2ZL seconds CPU time  ", p1, sec_used, frac_used )
$	s1	= (80-f$length( a ))/2
$	s2	= 80-f$length( a )-s1
$	b	= f$fao( "!#*%!AS!#*%", s1, a, s2 )
$	write	sys$output	b
$	write	sys$error	b
$	endsubroutine
