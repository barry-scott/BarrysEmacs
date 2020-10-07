;
; Python-console.ml
;
; Copyright 1999-2002 (c) Barry A. Scott
;
(declare-global python-console-buffer)
(setq python-console-buffer "Python Console")

(defun
    (Py-exec-console-line
        ~line

        (save-excursion
            (beginning-of-line)
            (set-mark)
            (end-of-line)
            (setq ~line (region-to-string))
            (if (eobp)
                (newline)
            )
        )
        (end-of-file)

        (Python-call "python_console_exec_line_store_line" ~line)

        (if (error-occurred (Python-exec
                (concat
                    "try:\n"
                    "    result = eval( __python_console_exec_line )\n"
                    "    pprint.pprint( result )\n"
                    "\n"
                    "except SyntaxError:\n"
                    "    exec( __python_console_exec_line )\n"
                )))
            (progn
                (end-of-file)
                (insert-string error-message)
            )
        )
        (end-of-file)
        (unset-mark)
    )
)

(defun
    (Python-console
        (switch-to-buffer python-console-buffer)
        (Python-console-mode)
        (use-local-map "Python Console Keymap")

        (Python-initialise)

        (if
            ; not initialised yet...
            (Python-eval "not locals().has_key('__python_console_exec_line')")
            (progn
                ; import the python modules needed
                (Python-import "bemacs_stdio")
                (Python-import "pprint")
                ; define the help function
                (Python-exec
                    (concat "__python_console_exec_line = ''\n"
                            "def python_console_exec_line_store_line( line ):\n"
                            "    global __python_console_exec_line\n"
                            "    __python_console_exec_line = line\n"))
            )
        )
    )
)
(save-excursion
    (temp-use-buffer "~python-hack")

    (define-keymap "Python Console Keymap")
    (use-local-map "Python Console Keymap")
    (local-bind-to-key "Py-exec-console-line" "\r")

    (kill-buffer "~python-hack")
    (novalue)
)
