;
;   console-ui.ml
;
(declare-global ui-clipboard)
(setq ui-clipboard "")

(defun UI-edit-copy()
    (error-occurred
        (setq ui-clipboard (region-to-string))
        (unset-mark)
    )
)

(defun UI-edit-paste(~primary 0)
        (insert-string ui-clipboard)
)

(defun UI-frame-state()
    "normal"
)

(defun filter-region( ~cmd (get-tty-string ": filter-region (through command) "))
    (error-message "filter-region is not implemented")
)
