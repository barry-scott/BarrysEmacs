$	!
$	!	images_bld.com
$	!
$	if hand_context_target_os .nes. "VMS" then exit
$
$	if p2 .nes. "" then goto 'p2
$ cld:		@images$com:cld_bld
$ cli:		@images$com:cli_bld
$ mail:!	@images$com:mail_bld
$ spell:	@images$com:spell_bld
$ sys:		@images$com:sys_bld
$ notes:	@images$com:vaxnotes_bld
$ cms:		@images$com:cms_bld
