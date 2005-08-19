; 
; electric C mode
; 
(progn
(defun
    (back-paren			; Go to last open parenthesis at current level
	(backward-paren 0))
)
(defun
    (fore-paren			; Go to next close parenthesis at current level
	(forward-paren 0))
)
(defun
    (flash-back-paren 		; Flash the matching left parenthesis
	(save-excursion
	    (backward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (flash-fore-paren		; Flash the matching close parenthesis
	(save-excursion
	    (forward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (c-paren			; Flashes matching open parenthesis when ')' is typed.
	(insert-character (last-key-struck))
	(save-excursion
	    (backward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (c-indent old-dot old-size	; indent a {...} apropriately
        (setq old-dot (dot))
	(setq old-size (buffer-size))
	(save-excursion
	    (previous-line)
	    (search-forward "^}")
	    (set-mark)
	    (backward-paren 0)
	    (beginning-of-line)
	    (exchange-dot-and-mark)
	    (end-of-line)
	    (forward-character)
	    (filter-region "indent -st")
	)
	(goto-character (/ (* (buffer-size) old-dot) old-size))
	(novalue)
    )
)
(defun
    (electric-}			; flash the matching '{'
	(if (eolp)
	    (progn
		(delete-white-space)
		(if (! (bolp))
		    (newline-and-indent)
		    (progn
			(delete-previous-character)
			(newline-and-indent)
		    ))
		(c-paren)
;		(dedent-line)
		(newline-and-indent)
		(dedent-line)
	    )
	    (c-paren)
	)
    )
)
(defun
    (electric-{			; Flash the matching '}'...
				; really should do this only if the line
				; isn't all whitespace
	(if (eolp)
	    (progn
		(insert-string "{")
		(newline-and-indent)
	    )
	    (insert-string "{")
	)
    )
)
(defun
    (dedent-line		; Move line to left by one tabstop
        (beginning-of-line)
	(to-col (- (current-indent) logical-tab-size))
	(insert-string ".")
	(delete-white-space)
	(delete-previous-character)
	(end-of-line)
    )
)
(defun
    (indent-line		; Move line to right by one tabstop
        (beginning-of-line)
	(to-col (+ (current-indent) logical-tab-size))
	(insert-string ".")
	(delete-white-space)
	(delete-previous-character)
	(end-of-line)
    )
)
(defun
    (electric-semi		; Insert a comment starter
	(insert-string ";")
	(if (eolp) (newline-and-indent))
    )
)
(defun
    (skip-spaces		; Skip 'following' whitespace
	(forward-character)
	(while (| (| (= (following-char) ' ')
		      (= (following-char) '	'))
		   (= (following-char) 10))
	    (forward-character)
	)
    )
)
(defun
    (electric-*			; If starting a comment, then do it.
	(if (& (eolp) (= (preceding-char) '/'))
	    (progn
		(delete-previous-character)
		(if (! (bolp))
		    (progn
			(delete-white-space)
			(if (bolp) (delete-previous-character))
		    )
		)
		(begin-C-comment)
	    )
	    (insert-string "*")
	)
    )
)
; If only thing on line and starting a comment, then don't indent 
(defun
    (electric-/			; If in a comment, close it off
	(if (& (eolp) (= (preceding-char) '*'))
	    (progn
		(delete-previous-character)
		(end-C-comment)
	    )
	    (insert-string "/")
	)
    )
)
; need to setup tab stops too! (tab at beginning of blank line should just
; tab. at beginning of non-blank line, should re-indent-line.)
(defun
    (electric-tab
	(if (| (& (eolp) (bolp)) (!= (current-column) 1))
	    (insert-character '\t')
	    (progn
		(delete-white-space)
		(to-col (c-indent-level))
	    )
	)
    )

 

    (c-indent-level
	(save-excursion
	    (if (error-occurred (re-search-reverse "^[ 	]*[!-~]"))
		(progn
		    logical-tab-size
		)
		(progn
		    (re-search-forward "[!-~]")
		    (- (current-column) 1)   
		)
	    )
	)
    )
)
(defun
    (begin-C-comment
				; should setup for a block comment if on a
				; blank line.
	(move-to-comment-column)
	(setq left-margin (current-column))
	(setq right-margin 78)
	(setq prefix-string "   ")
	(insert-string "/* ")
    )
)
(defun
    (end-C-comment		; Should close off a block comment.
	(setq right-margin 1000)
	(if (!= (preceding-char) ' ') (insert-string " "))
	(insert-string "*/")
    )
)

(defun
    (high-voltage-on
	(setq mode-string "electric-C high voltage")
	(local-bind-to-key "electric-semi" ";")
	(local-bind-to-key "electric-{" "{")
	(local-bind-to-key "electric-}" "}")
	(local-bind-to-key "electric-*" "*")
	(local-bind-to-key "electric-/" "/")
	(local-bind-to-key "electric-tab" "\t")
	(local-bind-to-key "flash-fore-paren" "\e}")
	(local-bind-to-key "flash-back-paren" "\e{")
	(high-voltage-off-key-binding)
	(message "high voltage on")
    )
)
(defun
    (high-voltage-off
	(setq mode-string "electric-C")
	(local-bind-to-key "self-insert" ";")
	(local-bind-to-key "self-insert" "{")
	(local-bind-to-key "self-insert" "\t")
	(local-bind-to-key "c-paren" "}")
	(local-bind-to-key "self-insert" "*")
	(local-bind-to-key "self-insert" "/")
	(high-voltage-on-key-binding)
	(message "high voltage off")
    )
)
(defun
    (electric-c-mode
	(setq mode-string "electric-C")
	(use-syntax-table "C")
	(use-local-map "electric-c-map")
	(use-abbrev-table "electric-c")
	(novalue)
    )
)
(use-syntax-table "C")
(modify-syntax-table "paren" "(" ")")
(modify-syntax-table "paren" "{" "}")
(modify-syntax-table "paren" "[" "]")
(modify-syntax-table "comment-1" "/*" "*/")
(modify-syntax-table "string-1" "\"")
(modify-syntax-table "string-2" "'")
(modify-syntax-table "prefix" "\\")
(modify-syntax-table "word" "_$")
(modify-syntax-table "comment-2" "//" "\n")
(modify-syntax-table "keyword-1" "auto")
(modify-syntax-table "keyword-1" "break")
(modify-syntax-table "keyword-1" "case")
(modify-syntax-table "keyword-1" "char")
(modify-syntax-table "keyword-1" "const")
(modify-syntax-table "keyword-1" "continue")
(modify-syntax-table "keyword-1" "default")
(modify-syntax-table "keyword-1" "do")
(modify-syntax-table "keyword-1" "double")
(modify-syntax-table "keyword-1" "else")
(modify-syntax-table "keyword-1" "enum")
(modify-syntax-table "keyword-1" "extern")
(modify-syntax-table "keyword-1" "float")
(modify-syntax-table "keyword-1" "for")
(modify-syntax-table "keyword-1" "goto")
(modify-syntax-table "keyword-1" "if")
(modify-syntax-table "keyword-1" "int")
(modify-syntax-table "keyword-1" "long")
(modify-syntax-table "keyword-1" "register")
(modify-syntax-table "keyword-1" "return")
(modify-syntax-table "keyword-1" "short")
(modify-syntax-table "keyword-1" "signed")
(modify-syntax-table "keyword-1" "sizeof")
(modify-syntax-table "keyword-1" "static")
(modify-syntax-table "keyword-1" "struct")
(modify-syntax-table "keyword-1" "switch")
(modify-syntax-table "keyword-1" "typedef")
(modify-syntax-table "keyword-1" "union")
(modify-syntax-table "keyword-1" "unsigned")
(modify-syntax-table "keyword-1" "void")
(modify-syntax-table "keyword-1" "volatile")
(modify-syntax-table "keyword-1" "while")
(modify-syntax-table "keyword-2" "bool")
(modify-syntax-table "keyword-2" "catch")
(modify-syntax-table "keyword-2" "class")
(modify-syntax-table "keyword-2" "delete")
(modify-syntax-table "keyword-2" "explicit")
(modify-syntax-table "keyword-2" "false")
(modify-syntax-table "keyword-2" "friend")
(modify-syntax-table "keyword-2" "inline")
(modify-syntax-table "keyword-2" "mutable")
(modify-syntax-table "keyword-2" "namespace")
(modify-syntax-table "keyword-2" "new")
(modify-syntax-table "keyword-2" "operator")
(modify-syntax-table "keyword-2" "private")
(modify-syntax-table "keyword-2" "protected")
(modify-syntax-table "keyword-2" "public")
(modify-syntax-table "keyword-2" "template")
(modify-syntax-table "keyword-2" "this")
(modify-syntax-table "keyword-2" "throw")
(modify-syntax-table "keyword-2" "true")
(modify-syntax-table "keyword-2" "try")
(modify-syntax-table "keyword-2" "typename")
(modify-syntax-table "keyword-2" "typeid")
(modify-syntax-table "keyword-2" "using")
(modify-syntax-table "keyword-2" "virtual")
(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "electric-c-map")
    (define-keymap "electric-c-ESC-map")
    (use-local-map "electric-c-map")
    (local-bind-to-key "electric-c-ESC-map" "\e")
    (execute-mlisp-file "electricc.key")
    (high-voltage-on-key-binding)
    (kill-buffer "keymap-hack")
)
(novalue)
)
