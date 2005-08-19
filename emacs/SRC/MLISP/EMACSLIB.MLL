[abbrev.key]
(progn
    (bind-to-key "define-global-abbrev-in-buffer" "\^xg")
    (bind-to-key "define-local-abbrev-in-buffer" "\^xl")
    (bind-to-key "define-global-abbrev-for-region" "\^xr")
    (bind-to-key "define-local-abbrev-for-region" "\^xs")
    (bind-to-key "edit-all-abbrevs" "\^xa")
    (bind-to-key "edit-local-abbrev" "\^xw")
)
[abbrev.ml]
; This file defines some functions to make defining and using the abbrev
; system in EMACS easier. The following functions are provided
; 
; define-global-abbrev-in-buffer which takes the word left of dot, and uses
; 				 it as the abbreviation. It then asks what
; 				 the expansion should be.
; define-local-abbrev-in-buffer  which does the same as the above, but uses
;   				 the local abbrev table.
; list-local-abbrevs		 which displays a list of the abbrevs and
; 				 their expansions which apply to the current
; 				 buffer. You get into a recursive edit.
; edit-local-abbrevs	 	 which gives you a list of the current
; 				 buffer's abbrevs, and then lets you edit
; 				 them in a recursive edit. When you exit
; 				 from the recursive edit, the chanes you
; 				 have made take effect.
; list-all-abbrevs		 same as list-local-abbrevs, but displays all
; 				 abbrev tables.
; edit-all-abbrevs		 same as edit-local-abbrevs but displays all
; 				 abbrev tables.
; define-global-abbrev-for-region
; 				 This command takes the current region, and
; 				 asks the user to specify an abbrev for it.
; 				 The abbrev is defined in the global table.
; define-local-abbrev-for-region This command takes the current region, and
; 				 asks the user to specify an abbrev for it.
; 				 The abbrev is defined in the current local
; 				 abbrev table.
; 

(progn
  (defun
    (~A-help
	(save-window-excursion
	    (switch-to-buffer "Help")
	    (erase-buffer)
	    (setq mode-line-format "")
	    (insert-string
		(concat
		    "ABBREV LIST: list of abbreviation definitions\n\n"
		    "    <sp>     next page\n"
		    "    <bs>     previous page\n"
		    "    n        next line\n"
		    "    p        previous line\n" 
		    "    e        move to end of list\n"
		    "    b        move to beginning of list\n"
		    "    q        quit (you will be prompted to confirm)\n"
		    "    x        execute and EMACS command\n\n"
		    "non-printing characters perform their usual functions.\n\n\n"
		)
	    )
	    (message "-- Type any character to view the abbreviation list --")
	    (get-tty-character)
	    (setq mode-line-format default-mode-line-format)
	)
    )
    (~A-illegal (illegal-operation))
    (define-global-abbrev-in-buffer
	(save-excursion
	    (backward-word)
	    (set-mark)
	    (forward-word)
	    (define-global-abbrev
		(region-to-string)
		(arg 1 (concat ": define-global-abbrev-in-buffer " (region-to-string) " "))
	    )
	)
    )
    (define-local-abbrev-in-buffer
	(save-excursion
	    (backward-word)
	    (set-mark)
	    (forward-word)
	    (define-local-abbrev
		(region-to-string)
		(arg 1 (concat ": define-local-abbrev-in-buffer " (region-to-string) " "))
	    )
	)
    )
    (~A-list-buffer-abbrevs
	~A-remove ~A-buffer-abbrev
	(setq ~A-remove -1)
	(setq ~A-buffer-abbrev current-buffer-abbrev-table)
	(pop-to-buffer "Abbrev list")
	(delete-other-windows)
	(erase-buffer)
	(insert-file "SYS$SCRATCH:TEMP.TMP")
	(unlink-file "SYS$SCRATCH:TEMP.TMP;0")
	(beginning-of-file)
	(while (! (error-occurred (re-search-forward "^[^ ]")))
	    (beginning-of-line)
	    (set-mark)
	    (if (>= ~A-remove 0)
		(progn
		    (goto-character ~A-remove)
		    (erase-region)
		)
	    )
	    (end-of-line)
	    (if (|
		    (= (region-to-string) "global")
		    (= (region-to-string) ~A-buffer-abbrev)
		)
	        (setq ~A-remove -1)
		(setq ~A-remove (progn (beginning-of-line) (dot)))
	    )
	    (next-line)
	    (beginning-of-line)
	)
	(end-of-file)
	(set-mark)
	(if (>= ~A-remove 0)
	    (progn
		(goto-character ~A-remove)
		(erase-region)
	    )
	)
        (beginning-of-file)
    )
    (list-local-abbrevs
	~A-buffer
	(setq ~A-buffer current-buffer-name)
	(write-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	(save-window-excursion
	    (~A-list-buffer-abbrevs)
	    (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
	    (setq mode-string "More")
	    (message "Use exit-emacs to continue editing.")
	    (use-local-map "more-map")
	    (recursive-edit)
	)
	(novalue)
    )
    (edit-local-abbrev
	~A-buffer
	(setq ~A-buffer current-buffer-name)
	(write-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	(save-window-excursion
	    (~A-list-buffer-abbrevs)
	    (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
	    (setq mode-string "Edit")
	    (message "Use exit-emacs to define the abbrevs.")
	    (use-local-map "default-global-keymap")
	    (recursive-edit)
	    (write-named-file "SYS$SCRATCH:TEMP.TMP")
	    (quietly-read-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	    (unlink-file "SYS$SCRATCH:TEMP.TMP;0")
	)
	(novalue)
    )
    (list-all-abbrevs
	~A-buffer
	(setq ~A-buffer current-buffer-name)
	(write-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	(save-window-excursion
	    (pop-to-buffer "Abbrev list")
	    (delete-other-windows)
	    (erase-buffer)
	    (insert-file "SYS$SCRATCH:TEMP.TMP")
	    (unlink-file "SYS$SCRATCH:TEMP.TMP;0")
	    (beginning-of-file)
	    (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
	    (setq mode-string "More")
	    (message "Use exit-emacs to continue editing.")
	    (use-local-map "more-map")
	    (recursive-edit)
	)
	(novalue)
    )
    (edit-all-abbrevs
	~A-buffer
	(setq ~A-buffer current-buffer-name)
	(write-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	(save-window-excursion
	    (pop-to-buffer "Abbrev list")
	    (delete-other-windows)
	    (erase-buffer)
	    (insert-file "SYS$SCRATCH:TEMP.TMP")
	    (unlink-file "SYS$SCRATCH:TEMP.TMP;0")
	    (beginning-of-file)
	    (setq mode-line-format (concat "%[Abbrevs %M (%m%a%c%r) " ~A-buffer "  %p%]"))
	    (setq mode-string "Edit")
	    (message "Use exit-emacs to define the abbrevs.")
	    (use-local-map "default-global-keymap")
	    (recursive-edit)
	    (write-named-file "SYS$SCRATCH:TEMP.TMP")
	    (quietly-read-abbrev-file "SYS$SCRATCH:TEMP.TMP")
	    (unlink-file "SYS$SCRATCH:TEMP.TMP;0")
	)
	(novalue)
    )
    (~A-check-abbrev
	~A-fail
	~A-abbrev
	(save-window-excursion
	    (switch-to-buffer "~A")
	    (setq ~A-abbrev (arg 1))
	    (insert-string ~A-abbrev)
	    (beginning-of-file)
	    (setq ~A-fail (error-occurred (re-search-forward "[\n 	]")))
	)
	(delete-buffer "~A")
	(if (! ~A-fail)
	    (error-message "abbrev \"" ~A-abbrev "\" is illegal.")
	)
    )
    (~A-check-region
	(save-window-excursion
	    (save-restriction
		(narrow-region)
		(beginning-of-file)
		(if (! (error-occurred (search-forward "\n")))
		    (error-message "Newlines are not allowed in abbrevs.")
		)
	    )
	)
    )
    (define-global-abbrev-for-region
	~A-arg
	(~A-check-region)
	(setq ~A-arg
	    (arg 1 ": define-global-abbrev-for-region ")
	)
	(~A-check-region)
	(~A-check-abbrev ~A-arg)
	(define-global-abbrev ~A-arg (region-to-string))
	(novalue)
    )
    (define-local-abbrev-for-region
	~A-arg
	(~A-check-region)
	(setq ~A-arg
	    (arg 1 ": define-global-abbrev-for-region phrase ")
	)
	(~A-check-region)
	(~A-check-abbrev ~A-arg)
	(define-local-abbrev ~A-arg (region-to-string))
	(novalue)
    )
  )
    (save-excursion
	i
	(temp-use-buffer "keymap hack")
	(define-keymap "more-map")
	(use-local-map "more-map")
	(setq i ' ')
	(while (<= i '~')
	    (local-bind-to-key "~A-illegal" (char-to-string i))
	    (setq i (+ i 1))
	)
	(local-bind-to-key "next-page" " ")
	(local-bind-to-key "previous-page" "\^h")
	(local-bind-to-key "next-line" "n")
	(local-bind-to-key "previous-line" "p")
	(local-bind-to-key "execute-extended-command" "x")
	(local-bind-to-key "exit-emacs" "\e")
	(local-bind-to-key "exit-emacs" "q")
	(local-bind-to-key "~A-help" "?")
	(local-bind-to-key "beginning-of-file" "b")
	(local-bind-to-key "end-of-file" "e")
	(delete-buffer "keymap hack")
	(novalue)
    )
    (error-occurred (execute-mlisp-file "abbrev.key"))
)
[addlog.ml]
(defun
    (add-log
	~log-file
	~nls
	(setq ~log-file (arg 1 ": add-log "))
	(setq ~nls "\n\n")
	(save-window-excursion
	    (switch-to-buffer "Log File")
 	    (setq current-buffer-file-name ~log-file)
	    (if (error-occurred (read-file ~log-file))
		(progn
		    (setq ~nls "")
		    (message
			(concat
			    "Creating new Log file \""
			    (current-file-name)
			    "\"")
		    )
		)
	    )
	    (text-mode)
	    (~TM-set-left-margin 4)
	    (~TM-set-right-margin 64)
	    (setq mode-string "Log file edit")
	    (end-of-file)
	    (insert-string ~nls)
	    (insert-string "Entry for ")
	    (insert-string (current-time))
	    (insert-string " made by ")
	    (insert-string (users-login-name))
	    (insert-string " at ")
	    (insert-string (system-name))
	    (newline)(newline)
	    (to-col 4)
	    (recursive-edit)
	    (temp-use-buffer "main")
	    (delete-buffer "Log File")
	)
    )
)
[addlog.mlp]
(defun
    (addlog-com
	(load "addlog")
	(add-log
	    (if (> (argc) 1) (argv 1) (arg 1 "Enter name of log file "))
	)
	(exit-emacs)
    )
)
[area.ml]
(progn
;	+----------------+----------------------------------------------+
;	| File Name:	 | 						|
;	| Edit Version:	 | 12						|
;	| Date Modified: | Wed Jun 22 13:18:10 1983			|
;	| Updated By:	 | SURTEES					|
;	| From Node:	 | _SIVA::					|
;	+----------------+----------------------------------------------+
;
; These routines were written to allow areas to be moved from one area
; another.  I have delete-area bound to ESC-w and insert-area to ESC-y.
;
; What delete-area effectively does is find the top-left corner and
; bottom-right corner of an area between mark and current cursor position
; and deletes it.  If the left-column and right-column are the same, then
; the right-hand column is assumed to be end-of-line for each line.
;
; The contents of this area are placed in a buffer called "Area" where they
; can be retreived with the insert-area command.  As the area is inserted,
; the trailing white space will be deleted if it at the end of the line,
; except on the last line of inserted text where white space is added.
; The reason for this is so that the mark can be set which will allow a
; delete-area to delete what was just inserted.
;
; If insert-area is given an argument of 4 (^U), then the lines in the Area
; buffer will be cycled into the buffer between dot and mark.
;
; Be careful with tabs as they may cause some strange results.
; 
;
(defun
    (delete-area		; Delete a two-dimensional area
	top-left		; Top left corner position	  
	top-line         	; Position of 1st character in top line
	bottom-line		; Position of 1st character in bottom line
	left-column		; Column number of left side of area
	right-column		; Column number of right side of area + 1
	number-lines		; Number of lines to be affected 
	line-counter		; Temporary variable for counting lines
	(progn
	    (setq bottom-line (dot))
	    (setq right-column (current-column))
	    (exchange-dot-and-mark)
	    (setq top-line (dot))
	    (setq left-column (current-column))
	    (if (< bottom-line top-line); Check to see that bottom > top
		(progn
		    (setq bottom-line (dot))
		    (setq right-column (current-column))
		    (exchange-dot-and-mark)
		    (setq top-line (dot))
		    (setq left-column (current-column))))
	    (if (< right-column left-column); Check that right > left
		(progn temp
		    (setq temp left-column)
		    (setq left-column right-column)
		    (setq right-column temp)))
	    (goto-character bottom-line); Set bottom-line to 1st character
            (if (eobp) (newline))
	    (beginning-of-line)
	    (setq bottom-line (dot))
	    (goto-character top-line)
	    (beginning-of-line)	; Set top-line to 1st character in line
	    (setq top-line (dot))
	    (while (< (current-column) left-column); Set top-left to real top
		(forward-character))
	    (setq top-left (dot))
	    (setq number-lines 1)
	    (while (< (dot) bottom-line); Count the number of lines in the
		; area to be affected
		(next-line)
		(setq number-lines (+ number-lines 1)))
	    (goto-character top-line)
	    (setq line-counter 0)
	    (set-mark)
	    (copy-region-to-buffer "Area"); Clear out Area buffer
	    (while (< line-counter number-lines); This is the main loop
		; which will delete the area from the current buffer
		; and place it in "Area" buffer
		(setq line-counter (+ line-counter 1))
		(set-mark)
		(next-line)
		(append-region-to-buffer "Area")
		(delete-to-killbuffer)
		(save-excursion
		    (temp-use-buffer "Area")
		    (to-col left-column); Go to left side of area
		    (previous-line)
		    (set-mark)
		    (beginning-of-line)
		    (delete-region-to-buffer "Hack buffer")
		    (next-line)
		    (to-col (+ (- right-column left-column) 1)); Right side
		    (previous-line)
		    (if (= left-column right-column) (end-of-line))
		    (set-mark)
		    (end-of-line)
		    (append-region-to-buffer "Hack buffer")
		    (delete-to-killbuffer); Also delete it from the buffer
		    (next-line)
		    (beginning-of-line)
		    (kill-to-end-of-line)); Kill the original line
		(yank-buffer "Hack buffer"); and replace it with the new one
		(newline)))	; Add an end-of-line character
	(goto-character top-left)
	(set-mark)
	(if (> number-lines 1)
	    (provide-prefix-argument (- number-lines 1) (next-line)))
	(exchange-dot-and-mark)
	(novalue))
    
    (insert-area
	top-line         	; Top left corner position
	bottom-line		; Position of 1st character in bottom line
	left-column		; Column number of left side of area
	right-column		; Column number of right side of area + 1
	number-lines		; Number of lines to be affected
	line-counter		; Temporary variable for counting lines
	replicate		; Equal to 1 if there is one line to insert
	(save-excursion
	    (setq top-line (dot))
	    (setq left-column (current-column))
	    (save-excursion
		(temp-use-buffer "Area")
		(beginning-of-file)
		(setq line-counter 0)
		(while (! (eobp))	; Count the number of lines
		    (setq line-counter (+ line-counter 1))
		    (end-of-line)
		    (if (> (current-column) right-column); Find right margin
			(setq right-column (current-column)))
		    (next-line)
		    (beginning-of-line))
		(setq number-lines line-counter))
	    (if (= prefix-argument 4)
		(progn
		    (if (> (dot)
			    (progn
				(exchange-dot-and-mark)
				(dot)))
			(exchange-dot-and-mark))
		    (setq bottom-line (dot))
		    (exchange-dot-and-mark)
		    (setq top-line (dot))
		    (setq line-counter 1)
		    (while (< (dot) bottom-line)
			(next-line)
			(setq line-counter (+ line-counter 1)))
		    (setq number-lines line-counter))))
	(progn
	    (setq line-counter 0)
	    (save-excursion
		(temp-use-buffer "Area")
		(beginning-of-file))
	    (while (< line-counter number-lines)
		(goto-character top-line)
		(if (> line-counter 0)
		    (provide-prefix-argument line-counter (next-line)))
		(if (eobp) (newline-and-backup))
		(to-col left-column)
		(insert-string
		    (save-excursion
			(temp-use-buffer "Area")
			(if (eobp)
			    (beginning-of-file))
			(set-mark)
			(next-line)
			(region-to-string)))
		(delete-previous-character); Get rid of eol
		(delete-white-space)
		(if(! (eolp))
		    (to-col (- (+ left-column right-column) 1)))
		(setq line-counter (+ line-counter 1))
		(if (= 0 (- line-counter (* (/ line-counter 22) 22)))
		    (sit-for 0)))
	    (if (!= prefix-argument 4)
		(to-col (- (+ left-column right-column) 1)))
	    (set-mark)
	    (goto-character top-line)
	    (exchange-dot-and-mark)
	    (novalue))))
)
[autoarg.ml]
(progn
(declare-global auto-arg-mode)
(setq auto-arg-mode 0)
(defun
    (~auto-arg
	~auto-current-string
	~auto-current-character
	~auto-current-binding
	(setq ~auto-current-string "")
	(push-back-character (last-key-struck))
	(while
	    (&
		(>=
		    (setq ~auto-current-character (get-tty-character))
		    '0'
		)
		(<=
		    ~auto-current-character
		    '9'
		)
	    )
	    (setq ~auto-current-string
		(concat
		    ~auto-current-string
		    (char-to-string ~auto-current-character)
		)
	    )
	    (message ~auto-current-string)
	)
	(if (error-occurred
		(setq ~auto-current-binding (describe-key ~auto-current-character))
	    )
	    (setq ~auto-current-binding "[unbound]")
	)
	(push-back-character ~auto-current-character)
	(setq ~auto-current-binding (substr ~auto-current-binding -13 13))
	(if (= ~auto-current-binding "\"self-insert\"")
	    (insert-string ~auto-current-string)
	    (return-prefix-argument ~auto-current-string)
	)
    )
    (auto-arg-mode
	(setq auto-arg-mode (!= auto-arg-mode 0))
	(setq auto-arg-mode (! auto-arg-mode))
	(if auto-arg-mode
	    (progn
		(bind-to-key "~auto-arg" "0")
		(bind-to-key "~auto-arg" "1")
		(bind-to-key "~auto-arg" "2")
		(bind-to-key "~auto-arg" "3")
		(bind-to-key "~auto-arg" "4")
		(bind-to-key "~auto-arg" "5")
		(bind-to-key "~auto-arg" "6")
		(bind-to-key "~auto-arg" "7")
		(bind-to-key "~auto-arg" "8")
		(bind-to-key "~auto-arg" "9")
		(message "Auto arg mode on.")
	    )
	    (progn
		(bind-to-key "digit" "0")
		(bind-to-key "digit" "1")
		(bind-to-key "digit" "2")
		(bind-to-key "digit" "3")
		(bind-to-key "digit" "4")
		(bind-to-key "digit" "5")
		(bind-to-key "digit" "6")
		(bind-to-key "digit" "7")
		(bind-to-key "digit" "8")
		(bind-to-key "digit" "9")
		(message "Auto arg mode off")
	    )
	)
    )
)
)
[blissmode.key]
(progn
    (setq-default bliss-auto-expansion 1) ; Set to zero if you dont want auto-expansion
    (local-bind-to-key "expand-bliss-keyword" "")
)
[blissmode.ml]
(progn
    (if (! (is-bound bliss-auto-expansion))
        (progn
	    (declare-buffer-specific bliss-auto-expansion)
	    (setq-default bliss-auto-expansion 1)
	)
    )
    (declare-global
	~bliss-module-name
    )
    (defun
	(bliss-mode
	    (use-local-map "BLISS-MAP")
	    (use-syntax-table "bliss-mode")
	    (use-abbrev-table "bliss-mode")
	    (setq abbrev-mode bliss-auto-expansion)
	    (setq mode-string "Bliss")
	    (language-set-keyword-case)
	    (novalue)
	)
	(~bliss-module
	    (setq ~bliss-module-name "")
	    (language-fetch-mandatory "Insert the module name and use exit-emacs.")
	    (save-window-excursion
		(goto-character template-start-field)
		(set-mark)
		(goto-character template-end-field)(backward-character)
		(setq ~bliss-module-name (region-to-string))
	    )
	    (goto-character template-end-field)
	)
	(~bliss-routine-name
	    (setq ~bliss-routine-name "")
	    (language-fetch-mandatory "Insert the routine name and use exit-emacs.")
	    (save-window-excursion
		(goto-character template-start-field)
		(set-mark)
		(goto-character template-end-field)(backward-character)
		(setq ~bliss-routine-name (region-to-string))
	    )
	    (goto-character template-end-field)
	)
	(~bliss-!
	    language-start-entity
	    language-exit-point
	    (language-template-driver "Bliss" "!" 0)
	)
	(~bliss-%
	    language-start-entity
	    language-exit-point
	    ~bliss-last-key-struck
	    (setq ~bliss-last-key-struck '\r')
	    (if (= (preceding-char) '%')
	        (if (= (current-syntax-entity) is-dull)
		    (progn
		    	(delete-previous-character)
		        (language-template-driver "Bliss" "%(" 0)
		    )
		    (insert-character (last-key-struck))
		)
		(insert-character (last-key-struck))
	    )
	)
	(~bliss-routine
	    bliss-r-fold
	    bliss-r-routine
	    bliss-routine-prefix
	    ~bliss-routine-name
	    (setq bliss-r-fold case-fold-search)
	    (setq case-fold-search 1)
	    (setq bliss-r-routine (provide-prefix-argument -1 (looking-at "external\\W\\W*\\=\\|forward\\W\\W*\\=\\|bind\\W\\W*\\=")))
	    (setq case-fold-search bliss-r-fold)
	    (if (! bliss-r-routine)
		(progn
		    (setq case-fold-search 1)
		    (if (provide-prefix-argument -1 (looking-at "global\\W\\W*\\="))
			(progn
			    (region-around-match 0)
			    (setq bliss-routine-prefix (region-to-string))
			    (erase-region)
			)
			(setq bliss-routine-prefix "")
		    )
		    (setq case-fold-search bliss-r-fold)
		    (template-exit)
		    (language-template-driver "Bliss" "routine-def" 0)
		)
	    )
	)
	(~bliss-exit-on-space
	    (if (!= bliss-last-key-struck '\r')
		(progn
		    (insert-string (char-to-string bliss-last-key-struck))
		    (template-exit)
		)
	    )
	)
	(~bliss-abbrev-handler
	    bliss-ah-current
	    (setq bliss-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= bliss-ah-current is-word)
			(= bliss-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    bliss-ah-case-fold-search
		    bliss-ah-looking-at
		    bliss-ah-data
		    language-start-entity
		    bliss-routine-mark
		    bliss-last-key-struck
		    language-exit-point
		    (setq bliss-ah-case-fold-search case-fold-search)
		    (setq case-fold-search 1)
		    (setq bliss-ah-looking-at
			(provide-prefix-argument -1
			    (looking-at abbrev-expansion)
			)
		    )
		    (setq case-fold-search bliss-ah-case-fold-search)
		    (if bliss-ah-looking-at
			(progn
			    (save-window-excursion
				(region-around-match 0)
				(erase-region)
			    )
			    (setq bliss-last-key-struck (last-key-struck))
			    (language-template-driver "Bliss" abbrev-expansion 1)
			)
		    )
		)
	    )
	    (novalue)
	)
	(~bliss-short-abbrev-handler
	    bliss-ah-current
	    (setq bliss-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= bliss-ah-current is-word)
			(= bliss-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    (delete-previous-word)
		    (insert-string abbrev-expansion)
		    (language-keyword-convert)
		)
	    )
	)
	(expand-bliss-keyword
	    bliss-ek-case-fold-search
	    bliss-ek-looking-at
	    bliss-ek-data
	    language-start-entity
	    bliss-routine-mark
	    bliss-last-key-struck
	    language-exit-point
	    (setq bliss-ek-case-fold-search case-fold-search)
	    (setq case-fold-search 1)
	    (setq bliss-last-key-struck '\r')
	    (setq bliss-ek-looking-at
		(provide-prefix-argument -1
		    (looking-at "\\w")
		)
	    )
	    (setq case-fold-search bliss-ek-case-fold-search)
	    (if bliss-ek-looking-at
		(progn
		    (save-window-excursion
			(backward-word)
			(set-mark)
			(forward-word)
			(case-region-lower)
			(setq bliss-ek-data (region-to-string))
			(erase-region)
		    )
		    (language-template-driver "Bliss" bliss-ek-data 0)
		)
	    )
	    (novalue)
	)
	(~bliss-routine-mark
	    (setq bliss-routine-mark (dot))
	)
	(~bliss-routine-params
	    (if (!= (+ bliss-routine-mark) (dot))
		(progn
		    (insert-string "\n")
		    (to-col
			(save-window-excursion
			    (goto-character bliss-routine-mark)
			    (current-indent)
			)
		    )
		    (insert-string ")")
		)
	    )
	)
    )
    (save-window-excursion
	(if (! (is-bound language-loaded))
	    (execute-mlisp-file "language_support")
	)
	(switch-to-buffer "bliss-mode")
	(use-abbrev-table "bliss-mode")
	(use-syntax-table "bliss-mode")
	(define-keymap "BLISS-MAP")
	(use-local-map "BLISS-MAP")
	(local-bind-to-key "~bliss-!" "!")
	(local-bind-to-key "~bliss-%" "(")
	(if (! (is-bound paren-flash-wait))
	    (execute-mlisp-file "flasher.ml")
	)
	(local-bind-to-key "paren-flash" ")")
	(local-bind-to-key "paren-flash" "]")
	(local-bind-to-key "paren-flash" "}")
	(error-occurred (execute-mlisp-file "blissmode.key"))
	(modify-syntax-table "word" "$_%")
	(modify-syntax-table "paren" "(" ")")
	(modify-syntax-table "paren" "[" "]")
	(modify-syntax-table "paren" "<" ">")
	(modify-syntax-table "string" "'")
	(modify-syntax-table "comment" "!" "\n")
	(modify-syntax-table "comment" "%(" ")%")
	(define-hooked-local-abbrev "module" "module" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "switches" "switches" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "library" "library" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "require" "require" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "begin" "begin" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "psect" "psect" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "own" "own" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "literal" "literal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "external" "external" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "routine" "routine" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "macro" "macro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "keywordmacro" "keywordmacro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "global" "global" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "forward" "forward" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bind" "bind" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "while" "while" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "until" "until" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "if" "if" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "else" "else" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "case" "case" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "select" "select" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selecta" "selecta" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectu" "selectu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectone" "selectone" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectonea" "selectonea" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "selectoneu" "selectoneu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incr" "incr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incra" "incra" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "incru" "incru" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decr" "decr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decru" "decru" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "decra" "decra" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "do" "do" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "leave" "leave" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "with" "with" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "exitloop" "exitloop" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "return" "return" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "abs" "abs" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "max" "max" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "maxu" "maxu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "maxa" "maxa" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "min" "min" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "minu" "minu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "mina" "mina" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "actualparameter" "actualparameter" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "nullparameter" "nullparameter" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "addressing_mode" "addressing_mode" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "actualcount" "actualcount" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "argptr" "argptr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "align" "align" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bitvector" "bitvector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "block" "block" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "blockvector" "blockvector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "vector" "vector" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "BUILTIN" "builtin" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "bliss" "bliss" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "compiletime" "compiletime" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "enable" "enable" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ident" "ident" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "initial" "initial" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "label" "label" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "linkage" "linkage" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "map" "map" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "mod" "mod" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "plit" "plit" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "uplit" "uplit" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "preserve" "preserve" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "nopreserve" "nopreserve" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "preset" "preset" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "register" "register" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "local" "local" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "stacklocal" "stacklocal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "rep" "rep" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "setunwind" "setunwind" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "sign" "sign" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "signal" "signal" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "signal_stop" "signal_stop" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "structure" "structure" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "undeclare" "undeclare" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%allocation" "%ALLOCATION" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%assign" "%assign" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%charcount" "%charcount" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%char" "%char" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ctce" "%ctce" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%declared" "%declared" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%if" "%if" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%error" "%error" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%errormacro" "%errormacro" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%exactstring" "%exactstring" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%expand" "%expand" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%explode" "%explode" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%fieldexpand" "%fieldexpand" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%identical" "%identical" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%inform" "%inform" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%isstring" "%isstring" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%length" "%length" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ltce" "%ltce" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%message" "%message" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%name" "%name" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%nbits" "%nbits" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%nbitsu" "%nbitsu" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%null" "%null" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%number" "%number" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%print" "%print" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%quote" "%quote" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%quotename" "%quotename" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%ref" "%ref" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%remove" "%remove" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%require" "%require" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%size" "%size" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%string" "%string" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%switches" "%switches" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%unquote" "%unquote" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "%warn" "%warn" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$a_rchar" "ch$a_rchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$a_wchar" "ch$a_wchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$allocation" "ch$allocation" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$compare" "ch$compare" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$copy" "ch$copy" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$diff" "ch$diff" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$eql" "ch$eql" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$fail" "ch$fail" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$fill" "ch$fill" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_ch" "ch$find_ch" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_not_ch" "ch$find_not_ch" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$find_sub" "ch$find_sub" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$geq" "ch$geq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$gtr" "ch$gtr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$leq" "ch$leq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$lss" "ch$lss" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$move" "ch$move" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$neq" "ch$neq" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$plus" "ch$plus" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$ptr" "ch$ptr" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$rchar" "ch$rchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$rchar_a" "ch$rchar_a" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$size" "ch$size" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$translate" "ch$translate" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$translatable" "ch$translatable" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$wchar" "ch$wchar" "~bliss-abbrev-handler")
	(define-hooked-local-abbrev "ch$wchar_a" "ch$wchar_a" "~bliss-abbrev-handler")

	; Short abbrev - just case convert as asked
	(define-hooked-local-abbrev "%x" "%X" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%variant" "%VARIANT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%upval" "%UPVAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%remaining" "%REMAINING" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%p" "%P" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%o" "%O" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%g" "%G" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%h" "%H" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%exititeration" "%EXITITERATION" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%exitmacro" "%EXITMACRO" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%e" "%E" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%d" "%D" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%decimal" "%DECIMAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%b" "%B" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bliss" "%BLISS" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%BLISS32" "%BLISS32" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpaddr" "%BPADDR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpunit" "%BPUNIT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%bpval" "%BPVAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%c" "%C" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%count" "%COUNT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascii" "%ASCII" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascic" "%ASCIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%asciz" "%ASCIZ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "%ascid" "%ASCID" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "weak" "WEAK" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "word" "WORD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "word_relative" "WORD_RELATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "write" "WRITE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "volatile" "volatile" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "unsigned" "UNSIGNED" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "signed" "SIGNED" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "share" "SHARE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "relative" "RELATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "relocatable" "RELOCATABLE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "read" "READ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "ref" "REF" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "otherwiae" "OTHERWISE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "outrange" "OUTRANGE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "overlay" "OVERLAY" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "pic" "PIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nodefaule" "NODEFAULT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noexecute" "NOEXECUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nonexternal" "NONEXTERNAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nopic" "NOPIC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noread" "NOREAD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "noshare" "NOSHARE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nowrite" "NOWRITE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "long" "LONG" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "long_relative" "LONG_RELEATIVE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "inrange" "INRANGE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "interrupt" "INTERRUP" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "jsb" "JSB" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "general" "GENERAL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran" "FORTRAN" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran_func" "FORTRAN_FUNC" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "fortran_sub" "FORTRAN_SUB" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "FIELD" "FIELD" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "execute" "EXECUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "concatenate" "CONCATENATE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "debug" "DEBUG" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "call" "CALL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "byte" "BYTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "not" "NOT" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eql" "EQL" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqla" "EQLA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqlu" "EQLU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lss" "LSS" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lssa" "LSSA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lssu" "LSSU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "leq" "LEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "leqa" "LEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "lequ" "LEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtr" "GTR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtra" "GTRA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gtru" "GTRU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "geq" "GEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "geqa" "GEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "gequ" "GEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "and" "AND" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "or" "OR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "eqv" "EQV" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "xor" "XOR" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "neq" "NEQ" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "neqa" "NEQA" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "nequ" "NEQU" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "absolute" "ABSOLUTE" "~bliss-short-abbrev-handler")
	(define-hooked-local-abbrev "always" "ALWAYS" "~bliss-short-abbrev-handler")
    )
    (delete-buffer "bliss-mode")
)
[bufdired.ml]
(progn
    (declare-global ~BUF-current-buffer)
    (declare-global ~BUF-visited-buffer)
    (declare-global ~BUF-wrap-save)
(defun
    (buffer-dired
	(error-occurred
	    (save-window-excursion
		(setq ~BUF-current-buffer (current-buffer-name))
		(setq ~BUF-wrap-save wrap-long-lines)
		(setq wrap-long-lines 0)
		(list-buffers)
		(switch-to-buffer "Buffer list")
		(delete-other-windows)
		(local-bind-to-key "~BUF-previous-line" "p")
		(local-bind-to-key "~BUF-next-line" "n")
		(local-bind-to-key "~BUF-help" "?")
		(local-bind-to-key "~BUF-delete-buffer" "d")
		(local-bind-to-key "~BUF-buffer-check" "c")
		(local-bind-to-key "~BUF-buffer-journal" "j")
		(local-bind-to-key "~BUF-buffer-modified" "m")
		(local-bind-to-key "~BUF-write-file" "w")
		(local-bind-to-key "~BUF-read-file" "r")
		(local-bind-to-key "~BUF-visit-file" "v")
		(local-bind-to-key "~BUF-visit-buffer" "b")
		(local-bind-to-key "~BUF-get-from-library" "g")
		(local-bind-to-key "~BUF-put-in-library" "l")
		(local-bind-to-key "~BUF-quit" "q")
		(local-bind-to-key "~BUF-examine-buffer" "e")
		(~BUF-locate-buffer)
		(recursive-edit)
	    )
	)
	(setq wrap-long-lines ~BUF-wrap-save)
	(if (length ~BUF-current-buffer)
	    (switch-to-buffer ~BUF-current-buffer)
	)
    )
)
(defun    
    (~BUF-help
	(save-window-excursion
	    (switch-to-buffer "Help")
	    (erase-buffer)
	    (delete-other-windows)
	    (insert-string "	Buffer DIRED\n\n")
	    (insert-string "	Help				?\n")
	    (insert-string "	Next line			n\n")
	    (insert-string "	Previous line			p\n")
	    (insert-string "	Delete buffer			d\n")
	    (insert-string "	Change checkpointing		c\n")
	    (insert-string "	Change journalling		j\n")
	    (insert-string "	Change buffer is modified	m\n")
	    (insert-string "	Edit this buffer		e\n")
	    (insert-string "	Fetch buffer-name from library	g\n")
	    (insert-string "	Put buffer-name into library	l\n")
	    (insert-string "	Write out this buffer		w\n")
	    (insert-string "	Re-read the file into buffer	r\n")
	    (insert-string "	Visit a file			v\n")
	    (insert-string "	Visit a buffer			b\n")
	    (insert-string "	Return to original buffer (*)	q\n")
	    (insert-string "\nType any character to continue\n")
	    (get-tty-character)
	    (novalue)
	)
    )
)
(defun    
    (~BUF-next-line
	(next-line)
	(if (eobp) (previous-line))
    )
)
(defun    
    (~BUF-previous-line
	(previous-line)
	(if (looking-at "   ----") (next-line))
    )
)
(defun    
    (~BUF-locate-buffer
	(list-buffers)
	(if (= operating-system-name "unix")
	    (progn
		(beginning-of-file)
		(next-line 2)
		(beginning-of-line)
		(set-mark)
		(end-of-file)
		(filter-region "sort -t_ -k1.13,1.14 -k1.17")
		(unset-mark)
		(beginning-of-file)
		(save-excursion (error-occurred (re-replace-string (getenv "HOME") "~")))
	    )
	)
	(beginning-of-file)
	(next-line)
	(next-line)
	(if (! (error-occurred
		   (while (!= ~BUF-current-buffer (~BUF-get-buffer-name))
		       (next-line))))
	    (progn
		(delete-next-character)
		(insert-character '*')
		(beginning-of-line))
	    (progn
		(beginning-of-file)
		(next-line)
		(next-line)
	    )
	)
    )
)
(defun    
    (~BUF-locate-visited-buffer
	(beginning-of-file)
	(next-line)
	(next-line)
	(if (error-occurred
		(while (!= ~BUF-visited-buffer (~BUF-get-buffer-name))
		    (next-line)))
	    (progn
		(beginning-of-file)
		(next-line)
		(next-line)
	    )
	)
    )
)
(defun    
    (~BUF-examine-buffer
	(setq ~BUF-current-buffer (~BUF-get-buffer-name))
	(exit-emacs)
    )
)
(defun
    (~BUF-quit
	(if (recursion-depth)
	    (exit-emacs)
	)
    )
)
(defun    
    (~BUF-buffer-check bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq current-buffer-checkpointable
		(! current-buffer-checkpointable)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-buffer-journal bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq current-buffer-journalled
		(! current-buffer-journalled)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-buffer-modified bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq buffer-is-modified (! buffer-is-modified))
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun
    (~BUF-get-from-library bufdir-dot local-dot buf-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (switch-to-buffer (~BUF-get-buffer-name))
	    (setq buf-name (current-buffer-name))
	    (setq local-dot (+ 0 (dot)))
	    (erase-buffer)
	    (fetch-database-entry "MLisp-library" (current-buffer-name))
	    (goto-character local-dot))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Fetched " buf-name " from the MLisp-library"))
    )
)
(defun   
    (~BUF-put-in-library bufdir-dot buf-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (setq buf-name (current-buffer-name))
	    (put-database-entry "MLisp-library" (current-buffer-name)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Put " buf-name " into the MLisp-library"))
    )
)
(defun
    (~BUF-visit-file file-name error-found
	(save-window-excursion
	    (setq file-name (arg 1 "Name of file to edit: "))
	    (setq error-found (error-occurred (visit-file file-name)))
	    (setq ~BUF-visited-buffer (current-buffer-name))
	)
	(~BUF-locate-buffer)
	(~BUF-locate-visited-buffer)
	(if error-found
	    (message "Problem visiting the file - Probably not there!")
	)
    )
)
(defun
    (~BUF-visit-buffer buffer-name
	(save-window-excursion
	    (setq buffer-name (arg 1 "Name of buffer to edit: "))
	    (switch-to-buffer buffer-name)
	    (setq ~BUF-visited-buffer (current-buffer-name))
	)
	(~BUF-locate-buffer)
	(~BUF-locate-visited-buffer)
    )
)
(defun
    (~BUF-read-file local-dot bufdir-dot
	(setq bufdir-dot (+ (dot) 0))
	(save-window-excursion
	    (switch-to-buffer (~BUF-get-buffer-name))
	    (setq local-dot (+ (dot) 0))
	    (if (error-occurred (read-file (current-file-name)))
		(message "Cannot find the file")
		(goto-character local-dot)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
(defun    
    (~BUF-write-file bufdir-dot buf-name file-name
	(setq bufdir-dot (+ 0 (dot)))
	(save-window-excursion
	    (temp-use-buffer (~BUF-get-buffer-name))
	    (write-current-file)
	    (setq buf-name (current-buffer-name))
	    (setq file-name (current-file-name)))
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
	(message (concat "Wrote buffer " buf-name " to " file-name))
    )
)
(defun    
    (~BUF-get-buffer-name buffer-name
	(save-window-excursion
	    (beginning-of-line)
	    (while (& (!= (current-column) 17) (! (eolp)))
		(forward-character))
	    (set-mark)
	    (re-search-forward " [ A]")
	    (backward-character)
	    (backward-character)
	    (setq buffer-name (region-to-string))
	    (if (error-occurred (use-old-buffer buffer-name))
		(error-message "Cannot locate buffer")
	    )
	    buffer-name
	)
    )
)
(defun    
    (~BUF-delete-buffer bufdir-dot
	(setq bufdir-dot (+ 0 (dot)))
	(if (= "Buffer list" (~BUF-get-buffer-name))
	    (error-message "You cannot delete this buffer!")
	    (progn
		~BUF-buffer-name
		(setq ~BUF-buffer-name (~BUF-get-buffer-name))
		(delete-buffer ~BUF-buffer-name)
		(if (= ~BUF-buffer-name ~BUF-current-buffer)
		    (setq ~BUF-current-buffer "")
		)
	    )
	)
	(~BUF-locate-buffer)
	(goto-character bufdir-dot) (beginning-of-line)
    )
)
)
[buff.ml]
(progn
; This code written at CMU, on or about Sun Jan 25 07:32:39 1981
; by Doug Philips.
;
; Modified:
;	10-Sept-81 Jeffrey Mogul @ Stanford
;	- now self-contained (no external functions needed)
;
; Mlisp code for doing a one-line buffer listing in the mini-buffer.
; If buffer list is longer than one line, it will print a line at a time
; and wait for a character to be input before moving on to the next line...
; Note:  buffers that have been changed since they were last saved are
; prefixed with an Asterisk(*)... and buffers that have no file associated
; with them are prefixed with a hash-mark(#) and empty buffers are flagged
; with a AtSign(@).
; Note: "load"ing this file binds 'one-line-buffer-list' to ^X^B.

(defun

    (first-non-blank				 ; a useful function
	(beginning-of-line)
	(while (| (= (following-char) 32)	 ; space
		  (= (following-char) 9))	 ; tab
	   (forward-character)
	)
	(current-column)			 ; returned for convience
    )

    (skip-forward-matching char			 ; originally in electric-lisp
	(setq char (following-char))
	(forward-character)
	(while (!= (following-char) char)
	    (forward-character)
	)
	(forward-character)
    )    

    (one-line-buffer-list msg puw empty
	(setq puw pop-up-windows)	    ; save value for later
	(setq pop-up-windows 0)		    ; re-use current window.
	(save-excursion
	    (list-buffers)		    ; generate buffer listings
	    (temp-use-buffer "Buffer list")
	    (beginning-of-file)
	    (kill-to-end-of-line)	    ; for loops needed!!!!
	    (kill-to-end-of-line)
	    (kill-to-end-of-line)
	    (kill-to-end-of-line)
	    (beginning-of-file)
; Flag all the modified buffers...
	    (while (! (eobp))
		(beginning-of-line)		    ; back to top of buffer
		(while (! (= 15 (current-column)))
		    (forward-character))
		(if (= (following-char) 'M')
		    (progn
			(delete-next-character)
			(forward-character)
			(insert-character '*')))
		(next-line))
	    (beginning-of-file)
; eliminate Non-file(Scratch?) buffers...
	    (while (! (error-occurred (search-forward "Scr")))
		(beginning-of-line)
		(kill-to-end-of-line)
		(kill-to-end-of-line)
	    )
	    (beginning-of-file)		    ; start at top again
	    (while (! (eobp))		    ; and wipe out the first -
		(setq empty (is-zero?))     ;    (remember if buffer is empty)
		(beginning-of-line)
		(delete-white-space)	    ; - two columns of the buffer
		(delete-next-word)	    ;   listing which are the size
		(delete-white-space)	    ;   and type columns...
		(delete-next-word)
		(delete-white-space)
		(if empty
		    (progn (beginning-of-line)
			   (insert-string "@")
			   (beginning-of-line)
		    )
		)
		(next-line)
	    )
	    (beginning-of-file)		    ; back at the beginning again
; Flag all the unnamed buffers...
	    (while (! (error-occurred (search-forward "[none]")))
		   (beginning-of-line)
		   (insert-character '#')
		   (next-line)
	    )
	    (beginning-of-file)		    ; back to the top!
	    (while (! (eobp))		    ; hack out all but buffer name
		(while (& (!= (following-char) ' '); space or
			  (!= (following-char) 9)); tab!
			  (forward-character)
		)
		(kill-to-end-of-line)
		(next-line)
		(beginning-of-line)
	    )
	    (end-of-file)
	    (delete-previous-character)	    ; kill trailing crlf
	    (beginning-of-file)		    ; go back and change
	    (error-occurred (replace-string "\n" ", "))	; newlines to /, /s
	    (beginning-of-file)
	    (set-mark)
	    (end-of-file)		    ; suck in buffer contents
	    (setq msg (concat "Buffers: " (region-to-string)))
	)
	(setq pop-up-windows puw)	    ; restore to previous value
	(if (> (length msg) 76)		    ; attempt to get string on one
		(setq msg (substr msg 10 -9)); line
	)
	(while (> (length msg) 76)	    ; multi-line mode
	    (message (concat (substr msg 1 76) "$")); print nth line
	    (setq msg (substr msg 77 10000000)); hack it off, and
	    (get-tty-character)		    ; wait for more
	)
	(message msg)			    ; print out final line
    )

    (is-zero?
	(first-non-blank)
	(if (= (following-char) '0')
	    (progn (forward-character)
		   (if (| (= (following-char) 32) (= (following-char) 9))
			1
			0
		   )
	    )
	    0
	)
	
    )
)

(bind-to-key "one-line-buffer-list" "\^X\^B")
(novalue)
)
[centerlin.ml]
(progn
    (declare-global justify-right-margin)
    (declare-global justify-left-margin)
    (if
	(= justify-right-margin 0)
	(setq justify-right-margin 78)
    )
    (defun
	(center-line width
	    (save-excursion
		(beginning-of-line)
		(delete-white-space)
		(end-of-line)
		(delete-white-space)
		(setq width (current-column))
		(beginning-of-line)
		(to-col
		    (+
			justify-left-margin
			(/ 
			    (- 
				(- justify-right-margin justify-left-margin)
				 width)
			    2)
		    )
		)
	    )
	)
    )
)
[cli.ml]
(progn
(external-function "cli_define_table" "emacs$share:emacs_cli_shr" "emacs$cli_define_table")
(external-function "cli_list_tables" "emacs$share:emacs_cli_shr" "emacs$cli_list_tables")
(external-function "cli$dcl_parse" "emacs$share:emacs_cli_shr" "emacs$cli$dcl_parse")
(external-function "cli$present" "emacs$share:emacs_cli_shr" "emacs$cli$present")
(external-function "cli$get_value" "emacs$share:emacs_cli_shr" "emacs$cli$get_value")

(~sys_literal_setup_equal "cli$_"
    "absent"     229872           
    "comma"      261433           
    "concat"     261417           
    "defaulted"  261409           
    "locneg"     229936           
    "locpres"    261425           
    "negated"    229880           
    "nocomd"     229552           
    "present"    261401           
)

(defun define-cli-table
    (
	~table (get-tty-string ": cli-define-table ")
	~file (get-tty-string ": cli-define-table (file) ")
    )
    (cli_define_table ~table ~file)
    
)
(defun
    (list-cli-tables
	(save-excursion
	    (pop-to-buffer "CLI table list")
	    (erase-buffer)
	    (cli_list_tables)
	)
    )
)
)
[cmode.key]
(progn
    (setq-default c-auto-expansion 1) ; Set to zero if you dont want auto-expansion
    (local-bind-to-key "expand-c-keyword" "")
)
[cmode.ml]
(progn
    (if (! (is-bound c-auto-expansion))
        (progn
	    (declare-buffer-specific c-auto-expansion)
	    (setq-default c-auto-expansion 1)
	)
    )
    (defun
	(c-mode
	    (use-syntax-table "c-mode")
	    (use-abbrev-table "c-mode")
	    (setq abbrev-mode c-auto-expansion)
	    (use-local-map "C-MAP")
	    (setq abbrev-mode 1)
	    (setq mode-string "C")
	    (novalue)
	)
	(~c-abbrev-handler
	    c-ah-current
	    (setq c-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= c-ah-current is-word)
			(= c-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    c-ah-case-fold-search
		    c-ah-looking-at
		    c-ah-data
		    language-start-entity
		    c-last-key-struck
		    language-exit-point
		    (setq c-ah-case-fold-search case-fold-search)
		    (setq case-fold-search 0)
		    (setq c-ah-looking-at
			(provide-prefix-argument -1
			    (looking-at abbrev-expansion)
			)
		    )
		    (setq case-fold-search c-ah-case-fold-search)
		    (if c-ah-looking-at
			(progn
			    (save-window-excursion
				(region-around-match 0)
				(erase-region)
			    )
			    (setq c-last-key-struck (last-key-struck))
			    (language-template-driver "C" abbrev-expansion 1)
			)
		    )
		)
	    )
	    (novalue)
	)
	(c-/*
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (preceding-char) '/')
	        (if (= (current-syntax-entity) is-dull)
		    (progn
		    	(delete-previous-character)
		        (language-template-driver "C" "/*" 0)
		    )
		    (insert-character '*')
		)
		(insert-character '*')
	    )
	)
	(c-{
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (current-syntax-entity) is-dull)
		(language-template-driver "C" "{" 0)
		(insert-character '{')
	    )
	)
	(c-?
	    language-start-entity
	    language-exit-point
	    c-last-key-struck
	    (setq c-last-key-struck '\r')
	    (if (= (current-syntax-entity) is-dull)
		(language-template-driver "C" "?" 0)
		(insert-character '?')
	    )
	)
	(expand-c-keyword
	    c-ek-case-fold-search
	    c-ek-looking-at
	    c-ek-data
	    language-start-entity
	    c-last-key-struck
	    language-exit-point
	    (setq c-ek-case-fold-search case-fold-search)
	    (setq case-fold-search 1)
	    (setq c-last-key-struck '\r')
	    (setq c-ek-looking-at
		(provide-prefix-argument -1
		    (looking-at "\\w")
		)
	    )
	    (setq case-fold-search c-ek-case-fold-search)
	    (if c-ek-looking-at
		(progn
		    (save-window-excursion
			(backward-word)
			(set-mark)
			(forward-word)
			(case-region-lower)
			(setq c-ek-data (region-to-string))
			(erase-region)
		    )
		    (language-template-driver "C" c-ek-data 0)
		)
	    )
	    (novalue)
	)
    )
    (save-window-excursion
	(if (! (is-bound language-loaded))
	    (execute-mlisp-file "language_support")
	)
	(if (! (is-bound paren-flash))
	    (execute-mlisp-file "flasher")
	)
	(switch-to-buffer "c-mode")
	(define-keymap "C-MAP")
	(use-local-map "C-MAP")
	(use-abbrev-table "c-mode")
	(use-syntax-table "c-mode")
	(modify-syntax-table "word" "$_#")
	(modify-syntax-table "paren" "(" ")")
	(modify-syntax-table "paren" "[" "]")
	(modify-syntax-table "paren" "{" "}")
	(modify-syntax-table "string" "\"")
	(modify-syntax-table "prefix" "\\")
	(modify-syntax-table "comment" "/*" "*/")
	(local-bind-to-key "c-{" "{")
	(local-bind-to-key "c-/*" "*")
	(local-bind-to-key "c-?" "?")
	(local-bind-to-key "paren-flash" ")")
	(local-bind-to-key "paren-flash" "]")
	(local-bind-to-key "paren-flash" "}")
	(error-occurred (execute-mlisp-file "cmode.key"))
	(define-hooked-local-abbrev "goto" "goto" "~c-abbrev-handler")
	(define-hooked-local-abbrev "return" "return" "~c-abbrev-handler")
	(define-hooked-local-abbrev "if" "if" "~c-abbrev-handler")
	(define-hooked-local-abbrev "else" "else" "~c-abbrev-handler")
	(define-hooked-local-abbrev "for" "for" "~c-abbrev-handler")
	(define-hooked-local-abbrev "do" "do" "~c-abbrev-handler")
	(define-hooked-local-abbrev "while" "while" "~c-abbrev-handler")
	(define-hooked-local-abbrev "switch" "switch" "~c-abbrev-handler")
	(define-hooked-local-abbrev "case" "case" "~c-abbrev-handler")
	(define-hooked-local-abbrev "default" "default" "~c-abbrev-handler")
	(define-hooked-local-abbrev "entry" "entry" "~c-abbrev-handler")
	(define-hooked-local-abbrev "sizeof" "sizeof" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#include" "#include" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#module" "#module" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#if" "#if" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#ifdef" "#ifdef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#ifndef" "#ifndef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#undef" "#undef" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#define" "#define" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#dictionary" "#dictionary" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#line" "#line" "~c-abbrev-handler")
	(define-hooked-local-abbrev "#" "#" "~c-abbrev-handler")
    )
    (delete-buffer "c-mode")
)
[cms.ml]
(progn
    (declare-global
        cms$_set_library
	cms$_show_class
	cms$_show_element
	cms$_show_group
	cms$_show_reservation
	cms$_fetch
	cms$_reserve
	cms$_unreserve
	cms$_annotate
	cms$_create_class
	cms$_create_group
	cms$_delete_class
	cms$_delete_element
	cms$_delete_group
	cms$_insert_element
	cms$_insert_generation
	cms$_insert_group
	cms$_replace
	cms$_create_element
	cms$_copy_element
	cms$_modify_class
	cms$_modify_element
	cms$_modify_group
	cms$_modify_library
	cms$_remove_element
	cms$_remove_generation
	cms$_remove_group
	cms$_remark
	~cms-library
	cms-element
	cms-group-flag
	cms-generation
	cms-user
	cms-nohistory
	cms-nonotes
	cms-concurrent
	cms-reservation
	cms-reference
	cms-keep
    )
    (declare-buffer-specific
        cms-remark
    )
    (setq-default cms-remark "")
    (setq cms-element "")
    (setq cms-group-flag "1")
    (setq cms-generation "")
    (setq cms-user "")
    (setq cms-nohistory "1")
    (setq cms-nonotes "1")
    (setq cms-concurrent "1")
    (setq cms-reservation "")
    (setq cms-reference "1")
    (setq cms-keep "1")
    (setq cms$_set_library 0)
    (setq cms$_show_class 1)
    (setq cms$_show_element 2)
    (setq cms$_show_group 3)
    (setq cms$_show_reservation 4)
    (setq cms$_fetch 5)
    (setq cms$_reserve 6)
    (setq cms$_unreserve 7)
    (setq cms$_annotate 8)
    (setq cms$_create_class 9)
    (setq cms$_create_group 10)
    (setq cms$_delete_class 11)
    (setq cms$_delete_element 12)
    (setq cms$_delete_group 13)
    (setq cms$_insert_element 14)
    (setq cms$_insert_generation 15)
    (setq cms$_insert_group 16)
    (setq cms$_replace 17)
    (setq cms$_create_element 18)
    (setq cms$_copy_element 19)
    (setq cms$_modify_class 20)
    (setq cms$_modify_element 21)
    (setq cms$_modify_group 22)
    (setq cms$_modify_library 23)
    (setq cms$_remove_element 24)
    (setq cms$_remove_generation 25)
    (setq cms$_remove_group 26)
    (setq cms$_remark 27)
    (setq ~cms-library "")
    (external-function "emacs$cms" "emacs$share:emacs_cms_shr")
    (defun
	(cms-set-library
	    ~cms-lib
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if ~cms-flag
		(setq ~cms-lib (arg 1 ": cms-set-library "))
	        (setq ~cms-lib (getenv "CMS$LIB"))
	    )
	    (emacs$cms cms$_set_library ~cms-lib)
	    (setq ~cms-lib (expand-file-name ~cms-lib))
	    (setq ~cms-library (substr ~cms-lib 1 (- (length ~cms-lib) 2)))
	    (if (interactive)
		(message "Library set to " ~cms-library)
	    )
	    (novalue)
	)
	(cms-show-library
	    (if (length ~cms-library)
	        (message "Your CMS library is " ~cms-library)
		(error-message "You have not defined a CMS library")
	    )
	    (novalue)
	)
        (cms-show-elements
	    ~cms-buffer
	    ~cms-elements
	    ~cms-group_flag
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (if ~cms-flag
		(if (! (form 1 19 "Show Elements" "~cms-show-elements"
		    "Elements" cms-element
		    "Include groups flag" cms-group-flag
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-elements (if (interactive) "" (arg 1)))
		    (setq ~cms-group_flag (if (interactive) 1 (arg 2)))
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_element ~cms-elements ~cms-group_flag)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified elements exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(~cms-show-elements ~cms-group
	    (setq cms-element (setq ~cms-elements (arg 1)))
	    (if (length (setq ~cms-group (arg 2)))
		(setq cms-group-flag (setq ~cms-group_flag ~cms-group))
	    )
	)
	(cms-show-class
	    ~cms-buffer
	    ~cms-classes
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (setq ~cms-classes
	        (if ~cms-flag
		    (arg 1 ": cms-show-class (class name) ")
		    ""
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_class ~cms-classes)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified classes exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(cms-show-group
	    ~cms-buffer
	    ~cms-groups
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (setq ~cms-groups
	        (if ~cms-flag
		    (arg 1 ": cms-show-group (group name) ")
		    ""
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_group ~cms-groups)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified groups exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(cms-show-reservation
	    ~cms-buffer
	    ~cms-elements
	    ~cms-generations
	    ~cms-user
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (if ~cms-flag
		(if (! (form 1 14 "Show Reservations" "~cms-show-reservations"
		    "Elements" cms-element
		    "Generation" cms-generation
		    "User name" cms-user
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-elements (if (interactive) "" (arg 1)))
		    (setq ~cms-generations (if (interactive) "" (arg 2)))
		    (setq ~cms-user (if (interactive) "" (arg 3)))
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_reservation ~cms-elements ~cms-generations ~cms-user)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified reservations exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(~cms-show-reservations
	    (setq cms-element (setq ~cms-elements (arg 1)))
	    (setq cms-generation (setq ~cms-generations (arg 2)))
	    (setq cms-user (setq ~cms-user (arg 3)))
	)
	(cms-fetch
	    ~cms-element
	    ~cms-generation
	    ~cms-nohistory
	    ~cms-nonotes
	    ~cms-buffer
	    ~cms-count
	    ~cms-gen_resp
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 15 "Fetch" "~cms-fetch"
		    "Element" ""
		    "Generation" ""
		    "No history flag" cms-nohistory
		    "No notes flag" cms-nonotes
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-fetch (element name) "))
		    (setq ~cms-generation (if (interactive) "" (arg 2)))
		    (setq ~cms-nohistory (if (interactive) 0 (arg 3)))
		    (setq ~cms-nonotes (if (interactive) 0 (arg 4)))
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "CMS fetch")
		(erase-buffer)
	        (emacs$cms cms$_fetch ~cms-element ~cms-generation
	            ~cms-nohistory ~cms-nonotes ~cms-gen_resp)
	    )
	    (if ask-about-buffer-names
	        (if (error-occurred (save-window-excursion (use-old-buffer ~cms-element)))
		(setq ~cms-buffer ~cms-element)
	        (progn
		    (setq ~cms-buffer
			(get-tty-string
		            (concat
				"Buffer name "
				~cms-element
				"is in use, type a new name or <CR> to clobber: "
			    )
			)
		    )
		    (if (= (length ~cms-buffer) 0)
		        (setq ~cms-buffer ~cms-element)
		    )
		    (error-occurred (delete-buffer ~cms-buffer))
		)
		)
		(progn
		    (setq ~cms-buffer ~cms-element)
		    (setq ~cms-count 1)
		    (save-window-excursion
			(while (! (error-occurred (use-old-buffer ~cms-buffer)))
			    (setq ~cms-count (+ ~cms-count 1))
			    (setq ~cms-buffer (concat ~cms-element "<" ~cms-count ">"))
			)
		    )
		)
	    )
	    (pop-to-buffer "CMS fetch")
	    (setq current-buffer-name ~cms-buffer)
	    (error-occurred (setq current-buffer-file-name ~cms-element))
	    (apply-auto-execute ~cms-element)
	    (setq buffer-is-modified 0)
	    (message "Generation " ~cms-gen_resp " of element " ~cms-element " fetched")
	    (novalue)
	)
	(~cms-fetch
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-generation (setq ~cms-generation (arg 2)))
	    (setq cms-nohistory (setq ~cms-nohistory (arg 3)))
	    (setq cms-nonotes (setq ~cms-nonotes (arg 4)))
	)
	(cms-reserve
	    ~cms-element
	    ~cms-generation
	    ~cms-remark
	    ~cms-nohistory
	    ~cms-nonotes
	    ~cms-concurrent
	    ~cms-buffer
	    ~cms-count
	    ~cms-gen_resp
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 16 "Reserve" "~cms-reserve"
		    "Element" ""
		    "Remark" cms-remark
		    "Generation" ""
		    "No history flag" cms-nohistory
		    "No notes flag" cms-nonotes
		    "Concurrency flag" cms-concurrent
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-reserve (element name) "))
		    (setq ~cms-remark (arg 2 ": cms-reserve (remark) "))
		    (setq ~cms-generation (if (interactive) "" (arg 3)))
		    (setq ~cms-nohistory (if (interactive) 0 (arg 4)))
		    (setq ~cms-nonotes (if (interactive) 0 (arg 5)))
		    (setq ~cms-concurrent (if (interactive) 1 (arg 5)))
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "CMS fetch")
		(erase-buffer)
		(emacs$cms cms$_reserve ~cms-element ~cms-generation ~cms-remark
	            ~cms-nohistory ~cms-nonotes ~cms-concurrent ~cms-gen_resp)
	    )
	    (if ask-about-buffer-names
	        (if (error-occurred (save-window-excursion (use-old-buffer ~cms-element)))
		(setq ~cms-buffer ~cms-element)
	        (progn
		    (setq ~cms-buffer
			(get-tty-string
		            (concat
				"Buffer name "
				~cms-element
				"is in use, type a new name or <CR> to clobber: "
			    )
			)
		    )
		    (if (= (length ~cms-buffer) 0)
		        (setq ~cms-buffer ~cms-element)
		    )
		    (error-occurred (delete-buffer ~cms-buffer))
		)
		)
		(progn
		    (setq ~cms-buffer ~cms-element)
		    (setq ~cms-count 1)
		    (save-window-excursion
			(while (! (error-occurred (use-old-buffer ~cms-buffer)))
			    (setq ~cms-count (+ ~cms-count 1))
			    (setq ~cms-buffer (concat ~cms-element "<" ~cms-count ">"))
			)
		    )
		)
	    )
	    (pop-to-buffer "CMS fetch")
	    (setq current-buffer-name ~cms-buffer)
	    (error-occurred (setq current-buffer-file-name ~cms-element))
	    (apply-auto-execute ~cms-element)
	    (message "Generation " ~cms-gen_resp " of element " ~cms-element " reserved")
	    (novalue)
	)
	(~cms-reserve
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq cms-generation (setq ~cms-generation (arg 3)))
	    (setq cms-nohistory (setq ~cms-nohistory (arg 4)))
	    (setq cms-nonotes (setq ~cms-nonotes (arg 5)))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	)
	(cms-unreserve
	    ~cms-element
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element (arg 1 ": cms-unreserve (element name) "))
	    (setq ~cms-remark
		(if ~cms-flag
		    ""
		    (arg 2 ": cms-unreserve (remark) ")
		)
	    )
	    (emacs$cms cms$_unreserve ~cms-element ~cms-remark)
	    (novalue)
	)
        (cms-create-class
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-create-class (class name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-create-class (remark) ")
	      )
	    )
	(emacs$cms cms$_create_class ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-create-group
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-create-group (group name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-create-group (remark) ")
	      )
	    )
	(emacs$cms cms$_create_group ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-class
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-class (class name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-class (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_class ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-element
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-element (element name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-element (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_element ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-group
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-group (group name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-group (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_group ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-insert-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Insert Element" "~cms-item-target"
		    "Element" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-element (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_insert_element ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
	(~cms-item-target
	    (setq ~cms-item (arg 1))
	    (setq ~cms-target (arg 2))
	    (setq cms-remark (setq ~cms-remark (arg 3)))
	)
        (cms-insert-group
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 9 "Insert Group" "~cms-item-target"
		    "Sub-group" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-group (sub-group name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-group (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_insert_group ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
        (cms-insert-generation
	    ~cms-item
	    ~cms-target
	    ~cms-gen
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 10 "Insert Generation" "~cms-insert-generation"
		    "Element" ""
		    "Class" ""
		    "Generation" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-generation (element name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-generation (class name) "))
		    (setq ~cms-gen (if (interactive) "" (arg 3)))
		    (setq ~cms-remark (if (interactive) "" (arg 4)))
		)
	    )
	    (emacs$cms cms$_insert_generation ~cms-item ~cms-target ~cms-remark ~cms-gen)
	    (novalue)
	)
	(~cms-insert-generation
	    (setq ~cms-item (arg 1))
	    (setq ~cms-target (arg 2))
	    (setq ~cms-gen (arg 3))
	    (setq cms-remark (setq ~cms-remark (arg 4)))
	)
	(cms-replace
	    ~cms-element
	    ~cms-remark
	    ~cms-variant
	    ~cms-reserve
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element
		(if (length current-buffer-file-name)
		    current-buffer-name
		    cms-element
		)
	    )
	    (if ~cms-flag
		(if (! (form 2 16 "Replace" "~cms-replace"
		    "Element" ~cms-element
		    "Remark" cms-remark
		    "Variant" ""
		    "Reservation flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-replace (element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 2)))
		    (setq ~cms-variant (if (interactive) "" (arg 3)))
		    (setq ~cms-reserve (if (interactive) "" (arg 4)))
		)
	    )
	    (emacs$cms cms$_replace ~cms-element ~cms-remark ~cms-variant ~cms-reserve)
	    (setq buffer-is-modified 0)
	    (novalue)
	)
	(~cms-replace
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-variant (arg 3))
	    (setq ~cms-reserve (arg 4))
	)
	(cms-create-element
	    ~cms-element
	    ~cms-remark
	    ~cms-history
	    ~cms-keep
	    ~cms-reserve
	    ~cms-concurrent
	    ~cms-reference
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element
		(if (length current-buffer-file-name)
		    current-buffer-name
		    cms-element
		)
	    )
	    (if ~cms-flag
		(if (! (form 2 16 "Create Element" "~cms-create-element"
		    "Element" ~cms-element
		    "Remark" cms-remark
		    "History" ""
		    "Keep flag" cms-keep
		    "Reservation flag" cms-reservation
		    "Concurrency flag" cms-concurrent
		    "Reference flag" cms-reference
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-create-element (element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 2)))
		    (setq ~cms-history (if (interactive) "" (arg 3)))
		    (setq ~cms-keep (if (interactive) "1" (arg 4)))
		    (setq ~cms-reserve (if (interactive) "0" (arg 5)))
		    (setq ~cms-concurrent (if (interactive) "1" (arg 6)))
		    (setq ~cms-reference (if (interactive) "1" (arg 7)))
		)
	    )
	    (emacs$cms cms$_create_element ~cms-element ~cms-remark ~cms-history
	        "" ~cms-keep ~cms-reserve ~cms-concurrent ~cms-reference)
	    (setq buffer-is-modified 0)
	    (novalue)
	)
	(~cms-create-element
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-history (arg 3))
	    (setq cms-keep (setq ~cms-keep (arg 4)))
	    (setq cms-reservation (setq ~cms-reserve (arg 5)))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	    (setq cms-reference (setq ~cms-reference (arg 7)))
	)
        (cms-copy-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 11 "Copy Element" "~cms-item-target"
		    "Element" ""
		    "New element" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-copy-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-copy-element (new element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_copy_element ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
        (cms-modify-class
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-read-only
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 16 "Modify Class" "~cms-modify"
		    "Class" ""
		    "Remark" cms-remark
		    "New class" ""
		    "New class remark" ""
		    "Read only flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_class ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark ~cms-read-only)
	    (novalue)
	)
	(~cms-modify
	    (setq ~cms-item (arg 1))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-new-item (arg 3))
	    (setq ~cms-new-remark (arg 4))
	    (setq ~cms-read-only (arg 5))
	)
        (cms-modify-group
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-read-only
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 16 "Modify Group" "~cms-modify"
		    "Group" ""
		    "Remark" cms-remark
		    "New group" ""
		    "New group remark" ""
		    "Read only flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_group ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark ~cms-read-only)
	    (novalue)
	)
        (cms-modify-element
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-history
	    ~cms-concurrent
	    ~cms-reference
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 18 "Modify Element" "~cms-modify"
		    "Element" ""
		    "Remark" cms-remark
		    "New element" ""
		    "New element remark" ""
		    "History" ""
		    "Concurrency flag" cms-concurrent
		    "Reference flag" cms-reference
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_element ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark
	        ~cms-history "" ~cms-concurrent ~cms-reference)
	    (novalue)
	)
	(~cms-modify
	    (setq ~cms-item (arg 1))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-new-item (arg 3))
	    (setq ~cms-new-remark (arg 4))
	    (setq ~cms-history (arg 5))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	    (setq cms-reference (setq ~cms-reference (arg 7)))
	)
        (cms-remove-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Element" "~cms-item-target"
		    "Element" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-element (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_element ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remove-generation
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Generation" "~cms-item-target"
		    "Element" ""
		    "Class" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-generation (element name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-generation (class name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_generation ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remove-group
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Group" "~cms-item-target"
		    "Sub-group" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-group (sub-group name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-group (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_group ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remark
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (emacs$cms cms$_remark (arg 1 ": cms-remark "))
	    (novalue)
	)
    )
    (error-occurred (execute-mlisp-file "cms.key"))
    (if (! (is-bound form-depth))
	(error-occurred (execute-mlisp-file "form"))
    )
)
[dabbrev.ml]
;;; Dynamic abbreviations package.

;;; db-backdot is used for memory in successive reverse searches.
;;; db-search-string is used, e.g., if you have just expanded "nan" to
;;;    "nanny", the next reverse search still looks for "nan".
;;; db-expanded word is used for communication between the word-location
;;;    and replacement functions.  It is the expanded version of the search
;;;    string.  It could be a local parameter, if I could get functions
;;;    with arguments to work.
;;; db-just-errorred is so, after you run out of search choices, you
;;;    come back full cycle instead of just dying.
(declare-buffer-specific db-backdot db-search-string db-just-errorred
                         db-expanded-word db-direction)
(defun
    (dabbrev
	(save-excursion start-position
	    (setq start-position (dot))
	    (if (| (!= (previous-command) this-command) (= db-just-errorred 1))
		;; This is a first expansion:  figure out
		;; db-search-string and db-backdot.
		(progn
		    (setq db-just-errorred 0)
		    (setq db-direction 0)
		    (setq db-expanded-word "")
		    (set-mark)
		    (backward-word)
		    (setq db-backdot (dot))
		    (setq db-search-string (region-to-string))
		)
		;; Otherwise, this is a second expansion, use already-existing stuff.
		(goto-character db-backdot)
	    )
	    (db-locate-expanded-word)
	)
	(db-replace-with-new-word)
    )
)

;;; Search backwards, starting with the current dot, to find a match.
;;; Our context includes the appropriate setting of dot.
(defun
    (db-locate-expanded-word db-new-expanded-word
	(if db-direction
	    (if (error-occurred (re-search-forward (concat "\\b" db-search-string "\\w")))
		(progn
		    (message "No more matches")
		    (setq db-expanded-word db-search-string)
		    (setq db-just-errorred 1)
		)
		(progn
		    ;; Get the expanded word into a string used by the next function.
		    (backward-word)
		    (set-mark)
		    (forward-word)
		    (setq db-new-expanded-word (region-to-string))
		    ;; Set backdot to the end of the match word, for
		    ;; use in later forward searches.
		    (setq db-backdot (dot))
		    (if (= db-new-expanded-word db-expanded-word)
			(db-locate-expanded-word)
			(setq db-expanded-word db-new-expanded-word)
		    )
		)
	    )
	    ; else
	    (if (error-occurred (re-search-reverse (concat "\\b" db-search-string "\\w")))
		(progn
		    (setq db-direction 1)
		    (goto-character start-position)
		    (db-locate-expanded-word)
		)
		(progn
		    ;; Set backdot to the beginning of the match word, for
		    ;; use in later reverse searches.
		    (setq db-backdot (dot))
		    ;; Get the expanded word into a string used by the next function.
		    (set-mark)
		    (forward-word)
		    (setq db-new-expanded-word (region-to-string))
		    (backward-word)
		    (if (= db-new-expanded-word db-expanded-word)
			(db-locate-expanded-word)
			(setq db-expanded-word db-new-expanded-word)
		    )
		)
	    )
	)
    )
)

;;; With the cursor back where it started, finally do the expansion.
(defun
    (db-replace-with-new-word
	;(message (concat "Expanded word is |" db-expanded-word "|"))
	;(sit-for 5)
	(delete-previous-word)
	(insert-string db-expanded-word)
    )
)
[dclmode.ml]
(progn
; 
; DCL-mode
; 
; 	package to help write DCL command procedures
; 
; Barry A. Scott	 7-Jun-1983
;
(defun
    (DCL-mode
	(remove-all-local-bindings)

	(local-bind-to-key "electric-DCL-!" "!")
	(local-bind-to-key "electric-DCL-newline" "\^j")

	(setq mode-string "DCL")
	(use-abbrev-table "DCL-mode")
	(use-syntax-table "DCL-mode")
	(novalue)
    )
)

(defun
    (electric-DCL-!
	(end-of-line) (delete-white-space)
	(if (= (current-column) 1)
	    (insert-string "$	!	"); comment on empty line
	    
	    (progn
		(beginning-of-line)
		(if (looking-at "^$[	 ]*$")
		    (progn
			(end-of-line)
			(insert-string "	!	")
		    )
		    (progn
			(end-of-line)
			(if (< (current-column) comment-column)
			    (progn
				(to-col comment-column)
				(insert-string "! ")
			    )
			    
			    (insert-string "	! ")
			)
		    )
		)
	    )
	)
    )
)    
(defun
    (electric-DCL-newline indent-level $-needed
	; 
	; Find the indent level
	; 
	(delete-white-space)
	(newline)
	(previous-line)
	(if (= (following-char) '$')
	    (progn
		(setq $-needed 1)
		(delete-next-character)
		(setq indent-level (current-indent))
		(insert-character '$')
	    )
	    (progn
		(setq $-needed 0)
		(setq indent-level (current-indent))
	    )
	)
	(end-of-line)
	(if (& $-needed (!= (preceding-char) '-'))
	    (progn
		(forward-character)
		(insert-character '$')
		(to-col indent-level)
	    )
	    (progn
		(forward-character)
		(to-col indent-level)
	    )
	)
    )
)
    
(use-syntax-table "DCL-mode")
(modify-syntax-table "paren" "(" ")")
(modify-syntax-table "string" "\"")
(modify-syntax-table "comment" "!" "\n")
(modify-syntax-table "word" "$%^_~*")
(novalue)
)
[debug.key]
(progn
    (setq debug-show-source 1)
    (setq debug-show-last-expression 1)
    (setq debug-sit-for 5)
    (local-bind-to-key "debug-help" "h")
    (local-bind-to-key "debug-help" "H")
    (local-bind-to-key "debug-help" "?")
    (local-bind-to-key "debug-help" "\^_")
    (local-bind-to-key "debug-help" "\(help)")

    (local-bind-to-key "debug-go" "g")
    (local-bind-to-key "debug-go" "G")
    (local-bind-to-key "debug-step" "s")
    (local-bind-to-key "debug-step" "S")
    (local-bind-to-key "debug-step-over" "o")
    (local-bind-to-key "debug-step-over" "O")
    (local-bind-to-key "debug-set-break" "b")
    (local-bind-to-key "debug-can-break" "B")
    (local-bind-to-key "debug-exit" "q")
    (local-bind-to-key "debug-exit" "Q")
    (local-bind-to-key "debug-show-breakpoints" "l")
    (local-bind-to-key "debug-show-breakpoints" "L")
    (local-bind-to-key "debug-show-calls" "c")
    (local-bind-to-key "debug-show-calls" "C")
    (local-bind-to-key "debug-trace-mode" "t")
    (local-bind-to-key "debug-notrace-mode" "T")
    (local-bind-to-key "debug-set-exception" "e")
    (local-bind-to-key "debug-cancel-exception" "E")
    (local-bind-to-key "debug-show-variable" "p")
    (local-bind-to-key "debug-show-last-expression" "v")
    (local-bind-to-key "debug-show-source" ".")

    (local-bind-to-key "debug-go" "\(comma)")
    (local-bind-to-key "debug-step" "\(gold)\(kp0)")
    (local-bind-to-key "debug-step-over" "\(kp0)")
    (local-bind-to-key "debug-show-calls" "\(gold)\(kp5)")
    (local-bind-to-key "debug-show-source" "\(kp5)")


(setq ~debug-help
    (concat
	"HELP: on debugging Mock Lisp programs\n\n"
	"	Key	Action\n"
	"	---	------\n"
	"	b	Sets a function breakpoint\n"
	"	B	Removes a function breakpoint\n"
	"	c	Shows call stack\n"
	"	. KP5	Show the current source\n"
	"	q	Exit DEBUG\n"
	"	g KP,	Continues program\n"
	"	h HELP	Displays help information\n"
	"	e	When an error occurs enter the debugger\n"
	"	E	Lets EMACS handle errors as usual\n"
	"	l	Lists all breakpoints\n"
	"	o KP0	Steps over one program expression\n"
	"	p	print a variable\n"
	"	s 	Steps one program expression\n"
	"	PF1-KP0	Steps one program expression\n"
	"	t	Sets trace mode\n"
	"	T	Unsets trace mode\n"
	"	v	Print last-expression value\n"
	"	?	Displays help informations\n"
    )
)
    (bind-to-key "debug" "\e\^d")
)
[debug.ml]
;
; DEBUG.ML
;
; A Mock Lisp debugger for EMACS V4.0
;
; Nick Emery and Barry Scott, 1985
; 
(save-window-excursion
i j
(temp-use-buffer "Debug")
(setq current-buffer-checkpointable 0)
(setq current-buffer-journalled 0)
(define-keymap "Debug")
(use-local-map "Debug")
(setq i 0)
(while (< i 256)
    (setq j (global-binding-of i))
    (if (| (= j "self-insert") (= j "digit"))
	(local-bind-to-key "illegal-operation" (char-to-string i))
    )
    (setq i (+ i 1))
)
(declare-global
    debug-show-source
    debug-show-last-expression
    ~debug-step-count
    ~debug-trace-mode
    debug-sit-for
    ~debug-help
    ~debug-old-error-hook
    ~debug-old-trace-hook
    ~debug-old-breakpoint-hook
    ~debug-old-global-mode-string
    ~debug-on
)
(setq ~debug-trace-mode 0)
(setq ~debug-on 0)
(defun
    (debug-error
	(if (!= (substr current-function 1 6) "debug-")
	    (save-window-excursion
		(debug-display)
		(erase-buffer)
		(message
		    (concat
			"Error `"
			error-message
			(if (!= current-function "")
			    (concat "\' occurred in function " current-function)
			    "' detected by DEBUG"
			)
		    )
		)
		(debug-show-source)
		(recursive-edit)
		(temp-use-buffer "Debug")
		(erase-buffer)
		(exit-emacs)
	    )
	)
    )
)
(defun
    (debug-show-source
	(erase-buffer)
	(if (& debug-show-source (!= current-function ""))
	    (progn
		(decompile-mlisp-function current-function)
		(set-mark)
		(end-of-line)
		(exchange-dot-and-mark)
	    )
	    (progn
		(insert-string (decompile-current-line))
		(unset-mark)
	    )
	)
	(if debug-show-last-expression
	    (save-window-excursion (debug-show-last-expression))
	)
    )
)
(defun
    (debug-trace
	(setq ~debug-step-count (- ~debug-step-count 1))
	(if (&
		(<= ~debug-step-count 0)
		(!= (substr current-function 1 6) "debug-")
	    )
	    (save-window-excursion
		(setq ~debug-step-count 0)
		(debug-display)
		(if (! ~debug-trace-mode)
		    (message (concat "Step in function " current-function))
		)
		(debug-show-source)
		(if (! ~debug-trace-mode)
		    (recursive-edit)
		    (sit-for debug-sit-for)
		)
		(temp-use-buffer "Debug")
		(erase-buffer)
	    )
	)
    )
)
(defun
    (debug-break
	(if (!= (substr current-function 1 6) "debug-")
	    (save-window-excursion
		(debug-display)
		(message
		    (concat
			"Breakpoint before function "
			current-breakpoint
			(if (!= current-function "")
			    (concat " called from function " current-function)
			    ""
			)
		    )
		)
		(debug-show-source)
		(recursive-edit)
		(temp-use-buffer "Debug")
		(erase-buffer)
	    )
	)
    )
)
(defun
    (debug-show-last-expression
	(debug-show-expression "last-expression" last-expression)
	(pop-to-buffer "Debug")
    )
)
(defun
    (debug-show-variable
	~debug-variable
	
	(setq ~debug-variable (get-tty-variable "Show variable "))
	(debug-show-expression
	    ~debug-variable
	    (execute-mlisp-line ~debug-variable))
	(pop-to-buffer "Debug")
    )
)
(defun
    debug-show-expression (name expr)
    type
    bounds
    
    (pop-to-buffer "Debug-variable")
    (setq current-buffer-checkpointable 0)
    (setq current-buffer-journalled 0)
    (erase-buffer)
    (setq type (type-of-expression expr))
    (insert-string (concat name " is type " type "\n"))
    (if (= type "integer")
	(insert-string (concat "Value " expr "\n"))
	(= type "string")
	(insert-string (concat "Value \"" expr "\""))
	(= type "marker")
	(insert-string (concat "Value \"" expr "\", " (+ expr)))
	(= type "windows")
	(insert-string "is a set of windows.")
	(= type "array")
	(progn
	    (setq bounds (bounds-of-array expr))
	    (insert-string
		(concat
		    "Has " (fetch-array bounds 1 0)
		    (if (= 1 (fetch-array bounds 1 0))
			" dimension\n"
			" dimensions\n"
		    )
		    "Element		Value\n"
		    "-------		-----\n"
		)
	    )
	    (debug-list-array expr bounds 1 "" "")
	)
	(insert-string (concat "unexpected type of " type))
    )
    (beginning-of-file)
)
(defun
    debug-list-array (array bounds level indices prefix)
    
    index high-bound expr type
    
    (setq index (fetch-array bounds 1 level))
    (setq high-bound (fetch-array bounds 2 level))
    (if (length prefix) (setq prefix (concat prefix ", ")))
    
    (if (= level (fetch-array bounds 1 0))
	(while (<= index high-bound)
	    (progn
		(insert-string (concat prefix index))
		(to-col 17)
		(setq expr
		    (execute-mlisp-line
			(concat "(fetch-array array " indices " index)")))
		(setq type (type-of-expression expr))
		(if (= type "integer")
		    (insert-string expr)
		    (= type "string")
		    (insert-string expr)
		    (= type "marker")
		    (insert-string (concat "\"" expr "\", " (+ expr)))
		    (= type "windows")
		    (insert-string "set of windows")
		    (insert-string (concat "unexpected type of " type))
		)
		(insert-string "\n")
		(setq index (+ index 1))
	    )
	)
	(progn
	    (setq level (+ level 1))

	    (while (<= index high-bound)
		(progn
		    (debug-list-array array bounds level
			(concat indices " " index) (concat prefix index))
		    (setq index (+ index 1))
		)
	    )
	)
    )
)
(defun
    (debug-mode-string
	(save-window-excursion
	    (temp-use-buffer "Debug")
	    (setq mode-string
		(if (= error-hook "debug-error") "Errors trapped" "")
	    )
	    (if ~debug-trace-mode
		(progn
		    (if (> (length mode-string) 0)
			(setq mode-string (concat mode-string ", "))
		    )
		    (setq mode-string
			(concat
			    mode-string
			    "Tracing"
			)
		    ))
	    )
	    (if (> (length mode-string) 0)
		(setq mode-string (concat " (" mode-string ")"))
	    )
	)
    )
)
(defun
    (debug-set-exception
	(setq error-hook "debug-error")
	(debug-mode-string)
    )
)
(defun
    (debug-cancel-exception
	(setq error-hook "")
	(debug-mode-string)
    )
)
(defun
    (debug-go
	(setq trace-into ~debug-trace-mode)
	(setq trace-mode ~debug-trace-mode)
	(if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug-step
	(setq trace-into 1)
	(setq trace-mode 1)
	(if prefix-argument-provided
	    (setq ~debug-step-count prefix-argument)
	    (setq ~debug-step-count 1)
	)
	(if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug-step-over
	(setq trace-into 0)
	(setq trace-mode 1)
	(if prefix-argument-provided
	    (setq ~debug-step-count prefix-argument)
	    (setq ~debug-step-count 1)
	)
	(if (> (recursion-depth) 0) (exit-emacs))
    )
)
(defun
    (debug
	(debug-display)
	(erase-buffer)
	(if (! ~debug-on)
	    (progn
		(setq mode-line-format "%M    Command level%m")
		(setq mode-string "")
		(setq ~debug-old-trace-hook trace-hook)
		(setq ~debug-old-breakpoint-hook breakpoint-hook)
		(setq ~debug-old-error-hook error-hook)
		(setq ~debug-old-global-mode-string global-mode-string)
		(setq trace-hook "debug-trace")
		(setq breakpoint-hook "debug-break")
		(setq error-hook "")
		(setq global-mode-string "DEBUG")
		(setq ~debug-on 1)
		(debug-mode-string)
	    )
	)
	(novalue)
    )
)
(defun
    (debug-set-break
	(breakpoint (get-tty-command "Set breakpoint (for function) ") 1)
    )
)
(defun
    (debug-can-break
	(breakpoint (get-tty-command "Cancel breakpoint (for function) ") 0)
    )
)
(defun
    (debug-show-breakpoints
	(save-window-excursion
	    (list-breakpoints)
	)
	(debug-display)
	(erase-buffer)
	(yank-buffer "Breakpoint list")
	(beginning-of-file)
    )
)
(defun
    (debug-show-calls
	(save-window-excursion
	    (dump-stack-trace)
	)
	(erase-buffer)
	(yank-buffer "Stack trace")
	(beginning-of-file)
	(provide-prefix-argument 6 (kill-to-end-of-line))
    )
)
(defun
    (debug-trace-mode
	(setq ~debug-trace-mode 1)
	(setq trace-mode 1)
	(setq ~debug-step-count 0)
	(debug-mode-string)
    )
)
(defun
    (debug-notrace-mode
	(setq ~debug-trace-mode 0)
	(setq trace-mode 0)
	(setq ~debug-step-count 0)
	(debug-mode-string)
    )
)
(defun
    (debug-exit
	(setq trace-hook ~debug-old-trace-hook)
	(setq breakpoint-hook ~debug-old-breakpoint-hook)
	(setq error-hook ~debug-old-error-hook)
	(setq global-mode-string ~debug-old-global-mode-string)
	(setq trace-mode 0)
	(pop-to-buffer "Debug")
	(delete-window)
	(if debug-show-last-expression
	    (progn
		(pop-to-buffer "Debug-variable")
		(delete-window)
	    )
	)
	(setq ~debug-on 0)
	(novalue)
    )
)
(defun
    (debug-help
	(save-window-excursion
	    (pop-to-buffer "Help")
	    (delete-other-windows)
	    (erase-buffer)
	    (insert-string ~debug-help)
	    (beginning-of-file)
	    (setq mode-line-format "")
	    (message " -- Type any character to continue DEBUG --")
	    (get-tty-character)
	    (setq mode-line-format default-mode-line-format)
	)
    )
)
; 
; put up the debugger windows onto the screen
; 
(defun
    (debug-display
	(pop-to-buffer "Debug")
	(if debug-show-last-expression
	    (if
		(save-window-excursion
		    (next-window)
		    (!= current-buffer-name "Debug-variable")
		)
		(progn
		    (if (< window-size 10)
			(setq window-size 10))
		    (split-current-window)
		    (switch-to-buffer "~debug-temp~")
		    (next-window)
		    (while (!= current-buffer-name "~debug-temp~")
			(if (= current-buffer-name "Debug-variable")
			    (delete-window)
			)
			(next-window)
		    )
		    (switch-to-buffer "Debug-variable")
		    (setq mode-line-format "Debug show expression %p")
		    (kill-buffer "~debug-temp~")
		    (setq window-size 3)
		    (previous-window)
		)
	    )
	)
    )
)
(execute-mlisp-file "debug.key")
(novalue)
)
[debug32.mlp]
(defun
    (debug32-com
	(visit-file (argv 1))	; the file to look at
	(if (> (argc) 2)
	    (progn
		(goto-line (argv 2))
		(beginning-of-line)
	    )
	)
	(if (> (argc) 3) (goto-character (+ (dot) (argv 3) -1) )
	)
    )
)
[dent.ml]
(progn
; 
; indent the region by logical-tab-size units
; and do it prefix-argument times.
; 
; note:
; 	If the argument is 0 this cleans up the tab s and spaces on
; 	the beginnings of lines.
; e.g
; "<space><tab>hello" => "<tab>hello"
; 

(defun
    (indent-region
	~indent-level		; amount of white space infront of line
	
	(message "Indenting region...") (sit-for 1)
	(save-restriction
	    (narrow-region)
	    (beginning-of-file)
	    (while
		(! (eobp))
		
		(if (! (eolp))
		    (progn
			(setq ~indent-level (current-indent))
			(delete-white-space)
			(to-col
			    (+
				~indent-level
				(* logical-tab-size prefix-argument)
			    )
			)
		    )
		)
		(next-line)
		(beginning-of-line)
	    )
	)
	(message "                  ...region Indented")
    )
)
(defun    
    (undent-region
	~indent-level
	
	(message "Undenting region...") (sit-for 1)
	(save-restriction
	    (narrow-region)
	    (beginning-of-file)
	    (while (! (eobp))
		(progn
		    (setq ~indent-level (current-indent))
		    (delete-white-space)
		    (to-col
			(-
			    ~indent-level
			    (* logical-tab-size prefix-argument)
			)
		    )
		    (next-line)
		    (beginning-of-line)
		)
	    )
	)
	(message "                  ...region Undented")
    )
    
)
)
[describex.ml]
(if (error-occurred (setq library-access (getenv "EMACS$LIBRARY_ACCESS")))
    (setq library-access 0))

(extend-database-search-list "describe" "emacs$library:emacsdesc" library-access)

(defun
    (describe-command word
	(setq word (get-tty-command ": describe-command "))
	(save-excursion
	    (temp-use-buffer "Help")
	    (erase-buffer)
	    (if (error-occurred
		    (fetch-database-entry "describe" (concat "C" word))
		)
		(error-message (concat "No help concerning " word))
		(progn old-pop-up-windows
		    (setq old-pop-up-windows pop-up-windows)
		    (setq pop-up-windows 1)
		    (pop-to-buffer "Help")
		    (setq pop-up-windows old-pop-up-windows)
		    (setq buffer-is-modified 0)
		    (beginning-of-file)
		)
	    )
	)
	(novalue)
    )
    
    (describe-variable word
	(setq word (get-tty-variable ": describe-variable "))
	(save-excursion
	    (temp-use-buffer "Help")
	    (erase-buffer)
	    (if (error-occurred
		    (fetch-database-entry "describe" (concat "V" word))
		)
		(error-message (concat "No help concerning " word))
		(progn old-pop-up-windows
		    (setq old-pop-up-windows pop-up-windows)
		    (setq pop-up-windows 1)
		    (pop-to-buffer "Help")
		    (setq pop-up-windows old-pop-up-windows)
		    (setq buffer-is-modified 0)
		    (beginning-of-file)
		)
	    )
	)
	(novalue)
    )
)
[describe_word_list.database]
(progn
(error-occurred (extend-database-search-list "subr-names"
	"emacs$library:emacs_qinfo_bliss" library-access))
(error-occurred (extend-database-search-list "subr-names"
	"emacs$library:emacs_qinfo_dcl" library-access))
(error-occurred (extend-database-search-list "subr-names"
	"emacs$library:emacs_qinfo_system_services" library-access))
(error-occurred (extend-database-search-list "subr-names"
	"emacs$library:emacs_qinfo_vaxc" library-access))
)
[desword.ml]
(progn
library-access
(if (error-occurred (setq library-access (getenv "EMACS$LIBRARY_ACCESS")))
    (setq library-access 0)
)
(execute-mlisp-file "describe_word_list.database")
(error-occurred
    (extend-database-search-list "subr-names"
	(concat "SYS$login:subrnames") library-access
    )
)

(declare-global describe-word-window-size)
(declare-global describe-word-keep-window)

(save-excursion
    (temp-use-buffer "syntax-table-hack")
    (use-syntax-table "describe-word")
    (modify-syntax-table "dull" "!-~")
    (modify-syntax-table "word" "A-Za-z0-9$_%")
)
(defun
    (describe-word-in-buffer subr-name syntax-tab
	(setq syntax-tab current-buffer-syntax-table)
	(use-syntax-table "describe-word")
	(if (> prefix-argument 1)
	    (save-window-excursion
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table syntax-tab)
		(edit-description (region-to-string))
	    )
	    (progn ~current-windows ~original-buffer
		(setq ~current-windows current-windows)
		(setq ~original-buffer current-buffer-name)
		(save-excursion
		    (error-occurred (forward-character))
		    (backward-word)
		    (set-mark)
		    (forward-word)
		    (use-syntax-table syntax-tab)
		    (setq subr-name (region-to-string))
		)
		(temp-use-buffer "Subroutine-Help")
		(setq current-buffer-checkpointable 0)
		(setq current-buffer-journalled 0)
		(setq mode-line-format "%b: %m")
		(erase-buffer)
		(if (error-occurred
			(fetch-database-entry "subr-names" subr-name))
		    (progn
			(delete-buffer "Subroutine-Help")
			(error-message "No description for " subr-name)
		    )
		    (if (> describe-word-window-size 0)
			(progn
			    (pop-to-buffer "Minibuf")
			    (previous-window)
			    (error-occurred (split-current-window))
			    (switch-to-buffer "Subroutine-Help")
			    (setq mode-string subr-name)
			    (error-occurred (while 1 (shrink-window)))
			    (error-occurred
				(while (<= window-size describe-word-window-size)
				    (enlarge-window)
				)
			    )
			    (beginning-of-file)
			    (if (! describe-word-keep-window)
				(push-back-character (get-tty-character))
			    )
			)
			(progn
			    (beginning-of-file)
			    (set-mark)
			    (end-of-line)
			    (message (region-to-string))
			)
		    )
		)
		(if describe-word-keep-window
		    (pop-to-buffer ~original-buffer)
		    (setq current-windows ~current-windows)
		)
	    )
	)
	(novalue)
    )
)
(defun    
    (edit-description ~edit-name
	(setq ~edit-name (arg 1 ": edit-description (of key) "))
	(pop-to-buffer "Edit description")
	(erase-buffer)
	(if (error-occurred (fetch-database-entry "subr-names" ~edit-name))
	    (message "New entry."))
	(setq mode-string (concat "Editing database entry for " ~edit-name))
	(setq mode-line-format "%[%m%]")
	(local-bind-to-key "exit-emacs" "\^X\^S")
	(local-bind-to-key "exit-emacs" "\^X\^F")
	(local-bind-to-key "exit-emacs" "\^X\^W")
	(recursive-edit)
	(temp-use-buffer "Edit description")
	(put-database-entry "subr-names" ~edit-name)
	(error-occurred (delete-buffer "Edit description"))
	(novalue)
    )
)
)
[dired.ml]
; 
; dired, the directory editor, version V3.0
; This version allows non-sub-process directories using the
; expand-file-name function. To get this, you must pre-declare
; the global dired-directory-command, and set it to the string
; FAST (in big letters), or after you load dired, you must
; set this variable to a null string.
; 
(if (= operating-system-name "VMS")
    (progn
	(progn
	(declare-global ~dired-directory dired-directory-command)
	(if (= (concat dired-directory-command) "0"
	)
	    (setq dired-directory-command 
		"DIRECTORY/NOHEAD/NOTRAIL/COL=1/SIZE=ALL/DATE"
	    )
	)
	(defun
	    (~dired-line
		(insert-string (concat "\t" (process-output "dired")))
	    )
	    (dired
		~dired-fspec
		(setq ~dired-fspec (arg 1 ": dired (file specification) "))
		(if (<= (length ~dired-fspec) 0)
		    (setq ~dired-fspec "*.*;*")
		)
		(if (| (interactive) (!( |
					   (<= (length dired-directory-command) 0)
					   (= dired-directory-command "FAST")
				       )
				     )
		    )
		    (progn
			(message ": dired (file specification) "
			    ~dired-fspec
			    ", please wait..."
			)
			(sit-for 0)
		    )
		)
		(save-window-excursion
		    (switch-to-buffer "~dired 0")
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (widen-region)
		    (erase-buffer)
		    (insert-string (concat "	" (expand-file-name ~dired-fspec)))
		    (if (provide-prefix-argument -1 (looking-at "[]>].;"))
			(progn
			    (setq ~dired-fspec (concat ~dired-fspec "*.*;*"))
			    (erase-buffer)
			    (insert-string (concat "	" (expand-file-name ~dired-fspec)))
			)
		    )
		    (if (provide-prefix-argument -1 (looking-at "*.*;*"))
			(progn
			    (provide-prefix-argument 5 (delete-previous-character))
			    (insert-string "No such directory.")
			)
		    )
		    (save-excursion
			(beginning-of-line)
			(forward-character)
			(set-mark)
			(re-search-forward "[]>]")
			(case-region-capitalize)
			(setq ~dired-directory (region-to-string))
		    )
		    (if (= (preceding-char) ';')
			(erase-buffer)
			( |
			    (<= (length dired-directory-command) 0)
			    (= dired-directory-command "FAST")
			)
			(while
			    (!
				(error-occurred
				    (insert-string
					(concat
					    "\n\t"
					    (expand-file-name "")
					)
				    )
				)
			    )
			    (novalue)
			)
			(progn
			    (erase-buffer)
			    (error-occurred (kill-process "dired"))
			    (start-DCL-process "dired")
			    (set-process-output-procedure "dired" "~dired-line")
			    (while (= (wait-for-process-input-request "dired") 0)
				(get-tty-character)
			    )
			    (send-string-to-process "dired"
				(concat
				    "$ "
				    dired-directory-command
				    " "
				    ~dired-fspec
				)
			    )
			    (while (= (wait-for-process-input-request "dired") 0)
				(get-tty-character)
			    )
			    (error-occurred (kill-process "dired"))
			)
		    )
		    (if (! (bobp)) (insert-character '\n'))
		    (setq mode-string ~dired-directory)
		    (setq mode-line-format "DIRED directory editor    Directory: %m    %p")
		    (use-local-map "~dired-map")
		    (use-syntax-table "~dired-syntax")
		    (beginning-of-file)
		    (if (error-occurred (re-replace-string "\t.*[]>]" "\t"))
			(insert-string "\tNo files found.")
		    )
		    (save-excursion
			(beginning-of-file)
			(while (! (eobp))
			    (progn
				(forward-character)
				(if (= (following-char) ' ')
				    (progn
					(forward-word)
					(goto-character (- (dot) 6))
					(insert-string ".")
					(backward-character)
					(delete-white-space)
					(delete-previous-character)
					(delete-white-space)
					(to-col 48)
					(delete-next-character)
				    )
				)
				(beginning-of-line) (next-line)
			    )
			)		    
		    )
		    (save-excursion
			(set-mark)
			(end-of-file)
			(case-region-capitalize)
		    )
		    (delete-other-windows)
		    (recursive-edit)
		    (while (~dired-continue) (recursive-edit))
		)
		(if (is-bound ~DIRED-com)
		    (if ~DIRED-com
			(progn
			    (setq ~DIRED-com 0)
			    (exit-emacs)
			)
		    )
		)
		(error-occurred (delete-buffer "~dired 0"))
		(error-occurred (delete-buffer "~dired 1"))
		(error-occurred (delete-buffer "~dired 2"))
		(novalue)
	    )
	    (~dired-continue
		~dired-char
		(save-window-excursion
		    (switch-to-buffer "~dired 1")
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (erase-buffer)
		    (yank-buffer "~dired 0")
		    (beginning-of-file)
		    (error-occurred (re-replace-string "^\t.*$" ""))
		    (error-occurred (re-replace-string "^\n" ""))
		    (error-occurred (re-replace-string "^D\t\\(.*;[0-9][0-9]*\\).*$" "\\1"))
		    (switch-to-buffer "~dired 2")
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (setq mode-line-format "")
		    (erase-buffer)
		    (yank-buffer "~dired 1")
		    (beginning-of-file)
		    (if (eobp)
			(progn
			    (message "No files deleted.")
			    0
			)
			(progn
			    (while (! (eobp))
				(to-col 8)
				(end-of-line)
				(error-occurred
				    (delete-next-character)
				    (to-col 33)
				    (end-of-line)
				    (delete-next-character)
				    (to-col 56)
				)
				(next-line)
				(if (! (eobp))
				    (beginning-of-line)
				)
			    )
			    (beginning-of-file)
			    (insert-string
				(concat
				    "\nDeleting the following files in directory "
				    ~dired-directory
				    "\n\n"
				)
			    )
			    (end-of-file)
			    (insert-string
				(concat
				    "\n\nY -- Go ahead\n"
				    "R -- Don't delete, resume DIRED\n"
				    "Anything else exits DIRED without deleting\n"
				)
			    )
			    (message "OK? ")
			    (setq ~dired-char (get-tty-character))
			    (if (| (= ~dired-char 'Y') (= ~dired-char 'y'))
				(progn (message "OK? YES") (~dired-nuke) 0)
				(| (= ~dired-char 'R') (= ~dired-char 'r'))
				(progn (message "OK? RESUME") 1)
				(progn (message "OK? NO") 0)
			    )
			)
		    )
		)
	    )
	    (~dired-nuke
		(temp-use-buffer "~dired 1")
		(beginning-of-file)
		(while (! (eobp))
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message "Deleting file " ~dired-directory (region-to-string))
		    (save-excursion (switch-to-buffer "~dired 2") (sit-for 0))
		    (if (< 0 (unlink-file (concat ~dired-directory (region-to-string)))
			)
			(get-tty-string
			    (concat
				"Failed to delete file "
				~dired-directory
				(region-to-string)
				". Press RETURN to continue")
			)
		    )
		    (next-line)
		)
	    )
	    (~dired-n
		(next-line)
		(beginning-of-line)
	    )
	    (~dired-p
		(previous-line)
		(beginning-of-line)
	    )
	    (~dired-d
		(beginning-of-line)
		(if (looking-at "^\t")
		    (insert-character 'D')
		)
		(~dired-n)
	    )
	    (~dired-u
		(beginning-of-line)
		(if (looking-at "^D\t")
		    (delete-next-character)
		)
		(~dired-n)
	    )
	    (~dired-h
		(save-window-excursion
		    (switch-to-buffer "~dired help")
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (delete-other-windows)
		    (erase-buffer)
		    (insert-string
			(concat
			    "DIRED:  directory editor\n\n"
			    "n	next line\n"
			    "p	prev line\n"
			    "d	mark this line's file for deletion\n"
			    "u	undelete this line's file\n"
			    "e	view this line's file\n"
			    "q	quit DIRED (you will be prompted to confirm)\n\n"
			    "Other characters perform their usual functions."
			)
		    )
		    (setq mode-line-format "")
		    (message "-- Type any character to continue DIRED --" )
		    (get-tty-character)
		)
		(delete-buffer "~dired help")
		(novalue)
	    )
	    (~dired-e
		(save-window-excursion
		    (beginning-of-line)
		    (if (! (error-occurred (re-search-forward "^.*\t\\(.*;[0-9][0-9]*\\).*$")))
			(progn
			    (region-around-match 1)
			    (setq ~dired-fspec
				(concat
				    ~dired-directory
				    (region-to-string)
				)
			    )
			    (switch-to-buffer "DIRED view")
			    (setq current-buffer-checkpointable 0)
			    (setq current-buffer-journalled 0)
			    (delete-other-windows)
			    (erase-buffer)
			    (normal-mode)
			    (insert-file ~dired-fspec)
			    (beginning-of-file)
			    (message "Use exit-emacs to resume DIRED")
			    (setq mode-line-format
				(concat
				    "DIRED view   File: "
				    ~dired-fspec
				    "   %p"
				)
			    )
			    (recursive-edit)
			    (error-occurred (delete-buffer "DIRED view"))
			)
		    )
		)
	    )
	)
	(save-excursion
	    (temp-use-buffer "Keymap hack")
	    (define-keymap "~dired-map")
	    (use-local-map "~dired-map")
	    (local-bind-to-key "~dired-n" "n")
	    (local-bind-to-key "~dired-n" "N")
	    (local-bind-to-key "~dired-p" "p")
	    (local-bind-to-key "~dired-p" "P")
	    (local-bind-to-key "~dired-d" "d")
	    (local-bind-to-key "~dired-u" "U")
	    (local-bind-to-key "~dired-u" "u")
	    (local-bind-to-key "~dired-e" "e")
	    (local-bind-to-key "~dired-e" "E")
	    (local-bind-to-key "~dired-h" "h")
	    (local-bind-to-key "~dired-h" "H")
	    (local-bind-to-key "~dired-h" "?")
	    (local-bind-to-key "~dired-h" "")
	    (local-bind-to-key "exit-emacs" "q")
	    (local-bind-to-key "exit-emacs" "Q")
	    (use-syntax-table "~dired-syntax")
	)
	(delete-buffer "Keymap hack")
	(novalue)
	)
    )
    (progn
	(declare-global Dired-keymap-defined Dired-directory)
	(setq Dired-keymap-defined 0)
	(setq Dired-directory (current-directory))

	(defun
	    (dired directory
		(save-window-excursion
		    (setq directory (arg 1 (concat ": dired on directory? ["
					       Dired-directory "] ")))
		    (if (= directory "")
			(setq directory Dired-directory))
		    (if (= "/" (substr directory -1 1))
			(setq directory (substr directory 1 -1)))
		    (setq Dired-directory directory)
		    (pop-to-buffer "dired")
		    (setq mode-line-format
			(concat "  Editing  directory:  " directory
			    "      %M   %[%p%]"))
		    (erase-buffer)  (set-mark)
		    (message (concat "Getting contents of " directory))
		    (sit-for 0)
		    (filter-region (concat "/bin/ls -l "
						(expand-file-name directory)))
		    (beginning-of-file)
		    (if (looking-at "total")
			(progn
			    (kill-to-end-of-line) (kill-to-end-of-line)
			    (beginning-of-file)
			    (re-replace-string "^." " &")
			    (beginning-of-file)
			)
			(progn 
			    (end-of-file)
			    (error-message (region-to-string))
			)
		    )
		    (message "Type ^C to exit, ? for help")
		    (save-excursion (recursive-edit))
		    (~dired-done)
		    (novalue)
		)
	    )
	)

	(defun    
	    (~dired-Mark-file-deleted
		(if (= 0 (buffer-size))
		    (error-message "dired already done!")
		    (progn
			(beginning-of-line)
			(if (looking-at " d")
			    (error-message "Can't delete a directory with dired"))
			(delete-next-character)
			(insert-string "D")
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    
	    (~dired-summary
		(message
		    "d-elete, u-ndelete, q-uit, r-emove, e,v-isit, n-ext, p-revious, ?-help")
	    )

	    (~dired-UnMark-file-deleted
		(if (= 0 (buffer-size))
		    (error-message "dired already done!")
		    (progn
			(beginning-of-line)
			(delete-next-character)
			(insert-string " ")
			(next-line)
			(beginning-of-line)
		    )
		)
	    )
	    
	    (~dired-backup-unmark
		(if (= 0 (buffer-size))
		    (error-message "dired already done!")
		    (! (bobp))
		    (previous-line))
		(beginning-of-line)
		(delete-next-character)
		(insert-string " ")
		(beginning-of-line)
	    )
	)

	(defun
	    (~dired-examine ans
		(save-excursion
		    (error-occurred
			(visit-file (~dired-get-fname))
			(message "Type ^C to return to DIRED")
			(recursive-edit)
		    )
		)
	    )
	)

	(defun    
	    (~dired-remove
		(beginning-of-line)
		(kill-to-end-of-line) (kill-to-end-of-line)
	    )
	    
	    (~dired-get-fname
		(save-excursion
		    (beginning-of-line)
			(re-search-forward "[0-9][0-9]:[0-9][0-9] ")
	;	    (goto-character (+ (dot) 43))
		    (set-mark)
		    (end-of-line)
		    (concat Dired-directory "/" (region-to-string))
		)
	    )
	    
	    (~dired-done ans
		(beginning-of-file)
		(re-replace-string "^ .*\n" "")
		(if (!= 0 (buffer-size))
		    (progn
			(message
			    "? [y-go through marked files; e-don't delete, exit; Anything else return]")
			(setq ans (get-tty-character))
			(if (| (= ans 'e') (= ans 'E'))
			    (progn
				(message
				    "Really exit without deleting?[y-yes, anything else continue dired]")
				(setq ans (get-tty-character))
				(if (| (= ans 'y') (= ans 'Y'))
				    (delete-buffer "dired")
				    (error-message "Aborted.")))
			    (| (= ans 'y') (= ans 'Y'))
			    (progn
				(while (! (eobp))
				    (if (= (following-char) 'D')
					(progn thisfile ans
					    (setq thisfile (~dired-get-fname))
					    (message (concat "Delete " thisfile "?"))
					    (setq ans (get-tty-character))
					    (if (| (= ans 'y') (= ans 'Y'))
						(if (unlink-file thisfile)
						    (progn
							(message "Couldn't delete it!")
							(sit-for 2))))
					)
				    )
				    (next-line)
				)
				(delete-buffer "dired")
			    )
			    (error-message "?Aborted.")
			)			
		    )
		)
	    )
	)    
	    
	    (progn loop
		(save-excursion
		    (temp-use-buffer "dired")
		    (define-keymap "dired-keymap")
		    (use-local-map "dired-keymap")
		    (setq loop 0)
		    (while (<= loop 127)
			(local-bind-to-key "~dired-summary" loop)
			(setq loop (+ loop 1))
		    )
		    (setq loop '0')
		    (while (<= loop '9')
			(local-bind-to-key "digit" loop)
			(setq loop (+ loop 1))
		    )
		    (local-bind-to-key "~dired-Mark-file-deleted" "d")
		    (local-bind-to-key "~dired-Mark-file-deleted" "D")
		    (local-bind-to-key "~dired-Mark-file-deleted" "")
		    (local-bind-to-key "~dired-backup-unmark" "\0177")
		    (local-bind-to-key "previous-line" "\^H")
		    (local-bind-to-key "previous-line" "p")
		    (local-bind-to-key "previous-line" "P")
		    (local-bind-to-key "previous-line" "\^P")
		    (local-bind-to-key "next-line" "n")
		    (local-bind-to-key "next-line" "N")
		    (local-bind-to-key "next-line" "\^N")
		    (local-bind-to-key "next-line" 13)
		    (local-bind-to-key "next-line" 10)
		    (local-bind-to-key "next-line" " ")
		    (local-bind-to-key "argument-prefix" "\^U")
		    (local-bind-to-key "previous-window" "\^Xp")
		    (local-bind-to-key "previous-window" "\^XP")
		    (local-bind-to-key "next-window" "\^Xn")
		    (local-bind-to-key "next-window" "\^XN")
		    (local-bind-to-key "delete-window" "\^Xd")
		    (local-bind-to-key "delete-window" "\^XD")
		    (local-bind-to-key "delete-other-windows" "\^X1")
		    (local-bind-to-key "~dired-examine" "\^X\^V")
		    (local-bind-to-key "next-page" "\^V")
		    (local-bind-to-key "previous-page" "\ev")
		    (local-bind-to-key "previous-page" "\eV")
		    (local-bind-to-key "beginning-of-file" "\e<")
		    (local-bind-to-key "end-of-file" "\e>")
		    (local-bind-to-key "~dired-UnMark-file-deleted" "u")
		    (local-bind-to-key "~dired-UnMark-file-deleted" "U")
		    (local-bind-to-key "exit-emacs" "\^C")
		    (local-bind-to-key "exit-emacs" "q")
		    (local-bind-to-key "exit-emacs" "Q")
		    (local-bind-to-key "~dired-examine" "e")
		    (local-bind-to-key "~dired-examine" "E")
		    (local-bind-to-key "~dired-examine" "v")
		    (local-bind-to-key "~dired-examine" "V")
		    (local-bind-to-key "~dired-remove" "r")
		    (local-bind-to-key "~dired-remove" "R")
		)
	    )
    )
)
[dired.mlp]
(defun
    (dired-com i
	(declare-global ~DIRED-com)
	(setq ~DIRED-com 1)
	(setq checkpoint-frequency 0)
	(setq silently-kill-processes 1)
	(execute-mlisp-file "dired")
	(dired
	    (if (> (argc) 1) (argv 1) "")
	)
    )
)
[dvidef.ml]
; 
; load up $dvidef symbols
; 
(if
    (if (is-bound $dvidef-loaded)
	(! $dvidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "dvi_def")
	(declare-global $dvidef-loaded)
	(setq $dvidef-loaded 1)
    )
)
[dvi_def.ml]
; 
; $dvidef
; 
(progn
    (~sys_literal_setup_incr "dvi$_" 2 2
	    "devchar"
	    "devclass"
	    "devtype"
	    "devbufsiz"
	    "devdepend"
	    "unit"
	    "pid"
	    "ownuic"
	    "vprot"
	    "errcnt"
	    "opcnt"
	    "recsiz"
	    "maxblock"
	    "devdepend2"
	    "refcnt"
	    "devnam"
	    "volnam"
	    "sectors"
	    "tracks"
	    "cylinders"
	    "freeblocks"
	    "logvolnam"
	    "volnumber"
	    "volcount"
	    "rootdevnam"
	    "nextdevnam"
	    "transcnt"
	    "mountcnt"
	    "cluster"
	    "maxfiles"
	    "serialnum"
	    "acppid"
	    "acptype"
	    "concealed"



	    "rec"
	    "ccl"
	    "trm"
	    "dir"
	    "sdi"
	    "sqd"
	    "spl"
	    "opr"
	    "rct"
	    "net"
	    "fod"
	    "dua"
	    "shr"
	    "gen"
	    "avl"
	    "mnt"
	    "mbx"
	    "dmt"
	    "elg"
	    "all"
	    "for"
	    "swl"
	    "idv"
	    "odv"
	    "rnd"
	    "rtm"
	    "rck"
	    "wck"




	    "tt_passall"
	    "tt_noecho"
	    "tt_notypeahd"
	    "tt_escape"
	    "tt_hostsync"
	    "tt_ttsync"
	    "tt_script"
	    "tt_lower"
	    "tt_mechtab"
	    "tt_wrap"
	    "tt_crfill"
	    "tt_lffill"
	    "tt_scope"
	    "tt_remote"
	    "tt_eightbit"
	    "tt_mbxdsabl"
	    "tt_nobrdcst"
	    "tt_readsync"
	    "tt_mechform"
	    "tt_halfdup"
	    "tt_modem"
	    "tt_oper"
	    "tt_page"



	    "tt_localecho"
	    "tt_autobaud"
	    "tt_hangup"
	    "tt_modhangup"
	    "tt_brdcstmbx"
	    "tt_dma"
	    "tt_altypeahd"
	    "tt_setspeed"
	    "tt_dcl_mailbx"
	    "tt_editing"
	    "tt_insert"
	    "tt_fallback"
	    "tt_dialup"
	    "tt_secure"
	    "tt_disconnect"
	    "tt_pasthru"
	    "tt_sixel"
	    "tt_drcs"
	    "tt_printer"
	    "tt_app_keypad"
	    "tt_syspwd"
	    "tt_ansicrt"
	    "tt_regis"
	    "tt_block"
	    "tt_avo"
	    "tt_edit"
	    "tt_deccrt"


	    "sts"
	    "devsts"
	    "devchar2"
	    "fulldevnam"
	    "lockid"
	    "alldevnam"
	    "volsetmem"
	    "devlocknam"



	    "alloclass"
	    "alt_host_avail"
	    "alt_host_name"
	    "alt_host_type"
	    "host_avail"
	    "host_count"
	    "host_name"
	    "host_type"
	    "remote_device"
	    "served_device"
	    "shdw_catchup_copying"
	    "shdw_master"
	    "shdw_master_name"
	    "shdw_member"
	    "shdw_merge_copying"
	    "shdw_next_mbr_name"



	    "tt_phydevnam"
	    "tt_deccrt2"

	    "media_name"
	    "media_type"
	    "media_id"



	    "shdw_failed_member"
    )
    (~sys_literal_setup_equal "dvi$"
	"m_secondary"   1
	"m_noredirect"  32768
    )
    (~sys_literal_setup_incr "dvi$c_" 1 1
	"acp_f11v1"
	"acp_f11v2"
	"acp_mta"
	"acp_net"
	"acp_rem"
    )
)
[edtsim.ml]
(progn
    (declare-global ~EDT-direction ~EDT-sect-length ~EDT-page-marker
    		    ~EDT-search-marker ~EDT-set-mark ~EDT-help)
    (setq ~EDT-direction 1)
    (setq ~EDT-sect-length 16)
    (setq ~EDT-page-marker "\^k\\|\^l")
    (setq ~EDT-search-marker 0)
    (setq ~EDT-set-mark 0)
    (error-occurred (setq global-mode-string "EDT"))
    (setq return-to-emacs-hook
	(setq enter-emacs-hook "EDT-keypad"))
    (setq exit-emacs-hook
	(setq leave-emacs-hook "EDT-no-keypad"))
    (autoload "~TM-justify-region" "textmode")
    (defun
	(~EDT-check-mark x y
	    (setq x (arg 1))
	    (setq y (arg 2))
	    (if (> x y)
		(progn (setq x (arg 2)) (setq y (arg 1)))
	    )
	    (if (& (>= ~EDT-set-mark x) (<= ~EDT-set-mark y))
		(setq ~EDT-set-mark 0)
	    )
	)
	(EDT-keypad
	    (send-string-to-terminal "\e=")
	)
	(EDT-no-keypad
	    (if (& terminal-is-terminal (! terminal-application-keypad))
		(send-string-to-terminal "\e>")
	    )
        )
	(EDT-find x y
	    (setq y prefix-argument)
	    (setq x (arg 1 "Search for: "))
	    (if ~EDT-direction
		(provide-prefix-argument y (search-forward x))
		(provide-prefix-argument y (search-reverse x))
	    )
	    (setq y (+ (dot) 0))
	    (save-excursion
		(region-around-match 0)
		(setq ~EDT-search-marker
		    (if (> (+ (mark) 0) y) (dot) (mark))
		)
	    )
	)
	(EDT-fndnxt y
	    (setq y prefix-argument)
	    (if ~EDT-direction
		(provide-prefix-argument y (search-forward ""))
		(provide-prefix-argument y (search-reverse ""))
	    )
	    (setq y (+ (dot) 0))
	    (save-excursion
		(region-around-match 0)
		(setq ~EDT-search-marker
		    (if (> (+ (mark) 0) y) (dot) (mark))
		)
	    )
	)
	(EDT-replace
	    (if (| ~EDT-set-mark ~EDT-search-marker)
		(save-excursion
		    (set-mark)
		    (goto-character
			(if ~EDT-set-mark ~EDT-set-mark ~EDT-search-marker)
		    )
		    (erase-region)
		    (error-occurred (yank-buffer "Paste-0"))
		    (if ~EDT-set-mark (setq ~EDT-set-mark 0))
		)
		(error-message "No select range active")
	    )
	)
	(EDT-subs y
	    (setq y prefix-argument)
	    (while (>= y 1)
		(EDT-replace)
		(EDT-fndnxt)
		(setq y (- y 1))
	    )
	)
	(EDT-del-l y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (while (>= y 1)
		(save-excursion
		    (set-mark)
		    (end-of-line)
		    (error-occurred (forward-character))
		    (~EDT-check-mark (mark) (dot))
		    (delete-region-to-buffer "del-l")
		)
		(setq y (- y 1))
	    )
	)
	(EDT-und-l y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (while (>= y 1)
		(error-occurred (yank-buffer "del-l"))
		(setq y (- y 1))
	    )
	)
	(EDT-advance (setq ~EDT-direction 1))
	(EDT-backup (setq ~EDT-direction 0))
	(~EDT-advance-mini
	    (setq ~EDT-direction 1)
	    (insert-character "\n")
	    (exit-emacs)
	)
	(~EDT-backup-mini
	    (setq ~EDT-direction 0)
	    (insert-character "\n")
	    (exit-emacs)
	)
	(~EDT-enter
	    (insert-character "\n")
	    (exit-emacs)
	)
	(EDT-not-implemented
	    (setq ~EDT-search-marker 0)
	    (error-message "This key sequence is not implemented")
	)
	(EDT-repeat y x
	    (setq y 0)
	    (setq x (last-key-struck))
	    (while (& (>= x '0') (<= x '9'))
		(setq y (+ (* y 10) (- x '0')))
		(message y)
		(setq x (get-tty-character))
	    )
	    (push-back-character x)
	    (return-prefix-argument y)
	)
	(EDT-cut x y
	    (setq ~EDT-search-marker 0)
	    (~EDT-check-mark (mark) (dot))
	    (delete-region-to-buffer "Paste-0")
	    (unset-mark)
	)
	(EDT-paste x
	    (setq ~EDT-search-marker 0)
	    (setq x prefix-argument)
	    (while (>= x 1)
		(yank-buffer "Paste-0")
	   	(setq x (- x 1))
	    )
	)
	(EDT-word y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (if ~EDT-direction
		(provide-prefix-argument y (forward-word))
		(provide-prefix-argument y (backward-word))
	    )
	)
	(EDT-bol y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(if ~EDT-direction
		    (progn
			(if (bolp)
			    (previous-line)
			)
			(beginning-of-line)
		    )
		    (progn
			(next-line)
			(beginning-of-line)
		    )
		)
		(setq y (- y 1))
	    )
	)
	(EDT-fill ~old-right-margin
	    (setq ~EDT-search-marker 0)
	    (~EDT-check-mark (mark) (dot))
	    (if (= (setq ~old-right-margin right-margin) 10000)
		(setq right-margin (- terminal-width 2))
	    )
	    (save-excursion
		(if (< (+ (dot)) (+ (mark)))
		    (exchange-dot-and-mark))
		(insert-character '\n')
		(~TM-justify-region)
	    )
	    (setq right-margin ~old-right-margin)
	    (unset-mark)
	)
	(EDT-eol y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(if ~EDT-direction
		    (progn
			(if (eolp)
			    (next-line)
			)
			(end-of-line)
		    )
		    (progn
			(previous-line)
			(end-of-line)
		    )
		)
		(setq y (- y 1))
	    )
	)
	(EDT-char y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (if ~EDT-direction
		(provide-prefix-argument y (forward-character))
		(provide-prefix-argument y (backward-character))
	    )
	)
	(EDT-del-eol y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(save-excursion
		    (set-mark)
		    (EDT-eol)
		    (~EDT-check-mark (mark) (dot))
		    (delete-region-to-buffer "del-l")
		)
		(setq y (- y 1))
	    )
	)
	(EDT-line y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(if ~EDT-direction
		    (next-line)
		    (if (bolp) (previous-line))
		)
		(beginning-of-line)
		(setq y (- y 1))
	    )
	)
	(EDT-noop (novalue))
	(EDT-del-c y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(save-excursion
		    (set-mark)
		    (forward-character)
		    (delete-region-to-buffer "del-c")
		)
		(setq y (- y 1))
	    )
	)
	(EDT-und-c  y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (yank-buffer "del-c"))
	)
	(EDT-del-w y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (while (>= y 1)
		(save-excursion
		    (set-mark)
		    (forward-word)
		    (~EDT-check-mark (mark) (dot))
		    (delete-region-to-buffer "del-w")
		)
		(setq y (- y 1))
	    )
	)
	(EDT-und-w y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (provide-prefix-argument y (yank-buffer "del-w"))
	)
	(EDT-specins
	    (setq ~EDT-search-marker 0)
	    (if prefix-argument-provided
		(if (> prefix-argument 255)
		    (error-message "Illegal character code")
		    (insert-character prefix-argument)
		)
		(insert-character 0)
	    )
	)
	(EDT-page y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (if (error-occurred
		    (if ~EDT-direction
		        (provide-prefix-argument y (re-search-forward ~EDT-page-marker))
		        (provide-prefix-argument y (re-search-reverse ~EDT-page-marker))
		    )
		)
		(if ~EDT-direction (end-of-file) (beginning-of-file))
	    )
	)
	(EDT-sect y
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (while (>= y 1)
		(error-occurred
		    (provide-prefix-argument 
			(if (> window-size ~EDT-sect-length)
			    ~EDT-sect-length
			    (/ (* window-size 4) 5)
			)
			(EDT-line)
		    )
		)
		(setq y (- y 1))
	    )
	)
	(EDT-sect-back y z
	    (setq ~EDT-search-marker 0)
	    (setq y prefix-argument)
	    (setq z ~EDT-direction)
	    (EDT-backup)
	    (while (>= y 1)
		(error-occurred
		    (provide-prefix-argument 
			(if (> window-size ~EDT-sect-length)
			    ~EDT-sect-length
			    (/ (* window-size 4) 5)
			)
			(EDT-line)
		    )
		)
		(setq y (- y 1))
	    )
	    (setq ~EDT-direction z)
	)
	(EDT-bottom
	    (setq ~EDT-search-marker 0)
	    (end-of-file)
	)    
	(EDT-top
	    (setq ~EDT-search-marker 0)
	    (beginning-of-file)
	)    
	(EDT-open-line y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (newline-and-backup))
	)
	(EDT-up y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (previous-line))
	)
	(EDT-down y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (next-line))
	)
	(EDT-left y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (backward-character))
	)
	(EDT-right y
	    (setq y prefix-argument)
	    (setq ~EDT-search-marker 0)
	    (provide-prefix-argument y (forward-character))
	)
	(EDT-chngcase y x
	    (setq y prefix-argument)
	    (if (| ~EDT-set-mark ~EDT-search-marker)
		(save-excursion
		    (set-mark)
		    (goto-character
			(if ~EDT-set-mark ~EDT-set-mark ~EDT-search-marker)
		    )
		    (case-region-invert)
		    (if ~EDT-set-mark (setq ~EDT-set-mark 0))
		)
		(while (>= y 1)
		    (setq x (following-char))
		    (if (& (>= x 'A') (<= x 'Z'))
			(progn
			    (delete-next-character)
			    (insert-character (+ (- x 'A') 'a'))
			    (backward-character)
			)
			(& (>= x 'a') (<= x 'z'))
			(progn
			    (delete-next-character)
			    (insert-character (+ (- x 'a') 'A'))
			    (backward-character)
			)
		    )
		    (EDT-char)
		    (setq y (- y 1))
		)
	    )
	)
	(EDT-select
	    (setq ~EDT-search-marker 0)
	    (if ~EDT-set-mark
		(error-message "Select range already active")
	    )
	    (setq ~EDT-set-mark (+ (dot) 0))
	    (set-mark)
	)
	(EDT-reset
	    (setq ~EDT-search-marker 0)
	    (setq ~EDT-set-mark 0)
	    (unset-mark)
	)
	(EDT-help x i
	    (if (! ~EDT-help)
		(save-window-excursion (error-occurred
		    (message "Formatting help. Please wait...") (sit-for 0)
		    (switch-to-buffer "EDT-help")
		    (erase-buffer)
		    (setq mode-line-format "")
		    (extend-database-search-list "EDT-help" "SYS$HELP:EDTHELP.HLB")
	 	    (setq x fetch-help-database-flags)
		    (setq fetch-help-database-flags 1)
		    (fetch-help-database-entry "EDT-help" "keypad vt100")
		    (setq fetch-help-database-flags x)
		    (beginning-of-file)
		    (set-mark)
		    (search-forward "l")
		    (erase-region)
		    (insert-character '+')
		    (replace-string "q" "-")
		    (replace-string "-w-" "-+-")
		    (re-replace-string "\\|\\|\e\\[[0-9]*m" "")
		    (re-replace-string "^x\\|x$" "|")
		    (re-replace-string "\\(\\W\\)x\\(\\W\\)" "\\1|\\2")
		    (replace-string "-k" "-+")
		    (replace-string "m-" "+-")
		    (replace-string "-v-" "-+-")
		    (replace-string "l-" "+-")
		    (replace-string "t-" "+-")
		    (replace-string "-n-" "-+-")
		    (replace-string "-j" "-+")
		    (replace-string "-u" "-+")
		    (replace-string " x" " |")
		    (replace-string "x " "| ")
		    (re-replace-string "^\\(.........................................\\) " "\\1")
		    (end-of-file)(set-mark)
		    (search-reverse "+")
		    (forward-character)
		    (exchange-dot-and-mark)
		    (erase-region)
		    (beginning-of-file)
		    (search-forward " +")
		    (backward-character)
		    (delete-previous-character)
		    (beginning-of-file)
		    (setq ~EDT-help 1)
		    (define-keymap "EDT-help-map")
		    (use-global-map "EDT-help-map")
		    (setq i 0)
		    (while (< i 256)
			(bind-to-key "exit-emacs" (char-to-string i))
			(setq i (+ i 1))
		    )
		)
	    ))
	    (save-window-excursion
		(use-old-buffer "EDT-help")
		(delete-other-windows)
	        (use-global-map "EDT-help-map")
	        (recursive-edit)
	        (use-global-map "default-global-keymap")
	    )
	)
    )
    (use-global-map "Minibuf-local-map")
    (bind-to-key "~EDT-advance-mini" "\(kp4)")
    (bind-to-key "~EDT-backup-mini" "\(kp5)")
    (bind-to-key "~EDT-enter" "\(enter)")
    (bind-to-key "~EDT-enter" "\(do)")
    (bind-to-key "expand-and-exit" "\(pf2)")
    (use-global-map "Minibuf-local-NS-map")
    (bind-to-key "~EDT-advance-mini" "\(kp4)")
    (bind-to-key "~EDT-backup-mini" "\(kp5)")
    (bind-to-key "~EDT-enter" "\(enter)")
    (bind-to-key "~EDT-enter" "\(do)")
    (bind-to-key "expand-and-exit" "\(pf2)")
    (use-global-map "default-global-keymap")
    (bind-to-key "EDT-not-implemented" "\(gold)-")
    (bind-to-key "EDT-repeat" "\(gold)0")
    (bind-to-key "EDT-repeat" "\(gold)1")
    (bind-to-key "EDT-repeat" "\(gold)2")
    (bind-to-key "EDT-repeat" "\(gold)3")
    (bind-to-key "EDT-repeat" "\(gold)4")
    (bind-to-key "EDT-repeat" "\(gold)5")
    (bind-to-key "EDT-repeat" "\(gold)6")
    (bind-to-key "EDT-repeat" "\(gold)7")
    (bind-to-key "EDT-repeat" "\(gold)8")
    (bind-to-key "EDT-repeat" "\(gold)9")
    (bind-to-key "EDT-sect" "\(kp8)")
    (bind-to-key "EDT-sect" "\(next screen)")
    (bind-to-key "EDT-sect-back" "\(prev screen)")
    (bind-to-key "EDT-cut" "\(kp6)")
    (bind-to-key "EDT-cut" "\(remove)")
    (bind-to-key "EDT-paste" "\(gold)\(kp6)")
    (bind-to-key "EDT-paste" "\(insert here)")
    (bind-to-key "EDT-advance" "\(kp4)")
    (bind-to-key "EDT-bottom" "\(gold)\(kp4)")
    (bind-to-key "EDT-backup" "\(kp5)")
    (bind-to-key "EDT-word" "\(kp1)")
    (bind-to-key "EDT-eol" "\(kp2)")
    (bind-to-key "EDT-bol" "\^h")
    (bind-to-key "execute-extended-command" "\(gold)\(kp7)")
    (bind-to-key "EDT-open-line" "\(gold)\(kp0)")
    (bind-to-key "EDT-line" "\(kp0)")
    (bind-to-key "EDT-select" "\(dot)")
    (bind-to-key "EDT-select" "\(select)")
    (bind-to-key "EDT-reset" "\(gold)\(dot)")
    (bind-to-key "EDT-help" "\(pf2)")
    (bind-to-key "help" "\(help)")
    (bind-to-key "EDT-del-eol" "\(gold)\(kp2)")
    (bind-to-key "EDT-char" "\(kp3)")
    (bind-to-key "EDT-top" "\(gold)\(kp5)")
    (bind-to-key "EDT-del-l" "\(pf4)")
    (bind-to-key "EDT-und-l" "\(gold)\(pf4)")
    (bind-to-key "EDT-find" "\(gold)\(pf3)")
    (bind-to-key "EDT-fndnxt" "\(pf3)")
    (bind-to-key "EDT-find" "\(find)")
    (bind-to-key "EDT-up" "\(up)")
    (bind-to-key "EDT-down" "\(down)")
    (bind-to-key "EDT-right" "\(right)")
    (bind-to-key "EDT-left" "\(left)")
    (bind-to-key "EDT-noop" "\(enter)")
    (bind-to-key "execute-extended-command" "\(do)")
    (bind-to-key "help" "\(help)")
    (bind-to-key "EDT-del-c" "\(comma)")
    (bind-to-key "EDT-und-c" "\(gold)\(comma)")
    (bind-to-key "EDT-del-w" "\(minus)")
    (bind-to-key "EDT-und-w" "\(gold)\(minus)")
    (bind-to-key "EDT-specins" "\(gold)\(kp3)")
    (bind-to-key "EDT-page" "\(kp7)")
    (bind-to-key "EDT-replace" "\(gold)\(kp9)")
    (bind-to-key "EDT-subs" "\(gold)\(enter)")
    (bind-to-key "EDT-chngcase" "\(gold)\(kp1)")
    (bind-to-key "EDT-not-implemented" "\(kp9)")
    (bind-to-key "EDT-fill" "\(gold)\(kp8)")
)
[electricc.key]
; 
; electric-c key bindings
; 
(progn
    (local-bind-to-key "begin-C-comment" "\e`")
    (local-bind-to-key "c-paren" ")")
    (local-bind-to-key "c-paren" "]")
    (local-bind-to-key "c-paren" "}")
    (local-bind-to-key "end-C-comment" "\e'")
    (local-bind-to-key "indent-C-procedure" "\ej")
    (local-bind-to-key "fore-paren" "\e)")
    (local-bind-to-key "back-paren" "\e(")
(defun
    (high-voltage-on-key-binding
	(local-bind-to-key "high-voltage-on" "\^^")
    )
)
(defun
    (high-voltage-off-key-binding
	(local-bind-to-key "high-voltage-off" "\^^")
    )
)
)
[electricc.ml]
; 
; electric C mode
; 
(progn
(defun
    (back-paren			; Go to last open parenthesis at current level
	(backward-paren 0))
)
(defun
    (fore-paren			; Go to next close parenthesis at current level
	(forward-paren 0))
)
(defun
    (flash-back-paren 		; Flash the matching left parenthesis
	(save-excursion
	    (backward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (flash-fore-paren		; Flash the matching close parenthesis
	(save-excursion
	    (forward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (c-paren			; Flashes matching open parenthesis when ')' is typed.
	(insert-character (last-key-struck))
	(save-excursion
	    (backward-paren 0)
	    (if (dot-is-visible)
		(sit-for 5)
		(progn
		    (beginning-of-line)
		    (set-mark)
		    (end-of-line)
		    (message (region-to-string)))
	    )
	)
    )
)
(defun
    (c-indent old-dot old-size	; indent a {...} apropriately
        (setq old-dot (dot))
	(setq old-size (buffer-size))
	(save-excursion
	    (previous-line)
	    (search-forward "^}")
	    (set-mark)
	    (backward-paren 0)
	    (beginning-of-line)
	    (exchange-dot-and-mark)
	    (end-of-line)
	    (forward-character)
	    (filter-region "indent -st")
	)
	(goto-character (/ (* (buffer-size) old-dot) old-size))
	(novalue)
    )
)
(defun
    (electric-}			; flash the matching '{'
	(if (eolp)
	    (progn
		(delete-white-space)
		(if (! (bolp))
		    (newline-and-indent)
		    (progn
			(delete-previous-character)
			(newline-and-indent)
		    ))
		(c-paren)
;		(dedent-line)
		(newline-and-indent)
		(dedent-line)
	    )
	    (c-paren)
	)
    )
)
(defun
    (electric-{			; Flash the matching '}'...
				; really should do this only if the line
				; isn't all whitespace
	(if (eolp)
	    (progn
		(insert-string "{")
		(newline-and-indent)
	    )
	    (insert-string "{")
	)
    )
)
(defun
    (dedent-line		; Move line to left by one tabstop
        (beginning-of-line)
	(to-col (- (current-indent) logical-tab-size))
	(insert-string ".")
	(delete-white-space)
	(delete-previous-character)
	(end-of-line)
    )
)
(defun
    (indent-line		; Move line to right by one tabstop
        (beginning-of-line)
	(to-col (+ (current-indent) logical-tab-size))
	(insert-string ".")
	(delete-white-space)
	(delete-previous-character)
	(end-of-line)
    )
)
(defun
    (electric-semi		; Insert a comment starter
	(insert-string ";")
	(if (eolp) (newline-and-indent))
    )
)
(defun
    (skip-spaces		; Skip 'following' whitespace
	(forward-character)
	(while (| (| (= (following-char) ' ')
		      (= (following-char) '	'))
		   (= (following-char) 10))
	    (forward-character)
	)
    )
)
(defun
    (electric-*			; If starting a comment, then do it.
	(if (& (eolp) (= (preceding-char) '/'))
	    (progn
		(delete-previous-character)
		(if (! (bolp))
		    (progn
			(delete-white-space)
			(if (bolp) (delete-previous-character))
		    )
		)
		(begin-C-comment)
	    )
	    (insert-string "*")
	)
    )
)
; If only thing on line and starting a comment, then don't indent 
(defun
    (electric-/			; If in a comment, close it off
	(if (& (eolp) (= (preceding-char) '*'))
	    (progn
		(delete-previous-character)
		(end-C-comment)
	    )
	    (insert-string "/")
	)
    )
)
; need to setup tab stops too! (tab at beginning of blank line should just
; tab. at beginning of non-blank line, should re-indent-line.)
(defun
    (electric-tab
	(if (| (& (eolp) (bolp)) (!= (current-column) 1))
	    (insert-character '\t')
	    (progn
		(delete-white-space)
		(to-col (c-indent-level))
	    )
	)
    )

 

    (c-indent-level
	(save-excursion
	    (if (error-occurred (re-search-reverse "^[ 	]*[!-~]"))
		(progn
		    logical-tab-size
		)
		(progn
		    (re-search-forward "[!-~]")
		    (- (current-column) 1)   
		)
	    )
	)
    )
)
(defun
    (begin-C-comment
				; should setup for a block comment if on a
				; blank line.
	(move-to-comment-column)
	(setq left-margin (current-column))
	(setq right-margin 78)
	(setq prefix-string "   ")
	(insert-string "/* ")
    )
)
(defun
    (end-C-comment		; Should close off a block comment.
	(setq right-margin 1000)
	(if (!= (preceding-char) ' ') (insert-string " "))
	(insert-string "*/")
    )
)

(defun
    (high-voltage-on
	(setq mode-string "electric-C high voltage")
	(local-bind-to-key "electric-semi" ";")
	(local-bind-to-key "electric-{" "{")
	(local-bind-to-key "electric-}" "}")
	(local-bind-to-key "electric-*" "*")
	(local-bind-to-key "electric-/" "/")
	(local-bind-to-key "electric-tab" "\t")
	(local-bind-to-key "flash-fore-paren" "\e}")
	(local-bind-to-key "flash-back-paren" "\e{")
	(high-voltage-off-key-binding)
	(message "high voltage on")
    )
)
(defun
    (high-voltage-off
	(setq mode-string "electric-C")
	(local-bind-to-key "self-insert" ";")
	(local-bind-to-key "self-insert" "{")
	(local-bind-to-key "self-insert" "\t")
	(local-bind-to-key "c-paren" "}")
	(local-bind-to-key "self-insert" "*")
	(local-bind-to-key "self-insert" "/")
	(high-voltage-on-key-binding)
	(message "high voltage off")
    )
)
(defun
    (electric-c-mode
	(setq mode-string "electric-C")
	(use-syntax-table "C")
	(use-local-map "electric-c-map")
	(use-abbrev-table "electric-c")
	(novalue)
    )
)
(use-syntax-table "C")
(modify-syntax-table "paren" "(" ")")
(modify-syntax-table "paren" "{" "}")
(modify-syntax-table "paren" "[" "]")
(modify-syntax-table "comment" "/*" "*/")
(modify-syntax-table "string" "\"")
(modify-syntax-table "string" "'")
(modify-syntax-table "prefix" "\\")
(modify-syntax-table "word" "_$")
(modify-syntax-table "comment" "//" "\n")
(modify-syntax-table "keyword-1" "auto")
(modify-syntax-table "keyword-1" "break")
(modify-syntax-table "keyword-1" "case")
(modify-syntax-table "keyword-1" "char")
(modify-syntax-table "keyword-1" "const")
(modify-syntax-table "keyword-1" "continue")
(modify-syntax-table "keyword-1" "default")
(modify-syntax-table "keyword-1" "do")
(modify-syntax-table "keyword-1" "double")
(modify-syntax-table "keyword-1" "else")
(modify-syntax-table "keyword-1" "enum")
(modify-syntax-table "keyword-1" "extern")
(modify-syntax-table "keyword-1" "float")
(modify-syntax-table "keyword-1" "for")
(modify-syntax-table "keyword-1" "goto")
(modify-syntax-table "keyword-1" "if")
(modify-syntax-table "keyword-1" "int")
(modify-syntax-table "keyword-1" "long")
(modify-syntax-table "keyword-1" "register")
(modify-syntax-table "keyword-1" "return")
(modify-syntax-table "keyword-1" "short")
(modify-syntax-table "keyword-1" "signed")
(modify-syntax-table "keyword-1" "sizeof")
(modify-syntax-table "keyword-1" "static")
(modify-syntax-table "keyword-1" "struct")
(modify-syntax-table "keyword-1" "switch")
(modify-syntax-table "keyword-1" "typedef")
(modify-syntax-table "keyword-1" "union")
(modify-syntax-table "keyword-1" "unsigned")
(modify-syntax-table "keyword-1" "void")
(modify-syntax-table "keyword-1" "volatile")
(modify-syntax-table "keyword-1" "while")
(modify-syntax-table "keyword-2" "class")
(modify-syntax-table "keyword-2" "delete")
(modify-syntax-table "keyword-2" "friend")
(modify-syntax-table "keyword-2" "inline")
(modify-syntax-table "keyword-2" "new")
(modify-syntax-table "keyword-2" "operator")
(modify-syntax-table "keyword-2" "private")
(modify-syntax-table "keyword-2" "protected")
(modify-syntax-table "keyword-2" "public")
(modify-syntax-table "keyword-2" "this")
(modify-syntax-table "keyword-2" "virtual")
(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "electric-c-map")
    (define-keymap "electric-c-ESC-map")
    (use-local-map "electric-c-map")
    (local-bind-to-key "electric-c-ESC-map" "\e")
    (execute-mlisp-file "electricc.key")
    (high-voltage-on-key-binding)
    (kill-buffer "keymap-hack")
)
(novalue)
)
[emacs_control_strings.ml]
; 
;   emacs_control_strings.ml
; 

; 
;   convert ESC-[ etc into eight bit form.
;   CSI, SS3, DCS, ???
; 
(setq control-string-convert-to-8-bit "[OP")
; 
;   convert SS3, DCS and ??? into CSI
; 
(setq control-string-convert-to-CSI "\217\220\232")
; 
; A control string consists of
; CSI <par>* <int>* <fin>
; where <par> is the parameter-characters and parameter-separators
;	<int> is the intermediate-characters
;	<fin> is the final-characters
; 
; The settings below conform to ISO control string standards as used
; in DEC VTxxx terminals.
; 
(setq control-string-final-characters
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
(setq control-string-intermediate-characters " !\"#$&'()*+,-./")
(setq control-string-parameter-characters "0123456789:<=>?")
(setq control-string-parameter-separators ";")
; 
; Convert CSI <n> ~ into two characters <\200> <n+' '>
; 
(setq control-string-convert-F-keys 1)
; 
; Convert CSI ... &w from the mouse
; 
(setq control-string-convert-mouse 1)
; 
;   Set the following true to enable the above processing.
; 
;   The logical name emacs$control_string_processing is looked
;   to provide a way to turn off control-string-processing.
; 
(progn value
    (if
	(error-occurred
	    (setq value (+ (getenv "EMACS$CONTROL_STRING_PROCESSING")))
	)
	(setq control-string-processing 1)
	(setq control-string-processing value)
    )
)
[emacs_default_autoloads.ml]
;
; EMACS_DEFAULT_AUTOLOADS.ML for EMACS V4.1
;
; This file contains the default set of autoloading functions. These
; functions are set up before the default bindings are set up.
;
(autoload "adjust-line-number" "goto")
(autoload "backward-paragraph" "paragraph")
(autoload "backward-sentence" "sentence")
(autoload "c-mode" "cmode")
(autoload "compile-it" "newcompil")
(autoload "debug" "debug")
(autoload "describe-command" "describex")
(autoload "describe-variable" "describex")
(autoload "describe-word-in-buffer" "desword")
(autoload "electric-c-mode" "electricc")
(autoload "error-message-parser" "errmsgprs") (setq error-message-parser "error-message-parser")
(autoload "expand-mlisp-variable" "expandx")
(autoload "expand-mlisp-word" "expandx")
(autoload "forward-paragraph" "paragraph")
(autoload "forward-sentence" "sentence")
(autoload "fundamental-mode" "fundmode")
(autoload "grep" "grep")
(autoload "goto-line" "goto")
(autoload "goto-percent" "goto")
(autoload "vms-help" "help")
(autoload "help" "helpchar")
(autoload "help-key" "helpchar")
(autoload "info" "info")
(autoload "incremental-search" "incsearch")
(autoload "justify-paragraph" "justify")
(autoload "lisp-mode" "lispmode")
(autoload "new-undo" "undo")
(autoload "normal-mode" "normalmod")
(autoload "notes" "notes")
(autoload "print-file" "queues")
(autoload "reverse-incremental-search" "incsearch")
(autoload "shell" "process")
(autoload "show-buffer" "show-buffer")
(autoload "show-queue" "queues")
(autoload "spawn" "spawn")
(autoload "text-mode" "textmode")
(autoload "trace" "trace") (setq trace-hook "trace")
(autoload "view-buffer" "view")
(autoload "view-file" "view")
(autoload "what-cursor-position" "whatcurs")
(autoload "write-region-to-file" "writereg")
(autoload "correct-word" "emacs_spell")
(autoload "get-tty-correction" "emacs_spell")
(autoload "spell-check-word" "emacs_spell")
(autoload "spell-word-in-buffer" "spell")
(execute-mlisp-file "emacs_sys")
[emacs_default_keybindings.ml]
; 
; EMACS_DEFAULT_KEYBINDINGS.ML for EMACS V4.0
;
; This file defines all the default key settings that EMACS uses.
; 
; 
; Initialize default-global-keymap. First set it to self-insert
; 

(use-global-map "default-global-keymap")
(error-occurred (execute-mlisp-file "escape_sequence_strings"))
(progn char
    (setq char 0)
    (while (< char 256)
	(bind-to-key "self-insert" (char-to-string char))
	(setq char (+ 1 char))
    )
)
(define-keymap "ESC-prefix")
(define-keymap "^X-prefix")
(bind-to-key "ESC-prefix" "\e")
(bind-to-key "^X-prefix" "\^x")
(bind-to-key "apropos" "\e?")
(bind-to-key "apropos-variable" "\^x?")
(bind-to-key "argument-prefix" "\^u")
(bind-to-key "backward-character" "\^b")
(bind-to-key "backward-paragraph" "\e(")
(bind-to-key "backward-sentence" "\ea")
(bind-to-key "backward-word" "\eb")
(bind-to-key "beginning-of-file" "\e<")
(bind-to-key "beginning-of-line" "\^a")
(bind-to-key "beginning-of-window" "\e,")
(bind-to-key "case-region-invert" "\e\^^")
(bind-to-key "case-word-invert" "\e^")
(bind-to-key "case-word-lower" "\el")
(bind-to-key "case-word-upper" "\eu")
(bind-to-key "column-to-left-of-window" "\e@")
(bind-to-key "compile-it" "\^x\^e")
(bind-to-key "debug" "\e\^d")
(bind-to-key "delete-next-character" "\^d")
(bind-to-key "delete-next-word" "\ed")
(bind-to-key "delete-other-windows" "\^x1")
(bind-to-key "delete-previous-character" "\^h")
(bind-to-key "delete-previous-character" "\177")
(bind-to-key "delete-previous-word" "\eh")
(bind-to-key "delete-region-to-buffer" "\e\^w")
(bind-to-key "delete-to-killbuffer" "\^w")
(bind-to-key "delete-window" "\^xd")
(bind-to-key "describe-word-in-buffer" "\^x\^d")
(progn char
    (setq char '0')
    (while (<= char '9')
	(bind-to-key "digit" char)
	(bind-to-key "meta-digit" (concat "\e" (char-to-string char)))
	(setq char (+ 1 char))
    )
)
(bind-to-key "end-of-file" "\e>")
(bind-to-key "end-of-line" "\^e")
(bind-to-key "end-of-window" "\e.")
(bind-to-key "enlarge-window" "\^xz")
(bind-to-key "exchange-dot-and-mark" "\^x\^x")
(bind-to-key "execute-extended-command" "\ex")
(bind-to-key "execute-keyboard-macro" "\^xe")
(bind-to-key "execute-mlisp-line" "\e\e")
(bind-to-key "execute-monitor-command" "\^x!")
(bind-to-key "exit-emacs" "\^c")
(bind-to-key "exit-emacs" "\^x\^c")
(bind-to-key "exit-emacs" "\e\^c")
(bind-to-key "forward-character" "\^f")
(bind-to-key "forward-paragraph" "\e)")
(bind-to-key "forward-sentence" "\ee")
(bind-to-key "forward-word" "\ef")
(bind-to-key "illegal-operation" "\^g")
(bind-to-key "indent-C-procedure" "\ej")
(bind-to-key "insert-file" "\^x\^i")
(bind-to-key "kill-to-end-of-line" "\^k")
(bind-to-key "line-to-top-of-window" "\e!")
(bind-to-key "list-buffers" "\^x\^b")
(progn char
    (setq char '0')
    (while (<= '9' char)
	(bind-to-key "meta-digit" (concat "\e" (char-to-string char)))
	(setq char (+ 1 char))
    )
)	
(bind-to-key "meta-minus" "\e-")
(bind-to-key "minus" "-")
(bind-to-key "newline" "\^m")
(bind-to-key "newline-and-backup" "\^o")
(bind-to-key "newline-and-indent" "\^j")
(bind-to-key "next-error" "\^x\^n")
(bind-to-key "next-line" "\^n")
(bind-to-key "next-page" "\^v")
(bind-to-key "next-window" "\^xn")
(bind-to-key "page-next-window" "\e\^v")
(bind-to-key "previous-line" "\^p")
(bind-to-key "previous-page" "\ev")
(bind-to-key "previous-window" "\^xp")
(bind-to-key "query-replace-string" "\eq")
(bind-to-key "quote-character" "\^q")
(bind-to-key "quote-character" "\^^q")
(bind-to-key "read-file" "\^x\^r")
(bind-to-key "redraw-display" "\^l")
(bind-to-key "replace-string" "\er")
(bind-to-key "return-to-monitor" "\^_")
(bind-to-key "scroll-one-line-down" "\ez")
(bind-to-key "scroll-one-line-up" "\^z")
(bind-to-key "search-forward" "\^s")
(bind-to-key "search-forward" "\^^s")
(bind-to-key "search-reverse" "\^r")
(bind-to-key "set-mark" "\0")
(bind-to-key "shrink-window" "\^x\^z")
(bind-to-key "split-current-window" "\^x2")
(bind-to-key "split-current-window-vertically" "\^x3")
(bind-to-key "start-remembering" "\^x(")
(bind-to-key "stop-remembering" "\^x)")
(bind-to-key "switch-to-buffer" "\^xb")
(bind-to-key "transpose-characters" "\^t")
(bind-to-key "undo" "\^x\^u")
(bind-to-key "new-undo" "\^x\^u")
(bind-to-key "unset-mark" "\^x\0")
(bind-to-key "use-old-buffer" "\^x\^o")
(bind-to-key "visit-file" "\^x\^v")
(bind-to-key "write-current-file" "\^x\^s")
(bind-to-key "write-current-file" "\^X\^^s")
(bind-to-key "write-file-exit" "\^x\^f")
(bind-to-key "write-modified-files" "\^x\^m")
(bind-to-key "write-named-file" "\^x\^w")
(bind-to-key "what-cursor-position" "\^x=")
(bind-to-key "yank-buffer" "\e\^y")
(bind-to-key "yank-from-killbuffer" "\^y")
(bind-to-key "help-key" "\^_")
(bind-to-key "interrupt-key" "\^G")

(execute-mlisp-file "lk201")


;
; Set up the Mini-buffer keymaps
;
(save-excursion
    (temp-use-buffer "setup-default-keybindings")
    (use-local-map "Minibuf-local-map")
    (local-bind-to-key "error-and-exit" "\^g")
    (local-bind-to-key "expand-and-exit" "\e")
    (local-bind-to-key "exit-emacs" "\^m")
    (local-bind-to-key "exit-emacs" "\^j")
    (local-bind-to-key "ESC-prefix" "\^\")
)
(save-excursion
    (temp-use-buffer "setup-default-keybindings")
    (use-local-map "Minibuf-local-NS-map")
    (local-bind-to-key "error-and-exit" "\^g")
    (local-bind-to-key "expand-and-exit" " ")
    (local-bind-to-key "expand-and-exit" "\t")
    (local-bind-to-key "expand-and-exit" "\e")
    (local-bind-to-key "exit-emacs" "\^m")
    (local-bind-to-key "exit-emacs" "\^j")
    (local-bind-to-key "ESC-prefix" "\^\")
    (local-bind-to-key "help-and-exit" "?")
)
(error-occurred (delete-buffer "setup-default-keybindings"))

[emacs_default_user_interface.ml]
(if (& (= terminal-is-terminal 3) (= operating-system-name "unix"))
    (progn
	(execute-mlisp-file "emacs_motif.key")
	(if (file-exists "sys$login:emacs_motif_settings.ml")
	    (execute-mlisp-file "sys$login:emacs_motif_settings.ml")
	    (error-occurred (execute-mlisp-file "emacs_motif_settings"))
	)
    )
)
(if (= operating-system-name "Windows")
    (progn
	(execute-mlisp-file "emacs_windows.ml")
	(if (file-exists "sys$login:emacs_windows_settings.ml")
	    (execute-mlisp-file "sys$login:emacs_windows_settings.ml")
	    (error-occurred (execute-mlisp-file "emacs_windows_settings"))
	)
    )
)

[emacs_motif.key]
; Autoloads
(autoload "motif-visit-file" "emacs_motif")
(autoload "motif-vf" "emacs_motif")
(autoload "motif-insert-file" "emacs_motif")
(autoload "motif-if" "emacs_motif")
(autoload "motif-save-file" "emacs_motif")
(autoload "motif-sf" "emacs_motif")
(autoload "motif-quit" "emacs_motif")
(autoload "motif-delete" "emacs_motif")
(autoload "motif-copy" "emacs_motif")
(autoload "motif-yank" "emacs_motif")
(autoload "motif-find-next" "emacs_motif")
(autoload "motif-find-prev" "emacs_motif")
(autoload "motif-preferences" "emacs_motif")
(autoload "motif-buffer" "emacs_motif")
(autoload "motif-restore-settings" "emacs_motif")
(autoload "motif-save-settings" "emacs_motif")
(autoload "motif-version" "emacs_motif")

; File menu
(bind-to-key "motif-visit-file" "\(menu)Fn")
(bind-to-key "motif-visit-file" "\(menu)Fo")
(bind-to-key "motif-vf" "\(menu)Fv")
(bind-to-key "motif-insert-file" "\(menu)Fi")
(bind-to-key "motif-if" "\(menu)Fk")
(bind-to-key "write-current-file" "\(menu)Fs")
(bind-to-key "motif-save-file" "\(menu)Fa")
(bind-to-key "motif-sf" "\(menu)Fc")
(bind-to-key "write-modified-files" "\(menu)Fw")
(bind-to-key "motif-quit" "\(menu)Fq")
(bind-to-key "write-file-exit" "\(menu)Fe")

; Edit menu
(bind-to-key "undo" "\(menu)Eu")
(bind-to-key "new-undo" "\(menu)Em")
(bind-to-key "motif-delete" "\(menu)Et")
(bind-to-key "motif-copy" "\(menu)Ec")
(bind-to-key "motif-yank" "\(menu)Ep")
(bind-to-key "erase-region" "\(menu)Ee")
(bind-to-key "set-mark" "\(menu)Es")

; Navigate menu
(bind-to-key "motif-find-next" "\(menu)Nfn")
(bind-to-key "motif-find-prev" "\(menu)Nfp")
(bind-to-key "re-search-forward" "\(menu)Ns")
(bind-to-key "re-query-replace-string" "\(menu)Nr")
(bind-to-key "next-page" "\(menu)Nap")
(bind-to-key "end-of-file" "\(menu)Nae")
(bind-to-key "next-line" "\(menu)Nal")
(bind-to-key "forward-sentence" "\(menu)Nas")
(bind-to-key "forward-paragraph" "\(menu)Nag")
(bind-to-key "previous-page" "\(menu)Npp")
(bind-to-key "beginning-of-file" "\(menu)Npb")
(bind-to-key "previous-line" "\(menu)Npl")
(bind-to-key "backward-sentence" "\(menu)Nps")
(bind-to-key "backward-paragraph" "\(menu)Npg")

; View menu
(bind-to-key "split-current-window" "\(menu)V2")
(bind-to-key "delete-window" "\(menu)Vd")
(bind-to-key "delete-other-windows" "\(menu)Vo")
(bind-to-key "pop-to-buffer" "\(menu)Vp")
(bind-to-key "switch-to-buffer" "\(menu)Vs")
(bind-to-key "redraw-display" "\(menu)Vr")

; Options menu
(UI-add-menu "\(menu)Cp" 50 "&Options" 10 "&Preferences...")
(UI-add-menu "\(menu)Cb" 50 "&Options" 20 "&Buffer...")
(UI-add-menu "-" 50 "&Options" 30 "Sep")
(UI-add-menu "\(menu)Cu" 50 "&Options" 40 "&Use Last Saved Settings")
(UI-add-menu "\(menu)Cs" 50 "&Options" 50 "&Save Current Settings")
(bind-to-key "motif-preferences" "\(menu)Cp")
(bind-to-key "motif-buffer" "\(menu)Cb")
(bind-to-key "motif-restore-settings" "\(menu)Cu")
(bind-to-key "motif-save-settings" "\(menu)Cs")

; Help menu
(bind-to-key "describe-command" "\(menu)Hc")
(bind-to-key "describe-variable" "\(menu)Ho")
(bind-to-key "describe-word-in-buffer" "\(menu)Hw")
(bind-to-key "apropos" "\(menu)Ha")
(bind-to-key "help" "\(menu)Hh")
(bind-to-key "motif-version" "\(menu)Hv")

[emacs_motif.ml]
(declare-buffer-specific motif-visit-pattern motif-visit-fn)
(declare-buffer-specific motif-insert-pattern motif-insert-fn)
(declare-buffer-specific motif-save-pattern motif-save-fn)
(setq-default motif-visit-pattern "")
(setq-default motif-insert-pattern "")
(setq-default motif-save-pattern "")
(defun
    (motif-vf
	(visit-file motif-visit-fn)
    )
)
(defun 
    (motif-visit-file
	(if (! (XmIsDialog "Visit File"))
	    (XmFileSelectionDialog "Visit File" "\(menu)Fv" motif-visit-pattern motif-visit-fn)
	)
	(if (XmOperateDialog "Visit File")
	    (motif-vf)
	)
    )
)
(defun
    (motif-if
	(insert-file motif-insert-fn)
    )
)
(defun 
    (motif-insert-file
	(if (! (XmIsDialog "Insert File"))
	    (XmFileSelectionDialog "Insert File" "\(menu)Fk" motif-insert-pattern motif-insert-fn)
	)
	(if (XmOperateDialog "Insert File")
	    (motif-if)
	)
    )
)
(defun
    (motif-sf
	(write-named-file motif-save-fn)
    )
)
(defun 
    (motif-save-file
	(if (! (XmIsDialog "Save File"))
	    (XmFileSelectionDialog "Save File" "\(menu)Fc" motif-save-pattern motif-save-fn)
	)
	(if (XmOperateDialog "Save File")
	    (motif-sf)
	)
    )
)
(defun 
    (motif-find-next
	(re-search-forward "")
    )
)
(defun 
    (motif-find-prev
	(re-search-reverse "")
    )
)
(defun 
    (motif-copy
       (copy-region-to-buffer "Kill buffer")
       (UI-copy-buffer)
    )
)
(defun 
    (motif-delete
       (UI-copy-buffer)
       (delete-to-killbuffer)
    )
)
(defun 
    (motif-yank
       (save-excursion
	    (temp-use-buffer "Kill buffer")
	    (erase-buffer)
	    (UI-paste-buffer)
       )
       (yank-from-killbuffer)
    )
)
(defun
    (motif-quit
	c l
	(setq l (fetch-array buffer-names 0))
	(setq c 1)
	(while (<= c l)
	    (temp-use-buffer (fetch-array buffer-names c))
	    (setq buffer-is-modified 0)
	    (setq c (+ c 1))
	)
	(exit-emacs)
    )
)
(defun
    (motif-version
	motif-version
	(if (! (XmIsDialog "Verson"))
	    (XmMessageDialog "Version" -1 motif-version)
	)
	(setq motif-version (emacs-version))
	(XmOperateDialog "Version")
    )
)
(declare-global
    ~motif-is-macro
    ~motif-is-scratch
    ~motif-is-file
    ~motif-slm
    ~motif-srm
    ~motif-lm
    ~motif-rm
)
(defun 
    (motif-preferences
	(if (! (XmIsDialog "Preferences"))
	    (XmFormDialog "Preferences" "\(menu)Cc" 960 675
	      (XmRowColumn 10 10 -1 -1 0 3 -1 2
		(XmFrame -1 -1 7
		    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Display Options")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmToggleButton -1 -1 "Activity Indicator" activity-indicator)
			(XmToggleButton -1 -1 "Display EOF" default-display-end-of-file)
			(XmToggleButton -1 -1 "Display Non-printing Characters" default-display-non-printing-characters)
			(XmToggleButton -1 -1 "Disply Controls with ^" ctlchar-with-^)
			(XmToggleButton -1 -1 "Highlight Region" default-highlight-region)
			(XmToggleButton -1 -1 "Force Redisplay" force-redisplay)
			(XmToggleButton -1 -1 "Quick Redisplay" quick-redisplay)
			(XmToggleButton -1 -1 "Visible Bell" visible-bell)
			(XmToggleButton -1 -1 "Wrap Long Lines" wrap-long-lines)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Mode Line Format")
			    (XmTextField -1 -1 25 1 default-mode-line-format)
			    (XmSeparator -1 -1 8 0 0)
			    (XmLabel 0 -1 "Mode Line Rendition")
			    (XmTextField -1 -1 16 1 mode-line-graphic-rendition)
			    (XmSeparator -1 -1 8 0 0)
			    (XmLabel 0 -1 "Highligh Region Rendition")
			    (XmTextField -1 -1 10 1 highlight-region-graphic-rendition)
			)
		    )
		)
		(XmFrame 347 10 7
		    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Editing Options")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmToggleButton -1 -1 "Replace Mode" default-replace-mode)
			(XmToggleButton -1 -1 "Ask About Buffer Names" ask-about-buffer-names)
			(XmToggleButton -1 -1 "Pop Up Windows" pop-up-windows)
			(XmToggleButton -1 -1 "Stack Trace on Error" stack-trace-on-error)
			(XmToggleButton -1 -1 "Track EOL on Up/Down Arrow" track-eol-on-^N-^P)
			(XmToggleButton -1 -1 "Remove Checkpoint Files" unlink-checkpoint-files)
			(XmToggleButton -1 -1 "Silently Kill Processes" silently-kill-processes)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn 17 515 -1 -1 0 -1 -1 -1
			    (XmLabel -1 -1 "Global Mode")
			    (XmTextField -1 -1 25 1 global-mode-string)
			)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
				(XmLabel 0 -1 "Search/Replace Options")
				(XmSeparator -1 -1 -1 1 -1)
			    )
			    (XmToggleButton -1 -1 "Case Fold Search" default-case-fold-search)
			    (XmToggleButton -1 -1 "Replace Case" replace-case)
			)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
				(XmLabel 0 -1 "Help Options")
				(XmSeparator -1 -1 -1 1 -1)
			    )
			    (XmToggleButton -1 -1 "Help on Command Error" help-on-command-completion-error)
			    (XmToggleButton -1 -1 "Remove Help Windows" remove-help-window)
			)
		    )
		)
		(XmFrame -1 -1 7
		    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Editing Options")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmScale -1 -1 "Tab Size" 290 -1 0 32 1 1 default-tab-size)
			(XmScale -1 -1 "Scroll Step" 290 -1 0 30 1 1 scroll-step)
			(XmScale -1 -1 "Split Height Threshold" 290 -1 0 30 1 1 split-height-threshhold)
			(XmScale -1 -1 "Comment Column" 290 -1 0 150 1 1 default-comment-column)
			(XmScale -1 -1 "Checkpoint Frequency" 290 -1 0 2000 1 1 checkpoint-frequency)
			(XmScale -1 -1 "Journal Frequency" 290 -1 0 300 1 1 journal-frequency)
		    )
		)
	      )
	    )
	)
	(XmOperateDialog "Preferences")
	(novalue)
    )
)
(defun 
    (motif-buf-post
	(if ~motif-slm
	    (setq left-margin ~motif-lm)
	)
	(if ~motif-srm
	    (setq right-margin ~motif-rm)
	)
	(if ~motif-is-file
	    (setq current-buffer-type "file")
	    ~motif-is-macro
	    (setq current-buffer-type "macro")
	    ~motif-is-scratch
	    (setq current-buffer-type "scratch")
	)
    )
)
(defun 
    (motif-buffer
	(if (! (XmIsDialog "Buffer Settings"))
	    (XmFormDialog "Buffer Settings" "\(menu)Cc" 630 750
	      (XmRowColumn 10 10 -1 -1 0 3 -1 2
		(XmFrame 347 10 7
		    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Buffer Type")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmFrame -1 -1 7
			    (XmRadioBox -1 -1 0
				(XmToggleButton -1 -1 "Macro" ~motif-is-macro)
				(XmToggleButton -1 -1 "File" ~motif-is-file)
				(XmToggleButton -1 -1 "Scratch" ~motif-is-scratch)
			    )
			)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Editing Settings")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmToggleButton -1 -1 "Replace Mode" replace-mode)
			(XmToggleButton -1 -1 "Buffer Modified" buffer-is-modified)
			(XmToggleButton -1 -1 "Abbrev Expansion" abbrev-mode)
			(XmToggleButton -1 -1 "Read Only" read-only-buffer)
			(XmToggleButton -1 -1 "Case Fold Search" case-fold-search)
			(XmSeparator -1 -1 5 0 0)
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Data Protection Settings")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmToggleButton -1 -1 "Checkpointable" current-buffer-checkpointable)
			(XmToggleButton -1 -1 "Journaling" current-buffer-journalled)
			(XmSeparator -1 -1 5 0 0)
		    )
		)
		(XmFrame -1 -1 7
		    (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			(XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
			    (XmLabel 0 -1 "Display Settings")
			    (XmSeparator -1 -1 -1 1 -1)
			)
			(XmToggleButton -1 -1 "Display EOF" display-end-of-file)
			(XmToggleButton -1 -1 "Display Non-printing Characters" display-non-printing-characters)
			(XmToggleButton -1 -1 "Highlight Region" highlight-region)
			(XmToggleButton -1 -1 "Wrap Long Lines" wrap-long-lines)
			(XmScale -1 -1 "Comment Column" 290 -1 0 150 1 1 comment-column)
			(XmScale -1 -1 "Tab Size" 290 -1 0 32 1 1 tab-size)
		    )
		)
	      )
	      (XmFrame 10 495 7
		(XmRowColumn -1 -1 600 150 0 -1 -1 -1
		    (XmScale -1 -1 "" 450 -1 0 100 1 1 ~motif-lm)
		    (XmToggleButton -1 -1 "Left Margin" ~motif-slm)
		    (XmScale -1 -1 "" 450 -1 30 250 1 1 ~motif-rm)
		    (XmToggleButton -1 -1 "Right Margin" ~motif-srm)
		)
	      )
	    )
	)
	(setq ~motif-is-macro 0)
	(setq ~motif-is-scratch 0)
	(setq ~motif-is-file 0)
	(setq ~motif-slm 0)
	(setq ~motif-srm 0)
	(setq ~motif-lm left-margin)
	(setq ~motif-rm right-margin)
	(if (= current-buffer-type "file")
	    (setq ~motif-is-file 1)
	    (= current-buffer-type "macro")
	    (setq ~motif-is-macro 1)
	    (setq ~motif-is-scratch 1)
	)
	(if (XmOperateDialog "Buffer Settings")
	    (motif-buf-post) 
	)
	(novalue)
    )
)
(defun 
    (motif-save-settings
      (save-excursion
	(temp-use-buffer "*GOK*")
	(erase-buffer)
	(insert-string "(setq activity-indicator " activity-indicator ")\n")
	(insert-string "(setq default-display-end-of-file " default-display-end-of-file ")\n")
	(insert-string "(setq default-display-non-printing-characters "  default-display-non-printing-characters ")\n")
	(insert-string "(setq ctlchar-with-^ " ctlchar-with-^ ")\n")
	(insert-string "(setq default-highlight-region " default-highlight-region ")\n")
	(insert-string "(setq force-redisplay " force-redisplay ")\n")
	(insert-string "(setq quick-redisplay " quick-redisplay ")\n")
	(insert-string "(setq visible-bell " visible-bell ")\n")
	(insert-string "(setq wrap-long-lines " wrap-long-lines ")\n")
	(insert-string "(setq default-replace-mode " default-replace-mode ")\n")
	(insert-string "(setq ask-about-buffer-names " ask-about-buffer-names ")\n")
	(insert-string "(setq stack-trace-on-error " stack-trace-on-error ")\n")
	(insert-string "(setq track-eol-on-^N-^P " track-eol-on-^N-^P ")\n")
	(insert-string "(setq unlink-checkpoint-files " unlink-checkpoint-files ")\n")
	(insert-string "(setq silently-kill-processes " silently-kill-processes ")\n")
	(insert-string "(setq default-case-fold-search " default-case-fold-search ")\n")
	(insert-string "(setq replace-case " replace-case ")\n")
	(insert-string "(setq help-on-command-completion-error " help-on-command-completion-error ")\n")
	(insert-string "(setq remove-help-window " remove-help-window ")\n")
	(insert-string "(setq default-tab-size " default-tab-size ")\n")
	(insert-string "(setq split-height-threshhold " split-height-threshhold ")\n")
	(insert-string "(setq default-comment-column " default-comment-column ")\n")
	(insert-string "(setq checkpoint-frequency " checkpoint-frequency ")\n")
	(insert-string "(setq journal-frequency " journal-frequency ")\n")
	(insert-string "(setq default-mode-line-format \"" default-mode-line-format "\")\n")
	(insert-string "(setq mode-line-graphic-rendition \"" mode-line-graphic-rendition "\")\n")
	(insert-string "(setq highlight-region-graphic-rendition \"" highlight-region-graphic-rendition "\")\n")
	(if (error-occurred (put-database-entry "MLisp-library" "emacs_motif_settings.ml"))
	    (write-named-file "sys$login:emacs_motif_settings.ml")
	)
      )
    (delete-buffer "*GOK*")
    )
)
(defun
    (motif-restore-settings
	(if (error-occurred (execute-mlisp-file "emacs_motif_settings"))
	    (error-occurred (execute-mlisp-file "sys$login:emacs_motif_settings.ml"))
	)
    )
)
[emacs_windows.key]
; 
; emacs_windows.key - windows default key bindings for menu and tool bar
; 
(bind-to-key "WIN-open-file" "\(menu)fo")
(bind-to-key "WIN-save-file" "\(menu)fs")
(bind-to-key "WIN-open-recent" "\(menu)fr")
(bind-to-key "WIN-save-file-as" "\(menu)fa")
(bind-to-key "WIN-save-environment" "\(menu)fe")
(bind-to-key "WIN-edit-cut" "\(menu)ex")
(bind-to-key "WIN-edit-copy" "\(menu)ec")
(bind-to-key "WIN-edit-paste" "\(menu)ev")
(bind-to-key "WIN-find-string" "\(menu)es")
(bind-to-key "WIN-find-and-replace" "\(menu)ef")
(progn
    ~seperator-required
    ~cmd ~file
    (setq ~seperator-required 0)

    (setq ~file "emacs$library:..\\BEmacsUG.hlp")
    (if (file-exists ~file)
	(progn
	    (setq ~seperator-required 1)
	    (setq ~cmd (concat "(WIN-help-command \"" (quote (expand-file-name ~file)) "\")"))
	    (bind-to-key ~cmd "\(menu)hu")
	    (UI-add-menu "hu" 99 "&Help" 3 "Emacs &User's Guide")
	)
    )
    (setq ~file "emacs$library:..\\BEmacsPG.hlp")
    (if (file-exists ~file)
	(progn
	    (setq ~seperator-required 1)
	    (setq ~cmd (concat "(WIN-help-command \"" (quote (expand-file-name ~file)) "\")"))
	    (bind-to-key ~cmd "\(menu)hp")
	    (UI-add-menu "hp" 99 "&Help" 4 "Emacs &Programer's Guide")
	)
    )
    (setq ~file "emacs$library:..\\BEmacsER.hlp")
    (if (file-exists ~file)
	(progn
	    (setq ~seperator-required 1)
	    (setq ~cmd (concat "(WIN-help-command \"" (quote (expand-file-name ~file)) "\")"))
	    (bind-to-key ~cmd "\(menu)he")
	    (UI-add-menu "he" 99 "&Help" 5 "Emacs &Extensions Reference")
	)
    )
    (if ~seperator-required
	(UI-add-menu "hu" 99 "&Help" 6 "-"))
)
(UI-window-maximize)
[emacs_windows.ml]
; 
; emacs_windows.ml - windows user interface functions
; 
; Barry A. Scott  (c) 1993-1995
; 
(defun
    (WIN-save-file-as
	~old-current-directory
	(setq ~old-current-directory (current-directory))

	(setq UI-save-as-file-name current-buffer-file-name)

	(if
	    (UI-file-save-as)
	    (progn
		(write-named-file UI-save-as-file-name)
	    )
	)
	(change-directory ~old-current-directory)
    )
)

(defun
    (WIN-open-file
	~old-current-directory
	(setq ~old-current-directory (current-directory))

	(if
	    (UI-file-open)
	    (progn
		(visit-file UI-open-file-name)
		(setq read-only-buffer UI-open-file-readonly)
	    )
	)
	(change-directory ~old-current-directory)
    )
)

(defun
    (WIN-open-recent
	(visit-file UI-open-file-name)
    )
)

(defun
    (WIN-save-file
	(write-current-file)
    )
)

(defun
    (WIN-save-environment
	(message "Saving Environment...") (sit-for 0)
	(save-environment "")
	(message "Environment saved")
    )
)

(defun
    (WIN-edit-cut
	(UI-edit-copy)
	(erase-region)
	(unset-mark)
    )
)

(defun
    (WIN-edit-copy
	(UI-edit-copy)
	(unset-mark)
    )
)

(defun
    (WIN-edit-paste
	(UI-edit-paste)
    )
)

(defun
    (WIN-find-string
	(setq UI-search-string search-string)
	(UI-find)                            
	(novalue)
    )
)

(defun
    (WIN-find-and-replace
	(setq UI-search-string search-string)
	(UI-find-and-replace)
	(novalue)
    )
)

;
;	drag and drop support function.
;	Do a visit-file unless the user is in the minibuffer
;
(defun UI-drop-file(~file)
    (if (= current-buffer-name "Minibuf")
	(insert-string ~file)
	(visit-file ~file)
    )
)

;
;	DDE support for file opening.
;	This requires that Emacs is registered with windows
;
(defun UI-DDE-command (command)
    ~cmd ~arg1
    (save-excursion
	(temp-use-buffer "~DDE-hack")
	(erase-buffer)
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(insert-string command)
	(beginning-of-file)
	(if (looking-at "[[]*open(\"\\(.*\\)\")")
	    (progn
		(setq ~cmd "visit-file")
		(region-around-match 1)
		(setq ~arg1 (region-to-string))
	    )
	    (message "Unknown DDE command " command)
	)
    )
    (if (= ~cmd "visit-file")
	(visit-file ~arg1)
    )
)

(defun
    WIN-help-command(~file (get-tty-file "Windows Help: "))
    (UI-windows-help ~file "contents" "")
)

; 
; load up the key bindings for the menu system
; 
(execute-mlisp-file "emacs_windows.key")
[emacs_spell.ml]
(progn
    (error-occurred (external-function "emacs$spell" "emacs$share:emacs_spell_shr"))

    (if (! (is-bound ~spell-init))
        (progn
	    (declare-global
		~spell-init
	    )
	    (setq ~spell-init 0)
	)
    )
    (if (! (is-bound spell-master-dictionary))
        (progn
	    (declare-global spell-master-dictionary)
	    (setq spell-master-dictionary "lngsplaed")
	)
    )
    (if (! (is-bound spell-ram-dictionary))
        (progn
	    (declare-global spell-ram-dictionary)
	    (setq spell-ram-dictionary "lngsplaem")
	)
    )
    (if (! (is-bound spell-memory-size))
        (progn
	    (declare-global spell-memory-size)
	    (setq spell-memory-size 4096)
	)
    )
    (if (! (is-bound spell-personal-dictionary))
        (progn
	    (declare-global spell-personal-dictionary)
	    (setq spell-personal-dictionary "personal")
	)
    )
(defun correct-word (cw-word (get-tty-string ": correct-word "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (emacs$spell 8 cw-word)
)
(defun spell-check-word (cw-word (get-tty-string ": spell-check-word "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (emacs$spell 7 cw-word)
)
(defun get-tty-correction
  (cw-word (get-tty-string ": get-tty-correction (word) ")
  cw-prompt (get-tty-string ": get-tty-correction (prompt) "))
    (if (! ~spell-init)
        (progn
 	    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
	    (emacs$spell 4 spell-personal-dictionary)
	    (setq ~spell-init 1)
	)
    )
    (expand-from-string-table cw-prompt (emacs$spell 8 cw-word))
)
)
[emacs_sys.ml]
(progn
(defun
    (~sys_literal_setup_incr
	~prefix ~value ~incr ~name ~index
	
	(setq ~prefix (arg 1))
	(setq ~value (arg 2))
	(setq ~incr (arg 3))
	(setq ~index 4)
	(while (<= ~index (nargs))
	    (progn
		(setq ~name (concat ~prefix (arg ~index)))
		(if (!= ~name "")
		    (progn
			(execute-mlisp-line
			    (concat "(declare-global " ~name ")"))
			(execute-mlisp-line
			    (concat "(setq " ~name " " ~value ")"))
		    )
		)
		(setq ~index (+ 1 ~index))
		(setq ~value (+ ~value ~incr))
	    )
	)
    )
)
(defun
    (~sys_literal_setup_mask
	~prefix ~value ~name ~index
	
	(setq ~prefix (arg 1))
	(setq ~value (arg 2))
	(setq ~index 3)
	(while (<= ~index (nargs))
	    (progn
		(setq ~name (concat ~prefix (arg ~index)))
		(if (!= ~name "")
		    (progn
			(execute-mlisp-line
			    (concat "(declare-global " ~name ")"))
			(execute-mlisp-line
			    (concat "(setq " ~name " " ~value ")"))
		    )
		)
		(setq ~index (+ 1 ~index))
		(setq ~value (<< ~value 1))
	    )
	)
    )
)
(defun
    (~sys_literal_setup_equal
	~prefix ~name ~index
	
	(setq ~prefix (arg 1))
	(setq ~index 2)
	(while (<= ~index (nargs))
	    (progn
		(setq ~name (concat ~prefix (arg ~index)))
		(setq ~index (+ 1 ~index))
		(if (!= ~name "")
		    (progn
			(execute-mlisp-line
			    (concat "(declare-global " ~name ")"))
			(execute-mlisp-line
			    (concat "(setq " ~name " " (arg ~index) ")"))
		    )
		)
		(setq ~index (+ 1 ~index))
	    )
	)
    )
)

(external-function "sys$fao" "emacs$share:emacs_sys_shr" "emacs$sys$fao")
(external-function "sys$filescan" "emacs$share:emacs_sys_shr" "emacs$sys$filescan")
(external-function "sys$getmsg" "emacs$share:emacs_sys_shr" "emacs$sys$getmsg")
(external-function "sys$setprn" "emacs$share:emacs_sys_shr" "emacs$sys$setprn")
(external-function "sys$getdvi" "emacs$share:emacs_sys_shr" "emacs$sys$getdvi")
(external-function "sys$getjpi" "emacs$share:emacs_sys_shr" "emacs$sys$getjpi")
(external-function "sys$getqui" "emacs$share:emacs_sys_shr" "emacs$sys$getqui")
(external-function "sys$getsyi" "emacs$share:emacs_sys_shr" "emacs$sys$getsyi")
(external-function "sys$sndjbc" "emacs$share:emacs_sys_shr" "emacs$sys$sndjbc")
(external-function "sys$setprv" "emacs$share:emacs_sys_shr" "emacs$sys$setprv")
(external-function "sys$getuai" "emacs$share:emacs_sys_shr" "emacs$sys$getuai")
(external-function "sys$setuai" "emacs$share:emacs_sys_shr" "emacs$sys$setuai")
)
[errmsgprs.ml]
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
			    (re-search-forward "^\\(..*\\..*\\)(\\([0-9][0-9]*\\)) :")
			)
			(if (error-occurred
				(re-search-forward
				    " \\([-\\_:.a-z0-9][-\\_:.a-z0-9]*\\) \\([0-9][0-9]*\\):")
			    )
			    (re-search-forward
				"\\([-\\_:.a-z0-9][-\\_:.a-z0-9]*\\)(\\([0-9][0-9]*\\)):")
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
	    (gcc-message-parser
		(if
		    (error-occurred
			(re-search-forward "^\\(.*\\):\\([0-9][0-9]*\\): .*$")
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
	    (error-message-parser
		(if (= operating-system-version "ultrix")
		    (c89-message-parser)
		    (gcc-message-parser)
		)
	    )
	)
    )
)
[expand-word-in-buffer.ml]
; 
; expand-word-in-buffer.ml
;
;   Improve to only expand the word that the cursor is on.
; 
(defun ~expand-generic-word-in-buffer
    (
	~type
	~prompt
    )
    ~word
    (save-excursion
	;
	; make sure that there is a word here
	(if (looking-at "\\w")
	    (progn
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
	    )
	    (progn
		(error-occurred (backward-character))
		(if (looking-at "\\w")
		    (progn
			(error-occurred (forward-character))
			(backward-word)
			(set-mark)
			(forward-word)
		    )
		    (set-mark)
		)
	    )
	)
	(setq ~word (region-to-string))
	(push-back-character ~word)
	(setq ~word
	    (execute-mlisp-line
		(concat
		    "(get-tty-" ~type " \": expand-" ~prompt "-in-buffer \")"
		)
	    )
	)
	(erase-region)
    )
    (insert-string ~word)
    (novalue)
)    
(defun
    (expand-mlisp-word-in-buffer
	(~expand-generic-word-in-buffer "command" "mlisp-word")
    )
)
(defun
    (expand-buffer-in-buffer
	(~expand-generic-word-in-buffer "buffer" "buffer")
    )
)
(defun
    (expand-file-in-buffer
	(~expand-generic-word-in-buffer "file" "file")
    )
)
(defun
    (expand-mlisp-variable-in-buffer
	(~expand-generic-word-in-buffer "variable" "mlisp-variable")
    )
)
[expandx.ml]
(defun
    (expand-mlisp-word
	(insert-string (get-tty-command ": expand-mlisp-word "))
	(novalue))

    (expand-mlisp-variable
	(insert-string (get-tty-variable ": expand-mlisp-variable "))
	(novalue))
)
[flasher.ml]
(progn

(declare-global paren-flash-loaded)
(declare-buffer-specific paren-flash-wait paren-flash-message)
(if (! paren-flash-loaded)
    (progn
	(setq-default paren-flash-message 1)
	(setq-default paren-flash-wait 10)
    )
)
(setq paren-flash-loaded 1)

(defun
    (paren-flash s			; "("
	(insert-character (last-key-struck))
	(save-excursion
	    (if (! (backward-paren 0))(error-message "No match"))
	    (if (dot-is-visible)
		(sit-for paren-flash-wait)
		(if paren-flash-message
		    (progn
			(set-mark)(beginning-of-line)
			(setq s (concat (region-to-string) " >>"))
			(end-of-line)(setq s (concat s (region-to-string)))
			(message s)
		    )
		)
	    )
	)
    )
)
)
[form.ml]
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
[fscndef.ml]
; 
; load up $fscndef symbols
; 
(if
    (if (is-bound $fscndef-loaded)
	(! $fscndef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "fscn_def")
	(declare-global $fscndef-loaded)
	(setq $fscndef-loaded 1)
    )
)
[fscn_def.ml]
; 
; $fscndef
; 
(~sys_literal_setup_incr "fscn$_" 1 1
    "filespec"             ;  complete filespec
    "node"                 ;  node:: field
    "device"               ;  device: field
    "root"                 ;  [root.] field
    "directory"            ;  [directory] field
    "name"                 ;  name field
    "type"                 ;  .typ field
    "version"              ;  ;version field
)
[fundmode.ml]
;-*-mlisp-*-
(progn

(defun
    (newline-and-indent-relative
	(if (= (current-indent) 1)
	    (newline)
	    (progn
		(newline)
		(indent-relative)
	    )
	)
    )
)
(defun
    (dedent-relative i col
	(setq i (current-indent))
	(setq col 1)
	(if (! (save-excursion (beginning-of-line) (bobp)))
	    (save-excursion
		(previous-line)
		(beginning-of-line)
		(while (& (! (bobp))
			   (>= (current-indent) i)
			   (!= (current-indent)
			       (save-excursion
				   (end-of-line) (current-column)
			       )
			   )
		       )
		    (previous-line)
		)
		(if (! (bobp)) (setq col (current-indent)))
	    )
	)
	(save-excursion (delete-white-space) (to-col col))
    )
)
(defun
    (indent-relative col i
	
	(setq col 0)
	(setq i (current-column))
	(if (! (save-excursion (beginning-of-line) (bobp)))
	    (save-excursion
		(previous-line)
		(if (! (eolp))(forward-character))
		(while
		    (&
			(! (eolp))
			(!
			    (&
				(| (= (preceding-char) 32)
				    (= (preceding-char) 9)
				)
				(!= (following-char) 9)
				(!= (following-char) 32)
			    )
			)
		    )
		    (forward-character))
		(if (! (eolp)) (setq col (current-column)))
	    )
	)
	(if col
	    (progn (delete-white-space) (to-col col))
	    (to-col
		(+
		    (* logical-tab-size (/ (current-column) logical-tab-size))
		    logical-tab-size
		)
	    )
	)
    )
)
(defun
    (fundamental-mode
	(use-local-map "Fundamental-mode-map")
	(use-syntax-table "Fundamental-mode")
	(use-abbrev-table "Fundamental-mode")
	(setq mode-string "Fundamental")
	(novalue)
    )
)
(save-window-excursion
    (switch-to-buffer "keymap-hack")
    (define-keymap "Fundamental-mode-map")
    (use-local-map "Fundamental-mode-map")
    (local-bind-to-key "indent-relative" "\t")
    (local-bind-to-key "dedent-relative" "\e\t")
    (local-bind-to-key "newline-and-indent-relative" "\n")
    (if (! (is-bound paren-flash-wait)) (execute-mlisp-file "flasher.ml"))
    (use-syntax-table "Fundamental-mode")
    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "paren" "{" "}")
    (modify-syntax-table "paren" "[" "]")
    (local-bind-to-key "paren-flash" ")")
    (local-bind-to-key "paren-flash" "]")
    (local-bind-to-key "paren-flash" "}")
)
)
[globdel.ml]
(progn
(defun
    (global-delete-lines
	(re-global-delete-lines (quote (arg 1 ": global-delete-lines ")))
    )
)
(defun
    (re-global-delete-lines target delcnt
	(setq delcnt 0)
	(setq target (arg 1 ": re-global-delete-lines containing "))
	(save-excursion
	    (error-occurred
		(beginning-of-file)
		(while 1
		    (re-search-forward target)
		    (beginning-of-line)
		    (kill-to-end-of-line)
		    (kill-to-end-of-line)
		    (setq delcnt (+ delcnt 1))
		)
	    )
	)
	(message (concat delcnt " lines deleted"))
    )
)
)
[goto.ml]
; 
; Barry A. Scott	18-Jun-1983
; Updated to use *defun syntax 13-Nov-1993 Barry A. Scott
; 
; The line-number-offset is used to make goto-line work
; in source files where you know only list line numbers.
; 
; The adjust-line-numbers function will set up this
; variable give the desired line number for the current
; line.
; 
; The page-number-offset is used to make goto-page work
; in source files where you know only list page numbers.
; 
; The adjust-page-numbers function will set up this
; variable give the desired page number for the current
; page.
; 
(declare-buffer-specific line-number-offset page-number-offset)
(setq-default line-number-offset 0)
(setq-default page-number-offset 0)

(defun
    adjust-line-number
    (~adjust-by (get-tty-string ": adjust-line-number (new value) "))
    ~line-no
    
    (setq line-number-offset 0)
    (setq ~line-no (current-line-number))
    
    (setq line-number-offset
	(- ~adjust-by ~line-no)
    )
    (novalue)
)

(defun
    
    ;***************************************************************
    ; goto-line
    ; - moves cursor to beginning of indicated line.
    ; - line number is taken from prefix if provided,
    ;   is prompted for otherwise.
    ; - first line of file is line 1
    goto-line
    (line
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (+ (get-tty-string ": goto-line "))
	)
    )
    (setq line (- line 1 line-number-offset))
    (beginning-of-file)
    (if (> line 0)
	(next-line line)
    )
    (beginning-of-line)
    (novalue)
)

(defun
    ;***************************************************************
    ; goto-percent
    ; - moves cursor past indicated percentage of the buffer.
    ; - percentage is taken from prefix if provided,
    ;   is prompted for otherwise.
    ; - (goto-percent n) goes to the character closest to the
    ;   beginning of the buffer that is reported as n% in the
    ;   status line.  This is 2 characters further into the
    ;   buffer than you'd expect.  As a result, (goto-percent 0)
    ;   goes to character 1 in the file, since I didn't feel like
    ;   fixing that special case.
    ; - (goto-percent 100) goes to the end of the buffer.
    ; - remember that the position of the first character
    ;   in the buffer is 1.
    goto-percent
    (percent
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (+ (get-tty-string ": goto-percent "))
	)
    )
    (goto-character (+ (/ (* (buffer-size) percent) 100) 2))
    (novalue)
    
)    
(defun
    (current-page-number
	~page-number
	(setq ~page-number 1)
	(looking-at "\^l")  	; prime search code
	(save-excursion
	    (while
		(! (error-occurred (re-search-reverse "")))
		(setq ~page-number (+ ~page-number 1))
	    )
	)
	~page-number
    )
)
(defun
    goto-page
    (~page-number
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (- (arg 1 ": goto-page ") 1 page-number-offset)
	)
    )
    (beginning-of-file)
    (looking-at "\^l")
    (error-occurred
	(while ~page-number
	    (re-search-forward "")
	    (setq ~page-number (- ~page-number 1))
	)
    )
    (next-line)
    (beginning-of-line)
    (novalue)
)
[grep.ml]
; 
; grep.ml
;   Copyright (c) 1993 Barry A. Scott
; 
; Grep for emacs that does not depend on any external programs
; and offers the full power of Emacs regular expressions
; 
(defun grep 
    (~pattern (get-tty-string ": grep (pattern) ")
	~files (get-tty-file ": grep (files) "))
    
    ~file
    
    (pop-to-buffer "grep-results")
    (define-keymap "grep-keymap")
    (use-local-map "grep-keymap")
    (execute-mlisp-file "grep.key")
    
    (temp-use-buffer "grep-file")
    (erase-buffer)
    (setq current-buffer-journalled 0)
    (temp-use-buffer "grep-results")
    (erase-buffer)
    (setq current-buffer-journalled 0)
    
    (setq ~file (expand-file-name ~files))
    (while
	(!= ~file "")
	; do
	(pop-to-buffer "grep-results") 
	(end-of-file)
	(message "grep working on " ~file "...")
	(sit-for 0)
	(save-excursion
	    (temp-use-buffer "grep-file")
	    (erase-buffer)
	    (insert-file ~file)
	    (while (! (error-occurred (re-search-forward ~pattern)))
		(save-excursion ~match ~line
		    (setq ~line (current-line-number))
		    (beginning-of-line)
		    (set-mark)
		    (next-line)
		    (setq ~match (region-to-string))
		    (pop-to-buffer "grep-results")
		    (end-of-file)
		    ; only insert the file name if a match is found
		    (if (!= ~file "")
			(progn
			    (insert-string ~file "\n")
			    (setq ~file "")
			)
		    )
		    (insert-string ~line "\t" ~match)
		    (sit-for 0)
		)
		(next-line)
		(beginning-of-line)
	    )
	)
	(setq ~file (expand-file-name ""))
	(end-of-file)
	(sit-for 0)
    )
    (beginning-of-file)
)

(defun
    (grep-visit-match
	~file ~line

	(setq ~file "")
	(setq ~line 0)
	(save-excursion
	    (beginning-of-line)
	    (if (looking-at "[0-9][0-9]*")
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
	    (re-search-reverse "^[^0-9]")
	    (beginning-of-line)
	    (set-mark)
	    (end-of-line)
	    (setq ~file (region-to-string))
	)	
	(visit-file ~file)
	(if ~line
	    (goto-line ~line))
    )
)
[grep.key]
;
;	Copyright 1993 Barry A. Scott
;
; grep.key - key bindings for the grep result buffer
;
(local-bind-to-key "grep-visit-match" "\^m")
[help.ml]
(progn
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
)
[helpchar.key]
(progn
    (bind-to-key "help-key" "\^_")	; CTRL-? for VT100s
    (bind-to-key "help-key" "\(help)")	; HELP for LK201s
)
[helpchar.ml]
(progn
    (define-keymap "~helpchar-prefix")
    (define-keymap "~helpchar-view-prefix")
    (defun
	(~helpchar-prompt
	    (delete-other-windows)
	    (split-current-window)
	    (pop-to-buffer "Minibuf")
	    (previous-window)
	    (switch-to-buffer "Help prompt")
	    (while (! (error-occurred (shrink-window))) 0)
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (setq mode-line-format "")
	    (erase-buffer)
	    (insert-string (arg 1))
	    (previous-window)
	)
	(~helpchar-more
	    (if (save-window-excursion
		    (end-of-file)
		    (dot-is-visible)
		)
		"- End -"
		"More..."
	    )
	)
	(~helpchar-next-page
	    (next-page)
	    (setq mode-string (~helpchar-more))
	)
	(~helpchar-previous-page
	    (previous-page)
	    (setq mode-string (~helpchar-more))
	)
	(~helpchar-beginning
	    (beginning-of-file)
	    (setq mode-string (~helpchar-more))
		)
	(~helpchar-end
	    (end-of-file)
	    (setq mode-string (~helpchar-more))
	)
	(help
	    (help-key)
	)
	(help-key
	    (message "Help category (? for help):")
	    (~helpchar-prefix)
	)
    )
    (save-excursion
	~helpchar-variable
	(temp-use-buffer "Help h window")
	(use-local-map "~helpchar-view-prefix")
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(setq mode-line-format "")
	(temp-use-buffer "Help window")
	(use-local-map "~helpchar-view-prefix")
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(setq mode-line-format "                                                                  %10m")
	(setq ~helpchar-variable 0)
	(while (< ~helpchar-variable 128)
	    (local-bind-to-key "illegal-operation" ~helpchar-variable)
	    (setq ~helpchar-variable (+ ~helpchar-variable 1))
	)
	(local-bind-to-key "~helpchar-next-page" " ")
	(local-bind-to-key "~helpchar-next-page" "\(next screen)")
	(local-bind-to-key "~helpchar-previous-page" "\^H")
	(local-bind-to-key "~helpchar-previous-page" "\(prev screen)")
	(local-bind-to-key "exit-emacs" "\^C")
	(local-bind-to-key "exit-emacs" "\(f10)")
	(local-bind-to-key "exit-emacs" "q")
	(local-bind-to-key "exit-emacs" "^]")
	(autoload "~helpchar-view-help" "helpchar_view_help")
	(local-bind-to-key "~helpchar-view-help" "?")
	(local-bind-to-key "~helpchar-view-help" "\^_")
	(local-bind-to-key "~helpchar-view-help" "\(help)")
	(local-bind-to-key "~helpchar-beginning" "b")
	(local-bind-to-key "~helpchar-beginning" "\(up)")
	(local-bind-to-key "~helpchar-end" "e")
	(local-bind-to-key "~helpchar-end" "\(down)")
	(use-local-map "~helpchar-prefix")
	(autoload "~helpchar-help" "helpchar_help")
	(local-bind-to-key "~helpchar-help" "?")
	(local-bind-to-key "~helpchar-help" "\^_")
	(local-bind-to-key "~helpchar-help" "\(help)")
	(autoload "~helpchar-describe" "helpchar_describe")
	(local-bind-to-key "~helpchar-describe" "d")
	(autoload "~helpchar-describe-bindings" "helpchar_describe_bindings")
	(local-bind-to-key "~helpchar-describe-bindings" "b")
	(autoload "~helpchar-variable" "helpchar_variable")
	(autoload "~helpchar-compose" "helpchar_compose")
	(local-bind-to-key "~helpchar-compose" "c")
	(local-bind-to-key "~helpchar-variable" "v")
	(autoload "~helpchar-apropos" "helpchar_apropos")
	(local-bind-to-key "~helpchar-apropos" "a")
	(autoload "~helpchar-news" "helpchar_news")
	(local-bind-to-key "~helpchar-news" "n")
	(if (! (is-bound ~info-is-init))
	    (autoload "info" "info")
	)
	(autoload "~helpchar-info" "helpchar_info")
	(local-bind-to-key "~helpchar-info" "i")
	(autoload "~helpchar-describe-key" "helpchar_describe_key")
	(local-bind-to-key "~helpchar-describe-key" "k")
	(autoload "~helpchar-help-i" "helpchar_help_i")
	(local-bind-to-key "~helpchar-help-i" "h")
	(autoload "~helpchar-where-is" "helpchar_where_is")
	(local-bind-to-key "~helpchar-where-is" "w")
    )
)
[helpchar_apropos.ml]
(defun
    (~helpchar-apropos
	~helpchar-variable
	(setq ~helpchar-variable (get-tty-string "Doc for word? "))
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (insert-string "Commands\n--------\n")
	    (delete-other-windows)
	    (apropos ~helpchar-variable)
	    (yank-buffer "Help")
	    (delete-buffer "Help")
	    (insert-string "\nVariables\n---------\n")
	    (apropos-variable ~helpchar-variable)
	    (yank-buffer "Help")
	    (delete-buffer "Help")
	    (beginning-of-file)
	    (use-local-map "~helpchar-view-prefix")
	    (setq mode-string (~helpchar-more))
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
[helpchar_compose.ml]
(defun
    (~helpchar-compose
	~helpchar-variable
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (delete-other-windows)
	    (fetch-database-entry "MLisp-library" "lk201_compose_chart")
	    (beginning-of-file)
	    (use-local-map "~helpchar-view-prefix")
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (setq mode-string (~helpchar-more))
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
[helpchar_describe.ml]
(defun
    (~helpchar-describe
	~helpchar-variable
	(setq ~helpchar-variable (get-tty-command "Doc for command? "))
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (delete-other-windows)
	    (extend-database-search-list "describe" "emacs$library:emacsdesc")
	    (fetch-database-entry "describe"  (concat "C" ~helpchar-variable))
	    (use-local-map "~helpchar-view-prefix")
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (setq mode-string (~helpchar-more))
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
[helpchar_describe_bindings.ml]
(defun
	(~helpchar-describe-bindings
	    (save-window-excursion
		(describe-bindings)
		(switch-to-buffer "Help window")
		(erase-buffer)
		(delete-other-windows)
		(yank-buffer "Help")
		(beginning-of-file)
		(use-local-map "~helpchar-view-prefix")
		(~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
		(setq mode-string (~helpchar-more))
		(recursive-edit)
		(use-local-map "~helpchar-prefix")
	    )
	)
)
[helpchar_describe_key.ml]
(defun
    (~helpchar-describe-key
	(push-back-character "\^Q")
	(message (describe-key (get-tty-string "Doc for keys (<Cr> to terminate)? ")))
    )
)
[helpchar_help.ml]
(defun
    (~helpchar-help
	x
	(save-window-excursion
	    (switch-to-buffer "Help h window")
	    (erase-buffer)
	    (delete-other-windows)
	    (setq mode-line-format "")
	    (insert-string
		(concat
		    "HELP: on Emacs\n\n"
		    "    Key        Meaning\n"
		    "    ---        -------\n"
		    "    a          Show command containing a string\n"
		    "    b          Describe all key bindings\n"
		    "    c          LK201 compose key sequence chart\n"
		    "    d          Describe a command\n"
		    "    h          Access to VAX/VMS help\n"
		    "    i          Access INFO\n"
		    "    k          Describe a key binding\n"
		    "    n          Print out Emacs news\n"
		    "    v          Describe a variable\n"
		    "    w          Find a function on the keyboard\n"
		    "    ?          Display this help text\n"
		)
	    )
	    (beginning-of-file)
	    (message "-- Type any character to continue HELP --")
	    (setq x (get-tty-character))
	    (if (| (= x 'a') (= x 'd') (= x 'i') (= x 'n') (= x 'v')
		    (= x 'h') (= x 'k') (= x 'b') (= x 'c') (= x 'w'))
		(push-back-character x)
	    )
	    (help-key)
	)
    )
)
[helpchar_help_i.ml]
(defun
    (~helpchar-help-i
	(message "Transferring to the VAX/VMS HELP reader")
	(sit-for 0)
	(vms-help)
    )
)
[helpchar_info.ml]
(defun
    (~helpchar-info
	(message "Transferring to the INFO documentation reader")
	(sit-for 0)
	(info)
    )
)
[helpchar_news.ml]
(defun
    (~helpchar-news
	~helpchar-variable
	(save-window-excursion
	    (switch-to-buffer "Help window")
	    (erase-buffer)
	    (delete-other-windows)
	    (if
		(error-occurred
		    (insert-file "SYS$HELP:EMACSNEWS.TXT")
		)
		(if
		    (error-occurred
			(insert-file "EMACS$LIBRARY:EMACSNEWS.TXT")
		    )
		    (error-message "No news is available.")
		)
	    )
	    (use-local-map "~helpchar-view-prefix")
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	    (setq mode-string (~helpchar-more))
	    (recursive-edit)
	    (use-local-map "~helpchar-prefix")
	)
    )
)
[helpchar_variable.ml]
(defun
	(~helpchar-variable
	    ~helpchar-variable
	    (setq ~helpchar-variable (get-tty-variable "Doc for variable? "))
	    (save-window-excursion
		(switch-to-buffer "Help window")
		(erase-buffer)
		(delete-other-windows)
		(extend-database-search-list "describe" "emacs$library:emacsdesc")
		(fetch-database-entry "describe"  (concat "V" ~helpchar-variable))
		(use-local-map "~helpchar-view-prefix")
		(~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
		(setq mode-string (~helpchar-more))
		(recursive-edit)
		(use-local-map "~helpchar-prefix")
	    )
	)
)
[helpchar_view_help.ml]
(defun
    (~helpchar-view-help
	x
	(save-window-excursion
	    (switch-to-buffer "Help view")
	    (erase-buffer)
	    (delete-other-windows)
	    (setq mode-line-format "")
	    (insert-string
		(concat
		    "HELP: on Emacs in help view mode\n\n"
		    "    Key        Meaning\n"
		    "    ---        -------\n"
		    "    <Space>	Move forward one page\n"
		    "    <Bs>	Move backward one page\n"
		    "    b		Move to the beginning of the help\n"
		    "    e		Move to the end of the help\n"
		    "    q		Continue editting\n"
		    "    ^C		Continue editting\n"
		    "    ?		Display this help text\n"
		)
	    )
	    (beginning-of-file)
	    (~helpchar-prompt "-- Type any character to continue HELP --")
	    (setq x (get-tty-character))
	    (if (| (= x ' ') (= x '^H') (= x 'b') (= x 'e') (= x 'q')
		    (= x '^C') (= x '^['))
		(push-back-character x)
	    )
	    (~helpchar-prompt "-- Type `q' or ^C to continue editing or ? for help --")
	)
    )
)
[helpchar_where_is.ml]
(defun
    (~helpchar-where-is
	i j
	(setq i (get-tty-command ": where-is "))
	(save-window-excursion
	    (describe-bindings)
	    (if (error-occurred (search-forward i))
		(error-message
		    (concat "Function \"" i "\" is not bound to a key")
		)
	    )
	    (beginning-of-line)
	    (set-mark)
	    (search-forward " ")
	    (backward-character)
	    (setq j (region-to-string))
	    (re-search-forward "[ ]*")
	    (set-mark)
	    (end-of-line)
	    (message
		(concat "Function """ (region-to-string) """ is bound to "
		    j)
	    )
	)
    )
)
[incsearch.key]
;
;	incsearch.key
;
;	setup keybindings for incsearch
;
(if xon-mode
    (setup-incsearch		; setup without Xon and Xoff
	"abort"		"\^g"
	"delete"	"\^?"
	"forward"	"\^^s"
	"reverse"	"\^r"
	"recurse"	"\^l"
	"replace"	"\^p"
	"quote"		"\^^q"
	"help"		"\^_"
	"exit"		"\e"
    )
    (setup-incsearch		; Setup using Xon and Xoff
	"abort"		"\^g"
	"delete"	"\^?"
	"forward"	"\^s"
	"reverse"	"\^r"
	"recurse"	"\^l"
	"replace"	"\^p"
	"quote"		"\^q"
	"help"		"\^_"
	"exit"		"\e"
    )
)
[incsearch.ml]
; 
;	incsearch.ml	Jeff Lomicka's incremental search 14-Feb-1984
;	Network address: REGINA::LOMICKAJ  Comments, enhancements, and
;	complaints are encouraged.
;	(Uses local keymap to acheive functions, therefore it works
;	with the keyboard macro facility.)
;
;	Modified:    21-Mar-1984	Barry A. Scott
;	Make changes to allow the keys to defined simply from
;	incsearch.key by calling setup-incsearch with parameters.
; 
;	5-Apr-1984	J.A.Lomicka	Reformat, make search-string
;	echo before searching.
; 
;	 1-Aug-1984	Barry A. Scott
;	add a push back key-exit function.
; 
;	12-Nov-1985	Barry A. Scott
;	change the name of the variable search-string.
; 
(progn
(declare-global
    ~inc-search-string
    ~inc-replace-string
    ~ins-direction
    ~ins-quote-key
    ~ins-stack-pointer
    ~ins-state-stack
    ~ins-is-failing
    ~ins-old-local-keymap
    ~ins-old-search-string)

(define-keymap "no-local-map");	Hack because we can't undo local map
(setq ~inc-search-string "")
(setq ~inc-replace-string "")
;
;	Because of some funny interactions with the global binding of
;	quote-character, the key sequence or character used for quote
;	character must be assigned to ~ins-quote-key, so that it can be
;	turned off and on when entering and leaving incremental search.
;	This sequence will also be used for the incremental search
;	quote-character operation.
;
(defun
    (setup-incsearch ~key ~arg-num ~arg
	(save-excursion
	    (temp-use-buffer "~inc-search-key-hack")
	    (define-keymap "~incsquotemap")
	    (define-keymap "~incsearchmap")
	    (use-local-map "~incsquotemap")
	    (setq ~key 0)
	    (while (< ~key 128)
		(local-bind-to-key "~ins-normal-char" ~key)
		(setq ~key (+ 1 ~key))
	    )
	    (use-local-map "~incsearchmap")
	    (setq ~key 0)
	    (while (< ~key ' ')
		(local-bind-to-key "~ins-exit" ~key)
		(setq ~key (+ 1 ~key))
	    )
	    (while (< ~key 127)
		(local-bind-to-key "~ins-normal-char" ~key)
		(setq ~key (+ 1 ~key))
	    )
	    ; 
	    ; setup key binds from parameters.
	    ; arg n is the name of the functions required and arg n+1 is
	    ; a string describing the key to be bound.
	    ; e.g. "abort" "\^g"
	    ; 
	    (setq ~arg-num 0)
	    (while (< (setq ~arg-num (+ 1 ~arg-num)) (nargs))
		(progn
		    (setq ~arg (arg ~arg-num))
		    (setq ~key (arg (setq ~arg-num (+ 1 ~arg-num))))
		    (if
			(= ~arg "delete") (local-bind-to-key "~ins-delete" ~key)
			(= ~arg "abort") (local-bind-to-key "~ins-abort" ~key)
			(= ~arg "forward") (local-bind-to-key "~ins-next" ~key)
			(= ~arg "reverse") (local-bind-to-key "~ins-previous" ~key)
			(= ~arg "recurse") (local-bind-to-key "~ins-recurse" ~key)
			(= ~arg "replace") (local-bind-to-key "~ins-replace" ~key)
			(= ~arg "help") (local-bind-to-key "~ins-help" ~key)
			(= ~arg "exit") (local-bind-to-key "~ins-GetOutOf" ~key)
			(= ~arg "exit-and-do") (local-bind-to-key "~ins-exit" ~key)
			(= ~arg "exit-and-do-key") (local-bind-to-key "~ins-exit" ~key)
			(= ~arg "normal") (local-bind-to-key "~ins-normal-char" ~key)
			(= ~arg "quote") (progn
					     (local-bind-to-key "~ins-quote" ~key)
					     (setq ~ins-quote-key ~key)
					 )
			(if (error-occurred (local-bind-to-key ~arg ~key))
			    (error-message "Unknown parameter to setup-incsearch " ~arg)
			)
		    )
		)
	    )
	    (delete-buffer "~inc-search-key-hack")
	);	end of save-excursion
    )
);	end of setup-incsearch
;
;	Functions for incremental search inner workings
;
(defun
    (~ins-push
	(setq ~ins-state-stack
	    (concat
		~ins-is-failing
		(if (= ~ins-direction "forward") "f" "r")
		(char-to-string (length ~inc-search-string))
		~ins-state-stack
	    )
	)
	(~ins-push-pos)
    )
)
    
(defun
    (~ins-pop dot len		; pop search state
	(if (> (length ~ins-state-stack) 0)
	    (progn
		(setq ~ins-is-failing (+ 0 (substr ~ins-state-stack 1 1)))
		(setq ~ins-direction
		    (if (= (substr ~ins-state-stack 2 1) "f")
			"forward" "reverse")
		)
		(setq len (string-to-char (substr ~ins-state-stack 3 1)))
		(setq ~inc-search-string (substr ~inc-search-string 1 len))
		(setq ~ins-state-stack (substr ~ins-state-stack 4 10000))
		(~ins-pop-pos)
	    )
	)
	(~ins-message)	; tell the world
    )
)
    
(defun
    (~ins-push-pos
	(setq ~ins-stack-pointer (+ ~ins-stack-pointer 1))
	(execute-mlisp-line
	    (concat
		"(progn (declare-global ~ins-mrk-stk-" ~ins-stack-pointer ")"
		"(setq ~ins-mrk-stk-" ~ins-stack-pointer " (dot)))"
	    )
	)
    )
)
    
(defun
    (~ins-pop-pos
	(execute-mlisp-line
	    (concat
		"(pop-to-buffer ~ins-mrk-stk-" ~ins-stack-pointer ")"
	    )
	)
	(execute-mlisp-line
	    (concat
		"(goto-character ~ins-mrk-stk-" ~ins-stack-pointer ")"
	    )
	)
	(setq ~ins-stack-pointer (- ~ins-stack-pointer 1))
    )
)
    
(defun
    (incremental-search
	(setq ~ins-old-local-keymap current-local-keymap)
	(setq ~ins-is-failing 0)
	(setq ~ins-stack-pointer 0)
	(setq ~ins-state-stack "")
	(setq ~ins-direction "forward")
	(setq ~ins-old-search-string ~inc-search-string)
	(setq ~inc-search-string "")
	(remove-binding ~ins-quote-key)
	(use-local-map "~incsearchmap")
	(~ins-message)
    )
)
    
(defun
    (reverse-incremental-search
	(incremental-search)
	(setq ~ins-direction "reverse")
	(~ins-message)
    )
)
    
(defun
    (~ins-message ; Print message according to state.
	(message
	    (concat
		(if ~ins-is-failing "Failing " "") 
		~ins-direction
		" I-search"
		(if (= ~inc-search-string "")
		    (concat " [" ~ins-old-search-string "]") "")
		": "
		~inc-search-string
	    )
	)
    )
)
    
(defun
    (~ins-forward-search ;search forward, set is-failing, print message.
	(~ins-message)
	(if (setq ~ins-is-failing
		(error-occurred (search-forward ~inc-search-string)))
	    (send-string-to-terminal "\^g")
	)
	(~ins-message)
    )
)
    
(defun
    (~ins-reverse-search ;search reverse, set is-failing, print message
	(~ins-message)
	(if (setq ~ins-is-failing
		(error-occurred (search-reverse ~inc-search-string)))
	    (send-string-to-terminal "\^g")
	)
	(~ins-message)
    )
)
    
(defun
    (~ins-AddAChar char			; Append last key typed
	(setq char (arg 1 "Next char?"))
	(if ~ins-is-failing
	    (progn
		(send-string-to-terminal "\^g")
		(~ins-message)
	    )
	    ; else
	    (progn
		(~ins-push)
		(setq ~inc-search-string (concat ~inc-search-string (char-to-string char)))
		(if (= ~ins-direction "forward") 
		    (if (c= char (following-char)) ;are we on target?
			(progn (forward-character)(~ins-message))
			;else we need to find the next one
			(~ins-forward-search)
		    )
		    ;else
		    (if (save-excursion
			    (goto-character (+ (dot) (length ~inc-search-string)))
			    (c= char (preceding-char))
			)
			(~ins-message)
			; else we need to find the previous one
			(~ins-reverse-search)
		    )
		)
	    )
	)
    )
)
    
;	Functions that are bound to incremental search keystrokes
    
(defun
    (~ins-exit				; Exit and execute key
	(~ins-GetOutOf)
	(push-back-character last-keys-struck)
    )
)
    
    
(defun
    (~ins-GetOutOf			; Exit
	(use-local-map
	    (if (= ~ins-old-local-keymap "")
		"no-local-map" ~ins-old-local-keymap
	    )
	)
	(bind-to-key "quote-character" ~ins-quote-key)
    )
)
    
(defun
    (~ins-quote
	(~ins-message)
	(~incsquotemap)
    )
)
    
(defun
    (~ins-normal-char
	(~ins-AddAChar (last-key-struck))
    )
)
    
(defun
    (~ins-abort
	(setq ~ins-state-stack
	    (substr
		~ins-state-stack
		(- (length ~ins-state-stack) 2)
		3
	    )
	)
	(setq ~ins-stack-pointer 1)
	(~ins-pop)(message "")
	(setq ~inc-search-string ~ins-old-search-string)
	(~ins-GetOutOf)
    )
)
    
(defun
    (~ins-next
	(~ins-push)
	(if (= (length ~inc-search-string) 0)
	    (setq ~inc-search-string ~ins-old-search-string))
	(setq ~ins-direction "forward")
	(if (> (length ~inc-search-string) 0); do not search if no string.
	    (~ins-forward-search); search
	)
	(~ins-message)
    )
    
)
    
(defun
    (~ins-previous
	(~ins-push)
	(if (= (length ~inc-search-string) 0)
	    (setq ~inc-search-string ~ins-old-search-string))
	(setq ~ins-direction "reverse")
	(if (> (length ~inc-search-string) 0); do not search if no string.
	    (~ins-reverse-search); search
	)
	(~ins-message)
    )
)
    
(defun
    (~ins-replace temp
	(setq temp
	    (get-tty-string (concat "replace with [" ~inc-replace-string "]:"))
	)
	(if (!= temp "")
	    (setq ~inc-replace-string temp)
	)
	(setq temp (length ~inc-search-string))
	(while (> temp 0)
	    (if (= ~ins-direction "reverse")
		(delete-next-character)
		;else
		(delete-previous-character)
	    )
	    (setq temp (- temp 1))
	)
	(insert-string ~inc-replace-string)
	(if (= ~ins-direction "reverse")
	    (search-reverse ~inc-replace-string)
	)
	(~ins-message)
    )
)
    
(defun
    (~ins-recurse
	(~ins-push)
	(~ins-GetOutOf)
	(recursive-edit)
	(remove-binding ~ins-quote-key)
	(use-local-map "~incsearchmap")
	(~ins-message)
    )
)
    
(defun
    (~ins-help
	(save-window-excursion
	    (describe-bindings)
	    (switch-to-buffer "Help")
	    (delete-other-windows)
	    (beginning-of-file)
	    (set-mark)
	    (re-search-forward "^Local Bindings")
	    (erase-region)
	    (insert-string
		(concat
		    "You are in incremental search. Exit incremental search by giving any\n"
		    "command that is not a special key. The special keys are:\n"
		)
	    )
	    (while (! (error-occurred (search-forward "~ins-exit")))
		(beginning-of-line)(set-mark)(next-line)(erase-region)
	    )
	    (beginning-of-file)
	    (error-occurred
		(~inc-replace-string
		    "~ins-next" "Search forward for the next occurance of string"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-previous" "Search reverse for a previous occurance of string"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-replace" "Replace string so far with another string (you type it in)"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-delete" "Undo action of last character"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-quote" "Quote next character into the search string"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-GetOutOf" "Exits incremental search here."
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-help" "Print help in help buffer."
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-abort" "Abort searching and go back to where we started"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-recurse" "recursive-edit & continue upon exit"
		))
	    (error-occurred
		(~inc-replace-string
		    "~ins-normal-char" "character is appended to the search string"
		))
	    (beginning-of-file)
	    (message "Type any character to continue incremental search")
	    (get-tty-character)
	    (~ins-message)
	); end save-window-excursion
    )
)
    
(defun
    (~ins-delete
	(~ins-pop); pop state.
    )
)
    ; 
    ; load up the key bindings and initialise the package
    ; 
    (execute-mlisp-file "incsearch.key")
)
[info.ml]
(progn
    (declare-global
	info-root-file
	info-default-directory
	~info-help-library
	~info-is-init
	~info-current-file
	~info-current-node
	~info-file-part
	~info-node-part
	~info-help-part
	~info-file-def
	~info-help-def
	~info-^n-file
	~info-^n-node
	~info-^n-help-flag
    )
    (setq info-root-file "emacsroot")
    (setq info-default-directory "emacs$info:")
    (setq ~info-current-file "")
    (setq ~info-current-node "")
    (defun
	(info
	    i
	    j
	    (if (= current-buffer-name "INFO")
		(error-message "You are already using INFO.")
	    )
	    (save-window-excursion
		(if (! ~info-is-init)
		    (progn
			(switch-to-buffer "Hack buffer")
			(widen-region)
			(erase-buffer)
			(setq case-fold-search 1)
			(setq current-buffer-checkpointable 0)
			(setq current-buffer-journalled 0)
			(setq ~info-file-def (expand-file-name info-default-directory))
			(insert-string ~info-file-def)
			(search-reverse ".")
			(set-mark)
			(end-of-file)
			(erase-region)
			(beginning-of-file)
			(setq ~info-file-def (region-to-string))
			(setq ~info-help-def (expand-file-name "SYS$HELP"))
			(insert-string ~info-help-def)
			(search-reverse ".")
			(set-mark)
			(end-of-file)
			(erase-region)
			(beginning-of-file)
			(setq ~info-help-def (region-to-string))
			(switch-to-buffer "--INFO stack--")
			(setq highlight-region 0)
			(setq mode-line-format "%m")
			(setq mode-string "--Internal INFO buffer--")
			(use-syntax-table "--INFO stack--")
			(modify-syntax-table "word" "!-~")
			(widen-region)
			(erase-buffer)
			(setq case-fold-search 1)
			(setq current-buffer-checkpointable 0)
			(setq current-buffer-journalled 0)
			(switch-to-buffer "--INFO--")
			(setq highlight-region 0)
			(setq mode-line-format "%m")
			(setq mode-string "--Internal INFO buffer--")
			(use-syntax-table "--INFO stack--")
			(widen-region)
			(erase-buffer)
			(setq case-fold-search 1)
			(setq current-buffer-checkpointable 0)
			(setq current-buffer-journalled 0)
			(switch-to-buffer "INFO")
			(setq highlight-region 0)
			(delete-other-windows)
			(setq mode-line-format "")
			(use-syntax-table "INFO")
			(modify-syntax-table "word" "!-~")
			(modify-syntax-table "dull" ",")
			(setq current-buffer-checkpointable 0)
			(setq current-buffer-journalled 0)
			(setq case-fold-search 1)
			(define-keymap "INFO-^R")
			(use-local-map "INFO-^R")
			(autoload "~info-save" "info_save")
			(local-bind-to-key "~info-save" "\^x\^f")
			(local-bind-to-key "exit-emacs" "\^c")
			(local-bind-to-key "exit-emacs" "\^]")
			(define-keymap "INFO")
			(use-local-map "INFO")
			(setq i 0)
			(while (< i 256)
			    (local-bind-to-key "illegal-operation" (char-to-string i))
			    (setq i (+ 1 i))
			)
			(local-bind-to-key "execute-extended-command" "x")
			(local-bind-to-key "execute-extended-command" "\ex")
			(local-bind-to-key "search-forward" "s")
			(local-bind-to-key "search-reverse" "r")
			(local-bind-to-key "search-forward" "\^s")
			(local-bind-to-key "search-reverse" "\^r")
			(local-bind-to-key "redraw-display" "\^l")
			(local-bind-to-key "next-page" " ")
			(local-bind-to-key "previous-page" "\^h")
			(local-bind-to-key "scroll-one-line-down" "a")
			(local-bind-to-key "scroll-one-line-up" "z")
			(local-bind-to-key "beginning-of-file" "b")
			(local-bind-to-key "end-of-file" "e")
			(autoload "~info-o" "info_o")
			(local-bind-to-key "~info-o" "o")
			(autoload "~info-h" "info_h")
			(local-bind-to-key "~info-g" "g")
			(autoload "~info-g" "info_g")
			(local-bind-to-key "~info-h" "h")
			(autoload "~info-hlp" "info_hlp")
			(local-bind-to-key "~info-hlp" "?")
			(autoload "~info-d" "info_d")
			(local-bind-to-key "~info-d" "d")
			(local-bind-to-key "exit-emacs" "q")
			(autoload "~info-m" "info_m")
			(local-bind-to-key "~info-m" "m")
			(autoload "~info-p" "info_p")
			(local-bind-to-key "~info-p" "p")
			(autoload "~info-n" "info_n")
			(local-bind-to-key "~info-n" "n")
			(autoload "~info-u" "info_u")
			(local-bind-to-key "~info-u" "u")
			(autoload "~info-l" "info_l")
			(local-bind-to-key "~info-l" "l")
			(autoload "~info-@" "info_at")
			(local-bind-to-key "~info-@" "@")
			(autoload "~info-^r" "info_ctrl_r")
			(local-bind-to-key "~info-^r" "\^e")
			(autoload "~info-^n" "info_ctrl_n")
			(local-bind-to-key "~info-^n" "\^n")
			(setq ~info-file-part
			    (concat info-default-directory
				info-root-file)
			)
			(setq ~info-node-part "TOP")
			(setq mode-line-format
			    "INFO documentation reader   Node: (%f)%m   --%p--")
			(if (> (nargs) 0)
			    (~info-g (arg 1))
			    (find-node ~info-file-part ~info-node-part 0)
			)
			(setq ~info-is-init 1)
		    )
		    (progn
		        (extend-database-search-list "info" ~info-current-file)
			(switch-to-buffer "INFO")
			(delete-other-windows)
		    )
		)
		(recursive-edit)
	    )
	    (if (is-bound ~INFO-com)
		(if ~INFO-com
		    (exit-emacs)
		    (setq ~INFO-com 0)
		)
	    )
	    (error-occurred (remove-database "info" ""))
	)
	(~info-trim
	    (save-window-excursion
		(switch-to-buffer "Hack buffer")
		(erase-buffer)
		(insert-string (arg 1))
		(beginning-of-file)
		(if (looking-at (quote ~info-file-def))
		    (progn
			(region-around-match 0)
			(erase-region)
		    )
		    (looking-at (quote ~info-help-def))
		    (progn
			(region-around-match 0)
			(erase-region)
		    )
		    (novalue)
		)
		(if (error-occurred (search-forward ";"))
		    (end-of-line)
		    (backward-character)
		)
		(error-occurred (search-reverse ".tlb"))
		(set-mark)
		(beginning-of-file)
		(case-region-upper)
		(region-to-string)
	    )
	)
	(info-check-menu
	    (save-excursion
		(beginning-of-file)
		(if (error-occurred (re-search-forward "^\*[ \t]*menu:"))
		    0
		    (dot)
		)
	    )
	)
	(info-goto-menu-item
	    t
	    dmr
	    (setq t (arg 1))
	    (if (! (setq dmr (info-check-menu)))
		(error-message "This node has no menu.")
	    )
	    (save-window-excursion
		(goto-character dmr)
		(if (error-occurred
			(re-search-forward
			    (concat
				"^\*[ \t]*"
				t
			    )
			)
		    )
		    (error-message "No such menu item: " t)
		    (setq dmr (dot))
		)
	    )
	    (goto-character dmr)
	    (search-forward ":")
	    (if (= (following-char) ':')
		(progn
		    (backward-character)
		    (set-mark)
		    (backward-word)
		    (setq ~info-node-part (region-to-string))
		    (setq ~info-help-part ~info-help-library)
		)
		(progn
		    (forward-word)
		    (backward-word)
		    (setq ~info-node-part "TOP")
		    (get-node-and-file)
		)
	    )
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
	(get-node-and-file
	    (forward-word)
	    (backward-word)
	    (if (= (following-char) '(')
		(progn
		    add-directory
		    (forward-character)
		    (set-mark)
		    (search-forward ")")
		    (backward-character)
		    (exchange-dot-and-mark)
		    (narrow-region)
		    (if (! (looking-at ".*[]:>]"))
			(setq add-directory 1)
			(setq add-directory 0)
		    )
		    (widen-region)
		    (setq ~info-file-part (region-to-string))
		    (if (looking-at "dir)")
			(setq ~info-file-part info-root-file)
		    )
		    (exchange-dot-and-mark)
		    (forward-character)
		    (if (= (substr (expand-file-name ~info-file-part) -2 2) ".;")
			(setq ~info-file-part (concat ~info-file-part ".tlb"))
		    )
		    (save-window-excursion
			(switch-to-buffer "Hack-buffer")
			(erase-buffer)
			(insert-string (expand-file-name ~info-file-part))
			(beginning-of-file)
			(setq ~info-help-part
			    (looking-at ".*\\.hlb;[0-9]*$")
			)
		    )
		    (if add-directory
			(setq ~info-file-part
			    (concat
				(if ~info-help-part
				    "SYS$HELP:"
				    info-default-directory
				)
				~info-file-part
			    )
			)
		    )
		)
	    )
	    (if (looking-at "\\([-a-zA-Z0-9 -+./:-\\{-~_][-a-zA-Z0-9 -+./:-\\{-~_]*\\)[ \t,\n]")
		(progn
		    (region-around-match 1)
		    (setq ~info-node-part (region-to-string))
		)
	    )
	)
	(find-node
	    file
	    node
	    helpp
	    new-file
	    new-node
	    dot
	    (setq new-file 0)
	    (setq dot (+ (dot 0)))
	    (save-window-excursion
		(setq file (arg 1))
		(setq node (arg 2))
		(setq helpp (arg 3))
		(if (!= (length file) 0)
		    (progn
			(setq file (expand-file-name file))
			(if (!= file ~info-current-file)
			    (progn
				(setq new-file 1)
				(extend-database-search-list "info" file)
				(if (!= (length ~info-current-file) 0)
				    (error-occurred
					(remove-database "info" ~info-current-file)
				    )
				)
			    )
			)
		    )
		)
		(if (| new-file (!= ~info-current-node node))
		    (save-window-excursion
			(setq new-node 1)
			(switch-to-buffer "--INFO--")
			(widen-region)
			(erase-buffer)
			(if (error-occurred
				(if helpp
				    (progn
					saved-h
					(setq saved-h (~info-trim ~info-current-file))
					(insert-string
					    (concat
						"Help Node: "
						node
					    )
					)
					(to-col
					    (- 76
						(+
						    (length saved-h)
						    (length ~info-current-node)
						    7
						)
					    )
					)
					(insert-string
					    (concat
						"Up: ("
						saved-h
						")"
						~info-current-node
						"\n"
					    )
					)
					(setq saved-h fetch-help-database-flags)
					(setq fetch-help-database-flags
					    (if (= (substr node -1 1) "*") 
						(progn
						    (setq node (substr node 1 -1))
						    7
						)
						1
					    )
					)
					(fetch-help-database-entry "info" node)
					(setq fetch-help-database-flags saved-h)
				    )
				    (fetch-database-entry "info" node)
				)
			    )
			    (progn
				(if new-file
				    (progn
					(remove-database "info" file)
					(extend-database-search-list "info" ~info-current-file)
				    )
				)
				(error-message node ": " error-message)
			    )
			)
		    )
		    (setq new-node 0)
		)
		(if (&
			(> (length ~info-current-node) 0)
			(> (length ~info-current-file) 0)
		    )
		    (progn
			(switch-to-buffer "--INFO stack--")
			(end-of-file)
			(insert-string
			    (concat
				~info-current-file
				"\n"
				~info-current-node
				"\n"
				dot
				"\n"
				~info-help-library
				"\n"
			    )
			)
		    )
		)
		(if (!= (length file) 0) (setq ~info-current-file file))
		(setq ~info-current-node node)
		(setq ~info-help-library helpp)
	    )
	    (if (| new-node new-file)
		(progn
		    (if new-file
			(setq current-buffer-macro-name (~info-trim file))
		    )
		    (setq mode-string ~info-current-node)
		    (widen-region)
		    (erase-buffer)
		    (yank-buffer "--INFO--")
		)
	    )
	    (beginning-of-file)
	)
    )
)
[info.mlp]
(progn
    (declare-global ~INFO-com)
    (setq ~INFO-com 1)
    (defun
	(info-com
	    (if (! (is-bound ~info-is-init)) (load "info"))
	    (if (> (argc) 1) (info (argv 1)) (info))
	    
	)
    )
)
[info_at.ml]
(defun
	(~info-@
	    path
	    menu-part
	    (save-window-excursion
		(setq path (get-tty-string "Path: "))
		(switch-to-buffer "Hack buffer")
		(erase-buffer)
		(insert-string (concat path " "))
		(beginning-of-file)
		(set-mark)
		(while (! (error-occurred (re-search-forward "  *")))
		    (delete-white-space)
		    (setq menu-part (region-to-string))
		    (erase-region)
		    (set-mark)
		    (save-excursion
			(switch-to-buffer "INFO")
			(info-goto-menu-item menu-part)
		    )
		)
	    )
	)
)
[info_ctrl_n.ml]
(defun
	(~info-^n
	    old-file
	    new-node
	    (setq new-node
		(arg 1 "New node is ")
	    )
	    (save-window-excursion
		(switch-to-buffer "--INFO--")
		(erase-buffer)
		(setq ~info-help-part 0)
		(setq ~info-node-part "TOP")
		(setq ~info-file-part ~info-current-file)
		(insert-string (concat new-node "\n"))
		(beginning-of-file)
		(get-node-and-file)
	    )
	    (setq old-file ~info-current-file)
	    (if ~info-help-part
		(error-message "You cannot insert entries into HELP libraries.")
	    )
	    (setq ~info-file-part (expand-file-name ~info-file-part))
	    (if (!= ~info-current-file ~info-file-part)
		(progn
		    (extend-database-search-list "info" ~info-file-part)
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "INFO edit")
		(setq mode-line-format
		    "[INFO edit   File: %f   Node: %m  --%p%*--]"
		)
		(setq current-buffer-macro-name ~info-file-part)
		(setq mode-string ~info-node-part)
		(use-local-map "INFO-^R")
		(widen-region)
		(erase-buffer)
		(beginning-of-file)
		(setq ~info-^n-file ~info-current-file)
		(setq ~info-^n-node ~info-current-node)
		(setq ~info-^n-help-flag ~info-help-library)
		(setq ~info-current-file ~info-file-part)
		(recursive-edit)
		(switch-to-buffer "INFO edit")
		(while buffer-is-modified
		    (if (c=
			    'y'
			    (string-to-char
				(get-tty-string "You have modified this node. Do you really want to exit? ")
			    )
			)
			(setq buffer-is-modified 0)
			(progn
			    (recursive-edit)
			    (switch-to-buffer "INFO edit")
			)
		    )
		)
	    )
	    (if (!= ~info-current-file old-file)
		(progn
		    (remove-database "info" ~info-current-file)
		)
	    )
	    (setq ~info-current-file old-file)
	    (find-node ~info-^n-file ~info-^n-node ~info-^n-help-flag)
	)
)
[info_ctrl_r.ml]
(defun
	(~info-^r
	    (save-window-excursion
		(switch-to-buffer "INFO edit")
		(text-mode)
		(setq mode-line-format
		    "[INFO edit   File: %f   Node: %m  --%p%*--]"
		)
		(use-local-map "INFO-^R")
		(setq current-buffer-macro-name ~info-current-file)
		(setq mode-string ~info-current-node)
		(widen-region)
		(erase-buffer)
		(if ~info-help-library
		    (fetch-database-entry "info" ~info-current-node)
		    (yank-buffer "INFO")
		)
		(beginning-of-file)
		(setq buffer-is-modified 0)
		(recursive-edit)
		(switch-to-buffer "INFO edit")
		(while buffer-is-modified
		    (if (c=
			    'y'
			    (string-to-char
				(get-tty-string "You have modified this node. Do you really want to exit? ")
			    )
			)
			(setq buffer-is-modified 0)
			(progn
			    (recursive-edit)
			    (switch-to-buffer "INFO edit")
			)
		    )
		)
	    )
	    (setq ~info-node-part ~info-current-node)
	    (setq ~info-current-node "")
	    (find-node ~info-current-file ~info-node-part ~info-help-library)
	)
)
[info_d.ml]
(defun
    (~info-d
	    (find-node
		(concat
		    info-default-directory
		    info-root-file
		)
		"TOP"
		0
	    )
	)
)
[info_g.ml]
(defun
	(~info-g
	    s
	    (setq s (arg 1 "Node: "))
	    (save-window-excursion
		(switch-to-buffer "--INFO--")
		(erase-buffer)
		(setq ~info-file-part (concat info-default-directory info-root-file))
		(setq ~info-node-part "TOP")
		(setq ~info-help-part 0)
		(insert-string (concat s "\n"))
		(beginning-of-file)
		(get-node-and-file)
	    )
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
[info_h.ml]
(defun
	(~info-h
	    (if (error-occurred
		    (find-node
			(concat
			    info-default-directory
			    "EMACSIH.TLB"
			)
			"help"
			0
		    )
		)
		(error-message "Help is not available.")
	    )
	)
)
[info_hlp.ml]
(defun
	(~info-hlp
	    (if (error-occurred
		    (find-node
			(concat
			    info-default-directory
			    info-root-file
			)
			"CMDS"
			0
		    )
		)
		(error-message "The command summary is not available.")
	    )
	)
)
[info_l.ml]
(defun
	(~info-l
	    dot
	    (save-window-excursion
		(switch-to-buffer "--INFO stack--")
		(if (error-occurred
			(end-of-file)
			(backward-character)
			(set-mark)
			(beginning-of-line)
			(setq ~info-help-part (+ (region-to-string) 0))
			(backward-character)
			(set-mark)
			(beginning-of-line)
			(setq dot (+ (region-to-string) 0))
			(backward-character)
			(set-mark)
			(beginning-of-line)
			(setq ~info-node-part (region-to-string))
			(backward-character)
			(set-mark)
			(beginning-of-line)
			(setq ~info-file-part (region-to-string))
			(exchange-dot-and-mark)
		    )
		    (error-message "There is no Last node.")
		)
	    )
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	    (save-excursion
		(temp-use-buffer "--INFO stack--")
		(end-of-file)
		(erase-region)
	    )
	    (goto-character dot)
	    (beginning-of-line)
	)
)
[info_m.ml]
(defun
    (~info-m
	s
	(if (setq s (info-check-menu))
	    (progn
		(save-window-excursion
		    (if (> (+ s 0) (+ 0 (dot)))
			(progn
			    (goto-character s)
			    (if (! (dot-is-visible))
				(error-occurred
				    (line-to-top-of-window)
				    (provide-prefix-argument 3 (scroll-one-line-down))
				)
			    )
			)
		    )
		    (setq s (get-tty-string "Menu item: "))
		)
		(info-goto-menu-item s)
	    )
	    (error-message "This node has no menu.")
	)
    )
)
[info_n.ml]
(defun
	(~info-n
	    (beginning-of-file)
	    (end-of-line)
	    (if (error-occurred (re-search-reverse "Next:[ \t]*"))
		(error-message "This node has no Next.")
	    )
	    (re-search-forward "")
	    (setq ~info-node-part "TOP")
	    (setq ~info-file-part "")
	    (get-node-and-file)
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
[info_o.ml]
(defun
	(~info-o
	    (append-to-file (get-tty-file "Append node to file: "))
	)
)
[info_p.ml]
(defun
	(~info-p
	    (beginning-of-file)
	    (end-of-line)
	    (if (error-occurred (re-search-reverse "Previous:[ \t]*"))
		(error-message "This node has no Previous.")
	    )
	    (re-search-forward "")
	    (setq ~info-node-part "TOP")
	    (setq ~info-file-part "")
	    (get-node-and-file)
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
[info_save.ml]
(defun
	(~info-save
	    (switch-to-buffer "INFO edit")
	    (put-database-entry "info" ~info-node-part)
	    (setq ~info-^n-file ~info-file-part)
	    (setq ~info-^n-node ~info-node-part)
	    (setq ~info-^n-help-flag ~info-help-part)
	    (setq buffer-is-modified 0)
	    (exit-emacs)
	)
)
[info_u.ml]
(defun
	(~info-u
	    (beginning-of-file)
	    (end-of-line)
	    (if (error-occurred (re-search-reverse "Up:[ \t]*"))
		(error-message "This node has no Up.")
	    )
	    (re-search-forward "")
	    (setq ~info-node-part "TOP")
	    (setq ~info-file-part "")
	    (get-node-and-file)
	    (find-node ~info-file-part ~info-node-part ~info-help-part)
	)
)
[jpidef.ml]
; 
; load up $jpidef symbols
; 
(if
    (if (is-bound $jpidef-loaded)
	(! $jpidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "jpi_def")
	(declare-global $jpidef-loaded)
	(setq $jpidef-loaded 1)
    )
)
[jpi_def.ml]
; 
; $jpidef
; 
(progn
    (~sys_literal_setup_incr "jpi$k_" 0 1
	    "other"
	    "network"
	    "batch"
	    "interactive"
    )
    (~sys_literal_setup_incr "jpi$k_" 0 1
	    "detached"
	    ""
	    ""
	    "local"
	    "dialup"
	    "remote"
    )

    (~sys_literal_setup_incr "jpi$_" 768 1
	    "astact"
	    "asten"
	    "pri"
	    "owner"
	    "uic"
	    "sts"
	    "state"
	    "mem"
	    "grp"
	    "prib"
	    "aptcnt"
	    "tmbu"
	    "gpgcnt"
	    "ppgcnt"
	    "astcnt"
	    "biocnt"
	    "biolm"
	    "bytcnt"
	    "diocnt"
	    "diolm"
	    "filcnt"
	    "tqcnt"
	    "efwm"
	    "efcs"
	    "efcu"
	    "pid"
	    "bytlm"
	    "prccnt"
	    "prcnam"
	    "terminal"
	    "jobprccnt"
	    "enqcnt"
	    "enqlm"
	    "swpfilloc"
	    "mode"
	    "jobtype"
	    "proc_index"
	    "master_pid"
    )

    (~sys_literal_setup_incr "jpi$_" 1024 1
	    "curpriv"
	    "wsauth"
	    "wsquota"
	    "dfwscnt"
	    "frep0va"
	    "frep1va"
	    "dfpfc"
	    "cputim"
	    "prclm"
	    "astlm"
	    "pageflts"
	    "dirio"
	    "bufio"
	    "cpulim"
	    "pgflquota"
	    "fillm"
	    "tqlm"
	    "wssize"
	    "authpriv"
	    "imagpriv"
	    "pagfilcnt"
	    "freptecnt"
	    "wsextent"
	    "wsauthext"
	    "authpri"
	    "pagfilloc"
	    "imagecount"
	    "phdflags"
    )

    (~sys_literal_setup_incr "jpi$_" 512 1
	    "virtpeak"
	    "wspeak"
	    "username"
	    "account"
	    "procpriv"
	    "volumes"
	    "logintim"
	    "imagname"
	    "sitespec"
	    "msgmask"
	    "cliname"
	    "tablename"
	    "creprc_flags"
	    "uaf_flags"
	    "maxdetach"
	    "maxjobs"
	    "shrfillm"
    )

    (~sys_literal_setup_incr "jpi$_" 256 1
	    "excvec"
	    "finalexc"
    )
    (~sys_literal_setup_incr "sch$c_" 1 1
	"colpg"
	"mwait"
	"cef"
	"pfw"
	"lef"
	"lefo"
	"hib"
	"hibo"
	"susp"
	"suspo"
	"fpg"
	"com"
	"como"
	"cur"
    )
)
[justify.ml]
(defun
    (justify-paragraph
	(error-occurred
	    (save-excursion
		(beginning-of-line)
		(while (& (! (bobp))
			   (! (eolp))
			   (!= (following-char) '@'))
		    (previous-line))
		(if (| (eolp) (= (following-char) '@'))
		    (next-line))
		(if (& (! (eolp))
			(!= (following-char) '@'))
		    (progn last-col c-col
			(delete-white-space)
			(to-col left-margin)
			(while (progn
				   (end-of-line)
				   (if (! (eobp)) (forward-character))
				   (& (! (eolp))
				       (!= (following-char) '@')))
			    (delete-previous-character)
			    (delete-white-space)
			    (insert-string " ")
			)
			(if (bolp) (backward-character))
			(setq c-col (current-column))
			(while (progn
				   (setq last-col c-col)
				   (insert-character ' ')
				   (delete-previous-character)
				   (beginning-of-line)
				   (if (= (following-char) '@')
				       (insert-character ' '))
				   (end-of-line)
				   (setq c-col (current-column))
				   (< c-col last-col))
			    (novalue))
		    )
		)
	    )
	    (message "Done!")
	    (novalue)
	)
    )
)
[justipara.ml]
; 
; Macro to extract a "paragraph" from the current buffer at dot and re-justify
; it with ragged or smooth right margin.  The effects of the macro are
; reversible.
; 
; Jeffrey Arno Barry, PONE::JBARRY, 231-6756, MRO1-2/E68, 9-Feb-83.
; PONE::DV$PONE___USR1:[JBARRY]JUSTIPARA.ML
;
; The biggest problem faced by this macro is determining the bounds of the 
; current paragraph.  This macro treats each paragraph as a collection of
; lines, the contents of which determine the bounds.  A line has the following
; form:
; 
; <header><words and stuff><linemark>
; 
; The <linemark> can be beginning-or-end-of-buffer, lf, cr-lf, vertical tab, or
; form feed.  Linemarks other than lf or cr-lf ALWAYS mark the beginning or
; end of a paragraph.  Other control characters are ignored except as noted.
; 
; Type The <header> has one of these forms:  Example of usage:
; ---- ------------------------------------  --------------------------------
;  1   <any printing character><white space> "C Fortran comments"
;  2   <punctuation character>		     "!Bliss comments"
;  3   <white space>			     "    Text with some left margin"
;  4   <nil>				     "Text with no left margin"
;
; The header of this paragraph, for example, is of type 1 and is "; ".  When
; you start the macro it looks at the start of the CURRENT line to determine
; what the header should be, so it's important to have the EMACS dot in the
; right place.  Exception: if the current line is indented 4 or more columns
; than the next line AND the headers are of the "same" type (1/2 with matching
; guard character or 3/4), then the header is taken from the next line, the
; current line is considered to be the start of the paragraph, and the extra
; indent will be reflected in the re-justified paragraph.  The header will be
; included in each line of the re-justified paragraph.
; 
; The beginning/end-of-paragraph tests are made AFTER the macro has bypassed
; the header at the beginning of the line.  There needn't be the same header
; on each line of the paragraph.  This macro makes the assumption that you've
; been messing up the paragraph, so it allows lines which have "less" header
; than the model header.  Again, make sure that you start dot on line which
; has the header you want.
; 
; The end of the paragraph is bounded (after header removal) by:
; (1) A blank line or line with less than 3 characters.
; (2) A line containing a tab or 4 or more adjacent spaces.
; (3) A line starting with <optional white space><trailing punctuation>, which
;     includes !.?,;:%>)]} when dealing with header types 3 & 4.
; 
; The beginning of the paragraph is bounded (after header removal) by:
; (a) Numbers 1 & 3 above.
; (b) Number 2 above EXCEPT if the tab or 4 or more spaces is at the beginning
;     of the line, in which case the line marks the beginning of the paragraph
;     and the extra indent and line are included in the paragraph.
; 
; If started on a blank or short line the macro will move to the next line
; and look for the paragraph there.  If it finds another blank line it punts.
; This feature, combined with dot being left at the end of a processed
; paragraph, means that you can process adjacent paragraphs with no extra
; keystrokes.
; 
; If the EMACS prefix-argument is provided then it is used as the right
; margin, otherwise the EMACS variable right-margin is used.  If the result is
; less than 24, 24 is used.  The left margin is determined by the header.  If
; you supply a negative prefix-argument, then you get a smooth right margin at
; the expense of eye-jarring gaps (notice my bias).  The macro uses the same
; definition of right margin as EMACS does, ie if you specify a right margin
; of 80, you get at most 79 columns on the line.
; 
; Now, if  you were  so foolish  as to  specify a<linemark>
; smooth  right  margin, here  is  what you  get.<linemark>
; Lines       which       only       hold       a<linemark>
; Sort-of-Very-Very-Very-Very-LONG-WORD<linemark>
; Very-Very-Very-Very-Very-Very-Very-Very-Very-Very-LONG-WORD<linemark>
; single  word are  left alone.   Tries  hard  to<linemark>
; spread  fill  spaces around  evenly.   Look  at<linemark>
; this  paragraph  as  an  example  of  a  filled<linemark>
; paragraph  with a  right margin  of 50  (ie  49<linemark>
; printing columns).<linemark>
; 
; The characters  .!?   have 2  spaces put  after them,  and :;, get 1 trailing
; space.  If <white space>.?!,:; is seen, then  the white space is deleted.  If
; .?!,:;<not white space> is  seen, the character is ignored.   Multiple spaces
; are reduced to single spaces.  All  other printing characters have no special
; effect.    (This paragraph  is  a more  reasonable  example of  smooth  right
; margin.)
; 
; When the macro terminates, dot and mark surround the new paragraph and the
; old paragraph is in the buffer "Old Paragraph", so you can quickly delete
; the new paragraph and get back the old one if you don't like the results.
; 

(progn	;first progn

; First, some warm-up globals and macros...
(declare-global ~jp-guard)

(defun (~jp-get-header gc (progn	; extract header, assume bol
    (if (looking-at "[!-~][ \t]")	; hdr form <print char><white>?
	(progn
	    (setq gc (following-char))	; guard char val to return
	    (forward-character)		; skip guard char
	    (if (error-occurred (re-search-forward "[^ \t]"))
		(end-of-line)		; not there.. hmm
		(backward-character))	; leave dot at first non ws
	    gc)				; return guard char
	(if (looking-at "[!-/:-@[-`{-~]"); hdr form <punc char>?
	    (progn			; yes
		(forward-character)	; skip guard char
		(preceding-char))	; return guard char
	    (progn			; else <whitespace> or <nil> header
		(if (error-occurred (re-search-forward "[^ \t]")); find non ws
		    (end-of-line)	; not there.. hmm
		    (backward-character)); leave dot at first non ws
		-1)))			; return no guard char
)))

; Bypass header according ot ~jp-guard.  Skip ws unless guard char is
; missing.  Return true if line is boundary.
(defun (~jp-bypass-header  		; bypass header, assume bol
    (if (> ~jp-guard 0)		; is there supposed to be a guard?
	(if (= ~jp-guard (following-char)); yes.. does it match?
	    (progn			; yes
		(forward-character)	; skip it and ws if any
		(if (error-occurred (re-search-forward "[^ \t]"))
		    1			; term if not non-blank
		    (progn
			(backward-character)
			(| (~jp-qt)		; check short/no line
			   (looking-at ".*\t\\|.*    "))))); & tab/4sp embed
	    (| (~jp-qt) ;guard missing.. dangerous.. chk short/no line
	       (looking-at ".*\t\\|.*    "); & tab/4sp embed
	       (looking-at "[ \t]*[]!.?,;:%>)}]"))); & trailing punc & ret test
	(if (error-occurred (re-search-forward "[^ \t]"));no guard.. skip ws
	    1				; term if not non-blank
	    (progn			; else do no guard tests
		(backward-character)
		(| (~jp-qt)		; do short/no line test
		   (looking-at ".*\t\\|.*    "); & tab/4sp embed test
		   (looking-at "[]!.?,;:%>)}]"))))); & trailing punc test
))

; Quicky macro to perform test 1: empty or short line from dot.
(defun (~jp-qt
    (if (eolp)				; empty line test
	1
	(if (looking-at "[ \t][ \t]*$")	; blank line test
	    1
	    (progn			; short line test
		(set-mark)
		(end-of-line)
		(exchange-dot-and-mark)
		(< (- (mark) (dot)) 3))))
))

; OK, now for the real thing...
(defun (justipara rtmrg smooth x s grd ind y col indent bopara eopara (progn

; Ini rtmrg and the smooth flag from right-margin and prefix-argument.
    (setq smooth 0)			; assume ragged right
    (if prefix-argument-provided
	(if (< prefix-argument 0)	; have pref arg
	    (progn
		(setq smooth 1)		; whoops.. smooth right
		(setq rtmrg (- 0 prefix-argument)))
	    (setq rtmrg prefix-argument))
	(setq rtmrg right-margin))	; else no pref arg, use right-margin
    (if (< rtmrg 24) (setq rtmrg 24))	; limit narrowness

; If looking at short or blank line, go to next line so macro can quickly
; process adjacent paragraphs.
    (if (error-occurred (re-search-reverse "\012\\|\013\\|\014")); fancy bol
	(beginning-of-file)
	(forward-character))
    (if (~jp-qt)			; short or blank line?
	(next-line))			; yes, skip it
    (setq x (dot))			; mark start
    (if (|  (= (following-char) 11)
	    (= (following-char) 12)
	    (~jp-qt))		; another blank/short line or ff or vt?
	(error-message "More than 1 blank line before paragraph."))

; Determine what the header is from the current and next lines.
    (setq indent -1)
    (setq bopara -1)
    (setq eopara -1)
    (setq ~jp-guard (~jp-get-header)); get guard char of 1st lin
    (setq col (current-column))		; and its indent too
    (if (! (| (error-occurred
		  (re-search-forward "\012\\|\013\\|\014")); fancy nxt lin
	      (= (preceding-char) 11)
	      (= (preceding-char) 12)	; anything at all wrong with next line?
	      (~jp-bypass-header)))	; no.. see if header of next lin ok
	(if (>= (- col (current-column)) 4); ok.. chk relative indents
	    (progn			; oh goody.. use header from next lin
		(setq indent col)	; set 1st lin indent
		(setq col (current-column)); set general indent
		(setq bopara x))))	; mark beginning of para found
    (goto-character x)			; back to start point

; Now loop backwards until we find the beginning of the paragraph.
    (setq x 200)			; line limit
    (setq y (dot))			; mark initial line start
    (while (< bopara 0)			; loop til we find bo para
	(progn bol
	    (if (<= (setq x (- x 1)) 0) (error-message "Paragraph >200 lines"))
	    (if (|  (bobp)		; if bo buf bound
		    (= (preceding-char) 11); or vt bound
		    (= (preceding-char) 12)); or ff bound
		(setq bopara (dot))	; then have bo para.. exit loop
		(progn			; else gotta do more checks
		    (backward-character); to beg of prev lin
		    (if (error-occurred (re-search-reverse
				   "\012\\|\013\\|\014")); fancy bol
			(beginning-of-file)
			(forward-character))
		    (setq bol (dot))		; mark line st
		    (if (~jp-bypass-header)	; bypass header & chk term
			(progn			; got term string in line
			    (goto-character y)	; nxt lin is 1st lin
			    (setq bopara (dot)))
			(progn ; is there a suff larg ind to qualify as para?
			    (setq y bol); sav lin st in case go round again
			    (if (>= (- (current-column) col) 4)
			    (progn		; yes.. have para begin
				(setq indent (current-column)); save indent
				(setq bopara bol)); mark start
			    (goto-character bol)); else no begin.. go bol
		    ))  ;end of line test if false progn
		))  ;end of if bound false progn
    ))   ;end of backward search
    (goto-character bopara)		; go to bo para
    (message "Beginning of paragraph") (sit-for 5)

; Now loop forwards until we find the end of the paragraph.  While we are
; moving along, transfer the lines minus headers to the "New Paragraph" buffer.
    (set-mark) (copy-region-to-buffer "New Paragraph"); clr new para
    (~jp-bypass-header)		; bypass h
    (setq x 200)			; line limit
    (while (< eopara 0)			; loop til we find eo para
	(progn
	    (if (<= (setq x (- x 1)) 0) (error-message "Paragraph >200 lines"))
	    (set-mark)
	    (if (error-occurred (re-search-forward
				   "\012\\|\013\\|\014")); fancy next-line
		(end-of-file)
		(if (!= (preceding-char) 10)
		    (backward-character)))
	    (setq y (dot))		; mark beg of lin
	    (append-region-to-buffer "New Paragraph"); cop prev lin to n p
	    (if (|  (eobp)		; if eo buf bound
		    (= (following-char) 11); or vt bound
		    (= (following-char) 12)); or ff bound
		(setq eopara (dot))	; then have eo para.. exit loop
		(if (|	(~jp-bypass-header); bypass h and chk
			(>= (- (current-column) col) 4)); chk new para too
		    (setq eopara y)))	; term found.. mark eo para
    ))   ;end of forward search
    (goto-character eopara)	; go to end of para
    (message "End of paragraph.") (sit-for 5)

; OK. Now that all the text is in "New Paragraph", go on over there and
; rejustify it.  This is simple compared to finding the darn paragraph.
    (setq s (current-buffer-name))	; so can get bak
    (temp-use-buffer "New Paragraph")	; go
    (beginning-of-file)

    (set-mark)				; created std header in prefix-string
    (insert-string "\012")
    (if (>= ~jp-guard 0) (insert-character ~jp-guard))
    (to-col col)
    (setq prefix-string (region-to-string))
    (delete-to-killbuffer)

    (error-occurred (replace-string "\015" " ")); CRs ==> spaces
    (error-occurred (replace-string "\012" " ")); LFs ==> spaces
    (error-occurred (replace-string "\011" " ")); tabs (if any) ==> spaces
    (while (! (error-occurred (search-forward "  "))); zap "  "
	(progn (delete-previous-character) (backward-character)))

    (end-of-file)			; " . " ==> ". " and for !?,;: also
    (insert-string " ")
    (beginning-of-file)
    (while (! (error-occurred (re-search-forward
		" \\. \\| ! \\| ? \\| , \\| ; \\| : ")))
	(progn (backward-character) (backward-character)
	       (delete-previous-character)))

    (end-of-file) (delete-white-space)	; ". " ==> ".  " and for !? also
    (beginning-of-file) (delete-white-space)
    (while (! (error-occurred (re-search-forward "\\. \\|! \\|? ")))
	(insert-string " "))		; ! and ? get extra trailing space

    (end-of-file) (insert-string " ")
    (beginning-of-file)			; now insert the 1st line's indent
    (insert-string " \012")
    (if (>= ~jp-guard 0) (insert-character ~jp-guard))
    (if (>= indent 0)
	(to-col indent)
	(to-col col))
    (setq ind (dot))			; mark 1st back search lim

    (setq x 4000)			; word limit
    (while (! (error-occurred (search-forward " "))) (progn; main justify loop
	(if (<= (setq x (- x 1)) 0)	; watch limit
	    (error-message "More than 4000 words in paragraph"))
	(if (> (current-column) rtmrg)	; prev "word" exceed margin?
	    (progn			; yes
		(backward-character)	; leave this sp alone
		(search-reverse " ")	; find prev sp or lin
		(forward-character)
		(if (<= (dot) ind)	; did we hit limit [ie the word
		    (search-forward " ")); is too long for the line]? go eo wd
		(if (! (eobp))	 	; if not at eo buf, break the line
		    (progn
			(delete-previous-character)
			(if (= (preceding-char) 32); zap "  " after .!?
			    (delete-previous-character))
			(insert-string prefix-string)
			(setq ind (dot))	; set new back search lim
			(if (= (following-char) 32)
			    (delete-next-character))))))
    ))  ;eo main jus lp

    (beginning-of-file) (delete-next-character); delete guard characters
    (delete-next-character)
    (end-of-file) (delete-previous-character)
    (message "Paragraph justified.") (sit-for 0)

; Ok, now do the filling if requested.
    (if smooth (progn
	(beginning-of-file)
	(setq x 200)			; lin lim
	(setq ind "                                "); create long blank string
	(setq ind (concat ind ind ind ind ind ind ind ind))))
    (while smooth (progn		; lp til done
	(if (<= (setq x (- x 1)) 0)
	    (error-message "More than 200 lines in paragraph"))

	(beginning-of-line)		; find lh end of line after header
	(if (= ~jp-guard (following-char)); do IFF first char matches guard
	    (forward-character))	; bypass guard char
	(if (error-occurred (re-search-forward "[^ \t]")); find non ws
	    (end-of-line)		; not there.. hmm
	    (backward-character))	; leave dot at first non ws
	(set-mark)			; mark it

	(end-of-line)
	(if (eobp) (progn (setq smooth 0) (set-mark))); watch for last line
	(setq y (- rtmrg (current-column))); number of spaces needed to fill
	(if (> y 0) (progn nhol		; do IFF filling to be done
	    (setq nhol 0)		; lp to count holes on line
	    (while (& (! (error-occurred (search-reverse " ")))
		      (> (dot) (mark)))
		(setq nhol (+ nhol 1)))
	    (goto-character (mark))
	    (if (= nhol 0) (progn	; use eo lin if no holes
			      (end-of-line) (set-mark)))
	    (goto-character (mark))	; go to proper start point on lin
	    (if (eolp)			; filling at end of line?
		(novalue)		; yes.. don't bother
		(progn phf rems mod cnt	; not eol.. calc how to spread damage
		    (setq phf (substr ind 1 (/ y nhol))); create per-hole str
		    (setq rems (% y nhol))	; rem sp after per-hole fills
		    (setq cnt 0)		; lp cnt
		    (if (<= (* rems 2) nhol)	; neater to spread sp or dels?
			(progn			; spread spaces
			    (if (= rems 0)
				(setq mod 100000)
				(setq mod (/ nhol rems)))
			    (while (< cnt nhol) (progn
				(search-forward " ")
				(insert-string phf)
				(if (& (> rems 0)
				       (= 0 (% (+ cnt (/ mod 2)) mod)))
				    (progn
					(setq rems (- rems 1))
					(insert-character ' ')))
				(setq cnt (+ cnt 1)))))
			(progn			; spread deletes
			    (setq phf (concat phf " "))
			    (setq rems (- nhol rems))
			    (setq mod (/ nhol rems))
			    (while (< cnt nhol) (progn
				(search-forward " ")
				(insert-string phf)
				(if (& (> rems 0)
				       (= 0 (% (+ cnt (/ mod 2)) mod)))
				    (progn
					(setq rems (- rems 1))
					(delete-previous-character)))
				(setq cnt (+ cnt 1)))))
		    )  ;end of if spread sp or dels
	    ))  ;end of if eolp false progn
	)) ;end of if y>0
	(next-line)
    ))  ;end of while

; Back to old buffer.  Out with the old and in with the new.
    (use-old-buffer s)
    (goto-character bopara)	; delete (saved) old para
    (set-mark)
    (goto-character eopara)
    (setq x (preceding-char))		; extra lf flag
    (delete-region-to-buffer "Old Paragraph")

    (yank-buffer "New Paragraph")
    (if (= x 10) (insert-string "\012")); insert lf if old para had one
    (sit-for 0) (message "Done.  ^W<esc>^YOld Paragraph<cr> to undo it.")
)))

; end of first progn
)
[killer.key]
;	Autoload and binding for J.Lomicka's kill ring package

(progn
  (declare-global kr-max-buf)
  (setq kr-max-buf 9)	; 9 Sets the number of ring buffers to 10

;	To force the package to autload, load the .KEY file only.  To load the
;	entire package at once, load the .ML file only.  The .ML file will
;	load the .KEY file if it finds kr-max-buf has not been defined.

  (autoload "kr-set-mark" "killer")
  (autoload "kr-toggle-highlight" "killer")
  (autoload "kr-yank" "killer")
  (autoload "kr-kill-forward-word" "killer")
  (autoload "kr-kill-backward-word" "killer")
  (autoload "kr-kill-region" "killer")
  (autoload "kr-kill-forward-character" "killer")
  (autoload "kr-kill-backward-character" "killer")
  (autoload "kr-kill-forward-line" "killer")
  (autoload "kr-kill-backward-line" "killer")
  (autoload "kr-copy-region" "killer")

;	Feel free to re-arrange the bindings, but be sure that you change
;	bindings both here, and in kr-more-bindings!

  (setq highlight-region-graphic-rendition "1;7")
  (bind-to-key "kr-set-mark" "\e.")
  (bind-to-key "kr-toggle-highlight" "\^X.")
  (bind-to-key "kr-set-mark" 0)
  (bind-to-key "kr-yank" "\^Y")
  (bind-to-key "kr-kill-forward-word" "\ed")
  (bind-to-key "kr-kill-backward-word" "\e\177")
  (bind-to-key "kr-kill-backward-word" "\e\^H")
  (bind-to-key "kr-kill-region" "\^W")
;  (bind-to-key "kr-kill-forward-character" "\^D")
;  (bind-to-key "kr-kill-backward-character" "\177")
  (bind-to-key "kr-kill-forward-line" "\^K")
  (bind-to-key "kr-kill-backward-line" "\ek")
  (bind-to-key "kr-copy-region" "\ec")

;	This binds keystrokes that continue deleting when killer is active.

  (defun kr-more-bindings()
    (bind-to-key "kr-arg-prefix" "\^U")
    (bind-to-key "kr-yank" "\^Y")
    (bind-to-key "kr-more-yank" "\ey")
    (bind-to-key "kr-more-forward-word" "\ed")
    (bind-to-key "kr-more-backward-word" "\e\177")
    (bind-to-key "kr-more-backward-word" "\e\^H")
    (bind-to-key "kr-more-region" "\^W")
    (bind-to-key "kr-more-forward-character" "\^D")
    (bind-to-key "kr-more-backward-character" "\177")
    (bind-to-key "kr-more-forward-line" "\^K")
    (bind-to-key "kr-more-backward-line" "\ek")
    (bind-to-key "kr-meta-digit" "\e0")
    (bind-to-key "kr-meta-digit" "\e1")
    (bind-to-key "kr-meta-digit" "\e2")
    (bind-to-key "kr-meta-digit" "\e3")
    (bind-to-key "kr-meta-digit" "\e4")
    (bind-to-key "kr-meta-digit" "\e5")
    (bind-to-key "kr-meta-digit" "\e6")
    (bind-to-key "kr-meta-digit" "\e7")
    (bind-to-key "kr-meta-digit" "\e8")
    (bind-to-key "kr-meta-digit" "\e9")
    (bind-to-key "kr-meta-minus" "\e-")
  )	   
)
[killer.ml]
;	killer.ml	An Emacs kill ring package

;	Requires Emacs V4.1 or later

;	Written  11-Oct-1984 by J.A.Lomicka (REGINA::LOMICKAJ)
;	Inspired by a request from monsur::gaede
;	Last edit 23-July-1986 by J.A.Lomicka

;	This package provides the ability to keep a history of killed
;	objects, and to provide the facility of defining a killed object
;	as any combination of successive kill commands.  The highlighting
;	of the select region is controlled as well, turning highlighting on
;	when mark is set, and off when the region is killed.  These kill-ring
;	commands should be usable within keyboard macros.

;	To load this package, (load "killer") in your emacsinit.ml (either
;	the default, or a modified version).  Be sure killer is in your
;	emacs search path.  If you wish a ring of some other size than 10,
;	you may modify kr-max-buf in KILLER.KEY and set it to some other number
;	before loading killer.

;	If you wish to change the bindings, you should edit killer.KEY only.

;	It works in the following way:

;		set-mark (with highlighting)	M-.
;		toggle-highlighting on or off	^X-.

;	Killing functions:	Default binding:

;		forward-word	M-d
;		backward-word	M-delete
;		kill-region	^W
;		forward-line	^K
;		backward-line	M-k

;	These functions all kill some text, and put the killed text into a
;	new kill buffer.  When any of these is followed by any other of
;	these, or with kill forward-char (^D) or kill backward-char (delete),
;	with no intervening keystrokes, the additional killed text is appended
;	to the correct end of the same buffer.  Both positive and negative
;	prefix arguments may be used.

;	Yank function:

;		yank-killbuffer	^Y
;		yank-more	M-Y (only following ^Y)

;	This function yanks the current kill-buffer, leaving the region
;	around the yanked data.  Successive M-Y following ^Y with no
;	intervening keystrokes will replace the yanked data with the
;	contents of other kill buffers.  If you yanked in the wrong place,
;	^W or (erase-region) will happily get rid of it.  Prefix argument
;	on yank-killbuffer yanks repeatedly, region around the whole set.
;	Prefix argument on yank-more is equivalent to multiple yank-mores,
;	but is faster.

(progn (message "loading killer.ml")(sit-for 0)
;
;	If the .KEY file is not already loaded, load it now.

(if (! (is-bound kr-max-buf)) (load "killer.key"))

(defun kr-toggle-highlight()
  (setq highlight-region (! highlight-region))
)

(defun kr-set-mark()
  (setq highlight-region 1)
  (set-mark)
)

(defun kr-setup()	; Turn on kill ring feature
	key
  (declare-global
    kr-current-buf	; Used to keep track of our place in the ring
    kr-current-thing	; Used to check for submode of killing mode
  )

;	Create the kill buffers

  (setq kr-current-buf 0)
  (save-excursion
    (while (<= kr-current-buf kr-max-buf)
      (temp-use-buffer (concat "-kr" kr-current-buf))
      (setq current-buffer-checkpointable 0)
      (setq current-buffer-journalled 0)
      (setq kr-current-buf (+ 1 kr-current-buf))
    )
  )
  (setq kr-current-buf 0)

;	Set up kill mode bindings

  (define-keymap "kr-inner-keymap")
  (use-global-map "kr-inner-keymap")
  (kr-more-bindings)
  (use-global-map "default-global-keymap")
)

(defun kr-enter()	; Enter killing mode
    kr-imitate-pa;	Prefix argument for subkills
    kr-imitate-pap;	Prefix argument provided for subkills
  (kr-prefix-off)
  (while (process-key (kr-inner-keymap))
    (if kr-imitate-pap (return-prefix-argument kr-imitate-pa))
  )
  (setq highlight-region 0)
  (push-back-character last-keys-struck)
  (if kr-imitate-pap (return-prefix-argument kr-imitate-pa))
)

(defun kr-arg-prefix()
  (setq kr-imitate-pa 4);	Initiate null argument string
  (setq kr-imitate-pap 2);	Set argument provided
  (kr-prefix-on)
)

(defun kr-meta-digit()
  (setq kr-imitate-pa (char-to-string (last-key-struck)))
  (setq kr-imitate-pap 1);	Set argument provided
  (kr-prefix-on)
)

(defun kr-meta-minus()
  (setq kr-imitate-pa "-")
  (setq kr-imitate-pap 1);	Set argument provided
  (kr-prefix-on)
)

(defun kr-digit();	Concat another digit
  (if (= kr-imitate-pap 2)
    (progn; Null prefix argument, override default of 4
      (setq kr-imitate-pap 1)
      (setq kr-imitate-pa "")
    )
  )
  (setq kr-imitate-pa (concat kr-imitate-pa (char-to-string (last-key-struck))))
)

(defun kr-minus();	Negate
  (if (= kr-imitate-pap 2)
    (progn
      (setq kr-imitate-pa "-")
      (setq kr-imitate-pap 1)
    )
  ; else this is an illegal minus
    (progn
      (use-global-map "kr-inner-keymap")
      (remove-binding "-")
      (use-global-map "default-global-keymap")
      (push-back-character "-")
    )
  )
)

(defun kr-prefix-on()
  (use-global-map "kr-inner-keymap")
  (bind-to-key "kr-digit" "0")
  (bind-to-key "kr-digit" "1")
  (bind-to-key "kr-digit" "2")
  (bind-to-key "kr-digit" "3")
  (bind-to-key "kr-digit" "4")
  (bind-to-key "kr-digit" "5")
  (bind-to-key "kr-digit" "6")
  (bind-to-key "kr-digit" "7")
  (bind-to-key "kr-digit" "8")
  (bind-to-key "kr-digit" "9")
  (bind-to-key "kr-minus" "-")
  (use-global-map "default-global-keymap")
)

(defun kr-prefix-off()
  (setq kr-imitate-pa 1)
  (setq kr-imitate-pap 0)
  (use-global-map "kr-inner-keymap")
  (if (= (global-binding-of "0") "kr-digit")
    (progn
      (remove-binding "0")
      (remove-binding "1")
      (remove-binding "2")
      (remove-binding "3")
      (remove-binding "4")
      (remove-binding "5")
      (remove-binding "6")
      (remove-binding "7")
      (remove-binding "8")
      (remove-binding "9")
    )
  )
  (if (= (global-binding-of "0") "kr-minus")
    (remove-binding "-")
  )
  (use-global-map "default-global-keymap")
)

(defun kr-kill-thing( kr-pp kr-pa thing) ; Internal use only, enter killing mode
;	The text in "thing" should be MLisp code that will mark out
;	a region that should start the next kill buffer.
  (setq highlight-region 0)
  (setq kr-current-thing thing)
  (setq kr-current-buf
    (if (>= kr-current-buf kr-max-buf) 0 (+ kr-current-buf 1))
  )
  (save-excursion
    (undo-boundary)
    (execute-mlisp-line (concat "(progn " thing ")"))
    (copy-region-to-buffer (concat "-kr" kr-current-buf))
    (erase-region)
  )
  (kr-enter)
)

(defun kr-more-thing( kr-pp kr-pa thing) ; Interal use only, continue killing
;	The text in "thing" should be MLisp code that will mark out
;	a region that should be appended to the current kill buffer.
;	kr-pp is prefix-argument-provided.
;	kr-pa is prefix-argument.  May be -1 with kr-pp false.
  (kr-prefix-off)
  (if (!= "yanking" kr-current-thing)	; Check submode
    (save-excursion
      (undo-boundary)
      (execute-mlisp-line (concat "(progn " thing ")"))
      (if (>= kr-pa 0)
	(append-region-to-buffer (concat "-kr" kr-current-buf))
	(prepend-region-to-buffer (concat "-kr" kr-current-buf))
      )
      (erase-region)
    )
  ;else change to new killing submode
    (kr-kill-thing kr-pp kr-pa thing)
  )
)

(defun kr-yank()	; Start yanking
  (setq highlight-region 1)
  (setq kr-current-thing "yanking")
  (undo-boundary)
  (set-mark)
  (prefix-argument-loop (yank-buffer (concat "-kr" kr-current-buf)))
  (kr-enter)
)

(defun kr-more-yank()	; Continue yanking
  (kr-prefix-off)
  (if (= "yanking" kr-current-thing)	; Check submode
    (progn	; Yank next thing
      (undo-boundary)
      (erase-region)
      (setq kr-current-buf
	(prefix-argument-loop
	  (if (<= kr-current-buf 0) kr-max-buf (- kr-current-buf 1))
	)
      )
      (error-occurred (yank-buffer (concat "-kr" kr-current-buf)))
    )
  ; else we start yanking
    (kr-yank)
  )
)

(defun kr-line( pp pa)
  (set-mark)
  (if pp
    (progn
      (if (!= pa 0) (beginning-of-line))
      (if (< pa 0) (setq pa (+ pa 1)))
      (provide-prefix-argument pa (next-line))
    )
  ; else no prefix argument, treat terminator seperately
    (if (>= pa 0)
      (if (eolp) (forward-character)(end-of-line))
      (if (bolp) (backward-character)(beginning-of-line))
    )
  )
)

(defun kr-word( pp pa)
  (set-mark)
  (if (>= pa 0)
    (provide-prefix-argument pa (forward-word))

    (provide-prefix-argument (- pa) (backward-word))  )
)

(defun kr-char( pp pa)
  (set-mark)
  (provide-prefix-argument pa (forward-character))
)

(defun kr-kill-forward-word()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-more-forward-word()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-word()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-more-backward-word()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-kill-region() (kr-kill-thing 1 1 ""))
(defun kr-more-region() (kr-more-thing 1 1 ""))

(defun kr-kill-forward-character()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-more-forward-character()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-character()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-more-backward-character()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-kill-forward-line()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-line kr-pp kr-pa)"
  )
)

(defun kr-more-forward-line()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-line kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-line()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-line kr-pp kr-pa)"
  )
)
(defun kr-more-backward-line()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-line kr-pp kr-pa)"
   )
)

(defun kr-copy-region()
  (setq kr-current-buf
    (if (>= kr-current-buf kr-max-buf) 0 (+ kr-current-buf 1))
  )
  (setq highlight-region 0)
  (copy-region-to-buffer (concat "-kr" kr-current-buf))
  (message "Region copied.")
)

(kr-setup)
(message "loaded  killer.ml")
)
[language_support.ml]
(progn
    (declare-global language-loaded)
    (setq language-loaded 1)
    (if (! (is-bound template-loaded))
	(execute-mlisp-file "template_driver")
    )
    (if (! (is-bound users-name))
	(progn
	    (declare-global users-name)
	    (setq users-name "")
	)
    )
    (declare-buffer-specific
	~language-convert-keyword
        ~language-comment-type
	language-keyword-case
    )
    (setq-default ~language-convert-keyword "(progn)")
    (setq-default language-keyword-case "as template")
    (error-occurred
	(extend-database-search-list "language" "emacs$library:emacslang"
	    (if (is-bound library-access)
		library-access
		0
	    )
	)
    )
    (error-occurred
	(extend-database-search-list "language" "sys$login:language-templates"
	    (if (is-bound library-access)
		library-access
		0
	    )
	)
    )
    (defun language-forward-bracket () (forward-paren 0))
    (defun language-backward-bracket () (backward-paren 0))
    (defun language-insert-if-comment (~language-iic-comment "" ~language-iic-nocomment "")
	(insert-string
	    (if (= (current-syntax-entity) is-comment)
		~language-iic-comment
		~language-iic-nocomment
	    )
	)
    )
    (defun language-exit-recursive-edit ()
        (if (recursion-depth) (exit-emacs) (error-message "You are already at top level"))
    )
    (defun language-comment-type ()
	(setq ~language-comment-type (following-char))
    )
    (defun language-check-comment-type ()
        (if (!= ~language-comment-type '\n')
	    (progn
		(delete-white-space)
		(delete-previous-character)
		(if (!= (preceding-char) ' ') (insert-character ' '))
	    )
	)
    )
    (defun language-indent ()
        (to-col (+ (current-column) logical-tab-size))
	(if (= (following-char) ' ') (delete-next-character))
    )
    (defun language-template-driver (language-td-prefix language-td-buf language-td-exit)
	language-td-data
	language-td-mode
	language-saved-exit
	language-td-name
	(setq language-td-data (concat language-td-prefix "-" language-td-buf "-" language-td-prefix))
	(setq language-td-name (concat language-td-prefix "-" language-td-buf))
	(save-window-excursion
	    (if (error-occurred (use-old-buffer language-td-data))
		(progn
		    (switch-to-buffer language-td-data)
		    (setq current-buffer-checkpointable 0)
		    (setq current-buffer-journalled 0)
		    (fetch-database-entry "language" language-td-name)
		    (end-of-file)
		    (if (= (preceding-char) '\n') (delete-previous-character))
		    (setq buffer-is-modified 0)
		    (setq mode-line-format "-%m template-")
		    (setq mode-string "Language")
		)
	    )
	)
	(setq language-td-mode mode-string)
	(setq mode-string (concat language-td-prefix " " language-td-buf))
	(error-occurred (language-set-exit))
	(template-driver language-td-data)
	(error-occurred (language-reset-exit))
	(setq mode-string language-td-mode)
	(error-occurred
	    (if (!= language-exit-point 0)
		(goto-character language-exit-point)
	    )
	)
	(if language-td-exit (exit-emacs))
    )
    (defun language-fetch-mandatory (language-fm-prompt)
	language-fm-count
        (while (&
	  	  (<= (- template-end-field template-start-field) 1)
	  	  (< language-fm-count 2)
	       )
	    (message language-fm-prompt)
	    (recursive-edit)
	    (if (= language-fm-count 0)
		(setq language-fm-prompt
		    (concat
		        "This field is mandatory. "
		        language-fm-prompt
		    )
		)
	    )
	    (setq language-fm-count (+ language-fm-count 1))
        )
        (goto-character template-end-field)
    )
    (defun language-fetch-comment (
	language-fc-prompt ""
	language-fc-mandatory 1
	language-fc-comment "!\t"
	language-fc-cols 2)
	language-fc-left-margin
	language-fc-right-margin
	language-fc-prefix-string
	language-fc-cr
	(setq language-fc-cr (local-binding-of "\r"))
        (local-bind-to-key "language-cr" "\r")
	(setq language-fc-prefix-string prefix-string)
	(setq language-fc-left-margin left-margin)
	(setq language-fc-right-margin right-margin)
	(setq left-margin
	    (save-window-excursion
	        (provide-prefix-argument language-fc-cols (backward-character))
		(current-column)
	    )
	)
	(setq right-margin 76)
	(setq prefix-string language-fc-comment)
	(if language-fc-mandatory
	    (language-fetch-mandatory language-fc-prompt)
	    (language-fetch-optional language-fc-prompt)
	)
	(if (!= language-fc-cr "nothing")
	    (local-bind-to-key language-fc-cr "\r")
	    (remove-local-binding "\r")
	)
	(setq left-margin language-fc-left-margin)
	(setq right-margin language-fc-right-margin)
	(setq prefix-string language-fc-prefix-string)
    )
    (defun language-cr ()
        (newline)
	(to-col left-margin)
	(insert-string prefix-string)
    )
    (defun language-erase-empty-entity (language-eee-lines 0 language-eee-replace "!\tNONE" language-eee-extra 0)
	(if (= (- (dot) language-start-entity) language-eee-lines)
	    (progn
		(save-window-excursion
		    (set-mark)
		    (error-occurred (goto-character language-start-entity))
		    (erase-region)
		    (provide-prefix-argument language-eee-extra (delete-previous-character))
		)
	    (insert-string language-eee-replace)
	    )
	)
    )
    (defun language-optional-entity (language-oe-prompt language-oe-extra 0)
	(message language-oe-prompt)
	(recursive-edit)
	(if (<= (- template-end-field template-start-field) 1)
	    (save-window-excursion
		(goto-character (+ language-oe-extra (- template-end-field 1)))
		(set-mark)
		(error-occurred (goto-character language-start-entity))
		(erase-region)
		(template-exit)
	    )
	)
    )
    (defun language-opt-cont-entity (language-oce-prompt language-oce-extra 0)
	(message language-oce-prompt)
	(recursive-edit)
	(if (<= (- template-end-field template-start-field) 1)
	    (save-window-excursion
		(goto-character (+ language-oce-extra (- template-end-field 1)))
		(set-mark)
		(error-occurred (goto-character language-start-entity))
		(erase-region)
	    )
	)
    )
    (defun
	(language-make-blank-line
	    (if (save-excursion (beginning-of-line) (looking-at "^\\W$"))
	        (if
		    (! (save-excursion
		        (previous-line)
			(beginning-of-line)
			(looking-at "^\\W$")
		    ))
		    (progn
			(end-of-line)
			(newline-and-indent)
		    )
		)
		(progn
		    (end-of-line)
		    (newline-and-indent)
		    (newline-and-indent)
		    (save-excursion
		        (previous-line)
			(delete-white-space)
		    )
		)
	    )
	)
        (language-insert-separator
	    (if (! (provide-prefix-argument -1 (looking-at "\\W\\=")))
	        (insert-string " ")
	    )
	)
        (language-keyword-convert
	    (execute-mlisp-line ~language-convert-keyword)
	)
	(language-set-keyword-case
	    (setq ~language-convert-keyword
		(if (= language-keyword-case "lower") "(case-word-lower)"
		    (= language-keyword-case "upper") "(case-word-upper)"
		    (= language-keyword-case "capital") "(case-word-capitalize)"
		    "(progn)"
		)
	    )
	)
        (insert-users-name
	    (if (<= (length users-name) 0)
	        (setq users-name
		    (concat
		        (get-tty-string "What ar your initials? ")
		        " "
		        (users-login-name)
		    )
		)
	    )
	    (insert-string users-name)
	)
        (VAX/VMS-date
	    EMACS-date
	    (setq EMACS-date (current-time))
	    (insert-string
	        (concat
		    (substr EMACS-date 9 2) "-"
		    (substr EMACS-date 5 3) "-"
		    (substr EMACS-date -4 4)
	        )
	    )
	)
	(language-start-entity
	    (error-occurred
		(setq language-start-entity (dot))
	    )
	)
	(language-fetch-optional
	    language-fetch-optional-del-count
	    (message (arg 1))
	    (recursive-edit)
	    (if (& (= (+ 1 template-start-field) template-end-field) (> (nargs) 1))
		(progn
		    (goto-character template-start-field)
		    (setq language-fetch-optional-del-count (arg 2))
		    (provide-prefix-argument language-fetch-optional-del-count (delete-previous-character))
		)
	    )
	    (goto-character template-end-field)
	)
	(language-exit-dot
	    (error-occurred
		(setq language-exit-point (dot))
	    )
	)
    )
)
[latexmode.key]
(progn
    (setq-default latex-auto-expansion 1) ; Set to zero if you dont want auto-expansion
    (local-bind-to-key "expand-LaTeX-keyword" "\^\")
)
[latexmode.ml]
(progn
    (if (! (is-bound latex-auto-expansion))
        (progn
	    (declare-buffer-specific latex-auto-expansion)
	    (setq-default latex-auto-expansion 1)
	)
    )
    (if (! (is-bound latex-spell-check))
        (progn
	    (declare-buffer-specific latex-spell-check)
	    (setq-default latex-spell-check 1)
	)
    )
    (defun ~LaTeX-auto-fill ()
	(if (!=
		(save-excursion
		    (beginning-of-line)
		    (forward-word)
		    (+ (dot))
		)
		(+ (dot))
	    )
	    (progn
		(backward-word)
		(delete-white-space)
	    )
	)
	(save-excursion (newline) (to-col left-margin))
	(error-occurred (if latex-spell-check (spell-line)))
	(next-line)
	(end-of-line)
    )
    (defun
	(latex-mode
	    (use-syntax-table "LaTeX-mode")
	    (use-abbrev-table "LaTeX-mode")
	    (setq auto-fill-hook "~LaTeX-auto-fill")
	    (setq abbrev-mode latex-auto-expansion)
	    (use-local-map "LaTeX-MAP")
	    (setq mode-string "LaTeX")
	    (setq right-margin 76)
	    (setq left-margin 1)
	    (novalue)
	)
	(~LaTeX-environment-name
	    (setq ~LaTeX-envir "")
	    (language-fetch-mandatory "Insert the environemnt name and use exit-emacs.")
	    (save-window-excursion
		(goto-character template-start-field)
		(set-mark)
		(goto-character template-end-field)(backward-character)
		(setq ~LaTeX-envir (region-to-string))
	    )
	    (goto-character template-end-field)
	)
	(~LaTeX-%
	    language-start-entity
	    language-exit-point
	    (language-template-driver "LaTeX" "%" 0)
	)
	(~LaTeX-abbrev-handler
	    LaTeX-ah-current
	    ~LaTeX-envir
	    (setq LaTeX-ah-current (current-syntax-entity))
	    (if (&
		    (|
			(= LaTeX-ah-current is-word)
			(= LaTeX-ah-current is-dull)
		    )
		    (> (length abbrev-expansion) 0)
		)
		(progn
		    LaTeX-ah-case-fold-search
		    LaTeX-ah-looking-at
		    LaTeX-ah-data
		    language-start-entity
		    LaTeX-last-key-struck
		    language-exit-point
		    (setq LaTeX-ah-case-fold-search case-fold-search)
		    (setq case-fold-search 0)
		    (setq LaTeX-ah-looking-at
			(provide-prefix-argument -1
			    (looking-at (quote abbrev-expansion))
			)
		    )
		    (setq case-fold-search LaTeX-ah-case-fold-search)
		    (if LaTeX-ah-looking-at
			(progn
			    (save-window-excursion
				(region-around-match 0)
				(erase-region)
			    )
			    (setq LaTeX-last-key-struck (last-key-struck))
			    (language-template-driver "LaTeX" abbrev-expansion 1)
			)
		    )
		)
	    )
	    (novalue)
	)
	(~LaTeX-obrack
	    language-start-entity
	    language-exit-point
	    LaTeX-last-key-struck
	    (setq LaTeX-last-key-struck '\r')
	    (if (!= (preceding-char) 92)
		(if (= (current-syntax-entity) is-dull)
		    (language-template-driver "LaTeX" (char-to-string (last-key-struck)) 0)
		    (insert-character (last-key-struck))
		)
		(insert-character (last-key-struck))
	    )
	)
	(~LaTeX-brackets
	    language-start-entity
	    language-exit-point
	    LaTeX-last-key-struck
	    (setq LaTeX-last-key-struck '\r')
	    (if (= (preceding-char) 92)
		(if (= (current-syntax-entity) is-dull)
		    (language-template-driver "LaTeX" (char-to-string (last-key-struck)) 0)
		    (insert-character (last-key-struck))
		)
		(insert-character (last-key-struck))
	    )
	)
	(~LaTeX-insert-optional
	    (if (= (dot) language-start-entity) (insert-string (arg 1)))
	)
	(expand-LaTeX-keyword
	    ~LaTeX-envir
	    LaTeX-ek-case-fold-search
	    LaTeX-ek-looking-at
	    LaTeX-ek-data
	    language-start-entity
	    LaTeX-last-key-struck
	    language-exit-point
	    (setq LaTeX-ek-case-fold-search case-fold-search)
	    (setq case-fold-search 1)
	    (setq LaTeX-last-key-struck '\r')
	    (setq LaTeX-ek-looking-at
		(provide-prefix-argument -1
		    (looking-at "\\w")
		)
	    )
	    (setq case-fold-search LaTeX-ek-case-fold-search)
	    (if LaTeX-ek-looking-at
		(progn
		    (save-window-excursion
			(backward-word)
			(set-mark)
			(forward-word)
			(case-region-lower)
			(setq LaTeX-ek-data (region-to-string))
			(erase-region)
		    )
		    (language-template-driver "LaTeX" LaTeX-ek-data 0)
		)
	    )
	    (novalue)
	)
    )
    (save-window-excursion
	(if (! (is-bound language-loaded))
	    (execute-mlisp-file "language_support")
	)
	(if (! (is-bound paren-flash))
	    (execute-mlisp-file "flasher")
	)
	(switch-to-buffer "LaTeX-mode")
	(define-keymap "LaTeX-MAP")
	(use-local-map "LaTeX-MAP")
	(use-abbrev-table "LaTeX-mode")
	(use-syntax-table "LaTeX-mode")
	(modify-syntax-table "word" "\\*")
	(modify-syntax-table "paren" "(" ")")
	(modify-syntax-table "paren" "[" "]")
	(modify-syntax-table "paren" "{" "}")
	(modify-syntax-table "comment" "%" "\n")
	(local-bind-to-key "~LaTeX-%" "%")
	(local-bind-to-key "~LaTeX-obrack" "$")
	(local-bind-to-key "~LaTeX-obrack" "{")
	(local-bind-to-key "~LaTeX-brackets" "(")
	(local-bind-to-key "~LaTeX-brackets" "[")
	(local-bind-to-key "paren-flash" ")")
	(local-bind-to-key "paren-flash" "]")
	(local-bind-to-key "paren-flash" "}")
	(if latex-spell-check
	    (local-bind-to-key "spell-newline" "\r")
	)
	(error-occurred (execute-mlisp-file "latexmode.key"))
	(define-hooked-local-abbrev "\\begin" "\\begin" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\item" "\\item" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\tag" "\\tag" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\mbox" "\\mbox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\footnote" "\\footnote" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\footnotemark" "\\footnotemark" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\footnotetext" "\\footnotetext" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\part" "\\part" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\chapter" "\\chapter" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\section" "\\section" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subsection" "\\subsection" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subsubsection" "\\subsubsection" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\paragraph" "\\paragraph" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subparagraph" "\\subparagraph" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\part*" "\\part*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\chapter*" "\\chapter*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\section*" "\\section*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subsection*" "\\subsection*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subsubsection*" "\\subsubsection*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\paragraph*" "\\paragraph*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\subparagraph*" "\\subparagraph*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\addcontentsline" "\\addcontentsline" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\addtocontents" "\\addtocontents" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\documentstyle" "\\documentstyle" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\pagestyle" "\\pagestyle" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\markright" "\\markright" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\markboth" "\\markboth" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\twocolumn" "\\twocolumn" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\pagenumbering" "\\pagenumbering" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\title" "\\title" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\author" "\\author" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\date" "\\date" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\thanks" "\\thanks" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\makelabel" "\\makelabel" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\usecounter" "\\usecounter" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\lefteqn" "\\lefteqn" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\frac" "\\frac" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\sqrt" "\\sqrt" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\pmod" "\\pmod" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\overline" "\\overline" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\underline" "\\underline" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\stackrel" "\\stackrel" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newcommand" "\\newcommand" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\renewcommand" "\\renewcommand" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newenvironment" "\\newenvironment" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\renewenvironment" "\\renewenvironment" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newtheorem" "\\newtheorem" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newcounter" "\\newcounter" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\setcounter" "\\setcounter" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\addtocounter" "\\addtocounter" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\value" "\\value" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\caption" "\\caption" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\arabic" "\\arabic" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\roman" "\\roman" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\alpha" "\\alpha" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\fnsymbol" "\\fnsymbol" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\marginpar" "\\marginpar" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\multicolumn" "\\multicolumn" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\cline" "\\cline" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\label" "\\label" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\ref" "\\ref" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\pageref" "\\pageref" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\index" "\\index" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\glossary" "\\glossary" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\linebreak" "\\linebreak" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\nolinebreak" "\\nolinebreak" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\pagebreak" "\\pagebreak" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\nopagebreak" "\\nopagebreak" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\setlength" "\\setlength" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\addtolength" "\\addtolength" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\vspace" "\\vspace" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\vspace*" "\\vspace*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\makebox" "\\makebox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\framebox" "\\framebox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\usebox" "\\usebox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\rule" "\\rule" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\put" "\\put" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\line" "\\line" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\bibliography" "\\bibliography" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\bibitem" "\\bibitem" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\cite" "\\cite" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\nocite" "\\nocite" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\include" "\\include" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\input" "\\input" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\includeonly" "\\includeonly" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\typeout" "\\typeout" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\typein" "\\typein" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\hyphenation" "\\hyphenation" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newlength" "\\newlength" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\stretch" "\\stretch" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\settowidth" "\\settowidth" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\hspace" "\\hspace" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\hspace*" "\\hspace*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\addvspace" "\\addvspace" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\fbox" "\\fbox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\savebox" "\\savebox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\parbox" "\\parbox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\raisebox" "\\raisebox" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\multiput" "\\multiput" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\vector" "\\vector" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\shortstack" "\\shortstack" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\circle" "\\circle" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\circle*" "\\circle*" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\oval" "\\oval" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\frame" "\\frame" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\load" "\\load" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\newfont" "\\newfont" "~LaTeX-abbrev-handler")
	(define-hooked-local-abbrev "\\symbol" "\\symbol" "~LaTeX-abbrev-handler")
    )
    (delete-buffer "LaTeX-mode")
    (novalue)
)
[lispmode.key]
;
;	define keys used in lisp-mode
;
(progn
    (autoload "expand-mlisp-word-in-buffer" "expand-word-in-buffer")
    (autoload "expand-mlisp-variable-in-buffer" "expand-word-in-buffer")
    (local-bind-to-key "Lisp-mode-ESC-map" "\e")
    (local-bind-to-key "Lisp-mode-^X-map" "\^x")
    (local-bind-to-key "expand-mlisp-word-in-buffer" "`")
    (local-bind-to-key "expand-mlisp-variable-in-buffer" "\e`")
    (local-bind-to-key "compile-mlisp-expression" "\ec")
    (local-bind-to-key "compile-mlisp-defun" "\^xc")
    (local-bind-to-key "electric-lisp-semi" ";")
    (local-bind-to-key "paren-pause" ")")
    (local-bind-to-key "forward-sexpr" "\e)")
    (local-bind-to-key "backward-sexpr" "\e(")
    (local-bind-to-key "indent-lisp-function" "\ej")
    (local-bind-to-key "nl-indent" "\^j")
    (local-bind-to-key "re-indent-line" "\ei")
;    (local-bind-to-key "zap-defun" "\^x\^l")
)
[lispmode.ml]
(progn
; 
; 	lisp mode package
; 
; 	updated:
; 	Barry A. Scott	 5-Jun-1983	correct syntax table
; 					add messages each side of a M-c
;	Barry A. Scott	 5-Feb-1986	improve defun-compiling and add
;					electric-lisp-end-comment.
; 
(defun
    (paren-pause dot instabs
	(if (eolp) (delete-white-space))
	(setq instabs (bolp))
	(setq dot (dot))
	(insert-character ')')
	(save-excursion
	    (backward-paren 0)
	    (if instabs
		(save-excursion descol
		    (setq descol (current-column))
		    (goto-character dot)
		    (to-col descol)
		)
	    )
	    (if (dot-is-visible) (sit-for 5))
	)
    )
)
(defun    
    (nl-indent column
	(delete-white-space)
	(save-excursion
	    (backward-balanced-paren-line 0)
	    (setq column
		(if (bolp)
		    (current-indent)
		    (+ (current-column) 4)
		)
	    )
	    (if (< column 5) (setq column 5))
	)
	(newline)
	(to-col column)
    )
)
(defun    
    (re-indent-line
	(save-excursion column
	    (beginning-of-line)
	    (delete-white-space)
	    (save-excursion
		(if (= (following-char) ')')
		    (progn
			(forward-character)
			(backward-paren 0)
			(setq column (current-column))
		    )
		    (progn
			(backward-character)
			(backward-balanced-paren-line 0)
			(setq column
			    (if (bolp)
				(current-indent)
				(+ (current-column) 4)
			    )
			)
			(if (< column 5) (setq column 5))
		    )
		)
	    )
	    (to-col column)
	)
    )
)
(defun
    (indent-lisp-function
	(message "Indenting function...") (sit-for 0)
	(save-excursion
	    (if (error-occurred (end-of-line) (search-reverse "(defun"))
		(error-message "Cannot find function")
	    )
	    (set-mark)
	    ;	    (forward-character)
	    (forward-paren 0)
	    (exchange-dot-and-mark)
	    (delete-white-space)
	    (beginning-of-line)
	    (next-line)
	    (while (& (! (eobp)) (<= (dot) (mark)))
		(re-indent-line)
		(next-line)
	    )
	)
	(message "Indenting function... done.")
    )
)
(defun    
    (electric-lisp-semi
	(beginning-of-line)
	(if (looking-at "[\t ]*$")
	    (progn
		(end-of-line)
		(insert-string "; ")
		(if (!= 3 (current-column))
		    (re-indent-line))
	    )
	    (progn
		(end-of-line)
		(move-to-comment-column)
		(insert-string "; ")
	    )
	)
	(setq left-margin (current-column))
	(setq right-margin 77)
	(setq prefix-string "; ")
    )
)
(defun
    (electric-lisp-end-comment
	(setq prefix-string "")
	(setq left-margin 1)
	(setq right-margin 9999)
    )
)
(defun
    (compile-mlisp-expression
	(message "Compiling MLisp expressions...") (sit-for 0)
	(execute-mlisp-buffer)
	(message "Compiling MLisp expressions... done.")
    )
)
(defun
    (compile-mlisp-defun
	name
	
	(save-window-excursion
	    (if (error-occurred (end-of-line)
		    (search-reverse (concat "(def" "un")))
		(error-message "Cannot find function")
	    )
	    (save-excursion
		(forward-word) (forward-word) (set-mark)
		(backward-word) (setq name (region-to-string))
	    )
	    (set-mark)
	    (forward-paren 0)
	    (copy-region-to-buffer "Lisp-mode-defun-compile")
	)
	(message "Compiling MLisp defun for " name "...") (sit-for 0)
	(save-window-excursion
	    (switch-to-buffer "Lisp-mode-defun-compile")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (beginning-of-file)
	    (execute-mlisp-buffer)
	)
	(message "Compiling MLisp defun for " name "... done.")
    )
)
(defun    
    (forward-sexpr
	;	(search-forward "(")
	(forward-paren 0)
    )
)
(defun    
    (backward-sexpr
	;	(search-reverse ")")
	(backward-paren 0)
    )
)
(defun
    (zap-defun
	(save-excursion
	    (end-of-line)
	    (search-reverse "(def")
	    (set-mark)
	    (forward-paren 0)
	    (end-of-line)
	    (forward-character)
	    (region-to-process "lisp")
	)
	(pop-to-buffer "lisp")
	(end-of-file)
    )
)
(defun    
    (lisp-mode
	(use-local-map "Lisp-mode-map")
	(setq mode-string "Lisp")
	(setq logical-tab-size 4)
	(use-abbrev-table "Lisp-mode")
	(use-syntax-table "Lisp-mode")
	(novalue)
    )
)
;
;	initialisation of lisp mode
;    
(save-excursion
    (temp-use-buffer "keymap-hack")
    ; get a set of maps to use
    (define-keymap "Lisp-mode-map")
    (define-keymap "Lisp-mode-ESC-map")
    (define-keymap "Lisp-mode-^X-map")
    (use-local-map "Lisp-mode-map")
    
    (execute-mlisp-file "lispmode.key")
    
    (use-syntax-table "Lisp-mode")
    (modify-syntax-table "paren" "(" ")")
    (modify-syntax-table "string" "'")
    (modify-syntax-table "string" "\"")
    (modify-syntax-table "comment" ";" "\n")
    (modify-syntax-table "prefix" "\\")
    (modify-syntax-table "word" "-+!$%^&=_~:/?*<>")
)
(novalue)
)
[lk201.ml]
; 
; LK201.ml  - support the LK201 keyboard
; 
(progn
; 
; define the key bindings
; 
(bind-to-key "help" "\(help)")				; Help
(bind-to-key "execute-extended-command" "\(do)")	; Do

(bind-to-key "~lk201-find" "\(find)")			; Find
    (bind-to-key "search-forward" "\(gold)\(find)")	; Gold Find
(bind-to-key "~lk201-insert-here" "\(insert here)")	; Insert Here
(bind-to-key "~lk201-remove" "\(remove)")		; Remove
(bind-to-key "set-mark" "\(select)")			; Select key
(bind-to-key "previous-page" "\(prev screen)")		; Previous Screen
(bind-to-key "next-page" "\(next screen)")		; Next Screen

(bind-to-key "previous-line" "\(up)")			; Up arrow
(bind-to-key "backward-character" "\(left)")		; Left arrow
(bind-to-key "next-line" "\(down)")			; Down arrow
(bind-to-key "forward-character" "\(right)")		; Right arrow

(bind-to-key "~lk201-push-back-ESC" "\(f11)")		; F11
(bind-to-key "beginning-of-line" "\(f12)")		; F12
(bind-to-key "~lk201-push-back-LF" "\(f13)")		; F13
(bind-to-key "toggle-replace-mode" "\(f14)")		; F14
(bind-to-key "visit-file" "\(f17)")			; F17
(bind-to-key "switch-to-buffer" "\(f18)")		; F18
(bind-to-key "delete-other-windows" "\(f19)")		; F19
(bind-to-key "split-current-window" "\(gold)\(f19)")	; PF1-F19

; 
; define the functions that make the keys work
; 
(defun
    (~lk201-find
	(search-forward "")
    )
)
(defun
    (~lk201-insert-here
	(yank-buffer "LK201-remove-buffer")
    )
)
(defun
    (~lk201-remove
	(delete-region-to-buffer "LK201-remove-buffer")
	(unset-mark)
    )
)

; 
; push back an ESC
; 
(defun
    (~lk201-push-back-ESC
	(push-back-character '\e')
	(if prefix-argument-provided (return-prefix-argument prefix-argument))
    )
)
; 
; push back an LF
; 
(defun
    (~lk201-push-back-LF
	(push-back-character '^j')
    )
)
; 
; Toggle the replace mode setting
; 
(defun
    (toggle-replace-mode
	(setq replace-mode (! replace-mode))
    )
)
; 
;  enable the keypad
; 
(defun
    (LK201-on
	(send-string-to-terminal "\e=\(csi)?1h")
    )
)
; 
; disable the keypad
; 
(defun
    (LK201-off
	(if (! terminal-application-keypad)
	    (send-string-to-terminal "\e>\(csi)?1l")
	)
    )
)
)
[lk201_compose_chart]
                        LK201 Compose Key Sequences
                        ----- ------- --- ---------

#  ++         ¨  x0         Â  A^         Ö  O"         ì  i`         é  e'
@  aa         ©  co         Ã  A~         ×  OE         í  i'         ê  e^
[  ((         ©  x0         Ä  A"         Ù  U`         î  i^         ë  e"
\  //         ª  a_         Å  A*         Ú  U'         ï  i"         Ó  O'
\  /<         «  <<         Å  A°         Û  U^         ñ  n~         Ô  O^
]  ))         °   *         Æ  AE         Ü  U"         ò  o`         Õ  O~
{  -(         ¹  1^         Ç  C,         Ý  Y"         ó  o'         ¿  ??
|  /^         ²  2^         È  E`         ß  ss         ô  o^         À  A`
}  )-         ³  3^         É  E'         à  a`         õ  o~         Á  A'
¡  !!         µ  /u         Ê  E^         á  a'         ö  o"         §  s!
¢  c/         ¶  p!         Ë  E"         ã  a~         ÷  oe         §  s0
¢  c|         ·  .^         Ì  I`         ä  a"         ø  o/         §  so
£  l-         ±  +-         Í  I'         å  a*         ù  u`
£  l=         º  o_         Î  I^         å  a°         ú  u'
¥  y-         »  >>         Ï  I"         æ  ae         û  U^
¥  y=         ¼  14         Ñ  N~         ç  c,         ü  u"
¨  xo         ½  12         Ò  O`         è  e`         ý  y"
[lk201_minibuf.ml]
; 
; Minibuffer bindings for LK201
; 
; 
; Here are the bindings of the VT100 keys in the Minibuf
; and the vt200_series bindings.
; 
(save-excursion
    (temp-use-buffer "keypad-hack")
    ; 
    ; Keymap used with spaces as data
    ; 
    (use-local-map "Minibuf-local-map")
    (remove-local-binding "\e")
    (remove-local-binding "\^\")
    (local-bind-to-key "self-insert" "?")		; ?
    (local-bind-to-key "help-and-exit" "\(help)")	; Help
    (local-bind-to-key "expand-and-exit" "\(do)")	; Do
    ; 
    ; Keypad used with spaces as expansion
    ; 
    (use-local-map "Minibuf-local-NS-map")
    (remove-local-binding "\e")
    (remove-local-binding "\^\")
    (local-bind-to-key "self-insert" "?")		; ?
    (local-bind-to-key "help-and-exit" "\(help)")	; Help
    (local-bind-to-key "expand-and-exit" "\(do)")	; Do

    (delete-buffer "keypad-hack")
)
[mail.commands]
	Emacs mail command summary
	--------------------------

	Mail command			Menu	Emacs key
	------------			----	---------
	answer				Yes	a
	back					b
	compress			Yes	C
	copy				Yes	c
	current						not required
	delete					D
	directory			Yes*	d
	dir/folder				lf
	exit					q
	extract				Yes	e
	file				Yes	F
	first					B	back to the top
	forward				Yes	f
	help					h ?
	last					N	next to bottom
	mail				Yes	m w
	move	- as file		Yes	F
	next					n
	print				Yes	p
	purge				Yes	P
	quit					Q
	read					r
	read/new				R
	reply				Yes	a	as answer
	search				Yes	S
	select				Yes	s
	send				Yes	m w
	set				Yes	M	Modify user profile
	show				Yes*	lp	List user profile

	* Menu only for prefix'ed command. "/" as prefix.

	In addition digits 0-9 act as a read command. So to read
	message 23 type "23<RET>"
[mail.key]
; 
; mail.key - key definitions for EMACS VAXmail interface
; 
(save-excursion
    (temp-use-buffer "mail-keymap-hack")
    (define-keymap "mail-keymap")
    (use-local-map "mail-keymap")
    (define-keymap "mail-null-keymap")

(defun
    (~mail-bind-keys
	~argnum ~charnum ~key ~str1 ~str2
	
	(setq ~argnum 1)
	(while (< ~argnum (nargs))
	    (progn
		(setq ~str1 (arg (+ ~argnum 1)))
		(setq ~str2 (arg (+ ~argnum 2)))
		(setq ~charnum 0)
		(while (<= ~charnum (length ~str2))
		    (local-bind-to-key
			(arg ~argnum)
			(concat ~str1 (substr ~str2 ~charnum 1))
		    )
		    (setq ~charnum (+ ~charnum 1))
		)
	    (setq ~argnum (+ ~argnum 3))
	    )
	)
    )
)
    (~mail-bind-keys
	"mail-c-not-a-command"		"" "abcdefghijklmnopqrstuvwxyz"
	"mail-c-not-a-command"		"" "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"mail-c-not-a-command"		"" "0123456789"
	"mail-c-not-a-command"		"" "!@#$%^&*()_+~-=`{}:;'\"\\|"; '"

	"mail-c-answer"			"a" ""

	"mail-c-back-message"		"b" ""

	"mail-c-compress"		"C" ""

	"mail-c-copy-message"		"c" ""

	"mail-c-delete-message"		"D" ""

	"mail-c-directory-messages"	"d" "mM"
	"mail-c-directory-new-messages"	"d" "nN"
	"mail-c-directory-folders"	"d" "fF"

	"mail-c-exit"			"q" ""

	"mail-c-extract"		"e" ""
	"mail-c-file-message"		"" "mF"
	"mail-c-first-message"		"B" ""
	"mail-c-forward"		"f" ""
	"mail-c-help"			"" "hH?"
	"mail-c-last-message"		"N" ""
	"mail-c-send-mail"		"w" ""
	"mail-c-next-message"		"n" ""
	"mail-c-previous-message"	"\^H" ""
	"mail-c-print"			"p" ""
	"mail-c-purge"			"P" ""
	"mail-c-quit"			"Q" ""
	"mail-c-read-message"		"r" ""
	"mail-c-read-new-message"	"R" ""
	"mail-c-search-messages"	"S" ""
	"mail-c-select-messages"	"s" ""
	"mail-c-modify-profile"		"M" "p"
	"mail-c-show-profile"		"l" "p"

	"mail-c-change-view"		"v" ""

	"mail-c-next-item"		"" "]"
	"mail-c-previous-item"		"" "["
	"argument-prefix"		"/" ""
	"mail-c-continue-command"	"\^m" ""
	"mail-c-enter-key"		"\t" ""
    )
    (if mail-keypad-binding
	(~mail-bind-keys
	    "mail-c-enter-key"		"\eOM" ""	; Enter
	)
    )
    (error-occurred (execute-mlisp-file "mail_user.key"))
    (delete-buffer "mail-keymap-hack")
)
[mail.ml]
;
; mail.ml	- VAXmail user interface
;

;
; All lines of code commented "*debug*" should be remove in the final
; version of the mail interface.
;

;
; Declare variable used by mail
;
(declare-global ~mail-debug)
(if (| ~mail-debug (! (is-bound mail-maximum-sessions)))
    (progn
	(declare-global mail-maximum-sessions)
	(setq mail-maximum-sessions 8)
    )
)
;
; The following set of arrays holds information about the active sessions.
; The arrays are index by the session number
;
(if (| ~mail-debug (! (is-bound mail-windows)))
    (progn
	~index

	(declare-global
	    mail-windows
	    mail-last-buffer
	    mail-mode
	    mail-file-is-open
	)
	(setq mail-windows (array 1 mail-maximum-sessions))
	(setq mail-mode (array 1 mail-maximum-sessions))
	(setq ~index 0)
	(while (< ~index mail-maximum-sessions)
	    (setq-array mail-mode (setq ~index (+ 1 ~index)) "")
	)
	(setq mail-last-buffer (array 1 mail-maximum-sessions))
	(setq mail-file-is-open (array 1 mail-maximum-sessions))
    )
)
(declare-buffer-specific
    ~mail-header-mode-line	; what should be on the mode-line
    ~mail-end-of-form		; marker set at the end of the form
    ~mail-lines-read
    ~mail-searching
)
(setq-default ~mail-header-mode-line "[-]\t\t\t\t\t\t\t\t\t     [-]")
(declare-global
    mail-message-reply-to	; -marc - the message being answered to 
    mail-k-reply-separator	; -marc - character(s) to set off a included
				; reply
    mail-number-of-mail-read
    mail-put-form-in-write-buffer
    mail-enter-key-reads-text
    mail-keypad-binding
    mail-current-session
    mail-active-sessions
    mail-users-windows

    mail-form-buffer

    mail-header
    mail-message
    mail-message-details
    mail-message-directory
    mail-folder-directory
    mail-profile
    mail-new-message
)
; 
; load the code of the mail interface
; 
(execute-mlisp-file "mail_routines.ml")
(execute-mlisp-file "mail_commands.ml")
; 
; default variables that may be set by the mail_setup.ml
; customisation module
; 
(setq mail-k-reply-separator "> ")
(setq mail-put-form-in-write-buffer 0)
(setq mail-keypad-binding 1)
(setq mail-enter-key-reads-text 1)
; load user customisation
(error-occurred (execute-mlisp-file "mail_setup.ml"))
; 
; load the key definitions
; 
(execute-mlisp-file "mail.key")


;
; mail
;
; This is the main routine that the user calls.
; It is responsible for setting up the entries in the mail arrays
; and the buffers that are used by a mail session.
; If there is already a session active mail prompts for a session number.
; The default is the first inactive session found in the array.
;
(defun
    (mail
	;
	;   select a session
	;
	(~mail-select-session)

	;
	; Now that we have a session number set up the session
	;
	(~mail-setup-session-variables)
	(if (= "" (fetch-array mail-mode mail-current-session))
	    (progn
		;
		; The session is not active yet so set up all the buffers
		; and all the windows.
		;

		;
		; setup the buffers
		;
		(~mail-setup-buffers)

		;
		; setup the windows
		;
		(setq-array mail-last-buffer mail-current-session
		    mail-message-directory)
		(~mail-setup-windows)
		(setq-array mail-windows mail-current-session
		    current-windows)

		;
		; Open up context that will be required
		;
		(mail-user-begin)
		(~mail-setup-header)

		(setq-array mail-mode mail-current-session "mail")
		(setq mail-active-sessions (+ 1 mail-active-sessions))

		;
		; Start things off with a directory of the notebook
		;
		(mail-c-directory-messages)
		(~mail-setup-header); -marc
		(message "Mail session " mail-current-session " started.")
	    )
	    (progn
		;
		; resume activity where the user left off
		;
		(setq current-windows
		    (fetch-array mail-windows mail-current-session)
		)
		(message "mail session " mail-current-session " resumed.")
	    )
	)
	(novalue)
    )
)


(defun
    (~mail-select-session
	~default-session
	~index
	~comma
	~current-sessions
	
	(if (< mail-active-sessions 0) (setq mail-active-sessions 0))
	(if mail-active-sessions
	    (progn
		
		;
		; pick a session
		;
		(setq ~index 0)
		(setq ~comma "")
		(setq ~current-sessions "")
		(setq ~default-session 0)
		(while (< (setq ~index (+ 1 ~index)) mail-maximum-sessions)
		    (if (!= (fetch-array mail-mode ~index) "")
			(progn
			    (setq ~current-sessions
				(concat ~current-sessions ~comma ~index))
			    (setq ~comma ",")
			    (if (= ~default-session 0)
				(setq ~default-session ~index)
			    )
			)
		    )
		)
		(if ~mail-debug 	; *debug*
		    (progn
			(message "~current-sessions " ~current-sessions)
			(sit-for 10)))
		(if (= ~default-session 0)
		    (setq ~default-session 1)
		)
		(while
		    (progn ~session-number
			(setq ~session-number
			    (get-tty-string
				(concat
				    ": mail select a session, active sessions ("
				    ~current-sessions
				    ") [" ~default-session "] "
				))
			)
			(if (= ~session-number "")
			    (setq ~session-number ~default-session)
			)
			(setq mail-current-session 
			    (setq ~session-number (+ ~session-number)))
			(&
			    (<= ~session-number 0)
			    (>= ~session-number mail-maximum-sessions)
			)
		    )
		    (novalue)	; all the action is in the expr.
		)
		(if (!= (type-of-expression mail-users-windows) "windows")
		    (setq mail-users-windows current-windows)
		)
	    )
	    ;
	    ; else select session one as the first
	    ;
	    (progn
		(setq mail-current-session 1)
		(setq mail-users-windows current-windows)
	    )
	)
    )
)


(defun ~mail-setup-buf
    (
	~name
	~eof
	~mode (concat "\t\t\t - Emacs  VAXmail  interface - \t\t\t")
    )
    (save-excursion
	(temp-use-buffer ~name)
	(erase-buffer)
	(setq mode-line-format ~mode)
	(setq display-C1-as-graphics 1)
	(use-local-map "mail-keymap")
	(if (>= ~eof 0) (setq display-end-of-file ~eof))
	(setq current-buffer-checkpointable 0)
    )
)
(defun
    (~mail-setup-buffers
	(~mail-setup-buf mail-form-buffer -1  "Form buffer %68m")
	(~mail-setup-buf mail-header 0)
	(~mail-setup-buf mail-message-details 1 "Folder %72m")
	(~mail-setup-buf mail-message-directory 1 "Folder %73m")
	(~mail-setup-buf mail-folder-directory 1 "Folder directory %63m")
	(~mail-setup-buf mail-profile -1)
	(~mail-setup-buf mail-message -1 "[-]\t%69m[-]")
	(save-excursion
	    (temp-use-buffer mail-new-message)
	    (setq display-C1-as-graphics 1)
	    (text-mode)
	    (setq mode-line-format  "Mail: new-message %62m")
	    (use-local-map "Text-mode-map")
	)
    )
)


(defun
    (~mail-setup-session-variables
	;
	; setup buffer names
	;
	(setq mail-message-reply-to
	    (concat "mail-message-reply-to-" mail-current-session))
	(setq mail-form-buffer
	    (concat "mail-form-" mail-current-session))
	(setq mail-message
	    (concat "mail-message-" mail-current-session))
	(setq mail-header
	    (concat "mail-header-" mail-current-session))
	(setq mail-message-details
	    (concat "mail-message-details-" mail-current-session))
	(setq mail-message-directory
	    (concat "mail-message-directory-" mail-current-session))
	(setq mail-folder-directory
	    (concat "mail-folder-directory-" mail-current-session))
	(setq mail-profile
	    (concat "mail-profile-" mail-current-session))
	(setq mail-new-message
	    (concat "mail-new-message-" mail-current-session))
	;
	; setup context variables
	;
	(setq mail-user-context mail-current-session)
	(setq mail-message-context mail-current-session)
	(setq mail-mailfile-context mail-current-session)
	(setq mail-send-context mail-current-session)
	(novalue)
    )
)
(defun
    (~mail-setup-windows
	(switch-to-buffer mail-header)
	(delete-other-windows)
	(split-current-window)
	(previous-window)
	(if (! ~mail-debug)	; *debug*
	    (while (> window-size 3) (shrink-window))
	)			; *debug*
	(next-window)
	(switch-to-buffer
	    (fetch-array mail-last-buffer mail-current-session))
    )
)
(defun
    (~mail-setup-header
	(save-excursion
	    (temp-use-buffer mail-header)
	    (erase-buffer)
	    (insert-string
		(concat
		    "Current folder:  " mail-message-folder
		    "   Mail file:  " mail-mailfile-result-spec
		    "\n\t\tType h for help"
		)
	    )
	    (beginning-of-file)
	)
    )
)
(defun ~mail-center-1 (~s)
    ~gap1 ~s-left

    (setq ~s-left (- 80 (length ~s)))
    (if (< ~s-left 2) (setq ~s-left 2))

    (setq ~gap1 (/ ~s-left 2))

    (sys$fao "!#* !AS" "ns" ~gap1 ~s)
)
(defun ~mail-center-2 (~s1 ~s2)
    ~gap

    (setq ~gap (- 80 (length (concat ~s1 ~s2))))
    (if (< ~gap 1) (setq ~gap 1))

    (sys$fao "!AS!#* !AS" "sns" ~s1 ~gap ~s2)
)
(defun ~mail-center-3 (~s1 ~s2 ~s3)
    ~gap1 ~gap2 ~s-left

    (setq ~s-left (- 80 (length (concat ~s1 ~s2 ~s3))))
    (if (< ~s-left 2) (setq ~s-left 2))

    (setq ~gap1 (/ ~s-left 2))
    (setq ~gap2 (- ~s-left ~gap1))

    (sys$fao "!AS!#* !AS!#* !AS" "snsns" ~s1 ~gap1 ~s2 ~gap2 ~s3)
)
(defun
    (~mail-setup-read-message-mode-line
	(setq mode-line-format
	    (sys$fao
		"[-] !16<!AS!>!56<!AS!AS!AS!AS!> [-]" "sssss"
		(if (- mail-message-size ~mail-lines-read)
		    (sys$fao "!SL more line!%S" "n"
			(- mail-message-size ~mail-lines-read)
		    )
		    "End of message"
		)
		(if mail-message-flag-newmsg ", New" "")
		(if mail-message-flag-replied ", Replied" "")
		(if mail-message-flag-marked ", Marked" "")
		(if ~mail-searching ", Searching" "")
	    )
	)
    )
)


;
; The following routines maintain the windows saved in the arrays
;
;  ~keys is either "t" text map
;		  "n" no map
;		  "c" command map
(defun ~mail-change-mode (~mode ~buffer ~keymap ~mode-line)
    
    ~cur-windows
    
    (progn
	(setq-array mail-windows mail-current-session current-windows)
	(setq current-windows (fetch-array mail-windows mail-current-session))
    )
    (switch-to-buffer ~buffer)
    (if
	(= "c" ~keymap) (use-local-map "mail-keymap")
	(= "n" ~keymap) (use-local-map "mail-null-keymap")
	(= "t" ~keymap) (use-local-map "Text-mode-map")
    )
    (setq-array mail-mode mail-current-session ~mode)
    (if (!= "" ~mode-line)
	(progn
	    (if (< (length ~mode-line) 80)
		(setq ~mode-line (sys$fao "!80<!AS!>" "s" ~mode-line)))
	    (save-excursion
		(temp-use-buffer mail-header)
		(setq mode-line-format ~mode-line)
	    )
	    (setq ~mail-header-mode-line ~mode-line)
	)
    )
    (novalue)
)
(defun ~mail-save-mode()
    (if
	(= "mail" (fetch-array mail-mode mail-current-session))
	(setq-array mail-windows mail-current-session current-windows)
    )
    (novalue)
)
(defun ~mail-open-mailfile
    (
	~filename (concat "mail")
    )
    (if (! (fetch-array mail-file-is-open mail-current-session))
	(progn
	    (mail-mailfile-begin)
	    (mail-mailfile-open ~filename)
	    (mail-message-begin)
	)
    )
)


; 
; routines to default an entity
; 
(defun
     (~mail-default-folder
	~name

	(setq ~name "")
	(if (= current-buffer-name mail-marker-directory)
	    (setq ~name (~mail-select-item))
	)
	(if (= "" ~name)
	    (setq ~name mail-marker-name)
	)

	~name
     )
)
(defun
    (~mail-default-message
	~name
	
	(setq ~name "")
	(if (= current-buffer-name mail-message-directory)
	    (setq ~name (~mail-select-item))
	)
	(if (= ~name "") (setq ~name mail-message-current-id))
	
	~name
    )
)
;
; Utility routines
;
(defun
    (~mail-select-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
		(re-search-forward "[\t ]*")
		(set-mark)
		(if
		    (error-occurred
			(re-search-forward "$\\|[nrm ][rm ]")
			(if (! (eolp))
			    (progn
				(backward-character) (backward-character)
			    )
			)
		    )
		    (end-of-line)
		)
		(region-to-string)
	    )
	    ""
	)
    )
)
(defun ~mail-change-state
    (
	~new-state
	~test (+ 0)
    )
    (if ~test (setq ~new-state (! ~new-state)))
    (if (= (fetch-array mail-file-is-open mail-current-session) ~new-state)
	(if ~new-state
	    (error-message "You have already opened a mailfile")
	    (error-message "You have not opened a mailfile yet")
	)
	(if (! ~test)
	    (setq-array mail-file-is-open mail-current-session ~new-state)
	)
    )
)
;
;   Puts up a form in buffer (arg 1) for the user to fill in the mode lines
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
(defun
    (~mail-fill-form
	~argnum ~result ~type ~prompt
	(save-window-excursion
	    (if (& (> window-size 4) (arg 6))
		(progn
		    (split-current-window)
		    (while (! (error-occurred (shrink-window))) (novalue))
		    (while
			(if (> (/ (nargs) 4) window-size)
			    (! (error-occurred (enlarge-window)))
			)
			(novalue)
		    )
		)
	    )
	    (switch-to-buffer (arg 1))
	    (setq mode-line-format (arg 2))
	    
	    ;
	    ; Create the template
	    ;
	    (setq ~argnum 7)
	    (end-of-file)
	    (error-occurred (goto-character ~mail-end-of-form))
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
			    (insert-string
				(concat
				    (if (= "b" (arg (+ 2 ~argnum)))
					(if (arg (+ 1 ~argnum)) "y" "n")
					(arg (+ 1 ~argnum))
				    )
				    "\n"
				)
			    )
			)
		    )
		    (setq ~argnum (+ 4 ~argnum))
		)
	    )
	    ; 
	    ; remember the end of the form
	    ;
	    (setq ~mail-end-of-form (dot))
	    ;
	    ; position at the first field
	    ;
	    (beginning-of-file)
	    (re-search-forward (concat "^" (arg 7) "[\t ]*"))
	    
	    ;
	    ; Let the user fill in the form
	    ;
	    (use-local-map "mail-null-keymap")
	    (save-window-excursion
		(arg 5)		; callers buffer setup actions
		(recursive-edit)
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
				    ;
				    (if (error-occurred (re-search-forward (concat "^" ~prompt "[\t ]*")))
					(error-message "Cannot find prompt " ~prompt " - the form has been corrupted"))
				    ;
				    ; process the type to get the field into ~result
				    ;
				    (if
					(= "s" ~type)
					(progn
					    ;
					    ; string arg
					    ;
					    (set-mark)
					    (end-of-line)
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
					    (set-mark)
					    (end-of-line)
					    (setq ~result (+ (region-to-string)))
					)
					(= "t" ~type)
					(novalue); ignore text items
					(error-message "~mail-fill-form unknow option " ~type)
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
		    (use-local-map "mail-keymap")
		    (unset-mark)
		    (error-message error-message)
		)
	    )
	    (use-local-map "mail-keymap")
	    (unset-mark)
	    ;
	    ;	See if we need to confirm the values filled
	    ;
	    (if (!= "" (setq ~prompt (arg 3)))
		(~mail-confirm-action ~prompt (arg 4))
		1
	    )
	)
    )
)
(defun ~mail-confirm-action
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

(defun ~mail-max (~value1 ~value2)
    (if (> ~value1 ~value2) ~value1 ~value2)
)


;
; end of module
;
[mail_commands.ml]
; 
; mail_commands.ml
; 
(message "mail_commands.ml") (sit-for 10)

;;;  Marc Nozell		
(defun
    (mail-c-answer		; -marc
	~ask-user ~mail-id ~to ~subject
	(setq ~ask-user prefix-argument-provided)
	(setq ~mail-id (~mail-default-message))
	(if (| ~ask-user (= ~mail-id ""))
	    (setq ~mail-id (get-tty-string "Answer mail (n) "))
	)
	(save-window-excursion
	    mail-message

	    (setq mail-message mail-message-reply-to)
	    (~mail-read-message mail$_message_id ~mail-id)
	    (setq ~to mail-message-reply-path)
	    (setq ~subject (concat "Re: " mail-message-subject))
	    (split-current-window)
	    (pop-to-buffer mail-message-reply-to)
	    (~mail-write-buffer 0 ~to ~subject (concat "Reply to #" ~mail-id)
		mail-message-reply-to "")
	    
	)
	(novalue)
    )
)
;;; 
;;; puts the message you are replying to in the message you are writing
;;; and prefixes each line with `mail-k-reply-separator'
;;; 
;;; (anyone got a better name? -marc)
;;; 
(defun	
    (mail-c-copy-reply-in-answer	; -marc
	(progn
	    (temp-use-buffer mail-message-reply-to)
	    (~mail-read-message-continue 1)
	)		; end progn
	(progn
	    (temp-use-buffer mail-new-message)
	    (end-of-file)
	    (yank-buffer mail-message-reply-to)
	    (error-occurred
		(goto-character ~mail-end-of-form)
	    )
	    (error-occurred (re-replace-string "^" mail-k-reply-separator))
	)
    )
)

(defun
    (mail-c-back-message
	(~mail-read-message mail$_message_back "")
	(novalue)
    )
)
(defun
    (mail-c-compress
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-copy-message 	; -marc
	~id ~folder ~file
	(setq ~folder mail-message-folder)
	(setq ~file mail-mailfile-result-spec)
	(setq ~id (~mail-default-message))
	(if
	    (~mail-fill-form
		mail-form-buffer    "File message"
		(concat "File message in folder " ~folder)
		"y"
		(novalue) 1
		; fields to fill
		"Message:" ~id "s" (setq ~id ~result)
		"Folder:" ~folder  "s" (setq ~folder ~result)
		"File:" ~file "s" (setq ~file ~result)
	    )
	    (progn
		(mail-message-copy
		    mail$_message_id		~id
		    ~folder
		    ~file			mail-mailfile-mail-directory
		    0
		)
		(if mail-message-file-created
		    (message "Folder and mailfile created")
		    (if mail-message-folder-created
			(message "Folder created")
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (xmail-c-copy-message
	~id ~folder ~file
	(setq ~folder mail-message-folder)
	(setq ~file mail-mailfile-result-spec)
	(setq ~id (~mail-default-message))
	(if
	    (~mail-fill-form
		mail-form-buffer    "Copy message"
		(concat "Copy message to " ~folder)
		"y"
		(novalue) 1
		; fields to fill
		"Message:" ~id "s" (setq ~id ~result)
		"Folder:" ~folder "" "s" (setq ~folder ~result)
		"File:" ~file "" "s" (setq ~file ~result)
	    )
	    (progn
		(mail-message-copy
		    mail$_message_id		~id
		    ~folder
		    ~file			mail-mailfile-mail-directory
		    0
		)
		(if mail-message-file-created
		    (message "Folder and mailfile created")
		    (if mail-message-folder-created
			(message "Folder created")
		    )
		)
	    )
	)
	(novalue)
    )
)

;;;  Marc Nozell		
(defun
    (mail-c-delete-message	; -marc
	~mail-id ~ask-user
	(setq ~mail-id (~mail-default-message))
	(setq ~ask-user prefix-argument-provided)	
	(if (| ~ask-user (= ~mail-id ""))
	    (progn
		(setq ~mail-id (get-tty-string "Delete mail (n) "))
		(re-search-forward 
		    (concat "[ 	][ 	]*" ~mail-id "[ 	][ 	]*"))
		(beginning-of-line)
	    )
	)
	(mail-message-delete ~mail-id)
	;;; now make the directory reflect the deletion!
	;;; Directory is in the form:
	;;; nnn   node::user    dd-mmm-yyyy title
	;;; becomes...
	;;; nnn   (deleted)     dd-mmm-yyyy title
	(if 
	    (= (current-buffer-name) mail-message-directory)
	    (progn
		(forward-character)
		(re-search-forward "[^ ]")
		(re-search-forward "[ ]")
		(re-search-forward "[^ ]")
		(backward-character)
		(set-mark)		
		(re-search-forward "[ ]")
		(re-search-forward "[^ ]")
		(backward-character)
		(delete-region-to-buffer "*garbage*")
		(set-mark)
		(re-search-forward "\-")		
		(backward-character)
		;;; depending on number of chars in day field, add
		;;; different number of spaces
		(if (= (length (region-to-string)) 2)
		    (progn
			(backward-character)   
			(backward-character)
			(insert-string "(deleted)            ")
		    )
		    (progn
			(backward-character)
			(insert-string "(deleted)             ")
		    )
		)
		(beginning-of-line)
	    )
	)			; end if is in mail-message-directory
	(message (concat "Deleted message #" ~mail-id))
	(novalue)
    )
)

(defun
    (mail-c-directory-messages
	(~mail-open-mailfile)
;	(mail-message-select "mail")	; -marc deleted
	(mail-message-select mail-message-folder)	; -marc added
	(~mail-setup-header)
	(~mail-change-mode "mail" mail-message-directory "c" 
	    "Mail #  Sts From                 Date        Title"
	)
	(erase-buffer)
	(~mail-directory-messages-continue 1)
	(novalue)
    )
)

;;;  Marc Nozell		
(defun
    (mail-c-directory-new-messages ~old-folder	; -marc
	(setq ~old-folder mail-message-folder)
	(if (error-occurred (mail-message-select "newmail"))
	    (progn
		(message (concat "Folder NEWMAIL does not exist"))
		(send-string-to-terminal "\^G")
		(sit-for 0)
		(mail-message-select ~old-folder)
		(~mail-setup-header)
	    )
	    ; else
	    (~mail-setup-header)
	)
	(mail-c-directory-messages)
    )
)


(defun ~mail-directory-messages-continue
    (
	~start
	~num-lines (- window-size 2)
	~start-id (+ mail-message-selected)
    )
    ~item
    (setq ~item (if ~start mail$_message_id mail$_message_back))
    (end-of-file)
    (previous-line)
    (line-to-top-of-window)
    (end-of-line)
    (newline)
    (save-window-excursion
	(while
	    (if
		(> ~num-lines 0)
		(! (error-occurred (mail-message-info ~item ~start-id)))
	    )
	    (progn
		(setq ~item mail$_message_back)
		(insert-string
		    (if mail-message-flag-del
			(sys$fao " !SL  (delete)\n" "n" mail-message-current-id)
			(sys$fao
			    " !5SL!3<!#*n!#*r!#*m!> !20AS !11AS !AS\n" "nnnnsss"
			    mail-message-current-id
			    mail-message-flag-newmsg
			    mail-message-flag-replied
			    mail-message-flag-marked
			    mail-message-sender
			    mail-message-date
			    mail-message-subject
			)
		    )
		)
		(setq ~num-lines (- ~num-lines 1))
		(sit-for 0)
	    )
	)
    )
)
(defun
    (mail-c-directory-folders
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-exit
	(if prefix-argument-provided
	    ;
	    ; Suspend this mail session
	    ;
	    (progn
		(if
		    (=
			"mail"
			(fetch-array mail-mode mail-current-session)
		    )
		    (setq-array mail-windows mail-current-session
			current-windows)
		)
		(setq current-windows mail-users-windows)
		(setq mail-users-windows 0)
		(message "Suspending mail session " mail-current-session "...")
	    )
	    (progn
		(mail-c-end-session)
	    )
	)
	(novalue)
    )
)
(defun
    (mail-c-end-session
	(setq mail-active-sessions (- mail-active-sessions 1))
	(setq-array mail-mode mail-current-session "")
	;
	; close down contexts
	;
	(mail-send-end)
	(mail-user-end)
	(mail-message-end)
	(mail-mailfile-end)
	;
	; get back to orginal windows
	;
	(setq current-windows mail-users-windows)
    )
)
;;;  Marc Nozell		
(defun
    (mail-c-extract		; -marc
	~ask-user
	~mail-id	
	~filename
	; ~ask-user - whether to ask which number to act on
	; ~mail-id - number to act on
	(setq ~ask-user prefix-argument-provided)
	(setq ~filename " ")
	(setq ~mail-id (~mail-default-message))
	(if
	    (|
		~ask-user
		(= ~mail-id "")
	    )
	    (setq ~mail-id (get-tty-string "Extract mail (n) "))
	)
	(save-window-excursion
	    (if 
		(~mail-fill-form
		    mail-form-buffer "Extract Info" 
		    "Extract message"
		    "y"
		    (novalue) 1
		    ; fields to fill
		    "Filename: " ~filename "s" (setq ~filename ~result)
		)
		(progn
		    mail-message

		    (message (concat "Extracting message #" ~mail-id " to " ~filename "..."))  (sit-for 5)
		    (setq mail-message "*garbage*")
		    (progn
			(temp-use-buffer mail-message)
			(erase-buffer)
		    )
		    (~mail-read-message mail$_message_id ~mail-id)
		    (~mail-read-message-continue 1)
		    (if (error-occurred (write-named-file ~filename))
			(progn
			    (send-string-to-terminal "\^G")
			    (message "bad file name!") (sit-for 0)
			)
			(progn	; else
			    (message (concat "Extracting message #" ~mail-id " to " ~filename "... done!"))  (sit-for 1)
			)
		    )
		    (delete-buffer mail-message)
		)
		(novalue)
	    )	    
	)
    )
)
    
    
(defun
    (mail-c-file-message 	; -marc
	~id ~folder ~file
	(setq ~folder mail-message-folder)
	(setq ~file mail-mailfile-result-spec)
	(setq ~id (~mail-default-message))
	(if
	    (~mail-fill-form
		mail-form-buffer    "File message"
		(concat "File message in folder " ~folder)
		"y"
		(novalue) 1
		; fields to fill
		"Message:" ~id "s" (setq ~id ~result)
		"Folder:" ~folder  "s" (setq ~folder ~result)
		"File:" ~file "s" (setq ~file ~result)
	    )
	    (progn
		(mail-message-copy
		    mail$_message_id		~id
		    ~folder
		    ~file			mail-mailfile-mail-directory
		    1
		)
		
		;;; update directory 		; -marc
		(re-search-forward 
		    (concat "[ 	][ 	]*" ~id "[ 	][ 	]*"))
		(beginning-of-line)
		;;; now make the directory reflect the deletion!
		;;; Directory is in the form:
		;;; nnn   node::user    dd-mmm-yyyy title
		;;; becomes...
		;;; nnn   (deleted)     dd-mmm-yyyy title
		(if 
		    (= (current-buffer-name) mail-message-directory)
		    (progn
			(forward-character)
			(re-search-forward "[^ ]")
			(re-search-forward "[ ]")
			(re-search-forward "[^ ]")
			(backward-character)
			(set-mark)		
			(re-search-forward "[ ]")
			(re-search-forward "[^ ]")
			(backward-character)
			(delete-region-to-buffer "*garbage*")
			(set-mark)
			(re-search-forward "\-")		
			(backward-character)
			;;; depending on number of chars in day field, add
			;;; different number of spaces
			(if (= (length (region-to-string)) 2)
			    (progn
				(backward-character)   
				(backward-character)
				(insert-string "(moved)              ")
			    )
			    (progn
				(backward-character)
				(insert-string "(moved)               ")
			    )
			)
			(beginning-of-line)
		    )		
		)
		
		(if mail-message-file-created
		    (message "Folder and mailfile created")
		    (if mail-message-folder-created
			(message "Folder created")
		    )
		)
	    )
	)
	(novalue)
    )
)
    


(defun
    (mail-c-first-message
	(~mail-read-message mail$_message_id "1")
	(novalue)
    )
)

;;; 
;;; %%%-forwarding-line are the lines to insert at the beginning and end
;;; of a forwarded message
;;; hook to do something with forwarding mail
(defun 
    (mail-forwarding-hook	; -marc
	(save-excursion
	    (temp-use-buffer mail-message)
	    (beginning-of-file)
	    (insert-string
		(concat "\n----------------Forwarded mail dated "
		    mail-message-date  " ----------------\n"
		)
	    )
	    (end-of-file)
	    (insert-string "\n----------------End of forwarded mail----------------\n")
	)
    )
)

(defun
    (mail-c-forward		; -marc
	~ask-user
	~mail-id	
	; ~ask-user - whether to ask which number to act on
	; ~mail-id - number to act on
	(setq ~ask-user prefix-argument-provided)
	(setq ~mail-id (~mail-default-message))
	(if
	    (|
		~ask-user
		(= ~mail-id "")
	    )
	    (setq ~mail-id (get-tty-string "Forward mail (n) "))
	)
	(save-window-excursion
	    mail-message

	    (setq mail-message mail-new-message)
	    (~mail-change-mode "mail" mail-new-message "c" 	   "")
	    ; put the message to be forwarded into the mail-new-message buffer
	    (~mail-read-message mail$_message_id ~mail-id)
;	    (~mail-read-message-continue 1)
	    (error-occurred (mail-forwarding-hook))
	    ; add to the forwarded message...
	    (~mail-write-buffer 0 "" "" "Forward mail %m" mail-new-message "")
	)	    
    )
)


(defun
    (mail-c-help
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-last-message
	(~mail-read-message mail$_message_id mail-message-selected)
	(novalue)
    )
)
(defun
    (mail-c-send-mail
	(save-window-excursion
	    (~mail-change-mode "mail" mail-new-message "c" "")
	    (~mail-write-buffer 0 "" "" "Write message %m" "" "")
	)
	(novalue)
    )
)
(defun
    (mail-c-next-message
	(~mail-read-message mail$_message_next "")
	(novalue)
    )
)
(defun
    (mail-c-previous-message	; -marc
	(~mail-read-message mail$_message_back "")
	(novalue)
    )
)

(defun
    (mail-c-print
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-purge
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-quit
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-read-message
	~ask-user ~mail-id
	
	(setq ~ask-user prefix-argument-provided)
	(setq ~mail-id (~mail-default-message))
	(if
	    (|
		~ask-user
		(= ~mail-id "")
	    )
	    (setq ~mail-id (get-tty-string "Read mail (n) "))
	)
	(~mail-read-message mail$_message_id ~mail-id)
	
	(novalue)
    )
)
;;;  Marc Nozell		
(defun
    (mail-c-read-new-message	; -marc
	(progn
	    (mail-message-select "NEWMAIL")
	    (~mail-setup-header)
	    (~mail-read-message mail$_message_id 1)
	    (novalue)
	)
    )
)

(defun
    (mail-c-search-messages
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-select-messages
	~folder
	(if prefix-argument-provided
		(~mail-fill-form
		    mail-form-buffer	"Modify Profile"
		    "Modify profile"
		    "y"
		    (novalue) 1
		    ; fields to fill
		    "Personal name:" mail-user-personal-name "s" (setq mail-user-personal-name ~result)
		    "Mail Directory:" mail-user-sub-directory "s" (setq mail-user-sub-directory ~result)
		    "Copy self on send:" mail-user-copy-send "b" (setq mail-user-copy-send ~result)
		    "Copy self on reply:" mail-user-copy-reply "b" (setq mail-user-copy-reply ~result)
		    "Copy self on forward:" mail-user-copy-forward "b" (setq mail-user-copy-forward ~result)
		    ;;;		"Default transport:" mail-user-transport "s" (setq mail-user-transport ~result)
		    "Auto purge:" mail-user-auto-purge "b" (setq mail-user-auto-purge ~result)
		)
	    (mail-message-select
		(get-tty-string "Select folder (folder name) "))
	)
	(~mail-setup-header)	
	(message mail-message-selected " messages selected")
    )
)
(defun
    (mail-c-modify-profile
	(mail-user-get-info)
	(if
	    (~mail-fill-form
		mail-form-buffer	"Modify Profile"
		"Modify profile"
		"y"
		(novalue) 1
		; fields to fill
		"Personal name:" mail-user-personal-name "s" (setq mail-user-personal-name ~result)
		"Mail Directory:" mail-user-sub-directory "s" (setq mail-user-sub-directory ~result)
		"Copy self on send:" mail-user-copy-send "b" (setq mail-user-copy-send ~result)
		"Copy self on reply:" mail-user-copy-reply "b" (setq mail-user-copy-reply ~result)
		"Copy self on forward:" mail-user-copy-forward "b" (setq mail-user-copy-forward ~result)
		;;;		"Default transport:" mail-user-transport "s" (setq mail-user-transport ~result)
		"Auto purge:" mail-user-auto-purge "b" (setq mail-user-auto-purge ~result)
	    )
	    (progn
		(mail-user-set-info)
		(mail-c-show-profile)
	    )
	)
	(novalue)
    )
)
(defun
    (mail-c-show-profile
	(mail-user-get-info)	; get the latest profile values
	(~mail-change-mode "mail" mail-profile "c" "Profile listing")
	(erase-buffer)
	(setq mode-string "Profile listing")
	(insert-string
	    (sys$fao
		(concat
		    "	New messages:		!SL\n"
		    "	Personal name:		!AS\n"
		    "	Mail directory:		!AS\n"
		    "	Copy self on send:	!AS\n"
		    "	Copy self on reply:	!AS\n"
		    "	Copy self on forward:	!AS\n"
		    ;;;		    "	Default transport:	!AS\n"
		    "	Auto purge:		!AS\n"
		) "nssssss"
		mail-user-new-messages
		mail-user-personal-name
		mail-user-sub-directory
		(if mail-user-copy-send "Yes" "No")
		(if mail-user-copy-reply "Yes" "No")
		(if mail-user-copy-forward "Yes" "No")
		;;;		(if (!= mail-user-transport "") mail-user-transport "None")
		(if mail-user-auto-purge "Yes" "No")
	    )
	)
	(beginning-of-file)
	(novalue)
    )
)
(defun
    (mail-c-change-view
	(if
	    (= current-buffer-name mail-header)
	    (~mail-setup-mailfile-windows)
	    (while
		(progn
		    (if
			(= current-buffer-name mail-message-directory)
			(switch-to-buffer mail-folder-directory)
			(= current-buffer-name mail-folder-directory)
			(switch-to-buffer mail-message-details)
 			(= current-buffer-name mail-message-details)
			(switch-to-buffer mail-profile)
			(= current-buffer-name mail-profile)
			(switch-to-buffer mail-message)
			(= current-buffer-name mail-message)
			(switch-to-buffer mail-message-directory)
			(switch-to-buffer mail-message-directory)
		    )
		    (&
			(= (buffer-size) 0)
			(!= current-buffer-name mail-message-directory)
		    )
		)
		(novalue)
	    )
	)
	(novalue)
    )
)
(defun
    (mail-c-continue-command
	~arg-provided ~arg
	
	(if prefix-argument-provided
	    (progn
		(setq ~arg-provided prefix-argument-provided)
		(setq ~arg prefix-argument)
	    )
	)
	;
	; based on the corrent buffer name continue what the buffer does
	;
	(if
	    (= mail-message-directory current-buffer-name)
	    (~mail-directory-messages-continue 0 ; -marc -fixed typo
		(if ~arg-provided 2000000000 (- window-size 2))
	    )
	    (= mail-message current-buffer-name)
	    (~mail-read-message-continue ~arg-provided ~arg)
	    (error-message "Unable to continue operation in "
		current-buffer-name)
	)
	(novalue)
    )
)
(defun
    (mail-c-enter-key
	(message "NYI") (sit-for 0)
	(novalue)
    )
)
(defun
    (mail-c-not-a-command
	(message "Key " (char-to-string (last-key-struck)) " is not a mail command.")
    )
)
(defun
    (mail-c-next-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(next-line)
	(if (eobp)
	    (previous-line))
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
	    )
	)
    )
)
(defun
    (mail-c-previous-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(previous-line)
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
	    )
	)
    )
)
(defun ~mail-write-buffer (~reply ~to ~subject ~mode-line ~other-window
			      ~personal-name); -marc
    ~cc
    ~first-char
    
    ;
    ;	Get the details for the message to be written, to, subject etc.
    ;
    (setq ~first-char 1)
    (if ~reply (error-occurred (split-current-window))
	(pop-to-buffer ~other-window)
    )
    
    (if
	(if mail-put-form-in-write-buffer
	    (~mail-fill-form
		mail-new-message	~mode-line
		"Send message"
		"y"
		(progn		; custom setup option
		    (use-local-map "Text-mode-map")
		    (error-occurred (mail-hook-setup-compose-buffer))
		)
		0
		; fields to fill
		"To:" ~to "s" (setq ~to ~result)
		"Subject:" ~subject "s" (setq ~subject ~result)
		"CC:" "" "s" (setq ~cc ~result)
		"Personal Name: " ~personal-name "s" (setq ~personal-name ~result); -marc
		"‘‘‘‘‘ Use erase-mail-text to empty buffer, use exit-emacs to write mail ‘‘‘‘‘‘‘‘\n"
		"" "t"
		(progn (forward-character) (setq ~first-char (dot)))
	    )
	    (progn
		(switch-to-buffer mail-new-message)
		(use-local-map "Text-mode-map")
		(error-occurred (mail-hook-setup-compose-buffer))
		
		(setq mode-line-format ~mode-line)
		(recursive-edit)
		
		(~mail-fill-form
		    mail-form-buffer	""
		    "Send message"
		    "y"
		    (novalue) 0
		    ; fields to fill
		    "To:" ~to "s" (setq ~to ~result)
		    "Subject:" ~subject "s" (setq ~subject ~result)
		    "CC:" "" "s" (setq ~cc ~result)
		    "Personal Name: " ~personal-name "s" (setq ~personal-name ~result); -marc
		)
	    )
	)
	(progn
	    (switch-to-buffer mail-new-message)
	    (goto-character ~first-char)
	    (set-mark)
	    (end-of-file)
	    (narrow-region)
	    (if (= ~personal-name ""); -marc
		(mail-send-begin mail-user-personal-name)
		(mail-send-begin ~personal-name)
	    )
	    (~mail-add-addresses mail$_to ~to)
	    (if (!= ~cc "")
		(~mail-add-addresses mail$_cc ~cc)
	    )
	    (mail-send-add-attribute ~subject ~to ~cc)
	    (mail-send-add-body)
	    (mail-send-message)
	    (mail-send-end)
	    (widen-region)
	)
    )
)
(defun ~mail-add-addresses
    (
	~type
	~address-list
    )
    (save-excursion
	(temp-use-buffer "mail-temp")
	(erase-buffer)
	(insert-string ~address-list)
	(beginning-of-file)
	(error-occurred (save-excursion (re-replace-string "," "\n")))
	(error-occurred (save-excursion (re-replace-string "^[\t ][\t ]*" "")))
	(error-occurred (save-excursion (re-replace-string "[\t ][\t ]*$" "")))
	(while
	    (! (error-occurred (re-search-forward "^@")))
	    (progn
		(error-message "@ not supported yet")
	    )
	)
	(beginning-of-file)
	(while
	    (! (eobp))
	    (progn
		(beginning-of-line) (set-mark) (end-of-line)
		(mail-send-add-address (region-to-string) ~type)
		(error-occurred (forward-character))
	    )
	)
    )
)
(defun
    (erase-mail-text
	(save-excursion
	    (end-of-file)
	    (set-mark)
	    (error-occurred (goto-character ~mail-end-of-form))
	    (erase-region)
	)
    )
)
(defun ~mail-read-message (~item ~value)
    ~mode
    ;
    ; select the message of interest
    ;
    (save-excursion
	(temp-use-buffer mail-message)
	(mail-message-get ~item ~value)
	(setq ~mode
	    (sys$fao "Message !4SL  !AS  !17AS !SL lines" "nssn"
		mail-message-current-id
		mail-message-sender
		mail-message-date
		mail-message-size
	    )
	)
    )
    (~mail-change-mode "mail" mail-message "c" ~mode)
    (setq ~mail-lines-read 0)
    (erase-buffer)
    ;
    ; read the body of the message
    ;
    (~mail-read-message-continue 0)
    (novalue)
)
(defun ~mail-read-message-continue (~read-the-lot)
    (if ~read-the-lot
	(mail-message-read
	    2000000
	    (if ~mail-searching mail-search-context mail-message-context)
	)
	(progn
	    (end-of-file)
	    (error-occurred
		(previous-line) (previous-line)
	    )
	    (line-to-top-of-window)
	    (mail-message-read
		(- window-size 3)
		(if ~mail-searching mail-search-context mail-message-context)
	    )
	    (end-of-file)
	    (provide-prefix-argument (- window-size 1) (previous-line))
	    (line-to-top-of-window)
	)
    )
    (~mail-setup-read-message-mode-line)
)
[mail_def.ml]
; 
; mail_def.ml
; 
(message "mail_def.ml") (sit-for 10)
(~sys_literal_setup_incr "mail$" 1 1
    "mailfile_begin"
    "mailfile_close"
    "mailfile_compress"
    "mailfile_end"
    "mailfile_info_file"
    "mailfile_modify"
    "mailfile_open"
    "mailfile_purge_waste"
    "message_begin"
    "message_copy"
    "message_delete"
    "message_end"
    "message_get"
    "message_info"
    "message_modify"
    "message_select"
    "send_abort"
    "send_add_address"
    "send_add_attribute"
    "send_add_bodypart"
    "send_begin"
    "send_end"
    "send_message"
    "user_begin"
    "user_delete_info"
    "user_end"
    "user_get_info"
    "user_set_info"
)
(~sys_literal_setup_incr "mail$_" 0 1 "noop")
(~sys_literal_setup_incr "mail$_send_" 1 1
	"spare_0"
	"foreign"
	"cc_line"
	"default_name"
	"default_transport"
	"error_entry"
	"filename"
	"from_line"
	"no_default_transport"
	"pers_name"
	"record"
	"resultspec"
	"server"
	"subject"
	"success_entry"
	"to_line"
	"uflags"
	"user_data"
	"username"
	"username_type"
	"fid"
	"no_pers_name"
	"in_spare3"
	"in_spare4"
	"in_spare5"
; 
; Send output parameters
;
	"copy_reply"
	"copy_send"
	"user"
	"copy_forward"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
)
(~sys_literal_setup_incr "mail$_mailfile_" 1025 1
 	"spare_0"
	"default_name"
	"folder_routine"
	"full_close"
	"name"
	"reclaim"
	"user_data"
	"wastebasket_name"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; file output parameters
;
	"data_reclaim"
	"data_scan"
	"deleted_bytes"
	"index_reclaim"
	"mail_directory"
	"messages_deleted"
	"resultspec"
	"total_reclaim"
	"wastebasket"
	"indexed"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
	"out_spare6"
	"out_spare7"
	"out_spare8"
	"out_spare9"
	"out_spare10"
	"out_spare11"
	"out_spare12"
	"out_spare13"
	"out_spare14"
	"out_spare15"
	"out_spare16"
	"out_spare17"
	"out_spare18"
	"out_spare19"
	"out_spare20"
)

;
; message input parameters
;

(~sys_literal_setup_incr "mail$_message_" 2048 1
	"spare_0"
	"back"
	"before"
	"cc_substring"
	"continue"
	"file_action"
	"folder_action"
	"default_name"
	"delete"
	"erase"
	"file_ctx"
	"filename"
	"flags"
	"folder"
	"from_substring"
	"id"
	"next"
	"since"
	"subj_substring"
	"to_substring"
	"uflags"
	"auto_newmail"
	"user_data"
	"flags_mbz"
	"min_class"
	"max_class"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; message output parameters
;
	"cc"
	"current_id"
	"date"
	"extid"
	"file_created"
	"folder_created"
	"from"
	"record"
	"record_type"
	"reply_path"
	"resultspec"
	"return_flags"
	"return_uflags"
	"selected"
	"sender"
	"size"
	"subject"
	"to"
	"buffer"
	"return_class"
	"binary_date"
	"spare4"
	"spare5"
	"spare6"
	"spare7"
	"spare8"
	"spare9"
	"spare10"
	"spare11"
	"spare12"
	"spare13"
	"spare14"
	"spare15"
	"spare16"
	"spare17"
	"spare18"
	"spare19"
;
; Constants returned
;
	"null"
	"header"
	"text"
	"spare20"
)

;
; user input parameters
;

(~sys_literal_setup_incr "mail$_user_" 3072 1
	"spare_0"
	"first"
	"next"
	"username"
	"set_auto_purge"
	"set_no_auto_purge"
	"set_sub_directory"
	"set_no_sub_directory"
	"set_forwarding"
	"set_no_forwarding"
	"set_personal_name"
	"set_no_personal_name"
	"set_copy_send"
	"set_no_copy_send"
	"set_copy_reply"
	"set_no_copy_reply"
	"set_new_messages"
	"create_if"
	"set_mailplus"
	"set_no_mailplus"
	"set_transport"
	"set_no_transport"
	"set_editor"
	"set_no_editor"
	"set_queue"
	"set_no_queue"
	"set_user1"
	"set_no_user1"
	"set_user2"
	"set_no_user2"
	"set_user3"
	"set_no_user3"
	"set_form"
	"set_no_form"
	"set_copy_forward"
	"set_no_copy_forward"
	"set_cc_prompt"
	"set_no_cc_prompt"
	"set_spare3"
	"set_no_spare3"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; Output item parameters
;
	"mailplus"
	"transport"
	"editor"
	"queue"
	"user1"
	"user2"
	"user3"
	"form"
	"copy_forward"
	"spare3"
	"return_username"
	"auto_purge"
	"sub_directory"
	"full_directory"
	"new_messages"
	"forwarding"
	"personal_name"
	"copy_send"
	"copy_reply"
	"captive"
	"cc_prompt"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
	"out_spare6"
	"out_spare7"
	"out_spare8"
	"out_spare9"
	"out_spare10"
	"out_spare11"
	"out_spare12"
	"out_spare13"
	"out_spare14"
	"out_spare15"
	"out_spare16"
	"out_spare17"
	"out_spare18"
	"out_spare19"
	"out_spare20"
)
(~sys_literal_setup_incr "mail$_" 1 1
	"to"
	"cc"
	"spare1"
	"spare2"
)
(~sys_literal_setup_equal "mail$_" 
    "coderr"	8290316
    "upgrade"	8290324
    "uafgeterr"	8290330
    "uafopnerr"	8290338
    "nosuchusr"	8290346
    "notreadin"	8290354
    "loglink"	8290362
    "creprijob"	8290370
    "senderr"	8290378
    "nomoremsg"	8290386
    "nomsgs"	8290394
    "userspec"	8290402
    "sendabort"	8290410
    "editproc"	8290418
    "invcmd"	8290426
    "notfound"	8290434
    "netioerr"	8290442
    "userdsabl"	8290450
    "forwloop"	8290458
    "namtoobig"	8290466
    "illchar"	8290474
    "datimused"	8290482
    "notisam"	8290490
    "delmsg"	8290498
    "delwaste"	8290506
    "nofolder"	8290514
    "notexist"	8290522
    "notsubdir"	8290530
    "illsubdir"	8290538
    "illfolnam"	8290546
    "nomsgpri"	8290554
    "altprijob"	8290562
    "illpernam"	8290570
    "nosuchusrat"	8290578
    "senderrat"	8290586
    "conabort"	8290594
    "notcaledt"	8290602
    "fileconfused"	8290610
    "captive"	8290618
    "ivprotval"	8290626
    "rectobig"	8290634
    "illctxadr"	8290642
    "wrongctx"	8290650
    "failgetvm"	8290658
    "invitmcod"	8290666
    "misreqitm"	8290674
    "invitmlen"	8290682
    "fileopen"	8290690
    "nofileopen"	8290698
    "conitmcod"	8290706
    "nomorectx"	8290714
    "wrongfile"	8290722
    "illtprt"	8290730
    "invitmval"	8290738
    "eracted"	8290746
    "formsg"	8290754
    "notformsg"	8290762
    "erractrns"	8290770
    "badvalue"	8290778
    "closedel"	8290786
    "filnotdel"	8290794
    "openin"	8290802
    "openout"	8290810
    "parsefail"	8290818
    "readerr"	8290826
    "searchfail"	8290834
    "syserror"	8290842
    "nosysnam"	8290850
    "nosysprv"	8290858
    "protocol"	8290866
    "selected"	8306307
    "delmsgs"	8306315
    "cvtfile"	8306323
    "delspace"	8306331
    "newfolder"	8306339
    "reclplswait"	8306347
    "reenter"	8306355
    "moremsg"	8314304
    "dirhd0"	8314312
    "dirhd10"	8314320
    "dirhd20"	8314328
    "dirdel10"	8314336
    "isub10"	8314344
    "isub20"	8314352
    "isub30"	8314360
    "presret"	8314368
    "youhavenew"	8314376
    "mail10"	8314384
    "mail20"	8314392
    "wantosend"	8314400
    "notify10"	8314408
    "notify20"	8314416
    "jobentered"	8314424
    "show10"	8314432
    "show20"	8314440
    "show30"	8314448
    "show40"	8314456
    "show50"	8314464
    "show60"	8314472
    "show70"	8314480
    "show80"	8314488
    "show90"	8314496
    "show100"	8314504
    "show110"	8314512
    "edit10"	8314520
    "info10"	8314528
    "info20"	8314536
    "info30"	8314544
    "send"	8314552
    "reply"	8314560
    "show120"	8314568
    "show130"	8314576
    "info50"	8314584
    "show140"	8314592
    "show150"	8314600
    "show160"	8314608
    "isub50"	8314616
    "mail30"	8314624
    "retcont"	8314632
    "newseq"	8314640
    "maildir"	8314648
    "mailfile"	8314656
    "mailfolder"	8314664
    "mail40"	8314672
    "crenewfold"	8314680
    "nonewmail"	8314688
    "showkey10"	8314696
    "showkey20"	8314704
    "showkey30"	8314712
    "showkey40"	8314720
    "showkey50"	8314728
    "showkey60"	8314736
    "showkey70"	8314744
    "notify30"	8314752
    "notify40"	8314760
    "noparent"	8314768
    "filempty"	8314776
    "nonefound"	8314784
    "nomorerec"	8314792
    "userfwd"	8314800
    "prompt"	8314808
    "info12"	8314816
    "info14"	8314824
    "info16"	8314832
    "info18"	8314840
    "crenewfile"	8314848
    "show15"	8314856
    "show25"	8314864
    "show35"	8314872
    "forward"	8314880
    "show200"	8314888
    "show205"	8314896
    "show210"	8314904
    "show215"	8314912
    "closein"	8314920
    "closeout"	8314928
    "invquaval"	8314936
    "writeerr"	8314944
    "msginfo"	8322313
    "msgtext"	8322305
    "msgsent"	8322321
    "msgsentat"	8322329
    "yes"	8322337
    "copiedr"	8322345
    "created"	8322353
    "renamed"	8322361
    "text"	8322369
)
(~sys_literal_setup_equal "mail$m_"
    "newmsg"	1
    "replied"	2
    "del"	4
    "extmsg"	8
    "extfnf"	16
    "sysmsg"	32
    "extnstd"	64
    "marked"	128
    "recmode"	256
)
(external-function "~mail" "emacs$share:emacs_mail_shr" "emacs$mail")
[mail_routines.ml]
;
; mail_routines.ml
;
(message "mail_routines.ml") (sit-for 10)
(execute-mlisp-file "mail_def")
;
; global variables
;
(declare-global
    mail-status

    ;
    ; The following hold the default contexts for the various operations
    ;
    mail-send-context
    mail-mailfile-context
    mail-message-context
    mail-user-context
)
;
; send routines
;
(declare-global
    mail-send-copy-forward
    mail-send-copy-send
    mail-send-copy-reply
    mail-send-user
)
(defun mail-send-begin
    (
	~personal-name (concat "")
	~default-transport (concat "")
	~ctx (+ mail-send-context)
    )
    (~mail mail$send_end ~ctx 0)
    (setq mail-status
	(~mail mail$send_begin ~ctx 2
	    (if (= ~personal-name "")
		mail$_send_no_pers_name mail$_send_pers_name
	    )		~personal-name
	    (if (= ~default-transport "")
		mail$_send_no_default_transport mail$_send_default_transport
	    )		~default-transport
	    ; output items
	    mail$_send_copy_forward	mail-send-copy-forward
	    mail$_send_copy_send	mail-send-copy-send
	    mail$_send_copy_reply	mail-send-copy-reply
	    mail$_send_user		mail-send-user
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-send-end
    (
	~ctx (+ mail-send-context)
    )

    (setq mail-status
	(~mail mail$send_end ~ctx 0)
    )
)
(defun mail-send-add-address
    (
	~address
	~type (+ mail$_to)
	~ctx (+ mail-send-context)
    )
    (setq mail-status
	(~mail mail$send_add_address ~ctx 2
	    mail$_send_username		~address
	    mail$_send_username_type	~type
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-send-add-attribute
    (
	~subject (concat "")
	~to_line (concat "")
	~cc_line (concat "")
	~from_line (concat "")
	~ctx (+ mail-send-context)
    )
    (setq mail-status
	(~mail mail$send_add_attribute ~ctx 4
	    (if (= ~subject "") mail$_noop mail$_send_subject
	    )			~subject
	    (if (= ~to_line "") mail$_noop mail$_send_to_line
	    )			~to_line
	    (if (= ~cc_line "") mail$_noop mail$_send_cc_line
	    )			~cc_line
	    (if (= ~from_line "") mail$_noop mail$_send_from_line
	    )			~from_line
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-send-message
    (
	~ctx (+ mail-send-context)
    )
    (setq mail-status
	(~mail mail$send_message ~ctx 0)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-send-add-body
    (
	~ctx (+ mail-send-context)
    )

    ;
    ; write the text of the message
    ;
    (save-excursion
	(beginning-of-file)
	(while (! (eobp))
	    (progn
		(set-mark)
		(end-of-line)
		(setq mail-status
		    (~mail mail$send_add_bodypart ~ctx 1
			mail$_send_record   (region-to-string)
		    )
		)
		(if (! (& mail-status 1)) (~mail-error-message))
		(error-occurred (forward-character))
	    )
	)
    )
)


;
; user routines
;
(declare-global
    mail-user-auto-purge
    mail-user-sub-directory
    mail-user-forwarding
    mail-user-personal-name
    mail-user-copy-send
    mail-user-copy-reply
    mail-user-new-messages
    mail-user-transport
    mail-user-copy-forward
)
(defun  mail-user-begin
    (
	~ctx (+ mail-user-context)
    )
    (~mail mail$user_end ~ctx 0)
    (setq mail-status
	(~mail mail$user_begin ~ctx 0
	    mail$_user_auto_purge	mail-user-auto-purge
	    mail$_user_sub_directory	mail-user-sub-directory
	    mail$_user_forwarding	mail-user-forwarding
	    mail$_user_personal_name	mail-user-personal-name
	    mail$_user_copy_send	mail-user-copy-send
	    mail$_user_copy_reply	mail-user-copy-reply
;	    mail$_user_new_messages	mail-user-new-messages
	    mail$_user_transport	mail-user-transport
	    mail$_user_copy_forward	mail-user-copy-forward
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-user-get-info
    (
	~ctx (+ mail-user-context)
    )
    (setq mail-status
	(~mail mail$user_get_info ~ctx 0
	    mail$_user_auto_purge		mail-user-auto-purge
	    mail$_user_sub_directory	mail-user-sub-directory
	    mail$_user_forwarding		mail-user-forwarding
	    mail$_user_personal_name	mail-user-personal-name
	    mail$_user_copy_send		mail-user-copy-send
	    mail$_user_copy_reply		mail-user-copy-reply
	    mail$_user_new_messages		mail-user-new-messages
	    mail$_user_transport		mail-user-transport
	    mail$_user_copy_forward		mail-user-copy-forward
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-user-set-info
    (
	~ctx (+ mail-user-context)
    )
    (setq mail-status
	(~mail mail$user_set_info ~ctx 9
	    (if mail-user-auto-purge
		mail$_user_set_auto_purge	mail$_user_set_no_auto_purge
	    )					mail-user-auto-purge
	    (if (!= mail-user-sub-directory "")
		mail$_user_set_sub_directory	mail$_user_set_no_sub_directory
	    )					mail-user-sub-directory
	    (if (!= mail-user-forwarding "")
		mail$_user_set_forwarding	mail$_user_set_no_forwarding
	    )					mail-user-forwarding
	    (if (!= mail-user-personal-name "")
		mail$_user_set_personal_name	mail$_user_set_no_personal_name
	    )					mail-user-personal-name
	    (if mail-user-copy-send
		mail$_user_set_copy_send	mail$_user_set_no_copy_send
	    )					mail-user-copy-send
	    (if mail-user-copy-reply
		mail$_user_set_copy_reply	mail$_user_set_no_copy_reply
	    )					mail-user-copy-reply
	    mail$_user_set_new_messages		mail-user-new-messages
	    (if (!= mail-user-transport "")
		mail$_user_set_transport	mail$_user_set_no_transport
	    )					mail-user-transport
	    (if mail-user-copy-forward
		mail$_user_set_copy_forward	mail$_user_set_no_copy_forward
	    )					mail-user-copy-forward
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-user-end
    (
	~ctx (+ mail-user-context)
    )

    (setq mail-status
	(~mail mail$user_end ~ctx 0)
    )
)


;
; mailfile routines
;
(declare-global
    mail-mailfile-result-spec
    mail-mailfile-mail-directory
    mail-mailfile-data-reclaim
    mail-mailfile-data-scan
    mail-mailfile-deleted-bytes
    mail-mailfile-index-reclaim
    mail-mailfile-messages-deleted
    mail-mailfile-total-reclaim
    mail-mailfile-indexed
    mail-mailfile-wastebasket
)
(defun mail-mailfile-begin
    (
	~ctx (+ mail-mailfile-context)
    )
    (~mail mail$mailfile_end ~ctx 0)
    (setq mail-status
	(~mail mail$mailfile_begin ~ctx 0
	    mail$_mailfile_mail_directory	mail-mailfile-mail-directory
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-end
    (
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_end ~ctx 0)
    )
)
(defun mail-mailfile-close
    (
	~full (+ 1)
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_close ~ctx 1
	    (if ~full mail$_mailfile_full_close mail$_noop)	0
	    ; outputs
	    mail$_data_reclaim		mail-mailfile-data-reclaim
	    mail$_data_scan		mail-mailfile-data-scan
	    mail$_index_reclaim		mail-mailfile-index-reclaim
	    mail$_messages_deleted	mail-mailfile-messages-deleted
	    mail$_total_reclaim		mail-mailfile-total-reclaim
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-compress
    (
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_compress ~ctx 0)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-info-file
    (
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_info_file    ~ctx 0
	    ; outputs
	    mail$_mailfile_deleted_bytes    mail-mailfile-deleted-bytes
	    mail$_mailfile_resultspec	    mail-mailfile-result-spec
	    mail$_mailfile_wastebasket	    mail-mailfile-wastebasket
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-purge-wastebasket
    (
	~reclaim (+ 0)
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_purge_waste ~ctx 1
	    (if ~reclaim mail$_mailfile_reclaim mail$_noop)	0
	    ; outputs
	    mail$_data_reclaim		    mail-mailfile-data-reclaim
	    mail$_data_scan		    mail-mailfile-data-scan
	    mail$_mailfile_deleted_bytes    mail-mailfile-deleted-bytes
	    mail$_index_reclaim		    mail-mailfile-index-reclaim
	    mail$_messages_deleted	    mail-mailfile-messages-deleted
	    mail$_total_reclaim		    mail-mailfile-total-reclaim
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-modify
    (
	~waste-name
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_modify    ~ctx 1
	    mail$_mailfile_wastebasket_name ~waste-name
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-mailfile-open
    (
	~mail-file-name (concat "mail")
	~mail-file-default (concat mail-mailfile-mail-directory ".mai")
	~ctx (+ mail-mailfile-context)
    )
    (setq mail-status
	(~mail mail$mailfile_open    ~ctx 2
	    mail$_mailfile_default_name	    ~mail-file-default
	    mail$_mailfile_name		    ~mail-file-name
	    ; outputs
	    mail$_mailfile_indexed	    mail-mailfile-indexed
	    mail$_mailfile_deleted_bytes    mail-mailfile-deleted-bytes
	    mail$_mailfile_resultspec	    mail-mailfile-result-spec
	    mail$_mailfile_wastebasket	    mail-mailfile-wastebasket
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)


;
; message routines
;
(declare-global
    mail-message-folder
    mail-message-selected
    mail-message-file-created
    mail-message-folder-created
    mail-message-result-spec
)
(declare-buffer-specific
    mail-message-cc
    mail-message-current-id
    mail-message-date
    mail-message-extid
    mail-message-from
    mail-message-reply-path
    mail-message-sender
    mail-message-size
    mail-message-subject
    mail-message-to
    mail-message-flags
    mail-message-flag-newmsg
    mail-message-flag-replied
    mail-message-flag-del
    mail-message-flag-extmsg
    mail-message-flag-extfnf
    mail-message-flag-sysmsg
    mail-message-flag-extnstd
    mail-message-flag-marked
    mail-message-flag-recmode
    mail-message-uflags
)
(setq mail-message-folder "mail")	; -marc
(setq-default mail-message-current-id "")
(defun mail-message-begin
    (
	~mailfile-ctx (+ mail-mailfile-context)
	~ctx (+ mail-message-context)
    )
    (~mail mail$message_end ~ctx 0)
    (setq mail-status
	(~mail mail$message_begin ~ctx 1
	    mail$_message_file_ctx		~mailfile-ctx
	    mail$_message_selected		mail-message-selected
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-message-end
    (
	~ctx (+ mail-message-context)
    )
    (setq mail-status
	(~mail mail$message_end ~ctx 0)
    )
)
(defun mail-message-copy
    (
	~select-item
	~id
	~folder
	~file (concat "")
	~file-default (concat mail-mailfile-mail-directory)
	~delete (concat 0)

	~ctx (+ mail-message-context)
    )
    (setq mail-status
	(~mail mail$message_copy ~ctx 5
	    ~select-item		~id
	    mail$_message_folder	~folder
	    (if (= ~file "") mail$_noop mail$_message_filename
	    )				~file
	    (if (= ~file "") mail$_noop mail$_message_default_name
	    )				~file-default
	    (if  ~delete mail$_message_delete mail$_noop)
	    ; outputs
	    mail$_message_file_created	    mail-message-file-created
	    mail$_message_folder_created    mail-message-folder-created
	    mail$_message_resultspec	    mail-message-result-spec
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-message-delete
    (
	~id
	~ctx (+ mail-message-context)
    )
    (setq mail-status
	(~mail mail$message_delete ~ctx 1
	    mail$_message_id	    ~id
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
)
(defun mail-message-get
    (
	~select-item
	~id
	
	~ctx (+ mail-message-context)
    )
    (setq mail-status
	(if (= mail-message-folder "newmail")
; in NEWMAIL and want to include mail$_message_auto_newmail
	    (~mail mail$message_get ~ctx 2
		~select-item		~id
		mail$_message_auto_newmail 	0		; -marc
		; outputs
		mail$_message_cc		mail-message-cc
		mail$_message_current_id	mail-message-current-id
		mail$_message_date		mail-message-date
		mail$_message_extid		mail-message-extid
		mail$_message_from		mail-message-from
		mail$_message_reply_path	mail-message-reply-path
		mail$_message_sender	mail-message-sender
		mail$_message_size		mail-message-size
		mail$_message_subject	mail-message-subject
		mail$_message_to		mail-message-to
		mail$_message_return_flags	mail-message-flags
		mail$_message_return_uflags	mail-message-uflags
	    )
	    ; else currently not in NEWMAIL
	    (~mail mail$message_get ~ctx 1
		~select-item		~id
		; outputs
		mail$_message_cc		mail-message-cc
		mail$_message_current_id	mail-message-current-id
		mail$_message_date		mail-message-date
		mail$_message_extid		mail-message-extid
		mail$_message_from		mail-message-from
		mail$_message_reply_path	mail-message-reply-path
		mail$_message_sender	mail-message-sender
		mail$_message_size		mail-message-size
		mail$_message_subject	mail-message-subject
		mail$_message_to		mail-message-to
		mail$_message_return_flags	mail-message-flags
		mail$_message_return_uflags	mail-message-uflags
	    )
	    
	)
    )
    (if (! (& mail-status 1)) (~mail-error-message))
    (~mail-setup-message-flags)
)
(defun
    (~mail-setup-message-flags
	(setq mail-message-flag-newmsg (!= (& mail-message-flags mail$m_newmsg) 0))
	(setq mail-message-flag-replied (!= (& mail-message-flags mail$m_replied) 0))
	(setq mail-message-flag-del (!= (& mail-message-flags mail$m_del) 0))
	(setq mail-message-flag-extmsg (!= (& mail-message-flags mail$m_extmsg) 0))
	(setq mail-message-flag-extfnf (!= (& mail-message-flags mail$m_extfnf) 0))
	(setq mail-message-flag-sysmsg (!= (& mail-message-flags mail$m_sysmsg) 0))
	(setq mail-message-flag-extnstd (!= (& mail-message-flags mail$m_extnstd) 0))
	(setq mail-message-flag-marked (!= (& mail-message-flags mail$m_marked) 0))
	(setq mail-message-flag-recmode (!= (& mail-message-flags mail$m_recmode) 0))
    )
)
    (defun mail-message-read
	(
	    ~lines
	    ~ctx (+ mail-message-context)
	)
	~record
	
	(setq mail-status 1)
	
	(save-excursion
	    (end-of-file)
	    (while
		(&
		    (>= (setq ~lines (- ~lines 1)) 0)
		    (& mail-status 1)
		)
		(if
		    (progn
			(setq mail-status
			    (~mail mail$message_get ~ctx 2
				mail$_message_continue	0
				mail$_message_auto_newmail  0; -marc
				; output
				mail$_message_record	~record
			    )
			)
			(if (= mail-status mail$_msgtext)
			    1
			    (= mail-status mail$_nomorerec)
			    0
			    (! (& mail-status 1))
			    (~mail-error-message)
			    0
			)
		    )
		    (progn
			;		    (sit-for 0)
			(insert-string (concat ~record "\n"))
			(setq ~mail-lines-read (+ ~mail-lines-read 1))
		    )
		)
	    )
	)
	(& 1 mail-status)		; true if read was o.k.
    )
    (defun mail-message-info
	(
	    ~select-item
	    ~id
	    
	    ~ctx (+ mail-message-context)
	)
	(setq mail-status
	    (~mail mail$message_info ~ctx 1
		~select-item		~id
		; outputs
		mail$_message_cc		mail-message-cc
		mail$_message_current_id	mail-message-current-id
		mail$_message_date		mail-message-date
		mail$_message_extid		mail-message-extid
		mail$_message_from		mail-message-from
		mail$_message_reply_path	mail-message-reply-path
		mail$_message_sender	mail-message-sender
		mail$_message_size		mail-message-size
		mail$_message_subject	mail-message-subject
		mail$_message_to		mail-message-to
		mail$_message_return_flags	mail-message-flags
		;	    mail$_message_return_uflags	mail-message-uflags
	    )
	)
	(if (! (& mail-status 1)) (~mail-error-message))
	(~mail-setup-message-flags)
    )
    (defun mail-message-modify
	(
	    ~select-item
	    ~id
	    ~flags
	    ~uflags
	    ~ctx (+ mail-message-context)
	)
	(setq mail-status
	    (~mail mail$message_modify ~ctx 3
		~select-item		~id
		mail$_message_flags		~flags
		mail$_message_uflags	~uflags
		; outputs
		mail$_message_current_id	mail-message-current-id
	    )
	)
	(if (! (& mail-status 1)) (~mail-error-message))
    )
    (defun mail-message-select
	(
	    ~folder (concat mail-message-folder)
	    ~before (concat "")
	    ~since (concat "")
	    ~to (concat "")
	    ~from (concat "")
	    ~cc (concat "")
	    ~subj (concat "")
	    ~flags (concat "")
	    ~ctx (+ mail-message-context)
	)
	(setq mail-status
	    (~mail mail$message_select ~ctx 8
		(if (= ~before "") mail$_noop mail$_message_before)		~before
		(if (= ~cc "") mail$_noop mail$_message_cc_substring)	~cc
		(if (= ~flags "") mail$_noop
		    (if (= (+ flags) 0 )
			mail$_message_flags_mbz mail$_message_flags))	~flags
		mail$_message_folder					~folder
		(if (= ~from "") mail$_noop mail$_message_from_substring)   ~from
		(if (= ~since "") mail$_noop mail$_message_since)		~since
		(if (= ~to "") mail$_noop mail$_message_to_substring)	~to
		(if (= ~subj "") mail$_noop mail$_message_subj_substring)   ~subj
		; outputs
		mail$_message_selected		mail-message-selected
	    )
	)
	(if (! (& mail-status 1)) (~mail-error-message))
	(setq mail-message-folder ~folder)
    )
    

    ;
    ; Utility routines
    ;
    (defun ~mail-add-message-to-log (~text ~error (+ 0))
	(save-excursion
	    (temp-use-buffer "mail-messages")
	    (end-of-file)
	    (insert-string (concat ~text "\n"))
	)
	(if ~error
	    (error-message ~text)
	    (message ~text)
	)
    )
    (defun ~mail-error-message (~status (+ mail-status))
	
	(if (! (& ~status 1))
	    (error-message
		(if (= (& ~status 7) 0) ; warnings are not signalled
		    (sys$getmsg mail-status)
		    (save-excursion
			(temp-use-buffer "mail-messages")
			(save-excursion
			    (backward-character)
			    (region-to-string)
			)
		    )
		)
	    )
	)
    )
    (defun ~mail-message (~status (+ mail-status))
	
	(save-excursion
	    (temp-use-buffer "mail-messages")
	    (save-excursion
		(backward-character)
		(message (region-to-string))
	    )
	)
    )
    (defun ~mail-element (~string ~sep ~index)
	
	~start ~count ~pos
	
	(setq ~count 0)
	(setq ~pos 1)
	;
	; scan for ~index''th seperator
	;
	(while
	    (&
		(<= ~pos (length ~string))
		(!= ~count ~index)
	    )
	    (progn
		(if (= ~sep (substr ~string ~pos 1))
		    (setq ~count (+ 1 ~count)))
		(setq ~pos (+ 1 ~pos))
	    )
	)
	(setq ~start ~pos)
	;
	;	scan for the next comma
	;
	(setq ~pos (+ 1 ~pos))
	(while
	    (&
		(<= ~pos (length ~string))
		(!= ~sep (substr ~string ~pos 1))
	    )
	    (setq ~pos (+ 1 ~pos))
	)
	(substr ~string ~start (- ~pos ~start))
    )
    ;
    ; returns a list of the elements in ~l1 not in ~l2
    ;
    (defun ~mail-list-diff (~l1 ~l2 ~separator)
	~l1-index ~l2-index ~result
	~l1-element ~l2-element ~absent
	
	(setq ~l1-index 0)
	(setq ~result "")
	(while (!= (setq ~l1-element (~mail-element ~l1 ~separator ~l1-index)) "")
	    (progn
		(setq ~absent 1)
		(setq ~l2-index 0)
		(while
		    (if ~absent
			(!=
			    (setq ~l2-element
				(~mail-element ~l2 ~separator ~l2-index))
			    ""
			)
		    )
		    (if (= (case-string-lower ~l2-element)
			    (case-string-lower ~l1-element))
			(setq ~absent 0)
			(setq ~l2-index (+ 1 ~l2-index))
		    )
		)
		(if ~absent
		    (setq ~result (concat ~result ~separator ~l1-element)))
		(setq ~l1-index (+ 1 ~l1-index))
	    )
	)
	(substr ~result 2 999)
    )
[makebox.ml]
; 
;	                      makebox.ml    November 2, 1986
;
;   The mlisp will draw a box around text. To use:
;   
;       1. mark a corner of the box using PF1-Select on
;	   a VT220 or esc-x rectangualar-select
;       2. go to any diagonally opposite corner
;       3. esc-x make-box
;   
;   Notes:
;       a) it is based on rectangular cut/paste distributed  with
;          EMACS 4.1.
;       b) the corners must be OUTSIDE the area to be boxed.
;       c) you can select the characters for drawing the box.
;   
;           (setq &h-symbol "-")  ; horizontal symbol
;           (setq &v-symbol "|")  ; vertical symbol
;           (setq &corners  "+")  ; corner symbol
;
;   	d) (use-stars)  will set the symbols to stars (*****).
;	e) (use-lines)  will set the symbols to lines (|---|).
;
;   Comments to ADVAX::HOM	
; 
; Modified 16-jul-1990 by Adrian Pasciuta prior to inclusion in
; EMACS V5.1 EMACSLIB.
; 
; Added "makebox" prefix to variable names and the "use-" function names.
; 
; Variables are now:		
;   makebox-h-symbol
;   makebox-v-symbol
;   makebox-corners
; 
; Changed functions are:
;   makebox-use-lines
;   makebox-use-stars
; 

(progn

(if (! (is-bound rectangular-select))
    (execute-mlisp-file "rectangular")
)

(declare-global
    makebox-h-symbol makebox-v-symbol makebox-corners
)

(setq makebox-h-symbol "-")
(setq makebox-v-symbol "|")
(setq makebox-corners  "+")
      
(defun
    (make-box
	&old-left-column &new-right-column
	(insert-vertical-bars)
	(setq &old-left-column left-column)
	(setq left-column (- right-column 1))
	(goto-right-column) (backward-character)
	(insert-vertical-bars)
	(setq left-column &old-left-column)
	(goto-left-column)
	(forward-character)
	(setq &new-right-column (- right-column 1))
	(while (< (current-column) &new-right-column)
	    (delete-next-character)
	    (insert-string makebox-h-symbol)
	)
	(goto-character start-point)
	(goto-left-column)
	(forward-character)
	(while (< (current-column) &new-right-column)
	    (delete-next-character)
	    (insert-string makebox-h-symbol)
	)
	(goto-left-column)
    )
)

(defun    
    (insert-vertical-bars
	&old-replace-mode 
	(setq &old-replace-mode replace-mode)
	(setq replace-mode 0)
	(set-end-point)
	(save-excursion
	    (next-line)
	    (if (eobp) (newline))
	)
	(check-corners)
	(goto-character start-point)
	(add-white-space)
	(untabify-line)
	(set-mark)
	(goto-character end-point)
	(end-of-line)
	(narrow-region)
	(untabify)				; remove tabs in the region
	(widen-region)			 	; to be cut
	(unset-mark)
	(goto-character start-point)
	(while (<= (dot) end-point)
	    (goto-left-column)
	    (delete-next-character)
	    (insert-string makebox-v-symbol)
	    (next-line)
	    (beginning-of-line)
	)
	(goto-character start-point)
	(goto-left-column)
	(delete-next-character)
	(insert-string makebox-corners)
	(goto-character end-point)
	(goto-left-column)
	(delete-next-character)
	(insert-string makebox-corners)
	(setq replace-mode &old-replace-mode)
    )
)

(defun    
    (makebox-use-lines
	(setq makebox-h-symbol "-")
	(setq makebox-v-symbol "|")
	(setq makebox-corners  "+")
    )
)

(defun    
    (makebox-use-stars
	(setq makebox-h-symbol "*")
	(setq makebox-v-symbol "*")
	(setq makebox-corners  "*")
    )
)

) ; end progn
[messages.key]
(local-bind-to-key "reset-message-buffer" "r")
(local-bind-to-key "delete-message-window" "q")
(local-bind-to-key "view-message-buffer" "v")
(local-bind-to-key "next-line" "n")
(local-bind-to-key "previous-line" "p")
(local-bind-to-key "beginning-of-file" "b")
(local-bind-to-key "end-of-file" "e")
[messages.ml]
; 
;  messages.ml package -- traps broadcasts to terminal and displays in
;  pop-up "Broadcast messages" buffer.  It removes non-printing characters
;  from the message, and optionally compresses REPLY format messages.
;
; (from EMACS X4.0 FT2 package)
; 
; Modified by Dave Butenhof, 06-Jan-1985
; 	1) Modify "Broadcast messages" mode line -- it will include a count
;	   of messages received, as well as the current time in the mode
;	   variable.
; 	2) Make the routine which pops up the "Broadcast messages" buffer
;	   and inserts the message callable (with message and time
;	   arguments) so that all packages which want to insert messages can
;	   do so easily and without duplication of code.
;	3) Fix search string corruption bug.
;
; modified by Barry A. Scott, 17-Nov-1985
;	merge Emacs V4.0 package with Daves mods.
; 
; modified by Barry A. Scott,  3-aug-1986
;	add key bindings to the message buffer

; 
;   broadcast message handling macro
; 
(if (! (is-bound broadcast-feeps))
    (progn
	(declare-global broadcast-feeps)
	(setq broadcast-feeps 1)
    )
)
(if (! (is-bound ~message-count))
    (progn
	(declare-global ~message-count)
	(setq ~message-count 0)
    )
)
(if (! (is-bound broadcast-compress-message))
    (progn
	(declare-global broadcast-compress-message)
	(setq broadcast-compress-message 0)
    )
)
(if (! (is-bound broadcast-display-whole-message))
    (progn
	(declare-global broadcast-display-whole-message)
	(setq broadcast-display-whole-message 0)
    )
)
(if (= operating-system-name "VMS")
(progn
; 
; process new message
; 
(defun
    ~insert-message (text new-mode) ~broadcast-i ~broadcast-win
    ;
    ; work out where the current window is releative to the
    ; Minibuf
    ; 
    (if (= current-buffer-name "Minibuf")
	(setq ~broadcast-win -1)
	(save-window-excursion
	    (setq ~broadcast-win 0)
	    (switch-to-buffer "~broadcast-find-me")
	    (pop-to-buffer "Minibuf")
	    (while (!= "~broadcast-find-me" current-buffer-name)
		(previous-window)
		(setq ~broadcast-win (+ ~broadcast-win 1))
	    )
	    (delete-buffer  "~broadcast-find-me")
	)
    )
    
    ; 
    ; create a one line message window at the bottom of the screen
    ; 
    (pop-to-buffer "Minibuf")
    (previous-window)
    (if (!= current-buffer-name "Broadcast messages")
	(if (error-occurred (split-current-window))
	    (error-occurred
		(provide-prefix-argument 3 (enlarge-window))
		(split-current-window)
	    )
	)
    )
    (if (!= current-buffer-name "Broadcast messages")
	(setq ~broadcast-win (+ ~broadcast-win 1))
    )
    (switch-to-buffer "Broadcast messages")
    (setq highlight-region 0)
    (if (!= new-mode "")
	(setq mode-string new-mode)
    )
    (error-occurred (while 1 (shrink-window)))
    (end-of-file)
    (if (! (bolp))
	(newline)
    )
    (set-mark)
    (if (!= text "")
	(progn
	    (setq ~message-count (+ ~message-count 1))
	    (insert-string text)
	)
	(if
	    (&
		(= ~message-count 0)
		(= (buffer-size) 0)
	    )
	    (insert-string "---- no messages ----")
	)
    )
    (setq mode-line-format
	(concat
	    ~message-count
	    " broadcast message"
	    (if (= ~message-count 1)
		(concat "")
		(concat "s")
	    )
	    ", at %m"
	)
    )
    (exchange-dot-and-mark)
    (save-restriction ~height
	(save-excursion
	    (narrow-region)
	    (error-occurred (re-replace-string "[\^A-\^H\^K-\^Z]" ""))
	    (error-occurred (re-replace-string "\^J\^J" "\^J"))
	    (if broadcast-compress-message
		(progn
		    (if (! (error-occurred (re-search-forward "\^MDECnet event")))
			(progn
			    (beginning-of-file)
			    (set-mark)
			    (re-search-forward "\^MDECnet")
			    (backward-word)
			    (erase-region)
			    
			)
			(|
			    (looking-at "^SHUTDOWN")
			    (looking-at "^\\*URGENT\\*")
			)
			(progn
			    (forward-word)
			    (delete-next-word)
			    (delete-next-word)
			    (delete-next-word)
			    (forward-word)
			    (delete-next-word)
			    (forward-word)
			    (delete-next-word)
			    (delete-next-word)
			)
			(looking-at "^Reply")
			(progn
			    (error-occurred (re-replace-string "\^J" "\^M"))
			    (provide-prefix-argument 6 
				(delete-next-word)
			    )
			    (insert-string "From")
			    (forward-word)
			    (delete-next-word)
			    (delete-next-word)
			)
		    )
		)
	    )
	)
	(if broadcast-display-whole-message
	    (setq window-size
		(+
		    (-
			(progn (end-of-file) (current-line-number))
			(progn (beginning-of-file) (current-line-number))
		    )
		    1
		)
	    )
	)
    )
    (if broadcast-display-whole-message
	(line-to-top-of-window)
	(progn
	    (exchange-dot-and-mark)
	    (end-of-file)
	    (error-occurred (backward-character))
	)
    )
    ;
    ; restore the cursor position
    ; 
    (pop-to-buffer "Minibuf")
    (while (>= (setq ~broadcast-win (- ~broadcast-win 1)) 0)
	(previous-window)
    )
    (if (!= text "")
	(progn
	    (setq ~broadcast-i broadcast-feeps)
	    (while (> ~broadcast-i 0)
		(setq ~broadcast-i (- ~broadcast-i 1))
		(send-string-to-terminal "\^G")
	    )
	)
    )
    (sit-for 0)
)
(defun
    (~broadcast-message
	(~insert-message
	    (process-output "broadcast-messages") 
	    (substr (current-time) 12 5)
	)
    )
)
(defun
    (reset-message-buffer
	(save-window-excursion
	    (temp-use-buffer "Broadcast messages")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (erase-buffer)
	    (use-syntax-table "broadcast-messages")
	    (modify-syntax-table "word" "$_:*")
	    (setq ~message-count 0)
	    (setq mode-line-format "0 broadcast messages, at %m")
	    (setq mode-string (substr (current-time) 12 5))
	    (sit-for 0)
	)
	(novalue)
    )
)
(defun
    (view-message-buffer
	~remove-message-window
	
	(setq ~remove-message-window 0)
	(save-window-excursion
	    (delete-other-windows)
	    (end-of-file)
	    (previous-line (- window-size 2))
	    (line-to-top-of-window)
	    (recursive-edit)
	)
	(if ~remove-message-window (delete-window))
    )
)
(defun
    (delete-message-window
	(delete-window)
	(if (is-bound ~remove-message-window)
	    (progn
		(setq ~remove-message-window 1)
		(exit-emacs)
	    )
	)
    )
)
(error-occurred (start-DCL-process "broadcast-messages"))
(set-process-output-procedure "broadcast-messages" "~broadcast-message")
(reset-message-buffer)
(save-excursion
    (temp-use-buffer "Broadcast messages")
    (error-occurred (execute-mlisp-file "messages.key"))
)
(novalue)
)
)
[minibuf-recall.key]
; minibuf-recall.key
;
; Define in current global keymap all minibuf local
; bindings except space, tab, and question mark.
(error-occurred (remove-binding "\e"))
(error-occurred (remove-binding "\^\")) ; \""))
(error-occurred (remove-binding "?"))
(bind-to-key "mini-error-and-exit" "\^G")
(bind-to-key "mini-expand-and-exit" "\(do)")
(bind-to-key "mini-newline" "\n")
(bind-to-key "mini-exit" "\r")
(bind-to-key "mini-recall" "\er")
(bind-to-key "mini-recall-reverse" "\ev")
(bind-to-key "mini-left" "\(left)")
(bind-to-key "mini-right" "\(right)")
(bind-to-key "mini-recall" "\(up)")
(bind-to-key "mini-recall-selection" "\(prev)")
(bind-to-key "mini-recall-selection" "\(next)")
(bind-to-key "mini-recall-reverse" "\(down)")
(bind-to-key "help-and-exit" "\(pf2)")
(bind-to-key "help-and-exit" "\(help)")
[minibuf-recall.ml]
(progn ; minibuf-recall.ml - patch the minibuf keymaps

; Patch the minibuf keymaps to allow all VT100 arrow and keypad keys
; to work.  Note that you may no longer use ESC to do command recognition.
; Use ESC-ESC instead.

; 
; AFter loading this package the minibuffer key maps have been modified
; to allow command recall in the minibuffer.
; 
; To all the VT100 Key pad keys and expansion keys execute
; minibuf-recall-kpx-keymaps. The binding for the minibuffer
; are read in from minibuf-recall.key.

; You also get a command recall facility on ^R

; Things to do:
; - Catch current buffer before replacing it
;
; 	15-SEP-1987 Daryl Gleason
; 	If current line to be stored is a superset of the last line
; 	stored, store current line over last line.  Also add mini-left
;	and mini-right to avoid messy errors in the minibuffer.
; 
;	12-sep-1987 Barry A. Scott
;	Add to Emacs kit
; 
;	1-may-1986 J.A.Lomicka Don't save null strings in recall ring
;	Edited 24-April-1986 to add command recall	J.A.Lomicka
;	Original code 15-Oct-1984 by Roy Lomicka
;
(declare-global
  mini-recall-buffer;	Array where commands are stored
  mini-recall-index;	Integer where commands go in
  mini-recall-point);	Place where commands come out
(setq mini-recall-buffer (array 0 63))

(declare-buffer-specific ~tmp)
(setq ~tmp 0)
(while (< ~tmp 64)
    (setq-array mini-recall-buffer ~tmp "")
    (setq ~tmp (+ ~tmp 1))
)

(defun
    (mini-newline
	; If you exit from an enlarged minibuf at arg level, it will stay
	; enlarged.  You'll have to get into another arg level minibuf and
	; make the window smaller, then exit from it to reverse the effect.
	; At command level, the enlargement doesn't seem to be so permanent.
	(error-occurred (enlarge-window))
	(newline)
    )
)

(defun
    (mini-exit-handler ~cur ~last
	; store string in recall buffer
	(beginning-of-file)(set-mark)(end-of-file)
	(setq ~cur (region-to-string))			; current string
	(setq						; last string
	    ~last
	    (fetch-array
		mini-recall-buffer (& 63 (- mini-recall-index 1))
	    )
	)
	(if (length ~cur)	; if cur string not null
	    (if
		(&
		    (>=		; if length of cur string >= last string
			(length ~cur)
			(length ~last)
		    )
		    (=		; & substr of cur string = last string
			(substr ~cur 0 (length ~last))
			~last
		    )
		)
		; then store the current string over the last string
		(setq-array
		    mini-recall-buffer
		    (& 63 (- mini-recall-index 1))
		    ~cur
		)
		; else store current string at next slot and inc pointers
		(progn
		    (setq-array
			mini-recall-buffer
			mini-recall-index
			~cur
		    )
		    (setq mini-recall-index (& 63 (+ mini-recall-index 1)))
		)
	    )
	)
	(setq mini-recall-point mini-recall-index)
	; undo any enlargement before calling expand-and-exit
	(setq window-size 1)
    )
)

(defun
    (mini-error-and-exit
	(setq mini-recall-point mini-recall-index)
	; undo any enlargement before calling error-and-exit
	(setq window-size 1)
	(error-and-exit)
    )
)

(defun
    (mini-expand-and-exit
	(error-occurred (mini-exit-handler))
	(expand-and-exit)
    )
)

(defun
    (mini-exit
	(error-occurred (mini-exit-handler))
	(exit-emacs)
    )
)

(defun
    (mini-recall ~string ~start-point ~move-by
	(setq-array mini-recall-buffer mini-recall-point (region-to-string))
	(error-occurred (erase-region))
	(set-mark)
	(setq ~start-point mini-recall-point)
	(setq ~move-by prefix-argument)
	(while
	    (progn
		(setq mini-recall-point (& 63 (- mini-recall-point ~move-by)))
		(setq ~string (fetch-array mini-recall-buffer mini-recall-point))
		(& (= (length ~string) 0) (!= mini-recall-point ~start-point))
	    )
	    (setq ~move-by (if (< ~move-by 0) -1 1))
	)
	(insert-string ~string)
    )
)

(defun
    (mini-recall-selection ~string ~start-point
	(setq-array mini-recall-buffer mini-recall-point (region-to-string))
	(error-occurred (erase-region))
	(set-mark)
	(setq ~start-point mini-recall-point)
	(if
	    (save-window-excursion
		(pop-to-buffer "Recall list")
		(local-bind-to-key "exit-emacs" "\^m")
		(erase-buffer)
		(unset-mark)
		(insert-string "Select the line to recall and press RETURN\n")
		(while
		    (progn
			(setq mini-recall-point (& 63 (- mini-recall-point 1)))
			(setq ~string (fetch-array mini-recall-buffer mini-recall-point))
			(!= mini-recall-point ~start-point)
		    )
		    (if (!= (length ~string) 0)
			(insert-string ~string "\n")
		    )
		)
		(beginning-of-file)
		(next-line)
		(beginning-of-line)
		(recursive-edit)
		(beginning-of-line)
		(if (! (bobp))
		    (progn
			(set-mark)
			(end-of-line)
			(setq ~string (region-to-string))
			1
		    )
		    0
		)
	    )
	    (insert-string ~string)
	)
    )
)

(defun
    (mini-recall-reverse
	(provide-prefix-argument (- prefix-argument) (mini-recall))
    )
)


(defun
    (mini-left
	(error-occurred (backward-character prefix-argument))
    )
)

(defun
    (mini-right
	(error-occurred (forward-character prefix-argument))
    )
)

(defun
    (~minipatch-kpx
	(bind-to-key "mini-exit" "\(pf2)")
	(bind-to-key "mini-exit" "\(pf3)")
	(bind-to-key "mini-exit" "\(pf4)")
	(bind-to-key "mini-exit" "\(kp0)")
	(bind-to-key "mini-exit" "\(kp1)")
	(bind-to-key "mini-exit" "\(kp2)")
	(bind-to-key "mini-exit" "\(kp3)")
	(bind-to-key "mini-exit" "\(kp4)")
	(bind-to-key "mini-exit" "\(kp5)")
	(bind-to-key "mini-exit" "\(kp6)")
	(bind-to-key "mini-exit" "\(kp7)")
	(bind-to-key "mini-exit" "\(kp8)")
	(bind-to-key "mini-exit" "\(kp9)")
	(bind-to-key "mini-exit" "\(comma)")
	(bind-to-key "mini-exit" "\(minus)")
	(bind-to-key "mini-exit" "\(dot)")
	(bind-to-key "mini-exit" "\(enter)")
	(bind-to-key "mini-exit" "\(gold)\(pf2)")
	(bind-to-key "mini-exit" "\(gold)\(pf3)")
	(bind-to-key "mini-exit" "\(gold)\(pf4)")
	(bind-to-key "mini-exit" "\(gold)\(kp0)")
	(bind-to-key "mini-exit" "\(gold)\(kp1)")
	(bind-to-key "mini-exit" "\(gold)\(kp2)")
	(bind-to-key "mini-exit" "\(gold)\(kp3)")
	(bind-to-key "mini-exit" "\(gold)\(kp4)")
	(bind-to-key "mini-exit" "\(gold)\(kp5)")
	(bind-to-key "mini-exit" "\(gold)\(kp6)")
	(bind-to-key "mini-exit" "\(gold)\(kp7)")
	(bind-to-key "mini-exit" "\(gold)\(kp8)")
	(bind-to-key "mini-exit" "\(gold)\(kp9)")
	(bind-to-key "mini-exit" "\(gold)\(comma)")
	(bind-to-key "mini-exit" "\(gold)\(minus)")
	(bind-to-key "mini-exit" "\(gold)\(dot)")
	(bind-to-key "mini-exit" "\(gold)\(enter)")
    )
)
    
(defun
    (minibuf-recall-kpx-keymaps
	(patch-minibuf-keymaps)
	(use-global-map "Minibuf-local-map")
	(~minipatch-kpx)
	(use-global-map "Minibuf-local-NS-map")
	(~minipatch-kpx)
	(use-global-map "default-global-keymap")
    )
)
    
; 
; set up the minibuf keymaps and set
; global keymap to "default-global-keymap"
; 
(use-global-map "Minibuf-local-map")
(execute-mlisp-file "minibuf-recall.key")
(use-global-map "Minibuf-local-NS-map")
(execute-mlisp-file "minibuf-recall.key")
(bind-to-key "mini-expand-and-exit" " ")
(bind-to-key "mini-expand-and-exit" "\t")
(use-global-map "default-global-keymap")

) ; end of 1st progn
[minibuf.ml]
; minibuf.ml
; 
; History:
; 
; modified by Rick Ellison to add capability for doing:
;
;tested with V4.1 linked on  2-SEP-1986 at 16:34 on HEART by SCOTT
;  -- not tested with v5 Emacs --
; 
; ^U<arg><esc><esc><minbuf stuff><esc><esc>
;	         or
;        <esc><esc><minbuf stuff>^U<arg><esc><esc>
; 
; either of which result in <minbuf stuff> being given the <arg> as a
; prefix-argument.  If both the entering and exiting args are provided, the
; exiting arg is used.
; 
; 
;   this packages has been worked on by:-
;	Bob Surtees, Rod, Burr, Roger Goun, Rick Ellison,
;	and Barry Scott.
; 
; Include into 4.1 Emacs from emacsprocs.not
;
;  Note: apostrophes are doubled so that paren matching,
;        forward-sexpr, and such work correctly.
; 
(progn				; for execute-mlisp-buffer
(defun
    (minibuf-exit val
	; 
	; Save the user''s s-expressions in a buffer, so they
	; can be restored later.
	; 
	(switch-to-buffer "minibuffer-save")
	(yank-buffer "minibuffer")
	(switch-to-buffer "minibuffer")
	
	; 
	; Build a defun around buffer.
	; 
	(beginning-of-file)
	(insert-string "(defun (~minifun ")	; open the "defun"
; 
; add to let ^U<arg><esc><esc><stuff><esc><esc> do <stuff> <arg> times
; 
	(insert-string " \n (provide-prefix-argument saved-prefix-argument" )
	(insert-string " \n (progn \n ")
; end of add
	(end-of-file)
; add to line below the \n and parens to balance those added above
	(insert-string "\n))))") 		; close the "defun"
	
	; 
	; Evaluate it in an error-occurred, so we do not get
	; stuck in the window if we blow it.
	; 
	(if (error-occurred
		(execute-mlisp-buffer))
	    ; if there was an error report it
	    (setq val (concat "definition error: " error-message))
	    ; else say no problem so far
	    (setq val "def ok")
	)
	; 
	; here if error or not
	; Restore the user''s s-expressions without the
	; surrounding "(defun( ... ))"
	; 
	(erase-buffer)			; get rid of what we had
	(yank-buffer "minibuffer-save")	; get original back
	(delete-buffer "minibuffer-save")	; eliminate save buffer
	(delete-window)			; become invisible
	
	; 
	; depending on success of definition of ~minifun
	; 
	(if (= val "def ok")
	    ; then execute the new function in error occurred
	    (if (error-occurred
		    (setq val (~minifun)))
		(if (!= error-message "\"~minifun\" did not return a value; \"setq\" was expecting it to.")
		    (progn ; else a real error
			(send-string-to-terminal "\^G")
			(message "execute error:" error-message)
		    )
		    ; probably a function that just did not return a value
		)
		(message val)
	    )
	    ; else had error during defun
	    (progn
		(send-string-to-terminal "\^G")
		(message val)
	    )
	)
    )
)

    
(defun
    (minibuf-enter
	(split-current-window)		; Make a new window.
 	(error-occurred
 	    (provide-prefix-argument
 		(- window-size minibuf-window-size 1)    ; make window big as
  		(shrink-window)			     ; desired
 	    )
 	)
	(switch-to-buffer "minibuffer")
	(save-excursion
	    (set-mark)				; to be able to remove mode-string
	    (insert-string mode-string)		; see if we are in Lisp mode
	    (exchange-dot-and-mark)		; get ready
	    (if (! (looking-at ".*Lisp"))	; if not looking at some lisp mode
		(lisp-mode))			; then get one
	    (erase-region)		; get mode back out of buffer
	)
    )
)


(defun
    (minibuf
; 
; note:  Esc-Esc (or whatever you bind to minibuf) brings you here for both
; entering and leaving the minibuf
; 
    (if (= current-buffer-name "minibuffer")
; 
; add for doing ^U<arg><esc><esc> upon leaving minibuf
; 
       	; then already in minibuf; so, exit
	(progn
	    (if prefix-argument-provided
		; then
		(setq saved-prefix-argument prefix-argument)
		; else use previous value
	     )  ; end of if
; end of add -- note paren to balance progn is added below
; 
	    (minibuf-exit)			;     in minibuffer so exit
; add to allow ^U<arg><esc><esc> upon entering
; 
	)  ; end of then already in minibuf; so, exit
	(progn  ; else not already in minibuf; so, enter it
	    (if prefix-argument-provided
		; then
		(setq saved-prefix-argument prefix-argument)
		; else use 1 -- no sticky entry values
		(setq saved-prefix-argument 1)
	     )  ; end of if
; end of add
	    (minibuf-enter)			; not in minibuffer so enter
; add paren to balance
	)  ; end else not already in minibuf; so, enter it
; 
	)
    )
)


(save-window-excursion
    ; Startup actions; to be executed only once.
    (declare-global	minibuf-window-size
; add global variable
			saved-prefix-argument)
    (if (= minibuf-window-size 0)
	(setq minibuf-window-size 5))	; set a default
    ; get key bindings, if any
    (if (error-occurred (execute-mlisp-file "minibuf.key"))
	; default if no .KEY file
	(bind-to-key "minibuf" "\e\e")
    )
    (switch-to-buffer "minibuffer")
    (lisp-mode)
)
)
[misc.ml]
; Miscellaneous local functions
 (defun
    (multi-autoload
	fns 
	(if (< (nargs) 2)
	    (error-message "Insufficient arguments to multi-autoload"))
	(setq fns 2)
	(while (>= (nargs) fns)
	  (progn (autoload (arg fns) (arg 1)) (setq fns (+ fns 1))))
	(novalue))


    (function-bind		   	; Do magic to set up bindings
	name			; function name
	file			; file containing definition
	key			; key to bind to
	(setq name (arg 1 "Function Name to bind: "))
	(setq file (arg 2 "File containing definition: "))
	(setq key (arg 3  "Key to bind to function: "))
	(autoload name file)
	(bind-to-key name key)
	(novalue))
 )
[mouse-rectangular.ml]
;	mouserect.ml	Mouse interface to rectangular package
;
;	27-Jan-1988	Jeff Lomicka
;
;	Requires "mousev5.ml" and "rectangular.ml" be loaded first.
;
;	Uses escape-prefix mouse buttons for rectangular operations.
;		Left - Rectangular copy between text cursor and mouse
;		Cetner - Rectangular cut between text cursor and mouse
;		Right - Paste at mouse location
;	If in replace-mode, the replace-mode variation of cut and paste
;	is used.

(progn

(defun mouse-rcopy()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (set-mark)
  (rectangular-select)
  (mouse-reposition mousex mousey mouseevent)
  (rectangular-copy)
)

(defun mouse-rcut()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (set-mark)
  (setq replace-with-white-space replace-mode)
  (rectangular-select)
  (mouse-reposition mousex mousey mouseevent)
  (rectangular-cut)
)

(defun mouse-rpaste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (setq overstrike-mode replace-mode)
  (rectangular-paste)
)

(bind-to-key "mouse-rcopy" "\e\201C")
(bind-to-key "mouse-rcut" "\e\201E")
(bind-to-key "mouse-rpaste" "\e\201G")
)
[mouse.ml]
;	mousev5.me	Mouse package for V5 emacs
;
;	25-Sep-1987	J.A.Lomicka
;
;	This mouse package requires V5 of VAX Emacs, which supports
;	the built-in goto-wondow-at-x-y and has full support
;	for CSI sequences.  By itself, it provides support for
;	VWS V3.2 terminal emulator and compatible terminals.
;
(declare-global
  mouse-continuous;	If set to 1, scrolling is continuous
  mouse-was-recent;	When set, mouse click was within 4 seconds
  favorite-set-mark;	Set-mark function
  favorite-copy-region;	Copy region function
  favorite-kill-region;	Kill region function
  favorite-yank;	Yank function
  foreign-mouse;	Set to indicate event was from a foreign mouse
  foreign-mouse-x
  foreign-mouse-y
  foreign-mouse-event
)

(defun mouse-on
  (cursor-type (+ (get-tty-string ": mouse-on (cursor-type 0-1)[0] ")))
;	Send the escape sequences that enable the VWS mouse.  Note that with
;	V3.2 of VWS, enabling the mouse implies taking responsibility for
;	cross-session cut and paste as well.

  (setq mouse-enable 1)
  (if terminal-DEC-CRT-level-2
    (send-string-to-terminal (concat "\e[" cursor-type ")u\e[1;3'{\e[1;2'z"))
  )
)

(defun mouse-mode()
    ;	Enable the control-string parser for parsing of VWS and BBN BitGraph
    ;	escape sequences.  Convert F-keys and mouse sequences to single
    ;	keystrokes.
    (if (! control-string-processing)
	(error-message
"Control String Processing must be enable for the Mouse package to work.")
    )
    (setq control-string-convert-mouse 1)
    ;	Set up the favorite copy/cut/paste commands if not already done
    
    (if (= favorite-set-mark "") (setq favorite-set-mark "set-mark"))
    (if (= favorite-copy-region "")
	(setq favorite-copy-region "copy-to-killbuffer")
    )
    (if (= favorite-kill-region "")
	(setq favorite-kill-region "delete-to-killbuffer")
    )
    (if (= favorite-yank "") (setq favorite-yank "v5bug-yank"))
    
    ;	Set up the default bindings for the mouse buttons
    
    (bind-to-key "mouse-pos" "\(mouse-1-down)")
    (bind-to-key "mouse-up" "\(mouse-1-up)")
    (bind-to-key "mouse-cut" "\(mouse-2-down)")
    (bind-to-key "mouse-up" "\(mouse-2-up)")
    (bind-to-key "mouse-paste" "\(mouse-3-down)")
    (bind-to-key "mouse-up" "\(mouse-3-up)")
    (bind-to-key "mouse-moved" "\201K")
    
    ;	Set up the default bindings for local COPY/PASTE
    
    (bind-to-key "accept-region" "\e[!x")
    (bind-to-key "accept-region" "\e[(")
    (bind-to-key "mouse-local-copy" "\^X\(mouse-2-down)")
    (bind-to-key "mouse-local-paste" "\^X\(mouse-3-down)")
    (bind-to-key "local-copy-region" "\^Xc")
    (bind-to-key "local-paste" "\^X\^Y")
)

(defun mouse-off()
  (setq mouse-enable 0)
  (if terminal-DEC-CRT-level-2
    (send-string-to-terminal "\e[0'{\e[0;0'z")
  )
)

;	The routine mouse-parameters is used to get the parameters out
;	of the CSI argument string and put them in the caller's variables
;	"mousex", "mousey", and "mouseevent".  This provides a central
;	place where mouse parameters are obtained from their corresponding
;	parameters, and inherently defines a hook that can be used to
;	accomodate other kinds of terminals.  (Load this package, then
;	redefine mouse-parameters to get the parameters correctly for the
;	foreign terminal.)

(defun mouse-parameters()
    x y event
  (if foreign-mouse
    (progn
      (setq y foreign-mouse-y)
      (setq x foreign-mouse-x)
      (setq event foreign-mouse-event)
      (setq foreign-mouse 0)
    )
  ; else this is a normal mouse
    (progn
      (setq y (+ (fetch-array control-string-parameters 1 3)))
      (setq x (+ (fetch-array control-string-parameters 1 4)))
      (setq event (+ (fetch-array control-string-parameters 1 1)))
    )
  )
  (if (is-bound mousey)
    (setq mousey y)
  )
  (if (is-bound mousex)
    (setq mousex x)
  )
  (if (is-bound mouseevent)
    (setq mouseevent event)
  )
  (if (is-bound mouse-second-click)
    (setq mouse-second-click (&
      mouse-was-recent
      (= x ~saved-mousex)
      (= y ~saved-mousey)
      (= (| event 1) (| ~saved-mouseevent 1))
    ))
  )
  (setq mouse-was-recent 1);	Start the double-click timer
  (schedule-procedure "mouse-not-recent" 4)
)

(defun mouse-not-recent();	Schedule to cancel double-click timer
  (setq mouse-was-recent 0)
)

;	The routine mouse-reposition is used to position the cursor at
;	the location the user specified in the last mouse click.  Also
;	performed is any set-up action needed to provide the scrolling and
;	mode bar movements of mouse-finish-scrolling.
(declare-global
  ~saved-mousex
  ~saved-mousey
  ~saved-mouseevent
  ~saved-mouseplace
)

(defun mouse-reposition( x y event)
    placecode
  (setq ~saved-mousex x)
  (setq ~saved-mousey y)
  (setq ~saved-mouseevent event)
  (setq ~saved-mouseplace (goto-window-at-x-y x y))
  (if replace-mode
    (if (= ~saved-mouseplace 0)
      (while (< (current-column) mousex) (insert-string " "))
    )
  )
)

;	The routine mouse-finish-scrolling is used to complete the
;	scrolling and mode bar movements that were initiated in
;	a call to mouse-reposition.

(defun mouse-finish-scroll( c r)
  (if (= ~saved-mouseplace 0);	Started in text, scroll text in window
    (error-occurred
      (provide-prefix-argument (- ~saved-mousey r) (scroll-one-line-up))
    )
  )
  (if (& ~saved-mouseplace 1);  Started in mode line, move mode line or h-scroll
    (progn; mode line and horizontal indicated,  Move the mode line
      (if (> ~saved-mousey r)
	(progn; Moving mode line up, delete windows in the way
	  (while (>= (- ~saved-mousey r) (- window-size 1)) 
	    (while (!= window-width terminal-width) (delete-window))
	    (delete-window)
	  )
	  (set-window-size (+ window-size (- r ~saved-mousey)))
	)
      (< ~saved-mousey r); elseif
	(progn; Moving mode line down
	  (down-window)
	  (while (>= (- r ~saved-mousey) (- window-size 1))
	    (setq ~saved-mousey (+ ~saved-mousey window-size))
	    (while (!= window-width terminal-width) (delete-window))
	    (delete-window)
	    (error-occurred (down-window))
	  )
	  (goto-window-at-x-y ~saved-mousex ~saved-mousey)
	  (set-window-size (+ window-size (- r ~saved-mousey)))
	); of moving mode line down progn
      (!= ~saved-mousex c); else if horizontal scroll
	(error-occurred (provide-prefix-argument (- ~saved-mousex c)
	  (scroll-one-column-right)
	))
      ); of moving mode line if
    ); horizontal and mode line progn
  ); of place 1 of
  (if (& ~saved-mouseplace 2);  Started in vertical bar, move vertical bar
    (progn; move a vertical bar
      (if (> ~saved-mousex c)
	(progn; Moving bar left, delete windows in the way
	  (while (>= (- ~saved-mousex c) (- window-width 1)) 
	    (delete-window)
	  )
	  (set-window-width (+ window-width (- c ~saved-mousex)))
	)
      (< ~saved-mousex c); elseif
	(progn; Moving mode line right
	  (right-window)
	  (while (>= (- c ~saved-mousex) (- window-width 1))
	    (setq ~saved-mousex (+ ~saved-mousex window-width))
	    (delete-window)
	    (right-window)
	  )
	  (left-window)
	  (set-window-width (+ window-width (- c ~saved-mousex)))
	); of moving mode line down progn
      ); moving right if
    ); moving vertical bar progn
  ); of place 2 if
)

;	The routine mouse-up is used as the action routine to respond to
;	a button-up that finishes a scrolling operation. Normally
;	it is associated with button 1, but it checks to make sure that
;	the most recent event (defined by the most recent call to
;	mouse-reposition) was a down event on whatever button it
;	is bound to, so it could be moved to any button-up key binding.

(defun mouse-up() mousex mousey mouseevent
  (if mouse-continuous (send-string-to-terminal "\e[1;3'{\e[1;2'z"))
  (mouse-parameters)
  (if (= (+ ~saved-mouseevent 1) mouseevent)
    (mouse-finish-scroll mousex mousey)
  )
  (setq ~saved-mouseevent mouseevent)
)

;	The routine "mouse-pos" is used as the action routine for a
;	down-stroke that performs the usual positioning and scrolling
;	operations.  It may be bound to any mouse down-key, or called as
;	the first action, with no parameters, in any function that is
;	bound to a mouse down-key.  In this way, packages can have local
;	bindings to the mouse keys that do other things after performing
;	the position function.

(defun mouse-pos() mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (if mouse-second-click
    (error-occurred (execute-keyboard-macro))
  )
  (if mouse-continuous (send-string-to-terminal (concat
    "\e[" mousey ";" mousex ";" mousey ";" mousex "'w"
  )))
)
;	Mouse-moved is the default action for filter rectangle events,
;	and is used to perform continuous scroll operations while the button
;	is held down.  We fetch the parameters directly, in order to avoid
;	resetting the double-click timer
(defun mouse-moved() mousex mousey mouseevent
  (if (= ~saved-mouseevent 2)
    (progn
      (setq mousey (+ (fetch-array control-string-parameters 1 3)))
      (setq mousex (+ (fetch-array control-string-parameters 1 4)))
      (mouse-finish-scroll mousex mousey)
      (setq ~saved-mousex mousex)
      (setq ~saved-mousey mousey)
      (send-string-to-terminal (concat
	"\e[" mousey ";" mousex ";" mousey ";" mousex "'w"
      ))
    )
  )
)

;	The routine "mouse-cut" is used as the action routine for a
;	down-strike that marks the opposite end of a region and copies
;	or cuts the region to the kill buffer.
(defun mouse-cut()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (execute-extended-command favorite-set-mark); Use user preference
  (mouse-reposition mousex mousey mouseevent)
  (exchange-dot-and-mark)
  (if mouse-second-click
    (execute-extended-command favorite-kill-region); Use user preference
  ;else this is first click
    (progn
      (execute-extended-command favorite-copy-region); Use user preference
      (message "Region copied")
    )
  )
)

(defun mouse-paste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (if mouse-second-click
    (execute-extended-command favorite-yank); Use user preference
  ; else
    (message "Press again to paste.")
  )
)

(defun copy-to-killbuffer( );	Missing from basic command set
  (copy-region-to-buffer "Kill buffer")
)

(defun v5bug-yank();		There is a bug in (ex-ext-cmd "yank..")
  (yank-buffer "Kill buffer")
)

; The function "local-copy-region" is used to copy the current select region
; into the terminal's local copy/paste buffer.

(defun local-copy-region ()
  (narrow-region)
  (send-string-to-terminal "\eP!x")
  (beginning-of-file)
  (while (! (eobp))
    (send-string-to-terminal
      (if (= (following-char) '\n')
	"0D";	Newline case
      ;	Else normal character, convert to hex
	(concat
	  (substr "0123456789ABCDEF" (+ 1 (& (/ (following-char) 16) 15)) 1)
	  (substr "0123456789ABCDEF" (+ 1 (& (following-char) 15)) 1)
	)
      )
    )
    (forward-character)
  )
  (send-string-to-terminal "\e\\")
  (widen-region)
)

;	The function "mouse-local-copy" is used to mark out a region and
;	copy it to the terminal's local copy/paste buffer

(defun mouse-local-copy()  mousex mousey mouseevent mouse-second-click
  (set-mark)
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (local-copy-region)
  (message "Region copied to terminal")
)

;	The function "local-paste" is used to request the paste buffer
;	of the terminal.

(defun local-paste ()
  (send-string-to-terminal "\e[0;0)t")
)

;	"mouse-local-paste" is used to paste into the current locator position

(defun mouse-local-paste()  mousex mousey mouseevent mouse-second-click
  (mouse-parameters)
  (mouse-reposition mousex mousey mouseevent)
  (local-paste)
)

(defun dehex( a b) c
  (setq c (+
    (* 16 (if (>= a 'A')  (- a 55) (- a '0')))
    (if (>= b 'A')  (- b 55) (- b '0'))
  ))
  (if (= c 13) 10 c)
)

;	The function "accept-region" will read the select region from the
;	terminal.  A bug in the current version of VWS causes VWS to lie
;	about the buffer length sometimes, so you can't trust the number
;	in C, you have to wait for the ST.

(defun accept-region () c s c1 c2
(send-string-to-terminal "\007")
  (setq c (+ 2 (fetch-array control-string-parameters 1 1)))
  (setq s "")
  (while (> c 0)
    (setq c1 (get-tty-character))
    (setq c2 (get-tty-character))
    (if (< c1 '0');	If first character is a control character
      (setq c -1);	exitloop
    ;else this is good hex data
      (setq s (concat s (char-to-string (dehex c1 c2))))
    )
  )
  (set-mark)
  (insert-string s)
)

(defun set-window-size ( size) r
  (setq r (- size window-size))
  (error-occurred (provide-prefix-argument r (enlarge-window)))
)

(defun set-window-width ( size) r
  (setq r (- size window-width))
  (error-occurred (provide-prefix-argument r (widen-window)))
)

(mouse-mode)

[newcompil.ml]
; This package provides a replacement for the standard ^X^E/^X^N compilation
; package.  It operates pretty much the same as the old except that the
; process control facilities are used.  Consequently, while a compilation is
; going on you can go off and do other things and (a major win) you can
; interrupt the compilation partway through.
(declare-global compilation-may-be-active compile-it-command errors-scanned)
(if (= operating-system-name "VMS")
    (setq compile-it-command "MMS")
    (= operating-system-name "Windows")
    (setq compile-it-command "make")
    (setq compile-it-command "make")
)
(error-occurred (rename-macro "compile-it" "old-compile-it"))
(error-occurred (rename-macro "next-error" "old-next-error"))
(error-occurred (rename-macro "previous-error" "old-previous-error"))

(defun
    (compile-it command
	~arg
	(setq ~arg (if (> prefix-argument 1)
			  (arg 1 ": compile-it using command: ")
			 ""))
	(if (length ~arg)
	    (setq compile-it-command ~arg))
	(setq compilation-may-be-active 1)
	(setq errors-scanned 0)
	(save-excursion
	    ; make sure that the Error log window appears
	    ; the save-excursion will undo the pop-to-buffer without the
	    ; split-current-window
	    (if (= (buffer-size) 0)
		(split-current-window))
	    (pop-to-buffer "Error log")
	    (erase-buffer)
	    (setq highlight-region 0)
	    (write-modified-files)
	    (error-occurred (kill-process "Error log"))
	    (if (= operating-system-name "VMS")
		(progn
		    (start-DCL-process "Error log")
		    (set-process-output-buffer "Error log" "Error log")
		    (set-process-input-procedure "Error log" "~start-MMS")
		)
		(= operating-system-name "Windows")
		(progn ~old-cli-name ~status
		    (setq ~old-cli-name cli-name)
		    (setq cli-name (concat (getenv "COMSPEC") " /c " compile-it-command))
		    (setq ~status (error-occurred (start-DCL-process "Error log")))
		    (setq cli-name ~old-cli-name)
		    (if ~status (error-message error-message))
		    (set-process-termination-procedure "Error log" "~compil-terminated")
		    (set-process-output-buffer "Error log" "Error log")
		)
		(progn
		    (start-process "Error log" compile-it-command)
		    (set-process-termination-procedure "Error log" "~compil-terminated")
		)
	    )
	    (setq mode-line-format "%b %M (%m)")
	    (setq mode-string "Compilation active")
	    (novalue)
	)
    )
)

(if (= operating-system-name "VMS")
    (progn
	(defun  (~start-MMS
		(set-process-input-procedure "Error log" "~stop-MMS")
		(send-string-to-process "Error log" compile-it-command)
		)
	)

	(defun (~stop-MMS
		(error-occurred
			(kill-process "Error log"))
		(setq compilation-may-be-active 0)
		(save-excursion
		    (temp-use-buffer "Error log")
		    (setq mode-string "Finished"))
		    (sit-for 0)
	       )
	)
    )
    (progn
	(defun (~compil-terminated
		(setq compilation-may-be-active 0)
		(save-excursion
		    (temp-use-buffer "Error log")
		    (setq mode-string "Finished"))
		    (sit-for 0)
	       )
	)
    )
)    
(defun
    (kill-compilation
	(save-excursion
	    (temp-use-buffer "Error log")
	    (error-occurred
		(kill-process "Error log"))
	    (setq mode-string "Compilation killed")
	    (sit-for 0)
	    (setq compilation-may-be-active 0)
	    (setq buffer-is-modified 0)
	    (novalue)
	)
    )
)
    
(defun
    (previous-error
	(if (! errors-scanned)
	    (next-error)
	    (if (error-occurred (old-previous-error))
		(progn
		    (save-excursion
			(pop-to-buffer "Error log")
			(sit-for 0)
		    )
		    (error-message "No more errors...")
		)
	    )
	)
    )
)

(defun
    (next-error
	(save-excursion
	    (if compilation-may-be-active
		(error-message "The compilation is still running")
	    )
	    (if (! errors-scanned)
		(progn
		    (pop-to-buffer "Error log")
		    (setq mode-string "Scanning for errors")
		    (sit-for 0)
		    (setq buffer-is-modified 0)
		    (beginning-of-file)
		    (set-mark)
		    (end-of-file)
		    (parse-error-messages-in-region)
		    (setq compilation-may-be-active 0)
		    (setq errors-scanned 1)
		    (beginning-of-file)
		    (setq mode-string "Displaying errors")
		    (sit-for 0)
		)
	    )
	)
	(if (error-occurred (old-next-error))
	    (progn
		(save-excursion
		    (pop-to-buffer "Error log")
		    (setq mode-string "All errors displayed")
		    (sit-for 0)
		)
		(error-message "No more errors...")
	    )
	)
	)
)
    
(bind-to-key "kill-compilation" "\^x\^K"))
(novalue)
[normalmod.ml]
(defun
    (normal-mode
	(setq left-margin 1)
	(setq right-margin 1000)
	(use-local-map "default-global-keymap")
	(use-abbrev-table "normal-mode")
	(use-syntax-table "normal-mode")
	(setq mode-line-format default-mode-line-format)
	(setq mode-string "Normal")
	(novalue)
    )
)
[notes.key]
; 
; notes.key - key definitions for EMACS VAXnotes interface
; 
(save-excursion
    (temp-use-buffer "notes-keymap-hack")
    (define-keymap "notes-keymap")
    (use-local-map "notes-keymap")

(defun
    (~notes-bind-keys
	~argnum ~charnum ~key ~str1 ~str2
	
	(setq ~argnum 1)
	(while (< ~argnum (nargs))
	    (progn
		(setq ~str1 (arg (+ ~argnum 1)))
		(setq ~str2 (arg (+ ~argnum 2)))
		(setq ~charnum 0)
		(while (<= ~charnum (length ~str2))
		    (local-bind-to-key
			(arg ~argnum)
			(concat ~str1 (substr ~str2 ~charnum 1))
		    )
		    (setq ~charnum (+ ~charnum 1))
		)
	    (setq ~argnum (+ ~argnum 3))
	    )
	)
    )
)
    (~notes-bind-keys
	"notes-c-not-a-command"		"" "abcdefghijklmnopqrstuvwxyz"
	"notes-c-not-a-command"		"" "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	"notes-c-not-a-command"		"" "0123456789"
	"notes-c-not-a-command"		"" "!@#$%^&*()_+~-=`{}:;'\"\\|"; '"
	"notes-c-add-entry"		"A" "eE"
	"notes-c-add-keyword"		"A" "kK"
	"notes-c-add-marker"		"A" "m"
	"notes-c-add-member"		"A" "MuU"

	"notes-c-answer"		"a" ""

	"notes-c-back-note"		"b" "nN"
	"notes-c-back-reply"		"b" "rR"
	"notes-c-back-reply"		"<" ""
	"notes-c-back-topic"		"b" "tT"
	"notes-c-back-topic"		"," ""

	"notes-c-close"			"c" ""

	"notes-c-create-conference"	"C" "cC"
	"notes-c-create-keyword"	"C" "kK"

	"notes-c-delete-entry"		"D" "eE"
	"notes-c-delete-keyword"	"D" "kK"
	"notes-c-delete-marker"		"D" "m"
	"notes-c-delete-member"		"D" "MuU"
	"notes-c-delete-note"		"D" "nN"

	"notes-c-directory-conferences"	"d" "cC"
	"notes-c-directory-entries"	"d" "eE"
	"notes-c-directory-notes"	"d" "nN"

	"notes-c-extract"		"e" ""

	"notes-c-forward"		"f" ""

	"notes-c-help"			"" "hH?"

	"notes-c-add-marker"		"m" ""

	"notes-c-modify-entry"		"M" "eE"
	"notes-c-modify-keyword"	"M" "kK"
	"notes-c-modify-member"		"M" "MuU"
	"notes-c-set-conference"	"M" "C"
	"notes-c-set-note"		"M" "nN"
	"notes-c-set-profile"		"M" "pP"

	"notes-c-next-note"		"n" "nN"
	"notes-c-next-reply"		"n" "rR"
	"notes-c-next-reply"		">" ""
	"notes-c-next-topic"		"n" "tT"
	"notes-c-next-topic"		"." ""
	"notes-c-next-unseen"		"n" "uU"
	"notes-c-next-unseen"		"" "u"

	"notes-c-open"			"" "o"
	"notes-c-open-if-unseen"	"" "O"
	"notes-c-print"			"" "pP"

	"notes-c-read-note"		"" "r"
	"notes-c-read-last"		"" "R"

	"notes-c-search"		"s" ""

	"notes-c-set-class"		"S" "c"
	"notes-c-set-conference"	"S" "C"
	"notes-c-set-moderator"		"S" "mM"
	"notes-c-set-note"		"S" "nN"
	"notes-c-set-profile"		"S" "pP"
	"notes-c-set-seen"		"S" "sS"
	"notes-c-modify-entry"		"S" "eE"
	"notes-c-modify-keyword"	"S" "kK"
	"notes-c-modify-member"		"S" "uU"

	"notes-c-show-class"		"l" "c"
	"notes-c-show-conference"	"l" "C"
	"notes-c-show-entry"		"l" "e"
	"notes-c-show-errors"		"l" "E"
	"notes-c-show-keywords"		"l" "kK"
	"notes-c-show-marker"		"l" "m"
	"notes-c-show-moderator"	"l" "M"
	"notes-c-show-members"		"l" "uU"
	"notes-c-show-note"		"l" "nN"
	"notes-c-show-profile"		"l" "pP"
	"notes-c-show-version"		"l" "vV"

	"notes-c-update"		"U" ""

	"notes-c-change-view"		"v" ""

	"notes-c-write-note"		"w" ""

	"notes-c-exit"			"" "qQ"

	"notes-c-next-item"		"" "]"
	"notes-c-previous-item"		"" "["
	"argument-prefix"		"/" ""
	"notes-c-continue-command"	"\^m" ""
	"notes-c-enter-key"		"\t" ""
    )
    (if notes-keypad-binding
	(~notes-bind-keys
	    "notes-c-back-reply"	"\(kp1)" ""	; kp1
	    "notes-c-next-topic"	"\(kp2)" ""	; kp2
	    "notes-c-next-reply"	"\(kp3)" ""	; kp3
	    "notes-c-back-topic"	"\(kp5)" ""	; kp5
	    "notes-c-select-key"	"\(kp7)" ""	; kp7
	    "notes-c-next-unseen"	"\(comma)" ""	; kp-,
	    "notes-c-read-last"		"\(minus)" ""	; kp--
	    "notes-c-help"		"\(pf2)" ""	; pf2
	    "notes-c-enter-key"		"\(enter)" ""	; Enter
	)
    )
    (error-occurred (execute-mlisp-file "notes_user.key"))
    (delete-buffer "notes-keymap-hack")
)
[notes.ml]
;
; notes.ml	- VAXnotes user interface
;

;
; All lines of code commented "*debug*" should be remove in the final
; version of the notes interface.
;

;
; Declare variable used by notes
;
(declare-global ~notes-debug)
(if (| ~notes-debug (! (is-bound notes-maximum-sessions)))
    (progn
	(declare-global notes-maximum-sessions)
	(setq notes-maximum-sessions 8)
    )
)
;
; The following set of arrays holds information about the active sessions.
; The arrays are index by the session number
;
(if (| ~notes-debug (! (is-bound notes-notebook-windows)))
    (progn
	~index

	(declare-global
	    notes-notebook-windows
	    notes-notefile-windows
	    notes-mode
	    notes-notebook-last-buffer
	    notes-notefile-last-buffer
	    notes-notefile-is-open
	)
	(setq notes-notebook-windows (array 1 notes-maximum-sessions))
	(setq notes-notefile-windows (array 1 notes-maximum-sessions))
	(setq notes-mode (array 1 notes-maximum-sessions))
	(setq ~index 0)
	(while (< ~index notes-maximum-sessions)
	    (setq-array notes-mode (setq ~index (+ 1 ~index)) "")
	)
	(setq notes-notebook-last-buffer (array 1 notes-maximum-sessions))
	(setq notes-notefile-last-buffer (array 1 notes-maximum-sessions))
	(setq notes-notefile-is-open (array 1 notes-maximum-sessions))
    )
)
(declare-buffer-specific
    ~notes-header-mode-line	; what should be on the mode-line
    ~notes-lines-read
    ~notes-searching
    ~notes-last-read-note-id	; used by read-last command
    ~notes-conference-directory
    notes-number-of-notes-read
    notes-directory-note-id
)
(setq-default ~notes-header-mode-line "[-]\t\t\t\t\t\t\t\t\t     [-]")
(declare-global
    notes-update-seen-map-frequency
    notes-put-form-in-write-buffer
    notes-entry-key-reads-text
    notes-keypad-binding
    notes-note-read-timeout
    notes-current-session
    notes-active-sessions
    notes-users-windows

    notes-form-buffer

    notes-notebook-header
    notes-notebook-entry-details
    notes-notebook-entry-directory
    notes-notebook-conference-directory
    notes-notebook-profile
    notes-notebook-class-directory
    notes-notebook-marker-directory

    notes-notefile-header
    notes-notefile-directory
    notes-notefile-member-directory
    notes-notefile-keyword-directory
    notes-notefile-note
    notes-notefile-new-note
    notes-notefile-note-details
    notes-notefile-conference-details
)
;
;   load code that notes depends on
; 
(execute-mlisp-file "form")

; 
; load the code of the notes interface
; 
(execute-mlisp-file "notes_routines.ml")
(execute-mlisp-file "notes_commands.ml")

; 
; default variables that may be set by the notes_setup.ml
; customisation module
; 
(setq notes-update-seen-map-frequency 10)
(setq notes-put-form-in-write-buffer 0)
(setq notes-keypad-binding 1)
(setq notes-entry-key-reads-text 1)
(setq notes-note-read-timeout 0)
; load user customisation
(error-occurred (execute-mlisp-file "notes_setup.ml"))
; 
; load the key definitions
; 
(execute-mlisp-file "notes.key")


;
; notes
;
; This is the main routine that the user calls.
; It is responsible for setting up the entries in the notes arrays
; and the buffers that are used by a notes session.
; If there is already a session active notes prompts for a session number.
; The default is the first inactive session found in the array.
;
(defun
    (notes
	;
	;   select a session
	;
	(~notes-select-session)
	
	;
	; Now that we have a session number set up the session
	;
	(~notes-setup-session-variables)
	(if (= "" (fetch-array notes-mode notes-current-session))
	    (progn
		;
		; The session is not active yet so set up all the buffers
		; and all the windows.
		;
		
		;
		; setup the buffers
		;
		(~notes-setup-buffers)
		
		(use-variables-of-buffer notes-notebook-header
		    (progn
			;
			; setup the windows
			;
			(setq-array notes-notefile-last-buffer notes-current-session
			    notes-notefile-note)
			(~notes-setup-notefile-windows)
			(setq-array notes-notefile-windows notes-current-session
			    current-windows)
			(setq-array notes-notebook-last-buffer notes-current-session
			    notes-notebook-entry-directory)
			(~notes-setup-notebook-windows)
			(setq-array notes-notebook-windows notes-current-session
			    current-windows)
			
			;
			; Open the notebook - create it if its not found
			;
			(notes-notebook-begin)
			(notes-profile-begin)
			(notes-marker-begin)
			(notes-class-begin)
			(notes-entry-begin)
			
			(notes-profile-get)
			(setq notes-notebook-current-class notes-profile-class-name)
			(~notes-setup-notebook-header)
			
			(setq notes-active-sessions (+ 1 notes-active-sessions))
		    )
		)
		;
		; Start things off with a directory of the notebook
		;
		(notes-c-directory-entries)
		(message "Notes session " notes-current-session " started.")
	    )
	    (progn
		;
		; resume activity where the user left off
		;
		(setq current-windows
		    (if
			(=
			    "notebook"
			    (fetch-array notes-mode notes-current-session)
			)
			(fetch-array notes-notebook-windows
			    notes-current-session)
			(fetch-array notes-notefile-windows
			    notes-current-session)
		    )
		)
		(message "Notes session " notes-current-session " resumed.")
	    )
	)
	(novalue)
    )
)


(defun
    (~notes-select-session
	~default-session
	~index
	~comma
	~current-sessions
	
	(if (< notes-active-sessions 0) (setq notes-active-sessions 0))
	(if notes-active-sessions
	    (progn
		
		;
		; pick a session
		;
		(setq ~index 0)
		(setq ~comma "")
		(setq ~current-sessions "")
		(setq ~default-session 0)
		(while (< (setq ~index (+ 1 ~index)) notes-maximum-sessions)
		    (if (!= (fetch-array notes-mode ~index) "")
			(progn
			    (setq ~current-sessions
				(concat ~current-sessions ~comma ~index))
			    (setq ~comma ",")
			    (if (= ~default-session 0)
				(setq ~default-session ~index)
			    )
			)
		    )
		)
		(if ~notes-debug 	; *debug*
		    (progn
			(message "~current-sessions " ~current-sessions)
			(sit-for 10)))
		(if (= ~default-session 0)
		    (setq ~default-session 1)
		)
		(while
		    (progn ~session-number
			(setq ~session-number
			    (get-tty-string
				(concat
				    ": notes select a session, active sessions ("
				    ~current-sessions
				    ") [" ~default-session "] "
				))
			)
			(if (= ~session-number "")
			    (setq ~session-number ~default-session)
			)
			(setq notes-current-session 
			    (setq ~session-number (+ ~session-number)))
			(&
			    (<= ~session-number 0)
			    (>= ~session-number notes-maximum-sessions)
			)
		    )
		    (novalue)	; all the action is in the expr.
		)
		(if (!= (type-of-expression notes-users-windows) "windows")
		    (setq notes-users-windows current-windows)
		)
	    )
	    ;
	    ; else select session one as the first
	    ;
	    (progn
		(setq notes-current-session 1)
		(setq notes-users-windows current-windows)
	    )
	)
    )
)


(defun ~notes-setup-buf
    (
	~name
	~eof
	~mode (concat "\t\t\t - Emacs  VAXnotes  interface - \t\t\t")
    )
    (save-excursion
	(temp-use-buffer ~name)
	(erase-buffer)
	(setq mode-line-format ~mode)
	(setq display-C1-as-graphics 1)
	(use-local-map "notes-keymap")
	(if (>= ~eof 0) (setq display-end-of-file ~eof))
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	; allow the user to customise the buffer
	(error-occurred (notes-hook-setup-buffer))
    )
)
(defun
    (~notes-setup-buffers
	(~notes-setup-buf notes-form-buffer -1  "Form buffer %68m")
	(~notes-setup-buf notes-notebook-header 0)
	(~notes-setup-buf notes-notebook-entry-details 1 "Entry %74m")
	(~notes-setup-buf notes-notebook-entry-directory 1 "Class %74m")
	(~notes-setup-buf notes-notebook-conference-directory 1 "Conferences at %65m")
	(~notes-setup-buf notes-notebook-profile -1)
	(~notes-setup-buf notes-notebook-class-directory 1)
	(~notes-setup-buf notes-notebook-marker-directory 1)
	(~notes-setup-buf notes-notefile-header 0)
	(~notes-setup-buf notes-notefile-directory 1)
	(~notes-setup-buf notes-notefile-member-directory 1)
	(~notes-setup-buf notes-notefile-keyword-directory 1)
	(~notes-setup-buf notes-notefile-note -1 "[-]\t%69m[-]")
	(save-excursion
	    (temp-use-buffer notes-notefile-new-note)
	    (setq display-C1-as-graphics 1)
	    (text-mode)
	    (setq mode-line-format  "Notes: new-note %64m")
	    (use-local-map "Text-mode-map")
	)
	(~notes-setup-buf notes-notefile-note-details 0)
	(~notes-setup-buf notes-notefile-conference-details 0)
    )
)


(defun
    (~notes-setup-session-variables
	;
	; setup buffer names
	;
	(setq notes-form-buffer
	    (concat "notes-forms-" notes-current-session))
	(setq notes-notebook-header
	    (concat "notebook-header-" notes-current-session))
	(setq notes-notebook-entry-details
	    (concat "notebook-entry-details-" notes-current-session))
	(setq notes-notebook-entry-directory
	    (concat "notebook-entry-directory-" notes-current-session))
	(setq notes-notebook-conference-directory
	    (concat "notebook-conference-directory-" notes-current-session))
	(setq notes-notebook-profile
	    (concat "notebook-profile-" notes-current-session))
	(setq notes-notebook-class-directory
	    (concat "notebook-class-directory-" notes-current-session))
	(setq notes-notebook-marker-directory
	    (concat "notebook-marker-directory-" notes-current-session))
	(setq notes-notefile-header
	    (concat "notefile-header-" notes-current-session))
	(setq notes-notefile-directory
	    (concat "notefile-directory-" notes-current-session))
	(setq notes-notefile-note
	    (concat "notefile-note-" notes-current-session))
	(setq notes-notefile-new-note
	    (concat "notefile-new-note-" notes-current-session))
	(setq notes-notefile-note-details
	    (concat "notefile-note-details-" notes-current-session))
	(setq notes-notefile-conference-details
	    (concat "notefile-conference-details-" notes-current-session))
	(setq notes-notefile-member-directory
	    (concat "notefile-member-directory-" notes-current-session))
	(setq notes-notefile-keyword-directory
	    (concat "notefile-keyword-directory-" notes-current-session))

	;
	; setup context variables
	;
	(setq notes-seen-map notes-current-session)
	(setq notes-profile-context notes-current-session)
	(setq notes-entry-context notes-current-session)
	(setq notes-class-context notes-current-session)
	(setq notes-notefile-context notes-current-session)
	(setq notes-keyword-context notes-current-session)
	(setq notes-marker-context
	    (+ notes-keyword-context notes-maximum-sessions))
	(setq notes-user-context notes-current-session)
	(setq notes-note-context notes-current-session)
	(setq notes-directory-context
	    (+ notes-note-context notes-maximum-sessions))
	(setq notes-search-context
	    (+ notes-directory-context notes-maximum-sessions))
	(setq notes-notebook-context notes-current-session)
	(setq notes-notefile-context
	    (+ notes-note-context notes-maximum-sessions))
	(novalue)
    )
)
(defun
    (~notes-setup-notebook-windows
	(switch-to-buffer notes-notebook-header)
	(delete-other-windows)
	(split-current-window)
	(previous-window)
	(if (! ~notes-debug)
	    (setq window-size 3))
	(next-window)
	(switch-to-buffer
	    (fetch-array notes-notebook-last-buffer notes-current-session))
    )
)
(defun
    (~notes-setup-notefile-windows
	(switch-to-buffer notes-notefile-header)
	(delete-other-windows)
	(split-current-window)
	(previous-window)
	(if (! ~notes-debug)
	    (setq window-size 5))
	(next-window)
	(switch-to-buffer
	    (fetch-array notes-notefile-last-buffer notes-current-session))
    )
)
(defun
    (~notes-setup-notebook-header
	(save-excursion
	    (temp-use-buffer notes-notebook-header)
	    (erase-buffer)
	    (insert-string
		(concat
		    "Current class:  " notes-notebook-current-class
		    "   Notebook:  " notes-notebook-result-spec
		    "\n\t\tType h for help"
		)
	    )
	    (beginning-of-file)
	)

    )
)
; 
; regenerate notefile header
; 
(defun
    (~notes-regenerate-notefile-header
	(~notes-setup-notefile-header)
	(~notes-setup-notefile-note-header)
	(save-window-excursion
	    (switch-to-buffer notes-notefile-note)
	    (~notes-setup-read-note-mode-line)
	)
	(progn ~cur-buf
	    (setq ~cur-buf current-buffer-name)
	    (pop-to-buffer notes-notefile-header)
	    (beginning-of-file)
	    (re-search-forward "-<\\(.*\\)>-")
	    (region-around-match 1)
	    (pop-to-buffer ~cur-buf)
	)
    )
)
(defun ~notes-center-1 (~s)
    ~gap1 ~s-left

    (setq ~s-left (- 80 (length ~s)))
    (if (< ~s-left 2) (setq ~s-left 2))

    (setq ~gap1 (/ ~s-left 2))

    (sys$fao "!#* !AS" "ns" ~gap1 ~s)
)
(defun ~notes-center-2 (~s1 ~s2)
    ~gap

    (setq ~gap (- 80 (length (concat ~s1 ~s2))))
    (if (< ~gap 1) (setq ~gap 1))

    (sys$fao "!AS!#* !AS" "sns" ~s1 ~gap ~s2)
)
(defun ~notes-center-3 (~s1 ~s2 ~s3)
    ~gap1 ~gap2 ~s-left

    (setq ~s-left (- 80 (length (concat ~s1 ~s2 ~s3))))
    (if (< ~s-left 2) (setq ~s-left 2))

    (setq ~gap1 (/ ~s-left 2))
    (setq ~gap2 (- ~s-left ~gap1))

    (sys$fao "!AS!#* !AS!#* !AS" "snsns" ~s1 ~gap1 ~s2 ~gap2 ~s3)
)
;
; notefile header
;
;			-<conference-title>-
; Updated: 10-jan-1986 15:44   999 Topics 1023 notes 6 unseen
; 
(defun
    (~notes-setup-notefile-header
	(save-window-excursion
	    (setq current-windows
		(fetch-array notes-notefile-windows notes-current-session))
	    (pop-to-buffer notes-notefile-header)
	    (erase-buffer)
	    (insert-string
		(~notes-center-1 (concat "-<" notes-notefile-title ">-")))
	    (insert-string
		(sys$fao
"\nUpdated: !17AS  !SL Topic!%S  !SL Note!%S  !SL Unseen  !AS\n"
		    "snnns"
		    notes-notefile-last-revised
		    notes-notefile-number-of-topics
		    notes-notefile-number-of-notes
		    notes-entry-unseen-est
		    (if notes-notefile-moderator-enabled
			"Moderator"
			(if notes-notefile-write-lock
			    "Readonly"
			    ""
			)
		    )
		)
	    )
	    (insert-string "‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘‘\n")
	    (beginning-of-file)
	    (re-search-forward "-<\\(.*\\)>-")
	    (region-around-match 1)
	    (pop-to-buffer
		(fetch-array notes-notefile-last-buffer notes-current-session))
	    (setq-array notes-notefile-windows notes-current-session
		current-windows)
	)
    )
)
(defun
    (~notes-setup-notefile-note-header
	(save-excursion
	    (temp-use-buffer notes-notefile-header)
	    (save-excursion
		(beginning-of-file)
		(next-line) (next-line)
		(beginning-of-line)
		(set-mark) (next-line) (next-line)
		(erase-region)
		(insert-string
		    (~notes-center-1 (concat "‘ " notes-note-topic-title " ‘\n")))
		(insert-string
		    (~notes-center-3
			(concat "Note"
			    (if (!= "" notes-note-conference-file-name) "*" "")
			    " "
			    notes-note-id
			)
			(concat "-=" notes-note-title "=-")
			(if notes-note-is-topic
			    (if
				(= 1 notes-note-number-of-responses)
				"1 reply"
				(= 0 notes-note-number-of-responses)
				"No replies"
				(concat notes-note-number-of-responses " replies")
			    )
			    (concat notes-note-reply-number " of " notes-note-number-of-responses)
			)
		    )
		)
	    )
	)
	(~notes-center-2
	    (concat
		notes-note-author
		(if (length notes-note-pen-name)
		    (concat " \"" notes-note-pen-name "\"") "")
	    )
	    (sys$fao "!SL line!%S !17AS" "ns"
		notes-note-number-of-records
		notes-note-create-time
	    )
	)
    )
)


(defun
    (~notes-setup-read-note-mode-line
	(setq mode-line-format
	    (sys$fao
		"[-] !16<!AS!>!AS!AS!AS  !AS !#* [-]" "sssssn"
		(if (- notes-note-number-of-records ~notes-lines-read)
		    (sys$fao "!SL more line!%S" "n"
			(- notes-note-number-of-records ~notes-lines-read)
		    )
		    "End of note"
		)
		(if notes-note-hidden ", Hidden" "")
		(if notes-note-write-lock ", Write locked" "")
		(if ~notes-searching ", Searching" "")
		notes-notefile-notice
		(~notes-max
		    0
		    (- 54
			(length notes-notefile-notice)
			(* notes-note-hidden 8)
			(* notes-note-write-lock 14)
			(* ~notes-searching 11)
		    )
		)
	    )
	)
    )
)


;
; The following routines maintain the windows saved in the arrays
;
;  ~keys is either "t" text map
;		  "n" no map
;		  "c" command map
(defun ~notes-change-mode (~mode ~buffer ~keymap ~mode-line)
    
    ~cur-mode
    ~cur-windows
    
    (setq ~cur-mode (fetch-array notes-mode notes-current-session))
    (if (!= ~mode ~cur-mode)
	(if
	    (= "notebook" ~cur-mode)
	    (progn ~cur-buf
		(setq-array notes-notebook-windows notes-current-session
		    current-windows)
		(setq current-windows
		    (fetch-array notes-notefile-windows notes-current-session))
		;
		; correct highlight on title
		;
		(setq ~cur-buf current-buffer-name)
		(pop-to-buffer notes-notefile-header)
		(beginning-of-file)
		(re-search-forward "-<\\(.*\\)>-")
		(region-around-match 1)
		(pop-to-buffer ~cur-buf)
	    )
	    (= "notefile" ~cur-mode)
	    (progn
		(setq-array notes-notefile-windows notes-current-session
		    current-windows)
		(setq current-windows
		    (fetch-array notes-notebook-windows notes-current-session))
	    )
	)
    )
    (switch-to-buffer ~buffer)
    (if
	(= "c" ~keymap) (use-local-map "notes-keymap")
	(= "n" ~keymap) (use-local-map "notes-null-keymap")
	(= "t" ~keymap) (use-local-map "Text-mode-map")
    )
    (setq-array notes-mode notes-current-session ~mode)
    (if (!= "" ~mode-line)
	(progn
	    (if (< (length ~mode-line) 80)
		(setq ~mode-line (sys$fao "!80<!AS!>" "s" ~mode-line)))
	    (save-excursion
		(temp-use-buffer
		    (if (= ~mode "notebook")
			notes-notebook-header
			notes-notefile-header
		    )
		)
		(setq mode-line-format ~mode-line)
	    )

	    (use-variables-of-buffer current-buffer-name
		(setq ~notes-header-mode-line ~mode-line)
	    )
	)
    )
    (novalue)
)
(defun ~notes-save-mode()
    (if
	(= "notebook" (fetch-array notes-mode notes-current-session))
	(setq-array notes-notebook-windows notes-current-session
	    current-windows)
	(= "notefile" (fetch-array notes-mode notes-current-session))
	(setq-array notes-notefile-windows notes-current-session
	    current-windows)
    )
    (novalue)
)


; 
; This is the common inner working of the update-entry function.
; It is here as a seperate function because it is used so often
; with in the commands.
; 
(defun ~notes-update-entry-inner (~entry ~seen)

    (setq notes-entry-last-revised notes-notefile-last-revised)
    (setq notes-entry-number-of-topics notes-notefile-number-of-topics)
    (setq notes-entry-number-of-notes notes-notefile-number-of-notes)
    (setq notes-entry-high-uid notes-notefile-high-uid)
    (notes-entry-modify-seen ~entry ~seen)
    (notes-entry-get ~entry)
)


; 
; routines to default an entity
; 
(defun
     (~notes-default-entry
	~name

	(setq ~name "")
	(if (= current-buffer-name notes-notebook-entry-directory)
	    (setq ~name (~notes-select-item))
	    (&
		(= current-buffer-name notes-notebook-class-directory)
		(save-excursion
		    (beginning-of-line)
		    (looking-at ".... ")
		)
	    )
	    (setq ~name (~notes-select-item))
	)
	(if (= "" ~name)
	    (setq ~name notes-current-entry-name)
	)

	~name
     )
)
(defun
     (~notes-default-marker
	~name

	(setq ~name "")
	(if (= current-buffer-name notes-notebook-marker-directory)
	    (setq ~name (~notes-select-item))
	)
	(if (= "" ~name)
	    (setq ~name notes-marker-name)
	)

	~name
     )
)
(defun
    (~notes-default-keyword
	~name
	
	(setq ~name "")
	(if (= current-buffer-name notes-notefile-keyword-directory)
	    (setq ~name (~notes-select-item))
	    (if (= current-buffer-name notes-notefile-note-details)
		(save-excursion
		    (beginning-of-file)
		    (re-search-forward "^‘ Keywords")
		    (beginning-of-line) (next-line)
		    (setq ~name (~notes-select-item))
		)
	    )
	)
	(if (= "" ~name)
	    (setq ~name notes-keyword-name)
	)
	
	~name
    )
)
(defun
    (~notes-default-class
	~name
	
	(setq ~name "")
	(if
	    (= current-buffer-name notes-notebook-class-directory)
	    (save-excursion
		(end-of-line)
		(if (! (error-occurred (re-search-reverse "^....[^ ]")))
		    (progn
			(goto-character (+ (dot) 4))
			(set-mark)
			(if
			    (error-occurred
				(re-search-forward "$\\|  ")
				(if (! (eolp))
				    (progn
					(backward-character)
					(backward-character)
				    )
				)
			    )
			    (end-of-line)
			)
			(setq ~name (region-to-string))
		    )
		)
	    )
	)
	(if (= "" ~name)
	    (setq ~name notes-notebook-current-class)
	)
	
	~name
    )
)
(defun
    (~notes-default-note-id
	~name
	
	(setq ~name "")
	(if (= current-buffer-name notes-notefile-directory)
	    (setq ~name (~notes-select-item))
	    (&
		(= current-buffer-name notes-notefile-keyword-directory)
		(save-excursion
		    (beginning-of-line)
		    (looking-at ". ")
		)
	    )
	    (setq ~name (~notes-select-item))
	)
	(if (= "" ~name)
	    (progn
		notes-marker-name   	; shadow global variable
		
		(setq notes-marker-name "")
		(setq ~name (~notes-default-marker))
		(error-occurred
		    (notes-marker-get ~name)
		    (setq ~name notes-marker-note-id)
		)
	    )
	)
	(if (= "" ~name)
	    (setq ~name notes-note-id)
	)
	
	~name
    )
)
(defun
    (~notes-default-member
	(if (= current-buffer-name notes-notefile-member-directory)
	    (~notes-select-item)
	    ""
	)
    )
)
;
; Utility routines
;
(defun
    (~notes-select-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
		(re-search-forward "[\t ]*")
		(set-mark)
		(if
		    (error-occurred
			(re-search-forward "$\\|  ")
			(if (! (eolp))
			    (progn
				(backward-character) (backward-character)
			    )
			)
		    )
		    (end-of-line)
		)
		(region-to-string)
	    )
	    ""
	)
    )
)
(defun ~notes-change-notefile-state
    (
	~new-state
	~test (+ 0)
    )
    (if ~test (setq ~new-state (! ~new-state)))
    (if (= ~new-state
	    (fetch-array notes-notefile-is-open notes-current-session))
	(if ~new-state
	    (error-message "You have already opened a conference")
	    (error-message "You have not opened a conference yet")
	)
	(if (! ~test)
	    (setq-array notes-notefile-is-open notes-current-session ~new-state)
	)
    )
)
(defun ~notes-map-note-id (~note-id)
    (if (error-occurred (notes-marker-get ~note-id))
	~note-id
	notes-marker-note-id
    )
)
(defun ~notes-add-keywords (~keywords ~note-id)
    ~index ~keyword ~failed ~comma

    (setq ~failed "")
    (setq ~comma "")
    (setq ~index 0)
    (while
	(progn
	    (setq ~keyword (~notes-element ~keywords "," ~index))
	    (!= ~keyword "")
	)
	(progn
	    (message "Adding keyword " ~keyword " to note " ~note-id " ...")
	    (sit-for 0)
	    (if (error-occurred (notes-keyword-add ~keyword ~note-id))
		(progn
		    (setq ~failed (concat ~failed ~comma ~keyword))
		    (setq ~comma ", ")
		)
	    )
	    (setq ~index (+ 1 ~index))
	)
    )
    (if (length ~failed)
	(error-message "Failed to add the following keywords " ~failed)
    )
)
(defun ~notes-delete-keywords (~keywords ~note-id)
    ~index ~keyword

    (setq ~index 0)
    (while
	(progn
	    (setq ~keyword (~notes-element ~keywords "," ~index))
	    (!= ~keyword "")
	)
	(progn
	    (if (= ~note-id "")
		(message "Deleting keyword " ~keyword "...")
		(message "Deleting keyword " ~keyword " to note " ~note-id " ...")
	    )
	    (sit-for 0)
	    (notes-keyword-delete ~keyword ~note-id)
	    (setq ~index (+ 1 ~index))
	)
    )
)


(defun ~notes-max (~value1 ~value2)
    (if (> ~value1 ~value2) ~value1 ~value2)
)


;
; end of module
;
[notes.mlp]
(defun
    (notes-com
	(notes)
    )
)
[notes_commands.ml]
;
; notes_commands.ml
;
;
; All the routines in this file directory implement a command
;
(defun
    (notes-c-add-entry
	~filename ~entry ~classes ~index ~class
	~user-area ~personal-name	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~filename
		    (if prefix-argument-provided
			(region-to-string)
			(= current-buffer-name notes-notebook-conference-directory)
			(progn
			    (end-of-line)
			    (goto-character
				(save-excursion
				    (re-search-reverse "^[ \200][^ ]")
				    (dot)
				)
			    )
			    (concat ~notes-conference-directory (~notes-select-item))
			)
			notes-note-conference-file-name
		    )
		)
		(sys$filescan ~filename fscn$_name ~entry)
		(setq ~class (~notes-default-class))
		(if
		    (form-fill-form
			notes-form-buffer	"Add entry to notebook"
			(concat "Add entry " ~entry " to classes " ~classes)
			"y"
			(novalue) 1
			; fields to fill
			"Notefile name:" ~filename "s" (if (= (setq ~filename ~result) "")
							   (error-message "you must specify a filename"))
			"Entry name:" ~entry "s"
			(setq ~entry
			    (progn
				(if (= ~result "")
				    (sys$filescan ~filename fscn$_name ~result))
				~result
			    )
			)
			"Classes:" ~class "s" (setq ~classes ~result)
			"Personal name:" "" "s" (setq ~personal-name ~result)
		    )
		    (progn
			(setq ~user-area (concat ~personal-name))
			(setq ~index 0)
			(while
			    (progn
				(setq ~class (~notes-element ~classes "," ~index))
				(!= ~class "")
			    )
			    (progn
				(message "Adding entry " ~entry " in class " ~class " for file " ~filename)
				(sit-for 0)
				(error-occurred (notes-class-add ~class))
				(notes-entry-add ~entry ~filename ~class ~user-area 1)
				(setq ~index (+ 1 ~index))
			    )
			)
		    )
		)
		(if
		    (= current-buffer-name notes-notebook-entry-directory)
		    (notes-c-directory-entries)
		    (= current-buffer-name notes-notebook-class-directory)
		    (notes-c-show-class)
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-add-keyword
	~note-id ~keywords
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(setq ~note-id (~notes-default-note-id))
		(setq ~keywords (~notes-default-keyword))
		
		(if
		    (form-fill-form
			notes-form-buffer	"Add keyword to note"
			(concat "Add keyword " ~keywords " to note " ~note-id)
			"y"
			(novalue) 1
			; fields to fill
			"Note id:" ~note-id "s" (setq ~note-id (~notes-map-note-id ~result))
			"Keywords:" ~keywords "s" (setq ~keywords ~result)
		    )
		    (~notes-add-keywords ~keywords ~note-id)
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-add-marker
	~entry ~marker ~note-id
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; get the entry name from the directory list
		;
		(setq ~entry (~notes-default-entry))
		(setq ~marker "")
		(setq ~note-id (~notes-default-note-id))
		(if
		    (form-fill-form
			notes-form-buffer	"Add note marker"
			(concat "Add marker " ~marker " for note " ~note-id)
			"y"
			(novalue) 1
			; fields to fill
			"Note id:" ~note-id "s" (setq ~note-id (~notes-map-note-id ~result))
			"Marker:" ~marker "s" (if (= (setq ~marker ~result) "")
						  (error-message "You must specify a marker name"))
			"Entry:" ~entry "s" (setq ~entry ~result)
		    )
		    (notes-marker-add ~marker ~note-id ~entry)
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-add-member
	~name ~access-list ~keyword ~moderate ~mail-addr
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(if (! notes-notefile-moderator-enabled)
		    (error-message "You are not enabled as a moderator"))
		
		(~notes-change-notefile-state 1 1)
		
		(if
		    (form-fill-form
			notes-form-buffer	"Add member"
			(concat "Add member " ~name)
			"y"
			(novalue) 1
			; fields to fill
			"Username:" "" "s" (if (=
						   (setq ~name
						       (case-string-upper ~result)) "")
					       (error-message "You must specify a username"))
			"Access list:" "" "s" (setq ~access-list (case-string-upper ~result))
			"Create keywords:" 0 "b" (setq ~keyword ~result)
			"Moderator:" 0 "b" (setq ~moderate ~result)
			"Mail address:" "" "s" (setq ~mail-addr ~result)
		    )
		    (progn
			(notes-user-add ~name ~access-list ~keyword ~moderate ~mail-addr)
			(if (= current-buffer-name notes-notefile-member-directory)
			    (notes-c-show-members))
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-answer
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; check that a notefile is open and change to notefile mode
		;
		(~notes-change-notefile-state 1 1)
		(~notes-change-mode "notefile" notes-notefile-note "c" "")
		
		(save-window-excursion
		    (~notes-write-buffer 1 (concat "Reply to note " notes-note-id "\t(%m)"))
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-back-note
	
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_back_note 0 notes$k_note_back_response)
	)
	(novalue)
    )
)
(defun
    (notes-c-back-reply
	
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_back_response)
	)
	(novalue)
    )
)
(defun
    (notes-c-back-topic
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_back_note)
	)
	(novalue)
    )
)
(defun
    (notes-c-close
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		;   Check to see if the notesfile is closed
		;
		(~notes-change-notefile-state 0)
		;
		; close the file and update the notebook entry for it
		;
		(notes-user-end)
		(notes-keyword-end)
		(notes-note-end notes-note-context)
		(notes-note-end notes-directory-context)
		(notes-note-end notes-search-context)
		
		;
		; retrive the seen map and other info
		;
		(if
		    (! (error-occurred (notes-notefile-end notes-seen-map)))
		    (if (!= notes-current-entry-name "")
			(progn
			    ;
			    ; get the latest entry info for the update
			    ;
			    (notes-entry-get)
			    (setq notes-entry-last-status 1)
			    (~notes-update-entry-inner
				notes-current-entry-name notes-seen-map)
			)
		    )
		    (message error-message)
		)
		;
		; change mode back to notebook directory
		;
		(notes-c-directory-entries)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-continue-command
	~arg-provided ~arg
	
	(if prefix-argument-provided
	    (progn
		(setq ~arg-provided prefix-argument-provided)
		(setq ~arg prefix-argument)
	    )
	)
	
	(use-variables-of-buffer notes-notebook-header
	    ;
	    ; based on the corrent buffer name continue what the buffer does
	    ;
	    (if
		(= notes-notefile-directory current-buffer-name)
		(~notes-directory-note-continue 0
		    (if ~arg-provided 2000000000 (- window-size 2))
		)
		(= notes-notefile-note current-buffer-name)
		(~notes-read-note-continue ~arg-provided ~arg)
		(error-message "Unable to continue operation in "
		    current-buffer-name)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-create-conference
	~file ~title ~restrict-members ~create-keywords
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		;   Check to see if the notesfile is open
		;
		(~notes-change-notefile-state 0 1)
		
		(if
		    (form-fill-form
			notes-form-buffer	"Create conference"
			(concat "Create conference " ~file)
			"y"
			(novalue) 1
			; fields to fill
			"Notefile name:" "" "s" (setq ~file ~result)
			"Conference title:" "" "s" (setq ~title ~result)
			"Restrict members:" 0 "b" (setq ~restrict-members ~result)
			"Create keywords:" 0 "b" (setq ~create-keywords ~result)
		    )
		    (progn
			(notes-notefile-create ~file ~title ~restrict-members ~create-keywords)
			(message "Conference created")
			
			;
			;   Change state to open
			;
			(~notes-change-notefile-state 1)
			(setq notes-current-entry-name "")
			
			(notes-user-begin)
			(notes-keyword-begin)
			(notes-note-begin notes-note-context)
			(notes-note-begin notes-directory-context)
			(notes-note-begin notes-search-context)
			(setq notes-note-id "1.0")
			(setq notes-number-of-notes-read 0)
			
			;
			; setup the notefile header
			;
			(~notes-setup-notefile-header)
			(~notes-change-mode "notefile" notes-notefile-note "c" notes-notefile-notice)
			(erase-buffer)
			
			;
			;   Write the introductory note
			;
			(notes-c-write-note)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-create-keyword
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(notes-keyword-add (get-tty-string "Create keyword (keyword) "))
	    )
	)
	
	(novalue)
    )
)
(defun
    (notes-c-delete-entry
	~entry ~class
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		; 
		; get the entry name from the directory list
		;
		(setq ~entry (~notes-default-entry))
		(setq ~class (~notes-default-class))
		
		(if
		    (form-fill-form
			notes-form-buffer	"Delete entry"
			(concat "Delete " ~entry " from class " ~class)
			"n"
			(novalue) 1
			; fields to fill
			"Entry:" ~entry "s" (setq ~entry ~result)
			"Class:" ~class "s" (setq ~class ~result)
		    )
		    (progn
			(notes-entry-delete ~entry ~class)
			(notes-c-directory-entries)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-delete-keyword
	~keywords ~note-id
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(setq ~keywords (~notes-default-keyword))
		(setq ~note-id (~notes-default-note-id))
		
		(if
		    (form-fill-form
			notes-form-buffer	"Delete keyword"
			(if (= "" ~note-id)
			    (concat "Delete keyword " ~keywords)
			    (concat "Delete keyword " ~keywords " from " ~note-id)
			)
			"n"
			(novalue) 1
			; fields to fill
			"Note id:" ~note-id "s" (setq ~note-id (~notes-map-note-id ~result))
			"Delete keywords:" ~keywords "s" (setq ~keywords ~result)
		    )
		    (~notes-delete-keywords ~keywords ~note-id)
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-delete-marker
	~entry ~marker
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; get the entry name from the directory list
		;
		(setq ~entry (~notes-default-entry))
		(setq ~marker (~notes-default-marker))
		
		(if
		    (form-fill-form
			notes-form-buffer	"Delete note marker"
			(concat "Delete marker " ~marker)
			"y"
			(novalue) 1
			; fields to fill
			"Marker:" ~marker "s" (setq ~marker ~result)
			"Entry:" ~entry "s" (setq ~entry ~result)
		    )
		    (notes-marker-delete ~marker ~entry)
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-delete-member
	~name
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(if (! notes-notefile-moderator-enabled)
		    (error-message "You are not enabled as a moderator"))
		
		(~notes-change-notefile-state 1 1)
		
		(setq ~name (~notes-default-member))
		(if (= ~name "") (setq ~name (get-tty-string "Delete member: ")))
		
		(notes-user-get ~name)
		
		(if (form-confirm-action
			(concat "Delete member " notes-user-name) "n")
		    (progn
			(notes-user-delete notes-user-name)
			(if (= current-buffer-name notes-notefile-member-directory)
			    (notes-c-show-members))
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-delete-note
	~note-id
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		;
		; get the entry name from the directory list
		;
		(setq ~note-id (~notes-default-note-id))
		(if
		    (form-fill-form
			notes-form-buffer	"Delete note"
			(concat "Delete note " ~note-id)
			"n"
			(novalue) 1
			; fields to fill
			"Note id:			" ~note-id "s" (setq ~note-id (~notes-map-note-id ~result))
		    )
		    (notes-note-delete ~note-id)
		)
	    )
	)
	(novalue)
    )
)
(defun notes-c-directory-conferences
    (
	~files (get-tty-string "Directory of conferences [notes$library:] ")
    )
    ~node ~dev ~dir ~name
    
    
    (use-variables-of-buffer notes-notebook-header
	(progn
	    ;
	    ; change mode
	    ;
	    (~notes-change-mode "notebook" notes-notebook-conference-directory "c"
		" Conference name                         Status"
		; name
	    )
	    (notes-notefile-list-begin ~files 31)
	    (erase-buffer)
	    (sys$filescan ~files
		fscn$_node ~node
		fscn$_device ~dev
		fscn$_directory ~dir
	    )
	    (setq ~notes-conference-directory (concat ~node ~dev ~dir))
	    
	    (while
		(progn
		    (notes-notefile-list-next 31)
		    (&
			(!= notes-status 12)
			(!= notes-status notes$_nmf)
		    )
		)
		(progn
		    (sys$filescan notes-notefile-result-spec
			fscn$_node ~node
			fscn$_device ~dev
			fscn$_directory ~dir
			fscn$_name ~name
		    )
		    (setq mode-string (concat ~node ~dev ~dir))
		    (if (& notes-status 1)
			(insert-string
			    (sys$fao " !39AS \n   Title:  !AS\n   Notice: !AS\n"
				"sss"
				~name
				notes-notefile-title
				notes-notefile-notice
			    )
			)
			(insert-string
			    (sys$fao " !39AS !AS\n"
				"ss"
				~name
				(sys$getmsg notes-status 1)
			    )
			)
		    )
		    (sit-for 0)
		)
	    )
	    (beginning-of-file)
	)
    )
    (novalue)
)
(defun
    (notes-c-directory-entries
	~cur-entry
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; change mode
		;
		(~notes-change-mode "notebook" notes-notebook-entry-directory "c"
		    " Entry name           Unseen   Last new note     Topics  Last update status"
		    ; 20-character-name         1   10-Jan-1986 10:53      1  Whatever went wrong
		)
		(setq mode-string notes-notebook-current-class)
		;
		; remember the current entry that the user has selected
		;
		(setq ~cur-entry (~notes-default-entry))
		;
		; Get buffer ready for enties
		;
		(erase-buffer)
		;
		; get class and then entries
		;
		(notes-class-get)
		(notes-class-get-entry)
		(while
		    (progn
			(notes-entry-get notes-class-entry-name)
			(insert-string
			    (sys$fao " !#AS !5SL   !17AS  !5SL  !AS\n" "nsnsns"
				(if (> (length notes-entry-name) 21)
				    (length notes-entry-name)
				    21
				)
				notes-entry-name
				notes-entry-unseen-est
				notes-entry-last-revised
				notes-entry-number-of-topics
				(if (|
					(& notes-entry-last-status 1)
					(= notes-entry-last-status 0)
				    )
				    ""
				    (sys$getmsg notes-entry-last-status 1)
				)
			    )
			)
			(end-of-file)
			(notes-class-get-entry-next)
		    )
		    (novalue)
		)
		(beginning-of-file)
		(error-occurred
		    (re-search-forward (concat "^." ~cur-entry))
		)
		(~notes-select-item)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-directory-notes
	~complex-dir
	~arg
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~complex-dir prefix-argument-provided)
		(setq ~arg prefix-argument)
		
		;
		;   Check to see if the notesfile is open
		;
		(~notes-change-notefile-state 1 1)
		
		;
		; change mode
		;
		(~notes-change-mode "notefile" notes-notefile-directory "c"
		    " Topic       Author               Date        Repl  Title                     "
		)
		;
		; get buffer ready
		;
		(erase-buffer)
		(if ~complex-dir
		    (progn
			(if
			    (~notes-fill-select-notes-form "Directory of notes" "Start directory of " "last-1")
			    
			    (~notes-directory-note-continue 1
				(if (> 4 ~arg) 2000000000 (- window-size 2))
			    )
			)
		    )
		    (~notes-directory-note-continue -1 (- window-size 2) "last-1")
		)
	    )
	)
	(novalue)
    )
)
(defun
    ~notes-directory-note-continue
    (
	~start (+ 0)
	~num-lines (- window-size 2)
	~start-id (concat "")
    )

    ;
    ; either start a directory of continue one
    ;
    (end-of-file)
    (line-to-top-of-window)
    (save-window-excursion
	(while
	    (if
		(> ~num-lines 0)
		(notes-note-search ~start 0 0 ~start-id notes-directory-context)
	    )
	    (progn
		(if (! (& notes-status 1)) (~notes-error-message))
		(insert-string
		    (if notes-note-is-topic
			(sys$fao
			    " !5SL       !20AS !11AS  !3SL  !AS\n" "nssns"
			    (~notes-element notes-note-id "." 0)
			    notes-note-author
			    notes-note-create-time
			    notes-note-number-of-responses
			    notes-note-topic-title
			)
			(sys$fao
			    " !10<!5SL.!SL!>  !20AS !11AS       !AS\n" "nnsss"
			    (~notes-element notes-note-id "." 0)
			    (~notes-element notes-note-id "." 1)
			    notes-note-author
			    notes-note-create-time
			    notes-note-title
			)
		    )
		)
		(setq ~num-lines (- ~num-lines 1))
		(setq ~start 0)
		(sit-for 0)
	    )
	)
    )
    (if (! (eobp))
	(~notes-select-item)
    )
    (novalue)
)
(defun ~notes-read-note-continue (~read-the-lot)
    (if (- notes-note-number-of-records ~notes-lines-read)
	(if ~read-the-lot
	    (notes-note-read notes-note-number-of-records notes-note-context notes-note-read-timeout)
	    (progn
		(end-of-file)
		(error-occurred
		    (previous-line) (previous-line)
		)
		(line-to-top-of-window)
		(notes-note-read (- window-size 3) notes-note-context notes-note-read-timeout)
		(end-of-file)
		(provide-prefix-argument (- window-size 1) (previous-line))
		(line-to-top-of-window)
	    )
	)
	(progn
	    (next-page)
	    (end-of-window)
	    (if (eobp)
		(progn
		    (beginning-of-line)
		    (previous-line (- window-size 1))
		    (line-to-top-of-window)
		)
		(beginning-of-window)
	    )
	)
    )
    (~notes-setup-read-note-mode-line)
)
;
; Exit the session closing all files and ending all contexts
;
(defun
    (notes-c-end-session
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq notes-active-sessions (- notes-active-sessions 1))
		(setq-array notes-mode notes-current-session "")
		;
		; close down contexts
		;
		(notes-entry-end)
		(notes-class-end)
		(notes-profile-end)
		(notes-marker-end)
		(notes-notebook-end)
		;
		; get back to orginal windows
		;
		(setq current-windows notes-users-windows)
	    )
	)
    )
)
;
; Based on the current note reading context do the next step in reading the
; conference.
;
; If there is unread text in the note read it.
; If There are more replies read the next.
; If not do a next-unseen.
;
(defun
    (notes-c-enter-key
	(use-variables-of-buffer notes-notebook-header
	    (if (= current-buffer-name notes-notefile-note)
		(if
		    (&
			notes-entry-key-reads-text
			(! notes-note-hidden)
			(!= notes-note-number-of-records ~notes-lines-read)
		    )
		    (~notes-read-note-continue 0)
		    (progn
			(if (!= notes-note-reply-number notes-note-number-of-responses)
			    (notes-c-next-reply)
			    (notes-c-next-unseen)
			)
		    )
		)
		(message "Enter only works in the note buffer.")
	    )
	)
    )
)
(defun
    (notes-c-extract
	~file ~buffer ~seen ~header
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(if
		    (form-fill-form
			
			notes-form-buffer	"Extract notes"
			(concat "Extract " notes-note-search-note-id) "y"
			(novalue) 1
			; fields
			"Note-id:" (~notes-default-note-id) "s" (setq notes-note-search-note-id ~result)
			"Mark as seen:" 0 "b" (setq ~seen ~result)
			"Include header:" 1 "b" (setq ~header ~result)
			"File:" "" "s" (setq ~file ~result)
			"Buffer:" "notefile-extract" "s" (setq ~buffer ~result)
			"Title:" notes-note-search-title "s" (setq notes-note-search-title ~result)
			"String:" notes-note-search-string "s" (setq notes-note-search-string ~result)
			"Author:" notes-note-search-author "s" (setq notes-note-search-author ~result)
			"Since:" notes-note-search-since "s" (setq notes-note-search-since ~result)
			"Before:" notes-note-search-before "s" (setq notes-note-search-before ~result)
			"Unseen:" notes-note-search-unseen "b" (setq notes-note-search-unseen ~result)
			"Keyword:" notes-note-search-keyword "s" (setq notes-note-search-keyword ~result)
		    )
		    (progn	
			(~notes-extract-to-buffer ~buffer ~seen ~header)
			(if
			    (!= ~file "")
			    (save-window-excursion
				(switch-to-buffer ~buffer)
				(write-named-file ~file)
				(setq current-buffer-type "scratch")
			    )
			)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun ~notes-extract-to-buffer (~buffer ~seen ~header)
    ~start
    (save-window-excursion
	(notes-note-begin 31)
	
	(setq ~start 1)
	(temp-use-buffer ~buffer)
	(erase-buffer)
	(if ~header (~notes-insert-notefile-header))
	(while
	    (notes-note-search ~start 1 ~seen ""  31)
	    (progn
		(if notes-note-is-topic
		    (if (! ~start) (insert-string "\^l\n"))
		    (insert-string "\n")
		)
		(setq ~start 0)
		(message "extracting " notes-note-id "...") (sit-for 0)
		(if ~header (~notes-insert-note-header))
		(if (! notes-note-hidden)
		    (notes-note-read 999999 31)
		)
		(end-of-file)
	    )
	)
	(notes-note-end 31)
    )
)
(defun
    (~notes-insert-notefile-header
	(insert-string
	    (concat
		(~notes-center-1
		    (concat "<<< " notes-notefile-result-spec " >>>")) "\n"
		(~notes-center-1
		    (concat "-<  " notes-notefile-title "  >-")) "\n"
	    )
	)
    )
)
(defun
    (~notes-insert-note-header
	(insert-string
	    (concat
		"================================================================================\n"
		(~notes-center-3
		    (concat "Note " notes-note-id)
		    notes-note-topic-title
		    (if notes-note-is-topic
			(if notes-note-number-of-responses
			    (if (= notes-note-number-of-responses 1)
				"1 reply"
				(concat notes-note-number-of-responses " replies")
			    )
			    "No replies"
			)
			(concat notes-note-reply-number " of " notes-note-number-of-responses)
		    )
		)
		"\n"
		(~notes-center-2
		    (concat notes-note-author
			(if (!= notes-note-pen-name "")
			    (concat " \"" notes-note-pen-name "\"") ""))
		    (sys$fao "!SL line!%S !17AS" "ns"
			notes-note-number-of-records
			notes-note-create-time
		    )
		)
		"\n"
		(if (! notes-note-is-topic)
		    (concat
			(~notes-center-1 (concat "-< " notes-note-title " >-"))
			"\n"
		    )
		    ""
		)
		"--------------------------------------------------------------------------------\n"
	    )
	)
    )
)

(defun
    (notes-c-forward
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(message "Command not implmented " (char-to-string (last-key-struck)))
	    )
	)
	(novalue)
    )
)
(error-occurred (extend-database-search-list
		    "notes_help" "EMACS$HELP:EMACS$NOTES_HELP.HLB" 1))
(autoload "~help-main" "help")
(defun
    (notes-c-help
	(save-window-excursion
	    ~help-database ~help-topic
	
	    (setq ~help-database "notes_help")
	    (setq ~help-topic "")
	    (~help-main)
	    (push-back-character "d")
	    (recursive-edit)
	    (setq ~help-database "help")
	    (~help-t)
	)
	(novalue)
    )
)
(defun
    (notes-c-modify-entry
	~entry ~filename ~entry-new-name ~classes ~new-classes ~class
	~delete-list ~add-list ~index
	~user-area ~personal-name
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; get the entry name from the directory list
		;
		(setq ~entry (~notes-default-entry))
		(notes-entry-get ~entry)
		(setq ~classes "")
		(while (notes-entry-get-class-next)
		    (setq ~classes (concat ~classes "," notes-entry-class-name))
		)
		(setq ~classes (substr ~classes 2 999))
		(setq ~personal-name (~notes-element notes-entry-user-area "\^a" 0))
		(if
		    (form-fill-form
			notes-form-buffer	"Modify entry"
			(concat "Modify entry " ~entry)
			"y"
			(novalue) 1
			; fields to fill
			"Entry name:" ~entry "s" (setq ~entry ~result)
			"New entry name:" ~entry "s" (setq ~entry-new-name ~result)
			"Notefile name:" notes-entry-file-name "s" (setq ~filename ~result)
			"Classes:" ~classes "s" (setq ~new-classes ~result)
			"Personal name:" ~personal-name "s" (setq ~personal-name ~result)
		    )
		    (progn
			(setq ~user-area (concat ~personal-name))
			(if
			    (|
				(!= ~filename notes-entry-file-name)
				(!= ~entry-new-name ~entry)
				(!= ~user-area notes-entry-user-area)
			    )
			    (progn
				(notes-entry-modify
				    ~entry
				    (if (= ~filename notes-entry-file-name) "" ~filename)
				    (if (= ~entry-new-name ~entry) "" ~entry-new-name)
				    ~user-area (!= ~user-area notes-entry-user-area)
				)
				(if (= ~entry notes-current-entry-name)
				    (setq notes-current-entry-name ~entry-new-name)
				)
			    )
			)
			;
			; calculate add and delete lists
			;
			(setq ~add-list (~notes-list-diff ~new-classes ~classes ","))
			(setq ~delete-list (~notes-list-diff ~classes ~new-classes ","))
			;
			; add entry to new classes
			;
			(setq ~index 0)
			(while
			    (progn
				(setq ~class (~notes-element ~add-list "," ~index))
				(!= ~class "")
			    )
			    (progn
				(message "Adding entry " ~entry-new-name " in class " ~class " for file " ~filename)
				(sit-for 0)
				(error-occurred (notes-class-add ~class))
				(notes-entry-add ~entry-new-name "" ~class "" 0)
				(setq ~index (+ 1 ~index))
			    )
			)
			;
			; delete entry from old classes
			;
			(setq ~index 0)
			(while
			    (progn
				(setq ~class (~notes-element ~delete-list "," ~index))
				(!= ~class "")
			    )
			    (progn
				(message "Deleting entry " ~entry-new-name " from class " ~class " for file " ~filename)
				(sit-for 0)
				(notes-entry-delete ~entry-new-name ~class)
				(setq ~index (+ 1 ~index))
			    )
			)
			(notes-c-directory-entries)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-modify-keyword
	~keyword ~keyword-new-name
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		;
		; get the keyword name from the directory list
		;
		(setq ~keyword (~notes-default-keyword))
		(if
		    (form-fill-form
			notes-form-buffer	"Modify keyword"
			(concat "Modify keyword " ~keyword)
			"y"
			(novalue) 1
			; fields to fill
			"Keyword name:" ~keyword "s" (setq ~keyword ~result)
			"New keyword name:" ~keyword "s" (setq ~keyword-new-name ~result)
		    )
		    (use-local-map "notes-keymap")
		)
		(notes-keyword-modify ~keyword ~keyword-new-name)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-modify-member
	~access-list ~keyword ~moderate ~mail-addr
	~name
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(if (! notes-notefile-moderator-enabled)
		    (error-message "You are not enabled as a moderator"))
		
		(~notes-change-notefile-state 1 1)
		
		(setq ~name (~notes-default-member))
		(if (= ~name "") (setq ~name (get-tty-string "Modify member: ")))
		
		(notes-user-get ~name)
		
		(if
		    (form-fill-form
			notes-form-buffer	(concat "Modify member " notes-user-name)
			(concat "Modify member " notes-user-name)
			"n"
			(novalue) 1
			; fields to fill
			"Access list:" notes-user-access-list "s" (setq ~access-list ~result)
			"Create keywords:" notes-user-create-keyword "b" (setq ~keyword ~result)
			"Moderator:" notes-user-moderate "b" (setq ~moderate ~result)
			"Mail address:" notes-user-mail-addr "s" (setq ~mail-addr ~result)
		    )
		    (progn
			(notes-user-modify
			    notes-user-name
			    ~access-list	(!= notes-user-access-list ~access-list)
			    ~keyword	(!= notes-user-create-keyword ~keyword)
			    ~moderate	(!= notes-user-moderate ~moderate)
			    ~mail-addr	(!= notes-user-mail-addr ~mail-addr)
			)
			(if (= current-buffer-name notes-notefile-member-directory)
			    (notes-c-show-members))
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-next-note
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_next_note 0 notes$k_note_next_response)
	)
	(novalue)
    )
)
(defun
    (notes-c-next-reply
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_next_response)
	)
	(novalue)
    )
)
(defun
    (notes-c-next-topic
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_next_note)
	)
	(novalue)
    )
)
(defun
    (notes-c-next-unseen
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_next_unseen 0 notes$k_note_next_note 0)
	)
	(novalue)
    )
)
(defun notes-c-open (~do-auto (+ 1))
    ~prompt
    ~entry
    
    
    (use-variables-of-buffer notes-notebook-header
	(progn
	    (setq ~prompt prefix-argument-provided)
	    
	    ;
	    ;   Check to see if the notesfile is open
	    ;
	    (~notes-change-notefile-state 0 1)
	    
	    ;
	    ; get the entry name from the directory list
	    ;
	    (setq ~entry (~notes-default-entry))
	    (if
		(|
		    ~prompt
		    (= ~entry "")
		)
		(setq ~entry (get-tty-string "Open entry: "))
	    )
	    
	    ;
	    ; do a full fetch of the entry to get the seen map
	    ;
	    (notes-entry-get ~entry notes-seen-map)
	    (setq notes-current-entry-name notes-entry-name)
	    
	    ;
	    ; open the notefile
	    ;
	    (message "Opening conference " notes-entry-file-name)
	    (if (interactive) (sit-for 0))
	    (notes-notefile-begin notes-entry-file-name notes-seen-map)
	    
	    ;
	    ;   Change state to open
	    ;
	    (~notes-change-notefile-state 1)
	    
	    ;
	    ;	erase all the buffers used to hold notefile data so that
	    ;	old data is removed from view
	    ;
	    (save-excursion
		(set-mark)
		(delete-region-to-buffer notes-notefile-directory)
		(delete-region-to-buffer notes-notefile-note-details)
		(delete-region-to-buffer notes-notefile-conference-details)
		(delete-region-to-buffer notes-notefile-member-directory)
		(delete-region-to-buffer notes-notefile-keyword-directory)
		(delete-region-to-buffer notes-notefile-note)
	    )
	    
	    ;
	    ; Update the unseen estimate for the first display of the
	    ; notefile header
	    ;
	    (~notes-update-entry-inner notes-current-entry-name 0)
	    
	    ;
	    ; create all the associated contexts
	    ;
	    (notes-user-begin)
	    (notes-keyword-begin)
	    (notes-note-begin notes-note-context)
	    (notes-note-begin notes-directory-context)
	    (notes-note-begin notes-search-context)
	    (setq notes-note-id "1.0")
	    (setq notes-number-of-notes-read 0)
	    
	    ;
	    ; setup the notefile header
	    ;
	    (~notes-setup-notefile-header)
	    (~notes-change-mode "notefile" notes-notefile-note "c" notes-notefile-notice)
	    (erase-buffer)
	    
	    ;
	    ; look in the profile to see what should be done now the
	    ; file is open
	    ;
	    (setq mode-string notes-notefile-notice)
	    (if ~do-auto
		(progn
		    (notes-profile-get)
		    (if
			notes-profile-auto-dir
			(notes-c-directory-notes)
			notes-profile-auto-unseen
			(notes-c-next-unseen)
		    )
		)
	    )
	)
    )
    (novalue)
)
(defun
    (notes-c-open-if-unseen
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(notes-c-open 0)
		(if (= notes-entry-unseen-est 0)
		    (progn
			(notes-c-close)
			(message "no unseen notes in " notes-notefile-result-spec)
		    )
		    (progn
			(notes-profile-get)
			(if
			    notes-profile-auto-dir
			    (notes-c-directory-notes)
			    notes-profile-auto-unseen
			    (notes-c-next-unseen)
			)
		    )
		)
	    )
	)
    )
)
(defun
    (notes-c-print
	~file ~print-options ~seen ~header
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(if
		    (form-fill-form
			notes-form-buffer	"Print notes"
			(concat "Print " notes-note-search-note-id) "y"
			(novalue) 1
			; fields
			"Note-id:" (~notes-default-note-id) "s" (setq notes-note-search-note-id ~result)
			"Include header:" 1 "b" (setq ~header 1)
			"Mark as seen:" 0 "b" (setq ~seen ~result)
			"Print options:" notes-profile-print "s" (setq ~print-options ~result)
			"Title:" notes-note-search-title "s" (setq notes-note-search-title ~result)
			"String:" notes-note-search-string "s" (setq notes-note-search-string ~result)
			"Author:" notes-note-search-author "s" (setq notes-note-search-author ~result)
			"Since:" notes-note-search-since "s" (setq notes-note-search-since ~result)
			"Before:" notes-note-search-before "s" (setq notes-note-search-before ~result)
			"Unseen:" notes-note-search-unseen "b" (setq notes-note-search-unseen ~result)
			"Keyword:" notes-note-search-keyword "s" (setq notes-note-search-keyword ~result)
		    )
		    
		    (progn
			(~notes-extract-to-buffer "notes-extract" ~seen ~header)
			(save-window-excursion
			    (switch-to-buffer "notes-extract")
			    (setq ~file (write-named-file "sys$scratch:emacs_notes.tmp"))
			    (setq current-buffer-type "scratch")
			)
			(print-file ~file (concat "/delete" ~print-options))
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-read-last
	(use-variables-of-buffer notes-notebook-header
	    (~notes-read-note notes$k_note_id
		(save-excursion
		    (temp-use-buffer notes-notefile-note)
		    ~notes-last-read-note-id
		)
	    )
	)
    )
)
(defun
    (notes-c-read-note
	~ask-user ~note-id
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~ask-user prefix-argument-provided)
		(setq notes-note-id "")
		(setq ~note-id
		    (progn
			notes-note-id	    	; shadow the global variable
			
			(~notes-default-note-id)
		    )
		)
		
		(if
		    (|
			~ask-user
			(= ~note-id "")
		    )
		    (setq ~note-id (get-tty-string "Read note (nn.rr) "))
		)
		(~notes-read-note notes$k_note_id (~notes-map-note-id ~note-id))
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-search
	~start-search
	~new-search
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~start-search prefix-argument-provided)
		
		(~notes-change-notefile-state 1 1)
		
		(setq ~new-search
		    (if (|
			    ~start-search
			    (! ~notes-searching)
			)
			(setq ~start-search
			    (~notes-fill-select-notes-form "Search notes" "Start searching " "*.*"))
			1
		    )
		)
		(if ~new-search
		    (progn
			(if (! (setq ~notes-searching
				   (notes-note-search
				       ~start-search 0 1 "" notes-search-context))
			    )
			    (progn
				(~notes-setup-read-note-mode-line)
				(~notes-error-message)
			    )
			)
			(~notes-read-note notes$k_note_id notes-note-id)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun ~notes-fill-select-notes-form (~title ~q-prompt ~note-id)
    (form-fill-form
	notes-form-buffer	~title
	(concat ~q-prompt notes-note-search-note-id) "y"
	(novalue) 1
	; fields
	"Note-id:" ~note-id "s" (setq notes-note-search-note-id ~result)
	"Title:" notes-note-search-title-text "s"
	(progn
	    (setq notes-note-search-title-text ~result)
	    (setq notes-note-search-title (if (length ~result) (concat "*" ~result "*") ""))
	)
	"String:" notes-note-search-string "s" (setq notes-note-search-string ~result)
	"Author:" notes-note-search-author "s" (setq notes-note-search-author ~result)
	"Since:" notes-note-search-since "s" (setq notes-note-search-since ~result)
	"Before:" notes-note-search-before "s" (setq notes-note-search-before ~result)
	"Unseen:" notes-note-search-unseen "b" (setq notes-note-search-unseen ~result)
	"Keyword:" notes-note-search-keyword "s" (setq notes-note-search-keyword ~result)
    )
)
;
; The select key has a complex job to do. It selects the next thing of
; interest based on the buffer that the key was hit in.
;
(defun
    (notes-c-select-key
	(use-variables-of-buffer notes-notebook-header
	    (if
		(= current-buffer-name notes-notefile-note)
		(notes-c-add-entry)
		(|
		    (= current-buffer-name notes-notefile-directory)
		    (= current-buffer-name notes-notefile-keyword-directory)
		    (= current-buffer-name notes-notebook-marker-directory)
		)
		(notes-c-read-note)
		(|
		    (= current-buffer-name notes-notebook-entry-directory)
		    (= current-buffer-name notes-notebook-class-directory)
		)
		(notes-c-open)
		(message "Nothing to select.")
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-class
	~class
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~class
		    (progn
			notes-notebook-current-class	; shadow the real current
			; class
			(setq notes-notebook-current-class "")
			(~notes-default-class)
		    )
		)
		
		(if (= ~class "")
		    (setq ~class (get-tty-string "Set-class "))
		)
		
		;
		; see if the name is valid
		;
		(notes-class-get ~class)
		(setq notes-notebook-current-class ~class)
		(~notes-setup-notebook-header)
		(notes-c-directory-entries)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-conference
	~title
	~notice
	~restrict-members
	~create-keywords
	~write-lock
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(notes-notefile-get)
		(if
		    (form-fill-form
			notes-form-buffer "Set conference"
			(concat "Set conference") "y"
			(novalue) 1
			; fields to fill
			"Title:" notes-notefile-title "s" (setq ~title ~result)
			"Notice:" notes-notefile-notice "s" (setq ~notice ~result)
			"Restrict members:" notes-notefile-restrict-members "b" (setq ~restrict-members ~result)
			"Create keywords:" notes-notefile-create-keywords "b" (setq ~create-keywords ~result)
			"Write-lock:" notes-notefile-write-lock "b" (setq ~write-lock ~result)
		    )
		    (progn
			(notes-notefile-modify
			    ~title	    (!= notes-notefile-title ~title)
			    ~notice	    (!= notes-notefile-notice ~notice)
			    ~write-lock	    (!= notes-note-write-lock ~write-lock)
			    ~restrict-members	    (!= notes-notefile-restrict-members ~restrict-members)
			    ~create-keywords	    (!= notes-notefile-create-keywords ~create-keywords)
			)
			;
			;   regenerate the notefile header
			;
			(~notes-regenerate-notefile-header)
			
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-moderator
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(notes-notefile-moderate)
		;
		;   regenerate the notefile header
		;
		(~notes-regenerate-notefile-header)
		
		(if
		    notes-notefile-moderator-enabled
		    (message "You now have moderator privileges.")
		    (message "You are no longer a moderator.")
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-note
	~ask-user
	~note-id
	~title ~new-title
	~hidden
	~write-lock
	~seen
	~file-name
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(setq ~note-id (~notes-default-note-id))
		(if
		    (|
			~ask-user
			(= "" ~note-id)
		    )
		    (setq ~note-id (get-tty-string "Set note (note-id) "))
		)
		(notes-note-get notes$k_note_id ~note-id)
		(setq ~seen (! notes-note-unseen))
		(setq ~title
		    (if notes-note-is-topic notes-note-topic-title notes-note-title))
		(if
		    (form-fill-form
			notes-form-buffer (concat "Set note " notes-note-id)
			(concat "Set note " notes-note-id) "y"
			(novalue) 1
			; fields to fill
			"Title:" ~title "s" (setq ~new-title ~result)
			"Hidden:" notes-note-hidden "b" (setq ~hidden ~result)
			"Write-lock:" notes-note-write-lock "b" (setq ~write-lock ~result)
			"Seen:" ~seen "b" (setq ~seen ~result)
			"File-name:" notes-note-conference-file-name "s" (setq ~file-name ~result)
		    )
		    
		    (notes-note-modify
			~note-id
			~new-title	    (!= ~title ~new-title)
			~hidden		    (!= notes-note-hidden ~hidden)
			~write-lock	    (!= notes-note-write-lock ~write-lock)
			~seen		    (!= (! notes-note-unseen) ~seen)
			~file-name	    (!= notes-note-conference-file-name ~file-name)
		    )
		)
		(~notes-regenerate-notefile-header)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-profile
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(notes-profile-get)
		(if
		    (form-fill-form
			notes-form-buffer	"Modify Profile"
			"Modify profile"
			"y"
			(novalue) 1
			; fields to fill
			"Personal name:" notes-profile-pen-name "s" (setq notes-profile-pen-name ~result)
			"Automatic Directory:" notes-profile-auto-dir "b" (setq notes-profile-auto-dir ~result)
			"Automatic Next unseen:" notes-profile-auto-unseen "b" (setq notes-profile-auto-unseen ~result)
			"Default class name:" notes-profile-class-name "s" (setq notes-profile-class-name ~result)
			"Print options:" notes-profile-print "s" (setq notes-profile-print ~result)
		    )
		    (progn
			(notes-profile-modify 0)
			(notes-c-show-profile)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-set-seen
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		
		(notes-notefile-set-seen
		    (case-string-upper
			(get-tty-string "Set seen before (time)[-- 0:0:0]: ")))
		
		(notes-c-update)
	    )
	)
    )
)
(defun
    (notes-c-show-class
	~list-entries
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~list-entries prefix-argument-provided)
		(notes-class-get "*")
		(~notes-change-mode "notebook" notes-notebook-class-directory "c"
		    (if ~list-entries
			"    Class name      Entry name           Unseen  Topics   Update status/Filename"
			"    Class name"
		    )
		)
		(erase-buffer)
		(setq mode-string "Class listing")
		
		(while
		    (progn
			(insert-string
			    (sys$fao "!4< !#*D!#** !>!AS\n" "nns"
				(= (case-string-lower notes-class-name)
				    (case-string-lower notes-profile-class-name))
				(= (case-string-lower notes-class-name)
				    (case-string-lower notes-notebook-current-class))
				notes-class-name
			    )
			)
			(if ~list-entries
			    (progn
				(if (! (error-occurred (notes-class-get-entry)))
				    (while
					(progn
					    (notes-entry-get notes-class-entry-name)
					    (insert-string
						(sys$fao "!20* !21AS !5SL   !5SL  !AS!AS\n" "snnss"
						    notes-entry-name
						    notes-entry-unseen-est
						    notes-entry-number-of-topics
						    (if (|
							    (& notes-entry-last-status 1)
							    (= notes-entry-last-status 0)
							)
							""
							(concat
							    (sys$getmsg notes-entry-last-status 1)
							    " "
							)
						    )
						    notes-entry-file-name
						)
					    )
					    (notes-class-get-entry-next)
					)
					(novalue)
				    )
				)
			    )
			)
			(notes-class-get-next)
		    )
		    (novalue)
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-conference
	(~notes-change-notefile-state 1 1)
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(notes-notefile-get)
		(~notes-change-mode "notefile" notes-notefile-conference-details "c"
		    (concat "Conference listing for entry " notes-current-entry-name))
		
		(erase-buffer)
		
		(insert-string
		    (concat
			"	Entry name:	" notes-current-entry-name "\n"
			"	File name:	" notes-notefile-result-spec "\n\n"
			
			"	Title:		" notes-notefile-title "\n"
			"	Notice:		" notes-notefile-notice "\n\n"
			"	Moderator:	" notes-notefile-moderator "\n\n"
			
			"\tThe conference has "
			notes-notefile-number-of-notes
			" notes in "
			notes-notefile-number-of-topics
			" topics.\n\tIt was create on "
			notes-notefile-create-time
			" and\n\tit was last revised on "
			notes-notefile-last-revised
			".\n\tAccess "
			(if notes-notefile-restrict-members "is" "is not")
			" restricted to joined members.\n\tKeyword creation "
			(if notes-notefile-create-keywords "is not" "is")
			" restricted.\n\tNotes "
			(if notes-notefile-write-lock "may not" "may") " be written.\n"
		    )
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-entry
	~entry ~classes
	~personal-name
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~entry
		    (if prefix-argument-provided
			(get-tty-string "List entry (entry name): ")
			(~notes-default-entry)
		    )
		)
		
		;
		;   get the info
		;
		(notes-entry-get ~entry)
		(setq ~classes "")
		(while (notes-entry-get-class-next)
		    (setq ~classes (concat ~classes "," notes-entry-class-name))
		)
		(setq ~classes (substr ~classes 2 999))
		(setq ~personal-name (~notes-element notes-entry-user-area "\^a" 0))
		
		;
		;   change buffer and mode and format the info
		;
		(~notes-change-mode "notebook" notes-notebook-entry-details "c"
		    "\tEntry details")
		(erase-buffer)
		(setq mode-string notes-entry-name)
		(insert-string
		    (concat
			"\n\tEntry name:\t" notes-entry-name
			"\n\tFile:\t\t" notes-entry-file-name
			"\n\tClasses:\t" ~classes
			"\n\tPersonal name:\t" ~personal-name
			"\n"
		    )
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-errors

	(save-window-excursion
	    (split-current-window)
	    (switch-to-buffer "notes-messages")
	    (setq current-buffer-journalled 0)
	    (setq current-buffer-checkpointable 0)
	    (error-occurred (exchange-dot-and-mark))
	    (get-tty-string "Press RETURN to continue. ")
	    (end-of-file)
	)
	(novalue)
    )
)
(defun
    (notes-c-show-keywords
	~list-notes ~keyword
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~list-notes prefix-argument-provided)
		
		(~notes-change-notefile-state 1 1)
		
		(setq ~keyword
		    (if ~list-notes
			(get-tty-string "List keyword (keyword)[*] ")
			"*"
		    )
		)
		(if (= ~keyword "") (setq ~keyword "*"))
		(if (error-occurred (notes-keyword-get ~keyword ~list-notes))
		    (if (= notes-status notes$_no_such_keyword)
			(message "No such Keywords")
			(error-message error-message)
		    )
		    (progn
			(~notes-change-mode "notefile" notes-notefile-keyword-directory "c"
			    (if ~list-notes
				" Keyword name                       Note"
				" Keyword name"
			    )
			)
			(erase-buffer)
			(while
			    (progn
				(insert-string (concat " " notes-keyword-name "\n"))
				(if ~list-notes
				    (progn
					(while
					    (notes-keyword-note-get)
					    (insert-string
						(sys$fao "!36* !AS\n" "s" notes-keyword-note-id)
					    )
					)
				    )
				)
				(sit-for 0)
				(notes-keyword-get-next ~list-notes)
			    )
			    (novalue)
			)
			(beginning-of-file)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-marker
	~marker ~entry
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~marker
		    (if prefix-argument-provided
			(get-tty-string "List marker (name) ")
			"*"
		    )
		)
		(if (= ~marker "") (setq ~marker "*"))
		;
		; get the entry name from the directory list
		;
		(setq ~entry (~notes-default-entry))
		
		(if (error-occurred (notes-marker-get ~marker ~entry))
		    (if (= notes-status notes$_no_such_keyword)
			(message "No such Marker")
			(error-message error-message)
		    )
		    (progn
			(~notes-change-mode "notebook" notes-notebook-marker-directory "c"
			    " Marker name                      Note      Entry"
			)
			(erase-buffer)
			(while
			    (progn
				(insert-string
				    (sys$fao " !32AS !9AS !AS\n" "sss"
					notes-marker-name
					notes-marker-note-id
					notes-marker-entry
				    )
				)
				(notes-marker-get-next)
			    )
			    (novalue)
			)
			(beginning-of-file)
		    )
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-moderator
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		(if prefix-argument-provided
		    (notes-user-get (get-tty-string "List moderators (member name): "))
		    (notes-user-get "*")
		)
		
		(~notes-show-members 1)	; show all moderators
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-members
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(~notes-change-notefile-state 1 1)
		(if prefix-argument-provided
		    (notes-user-get (get-tty-string "List members (member name): "))
		    (notes-user-get "*")
		)
		
		(~notes-show-members 0)	; show all members
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-note
	~ask-user
	~note-id
	
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(setq ~ask-user prefix-argument-provided)
		(~notes-change-notefile-state 1 1)
		
		(setq ~note-id (~notes-default-note-id))
		(if
		    (|
			(= "" ~note-id)
			~ask-user
		    )
		    (setq ~note-id (get-tty-string "List note (nn.rr) "))
		)
		
		(notes-note-get
		    notes$k_note_id (~notes-map-note-id ~note-id)
		    notes$k_noop 0
		    2			; hint get keywords
		)
		
		(~notes-change-mode "notefile" notes-notefile-note-details "c"
		    (concat "Details of note " notes-note-id)
		)
		(erase-buffer)
		
		(insert-string
		    (concat
			"Topic Title:\t"     notes-note-topic-title
			(if (!= "" notes-note-title)
			    (concat "Title:\t\t" notes-note-title) "")
			"\nAuthor:\t\t" notes-note-author
			(if (!= "" notes-note-pen-name)
			    (concat " \"" notes-note-pen-name "\"") "")
			(if (!= "" notes-note-conference-file-name)
			    (concat "\nFile:"  notes-note-conference-file-name)
			    ""
			)
			"\nDate:\t\t" notes-note-create-time
			"\n\n\tThe note is " (if notes-note-hidden "" "not ") "hidden."
			"\n\tReplies may "
			(if notes-note-write-lock "not " "")
			"be written."
			"\n\tThe note is " notes-note-number-of-records " lines long"
			(if notes-note-unseen "\n\tYou have not seen the note." "")
			"\n"
		    )
		)
		(insert-string (sys$fao "\n‘ Keywords !#*‘\n" "n" 69))
		(while
		    (notes-note-keyword-get)
		    (insert-string (concat "  " notes-note-keyword "\n"))
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-profile
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		(notes-profile-get)	; get the latest profile values
		(~notes-change-mode "notebook" notes-notebook-profile "c"
		    "Profile listing"
		)
		(erase-buffer)
		(setq mode-string "Profile listing")
		(insert-string
		    (sys$fao
			(concat
			    "	Editor:			    !AS !AS\n"
			    "	Personal name:		    !AS\n"
			    "	Automatic command on open:  !AS\n"
			    "	Default class name:	    !AS\n"
			    "	Print options:		    !AS\n"
			) "ssssss"
			notes-profile-editor
			(if notes-profile-editor-spawn " (Spawned)" "")
			(if (= "" notes-profile-pen-name) "None" notes-profile-pen-name)
			(if notes-profile-auto-dir "Directory"
			    (if notes-profile-auto-unseen "Next unseen" "None"))
			notes-profile-class-name
			(if (= "" notes-profile-print) "None" notes-profile-print)
		    )
		)
		(beginning-of-file)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-show-version
	(message "Emacs VAX Notes version V2.0 25-oct-1987")
    )
)
(defun
    (notes-c-change-view
	~cur-mode
	
	(setq ~cur-mode (fetch-array notes-mode notes-current-session))
	(if prefix-argument-provided
	    (progn
		;
		; change mode
		;
		(if
		    (= "notebook" ~cur-mode)
		    (progn
			(message "Changing mode to notefile mode.")
			(setq-array notes-notebook-windows notes-current-session
			    current-windows)
			(setq current-windows
			    (fetch-array notes-notefile-windows notes-current-session))
			(setq-array notes-mode notes-current-session "notefile")
		    )
		    (= "notefile" ~cur-mode)
		    (progn
			(message "Changing mode to notebook mode.")
			(setq-array notes-notefile-windows notes-current-session
			    current-windows)
			(setq current-windows
			    (fetch-array notes-notebook-windows notes-current-session))
			(setq-array notes-mode notes-current-session "notebook")
		    )
		)
	    )
	    (progn
		(message "Change buffer")
		;
		; change buffer within mode
		;
		(if
		    (= "notefile" ~cur-mode)
		    (if
			(= current-buffer-name notes-notefile-header)
			(~notes-setup-notefile-windows)
			(while
			    (progn
				(if
				    (= current-buffer-name notes-notefile-note)
				    (switch-to-buffer notes-notefile-directory)
				    (= current-buffer-name notes-notefile-directory)
				    (switch-to-buffer notes-notefile-note-details)
				    (= current-buffer-name notes-notefile-note-details)
				    (switch-to-buffer notes-notefile-conference-details)
				    (= current-buffer-name notes-notefile-conference-details)
				    (switch-to-buffer notes-notefile-member-directory)
				    (= current-buffer-name notes-notefile-member-directory)
				    (switch-to-buffer notes-notefile-keyword-directory)
				    (switch-to-buffer notes-notefile-note)
				)
				(&
				    (= (buffer-size) 0)
				    (!= current-buffer-name notes-notefile-note)
				)
			    )
			    (novalue)
			)
		    )
		    (if
			(= current-buffer-name notes-notebook-header)
			(~notes-setup-notebook-windows)
			(while
			    (progn
				(if
				    (= current-buffer-name notes-notebook-entry-details)
				    (switch-to-buffer notes-notebook-entry-directory)
				    (= current-buffer-name notes-notebook-entry-directory)
				    (switch-to-buffer notes-notebook-profile)
				    (= current-buffer-name notes-notebook-profile)
				    (switch-to-buffer notes-notebook-class-directory)
				    (= current-buffer-name notes-notebook-class-directory)
				    (switch-to-buffer notes-notebook-marker-directory)
				    (switch-to-buffer notes-notebook-entry-details)
				)
				(&
				    (= (buffer-size) 0)
				    (!= current-buffer-name notes-notebook-entry-directory)
				)
			    )
			    (novalue)
			)
		    )
		)
	    )
	)
	(progn
	    ~mode-line
	    (use-variables-of-buffer current-buffer-name
		(setq ~mode-line ~notes-header-mode-line)
	    )
	    (save-excursion
		(temp-use-buffer
		    (if (= ~cur-mode "notebook")
			notes-notebook-header
			notes-notefile-header
		    )
		)
		(setq mode-line-format ~mode-line)
	    )
	)
	(novalue)
    )
)
(defun notes-c-update
    (
	~entries (concat "")
	~classes (concat notes-notebook-current-class)
    )
    ~arg ~default-entry
    
    
    (use-variables-of-buffer notes-notebook-header
	(progn
	    (setq ~arg prefix-argument-provided)
	    (setq ~default-entry (~notes-default-entry))
	    
	    (if
		(|
		    (= current-buffer-name notes-notefile-directory)
		    (= current-buffer-name notes-notefile-note)
		    (= current-buffer-name notes-notefile-note-details)
		)
		(~notes-update-seen-map)
		(save-window-excursion
		    (split-current-window)
		    (switch-to-buffer "notes-messages")
		    (setq current-buffer-journalled 0)
		    (setq current-buffer-checkpointable 0)
		    (end-of-file)
		    (line-to-top-of-window)
		    
		    (if (& (! ~arg) (= ~entries ""))
			(~notes-update-entry ~default-entry)
			(progn
			    (if (= ~entries "")
				(form-fill-form
				    notes-form-buffer	"Update notebook entries"
				    "" ""
				    (novalue) 1
				    ; fields to fill
				    "Entries:" "*" "s" (setq ~entries ~result)
				    "Classes:"  notes-notebook-current-class "s"
				    (setq ~classes ~result)
				)
			    )
			    (notes-class-get ~classes)
			    
			    (while
				(progn
				    (progn
					(if
					    (= ~entries "*")
					    (if (! (error-occurred (notes-class-get-entry)))
						(while
						    (progn
							(~notes-update-entry
							    notes-class-entry-name)
							(end-of-file)
							(notes-class-get-entry-next)
						    )
						    (novalue)
						)
					    )
					    (progn
						(~notes-update-entry ~entries)
						(end-of-file)
					    )
					)
				    )
				    (if (= ~classes "*")
					(notes-class-get-next)
					0
				    )
				)
				(novalue)
			    )
			)
		    )
		)
	    )
	    
	    (if (= current-buffer-name notes-notebook-entry-directory)
		(notes-c-directory-entries))
	    
	)
    )
    (novalue)
)
(defun ~notes-update-entry (~entry)

    (notes-entry-get ~entry)
    (~notes-add-message-to-log
	(concat "Updating entry " ~entry " (file " notes-entry-file-name ")...")
    )
    (sit-for 0)
    (error-occurred (notes-notefile-begin notes-entry-file-name 0 31))
    (setq notes-entry-last-status notes-status)
    (error-occurred (notes-notefile-end 0 31))
    (if (& 1 notes-entry-last-status)
	(progn
	    (~notes-update-entry-inner ~entry 0)
	    (~notes-add-message-to-log
		(sys$fao "... !5SL Unseen, !5SL Topics, Last revised !17AS"
		    "nns"
		    notes-entry-unseen-est
		    notes-entry-number-of-topics
		    notes-entry-last-revised
		)
	    )
	)
	(~notes-add-message-to-log
	    (concat "... " (sys$getmsg notes-entry-last-status 1))
	)
    )
    (sit-for 0)
)
(defun
    (notes-c-write-note
	
	(use-variables-of-buffer notes-notebook-header
	    (progn
		;
		; check that a notefile is open and change to notefile mode
		;
		(~notes-change-notefile-state 1 1)
		(~notes-change-mode "notefile" notes-notefile-note "c" "")
		
		(save-window-excursion
		    (~notes-write-buffer 0 "Write new note %m")
		)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-exit
	(if prefix-argument-provided
	    ;
	    ; Suspend this notes session
	    ;
	    (progn
		(if
		    (=
			"notebook"
			(fetch-array notes-mode notes-current-session)
		    )
		    (setq-array notes-notebook-windows notes-current-session
			current-windows)
		    (setq-array notes-notefile-windows notes-current-session
			current-windows)
		)
		(setq current-windows notes-users-windows)
		(setq notes-users-windows 0)
		(message "Suspending notes session " notes-current-session "...")
	    )
	    (progn
		;
		; If the conference is open close it
		;
		(if (fetch-array notes-notefile-is-open notes-current-session)
		    (notes-c-close)
		)
		;
		; now clean up and exit
		;
		(notes-c-end-session)
	    )
	)
	(novalue)
    )
)
(defun
    (notes-c-next-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(next-line)
	(if (eobp)
	    (previous-line))
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
	    )
	)
    )
)
(defun
    (notes-c-previous-item
	(save-excursion
	    (beginning-of-file)
	    (error-occurred (replace-string "\200" " "))
	)
	(previous-line)
	(beginning-of-line)
	(if (! (eobp))
	    (save-excursion
		(insert-character '\200')
		(delete-next-character)
	    )
	)
    )
)
(defun
    (notes-c-not-a-command
	(message "Key " (char-to-string (last-key-struck)) " is not a notes command.")
    )
)


; ~
;
; read the body of a note, assuming the note has been Selected
;
(defun ~notes-read-note
    (
	~op
	~arg (+ 0)
	~op2 (+ notes$k_noop)
	~arg2 (+ 0)
	~seen (+ 1)
    )
    ~last-note-id

    (~notes-change-notefile-state 1 1)

    ;
    ;	if the op is note_id, see if arg is a marker
    ;
    (if (= notes$k_note_id ~op)
	(error-occurred
	    (notes-marker-get ~arg)
	    (setq ~arg notes-marker-note-id)
	)
    )
    ;
    ;	Save current note id for storing in ~notes-last-read-note-id
    ;
    (setq ~last-note-id notes-note-id)

    ;
    ; select the note of interest
    ;
    (notes-note-get ~op ~arg ~op2 ~arg2 1 ~seen)

    ;
    ; notefile mode for reading a note
    ;
    (~notes-change-mode "notefile" notes-notefile-note "c"
	;
	; setup the note header info
	;
	(~notes-setup-notefile-note-header)
    )

    (setq ~notes-last-read-note-id ~last-note-id)
    (setq ~notes-lines-read 0)
    (erase-buffer)
    (sit-for 0)

    (setq notes-number-of-notes-read (+ 1 notes-number-of-notes-read))

    ;
    ; read the body of the message
    ;
    (if (&; (! (& notes-note-hidden (! notes-notefile-moderator-enabled)))
	    (!= notes-note-number-of-records 0))
	(notes-note-read
	    (- window-size 1)
	    notes-note-context
	    notes-note-read-timeout
	)
    )
    (beginning-of-file)
    (~notes-setup-read-note-mode-line)

    ;
    ;	Check to see if its time to checkpoint the seen map
    ;
    (if (= 0 (% notes-number-of-notes-read notes-update-seen-map-frequency))
	(~notes-update-seen-map)
    )
)
(defun ~notes-write-buffer (~reply ~mode-line)
    ~title ~hide ~write-lock ~keywords
    ~first-char

    ;
    ;	Get the details for the note to be written, title etc...
    ;
    (setq ~first-char 1)
    (if ~reply (error-occurred (split-current-window)))
    (if
	(if notes-put-form-in-write-buffer
	    (form-fill-form
		notes-notefile-new-note	~mode-line
		(concat "Write note (" ~title ")")
		"y"
		(progn		; custom setup option
		    (use-local-map "Text-mode-map")
		    (error-occurred (notes-hook-setup-compose-buffer))
		)
		0
		; fields to fill
		"Title:" "" "s" (setq ~title ~result)
		"Add keywords:" "" "s" (setq ~keywords ~result)
		"Hide note:" 0 "b" (setq ~hide ~result)
		(if (! ~reply) "Write lock:" "") 0 "b" (setq ~write-lock ~result)
		"‘‘‘‘‘ Use erase-note-text to empty buffer, use exit-emacs to write note ‘‘‘‘‘‘‘‘\n"
		"" "t"
		(progn (forward-character) (setq ~first-char (dot)))
	    )
	    (progn
		(switch-to-buffer notes-notefile-new-note)
		(use-local-map "Text-mode-map")
		(error-occurred (notes-hook-setup-compose-buffer))
		
		(setq mode-line-format ~mode-line)
		(recursive-edit)
		
		(form-fill-form
		    notes-form-buffer	""
		    (concat "Write note (" ~title ")")
		    "y"
		    (novalue) 0
		    ; fields to fill
		    "Title:			" "" "s" (setq ~title ~result)
		    "Add keywords:" "" "s" (setq ~keywords ~result)
		    "Hide note:" 0 "b" (setq ~hide ~result)
		    (if (! ~reply) "Write lock:" "") 0 "b" (setq ~write-lock ~result)
		)
	    )
	)
	(progn
	    (switch-to-buffer notes-notefile-new-note)
	    (goto-character ~first-char)
	    (set-mark)
	    (end-of-file)
	    (narrow-region)
	    (notes-note-write ~reply ~title ~hide ~write-lock)
	    (widen-region)
	
	    ;
	    ; select the note of interest
	    ;
	    (notes-note-get notes$k_note_id notes-note-id)
	
	    ;
	    ; copy text of note into reading buffer
	    ;
	    (copy-region-to-buffer notes-notefile-note)
	    (unset-mark)
	
	    ;
	    ; notefile mode for reading a note
	    ;
	    (~notes-change-mode "notefile" notes-notefile-note "c"
		;
		; setup the note header info
		;
		(~notes-setup-notefile-note-header)
	    )
	    (setq ~notes-lines-read notes-note-number-of-records)
	    (~notes-setup-read-note-mode-line)
	    ;
	    ;	Add any keywords that where specified
	    ;
	    (~notes-add-keywords ~keywords notes-note-id)
	)
    )
)
(defun
    (erase-note-text
	(save-excursion
	    (end-of-file)
	    (set-mark)
	    (error-occurred (goto-character form-end-of-form))
	    (erase-region)
	)
    )
)
(defun
    (~notes-update-seen-map
	(message "Updating the seen map...") (sit-for 0)
	;
	; retrive the seen map and other info
	;
	(notes-notefile-get notes-seen-map)

	;
	; get the latest entry info for the update
	;
	(notes-entry-get)
	(~notes-update-entry-inner notes-current-entry-name notes-seen-map)

	;
	;   regenerate the notefile header
	;
	(~notes-regenerate-notefile-header)
    )
)


(defun ~notes-show-members
    (
	~moderators-only
    )
    (~notes-change-mode "notefile" notes-notefile-member-directory "c"
	;Scott__barry  marvin,heart,
	" Member         Mail address       Privs  Access List"
    )
    
    (erase-buffer)
    
    (while
	(progn
	    (if (|
		    (! ~moderators-only)
		    notes-user-moderate
		)
		(insert-string
		    (sys$fao " !14AS !18AS !6AS !AS\n"
			"ssss"
			notes-user-name
			notes-user-mail-addr
			(sys$fao "!#<[!>!#<m!>!#<k!>!#<]!>"
			    "nnnn"
			    (| notes-user-moderate notes-user-create-keyword)
			    notes-user-moderate
			    notes-user-create-keyword
			    (| notes-user-moderate notes-user-create-keyword)
			)
			notes-user-access-list
		    )
		)
	    )
	    (notes-user-get-next)
	)
	(novalue)
    )
    (unset-mark)
    (beginning-of-file)
)


;
; end of module
;
[notes_def.ml]
; 
; notes_def.ml
; 
    (execute-mlisp-file "fscndef")
    (progn
    (~sys_literal_setup_incr "notes$k_" 0 1
	"noop"			; Noop, do nothing with this item
	"nosignal"		; The default is to signal errors.
				; Setting this on a BEGIN operation disables signals,
				; any error status will be returned in R0.
;-----------------------------;
	"text_string"		; String of text record in a note.
	"text_end"		; Indicates the end of the note text, for a note being added.
	"text_type"		; Longword type of the text record.
;-----------------------------;
	"note_all_responses"	; Indicates responses should also be returned.
	"note_author"		; String of author name.
	"note_back_note"	; Select the previous base note.
	"note_back_response"	; Select the previous response.
	"note_before_time"	; Select only notes before this quadword date/time.
	"note_blink_id"		; String note identifier.
	"note_blink_uid"	; Longword unique note identifier.
	"note_create_time"	; Quadword date/time of note creation.
	"note_hidden"		; Select or modify hidden notes.
				; For selection, 1=select hidden notes, 0=select unhidden notes.
				; for modifying a note, 1=hide note, 0=unhide note.
	"note_id"		; String note identifier or range of notes.
	"note_next_note"	; Select the next following base note.
	"note_next_response"	; Select the next following response.
	"note_numrecords"	; Longword number of text records in this note.
	"note_numresponses"	; Longword response number of last response.
	"note_pen_name"		; String of user pen name (personal name).
	"note_search_string"	; Select only notes containing this search string.
	"note_search_title"	; Select only notes containing this string in title.
	"note_since_time"	; Select only notes after this quadword date/time.
	"note_title"		; String of title.
	"note_uid"		; Longword unique note identifier.
	"note_unseen"		; Select notes; 1=select unseen notes, 0=select seen notes.
	"note_user_area"	; String of user-programmable data.
	"note_writelock"	; Modify locking of notes.
				; Value 1=writelock note, 0=remove writelock on note.
;-----------------------------;
	"notefile_context"	; Passes the NOTEFILE context to other operations.
	"class_context"		; Passes the CLASS context to other operations.
	"entry_context"		; Passes the ENTRY context to other operations.
	"keyword_context"	; Passes the KEYWORD context to other operations.
	"note_context"		; Passes the NOTES context to other operations.
	"profile_context"	; Passes the PROFILE context to other operations.
	"server_context"	; Passes the SERVER context to other operations.
	"user_context"		; Passes the USER context to other operations.
;-----------------------------;
	"class_name"		; String of class name, wildcards may be allowed, depending on the context.
	"class_new_name"	; String of new class name, renames the class.
;-----------------------------;
	"continue"		; Keep returning information.
;-----------------------------;
	"entry_name"		; String of entry name, wildcards may be allowed, depending on the context.
	"entry_new_name"	; String of new entry name, renames the entry.
	"entry_user_area"	; String of user-programmable data.
;-----------------------------;
	"hint"			; Will be expanded later
;-----------------------------;
	"keyword_name"		; String of keyword name, wildcards may be allowed, depending on the context.
	"keyword_new_name"	; String of new keyword name, renames the keyword.
;-----------------------------;
	"notefile_create"	; Create the specified notefile.
	"notefile_create_time"	; Quadword date/time of the notefile creation.
	"notefile_default_name"	; String of default notefile name.
	"notefile_entrytotal"	; Total number of notes and responses.
	"notefile_file_name"	; String of notefile name.
	"notefile_format"	; Longword indicating notefile format.
	"notefile_high_uid"	; Longword of highest used note uid.
	"notefile_lastrev"	; Quadword date/time of the last notefile revision.
	"notefile_moderate"	; Change moderator status for the notefile.
				; Value 1=attain moderator status, 0=turn off moderator status.
	"notefile_moderator"	; String of moderator name.
	"notefile_notice"	; String of notice of the day.
	"notefile_numnotes"	; Longword note number of last base note in notefile.
	"notefile_related_name"	; String of related name for the notefile.
	"notefile_restricted"	; Modify or get whether user access is restricted.
				; Value 1=only authorized users may participate, 0=any users may participate.
	"notefile_result_spec"	; String resultant file name spec of the notefile.
	"notefile_title"	; String of title of the notefile.
	"notefile_transport"	; Transport mechanism, TBD.
	"notefile_user_area"	; String of user-programmable data.
;-----------------------------;
	"profile_auto_dir"	; Modify or get auto-directory setting.
				; Value 1=auto-directory,reverse, 0=no auto-directory.
	"profile_auto_unseen"	; Modify or get auto-unseen setting.
				; Value 1=auto-unseen, 0=no auto-unseen.
	"profile_class_name"	; String of default entry-class to use.
	"profile_editor"	; String of editor name.
	"profile_editor_spawn"	; Flag to indicate whether editor must be spawned.
				; Value 1=spawn editor, 0=callable editor.
	"profile_pen_name"	; String of personal, pen, name.
	"profile_print"		; String of default print qualifiers to apply.
				; This allows the user to define defaults for the PRINT command.
	"profile_temporary"	; Indicates the specified profile settings are temporary, and should not be stored.
;-----------------------------;
	"seen_map"		; String indicating which notes have been seen.
				; User programs may not interpret this byte string, except that a null string
				; indicates no notes have been seen.
;-----------------------------;
	"user_create_keyword"	; Modify or get whether user may create keywords.
				; Value 1=may create keywords, 0=may not create keywords.
	"user_mail_addr"	; String of users mailing address.
	"user_moderate"		; Modify or get whether user may moderate the notefile.
				; Value 1=may moderate, 0=may not moderate.
	"user_name"		; String of username, wildcards may be allowed, depending on the context.
	"user_new_name"		; String of new username (rename the user).
	"user_new_nodename"	; Chang the string of nodenames.
	"user_nodename"		; String of nodenames (comma-separated).
				; This, together with the username, indicates which processes may access
				; this (restricted) notefile.
;-----------------------------;

	"note_blink_title"	; String of title of the base note.
	"note_mark_seen"	; Mark the notes as having been seen.
	"note_next_unseen"	; Select the next unseen note.
	"note_hint_get_text"	; Will be getting text of note also
	"note_x_keyword"	; List of keywords on a note
	"keyword_x_note"	; List of notes for a keyword (or marker)
	"class_x_entry"		; List of entries for a class
	"entry_x_class"		; List of classes for an entry
	"entry_x_keyword"	; List of keywords for an entry
	"delif0"		; Delete (class) if empty
	"unientry"		; Unique by entry (for markers)
	"entry_unseen_est"	; Estimated number of unseen notes
	"entry_last_status"	; Last status from NOTEFILE_BEGIN
	"note_can_reply"	; flag to determine whether caller may reply
	"note_before_time_a"	; Select only notes before this ascii date/time.
	"note_create_time_a"	; Ascii date/time of note creation.
	"note_since_time_a"	; Select only notes after this ascii date/time.
	"notefile_create_time_a"; Ascii date/time of the notefile creation.
	"notefile_lastrev_a"	; Ascii date/time of the last notefile revision.
	"note_hint_get_keyword"	; Will be getting keywords for note
	"keyword_hint_get_note"	; Will be getting notes for keyword
	"server_max_item"
	"class_hint_get_entry"
	"class_user_area"
	"class_uid"
	"entry_hint_get_class"
	"entry_hint_get_keyword"
	"entry_object_name"
	"entry_object_spec"
	"entry_uid"
	"keyword_user_area"
	"keyword_uid"
	"node_name"
	"node_x_username"
	"note_new_id"
	"note_type"
	"notefile_reply_only"
	"profile_user_area"
	"user_access_list"
	"user_write_bypass"
	"user_user_area"
	"user_uid"
	"user_noaccess"

;-----------------------------;
	"max_item"		; Highest item-code; insert others before this.
    )
    (~sys_literal_setup_incr "notes$" 1 1
	"user_begin"
	"user_end"
	"user_add"
	"user_delete"
	"user_get"
	"user_modify"
	"profile_begin"
	"profile_end"
	"profile_get"
	"profile_modify"
	"note_begin"
	"note_end"
	"note_add"
	"note_delete"
	"note_get"
	"note_modify"
	"note_add_text"
	"note_get_text"
	"note_get_keyword"
	"notefile_begin"
	"notefile_end"
	"notefile_get_info"
	"notefile_modify"
	"notefile_list_begin"
	"notefile_list"
	"keyword_begin"
	"keyword_end"
	"keyword_add"
	"keyword_delete"
	"keyword_get"
	"keyword_modify"
	"keyword_get_note"
	"entry_begin"
	"entry_end"
	"entry_add"
	"entry_delete"
	"entry_get"
	"entry_modify"
	"entry_get_class"
	"entry_get_keyword"
	"class_begin"
	"class_end"
	"class_add"
	"class_delete"
	"class_get"
	"class_modify"
	"class_get_entry"
    )
    (~sys_literal_setup_equal "notes$k_" 
	"notefile_writelock" notes$k_note_writelock
	"unknown_type" 0
	"ddif_type" 1
	"ascii_type" 2
	"reserved_type" 3
	"wps_plus_type" 4
    )
    (~sys_literal_setup_equal "notes$_" 
	"no_more_classes"	66822152
	"no_more_entries"	66822160
	"no_more_keywords"	66822168
	"no_more_notes"		66822176
	"no_more_text"		66822184
	"no_more_users"		66822192
	"no_such_class"		66830618
	"no_such_entry"		66830626
	"no_such_keyword"	66830634
	"no_such_note"		66830642
	"no_such_user"		66830650
	"nmf"			66822840
    )
    (external-function "~notes" "emacs$share:emacs_vaxnotes_shr" "emacs$vaxnotes")
    )
[notes_routines.ml]
; 
; Notes_routines.ml
; 
; routines for access notebooks and notesfiles
; 
(execute-mlisp-file "notes_def")
; 
; global variables held in the notebook-header buffer
; 
(declare-global
    notes-status

    ; 
    ; The following hold the default contexts for the various operations
    ; 
    notes-notebook-context
    notes-profile-context
    notes-directory-context
    notes-search-context
    notes-entry-context
    notes-class-context
    notes-notefile-context
    notes-keyword-context
    notes-marker-context
    notes-note-context
    notes-user-context
    ;
    ; and the seen map
    ;
    notes-seen-map
)

; 
; Notebook routines
; 
(declare-buffer-specific
    notes-notebook-result-spec
    notes-notebook-current-class
)
(defun notes-notebook-begin
    (
	~file (concat "NOTES$NOTEBOOK")
	~create (+ 0)
	~ctx (+ notes-notebook-context)
    )
    
    (~notes notes$notefile_end ~ctx 0)
    (setq notes-status
	(~notes notes$notefile_begin ~ctx 4
	    notes$k_notefile_file_name		~file
	    notes$k_notefile_default_name	"SYS$LOGIN:.NOTE"
	    (if ~create notes$k_notefile_create notes$k_noop)	0
	    notes$k_user_moderate		1
	    
	    notes$k_notefile_result_spec	notes-notebook-result-spec
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notebook-end (~ctx (+ notes-notebook-context))
    
    (setq notes-status
	(~notes notes$notefile_end ~ctx 0)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)


; 
; profile routines
; 
(declare-buffer-specific
    notes-profile-editor
    notes-profile-editor-spawn
    notes-profile-pen-name
    notes-profile-auto-dir
    notes-profile-auto-unseen
    notes-profile-class-name
    notes-profile-print
    notes-profile-temporary
)
(defun notes-profile-begin
    (
	~book-ctx (+ notes-notebook-context)
	~ctx (+ notes-profile-context)
    )

    (~notes notes$profile_end ~ctx 0)
    (setq notes-status
	(~notes notes$profile_begin ~ctx 1
	    notes$k_notefile_context ~book-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-profile-get (~ctx (+ notes-profile-context))

    (setq notes-status
	(~notes notes$profile_get ~ctx 0
	    notes$k_profile_editor		notes-profile-editor
	    notes$k_profile_editor_spawn	notes-profile-editor-spawn
	    notes$k_profile_pen_name		notes-profile-pen-name
	    notes$k_profile_auto_dir		notes-profile-auto-dir
	    notes$k_profile_auto_unseen		notes-profile-auto-unseen
	    notes$k_profile_class_name		notes-profile-class-name
	    notes$k_profile_print		notes-profile-print
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-profile-modify
    (
	~temp (+ 0)
	~ctx (+ notes-profile-context)
    )

    (setq notes-status
	(~notes notes$profile_modify ~ctx 8
	    notes$k_profile_editor		notes-profile-editor
	    notes$k_profile_editor_spawn	notes-profile-editor-spawn
	    notes$k_profile_pen_name		notes-profile-pen-name
	    notes$k_profile_auto_dir		notes-profile-auto-dir
	    notes$k_profile_auto_unseen		notes-profile-auto-unseen
	    notes$k_profile_class_name		notes-profile-class-name
	    notes$k_profile_print		notes-profile-print
	    (if ~temp
		notes$k_profile_temporary
		notes$k_noop
	    )	0
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-profile-end (~ctx (+ notes-profile-context))

    (setq notes-status
	(~notes notes$profile_end ~ctx 0)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)


; 
; class routines
; 
(declare-buffer-specific
    notes-class-name
    notes-class-entry-name
)
(defun notes-class-begin
    (
	~book-ctx (+ notes-notebook-context)
	~ctx (+ notes-class-context)
    )

    (~notes notes$class_end ~ctx 0)
    (setq notes-status
	(~notes notes$class_begin ~ctx 1
	    notes$k_notefile_context ~book-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-class-add
    (
	~name
	~ctx (+ notes-class-context)
    )
    (setq notes-status
	(~notes notes$class_add ~ctx 1
	    notes$k_class_name	    ~name
	    notes$k_class_name	    notes-class-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-class-get
    (
	~name (concat notes-notebook-current-class)
	~ctx (+ notes-class-context)
    )

    (setq notes-status
	(~notes notes$class_get ~ctx 1
	    notes$k_class_name	    ~name
	    notes$k_class_name	    notes-class-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-class-get-next
    (
	~ctx (+ notes-class-context)
    )

    (setq notes-status
	(~notes notes$class_get ~ctx 1
	    notes$k_continue	    0
	    notes$k_class_name	    notes-class-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_classes)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-class-get-entry
    (
	~ctx (+ notes-class-context)
    )

    (setq notes-status
	(~notes notes$class_get_entry ~ctx 1
	    notes$k_continue	    0
	    notes$k_entry_name		notes-class-entry-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_entries)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-class-get-entry-next (~ctx (+ notes-class-context))

    (setq notes-status
	(~notes notes$class_get_entry ~ctx 1
	    notes$k_continue		0
	    notes$k_entry_name		notes-class-entry-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_entries)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-class-end (~ctx (+ notes-class-context))

    (setq notes-status
	(~notes notes$class_end ~ctx 0)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)


; 
; entry routines
; 
(declare-buffer-specific
    notes-current-entry-name

    notes-entry-name
    notes-entry-new-name
    notes-entry-unseen-est
    notes-entry-last-status
    notes-entry-last-revised
    notes-entry-number-of-topics
    notes-entry-number-of-notes
    notes-entry-high-uid
    notes-entry-file-name
    notes-entry-class-name
    notes-entry-user-area
    notes-entry-pen-name
)
(defun notes-entry-begin
    (
	~book-ctx (+ notes-notebook-context)
	~ctx (+ notes-entry-context)
    )

    (~notes notes$entry_end ~ctx 0)
    (setq notes-status
	(~notes notes$entry_begin ~ctx 1
	    notes$k_notefile_context ~book-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-entry-get
    (
	~name (concat notes-current-entry-name)
	~seen (+ 0)
	~ctx (+ notes-entry-context)
    )

    (setq notes-status
	(~notes notes$entry_get ~ctx 1
		notes$k_entry_name		~name
	        notes$k_entry_name		notes-entry-name
		notes$k_entry_unseen_est	notes-entry-unseen-est
		notes$k_entry_last_status	notes-entry-last-status
		notes$k_notefile_lastrev	notes-entry-last-revised
		notes$k_notefile_numnotes	notes-entry-number-of-topics
		notes$k_notefile_entrytotal	notes-entry-number-of-notes
		notes$k_notefile_high_uid	notes-entry-high-uid
		notes$k_notefile_file_name	notes-entry-file-name
		notes$k_entry_user_area		notes-entry-user-area
		(if ~seen
		    notes$k_seen_map
		    notes$k_noop
		)				~seen
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_entries)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-entry-get-next (~ctx (+ notes-entry-context))

    (setq notes-status
	(~notes notes$entry_get ~ctx 1
	    notes$k_continue		0
	    notes$k_entry_name		notes-entry-name
	    notes$k_entry_unseen_est	notes-entry-unseen-est
	    notes$k_entry_last_status	notes-entry-last-status
	    notes$k_notefile_lastrev	notes-entry-last-revised
	    notes$k_notefile_numnotes	notes-entry-number-of-topics
	    notes$k_notefile_entrytotal	notes-entry-number-of-notes
	    notes$k_notefile_high_uid	notes-entry-high-uid
	    notes$k_notefile_file_name	notes-entry-file-name
	    notes$k_entry_user_area	notes-entry-user-area
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_entries)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-entry-get-class-next (~ctx (+ notes-entry-context))

    (setq notes-status
	(~notes notes$entry_get_class ~ctx 1
	    notes$k_continue		0
	    notes$k_class_name		notes-entry-class-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_classes)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-entry-add
    (
	~name
	~file
	~class
	~user-area  ~add-user-area
	~ctx (+ notes-entry-context)
    )
    (setq notes-status
	(~notes notes$entry_add ~ctx 4
	    notes$k_entry_name		~name
	    (if (= ~file "") notes$k_noop notes$k_notefile_file_name
	    )				~file
	    notes$k_class_name		~class
	    (if ~add-user-area notes$k_entry_user_area notes$k_noop
	    )				~user-area
	)
    )
    (if (! (& notes-status 1))
	(~notes-error-message)
    )
)
(defun notes-entry-delete
    (
	~name
	~class
	~ctx (+ notes-entry-context)
    )
    (setq notes-status
	(if (= "*" ~name)
	    (~notes notes$class_delete ~ctx 1
		notes$k_class_name		~class
	    )
	    (~notes notes$entry_delete ~ctx 3
		notes$k_delif0			0
		notes$k_entry_name		~name
		notes$k_class_name		~class
	    )
	)
    )
    (if (! (& notes-status 1))
	(~notes-error-message)
    )
)
(defun notes-entry-modify
    (
	~name
	~file
	~new-name
	~user-area  ~mod-user-area
	~ctx (+ notes-entry-context)
    )
    (setq notes-status
	(~notes notes$entry_modify ~ctx 4
	    notes$k_entry_name		~name
	    (if (= "" ~file) notes$k_noop notes$k_notefile_file_name)	~file
	    (if (= "" ~new-name) notes$k_noop notes$k_entry_new_name)	~new-name
	    (if ~mod-user-area notes$k_entry_user_area notes$k_noop)	~user-area
	)
    )
    (if (! (& notes-status 1))
	(~notes-error-message)
    )
)
(defun notes-entry-modify-seen
    (
	~name (concat notes-current-entry-name)
	~seen (+ 0)
	~ctx (+ notes-entry-context)
    )
    
    (setq notes-status
	(~notes notes$entry_modify ~ctx 7
	    notes$k_entry_name		~name
	    notes$k_entry_last_status	notes-entry-last-status
	    notes$k_notefile_lastrev	notes-entry-last-revised
	    notes$k_notefile_numnotes	notes-entry-number-of-topics
	    notes$k_notefile_entrytotal	notes-entry-number-of-notes
	    notes$k_notefile_high_uid	notes-entry-high-uid
	    (if ~seen
		notes$k_seen_map
		notes$k_noop
	    )				~seen
	)
    )
    (if (! (& notes-status 1))
	    (~notes-error-message)
    )
)
(defun notes-entry-end (~ctx (+ notes-entry-context))

    (setq notes-status
	(~notes notes$entry_end ~ctx 0)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)


; 
; notefile routines
; 
(declare-buffer-specific
    notes-notefile-create-time
    notes-notefile-result-spec
    notes-notefile-number-of-topics
    notes-notefile-number-of-notes
    notes-notefile-high-uid
    notes-notefile-last-revised
    notes-notefile-title
    notes-notefile-notice
    notes-notefile-restrict-members
    notes-notefile-write-lock
    notes-notefile-moderator
    notes-notefile-moderator-enabled
    notes-notefile-create-keywords
)

(defun notes-notefile-begin
    (
	~file (concat notes-entry-file-name)
	~seen-map (+ 0)
	~ctx (+ notes-notefile-context)
    )
    (~notes notes$notefile_end ~ctx 0)
    (setq notes-notefile-moderator-enabled 0)
    (setq notes-status
	(~notes notes$notefile_begin ~ctx 3
	    notes$k_notefile_file_name		~file
	    notes$k_notefile_default_name	"NOTES$LIBRARY:.NOTE"
	    (if ~seen-map
		notes$k_seen_map
		notes$k_noop)			~seen-map
	    
	    notes$k_notefile_result_spec	notes-notefile-result-spec
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_notefile_moderator		notes-notefile-moderator
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-create
    (
	~file
	~title
	~restrict-members
	~create-keywords
	~ctx (+ notes-notefile-context)
    )
    (~notes notes$notefile_end ~ctx 0)
    (setq notes-notefile-moderator-enabled 0)
    (setq notes-status
	(~notes notes$notefile_begin ~ctx 5
	    notes$k_notefile_file_name		~file
	    notes$k_notefile_default_name	"NOTES$LIBRARY:.NOTE"
	    notes$k_notefile_create		0
	    notes$k_notefile_title		~title
	    notes$k_notefile_restricted		~restrict-members
	    notes$k_user_create_keyword		~create-keywords

	    notes$k_notefile_result_spec	notes-notefile-result-spec
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_notefile_moderator		notes-notefile-moderator
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-moderate
    (
	~enable (! notes-notefile-moderator-enabled)
	~ctx (+ notes-notefile-context)
    )
    (setq notes-status
	(~notes notes$notefile_modify ~ctx 1
	    notes$k_notefile_moderate	~enable
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
    (setq notes-notefile-moderator-enabled (! notes-notefile-moderator-enabled))
)
(defun notes-notefile-set-seen
    (
	~before (concat "")
	~ctx (+ notes-notefile-context)
    )
    (if (= ~before "") (setq ~before "-- 0:0:0"))	; default to midnight
    (setq notes-status
	(~notes notes$notefile_modify ~ctx 2
	    notes$k_note_before_time	(case-string-upper ~before)
	    notes$k_note_mark_seen	0
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-get
    (
	~seen-map (+ 0)
	~ctx (+ notes-notefile-context)
    )
    (setq notes-status
	(~notes notes$notefile_get_info ~ctx 0
	    (if ~seen-map notes$k_seen_map
		notes$k_noop)			~seen-map
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_notefile_moderator		notes-notefile-moderator
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-modify
    (
	~title			~mod-title
	~notice			~mod-notice
	~write-lock		~mod-write-lock
	~restrict-members	~mod-restrict-members
	~create-keywords	~mod-create-keywords

	~ctx (+ notes-notefile-context)
    )
    (setq notes-status
	(~notes notes$notefile_modify ~ctx 4
	    (if ~mod-title notes$k_notefile_title notes$k_noop
	    )		~title
	    (if ~mod-notice notes$k_notefile_notice notes$k_noop
	    )		~notice
	    (if ~mod-restrict-members notes$k_notefile_restricted notes$k_noop
	    )		~restrict-members
	    (if ~mod-create-keywords notes$k_user_create_keyword notes$k_noop
	    )		~create-keywords
	    (if ~mod-write-lock notes$k_notefile_writelock notes$k_noop
	    )		~write-lock
	    ; output items
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_notefile_moderator		notes-notefile-moderator
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-end
    (
	~seen-map (+ 0)
	~ctx (+ notes-notefile-context)
    )
    (setq notes-status
	(~notes notes$notefile_end ~ctx 0
	    (if ~seen-map
		notes$k_seen_map
		notes$k_noop)			~seen-map
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
    (if (! (& notes-status 1))
	(progn
	    ;
	    ; insist on closing the file
	    ; 
	    (~notes notes$notefile_end ~ctx 0)
	    (~notes-error-message)
	)
    )
)
(defun notes-notefile-list-begin
    (
	~file (concat "")
	~ctx (+ notes-notefile-context)
    )
    (~notes notes$notefile_end ~ctx 0)
    (setq notes-status
	(~notes notes$notefile_list_begin ~ctx 2
	    notes$k_notefile_file_name		~file
	    notes$k_notefile_default_name	"NOTES$LIBRARY:*.NOTE"
	    
	    notes$k_notefile_result_spec	notes-notefile-result-spec
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-notefile-list-next (~ctx)
    (setq notes-status
	(~notes notes$notefile_list ~ctx 0
	    notes$k_notefile_result_spec	notes-notefile-result-spec
	    notes$k_notefile_create_time	notes-notefile-create-time
	    notes$k_notefile_numnotes		notes-notefile-number-of-topics
	    notes$k_notefile_lastrev		notes-notefile-last-revised
	    notes$k_notefile_title		notes-notefile-title
	    notes$k_notefile_notice		notes-notefile-notice
	    notes$k_notefile_entrytotal		notes-notefile-number-of-notes
	    notes$k_notefile_high_uid		notes-notefile-high-uid
	    notes$k_notefile_restricted		notes-notefile-restrict-members
	    notes$k_notefile_writelock		notes-notefile-write-lock
	    notes$k_notefile_moderator		notes-notefile-moderator
	    notes$k_user_create_keyword		notes-notefile-create-keywords
	)
    )
;    (if (! (& notes-status 1)) (~notes-error-message))
;    1
)


; 
; user routines
; 
(declare-buffer-specific
    notes-user-name
    notes-user-create-keyword
    notes-user-mail-addr
    notes-user-moderate
    notes-user-access-list
)
(defun notes-user-begin
    (
	~ctx (+ notes-user-context)
	~file-ctx (+ notes-notefile-context)
    )
    (~notes notes$user_end ~ctx 0)
    (setq notes-status
	(~notes notes$user_begin ~ctx 1
	    notes$k_notefile_context ~file-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-user-add
    (
	~name
	~access_list
	~create-keyword
	~moderate
	~mail-addr
	~ctx (+ notes-user-context)
    )
    (setq notes-status
	(~notes notes$user_add ~ctx 5
	    notes$k_user_name		~name
	    notes$k_user_access_list	~access-list
	    notes$k_user_mail_addr	~mail-addr
	    notes$k_user_create_keyword	~create-keyword
	    notes$k_user_moderate	~moderate
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)	
(defun notes-user-modify
    (
	~name
	~access-list	~mod-access-list
	~create-keyword ~mod-create-keyword
	~moderate	~mod-moderate
	~mail-addr	~mod-mail-addr
	~ctx (+ notes-user-context)
    )
    (setq notes-status
	(~notes notes$user_modify ~ctx 5
	    notes$k_user_name		~name
	    (if ~mod-access-list notes$k_user_access_list notes$k_noop
	    )			~access-list
	    (if ~mod-mail-addr notes$k_user_mail_addr notes$k_noop
	    )			~mail-addr
	    (if ~mod-create-keyword notes$k_user_create_keyword notes$k_noop
	    )			~create-keyword
	    (if ~mod-moderate notes$k_user_moderate notes$k_noop
	    )			~moderate

	    notes$k_user_name		notes-user-name
	    notes$k_user_create_keyword	notes-user-create-keyword
	    notes$k_user_mail_addr	notes-user-mail-addr
	    notes$k_user_moderate	notes-user-moderate
	    notes$k_user_access_list	notes-user-access-list
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)	
(defun notes-user-delete
    (
	~name
	~ctx (+ notes-user-context)
    )
    (setq notes-status
	(~notes notes$user_delete ~ctx 1
	    notes$k_user_name		~name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-user-get    
    (
	~name
	~ctx (+ notes-user-context)
    )
    (setq notes-status
	(~notes notes$user_get ~ctx 1
	    notes$k_user_name		~name

	    notes$k_user_name		notes-user-name
	    notes$k_user_create_keyword	notes-user-create-keyword
	    notes$k_user_mail_addr	notes-user-mail-addr
	    notes$k_user_moderate	notes-user-moderate
	    notes$k_user_access_list	notes-user-access-list
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-user-get-next
    (
	~ctx (+ notes-user-context)
    )
    (setq notes-status
	(~notes notes$user_get ~ctx 1
	    notes$k_continue		0

	    notes$k_user_name		notes-user-name
	    notes$k_user_create_keyword	notes-user-create-keyword
	    notes$k_user_mail_addr	notes-user-mail-addr
	    notes$k_user_moderate	notes-user-moderate
	    notes$k_user_access_list	notes-user-access-list
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_users)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-user-end
    (
	~ctx (+ notes-user-context)
    )
    (~notes notes$user_end ~ctx 0)
)


; 
; keyword routines
; 
(declare-buffer-specific
    notes-keyword-name
    notes-keyword-note-id
)
(setq-default notes-keyword-name "")
(setq-default notes-keyword-note-id "")

(defun notes-keyword-begin
    (
	~ctx (+ notes-keyword-context)
	~file-ctx (+ notes-notefile-context)
    )
    (~notes notes$keyword_end ~ctx 0)
    (setq notes-status
	(~notes notes$keyword_begin ~ctx 1
	    notes$k_notefile_context ~file-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-keyword-add
    (
	~keyword
	~note-id (concat "")
	~ctx (+ notes-keyword-context)
    )
    (setq notes-status
	(~notes notes$keyword_add ~ctx 2
	    notes$k_keyword_name	~keyword
	    (if (= "" ~note-id) notes$k_noop notes$k_note_id)	~note-id
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-keyword-delete
    (
	~keyword
	~note-id (concat "")
	~ctx (+ notes-keyword-context)
    )
    (setq notes-status
	(~notes notes$keyword_delete ~ctx 2
	    notes$k_keyword_name	~keyword
	    (if (= "" ~note-id) notes$k_noop notes$k_note_id)	~note-id
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-keyword-get
    (
	~name
	~hint (+ 0)
	~ctx (+ notes-keyword-context)
    )

    (setq notes-status
	(~notes notes$keyword_get ~ctx 2
	    notes$k_keyword_name	    ~name
	    (if ~hint notes$k_keyword_hint_get_note notes$k_noop)   0
	    notes$k_keyword_name	    notes-keyword-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-keyword-get-next
    (
	~hint (+ 0)
	~ctx (+ notes-keyword-context)
    )

    (setq notes-status
	(~notes notes$keyword_get ~ctx 2
	    notes$k_continue	    0
	    (if ~hint notes$k_keyword_hint_get_note notes$k_noop)   0
	    notes$k_keyword_name	    notes-keyword-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_keywords)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-keyword-note-get
    (
	~ctx (+ notes-keyword-context)
    )

    (setq notes-status
	(~notes notes$keyword_get_note ~ctx 0
	    notes$k_note_id		notes-keyword-note-id
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_notes)
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-keyword-modify
    (
	~keyword
	~new-name
	~ctx (+ notes-keyword-context)
    )
    (setq notes-status
	(~notes notes$keyword_modify ~ctx 2
	    notes$k_keyword_name	~keyword
	    (if (= "" ~new-name) notes$k_noop notes$k_keyword_new_name
		)			~new-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-keyword-end
    (
	~ctx (+ notes-keyword-context)
    )
    (~notes notes$keyword_end ~ctx 0)
)


; 
; marker routines
; 
(declare-buffer-specific
    notes-marker-name
    notes-marker-note-id
    notes-marker-entry
)
(setq-default notes-marker-name "")
(setq-default notes-marker-note-id "")
(setq-default notes-marker-entry "")

(defun notes-marker-begin
    (
	~ctx (+ notes-marker-context)
	~file-ctx (+ notes-notebook-context)
    )
    (~notes notes$keyword_end ~ctx 0)
    (setq notes-status
	(~notes notes$keyword_begin ~ctx 1
	    notes$k_notefile_context ~file-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-marker-add
    (
	~marker
	~note-id
	~entry (concat notes-current-entry-name)
	~ctx (+ notes-marker-context)
    )
    (setq notes-status
	(~notes notes$keyword_add ~ctx 4
	    notes$k_unientry		0
	    notes$k_keyword_name	~marker
	    notes$k_note_id		~note-id
	    notes$k_entry_name		~entry
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-marker-delete
    (
	~marker
	~entry (concat notes-current-entry-name)
	~ctx (+ notes-marker-context)
    )
    (setq notes-status
	(~notes notes$keyword_delete ~ctx 2
	    notes$k_entry_name		~entry
	    notes$k_keyword_name	~marker
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-marker-get
    (
	~name
	~entry (concat notes-current-entry-name)
	~ctx (+ notes-marker-context)
    )

    (setq notes-status
	(~notes notes$keyword_get ~ctx 2
	    notes$k_entry_name		    ~entry
	    notes$k_keyword_name	    ~name
	    notes$k_keyword_name	    notes-marker-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
    (setq notes-status
	(~notes notes$keyword_get_note ~ctx 0
	    notes$k_keyword_name	    notes-marker-name
	    notes$k_note_id		    notes-marker-note-id
	    notes$k_entry_name		    notes-marker-entry
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-marker-get-next
    (
	~ctx (+ notes-marker-context)
    )
    
    (setq notes-status
	(~notes notes$keyword_get ~ctx 1
	    notes$k_continue	    0
	    notes$k_keyword_name    notes-marker-name
	)
    )
    (if (! (& notes-status 1))
	(if (= notes-status notes$_no_more_keywords)
	    0
	    (~notes-error-message)
	)
	(progn
	    (setq notes-status
		(~notes notes$keyword_get_note ~ctx 0
		    notes$k_keyword_name    notes-marker-name
		    notes$k_note_id	    notes-marker-note-id
		    notes$k_entry_name	    notes-marker-entry
		)
	    )
	    (if (! (& notes-status 1)) (~notes-error-message))
	    1
	)
    )
)
(defun notes-marker-modify
    (
	~marker
	~new-name
	~entry (concat notes-current-entry-name)
	~ctx (+ notes-marker-context)
    )
    (setq notes-status
	(~notes notes$keyword_modify ~ctx 2
	    notes$k_entry_name		~entry
	    notes$k_keyword_name	~marker
	    (if (= "" ~new-name) notes$k_noop notes$k_keyword_new_name
		)			~new-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-marker-end
    (
	~ctx (+ notes-marker-context)
    )
    (~notes notes$keyword_end ~ctx 0)
)


; 
; note routines
; 
(declare-buffer-specific
    notes-note-is-topic
    notes-note-id
    notes-note-create-time
    notes-note-number-of-responses
    notes-note-author
    notes-note-title
    notes-note-topic-title
    notes-note-topic-id
    notes-note-hidden
    notes-note-write-lock
    notes-note-number-of-records
    notes-note-unseen
    notes-note-pen-name
    notes-note-base-number
    notes-note-reply-number
    notes-note-conference-file-name
    notes-note-keyword
    notes-note-is-ddif

    ;
    ;	used by commands that search for a selection of notes
    ;	examples are directory, search, extract
    ; 
    notes-note-search-string
    notes-note-search-author
    notes-note-search-since
    notes-note-search-before
    notes-note-search-title
    notes-note-search-title-text
    notes-note-search-note-id
    notes-note-search-unseen
    notes-note-search-keyword
)
(setq-default notes-note-conference-file-name "")
(setq-default notes-note-search-string "")
(setq-default notes-note-search-author "")
(setq-default notes-note-search-since "")
(setq-default notes-note-search-before "")
(setq-default notes-note-search-title "")
(setq-default notes-note-search-title-text "")
(setq-default notes-note-search-note-id "")
(setq-default notes-note-search-keyword "")
(setq-default notes-note-is-ddif 0)

(defun notes-note-begin
    (
	~ctx (+ notes-note-context)
	~file-ctx (+ notes-notefile-context)
    )
    (~notes notes$note_end ~ctx 0)
    (setq notes-status
	(~notes notes$note_begin ~ctx 1
	    notes$k_notefile_context	~file-ctx
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-note-read
    (
	~lines (+ notes-note-number-of-records)
	~ctx (+ notes-note-context)
	~timeout (+ 0)
    )
    ~record
    ~last_time
    ~text-type

    (setq notes-status 1)
    
    (if ~timeout
	(setq ~last_time (~notes-seconds))
    )

    (save-excursion
	(end-of-file)
	(while
	    (&
		(>= (setq ~lines (- ~lines 1)) 0)
		(& 1 notes-status)
	    )
	    (if
		(progn
		    (setq notes-status
			(~notes notes$note_get_text ~ctx 1
			    notes$k_continue	0
			    notes$k_text_string	~record
			    notes$k_text_type ~text-type
			)
		    )
		    (if
			(! (& notes-status 1))
			(if (= notes-status notes$_no_more_text)
			    0
			    (~notes-error-message)
			)
			1
		    )
		)
		(progn
		    ;		    (sit-for 0)
		    (setq ~notes-lines-read (+ ~notes-lines-read 1))
		    (if
			(= ~text-type notes$k_ddif_type)
			(progn
			    ; For DDIF just insert the record
			    (setq notes-note-is-ddif 1)
			    (insert-string ~record)
			)
			(progn
			    ;  Otherwise perform <CR><LF> stripping
			    (if (= (substr ~record -2 2) "\r\n")
				(setq ~record
				    (substr ~record 1 (- (length ~record) 2))))
			    (insert-string (concat ~record "\n"))
			)
		    )
		    (if ~timeout
			(progn
			    ~current-time
			    (setq ~current-time (~notes-seconds))
			    (if (< ~current-time (+ ~last_time ~timeout))
				(progn
				    (setq ~last_time ~current-time)
				    (sit-for 0)
				)
			    )
			)
		    )
		)
	    )
	)
    )
    (& 1 notes-status)		; true if read was o.k.
)
(defun notes-note-write
    (
	~reply
	~title
	~hide (+ 0)
	~lock (+ 0)
	~base (concat notes-note-topic-id)
	~ctx (+ notes-note-context)
    )
    ~pen-name

    (setq ~pen-name (~notes-element notes-entry-user-area "\^a" 0))
    (if (= ~pen-name "")
	(setq ~pen-name notes-profile-pen-name))

    (setq notes-status
	(~notes notes$note_add ~ctx 5
	    notes$k_note_title		~title
	    notes$k_note_pen_name	~pen-name
	    (if ~reply notes$k_note_blink_id notes$k_noop
	    )				~base
	    notes$k_note_mark_seen	0
	    notes$k_note_hidden		~hide
	    notes$k_note_writelock	~lock
	    notes$k_note_id		notes-note-id
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
    
    ; 
    ; write the text of the note
    ; 
    (save-excursion
	(beginning-of-file)
	(while (! (eobp))
	    (progn
		(set-mark)
		(end-of-line)
		(setq notes-status
		    (~notes notes$note_add_text ~ctx 2
			notes$k_text_string (region-to-string)
			notes$k_continue 0
		    )
		)
		(if (! (& notes-status 1)) (~notes-error-message))
		(error-occurred (forward-character))
	    )
	)
	(setq notes-status
	    (~notes notes$note_add_text ~ctx 2
		notes$k_text_end		0
		notes$k_continue		0
	    )
	)
	(if (! (& notes-status 1)) (~notes-error-message))
    )
)
(defun notes-note-get
    (
	~op (+ notes$k_note_next_note)
	~arg (+ 0)
	~op2 (+ notes$k_noop)
	~arg2 (+ 0)
	~hint (+ 0) 		; 1 - text, 2 - keys, 3 both
	~seen (+ 0)
	~ctx (+ notes-note-context)
    )
    
    (setq notes-status
	(~notes notes$note_get ~ctx 6
	    (if (= ~op notes$k_note_id)
		notes$k_noop notes$k_note_id)	notes-note-id
	    ~op				~arg
	    ~op2				~arg2
	    (if (& ~hint 1) notes$k_note_hint_get_text notes$k_noop) 0    
	    (if (& ~hint 2) notes$k_note_hint_get_keyword notes$k_noop) 0
	    (if ~seen notes$k_note_mark_seen notes$k_noop)	0
	    
	    notes$k_note_id		notes-note-id
	    notes$k_note_create_time	notes-note-create-time
	    notes$k_note_numresponses	notes-note-number-of-responses
	    notes$k_note_author		notes-note-author
	    notes$k_note_title		notes-note-title
	    notes$k_note_blink_title	notes-note-topic-title
	    notes$k_note_blink_id	notes-note-topic-id
	    notes$k_note_hidden		notes-note-hidden
	    notes$k_note_writelock	notes-note-write-lock
	    notes$k_note_numrecords	notes-note-number-of-records
	    notes$k_note_pen_name	notes-note-pen-name
	    notes$k_note_unseen		notes-note-unseen
	    notes$k_notefile_file_name	notes-note-conference-file-name
	)
    )
    (if (< (string-to-char (substr notes-note-title -1 1)) ' ')
	(setq notes-note-title
	    (substr notes-note-title 0 (- (length notes-note-title) 1))
	)
    )
    (if (< (string-to-char (substr notes-note-topic-title -1 1)) ' ')
	(setq notes-note-topic-title
	    (substr notes-note-topic-title 0 (- (length notes-note-topic-title) 1))
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
    (if
	(! (length notes-note-topic-id))
	(progn			; is a topic
	    (setq notes-note-topic-id notes-note-id)
	    (setq notes-note-topic-title notes-note-title)
	    (setq notes-note-title "")
	    (setq notes-note-is-topic 1)
	)
	(setq notes-note-is-topic 0)	; is a reply
    )
    (setq notes-note-base-number (~notes-element notes-note-id "." 0))
    (setq notes-note-reply-number (~notes-element notes-note-id "." 1))
    (setq notes-note-write-lock (& notes-note-write-lock 1))
    (setq notes-note-hidden (& notes-note-hidden 1))
    (setq notes-note-unseen (& notes-note-unseen 1))
    (setq notes-note-is-ddif 0)
    (!= notes-status notes$_no_such_note)
)
(defun notes-note-search
    (
	~first-time (+ 1)
	~hint (+ 0)
	~seen (+ 0)
	~note-id (concat "")
	~ctx (+ notes-note-context)
    )
    
    (setq notes-status
	(if (= ~first-time 1)
	    (~notes notes$note_get ~ctx 10
		(if (!= "" notes-note-search-note-id)
		    notes$k_note_id notes$k_noop
		)	notes-note-search-note-id
		(if (!= "" notes-note-search-string)
		    notes$k_note_search_string notes$k_noop
		)	notes-note-search-string
		(if (!= "" notes-note-search-author)
		    notes$k_note_author notes$k_noop
		)	notes-note-search-author
		(if (!= "" notes-note-search-since)
		    notes$k_note_since_time notes$k_noop
		)	(case-string-upper notes-note-search-since)
		(if (!= "" notes-note-search-before)
		    notes$k_note_before_time notes$k_noop
		)	(case-string-upper notes-note-search-before)
		(if (!= "" notes-note-search-title)
		    notes$k_note_title notes$k_noop
		)	notes-note-search-title
		(if notes-note-search-unseen
		    notes$k_note_unseen notes$k_noop
		)	1
		(if (!= "" notes-note-search-keyword)
		    notes$k_keyword_name notes$k_noop
		)	notes-note-search-keyword
		(if ~hint notes$k_note_hint_get_text notes$k_noop)	0    
		(if ~seen notes$k_note_mark_seen notes$k_noop)	0
		
		notes$k_note_id			notes-note-id
		notes$k_note_create_time	notes-note-create-time
		notes$k_note_numresponses	notes-note-number-of-responses
		notes$k_note_author		notes-note-author
		notes$k_note_title		notes-note-title
		notes$k_note_blink_title	notes-note-topic-title
		notes$k_note_blink_id		notes-note-topic-id
		notes$k_note_hidden		notes-note-hidden
		notes$k_note_writelock		notes-note-write-lock
		notes$k_note_numrecords		notes-note-number-of-records
		notes$k_note_pen_name		notes-note-pen-name
		notes$k_note_unseen		notes-note-unseen
		notes$k_notefile_file_name	notes-note-conference-file-name
	    )
	    (~notes notes$note_get ~ctx 3
		(if ~first-time notes$k_note_id notes$k_continue
		)				~note-id
		(if ~hint notes$k_note_hint_get_text notes$k_noop)	0    
		(if ~seen notes$k_note_mark_seen notes$k_noop)	0
		
		notes$k_note_id			notes-note-id
		notes$k_note_create_time	notes-note-create-time
		notes$k_note_numresponses	notes-note-number-of-responses
		notes$k_note_author		notes-note-author
		notes$k_note_title		notes-note-title
		notes$k_note_blink_title	notes-note-topic-title
		notes$k_note_blink_id		notes-note-topic-id
		notes$k_note_hidden		notes-note-hidden
		notes$k_note_writelock		notes-note-write-lock
		notes$k_note_numrecords		notes-note-number-of-records
		notes$k_note_pen_name		notes-note-pen-name
		notes$k_note_unseen		notes-note-unseen
		notes$k_notefile_file_name	notes-note-conference-file-name
	    )
	)
    )
    (if (< (string-to-char (substr notes-note-title -1 1)) ' ')
	(setq notes-note-title
	    (substr notes-note-title 0 (- (length notes-note-title) 1))
	)
    )
    (if (< (string-to-char (substr notes-note-topic-title -1 1)) ' ')
	(setq notes-note-topic-title
	    (substr notes-note-topic-title 0 (- (length notes-note-topic-title) 1))
	)
    )
    (if
	(&
	    (! ~first-time)
	    (|
		(= notes-status notes$_no_such_note)
		(= notes-status notes$_no_more_notes)
	    )
	)
	0			; no more to come
	(progn
	    (if (! (& notes-status 1)) (~notes-error-message))
	    (if
		(! (length notes-note-topic-id))
		(progn			; is a topic
		    (setq notes-note-topic-id notes-note-id)
		    (setq notes-note-topic-title notes-note-title)
		    (setq notes-note-title "")
		    (setq notes-note-is-topic 1)
		)
		(setq notes-note-is-topic 0)	; is a reply
	    )
	    (setq notes-note-base-number (~notes-element notes-note-id "." 0))
	    (setq notes-note-reply-number (~notes-element notes-note-id "." 1))
	    (setq notes-note-write-lock (& notes-note-write-lock 1))
	    (setq notes-note-hidden (& notes-note-hidden 1))
	    (setq notes-note-unseen (& notes-note-unseen 1))
	    1
	)
    )
)


(defun notes-note-modify
    (
	~note-id
	~title		~mod-title
	~hidden		~mod-hidden
	~write-lock	~mod-write-lock
	~seen		~mod-seen
	~file-name	~mod-file-name
	~ctx (+ notes-note-context)
    )
    
    (setq notes-status
	(~notes notes$note_modify ~ctx 6
	    notes$k_note_id		~note-id
	    (if ~mod-title notes$k_note_title notes$k_noop
	    )		~title
	    (if ~mod-hidden notes$k_note_hidden notes$k_noop
	    )		~hidden
	    (if ~mod-write-lock notes$k_note_writelock notes$k_noop
	    )		~write-lock
	    (if (& ~mod-seen ~seen) notes$k_note_mark_seen notes$k_noop
	    )		0
	    (if ~mod-file-name notes$k_notefile_file_name notes$k_noop
	    )		~file-name
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
    (setq notes-status
	(~notes notes$note_get ~ctx 1
	    notes$k_note_id		~note-id
		
	    notes$k_note_id		notes-note-id
	    notes$k_note_create_time	notes-note-create-time
	    notes$k_note_numresponses	notes-note-number-of-responses
	    notes$k_note_author		notes-note-author
	    notes$k_note_title		notes-note-title
	    notes$k_note_blink_title	notes-note-topic-title
	    notes$k_note_blink_id	notes-note-topic-id
	    notes$k_note_hidden		notes-note-hidden
	    notes$k_note_writelock	notes-note-write-lock
	    notes$k_note_numrecords	notes-note-number-of-records
	    notes$k_note_pen_name	notes-note-pen-name
	    notes$k_note_unseen		notes-note-unseen
	    notes$k_notefile_file_name	notes-note-conference-file-name
	)
    )
    (if (< (string-to-char (substr notes-note-title -1 1)) ' ')
	(setq notes-note-title
	    (substr notes-note-title 0 (- (length notes-note-title) 1))
	)
    )
    (if (< (string-to-char (substr notes-note-topic-title -1 1)) ' ')
	(setq notes-note-topic-title
	    (substr notes-note-topic-title 0 (- (length notes-note-topic-title) 1))
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))

    (if
	(! (length notes-note-topic-id))
	(progn			; is a topic
	    (setq notes-note-topic-id notes-note-id)
	    (setq notes-note-topic-title notes-note-title)
	    (setq notes-note-title "")
	    (setq notes-note-is-topic 1)
	)
	(setq notes-note-is-topic 0)	; is a reply
    )
    (setq notes-note-base-number (~notes-element notes-note-id "." 0))
    (setq notes-note-reply-number (~notes-element notes-note-id "." 1))
    (setq notes-note-write-lock (& notes-note-write-lock 1))
    (setq notes-note-hidden (& notes-note-hidden 1))
    (setq notes-note-unseen (& notes-note-unseen 1))
    (!= notes-status notes$_no_such_note)
)
(defun notes-note-keyword-get
    (
	~ctx (+ notes-note-context)
    )
    (setq notes-status
	(~notes notes$note_get_keyword ~ctx 1
	    notes$k_continue	    0
	    notes$k_keyword_name    notes-note-keyword
	)
    )
    (if (! (& notes-status 1))
	(if (|
		(= notes-status notes$_no_such_keyword)
		(= notes-status notes$_no_more_keywords)
		(= notes-status notes$_no_such_entry)
		(= notes-status notes$_no_more_entries)
	    )
	    0
	    (~notes-error-message)
	)
	1
    )
)
(defun notes-note-delete
    (
	~note (concat notes-note-id)
	~ctx (+ notes-note-context)
    )
    (setq notes-status
	(~notes notes$note_delete ~ctx 1
	    notes$k_note_id	    ~note
	)
    )
    (if (! (& notes-status 1)) (~notes-error-message))
)
(defun notes-note-end
    (
	~ctx (+ notes-note-context)
    )
    (~notes notes$note_end ~ctx 0)
)


(defun ~notes-add-message-to-log (~text ~error (+ 0))
    (save-excursion
	(temp-use-buffer "notes-messages")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(end-of-file)
	(insert-string (concat ~text "\n"))
    )
    (if ~error
	(error-message ~text)
	(message ~text)
    )
)
(defun ~notes-error-message (~status (+ notes-status))
    
    (if (! (& ~status 1))
	(error-message
	    (if (= (& ~status 7) 0) ; warnings are not signalled
		(sys$getmsg notes-status) 
		(save-excursion
		    (temp-use-buffer "notes-messages")
		    (setq current-buffer-journalled 0)
		    (setq current-buffer-checkpointable 0)
		    (save-excursion
			(backward-character)
			(region-to-string)
		    )
		)
	    )
	)
    )
)
(defun ~notes-message (~status (+ notes-status))
    
    (save-excursion
	(temp-use-buffer "notes-messages")
	(setq current-buffer-journalled 0)
	(setq current-buffer-checkpointable 0)
	(save-excursion
	    (backward-character)
	    (message (region-to-string))
	)
    )
)
(defun ~notes-element (~string ~sep ~index)

    ~start ~count ~pos

    (setq ~count 0)
    (setq ~pos 1)
    ;
    ; scan for ~index''th seperator
    ;
    (while
	(&
	    (<= ~pos (length ~string))
	    (!= ~count ~index)
	)
	(progn
	    (if (= ~sep (substr ~string ~pos 1))
		(setq ~count (+ 1 ~count)))
	    (setq ~pos (+ 1 ~pos))
	)
    )
    (setq ~start ~pos)
    ;
    ;	scan for the next comma
    ;
    (setq ~pos (+ 1 ~pos))
    (while
	(&
	    (<= ~pos (length ~string))
	    (!= ~sep (substr ~string ~pos 1))
	)
	(setq ~pos (+ 1 ~pos))
    )
    (substr ~string ~start (- ~pos ~start))
)
; 
; returns a list of the elements in ~l1 not in ~l2
; 
(defun ~notes-list-diff (~l1 ~l2 ~separator)
    ~l1-index ~l2-index ~result
    ~l1-element ~l2-element ~absent

    (setq ~l1-index 0)
    (setq ~result "")
    (while (!= (setq ~l1-element (~notes-element ~l1 ~separator ~l1-index)) "")
	(progn
	    (setq ~absent 1)
	    (setq ~l2-index 0)
	    (while
		(if ~absent
		    (!=
			(setq ~l2-element
			    (~notes-element ~l2 ~separator ~l2-index))
			""
		    )
		)
		(if (= (case-string-lower ~l2-element)
			(case-string-lower ~l1-element))
		    (setq ~absent 0)
		    (setq ~l2-index (+ 1 ~l2-index))
		)
	    )
	    (if ~absent
		(setq ~result (concat ~result ~separator ~l1-element)))
	    (setq ~l1-index (+ 1 ~l1-index))
	)
    )
    (substr ~result 2 999)
)
; 
; return the time in seconds
; 
(defun
    (~notes-seconds
	~time
	(setq ~time (current-time))
	(setq ~time
	    (+
		(substr ~time 18 2) ; seconds
		(* 60		; convert to seconds
		    (+
			(substr ~time 15 2); minutes
			(* 60	; convert to minutes
			    (+
				(substr ~time 12 2); hours
				(* 24
				    (substr ~time 10 1)	; days
				)
			    )
			)
		    )
		)
	    )
	)
    )
)
[number.ml]
;
;	Convert a number in the following format
;		0xhhhhh		- hex
;		0oooooo		- octal
;		ddddddd		- decimal
;	into hex octal and decimal and display in the message line
;
(progn
(defun
    (change-to-hex num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (concat "0x" (hex-to-string num)))
	(novalue)
    )
)

(defun
    (change-to-octal num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (concat "0x" (hex-to-string num)))
	(novalue)
    )
)

(defun
    (change-to-decimal num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (+ num))
	(novalue)
    )
)

(defun
    string-to-binary (num)
    
    l
    
    (setq l (length num))
    (if (c= (string-to-char num) '0')
	;then
	(progn
	    (setq l (- l 1))
	    (setq num (substr num 2 l))
	    (if (c= (string-to-char num) 'x')
		;then
		(progn	; hex convertion
		    (setq l (- l 1))
		    (setq num (substr num 2 l))
		    (setq num (string-to-hex num))
		)
		;else
		(setq num (string-to-octal num))
	    )
	    ;else
	    (setq num (+ num))	; convert num in decimal
	)
    )
    num
)
(defun
    (number num l original hex oct
	(setq original (arg 1 "Number to convert: "))
	(setq num original)
	(setq l (length num))
	
	(if (c= (string-to-char num) '0')
	    ;then
	    (progn
		(setq l (- l 1))
		(setq num (substr num 2 l))
		(if (c= (string-to-char num) 'x')
		    ;then
		    (progn	; hex convertion
			(setq l (- l 1))
			(setq num (substr num 2 l))
			(setq num (string-to-hex num))
		    )
		    ;else
		    (setq num (string-to-octal num))
		)
		;else
		(setq num (+ 0 num))	; convert num to decimal
	    )
	)
	(message (concat "Converted " original " to: 0x" (hex-to-string num)
		     " 0" (octal-to-string num)
		     " " num)
	)
	(novalue)
    )
)
;
;	Convert a string into a number using hex
;
(defun
    (string-to-hex
	n			; number
	d			; digit
	s			; string
	(setq s (arg 1 ": string-to-hex "))
	(setq n 0)
	(while (> (length s) 0)
	    (progn
		(setq d (string-to-char s))
;(message (concat "s = " s " d = " (char-to-string d))) (sit-for 10)
		(if
		    (c= d '0')	(setq d 0)
		    (c= d '1')	(setq d 1)
		    (c= d '2')	(setq d 2)
		    (c= d '3')	(setq d 3)
		    (c= d '4')	(setq d 4)
		    (c= d '5')	(setq d 5)
		    (c= d '6')	(setq d 6)
		    (c= d '7')	(setq d 7)
		    (c= d '8')	(setq d 8)
		    (c= d '9')	(setq d 9)
		    (c= d 'a')	(setq d 10)
		    (c= d 'b')	(setq d 11)
		    (c= d 'c')	(setq d 12)
		    (c= d 'd')	(setq d 13)
		    (c= d 'e')	(setq d 14)
		    (c= d 'f')	(setq d 15)
		    (error-message (concat "bad digit " (char-to-string d) " in string-to-hex " s))
		)
;(message (concat "n = " n " d = " d)) (sit-for 10)
		(setq n (+ (* n 16) d))
		(setq s (substr s 2 (- (length s) 1)))
	    )
	)
	n
    )
)
;
;	Convert a string into a number using octal
;
(defun
    (string-to-octal
	n			; number
	d			; digit
	s			; string
	(setq s (arg 1 ": string-to-octal "))
	(setq n 0)
	(while (> (length s) 0)
	    (progn
		(setq d (string-to-char s))
		;(message (concat "s = " s " d = " (char-to-string d))) (sit-for 10)
		(if
		    (c= d '0')	(setq d 0)
		    (c= d '1')	(setq d 1)
		    (c= d '2')	(setq d 2)
		    (c= d '3')	(setq d 3)
		    (c= d '4')	(setq d 4)
		    (c= d '5')	(setq d 5)
		    (c= d '6')	(setq d 6)
		    (c= d '7')	(setq d 7)
		    (error-message (concat "bad digit " (char-to-string d) " in string-to-octal " s))
		)
		;(message (concat "n = " n " d = " d)) (sit-for 10)
		(setq n (+ (* n 8) d))
		(setq s (substr s 2 (- (length s) 1)))
	    )
	)
	n
    )
)
;
;	Convert a number into a string of hex digits
;
(defun
    (hex-to-string
	n
	s
	(setq n (arg 1 ": hex-to-string "))
	(setq s (substr "0123456789abcdef" (+ (% n 16) 1) 1))
	(setq n (/ n 16))
	(while (> n 0)
	    (progn
		(setq s (concat (substr "0123456789abcdef" (+ (% n 16) 1) 1) s))
		(setq n (/ n 16))
	    )
	)
	s
    )
)
;
;	Convert a number into a string of octal digits
;
(defun
    (octal-to-string
	n
	s
	(setq n (arg 1 ": octal-to-string "))
	(setq s (substr "01234567" (+ (% n 8) 1) 1))
	(setq n (/ n 8))
	(while (> n 0)
	    (progn
		(setq s (concat (substr "01234567" (+ (% n 8) 1) 1) s))
		(setq n (/ n 8))
	    )
	)
	s
    )
)
)
[page.ml]
;							-*-mlisp-*-
;
; Page oriented commands using virtual buffer bounds
; See end of this file for default
; key bindings.  Defining a function called ~page-load-hook supresses
; these defaults.
; 
; The string value of global variable <FF> defines a page boundary.
; Default is a line beginning with ^L (ascii 12.).  Beginning and
; and of buffer always act like page boundaries.
;
; A. Witkin 6/81
; Modified 2/82 by D. Oran to use multi-autoload (see misc.ml)
(progn

(declare-global <FF>)			       ; It works to redefine this.
(setq <FF> "^\^l")

(defun

; next pagemark, or eob.  Returns 1 if search succeeds, else 0.
(next-ff
  (if (error-occurred (re-search-forward <FF>))
      (progn (end-of-file) 0) 1))


; previous pagemark, or bob. Returns 1 if search succeeds, else 0.
(previous-ff
  (if (error-occurred (re-search-reverse <FF>))
      (progn (beginning-of-file) 0) 1))


;put mark just AFTER previous pagemark, dot just BEFORE next.  i.e. page
;marks aren't part of the region.
(mark-page
  (if (previous-ff) (re-search-forward "")) (set-mark)
  (if (next-ff) (re-search-reverse "")))


; place virtual bounds around page, excluding pagemarks, minimizing
; redisplay.
(narrow-bounds-to-page
  dot tdot zdot
  (error-occurred (widen-bounds))
  (setq dot (dot))
  (setq tdot (save-excursion (beginning-of-window)(dot)))
  (mark-page)
  (setq zdot (- (~mark) 1))
  (narrow-bounds-to-region)
  (goto-character (- tdot zdot))
  (line-to-top-of-window)
  (goto-character (- dot zdot)))
  
;jump to next page, narrow bounds
(Next-page
  (error-occurred (~widen-bounds))
  (next-ff)
  (narrow-bounds-to-page)
  (beginning-of-file))

; Insert a page mark at dot, and narrow bounds to the second
; of the new pages thus created.
(Cut-page
  (widen-bounds)
  (insert-string <FF>)
  (narrow-bounds-to-page))

; Join current page with next one, leave bounds narrowed to the combined
; page
(Join-page
  (end-of-file)
  (widen-bounds)
  (if (next-ff)
      (progn (set-mark)(search-reverse "")
	     (delete-to-killbuffer)))
  (narrow-bounds-to-page))

;jump to previous page, narrow bounds
(Previous-page
  (error-occurred (~widen-bounds))
  (previous-ff)
  (narrow-bounds-to-page)
  (beginning-of-file))

;prompts for page # (1-indexed), and goes there with narrowed bounds.
(goto-page
  page# i
  (setq i 1)
  (setq page# (get-tty-string "Page #: "))
  (error-occurred (~widen-bounds))
  (beginning-of-file)
  (while (< i page#)
	 (next-ff)
	 (setq i (+ 1 i)))
  (narrow-bounds-to-page)
  (beginning-of-file))


; place a listing of page number and 1st non-blank line in buffer Help.
(~generate-page-directory
  dir i dot mark prev-ff-dot ff-dot nb-dot was-narrow dotn markn tdotn
  (setq dotn (dot))			       ; save these guys
  (setq markn (~mark))			       ; to preserve window state.
  (setq tdotn (save-excursion (beginning-of-window) (dot)))
  (setq was-narrow (! (error-occurred (~widen-bounds))))
  (setq dir "")				       ; this string holds directory
  (save-excursion
    (temp-use-buffer "Help")
    (erase-buffer)
    (insert-string "Page	First non-blank line")(newline)
    (insert-string "----	--------------------")(newline)(newline))
  (setq dot (dot))(setq mark (~mark))
  (setq i 1)				       ; page # count
  (beginning-of-file)
  (while (>= ff-dot 0)
	 (setq prev-ff-dot ff-dot)
	 (setq ff-dot (save-excursion (if (next-ff) (dot) -1)))
	 (setq nb-dot (save-excursion (forward-word) (dot)))
	 (if (| (< ff-dot 0)(> ff-dot nb-dot))
	     (progn (goto-character nb-dot)
		    (beginning-of-line)
		    (if (< (dot) prev-ff-dot)
			(goto-character prev-ff-dot))
		    (set-mark)
		    (end-of-line)
		    (setq dir 
			  (concat i 
				  "	"
				  (region-to-string))))
		    (setq dir (concat dir i (char-to-string 10))))
	 (save-excursion
	   (temp-use-buffer "Help")
	   (end-of-file)
	   (insert-string dir)(newline))
	 (setq i (+ 1 i))
	 (if (> ff-dot 0)(goto-character ff-dot)))
  (goto-character mark)(set-mark)
  (goto-character dot)
  (if was-narrow
      (progn
	(narrow-bounds-to-region)
	(goto-character markn)(set-mark)
	(goto-character tdotn)(line-to-top-of-window)
	(goto-character dotn))))

; view page directory using *more*
(view-page-directory
  wll
  (~generate-page-directory)
  (split-current-window)
  (switch-to-buffer "Help")
  (setq wll wrap-long-lines)(setq wrap-long-lines 0)
  (beginning-of-file)
  (*more*)
  (setq wrap-long-lines wll)
  (delete-window)
  (novalue))

; insert page directory listing at dot.
(insert-page-directory
  d
  (setq d (dot))
  (~generate-page-directory)
  (yank-buffer "Help")
  (goto-character d)
  (novalue))

(~mark m (if (error-occurred (setq m (mark))) 0 m)); error-proof (mark)

)

; these are the default key bindings.  If you want to supress them,
; define a function called ~page-load-hook before loading page.
(if (error-occurred (~page-load-hook))
    (progn
      ; view-page-directory and insert-page directory
      ; are not bound by default.
      (bind-to-key "goto-page" "\^x")
      (bind-to-key "Next-page" "\^x]")
      (bind-to-key "Previous-page" "\^x[")
      (bind-to-key "Cut-page" "\^xc")
      (bind-to-key "Join-page" "\^xj")
      (bind-to-key "widen-bounds" "\^xw")
      (bind-to-key "narrow-bounds-to-page" "\^x\^l"))
      (bind-to-key "narrow-bounds-to-region" "\^xq")))

)
[paragraph.ml]
(progn
(if (! (is-bound paragraph-delimiters))
    (progn
	(declare-buffer-specific paragraph-delimiters)
	(setq-default paragraph-delimiters "^\n\\|^[@.' \t].*\n")
    )
)

(defun
    (forward-paragraph
	(beginning-of-line)
	(re-search-forward paragraph-delimiters)
	(while (looking-at "")
	    (search-forward "")
	)
    )
)

(defun
    (backward-paragraph
	(beginning-of-line)
	(previous-line)
	(while (& (looking-at paragraph-delimiters) (! (bobp)))
	    (progn (previous-line) (beginning-of-line)))
	(if (error-occurred (re-search-reverse ""))
	    (beginning-of-file))
	(while (looking-at "")
	    (search-forward "")
	)
    )
)
)
[pascal.ml]
(progn
;;; Pretty Brain Damaged at this point!
(defun
    (electric-pascal-mode
	(use-abbrev-table "Pascal")
	(use-syntax-table "Pascal")
	(Do-Pascal-Bindings)
	(setq mode-string "philips' pascal"))

    (Make-Pascal-Abbrevs
	(use-abbrev-table "Pascal")
	(define-local-abbrev "and" "AND")
	(define-local-abbrev "array" "ARRAY")
	(define-local-abbrev "begin" "BEGIN")
	(define-local-abbrev "boolean" "BOOLEAN")
	(define-local-abbrev "logical" "BOOLEAN")
	(define-local-abbrev "cand" "CAND")
	(define-local-abbrev "case" "CASE")
	(define-local-abbrev "chr" "CHR")
	(define-local-abbrev "const" "CONST")
	(define-local-abbrev "cor" "COR")
	(define-local-abbrev "div" "DIV")
	(define-local-abbrev "do" "DO")
	(define-local-abbrev "downto" "DOWNTO")
	(define-local-abbrev "else" "ELSE")
	(define-local-abbrev "end" "END")
	(define-local-abbrev "exit" "EXIT")
	(define-local-abbrev "exports" "EXPORTS")
	(define-local-abbrev "false" "FALSE")
	(define-local-abbrev "file" "FILE")
	(define-local-abbrev "for" "FOR")
	(define-local-abbrev "forward" "FORWARD")
	(define-local-abbrev "from" "FROM")
	(define-local-abbrev "fun" "FUNCTION")
	(define-local-abbrev "function" "FUNCTION")
	(define-local-abbrev "get" "GET")
	(define-local-abbrev "goto" "GOTO")
	(define-local-abbrev "if" "IF")
	(define-local-abbrev "imports" "IMPORTS")
	(define-local-abbrev "in" "IN")
	(define-local-abbrev "input" "INPUT")
	(define-local-abbrev "integer" "INTEGER")
	(define-local-abbrev "label" "LABEL")
	(define-local-abbrev "long" "LONG")
	(define-local-abbrev "mod" "MOD")
	(define-local-abbrev "module" "MODULE")
	(define-local-abbrev "new" "NEW")
	(define-local-abbrev "nil" "NIL")
	(define-local-abbrev "null" "NIL")
	(define-local-abbrev "not" "NOT")
	(define-local-abbrev "of" "OF")
	(define-local-abbrev "or" "OR")
	(define-local-abbrev "ord" "ORD")
	(define-local-abbrev "otherwise" "OTHERWISE")
	(define-local-abbrev "output" "OUTPUT")
	(define-local-abbrev "packed" "PACKED")
	(define-local-abbrev "private" "PRIVATE")
	(define-local-abbrev "proc" "PROCEDURE")
	(define-local-abbrev "procedure" "PROCEDURE")
	(define-local-abbrev "program" "PROGRAM")
	(define-local-abbrev "put" "PUT")
	(define-local-abbrev "read" "READ")
	(define-local-abbrev "readln" "READLN")
	(define-local-abbrev "real" "REAL")
	(define-local-abbrev "record" "RECORD")
	(define-local-abbrev "repeat" "REPEAT")
	(define-local-abbrev "reset" "RESET")
	(define-local-abbrev "rewrite" "REWRITE")
	(define-local-abbrev "set" "SET")
	(define-local-abbrev "string" "STRING")
	(define-local-abbrev "text" "TEXT")
	(define-local-abbrev "then" "THEN")
	(define-local-abbrev "to" "TO")
	(define-local-abbrev "true" "TRUE")
	(define-local-abbrev "type" "TYPE")
	(define-local-abbrev "until" "UNTIL")
	(define-local-abbrev "var" "VAR")
	(define-local-abbrev "while" "WHILE")
	(define-local-abbrev "with" "WITH")
	(define-local-abbrev "write" "WRITE")
	(define-local-abbrev "writeln" "WRITELN")
    ) ;;; End of Make-Pascal-Abbrevs

    (Do-Pascal-Bindings
	(local-bind-to-key "Enter-Comment-Mode" '{')
	(local-bind-to-key "Leave-Comment-Mode" '}')
	(local-bind-to-key "Enter-Fuzzy-Comment" '*')
	(local-bind-to-key "End-Fuzzy-Comment" ')')
	(local-bind-to-key "Toggle-Comment-Mode" 39) ; Single Quote
    ) ;;; End of Do-Pascal-Bindings

    (Toggle-Comment-Mode
	(if (= abbrev-mode 0)
	    (set "abbrev-mode" 1)
	    (set "abbrev-mode" 0))
	(insert-character (last-key-struck))
    ) ;;; End of Toggle-Comment-Mode

    (Enter-Comment-Mode
	(set "abbrev-mode" 0)
	(insert-character (last-key-struck))
    ) ;;; End of Enter-Comment-Mode

    (Leave-Comment-Mode
	(set "abbrev-mode" 1)
	(insert-character (last-key-struck))
    ) ;;; End of Leave-Comment-Mode

    (Enter-Fuzzy-Comment last-key prev-char
	(setq last-key (last-key-struck))
	(setq prev-char (preceding-char))
	(if (& (= last-key  42)		; 42 = Asterisk
	       (= prev-char 40))	; 40 = open paren
	    (progn
		(set "abbrev-mode" 0)
		(insert-string "*"))
	    (insert-character last-key)))

    (End-Fuzzy-Comment last-key prev-char
	(setq last-key (last-key-struck))
	(setq prev-char (preceding-char))
	(if (& (= last-key  41)		; 41 = close paren
	       (= prev-char 42))	; 42 = Asterisk
	    (set "abbrev-mode" 1))
	(insert-character last-key))
	
    (Pascal-Skeleton name type Prompt colno
	(setq Prompt ": Pascal-Skeleton ")
	(setq name (get-tty-string (concat Prompt "name: ")))
	(if (= name "")
	    (error-occurred "Aborted."))
	(setq colno (current-column))
	(setq type (get-tty-string (concat Prompt name " Result: ")))
	(if (= type "")
	    (progn pos 				; We have a procedure
		(insert-character 10)(to-col colno)
		(insert-string (concat "PROCEDURE " name ))
		(save-excursion
		    (insert-string ";")
		    (insert-character 10)(to-col colno)
		    (insert-string "BEGIN")
		    (insert-character 10)(to-col colno)
		    (insert-string (concat "END; (* " name " *)"))
		    (insert-character 10)))
	    (progn				; We have a function
		(insert-character 10)(to-col colno)
		(insert-string (concat "FUNCTION " name))
		(save-excursion
		    (insert-string (concat ": " type ";"))
		    (insert-character 10)(to-col colno)
		    (insert-string "VAR")
		    (insert-character 10)(to-col (+ colno 4))
		    (insert-string (concat "Answer: " type ";"))
		    (insert-character 10)(to-col colno)
		    (insert-string "BEGIN")
		    (insert-character 10)(to-col (+ colno 4))
		    (insert-string (concat name " := Answer;"))
		    (insert-character 10)(to-col colno)
		    (insert-string (concat "END; (* " name " *)"))
		    (insert-character 10))))
    );;; End of Pascal-Skeleton


) ;;; End of Massive Defun
    (Make-Pascal-Abbrevs)
    (electric-pascal-mode)
    (use-syntax-table "Pascal")
    (modify-syntax-table "word" "_")
)
[process.key]
(progn
    (local-bind-to-key "~shell-grab-last-line" "\^r")
    (local-bind-to-key "~shell-cr" "\r")
    (local-bind-to-key "~shell-nl" "\n")
    (local-bind-to-key "~shell-kill" "\^]")
    (local-bind-to-key "~shell-send-^C" "\^c")
    (local-bind-to-key "~shell-help" "\^_")
)
[process.ml]
; 
; process.ml	- Barry A. Scott 31-Aug-1994
; 
;   This module supports Unix processes, VMS and NT processes
;   The code is OS dependant - however the VMS and NT code is
;   resonable common.
; 
(if (|
	(= operating-system-name "VMS")
	(= operating-system-name "Windows")
    )
    (progn
	(declare-global
	    ~shell
	    shell-cli
	    shell-max-lines		; size of the lines array
	    ~shell-cr-prefix		;  what to put on the start of a
					; command line
	    ~shell-line-suffix		; what to tack on the end of a line
	)
	(declare-buffer-specific
	    ~shell-ready	    	; true when the process will accept input
	    ~shell-input-lines  	; array of input lines past, present and
				    ; future
	    ~shell-first-line	; first line used in the array
	    ~shell-last-line	; number of last line used in the array
	    ~shell-current-line	; number of current line in the array
	    ~shell-recall-line	; number of line to be recalled next
	    shell-ctrlc
	    shell-feeps)
	(if (= operating-system-name "VMS")
	    (progn
		(if (= (concat shell-cli) "0")
		    (setq shell-cli "DCL")
		)
		(setq ~shell-cr-prefix "$")
		(setq ~SHELL-LINE-SUFFIX "")
	    )
	    (= operating-system-name "Windows")
	    (progn
		(if (= (concat shell-cli) "0")
		    (setq shell-cli (concat (getenv "COMSPEC") " /q"))
		)
		(setq ~shell-cr-prefix "")
		(setq ~shell-line-suffix "\n")
	    )
	    (error-message "process.ml does not support " operating-system-name)
	)
	
	(setq-default ~shell-last-line "")	; Last line in the window
	(setq-default shell-ctrlc 44) 	; set to %SYSTEM-F-ABORT
	(setq-default shell-feeps 0)	; # of bell after command complete
	(if (! shell-max-lines) (setq shell-max-lines 20))
    (defun
	(shell i j
	    (setq i (concat "shell-" ~shell))
	    (setq j cli-name)
	    (setq cli-name shell-cli)
	    (if
		(error-occurred
		    (start-DCL-process i)
		)
		(progn
		    (error-message "Cannot create any more DCL processes")
		    (setq cli-name j)
		)
	    )
	    (setq cli-name j)
	    (switch-to-buffer i)
	    (setq ~shell-input-lines (array 1 2 0 (- shell-max-lines 1)))
	    (setq ~shell-current-line -1)
	    (setq ~shell-first-line -1)
	    (setq ~shell-last-line -1)
	    (setq ~shell-recall-line -1)
	    (setq ~shell-ready 1)
	    (erase-buffer)
	    (set-process-output-buffer i i)
	    (~shell-wait i)
	    (setq mode-line-format
		(concat "DCL Interaction Window " ~shell " (%m)")
	    )
	    (setq mode-string "Ready")
	    (setq highlight-region 0)
	    (set-process-input-procedure i "~shell-prompt")
	    (set-process-termination-procedure i "~shell-term")
	    (execute-mlisp-file "process.key")
	    (setq ~shell (+ ~shell 1))
	    (novalue)
	)
    )
    (defun
	(~shell-wait
	    (wait-for-process-input-request (arg 1))
	)
    )
    (defun
	(~shell-kill
	    (error-occurred (kill-process current-buffer-name))
	    (novalue)
	)
    )
    (defun
	(~shell-cr ~shell-line
	    (beginning-of-line)
	    (set-mark)
	    (end-of-line)
	    (setq ~shell-line (region-to-string))
	    (unset-mark)
	    (newline)
	    (~shell-next-input-line ~shell-line ~shell-cr-prefix)
	    (novalue)
	)
    )
    (defun
	(~shell-nl ~shell-line
	    (beginning-of-line)
	    (set-mark)
	    (end-of-line)
	    (setq ~shell-line (region-to-string))
	    (unset-mark)
	    (newline)
	    (~shell-next-input-line ~shell-line "")
	    (novalue)
	)
    )
    (defun
	(~shell-help
	    (save-window-excursion
		(switch-to-buffer "Hack Buffer")
		(erase-buffer)
		(insert-string
		    (concat
			"SHELL: execute DCL commands\n\n"
			"	Key		Action\n"
			"	---		------\n"
			"	^?		print this help text\n"
			"	^R		insert the last execute command at the cursor\n"
			"	<RET>		execute the current line as a DCL command\n"
			"	<LF>		send the current line to the running program\n"
			"	^]		finish this shell session\n"
			"	^C		abort the currently running program\n"
			"	Other characters perform their usual functions.\n"
			"	Variable	Action\n"
			"	--------	------\n"
			"	shell-ctrlc	provide the termination code for ^C\n"
			"	shell-feeps	set the number of bells output when a\n"
			"			process input request is made."
		    )
		)
		(setq mode-line-format "")
		(message "-- Type any character to continue SHELL --")
		(get-tty-character)
		(setq mode-line-format default-mode-line-format)
	    )
	    (novalue)
	)
    )
    (defun
	(~shell-send-^C
	    (if (! ~shell-ready)
		(progn
		    (force-exit-process current-buffer-name shell-ctrlc)
		    (setq mode-string "Aborting")
		)
		(novalue)
	    )
	)
    )
    (defun
	(~shell-term old-buffer
	    (temp-use-buffer (current-process-name))
	    (setq mode-string "Normal")
	    (setq mode-line-format default-mode-line-format)
	    (remove-all-local-bindings)
	    (message
		(concat "Finished interactive DCL "
		    (substr (current-process-name) 7 
			(- (length (current-process-name)) 6)
		    )
		)
	    )
	    (sit-for 0)
	)
    )
    (defun
	(~shell-prompt old-buffer
	    (setq old-buffer current-buffer-name)
	    (temp-use-buffer (current-process-name))
	    (unset-mark)
	    (setq mode-string "Ready")
	    (setq prefix-argument shell-feeps)
	    (prefix-argument-loop (send-string-to-terminal ""))
	    (setq ~shell-ready 1)
	    (~shell-send-next-line-to-process)
	    (temp-use-buffer old-buffer)
	    (sit-for 0)
	    (novalue)
	)
    )
    (defun
	~shell-next-input-line (~shell-line ~shell-line-prefix)
	
	~shell-index
	
	(setq ~shell-last-line (+ 1 ~shell-last-line))    
	(setq ~shell-recall-line ~shell-last-line)
	(setq ~shell-index (% ~shell-last-line shell-max-lines))
	
	(setq-array ~shell-input-lines 1 ~shell-index ~shell-line)
	(setq-array ~shell-input-lines 2 ~shell-index ~shell-line-prefix)
	
	(~shell-send-next-line-to-process)
    )
    (defun
	(~shell-send-next-line-to-process
	    ~shell-index
	    ~shell-line
	    
	    (if ~shell-ready
		(if (< ~shell-current-line ~shell-last-line)
		    (progn
			(setq ~shell-index
			    (%
				(setq ~shell-current-line (+ 1 ~shell-current-line))
				shell-max-lines
			    )
			)
			(setq ~shell-line
			    (concat
				(fetch-array ~shell-input-lines 2 ~shell-index)
				(fetch-array ~shell-input-lines 1 ~shell-index)
			    )
			)
			(send-string-to-process
			    current-buffer-name
			    (concat ~shell-line ~shell-line-suffix)
			)
			(setq mode-string "Processing")
			(setq ~shell-ready 0)
		    )
		)
	    )
	)
    )
    (defun
	(~shell-grab-last-line
	~shell-index
	    (setq ~shell-index (% ~shell-recall-line shell-max-lines))
	    (setq ~shell-recall-line (+ shell-max-lines (- ~shell-recall-line 1)))
	    (end-of-file)
	    (beginning-of-line)
	    (kill-to-end-of-line)
	    (insert-string (fetch-array ~shell-input-lines 1 ~shell-index))
	    (message "Recalled " ~shell-index)
	    (novalue)
	)
    )
    )
    (= operating-system-name "unix")
    (progn
	(declare-buffer-specific last-line)
	(if (! (is-bound ~shell-command))
	    (progn (setq-default ~shell-command (getenv "SHELL"))
		   (if (= ~shell-command "") (setq ~shell-command "/bin/sh"))))
	(if (! (is-bound shell-prompt-template))
	    (setq-default shell-prompt-template
		(if (= ~shell-command "/bin/csh") "^% " "^$ ")))
	; Bound to carriage return.  This function sends the last line of input to
	; the current process.  If we are in the middle of the buffer, we guess
	; about how much to send to the process, then we copy it to the end and send
	; it.  If we're at the end of the buffer, we send the current region.
	(defun
	    (pr-newline
		(end-of-line)
		(if (eobp)
		    (newline)
		    (progn com
			(beginning-of-line)
			(if (looking-at shell-prompt-template)
			    (region-around-match 0))
			(set-mark)
			(end-of-line)
			(forward-character)
			(setq com (region-to-string))
			(end-of-file)
			(set-mark)
			(insert-string com)
		    )
		)
		(setq last-line (region-to-string))
		;(erase-region)
		(send-string-to-process current-buffer-name
		    (concat last-line "\n"))
		(set-mark)
	    )
	)
	; Similar to pr-newline except that if it is invoked on a line in the middle
	; of the buffer, that line is set to the subprocess and the rest of the
	; buffer is erased.
	(defun
	    (pr-newline-kill
		(end-of-line)
		(if (eobp)
		    (progn (newline)
			   (setq last-line (region-to-string)))
		    (progn (beginning-of-line)
			   (if (looking-at shell-prompt-template)
			       (region-around-match 0))
			   (set-mark)
			   (end-of-line)
			   (forward-character)
			   (setq last-line (region-to-string))
			   (set-mark)
			   (end-of-file)
			   (erase-region)
		    )
		)
		(send-string-to-process current-buffer-name
		    (concat last-line "\n"))
		(set-mark)
	    )
	)
	; Expand a file name inline
	(defun (expand-inline-filename
		   (save-excursion
		       (set-mark)
		       (push-back-character ' ')
		       (while (& (! (bolp))
				 (| (>= (preceding-char) 'A')
				    (& (>= (preceding-char) '.')
				       (<= (preceding-char) '9'))))
			      (backward-character))
		       (push-back-character (region-to-string))
		       (erase-region))
		   (insert-string (get-tty-file ": file ") " ")))
	; Send and end-of-file signal to the process if we're at the end of the
	; buffer, else just do what this key normally does.
	(defun
	    (send-eot
		(if (eobp)
		    (send-eof-to-process current-buffer-name)
		)
	    )
	)
	; Simulate keyboard interrupts.
	(defun
	    (send-int-signal
		(force-exit-process current-buffer-name)))
	(defun
	    (send-quit-signal
		(quit-process (active-process))))
	; Insert the last line the user sent to the process in this buffer.
	(defun
	    (grab-last-line
		(end-of-file)
		(set-mark)
		(insert-string last-line)
		(delete-previous-character)
	    )
	)
	; Insert this line at end of buffer for user.
	(defun 
	    (grab-current-line prompt line
		; Find current prompt.
		(save-excursion 
		    (end-of-file)
		    (set-mark)
		    (beginning-of-line)
		    (setq prompt (region-to-string)))
		; If this line has same prompt, skip it.
		(beginning-of-line)
		(if (looking-at (quote prompt))
		    (provide-prefix-argument 
			(length prompt)
			(forward-character)))
		; Now grab text and insert it at end.
		(set-mark)
		(end-of-line)
		(setq line (region-to-string))
		(end-of-file)
		(set-mark)
		(insert-string line)
		(novalue)
	    )
	)
	; Clever hook to cd emacs with shell cds.
(defun
    (shell-cd
	(if (= (- (dot) (mark)) 2)
	    (progn
		(if (error-occurred (change-directory (get-tty-file ": cd ")))
		    (progn
			(delete-previous-word)
			(message "There is no such directory")
		    )
		    (progn
			(insert-string " " (current-directory))
			(pr-newline)
		    )
		)
		0)
	    1)
    )
)
	(defun 
	    (generic-kill-output prompt
		(end-of-file)
		(save-excursion 
		    (set-mark)
		    (beginning-of-line)
		    (setq prompt (region-to-string)))
		(beginning-of-line)
		(set-mark)
		(re-search-reverse (concat "^" (quote prompt)))
		(if (! (dot-is-visible)) (line-to-top-of-window))
		(next-line)
		(erase-region)
		(insert-string "[output flushed]\n")
		(end-of-file)
		(set-mark)
	    )
	)
	; Go to a shell buffer and run a shell there.
	(defun
	    (new-shell name
		(setq name (arg 1 ": new-shell (buffer name) "))
		(pop-to-buffer name)
		(setq highlight-region 0)
		(if (< (process-status name) 0)
		    (progn
			cmd
			  (setq cmd (if (= (substr ~shell-command -3 3) "csh") "exec " ""))
			  (start-process name (concat cmd ~shell-command))
			  (use-abbrev-table "shell")
			  (use-syntax-table "shell")
			  (setq abbrev-mode 1)
			  (setq current-buffer-checkpointable 0)
			  (setq current-buffer-journalled 0)
			  (setq wrap-long-lines 0)
			  (local-bind-to-key "generic-kill-output" "\^X\^K")
			  (local-bind-to-key "pr-newline" "\^M")
			  (local-bind-to-key "pr-newline-kill" "\e\^M")
			  (local-bind-to-key "send-eot" "\^D")
			  (local-bind-to-key "send-int-signal" "\^C")
			  (local-bind-to-key "send-quit-signal" "\034")
			  (local-bind-to-key "grab-last-line" "\e=")
			  (local-bind-to-key "grab-current-line" "\e+")
			  (local-bind-to-key "expand-inline-filename" "\e`")
			  (error-occurred (shell-hook))))
		(end-of-file)
		(novalue)
	    )
	)
	(defun
	    (shell
		  (new-shell "shell")
	    )
	)
	; Set up global stuff.
	(save-excursion
	    (temp-use-buffer "shell")
	    (use-abbrev-table "shell")
	    (setq abbrev-mode 1)
	    (define-local-abbrev "~" (getenv "HOME"))
	    (define-hooked-local-abbrev "cd" "cd" "shell-cd")
	    (use-syntax-table "shell")
	    (modify-syntax-entry "w    ~")
	    (novalue))
    )
)
[queues.ml]
; 
; queues.ml
; 
(execute-mlisp-file "quidef")
(execute-mlisp-file "sjcdef")
(execute-mlisp-file "fscndef")


; 
; show-queue
; 
(defun
    (show-queue
	~status
	~q_name ~q_flags ~q_node ~q_line ~q_status
	~j_header ~j_name ~j_entry ~j_username ~j_status
	~j_intervening ~j_after
	
	(sys$getqui qui$_cancel_operation)
	
	(save-excursion
	    (pop-to-buffer "Show Queue")
	    (erase-buffer)
	    (while
		(& 1
		    (setq
			~status
			(sys$getqui qui$_display_queue
			    qui$_search_name	"*"
			    qui$_search_flags	qui$m_search_wildcard

			    qui$_queue_name	~q_name
			    qui$_queue_flags	~q_flags
			    qui$_scsnode_name	~q_node
			    qui$_queue_status	~q_status
			)
		    )
		)
		(progn
		    (setq ~q_line "")
		    (if (& ~q_flags qui$m_queue_generic)
			(setq ~q_line "generic "))
		    (if (& ~q_flags qui$m_queue_batch)
			(setq ~q_line (concat ~q_line "batch ")))
		    (if (& ~q_flags qui$m_queue_terminal)
			(setq ~q_line (concat ~q_line "terminal ")))
		    (setq ~q_line (concat ~q_line "queue " ~q_name))
		    (insert-string ~q_line)
		    (beginning-of-line)
		    (case-word-capitalize)
		    (end-of-line)
		    (insert-string
			(concat
			    (~shoque-queue-status ~q_status)
			    (if (!= ~q_node "") (concat ", on " ~q_node "::") "")
			    "\n"
			)
		    )
		    (sit-for 0)
		    (setq ~j_header
			(concat
"  Jobnam                    Username     Entry   Status\n"
"  ------                    --------     -----   ------\n"
			)
		    )
		    (while
			(& 1
			    (setq ~status
				(sys$getqui qui$_display_job
				    qui$_search_flags	qui$m_search_wildcard
				    qui$_job_name	~j_name
				    qui$_entry_number	~j_entry
				    qui$_username	~j_username
				    qui$_job_status	~j_status
				    qui$_intervening_jobs ~j_intervening
				    qui$_after_time	~j_after
				)
			    )
			)
			(progn
			    (insert-string ~j_header) (setq ~j_header "")
			    (if ~j_intervening
				(insert-string
				    (sys$fao
					"         (!SL intervening job!%s)\n" "n"
					~j_intervening
				    )
				)
			    )
			    (insert-string
				(sys$fao "  !25AS !12AS !5SL  !AS\n" "ssns"
				    ~j_name
				    ~j_username
				    ~j_entry
				    (~shoque-job-status ~j_status)
				)
			    )
			    (sit-for 0)
			)
		    )
		    ; end of job scan
		)
	    )
	    ; end of queue scan
	    (beginning-of-file)
	)
	(novalue)
    )
)
(defun ~shoque-queue-status (~status)
    (concat
	(if (& ~status qui$m_queue_aligning) ", aligning" "")
	(if (& ~status qui$m_queue_lowercase) ", lowercase" "")
	(if (& ~status qui$m_queue_operator_request) ", operator_request" "")
	(if (& ~status qui$m_queue_paused) ", paused" "")
	(if (& ~status qui$m_queue_pausing) ", pausing" "")
	(if (& ~status qui$m_queue_remote) ", remote" "")
	(if (& ~status qui$m_queue_resetting) ", resetting" "")
	(if (& ~status qui$m_queue_resuming) ", resuming" "")
	(if (& ~status qui$m_queue_server) ", server" "")
	(if (& ~status qui$m_queue_stalled) ", stalled" "")
	(if (& ~status qui$m_queue_starting) ", starting" "")
	(if (& ~status qui$m_queue_stopped) ", stopped" "")
	(if (& ~status qui$m_queue_stopping) ", stopping" "")
	(if (& ~status qui$m_queue_unavailable) ", unavailable" "")
    )
)

(defun ~shoque-job-status (~status)
    (if
	(& ~status qui$m_job_executing) "Executing"
	(& ~status qui$m_job_aborting) "Aborting"
	(& ~status qui$m_job_holding) "Holding"
	(& ~status qui$m_job_retained) "Retained"
	(& ~status qui$m_job_refused) "Refused"
	(& ~status qui$m_job_starting) "Starting"
	(& ~status qui$m_job_timed)
	(sys$fao "Holding until !17AS" "s" ~j_after)
	(sys$fao "Job status = !XL" "n" ~status)
    )
    
)
; 
; submit
; 

(defun
    (submit
	~status ~queue ~file ~job_name ~entry ~j_status ~q_name ~j_name

	(setq ~file (arg 1 "Submit (file) "))
	(setq ~queue (arg 2 (concat "Submit (file) " ~file " (on) ")))
	(setq ~job_name (sys$filescan ~file fscn$_name))
    
	(setq
	    ~status
	    (sys$sndjbc sjc$_enter_file
		sjc$_queue		(case-string-upper ~queue)
		sjc$_file_specification	(case-string-upper ~file)
		sjc$_notify		0
		sjc$_no_log_spool	0
		sjc$_no_log_delete	0

		sjc$_entry_number_output    ~entry
	    )
	)
	(if (! (& 1 ~status))
	    (error-message (sys$getmsg ~status)))
	(setq ~status (sys$getqui qui$_display_queue qui$_search_name ~queue))
	(setq ~status
	    (sys$getqui qui$_display_job
		qui$_search_number	~entry
		qui$_job_status		~j_status
		qui$_queue_name		~q_name
		qui$_job_name		~j_name
	    )
	)

	(message "Job " ~j_name "(queue " ~queue ", entry " ~entry ") " ~j_status)
    )
)
(execute-mlisp-file "cli")
(execute-mlisp-file "sjcdef")
(define-cli-table "print_options" "emacs$share:emacs_print_options")
(defun print-file
    (
	~file (get-tty-file ": print-file ")
	~print-options (get-tty-string ": print-file (options) ")
    )
    ~status

    (setq ~status
	(cli$dcl_parse
	    "print_options"
	    (concat "print/queue=sys$print " ~file " " ~print-options)
	)
    )
    (if (! (& ~status 1))
	(error-message (sys$getmsg ~status)))

    (~queue-do-print
	"s" "file"	sjc$_file_specification	0
	"s" "after"	sjc$_after_time		0
	"f" "burst"	sjc$_file_burst		sjc$_no_file_burst
	"n" "copies"	sjc$_file_copies	0
	"f" "delete"	sjc$_delete_file	sjc$_no_delete_file
	"f" "feed"	sjc$_form_sheet_feed	sjc$_no_form_sheet_feed
	"f" "flag"	sjc$_file_flag		sjc$_no_file_flag
	"s" "form"	sjc$_form_name		0
	"f" "header"	sjc$_page_header	sjc$_no_page_header
	"f" "hold"	sjc$_hold		sjc$_no_hold
	"f" "identify"	0			0
	"n" "job_count"	sjc$_job_copies		0
	"f" "lowercase"	sjc$_lowercase		sjc$_no_lowercase
	"s" "name"	sjc$_job_name		0
	"s" "note"	sjc$_note		0
	"f" "notify"	sjc$_notify		sjc$_no_notify
	"s" "operator"	sjc$_operator_request	0
	"f" "passall"	sjc$_passall		sjc$_no_passall
	"n" "priority"	sjc$_priority		0
	"s" "queue"	sjc$_queue		0
	"f" "restart"	sjc$_restart		sjc$_no_restart
	"s" "setup"	sjc$_file_setup_modules	0
	"f" "space"	sjc$_double_space	sjc$_no_double_space
	"f" "trailer"	sjc$_file_trailer	sjc$_no_file_trailer
	"s" "user"	sjc$_username		0
    )
)
; 
; do-print
;	call this routine with a list of args in 4''s
; 
;	type	name	present-item	negated-item
; 
; 
;(defun
;    (test
;	(pop-to-buffer "print-temp")
;	(erase-buffer)
;	(~queue-print-file "tmp:a.a" (get-tty-string "print options "))
;    )
;)
(defun
    (~queue-do-print
	~argnum ~status ~job_status
	~type ~name ~value
	~command
	(setq ~command "(sys$sndjbc sjc$_enter_file")
	(setq ~argnum 1)
	(while (< ~argnum (nargs))
	    (progn
		(setq ~type (arg ~argnum))
		(setq ~name (arg (+ 1 ~argnum)))
		(setq ~status (cli$present ~name))
		(setq ~command
		    (concat ~command
			(if
			    (|
				(= ~status cli$_present)
				(= ~status cli$_locpres)
				(= ~status cli$_defaulted)
			    )
			    (progn
				(if (= ~type "f")
				    (concat "\n" (arg (+ 2 ~argnum)) " 0")
				    (= ~type "n")
				    (progn
					(cli$get_value ~name ~value)
					(concat "\n" (arg (+ 2 ~argnum)) " " (+ ~value))
				    )
				    (= ~type "s")
				    (progn
					(cli$get_value ~name ~value)
					(concat "\n" (arg (+ 2 ~argnum)) " \"" ~value "\"")
				    )
				)
			    )
			    (| (= ~status cli$_negated) (= ~status cli$_locneg))
			    (concat "\n" (arg (+ 3 ~argnum)) " 0")
			    (= ~status cli$_absent)
			    ""
			    (error-message "Unknown status from cli$present " ~status)
			)
		    )
		)
	    )
;	    (insert-string
;		(sys$fao "!AS is of type !AS, present status is !XL (!AS)\n" "ssns"
;		    ~name ~type ~status (sys$getmsg ~status 15)))
	    (setq ~argnum (+ ~argnum 4))
	)
	(setq ~command (concat ~command " sjc$_job_status_output ~job_status\n)"))
;	(insert-string ~command)
	(setq ~status (execute-mlisp-line ~command))
	(if (! (& 1 ~status))
	    (error-message (sys$getmsg ~status)))
	(message ~job_status)
    )
)
[quidef.ml]
; 
; load up $quidef symbols
; 
(if
    (if (is-bound $quidef-loaded)
	(! $quidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "qui_def")
	(declare-global $quidef-loaded)
	(setq $quidef-loaded 1)
    )
)
[qui_def.ml]
; 
; $quidef
; 
(progn
(~sys_literal_setup_incr "qui$_" 1 1
;
;   function codes
;
	"cancel_operation"
	"display_characteristic"
	"display_file"
	"display_form"
	"display_job"
	"display_queue"
	"translate_queue"
)
(~sys_literal_setup_incr "qui$_" 1 1
; 
;   item codes
; 
	"account_name"
	"after_time"
	"assigned_queue_name"
	"base_priority"
	"characteristic_name"
	"characteristic_number"
	"characteristics"
	"checkpoint_data"
	"cli"
	"completed_blocks"
	"condition_vector"
	"cpu_default"
	"cpu_limit"
	"device_name"
	"entry_number"
	"file_copies"
	"file_copies_chkpt"
	"file_copies_done"
	"file_flags"
	"file_setup_modules"
	"file_specification"
	"file_status"
	"first_page"
	"form_description"
	"form_flags"
	"form_length"
	"form_margin_bottom"
	"form_margin_left"
	"form_margin_right"
	"form_margin_top"
	"form_name"
	"form_number"
	"form_setup_modules"
	"form_stock"
	"form_width"
	"generic_target"
	"intervening_blocks"
	"intervening_jobs"
	"job_copies"
	"job_copies_chkpt"
	"job_copies_done"
	"job_flags"
	"job_limit"
	"job_name"
	"job_reset_modules"
	"job_size"
	"job_size_maximum"
	"job_size_minimum"
	"job_status"
	"last_page"
	"library_specification"
	"log_queue"
	"log_specification"
	"note"
	"operator_request"
	"owner_uic"
	"page_setup_modules"
	"parameter_1"
	"parameter_2"
	"parameter_3"
	"parameter_4"
	"parameter_5"
	"parameter_6"
	"parameter_7"
	"parameter_8"
	"priority"
	"processor"
	"protection"
	"queue_flags"
	"queue_name"
	"queue_status"
	"refusal_reason"
	"requeue_priority"
	"requeue_queue_name"
	"scsnode_name"
	"search_flags"
	"search_name"
	"search_number"
	"submission_time"
	"uic"
	"username"
	"wsdefault"
	"wsextent"
	"wsquota"
	"reserved_boolean_1"
	"reserved_boolean_2"
	"reserved_input_1"
	"reserved_input_2"
	"default_form_name"
	"default_form_number"
	"default_form_stock"
	"job_pid"
	"reserved_output_5"
	"reserved_output_6"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of file_flags item.
; 
	"file_burst"
	"file_burst_exp"
	"file_delete"
	"file_double_space"
	"file_flag"
	"file_flag_exp"
	"file_trailer"
	"file_trailer_exp"
	"file_page_header"
	"file_paginate"
	"file_passall"
	"file_paginate_exp"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of file_status item.
; 
	"file_checkpointed"
	"file_executing"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of form_flags item.
; 
	"form_sheet_feed"
	"form_truncate"
	"form_wrap"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of job_flags item.
; 
	"job_cpu_limit"
	"job_file_burst"
	"job_file_burst_one"
	"job_file_burst_exp"
	"job_file_flag"
	"job_file_flag_one"
	"job_file_flag_exp"
	"job_file_trailer"
	"job_file_trailer_one"
	"job_file_trailer_exp"
	"job_log_delete"
	"job_log_null"
	"job_log_spool"
	"job_lowercase"
	"job_notify"
	"job_restart"
	"job_wsdefault"
	"job_wsextent"
	"job_wsquota"
	"job_file_paginate"
	"job_file_paginate_exp"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of job_status item.
; 
	"job_aborting"
	"job_executing"
	"job_holding"
	"job_inaccessible"
	"job_refused"
	"job_requeue"
	"job_restarting"
	"job_retained"
	"job_starting"
	"job_timed"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of queue_flags item.
; 
	"queue_batch"
	"queue_cpu_default"
	"queue_cpu_limit"
	"queue_file_burst"
	"queue_file_burst_one"
	"queue_file_flag"
	"queue_file_flag_one"
	"queue_file_trailer"
	"queue_file_trailer_one"
	"queue_generic"
	"queue_generic_selection"
	"queue_job_burst"
	"queue_job_flag"
	"queue_job_size_sched"
	"queue_job_trailer"
	"queue_retain_all"
	"queue_retain_error"
	"queue_swap"
	"queue_terminal"
	"queue_wsdefault"
	"queue_wsextent"
	"queue_wsquota"
	"queue_file_paginate"
	"queue_record_blocking"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  structure of queue_status item.
; 
	"queue_aligning"
	"queue_idle"
	"queue_lowercase"
	"queue_operator_request"
	"queue_paused"
	"queue_pausing"
	"queue_remote"
	"queue_resetting"
	"queue_resuming"
	"queue_server"
	"queue_stalled"
	"queue_starting"
	"queue_stopped"
	"queue_stopping"
	"queue_unavailable"
)
(~sys_literal_setup_mask "qui$m_" 1
; 
;  definition of search_flags
; 
	"search_all_jobs"
	"search_wildcard"
	"search_batch"
	"search_symbiont"
	"search_this_job"
)
)
[rectangular.key]
(progn
(defun

    (rect-cut-space %old-value
	(setq %old-value replace-with-white-space)
	(setq replace-with-white-space 1)
	(rectangular-cut)
	(setq replace-with-white-space %old-value)
    )

    (rect-paste-overstrike %old-value
	(setq %old-value overstrike-mode)
	(setq overstrike-mode 1)
	(rectangular-paste)
	(setq overstrike-mode %old-value)
    )

    (rect-cut %old-value
	(setq %old-value replace-with-white-space)
	(setq replace-with-white-space 0)
	(rectangular-cut)
	(setq replace-with-white-space %old-value)
    )

    (rect-paste %old-value
	(setq %old-value overstrike-mode)
	(setq overstrike-mode 0)
	(rectangular-paste)
	(setq overstrike-mode %old-value)
    )

    (rectangular-mode
	(bind-to-key "rect-cut" "\(remove)")
	(bind-to-key "rect-paste" "\(insert)")
	(rectangular-select)
    )

    (lk201-select-mode
	(bind-to-key "~lk201-insert-here" "\(insert)")	; Insert Here
	(bind-to-key "~lk201-remove" "\(remove)")	; Remove
	(set-mark)
    )

)

(bind-to-key "rect-paste-overstrike" "\(gold)\(insert)"); gold Insert
(bind-to-key "rect-cut-space" "\(gold)\(remove)")	; gold Remove

(bind-to-key "rectangular-mode" "\(gold)\(select)")	; gold select
(bind-to-key "lk201-select-mode" "\(select)")		; Select key

)
[rectangular.ml]
; 
; 	RECTANGULAR Cut and Paste
;
;       updated:
;	Gim Hom		20-May-1990 	rewrote package to handle journaling
;					by using a temporary buffer.
;
;	Gim Hom		30-Dec-1987	Converted to EMACSV5: used
;					(forward-character repeat-value) ...; 
;					added rectangular-copy
;
;       Gim Hom         18-Nov-1986	Fixed end-of-file bug
;
;       Gim Hom         16-DEC-1985     Improved  performance  by  using
;					(provide-prefix-argument ...  in
;					place of (while ...)
;
;       Gim Hom          3-SEP-1985     Made user-friendly, rectangle is
;                                       now any two opposite corners.
;
;       Gim Hom          2-SEP-1985	Redefined keys.     Fixed so will
;					work in EMACS REPLACE MODE.
;
;  Rectangular cut  and  paste  is modelled after EDD Rectangular Cut and
;  paste.   It  will cut and paste rectangular areas bounded by the upper
;  left  corner  and  the  lower right corner.  It handles tabs and fixes
;  problems in the AREA.ML.
;
;  To handle  all  possible  options, there are two modes for cut and two
;  modes for paste.
;
;  RECTANGULAR-COPY
;   copy the area of text into the buffer "Area"
;
;  RECTANGULAR-CUT
;   replace-with-white-space =>1  replace the deleted area with white space.
;   replace-with-white-space =>0  just delete the area.
;
;  RECTANGULAR-PASTE
;   overstrike-mode =>1  paste in the area in overstrike (replace) mode.
;   overstrike-mode =>0  paste in the area in insert mode.
;
;  To use,   position  the  cursor  to
;  the upper right corner and execute:   (rectangular-select)
;
;  Now position   the  cursor  to  the
;  lower  left  corner   and  execute:   (rectangular-cut)
;
;  To insert  the  text, just position
;  the  cursor  to  the  desired upper
;  left     corner     and    execute:   (rectangular-paste)
;
;  Notes:
;  1.  This  is truly  a rectangular cut and paste.  If the paste area is
;      greater  than the currect file, it will add spaces and lines to so
;      that the full paste area is inserted.
;  2.  The  region to be cut and pasted will have tabs removed and spaces
;      inserted.
;  3.  White  spaces  are  deleted  at the end of the lines in the affect
;      region.
;  4.  Default bindings is the VT200 Select, Remove and Insert keys.
;
;       GOLD SELECT will bind
;	   REMOVE    rectangular-cut with replace-with-white-space = 0
;	   INSERT    rectangular-paste with overstrike-mode = 0
;
;	SELECT will bind
; 	   REMOVE    ~lk201-insert-here
;	   INSERT    ~lk201-remove
;
;	Thus hitting GOLD SELECT will put the REMOVE and INSERT keys
;	in "rectangular CUT/PASTE" mode.
;
;	GOLD REMOVE is permanently bound to
;	   rectanugular-cut with replace-with-white-space = 1.
;	GOLD INSERT is permanently bound to
;	   rectangular-paste with overstrike-mode = 1.
;
;  5.  Because  of  the  intelligent  QIO's  of  EMACS,  when  you  cut a
;      rectangular  area  where  the  upper  right corner is a space, the
;      highlighted  space  may  remain hightlighted after the cut.  Don't
;      worry  ...   Just  do a refresh screen.  At other times the screen
;      may get messed up; again just do a refresh screen.


(declare-global
    right-column		; right column of rectangle
    start-point			; upper left corner
    end-point 			; lower right corner
    left-column			; left column of rectangle
    replace-with-white-space 	; if 1, replace deleted area with space
    overstrike-mode 		; if 1, paste by overstriking (replace mode)
)

(defun
    (rectangular-select
	(set-start-point)
	(mark-start-point)
	(message
           "Rectangular selection started ...  Use redraw-display to cancel."
	)
    )

    (rectangular-copy
	(rc-copy)
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )
    
    (rectangular-cut
	(rc-copy)
	(erase-region)
	(yank-buffer "rc-buffer")
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )
    
    (rectangular-paste area-width line count
	(rc-paste)
	(erase-region)
	(beginning-of-line)
	(yank-buffer "rc-buffer")
;	(delete-buffer "rc-buffer")	; avoid journaling on buffer creation
	(exchange-dot-and-mark)
	(goto-left-column)
	(unset-mark)
    )

    (rc-copy  line count
	(set-end-point)
	(save-excursion			; avoid problems on last line
	    (next-line)
	    (if (eobp) (newline))
	)
	(check-corners)
	(goto-character start-point)
	(beginning-of-line)
	(set-mark)
	(goto-character end-point)
	(end-of-line)
	(copy-region-to-buffer "rc-buffer")
	(save-excursion
	    (temp-use-buffer "rc-buffer")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (end-of-file)
	    (setq line (current-line-number))
	    (add-white-space)
	    (save-excursion (temp-use-buffer "Area")
		(setq current-buffer-checkpointable 0)
		(setq current-buffer-journalled 0)
		(erase-buffer)
	    )
	    (untabify)
	    (beginning-of-file)
	    (setq count 0)
	    (while (> line count)
		(goto-left-column)
		(set-mark)
		(goto-right-column)
		(append-region-to-buffer "Area")
		(erase-region)
;		(unset-mark)
		(save-excursion
		    (temp-use-buffer "Area")
		    (newline)
		)
		(if replace-with-white-space
		    (to-col right-column)
		)
		(next-line)
		(setq count (+ count 1))
	    )
	    (beginning-of-file)
	    (goto-left-column)
	)
;	(delete-buffer "rc-buffer")	; avoid journaling on buffer creation
    )

    (rc-paste %rc-string
	(set-start-point)
	(save-excursion
	    (temp-use-buffer "Area")
	    (beginning-of-file)
	    (end-of-line)
	    (setq area-width (current-column))
	    (end-of-file)
	    (setq line (current-line-number))
	    (beginning-of-file)
	)
	(beginning-of-line)
	(set-mark)
	(next-line line)
	(end-of-line)
	(copy-region-to-buffer "rc-buffer")
	(save-excursion
	    (temp-use-buffer "rc-buffer")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (untabify)
	    (beginning-of-file)
	    (setq count 1)
	    (goto-left-column)
	    (while (> line count)
		(save-excursion
		    (temp-use-buffer "Area")
		    (set-mark)
		    (end-of-line)
		    (setq %rc-string (region-to-string))
		    ;		(erase-region)
		    ;		(delete-next-character)
		    (forward-character)
		)
		;	    (untabify-line)
		(if overstrike-mode       ; delete spaces before inserting
		    (delete-column-width)
		)
		(insert-string %rc-string)
		(end-of-line)
		(delete-white-space)
		(if (eobp)
		    (progn
			(newline)
			(to-col left-column)
		    )
		    (progn
			(next-line)
			(goto-left-column)
		    )
		)
		(setq count (+ count 1))
	    )
	)
    )
    

;
;	supporting routines; listed in alphabetical order.
;

    (add-white-space		; add white space to form a rectangle
	(beginning-of-file)
	(setq count 0)
	(while (> line count)
	    (end-of-line)
	    (if (< (current-column) right-column)
		(to-col right-column)
	    )
	    (next-line)
	    (setq count (+ count 1))
	)
    )

    (check-corners new-right-column new-end-point
	(if (& (> (- right-column 1) left-column) (< end-point start-point))
	    			; reverse start/end; columns okay
	    (progn
		(setq new-end-point start-point)
		(setq start-point end-point)
		(setq end-point new-end-point)
		(goto-character start-point)
		(untabify-line)
		(goto-left-column)
		(setq start-point (dot))
		(goto-character end-point)
		(end-of-line)
		(if (< (current-column) right-column)
		    (to-col right-column)
		)
		(untabify-line)
		(beginning-of-line)
		(forward-character (- right-column 1))
		(setq end-point (dot))
	    )
	)
	(if (& (< (- right-column 1) left-column) (> end-point start-point))
	    			; start/end  ok, reverse columns
	    (progn
		(goto-character start-point)
		(untabify-line)
		(setq new-right-column left-column)
		(setq left-column (- right-column 1))
		(setq right-column (+ new-right-column 1))
		(goto-left-column)
		(setq start-point (dot))
		(goto-character end-point)
		(end-of-line)
		(if (< (current-column) right-column)
		    (to-col right-column)
		)
		(untabify-line)
		(beginning-of-line)
		(forward-character (- right-column 1))
		(setq end-point (dot))
	    )
	)
	(if (& (< end-point start-point) (<= (- right-column 1) left-column))
				; reverse start/end; reverse columns
	    (progn
		(setq new-end-point start-point)
		(setq start-point end-point)
		(setq end-point new-end-point)
		(setq new-right-column left-column)
		(setq left-column (- right-column 1))
		(setq right-column (+ new-right-column 1))
	    )
	)
    )

    (delete-column-width temp-counter	; delete width of area
	(set-mark)                      ; must not contain any tabs
	(end-of-line)
	(if (>= (current-column)  (+ left-column area-width))
	    (progn
		(goto-character (mark))
	        (forward-character (- area-width 1))
	    )
	)
	(erase-region)
;	(unset-mark)
    )

    (goto-left-column			; go to the left side of the area
	(end-of-line)			; must not contain any tabs
	(if (< (current-column) left-column)
	    (to-col  left-column)
	    (progn
		(beginning-of-line)
		(forward-character (- left-column 1))
	    )
	)
    )

    (goto-right-column		; go to the right side of the area
        (forward-character (- right-column (current-column)))
    )

    (mark-start-point 		; this will highlight the start point
	(send-string-to-terminal (concat "\e[7m"
              (char-to-string (following-char)) "\e[0m" )
	)
    )

    (set-start-point
	(if (eobp)
	    (progn
		(insert-string " \n")
		(backward-character)
		(backward-character)
	    )
	)
	(setq start-point (dot))
	(setq left-column (current-column))
    )
    
    (set-end-point
	(if (eobp)
	    (progn
		(insert-string " \n")
		(backward-character)
		(backward-character)
	    )
	)
	(setq end-point (dot))
	(setq right-column (+ (current-column) 1))
    )

    (untabify	; remove tabs, and put in correct number of spaces.
	col
	(beginning-of-file)
	(while (! (error-occurred (search-forward "\t")))
	    (progn
		(setq col (current-column))
		(delete-previous-character)
		(while (< (current-column) col)
		    (insert-string " "))
	    )
	)
    )

    (untabify-line  col
	(save-restriction
	    (beginning-of-line) (set-mark) (end-of-line) (narrow-region)
	    (beginning-of-line)
	    (while (! (error-occurred (search-forward "\t")))
		(progn (setq col (current-column))
		    (delete-previous-character)
		    (while (< (current-column) col)
			(insert-string " "))
		)
	    )
	    (unset-mark)
	    (widen-region)
	)
    )

)
(execute-mlisp-file "rectangular.key")
[scribe.ml]
(defun    
    (apply-look go-forward
	(save-excursion c
	    (if (! (eolp)) (forward-character))
	    (setq go-forward -1)
	    (backward-word)
	    (setq c (get-tty-character))
	    (if (> c ' ')
		(progn (insert-character '@')
		    (insert-character c)
		    (insert-character '[')
		    (forward-word)
		    (setq go-forward (dot))
		    (insert-character ']')
		)
	    )
	)
	(if (= go-forward (dot)) (forward-character))
    )
)

(defun
    (scribe-mode
	(remove-all-local-bindings)
	(if (! buffer-is-modified)
	    (save-excursion
		(error-occurred
		    (goto-character 2000)
		    (search-reverse "LastEditDate=""")
		    (search-forward """")
		    (set-mark)
		    (search-forward """")
		    (backward-character)
		    (delete-to-killbuffer)
		    (insert-string (current-time))
		    (setq buffer-is-modified 0)
		)
	    )
	)
	(local-bind-to-key "justify-paragraph" "\ej")
	(local-bind-to-key "apply-look" "\el")
	(setq right-margin 77)
	(setq mode-string "Scribe")
	(setq case-fold-search 1)
	(use-syntax-table "text-mode")
	(modify-syntax-table "word" "-'")
	(use-abbrev-table "text-mode")
	(setq left-margin 1)
	(novalue)
    )
)

(scribe-mode)
(novalue)
[sentence.ml]
(if (! (is-bound sentence-delimiters))
    (progn
	(declare-global sentence-delimiters)
	(setq sentence-delimiters "[.!?][ \n\t][ \n\t]*")
    ))

(defun (forward-sentence
	   (re-search-forward sentence-delimiters)
	   (while (looking-at "")
	       (search-forward "")))
)

(defun (backward-sentence stpos searchp
	   (setq stpos (dot))
	   (preceding-char)
	   (if (error-occurred (re-search-reverse sentence-delimiters))
	       (beginning-of-file))
	   (setq searchp (dot))
	   (while (looking-at "")
	       (search-forward ""))
	   (if (>= (dot) stpos)
	       (progn
		   (goto-character searchp)
		   (if (error-occurred (re-search-reverse ""))
		       (beginning-of-file))
		   (while (looking-at "")
		       (search-forward "")))
	   )
       )
)
[shell.mlp]
(progn
    (argc)
    (shell)
)
[show-buffer.ml]
(progn		; for EMACS v5
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
		  "                 File:  " current-buffer-file-name "\n\n"

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
		  "     Mode line format: " mode-line-format 
;		  "                        "
		)
	    )
	    (delete-other-windows)
	    (switch-to-buffer "show-buffer-data")
	    (setq mode-line-format 
	 	" %[%hBuffer: %1*%18b %h   Depress space bar to return and continue ...              ")
	    (beginning-of-file)
	    (beginning-of-line)
	    (setq current-buffer-checkpointable 0)
	    (erase-buffer)
	    (insert-string data)
	    (setq buffer-is-modified 0)
	    (get-tty-character)
;	    (delete-buffer "show-buffer-data")
	    (setq mode-line-format default-mode-line-format)
	    (novalue)
	)
    )
)
)
[show-system.ml]
; 
; show-system.ml
; 
(progn

(execute-mlisp-file "jpidef")
(execute-mlisp-file "syidef")

(declare-global ~show-system-process-states)
(setq ~show-system-process-states (array 1 14))

(setq-array ~show-system-process-states sch$c_colpg "colpg")
(setq-array ~show-system-process-states sch$c_mwait "mwait")
(setq-array ~show-system-process-states sch$c_cef "cef")
(setq-array ~show-system-process-states sch$c_pfw "pfw")
(setq-array ~show-system-process-states sch$c_lef "lef")
(setq-array ~show-system-process-states sch$c_lefo "lefo")
(setq-array ~show-system-process-states sch$c_hib "hib")
(setq-array ~show-system-process-states sch$c_hibo "hibo")
(setq-array ~show-system-process-states sch$c_susp "susp")
(setq-array ~show-system-process-states sch$c_suspo "suspo")
(setq-array ~show-system-process-states sch$c_fpg "fpg")
(setq-array ~show-system-process-states sch$c_com "com")
(setq-array ~show-system-process-states sch$c_como "como")
(setq-array ~show-system-process-states sch$c_cur "cur")

(defun
    (show-system
	~nodename ~version ~boottime
	
	~ctx
	~pid ~prcnam ~state ~pri ~bufio ~dirio ~cputim ~faults ~ppg ~gpg
	~parent-pid ~mode
	
	(save-excursion
	    (setq ~ctx -1)
	    (pop-to-buffer "Show System")
	    (erase-buffer)
	    (sys$getsyi ""
		syi$_nodename	~nodename
		syi$_version	~version
		syi$_boottime	~boottime
	    )
	    (insert-string
		(concat
		    (sys$fao "VAX/VMS !4AS on node !6AS !%D  Up since: !17AS\n"
			"ssns" ~version ~nodename 0 ~boottime
		    )
		    "  Pid    Process Name    State  Pri      I/O       CPU       Page flts Ph.Mem\n"
		)
	    )	
	    (while
		(& 1 (sys$getjpi "i" ~ctx
			 jpi$_pid	~pid
			 jpi$_prcnam	~prcnam
			 jpi$_state	~state
			 jpi$_pri	~pri
			 jpi$_bufio	~bufio
			 jpi$_dirio	~dirio
			 jpi$_cputim	~cputim
			 jpi$_pageflts	~faults
			 jpi$_ppgcnt	~ppg
			 jpi$_gpgcnt	~gpg
			 jpi$_mode	~mode
			 jpi$_owner	~parent-pid
		     )
		)

		(progn
		    (insert-string
			(sys$fao
			    "!XL !16AS!7AS!3SL   !6SL !15AS   !7SL  !5SL  !1AS\n" 
			    "nssnnsnns"
			    ~pid ~prcnam
			    (fetch-array ~show-system-process-states ~state)
			    ~pri (+ ~bufio ~dirio)
			    (~sys-time ~cputim)
			    ~faults (+ ~ppg ~gpg)
			    (if ~parent-pid
				"S"
				(if (= ~mode jpi$k_network)
				    "N"
				    (= ~mode jpi$k_batch)
				    "B"
				    ""
				)
			    )
			)
		    )
		    (sit-for 0)
		)
	    )
	    (beginning-of-file)
	)
	(novalue)
    )
)
(defun ~sys-time (~time)
    ~hours ~minutes ~seconds ~frac-secs
    
    (setq ~frac-secs (% ~time 100)) (setq ~time (/ ~time 100))
    (setq ~seconds (% ~time 60)) (setq ~time (/ ~time 60))
    (setq ~minutes (% ~time 60)) (setq ~time (/ ~time 60))
    (setq ~hours (% ~time 24)) (setq ~time (/ ~time 24))
    (sys$fao "!3SL !2ZL:!2ZL:!2ZL.!2ZL" "nnnnn"
	~time ~hours ~minutes ~seconds ~frac-secs	
    )
)
)
[sjcdef.ml]
; 
; load up $sjcdef symbols
; 
(if
    (if (is-bound $sjcdef-loaded)
	(! $sjcdef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "sjc_def")
	(declare-global $sjcdef-loaded)
	(setq $sjcdef-loaded 1)
    )
)
[sjc_def.ml]
; 
; $sjcdef
; 
(progn
    (~sys_literal_setup_incr "sjc$_" 1 1
	    "abort_job"
	    "add_file"
	    "alter_job"
	    "alter_queue"
	    "assign_queue"
	    "batch_checkpoint"
	    "batch_service"
	    "close_delete"
	    "close_job"
	    "create_job"
	    "create_queue"
	    "deassign_queue"
	    "define_characteristic"
	    "define_form"
	    "delete_characteristic"
	    "delete_form"
	    "delete_job"
	    "delete_queue"
	    "enter_file"
	    "merge_queue"
	    "pause_queue"
	    "reset_queue"
	    "start_accounting"
	    "start_queue"
	    "start_queue_manager"
	    "stop_accounting"
	    "stop_queue"
	    "stop_queue_manager"
	    "synchronize_job"
	    "write_accounting"
    )
    (~sys_literal_setup_incr "sjc$_" 1 1
    ;
    ;  item codes
    ;
	    "accounting_message"
	    "accounting_types"
	    "after_time"
	    "no_after_time"
	    "alignment_mask"
	    "alignment_pages"
	    "base_priority"
	    "batch"
	    "no_batch"
	    "batch_input"
	    "batch_output"
	    "characteristic_name"
	    "characteristic_number"
	    "no_characteristics"
	    "checkpoint_data"
	    "no_checkpoint_data"
	    "cli"
	    "no_cli"
	    "cpu_default"
	    "no_cpu_default"
	    "cpu_limit"
	    "no_cpu_limit"
	    "create_start"
	    "delete_file"
	    "no_delete_file"
	    "destination_queue"
	    "device_name"
	    "double_space"
	    "no_double_space"
	    "entry_number"
	    "entry_number_output"
	    "file_burst"
	    "file_burst_one"
	    "no_file_burst"
	    "file_copies"
	    "file_flag"
	    "file_flag_one"
	    "no_file_flag"
	    "file_identification"
	    "file_setup_modules"
	    "no_file_setup_modules"
	    "file_specification"
	    "file_trailer"
	    "file_trailer_one"
	    "no_file_trailer"
	    "first_page"
	    "no_first_page"
	    "form_description"
	    "form_length"
	    "form_margin_bottom"
	    "form_margin_left"
	    "form_margin_right"
	    "form_margin_top"
	    "form_name"
	    "form_number"
	    "form_setup_modules"
	    "no_form_setup_modules"
	    "form_sheet_feed"
	    "no_form_sheet_feed"
	    "form_stock"
	    "form_truncate"
	    "no_form_truncate"
	    "form_width"
	    "form_wrap"
	    "no_form_wrap"
	    "generic_queue"
	    "no_generic_queue"
	    "generic_selection"
	    "no_generic_selection"
	    "generic_target"
	    "hold"
	    "no_hold"
	    "job_burst"
	    "no_job_burst"
	    "job_copies"
	    "job_flag"
	    "no_job_flag"
	    "job_limit"
	    "job_name"
	    "job_reset_modules"
	    "no_job_reset_modules"
	    "job_size_maximum"
	    "no_job_size_maximum"
	    "job_size_minimum"
	    "no_job_size_minimum"
	    "job_size_scheduling"
	    "no_job_size_scheduling"
	    "job_status_output"
	    "job_trailer"
	    "no_job_trailer"
	    "last_page"
	    "no_last_page"
	    "library_specification"
	    "no_library_specification"
	    "log_delete"
	    "no_log_delete"
	    "log_queue"
	    "log_specification"
	    "no_log_specification"
	    "log_spool"
	    "no_log_spool"
	    "lowercase"
	    "no_lowercase"
	    "new_version"
	    "next_job"
	    "note"
	    "no_note"
	    "notify"
	    "no_notify"
	    "operator_request"
	    "no_operator_request"
	    "owner_uic"
	    "page_header"
	    "no_page_header"
	    "page_setup_modules"
	    "no_page_setup_modules"
	    "paginate"
	    "no_paginate"
	    "parameter_1"
	    "parameter_2"
	    "parameter_3"
	    "parameter_4"
	    "parameter_5"
	    "parameter_6"
	    "parameter_7"
	    "parameter_8"
	    "no_parameters"
	    "passall"
	    "no_passall"
	    "priority"
	    "processor"
	    "no_processor"
	    "protection"
	    "queue"
	    "queue_file_specification"
	    "relative_page"
	    "requeue"
	    "restart"
	    "no_restart"
	    "retain_all_jobs"
	    "retain_error_jobs"
	    "no_retain_jobs"
	    "scsnode_name"
	    "search_string"
	    "swap"
	    "no_swap"
	    "terminal"
	    "no_terminal"
	    "top_of_file"
	    "user_identification"
	    "wsdefault"
	    "no_wsdefault"
	    "wsextent"
	    "no_wsextent"
	    "wsquota"
	    "no_wsquota"
	    "account_name"
	    "uic"
	    "username"
	    "buffer_count"
	    "extend_quantity"
	    "record_blocking"
	    "no_record_blocking"
	    "queman_restart"
	    "no_queman_restart"
	    "default_form_name"
	    "default_form_number"
    )
    (~sys_literal_setup_mask "sjc$m_" 1
    ; 
    ;  structure of accounting_types item.  these definitions must track
    ;  exe$gl_acmflags.
    ; 
	    "acct_process"
	    "acct_image"
	    "acct_interactive"
	    "acct_login_failure"
	    "acct_subprocess"
	    "acct_detached"
	    "acct_batch"
	    "acct_network"
	    "acct_print"
	    "acct_message"
    )
)
[spawn.ml]
; 
; spawn.ml
; 
;   Barry A. Scott  26-Mar-1985
(defun
    (spawn
	(return-to-monitor)
    )
)
[spell.key]
(progn
    (bind-to-key "spell-word-in-buffer" "\^Xs")
    (bind-to-key "spell-region" "\^Xr")
    (bind-to-key "spell-buffer" "\^Xf")
    (bind-to-key "spell-line" "\^Xl")
    (bind-to-key "spell-add-word-in-buffer" "\^Xa")
    (bind-to-key "spell-ignore-word-in-buffer" "\^Xi")
)
[spell.ml]
(progn
    (external-function "emacs$spell" "emacs$share:emacs_spell_shr")

    (if (! (is-bound ~spell-init))
        (progn
	    (declare-global
		~spell-init
	    )
	    (setq ~spell-init 0)
	)
    )
    (if (! (is-bound spell-master-dictionary))
        (progn
	    (declare-global spell-master-dictionary)
	    (setq spell-master-dictionary "lngsplaed")
	)
    )
    (if (! (is-bound spell-ram-dictionary))
        (progn
	    (declare-global spell-ram-dictionary)
	    (setq spell-ram-dictionary "lngsplaem")
	)
    )
    (if (! (is-bound spell-memory-size))
        (progn
	    (declare-global spell-memory-size)
	    (setq spell-memory-size 4096)
	)
    )
    (if (! (is-bound spell-personal-dictionary))
        (progn
	    (declare-global spell-personal-dictionary)
	    (setq spell-personal-dictionary "personal")
	)
    )
    (if (! (is-bound spell-line-exit-on-action))
        (progn
	    (declare-global
		spell-line-exit-on-action
	    )
	    (setq spell-line-exit-on-action 0)
	)
    )
    (declare-global ~spell-in-spell-line)
    (defun
	(spell-region
	    end
	    word
	    saved-mode-line
	    saved-syntax-table
	    old-in-spell-line
	    (setq old-in-spell-line ~spell-in-spell-line)
	    (setq ~spell-in-spell-line 1)
	    (mark)
	    (if (interactive)
		(progn
		    (message "Spell checking region...")
		    (sit-for 0)
		)
	    )
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(if (< (+ (mark) 0) (+ (dot) 0))
		    (exchange-dot-and-mark)
		)
		(setq end (+ (mark) 0))
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(forward-word)
		(while (& (! (eobp)) (<= (+ 0 (dot)) end))
		    (set-mark)
		    (backward-word)
		    (use-syntax-table saved-syntax-table)
		    (exchange-dot-and-mark)
		    (if (! (emacs$spell 7 (setq word (region-to-string))))
			(progn
			    (message "I do not understand `"
				word
			 	"'. Please correct it and press ^C to continue")
			    (setq saved-mode-line mode-line-format)
			    (setq mode-line-format
				(concat
				    " Correcting `"
				    word
				    "'. Press ^C to continue "))
			    (recursive-edit)
			    (setq mode-line-format saved-mode-line)
			)
		    )
		    (use-syntax-table "spell")
		    (forward-word)
		)
		(use-syntax-table saved-syntax-table)
	    )
	    (if (interactive)
		(message "Spell checking region... Done.")
	    )
	    (setq ~spell-in-spell-line old-in-spell-line)
	    (novalue)
	)
	(spell-buffer
	    (if (interactive)
		(progn
		    (message "Spell checking buffer...")
		    (sit-for 0)
		)
	    )
	    (save-window-excursion
		(beginning-of-file)
		(set-mark)
		(end-of-file)
		(spell-region)
	    )
	    (if (interactive)
		(message "Spell checking buffer... Done.")
	    )
	    (novalue)
	)
	(spell-line
	    end
	    word
	    saved-mode-line
	    saved-syntax-table
	    type-ahead
	    type-ahead-done
	    old-in-spell-line
	    (setq old-in-spell-line ~spell-in-spell-line)
	    (setq ~spell-in-spell-line 1)
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq type-ahead-done 0)
	    (setq type-ahead "")
	    (save-window-excursion
		(end-of-line)
		(setq end (+ 0 (dot)))
		(beginning-of-line)
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(forward-word)
		(while (& (! (eobp)) (<= (+ 0 (dot)) end))
		    (set-mark)
		    (backward-word)
		    (use-syntax-table saved-syntax-table)
		    (exchange-dot-and-mark)
		    (if (! (emacs$spell 7 (setq word (region-to-string))))
			(progn
			    (setq prefix-argument spell-feeps)
			    (prefix-argument-loop (send-string-to-terminal ""))
			    (message "I do not understand `"
				word
			 	"'. Please correct it and press ^C to continue")
			    (setq saved-mode-line mode-line-format)
			    (setq mode-line-format
				(concat
				    " Correcting `"
				    word
				    "'. Press ^C to continue "))
			    (if (! type-ahead-done)
			        (progn
				    (while keyboard-input-available
	        		        (setq type-ahead (concat type-ahead (char-to-string (get-tty-character))))
				    )
				    (setq type-ahead-done 1)
				)
			    )
			    (recursive-edit)
			    (setq mode-line-format saved-mode-line)
			)
		    )
		    (use-syntax-table "spell")
		    (forward-word)
		)
		(use-syntax-table saved-syntax-table)
	    )
	    (if (& type-ahead-done (!= (length type-ahead) 0)) (push-back-character type-ahead))
	    (setq ~spell-in-spell-line old-in-spell-line)
	    (novalue)
	)
	(spell-word-in-buffer
	    word
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(if (emacs$spell 7 (setq word (region-to-string)))
		    (message "`" word "\' is spelt correctly")
		    (progn
		      (save-window-excursion
			(pop-to-buffer "Help")
			(erase-buffer)
			(setq word (emacs$spell 8 word))
			(insert-string word)
			(beginning-of-file)
			(delete-white-space)
			(error-occurred (re-replace-string "\\(\\w\\w*\\)\\W\\W*" "\\1\n"))
			(error-occurred
			    (while (! (eobp))
				(end-of-line)
				(to-col 25)
				(insert-character ' ')
				(delete-next-character)
				(end-of-line)
				(to-col 50)
				(insert-character ' ')
				(delete-next-character)
				(next-line)
			    )
			)
			(beginning-of-file)
			(insert-string "Choose one of the following:\n")
			(beginning-of-file)
			(setq word
			    (expand-from-string-table "Select the correction required: "
				word
			    )
			  )
		      )
			(if (length word)
			    (progn
				(erase-region)
				(insert-string word)
			    )
			)
		    )
		)
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-newline
	    (spell-line)
	    (newline)
	)
	(spell-newline-and-indent
	    (spell-line)
	    (newline-and-indent)
	)
	(make-text-mode-spell
	    (declare-global TM-spell-line-hook)
	    (setq TM-spell-line-hook "spell-line")
	)
	(spell-ignore-word-in-buffer
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(emacs$spell 5 (region-to-string))
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-add-word-in-buffer
	    word
	    saved-syntax-table
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (save-window-excursion
		(setq saved-syntax-table current-buffer-syntax-table)
		(use-syntax-table "spell")
		(error-occurred (forward-character))
		(backward-word)
		(set-mark)
		(forward-word)
		(use-syntax-table saved-syntax-table)
		(setq word (region-to-string))
		(emacs$spell 5 word)
		(emacs$spell 6 word)
	    )
	    (if (& ~spell-in-spell-line spell-line-exit-on-action) (exit-emacs))
	    (novalue)
	)
	(spell-ignore-word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (emacs$spell 5 (arg 1 ": spell-ignore-word "))
	    (novalue)
	)
	(spell-add-word
	    word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq word (arg 1 ": spell-add-word "))
	    (emacs$spell 5 word)
	    (emacs$spell 6 word)
	    (novalue)
	)
	(spell-load-personal-dictionary
	    word
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (if prefix-argument-provided
	        (setq spell-personal-dictionary (arg 1 ": spell-load-personal-dictionary "))
	    )
	    (emacs$spell 4 spell-personal-dictionary)
	)
	(spell-add-dictionary
	    ~sad-dict
	    (if (! ~spell-init)
		(progn
		    (emacs$spell 3 spell-master-dictionary spell-ram-dictionary spell-memory-size)
		    (error-occurred (emacs$spell 4 spell-personal-dictionary))
		    (setq ~spell-init 1)
		)
	    )
	    (setq ~sad-dict (arg 1 ": spell-add-dictionary "))
	    (emacs$spell 4 ~sad-dict 0)
	)
    )
    (declare-global spell-feeps)
    (setq spell-feeps 1)
    (save-window-excursion
	(switch-to-buffer "Hack buffer")
	(use-syntax-table "spell")
	(modify-syntax-table "dull" "~")
	(modify-syntax-table "word" "-a-zA-Z'.0-9`\"")
    )
    (delete-buffer "Hack buffer")
    (error-occurred (execute-mlisp-file "spell.key"))
    (novalue)
)
[syidef.ml]
; 
; load up $syidef symbols
; 
(if
    (if (is-bound $syidef-loaded)
	(! $syidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "syi_def")
	(declare-global $syidef-loaded)
	(setq $syidef-loaded 1)
    )
)
[syi_def.ml]
; 
; $syidef
; 
(progn
    (~sys_literal_setup_incr "syi$_" 4096 1
	    "version"
	    "sid"
	    "pfcdefault"
	    "pagtblpfc"
	    "syspfc"
	    "kfilstcnt"
	    "gblsections"
	    "gblpages"
	    "gblpagfil"
	    "maxprocesscnt"
	    "pixscan"
	    "procsectcnt"
	    "minwscnt"
	    "pagfilcnt"
	    "swpfilcnt"
	    "sysmwcnt"
	    "intstkpages"
	    "dlckextrastk"
	    "balsetcnt"
	    "irpcount"
	    "irpcountv"
	    "wsmax"
	    "npagedyn"
	    "npagevir"
	    "pagedyn"
	    "virtualpagecnt"
	    "sptreq"
	    "exusrstk"
	    "lrpcount"
	    "lrpcountv"
	    "lrpsize"
	    "lrpmin"
	    "srpcount"
	    "srpcountv"
	    "srpsize"
	    "srpmin"
	    "channelcnt"
	    "piopages"
	    "ctlpages"
	    "ctlimglim"
	    "imgiocnt"
	    "quantum"
	    "mpw_wrtcluster"
	    "mpw_hilimit"
	    "mpw_lolimit"
	    "mpw_prio"
	    "swp_prio"
	    "mpw_thresh"
	    "mpw_waitlimit"
	    "tbskipwsl"
	    "physicalpages"
	    "pfratl"
	    "pfrath"
	    "pfrats"
	    "wsinc"
	    "wsdec"
	    "awsmin"
	    "awstime"
	    "swprate"
	    "swpoutpgcnt"
	    "swpallocinc"
	    "iota"
	    "longwait"
	    "swpfail"
	    "vmsd1"
	    "vmsd2"
	    "vmsd3"
	    "vmsd4"
	    "vms5"
	    "vms6"
	    "vms7"
	    "vms8"
	    "userd1"
	    "userd2"
	    "user3"
	    "user4"
	    "extracpu"
	    "maxsysgroup"
	    "mvtimeout"
	    "maxbuf"
	    "defmbxbufquo"
	    "defmbxmxmsg"
	    "defmbxnummsg"
	    "freelim"
	    "freegoal"
	    "growlim"
	    "borrowlim"
	    "lockretry"
	    "xfmaxrate"
	    "lamapregs"
	    "realtime_spts"
	    "clisymtbl"
	    "lockidtbl"
	    "reshashtbl"
	    "deadlock_wait"
	    "scsbuffcnt"
	    "scsconncnt"
	    "scsrespcnt"
	    "scsmaxdg"
	    "scsmaxmsg"
	    "scsflowcush"
	    "scssystemid"
	    "scssystemidh"
	    "scsnode"
	    ""
	    "prcpolinterval"
	    "pastimout"
	    "pastdgbuf"
	    "panumpoll"
	    "papollinterval"
	    "papoolinterval"
	    "timepromptwait"
	    "udaburstrate"
	    "lnmshashtbl"
	    "lnmphashtbl"
	    "tty_scandelta"
	    "tty_dialtype"
	    "tty_speed"
	    "tty_rspeed"
	    "tty_parity"
	    "tty_buf"
	    "tty_defchar"
	    "tty_defchar2"
	    "tty_typahdsz"
	    "tty_altypahd"
	    "tty_altalarm"
	    "tty_dmasize"
	    "tty_prot"
	    "tty_owner"
	    "tty_classname"
	    "tty_silotime"
	    "tty_defport"
	    "rms_dfmbc"
	    "rms_dfmbfsdk"
	    "rms_dfmbfsmt"
	    "rms_dfmbfsur"
	    "rms_dfmbfrel"
	    "rms_dfmbfidx"
	    "rms_dfmbfhsh"
	    "rms_prologue"
	    "rms_extend_size"
	    "rms_fileprot"
	    "pql_dastlm"
	    "pql_mastlm"
	    "pql_dbiolm"
	    "pql_mbiolm"
	    "pql_dbytlm"
	    "pql_mbytlm"
	    "pql_dcpulm"
	    "pql_mcpulm"
	    "pql_ddiolm"
	    "pql_mdiolm"
	    "pql_dfillm"
	    "pql_mfillm"
	    "pql_dpgflquota"
	    "pql_mpgflquota"
	    "pql_dprclm"
	    "pql_mprclm"
	    "pql_dtqelm"
	    "pql_mtqelm"
	    "pql_dwsdefault"
	    "pql_mwsdefault"
	    "pql_dwsquota"
	    "pql_mwsquota"
	    "pql_dwsextent"
	    "pql_mwsextent"
	    "pql_denqlm"
	    "pql_menqlm"
	    "acp_mapcache"
	    "acp_hdrcache"
	    "acp_dircache"
	    "acp_workset"
	    "acp_fidcache"
	    "acp_extcache"
	    "acp_extlimit"
	    "acp_quocache"
	    "acp_sysacc"
	    "acp_maxread"
	    "acp_window"
	    "acp_writeback"
	    "acp_datacheck"
	    "acp_baseprio"
	    "acp_swapflgs"
	    "defpri"
	    "ijoblim"
	    "bjoblim"
	    "njoblim"
	    "rjoblim"
	    "quorum"
	    "votes"
	    "recnxinterval"
	    "boottime"
	    "lockidtbl_max"
	    "tailored"
	    "startup_p1"
	    "startup_p2"
	    "startup_p3"
	    "startup_p4"
	    "startup_p5"
	    "startup_p6"
	    "startup_p7"
	    "startup_p8"
	    "cluster_nodes"
	    "cluster_votes"
	    "cluster_quorum"
	    "cluster_fsysid"
	    "cluster_ftime"
	    "cluster_member"
	    "node_csid"
	    "node_votes"
	    "node_quorum"
	    "node_systemid"
	    "node_swincarn"
	    "node_swtype"
	    "node_swvers"
	    "node_hwtype"
	    "node_hwvers"
	    "nodename"
	    "archflag"
	    "scs_exists"
	    "disk_quorum"
	    ""
	    ""
	    ""
	    "pamaxport"
	    "pasanity"
	    "defquepri"
	    "maxquepri"
	    "qdskinterval"
	    "alloclass"
	    "lgi_retry_lim"
	    "lgi_retry_tmo"
	    "lgi_brk_lim"
	    "lgi_brk_tmo"
	    "lgi_hid_tim"
	    "lgi_pwd_tmo"
	    "pql_djtquota"
	    "pql_mjtquota"
	    "vaxcluster"
	    "lockdirwt"
	    "qdskvotes"
	    "dormantwait"
	    "pagefile_page"
	    "swapfile_page"
	    "pagefile_free"
	    "swapfile_free"
	    "tty_timeout"
	    "tty_autochar"
	    "panopoll"
	    "pe1"
	    "pe2"
	    "pe3"
	    "pe4"
	    "pe5"
	    "pe6"
	    "rms_gblbufquo"
	    "rms_dfnbc"
	    "acp_dindxcache"
	    "maxattachpri"
    )
    (~sys_literal_setup_incr "syi$_" 8192 1
	    "cpu"
	    "bugreboot"
	    "crdenable"
	    "dumpbug"
	    "bugcheckfatal"
	    "acp_multiple"
	    "noautoconfig"
	    "noclock"
	    "nocluster"
	    "poolpaging"
	    "sbierrenable"
	    "settime"
	    "acp_share"
	    "syspaging"
	    "uafalternate"
	    "writablesys"
	    "resalloc"
	    "ssinhibit"
	    "conceal_devices"
	    "Savedump"
	    "mountmsg"
	    "dismoumsg"
	    "loaderapt"
	    "loadchkprt"
	    ""
	    "cjfload"
	    "cjfsysruj"
	    "node_area"
	    "node_number"
	    "class_prot"
	    "character_emulated"
	    "decimal_emulated"
	    "d_float_emulated"
	    "f_float_emulated"
	    "g_float_emulated"
	    "h_float_emulated"
	    "loadmtaccess"
	    "acp_xqp_res"
	    "writesysparams"
	    "lgi_brk_term"
	    "lgi_brk_disuser"
	    "acp_rebldsysd"
	    "ws_opa0"
    )
)
[tabs.ml]
;
; tabs.ml
; 	a package to do smart things with the tab key
; 
; 	It uses the variable logical-tab-size to compute the
; 	text to insert or delete.
; 
; tab-key
;	this function inserts tabs and space to move to the next tab stop
; tab-delete
;	this function deletes enough tabs and spaces to move back a tab stop
; 
(progn
(defun
    (tab-key
	~current-column
	
	(setq ~current-column (current-column))
	(insert-character '.') (backward-character)
	(delete-white-space) (delete-next-character)
	(to-col
	    (+
		(*
		    (/
			(+ ~current-column logical-tab-size -1)
			logical-tab-size
		    )
		    logical-tab-size
		)
		1
	    )
	)
    )
)
(defun
    (tab-delete
	~current-column
	
	(setq ~current-column (current-column))
	(insert-character '.') (backward-character)
	(delete-white-space) (delete-next-character)
	(to-col
	    (+
		(*
		    (/
			(- ~current-column 2)
			logical-tab-size
		    )
		    logical-tab-size
		)
		1
	    )
	)
    )
)
)
	    
[tags.key]
(progn
    (bind-to-key "add-tag" "\^x\^a")
    (bind-to-key "goto-tag" "\^x\^l")
    (bind-to-key "visit-function" "\^x\^p")
)
[tags.ml]
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
		    (re-search-forward "\\([^ \t][^ \t]*\\)")
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
	    (insert-string ~TAGS-file)
	    (newline)
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
	(if (& (= (buffer-size) 0) (= (current-file-name) ""))
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
	    (search-reverse ""))
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
    (add-typed-tags ~TAGS-ext ~TAGS-pattern
	(setq ~TAGS-ext (current-file-name))
	(save-excursion
	    (temp-use-buffer "*TAGP*")
	    (erase-buffer)
	    (insert-string ~TAGS-ext)
	    (beginning-of-file)
	    (end-of-line)
	    (search-reverse ".")
	    (forward-character)
	    (set-mark)
	    (end-of-line)
	    (error-occurred (search-reverse ";"))
	    (setq ~TAGS-ext (region-to-string))
	)
	(progn
	    ~TAGS-arg
	    ~TAGS-proccesor
	(setq ~TAGS-arg
	    (if
		(= ~TAGS-ext (concat tag-language-type "")) tag-language-pattern
		(= ~TAGS-ext "c") ""
		;"^\\S\\C[A-Za-z_$*][A-Za-z0-9_$* \t\n]*([A-Za-z0-9_$*, \t\n()]*[ \t\n]*{"
		(= ~TAGS-ext "h") ""
		(= ~TAGS-ext "cpp") ""
		(= ~TAGS-ext "cxx") ""
		(= ~TAGS-ext "ml") "^(defun[ \t\n]*[(]*"
		(= ~TAGS-ext "bli") "^[ \t]*routine.*\\|^[ \t]*global routine.*"
		(= ~TAGS-ext "b16") "^[ \t]*routine.*\\|^[ \t]*global routine.*"
		(= ~TAGS-ext "b32") "^[ \t]*routine.*\\|^[ \t]*global routine.*"
		(= ~TAGS-ext "mar") "^.*\\.entry\\|^[a-zA-Z].*:"
		(= ~TAGS-ext "rno") "^\\.hl\\|^\\.ch\\|^\\.ax"
		(= ~TAGS-ext "for") "^[ 	][ 	]*subroutine\\|^[ 	][ 	]*[a-zA-Z]*[ 	][ 	]*function"
		(error-message
		    (concat
			"Cannot tags files with a type of ."
			~TAGS-ext
		    )
		)
	    )
	)
	(setq ~TAGS-proccesor
	    (if
		(= ~TAGS-ext "h")
		"~TAGS-c-pattern-processor"
		(= ~TAGS-ext "c")
		"~TAGS-c-pattern-processor"
		(= ~TAGS-ext "cpp")
		"~TAGS-c-pattern-processor"
		(= ~TAGS-ext "cxx")
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
[teach.mlp]
(progn
    ~teach-mode-line-format
    (switch-to-buffer "main")
    (setq ~teach-mode-line-format mode-line-format)
    (if (file-exists "sys$help:emacs.cbi")
	(progn (visit-file "sys$help:emacs.cbi"))
	(file-exists "emacs$library:emacs.cbi")
	(progn (visit-file "emacs$library:emacs.cbi"))
	(progn
	    (switch-to-buffer "Teach error")
	    (insert-string "No help available today")
	)
    )
    (delete-other-windows)
    (setq mode-line-format ~teach-mode-line-format)
    (setq current-buffer-type "scratch")
    (setq current-buffer-checkpointable 0)
    (argc)
    (beginning-of-file)
)
[teco.ml]
; TECO.ML	Emacs routines to execute TECO commands on a buffer
; * Requires that the DCL command MUNG be defined as: *
;$	mung :==  $sys$system:teco mung
;
;	Usage is (teco <teco command>), user is prompted if not provided.
;	The command string may contain "~" instead of <esc>.  (Good for
;	PRO keyboards.)  There is no (yet) to enter "~".
;
;	Current position is left where TECO put it.
;
(defun (teco-command tecostring
  (setq tecostring (arg 1 "Enter TECO command:"))
  (save-excursion
    (temp-use-buffer "Teco")
    (erase-buffer)
    (insert-string
      (concat "ERSYS$INPUT:EWSYS$OUTPUT:P" tecostring "i
emacsteco dot
EX"))
    (beginning-of-file)
    (error-occurred (replace-string "~" ""))
    (write-named-file "sys$login:emacsteco.tmp")
  )
  (save-excursion
    (set-mark)
    (end-of-file)
    (filter-region "MUNG sys$login:emacsteco.tmp")
  )
  (search-forward "
emacsteco dot
")
  (provide-prefix-argument 15 (delete-previous-character))
))
[template_driver.ml]
(progn
(declare-global
    ~template-array
    ~template-depth
)
(setq ~template-array (array 1 11 0 50 0 3))
(setq ~template-depth 0)
(defun template-exit ()
    (save-window-excursion
	(goto-character (- template-end-field 1))
	(set-mark)
	(goto-character (+ (fetch-array ~template-array ~template-depth 0 0)))
	(erase-region)
    )
    (error-occurred (setq ~template-exit 1))
)
(defun template-driver (~template)
    ~template-indent
    ~template-count
    ~template-i
    ~template-exit
    template-end-field
    template-start-field
    template-exit-dot
    (setq ~template-depth (+ ~template-depth 1))
    (setq template-exit-dot 0)
    (setq ~template-exit 0)
    (setq ~template-count (~template-insert ~template (current-indent)))
    (setq ~template-i 1)
    (while (& (! ~template-exit) (<= ~template-i ~template-count))
        (goto-character (+ (fetch-array ~template-array ~template-depth ~template-i 2)))
	(save-window-excursion
	    (set-mark)
	    (goto-character (+ (fetch-array ~template-array ~template-depth ~template-i 3)))
	    (erase-region)
	)
	(setq template-start-field (dot))
	(save-window-excursion 
	    (if (error-occurred (forward-character))
	        (insert-character '\n')
	    )
	    (setq template-end-field (dot))
	)
	(execute-mlisp-line (fetch-array ~template-array ~template-depth ~template-i 1))
	(setq ~template-i (+ 1 ~template-i))
    )
    (if template-exit-dot
        (goto-character template-exit-dot)
    )
    (setq ~template-depth (- ~template-depth 1))
    (novalue)
)
(defun ~template-insert (~template-buffer ~template-indent 0)
    ~template-index
    ~template-count
    (save-window-excursion
	(set-mark)
	(yank-buffer ~template-buffer)
	(setq-array ~template-array ~template-depth 0 0 (dot))
	(narrow-region)
	(if ~template-indent
	    (progn
		(beginning-of-file)
		(while (! (error-occurred (re-search-forward "\n")))
		    (to-col ~template-indent)
		)
	    )
	)
	(beginning-of-file)
	(setq ~template-count 0)
	(while (! (error-occurred (re-search-forward "§\\([^¶]*\\)¶\\([^¥]*\\)¥\\([^¨]*\\)¨")))
	    (region-around-match 1)
	    (setq ~template-index (+ (region-to-string)))
	    (region-around-match 2)
	    (setq-array ~template-array ~template-depth ~template-index 0 (region-to-string))
	    (region-around-match 3)
	    (setq-array ~template-array ~template-depth ~template-index 1 (region-to-string))
	    (region-around-match 0)
	    (erase-region)
	    (setq-array ~template-array ~template-depth ~template-index 2 (dot))
	    (insert-string (fetch-array ~template-array ~template-depth ~template-index 0))
	    (setq-array ~template-array ~template-depth ~template-index 3 (dot))
	    (setq ~template-count (+ ~template-count 1))
	)
	(widen-region)
    )
    ~template-count
)
)
[textmode.key]
; text-mode local keymap definitions
(progn
    (local-bind-to-key "Text-mode-ESC-map" "\e")
    (local-bind-to-key "set-indent-offset" "\ei")
    (local-bind-to-key "fill-and-justify-paragraph" "\ej")
    (local-bind-to-key "fill-and-justify-region" "\eJ")
    (local-bind-to-key "set-left-margin" "\e1")
    (local-bind-to-key "set-right-margin" "\e\^h")
    (local-bind-to-key "justify-mode" "\em")
    (local-bind-to-key "~TM-center-line" "\ec")
    (if (is-bound TM-spell-line-hook)
	(if (error-occurred
	        (local-bind-to-key "spell-newline" "\r")
	        (local-bind-to-key "spell-newline-and-indent" "\n")
	    )
	    (error-occurred
	        (autoload "spell-newline" "spell")
	        (autoload "spell-newline-and-indent" "spell")
	        (local-bind-to-key "spell-newline" "\r")
	        (local-bind-to-key "spell-newline-and-indent" "\n")
	    )
	)
    )
)
[textmode.ml]
(progn
;
; 	updated:
;			 3-Sep-1990	rewrote to restore cursor location
;	Gim Hom		20-May-1990 	rewrote package to handle journaling
;					by using a temporary buffer.
;	Gim Hom		15-Nov-1989	rename ~TM functions
;	Gim Hom		26-Jul-1989	Fixed para bug (one cr at bobp)
;	Gim Hom		19-Jan-1988	Fixed minor indent and para bug
;	Gim Hom		13-Nov-1986	Work with international characters.
;	        	 		Made word syntax independent; put
;					two spaces after a period;  fixed
;					bug in set-margins
;
;	Gim Hom		24-FEB-1985	Add autowrap (auto-fill) toggle;
;					added indent-offset +/-; margin-
;					checking; auto-margin seeking;
;					corrected justify paragraph
;
; 	Nick Emery	30-Mar-1984	Added spell check hooks
;
;	John Harper	3-Nov-83	Allow user-specific initialisation
;
; 	John L. Adelus	8-Jul-1983	Removed space processor routine.
;					Wrote ~TM-auto-fill auto-fill-hook
;					Fixed justify-line to work with
;					auto-fill-hook.
;					fill-and-justify-paragraph rewritten
;					using regions.
; 
; 	John L. Adelus 24-Jul-1983	Rewrote fill-and-justify-paragraph
;					Added fill-and-justify-region
; 
;	Barry A. Scott	27-Jul-1983	Added Text-mode-map so that the
;					keymap is built once.
; 
(defun
    (text-mode
	(use-local-map "Text-mode-map")
	(use-abbrev-table "Text-mode")
	(use-syntax-table "Text-mode")
	(setq buffer-justify-flag 1)
	(setq auto-fill-hook "test-auto-fill")
	(setq autowrap-mode-flag 1)
	(setq mode-line-format default-mode-line-format)
	(setq paragraph-delimiters "\^l\n\\|\n[\t ]*\n")
	(setq indent-offset 0)
	(setq left-margin 1)
	(setq right-margin 76)
	(setq mode-string "Text")
	(error-occurred
	    (TM-user-buffer-initialization)
	)
	(~TM-mode)
	(novalue)
    )
    ;
    ; set-margins will automatically set the left and right margins as
    ; as the indent-offset.  The right margin is the  defined as the right
    ; most column in the paragraph for non-justified paragraphs.
    ;
    (set-margins TM-IO end-of-para begin-para-mark  
	(save-excursion
    	    (tm-forward-paragraph)
	    (tm-backward-paragraph)
	    (if (looking-at "[ \t]*\n")
		(progn
		    (tm-forward-word)
		    (beginning-of-line)
		)
	    )
	    (setq begin-para-mark (dot))	; mark the start of para
	    (setq TM-IO 1)
	    (if (looking-at "[\t ]")
		(progn
		    (re-search-forward "[\t ][\t ]*")
		    (setq TM-IO (current-column))
		)
	    )		; found the indent column
	    (tm-forward-paragraph)
	    (backward-character)
	    (while (! (looking-at "\\w"))
		(backward-character)
	    )
	    (setq end-of-para (dot))	; mark the end of the paragraph
	    (beginning-of-line)
	    (if (looking-at "[\t ]")
		(re-search-forward "[\t ][\t ]*")
	    )
	    (setq left-margin  (current-column))
	    (goto-character begin-para-mark)
	    (end-of-line) (setq right-margin (current-column))
	    (while (<= (dot) end-of-para)	; find right most margin
		(next-line) (end-of-line)
		(if (> (current-column) right-margin)
		    (setq right-margin (current-column))
		)
	    )
	    (setq right-margin (- right-margin 1))
	    (setq indent-offset (- TM-IO left-margin))
	    (~TM-mode)
	    
	)
    )
    
    (remove-tabs-in-line  col 	
	(save-restriction
	    (beginning-of-line) (set-mark) (end-of-line) (narrow-region)
	    (beginning-of-line)
	    (while (! (error-occurred (search-forward "\t")))
		(progn (setq col (current-column))
		    (delete-previous-character)
		    (while (< (current-column) col)
			(insert-string " "))
		)
	    )
	    (unset-mark)
	    (widen-region)
	)
    )
    
    (new-compact-white-space first	
	(if (& begin-paragraph  (< indent-offset 0)) ; execute for first line
	    (progn				     ;   in the pargraph only.
		(save-restriction
		    (remove-tabs-in-line)
		    (beginning-of-line)
		    (delete-white-space)
		    (set-mark) (end-of-line) (narrow-region)
		    (beginning-of-line)
		    (setq first 1)
		    (while (! (error-occurred (re-search-forward "[ \t]")))
			(if (! first) 
			    (progn 
				(delete-white-space)
				(if (= (preceding-char) '.') 
				    (insert-string " ")
				) 
;				(if (provide-prefix-argument -1
;					(looking-at extra-space-chars)
;				    )
;				    (insert-string " ")
;				) 
				(insert-string " ")
			    )
			)
			(tm-forward-word)
			(setq first 0)
		    )
		    (unset-mark)
		    (widen-region)
		)
		(end-of-line)
		(delete-white-space)
	    )
	    (compact-white-space-in-line)
	)
    )
    (test-for-begin-paragraph	; sets begin-paragraph =1 if at begin-para.
	(setq begin-paragraph 0)
	(save-excursion
	    (beginning-of-line)
	    (if  (bobp) (setq begin-paragraph 1))
	    (previous-line)
	    (if  (looking-at "[\t ]*\n") 
		(setq begin-paragraph 1))
	)
    )

    (toggle-justify-mode
	(if (= auto-fill-hook "") (setq auto-fill-hook "test-auto-fill"))
	(if buffer-justify-flag
	    (setq buffer-justify-flag 0)
	    (setq buffer-justify-flag 1)
	)
	(~TM-mode)
	(novalue)
    )
    
    (toggle-autowrap-mode
	(if (= auto-fill-hook "") (setq auto-fill-hook "test-auto-fill"))
	(if autowrap-mode-flag 
	    (progn
		(setq autowrap-mode-flag 0)
;		(setq auto-fill-hook "novalue" ) 
		(message "AUTOWRAP is turned off.")
	    )
	    (progn
		(setq autowrap-mode-flag 1)
;		(setq auto-fill-hook "test-auto-fill")	
		(message "AUTOWRAP is ENABLED.")
	    )
	)
    )

    (test-auto-fill
	(if autowrap-mode-flag
	    (~TM-auto-fill)
	    (novalue)
	)
    )

    (~TM-center-line width
	(save-excursion
	    (beginning-of-line)
	    (delete-white-space)
	    (end-of-line)
	    (delete-white-space)
	    (setq width (current-column))
	    (beginning-of-line)
	    (to-col
		(+ left-margin (/ (- (- right-margin left-margin) width) 2)
		)
	    )
	)
    )
    (set-indent-offset
	io
	(if (! prefix-argument-provided)
	    (setq io (+ (arg 1 "Set indent offset to ") 0))
	)
	(if (>= (+ left-margin io) right-margin)
	    (error-message "Indent offset greater than line width! ")
	)
	(if (< (+ left-margin io ) 0)
	    (error-message "Left margin + indent offset less than zero! ")
	)
	(setq indent-offset io)
	(~TM-mode)
    )
    (set-left-margin
	lm
	(if (! prefix-argument-provided)
	    (setq lm (arg 1 "Set left margin to "))
	)
	(if (= lm "") (setq lm (current-column)))
	(setq lm (+ lm))
	(if (>= lm right-margin)
	    (error-message "Left margin must be to the left of Right margin")
	)
	(if (>= lm (- right-margin 10))
	    (error-message "Left margin too close to Right margin")
	)
	(setq left-margin lm)
	(~TM-mode)
    )
    (set-right-margin
	rm
	(if (! prefix-argument-provided)
	    (setq rm (arg 1 "Set right margin to "))
	)
	(if (= rm "") (setq rm (current-column)))
	(setq rm (+ rm))
	(if (<= rm left-margin)
	    (error-message "Right margin must be to the right of Left margin")
	)
	(if (<= rm (+ left-margin 5))
	    (error-message "Right margin too close to Left margin")
	)
	(setq right-margin rm)
	(~TM-mode)
    )

    (~TM-mode %this-mode-name %this-mode-string
	(setq %this-mode-string mode-string)
	(save-excursion
	    (temp-use-buffer "temp-work-buffer")
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (insert-string %this-mode-string)
	    (beginning-of-line)
	    (set-mark)
	    (if (error-occurred (re-search-forward "[ -,]"))
		(end-of-file)
	        (backward-character)
	    )
	    (setq %this-mode-name (region-to-string))
	    (delete-buffer "temp-work-buffer")
	)
;	(message %this-mode-name)
	(setq mode-string 
	    (concat
		%this-mode-name
		(if buffer-display-margins
		    (concat 
			" L "left-margin
			(if buffer-justify-flag ", J " ", R ") right-margin
			(if (!= indent-offset 0) ", I " "")
			(if (!= indent-offset 0) indent-offset "")
		    )
		    ""
		)
	    )
	)
    )


    (justify-mode
	(setq buffer-justify-flag (! buffer-justify-flag))
	(~TM-mode)
	(novalue)
    )
    (compact-white-space-in-line
	(beginning-of-line)
	(delete-white-space)
	(while (! (eolp))
	    (progn
		(tm-forward-word)
		(delete-white-space)
		(if (= (preceding-char) '.') 
		    (insert-string " ")
		) 
;		(if (provide-prefix-argument -1 (looking-at extra-space-chars))
;		    (insert-string " ")
;		) 
		(insert-string " ")
	    )
	)
	(end-of-line)
	(delete-white-space)
    )
    (fill-line
	(if buffer-justify-flag
	    (while (save-excursion
		       (end-of-line)
		       (< (current-column) (+ 1 right-margin))
		   )
		(beginning-of-line)
		(tm-forward-word)
		(if (& (! (eolp)) begin-paragraph) 
		    (tm-forward-word)
		)
		(if (eolp)
		    (to-col (+ 1 right-margin))
		)
		(while (&
			   (! (eolp))
			   (save-excursion
			       (end-of-line)
			       (< (current-column) (+ 1 right-margin))
			   )
		       )
		    (insert-string " ")
		    (tm-forward-word)
		)
	    )
	)
    )
    (justify-line
	(beginning-of-line)
	(if begin-paragraph 
	    (progn (to-col (+ left-margin indent-offset)) )
	    (to-col left-margin)
	)
	(tm-forward-word)
	(if (> (current-column) right-margin)
	    (progn		; Word is longer than column width
		(beginning-of-line)
		(delete-white-space)
		(goto-character (+ 1 (dot) (- right-margin left-margin)))
		(save-excursion
		    (beginning-of-line)
		    (to-col left-margin)
		)
		(newline)
		(delete-white-space)
		(to-col left-margin)
		(beginning-of-line)
		(backward-character)
		1
	    )
	    (progn
		(while 
		    (& (! (eolp)) (<= (current-column) (+ 1 right-margin)))
		    (tm-forward-word)
		)
		(if (& (eolp) (= (current-column) (+ 1 right-margin)))
		    (progn
			(if (! ~TM-end-blank)
			    (progn
				(re-search-reverse "[ \t][ \t]*") ; backward-word
				(delete-white-space)
			    )
			)
			(newline)
			(delete-white-space)
			(to-col left-margin)
			(beginning-of-line)
			(backward-character)
			1
		    )
		    (if (> (current-column) (+ 1 right-margin))
			(progn
			    (re-search-reverse "[ \t][ \t]*") ; backward-word
			    (delete-white-space)
			    (newline)
			    (to-col left-margin)
			    (beginning-of-line)
			    (backward-character)
			    1
			)
			(if
			    (&
				~TM-end-blank
				(> (+ 1 (current-column)) right-margin)
			    )
			    (progn
				(newline)
				(to-col left-margin)
				(beginning-of-line)
				(backward-character)
				1
			    )
			    0
			)
		    )
		)
	    )
	)
    )
    (~TM-auto-fill
	split
;	(use-syntax-table "Text-mode")
	(end-of-line)
	(if (is-bound TM-spell-line-hook)
	    (save-window-excursion
		(backward-character)
		(if (looking-at "\\w")
		    (backward-word)
		)
		(newline-and-backup)
		(execute-mlisp-line
		    (concat
			"("
			TM-spell-line-hook
			")"
		    )
		)
		(end-of-line)
		(delete-next-character)
	    )
	)
	(setq ~TM-end-blank (= (preceding-char) ' '))
	(test-for-begin-paragraph) ; begin-paragrph = 1 if at begin of paraph
	(new-compact-white-space)
	(if (justify-line)
	    (progn
		(fill-line)
		(end-of-line)
		(next-line)
		(if
		    (&
			(! (= (current-column) left-margin))
			~TM-end-blank
		    )
		    (insert-string " ")
		)
	    )
	    (if ~TM-end-blank (insert-string " "))
	)
	(end-of-line)
	(setq ~TM-end-blank 0)
	(novalue)
    )

    (fill-and-justify-paragraph %tm-dot
	(message "Justifying...") (sit-for 0)
	(setq %tm-dot (+ (dot)))	; store current location
	(save-excursion
	    (if (error-occurred (re-search-reverse "\n[\n\^k\^l \t]*\n"))
		(progn
		    (beginning-of-file)
		    (if (looking-at "")  (re-search-forward ""))
		)
		(re-search-forward "")
	    )
            (if (looking-at "[\^k\^l \t]*\n") (re-search-forward ""))
	    (set-mark)

	    ; store relative location relative to start of paragraph
	    (setq %tm-dot (- %tm-dot (dot)))	

	    (tm-forward-paragraph)
	    (backward-character)
	    (while (looking-at "[\t ]*\n")
;	    (while (! (looking-at "\\w"))
		(backward-character)
	    )
	    (forward-character)
	    (if prefix-argument-provided
		(error-occurred (execute-mlisp-line "(spell-region)"))
	    )
	    (~TM-justify-region)
	    (message "Justifying... Done.")
	)
        ; go to original character. If space or tab, the original
	; dot was probably a space and was deleted.
	(goto-character (+ (dot) %tm-dot -1))
	(if (looking-at "[ \t]") (re-search-forward "[ \t]*"))
    )

    (fill-and-justify-region
	~TM-end-of-region count
	(if (= (dot) (mark))
	    (error-message "No region selected")
	)
	(if prefix-argument-provided
	    (error-occurred (execute-mlisp-line "(spell-region)"))
	)
	(message "Justifying region...") (sit-for 0)
	(save-excursion
	    (save-restriction
		(narrow-region)
		(if (> (dot) (mark)) (exchange-dot-and-mark))
		(tm-backward-paragraph)
		(setq ~TM-end-of-region 0)
		(while (! ~TM-end-of-region)
		    (set-mark)
		    (tm-forward-paragraph)
		    (if (eobp) (setq ~TM-end-of-region 1))
		    (error-occurred (backward-character))
		    (while (! (looking-at "\\w"))
			(progn
			    (if (error-occurred (backward-character))
				(error-message "Region does not contain a paragraph")
			    )
			)
		    )
		    (forward-character)
		    (~TM-justify-region)
		    (tm-forward-paragraph)
		)
		(message "Justifying region... Done.")
	    )
	)
    )

    (~TM-justify-region
	~TM-right-column ~TM-end-of-paragraph ~TM-last-line-length
	old-rm old-lm old-io old-justify
	(setq old-rm right-margin)
	(setq old-lm left-margin)
	(setq old-io indent-offset)
	(setq old-justify buffer-justify-flag)
	(copy-region-to-buffer "tm-work-buffer")
	(save-excursion
	    (temp-use-buffer "tm-work-buffer")
	    (setq current-buffer-journalled 0)
	    (setq current-buffer-checkpointable 0)
	    (setq right-margin old-rm)
	    (setq left-margin  old-lm)
	    (setq indent-offset old-io)
	    (setq buffer-justify-flag old-justify)
	    (setq ~TM-end-blank 1)
	    (setq ~TM-right-column (- right-margin left-margin))

	    ; goto original character. Used error-occured because
	    ; this variable in NOT used in fill-and-justify region
	    (error-occurred (goto-character (+  %tm-dot 1)))

	    (save-excursion
		(beginning-of-file)
		(set-mark)
		(end-of-file)
		(exchange-dot-and-mark)
		(setq ~TM-end-of-paragraph 0)
		(while (! ~TM-end-of-paragraph)
		    (test-for-begin-paragraph)
		    (new-compact-white-space)
		    (setq ~TM-last-line-length (current-column))
		    (while
			(&
			    (<= (current-column) ~TM-right-column)
			    (! ~TM-end-of-paragraph)
			)
			(delete-next-character)
			(insert-string " ")
			(new-compact-white-space)
			(if (= (current-column) ~TM-last-line-length)
			    (setq ~TM-end-of-paragraph 1)
			    (setq ~TM-last-line-length (current-column))
			)
		    )
		    (test-for-begin-paragraph)
		    (justify-line)
		    (if (! ~TM-end-of-paragraph)
			(progn
			    (fill-line)
			    (next-line)
			)
			(if
			    (& (eobp) (= (current-column) left-margin))
			    (progn
				(set-mark)
				(previous-line)
				(end-of-line)
				(erase-region)
			    )
			)
		    )
		)
	    )
	    ; now store the new dot of the original character
	    ; this variable in NOT used in fill-and-justify region
	    (error-occurred (setq %tm-dot (+ (dot))))
	)

	(erase-region)
	(yank-buffer "tm-work-buffer")
    )

)
; 
;   initialisation for the package
; 
    (use-syntax-table "Text-mode")
    (modify-syntax-table "word" "!-~")
;    (declare-global extra-space-chars)
;    (setq extra-space-chars "[.!?]")
    (declare-buffer-specific buffer-justify-flag
			     ~TM-end-blank 
			     begin-paragraph
			     autowrap-mode-flag 
			     indent-offset)
    (setq-default buffer-justify-flag 1)
    (setq-default buffer-display-margins 1)
    (setq auto-fill-hook "test-auto-fill")	
    (execute-mlisp-file "paragraph")
    (save-excursion
	(temp-use-buffer "keymap-hack")
	(define-keymap "Text-mode-map")
	(define-keymap "Text-mode-ESC-map")
	(use-local-map "Text-mode-map")
	(execute-mlisp-file "textmode.key")
	(kill-buffer "keymap-hack")
    )
    (error-occurred
        (TM-user-global-initialization)
    )


(defun
    (tm-forward-paragraph
	(beginning-of-line)
	(if (looking-at "[ \t\^l\n]*\n")
	    (if (error-occurred (re-search-forward "[ \t\^l\n]*\n"))
		(end-of-file)
	    )
	    (if (error-occurred (re-search-forward "\n[ \t\^l\n]*\n"))
		(end-of-file)
	    )
	)
    )
    (tm-backward-paragraph
	(if (bolp) (previous-line))
	(beginning-of-line)
	(if (looking-at "[ \t\^l]*\n")
	    ; looking at white space.  Find a non-white-space character
	    (if (error-occurred (re-search-reverse "[^\n\t\^l ]"))
		(beginning-of-file)
	    )
	)
	; find the beginning of white space
	(if (error-occurred (re-search-reverse "\n[\t\^l ]*\n"))
	    (beginning-of-file)
	)
	; goto the a non-white-space character
	(if (! (error-occurred (re-search-forward "[^\n\t\^l ]")))
	    (beginning-of-line)
	)
    )

    (tm-forward-word
	(if (error-occurred (re-search-forward "[^\t \n][^\t \n]*"))
;	(if (error-occurred (re-search-forward "[!-~][!-~]*"))
	    (end-of-file)
	)
    )
)    

)
[thesaurus.ml]
(progn
    (external-function "emacs$spell" "emacs$share:emacs_spell_shr")
    (if (! (is-bound ~et-initialized))
        (progn
	    (declare-global 
		~et-initialized
	    )
	    (setq ~et-initialized 0)
	)
    )
    (if (! (is-bound et-dictionary))
        (progn
	    (declare-global 
		et-dictionary
	    )
	    (setq et-dictionary "lngsplaet")
	)
    )
    (defun et-word-in-buffer ()
	~spell-word
	~spell-saved-syntax-table
	(save-window-excursion
	    (setq ~spell-saved-syntax-table current-buffer-syntax-table)
	    (use-syntax-table "spell")
	    (error-occurred (forward-character))
	    (backward-word)
	    (set-mark)
	    (forward-word)
	    (setq ~spell-word (region-to-string))
	    (use-syntax-table ~spell-saved-syntax-table)
	)
	(et-word ~spell-word)
    )
    (defun et-word (~et-spell-word (get-tty-string ": et-word "))
	~spell-length
	(if (! ~et-initialized) 
	    (progn
	        (emacs$spell 0 et-dictionary)
		(setq ~et-initialized 1)
	    )
	)
	(save-window-excursion
	    (pop-to-buffer "Electronic Thesaurus")
	    (setq mode-line-format " Electronic Thesaurus ")
	    (setq read-only-buffer 0)
	    (erase-buffer)
	    (emacs$spell 1 ~et-spell-word)
	    (beginning-of-file)
	    (setq ~spell-length (buffer-size))
	    (setq buffer-is-modified 0)
	    (setq current-buffer-checkpointable 0)
	    (setq current-buffer-journalled 0)
	    (setq read-only-buffer 1)
	)
	(if ~spell-length
	    (save-excursion (pop-to-buffer "Electronic Thesaurus"))
	    (error-message "\"" ~et-spell-word "\" is not in the Electronic Thesaurus")
	)
	(novalue)
    )
    (error-occurred (execute-mlisp-file "thesaurus.key"))
)
[time.ml]
(progn
    (declare-global time-interval &time-running)
    (if (= time-interval 0) (setq time-interval 1))
    (defun
	(%time %time-is
	    (setq %time-is (substr (current-time) 12 8))
	    (setq mode-string %time-is)
	    (schedule-procedure "%time" time-interval)
	)
	(time
	    (if &time-running
		(schedule-procedure "%time" 0)
		(schedule-procedure "%time" time-interval)
	    )
	    (setq &time-running (!= &time-running 0))
	    (setq &time-running (! &time-running))
	    (novalue)
	)
    )
)
[trace.ml]
(defun
    (trace
	x
	(setq x (decompile-current-line))
	(if (> (length x) 0)
	    (progn
		(message x)
		(sit-for trace-mode)
	    )
	)
    )
)
[uaidef.ml]
;
; load up $uaidef symbols
;
(if
    (if (is-bound $uaidef-loaded)
	(! $uaidef-loaded)
	1
    )
    (progn
	(execute-mlisp-file "uai_def")
	(declare-global $uaidef-loaded)
	(setq $uaidef-loaded 1)
    )
)
[uai_def.ml]
;
; $uaidef
; 
(progn
    (~sys_literal_setup_incr "uai$c_" 0 1
           "ad_ii"
           "purdy"
           "purdy_v"
    )
    (~sys_literal_setup_incr "uai$m_" 1 1
           "disctly"
           "defcli"
    )
    (~sys_literal_setup_incr "uai$m_" 4 1
           "lockpwd"
    )
    (~sys_literal_setup_incr "uai$m_" 8 1
           "captive"
    )
    (~sys_literal_setup_incr "uai$m_" 16 1
           "disacnt"   	       	       	
    )
    (~sys_literal_setup_incr "uai$m_" 32 1
           "diswelcom"
    )
    (~sys_literal_setup_incr "uai$m_" 64 1
           "dismail"
    )
    (~sys_literal_setup_incr "uai$m_" 128 1
           "nomail"
    )
    (~sys_literal_setup_incr "uai$m_" 256 1
           "genpwd"
    )
    (~sys_literal_setup_incr "uai$m_" 512 1
           "pwd_expired"
    )
    (~sys_literal_setup_incr "uai$m_" 1024 1
           "pwd2_expired"
    )
    (~sys_literal_setup_incr "uai$m_" 2048 1
           "audit"
    )
    (~sys_literal_setup_incr "uai$m_" 4096 1
           "disreport"
    )
    (~sys_literal_setup_incr "uai$m_" 8192 1
           "disreconnect"
    )
    (~sys_literal_setup_incr "uai$m_" 16384 1
           "autologin"
    )
    (~sys_literal_setup_incr "uai$v_" 0 1
	  "monday"
	  "tuesday"
	  "wednesday"
	  "thursday"
	  "friday"
	  "saturday"
     	  "sunday"
    )
    (~sys_literal_setup_incr "uai$v_" 0 1
	  "disctly"
	  "defcli"
	  "lockpwd"
	  "captive"
	  "disacnt"
	  "diswelcom"
	  "dismail"
	  "nomail"
	  "genpwd"
	  "pwd_expired"
	  "pwd2_expired"
	  "audit"
	  "disreport"
	  "disreconnect"
	  "autologin"
    )

    (~sys_literal_setup_incr "uai$_" 1 1
        "rtype"
        "version"
        "usrdatoff"
        "username"
        "username_tag"
        "uic"
        "mem"
        "grp"
        "sub_id"
        "parent_id"
        "account"
        "owner"
        "defdev"
        "defdir"
        "lgicmd"
        "defcli"
        "clitables"
        "pwd"
        "pwd2"
        "logfails"
        "salt"
        "encrypt"
        "encrypt2"
        "pwd_length"
        "expiration"
        "pwd_lifetime"
        "pwd_date"
        "pwd2_date"
        "lastlogin_i"
        "lastlogin_n"
        "priv"
        "def_priv"
        "min_class"
        "max_class"
        "flags"
        "network_access_p"
        "network_access_s"
        "batch_access_p"
        "batch_access_s"
        "local_access_p"
        "local_access_s"
        "dialup_access_p"
        "dialup_access_s"
        "remote_access_p"
        "remote_access_s"
        "primedays"
        "pri"
        "quepri"
        "maxjobs"
        "maxacctjobs"
        "maxdetach"
        "prccnt"
        "biolm"
        "diolm"
        "tqcnt"
        "astlm"
        "enqlm"
        "fillm"
        "shrfillm"
        "wsquota"
        "dfwscnt"
        "wsextent"
        "pgflquota"
        "cputim"
        "bytlm"
        "pbytlm"
        "jtquota"
        "proxy_lim"
        "proxies"
        "account_lim"
        "accounts"
    )

)                                                 
[undo.ml]
(defun (new-undo c
	   (undo)
	   (while (progn (message "Hit <space> to undo more")
			 (= (setq c (get-tty-character)) ' '))
		  (undo-boundary)
		  (undo-more))
	   (message "Finished undoing.")
	   (if (& (!= c '\n')
		  (!= c '\r'))
	       (push-back-character c))
       ))
[view.ml]
;;; Corrected some problems caused by upgrading to V3
;;; Made use of get-tty-file command in view-file
;;; Added exit if end of buffer is reached and space is entered again
;;; Added same thing for backing up through the file
;;;
;;; Bob Surtees
;;; 	+----------------+----------------------------------------------+
;;; 	| File Name:	 | 						|
;;; 	| Edit Version:	 | 1						|
;;; 	| Date Modified: | Tue Aug 23 12:29:43 1983			|
;;; 	| Updated By:	 | SURTEES					|
;;; 	| From Node:	 | _SIVA::					|
;;; 	+----------------+----------------------------------------------+
;;;
(progn
    ~VIEW-i
    (declare-global ~VIEW-flag ~VIEW-mode-line)
    (define-keymap "~VIEW-map")
    (use-global-map "~VIEW-map")
    (setq ~VIEW-i 0)
    (while (<= ~VIEW-i 127)
	(bind-to-key "exit-emacs" ~VIEW-i)
	(setq ~VIEW-i (+ ~VIEW-i 1))
    )
    (bind-to-key "~VIEW-next-page" " ")
    (bind-to-key "~VIEW-previous-page" "\^h")
    (use-global-map "default-global-keymap")
        
(defun
    (~VIEW-next-page
	(if (eobp)
	    (push-back-character ''))
	(next-page)
	(~VIEW-mode-line)
    )
)
(defun
    (~VIEW-previous-page
	(if (bobp)
	    (push-back-character ''))
	(previous-page)
	(~VIEW-mode-line)
    )
)
(defun
    (~VIEW-mode-line
	(setq mode-line-format
	    (concat
		"%[" ~VIEW-mode-line
		(if (save-excursion (end-of-file) (dot-is-visible))
		    ""
		    "     --more--"
		)
		"%]"
	    )
	)
    )
)
(defun
    (view-buffer
	~VIEW-buffer ~VIEW-bufnam ~VIEW-dot
	(setq ~VIEW-buffer (current-buffer-name))
	(if (! (length (setq ~VIEW-bufnam (arg 1 ": view-buffer "))))
	    (setq ~VIEW-bufnam (current-buffer-name))
	)
	(if (error-occurred (save-window-excursion (use-old-buffer ~VIEW-bufnam)))
	    (error-message
		(concat
		    "Buffer \""
		    ~VIEW-bufnam
		    "\" does not exist")
	    )
	)
	(switch-to-buffer "~VIEW-buffer")
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(erase-buffer)
	(yank-buffer ~VIEW-bufnam)
	(use-local-map "~VIEW-map")
	(beginning-of-file)
	(setq ~VIEW-mode-line
	    (concat 
		"VIEW buffer (%3p): "
		~VIEW-bufnam
	    )
	)
	(~VIEW-mode-line)
	(message "(Type SPACE for next page, BACKSPACE for previous page)")
	(recursive-edit)
	(if (!= (last-key-struck) 13)
	    (switch-to-buffer ~VIEW-buffer)
	    (progn
		(setq ~VIEW-dot (dot))
		(switch-to-buffer ~VIEW-bufnam)
		(goto-character ~VIEW-dot)
	    )
	)
	(if
	    (&
		(&
		    (&
			(!= (last-key-struck) 3)
			(!= (last-key-struck) 035)
		    )
		    (!= (last-key-struck) 13)
		)
		(!= (last-key-struck) 0177)
	    )
	    (push-back-character (last-key-struck))
	)
	(kill-buffer "~VIEW-buffer")
	(novalue)
    )
)
(defun
    view-file (~VIEW-filename (get-tty-file "File name: ")
		  ~VIEW-buffer (current-buffer-name)
		  ~VIEW-viewb (concat "VIEW " (current-time)))
    (switch-to-buffer ~VIEW-viewb)
    (if (error-occurred (insert-file ~VIEW-filename))
	(progn
	    (switch-to-buffer ~VIEW-buffer)
	    (kill-buffer ~VIEW-viewb)
	    (error-message
		(concat
		    "Can't access file \""
		    ~VIEW-filename
		    "\""
		)
	    )
	)
    )
    (use-global-map "~VIEW-map")
    (beginning-of-file)
    (setq ~VIEW-mode-line
	(concat 
	    "VIEW file (%3p): "
	    ~VIEW-filename
	)
    )
    (~VIEW-mode-line)
    (message "(Type SPACE for next page, BACKSPACE for previous page)")
    (recursive-edit)
    (use-global-map "default-global-keymap")
    (switch-to-buffer ~VIEW-buffer)
    (if
	(&
	    (&
		(&
		    (!= (last-key-struck) 3)
		    (!= (last-key-struck) 035)
		)
		(!= (last-key-struck) 13)
	    )
	    (!= (last-key-struck) 0177)
	)
	(push-back-character (last-key-struck))
    )
    (kill-buffer ~VIEW-viewb)
    (novalue)
)
)
[vms_mail.key]
(novalue)
[vms_mail.mlp]
;
;   vms_mail.mlp
; 
; This is the package that interfaces the callable_emacs feature of VMS MAIL
; to EMACS.
; 
; Inputs:
;   argv 1  - input file, may be blank
;   argv 2  - output file 
;
(progn
(declare-global
    vms-mail-exit-key		; exit key string
    vms-mail-quit-key		; quit key string
)
(defun
    (vms_mail-com
	vms-mail-quit
	exit-key
	quit-key
	
	(setq vms-mail-quit 0)
	
	(save-window-excursion mail-file
	    (if (error-occurred
		    (use-old-buffer "vms_mail_edit")
		)
		(progn
		    (switch-to-buffer "vms_mail_edit")
		    (error-occurred
			(execute-mlisp-line
			    (concat "(" vms-mail-hook ")")
			)
		    )
		    (execute-mlisp-file "vms_mail.key")
		)
	    )
	    (switch-to-buffer "vms_mail_edit")
	    (erase-buffer)
	    (error-occurred (read-file (argv 1)))
	    (delete-other-windows)
	    (recursive-edit)
	    (if (& (> (argc) 0) (= (argv 0) "vms_mail"))
		(progn ~send-message
		    (switch-to-buffer "vms_mail_edit")
		    (setq ~send-message
			(substr (get-tty-string "Send mail message (y,n)[y] ")
			    1 1))
		    (if (|
			    (= ~send-message "y")
			    (= ~send-message "Y")
			    (= (length ~send-message) 0)
			)
			(write-named-file (if (> (argc) 2) (argv 2) (argv 1)))
			(progn (message "VMS Mail edit quiting...") (sit-for 1))
		    )
		    (setq current-buffer-type "scratch")
		)
	    )
	)
	(if (& (> (argc) 0) (= (argv 0) "vms_mail"))
	    (pause-emacs)
	)
    )
)
(defun
    (~vms-mail-exit
	(setq vms-mail-quit 0)
	(exit-emacs)
    )
)
(defun
    (~vms-mail-quit
	(setq vms-mail-quit 1)
	(exit-emacs)
    )
)
)
[whatcurs.ml]
;function to show the cursor position in relation to the window
; 9-FEB-1985  cursor postion relative to top of page added by G Hom
;[***] el:whatcurs.=2, Edit by _VAX4::Prentice
;editno=2							-*-mlisp-*-
; Barry A. Scott	 2-Jul-1983
; convert to v3 emacs
(defun
    (what-cursor-position line dot char col beg end pline
	(save-excursion
	    (setq dot (dot))
	    (if (eobp)
		(setq char "eob")
		(setq char (following-char))
	    )
	    (end-of-window)
	    (setq end (dot))
	    (beginning-of-window)
	    (setq beg (dot))
	    (setq line 0)
	    (setq pline 0)
	    (while (& (<= (dot) dot) (! (eobp)))
		(next-line)
		(setq line (+ line 1)))
	    (goto-character dot)
	    (if (error-occurred (search-reverse "\014"))
		(beginning-of-file))
	    (while (& (<= (dot) dot) (! (eobp)))
		(next-line)
		(setq pline (+ pline 1)))
	)
	(if (eobp) (setq line (+ line 1)))
	(if (eobp) (setq pline (+ pline 1)))
	(message (concat "X=" (current-column)
		     " Y=" line
		     " CH=" char
		     " .=" (+ 0 dot)
		     "(" (if (> (buffer-size) 0)
			     (/ (* dot 100) (buffer-size))
			     0)
		     "% of " (buffer-size)
		     ")  W=<" (+ 0 beg) "," (+ 0 end) ">"
		     "  Line " pline " from top of page." ))
	(novalue)
    )
)
	
[whereis.ml]
;	+----------------+----------------------------------------------+
;	| File Name:	 | dba1:[surtees.emacs]whereis.ml		|
;	| Edit Version:	 | 4						|
;	| Date Modified: | Tue Apr  6 23:35:45 1982			|
;	| Updated By:	 | SURTEES					|
;	| From Node:	 | _R2ME2::					|
;	+----------------+----------------------------------------------+
;
; Display the key that a command is bound to.
;
(defun
    (where-is key command
	(if (! (interactive)) (error-message "where-is cannot be called from MLisp"))
	(setq command (get-tty-command ": where-is "))
	(save-window-excursion
	    (describe-bindings)
	    (if (error-occurred (search-forward command))
		(error-message
		    (concat "Command """ command """ is not bound to a key")))
	    (beginning-of-line)
	    (set-mark)
	    (search-forward " ")
	    (backward-character)
	    (setq key (region-to-string))
	    (re-search-forward "[ ]*")
	    (set-mark)
	    (end-of-line)
	    (message
		(concat "Command """ (region-to-string) """ is bound to "
		    key)))))
[windows.ml]
; 
; windows.ml
;   Copyright (c) 1985 Barry A. Scott
; 
; this package implements windows rings.
; 
(progn
(defun
    (add-windows-to-ring
	(setq windows-ring-position (+ 1 windows-ring-position))
	(setq windows-ring-used (+ 1 windows-ring-used))
	; 
	; do we need to extend the array?
	; 
	(if (>  windows-ring-used (fetch-array windows-ring 0))
	    (progn
		new-array size i
		(setq size (fetch-array windows-ring 0))
		(setq new-array (array 0 (+ size 10)))
		(setq i 0)
		(while (<= i size)
		    (setq-array new-array i (fetch-array windows-ring i))
		    (setq i (+ i 1))
		)
		(setq-array new-array 0 (+ size 10))
		(setq windows-ring new-array)
	    )
	)
	(if (= windows-ring-position windows-ring-used)
	    (progn
		; 
		; just add to the end of the array
		; 
		(setq-array windows-ring windows-ring-position current-windows)
	    )
	    (progn
		; 
		; need to slide later enties up array
		; 
		i
		(setq i windows-ring-used)
		(while (> i windows-ring-position)
		    (setq-array windows-ring i
			(fetch-array windows-ring (- i 1)))
		    (setq i (- i 1))
		)
		(setq-array windows-ring windows-ring-position current-windows)
	    )
	)
	(message "Current windows added to ring at position "
	    windows-ring-position)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (remove-windows-from-ring
	i
	(~windows-current-in-ring)
	(setq i windows-ring-position)
	(while (< i windows-ring-used)
	    (setq-array windows-ring i (fetch-array windows-ring (+ i 1)))
	    (setq i (+ i 1))
	)
	(setq windows-ring-used (- windows-ring-used 1))
	(message "Removed windows from ring at position "
	    windows-ring-position)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (current-windows-in-ring
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (~windows-current-in-ring
	(if (= 0 windows-ring-used)
	    (error-message "You have not saved any windows yet.")
	)
	(if (> windows-ring-position windows-ring-used)
	    (setq windows-ring-position 1)
	)
	(if (< windows-ring-position 1)
	    (setq windows-ring-position windows-ring-used)
	)
	(setq current-windows
	    (fetch-array windows-ring windows-ring-position)
	)
	(message "At position " windows-ring-position " in windows ring.")
	(novalue)
    )
)
(defun
    (goto-windows-in-ring
	(setq windows-ring-position (+ (arg 1 "Goto windows ring position: ")))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (next-windows-in-ring
	(setq windows-ring-position (+ windows-ring-position 1))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(defun
    (previous-windows-in-ring
	(setq windows-ring-position (- windows-ring-position 1))
	(~windows-current-in-ring)
	(if windows-cycle (windows-keymap))
    )
)
(define-keymap "windows-keymap")
(declare-global
    windows-ring			; the ring of windows
    windows-ring-position		; current position in the ring
    windows-ring-used			; number of windows in the ring
    windows-cycle			; true if next and previous cycle
					; through the windows
)
(setq windows-ring (array 0 10))	; array of 11 entries 
(setq-array windows-ring 0 10)		; entry zero is size of ring
(setq windows-ring-position 0)		; ring is empty
(setq windows-ring-used 0)		; amount of space used in the ring
    
(execute-mlisp-file "windows.key")
)
[writereg.ml]
(defun
    (write-region-to-file fname
	(setq fname
	    (if (interactive)
		(get-tty-file ": write-region-to-file ")
		(arg 1 ": write-region-to-file ")
	    )
	)
	(save-excursion
	    (copy-region-to-buffer "write-temp")
	    (temp-use-buffer "write-temp")
	    (write-named-file fname)
	    (kill-buffer "write-temp")
	)
	(novalue)
    )
)
