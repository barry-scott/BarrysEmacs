(defun
    (unix-man-word-in-buffer
	~word
	~prefix
	
	(setq ~prefix prefix-argument-provided)
	(save-excursion
	    (forward-character)
	    (backward-word)
	    (set-mark)
	    (forward-word)
	    (setq ~word (region-to-string))	
	)
	(if ~prefix
	     (setq ~word (concat (get-tty-string (concat ": Unix man " ~word " section: ")) " " ~word))
	)
	(unix-man ~word)
    )
)
(defun
    unix-man (~word (get-tty-string ": Unix man: "))
    overstrike-length    
    (pop-to-buffer "Unix man")
    (erase-buffer)
    (set-mark)
    (message "Unix-man " ~word " fetching text...")(sit-for 0)
    (filter-region (concat "man " ~word))
    (unset-mark)
    (message "Unix-man " ~word " cleaning up text...")(sit-for 0)
    (beginning-of-file)
    ;
    ; calibrate the overstrike amount
    ; 
    (setq overstrike-length 2)
    (error-occurred
	pattern
	(re-search-forward "[^_]\^h")
	(setq pattern
	    (concat
		(char-to-string (following-char))	
		"^\h"
		(char-to-string (following-char))	
	    )
	)
	(setq overstrike-length 2)
	(forward-character)
	(while (looking-at pattern)
	    (setq overstrike-length (+ overstrike-length 3))
	    (forward-character 3)
	)
    )
    (beginning-of-file)
    (while (! (error-occurred (search-forward "\^h")))
	(backward-character 2)
	(if
	    (if
		(= (following-char) '_')
		(! (looking-at "_\^h_"))
		0
	    )
	    ; its underlined text
	    (progn
		(delete-next-character 2)
		(apply-colour-to-region (dot) (+ (dot) 1) 1)
	    )
	    (looking-at "O\^h+")
	    ; its a bullet mark
	    (progn
		(delete-next-character 3)
		(insert-string "º")
		(apply-colour-to-region (dot) (- (dot) 1) 2)
	    )
	    ; its assumed to be bold text
	    (progn
		(delete-next-character overstrike-length)
		(apply-colour-to-region (dot) (+ (dot) 1) 2)
	    )
	)
    )
    (beginning-of-file)
)
