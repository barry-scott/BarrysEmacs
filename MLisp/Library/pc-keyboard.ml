;
; pc-keyboard.ml
;
; Barry A. Scott (c) 1997-2016
;
; bind keys on a PC style keyboard to functions that
; match the behaviour defined by Windows interface.
;

(declare-global
    PC-open-file-filters)
(setq PC-open-file-filters
    (concat
        (if (= "Windows" operating-system-name) "All (*.*)" "All (*)") "\n"
        "C/C++ (*.h *.c *.hpp *.hxx *.cpp *.cxx)" "\n"
        "MLisp (*.ml *.key *.mlp)" "\n"
        "HTML (*.html *.css *.js)" "\n"
        "Python (*.py)"
    )
)

(defun
    (PC-open-file
        (visit-file
            (UI-python-hook
                "open-file-dialog"
                "Open file"
                1               ; existing file
                PC-open-file-filters
                1               ; detailed
                (current-directory)
                ""
            )
        )
    )
)

(defun
    (PC-save-file
        (if (= current-buffer-type "scratch")
            (PC-save-file-as)
            (= current-buffer-file-name "")
            (PC-save-file-as)
            (write-current-file)
        )
    )
)

(defun
    (PC-save-file-as
        (write-named-file
            (UI-python-hook
                "open-file-dialog"
                "Save file as"
                0
                PC-open-file-filters
                1
                (if (= current-buffer-type "scratch")
                    (current-directory)
                    (file-format-string "%pa" (current-file-name))
                )
                (if (= current-buffer-type "scratch")
                    current-buffer-name
                    (file-format-string "%fa" (current-file-name))
                )
            )
        )
        ; change the buffer name to reflect the new filename
        (reset-buffer-name-from-filename)
    )
)

(defun
    (PC-home-key
        (goto-character
            (save-excursion
                ~starting-dot
                (setq ~starting-dot (dot))
                (beginning-of-line)
                (if (looking-at "[ \t][ \t]*")
                    (progn
                        (region-around-match 0)
                        (if (= ~starting-dot (dot))
                            (beginning-of-line)
                        )
                    )
                )
                (dot)
            )
        )
    )
)
(defun
    (PC-edit-copy
        ;  only copy if a region is set
        (if (! (error-occurred (mark)))
            (UI-edit-copy)
        )
    )
)
(defun
    (PC-edit-cut
        ;  only cot if a region is set
        (if (! (error-occurred (mark)))
            (progn
                (save-excursion (UI-edit-copy))
                (erase-region)
                (unset-mark)
            )
        )
    )
)

(defun
    (PC-select-all
        (beginning-of-file)
        ; Use gui version of set-mark
        (set-mark 1)
        (end-of-file)
    )
)

(defun
    (PC-cut-or-ctrl-x-prefix
        ; if mark set then cut
        (if (! (error-occurred (mark)))
            (PC-edit-cut)
            (progn
                (push-back-character '^x')
                (if prefix-argument-provided
                    (return-prefix-argument prefix-argument)
                )
            )
        )
    )
)
(bind-to-key "previous-line" "\[up]")
(bind-to-key "next-line" "\[down]")

(bind-to-key "forward-character" "\[right]")
(bind-to-key "backward-character" "\[left]")

(bind-to-key "scroll-one-line-up" "\[ctrl-up]")
(bind-to-key "scroll-one-line-down" "\[ctrl-down]")

(bind-to-key "forward-word" "\[ctrl-right]")
(bind-to-key "backward-word" "\[ctrl-left]")

(bind-to-key "(setq replace-mode (! replace-mode))" "\[insert]")
(bind-to-key "delete-next-character" "\[delete]")
(bind-to-key "PC-home-key" "\[home]")
(bind-to-key "end-of-line" "\[end]")

(bind-to-key "beginning-of-file" "\[ctrl-home]")
(bind-to-key "end-of-file" "\[ctrl-end]")

(bind-to-key "next-page" "\[page-down]")
(bind-to-key "previous-page" "\[page-up]")

(bind-to-key "end-of-window" "\[ctrl-page-down]")
(bind-to-key "beginning-of-window" "\[ctrl-page-up]")

(bind-to-key "delete-previous-word" "\[ctrl-backspace]")
(bind-to-key "delete-next-word" "\[ctrl-delete]")

;
; Old Windows cut/copy/paste bindings
;
(bind-to-key "PC-edit-copy" "\[ctrl-insert]")
(bind-to-key "UI-edit-paste" "\[shift-insert]")
(bind-to-key "PC-edit-cut" "\[shift-delete]")

;
; Windows/Mac short cut keys for edit functions
;
(bind-to-key "PC-select-all" "\^a")
(bind-to-key "new-undo" "\^z")
(bind-to-key "PC-edit-copy" "\^c")
(bind-to-key "UI-edit-paste" "\^v")
(bind-to-key "PC-cut-or-ctrl-x-prefix" "\034")
(bind-to-key "PC-cut-or-ctrl-x-prefix" "\035")

;
; It is expected that the user will swap one of the \034 \035
; with ^x so that ^x is the cut and ^] or ^\ is the ctrl-x prefix
;

; Function keys

(bind-to-key "help" "\[f1]")


;
; Menu binding
;
(defun PC-toggle-bool-var(~var)
    (save-excursion
        (if (!= "" (previous-buffer-name))
            (temp-use-buffer (previous-buffer-name))
        )
        (set ~var (! (execute-mlisp-line ~var)))
    )
)

; File
(bind-to-key "PC-open-file"                                 "\[menu]fo")
(bind-to-key "PC-save-file"                                 "\[menu]fs")
(bind-to-key "PC-save-file-as"                              "\[menu]fa")
(bind-to-key "write-modified-files"                         "\[menu]fl")

; Edit
(bind-to-key "new-undo"                                     "\[menu]eu")
(bind-to-key "PC-edit-cut"                                  "\[menu]ex")
(bind-to-key "PC-edit-copy"                                 "\[menu]ec")
(bind-to-key "UI-edit-paste"                                "\[menu]ev")
(bind-to-key "(PC-toggle-bool-var \"case-fold-search\")"    "\[menu]eS")
(bind-to-key "(PC-toggle-bool-var \"replace-case\")"        "\[menu]eR")
(bind-to-key "goto-line"                                    "\[menu]eg")
(bind-to-key "PC-select-all"                                "\[menu]ea")
; Edit Advanced...
(bind-to-key "delete-white-space"                           "\[menu]cw")
(bind-to-key "case-upper"                                   "\[menu]cu")
(bind-to-key "case-lower"                                   "\[menu]cl")
(bind-to-key "case-capitalize"                              "\[menu]cc")
(bind-to-key "case-invert"                                  "\[menu]ci")
; Edit Region...
(bind-to-key "replace-tabs-with-spaces-in-buffer"           "\[menu]rT")
(bind-to-key "indent-region"                                "\[menu]ri")
(bind-to-key "undent-region"                                "\[menu]rI")
(bind-to-key "narrow-region"                                "\[menu]rn")
(bind-to-key "widen-region"                                 "\[menu]rw")
; View
(bind-to-key "(PC-toggle-bool-var \"display-non-printing-characters\")"
                                                            "\[menu]vw")
(bind-to-key "(PC-toggle-bool-var \"wrap-long-lines\")"     "\[menu]vl")
; Macro
(bind-to-key "start-remembering"                            "\[menu]mr")
(bind-to-key "stop-remembering"                             "\[menu]ms")
(bind-to-key "execute-keyboard-macro"                       "\[menu]mp")
; Buffer
(bind-to-key "switch-to-buffer"                             "\[menu]bs")
(bind-to-key "list-buffers"                                 "\[menu]bl")
(bind-to-key "erase-buffer"                                 "\[menu]be")
; Window
(bind-to-key "split-current-window"                         "\[menu]wh")
(bind-to-key "split-current-window-vertically"              "\[menu]wv")
(bind-to-key "delete-other-windows"                         "\[menu]wo")
(bind-to-key "delete-window"                                "\[menu]wt")
; Build
(bind-to-key "compile-it"                                   "\[menu]bc")
(bind-to-key "next-error"                                   "\[menu]bn")
(bind-to-key "previous-error"                               "\[menu]bp")
; Tools
(bind-to-key "grep-in-files"                                "\[menu]tg")
(bind-to-key "grep-in-buffers"                              "\[menu]tb")
(bind-to-key "grep-current-buffer"                          "\[menu]tc")
(bind-to-key "shell"                                        "\[menu]ts")
(bind-to-key "filter-region"                                "\[menu]rf")
(bind-to-key "(filter-region \"sort\")"                     "\[menu]rs")
