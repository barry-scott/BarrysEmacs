; 
; emacsinit.ml
; 
(progn
; 
; Set up EMACS 
;	to use LK201 keyboard
;	to use EDT keypad
;	have command recall in the minibuffer
;	to checkpoint files
;	to have useful defaults for variables
; 

; 
; set up variables
; 
(setq display-end-of-file (setq default-display-end-of-file 1))
(setq checkpoint-frequency 500)
(setq activity-indicator 1)
(setq case-fold-search (setq default-case-fold-search 1))
(setq unlink-checkpoint-files 1)
(setq silently-kill-processes 1)

; 
; set up LK201 and EDT keypad
; 
(execute-mlisp-file "lk201")	; LK201 keys
(execute-mlisp-file "edtsim")	; and overlay the EDT keypad on it
(execute-mlisp-file "mouse")	; mouse support
; 
;   Set up minibuffer command recall
; 
(execute-mlisp-file "minibuf-recall")

; 
;   Set up some aditional binding
; 
(bind-to-key "pause-emacs" "\(pf1)\r")	; PF1-Return

; 
; set up auto executes
; 
(auto-execute "lisp-mode" "*.ml")
(auto-execute "lisp-mode" "*.key")
(auto-execute "lisp-mode" "*.mlp")

; 
; set up enter and exit hooks
; 
(defun
    (user-enter-actions
	; 
	; list of things to be done when entering EMACS from DCL
	; 
	(EDT-keypad)
	(if (= operating-system-name "Windows")
	    (progn
		; turn on the mouse
		(mouse-on 0)
		; make the window fill the frame
		(UI-window-maximize)
	    )
	)
    )
    (user-exit-actions ~yes
	;
	; list of things to be done when leaving EMACS to DCL
	;
	(EDT-no-keypad)
	(setq ~yes (substr (get-tty-string "Save any modified files (Yes/No) [Yes]? ") 1 1))
	(if (| (= ~yes "Y") (= ~yes "y") (= (length ~yes) 0))
	    (write-modified-files)
	    ; leave the message on the screen
	    (sit-for 5)
	)
    )
)
(setq enter-emacs-hook
    (setq return-to-emacs-hook "user-enter-actions"))
(setq exit-emacs-hook
    (setq leave-emacs-hook "user-exit-actions"))
)
