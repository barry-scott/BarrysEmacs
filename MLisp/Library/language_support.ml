(progn
    (declare-global language-loaded)
    (setq language-loaded 1)
    (if (! (is-bound template-loaded))
	(execute-mlisp-file "template_driver")
    )
    (if (! (is-bound users-name))
	(progn
	    (declare-global users-name)
	    (setq users-name "")
	)
    )
    (declare-buffer-specific
	~language-convert-keyword
        ~language-comment-type
	language-keyword-case
    )
    (setq-default ~language-convert-keyword "(progn)")
    (setq-default language-keyword-case "as template")
    (error-occurred
	(extend-database-search-list "language" "emacs$library:emacslang"
	    (if (is-bound library-access)
		library-access
		0
	    )
	)
    )
    (error-occurred
	(extend-database-search-list "language" "sys$login:language-templates"
	    (if (is-bound library-access)
		library-access
		0
	    )
	)
    )
    (defun language-forward-bracket () (forward-paren 0))
    (defun language-backward-bracket () (backward-paren 0))
    (defun language-insert-if-comment (~language-iic-comment "" ~language-iic-nocomment "")
	(insert-string
	    (if (= (current-syntax-entity) is-comment)
		~language-iic-comment
		~language-iic-nocomment
	    )
	)
    )
    (defun language-exit-recursive-edit ()
        (if (recursion-depth) (exit-emacs) (error-message "You are already at top level"))
    )
    (defun language-comment-type ()
	(setq ~language-comment-type (following-char))
    )
    (defun language-check-comment-type ()
        (if (!= ~language-comment-type '\n')
	    (progn
		(delete-white-space)
		(delete-previous-character)
		(if (!= (preceding-char) ' ') (insert-character ' '))
	    )
	)
    )
    (defun language-indent ()
        (to-col (+ (current-column) logical-tab-size))
	(if (= (following-char) ' ') (delete-next-character))
    )
    (defun language-template-driver (language-td-prefix language-td-buf language-td-exit)
	language-td-data
	language-td-mode
	language-saved-exit
	language-td-name
	(setq language-td-data (concat language-td-prefix "-" language-td-buf "-" language-td-prefix))
	(setq language-td-name (concat language-td-prefix "-" language-td-buf))
	(save-window-excursion
	    (if (error-occurred (use-old-buffer language-td-data))
		(progn
		    (switch-to-buffer language-td-data)
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (fetch-database-entry "language" language-td-name)
		    (end-of-file)
		    (if (= (preceding-char) '\n') (delete-previous-character))
		    (setq buffer-is-modified 0)
		    (setq mode-line-format "-%m template-")
		    (setq mode-string "Language")
		)
	    )
	)
	(setq language-td-mode mode-string)
	(setq mode-string (concat language-td-prefix " " language-td-buf))
	(error-occurred (language-set-exit))
	(template-driver language-td-data)
	(error-occurred (language-reset-exit))
	(setq mode-string language-td-mode)
	(error-occurred
	    (if (!= language-exit-point 0)
		(goto-character language-exit-point)
	    )
	)
	(if language-td-exit (exit-emacs))
    )
    (defun language-fetch-mandatory (language-fm-prompt)
	language-fm-count
        (while (&
	  	  (<= (- template-end-field template-start-field) 1)
	  	  (< language-fm-count 2)
	       )
	    (message language-fm-prompt)
	    (recursive-edit)
	    (if (= language-fm-count 0)
		(setq language-fm-prompt
		    (concat
		        "This field is mandatory. "
		        language-fm-prompt
		    )
		)
	    )
	    (setq language-fm-count (+ language-fm-count 1))
        )
        (goto-character template-end-field)
    )
    (defun language-fetch-comment (
	language-fc-prompt ""
	language-fc-mandatory 1
	language-fc-comment "!\t"
	language-fc-cols 2)
	language-fc-left-margin
	language-fc-right-margin
	language-fc-prefix-string
	language-fc-cr
	(setq language-fc-cr (local-binding-of "\r"))
        (local-bind-to-key "language-cr" "\r")
	(setq language-fc-prefix-string prefix-string)
	(setq language-fc-left-margin left-margin)
	(setq language-fc-right-margin right-margin)
	(setq left-margin
	    (save-window-excursion
	        (provide-prefix-argument language-fc-cols (backward-character))
		(current-column)
	    )
	)
	(setq right-margin 76)
	(setq prefix-string language-fc-comment)
	(if language-fc-mandatory
	    (language-fetch-mandatory language-fc-prompt)
	    (language-fetch-optional language-fc-prompt)
	)
	(if (!= language-fc-cr "nothing")
	    (local-bind-to-key language-fc-cr "\r")
	    (remove-local-binding "\r")
	)
	(setq left-margin language-fc-left-margin)
	(setq right-margin language-fc-right-margin)
	(setq prefix-string language-fc-prefix-string)
    )
    (defun language-cr ()
        (newline)
	(to-col left-margin)
	(insert-string prefix-string)
    )
    (defun language-erase-empty-entity (language-eee-lines 0 language-eee-replace "!\tNONE" language-eee-extra 0)
	(if (= (- (dot) language-start-entity) language-eee-lines)
	    (progn
		(save-window-excursion
		    (set-mark)
		    (error-occurred (goto-character language-start-entity))
		    (erase-region)
		    (provide-prefix-argument language-eee-extra (delete-previous-character))
		)
	    (insert-string language-eee-replace)
	    )
	)
    )
    (defun language-optional-entity (language-oe-prompt language-oe-extra 0)
	(message language-oe-prompt)
	(recursive-edit)
	(if (<= (- template-end-field template-start-field) 1)
	    (save-window-excursion
		(goto-character (+ language-oe-extra (- template-end-field 1)))
		(set-mark)
		(error-occurred (goto-character language-start-entity))
		(erase-region)
		(template-exit)
	    )
	)
    )
    (defun language-opt-cont-entity (language-oce-prompt language-oce-extra 0)
	(message language-oce-prompt)
	(recursive-edit)
	(if (<= (- template-end-field template-start-field) 1)
	    (save-window-excursion
		(goto-character (+ language-oce-extra (- template-end-field 1)))
		(set-mark)
		(error-occurred (goto-character language-start-entity))
		(erase-region)
	    )
	)
    )
    (defun
	(language-make-blank-line
	    (if (save-excursion (beginning-of-line) (looking-at "^\\W$"))
	        (if
		    (! (save-excursion
		        (previous-line)
			(beginning-of-line)
			(looking-at "^\\W$")
		    ))
		    (progn
			(end-of-line)
			(newline-and-indent)
		    )
		)
		(progn
		    (end-of-line)
		    (newline-and-indent)
		    (newline-and-indent)
		    (save-excursion
		        (previous-line)
			(delete-white-space)
		    )
		)
	    )
	)
        (language-insert-separator
	    (if (! (provide-prefix-argument -1 (looking-at "\\W\\=")))
	        (insert-string " ")
	    )
	)
        (language-keyword-convert
	    (execute-mlisp-line ~language-convert-keyword)
	)
	(language-set-keyword-case
	    (setq ~language-convert-keyword
		(if (= language-keyword-case "lower") "(case-word-lower)"
		    (= language-keyword-case "upper") "(case-word-upper)"
		    (= language-keyword-case "capital") "(case-word-capitalize)"
		    "(progn)"
		)
	    )
	)
        (insert-users-name
	    (if (<= (length users-name) 0)
	        (setq users-name
		    (concat
		        (get-tty-string "What ar your initials? ")
		        " "
		        (users-login-name)
		    )
		)
	    )
	    (insert-string users-name)
	)
        (VAX/VMS-date
	    EMACS-date
	    (setq EMACS-date (current-time))
	    (insert-string
	        (concat
		    (substr EMACS-date 9 2) "-"
		    (substr EMACS-date 5 3) "-"
		    (substr EMACS-date -4 4)
	        )
	    )
	)
	(language-start-entity
	    (error-occurred
		(setq language-start-entity (dot))
	    )
	)
	(language-fetch-optional
	    language-fetch-optional-del-count
	    (message (arg 1))
	    (recursive-edit)
	    (if (& (= (+ 1 template-start-field) template-end-field) (> (nargs) 1))
		(progn
		    (goto-character template-start-field)
		    (setq language-fetch-optional-del-count (arg 2))
		    (provide-prefix-argument language-fetch-optional-del-count (delete-previous-character))
		)
	    )
	    (goto-character template-end-field)
	)
	(language-exit-dot
	    (error-occurred
		(setq language-exit-point (dot))
	    )
	)
    )
)
