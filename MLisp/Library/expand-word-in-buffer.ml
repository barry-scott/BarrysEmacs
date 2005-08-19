; 
; expand-word-in-buffer.ml
;
;   Improve to only expand the word that the cursor is on.
; 
(defun ~expand-generic-word-in-buffer
    (
	~type
	~prompt
    )
    ~word
    (save-excursion
	;
	; make sure that there is a word here
	(if (looking-at "\\w")
	    (progn
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
	    )
	    (progn
		(error-occurred (backward-character))
		(if (looking-at "\\w")
		    (progn
			(error-occurred (forward-character))
			(backward-word)
			(set-mark)
			(forward-word)
		    )
		    (set-mark)
		)
	    )
	)
	(setq ~word (region-to-string))
	(push-back-character ~word)
	(setq ~word
	    (execute-mlisp-line
		(concat
		    "(get-tty-" ~type " \": expand-" ~prompt "-in-buffer \")"
		)
	    )
	)
	(erase-region)
    )
    (insert-string ~word)
    (novalue)
)    
(defun
    (expand-mlisp-word-in-buffer
	(~expand-generic-word-in-buffer "command" "mlisp-word")
    )
)
(defun
    (expand-buffer-in-buffer
	(~expand-generic-word-in-buffer "buffer" "buffer")
    )
)
(defun
    (expand-file-in-buffer
	(~expand-generic-word-in-buffer "file" "file")
    )
)
(defun
    (expand-mlisp-variable-in-buffer
	(~expand-generic-word-in-buffer "variable" "mlisp-variable")
    )
)
