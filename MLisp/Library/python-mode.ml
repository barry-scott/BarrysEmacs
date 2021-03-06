;
; Python mode
;
(declare-global Python-checker-command)
(declare-global Python-default-version)
(declare-global Python2-executable)
(declare-global Python3-executable)
(declare-buffer-specific Python-command)

(defun
    (is-windows
        (= operating-system-name "Windows")
    )
)

(if (is-windows)
    (progn
        (setq Python-checker-command "nmake.exe check")
        (setq Python-default-version 3)
        (setq Python2-executable "py -2")
        (setq Python3-executable "py -3")
        (setq-default Python-command Python3-executable)
    )
    (progn
        (setq Python-checker-command "make check")
        (setq Python-default-version 3)
        (setq Python2-executable "python2")
        (setq Python3-executable "python3")
        (setq-default Python-command Python3-executable)
    )
)

(defun
    (Python-mode
        (if
            (= (Python-guess-version) 2)
            (Python2-mode)
            (Python3-mode)
        )
        (novalue)
    )
)

(defun
    (Python2-mode
        (setq mode-string "Python2")
        (use-syntax-table "Python2")
        (use-local-map "Python-map")
        (use-abbrev-table "Python2")
        (setq Python-command Python2-executable)
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.py"))
        (novalue)
    )
)

(defun
    (Python3-mode
        (setq mode-string "Python3")
        (use-syntax-table "Python3")
        (use-local-map "Python-map")
        (use-abbrev-table "Python3")
        (setq Python-command Python3-executable)
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.py"))
        (novalue)
    )
)

(defun
    (Python-guess-version
        (save-excursion
            (beginning-of-file)
            (if
                (ere-looking-at "#!")
                (if
                    (ere-looking-at ".*python3")
                    3
                    2
                )
                Python-default-version
            )
        )
    )
)

;
; used by the Python-console module which share all but the keymap
;
(defun
    (Python-console-mode
        (setq mode-string "Python")
        (use-syntax-table "Python3")
        (use-abbrev-table "Python3")
        (novalue)
    )
)

(defun
    (Python-syntax-check
        ~file
        ~cmd

        (setq ~file current-buffer-file-name)
        (if buffer-is-modified
            (write-current-file)
        )

        (setq ~cmd
            (concat
                Python-command " -c \"import py_compile,sys;sys.stderr = sys.stdout;py_compile.compile('" (~Python-quote-file ~file) "')\""
            )
        )

        (temp-use-buffer "Python Compile")
        (erase-buffer)
        (set-mark)

        (filter-region ~cmd)

        (beginning-of-file)
        (if (! (error-occurred (re-search-forward "File \"\\(.*\\)\", line \\([0-9][0-9]*\\)")))
            (progn
                ~file ~line
                (region-around-match 1)
                (setq ~file (region-to-string))
                (region-around-match 2)
                (setq ~line (region-to-string))
                (unset-mark)
                (beginning-of-line)
                (visit-file ~file)
                (goto-line (+ ~line))
                (save-excursion
                    (pop-to-buffer "Python Compile")
                )
            )
            (message "No errors found")
        )
        (novalue)
    )
)

(defun
    (Python-checker-error-message-parser
        (if
            (error-occurred
                (if
                    (error-occurred
                        (re-search-forward
                            "\\(.*\\):\\([0-9][0-9]*\\)[:]* "
                        )
                    )
                    (setq error-line-number 0)
                    (save-window-excursion
                        (region-around-match 1)
                        (if (!= (dot) (mark))
                            (progn
                                (setq error-file-name (region-to-string))
                                (while (! (file-exists error-file-name))
                                    (setq error-file-name (get-tty-file "Locate file: " error-file-name))
                                )
                                (region-around-match 2)
                                (setq error-line-number (region-to-string))
                            )
                            (progn
                                (region-around-match 3)
                                (setq error-file-name (region-to-string))
                                (region-around-match 4)
                                (setq error-line-number (region-to-string))
                            )
                        )
                        (beginning-of-line)
                        (setq error-start-position (dot))
                        (next-line)
                        (beginning-of-line)
                    )
                )
            )
            (progn
                (get-tty-string error-message)
                (error-message error-message)
            )
        )
    )
)

(defun
    (Python-checker
        ~old-parser

        (write-modified-files)

        (pop-to-buffer "Python Checker")
        (erase-buffer)
        (set-mark)

        (message "Running PyChecker...")(sit-for 0)
        (filter-region Python-checker-command)

        (setq ~old-parser error-message-parser)
        (setq error-message-parser "Python-checker-error-message-parser")

        (beginning-of-file)
        (set-mark)
        (end-of-file)
        (parse-error-messages-in-region)

        (setq error-message-parser ~old-parser)

        (next-error)

        (novalue)
    )
)


(defun
    (Python-compile
        (Python-syntax-check)
    )
)

(defun
    (Python-previous-class
        (if
            (error-occurred
                (beginning-of-line)
                (ere-search-reverse "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
            )
            (error-message "Python class not found")
        )
    )
)

(defun
    (Python-next-class
        (if
            (error-occurred
                (end-of-line)
                (ere-search-forward "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
            )
            (error-message "Python class not found")
        )
    )
)

(defun
    (Python-previous-def
        (if
            (error-occurred
                (beginning-of-line)
                (ere-search-reverse "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
                (error-occurred
                    (message "In " (Python-within-class-def)))
            )
            (error-message "Python function def not found")
        )
    )
)

(defun
    (Python-next-def
        (if
            (error-occurred
                (end-of-line)
                (ere-search-forward "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
                (beginning-of-line)
                (error-occurred
                    (message "In " (Python-within-class-def)))
            )
            (error-message "Python function def not found")
        )
    )
)

(defun
    (Python-within-class
        (save-window-excursion
            (end-of-line)       ; cover the case of being on the class line
            (ere-search-reverse "^[ \t]*class\\s+(\\w+)(?S=C*S*)")
            (region-around-match 1)
            (region-to-string)
        )
    )
)

(defun
    (Python-within-def
        (save-window-excursion
            (end-of-line)       ; cover the case of being on the def line
            (ere-search-reverse "^[ \t]*\\bdef\\s+(\\w+)(?S=C*S*)")
            (region-around-match 1)
            (region-to-string)
        )
    )
)

(defun
    (Python-within-class-def
        (save-window-excursion
            ~class-name ~class-column
            ~def-name ~def-column

            (set-mark)
            (end-of-line)
            (if
                (error-occurred (ere-search-reverse "^([ \t]*)class\\s+(\\w+)(?S=C*S*)"))
                ; no class
                (if
                    (error-occurred (Python-within-def))
                    ; no def or class
                    "module level"
                    (Python-within-def)
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
    ~Python-quote-file( ~file )
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

(defun
    (Python-traceback-visit-file
        ~filename ~line
        (setq ~filename "")
        (beginning-of-line)
        (if
            (error-occurred
                (ere-search-forward "File \"(.*)\", line (\\d+),")
                (save-excursion
                    (region-around-match 1)
                    (setq ~filename (region-to-string))
                    (region-around-match 2)
                    (setq ~line (region-to-string))
                )
                (beginning-of-line)
                (next-line)
            )
            (message "Python traceback not found")
            (= ~filename "<stdin>")
            (message "traceback in <stdin>")
            (progn
                (visit-file ~filename)
                (goto-line ~line)
                (message "foo")
            )
        )
    )
)

(defun
    (Python-insert-main-boiler-plate
        (insert-string
            "#!/usr/bin/env python" "\n"
            "import sys" "\n"
            "\n"
            "def main( argv ):" "\n"
            "    return 0" "\n"
            "\n"
            "if __name__ == '__main__':" "\n"
            "    sys.exit( main( sys.argv ) )" "\n"
        )
        (novalue)
    )
)

;
; Python 2 syntax table
;
(save-window-excursion
    (temp-use-buffer "~python-hack")
    (use-syntax-table "Python2")

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

    (modify-syntax-table "problem,ere" "  *\t+")
    (modify-syntax-table "problem,ere" " [ \t]*$")
    (modify-syntax-table "problem,ere" "\t[ \t]*$")

    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "paren" "{" "}")
    (modify-syntax-table "paren" "[" "]")
    (modify-syntax-table "string,paired" "\"\"\"" "\"\"\"")
    (modify-syntax-table "string" "\"")
    (modify-syntax-table "string,paired" "'''" "'''")
    (modify-syntax-table "string" "'")
    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "_")
    (modify-syntax-table "comment" "#" "\n")

    (modify-syntax-table "keyword-1" "as")
    (modify-syntax-table "keyword-1" "and")
    (modify-syntax-table "keyword-1" "assert")
    (modify-syntax-table "keyword-1" "break")
    (modify-syntax-table "keyword-1" "class")
    (modify-syntax-table "keyword-1" "continue")
    (modify-syntax-table "keyword-1" "def")
    (modify-syntax-table "keyword-1" "del")
    (modify-syntax-table "keyword-1" "elif")
    (modify-syntax-table "keyword-1" "else:")
    (modify-syntax-table "keyword-1" "except")
    (modify-syntax-table "keyword-1" "exec")
    (modify-syntax-table "keyword-1" "finally")
    (modify-syntax-table "keyword-1" "for")
    (modify-syntax-table "keyword-1" "from")
    (modify-syntax-table "keyword-1" "global")
    (modify-syntax-table "keyword-1" "if")
    (modify-syntax-table "keyword-1" "import")
    (modify-syntax-table "keyword-1" "in")
    (modify-syntax-table "keyword-1" "is")
    (modify-syntax-table "keyword-1" "lambda")
    (modify-syntax-table "keyword-1" "not")
    (modify-syntax-table "keyword-1" "or")
    (modify-syntax-table "keyword-1" "pass")
    (modify-syntax-table "keyword-1" "print")
    (modify-syntax-table "keyword-1" "raise")
    (modify-syntax-table "keyword-1" "return")
    (modify-syntax-table "keyword-1" "try")
    (modify-syntax-table "keyword-1" "while")
    (modify-syntax-table "keyword-1" "with")
    (modify-syntax-table "keyword-1" "yield")

    (modify-syntax-table "keyword-2" "ArithmeticError")
    (modify-syntax-table "keyword-2" "AssertionError")
    (modify-syntax-table "keyword-2" "AttributeError")
    (modify-syntax-table "keyword-2" "DeprecationWarning")
    (modify-syntax-table "keyword-2" "EnvironmentError")
    (modify-syntax-table "keyword-2" "EOFError")
    (modify-syntax-table "keyword-2" "Exception")
    (modify-syntax-table "keyword-2" "FloatingPointError")
    (modify-syntax-table "keyword-2" "FutureWarning")
    (modify-syntax-table "keyword-2" "ImportError")
    (modify-syntax-table "keyword-2" "IndentationError")
    (modify-syntax-table "keyword-2" "IndexError")
    (modify-syntax-table "keyword-2" "IOError")
    (modify-syntax-table "keyword-2" "KeyboardInterrupt")
    (modify-syntax-table "keyword-2" "KeyError")
    (modify-syntax-table "keyword-2" "LookupError")
    (modify-syntax-table "keyword-2" "MemoryError")
    (modify-syntax-table "keyword-2" "NameError")
    (modify-syntax-table "keyword-2" "NotImplementedError")
    (modify-syntax-table "keyword-2" "OSError")
    (modify-syntax-table "keyword-2" "OverflowError")
    (modify-syntax-table "keyword-2" "PendingDeprecationWarning")
    (modify-syntax-table "keyword-2" "ReferenceError")
    (modify-syntax-table "keyword-2" "RuntimeError")
    (modify-syntax-table "keyword-2" "RuntimeWarning")
    (modify-syntax-table "keyword-2" "StandardError")
    (modify-syntax-table "keyword-2" "StopIteration")
    (modify-syntax-table "keyword-2" "SyntaxError")
    (modify-syntax-table "keyword-2" "SyntaxWarning")
    (modify-syntax-table "keyword-2" "SystemError")
    (modify-syntax-table "keyword-2" "SystemExit")
    (modify-syntax-table "keyword-2" "TabError")
    (modify-syntax-table "keyword-2" "TypeError")
    (modify-syntax-table "keyword-2" "UnboundLocalError")
    (modify-syntax-table "keyword-2" "UnicodeDecodeError")
    (modify-syntax-table "keyword-2" "UnicodeEncodeError")
    (modify-syntax-table "keyword-2" "UnicodeError")
    (modify-syntax-table "keyword-2" "UnicodeTranslateError")
    (modify-syntax-table "keyword-2" "UserWarning")
    (modify-syntax-table "keyword-2" "ValueError")
    (modify-syntax-table "keyword-2" "Warning")
    (modify-syntax-table "keyword-2" "WindowsError")
    (modify-syntax-table "keyword-2" "ZeroDivisionError")

    (modify-syntax-table "keyword-2" "_")
    (modify-syntax-table "keyword-2" "__debug__")
    (modify-syntax-table "keyword-2" "__doc__")
    (modify-syntax-table "keyword-2" "__import__")
    (modify-syntax-table "keyword-2" "__name__")
    (modify-syntax-table "keyword-2" "abs")
    (modify-syntax-table "keyword-2" "apply")
    (modify-syntax-table "keyword-2" "basestring")
    (modify-syntax-table "keyword-2" "bool")
    (modify-syntax-table "keyword-2" "buffer")
    (modify-syntax-table "keyword-2" "callable")
    (modify-syntax-table "keyword-2" "chr")
    (modify-syntax-table "keyword-2" "classmethod")
    (modify-syntax-table "keyword-2" "cmp")
    (modify-syntax-table "keyword-2" "coerce")
    (modify-syntax-table "keyword-2" "compile")
    (modify-syntax-table "keyword-2" "complex")
    (modify-syntax-table "keyword-2" "delattr")
    (modify-syntax-table "keyword-2" "dict")
    (modify-syntax-table "keyword-2" "dir")
    (modify-syntax-table "keyword-2" "divmod")
    (modify-syntax-table "keyword-2" "Ellipsis")
    (modify-syntax-table "keyword-2" "enumerate")
    (modify-syntax-table "keyword-2" "eval")
    (modify-syntax-table "keyword-2" "Exception")
    (modify-syntax-table "keyword-2" "execfile")
    (modify-syntax-table "keyword-2" "False")
    (modify-syntax-table "keyword-2" "file")
    (modify-syntax-table "keyword-2" "filter")
    (modify-syntax-table "keyword-2" "float")
    (modify-syntax-table "keyword-2" "frozenset")
    (modify-syntax-table "keyword-2" "getattr")
    (modify-syntax-table "keyword-2" "globals")
    (modify-syntax-table "keyword-2" "hasattr")
    (modify-syntax-table "keyword-2" "hash")
    (modify-syntax-table "keyword-2" "help")
    (modify-syntax-table "keyword-2" "hex")
    (modify-syntax-table "keyword-2" "id")
    (modify-syntax-table "keyword-2" "input")
    (modify-syntax-table "keyword-2" "int")
    (modify-syntax-table "keyword-2" "intern")
    (modify-syntax-table "keyword-2" "isinstance")
    (modify-syntax-table "keyword-2" "issubclass")
    (modify-syntax-table "keyword-2" "iter")
    (modify-syntax-table "keyword-2" "len")
    (modify-syntax-table "keyword-2" "list")
    (modify-syntax-table "keyword-2" "locals")
    (modify-syntax-table "keyword-2" "long")
    (modify-syntax-table "keyword-2" "map")
    (modify-syntax-table "keyword-2" "max")
    (modify-syntax-table "keyword-2" "min")
    (modify-syntax-table "keyword-2" "None")
    (modify-syntax-table "keyword-2" "NotImplemented")
    (modify-syntax-table "keyword-2" "object")
    (modify-syntax-table "keyword-2" "oct")
    (modify-syntax-table "keyword-2" "open")
    (modify-syntax-table "keyword-2" "ord")
    (modify-syntax-table "keyword-2" "pow")
    (modify-syntax-table "keyword-2" "property")
    (modify-syntax-table "keyword-2" "range")
    (modify-syntax-table "keyword-2" "raw_input")
    (modify-syntax-table "keyword-2" "reduce")
    (modify-syntax-table "keyword-2" "reload")
    (modify-syntax-table "keyword-2" "repr")
    (modify-syntax-table "keyword-2" "round")
    (modify-syntax-table "keyword-2" "set")
    (modify-syntax-table "keyword-2" "setattr")
    (modify-syntax-table "keyword-2" "slice")
    (modify-syntax-table "keyword-2" "staticmethod")
    (modify-syntax-table "keyword-2" "str")
    (modify-syntax-table "keyword-2" "sum")
    (modify-syntax-table "keyword-2" "super")
    (modify-syntax-table "keyword-2" "True")
    (modify-syntax-table "keyword-2" "tuple")
    (modify-syntax-table "keyword-2" "type")
    (modify-syntax-table "keyword-2" "unichr")
    (modify-syntax-table "keyword-2" "unicode")
    (modify-syntax-table "keyword-2" "vars")
    (modify-syntax-table "keyword-2" "xrange")
    (modify-syntax-table "keyword-2" "zip")
)
(save-window-excursion
    (temp-use-buffer "~python-hack")
    (use-syntax-table "Python3")

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
    (modify-syntax-table "string,paired" "\"\"\"" "\"\"\"")
    (modify-syntax-table "string" "\"")
    (modify-syntax-table "string,paired" "'''" "'''")
    (modify-syntax-table "string" "'")
    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "_")
    (modify-syntax-table "comment" "#" "\n")

    (modify-syntax-table "keyword-1" "and")
    (modify-syntax-table "keyword-1" "as")
    (modify-syntax-table "keyword-1" "assert")
    (modify-syntax-table "keyword-1" "async")
    (modify-syntax-table "keyword-1" "await")
    (modify-syntax-table "keyword-1" "break")
    (modify-syntax-table "keyword-1" "class")
    (modify-syntax-table "keyword-1" "continue")
    (modify-syntax-table "keyword-1" "def")
    (modify-syntax-table "keyword-1" "del")
    (modify-syntax-table "keyword-1" "elif")
    (modify-syntax-table "keyword-1" "else")
    (modify-syntax-table "keyword-1" "else:")
    (modify-syntax-table "keyword-1" "except")
    (modify-syntax-table "keyword-1" "False")
    (modify-syntax-table "keyword-1" "finally")
    (modify-syntax-table "keyword-1" "for")
    (modify-syntax-table "keyword-1" "from")
    (modify-syntax-table "keyword-1" "global")
    (modify-syntax-table "keyword-1" "if")
    (modify-syntax-table "keyword-1" "import")
    (modify-syntax-table "keyword-1" "in")
    (modify-syntax-table "keyword-1" "is")
    (modify-syntax-table "keyword-1" "lambda")
    (modify-syntax-table "keyword-1" "None")
    (modify-syntax-table "keyword-1" "nonlocal")
    (modify-syntax-table "keyword-1" "not")
    (modify-syntax-table "keyword-1" "or")
    (modify-syntax-table "keyword-1" "pass")
    (modify-syntax-table "keyword-1" "raise")
    (modify-syntax-table "keyword-1" "return")
    (modify-syntax-table "keyword-1" "True")
    (modify-syntax-table "keyword-1" "try")
    (modify-syntax-table "keyword-1" "while")
    (modify-syntax-table "keyword-1" "with")
    (modify-syntax-table "keyword-1" "yield")

    (modify-syntax-table "keyword-2" "Exception")
    (modify-syntax-table "keyword-2" "BaseException")

    (modify-syntax-table "keyword-2" "ArithmeticError")
    (modify-syntax-table "keyword-2" "AssertionError")
    (modify-syntax-table "keyword-2" "AttributeError")
    (modify-syntax-table "keyword-2" "BlockingIOError")
    (modify-syntax-table "keyword-2" "BrokenPipeError")
    (modify-syntax-table "keyword-2" "BufferError")
    (modify-syntax-table "keyword-2" "ChildProcessError")
    (modify-syntax-table "keyword-2" "ConnectionAbortedError")
    (modify-syntax-table "keyword-2" "ConnectionError")
    (modify-syntax-table "keyword-2" "ConnectionRefusedError")
    (modify-syntax-table "keyword-2" "ConnectionResetError")
    (modify-syntax-table "keyword-2" "EnvironmentError")
    (modify-syntax-table "keyword-2" "EOFError")
    (modify-syntax-table "keyword-2" "FileExistsError")
    (modify-syntax-table "keyword-2" "FileNotFoundError")
    (modify-syntax-table "keyword-2" "FloatingPointError")
    (modify-syntax-table "keyword-2" "GeneratorExit")
    (modify-syntax-table "keyword-2" "ImportError")
    (modify-syntax-table "keyword-2" "IndentationError")
    (modify-syntax-table "keyword-2" "IndexError")
    (modify-syntax-table "keyword-2" "InterruptedError")
    (modify-syntax-table "keyword-2" "IOError")
    (modify-syntax-table "keyword-2" "IsADirectoryError")
    (modify-syntax-table "keyword-2" "KeyboardInterrupt")
    (modify-syntax-table "keyword-2" "KeyError")
    (modify-syntax-table "keyword-2" "LookupError")
    (modify-syntax-table "keyword-2" "MemoryError")
    (modify-syntax-table "keyword-2" "NameError")
    (modify-syntax-table "keyword-2" "NotADirectoryError")
    (modify-syntax-table "keyword-2" "NotImplementedError")
    (modify-syntax-table "keyword-2" "OSError")
    (modify-syntax-table "keyword-2" "OverflowError")
    (modify-syntax-table "keyword-2" "PermissionError")
    (modify-syntax-table "keyword-2" "ProcessLookupError")
    (modify-syntax-table "keyword-2" "ReferenceError")
    (modify-syntax-table "keyword-2" "RuntimeError")
    (modify-syntax-table "keyword-2" "StandardError")
    (modify-syntax-table "keyword-2" "StopIteration")
    (modify-syntax-table "keyword-2" "SyntaxError")
    (modify-syntax-table "keyword-2" "SystemError")
    (modify-syntax-table "keyword-2" "SystemExit")
    (modify-syntax-table "keyword-2" "TabError")
    (modify-syntax-table "keyword-2" "TimeoutError")
    (modify-syntax-table "keyword-2" "TypeError")
    (modify-syntax-table "keyword-2" "UnboundLocalError")
    (modify-syntax-table "keyword-2" "UnicodeDecodeError")
    (modify-syntax-table "keyword-2" "UnicodeEncodeError")
    (modify-syntax-table "keyword-2" "UnicodeError")
    (modify-syntax-table "keyword-2" "UnicodeTranslateError")
    (modify-syntax-table "keyword-2" "ValueError")
    (modify-syntax-table "keyword-2" "Warning")
    (modify-syntax-table "keyword-2" "WindowsError")
    (modify-syntax-table "keyword-2" "ZeroDivisionError")

    (modify-syntax-table "keyword-2" "BytesWarning")
    (modify-syntax-table "keyword-2" "DeprecationWarning")
    (modify-syntax-table "keyword-2" "DeprecationWarning")
    (modify-syntax-table "keyword-2" "FutureWarning")
    (modify-syntax-table "keyword-2" "ImportWarning")
    (modify-syntax-table "keyword-2" "PendingDeprecationWarning")
    (modify-syntax-table "keyword-2" "ResourceWarning")
    (modify-syntax-table "keyword-2" "RuntimeWarning")
    (modify-syntax-table "keyword-2" "SyntaxWarning")
    (modify-syntax-table "keyword-2" "UnicodeWarning")
    (modify-syntax-table "keyword-2" "UserWarning")
    (modify-syntax-table "keyword-2" "Warning")

    (modify-syntax-table "keyword-2" "_")
    (modify-syntax-table "keyword-2" "__class__")
    (modify-syntax-table "keyword-2" "__debug__")
    (modify-syntax-table "keyword-2" "__doc__")
    (modify-syntax-table "keyword-2" "__import__")
    (modify-syntax-table "keyword-2" "__name__")
    (modify-syntax-table "keyword-2" "abs")
    (modify-syntax-table "keyword-2" "all")
    (modify-syntax-table "keyword-2" "any")
    (modify-syntax-table "keyword-2" "ascii")
    (modify-syntax-table "keyword-2" "bin")
    (modify-syntax-table "keyword-2" "bool")
    (modify-syntax-table "keyword-2" "bytearray")
    (modify-syntax-table "keyword-2" "bytes")
    (modify-syntax-table "keyword-2" "callable")
    (modify-syntax-table "keyword-2" "chr")
    (modify-syntax-table "keyword-2" "classmethod")
    (modify-syntax-table "keyword-2" "compile")
    (modify-syntax-table "keyword-2" "complex")
    (modify-syntax-table "keyword-2" "copyright")
    (modify-syntax-table "keyword-2" "credits")
    (modify-syntax-table "keyword-2" "delattr")
    (modify-syntax-table "keyword-2" "dict")
    (modify-syntax-table "keyword-2" "dir")
    (modify-syntax-table "keyword-2" "divmod")
    (modify-syntax-table "keyword-2" "enumerate")
    (modify-syntax-table "keyword-2" "eval")
    (modify-syntax-table "keyword-2" "exec")
    (modify-syntax-table "keyword-2" "exit")
    (modify-syntax-table "keyword-2" "filter")
    (modify-syntax-table "keyword-2" "float")
    (modify-syntax-table "keyword-2" "format")
    (modify-syntax-table "keyword-2" "frozenset")
    (modify-syntax-table "keyword-2" "getattr")
    (modify-syntax-table "keyword-2" "globals")
    (modify-syntax-table "keyword-2" "hasattr")
    (modify-syntax-table "keyword-2" "hash")
    (modify-syntax-table "keyword-2" "help")
    (modify-syntax-table "keyword-2" "hex")
    (modify-syntax-table "keyword-2" "id")
    (modify-syntax-table "keyword-2" "input")
    (modify-syntax-table "keyword-2" "int")
    (modify-syntax-table "keyword-2" "isinstance")
    (modify-syntax-table "keyword-2" "issubclass")
    (modify-syntax-table "keyword-2" "iter")
    (modify-syntax-table "keyword-2" "len")
    (modify-syntax-table "keyword-2" "license")
    (modify-syntax-table "keyword-2" "list")
    (modify-syntax-table "keyword-2" "locals")
    (modify-syntax-table "keyword-2" "map")
    (modify-syntax-table "keyword-2" "max")
    (modify-syntax-table "keyword-2" "memoryview")
    (modify-syntax-table "keyword-2" "min")
    (modify-syntax-table "keyword-2" "next")
    (modify-syntax-table "keyword-2" "object")
    (modify-syntax-table "keyword-2" "oct")
    (modify-syntax-table "keyword-2" "open")
    (modify-syntax-table "keyword-2" "ord")
    (modify-syntax-table "keyword-2" "pow")
    (modify-syntax-table "keyword-2" "print")
    (modify-syntax-table "keyword-2" "property")
    (modify-syntax-table "keyword-2" "quit")
    (modify-syntax-table "keyword-2" "range")
    (modify-syntax-table "keyword-2" "repr")
    (modify-syntax-table "keyword-2" "reversed")
    (modify-syntax-table "keyword-2" "round")
    (modify-syntax-table "keyword-2" "set")
    (modify-syntax-table "keyword-2" "setattr")
    (modify-syntax-table "keyword-2" "slice")
    (modify-syntax-table "keyword-2" "sorted")
    (modify-syntax-table "keyword-2" "staticmethod")
    (modify-syntax-table "keyword-2" "str")
    (modify-syntax-table "keyword-2" "sum")
    (modify-syntax-table "keyword-2" "super")
    (modify-syntax-table "keyword-2" "tuple")
    (modify-syntax-table "keyword-2" "type")
    (modify-syntax-table "keyword-2" "vars")
    (modify-syntax-table "keyword-2" "zip")
)
(save-window-excursion
    (temp-use-buffer "~python-hack")
    (define-keymap "Python-map")
    (define-keymap "Python-ESC-map")
    (use-local-map "Python-map")
    (local-bind-to-key "Python-ESC-map" "\e")
    (execute-mlisp-file "python-mode.key")
    (kill-buffer "~python-hack")
)
(novalue)
