(defun
    (~helpchar-where-is
        i j
        (setq i (get-tty-command ": where-is "))
        (save-window-excursion
            (describe-bindings)
            (if (error-occurred (search-forward i))
                (error-message
                    (concat "Function \"" i "\" is not bound to a key")
                )
            )
            (beginning-of-line)
            (set-mark)
            (search-forward " ")
            (backward-character)
            (setq j (region-to-string))
            (re-search-forward "[ ]*")
            (set-mark)
            (end-of-line)
            (message
                (concat "Function """ (region-to-string) """ is bound to "
                    j)
            )
        )
    )
)
