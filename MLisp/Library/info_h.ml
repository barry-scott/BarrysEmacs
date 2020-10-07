(defun
    (~info-h
        (if (error-occurred
                (find-node
                    (concat
                        info-default-directory
                        "EMACSIH.TLB"
                    )
                    "help"
                    0
                )
            )
            (error-message "Help is not available.")
        )
    )
)
