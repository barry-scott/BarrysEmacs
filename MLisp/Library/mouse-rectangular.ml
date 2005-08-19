;	mouserect.ml	Mouse interface to rectangular package
;
;	27-Jan-1988	Jeff Lomicka
;
;	Requires "mousev5.ml" and "rectangular.ml" be loaded first.
;
;	Uses escape-prefix mouse buttons for rectangular operations.
;		Left - Rectangular copy between text cursor and mouse
;		Cetner - Rectangular cut between text cursor and mouse
;		Right - Paste at mouse location
;	If in replace-mode, the replace-mode variation of cut and paste
;	is used.

(progn

(defun mouse-rcopy()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (set-mark)
  (rectangular-select)
  (mouse-reposition mousex mousey mouseevent)
  (rectangular-copy)
)

(defun mouse-rcut()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (set-mark)
  (setq replace-with-white-space replace-mode)
  (rectangular-select)
  (mouse-reposition mousex mousey mouseevent)
  (rectangular-cut)
)

(defun mouse-rpaste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (setq overstrike-mode replace-mode)
  (rectangular-paste)
)

(bind-to-key "mouse-rcopy" "\e\201C")
(bind-to-key "mouse-rcut" "\e\201E")
(bind-to-key "mouse-rpaste" "\e\201G")
)
