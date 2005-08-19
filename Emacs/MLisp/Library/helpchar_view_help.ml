(defun
    (~helpchar-view-help
	x
	(save-window-excursion
	    (switch-to-buffer "Help view")
	    (erase-buffer)
	    (delete-other-windows)
	    (setq mode-line-format "")
	    (insert-string
		(concat
		    "HELP: on Emacs in help view mode\n\n"
		    "    Key        Meaning\n"
		    "    ---        -------\n"
		    "    <Space>	Move forward one page\n"
		    "    <Bs>	Move backward one page\n"
		    "    b		Move to the beginning of the help\n"
		    "    e		Move to the end of the help\n"
		    "    q		Continue editting\n"
		    "    ^C		Continue editting\n"
		    "    ?		Display this help text\n"
		)
	    )
	    (beginning-of-file)
	    (~helpchar-prompt "-- Type any character to continue HELP --")
	    (setq x (get-tty-character))
	    (if (| (= x ' ') (= x '^H') (= x 'b') (= x 'e') (= x 'q')
		    (= x '^C') (= x '^['))
		(push-back-character x)
	    )
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	)
    )
)
