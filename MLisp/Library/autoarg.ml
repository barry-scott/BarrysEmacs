(progn
(declare-global auto-arg-mode)
(setq auto-arg-mode 0)
(defun
    (~auto-arg
        ~auto-current-string
        ~auto-current-character
        ~auto-current-binding
        (setq ~auto-current-string "")
        (push-back-character (last-key-struck))
        (while
            (&
                (>=
                    (setq ~auto-current-character (get-tty-character))
                    '0'
                )
                (<=
                    ~auto-current-character
                    '9'
                )
            )
            (setq ~auto-current-string
                (concat
                    ~auto-current-string
                    (char-to-string ~auto-current-character)
                )
            )
            (message ~auto-current-string)
        )
        (if (error-occurred
                (setq ~auto-current-binding (describe-key ~auto-current-character))
            )
            (setq ~auto-current-binding "[unbound]")
        )
        (push-back-character ~auto-current-character)
        (setq ~auto-current-binding (substr ~auto-current-binding -13 13))
        (if (= ~auto-current-binding "\"self-insert\"")
            (insert-string ~auto-current-string)
            (return-prefix-argument ~auto-current-string)
        )
    )
    (auto-arg-mode
        (setq auto-arg-mode (!= auto-arg-mode 0))
        (setq auto-arg-mode (! auto-arg-mode))
        (if auto-arg-mode
            (progn
                (bind-to-key "~auto-arg" "0")
                (bind-to-key "~auto-arg" "1")
                (bind-to-key "~auto-arg" "2")
                (bind-to-key "~auto-arg" "3")
                (bind-to-key "~auto-arg" "4")
                (bind-to-key "~auto-arg" "5")
                (bind-to-key "~auto-arg" "6")
                (bind-to-key "~auto-arg" "7")
                (bind-to-key "~auto-arg" "8")
                (bind-to-key "~auto-arg" "9")
                (message "Auto arg mode on.")
            )
            (progn
                (bind-to-key "digit" "0")
                (bind-to-key "digit" "1")
                (bind-to-key "digit" "2")
                (bind-to-key "digit" "3")
                (bind-to-key "digit" "4")
                (bind-to-key "digit" "5")
                (bind-to-key "digit" "6")
                (bind-to-key "digit" "7")
                (bind-to-key "digit" "8")
                (bind-to-key "digit" "9")
                (message "Auto arg mode off")
            )
        )
    )
)
)
