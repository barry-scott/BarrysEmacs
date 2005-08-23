; 
; smart-auto-execute.ml
; 
; Copyright (c) 2004-2005 Barry A. Scott
; 
(defun
    (smart-auto-execute
	(save-window-excursion
	    (save-excursion
		(beginning-of-file)
		(if
		    (looking-at "#!.*python")
		    (Python-mode)

		    (looking-at "#!.*perl")
		    (sh-mode)

		    (looking-at "#!.*[a-zA-Z]*sh")
		    (sh-mode)

		    (looking-at "<!DOCTYPE html")
		    (HTML-mode)

		    (looking-at "<html")
		    (HTML-mode)

		    (looking-at "<\?xml")
		    (progn
			; maybe XHTML
			(next-line)
			(if
			    (looking-at "<!DOCTYPE html")
			    (HTML-mode)
			    (looking-at "<html")
			    (HTML-mode)
			    (XML-mode)
			)
		    )
		    ; else
		    (normal-mode)
		)
	    )
	)
    )
)
