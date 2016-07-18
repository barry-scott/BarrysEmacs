(defun
    (~info-save
        (switch-to-buffer "INFO edit")
        (put-database-entry "info" ~info-node-part)
        (setq ~info-^n-file ~info-file-part)
        (setq ~info-^n-node ~info-node-part)
        (setq ~info-^n-help-flag ~info-help-part)
        (setq buffer-is-modified 0)
        (exit-emacs)
    )
)
