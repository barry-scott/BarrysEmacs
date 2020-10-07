(progn
    (external-function "emacs$spell" "emacs$share:emacs_spell_shr")
    (if (! (is-bound ~et-initialized))
        (progn
            (declare-global
                ~et-initialized
            )
            (setq ~et-initialized 0)
        )
    )
    (if (! (is-bound et-dictionary))
        (progn
            (declare-global
                et-dictionary
            )
            (setq et-dictionary "lngsplaet")
        )
    )
    (defun et-word-in-buffer ()
        ~spell-word
        ~spell-saved-syntax-table
        (save-window-excursion
            (setq ~spell-saved-syntax-table current-buffer-syntax-table)
            (use-syntax-table "spell")
            (error-occurred (forward-character))
            (backward-word)
            (set-mark)
            (forward-word)
            (setq ~spell-word (region-to-string))
            (use-syntax-table ~spell-saved-syntax-table)
        )
        (et-word ~spell-word)
    )
    (defun et-word (~et-spell-word (get-tty-string ": et-word "))
        ~spell-length
        (if (! ~et-initialized)
            (progn
                (emacs$spell 0 et-dictionary)
                (setq ~et-initialized 1)
            )
        )
        (save-window-excursion
            (pop-to-buffer "Electronic Thesaurus")
            (setq mode-line-format " Electronic Thesaurus ")
            (setq read-only-buffer 0)
            (erase-buffer)
            (emacs$spell 1 ~et-spell-word)
            (beginning-of-file)
            (setq ~spell-length (buffer-size))
            (setq buffer-is-modified 0)
            (setq current-buffer-checkpointable 0)
            (setq current-buffer-journalled 0)
            (setq read-only-buffer 1)
        )
        (if ~spell-length
            (save-excursion (pop-to-buffer "Electronic Thesaurus"))
            (error-message "\"" ~et-spell-word "\" is not in the Electronic Thesaurus")
        )
        (novalue)
    )
    (error-occurred (execute-mlisp-file "thesaurus.key"))
)
