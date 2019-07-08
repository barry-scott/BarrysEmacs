;
; P4  change mode
;
(defun
    (p4-change-mode
        (setq mode-string "P4 Change")
        (use-syntax-table "P4 Change")
        (use-local-map "P4-change-map")
        (use-abbrev-table "P4-change")

        ; need to use hard tabs in this form
        (setq logical-tab-size 8)
        (setq indent-use-tab 1)

        (beginning-of-file)
        (unset-mark)
        (re-search-forward "^Description:\n\t")
        (sit-for 0)
        (if (looking-at "<enter description here>")
            ; first edit of the change comment everything out for safety
            (p4-all-sections-exclude-all)
        )
        (novalue)
    )
)

(defun
    (p4-toggle-include
        ~section

        (setq ~section "")

        (error-occurred
            (save-window-excursion
                ; find section
                (ere-search-reverse "^(\\w+):")
                (region-around-match 1)
                (setq ~section (region-to-string))
            )
        )

        (if (| (= ~section "Jobs") (= ~section "Files"))
            (progn
                (beginning-of-line)
                (if
                    (ere-looking-at "\t# ")
                    (progn
                        (forward-character 1)
                        (delete-next-character 2)
                        (next-line)
                        (beginning-of-line)
                    )
                    (ere-looking-at "\t[A-Z]|\t//")
                    (progn
                        (forward-character 1)
                        (insert-string "# ")
                        (next-line)
                        (beginning-of-line)
                    )
                )
            )
            (error-message "toggle needs a line in the Jobs or Files sections")
        )
        (novalue)
    )
)
(defun
    (p4-all-sections-exclude-all
        (save-window-excursion
            (~p4-section-exclude-all "Jobs")
            (~p4-section-exclude-all "Files")
        )
    )
)

(defun ~p4-section-exclude-all(~section)
    (beginning-of-file)
    (if
        (! (error-occurred (ere-search-forward (concat "^" ~section ":"))))
        (progn
            (next-line)
            (beginning-of-line)
            (while
                (if
                    (ere-looking-at "\t[A-Z]|\t//")
                    (progn
                        (p4-toggle-include)
                        1
                    )
                    (ere-looking-at "\t#")
                    (progn
                        (next-line)
                        (beginning-of-line)
                        1
                    )
                    0
                )
                (progn
                    (sit-for 0)
                    (novalue)
                )
            )
        )
    )
)

(defun
    (p4-visit-file
        (save-excursion
            (beginning-of-line)
            (if (looking-at "\t//")
                (progn
                    (ere-search-forward "\t(//.*?)[\t ]")
                    (region-around-match 1)
                    (visit-file
                        (p4-clientspec-to-filename (region-to-string)))
                )
            )
        )
    )
)

(defun
    (p4-change-diff
        (save-excursion
            (beginning-of-line)
            (if (looking-at "\t//")
                (progn
                    (ere-search-forward "\t(//.*?)[\t ]")
                    (region-around-match 1)
                    (execute-monitor-command
                        (concat "p4 diff -du \""
                            (region-to-string) "\""))
                    (pop-to-buffer "P4 change diff")
                    (erase-buffer)
                    (yank-buffer "command execution")
                    (beginning-of-file)
                    (diff-mode)
                )
            )
        )
    )
)

(save-window-excursion
    (temp-use-buffer "~p4-hack")

    (define-keymap "P4-change-map")
    (define-keymap "P4-change-ESC-map")
    (use-local-map "P4-change-map")
    (local-bind-to-key "P4-change-ESC-map" "\e")
    (execute-mlisp-file "p4-change-mode.key")

    (use-syntax-table "P4 Change")
    (modify-syntax-table "word" "_")
    (modify-syntax-table "comment" "#" "\n")

    (modify-syntax-table "keyword-1"  "Change:")
    (modify-syntax-table "keyword-1"  "Client:")
    (modify-syntax-table "keyword-1"  "User:")
    (modify-syntax-table "keyword-1"  "Status:")
    (modify-syntax-table "keyword-1"  "Description:")
    (modify-syntax-table "keyword-1"  "Jobs:")
    (modify-syntax-table "keyword-1"  "Files:")

    (kill-buffer "~p4-hack")
)
(novalue)
