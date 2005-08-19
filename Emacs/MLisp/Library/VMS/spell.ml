(progn
    (external-function "emacs$spell" "emacs$share:emacs_spell_shr")

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
    (if (! (is-bound spell-line-exit-on-action))
        (progn
	    (declare-global
		spell-line-exit-on-action
	    )
	    (setq spell-line-exit-on-action 0)
	)
    )
    (declare-global ~spell-in-spell-line)
    (defun
	(spell-region
	    end
	    word
	    saved-mode-line
	    saved-syntax-table
	    old-in-spell-line
	    (setq old-in-spell-line ~spell-in-spell-line)
	    (setq ~spell-in-spell-line 1)
	    (mark)
	    (if (interactive)
		(progn
		    (message "Spell checking region...")
		    (sit-for 0)
		)
	    )
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(if (< (+ (mark) 0) (+ (dot) 0))
		    (exchange-dot-and-mark)
		)
		(setq end (+ (mark) 0))
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(forward-word)
		(while (& (! (eobp)) (<= (+ 0 (dot)) end))
		    (set-mark)
		    (backward-word)
		    (use-syntax-table saved-syntax-table)
		    (exchange-dot-and-mark)
		    (if (! (emacs$spell 7 (setq word (region-to-string))))
			(progn
			    (message "I do not understand `"
				word
			 	"'. Please correct it and press ^C to continue")
			    (setq saved-mode-line mode-line-format)
			    (setq mode-line-format
				(concat
				    " Correcting `"
				    word
				    "'. Press ^C to continue "))
			    (recursive-edit)
			    (setq mode-line-format saved-mode-line)
			)
		    )
		    (use-syntax-table "spell")
		    (forward-word)
		)
		(use-syntax-table saved-syntax-table)
	    )
	    (if (interactive)
		(message "Spell checking region... Done.")
	    )
	    (setq ~spell-in-spell-line old-in-spell-line)
	    (novalue)
	)
	(spell-buffer
	    (if (interactive)
		(progn
		    (message "Spell checking buffer...")
		    (sit-for 0)
		)
	    )
	    (save-window-excursion
		(beginning-of-file)
		(set-mark)
		(end-of-file)
		(spell-region)
	    )
	    (if (interactive)
		(message "Spell checking buffer... Done.")
	    )
	    (novalue)
	)
	(spell-line
	    end
	    word
	    saved-mode-line
	    saved-syntax-table
	    type-ahead
	    type-ahead-done
	    old-in-spell-line
	    (setq old-in-spell-line ~spell-in-spell-line)
	    (setq ~spell-in-spell-line 1)
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq type-ahead-done 0)
	    (setq type-ahead "")
	    (save-window-excursion
		(end-of-line)
		(setq end (+ 0 (dot)))
		(beginning-of-line)
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(forward-word)
		(while (& (! (eobp)) (<= (+ 0 (dot)) end))
		    (set-mark)
		    (backward-word)
		    (use-syntax-table saved-syntax-table)
		    (exchange-dot-and-mark)
		    (if (! (emacs$spell 7 (setq word (region-to-string))))
			(progn
			    (setq prefix-argument spell-feeps)
			    (prefix-argument-loop (send-string-to-terminal ""))
			    (message "I do not understand `"
				word
			 	"'. Please correct it and press ^C to continue")
			    (setq saved-mode-line mode-line-format)
			    (setq mode-line-format
				(concat
				    " Correcting `"
				    word
				    "'. Press ^C to continue "))
			    (if (! type-ahead-done)
			        (progn
				    (while keyboard-input-available
	        		        (setq type-ahead (concat type-ahead (char-to-string (get-tty-character))))
				    )
				    (setq type-ahead-done 1)
				)
			    )
			    (recursive-edit)
			    (setq mode-line-format saved-mode-line)
			)
		    )
		    (use-syntax-table "spell")
		    (forward-word)
		)
		(use-syntax-table saved-syntax-table)
	    )
	    (if (& type-ahead-done (!= (length type-ahead) 0)) (push-back-character type-ahead))
	    (setq ~spell-in-spell-line old-in-spell-line)
	    (novalue)
	)
	(spell-word-in-buffer
	    word
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(if (emacs$spell 7 (setq word (region-to-string)))
		    (message "`" word "\' is spelt correctly")
		    (progn
		      (save-window-excursion
			(pop-to-buffer "Help")
			(erase-buffer)
			(setq word (emacs$spell 8 word))
			(insert-string word)
			(beginning-of-file)
			(delete-white-space)
			(error-occurred (re-replace-string "\\(\\w\\w*\\)\\W\\W*" "\\1\n"))
			(error-occurred
			    (while (! (eobp))
				(end-of-line)
				(to-col 25)
				(insert-character ' ')
				(delete-next-character)
				(end-of-line)
				(to-col 50)
				(insert-character ' ')
				(delete-next-character)
				(next-line)
			    )
			)
			(beginning-of-file)
			(insert-string "Choose one of the following:\n")
			(beginning-of-file)
			(setq word
			    (expand-from-string-table "Select the correction required: "
				word
			    )
			  )
		      )
			(if (length word)
			    (progn
				(erase-region)
				(insert-string word)
			    )
			)
		    )
		)
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-newline
	    (spell-line)
	    (newline)
	)
	(spell-newline-and-indent
	    (spell-line)
	    (newline-and-indent)
	)
	(make-text-mode-spell
	    (declare-global TM-spell-line-hook)
	    (setq TM-spell-line-hook "spell-line")
	)
	(spell-ignore-word-in-buffer
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(emacs$spell 5 (region-to-string))
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-add-word-in-buffer
	    word
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(setq word (region-to-string))
		(emacs$spell 5 word)
		(emacs$spell 6 word)
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-ignore-word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (emacs$spell 5 (arg 1 ": spell-ignore-word "))
	    (novalue)
	)
	(spell-add-word
	    word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq word (arg 1 ": spell-add-word "))
	    (emacs$spell 5 word)
	    (emacs$spell 6 word)
	    (novalue)
	)
	(spell-load-personal-dictionary
	    word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (if prefix-argument-provided
	        (setq spell-personal-dictionary (arg 1 ": spell-load-personal-dictionary "))
	    )
	    (emacs$spell 4 spell-personal-dictionary)
	)
	(spell-add-dictionary
	    ~sad-dict
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq ~sad-dict (arg 1 ": spell-add-dictionary "))
	    (emacs$spell 4 ~sad-dict 0)
	)
    )
    (declare-global spell-feeps)
    (setq spell-feeps 1)
    (save-window-excursion
	(switch-to-buffer "Hack buffer")
	(use-syntax-table "spell")
	(modify-syntax-table "dull" "~")
	(modify-syntax-table "word" "-a-zA-Z'.0-9`\"")
    )
    (delete-buffer "Hack buffer")
    (error-occurred (execute-mlisp-file "spell.key"))
    (novalue)
)
