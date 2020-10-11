;
; lua mode
;
(defun
    (lua-mode
        (setq mode-string "lua")
        (use-syntax-table "lua")
        (use-local-map "lua-map")
        (use-abbrev-table "lua")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.lua"))
        (novalue)
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
    (~lua-mode-setup-syntax-table
        (modify-syntax-table "paren" "(" ")")
        (modify-syntax-table "paren" "{" "}")
        (modify-syntax-table "paren" "[" "]")
        (modify-syntax-table "comment" "--" "\n")
        (modify-syntax-table "comment" "--[[" "--]]")
        (modify-syntax-table "string-1" "\"")
        (modify-syntax-table "string-1" "'")
        (modify-syntax-table "prefix" "\\")
        (modify-syntax-table "word" "_")

        (~mode-modify-syntax-table "keyword-1"
            "and"
            "break"
            "do"
            "else"
            "elseif"
            "end"
            "false"
            "for"
            "function"
            "if"
            "in"
            "local"
            "nil"
            "not"
            "or"
            "repeat"
            "return"
            "then"
            "true"
            "until"
            "while"
        )
        (~mode-modify-syntax-table "keyword-2"
            "assert"
            "collectgarbage"
            "dofile"
            "error"
            "_G"
            "getfenv"
            "getmetatable"
            "ipairs"
            "load"
            "loadfile"
            "loadstring"
            "next"
            "pairs"
            "pcall"
            "print"
            "rawequal"
            "rawget"
            "rawset"
            "select"
            "setfenv"
            "setmetatable"
            "tonumber"
            "tostring"
            "type"
            "unpack"
            "_VERSION"
            "xpcall"
            ; libraries
            "require"
            "package"
            "string"
            "coroutine"
            "table"
            "math"
            "io"
            "os"
            "debug"
        )
    )
)

(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "lua-map")
    (define-keymap "lua-ESC-map")
    (use-local-map "lua-map")
    (local-bind-to-key "lua-ESC-map" "\e")
    (error-occurred (execute-mlisp-file "lua-mode.key"))

    (use-syntax-table "lua")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )
    (if (is-bound check-for-use-of-tabs-problems)
        (if check-for-use-of-tabs-problems
            (add-check-for-use-of-tabs-problems-syntax-table)
        )
    )
    (~lua-mode-setup-syntax-table)
    (delete-buffer "keymap-hack")
    (novalue)
)
