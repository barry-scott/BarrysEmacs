(declare-global
    ~shell
    shell-cli
    shell-max-lines             ; size of the lines array
    ~shell-cr-prefix            ;  what to put on the start of a
                                ; command line
    ~shell-line-suffix          ; what to tack on the end of a line
)
(declare-buffer-specific
    ~shell-ready                ; true when the process will accept input
    ~shell-input-lines          ; array of input lines past, present and
                            ; future
    ~shell-first-line   ; first line used in the array
    ~shell-last-line    ; number of last line used in the array
    ~shell-current-line ; number of current line in the array
    ~shell-recall-line  ; number of line to be recalled next
    shell-ctrlc
    shell-feeps)
    (progn
        (if (= (concat shell-cli) "0")
            (setq shell-cli "DCL")
        )
        (setq ~shell-cr-prefix "$")
        (setq ~SHELL-LINE-SUFFIX "")
    )
)

(setq-default ~shell-last-line "")      ; Last line in the window
(setq-default shell-ctrlc 44)   ; set to %SYSTEM-F-ABORT
(setq-default shell-feeps 0)    ; # of bell after command complete
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
                    "   Key             Action\n"
                    "   ---             ------\n"
                    "   ^?              print this help text\n"
                    "   ^R              insert the last execute command at the cursor\n"
                    "   <RET>           execute the current line as a DCL command\n"
                    "   <LF>            send the current line to the running program\n"
                    "   ^]              finish this shell session\n"
                    "   ^C              abort the currently running program\n"
                    "   Other characters perform their usual functions.\n"
                    "   Variable        Action\n"
                    "   --------        ------\n"
                    "   shell-ctrlc     provide the termination code for ^C\n"
                    "   shell-feeps     set the number of bells output when a\n"
                    "                   process input request is made."
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
