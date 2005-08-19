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
