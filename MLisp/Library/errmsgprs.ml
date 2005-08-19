(if (= operating-system-name "VMS")
    (progn
	(if (! (is-bound bliss-source-file))
	    (progn
		(declare-global bliss-source-file)
		(setq bliss-source-file "")
	    )
	)
	(defun
	    (BLISS-check-source
		(while (<= (length bliss-source-file) 0)
		    (progn
			(setq bliss-source-file
			    (get-tty-file "Name the BLISS source file: ")
			)
			(if (! (file-exists bliss-source-file))
			    (progn
				(get-tty-string
				    (concat
					bliss-source-file
					" does not exist. Press RETURN to continue."
				    )
				)
				(setq bliss-source-file "")
			    )
			)
		    )
		)
		(setq error-file-name bliss-source-file)
	    )
	    (BLISS-message-parser
		(if (error-occurred
			(re-search-forward "^%[ 	]*WARN#\\|^?[ 	]*ERR #")
		    )
		    (setq error-line-number 0)
		    (save-window-excursion
			(beginning-of-line)
			(setq error-start-position (dot))
			(save-window-excursion
			    (if (error-occurred
				    (re-search-reverse
					"^;[ 	]*[PM]*[ 	]*\\([0-9][0-9]*\\)[ 	]"
				    )
				)
				(setq error-line-number 0)
				(progn
				    (region-around-match 1)
				    (setq error-line-number (region-to-string))
				    (BLISS-check-source)
				)
			    )
			)
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (C|PL/I-message-parser
		(if
		    (error-occurred
			(re-search-forward "^[ 	]*At line number \\([0-9][0-9]*\\) in \\(.*\\)\\.[ 	]*$")
		    )
		    (setq error-line-number 0)
		    (save-window-excursion
			(setq bliss-source-file "")
			(region-around-match 1)
			(setq error-line-number (region-to-string))
			(region-around-match 2)
			(setq error-file-name (region-to-string))
			(if (error-occurred (re-search-reverse "^%"))
			    (setq error-line-number 0)
			    (progn
				(beginning-of-line)
				(setq error-start-position (dot))
			    )
			)
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (error-message-parser
		(if (progn
			BLS C|PL1
			(setq BLS 2147483647)
			(error-occurred
			    (save-window-excursion
				(re-search-forward
				    "^%[ 	]*WARN#\\|^?[ 	]*ERR #"
				)
				(setq BLS (+ (dot) 0))
			    )
			)
			(setq C|PL1 2147483647)
			(error-occurred
			    (save-window-excursion
				(re-search-forward
				    "^[ 	]*At line number \\([0-9][0-9]*\\) in \\(.*\\)\\.[ 	]*$"
				)
				(setq C|PL1 (+ (dot) 0))
			    )
			)
			(< C|PL1 BLS)
		    )
		    (C|PL/I-message-parser)
		    (BLISS-message-parser)
		)
	    )
	)
    )
    (= operating-system-name "Windows")
    (progn
	(defun
	    (Microsoft-C-message-parser
		(error-occurred
		    (setq error-line-number 0)
		    (if
			(error-occurred
			    (re-search-forward "^ *\\(..*\\..*\\)(\\([0-9][0-9]*\\)) *:")
			)
			(if (error-occurred
				(re-search-forward
				    " \\([-\\_:.a-z0-9][-\\_:.a-z0-9]*\\) \\([0-9][0-9]*\\) *:")
			    )
			    (re-search-forward
				"\\([-\\_:.a-z0-9][-\\_:.a-z0-9]*\\)(\\([0-9][0-9]*\\)) *:")
			)
		    )
		    (setq error-start-position (dot))
		    (region-around-match 1)
		    (setq error-file-name (region-to-string))
		    (region-around-match 2)
		    (setq error-line-number (+ (region-to-string)))
		)
	    )
	    (error-message-parser
		(Microsoft-C-message-parser)
	    )
	)
    )
    (progn
	(defun
	    (c89-message-parser
		(if
		    (error-occurred
			(re-search-forward "^\\(.*\\):\\([0-9][0-9]*\\): *[ew][ra]r[on][ri][ng]* *:.*$")
		    )
		    (setq error-line-number 0)
		    (save-window-excursion
			(region-around-match 2)
			(setq error-line-number (region-to-string))
			(region-around-match 1)
			(setq error-file-name (region-to-string))
			(beginning-of-line)
			(setq error-start-position (dot))
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (hp-cc-message-parser
		(if
		    (error-occurred
			(re-search-forward
			    "\"\\(.*\\)\", line \\([0-9][0-9]*\\):.*$\\|\\(.*\\): \\([0-9][0-9]*\\): .*$"
			))
		    (setq error-line-number 0)
		    (save-window-excursion
			(region-around-match 1)
			
			(if (!= (dot) (mark))
			    (progn
				(setq error-file-name (region-to-string))
				(region-around-match 2)
				(setq error-line-number (region-to-string))
			    )
			    (progn
				(region-around-match 3)
				(setq error-file-name (region-to-string))
				(region-around-match 4)
				(setq error-line-number (region-to-string))
			    )
			)
			(beginning-of-line)
			(setq error-start-position (dot))
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (AIX-CSet++-message-parser
		(if
		    (error-occurred
			(re-search-forward
			    "\"\\(.*\\)\", line \\([0-9][0-9]*\\)\.[0-9][0-9]*:.*$"
			))
		    (setq error-line-number 0)
		    (save-window-excursion
			(region-around-match 1)
			(if (!= (dot) (mark))
			    (progn
				(setq error-file-name (region-to-string))
				(region-around-match 2)
				(setq error-line-number (region-to-string))
			    )
			    (progn
				(region-around-match 3)
				(setq error-file-name (region-to-string))
				(region-around-match 4)
				(setq error-line-number (region-to-string))
			    )
			)
			(beginning-of-line)
			(setq error-start-position (dot))
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (gcc-message-parser
		(if
		    (error-occurred
			(ere-search-forward 
			    "(^In file included from |^)([^:\n]+):(\\d+):.*$")
		    )
		    (setq error-line-number 0)
		    (save-window-excursion
			(region-around-match 3)
			(setq error-line-number (region-to-string))
			(region-around-match 2)
			(setq error-file-name (region-to-string))
			(beginning-of-line)
			(setq error-start-position (dot))
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    (error-message-parser
		(if (= operating-system-version "ultrix")
		    (c89-message-parser)
		    (= operating-system-version "hpux")
		    (hp-cc-message-parser)
		    (= operating-system-version "aix")
		    (AIX-CSet++-message-parser)
		    (gcc-message-parser)
		)
	    )
	)
    )
)
