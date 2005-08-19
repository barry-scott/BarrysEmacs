(defun
	(~info-g
	    s
	    (setq s (arg 1 "Node: "))
	    (save-window-excursion
		(switch-to-buffer "--INFO--")
		(erase-buffer)
		(setq ~info-file-part (concat info-default-directory info-root-file))
		(setq ~info-node-part "TOP")
		(setq ~info-help-part 0)
		(insert-string (concat s "\n"))
		(beginning-of-file)
		(get-node-and-file)
	    )
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
