;
; load up $uaidef symbols
;
(if
    (if (is-bound $uaidef-loaded)
	(! $uaidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "uai_def")
	(declare-global $uaidef-loaded)
	(setq $uaidef-loaded 1)
    )
)
