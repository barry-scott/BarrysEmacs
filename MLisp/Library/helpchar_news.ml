(defun
    (~helpchar-news
	~helpchar-variable
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (delete-other-windows)
	    (if
		(error-occurred
		    (insert-file "SYS$HELP:EMACSNEWS.TXT")
		)
		(if
		    (error-occurred
			(insert-file "EMACS_LIBRARY:EMACSNEWS.TXT")
		    )
		    (error-message "No news is available.")
		)
	    )
	    (use-local-map "~helpchar-view-prefix")
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (setq mode-string (~helpchar-more))
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
