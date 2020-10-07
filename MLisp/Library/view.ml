;;; Corrected some problems caused by upgrading to V3
;;; Made use of get-tty-file command in view-file
;;; Added exit if end of buffer is reached and space is entered again
;;; Added same thing for backing up through the file
;;;
;;; Bob Surtees
;;;     +----------------+----------------------------------------------+
;;;     | File Name:     |                                              |
;;;     | Edit Version:  | 1                                            |
;;;     | Date Modified: | Tue Aug 23 12:29:43 1983                     |
;;;     | Updated By:    | SURTEES                                      |
;;;     | From Node:     | _SIVA::                                      |
;;;     +----------------+----------------------------------------------+
;;;
(progn
    ~VIEW-i
    (declare-global ~VIEW-flag ~VIEW-mode-line)
    (define-keymap "~VIEW-map")
    (use-global-map "~VIEW-map")
    (setq ~VIEW-i 0)
    (while (<= ~VIEW-i 127)
        (bind-to-key "exit-emacs" ~VIEW-i)
        (setq ~VIEW-i (+ ~VIEW-i 1))
    )
    (bind-to-key "~VIEW-next-page" " ")
    (bind-to-key "~VIEW-previous-page" "\^h")
    (use-global-map "default-global-keymap")

(defun
    (~VIEW-next-page
        (if (eobp)
            (push-back-character ''))
        (next-page)
        (~VIEW-mode-line)
    )
)
(defun
    (~VIEW-previous-page
        (if (bobp)
            (push-back-character ''))
        (previous-page)
        (~VIEW-mode-line)
    )
)
(defun
    (~VIEW-mode-line
        (setq mode-line-format
            (concat
                "%[" ~VIEW-mode-line
                (if (save-excursion (end-of-file) (dot-is-visible))
                    ""
                    "     --more--"
                )
                "%]"
            )
        )
    )
)
(defun
    (view-buffer
        ~VIEW-buffer ~VIEW-bufnam ~VIEW-dot
        (setq ~VIEW-buffer (current-buffer-name))
        (if (! (length (setq ~VIEW-bufnam (arg 1 ": view-buffer "))))
            (setq ~VIEW-bufnam (current-buffer-name))
        )
        (if (error-occurred (save-window-excursion (use-old-buffer ~VIEW-bufnam)))
            (error-message
                (concat
                    "Buffer \""
                    ~VIEW-bufnam
                    "\" does not exist")
            )
        )
        (switch-to-buffer "~VIEW-buffer")
        (setq current-buffer-checkpointable 0)
        (setq current-buffer-journalled 0)
        (erase-buffer)
        (yank-buffer ~VIEW-bufnam)
        (use-local-map "~VIEW-map")
        (beginning-of-file)
        (setq ~VIEW-mode-line
            (concat
                "VIEW buffer (%3p): "
                ~VIEW-bufnam
            )
        )
        (~VIEW-mode-line)
        (message "(Type SPACE for next page, BACKSPACE for previous page)")
        (recursive-edit)
        (if (!= (last-key-struck) 13)
            (switch-to-buffer ~VIEW-buffer)
            (progn
                (setq ~VIEW-dot (dot))
                (switch-to-buffer ~VIEW-bufnam)
                (goto-character ~VIEW-dot)
            )
        )
        (if
            (&
                (&
                    (&
                        (!= (last-key-struck) 3)
                        (!= (last-key-struck) 035)
                    )
                    (!= (last-key-struck) 13)
                )
                (!= (last-key-struck) 0177)
            )
            (push-back-character (last-key-struck))
        )
        (kill-buffer "~VIEW-buffer")
        (novalue)
    )
)
(defun
    view-file (~VIEW-filename (get-tty-file "File name: ")
                  ~VIEW-buffer (current-buffer-name)
                  ~VIEW-viewb (concat "VIEW " (current-time)))
    (switch-to-buffer ~VIEW-viewb)
    (if (error-occurred (insert-file ~VIEW-filename))
        (progn
            (switch-to-buffer ~VIEW-buffer)
            (kill-buffer ~VIEW-viewb)
            (error-message
                (concat
                    "Can't access file \""
                    ~VIEW-filename
                    "\""
                )
            )
        )
    )
    (use-global-map "~VIEW-map")
    (beginning-of-file)
    (setq ~VIEW-mode-line
        (concat
            "VIEW file (%3p): "
            ~VIEW-filename
        )
    )
    (~VIEW-mode-line)
    (message "(Type SPACE for next page, BACKSPACE for previous page)")
    (recursive-edit)
    (use-global-map "default-global-keymap")
    (switch-to-buffer ~VIEW-buffer)
    (if
        (&
            (&
                (&
                    (!= (last-key-struck) 3)
                    (!= (last-key-struck) 035)
                )
                (!= (last-key-struck) 13)
            )
            (!= (last-key-struck) 0177)
        )
        (push-back-character (last-key-struck))
    )
    (kill-buffer ~VIEW-viewb)
    (novalue)
)
)
