; 
; load up $sjcdef symbols
; 
(if
    (if (is-bound $sjcdef-loaded)
	(! $sjcdef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "sjc_def")
	(declare-global $sjcdef-loaded)
	(setq $sjcdef-loaded 1)
    )
)
