(defun
    (~info-m
        ~s-pos
        (if (setq ~s-pos (info-check-menu))
            (progn
                (save-window-excursion
                    (if (> (+ ~s-pos 0) (+ 0 (dot)))
                        (progn
                            (goto-character ~s-pos)
                            (if (! (dot-is-visible))
                                (error-occurred
                                    (line-to-top-of-window)
                                    (provide-prefix-argument 3 (scroll-one-line-down))
                                )
                            )
                        )
                    )
                    (setq ~s-pos (get-tty-string "Menu item: "))
                )
                (info-goto-menu-item ~s-pos)
            )
            (error-message "This node has no menu.")
        )
    )
)
