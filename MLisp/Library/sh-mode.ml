;
; sh mode
;
(defun
    (sh-mode
        (setq mode-string "sh")
        (use-syntax-table "sh")
        (use-local-map "sh-map")
        (use-abbrev-table "sh")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.sh,*.bash,*.inc,.bashrc,.bash_profile"))
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
    (~sh-mode-setup-syntax-table
        (modify-syntax-table "paren" "(" ")")
        (modify-syntax-table "paren" "{" "}")
        (modify-syntax-table "paren" "[" "]")
        (modify-syntax-table "comment" "#" "\n")
        (modify-syntax-table "string-1" "\"")
        (modify-syntax-table "string-1" "'")
        (modify-syntax-table "string-2,paired" "${" "}")
        (modify-syntax-table "string-2" "`")
        (modify-syntax-table "prefix" "\\")
        (modify-syntax-table "word" "-~_.")

        (~mode-modify-syntax-table "keyword-1"
            "."
            "alias"
            "autoload"
            "bg"
            "break"
            "case"
            "cd"
            "complete"
            "continue"
            "do"
            "done"
            "echo"
            "elif"
            "else"
            "esac"
            "eval"
            "exec"
            "exit"
            "export"
            "false"
            "fc"
            "fg"
            "fi"
            "for"
            "function"
            "getopts"
            "if"
            "in"
            "integer"
            "jobs"
            "kill"
            "let"
            "popd"
            "print"
            "pushd"
            "pwd"
            "read"
            "readonly"
            "return"
            "select"
            "set"
            "shift"
            "shopt"
            "source"
            "test"
            "then"
            "time"
            "times"
            "trap"
            "true"
            "type"
            "typeset"
            "ulimit"
            "umask"
            "unalias"
            "unset"
            "until"
            "wait"
            "whence"
            "while"
        )
        ; unix commands
        (~mode-modify-syntax-table "keyword-2"
            "awk"
            "bash"
            "cat"
            "chmod"
            "chown"
            "cp"
            "curl"
            "dd"
            "diff"
            "dnf"
            "expr"
            "find"
            "grep"
            "gzip"
            "head"
            "hostname"
            "ifconfig"
            "ip"
            "less"
            "ln"
            "ls"
            "mkdir"
            "more"
            "mount"
            "mv"
            "netstat"
            "ps"
            "pwd"
            "rm"
            "scp"
            "sed"
            "sh"
            "ss"
            "ssh"
            "ssh-add"
            "sleep"
            "stty"
            "sudo"
            "tail"
            "tar"
            "touch"
            "tty"
            "umount"
            "uname"
            "unzip"
            "which"
            "yum"
            "zip"
            "zcat"
            "zdiff"
            "zmore"
            "zgrep"
        )
    )
)

(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "sh-map")
    (define-keymap "sh-ESC-map")
    (use-local-map "sh-map")
    (local-bind-to-key "sh-ESC-map" "\e")
    (error-occurred (execute-mlisp-file "sh-mode.key"))

    (use-syntax-table "sh")

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
    (~sh-mode-setup-syntax-table)
    (delete-buffer "keymap-hack")
)
