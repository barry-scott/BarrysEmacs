(progn
; 
; DCL-mode
; 
; 	package to help write DCL command procedures
; 
; Barry A. Scott	 7-Jun-1983
;
(defun
    (DCL-mode
	(remove-all-local-bindings)

	(local-bind-to-key "electric-DCL-!" "!")
	(local-bind-to-key "electric-DCL-newline" "\^j")

	(setq mode-string "DCL")
	(use-abbrev-table "DCL-mode")
	(use-syntax-table "DCL-mode")
	(novalue)
    )
)

(defun
    (electric-DCL-!
	(end-of-line) (delete-white-space)
	(if (= (current-column) 1)
	    (insert-string "$	!	"); comment on empty line
	    
	    (progn
		(beginning-of-line)
		(if (looking-at "^$[	 ]*$")
		    (progn
			(end-of-line)
			(insert-string "	!	")
		    )
		    (progn
			(end-of-line)
			(if (< (current-column) comment-column)
			    (progn
				(to-col comment-column)
				(insert-string "! ")
			    )
			    
			    (insert-string "	! ")
			)
		    )
		)
	    )
	)
    )
)    
(defun
    (electric-DCL-newline indent-level $-needed
	; 
	; Find the indent level
	; 
	(delete-white-space)
	(newline)
	(previous-line)
	(if (= (following-char) '$')
	    (progn
		(setq $-needed 1)
		(delete-next-character)
		(setq indent-level (current-indent))
		(insert-character '$')
	    )
	    (progn
		(setq $-needed 0)
		(setq indent-level (current-indent))
	    )
	)
	(end-of-line)
	(if (& $-needed (!= (preceding-char) '-'))
	    (progn
		(forward-character)
		(insert-character '$')
		(to-col indent-level)
	    )
	    (progn
		(forward-character)
		(to-col indent-level)
	    )
	)
    )
)
    
(use-syntax-table "DCL-mode")
(modify-syntax-table "paren" "(" ")")
(modify-syntax-table "string" "\"")
(modify-syntax-table "comment" "!" "\n")
(modify-syntax-table "word" "$%^_~*")
(novalue)
)
