;
;   makefile-mode.ml
;
; Copyright (c) 2004-2005 Barry A. Scott
; 
(defun
    (makefile-mode
        (setq indent-use-tab 1)
        (setq logical-tab-size 8)
        (setq tab-size 8)
        (sh-mode)
        (setq mode-string "Makefile")
    )
)
