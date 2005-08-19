(defun
    (~helpchar-describe-key
	(push-back-character "\^Q")
	(message (describe-key (get-tty-string "Doc for keys (<Cr> to terminate)? ")))
    )
)
