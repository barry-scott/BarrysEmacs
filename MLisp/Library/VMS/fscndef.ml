;
; load up $fscndef symbols
;
(if
    (if (is-bound $fscndef-loaded)
        (! $fscndef-loaded)
        1
    )
    (progn
        (execute-mlisp-file "fscn_def")
        (declare-global $fscndef-loaded)
        (setq $fscndef-loaded 1)
    )
)
