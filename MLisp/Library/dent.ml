; 
; Copyright 1996 Barry A. Scott
; 
; indent the region by logical-tab-size units
; and do it prefix-argument times.
; if the regionis not set then indent the current line
; note:
; 	If the argument is 0 this cleans up the tabs and spaces on
; 	the beginnings of lines.
; e.g
; "<space><tab>hello" => "<tab>hello"
; 
(defun
    (indent-region
	~indent-level		; amount of white space infront of line
	
	(if (error-occurred (mark))
	    (save-excursion
		(beginning-of-line)
		(set-mark)
		(end-of-line)
		(indent-region)
	    )
	    (save-excursion
		(message "Indenting region...") (sit-for 1)
		(save-excursion
		    (save-restriction
			(narrow-region)
			(unset-mark)
			(beginning-of-file)
			(while (! (eobp))
			    (if (! (eolp))
				(progn
				    (setq ~indent-level (current-indent))
				    (delete-white-space)
				    (to-col
					(+
					    ~indent-level
					    (* logical-tab-size prefix-argument)
					)
				    )
				)
			    )
			    (beginning-of-line)
			    (next-line)
			    (if (! (eobp))
				(beginning-of-line)
			    )
			)
		    )
		)
		(message "                  ...region Indented")
	    )
	)
    )
)

(defun    
    (undent-region
	~indent-level
	
	(if (error-occurred (mark))
	    (save-excursion
		(beginning-of-line)
		(set-mark)
		(end-of-line)
		(undent-region)
	    )
	    (save-excursion
		(message "Undenting region...") (sit-for 1)
		(save-excursion
		    (save-restriction
			(narrow-region)
			(unset-mark)
			(beginning-of-file)
			(while (! (eobp))
			    (setq ~indent-level (current-indent))
			    (delete-white-space)
			    (to-col
				(-
				    ~indent-level
				    (* logical-tab-size prefix-argument)
				)
			    )
			    (beginning-of-line)
			    (next-line)
			    (if (! (eobp))
				(beginning-of-line)
			    )
			)
		    )
		)
		(message "                  ...region Undented")
	    )
	)
    )
)

; 
; Work out the indent values used to create the text in the current buffer
; 
(defun
    (auto-set-indent
	(save-excursion
	    ~tab-found
	    ~min-indent
	    
	    ; are TABs in use in the buffer?
	    (beginning-of-file)
	    (setq ~tab-found (! (error-occurred (ere-search-forward "^\t"))))
	    
	    ; find the indent levels that are in use
	    (beginning-of-file)
	    (setq ~min-indent 65)
	    (while (! (error-occurred (ere-search-forward "^ {2,}")))
		(if (< (current-column) ~min-indent)
		    (setq ~min-indent (current-column))
		)
	    )
	    
	    (if (= ~min-indent 65)
		(message "auto-set-indent leaving indent settings unchanged")
		(progn
		    ~msg
		    (setq ~msg "")
		    (if (!= ~tab-found indent-use-tab)
			(progn
			    (setq ~msg
				(concat ~msg "indent-use-tab: " ~tab-found))
			    (setq indent-use-tab ~tab-found)
			)
		    )
		    (setq ~min-indent (- ~min-indent 1))
		    (if (!= ~min-indent logical-tab-size)
			(progn
			    (setq ~msg
				(concat ~msg "logical-tab-size: " ~min-indent))
			    (setq logical-tab-size ~min-indent)
			)
		    )
		    (if (length ~msg)
			(message "auto-set-indent " ~msg)
			(novalue)
		    )
		)
	    )
	)
    )
)
