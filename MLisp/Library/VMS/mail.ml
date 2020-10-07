;
; mail.ml       - VAXmail user interface
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
    ~mail-header-mode-line      ; what should be on the mode-line
    ~mail-end-of-form           ; marker set at the end of the form
    ~mail-lines-read
    ~mail-searching
)
(setq-default ~mail-header-mode-line "[-]\t\t\t\t\t\t\t\t\t     [-]")
(declare-global
    mail-message-reply-to       ; -marc - the message being answered to
    mail-k-reply-separator      ; -marc - character(s) to set off a included
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
                (if ~mail-debug         ; *debug*
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
                    (novalue)   ; all the action is in the expr.
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
        (if (! ~mail-debug)     ; *debug*
            (while (> window-size 3) (shrink-window))
        )                       ; *debug*
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
;                 "n" no map
;                 "c" command map
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
;       (arg n)     - prompt
;       (arg n+1)   - default
;       (arg n+2)   - type s=string, b=boolean
;       (arg n+3)    - call back to process result e.g. (setq x ~result)
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
                (arg 5)         ; callers buffer setup actions
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
            ;   See if we need to confirm the values filled
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
