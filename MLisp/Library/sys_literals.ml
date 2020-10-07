;
; sys_literal.ml
;   Copyright (c) 1996 Barry A. Scott
;

(defun
    (~sys_literal_setup_incr
        ~prefix ~value ~incr ~name ~index

        (setq ~prefix (arg 1))
        (setq ~value (arg 2))
        (setq ~incr (arg 3))
        (setq ~index 4)
        (while (<= ~index (nargs))
            (progn
                (setq ~name (concat ~prefix (arg ~index)))
                (if (!= ~name "")
                    (progn
                        (execute-mlisp-line
                            (concat "(declare-global " ~name ")"))
                        (execute-mlisp-line
                            (concat "(setq " ~name " " ~value ")"))
                    )
                )
                (setq ~index (+ 1 ~index))
                (setq ~value (+ ~value ~incr))
            )
        )
    )
)
(defun
    (~sys_literal_setup_mask
        ~prefix ~value ~name ~index

        (setq ~prefix (arg 1))
        (setq ~value (arg 2))
        (setq ~index 3)
        (while (<= ~index (nargs))
            (progn
                (setq ~name (concat ~prefix (arg ~index)))
                (if (!= ~name "")
                    (progn
                        (execute-mlisp-line
                            (concat "(declare-global " ~name ")"))
                        (execute-mlisp-line
                            (concat "(setq " ~name " " ~value ")"))
                    )
                )
                (setq ~index (+ 1 ~index))
                (setq ~value (<< ~value 1))
            )
        )
    )
)
(defun
    (~sys_literal_setup_equal
        ~prefix ~name ~index

        (setq ~prefix (arg 1))
        (setq ~index 2)
        (while (<= ~index (nargs))
            (progn
                (setq ~name (concat ~prefix (arg ~index)))
                (setq ~index (+ 1 ~index))
                (if (!= ~name "")
                    (progn
                        (execute-mlisp-line
                            (concat "(declare-global " ~name ")"))
                        (execute-mlisp-line
                            (concat "(setq " ~name " " (arg ~index) ")"))
                    )
                )
                (setq ~index (+ 1 ~index))
            )
        )
    )
)
(declare-global sys_literal_loaded)
