$	ver	= 'f$ver(0)
$	if "''base_debug'" then	set verify
$	!
$	!	cc.com
$	!
$	!	p1	- component
$	!	p2	- library
$	!	p3	- source file
$	!	p4	- optional output file name
$	!	p5	- optional qualifiers
$	!	p6	- optional include directories
$	!
$	if p4 .eqs. "-" then p4 = ""
$	source	= f$parse( "0::"+p3, "''p1'$src:.c",,,"syntax_only" ) - "0::"
$	object	= f$parse( "''p1'$obj:''p4'.obj", source )
$	anafile	= f$parse( "''p1'$obj:.ana", object )
$	if p2 .nes. "-" then -
		objlib	= f$parse( p2, "''p1'$obj:.olb",,,"syntax_only" )
$
$	on error then	exit	$status+0*f$ver(ver)
$	if hand_context_target_features .eqs. "SCA" then ana_qual = "/analysis_data='p1'$obj:"
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		define/proc/user gemc$cc disk$kits:[alpha.cc]gemc$cc
$		define/proc/user decc_message disk$kits:[alpha.cc]decc_message
$		define/proc/user target_library alpha$library:
$		if p6 .nes. "" then p6 = p6 + ","
$		if hand_context_target_type .eqs. "PRODUCTION"
$		then
$			cc	= "gemc/include=(''p6'disk$kits:[alpha.cc])/opt"
$		else
$			cc	= "gemc/include=(''p6'disk$kits:[alpha.cc])/noopt"
$		endif
$		librarian = "librarian/alpha"
$	else
$		if hand_context_target_type .eqs. "PRODUCTION"
$		then
$			cc	= "cc/decc/opt/extern=strict_refdef"
$		else
$			cc	= "cc/decc/noopt/extern=strict_refdef"
$		endif
$		if p6 .nes. "" then cc = cc + "/include=("+p6+")"
$		define/proc/user target_library vax$library:
$		librarian = "librarian"
$	endif
$	write sys$output cc," ",source," ''ana_qual'",p5
$	cc 'source' 'ana_qual' -
		/debug -
		/list='p1'$lis:'p4' -
		/object='object' -
		/diag='p1'$obj:'p4' 'p5'
$
$	if p2 .nes. "-"
$	then
$		if f$search( objlib ) .eqs. "" then librarian/create 'objlib'
$		librarian	'objlib' 'object'
$		delete	'object'*
$	endif
$	if hand_context_target_features .eqs. "SCA" -
	.and. "''parallel'" .eqs. "" -
	.and. f$search( anafile  ) .nes. ""
$	then
$		write sys$output "loading SCA file ",p3,"..."
$		sca load/lib='p1'$sca: 'p1'$obj:'p3 /log
$		dele 'p1'$obj:'p3'.ana;*
$	endif
$	exit	$status+0*f$ver(ver)
