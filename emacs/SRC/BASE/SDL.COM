$	ver	= 'f$ver(0)
$	if "''base_debug'" then	set verify
$	!
$	!	bliss.com
$	!
$	!	p1	- component
$	!	p2	- source file
$	!	p3	- optional qualifiers
$	!
$	source	= f$parse( "0::"+p2, "''p1'$src:.sdl",,,"syntax_only" ) - "0::"
$	sdl/lang=(c='p1'$obj:,blissf='p1'$obj:,align=sys$output:) 'source
$	exit	$status + 0*f$verify(ver)
