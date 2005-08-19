(declare-global ~BUF-current-buffer)
(declare-global ~BUF-visited-buffer)
(declare-global ~BUF-wrap-save)
(defun
    (buffer-dired
	(error-occurred
	    (save-window-excursion
		(setq ~BUF-current-buffer (current-buffer-name))
		(setq ~BUF-wrap-save wrap-long-lines)
		(setq wrap-long-lines 0)
		(switch-to-buffer "Buffer list")
		(delete-other-windows)
		(local-bind-to-key "~BUF-previous-line" "p")
		(local-bind-to-key "~BUF-next-line" "n")
		(local-bind-to-key "~BUF-help" "?")
		(local-bind-to-key "~BUF-delete-buffer" "d")
		(local-bind-to-key "~BUF-buffer-check" "c")
		(local-bind-to-key "~BUF-buffer-journal" "j")
		(local-bind-to-key "~BUF-buffer-modified" "m")
		(local-bind-to-key "~BUF-write-file" "w")
		(local-bind-to-key "~BUF-read-file" "r")
		(local-bind-to-key "~BUF-visit-file" "v")
		(local-bind-to-key "~BUF-visit-buffer" "b")
		(local-bind-to-key "~BUF-get-from-library" "g")
		(local-bind-to-key "~BUF-put-in-library" "l")
		(local-bind-to-key "~BUF-quit" "q")
		(local-bind-to-key "~BUF-examine-buffer" "e")
		(~BUF-locate-buffer)
		(recursive-edit)
	    )
	)
	(setq wrap-long-lines ~BUF-wrap-save)
	(if (length ~BUF-current-buffer)
	    (switch-to-buffer ~BUF-current-buffer)
	)
    )
)
(defun    
    (~BUF-help
	(save-window-excursion
	    (switch-to-buffer "Help")
	    (erase-buffer)
	    (delete-other-windows)
	    (insert-string "	Buffer DIRED\n\n")
	    (insert-string "	Help				?\n")
	    (insert-string "	Next line			n\n")
	    (insert-string "	Previous line			p\n")
	    (insert-string "	Delete buffer			d\n")
	    (insert-string "	Change checkpointing		c\n")
	    (insert-string "	Change journalling		j\n")
	    (insert-string "	Change buffer is modified	m\n")
	    (insert-string "	Edit this buffer		e\n")
	    (insert-string "	Fetch buffer-name from library	g\n")
	    (insert-string "	Put buffer-name into library	l\n")
	    (insert-string "	Write out this buffer		w\n")
	    (insert-string "	Re-read the file into buffer	r\n")
	    (insert-string "	Visit a file			v\n")
	    (insert-string "	Visit a buffer			b\n")
	    (insert-string "	Return to original buffer (*)	q\n")
	    (insert-string "\nType any character to continue\n")
	    (get-tty-character)
	    (novalue)
	)
    )
)
(defun    
    (~BUF-next-line
	(next-line)
	(if (eobp) (previous-line))
    )
)
(defun    
    (~BUF-previous-line
	(previous-line)
	(if (looking-at "   ----") (next-line))
    )
)
(defun    
    (~BUF-locate-buffer
	(list-buffers)
	(beginning-of-file)
	(next-line)
	(next-line)
	(if (! (error-occurred
		   (while (!= ~BUF-current-buffer (~BUF-get-buffer-name))
		       (next-line))))
	    (progn
		(delete-next-character)
		(insert-character '*')
		(beginning-of-line))
	    (progn
		(beginning-of-file)
		(next-line)
		(next-line)
	    )
	)
    )
)
(defun    
    (~BUF-locate-visited-buffer
	(beginning-of-file)
	(next-line)
	(next-line)
	(if (error-occurred
		(while (!= ~BUF-visited-buffer (~BUF-get-buffer-name))
		    (next-line)))
	    (progn
		(beginning-of-file)
		(next-line)
		(next-line)
	    )
	)
    )
)
(defun    
    (~BUF-examine-buffer
	(setq ~BUF-current-buffer (~BUF-get-buffer-name))
	(exit-emacs)
    )
)
(defun
    (~BUF-quit
	(if (recursion-depth)
	    (exit-emacs)
	)
    )
)
(defun    
    (~BUF-buffer-check bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq current-buffer-checkpointable
		(! current-buffer-checkpointable)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-buffer-journal bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq current-buffer-journalled
		(! current-buffer-journalled)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-buffer-modified bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq buffer-is-modified (! buffer-is-modified))
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun
    (~BUF-get-from-library bufdir-dot local-dot buf-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (switch-to-buffer (~BUF-get-buffer-name))
	    (setq buf-name (current-buffer-name))
	    (setq local-dot (+ 0 (dot)))
	    (erase-buffer)
	    (fetch-database-entry "MLisp-library" (current-buffer-name))
	    (goto-character local-dot))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Fetched " buf-name " from the MLisp-library"))
    )
)
(defun   
    (~BUF-put-in-library bufdir-dot buf-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq buf-name (current-buffer-name))
	    (put-database-entry "MLisp-library" (current-buffer-name)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Put " buf-name " into the MLisp-library"))
    )
)
(defun
    (~BUF-visit-file file-name error-found
	(save-window-excursion
	    (setq file-name (arg 1 "Name of file to edit: "))
	    (setq error-found (error-occurred (visit-file file-name)))
	    (setq ~BUF-visited-buffer (current-buffer-name))
	)
	(~BUF-locate-buffer)
	(~BUF-locate-visited-buffer)
	(if error-found
	    (message "Problem visiting the file - Probably not there!")
	)
    )
)
(defun
    (~BUF-visit-buffer buffer-name
	(save-window-excursion
	    (setq buffer-name (arg 1 "Name of buffer to edit: "))
	    (switch-to-buffer buffer-name)
	    (setq ~BUF-visited-buffer (current-buffer-name))
	)
	(~BUF-locate-buffer)
	(~BUF-locate-visited-buffer)
    )
)
(defun
    (~BUF-read-file local-dot bufdir-dot
	(setq bufdir-dot (+ (dot) 0))
	(save-window-excursion
	    (switch-to-buffer (~BUF-get-buffer-name))
	    (setq local-dot (+ (dot) 0))
	    (if (error-occurred (read-file (current-file-name)))
		(message "Cannot find the file")
		(goto-character local-dot)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-write-file bufdir-dot buf-name file-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (write-current-file)
	    (setq buf-name (current-buffer-name))
	    (setq file-name (current-file-name)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Wrote buffer " buf-name " to " file-name))
    )
)
(defun    
    (~BUF-get-buffer-name buffer-name
	(save-window-excursion
	    (beginning-of-line)
	    (while (& (!= (current-column) 17) (! (eolp)))
		(forward-character))
	    (set-mark)
	    (re-search-forward " [ A]")
	    (backward-character)
	    (backward-character)
	    (setq buffer-name (region-to-string))
	    (if (error-occurred (use-old-buffer buffer-name))
		(error-message "Cannot locate buffer")
	    )
	    buffer-name
	)
    )
)
(defun    
    (~BUF-delete-buffer bufdir-dot
	(setq bufdir-dot (+ (dot)))
	(if (= "Buffer list" (~BUF-get-buffer-name))
	    (error-message "You cannot delete this buffer!")
	    (progn
		~BUF-buffer-name
		(setq ~BUF-buffer-name (~BUF-get-buffer-name))
		(delete-buffer ~BUF-buffer-name)
		(if (= ~BUF-buffer-name ~BUF-current-buffer)
		    (setq ~BUF-current-buffer "")
		)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
