;
; notes.ml      - VAXnotes user interface
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
    ~notes-header-mode-line     ; what should be on the mode-line
    ~notes-lines-read
    ~notes-searching
    ~notes-last-read-note-id    ; used by read-last command
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
                (if ~notes-debug        ; *debug*
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
                    (novalue)   ; all the action is in the expr.
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
;                       -<conference-title>-
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
            (insert-string "