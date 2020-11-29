;
; P4  clientspec mode
;
(defun
    (p4-clientspec-mode
        (setq mode-string "P4 Clientspec")
        (use-syntax-table "P4 Clientspec")
        (use-local-map "P4-clientspec-map")
        (use-abbrev-table "P4-clientspec")

        (if p4-remove-header-comments
            (progn
                (beginning-of-file)
                ; leave the first line that identifies the file as a p4 change
                (next-line)
                (set-mark)
                (re-search-forward "^Client:")
                (beginning-of-line)
                (erase-region)
                (unset-mark)
            )
        )

        (beginning-of-file)
        (re-search-forward "^View:")
        (line-to-top-of-window)
        (next-line)
        (novalue)
    )
)

(save-window-excursion
    (temp-use-buffer "~p4-hack")

    (define-keymap "P4-clientspec-map")
    (define-keymap "P4-clientspec-ESC-map")
    (use-local-map "P4-clientspec-map")
    (local-bind-to-key "P4-clientspec-ESC-map" "\e")
    (execute-mlisp-file "p4-clientspec-mode.key")

    (use-syntax-table "P4 Clientspec")
    (modify-syntax-table "word" "_")
    (modify-syntax-table "comment" "#" "\n")

    (modify-syntax-table "keyword-1"  "View:")
    (modify-syntax-table "keyword-1"  "Client")
    (modify-syntax-table "keyword-1"  "Update")
    (modify-syntax-table "keyword-1"  "Access")
    (modify-syntax-table "keyword-1"  "Owner")
    (modify-syntax-table "keyword-1"  "Host")
    (modify-syntax-table "keyword-1"  "Description")
    (modify-syntax-table "keyword-1"  "Root")
    (modify-syntax-table "keyword-1"  "Options")
    (modify-syntax-table "keyword-1"  "SubmitOptions")
    (modify-syntax-table "keyword-1"  "LineEnd")

    (kill-buffer "~p4-hack")
)
(novalue)
