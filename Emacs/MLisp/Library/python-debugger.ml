(declare-global
    Python-debugger-python	    ; the python image to run
    Python-debugger-buffer	    ; buffer name for debug output
    Python-debugger-output-buffer   ; buffer name for program output
    Python-debugger-process	    ; process name for debugger 
    Python-debugger-main-module	    ; main module being debugged
    ~Python-debugger-temp-buffer    ; temp buffer
)

(setq Python-debugger-python "python -u")
(setq Python-debugger-process "Python-debugger")
(setq Python-debugger-buffer "Python Debugger")
(setq Python-debugger-output-buffer "Python Output")
(setq Python-debugger-main-module "")
(setq ~Python-debugger-temp-buffer "~Python Debugger Temp")

(defun
    (Python-debugger
	(if (~pydeb-running)
	    (progn
		(message "Python debugger running")
		(pop-to-buffer Python-debugger-output-buffer)
		(pop-to-buffer Python-debugger-buffer)
	    )
	    (progn
		
		(setq Python-debugger-main-module (get-tty-file "Python-debugger main python module: "))
		
		(error-occurred (kill-process Python-debugger-process))
		
		(setq cli-name (concat Python-debugger-python " " (file-name "emacs$library:bemacs_python_debugger.py") " " Python-debugger-main-module))
		(start-DCL-process Python-debugger-process)
		
		(pop-to-buffer Python-debugger-output-buffer)
		(Python-debugger-setup-keybinding)
		(erase-buffer)
		(setq mode-line-format "Python program output")
		(Python-debugger-setup-keybinding)
		
		(pop-to-buffer Python-debugger-buffer)
		(Python-debugger-setup-keybinding)
		(erase-buffer)
		(setq mode-line-format "Python-debugger (%m)")
		(setq mode-string "Initialising")
		
		(set-process-output-procedure Python-debugger-process "~pydeb-output")

		(message "Python debugger starting")
		(sit-for 0)
		
		(set-process-termination-procedure Python-debugger-process "~pydeb-terminate")
	    )
	)
	(novalue)
    )
)

(defun
    (~pydeb-running
	(= (process-status Python-debugger-process) 1)
    )
)

(defun 
    (~pydeb-start-debugger
	
	(wait-for-process-input-request Python-debugger-process)
	(send-string-to-process Python-debugger-process
	    (concat "python l:\\zzz\\mypdb.py " Python-debugger-main-module "\n"))
    )
)
(defun
    (~pydeb-output
	(save-window-excursion
	    (switch-to-buffer ~Python-debugger-temp-buffer)
	    (end-of-file)
	    (insert-string (process-output Python-debugger-process))
	)
	(~pydeb-handle-new-output)
    )
)

(defun
    (~pydeb-handle-new-output
	~process-new
	~process-pdb
	
	(setq ~process-new 1)
	(setq ~process-pdb 0)
	
	(while ~process-new
	    (setq ~process-new 0)
	    (save-window-excursion
		(switch-to-buffer ~Python-debugger-temp-buffer)
		(beginning-of-file)
		(set-mark)
		(if (! (error-occurred (search-forward "\^b")))
		    (progn
			(if (> (+ (dot)) 2)
			    (progn
				(backward-character 1)
				(append-region-to-buffer Python-debugger-output-buffer)
				(erase-region)
			    )
			)
			(if (! (error-occurred (search-forward "\^e")))
			    (progn
				; delete the last commands output
				(erase-region)
				; run the handler again in case there is a ^B
				(temp-use-buffer Python-debugger-buffer)
				(setq mode-string "Running")
				(setq ~process-new 1)
			    )
			    (! (error-occurred (search-forward "\^p")))
			    (progn
				; we have a prompt and do not have an end
				(temp-use-buffer Python-debugger-buffer)
				(setq mode-string "Waiting")
				(setq ~process-pdb 1)
			    )
			)
		    )
		    ; else no ^B
		    (progn
			(end-of-file)
			(append-region-to-buffer Python-debugger-output-buffer)
			(erase-region)
			
		    )
		)
	    )
	)
	(if ~process-pdb
	    (~pydeb-handle-pdb-output)
	)
    )
)

(defun
    (~pydeb-handle-pdb-output
	~file
	~line
	
	(setq ~file "")
	(setq ~line 0)
	
	(save-window-excursion
	    (switch-to-buffer ~Python-debugger-temp-buffer)
	    (beginning-of-file)
	    (if (! (error-occurred (ere-search-forward "^> (.*?\\()(\\d+)\\)")))
		(progn
		    
		    (region-around-match 1)
		    (setq ~file (string-extract (region-to-string) 0 -1))
		    
		    (region-around-match 2)
		    (setq ~line (+ (region-to-string)))
		)
	    )	 
	    (goto-character 3)	; skip ^B\n
	    (set-mark)
	    (search-forward "\^p")
	    (backward-character 1)
	    (copy-region-to-buffer Python-debugger-buffer)
	    (erase-buffer)
	    (insert-string "\^b\n")
	)
	
	(pop-to-buffer Python-debugger-output-buffer)
	(pop-to-buffer Python-debugger-buffer)
	
	(if (file-exists ~file)
	    (progn
		(visit-file ~file)
		(if (= ~line 0)
		    (setq ~line 1))
		(goto-line ~line)
	    )
	)
    )
)

(defun
    (~pydeb-terminate
	(save-excursion
	    (temp-use-buffer Python-debugger-buffer)
	    (setq mode-string "Exited")
	    (message "Python-debugger process Exited")
	    (sit-for 0)
	)
    )
)

; 
; Commands
; 
(defun
    ~pydeb-cmd( ~cmd)
    (if (~pydeb-running)
	(send-string-to-process Python-debugger-process
	    (concat ~cmd "\n"))
    )
)
(defun
    (~pydeb-cmd-prompt
	(if (~pydeb-running)
	    (~pydeb-cmd (get-tty-string "Python-debugger command: "))
	)
    )
)

(defun
    (~pydeb-cmd-step
	(~pydeb-cmd "step")
    )
)

(defun
    (~pydeb-cmd-next
	(~pydeb-cmd "next")
    )
)

(defun
    (~pydeb-cmd-continue
	(~pydeb-cmd "continue")
    )
)

(defun
    (~pydeb-cmd-break
	(~pydeb-cmd
	    (concat
		"break " current-buffer-file-name ":" (current-line-number)
	    )
	)
    )
)

(defun
    (~pydeb-cmd-print
	(if (~pydeb-running)
	    (if (error-occurred (mark))
		(~pydeb-cmd
		    (concat
			"print " (get-tty-string "Python-debugger print: ")
		    )
		)
		(~pydeb-cmd
		    (concat
			"print " (region-to-string)
		    )
		)
	    )
	)
    )
)

(defun
    (~pydeb-cmd-kill
	(if (~pydeb-running)
	    (kill-process Python-debugger-process) 
	)
    )
)

(defun
    (Python-debugger-setup-keybinding
	(local-bind-to-key "~pydeb-cmd-prompt" "``")
	(local-bind-to-key "~pydeb-cmd-step" "`s")
	(local-bind-to-key "~pydeb-cmd-continue" "`c")
	(local-bind-to-key "~pydeb-cmd-next" "`n")
	(local-bind-to-key "~pydeb-cmd-print" "`p")
	(local-bind-to-key "~pydeb-cmd-break" "`b")
	(local-bind-to-key "~pydeb-cmd-kill" "`k")
    )
)
