(progn
(declare-global
    ~template-array
    ~template-depth
)
(setq ~template-array (array 1 11 0 50 0 3))
(setq ~template-depth 0)
(defun template-exit ()
    (save-window-excursion
        (goto-character (- template-end-field 1))
        (set-mark)
        (goto-character (+ (fetch-array ~template-array ~template-depth 0 0)))
        (erase-region)
    )
    (error-occurred (setq ~template-exit 1))
)
(defun template-driver (~template)
    ~template-indent
    ~template-count
    ~template-i
    ~template-exit
    template-end-field
    template-start-field
    template-exit-dot
    (setq ~template-depth (+ ~template-depth 1))
    (setq template-exit-dot 0)
    (setq ~template-exit 0)
    (setq ~template-count (~template-insert ~template (current-indent)))
    (setq ~template-i 1)
    (while (& (! ~template-exit) (<= ~template-i ~template-count))
        (goto-character (+ (fetch-array ~template-array ~template-depth ~template-i 2)))
        (save-window-excursion
            (set-mark)
            (goto-character (+ (fetch-array ~template-array ~template-depth ~template-i 3)))
            (erase-region)
        )
        (setq template-start-field (dot))
        (save-window-excursion
            (if (error-occurred (forward-character))
                (insert-character '\n')
            )
            (setq template-end-field (dot))
        )
        (execute-mlisp-line (fetch-array ~template-array ~template-depth ~template-i 1))
        (setq ~template-i (+ 1 ~template-i))
    )
    (if template-exit-dot
        (goto-character template-exit-dot)
    )
    (setq ~template-depth (- ~template-depth 1))
    (novalue)
)
(defun ~template-insert (~template-buffer ~template-indent 0)
    ~template-index
    ~template-count
    (save-window-excursion
        (set-mark)
        (yank-buffer ~template-buffer)
        (setq-array ~template-array ~template-depth 0 0 (dot))
        (narrow-region)
        (if ~template-indent
            (progn
                (beginning-of-file)
                (while (! (error-occurred (re-search-forward "\n")))
                    (to-col ~template-indent)
                )
            )
        )
        (beginning-of-file)
        (setq ~template-count 0)
        (while (! (error-occurred (re-search-forward "