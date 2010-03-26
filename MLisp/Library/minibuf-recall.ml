(progn ; minibuf-recall.ml - patch the minibuf keymaps

; Patch the minibuf keymaps to allow all VT100 arrow and keypad keys
; to work.  Note that you may no longer use ESC to do command recognition.
; Use ESC-ESC instead.

; 
; AFter loading this package the minibuffer key maps have been modified
; to allow command recall in the minibuffer.
; 
; To all the VT100 Key pad keys and expansion keys execute
; minibuf-recall-kpx-keymaps. The binding for the minibuffer
; are read in from minibuf-recall.key.

; You also get a command recall facility on ^R

; Things to do:
; - Catch current buffer before replacing it
;
; 	15-SEP-1987 Daryl Gleason
; 	If current line to be stored is a superset of the last line
; 	stored, store current line over last line.  Also add mini-left
;	and mini-right to avoid messy errors in the minibuffer.
; 
;	12-sep-1987 Barry A. Scott
;	Add to Emacs kit
; 
;	1-may-1986 J.A.Lomicka Don't save null strings in recall ring
;	Edited 24-April-1986 to add command recall	J.A.Lomicka
;	Original code 15-Oct-1984 by Roy Lomicka
;
(declare-global
  mini-recall-buffer;	Array where commands are stored
  mini-recall-index;	Integer where commands go in
  mini-recall-point);	Place where commands come out
(setq mini-recall-buffer (array 0 63))

(declare-buffer-specific ~tmp)
(setq ~tmp 0)
(while (< ~tmp 64)
    (setq-array mini-recall-buffer ~tmp "")
    (setq ~tmp (+ ~tmp 1))
)

(defun
    (mini-newline
	; If you exit from an enlarged minibuf at arg level, it will stay
	; enlarged.  You'll have to get into another arg level minibuf and
	; make the window smaller, then exit from it to reverse the effect.
	; At command level, the enlargement doesn't seem to be so permanent.
	(error-occurred (enlarge-window))
	(newline)
    )
)

(defun
    (mini-exit-handler ~cur ~last
	; store string in recall buffer
	(beginning-of-file)(set-mark)(end-of-file)
	(setq ~cur (region-to-string))			; current string
	(setq						; last string
	    ~last
	    (fetch-array
		mini-recall-buffer (& 63 (- mini-recall-index 1))
	    )
	)
	(if (length ~cur)	; if cur string not null
	    (if
		(&
		    (>=		; if length of cur string >= last string
			(length ~cur)
			(length ~last)
		    )
		    (=		; & substr of cur string = last string
			(substr ~cur 0 (length ~last))
			~last
		    )
		)
		; then store the current string over the last string
		(setq-array
		    mini-recall-buffer
		    (& 63 (- mini-recall-index 1))
		    ~cur
		)
		; else store current string at next slot and inc pointers
		(progn
		    (setq-array
			mini-recall-buffer
			mini-recall-index
			~cur
		    )
		    (setq mini-recall-index (& 63 (+ mini-recall-index 1)))
		)
	    )
	)
	(setq mini-recall-point mini-recall-index)
	; undo any enlargement before calling expand-and-exit
	(setq window-size 1)
    )
)

(defun
    (mini-error-and-exit
	(setq mini-recall-point mini-recall-index)
	; undo any enlargement before calling error-and-exit
	(setq window-size 1)
	(error-and-exit)
    )
)

(defun
    (mini-expand-and-exit
	(error-occurred (mini-exit-handler))
	(expand-and-exit)
    )
)

(defun
    (mini-exit
	(error-occurred (mini-exit-handler))
	(exit-emacs)
    )
)

(defun
    (mini-recall ~string ~start-point ~move-by
	(setq-array mini-recall-buffer mini-recall-point (region-to-string))
	(error-occurred (erase-region))
	(set-mark)
	(setq ~start-point mini-recall-point)
	(setq ~move-by prefix-argument)
	(while
	    (progn
		(setq mini-recall-point (& 63 (- mini-recall-point ~move-by)))
		(setq ~string (fetch-array mini-recall-buffer mini-recall-point))
		(& (= (length ~string) 0) (!= mini-recall-point ~start-point))
	    )
	    (setq ~move-by (if (< ~move-by 0) -1 1))
	)
	(insert-string ~string)
    )
)

(defun
    (mini-recall-selection ~string ~start-point
	(setq-array mini-recall-buffer mini-recall-point (region-to-string))
	(error-occurred (erase-region))
	(set-mark)
	(setq ~start-point mini-recall-point)
	(if
	    (save-window-excursion
		(pop-to-buffer "Recall list")
		(local-bind-to-key "exit-emacs" "\^m")
		(erase-buffer)
		(unset-mark)
		(insert-string "Select the line to recall and press RETURN\n")
		(while
		    (progn
			(setq mini-recall-point (& 63 (- mini-recall-point 1)))
			(setq ~string (fetch-array mini-recall-buffer mini-recall-point))
			(!= mini-recall-point ~start-point)
		    )
		    (if (!= (length ~string) 0)
			(insert-string ~string "\n")
		    )
		)
		(beginning-of-file)
		(next-line)
		(beginning-of-line)
		(recursive-edit)
		(beginning-of-line)
		(if (! (bobp))
		    (progn
			(set-mark)
			(end-of-line)
			(setq ~string (region-to-string))
			1
		    )
		    0
		)
	    )
	    (insert-string ~string)
	)
    )
)

(defun
    (mini-recall-reverse
	(provide-prefix-argument (- prefix-argument) (mini-recall))
    )
)


(defun
    (mini-left
	(error-occurred (backward-character prefix-argument))
    )
)

(defun
    (mini-right
	(error-occurred (forward-character prefix-argument))
    )
)

(defun
    (~minipatch-kpx
        (novalue)
        ; what needs to bound in all keymaps?
    )
)
    
(defun
    (minibuf-recall-kpx-keymaps
	(patch-minibuf-keymaps)
	(use-global-map "Minibuf-local-map")
	(~minipatch-kpx)
	(use-global-map "Minibuf-local-NS-map")
	(~minipatch-kpx)
	(use-global-map "default-global-keymap")
    )
)
    
; 
; set up the minibuf keymaps and set
; global keymap to "default-global-keymap"
; 
(use-global-map "Minibuf-local-map")
(execute-mlisp-file "minibuf-recall.key")
(use-global-map "Minibuf-local-NS-map")
(execute-mlisp-file "minibuf-recall.key")
(bind-to-key "mini-expand-and-exit" " ")
(bind-to-key "mini-expand-and-exit" "\t")
(use-global-map "default-global-keymap")

) ; end of 1st progn
