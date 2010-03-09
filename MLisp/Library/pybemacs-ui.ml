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
