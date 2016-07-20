; 
; xhtml-mode.ml
; 
(autoload "~HTML-setup-syntax" "HTML-mode" )

(defun
    (XHTML-mode
        (setq mode-string "XHTML")
        (use-syntax-table "XHTML")
        (use-local-map "XHTML-map")
        (use-abbrev-table "XHTML")
        (novalue)
    )
)

(defun
    (~XHTML-setup-keymap
        (temp-use-buffer "~mode-hack")

        (define-keymap "XHTML-map")
        (use-local-map "XHTML-map")

        (execute-mlisp-file "xhtml-mode.key")

        (kill-buffer "~mode-hack")
        (novalue)
    )
)

(~HTML-setup-syntax "XHTML" "")
(~XHTML-setup-keymap)
