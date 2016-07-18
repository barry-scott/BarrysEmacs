(defun
    (~helpchar-compose
        ~helpchar-variable
        (save-window-excursion
            (switch-to-buffer "Help window")
            (erase-buffer)
            (delete-other-windows)
            (fetch-database-entry "MLisp-library" "lk201_compose_chart")
            (beginning-of-file)
            (use-local-map "~helpchar-view-prefix")
            (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
            (setq mode-string (~helpchar-more))
            (recursive-edit)
            (use-local-map "~helpchar-prefix")
        )
    )
)
