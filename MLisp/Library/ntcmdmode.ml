;
; WinNT CMD mode
;
(defun
    (ntcmd-back-paren                   ; Go to last open parenthesis at current level
        (backward-paren 0))
)

(defun
    (ntcmd-fore-paren                   ; Go to next close parenthesis at current level
        (forward-paren 0))
)

(defun
    (ntcmd-flash-back-paren             ; Flash the matching left parenthesis
        (save-excursion
            (backward-paren 0)
            (if (dot-is-visible)
                (sit-for 5)
                (progn
                    (beginning-of-line)
                    (set-mark)
                    (end-of-line)
                    (message (region-to-string)))
            )
        )
    )
)

(defun
    (ntcmd-flash-fore-paren             ; Flash the matching close parenthesis
        (save-excursion
            (forward-paren 0)
            (if (dot-is-visible)
                (sit-for 5)
                (progn
                    (beginning-of-line)
                    (set-mark)
                    (end-of-line)
                    (message (region-to-string))
                )
            )
        )
    )
)

(defun
    (ntcmd-paren                        ; Flashes matching open parenthesis when paren is typed.
        (insert-character (last-key-struck))
        (ntcmd-flash-back-paren)
    )
)

(defun
    (~mode-modify-syntax-table
        ~type
        ~arg
        (setq ~type (arg 1))
        (setq ~arg 2)
        (while (<= ~arg (nargs))
            (modify-syntax-table ~type (arg ~arg))
            (setq ~arg (+ ~arg 1))
        )
    )
)

(defun
    (ntcmd-mode
        (setq mode-string "CMD")
        (use-syntax-table "ntcmd")
        (use-local-map "ntcmd-map")
        (use-abbrev-table "ntcmd")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.cmd,*.bat"))
        (novalue)
    )
)

;
; NT-CMD keywords etc.
;
(save-excursion
    (temp-use-buffer "~ntcmd-hack~")
    (use-syntax-table "ntcmd")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )

    (modify-syntax-table "paren" "(" ")")

    (modify-syntax-table "string" "\"")

    (modify-syntax-table "word" "-_")       ;  Nb -_ is order-sensitive

    (modify-syntax-table "comment,case-fold,ere" "rem\b" "\n")

    ; Keyword-1 is used for command-shell builtins.
    ;
    (~mode-modify-syntax-table "keyword-1,case-fold"
        "assoc"

        "break"

        "call"
        "cd"
        "chcp"
        "chdir"
        "cls"
        "cmdcmdline"
        "cmdextversion"
        "color"
        "copy"

        "date"
        "defined"
        "del"
        "dir"
        "diskcomp"
        "diskcopy"
        "do"

        "equ"
        "neq"
        "lss"
        "leq"
        "gtr"
        "geq"

        "echo"
        "endlocal"
        "erase"
        "errorlevel"
        "exist"
        "exit"

        "for"
        "format"
        "ftype"

        "goto"
        "graftabl"

        "if"
        "in"

        "keyb"

        "md"
        "mkdir"
        "mode"
        "more"
        "move"

        "not"

        "off"
        "on"

        "path"
        "pause"
        "popd"
        "prompt"
        "pushd"

        "rd"
        "ren"
        "rename"
        "rmdir"

        "set"
        "setlocal"
        "shift"
        "start"

        "time"
        "title"
        "tree"
        "type"

        "ver"
        "verify"
        "vol"
    )

    ; Keyword-2 is used for utilities external to the command shell itself.
    ;
    (~mode-modify-syntax-table "keyword-2,case-fold"
        "at"
        "attrib"

        "cacls"
        "chkdsk"
        "cmd"
        "comp"
        "compact"
        "convert"

        "doskey"

        "fc"
        "find"
        "findstr"

        "help"

        "label"

        "move"

        "print"

        "recover"
        "replace"
        "restore"

        "sort"
        "subst"

        "xcopy"
    )

    (define-keymap "ntcmd-map")
    (define-keymap "ntcmd-ESC-map")
    (use-local-map "ntcmd-map")
    (local-bind-to-key "ntcmd-ESC-map" "\e")
    (execute-mlisp-file "ntcmdmode.key")
    ;(high-voltage-on-key-binding)
    (kill-buffer "~ntcmd-hack~")
)
(novalue)
