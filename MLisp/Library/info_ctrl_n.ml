(defun
	(~info-^n
	    old-file
	    new-node
	    (setq new-node
		(arg 1 "New node is ")
	    )
	    (save-window-excursion
		(switch-to-buffer "--INFO--")
		(erase-buffer)
		(setq ~info-help-part 0)
		(setq ~info-node-part "TOP")
		(setq ~info-file-part ~info-current-file)
		(insert-string (concat new-node "\n"))
		(beginning-of-file)
		(get-node-and-file)
	    )
	    (setq old-file ~info-current-file)
	    (if ~info-help-part
		(error-message "You cannot insert entries into HELP libraries.")
	    )
	    (setq ~info-file-part (expand-file-name ~info-file-part))
	    (if (!= ~info-current-file ~info-file-part)
		(progn
		    (extend-database-search-list "info" ~info-file-part)
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "INFO edit")
		(setq mode-line-format
		    "[INFO edit   File: %f   Node: %m  --%p%*--]"
		)
		(setq current-buffer-macro-name ~info-file-part)
		(setq mode-string ~info-node-part)
		(use-local-map "INFO-^R")
		(widen-region)
		(erase-buffer)
		(beginning-of-file)
		(setq ~info-^n-file ~info-current-file)
		(setq ~info-^n-node ~info-current-node)
		(setq ~info-^n-help-flag ~info-help-library)
		(setq ~info-current-file ~info-file-part)
		(recursive-edit)
		(switch-to-buffer "INFO edit")
		(while buffer-is-modified
		    (if (c=
			    'y'
			    (string-to-char
				(get-tty-string "You have modified this node. Do you really want to exit? ")
			    )
			)
			(setq buffer-is-modified 0)
			(progn
			    (recursive-edit)
			    (switch-to-buffer "INFO edit")
			)
		    )
		)
	    )
	    (if (!= ~info-current-file old-file)
		(progn
		    (remove-database "info" ~info-current-file)
		)
	    )
	    (setq ~info-current-file old-file)
	    (find-node ~info-^n-file ~info-^n-node ~info-^n-help-flag)
	)
)
