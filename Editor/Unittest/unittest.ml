(defun
    log-error(~test-name ~reason)
    (save-excursion 
	(temp-use-buffer "Unittest.log")
	(end-of-file)
	(insert-string "Error: " ~test-name " - " ~reason "\n")
	(write-named-file "Unittest.log")
    )
)

(defun
    log-info(~test-name)
    (save-excursion 
	(temp-use-buffer "Unittest.log")
	(end-of-file)
	(insert-string "Info: Pass test " ~test-name "\n")
	(write-named-file "Unittest.log")
    )
)

(defun
    (unittest
	(switch-to-buffer "Unittest.log")
	(erase-buffer)
	(write-named-file "Unittest.log")
	(log-info "unittest search starting")
	(if (error-occurred (execute-mlisp-file "search") (unittest-search))
	    (log-error "unittest search raised an error" error-message)
	    (log-info "unittest search complete")
	)
    )
)
