;
; DEBUG.ML
;
; A Mock Lisp debugger for EMACS V4.0
;
; Nick Emery and Barry Scott, 1985
;
(save-window-excursion
i j
(temp-use-buffer "Debug")
(setq current-buffer-checkpointable 0)
(setq current-buffer-journalled 0)
(define-keymap "Debug")
(use-local-map "Debug")
(setq i 0)
(while (< i 256)
    (setq j (global-binding-of i))
    (if (| (= j "self-insert") (= j "digit"))
        (local-bind-to-key "illegal-operation" (char-to-string i))
    )
    (setq i (+ i 1))
)
(declare-global
    debug-show-source
    debug-show-last-expression
    ~debug-step-count
    ~debug-trace-mode
    debug-sit-for
    ~debug-help
    ~debug-old-error-hook
    ~debug-old-trace-hook
    ~debug-old-breakpoint-hook
    ~debug-old-global-mode-string
    ~debug-on
)
(setq ~debug-trace-mode 0)
(setq ~debug-on 0)
(defun
    (debug-error
        (if (!= (substr current-function 1 6) "debug-")
            (save-window-excursion
                (debug-display)
                (erase-buffer)
                (message
                    (concat
                        "Error `"
                        error-message
                        (if (!= current-function "")
                            (concat "\' occurred in function " current-function)
                            "' detected by DEBUG"
                        )
                    )
                )
                (debug-show-source)
                (recursive-edit)
                (temp-use-buffer "Debug")
                (erase-buffer)
                (exit-emacs)
            )
        )
    )
)
(defun
    (debug-show-source
        (erase-buffer)
        (if (& debug-show-source (!= current-function ""))
            (progn
                (decompile-mlisp-function current-function)
                (set-mark)
                (end-of-line)
                (exchange-dot-and-mark)
            )
            (progn
                (insert-string (decompile-current-line))
                (unset-mark)
            )
        )
        (if debug-show-last-expression
            (save-window-excursion (debug-show-last-expression))
        )
    )
)
(defun
    (debug-trace
        (setq ~debug-step-count (- ~debug-step-count 1))
        (if (&
                (<= ~debug-step-count 0)
                (!= (substr current-function 1 6) "debug-")
            )
            (save-window-excursion
                (setq ~debug-step-count 0)
                (debug-display)
                (if (! ~debug-trace-mode)
                    (message (concat "Step in function " current-function))
                )
                (debug-show-source)
                (if (! ~debug-trace-mode)
                    (recursive-edit)
                    (sit-for debug-sit-for)
                )
                (temp-use-buffer "Debug")
                (erase-buffer)
            )
        )
    )
)
(defun
    (debug-break
        (if (!= (substr current-function 1 6) "debug-")
            (save-window-excursion
                (debug-display)
                (message
                    (concat
                        "Breakpoint before function "
                        current-breakpoint
                        (if (!= current-function "")
                            (concat " called from function " current-function)
                            ""
                        )
                    )
                )
                (debug-show-source)
                (recursive-edit)
                (temp-use-buffer "Debug")
                (erase-buffer)
            )
        )
    )
)
(defun
    (debug-show-last-expression
        (debug-show-expression "last-expression" last-expression)
        (pop-to-buffer "Debug")
    )
)
(defun
    (debug-show-variable
        ~debug-variable

        (setq ~debug-variable (get-tty-variable "Show variable "))
        (debug-show-expression
            ~debug-variable
            (execute-mlisp-line ~debug-variable))
        (pop-to-buffer "Debug")
    )
)
(defun
    debug-show-expression (name expr)
    type
    bounds

    (pop-to-buffer "Debug-variable")
    (setq current-buffer-checkpointable 0)
    (setq current-buffer-journalled 0)
    (erase-buffer)
    (setq type (type-of-expression expr))
    (insert-string (concat name " is type " type "\n"))
    (if (= type "integer")
        (insert-string (concat "Value " expr "\n"))
        (= type "string")
        (insert-string (concat "Value \"" expr "\""))
        (= type "marker")
        (insert-string (concat "Value \"" expr "\", " (+ expr)))
        (= type "windows")
        (insert-string "is a set of windows.")
        (= type "array")
        (progn
            (setq bounds (bounds-of-array expr))
            (insert-string
                (concat
                    "Has " (fetch-array bounds 1 0)
                    (if (= 1 (fetch-array bounds 1 0))
                        " dimension\n"
                        " dimensions\n"
                    )
                    "Element            Value\n"
                    "-------            -----\n"
                )
            )
            (debug-list-array expr bounds 1 "" "")
        )
        (insert-string (concat "unexpected type of " type))
    )
    (beginning-of-file)
)
(defun
    debug-list-array (array bounds level indices prefix)

    index high-bound expr type

    (setq index (fetch-array bounds 1 level))
    (setq high-bound (fetch-array bounds 2 level))
    (if (length prefix) (setq prefix (concat prefix ", ")))

    (if (= level (fetch-array bounds 1 0))
        (while (<= index high-bound)
            (progn
                (insert-string (concat prefix index))
                (to-col 17)
                (setq expr
                    (execute-mlisp-line
                        (concat "(fetch-array array " indices " index)")))
                (setq type (type-of-expression expr))
                (if (= type "integer")
                    (insert-string expr)
                    (= type "string")
                    (insert-string expr)
                    (= type "marker")
                    (insert-string (concat "\"" expr "\", " (+ expr)))
                    (= type "windows")
                    (insert-string "set of windows")
                    (insert-string (concat "unexpected type of " type))
                )
                (insert-string "\n")
                (setq index (+ index 1))
            )
        )
        (progn
            (setq level (+ level 1))

            (while (<= index high-bound)
                (progn
                    (debug-list-array array bounds level
                        (concat indices " " index) (concat prefix index))
                    (setq index (+ index 1))
                )
            )
        )
    )
)
(defun
    (debug-mode-string
        (save-window-excursion
            (temp-use-buffer "Debug")
            (setq mode-string
                (if (= error-hook "debug-error") "Errors trapped" "")
            )
            (if ~debug-trace-mode
                (progn
                    (if (> (length mode-string) 0)
                        (setq mode-string (concat mode-string ", "))
                    )
                    (setq mode-string
                        (concat
                            mode-string
                            "Tracing"
                        )
                    ))
            )
            (if (> (length mode-string) 0)
                (setq mode-string (concat " (" mode-string ")"))
            )
        )
    )
)
(defun
    (debug-set-exception
        (setq error-hook "debug-error")
        (debug-mode-string)
    )
)
(defun
    (debug-cancel-exception
        (setq error-hook "")
        (debug-mode-string)
    )
)
(defun
    (debug-go
        (setq trace-into ~debug-trace-mode)
        (setq trace-mode ~debug-trace-mode)
        (if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug-step
        (setq trace-into 1)
        (setq trace-mode 1)
        (if prefix-argument-provided
            (setq ~debug-step-count prefix-argument)
            (setq ~debug-step-count 1)
        )
        (if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug-step-over
        (setq trace-into 0)
        (setq trace-mode 1)
        (if prefix-argument-provided
            (setq ~debug-step-count prefix-argument)
            (setq ~debug-step-count 1)
        )
        (if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug
        (debug-display)
        (erase-buffer)
        (if (! ~debug-on)
            (progn
                (setq mode-line-format "%M    Command level%m")
                (setq mode-string "")
                (setq ~debug-old-trace-hook trace-hook)
                (setq ~debug-old-breakpoint-hook breakpoint-hook)
                (setq ~debug-old-error-hook error-hook)
                (setq ~debug-old-global-mode-string global-mode-string)
                (setq trace-hook "debug-trace")
                (setq breakpoint-hook "debug-break")
                (setq error-hook "")
                (setq global-mode-string "DEBUG")
                (setq ~debug-on 1)
                (debug-mode-string)
            )
        )
        (novalue)
    )
)
(defun
    (debug-set-break
        (breakpoint (get-tty-command "Set breakpoint (for function) ") 1)
    )
)
(defun
    (debug-can-break
        (breakpoint (get-tty-command "Cancel breakpoint (for function) ") 0)
    )
)
(defun
    (debug-show-breakpoints
        (save-window-excursion
            (list-breakpoints)
        )
        (debug-display)
        (erase-buffer)
        (yank-buffer "Breakpoint list")
        (beginning-of-file)
    )
)
(defun
    (debug-show-calls
        (save-window-excursion
            (dump-stack-trace)
        )
        (erase-buffer)
        (yank-buffer "Stack trace")
        (beginning-of-file)
        (provide-prefix-argument 6 (kill-to-end-of-line))
    )
)
(defun
    (debug-trace-mode
        (setq ~debug-trace-mode 1)
        (setq trace-mode 1)
        (setq ~debug-step-count 0)
        (debug-mode-string)
    )
)
(defun
    (debug-notrace-mode
        (setq ~debug-trace-mode 0)
        (setq trace-mode 0)
        (setq ~debug-step-count 0)
        (debug-mode-string)
    )
)
(defun
    (debug-exit
        (setq trace-hook ~debug-old-trace-hook)
        (setq breakpoint-hook ~debug-old-breakpoint-hook)
        (setq error-hook ~debug-old-error-hook)
        (setq global-mode-string ~debug-old-global-mode-string)
        (setq trace-mode 0)
        (pop-to-buffer "Debug")
        (delete-window)
        (if debug-show-last-expression
            (progn
                (pop-to-buffer "Debug-variable")
                (delete-window)
            )
        )
        (setq ~debug-on 0)
        (novalue)
    )
)
(defun
    (debug-help
        (save-window-excursion
            (pop-to-buffer "Help")
            (delete-other-windows)
            (erase-buffer)
            (insert-string ~debug-help)
            (beginning-of-file)
            (setq mode-line-format "")
            (message " -- Type any character to continue DEBUG --")
            (get-tty-character)
            (setq mode-line-format default-mode-line-format)
        )
    )
)
;
; put up the debugger windows onto the screen
;
(defun
    (debug-display
        (pop-to-buffer "Debug")
        (if debug-show-last-expression
            (if
                (save-window-excursion
                    (next-window)
                    (!= current-buffer-name "Debug-variable")
                )
                (progn
                    (if (< window-size 10)
                        (setq window-size 10))
                    (split-current-window)
                    (switch-to-buffer "~debug-temp~")
                    (next-window)
                    (while (!= current-buffer-name "~debug-temp~")
                        (if (= current-buffer-name "Debug-variable")
                            (delete-window)
                        )
                        (next-window)
                    )
                    (switch-to-buffer "Debug-variable")
                    (setq mode-line-format "Debug show expression %p")
                    (kill-buffer "~debug-temp~")
                    (setq window-size 3)
                    (previous-window)
                )
            )
        )
    )
)
(execute-mlisp-file "debug.key")
(novalue)
)
