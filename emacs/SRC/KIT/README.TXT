	       Emacs Version X6.1.23
	       _____________________

Introduction

	This Emacs is copyright (c) 1991-1995 by Barry A.  Scott.
	You  can contact me electronically at the following Email
	address.

		Internet: barry@scottb.demon.co.uk

	This is a field test version of V6.1 of Emacs.

	This kit  works  on:  Windows NT, Windows 95, Windows 3.1
	with WIN32S and MSDOS.

Features of this kit.

	* Full documentation as Windows help files

	* Windows style setup installation

	* Process control for Windows NT systems

	* Print buffer works for Windows

	* Recent file support

	* Region colouring

	* Windows Emacs with new user interface

	* Save environment support

	* Windows help for emacs

	* Programmable  Windows User interface including menus and
	  certain dialogs.

	* Windows File Manager integration via DDE support

	* Syntax colouring

	* grep package

	* Syntax based searching

Kit contents

	This kit contains two versions of Emacs X6.1.23.

	\EMACS060\EMDOS32.EXE

	DOS extended Emacs.

	\EMACS060\EMWINNT.EXE

	Windows 32  bit  Emacs  with  the  new user interface and
	support for drag-and-drop.

	All these  versions  of Emacs use the same set of library
	files.

	\EMACS060\DOS

	DOS versions of the Emacs support tools.

	\EMACS060\NT_X86

	Windows NT  for  Intel  x86  version of the Emacs support
	tools.

	\EMACS060\LIBRARY

	This directory  contains all the standard MLisp libraries
	and files that are shipped with Emacs.

	\EMACS060\USERLIB

	Place your  personalised  MLisp  files in this directory.
	You will need to create this directory.

	\EMACS060\INIFILES

	Templates of the .INI files that Emacs needs are provided
	in this directory.

Installation

	For Windows  V3.1 you must first install the WIN32S V1.25
	or later extension to Windows.  Unzip the kit and run the
	setup program.

	For example:

		md c:\scratch
		pkunzip win32s11.zip c:\scratch\
		cd c:\scratch
		win setup

	Unzip the Emacs kit into an empty directory:

		md c:\scratch
		pkunzip emacsv60.zip c:\scratch

	From Windows  3.1  run c:\scratch\setup3_1 and follow the
	instructions on the screen.

	From Window  95  and  Windows NT run c:\scratch\setup and
	follow the instructions on the screen.

	If you  do not have Windows then use the DOS installation
	procedure DOSSETUP.BAT.

		c:
		cd \scratch
		DOSSETUP

	EMDOS32 requires  that  you add C:\EMACS060 to your PATH.
	This  is  required  for  EMDOS32 to find the DOS extender
	DOS4GW.

	This completes the Emacs installation.

Command line options

	Emacs supports the following command line options

	/restore=file

	Cause emacs  to  restore  the  saved environment from the
	file  named "file".  Use /restore=_nothing_ if you do not
	wish to load any saved environment.

	/package=name

	This causes   emacs   to   execute   the   MLisp  command
	"name"-com.   If "name"-com is not yet defined then emacs
	loads the MLisp module named "name".mlp.

NumLock support in EMDOS32

	Use the  NMLK_TSR  program  to map the NumLock key into a
	usable  Gold/PF1 key.  For example uses these commands in
	a .BAT file

	NMLK_TSR /K5E /C
	EMDOS32 %1 %2 %3 %4 %5
	NMLK_TSR /R

Windows Recent file support

	Call the  command  UI-add-to-recent-file-list to add file
	names to the recent file list in the File menu.  The file
	name  is  the  commands  only parameter.  The recent file
	list  is  saved  and  restored  each time emacs exits and
	startup again.

	Example:
		(UI-add-to-recent-file-list "emacsini.ml")

Region colouring support

	Emacs now  support  highlighting  text  with one of eight
	user     defined     colours.      Use     the    command
	apply-colour-to-region  to  tell  Emacs  the  region in a
	buffer to add colour to.

	Syntax: (apply-colour-to-region start end colour)

	Where the  region  covers  the characters between "start"
	and  "end"  and  will  take  on the colour "colour".  The
	valid  range  for colour is 1 to 8, which selects colours
	User  1  to  User 8.  To clear the colouring from a range
	use colour 0.

	See the  end  of  this  readme.txt file for example MLisp
	code.

Screen colouring priority

	When emacs  is drawing the characters of a buffer it uses
	the  following  priority  order when deciding what colour
	the text will be displayed in.

	1.  If  the  character  is in the region (between dot and
	mark) Then colour using the hightlight region rendition.

	2.  If  the  character  is  inside a region that has been
	coloured   with   apply-colour-to-region  then  use  that
	colour.

	3.  If   syntax  colouring  is  enabled  use  the  colour
	selected by the syntax of the character.

	4. Colour using the window graphic rendition

Associating Emacs with data files under Windows

	Emacs has  the  necessary  support  to  allow  EMWINNT to
	integrate with Window File Manager and Program manager.

	First you must register Emacs with Windows as follows:

	1.  Start the windows file manager.

	2.  Select the file C:\EMACS060\INIFILES\EMACS060.REG and
	press enter to run it.

	Windows will   then   inform  you  that  Emacs  has  been
	successfully registered.	

	Now Emacs is known to Windows and will be associated with
	files  with  the  .ML file type.  To associate Emacs with
	other file types to the following:

	1.  Start the Windows file manager

	2.  Select  a file that you wish to associate Emacs with,
	FRED.C for example.

	3.  Choose the menu item File Associate

	4.  Select  "Emacs  Edit  Text  File" from the "Associate
	with" list.

	5.  Click OK.

	6.  Repeat  steps 2 to 5 for each file type that you wish
	Emacs to be associated with.

	Double clicking any file associated with Emacs will start
	EMWINNT if is not already running.  However is EMWINNT is
	running  then the file will be opened in the exist Emacs.

Configuration of EMWINNT.EXE

	EMWINNT automatically configures itself the first time it
	is  run.   You  can  refine  the configurations using the
	dialogs.

	Options/Font...		Setup the display font
	Options/Colours...	Setup screen colours
	Options/Directories...	Setup directories

	EMWINNT reads  it  configuration  from  the  EMACS060.INI
	file.   Edit  this  file  to  change any of the following
	settings:

	Section [environment]

	Place environment  variable settings here.  These will be
	variables  that  you  want  to get the value of using the
	(getenv "xxx") command.

Configuration of EMDOS32.EXE

	EMDOS32.EXE reads  EMDOS32.INI  to  get its configuration
	settings.    EMDOS32.INI   is  looked  for  in  the  same
	directory where EMDOS32.EXE is located.

	The configuration file has three sections:

	[DeviceNames] See  "Cross systems compatibility features"
	below for details.

	Example define SYS$LOGIN:

	[DeviceNames]
	sys$login=c:\emacs060\

	[Colours] This section lists the default values for parts
	of  the  Emacs  screen.  Current the only colours defined
	are WindowText, HighlightText and ModeLine;

	For example

	[Colours]
	WindowText=1000;2000;3000;4255;5255;6255
	HighlightText=4255;1255;2255;3255
	ModeLine=6255;1255;2255

	[Environment] This    section   holds   the   values   of
	environment  variables  that  Emacs uses to customise its
	operation.

	emacs_path is  a  comma  seperated list of directories to
	search of MLisp files.

	emacs_memory is  the  filename used to save and restore
	Emacs memory information from.
	
	video_mode is  used  to  tell  Emacs  about special video
	modes  that  you  wish  to use.  Video_mode is defined as
	three  numbers.  The first is the mode to put the display
	into - see your VGA cards documentation to find out about
	the  modes  it supports.  The second number is the screen
	width in that mode and the third is the screen height.

	As an  example  the  STB  Power VGA card mode 34 puts the
	display into 132x44. 

	[Environment]
	video_mode=34 132 44

Emacs saved environment support

	If you wish to dramatically reduce the time that it takes
	to initialise Emacs use the saved environment feature.

	Emacs will automatically load a saved environment file if
	one exists.

	Create the   saved  environment  by  starting  emacs  and
	allowing it to initialise.  Then use the save-environment
	command and press return when prompted for a file name.

	Each version  of  emacs  has  its own default environment
	file. The defaults are:

	Emacs version			Save environment file
	-------------			---------------------
	emwinnt.exe on Windows NT	emacs$user:emacs_nt.env
	emwinnt.exe on Windows 95	emacs$user:emacs_95.env
	emwinnt.exe on Windows 3.1	emacs$user:emacs_32.env
	emdos32.exe			emacs$user:emacsdos.env

	emacs$user: is typically in c:\emacs060\library.

	When ever  you  change your emcasinit.ml you will need to
	rebuild  the  saved  environment  file.   You can prevent
	emacs   from   loading  the  saved  environment  file  by
	deleting it.

	If the environment file cannot be used Emacs will issue a
	warning   message   and   then  load  emacsinit.ml.   use
	save-environment to create a new environment file.

Emacs database files (.DIR, .PAG and .DAT)

	Each Emacs  database  is  stored  in a set of three files
	whos name is the name of the database and whos extentions
	are .DIR, .PAG and .DAT.

	The Emacs database commands use these database files.

	You can  maintain  these  database  files using the Emacs
	support tools.

	dbcreate db-name [-c]

	This command will create a database named 'db-name'.  Use
	the options -c parameter to force an empty database to be
	created.   Normally  dbcreate will report an error if you
	attempt to create a database that already exists.

	dbadd db-name key <input-file

	Add a  new  entry  to database 'db-name'.  The entry will
	have  the key 'key' and the contents of the entry will be
	taken  from  standard input.  Use redirection to as above
	to add the contents of a file.

	dbdel db-name key

	Delete the entry named 'key' from the database 'db-name'.

	dbprint db-name key >output-file

	Fetch the  contents  of  entry  'key'  from  the database
	'db-name'.   The  contents  of  the  entry  are output to
	standard output.  Use redirection to save the contents to
	a file, as show above.

	dblist db-name

	Print a  list  of  all  the  keys  in  the database names
	'db-name'.

	mll-2-db mll-file db-name

	This utility  will  convert  a  .MLL file into a database
	named  'db-name'.  mll-2-db expects the database to exist
	when it is run. Use dbcreate to create an empty database.

Emacs database files (.MLL and .MLI)

	Emacs no  longer  supports  .MLL  files  use  mll-2-db to
	convert  any  .MLL  files  you  have  into Emacs database
	format.

PC keyboard mapping

	Emacs emulates a Digital LK201 keyboard. 

	The key name tables for \(key) only lists the LK201 names
	at  the  moment.   The  way the keys are mapped to the PC
	keyboard is as follows:

        LK201 key       PC key
        ---------       ------
        F6              F1
                ...
        F14             F9

        HELP            F10
        DO              F11


        F17             F12
        F18             -N/A-
        F19             Scroll Lock
        F20             Pause

        PF1             NUMLOCK
        PF2             KP-/
        PF3             KP-*
        PF4             KP--

        Insert Here     INS
        Remove          DEL
        Prev            PgUp
        Next            PgDn
        Find            Home
        Select          End

Cross systems compatibility features.

	Emacs V6.1 contains special code to deal with file system
	differences.

		Logical names used as devices

	Many existing  MLisp  files expect to use file specs that
	have  VMS  style  logical  names in them.  To accommodate
	this  use  of logical names on Windows 3.1 and Windows NT
	Emacs looks up device names in EMACS060.INI.

	In section [DeviceNames] add entries for the translations
	of  your  logical names and device names.  For example to
	add   the   logical   name   emacs$src:   to   point   to
	d:\emacs060\src.

	[DeviceNames]
	emacs$src=d:\emacs060\src\

Long file name emulation on MSDOS

	The MSDOS  8.3 file name limit is another serious problem
	for  porting  MLisp  files  between  systems.  Many MLisp
	files  expect  to  be  able  to  load and open files with
	longer names.

	Emacs will map the long names used into shorter names via
	the  FILENAME.DAT  file.  Each directory can have its own
	FILENAME.DAT file.  The entries in the file list the name
	of the file as found on the disk and the long name of the
	file  that  is  used  to reference the file.  For example
	your  emacsinit.ml  file  will  be stored in a file named
	emacsini.ml on MSDOS disk.

	------------FILENAME.DAT-------------
	emacsini.ml emacsinit.ml
	-------------------------------------

Environment variables and general logical names

	The MLisp  function  getenv  will  first  lookup the name
	passed  to it in the EMACS060.INI section [Emacs].  If an
	entry  is  found  it  will be returned.  Otherwise getenv
	will  return  the value of any matching variable from the
	environment.

	The [Emacs]  section  is  used  to  greatly  simplify the
	installation and configuration of Emacs on Windows.

Example:
(defun
    (test-1
	(test-0)
	(switch-to-buffer "Test rendition regions")
	(erase-buffer)
	(insert-string " Red Green Blue ")
	
	(apply-colour-to-region 2 5 1)
	(apply-colour-to-region 6 11 2)
	(apply-colour-to-region 12 16 3)
    )
)
(defun
    (test-2
	(test-0)
	(switch-to-buffer "Test rendition regions")
	(erase-buffer)
	(insert-string " 123 ")
	
	(apply-colour-to-region 2 3 1)
	(apply-colour-to-region 3 4 2)
	(apply-colour-to-region 4 5 3)
    )
)
(defun
    (test-3
	(test-0)
	(switch-to-buffer "Test rendition regions")
	(erase-buffer)
	(insert-string " trim-this-down ")
	
	(apply-colour-to-region 2 16 1)
	(sit-for 20)
	(apply-colour-to-region 2 6 2)
	(sit-for 20)
	(apply-colour-to-region 12 16 3)
    )
)
(defun
    (test-4
	(test-0)
	(switch-to-buffer "Test rendition regions")
	(erase-buffer)
	(insert-string " make-a-hole-in-this ")
	
	(apply-colour-to-region 2 21 1)
	(sit-for 20)
	(apply-colour-to-region 6 17 2)
	(sit-for 20)
	(apply-colour-to-region 9 14 3)
    )
)
(defun
    (test-5
	(test-0)
	(switch-to-buffer "Test rendition regions")
	(erase-buffer)
	(insert-string " aaabbb ")
	
	(apply-colour-to-region 2 5 1)
	(apply-colour-to-region 5 8 2)
    )
)
(defun
    (test-0
	(switch-to-buffer "Test rendition regions")
	(apply-colour-to-region 0 999 0)
    )
)

[end readme.txt]
