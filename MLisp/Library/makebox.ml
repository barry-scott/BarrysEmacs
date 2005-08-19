; 
;	                      makebox.ml    November 2, 1986
;
;   The mlisp will draw a box around text. To use:
;   
;       1. mark a corner of the box using PF1-Select on
;	   a VT220 or esc-x rectangualar-select
;       2. go to any diagonally opposite corner
;       3. esc-x make-box
;   
;   Notes:
;       a) it is based on rectangular cut/paste distributed  with
;          EMACS 4.1.
;       b) the corners must be OUTSIDE the area to be boxed.
;       c) you can select the characters for drawing the box.
;   
;           (setq &h-symbol "-")  ; horizontal symbol
;           (setq &v-symbol "|")  ; vertical symbol
;           (setq &corners  "+")  ; corner symbol
;
;   	d) (use-stars)  will set the symbols to stars (*****).
;	e) (use-lines)  will set the symbols to lines (|---|).
;
;   Comments to ADVAX::HOM	
; 
; Modified 16-jul-1990 by Adrian Pasciuta prior to inclusion in
; EMACS V5.1 EMACSLIB.
; 
; Added "makebox" prefix to variable names and the "use-" function names.
; 
; Variables are now:		
;   makebox-h-symbol
;   makebox-v-symbol
;   makebox-corners
; 
; Changed functions are:
;   makebox-use-lines
;   makebox-use-stars
; 

(progn

(if (! (is-bound rectangular-select))
    (execute-mlisp-file "rectangular")
)

(declare-global
    makebox-h-symbol makebox-v-symbol makebox-corners
)

(setq makebox-h-symbol "-")
(setq makebox-v-symbol "|")
(setq makebox-corners  "+")
      
(defun
    (make-box
	&old-left-column &new-right-column
	(insert-vertical-bars)
	(setq &old-left-column left-column)
	(setq left-column (- right-column 1))
	(goto-right-column) (backward-character)
	(insert-vertical-bars)
	(setq left-column &old-left-column)
	(goto-left-column)
	(forward-character)
	(setq &new-right-column (- right-column 1))
	(while (< (current-column) &new-right-column)
	    (delete-next-character)
	    (insert-string makebox-h-symbol)
	)
	(goto-character start-point)
	(goto-left-column)
	(forward-character)
	(while (< (current-column) &new-right-column)
	    (delete-next-character)
	    (insert-string makebox-h-symbol)
	)
	(goto-left-column)
    )
)

(defun    
    (insert-vertical-bars
	&old-replace-mode 
	(setq &old-replace-mode replace-mode)
	(setq replace-mode 0)
	(set-end-point)
	(save-excursion
	    (next-line)
	    (if (eobp) (newline))
	)
	(check-corners)
	(goto-character start-point)
	(add-white-space)
	(untabify-line)
	(set-mark)
	(goto-character end-point)
	(end-of-line)
	(narrow-region)
	(untabify)				; remove tabs in the region
	(widen-region)			 	; to be cut
	(unset-mark)
	(goto-character start-point)
	(while (<= (dot) end-point)
	    (goto-left-column)
	    (delete-next-character)
	    (insert-string makebox-v-symbol)
	    (next-line)
	    (beginning-of-line)
	)
	(goto-character start-point)
	(goto-left-column)
	(delete-next-character)
	(insert-string makebox-corners)
	(goto-character end-point)
	(goto-left-column)
	(delete-next-character)
	(insert-string makebox-corners)
	(setq replace-mode &old-replace-mode)
    )
)

(defun    
    (makebox-use-lines
	(setq makebox-h-symbol "-")
	(setq makebox-v-symbol "|")
	(setq makebox-corners  "+")
    )
)

(defun    
    (makebox-use-stars
	(setq makebox-h-symbol "*")
	(setq makebox-v-symbol "*")
	(setq makebox-corners  "*")
    )
)

) ; end progn
