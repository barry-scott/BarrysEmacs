; Miscellaneous local functions
 (defun
    (multi-autoload
        fns
        (if (< (nargs) 2)
            (error-message "Insufficient arguments to multi-autoload"))
        (setq fns 2)
        (while (>= (nargs) fns)
          (progn (autoload (arg fns) (arg 1)) (setq fns (+ fns 1))))
        (novalue))


    (function-bind              ; Do magic to set up bindings
        name                    ; function name
        file                    ; file containing definition
        key                     ; key to bind to
        (setq name (arg 1 "Function Name to bind: "))
        (setq file (arg 2 "File containing definition: "))
        (setq key (arg 3  "Key to bind to function: "))
        (autoload name file)
        (bind-to-key name key)
        (novalue))
 )
