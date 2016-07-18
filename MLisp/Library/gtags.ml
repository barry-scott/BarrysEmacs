;
;
; gtags.ml
;
; Copyright 1999 (c) Barry A. Scott
;
;
(defun ~gtags-global-command(~command ~arg)
    (save-excursion
        (temp-use-buffer "~gtags-global-command-output")
        (erase-buffer)
        (set-mark)
        (filter-region (concat "global " ~command " " ~arg))
        (unset-mark)
        (beginning-of-file)
    )
)


;
; extract part of a global tag description
;
(defun
    (~gtags-extract-location
        (beginning-of-line)
        (looking-at "^\\([^ ][^ ]*\\)  *\\([^ ][^ ]*\\)  *\\([^ ][^ ]*\\)  *\\(.*\\)")
        (region-around-match 2)     (setq ~line-number (+ (region-to-string)))
        (region-around-match 3)     (setq ~src-file (region-to-string))
        (region-around-match 4)     (setq ~context (region-to-string))
    )
)

(defun
    gtags-goto-definition( ~tag (get-tty-string ": gtags-goto-definition (tag) "))

    (~gtags-goto-tag "-x" ~tag)
)

(defun
    gtags-goto-reference( ~tag (get-tty-string ": gtags-goto-reference (tag) "))

    (~gtags-goto-tag "-gx" ~tag)
)

(defun
    gtags-goto-symbol( ~tag (get-tty-string ": gtags-goto-symbol (tag) "))

    (~gtags-goto-tag "-sx" ~tag)
)

(defun
    ~gtags-goto-tag( ~cmd ~tag )

    ~src-file ~line-number ~context

    (~gtags-global-command ~cmd ~tag)

    (save-window-excursion
        ~num-lines

        (pop-to-buffer "~gtags-global-command-output")

        (end-of-file)
        (setq ~num-lines (- (current-line-number) 1))
        (beginning-of-file)

        (if
            (= ~num-lines 0)
            (error-message "No such tag found - " ~tag)

            (= ~num-lines 1)
            (~gtags-extract-location)

            (progn
                (local-bind-to-key "exit-emacs" "\r")
                (message "Select the tag to goto and press enter")
                (recursive-edit)
                (if (eobp)
                    (previous-line)
                )
                (~gtags-extract-location)
            )
        )
    )
    (visit-file ~src-file)
    (goto-line ~line-number)
    (save-window-excursion
        (recursive-edit)
    )
)

(defun
    (gtags-goto-definition-in-buffer
        (~gtags-goto-in-buffer "-x")
    )
)
(defun
    (gtags-goto-reference-in-buffer
        (~gtags-goto-in-buffer "-gx")
    )
)
(defun
    (gtags-goto-symbol-in-buffer
        (~gtags-goto-in-buffer "-sx")
    )
)
(defun
    ~gtags-goto-in-buffer(~cmd)
    ~tag

    (if
        (looking-at "\\w")
        (save-excursion
            (forward-character)
            (backward-word)
            (set-mark)
            (forward-word)
            (setq ~tag (region-to-string))
        )
        (looking-at "\\b")
        (save-excursion
            (set-mark)
            (backward-word)
            (setq ~tag (region-to-string))
        )
        (error-message "Position the cursor over a word")
    )

    (~gtags-goto-tag ~cmd ~tag)
)
(execute-mlisp-file "gtags.key")
