; minibuf.ml
; 
; History:
; 
; modified by Rick Ellison to add capability for doing:
;
;tested with V4.1 linked on  2-SEP-1986 at 16:34 on HEART by SCOTT
;  -- not tested with v5 Emacs --
; 
; ^U<arg><esc><esc><minbuf stuff><esc><esc>
;	         or
;        <esc><esc><minbuf stuff>^U<arg><esc><esc>
; 
; either of which result in <minbuf stuff> being given the <arg> as a
; prefix-argument.  If both the entering and exiting args are provided, the
; exiting arg is used.
; 
; 
;   this packages has been worked on by:-
;	Bob Surtees, Rod, Burr, Roger Goun, Rick Ellison,
;	and Barry Scott.
; 
; Include into 4.1 Emacs from emacsprocs.not
;
;  Note: apostrophes are doubled so that paren matching,
;        forward-sexpr, and such work correctly.
; 
(progn				; for execute-mlisp-buffer
(defun
    (minibuf-exit val
	; 
	; Save the user''s s-expressions in a buffer, so they
	; can be restored later.
	; 
	(switch-to-buffer "minibuffer-save")
	(yank-buffer "minibuffer")
	(switch-to-buffer "minibuffer")
	
	; 
	; Build a defun around buffer.
	; 
	(beginning-of-file)
	(insert-string "(defun (~minifun ")	; open the "defun"
; 
; add to let ^U<arg><esc><esc><stuff><esc><esc> do <stuff> <arg> times
; 
	(insert-string " \n (provide-prefix-argument saved-prefix-argument" )
	(insert-string " \n (progn \n ")
; end of add
	(end-of-file)
; add to line below the \n and parens to balance those added above
	(insert-string "\n))))") 		; close the "defun"
	
	; 
	; Evaluate it in an error-occurred, so we do not get
	; stuck in the window if we blow it.
	; 
	(if (error-occurred
		(execute-mlisp-buffer))
	    ; if there was an error report it
	    (setq val (concat "definition error: " error-message))
	    ; else say no problem so far
	    (setq val "def ok")
	)
	; 
	; here if error or not
	; Restore the user''s s-expressions without the
	; surrounding "(defun( ... ))"
	; 
	(erase-buffer)			; get rid of what we had
	(yank-buffer "minibuffer-save")	; get original back
	(delete-buffer "minibuffer-save")	; eliminate save buffer
	(delete-window)			; become invisible
	
	; 
	; depending on success of definition of ~minifun
	; 
	(if (= val "def ok")
	    ; then execute the new function in error occurred
	    (if (error-occurred
		    (setq val (~minifun)))
		(if (!= error-message "\"~minifun\" did not return a value; \"setq\" was expecting it to.")
		    (progn ; else a real error
			(send-string-to-terminal "\^G")
			(message "execute error:" error-message)
		    )
		    ; probably a function that just did not return a value
		)
		(message val)
	    )
	    ; else had error during defun
	    (progn
		(send-string-to-terminal "\^G")
		(message val)
	    )
	)
    )
)

    
(defun
    (minibuf-enter
	(split-current-window)		; Make a new window.
 	(error-occurred
 	    (provide-prefix-argument
 		(- window-size minibuf-window-size 1)    ; make window big as
  		(shrink-window)			     ; desired
 	    )
 	)
	(switch-to-buffer "minibuffer")
	(save-excursion
	    (set-mark)				; to be able to remove mode-string
	    (insert-string mode-string)		; see if we are in Lisp mode
	    (exchange-dot-and-mark)		; get ready
	    (if (! (looking-at ".*Lisp"))	; if not looking at some lisp mode
		(lisp-mode))			; then get one
	    (erase-region)		; get mode back out of buffer
	)
    )
)


(defun
    (minibuf
; 
; note:  Esc-Esc (or whatever you bind to minibuf) brings you here for both
; entering and leaving the minibuf
; 
    (if (= current-buffer-name "minibuffer")
; 
; add for doing ^U<arg><esc><esc> upon leaving minibuf
; 
       	; then already in minibuf; so, exit
	(progn
	    (if prefix-argument-provided
		; then
		(setq saved-prefix-argument prefix-argument)
		; else use previous value
	     )  ; end of if
; end of add -- note paren to balance progn is added below
; 
	    (minibuf-exit)			;     in minibuffer so exit
; add to allow ^U<arg><esc><esc> upon entering
; 
	)  ; end of then already in minibuf; so, exit
	(progn  ; else not already in minibuf; so, enter it
	    (if prefix-argument-provided
		; then
		(setq saved-prefix-argument prefix-argument)
		; else use 1 -- no sticky entry values
		(setq saved-prefix-argument 1)
	     )  ; end of if
; end of add
	    (minibuf-enter)			; not in minibuffer so enter
; add paren to balance
	)  ; end else not already in minibuf; so, enter it
; 
	)
    )
)


(save-window-excursion
    ; Startup actions; to be executed only once.
    (declare-global	minibuf-window-size
; add global variable
			saved-prefix-argument)
    (if (= minibuf-window-size 0)
	(setq minibuf-window-size 5))	; set a default
    ; get key bindings, if any
    (if (error-occurred (execute-mlisp-file "minibuf.key"))
	; default if no .KEY file
	(bind-to-key "minibuf" "\e\e")
    )
    (switch-to-buffer "minibuffer")
    (lisp-mode)
)
)
