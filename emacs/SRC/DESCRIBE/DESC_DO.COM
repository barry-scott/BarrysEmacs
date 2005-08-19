
$	!
$	!	describe_do_file.com
$	!
$	!	p1	- MLL file
$	!	p2	= long file name
$	!	p2	= long file type
$	!
$	write sys$output "Database: ",p1
$	if hand_context_target_os .eqs. "VMS"
$	then
$		call create_tlb 'p1 'p2
$	endif
$	if hand_context_target_os .eqs. "MSDOS" -
	.or. hand_context_target_os .eqs. "NT"
$	then
$		convert describe$src:'p1 describe$exe:/fdl=sys$input
$deck
FILE
	ORGANIZATION		sequential

RECORD
	CARRIAGE_CONTROL	carriage_return
	FORMAT			stream
	SIZE			0
$eod
$		close/nolog filename_dat
$		open/append filename_dat describe$exe:filename.dat
$		write filename.dat "''p1' ''p2'''p3'"
$		close filename_dat
$	endif
$	if hand_context_target_os .eqs. "ULTRIX" -
	.or. hand_context_target_os .eqs. "OSF/1"
$	then
$		input_file = "''p1'$obj:''p2'"
$		output_file = "''p1'$obj:"+p2
$		call create_db 'p1 'p2 'p3
$		p2 = f$edit(p2,"lowercase")
$		p3 = f$edit(p3,"lowercase")
$		@base$src:rshell "mlisp mlisp_do.ksh ''p1' ''p2' ''p3'"
$		convert describe$src:'p1 describe$obj:'p1'/fdl=sys$input
$deck
FILE
	ORGANIZATION		sequential

RECORD
	CARRIAGE_CONTROL	carriage_return
	FORMAT			stream_LF
	SIZE			0
$eod
$		p1 = f$edit(p1,"lowercase")
$		p2 = f$edit(p2,"lowercase")
$		@base$src:rshell "describe cvtmlltodb ''p1' ''p2'"
$	endif
$	exit
$
$ create_tlb:
$	subroutine
$	librarian/create=keysize=64 describe$exe:'p2' /text /log
$
$	module = ""
$	close/nolog mll_file
$	open/read mll_file describe$src:'p1
$ 10$:
$	read mll_file line /end=20$
$	if line .eqs. "[" + (line - "[" - "]") + "]"
$	then
$		if module .nes. ""
$		then
$			close/nolog mll_entry
$			librarian/text describe$exe:'p2' -
				describe$obj:entry.tmp/module="''module'"
$			delete/nolog/noconfirm describe$obj:entry.tmp;*
$		endif
$		module = line - "[" - "]"
$!		write sys$output "Module: ",module
$		open/write mll_entry describe$obj:entry.tmp
$	else
$		write mll_entry line
$	endif
$	goto 10$
$ 20$:
$	close mll_file
$	close mll_entry
$	librarian/text describe$exe:'p2' -
		describe$obj:entry.tmp/module="''module'"
$	delete/nolog/noconfirm describe$obj:entry.tmp;*
$	endsubroutine
