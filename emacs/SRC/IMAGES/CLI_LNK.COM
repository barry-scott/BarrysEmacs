$	link	/share=images$exe:emacs_cli_shr -
		/map=images$lis:emacs_cli_shr -
		/notraceback /nodebug 'p3 -
			images$obj:emacs_cli, -
			images$obj:cli.opt/option
$
