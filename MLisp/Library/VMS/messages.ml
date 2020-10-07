;
;  messages.ml package -- traps broadcasts to terminal and displays in
;  pop-up "Broadcast messages" buffer.  It removes non-printing characters
;  from the message, and optionally compresses REPLY format messages.
;
; (from EMACS X4.0 FT2 package)
;
; Modified by Dave Butenhof, 06-Jan-1985
;       1) Modify "Broadcast messages" mode line -- it will include a count
;          of messages received, as well as the current time in the mode
;          variable.
;       2) Make the routine which pops up the "Broadcast messages" buffer
;          and inserts the message callable (with message and time
;          arguments) so that all packages which want to insert messages can
;          do so easily and without duplication of code.
;       3) Fix search string corruption bug.
;
; modified by Barry A. Scott, 17-Nov-1985
;       merge Emacs V4.0 package with Daves mods.
;
; modified by Barry A. Scott,  3-aug-1986
;       add key bindings to the message buffer

;
;   broadcast message handling macro
;
(if (! (is-bound broadcast-feeps))
    (progn
        (declare-global broadcast-feeps)
        (setq broadcast-feeps 1)
    )
)
(if (! (is-bound ~message-count))
    (progn
        (declare-global ~message-count)
        (setq ~message-count 0)
    )
)
(if (! (is-bound broadcast-compress-message))
    (progn
        (declare-global broadcast-compress-message)
        (setq broadcast-compress-message 0)
    )
)
(if (! (is-bound broadcast-display-whole-message))
    (progn
        (declare-global broadcast-display-whole-message)
        (setq broadcast-display-whole-message 0)
    )
)
(if (= operating-system-name "VMS")
(progn
;
; process new message
;
(defun
    ~insert-message (text new-mode) ~broadcast-i ~broadcast-win
    ;
    ; work out where the current window is releative to the
    ; Minibuf
    ;
    (if (= current-buffer-name "Minibuf")
        (setq ~broadcast-win -1)
        (save-window-excursion
            (setq ~broadcast-win 0)
            (switch-to-buffer "~broadcast-find-me")
            (pop-to-buffer "Minibuf")
            (while (!= "~broadcast-find-me" current-buffer-name)
                (previous-window)
                (setq ~broadcast-win (+ ~broadcast-win 1))
            )
            (delete-buffer  "~broadcast-find-me")
        )
    )

    ;
    ; create a one line message window at the bottom of the screen
    ;
    (pop-to-buffer "Minibuf")
    (previous-window)
    (if (!= current-buffer-name "Broadcast messages")
        (if (error-occurred (split-current-window))
            (error-occurred
                (provide-prefix-argument 3 (enlarge-window))
                (split-current-window)
            )
        )
    )
    (if (!= current-buffer-name "Broadcast messages")
        (setq ~broadcast-win (+ ~broadcast-win 1))
    )
    (switch-to-buffer "Broadcast messages")
    (setq highlight-region 0)
    (if (!= new-mode "")
        (setq mode-string new-mode)
    )
    (error-occurred (while 1 (shrink-window)))
    (end-of-file)
    (if (! (bolp))
        (newline)
    )
    (set-mark)
    (if (!= text "")
        (progn
            (setq ~message-count (+ ~message-count 1))
            (insert-string text)
        )
        (if
            (&
                (= ~message-count 0)
                (= (buffer-size) 0)
            )
            (insert-string "---- no messages ----")
        )
    )
    (setq mode-line-format
        (concat
            ~message-count
            " broadcast message"
            (if (= ~message-count 1)
                (concat "")
                (concat "s")
            )
            ", at %m"
        )
    )
    (exchange-dot-and-mark)
    (save-restriction ~height
        (save-excursion
            (narrow-region)
            (error-occurred (re-replace-string "[\^A-\^H\^K-\^Z]" ""))
            (error-occurred (re-replace-string "\^J\^J" "\^J"))
            (if broadcast-compress-message
                (progn
                    (if (! (error-occurred (re-search-forward "\^MDECnet event")))
                        (progn
                            (beginning-of-file)
                            (set-mark)
                            (re-search-forward "\^MDECnet")
                            (backward-word)
                            (erase-region)

                        )
                        (|
                            (looking-at "^SHUTDOWN")
                            (looking-at "^\\*URGENT\\*")
                        )
                        (progn
                            (forward-word)
                            (delete-next-word)
                            (delete-next-word)
                            (delete-next-word)
                            (forward-word)
                            (delete-next-word)
                            (forward-word)
                            (delete-next-word)
                            (delete-next-word)
                        )
                        (looking-at "^Reply")
                        (progn
                            (error-occurred (re-replace-string "\^J" "\^M"))
                            (provide-prefix-argument 6
                                (delete-next-word)
                            )
                            (insert-string "From")
                            (forward-word)
                            (delete-next-word)
                            (delete-next-word)
                        )
                    )
                )
            )
        )
        (if broadcast-display-whole-message
            (setq window-size
                (+
                    (-
                        (progn (end-of-file) (current-line-number))
                        (progn (beginning-of-file) (current-line-number))
                    )
                    1
                )
            )
        )
    )
    (if broadcast-display-whole-message
        (line-to-top-of-window)
        (progn
            (exchange-dot-and-mark)
            (end-of-file)
            (error-occurred (backward-character))
        )
    )
    ;
    ; restore the cursor position
    ;
    (pop-to-buffer "Minibuf")
    (while (>= (setq ~broadcast-win (- ~broadcast-win 1)) 0)
        (previous-window)
    )
    (if (!= text "")
        (progn
            (setq ~broadcast-i broadcast-feeps)
            (while (> ~broadcast-i 0)
                (setq ~broadcast-i (- ~broadcast-i 1))
                (send-string-to-terminal "\^G")
            )
        )
    )
    (sit-for 0)
)
(defun
    (~broadcast-message
        (~insert-message
            (process-output "broadcast-messages")
            (substr (current-time) 12 5)
        )
    )
)
(defun
    (reset-message-buffer
        (save-window-excursion
            (temp-use-buffer "Broadcast messages")
            (setq current-buffer-checkpointable 0)
            (setq current-buffer-journalled 0)
            (erase-buffer)
            (use-syntax-table "broadcast-messages")
            (modify-syntax-table "word" "$_:*")
            (setq ~message-count 0)
            (setq mode-line-format "0 broadcast messages, at %m")
            (setq mode-string (substr (current-time) 12 5))
            (sit-for 0)
        )
        (novalue)
    )
)
(defun
    (view-message-buffer
        ~remove-message-window

        (setq ~remove-message-window 0)
        (save-window-excursion
            (delete-other-windows)
            (end-of-file)
            (previous-line (- window-size 2))
            (line-to-top-of-window)
            (recursive-edit)
        )
        (if ~remove-message-window (delete-window))
    )
)
(defun
    (delete-message-window
        (delete-window)
        (if (is-bound ~remove-message-window)
            (progn
                (setq ~remove-message-window 1)
                (exit-emacs)
            )
        )
    )
)
(error-occurred (start-DCL-process "broadcast-messages"))
(set-process-output-procedure "broadcast-messages" "~broadcast-message")
(reset-message-buffer)
(save-excursion
    (temp-use-buffer "Broadcast messages")
    (error-occurred (execute-mlisp-file "messages.key"))
)
(novalue)
)
)
