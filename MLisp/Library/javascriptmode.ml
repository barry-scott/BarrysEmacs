; 
; Javascript mode
; 
(defun
    (JavaScript-back-paren			; Go to last open parenthesis at current level
	(backward-paren 0))
)
(defun
    (JavaScript-fore-paren			; Go to next close parenthesis at current level
	(forward-paren 0))
)
(defun
    (JavaScript-flash-back-paren 		; Flash the matching left parenthesis
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
    (JavaScript-flash-fore-paren		; Flash the matching close parenthesis
	(save-excursion
	    (forward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string))
		)
	    )
	)
    )
)
(defun
    (JavaScript-paren			; Flashes matching open parenthesis when ')' is typed.
	(insert-character (last-key-struck))
	(save-excursion
	    (backward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string))
		)
	    )
	)
    )
)
(defun
    (JavaScript-indent old-dot old-size	; indent a {...} apropriately
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
    (JavaScript-}			; flash the matching '{'
	(if (eolp)
	    (progn
		(delete-white-space)
		(if (! (bolp))
		    (newline-and-indent)
		    (progn
			(delete-previous-character)
			(newline-and-indent)
		    ))
		(JavaScript-paren)
		(newline-and-indent)
		(dedent-line)
	    )
	    (JavaScript-paren)
	)
    )
)
(defun
    (JavaScript-{			; Flash the matching '}'...
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
; need to setup tab stops too! (tab at beginning of blank line should just
; tab. at beginning of non-blank line, should re-indent-line.)
(defun
    (JavaScript-tab
	(if (| (& (eolp) (bolp)) (!= (current-column) 1))
	    (insert-character '\t')
	    (progn
		(delete-white-space)
		(to-col (JavaScript-indent-level))
	    )
	)
    )
)
 
(defun
    (JavaScript-indent-level
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
    (JavaScript-compile
	(setq compile-it-command (concat "javac " current-buffer-file-name))

	(compile-it)
    )
)

(defun
    (JavaScript-high-voltage-on
	(setq mode-string "JavaScript high voltage")
	(local-bind-to-key "JavaScript-{" "{")
	(local-bind-to-key "JavaScript-}" "}")
	(local-bind-to-key "JavaScript-tab" "\t")
	(local-bind-to-key "JavaScript-flash-fore-paren" "\e}")
	(local-bind-to-key "JavaScript-flash-back-paren" "\e{")
	(high-voltage-off-key-binding)
	(message "high voltage on")
    )
)
(defun
    (JavaScript-high-voltage-off
	(setq mode-string "JavaScript")
	(local-bind-to-key "self-insert" ";")
	(local-bind-to-key "self-insert" "{")
	(local-bind-to-key "self-insert" "\t")
	(local-bind-to-key "JavaScript-paren" "}")
	(high-voltage-on-key-binding)
	(message "high voltage off")
    )
)
(defun
    (JavaScript-mode
	(setq mode-string "JavaScript")
	(use-syntax-table "JavaScript")
	(use-local-map "JavaScript-map")
	(use-abbrev-table "JavaScript")
	(novalue)
    )
)
(save-excursion
    (temp-use-buffer "~JavaScript-hack~")
    (use-syntax-table "JavaScript")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )

    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "paren" "{" "}")
    (modify-syntax-table "paren" "[" "]")
    (modify-syntax-table "string" "\"")
    (modify-syntax-table "string" "'")
    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "_$")
    (modify-syntax-table "comment" "//" "\n")
    (modify-syntax-table "comment" "/*" "*/")

    ; Reserved JavaScript Keywords
    (modify-syntax-table "keyword-1" "boolean")
    (modify-syntax-table "keyword-1" "break")
    (modify-syntax-table "keyword-1" "continue")
    (modify-syntax-table "keyword-1" "delete")
    (modify-syntax-table "keyword-1" "else")
    (modify-syntax-table "keyword-1" "false")
    (modify-syntax-table "keyword-1" "for")
    (modify-syntax-table "keyword-1" "function")
    (modify-syntax-table "keyword-1" "if")
    (modify-syntax-table "keyword-1" "in")
    (modify-syntax-table "keyword-1" "new")
    (modify-syntax-table "keyword-1" "null")
    (modify-syntax-table "keyword-1" "return")
    (modify-syntax-table "keyword-1" "this")
    (modify-syntax-table "keyword-1" "true")
    (modify-syntax-table "keyword-1" "typeof")
    (modify-syntax-table "keyword-1" "var")
    (modify-syntax-table "keyword-1" "void")
    (modify-syntax-table "keyword-1" "while")
    (modify-syntax-table "keyword-1" "with")

    ; builtin Function and Property Names
    (modify-syntax-table "keyword-2" "alert")
    (modify-syntax-table "keyword-2" "Anchor")
    (modify-syntax-table "keyword-2" "Area")
    (modify-syntax-table "keyword-2" "Array")
    (modify-syntax-table "keyword-2" "assign")
    (modify-syntax-table "keyword-2" "blur")
    (modify-syntax-table "keyword-2" "Boolean")
    (modify-syntax-table "keyword-2" "Button")
    (modify-syntax-table "keyword-2" "Checkbox")
    (modify-syntax-table "keyword-2" "clearTimeout")
    (modify-syntax-table "keyword-2" "close")
    (modify-syntax-table "keyword-2" "closed")
    (modify-syntax-table "keyword-2" "confirm")
    (modify-syntax-table "keyword-2" "Date")
    (modify-syntax-table "keyword-2" "defaultStatus")
    (modify-syntax-table "keyword-2" "Document")
    (modify-syntax-table "keyword-2" "document")
    (modify-syntax-table "keyword-2" "Element")
    (modify-syntax-table "keyword-2" "escape")
    (modify-syntax-table "keyword-2" "eval")
    (modify-syntax-table "keyword-2" "FileUpload")
    (modify-syntax-table "keyword-2" "focus")
    (modify-syntax-table "keyword-2" "Form")
    (modify-syntax-table "keyword-2" "Frame")
    (modify-syntax-table "keyword-2" "frames")
    (modify-syntax-table "keyword-2" "Function")
    (modify-syntax-table "keyword-2" "getClass")
    (modify-syntax-table "keyword-2" "Hidden")
    (modify-syntax-table "keyword-2" "History")
    (modify-syntax-table "keyword-2" "history")
    (modify-syntax-table "keyword-2" "Image")
    (modify-syntax-table "keyword-2" "isNaN")
    (modify-syntax-table "keyword-2" "java")
    (modify-syntax-table "keyword-2" "JavaArray")
    (modify-syntax-table "keyword-2" "JavaClass")
    (modify-syntax-table "keyword-2" "JavaObject")
    (modify-syntax-table "keyword-2" "JavaPackage")
    (modify-syntax-table "keyword-2" "length")
    (modify-syntax-table "keyword-2" "Link")
    (modify-syntax-table "keyword-2" "Location")
    (modify-syntax-table "keyword-2" "location")
    (modify-syntax-table "keyword-2" "Math")
    (modify-syntax-table "keyword-2" "MImeType")
    (modify-syntax-table "keyword-2" "name")
    (modify-syntax-table "keyword-2" "navigate")
    (modify-syntax-table "keyword-2" "Navigator")
    (modify-syntax-table "keyword-2" "navigator")
    (modify-syntax-table "keyword-2" "netscape")
    (modify-syntax-table "keyword-2" "Number")
    (modify-syntax-table "keyword-2" "Object")
    (modify-syntax-table "keyword-2" "onblur")
    (modify-syntax-table "keyword-2" "onerror")
    (modify-syntax-table "keyword-2" "onfocus")
    (modify-syntax-table "keyword-2" "onload")
    (modify-syntax-table "keyword-2" "onunload")
    (modify-syntax-table "keyword-2" "open")
    (modify-syntax-table "keyword-2" "opener")
    (modify-syntax-table "keyword-2" "Option")
    (modify-syntax-table "keyword-2" "Packages")
    (modify-syntax-table "keyword-2" "parent")
    (modify-syntax-table "keyword-2" "parseFloat")
    (modify-syntax-table "keyword-2" "parseInt")
    (modify-syntax-table "keyword-2" "Password")
    (modify-syntax-table "keyword-2" "Plugin")
    (modify-syntax-table "keyword-2" "prompt")
    (modify-syntax-table "keyword-2" "prototype")
    (modify-syntax-table "keyword-2" "Radio")
    (modify-syntax-table "keyword-2" "ref")
    (modify-syntax-table "keyword-2" "Reset")
    (modify-syntax-table "keyword-2" "scroll")
    (modify-syntax-table "keyword-2" "Select")
    (modify-syntax-table "keyword-2" "self")
    (modify-syntax-table "keyword-2" "setTimeout")
    (modify-syntax-table "keyword-2" "status")
    (modify-syntax-table "keyword-2" "String")
    (modify-syntax-table "keyword-2" "Submit")
    (modify-syntax-table "keyword-2" "sun")
    (modify-syntax-table "keyword-2" "taint")
    (modify-syntax-table "keyword-2" "Text")
    (modify-syntax-table "keyword-2" "Textarea")
    (modify-syntax-table "keyword-2" "top")
    (modify-syntax-table "keyword-2" "toString")
    (modify-syntax-table "keyword-2" "unescape")
    (modify-syntax-table "keyword-2" "untaint")
    (modify-syntax-table "keyword-2" "valueOf")
    (modify-syntax-table "keyword-2" "Window")
    (modify-syntax-table "keyword-2" "window")

    ; Java Keywords Reserved by JavaScript
    (modify-syntax-table "keyword-3" "abstract")
    (modify-syntax-table "keyword-3" "boolean")
    (modify-syntax-table "keyword-3" "byte")
    (modify-syntax-table "keyword-3" "case")
    (modify-syntax-table "keyword-3" "catch")
    (modify-syntax-table "keyword-3" "char")
    (modify-syntax-table "keyword-3" "class")
    (modify-syntax-table "keyword-3" "const")
    (modify-syntax-table "keyword-3" "default")
    (modify-syntax-table "keyword-3" "do")
    (modify-syntax-table "keyword-3" "double")
    (modify-syntax-table "keyword-3" "extends")
    (modify-syntax-table "keyword-3" "final")
    (modify-syntax-table "keyword-3" "finally")
    (modify-syntax-table "keyword-3" "float")
    (modify-syntax-table "keyword-3" "goto")
    (modify-syntax-table "keyword-3" "implements")
    (modify-syntax-table "keyword-3" "import")
    (modify-syntax-table "keyword-3" "instanceof")
    (modify-syntax-table "keyword-3" "int")
    (modify-syntax-table "keyword-3" "interface")
    (modify-syntax-table "keyword-3" "long")
    (modify-syntax-table "keyword-3" "native")
    (modify-syntax-table "keyword-3" "package")
    (modify-syntax-table "keyword-3" "private")
    (modify-syntax-table "keyword-3" "protected")
    (modify-syntax-table "keyword-3" "public")
    (modify-syntax-table "keyword-3" "short")
    (modify-syntax-table "keyword-3" "static")
    (modify-syntax-table "keyword-3" "super")
    (modify-syntax-table "keyword-3" "switch")
    (modify-syntax-table "keyword-3" "synchronized")
    (modify-syntax-table "keyword-3" "throw")
    (modify-syntax-table "keyword-3" "throws")
    (modify-syntax-table "keyword-3" "transient")
    (modify-syntax-table "keyword-3" "try")

    (define-keymap "JavaScript-map")
    (define-keymap "JavaScript-ESC-map")
    (use-local-map "JavaScript-map")
    (local-bind-to-key "JavaScript-ESC-map" "\e")
    (execute-mlisp-file "javascriptmode.key")
    (high-voltage-on-key-binding)
    (kill-buffer "~JavaScript-hack~")
)
(novalue)
