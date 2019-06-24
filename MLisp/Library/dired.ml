;
; dired, the directory editor, version V3.0
; This version allows non-sub-process directories using the
; expand-file-name function. To get this, you must pre-declare
; the global dired-directory-command, and set it to the string
; FAST (in big letters), or after you load dired, you must
; set this variable to a null string.
;
(declare-global Dired-directory)
(setq Dired-directory "")

(defun
    dired( ~dired-directory (get-tty-directory
        (concat
            ": dired of directory? ["
            (if (= Dired-directory "") (current-directory) Dired-directory)
            "]")))
    (save-window-excursion
        (if (= ~dired-directory "")
            (setq ~dired-directory (if (= Dired-directory "") (current-directory) Dired-directory))
            ; save the users choice - leaving blank if using
            ; current-directory
            (setq Dired-directory ~dired-directory)
        )
        ; normalise
        (setq ~dired-directory (file-name-expand-and-default ~dired-directory ""))

        ; setup dired buffer
        (pop-to-buffer "dired")
        (use-local-map "dired-keymap")
        (setq mode-line-format
            (concat "  Editing  directory:  " ~dired-directory
                "      %M   %[%p%]"))

        ; get the directory listing
        (erase-buffer)
        (set-mark)
        (message "Getting contents of " ~dired-directory)
        (sit-for 0)
        (filter-region (concat "/bin/ls -l " ~dired-directory))

        (beginning-of-file)
        (if (looking-at "total")
            (progn
                (kill-to-end-of-line 2)
                (beginning-of-file)
                (re-replace-string "^." " &")
                (beginning-of-file)
            )
            (progn
                (end-of-file)
                (error-message (region-to-string))
            )
        )
        (unset-mark)

        (message "Type q to exit, ? for help")
        (save-excursion (recursive-edit))

        (~dired-done)
        (novalue)
    )
)

(defun
    (~dired-Mark-file-deleted
        (if (= 0 (buffer-size))
            (error-message "dired already done!")
            (progn
                (beginning-of-line)
                (if (looking-at " d")
                    (error-message "Can't delete a directory with dired"))
                (delete-next-character)
                (insert-string "D")
                (next-line)
                (beginning-of-line)
            )
        )
    )
)

(defun
    (~dired-summary
        (message
            "d-elete, u-ndelete, q-uit, r-emove, e,v-isit, n-ext, p-revious, ?-help")
    )
)

(defun
    (~dired-UnMark-file-deleted
        (if (= 0 (buffer-size))
            (error-message "dired already done!")
            (progn
                (beginning-of-line)
                (delete-next-character)
                (insert-string " ")
                (next-line)
                (beginning-of-line)
            )
        )
    )
)

(defun
    (~dired-backup-unmark
        (if (= 0 (buffer-size))
            (error-message "dired already done!")
            (! (bobp))
            (previous-line))
        (beginning-of-line)
        (delete-next-character)
        (insert-string " ")
        (beginning-of-line)
    )
)

(defun
    (~dired-examine
        ~mode
        (setq ~mode (~dired-get-mode))
        (if (| (= ~mode '-') (= ~mode 'l'))
            (save-excursion
                (beginning-of-line)
                (error-occurred
                    (visit-file (~dired-get-fname))
                    (message "Type ^X^C to return to DIRED")
                    (recursive-edit)
                )
            )
            (message "Not a file")
        )
    )
)

(defun
    (~dired-remove
        (beginning-of-line)
        (kill-to-end-of-line 2)
    )
)

(defun
    (~dired-get-mode
        (save-excursion
            (beginning-of-line)
            (forward-character)
            (following-char)
        )
    )
)

(defun
    (~dired-get-fname
        (save-excursion
            ~is-link
            ~is-quoted

            (beginning-of-line)
            ; need to know its s synlink to find the end of the name
            (setq ~is-link (= (~dired-get-mode) 'l'))

            ; start of file name if after the date and time
            (ere-search-forward "\\d [A-Za-z][a-z][a-z] [ 1-9][0-9] ( \\d{4}|\\d{2}:\\d{2}) ")

            ; some /bin/ls use single quotes around names with spaces
            (setq ~is-quoted (looking-at "'"))
            (if ~is-quoted
                (forward-character))
            (set-mark)
            (if ~is-link
                (progn
                    ; symlinks are symlink -> filename
                    (search-forward " -> ")
                    (backward-character 4)
                )
                (end-of-line)
            )
            (if ~is-quoted
                (backward-character))

            (concat ~dired-directory "/" (region-to-string))
        )
    )
)

(defun
    (~dired-done
        ~answer
        (beginning-of-file)
        (re-replace-string "^ .*\n" "")
        (if (!= 0 (buffer-size))
            (progn
                (message
                    "? [y-go through marked files; e-don't delete, exit; Anything else return]")
                (setq ~answer (get-tty-character))
                (if (| (= ~answer 'e') (= ~answer 'E'))
                    (progn
                        (message
                            "Really exit without deleting?[y-yes, anything else continue dired]")
                        (setq ~answer (get-tty-character))
                        (if (| (= ~answer 'y') (= ~answer 'Y'))
                            (delete-buffer "dired")
                            (error-message "Aborted."))
                        )
                    (| (= ~answer 'y') (= ~answer 'Y'))
                    (progn
                        (while (! (eobp))
                            (if (= (following-char) 'D')
                                (progn thisfile ~answer
                                    (setq thisfile (~dired-get-fname))
                                    (message (concat "Delete " thisfile "?"))
                                    (setq ~answer (get-tty-character))
                                    (if (| (= ~answer 'y') (= ~answer 'Y'))
                                        (if (unlink-file thisfile)
                                            (progn
                                                (message "Couldn't delete it!")
                                                (sit-for 2))))
                                )
                            )
                            (next-line)
                        )
                        (delete-buffer "dired")
                    )
                    (error-message "?Aborted.")
                )
            )
        )
    )
)

(progn
    (save-excursion
        ~loop
        (temp-use-buffer "dired")
        (define-keymap "dired-keymap")
        (use-local-map "dired-keymap")

        (setq ~loop ' ')
        (while (<= ~loop 127)
            (local-bind-to-key "~dired-summary" ~loop)
            (setq ~loop (+ ~loop 1))
        )

        (setq ~loop '0')
        (while (<= ~loop '9')
            (local-bind-to-key "digit" ~loop)
            (setq ~loop (+ ~loop 1))
        )
        (local-bind-to-key "~dired-Mark-file-deleted" "d")
        (local-bind-to-key "~dired-Mark-file-deleted" "D")
        (local-bind-to-key "~dired-Mark-file-deleted" "")
        (local-bind-to-key "~dired-backup-unmark" "\0177")
        (local-bind-to-key "previous-line" "p")
        (local-bind-to-key "previous-line" "P")
        (local-bind-to-key "next-line" "n")
        (local-bind-to-key "next-line" "N")
        (local-bind-to-key "next-line" " ")
        (local-bind-to-key "~dired-examine" "\^X\^V")
        (local-bind-to-key "~dired-UnMark-file-deleted" "u")
        (local-bind-to-key "~dired-UnMark-file-deleted" "U")
        (local-bind-to-key "exit-emacs" "q")
        (local-bind-to-key "exit-emacs" "Q")
        (local-bind-to-key "~dired-examine" "e")
        (local-bind-to-key "~dired-examine" "E")
        (local-bind-to-key "~dired-examine" "v")
        (local-bind-to-key "~dired-examine" "V")
        (local-bind-to-key "~dired-remove" "r")
        (local-bind-to-key "~dired-remove" "R")
    )
    (delete-buffer "dired")
)
