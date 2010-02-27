;
;   pybemacs-ui.ml
;
(defun UI-edit-copy()
    (error-occurred
        (UI-python-hook "edit-copy" (region-to-string))
    )
)

(defun UI-edit-paste()
    (error-occurred
        (insert-string (UI-python-hook "edit-paste"))
    )
)

(defun UI-frame-state()
    "normal"
)
