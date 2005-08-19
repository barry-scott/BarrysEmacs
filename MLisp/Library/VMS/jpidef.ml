; 
; load up $jpidef symbols
; 
(if
    (if (is-bound $jpidef-loaded)
	(! $jpidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "jpi_def")
	(declare-global $jpidef-loaded)
	(setq $jpidef-loaded 1)
    )
)
