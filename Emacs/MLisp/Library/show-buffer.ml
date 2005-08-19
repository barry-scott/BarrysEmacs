; 
; show-buffer.ml
;   Copyright (c) 1997-2002 Barry A. Scott
; 
(defun
    (show-buffer num-lines page-num data num-pages
	(setq num-pages 0)
	(save-window-excursion
	    (end-of-file)
	    (setq num-lines (current-line-number))
	    (setq page-num  (/ num-lines 60))
	    (while (! (error-occurred (search-reverse "\^l")))
		(setq num-pages (+ num-pages 1))
	    )
	    (setq data 
		(concat "\n"
		  "               Buffer:  " current-buffer-name 
			(if buffer-is-modified "  (modified)" "") "\n"
		  "                 Type:  " current-buffer-type "\n"
		  "                 File:  " current-buffer-file-name "\n"
		  "          End-of-line:  " current-buffer-end-of-line-style "\n\n"

		  "      No. of ff chars:  " num-pages "\n"
		  "         No. of lines:  " num-lines  " (~" (+ page-num  1)  
			  (if (< page-num 1)  " page" " pages" ) "\) \n"
		  "    No. of characters:  " (buffer-size) "\n\n"

		  "   Abbreviation table:  " current-buffer-abbrev-table "\n"
		  "    Abbreviation mode:  " (if abbrev-mode "On" "Off") "\n"
		  "         Syntax table:  " current-buffer-syntax-table "\n"
		  "         Syntax Array:  " (if syntax-array "On" "Off") "\n"
		  "      Syntax Coluring:  " (if syntax-colouring "On" "Off") "\n"
		  "              Margins:  "
			"L " left-margin ", R " right-margin
			(if (is-bound buffer-justify-flag)
			    (if buffer-justify-flag 
				(concat ", justified right margin" ""
				    (if (!= indent-offset 0) ", Indent " "")
				    (if (!= indent-offset 0) indent-offset "")
				)
				""
			    )
			    ""
			) "\n"
		  "               Keymap:  " current-local-keymap "\n"
		  "         Replace mode:  " replace-mode "\n"
		  "                Macro:  " current-buffer-macro-name "\n\n"
                                                
		  "       Checkpointable:  " current-buffer-checkpointable 
			(if current-buffer-checkpointable 
			   (concat "  (frequency: " checkpoint-frequency ")")
			   ""
			)   "\n"
		  "          Journalable:  " current-buffer-journalled "\n"
		  "     Mode line format: " mode-line-format "\n"
;		  "                        "
		)
	    )
	    (delete-other-windows)
	    (switch-to-buffer "show-buffer-data")
	    (setq mode-line-format 
	 	" %[%hBuffer: %1*%18b %h   Press SPACE to return and continue ...              ")
	    (setq current-buffer-checkpointable 0)
	    (erase-buffer)
	    (insert-string data)
	    (setq buffer-is-modified 0)
	    (beginning-of-file)
	    (get-tty-character)
	    (delete-buffer "show-buffer-data")
	    (setq mode-line-format default-mode-line-format)
	    (novalue)
	)
    )
)
