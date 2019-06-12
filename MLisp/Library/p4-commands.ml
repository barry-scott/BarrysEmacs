(declare-global p4-reviewers)
(setq p4-reviewers "")

(autoload "diff-mode" "diff.ml")

(defun
    (p4-tab-key
        (insert-string "\t")
    )
)

(defun
    (p4-open
        (execute-monitor-command (concat "p4 open \"" current-buffer-file-name "\""))
        ; pick up the change
        (synchronise-files)
        ; show the result of the open
        (save-window-excursion
            (pop-to-buffer "command execution")
            (get-tty-string "P4 open result: ")
        )
    )
)

(defun
    (p4-diff
        ~diff-buffer
        (setq ~diff-buffer current-buffer-name)
        (execute-monitor-command (concat "p4 diff -du \"" current-buffer-file-name "\""))
        (pop-to-buffer "P4 diff")
        (erase-buffer)
        (yank-buffer "command execution")
        (setq diff-buffer ~diff-buffer)
        (beginning-of-file)
        ; remove the duplicate diff headers
        (next-line 2)
        (set-mark)
        (next-line 2)
        (erase-region)
        (unset-mark)
        (diff-mode)
    )
)

(defun
    p4-clientspec-to-filename( ~clientspec)
    (save-window-excursion
        (execute-monitor-command (concat "p4 fstat -T clientFile \"" ~clientspec "\""))
        (pop-to-buffer "command execution")
        (beginning-of-file)
        (ere-search-forward "... clientFile ")
        (set-mark)
        (end-of-line)
        (region-to-string)
    )
)

(defun
    (p4-add-review-line
        (beginning-of-file)
        (ere-search-forward "^Description:")
        (ere-search-forward "^[A-Z]")
        (beginning-of-line)
        (insert-string "\t" "#review " p4-reviewers "\n")
        (novalue)
    )
)

(autoload "smart-auto-register-handler" "smart-auto-execute.ml")
(autoload "p4-change-mode" "p4-change-mode.ml")
(autoload "p4-clientspec-mode" "p4-clientspec-mode.ml")
(smart-auto-register-handler "# A Perforce Change Specification." "p4-change-mode")
(smart-auto-register-handler "# A Perforce Client Specification." "p4-clientspec-mode")
