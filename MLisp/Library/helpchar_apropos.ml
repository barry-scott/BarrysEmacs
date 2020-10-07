(defun
    (~helpchar-apropos
        ~helpchar-variable
        (setq ~helpchar-variable (get-tty-string "Documentation for word? "))
        (save-window-excursion
            (switch-to-buffer "Help window")
            (erase-buffer)

            (apropos ~helpchar-variable)
            (yank-buffer "Help")
            (delete-buffer "Help")

            (beginning-of-file)
            (use-local-map "~helpchar-view-prefix")
            (setq mode-string (~helpchar-more))
            (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
            (recursive-edit)
            (use-local-map "~helpchar-prefix")
        )
    )
)
