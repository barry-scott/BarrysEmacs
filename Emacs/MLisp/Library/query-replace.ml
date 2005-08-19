; 
;   query-replace.ml
;
(defun
    ere-query-replace-string(~old-string (get-tty-string "ERE old pattern: ")
			  ~new-string (get-tty-string "New string "))
    
    (~query-replace-string-helper ~old-string ~new-string "ere-query-replace-string" "ere-search-forward" "re-replace-search-text" "ere-replace-string")
)

(defun
    re-query-replace-string(~old-string (get-tty-string "RE old pattern: ")
			  ~new-string (get-tty-string "New string "))
    
    (~query-replace-string-helper ~old-string ~new-string "re-query-replace-string" "re-search-forward" "re-replace-search-text" "re-replace-string")
)

(defun
    query-replace-string(~old-string (get-tty-string "Old string: ")
			      ~new-string (get-tty-string "New string: "))

    (~query-replace-string-helper ~old-string ~new-string "query-replace-string" "search-forward" "replace-search-text" "replace-string")

)
(defun
    ~query-replace-string-helper(~old-string ~new-string ~title ~search-cmd ~replace-cmd ~search-and-replace-cmd)
    
    ~char
    ~query
    ~replaced
    ~can-replace
    ~did-not-replace
    ~cannot-continue-searching
    
    (save-excursion
	(execute-mlisp-line (concat "(" ~search-cmd " ~old-string)" ))
	
	(setq ~replaced 0)
	(setq ~query 1)
	(while ~query
	    (setq ~can-replace 1)
	    (setq ~did-not-replace 1)
	    (setq ~cannot-continue-searching 1)
	    (while ~cannot-continue-searching
		(message ~title " \"" ~old-string "\" with \"" ~new-string "\" [replaced " ~replaced "] command?")
		(setq ~char (get-tty-character))
		
		(if (|
			(= ~char ' ')
			(= ~char 'y')
		    )
		    (progn
			(if ~can-replace
			    (progn
				(setq ~replaced (+ ~replaced 1))
				(execute-mlisp-line (concat "(" ~replace-cmd " ~new-string)" ))
				(setq ~did-not-replace 0)
			    )
			)
			(setq ~can-replace 0)
			(setq ~cannot-continue-searching 0)
		    )
		    
		    (= ~char '.')
		    (progn
			(if ~can-replace
			    (progn
				(setq ~replaced (+ ~replaced 1))
				(execute-mlisp-line (concat "(" ~replace-cmd " ~new-string)" ))
				(setq ~did-not-replace 0)
			    )
			)
			(setq ~can-replace 0)
			(setq ~cannot-continue-searching 0)
			(setq ~query 0)
		    )
		    
		    (= ~char 'n')
		    (progn
			(setq ~can-replace 0)
			(setq ~cannot-continue-searching 0)
		    )
		    
		    (= ~char ',')
		    (progn
			(if ~can-replace
			    (progn
				(setq ~replaced (+ ~replaced 1))
				(execute-mlisp-line (concat "(" ~replace-cmd " ~new-string)" ))
				(setq ~did-not-replace 0)
			    )
			)
			(setq ~can-replace 0)
		    )
		    
		    (|
			(= ~char 'r')
			(= ~char 'p')
		    )
		    (save-window-excursion
			(message "Invoke exit-emacs to return to " ~title)
			(recursive-edit)
		    )
		    
		    (|
			(= ~char '!')
			(= ~char 'a')
		    )
		    (progn
			(execute-mlisp-line (concat "(" ~replace-cmd " ~new-string)" ))
			(setq ~can-replace 0)
			(setq ~cannot-continue-searching 0)
			(setq ~replaced (+ ~replaced 1))
			(setq ~replaced
			    (+ ~replaced
				(execute-mlisp-line
				    (concat "(" ~search-and-replace-cmd " ~old-string ~new-string)" ))))
		    )
		    
		    (= ~char 'q')
		    (progn
			(setq ~query 0)
			(setq ~cannot-continue-searching 0)
			
		    )
		    
		    (|
			(<= ~char 31)
			(& (>= ~char 128) (<= ~char 191))
		    )
		    (progn
			(push-back-character ~char)
			(setq ~query 0)
			(setq ~cannot-continue-searching 0)
		    )
		    
		    ; default is to show help text
		    (~query-replace-help)
		)
	    )
	    (if ~query
		(progn
		    ~start-dot
		    ; remember the start position
		    (setq ~start-dot (dot))
		    (if (error-occurred (execute-mlisp-line (concat "(" ~search-cmd " ~old-string)" )))
			(setq ~query 0)
			; else
			(if
			    (&
				~did-not-replace    ; if a replace happened then its ok to not move 
				(= (dot) ~start-dot)
			    )
			    ; Oh the search did not move to the next
			    (progn
				(if (error-occurred
					(forward-character)
					(execute-mlisp-line (concat "(" ~search-cmd " ~old-string)" ))
				    )
				    (setq ~query 0)
				)
			    )
			)
		    )
		)
	    )
	)
    )
    (message "Replaced " ~replaced " times")
)
(defun
    (~query-replace-help
	(save-window-excursion
	    (switch-to-buffer "~query-help")
	    (erase-buffer)
	    (insert-string
		"query replace help\n"
		"\n"
		"    y, SPACE - replace and find next\n"
		"    n        - don't replace and find next\n"
		"    ,        - replace\n"
		"    .        - replace and quit\n"
		"    a, !     - replace this and all others\n"
		"    r        - pause in a recursive edit use ^X-^C to continue\n"
		"    p        - pause in a recursive edit use ^X-^C to continue\n"
		"    q        - quit\n"
		"\n"
		"Press Space to continue\n"
	    )
	    (beginning-of-file)
	    (get-tty-character)
	)
	(delete-buffer "~query-help")
    )
)
