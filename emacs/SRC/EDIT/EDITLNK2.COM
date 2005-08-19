$	link -
	/exe='p1' -
	/map='p2'/cross/full -
	/traceback 'p3 -
		edit$obj:edit/include=start, -! root module
		edit$obj:em_mess,-
		edit$obj:version, -	! version string
		edit$obj:edit/lib, -	! lib with all the obj's
		edit$obj:emacs_ident.dat/opt, -
		edit$obj:edit.opt/option
$
