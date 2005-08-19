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
