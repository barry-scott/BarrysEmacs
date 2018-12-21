;
;   diff.ml
;
(declare-global
    diff-command)               ; holds the diff command to use

(declare-buffer-specific
    diff-buffer)                ; holds the buffer that was last differ

; diff current buffer against previous state
(defun
    (diff
        ~prefix

        (setq ~prefix prefix-argument-provided)

        (if (= current-buffer-type "file")
            (progn
                ~buffer
                ~filename
                ~backup-filename
                ~buffer-is-modified

                (setq ~buffer current-buffer-name)
                (setq ~filename current-buffer-file-name)
                (setq ~buffer-is-modified (!= buffer-is-modified 0))

                (setq ~backup-filename (file-format-string backup-filename-format ~filename))
                (if ~buffer-is-modified
                    (save-excursion
                        (save-restriction
                            (widen-region)
                            (beginning-of-file)
                            (set-mark)
                            (end-of-file)
                            (write-region-to-file "_diff_.tmp")
                        )
                    )
                )
                ; see it is possible to do the diff
                (if (| ~buffer-is-modified (file-exists ~backup-filename))
                    (progn
                        (pop-to-buffer "diff")
                        (setq diff-buffer ~buffer)
                        (diff-mode)
                        (erase-buffer)
                        (set-mark)

                        (if (| ~prefix (! ~buffer-is-modified))
                            (filter-region
                                (concat diff-command " \"" ~backup-filename "\" \"" ~filename "\"") )
                            (filter-region
                                (concat diff-command " \"" ~filename "\"  _diff_.tmp") )
                        )


                        (beginning-of-file)
                        (unset-mark)
                        (if ~buffer-is-modified
                            (unlink-file "_diff_.tmp")
                        )
                        (novalue)
                    )
                    ; else nothing to do
                    (message "Nothing to diff")
                )
            )
            ; else nothing to do
            (message "Nothing to diff")
        )
    )
)

(defun
    (diff-mode
        (if (= diff-buffer 0)
            (setq mode-string "Diff")
            (setq mode-string (concat "Diff of " diff-buffer))
        )
        (use-local-map "diff-mode-map")
        (use-syntax-table "diff-mode")

        (save-excursion
            (end-of-file)
            (set-mark)
            (beginning-of-file)
            (apply-colour-to-region (dot) (mark) 0)
            (unset-mark)
            (while (! (error-occurred (ere-search-forward "^\\+.*")))
                (region-around-match 0)
                (apply-colour-to-region (dot) (mark) 1)
            )
            (beginning-of-file)
            (while (! (error-occurred (ere-search-forward "^\\-.*")))
                (region-around-match 0)
                (apply-colour-to-region (dot) (mark) 2)
            )
        )
        (novalue)
    )
)
;
; Bound to return
;
(defun
    (diff-goto-diff
        ~line

        (if (= operating-system-name "Windows")
            (save-excursion
                (beginning-of-line)
                (while (& (! (bobp)) (! (looking-at " *[0-9]")))
                    (previous-line)
                )
                (looking-at " *[0-9][0-9]*")
                (region-around-match 0)
                (setq ~line (+ (region-to-string)))
            )
            (save-excursion
                (beginning-of-file)
                (looking-at "--- ")
            )
            ; diff -u output
            (save-excursion
                (setq ~line (current-line-number))
                (beginning-of-line)
                (while (& (! (bobp)) (! (looking-at "@@ ")))
                    (if (looking-at "-")
                        (setq ~line (- ~line 1))
                    )
                    (previous-line)
                )
                (ere-looking-at ".*\\+(\\d+),")
                (region-around-match 1)
                (setq ~line
                    (+ (region-to-string) (- ~line (current-line-number) 1)))
            )
            ; default parsing - unix style
            (save-excursion
                (beginning-of-line)
                (while (& (! (bobp)) (! (looking-at "[0-9]")))
                    (previous-line)
                )
                (looking-at "[0-9][0-9]*")
                (region-around-match 0)
                (setq ~line (+ (region-to-string)))
            )
        )
        (pop-to-buffer diff-buffer)
        (goto-line ~line)
    )
)

(if (= diff-command 0)
    (if (= operating-system-name "unix")
        (setq diff-command "diff -u")
        (= operating-system-name "vms")
        (setq diff-command "diff")
        (= operating-system-name "Windows")
        (setq diff-command "fc /n")
        ; default
        (setq diff-command "diff")
    )
)

;
; diff syntax table
;
(save-window-excursion
    (temp-use-buffer "~diff-mode-hack")
    (define-keymap "diff-mode-map")
    (define-keymap "diff-mode-ESC-map")

    (use-local-map "diff-mode-map")
    (local-bind-to-key "diff-mode-ESC-map" "\e")

    (execute-mlisp-file "diff.key")
    (kill-buffer "~diff-mode-hack")
)
(novalue)
