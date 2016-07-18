;
; LK201.ml  - support the LK201 keyboard
;
(progn
;
; turn on LK201 keyboard emulation
;
(setq keyboard-emulates-lk201-keypad 1)

;
; define the key bindings
;
(bind-to-key "help" "\(help)")                          ; Help
(bind-to-key "execute-extended-command" "\(do)")        ; Do

(bind-to-key "~lk201-find" "\(find)")                   ; Find
    (bind-to-key "search-forward" "\(gold)\(find)")     ; Gold Find
(bind-to-key "~lk201-insert-here" "\(insert here)")     ; Insert Here
(bind-to-key "~lk201-remove" "\(remove)")               ; Remove
(bind-to-key "set-mark" "\(select)")                    ; Select key
(bind-to-key "previous-page" "\(prev screen)")          ; Previous Screen
(bind-to-key "next-page" "\(next screen)")              ; Next Screen

(bind-to-key "previous-line" "\(up)")                   ; Up arrow
(bind-to-key "backward-character" "\(left)")            ; Left arrow
(bind-to-key "next-line" "\(down)")                     ; Down arrow
(bind-to-key "forward-character" "\(right)")            ; Right arrow

(bind-to-key "~lk201-push-back-ESC" "\(f11)")           ; F11
(bind-to-key "beginning-of-line" "\(f12)")              ; F12
(bind-to-key "~lk201-push-back-LF" "\(f13)")            ; F13
(bind-to-key "toggle-replace-mode" "\(f14)")            ; F14
(bind-to-key "visit-file" "\(f17)")                     ; F17
(bind-to-key "switch-to-buffer" "\(f18)")               ; F18
(bind-to-key "delete-other-windows" "\(f19)")           ; F19
(bind-to-key "split-current-window" "\(gold)\(f19)")    ; PF1-F19

;
; define the functions that make the keys work
;
(defun
    (~lk201-find
        (search-forward "")
    )
)
(defun
    (~lk201-insert-here
        (yank-buffer "LK201-remove-buffer")
    )
)
(defun
    (~lk201-remove
        (delete-region-to-buffer "LK201-remove-buffer")
        (unset-mark)
    )
)

;
; push back an ESC
;
(defun
    (~lk201-push-back-ESC
        (push-back-character '\e')
        (if prefix-argument-provided (return-prefix-argument prefix-argument))
    )
)
;
; push back an LF
;
(defun
    (~lk201-push-back-LF
        (push-back-character '^j')
    )
)
;
; Toggle the replace mode setting
;
(defun
    (toggle-replace-mode
        (setq replace-mode (! replace-mode))
    )
)
;
;  enable the keypad
;
(defun
    (LK201-on
        (setq keyboard-emulates-lk201-keypad 1)
        (if (= terminal-is-terminal 1)
            (send-string-to-terminal "\e=\(csi)?1h")
        )
    )
)
;
; disable the keypad
;
(defun
    (LK201-off
        (if (= terminal-is-terminal 1)
            (if (! terminal-application-keypad)
                (send-string-to-terminal "\e>\(csi)?1l")
            )
        )
    )
)
)
