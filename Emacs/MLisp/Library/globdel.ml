(progn
(defun
    (global-delete-lines
	(re-global-delete-lines (quote (arg 1 ": global-delete-lines ")))
    )
)
(defun
    (re-global-delete-lines target delcnt
	(setq delcnt 0)
	(setq target (arg 1 ": re-global-delete-lines containing "))
	(save-excursion
	    (error-occurred
		(beginning-of-file)
		(while 1
		    (re-search-forward target)
		    (beginning-of-line)
		    (kill-to-end-of-line)
		    (kill-to-end-of-line)
		    (setq delcnt (+ delcnt 1))
		)
	    )
	)
	(message (concat delcnt " lines deleted"))
    )
)
)
