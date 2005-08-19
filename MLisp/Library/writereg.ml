(defun
    (write-region-to-file fname
	(setq fname
	    (if (interactive)
		(get-tty-file ": write-region-to-file ")
		(arg 1 ": write-region-to-file ")
	    )
	)
	(save-excursion
	    (copy-region-to-buffer "write-temp")
	    (temp-use-buffer "write-temp")
	    (write-named-file fname)
	    (kill-buffer "write-temp")
	)
	(novalue)
    )
)
