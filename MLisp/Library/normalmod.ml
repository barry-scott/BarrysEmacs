(defun
    (normal-mode
        (setq left-margin 1)
        (setq right-margin 1000)
        (use-local-map "default-global-keymap")
        (use-abbrev-table "normal-mode")
        (use-syntax-table "normal-mode")
        (setq mode-line-format default-mode-line-format)
        (setq mode-string "Normal")
        (novalue)
    )
)
