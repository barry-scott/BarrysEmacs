;
; rust-mode.ml
;
(declare-global
    cargo-maybe-active
    cargo-errors-scanned)
(setq cargo-maybe-active 0)
(setq cargo-errors-scanned 0)

(defun
    (is-windows
        (= operating-system-name "Windows")
    )
)

(defun
    (rust-mode
        (setq mode-string "rust")
        (use-syntax-table "rust")
        (use-local-map "rust-map")
        (use-abbrev-table "rust")
        (setq compile-it-command "cargo build")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.rs"))
        (novalue)
    )
)

;
; used by the rust-console module which share all but the keymap
;
(defun
    (rust-console-mode
        (setq mode-string "rust")
        (use-syntax-table "rust")
        (use-abbrev-table "rust")
        (novalue)
    )
)

(defun
    (rust-error-message-parser
        ~project-folder
        ~error-column

        (setq ~project-folder ".")
        (setq ~error-column 1)

        (save-excursion
            (beginning-of-file)
            (error-occurred
                (ere-looking-at "^.*(\\(.*\\))")
                (region-around-match 1)
                (setq ~project-folder (region-to-string))
            )
        )

        ; parsing stops on errors
        (if
            (error-occurred
                (ere-search-forward "^(error|warning).*\n  --> (.+):(\\d+):(\\d+)"))
            (progn
                (setq error-line-number 0)
            )
            (progn
                (save-window-excursion
                    (region-around-match 2)
                    (setq error-file-name (region-to-string))
                    (while (! (file-exists error-file-name))
                        (setq error-file-name (get-tty-file "Locate file: " error-file-name))
                    )
                    (region-around-match 3)
                    (setq error-line-number (region-to-string))
                    (region-around-match 4)
                    (setq ~error-column (region-to-string))
                )
                (beginning-of-line)
                (previous-line)
                (setq error-start-position (+ (dot) ~error-column -1))
                (next-line 2)
                (beginning-of-line)
            )
        )
    )
)

(defun
    (rust-compile
        ~arg
        ~cargo-command

        (setq ~arg (if (> prefix-argument 1)
                          (arg 1 ": rust-compile cargo [build]: ")
                         ""))
        (if (length ~arg)
            (setq ~cargo-command ~arg)
            (setq ~cargo-command "build"))

        (setq cargo-maybe-active 1)
        (setq cargo-errors-scanned 0)
        (save-excursion
            ; make sure that the Error log window appears
            ; the save-excursion will undo the pop-to-buffer without the
            ; split-current-window
            (if (= (buffer-size) 0)
                (split-current-window))
            (pop-to-buffer "Error log")
            (erase-buffer)
            (setq highlight-region 0)
            (write-modified-files)
            (error-occurred (kill-process "Error log"))
            (if (= operating-system-name "Windows")
                (progn ~old-cli-name ~status
                    (setq ~old-cli-name cli-name)
                    (setq cli-name (concat (getenv "COMSPEC") " /c " compile-it-command))
                    (setq ~status (error-occurred (start-DCL-process "Error log")))
                    (setq cli-name ~old-cli-name)
                    (if ~status (error-message error-message))
                    (set-process-termination-procedure "Error log" "~cargo-terminated")
                    (set-process-output-buffer "Error log" "Error log")
                )
                (progn
                    (start-process "Error log" (concat "cargo " ~cargo-command))
                    (set-process-termination-procedure "Error log" "~cargo-terminated")
                )
            )
            (setq mode-line-format "%b %M (%m)")
            (setq mode-string "Compilation active")
            (novalue)
        )
    )
)

(defun
    (~cargo-terminated
        ~old-parser

        (setq cargo-maybe-active 0)
        (save-excursion
            (temp-use-buffer "Error log")
            (setq mode-string "Finished")

            (setq ~old-parser error-message-parser)
            (setq error-message-parser "rust-error-message-parser")

            (beginning-of-file)
            (set-mark)
            (end-of-file)
            (parse-error-messages-in-region)

            (setq error-message-parser ~old-parser)
        )
        (unset-mark)
        (sit-for 0)

        (next-error)
    )
)

(defun
    (rust-previous-class
        (if
            (error-occurred
                (beginning-of-line)
                (ere-search-reverse "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
            )
            (error-message "rust class not found")
        )
    )
)

(defun
    (rust-next-class
        (if
            (error-occurred
                (end-of-line)
                (ere-search-forward "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
            )
            (error-message "rust class not found")
        )
    )
)

(defun
    (rust-previous-def
        (if
            (error-occurred
                (beginning-of-line)
                (ere-search-reverse "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
                (error-occurred
                    (message "In " (rust-within-class-def)))
            )
            (error-message "rust function def not found")
        )
    )
)

(defun
    (rust-next-def
        (if
            (error-occurred
                (end-of-line)
                (ere-search-forward "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
                (error-occurred
                    (message "In " (rust-within-class-def)))
            )
            (error-message "rust function def not found")
        )
    )
)

(defun
    (rust-within-class
        (save-window-excursion
            (end-of-line)       ; cover the case of being on the class line
            (ere-search-reverse "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
            (region-around-match 1)
            (region-to-string)
        )
    )
)

(defun
    (rust-within-def
        (save-window-excursion
            (end-of-line)       ; cover the case of being on the def line
            (ere-search-reverse "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
            (region-around-match 1)
            (region-to-string)
        )
    )
)

(defun
    (rust-within-class-def
        (save-window-excursion
            ~class-name ~class-column
            ~def-name ~def-column

            (set-mark)
            (end-of-line)
            (if
                (error-occurred (ere-search-reverse "^([ \t]*)class\\s+(\\w+)(?S=C*S*)"))
                ; no class
                (if
                    (error-occurred (rust-within-def))
                    ; no def or class
                    "module level"
                    (rust-within-def)
                )
                ; have a class
                (progn
                    (save-excursion
                        (region-around-match 1)
                        (setq ~class-column (current-column))
                        (region-around-match 2)
                        (setq ~class-name (region-to-string))
                    )
                    (exchange-dot-and-mark)
                    (end-of-line)

                    (save-restriction
                        (narrow-region)
                        (if (error-occurred (ere-search-reverse "^([ \t]*)\\bdef\\s+(\\w+)(?S=C*S*)"))
                            ; no def found
                            ~class-name

                            ; have a class and def maybe
                            (progn
                                (region-around-match 1)
                                (setq ~def-column (current-column))
                                (region-around-match 2)
                                (setq ~def-name (region-to-string))
                                (if (> ~def-column ~class-column)
                                    (concat ~class-name "." ~def-name)
                                    ~def-name
                                )
                            )
                        )
                    )
                )
            )
        )
    )
)

(defun
    ~rust-quote-file( ~file )
    ~new-file
    ~limit
    ~pos
    ~char

    (setq ~new-file "")
    (setq ~limit (length ~file))
    (setq ~pos 0)
    (while (< ~pos ~limit)
        (setq ~pos (+ ~pos 1))
        (setq ~char (substr ~file ~pos 1))
        (if (= ~char "\\")
            (setq ~new-file (concat ~new-file ~char ~char))
            (setq ~new-file (concat ~new-file ~char))
        )
    )
    ~new-file
)


;
; rust syntax table
;
(save-window-excursion
    (temp-use-buffer "~rust-hack")
    (use-syntax-table "rust")

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

    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "paren" "{" "}")
    (modify-syntax-table "paren" "[" "]")

    (modify-syntax-table "string" "\"")
    (modify-syntax-table "string,paired" "r#\"" "\"#")
    (modify-syntax-table "string,paired" "br#\"" "\"#")
    (modify-syntax-table "string,paired" "b\"" "\"")

    ; rust uses ' as a prefix for loop labels as well as for the char literal
    ; 'label: loop
    ; let ch = 'a'
    (modify-syntax-table "string,char" "'")
    (modify-syntax-table "string,paired" "b'" "'")

    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "_")
    (modify-syntax-table "comment" "//" "\n")
    (modify-syntax-table "comment" "/*" "*/")

    (modify-syntax-table "keyword-1" "->")
    (modify-syntax-table "keyword-1" "=>")
    (modify-syntax-table "keyword-1" "and")
    (modify-syntax-table "keyword-1" "as")
    (modify-syntax-table "keyword-1" "async")
    (modify-syntax-table "keyword-1" "await")
    (modify-syntax-table "keyword-1" "break")
    (modify-syntax-table "keyword-1" "const")
    (modify-syntax-table "keyword-1" "continue")
    (modify-syntax-table "keyword-1" "crate")
    (modify-syntax-table "keyword-1" "else")
    (modify-syntax-table "keyword-1" "enum")
    (modify-syntax-table "keyword-1" "extern")
    (modify-syntax-table "keyword-1" "false")
    (modify-syntax-table "keyword-1" "fn")
    (modify-syntax-table "keyword-1" "for")
    (modify-syntax-table "keyword-1" "if")
    (modify-syntax-table "keyword-1" "impl")
    (modify-syntax-table "keyword-1" "in")
    (modify-syntax-table "keyword-1" "let")
    (modify-syntax-table "keyword-1" "loop")
    (modify-syntax-table "keyword-1" "match")
    (modify-syntax-table "keyword-1" "mod")
    (modify-syntax-table "keyword-1" "move")
    (modify-syntax-table "keyword-1" "mut")
    (modify-syntax-table "keyword-1" "pub")
    (modify-syntax-table "keyword-1" "ref")
    (modify-syntax-table "keyword-1" "return")
    (modify-syntax-table "keyword-1" "self")
    (modify-syntax-table "keyword-1" "Self")
    (modify-syntax-table "keyword-1" "static")
    (modify-syntax-table "keyword-1" "struct")
    (modify-syntax-table "keyword-1" "super")
    (modify-syntax-table "keyword-1" "trait")
    (modify-syntax-table "keyword-1" "true")
    (modify-syntax-table "keyword-1" "try")
    (modify-syntax-table "keyword-1" "type")
    (modify-syntax-table "keyword-1" "use")
    (modify-syntax-table "keyword-1" "where")
    (modify-syntax-table "keyword-1" "while")

    (modify-syntax-table "keyword-2" "unsafe")
    (modify-syntax-table "keyword-2" "union")

    (modify-syntax-table "keyword-3" "bool")
    (modify-syntax-table "keyword-3" "true")
    (modify-syntax-table "keyword-3" "false")

    (modify-syntax-table "keyword-3" "tup")
    (modify-syntax-table "keyword-3" "char")

    (modify-syntax-table "keyword-3" "u8")
    (modify-syntax-table "keyword-3" "i8")
    (modify-syntax-table "keyword-3" "u16")
    (modify-syntax-table "keyword-3" "i16")
    (modify-syntax-table "keyword-3" "u32")
    (modify-syntax-table "keyword-3" "i32")
    (modify-syntax-table "keyword-3" "u64")
    (modify-syntax-table "keyword-3" "i64")
    (modify-syntax-table "keyword-3" "u128")
    (modify-syntax-table "keyword-3" "i128")
    (modify-syntax-table "keyword-3" "usize")
    (modify-syntax-table "keyword-3" "isize")
    (modify-syntax-table "keyword-3" "f32")
    (modify-syntax-table "keyword-3" "f64")
)
(save-window-excursion
    (temp-use-buffer "~rust-hack")
    (define-keymap "rust-map")
    (define-keymap "rust-ESC-map")
    (use-local-map "rust-map")
    (local-bind-to-key "rust-ESC-map" "\e")
    (execute-mlisp-file "rust-mode.key")
    (kill-buffer "~rust-hack")
)
(novalue)
