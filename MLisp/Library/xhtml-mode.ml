; 
; xhtml-mode.ml
; 
(autoload "~HTML-setup-syntax" "html-mode" )

(defun
    (XHTML-mode
        (setq mode-string "XHTML")
        (use-syntax-table "XHTML")
        (use-local-map "XHTML-keymap")
        (use-abbrev-table "XHTML")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.html"))
        (novalue)
    )
)

(defun
    (~XHTML-setup-keymap
        (temp-use-buffer "~mode-hack")

        (define-keymap "XHTML-keymap")
        (use-local-map "XHTML-keymap")

        (execute-mlisp-file "xhtml-mode.key")

        (kill-buffer "~mode-hack")
        (novalue)
    )
)

(~HTML-setup-syntax "XHTML" "")
(~XHTML-setup-keymap)
