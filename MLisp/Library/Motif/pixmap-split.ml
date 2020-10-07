(defun
    (pixmap-split
        ~filename
        ~pixmap
        ~index
        ~limit
        ~line

        (setq ~filename current-buffer-file-name)
        (beginning-of-file)
        (set-mark)
        (next-line 3)
        (save-excursion
            (forward-word)
            (set-mark)
            (backward-word)
            (setq ~limit (/ (+ (region-to-string)) 16))
        )
        (end-of-file)
        (setq ~pixmap (region-to-string))

        (setq ~index 0)
        (while (< ~index ~limit)
            (pop-to-buffer "~pixmap-split")
            (erase-buffer)
            (insert-string ~pixmap)
            (beginning-of-file)    (next-line 3)
            (forward-word)
            (delete-previous-word)
            (insert-string "16")
            (end-of-file)
            (previous-line 16)
            (setq ~line 0)
            (while (< ~line 15)
                (setq ~line (+ ~line 1))
                (forward-character)
                (set-mark)
                (forward-character (* ~index 16))
                (erase-region)
                (forward-character 16)
                (set-mark)
                (end-of-line)
                (re-search-reverse "\"")
                (erase-region)
                (beginning-of-line)
                (next-line)
            )
            (write-named-file (concat ~filename "-" ~index))
            (message "Done index " ~index) (sit-for 5)
            (setq ~index (+ ~index 1))
        )
    )
)
