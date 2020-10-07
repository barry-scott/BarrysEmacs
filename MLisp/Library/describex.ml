(if (error-occurred (setq library-access (getenv "EMACS_LIBRARY_ACCESS")))
    (setq library-access 0))

(extend-database-search-list "describe" "emacs_library:emacsdesc" library-access)

(defun
    (describe-command word
        (setq word (get-tty-command ": describe-command "))
        (save-excursion
            (temp-use-buffer "Help")
            (erase-buffer)
            (if (error-occurred
                    (fetch-database-entry "describe" (concat "C" word))
                )
                (error-message (concat "No help concerning " word))
                (progn old-pop-up-windows
                    (setq old-pop-up-windows pop-up-windows)
                    (setq pop-up-windows 1)
                    (pop-to-buffer "Help")
                    (setq pop-up-windows old-pop-up-windows)
                    (setq buffer-is-modified 0)
                    (beginning-of-file)
                )
            )
        )
        (novalue)
    )

    (describe-variable word
        (setq word (get-tty-variable ": describe-variable "))
        (save-excursion
            (temp-use-buffer "Help")
            (erase-buffer)
            (if (error-occurred
                    (fetch-database-entry "describe" (concat "V" word))
                )
                (error-message (concat "No help concerning " word))
                (progn old-pop-up-windows
                    (setq old-pop-up-windows pop-up-windows)
                    (setq pop-up-windows 1)
                    (pop-to-buffer "Help")
                    (setq pop-up-windows old-pop-up-windows)
                    (setq buffer-is-modified 0)
                    (beginning-of-file)
                )
            )
        )
        (novalue)
    )
)
