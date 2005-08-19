(defun
	(~helpchar-describe-bindings
	    (save-window-excursion
		(describe-bindings)
		(switch-to-buffer "Help window")
		(erase-buffer)
		(delete-other-windows)
		(yank-buffer "Help")
		(beginning-of-file)
		(use-local-map "~helpchar-view-prefix")
		(~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
		(setq mode-string (~helpchar-more))
		(recursive-edit)
		(use-local-map "~helpchar-prefix")
	    )
	)
)
