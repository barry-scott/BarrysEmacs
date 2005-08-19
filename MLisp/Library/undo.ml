; 
; undo.ml
; 
; Barry A. Scott (c) 1997
; 
(defun
    (new-undo
	~c
	
	~cont-char
	
	~message
	
	(if (= (length last-keys-struck) 1)
	    (progn
		(setq ~c (string-to-char last-keys-struck))
		(if (< ~c ' ')
		    (setq ~message (concat "Press <Ctrl-" (char-to-string (+ ~c '@')) "> or <space> to undo more"))
		    (setq ~message (concat "Press <" last-keys-struck "> or <space> to undo more"))
		)
		(setq ~cont-char (string-to-char last-keys-struck))
	    )
	    (progn
		(setq ~message "Press <space> to undo more")
		(setq ~cont-char ' ')
	    )
	)

	(undo)

	(while 
	    (progn 
		(message ~message)
		(setq ~c (get-tty-character))
		(| (= ~c ~cont-char) (= ~c ' '))
	    )
	    (undo-boundary)
	    (undo-more)
	)

	(message "Finished undoing.")

	(if (&
		(!= ~c '\n')
		(!= ~c '\r')
	    )
	    (push-back-character ~c)
	)
    )
)
