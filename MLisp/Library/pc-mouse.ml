;
;	pc-mouse.ml
; 
;	Copyright (c) 1998-2001 Barry A. Scott
;
;	23-Mar-1998	Convert into pc-mouse.ml
; 
;	25-Sep-1987	J.A.Lomicka
;
;	This mouse package requires V5 of VAX Emacs, which supports
;	the built-in goto-wondow-at-x-y and has full support
;	for CSI sequences.  By itself, it provides support for
;	VWS V3.2 terminal emulator and compatible terminals.
;

; 
; must turn this on before the rest of the file is parsed
; 
(setq control-string-convert-mouse 1)

(declare-global
    mouse-double-click-time	; time in mSec for the double click interval

    mouse-1-up-time		; time when button one came up last
    mouse-2-up-time		; time when button two came up last
    mouse-3-up-time		; time when button three came up last

    mouse-multi-click-count	; the number of multiple clicks so far

    favorite-set-mark		; Set-mark function
    favorite-copy-region	; Copy region function
    favorite-kill-region	; Kill region function
    favorite-yank		; Yank function

    ~last-mouse-1-down-hooked-double-click

    mouse-region-start
    mouse-region-end
    mouse-mode			; char=0, word=1, line=2
    ~mouse-debug	    	; true if mouse debug messages are to be
				; logged
    ~saved-mousex
    ~saved-mousey
    ~saved-mouseevent
    ~saved-mouseplace
    ~saved-click-count
    ~mouse-version
    ~mouse-drag-active
)
(declare-buffer-specific
    mouse-1-double-click-hook	; action for double-click of mouse-1
)
(setq-default mouse-1-double-click-hook "")
(setq ~mouse-version "pc v1.0")
(setq ~last-mouse-1-down-hooked-double-click 0)
(defun mouse-on()
    (setq mouse-enable 3)
    (setq mouse-multi-click-count 0)
)

(defun mouse-mode()
    ;	Enable the control-string parser for parsing of
    ;	escape sequences.  Convert F-keys and mouse sequences to single
    ;	keystrokes.
    (if (! control-string-processing)
	(error-message
	    "Control String Processing must be enable for the Mouse package to work.")
    )
    
    ;	Set up the default bindings for the mouse buttons
    (if (= mouse-double-click-time 0)
	(setq mouse-double-click-time 300)
    )
    
    (bind-to-key "mouse-1-down" "\(mouse-1-down)")
    (bind-to-key "mouse-1-up" "\(mouse-1-up)")
    (bind-to-key "mouse-motion" "\(mouse)")
    (bind-to-key "mouse-2-down" "\(mouse-2-down)")
    (bind-to-key "mouse-2-up" "\(mouse-2-up)")
    (bind-to-key "mouse-3-down" "\(mouse-3-down)")
    (bind-to-key "mouse-3-up" "\(mouse-3-up)")
    
    ;
    ; mouse-wheel bindings
    ; 
    (bind-to-key
	"(scroll-one-line-down (* 3 (fetch-array  control-string-parameters 1 2)))"
	"\[mouse-wheel-pos]"
    )
    (bind-to-key
	"(scroll-one-line-up (* 3 (fetch-array  control-string-parameters 1 2)))"
	"\[mouse-wheel-neg]"
    )
    (bind-to-key
	"(previous-line (* 3 (fetch-array  control-string-parameters 1 2)))"
	"\[shift-mouse-wheel-pos]"
    )
    (bind-to-key
	"(next-line (* 3 (fetch-array  control-string-parameters 1 2)))"
	"\[shift-mouse-wheel-neg]"
    )
    (bind-to-key
	"(previous-page (fetch-array  control-string-parameters 1 2))"
	"\[ctrl-mouse-wheel-pos]"
    )
    (bind-to-key
	"(next-page (fetch-array  control-string-parameters 1 2))"
	"\[ctrl-mouse-wheel-neg]"
    )
    (bind-to-key
	"(previous-page (fetch-array  control-string-parameters 1 2))"
	"\[ctrl-shift-mouse-wheel-pos]"
    )
    (bind-to-key
	"(next-page (fetch-array  control-string-parameters 1 2))"
	"\[ctrl-shift-mouse-wheel-neg]"
    )

    ;    (bind-to-key "mouse-cut" "\(mouse-2-down)")
    ;    (bind-to-key "mouse-up" "\(mouse-2-up)")
    ;    (bind-to-key "mouse-paste" "\(mouse-3-down)")
    ;    (bind-to-key "mouse-up" "\(mouse-3-up)")
)

(defun mouse-off()
    (setq mouse-enable 0)
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
    (setq y (+ (fetch-array control-string-parameters 1 3)))
    (setq x (+ (fetch-array control-string-parameters 1 4)))
    (setq event (+ (fetch-array control-string-parameters 1 1)))
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
	(setq mouse-second-click 
	    (&
		(mouse-double-click mouse-1-up-time)
		(= x ~saved-mousex)
		(= y ~saved-mousey)
		(= (| event 1) (| ~saved-mouseevent 1))
	    )
	)
    )
)

(defun mouse-double-click( ~down-time )
    (< (- elapse-time ~down-time) mouse-double-click-time)
)

;	The routine mouse-reposition is used to position the cursor at
;	the location the user specified in the last mouse click.  Also
;	performed is any set-up action needed to provide the scrolling and
;	mode bar movements of mouse-finish-scroll.

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
    (~mouse-log (concat "mouse-finish-scroll( " c " " r " )"))
    (if (= ~saved-mouseplace 0);	Started in text, scroll text in window
	(error-occurred
	    (scroll-one-line-up (- ~saved-mousey r))
	)
    )
    (if (> window-size 1)
	(progn
	    (if (& ~saved-mouseplace 1);  Started in mode line, move mode line or h-scroll
		(progn; mode line and horizontal indicated,  Move the mode line
		    (if (> ~saved-mousey r)
			(progn
			    (~mouse-log (concat "Moving mode line up by " (- ~saved-mousey r)))
			    (while (>= (- ~saved-mousey r) (- window-size 1)) 
				(while (!= window-width terminal-width) (delete-window))
				(delete-window)
			    )
			    (setq window-size (+ window-size (- r ~saved-mousey)))
			)
			(< ~saved-mousey r); elseif
			(progn
			    (~mouse-log (concat "Moving mode line down by " (- r ~saved-mousey)))
			    (down-window)
			    (while (>= (- r ~saved-mousey) (- window-size 1))
				(setq ~saved-mousey (+ ~saved-mousey window-size))
				(while (!= window-width terminal-width) (delete-window))
				(delete-window)
				(error-occurred (down-window))
			    )
			    (goto-window-at-x-y ~saved-mousex ~saved-mousey)
			    (setq window-size (+ window-size (- r ~saved-mousey)))
			); of moving mode line down progn
		    ); of moving mode line if
		); horizontal and mode line progn
	    ); of place 1 of
	    (if (& ~saved-mouseplace 2);  Started in vertical bar, move vertical bar
		(progn; move a vertical bar
		    (if (> ~saved-mousex c)
			(progn
			    (~mouse-log "Moving bar left, delete windows in the way")
			    (while (>= (- ~saved-mousex c) (- window-width 1)) 
				(delete-window)
			    )
			    (setq window-width (+ window-width (- c ~saved-mousex)))
			)
			(< ~saved-mousex c); elseif
			(progn
			    (~mouse-log "Moving mode line right")
			    (right-window)
			    (while (>= (- c ~saved-mousex) (- window-width 1))
				(setq ~saved-mousex (+ ~saved-mousex window-width))
				(delete-window)
				(right-window)
			    )
			    (left-window)
			    (setq window-width (+ window-width (- c ~saved-mousex)))
			); of moving mode line down progn
		    ); moving right if
		); moving vertical bar progn
	    ); of place 2 if
	)
    )
    ; update the saved x and y positions
    (setq ~saved-mousex c)
    (setq ~saved-mousey r)
)

;	The routine mouse-up is used as the action routine to respond to
;	a button-up that finishes a scrolling operation. Normally
;	it is associated with button 1, but it checks to make sure that
;	the most recent event (defined by the most recent call to
;	mouse-reposition) was a down event on whatever button it
;	is bound to, so it could be moved to any button-up key binding.

(defun mouse-up() mousex mousey mouseevent
    (mouse-parameters)
    (if (= (+ ~saved-mouseevent 1) mouseevent)
	(mouse-finish-scroll mousex mousey)
    )
    (setq ~saved-mouseevent mouseevent)
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

(defun mouse-paste-clipboard()
    mousex mousey mouseevent mouse-second-click

    (mouse-parameters)
    (mouse-reposition mousex mousey mouseevent)
    (UI-edit-paste "clipboard")
)

(defun mouse-paste-primary()
    mousex mousey mouseevent mouse-second-click

    (mouse-parameters)
    (mouse-reposition mousex mousey mouseevent)
    (UI-edit-paste "primary")
)


(defun copy-to-killbuffer( );	Missing from basic command set
    (copy-region-to-buffer "Kill buffer")
)

(defun v5bug-yank();		There is a bug in (ex-ext-cmd "yank..")
    (yank-buffer "Kill buffer")
)


(defun
    (mouse-1-down
	mousex mousey mouseevent mouse-second-click
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-1-down x:"
			mousex " y:" mousey " event:" mouseevent " second:" mouse-second-click))
	
	(setq ~last-mouse-1-down-hooked-double-click 0)
	
	(if 
	    (if
		(= mouse-second-click 0)	; not a double click
		(progn
		    ~new-dot
		    
		    (goto-window-at-x-y mousex mousey 0 ~new-dot)
		    
		    (if
			(error-occurred (mark))	; mark is not set
			0
			; and the new dot is inside the region
			(if (< (dot) (mark))
			    (& (>= ~new-dot (dot)) (< ~new-dot (mark)))
			    (& (>= ~new-dot (mark)) (< ~new-dot (dot)))
			)
		    )
		)
		0
	    )
	    ; mark is set and its is not a double click
	    (progn
		(~mouse-log (concat "mouse-1-down Drag dot:" (+ (dot)) "mark:"(+ (mark)) ))
		(setq ~mouse-drag-active 1)
		; 
		; save the region
		; 
		(if (< (dot) (mark))
		    (exchange-dot-and-mark)
		)
		(setq mouse-region-start (mark))
		(setq mouse-region-end (dot))
		(unset-mark)
		(mouse-reposition mousex mousey mouseevent)
		(message "Drag the region to a new position")
	    )
	    ; mark is not set OR a second mouse click
	    (progn
		(~mouse-log "mouse-1-down click")
		(setq ~mouse-drag-active 0)
		(mouse-reposition mousex mousey mouseevent)
		(if mouse-second-click
		    (progn
			(setq mouse-multi-click-count (% (+ mouse-multi-click-count 1) 3))
			(if
			    (&
				(= mouse-multi-click-count 1)
				(!= mouse-1-double-click-hook "")
			    )
			    (progn
				(execute-mlisp-line (concat "(" mouse-1-double-click-hook ")"))
				(setq mouse-multi-click-count 0)
				(setq ~last-mouse-1-down-hooked-double-click 1)
			    )
			)
		    )
		    (setq mouse-multi-click-count 0)
		)
		(setq mouse-mode mouse-multi-click-count)
		(if (= mouse-multi-click-count 0)
		    (setq mouse-region-start (setq mouse-region-end (dot))))
		(mouse-position-update 1)
	    )
	)
    )
)

(defun
    (mouse-1-up
	mousex mousey mouseevent
	
	; 
	; Start the double-click timer
	; 
	(setq mouse-1-up-time elapse-time)
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-1-up x:" mousex " y:" mousey " event:" mouseevent " drag: " ~mouse-drag-active))
	
	(if ~mouse-drag-active
	    (progn
		(setq ~mouse-drag-active 0)
		(goto-window-at-x-y mousex mousey)
		(if
		    (&
			(> (dot) mouse-region-start)
			(< (dot) mouse-region-end)
		    )
		    (progn
			; 
			; abandon the drag
			; unset the mark and set the dot to the up position
			; 
			(~mouse-log "mouse-1-up drag cancelled")
			(unset-mark)
		    )
		    (progn
			; do drag
			~insert-point
			~region-text
			
			(~mouse-log "mouse-1-up do drag")
			
			; remember where to insert
			(setq ~insert-point (dot))
			; save text of region
			(goto-character mouse-region-start)
			(set-mark)
			(goto-character mouse-region-end)
			(setq ~region-text (region-to-string))
			(erase-region)
			; move back to the insert point
			(goto-character ~insert-point)
			; insert the text
			(insert-string ~region-text)
			; set region around the inserted text
			(set-mark 1)
			(goto-character ~insert-point)
		    )
		)
	    )
	    (progn
		(if
		    (= ~last-mouse-1-down-hooked-double-click 0)
		    (progn
			(goto-window-at-x-y mousex mousey)
			(mouse-position-update 1)
		
			; 
			; unset the mark if the mouse was just positioning the dot
			; 
			(if (= (mark) (dot))
			    (unset-mark)
			)
		    )
		)
	    )
	)
    )
)

(defun
    (mouse-2-down
	mousex mousey mouseevent
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-2-down x:" mousex " y:" mousey))
	
	(setq ~saved-mousex mousex)
	(setq ~saved-mousey mousey)
    )
)

(defun
    (mouse-2-up
	mousex mousey mouseevent
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-2-up x:" mousex " y:" mousey))
	(if
	    (&
		(=  ~saved-mousex mousex)
		(= ~saved-mousey mousey)
	    )
	    (novalue)
	)
    )
)

(defun
    (mouse-3-down
	mousex mousey mouseevent
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-3-down x:" mousex " y:" mousey))
	
	(setq ~saved-mousex mousex)
	(setq ~saved-mousey mousey)
    )
)

(defun
    (mouse-3-up
	mousex mousey mouseevent
	
	(mouse-parameters)
	(~mouse-log (concat "mouse-3-up x:" mousex " y:" mousey))
	
	
	(if
	    (progn
		; is the mouse in a region?
		~new-dot
		
		(goto-window-at-x-y mousex mousey 0 ~new-dot)
		
		(if
		    (error-occurred (mark))	; mark is not set
		    0
		    ; and the new dot is inside the region
		    (if (< (dot) (mark))
			(& (>= ~new-dot (dot)) (< ~new-dot (mark)))
			(& (>= ~new-dot (mark)) (< ~new-dot (dot)))
		    )
		)
	    )
	    ; leave the dot where it is
	    (novalue)
	    ; move the dot to the up click location
	    (progn
		(mouse-reposition mousex mousey mouseevent)
	    )
	)
	(if
	    (&
		(= ~saved-mousex mousex)
		(= ~saved-mousey mousey)
	    )
	    (UI-popup-menu mousex mousey)
	)
    )
)


(defun ~mouse-forward-word()
    (if
	(looking-at "\\w")
	(forward-word)
	(looking-at "[\t ][\t ]*")
	(goto-character
	    (save-excursion
		(region-around-match 0)
		(dot)
	    )
	)
	(error-occurred (forward-character))
    )
)
(defun ~mouse-backward-word()
    (if
	(looking-at "\\w")
	(progn
	    (forward-character)
	    (backward-word)
	)
	(looking-at "[\t ]")
	(goto-character
	    (save-excursion
		(forward-character)
		(re-search-reverse "[^\t ]")
		(dot)
	    )
	)
    )
)

(defun mouse-motion()
    mousex mousey mouseevent
    
    (setq mousey (+ (fetch-array control-string-parameters 1 3)))
    (setq mousex (+ (fetch-array control-string-parameters 1 4)))
    (setq mouseevent (+ (fetch-array control-string-parameters 1 1)))
    
    (if ~mouse-drag-active
	(progn
	    (goto-window-at-x-y mousex mousey)
	    (message "Dragging region")
	)
	(if (!= ~saved-mouseplace 0)
	    ; move modeline
	    (mouse-finish-scroll mousex mousey)
	    ; set region
	    (progn	
		(goto-window-at-x-y mousex mousey)
		(mouse-position-update 0)
	    )
	    
	)
    )
)

(defun mouse-position-update( ~assume-moved )
    ~moved
    (setq ~moved (| ~assume-moved (!= mousex ~saved-mousex) (!= mousey ~saved-mousey)))
    
    (~mouse-log (concat "mouse-position-update moved=" ~moved))
    
    ; has the mouse moved?
    (if ~moved
	(progn ~start ~end
	    ; yes moved
	    (save-excursion
		(setq mouse-region-end (dot))
		(if (>= mouse-region-end mouse-region-start)
		    (progn
			(setq ~start mouse-region-start)
			(setq ~end mouse-region-end)
		    )
		    (progn
			(setq ~end mouse-region-start)
			(setq ~start mouse-region-end)
		    )
		)
		; move the region end point
		(goto-character ~end)
		(if
		    (= mouse-mode 0)
		    ; select a simple range
		    (progn
			(~mouse-log "End: Simple range")
			(setq ~end (dot))		
		    )
		    (= mouse-mode 1)
		    ; select a word range
		    (progn
			(~mouse-log "End: Word range")
			(~mouse-forward-word)
			(setq ~end (dot))
		    )
		    ; otherwise
		    ; select a line range
		    (progn
			(~mouse-log "End: Line range")
			(end-of-line) (forward-character)
			(setq ~end (dot))
		    )
		)
		
		; move the region start point
		(goto-character ~start)
		(if
		    (= mouse-mode 0)
		    ; select a simple range
		    (progn
			(~mouse-log "Start: Simple range")
			(setq ~start (dot))
		    )
		    (= mouse-mode 1)
		    ; select a word range
		    (progn
			(~mouse-log "Start: Word range")
			(~mouse-backward-word)
			(setq ~start (dot))
		    )
		    ; otherwise
		    ; select a line range
		    (progn
			(~mouse-log "Start: Line range")
			(beginning-of-line)
			(setq ~start (dot))
		    )
		)
		
		(~mouse-log (concat "mouse-position-update ~start=" (+ ~start) " ~end=" (+ ~end)))
		
	    )
	    (goto-character ~start) (set-mark 1)
	    (goto-character ~end)
	    
	    ; update the colouring
	    ;	    (apply-colour-to-region 1 (+ (buffer-size) 1) 0)
	    ;	    (apply-colour-to-region ~start ~end 1)		
	)
    )
    (~mouse-log (concat "mouse-position-update done"))
)

(mouse-mode)

(defun
    ~mouse-log (~msg)
    
    (if ~mouse-debug
	(save-window-excursion old-gui
	    (setq input-mode 0)
	    (pop-to-buffer "Mouse Log")
	    (end-of-file)
	    (insert-string
		(sprintf
		    "%8.8d X:%3.3d(S:%3.3d) Y:%3.3d(S:%3.3d) E:%d(S:%d) R:%6.6d C:%d M:%d Region(%d,%d) (%s)\n"
		    elapse-time
		    mousex ~saved-mousex mousey ~saved-mousey mouseevent ~saved-mouseevent
		    (- elapse-time mouse-1-up-time) mouse-multi-click-count mouse-mode 
		    mouse-region-start mouse-region-end ~msg
		    "\n"
		)
	    )
	    (setq input-mode 1)
	)
    )
)
