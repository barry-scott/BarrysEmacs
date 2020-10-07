(progn
    (declare-global
        info-root-file
        info-default-directory
        ~info-help-library
        ~info-is-init
        ~info-current-file
        ~info-current-node
        ~info-file-part
        ~info-node-part
        ~info-help-part
        ~info-file-def
        ~info-help-def
        ~info-^n-file
        ~info-^n-node
        ~info-^n-help-flag
    )
    (setq info-root-file "emacsroot")
    (setq info-default-directory "emacs_info:")
    (setq ~info-current-file "")
    (setq ~info-current-node "")
    (defun
        (info
            i
            j
            (if (= current-buffer-name "INFO")
                (error-message "You are already using INFO.")
            )
            (save-window-excursion
                (if (! ~info-is-init)
                    (progn
                        (switch-to-buffer "Hack buffer")
                        (widen-region)
                        (erase-buffer)
                        (setq case-fold-search 1)
                        (setq current-buffer-checkpointable 0)
                        (setq current-buffer-journalled 0)
                        (setq ~info-file-def (expand-file-name info-default-directory))
                        (insert-string ~info-file-def)
                        (search-reverse ".")
                        (set-mark)
                        (end-of-file)
                        (erase-region)
                        (beginning-of-file)
                        (setq ~info-file-def (region-to-string))
                        (setq ~info-help-def (expand-file-name "SYS$HELP"))
                        (insert-string ~info-help-def)
                        (search-reverse ".")
                        (set-mark)
                        (end-of-file)
                        (erase-region)
                        (beginning-of-file)
                        (setq ~info-help-def (region-to-string))
                        (switch-to-buffer "--INFO stack--")
                        (setq highlight-region 0)
                        (setq mode-line-format "%m")
                        (setq mode-string "--Internal INFO buffer--")
                        (use-syntax-table "--INFO stack--")
                        (modify-syntax-table "word" "!-~")
                        (widen-region)
                        (erase-buffer)
                        (setq case-fold-search 1)
                        (setq current-buffer-checkpointable 0)
                        (setq current-buffer-journalled 0)
                        (switch-to-buffer "--INFO--")
                        (setq highlight-region 0)
                        (setq mode-line-format "%m")
                        (setq mode-string "--Internal INFO buffer--")
                        (use-syntax-table "--INFO stack--")
                        (widen-region)
                        (erase-buffer)
                        (setq case-fold-search 1)
                        (setq current-buffer-checkpointable 0)
                        (setq current-buffer-journalled 0)
                        (switch-to-buffer "INFO")
                        (setq highlight-region 0)
                        (delete-other-windows)
                        (setq mode-line-format "")
                        (use-syntax-table "INFO")
                        (modify-syntax-table "word" "!-~")
                        (modify-syntax-table "dull" ",")
                        (setq current-buffer-checkpointable 0)
                        (setq current-buffer-journalled 0)
                        (setq case-fold-search 1)
                        (define-keymap "INFO-^R")
                        (use-local-map "INFO-^R")
                        (autoload "~info-save" "info_save")
                        (local-bind-to-key "~info-save" "\^x\^f")
                        (local-bind-to-key "exit-emacs" "\^c")
                        (local-bind-to-key "exit-emacs" "\^]")
                        (define-keymap "INFO")
                        (use-local-map "INFO")
                        (setq i 0)
                        (while (< i 256)
                            (local-bind-to-key "illegal-operation" (char-to-string i))
                            (setq i (+ 1 i))
                        )
                        (local-bind-to-key "execute-extended-command" "x")
                        (local-bind-to-key "execute-extended-command" "\ex")
                        (local-bind-to-key "search-forward" "s")
                        (local-bind-to-key "search-reverse" "r")
                        (local-bind-to-key "search-forward" "\^s")
                        (local-bind-to-key "search-reverse" "\^r")
                        (local-bind-to-key "redraw-display" "\^l")
                        (local-bind-to-key "next-page" " ")
                        (local-bind-to-key "previous-page" "\^h")
                        (local-bind-to-key "scroll-one-line-down" "a")
                        (local-bind-to-key "scroll-one-line-up" "z")
                        (local-bind-to-key "beginning-of-file" "b")
                        (local-bind-to-key "end-of-file" "e")
                        (autoload "~info-o" "info_o")
                        (local-bind-to-key "~info-o" "o")
                        (autoload "~info-h" "info_h")
                        (local-bind-to-key "~info-g" "g")
                        (autoload "~info-g" "info_g")
                        (local-bind-to-key "~info-h" "h")
                        (autoload "~info-hlp" "info_hlp")
                        (local-bind-to-key "~info-hlp" "?")
                        (autoload "~info-d" "info_d")
                        (local-bind-to-key "~info-d" "d")
                        (local-bind-to-key "exit-emacs" "q")
                        (autoload "~info-m" "info_m")
                        (local-bind-to-key "~info-m" "m")
                        (autoload "~info-p" "info_p")
                        (local-bind-to-key "~info-p" "p")
                        (autoload "~info-n" "info_n")
                        (local-bind-to-key "~info-n" "n")
                        (autoload "~info-u" "info_u")
                        (local-bind-to-key "~info-u" "u")
                        (autoload "~info-l" "info_l")
                        (local-bind-to-key "~info-l" "l")
                        (autoload "~info-@" "info_at")
                        (local-bind-to-key "~info-@" "@")
                        (autoload "~info-^r" "info_ctrl_r")
                        (local-bind-to-key "~info-^r" "\^e")
                        (autoload "~info-^n" "info_ctrl_n")
                        (local-bind-to-key "~info-^n" "\^n")
                        (setq ~info-file-part
                            (concat info-default-directory
                                info-root-file)
                        )
                        (setq ~info-node-part "TOP")
                        (setq mode-line-format
                            "INFO documentation reader   Node: (%f)%m   --%p--")
                        (if (> (nargs) 0)
                            (~info-g (arg 1))
                            (find-node ~info-file-part ~info-node-part 0)
                        )
                        (setq ~info-is-init 1)
                    )
                    (progn
                        (extend-database-search-list "info" ~info-current-file)
                        (switch-to-buffer "INFO")
                        (delete-other-windows)
                    )
                )
                (recursive-edit)
            )
            (if (is-bound ~INFO-com)
                (if ~INFO-com
                    (exit-emacs)
                    (setq ~INFO-com 0)
                )
            )
            (error-occurred (remove-database "info" ""))
        )
        (~info-trim
            (save-window-excursion
                (switch-to-buffer "Hack buffer")
                (erase-buffer)
                (insert-string (arg 1))
                (beginning-of-file)
                (if (looking-at (quote ~info-file-def))
                    (progn
                        (region-around-match 0)
                        (erase-region)
                    )
                    (looking-at (quote ~info-help-def))
                    (progn
                        (region-around-match 0)
                        (erase-region)
                    )
                    (novalue)
                )
                (if (error-occurred (search-forward ";"))
                    (end-of-line)
                    (backward-character)
                )
                (error-occurred (search-reverse ".tlb"))
                (set-mark)
                (beginning-of-file)
                (case-region-upper)
                (region-to-string)
            )
        )
        (info-check-menu
            (save-excursion
                (beginning-of-file)
                (if (error-occurred (re-search-forward "^\*[ \t]*menu:"))
                    0
                    (dot)
                )
            )
        )
        (info-goto-menu-item
            t
            dmr
            (setq t (arg 1))
            (if (! (setq dmr (info-check-menu)))
                (error-message "This node has no menu.")
            )
            (save-window-excursion
                (goto-character dmr)
                (if (error-occurred
                        (re-search-forward
                            (concat
                                "^\*[ \t]*"
                                t
                            )
                        )
                    )
                    (error-message "No such menu item: " t)
                    (setq dmr (dot))
                )
            )
            (goto-character dmr)
            (search-forward ":")
            (if (= (following-char) ':')
                (progn
                    (backward-character)
                    (set-mark)
                    (backward-word)
                    (setq ~info-node-part (region-to-string))
                    (setq ~info-help-part ~info-help-library)
                )
                (progn
                    (forward-word)
                    (backward-word)
                    (setq ~info-node-part "TOP")
                    (get-node-and-file)
                )
            )
            (find-node ~info-file-part ~info-node-part ~info-help-part)
        )
        (get-node-and-file
            (forward-word)
            (backward-word)
            (if (= (following-char) '(')
                (progn
                    add-directory
                    (forward-character)
                    (set-mark)
                    (search-forward ")")
                    (backward-character)
                    (exchange-dot-and-mark)
                    (narrow-region)
                    (if (! (looking-at ".*[]:>]"))
                        (setq add-directory 1)
                        (setq add-directory 0)
                    )
                    (widen-region)
                    (setq ~info-file-part (region-to-string))
                    (if (looking-at "dir)")
                        (setq ~info-file-part info-root-file)
                    )
                    (exchange-dot-and-mark)
                    (forward-character)
                    (if (= (substr (expand-file-name ~info-file-part) -2 2) ".;")
                        (setq ~info-file-part (concat ~info-file-part ".tlb"))
                    )
                    (save-window-excursion
                        (switch-to-buffer "Hack-buffer")
                        (erase-buffer)
                        (insert-string (expand-file-name ~info-file-part))
                        (beginning-of-file)
                        (setq ~info-help-part
                            (looking-at ".*\\.hlb;[0-9]*$")
                        )
                    )
                    (if add-directory
                        (setq ~info-file-part
                            (concat
                                (if ~info-help-part
                                    "SYS$HELP:"
                                    info-default-directory
                                )
                                ~info-file-part
                            )
                        )
                    )
                )
            )
            (if (looking-at "\\([-a-zA-Z0-9 -+./:-\\{-~_][-a-zA-Z0-9 -+./:-\\{-~_]*\\)[ \t,\n]")
                (progn
                    (region-around-match 1)
                    (setq ~info-node-part (region-to-string))
                )
            )
        )
        (find-node
            file
            node
            helpp
            new-file
            new-node
            dot
            (setq new-file 0)
            (setq dot (+ (dot 0)))
            (save-window-excursion
                (setq file (arg 1))
                (setq node (arg 2))
                (setq helpp (arg 3))
                (if (!= (length file) 0)
                    (progn
                        (setq file (expand-file-name file))
                        (if (!= file ~info-current-file)
                            (progn
                                (setq new-file 1)
                                (extend-database-search-list "info" file)
                                (if (!= (length ~info-current-file) 0)
                                    (error-occurred
                                        (remove-database "info" ~info-current-file)
                                    )
                                )
                            )
                        )
                    )
                )
                (if (| new-file (!= ~info-current-node node))
                    (save-window-excursion
                        (setq new-node 1)
                        (switch-to-buffer "--INFO--")
                        (widen-region)
                        (erase-buffer)
                        (if (error-occurred
                                (if helpp
                                    (progn
                                        saved-h
                                        (setq saved-h (~info-trim ~info-current-file))
                                        (insert-string
                                            (concat
                                                "Help Node: "
                                                node
                                            )
                                        )
                                        (to-col
                                            (- 76
                                                (+
                                                    (length saved-h)
                                                    (length ~info-current-node)
                                                    7
                                                )
                                            )
                                        )
                                        (insert-string
                                            (concat
                                                "Up: ("
                                                saved-h
                                                ")"
                                                ~info-current-node
                                                "\n"
                                            )
                                        )
                                        (setq saved-h fetch-help-database-flags)
                                        (setq fetch-help-database-flags
                                            (if (= (substr node -1 1) "*")
                                                (progn
                                                    (setq node (substr node 1 -1))
                                                    7
                                                )
                                                1
                                            )
                                        )
                                        (fetch-help-database-entry "info" node)
                                        (setq fetch-help-database-flags saved-h)
                                    )
                                    (fetch-database-entry "info" node)
                                )
                            )
                            (progn
                                (if new-file
                                    (progn
                                        (remove-database "info" file)
                                        (extend-database-search-list "info" ~info-current-file)
                                    )
                                )
                                (error-message node ": " error-message)
                            )
                        )
                    )
                    (setq new-node 0)
                )
                (if (&
                        (> (length ~info-current-node) 0)
                        (> (length ~info-current-file) 0)
                    )
                    (progn
                        (switch-to-buffer "--INFO stack--")
                        (end-of-file)
                        (insert-string
                            (concat
                                ~info-current-file
                                "\n"
                                ~info-current-node
                                "\n"
                                dot
                                "\n"
                                ~info-help-library
                                "\n"
                            )
                        )
                    )
                )
                (if (!= (length file) 0) (setq ~info-current-file file))
                (setq ~info-current-node node)
                (setq ~info-help-library helpp)
            )
            (if (| new-node new-file)
                (progn
                    (if new-file
                        (setq current-buffer-macro-name (~info-trim file))
                    )
                    (setq mode-string ~info-current-node)
                    (widen-region)
                    (erase-buffer)
                    (yank-buffer "--INFO--")
                )
            )
            (beginning-of-file)
        )
    )
)
