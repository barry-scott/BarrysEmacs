; 
; vms-help.ml
; 
(declare-global ~help-topic ~help-level)
(defun
    (~help-sp			; move to next page in this node
	(next-page)
	(~help-mode-line)
    )
)
(defun
    (~help-bs			; move to previous page in this node
	(previous-page)
	(~help-mode-line)
    )
)
(defun
    (~help-t			; set the help topic to blank
	(~help-n "")
    )
)
(defun
    (~help-q			; quit help
	(if (!= (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (~help-a			; move to next line
	(scroll-one-line-up)
	(~help-mode-line)
    )
)
(defun
    (~help-z			; move to previous line
	(scroll-one-line-down)
	(~help-mode-line)
    )
)
(defun
    (~help-b			; goto beginning of entry
	(beginning-of-file)
	(~help-mode-line)
    )
)
(defun
    (~help-e			; goto end of entry
	(end-of-file)
	(~help-mode-line)
    )
)
(defun
    (~help-help			; get help about info
	lf
	(setq lf (char-to-string 10))
	(switch-to-buffer "~help-hack")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(erase-buffer)
	(insert-string
	    (concat
		"HELP: help documentation reader" lf lf
		"	Key	Action" lf
		"	---	------" lf
		"	<sp>	move to next page" lf
		"	<bs>	move to previous page" lf
		"	a	scroll up one line" lf
		"	z	scroll down one line" lf
		"	b	go to beginning of topic" lf
		"	e	go to end of topic" lf
		"	n	enter a new topic pacth" lf
		"	d	go down a topic" lf
		"	u	go up a topic" lf
		"	t	go to the Root topic" lf
		"	q	quit HELP" lf
		"	?	write this help text" lf
		"	^C	exit Emacs" lf lf
		"	^L	redraw screen" lf lf
		"	Other keys are illegal" lf lf
		" -- Type any character to continue HELP --" lf
	    )
	)
	(get-tty-character)
	(switch-to-buffer "*help*")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
    )
)
(defun
    (~help-root			; input file information
	lf
	(list-databases)
	(erase-buffer)
	(yank-buffer "Database list")
	(beginning-of-file)
	(if (error-occurred
		(re-search-forward (concat "^" ~help-database ":$"))
	    )
	    (progn
		(erase-buffer)
		(insert-string "No help files are available.")
	    )
	    (progn
		(forward-character)
		(set-mark)
		(beginning-of-file)
		(erase-region)
		(if (! (error-occurred (re-search-forward "^.*:$")))
		    (progn
			(beginning-of-line)
			(set-mark)
			(end-of-file)
			(erase-region)
		    )
		)
		(beginning-of-file)
		(error-occurred (replace-string "(read only)" ""))
	    )
	)
	(beginning-of-file)
	(setq lf (char-to-string 10))
	(insert-string
	    (concat
		lf
		"			   HELP Root Topic" lf
		"			   ---- ---- -----" lf lf
		"The available help files are: "
		lf lf
	    )
	)
	(beginning-of-file)
    )
)
(defun
    (~help-u i			; go to previous node
	(if (<= (length ~help-topic) 0)
	    (error-message "This is the Root topic")
	)
	(temp-use-buffer "~help-hack")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(erase-buffer)
	(insert-string ~help-topic)
	(backward-word)
	(kill-to-end-of-line)
	(delete-white-space)
	(set-mark)
	(beginning-of-file)
	(setq i (region-to-string))
	(switch-to-buffer "*help*")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(~help-n i)
	(~help-mode-line)
    )
)
(defun
    (~help-d i j k			; get next item from a menu
	(setq i (dot))
	(if (<= (length ~help-topic) 0)
	    (progn
		(~help-n "HELP")
		(setq ~help-topic "")
	    )
	)
	(beginning-of-file)
	(if
	    (error-occurred
		(search-forward "Additional information")
	    )
	    (progn
		(goto-character i)
		(error-message
		    "No more information is known about this topic")
	    )
	)
	(next-line)(next-line)
	(line-to-top-of-window)
	(~help-mode-line)
	(if (> (length (setq j (get-tty-string "Topic "))) 0)
	    (setq j (concat ~help-topic " " j))
	    (setq j ~help-topic)
	)
	(~help-n j)
	(~help-mode-line)
    )
)
(defun
    (~help-n			; get next node spec
	new-spec
	(temp-use-buffer "~help-hack")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(setq new-spec (arg 1 "New topic is "))
	(erase-buffer)
	(if (<= (length new-spec) 0)
	    (~help-root)
	    (if (error-occurred (fetch-help-database-entry ~help-database new-spec))
		(progn
		    (switch-to-buffer "*help*")
		    (setq current-buffer-journalled 0)
		    (setq current-buffer-checkpointable 0)
		    (~help-mode-line)
		    (error-message
			(concat "No help available for `"
			    new-spec
			    "'"
			)
		    )
		)
	    )
	)
	(switch-to-buffer "*help*")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(erase-buffer)
	(yank-buffer "~help-hack")
	(beginning-of-file)
	(setq ~help-topic new-spec)
	(~help-mode-line)
    )
)
(defun
    (vms-help
	(save-window-excursion
	    ~help-database
	    (setq ~help-database "help")
	    (~help-main)
	    (recursive-edit)
	)
    )
)
(defun
    (~help-main
	i
	(if ~help-level
	    (progn
		(switch-to-buffer "*help*")
		(setq current-buffer-journalled 0)
		(setq current-buffer-checkpointable 0)
		(delete-other-windows)
	    )
	    (progn
		(temp-use-buffer "~help-hack")
		(setq current-buffer-journalled 0)
		(setq current-buffer-checkpointable 0)
		(use-syntax-table "*help*")
		(modify-syntax-table "word" "!-~")
		(setq mode-line-format "")
		(switch-to-buffer "*help*")
		(setq current-buffer-journalled 0)
		(setq current-buffer-checkpointable 0)
		(setq highlight-region 0)
		(use-syntax-table "*help*")
		(setq ~help-topic "")
		(erase-buffer)
		(setq i 0)
		(while (<= i 255)
		    (local-bind-to-key "illegal-operation" (char-to-string i))
		    (setq i (+ i 1))
		)
		(local-bind-to-key "execute-extended-command" "\ex")
		(local-bind-to-key "execute-extended-command" "\(do)")
		(local-bind-to-key "~help-sp" " ")
		(local-bind-to-key "~help-bs" "\^h")
		(local-bind-to-key "~help-a" "a")
		(local-bind-to-key "~help-z" "z")
		(local-bind-to-key "~help-b" "b")
		(local-bind-to-key "~help-e" "e")
		(local-bind-to-key "~help-help" "?")
		(local-bind-to-key "~help-n" "n")
		(local-bind-to-key "~help-d" "d")
		(local-bind-to-key "~help-t" "t")
		(local-bind-to-key "~help-u" "u")
		(local-bind-to-key "~help-q" "q")
		(local-bind-to-key "exit-emacs" "\^c")
		(local-bind-to-key "redraw-display" "\^l")
		(setq ~help-level 1)
		(error-occurred
		    (extend-database-search-list "help" "SYS$LOGIN:HELP.HLB" library-access)
		)
		(extend-database-search-list "help" "SYS$HELP:HELPLIB.HLB" 1)
		(~help-root)
		(~help-mode-line)
		(setq fetch-help-database-flags 5); don''t insert the key lines
	    )
	)
	(novalue)
    )
)
(defun
    (~help-mode-line
	(setq mode-line-format "%m")
	(setq mode-string
	    (concat
		"HELP documentation reader    "
		(if (<= (length ~help-topic) 0)
		    "Root topic"
		    (concat "Topic:" ~help-topic)
		)
		(if (save-excursion (end-of-window) (eobp))
		    ""
		    "    --More--"
		)
	    )
	)
	(delete-other-windows)
    )
)
