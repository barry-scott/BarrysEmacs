; This package provides a replacement for the standard ^X^E/^X^N compilation
; package.  It operates pretty much the same as the old except that the
; process control facilities are used.  Consequently, while a compilation is
; going on you can go off and do other things and (a major win) you can
; interrupt the compilation partway through.
(declare-global compilation-may-be-active compile-it-command errors-scanned)
(if (= operating-system-name "VMS")
    (setq compile-it-command "MMS")
    (= operating-system-name "Windows")
    (setq compile-it-command "make")
    (setq compile-it-command "make")
)
(error-occurred (rename-macro "compile-it" "old-compile-it"))
(error-occurred (rename-macro "next-error" "old-next-error"))
(error-occurred (rename-macro "previous-error" "old-previous-error"))

(defun
    (compile-it command
        ~arg
        (setq ~arg (if (> prefix-argument 1)
                          (arg 1 ": compile-it using command: ")
                         ""))
        (if (length ~arg)
            (setq compile-it-command ~arg))
        (setq compilation-may-be-active 1)
        (setq errors-scanned 0)
        (save-excursion
            ; make sure that the Error log window appears
            ; the save-excursion will undo the pop-to-buffer without the
            ; split-current-window
            (if (= (buffer-size) 0)
                (split-current-window))
            (pop-to-buffer "Error log")
            (erase-buffer)
            (setq highlight-region 0)
            (write-modified-files)
            (error-occurred (kill-process "Error log"))
            (if (= operating-system-name "VMS")
                (progn
                    (start-DCL-process "Error log")
                    (set-process-output-buffer "Error log" "Error log")
                    (set-process-input-procedure "Error log" "~start-MMS")
                )
                (= operating-system-name "Windows")
                (progn ~old-cli-name ~status
                    (setq ~old-cli-name cli-name)
                    (setq cli-name (concat (getenv "COMSPEC") " /c " compile-it-command))
                    (setq ~status (error-occurred (start-DCL-process "Error log")))
                    (setq cli-name ~old-cli-name)
                    (if ~status (error-message error-message))
                    (set-process-termination-procedure "Error log" "~compil-terminated")
                    (set-process-output-buffer "Error log" "Error log")
                )
                (progn
                    (start-process "Error log" compile-it-command)
                    (set-process-termination-procedure "Error log" "~compil-terminated")
                )
            )
            (setq mode-line-format "%b %M (%m)")
            (setq mode-string "Compilation active")
            (novalue)
        )
    )
)

(if (= operating-system-name "VMS")
    (progn
        (defun  (~start-MMS
                (set-process-input-procedure "Error log" "~stop-MMS")
                (send-string-to-process "Error log" compile-it-command)
                )
        )

        (defun (~stop-MMS
                (error-occurred
                        (kill-process "Error log"))
                (setq compilation-may-be-active 0)
                (save-excursion
                    (temp-use-buffer "Error log")
                    (setq mode-string "Finished"))
                    (sit-for 0)
               )
        )
    )
    (progn
        (defun (~compil-terminated
                (setq compilation-may-be-active 0)
                (save-excursion
                    (temp-use-buffer "Error log")
                    (setq mode-string "Finished"))
                    (sit-for 0)
               )
        )
    )
)
(defun
    (kill-compilation
        (save-excursion
            (temp-use-buffer "Error log")
            (error-occurred
                (kill-process "Error log"))
            (setq mode-string "Compilation killed")
            (sit-for 0)
            (setq compilation-may-be-active 0)
            (setq buffer-is-modified 0)
            (novalue)
        )
    )
)

(defun
    (previous-error
        (if (! errors-scanned)
            (next-error)
            (if (error-occurred (old-previous-error))
                (progn
                    (save-excursion
                        (pop-to-buffer "Error log")
                        (sit-for 0)
                    )
                    (error-message "No more errors...")
                )
            )
        )
    )
)

(defun
    (next-error
        (save-excursion
            (if compilation-may-be-active
                (error-message "The compilation is still running")
            )
            (if (! errors-scanned)
                (progn
                    (pop-to-buffer "Error log")
                    (setq mode-string "Scanning for errors")
                    (sit-for 0)
                    (setq buffer-is-modified 0)
                    (beginning-of-file)
                    (set-mark)
                    (end-of-file)
                    (parse-error-messages-in-region)
                    (setq compilation-may-be-active 0)
                    (setq errors-scanned 1)
                    (beginning-of-file)
                    (setq mode-string "Displaying errors")
                    (sit-for 0)
                )
            )
        )
        (if (error-occurred (old-next-error))
            (progn
                (save-excursion
                    (pop-to-buffer "Error log")
                    (setq mode-string "All errors displayed")
                    (sit-for 0)
                )
                (error-message "No more errors...")
            )
        )
        )
)

(bind-to-key "kill-compilation" "\^x\^K"))
(novalue)
