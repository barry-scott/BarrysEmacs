;
; tabs.ml
;       a package to do smart things with the tabs
;
;       It uses the variable logical-tab-size to compute the
;       text to insert or delete.
;
; tab-key
;       this function inserts tabs and space to move to the next tab stop
; tab-delete
;       this function deletes enough tabs and spaces to move back a tab stop
; replace-tabs-with-spaces-in-buffer
;       this function replaces every tab in the buffer with the equilivent
;       number of spaces
; resize-tabs-in-buffer
;       this function changes the tab size in a buffer thus changing the
;       the indentation from the old tab size to the new tab size
;
(declare-global visual-tab-updates)
(setq visual-tab-updates 1)

(defun
    (tab-key
        (if (= (current-buffer-name) "Minibuf")
            (insert-string "\t")
            (& tab-indents-region (! (error-occurred (mark))))
            (indent-region)
            (progn
                ~current-column

                (setq ~current-column (current-column))
                (insert-character '.') (backward-character)
                (delete-white-space) (delete-next-character)
                (to-col
                    (+
                        (*
                            (/
                                (+ ~current-column logical-tab-size -1)
                                logical-tab-size
                            )
                            logical-tab-size
                        )
                        1
                    )
                )
            )
        )
    )
)

(defun
    (tab-delete
        (if (& tab-indents-region (! (error-occurred (mark))))
            (undent-region)
            (progn
                ~current-column

                (setq ~current-column (current-column))
                (insert-character '.') (backward-character)
                (delete-white-space) (delete-next-character)
                (to-col
                    (+
                        (*
                            (/
                                (- ~current-column 2)
                                logical-tab-size
                            )
                            logical-tab-size
                        )
                        1
                    )
                )
            )
        )
    )
)

(defun
    (replace-tabs-with-spaces-in-buffer
        ~old-column

        (beginning-of-file)
        (while
            (! (error-occurred
                   (re-search-forward "\t\t*")
               ))
            (region-around-match 0)
            (setq ~old-column (current-column))
            (erase-region)
            (while (< (current-column) ~old-column)
                (insert-string " ")
            )
            (if visual-tab-updates (sit-for 0))
            (unset-mark)
        )
    )
)


(defun resize-tabs-in-buffer
    (
        ~old-tab-size (get-tty-string "Old tab-size: ")
        ~new-tab-size (get-tty-string "New tab-size: ")
    )
    ~old-column ~old-tab-stops
    ~new-column

    (beginning-of-file)
    (while
        (! (error-occurred
               (re-search-forward "^[\t ][\t ]*")
           ))
        (region-around-match 0)
        (setq ~old-column (- (current-column) 1))
        (setq ~old-tab-stops (/ ~old-column ~old-tab-size))
        (setq ~old-column (- ~old-column (* ~old-tab-stops ~old-tab-size)))

        (setq ~new-column (+ 1 ~old-column (* ~old-tab-stops ~new-tab-size)))

        ;       (get-tty-string (concat "Old( " ~old-tab-stops ", " ~old-column ") New( " ~new-column ") " ))
        (if visual-tab-updates (sit-for 0))

        (erase-region)
        (unset-mark)
        (to-col ~new-column)
    )
)
