(progn
    (if (! (is-bound bliss-auto-expansion))
        (progn
	    (declare-buffer-specific bliss-auto-expansion)
	    (setq-default bliss-auto-expansion 1)
	)
    )
    (declare-global
	~bliss-module-name
    )
    (defun
	(bliss-mode
	    (use-local-map "BLISS-MAP")
	    (use-syntax-table "bliss-mode")
	    (use-abbrev-table "bliss-mode")
	    (setq abbrev-mode bliss-auto-expansion)
	    (setq mode-string "Bliss")
	    (language-set-keyword-case)
	    (novalue)
	)
	(~bliss-module
	    (setq ~bliss-module-name "")
	    (language-fetch-mandatory "Insert the module name and use exit-emacs.")
	    (save-window-excursion
		(goto-character template-start-field)
		(set-mark)
		(goto-character template-end-field)(backward-character)
		(setq ~bliss-module-name (region-to-string))
	    )
	    (goto-character template-end-field)
	)
	(~bliss-routine-name
	    (setq ~bliss-routine-name "")
	    (language-fetch-mandatory "Insert the routine name and use exit-emacs.")
	    (save-window-excursion
		(goto-character template-start-field)
		(set-mark)
		(goto-character template-end-field)(backward-character)
		(setq ~bliss-routine-name (region-to-string))
	    )
	    (goto-character template-end-field)
	)
	(~bliss-!
	    language-start-entity
	    language-exit-point
	    (language-template-driver "Bliss" "!" 0)
	)
	(~bliss-%
	    language-start-entity
	    language-exit-point
	    ~bliss-last-key-struck
	    (setq ~bliss-last-key-struck '\r')
	    (if (= (preceding-char) '%')
	        (if (= (current-syntax-entity) is-dull)
		    (progn
		    	(delete-previous-character)
		        (language-template-driver "Bliss" "%(" 0)
		    )
		    (insert-character (last-key-struck))
		)
		(insert-character (last-key-struck))
	    )
	)
	(~bliss-routine
	    bliss-r-fold
	    bliss-r-routine
	    bliss-routine-prefix
	    ~bliss-routine-name
	    (setq bliss-r-fold case-fold-search)
	    (setq case-fold-search 1)
	    (setq bliss-r-routine (provide-prefix-argument -1 (looking-at "external\\W\\W*\\=\\|forward\\W\\W*\\=\\|bind\\W\\W*\\=")))
	    (setq case-fold-search bliss-r-fold)
	    (if (! bliss-r-routine)
		(progn
		    (setq case-fold-search 1)
		    (if (provide-prefix-argument -1 (looking-at "global\\W\\W*\\="))
			(progn
			    (region-around-match 0)
			    (setq bliss-routine-prefix (region-to-string))
			    (erase-region)
			)
			(setq bliss-routine-prefix "")
		    )
		    (setq case-fold-search bliss-r-fold)
		    (template-exit)
		    (language-template-driver "Bliss" "routine-def" 0)
		)
	    )
	)
	(~bliss-exit-on-space
	    (if (!= bliss-last-key-struck '\r')
		(progn
		    (insert-string (char-to-string bliss-last-key-struck))
		    (template-exit)
		)
	    )
	)
	(~bliss-abbrev-handler
	    bliss-ah-current
	    (setq bliss-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= bliss-ah-current is-word)
			(= bliss-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    bliss-ah-case-fold-search
		    bliss-ah-looking-at
		    bliss-ah-data
		    language-start-entity
		    bliss-routine-mark
		    bliss-last-key-struck
		    language-exit-point
		    (setq bliss-ah-case-fold-search case-fold-search)
		    (setq case-fold-search 1)
		    (setq bliss-ah-looking-at
			(provide-prefix-argument -1
			    (looking-at abbrev-expansion)
			)
		    )
		    (setq case-fold-search bliss-ah-case-fold-search)
		    (if bliss-ah-looking-at
			(progn
			    (save-window-excursion
				(region-around-match 0)
				(erase-region)
			    )
			    (setq bliss-last-key-struck (last-key-struck))
			    (language-template-driver "Bliss" abbrev-expansion 1)
			)
		    )
		)
	    )
	    (novalue)
	)
	(~bliss-short-abbrev-handler
	    bliss-ah-current
	    (setq bliss-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= bliss-ah-current is-word)
			(= bliss-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    (delete-previous-word)
		    (insert-string abbrev-expansion)
		    (language-keyword-convert)
		)
	    )
	)
	(expand-bliss-keyword
	    bliss-ek-case-fold-search
	    bliss-ek-looking-at
	    bliss-ek-data
	    language-start-entity
	    bliss-routine-mark
	    bliss-last-key-struck
	    language-exit-point
	    (setq bliss-ek-case-fold-search case-fold-search)
	    (setq case-fold-search 1)
	    (setq bliss-last-key-struck '\r')
	    (setq bliss-ek-looking-at
		(provide-prefix-argument -1
		    (looking-at "\\w")
		)
	    )
	    (setq case-fold-search bliss-ek-case-fold-search)
	    (if bliss-ek-looking-at
		(progn
		    (save-window-excursion
			(backward-word)
			(set-mark)
			(forward-word)
			(case-region-lower)
			(setq bliss-ek-data (region-to-string))
			(erase-region)
		    )
		    (language-template-driver "Bliss" bliss-ek-data 0)
		)
	    )
	    (novalue)
	)
	(~bliss-routine-mark
	    (setq bliss-routine-mark (dot))
	)
	(~bliss-routine-params
	    (if (!= (+ bliss-routine-mark) (dot))
		(progn
		    (insert-string "\n")
		    (to-col
			(save-window-excursion
			    (goto-character bliss-routine-mark)
			    (current-indent)
			)
		    )
		    (insert-string ")")
		)
	    )
	)
    )
    (save-window-excursion
	(if (! (is-bound language-loaded))
	    (execute-mlisp-file "language_support")
	)
	(switch-to-buffer "bliss-mode")
	(use-abbrev-table "bliss-mode")
	(use-syntax-table "bliss-mode")
	(define-keymap "BLISS-MAP")
	(use-local-map "BLISS-MAP")
	(local-bind-to-key "~bliss-!" "!")
	(local-bind-to-key "~bliss-%" "(")
	(if (! (is-bound paren-flash-wait))
	    (execute-mlisp-file "flasher.ml")
	)
	(local-bind-to-key "paren-flash" ")")
	(local-bind-to-key "paren-flash" "]")
	(local-bind-to-key "paren-flash" "}")
	(error-occurred (execute-mlisp-file "blissmode.key"))
	(modify-syntax-table "word" "$_%")
	(modify-syntax-table "paren" "(" ")")
	(modify-syntax-table "paren" "[" "]")
	(modify-syntax-table "paren" "<" ">")
	(modify-syntax-table "string" "'")
	(modify-syntax-table "comment" "!" "\n")
	(modify-syntax-table "comment" "%(" ")%")
	(define-hooked-local-abbrev "module" "module" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "switches" "switches" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "library" "library" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "require" "require" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "begin" "begin" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "psect" "psect" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "own" "own" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "literal" "literal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "external" "external" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "routine" "routine" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "macro" "macro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "keywordmacro" "keywordmacro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "global" "global" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "forward" "forward" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bind" "bind" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "while" "while" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "until" "until" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "if" "if" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "else" "else" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "case" "case" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "select" "select" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selecta" "selecta" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectu" "selectu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectone" "selectone" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectonea" "selectonea" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectoneu" "selectoneu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incr" "incr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incra" "incra" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incru" "incru" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decr" "decr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decru" "decru" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decra" "decra" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "do" "do" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "leave" "leave" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "with" "with" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "exitloop" "exitloop" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "return" "return" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "abs" "abs" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "max" "max" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "maxu" "maxu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "maxa" "maxa" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "min" "min" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "minu" "minu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "mina" "mina" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "actualparameter" "actualparameter" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "nullparameter" "nullparameter" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "addressing_mode" "addressing_mode" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "actualcount" "actualcount" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "argptr" "argptr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "align" "align" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bitvector" "bitvector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "block" "block" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "blockvector" "blockvector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "vector" "vector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "BUILTIN" "builtin" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bliss" "bliss" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "compiletime" "compiletime" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "enable" "enable" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ident" "ident" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "initial" "initial" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "label" "label" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "linkage" "linkage" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "map" "map" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "mod" "mod" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "plit" "plit" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "uplit" "uplit" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "preserve" "preserve" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "nopreserve" "nopreserve" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "preset" "preset" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "register" "register" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "local" "local" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "stacklocal" "stacklocal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "rep" "rep" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "setunwind" "setunwind" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "sign" "sign" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "signal" "signal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "signal_stop" "signal_stop" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "structure" "structure" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "undeclare" "undeclare" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%allocation" "%ALLOCATION" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%assign" "%assign" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%charcount" "%charcount" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%char" "%char" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ctce" "%ctce" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%declared" "%declared" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%if" "%if" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%error" "%error" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%errormacro" "%errormacro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%exactstring" "%exactstring" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%expand" "%expand" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%explode" "%explode" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%fieldexpand" "%fieldexpand" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%identical" "%identical" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%inform" "%inform" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%isstring" "%isstring" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%length" "%length" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ltce" "%ltce" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%message" "%message" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%name" "%name" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%nbits" "%nbits" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%nbitsu" "%nbitsu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%null" "%null" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%number" "%number" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%print" "%print" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%quote" "%quote" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%quotename" "%quotename" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ref" "%ref" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%remove" "%remove" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%require" "%require" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%size" "%size" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%string" "%string" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%switches" "%switches" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%unquote" "%unquote" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%warn" "%warn" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$a_rchar" "ch$a_rchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$a_wchar" "ch$a_wchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$allocation" "ch$allocation" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$compare" "ch$compare" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$copy" "ch$copy" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$diff" "ch$diff" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$eql" "ch$eql" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$fail" "ch$fail" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$fill" "ch$fill" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_ch" "ch$find_ch" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_not_ch" "ch$find_not_ch" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_sub" "ch$find_sub" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$geq" "ch$geq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$gtr" "ch$gtr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$leq" "ch$leq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$lss" "ch$lss" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$move" "ch$move" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$neq" "ch$neq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$plus" "ch$plus" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$ptr" "ch$ptr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$rchar" "ch$rchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$rchar_a" "ch$rchar_a" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$size" "ch$size" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$translate" "ch$translate" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$translatable" "ch$translatable" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$wchar" "ch$wchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$wchar_a" "ch$wchar_a" "~bliss-abbrev-handler")

	; Short abbrev - just case convert as asked
	(define-hooked-local-abbrev "%x" "%X" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%variant" "%VARIANT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%upval" "%UPVAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%remaining" "%REMAINING" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%p" "%P" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%o" "%O" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%g" "%G" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%h" "%H" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%exititeration" "%EXITITERATION" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%exitmacro" "%EXITMACRO" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%e" "%E" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%d" "%D" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%decimal" "%DECIMAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%b" "%B" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bliss" "%BLISS" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%BLISS32" "%BLISS32" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpaddr" "%BPADDR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpunit" "%BPUNIT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpval" "%BPVAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%c" "%C" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%count" "%COUNT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascii" "%ASCII" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascic" "%ASCIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%asciz" "%ASCIZ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascid" "%ASCID" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "weak" "WEAK" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "word" "WORD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "word_relative" "WORD_RELATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "write" "WRITE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "volatile" "volatile" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "unsigned" "UNSIGNED" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "signed" "SIGNED" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "share" "SHARE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "relative" "RELATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "relocatable" "RELOCATABLE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "read" "READ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "ref" "REF" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "otherwise" "OTHERWISE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "outrange" "OUTRANGE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "overlay" "OVERLAY" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "pic" "PIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nodefault" "NODEFAULT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noexecute" "NOEXECUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nonexternal" "NONEXTERNAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nopic" "NOPIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noread" "NOREAD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noshare" "NOSHARE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nowrite" "NOWRITE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "long" "LONG" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "long_relative" "LONG_RELEATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "inrange" "INRANGE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "interrupt" "INTERRUP" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "jsb" "JSB" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "general" "GENERAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran" "FORTRAN" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran_func" "FORTRAN_FUNC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran_sub" "FORTRAN_SUB" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "FIELD" "FIELD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "execute" "EXECUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "concatenate" "CONCATENATE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "debug" "DEBUG" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "call" "CALL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "byte" "BYTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "not" "NOT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eql" "EQL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqla" "EQLA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqlu" "EQLU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lss" "LSS" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lssa" "LSSA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lssu" "LSSU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "leq" "LEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "leqa" "LEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lequ" "LEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtr" "GTR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtra" "GTRA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtru" "GTRU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "geq" "GEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "geqa" "GEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gequ" "GEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "and" "AND" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "or" "OR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqv" "EQV" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "xor" "XOR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "neq" "NEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "neqa" "NEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nequ" "NEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "absolute" "ABSOLUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "always" "ALWAYS" "~bliss-short-abbrev-handler")
    )
    (delete-buffer "bliss-mode")
)
