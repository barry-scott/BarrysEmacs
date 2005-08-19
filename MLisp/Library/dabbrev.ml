;;; Dynamic abbreviations package.
;;; dabbrev-expand-whole-word
;;;    0 - move to end of the word and expand
;;;    1 - expand the word to the left and erase to extra chars to the right
;;;
;;; db-backdot is used for memory in successive reverse searches.
;;; db-search-string is used, e.g., if you have just expanded "nan" to
;;;    "nanny", the next reverse search still looks for "nan".
;;; db-expanded word is used for communication between the word-location
;;;    and replacement functions.  It is the expanded version of the search
;;;    string.  It could be a local parameter, if I could get functions
;;;    with arguments to work.
;;; db-just-errorred is so, after you run out of search choices, you
;;;    come back full cycle instead of just dying.

(declare-global dabbrev-expand-whole-word)
(setq dabbrev-expand-whole-word 0); erase to right

(declare-buffer-specific db-backdot db-search-string db-just-errorred
                         db-expanded-word db-direction db-erased-word-tail db-word-tail-pos)
(defun
    (dabbrev
	(save-excursion start-position
	    (setq start-position (dot))
	    (if (| (!= (previous-command) this-command) (= db-just-errorred 1))
		;; This is a first expansion:  figure out
		;; db-search-string and db-backdot.
		(progn
		    (setq db-just-errorred 0)
		    (setq db-direction 0)
		    (setq db-expanded-word "")
		    ; cannot work at the start of the buffer
		    (if (= 1 (dot))
			(progn
			    (setq db-just-errorred 1)
			    (error-message "dabbrev: position after at least one character of a word")
			)
		    )
		    (set-mark)
		    ; must be in or at the end of a word
		    (backward-character)
		    (if (! (looking-at "\\w"))
			(progn
			    (setq db-just-errorred 1)
			    (error-message "dabbrev: position after at least one character of a word")
			)
		    )
		    (forward-character)
		    (setq db-erased-word-tail "")
		    (if (looking-at "\\w")
			(if dabbrev-expand-whole-word
			    (progn
				(forward-word)
				(setq start-position (dot))
				(set-mark)
			    )
			    (progn
				(forward-word)
				(setq db-erased-word-tail (region-to-string))
				(erase-region)
				(setq db-word-tail-pos (dot))
			    )
			)
		    )
		    (backward-word)
		    (setq db-backdot (dot))
		    (setq db-search-string (region-to-string))
		)
		;; Otherwise, this is a second expansion, use already-existing stuff.
		(goto-character db-backdot)
	    )
	    (db-locate-expanded-word)
	)
	(db-replace-with-new-word)
    )
)

;;; Search backwards, starting with the current dot, to find a match.
;;; Our context includes the appropriate setting of dot.
(defun
    (db-locate-expanded-word db-new-expanded-word
	(if db-direction
	    (if (error-occurred (re-search-forward (concat "\\b" db-search-string "\\w")))
		(progn
		    (message "No more matches")
		    (setq db-expanded-word db-search-string)
		    (setq db-just-errorred 1)
		)
		(progn
		    ;; Get the expanded word into a string used by the next function.
		    (backward-word)
		    (set-mark)
		    (forward-word)
		    (setq db-new-expanded-word (region-to-string))
		    ;; Set backdot to the end of the match word, for
		    ;; use in later forward searches.
		    (setq db-backdot (dot))
		    (if (= db-new-expanded-word db-expanded-word)
			(db-locate-expanded-word)
			(setq db-expanded-word db-new-expanded-word)
		    )
		)
	    )
	    ; else
	    (if (error-occurred (re-search-reverse (concat "\\b" db-search-string "\\w")))
		(progn
		    (setq db-direction 1)
		    (goto-character start-position)
		    (db-locate-expanded-word)
		)
		(progn
		    ;; Set backdot to the beginning of the match word, for
		    ;; use in later reverse searches.
		    (setq db-backdot (dot))
		    ;; Get the expanded word into a string used by the next function.
		    (set-mark)
		    (forward-word)
		    (setq db-new-expanded-word (region-to-string))
		    (backward-word)
		    (if (= db-new-expanded-word db-expanded-word)
			(db-locate-expanded-word)
			(setq db-expanded-word db-new-expanded-word)
		    )
		)
	    )
	)
    )
)

;;; With the cursor back where it started, finally do the expansion.
(defun
    (db-replace-with-new-word
	;(message (concat "Expanded word is |" db-expanded-word "|"))
	;(sit-for 5)
	(if db-just-errorred
	    (progn
		(delete-previous-word)
		(insert-string db-search-string)
		; leave dot here
		(save-excursion (insert-string db-erased-word-tail))
		(setq db-erased-word-tail "")
	    )
	    (progn
		(delete-previous-word)
		(save-excursion
		    (if (looking-at "\\w")
			(delete-next-word)
		    )
		)
		(insert-string db-expanded-word)
	    )
	)
    )
)
