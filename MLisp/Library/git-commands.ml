(defun
    (git-diff
        ~diff-buffer
        (setq ~diff-buffer current-buffer-name)
        (execute-monitor-command (concat "git diff \"" current-buffer-file-name "\""))
        (pop-to-buffer "git diff")
        (erase-buffer)
        (yank-buffer "command execution")
        (setq diff-buffer ~diff-buffer)
        (beginning-of-file)
        (diff-mode)
    )
)
