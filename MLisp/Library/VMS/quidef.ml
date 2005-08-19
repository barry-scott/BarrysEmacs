; 
; load up $quidef symbols
; 
(if
    (if (is-bound $quidef-loaded)
	(! $quidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "qui_def")
	(declare-global $quidef-loaded)
	(setq $quidef-loaded 1)
    )
)
