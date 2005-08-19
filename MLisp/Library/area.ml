(progn
;	+----------------+----------------------------------------------+
;	| File Name:	 | 						|
;	| Edit Version:	 | 12						|
;	| Date Modified: | Wed Jun 22 13:18:10 1983			|
;	| Updated By:	 | SURTEES					|
;	| From Node:	 | _SIVA::					|
;	+----------------+----------------------------------------------+
;
; These routines were written to allow areas to be moved from one area
; another.  I have delete-area bound to ESC-w and insert-area to ESC-y.
;
; What delete-area effectively does is find the top-left corner and
; bottom-right corner of an area between mark and current cursor position
; and deletes it.  If the left-column and right-column are the same, then
; the right-hand column is assumed to be end-of-line for each line.
;
; The contents of this area are placed in a buffer called "Area" where they
; can be retreived with the insert-area command.  As the area is inserted,
; the trailing white space will be deleted if it at the end of the line,
; except on the last line of inserted text where white space is added.
; The reason for this is so that the mark can be set which will allow a
; delete-area to delete what was just inserted.
;
; If insert-area is given an argument of 4 (^U), then the lines in the Area
; buffer will be cycled into the buffer between dot and mark.
;
; Be careful with tabs as they may cause some strange results.
; 
;
(defun
    (delete-area		; Delete a two-dimensional area
	top-left		; Top left corner position	  
	top-line         	; Position of 1st character in top line
	bottom-line		; Position of 1st character in bottom line
	left-column		; Column number of left side of area
	right-column		; Column number of right side of area + 1
	number-lines		; Number of lines to be affected 
	line-counter		; Temporary variable for counting lines
	(progn
	    (setq bottom-line (dot))
	    (setq right-column (current-column))
	    (exchange-dot-and-mark)
	    (setq top-line (dot))
	    (setq left-column (current-column))
	    (if (< bottom-line top-line); Check to see that bottom > top
		(progn
		    (setq bottom-line (dot))
		    (setq right-column (current-column))
		    (exchange-dot-and-mark)
		    (setq top-line (dot))
		    (setq left-column (current-column))))
	    (if (< right-column left-column); Check that right > left
		(progn temp
		    (setq temp left-column)
		    (setq left-column right-column)
		    (setq right-column temp)))
	    (goto-character bottom-line); Set bottom-line to 1st character
            (if (eobp) (newline))
	    (beginning-of-line)
	    (setq bottom-line (dot))
	    (goto-character top-line)
	    (beginning-of-line)	; Set top-line to 1st character in line
	    (setq top-line (dot))
	    (while (< (current-column) left-column); Set top-left to real top
		(forward-character))
	    (setq top-left (dot))
	    (setq number-lines 1)
	    (while (< (dot) bottom-line); Count the number of lines in the
		; area to be affected
		(next-line)
		(setq number-lines (+ number-lines 1)))
	    (goto-character top-line)
	    (setq line-counter 0)
	    (set-mark)
	    (copy-region-to-buffer "Area"); Clear out Area buffer
	    (while (< line-counter number-lines); This is the main loop
		; which will delete the area from the current buffer
		; and place it in "Area" buffer
		(setq line-counter (+ line-counter 1))
		(set-mark)
		(next-line)
		(append-region-to-buffer "Area")
		(delete-to-killbuffer)
		(save-excursion
		    (temp-use-buffer "Area")
		    (to-col left-column); Go to left side of area
		    (previous-line)
		    (set-mark)
		    (beginning-of-line)
		    (delete-region-to-buffer "Hack buffer")
		    (next-line)
		    (to-col (+ (- right-column left-column) 1)); Right side
		    (previous-line)
		    (if (= left-column right-column) (end-of-line))
		    (set-mark)
		    (end-of-line)
		    (append-region-to-buffer "Hack buffer")
		    (delete-to-killbuffer); Also delete it from the buffer
		    (next-line)
		    (beginning-of-line)
		    (kill-to-end-of-line)); Kill the original line
		(yank-buffer "Hack buffer"); and replace it with the new one
		(newline)))	; Add an end-of-line character
	(goto-character top-left)
	(set-mark)
	(if (> number-lines 1)
	    (provide-prefix-argument (- number-lines 1) (next-line)))
	(exchange-dot-and-mark)
	(novalue))
    
    (insert-area
	top-line         	; Top left corner position
	bottom-line		; Position of 1st character in bottom line
	left-column		; Column number of left side of area
	right-column		; Column number of right side of area + 1
	number-lines		; Number of lines to be affected
	line-counter		; Temporary variable for counting lines
	replicate		; Equal to 1 if there is one line to insert
	(save-excursion
	    (setq top-line (dot))
	    (setq left-column (current-column))
	    (save-excursion
		(temp-use-buffer "Area")
		(beginning-of-file)
		(setq line-counter 0)
		(while (! (eobp))	; Count the number of lines
		    (setq line-counter (+ line-counter 1))
		    (end-of-line)
		    (if (> (current-column) right-column); Find right margin
			(setq right-column (current-column)))
		    (next-line)
		    (beginning-of-line))
		(setq number-lines line-counter))
	    (if (= prefix-argument 4)
		(progn
		    (if (> (dot)
			    (progn
				(exchange-dot-and-mark)
				(dot)))
			(exchange-dot-and-mark))
		    (setq bottom-line (dot))
		    (exchange-dot-and-mark)
		    (setq top-line (dot))
		    (setq line-counter 1)
		    (while (< (dot) bottom-line)
			(next-line)
			(setq line-counter (+ line-counter 1)))
		    (setq number-lines line-counter))))
	(progn
	    (setq line-counter 0)
	    (save-excursion
		(temp-use-buffer "Area")
		(beginning-of-file))
	    (while (< line-counter number-lines)
		(goto-character top-line)
		(if (> line-counter 0)
		    (provide-prefix-argument line-counter (next-line)))
		(if (eobp) (newline-and-backup))
		(to-col left-column)
		(insert-string
		    (save-excursion
			(temp-use-buffer "Area")
			(if (eobp)
			    (beginning-of-file))
			(set-mark)
			(next-line)
			(region-to-string)))
		(delete-previous-character); Get rid of eol
		(delete-white-space)
		(if(! (eolp))
		    (to-col (- (+ left-column right-column) 1)))
		(setq line-counter (+ line-counter 1))
		(if (= 0 (- line-counter (* (/ line-counter 22) 22)))
		    (sit-for 0)))
	    (if (!= prefix-argument 4)
		(to-col (- (+ left-column right-column) 1)))
	    (set-mark)
	    (goto-character top-line)
	    (exchange-dot-and-mark)
	    (novalue))))
)
