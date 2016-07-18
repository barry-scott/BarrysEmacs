(defun
    (~helpchar-describe
        ~helpchar-variable
        (setq ~helpchar-variable (get-tty-command "Documentation for command? "))
        (save-window-excursion
            (switch-to-buffer "Help window")
            (erase-buffer)

            (delete-other-windows)

            (extend-database-search-list "describe" "emacs_library:emacsdesc")
            (fetch-database-entry "describe"  (concat "C" ~helpchar-variable))

            (use-local-map "~helpchar-view-prefix")
            (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
            (setq mode-string (~helpchar-more))

            (beginning-of-file)
            (recursive-edit)
            (use-local-map "~helpchar-prefix")
        )
    )
)
