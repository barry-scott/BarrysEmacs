$	!
$	!	edit_bld.com
$	!
$
$	! delete all objects and call edit_mak to do the work
$
$	delete	= "delete/log"
$	create	edit$obj:delete.tmp
$	create	edit$sca:delete.tmp
$
$	set default edit$obj
$	delete	edit$obj:*.*.*/exclude=ident.dat
$	delete	edit$sca:*.*.*/exclude=ident.dat
$
$	@edit$src:edit_mak
