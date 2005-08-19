; 
; load up $dvidef symbols
; 
(if
    (if (is-bound $dvidef-loaded)
	(! $dvidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "dvi_def")
	(declare-global $dvidef-loaded)
	(setq $dvidef-loaded 1)
    )
)
