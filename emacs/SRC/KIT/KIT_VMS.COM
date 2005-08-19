$	!
$	!	kit_bld_vms.com
$	!
$	output		== "write sys$output"
$	kit_name	="emacs_"+hand_context_target_cpu+"_X060"
$	if p1 .nes. "" then goto make_saveset
$ kit:
$ kit_a:
$	output	"Creating ''kit_name'.A..."
$	@kit$src:kit_bld_vms 'kit_name'.a kit$exe: - ! saveset A base
kit$src:kitinstal.com;
$	'p2'
$ kit_b:
$	output	"... ''kit_name'.B..."
$	@kit$src:kit_bld_vms 'kit_name'.b kit$exe: - ! saveset B base
emacsshr$src:emacsshr_lnk.com;,-
emacsshr$obj:emacsshr.obj;,emacsshr_cld.obj;,em_mess.obj;,emacsshr.opt;,-
edit$src:editlnk2.com;,-
edit$obj:edit.olb;,edit.opt;,emacs_ident.dat;,version.obj;,-
images$src:*_lnk.com;,images$obj:*.opt;,images$obj:*.obj;,-
kit$src:emacs.hlp;,-
mlisp$exe:emacs_profile.ml;,-
emacsinit.ml;,-
kit$src:emacsshut.com;,-
emacstart.com;,-
emacs_save_default_environment.com;,-
emacs_save_environment.com;,-
emacs.cld;,-
edit$obj:em_user.r32;,-
em_user.h;,-
em_user.for;,-
em_user.mar;,-
em_user.pli;,-
em_user.pas;,-
em_user.ada;,-
language$exe:emacslang.tlb;,-
mlisp$exe:emacslib.tlb;
$	'p2
$
$ kit_c:
$	output	"... ''kit_name'.C..."
$	@kit$src:kit_bld_vms 'kit_name'.c kit$exe: - ! saveset C desc
describe$exe:emacsdesc.tlb;
$	'p2
$
$ kit_d:
$	output	"... ''kit_name'.D..."
$	@kit$src:kit_bld_vms 'kit_name'.d kit$exe: - ! saveset D qinfo
describe$exe:emacs_qinfo_bliss.tlb;,-
emacs_qinfo_dcl.tlb;,-
emacs_qinfo_system_services.tlb;,-
emacs_qinfo_vaxc.tlb;
$	'p2
$
$ 	output 	"Kit build complete."
	exit
$ make_saveset:
$	!	p1 - saveset name
$	!	p2 - output to
$	!	p3 - file list
$	set file/prot=(S:RWED,O:RWED,G:RWED,W:RE) 'p3
$	backup/noassist/comment="VAX/VMS SPKITBLD Procedure"/inter/log/verify -
'p3 'p2''p1' /save/block=9000/group=25
$	exit
