(progn
    (declare-global justify-right-margin)
    (declare-global justify-left-margin)
    (if
	(= justify-right-margin 0)
	(setq justify-right-margin 78)
    )
    (defun
	(center-line width
	    (save-excursion
		(beginning-of-line)
		(delete-white-space)
		(end-of-line)
		(delete-white-space)
		(setq width (current-column))
		(beginning-of-line)
		(to-col
		    (+
			justify-left-margin
			(/ 
			    (- 
				(- justify-right-margin justify-left-margin)
				 width)
			    2)
		    )
		)
	    )
	)
    )
)
