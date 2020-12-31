;
;   unix-process.ml     - Barry A. Scott 15-Dec-1995
;
;   This module supports Unix processes
;
(declare-buffer-specific last-line)
(if (! (is-bound ~shell-command))
    (progn
        (declare-buffer-specific ~shell-command)
        (setq-default ~shell-command (getenv "SHELL"))
           (if (= ~shell-command "")
               (setq ~shell-command "/bin/sh"))
    )
)
(if (! (is-bound shell-prompt-template))
    (progn
        (declare-buffer-specific shell-prompt-template)
        (setq-default shell-prompt-template
            (if (= ~shell-command "/bin/csh") "^[^%]% " "^[^$]*$ ")
        )
    )
)

; Bound to carriage return.  This function sends the last line of input to
; the current process.  If we are in the middle of the buffer, we guess
; about how much to send to the process, then we copy it to the end and send
; it.  If we're at the end of the buffer, we send the current region.
(defun
    (pr-newline
        ~end-of-output-marker
        ~command
        (end-of-line)
        (if (eobp)
            (progn
                ; assume the text from end of output to end of line is the
                ; command to send
                (goto-character (process-end-of-output current-buffer-name))
                (set-mark)
            )
            (progn
                ~com
                (beginning-of-line)
                (if (ere-looking-at shell-prompt-template)
                    ; move dot the end of the match
                    (region-around-match 0))
                (set-mark)
            )
        )
        (end-of-line)
        (setq ~command (region-to-string))
        (erase-region)
        (unset-mark)
        (end-of-file)
        (insert-string ~command)
        (setq last-line ~command)
        (newline)
        (end-of-file)
        (send-string-to-process current-buffer-name (concat last-line "\n"))
    )
)

; Similar to pr-newline except that if it is invoked on a line in the middle
; of the buffer, that line is sent to the subprocess and the rest of the
; buffer is erased.
(defun
    (pr-newline-kill
        (end-of-line)
        (if (eobp)
            (progn
                (setq last-line (region-to-string))
            )
            (progn
                (beginning-of-line)
                (if (looking-at shell-prompt-template)
                    (region-around-match 0))
                (set-mark)
                (end-of-line)
                (setq last-line (region-to-string))
                (set-mark)
                (end-of-file)
                (erase-region)
            )
        )
        (newline)
        (send-string-to-process current-buffer-name
            (concat last-line "\n"))
        (set-mark)
    )
)
; Expand a file name inline
(defun
    (expand-inline-filename
        (save-excursion
            (set-mark)
            (push-back-character ' ')
            (while (& (! (bolp))
                (| (>= (preceding-char) 'A')
                    (& (>= (preceding-char) '.')
                        (<= (preceding-char) '9'))))
                (backward-character))
            (push-back-character (region-to-string))
            (erase-region)
        )
        (insert-string (get-tty-file ": file ") " ")
    )
)
; Send and end-of-file signal to the process if we're at the end of the
; buffer, else just do what this key normally does.
(defun
    (send-eot
        (if (eobp)
            (send-eof-to-process current-buffer-name)
        )
    )
)
; Simulate keyboard interrupts.
(defun
    (send-int-signal
        (force-exit-process current-buffer-name)
    )
)
(defun
    (send-quit-signal
        (quit-process current-buffer-name)
    )
)
; Insert the last line the user sent to the process in this buffer.
(defun
    (grab-last-line
        (end-of-file)
        (set-mark)
        (insert-string last-line)
    )
)
; Insert this line at end of buffer for user.
(defun
    (grab-current-line prompt line
        ; Find current prompt.
        (save-excursion
            (end-of-file)
            (set-mark)
            (beginning-of-line)
            (setq prompt (region-to-string))
        )
        ; If this line has same prompt, skip it.
        (beginning-of-line)
        (if (looking-at (quote prompt))
            (provide-prefix-argument
                (length prompt)
                (forward-character)
            )
        )
        ; Now grab text and insert it at end.
        (set-mark)
        (end-of-line)
        (setq line (region-to-string))
        (end-of-file)
        (set-mark)
        (insert-string line)
        (novalue)
    )
)
; Clever hook to cd emacs with shell cds.
(defun
    (shell-cd
        (if
            (= (- (dot) (mark)) 2)
            (progn
                (if (error-occurred (change-directory (get-tty-file ": cd ")))
                    (progn
                        (delete-previous-word)
                        (message "There is no such directory")
                    )
                    (progn
                        (insert-string " " (current-directory))
                        (pr-newline)
                    )
                )
                0
            )
            1
        )
    )
)
(defun
    (generic-kill-output prompt
        (end-of-file)
        (save-excursion
            (set-mark)
            (beginning-of-line)
            (setq prompt (region-to-string)))
        (beginning-of-line)
        (set-mark)
        (re-search-reverse (concat "^" (quote prompt)))
        (if (! (dot-is-visible)) (line-to-top-of-window))
        (next-line)
        (erase-region)
        (insert-string "[output flushed]\n")
        (end-of-file)
        (set-mark)
    )
)
; Go to a shell buffer and run a shell there.
(defun
    (new-shell
        ~name
        (setq ~name (arg 1 ": new-shell (buffer name) "))
        (pop-to-buffer ~name)
        (if (< (process-status ~name) 0)
            (progn
                ~cmd
                (setq ~cmd (if (= (substr ~shell-command -3 3) "csh") "exec " ""))
                (start-process ~name (concat ~cmd ~shell-command))
                (use-abbrev-table "shell")
                (use-syntax-table "shell")
                (setq abbrev-mode 1)
                (setq current-buffer-checkpointable 0)
                (setq current-buffer-journalled 0)
                (setq wrap-long-lines 0)
                (error-occurred (shell-hook))
            )
        )
        (end-of-file)
        (novalue)
    )
)
(defun
    (shell
        (new-shell "shell")
    )
)
; Set up global stuff.
(save-excursion
    (temp-use-buffer "shell")
    (use-abbrev-table "shell")
    (setq abbrev-mode 1)
    (define-local-abbrev "~" (getenv "HOME"))
    (define-hooked-local-abbrev "cd" "cd" "shell-cd")
    (use-syntax-table "shell")
    (modify-syntax-entry "w    ~")

    (define-keymap "shell-map")
    (define-keymap "shell-ESC-map")
    (define-keymap "shell-^X-map")
    (use-local-map "shell-map")
    (local-bind-to-key "shell-ESC-map" "\e")
    (local-bind-to-key "shell-^X-map" "\^x")

    (execute-mlisp-file "unix-process.key")
    (novalue)
)
