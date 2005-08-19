; 
; windows.ml
;   Copyright (c) 1985 Barry A. Scott
; 
; this package implements windows rings.
; 
(progn
(defun
    (add-windows-to-ring
	(setq windows-ring-position (+ 1 windows-ring-position))
	(setq windows-ring-used (+ 1 windows-ring-used))
	; 
	; do we need to extend the array?
	; 
	(if (>  windows-ring-used (fetch-array windows-ring 0))
	    (progn
		new-array size i
		(setq size (fetch-array windows-ring 0))
		(setq new-array (array 0 (+ size 10)))
		(setq i 0)
		(while (<= i size)
		    (setq-array new-array i (fetch-array windows-ring i))
		    (setq i (+ i 1))
		)
		(setq-array new-array 0 (+ size 10))
		(setq windows-ring new-array)
	    )
	)
	(if (= windows-ring-position windows-ring-used)
	    (progn
		; 
		; just add to the end of the array
		; 
		(setq-array windows-ring windows-ring-position current-windows)
	    )
	    (progn
		; 
		; need to slide later enties up array
		; 
		i
		(setq i windows-ring-used)
		(while (> i windows-ring-position)
		    (setq-array windows-ring i
			(fetch-array windows-ring (- i 1)))
		    (setq i (- i 1))
		)
		(setq-array windows-ring windows-ring-position current-windows)
	    )
	)
	(message "Current windows added to ring at position "
	    windows-ring-position)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (remove-windows-from-ring
	i
	(~windows-current-in-ring)
	(setq i windows-ring-position)
	(while (< i windows-ring-used)
	    (setq-array windows-ring i (fetch-array windows-ring (+ i 1)))
	    (setq i (+ i 1))
	)
	(setq windows-ring-used (- windows-ring-used 1))
	(message "Removed windows from ring at position "
	    windows-ring-position)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (current-windows-in-ring
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (~windows-current-in-ring
	(if (= 0 windows-ring-used)
	    (error-message "You have not saved any windows yet.")
	)
	(if (> windows-ring-position windows-ring-used)
	    (setq windows-ring-position 1)
	)
	(if (< windows-ring-position 1)
	    (setq windows-ring-position windows-ring-used)
	)
	(setq current-windows
	    (fetch-array windows-ring windows-ring-position)
	)
	(message "At position " windows-ring-position " in windows ring.")
	(novalue)
    )
)
(defun
    (goto-windows-in-ring
	(setq windows-ring-position (+ (arg 1 "Goto windows ring position: ")))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (next-windows-in-ring
	(setq windows-ring-position (+ windows-ring-position 1))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (previous-windows-in-ring
	(setq windows-ring-position (- windows-ring-position 1))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(define-keymap "windows-keymap")
(declare-global
    windows-ring			; the ring of windows
    windows-ring-position		; current position in the ring
    windows-ring-used			; number of windows in the ring
    windows-cycle			; true if next and previous cycle
					; through the windows
)
(setq windows-ring (array 0 10))	; array of 11 entries 
(setq-array windows-ring 0 10)		; entry zero is size of ring
(setq windows-ring-position 0)		; ring is empty
(setq windows-ring-used 0)		; amount of space used in the ring
    
(execute-mlisp-file "windows.key")
)
