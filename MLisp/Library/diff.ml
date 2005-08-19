; 
;   diff.ml
; 
(declare-global
    diff-command)   		; holds the diff command to use

(declare-buffer-specific
    diff-buffer)		; holds the buffer that was last differ

(defun
    (diff
	~prefix 
	
	(setq ~prefix prefix-argument-provided)
	
	(if (= current-buffer-type "file")
	    (progn
		~buffer
		~filename
		~backup-filename
		~buffer-is-not-modified
		
		(setq ~buffer current-buffer-name)
		(setq ~filename current-buffer-file-name)
		(setq ~buffer-is-not-modified (= buffer-is-modified 0))
		
                (setq ~backup-filename (file-format-string backup-filename-format ~filename))
		(save-excursion
		    (save-restriction
			(widen-region)
			(beginning-of-file)
			(set-mark)
			(end-of-file)
			(write-region-to-file "_diff_.tmp")
		    )
		)
		
		(pop-to-buffer "diff")
		(setq diff-buffer ~buffer)
		(diff-mode)
		(erase-buffer)
		(set-mark)
		
		(if (| ~buffer-is-not-modified ~prefix)
		    (filter-region
			(concat diff-command " _diff_.tmp \"" ~backup-filename "\"") )
		    (filter-region
			(concat diff-command " _diff_.tmp \"" ~filename "\"") )
		)
		
		(beginning-of-file)
		(unset-mark)
		(unlink-file "_diff_.tmp")
		(novalue)
	    )
	    ; else nothing to do
	    (message "Nothing to diff")
	)
    )
)
(defun
    (diff-mode
	(setq mode-string (concat "Diff of " diff-buffer))
	(use-local-map "diff-mode-map")
    )
)
; 
; Bound to return
; 
(defun
    (diff-goto-diff
	~line 
	
	(if (= operating-system-name "Windows")
	    (save-excursion
		(beginning-of-line)
		(while (& (! (bobp)) (! (looking-at " *[0-9]")))
		    (previous-line)
		)
		(looking-at " *[0-9][0-9]*")
		(region-around-match 0)
		(setq ~line (+ (region-to-string)))
	    )
	    ; default parsing - unix style
	    (save-excursion
		(beginning-of-line)
		(while (& (! (bobp)) (! (looking-at "[0-9]")))
		    (previous-line)
		)
		(looking-at "[0-9][0-9]*")
		(region-around-match 0)
		(setq ~line (+ (region-to-string)))
	    )
	)	
	(pop-to-buffer diff-buffer)
	(goto-line ~line)
    )
)

(if (= diff-command 0)
    (if (= operating-system-name "unix")
	(setq diff-command "diff")
	(= operating-system-name "vms")
	(setq diff-command "diff")
	(= operating-system-name "Windows")
	(setq diff-command "fc /n")
	; default
	(setq diff-command "diff")
    )
)
(save-excursion
    (temp-use-buffer "diff")
    (define-keymap "diff-mode-map")
    (use-local-map "diff-mode-map")
    (local-bind-to-key "diff-goto-diff" "\r")
)
