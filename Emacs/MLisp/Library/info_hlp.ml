(defun
	(~info-hlp
	    (if (error-occurred
		    (find-node
			(concat
			    info-default-directory
			    info-root-file
			)
			"CMDS"
			0
		    )
		)
		(error-message "The command summary is not available.")
	    )
	)
)
