;	mousev5.me	Mouse package for V5 emacs
;
;	25-Sep-1987	J.A.Lomicka
;
;	This mouse package requires V5 of VAX Emacs, which supports
;	the built-in goto-wondow-at-x-y and has full support
;	for CSI sequences.  By itself, it provides support for
;	VWS V3.2 terminal emulator and compatible terminals.
;
(declare-global
  mouse-continuous;	If set to 1, scrolling is continuous
  mouse-was-recent;	When set, mouse click was within 4 seconds
  favorite-set-mark;	Set-mark function
  favorite-copy-region;	Copy region function
  favorite-kill-region;	Kill region function
  favorite-yank;	Yank function
  foreign-mouse;	Set to indicate event was from a foreign mouse
  foreign-mouse-x
  foreign-mouse-y
  foreign-mouse-event
)

(defun mouse-on
  (cursor-type (+ (get-tty-string ": mouse-on (cursor-type 0-1)[0] ")))
;	Send the escape sequences that enable the VWS mouse.  Note that with
;	V3.2 of VWS, enabling the mouse implies taking responsibility for
;	cross-session cut and paste as well.

  (setq mouse-enable 1)
  (if terminal-DEC-CRT-level-2
    (send-string-to-terminal (concat "\e[" cursor-type ")u\e[1;3'{\e[1;2'z"))
  )
)

(defun mouse-mode()
    ;	Enable the control-string parser for parsing of VWS and BBN BitGraph
    ;	escape sequences.  Convert F-keys and mouse sequences to single
    ;	keystrokes.
    (if (! control-string-processing)
	(error-message
"Control String Processing must be enable for the Mouse package to work.")
    )
    (setq control-string-convert-mouse 1)
    ;	Set up the favorite copy/cut/paste commands if not already done
    
    (if (= favorite-set-mark "") (setq favorite-set-mark "set-mark"))
    (if (= favorite-copy-region "")
	(setq favorite-copy-region "copy-to-killbuffer")
    )
    (if (= favorite-kill-region "")
	(setq favorite-kill-region "delete-to-killbuffer")
    )
    (if (= favorite-yank "") (setq favorite-yank "v5bug-yank"))
    
    ;	Set up the default bindings for the mouse buttons
    
    (bind-to-key "mouse-pos" "\(mouse-1-down)")
    (bind-to-key "mouse-up" "\(mouse-1-up)")
    (bind-to-key "mouse-cut" "\(mouse-2-down)")
    (bind-to-key "mouse-up" "\(mouse-2-up)")
    (bind-to-key "mouse-paste" "\(mouse-3-down)")
    (bind-to-key "mouse-up" "\(mouse-3-up)")
    (bind-to-key "mouse-moved" "\201K")
    
    ;	Set up the default bindings for local COPY/PASTE
    
    (bind-to-key "accept-region" "\e[!x")
    (bind-to-key "accept-region" "\e[(")
    (bind-to-key "mouse-local-copy" "\^X\(mouse-2-down)")
    (bind-to-key "mouse-local-paste" "\^X\(mouse-3-down)")
    (bind-to-key "local-copy-region" "\^Xc")
    (bind-to-key "local-paste" "\^X\^Y")
)

(defun mouse-off()
  (setq mouse-enable 0)
  (if terminal-DEC-CRT-level-2
    (send-string-to-terminal "\e[0'{\e[0;0'z")
  )
)

;	The routine mouse-parameters is used to get the parameters out
;	of the CSI argument string and put them in the caller's variables
;	"mousex", "mousey", and "mouseevent".  This provides a central
;	place where mouse parameters are obtained from their corresponding
;	parameters, and inherently defines a hook that can be used to
;	accomodate other kinds of terminals.  (Load this package, then
;	redefine mouse-parameters to get the parameters correctly for the
;	foreign terminal.)

(defun mouse-parameters()
    x y event
  (if foreign-mouse
    (progn
      (setq y foreign-mouse-y)
      (setq x foreign-mouse-x)
      (setq event foreign-mouse-event)
      (setq foreign-mouse 0)
    )
  ; else this is a normal mouse
    (progn
      (setq y (+ (fetch-array control-string-parameters 1 3)))
      (setq x (+ (fetch-array control-string-parameters 1 4)))
      (setq event (+ (fetch-array control-string-parameters 1 1)))
    )
  )
  (if (is-bound mousey)
    (setq mousey y)
  )
  (if (is-bound mousex)
    (setq mousex x)
  )
  (if (is-bound mouseevent)
    (setq mouseevent event)
  )
  (if (is-bound mouse-second-click)
    (setq mouse-second-click (&
      mouse-was-recent
      (= x ~saved-mousex)
      (= y ~saved-mousey)
      (= (| event 1) (| ~saved-mouseevent 1))
    ))
  )
  (setq mouse-was-recent 1);	Start the double-click timer
  (schedule-procedure "mouse-not-recent" 4)
)

(defun mouse-not-recent();	Schedule to cancel double-click timer
  (setq mouse-was-recent 0)
)

;	The routine mouse-reposition is used to position the cursor at
;	the location the user specified in the last mouse click.  Also
;	performed is any set-up action needed to provide the scrolling and
;	mode bar movements of mouse-finish-scrolling.
(declare-global
  ~saved-mousex
  ~saved-mousey
  ~saved-mouseevent
  ~saved-mouseplace
)

(defun mouse-reposition( x y event)
    placecode
  (setq ~saved-mousex x)
  (setq ~saved-mousey y)
  (setq ~saved-mouseevent event)
  (setq ~saved-mouseplace (goto-window-at-x-y x y))
  (if replace-mode
    (if (= ~saved-mouseplace 0)
      (while (< (current-column) mousex) (insert-string " "))
    )
  )
)

;	The routine mouse-finish-scrolling is used to complete the
;	scrolling and mode bar movements that were initiated in
;	a call to mouse-reposition.

(defun mouse-finish-scroll( c r)
  (if (= ~saved-mouseplace 0);	Started in text, scroll text in window
    (error-occurred
      (provide-prefix-argument (- ~saved-mousey r) (scroll-one-line-up))
    )
  )
  (if (& ~saved-mouseplace 1);  Started in mode line, move mode line or h-scroll
    (progn; mode line and horizontal indicated,  Move the mode line
      (if (> ~saved-mousey r)
	(progn; Moving mode line up, delete windows in the way
	  (while (>= (- ~saved-mousey r) (- window-size 1)) 
	    (while (!= window-width terminal-width) (delete-window))
	    (delete-window)
	  )
	  (set-window-size (+ window-size (- r ~saved-mousey)))
	)
      (< ~saved-mousey r); elseif
	(progn; Moving mode line down
	  (down-window)
	  (while (>= (- r ~saved-mousey) (- window-size 1))
	    (setq ~saved-mousey (+ ~saved-mousey window-size))
	    (while (!= window-width terminal-width) (delete-window))
	    (delete-window)
	    (error-occurred (down-window))
	  )
	  (goto-window-at-x-y ~saved-mousex ~saved-mousey)
	  (set-window-size (+ window-size (- r ~saved-mousey)))
	); of moving mode line down progn
      (!= ~saved-mousex c); else if horizontal scroll
	(error-occurred (provide-prefix-argument (- ~saved-mousex c)
	  (scroll-one-column-right)
	))
      ); of moving mode line if
    ); horizontal and mode line progn
  ); of place 1 of
  (if (& ~saved-mouseplace 2);  Started in vertical bar, move vertical bar
    (progn; move a vertical bar
      (if (> ~saved-mousex c)
	(progn; Moving bar left, delete windows in the way
	  (while (>= (- ~saved-mousex c) (- window-width 1)) 
	    (delete-window)
	  )
	  (set-window-width (+ window-width (- c ~saved-mousex)))
	)
      (< ~saved-mousex c); elseif
	(progn; Moving mode line right
	  (right-window)
	  (while (>= (- c ~saved-mousex) (- window-width 1))
	    (setq ~saved-mousex (+ ~saved-mousex window-width))
	    (delete-window)
	    (right-window)
	  )
	  (left-window)
	  (set-window-width (+ window-width (- c ~saved-mousex)))
	); of moving mode line down progn
      ); moving right if
    ); moving vertical bar progn
  ); of place 2 if
)

;	The routine mouse-up is used as the action routine to respond to
;	a button-up that finishes a scrolling operation. Normally
;	it is associated with button 1, but it checks to make sure that
;	the most recent event (defined by the most recent call to
;	mouse-reposition) was a down event on whatever button it
;	is bound to, so it could be moved to any button-up key binding.

(defun mouse-up() mousex mousey mouseevent
  (if mouse-continuous (send-string-to-terminal "\e[1;3'{\e[1;2'z"))
  (mouse-parameters)
  (if (= (+ ~saved-mouseevent 1) mouseevent)
    (mouse-finish-scroll mousex mousey)
  )
  (setq ~saved-mouseevent mouseevent)
)

;	The routine "mouse-pos" is used as the action routine for a
;	down-stroke that performs the usual positioning and scrolling
;	operations.  It may be bound to any mouse down-key, or called as
;	the first action, with no parameters, in any function that is
;	bound to a mouse down-key.  In this way, packages can have local
;	bindings to the mouse keys that do other things after performing
;	the position function.

(defun mouse-pos() mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (if mouse-second-click
    (error-occurred (execute-keyboard-macro))
  )
  (if mouse-continuous (send-string-to-terminal (concat
    "\e[" mousey ";" mousex ";" mousey ";" mousex "'w"
  )))
)
;	Mouse-moved is the default action for filter rectangle events,
;	and is used to perform continuous scroll operations while the button
;	is held down.  We fetch the parameters directly, in order to avoid
;	resetting the double-click timer
(defun mouse-moved() mousex mousey mouseevent
  (if (= ~saved-mouseevent 2)
    (progn
      (setq mousey (+ (fetch-array control-string-parameters 1 3)))
      (setq mousex (+ (fetch-array control-string-parameters 1 4)))
      (mouse-finish-scroll mousex mousey)
      (setq ~saved-mousex mousex)
      (setq ~saved-mousey mousey)
      (send-string-to-terminal (concat
	"\e[" mousey ";" mousex ";" mousey ";" mousex "'w"
      ))
    )
  )
)

;	The routine "mouse-cut" is used as the action routine for a
;	down-strike that marks the opposite end of a region and copies
;	or cuts the region to the kill buffer.
(defun mouse-cut()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (execute-extended-command favorite-set-mark); Use user preference
  (mouse-reposition mousex mousey mouseevent)
  (exchange-dot-and-mark)
  (if mouse-second-click
    (execute-extended-command favorite-kill-region); Use user preference
  ;else this is first click
    (progn
      (execute-extended-command favorite-copy-region); Use user preference
      (message "Region copied")
    )
  )
)

(defun mouse-paste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (if mouse-second-click
    (execute-extended-command favorite-yank); Use user preference
  ; else
    (message "Press again to paste.")
  )
)

(defun copy-to-killbuffer( );	Missing from basic command set
  (copy-region-to-buffer "Kill buffer")
)

(defun v5bug-yank();		There is a bug in (ex-ext-cmd "yank..")
  (yank-buffer "Kill buffer")
)

; The function "local-copy-region" is used to copy the current select region
; into the terminal's local copy/paste buffer.

(defun local-copy-region ()
  (narrow-region)
  (send-string-to-terminal "\eP!x")
  (beginning-of-file)
  (while (! (eobp))
    (send-string-to-terminal
      (if (= (following-char) '\n')
	"0D";	Newline case
      ;	Else normal character, convert to hex
	(concat
	  (substr "0123456789ABCDEF" (+ 1 (& (/ (following-char) 16) 15)) 1)
	  (substr "0123456789ABCDEF" (+ 1 (& (following-char) 15)) 1)
	)
      )
    )
    (forward-character)
  )
  (send-string-to-terminal "\e\\")
  (widen-region)
)

;	The function "mouse-local-copy" is used to mark out a region and
;	copy it to the terminal's local copy/paste buffer

(defun mouse-local-copy()  mousex mousey mouseevent mouse-second-click
  (set-mark)
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (local-copy-region)
  (message "Region copied to terminal")
)

;	The function "local-paste" is used to request the paste buffer
;	of the terminal.

(defun local-paste ()
  (send-string-to-terminal "\e[0;0)t")
)

;	"mouse-local-paste" is used to paste into the current locator position

(defun mouse-local-paste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (local-paste)
)

(defun dehex( a b) c
  (setq c (+
    (* 16 (if (>= a 'A')  (- a 55) (- a '0')))
    (if (>= b 'A')  (- b 55) (- b '0'))
  ))
  (if (= c 13) 10 c)
)

;	The function "accept-region" will read the select region from the
;	terminal.  A bug in the current version of VWS causes VWS to lie
;	about the buffer length sometimes, so you can't trust the number
;	in C, you have to wait for the ST.

(defun accept-region () c s c1 c2
(send-string-to-terminal "\007")
  (setq c (+ 2 (fetch-array control-string-parameters 1 1)))
  (setq s "")
  (while (> c 0)
    (setq c1 (get-tty-character))
    (setq c2 (get-tty-character))
    (if (< c1 '0');	If first character is a control character
      (setq c -1);	exitloop
    ;else this is good hex data
      (setq s (concat s (char-to-string (dehex c1 c2))))
    )
  )
  (set-mark)
  (insert-string s)
)

(defun set-window-size ( size) r
  (setq r (- size window-size))
  (error-occurred (provide-prefix-argument r (enlarge-window)))
)

(defun set-window-width ( size) r
  (setq r (- size window-width))
  (error-occurred (provide-prefix-argument r (widen-window)))
)

(mouse-mode)

