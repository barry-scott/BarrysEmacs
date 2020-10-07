(declare-buffer-specific buffer-file-reload-mode-hook)
(setq-default buffer-file-reload-mode-hook "novalue")

(defun
    emacs-file-reload-handler( ~buffer-name )
    (save-excursion
        (temp-use-buffer ~buffer-name)
        (execute-mlisp-line
            (concat "(" buffer-file-reload-mode-hook ")")
        )
    )
)

(setq buffer-file-reloaded-hook "emacs-file-reload-handler")
