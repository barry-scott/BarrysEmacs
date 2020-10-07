;
;   pybemacs-ui.ml
;
(defun UI-edit-copy()
    (error-occurred
        (UI-python-hook "edit-copy" (region-to-string))
    )
)

(defun UI-edit-paste(~primary 0)
    (error-occurred
        (insert-string (UI-python-hook "edit-paste" ~primary))
    )
)

(defun UI-frame-state()
    "normal"
)

;(if (! (is-bound filter-region))
;    (defun filter-region( ~cmd (get-tty-string ": filter-region (through command) "))
;        ~input-string
;        (setq ~input-string (region-to-string))
;        (erase-region)
;        (insert-string (UI-python-hook "filter-string" ~cmd ~input-string))
;        (novalue)
;    )
;)
