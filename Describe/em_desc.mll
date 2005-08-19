[C!]
COMMAND NAME
    !

DESCRIPTION
    An  arithmetic  operator  function  that returns the complement of
    <expression>. This function is only usable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (! <expression>)

SEE ALSO
[C!=]
COMMAND NAME
    !=

DESCRIPTION
    An  arithmetic  operator function that returns 1 if <expression-1>
    is not equal to <expression-2>. This functions is only callable as
    an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (!= <expression-1> <expression-2>)

SEE ALSO
[C%]
COMMAND NAME
    %

DESCRIPTION

    An arithmetic  operator function that returns the remainder of the
    first  argument  by  the  rest of its arguments. This functions is
    only callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (% <expression-1> <expression-2>)
    (% 15 8) => 7
    (% 15 8 3) => 1

SEE ALSO
[C&]
COMMAND NAME
    &

DESCRIPTION
    An  arithmetic operator function that returns the result of anding
    all  its arguments together. This functions is only callable as an
    MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (& <expression-1> <expression-2>)
    (& 7 3) => 3
    (& 7 3 5) => 1

SEE ALSO
[C*]
COMMAND NAME
    *

DESCRIPTION
    An  arithmetic  operator  function  that  returns  the  result  of
    multiplying all  its  arguments  together.  this  function is only
    callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (* <expression-1> <expression-2>)
    (* 3 7 4) => 84

SEE ALSO
[C+]
COMMAND NAME
    +

DESCRIPTION
    An  arithmetic  operator  function that returns the sum of all its
    arguments.  This function is only callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (+ <expression-1> <expression-2>)
    (+ 1 2 7) => 10

SEE ALSO
[C-]
COMMAND NAME
    -

DESCRIPTION
    An  arithmetic  operator  function that returns the first argument
    minus  the sum of the rest of the arguments. This function is only
    callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (- <expression-1> <expression-2>)
    (- 10 3 1) => 6

SEE ALSO
[C/]
COMMAND NAME
    /

DESCRIPTION
    An  arithmetic  operator  function  that  returns  the  result  of
    dividing  its  first  argument  by  rest  of  the  arguments. This
    function is only callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (/ <expression-1> <expression-2>)
    (/ 42 2 7) => 3

SEE ALSO
[C<]
COMMAND NAME
    <

DESCRIPTION
    An  arithmetic  operator function that returns 1 <expression-1> is
    less  than  <expression-2>.  This  function is only callable as an
    MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (< <expression-1> <expression-2>)

SEE ALSO
[C<<]
COMMAND NAME
    <<

DESCRIPTION
    An  arithmetic  operator  function  that  returns  the  result  of
    shifting  left its first argument by the number of places which is
    the  sum  of  the  rest  of  the arguments. This functions is only
    callable from Mock Lisp.

DEFAULT BINDING
    None.

SYNOPSIS
    (<< <expression-1> <expression-2>)
    (<< 1 2 3) => 32

SEE ALSO
[C<=]
COMMAND NAME
    <=

DESCRIPTION
    An  arithmetic  operator function that returns 1 if <expression-1>
    is  less  than  or  equal to <expression-2>. This function is only
    callable from Mock Lisp.

DEFAULT BINDING
    None.

SYNOPSIS
    (<= <expression-1> <expression-2>)

SEE ALSO
[C=]
COMMAND NAME
    =

DESCRIPTION
    An  arithmetic  operator function that returns 1 if <expression-1>
    is the same as <expression-2>. This function is only callable from
    Mock Lisp.

DEFAULT BINDING
    None.

SYNOPSIS
    (= <expression-1> <expression-2>)

SEE ALSO
[C>]
COMMAND NAME
    >

DESCRIPTION
    An   arithmetic   operator   function   that   returns   true   if
    <expression-1>  is  greater  than <expression-2>. This function is
    only callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (> <expression-1> <expression-2>)

SEE ALSO
[C>=]
COMMAND NAME
    >=

DESCRIPTION
    An   arithmetic   operator   function   that   returns   true   if
    <expression-1>  is  greater  than or equal to <expression-2>. This
    function is only callable as an MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (>= <expression-1> <expression-2>)

SEE ALSO
[C>>]
COMMAND NAME
    >>

DESCRIPTION
    An  arithmetic  operator  function  that  returns  the  result  of
    shifting right its first argument by the number of places which is
    the  sum  of  the  rest  of  the  arguments. This function is only
    callable from mock lisp.

DEFAULT BINDING
    None.

SYNOPSIS
    (>> <expression-1> <expression-2>)
    (>> 128 2 3) => 4

SEE ALSO
[CESC-prefix]
COMMAND NAME
    ESC-prefix

DESCRIPTION
    ESC-prefix  is the ESC command prefix keymap. If you execute this
    command,  the  next character will act as if ESC was typed before
    it.

DEFAULT BINDING
    ESC

SYNOPSIS
    (ESC-prefix)

SEE ALSO
    define-keymap
    use-global-map
    use-local-map
[CMinibuf-local-NS-map]
COMMAND NAME
    Minibuf-local-NS-map

DESCRIPTION
    Minibuf-local-NS-map  is  the  keymap used to read commands in the
    mini-buffer  when spaces are not command terminators. For example:
    reading a string for get-tty-string.

    The following keys are bound in the keymap

	^G	- error-and-exit
	ESC	- expand-and-exit
	^C	- exit-emacs
	<CR>	- exit-emacs
	<LF>	- exit-emacs
	^\	- ESC-prefix
	<Space>	- expand-and-exit
	<Tab>	- expand-and-exit
	
DEFAULT BINDING
    None.

SYNOPSIS
    (Minibuf-local-NS-map)

SEE ALSO
    Minibuf-local-map
    error-and-exit
    expand-and-exit
    help-and-exit
[CMinibuf-local-map]
COMMAND NAME
    Minibuf-local-map

DESCRIPTION
    Minibuf-local-map  is  the  keymap  used  to  read commands in the
    mini-buffer  when  spaces  are  command  terminators. For example:
    reading a command name with expand-mlisp-word.

    The following keys are bound in the keymap

	^G	- error-and-exit
	ESC	- expand-and-exit
	?	- help-and-exit
	^C	- exit-emacs
	<CR>	- exit-emacs
	<LF>	- exit-emacs
	^\	- ESC-prefix

DEFAULT BINDING
    None.

SYNOPSIS
    None.

SEE ALSO
    Minibuf-local-NS-map
    exit-and-expand
    exit-and-help
[CUI-add-menu]
COMMAND NAME
    UI-add-menu (Windows)

DESCRIPTION

    UI-add-menu adds  a  single  menu item to the existing menus.
    The  <menu-id>  is  the two character string to bind the menu
    item  to.   When  selected  the  menu  with  send  the string
    "\(menu)<menu-id>".

    The values  of  <menu-ID>  are  restricted  to a set of known
    menu-IDs  and  a range of user values.  There can be 100 user
    defined  values.  Any ID that is not one of the built in ones
    can  be used.  To avoid picking a builting value use an upper
    case letter as the first character of the ID.  The known menu
    IDs  correspond  to  menu  actions  that require some special
    support  from Emacs.  The following table lists the known IDs

    Name	    ID	    Special Action
    ----	    --      --------------
    Edit Copy	    "ec"    Enabled only if mark is set
    Edit Cut	    "ex"    Enabled only if mark is set
    Edit Clear	    "er"    Enabled only if mark is set
    Edit Paste	    "ev"    Enabled when text in clipboard
    Search Find	    "sf"    Enabled if the find dialog can be
			    started
    Search Replace  "sr"    Enabled if the replace dialog can
			    be started
    File Open	    "fo"    none
    File Save	    "fs"    Enabled only if buffer is modified
    File Save As    "fa"    none

    There can  be  upto  9  pairs  of  the  <position> and <name>
    parameters.  Each pair describes one level of menu hierarchy.
    A  menu item named <name> is inserted at position <position>.
    The   first  menu  position  is  numbered  0.   Use  a  large
    <position>  value  to insert at the end of a menu when you do
    not know how many items are in the menu.

    You cannot insert a first level menu after the "&Help" menu.

    The <name>  parameter  has  one  special value, "-", which is
    will  cause a menu seperating line to be inserted.  Otherwise
    the  <name> parameter can be any string.  Place an "&" before
    the  character  in  name  string that you wish to be the menu
    accelerator character.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-menu <menu-ID> <position> <name> [<position> <name>] ...)

EXAMPLE

    This example  adds  the Project menu with two items, Make and
    Debug with a separator line between them.

    (UI-add-menu "Pm" 999 "&Project" 999 "&Make")
    (UI-add-menu "" 999 "&Project" 999 "-")
    (UI-add-menu "Pd" 999 "&Project" 999 "De&bug")

SEE ALSO
    UI-remove-menu
    UI-list-menus

[CUI-add-menu-keys]
COMMAND NAME
    UI-add-menu-keys (Motif)

DESCRIPTION

    Add a  new  item  into the Emacs menu bar which will send the
    <keys> to Emacs, when selected.

    UI-add-menu-keys can  create  a  multi-level  menu heirarchy.
    Each  level of menu is specified by a <position> <name> value
    pair.   The  <position>  is  an integer value that is used to
    sort  this new menu item into the desired position within the
    existing menu items.

    The first  <position>  <name> pair creates a menu item on the
    menu  bar.   The second <position> <name> creates a pull down
    menu  item  from  the  menu bar.  Further levels add cascaded
    menus.

    To create  a  menu separator specify the special <keys> value
    of "-".

    Optionally place  an  "&"  character  before the character in
    <name>  that  you  wish  to be the keyboard short cut for the
    menu    item.    Short   cuts   can   be   used   by   typing
    ALT-<short-cut-key>.   In the example below Alt-d, Alt-c will
    select the menu item Development, Compile.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-menu-keys <keys> <position> <name> [<position> <name>] ...)

EXAMPLE

    (UI-add-menu-keys "\^x\^e" 80 "&Development" 10 "&Compile")
    (UI-add-menu-keys "\^x\^n" 80 "&Development" 20 "Next &Error")
    (UI-add-menu-keys "-" 80 "&Development"" 30 "Sep1")
    (UI-add-menu-keys "\^xR" 80 "&Help" 40 "&Run")

SEE ALSO

[CUI-add-menu-procedure]
COMMAND NAME
    UI-add-menu-procedure (Motif)

DESCRIPTION
    Add a  new  item  into the Emacs menu bar which will send the
    <keys> to Emacs, when selected.

    UI-add-menu-procedure can  create  a  multi-level  menu heirarchy.
    Each  level of menu is specified by a <position> <name> value
    pair.   The  <position>  is  an integer value that is used to
    sort  this new menu item into the desired position within the
    existing menu items.

    The first  <position>  <name> pair creates a menu item on the
    menu  bar.   The second <position> <name> creates a pull down
    menu  item  from  the  menu bar.  Further levels add cascaded
    menus.

    To create  a  menu separator use UI-add-menu-keys.

    Optionally place  an  "&"  character  before the character in
    <name>  that  you  wish  to be the keyboard short cut for the
    menu    item.    Short   cuts   can   be   used   by   typing
    ALT-<short-cut-key>.   In the example below Alt-d, Alt-c will
    select the menu item Development, Compile.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-menu-procedure <procedure>
		<position> <name> [<position> <name>] ...)

EXAMPLE

    (UI-add-menu-procedure "compile-it"
			80 "&Development" 10 "&Compile")
    (UI-add-menu-procedure "next-error"
			80 "&Development" 20 "&Next Error")
    (UI-add-menu-procedure "previous-error"
			80 "&Development" 25 "&Previous Error")
    (UI-add-menu-keys "-" 80 "&Development"" 30 "Sep1")
    (UI-add-menu-procedure "run-it"
			80 "&Help" 40 "&Run")

SEE ALSO

[CUI-add-to-recent-file-list]
COMMAND NAME
    UI-add-to-recent-file-list (Windows)

DESCRIPTION

    Add the  <filename>  to  the  recent file list.  Items in the
    recent file list are accessed from the file menu.  The recent
    file list is saved and restored automatically by Emacs.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-to-recent-file-list <filename>)

EXAMPLE

SEE ALSO

[CUI-add-tool-bar-button-keys]
COMMAND NAME
    UI-add-tool-bar-button-keys (Motif)

DESCRIPTION

    Add a   new  button  named  <name>  to  the  tool  bar  named
    <tool-bar>.   If  <label>  is file name the file is used as a
    bitmap  to  label  the  button.  Otherwise the <label> is the
    text  string  to  use  as the buttons label.  Emacs considers
    <label>  to  be  a  filename  if there is a ":" or "/" in the
    text.

    When the  button  is  pressed  the keys in <keys> are sent to
    emacs as if you had typed them.

    At the moment the only toolbar available is named "Global".

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-tool-bar-button-keys
		<tool-bar> <name> <label> <keys>)

EXAMPLE
    (UI-add-tool-bar-button-keys
	"Global" "open_curly_brace"
	"emacs_library:open-curly-brace.bm"
	"\e(")
    (UI-add-tool-bar-button-keys
	"Global" "compile_it"
	"COmpile"
	"\^x\^e")

SEE ALSO
    UI-add-tool-bar-button-procedure
    UI-add-tool-bar-separator
[CUI-add-tool-bar-button-procedure]
COMMAND NAME
    UI-add-tool-bar-button-procedure (Motif)

DESCRIPTION
    Add a   new  button  named  <name>  to  the  tool  bar  named
    <tool-bar>.   If  <label>  is file name the file is used as a
    bitmap  to  label  the  button.  Otherwise the <label> is the
    text  string  to  use  as the buttons label.  Emacs considers
    <label>  to  be  a  filename  if there is a ":" or "/" in the
    text.

    When the  button  is pressed the MLisp procedure named <proc>
    is executed.

    At the moment the only toolbar available is named "Global".

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-tool-bar-button-procedure
		<tool-bar> <name> <label> <proc>)

EXAMPLE
    (UI-add-tool-bar-button-procedure
	"Global" "stop"
	"Stop!"	"interrupt-key")
    (UI-add-tool-bar-button-procedure
	"Global" "openfile"
	"emacs_library:openfile.bm"
	"motif-visit-file")

SEE ALSO
    UI-add-tool-bar-button-keys
    UI-add-tool-bar-separator
[CUI-add-tool-bar-separator]
COMMAND NAME
    UI-add-tool-bar-separator (Motif)

DESCRIPTION
    
    Add a new button separator named <name> to the tool bar named
    <tool-bar>.   The  optional  <width>  argument  allows you to
    increase the size of the separator.

    At the moment the only toolbar available is named "Global".

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-add-tool-bar-separator <tool-bar> <name> [<width>])

EXAMPLE
    (UI-add-tool-bar-separator "Global" "sep1")

SEE ALSO
    UI-add-tool-bar-button-keys
    UI-add-tool-bar-button-procedure

[CUI-edit-copy]
COMMAND NAME
    UI-edit-copy (Windows) (Motif)

DESCRIPTION
    Copy the current buffers region to the clipboard.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-edit-copy)

SEE ALSO
    UI-edit-paste

[CUI-edit-paste]
COMMAND NAME
    UI-edit-paste (Windows) (Motif)

DESCRIPTION
    Insert  the contents of the clipboard into the current buffer
    at  dot.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-edit-paste)

SEE ALSO
    UI-edit-copy

[CUI-file-open]
COMMAND NAME
    UI-file-open (Windows) (Motif)

DESCRIPTION

    Pop up  the standard file open dialog and allows the users to
    select  a  file.  The UI-filter-file-list variable is read to
    setup  the  file  filters.  The UI-open-file-name variable is
    used to initialise the file name and path parts of the dialog
    box.

    On completion UI-file-open returns 1 if the user clicked "OK"
    and 0 if the user clicked "Cancel".  The file selected by the
    user  is  returned  in  the  UI-open-file-name variable.  The
    UI-open-file-readonly  variable is true if the readonly check
    box was set on.

DEFAULT BINDING
    none

SYNOPSIS
    (setq result (UI-file-open))

EXAMPLE

    (setq UI-open-file-name "d:\fred\joe.txt")
    (if (UI-file-open)
	(progn
	    (visit-file UI-open-file-name)
	    (setq read-only-buffer UI-open-file-readonly)
	)
    )

SEE ALSO
    UI-file-save-as
    UI-filter-file-list variable
    UI-open-file-name variable
    UI-open-file-readonly variable

[CUI-file-save-as]
COMMAND NAME
    UI-file-save-as (Windows)

DESCRIPTION

    Pop up  the standard file save as dialog and allows the users
    to  select  where  to save the file.  The UI-filter-file-list
    variable   is   read   to   setup   the  file  filters.   The
    UI-save-as-file-name  variable is used to initialise the file
    name and path parts of the dialog box.

    On completion UI-file-open returns 1 if the user clicked "OK"
    and 0 if the user clicked "Cancel".  The file selected by the
    user is returned in the UI-save-as-file-name variable.


DEFAULT BINDING
    none

SYNOPSIS
    (UI-file-save-as)

EXAMPLE

    (setq UI-save-as-file-name current-buffer-file-name)
    (if (UI-file-save-as)
	(write-named-file UI-save-as-file-name)
    )

SEE ALSO
    UI-open-file
    UI-save-as-file-name variable
    UI-filter-file-list variable

[CUI-find]
COMMAND NAME
    UI-find (Windows)

DESCRIPTION

    Pop up   the   find   string   modeless   dialog   box.   The
    UI-search-string  variable  is  used  to initialise the "Find
    what" field.

DEFAULT BINDING
    none

SYNOPSIS
    (setq UI-search-string "find me")
    (UI-find)

SEE ALSO
    UI-find-and-replace
    UI-search-string variable

[CUI-find-and-replace]
COMMAND NAME
    UI-find-and-replace (Windows)

DESCRIPTION

    Pop up   the   find and replace   modeless   dialog   box.   The
    UI-search-string  variable  is  used  to initialise the "Find
    what"  field.   And the UI-replace-string variable is used to
    initialise the "Replace with" field.

DEFAULT BINDING
    none

SYNOPSIS
    (setq UI-search-string "find me")
    (setq UI-replace-string "new string")
    (UI-find-and-replace)

SEE ALSO

[CUI-frame-maximize]
COMMAND NAME
    UI-frame-maximize (Windows)

DESCRIPTION

    Maximize the frame window.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-frame-maximize)

SEE ALSO
    UI-frame-minimize
    UI-frame-restore

[CUI-frame-minimize]
COMMAND NAME
    UI-frame-minimize (Windows) (Motif)

DESCRIPTION
   
    Minimize the frame window.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-frame-minimize)

SEE ALSO
    UI-frame-maximize
    UI-frame-restore

[CUI-frame-raise]
COMMAND NAME
    UI-frame-raise (Motif)

DESCRIPTION

    Place the Emacs frame window on the top of all other windows.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-frame-raise)

EXAMPLE

SEE ALSO

[CUI-frame-restore]
COMMAND NAME
    UI-frame-restore (Windows) (Motif)

DESCRIPTION
    Restore  the  frame  window  to  the size and position it had
    before being maximised or minimised (iconised).

DEFAULT BINDING

SYNOPSIS
    (UI-frame-restore)

SEE ALSO
    UI-frame-maximize
    UI-frame-minimize

[CUI-list-menus]
COMMAND NAME
    UI-list-menus (Windows) (Motif)

DESCRIPTION

    List the menu structure that is currently in effect.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-list-menus)

SEE ALSO
    UI-add-menu (Windows)
    UI-add-menu-keys (Motif)
    UI-add-menu-procedure (Motif)
    UI-remove-menu (Windows) (Motif)

[CUI-list-tool-bars]
COMMAND NAME
    UI-list-tool-bars (Motif)

DESCRIPTION

    List the Tool bar structure that is currently in effect.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-list-tool-bars)

EXAMPLE

SEE ALSO

[CUI-remove-menu]
COMMAND NAME
    UI-remove-menu (Windows)

DESCRIPTION

    UI-remove-menu removes  a  single menu item from the existing
    menus.

    There can  be upto 9 <name> parameters, one for each level of
    menu hierarchy.  A menu item named <name> will be removed.

    The <name>  parameter can be any string.  Place an "&" before
    the  character  in the name string that is a menu accelerator
    character.

DEFAULT BINDING

SYNOPSIS
    (UI-remove-menu)

SEE ALSO
    UI-add-menu
    UI-list-menus

[CUI-remove-tool-bar-button]
COMMAND NAME
    UI-remove-tool-bar-button (Motif)

DESCRIPTION

    Remove  the  button  named  <name>  from  the  tool bar named
    <tool-bar>.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-remove-tool-bar-button <tool-bar> <name>)

EXAMPLE

SEE ALSO
    UI-add-tool-bar-button-procedure
    UI-add-tool-bar-button-keys
    UI-list-tool-bars
[CUI-select-buffer]
COMMAND NAME
    UI-select-buffer (Windows)

DESCRIPTION

    Put up  a  dialog box that allows the user to select to a new
    buffer or an existing buffer.

    The dialog  box  using  the  the <title> string as its title.
    The  text  field of the combo box is set to the <buffer-name>
    string  parameter.   <buffer-name>  defaults  to  the current
    buffer  name.   The  list  part  of  the  combo box lists all
    buffers  by  default.   Supply the <buffer-types> string as a
    list  of  the types of buffers that you wish to list.  "file"
    for  file  buffers, "scratch" for scratch buffers and "macro"
    for macro buffer.  Seperate multiple types with a comma.  For


DEFAULT BINDING

SYNOPSIS
    (UI-select-buffer <title> [<buffer-name>] [<buffer-types>])

EXAMPLE
    Prompt for a scratch buffer:

    (UI-select-buffer "Scratch buffer" "output" "scratch")

    Prompt for a file or scratch buffer:

    (UI-select-buffer
	"Change to buffer"
	(current-buffer-name)
	"file,scratch")

SEE ALSO
    UI-switch-buffer
    UI-pop-to-buffer

[CUI-switch-buffer]
COMMAND NAME
    UI-switch-buffer (Windows)

DESCRIPTION

    Put up  a  dialog box that allows the user to switch to a new
    buffer or change to an existing buffer.

DEFAULT BINDING

SYNOPSIS
    (UI-switch-buffer)

SEE ALSO
    UI-select-buffer
    UI-pop-to-buffer
[CUI-pop-to-buffer]
COMMAND NAME
    UI-pop-to-buffer (Windows)

DESCRIPTION

    Put up  a  dialog box that allows the user to pop to a new
    buffer or an existing buffer.

DEFAULT BINDING

SYNOPSIS
    (UI-pop-to-buffer)

SEE ALSO
    UI-select-buffer
    UI-switch-buffer


[CUI-view-status-bar]
COMMAND NAME
    UI-view-status-bar (Windows) (Motif)

DESCRIPTION

    Call this command to toggle the status bar on and off.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-view-status-bar)

SEE ALSO
    UI-view-tool-bar
    UI-view-scroll-bars
[CUI-view-scroll-bars]
COMMAND NAME
    UI-view-scroll-bars (Windows) (Motif)

DESCRIPTION

    Call this command to toggle the scroll bars on and off.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-view-scroll-bars)

SEE ALSO
    UI-view-tool-bar
    UI-view-status-bar
[CUI-view-tool-bar]
COMMAND NAME
    UI-view-tool-bar (Windows) (Motif)

DESCRIPTION

    Call this command to toggle the tool bar on and off.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-view-tool-bar)

SEE ALSO
    UI-view-status-bar
    UI-view-scroll-bars
[CUI-window-cascade]
COMMAND NAME
    UI-window-cascade (Windows)

DESCRIPTION

    Cascade the document windows.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-cascade)

SEE ALSO
    UI-window-tile-horizontal
    UI-window-tile-vertical
    UI-window-minimize
    UI-window-maximize
    UI-window-restore

[CUI-window-maximize]
COMMAND NAME
    UI-window-maximize (Windows)

DESCRIPTION

    Maximize the document window.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-maximize)

SEE ALSO
    UI-window-cascade
    UI-window-tile-horizontal
    UI-window-tile-vertical
    UI-window-minimize
    UI-window-restore

[CUI-window-minimize]
COMMAND NAME
    UI-window-minimize (Windows)

DESCRIPTION

    Minimize the document window.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-minimize)

SEE ALSO
    UI-window-cascade
    UI-window-tile-horizontal
    UI-window-tile-vertical
    UI-window-maximize
    UI-window-restore

[CUI-window-new]
COMMAND NAME
    UI-window-new (Windows)

DESCRIPTION

    This command  creates  a new Viewing Window.  The new Viewing
    Window  will  contain  a  copy  the  Emacs  windows  from the
    currently active Viwing Window.

DEFAULT BINDING
    none.

SYNOPSIS
    (UI-window-new)

SEE ALSO
    UI-window-next

[CUI-window-next]
COMMAND NAME
    UI-window-next (Windows)

DESCRIPTION

    Cycle to the next Viewing Window.

DEFAULT BINDING
    Ctrl-F6

SYNOPSIS
    (UI-window-next)

SEE ALSO
    UI-window-new
[CUI-window-restore]
COMMAND NAME
    UI-window-restore (Windows)

DESCRIPTION

    Restore the document window.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-restore)

SEE ALSO
    UI-window-cascade
    UI-window-tile-horizontal
    UI-window-tile-vertical
    UI-window-minimize
    UI-window-maximize

[CUI-window-tile-horizontal]
COMMAND NAME
    UI-window-tile-horizontal (Windows)

DESCRIPTION

    Horizontally tile the document windows.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-tile-horizontal)

SEE ALSO
    UI-window-cascade
    UI-window-tile-vertical
    UI-window-minimize
    UI-window-maximize
    UI-window-restore

[CUI-window-tile-vertical]
COMMAND NAME
    UI-window-tile-vertical (Windows)

DESCRIPTION

    Vertically tile the document windows.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-window-tile-vertical)

SEE ALSO
    UI-window-cascade
    UI-window-tile-horizontal
    UI-window-minimize
    UI-window-maximize
    UI-window-restore

[CUI-windows-execute-program]
COMMAND NAME
    UI-windows-execute-program (Windows)

DESCRIPTION

    This command  will start a windows program executing named in
    <command>.   The  program  will  be  stated in the show state
    named   in   <show>.   <show>  can  be  one  of  the  strings
    "minimized", "maximized" or "normal".


DEFAULT BINDING
    none

SYNOPSIS
    (UI-windows-execute-program <command> <show>)

EXAMPLE

    Start the windows file manager in a maximized window.

    (UI-windows-execute-program "winfile" "maximized")

SEE ALSO
    UI-windows-help

[CUI-windows-help]
COMMAND NAME
    UI-windows-help (Windows)

DESCRIPTION

    The UI-windows-help  command  is  an interface to the Windows
    Help  system.   The <help-file> parameter is the file name of
    the  help file that will be used.  What is done with the help
    file  is  determined by the <cmd> and <data> parameters.  THe
    following  table  details  the  <cmd> values and any required
    <data>.

    <cmd> Value	    Description
    "context"	    Display help for context number <data>.
    "contents"	    Display the help file contents.
    "setcontents"   Select contents number <data>.
    "contextpopup"  Display in a pop-up window help for context
		    number <data>.
    "key"	    Display help for topic string <data>.
    "partialkey"    Display help for topic string <data>.
		    If <data> is the string "" then display the
		    search dialog.
    "command"	    Execute the help macro in string <data>.
    "forcefile"	    Force Windows Help to display the correct help
		    file.
    "helponhelp"    Open Window Help Help file
    "quit"	    Quit Windows Help

    For more  information about these <cmd> and <data> values see
    the  documentation  for  Windows  function  WinHelp  and  the
    Windows Help compiler.

DEFAULT BINDING
    none

SYNOPSIS
    (UI-windows-help <help-file> <cmd> <data>)

EXAMPLE

    Display the  help topic for WinHelp function from the Windows
    3.1 programs reference help file.

    (UI-windows-help "win31.hlp" "key" "WinHelp")

SEE ALSO
    UI-windows-execute-program
[CUI-windows-shell-execute]
COMMAND NAME
    UI-windows-shell-execute (Windows)

DESCRIPTION

    This command  will  start  the  windows application executing
    that  is  associated  with  the  <file>.

    The application  will  be asked to perform the <operation> on
    the <file>. Operations are the strings "open" and "print".

    Some applications  require additional parameters pass them in
    the <parameters> argument.

    The application  will  be  started  with  a current directory
    named in <directory>.

    The program  will  be  started  in  the  show  state named in
    <show>.   <show>  can  be  one  of  the  strings "minimized",
    "maximized" or "normal".


DEFAULT BINDING
    none

SYNOPSIS
    (UI-windows-shell-execute 
	<operation> <file> <parameters> <directory> <show>)

EXAMPLE

    Start the default Web browser to view intro.html. 

    (UI-windows-shell-execute "open" "c:\\docs\\intro.html"
		 "" (current-directory) "normal")

SEE ALSO
    UI-windows-help
    UI-windows-execute-program

[CProfile-emacs]
This function is for EMACS development work - do not use.
[CProfile-emacs-histogram]
This function is for EMACS development work - do not use.
[CVMS-load-averages]
COMMAND NAME
    VMS-load-averages (VMS)

DESCRIPTION
    An  MLisp  function  that returns some process statistics for the
    process  running  EMACS.  The  string  returned has the following
    format:-

     ELAPSED = 14:08:08.43  CPU = 0:08:04.60  BUFIO = 19767 DIRIO = 2426
     PAGEFAULTS = 56661

    without any newlines.

DEFAULT BINDING
    None.

SYNOPSIS
    (VMS-load-averages)

SEE ALSO
[CXmCheckBox]
COMMAND NAME
    XmCheckBox (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmCheckBox x y border-width <contents>)

EXAMPLE

SEE ALSO

[CXmDestroyDialog]
COMMAND NAME
    XmDestroyDialog (Motif)

DESCRIPTION
    This command destroyes the name motif dialog.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmDestroyDialog <name>)

EXAMPLE

SEE ALSO

[CXmDumpDialogs]
COMMAND NAME
    XmDumpDialogs (Motif)

DESCRIPTION
    Lists all the defined dialogs in the buffer "Dialog table".

DEFAULT BINDING
    none.

SYNOPSIS
    (XmDumpDialogs)

EXAMPLE

SEE ALSO

[CXmFileSelectionDialog]
COMMAND NAME
    XmFileSelectionDialog (Motif)

DESCRIPTION



DEFAULT BINDING
    none.

SYNOPSIS
    (XmFileSelectionDialog name apply-keys pattern-variable result-variable)

EXAMPLE

SEE ALSO

[CXmFormDialog]
COMMAND NAME
    XmFormDialog (Motif)

DESCRIPTION
    ~Todo~

DEFAULT BINDING
    none.

SYNOPSIS
    (XmFormDialog name apply-keys width height <contents>*)

EXAMPLE

SEE ALSO

[CXmFrame]
COMMAND NAME
    XmFrame (Motif)

DESCRIPTION

    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmFrame x y shadow-type <contents>)

EXAMPLE

SEE ALSO

[CXmIsDialog]
COMMAND NAME
    XmIsDialog (Motif)

DESCRIPTION
    Return  1 if there is a dialog named <name>, otherwise return
    0.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmIsDialog <name>)

EXAMPLE

SEE ALSO

[CXmLabel]
COMMAND NAME
    XmLabel (Motif)

DESCRIPTION

    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmLabel x y label-text)

EXAMPLE

SEE ALSO

[CXmMessageDialog]
COMMAND NAME
    XmMessageDialog (Motif)

DESCRIPTION

    ~Todo~

DEFAULT BINDING
    none.

SYNOPSIS
    (XmMessageDialog name dialog-type message-variable
	[ok-text] [cancel-text] )

EXAMPLE

SEE ALSO

[CXmOperateModalDialog]
COMMAND NAME
    XmOperateModalDialog (Motif)

DESCRIPTION
    Operate the modal dialog named <name>.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmOperateModalDialog <name>)

EXAMPLE

SEE ALSO

[CXmOperateModelessDialog]
COMMAND NAME
    XmOperateModelessDialog (Motif)

DESCRIPTION
    Start operating the modeless dialog <name>.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmOperateModelessDialog <name>)

EXAMPLE

SEE ALSO

[CXmPushButtonApply]
COMMAND NAME
    XmPushButtonApply (Motif)

DESCRIPTION

    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmPushButtonApply <label> [<x>] [<y>] [<width>] [<height>])

EXAMPLE

SEE ALSO

[CXmPushButtonCancel]
COMMAND NAME
    XmPushButtonCancel (Motif)

DESCRIPTION

    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmPushButtonCancel <label> [<x>] [<y>] [<width>] [<height>])

EXAMPLE

SEE ALSO

[CXmPushButtonOK]
COMMAND NAME
    XmPushButtonOK (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmPushButtonOK <label> [<x>] [<y>] [<width>] [<height>])

EXAMPLE

SEE ALSO

[CXmPushButtonUser]
COMMAND NAME
    XmPushButtonUser (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmPushButtonUser <label> [<x>] [<y>] [<width>] [<height>])

EXAMPLE

SEE ALSO

[CXmRadioBox]
COMMAND NAME
    XmRadioBox (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmRadioBox x y border-width <contents>)

EXAMPLE

SEE ALSO

[CXmRowColumn]
COMMAND NAME
    XmRowColumn (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmRowColumn x y width height border-width columns orientation pack)

EXAMPLE

SEE ALSO

[CXmScale]
COMMAND NAME
    XmScale (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmScale x y title width height min max is_horz show variable)

EXAMPLE

SEE ALSO

[CXmSeparator]
COMMAND NAME
    XmSeparator (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmSeparator x y length is_horz separator-type)

EXAMPLE

SEE ALSO

[CXmTextField]
COMMAND NAME
    XmTextField (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmTextField x y columns resize variable)

EXAMPLE

SEE ALSO

[CXmToggleButton]
COMMAND NAME
    XmToggleButton (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmToggleButton x y label variable)

EXAMPLE

SEE ALSO

[CXmUpdatedLabel]
COMMAND NAME
    XmUpdatedLabel (Motif)

DESCRIPTION
    This  command  builds  the named motif object.  See the Motif
    programmers   information   for   details   of  this  objects
    parameters and behaviour.

DEFAULT BINDING
    none.

SYNOPSIS
    (XmUpdatedLabel x y variable)

EXAMPLE

SEE ALSO

[C^]
COMMAND NAME
    ^

DESCRIPTION
    An  Arithmetic  operator  function  that returns all its arguments
    exclusive  ored  together.  This  functions is only callable as an
    MLisp expression.

DEFAULT BINDING
    None.

SYNOPSIS
    (^ <expression-1> <expression-2>)

SEE ALSO
[C^X-prefix]
COMMAND NAME
    ^X-prefix

DESCRIPTION

    ^X-prefix  is  the  ^X command prefix keymap. If you execute this
    command,  the  next  character will act as if ^X was typed before
    it.

DEFAULT BINDING
    ^X

SYNOPSIS
    (^X-prefix)

SEE ALSO
[Cappend-region-to-buffer]
COMMAND NAME
    append-region-to-buffer

DESCRIPTION
    Appends   the   region   between   dot  and  mark  to  the  buffer
    <buffer-name>. Neither the original text in the destination buffer
    nor the text in the region between dot and mark will be disturbed.

DEFAULT BINDING
    None.

SYNOPSIS
    (append-region-to-buffer <buffer-name>)

SEE ALSO
    prepend-region-to-buffer
[Cappend-to-file]
COMMAND NAME
    append-to-file

DESCRIPTION
    Takes  the  contents  of  the current buffer and appends it to the
    file named  <file-name>.  If  the files does not exist, it will be
    created.

DEFAULT BINDING
    None.

SYNOPSIS
    (append-to-file <file-name>)

SEE ALSO
[Capply-auto-execute]
COMMAND NAME
    apply-auto-execute

DESCRIPTION
    Scans  the  list  of file name patterns set up by auto-execute and
    executes  an  associated  command if the supplied filename matches
    one of the patterns.

DEFAULT BINDING
    None.

SYNOPSIS
    (apply-auto-execute "<filename>")

SEE ALSO
    auto-execute
[Capply-colour-to-region]
COMMAND NAME
    apply-colour-to-region

DESCRIPTION

    Use the  command  apply-colour-to-region  to  tell  Emacs the
    region in a buffer to add colour to.

    Where the  region  covers  the characters between "start" and
    "end"  and will take on the colour "colour".  The valid range
    for colour is 1 to 8, which selects colours User 1 to User 8.
    To clear the colouring from a range use colour 0.

DEFAULT BINDING
    none.

SYNOPSIS
    (apply-colour-to-region <start> <end> <colour>)

EXAMPLE

(defun
    (test-0
	(switch-to-buffer "Test rendition regions")
	(apply-colour-to-region 0 999 0)
    )
)
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

SEE ALSO

[Capropos]
COMMAND NAME
    apropos

DESCRIPTION
    Prints  a  list  of  all  the commands whose extended command name
    contains  the  word  <keyword>.  For  example, if you forget which
    commands deal with windows, just type `ESC-? window ESC'.

DEFAULT BINDING
    ESC-?

SYNOPSIS
    (apropos "<keyword>")

SEE ALSO
   apropos-variable
[Capropos-variable]
COMMAND NAME
    apropos-variable

DESCRIPTION
    Prints  a  list  of  all  the variables whose extended command name
    contains  the  word  <keyword>.  For  example, if you forget which
    variables deal with windows, just type `^X-? window <CR>'.

DEFAULT BINDING
    ^X-?

SYNOPSIS
    (apropos-variable "<keyword>")

SEE ALSO
    apropos
[Carg]
COMMAND NAME
    arg

DESCRIPTION
    Evaluates  the <expression-1>'th argument of the invoking function
    or  prompts  for  it  if called interactively. Note: the prompt is
    optional,  if  it  is  omitted,  the  function  cannot  be  called
    interactively.

    Some examples,

              (arg 1 "Enter a number: ")

    evaluates  to  the  value  of  the  first  argument of the current
    function, if the current function was called from MLisp. If it was
    called interactively then the argument is prompted for.

    Given:

              (defun (foo (+ (arg 1 "Number to increment? ") 1)))

    then  (foo  10) returns 11, but typing "ESC-X foo" causes EMACS to
    ask  "Number  to  increment?  ".

    Language  purists  will  no  doubt cringe at this rather primitive
    parameter mechanism, but what-the-hell... it's amazingly powerful.

DEFAULT BINDING
    None.

SYNOPSIS
    (arg <expression-1> "<prompt-string>")

SEE ALSO
[Cargc]
COMMAND NAME
    argc

DESCRIPTION
    A  function  that returns the number of arguments that were passed
    to  EMACS  when  it  was invoked from DCL. If argc is called early
    enough  then EMACS's startup action of visiting the files named on
    the command line is suppressed.

DEFAULT BINDING
    None.

SYNOPSIS
    (argc)

SEE ALSO
    argv
[Cargument-prefix]
COMMAND NAME
    argument-prefix

DESCRIPTION
    When followed by a string of digits cause that string of digits to
    be  interpreted  as a numeric prefix argument which is generally a
    repetition count for the following command.

    For example, ^U10^N moves down 10 lines (the 10'th next).

    For each ^U typed before a command, the current prefix argument is
    multiplied by 4. So ^U is 4, ^U-^U is 16 and ^U-^U-^U is 64.

    CAVEAT:  Argument-prefix  should  never  be  called  from an MLisp
    function.

DEFAULT BINDING
    ^U

SYNOPSIS
    None.

SEE ALSO
[Cargv]
COMMAND NAME
    argv

DESCRIPTION
    A  function  that  returns the <expression-1>'th argument that was
    passed to EMACS when it was invoked from DCL. If EMACS was invoked
    as  "EMACS blatto" then (argv 1) would return the string "blatto".
    If  argv  is  called  early  enough then EMACS's startup action of
    visiting the files named on the command line is suppressed.

DEFAULT BINDING
    None.

SYNOPSIS
    (argv <expression-1>)

SEE ALSO
    argc
[Carray]
COMMAND NAME
    array

DESCRIPTION
    This command creates an array of up to 10 dimensions. The argument
    to the command are pairs of bounds, first the lower bound then the
    upper  bound.  The  array is initialised to integer value 0 in all
    elements of the array.

EXAMPLES
    Create a two dimension array.

    (setq 2d-array (array -5 5 3 7))

    This  creates  an array where the first index ranges from -5 to +5
    and  the  second index ranges from +3 to +7. The array has a total
    of 55 elements in it.

DEFAULT BINDING
    none.

SYNOPSIS
    (array <low-bound> <high-bound> ...)

SEE ALSO
    setq-array
    fetch-array
    type-of-expression
    bounds-of-array
[Cauto-execute]
COMMAND NAME
    auto-execute

DESCRIPTION
    Sets  up a filename to command association. When a file is read in
    via  visit-file or read-file whose name matches the given pattern,
    the  given  command will be executed. The command is generally one
    which sets the mode for the buffer.

    Filename  patterns  must  be  of  the form "*string" or "string*":
    "*string" matches any filename whose suffix is "string"; "string*"
    matches any filename prefixed by "string".

    For  example:

	(auto-execute  "c-mode" "*.c")

    will  put EMACS into C mode for all files with the extension ".c".

DEFAULT BINDING
    None.

SYNOPSIS
    (auto-execute "<command-name>" "<file-pattern>")

SEE ALSO
    visit-file
    read-file
    apply-auto-execute
[Cautoload]
COMMAND NAME
    autoload

DESCRIPTION
    Defines  the  specified  command  to  be autoload'ed from the named
    file.  When  an attempt to execute the command is encountered, the
    file  is  loaded  and  then  the execution is attempted again. The
    loading of the file must have redefined the command.

    Autoloading  is useful when you have some command written in MLisp
    but  you  do not want  to  have  the  code  loaded in unless it is
    actually needed.

    For  example,  if you have a function named box-it in a file named
    boxit.ml, then the command

	(autoload  "box-it" "boxit.ml")

    will define the box-it command, but won't load its definition from
    boxit.ml.  The  loading  will  happen  when you try to execute the
    box-it command.

DEFAULT BINDING
    None.

SYNOPSIS
    (autoload "<command-string>" "<file-name>")

SEE ALSO
[Cbackward-balanced-paren-line]
COMMAND NAME
    backward-balanced-paren-line

DESCRIPTION
    Moves  dot  backwards  until either the beginning of the buffer is
    reached,  or  an unmatched open parenthesis is encountered, or the
    beginning  of  a  line is encountered at "parenthesis level zero".
    That  is,  without an unmatched ')' existing between there and the
    starting position of dot.

    The  definitions  of parenthesis is used from the syntax table for
    the current buffer.

    <expression-1>  is  the  number  of  parentheses  to  skip  before
    applying the match.

DEFAULT BINDING
    None.

SYNOPSIS
    (backward-balanced-paren-line <expression-1>)

SEE ALSO
    backward-paren
    forward-balanced-paren-line
    forward-paren
    dump-syntax-table
    modify-syntax-entry
    use-syntax-table
[Cbackward-character]
COMMAND NAME
    backward-character

DESCRIPTION
    Moves dot backwards <prefix-argument> characters. End-of-lines and
    tabs  each  count  as one character. You can't move back to before
    the beginning of the buffer.

DEFAULT BINDING
    ^B

SYNOPSIS
    (backward-character)

SEE-ALSO
    forward-character to see how to reverse this commands action.
[Cbackward-paragraph]
COMMAND NAME
    backward-paragraph (package)

DESCRIPTION
    Moves to the beginning of the current or previous paragraph. Blank
    lines,  Scribe  and  DSR/RNO command lines separate paragraphs and
    are not parts of paragraphs.

DEFAULT BINDING
    ESC-)

SYNOPSIS
    (backward-paragraph)

SEE ALSO
    forward-paragraph
    Variable paragraph-delimiters
[Cbackward-paren]
COMMAND NAME
    backward-paren

DESCRIPTION
    Moves  dot  backward  until  an  unmatched open parenthesis or the
    beginning  of  the  buffer  is  found.  This can be used to aid in
    skipping over Lisp S-expressions.

    The  definitions  of parenthesis is used from the syntax table for
    the current buffer.

    <expression-1>  is  the  number  of  parentheses  to  skip  before
    applying the match.

DEFAULT BINDING
    None.

SYNOPSIS
    (backward-paren <expression-1>)

SEE ALSO
    forward-paren
    dump-syntax-table
    modify-syntax-entry
    use-syntax-table
[Cbackward-sentence]
COMMAND NAME
    backward-sentence (package)

DESCRIPTION
    Moves  dot  to the beginning of the current sentence, or if dot is
    not  in  a  sentence,  moves  dot to the beginning of the previous
    sentence.

    A  sentence  is  defined  by  the regular expression search string
    contained in the variable sentence-delimiters.

DEFAULT BINDING
    ESC-a

SYNOPSIS
    (backward-sentence)

SEE ALSO
    forward-sentence
    variable sentence-delimiters
[Cbackward-word]
COMMAND NAME
    backward-word

DESCRIPTION
    Moves dot backward over <prefix-argument> words.

    A word is defined by the syntax table currently in use.

DEFAULT BINDING
    ESC-B

SYNOPSIS
    (backward-word)

SEE ALSO
    forward-word
    dump-syntax-table
    modify-syntax-entry
    use-syntax-table
[Cbaud-rate]
COMMAND NAME
   baud-rate

DESCRIPTION
    A  function that returns what EMACS thinks is the baud rate of the
    communication  line to the terminal. The baud rate is (usually) 10
    times the number of characters transmitted per second.

    (Baud-rate)  can  be used for such things as conditionally setting
    the  display-file-percentage variable in your EMACS profile:

    (setq display-file-percentage (> (baud-rate) 600))

    baud-rate does   not   return   anything   reasonable   under  GUI
    interfaces.

DEFAULT BINDING
    None.

SYNOPSIS
    (baud-rate)

SEE ALSO
[Cbeginning-of-file]
COMMAND NAME
    beginning-of-file

DESCRIPTION
    Moves  dot  to  just  before  the  first  character of the current
    buffer.

DEFAULT BINDING
    ESC-<

SYNOPSIS
    (beginning-of-file)

SEE ALSO
    end-of-file
[Cbeginning-of-line]
COMMAND NAME
    beginning-of-line

DESCRIPTION
    Moves  dot to the beginning of the line in the current buffer that
    contains dot; that is, to just after the preceding  end-of-line or
    the beginning of the buffer.

DEFAULT BINDING
    ^A

SYNOPSIS
    (beginning-of-line)

SEE ALSO
    end-of-line
[Cbeginning-of-window]
COMMAND NAME
    beginning-of-window

DESCRIPTION
    Moves  dot  to  just  in front of the first character of the first
    line displayed in the current window.

DEFAULT BINDING
    ESC-,

SYNOPSIS
    (beginning-of-window)

SEE ALSO
    end-of-window
[Cbind-to-key]
COMMAND NAME
    bind-to-key

DESCRIPTION
    Bind  the  extended command <command-name> to a given key sequence
    <keys>.  All  future  hits  on  the  key  sequence  will cause the
    extended  command  to be called. For example, if you want ESC-= to
    behave  the  way  ESC-X  print does, then typing ESC-X bind-to-key
    print  ESC-=  will  do  it.  The  binding will be in effect in all
    buffer  except  those  with  an  identical  local binding if a key
    sequence  specifies  a keymap that does not exist, then the keymap
    is automatically generated.

    The <keys>  parameter  can  use  the symbolic key names that Emacs
    supports. The names are

    Editing keypad key names:

	  +---------------+----------------+----------------+
	  |    \(find)    |   \(insert)    |   \(remove)    |
	  |               | \(insert here) |                |
	  +---------------+----------------+----------------+
	  |   \(select)   |    \(prev)     |    \(next)     |
	  |	          | \(prev screen) | \(next screen) |
	  +---------------+----------------+----------------+
			  |     \(up)      |
			  |		   |
	  +---------------+----------------+----------------+
	  |    \(left)    |    \(down)     |    \(right)    |
	  |		  |		   |		    |
	  +---------------+----------------+----------------+

    Numeric keypad key names:

		+---------+--------+--------+----------+
		| \(pf1)  | \(pf2) | \(pf3) |  \(pf4)  |
		| \(gold) |        |        |          |
		+---------+--------+--------+----------+
		| \(kp7)  | \(kp8) | \(kp9) | \(minus) |
		|         |        |        |          |
		+---------+--------+--------+----------+
		| \(kp4)  | \(kp5) | \(kp6) | \(comma) |
		|         |        |        |          |
		+---------+--------+--------+----------+
		| \(kp1)  | \(kp2) | \(kp3) |          |
		|         |        |        |          |
		+---------+--------+--------+          |
		|      \(kp0)      | \(dot) | \(enter) |
		|                  |        |          |
		+------------------+--------+----------+

    LK201 function keys:

	+-------+-------+-------+-------+--------+
	| \(f6) | \(f7) | \(f8) | \(f9) | \(f10) |
	+-------+-------+-------+-------+--------+

	+--------+--------+--------+--------+   +---------+-----------+
	| \(f11) | \(f12) | \(f13) | \(f14) |   | \(help) |   \(do)   |
	+--------+--------+--------+--------+   +---------+-----------+

	+--------+--------+--------+--------+
	| \(f17) | \(f18) | \(f19) | \(f20) |
	+--------+--------+--------+--------+

    Mouse event key names:

	\(mouse-1-down)	\(mouse-2-down)	\(mouse-3-down)	\(mouse-4-down)
	\(mouse-1-up)	\(mouse-2-up)	\(mouse-3-up)	\(mouse-4-up)

   Miscellaneous key names:

	\(csi)	\(ss3)	\(mouse)


DEFAULT BINDING
    None.

SYNOPSIS
    (bind-to-key "<command-name>" "<keys>")

SEE ALSO
    local-bind-to-key
    define-keymap
    use-global-map
    use-local-map
[Cbobp]
COMMAND NAME
    bobp

DESCRIPTION
    An  MLisp  function  which returns 1 if dot is at the beginning of
    the buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (bobp)

SEE ALSO
    eobp
    bolp
    eolp
[Cbolp]
COMMAND NAME
    bolp

DESCRIPTION
    An MLisp function which is 1 if dot is at the beginning of a line.

DEFAULT BINDING
    None.

SYNOPSIS
    (bolp)

SEE ALSO
    eobp
    bobp
    eolp
[Cbounds-of-array]
COMMAND NAME
    bounds-of-array

DESCRIPTION

    This  command  return an output-array describing the bounds of the
    input-array  that  is  its  argument.  The  output-array  is a two
    dimensional  array.  Its  first  index  ranges from 1 to 2 and the
    second from 0 to the number of dimension of the input-array.

    Element  1,0 in the output-array is the number of dimension of the
    input-array. Element 2,0 is the total size of the array.

    Element  1,1 to 1,n are the lower bounds of the input-array, where
    n  is  the  number  of dimensions. Element 2,1 to 2,n are the high
    bounds of the input-array.

EXAMPLE
    The  following  MLisp  code  inserts  into  the  current buffer an
    analysis of the input-array.

    (setq output-array (bounds-of-array input-array))
    (setq dims (fetch-array output-array 1 0))
    (setq i 1)
    (insert-string (concat "Array has " dims " dimensions\n"))
    (insert-string "low\thigh\n")
    (while (< i dims)
	(insert-string
	    (concat
		(fetch-array output-array 1 i) "\t"
		(fetch-array output-array 2 i) "\n"
	    )
	)
    )

DEFAULT BINDING
    none

SYNOPSIS
    (bounds-of-array <array>)

SEE ALSO
    type-of-expression
    array
    setq-array
    fetch-array
[Cbreakpoint]
COMMAND NAME
    breakpoint

DESCRIPTION
    Sets the breakpoint flag on the specified function. If the flag is
    set   non-zero   for  a  particular  function,  and  the  variable
    breakpoint-hook  contains  the name of a function, just before the
    function is executed, the breakpoint function will be invoked.

DEFAULT BINDING
    None.

SYNOPSIS
    (breakpoint "<function-name>" <value>)

SEE ALSO
    list-breakpoints
    variable breakpoint-hook
[Cbuffer-size]
COMMAND NAME
    buffer-size

DESCRIPTION
    An  MLisp  function  that  returns the number of characters in the
    current buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (buffer-size)

SEE ALSO
[Cc-mode]
COMMAND NAME
    c-mode (package)

DESCRIPTION
    Set  the  major  mode  to C to provide some basic help for writing
    programs in the C language. ESC-j is bound to the extended command
    indent-C-procedure  which  indents  the  procedure in which dot is
    located, ESC-` starts a C comment and ESC-' ends a C comment.

DEFAULT BINDING
    None.

SYNOPSIS
    (c-mode)

SEE ALSO
    indent-C-procedure
    Programming in VAX-11 C for the C language on VAX/VMS.
[Cc=]
COMMAND NAME
    c=

DESCRIPTION
    An  MLisp function that returns 1 if <Char-1> is equal to <Char-2>
    taking  into  account  the  character  translations  indicated  by
    case-fold-search.  If  case-fold-search  is  in effect, then upper
    case letters are "c=" to their lower case equivalents.

DEFAULT BINDING
    None.

SYNOPSIS
    (c= "<Char-1>" "<Char-2>")

SEE ALSO
[Ccase-region-capitalize]
COMMAND NAME
    case-region-capitalize

DESCRIPTION
    Capitalize  all  the  words  in the region between dot and mark by
    making  their  first  characters upper case and all the rest lower
    case.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-region-capitalize)

SEE ALSO
    case-region-invert
    case-region-lower
    case-region-upper
    case-word-capitalize
[Ccase-region-invert]
COMMAND NAME
    case-region-invert

DESCRIPTION
    Invert the case of all alphabetic characters in the region between
    dot  and mark. Lower case characters become upper case characters,
    and upper case characters become lower case.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-region-invert)

SEE ALSO
    case-region-capitalize
    case-region-lower
    case-region-upper
[Ccase-region-lower]
COMMAND NAME
    case-region-lower

DESCRIPTION
    Change  all  alphabetic  characters  in the region between dot and
    mark to lower case.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-region-lower)

SEE ALSO
    case-region-capitalize
    case-region-invert
    case-region-upper
[Ccase-region-upper]
COMMAND NAME
    case-region-upper

DESCRIPTION
    Change  all  alphabetic  characters  in the region between dot and
    mark to upper case.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-region-upper)

SEE ALSO
    case-region-capitalize
    case-region-invert
    case-region-lower
[Ccase-string-capitalize]
COMMAND NAME
    case-string-capitalize

DESCRIPTION
    Capitalize each  word of string-1 by making the first character of
    each  word  upper  case  and all the others lower case. Return the
    capitalized string as the functions result.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-string-capitalize string-1)

SEE ALSO
    case-string-invert
    case-string-lower
    case-string-upper
[Ccase-string-invert]
COMMAND NAME
    case-string-invert

DESCRIPTION
    Invert the  case  of  all  alphabetic  characters  in string-1 and
    return the inverted string as the functions result.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-string-invert)

SEE ALSO
    case-string-capitalize
    case-string-lower
    case-string-upper
[Ccase-string-lower]
COMMAND NAME
    case-string-lower

DESCRIPTION
    Change  the  case  of  all  alphabetic  characters  in string-1 to
    lower-case and return the changed string as the functions result.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-string-lower)

SEE ALSO
    case-string-capitalize
    case-string-invert
    case-string-upper
[Ccase-string-upper]
COMMAND NAME
    case-string-upper

DESCRIPTION
    Change  the  case  of  all  alphabetic  characters  in string-1 to
    upper-case and return the changed string as the functions result.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-string-upper)

SEE ALSO
    case-string-capitalize
    case-string-invert
    case-string-lower
[Ccase-word-capitalize]
COMMAND NAME
    case-word-capitalize

DESCRIPTION
    Capitalize the next <prefix-argument> words (starting with the one
    above or to the left of dot) by making their first character upper
    case and all the others lower case.

DEFAULT BINDING
    None.

SYNOPSIS
    (case-word-capitalize)

SEE ALSO
    case-word-invert
    case-word-lower
    case-word-upper
[Ccase-word-invert]
COMMAND NAME
    case-word-invert

DESCRIPTION
    Invert   the  case  of  all  alphabetic  characters  in  the  next
    <prefix-argument> words starting with the one above or to the left
    of dot.

DEFAULT BINDING
    ESC-^

SYNOPSIS
    (case-word-invert)

SEE ALSO
    case-word-capitalize
    case-word-lower
    case-word-upper
[Ccase-word-lower]
COMMAND NAME
    case-word-lower

DESCRIPTION
    Change  all  alphabetic  characters  in the next <prefix-argument>
    words (starting with the one above or to the left of dot) to lower
    case.

DEFAULT BINDING
    ESC-L

SYNOPSIS
    (case-word-lower)

SEE ALSO
    case-word-capitalize
    case-word-invert
    case-word-upper
[Ccase-word-upper]
COMMAND NAME
    case-word-upper

DESCRIPTION
    Change  all  alphabetic  characters  in the next <prefix-argument>
    words (starting with the one above or to the left of dot) to upper
    case.

DEFAULT BINDING
    ESC-U

SYNOPSIS
    (case-word-upper)

SEE ALSO
    case-word-capitalize
    case-word-invert
    case-word-lower
[Cchange-directory]
COMMAND NAME
    change-directory

DESCRIPTION
    Change  the  current  directory (for EMACS) to <directory-string>.
    All  future  calls  to  commands  which access named files will be
    interpreted relative to the new directory.

DEFAULT BINDING
    None.

SYNOPSIS
    (change-directory "<directory-string>")

SEE ALSO
    current-directory
[Cchar-to-string]
COMMAND NAME
    char-to-string

DESCRIPTION
    Take  a  numeric  argument and returns a one character string that
    results  from  considering  the  number  as  a member of the ASCII
    character set.

DEFAULT BINDING
    None.

SYNOPSIS
    (char-to-string <expression-1>)

SEE ALSO
[Ccheckpoint]
COMMAND NAME
    checkpoint

DESCRIPTION
    Calls the  checkpoint  function whose  name  is  contained  in the
    variable  checkpoint-hook.    The   checkpoint   function   should
    checkpoint EMACS in some   way.   This    command    is   executed
    automatically every checkpoint-frequency key strokes.

DEFAULT BINDING
    None.

SYNOPSIS
    (checkpoint)

SEE ALSO
    checkpoint-buffers
    variable checkpoint-frequency
    variable checkpoint-hook
[Ccheckpoint-buffers]
COMMAND NAME
    checkpoint-buffers

DESCRIPTION
    Write  out  all  modified  buffers  to their associated checkpoint
    files. This function is called as the default checkpoint function.

DEFAULT BINDING
    None.

SYNOPSIS
    (checkpoint-buffers)

SEE ALSO
    checkpoint
    variable checkpoint-frequency
    variable checkpoint-hook
[Ccolumn-to-left-of-window]
COMMAND NAME
    column-to-left-of-window

DESCRIPTION
    Reposition  the current window over the current buffer so that the
    current column is against the left edge of the window.

DEFAULT BINDING
    ESC-@

SYNOPSIS
    (column-to-left-of-window)

SEE ALSO
    scroll-one-line-up
    scroll-one-line-down
    scroll-one-column-right

    window-first-column
[Ccompile-it]
COMMAND NAME
    compile-it (package)

DESCRIPTION
    Write out all the modified file buffers, and execute a DCL command
    to  compile and link the written files. If no <prefix-argument> is
    specified,  then  the  command  executed  is  MMS.  Otherwise, the
    command to executed is requested as a string argument. Output from
    the  command  appears in the buffer "Error log" ready to be parsed
    for  error  messages  with the next-error command. If a command is
    provided  which  is a null string, then the last command supplied,
    if any, will be used.

DEFAULT BINDING
    ^X-^E

SYNOPSIS
    (compile-it "<DCL-command>")

SEE ALSO
    next-error
    previous-error
    Variable compile-it-command
[Cconcat]
COMMAND NAME
    concat

DESCRIPTION
    Take a set of string arguments and return their concatenation.

DEFAULT BINDING
    None.

SYNOPSIS
    (concat <string-expression>...)

SEE ALSO
[Cconvert-key-string]
COMMAND NAME
    convert-key-string

DESCRIPTION
    This  function  takes  a  string and process it according to Emacs
    control  string  processing  rules.   See  the  control-string-...
    variables for details of the processing that is performed.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq converted-string (convert-key-string <string-expression>)

SEE ALSO
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Ccopy-region-to-buffer]
COMMAND NAME
    copy-region-to-buffer

DESCRIPTION
    Copy  the  region  between  dot  and mark to the named buffer. The
    buffer  is  emptied  before the text is copied into it; the region
    between dot and mark is left undisturbed.

DEFAULT BINDING
    None.

SYNOPSIS
    (copy-region-to-buffer "<buffer-name>")

SEE ALSO
    append-region-to-buffer
    delete-region-to-buffer
[Ccorrect-word]
COMMAND NAME
    correct-word (package)

DESCRIPTION
    Use  the  supplied  incorrectly  spelt word and generate a list of
    possible  spelling  corrections  for  it.  Return  this  list as a
    string.

DEFAULT BINDING
    None.

SYNOPSIS
    (correct-word "string")

SEE ALSO
    get-tty-correction
    spell-check-word
[Ccurrent-buffer-name]
COMMAND NAME
    current-buffer-name

DESCRIPTION
    An  MLisp  function  that  returns  the  current  buffer name as a
    string.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-buffer-name)

SEE ALSO
    current-column
    current-directory
    current-file-name
    current-indent
    current-process-name
    current-time
[Ccurrent-column]
COMMAND NAME
    current-column

DESCRIPTION
    An  MLisp  function that returns the printing column number of the
    character immediately following dot.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-column)

SEE ALSO
    current-line-number
    current-buffer-name
    current-directory
    current-file-name
    current-indent
    current-process-name
    current-time
[Ccurrent-directory]
COMMAND NAME
    current-directory

DESCRIPTION
    An  MLisp  function  that  returns  a  string which is the default
    directory  as  set  by the change-directory command, or set to the
    default directory when EMACS starts up.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-directory)

SEE ALSO
    current-buffer-name
    current-column
    current-file-name
    current-indent
    current-process-name
    current-time
[Ccurrent-file-name]
COMMAND NAME
    current-file-name

DESCRIPTION
    An  MLisp  function that returns the file name associated with the
    current  buffer  as a string. If there is no associated file name,
    the null string is returned.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-file-name)

SEE ALSO
    current-buffer-name
    current-column
    current-directory
    current-indent
    current-process-name
    current-time
[Ccurrent-indent]
COMMAND NAME
    current-indent

DESCRIPTION
    An  MLisp  function  the  returns  the amount of white-space at the
    beginning  of the line which dot is in (the printing column number
    of the first non-white-space character).

DEFAULT BINDING
    None.

SYNOPSIS
    (current-indent)

SEE ALSO
    current-buffer-name
    current-column
    current-directory
    current-file-name
    current-process-name
    current-time
[Ccurrent-line-number]
COMMAND NAME
    current-line-number

DESCRIPTION
    This function  returns  the  line number that dot is in. The first
    line in the file is line 1.

SYNOPSIS
    (current-line-number)

SEE ALSO
    current-column
[Ccurrent-process-name]
COMMAND NAME
    current-process-name

DESCRIPTION
    An  MLisp  function  that returns the name of the current process.
    The  current process is defined as being the last process accessed
    by  any  of  the  process commands or the process which caused the
    current  procedure  to be run. If there is no current process, the
    null string is returned.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-process-name)

SEE ALSO
    current-buffer-name
    current-column
    current-directory
    current-file-name
    current-indent
    current-time
[Ccurrent-syntax-entity]
COMMAND NAME
    current-syntax-entity

DESCRIPTION
    An  MLisp  function  which  returns  an  indication of the type of
    syntax entity dot is currently located in according to the current
    syntax  table.  The  value  return is the value of one of the is-*
    constants.

DEFAULT BINDING
    None.

SYNOPSIS
    (current-syntax-entity)

SEE ALSO
    constant is-dull
    constant is-word
    constant is-string
    constant is-quote
    constant is-comment
[Ccurrent-time]
COMMAND NAME
    current-time

DESCRIPTION
    An MLisp function that returns the current time of day as a string
    in the following format: "Wed Jun 30 12:18:21 1982".

DEFAULT BINDING
    None.

SYNOPSIS
    (current-time)

SEE ALSO
    current-buffer-name
    current-column
    current-directory
    current-file-name
    current-indent
    current-process-name
[Cdebug]
COMMAND NAME
    debug (package)

DESCRIPTION
    Use this command to enter the MLisp debugger. When in the debugger
    type "h" to get help on its key bindings. See the EMACS manual for
    details of debugging MLisp code and the debugger.

DEFAULT BINDING
    ESC-^D

SYNOPSIS
    (debug)

SEE ALSO
    trace
    variable trace-mode
[Cdebug-emacs]
COMMAND NAME
    debug-emacs

DESCRIPTION
    This function is for EMACS development work - do not use.

    On VMS:

    When you   execute   the   debug-emacs   command,   the   terminal
    characteristics  are  reset, and control passed to VAX-11 DEBUG so
    that you can examine EMACS' data structures.

DEFAULT BINDING
    None.

SYNOPSIS
    (debug-emacs)

SEE ALSO
[Cdeclare-buffer-specific]
COMMAND NAME
    declare-buffer-specific

DESCRIPTION
    Create  each variable named in <variable-list> that is not already
    bound.  Buffer  specific  bindings  outlive  all  extended command
    calls.  Each  buffer  will  have its own copy of a buffer specific
    variable.

DEFAULT BINDING
    None.

SYNOPSIS
    (declare-buffer-specific <variable-name> ...)

SEE ALSO
    declare-global
[Cdeclare-global]
COMMAND NAME
    declare-global

DESCRIPTION
    Globally create each variable named in <variable-list> that is not
    already bound. Global bindings outlive all extended command calls.

DEFAULT BINDING
    None.

SYNOPSIS
    (declare-global <variable-name> ...)

SEE ALSO
    declare-buffer-specific
[Cdecompile-current-line]
COMMAND NAME
    decompile-current-line

DESCRIPTION
    This  function return the currently executing line decompiled into
    a Mock Lisp expression. If called interactively, then it returns a
    null   string.   If   called  non-interactively,  it  returns  the
    decompiled MLisp for the line about to be traced.

DEFAULT BINDING
    None.

SYNOPSIS
    (decompile-current-line)

SEE ALSO
    trace
    decompile-mlisp-function
    variable trace-mode
    variable trace-hook
[Cdecompile-mlisp-function]
COMMAND NAME
    decompile-mlisp-function

DESCRIPTION
    Decompile  the  MLisp function whose name is supplied, and put the
    output into the current buffer at dot.

DEFAULT BINDING
    None.

SYNOPSIS
    (decompile-mlisp-function "<function-name>")

SEE ALSO
    trace
    decompile-current-line
[Cdefault-emacs-init]
COMMAND NAME
    default-emacs-init

DESCRIPTION

    Add this  function  to your emacsinit.ml to setup the default
    emacs  key  bindings,  mouse behaviour, variable settings and
    autoloads.

    The User  Guide  assumes  the  Emacs  configuration  setup by
    default-emacs-init in its discussion. 

    The user-interface-style  parameter allows default-emacs-init
    to  setup  the  user interface in a particular style.  At the
    moment  the only user-interface-style supported is "windows". 

    In detail  (default-emacs-init "windows") does the following: 

    * Turn off checkpointing. 
    * Turn on journaling. 
    * Set case-fold-search on. 
    * Set silently-kill-processes on 
    * Set activity-indicator on 
    * Set display-end-of-file on 
    * Set ctlchar-with-^ on 
    * Set automatic-horizontal-scrolling on 
    * Set horizontal-scroll-step to 8 
    * Set highlight-region on 
    * Set syntax-colouring on 
    * Turn off track-eol-on-^N-^P 
    * Set expansion-only-completes-unique-choices on 
    * Set input-mode on 
    * Load the pc-keyboard package 
    * Load the pc-mouse package 
    * Load the minibuf-recall package 
    * Add key bindings for the tab handing on Tab and Esc-Tab 
    * Add key bindings for case conversion on Esc-C, Esc-l, Esc-u
      and Esc-i 
    * Add key binding for the minibuf function on Esc-Esc 
    * Auto execute lisp-mode for *.ml, *.key and *.mlp 
    * Auto  execute  electric-c-mode for *.c, *.cc, *.cpp, *.cxx,
      *.h, *.hh, *.hpp and *.hxx 
    * Auto execute Java-mode for *.java 
    * Auto execute JavaScript-mode for *.js 

SYNOPSIS
    (default-emacs-init user-interface-style)

EXAMPLE

    ;
    ; emacsinit.ml
    ;
    (default-emacs-init "windows")
    ;
    ; my customisations
    ;


[Cdefault-global-keymap]
COMMAND NAME
    default-global-keymap

DESCRIPTION
    The  global  keymap  in which EMACS starts. The bindings are those
    wired into EMACS, plus any that have been bound using bind-to-key.

DEFAULT BINDING
    None.

SYNOPSIS
    (default-global-keymap)

SEE ALSO
    use-local-keymap
    use-global-keymap
[Cdefine-buffer-macro]
COMMAND NAME
    define-buffer-macro

DESCRIPTION
    Take  the  contents of the current buffer and define it as a macro
    whose  name  is  associated  with  the  buffer.  This  is  how you
    redefines a macro that has been edited using edit-macro.

DEFAULT BINDING
   None.

SYNOPSIS
    (define-buffer-macro)

SEE ALSO
    define-keyboard-macro
    define-string-macro
    edit-macro
    rename-macro
[Cdefine-global-abbrev]
COMMAND NAME
    define-global-abbrev

DESCRIPTION
    Define  or  redefine  an  abbreviation with the given name for the
    given phrase in the global abbreviation table.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-global-abbrev "<abbreviation>" "<phrase>")

SEE ALSO
    define-local-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    use-abbrev-table
    write-abbrev-file
[Cdefine-hooked-global-abbrev]
COMMAND NAME
    define-hooked-global-abbrev

DESCRIPTION
    Define  or  redefine  an  abbreviation with the given name for the
    given  phrase  in  the  global  abbreviation table and associate a
    function with the abbreviation.

    When  the  abbreviation  is  typed  the  procedure  is called. The
    variable  abbrev-expansion  holds the expanded phrase and dot will
    be at the end of the word.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-hooked-global-abbrev <abbrev> <phrase> <procedure>)

SEE ALSO
    define-hooked-local-abbrev
    abbrev-expansion
    define-global-abbrev
    define-local-abbrev
[Cdefine-hooked-local-abbrev]
COMMAND NAME
    define-hooked-local-abbrev

DESCRIPTION
    Define  or  redefine  an  abbreviation with the given name for the
    given  phrase  in  the  local  abbreviation  table and associate a
    procedure with the abbreviation.

    When  the  abbreviation  is  typed  the  procedure  is called. The
    variable  abbrev-expansion  holds the expanded phrase and dot will
    be at the end of the word.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-hooked-local-abbrev <abbrev> <phrase> <procedure>)

SEE ALSO
    define-hooked-global-abbrev
    abbrev-expansion
    define-global-abbrev
    define-local-abbrev
[Cdefine-keyboard-macro]
COMMAND NAME
    define-keyboard-macro

DESCRIPTION
    Give  a  name  to  the current keyboard macro. A keyboard macro is
    defined   by  using  the  start-remembering  and  stop-remembering
    commands;  define-keyboard-macro takes the current keyboard macro,
    makes  a  copy  of it in a safe place, gives it a name, and erases
    the keyboard macro.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-keyboard-macro "<macro-name>")

SEE ALSO
    execute-keyboard-macro
    start-remembering
    stop-remembering
[Cdefine-keymap]
COMMAND NAME
    define-keymap

DESCRIPTION
    Define a new, empty, keymap with the given name.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-keymap "<keymap>")

SEE ALSO
    use-global-map
    use-local-map
[Cdefine-local-abbrev]
COMMAND NAME
    define-local-abbrev

DESCRIPTION
    Define  (or  redefine) an abbreviation with the given name for the
    given phrase in the local abbreviation table. A local abbreviation
    table must have already been set up with use-abbrev-table.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-local-abbrev "<abbrev-name>" "<expansion>")

SEE ALSO
    define-global-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    use-abbrev-table
    write-abbrev-file
[Cdefine-string-macro]
COMMAND NAME
    define-string-macro

DESCRIPTION
    Define  a macro given a name and a body as a string entered in the
    mini-buffer. Note: to get a control character into the body of the
    macro it must be quoted with ^Q.

DEFAULT BINDING
    None.

SYNOPSIS
    (define-string-macro "<macro-name>" "<body>")

SEE ALSO
    define-buffer-macro
    define-keyboard-macro
    edit-macro
    rename-macro
[Cdefun]
COMMAND NAME
    defun

DESCRIPTION
    An MLisp function that defines a new MLisp function with the given
    name  and  a  body composed of the given expressions. The value of
    the function is the value of the last expression executed.

    An  alternative form of defun has a parameter list. The parameters
    may have default values, see examples for details.

    The  advantage  of the second form is that the arguments are bound
    to  the  parameters  before  the context of the function is setup.
    With  the  first  form of defun the arguments are evaluated in the
    context of the function.

    An example of the difference should make things clearer. The first
    form of defun causes an error for the following:

    (defun
	(function-print
	    num
	    (message "Number is " num)
	)
    )
    (defun
	(call-print
	    num
	    (setq num 12)
	    (function-print num)
	)
    )

    (call-print) => "Number is 0"

    And the same problem solved using the second form of defun.

    (defun
	function-print (num)
	(message "Number is " num)
    )
    (defun
	(call-print
	    num
	    (setq num 12)
	    (function-print num)
	)
    )

    (call-print) => "Number is 12"

DEFAULT BINDING
    None.

SYNOPSIS
    (defun (<function-name> <local-variables> <body>))
    (defun <function-name> (parameter-list) <local-variables> <body>)

SEE ALSO
[Cdelete-buffer]
COMMAND NAME
    delete-buffer

DESCRIPTION
    delete-buffer is used to remove a buffer from the list of buffers,
    and  to throw away all the text it contains. It takes one argument
    which is the name of the buffer to delete.

    If  you  use  the delete-buffer command from the terminal, and the
    target  buffer  is  modified,  delete-buffer  will  ask you if you
    really meant to delete the specified buffer.

    If  you delete the current buffer, then dot is moved to one of the
    other  buffers  displayed  on  the  screen.  Of there are no other
    buffers, then "main" is used (and created if necessary).

DEFAULT BINDING
    None.

SYNOPSIS
    (delete-buffer "<buffer-name>")

SEE ALSO
    kill-buffer
[Cdelete-next-character]
COMMAND NAME
    delete-next-character

DESCRIPTION
    Delete the character next <prefix-argument> characters immediately
    following dot Lines may be merged by deleting newlines.

DEFAULT BINDING
    ^D

SYNOPSIS
    (delete-next-character)

SEE ALSO
[Cdelete-next-word]
COMMAND NAME
    delete-next-word

DESCRIPTION
    Delete  the  next  <prefix-argument>  words starting with the word
    that dot is currently in. If dot is not in a word, all punctuation
    up  to  the  beginning  of the next word is deleted as well as the
    word.

DEFAULT BINDING
    ESC-D

SYNOPSIS
    (delete-next-word)

SEE ALSO
[Cdelete-other-windows]
COMMAND NAME
    delete-other-windows

DESCRIPTION
    Go  back  to  one-window  mode.  Generally  useful  when EMACS has
    spontaneously  generated  a  window  (for  example for the apropos
    command) and you want to get rid of it.

DEFAULT BINDING
    ^X-1

SYNOPSIS
    (delete-other-windows)

SEE ALSO
    delete-other-windows
    delete-window
    enlarge-window
    next-window
    previous-window
    shrink-window
    split-current-window
    split-current-window-vertically
[Cdelete-previous-character]
COMMAND NAME
    delete-previous-character

DESCRIPTION
    Delete <prefix-argument> characters backwards starting immediately
    preceding dot; that is, the character to the left of the terminals
    cursor. If you have just typed a character, RUBOUT will delete it.
    Lines may be merged by deleting newlines.

DEFAULT BINDING
    DEL

SYNOPSIS
    (delete-previous-character)

SEE ALSO
[Cdelete-previous-word]
COMMAND NAME
    delete-previous-word

DESCRIPTION
    Delete <prefix-argument> words backwards starting with the word in
    which  dot  is. If dot is not in a word, then delete all the white
    space up to the previous word as well.

DEFAULT BINDING
    ESC-^H

SYNOPSIS
    (delete-previous-word)

SEE ALSO
[Cdelete-region-to-buffer]
COMMAND NAME
    delete-region-to-buffer

DESCRIPTION
    Delete  all  characters between dot and the mark. The deleted text
    is saved in the specified buffer. This buffer is emptied first.

DEFAULT BINDING
    None.

SYNOPSIS
    (delete-region-to-buffer "<buffer-name>")

SEE ALSO
    append-region-to-buffer
[Cdelete-to-killbuffer]
COMMAND NAME
    delete-to-killbuffer

DESCRIPTION
    Delete  all  characters between dot and the mark. The deleted text
    is moved to the buffer "Kill buffer", which is emptied first.

DEFAULT BINDING
    ^W

SYNOPSIS
    (delete-to-killbuffer)

SEE ALSO
    kill-to-end-of-line
    yank-from-killbuffer
[Cdelete-white-space]
COMMAND NAME
    delete-white-space

DESCRIPTION
    Delete  all  white-space  characters  on  either  side  of dot. The
    definition of white space is used from the current syntax table.

DEFAULT BINDING
    None.

SYNOPSIS
    (delete-white-space)

SEE ALSO
[Cdelete-window]
COMMAND NAME
    delete-window

DESCRIPTION
    Remove  the  current window from the screen and give it's space to
    it's  neighbour  below (or above if it is the bottom window). Make
    the current window and buffer those of the neighbour.

DEFAULT BINDING
    ^X-D

SYNOPSIS
    (delete-window)

SEE ALSO
    delete-other-windows
    enlarge-window
    shrink-window
    split-current-window
    split-current-window-vertically
[Cdescribe-bindings]
COMMAND NAME
    describe-bindings

DESCRIPTION
    Place  in  the  Help window a list of all the keys and the name of
    the  extended  command  that  they  are  bound to. This listing is
    suitable for printing and making your own quick-reference card for
    your own customized version of EMACS.

DEFAULT BINDING
    None.

SYNOPSIS
    (describe-bindings)

SEE ALSO
    describe-command
    describe-key
    describe-variable
    describe-word-in-buffer
[Cdescribe-command]
COMMAND NAME
    describe-command (package)

DESCRIPTION
    Describe  the named extended command. An "extended command" is the
    first   word   that   you   type   to  the  ESC-X  command.  ESC-X
    describe-command describe-command will print the documentation for
    the describe-command extended command.

    The  following  sections  will  help  you  to  understand  how  to
    interpret command descriptions.

    A command description is split into the following sections:

    COMMAND NAME
	Shows the extended command name being described.
	
    DESCRIPTION
	Describes  the  action  of  the  command. Arguments may appear
	between  angled  brackets  to indicate the order in which they
	are  expected. The special string <prefix-argument> is used to
	indicate  how the command treats a numeric prefix argument (e.g.
	one typed using the ^U<digits> sequence).
	
    BINDING
	Shows  the  default key sequence that the command is bound to.
	If  you type this sequence, then the command will be executed.
	ESC  is  used to indicate that the escape (or altmode or meta)
	key  should  be  pressed  and  ^ is used to indicate a control
	character.
	
    SYNOPSIS
	Shows  how  to  call  the  command  from  MLisp, in what order
	arguments  should  be  passed  and  what type of arguments are
	expected.  The  synopsis  is  written  as a fragment of MLisp.
	Arguments  to  the  command  are given in angled brackets: the
	name  relating  to  the  type  of  argument  required.  String
	arguments are shown in string quotes.
	
    SEE ALSO
	Shows related commands.
	
DEFAULT BINDING
    None.

SYNOPSIS
    (describe-command "<command-name>")

SEE ALSO
    describe-bindings
    describe-key
    describe-variable
[Cdescribe-key]
COMMAND NAME
    describe-key

DESCRIPTION
    Print  out  a  line  of  information  describing what the sequence
    <keys> is bound to.

DEFAULT BINDING
    None.

SYNOPSIS
    (describe-key "<keys>")

SEE ALSO
    describe-bindings
    describe-command
    describe-variable
[Cdescribe-variable]
COMMAND NAME
    describe-variable (package)

DESCRIPTION
    Describe  the  named  variable. A "variable" is something that you
    can  set  with the ESC-X set command or print with the ESC-X print
    command.  They  let  the  user fine-tune EMACS to their own taste.
    ESC-X  describe-variable  right-margin  will  print  documentation
    about the right-margin setting.

DEFAULT BINDING
    None.

SYNOPSIS
    (describe-variable "<variable-name>")

SEE ALSO
    describe-bindings
    describe-command
    describe-key
[Cdescribe-word-in-buffer]
COMMAND NAME
    describe-word-in-buffer (package)

DESCRIPTION
    Take the word nearest the cursor and look it up in a data base and
    prints  the  information  found.  This  data  base  contains short
    one-line  descriptions of all of the VAX-11 C standard functions and
    VAX/VMS  System  Services. The idea is that if you have just typed
    in  the  name  of  some  procedure  and can't quite remember which
    arguments go where, just type ^X^D and EMACS will try to tell you.

    This command uses the "subr-names" database search list to find an
    entry, and also attempts to load in the file

	SYS$LOGIN:SUBRNAMES.TLB

    into this search list when it initially starts up. If you have the
    text   library,   then   you   can   make  your  own  entries.  If
    <prefix-argument> is provided, then a copy of the help is put into
    a buffer to be edited (recursively). When you exit (type ^C), then
    the new definition is written into SYS$LOGIN:SUBRNAMES.TLB.

DEFAULT BINDING
    ^X-^D

SYNOPSIS
    (describe-word-in-buffer)

SEE ALSO
    extend-database-search-list
    fetch-database-entry
    fetch-help-database-entry
    list-databases
    put-database-entry
[Cdigit]
COMMAND NAME
    digit

DESCRIPTION
    This  command  is  bound  to  the  digit  keys after a call to the
    argument-prefix   command   and  until  a  non-digit  function  is
    executed.  This  means  that  the  bindings for the digits changes
    during  prefix argument collection, so that the prefix argument is
    correctly evaluated.

DEFAULT BINDING
    None.

SYNOPSIS
    None.

SEE ALSO
    argument-prefix
[Cdot]
COMMAND NAME
    dot

DESCRIPTION
    An  MLisp  function  that  returns the number of characters to the
    left  of dot plus 1 (i.e. if dot is at the beginning of the buffer,
    (dot) returns 1).

DEFAULT BINDING
    None.

SYNOPSIS
    (dot)

SEE ALSO
[Cdot-is-visible]
COMMAND NAME
    dot-is-visible

DESCRIPTION
    An MLisp function that returns 1 if dot is in a window.

DEFAULT BINDING
    None.

SYNOPSIS
    (dot-is-visible)

SEE ALSO
[Cdown-window]
COMMAND NAME
    down-window

DESCRIPTION
    This commands  move  the  cursor into the window that is below the
    current  window.  Unlike next-window that moves the cursor without
    regard to the physical position of the windows on the screen.

    down-window reports  an  error  of there is not a window below the
    current window.

DEFAULT BINDING
    None.

SYNOPSIS
    (down-window)

SEE ALSO
    up-window
    left-window
    right-window
[Cdump-abbreviation-tables]
COMMAND NAME
    dump-abbreviation-tables

DESCRIPTION
    Dump  a  readable listing of an abbreviation table into the buffer
    "Abbreviation table" and make the buffer visible.

    The listing  includes the abbreviation, its expansion and the name
    of any hook MLisp function associated with the abbreviation.

DEFAULT BINDING
    None.

SYNOPSIS
    (dump-abbreviation-tables)

SEE ALSO
    list-abbreviation-tables
[Cdump-memory-statistics]
COMMAND NAME
    dump-memory-statistics

DESCRIPTION
    This  is  a  function used by the EMACS developers to determine the
    memory usage of EMACS.

DEFAULT BINDING
    None.

SYNOPSIS
    (dump-memory-statistics)

SEE ALSO
[Cdump-stack-trace]
COMMAND NAME
    dump-stack-trace

DESCRIPTION
    Dump an MLisp stack trace to the buffer "Stack trace". This can be
    used  to  debug  MLisp  functions.  The stack trace consists of an
    indication  of  what  was executing when the trace occurred, and a
    line for each function that was active.

DEFAULT BINDING
    None.

SYNOPSIS
    (dump-stack-trace)

SEE ALSO
    Variables trace-mode
[Cdump-syntax-table]
COMMAND NAME
    dump-syntax-table

DESCRIPTION
    Dump  a  readable listing of a syntax table into a buffer and make
    that buffer visible.

    Each  ASCII  character  in  the  range  0  to  127  is  listed  on
    consecutive  lines.  For  runs  of  characters  that have the same
    characteristic,  the  upper  and  lower  bounds are separated by a
    dash.

    Dull characters are indicated by no flag.

    Word characters are indicated with the `w' flag.

    Quote characters are indicated with a '\'/ flag.

    Brackets are  indicated  with  a  '(' or ')' and the corresponding
    bracket character written out.

    Comment  characters  are  indicated  with  a  '{' or '}' flag. The
    second character is also displayed.

DEFAULT BINDING
    None.

SYNOPSIS
    (dump-syntax-table)

SEE ALSO
[Cedit-macro]
COMMAND NAME
    edit-macro

DESCRIPTION
    Take  the  body  of  a named macro and place it in a buffer called
    Macro  edit.  The  name of the macro is associated with the buffer
    and  appears  in  the  mode  line at the bottom of the window. The
    buffer may be edited just like any other buffer (this is, in fact,
    the  intent).  After  the  macro  body  has  been edited it may be
    redefined using define-buffer-macro.

DEFAULT BINDING
    None.

SYNOPSIS
    (edit-macro "<macro-name>")

SEE ALSO
    define-buffer-macro
    define-keyboard-macro
    define-string-macro
    rename-macro
[Cemacs-version]
COMMAND NAME
    emacs-version

DESCRIPTION
    An  MLisp function that returns a string describing the version of
    EMACS  that  you are running. For version 5.0 of EMACS the string
    will look something like

	V5.0 linked on  6-OCT-1989 at 15:25 on LEGB4 by SCOTT

    The  string  contains  the version number "V5.0" the date when the
    EMACS  was made "6-OCT-1989 at 15:25" and the node where EMACS
    was made "LEGB4" and sho built Emacs "SCOTT"

    When  reporting  bugs or suggestions about EMACS please quote the
    version number of EMACS that the comments are about.

DEFAULT BINDING
    None.

SYNOPSIS
    (emacs-version)

SEE ALSO
    EMACS release notes.
[Cend-of-file]
COMMAND NAME
    end-of-file

DESCRIPTION
    Move dot to just after the last character of the buffer.

DEFAULT BINDING
    ESC->

SYNOPSIS
    (end-of-file)

SEE ALSO
    beginning-of-file
[Cend-of-line]
COMMAND NAME
    end-of-line

DESCRIPTION
    Move  dot  to  the  end  of  the  line  in the current buffer that
    contains  the  cursor;  that  is,  to  just  after  the  following
    end-of-line or the end of the buffer.

DEFAULT BINDING
    ^E

SYNOPSIS
    (end-of-line)

SEE ALSO
    beginning-of-line
[Cend-of-window]
COMMAND NAME
    end-of-window

DESCRIPTION
    Move dot to just after the last character visible in the window.

DEFAULT BINDING
    ESC-.

SYNOPSIS
    (end-of-window)

SEE ALSO
    beginning-of-window
    line-to-top-of-window
    page-next-window
[Cenlarge-window]
COMMAND NAME
    enlarge-window

DESCRIPTION
    Make  the  current  window <prefix-argument> lines taller, and the
    window  below  (or  the  one  above  if  there is no window below)
    <prefix-argument>  lines  shorter.  Can't be used if there is only
    one window on the screen.

DEFAULT BINDING
    ^X-Z

SYNOPSIS
    (enlarge-window)

SEE ALSO
    delete-other-windows
    delete-window
    enlarge-window
    next-window
    previous-window
    shrink-window
    split-current-window
    split-current-window-vertically
[Ceobp]
COMMAND NAME
    eobp

DESCRIPTION
    An  MLisp  function  that  returns  1  if dot is at the end of the
    buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (eobp)

SEE ALSO
    bobp
[Ceolp]
COMMAND NAME
    eolp

DESCRIPTION
    An MLisp function that returns 1 if dot is at the end of a line.

DEFAULT BINDING
    None.

SYNOPSIS
    (eolp)

SEE ALSO
    bolp
[Cerase-buffer]
COMMAND NAME
    erase-buffer

DESCRIPTION
    Delete  all  text  from  the  current buffer and set the buffer as
    unmodified.  It does not ask to make sure if you really want to do
    it.

DEFAULT BINDING
    None.

SYNOPSIS
    (erase-buffer)

SEE ALSO
    erase-region
[Cerase-region]
COMMAND NAME
    erase-region

DESCRIPTION
    Delete  all  text  between dot and mark in the current buffer. It
    does not ask to make sure if you really want to do it.

DEFAULT BINDING
    None.

SYNOPSIS
    (erase-region)

SEE ALSO
    erase-buffer
[Cerror-and-exit]
COMMAND NAME
    error-and-exit

DESCRIPTION
    This  function  is  used in the mini-buffer local keymaps to abort
    the  current  input operation. If called directly this function is
    the same as exit-emacs.

DEFAULT BINDING
    ^G	- in the mini-buffer

SYNOPSIS
    (error-and-exit)

SEE ALSO
    help-and-exit
    expand-and-exit
    exit-emacs
    Minibuf-local-NS-map
    Minibuf-local-map
[Cerror-message]
COMMAND NAME
    error-message

DESCRIPTION
    Concatenate  the  supplied string expressions and send then to the
    screen as an error message where they will appear at the bottom of
    the screen. EMACS will return to keyboard level and output a bell.

DEFAULT BINDING
    None.

SYNOPSIS
    (error-message "<string-expression>"...)

SEE ALSO
[Cerror-message-parser]
COMMAND NAME
    error-message-parser (package)

DESCRIPTION
    Parse  error  message  for  the parse-error-message-in-region  and
    compile-it commands.

    This  function  is  the  default  error  message  parser  for  the
    parse-error-message-in-region   command.  It  understands  how  to
    locate error messages for VAX C, PL/I and BLISS.

DEFAULT BINDING
    None.

SYNOPSIS
    (error-message-parser)

SEE ALSO
    parse-error-message-in-region
    Variable error-file-name
    Variable error-line-number
    Variable error-message-parser
    Variable error-start-position
[Cerror-occurred]
COMMAND NAME
    error-occurred

DESCRIPTION
    Execute  the  given  expressions  and  ignore their values. If all
    expressions execute successfully, then return 0. Otherwise, return
    1  and  all  expressions after the one which encountered the error
    will not be executed.

    The  text  of any error message is available for processing in the
    string variable error-message.

DEFAULT BINDING
    None.

SYNOPSIS
    (error-occurred <expressions>)

SEE ALSO
    Variable  error-message
[Cexchange-dot-and-mark]
COMMAND NAME
    exchange-dot-and-mark

DESCRIPTION
    Set dot to the currently marked position and mark the old position
    of dot. Useful for bouncing back and forth between two points in a
    file;  particularly useful when the two points delimit a region of
    text  that  is  going  to  be  operated on by some command like ^W
    (delete-to-killbuffer).

DEFAULT BINDING
    ^X-^X

SYNOPSIS
    (exchange-dot-and-mark)

SEE ALSO
    set-mark
    region-around-match
    mark
[Cexecute-extended-command]
COMMAND NAME
    execute-extended-command

DESCRIPTION
    Prompt  in  the  mini-buffer  for a command from the extended set.
    These  deal with rarely used features. Commands are parsed using a
    command  completion:  You  can type ESC or space to invoke command
    completion, or  '?'  for help with what you are allowed to type at
    that point.  This  does not work if it's asking for a key or macro
    name or something.

DEFAULT BINDING
    ESC-X

SYNOPSIS
    (execute-extended-command "<extended-command-string>")

SEE ALSO
[Cexecute-keyboard-macro]
COMMAND NAME
    execute-keyboard-macro

DESCRIPTION
    Take  the  keystrokes remembered with ^X-( and ^X-) and treat them
    as  though  they  had  been  typed again. This is a cheap and easy
    macro facility.

DEFAULT BINDING
    ^X-E

SYNOPSIS
    (execute-keyboard-macro)

SEE ALSO
    define-keyboard-macro
    start-remembering
    stop-remembering
[Cexecute-mlisp-buffer]
COMMAND NAME
    execute-mlisp-buffer

DESCRIPTION
    Parse  and  compile  the  current  buffer  as  as  a  single MLisp
    expression and execute it. This is the function generally used for
    testing  out  new  functions:  place  your  functions  in a buffer
    wrapped in a progn and use execute-mlisp-buffer to define them.

	CAVEAT:   Only   one   expression   is  parsed  and  compiled.
	subsequent expressions are ignored.

DEFAULT BINDING
    None.

SYNOPSIS
    (execute-mlisp-buffer)

SEE ALSO
    execute-mlisp-file
    execute-mlisp-line
[Cexecute-mlisp-file]
COMMAND NAME
    execute-mlisp-file

DESCRIPTION
    Read  the  named file as a series of MLisp expressions and execute
    them.   Typically   the   file   consists  primarily  of  defun's,
    buffer-specific    variable    assignments   and   key   bindings.
    execute-mlisp-file  is usually used to load macro libraries and is
    used  to  load  "emacsinit.ml" from your home directory when EMACS
    starts up.

    The  file  name  given  is  interpreted relative to the EMACS$PATH
    logical  name.  This should be set up to contain a comma separated
    list  of  device/directory  specification  used  to search for the
    file.  If  the  file  cannot  be  located using EMACS$PATH, then a
    search is made in the database search list "MLisp-library".

    The default file type is .ML

DEFAULT BINDING
    None.

SYNOPSIS
    (execute-mlisp-file "<file-name>")

SEE ALSO
    execute-mlisp-buffer
    execute-mlisp-line
[Cexecute-mlisp-line]
COMMAND NAME
    execute-mlisp-line

DESCRIPTION
    Prompt  for  a  string  in  the  mini-buffer, parse it as an MLisp
    expression and execute it.

    If  called from a Mock Lisp program, the command to be executed is
    supplied as a string. This provides for dynamic compilation during
    the execution of a program.

DEFAULT BINDING
    ESC-ESC

SYNOPSIS
    (execute-mlisp-line "<S-expression>")

SEE ALSO
    execute-mlisp-buffer
    execute-mlisp-file
[Cexecute-monitor-command]
COMMAND NAME
    execute-monitor-command

DESCRIPTION
    Execute  a  DCL  command  placing  its output into a buffer called
    "Command  execution"  and  making that buffer visible in a window.
    The  command  will not be able to read from its SYS$INPUT since it
    will be connected to NLA0:.

DEFAULT BINDING
    ^X-!

SYNOPSIS
    (execute-monitor-command "<DCL-command>")

SEE ALSO
    filter-region
[Cexit-emacs]
COMMAND NAME
    exit-emacs

DESCRIPTION
    If  the  current  recursion depth is zero, exit EMACS. The command
    will  ask for confirmation if there are any buffers that have been
    modified but not written out.

    If this  function is called with a prefix argument, it will prompt
    for  a  command to execute in the parent process.  This feature is
    implemented  in  the EMACSSHR image and will not work if EMACS was
    simply SPAWNED or ATTACHED using the DCL commands.

    If  the  recursion  depth  is  not  zero,  then  leave the current
    recursive edit call.

DEFAULT BINDING
    ^C or ESC-^C

SYNOPSIS
    (exit-emacs)
    (exit-emacs "<DCL-command>")

SEE ALSO
    recursive-edit
    write-files-exit
[Cexpand-and-exit]
COMMAND NAME
    expand-and-exit

DESCRIPTION
    This  function  is  used in the mini-buffer local keymaps to cause
    command  completion.  If called directly this function is the same
    as exit-emacs.

DEFAULT BINDING
    ESC - in the mini-buffer

SYNOPSIS
    (expand-and-exit)

SEE ALSO
    help-and-exit
    error-and-exit
    exit-emacs
    Minibuf-local-NS-map
    Minibuf-local-map
[Cexpand-file-name]
COMMAND NAME
    expand-file-name

DESCRIPTION

    An MLisp   function  that  return  the  fully  expanded  file
    specification of the its file name argument.

    Wild-cards may  be  specified  to  obtain  a sequence of file
    names.   Call  expand-file-name first with the wild-card file
    specification.   Then call expand-file-name specifying a null
    filename until a null string is returned.

DEFAULT BINDING
    None.

SYNOPSIS
    (expand-file-name <file-name>)

SEE-ALSO
    expand-file-name-recursive
    file-exists
    unlink-file
[Cexpand-file-name-recursive]
COMMAND NAME
    expand-file-name-recursive

DESCRIPTION

    An MLisp  function that returns a sequence of file names.

    The <file-name>   parameter  is  expanded  and  defaulted  to
    provide  the  starting  point for the sequence.  The expanded
    file-name  is split into its directory part and its file name
    part.

    expand-file-name-recursive returns  the  names  of  all files
    that  match the filename part in each directory starting from
    the directory part.

    Wild-cards may  be  specified  in the filename part.

    Call expand-file-name-recursive    first    with   the   file
    specification.     Then    call    expand-file-name-recursive
    specifying a null filename until a null string is returned.

DEFAULT BINDING
    none.

SYNOPSIS
   (expand-file-name-recursive <file-name>)

EXAMPLE

    Find all  the  html source files in c:\webproject and all the
    directoruies within c:\webproject.

    (defun
	(list-html
	    ~files ~file
	    (setq ~files "c:\\webproject\\*.html")
	    (while
		(progn
		    (setq ~file (expand-file-name-recursive ~files))
		    (!= ~file "")
		)
		(insert-file ~file "\n")
	    )
	)
    )


SEE ALSO
    expand-file-name
    file-exists
    unlink-file

[Cexpand-from-string-table]
COMMAND NAME
    expand-from-string-table

DESCRIPTION
    This  function  provides  access  to  the  Emacs  input  expansion
    mechanism.  The function has two parameters the first is a prompt.
    The  second  is  a  string that contains a space separated list of
    words  that  the  users  is  to  select  from.   The result of the
    function is the selected word.  expand-from-string-table will only
    allow one of the words in the list to be returned.

EXAMPLE
    (expand-from-string-table "Command: " "exit" "read" "write" "quit")

    The user  will be prompted to enter one of the four words from the
    list, "exit", "read", "write", "quit".

DEFAULT BINDING
    None.

SYNOPSIS
    (expand-from-string-table <prompt-string> <word-list-string>)

SEE ALSO
[Cexpand-mlisp-variable]
COMMAND NAME
    expand-mlisp-variable (package)

DESCRIPTION
    Prompt for the name of a declared variable then insert the name as
    text  into  the  current buffer. This is very handy  for typing in
    MLisp  functions.  It's also fairly useful to bind it to a key for
    easy access.

DEFAULT BINDING
    None.

SYNOPSIS
    (expand-mlisp-variable "<variable-name>")

SEE ALSO
    expand-mlisp-word
[Cexpand-mlisp-word]
COMMAND NAME
    expand-mlisp-word (package)

DESCRIPTION
    Prompt for the name of a command then insert the name as text into
    the  current  buffer.  This  is  very  handy   for typing in MLisp
    functions.  It's  also  fairly useful to bind it to a key for easy
    access.

DEFAULT BINDING
    None.

SYNOPSIS
    (expand-mlisp-word "<command-name>")

SEE ALSO
    expand-mlisp-variable
[Cextend-database-search-list]
COMMAND NAME
    extend-database-search-list

DESCRIPTION

    Add  the  given  data  base  file  to  the  data base search list
    <dbname>.  If  the database is already in the search list then it
    is  left, otherwise the new database is added at the beginning of
    the  list  of databases. Database files are VAX/VMS text, help or
    macro libraries.

    <access>  specifies  the  type of access required using a bit mask
    and is optional. These bits are defined as follows

	Bit 0	force read-only access.

	Bit 1	Keep file open between database accesses.

DEFAULT BINDING
    None.

SYNOPSIS
    (extend-database-search-list "<dbname>" "<file-name>" <access>)

SEE ALSO
    fetch-database-entry
    fetch-help-database-entry
    list-databases
    put-database-entry
[Cexternal-function]
COMMAND NAME
    external-function

DESCRIPTION
    This function  declares  "function" as being provided by "routine"
    in  the  "file".  The  "file" is dynamically linked into the EMACS
    process  when  the "function" is called. See the EMACS programmers
    guide for details of how to write an EMACS external function.

DEFAULT BINDING
    None.

SYNOPSIS
    (external-function function file routine)

EXAMPLE
    (external-function
	"sys$fao"
	"emacs$share:emacs_sys_shr"
	"emacs$sys$fao"
    )

SEE ALSO
    list-images
[Cfetch-array]
COMMAND NAME
    fetch-array

DESCRIPTION
    This command retrieves a value from an array. The arguments to this
    function  are  the array and the indices of the array. fetch-array
    check that the there exactly the right number of indices and range
    checks them.

DEFAULT BINDING
    none.

EXAMPLE
    Retrieve element 3,5 from array 2d-array.

    (setq value (fetch-array 2d-array 3 5))

SYNOPSIS
    (fetch-array <array> <indices> ...)
SEE ALSO
    type-of-expression
    bounds-of-array
    setq-array
    array
[Cfetch-database-entry]
COMMAND NAME
    fetch-database-entry

DESCRIPTION
    Read the entry in the data base corresponding to the given key and
    insert it into the current buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (fetch-database-entry "<dbname>" "<key>")

SEE ALSO
    extend-database-search-list
    fetch-help-database-entry
    list-databases
    put-database-entry
[Cfetch-help-database-entry]
COMMAND NAME
    fetch-help-database-entry (VMS)

DESCRIPTION
    Take the entry in the help database corresponding to the given key
    string and insert it into the current buffer.

    The key string is a sequence of up to 9 white space separated keys
    to search for. For example, to insert the help for the entry

	copy /log

    the string would be

	"copy /log"

    The  variable  fetch-help-database-flags  is  used  to control the
    information retrieved from the help database file.

DEFAULT BINDING
    None.

SYNOPSIS
    (fetch-help-database-entry "<dbname>" "<keys-string>")

SEE ALSO
    extend-database-search-list
    fetch-database-entry
    list-databases
    put-database-entry
    Variable fetch-help-database-flags
[Cfile-exists]
COMMAND NAME
    file-exists

DESCRIPTION

    An  MLisp function that returns 1 if the specified file exists and
    is  writable,  -1  if  the file exists but is only readable, and 0
    otherwise.

DEFAULT BINDING
    None.

SYNOPSIS
    (file-exists "<file-name>")

SEE ALSO
    expand-file-name
    file-is-a-directory

[Cfile-name-expand-and-default]
COMMAND NAME
    file-name-expand-and-default

DESCRIPTION

    file-name-expand-and-default returns  a  fully  expanded  filename
    based   on  <filename>.   Any  missing  parts  of  a  filename  in
    <filename>  will  be  defaulted from <default> first and then from
    the current working directory.

DEFAULT BINDING
    none.

SYNOPSIS
    (file-name-expand-and-default <filename> <default>)

EXAMPLE
    The current directory is D:\project then the following:

    (file-name-expand-and-default "emacs" ".\\editor\\.cpp")

    returns: d:\project\editor\emacs.cpp

    (file-name-expand-and-default "emacs.cxx" ".cpp")

    returns: d:\project\emacs.cxx

    (file-name-expand-and-default ".h" "d:\\project\\editor\\emacs.cpp")

    returns: d:\project\editor\emacs.h

SEE ALSO
    expand-file-name
    expand-file-name-recursive
[Cfile-is-a-directory]
COMMAND NAME
    file-is-a-directory

DESCRIPTION

    file-is-a-directory return  true  if its <filename> parameter
    is a directory.

DEFAULT BINDING
    none.

SYNOPSIS
    (file-is-a-directory <filename>)

EXAMPLE

    (file-is-a-directory "/tmp")

    (file-is-a-directory "c:\\temp")

SEE ALSO

[Cfile-format-string]
COMMAND NAME
    file-format-string

DESCRIPTION

    file-format-string is  used  to create formatted string based
    on parts of filenames.

    The format  string  is  made  of  format  specifications  and
    literal  characters.   literal  characters  are copied to the
    output  as  is.   format  specifications  are  replaced  with
    appropiate text.

    The format specification syntax is

	% <arg> <fmt-type>

    The optional arg is a positive or negative decimal number.

    The fmt-type is one of

	Type	Description
	----	-----------
	%	literal % character
	fn	filename
	ft	filetype
	fa	filename and filetype
	pc	path character
	pa	disk and directory
	ps	set path split point
	ph	path head
	pt	path tail
	pv	disk
	pd	directory

EXAMPLES

    The examples are based on the file:

	c:\users\barry\bemacs\emacsinit.ml

    Example	Output
    -------	------
    %%		%
    %fn		emacsinit
    %ft		.ml
    %fa		emacsinit.ml
    %pc		\
    %pa		c:\users\barry\bemacs\

    %4ps%ph	c:\users\barry\bemacs\
    %3ps%ph	c:\users\barry\bemacs\
    %2ps%ph	c:\users\barry\
    %1ps%ph	c:\users\
    %0ps%ph	c:\users\barry\bemacs\
    %-1ps%ph	c:\users\barry\
    %-2ps%ph	c:\users\

    %3ps%pt		
    %2ps%pt	bemacs\
    %1ps%pt	barry\bemacs\
    %0ps%pt
    %-1ps%pt	bemacs\
    %-2ps%pt	barry\bemacs\

    %pv		c:
    %pd		\users\barry\bemacs\


DEFAULT BINDING
    none.

SYNOPSIS
    (file-format-string <format> <filename>)

EXAMPLE

    ; return the filename and type
    (file-format-string "%fa" current-buffer-filename)
    ; seperate the filename and path
    (file-format-string "File: %fa Path: %pa" current-buffer-filename)

SEE ALSO
    sprintf

[Cfilter-region]
COMMAND NAME
    filter-region

DESCRIPTION
    Take  the region between dot and mark and pass it to Sys$Input for
    the  given  command line. Sys$Output from the command replaces the
    region  between  dot  and mark. Use this to run a region through a
    program.

    The  contents  of  the  old  region  are saved in the buffer "Kill
    buffer".

DEFAULT BINDING
    None.

SYNOPSIS
    (filter-region "<DCL-command>")

SEE ALSO
    execute-monitor-command
[Cfollowing-char]
COMMAND NAME
    following-char

DESCRIPTION
    An MLisp function that returns the character immediately following
    dot as an integer. The null character (0) is returned if dot is at
    the end of the buffer.

    Remember  that  dot  is not `at' some character, it is between two
    characters.

DEFAULT BINDING
    None.

SYNOPSIS
    (following-char)

SEE ALSO
    preceding-char
[Cforce-exit-process]
COMMAND NAME
    force-exit-process

DESCRIPTION
    Force  the image running is the specified sub-process to exit with
    the specified code. This can be used to stop an image that has run
    a-muck. All exit handlers for the image will be run.

    If  no image is running in the process, the exit will be queued up
    and  executed  next  time the target process executes in user mode
    (i.e. next time an image is run).

DEFAULT BINDING
    None.

SYNOPSIS
    (force-exit-process "<process-name>" <exit-code>)

SEE ALSO
    current-process-name
    kill-process
    list-processes
    pause-process
    resume-process
    set-current-process
    set-process-name
    start-DCL-process
    wait-for-process-input-request
[Cforward-balanced-paren-line]
COMMAND NAME
    forward-balanced-paren-line

DESCRIPTION
    Move dot forward until either the end of the buffer is reached, or
    an unmatched close parenthesis is encountered, or the beginning of
    a  line  is  encountered  at  "parenthesis  level  zero". That is,
    without  an  unmatched  '('  existing  between  the  line  and the
    starting position of dot.

    The  definitions of parenthesis are used from the syntax table for
    the  current buffer. The parameter is the number of parentheses to
    ignore before applying the match.

DEFAULT BINDING
    None.

SYNOPSIS
    (forward-balanced-paren-line <expression-1>)

SEE ALSO
    backward-balanced-paren-line
    backward-paren
    forward-paren
[Cforward-character]
COMMAND NAME
    forward-character

DESCRIPTION
    Move  dot  forward  <prefix-argument>  character. End-of-lines and
    tabs  each count as one character. You can't move forward to after
    the end of the buffer.

DEFAULT BINDING
    ^F

SYNOPSIS
    (forward-character)

SEE ALSO
    backward-character
[Cforward-paragraph]
COMMAND NAME
    forward-paragraph (package)

DESCRIPTION
    Move  to  the end of the current or following paragraph. Blank and
    DSR command lines separate paragraphs and are not parts of then.

    Paragraphs are defined by the variable paragraph-delimiters.

DEFAULT BINDING
    ESC-)

SYNOPSIS
    (forward-paragraph)

SEE ALSO
    backward-paragraph
[Cforward-paren]
COMMAND NAME
    forward-paren

DESCRIPTION
    Move  dot forward until an unmatched close parenthesis, or the end
    of  the  buffer is found. This can be used to aid in skipping over
    Lisp S-expressions.

    The  definitions of parenthesis are used from the syntax table for
    the  current  buffer. The argument is the number of parentheses to
    ignore before applying the match.

DEFAULT BINDING
    None.

SYNOPSIS
    (forward-paren)

SEE ALSO
    backward-paren
[Cforward-sentence]
COMMAND NAME
    forward-sentence (package)

DESCRIPTION
    Move dot forward to the end of a sentence.

    A sentence is bounded by the string sentence-delimiters.

DEFAULT BINDING
    ESC-E

SYNOPSIS
    (forward-sentence)

SEE ALSO
    backward-sentence
[Cforward-word]
COMMAND NAME
    forward-word

DESCRIPTION
    Move  dot  forward  to the end of the <prefix-argument>th word. If
    not  currently  in  the  middle  of  a  word, skip all intervening
    punctuation.  Then  skip  over  the  words, leaving dot positioned
    after the last character of the word.

    A word is defined in the current syntax table.

DEFAULT BINDING
    ESC-F

SYNOPSIS
    (forward-word)

SEE ALSO
    backward-word
[Cfundamental-mode]
COMMAND NAME
    fundamental-mode (package)

DESCRIPTION
    A  major  mode  similar  to  normal-mode.  It  provides  a natural
    environment  for  processing ordinary text, or program sources for
    which  there  is no specialised language mode. Its major advantage
    over normal-mode, is that it displays bracket balancing.

DEFAULT BINDING
    None.

SYNOPSIS
    (fundamental-mode)

SEE ALSO
    normal-mode
    text-mode
    c-mode
    lisp-mode
[Cget-tty-buffer]
COMMAND NAME
    get-tty-buffer

DESCRIPTION

    Prompt for  a  buffer  name using command completion and providing
    help.  The function returns the name of the buffer as a string.

    The optional  <default-value>  parameter will provides the default
    input.

    To insert a buffer name into the buffer:-

       (insert-string (get-tty-buffer "Save buffer: ")).

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-buffer "<prompt-string>" ["<default-value>"])

SEE ALSO
    get-tty-character
    get-tty-command
    get-tty-correction
    get-tty-file
    get-tty-string
    get-tty-variable
[Cget-tty-character]
COMMAND NAME
    get-tty-character

DESCRIPTION
    Read  a  single  character  from  the terminal and return it as an
    integer.  The  cursor is not moved to the message area: it is left
    in  the  text  window.  This  is  useful  when writing things like
    query-replace and incremental search programs in MLisp.

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-character)

SEE ALSO
    get-tty-buffer
    get-tty-command
    get-tty-correction
    get-tty-file
    get-tty-string
    get-tty-variable
[Cget-tty-command]
COMMAND NAME
    get-tty-command

DESCRIPTION
    Prompt  for  the  name  of  a  declared  function  (using  command
    completion and providing help) and return the name of the function
    as a string.

    The optional  <default-value>  parameter will provides the default
    input.

    For example, the expand-mlisp-word function is simply

	(insert-string (get-tty-command ": expand-mlisp-word ")).

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-command "<command-name>" ["<default-value>"])

SEE ALSO
    get-tty-buffer
    get-tty-character
    get-tty-correction
    get-tty-file
    get-tty-string
    get-tty-variable
[Cget-tty-correction]
COMMAND NAME
    get-tty-correction (package)

DESCRIPTION
    Uses  the supplied misspelt word and the prompt string to obtain a
    correction  for  the  word  from  the user. Full word expansion is
    enabled.

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-correction "word" <"prompt">)

SEE ALSO
    get-tty-buffer
    get-tty-character
    get-tty-command
    get-tty-file
    get-tty-string
    get-tty-variable
    correct-word
    spell-check-word
[Cget-tty-file]
COMMAND NAME
    get-tty-file

DESCRIPTION
    Prompt  for  a  file  name  using command completion and providing
    help. The function returns the name of the file as a string.

    The optional  <default-value>  parameter will provides the default
    input.

    To insert a file name into the buffer:-

       (insert-string (get-tty-file "Next file name: ")).

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-file "<prompt-string>" ["<default-value>"])

SEE ALSO
    get-tty-buffer
    get-tty-character
    get-tty-command
    get-tty-correction
    get-tty-string
    get-tty-variable
[Cget-tty-string]
COMMAND NAME
    get-tty-string

DESCRIPTION
    Read  a string from the terminal using its single string parameter
    for  a  prompt.  Generally  used  inside  MLisp  programs  to  ask
    questions.

    The optional  <default-value>  parameter will provides the default
    input.

DEFAULT BINDING
    None.

SYNOPSIS
    (get-tty-string "<prompt>" ["<default-value>"])

SEE ALSO
    get-tty-buffer
    get-tty-character
    get-tty-command
    get-tty-correction
    get-tty-file
    get-tty-variable
[Cget-tty-variable]
COMMAND NAME
    get-tty-variable

DESCRIPTION
    Prompt  for  the  name  of  a  declared  variable  (using  command
    completion and providing help) and return the name of the variable
    as a string.

    The optional  <default-value>  parameter will provides the default
    input.

DEFAULT BINDING
    None.

SYNOPSIS
     (get-tty-variable "<prompt>" ["<default-value>"])

SEE ALSO
    get-tty-buffer
    get-tty-character
    get-tty-command
    get-tty-correction
    get-tty-file
    get-tty-string
[Cgetenv]
COMMAND NAME
    getenv

DESCRIPTION
    Returns the value of the named environment symbol.

DEFAULT BINDING
    None.

SYNOPSIS
    (getenv "<env-name">)

SEE ALSO
[Cglobal-binding-of]
COMMAND NAME
    global-binding-of

DESCRIPTION
    An  MLisp  function that returns the name of the function that the
    provided keystroke will invoke, using the global keymap.

DEFAULT BINDING
    None.

SYNOPSIS
    (global-binding-of "<key-sequence>")

SEE ALSO
    local-binding-of
    describe-binding
[Cgoto-character]
COMMAND NAME
    goto-character

DESCRIPTION
    Go  to  the  given  character-position. (goto-character 5) goes to
    character position 5.

DEFAULT BINDING
    None.

SYNOPSIS
    (goto-character <expression>)

SEE ALSO
[Cgoto-window-at-x-y]
COMMAND NAME
    goto-window-at-x-y

DESCRIPTION
    This  MLisp  function  is  for  supporting  a mouse style pointing
    device.    The   function   takes  two  arguments,  the  X  and  Y
    co-ordinates of the pointer on the screen.

    goto-window-at-x-y performs  two  operations, first it changes the
    current  window  to  be  the  one  at the X and Y co-ordinates.  A
    window  is  considered  to  be  the  mode  line of the window, the
    vertical  dividing line to its right and the content region of the
    window.

    If the  co-ordinates are in the contents of the window dot will be
    set  to  the  nearest  position in the buffer to the co-ordinates.
    Otherwise dot is left as it was.

    goto-window-at-x-y returns  an integer result that tells you which
    part of a window the co-ordinates are in.

	0   - in the content region of the window
	1   - in the mode-line
	2   - in the vertical divider
	3   - in both the mode-line and the vertical divider

DEFAULT BINDING
    None.

SYNOPSIS
    (setq <integer> (goto-window-at-x-y <X-expression> <Y-expression>))

SEE ALSO
[Chelp-and-exit]
COMMAND NAME
    help-and-exit

DESCRIPTION
    This  function  is  used in the mini-buffer local keymaps to cause
    the  Help  window  to  pop  up  and list the options available. If
    called directly this function is the same as exit-emacs.

DEFAULT BINDING
    ? - in the mini-buffer

SYNOPSIS
    (help-and-exit)

SEE ALSO
    error-and-exit
    expand-and-exit
    exit-emacs
    Minibuf-local-NS-map
    Minibuf-local-map
[Cif]
COMMAND NAME
    if

DESCRIPTION
    An   MLisp  function  that  executes  and  returns  the  value  of
    <then-clause>  if  <test>  is  not  zero;  otherwise  it  executes
    <else-clause> if it is present.

    For example:

              (if (eolp)
                  (to-col 33)
              )

    will  tab  over  to  column 33 if dot is currently at the end of a
    line.

DEFAULT BINDING
    None.

SYNOPSIS
    (if <test> <then-clause> <else-clause>)

SEE ALSO
[Cillegal-operation]
COMMAND NAME
    illegal-operation

DESCRIPTION
    Illegal-operation  is  bound  to  those  keys  that  do not have a
    defined interpretation. Executing illegal-operation is an error.

DEFAULT BINDING
    All key not currently bound

SYNOPSIS
    (illegal-operation)

SEE ALSO
[Cindent-c-procedure]
COMMAND NAME
    indent-C-procedure

DESCRIPTION
    Locate  the  current  C  language  procedure  and  pass  it as the
    standard  input  to  the  INDENT program. The resulting indented C
    procedure will replace the initial C procedure.

    The old text is temporarily stored in "Kill buffer".

    A  C  procedure  is  identified  by  an  identified followed by an
    parentheses followed by { at the top and by } in column one at the
    bottom of the text.

			      CAVEAT

		   To  run  this function you must
		   set  up the INDENT program as a
		   foreign  command.  This program
		   is  distributed  with the EMACS
		   V3.00 kit.

DEFAULT BINDING
    ESC-J

SYNOPSIS
    (indent-C-procedure)

SEE ALSO
   The INDENT program which is used to format a procedure.
[Cinsert-character]
COMMAND NAME
    insert-character

DESCRIPTION
    Insert  its numeric argument into the buffer as a single character
    <prefix-argument>   times.   (insert-character  '0')  inserts  the
    character '0' into the buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (insert-character <expression>)

SEE ALSO
[Cinsert-file]
COMMAND NAME
    insert-file

DESCRIPTION
    Prompt  for  the  name of a file and insert its contents at dot in
    the current buffer.

DEFAULT BINDING
    ^X-^I

SYNOPSIS
    (insert-file "<file-name>")

SEE ALSO
    append-to-file
    read-file
    visit-file
    write-current-file
    write-file-exit
    write-modified-files
    write-named-file
[Cinsert-string]
COMMAND NAME
    insert-string

DESCRIPTION
    An  MLisp  function  that  inserts  the  string  that results from
    evaluating the given <expression>.

DEFAULT BINDING
    None.

SYNOPSIS
    (insert-string "<expression>")

SEE ALSO
[Cinteractive]
COMMAND NAME
    interactive

DESCRIPTION
    An  MLisp  function which returns 1 if the invoking MLisp function
    was called interactively (i.e. bound to a key or by ESC-X).

DEFAULT BINDING
    None.

SYNOPSIS
    (interactive)

SEE ALSO
[Cinterrupt-key]
COMMAND NAME
    interrupt-key

DESCRIPTION
    interrupt-key,  when bound to a single key, and that key is stuck,
    causes  any  current  activity  within   EMACS  to be aborted, and
    forces EMACS to accept commands from the keyboard.

			      CAVEAT

		   Binding   interrupt-key   to  a
		   sequence    longer   than   one
		   character  does  not  allow the
		   key   sequence   to   interrupt
		   EMACS.    Only    single    key
		   sequences   can   be  used  for
		   interrupting.

DEFAULT  BINDING
    ^G

SYNOPSIS
    (interrupt-key)

SEE ALSO
[Cis-bound]
COMMAND NAME
    is-bound

DESCRIPTION
    An  MLisp  function  that  returns  1  if all of its variable name
    arguments are bound to some storage.

DEFAULT BINDING
    None.

SYNOPSIS
    (is-bound <variable-name> ...)

SEE ALSO
[Cis-function]
COMMAND NAME
    is-function

DESCRIPTION
    This  function  checks to see if its string parameter is a defined
    function. It returns 1 if it is defined and 0 otherwise.

    Using this  function  you can determine if a function is defined to
    Emacs before attempting to execute it.

DEFAULT BINDING
    None.

SYNOPSIS
    (is-function <string>)

SEE ALSO
[Cjournal-recover]
COMMAND NAME
    journal-recover

DESCRIPTION
    This  command  is  used to recover a file or buffer after a system
    crash or other disaster.

    All the  journal  files that Emacs write are kept in the directory
    EMACS$JOURNAL: For each buffer that had journaling enabled and had
    not been saved there will be an Emacs  journal file.

    For buffers  with  associated files the journal name will be based
    on the name of file.  For example the file LOGIN.COM will have a
    journal file named LOGIN.COM_EMACS_JOURNAL.

    For buffers  without  associated  files the journal has a slightly
    different  name.   For  example the buffer kill-buffer will have a
    journal called KILL-BUFFER.BUFFER_EMACS_JOURNAL.

    journal-recover prompts  for  the  name of the journal file, which
    must be in EMACS$JOURNAL:.  Emacs will check that the journal file
    is  valid  before attempting to apply it.  For a file Emacs checks
    that the versions number of the file is the same as the journal was
    recorded  for.  For a buffer Emacs checks that the buffer does not
    exist.   These  check  ensure  that using journal-recover will not
    damage your data.

    If journal-recover  tells you that there is a newer version of the
    file  and  you  really wish to apply the journal do the following.
    Rename  or  delete all higher version numbered files until you get
    to the version that journal-recover requires.  BUT TAKE GREAT CARE
    when doing this.

DEFAULT BINDING
    None.

SYNOPSIS
    (journal-recover <journal-file-name>)

SEE ALSO
    variable animated-journal-recovery
[Cjustify-paragraph]
COMMAND NAME
    justify-paragraph (package)

DESCRIPTION
    Take  the  current  paragraph, bounded by blank lines, and make it
    conform  to  the variables left-margin and right-margin. The right
    margin will be ragged.

DEFAULT BINDING
    None.

SYNOPSIS
    (justify-paragraph)

SEE ALSO
    text-mode for a better text indenter.
[Ckill-buffer]
COMMAND NAME
    kill-buffer

DESCRIPTION
    kill-buffer  is  used to remove a buffer from the list of buffers,
    and  to throw away all the text it contains. It takes one argument
    which is the name of the buffer to delete.

    If  you  use  the kill-buffer  command from  the terminal, and the
    target  buffer  is  modified,  kill-buffer   will  ask you  if you
    really meant to delete the specified buffer.

    If  you delete the current buffer, then dot is moved to one of the
    other  buffers  displayed  on  the  screen.  Of there are no other
    buffers, then "main" is used (and created if necessary).


			      CAVEAT

		   This   command   has  the  same
		   functionality as delete-buffer.
		   So,  from  the  next release of
		   EMACS,   kill-buffer   will  be
		   removed.


DEFAULT BINDING
    None.

SYNOPSIS
    (kill-buffer "<buffer-name>")

SEE ALSO
    kill-buffer
[Ckill-process]
COMMAND NAME
    kill-process

DESCRIPTION
    Kill  the  specified  sub-process.  The  sub-process  is killed by
    executing  a  delete process request. Therefore, exit handlers for
    the target process will not be executed.

DEFAULT BINDING
    None.

SYNOPSIS
    (kill-process "<process-name>")

SEE ALSO
    force-exit-process
    kill-process
    list-processes
    pause-process
    resume-process
    set-current-process
    start-DCL-process
    wait-for-process-input-request
[Ckill-to-end-of-line]
COMMAND NAME
    kill-to-end-of-line

DESCRIPTION
    Delete characters  forward  from  dot to the immediately following
    end-of-line (or  end of buffer if there is not an end of line). If
    dot  is  positioned  at  the  end  of  a line then the end-of-line
    character is deleted.

    Text  deleted by the ^K command is placed into "Kill buffer". A ^K
    command  normally  erases  the  contents of the kill buffer first;
    subsequent ^K's in an unbroken sequence append to the kill buffer.
    The number of lines killed is controlled by <prefix-argument>.

DEFAULT BINDING
    ^K

SYNOPSIS
    (kill-to-end-of-line)

SEE ALSO
[Clast-key-struck]
COMMAND NAME
    last-key-struck

DESCRIPTION
    An  MLisp function that returns the last command character struck.
    If  you  have  a  function bound to many keys the function may use
    last-key-struck   to  tell  which  key  was  used  to  invoke  it.
    (insert-character (last-key-struck)) does the obvious thing.

DEFAULT BINDING
    None.

SYNOPSIS
    (last-key-struck)

SEE ALSO
    previous-command
[Cleft-marker]
COMMAND NAME
    left-marker

DESCRIPTION
    This  function  takes  a  marker  as a parameter and returns a new
    marker.   The  new marker will at the same position as the original
    but with left hand affinity.  This means that text inserted at the
    position  of the marker will not move the markers position.  Right
    hand  affinity  markers on the other hand move to the right as text
    is inserted.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq left-hand-end (left-marker <marker>)

SEE ALSO
    right-marker
[Cleft-window]
COMMAND NAME
    left-window

DESCRIPTION
    This commands  move the cursor into the window that is to the left
    of  the  current window.  Unlike next-window that moves the cursor
    without  regard  to  the  physical  position of the windows on the
    screen.

    left-window reports  an error of there is not a window to the left
    of the current window.

DEFAULT BINDING
    None.

SYNOPSIS
    (left-window)

SEE ALSO
    up-window
    down-window
    right-window
[Clength]
COMMAND NAME
    length

DESCRIPTION
    An MLisp function that returns the length of its string parameter.

DEFAULT BINDING
    None.

SYNOPSIS
    (length "<string-expression>")
    (length "time") => 4

SEE ALSO
[Cline-to-top-of-window]
COMMAND NAME
    line-to-top-of-window

DESCRIPTION
    Move  the  line  in  which  dot  resides  to the top of the window
    associated with the current buffer.

DEFAULT BINDING
    ESC-!

SYNOPSIS
    (line-to-top-of-window)

SEE ALSO
[Clisp-mode]
COMMAND NAME
    lisp-mode (package)

DESCRIPTION

    A major mode for editing programs written in the Lisp language and
    also  MLisp  programs for EMACS. It provides a number of extra key
    bindings:

    )	causes  the  corresponding  open  bracket  to  be indicated by
	moving  the  cursor over it for a second. The cursor will move
	back to dot if you start typing, or if the second is up.

    `	is  bound  to  expand-mlisp-word  so that you can insert EMACS
	extended command names easily.

    LF	is  bound  so  that it inserts a newline, and enough spaces to
	get  to the correct indent level according to the depth of the
	current S-expression.

    ESC-(
	goes to the beginning of the next S-expression.
	
    ESC-)
	goes to the beginning of the previous S-expression.
	
    ESC-c
	is bound to execute-mlisp-buffer.
	
    ESC-i
  	indents the current lisp line.
	
    ESC-j
  	indents the current lisp defun.

DEFAULT BINDING
    None.

SYNOPSIS
    (lisp-mode)

SEE ALSO
    The EMACS manual for the detailed description of lisp-mode.
[Clist-abbreviation-tables]
COMMAND NAME
    list-abbreviation-tables

DESCRIPTION
    Creates  a  buffer  called "Abbreviation table list" and inserts a
    list of the abbreviation tables that have been define to Emacs.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-abbreviation-tables)

SEE ALSO
    dump-abbreviation-tables
[Clist-auto-executes]
COMMAND NAME
    list-auto-executes

DESCRIPTION
    Creates  a buffer called "Auto Execute list" and inserts a list of
    all the auto-execute patterns and there functions.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-auto-executes)

SEE ALSO
[Clist-breakpoints]
COMMAND NAME
    list-breakpoints

DESCRIPTION
    Creates  a  buffer  called "Breakpoint list" and inserts a list of
    the functions that have breakpoints set for them.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-breakpoints)

SEE ALSO
    breakpoint
    variable breakpoint-hook
[Clist-buffers]
COMMAND NAME
    list-buffers

DESCRIPTION
    Produce  a listing of all existing buffers giving their names, the
    name  of  the  associated  file  (if  there is one), the number of
    characters  in  the buffer and an indication of whether or not the
    buffer  has  been  modified  since it was read or written from the
    associated file.

DEFAULT BINDING
    ^X-^B

SYNOPSIS
    (list-buffers)

SEE ALSO
[Clist-databases]
COMMAND NAME
    list-databases

DESCRIPTION
    List  all  data  base  search  lists and the names of the database
    files contained in them.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-databases)

SEE ALSO
[Clist-images]
COMMAND NAME
    list-images

DESCRIPTION
    List  the  names  of  all images referenced by external-function's.
    The  listing  shows the image name, the value of the context value
    and the filename of that holds the image.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-images)

SEE ALSO
    external-function
[Clist-processes]
COMMAND NAME
    list-processes

DESCRIPTION
    List  the  name  of  all  existing sub-processes. Also include the
    state  in  which  each  process is running, the time at which each
    process  entered the state and some information about the names of
    buffers and procedures associated with the process.

    Processes  which  have  died  only appear once in this list before
    completely disappearing.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-processes)

SEE ALSO
    current-process-name
    force-exit-process
    kill-process
    pause-process
    resume-process
    start-DCL-process
    wait-for-process-input-request
[Clist-syntax-tables]
COMMAND NAME
    list-syntax-tables

DESCRIPTION
    List all the syntax tables that have been defined.

DEFAULT BINDING
    None.

SYNOPSIS
    (list-syntax-tables)

SEE ALSO
[Cload]
COMMAND NAME
    load

DESCRIPTION
    Load  the  nominated  file  as  a  series of Mock Lisp expressions
    executing each in turn.

    This function has identical functionality to execute-mlisp-file.

			      CAVEAT

		   This  function  will be removed
		   after  this release of EMACS. A
		   function  with  the  same name,
		   but        with       different
		   functionality may replace it.

DEFAULT BINDING
    None.

SYNOPSIS
    (load "<file-name>")

SEE ALSO
    execute-mlisp-file
[Clocal-bind-to-key]
COMMAND NAME
    local-bind-to-key

DESCRIPTION
    Prompt  for the name of a command and a key sequence and bind that
    command to the given key sequence. Unlike bind-to-key, the binding
    only  has effect in the current buffer. This is generally used for
    mode  specific  bindings that will generally differ from buffer to
    buffer.

    See bind-to-key for details of symbolic key names.

DEFAULT BINDING
    None.

SYNOPSIS
    (local-bind-to-key "<extended-command>" "<key-sequence>")

SEE ALSO
    bind-to-key
[Clocal-binding-of]
COMMAND NAME
    local-binding-of

DESCRIPTION
    This  MLisp  function  returns  the  name of the function that the
    keystroke  provided  will  invoke,  using the current local keymap
    keymap.

DEFAULT BINDING
    None.

SYNOPSIS
    (local-binding-of "<key-sequence>")

SEE ALSO
    global-binding-of
    describe-binding
[Clooking-at]
COMMAND NAME
    looking-at

DESCRIPTION
    An  MLisp function which returns 1 if the given regular expression
    search string  matches the text immediately following dot. This is
    for use in packages that want to do a limited sort of parsing.

    For example, if dot is at the beginning of a line then

 	 (looking-at "[ \t]*else")

    will be true if the line starts with an "else".

    If a negative prefix argument is supplied to looking-at, it checks
    the character to the left of dot, rather than those to the right.

DEFAULT BINDING
    None.

SYNOPSIS
    (looking-at "<regular-expression>")

SEE ALSO
    re-search-forward
    re-search-reverse
    search-forward
    search-reverse
[Cmark]
COMMAND NAME
    mark

DESCRIPTION
    An  MLisp  function that returns the position of the marker in the
    current buffer. An error is displayed if the marker is not set.

DEFAULT BINDING
    None.

SYNOPSIS
    (mark)

SEE ALSO
    set-mark
    region-around-match
    exchange-dot-and-mark
[Cmessage]
COMMAND NAME
    message

DESCRIPTION
    An  MLisp functions that  concatenates  its  string  arguments and
    displayed then in the mini-buffer at the bottom of the screen next
    time the screen is updated.

    The screen is only updated when an input request needs to pause to
    obtain  characters,  or  the  sit-for function is executed. If you
    wish to display a message then use

  	(sit-for 0)

    to cause the screen to be updated.

DEFAULT BINDING
    None.

SYNOPSIS
    (message "<string-1>" "<string-2>" ...)

SEE ALSO
    sit-for
[Cmeta-digit]
COMMAND NAME
    meta-digit

DESCRIPTION
    This  function  provides the facility for a quick prefix argument.
    It  is  bound to all the numeric keys but prefixed with ESC. So to
    get a prefix of 5, just type ESC-5 followed by the command.

DEFAULT BINDING
    ESC-0 to ESC-9

SYNOPSIS
    None.

SEE ALSO
    argument-prefix
    digit
    meta-minus
    minus
[Cmeta-minus]
COMMAND NAME
    meta-minus

DESCRIPTION
    Convert a meta collected digit to a negative prefix argument.

DEFAULT BINDING
    ESC--

SYNOPSIS
    None.

SEE ALSO
    argument-prefix
    digit
    meta-digit
    minus
[Cminus]
COMMAND NAME
    minus

DESCRIPTION
    This  command  is bound to the minus key after the argument-prefix
    command  has  been  executed.  It  is  used  to  negate the prefix
    argument.

DEFAULT BINDING
    -

SYNOPSIS
    None.

SEE ALSO
    argument-prefix
    digit
[Cmodify-syntax-entry]
COMMAND NAME
    modify-syntax-entry

DESCRIPTION

    USE modify-syntax-table   FOR  ALL  NEW  CODE.   This  command  is
    retained for compatibility with V5 Emacs MLisp.

    modify-syntax-entry  is  used  to  modify  a set of entries in the
    syntax table associated with the current buffer.

    Syntax   tables   are   associated   with  buffers  by  using  the
    use-syntax-table  command. Syntax tables are used by commands like
    forward-paren  to  do  a  limited  form  of  parsing  for language
    dependent  routines.  They  define such things as which characters
    are parts of words, which quote strings and which delimit comments
    (currently, nothing uses the comment specification).

    The  parameter to modify-syntax-entry is a string whose first five
    characters  specify  the interpretation of the sixth and following
    characters.

    The  first  character  specifies  the  type.  It may be one of the
    following:


    'w'	A word character, as used by such commands as forward-word and
	case-word-capitalize.

    space
	A character with no special interpretation.

    '('	A  left  parenthesis. Typical candidates for this type are the
	characters '(', '[' and '{'. Characters of this type also have
	a  matching  right parenthesis specified (')', ']' and '}' for
	example)   which  appears  as  the  second  character  of  the
	parameter to modify-syntax-entry.

    ')'	A  right parenthesis. Typical candidates for this type are the
	characters ')', ']' and '}'. Characters of this type also have
	a  matching  left  parenthesis specified ('(', '[' and '{' for
	example)   which  appears  as  the  second  character  of  the
	parameter to modify-syntax-entry.

    '"'	A quote character. The C string delimiters " and ' are usually
	given this class, as is the Lisp |.

    '\'	A prefix character, like \ in C or / in MLisp.

    The  second character of the parameter is the matching parenthesis
    if  the character is of the left or right parenthesis type. If you
    specify  that  '(' is a right parenthesis matched by ')', then you
    should also specify that ')' is a left parenthesis matched by '('.

    The  third  character,  if  equal  to '{', says that the character
    described  by  this  syntax  entry  can begin a comment; the forth
    character,  if  equal  to '}' says that the character described by
    this  syntax  entry  can end a comment. If either the beginning or
    ending  comment  sequence  is  two characters long, then the fifth
    character provides the second character of the comment sequence.

    The  sixth  and  following characters specify which characters are
    described by this entry; a range of characters can be specified by
    putting  a  '-' between them, a '-' can be described if it appears
    as the sixth character.

    A few examples are:

       (modify-syntax-entry "w    -")  	; makes '-' behave as a word
				        ; character (ESC-F will con-
                                        ; sider - as part of a word)
       (modify-syntax-entry "(]   [")	; makes  '['  behave  as  an
					; open   bracket   which  is
					; matched by ']'
       (modify-syntax-entry ")[   ]")   ; makes  ']'   behave  as  a
					; close  bracket   which  is
				        ; matched by '['

DEFAULT BINDING
    None.

SYNOPSIS
    (modify-syntax-entry "<syntax-string>")

SEE ALSO
    dump-syntax-table
    use-syntax-table
    modify-syntax-table
[Cmodify-syntax-table]
COMMAND NAME
    modify-syntax-table

DESCRIPTION
    modify-syntax-table  is  used  to  modify  a set of entries in the
    syntax table associated with the current buffer.

    Syntax tables   are   associated   with   buffers   by  using  the
    use-syntax-table command.  Syntax tables are used by commands like
    forward-paren  to  do  a  limited  form  of  parsing  for language
    dependent  routines.   They define such things as which characters
    are  parts  of  words,  which  quote  strings  and  which  delimit
    comments.

    syntax colouring  and  regular  expression  searchs  depend on the
    information in the syntax table.

    modify-syntax-table can be called in one of the following ways:

		(modify-syntax-table "word" <char-set>)

    Defines the  characters  in <char-set> as word characters, as used
    by such commands as forward-word and case-word-capitalize.

		(modify-syntax-table "dull" <char-set>)

    Defines the characters in <char-set> as having no special meaning.

	 (modify-syntax-table "comment" <comment-start-string>
			 <comment-end-string>)

    Defines a comment that starts with <comment-start-string> and ends
    with <comment-end-string>.

	(modify-syntax-table "paren" <open-paren> <close-paren>)

    Defines a matching pair of parentheses.

	       (modify-syntax-table "prefix" <char-set>)

    Defines the  characters  in  <char-set>  as  being  a  prefix that
    prevents  the  following  character  being interpreted as a string
    introducer.

	       (modify-syntax-table "string" <char-set>)

    Defines the characters in <char-set> as delimiting a string.

	       (modify-syntax-table "keyword-1" <string>)

    Define <string> as a keyword type 1.

	       (modify-syntax-table "keyword-2" <string>)

    Define <string> as a keyword type 2.

	       (modify-syntax-table "keyword-3" <string>)

    Define <string> as a keyword type 3.

    The <char-set>  is  a  string  that contains a list of characters.
    Ranges of characters can be conviently entered by giving the first
    and last characters of the range seperated by a "-".

    A few examples are:

	; makes A to Z, a to z, 0 to 9 and '-' behave as a word
	; characters (ESC-F will consider - as part of a word)
	(modify-syntax-table "word" "A-Za-z0-9-")
	; makes  '['  behave  as  an open   bracket   which  is
	; matched by ']'
	(modify-syntax-table "paren" "[" "]")
	; define the C++ one line comment 
	(modify-syntax-table "comment" "//" "\n")

DEFAULT BINDING
    None.

SYNOPSIS
    (modify-syntax-table "<syntax-string>")

SEE ALSO
    dump-syntax-table
    modify-syntax-entry
    use-syntax-table
[Cmove-to-comment-column]
COMMAND NAME
    move-to-comment-column

DESCRIPTION
    If  the  cursor is not at the beginning of a line, ESC-C moves the
    cursor  to  the column specified by the comment-column variable by
    inserting tabs and spaces as needed. In any case, it then sets the
    right  margin  to the column finally reached. This is usually used
    in MLisp for language-specific comments.

DEFAULT BINDING
    None.

SYNOPSIS
    (move-to-comment-column)

SEE ALSO
    variable comment-column
    variable default-comment-column
[Cnargs]
COMMAND NAME
    nargs

DESCRIPTION
    An  MLisp function which returns the number of arguments passed to
    the invoking MLisp function.

    For  example, within the execution of foo invoked by (foo x y) the
    value of nargs will be 2.

DEFAULT BINDING
    None.

SYNOPSIS
    (nargs)

SEE ALSO
    arg
[Cnarrow-window]
COMMAND NAME
    narrow-window

DESCRIPTION
    Reduce  the  width of a vertically split window.  A window has to
    be at .least one column wide.

DEFAULT BINDING
    None.

SYNOPSIS
    (narrow-window)

SEE ALSO
    widen-window
    variable window-width
[Cnarrow-region]
COMMAND NAME
    narrow-region

DESCRIPTION
    Restrict  the  amount  of  a buffer that can be manipulated to the
    region   between  dot  and  mark.  widen-region  will  remove  the
    restriction.

    File operations ignore this restriction.

    This  function  is useful for replacing strings over a small parts
    of  a buffer or for running packages that usually affect the whole
    of a buffer but a required to operate on a portion of it.

SYNOPSIS
    (narrow-region)

SEE ALSO
    widen-region
    save-restriction
[Cnewline]
COMMAND NAME
    newline

DESCRIPTION
    Insert a newline character into the buffer.

DEFAULT BINDING
    <CR> or ^M

SYNOPSIS
    (newline)

SEE ALSO
    newline-and-backup
    newline-and-indent
[Cnewline-and-backup]
COMMAND NAME
    newline-and-backup

DESCRIPTION
    Insert  an  end-of-line immediately after dot, effectively opening
    up the line. If dot is positioned at the beginning of a line, then
    ^O  will  create  a  blank  line  preceding  the  current line and
    position dot on that new line.

DEFAULT BINDING
    ^O

SYNOPSIS
    (newline-and-backup)

SEE ALSO
    newline
    newline-and-indent
[Cnewline-and-indent]
COMMAND NAME
    newline-and-indent

DESCRIPTION
    Insert  a  newline,  just  as  typing RETURN does, but then insert
    enough tabs and spaces so that the newly created line has the same
    indentation as the old one had.

    This  is  quite  useful  when you are typing in a block of program
    text, all at the same indentation level.

DEFAULT BINDING
    <LF> or ^J

SYNOPSIS
    (newline-and-indent)

SEE ALSO
    newline
    newline-and-backup
[Cnext-error]
COMMAND NAME
    next-error

DESCRIPTION
    Take  the  next  error  message  (as  returned from the compile-it
    command),  visit  the file in which the error occurred and set dot
    to  the  line on which that error occurred. The error message will
    be  displayed  at  the top of the window associated with the Error
    log buffer.

DEFAULT BINDING
    ^X-^N

SYNOPSIS
    (next-error)

SEE ALSO
    compile-it
    previous-error
[Cnext-line]
COMMAND NAME
    next-line

DESCRIPTION
    Move  dot  down <prefix-argument> lines. next-line attempt to keep
    dot  at  the  same  horizontal character position as you move from
    line to line.

DEFAULT BINDING
    ^N

SYNOPSIS
    (next-line)

SEE ALSO
    previous-line
    variable track-eol-on-^N-^P
[Cnext-page]
COMMAND NAME
    next-page

DESCRIPTION
    Reposition  the  current  window on the current buffer so that the
    page  <prefix-argument>  pages  forward  is  visible in the window
    (where  a  page is a group of lines 4/5th the size of the window).
    If possible, dot is kept where it is, otherwise it is moved to the
    middle of the new page.

DEFAULT BINDING
    ^V

SYNOPSIS
    (next-page)

SEE ALSO
    previous-page
    page-next-window
[Cnext-window]
COMMAND NAME
    next-window

DESCRIPTION
    Switch  to  the  window  (and associated buffer) that is below the
    current  window. The window below the last window is the window at
    the top of the screen.

DEFAULT BINDING
    ^X-N

SYNOPSIS
    (next-window)

SEE ALSO
    beginning-of-window
    end-of-window
    line-to-top-of-window
    previous-window
[Cnormal-mode]
COMMAND NAME
    normal-mode (package)

DESCRIPTION
    normal-mode is the default major mode which EMACS gives to all new
    buffers  that  it creates. It provides a general text manipulating
    environment, with no special features.

DEFAULT BINDING
    None.

SYNOPSIS
    (normal-mode)

SEE ALSO
    fundamental-mode
    text-mode
    c-mode
    lisp-mode
[Cnovalue]
COMMAND NAME
    novalue

DESCRIPTION
    Does nothing. (novalue) is a complete no-op, it performs no action
    and  returns no value. Generally the value of an MLisp function is
    the  value of the last expression evaluated in it's body, but this
    value may not be desired, so (novalue) is provided so that you can
    throw it away.

DEFAULT BINDING
    None.

SYNOPSIS
    (novalue)

SEE ALSO
[Cpage-next-window]
COMMAND NAME
    page-next-window

DESCRIPTION
    Reposition the window below the current one (or the top one if the
    current  window  is the lowest one on the screen) on the displayed
    buffer  so  that  the  next  page  of the buffer is visible in the
    window  (where  a  page  is a group of lines 4/5th the size of the
    window).

    If  page-next-window  is  given a prefix argument it will flip the
    buffer backward a page, rather than forwards. So ESC-^V is roughly
    equivalent  to  ^V  and  ^U-ESC-^V  is roughly equivalent to ESC-V
    except that they deal with the other window.

DEFAULT BINDING
    ESC-^V

SYNOPSIS
    (page-next-window)

SEE ALSO
    beginning-of-window
    end-of-window
    line-to-top-of-window
    next-window
    page-next-window
[Cparse-error-messages-in-region]
COMMAND NAME
    parse-error-messages-in-region

DESCRIPTION
    Parse  the  region between dot and mark for error messages (as the
    compile-it/next-error   commands)   and   set  up  for  subsequent
    invocations of next-error.

DEFAULT BINDING
    None.

SYNOPSIS
    (parse-error-messages-in-region)

SEE ALSO
    compile-it
    next-error
[Cpause-emacs]
COMMAND NAME
    pause-emacs (VMS)

DESCRIPTION
    Attach  control  back  to  the  process  that  started the process
    running   a  sub-process  version  of  EMACS,  using  the  VAX/VMS
    SPAWN/ATTACH  commands. EMACS goes back to sleep, until control is
    passed back to it by running EMACS again.

    If this  function is called with a prefix argument, it will prompt
    for  a  command to execute in the parent process.  This feature is
    implemented  in  the EMACSSHR image and will not work if EMACS was
    simply SPAWNED or ATTACHED using the DCL commands.

    After  this  function  is  called the leave-emacs-hook variable is
    examined.  If it specifies a function, then the function is called
    just before attaching back to the parent process.

    On returning to EMACS, the return-to-emacs-hook is examined. IF it
    specifies  a  function, then the function is called before reading
    more commands from the keyboard.

    This  command  is  different  to  return-to-monitor  because  that
    control passes back to the parent process. With return-to-monitor,
    control passes down a sub-process level

DEFAULT BINDING
    None.

SYNOPSIS
    (pause-emacs)
    (pause-emacs "<DCL-command>")

SEE ALSO
    return-to-monitor
    Variable leave-emacs-hook
    Variable return-to-emacs-hook
[Cpause-process]
COMMAND NAME
    pause-process (VMS)

DESCRIPTION
    Set  the  specified sub-process into VAX/VMS state suspended. This
    stops  the  process from executing. Use resume-process to start it
    going again.

DEFAULT BINDING
    None.

SYNOPSIS
    (pause-process "<process-name>")

SEE ALSO
    force-exit-process
    kill-process
    list-processes
    resume-process
    start-DCL-process
[Cpop-to-buffer]
COMMAND NAME
    pop-to-buffer

DESCRIPTION
    Switch to a buffer whose name is provided and tie that buffer to a
    newly  created  window.  Pop-to-buffer  is  exactly  the  same  as
    switch-to-buffer  except  that switch-to-buffer ties the buffer to
    the current window, pop-to-buffer finds a new window to tie it to.

DEFAULT BINDING
    None.

SYNOPSIS
    (pop-to-buffer "<buffer-name>")

SEE ALSO
    switch-to-buffer
[Cpreceding-char]
COMMAND NAME
    preceding-char

DESCRIPTION
    An MLisp function that returns the character immediately preceding
    dot as an integer. The null character (0) is returned if dot is at
    the  beginning  of  the buffer. Remember that dot is not `at' some
    character, it is between two characters.

DEFAULT BINDING
    None.

SYNOPSIS
    (preceding-char)

SEE ALSO
    following-character
[Cprefix-argument-loop]
COMMAND NAME
    prefix-argument-loop

DESCRIPTION
    An  MLisp  function  that  executes  <statements>  prefix-argument
    times.  Every  function  invocation  is  always  prefixed  by some
    argument,  usually  by  the user typing ^Us. If no prefix argument
    has been provided, 1 is assumed.

DEFAULT BINDING
    None.

SYNOPSIS
    (prefix-argument-loop <statements>)

SEE ALSO
    provide-prefix-argument
    argument-prefix
    variable prefix-argument
[Cprepend-region-to-buffer]
COMMAND NAME
    prepend-region-to-buffer

DESCRIPTION
    Prepend  the  region between dot and mark to the buffer specified.
    Neither  the  original text in the destination buffer nor the text
    in the region between dot and mark are disturbed.

DEFAULT BINDING
    None.

SYNOPSIS
    (prepend-region-to-buffer <buffer-name>)

SEE ALSO
    append-region-to-buffer
[Cprevious-command]
COMMAND NAME
    previous-command

DESCRIPTION
    An  MLisp function that returns the last command character struck.
    If  you  have  a  function bound to many keys the function may use
    previous-command to tell which key was used to invoke it.

DEFAULT BINDING
    None.

SYNOPSIS
    (previous-command)

SEE ALSO
    last-key-struck
[Cprevious-line]
COMMAND NAME
    previous-line

DESCRIPTION
    Move  dot  to the <prefix-argument>th previous line. previous-line
    attempt  to  keep  dot at the same horizontal position as you move
    from line to line.

DEFAULT BINDING
    ^P

SYNOPSIS
    (previous-line)

SEE ALSO
    next-line
    variable track-eol-on-^N-^P
[Cprevious-error]
COMMAND NAME
    previous-error

DESCRIPTION

    Take the  previous  error message (as returned from the compile-it
    command),  visit  the file in which the error occurred and set dot
    to  the line on which that error occurred.  The error message will
    be  displayed  at  the top of the window associated with the Error
    log buffer.

DEFAULT BINDING
    none

SYNOPSIS
    (previous-error)

SEE ALSO
    compile-it
    next-error
[Cprevious-page]
COMMAND NAME
    previous-page

DESCRIPTION
    Reposition  the  current  window on the current buffer so that the
    <prefix-argument>th  previous page of the buffer is visible in the
    window  (where  a  page  is a group of lines 4/5th the size of the
    window).  If  possible,  dot  is kept where it is, otherwise it is
    moved to the middle of the new page.

DEFAULT BINDING
    ESC-V

SYNOPSIS
    (previous-page)

SEE ALSO
    next-page
    page-next-window
[Cprevious-window]
COMMAND NAME
    previous-window

DESCRIPTION
    Switch  to  the  window  (and associated buffer) that is above the
    current  window.  If  there is no window above the current window,
    then the bottom window is used.

DEFAULT BINDING
    ^X-P

SYNOPSIS
    (previous-window)

SEE ALSO
    delete-other-windows
    delete-window
    enlarge-window
    next-window
    shrink-window
    split-current-window
    split-current-window-vertically
[Cprint]
COMMAND NAME
    print

DESCRIPTION
    Print the value of the named variable. This is the command you use
    when  you  want  to  inquire  about  the setting of some switch or
    option.

DEFAULT BINDING
    None.

SYNOPSIS
    (print "<variable-name>")

SEE ALSO
    set
    print-default
[Cprint-default]
COMMAND NAME
    print-default

DESCRIPTION
    Print the default value of the named variable. This is the command
    you  use when you want to inquire about the setting of some switch
    or option.

DEFAULT BINDING
    None.

SYNOPSIS
    (print-default "<variable-name>")

SEE ALSO
    set-default
    print
[Cprocess-id]
COMMAND NAME
    process-id (Unix)

DESCRIPTION
    Returns the process id of the names process.

DEFAULT BINDING
    none.

SYNOPSIS
    (process-id <process-name>)

EXAMPLE

SEE ALSO

[Cprocess-key]
COMMAND NAME
    process-key

DESCRIPTION
    This commands  takes  one  argument  which must result in a keymap
    function  being executed. Then that keymap is used to read one key
    stroke  sequence.  The  command returns the result of the function
    executed  from  the  keymap.  If  the keys struck do not match the
    keymap process-key reports an error.

EXAMPLE

	(if (error-occurred (setq result (process-key (key-map)))
		(message "keys type not in map")
		(message "result is " result)
	)

DEFAULT BINDING
    None.

SYNOPSIS
    (process-key <S-expression>)

SEE ALSO
    process-key-hook
    last-keys-struck
[Cprocess-output]
COMMAND NAME
    process-output

DESCRIPTION
    An  MLisp  function  that returns the next line of output from the
    specified sub-process. This function is typically used in an MLisp
    procedure   specified   using   the   set-process-output-procedure
    command.

    If  no  output  is  available  from  the  sub-process, an error is
    displayed.

DEFAULT BINDING
    None.

SYNOPSIS
    (process-output "<process-name>")

SEE ALSO
    current-process-name
    send-string-to-process
    set-current-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
    wait-for-process-input-request
[Cprocess-status]
COMMAND NAME
    process-status

DESCRIPTION
    Returns the status of the named process.

    Return Value	Description
    ------------	-----------
	-1		Process does not exist
	 0		Process is not running
	 1		Process is running

DEFAULT BINDING
    none.

SYNOPSIS
    (process-status <process-name>)

EXAMPLE

SEE ALSO

[Cprogn]
COMMAND NAME
    progn

DESCRIPTION
    An  MLisp  function that evaluates the expressions and returns the
    value   of   the  last  expression  evaluated.  Progn  is  roughly
    equivalent  to  a  compound  statement  (begin-end  block) in more
    conventional  languages  and  is  used  where  you want to execute
    several expressions when there is space for only one (e.g. the then
    or else parts of an if expression).

DEFAULT BINDING
    None.

SYNOPSIS
    (progn [<local-declarations>]...<expression> ...)

SEE ALSO
    save-excursion
    save-window-excursion
    save-restriction
[Cprovide-prefix-argument]
COMMAND NAME
    provide-prefix-argument

DESCRIPTION
    Provide the prefix argument <value> to the <statement>.

    For  example, the most efficient way to skip forward 5 words in an
    MLisp program is:

      (provide-prefix-argument 5 (forward-word))

DEFAULT BINDING
    None.

SYNOPSIS
    (provide-prefix-argument <expression-1> <statement>)

SEE ALSO
    prefix-argument-loop
    variable prefix-argument
[Cpush-back-character]
COMMAND NAME
    push-back-character

DESCRIPTION

    Takes the character or string provided as its argument and cause it
    to  be  used as the next character(s) read from the keyboard. It is
    generally only useful in MLisp functions which read characters from
    the  keyboard,  and  upon  finding one that they do not understand,
    terminate and behave as though the key had been struck to the EMACS
    keyboard command interpreter.

    About 100 characters can be pushed back.


DEFAULT BINDING
    None.

SYNOPSIS
    (push-back-character <expression>)
    (push-back-character <string>)

SEE ALSO
[Cput-database-entry]
COMMAND NAME
    put-database-entry

DESCRIPTION
    Take the current buffer and store it into the named database under
    the  given  key.  The  key  and  data  are  written into the first
    database  file in the specified database search list. If this file
    happens to be read-only, the put will fail with an error.

DEFAULT BINDING
    None.

SYNOPSIS
    (put-database-entry "<dbname>" "<key>")

SEE ALSO
    extend-database-search-list
    fetch-database-entry
    fetch-help-database-entry
    list-databases
[Cputenv]
COMMAND NAME
    putenv

DESCRIPTION
    Under  Unix this command adds the environment variable <name>
    to  Emacs's  environment.  The environment variables value is
    set  to  <value>.   Any process created by emacs will inherit
    the variable.

    Under Windows   this  command  adds  <name>  to  the  Windows
    registry  "Environment"  key  for Barry's Emacs.  The new key
    will have the value <value>.

DEFAULT BINDING
    none.

SYNOPSIS
    (putenv <name> <value>)

EXAMPLE

SEE ALSO
    getenv
[Cquery-replace-string]
COMMAND NAME
    query-replace-string

DESCRIPTION
    Replace  all  occurrences  of one string with another, starting at
    dot  and ending at the end of the buffer. EMACS prompts for an old
    and  a  new  string in the mini-buffer. For each occurrence of the
    old  string,  EMACS  requests that the user type in a character to
    tell  it  what  to do (dot will be positioned just after the found
    string). The possible replies are:

    <space>
	Change this occurrence and continue to the next.

    ,	Change  this occurrence  and  display the change.Then wait for
	another character to be typed. Re-typing `,' or <space> has no
	effect.

    n	Do not change this occurrence, but continue to the next.

    !	Change  this  occurrence  and  all the rest of the occurrences
	without bothering to ask.

    .	Change this one and stop: do not do any more replaces.

    ^G	Do not  change  this  occurrence  and  stop:  do not do any more
	replaces.

    r	Enter recursive edit with dot set at the displayed position.

    ?	(or  anything  else)  Print  a short list of the query/replace
	options.

DEFAULT BINDING
    ESC-Q

SYNOPSIS
    (query-replace-string "<source-string>" "<replace-string>")

SEE ALSO
    re-query-replace-string
    replace-string
    re-replace-strings
[Cquietly-read-abbrev-file]
COMMAND NAME
    quietly-read-abbrev-file

DESCRIPTION
    Read  in and define abbreviations  appearing in a named file. This
    file  should  have  been  written  using write-abbrev-file. Unlike
    read-abbrev-file,  an  error  message  is  not printed if the file
    cannot be found.

DEFAULT BINDING
    None.

SYNOPSIS
    (quietly-read-abbrev-file "<file-name>")

SEE ALSO
    define-local-abbrev
    read-abbrev-file
    use-abbrev-table
    write-abbrev-file
[Cquit-process]
COMMAND NAME
    quit-process (Unix)

DESCRIPTION
    Send the signal SIGQUIT to the named process.

DEFAULT BINDING
    none.

SYNOPSIS
    (quit-process <process-name>)

EXAMPLE

SEE ALSO

[Cquote]
COMMAND NAME
    quote

DESCRIPTION
    An  MLisp  function  that  returns its string argument so that any
    meta  characters  used  by regular expression string searching are
    quoted  so  that  they  have  no  effect. This is useful for MLisp
    packages that use regular expression search commands.

DEFAULT BINDING
    None.

SYNOPSIS
    (quote "<expression>")

SEE ALSO
    re-search-forward
    re-search-reverse
    re-query-replace-string
    re-replace-string
[Cquote-character]
COMMAND NAME
    quote-character

DESCRIPTION
    Insert   into   the   buffer  the  next  character  typed  without
    interpreting  it  as  a  command.  This  is  how  you insert funny
    characters.

    For  example,  to  insert a ^L (form feed or page break character)
    type ^^-Q-^L.

DEFAULT BINDING
    ^^-Q or ^Q

SYNOPSIS
    (quote-character)

SEE ALSO
[Cre-query-replace-string]
COMMAND NAME
    re-query-replace-string

DESCRIPTION
    Replace  all  occurrences  of a pattern with a string, starting at
    dot  and ending at the end of the buffer. EMACS prompts for an old
    and  a  new  string in the mini-buffer. For each occurrence of the
    old  string,  EMACS  requests that the user type in a character to
    tell  it  what  to do (dot will be positioned just after the found
    string).  The  possible  replies  are  described  in  the help for
    query-replace-string.

DEFAULT BINDING
    None.

SYNOPSIS
    (re-query-replace-string "<patter>" "<replacement>")

SEE ALSO
    query-replace-string
    re-replace-string
    replace-string
[Cre-replace-search-text]
COMMAND NAME
    re-replace-search-text

DESCRIPTION

    Do one regular expression replacement of the last pattern searched
    for.   This  command  is intended to allow you to write MLisp that
    duplicates the actions of the query-replace-string function.

    For example:

	(re-search-forward "\\(\\w\\w*\\)++")
	(re-replace-search-text "\\1 = \\1 + 1")

DEFAULT BINDING
    None.

SYNOPSIS
    (re-replace-search-text "<replacement>")

SEE ALSO
    query-replace-string
    replace-search-text
    re-query-replace-string
    re-search-forward
[Cre-replace-string]
COMMAND NAME
    re-replace-string

DESCRIPTION
    Replace  all  occurrences  of  a regular expression with a string,
    starting  at  dot  and  ending  and  the  end of the buffer. EMACS
    prompts  for  an old and a new string in the mini-buffer. Dot will
    be left where it started.

    The characters in the replacement string are specially interpreted
    as follows:

     o	Any   character   except   a  special  character  is  inserted
 	unchanged.

     o	A  `\'  followed  by  any character except a digit causes that
	character to be inserted unchanged.
	
     o	A  `\'  followed by a digit n causes the string matched by the
	n'th bracketed expression to be inserted.
	
     o	An  `&'  causes the string matched by the entire search string
	to be inserted.
	
DEFAULT BINDING
    None.

SYNOPSIS
    (re-replace-string "<pattern>" "<replacement>")

SEE ALSO
    replace-string
    query-replace-string
    re-query-replace-string
[Cre-search-forward]
COMMAND NAME
    re-search-forward

DESCRIPTION
    Prompt for a pattern and search for a match in the current buffer,
    moving  forwards  from dot, stopping at the end of the buffer. Dot
    is  left  at the end of the matched string if a match is found, or
    is unmoved if not.

    The  pattern  is  interpreted  as a regular expression and matched
    against the buffer according to the following rules:

     o	Any  character  except  a  special  character  matches itself.
	Special  characters are `\' `[' `.' and sometimes `^' `*' `$'.
	
     o	A `.' matches any character except newline.
	
     o	A  `\' followed by any character except those mentioned in the
	following rules matches that character.
	
     o	A  `\w'  Matches  any word character, as defined by the syntax
	table.
	
     o	A  `\W'  Matches  any  non-word  character,  as defined by the
	syntax table.
	
     o	A  `\b'  Matches  at  a boundary between a word and a non-word
	character, as defined by the syntax table.
	
     o	A `\B' Matches anywhere but at a boundary between a word and a
	non-word character, as defined by the syntax table.
	
     o	A `\`' Matches at the beginning of the buffer.
	
     o	A `\'' Matches at the end of the buffer.
	
     o	A `\<' Matches anywhere before dot.
	
     o	A `\>' Matches anywhere after dot.
	
     o	A `\=' Matches at dot.
	
     o	A non empty  string `s' bracketed ``[s]'' (or ``[^s]'' matches
	any  character  in (or not in) `s'. In `s', `\' has no special
	meaning,  and  `]'  may  only  appear  as  the first letter. A
	sub-string `a-b',  with  `a' and `b' in ascending ASCII order,
	stands for the inclusive range of ASCII characters.
	
     o	A  `\' followed by a digit n matches a copy of the string that
	the  bracketed regular expression beginning with the n th `\('
	matched.
	
     o	A  regular  expression of one of the preceding  forms followed
	by  `*' matches a sequence of 0 or more matches of the regular
	expression.
	
     o	A  regular  expression,  `x', bracketed ``\(x\)'' matches what
	`x' matches.
	
     o	A  regular  expression of this or one of the preceding  forms,
	`x', followed by a regular expression of one of the  preceding
	forms,  `y'  matches  a  match for `x' followed by a match for
	`y',  with the `x' match being as long as possible while still
	permitting a `y' match.
	
     o	A  regular  expression of one of the  preceding forms preceded
	by  `^'  (or  followed by `$'), is constrained to matches that
	begin at the left (or end at the right) end of a line.
	
     o	A  sequence  of  regular  expressions of one of the  preceding
	forms  separated  by  `\|'s  matches  any  one  of the regular
	expressions.
	
     o	A  regular expression of one of the preceding  forms picks out
	the  longest  amongst  the  left  most  matches  if  searching
	forward, right most if searching backward.
	
     o	An  empty  regular  expression  stands  for a copy of the last
	regular expression encountered.

    If syntax-array  is  true  for  the  buffer  the  following syntax
    related  extension  are added to the above.  Initially the pattern
    will  match without regard to the syntax of the characters.  

    The four  RE's  '\s', '\c', '\S' and '\C' change where the pattern
    is allowed to match.

    A '\c' limits a search to comments, use '\c' to remove the limit.

    A '\C'  excludes  comments from the search, use '\C' to remove the
    exclusion.

    A '\s' limits a search to strings, use '\s' to remove the limit.

    A '\S'  excludes  strings from the search, use '\C' to remove the
    exclusion.

    A '\c\s' limits a search to comments and strings.

    A '\C\S' to excludes both comments and strings from the search.

    Syntax example:

    Search for the text "printf" of a string with "error" in it.

    (re-search-forward "\S\Cprintf.*\S.*error")

DEFAULT BINDING
    None.

SYNOPSIS
    (re-search-forward "<pattern>")

SEE ALSO
    re-search-reverse
    search-forward
    search-reverse
[Cre-search-reverse]
COMMAND NAME
    re-search-reverse

DESCRIPTION
    Prompt for a pattern and search for a match in the current buffer,
    moving  backwards  from  dot,  stopping  at  the  beginning of the
    buffer.  Dot  is  left at the beginning of the matched string if a
    match is found, or is unmoved if not.

    Regular   expression  patterns  are  described  in  the  help  for
    re-search-forward.

DEFAULT BINDING
    None.

SYNOPSIS
    (re-search-reverse "<pattern>")

SEE ALSO
    re-search-forward
    search-forward
    search-reverse
[Cread-abbrev-file]
COMMAND NAME
    read-abbrev-file

DESCRIPTION
    Read  in  and define abbreviations appearing in a named file. This
    file  should  have  been written using write-abbrev-file. An error
    message is printed if the file cannot be found.

DEFAULT BINDING
    None.

SYNOPSIS
    (read-abbrev-file "<file-name>")

SEE ALSO
    define-local-abbrev
    read-abbrev-file
    use-abbrev-table
    write-abbrev-file
[Cread-file]
COMMAND NAME
    read-file

DESCRIPTION
    Prompt  for  the name of a file; erase the contents of the current
    buffer;  read the file into the buffer and associate the name with
    the buffer. Dot is set to the beginning of the buffer.

DEFAULT BINDING
    ^X-^R

SYNOPSIS
    (read-file "<file-name>")

SEE ALSO
    append-to-file
    insert-file
    view-file
    visit-file
    write-current-file
    write-file-exit
    write-modified-files
    write-named-file
[Crecursion-depth]
COMMAND NAME
    recursion-depth

DESCRIPTION
    An  MLisp function that returns the current number of active calls
    to recursive-edit.

DEFAULT BINDING
    None.

SYNOPSIS
    (recursion-depth)

SEE ALSO
    recursive-edit
[Crecursive-edit]
COMMAND NAME
    recursive-edit

DESCRIPTION
    The   recursive-edit   function   is   a   call  on  the  keyboard
    read/interpret/execute routine. After recursive-edit is called the
    user  can  enter  commands from the keyboard as usual, except that
    when  he exits EMACS by calling exit-emacs (typing ^C) it actually
    returns  from  the  call to recursive-edit. This function is handy
    for  packages  that  want  to pop into some state, let the user do
    some editing, then when they are done perform some cleanup and let
    the user resume.

    For example, a mail system could use this for message composition.

    The  standard  mode  lines  for  all  the windows displayed on the
    screen  will  be enclosed in [ and ] when a recursive-edit command
    is executing.

DEFAULT BINDING
    None.

SYNOPSIS
    (recursive-edit)

SEE ALSO
    recursion-depth
    exit-emacs
[Credraw-display]
COMMAND NAME
    redraw-display

DESCRIPTION
    Clear   the  screen  and  rewrite  it.  This  is  useful  if  some
    transmission  glitch, or a message from a friend has messed up the
    screen.

DEFAULT BINDING
    ^L

SYNOPSIS
    (redraw-display)

SEE ALSO
[Cregion-around-match]
COMMAND NAME
    region-around-match

DESCRIPTION
    Set  dot and mark around the region matched by the last search. An
    argument of  `n'  puts  dot  and  mark around the n'th sub-pattern
    matched  by  `\(' and `\)'. An argument of zero matches the entire
    string. This can then be used in conjunction with region-to-string
    to extract fields matched by a pattern.

    For  example,  consider  the following fragment that extracts user
    names and host names from mail addresses:

     (re-search-forward "\\([a-z][a-z]*\\) *@ *\\([a-z][a-z]*\\)")
     (region-around-match 1)
     (setq username (region-to-string))
     (region-around-match 2)
     (setq host (region-to-string))

    Applying  this  MLisp  code  to  the  text "send it to FOO@MARVIN"
    would,  set  the  variable  `username'  to  "FOO"  and  `host'  to
    "MARVIN".

DEFAULT BINDING
    None.

SYNOPSIS
    (region-around-match [<expression>])

SEE ALSO
    re-search-forward
    re-search-reverse
    search-forward
    search-reverse
[Cregion-to-string]
COMMAND NAME
    region-to-string

DESCRIPTION
    An  MLisp function that returns the region between dot and mark as
    a string.

DEFAULT BINDING
    None.

SYNOPSIS
    (region-to-string)

SEE ALSO
[Cremove-all-local-bindings]
COMMAND NAME
    remove-all-local-bindings

DESCRIPTION
    Perform  a remove-local-binding for all possible keys; effectively
    undoes all local bindings.

DEFAULT BINDING
    None.

SYNOPSIS
    (remove-all-local-bindings)

SEE ALSO
    remove-binding
    remove-local-binding
[Cremove-binding]
COMMAND NAME
    remove-binding

DESCRIPTION
    Remove  the  global  binding  of  the given key. Actually, it just
    rebinds the key to illegal-operation.

DEFAULT BINDING
    None.

SYNOPSIS
    (remove-binding "<key-sequence>")

SEE ALSO
    remove-all-local-bindings
    remove-local-binding
[Cremove-database]
COMMAND NAME
    remove-database

DESCRIPTION
    This  function  removes  a single database file from the specified
    database search list. the database search list will not be deleted
    when the last file has been removed.

DEFAULT BINDING
    None.

SYNOPSIS
    (remove-database "<database-search-list>" "<database-file>")

SEE ALSO
[Cremove-local-binding]
COMMAND NAME
    remove-local-binding

DESCRIPTION
    Remove the local binding of the given key. The global binding will
    subsequently be used when interpreting the key sequence.

DEFAULT BINDING
    None.

SYNOPSIS
    (remove-local-binding "<key-sequence>")

SEE ALSO
    remove-all-local-bindings
    remove-binding
[Crename-macro]
COMMAND NAME
    rename-macro

DESCRIPTION
    Rename  the  specified  extended command to a new name. You cannot
    rename  to  a  name  used by an already existing wired-in extended
    command.  If  the  new  name  you use exists, the existing code in
    removed.

DEFAULT BINDING
    None.

SYNOPSIS
    (rename-macro "<old-command-name>" "<new-command-name>")

SEE ALSO
    defun
[Creplace-search-text]
COMMAND NAME
    replace-search-text

DESCRIPTION

    Do one replacement of the last pattern searched for.  This command
    is  intended  to  allow  you  to  write  MLisp that duplicates the
    actions of the query-replace-string function.

    For example:

	(search-forward "fred")
	(replace-search-text "joe")

DEFAULT BINDING
    None.

SYNOPSIS
    (replace-search-text "<replacement>")

SEE ALSO
    query-replace-string
    re-replace-search-text
    re-query-replace-string
    re-search-forward
[Creplace-string]
COMMAND NAME
    replace-string

DESCRIPTION
    Replace all occurrences of one string for another, starting at dot
    and  ending at the end of the buffer. EMACS prompts for an old and
    a new string in the mini-buffer. Unlike query-replace-string EMACS
    does not ask  any  questions about particular occurrences, it just
    changes them. Dot will be left where it started.

DEFAULT BINDING
    ESC-R

SYNOPSIS
    (replace-string "<search-string>" "<replacement-string>")

SEE ALSO
    query-replace-string
    re-query-replace-string
    re-replace-string
[Cresume-process]
COMMAND NAME
    resume-process

DESCRIPTION
    Resume a sub-process that has been paused (with the pause-process)
    command.  The  image  in  the process starts working again. If the
    process is not paused, then an error is generated.

DEFAULT BINDING
    None.

SYNOPSIS
    (pause-process "<process-name>")

SEE ALSO
    current-process-name
    force-exit-process
    kill-process
    resume-process
    start-DCL-process
[Creturn-prefix-argument]
COMMAND NAME
    return-prefix-argument

DESCRIPTION
    An MLisp function that causes <expression> to be considered as the
    prefix  argument to the next command executed (even if the command
    is  initiated  from  the keyboard). This can be useful for writing
    alternate prefix argument packages.

    This  command  must  precede  the  function  to which it should be
    applied. If you require to cause the argument to be applied to the
    next  keyboard command, it must be the last command in a series on
    MLisp commands.

DEFAULT BINDING
    None.

SYNOPSIS
    (return-prefix-argument <expression>)

SEE ALSO
    variable prefix-argument
    variable prefix-argument-provided
[Creturn-to-monitor]
COMMAND NAME
    return-to-monitor

DESCRIPTION
    Recursively invoke a new process running DCL, allowing the user to
    enter normal DCL commands. Return to EMACS by using the DCL LOGOUT
    command.

DEFAULT BINDING
    None.

SYNOPSIS
    (return-to-monitor)

SEE ALSO
    pause-emacs
[Cright-marker]
COMMAND NAME
    right-marker

DESCRIPTION
    This function  takes  a  marker  as  a parameter and returns a new
    marker.   The  new marker will at the same position as the original
    but  with  right  hand affinity.  This means that text inserted at
    the  position  of the marker will move the markers position to the
    right.  Left hand affinity markers on the other hand do not move to
    the right as text is inserted.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq right-hand-end right-marker <marker>)

SEE ALSO
    left-marker
[Cright-window]
COMMAND NAME
    right-window

DESCRIPTION
    This commands move the cursor into the window that is to the right
    of  the  current  window.   Unlike  previous-window  that move the
    cursor  without  regard to the physical position of the windows on
    the screen.

    right-window reports  an  error  of  there  is not a window to the
    right of the current window.

DEFAULT BINDING
    None.

SYNOPSIS
    (right-window)

SEE ALSO
    up-window
    down-window
    left-window
[Csave-environment]
COMMAND NAME
    save-environment

DESCRIPTION
    This  command  is  used  to save the current state of Emacs into a
    Emacs environment  file.  The command takes one argument, the file
    name  to  save  the  environment  into.   The default file type is
    ".EMACS_ENVIRONEMNT".

    EMACS  saves enough information in the file to be able to recreate
    the current Emacs session.

    Use the EMACS/RESTORE DCL command to restore a saved environment.

    See the EMACS MANUAL for more details about saved environments.

DEFAULT BINDING
    none
SYNOPSIS
    (save-environment <file-name>
SEE ALSO
[Csave-excursion]
COMMAND NAME
    save-excursion

DESCRIPTION
    An MLisp function that evaluates the given expressions and returns
    the  value of the last expression evaluated. It is much like progn
    except  that  before  any  expressions  are  executed  dot and the
    current  buffer  are  "marked"  (via the marker mechanism) and the
    search-string is saved.

    After the  last  expression is executed dot and the current buffer
    are reset to the marked values. And the search-string is restored.
    This  properly takes into account all movements of dot, insertions
    and deletions that occur.

    Save-excursion is  useful  in MLisp functions where you want to go
    and  do  something somewhere else in this or some other buffer but
    want  to  return  to  the  same  place when you have finished. The
    setting of the current buffer mark is also remembered. If the mark
    was  not  set  before  calling  save-excursion, it will not be set
    afterwards.

    For example, inserting a tab at the beginning of the current line.

	(save-excursion
	    (beginning-of-line)
	    (insert-character '\t')
	)

DEFAULT BINDING
    None.

SYNOPSIS
    (save-excursion [local-declarations...] <expression> ...)

SEE ALSO
    save-window-excursion
    save-restriction
[Csave-restriction]
COMMAND NAME
    save-restriction

DESCRIPTION
    Save  the  current  dot,  mark,  buffer,  search-string and buffer
    restriction;  execute  the  provided  expressions  and restore the
    saved  information.  Return  the  value  of  the  last  expression
    evaluated.    If   the   mark   was   not   set   before   calling
    save-restriction, it will not be set afterwards.

DEFAULT BINDING
    None.

SYNOPSIS
    (save-restriction [local-declarations...] <expression> ...)

SEE ALSO
    save-excursion
    save-window-excursion
[Csave-window-excursion]
COMMAND NAME
    save-window-excursion

DESCRIPTION
    Save the  current  dot,  mark,  buffer,  search-string  and window
    state; execute the expressions; and restore the saved information.
    Return the value of the last expression evaluated. If the mark was
    not  set  before calling save-window-excursion, it will not be set
    afterwards.

    When  the  window state is saved EMACS remembers which buffers were
    visible.  When  it is restored, EMACS makes sure that exactly those
    buffers are visible if the still exist.

DEFAULT BINDING
    None.

SYNOPSIS
    (save-window-excursion [local-declarations...] <expression> ...)

SEE ALSO
    save-excursion
    save-restriction
[Cschedule-procedure]
COMMAND NAME
    schedule-procedure

DESCRIPTION
    Execute the given procedure after the specified number of seconds.
    If  the  number  of seconds is zero, then any outstanding schedule
    for the specified procedure is removed. If a schedule is specified
    for a procedure that already has a schedule, then the old schedule
    is thrown away first.

DEFAULT BINDING
    None.

SYNOPSIS
    (schedule-procedure "<command-name>")

SEE ALSO
[Cscroll-one-column-left]
COMMAND NAME
    scroll-one-column-left

DESCRIPTION
    Scroll the current window one column to the left.

DEFAULT BINDING
    None

SYNOPSIS
    (scroll-one-column-left)

SEE ALSO
    scroll-one-line-up
    scroll-one-line-down
    scroll-one-column-right
    column-to-left-of-window

    window-first-column
[Cscroll-one-column-right]
COMMAND NAME
    scroll-one-column-right

DESCRIPTION
    Scroll the current window one column to the right.

DEFAULT BINDING
    None

SYNOPSIS
    (scroll-one-column-right

SEE ALSO
    scroll-one-line-up
    scroll-one-line-down
    scroll-one-column-left
    column-to-left-of-window

    window-first-column
[Cscroll-one-line-down]
COMMAND NAME
    scroll-one-line-down

DESCRIPTION
    Reposition  the  current  window on the current buffer so that the
    line  which is currently the second to the last line in the window
    becomes the  last  -- effectively move the buffer down one line in
    the window.

DEFAULT BINDING
    ESC-Z

SYNOPSIS
    (scroll-one-line-down)

SEE ALSO
    scroll-one-line-up
    scroll-one-column-left
    scroll-one-column-right
[Cscroll-one-line-up]
COMMAND NAME
    scroll-one-line-up

DESCRIPTION
    Reposition  the  current  window on the current buffer so that the
    line  which is currently the second line in the window becomes the
    first -- effectively move the buffer up one line in the window.

DEFAULT BINDING
    ^Z

SYNOPSIS
    (scroll-one-line-up)

SEE ALSO
    scroll-one-line-down
[Csearch-forward]
COMMAND NAME
    search-forward

DESCRIPTION
    Prompt  for a string and search for a match in the current buffer,
    moving  forwards  from  dot and stopping at the end of the buffer.
    Dot  is left at the end of the matched string if a match is found,
    or is unmoved if not.

DEFAULT BINDING
    ^^-S or ^S

SYNOPSIS
    (search-forward "<search-string>")

SEE ALSO
    re-search-forward
    re-search-reverse
    search-reverse
[Csearch-reverse]
COMMAND NAME
    search-reverse

DESCRIPTION
    Prompt  for a string and search for a match in the current buffer,
    moving  backwards  from  dot  and stopping at the beginning of the
    buffer.  Dot  is  left at the beginning of the matched string if a
    match is found, or is unmoved if not.

DEFAULT BINDING
    ^R

SYNOPSIS
    (search-reverse "<search-string>")

SEE ALSO
    re-search-forward
    re-search-reverse
    search-forward
[Cself-insert]
COMMAND NAME
    self-insert

DESCRIPTION
    This  command  is bound to those keys which are supposed to insert
    themselves  into  the  current  buffer.  It is roughly the same as
    (insert-character  (last-key-struck))  with  the exception that it
    does not work unless it is bound to a key.

DEFAULT BINDING
    inserting keys

SYNOPSIS
    None.

SEE ALSO
[Csend-eof-to-process]
COMMAND NAME
    send-eof-to-process (Unix)

DESCRIPTION
    Send an end-of-file (EOF) to the named process.

DEFAULT BINDING
    none.

SYNOPSIS
    (send-eof-to-process <process-name>)

EXAMPLE

SEE ALSO

[Csend-string-to-process]
COMMAND NAME
    send-string-to-process

DESCRIPTION
    Send  the  specified string to the specified sub-process. An error
    is  generated  if  the target process is not asking for input. Use
    the  wait-for-process-input-request  command  to  insure  that the
    process is ready.

DEFAULT BINDING
    None.

SYNOPSIS
    (send-string-to-process "<process-name>" "<expression>")

SEE ALSO
    current-process-name
    process-output
    wait-for-process-input-request
[Csend-string-to-terminal]
COMMAND NAME
    send-string-to-terminal

DESCRIPTION
    Send  the  string  argument  to the terminal with no conversion or
    interpretation.  This should only be used for such applications as
    loading  function  keys  when EMACS starts up. If you screw up the
    screen,   EMACS   won't   know  about  it  and  won't  fix  it  up
    automatically for you -- you will have to use redraw-display.

DEFAULT BINDING
    None.

SYNOPSIS
    (send-string-to-terminal "<expression>")

SEE ALSO
    redraw-display
[Cset]
COMMAND NAME
    set

DESCRIPTION
    Set  the  value of some variable. EMACS will ask for the name of a
    variable  and  a  value  to  set it to. The variables control such
    things  as margins, display layout options, the behaviour of search
    commands,  and much more. The available variables and switches are
    described  elsewhere.  Note  that  if  set  is used from MLisp the
    variable name must be a string: (set "left-margin" 77).

    If  you  require  a value of true, then from the terminal, you can
    type  the string true or on. If you require a value of false, then
    from the terminal, you can type the string false or off.

DEFAULT BINDING
    None.

SYNOPSIS
    (set "<variable-name>" <expression>)

SEE ALSO
    setq
    setq-default
    set
[Cset-default]
COMMAND NAME
    set-default

DESCRIPTION
    Set  the  default value of a variable. EMACS will ask for the name
    of a variable and a value to set it to. The variables control such
    things  as margins, display layout options, the behaviour of search
    commands,  and much more. The available variables and switches are
    described  elsewhere.  Note that if set-default is used from MLisp
    the variable name must be a string:

	(set-default "case-fold-search" 1).

    If  you  require  a value of true, then from the terminal, you can
    type  the string true or on. If you require a value of false, then
    from the terminal, you can type the string false or off.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-default "<variable-name>" <expression>)

SEE ALSO
    setq-default
    set
    setq
[Cset-current-process]
COMMAND NAME
    set-current-process

DESCRIPTION
    Set  the current sub-process to the specified process. The current
    sub-process name is used in all of the sub-process commands when a
    blank process name is specified.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-current-process "<process-name>")

SEE ALSO
    current-process-name
    list-processes
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
[Cset-mark]
COMMAND NAME
    set-mark

DESCRIPTION
    Put  the marker for this buffer at the place where dot is now, and
    leave  it  there.  As text is inserted or deleted around the mark,
    the  mark  will remain in place. Use exchange-dot-and-mark to move
    to the currently marked position.

DEFAULT BINDING
    ^@ (NULL)

SYNOPSIS
    (set-mark)

SEE ALSO
    unset-mark
    mark
    exchange-dot-and-mark
[Cset-process-input-procedure]
COMMAND NAME
    set-process-input-procedure

DESCRIPTION
    Set  the  input  procedure for the specified sub-process. Whenever
    the  sub-process  asks  for input, the specified procedure is run.
    This  procedure  may  then  supply  the  process with input (using
    send-string-to-process).

DEFAULT BINDING
    None.

SYNOPSIS
    (set-process-input-procedure "<process-name>" "<command-name>")

SEE ALSO
    list-processes
    send-string-to-process
    set-current-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
[Cset-process-name]
COMMAND NAME
    set-process-name

DESCRIPTION
    Change  the  name  of a sub-process. If the process is the current
    process, then the current process name is changed to the new name.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-process-name "<old-process-name>" "<new-process-name>")

SEE ALSO
    current-process-name
    list-processes
    set-current-process
    set-process-input-procedure
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
[Cset-process-output-buffer]
COMMAND NAME
    set-process-output-buffer

DESCRIPTION

    Connect  a  buffer  to  the  output  of the specified process. All
    output  from the process appears in this buffer. The buffer is not
    allowed   to   grow   indefinitely:  if  it  becomes  larger  than
    maximum-DCL-buffer-size   characters,  it  will  be  truncated  by
    DCL-buffer-reduction  characters  before  the next output from the
    process is inserted into it.

    If  a  process has has an output procedure, this is called instead
    of inserting the text in the buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-process-output-buffer "<process-name>" "<buffer-name>")

SEE ALSO
    set-current-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-termination-procedure
    wait-for-process-input-request
[Cset-process-output-procedure]
COMMAND NAME
    set-process-output-procedure

DESCRIPTION
    Define  the  name  of  an  MLisp  procedure  to  be  called when a
    sub-process  produces  output. This procedure is called instead of
    the  insertion  of  any  output  into  an  output  buffer. Use the
    process-output  function  to obtain the latest line of output from
    the process.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-process-output-procedure "<process-name>" "<command-name>")

SEE ALSO
    list-processes
    process-output
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-termination-procedure
[Cset-process-termination-procedure]
COMMAND NAME
    set-process-termination-procedure

DESCRIPTION
    Define  an  MLisp  procedure  to  be  called  after  a process has
    terminated. This procedure should be used to tidy up any resources
    or MLisp variables that the process may have used.

DEFAULT BINDING
    None.

SYNOPSIS
    (set-process-termination-procedure "<process-name>" "<command-name>")

SEE ALSO
    current-process-name
    set-current-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
[Csetq]
COMMAND NAME
    setq

DESCRIPTION
    Assign a new value to a variable. Variables may be assigned either
    string,  integer or marker values. (setq i 5) sets i to 5; (setq s
    (concat "a" "b")) sets s to "ab".

DEFAULT BINDING
    None.

SYNOPSIS
    (setq <variable-name> <expression>)

SEE ALSO
    set
[Csetq-array]
COMMAND NAME
    setq-array

DESCRIPTION
    This  command is used to store values into an array. Its arguments
    are an array to store into, the indices of the array and the value
    to  store  in  the  array. setq-array check that exactly the right
    number of indices are specified and that each index is in range.

EXAMPLE
    Set element 5 to be "hello fred" in array vector.

    (setq-array vector 5 "hello fred")

DEFAULT BINDING
    none

SYNOPSIS
    (setq-array <array> <indices> <value>)

SEE ALSO
    type-of-expression
    bounds-of-array
    fetch-array
    array
[Csetq-default]
COMMAND NAME
    setq-default

DESCRIPTION
    Assign  a  new  default  value  to  a variable. Variables may have
    either   string,   integer   or   marker   values.   (setq-default
    case-fold-search 1) sets the default-case-fold-search to 1.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq-default <variable-name> <expression>)

SEE ALSO
    set-default
[Cshell]
COMMAND NAME
    shell (package)

DESCRIPTION
    Start  up  an interactive DCL session in an EMACS buffer "shell-n"
    (where  n  is a sequence number used to identify multiple shells).
    The following keys are specially defined

    <CR> Send the current line to DCL as a command.

    <LF> Send the current line to the running program.

    ^]	 Terminate  the  shell  session.  The  keys all revert back to
	 their usual meaning and the mode line is reset.

    ^C	 Force  the  running  program  to  terminate.  The termination
	 reason usually is %SYSTEM-F-ABORT, abort.

    ^R	 Insert into the buffer the last command sent to DCL.

    ?    Display a short help text.

DEFAULT BINDING
    None.

SYNOPSIS
    (shell)

SEE ALSO
[Cshow-buffer]
COMMAND NAME
    show-buffer (package)

DESCRIPTION
    This  command  displays  information about the current buffer. The
    displayed information is put into the buffer "Show Buffer".

DEFAULT BINDING
    None

SYNOPSIS
    (show-buffer)

SEE ALSO
    list-buffers
[Cshrink-window]
COMMAND NAME
    shrink-window

DESCRIPTION
    Make  the  current window <prefix-argument> lines shorter, and the
    window  below  (or  the  one  above  if  there is no window below)
    <prefix-argument> lines taller. Can't be used if there is only one
    window on the screen.

DEFAULT BINDING
    ^X-^Z

SYNOPSIS
    (shrink-window)

SEE ALSO
    enlarge-window
[Csit-for]
COMMAND NAME
    sit-for

DESCRIPTION
    Update the  display and pause for n/10 seconds. (sit-for 10) waits
    for one second. This is useful in such things as a Lisp auto-paren
    balancer.  If  an  argument  of  zero  is  used,  then no pause is
    executed, but the display is still updated.

DEFAULT BINDING
    None.

SYNOPSIS
    (sit-for <expression>)

SEE ALSO
[Cspell-check-word]
COMMAND NAME
    spell-check-word (package)

DESCRIPTION
    Check the  spelling  of  the  supplied  word. If the word is spelt
    correctly then 1 is returned otherwise 0 is returned.

DEFAULT BINDING
    None.

SYNOPSIS
    (spell-check-word "word")

SEE ALSO
    correct-word
    get-tty-correction
[Csplit-current-window]
COMMAND NAME
    split-current-window

DESCRIPTION

    Take the  current window and split it into two windows, one on top
    of the other, dividing the space on the screen equally between the
    two windows.  An arbitrary number of windows can be created -- the
    only  limit  is on the number of lines on the screen.  Each window
    must be at least one line high.

DEFAULT BINDING
    ^X-2

SYNOPSIS
    (split-current-window)

SEE ALSO
    delete-other-windows
    delete-window
    enlarge-window
    next-window
    previous-window
    shrink-window
    split-current-window-vertically
[Csplit-current-window-vertically]
COMMAND NAME
    split-current-window-vertically

DESCRIPTION

    Take the  current  window  and  split it into two windows, size by
    size,  dividing  the  space  on the screen equally between the two
    windows.   An  arbitrary  number  of windows can be created -- the
    only limit is on the width of the screen.  Each window requires to
    be at least one column wide.


DEFAULT BINDING
    ^X-3

SYNOPSIS
    (split-current-window-vertically)

SEE ALSO
    delete-other-windows
    delete-window
    enlarge-window
    next-window
    previous-window
    shrink-window
    split-current-window-vertically
[Cstart-DCL-process]
COMMAND NAME
    start-DCL-process

DESCRIPTION
    Start  a  sub-process  running a CLI. Use  the  other  sub-process
    commands  to  attach  buffers and MLisp procedures to the process.
    The new process becomes the current process.

    This function is a synonym for start-process.

DEFAULT BINDING
    None.

SYNOPSIS
    (start-DCL-process "<process-name>")

SEE ALSO
    start-process
    force-exit-process
    kill-process
    pause-process
    resume-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
    wait-for-process-input-request
[Cstart-process]
COMMAND NAME
    start-process

DESCRIPTION
    Start  a  sub-process  running a CLI. Use  the  other  sub-process
    commands  to  attach  buffers and MLisp procedures to the process.
    The new process becomes the current process.

DEFAULT BINDING
    None.

SYNOPSIS
    (start-process "<process-name>")

SEE ALSO
    force-exit-process
    kill-process
    pause-process
    resume-process
    set-process-input-procedure
    set-process-name
    set-process-output-buffer
    set-process-output-procedure
    set-process-termination-procedure
    wait-for-process-input-request
    start-DCL-process
[Cstart-remembering]
COMMAND NAME
    start-remembering

DESCRIPTION
    All  following  keystrokes  will  be  remembered  by  EMACS as the
    current keyboard macro. The functions to which they are bound will
    still be executed.

DEFAULT BINDING
    ^X-(

SYNOPSIS
    (start-remembering)

SEE ALSO
    define-keyboard-macro
    execute-keyboard-macro
    stop-remembering
[Cstop-remembering]
COMMAND NAME
    stop-remembering

DESCRIPTION
    Stop  remembering  keystrokes,  as initiated by start-remembering.
    The remembered keystrokes are not forgotten and may be re-executed
    with execute-keyboard-macro command.

DEFAULT BINDING
    ^X-)

SYNOPSIS
    (stop-remembering)

SEE ALSO
    define-keyboard-macro
    execute-keyboard-macro
    start-remembering
[Cstring-extract]
COMMAND NAME
    string-extract

DESCRIPTION

    Extract a  subpart  of the <string> between positions <start>
    and  <end>.   The  first  string position is at 0, zero.  Use
    negative position values to index from the end of the string.

    If <start>  is  negative and <end> is zero then the end taken
    to be the end of the sting.

    Any position  that is outside of the strings length is set to
    the  start,  if  its  before  the  string,  of the end of the
    string.

DEFAULT BINDING
    none.

SYNOPSIS
    (string-extract <string> <start> <end>)

EXAMPLE
    (string-extract "abcdefg" 0 3) => "abc"
    (string-extract "abcdefg" 1 3) => "bc"
    (string-extract "abcdefg" 3 1) => "bc"
    (string-extract "abcdefg" -3 0) => "efg"
    (string-extract "abcdefg" 0 -3) => "abcd"
    (string-extract "abcdefg" -3 -1) => "ef"
    (string-extract "abcdefg" -1 -3) => "ef"
    (string-extract "abcdefg" 0 0) => ""
    (string-extract "abcdefg" 2 2) => ""

SEE ALSO

[Cstring-index-of-first]
COMMAND NAME
    string-index-of-first

DESCRIPTION

    Returns the  zero  based  index  into  <string>  of the first
    occurrance  of  the  string  <find>.   -1  is returned if the
    string is not found.

DEFAULT BINDING
    none.

SYNOPSIS
    (string-index-of-first <string> <find>)

EXAMPLE
    (string-index-of-first "abcdefgabcdefg" "cde") => 2
    (string-index-of-first "abcdefgabcdefg" "xyz") => -1

SEE ALSO
    string-index-of-last
    string-index-of-string
[Cstring-index-of-last]
COMMAND NAME
    string-index-of-last

DESCRIPTION

    Returns the  zero  based  index  into  <string>  of the last
    occurrance  of  the  string  <find>.   -1  is returned if the
    string is not found.

DEFAULT BINDING
    none.

SYNOPSIS
    (string-index-of-last <string> <find>)
    

EXAMPLE
    (string-index-of-last "abcdefgabcdefg" "cde") => 9
    (string-index-of-last "abcdefgabcdefg" "xyz") => -1

SEE ALSO
    string-index-of-first
    string-index-of-string
[Cstring-index-of-string]
COMMAND NAME
    string-index-of-string

DESCRIPTION

    Returns the  zero  based  index  into  <string>  of the first
    occurrance  of the string <find> start at position <pos>.  -1
    is returned if the string is not found.

DEFAULT BINDING
    none.

SYNOPSIS
    (string-index-of-string <string> <find> <pos>)

EXAMPLE
    (string-index-of-string "abcdefgabcdefg" "cde" 1) => 2
    (string-index-of-string "abcdefgabcdefg" "cde" 4) => 9
    (string-index-of-string "abcdefgabcdefg" "cde" 10) => -1
    (string-index-of-string "abcdefgabcdefg" "xyz" 5) => -1

SEE ALSO
    string-index-of-first
    string-index-of-last
[Cstring-to-char]
COMMAND NAME
    string-to-char

DESCRIPTION
    Return  the  integer  value  of  the first character of its string
    argument.

DEFAULT BINDING
    None.

SYNOPSIS
    (string-to-char "<expression>")
    (string-to-char "A") => 'A'

SEE ALSO
    char-to-string
[Csynchronise-files]
COMMAND NAME
    synchronise-files

DESCRIPTION

    Use this  command to have Emacs update all file buffers with newer
    copies of the files from the disk.  For each file that has a newer
    version  on  the disk Emacs prompts asking you if you wish to read
    in the newer file.

DEFAULT BINDING
    None.

SYNOPSIS
    (synchronise-files)

SEE ALSO
[Csubstr]
COMMAND NAME
    substr

DESCRIPTION
    An  MLisp  function  that  returns  the sub-string of string <str>
    starting  at position <pos> (numbering from 1) and running for <n>
    characters. If <pos> is less than 0, then the length of the string
    is added to it; the same is done for <n>.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq <string> (substr <str> <pos> <n>))
    (substr "kzin" 2 2) => "zi"
    (substr "blotto.c" -2 2) => ".c"

SEE ALSO
[Cswitch-to-buffer]
COMMAND NAME
    switch-to-buffer

DESCRIPTION
    Prompt  for  the  name  of  the  buffer  and associate it with the
    current  window.  The  old  buffer  associated with this window is
    merely lost. If the new buffer does not exist, it will be created.
    Buffer name recognition is performed by using the ESC key.

DEFAULT BINDING
    ^X-B

SYNOPSIS
    (switch-to-buffer "<buffer-name>")

SEE ALSO
    pop-to-buffer
    temp-use-buffer
    use-old-buffer
[Csystem-name]
COMMAND NAME
    system-name

DESCRIPTION
    An  MLisp  function  that  returns the name of the system on which
    EMACS  is  being  run.  This  should  be the DECnet or ArpaNet (or
    whatever) host name of the machine.

DEFAULT BINDING
    None.

SYNOPSIS
    (system-name)

SEE ALSO
[Csys$fao]
COMMAND NAME
    sys$fao (VMS package)

DESCRIPTION
    This command calls the VMS system service SYS$FAO. See the VAX/VMS
    System Services reference manual for details of FAO operation.

    Emacs calls SYS$FAO with a ctrstr and the parameters P1 to Pn. The
    par-desc  string  parameter  describes  each of P1 to Pn. For each
    parameter to be passed to SYS$FAO include 1 letter to describe the
    type  of  the  parameter. Use "s" if the parameter is a string and
    "n" if it is a number.
    

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$fao ctrstr par-desc P1 ... Pn)

EXAMPLE
    (sys$fao "!AS = !XL at !%D" "snn" "status" 145 0)

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getdvi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$filescan]
COMMAND NAME
    sys$filescan (VMS package)

DESCRIPTION
    This  command  calls  the VMS system service SYS$FILESCAN. See the
    VAX/VMS  System  Services  reference  manual  for  details  of FAO
    operation.

    Emacs calls  SYS$FILESCAN  with  the  srcstr and an item list. The
    item  list  is  specified as pairs of parameters, item and result.
    The  result  parameters  have  to be variables. The result of each
    item is return into the result variables.

    The sys$filescan   function   returns  the  flgflags  returned  by
    SYS$FILESCAN.

    The FSCN symbols are defined by loading the MLisp file FSCNDEF.

DEFAULT BINDING
    None.

SYNOPSIS
    (setq fldflags (sys$filescan srcstr [item result] ...))

EXAMPLE
    Split a filespec up into its name and type strings.

    (progn name type
	(execute-mlisp-file "fscndef")
	(sys$filescan "sys$system:Emacs.exe" fscn$_name name fscn$_type type)
	(message "file name is " name " and the type is " type)
    )

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getdvi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$getmsg]
COMMAND NAME
    sys$getmsg (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$GETMSG. See the
    VAX/VMS  System  Services  reference  manual for details of GETMSG
    operation.

    Emacs calls  SYS$GETMSG  with  the  msgid  and  the optional flags
    parameter. sys$getmsg returns the string message as its result.
    
DEFAULT BINDING
    None.

SYNOPSIS
    (setq msgtext (sys$getmsg msgid [flags]))

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getdvi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$setprn]
COMMAND NAME
    sys$setprn (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$SETPRN. See the
    VAX/VMS  System  Services  reference  manual for details of SETPRN
    operation.

    Emacs calls SYS$SETPRN with the prcnam argument to set the process
    name.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$setprn prcnam)

EXAMPLE
    (sys$setprn (concat "Emacs$" (getenv "TT")))

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getdvi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$getdvi]
COMMAND NAME
    sys$getdvi (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$GETDVI. See the
    VAX/VMS  System  Services  reference  manual for details of GETDVI
    operation.

    Emacs calls  SYS$GETDVI with the devnam argument and an item list.
    The  item  list is made up from item, value pairs of parameters to
    sys$getdvi.  After  sys$getdvi  returns the requested items values
    are stored in the value variables.

    The DVI$ symbols are defined by loading the DVIDEF MLisp file.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$getdvi devnam item value ...)

EXAMPLE
    (sys$getdvi "TT" dvi$_opcnt op-count dvi$_tt_phydevnam real-dev-name)    

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getdvi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$getjpi]
COMMAND NAME
    sys$getjpi (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$GETJPI. See the
    VAX/VMS  System  Services  reference  manual for details of GETJPI
    operation.

    Emacs calls  SYS$GETJPI  with  the  pidadr and prcnam arguments as
    specified  in  the  ctrl-str parameter. If ctrl-str has the letter
    "i"  in  it the pidadr parameter is included. If the letter "n" is
    in  it the prcnam parameter is included. The pidadr parameter must
    be  passed  to  sys$getjpi in a variable. After sys$getjpi returns
    the  pidadr  variable  is  updated  with any new value returned by
    SYS$GETJPI.

    The  item  list is made up from item, value pairs of parameters to
    sys$getjpi.  After  sys$getjpi  returns the requested items values
    are stored in the value variables. The result of sys$getjpi is the
    status  returned  from  VMS.  Use  sys$getmsg to format the status
    value.

    The JPI$ symbols are defined by loading the JPIDEF MLisp file.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$getjpi ctrl-str [pidadr] [prcnam] item value ...)

EXAMPLE
    Find the process name of the current process.

    (sys$getjpi "" jpi$_prcnam process-name)

    Find the username of process "BATCH_145".

    (sys$getjpi "n" "BATCH_145" jpi$_username user-name)

    Find the master PID of the process with PID 12432.

    (setq pid 12432)
    (sys$getjpi "i" pid jpi$_master_pid master-pid)

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getjpi
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$getqui]
COMMAND NAME
    sys$getqui (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$GETQUI. See the
    VAX/VMS  System  Services  reference  manual for details of GETQUI
    operation.

    Emacs calls  SYS$GETQUI  with  the func argument and an item list.
    The  item  list  is made up from item value pairs of parameters to
    sys$getqui.  After  sys$getqui  returns the requested items values
    are stored in the value variables.

    The QUI$ symbols are defined by loading the QUIDEF MLisp file.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$getqui func item value ...)

EXAMPLE
    See QUEUES.ML for example of use.

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getqui
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$getsyi]
COMMAND NAME
    sys$getsyi (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$GETSYI. See the
    VAX/VMS  System  Services  reference  manual for details of GETSYI
    operation.

    Emacs calls  SYS$GETSYI  with  the  pidadr and prcnam arguments as
    specified  in  the  ctrl-str parameter. If ctrl-str has the letter
    "i"  in it the csidadr parameter is included. If the letter "n" is
    in  it  the  nodename  parameter is included. The csidadr parameter
    must  be  passed  to  sys$getsyi  in  a variable. After sys$getsyi
    returns  the  csidadr  variable  is  updated  with  any  new value
    returned by SYS$GETSYI.

    The  item  list is made up from item, value pairs of parameters to
    sys$getsyi.  After  sys$getsyi  returns the requested items values
    are stored in the value variables. The result of sys$getsyi is the
    status  returned  from  VMS.  Use  sys$getmsg to format the status
    value.

    The SYI$ symbols are defined by loading the SYIDEF MLisp file.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$getsyi ctrl-str [csidadr] [nodename] item value ...)

EXAMPLE
    Find the SCS node name of this node.

    (sys$getsyi "" syi$_scsnode scs-node)

    Find the number of votes node "HEART" has.

    (sys$getsyi "n" "HEART" syi$_node_votes votes)

    Find the hardware type of csid 10018.

    (setq csid 10018)
    (sys$getsyi "i" csid syi$_node_hwtype hardware-type)

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$getsyi
    sys$getsyi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Csys$sndjbc]
COMMAND NAME
    sys$sndjbc (VMS package)

DESCRIPTION
    This  command  calls  the  VMS  system service SYS$SNDJBC. See the
    VAX/VMS  System  Services  reference  manual for details of SNDJBC
    operation.

    Emacs calls  SYS$SNDJBC  with  the func argument and an item list.
    The  item  list  is  made  up from item value pairs of parameters.

    The SJC$ symbols are defined by loading the SJCDEF MLisp file.

DEFAULT BINDING
    None.

SYNOPSIS
    (sys$sndjbc func item value ...)

EXAMPLE
    See QUEUES.ML for examples of use.

SEE ALSO
    sys$fao
    sys$filescan
    sys$getmsg
    sys$setprn
    sys$sndjbc
    sys$getjpi
    sys$getqui
    sys$getsyi
    sys$sndjbc
[Ctemp-use-buffer]
COMMAND NAME
    temp-use-buffer

DESCRIPTION
    Switch to a named buffer without changing window associations. The
    commands pop-to-buffer and switch-to-buffer both cause a window to
    be  bound  to the selected buffer, temp-use-buffer does not. There
    is  a  problem  that  you must beware when using this command: the
    keyboard  command  driver  insists  that  the  buffer  tied to the
    current window be the current buffer, if it sees a difference then
    it  changes  the  current buffer to be the one tied to the current
    window.  This  means that temp-use-buffer will be ineffective from
    the keyboard, switch-to-buffer should be used instead.

DEFAULT BINDING
    None.

SYNOPSIS
    (temp-use-buffer "<buffer-name>")

SEE ALSO
    switch-to-buffer
    pop-to-buffer
    use-old-buffer
[Ctest-abbrev-expand]
COMMAND NAME
    test-abbrev-expand

DESCRIPTION
    An  MLisp  function that return the abbreviation expansion for the
    specified  string. An error is generated if there is no expansion.

    The format of the returned string is
	
	"abc" => "alphabet"

    for the abbreviation abc expanding to alphabet.

DEFAULT BINDING
    None.

SYNOPSIS
    (test-abbrev-expand "<abbreviation>")

SEE ALSO
    define-local-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    use-abbrev-table
    write-abbrev-file
[Ctext-mode]
COMMAND NAME
    text-mode (package)

DESCRIPTION
    Set  the  major mode of the current buffer to text-mode. This mode
    is useful  for writing justified or filled and justified text. The
    following keys are defined:

    ESC-^H
	to set the right margin.

    ESC-1
	to set the left margin.

    ESC-c
	to centre the current line.

    ESC-i
	to set the indent offset for the paragraphs.

    ESC-j
	to re-justify the current paragraph.

    ESC-J
	to justify all paragraphs in the current region.

    ESC-m
	to toggle justify minor mode.

DEFAULT BINDING
    None.

SYNOPSIS
    (text-mode)

SEE ALSO
    normal-mode
    c-mode
    lisp-mode
[Cto-col]
COMMAND NAME
    to-col

DESCRIPTION
    An  MLisp  function  that  inserts  tabs  and  spaces  to move the
    following character to printing column n in the current buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (to-col <expression>)

SEE ALSO
[Ctop-level]
COMMAND NAME
    top-level

DESCRIPTION
    This  command  will  take  you  back  to the top level exiting all
    recursive   edits.   This  command  allows  to  you  recover  from
    situations  where the recursion depth make typing  ^C tedious. All
    Mock Lisp code is aborted.

    CAVEAT:  If  you  use  this  in  a  package that alters and buffer
    association  (e.g.  syntax  table or local keymap) then these will
    stay in effect after control returns to the top level.

DEFAULT BINDING
    None.

SYNOPSIS
    (top-level)

SEE ALSO
[Ctrace]
COMMAND NAME
    trace (package)

DESCRIPTION
    This  is  the  default  trace function. It is auto-loaded from the
    TRACE  package.  It allows Mock Lisp users extensive debugging and
    tracing facilities to help with the development of Mock Lisp code.

    After trace-mode has been turned on each MLisp expression executed
    in  displayed  in  the  minibuffer  line.  Then  trace  pauses for
    ``trace-mode'' tenths of a second.

DEFAULT BINDING
    None.

SYNOPSIS
    None.

SEE ALSO
    variable trace-mode
    variable trace-hook
    decompile-current-line
    decompile-mlisp-function
[Ctranspose-characters]
COMMAND NAME
    transpose-characters

DESCRIPTION
    Take  the  two  characters preceding dot and exchange them. One of
    the  most  common  errors  for  typists to make is transposing two
    letters,  typing  "hte"  when  "the" is meant. ^T makes correcting
    these errors easy, especially if you can develop a "^T reflex".

DEFAULT BINDING
    ^T

SYNOPSIS
    (transpose-characters)

SEE ALSO
[Ctype-of-expression]
COMMAND NAME
    type-of-expression

DESCRIPTION
    This   command  returns  a  string  describing  the  type  of  the
    expression  that  is  its  argument. The string will be one of the
    following:-

	integer	    - expression is an integer
	string	    - expression is a string
	marker	    - expression is a marker
	windows	    - expression is a set of windows
	array	    - expression is an array

EXAMPLE
    Check that fred is an array.

    (if (!= "array" (type-of-expression fred))
	(error-message "fred should be an array"))

DEFAULT BINDING
    none

SYNOPSIS
    (type-of-expression <expression>)

SEE ALSO
[Cundo]
COMMAND NAME
    undo

DESCRIPTION
    The  undo functions starts off a sequence of undo operations which
    are  continued  with the use of the undo-more function. Each undo,
    or  undo-more  undoes  one  more  series  of  changes  to the next
    undo-boundary.  undo-boundaries  are  planted before each keyboard
    key stroke.

    A  user interface package has been developed for undo called UNDO,
    and the function used to undo changes is called new-undo.

DEFAULT BINDING
    ^X-^U

SYNOPSIS
    (undo)

SEE ALSO
    undo-boundary
    undo-more
    new-undo
[Cundo-boundary]
COMMAND NAME
    undo-boundary

DESCRIPTION
    All  operations  between  a pair of calls to undo-boundary will be
    undone   by   undo   and   undo-more.   undo-boundary   is  called
    automatically between keystrokes.

DEFAULT BINDING
    None.

SYNOPSIS
    (undo-boundary)

SEE ALSO
    undo
    undo-more
    new-undo
[Cundo-more]
COMMAND NAME
    undo-more

DESCRIPTION
    Each  time undo-more is called the state of the buffers is restore
    to  the  previous  undo-boundary.  If  called  from  the  keyboard
    processor,  it  will  simply  undo the first call to undo. Use the
    UNDO package.

DEFAULT BINDING
    None.

SYNOPSIS
    (undo-more)

SEE ALSO
    undo
    undo-boundary
    new-undo
[Cunlink-file]
COMMAND NAME
    unlink-file

DESCRIPTION
    An MLisp functions which deletes the specified file.

DEFAULT BINDING
    None.

SYNOPSIS
    (unlink-file "<file-spec>")

SEE ALSO
[Cunset-mark]
COMMAND NAME
    unset-mark

DESCRIPTION
    The mark in the current buffer is destroyed, unset.

DEFAULT BINDING
    C-X C-@

SYNOPSIS
    (unset-mark)

SEE ALSO
    set-mark
[Cup-window]
COMMAND NAME
    up-window

DESCRIPTION
    This commands  move  the  cursor into the window that is above the
    current  window.   Unlike  previous-window  that  moves the cursor
    without  regard  to  the  physical  position of the windows on the
    screen.

    up-window reports  an  error  of  there  is not a window above the
    current window.

DEFAULT BINDING
    None.

SYNOPSIS
    (up-window)

SEE ALSO
    down-window
    left-window
    right-window
[Cuse-abbrev-table]
COMMAND NAME
    use-abbrev-table

DESCRIPTION
    Set the current local abbreviation table to the one with the given
    name.  Local  abbreviation  tables  are  buffer  specific  and are
    usually  set depending on the major mode. Several buffers may have
    the   same  local  abbreviation  table.  If  either  the  selected
    abbreviation  table or the global abbreviation table have had some
    abbreviations  defined  in  them, abbrev-mode is turned on for the
    current buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (use-abbrev-table "<abbrev-table-name>")

SEE ALSO
    define-local-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    test-abbrev-expand
    write-abbrev-file
[Cuse-global-map]
COMMAND NAME
    use-global-map

DESCRIPTION
    Specify  that  the  named  keymap  will  be  used  for  the global
    interpretation of all key strokes. use-local-map is used to change
    the local interpretation of key strokes.

DEFAULT BINDING
    None.

SYNOPSIS
    (use-global-map "<keymap-name>")

SEE ALSO
    define-keymap
    use-local-map
[Cuse-local-map]
COMMAND NAME
    use-local-map

DESCRIPTION
    Specify   that   the  named  keymap  to  be  used  for  the  local
    interpretation  of  all  key  strokes.  use-global-map  is used to
    change the global interpretation of key strokes.

DEFAULT BINDING
    None.

SYNOPSIS
    (use-local-map "<keymap-name>")

SEE ALSO
    define-keymap
    use-global-map
[Cuse-old-buffer]
COMMAND NAME
    use-old-buffer

DESCRIPTION
    Prompt  for the name of a buffer and associate it with the current
    window.  The  old  buffer associated with this window merely loses
    that  association:  it  is  not  erased or changed in any way. The
    buffer must already exist.

DEFAULT BINDING
    ^X-^O

SYNOPSIS
    (use-old-buffer "<buffer-name>")

SEE ALSO
    switch-to-buffer
    pop-to-buffer
    temp-use-buffer
[Cuse-syntax-table]
COMMAND NAME
    use-syntax-table

DESCRIPTION
    Associate the named syntax table with the current buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (use-syntax-table "<syntax-table-name>")

SEE ALSO
    dump-syntax-table
    modify-syntax-entry
[Cuse-variables-of-buffer]
COMMAND NAME
    use-variables-of-buffer

DESCRIPTION
    This  function  allows  an  MLisp  procedure  to access the buffer
    specific variables of any buffer without switching to it.

    The function takes two parameters the first is a buffer name.  The
    second is an MLisp expression to execute.

    This function  is  useful  for  writing packages that wish to keep
    context  in  the  buffer  specific variables of one buffer but use
    many buffers.

    The following  example inserts the file name of buffer "fred" into
    the buffer "joe".

    (switch-to-buffer "joe")
    (use-variables-of-buffer "fred"
	(insert-string current-buffer-file-name))

DEFAULT BINDING
    None.

SYNOPSIS
    (use-variables-of-buffer <string> <s-expression>)

SEE ALSO
[Cusers-full-name]
COMMAND NAME
    users-full-name

DESCRIPTION
    An MLisp function that returns the users full name as a string. In
    VAX/VMS EMACS this is the same as a user's login name.

DEFAULT BINDING
    None.

SYNOPSIS
    (users-full-name)

SEE ALSO
    user-login-name
[Cusers-login-name]
COMMAND NAME
    users-login-name

DESCRIPTION
    An MLisp function that returns the users login name as a string.

DEFAULT BINDING
    None.

SYNOPSIS
    (users-login-name)

SEE ALSO
    users-full-name
[Cview-buffer]
COMMAND NAME
    view-buffer (package)

DESCRIPTION
    Switch  into  view-mode  and  look at the specified buffer. If the
    specified  buffer  name is blank, then look at the current buffer.
    Use  the space key to go forward a page, and the back space key to
    go  backward a page. To exit, hit ^C to go back to where you were,
    <CR>  to position dot in the viewed buffer at the current position,
    or anything else which gets executed in the original buffer.

DEFAULT BINDING
    None.

SYNOPSIS
    (view-buffer "<buffer-name>")

SEE ALSO
    view-file
[Cview-file]
COMMAND NAME
    view-file (package)

DESCRIPTION
    View  the  specified  file. The file is read into an EMACS buffer,
    which is removed upon exiting from view mode. Hit the space key to
    go  forward  a page, and the back space key to go back a page. All
    other  keys cause view mode to be left, and the original buffer to
    be displayed.

DEFAULT BINDING
    None.

SYNOPSIS
    (view-file "<file-name>")

SEE ALSO
    view-buffer
[Cvisit-file]
COMMAND NAME
    visit-file

DESCRIPTION
    Ask  for  the  name of a file and switch to a buffer that contains
    it.  The  file  name is expanded to it's full absolute form. If no
    buffer  contains  the file already then EMACS will switch to a new
    buffer and read the file into it. The name of this new buffer will
    be  just  the  file name and type. If there is already a buffer by
    that  name, and it contains some other file, EMACS will ask "Enter
    a new buffer name or <CR> to overwrite the old buffer".

DEFAULT BINDING
    ^X-^V

SYNOPSIS
    (visit-file "<file-name>")

SEE ALSO
    variable ask-about-buffer-names
[Cwait-for-process-input-request]
COMMAND NAME
    wait-for-process-input-request

DESCRIPTION
    Suspend  keyboard processing until an input request is made by the
    target  process.  Return 1 if the process wants input, or 0 if the
    user type a key at the keyboard.

    Note  that  to  force  a  wait  for a process, you must place this
    command in a loop, and throw away any input the user types.

DEFAULT BINDING
    None.

SYNOPSIS
    (wait-for-process-input-request "<process-name>")

SEE ALSO
    process-output
    send-string-to-process
[Cwhat-cursor-position]
COMMAND NAME
    what-cursor-position (package)

DESCRIPTION
    Write  out  a  brief summary of the position of dot in the current
    window.  It  reports  the  co-ordinates of dot with respect to the
    current  window,  what  the character to the right of dot is, what
    character  number dot is at, and how many percent down the file it
    is, and what portion of the file is on display in the window.

DEFAULT BINDING
    None.

SYNOPSIS
    (what-cursor-position)

SEE ALSO
[Cwhile]
COMMAND NAME
    while

DESCRIPTION
    An  MLisp  function  that executes the given expressions while the
    test is not equal to zero.

DEFAULT BINDING
    None.

SYNOPSIS
    (while <test> <expression> ...)

SEE ALSO
    if
[Cwiden-region]
COMMAND NAME
    widen-region

DESCRIPTION
    This  MLisp  function  removes  a  restriction  put on a buffer by
    narrow-region.

SYNOPSIS
    (widen-region)

SEE ALSO
    narrow-region
    save-restriction
[Cwiden-window]
COMMAND NAME
    widen-window

DESCRIPTION
    Increase the width of a vertically split window.  widen-window can
    only  succeed  if  at least one of the other windows is greater then
    one column wide.

DEFAULT BINDING
    None.

SYNOPSIS
    (widen-window)

SEE ALSO
    narrow-window
    variable window-width
[Cwindow-dimensions]
COMMAND NAME
    window-dimensions

DESCRIPTION
    This  command  returns a one dimensional array containing the
    top,  left,  bottom  and  right  coordinates  of  the current
    window.

    Index	Description
    -----	-----------
      1		Top Y coordinate
      2		Left X coordinate
      3		Bottom Y coordinate
      4		Right X coordinate

    The position    values    are    the    same   as   used   by
    goto-window-at-x-y  and returned in control-string-parameters
    for a mouse click.

DEFAULT BINDING
    none.

SYNOPSIS
    (window-dimensions)

EXAMPLE

SEE ALSO

[Cwrite-abbrev-file]
COMMAND NAME
    write-abbrev-file

DESCRIPTION
    Write  all  defined  abbreviations  to  a named file. This file is
    suitable for reading back with read-abbrev-file.

DEFAULT BINDING
    None.

SYNOPSIS
    (write-abbrev-file "<file-name>")

SEE ALSO
    define-local-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    use-abbrev-table
[Cwrite-current-file]
COMMAND NAME
    write-current-file

DESCRIPTION
    Write the contents of the current buffer to the file whose name is
    associated with the buffer.

DEFAULT BINDING
    ^X-^^-S or ^X^S

SYNOPSIS
    (write-current-file "<file-name>")

SEE ALSO
    append-to-file
    write-file-exit
    write-modified-files
    write-named-file
[Cwrite-file-exit]
COMMAND NAME
    write-file-exit

DESCRIPTION
    Write  all  modified  buffers to their associated files and if all
    goes well, EMACS will exit.

DEFAULT BINDING
    ^X-^F

SYNOPSIS
    (write-file-exit)

SEE ALSO
    append-to-file
    write-current-file
    write-modified-files
    write-named-file
[Cwrite-modified-files]
COMMAND NAME
    write-modified-files

DESCRIPTION
    Write  each modified buffer into the file whose name is associated
    with the buffer. EMACS will complain if a modified buffer does not
    have an associated file.

DEFAULT BINDING
    ^X-^M

SYNOPSIS
    (write-modified-files)

SEE ALSO
    append-to-file
    write-current-file
    write-file-exit
    write-named-file
[Cwrite-named-file]
COMMAND NAME
    write-named-file

DESCRIPTION
    Prompt for a filename and write the contents of the current buffer
    to the named file.

DEFAULT BINDING
    ^X-^W

SYNOPSIS
    (write-named-file "<file-name>")

SEE ALSO
    append-to-file
    write-current-file
    write-file-exit
    write-modified-files
[Cyank-buffer]
COMMAND NAME
    yank-buffer

DESCRIPTION
    Take  the  contents  of  the buffer whose name is prompted for and
    insert  it  at  dot  in  the current buffer. Dot is left after the
    inserted text.

DEFAULT BINDING
    ESC-Y

SYNOPSIS
    (yank-buffer "<buffer-name>")

SEE ALSO
    yank-from-killbuffer
[Cyank-from-killbuffer]
COMMAND NAME
    yank-from-killbuffer

DESCRIPTION
    Take  the  contents of the kill buffer and insert it at dot in the
    current buffer. Dot is left after the inserted text.

DEFAULT BINDING
    ^Y

SYNOPSIS
    (yank-from-killbuffer)

SEE ALSO
    yank-buffer
[C|]
COMMAND NAME
    |

DESCRIPTION
    An  MLisp  function  that  returns  the  result  of  oring all its
    arguments together.

DEFAULT BINDING
    None.

SYNOPSIS
    (| <expression-1> <expression-2>)
    (| 5 4 128) => 133
[VCPU-type]
VARIABLE NAME
    CPU-type (Unix)

DESCRIPTION
    Contains  a string decribing the type of CPU emacs is running
    on.

    The value will be one of:

	AXP	Digital Alpha processor
	i386	Intel x86 processor
	pa_risc	HP PA Risc processor
	m68k	Motorola 68000 family processor
	r6000	IBM Risc processor

DEFAULT VALUE
    Depends on host systems CPU.

[VDCL-buffer-reduction]
VARIABLE NAME
    DCL-buffer-reduction

DESCRIPTION
    This  variable controls how many characters are deleted from a DCL
    buffer   after   it  contains  more  than  maximum-DCL-buffer-size
    characters.

DEFAULT VALUE
    500

SEE ALSO
    maximum-DCL-buffer-size
[VUI-filter-file-list]
VARIABLE NAME
    UI-filter-file-list

DESCRIPTION
    This  variable  provides the data for the "List File of Type" part
    of the Windows File Open and File Save As dialog boxes.

    The value  is a set of pairs of text lines.  The first line of the
    pair is a description and the second line of the pair is a list of
    file filters.

    For example to have filters for text files (*.txt) and C file (*.c
    and *.h):

    (setq UI-filter-file-list "Text files\n*.txt\nC files\n*.c;*.h")

DEFAULT
    "C/C++ Source code\n"
    "*.c;*.h;*.cpp;*.rc;*.def\n"
    "MLisp source code\n"
    "*.ml;*.mlp,*.key\n"
    "Text Files\n"
    "*.txt\n"
    "Ini Files\n"
    "*.ini;*.sys;*.bat;*.btm\n"
    "All Files (*.*)\n"
    "*.*\n"

SEE ALSO
    command UI-open-file
[VUI-open-file-name]
VARIABLE NAME
    UI-open-file-name

DESCRIPTION

    This variable  provides  the  file  name for the Windows File Open
    dialog boxes.

DEFAULT

SEE ALSO
    command UI-open-file
[VUI-open-file-readonly]
VARIABLE NAME
    UI-open-file-readonly

DESCRIPTION

    This variable  provides the setting of the Read only check box for
    the Windows File Open dialog boxes.

DEFAULT
    0

SEE ALSO
    command UI-open-file
[VUI-save-as-file-name]
VARIABLE NAME
    UI-save-as-file-name

DESCRIPTION

    This variable provides the file name for the Windows File Save As
    dialog box.


DEFAULT
    ""

SEE ALSO
    UI-file-save-as
[VUI-search-string]
VARIABLE NAME
    UI-search-string

DESCRIPTION

    This variable  provides  the  initial  "Find  what" string for the
    Windows Find and Find and Replace dialog boxes.

    When the  dialog  box is close UI-search-string will hold the last
    value of the "Find what" field.

DEFAULT
    ""

SEE ALSO
    command UI-find
    command UI-find-and-replace
    UI-replace-string
[VUI-replace-string]
VARIABLE NAME
    UI-replace-string

DESCRIPTION

    This variable  provides  the initial "Replace with" string for the
    Windows Find and Replace dialog box.

    When the  dialog box is close UI-replace-string will hold the last
    value of the "Replace with" field.

DEFAULT

SEE ALSO
    command UI-find
    command UI-find-and-replace
    UI-search-string
[Vabbrev-expansion]
VARIABLE NAME
    abbrev-expansion

DESCRIPTION
    This  string variable holds the expansion of the abbreviation when
    an  abbreviation  hook  function  is called. At all other times it
    contains a null string.

DEFAULT VALUE
    None.

SEE ALSO
    abbrev-mode
    Function define-hooked-local-abbrev
    Function define-hooked-global-abbrev
[Vabbrev-mode]
VARIABLE NAME
    abbrev-mode

DESCRIPTION
    This variable  controls whether abbreviations are expended. If set
    to  none  zero,  then  abbreviations  are  checked and expanded as
    required.  Otherwise,  they  are  not.  This  variable  is  buffer
    specific  and  its  state  is  displayed in the mode line for each
    window.

DEFAULT VALUE
    OFF

SEE ALSO
    define-global-abbrev
    define-local-abbrev
    quietly-read-abbrev-file
    read-abbrev-file
    test-abbrev-expand
    use-abbrev-table
    write-abbrev-file
[Vactivity-indicator]
VARIABLE NAME
    activity-indicator

DESCRIPTION

    Set  this  variable  to  get  the activity indicator displayed. The
    activity indicator is a single letter code that is displayed at the
    bottom  left  hand  corner of the screen. Currently there are three
    characters  defined. "i" indicates that EMACS is waiting for input,
    "b"  indicates that EMACS is busy working and "c" is displayed when
    EMACS is checkpointing.

    To  make  room  for  the activity indicator the minibuffer is moved
    over two columns to the right.

DEFAULT VALUE
    OFF

SEE ALSO
[Vanimated-journal-recovery]
VARIABLE NAME
    animated-journal-recovery    

DESCRIPTION
    When  set  to  true  the  journal-recover  command will update the
    screen   while   recovering   the   contents   of  a  buffer.   If
    animated-journal-recovery is false the screen updates after all of
    the journal has been used to recover the buffer.

DEFAULT VALUE
    Off

SEE ALSO
[Vask-about-buffer-names]
VARIABLE NAME
    ask-about-buffer-names

DESCRIPTION
    The  ask-about-buffer-names  variable controls what the visit-file
    function  does if the buffer name it constructs is already in use.

    If  ask-about-buffer-names  is none zero then EMACS will ask for a
    new  buffer  name  to be given, or for <CR> to be typed which will
    overwrite the old buffer.

    If  it is zero then a buffer name will be synthesized by appending
    "<n>" to the buffer name, for a unique value of n.

    For  example, if I visit-file "makefile" then the buffer name will
    be  "makefile";  then  if  I visit-file "[man]makefile" the buffer
    name will be "makefile<2>".

DEFAULT VALUE
    ON

SEE ALSO
    visit-file
[Vask-about-synchronise-for-none-modified-buffers]
VARIABLE NAME
    ask-about-synchronise-for-none-modified-buffers

DESCRIPTION

    Normally the  synchronise-files  commands  always asks the user to
    confirm any change to a buffer.
	
    If ask-about-synchronise-for-none-modified-buffers   is   1   then
    synchronise-files  will not ask for permission to delete or reread
    the contents of buffers that have not been modified.

DEFAULT VALUE
    0

SEE ALSO
    synchronise-files
[Vauto-fill-hook]
VARIABLE NAME
    auto-fill-hook

DESCRIPTION
    This  variable  controls  the  calling  of  a function to fill the
    current  line.  The variable is set to the name of the function to
    be  called  to fill the line, or the null string if no function is
    to be called.

    This hook is useful for implementing text-mode type functions.

DEFAULT VALUE
    default-auto-fill-hook

SEE ALSO
    default-auto-fill-hook
    right-margin
    left-margin
[Vautomatic-horizontal-scrolling]
VARIABLE NAME
    automatic-horizontal-scrolling

DESCRIPTION

    When automatic-horizontal-scrolling  is  set  to 1 emacs will
    automatic  scroll  the  current  window until dot is visible.
    Otherwise  emacs will allow dot to be horizontally outside of
    the window.

DEFAULT
    0

SEE ALSO
    horizontal-scroll-step

[Vbackup-file-mode]
VARIABLE NAME
    backup-file-mode

DESCRIPTION

    The value  of  this  variable  controls  the  way  that Emacs
    creates backup files.

	Value		Description
	-----		-----------
	"none"		Do not create backup files
	"copy"		Create backup file by copying
	"rename"	Create backup file by renaming

    Use backup  by  "copy"  on  Unix  systems  to  leave the file
    protection and symbolic links unchanged.

    Use backup by "rename" on Windows to speed up file saving.

    Use backup "none" to prevent backup files being created.

DEFAULT VALUE
    "none" on VMS
    "copy" on Unix
    "copy" on Windows

SEE ALSO
    backup-filename-format

[Vbackup-filename-format]
VARIABLE NAME
    backup-filename-format

DESCRIPTION

    This variable  controls  how emacs names backup files.  Emacs
    bases  the  backup  filename  on  the  buffers  file  name as
    directed by the format string.

    The syntax  of  the  format string is the same as used by the
    command file-format-string. The equivilent MLisp would be:

    (setq backup-filename
	(file-format-string
	    backup-filename-format 
	    current-buffer-file-name))

    The default  format  create backup files in the same director
    as  the  original file but with a leading "_" prefixed to the
    file  type.   For  example c:\docs\readme.txt is backed up as
    c:\docs\readme._txt.


DEFAULT VALUE
    %pa%fn._%1ft

SEE ALSO
    backup-file-mode
    command file-format-string

[Vblack-on-white]
VARIABLE NAME
    black-on-white

DESCRIPTION
    If  none  zero,  then  text  is  displayed  on the screen as black
    characters on a white background. This information is used by some
    terminal interfaces to decide how to display a visible bell.

DEFAULT VALUE
    OFF

SEE ALSO
    illegal-operation
    interrupt-key
[Vbreakpoint-hook]
VARIABLE NAME
    breakpoint-hook

DESCRIPTION
    This variable contains the name of the breakpoint function. When a
    breakpoint  occurs,  the function whose name is in breakpoint-hook
    is called.

    The  breakpoint function may decompile the expression that will be
    executed by using the decompile-current-line function.

DEFAULT VALUE
    ""

SEE ALSO
    trace-mode
    trace-hook
    current-breakpoint
    function breakpoint
    function list-breakpoints
[Vbuffer-is-modified]
VARIABLE NAME
    buffer-is-modified

DESCRIPTION
    buffer-is-modified  is  positive  if  the  current buffer has been
    modified  since it was last written out to disk. You may set it to
    0  if  you  want  EMACS to ignore the modifications that have been
    made  to  this  buffer. However, this does not get you back to the
    unmodified  version,  it just tells EMACS not to write it out with
    the  other  modified files. EMACS sets buffer-is-modified positive
    any time the buffer is modified.

DEFAULT VALUE
    0

SEE ALSO
    write-modified-files
    write-files-exit
[Vbuffer-names]
VARIABLE NAME
    buffer-names

DESCRIPTION
    This  variables  is  a  1 dimensional array which contains all the
    buffer names currently in use in EMACS. Element 0 of the array is a
    count  of  the  total  number  of  buffers contained in the array.
    Elements 1 to n are the names of the buffers, where n is the total
    number of buffers in use in EMACS.

EXAMPLE
    List all buffers in use in Emacs

(defun
    (list-buffers
    i
    (pop-to-buffer "Buffer list")
    (erase-buffer)
    (insert-string "Buffer\n------\n")
    (setq i 1)
    (while (< i (fetch-array buffer-names 0))
    (insert-string
    (concat
    (fetch-array buffer-names i)
    "\n"
)
		)
		(setq i (+ i 1))
	    )
	    (beginning-of-file)
	)
    )

DEFAULT VALUE
    n/a
SEE ALSO
[Vcase-fold-search]
VARIABLE NAME
    case-fold-search

DESCRIPTION
    If  none  zero,  all  searches  will ignore the case of alphabetic
    characters when doing comparisons.

DEFAULT VALUE
    OFF

SEE ALSO
    re-search-forward
    re-search-reverse
    search-forward
    search-reverse
    query-replace-string
    re-query-replace-string
    re-replace-string
    replace-string
    c=
[Vcheckpoint-frequency]
VARIABLE NAME
    checkpoint-frequency

DESCRIPTION
    The    number    of    keystrokes   between   checkpoints.   Every
    checkpoint-frequency   keystrokes  all  buffers  which  have  been
    modified   since   they  were  last  checkpointed  and  that  have
    checkpointing enabled are written to disk.

    The  checkpoint  file  name  is  the file name associated with the
    buffer,  or  if  that is null, the name of the buffer but with the
    extension changed to ".CKP".

    If the value is zero, then checkpointing is disabled.

DEFAULT VALUE
    OFF

SEE ALSO
    Function checkpoint
[Vcheckpoint-hook]
VARIABLE NAME
    checkpoint-hook

DESCRIPTION
    This variable contains the name of an EMACS function which will be
    called  when  the checkpoint function  is invoked.  The checkpoint
    function should, in some way, checkpoint EMACS.

DEFAULT VALUE
    checkpoint-buffers

SEE ALSO
    Function checkpoint-buffers
    Function checkpoint
    checkpoint-frequency
[Vcli-name]
VARIABLE NAME
    cli-name

DESCRIPTION
    A  string  variable  that  indicates what CLI caused EMACS to run.
    The  contents of this variable is used to tell sub-processes which
    CLI  to use. You can alter its value to start a sub-process with a
    different CLI

DEFAULT VALUE
   The name of the CLI that started EMACS

SEE ALSO
    command start-process
    command start-DCL-process
    command execute-monitor-command
    command return-to-monitor
    command compile-it
    command filter-region
[Vcomment-column]
VARIABLE NAME
    comment-column

DESCRIPTION
    The column at which program comments are to start. This is used by
    the  language-dependent  commenting  features through the move-to-
    comment-column command.

DEFAULT VALUE
    33

SEE ALSO
    command move-to-comment-column
[Vcompile-it-command]
VARIABLE NAME
    compile-it-command

DESCRIPTION
    This  variable  contains  the  string  which  will  be used by the
    compile-it command  if  you type a null string to its prompt for a
    command  to  execute.  Also,  when  you type a command, compile-it
    saves the command you type in the variable for later re-use.

DEFAULT VALUE
    ""

SEE ALSO
    compile-it command
[Vconfirm-expansion-choice]
VARIABLE NAME
    confirm-expansion-choice

DESCRIPTION

    This variable  changes  the  way that Emacs input completion logic
    works. 

    When confirm-expansion-choice   is  true  emacs  will  always
    prompt  once  more after expanding a unique input choice.  It
    confirm-expansion-choice  is false emacs will take the unique
    choice and use it with out further prompts.

DEFAULT VALUE
    OFF

SEE ALSO
    expansion-only-completes-unique-choices

[Vcontrol-string-convert-f-keys]
VARIABLE NAME
    control-string-convert-F-keys

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    When control-string-convert-F-keys is ON the keys F6 to F20 and E1
    to  E6  are  converted  on  input to a compact form.  This has the
    effect  of  improving Emacs performance in a number of ways.  There
    is  less memory used to store keymaps, keyboard handling uses less
    CPU time.

    Before conversion the control sequence is

	CSI number ~

    This is  converted  to CSI followed by one key whose value is 32 +
    the number from above.

DEFAULT VALUE
    ON

SEE ALSO
    convert-key-string
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-convert-mouse]
VARIABLE NAME
    control-string-convert-mouse

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    When control-string-convert-mouse  is  ON all control sequences of
    the form

	CSI event;params... &w

    are converted  into  \201  followed  by one character which is the
    event number plus 'A'.

DEFAULT VALUE
    ON

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-convert-to-8-bit]
VARIABLE NAME
    control-string-convert-to-8-bit

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    All control  sequences  which  start  with  ESC <char> have a 8 bit
    equivalent     control     sequence.     The    string    variable
    control-string-convert-to-8-bit contains all the <char>'s that are
    to converted to there 8 bit form.

DEFAULT VALUE
    "[OP:"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-convert-to-CSI]
VARIABLE NAME
    control-string-convert-to-CSI

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    The string  variable control-string-convert-to-CSI contains a list
    of  characters  to  be coerced into the single code for CSI.  This
    has  the  effect of reducing the number of keymaps that Emacs must
    maintain.

DEFAULT VALUE
    "\217\220\232"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-final-characters]
VARIABLE NAME
    control-string-final-characters

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    This string variable contains all the characters that mark the end
    of a control sequence. See ISO 2020 for full details.

DEFAULT VALUE
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-intermediate-characters]
VARIABLE NAME
    control-string-intermediate-characters

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    This string  variable  contains all the characters that follow the
    parameters  and precede the final character of a control sequence.
    See ISO 2020 for full details.

DEFAULT VALUE
    " !"#$&'()*+,-./"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-parameters]
VARIABLE NAME
    control-string-parameters

DESCRIPTION
    This  two  dimensional  array variable contains the parameters and
    parameter  separators of the last control sequence that was parsed
    by Emacs. All elements of the array are strings.

    Row 1 of the array contains the parameter value and Row 2 contains
    the value of the separator.  The first parameter is in column 0 of
    the array the second in column 1 and so on.

    Use bounds-of-array  to  find  out  how  many  parameters  are  in
    control-string-parameters.

    This is a read only variable.

    Also see control-string-processing for general information.

DEFAULT VALUE
    Empty Array

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-parameter-characters]
VARIABLE NAME
    control-string-parameter-characters

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    This string  variable  contains  all  the  characters  that can be
    parameter characters.  See ISO 2020 for full details.

DEFAULT VALUE
    "0123456789:<=>?"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-separators
    variable control-string-processing 
[Vcontrol-string-parameter-separators]
VARIABLE NAME
    control-string-parameter-separators

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    Also see control-string-processing for general information.

    This string  variable  contains  all  the  characters  that can be
    parameter separator characters.  See ISO 2020 for full details.

DEFAULT VALUE
    ";"

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-processing 
[Vcontrol-string-processing]
VARIABLE NAME
    control-string-processing

DESCRIPTION
    WARNING.  The  value  of this variable must only be changed in the
    file   EMACS_CONTROL_STRINGS.ML.   If  you  wish  to  change  this
    parameter make a copy of EMACS$LIBRARY:EMACS_CONTROL_STRINGS.ML in
    your  EMACS$PATH:.   Then  rebuild  your  Emacs  environment  with
    EMACS$LIBRARY:EMACS_SAVE_ENVIRONMENT.COM.

    This variable  is the master control for all of the control string
    processing.   Only  when  control-string-processing is ON will any
    processing be performed.

    control-string-processing  changes  the  behaviour  of a number of
    parts of Emacs.

    1.  All  key  strokes  are compressed and process according to the
	control-string variables.

    2.  bind-to-key,  local-bind-to-key  will  use  the control-string
	variables to compress its <keys> key sequence.

    3.  describe-bindings,  describe-key, apropos will use the builtin
	key names table to print names of keys.

DEFAULT VALUE
    ON

SEE ALSO
    convert-key-string
    variable control-string-convert-F-keys
    variable control-string-convert-mouse
    variable control-string-convert-to-8-bit
    variable control-string-convert-to-CSI
    variable control-string-final-characters
    variable control-string-intermediate-characters
    variable control-string-parameters
    variable control-string-parameter-characters
    variable control-string-parameter-separators
[Vcurrent-breakpoint]
VARIABLE NAME
    current-breakpoint

DESCRIPTION
    This  read-only  variable  contains  the name of the function that
    caused  the  breakpoint  function  to  run.  This  variable is not
    altered from Mock Lisp in a trace or breakpoint function.

DEFAULT VALUE
    ""

SEE ALSO
    current-function
    trace-hook
    breakpoint-hook
    trace-mode
[Vcurrent-buffer-RMS-record-attribute]
VARIABLE NAME
    current-buffer-RMS-record-attribute

DESCRIPTION
    Contains the  RMS record attribute for the file that is associated
    with      the      current      buffer.       You      can     set
    current-buffer-RMS-record-attribute  to  the record attribute that
    you wish the file to be written out with.

    current-buffer-RMS-record-attribute can be set to one of

    Value			Description
    -----			-----------
    "none"			Not a file buffer
    "fixed"			Image data
    "variable"			Normal VMS file
    "variable-fixed-control"	DCL created file
    "stream"			MS-DOS file
    "stream-lf"			Unix file
    "stream-cr"			Macintosh file

DEFAULT
    Set from the file that is read in.

SEE ALSO
    default-buffer-RMS-record-attribute
    override-RMS-record-attribute
[Vcurrent-buffer-abbrev-table]
VARIABLE NAME
    current-buffer-abbrev-table

DESCRIPTION

    This variable   contains   the   name   of  the  current  buffer's
    abbreviation  table.  If you attempt to assign a value to it, then
    the  current  buffer's abbreviation table is changed also, and the
    name you type must be the name of an already existing abbreviation
    table.

DEFAULT VALUE
    None.

SEE ALSO
    use-abbrev-table
[Vcurrent-buffer-allocation-size]
VARIABLE NAME
    current-buffer-allocation-size

DESCRIPTION
    When  read current-buffer-allocation-size returns the size in
    characters allocated for use in the current buffer.

    Assigning a        new,        larger,        value        to
    current-buffer-allocation-size  will increase the capacity of
    the current buffer.

    It is  not  normally nessesary to use this variable.  However
    as a performance enhancement it is sometimes useful.

    If you  plan  to  perform  a  complex  set of operations on a
    buffer.   These  operations  will  cause the buffer to slowly
    grow    to    a    much    larger    size.     Assigning   to
    current-buffer-allocation-size  a  suitable  large value will
    reduce the CPU overhead of expanding the buffer is many small
    increments.

DEFAULT VALUE
    None.

SEE ALSO
[Vcurrent-buffer-checkpointable]
VARIABLE NAME
    current-buffer-checkpointable

DESCRIPTION
    This  variable  contains the state of the checkpointability of the
    current  buffer. If none zero, then when a checkpoint occurs, this
    buffer  will  be checkpointed if modifications dictate. Otherwise,
    the buffer will never be checkpointed.

    A  buffer  can never be checkpointed if checkpoint-frequency is set
    to zero.

DEFAULT VALUE
    Depends  on  checkpoint-frequency  when the buffer was created. If
    checkpoint-frequency  is  none  zero,  then  the  default for this
    variable is ON. Otherwise it if OFF.

SEE ALSO
    checkpoint-frequency
    checkpoint command
[Vcurrent-buffer-file-name]
VARIABLE NAME
    current-buffer-file-name

DESCRIPTION
    This  variable  contains  the  fully expanded file name associated
    with  the current buffer. If the file does not already exist, then
    the generation number will be omitted.

    Setting  this  variable  sets the filename to which this buffer is
    connected,  but  without  writing  the  file.  Also, the buffer is
    forced to be a File buffer.

    If  this  buffer  is  not  a  File  buffer,  then a null string is
    returned.

DEFAULT VALUE
    File name of file read into buffer or last written out.

SEE ALSO
[Vcurrent-buffer-journalled]
VARIABLE NAME
    current-buffer-journalled

DESCRIPTION

    This buffer  specific  variable  is ON if the buffer is enabled to
    journal modification.

DEFAULT VALUE
    If journal-frequency is 0 then OFF else ON.

SEE ALSO
    journal-frequency
[Vcurrent-buffer-macro-name]
VARIABLE NAME
    current-buffer-macro-name

DESCRIPTION
    This  variable contains the macro name associated with the current
    buffer.

    Setting  this  variable  sets  the  macro  to which this buffer is
    connected. The buffer will be forced to be a Macro buffer.

    Read  this  variable  to  find  out the name of the current buffer
    macro's name.

    If  this  buffer  is  not  a  Macro buffer,  then a null string is
    returned.

DEFAULT VALUE
    ""

SEE ALSO
[Vcurrent-buffer-name]
VARIABLE NAME
    current-buffer-name

DESCRIPTION
    This  variable contains the name of the currently selected buffer.
    You  can  change the name of the current buffer by assigning a new
    name  to the variable. The new buffer name must not exist already,
    or an error is displayed.

DEFAULT VALUE
    The name of the current buffer.

SEE ALSO
[Vcurrent-buffer-syntax-table]
VARIABLE NAME
    current-buffer-syntax-table

DESCRIPTION
    This  variable  contains  the  name of the current buffer's syntax
    table.  Assigning  to  this  variable changes the current buffer's
    syntax table. The specified syntax table must exist already.

DEFAULT VALUE
    ""

SEE ALSO
    use-syntax-table
[Vcurrent-buffer-type]
VARIABLE NAME
    current-buffer-type

DESCRIPTION
    This variable contains a string describing the type of the current
    buffer. It will be one of the following strings:

	"file" for File buffers

	"scratch" for Scratch buffers

	"macro" for Macro buffers

    To  change  the type of a buffer, assign the appropriate string to
    this variable in the buffer which you want to change.

DEFAULT VALUE
    None.

SEE ALSO
[Vcurrent-function]
VARIABLE NAME
    current-function

DESCRIPTION
    This  read-only  variable  contains  the  name  of  the  currently
    executing  Mock  Lisp  function.  The  value  is  not  changed for
    functions executed in a trace or breakpoint function.

DEFAULT VALUE
    ""

SEE ALSO
    current-breakpoint
    trace-hook
    breakpoint-hook
    trace-mode
[Vcurrent-local-keymap]
VARIABLE NAME
    current-local-keymap

DESCRIPTION
    This  variable  contains  the  name  of the current buffer's local
    keymap  is set up by use-local-map. If you assign the variable, it
    has  the  same  effect  as using use-local-map. An existing keymap
    MUST be specified.

DEFAULT VALUE
    ""

SEE ALSO
    use-local-map
[Vcurrent-windows]
VARIABLE NAME
    current-windows

DESCRIPTION
    This  variable contains the set of windows that are on the screen.
    If  read  current-windows  returns  a  value  that  represents the
    windows  on  the screen. When current-windows is set the screen is
    restored   from   the   windows   expression   that  is  set  into
    current-windows.

EXAMPLE
    Simple windows save and restore facility.

    (defun
	(save-windows
	    (setq saved-windows current-windows)
	)
	(restore-windows
	    (setq current-windows saved-windows)
	)
    )


DEFAULT VALUE
    current windows on the screen

SEE ALSO
    package windows
[Vctlchar-with-^]
VARIABLE NAME
    ctlchar-with-^

DESCRIPTION
    If  none  zero,  control  characters  are  printed  as  ^C (an '^'
    character  followed  by the upper case alphabetic that corresponds
    to  the  control  character),  otherwise they are printed as a '\'
    followed by a three digit octal number.

DEFAULT VALUE
    OFF

SEE ALSO
[Vdebug-it-command]
VARIABLE NAME
    debug-it-command

DESCRIPTION
    Used to hold a command that can be used to debug the program built
    with the compile-it-command

DEFAULT

SEE ALSO
    compile-it-command

[Vdefault-auto-fill-hook]
VARIABLE NAME
    default-auto-fill-hook

DESCRIPTION
    This variable   controls   is   used  to  default  a  new  buffers
    auto-fill-hook.

DEFAULT VALUE
    ""

SEE ALSO
    auto-fill-hook
    right-margin
    left-margin
[Vdefault-buffer-RMS-record-attribute]
VARIABLE NAME
    default-buffer-RMS-record-attribute

DESCRIPTION
    Provide  the  default RMS record attribute for buffer that has not
    been read in from a file.

    default-buffer-RMS-record-attribute can be set to one of

    Value			Description
    -----			-----------
    "none"			Not a file buffer
    "fixed"			Image data
    "variable"			Normal VMS file
    "variable-fixed-control"	DCL created file
    "stream"			MS-DOS file
    "stream-lf"			Unix file
    "stream-cr"			Macintosh file

DEFAULT
    "none"

SEE ALSO
    current-buffer-RMS-record-attribute
    override-RMS-record-attribute

[Vdefault-case-fold-search]
VARIABLE NAME
    default-case-fold-search

DESCRIPTION
    Provide the  default  value  used  to  initialise case-fold-search
    whenever a new buffer is created.

DEFAULT VALUE
    OFF

SEE ALSO
    case-fold-search
[Vdefault-comment-column]
VARIABLE NAME
    default-comment-column

DESCRIPTION
    Provide  the  default  value  used  to  initialise  comment-column
    whenever a new buffer is created.

DEFAULT VALUE
    33

SEE ALSO
    comment-column
[Vdefault-display-end-of-file]
VARIABLE NAME
    default-display-end-of-file

DESCRIPTION
    Provide  the  default  value used to initialise display-end-of-file
    whenever a new buffer is created.

DEFAULT VALUE
    0

SEE ALSO
    display-end-of-file
[Vdefault-display-c1-as-graphics]
VARIABLE NAME
    default-display-C1-as-graphics

DESCRIPTION
    Provide     the     default     value     used    to    initialise
    default-display-C1-as-graphics whenever a new buffer is created.

DEFAULT VALUE
    0

SEE ALSO
    display-C1-as-graphics
[Vdefault-display-non-printing-characters]
VARIABLE NAME
    default-display-non-printing-characters

DESCRIPTION
    Provide     the     default     value     used    to    initialise
    display-non-printing-characters whenever a new buffer is created.

DEFAULT VALUE
    0

SEE ALSO
    display-non-printing-characters
[Vdefault-highlight-region]
VARIABLE NAME
    default-highlight-region

DESCRIPTION
    Provide  the  default  value  used  to  initialise highlight-region
    whenever a new buffer is created.

DEFAULT VALUE
    0

SEE ALSO
    highlight-region
    highlight-region-graphic-rendition
[Vdefault-left-margin]
VARIABLE NAME
    default-left-margin

DESCRIPTION
    Provide  the default value used to initialise left-margin whenever
    a new buffer is created.

DEFAULT VALUE
    1

SEE ALSO
    left-margin
[Vdefault-mode-line-format]
VARIABLE NAME
    default-mode-line-format

DESCRIPTION
    Provide   a   default   mode   line   format  used  to  initialise
    mode-line-format whenever a buffer is created.

DEFAULT VALUE
    " %[Buffer: %b%*  File: %f  %M (%m%c%r%a)  %p%]"

SEE ALSO
    mode-line-format
[Vdefault-syntax-array]
VARIABLE NAME
    default-syntax-array

DESCRIPTION
    This  variable  provides  the default value of syntax-array in all
    new buffers that are created.

DEFAULT
    0

SEE ALSO
    syntax-array
[Vdefault-syntax-colouring]
VARIABLE NAME
    default-syntax-colouring

DESCRIPTION
    This  variable  provides  the default value of syntax-colouring in
    all new buffers that are created.

DEFAULT
    0

SEE ALSO
    syntax-colouring
    syntax-array
[Vdefault-read-only-buffer]
VARIABLE NAME
    default-read-only-buffer

DESCRIPTION
    Provide  the  default  value  used  to initialise read-only-buffer
    whenever a new buffer is created.

DEFAULT VALUE
    0

SEE ALSO
    read-only-buffer
[Vdefault-replace-mode]
VARIABLE NAME
    default-replace-mode

DESCRIPTION
    Provide the default value used to initialise replace-mode whenever
    a new buffer is created.

DEFAULT VALUE
    10000

SEE ALSO
    replace-mode
[Vdefault-right-margin]
VARIABLE NAME
    default-right-margin

DESCRIPTION
    Provide the default value used to initialise right-margin whenever
    a new buffer is created.

DEFAULT VALUE
    10000

SEE ALSO
    right-margin
[Vdefault-tab-size]
VARIABLE NAME
    default-tab-size

DESCRIPTION
    Provide  a  default  value  used to initialise tab-size whenever a
    buffer is created.

DEFAULT VALUE
    8

SEE ALSO
    tab-size
    logical-tab-size
[Vdefault-wrap-long-lines]
VARIABLE NAME
    default-wrap-long-lines

DESCRIPTION
    Provides  the  default  value  of  a  new  buffers wrap-long-lines
    variable.

DEFAULT
    0

SEE ALSO
    wrap-long-lines
[Vdisplay-c1-as-graphics]
VARIABLE NAME
    display-C1-as-graphics

DESCRIPTION
    Setting  this  variable TRUE, for a buffer, tells EMACS to display
    all  the characters in the range 128 to 159 as the special graphic
    characters of the VTxxx terminal.

DEFAULT VALUE
    0

SEE ALSO
    default-display-C1-as-graphics
[Vdisplay-end-of-file]
VARIABLE NAME
    display-end-of-file

DESCRIPTION
    Setting  this  variable TRUE, for a buffer, tells EMACS to display
    a diamond graphic character at the end of the buffer.

DEFAULT VALUE
    0

SEE ALSO
    default-display-end-of-file
[Vdisplay-non-printing-characters]
VARIABLE NAME
    display-non-printing-characters

DESCRIPTION
    Setting  this  variable TRUE, for a buffer, tells EMACS to display
    non printing characters. The non printing characters that are
    displayed are <LF>, <TAB> and an end of buffer marker.

    <LF>  is displayed as the N/L graphic character and will appear at
    the end of ever line.

    <TAB>  is  displayed as the H/T graphic character. The rest of the
    white  space  taken  up  by the <TAB> is displayed as centred dot
    characters.

    The  end  of  buffer  position  is  displayed as a diamond graphic
    character.

DEFAULT VALUE
    0

SEE ALSO
    default-display-non-printing-characters
[Velapse-time]
VARIABLE NAME
    elapse-time

DESCRIPTION

    This read-only  variable  contains the number of milliseconds that
    have elapsed since emacs started.

    elapse-time was  added  to  emacs  to  allow GUI code to calculate
    intervals of time as requested for mouse double click detection.

    elapse-time's value will overflow after Emacs has been running for
    24 days.

VALUE
    The number of milli seconds since emacs started

SEE ALSO
[Venter-emacs-hook]
VARIABLE NAME
    enter-emacs-hook

DESCRIPTION

    This variable contains the name of an EMACS function which will be
    called when EMACS initially starts up. You can assign the variable
    in  your  emacsinit.ml  file  to  contain  the name of any already
    existing  function.  (You can also assign it anywhere else, to the
    function cannot ever be called!)

DEFAULT VALUE
    ""

SEE ALSO
    leave-emacs-hook
    return-to-emacs-hook
    exit-emacs-hook
[Verror-file-name]
VARIABLE NAME
    error-file-name

DESCRIPTION
    A  string  variable used by error message parser (as specified in
    the variable error-message-parser) to indicate to the parse-error-
    message-in-region  function  in  what  file  the  current error is
    located.  If  an  error  message parser sets this to a zero length
    string, then error message parsing is stopped.

DEFAULT VALUE
    ""

SEE ALSO
    error-line-number
    error-message-parser
    error-start-position
    Command parse-error-message-in-region
    Command compile-it
[Verror-hook]
VARIABLE NAME
    error-hook

DESCRIPTION
    A  String variable which defines the error function. When an error
    occurs  that  would  cause  an  error message to be displayed, the
    function  whose  name is contained in this variable is called. The
    variable error-message will contain the text of the error.

    When the error function exits, the error message will be displayed
    in  the  usual  way  unless  exit-emacs  is used to exit the error
    function.

DEFAULT VALUE
    ""

SEE ALSO
[Verror-line-number]
VARIABLE NAME
    error-line-number

DESCRIPTION
    A  numeric variable used by error message parser (as specified in
    the variable error-message-parser) to indicate to the parse-error-
    message-in-region  function  on  which  line  the current error is
    located.  If an error message parser sets this to zero, then error
    message parsing is stopped.

DEFAULT VALUE
    0

SEE ALSO
    error-file-name
    error-message-parser
    error-start-position
    Command parse-error-message-in-region
    Command compile-it
[Verror-message]
VARIABLE NAME
    error-message

DESCRIPTION
    This  read only  string  variable  holds the text of the last error
    message  that EMACS or error-message generated. This variable will
    always  hold  the  last  error message, even if the error occurred
    inside an error-occurred block.

DEFAULT VALUE
    ""

SEE ALSO
[Verror-message-parser]
VARIABLE NAME
    error-file-name

DESCRIPTION
    A  string variable used to specify the name of a function that the
    parse-error-message-in-region  function  should call to locate the
    next error.

    The  error parser function should locate the error message, moving
    dot   to   its   end,  and  fill  the  variables  error-file-name,
    error-line-number    and    error-start-position   with   suitable
    information.  Parsing is stopped if any of these variables are set
    to zero or a null length string.

DEFAULT VALUE
    "error-message-parser"

SEE ALSO
    error-line-number
    error-start-position
    Command parse-error-message-in-region
    Command error-message-parser
    Command compile-it
[Verror-message-format]
VARIABLE NAME
    error-message-format

    This  variable is obsolete, and will be removed in the next version
    of EMACS.
[Verror-messages-buffer]
VARIABLE NAME
    error-messages-buffer

DESCRIPTION

    The string  variable  error-messages-buffer  is  checked each time
    Emacs  generates  an  error  message.   If the value is not a null
    string then the value is used as the name of a buffer.  Emacs will
    add  details  of each error message and its context to the buffer.
    Emacs creates the buffer if nessesary.

    Using this  variable  is  often  the  only  way  to  find  out why
    schedule-procedure  functions  fail.   Its also useful for finding
    hard to reproduce bugs in long running MLisp.

DEFAULT VALUE
    ""

SEE ALSO

[Verror-start-position]
VARIABLE NAME
    error-start-position

DESCRIPTION
    A  numeric variable used by error message parser (as specified in
    the variable error-message-parser) to indicate to the parse-error-
    message-in-region function on which line the current error message
    starts.  If  an error message parser sets this to zero, then error
    message parsing is stopped.

DEFAULT VALUE
    0

SEE ALSO
    error-file-name
    error-line-number
    error-message-parser
    Command parse-error-message-in-region
    Command compile-it
[Vexecute-monitor-command]
VARIABLE NAME
    execute-monitor-command

DESCRIPTION
    This variable contains the last string sent to the DCL sub-process
    executing  for execute-monitor-command. If you specify no argument
    to  the  command,  then this variables contents are re-used as the
    command string.

DEFAULT VALUE
    ""

SEE ALSO
    Command execute-monitor-command
[Vexit-emacs-hook]
VARIABLE NAME
    exit-emacs-hook

DESCRIPTION
    This variable contains the name of an EMACS function which will be
    called  when  EMACS  exits  to DCL. You can assign the variable to
    contain the name of any already existing function.

DEFAULT VALUE
    ""

SEE ALSO
    enter-emacs-hook
    leave-emacs-hook
    return-to-emacs-hook
[Vexpansion-only-completes-unique-choices]
VARIABLE NAME
    expansion-only-completes-unique-choices

DESCRIPTION

    This variable  changes  the  way that Emacs input completion logic
    works.  It is only applicable is the case where some of the choice
    are prefixes of some of the other choice.

    For example   the  command  "set"  is  a  prefix  of  the  command
    "set-default".   Consider  what  happens when you have typed "set"
    and press a key bound to expand-and-exit.

    If expansion-only-completes-unique-choices  is  0 the choice "set"
    is made.

    If expansion-only-completes-unique-choices  is  1  expand-and-exit
    does not choose anything.  It is nessesary to press a key bound to
    exit-emacs to choose "set" and you need to type more text, atleast
    a   "-"   followed  by  expand-and-exit,  to  be  able  to  choose
    "set-default".

DEFAULT VALUE
    0

SEE ALSO
[Vfalse]
CONSTANT NAME
    false

DESCRIPTION
    The boolean truth value representing FALSE.

DEFAULT VALUE
    FALSE

SEE ALSO
    true
[Vfetch-help-database-flags]
VARIABLE NAME
    fetch-help-database-flags

DESCRIPTION
    A  variable  used  to  describe  to  the fetch-help-database-entry
    command which  portions  of  the  information  available should be
    inserted  into  the  buffer. It is the logical oring of the values
    which  indicate  that a portion should be included. The bit values
    available are

    1	means include the text of this entry

    2	means include the key name lines for this entry

    4	means include the additional information lines

DEFAULT VALUE
    7

SE ALSO
    fetch-help-database-entry
[Vfilter-region-command]
VARIABLE NAME
    filter-region-command

DESCRIPTION
    This  variable  contains the last command string issued to the DCL
    sub-processing that the filter-region command uses. If you provide
    a  null  string  argument  to  the  command,  then  this variables
    contents will be re-used.

DEFAULT VALUE
    ""

SEE ALSO
    filter-region command
[Vforce-redisplay]
VARIABLE NAME
    force-redisplay

DESCRIPTION
    This  variable  control  how  the  screen  is updated when a large
    quantity  of  data  is emitted by a sub-process with an associated
    output  buffer. If none zero, then the screen is updated for every
    line   that   arrives.   Otherwise  the  screen  is  updated  less
    frequently.

DEFAULT VALUE
    OFF

SEE ALSO
[Vglobal-mode-string]
VARIABLE NAME
    global-mode-string

DESCRIPTION
    This variable contains the `global' mode that EMACS is running in.
    The contents of this variable are displayed in every window's mode
    line with the use of the "%M" mode line format identifier.

DEFAULT VALUE
    ""

SEE ALSO
    mode-line-format
[Vhelp-on-command-completion-error]
VARIABLE NAME
    help-on-command-completion-error

DESCRIPTION
    If  none  zero,  EMACS  will print a list of possibilities when an
    ambiguous  entity  (command,  database search list etc.) is given,
    otherwise it just rings the bell and waits for you to type more.

DEFAULT VALUE
    ON

SEE ALSO
[Vhighlight-region]
VARIABLE NAME
    highlight-region

DESCRIPTION
    When  set the region between mark and dot is displayed with graphic
    rendition defined by highlight-region-graphic-rendition. By default
    the  region  is  BOLDed.  If  mark is not set the buffer is display
    normally.

DEFAULT VALUE
    0
SEE ALSO
    default-highlight-region
    highlight-region-graphic-rendition
[Vhighlight-region-graphic-rendition]
VARIABLE NAME
    highlight-region-graphic-rendition

DESCRIPTION
    This  variable  is  used  to  control  the way that the highlighted
    region  is  displayed on terminals with the ANSI_CRT capability. It
    is the string of graphic rendition characters which are part of the
    ANSI SGR sequence.

    On  VT100  series  terminals  there  are  the following renditions
    possible:-

	"1"	- BOLD
	"4"	- UNDERSCORE
	"5"	- BLINKING
	"7"	- REVERSE VIDEO

    These renditions can be combined by separating each rendition with
    a ";". For example to get BOLD and UNDERSCORE together use "1;4".

DEFAULT VALUE
    "1"	(BOLD)

SEE ALSO
    highlight-region
    default-highlight-region
[Vhorizontal-scroll-step]
VARIABLE NAME
    horizontal-scroll-step

DESCRIPTION

    If automatic-horizontal-scrolling    is    1    the    value    of
    horizontal-scroll-step is used.

    horizontal-scroll-step controls how far Emacs will move the window
    horizontally to ensure that dot is visible.

    set horizontal-scroll-step  to  the  number  of columns that emacs
    will ensure is between dot and the edge of the window.

DEFAULT VALUE
    40

SEE ALSO
[Vindent-use-tab]
VARIABLE NAME
    indent-use-tab

DESCRIPTION
    Emacs will only use the TAB character while indenting when
    indent-use-tab is 1.

DEFAULT VALUE
    0

SEE ALSO
    logical-tab-size
    tab-size
    tab-indents-region
[Vinput-mode]
VARIABLE NAME
    input-mode

DESCRIPTION

    Emacs can  operate  in  two different input modes, traditional and
    GUI.

    In GUI  mode  Emacs  operates the highlight region in the style of
    Windows and Motif, a more intuitive mode.

    The region  will  be  set if you move the cursor while holding the
    shift key.

    Typing or  otherwise  inserting  text while the region is set will
    erase the region first.  In effect replacing region with new text.

    Using a delete command will erase the region rather then doing the
    normal  delete  action.   delete  commands  includes,  but  is not
    limited      to:      delete-next-character,     delete-next-word,
    delete-previous-character, delete-previous-word, erase-region.

    Setting the  mark  manually,  using  the  set-mark  command,  will
    temporarily disable the GUI input mode.

DEFAULT VALUE
    0

SEE ALSO
[Vis-comment]
CONSTANT NAME
    is-comment

DESCRIPTION
    This constant is return by the current-syntax-entity function when
    dot is located in a comment.

VALUE
    4

SEE ALSO
    function current-syntax-entity
    is-dull
    is-word
    is-string
    is-quote
[Vis-dull]
CONSTANT NAME
    is-dull

DESCRIPTION
    This constant is return by the current-syntax-entity function when
    dot is located in punctuation or white space.

VALUE
    0

SEE ALSO
    function current-syntax-entity
    is-word
    is-string
    is-quote
    is-comment
[Vis-quote]
CONSTANT NAME
    is-quote

DESCRIPTION
    This constant is return by the current-syntax-entity function when
    dot is located just after a quote character.

VALUE
    3

SEE ALSO
    function current-syntax-entity
    is-dull
    is-word
    is-string
    is-comment
[Vis-string]
CONSTANT NAME
    is-string

DESCRIPTION
    This constant is return by the current-syntax-entity function when
    dot is located in a matched quoted string.

VALUE
    2

SEE ALSO
    function current-syntax-entity
    is-dull
    is-word
    is-quote
    is-comment
[Vis-word]
CONSTANT NAME
    is-word

DESCRIPTION
    This constant is return by the current-syntax-entity function when
    dot is located in a word.

VALUE
    1

SEE ALSO
    function current-syntax-entity
    is-dull
    is-string
    is-quote
    is-comment
[Vjournal-frequency]
VARIABLE NAME
    journal-frequency

DESCRIPTION
    This  variable  sets  the time in seconds before any journal files
    are  flushed  to disk.  A reasonable value for this variable is 30
    seconds. To disable journaling set this variable to 0, OFF.

DEFAULT VALUE
    OFF

SEE ALSO
    current-buffer-journalled
[Vjournal-scratch-buffers]
VARIABLE NAME
    journal-scratch-buffers

DESCRIPTION
    By  default  Emacs  will  not  journal  scratch buffers, even when
    current-buffer-journalled  is 1.  This is normally a good thing as
    its  improves the performance of Emacs.  Many packages use scratch
    buffers which would be journalled if this variable was set to 1.

    Set this variable to 1 will enable journalling on all buffers.

DEFAULT VALUE
    0

SEE ALSO
[Vkeyboard-input-available]
VARIABLE NAME
    keyboard-input-available

DESCRIPTION
    This  variable is a count of the number of characters in the Emacs
    keyboard input buffer.  Use this function to check if anything has
    been typed.

DEFAULT VALUE
    None.

SEE ALSO
[Vkeystroke-history-buffer]
VARIABLE NAME
    keystroke-history-buffer

DESCRIPTION

    The string  variable  keystroke-history-buffer  is  checked each time
    Emacs  processes a complete key sequence.

    If the  value  is  not a null string then the value is used as the
    name of a buffer.  Emacs will add details of each key sequence and
    the  command,  if  any,  it executed.  Emacs creates the buffer if
    nessesary.

    Using this  variable  is  often  the only way to find out what you
    actually  typed when the wrong command executed or no command gets
    executed.

DEFAULT VALUE
    None.

SEE ALSO
[Vlast-expression]
VARIABLE NAME
    last-expression

DESCRIPTION
    last-expression is used by EMACS debugging packages to pick up the
    value of the last MLisp expression evaluated by EMACS.

SEE ALSO
[Vlast-keys-struck]
VARIABLE NAME
    last-keys-struck

DESCRIPTION
    last-keys-struck  holds the string of characters typed to activate
    the last command from a keymap.

SEE ALSO
    process-key
    process-key-hook
[Vleft-margin]
VARIABLE NAME
    left-margin

DESCRIPTION
    This  variable  contains  the column number of the left margin for
    automatic  text  justification.  After  an automatically generated
    newline the new line will be indented to the left margin.

DEFAULT VALUE
    Initialised from the variable default-left-margin

SEE ALSO
    text-mode command
    default-left-margin
[Vleave-emacs-hook]
VARIABLE NAME
    leave-emacs-hook

DESCRIPTION
    This variable contains the name of an EMACS function which will be
    called  when  EMACS  temporarily returns control to DCL (either by
    pause-emacs for return-to-monitor). You can assign the variable to
    contain the name of any already existing function.

DEFAULT VALUE
    ""

SEE ALSO
    enter-emacs-hook
    return-to-emacs-hook
    exit-emacs-hook
[Vlogical-tab-size]
VARIABLE NAME
    logical-tab-size (package)

DESCRIPTION
    Used by the tabs.ml package to set the size of a logical tab,
    which can be smaller then a hard tab.

DEFAULT VALUE
    8

SEE ALSO
    tab-size
    indent-use-tab
    tab-indents-region
[Vmaximum-DCL-buffer-size]
VARIABLE NAME
    maximum-DCL-buffer-size

DESCRIPTION
    When  a DCL buffer is created this variable is used to control the
    maximum  amount of text that will be remembered in the DCL buffer.
    If  the  number of characters is exceeded, the first characters in
    the buffer will be discarded.

DEFAULT VALUE
    10000

SEE ALSO
    DCL-buffer-reduction
[Vmode-line-format]
VARIABLE NAME
    mode-line-format

DESCRIPTION
    This  variable control the way that the mode line at the bottom of
    each window is displayed. The variable contains raw text, which is
    display  as  it  is typed, and format effectors with the following
    format:

	%nx

    "%" introduces a format effector. "n" is an integer specifying the
    absolute size that the field can occupy. Padding and truncation on
    the  right will occur where necessary. "x" is one of the following
    characters with the following meanings:

    `a'	Insert the string ",Abbrev" if abbreviation mode is on for the
	associated buffer.

    `b'	Insert the associated buffer's name.

    `c'	Insert  the string ",Checkpoint" if checkpointing is generally
	enabled, and specifically enabled for the associated buffer.

    `f'	Insert  the  file  name  if  the  associated  buffer is a File
	buffer, or the macro name if it is a Macro buffer.

    `m'	Insert   the   value  of  the  variable  mode-string  for  the
	associated buffer.

    `M'	Insert the value of the variable global-mode-string.

    `p'	Insert  the  position  of  dot  in  the associated buffer as a
	percentage  or  the string "Top" if dot is at the beginning of
	the buffer or "Bottom" if it is at the end.

    `r'	Insert  the  string ",Replace"if replace mode is in effect for
	the associated buffer.

    `*'	Insert  an asterisk if the associated buffer has been modified
	since it was last written to disk.

    `['	Insert zero or more open square brackets, one bracket for each
	recursive  edit level. If the recursion  depth is greater than
	4,  then  switch format to "[n[" where n is the recursive edit
	depth.

    `]'	Insert  zero  or  more  close square brackets, one bracket for
	each  recursive edit level. If the recursion  depth is greater
	than  4,  then switch format to "]n]" where n is the recursive
	edit depth. DEFAULT

DEFAULT VALUE
    This  variable is  initialised  from the contents of default-mode-
    line-format when a buffer is created.

SEE ALSO
    default-mode-line-format
[Vmode-line-graphic-rendition]
VARIABLE NAME
    mode-line-graphic-rendition

DESCRIPTION

    This  variable  is  used  to control the way that the mode line is
    displayed  on  terminals  with  the ANSI_CRT capability. It is the
    string  of graphic rendition characters which are part of the ANSI
    SGR sequence.

    VT100  series terminals have the following renditions available on
    them:-

	"1"	- BOLD
	"4"	- UNDERSCORE
	"5"	- BLINKING
	"7"	- REVERSE VIDEO

    These renditions can be combined by separating each rendition with
    a ";". For example to get BOLD and UNDERSCORE together use "1;4".

DEFAULT VALUE
    "7"	(REVERSE VIDEO)

SEE ALSO
    highlight-region
    default-highlight-region
[Vmode-string]
VARIABLE NAME
    mode-string

DESCRIPTION
    This  variable  contains the major mode for a buffer. The value of
    this  variable is displayed in the mode line for each window where
    the format effector "%m" is used.

DEFAULT VALUE
    "Normal"

SEE ALSO
   mode-line-format
[Vmouse-enable]
VARIABLE NAME
    mouse-enable

DESCRIPTION
    This  variable enables the generation of mouse input to Emacs when
    set to 1.

    This variable  does  not  control  the  mouse  input from terminal
    emulators  like  Xterms.

DEFAULT
    1

SEE ALSO
    mouse-on (command)
    mouse-off (command)
[Vnot-accessible]
CONSTANT NAME
    not-accessible

DESCRIPTION
    The value of this constant is returned by the file-exists function
    to indicate that the target file cannot be accessed.

VALUE
    0

SEE ALSO
    function file-exists
    read-only
    read-write
[Vomit-file-version-expansion]
VARIABLE NAME
    omit-file-version-expansion (VMS)

DESCRIPTION
    This variable is used by Emacs on VMS only.

    When  this  variable is TRUE it causes file versions to be omitted
    when file recognition is used with visit-file, read-file etc.

DEFAULT VALUE
    0

SEE ALSO
[Voperating-system-name]
VARIABLE NAME
    operating-system-name

DESCRIPTION
    This  read only variable contains the name of the operating system
    that Emacs is running on.

    Value	    Description
    -----	    -----------
    "Windows"	    Microsoft Windows NT or Windows 3.x
    "Macintosh"	    Apple Macintosh
    "MS-DOS"	    Microsoft MS-DOS
    "osf1"	    OSF/1
    "ultrix"	    Ultrix
    "VMS"	    OpenVMS/VAX or OpenVMS/AXP

DEFAULT

SEE ALSO
    operating-system-version

[Voperating-system-version]
VARIABLE NAME
    operating-system-version

DESCRIPTION
    This read  only  variable  contains  the  version of the operating
    system that Emacs is running on.

    In the  case  of  the  Microsoft  Windows set of operating systems
    operating-system-version allows you to tell them apart.

    Value	Description
    -----	-----------
    V3.10 Win32	Windows 3.1
    V3.10 NT	Windows NT 3.1

    On other  operating  systems  it  returns  the version in a format
    compatible with the host operating systems conventions.

DEFAULT

SEE ALSO
    operating-system-name
[Voverride-RMS-record-attribute]
VARIABLE NAME
    override-RMS-record-attribute

DESCRIPTION

    Set override-RMS-record-attribute  to  an  RMS record attribute to
    have  all files write out in with that record attibute.  This will
    override the current-buffer-RMS-record-attribute value.

    override-RMS-record-attribute can be set to one of

    Value			Description
    -----			-----------
    ""				Do not override
    "none"			Not a file buffer
    "fixed"			Image data
    "variable"			Normal VMS file
    "variable-fixed-control"	DCL created file
    "stream"			MS-DOS file
    "stream-lf"			Unix file
    "stream-cr"			Macintosh file

DEFAULT
    ""
SEE ALSO
    current-buffer-RMS-record-attribute
    default-buffer-RMS-record-attribute
[Vpop-up-windows]
VARIABLE NAME
    pop-up-windows

DESCRIPTION
    If  none  zero,  EMACS  will try to use some window other than the
    current one when it spontaneously generates a buffer that it wants
    you  to  see  or  when  you visit a file (it may split the current
    window). If OFF the current window is always used.

DEFAULT VALUE
    ON

SEE ALSO
    command pop-to-buffer
[Vprefix-argument]
VARIABLE NAME
    prefix-argument

DESCRIPTION
    Every   function  invocation  is  always  prefixed  by  a  numeric
    argument,   either   explicitly   with   ^Un  or  implicitly  with
    provide-prefix-argument. The value of the variable prefix-argument
    is  the  argument  prefixed to the invocation of the current MLisp
    function.

    For example, if the following function:

    (defun
        (show-it
            (message "The prefix argument is " prefix-argument)
        )
    )

    were  bound to the key ^A then typing ^U^A would cause the message
    "The  prefix  argument is 4" to be printed, and ^U13^A would print
    "The prefix argument is 13".

DEFAULT VALUE
    None.

SEE ALSO
    provide-prefix-argument
    prefix-argument-loop
    argument-prefix
    prefix-argument-provided
[Vprefix-argument-provided]
VARIABLE NAME
    prefix-argument-provided

DESCRIPTION
    If  none zero, a prefix argument has been provided for the current
    function. The value of it can be obtained from the prefix-argument
    variable.

DEFAULT VALUE
    None.

SEE ALSO
    prefix-argument
[Vprefix-string]
VARIABLE NAME
    prefix-string

DESCRIPTION
    This  variable  contains  the  string  that  is  inserted after an
    automatic newline has been generated in response to going past the
    right  margin.  This  is  generally used by the language-dependent
    commenting features.

DEFAULT VALUE
    ""

SEE ALSO
[Vprocess-key-hook]
VARIABLE NAME
    process-key-hook

DESCRIPTION
    This  buffer-specific variable hold the name of a function to call
    after  each  keymap  command.  last-keys-struck hold the string of
    keys that invoked the last command.

DEFAULT VALUE
    ""

SEE ALSO
    process-key
    last-keys-struck
[Vquery-replace-use-region-for-search]
VARIABLE NAME
    query-replace-use-region-for-search

DESCRIPTION
    Controls how  the  query  replace  functions  default  the  search
    string.   When ON and there is a region selected it is used as the
    search string defualt.

DEFAULT VALUE
    ON

SEE ALSO
[Vquick-redisplay]
VARIABLE NAME
    quick-redisplay

DESCRIPTION
    If  none  zero,  EMACS will not worry so much about the case where
    you  have  the  same buffer on view in several windows. It may let
    the  other  windows be inaccurate for a short while, but they will
    eventually be fixed up.

    Turning this ON speeds up EMACS substantially when the same buffer
    is  on  view  in  several windows. When it is OFF, all windows are
    always accurate.

DEFAULT VALUE
    OFF

SEE ALSO
[Vread-only]
CONSTANT NAME
    read-only

DESCRIPTION
    The value of this constant is returned by the file-exists function
    to indicate that the target file is readable.

VALUE
    -1

SEE ALSO
    function file-exists
    not-accessible
    read-write
[Vread-only-buffer]
VARIABLE NAME
    read-only-buffer

DESCRIPTION
    When  read-only-buffer  is  set  TRUE  all  attempts to modify the
    buffer result in an error.

DEFAULT VALUE
    OFF

SEE ALSO
    default-read-only-buffer
[Vread-write]
CONSTANT NAME
    read-write

DESCRIPTION
    The value of this constant is returned by the file-exists function
    to indicate that the target file can be read and written.

VALUE
    1

SEE ALSO
    function file-exists
    read-only
    not-accessible
[Vremove-help-window]
VARIABLE NAME
    remove-help-window

DESCRIPTION
    When  none  zero, help windows that have been automatically popped
    up onto the screen are remove. Otherwise the help window will stay
    on the screen after they have been finished with.

DEFAULT VALUE
    ON

SEE ALSO
[Vreplace-case]
VARIABLE NAME
    replace-case (not implemented?)

DESCRIPTION
    If  none  zero,  EMACS  will alter the case of strings substituted
    with (re-)replace-string or (re-)query-replace-string to match the
    case of the original string.

    For  example,  replacing "which" by "that" in the string "Which is
    silly" results in "That is silly"; in the string "the car which is
    red"  results  in  "the car that is red"; and in the string "WHICH
    THING?" results in "THAT THING?".

DEFAULT VALUE
    OFF

SEE ALSO
    command query-replace-string
    command re-query-replace-string
    command re-replace-string
    command replace-string
[Vreplace-mode]
VARIABLE NAME
    replace-mode

DESCRIPTION
    This  variable controls the replace minor mode. If none zero for a
    buffer,  characters  that  are  inserted  into  the buffer replace
    existing characters. This can be useful for editing pictures etc.

DEFAULT VALUE
    OFF

SEE ALSO
[Vrestored-environment]
VARIABLE NAME
    restored-environment

DESCRIPTION
    This  read-only variable is 0 if EMACS has not been restored from a
    saved  environment. Otherwise it is a count of the number of times
    that the current environment has been saved and restored.

DEFAULT VALUE
    0

SEE ALSO
    command save-environment
[Vreturn-to-emacs-hook]
VARIABLE NAME
    return-to-emacs-hook

DESCRIPTION
    This variable contains the name of an EMACS function which will be
    called  when  EMACS  returns  from  DCL  after  a  pause-emacs  or
    return-to-monitor  command. You can assign the variable to contain
    the name of any already existing function.

DEFAULT VALUE
    ""

SEE ALSO
    enter-emacs-hook
    leave-emacs-hook
    exit-emacs-hook
[Vright-margin]
VARIABLE NAME
    right-margin

DESCRIPTION
    The  right margin for automatic text justification. If a character
    is  inserted  at  the  end of a line to the right of right-margin,
    EMACS  will automatically insert at the beginning of the preceding
    word  a newline, tabs and spaces to indent to the left margin, and
    the  prefix  string. With the right margin set to 72 (for example)
    you  can type in a document without worrying about when to hit the
    return  key, EMACS will automatically do it for you at exactly the
    right place.

DEFAULT VALUE
    Initialised from default-right-margin at buffer creation.

SEE ALSO
    default-right-margin
    left-margin
    Command text-mode
[Vscroll-step]
VARIABLE NAME
    scroll-step

DESCRIPTION
    This  variable controls the number of lines by which a window move
    when dot moves out of the window.

    For example, if scroll-step is set to one, and you are on the last
    line  displayed  in  a window, then going down one line will cause
    the buffer to scroll up one line.

DEFAULT VALUE
    7

SEE ALSO
[Vsearch-string]
VARIABLE NAME
    search-string

DESCRIPTION
    This  read-only  variable holds the last search-string used by any
    search   command.  The  search-string  variable  is  preserved  by
    save-... commands.

SEE ALSO
[Vsilently-kill-processes]
VARIABLE NAME
    silently-kill-processes

DESCRIPTION
    If  none  zero,  EMACS  will  kill processes when it exits without
    asking any questions. Normally, if you have processes running when
    EMACS exits, the question "You have processes on the prowl, should
    I chase them down for you" is asked.

DEFAULT VALUE
    OFF

SEE ALSO
    kill-process
    start-DCL-process to start a process.
[Vsplit-height-threshhold]
VARIABLE NAME
    split-height-threshhold

DESCRIPTION
    This  variable  controls  which  windows  EMACS  considers when it
    decides  to  create a new window. Any window with a height that is
    less than split-height-threshold is a candidate for being split to
    create the new window.

DEFAULT VALUE
    7

SEE ALSO
[Vstack-maximum-depth]
VARIABLE NAME
    stack-maximum-depth

DESCRIPTION
    Emacs will   limit   the   number   of   nested   MLisp  calls  to
    stack-maximum-depth  calls.   Raise  this  limit  if  you need run
    recursive or very complex MLisp.

DEFAULT VALUE
    100

SEE ALSO

[Vstack-trace-on-error]
VARIABLE NAME
    stack-trace-on-error

DESCRIPTION
    If  none  zero, EMACS will write a MLisp stack trace to the "Stack
    trace"  buffer  whenever  an  error  is encountered from within an
    MLisp function (even inside an error-occurred).

DEFAULT VALUE
    OFF

SEE ALSO
    command dump-stack-trace
    trace-mode

[Vswap-ctrl-x-char]
VARIABLE NAME
    swap-ctrl-x-char

DESCRIPTION

    This variable  allows  you  to  change  the  way  that  emacs
    interprets control keys you type.

    The value of this variable is the character code to swap with
    ^X.   Whenever  emacs sees a ^X the value of swap-ctrl-x-char
    is   substituted.    And   whenever  the  you  type  the  key
    corrisponding   to   the  value  of  swap-ctrl-x-char  ^x  is
    substituted.

    The reason  this  variable  was added to emacs was to make it
    easy  to  use  ^X  as  the Windows style Cut action.  Without
    rewritting  all  the  existing MLisp code so that it does not
    use ^X as a key binding prefix.

    The default-emacs-init  function  sets up ^\ (28) and ^] (29)
    as suitable swap candadates.

EXAMPLE
    (setq swap-ctrl-x-char 28) ; swap ^x and ^\

    (setq swap-ctrl-x-char 29) ; swap ^x and ^\

DEFAULT VALUE
    24	^X

SEE ALSO
    default-emacs-init command
[Vsyntax-array]
VARIABLE NAME
    syntax-array

DESCRIPTION

    When syntax-array  is  true  Emacs  will optimise the operation of
    commands  that  depend  on  syntax  data.  However maintaining the
    syntax array will slow down other operations in Emacs.

    The following  compares  commands  and  features  with and without
    syntax-array:

    syntax colouring
   
    Syntax colouring  is  disabled  if  syntax-array  is  turned  off.
    Turning  on  syntax-colouring will also turn on synatx-array for a
    buffer.

    re-search-forward, re-search-reverse,  re-query-replace-string and
    re-replace-string

    The \S,  \C, \s, \c regular expression features are only available
    when syntax-array is true.

    backward-paren, forward-paren,   backward-balanced-paren-line  and
    forward-balanced-paren-line

    When syntax-array  is  true  these  commands  correctly  take into
    account  comments  and  strings  when searching for a parenthesis.
    When  syntax-array  is  false these commands uses a dead reckoning
    algorithm  that  in  some  situations  will  not  find the correct
    parenthesis.

    current-syntax-entity

    When syntax-array  is  true  this  command  is fast.  However when
    syntax-array  is  false current-syntax-entity has to calculate the
    syntax by examining each character from the start of the buffer up
    to dot.

DEFAULT
    default-syntax-array

SEE ALSO
    default-syntax-array
    syntax-colouring
[Vsyntax-colouring]
VARIABLE NAME
    syntax-colouring

DESCRIPTION
    When syntax-colouring  is true Emacs will colour the characters of
    a buffer depending on the syntax of those characters.

    The colours are taken from the variables:

        syntax-word-rendition
        syntax-string-rendition
        syntax-comment-rendition
        syntax-keyword1-rendition
        syntax-keyword2-rendition                 

    All dull  characters are displayed in the window-graphic-rendition
    colour.

    If syntax-colouring  is  false all characters are displayed in the
    window-graphic-rendition.

DEFAULT
    default-syntax-colouring

SEE ALSO
    default-syntax-colouring
    syntax-array
    default-syntax-array
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword2-rendition                 
    window-graphic-rendition
[Vsyntax-word-rendition]
VARIABLE NAME
    syntax-word-rendition

DESCRIPTION
    Defines  the colour that will be used to draw word characters when
    syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword2-rendition                 
    window-graphic-rendition
[Vsyntax-string-rendition]
VARIABLE NAME
    syntax-string-rendition

DESCRIPTION
    Defines the  colour  that  will  be used to draw string characters
    when syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword2-rendition                 
    window-graphic-rendition
[Vsyntax-comment-rendition]
VARIABLE NAME
    syntax-comment-rendition

DESCRIPTION
    Defines the  colour  that  will be used to draw comment characters
    when syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword2-rendition                 
    window-graphic-rendition
[Vsyntax-keyword1-rendition]
VARIABLE NAME
    syntax-keyword1-rendition

DESCRIPTION
    Defines the  colour  that  will  be  used  to  draw keyword type 1
    characters when syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword2-rendition
    syntax-keyword3-rendition                 
    window-graphic-rendition
[Vsyntax-keyword2-rendition]
VARIABLE NAME
    syntax-keyword2-rendition

DESCRIPTION
    Defines the  colour  that  will  be  used  to  draw keyword type 2
    characters when syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword3-rendition                 
    window-graphic-rendition
[Vsyntax-keyword3-rendition]
VARIABLE NAME
    syntax-keyword3-rendition

DESCRIPTION
    Defines the  colour  that  will  be  used  to  draw keyword type 3
    characters when syntax-colouring is true.

DEFAULT
    "" black on white

SEE ALSO
    syntax-colouring
    syntax-word-rendition
    syntax-string-rendition
    syntax-comment-rendition
    syntax-keyword1-rendition
    syntax-keyword2-rendition                 
    window-graphic-rendition
[Vtab-indents-region]
VARIABLE NAME
    tab-indents-region

DESCRIPTION
    When tab-indents-region controls what happends when you type a
    tab and the region is set. If tab-indents-region is 1 the region
    is indented, if 0 the region is replaced with a tab.

DEFAULT VALUE
    1

SEE ALSO
    logical-tab-size
    indent-use-tab
    tab-size
[Vtab-size]
VARIABLE NAME
    tab-size

DESCRIPTION
    This  variable  controls  how  a  tab is display on the screen. It
    describes  the number of space characters used to display a single
    tab character.

DEFAULT VALUE
    8

SEE ALSO
    logical-tab-size
    indent-use-tab
    tab-indents-region
[Vterminal-cursor-type]
VARIABLE NAME
    terminal-cursor-type

DESCRIPTION
    This  variable controls the shape of the cursor.  A line cursor is
    used when the value is 0 and a block cursor is used when the value
    is 1.

DEFAULT
    0

SEE ALSO

[Vterminal-DEC-CRT-level-1]
VARIABLE NAME
    terminal-DEC-CRT-level-1

DESCRIPTION

    The variable is true if the terminal has the DEC_CRT characteristic
    set. This variable is setup each time EMACS is entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-DEC-CRT-level-2]
VARIABLE NAME
    terminal-DEC-CRT-level-2

DESCRIPTION

    The   variable   is   true   if   the  terminal  has  the  DECCRT2
    characteristic  set.  This  variable  is  setup each time EMACS is
    entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-DEC-CRT-level-3]
VARIABLE NAME
    terminal-DEC-CRT-level-3

DESCRIPTION

    The   variable   is   true   if   the  terminal  has  the  DECCRT3
    characteristic  set.  This  variable  is  setup each time EMACS is
    entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-DEC-CRT-level-4]
VARIABLE NAME
    terminal-DEC-CRT-level-4

DESCRIPTION

    The   variable   is   true   if   the  terminal  has  the  DECCRT4
    characteristic  set.  This  variable  is  setup each time EMACS is
    entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-advanced-video-option]
VARIABLE NAME
    terminal-advanced-video-option

DESCRIPTION

    The  variable  is  true if the terminal has the AVO characteristic
    set. This variable is setup each time EMACS is entered.

DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-ansi-crt]
VARIABLE NAME
    terminal-ansi-crt

DESCRIPTION

    The  variable  is true if the terminal has the ANSI characteristic
    set. This variable is setup each time EMACS is entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-application-keypad]
VARIABLE NAME
    terminal-application-keypad

DESCRIPTION

    The  variable  is  true if the terminal has the APPLICATION_KEYPAD
    characteristic  set.  This  variable  is  setup each time EMACS is
    entered.  When  this  variable is set assume that the keypad is in
    application  mode.  This means that you should not turn the keypad
    off when exiting or pausing from EMACS.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-does-not-need-null-padding]
VARIABLE NAME
    terminal-does-not-need-null-padding

DESCRIPTION

    When set  true this variable stops Emacs sending NULL's as padding
    characters  to  the  terminal. This is only useful on VAX stations
    when xon-mode is turned off.

DEFAULT VALUE
    0

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-edit]
VARIABLE NAME
    terminal-edit

DESCRIPTION

    The  variable  is true if the terminal has the EDIT characteristic
    set. This variable is setup each time EMACS is entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-eightbit
    terminal-soft-chars
[Vterminal-eightbit]
VARIABLE NAME
    terminal-eightbit

DESCRIPTION

    The   variable   is   true   if  the  terminal  has  the  EIGHTBIT
    characteristic  set.  This  variable  is  setup each time EMACS is
    entered.  When  set  the  full  DEC  multi-national  character set is
    printable on the terminal.

DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-soft-chars
[Vterminal-is-terminal]
VARIABLE NAME
    terminal-is-terminal

DESCRIPTION
    This  variable  is  TRUE if EMACS is connected to a terminal. This
    variable is FALSE is EMACS is reading input from a command file.

    This variable has the value 0 if the display is a file.

    This variable has the value 1 if the display is a terminal device.

    This variable has the value 3 if the display is a GUI device.

SEE ALSO
[Vterminal-keyboard]
VARIABLE NAME
    terminal-keyboard (not implemented?)

DESCRIPTION
    This  read  only  variable  returns  the  number  of  keys  on the
    keyboard as reported by the operating system.  THe value is 0 when
    the  operating  systems  does not keep the information.  Typically
    this is only useful on MS-DOS, Windows and Macintosh systems.

DEFAULT

SEE ALSO

[Vterminal-length]
VARIABLE NAME
    terminal-length

DESCRIPTION
    This  variable  holds the length of the terminal in lines. If this
    variable  is  set  to a new length EMACS will redraw the screen to
    include  only  that  number  of lines. This variable is setup each
    time EMACS is entered.

    If  the  new  size  is  smaller hen the old size EMACS deletes all
    other  windows  on  the  screen and adjusts the size of the current
    windows to fill the whole screen.

    If  the  new  size is larger then the old size EMACS will increase
    the size of the current window to include the extra lines.

    This  variable  is  defaulted  from the terminal page length. When
    EMACS  exits  or  pauses  it updates the terminal characteristics to
    include the new length.

DEFAULT VALUE
    Taken from terminal setting, typically 24.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-output-buffer-size]
VARIABLE NAME
    terminal-output-buffer-size

DESCRIPTION

    This variable  controls  the  size of the buffer used to output to
    the terminal from Emacs.

DEFAULT VALUE
    256

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-regis]
VARIABLE NAME
    terminal-regis

DESCRIPTION

    The  variable is true if the terminal has the REGIS characteristic
    set. This variable is setup each time EMACS is entered.


DEFAULT VALUE
    Taken from terminal setting.

SEE ALSO
    terminal-DEC-CRT-level-1
    terminal-DEC-CRT-level-2
    terminal-advanced-video-option
    terminal-ansi-crt
    terminal-application-keypad
    terminal-edit
    terminal-eightbit
    terminal-soft-chars
[Vterminal-width]
VARIABLE NAME
    terminal-width

DESCRIPTION

    This  variable controls the width of the terminal. If the terminal
    is   capable   of   showing  different  screen  widths,  then  the
    appropriate  width will be selected when the variable is assigned.
    The  value  is specified as a number, and can be in the range from
    32 to 145 inclusive.

DEFAULT VALUE
    The width of the terminal as obtained from VAX/VMS.

SEE ALSO
[Vthis-command]
VARIABLE NAME
    this-command

DESCRIPTION
    This  read  only variable contains the ASCII character code of the
    last  character  typed  in  the  key  sequence  used to invoke the
    current  function.  It can be used by functions bound to many keys
    to decide what to do.

DEFAULT VALUE
    Last character struck

SEE ALSO
[Vtrace-hook]
VARIABLE NAME
    trace-hook

DESCRIPTION
    This  variable  contains  the  name  of  the  trace function. When
    trace-mode is enabled, the function whose name is in trace-hook is
    called  just  before  each  complex  MLisp expression is executed.
    Simple  expressions  (like  45 or "Hello" or foo) do not cause the
    trace function to be executed.

    The  trace  function  may  decompile  the  expression that will be
    executed by using the decompile-current-line function.

DEFAULT VALUE
    trace

SEE ALSO
    trace-mode
    trace
    function decompile-current-line
    function decompile-mlisp-function
[Vtrace-into]
VARIABLE NAME
    trace-into

DESCRIPTION
    If  non-zero,  then  tracing  will continue into called functions.
    Otherwise,  tracing  is  be  temporarily  disabled while a call to
    another function is made.

DEFAULT VALUE
    1

SEE ALSO
    trace-mode
    trace-hook
[Vtrace-mode]
VARIABLE NAME
    trace-mode

DESCRIPTION
    This  variable  controls  trace  mode.  When  none zero, Mock lisp
    execution  is  traced.  When  trace-mode  is enabled, the function
    whose  name  is  in  trace-hook is called just before each complex
    MLisp  expression  is  executed.  Simple  expressions  (like 45 or
    "Hello" or foo) do not cause the trace function to be executed.

DEFAULT VALUE
    OFF

SEE ALSO
    trace-hook
    function trace
    function decompile-current-line
    function decompile-mlisp-function
[Vtrack-eol-on-^n-^p]
VARIABLE NAME
    track-eol-on-^N-^P

DESCRIPTION
    If none zero, then next-line and previous-line will "stick" to the
    end  of  a line if they are started there. If zero, these commands
    will  try  to stay in the same column as you move up and down even
    if you started at the end of a line.

DEFAULT VALUE
    OFF

SEE ALSO
    next-line
    previous-line
[Vtrue]
CONSTANT NAME
    true

DESCRIPTION
    The boolean truth value representing TRUE.

DEFAULT VALUE
    TRUE

SEE ALSO
    false
[Vuser-colour-1]
VARIABLE NAME
    user-colour-1

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   1   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-2
    user-colour-3
    user-colour-4
    user-colour-5
    user-colour-6
    user-colour-7
    user-colour-8
[Vuser-colour-2]
VARIABLE NAME
    user-colour-2

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   2   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-3
    user-colour-4
    user-colour-5
    user-colour-6
    user-colour-7
    user-colour-8
[Vuser-colour-3]
VARIABLE NAME
    user-colour-3

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   3   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-4
    user-colour-5
    user-colour-6
    user-colour-7
    user-colour-8
[Vuser-colour-4]
VARIABLE NAME
    user-colour-4

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   4   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-3
    user-colour-5
    user-colour-6
    user-colour-7
    user-colour-8
[Vuser-colour-5]
VARIABLE NAME
    user-colour-5

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   5   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-3
    user-colour-4
    user-colour-6
    user-colour-7
    user-colour-8
[Vuser-colour-6]
VARIABLE NAME
    user-colour-6

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   6   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-3
    user-colour-4
    user-colour-5
    user-colour-7
    user-colour-8
[Vuser-colour-7]
VARIABLE NAME
    user-colour-7

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   7   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-3
    user-colour-4
    user-colour-5
    user-colour-6
    user-colour-8
[Vuser-colour-8]
VARIABLE NAME
    user-colour-8

DESCRIPTION
    Graphic    rendition    for    any   text   coloured   as   8   by
    apply-colour-to-region.

SEE ALSO
    Command apply-colour-to-region
    user-colour-1
    user-colour-2
    user-colour-3
    user-colour-4
    user-colour-5
    user-colour-6
    user-colour-7
[Vuser-interface-hook]
VARIABLE NAME
    user-interface-hook

DESCRIPTION

    This variable  is intended for use of the shipped Emacs MLisp code
    only.

    The procedure named in this variable is executed:

    * after   restoring   the  Emacs  environment,  or  executing  the
    emacs_profile.ml

    * before calling the procedure in the enter-emacs-hook.

    The user-interface-hook  is  intended  to  run  a  procedure  that
    customises  the Emacs user interface based on present environment.
    This  cannot  be  done  and  saved in environment file and it is a
    snapshot taken in the past.

DEFAULT VALUE
    None.

SEE ALSO
[Vunlink-checkpoint-files]
VARIABLE NAME
    unlink-checkpoint-files

DESCRIPTION
    If  none zero, EMACS will delete the corresponding checkpoint file
    after  the  master  copy is written -- this avoids having a lot of
    .CKP files lying around but it does compromise safety a little.

    For  example,  as you are editing a file called "foo.c" EMACS will
    be periodically be writing a checkpoint file called "foo.CKP" that
    contains  all of your recent changes. When you rewrite the file if
    unlink-checkpoint-files is ON then the .CKP file will be deleted,
    otherwise  it will be left. When EMACS exits, all checkpoint files
    will be deleted.

DEFAULT VALUE
    OFF

SEE ALSO
    checkpoint
[Vvisible-bell]
VARIABLE NAME
    visible-bell

DESCRIPTION
    If  none zero, EMACS will attempt to use a visible bell, usually a
    horrendous  flashing  of  the screen, instead of the audible bell,
    when  it  is notifying you of some error. This is a more "socially
    acceptable"  technique  when  people  are  working  in  a  crowded
    terminal room.

DEFAULT VALUE
    OFF

SEE ALSO
    black-on-white
[Vwindow-size]
VARIABLE NAME
    window-size

DESCRIPTION
    This  read  only  variable  contains  the  height  of  the current
    buffer's window.

DEFAULT VALUE
    None.

SEE ALSO
[Vwindow-first-column]
VARIABLE NAME
    window-first-column

DESCRIPTION
    This  variable is set to the column number of the first column that
    is visible in the current window.

    The  command  scroll-one-column-left  decreases  the  value of this
    variable  and  scroll-one-column-right  increases the value of this
    variable.

SEE ALSO
    scroll-one-column-left
    scroll-one-column-right
[Vwindow-graphic-rendition]
VARIABLE NAME
    window-graphic-rendition

DESCRIPTION

    If syntax-colouring  is  false all characters are displayed in the
    window-graphic-rendition.    Otherwise  all  dull  characters  are
    displayed in the window-graphic-rendition colour.


DEFAULT

SEE ALSO
    syntax-colouring
[Vwindow-width]
VARIABLE NAME
    window-width

DESCRIPTION
    This  variable  contains  the width of the current window.  If the
    window  is  a vertically split one then setting this variable will
    change the width of the window.

DEFAULT VALUE
    The width of the current window.

SEE ALSO
[Vwrap-long-lines]
VARIABLE NAME
    wrap-long-lines

DESCRIPTION
    If  none  zero,  EMACS will display long lines by "wrapping" their
    continuation onto the next line (the first line will be terminated
    with  a '\'). If OFF long lines get truncated at the right edge of
    the  screen  and  a  '$'  is  display  to  indicate  that this has
    happened.

DEFAULT VALUE
    default-wrap-long-lines

SEE ALSO
    default-wrap-long-lines
[Vxon-mode]
VARIABLE NAME
    xon-mode

DESCRIPTION
    xon-mode sets the way EMACS synchronises with the terminal.

    When  on,  ^S  is  treated  as  an  XOFF  character, and causes the
    terminal  to suspend outputting, and ^Q  is treated as an XON which
    causes  character transmission to be resumed. These characters are
    processed  by  VMS,  and  never  reach  EMACS,  so you cannot have
    anything bound to these keys.

    When  xon-mode  is  off,  ^S  and ^Q are just treated as any other
    character.

    Some  terminals  require  to  run with xon-mode ON. If they have a
    specialist  EMACS  terminal driver, then xon-mode will be switched
    on  by  default. If you have a terminal that requires xon-mode and
    you pretend  to be some other terminal, then you will need to turn
    xon-mode  on  in  you  emacsinit.ml  (or face the consequences). A
    notable terminal that falls into the second class is the VT125.

DEFAULT VALUE
    Depends on the terminal, but mostly ON

SEE ALSO
