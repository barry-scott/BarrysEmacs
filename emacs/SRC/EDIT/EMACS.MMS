!
!	Make file for EMacs EDIT component
!
bliss_flags = /debug
cc_flags = /machine

.first
	if hand_context_target_cpu .eqs. "VAX" then define/process target_library vax$library:
	if hand_context_target_cpu .eqs. "ALPHA" then define/process target_library alpha$library:
	if hand_context_target_features .eqs. "SCA" .and. f$search( f$parse( "edit$sca:sca$event.dat" ) ) .eqs. "" then sca create library edit$sca:
	if hand_context_target_features .eqs. "SCA" then sca set library edit$sca:

editor : edit$obj:emacs.exe  edit$exe:emacs.exe edit$obj:em_user.mar
	write sys$output "Done"

edit$obj:em_user.mar : edit$src:em_user.sdl
	sdl	edit$src:em_user -	! for the kit
		/language=(fortran=edit$obj:,macro=edit$obj:,-
			pli=edit$obj:,pascal=edit$obj:,ada=edit$obj:)

edit$obj:tt.r32 : edit$src:tt.sdl
	@base$src:sdl edit tt

edit$obj:em_user.h : edit$src:em_user.sdl
	@base$src:sdl edit em_user

edit$obj:edit.olb(start) : edit$src:start.b32 edit$obj:tt.r32
	@base$src:bliss edit edit start - $(BLISS_flags)

edit$obj:edit.olb(term_io) : edit$src:term_io.b32 edit$obj:tt.r32
	@base$src:bliss edit edit term_io - $(BLISS_flags)

edit$obj:edit.olb(vms_rtl) : edit$src:vms_rtl.b32 edit$obj:tt.r32
	@base$src:bliss edit edit vms_rtl - $(BLISS_flags)

edit$obj:edit.olb(abbrev) : edit$src:abbrev.c
	@base$src:cc edit edit abbrev - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(abspath) : edit$src:abspath.b32
	@base$src:bliss edit edit abspath - $(BLISS_flags)

edit$obj:edit.olb(arith) : edit$src:arith.c
	@base$src:cc edit edit arith - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(buffer) : edit$src:buffer.c
	@base$src:cc edit edit buffer - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(caseconv) : edit$src:caseconv.c
	@base$src:cc edit edit caseconv - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(columns) : edit$src:columns.c
	@base$src:cc edit edit columns - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(dbman) : edit$src:dbman.c edit$src:ndbm.h
	@base$src:cc edit edit dbman - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(display) : edit$src:display.c edit$src:tt.h
	@base$src:cc edit edit display - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(edoprint) : edit$src:edoprint.c edit$src:doprint.c
	@base$src:cc edit edit edoprint - $(cc_flags) edit$src:,edit$obj:

edit$obj:tdoprint.obj : edit$src:tdoprint.c edit$src:doprint.c
	@base$src:cc edit - tdoprint - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(emacs) : edit$src:emacs.c
	@base$src:cc edit edit emacs - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(em_time) : edit$src:em_time.c edit$src:em_timer.h
	@base$src:cc edit edit em_time - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(emacsrtl) : edit$src:emacsrtl.c
	@base$src:cc edit edit emacsrtl - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(errlog) : edit$src:errlog.c
	@base$src:cc edit edit errlog - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(fileio) : edit$src:fileio.c
	@base$src:cc edit edit fileio - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(fileserv) : edit$src:fileserv.c
	@base$src:cc edit edit fileserv - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(fio) : edit$src:fio.b32
	@base$src:bliss edit edit fio - $(bliss_flags)

edit$obj:edit.olb(function) : edit$src:function.c edit$src:funclist.h
	@base$src:cc edit edit function - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(glob_var) : edit$src:glob_var.c
	@base$src:cc edit edit glob_var - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(journal) : edit$src:journal.c
	@base$src:cc edit edit journal - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(keyboard) : edit$src:keyboard.c
	@base$src:cc edit edit keyboard - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(lispfunc) : edit$src:lispfunc.c
	@base$src:cc edit edit lispfunc - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(macros) : edit$src:macros.c
	@base$src:cc edit edit macros - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(mem_man) : edit$src:mem_man.c
	@base$src:cc edit edit mem_man - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(membegin) : edit$src:membegin.c
	@base$src:cc edit edit membegin - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(memend) : edit$src:memend.c
	@base$src:cc edit edit memend - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(metacomm) : edit$src:metacomm.c
	@base$src:cc edit edit metacomm - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(minibuf) : edit$src:minibuf.c
	@base$src:cc edit edit minibuf - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(mlispars) : edit$src:mlispars.c
	@base$src:cc edit edit mlispars - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(mlisproc) : edit$src:mlisproc.c
	@base$src:cc edit edit mlisproc - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(vms_ndbm) : edit$src:vms_ndbm.b32
	@base$src:bliss edit edit vms_ndbm - $(BLISS_flags)

edit$obj:edit.olb(options) : edit$src:options.c edit$obj:em_user.h
	@base$src:cc edit edit options - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(osfile) : edit$src:osfile.c
	@base$src:cc edit edit osfile - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(osinter) : edit$src:osinter.c
	@base$src:cc edit edit osinter - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(profile) : edit$src:profile.c
	@base$src:cc edit edit profile - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(prompt) : edit$src:prompt.c edit$src:ndbm.h
	@base$src:cc edit edit prompt - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(queue) : edit$src:queue.c
	@base$src:cc edit edit queue - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(search) : edit$src:search.c
	@base$src:cc edit edit search - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(searchin) : edit$src:searchin.c
	@base$src:cc edit edit searchin - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(simpcomm) : edit$src:simpcomm.c
	@base$src:cc edit edit simpcomm - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(subproc) : edit$src:subproc.c
	@base$src:cc edit edit subproc - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(syntax) : edit$src:syntax.c
	@base$src:cc edit edit syntax - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(term) : edit$src:term.c edit$src:term.h edit$src:tt.h
	@base$src:cc edit edit term - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(timer) : edit$src:timer.c edit$src:em_timer.h
	@base$src:cc edit edit timer - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(trm_ansi) : edit$src:trm_ansi.c edit$src:term.h edit$src:tt.h
	@base$src:cc edit edit trm_ansi - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(motiftrm) : edit$src:motiftrm.c
	@base$src:cc edit edit motiftrm - $(cc_flags)/standard=relaxed_ansi89 edit$src:,edit$obj:

edit$obj:edit.olb(motiftk) : edit$src:motiftk.c
	@base$src:cc edit edit motiftk - $(cc_flags)/standard=relaxed_ansi89 edit$src:,edit$obj:

edit$obj:edit.olb(motifcmd) : edit$src:motifcmd.c
	@base$src:cc edit edit motifcmd - $(cc_flags)/standard=relaxed_ansi89 edit$src:,edit$obj:

edit$obj:xwin_trm.obj : edit$src:xwin_trm.c edit$src:term.h edit$src:tt.h
	@base$src:cc edit - xwin_trm - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(undo) : edit$src:undo.c
	@base$src:cc edit edit undo - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(variable) : edit$src:variable.c edit$src:varlist.h
	@base$src:cc edit edit variable - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(varthunk) : edit$src:varthunk.c
	@base$src:cc edit edit varthunk - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(vmsabs) : edit$src:vmsabs.c
	@base$src:cc edit edit vmsabs - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(vms_comm) : edit$src:vms_comm.c
	@base$src:cc edit edit vms_comm - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(windman) : edit$src:windman.c
	@base$src:cc edit edit windman - $(cc_flags) edit$src:,edit$obj:

edit$obj:edit.olb(window) : edit$src:window.c
	@base$src:cc edit edit window - $(cc_flags) edit$src:,edit$obj:

edit$obj:em_mess.obj : edit$src:em_mess.msg
	@edit$src:em_mess

edit$obj:emacs$display_ansi.exe				: -
		edit$src:ansi_lnk.com -		
		edit$obj:tdoprint.obj -
		edit$obj:term_io.obj -
		edit$obj:trm_ansi.obj
	@edit$src:ansi_lnk.com
		
edit$obj:emacs$display_xwindow.exe				: -
		edit$src:xwin_lnk.com -		
		edit$obj:tdoprint.obj -
		edit$obj:xwin_trm.obj
	@edit$src:xwin_lnk.com
		
edit$obj:emversio.exe	: edit$obj:emversio.obj
	link/exe=edit$obj:emversio.exe edit$obj:emversio

edit$obj:emversio.obj : edit$src:emversio.c edit$src:os.h
	cc/decc edit$src:emversio.c /include=edit$src: /nolist/obj=edit$obj:emversio /debug

edit$obj:emacs.exe				: -
		edit$obj:emversio.exe -
		edit$obj:em_mess.obj -
		edit$obj:edit.olb(start) -
		edit$obj:edit.olb(vms_rtl) -
		edit$obj:edit.olb(vms_comm) -
		edit$obj:edit.olb(abbrev) -
		edit$obj:edit.olb(abspath) -
		edit$obj:edit.olb(arith) -
		edit$obj:edit.olb(buffer) -
		edit$obj:edit.olb(caseconv) -
		edit$obj:edit.olb(columns) -
		edit$obj:edit.olb(dbman) -
		edit$obj:edit.olb(display) -
		edit$obj:edit.olb(edoprint) -
		edit$obj:edit.olb(emacs) -
		edit$obj:edit.olb(emacsrtl) -
		edit$obj:edit.olb(em_time) -
		edit$obj:edit.olb(errlog) -
		edit$obj:edit.olb(fileio) -
		edit$obj:edit.olb(fio) -
		edit$obj:edit.olb(function) -
		edit$obj:edit.olb(glob_var) -
		edit$obj:edit.olb(keyboard) -
		edit$obj:edit.olb(lispfunc) -
		edit$obj:edit.olb(macros) -
		edit$obj:edit.olb(mem_man) -
		edit$obj:edit.olb(membegin) -
		edit$obj:edit.olb(memend) -
		edit$obj:edit.olb(metacomm) -
		edit$obj:edit.olb(minibuf) -
		edit$obj:edit.olb(mlispars) -
		edit$obj:edit.olb(mlisproc) -
		edit$obj:edit.olb(vms_ndbm) -
		edit$obj:edit.olb(options) -
		edit$obj:edit.olb(queue) -
		edit$obj:edit.olb(search) -
		edit$obj:edit.olb(searchin) -
		edit$obj:edit.olb(simpcomm) -
		edit$obj:edit.olb(subproc) -
		edit$obj:edit.olb(syntax) -
		edit$obj:edit.olb(term) -
		edit$obj:edit.olb(timer) -
		edit$obj:edit.olb(undo) -
		edit$obj:edit.olb(variable) -
		edit$obj:edit.olb(varthunk) -
		edit$obj:edit.olb(windman) -
		edit$obj:edit.olb(window) -
		edit$obj:edit.olb(prompt) -
		edit$obj:edit.olb(journal) -
		edit$obj:edit.olb(fileserv) -
		edit$obj:edit.olb(term_io) -
		edit$obj:edit.olb(trm_ansi) -
		edit$obj:edit.olb(motiftk) -
		edit$obj:edit.olb(motiftrm) -
		edit$obj:edit.olb(motifcmd) -
		edit$src:edit_lnk.com
	@edit$com:edit_lnk debug

edit$exe:emacs.exe : edit$obj:emacs.exe
	@edit$com:edit_lnk
