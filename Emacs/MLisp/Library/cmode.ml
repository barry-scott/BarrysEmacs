(progn
    (if (! (is-bound c-auto-expansion))
        (progn
	    (declare-buffer-specific c-auto-expansion)
	    (setq-default c-auto-expansion 1)
	)
    )
    (defun
	(c-mode
	    (use-syntax-table "c-mode")
	    (use-abbrev-table "c-mode")
	    (setq abbrev-mode c-auto-expansion)
	    (use-local-map "C-MAP")
	    (setq abbrev-mode 1)
	    (setq mode-string "C")
	    (novalue)
	)
	(~c-abbrev-handler
	    c-ah-current
	    (setq c-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= c-ah-current is-word)
			(= c-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    c-ah-case-fold-search
		    c-ah-looking-at
		    c-ah-data
		    language-start-entity
		    c-last-key-struck
		    language-exit-point
		    (setq c-ah-case-fold-search case-fold-search)
		    (setq case-fold-search 0)
		    (setq c-ah-looking-at
			(provide-prefix-argument -1
			    (looking-at abbrev-expansion)
			)
		    )
		    (setq case-fold-search c-ah-case-fold-search)
		    (if c-ah-looking-at
			(progn
			    (save-window-excursion
				(region-around-match 0)
				(erase-region)
			    )
			    (setq c-last-key-struck (last-key-struck))
			    (language-template-driver "C" abbrev-expansion 1)
			)
		    )
		)
	    )
	    (novalue)
	)
	(c-/*
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (preceding-char) '/')
	        (if (= (current-syntax-entity) is-dull)
		    (progn
		    	(delete-previous-character)
		        (language-template-driver "C" "/*" 0)
		    )
		    (insert-character '*')
		)
		(insert-character '*')
	    )
	)
	(c-{
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (current-syntax-entity) is-dull)
		(language-template-driver "C" "{" 0)
		(insert-character '{')
	    )
	)
	(c-?
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (current-syntax-entity) is-dull)
		(language-template-driver "C" "?" 0)
		(insert-character '?')
	    )
	)
	(expand-c-keyword
	    c-ek-case-fold-search
	    c-ek-looking-at
	    c-ek-data
	    language-start-entity
	    c-last-key-struck
	    language-exit-point
	    (setq c-ek-case-fold-search case-fold-search)
	    (setq case-fold-search 1)
	    (setq c-last-key-struck '\r')
	    (setq c-ek-looking-at
		(provide-prefix-argument -1
		    (looking-at "\\w")
		)
	    )
	    (setq case-fold-search c-ek-case-fold-search)
	    (if c-ek-looking-at
		(progn
		    (save-window-excursion
			(backward-word)
			(set-mark)
			(forward-word)
			(case-region-lower)
			(setq c-ek-data (region-to-string))
			(erase-region)
		    )
		    (language-template-driver "C" c-ek-data 0)
		)
	    )
	    (novalue)
	)
    )
    (save-window-excursion
	(if (! (is-bound language-loaded))
	    (execute-mlisp-file "language_support")
	)
	(if (! (is-bound paren-flash))
	    (execute-mlisp-file "flasher")
	)
	(switch-to-buffer "c-mode")
	(define-keymap "C-MAP")
	(use-local-map "C-MAP")
	(use-abbrev-table "c-mode")
	(use-syntax-table "c-mode")
	(modify-syntax-table "word" "$_#")
	(modify-syntax-table "paren" "(" ")")
	(modify-syntax-table "paren" "[" "]")
	(modify-syntax-table "paren" "{" "}")
	(modify-syntax-table "string" "\"")
	(modify-syntax-table "prefix" "\\")
	(modify-syntax-table "comment" "/*" "*/")
	(local-bind-to-key "c-{" "{")
	(local-bind-to-key "c-/*" "*")
	(local-bind-to-key "c-?" "?")
	(local-bind-to-key "paren-flash" ")")
	(local-bind-to-key "paren-flash" "]")
	(local-bind-to-key "paren-flash" "}")
	(error-occurred (execute-mlisp-file "cmode.key"))
	(define-hooked-local-abbrev "goto" "goto" "~c-abbrev-handler")
	(define-hooked-local-abbrev "return" "return" "~c-abbrev-handler")
	(define-hooked-local-abbrev "if" "if" "~c-abbrev-handler")
	(define-hooked-local-abbrev "else" "else" "~c-abbrev-handler")
	(define-hooked-local-abbrev "for" "for" "~c-abbrev-handler")
	(define-hooked-local-abbrev "do" "do" "~c-abbrev-handler")
	(define-hooked-local-abbrev "while" "while" "~c-abbrev-handler")
	(define-hooked-local-abbrev "switch" "switch" "~c-abbrev-handler")
	(define-hooked-local-abbrev "case" "case" "~c-abbrev-handler")
	(define-hooked-local-abbrev "default" "default" "~c-abbrev-handler")
	(define-hooked-local-abbrev "entry" "entry" "~c-abbrev-handler")
	(define-hooked-local-abbrev "sizeof" "sizeof" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#include" "#include" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#module" "#module" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#if" "#if" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#ifdef" "#ifdef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#ifndef" "#ifndef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#undef" "#undef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#define" "#define" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#dictionary" "#dictionary" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#line" "#line" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#" "#" "~c-abbrev-handler")
    )
    (delete-buffer "c-mode")
)
