;
;   makefile-mode.ml
;
; Copyright (c) 2004-2016 Barry A. Scott
;
(defun
    (makefile-mode
        (setq indent-use-tab 1)
        (setq logical-tab-size 8)
        (setq tab-size 8)
        (setq mode-string "Makefile")
        (use-abbrev-table "Makefile")
    )
)

(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (use-syntax-table "Makefile")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )

    ; use sh-modes syntax table for the shell commands
    (load "sh-mode")
    (~sh-mode-setup-syntax-table)

    (delete-buffer "keymap-hack")
)
