;
; edit-variable.ml
;
; Copyright (c) 2004-2005 Barry A. Scott
;
(defun edit-variable( ~var (get-tty-variable ": edit-variable "))
    (save-window-excursion
        (pop-to-buffer "Edit variable")
        (setq current-buffer-journalled 0)
        (unset-mark)
        (setq mode-string (concat "Variable: " ~var))
        (erase-buffer)
        (insert-string (execute-mlisp-line (concat "(concat " ~var ")")))
        (recursive-edit)
        (beginning-of-file)
        (set-mark)
        (end-of-file)
        (get-tty-string (concat ": set " ~var " " (region-to-string)))
        (set ~var (region-to-string))
        (novalue)
    )
)
