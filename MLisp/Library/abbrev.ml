; This file defines some functions to make defining and using the abbrev
; system in EMACS easier. The following functions are provided
;
; define-global-abbrev-in-buffer which takes the word left of dot, and uses
;                                it as the abbreviation. It then asks what
;                                the expansion should be.
; define-local-abbrev-in-buffer  which does the same as the above, but uses
;                                the local abbrev table.
; list-local-abbrevs             which displays a list of the abbrevs and
;                                their expansions which apply to the current
;                                buffer. You get into a recursive edit.
; edit-local-abbrevs             which gives you a list of the current
;                                buffer's abbrevs, and then lets you edit
;                                them in a recursive edit. When you exit
;                                from the recursive edit, the chanes you
;                                have made take effect.
; list-all-abbrevs               same as list-local-abbrevs, but displays all
;                                abbrev tables.
; edit-all-abbrevs               same as edit-local-abbrevs but displays all
;                                abbrev tables.
; define-global-abbrev-for-region
;                                This command takes the current region, and
;                                asks the user to specify an abbrev for it.
;                                The abbrev is defined in the global table.
; define-local-abbrev-for-region This command takes the current region, and
;                                asks the user to specify an abbrev for it.
;                                The abbrev is defined in the current local
;                                abbrev table.
;

(progn
  (defun
    (~A-help
        (save-window-excursion
            (switch-to-buffer "Help")
            (erase-buffer)
            (setq mode-line-format "")
            (insert-string
                (concat
                    "ABBREV LIST: list of abbreviation definitions\n\n"
                    "    <sp>     next page\n"
                    "    <bs>     previous page\n"
                    "    n        next line\n"
                    "    p        previous line\n"
                    "    e        move to end of list\n"
                    "    b        move to beginning of list\n"
                    "    q        quit (you will be prompted to confirm)\n"
                    "    x        execute and EMACS command\n\n"
                    "non-printing characters perform their usual functions.\n\n\n"
                )
            )
            (message "-- Type any character to view the abbreviation list --")
            (get-tty-character)
            (setq mode-line-format default-mode-line-format)
        )
    )
    (~A-illegal (illegal-operation))
    (define-global-abbrev-in-buffer
        (save-excursion
            (backward-word)
            (set-mark)
            (forward-word)
            (define-global-abbrev
                (region-to-string)
                (arg 1 (concat ": define-global-abbrev-in-buffer " (region-to-string) " "))
            )
        )
    )
    (define-local-abbrev-in-buffer
        (save-excursion
            (backward-word)
            (set-mark)
            (forward-word)
            (define-local-abbrev
                (region-to-string)
                (arg 1 (concat ": define-local-abbrev-in-buffer " (region-to-string) " "))
            )
        )
    )
    (~A-list-buffer-abbrevs
        ~A-remove ~A-buffer-abbrev
        (setq ~A-remove -1)
        (setq ~A-buffer-abbrev current-buffer-abbrev-table)
        (pop-to-buffer "Abbrev list")
        (delete-other-windows)
        (erase-buffer)
        (insert-file "sys$scratch:temp.tmp")
        (unlink-file "sys$scratch:temp.tmp")
        (beginning-of-file)
        (while (! (error-occurred (re-search-forward "^[^ ]")))
            (beginning-of-line)
            (set-mark)
            (if (>= ~A-remove 0)
                (progn
                    (goto-character ~A-remove)
                    (erase-region)
                )
            )
            (end-of-line)
            (if (|
                    (= (region-to-string) "global")
                    (= (region-to-string) ~A-buffer-abbrev)
                )
                (setq ~A-remove -1)
                (setq ~A-remove (progn (beginning-of-line) (dot)))
            )
            (next-line)
            (beginning-of-line)
        )
        (end-of-file)
        (set-mark)
        (if (>= ~A-remove 0)
            (progn
                (goto-character ~A-remove)
                (erase-region)
            )
        )
        (beginning-of-file)
    )
    (list-local-abbrevs
        ~A-buffer
        (setq ~A-buffer current-buffer-name)
        (write-abbrev-file "sys$scratch:temp.tmp")
        (save-window-excursion
            (~A-list-buffer-abbrevs)
            (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
            (setq mode-string "More")
            (message "Use exit-emacs to continue editing.")
            (use-local-map "more-map")
            (recursive-edit)
        )
        (novalue)
    )
    (edit-local-abbrev
        ~A-buffer
        (setq ~A-buffer current-buffer-name)
        (write-abbrev-file "sys$scratch:temp.tmp")
        (save-window-excursion
            (~A-list-buffer-abbrevs)
            (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
            (setq mode-string "Edit")
            (message "Use exit-emacs to define the abbrevs.")
            (use-local-map "default-global-keymap")
            (recursive-edit)
            (write-named-file "sys$scratch:temp.tmp")
            (quietly-read-abbrev-file "sys$scratch:temp.tmp")
            (unlink-file "sys$scratch:temp.tmp")
        )
        (novalue)
    )
    (list-all-abbrevs
        ~A-buffer
        (setq ~A-buffer current-buffer-name)
        (write-abbrev-file "sys$scratch:temp.tmp")
        (save-window-excursion
            (pop-to-buffer "Abbrev list")
            (delete-other-windows)
            (erase-buffer)
            (insert-file "sys$scratch:temp.tmp")
            (unlink-file "sys$scratch:temp.tmp")
            (beginning-of-file)
            (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
            (setq mode-string "More")
            (message "Use exit-emacs to continue editing.")
            (use-local-map "more-map")
            (recursive-edit)
        )
        (novalue)
    )
    (edit-all-abbrevs
        ~A-buffer
        (setq ~A-buffer current-buffer-name)
        (write-abbrev-file "sys$scratch:temp.tmp")
        (save-window-excursion
            (pop-to-buffer "Abbrev list")
            (delete-other-windows)
            (erase-buffer)
            (insert-file "sys$scratch:temp.tmp")
            (unlink-file "sys$scratch:temp.tmp")
            (beginning-of-file)
            (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
            (setq mode-string "Edit")
            (message "Use exit-emacs to define the abbrevs.")
            (use-local-map "default-global-keymap")
            (recursive-edit)
            (write-named-file "sys$scratch:temp.tmp")
            (quietly-read-abbrev-file "sys$scratch:temp.tmp")
            (unlink-file "sys$scratch:temp.tmp")
        )
        (novalue)
    )
    (~A-check-abbrev
        ~A-fail
        ~A-abbrev
        (save-window-excursion
            (switch-to-buffer "~A")
            (setq ~A-abbrev (arg 1))
            (insert-string ~A-abbrev)
            (beginning-of-file)
            (setq ~A-fail (error-occurred (re-search-forward "[\n       ]")))
        )
        (delete-buffer "~A")
        (if (! ~A-fail)
            (error-message "abbrev \"" ~A-abbrev "\" is illegal.")
        )
    )
    (~A-check-region
        (save-window-excursion
            (save-restriction
                (narrow-region)
                (beginning-of-file)
                (if (! (error-occurred (search-forward "\n")))
                    (error-message "Newlines are not allowed in abbrevs.")
                )
            )
        )
    )
    (define-global-abbrev-for-region
        ~A-arg
        (~A-check-region)
        (setq ~A-arg
            (arg 1 ": define-global-abbrev-for-region ")
        )
        (~A-check-region)
        (~A-check-abbrev ~A-arg)
        (define-global-abbrev ~A-arg (region-to-string))
        (novalue)
    )
    (define-local-abbrev-for-region
        ~A-arg
        (~A-check-region)
        (setq ~A-arg
            (arg 1 ": define-global-abbrev-for-region phrase ")
        )
        (~A-check-region)
        (~A-check-abbrev ~A-arg)
        (define-local-abbrev ~A-arg (region-to-string))
        (novalue)
    )
  )
    (save-excursion
        i
        (temp-use-buffer "keymap hack")
        (define-keymap "more-map")
        (use-local-map "more-map")
        (setq i ' ')
        (while (<= i '~')
            (local-bind-to-key "~A-illegal" (char-to-string i))
            (setq i (+ i 1))
        )
        (local-bind-to-key "next-page" " ")
        (local-bind-to-key "previous-page" "\^h")
        (local-bind-to-key "next-line" "n")
        (local-bind-to-key "previous-line" "p")
        (local-bind-to-key "execute-extended-command" "x")
        (local-bind-to-key "exit-emacs" "\e")
        (local-bind-to-key "exit-emacs" "q")
        (local-bind-to-key "~A-help" "?")
        (local-bind-to-key "beginning-of-file" "b")
        (local-bind-to-key "end-of-file" "e")
        (delete-buffer "keymap hack")
        (novalue)
    )
    (error-occurred (execute-mlisp-file "abbrev.key"))
)
