(declare-global bookmark-names)
(declare-global bookmark-last-bookmark-name)
(setq bookmark-names "")
(setq bookmark-last-bookmark-name "")

(defun
    set-bookmark(~public-name (get-tty-string "Set Bookmark [name] " bookmark-last-bookmark-name))

    ~name
    (setq bookmark-last-bookmark-name ~public-name)
    (setq ~name (concat "bookmark_" bookmark-last-bookmark-name))
    (error-occurred
        (execute-mlisp-line
            (concat "(declare-global " ~name ")")
        )
    )
    (execute-mlisp-line (concat "(setq " ~name " (dot))"))
    (message "Set bookmark " bookmark-last-bookmark-name)
    (novalue)
)

(defun
    goto-bookmark( ~public-name (get-tty-string "Goto Bookmark [name] " bookmark-last-bookmark-name))

    ~name ~mark
    (setq ~name (concat "bookmark_" ~public-name))
    (if
        (error-occurred (execute-mlisp-line (concat "(setq ~mark " ~name ")")))
        (error-message "Cannot find bookmark " ~public-name)
    )
    (switch-to-buffer ~mark)
    (goto-character (+ ~mark))
    (message "At bookmark " ~public-name)
    (novalue)

)

(declare-global ~bookmark-UI-last-entry)        ; defaults to 0
(defun
    (UI-set-bookmark
        ~shortcut

        (if (< ~bookmark-UI-last-entry 9)
            (setq ~shortcut (concat "&" (char-to-string (+ '1' ~bookmark-UI-last-entry)) " "))
            (setq ~shortcut "  ")
        )
        (set-bookmark (concat "UI-bookmark-" ~bookmark-UI-last-entry))
        (UI-add-menu
            (sprintf "BG%3.3d" ~bookmark-UI-last-entry)
            300
            "Boo&kmark"
            (+ 100 ~bookmark-UI-last-entry)
            (concat ~shortcut "Goto " current-buffer-name " - " (current-line-number))
        )
        (setq ~bookmark-UI-last-entry (+ ~bookmark-UI-last-entry 1))
    )
)
(defun
    (UI-goto-bookmark
        ~string

        (setq ~string
            (concat
                (char-to-string (get-tty-character))
                (char-to-string (get-tty-character))
                (char-to-string (get-tty-character))
            )
        )
        (goto-bookmark (concat "UI-bookmark-" (+ ~string)))
    )
)

(if (= terminal-is-terminal 3)
    (progn
        (UI-add-menu "BS" 300 "Boo&kmark" 10 "&Set bookmark here...")
        (UI-add-menu "-" 300 "Boo&kmark" 20 "-")
        (bind-to-key "UI-set-bookmark" "\[menu]BS")
        (bind-to-key "UI-goto-bookmark" "\[menu]BG")
    )
)
