;
; describe-word-in-buffer
; Copyright 1998 (c) Barry A. Scott
;

(defun ~describe-generic-word-in-buffer
    (
        ~type
    )
    ~word
    (save-excursion
        ;
        ; make sure that there is a word here
        (if (looking-at "\\w")
            (progn
                (error-occurred (forward-character))
                (backward-word)
                (set-mark)
                (forward-word)
            )
            (progn
                (error-occurred (backward-character))
                (if (looking-at "\\w")
                    (progn
                        (error-occurred (forward-character))
                        (backward-word)
                        (set-mark)
                        (forward-word)
                    )
                    (set-mark)
                )
            )
        )
        (setq ~word (region-to-string))
        (push-back-character ~word)
        (execute-mlisp-line
            (concat
                "(describe-" ~type " (get-tty-" ~type " \": expand-" ~type "-in-buffer \"))"
            )
        )
    )
    (novalue)
)
(defun
    (describe-command-in-buffer
        (~describe-generic-word-in-buffer "command")
    )
)
(defun
    (describe-variable-in-buffer
        (~describe-generic-word-in-buffer "variable")
    )
)
