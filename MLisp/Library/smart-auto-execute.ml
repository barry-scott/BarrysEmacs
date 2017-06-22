;
; smart-auto-execute.ml
;
; Copyright (c) 2004-2017 Barry A. Scott
;
(declare-global smart-auto-handlers)

(defun smart-auto-register-handler(~pattern ~handler)
    ~next

    (setq ~next (fetch-array smart-auto-handlers 0 1))

    (setq-array smart-auto-handlers ~next 1 ~pattern)
    (setq-array smart-auto-handlers ~next 2 ~handler)

    (setq-array smart-auto-handlers 0 1 (+ 1 ~next))
)

(defun
    (smart-auto-execute
        ~looking ~next ~last ~pattern ~handler

        (setq ~looking 1)

        (beginning-of-file)
        (setq ~next 1)
        (setq ~last (fetch-array smart-auto-handlers 0 1))

        (while (< ~next ~last)
            (if (looking-at (fetch-array smart-auto-handlers ~next 1))
                (progn
                    (setq ~last 0)
                    (setq ~looking 0)
                    (execute-mlisp-line
                        (concat
                            "("
                            (fetch-array smart-auto-handlers ~next 2)
                            ")"))
                )
                (progn
                    (setq ~next (+ 1 ~next))
                )
            )
        )
        (if ~looking
            (normal-mode)
        )
    )
)

(defun
    (smart-auto-XML-HTML-handler
        ; maybe XHTML
        (next-line)
        (if
            (looking-at "<!DOCTYPE html")
            (HTML-mode)
            (looking-at "<html")
            (HTML-mode)
            (XML-mode)
        )
    )
)

; init handler array
(if (= (type-of-expression smart-auto-handlers) "integer")
    (progn
        (setq smart-auto-handlers (array 0 100 1 2))
        ; init the next free
        (setq-array smart-auto-handlers 0 1 1)

        (smart-auto-register-handler "#!.*python" "Python-mode")
        (smart-auto-register-handler "#!.*perl" "sh-mode")
        (smart-auto-register-handler "#!.*[a-zA-Z]*sh" "sh-mode")
        (smart-auto-register-handler "<!DOCTYPE html" "HTML-mode")
        (smart-auto-register-handler "<html" "HTML-mode")
        (smart-auto-register-handler "<\?xml" "smart-auto-XML-HTML-handler")
    )
)
