(defun
    (~helpchar-help
	x
	(save-window-excursion
	    (switch-to-buffer "Help h window")
	    (erase-buffer)
	    (delete-other-windows)
	    (setq mode-line-format "")
	    (insert-string
		(concat
		    "HELP: on Emacs\n\n"
		    "    Key        Meaning\n"
		    "    ---        -------\n"
		    "    a          Show commands and variables whoes name\n"
		    "               containing a string using apropos\n"
		    "    b          Describe all key bindings\n"
		    "    c          LK201 compose key sequence chart\n"
		    "    d          Describe a command\n"
;		    "    h          Access to VAX/VMS help\n"
;		    "    i          Access INFO\n"
		    "    k          Describe a key binding\n"
;		    "    n          Print out Emacs news\n"
		    "    v          Describe a variable\n"
		    "    w          Find a function on the keyboard\n"
		    "    ?          Display this help text\n"
		)
	    )
	    (beginning-of-file)
	    (message "-- Type any character to continue HELP --")
	    (setq x (get-tty-character))
	    (if (| (= x 'a') (= x 'd') (= x 'i') (= x 'n') (= x 'v')
		    (= x 'h') (= x 'k') (= x 'b') (= x 'c') (= x 'w'))
		(push-back-character x)
	    )
	    (help-key)
	)
    )
)
