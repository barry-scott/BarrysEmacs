; 
; pc-keyboard.ml
; 
; Barry A. Scott (c) 1997-2006
; 
; bind keys on a PC style keyboard to functions that
; match the behaviour defined by Windows interface.
; 

(defun
    (PC-home-key
	(goto-character
	    (save-excursion
		~starting-dot
		(setq ~starting-dot (dot))
		(beginning-of-line)
		(if (looking-at "[ \t][ \t]*")
		    (progn
			(region-around-match 0)
			(if (= ~starting-dot (dot))
			    (beginning-of-line)
			)
		    )		    
		)
		(dot)
	    )
	)	    
    )
)
(defun
    (PC-edit-copy
        ;  only copy if a region is set
        (if (! (error-occurred (mark)))
            (UI-edit-copy)
        )
    )
)
(defun
    (PC-edit-cut
        ;  only cot if a region is set
        (if (! (error-occurred (mark)))
            (progn
                (UI-edit-copy)
                (erase-region)
                (unset-mark)
            )
        )
    )
)

(defun
    (PC-select-all
	(beginning-of-file)
	; Use gui version of set-mark
	(set-mark 1)
	(end-of-file)
    )
)

(defun
    (PC-cut-or-ctrl-x-prefix
	; if mark set then cut
	(if (! (error-occurred (mark)))
	    (PC-edit-cut)
	    (progn
		(push-back-character '^x')
		(if prefix-argument-provided
		    (return-prefix-argument prefix-argument)
		)
	    )
	)
    )
)
(bind-to-key "previous-line" "\[up]")
(bind-to-key "next-line" "\[down]")

(bind-to-key "forward-character" "\[right]")
(bind-to-key "backward-character" "\[left]")

(bind-to-key "scroll-one-line-up" "\[ctrl-up]")
(bind-to-key "scroll-one-line-down" "\[ctrl-down]")

(bind-to-key "forward-word" "\[ctrl-right]")
(bind-to-key "backward-word" "\[ctrl-left]")

(bind-to-key "(setq replace-mode (! replace-mode))" "\[insert]")
(bind-to-key "delete-next-character" "\[delete]")
(bind-to-key "PC-home-key" "\[home]")
(bind-to-key "end-of-line" "\[end]")

(bind-to-key "beginning-of-file" "\[ctrl-home]")
(bind-to-key "end-of-file" "\[ctrl-end]")

(bind-to-key "next-page" "\[page-down]")
(bind-to-key "previous-page" "\[page-up]")

(bind-to-key "end-of-window" "\[ctrl-page-down]")
(bind-to-key "beginning-of-window" "\[ctrl-page-up]")

(bind-to-key "delete-previous-word" "\[ctrl-backspace]")
(bind-to-key "delete-next-word" "\[ctrl-delete]")

; 
; Old Windows cut/copy/paste bindings
; 
(bind-to-key "PC-edit-copy" "\[ctrl-insert]")
(bind-to-key "UI-edit-paste" "\[shift-insert]")
(bind-to-key "PC-edit-cut" "\[shift-delete]")

; 
; Windows/Mac short cut keys for edit functions
; 
(bind-to-key "PC-select-all" "\^a")
(bind-to-key "new-undo" "\^z")
(bind-to-key "PC-edit-copy" "\^c")
(bind-to-key "UI-edit-paste" "\^v")
(bind-to-key "PC-cut-or-ctrl-x-prefix" "\034")
(bind-to-key "PC-cut-or-ctrl-x-prefix" "\035")

; 
; It is expected that the user will swap one of the \034 \035
; with ^x so that ^x is the cut and ^] or ^\ is the ctrl-x prefix
; 

; Function keys

(bind-to-key "help" "\[f1]")
