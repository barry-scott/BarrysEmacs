(defun
    (~helpchar-variable
	~helpchar-variable
	(setq ~helpchar-variable (get-tty-variable "Documentation for variable? "))
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (delete-other-windows)

	    (extend-database-search-list "describe" "emacs_library:emacsdesc")
	    (fetch-database-entry "describe"  (concat "V" ~helpchar-variable))

	    (use-local-map "~helpchar-view-prefix")
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (setq mode-string (~helpchar-more))

	    (beginning-of-file)
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
