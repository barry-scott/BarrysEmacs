;
; json mode
;
(defun
    (json-mode
        (setq mode-string "json")
        (use-syntax-table "json")
        (use-local-map "json-map")
        (use-abbrev-table "json")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.json"))
        (novalue)
    )
)

(defun
    (~json-mode-setup-syntax-table
        (modify-syntax-table "paren" "(" ")")
        (modify-syntax-table "paren" "{" "}")
        (modify-syntax-table "paren" "[" "]")
        (modify-syntax-table "string-1" "\"")
        (modify-syntax-table "prefix" "\\")
        (modify-syntax-table "keyword-1" "null")
        (modify-syntax-table "keyword-1" "true")
        (modify-syntax-table "keyword-1" "false")
    )
)

(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "json-map")
    (define-keymap "json-ESC-map")
    (use-local-map "json-map")
    (local-bind-to-key "json-ESC-map" "\e")
    (error-occurred (execute-mlisp-file "json-mode.key"))

    (use-syntax-table "json")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )
    (if (is-bound check-for-use-of-tabs-problems)
        (if check-for-use-of-tabs-problems
            (add-check-for-use-of-tabs-problems-syntax-table)
        )
    )
    (~json-mode-setup-syntax-table)
    (delete-buffer "keymap-hack")
    (novalue)
)
