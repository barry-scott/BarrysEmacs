;
; dired, the directory editor, version V3.0
; This version allows non-sub-process directories using the
; expand-file-name function. To get this, you must pre-declare
; the global dired-directory-command, and set it to the string
; FAST (in big letters), or after you load dired, you must
; set this variable to a null string.
;
(if (= operating-system-name "VMS")
    (progn
        (progn
        (declare-global ~dired-directory dired-directory-command)
        (if (= (concat dired-directory-command) "0"
        )
            (setq dired-directory-command
                "DIRECTORY/NOHEAD/NOTRAIL/COL=1/SIZE=ALL/DATE"
            )
        )
        (defun
            (~dired-line
                (insert-string (concat "\t" (process-output "dired")))
            )
            (dired
                ~dired-fspec
                (setq ~dired-fspec (arg 1 ": dired (file specification) "))
                (if (<= (length ~dired-fspec) 0)
                    (setq ~dired-fspec "*.*;*")
                )
                (if (| (interactive) (!( |
                                           (<= (length dired-directory-command) 0)
                                           (= dired-directory-command "FAST")
                                       )
                                     )
                    )
                    (progn
                        (message ": dired (file specification) "
                            ~dired-fspec
                            ", please wait..."
                        )
                        (sit-for 0)
                    )
                )
                (save-window-excursion
                    (switch-to-buffer "~dired 0")
                    (setq current-buffer-checkpointable 0)
                    (setq current-buffer-journalled 0)
                    (widen-region)
                    (erase-buffer)
                    (insert-string (concat "    " (expand-file-name ~dired-fspec)))
                    (if (provide-prefix-argument -1 (looking-at "[]>].;"))
                        (progn
                            (setq ~dired-fspec (concat ~dired-fspec "*.*;*"))
                            (erase-buffer)
                            (insert-string (concat "    " (expand-file-name ~dired-fspec)))
                        )
                    )
                    (if (provide-prefix-argument -1 (looking-at "*.*;*"))
                        (progn
                            (provide-prefix-argument 5 (delete-previous-character))
                            (insert-string "No such directory.")
                        )
                    )
                    (save-excursion
                        (beginning-of-line)
                        (forward-character)
                        (set-mark)
                        (re-search-forward "[]>]")
                        (case-region-capitalize)
                        (setq ~dired-directory (region-to-string))
                    )
                    (if (= (preceding-char) ';')
                        (erase-buffer)
                        ( |
                            (<= (length dired-directory-command) 0)
                            (= dired-directory-command "FAST")
                        )
                        (while
                            (!
                                (error-occurred
                                    (insert-string
                                        (concat
                                            "\n\t"
                                            (expand-file-name "")
                                        )
                                    )
                                )
                            )
                            (novalue)
                        )
                        (progn
                            (erase-buffer)
                            (error-occurred (kill-process "dired"))
                            (start-DCL-process "dired")
                            (set-process-output-procedure "dired" "~dired-line")
                            (while (= (wait-for-process-input-request "dired") 0)
                                (get-tty-character)
                            )
                            (send-string-to-process "dired"
                                (concat
                                    "$ "
                                    dired-directory-command
                                    " "
                                    ~dired-fspec
                                )
                            )
                            (while (= (wait-for-process-input-request "dired") 0)
                                (get-tty-character)
                            )
                            (error-occurred (kill-process "dired"))
                        )
                    )
                    (if (! (bobp)) (insert-character '\n'))
                    (setq mode-string ~dired-directory)
                    (setq mode-line-format "DIRED directory editor    Directory: %m    %p")
                    (use-local-map "~dired-map")
                    (use-syntax-table "~dired-syntax")
                    (beginning-of-file)
                    (if (error-occurred (re-replace-string "\t.*[]>]" "\t"))
                        (insert-string "\tNo files found.")
                    )
                    (save-excursion
                        (beginning-of-file)
                        (while (! (eobp))
                            (progn
                                (forward-character)
                                (if (= (following-char) ' ')
                                    (progn
                                        (forward-word)
                                        (goto-character (- (dot) 6))
                                        (insert-string ".")
                                        (backward-character)
                                        (delete-white-space)
                                        (delete-previous-character)
                                        (delete-white-space)
                                        (to-col 48)
                                        (delete-next-character)
                                    )
                                )
                                (beginning-of-line) (next-line)
                            )
                        )
                    )
                    (save-excursion
                        (set-mark)
                        (end-of-file)
                        (case-region-capitalize)
                    )
                    (delete-other-windows)
                    (recursive-edit)
                    (while (~dired-continue) (recursive-edit))
                )
                (if (is-bound ~DIRED-com)
                    (if ~DIRED-com
                        (progn
                            (setq ~DIRED-com 0)
                            (exit-emacs)
                        )
                    )
                )
                (error-occurred (delete-buffer "~dired 0"))
                (error-occurred (delete-buffer "~dired 1"))
                (error-occurred (delete-buffer "~dired 2"))
                (novalue)
            )
            (~dired-continue
                ~dired-char
                (save-window-excursion
                    (switch-to-buffer "~dired 1")
                    (setq current-buffer-checkpointable 0)
                    (setq current-buffer-journalled 0)
                    (erase-buffer)
                    (yank-buffer "~dired 0")
                    (beginning-of-file)
                    (error-occurred (re-replace-string "^\t.*$" ""))
                    (error-occurred (re-replace-string "^\n" ""))
                    (error-occurred (re-replace-string "^D\t\\(.*;[0-9][0-9]*\\).*$" "\\1"))
                    (switch-to-buffer "~dired 2")
                    (setq current-buffer-checkpointable 0)
                    (setq current-buffer-journalled 0)
                    (setq mode-line-format "")
                    (erase-buffer)
                    (yank-buffer "~dired 1")
                    (beginning-of-file)
                    (if (eobp)
                        (progn
                            (message "No files deleted.")
                            0
                        )
                        (progn
                            (while (! (eobp))
                                (to-col 8)
                                (end-of-line)
                                (error-occurred
                                    (delete-next-character)
                                    (to-col 33)
                                    (end-of-line)
                                    (delete-next-character)
                                    (to-col 56)
                                )
                                (next-line)
                                (if (! (eobp))
                                    (beginning-of-line)
                                )
                            )
                            (beginning-of-file)
                            (insert-string
                                (concat
                                    "\nDeleting the following files in directory "
                                    ~dired-directory
                                    "\n\n"
                                )
                            )
                            (end-of-file)
                            (insert-string
                                (concat
                                    "\n\nY -- Go ahead\n"
                                    "R -- Don't delete, resume DIRED\n"
                                    "Anything else exits DIRED without deleting\n"
                                )
                            )
                            (message "OK? ")
                            (setq ~dired-char (get-tty-character))
                            (if (| (= ~dired-char 'Y') (= ~dired-char 'y'))
                                (progn (message "OK? YES") (~dired-nuke) 0)
                                (| (= ~dired-char 'R') (= ~dired-char 'r'))
                                (progn (message "OK? RESUME") 1)
                                (progn (message "OK? NO") 0)
                            )
                        )
                    )
                )
            )
            (~dired-nuke
                (temp-use-buffer "~dired 1")
                (beginning-of-file)
                (while (! (eobp))
                    (beginning-of-line)
                    (set-mark)
                    (end-of-line)
                    (message "Deleting file " ~dired-directory (region-to-string))
                    (save-excursion (switch-to-buffer "~dired 2") (sit-for 0))
                    (if (< 0 (unlink-file (concat ~dired-directory (region-to-string)))
                        )
                        (get-tty-string
                            (concat
                                "Failed to delete file "
                                ~dired-directory
                                (region-to-string)
                                ". Press RETURN to continue")
                        )
                    )
                    (next-line)
                )
            )
            (~dired-n
                (next-line)
                (beginning-of-line)
            )
            (~dired-p
                (previous-line)
                (beginning-of-line)
            )
            (~dired-d
                (beginning-of-line)
                (if (looking-at "^\t")
                    (insert-character 'D')
                )
                (~dired-n)
            )
            (~dired-u
                (beginning-of-line)
                (if (looking-at "^D\t")
                    (delete-next-character)
                )
                (~dired-n)
            )
            (~dired-h
                (save-window-excursion
                    (switch-to-buffer "~dired help")
                    (setq current-buffer-checkpointable 0)
                    (setq current-buffer-journalled 0)
                    (delete-other-windows)
                    (erase-buffer)
                    (insert-string
                        (concat
                            "DIRED:  directory editor\n\n"
                            "n  next line\n"
                            "p  prev line\n"
                            "d  mark this line's file for deletion\n"
                            "u  undelete this line's file\n"
                            "e  view this line's file\n"
                            "q  quit DIRED (you will be prompted to confirm)\n\n"
                            "Other characters perform their usual functions."
                        )
                    )
                    (setq mode-line-format "")
                    (message "-- Type any character to continue DIRED --" )
                    (get-tty-character)
                )
                (delete-buffer "~dired help")
                (novalue)
            )
            (~dired-e
                (save-window-excursion
                    (beginning-of-line)
                    (if (! (error-occurred (re-search-forward "^.*\t\\(.*;[0-9][0-9]*\\).*$")))
                        (progn
                            (region-around-match 1)
                            (setq ~dired-fspec
                                (concat
                                    ~dired-directory
                                    (region-to-string)
                                )
                            )
                            (switch-to-buffer "DIRED view")
                            (setq current-buffer-checkpointable 0)
                            (setq current-buffer-journalled 0)
                            (delete-other-windows)
                            (erase-buffer)
                            (normal-mode)
                            (insert-file ~dired-fspec)
                            (beginning-of-file)
                            (message "Use exit-emacs to resume DIRED")
                            (setq mode-line-format
                                (concat
                                    "DIRED view   File: "
                                    ~dired-fspec
                                    "   %p"
                                )
                            )
                            (recursive-edit)
                            (error-occurred (delete-buffer "DIRED view"))
                        )
                    )
                )
            )
        )
        (save-excursion
            (temp-use-buffer "Keymap hack")
            (define-keymap "~dired-map")
            (use-local-map "~dired-map")
            (local-bind-to-key "~dired-n" "n")
            (local-bind-to-key "~dired-n" "N")
            (local-bind-to-key "~dired-p" "p")
            (local-bind-to-key "~dired-p" "P")
            (local-bind-to-key "~dired-d" "d")
            (local-bind-to-key "~dired-u" "U")
            (local-bind-to-key "~dired-u" "u")
            (local-bind-to-key "~dired-e" "e")
            (local-bind-to-key "~dired-e" "E")
            (local-bind-to-key "~dired-h" "h")
            (local-bind-to-key "~dired-h" "H")
            (local-bind-to-key "~dired-h" "?")
            (local-bind-to-key "~dired-h" "")
            (local-bind-to-key "exit-emacs" "q")
            (local-bind-to-key "exit-emacs" "Q")
            (use-syntax-table "~dired-syntax")
        )
        (delete-buffer "Keymap hack")
        (novalue)
        )
    )
    ; else
    (progn
        (declare-global Dired-keymap-defined Dired-directory)
        (setq Dired-keymap-defined 0)
        (setq Dired-directory (current-directory))

        (defun
            (dired directory
                (save-window-excursion
                    (setq directory (arg 1 (concat ": dired on directory? ["
                                               Dired-directory "] ")))
                    (if (= directory "")
                        (setq directory Dired-directory))
                    (if (= "/" (substr directory -1 1))
                        (setq directory (substr directory 1 -1)))
                    (setq Dired-directory directory)
                    (pop-to-buffer "dired")
                    (setq mode-line-format
                        (concat "  Editing  directory:  " directory
                            "      %M   %[%p%]"))
                    (erase-buffer)  (set-mark)
                    (message "Getting contents of " directory)
                    (sit-for 0)
                    (filter-region (concat "/bin/ls -l " (file-name-expand-and-default directory "")))
                    (beginning-of-file)
                    (if (looking-at "total")
                        (progn
                            (kill-to-end-of-line) (kill-to-end-of-line)
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
                    (message "Type ^C to exit, ? for help")
                    (save-excursion (recursive-edit))
                    (~dired-done)
                    (novalue)
                )
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

            (~dired-summary
                (message
                    "d-elete, u-ndelete, q-uit, r-emove, e,v-isit, n-ext, p-revious, ?-help")
            )

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
            (~dired-examine ans
                (save-excursion
                    (error-occurred
                        (visit-file (~dired-get-fname))
                        (message "Type ^C to return to DIRED")
                        (recursive-edit)
                    )
                )
            )
        )

        (defun
            (~dired-remove
                (beginning-of-line)
                (kill-to-end-of-line) (kill-to-end-of-line)
            )

            (~dired-get-fname
                (save-excursion
                    (beginning-of-line)
                        (re-search-forward "[0-9][0-9]:[0-9][0-9] ")
        ;           (goto-character (+ (dot) 43))
                    (set-mark)
                    (end-of-line)
                    (concat Dired-directory "/" (region-to-string))
                )
            )

            (~dired-done ans
                (beginning-of-file)
                (re-replace-string "^ .*\n" "")
                (if (!= 0 (buffer-size))
                    (progn
                        (message
                            "? [y-go through marked files; e-don't delete, exit; Anything else return]")
                        (setq ans (get-tty-character))
                        (if (| (= ans 'e') (= ans 'E'))
                            (progn
                                (message
                                    "Really exit without deleting?[y-yes, anything else continue dired]")
                                (setq ans (get-tty-character))
                                (if (| (= ans 'y') (= ans 'Y'))
                                    (delete-buffer "dired")
                                    (error-message "Aborted.")))
                            (| (= ans 'y') (= ans 'Y'))
                            (progn
                                (while (! (eobp))
                                    (if (= (following-char) 'D')
                                        (progn thisfile ans
                                            (setq thisfile (~dired-get-fname))
                                            (message (concat "Delete " thisfile "?"))
                                            (setq ans (get-tty-character))
                                            (if (| (= ans 'y') (= ans 'Y'))
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

        (progn loop
            (save-excursion
                (temp-use-buffer "dired")
                (define-keymap "dired-keymap")
                (use-local-map "dired-keymap")
                (setq loop 0)
                (while (<= loop 127)
                    (local-bind-to-key "~dired-summary" loop)
                    (setq loop (+ loop 1))
                )
                (setq loop '0')
                (while (<= loop '9')
                    (local-bind-to-key "digit" loop)
                    (setq loop (+ loop 1))
                )
                (local-bind-to-key "~dired-Mark-file-deleted" "d")
                (local-bind-to-key "~dired-Mark-file-deleted" "D")
                (local-bind-to-key "~dired-Mark-file-deleted" "")
                (local-bind-to-key "~dired-backup-unmark" "\0177")
                (local-bind-to-key "previous-line" "\^H")
                (local-bind-to-key "previous-line" "p")
                (local-bind-to-key "previous-line" "P")
                (local-bind-to-key "previous-line" "\^P")
                (local-bind-to-key "next-line" "n")
                (local-bind-to-key "next-line" "N")
                (local-bind-to-key "next-line" "\^N")
                (local-bind-to-key "next-line" 13)
                (local-bind-to-key "next-line" 10)
                (local-bind-to-key "next-line" " ")
                (local-bind-to-key "argument-prefix" "\^U")
                (local-bind-to-key "previous-window" "\^Xp")
                (local-bind-to-key "previous-window" "\^XP")
                (local-bind-to-key "next-window" "\^Xn")
                (local-bind-to-key "next-window" "\^XN")
                (local-bind-to-key "delete-window" "\^Xd")
                (local-bind-to-key "delete-window" "\^XD")
                (local-bind-to-key "delete-other-windows" "\^X1")
                (local-bind-to-key "~dired-examine" "\^X\^V")
                (local-bind-to-key "next-page" "\^V")
                (local-bind-to-key "previous-page" "\ev")
                (local-bind-to-key "previous-page" "\eV")
                (local-bind-to-key "beginning-of-file" "\e<")
                (local-bind-to-key "end-of-file" "\e>")
                (local-bind-to-key "~dired-UnMark-file-deleted" "u")
                (local-bind-to-key "~dired-UnMark-file-deleted" "U")
                (local-bind-to-key "exit-emacs" "\^C")
                (local-bind-to-key "exit-emacs" "q")
                (local-bind-to-key "exit-emacs" "Q")
                (local-bind-to-key "~dired-examine" "e")
                (local-bind-to-key "~dired-examine" "E")
                (local-bind-to-key "~dired-examine" "v")
                (local-bind-to-key "~dired-examine" "V")
                (local-bind-to-key "~dired-remove" "r")
                (local-bind-to-key "~dired-remove" "R")
            )
        )
    )
)
