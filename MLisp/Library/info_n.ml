(defun
	(~info-n
	    (beginning-of-file)
	    (end-of-line)
	    (if (error-occurred (re-search-reverse "Next:[ \t]*"))
		(error-message "This node has no Next.")
	    )
	    (re-search-forward "")
	    (setq ~info-node-part "TOP")
	    (setq ~info-file-part "")
	    (get-node-and-file)
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
