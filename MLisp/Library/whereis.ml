;       +----------------+----------------------------------------------+
;       | File Name:     | dba1:[surtees.emacs]whereis.ml               |
;       | Edit Version:  | 4                                            |
;       | Date Modified: | Tue Apr  6 23:35:45 1982                     |
;       | Updated By:    | SURTEES                                      |
;       | From Node:     | _R2ME2::                                     |
;       +----------------+----------------------------------------------+
;
; Updated: 11-May-1998 by Barry A. Scott
; Now uses apropos-command so that the full set of keys bound to a command
; are reported
;
; Display the keys that a command is bound to.
;
(defun
    (where-is ~keys ~command
        (if (! (interactive))
            (error-message "where-is cannot be called from MLisp")
        )
        (setq ~command (get-tty-command ": where-is "))
        (save-window-excursion
            (apropos-command ~command)
            (switch-to-buffer "Help")
            (if
                (error-occurred
                    (re-search-forward (concat "^" ~command "  *(\\(.*\\))"))
                )
                (error-message "Command """ ~command """ is not bound to a key")
            )
            (region-around-match 1)
            (setq ~keys (region-to-string))
            (re-search-forward "[ ]*")
            (message "Command """ ~command """ is bound to " ~keys)
        )
    )
)
