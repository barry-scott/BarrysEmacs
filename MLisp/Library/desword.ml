(progn
library-access
(if (error-occurred (setq library-access (getenv "EMACS$LIBRARY_ACCESS")))
    (setq library-access 0)
)
(execute-mlisp-file "describe_word_list.database")
(error-occurred
    (extend-database-search-list "subr-names"
	(concat "sys$login:subrnames") library-access
    )
)
(error-occurred
    (extend-database-search-list "subr-names"
	(concat "emacs$user:subrnames") library-access
    )
)

(declare-global describe-word-window-size)
(declare-global describe-word-keep-window)

(save-excursion
    (temp-use-buffer "syntax-table-hack")
    (use-syntax-table "describe-word")
    (modify-syntax-table "dull" "!-~")
    (modify-syntax-table "word" "A-Za-z0-9$_%")
)
(defun
    (describe-word-in-buffer subr-name syntax-tab
	(setq syntax-tab current-buffer-syntax-table)
	(use-syntax-table "describe-word")
	(if (> prefix-argument 1)
	    (save-window-excursion
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table syntax-tab)
		(edit-description (region-to-string))
	    )
	    (progn ~current-windows ~original-buffer
		(setq ~current-windows current-windows)
		(setq ~original-buffer current-buffer-name)
		(save-excursion
		    (error-occurred (forward-character))
		    (backward-word)
		    (set-mark)
		    (forward-word)
		    (use-syntax-table syntax-tab)
		    (setq subr-name (region-to-string))
		)
		(temp-use-buffer "Subroutine-Help")
		(setq current-buffer-checkpointable 0)
		(setq current-buffer-journalled 0)
		(setq mode-line-format "%b: %m")
		(erase-buffer)
		(if (error-occurred
			(fetch-database-entry "subr-names" subr-name))
		    (progn
			(delete-buffer "Subroutine-Help")
			(error-message "No description for " subr-name)
		    )
		    (if (> describe-word-window-size 0)
			(progn
			    (pop-to-buffer "Minibuf")
			    (previous-window)
			    (error-occurred (split-current-window))
			    (switch-to-buffer "Subroutine-Help")
			    (setq mode-string subr-name)
			    (error-occurred (while 1 (shrink-window)))
			    (error-occurred
				(while (<= window-size describe-word-window-size)
				    (enlarge-window)
				)
			    )
			    (beginning-of-file)
			    (if (! describe-word-keep-window)
				(push-back-character (get-tty-character))
			    )
			)
			(progn
			    (beginning-of-file)
			    (set-mark)
			    (end-of-line)
			    (message (region-to-string))
			)
		    )
		)
		(if describe-word-keep-window
		    (pop-to-buffer ~original-buffer)
		    (setq current-windows ~current-windows)
		)
	    )
	)
	(novalue)
    )
)
(defun    
    (edit-description ~edit-name
	(setq ~edit-name (arg 1 ": edit-description (of key) "))
	(pop-to-buffer "Edit description")
	(erase-buffer)
	(if (error-occurred (fetch-database-entry "subr-names" ~edit-name))
	    (message "New entry."))
	(setq mode-string (concat "Editing database entry for " ~edit-name))
	(setq mode-line-format "%[%m%]")
	(local-bind-to-key "exit-emacs" "\^X\^S")
	(local-bind-to-key "exit-emacs" "\^X\^F")
	(local-bind-to-key "exit-emacs" "\^X\^W")
	(recursive-edit)
	(temp-use-buffer "Edit description")
	(put-database-entry "subr-names" ~edit-name)
	(error-occurred (delete-buffer "Edit description"))
	(novalue)
    )
)
)
