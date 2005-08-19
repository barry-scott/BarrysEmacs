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
