;
;   emacs_motif.ml
;
;   Copyright 1996-2001 Barry A. Scott
;
(execute-mlisp-file "motif-def")

(declare-buffer-specific motif-visit-pattern motif-visit-fn)
(declare-buffer-specific motif-insert-pattern motif-insert-fn)
(declare-buffer-specific motif-save-pattern motif-save-fn)
(setq-default motif-visit-pattern "")
(setq-default motif-insert-pattern "")
(setq-default motif-save-pattern "")
(defun
    (motif-visit-file
        (if (! (XmIsDialog "Visit File"))
            (XmFileSelectionDialog "Visit File" "" motif-visit-pattern motif-visit-fn)
        )
        (if (XmOperateModalDialog "Visit File")
            (visit-file motif-visit-fn)
        )
    )
)
(defun
    (motif-insert-file
        (if (! (XmIsDialog "Insert File"))
            (XmFileSelectionDialog "Insert File" "" motif-insert-pattern motif-insert-fn)
        )
        (if (XmOperateModalDialog "Insert File")
            (insert-file motif-insert-fn)
        )
    )
)
(defun
    (motif-not-implemented
        (error-message "Not implemented yet.")
    )
)
(defun
    (motif-sort-region
        (filter-region "sort")
    )
)
(defun
    (motif-view-white-space
        (setq display-non-printing-characters (! display-non-printing-characters))
    )
)
(defun
    (motif-view-wrap-long-line
        (setq wrap-long-lines (! wrap-long-lines))
    )
)
(defun
    (motif-view-read-only
        (setq read-only-buffer (! read-only-buffer))
    )
)

(defun
    (motif-save-file
        (if (! (XmIsDialog "Save File"))
            (XmFileSelectionDialog "Save File" "" motif-save-pattern motif-save-fn)
        )
        (if (XmOperateModalDialog "Save File")
            (write-named-file motif-save-fn)
        )
    )
)
(defun
    (motif-open-selected-file ~file
        (setq ~file (region-to-string))
        (if (> (length ~file) 1024)
            (error-message "The region is too big to be a reasonable filename")
        )
        (visit-file ~file)
    )
)
(defun
    (motif-copy
       (UI-edit-copy)
    )
)
(defun
    (motif-cut
       (UI-edit-copy)
       (erase-region)
       (unset-mark)
    )
)
(defun
    (motif-paste
        (UI-edit-paste)
    )
)
(defun
    (motif-edit-select-all
        (beginning-of-file)
        (set-mark 1)
        (end-of-file)
    )
)
(defun
    (motif-case-blind-search
        (setq case-fold-search (! case-fold-search))
    )
)
(defun
    (motif-save-environment
        (message "Environment saved in: " (save-environment ""))
    )
)
(defun
    (motif-delete-buffer
        (delete-buffer (current-buffer-name))
    )
)
(defun
    (motif-execute-line
        ~command
        (beginning-of-line)
        (set-mark)
        (end-of-line)
        (setq ~command (region-to-string))
        (erase-region)
        (if (eolp)
            (delete-next-character)
        )
        (set-mark)
        (filter-region ~command)
    )
)
(defun
    (motif-quit
        c l
        (setq l (fetch-array buffer-names 0))
        (setq c 1)
        (while (<= c l)
            (temp-use-buffer (fetch-array buffer-names c))
            (setq buffer-is-modified 0)
            (setq c (+ c 1))
        )
        (exit-emacs)
    )
)
(defun
    (motif-version
        motif-version

        (setq motif-version
            (concat
                "           Barry's Emacs for Unix\n\n"
                "       Copyright (c) Barry A. Scott\n\n"
                "Internet mail: barry@barrys-emacs.org\n\n"
                "           Version: " (string-extract (emacs-version) 0 (string-index-of-first (emacs-version) " ")) "\n"
            )
        )
        (if (! (XmIsDialog "Version"))
            (XmMessageDialog "Version" -1 motif-version)
        )
        (XmOperateModalDialog "Version")
    )
)
(declare-global
    ~motif-is-macro
    ~motif-is-scratch
    ~motif-is-file
    ~motif-lm
    ~motif-rm
)
(defun
    (motif-preferences
        (error-occurred (XmDestroyDialog "Preferences"))
        (if (! (XmIsDialog "Preferences"))
            (XmFormDialog "Preferences" "" 910 550
                (XmPushButtonOK "OK" 15 12 70 -1)
                (XmPushButtonCancel "Cancel" 205 12 70 -1)
                (XmRowColumn -1 -1 400 400 0 3 -1 2
                    (XmFrame 100 10 7
                        (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Editing Options")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmToggleButton -1 -1 "Default Replace Mode" default-replace-mode)
                            (XmToggleButton -1 -1 "Ask About Buffer Names" ask-about-buffer-names)
                            (XmToggleButton -1 -1 "Pop Up Windows" pop-up-windows)
                            (XmToggleButton -1 -1 "Stack Trace on Error" stack-trace-on-error)
                            (XmToggleButton -1 -1 "Track EOL on Up/Down Arrow" track-eol-on-^N-^P)
                            (XmToggleButton -1 -1 "Remove Checkpoint Files" unlink-checkpoint-files)
                            (XmToggleButton -1 -1 "Silently Kill Processes" silently-kill-processes)
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn 17 -1 -1 -1 0 -1 -1 -1
                                (XmLabel -1 -1 "Global Mode")
                                (XmTextField -1 -1 25 0 global-mode-string)
                            )
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                    (XmLabel 0 -1 "Search and Replace Options")
                                    (XmSeparator -1 -1 -1 1 -1)
                                )
                                (XmToggleButton -1 -1 "Default Case Fold Search" default-case-fold-search)
                                (XmToggleButton -1 -1 "Replace Case" replace-case)
                            )
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                    (XmLabel 0 -1 "Help Options")
                                    (XmSeparator -1 -1 -1 1 -1)
                                )
                                (XmToggleButton -1 -1 "Help on Command Error" help-on-command-completion-error)
                                (XmToggleButton -1 -1 "Remove Help Windows" remove-help-window)
                            )
                        )
                    )
                    (XmFrame 100 10 7
                        (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Editing Options")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmScale -1 -1 "Default Tab Size" 290 -1 0 32 1 1 default-tab-size)
                            (XmScale -1 -1 "Scroll Step" 290 -1 0 30 1 1 scroll-step)
                            (XmScale -1 -1 "Split Height Threshold" 290 -1 0 30 1 1 split-height-threshold)
                            (XmScale -1 -1 "Default Comment Column" 290 -1 0 150 1 1 default-comment-column)
                            (XmScale -1 -1 "Checkpoint Frequency" 290 -1 0 2000 1 1 checkpoint-frequency)
                            (XmScale -1 -1 "Journal Frequency" 290 -1 0 300 1 1 journal-frequency)
                        )
                    )
                    (XmFrame 100 -1 7
                        (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Display Options")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmToggleButton -1 -1 "Activity Indicator" activity-indicator)
                            (XmToggleButton -1 -1 "Default Display End of file" default-display-end-of-file)
                            (XmToggleButton -1 -1 "Default Display Non-printing Characters" default-display-non-printing-characters)
                            (XmToggleButton -1 -1 "Disply Controls with ^" ctlchar-with-^)
                            (XmToggleButton -1 -1 "Default Highlight Region" default-highlight-region)
                            (XmToggleButton -1 -1 "Force Redisplay" force-redisplay)
                            (XmToggleButton -1 -1 "Quick Redisplay" quick-redisplay)
                            (XmToggleButton -1 -1 "Visible Bell" visible-bell)
                            (XmToggleButton -1 -1 "Default Wrap Long Lines" default-wrap-long-lines)
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Default Mode Line Format")
                                (XmTextField -1 -1 25 0 default-mode-line-format)
                                (XmSeparator -1 -1 8 0 0)
                                (XmLabel 0 -1 "Mode Line Rendition")
                                (XmTextField -1 -1 16 0 mode-line-graphic-rendition)
                                (XmSeparator -1 -1 8 0 0)
                                (XmLabel 0 -1 "Highligh Region Rendition")
                                (XmTextField -1 -1 10 0 highlight-region-graphic-rendition)
                            )
                        )
                    )
                )
            )
        )
        (XmOperateModalDialog "Preferences")
        (novalue)
    )
)
(defun
    (motif-buffer
        (error-occurred (XmDestroyDialog "Buffer Settings"))
        (if (! (XmIsDialog "Buffer Settings"))
            (XmFormDialog "Buffer Settings" "" 550 470
                (XmPushButtonOK "OK" 15 12 70 -1)
                (XmPushButtonCancel "Cancel" 110 12 70 -1)
                (XmRowColumn 10 10 -1 -1 0 3 -1 2
                    (XmFrame 347 10 7
                        (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Buffer Type")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmFrame -1 -1 7
                                (XmRadioBox -1 -1 0
                                    (XmToggleButton -1 -1 "Macro" ~motif-is-macro)
                                    (XmToggleButton -1 -1 "File" ~motif-is-file)
                                    (XmToggleButton -1 -1 "Scratch" ~motif-is-scratch)
                                )
                            )
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Editing Settings")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmToggleButton -1 -1 "Replace Mode" replace-mode)
                            (XmToggleButton -1 -1 "Buffer Modified" buffer-is-modified)
                            (XmToggleButton -1 -1 "Abbrev Expansion" abbrev-mode)
                            (XmToggleButton -1 -1 "Read Only" read-only-buffer)
                            (XmToggleButton -1 -1 "Case Fold Search" case-fold-search)
                            (XmSeparator -1 -1 5 0 0)
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Data Protection Settings")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmToggleButton -1 -1 "Checkpointable" current-buffer-checkpointable)
                            (XmToggleButton -1 -1 "Journaling" current-buffer-journalled)
                            (XmSeparator -1 -1 5 0 0)
                        )
                    )
                    (XmFrame -1 -1 7
                        (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                            (XmRowColumn -1 -1 -1 -1 0 -1 -1 -1
                                (XmLabel 0 -1 "Display Settings")
                                (XmSeparator -1 -1 -1 1 -1)
                            )
                            (XmToggleButton -1 -1 "Display End of file" display-end-of-file)
                            (XmToggleButton -1 -1 "Display Non-printing Characters" display-non-printing-characters)
                            (XmToggleButton -1 -1 "Highlight Region" highlight-region)
                            (XmToggleButton -1 -1 "Wrap Long Lines" wrap-long-lines)
                            (XmScale -1 -1 "Comment Column" 250 -1 1 150 1 1 comment-column)
                            (XmScale -1 -1 "Tab Size" 128 -1 1 64 1 1 tab-size)
                            (XmLabel 0 -1 "Left Margin")
                            (XmTextField -1 -1 8 1 ~motif-lm)
                            (XmLabel 0 -1 "Right Margin")
                            (XmTextField -1 -1 8 1 ~motif-rm)
                        )
                    )
                )
            )
        )
        (setq ~motif-is-macro 0)
        (setq ~motif-is-scratch 0)
        (setq ~motif-is-file 0)
        (setq ~motif-lm left-margin)
        (setq ~motif-rm right-margin)
        (if (= current-buffer-type "file")
            (setq ~motif-is-file 1)
            (= current-buffer-type "macro")
            (setq ~motif-is-macro 1)
            (setq ~motif-is-scratch 1)
        )
        (if (XmOperateModalDialog "Buffer Settings")
            (progn
                (if
                    (error-occurred
                        (setq left-margin ~motif-lm)
                        (setq right-margin ~motif-rm)
                    )
                    (progn
                        (setq right-margin ~motif-rm)
                        (setq left-margin ~motif-lm)
                    )
                )
                (if ~motif-is-file
                    (setq current-buffer-type "file")
                    ~motif-is-macro
                    (setq current-buffer-type "macro")
                    ~motif-is-scratch
                    (setq current-buffer-type "scratch")
                )
            )
        )
        (novalue)
    )
)
(defun
    (motif-save-settings
        (save-excursion
            (temp-use-buffer "*GOK*")
            (erase-buffer)
            (insert-string "(setq activity-indicator " activity-indicator ")\n")
            (insert-string "(setq default-display-end-of-file " default-display-end-of-file ")\n")
            (insert-string "(setq default-display-non-printing-characters "  default-display-non-printing-characters ")\n")
            (insert-string "(setq ctlchar-with-^ " ctlchar-with-^ ")\n")
            (insert-string "(setq default-highlight-region " default-highlight-region ")\n")
            (insert-string "(setq force-redisplay " force-redisplay ")\n")
            (insert-string "(setq quick-redisplay " quick-redisplay ")\n")
            (insert-string "(setq visible-bell " visible-bell ")\n")
            (insert-string "(setq wrap-long-lines " wrap-long-lines ")\n")
            (insert-string "(setq default-replace-mode " default-replace-mode ")\n")
            (insert-string "(setq ask-about-buffer-names " ask-about-buffer-names ")\n")
            (insert-string "(setq stack-trace-on-error " stack-trace-on-error ")\n")
            (insert-string "(setq track-eol-on-^N-^P " track-eol-on-^N-^P ")\n")
            (insert-string "(setq unlink-checkpoint-files " unlink-checkpoint-files ")\n")
            (insert-string "(setq silently-kill-processes " silently-kill-processes ")\n")
            (insert-string "(setq default-case-fold-search " default-case-fold-search ")\n")
            (insert-string "(setq replace-case " replace-case ")\n")
            (insert-string "(setq help-on-command-completion-error " help-on-command-completion-error ")\n")
            (insert-string "(setq remove-help-window " remove-help-window ")\n")
            (insert-string "(setq default-tab-size " default-tab-size ")\n")
            (insert-string "(setq split-height-threshold " split-height-threshold ")\n")
            (insert-string "(setq default-comment-column " default-comment-column ")\n")
            (insert-string "(setq checkpoint-frequency " checkpoint-frequency ")\n")
            (insert-string "(setq journal-frequency " journal-frequency ")\n")
            (insert-string "(setq default-mode-line-format \"" default-mode-line-format "\")\n")
            (insert-string "(setq mode-line-graphic-rendition \"" mode-line-graphic-rendition "\")\n")
            (insert-string "(setq highlight-region-graphic-rendition \"" highlight-region-graphic-rendition "\")\n")
            (if (error-occurred (put-database-entry "MLisp-library" "emacs_motif_settings.ml"))
                (write-named-file "sys$login:emacs_motif_settings.ml")
            )
        )
        (delete-buffer "*GOK*")
    )
)
(defun
    (motif-restore-settings
        (if (error-occurred (execute-mlisp-file "emacs_motif_settings"))
            (error-occurred (execute-mlisp-file "sys$login:emacs_motif_settings.ml"))
        )
    )
)
(declare-global
    ~motif-find-string
    ~motif-replace-string
    ~motif-match-case
    ~motif-regular-expression
)

(setq ~motif-find-string "")
(setq ~motif-replace-string "")
(defun
    (motif-find
        (error-occurred (XmDestroyDialog "Find"))
        (if (! (XmIsDialog "Find"))
            (XmFormDialog "Find" "\[menu]MFf" 490 80
                (XmPushButtonUser "Forward" "\[menu]MFf" 15 12 80 -1)
                (XmPushButtonUser "Backward" "\[menu]MFb" 110 12 80 -1)
                (XmPushButtonCancel "Close" 205 12 80 -1)
                (XmRowColumn -1 -1 -1 -1 0 3 XmHORIZONTAL XmPACK_TIGHT
                    (XmLabel -1 -1 "Search for:")
                    (XmTextField -1 -1 40 1 ~motif-find-string)
                    (XmToggleButton -1 -1 "Regular expression" ~motif-regular-expression)
                )
                (XmToggleButton 338 42 "Match Case" ~motif-match-case)
            )
        )
        (setq ~motif-find-string search-string)
        (XmOperateModelessDialog "Find")
    )
)
(defun
    (motif-find-and-replace
        (error-occurred (XmDestroyDialog "Find and Replace"))
        (if (! (XmIsDialog "Find and Replace"))
            (XmFormDialog "Find and Replace" "\[menu]MFf" 490 125
                (XmPushButtonUser "Forward" "\[menu]MFf" 15 12 80 -1)
                (XmPushButtonUser "Backward" "\[menu]MFb" 110 12 80 -1)
                (XmPushButtonUser "Replace" "\[menu]MFr" 205 12 80 -1)
                (XmPushButtonUser "Replace All" "\[menu]MFa" 300 12 80 -1)
                (XmPushButtonCancel "Close" 395 12 80 -1)
                (XmRowColumn -1 -1 -1 -1 0 1 XmVERTICAL XmPACK_COLUMN
                    (XmRowColumn -1 -1 -1 -1 0 3 XmHORIZONTAL XmPACK_TIGHT
                        (XmLabel -1 -1 "  Search for:")
                        (XmTextField -1 -1 40 1 ~motif-find-string)
                        (XmToggleButton -1 -1 "Regular expression" ~motif-regular-expression)
                    )
                    (XmRowColumn -1 -1 -1 -1 0 3 XmHORIZONTAL XmPACK_TIGHT
                        (XmLabel -1 -1 "Replace with:")
                        (XmTextField -1 -1 40 1 ~motif-replace-string)
                        (XmToggleButton -1 -1 "Match Case" ~motif-match-case)
                    )
                )
            )
        )
        (setq ~motif-find-string search-string)
        (XmOperateModelessDialog "Find and Replace")
    )
)
(defun
    (motif-do-find-forward
        ~status
        ~saved-case-fold-search

        (setq ~saved-case-fold-search case-fold-search)
        (setq case-fold-search (! ~motif-match-case))
        (setq ~status
            (error-occurred
                (if ~motif-regular-expression
                    (re-search-forward ~motif-find-string)
                    (search-forward ~motif-find-string)
                )
            )
        )
        (setq case-fold-search ~saved-case-fold-search)
        (if ~status (message error-message))
    )
)
(defun
    (motif-do-find-backward
        ~status
        ~saved-case-fold-search

        (setq ~saved-case-fold-search case-fold-search)
        (setq case-fold-search (! ~motif-match-case))
        (setq ~status
            (error-occurred
                (if ~motif-regular-expression
                    (re-search-reverse ~motif-find-string)
                    (search-reverse ~motif-find-string)
                )
            )
        )
        (setq case-fold-search ~saved-case-fold-search)
        (if ~status (message error-message))
    )
)
(defun
    (motif-do-find-replace-one
        (message "Replace one " ~motif-find-string " with " ~motif-replace-string)
    )
)
(defun
    (motif-do-find-replace-all
        (message "Replace all " ~motif-find-string " with " ~motif-replace-string)
    )
)
(bind-to-key "motif-do-find-forward" "\[menu]MFf")
(bind-to-key "motif-do-find-backward" "\[menu]MFb")
(bind-to-key "motif-do-find-replace-one" "\[menu]MFr")
(bind-to-key "motif-do-find-replace-all" "\[menu]MFa")
