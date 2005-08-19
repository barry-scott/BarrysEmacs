; 
; load up $syidef symbols
; 
(if
    (if (is-bound $syidef-loaded)
	(! $syidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "syi_def")
	(declare-global $syidef-loaded)
	(setq $syidef-loaded 1)
    )
)
