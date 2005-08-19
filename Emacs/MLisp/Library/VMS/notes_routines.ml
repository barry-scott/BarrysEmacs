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
