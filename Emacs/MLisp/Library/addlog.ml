(defun
    (add-log
	~log-file
	~nls
	(setq ~log-file (arg 1 ": add-log "))
	(setq ~nls "\n\n")
	(save-window-excursion
	    (switch-to-buffer "Log File")
 	    (setq current-buffer-file-name ~log-file)
	    (if (error-occurred (read-file ~log-file))
		(progn
		    (setq ~nls "")
		    (message
			(concat
			    "Creating new Log file \""
			    (current-file-name)
			    "\"")
		    )
		)
	    )
	    (text-mode)
	    (~TM-set-left-margin 4)
	    (~TM-set-right-margin 64)
	    (setq mode-string "Log file edit")
	    (end-of-file)
	    (insert-string ~nls)
	    (insert-string "Entry for ")
	    (insert-string (current-time))
	    (insert-string " made by ")
	    (insert-string (users-login-name))
	    (insert-string " at ")
	    (insert-string (system-name))
	    (newline)(newline)
	    (to-col 4)
	    (recursive-edit)
	    (temp-use-buffer "main")
	    (delete-buffer "Log File")
	)
    )
)
