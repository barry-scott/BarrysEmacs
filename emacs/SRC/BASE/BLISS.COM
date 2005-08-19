$	ver	= 'f$ver(0)
$	if "''base_debug'" then	set verify
$	!
$	!	bliss.com
$	!
$	!	p1	- component
$	!	p2	- library
$	!	p3	- source file
$	!	p4	- optional output file name
$	!	p5	- optional qualifiers
$	!
$	if p4 .eqs. "-" then p4 = ""
$	source	= f$parse( "0::"+p3, "''p1'$src:.b32",,,"syntax_only" ) - "0::"
$	object	= f$parse( "''p1'$obj:''p4'.obj", source )
$	anafile	= f$parse( "''p1'$obj:.ana", object )
$	if p2 .nes. "-" then -
		objlib	= f$parse( p2, "''p1'$obj:.olb",,,"syntax_only" )
$
$	on error then	exit	$status+0*f$ver(ver)
$	if hand_context_target_features .eqs. "SCA" then ana_qual = "/analysis_data='p1'$obj:"
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		define/proc/user target_library alpha$library:
$		librarian = "librarian/alpha"
$		if hand_context_target_type .eqs. "PRODUCTION"
$		then
$			bliss = "bliss/a32/opt"
$		else
$			bliss = "bliss/a32/noopt"
$		endif
$		bliss 'source' 'ana_qual' -
			/debug -
			/check=(align,field,opt,redecl) -
			/machine -
			/list='p1'$lis:'p4' -
			/object='object' -
			/diag='p1'$obj:'p4' 'p5'
$	else
$		define/proc/user target_library vax$library:
$		librarian = "librarian"
$		bliss = "bliss"
$		bliss 'source' 'ana_qual' -
			/check=(field,initial,optimize,noredeclare) -
			/debug -
			/machine=(assembler,unique_names) -
			/optimize=(level=3,speed) -
			/list='p1'$lis:'p4' -
			/object='object' -
			/diag='p1'$obj:'p4' 'p5'
$	endif
$
$	if p2 .nes. "-"
$	then
$		if f$search( objlib ) .eqs. "" then librarian/create 'objlib'
$		librarian 'objlib' 'object'
$		delete	'object'*
$	endif
$	if hand_context_target_features .eqs. "SCA" -
	.and. "''parallel'" .eqs. "" -
	.and. f$search( anafile  ) .nes. ""
$	then
$		write sys$output "loading SCA file ",p3,"..."
$		sca load/lib='p1'$sca: 'p1'$obj:'p3 /log
$		delete 'p1'$obj:'p3'.ana;*
$	endif
$	exit	$status+0*f$ver(ver)
