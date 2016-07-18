;
; electric Java mode
;
(defun
    (Java-back-paren                    ; Go to last open parenthesis at current level
        (backward-paren 0))
)
(defun
    (Java-fore-paren                    ; Go to next close parenthesis at current level
        (forward-paren 0))
)
(defun
    (Java-flash-back-paren              ; Flash the matching left parenthesis
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
    (Java-flash-fore-paren              ; Flash the matching close parenthesis
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
    (Java-paren                 ; Flashes matching open parenthesis when ')' is typed.
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
    (Java-indent old-dot old-size       ; indent a {...} apropriately
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
    (Java-}                     ; flash the matching '{'
        (if (eolp)
            (progn
                (delete-white-space)
                (if (! (bolp))
                    (newline-and-indent)
                    (progn
                        (delete-previous-character)
                        (newline-and-indent)
                    ))
                (Java-paren)
                (newline-and-indent)
                (dedent-line)
            )
            (Java-paren)
        )
    )
)
(defun
    (Java-{                     ; Flash the matching '}'...
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
    (Java-semi          ; Insert a comment starter
        (insert-string ";")
        (if (eolp) (newline-and-indent))
    )
)
; need to setup tab stops too! (tab at beginning of blank line should just
; tab. at beginning of non-blank line, should re-indent-line.)
(defun
    (Java-tab
        (if (| (& (eolp) (bolp)) (!= (current-column) 1))
            (insert-character '\t')
            (progn
                (delete-white-space)
                (to-col (Java-indent-level))
            )
        )
    )
)

(defun
    (Java-indent-level
        (save-excursion
            (if (error-occurred (re-search-reverse "^[  ]*[!-~]"))
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
    (Java-compile
        (setq compile-it-command (concat "javac " current-buffer-file-name))

        (compile-it)
    )
)

(defun
    (Java-high-voltage-on
        (setq mode-string "Java high voltage")
        (local-bind-to-key "Java-semi" ";")
        (local-bind-to-key "Java-{" "{")
        (local-bind-to-key "Java-}" "}")
        (local-bind-to-key "Java-tab" "\t")
        (local-bind-to-key "Java-flash-fore-paren" "\e}")
        (local-bind-to-key "Java-flash-back-paren" "\e{")
        (high-voltage-off-key-binding)
        (message "high voltage on")
    )
)
(defun
    (Java-high-voltage-off
        (setq mode-string "Java")
        (local-bind-to-key "self-insert" ";")
        (local-bind-to-key "self-insert" "{")
        (local-bind-to-key "self-insert" "\t")
        (local-bind-to-key "Java-paren" "}")
        (high-voltage-on-key-binding)
        (message "high voltage off")
    )
)
(defun
    (Java-mode
        (setq mode-string "Java")
        (use-syntax-table "Java")
        (use-local-map "Java-map")
        (use-abbrev-table "Java")
        (novalue)
    )
)
(save-excursion
    (temp-use-buffer "~Java-hack~")
    (use-syntax-table "Java")

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
    (modify-syntax-table "keyword-1" "boolean")
    (modify-syntax-table "keyword-1" "true")
    (modify-syntax-table "keyword-1" "false")
    (modify-syntax-table "keyword-1" "char")
    (modify-syntax-table "keyword-1" "byte")
    (modify-syntax-table "keyword-1" "short")
    (modify-syntax-table "keyword-1" "int")
    (modify-syntax-table "keyword-1" "long")
    (modify-syntax-table "keyword-1" "float")
    (modify-syntax-table "keyword-1" "double")
    (modify-syntax-table "keyword-1" "void")
    (modify-syntax-table "keyword-1" "null")
    (modify-syntax-table "keyword-1" "new")
    (modify-syntax-table "keyword-1" "this")
    (modify-syntax-table "keyword-1" "super")
    (modify-syntax-table "keyword-1" "if")
    (modify-syntax-table "keyword-1" "else")
    (modify-syntax-table "keyword-1" "switch")
    (modify-syntax-table "keyword-1" "case")
    (modify-syntax-table "keyword-1" "break")
    (modify-syntax-table "keyword-1" "default")
    (modify-syntax-table "keyword-1" "for")
    (modify-syntax-table "keyword-1" "continue")
    (modify-syntax-table "keyword-1" "do")
    (modify-syntax-table "keyword-1" "while")
    (modify-syntax-table "keyword-1" "return")
    (modify-syntax-table "keyword-1" "throw")
    (modify-syntax-table "keyword-1" "synchronized")
    (modify-syntax-table "keyword-1" "try")
    (modify-syntax-table "keyword-1" "catch")
    (modify-syntax-table "keyword-1" "finally")
    (modify-syntax-table "keyword-1" "static")
    (modify-syntax-table "keyword-1" "abstract")
    (modify-syntax-table "keyword-1" "final")
    (modify-syntax-table "keyword-1" "private")
    (modify-syntax-table "keyword-1" "protected")
    (modify-syntax-table "keyword-1" "public")
    (modify-syntax-table "keyword-1" "transient")
    (modify-syntax-table "keyword-1" "volatile")
    (modify-syntax-table "keyword-1" "class")
    (modify-syntax-table "keyword-1" "instanceof")
    (modify-syntax-table "keyword-1" "throws")
    (modify-syntax-table "keyword-1" "native")
    (modify-syntax-table "keyword-1" "extends")
    (modify-syntax-table "keyword-1" "interface")
    (modify-syntax-table "keyword-1" "implements")
    (modify-syntax-table "keyword-1" "package")
    (modify-syntax-table "keyword-1" "import")
    (modify-syntax-table "keyword-1" "cast")
    (modify-syntax-table "keyword-1" "const")
    (modify-syntax-table "keyword-1" "future")
    (modify-syntax-table "keyword-1" "generic")
    (modify-syntax-table "keyword-1" "goto")
    (modify-syntax-table "keyword-1" "inner")
    (modify-syntax-table "keyword-1" "operator")
    (modify-syntax-table "keyword-1" "outer")
    (modify-syntax-table "keyword-1" "rest")
    (modify-syntax-table "keyword-1" "var")

    (define-keymap "Java-map")
    (define-keymap "Java-ESC-map")
    (use-local-map "Java-map")
    (local-bind-to-key "Java-ESC-map" "\e")
    (execute-mlisp-file "javamode.key")
    (high-voltage-on-key-binding)
    (kill-buffer "~Java-hack~")
)
(novalue)
