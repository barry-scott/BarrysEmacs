$	!
$	!	mlisp_do_file.com
$	!
$	!	p1	- component
$	!	p2	- input file
$	!	p3	- long file name
$	!	p4	- long file type
$	!
$	!
$	ver = f$verify(0)
$	p1 = f$edit(p1,"lowercase")
$	write sys$output "Database: ",p2
$	if hand_context_target_os .eqs. "VMS"
$	then
$		if p4 .eqs. ".MLL"
$		then
$			call create_tlb 'p1 'p2 'p3
$		else
$			copy 'p1'$src:'p2  'p1'$exe:'p3''p4'
$		endif
$	endif
$	if hand_context_target_os .eqs. "MSDOS" -
	.or. hand_context_target_os .eqs. "NT"
$	then
$		convert 'p1'$src:'p2 'p1'$exe:/fdl=sys$input
$deck
FILE
	ORGANIZATION		sequential

RECORD
	CARRIAGE_CONTROL	carriage_return
	FORMAT			stream
	SIZE			0
$eod
$		close/nolog filename_dat
$		open/append filename_dat 'p1'$exe:filename.dat
$		write filename.dat "''p2' ''p3'''p4'"
$		close filename_dat
$	endif
$	if hand_context_target_os .eqs. "ULTRIX" -
	.or. hand_context_target_os .eqs. "OSF/1"
$	then
$		if p4 .eqs. ".MLL"
$		then
$			p2 = f$edit(p2,"lowercase")
$			p3 = f$edit(p3,"lowercase")
$			@base$src:rshell "mlisp mlisp_do.ksh ''p1' ''p2' ''p3'"
$		else
$			input_file = "''p1'$src:''p2'"
$			output_file = "''p1'$exe:"+p3+p4
$			convert 'input_file' 'output_file' /fdl=sys$input
$deck
FILE
	ORGANIZATION		sequential

RECORD
	CARRIAGE_CONTROL	carriage_return
	FORMAT			stream_LF
	SIZE			0
$eod
$		endif
$	endif
$	if ver then set verify
$	exit
$
$ create_tlb:
$	subroutine
$	librarian/create=keysize=64 'p1'$exe:'p3' /text /log
$
$	module = ""
$	close/nolog mll_file
$	open/read mll_file 'p1'$src:'p2
$ 10$:
$	read mll_file line /end=20$
$	if f$length(line) .lt. 200 .and. line .eqs. "[" + (line - "[" - "]") + "]"
$	then
$		if module .nes. ""
$		then
$			close/nolog mll_entry
$			librarian/text 'p1'$exe:'p3' -
				'p1'$obj:entry.tmp/module="''module'"
$			delete/nolog/noconfirm 'p1'$obj:entry.tmp;*
$		endif
$		module = f$edit( line, "upcase" ) - "[" - ".ML]" - "]"
$		write sys$output "Module: ",module
$		open/write mll_entry 'p1'$obj:entry.tmp
$	else
$		write/symbol mll_entry line
$	endif
$	goto 10$
$ 20$:
$	close mll_file
$	close mll_entry
$	librarian/text 'p1'$exe:'p3' 'p1'$obj:entry.tmp/module="''module'"
$	delete/nolog/noconfirm 'p1'$obj:entry.tmp;*
$	endsubroutine
