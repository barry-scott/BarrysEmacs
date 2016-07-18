(defun
    (show-terminal
        (save-window-excursion
            (switch-to-buffer "~show-term-tmp")
            (setq current-buffer-journalled 0)
            (setq current-buffer-checkpointable 0)
            (delete-other-windows)
            (erase-buffer)
            (insert-string
                "\n\n"
                "\t                       mouse-enable: " mouse-enable "\n"
                "\n"
                "\t        mode-line-graphic-rendition: " mode-line-graphic-rendition "\n"
                "\t highlight-region-graphic-rendition: " highlight-region-graphic-rendition "\n"
                "\t           window-graphic-rendition: " window-graphic-rendition "\n"
                "\t              syntax-word-rendition: " syntax-word-rendition "\n"
                "\t            syntax-string-rendition: " syntax-string-rendition "\n"
                "\t           syntax-comment-rendition: " syntax-comment-rendition "\n"
                "\t          syntax-keyword1-rendition: " syntax-keyword1-rendition "\n"
                "\t          syntax-keyword2-rendition: " syntax-keyword2-rendition "\n"
                "\n"
                "\t                     terminal-width: " terminal-width "\n"
                "\t                    terminal-length: " terminal-length "\n"
                "\t               terminal-cursor-type: " terminal-cursor-type "\n"
            )
            (get-tty-character)
        )
    )
)
