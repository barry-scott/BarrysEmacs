$	!
$	!	mail_bld.com
$	!
$	!	make the mail interface image
$	!
$	if p1 .nes. "" then	goto	'p1
$ bli:	bliss	images$src:emacs_mail /obj=images$obj: /nolist
$ link:	link	/share=images$exe:emacs_mail_shr.exe -
		/map=images$lis:emacs_mail_shr.map -
		/notraceback /nodebug 'p2 -
			images$obj:emacs_mail, -
			sys$input/option
! options
sys$library:mail$share/share
universal=emacs$mail
gsmatch=lequal,1,0
$
