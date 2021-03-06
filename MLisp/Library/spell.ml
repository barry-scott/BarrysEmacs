;
;       spell.ml
;
(progn
(declare-global
    spell-locale
    ~spell-init-done
    ~spell-word-pattern
    ~spell-word-to-check
    ~spell-word-start-mark
    ~spell-word-end-mark
    ~spell-range-start
    ~spell-range-end
    ~spell-suggestions
    ~spell-chosen-suggestion
    ~spell-checked
    ~spell-corrected
    ~spell-start-position
    ~spell-old-local-keymap
    ~spell-debug)

(define-keymap "no-local-map"); Hack because we can't undo local map
(setq spell-locale "en_US")
(setq ~spell-init-done 0)
(setq ~spell-suggestions 0)
(setq ~spell-debug 0)
(setq ~spell-checked 0)
(setq ~spell-corrected 0)
(setq ~spell-word-to-check "")

(defun ~spell-debug( ~msg)
    (if ~spell-debug
        (save-window-excursion
            (temp-use-buffer "spell-debug")
            (end-of-file)
            (insert-string ~msg "\n")
        )
    )
)

(defun
    (~spell-init
        (if (! ~spell-init-done)
            (progn
                (spell-check-init spell-locale)
                (setq ~spell-init-done 1)
            )
        )
    )
)

(defun
    (setup-spell-checker ~key ~arg-num ~arg
        (save-excursion
            (temp-use-buffer "~spell-key-hack")
            (define-keymap "~spell-map")

            (define-keymap "~spell-map-key-prefix")
            (define-keymap "~spell-map-mouse-prefix")
            (define-keymap "~spell-map-menu-prefix")

            (use-local-map "~spell-map-key-prefix")
            (local-bind-to-key "~spell-help" "\[default]")

            (use-local-map "~spell-map-mouse-prefix")
            (local-bind-to-key "~spell-help" "\[default]")

            (use-local-map "~spell-map-menu-prefix")
            (local-bind-to-key "~spell-help" "\[default]")

            (use-local-map "~spell-map")
            (local-bind-to-key "interrupt-key" "\^g")
            (local-bind-to-key "~spell-help" "\[default]")
            (local-bind-to-key "~spell-map-key-prefix" "\[key-prefix]")
            (local-bind-to-key "~spell-map-mouse-prefix" "\[mouse-prefix]")
            (local-bind-to-key "~spell-map-menu-prefix" "\[menu-prefix]")

            ;
            ; setup key binds from parameters.
            ; arg n is the name of the functions required and arg n+1 is
            ; a string describing the key to be bound.
            ; e.g. "abort" "\^g"
            ;
            (setq ~arg-num 0)
            (while (< (setq ~arg-num (+ 1 ~arg-num)) (nargs))
                (progn
                    (setq ~arg (arg ~arg-num))
                    (setq ~key (arg (setq ~arg-num (+ 1 ~arg-num))))
                    (~spell-setup-bind-key)
                )
            )
            (delete-buffer "~spell-key-hack")
        )
    )
)

(defun
    (~spell-setup-bind-key
        (if
            (= ~arg "suggestion-1") (local-bind-to-key "~spell-suggestion-1" ~key)
            (= ~arg "suggestion-2") (local-bind-to-key "~spell-suggestion-2" ~key)
            (= ~arg "suggestion-3") (local-bind-to-key "~spell-suggestion-3" ~key)
            (= ~arg "suggestion-4") (local-bind-to-key "~spell-suggestion-4" ~key)
            (= ~arg "suggestion-5") (local-bind-to-key "~spell-suggestion-5" ~key)
            (= ~arg "suggestion-6") (local-bind-to-key "~spell-suggestion-6" ~key)
            (= ~arg "suggestion-7") (local-bind-to-key "~spell-suggestion-7" ~key)
            (= ~arg "suggestion-8") (local-bind-to-key "~spell-suggestion-8" ~key)
            (= ~arg "suggestion-9") (local-bind-to-key "~spell-suggestion-9" ~key)
            (= ~arg "quit") (local-bind-to-key "~spell-quit" ~key)
            (= ~arg "undo") (local-bind-to-key "~spell-undo" ~key)
            (= ~arg "apply-to-all") (local-bind-to-key "~spell-apply-to-all" ~key)
            (= ~arg "next") (local-bind-to-key "~spell-next" ~key)
            (= ~arg "edit") (local-bind-to-key "~spell-edit" ~key)
            (= ~arg "help") (local-bind-to-key "~spell-help" ~key)
            (if (error-occurred (local-bind-to-key ~arg ~key))
                (error-message "Unknown parameter to setup-spell " ~arg)
            )
        )
    )
)

(defun
    (spell-check-buffer
        (setq ~spell-word-pattern "\\w+")
        (~spell-buffer)
    )
)

(defun
    (spell-check-region
        (setq ~spell-word-pattern "\\w+")
        (~spell-region)
    )
)

(defun
    (spell-check-comments-in-buffer
        (setq ~spell-word-pattern "(?Sc*=)\\w+")
        (~spell-buffer)
    )
)

(defun
    (spell-check-comments-in-region
        (setq ~spell-word-pattern "(?Sc*=)\\w+")
        (~spell-region)
    )
)

(defun
    (spell-check-strings-in-buffer
        (setq ~spell-word-pattern "(?Ss*=)\\w+")
        (~spell-buffer)
    )
)

(defun
    (spell-check-strings-in-region
        (setq ~spell-word-pattern "(?Ss*=)\\w+")
        (~spell-region)
    )
)

(defun
    (spell-check-word-in-buffer
        (setq ~spell-word-pattern "\\w+")
        (save-window-excursion
            (error-occurred (forward-character))
            (backward-word)
            (setq ~spell-range-start (dot))
            (forward-word)
            (setq ~spell-range-end (dot))
        )
        (~spell-range)
    )
)

(defun
    (~spell-buffer
        (save-window-excursion
            (beginning-of-file)
            (setq ~spell-range-start (dot))
            (end-of-file)
            (setq ~spell-range-end (dot))
        )
        (~spell-range)
    )
)

(defun
    (~spell-region
        (if (< (dot) (mark))
            (progn
                (setq ~spell-range-start (dot))
                (setq ~spell-range-end (mark))
            )
            (progn
                (setq ~spell-range-start (mark))
                (setq ~spell-range-end (dot))
            )
        )
        (~spell-range)
    )
)

(defun
    (~spell-range
        (~spell-init)

        (setq ~spell-checked 0)
        (setq ~spell-corrected 0)
        (setq ~spell-word-to-check "")
        (setq ~spell-chosen-suggestion "")
        (setq ~spell-old-local-keymap current-local-keymap)

        (use-local-map "~spell-map")
        (setq ~spell-start-position (dot))
        (goto-character ~spell-range-start)
        (if (~spell-find-word-to-correct)
            (progn
                (~spell-show-suggestions)
                (recursive-edit)
            )
            (progn
                (~spell-restore-state)
                (novalue)
            )
        )
    )
)

(defun
    ~spell-choose-suggestion(~suggestion-index)
    (erase-region)
    ; set region around the replacement
    (set-mark)
    (setq ~spell-chosen-suggestion
        (fetch-array ~spell-suggestions ~suggestion-index))
    (insert-string ~spell-chosen-suggestion)
    (~spell-show-suggestions)
)

(defun
    (~spell-show-suggestions
        ~msg ~index
        (setq ~msg "Suggestions")
        (setq ~index 0)
        (while (< ~index (fetch-array ~spell-suggestions 0))
            (setq ~index (+ 1 ~index))
            (setq  ~msg (concat ~msg " " ~index ":" (fetch-array ~spell-suggestions ~index)))
        )
        (message ~msg)
        (novalue)
    )
)

(defun
    (~spell-restore-local-keymap
        ; restore the out local keymap
        (use-local-map
            (if (= ~spell-old-local-keymap "")
                "no-local-map" ~spell-old-local-keymap
            )
        )
        ; check for perverse errors that leave spell maps as the local map
        (if
            (= current-local-keymap "~spell-map")
            (use-local-map "no-local-map")
        )
    )
)

;
;       Functions that are bound to spell keystrokes
;
(defun
    (~spell-suggestion-1
        (~spell-choose-suggestion 1)
    )
)

(defun
    (~spell-suggestion-2
        (~spell-choose-suggestion 2)
    )
)

(defun
    (~spell-suggestion-3
        (~spell-choose-suggestion 3)
    )
)

(defun
    (~spell-suggestion-4
        (~spell-choose-suggestion 4)
    )
)

(defun
    (~spell-suggestion-5
        (~spell-choose-suggestion 5)
    )
)

(defun
    (~spell-suggestion-6
        (~spell-choose-suggestion 6)
    )
)

(defun
    (~spell-suggestion-7
        (~spell-choose-suggestion 7)
    )
)

(defun
    (~spell-suggestion-8
        (~spell-choose-suggestion 8)
    )
)

(defun
    (~spell-suggestion-9
        (~spell-choose-suggestion 9)
    )
)

(defun
    (~spell-undo
        (erase-region)
        ; set region around the replacement
        (set-mark)
        (setq ~spell-chosen-suggestion ~spell-word-to-check)
        (insert-string ~spell-chosen-suggestion)
        (~spell-show-suggestions)
    )
)

(defun
    (~spell-apply-to-all
        (get-tty-string "Not yet implemented.")
        (novalue)
        (~spell-show-suggestions)
    )
)

(defun
    (~spell-quit
        (~spell-restore-state)
        (exit-emacs)
    )
)

(defun
    (~spell-restore-state
        (~spell-update-counts)
        (~spell-restore-local-keymap)
        (message (concat "spell checked " ~spell-checked " and corrected " ~spell-corrected))
        (unset-mark)
        (goto-character ~spell-start-position)
    )
)

(defun
    (~spell-next
        (if (~spell-find-word-to-correct)
            ; must show the suggestions
            (~spell-show-suggestions)
            (~spell-quit)
        )
    )
)

(defun
    (~spell-find-word-to-correct
        ; return 1 if there is spell correction required
        (~spell-update-counts)
        ; while we can find words to check...
        (if
            (error-occurred
                (while
                    (progn
                        (if (>= (dot) ~spell-range-end)
                            (error-message "spell done")
                        )
                        (ere-search-forward ~spell-word-pattern)
                        (if (> (dot) ~spell-range-end)
                            (error-message "spell done")
                        )
                        (setq ~spell-checked (+ 1 ~spell-checked))
                        (region-around-match 0)
                        (setq ~spell-word-to-check (region-to-string))
                        (spell-check-word ~spell-word-to-check)
                    )
                    (novalue)
                )
            )
            0
            (progn
                (setq ~spell-word-end-mark (dot))
                (setq ~spell-word-start-mark (left-marker (mark)))
                (setq ~spell-suggestions (spell-check-suggestions ~spell-word-to-check))
                1
            )
        )
    )
)

(defun
    (~spell-update-counts
        (if
            (&
                ; not found a word to check yet
                (!= ~spell-word-to-check "")
                ; not chosen a replacement
                (!= ~spell-chosen-suggestion "")
                ; replacement is different
                (!= ~spell-chosen-suggestion ~spell-word-to-check)
            )
            (setq ~spell-corrected (+ 1 ~spell-corrected))
        )
        (setq ~spell-word-to-check "")
        (setq ~spell-chosen-suggestion "")
    )
)

(defun
    (~spell-edit
        (~spell-restore-local-keymap)
        (save-window-excursion
            (recursive-edit)
        )
        (use-local-map "~spell-map")
        (goto-character ~spell-word-start-mark)
        (set-mark)
        (goto-character  ~spell-word-end-mark)
        (~spell-show-suggestions)
    )
)

(defun
    (~spell-help
        (save-window-excursion
            (describe-bindings)
            (switch-to-buffer "Help")
            (delete-other-windows)
            (beginning-of-file)
            (set-mark)
            (re-search-forward "^Local Bindings.*")
            (erase-region)
            (insert-string
                "You are in spell-check and the special keys are:\n"
            )

            (beginning-of-file)
            (ere-search-forward "^Global Bindings.*:$")
            (beginning-of-line)
            (set-mark)
            (end-of-file)
            (erase-region)
            (unset-mark)

            (beginning-of-file)
            (error-occurred
                (ere-replace-string "^.*~spell-help\n" "")
            )

            (beginning-of-file)
            (error-occurred
                (ere-replace-string "^.*<unbound>\n" "")
            )

            (beginning-of-file)
            (error-occurred
                (ere-replace-string "^.*~spell-map-.*\n" "")
            )

            (beginning-of-file)
            (~spell-help-replace-string
                "~spell-quit" "Quit spell-checker"
            )
            (~spell-help-replace-string
                "~spell-undo" "Undo spelling change"
            )
            (~spell-help-replace-string
                "~spell-next" "Find next spelling error"
            )
            (~spell-help-replace-string
                "~spell-suggestion-1" "Choose suggestion 1"
            )
            (~spell-help-replace-string
                "~spell-suggestion-2" "Choose suggestion 2"
            )
            (~spell-help-replace-string
                "~spell-suggestion-3" "Choose suggestion 3"
            )
            (~spell-help-replace-string
                "~spell-suggestion-4" "Choose suggestion 4"
            )
            (~spell-help-replace-string
                "~spell-suggestion-5" "Choose suggestion 5"
            )
            (~spell-help-replace-string
                "~spell-suggestion-6" "Choose suggestion 6"
            )
            (~spell-help-replace-string
                "~spell-suggestion-7" "Choose suggestion 7"
            )
            (~spell-help-replace-string
                "~spell-suggestion-8" "Choose suggestion 8"
            )
            (~spell-help-replace-string
                "~spell-suggestion-9" "Choose suggestion 9"
            )
            (~spell-help-replace-string
                "~spell-edit" "recursive-edit and continue upon exit"
            )

            (beginning-of-file)
            (message "Type any character to continue spell check")
            (get-tty-character)
            (~spell-show-suggestions)
        ); end save-window-excursion
    )
)
(defun
    ~spell-help-replace-string( ~old ~new)
    (beginning-of-file)
    (error-occurred
        (replace-string ~old ~new)
    )
)

;
; load up the key bindings and initialise the package
;
(execute-mlisp-file "spell.key")
(novalue)
)
