(defun
    (bug-ere-case-1
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "b 2019-04 a\n")
        (beginning-of-file)
        (ere-search-forward "\\d{4}-\\d+ ")
        (beginning-of-file)
        (ere-search-forward "(x|\\d{4}-\\d+) ")
    )
)
(defun
    (bug-ere-case-1b
        ~s
        (setq ~s "b 2019-04 a\n")
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string ~s)

        (message "buffer: " ~s)
        (beginning-of-file)
        (ere-search-forward "\\d{4}-")

        (message "buffer: " ~s)
        (beginning-of-file)
        (ere-search-forward "(x|\\d{4})-")
    )
)
(defun
    (bug-ere-case-2
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "preamble\n"
            "match 1 xxx more\n")
        (beginning-of-file)
        (ere-search-forward "(xxx|\\d{4}-\\d+-\\d+) ")
    )
)
(defun
    (bug-ere-case-3
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "preamble\n"
            "match 1 2019-04-07 more\n")
        (beginning-of-file)
        (ere-search-forward "(\\d{4}-\\d+-\\d+|xxx) ")
    )
)
(defun
    (bug-ere-case-4
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "preamble\n"
            "match 1 xxx more\n")
        (beginning-of-file)
        (ere-search-forward "(\\d{4}-\\d+-\\d+|xxx) ")
    )
)
(defun
    (bug-ere-case-5
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "preamble\n"
            "match 5 2019-04-07 more\n")
        (beginning-of-file)
        (ere-search-forward "(\\d{4}-\\d+-\\d+|xxx)")
    )
)
(defun
    (bug-ere-case-6
        (pop-to-buffer "bug")
        (erase-buffer)
        (insert-string
            "preamble\n"
            "match 5 2019-04-07 more\n")
        (beginning-of-file)
        (ere-search-forward "(\\d{4}-\\d+-\\d+|xxx)")
    )
)
