(defun
    (next-file-buffer
        ~index
        ~limit
        ~buffer-name
        ~looping

        (setq ~limit (fetch-array buffer-names 0))

        (setq ~index 1)
        (setq ~looping 1)
        (while ~looping
            (setq ~buffer-name (fetch-array buffer-names ~index))
            (if (= current-buffer-name ~buffer-name)
                (setq ~looping 0)
                (setq ~index (+ ~index 1))
            )
        )
        (setq ~looping 1)
        (while ~looping
            (if (< ~index ~limit)
                (setq ~index (+ ~index 1))
                (setq ~index 1)
            )
            (setq ~buffer-name (fetch-array buffer-names ~index))
            (if (= (save-excursion (temp-use-buffer ~buffer-name) current-buffer-type) "file")
                (setq ~looping 0)
            )
            (if (= current-buffer-name ~buffer-name)
                (error-message "next-file-buffer - no next file buffers")
            )
        )
        (switch-to-buffer ~buffer-name)
    )
)

(defun
    (previous-file-buffer
        ~index
        ~limit
        ~buffer-name
        ~looping

        (setq ~limit (fetch-array buffer-names 0))

        (setq ~index 1)
        (setq ~looping 1)
        (while ~looping
            (setq ~buffer-name (fetch-array buffer-names ~index))
            (if (= current-buffer-name ~buffer-name)
                (setq ~looping 0)
                (setq ~index (+ ~index 1))
            )
        )
        (setq ~looping 1)
        (while ~looping
            (if (> ~index 1)
                (setq ~index (- ~index 1))
                (setq ~index ~limit)
            )
            (setq ~buffer-name (fetch-array buffer-names ~index))
            (if (= (save-excursion (temp-use-buffer ~buffer-name) current-buffer-type) "file")
                (setq ~looping 0)
            )
            (if (= current-buffer-name ~buffer-name)
                (error-message "previous-file-buffer - no previous file buffers")
            )
        )
        (switch-to-buffer ~buffer-name)
    )
)
