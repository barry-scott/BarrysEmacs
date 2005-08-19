$	!
$	!	edit_lnk.com
$	!
$	!	p1	- DEBUG
$	!
$	if p1 .eqs. "DEBUG"
$	then
$		image_name	= "edit$obj:emacs"
$		map_name	= "edit$lis:emacs_obj"
$		link_qual	= "/debug/symb=edit$obj:"
$	else
$		image_name	= "edit$exe:emacs"
$		map_name	= "edit$lis:emacs_exe"
$		link_qual	= "/nodebug/symb=edit$obj:"
$	endif
$	write	sys$output	"Edit Lnk: ",hand_context_target_cpu," ",p1," emacs..."
$	date = f$extract(0,11,f$time())
$	sdate = f$extract(0,7,f$time())+f$extract(9,2,f$time())
$	time = f$extract(12,5,f$time())
$	version	= "X6.0"
$	ver	= version+" linked on "+date+" at "+time+" on "+-
		f$log("sys$node")-"_"-"::"+" by "+f$getjpi(0,"USERNAME")
$	close/nolog	em$ident
$	open/write	em$ident edit$obj:emacs_ident.dat
$	write		em$ident "ident=""",version," ",sdate,""""
$	close		em$ident
$
$	emversion = "$edit$obj:emversio"
$	emversion edit$obj:version.c
$	@base$src:cc edit - edit$obj:version - /noana edit$src:
$	if p1 .nes. "" .and. f$ext( 0,1, p1 ) .nes. "/" then -
			p1	= "/" + p1
$	write sys$output "Edit Lnk: linking..."
$	define /proc /user /nolog obj edit$obj:
$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		link = "link/alpha"
$		create edit$obj:edit.opt
target_library:decc$shr.exe/share
target_library:cma$tis_shr.exe/share
target_library:lbrshr/share
target_library:decw$xmlibshr/share 
target_library:decw$dwtlibshr/share 
target_library:decw$xlibshr/share
target_library:decw$xtshr/share
target_library:decw$transport_common/share
$	else
$		link = "link"
$		create edit$obj:edit.opt
universal=term_width
target_library:decc$shr.exe/share
target_library:cma$tis_shr.exe/share
target_library:lbrshr/share
target_library:decw$xmlibshr/share 
$	endif
$
$	@edit$src:editlnk2 'image_name' 'map_name' 'link_qual'
