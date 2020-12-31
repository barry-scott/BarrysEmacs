(defun
    chmod-current-buffer
    (~mode
        (get-tty-string ": chmod-current-buffer (mode) ")
    )

    ~buffer
    ~file

    (if (= (length ~mode) 0)
        (error-message "mode is blank"))

    (setq ~buffer (current-buffer-name))
    (setq ~file current-buffer-file-name)

    (if (!= current-buffer-type "file")
        (error-message "Cannot chmod a buffer that does not have a file"))

    (if (! (file-exists ~file))
        (progn
            (write-current-file)
            (sit-for 5)
        )
    )

    (save-window-excursion
        (pop-to-buffer "Unix Buffer Commands")
        (end-of-file)
        (insert-string "chmod " ~mode " buffer " ~buffer "\n")
        (set-mark) (filter-region (concat "chmod " ~mode " " ~file))
        (if (= (- (dot) (mark)) 0)
            (progn
                (filter-region (concat "ls -l " ~file))
                (unset-mark)
            )
        )
        (message "Press space to continue")
        (get-tty-character)
        (novalue)
    )
)

(defun
    chown-current-buffer
    (~owner
        (get-tty-string ": chown-current-buffer (owner) ")
    )

    ~buffer
    ~file

    (if (= (length ~owner) 0)
        (error-message "Owner is blank"))

    (setq ~buffer (current-buffer-name))
    (setq ~file current-buffer-file-name)

    (if (!= current-buffer-type "file")
        (error-message "Cannot chown a buffer that does not have a file"))

    (if (! (file-exists ~file))
        (progn
            (write-current-file)
            (sit-for 5)
        )
    )

    (save-window-excursion
        (pop-to-buffer "Unix Buffer Commands")
        (end-of-file)
        (insert-string "chown " ~owner " buffer " ~buffer "\n")
        (set-mark) (filter-region (concat "chown " ~owner " " ~file))
        (if (= (- (dot) (mark)) 0)
            (progn
                (filter-region (concat "ls -l " ~file))
                (unset-mark)
            )
        )
        (message "Press space to continue")
        (get-tty-character)
        (novalue)
    )
)
