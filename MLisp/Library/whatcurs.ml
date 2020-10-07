;function to show the cursor position in relation to the window
; 9-FEB-1985  cursor postion relative to top of page added by G Hom
;[***] el:whatcurs.=2, Edit by _VAX4::Prentice
;editno=2                                                       -*-mlisp-*-
; Barry A. Scott         2-Jul-1983
; convert to v3 emacs
(defun
    (what-cursor-position line dot char col beg end pline
        (save-excursion
            (setq dot (dot))
            (if (eobp)
                (setq char "eob")
                (setq char (following-char))
            )
            (end-of-window)
            (setq end (dot))
            (beginning-of-window)
            (setq beg (dot))
            (setq line 0)
            (setq pline 0)
            (while (& (<= (dot) dot) (! (eobp)))
                (next-line)
                (setq line (+ line 1)))
            (goto-character dot)
            (if (error-occurred (search-reverse "\014"))
                (beginning-of-file))
            (while (& (<= (dot) dot) (! (eobp)))
                (next-line)
                (setq pline (+ pline 1))
            )
        )
        (if (eobp) (setq line (+ line 1)))
        (if (eobp) (setq pline (+ pline 1)))
        (message
            "X=" (current-column)
            " Y=" line
            " CH=" char
            " .=" (+ 0 dot)
            "(" (if (> (buffer-size) 0)
                    (/ (* dot 100) (buffer-size))
                    0)
            "% of " (buffer-size)
            ")  W=<" (+ 0 beg) "," (+ 0 end) ">"
            "  Line " pline " from top of page."
        )
        (novalue)
    )
)
