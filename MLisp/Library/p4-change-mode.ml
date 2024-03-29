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

        (if p4-remove-header-comments
            (progn
                (beginning-of-file)
                ; leave the first line that identifies the file as a p4 change
                (next-line)
                (set-mark)
                (while (looking-at "#")
                    (next-line)
                )
                (erase-region)
            )
        )

        (if p4-change-remove-jobs-section
            (progn
                (beginning-of-file)
                (if (! (error-occurred (re-search-forward "^Jobs:")))
                    (progn
                        (next-line)
                        (beginning-of-line)
                        (set-mark)
                        (re-search-forward "^Files:")
                        (beginning-of-line)
                        (erase-region)
                    )
                )
            )
        )

        (beginning-of-file)
        (unset-mark)
        (re-search-forward "^Description:\n\t")
        (sit-for 0)
        (if (looking-at "<enter description here>")
            ; first edit of the change comment everything out for safety
            (progn
                (kill-to-end-of-line)
                (p4-all-sections-exclude-all)
            )
        )
        (novalue)
    )
)

(defun
    (p4-add-review-line
        (if (= mode-string "P4 Change")
            (progn
                (beginning-of-file)
                (ere-search-forward "^Description:")
                (ere-search-forward "^[A-Z]")
                (beginning-of-line)
                (insert-string "\t" "#review " p4-reviewers "\n")
                (novalue)
            )
            (error-occurred "p4-add-review-line is only valid in P4 Change mode")
        )
    )
)

(defun
    (p4-spell-check-description
        (save-window-excursion
            (save-restriction
                (beginning-of-file)
                (re-search-forward "^Description:\n")
                (set-mark)
                (re-search-forward "^Jobs:")
                (beginning-of-line)
                (narrow-region)
                (beginning-of-file)
                (spell-check-buffer)
                (unset-mark)
            )
        )
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
            (if (ere-looking-at "\t(# )?//")
                (progn
                    (ere-search-forward "\t(# )?(//.*?)[\t ]")
                    (region-around-match 2)
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
            (if (ere-looking-at "\t(# )?//")
                (progn
                    (ere-search-forward "\t(# )?(//.*?)[\t ]")
                    (region-around-match 2)
                    (execute-monitor-command
                        (concat "P4DIFF='diff -u' p4 diff \"" (region-to-string) "\""))
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
