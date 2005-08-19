$	!
$	!		emacstart.com
$	!
$	!	Use @EMACSTART HELP to get help.
$	!
$	if f$locate( "HELP", p1 ) .ne. f$length( p1 ) then	goto	help
$	if p1 .eqs. "" then	p1	= "PRIVATE,JOB"
$	private		= f$locate( "PRIVATE", p1 ) .ne. f$length( p1 )
$	system		= f$locate( "SYSTEM", p1 ) .ne. f$length( p1 )
$	install_all	= f$locate( "INSTALL_ALL", p1 ) .ne. f$length( p1 )
$	define_job	= f$locate( "JOB", p1 ) .ne. f$length( p1 )
$	if private .and. (system .or. install_all) then	goto	50$
$
$	exec	= ""
$	table	= "LNM$PROCESS"
$	if define_job then	define	= "define/job"
$	if .not. system then goto 10$
$	required_privs = "CMKRNL,SYSNAM"
$	oldpriv = f$setprv (required_privs)
$	if .not. f$privilege (required_privs) then exit %X24
$	define	= "define/system"
$	deassign= "deassign/system"
$	table	= "LNM$SYSTEM"
$	exec	= "/exec"
$ 10$:
$	lib	= "sys$library:"
$	exe	= "sys$system:"
$	hlp	= "sys$help:"
$	shr	= "sys$share:"
$	book	= "decw$book:"
$	if system .and. p2 .eqs. "" then	goto	20$
$	default_path	= f$environment( "procedure" )
$	if p2 .eqs. "" then -
p2 = f$parse( default_path,,,"device" ) + f$parse( default_path,,,"directory" )
$	lib	= p2
$	exe	= p2
$	hlp	= p2
$	shr	= p2
$	book	= p2
$ 20$:
$
$	set noon
$	if f$trnlnm( "EMACSSHR", table ) then	deassign	emacsshr
$	set on
$	define		emacs$exe		'exe
$	define		emacs$library		'lib'
$	define		emacs$help		'hlp'
$	define		emacs$book		'book'
$	define		emacs$share		'shr'
$	define		emacs$info		'lib'
$	define		emacs$memory		sys$login:
$	define		emacs$path		sys$disk:[],emacs$library:
$	define		emacs$environment	sys$login:,emacs$library:
$	define		emacs$journal		sys$login:
$	define		emacs$checkpoint	sys$disk:[]
$	if p2 .nes. "" then	-
		define 'exec'	emacsshr	'shr'emacsshr
$	if p2 .nes. "" then	-
		define 'exec'	emacs$emacs	'exe'emacs$emacs
$
$	if private
$	then
$		set command emacs$library:emacs
$	endif
$	if f$search("sys$share:cma$tis_shr.exe") .eqs. "" -
	.and. f$search("emacs$share:cma$tis_shr.exe") .nes. "" then -
		define 'exec cma$tis_shr emacs$share:cma$tis_shr
$	if f$search("sys$share:decc$shr.exe") .eqs. "" -
	.and. f$search("emacs$share:decc$shr.exe") .nes. "" then -
		define 'exec decc$shr emacs$share:decc$shr
$	if .not. system then	exit
$
$	install	= "$install/command_mode"
$
$	index	= 0
$	images	=	"emacs$exe:emacs,force," + -
			"emacs$exe:emacs$emacs,," + -
			"emacs$share:emacsshr,force," + -
			"emacs$share:emacs_sys_shr,," + -
			"emacs$share:emacs_cms_shr,," + -
			"emacs$share:emacs_cli_shr,," + -
			"emacs$share:emacs_vaxnotes_shr,"
$ 30$:
$	image	= f$element( index, ",", images )
$	force_install	= f$element( index+1, ",", images ) .eqs. "force"
$	index	= index + 2
$	if image .eqs. "," then	goto	40$
$	if f$search( image + ".exe" ) .eqs. "" then goto 30$
$	if f$file( image + ".exe", "known" ) then	install remove 'image'
$	if force_install .or. install_all then -
		install	add 'image' /open /share
$	goto	30$
$ 40$:
$	install replace emacs$share:emacsshr
$	oldpriv = f$setprv (oldpriv)
$	exit
$ 50$:
$	type sys$input

	You cannot specify the SYSTEM or INSTALL_ALL option with PRIVATE.

$ help:
$	type	sys$input

	This command  procedure  is designed to be used to set up
	the   logical  names  that  are  required  to  run  Emacs
	successully.  For  privileged users, it will also install
	Emacs.

	The command procedure takes the following two parameters:

	P1      This  parameter is a list of keywords controlling
		the  installation  of  Emacs.  Valid keywords are
		SYSTEM, INSTALL_ALL,   PRIVATE,   JOB  and  HELP.
		Seperate keywords with commas.

		The SYSTEM  keyword  causes  system  wide logical
		names  to  be  defined.  The images EMACS.EXE and
		EMACSSHR.EXE are installed sharable.

		The INSTALL_ALL  keyword  cuases all other images
		that are part of the Emacs kit to be installed.

		The HELP keyword prints this text.

		Use the PRIVATE keyword if you do not want any of
		the above options.

		The JOB  keyword  causes  all logical names to be
		defines in the job wide.

		Default for P1 is PRIVATE,JOB.

	P2      Device  and  directory  holding  the emacs files.
		Only  required  if EMACS was not installed in the
		standard system directories.

		This parameter  defaults  to the location of this
		command file for PRIVATE installations.

Examples:

	Full Emacs  start  from  the system standard directories.
	All images are installed.

		@sys$manager:emacstart system,install_all

	Private start of Emacs.

		@disk:[emacsdir]emacstart

$	exit
