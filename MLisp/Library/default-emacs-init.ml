;
; default-emacs-init.ml
;
; This module can be used as the basis of the users emacsinit.ml
; initialisation procedure.
;
; Copyright (c) 1998-2017 Barry A. Scott
;

;
; ~ui-style is one of:-
;   "windows" - Microsoft windows style UI
;
(defun ~is-ui-style(~ui-style)
    ; return true if the style is in the list
    ;  ~ui-style-list is inherited from the outer environment
    (>=
        (string-index-of-string
            (concat "," ~ui-style-list ",")
            (concat "," ~ui-style ",")
            0
        )
        0
    )
)

(defun
    default-emacs-init(~ui-style-list)

    ;
    ; turn off checkpointing
    ;
    (setq unlink-checkpoint-files 1)
    (setq checkpoint-frequency 0)
    ;
    ; use journaling for security of edits
    ;
    (setq journal-frequency 30) ; flush journal after 30 seconds of inacivity
    (setq journal-scratch-buffers 0); only journal file buffers

    ;
    ; Default to case blind searching
    ;
    (setq case-fold-search (setq default-case-fold-search 1))

    ;
    ; Default to replace case
    ;
    (setq replace-case 1)

    ;
    ; Do not bother the user about processes left running
    ;
    (setq silently-kill-processes 1)

    ;
    ; Display preferences
    ;
    (setq activity-indicator 1)
    (setq display-end-of-file (setq default-display-end-of-file 1))
    (setq ctlchar-with-^ 1)
    (setq automatic-horizontal-scrolling 1)
    (setq horizontal-scroll-step 8)
    (setq highlight-region (setq default-highlight-region 1))
    (setq syntax-array (setq default-syntax-array 1))
    (setq syntax-colouring (setq default-syntax-colouring 1))

    ;
    ; user interface choices
    ;
    (setq track-eol-on-^N-^P 0)
    (setq expansion-only-completes-unique-choices 1)


    (if
        (~is-ui-style "windows")
        (progn
            ;
            ; set up keyboard handling for a PC style keyboard and mouse
            ;
            (setq input-mode 1)                 ; turn on GUI input style

            ; Ctrl-X will work as documented
            ; after the swap as pc-keyboard
            ; has smart Ctrl-X handling
            (setq swap-ctrl-x-char '\034')
            (execute-mlisp-file "pc-keyboard")  ; PC keyboard handling
            (execute-mlisp-file "pc-mouse")     ; mouse support
            (if (!= operating-system-name "window")
                (bind-to-key "mouse-paste-primary" "\[mouse-2-down]")
            )
        )
    )

    ;
    (if
        (~is-ui-style "windows-extended")
        (progn
            ; New Windows style single control key binding
            ;
            (bind-to-key "visit-file" "\^o")
            (bind-to-key "switch-to-buffer" "\^b")
            (bind-to-key "write-current-file" "\^s")
            (bind-to-key "buffer-dired" "\^l\^b")
            (if (is-function "list-processes")
                (bind-to-key "list-processes" "\^l\^p")
            )
            (bind-to-key "list-databases" "\^l\^d")
            (bind-to-key "redraw-display" "\^l\^l")
            (bind-to-key "incremental-search" "\^f")
            (bind-to-key "reverse-incremental-search" "\^r")

            (bind-to-key "dabbrev" "\^w")
        )
    )
    (if
        (~is-ui-style "ere-searching")
        (progn
            (bind-to-key "ere-search-forward" "\es")
            (bind-to-key "ere-search-reverse" "\er")
            (bind-to-key "ere-query-replace-string" "\eq")
        )
    )
    ;
    ;   Set up minibuffer command recall
    ;
    (execute-mlisp-file "minibuf-recall")

    ;
    ; add ESC bindings for ^X prefix commands that access the region
    ;
    (bind-to-key "exchange-dot-and-mark" "\e\^x"); replace ^X-^X
    (bind-to-key "unset-mark" "\e\0")           ; replace ^X-\0

    ;
    ;   Set up some aditional binding
    ;
    (bind-to-key "tab-key" "\^I")               ; smart tab key
    (bind-to-key "tab-delete" "\e\^I")          ; smart tab delete
    (bind-to-key "tab-delete" "\[shift-tab]")   ; smart tab delete
    (bind-to-key "minibuf" "\e\e")              ; use the minibuffer to eval things
    (bind-to-key "case-capitalize" "\eC")       ; capitalise region or word
    (bind-to-key "case-lower" "\el")            ; lower case region of word
    (bind-to-key "case-upper" "\eu")            ; upper case region of word
    (bind-to-key "case-invert" "\ei")           ; invert case region of word

    (bind-to-key "goto-line" "\eg")             ; goto line

    ;
    ; set up auto executes
    ;
    ; lisp files
    (auto-execute "lisp-mode" "*.ml")
    (auto-execute "lisp-mode" "*.key")
    (auto-execute "lisp-mode" "*.mlp")
    ; C and C++ files
    (auto-execute "electric-c-mode" "*.c")
    (auto-execute "electric-c-mode" "*.cc")
    (auto-execute "electric-c-mode" "*.cpp")
    (auto-execute "electric-c-mode" "*.cxx")
    (auto-execute "electric-c-mode" "*.h")
    (auto-execute "electric-c-mode" "*.hh")
    (auto-execute "electric-c-mode" "*.hpp")
    (auto-execute "electric-c-mode" "*.hxx")
    ; Install Shield files
    (auto-execute "install-shield-mode" "*.rul")
    ; diff and patch files
    (auto-execute "diff-mode" "*.patch")
    (auto-execute "diff-mode" "*.diff")
    ; puppet files
    (auto-execute "puppet-mode" "*.pp")
    ; Python files
    (auto-execute "Python-mode" "*.py")
    ; lua files
    (auto-execute "lua-mode" "*.lua")
    ; Java files
    (auto-execute "Java-mode" "*.java")
    ; LaTex files
    (auto-execute "latex-mode" "*.tex")
    ; SQL files
    (auto-execute "SQL-mode" "*.sql")

    ; JavaScript files
    (auto-execute "JavaScript-mode" "*.js")
    (auto-execute "json-mode" "*.json")
    ; VB Script
    (auto-execute "vbscript-mode" "*.vbs")
    ; Pascal files
    (auto-execute "electric-pascal-mode" "*.pas")
    ; HTML files
    (auto-execute "HTML-mode" "*.htm")
    (auto-execute "HTML-mode" "*.html")
    ; XML files
    (auto-execute "XML-mode" "*.xml")

    ; Make files
    (auto-execute "makefile-mode" "Makefile")
    (auto-execute "makefile-mode" "Makefile.in")
    (auto-execute "makefile-mode" "makefile")
    (auto-execute "makefile-mode" "*.mak")
    (auto-execute "makefile-mode" "*.mk")

    ; Windows NT CMD mode
    (auto-execute "ntcmd-mode" "*.cmd")
    (auto-execute "ntcmd-mode" "*.bat")

    ; Shell mode for sh, bash and ksh
    (auto-execute "sh-mode" "*.sh")
    (auto-execute "sh-mode" "*.ksh")
    (auto-execute "sh-mode" "*.bash")
    (auto-execute "sh-mode" ".profile")
    (error-occurred
        ; ENV is used by sh to name the shell file to use to init subshells
        (auto-execute "sh-mode" (getenv "ENV"))
    )
    (auto-execute "sh-mode" ".bashrc")
    (auto-execute "sh-mode" ".bash_profile")

    ; if none of the auto executes match the filename then run this hook
    (setq auto-execute-hook "smart-auto-execute")

    (setq enter-emacs-hook (setq return-to-emacs-hook "default-user-enter-actions"))
    (setq buffer-choose-name-hook "default-buffer-choose-name")
)

;
; hook procedure to make recent file menu work
;
(defun
    default-buffer-choose-name(~filename)

    ; try to add to the recnet file list,
    ; but don't worry about errors
    (error-occurred (UI-add-to-recent-file-list ~filename))

    ; return the name of the filename ~filename
    (file-format-string "%fa" ~filename)
)

;
; set up enter and exit hooks
;
(defun
    (default-user-enter-actions
        ;
        ; list of things to be done when entering EMACS
        ;
        (if (= terminal-is-terminal 3)  ; graphics terminal
            ; turn on the mouse
            (mouse-on)
        )
    )
)
