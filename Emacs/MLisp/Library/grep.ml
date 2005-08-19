; 
; grep.ml
;   Copyright (c) 1993-2000 Barry A. Scott
; 
; Grep for emacs that does not depend on any external programs
; and offers the full power of Emacs regular expressions
; 
; Defines:
;   grep-in-files aka grep
;   grep-in-buffers
;   grep-current-buffer
; 
; 
(declare-global
    grep-options
    grep-search-pattern
    grep-file-list
    grep-results-buffer-name
    grep-exclude-file-list
    grep-matched-string-colour
    grep-name-colour
    grep-position-to-match-column

    grep-current-buffer-options
    grep-current-buffer-search-pattern

    grep-in-buffers-options
    grep-in-buffers-search-pattern
)

(declare-buffer-specific
    ~grep-search-pattern
    ~grep-goto-method
)

(if (= (type-of-expression grep-options) "integer")
    (progn
	(setq grep-options "x")
	(setq grep-search-pattern "")
	(setq grep-file-list "")
	(setq grep-results-buffer-name "grep-results")
	(setq grep-exclude-file-list "*._* *.exe *.obj *.pch *.pdb *.zip *.tar.gz *.tgz *.dll *.sbr *.pyd *.ico *.ncb *.lnk *.bsc *.ilk *.exp *.lib *.aps *.opt")
	(setq grep-matched-string-colour 1)	; override in grep.key
	(setq grep-name-colour 2)		; override in grep.key
        (setq grep-position-to-match-column 1)	; override in grep.key

	(setq grep-in-buffers-options "f")
	(setq grep-in-buffers-search-pattern "")

	(setq grep-current-buffer-options "")
	(setq grep-current-buffer-search-pattern "")

	(setq-default ~grep-search-pattern "")
	(setq-default ~grep-goto-method "")
    )
)

;--------------------------------------------------------------------------------
; 
; grep-in-files
; 
;--------------------------------------------------------------------------------

; supplied for backwards compatibility
(defun grep
    (~options (get-tty-string ": grep (options s - case-sensitive r - recursive b - binary x - exclude 1-9 - buffer) " grep-options)
	~pattern (get-tty-string ": grep (pattern) " grep-search-pattern)
	~multi-files (get-tty-file ": grep (files,files) " grep-file-list))

    (grep-in-files ~options ~pattern ~multi-files)
)

(defun grep-in-files 
    (~options (get-tty-string ": grep-in-files (options s - case-sensitive r - recursive b - binary x - exclude 1-9 - buffer) " grep-options)
	~pattern (get-tty-string ": grep-in-files (pattern) " grep-search-pattern)
	~multi-files (get-tty-file ": grep-in-files (files,files) " grep-file-list))
    
    ~index
    ~char

    ~file
    ~files
    
    ~start-pos ~end-pos

    ~option-recursive
    ~option-case-sensitive
    ~option-text-file-only
    ~option-results-buffer
    ~option-use-exclude-list

    (setq ~option-recursive 0)
    (setq ~option-case-sensitive 0)
    (setq ~option-text-file-only 1)
    (setq ~option-use-exclude-list 0)
    (setq ~option-results-buffer grep-results-buffer-name)    ; option 0 is this buffer

    (setq ~index 1)
    (while (<= ~index (length ~options))
	~char
	(setq ~char (substr ~options ~index 1))
	(if (= ~char "b")
	    (setq ~option-text-file-only 0)
	    (= ~char "r")
	    (setq ~option-recursive 1)
	    (= ~char "s")
	    (setq ~option-case-sensitive 1)
	    (= ~char "x")
	    (setq ~option-use-exclude-list 1)
	    (& (>= ~char "1") (<= ~char "9"))
	    (setq ~option-results-buffer (concat grep-results-buffer-name "-" ~char))
	    (= ~char "0")
	    (setq ~option-results-buffer grep-results-buffer-name)
	    (error-message "grep: unknown option " ~char)
	)
	(setq ~index (+ ~index 1))
    )

    ; 
    ; save the parameter as defaults for the next grep
    ; 
    (setq grep-options ~options)
    (setq grep-search-pattern ~pattern)
    (setq grep-file-list ~multi-files)

    (pop-to-buffer ~option-results-buffer)
    (setq ~grep-search-pattern ~pattern)    
    (setq ~grep-goto-method "visit-file")

    (use-local-map "grep-keymap")
    (setq current-buffer-journalled 0)
    (erase-buffer)
    (unset-mark)

    ; clear out the colouring
    (apply-colour-to-region 1 (buffer-size) 0)

    (temp-use-buffer "grep-file")
    (erase-buffer)
    (error-occurred (grep-user-setup-grep-file))
    (setq case-fold-search (! ~option-case-sensitive))
    (setq current-buffer-journalled 0)

    (temp-use-buffer ~option-results-buffer)

    (setq ~multi-files (concat ~multi-files ","))
    (setq ~start-pos 0)
    (while
	(progn
	    (setq ~end-pos (string-index-of-string ~multi-files "," ~start-pos))
	    (> ~end-pos 0)
	)
	(setq ~files (string-extract ~multi-files ~start-pos ~end-pos))
	(setq ~start-pos (+ ~end-pos 1))
	(while
	    (progn
		(if ~option-recursive
		    (setq ~file (expand-file-name-recursive ~files))
		    (setq ~file (expand-file-name ~files))
		)
		(setq ~files "")
		(!= ~file "")
	    )
	    ; do
	    (if (~grep-include-file ~file)
		(~grep-one-file ~file)
	    )
	    (end-of-file)
	)
    )
    (beginning-of-file)
    (message "Grep-in-files done")
)

;--------------------------------------------------------------------------------
; 
; grep-in-buffers
; 
;--------------------------------------------------------------------------------
(defun grep-in-buffers 
    (~options (get-tty-string ": grep-in-buffers (options s - case-sensitive f - file-only 1-9 - buffer) " grep-in-buffers-options)
	~pattern (get-tty-string ": grep-in-buffers (pattern) " grep-in-buffers-search-pattern))
    
    ~index
    ~char
    
    ~buffer-name
    
    ~start-pos ~end-pos
    
    ~option-case-sensitive
    ~option-file-buffer-only
    ~option-results-buffer
    
    (setq ~option-case-sensitive 0)
    (setq ~option-file-buffer-only 0)
    (setq ~option-results-buffer grep-results-buffer-name)    ; option 0 is this buffer
    
    (setq ~index 1)
    (while (<= ~index (length ~options))
	~char
	(setq ~char (substr ~options ~index 1))
	(if (= ~char "f")
	    (setq ~option-file-buffer-only 1)
	    (= ~char "s")
	    (setq ~option-case-sensitive 1)
	    (& (>= ~char "1") (<= ~char "9"))
	    (setq ~option-results-buffer (concat grep-results-buffer-name "-" ~char))
	    (= ~char "0")
	    (setq ~option-results-buffer grep-results-buffer-name)
	    (error-message "grep-in-buffers: unknown option " ~char)
	)
	(setq ~index (+ ~index 1))
    )
    
    ; 
    ; save the parameter as defaults for the next grep
    ; 
    (setq grep-in-buffers-options ~options)
    (setq grep-in-buffers-search-pattern ~pattern)
    
    (pop-to-buffer ~option-results-buffer)
    (setq ~grep-goto-method "pop-to-buffer")
    (setq ~grep-search-pattern ~pattern)    
    (use-local-map "grep-buffers-keymap")
    (setq current-buffer-journalled 0)
    (erase-buffer)
    (unset-mark)
    
    ; clear out the colouring
    (apply-colour-to-region 1 (buffer-size) 0)
    
    (setq ~index 0)
    (while
	(progn
	    (setq ~index (+ ~index 1))
	    (< ~index (fetch-array buffer-names 0))
	)
	(setq ~buffer-name (fetch-array buffer-names ~index))
	
	(save-excursion
	    (temp-use-buffer ~buffer-name)
	    (if
		(if ~option-file-buffer-only
		    (= current-buffer-type "file")
		    1
		)
		; don't search grep's buffers
		(if (!= (string-extract ~buffer-name 0 (length grep-results-buffer-name)) grep-results-buffer-name)
		    (~grep-buffer ~buffer-name))
	    )
	)
    )
    
    (beginning-of-file)
    (message "Grep-in-buffers done")
)

;--------------------------------------------------------------------------------
; 
; grep-current-buffer
; 
;--------------------------------------------------------------------------------
(defun grep-current-buffer 
    (~options (get-tty-string ": grep-current-buffer (options s - case-sensitive 1-9 - buffer) " grep-current-buffer-options)
	~pattern (get-tty-string ": grep-current-buffer (pattern) " grep-current-buffer-search-pattern))
    
    ~index
    ~char
    
    ~buffer-name
    
    ~start-pos ~end-pos
    
    ~option-case-sensitive
    ~option-file-buffer-only
    ~option-results-buffer
    
    (setq ~buffer-name (current-buffer-name))
    
    (setq ~option-case-sensitive 0)
    (setq ~option-file-buffer-only 0)
    (setq ~option-results-buffer grep-results-buffer-name)    ; option 0 is this buffer
    
    (setq ~index 1)
    (while (<= ~index (length ~options))
	~char
	(setq ~char (substr ~options ~index 1))
	(if (= ~char "f")
	    (setq ~option-file-buffer-only 1)
	    (= ~char "s")
	    (setq ~option-case-sensitive 1)
	    (& (>= ~char "1") (<= ~char "9"))
	    (setq ~option-results-buffer (concat grep-results-buffer-name "-" ~char))
	    (= ~char "0")
	    (setq ~option-results-buffer grep-results-buffer-name)
	    (error-message "grep-in-buffers: unknown option " ~char)
	)
	(setq ~index (+ ~index 1))
    )
    
    ; 
    ; save the parameter as defaults for the next grep
    ; 
    (setq grep-current-buffer-options ~options)
    (setq grep-current-buffer-search-pattern ~pattern)
    
    (pop-to-buffer ~option-results-buffer)
    (setq ~grep-search-pattern ~pattern)    
    (setq ~grep-goto-method "pop-to-buffer")

    (use-local-map "grep-buffers-keymap")
    (setq current-buffer-journalled 0)
    (erase-buffer)
    (unset-mark)
    
    ; clear out the colouring
    (apply-colour-to-region 1 (buffer-size) 0)
    
    (save-excursion
	(temp-use-buffer ~buffer-name)
	; don't search grep's buffers
	(if (!= (string-extract ~buffer-name 0 (length grep-results-buffer-name)) grep-results-buffer-name)
	    (~grep-buffer ~buffer-name))
    )
    (beginning-of-file)
    (message "Grep-current-buffer done")
)


(defun
    ~grep-one-file(~file)
    
    (pop-to-buffer ~option-results-buffer) 
    (end-of-file)
    (save-excursion
	(temp-use-buffer "grep-file")
	(erase-buffer)
	(error-occurred (insert-file ~file))
	(if ~option-text-file-only
	    (progn
		(if (< (buffer-size) 1024)
		    (end-of-file)
		    (goto-character 1024)
		)
		; NUL means its a binary file
		(if (! (error-occurred (search-reverse "\000")))
		    (erase-buffer)
		)
	    )
	)
	(~grep-buffer ~file)
    )
)

(defun
    ~grep-buffer(~name)

    ~old-case-fold-search
    (setq ~old-case-fold-search case-fold-search)
    (setq case-fold-search (! ~option-case-sensitive))

    (message "grep working on " ~name "...")
    (sit-for 0)

    (beginning-of-file)
    ; compile RE
    (ere-looking-at ~pattern)
    ; use compiled RE in loop
    (while (! (error-occurred (ere-search-forward "")))
	(save-excursion ~pre-match ~match ~post-match ~line
	    (setq ~line (current-line-number))
	    (region-around-match 0)
	    (setq ~match (region-to-string))
	    (save-excursion
		(beginning-of-line)
		(setq ~pre-match (region-to-string))
	    )
	    (exchange-dot-and-mark)
	    (end-of-line)
	    (setq ~post-match (region-to-string))

	    (pop-to-buffer ~option-results-buffer)
	    (end-of-file)
	    ; only insert the name if a match is found
	    (if (!= ~name "")
		(progn
		    (if (! (bobp))
			(insert-string "\n")
		    )
		    (set-mark)
		    (insert-string ~name)
		    (apply-colour-to-region (mark) (dot) grep-name-colour)
		    (insert-string "\n")
		    (setq ~name "")
		)
	    )
	    (insert-string ~line "\t" ~pre-match)
	    (set-mark)
	    (insert-string ~match)
	    (apply-colour-to-region (mark) (dot) grep-matched-string-colour)
	    (insert-string ~post-match "\n")
	)
	(end-of-line)
	(error-occurred (forward-character))
    )
    (setq case-fold-search ~old-case-fold-search)
)

(defun
    ~grep-include-file(~file)
    ~include
    (setq ~include 1)
    
    (if ~option-use-exclude-list
	(progn
	    ~start-pos ~end-pos ~pattern ~filename ~list

	    (setq ~list (concat grep-exclude-file-list " "))
	    (setq ~filename (file-format-string "%fa" ~file))
	    (setq ~start-pos 0)
	    
	    (while
		(&
		    ~include
		    (progn
			(setq ~end-pos (string-index-of-string ~list " " ~start-pos))
			(> ~end-pos 0)
		    )
		)
		(setq ~pattern (string-extract ~list ~start-pos ~end-pos))
		(setq ~start-pos (+ ~end-pos 1))
		
		(error-occurred	; only get an error in pre 7.0.84 emacs
		    (setq ~include (! (match-wild ~filename ~pattern)))
		)
	    )
	)
    )
    ~include
)


(defun
    (grep-visit-match
	(~grep-goto-match ~grep-goto-method grep-position-to-match-column)
    )
)

(defun
    (grep-pop-to-match
	(~grep-goto-match ~grep-goto-method grep-position-to-match-column)
    )
)

(defun
    ~grep-goto-match(~method ~to-match)
    ~container ~line ~pattern
    
    (setq ~container "")
    (setq ~line 0)
    (setq ~pattern ~grep-search-pattern)
    (save-excursion
	(beginning-of-line)
	(if (ere-looking-at "\\d+")
	    (setq ~line
		(+
		    (progn
			(region-around-match 0)
			(region-to-string)
		    )
		)
	    )
	)
	(forward-character)
	(ere-search-reverse "^\\D")
	(beginning-of-line)
	(set-mark)
	(end-of-line)
	(setq ~container (region-to-string))
    )	
    (execute-mlisp-line (concat "(" ~method " ~container)"))
    (if ~line
	(progn
	    (goto-line ~line)
	    (if ~to-match
		(if (length ~pattern)
		    (error-occurred
			(goto-character
			    (save-excursion
				(re-search-forward ~pattern)
				(region-around-match 0)
				(exchange-dot-and-mark)
				(dot)
			    )
			)
		    )
		)
	    )
	)
    )
)

(defun grep-replace-string
    (~pattern (get-tty-string ": grep-replace (pattern) ")
	~replace (get-tty-string ": grep-replace (replacement) "))

    ~option-results-buffer

    (if (= (substr (current-buffer-name) 1 (length grep-results-buffer-name)) grep-results-buffer-name)
	(setq ~option-results-buffer (current-buffer-name))
	(setq ~option-results-buffer grep-results-buffer-name)
    )

    (pop-to-buffer ~option-results-buffer)
    
    (while (! (eobp))
	(beginning-of-line)
	(if (ere-looking-at "\\d+")
	    (progn
		(~grep-goto-match ~grep-goto-method 0)
		
		(save-excursion
		    (set-mark)
		    (end-of-line)
		    (save-restriction
			(narrow-region)
			(beginning-of-line)
			(error-occurred (ere-replace-string ~pattern ~replace))
		    )
		)
	    )
	)
	(pop-to-buffer ~option-results-buffer)
	(next-line)
    )
)

(save-excursion
    (switch-to-buffer grep-results-buffer-name)
    (error-occurred (define-keymap "grep-keymap"))
    (use-local-map "grep-keymap")
    (execute-mlisp-file "grep.key")

    (error-occurred (define-keymap "grep-buffers-keymap"))
    (use-local-map "grep-buffers-keymap")
    (execute-mlisp-file "grep-in-buffers.key")
)
