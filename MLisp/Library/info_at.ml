(defun
        (~info-@
            path
            menu-part
            (save-window-excursion
                (setq path (get-tty-string "Path: "))
                (switch-to-buffer "Hack buffer")
                (erase-buffer)
                (insert-string (concat path " "))
                (beginning-of-file)
                (set-mark)
                (while (! (error-occurred (re-search-forward "  *")))
                    (delete-white-space)
                    (setq menu-part (region-to-string))
                    (erase-region)
                    (set-mark)
                    (save-excursion
                        (switch-to-buffer "INFO")
                        (info-goto-menu-item menu-part)
                    )
                )
            )
        )
)
