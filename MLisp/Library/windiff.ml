;
; windiff.ml
; copyright (c) 1995 Barry A. Scott
;

;
;   diff the current window and the next
;   starting from the top
;
(defun
    (window-diff-start
        (beginning-of-file)
        (next-window)
        (beginning-of-file)
        (previous-window)
        (window-diff-continue)
    )
)
(defun
    (window-diff-continue
        ~line
        (while
            (progn
                (beginning-of-line)
                (set-mark)
                (end-of-line)
                (setq ~line (region-to-string))
                (next-window)
                (beginning-of-line)
                (set-mark)
                (end-of-line)
                (exchange-dot-and-mark)
                (= ~line (region-to-string))
            )
            ;(sit-for 0)
            (next-line)
            (previous-window)
            (forward-character)
        )
    )
)
