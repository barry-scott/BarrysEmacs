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
