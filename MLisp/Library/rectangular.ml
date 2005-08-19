; 
; 	RECTANGULAR Cut and Paste
;
;       updated:
;	Gim Hom		20-May-1990 	rewrote package to handle journaling
;					by using a temporary buffer.
;
;	Gim Hom		30-Dec-1987	Converted to EMACSV5: used
;					(forward-character repeat-value) ...; 
;					added rectangular-copy
;
;       Gim Hom         18-Nov-1986	Fixed end-of-file bug
;
;       Gim Hom         16-DEC-1985     Improved  performance  by  using
;					(provide-prefix-argument ...  in
;					place of (while ...)
;
;       Gim Hom          3-SEP-1985     Made user-friendly, rectangle is
;                                       now any two opposite corners.
;
;       Gim Hom          2-SEP-1985	Redefined keys.     Fixed so will
;					work in EMACS REPLACE MODE.
;
;  Rectangular cut  and  paste  is modelled after EDD Rectangular Cut and
;  paste.   It  will cut and paste rectangular areas bounded by the upper
;  left  corner  and  the  lower right corner.  It handles tabs and fixes
;  problems in the AREA.ML.
;
;  To handle  all  possible  options, there are two modes for cut and two
;  modes for paste.
;
;  RECTANGULAR-COPY
;   copy the area of text into the buffer "Area"
;
;  RECTANGULAR-CUT
;   replace-with-white-space =>1  replace the deleted area with white space.
;   replace-with-white-space =>0  just delete the area.
;
;  RECTANGULAR-PASTE
;   overstrike-mode =>1  paste in the area in overstrike (replace) mode.
;   overstrike-mode =>0  paste in the area in insert mode.
;
;  To use,   position  the  cursor  to
;  the upper right corner and execute:   (rectangular-select)
;
;  Now position   the  cursor  to  the
;  lower  left  corner   and  execute:   (rectangular-cut)
;
;  To insert  the  text, just position
;  the  cursor  to  the  desired upper
;  left     corner     and    execute:   (rectangular-paste)
;
;  Notes:
;  1.  This  is truly  a rectangular cut and paste.  If the paste area is
;      greater  than the currect file, it will add spaces and lines to so
;      that the full paste area is inserted.
;  2.  The  region to be cut and pasted will have tabs removed and spaces
;      inserted.
;  3.  White  spaces  are  deleted  at the end of the lines in the affect
;      region.
;  4.  Default bindings is the VT200 Select, Remove and Insert keys.
;
;       GOLD SELECT will bind
;	   REMOVE    rectangular-cut with replace-with-white-space = 0
;	   INSERT    rectangular-paste with overstrike-mode = 0
;
;	SELECT will bind
; 	   REMOVE    ~lk201-insert-here
;	   INSERT    ~lk201-remove
;
;	Thus hitting GOLD SELECT will put the REMOVE and INSERT keys
;	in "rectangular CUT/PASTE" mode.
;
;	GOLD REMOVE is permanently bound to
;	   rectanugular-cut with replace-with-white-space = 1.
;	GOLD INSERT is permanently bound to
;	   rectangular-paste with overstrike-mode = 1.
;
;  5.  Because  of  the  intelligent  QIO's  of  EMACS,  when  you  cut a
;      rectangular  area  where  the  upper  right corner is a space, the
;      highlighted  space  may  remain hightlighted after the cut.  Don't
;      worry  ...   Just  do a refresh screen.  At other times the screen
;      may get messed up; again just do a refresh screen.


(declare-global
    right-column		; right column of rectangle
    start-point			; upper left corner
    end-point 			; lower right corner
    left-column			; left column of rectangle
    replace-with-white-space 	; if 1, replace deleted area with space
    overstrike-mode 		; if 1, paste by overstriking (replace mode)
)

(defun
    (rectangular-select
	(set-start-point)
	(mark-start-point)
	(message
           "Rectangular selection started ...  Use redraw-display to cancel."
	)
    )

    (rectangular-copy
	(rc-copy)
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )
    
    (rectangular-cut
	(rc-copy)
	(erase-region)
	(yank-buffer "rc-buffer")
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )
    
    (rectangular-paste area-width line count
	(rc-paste)
	(erase-region)
	(beginning-of-line)
	(yank-buffer "rc-buffer")
;	(delete-buffer "rc-buffer")	; avoid journaling on buffer creation
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )

    (rc-copy  line count
	(set-end-point)
	(save-excursion			; avoid problems on last line
	    (next-line)
	    (if (eobp) (newline))
	)
	(check-corners)
	(goto-character start-point)
	(beginning-of-line)
	(set-mark)
	(goto-character end-point)
	(end-of-line)
	(copy-region-to-buffer "rc-buffer")
	(save-excursion
	    (temp-use-buffer "rc-buffer")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (end-of-file)
	    (setq line (current-line-number))
	    (add-white-space)
	    (save-excursion (temp-use-buffer "Area")
		(setq current-buffer-checkpointable 0)
		(setq current-buffer-journalled 0)
		(erase-buffer)
	    )
	    (untabify)
	    (beginning-of-file)
	    (setq count 0)
	    (while (> line count)
		(goto-left-column)
		(set-mark)
		(goto-right-column)
		(append-region-to-buffer "Area")
		(erase-region)
;		(unset-mark)
		(save-excursion
		    (temp-use-buffer "Area")
		    (newline)
		)
		(if replace-with-white-space
		    (to-col right-column)
		)
		(next-line)
		(setq count (+ count 1))
	    )
	    (beginning-of-file)
	    (goto-left-column)
	)
;	(delete-buffer "rc-buffer")	; avoid journaling on buffer creation
    )

    (rc-paste %rc-string
	(set-start-point)
	(save-excursion
	    (temp-use-buffer "Area")
	    (beginning-of-file)
	    (end-of-line)
	    (setq area-width (current-column))
	    (end-of-file)
	    (setq line (current-line-number))
	    (beginning-of-file)
	)
	(beginning-of-line)
	(set-mark)
	(next-line line)
	(end-of-line)
	(copy-region-to-buffer "rc-buffer")
	(save-excursion
	    (temp-use-buffer "rc-buffer")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (untabify)
	    (beginning-of-file)
	    (setq count 1)
	    (goto-left-column)
	    (while (> line count)
		(save-excursion
		    (temp-use-buffer "Area")
		    (set-mark)
		    (end-of-line)
		    (setq %rc-string (region-to-string))
		    ;		(erase-region)
		    ;		(delete-next-character)
		    (forward-character)
		)
		;	    (untabify-line)
		(if overstrike-mode       ; delete spaces before inserting
		    (delete-column-width)
		)
		(insert-string %rc-string)
		(end-of-line)
		(delete-white-space)
		(if (eobp)
		    (progn
			(newline)
			(to-col left-column)
		    )
		    (progn
			(next-line)
			(goto-left-column)
		    )
		)
		(setq count (+ count 1))
	    )
	)
    )
    

;
;	supporting routines; listed in alphabetical order.
;

    (add-white-space		; add white space to form a rectangle
	(beginning-of-file)
	(setq count 0)
	(while (> line count)
	    (end-of-line)
	    (if (< (current-column) right-column)
		(to-col right-column)
	    )
	    (next-line)
	    (setq count (+ count 1))
	)
    )

    (check-corners new-right-column new-end-point
	(if (& (> (- right-column 1) left-column) (< end-point start-point))
	    			; reverse start/end; columns okay
	    (progn
		(setq new-end-point start-point)
		(setq start-point end-point)
		(setq end-point new-end-point)
		(goto-character start-point)
		(untabify-line)
		(goto-left-column)
		(setq start-point (dot))
		(goto-character end-point)
		(end-of-line)
		(if (< (current-column) right-column)
		    (to-col right-column)
		)
		(untabify-line)
		(beginning-of-line)
		(forward-character (- right-column 1))
		(setq end-point (dot))
	    )
	)
	(if (& (< (- right-column 1) left-column) (> end-point start-point))
	    			; start/end  ok, reverse columns
	    (progn
		(goto-character start-point)
		(untabify-line)
		(setq new-right-column left-column)
		(setq left-column (- right-column 1))
		(setq right-column (+ new-right-column 1))
		(goto-left-column)
		(setq start-point (dot))
		(goto-character end-point)
		(end-of-line)
		(if (< (current-column) right-column)
		    (to-col right-column)
		)
		(untabify-line)
		(beginning-of-line)
		(forward-character (- right-column 1))
		(setq end-point (dot))
	    )
	)
	(if (& (< end-point start-point) (<= (- right-column 1) left-column))
				; reverse start/end; reverse columns
	    (progn
		(setq new-end-point start-point)
		(setq start-point end-point)
		(setq end-point new-end-point)
		(setq new-right-column left-column)
		(setq left-column (- right-column 1))
		(setq right-column (+ new-right-column 1))
	    )
	)
    )

    (delete-column-width temp-counter	; delete width of area
	(set-mark)                      ; must not contain any tabs
	(end-of-line)
	(if (>= (current-column)  (+ left-column area-width))
	    (progn
		(goto-character (mark))
	        (forward-character (- area-width 1))
	    )
	)
	(erase-region)
;	(unset-mark)
    )

    (goto-left-column			; go to the left side of the area
	(end-of-line)			; must not contain any tabs
	(if (< (current-column) left-column)
	    (to-col  left-column)
	    (progn
		(beginning-of-line)
		(forward-character (- left-column 1))
	    )
	)
    )

    (goto-right-column		; go to the right side of the area
        (forward-character (- right-column (current-column)))
    )

    (mark-start-point 		; this will highlight the start point
	(send-string-to-terminal (concat "\e[7m"
              (char-to-string (following-char)) "\e[0m" )
	)
    )

    (set-start-point
	(if (eobp)
	    (progn
		(insert-string " \n")
		(backward-character)
		(backward-character)
	    )
	)
	(setq start-point (dot))
	(setq left-column (current-column))
    )
    
    (set-end-point
	(if (eobp)
	    (progn
		(insert-string " \n")
		(backward-character)
		(backward-character)
	    )
	)
	(setq end-point (dot))
	(setq right-column (+ (current-column) 1))
    )

    (untabify	; remove tabs, and put in correct number of spaces.
	col
	(beginning-of-file)
	(while (! (error-occurred (search-forward "\t")))
	    (progn
		(setq col (current-column))
		(delete-previous-character)
		(while (< (current-column) col)
		    (insert-string " "))
	    )
	)
    )

    (untabify-line  col
	(save-restriction
	    (beginning-of-line) (set-mark) (end-of-line) (narrow-region)
	    (beginning-of-line)
	    (while (! (error-occurred (search-forward "\t")))
		(progn (setq col (current-column))
		    (delete-previous-character)
		    (while (< (current-column) col)
			(insert-string " "))
		)
	    )
	    (unset-mark)
	    (widen-region)
	)
    )

)
(execute-mlisp-file "rectangular.key")
