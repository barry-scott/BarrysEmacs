(defun
	(~info-p
	    (beginning-of-file)
	    (end-of-line)
	    (if (error-occurred (re-search-reverse "Previous:[ \t]*"))
		(error-message "This node has no Previous.")
	    )
	    (re-search-forward "")
	    (setq ~info-node-part "TOP")
	    (setq ~info-file-part "")
	    (get-node-and-file)
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
