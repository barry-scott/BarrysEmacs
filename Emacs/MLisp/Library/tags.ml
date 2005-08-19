; 
; tags.ml
; modified: Barry A. Scott  20-Oct-1994
; Fix C taging
; If no ANSI C tags are found then revert to K&R C taging
; 
(declare-global last-search-tag tag-language-type tag-language-pattern)
(declare-global tag-directory)
(setq tag-directory "")

(defun
    (set-tag-language ~TAGS-p
	(setq ~TAGS-p (arg 1 ": set-tag-language (type) ."))
	(if (> (length ~TAGS-p) 3) (error-message "Illegal file type"))
	(setq tag-language-type ~TAGS-p)
	(setq tag-language-pattern
	    (arg 2
		(concat
		    ": set-tag-language (type) ."
		    ~TAGS-p
		    " (pattern) "
		)
	    )
	)
	(novalue)
    )
)

(defun ~TAGS-dir (~TAGS-files)
    ~TAGS-file
    ~TAGS-fna
    
    (save-excursion
	(temp-use-buffer "*TAGP*")
	(erase-buffer)
	(insert-string ~TAGS-files)
	(beginning-of-file)
    )
    (while
	(!
	    (error-occurred
		(save-excursion
		    (temp-use-buffer "*TAGP*")
		    (re-search-forward "\\([^ \t,][^ \t,]*\\)")
		    (region-around-match 1)
		    (setq ~TAGS-fna (region-to-string))
		    (erase-region)
		)
	    )
	)
	(while
	    (if
		(&
		    (! (error-occurred (setq ~TAGS-file (expand-file-name ~TAGS-fna))))
		    (> (length ~TAGS-file) 0)
		)
		(file-exists ~TAGS-file)
		0
	    )
	    (setq ~TAGS-fna "")
	    (if (! (file-is-a-directory ~TAGS-file))
		(progn
		    (insert-string ~TAGS-file )
		    (newline)
		)
	    )
	)
    )
    (beginning-of-file)
    (error-occurred (re-replace-string "^.*[]>/\\\\]" ""))
    (error-occurred (re-replace-string "^.*" "\^_&"))
    (error-occurred (re-replace-string "\;[0-9]*" ""))
    (end-of-file)
)

(defun
    (~TAGS-mk-fn
	~TAGS-fn
	(setq ~TAGS-fn (current-file-name))
	(save-excursion
	    (temp-use-buffer "*TAGP*")
	    (erase-buffer)
	    (insert-string ~TAGS-fn)
	    (error-occurred (search-reverse ";"))
	    (set-mark)
	    ; find the start of the filename,
	    ; "]>" vms, "\" MSDOS, "/" unix, ":" macintosh
	    (if (error-occurred (re-search-reverse "[]\\/>:]"))
		(beginning-of-file)
		(forward-character)
	    )
	    (setq ~TAGS-fn (region-to-string))
	    (delete-buffer "*TAGP*")
	)
	~TAGS-fn
    )
)

(defun
    (to-tag-buffer
	(if (interactive)
	    (switch-to-buffer "*TAG*")
	    (temp-use-buffer "*TAG*")
	)
	(if (= (buffer-size) 0)
	    (progn
		(if (error-occurred (read-file "tags.tag"))
		    (progn
			(setq current-buffer-file-name "tags.tag")
			(message "New tag file")
		    )
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)

(defun
    (visit-tag-table ~TAGS-tfn
	(setq ~TAGS-tfn (arg 1 ": visit-tag-table "))
	(save-excursion
	    (temp-use-buffer "*TAG*")
	    (if (error-occurred (read-file ~TAGS-tfn))
		(setq current-buffer-file-name ~TAGS-tfn)
	    )
	)
	(novalue)
    )
)

(defun
    (list-tags
	(save-window-excursion
	    (to-tag-buffer)
	)
	(pop-to-buffer "Tag list")
	(erase-buffer)
	(yank-buffer "*TAG*")
	(beginning-of-file)
	(error-occurred (re-replace-string "^\^_\\(.*\\)$" "\nTags for file `\\1\'\n"))
	(error-occurred (re-replace-string "^\^A\\(.*\\)\^B.*$" "    \\1"))
    )
)

(defun
    (tag-replace-string
	~TAGS-pattern
	~TAGS-replacement
	(setq ~TAGS-pattern (arg 1 "Old pattern: "))
	(setq ~TAGS-replacement (arg 2 "New strings: "))
	(~TAGS-replace 0 ~TAGS-pattern ~TAGS-replacement)
	(message "Done.")
	(novalue)
    )
)

(defun
    (tag-query-replace-string
	~TAGS-pattern
	~TAGS-replacement
	(setq ~TAGS-pattern (arg 1 "Old pattern: "))
	(setq ~TAGS-replacement (arg 2 "New strings: "))
	(~TAGS-replace 5 ~TAGS-pattern ~TAGS-replacement)
	(message "Done.")
	(novalue)
    )
)

(defun
    (tag-execute-mlisp
	~TAGS-mlisp ~TAGS-buf ~TAGS-fn ~TAGS-pattern
	(setq ~TAGS-pattern (arg 1 ": tag-execute-mlisp (pattern) "))
	(setq ~TAGS-mlisp (arg 2 "MLisp expression: "))
	(save-window-excursion
	    (to-tag-buffer)
	    (beginning-of-file)
	    (while (! (error-occurred
			  (re-search-forward "^\^_\\(.*\\)$")
			  (region-around-match 1)
		      )
		   )
		(save-window-excursion
		    (save-window-excursion
			(visit-file (concat tag-directory (region-to-string)))
			(setq ~TAGS-buf current-buffer-name)
			(setq ~TAGS-fn current-buffer-file-name)
		    )
		    (message "Processing file `" ~TAGS-fn "'")
		    (sit-for 0)
		    (switch-to-buffer ~TAGS-buf)
		    (delete-other-windows)
		    (beginning-of-file)
		    (while (! (error-occurred (re-search-forward ~TAGS-pattern)))
			(save-excursion
			    (error-occurred (execute-mlisp-line ~TAGS-mlisp))
			)
		    )
		)
	    )
	)
	(message "Done.")
	(novalue)
    )
)

(defun ~TAGS-replace (~TAGS-flag ~TAGS-pat ~TAGS-rep)
    ~TAGS-buf ~TAGS-fn
    (save-window-excursion
	(to-tag-buffer)
	(beginning-of-file)
	(while (! (error-occurred
		      (re-search-forward "^\^_\\(.*\\)$")
		      (region-around-match 1)
		  )
	       )
	    (save-window-excursion
		(save-window-excursion
		    (visit-file (concat tag-directory (region-to-string)))
		    (setq ~TAGS-buf current-buffer-name)
		    (setq ~TAGS-fn current-buffer-file-name)
		)
		(message "Processing file `" ~TAGS-fn "'")
		(sit-for ~TAGS-flag)
		(switch-to-buffer ~TAGS-buf)
		(delete-other-windows)
		(error-occurred
		    (beginning-of-file)
		    (if ~TAGS-flag
			(re-query-replace-string ~TAGS-pat ~TAGS-rep)
			(re-replace-string ~TAGS-pat ~TAGS-rep)
		    )
		)
	    )
	)
    )
)
    
(defun
    (goto-tag ~TAGS-fn ~TAGS-str ~TAGS-pos ~TAGS-restart ~TAGS-cff ~TAGS-dcf
	(setq ~TAGS-dcf default-case-fold-search)
	(setq default-case-fold-search 1)
	(setq ~TAGS-cff case-fold-search)
	(setq case-fold-search 1)
	(setq ~TAGS-restart 0)
	(if (! prefix-argument-provided)
	    (progn
		(setq last-search-tag
		    (concat "\^A[^\^B]*" (quote (arg 1 ": goto-tag ")))
		)
		(setq ~TAGS-restart 1)))
	(save-excursion
	    (to-tag-buffer)
	    (if ~TAGS-restart (beginning-of-file))
	    (if (error-occurred (re-search-forward last-search-tag))
		(error-message
		    (concat "Cannot find tag `" 
			(substr last-search-tag 7 100)
			"\'"
		    )
		)
	    )
	    (beginning-of-line)
	    (re-search-forward "\^A\\([^\^B]*\\)\^B\\(.*\\)")
	    (region-around-match 1)
	    (setq ~TAGS-str (region-to-string))
	    (region-around-match 2)
	    (setq ~TAGS-pos (- (region-to-string) 300))
	    (save-excursion
		(re-search-reverse "\^_\\(.*\\)")
		(region-around-match 1)
		(setq ~TAGS-fn (region-to-string))
	    )
	)
	(visit-file (concat tag-directory ~TAGS-fn))
	(goto-character ~TAGS-pos)
	(if (error-occurred (search-forward ~TAGS-str))
	    (if (error-occurred (search-reverse ""))
		(message "Unable to locate tag " ~TAGS-str)
	    )
	)
	(beginning-of-line)
	(line-to-top-of-window)
	(setq case-fold-search ~TAGS-cff)
	(setq default-case-fold-search ~TAGS-dcf)
	(novalue)
    )
)

(defun
    (add-tag
	~TAGS-str
	~TAGS-pos
	~TAGS-fn
	~TAGS-cff ~TAGS-dcf
	(setq ~TAGS-dcf default-case-fold-search)
	(setq default-case-fold-search 1)
	(setq ~TAGS-cff case-fold-search)
	(setq case-fold-search 1)
	(save-window-excursion
	    (setq ~TAGS-fn (~TAGS-mk-fn))
	    (beginning-of-line)
	    (setq ~TAGS-pos (+ (dot) 0))
	    (set-mark)
	    (end-of-line)
	    (setq ~TAGS-str (region-to-string))
	    (to-tag-buffer)
	    (beginning-of-file)
	    (if (error-occurred (search-forward (concat "\^_" ~TAGS-fn "\n")))
		(progn
		    (beginning-of-file)
		    (insert-character '^_')
		    (insert-string ~TAGS-fn)
		    (newline)
		)
	    )
	    (insert-character '^A')
	    (insert-string ~TAGS-str)
	    (insert-character '^B')
	    (insert-string ~TAGS-pos)
	    (newline)
	    (beginning-of-file))
	(if (interactive)
	    (message (concat "Tagged `" ~TAGS-str "\'"))
	)
	(setq case-fold-search ~TAGS-cff)
	(setq default-case-fold-search ~TAGS-dcf)
	(novalue)
    )
)

(defun ~TAGS-default-pattern-processor (~TAGS-pattern)
    (re-search-forward ~TAGS-pattern)
    (setq ~TAGS-start-point (dot))
    (region-around-match 0)
    (exchange-dot-and-mark)
)

(defun ~TAGS-c-pattern-processor (~TAGS-arg)
    ~TAGS-word-start
    ; 
    ; Look for "word(" which can mark a declaration of a routine
    ; Keep looking if word is a keyword.
    ; Routine is found if the first char after the matching ")"
    ; is a "{" - ansi-C - or a word char - K&R C.
    ; This should also match #define func() entries
    ; 
    (while
	(progn
	    (re-search-forward "\\S\\C\\(\\w\\w*\\)[ \t\n]*(")
	    ; find the routines name and set the region around it
	    (setq ~TAGS-start-point (dot))
	    (region-around-match 1)
	    (setq ~TAGS-word-start (mark))
	    (backward-word)
	    ; do not tag keywords
	    (if (looking-at "if\\b\\|while\\b\\|switch\\b\\|for\\b")
		1
		(progn
		    (forward-paren 0)
		    (re-search-forward "\\S\\C[\n\t ]*")
		    (! (looking-at "{\\|\\w"))
		)
	    )
	)
	(goto-character ~TAGS-start-point)
    )
    (goto-character ~TAGS-word-start)
    (set-mark)
    (forward-word)
)

(defun add-all-tags
    (
	~TAGS-pattern (get-tty-string ": add-all-tags (pattern) ")
	~TAGS-pattern-processor (concat "~TAGS-default-pattern-processor")
    )
    ~TAGS-fn ~TAGS-cff ~TAGS-dcf
    (setq ~TAGS-dcf default-case-fold-search)
    (setq default-case-fold-search 1)
    (setq ~TAGS-cff case-fold-search)
    (setq case-fold-search 1)
    
    (setq ~TAGS-fn (~TAGS-mk-fn))
    (save-excursion
	(error-occurred
	    (to-tag-buffer)
	    (beginning-of-file)
	    (search-forward (concat "\^_" ~TAGS-fn))
	    (beginning-of-line)
	    (next-line)
	    (set-mark)
	    (while (& (! (eobp)) (= (following-char) '^A'))
		(next-line)
	    )
	    (erase-region)
	)
    )
    (save-excursion ~TAGS-start-point
	(error-occurred ~TAG-tags-found ~TAG-processor-var
	    (beginning-of-file)
	    (setq ~TAG-tags-found 0)
	    (setq ~TAG-processor-var 0)
	    (setq ~TAGS-start-point (dot))

	    (while 1
		(goto-character ~TAGS-start-point)
		(execute-mlisp-line (concat "(" ~TAGS-pattern-processor " \"" ~TAGS-pattern "\")"))
		(save-excursion (add-tag))
		(setq ~TAG-tags-found (+ ~TAG-tags-found 1))
	    )
	)
    )
    (setq case-fold-search ~TAGS-cff)
    (setq default-case-fold-search ~TAGS-dcf)
    (novalue)
    
)

(defun
    (add-typed-tags
	~TAGS-ext ~TAGS-pattern
	(setq ~TAGS-ext (case-string-lower (file-format-string "%1ft" (current-file-name))))
	(progn
	    ~TAGS-arg
	    ~TAGS-proccesor
	    (setq ~TAGS-arg
		(if
		    (= ~TAGS-ext (concat tag-language-type)) tag-language-pattern
		    ;"^\\S\\C[A-Za-z_$*][A-Za-z0-9_$* \t\n]*([A-Za-z0-9_$*, \t\n()]*[ \t\n]*{"
		    (|
			(= ~TAGS-ext "h") 
			(= ~TAGS-ext "hpp") 
			(= ~TAGS-ext "hxx") 
			(= ~TAGS-ext "hh") 
			(= ~TAGS-ext "H") 
			(= ~TAGS-ext "c")
			(= ~TAGS-ext "C") 
			(= ~TAGS-ext "cpp") 
			(= ~TAGS-ext "cxx")
		    ) ""
		    (= ~TAGS-ext "ml") "^(defun[ \t\n]*[(]*"
		    (= ~TAGS-ext "py") "^[ \t]*def\\\\b\\\\|^[ \t]*class\\\\b"
		    (= ~TAGS-ext "bli") "^[ \t]*routine.*\\\\|^[ \t]*global routine.*"
		    (= ~TAGS-ext "b16") "^[ \t]*routine.*\\\\|^[ \t]*global routine.*"
		    (= ~TAGS-ext "b32") "^[ \t]*routine.*\\\\|^[ \t]*global routine.*"
		    (= ~TAGS-ext "pas") "^[ \t]*FUNCTION\\\\b.*\\\\|^[ \t]*PROCEDURE\\\\b.*"
		    (= ~TAGS-ext "mar") "^.*\\\\.entry\\\\|^[a-zA-Z].*:"
		    (= ~TAGS-ext "rno") "^\\\\.hl\\\\|^\\\\.ch\\\\|^\\\\.ax"
		    (= ~TAGS-ext "for") "^[ 	][ 	]*subroutine\\\\|^[ 	][ 	]*[a-zA-Z]*[ 	][ 	]*function"
		    (get-tty-string
			(concat
			    "Cannot tags files with a type of ."
			    ~TAGS-ext
			    "-- press enter to continue tagging other files"
			)
		    )
		)
	    )
	    (setq ~TAGS-proccesor
		(if
		    (|
			(= ~TAGS-ext "h") 
			(= ~TAGS-ext "hpp") 
			(= ~TAGS-ext "hxx") 
			(= ~TAGS-ext "hh") 
			(= ~TAGS-ext "H") 
			(= ~TAGS-ext "c")
			(= ~TAGS-ext "C") 
			(= ~TAGS-ext "cpp") 
			(= ~TAGS-ext "cxx")
		    )
		    "~TAGS-c-pattern-processor"
		    "~TAGS-default-pattern-processor"
		)
	    )
	    (add-all-tags ~TAGS-arg ~TAGS-proccesor)
	    (novalue)
	)
    )
)
    
(defun
    (recompute-all-tags
	~TAGS-cff ~TAGS-dcf
	(save-window-excursion
	    ~TAGS-fn
	    (setq ~TAGS-dcf default-case-fold-search)
	    (setq default-case-fold-search 1)
	    (setq ~TAGS-cff case-fold-search)
	    (setq case-fold-search 1)
	    (if (interactive)
		(error-occurred 
		    (message "Recomputing...")
		    (sit-for 0)
		    (delete-buffer "Tag list")
		)
	    )
	    (save-excursion
		(to-tag-buffer)
		(beginning-of-file)
		(while (! (error-occurred (re-search-forward "\^_\\(.*\\)")))
		    (region-around-match 1)
		    (setq ~TAGS-fn (region-to-string))
		    (save-window-excursion
			(if (! (error-occurred (visit-file (concat tag-directory ~TAGS-fn))))
			    (progn
				(delete-other-windows)
				(message "Tagging " ~TAGS-fn " ...") (sit-for 0)
				(add-typed-tags)
			    )
			)
		    )
		    (next-line)
		    (beginning-of-line)
		)
	    )
	)
	(if (interactive)
	    (save-excursion
		(list-tags)
		(message "Recomputing... Done!")
	    )
	)
	(setq case-fold-search ~TAGS-cff)
	(setq default-case-fold-search ~TAGS-dcf)
	(novalue)
    )
)
    
(defun
    (make-tag-table ~TAGS-fns ~TAGS-cff ~TAGS-dcf
	(setq ~TAGS-dcf default-case-fold-search)
	(setq default-case-fold-search 1)
	(setq ~TAGS-cff case-fold-search)
	(setq case-fold-search 1)
	(setq ~TAGS-fns (arg 1 ": make-tag-table (from filenames) "))
	(if (= operating-system-name "vms")
	    (save-excursion
		(temp-use-buffer "*TAGP*")
		(erase-buffer)
		(insert-string ~TAGS-fns)
		(beginning-of-file)
		(if (error-occurred (search-forward ";"))
		    (setq ~TAGS-fns (concat ~TAGS-fns ";0"))
		)
		(delete-buffer "*TAGP*")
	    )
	)
	(message "Making tags...")
	(sit-for 0)
	(save-window-excursion
	    (to-tag-buffer)
	    (erase-buffer)
	    (~TAGS-dir ~TAGS-fns)
	    (end-of-file)
	    (recompute-all-tags)
	    (beginning-of-file)
	)
	(if (interactive)
	    (save-excursion
		(list-tags)
		(message "Making tags... Done!")
	    )
	)
	(setq case-fold-search ~TAGS-cff)
	(setq default-case-fold-search ~TAGS-dcf)
	(novalue)
    )
)

(defun
    (visit-function ~TAGS-func
	(save-window-excursion
	    (if prefix-argument-provided
		(setq ~TAGS-func (arg 1 ": visit-function "))
		(save-excursion
		    (error-occurred (forward-character))
		    (backward-word)
		    (set-mark)
		    (forward-word)
		    (setq ~TAGS-func (region-to-string))
		    (unset-mark)
		)
	    )
	    (goto-tag ~TAGS-func)
	    (message "Invoke exit-emacs to return")
	    (recursive-edit)
	)
	(novalue)
    )
)
    
(error-occurred (execute-mlisp-file "tags.key"))

