(progn
;
;    Copyright (c) 1983-2010 Barry A. Scott
; 
; 	lisp mode package
; 
; 	updated:
; 	Barry A. Scott	 5-Jun-1983	correct syntax table
; 					add messages each side of a M-c
;	Barry A. Scott	 5-Feb-1986	improve defun-compiling and add
;					electric-lisp-end-comment.
; 
(defun
    (paren-pause dot instabs
	(if (eolp) (delete-white-space))
	(setq instabs (bolp))
	(setq dot (dot))
	(insert-character ')')
	(save-excursion
	    (backward-paren 0)
	    (if instabs
		(save-excursion descol
		    (setq descol (current-column))
		    (goto-character dot)
		    (to-col descol)
		)
	    )
	)
        (paren-flash-only)
    )
)
(defun    
    (nl-indent column
	(delete-white-space)
	(save-excursion
	    (backward-balanced-paren-line 0)
	    (setq column
		(if (bolp)
		    (current-indent)
		    (+ (current-column) 4)
		)
	    )
	    (if (< column 5) (setq column 5))
	)
	(newline)
	(to-col column)
    )
)
(defun    
    (re-indent-line
	(save-excursion column
	    (beginning-of-line)
	    (delete-white-space)
	    (save-excursion
		(if (= (following-char) ')')
		    (progn
			(forward-character)
			(backward-paren 0)
			(setq column (current-column))
		    )
		    (progn
			(backward-character)
			(backward-balanced-paren-line 0)
			(setq column
			    (if (bolp)
				(current-indent)
				(+ (current-column) 4)
			    )
			)
			(if (< column 5) (setq column 5))
		    )
		)
	    )
	    (to-col column)
	)
    )
)
(defun
    (indent-lisp-function
	(message "Indenting function...") (sit-for 0)
	(save-excursion
	    (if (error-occurred (end-of-line) (search-reverse "(defun"))
		(error-message "Cannot find function")
	    )
	    (set-mark)
	    ;	    (forward-character)
	    (forward-paren 0)
	    (exchange-dot-and-mark)
	    (delete-white-space)
	    (beginning-of-line)
	    (next-line)
	    (while (& (! (eobp)) (<= (dot) (mark)))
		(re-indent-line)
		(next-line)
	    )
	)
	(message "Indenting function... done.")
    )
)
(defun    
    (electric-lisp-semi
	~syntax
	(setq ~syntax (current-syntax-entity))
	(if (& (!= ~syntax is-comment) (!= ~syntax is-string)) 
	    (progn
		(beginning-of-line)
		(if (looking-at "[\t ]*$")
		    (progn
			(end-of-line)
			(insert-string "; ")
			(if (!= 3 (current-column))
			    (re-indent-line))
		    )
		    (progn
			(end-of-line)
			(move-to-comment-column)
			(insert-string "; ")
		    )
		)
		(setq left-margin (current-column))
		(setq right-margin 77)
		(setq prefix-string "; ")
	    )
	    (insert-string last-keys-struck)
	)
    )
)
(defun
    (electric-lisp-end-comment
	(setq prefix-string "")
	(setq left-margin 1)
	(setq right-margin 9999)
    )
)
(defun
    (compile-mlisp-expression
	(message "Compiling MLisp expressions...") (sit-for 0)
	(execute-mlisp-buffer)
	(message "Compiling MLisp expressions... done.")
    )
)
(defun
    (compile-mlisp-defun
	~name
	
	(save-window-excursion
	    (if (error-occurred (end-of-line)
		    (re-search-reverse "\\S\\C(defun"))
		(error-message "Cannot find function")
	    )
	    (save-excursion
		(forward-word) (forward-word) (set-mark)
		(backward-word) (setq ~name (region-to-string))
	    )
	    (set-mark)
	    (forward-paren 0)
	    (copy-region-to-buffer "Lisp-mode-defun-compile")
	)
	(message "Compiling MLisp defun for " ~name "...") (sit-for 0)
	(save-window-excursion
	    (switch-to-buffer "Lisp-mode-defun-compile")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (beginning-of-file)
	    (execute-mlisp-buffer)
	)
	(message "Compiling MLisp defun for " ~name "... done.")
    )
)
(defun    
    (forward-sexpr
	(forward-paren 0)
    )
)
(defun    
    (backward-sexpr
	(backward-paren 0)
    )
)
(defun
    (zap-defun
	(save-excursion
	    (end-of-line)
	    (re-search-reverse "\\S\\C(defun")
	    (set-mark)
	    (forward-paren 0)
	    (end-of-line)
	    (forward-character)
	    (region-to-process "lisp")
	)
	(pop-to-buffer "lisp")
	(end-of-file)
    )
)
(defun    
    (lisp-mode
	(use-local-map "Lisp-mode-map")
	(setq mode-string "Lisp")
	(setq logical-tab-size 4)
	(use-abbrev-table "Lisp-mode")
	(use-syntax-table "Lisp-mode")
	(novalue)
    )
)
;
;	initialisation of lisp mode
;    
(save-excursion
    (temp-use-buffer "keymap-hack")
    ; get a set of maps to use
    (define-keymap "Lisp-mode-map")
    (define-keymap "Lisp-mode-ESC-map")
    (define-keymap "Lisp-mode-^X-map")
    (use-local-map "Lisp-mode-map")
    
    (execute-mlisp-file "lispmode.key")
    
    (use-syntax-table "Lisp-mode")
    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "string" "'")
    (modify-syntax-table "string" "\"")
    (modify-syntax-table "comment" ";" "\n")
    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "-+!$%^&=_~:/?*<>")
)
(novalue)
)
