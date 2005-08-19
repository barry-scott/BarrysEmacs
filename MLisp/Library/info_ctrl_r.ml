(defun
	(~info-^r
	    (save-window-excursion
		(switch-to-buffer "INFO edit")
		(text-mode)
		(setq mode-line-format
		    "[INFO edit   File: %f   Node: %m  --%p%*--]"
		)
		(use-local-map "INFO-^R")
		(setq current-buffer-macro-name ~info-current-file)
		(setq mode-string ~info-current-node)
		(widen-region)
		(erase-buffer)
		(if ~info-help-library
		    (fetch-database-entry "info" ~info-current-node)
		    (yank-buffer "INFO")
		)
		(beginning-of-file)
		(setq buffer-is-modified 0)
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
	    (setq ~info-node-part ~info-current-node)
	    (setq ~info-current-node "")
	    (find-node ~info-current-file ~info-node-part ~info-help-library)
	)
)
