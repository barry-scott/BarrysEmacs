(defun
    UI-unix-client-command(~cmd)
    ~arg
    ~first-char
    ~position
    ~readonly
    ~cur-buf
    ~client-wd
    ~result

    (setq ~result "")
    (setq ~position 0)
    (setq ~readonly 0)
    (save-excursion
	(temp-use-buffer "~Server Commands")
	(erase-buffer)
	(insert-string ~cmd "\1")
	(beginning-of-file)
	(set-mark)
	(search-forward "\1")
	(backward-character)
	(setq ~client-wd (region-to-string))
	(forward-character)
	(set-mark)	
    )
    (while
	(!
	    (error-occurred
		(save-excursion
		    (temp-use-buffer "~Server Commands")
		    (search-forward "\1")
		    (backward-character)
		    (setq ~arg (region-to-string))
		    (forward-character)
		    (set-mark)
		)
	    )
	)
	(setq ~first-char (substr ~arg 1 1))
	(if (= ~first-char "-")
	    ; option
	    (if (= ~arg "-r")
		(setq ~readonly 1)
		(novalue)
	    )
	    
	    (= ~first-char "+")
	    ; position arg
	    (setq ~position (+ (substr ~arg 2 -1)))
	    
	    (= ~first-char "(")
	    ; MLisp
	    (error-occurred (setq ~result (execute-mlisp-line ~arg)))
	    ; Default is to visit the file
	    (progn
		~emacs-wd
		(setq ~emacs-wd (current-directory))
		(error-occurred
		    (change-directory ~client-wd)
		    (visit-file ~arg)
		    (if (> ~position 0) (goto-line ~position))
		    (setq ~position 0)
		    (if ~readonly (setq read-only-buffer 1))
		    (setq ~readonly 0)
		)
		(change-directory ~emacs-wd)
		(setq ~result "ok")
	   )
	)
    )
    ~result
)
