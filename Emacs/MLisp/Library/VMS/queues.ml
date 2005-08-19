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
