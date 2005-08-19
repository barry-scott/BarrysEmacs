(defun
    (~info-m
	s
	(if (setq s (info-check-menu))
	    (progn
		(save-window-excursion
		    (if (> (+ s 0) (+ 0 (dot)))
			(progn
			    (goto-character s)
			    (if (! (dot-is-visible))
				(error-occurred
				    (line-to-top-of-window)
				    (provide-prefix-argument 3 (scroll-one-line-down))
				)
			    )
			)
		    )
		    (setq s (get-tty-string "Menu item: "))
		)
		(info-goto-menu-item s)
	    )
	    (error-message "This node has no menu.")
	)
    )
)
