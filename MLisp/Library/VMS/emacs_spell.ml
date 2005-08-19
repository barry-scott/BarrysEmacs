(progn
    (error-occurred (external-function "emacs$spell" "emacs$share:emacs_spell_shr"))

    (if (! (is-bound ~spell-init))
        (progn
	    (declare-global
		~spell-init
	    )
	    (setq ~spell-init 0)
	)
    )
    (if (! (is-bound spell-master-dictionary))
        (progn
	    (declare-global spell-master-dictionary)
	    (setq spell-master-dictionary "lngsplaed")
	)
    )
    (if (! (is-bound spell-ram-dictionary))
        (progn
	    (declare-global spell-ram-dictionary)
	    (setq spell-ram-dictionary "lngsplaem")
	)
    )
    (if (! (is-bound spell-memory-size))
        (progn
	    (declare-global spell-memory-size)
	    (setq spell-memory-size 4096)
	)
    )
    (if (! (is-bound spell-personal-dictionary))
        (progn
	    (declare-global spell-personal-dictionary)
	    (setq spell-personal-dictionary "personal")
	)
    )
(defun correct-word (cw-word (get-tty-string ": correct-word "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (emacs$spell 8 cw-word)
)
(defun spell-check-word (cw-word (get-tty-string ": spell-check-word "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (emacs$spell 7 cw-word)
)
(defun get-tty-correction
  (cw-word (get-tty-string ": get-tty-correction (word) ")
  cw-prompt (get-tty-string ": get-tty-correction (prompt) "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (expand-from-string-table cw-prompt (emacs$spell 8 cw-word))
)
)
