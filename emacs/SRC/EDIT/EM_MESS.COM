$	if hand_context_target_cpu .eqs. "ALPHA"
$	then
$		qual = "/alpha"
$		val_pos = 2
$	else
$		val_pos = 1
$		qual = ""
$	endif
$	message 'qual' edit$src:em_mess/object=edit$obj:em_mess /nolist
$	analys 'qual' /object edit$obj:em_mess/output=edit$obj:em_mess.tmp
$	open/read em_mess$in edit$obj:em_mess.tmp
$	open/write em_mess$out edit$obj:em_mess.h
$
$	if qual .eqs. "/alpha" then goto 20$
$ 10$:
$	read/end=30$	em_mess$in	line
$	if f$locate( "value:", line ) .eq. f$length( line ) then goto 10$
$	value	= f$element( val_pos, " ", line )
$	read		em_mess$in	line
$	if f$locate( "symbol:", line ) .eq. f$length( line ) then goto 10$
$	symbol	= f$element( 1, """", line )
$	write em_mess$out "#define ",symbol," ",value
$	goto 10$
$ 20$:
$	read/end=30$	em_mess$in	line
$	if f$locate( "value:", line ) .eq. f$length( line ) then goto 20$
$	value	= f$element( val_pos, " ", line ) - "'"
$	read		em_mess$in	line
$	if f$locate( "symbol:", line ) .eq. f$length( line ) then goto 20$
$	symbol	= f$element( 1, """", line )
$	write em_mess$out "#define ",symbol," 0x",value
$	goto 20$
$ 30$:
$	close em_mess$in
$	close em_mess$out
