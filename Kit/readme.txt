	       Emacs Version @#@version@#@
	       ____________________________________________

Introduction

	Barry's Emacs is copyright (c) 1991-@#@format:%(year)s@#@ by Barry A. Scott.

	You can contact me at the following Email address.

		Internet: barry@barrys-emacs.org

	Barry's Emacs is discussed on

		http://groups.yahoo.com/group/barrysemacs.

	Barry's Emacs website is

		http://www.barrys-emacs.org/

	This is a release version of @#@format:%(maturity)s%(major)s.%(minor)s@#@ of Barry's Emacs.

	This kit works on:
		Windows XP
		Windows 2000
		Windows NT 4.0
		Windows Me
		Windows 98
		Linux (tested on: RedHat 7.2, RedHat 9.0 and Mandrake 9.2)
		FreeBSD (tested on 4.9)
		OpenBSD (tested on 3.3)

	This kit should work on:
		Windows 95 (not tested)

New in @#@version@#@
	New in this release

	* XML-mode,  a  simple  mode  that colours XML files with
	  forward and backward balenced tag finding.

	* query  replace  functions  default the search string to
	  the    region    if   it   is   set.    Controlled   by
	  query-replace-use-region-for-search,  that  defaults to
	  1.

	  To  replace  foo  with bar copy "bar" to the clipboard,
	  highlight  the  first  "foo"  to  replace  and run your
	  favorite query-replace, ESC-q.

	* stack-maximum-depth   variable  added  to  control  the
	  number of nested MLisp calls.  Prevents MLisp code with
	  recursion errors from crashing Emacs.

	* Fix problem with LF files being detected as UNKN if the
	  first char was an LF.

	* Filenames  on  Unix where compared case blind, fixed to
	  be case sensitive.

	* Update python support to python 2.3.

	* Updated   Python-mode  with  Python  2.3  keywords  and
	  builtins.

	* TAB  and  Shift-TAB  ,and  ESC-TAB, will now indent and
	  undent  the  region  when it is set.  Controlled by the
	  tab-indents-region variable.

	* backup  files  on unix default to traditional filename~
	  format.

	* Support  multiple  selections under X11.  UI-edit-paste
	  and  UI-edit-copy  take  one  optional string parameter
	  with  the name of the selection: "primary", "secondary"
	  and  "clipboard".   "clipboard"  is  the  default.  The
	  middle mouse button will paste the "primary" selection.

	* to-col will only insert tab characters if
          indent-use-tab is 1.

	* Fix  problem with ere-query-replace-string with strings
	  like "\\n" inserting a LF not a \ and a n.

	* Add OpenBSD as a supported operating system

	* Shift-TAB can be bound as ESC-\t.

	* Fixed crash on multi processor systems.

	* Right  clicking  with  the  mouse will now set the dot.
	  This  makes  right  clicking  and  selecting Paste work
	  intuitively.

	* maximum-file-read-size  variable  is checked when emacs
	  reads  a file.  If the size of the file is greater then
	  the maximum the file will not be read in.

	  This is  useful  for  preventing  emacs  from read very
	  large  files;  for  example log files that are 100MB in
	  size  may  exceed  memory available or just take a long
	  time to load.

	* fixed  a bug that crashed emacs when attempting to read
	  files  that  are  very  large.  Emacs will now read the
	  file   if  possible  and  turn  off  syntax  array  and
	  colouring. Tested by reading a 900MB file.	  

	* replace  case  control  has been added to the Edit menu
	  and to the Find and Replace dialog.

	* Fix  parsing  bug in extended regular expressions, ere,
	  functions.  Repeats are now allowed after char sets and
	  groups: "[a-b]*" and "(fred|joe)*" for example now work

	* diff now honors the backup-filename-format format.

[end readme.txt]
