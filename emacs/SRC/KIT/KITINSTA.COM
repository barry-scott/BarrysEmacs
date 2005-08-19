$	!
$	!   EMACS X6.0 display editor
$	!
$	!
$	! Let vmsinstal clean up after control_y
$
$	on control_y then vmi$callback CONTROL_Y
$	!
$	! There should not be any warning messages. 
$	!
$	on warning then exit vmi$_failure
$	if p1 .eqs. "VMI$_INSTALL" THEN GOTO INSTALL
$	if f$extract(0,5,p1) .eqs. "HELP_" then goto 'p1
$	exit vmi$_unsupported
$
$ HELP_DESCRIBE_DB:
$	type sys$input:

	The explain  database  is used by the "describe" command.
	It  contains  descriptions of standard functions.  If you
	decide  to  not include it, the describe command will not
	work;  if  you  do  include it, it will use a few hundred
	disk blocks.

$	exit	vmi$_success
$ HELP_PS_DOCS:
$	type sys$input:

	The Emacs  kit  comes with documentation created only for
	PostScript  printers and DECwindows Bookreader.  You will
	be asked about each separately.

$	exit	vmi$_success
$ HELP_BOOK_DOCS:
$	type sys$input:

	The Emacs  kit  comes with documentation created only for
	PostScript  printers and DECwindows Bookreader.  You will
	be asked about each separately.

$	exit	vmi$_success
$ HELP_INFO_DB:
$	type sys$input:

	The info  data base is used by some of the help commands.

$	exit	vmi$_success
$ HELP_QINFO_DB:
$	type sys$input:

	The quick    info    data    base    is   used   by   the
	describe-word-in-buffer command.

$	exit	vmi$_success
$ HELP_PURGE:
$	type sys$input:

	If you answer yes, older versions of the files created by
	this   procedure  will  be  deleted.   This  may  save  a
	significant  amount  of  disk space; the tradeoff is that
	you might have some use for the older versions.  
    
	If you  are  putting  the files in a different directory,
	the  old  copies  will  not  be purged even if you answer
	"yes".

$	exit	vmi$_success
$
$ HELP_STD_DIRS:
$	type sys$input:

	The standard system directories are:-

	SYS$SYSTEM:	SYS$LIBRARY:	SYS$SHARE:
	SYS$MANAGER:	DECW$BOOK:

	Unless you  have a need to use different directories, use
	the   default   for   consistency   with   other  VAX/VMS
	layered-product installations.

$	exit	vmi$_success
$ HELP_DEV_DIR:
$	type sys$input:

	You have  chosen  to  not  use  the  default  device  and
	directory,  so  you  must  enter another location for the
	Emacs files to be placed.

$	exit	vmi$_success
$ INSTALL:
$	type sys$input: 

*********************************************************************
*								    *
*			EMACS display editor			    *
*								    *
*								    *
* June 1994					      EMACS X6.0.42 *
* From: Barry Scott         Internet mail: barry@scottb.demon.co.uk *
*********************************************************************

	Type "?"  in  response  to to any question for help about
	that question.

$	is_vax = f$ext(0,4,f$getsyi("hw_name")) .eqs. "VAX"
$	if is_vax
$	then
$		! Make sure this system has VAX VMS 5.0 or better
$		vms_version = "050"
$		vms_version_string = "5.0"
$	else
$		! Make sure this system has Alpha vms 1.0 or better
$		vms_version = "010"
$		vms_version_string = "1.0"
$	endif
$	vmi$callback CHECK_VMS_VERSION emacs$work 'vms_version
$
$	if .not. emacs$work
$	then
$		VMI$CALLBACK MESSAGE e badvms "EMACS requires version ''vms_version_string of VMS."
$		exit vmi$_failure
$	endif
$
$	say	= "write sys$output"
$
$	emacs$size		= 3133
$	emacs$size_desc		= 789
$	emacs$size_ps_docs	= 3327
$	emacs$size_book_docs	= 2166
$	emacs$size_info		= 765
$	emacs$size_qinfo	= 494
$	emacs$private_installation = f$type( emacs$private_installation ) .nes. ""
$
$	!
$	!	ask if documentation and info is to be installed
$	!
$	say "The base EMACS system requires ''emacs$size' blocks."
$	type sys$input

	The next few questions ask you to select which
	optional parts of the Emacs kit you wish to install.

$	vmi$callback ask emacs$desc -
"Install the EMACS describe database (''emacs$size_desc' blocks)" "YES" b -
"@vmi$kwd:kitinstal help_describe_db"
$	if emacs$desc then	emacs$size	= emacs$size + emacs$size_desc
$	vmi$callback ask emacs$ps_docs -
"Install the EMACS PostScript documentation (''emacs$size_ps_docs' blocks)" "YES" b -
"@vmi$kwd:kitinstal help_ps_docs"
$	if emacs$ps_docs then	emacs$size	= emacs$size + emacs$size_ps_docs
$	vmi$callback ask emacs$book_docs -
"Install the EMACS Book Reader documentation (''emacs$size_book_docs' blocks)" "YES" b -
"@vmi$kwd:kitinstal help_book_docs"
$	if emacs$book_docs then	emacs$size	= emacs$size + emacs$size_book_docs
$	vmi$callback ask emacs$info -
"Install the EMACS INFO database (''emacs$size_info' blocks)" "YES" b -
"@vmi$kwd:kitinstal help_info_db"
$	if emacs$info then	emacs$size	= emacs$size + emacs$size_info
$	vmi$callback ask emacs$qinfo -
"Install the EMACS quick INFO database (''emacs$size_qinfo' blocks)" "YES" b -
"@vmi$kwd:kitinstal help_qinfo_db"
$	if emacs$qinfo then	emacs$size	= emacs$size + emacs$size_qinfo
$
$	!
$	! Should I keep only the current version of EMACS on the system?
$	!
$	vmi$callback set purge ask "" "@vmi$kwd:kitinstal help_purge"
$	!
$	!	Ask where emacs is to be installed
$	!
$	if emacs$private_installation then	goto	10$ 
$	say ""
$	say "	EMACS  can  be  installed  on  the  system  disk  or on a"
$	say "	separate  disk,  if  the system disk does not have enough"
$	say "	disk space for ''emacs$size' blocks used by EMACS."
$	say ""
$	!	
$	!	default the answers for the location of the
$	!	emacs files from the logical EMACS$EXE
$	!
$	emacs$default	= f$log("emacs$exe")
$	emacs$answer	= "NO"
$	if emacs$default .eqs. "" then			emacs$answer	= "YES"
$	if emacs$default .eqs. "SYS$SYSTEM:" then	emacs$answer	= "YES"
$
$	vmi$callback ask emacs$work -
"Do you want to install EMACS in the standard system directories" -
	'emacs$answer' b "@vmi$kwd:kitinstal help_std_dirs"
$
$	emacs$exe	= "vmi$root:[sysexe]"
$	emacs$lib	= "vmi$root:[syslib]"
$	emacs$hlp	= "vmi$root:[syshlp]"
$	emacs$book	= "vmi$root:[decw$book]"
$	emacs$shelf	= "vmi$root:[decw$book]"
$	emacs$upd	= "vmi$root:[sysupd]"
$	emacs$mgr	= "vmi$root:[sysmgr]"
$
$	if emacs$work then	goto	50$
$
$ 10$:
$	vmi$callback ask emacs$work -
"Enter the device and directory to install EMACS in" "''emacs$default'" s -
"@vmi$kwd:kitinstal help_dev_dir"
$
$	device		= f$parse( emacs$work,,,"device" )
$	if .not. f$getdvi( device, "exists" ) -
	.or. .not. f$getdvi( device, "mnt" ) then -
		goto	20$
$	freespace	= f$getdvi( device, "freeblocks" )
$	if freespace .lt. emacs$size then	goto	30$
$	goto	40$
$ 20$:
$	say	"Device ''device' does not exist or is not mounted."
$	goto	10$
$ 30$:
$	say	"Device ''device' has ''freespace' blocks, ''emacs$size' are required."
$	goto	10$
$ 40$:
$	if f$parse( emacs$work ) .eqs. "" then -
		create/directory/log 'emacs$work'
$
$	emacs$exe	= emacs$work
$	emacs$lib	= emacs$work
$	emacs$hlp	= emacs$work
$	emacs$book	= emacs$work
$	if emacs$private_installation
$	then
$		emacs$shelf	= emacs$work
$	else
$		emacs$shelf	= "vmi$root:[decw$book]"
$	endif
$	emacs$upd	= emacs$work
$	emacs$mgr	= emacs$work
$	goto	60$
$ 50$:
$	!
$	! Make sure this system disk has enough space for EMACS.
$	!
$	vmi$callback CHECK_NET_UTILIZATION emacs$work 'emacs$size'
$	if .not. emacs$work then vmi$callback MESSAGE e nospace -
       "EMACS utilizes ''emacs$size' additional free blocks after installation."
$	if .not. emacs$work then exit vmi$_failure
$ 60$:
$
$	!
$	! Copying the files to the correct places.
$	!
$	type	sys$input:

	All of the installation questions have been answered. The installation
	will proceed automatically from here.

$	vmi$callback restore_saveset b
$	!
$	! Link the images
$	!
$	if .not. emacs$private_installation then -
		define/process sys$library vmi$root:[syslib]
$	define/process target_library sys$library:
$	define/process edit$obj vmi$kwd:
$	define/process edit$lis nl:
$	say "Linking EMACS.EXE ..."
$	@vmi$kwd:editlnk2 vmi$kwd:emacs.exe nl:
$	deassign/process edit$obj
$	deassign/process edit$lis
$
$	define/process emacsshr$obj vmi$kwd:
$	define/process emacsshr$lis nl:
$	define/process emacsshr$exe vmi$kwd:
$	say "Linking EMACSSHR.EXE and EMACS$EMACS.EXE ..."
$	@vmi$kwd:emacsshr_lnk "nomap"
$	deassign/process emacsshr$obj
$	deassign/process emacsshr$exe
$
$	define/process images$obj vmi$kwd:
$	define/process images$lis nl:
$	define/process images$exe vmi$kwd:
$	say "Linking Emacs support images..."
$	@vmi$kwd:cld_lnk
$	@vmi$kwd:cli_lnk
$	@vmi$kwd:spell_lnk
$	@vmi$kwd:sys_lnk
$	@vmi$kwd:vaxnotes_lnk
$	@vmi$kwd:cms_lnk
$	deassign/process images$obj
$	deassign/process images$lis
$	deassign/process images$exe
$
$	if .not. emacs$private_installation then -
		deassign/process sys$library
$	deassign/process target_library
$
$	!
$	!	Create the initial emacsinit environment file
$	!
$	set command vmi$kwd:emacs
$	type sys$input

	Testing the new Emacs image.

$	define/user/process	emacs$exe	vmi$kwd:
$	define/user/process	emacs$share	vmi$kwd:
$	define/user/process	emacs$help	vmi$kwd:
$	define/user/process	emacs$library	vmi$kwd:
$	define/user/process	emacs$path	vmi$kwd:
$	@vmi$kwd:emacs_save_default_environment
$	type sys$input

	Test completed sucessfully.

$
$	!
$	! ADD the images to sys$system:
$	!
$	vmi$callback provide_image emacs$work emacs.exe 'emacs$exe'
$	vmi$callback provide_image emacs$work emacs$emacs.exe 'emacs$exe'
$!!	vmi$callback provide_image emacs$work indent.exe 'emacs$exe'
$	vmi$callback provide_image emacs$work emacsshr.exe 'emacs$lib'
$!!	vmi$callback provide_image emacs$work lispindnt.exe 'emacs$exe'
$	vmi$callback provide_image emacs$work emacs_sys_shr.exe 'emacs$lib'
$	if f$search("vmi$kwd:emacs_cms_shr.exe") .nes. ""
$	then
$		say "Providing optional image EMACS_CMS_SHR.EXE"
$		vmi$callback provide_image emacs$work emacs_cms_shr.exe 'emacs$lib'
$	else
$		say "Optional image EMACS_CMS_SHR.EXE will not be provided"
$	endif
$	if f$search("vmi$kwd:emacs_cli_shr.exe") .nes. ""
$	then
$		say "Providing optional image EMACS_CLI_SHR.EXE"
$		vmi$callback provide_image emacs$work emacs_cli_shr.exe 'emacs$lib'
$	else
$		say "Optional image EMACS_CLI_SHR.EXE will not be provided"
$	endif
$	if f$search("vmi$kwd:emacs_spell_shr.exe") .nes. ""
$	then
$		say "Providing optional image EMACS_SPELL_SHR.EXE"
$		vmi$callback provide_image emacs$work emacs_spell_shr.exe 'emacs$lib'
$	else
$		say "Optional image EMACS_SPELL_SHR.EXE will not be provided"
$	endif
$	if f$search("vmi$kwd:emacs_vaxnotes_shr.exe") .nes. ""
$	then
$		say "Providing optional image EMACS_VAXNOTES_SHR.EXE"
$		vmi$callback provide_image emacs$work emacs_vaxnotes_shr.exe 'emacs$lib'
$	else
$		say "Optional image EMACS_VAXNOTES_SHR.EXE will not be provided"
$	endif
$	if f$search("vmi$kwd:emacs_print_options.exe") .nes. ""
$	then
$		say "Providing optional image EMACS_PRINT_OPTIONS.EXE"
$		vmi$callback provide_image emacs$work emacs_print_options.exe 'emacs$lib'
$	else
$		say "Optional image EMACS_PRINT_OPTIONS.EXE will not be provided"
$	endif
$	!
$	! Provide the EMACS$LIBRARY files
$	!
$		! mandatory files
$	vmi$callback provide_file emacs$work emacslib.tlb 'emacs$lib'
$	vmi$callback provide_file emacs$work emacslang.tlb 'emacs$lib'
$	vmi$callback provide_file emacs$work emacs_profile.ml 'emacs$lib'
$	vmi$callback provide_file emacs$work emacsinit.ml 'emacs$lib'
$!!	vmi$callback provide_file emacs$work emacsterm.dat 'emacs$lib'
$	vmi$callback provide_file emacs$work emacs_save_environment.com 'emacs$lib'
$	vmi$callback provide_file emacs$work emacs_save_default_environment.com 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.r32 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.h 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.for 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.mar 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.pli 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.pas 'emacs$lib'
$	vmi$callback provide_file emacs$work em_user.ada 'emacs$lib'
$	vmi$callback secure_file 'emacs$lib'emacslib.tlb "" "o:rwed,s:r,g:r,w:r"
$	vmi$callback secure_file 'emacs$lib'emacslang.tlb "" "o:rwed,s:r,g:r,w:r"
$	!
$	!	Provide the help files for notes
$	!
$!!	vmi$callback provide_file emacs$work emacs$notes_shortguide.mem 'emacs$hlp'
$!!	vmi$callback provide_file emacs$work emacs$notes_help.hlb 'emacs$hlp'
$!!	vmi$callback provide_file emacs$work emacs.cbi 'emacs$hlp'
$		! optional describe database
$	if emacs$desc
$	then
$	 vmi$callback restore_saveset c
$	 vmi$callback provide_file emacs$work emacsdesc.tlb 'emacs$lib'
$	 vmi$callback secure_file 'emacs$lib'emacsdesc.tlb "" "o:rwed,s:r,g:r,w:r"
$	endif
$		! optional quick info database
$	if emacs$qinfo
$	then
$	 vmi$callback restore_saveset d
$	 vmi$callback provide_file emacs$work emacs_qinfo_dcl.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacs_qinfo_bliss.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacs_qinfo_system_services.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacs_qinfo_vaxc.tlb 'emacs$lib'
$	 vmi$callback secure_file 'emacs$lib'emacs_qinfo_dcl.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacs_qinfo_bliss.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacs_qinfo_system_services.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacs_qinfo_vaxc.tlb "" "o:rwed,s:r,g:r,w:r"
$	endif
$		! optional documentation
$	if emacs$book_docs .and. 0
$	then
$	 !
$	 !	Find the library file and add emacs to it
$	 !
$	 say "Adding the Emacs Bookreader shelf to library.decw$bookshelf"
$	 emacs$provide_shelf	= 1
$	 emacs$line_seen		= 0
$
$	 vmi$callback find_file emacs$work 'emacs$book'library.decw$bookshelf "" s emacs$temp
$	 if emacs$temp .eqs. "S"
$	 then
$		copy emacs$work vmi$kwd:library.tmp
$	 else
$		create vmi$kwd:library.tmp
$	 endif
$
$	 close/nolog emacs$shelf_in
$	 close/nolog emacs$shelf_out
$	 open/write/error=100$ emacs$shelf_out vmi$kwd:library.decw$bookshelf
$	 open/error=100$ emacs$shelf_in vmi$kwd:library.tmp
$ 90$:	 read/end=110$/error=100$ emacs$shelf_in emacs$line
$	 say "Lib: ",emacs$line
$	 emacs$tmp = f$edit( f$element( 1, "\", emacs$line ), "upcase" )
$	 if f$locate( "EMACS", emacs$tmp ) .eq. f$length( emacs$tmp )
$	 then
$		write emacs$shelf_out emacs$line
$	 else
$		emacs$line_seen = 1
$		if emacs$tmp .eqs. "EMACS$BOOK:EMACS"
$		then
$			emacs$provide_shelf = 0
$		else
$			write/error=100$ emacs$shelf_out -
				"shelf\emacs$book:emacs\VAX Emacs X6.0"
$		endif
$	 endif
$	 goto 90$
$ 100$:
$	 x = $status
$	 say "File error on library.decw$bookshelf ",f$message(x)
$	 exit x
$ 110$:
$	 if .not. emacs$line_seen
$	 then
$		write/err=100$ emacs$shelf_out "shelf\emacs$book:emacs\VAX Emacs"
$	 endif
$	 close/nolog	emacs$shelf_out
$	 close/nolog	emacs$shelf_in
$
$	 vmi$callback restore_saveset e
$	 if emacs$provide_shelf
$	 then
$		type sys$input

	The Emacs Bookreader shelf has been added to
	DECW$BOOK:LIBRARY.DECW$BOOKSHELF. The previous
	version of this file will not be purged. You
	may wish to purge this file later.

$		vmi$callback provide_file emacs$work library.decw$bookshelf 'emacs$shelf' K
$	 endif
$	 vmi$callback provide_file emacs$work emacs.decw$bookshelf 'emacs$shelf'
$	 vmi$callback provide_file emacs$work emacs_ext_ref.decw$book 'emacs$book'
$	 vmi$callback provide_file emacs$work emacs_mlisp_ref.decw$book 'emacs$book'
$	 vmi$callback provide_file emacs$work emacs_prog_guide.decw$book 'emacs$book'
$	 vmi$callback provide_file emacs$work emacs_user_guide.decw$book 'emacs$book'
$	endif
$	if emacs$ps_docs .and. 0
$	then
$	 vmi$callback restore_saveset f
$	 vmi$callback provide_file emacs$work emacs_ext_ref.ps 'emacs$hlp'
$	 vmi$callback provide_file emacs$work emacs_mlisp_ref.ps 'emacs$hlp'
$	 vmi$callback provide_file emacs$work emacs_prog_guide.ps 'emacs$hlp'
$	 vmi$callback provide_file emacs$work emacs_user_guide.ps 'emacs$hlp'
$	endif
$		! optional info database
$	if emacs$info .and. 0
$	then
$	 vmi$callback restore_saveset g
$	 vmi$callback provide_file emacs$work emacshgat.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacsih.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacsmn.tlb 'emacs$lib'
$	 vmi$callback provide_file emacs$work emacsroot.tlb 'emacs$lib'
$	 vmi$callback secure_file 'emacs$lib'emacshgat.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacsih.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacsmn.tlb "" "o:rwed,s:r,g:r,w:r"
$	 vmi$callback secure_file 'emacs$lib'emacsroot.tlb "" "o:rwed,s:r,g:r,w:r"
$	endif
$
$	vmi$callback provide_file emacs$work emacstart.com 'emacs$mgr'
$	vmi$callback provide_file emacs$work emacsshut.com 'emacs$mgr'
$
$	!
$	!	If this is a private installation then we cannot
$	!	modify the system help file or command tables.
$	!
$	if .not. emacs$private_installation
$	then
$	 !
$	 ! Modify the system help library to include help on EMACS.
$	 !
$	 vmi$callback provide_dcl_help emacs.hlp
$	 !
$	 ! Modify the system command tables to include the verb EMACS.
$	 !
$	 vmi$callback provide_dcl_command emacs.cld
$	else
$	 !
$	 !	Just provide the help file and cld file
$	 !
$	 vmi$callback provide_file emacs$work emacs.hlp 'emacs$hlp'
$	endif
$	vmi$callback provide_file emacs$work emacs.cld 'emacs$lib'
$!!	vmi$callback provide_file emacs$work emacsinit.emacs_environment 'emacs$lib'link
$
$	type sys$input:

	The file EMACSTART.COM has been provided to set up the logical
	names  that  EMACS  uses. You should edit your startup file to
	include the supplied commands.

	For example:

$	if emacs$exe .eqs. "vmi$root:[sysexe]" -
	.and. .not. emacs$private_installation then -
		say -
"	$ @SYS$MANAGER:EMACSTART SYSTEM"
$	if emacs$exe .nes. "vmi$root:[sysexe]" -
	.and. .not. emacs$private_installation then -
		say -
"	$ @",emacs$exe,"EMACSTART SYSTEM ",emacs$exe
$	if emacs$private_installation then -
		say -
"	$ @",emacs$exe,"EMACSTART"
$	exit vmi$_success
$	!
$	!	MAIL the EMACS developers details about the installation
$	!
$	type sys$input

	Mailing the EMACS developer details about the installation to
	MARVIN::SCOTT. Ignore any error messages from VMS MAIL.

$	on warning then		exit vmi$_success
$	open/write	details		vmi$kwd:emacs060.details
$	w	= "write details"
$	w      "Time		= ",f$time()
$	w      "Node		= ",f$log("SYS$NODE")
$	w      "Cluster node	= ",f$log("SYS$CLUSTER_NODE")
$	w      "SCSnode		= ",f$getsyi("scsnode")
$	w      "VMS version	= ",f$getsyi("version")
$	w      "CPU type	= ",f$getsyi("cpu")
$	w      "Installation options."
$	w      "Private		= ",emacs$private_installation
$	w      "Desc		= ",emacs$desc
$	w      "PostScript Docs	= ",emacs$ps_docs
$	w      "book Docs	= ",emacs$book_docs
$	w      "Info		= ",emacs$info
$	w      "QInfo		= ",emacs$qinfo
$	w      "EMACS$EXE	= ",emacs$exe
$	close	details
$
$	set noon
$	if f$search("sys$library:nm_mailshr.exe") .eqs. ""
$	then
$		mail/subject="EMACS X6.0.42 installation report" -
			vmi$kwd:emacs060.details -
			43059::scott
$		if $status then exit vmi$_success
$		mail/subject="EMACS X6.0.42 installation report" -
			vmi$kwd:emacs060.details -
			1075::scott
$	else
$		mail/subject="EMACS X6.0.42 installation report" -
			vmi$kwd:emacs060.details -
			nm%43059::scott,nm%1075::scott
$	endif
$
$	exit vmi$_success
$
$ find_image_link_date:
$	subroutine
$	analyse/image 'p1 /header /output=vmi$kwd:a.a
$	search vmi$kwd:a.a "link date/time" /out=vmi$kwd:a.b
$	open xxx vmi$kwd:a.b
$	read xxx line
$	close xxx
$	pos = f$locate(":",line)
$	emacs$image_date == f$cvtime( f$edit( f$extract( pos+1,999,line ), "trim" ) )
$	endsubroutine
$
$ provide_new_image:
$	subroutine
$	call find_image_link_date vmi$kwd:'p1'
$	new_date = emacs$image_date
$	old_date = ""
$	old_file = f$search(p1,"vmi$root:[syslib];0")
$	if old_file .nes. ""
$	then
$		call find_image_link_date 'old_file
$		old_date = emacs$image_date
$	endif
$	if new_date .gts. old_date
$	then
$		if emacs$private_installation
$		then
$			say "Providing image ",emacs$lib,p1
$			vmi$callback provide_image emacs$work 'p1' 'emacs$lib
$		else
$			say "Providing image SYS$LIBRARY:",p1
$			vmi$callback provide_image emacs$work 'p1' vmi$root:[syslib]
$		endif
$	endif
$	endsubroutine
$	!
$	! end of EMACS kitinstal.com
$	!
