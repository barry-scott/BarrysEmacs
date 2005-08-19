; 
;  form.ml
; 
; 
;   Barry A. Scott  17-mar-1989	Add the form handling from notes to this
;				module.
; 

;
;   Puts up a form in buffer (arg 1) for the user to fill in. The mode lines
;   is set from (arg 2).
; 
;   After the form is filled confirm is used with (arg 3) being its prompt
;   and (arg 4) being the default. This is the result of form fill.
;   if (arg 3) is the null string confirm is not called.
;
;   (arg 5)  is the commands to setup the buffer prior to letting the user
;   gets control. Use (novalue) is the default setup is sufficient.
; 
;   (arg 6) is used to control if the form window is shrink to the
;   size of the form.
; 
;   The following parameters are in sets of 4
;	(arg n)	    - prompt
;	(arg n+1)   - default
;	(arg n+2)   - type s=string, b=boolean
;	(arg n+3)    - call back to process result e.g. (setq x ~result)
; 
(declare-buffer-specific
    form-end-of-form		; marker set at the end of the form
)
(setq-default form-end-of-form 0)
(define-keymap "form-null-keymap")
(defun
    (form-fill-form
	~argnum ~result ~type ~prompt
	~fields
	~old-keymap
	
	(setq ~fields (array 1 2 7 (nargs)))
	(save-window-excursion
	    (if (& (> window-size 4) (arg 6))
		(progn
		    (split-current-window)
		    (setq window-size (/ (nargs) 4))
		)
	    )
	    (switch-to-buffer (arg 1))
	    (setq ~old-keymap current-local-keymap)
	    (setq mode-line-format (arg 2))
	    
	    ;
	    ; Create the template
	    ;
	    (setq ~argnum 7)
	    (end-of-file)
	    (error-occurred
		(goto-character
		    (use-variables-of-buffer current-buffer-name
			form-end-of-form)))
	    (set-mark)
	    (beginning-of-file)
	    (erase-region)
	    (unset-mark)
	    (while (< ~argnum (nargs))
		(progn
		    (setq ~prompt (arg ~argnum))
		    (if (!= ~prompt "")
			(progn
			    (insert-string ~prompt)
			    (to-col 25)
			    (newline-and-backup)
			    (setq-array ~fields 1 ~argnum (dot))
			    (setq-array ~fields 2 ~argnum (right-marker (dot)))
			    (insert-string
				(concat
				    (if (= "b" (arg (+ 2 ~argnum)))
					(if (arg (+ 1 ~argnum)) "y" "n")
					(arg (+ 1 ~argnum))
				    )
				)
			    )
			    (forward-character)
			)
		    )
		    (setq ~argnum (+ 4 ~argnum))
		)
	    )
	    (use-variables-of-buffer current-buffer-name
		(progn
		    ; 
		    ; remember the end of the form
		    ;
		    (setq form-end-of-form (dot))
		    ;
		    ; position at the first field
		    ;
		    (beginning-of-file)
		    (goto-character (fetch-array ~fields 1 7))
		    
		    ;
		    ; Let the user fill in the form
		    ;
		    (use-local-map "form-null-keymap")
		    (save-window-excursion
			(arg 5)		; callers buffer setup actions
			(recursive-edit)
		    )
		)
	    )
	    ;
	    ; Extract the results
	    ;
	    (if
		(error-occurred
		    (beginning-of-file)
		    (setq ~argnum 7)
		    (while (< ~argnum (nargs))
			(progn
			    (setq ~prompt (arg ~argnum))
			    (if (!= ~prompt "")
				(progn
				    (setq ~type (arg (+ 2 ~argnum)))
				    ;
				    ; position to the start of the field
				    ; and set region around the fields value
				    (goto-character
					(fetch-array ~fields 2 ~argnum))
				    (set-mark)
				    (goto-character
					(fetch-array ~fields 1 ~argnum))
				    ;
				    ; process the type to get the field into ~result
				    ;
				    (if
					(= "s" ~type)
					(progn
					    ;
					    ; string arg
					    ;
					    (setq ~result (region-to-string))
					)
					(= "b" ~type)
					;
					; boolean arg
					;
					(setq ~result (looking-at "[ \t]*[yY]"))
					(= "n" ~type)
					(progn
					    ;
					    ; numeric arg
					    ;
					    (setq ~result (+ (region-to-string)))
					)
					(= "t" ~type)
					(novalue); ignore text items
					(error-message "form-fill-form unknown option " ~type)
				    )
				    ;
				    ; return the result to the caller
				    ;
				    (arg (+ 3 ~argnum))
				)
			    )
			    (setq ~argnum (+ 4 ~argnum))
			)
		    )
		)
		(progn
		    (error-occurred (use-local-map ~old-keymap))
		    (unset-mark)
		    (error-message error-message)
		)
	    )
	    (error-occurred (use-local-map ~old-keymap))
	    (unset-mark)
	    ;
	    ;	See if we need to confirm the values filled
	    ;
	    (if (!= "" (setq ~prompt (arg 3)))
		(form-confirm-action ~prompt (arg 4))
		1
	    )
	)
    )
)
(defun form-confirm-action
    (
	~prompt
	~default
    )
    ~do-it

    (setq ~do-it (get-tty-string (concat ~prompt " [" ~default "]: ")))
    (if
	(= ~do-it "")
	(setq ~do-it ~default)
    )
    (|
	(= "y" (substr ~do-it 1 1))
	(= "Y" (substr ~do-it 1 1))
    )
)

; 
; form handling used initially by Emacs CMS.
; 
(progn
    (save-window-excursion
        (switch-to-buffer "Form")
	(define-keymap "Form")
	(use-local-map "Form")
	(local-bind-to-key "form-next-field" "\r")
	(local-bind-to-key "form-next-field" "\t")
	(local-bind-to-key "form-next-field" "\(down)")
	(local-bind-to-key "form-previous-field" "\^h")
	(local-bind-to-key "form-previous-field" "\(up)")
	(local-bind-to-key "form-quit" "\^Y")
	(local-bind-to-key "form-quit" "\^G")
	(local-bind-to-key "form-quit" "\^C")
    )
    (declare-global
        form-depth
	form-quit
    )
    (setq form-depth 0)
(defun form-next-field ()
    (next-line)
    (beginning-of-line)
    (if (error-occurred (search-forward ":"))
        (progn
	    (beginning-of-file)
	    (next-line)
	    (next-line)
	    (search-forward ":")
	)
    )
    (while (< (current-column) start-col) (forward-character))
)
(defun form-quit ()
    (setq form-quit 1)
    (if (> (recursion-depth) 0) (exit-emacs))
)
(defun form-previous-field ()
    (previous-line)
    (end-of-line)
    (if (error-occurred (search-reverse ":"))
        (progn
	    (end-of-file)
	    (previous-line)
	    (previous-line)
	)
    )
    (beginning-of-line)
    (while (< (current-column) start-col) (forward-character))
)
(defun
    (form
	start-col
	start-line
	start-dot
        count
	i
	heading
	exec dot lines
	buffer
	old-form-quit
	(setq old-form-quit form-quit)
	(setq exec (concat "(" (arg 4) " "))
	(save-window-excursion
	    (setq buffer (concat "Form-" form-depth))
	    (setq form-depth (+ form-depth 1))
	    (setq form-quit 0)
	    (setq start-dot 1)
	    (setq lines (+ (/ (- (setq count (nargs)) 4) 2) 5))
	    (setq start-line (+ 1 (arg 1)))
	    (setq start-col (+ (arg 2) 14))
	    (setq heading (arg 3))
	    (pop-to-buffer "Minibuf")
	    (previous-window)
	    (if (> window-size 3)
		(progn
	    	    (split-current-window)
		    (while (! (error-occurred (shrink-window))) (novalue))
		)
	    )
	    (switch-to-buffer buffer)
	    (error-occurred
		(while (< window-size lines)
		    (enlarge-window)
		)
	    )
	    (erase-buffer)
	    (setq mode-line-format "")
	    (setq replace-mode 1)
	    (setq display-C1-as-graphics 1)
	    (setq highlight-region 0)
	    (insert-character '\n')
	    (use-local-map "Form")
	    (setq i (/ (- 80 (length heading)) 2))
	    (while (< (current-column) i) (insert-character ' '))
	    (insert-string heading)
	    (insert-string "\n        Œ‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‹\n")
	    (setq i 5)
	    (while (< i count)
		(insert-string "        ˜ ")
		(insert-string (arg i))
		(insert-string ":")
		(while (< (current-column) start-col) (insert-character ' '))
		(if (= start-line (/ i 2))
		    (setq start-dot (+ (dot)))
		)
		(insert-string (arg (+ i 1)))
		(while (< (current-column) 73) (insert-character ' '))
		(insert-string "˜\n")
		(setq i (+ i 2))
	    )
	    (insert-string "        ‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘Š\n")
	    (goto-character start-dot)
	    (message "Fill in the form and use exit-emacs")
	    (save-window-excursion 
	        (recursive-edit)
	    )
	    (setq form-depth (- form-depth 1))
	    (if (! form-quit)
	        (progn
		    (beginning-of-file)
		    (next-line)(next-line)(next-line)
		    (setq i 5)
		    (while (< i count)
			(beginning-of-line)
			(while (< (current-column) start-col) (forward-character))
			(setq dot (dot))
			(end-of-line)
			(if (!= (preceding-char) '\230')(insert-string "\230"))
			(goto-character (+ dot))
			(re-search-forward " *\230$")
			(region-around-match 0)
			(erase-region)
			(set-mark)
			(goto-character (+ dot))
			(setq exec (concat exec " \"" (region-to-string) "\""))
			(next-line)
			(setq i (+ i 2))
		    )
		)
	    )
	)
	(if (! form-quit)
	    (progn
		(setq exec (concat exec ")"))
		(execute-mlisp-line exec)
	    )
	)
	(setq i (! form-quit))
	(setq form-quit old-form-quit)
	i
    )
)
)
