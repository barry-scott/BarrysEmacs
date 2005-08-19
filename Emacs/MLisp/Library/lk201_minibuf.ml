; 
; Minibuffer bindings for LK201
; 
; 
; Here are the bindings of the VT100 keys in the Minibuf
; and the vt200_series bindings.
; 
(save-excursion
    (temp-use-buffer "keypad-hack")
    ; 
    ; Keymap used with spaces as data
    ; 
    (use-local-map "Minibuf-local-map")
    (remove-local-binding "\e")
    (remove-local-binding "\^\")
    (local-bind-to-key "self-insert" "?")		; ?
    (local-bind-to-key "help-and-exit" "\(help)")	; Help
    (local-bind-to-key "expand-and-exit" "\(do)")	; Do
    ; 
    ; Keypad used with spaces as expansion
    ; 
    (use-local-map "Minibuf-local-NS-map")
    (remove-local-binding "\e")
    (remove-local-binding "\^\")
    (local-bind-to-key "self-insert" "?")		; ?
    (local-bind-to-key "help-and-exit" "\(help)")	; Help
    (local-bind-to-key "expand-and-exit" "\(do)")	; Do

    (delete-buffer "keypad-hack")
)
