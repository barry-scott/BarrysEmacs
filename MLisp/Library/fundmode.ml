;-*-mlisp-*-
(progn

(defun
    (newline-and-indent-relative
        (if (= (current-indent) 1)
            (newline)
            (progn
                (newline)
                (indent-relative)
            )
        )
    )
)
(defun
    (dedent-relative i col
        (setq i (current-indent))
        (setq col 1)
        (if (! (save-excursion (beginning-of-line) (bobp)))
            (save-excursion
                (previous-line)
                (beginning-of-line)
                (while (& (! (bobp))
                           (>= (current-indent) i)
                           (!= (current-indent)
                               (save-excursion
                                   (end-of-line) (current-column)
                               )
                           )
                       )
                    (previous-line)
                )
                (if (! (bobp)) (setq col (current-indent)))
            )
        )
        (save-excursion (delete-white-space) (to-col col))
    )
)
(defun
    (indent-relative col i

        (setq col 0)
        (setq i (current-column))
        (if (! (save-excursion (beginning-of-line) (bobp)))
            (save-excursion
                (previous-line)
                (if (! (eolp))(forward-character))
                (while
                    (&
                        (! (eolp))
                        (!
                            (&
                                (| (= (preceding-char) 32)
                                    (= (preceding-char) 9)
                                )
                                (!= (following-char) 9)
                                (!= (following-char) 32)
                            )
                        )
                    )
                    (forward-character))
                (if (! (eolp)) (setq col (current-column)))
            )
        )
        (if col
            (progn (delete-white-space) (to-col col))
            (to-col
                (+
                    (* logical-tab-size (/ (current-column) logical-tab-size))
                    logical-tab-size
                )
            )
        )
    )
)
(defun
    (fundamental-mode
        (use-local-map "Fundamental-mode-map")
        (use-syntax-table "Fundamental-mode")
        (use-abbrev-table "Fundamental-mode")
        (setq mode-string "Fundamental")
        (novalue)
    )
)
(save-window-excursion
    (switch-to-buffer "keymap-hack")
    (define-keymap "Fundamental-mode-map")
    (use-local-map "Fundamental-mode-map")
    (local-bind-to-key "indent-relative" "\t")
    (local-bind-to-key "dedent-relative" "\e\t")
    (local-bind-to-key "newline-and-indent-relative" "\n")
    (use-syntax-table "Fundamental-mode")
    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "paren" "{" "}")
    (modify-syntax-table "paren" "[" "]")
    (local-bind-to-key "paren-flash" ")")
    (local-bind-to-key "paren-flash" "]")
    (local-bind-to-key "paren-flash" "}")
)
)
