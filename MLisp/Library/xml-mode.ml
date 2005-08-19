(declare-global ~XML-debug ~XML-msg-level)

(defun
    (XML-mode
	(setq mode-string "XML")
	(use-syntax-table "XML")
	(use-local-map "XML-map")
	(use-abbrev-table "XML")
	(novalue)
    )
)

(defun
    (XML-view-buffer
	(if buffer-is-modified
	    (write-current-file)
	)
	(WIN-HTML-command current-buffer-file-name)
    )
)

(defun
    (~XML-log
	(if (>= ~XML-debug (arg 1))
	    (progn
		(if (> (+ (arg 2)) 0)
		    (setq ~XML-msg-level (+ ~XML-msg-level (arg 2)))
		)

		(save-window-excursion
		    ~index
		    ~msg
		    (setq ~index 3)
		    (setq ~msg "")
		    (while (<= ~index (nargs))
			(setq ~msg (concat ~msg (arg ~index)))
			(setq ~index (+ 1 ~index))
		    )
		    
		    (pop-to-buffer "XML-log")
		    (end-of-file)
		    (setq ~index 0)
		    (while (< ~index ~XML-msg-level)
			(setq ~index (+ ~index 1))
			(insert-string "     ")
		    )
		    (insert-string ~msg "\n")
		    (end-of-file)
		)
		
		(if (< (+ (arg 2)) 0)
		    (setq ~XML-msg-level (+ ~XML-msg-level (arg 2)))
		)
	    )
	)
	(novalue)
    )
)

(defun
    ~XML-setup()
    (save-window-excursion
	(temp-use-buffer "~mode-hack")
	(use-syntax-table "XML")
	(modify-syntax-table "word" "-_")
	
	(modify-syntax-table "comment" ">" "<")
	(modify-syntax-table "string" "\"")	
	
	(define-keymap "XML-map")
	(use-local-map "XML-map")
	
	(execute-mlisp-file "xml-mode.key")

	(kill-buffer "~mode-hack")
	(novalue)
    )
)

(defun
    (~XML-tag-info-common
	(save-restriction
	    (narrow-region)
	    (beginning-of-file)
	    (if
		; is it </tag> ?
		(ere-looking-at "^\\s*/")
		(setq ~tag-type ">")
		; is if <tag/> ?
		(ere-looking-at ".*/\\s*$")
		(setq ~tag-type "<>")
		; it should be <tag ...>
		(setq ~tag-type "<")
	    )
	    (ere-looking-at "\\s*(/\\s*)?([^ /]+)")
	    (region-around-match 2)
	    (setq ~tag-name (region-to-string))
	    (if (= (string-extract ~tag-name 0 1) "?")
		; it is a processing instruction
		(setq ~tag-type "?")
	    )
	)
    )
)

(defun
    (~XML-tag-info-forward
	~start-line
	
	(save-excursion
	    (search-forward ">")
	    (setq ~tag-end (dot))
	    (backward-character)
	    (set-mark)
	    
	    (search-reverse "<")
	    (setq ~tag-start (dot))
	    (setq ~start-line (current-line-number))
	    (forward-character)
	    (~XML-tag-info-common)
	)	    
	(~XML-log 2 0 "~XML-tag-info-forward " ~tag-name " " ~tag-type " " (+ ~tag-start) ":" (+ ~tag-end) " line " ~start-line)
    )
)

(defun
    (~XML-tag-info-backward
	~start-line
	
	(save-excursion
	    (search-reverse "<")
	    (setq ~tag-start (dot))
	    (setq ~start-line (current-line-number))
	    (forward-character)
	    (set-mark)
	    
	    (search-forward ">")
	    (setq ~tag-end (dot))
	    (backward-character)
	    (~XML-tag-info-common 0)
	)
	(~XML-log 2 0 "~XML-tag-info-backward " ~tag-name " " ~tag-type " " (+ ~tag-start) ":" (+ ~tag-end) " line " ~start-line)
    )
)

(defun XML-next-tag()
    (goto-character
	(save-excursion
	    (search-forward ">")
	    (dot)
	)
    )
)

(defun XML-previous-tag()
    (goto-character
	(save-excursion
	    (search-forward "<")
	    (dot)
	)
    )
)

(defun ~XML-matching-tag-forward()
    ~tag-name-start
    ~tag-name-end
    
    (~XML-log 1 1 "--> ~XML-match-tag-forward start ")
    
    (goto-character
	(save-excursion
	    ~tag-name
	    ~tag-type
	    ~tag-start
	    ~tag-end
	    
	    (~XML-tag-info-forward)
	    
	    (setq ~tag-name-start ~tag-name)
	    (setq ~tag-name-end "")
	    
	    (~XML-log 1 0 "--- ~XML-match-tag-forward start " ~tag-name-start " " ~tag-type " line=" (current-line-number) " col=" (current-column))
	    
	    (if
		(= ~tag-type "<>")
		(progn
		    (setq ~tag-name-end ~tag-name)
		    (goto-character ~tag-end)
		)
		(= ~tag-type "?")
		(progn
		    (setq ~tag-name-end ~tag-name)
		    (goto-character ~tag-end)
		)
		(= ~tag-type ">")
		(progn
		    (setq ~tag-name-start "")
		    (setq ~tag-name-end ~tag-name)
		    (goto-character ~tag-end)
		)
		; else = "<" move forward matched tags
		(progn
		    ; go past the start tag
		    (goto-character ~tag-end)
		    
		    (while
			(progn
			    (~XML-tag-info-forward)
			    (if
				(= "<" ~tag-type)
				(progn
				    ; continue if we find a match for this
				    ; opening tag
				    (~XML-matching-tag-forward)
				)
				(= "<>" ~tag-type)
				(progn
				    ; continue for the open-close tag
				    (goto-character ~tag-end)
				    1
				)
				(= "?" ~tag-type)
				(progn
				    ; continue for the open-close tag
				    (goto-character ~tag-end)
				    1
				)
				; else = ">"
				(progn
				    ; break out on a closing tag and record the end tag
				    (setq ~tag-name-end ~tag-name)
				    (goto-character ~tag-end)
				    0
				)
			    )
			)
			(novalue)
		    )
		)
	    )
	    
	    (dot)
	)
    )
    (~XML-log 1 -1 "<-- ~XML-match-tag-forward end " ~tag-name-start ":" ~tag-name-end " dot=" (+ (dot)) " line=" (current-line-number) " col=" (current-column))
    (= ~tag-name-start ~tag-name-end)
)

(defun XML-matching-tag-forward()
    ~tag-name
    ~tag-type
    ~tag-start
    ~tag-end
    
    (setq ~XML-msg-level 0)
    (~XML-log 1 1 ">>> XML-matching-tag-forward")

    (~XML-tag-info-forward)

    (if (= ~tag-type "<")
	(if (! (~XML-matching-tag-forward))
	    (error-message "XML tag " ~tag-name " not matched")
	)
	(~XML-matching-tag-forward)
    )

    (~XML-log 1 -1 ">>> XML-matching-tag-forward")
    (novalue)
)

(defun ~XML-matching-tag-backward()
    ~tag-name-start
    ~tag-name-end
    
    (~XML-log 1 1 "--> ~XML-matching-tag-backward end line=" (current-line-number) " col=" (current-column))

    (goto-character
	(save-excursion
	    ~tag-name
	    ~tag-type
	    ~tag-start
	    ~tag-end

	    (~XML-tag-info-backward)
	    
	    (setq ~tag-name-start "")
	    (setq ~tag-name-end ~tag-name)
	    
	    (~XML-log 1 0 "--- ~XML-matching-tag-backward end " ~tag-name-end " " ~tag-type " line=" (current-line-number) " col=" (current-column))
	    
	    (if
		(= ~tag-type "<>")
		(progn
		    (setq ~tag-name-start ~tag-name)
		    (goto-character ~tag-start)
		)
		(= ~tag-type "?")
		(progn
		    (setq ~tag-name-start ~tag-name)
		    (goto-character ~tag-start)
		)
		(= ~tag-type "<")
		(progn
		    (setq ~tag-name-end "")
		    (setq ~tag-name-start ~tag-name)
		    (goto-character ~tag-start)
		)
		; = ">"
		(progn
		    ; move backward matched tags
		    (goto-character ~tag-start)
		    (while
			(progn
			    (~XML-tag-info-backward)
			    (if
				(= ">" ~tag-type)
				(progn
				    ; continue if matching tag found
				    (~XML-matching-tag-backward)
				)
				(= "<>" ~tag-type)
				(progn
				    (goto-character ~tag-start)
				    1
				)
				(= "?" ~tag-type)
				(progn
				    (goto-character ~tag-start)
				    1
				)
				; else = "<"
				(progn
				    (setq ~tag-name-start ~tag-name)
				    (goto-character ~tag-start)
				    0
				)
			    )
			)
			(novalue)
		    )
		    (setq ~tag-name-start ~tag-name)
		)
	    )
	    
	    (dot)
	)
    )
    (~XML-log 1 -1 "<-- ~XML-matching-tag-backward start " ~tag-name-start " line=" (current-line-number) " col=" (current-column))
    (= ~tag-name-start ~tag-name-end)
)

(defun XML-matching-tag-backward()
    ~tag-name
    ~tag-type
    ~tag-start
    ~tag-end
    
    (setq ~XML-msg-level 0)
    (~XML-log 1 1 ">>> XML-matching-tag-backward")

    (~XML-tag-info-backward)

    (if (= ~tag-type ">")
	(if (! (~XML-matching-tag-backward))
	    (error-message "XML tag " ~tag-name " not matched")
	)
	(~XML-matching-tag-backward)
    )
    (~XML-log 1 -1 "<<< XML-matching-tag-backward")
    (novalue)
)

(~XML-setup)
