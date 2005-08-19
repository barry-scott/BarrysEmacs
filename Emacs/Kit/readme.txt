	       Emacs Version @#@version@#@
	       ____________________________________________

Introduction

	Barry's Emacs is copyright (c) 1991-@#@format:%(year)s@#@ by Barry A. Scott.

	You can contact me at the following Email address.

		Internet: barry@barrys-emacs.org

	Barry's Emacs is discussed on

		http://groups.yahoo.com/group/barrysemacs.

	BArry's Emacs website is

		http://www.barrys-emacs.org/

	This is a release version of @#@format:%(maturity)s%(major)s.%(minor)s@#@ of BArry's Emacs.

	This kit works on:
		
		Windows XP
		Windows 2000
		Windows NT 4.0
		Windows Me
		Windows 98
		Linux (tested in RedHat 7.2)
		FreeBSD (tested on 4.8)
		OpenBSD (tested on 3.3)

	This kit should work on:
		Windows 95 (not tested)

New in @#@version@#@
	New in this release
	
	* Add OpenBSD as a supported operating system

	* maximum-file-read-size  variable  is checked when emacs
	  reads  a file.  If the size of the file is greater then
	  the maximum the read will fail.

	  This is  useful  for  preventing  emacs  from read very
	  large  files;  for  example log files that are 100MB in
	  size  may  exceed  memory available or just take a long
	  time to load.

	* replace  case  control  has been added to the Edit menu
	  and to the Find and Replace dialog.

	* Fix  parsing  bug in extended regular expressions, ere,
	  functions.  Repeats are now allowed after char sets and
	  groups: "[a-b]*" and "(fred|joe)*" for example now work


New in V7.2.0 Release,Win32 build 211

	* The  HTML Documentation has been updated to reflect the
	  current  state  of  Barry's  Emacs  in many areas.  The
	  MLisp  programmers  guide  has  new sections on Python,
	  database tools and ActiveX.

	* Barry's Emacs now has a FAQ. 

	* New  installer  for  Windows  (INNO  setup).   This has
	  shrunk  the  kit  size  to  approx.  2.2MB from the old
	  2.5MB.

	* The keypad can be toggled between Windows mode of LK201
	  emulation   mode.    See   documentation   on  variable
	  keyboard-emulates-lk201-keypad.    This   variable  was
	  called  keyboard-emulates-vt100-keypad  in earlier beta
	  releases  but  has  been  renamed to better reflect the
	  behaviour it controls.

	* Renamed   /norestore   to  /nowindow-restore  to  avoid
	  conflict with the other /norestore

	* BEmacs with expand wild card filenames from the command
	  line.   For  example:  bemacs  *.h will open all the .H
	  files.

	* default-emacs-init  now  arranges to add all files that
	  are opened to the recent files menu.

	* Added  new  Contrib  directory  that  will  hold useful
	  additional scripts and programs.

	* First   Contrib  is  make_bemacs_cmd.py.   This  Python
	  script  allows you to make a customised BEmacs command.
	  Run python make_bemacs_cmd.py for help info.

	* Added   BemacsWait.exe   that  is  a  customisation  of
	  BEmacs.exe with the /wait qualifier.

	* query-replace  commands now support additional commands
	  like 'y' for yes replace.

	* The  mouse package now supports defining a double click
	  action.

	* double click on a line in the grep buffer will take you
	  to the corresponding line.

	* synchronise-files  no  longer  reloads  a  file  if its
	  attributes (permissions) change.

	* incremental  search can now copy the rest of a word you
	  have found.

	* Extended regular expression searching implemented.  See
	  the  HTML  documentation  for  details  of  syntax  and
	  features (User Guide, Advanced Editing, Searching).

	* default-emacs-init  improved  to  better  windows  like
	  environment.  Key binding have been extended to improve
	  usablity  under  windows.   See  HTML documentation for
	  details the default-emacs-init function and the default
	  key bindings.

	* Ctrl-X  handling has been improved.  Ctrl-X can be used
	  as  the  Cut  function  as  well as a prefix for Ctrl-X
	  commands.

	* incremental   search   has  been  improved.   searching
	  defaults  to  the hilighted region and you can paste in
	  the  clipboard  contents  as search text.  See the HTML
	  docs for details.

	* dabbrev has changed the way it works when in the middle
	  of a word. See the dabbrev HTML docs for details.

	* hooks  have  been  added to allow customisation of file
	  loading,  saving  and  backup operations.  See docs for
	  buffer-backup-filename-hook,   buffer-choose-name-hook,
	  buffer-file-loaded-hook and buffer-saved-as-hook.

	* end-of-line  style  is  now controlled by the variables
	  override-end-of-line-style,      current-buffer-end-of-
	  line-style, and default-buffer-end-of-line-style .  The
	  old RMS variables exist for compatibility.

New in V7.1 Release build 149

	* Python-mode  support  PyChecker package via nmake check
	  call.

[end readme.txt]
