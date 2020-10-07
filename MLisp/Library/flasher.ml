(progn

(declare-global paren-flash-loaded)
(declare-buffer-specific paren-flash-wait paren-flash-message)
(if (! paren-flash-loaded)
    (progn
        (setq-default paren-flash-message 1)
        (setq-default paren-flash-wait 10)
    )
)
(setq paren-flash-loaded 1)

(defun
    (paren-flash                        ; "("
        (insert-character (last-key-struck))
        (paren-flash-only)
    )
)

(defun
    (paren-flash-only
        (save-excursion
            (if (! (backward-paren 0))(error-message "No match"))
            (if (dot-is-visible)
                (sit-for paren-flash-wait)
                (if paren-flash-message
                    (progn ~s
                        (forward-character)
                        (set-mark)(beginning-of-line)
                        (setq ~s (concat (region-to-string) "<< "))
                        (end-of-line)(setq ~s (concat ~s (region-to-string)))
                        (message ~s)
                    )
                )
            )
        )
    )
)
)
