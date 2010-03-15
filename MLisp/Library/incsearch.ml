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
    inc-search-use-region
    ~inc-search-string
    ~inc-replace-string
    ~ins-direction
    ~ins-quote-key
    ~ins-stack-pointer
    ~ins-state-stack
    ~ins-is-failing
    ~ins-old-local-keymap
    ~ins-old-search-string
    ~ins-debug)

(define-keymap "no-local-map");	Hack because we can't undo local map
(setq inc-search-use-region 1)
(setq ~inc-search-string "")
(setq ~inc-replace-string "")
(setq ~ins-state-stack "")
(setq ~ins-debug 0)

(defun ~ins-debug( ~msg)
    (if ~ins-debug
	(save-window-excursion
	    (temp-use-buffer "inc-search-debug")
	    (end-of-file)
	    (insert-string ~msg "\n")
	)
    )
)

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

            (define-keymap "~incsearchmap-key-prefix")
            (define-keymap "~incsearchmap-mouse-prefix")
            (define-keymap "~incsearchmap-menu-prefix")

            (use-local-map "~incsearchmap-key-prefix")
            (local-bind-to-key "~ins-exit" "\[default]")

            (use-local-map "~incsearchmap-mouse-prefix")
            (local-bind-to-key "~ins-exit" "\[default]")

            (use-local-map "~incsearchmap-menu-prefix")
            (local-bind-to-key "~ins-exit" "\[default]")

	    (use-local-map "~incsquotemap")
            (local-bind-to-key "~ins-normal-char" "\[default]")

	    (use-local-map "~incsearchmap")
            (local-bind-to-key "~ins-normal-char" "\[default]")
	    (setq ~key 0)
	    ; C0 controls
	    (while (< ~key 32)
		(local-bind-to-key "~ins-exit" ~key)
		(setq ~key (+ 1 ~key))
	    )
            (local-bind-to-key "~ins-exit" 127)

	    ; tab is normal
	    (local-bind-to-key "~ins-normal-char" "\t")

	    ; DEL + C1 controls
	    (setq ~key 128)
	    (while (< ~key 160)
		(local-bind-to-key "~ins-exit" ~key)
		(setq ~key (+ 1 ~key))
	    )
            (local-bind-to-key "~incsearchmap-key-prefix" "\[key-prefix]")
            (local-bind-to-key "~incsearchmap-mouse-prefix" "\[mouse-prefix]")
            (local-bind-to-key "~incsearchmap-menu-prefix" "\[menu-prefix]")

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
			(= ~arg "quote")
			(progn
			    (local-bind-to-key "~ins-quote" ~key)
			    (setq ~ins-quote-key ~key)
			)
			
			(~ins-setup-bind-key)
		    )
		    
		)
	    )
	    (delete-buffer "~inc-search-key-hack")
	);	end of save-excursion
    )
);	end of setup-incsearch

(defun 
    (setup-more-incsearch ~key ~arg-num ~arg
	(save-excursion
	    (temp-use-buffer "~inc-search-key-hack")
	    (use-local-map "~incsearchmap")

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
		    (~ins-setup-bind-key)
		)
	    )
	    (delete-buffer "~inc-search-key-hack")
	)
    )
)

(defun
    (~ins-setup-bind-key
	(if
	    (= ~arg "delete") (local-bind-to-key "~ins-delete" ~key)
	    (= ~arg "abort") (local-bind-to-key "~ins-abort" ~key)
	    (= ~arg "forward") (local-bind-to-key "~ins-next" ~key)
	    (= ~arg "reverse") (local-bind-to-key "~ins-previous" ~key)
	    (= ~arg "recurse") (local-bind-to-key "~ins-recurse" ~key)
	    (= ~arg "replace") (local-bind-to-key "~ins-replace" ~key)
	    (= ~arg "search-clipboard") (local-bind-to-key "~ins-search-clipboard" ~key)
	    (= ~arg "search-word") (local-bind-to-key "~ins-search-word" ~key)
	    (= ~arg "help") (local-bind-to-key "~ins-help" ~key)
	    (= ~arg "exit") (local-bind-to-key "~ins-GetOutOf" ~key)
	    (= ~arg "exit-and-do") (local-bind-to-key "~ins-exit" ~key)
	    (= ~arg "exit-and-do-key") (local-bind-to-key "~ins-exit" ~key)
	    (= ~arg "normal") (local-bind-to-key "~ins-normal-char" ~key)
	    (if (error-occurred (local-bind-to-key ~arg ~key))
		(error-message "Unknown parameter to setup-incsearch " ~arg)
	    )
	)
    )
)

;
;	Functions for incremental search inner workings
;
(defun
    (~ins-push
	(~ins-debug (concat "~ins-push: before " ~ins-state-stack))
	(setq ~ins-state-stack
	    (concat
		(if ~ins-is-failing "1" "0")
		(if (= ~ins-direction "forward") "f" "r")
		(sprintf "%6d" (length ~inc-search-string))
		~ins-state-stack
	    )
	)
	(~ins-push-pos)
	(~ins-debug (concat "~ins-push: after " ~ins-state-stack))
    )
)
 
(defun
    (~ins-pop ~len		; pop search state
	(~ins-debug (concat "~ins-pop: before " ~ins-state-stack))
	(if (> (length ~ins-state-stack) 0)
	    (progn
		(setq ~ins-is-failing (+ (substr ~ins-state-stack 1 1)))
		(setq ~ins-direction
		    (if (= (substr ~ins-state-stack 2 1) "f")
			"forward" "reverse")
		)
		(setq ~len (+ (substr ~ins-state-stack 3 6)))
		(setq ~inc-search-string (substr ~inc-search-string 1 ~len))
		(setq ~ins-state-stack (substr ~ins-state-stack 9 10000))
		(~ins-pop-pos)
	    )
	)
	(~ins-debug (concat "~ins-pop: after " ~ins-state-stack))
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

	(if (& inc-search-use-region (! (error-occurred (mark))))
	    (setq ~ins-old-search-string (region-to-string))
	    (setq ~ins-old-search-string ~inc-search-string)
	)

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
    (~ins-AddAChar ~char			; Append last key typed
	(setq ~char (arg 1 "Next char?"))
	(if ~ins-is-failing
	    (progn
		(send-string-to-terminal "\^g")
		(~ins-message)
	    )
	    ; else
	    (progn
		(~ins-push)
		(setq ~inc-search-string (concat ~inc-search-string (char-to-string ~char)))
		(if (= ~ins-direction "forward") 
		    (if (c= ~char (following-char)) ;are we on target?
			(progn (forward-character)(~ins-message))
			;else we need to find the next one
			(~ins-forward-search)
		    )
		    ;else
		    (if (save-excursion
			    (goto-character (+ (dot) (length ~inc-search-string)))
			    (c= ~char (preceding-char))
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

(defun
    (~ins-get-clipboard-contents ~string
	(save-window-excursion
	    (temp-use-buffer "~ins-clipboard-temp")
	    (erase-buffer)
	    (UI-edit-paste)
	    (beginning-of-file)
	    (set-mark)
	    (end-of-file)
	    (setq ~string (region-to-string))
	    (delete-buffer "~ins-clipboard-temp")
	    ~string
	)
    )
)

(defun
    (~ins-search-clipboard
	~index
	~string			; append the clipboard
	(setq ~string (~ins-get-clipboard-contents))
	(setq ~index 0)
	(while (< ~index (length ~string))
	    (setq ~index (+ ~index 1))
	    (if (! ~ins-is-failing)
		(~ins-AddAChar (string-to-char (substr ~string ~index 1)))
	    )
	)
    )
)

(defun
    (~ins-get-word
	(save-excursion
	    (if (!= ~ins-direction "forward")
		(forward-character (length ~inc-search-string))
	    )
	    (if (ere-looking-at "\\w+")
		(progn
		    (region-around-match 0)
		    (region-to-string)
		)
		""
	    )
	)
    )
)

(defun
    (~ins-search-word
	~index
	~string			; append the clipboard
	(setq ~string (~ins-get-word))
	(setq ~index 0)
	(while (< ~index (length ~string))
	    (setq ~index (+ ~index 1))
	    (if (! ~ins-is-failing)
		(~ins-AddAChar (string-to-char (substr ~string ~index 1)))
	    )
	)
	(~ins-message)
    )
)

; 
;	Functions that are bound to incremental search keystrokes
; 
(defun
    (~ins-exit				; Exit and execute key
	(~ins-GetOutOf)
	(push-back-character last-keys-struck)
    )
)
    
    
(defun
    (~ins-GetOutOf			; Exit
	; restore the out local keymap
	(use-local-map
	    (if (= ~ins-old-local-keymap "")
		"no-local-map" ~ins-old-local-keymap
	    )
	)
	; check for perverse errors that leave inc search maps as the local
	; map
	(if
	    (|
		(= current-local-keymap "~incsquotemap")
		(= current-local-keymap "~incsearchmap")
	    )
	    (use-local-map "no-local-map")
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
	    (re-search-forward "^Local Bindings.*")
	    (erase-region)
	    (insert-string
		(concat
		    "You are in incremental search.\n"
		    "The special keys are:\n"
		)
	    )
;	    (while (! (error-occurred (search-forward "~ins-exit")))
;		(beginning-of-line)(set-mark)(next-line)(erase-region)
;	    )
	    (beginning-of-file)
		(~ins-help-replace-string
		    "~ins-exit" "Exit incremental search and execute command bound to this key"
		)
		(~ins-help-replace-string
		    "~ins-next" "Search forward for the next occurance of string"
		)
		(~ins-help-replace-string
		    "~ins-previous" "Search reverse for a previous occurance of string"
		)
		(~ins-help-replace-string
		    "~ins-replace" "Replace string so far with another string (you type it in)"
		)
		(~ins-help-replace-string
		    "~ins-delete" "Undo action of last character"
		)
		(~ins-help-replace-string
		    "~ins-quote" "Quote next character into the search string"
		)
		(~ins-help-replace-string
		    "~ins-GetOutOf" "Exits incremental search here."
		)
		(~ins-help-replace-string
		    "~ins-search-clipboard" "Paste the clipboard contents into the search string."
		)
		(~ins-help-replace-string
		    "~ins-help" "Print help in help buffer."
		)
		(~ins-help-replace-string
		    "~ins-abort" "Abort searching and go back to where we started"
		)
		(~ins-help-replace-string
		    "~ins-recurse" "recursive-edit & continue upon exit"
		)
		(~ins-help-replace-string
		    "~ins-normal-char" "character is appended to the search string"
		)
	    (beginning-of-file)
	    (message "Type any character to continue incremental search")
	    (get-tty-character)
	    (~ins-message)
	); end save-window-excursion
    )
)
(defun
    ~ins-help-replace-string( ~old ~new)
    (beginning-of-file)
    (error-occurred
	(replace-string ~old ~new)
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
