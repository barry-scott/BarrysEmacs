(declare-global
    p4-reviewers
    p4-change-remove-jobs-section
    p4-remove-header-comments)
(setq p4-change-remove-jobs-section 0)
(setq p4-remove-header-comments 0)
(setq p4-reviewers "")

(autoload "diff-mode" "diff.ml")

(defun
    (p4-tab-key
        (insert-string "\t")
    )
)

(defun
    (p4-edit
        (execute-monitor-command (concat "p4 edit \"" current-buffer-file-name "\""))
        ; pick up the change
        (synchronise-files)
        ; show the result of the open
        (save-window-excursion
            (pop-to-buffer "command execution")
            (get-tty-string "P4 edit result: ")
        )
    )
)

(defun
    (p4-diff
        ~diff-buffer
        (setq ~diff-buffer current-buffer-name)
        ; use diff -u but ensure that we get both sets of file detail lines
        ; by using -du
        (execute-monitor-command (concat "P4DIFF='diff -u' p4 diff -du \"" current-buffer-file-name "\""))
        (pop-to-buffer "P4 diff")
        (erase-buffer)
        (yank-buffer "command execution")
        (setq diff-buffer ~diff-buffer)
        (beginning-of-file)
        ; remove the extra diff headers from diff -u but keep
        ; the p4 diff -du headers
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

(autoload "smart-auto-register-handler" "smart-auto-execute.ml")
(autoload "p4-change-mode" "p4-change-mode.ml")
(autoload "p4-clientspec-mode" "p4-clientspec-mode.ml")
(smart-auto-register-handler "# A Perforce Change Specification." "p4-change-mode")
(smart-auto-register-handler "# A Perforce Client Specification." "p4-clientspec-mode")
