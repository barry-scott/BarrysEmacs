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
