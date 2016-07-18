(defun
    (trace
        x
        (setq x (decompile-current-line))
        (if (> (length x) 0)
            (progn
                (message x)
                (sit-for trace-mode)
            )
        )
    )
)
