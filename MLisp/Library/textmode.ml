(progn
;
;       updated:
;                        3-Sep-1990     rewrote to restore cursor location
;       Gim Hom         20-May-1990     rewrote package to handle journaling
;                                       by using a temporary buffer.
;       Gim Hom         15-Nov-1989     rename ~TM functions
;       Gim Hom         26-Jul-1989     Fixed para bug (one cr at bobp)
;       Gim Hom         19-Jan-1988     Fixed minor indent and para bug
;       Gim Hom         13-Nov-1986     Work with international characters.
;                                       Made word syntax independent; put
;                                       two spaces after a period;  fixed
;                                       bug in set-margins
;
;       Gim Hom         24-FEB-1985     Add autowrap (auto-fill) toggle;
;                                       added indent-offset +/-; margin-
;                                       checking; auto-margin seeking;
;                                       corrected justify paragraph
;
;       Nick Emery      30-Mar-1984     Added spell check hooks
;
;       John Harper     3-Nov-83        Allow user-specific initialisation
;
;       John L. Adelus  8-Jul-1983      Removed space processor routine.
;                                       Wrote ~TM-auto-fill auto-fill-hook
;                                       Fixed justify-line to work with
;                                       auto-fill-hook.
;                                       fill-and-justify-paragraph rewritten
;                                       using regions.
;
;       John L. Adelus 24-Jul-1983      Rewrote fill-and-justify-paragraph
;                                       Added fill-and-justify-region
;
;       Barry A. Scott  27-Jul-1983     Added Text-mode-map so that the
;                                       keymap is built once.
;
(defun
    (text-mode
        (use-local-map "Text-mode-map")
        (use-abbrev-table "Text-mode")
        (use-syntax-table "Text-mode")
        (setq buffer-justify-flag 1)
        (setq auto-fill-hook "test-auto-fill")
        (setq autowrap-mode-flag 1)
        (setq mode-line-format default-mode-line-format)
        (setq paragraph-delimiters "\^l\n\\|\n[\t ]*\n")
        (setq indent-offset 0)
        (setq left-margin 1)
        (setq right-margin 76)
        (setq mode-string "Text")
        (error-occurred
            (TM-user-buffer-initialization)
        )
        (~TM-mode)
        (novalue)
    )
    ;
    ; set-margins will automatically set the left and right margins as
    ; as the indent-offset.  The right margin is the  defined as the right
    ; most column in the paragraph for non-justified paragraphs.
    ;
    (set-margins TM-IO end-of-para begin-para-mark
        (save-excursion
            (tm-forward-paragraph)
            (tm-backward-paragraph)
            (if (looking-at "[ \t]*\n")
                (progn
                    (tm-forward-word)
                    (beginning-of-line)
                )
            )
            (setq begin-para-mark (dot))        ; mark the start of para
            (setq TM-IO 1)
            (if (looking-at "[\t ]")
                (progn
                    (re-search-forward "[\t ][\t ]*")
                    (setq TM-IO (current-column))
                )
            )           ; found the indent column
            (tm-forward-paragraph)
            (backward-character)
            (while (! (looking-at "\\w"))
                (backward-character)
            )
            (setq end-of-para (dot))    ; mark the end of the paragraph
            (beginning-of-line)
            (if (looking-at "[\t ]")
                (re-search-forward "[\t ][\t ]*")
            )
            (setq left-margin  (current-column))
            (goto-character begin-para-mark)
            (end-of-line) (setq right-margin (current-column))
            (while (<= (dot) end-of-para)       ; find right most margin
                (next-line) (end-of-line)
                (if (> (current-column) right-margin)
                    (setq right-margin (current-column))
                )
            )
            (setq right-margin (- right-margin 1))
            (setq indent-offset (- TM-IO left-margin))
            (~TM-mode)

        )
    )

    (remove-tabs-in-line  col
        (save-restriction
            (beginning-of-line) (set-mark) (end-of-line) (narrow-region)
            (beginning-of-line)
            (while (! (error-occurred (search-forward "\t")))
                (progn (setq col (current-column))
                    (delete-previous-character)
                    (while (< (current-column) col)
                        (insert-string " "))
                )
            )
            (unset-mark)
            (widen-region)
        )
    )

    (new-compact-white-space first
        (if (& begin-paragraph  (< indent-offset 0)) ; execute for first line
            (progn                                   ;   in the pargraph only.
                (save-restriction
                    (remove-tabs-in-line)
                    (beginning-of-line)
                    (delete-white-space)
                    (set-mark) (end-of-line) (narrow-region)
                    (beginning-of-line)
                    (setq first 1)
                    (while (! (error-occurred (re-search-forward "[ \t]")))
                        (if (! first)
                            (progn
                                (delete-white-space)
                                (if (= (preceding-char) '.')
                                    (insert-string " ")
                                )
;                               (if (provide-prefix-argument -1
;                                       (looking-at extra-space-chars)
;                                   )
;                                   (insert-string " ")
;                               )
                                (insert-string " ")
                            )
                        )
                        (tm-forward-word)
                        (setq first 0)
                    )
                    (unset-mark)
                    (widen-region)
                )
                (end-of-line)
                (delete-white-space)
            )
            (compact-white-space-in-line)
        )
    )
    (test-for-begin-paragraph   ; sets begin-paragraph =1 if at begin-para.
        (setq begin-paragraph 0)
        (save-excursion
            (beginning-of-line)
            (if  (bobp) (setq begin-paragraph 1))
            (previous-line)
            (if  (looking-at "[\t ]*\n")
                (setq begin-paragraph 1))
        )
    )

    (toggle-justify-mode
        (if (= auto-fill-hook "") (setq auto-fill-hook "test-auto-fill"))
        (if buffer-justify-flag
            (setq buffer-justify-flag 0)
            (setq buffer-justify-flag 1)
        )
        (~TM-mode)
        (novalue)
    )

    (toggle-autowrap-mode
        (if (= auto-fill-hook "") (setq auto-fill-hook "test-auto-fill"))
        (if autowrap-mode-flag
            (progn
                (setq autowrap-mode-flag 0)
;               (setq auto-fill-hook "novalue" )
                (message "AUTOWRAP is turned off.")
            )
            (progn
                (setq autowrap-mode-flag 1)
;               (setq auto-fill-hook "test-auto-fill")
                (message "AUTOWRAP is ENABLED.")
            )
        )
    )

    (test-auto-fill
        (if autowrap-mode-flag
            (~TM-auto-fill)
            (novalue)
        )
    )

    (~TM-center-line width
        (save-excursion
            (beginning-of-line)
            (delete-white-space)
            (end-of-line)
            (delete-white-space)
            (setq width (current-column))
            (beginning-of-line)
            (to-col
                (+ left-margin (/ (- (- right-margin left-margin) width) 2)
                )
            )
        )
    )
    (set-indent-offset
        io
        (if (! prefix-argument-provided)
            (setq io (+ (arg 1 "Set indent offset to ") 0))
        )
        (if (>= (+ left-margin io) right-margin)
            (error-message "Indent offset greater than line width! ")
        )
        (if (< (+ left-margin io ) 0)
            (error-message "Left margin + indent offset less than zero! ")
        )
        (setq indent-offset io)
        (~TM-mode)
    )
    (set-left-margin
        lm
        (if (! prefix-argument-provided)
            (setq lm (arg 1 "Set left margin to "))
        )
        (if (= lm "") (setq lm (current-column)))
        (setq lm (+ lm))
        (if (>= lm right-margin)
            (error-message "Left margin must be to the left of Right margin")
        )
        (if (>= lm (- right-margin 10))
            (error-message "Left margin too close to Right margin")
        )
        (setq left-margin lm)
        (~TM-mode)
    )
    (set-right-margin
        rm
        (if (! prefix-argument-provided)
            (setq rm (arg 1 "Set right margin to "))
        )
        (if (= rm "") (setq rm (current-column)))
        (setq rm (+ rm))
        (if (<= rm left-margin)
            (error-message "Right margin must be to the right of Left margin")
        )
        (if (<= rm (+ left-margin 5))
            (error-message "Right margin too close to Left margin")
        )
        (setq right-margin rm)
        (~TM-mode)
    )

    (~TM-mode %this-mode-name %this-mode-string
        (setq %this-mode-string mode-string)
        (save-excursion
            (temp-use-buffer "temp-work-buffer")
            (setq current-buffer-checkpointable 0)
            (setq current-buffer-journalled 0)
            (insert-string %this-mode-string)
            (beginning-of-line)
            (set-mark)
            (if (error-occurred (re-search-forward "[ -,]"))
                (end-of-file)
                (backward-character)
            )
            (setq %this-mode-name (region-to-string))
            (delete-buffer "temp-work-buffer")
        )
;       (message %this-mode-name)
        (setq mode-string
            (concat
                %this-mode-name
                (if buffer-display-margins
                    (concat
                        " L "left-margin
                        (if buffer-justify-flag ", J " ", R ") right-margin
                        (if (!= indent-offset 0) ", I " "")
                        (if (!= indent-offset 0) indent-offset "")
                    )
                    ""
                )
            )
        )
    )


    (justify-mode
        (setq buffer-justify-flag (! buffer-justify-flag))
        (~TM-mode)
        (novalue)
    )
    (compact-white-space-in-line
        (beginning-of-line)
        (delete-white-space)
        (while (! (eolp))
            (progn
                (tm-forward-word)
                (delete-white-space)
                (if (= (preceding-char) '.')
                    (insert-string " ")
                )
;               (if (provide-prefix-argument -1 (looking-at extra-space-chars))
;                   (insert-string " ")
;               )
                (insert-string " ")
            )
        )
        (end-of-line)
        (delete-white-space)
    )
    (fill-line
        (if buffer-justify-flag
            (while (save-excursion
                       (end-of-line)
                       (< (current-column) (+ 1 right-margin))
                   )
                (beginning-of-line)
                (tm-forward-word)
                (if (& (! (eolp)) begin-paragraph)
                    (tm-forward-word)
                )
                (if (eolp)
                    (to-col (+ 1 right-margin))
                )
                (while (&
                           (! (eolp))
                           (save-excursion
                               (end-of-line)
                               (< (current-column) (+ 1 right-margin))
                           )
                       )
                    (insert-string " ")
                    (tm-forward-word)
                )
            )
        )
    )
    (justify-line
        (beginning-of-line)
        (if begin-paragraph
            (progn (to-col (+ left-margin indent-offset)) )
            (to-col left-margin)
        )
        (tm-forward-word)
        (if (> (current-column) right-margin)
            (progn              ; Word is longer than column width
                (beginning-of-line)
                (delete-white-space)
                (goto-character (+ 1 (dot) (- right-margin left-margin)))
                (save-excursion
                    (beginning-of-line)
                    (to-col left-margin)
                )
                (newline)
                (delete-white-space)
                (to-col left-margin)
                (beginning-of-line)
                (backward-character)
                1
            )
            (progn
                (while
                    (& (! (eolp)) (<= (current-column) (+ 1 right-margin)))
                    (tm-forward-word)
                )
                (if (& (eolp) (= (current-column) (+ 1 right-margin)))
                    (progn
                        (if (! ~TM-end-blank)
                            (progn
                                (re-search-reverse "[ \t][ \t]*") ; backward-word
                                (delete-white-space)
                            )
                        )
                        (newline)
                        (delete-white-space)
                        (to-col left-margin)
                        (beginning-of-line)
                        (backward-character)
                        1
                    )
                    (if (> (current-column) (+ 1 right-margin))
                        (progn
                            (re-search-reverse "[ \t][ \t]*") ; backward-word
                            (delete-white-space)
                            (newline)
                            (to-col left-margin)
                            (beginning-of-line)
                            (backward-character)
                            1
                        )
                        (if
                            (&
                                ~TM-end-blank
                                (> (+ 1 (current-column)) right-margin)
                            )
                            (progn
                                (newline)
                                (to-col left-margin)
                                (beginning-of-line)
                                (backward-character)
                                1
                            )
                            0
                        )
                    )
                )
            )
        )
    )
    (~TM-auto-fill
        split
;       (use-syntax-table "Text-mode")
        (end-of-line)
        (if (is-bound TM-spell-line-hook)
            (save-window-excursion
                (backward-character)
                (if (looking-at "\\w")
                    (backward-word)
                )
                (newline-and-backup)
                (execute-mlisp-line
                    (concat
                        "("
                        TM-spell-line-hook
                        ")"
                    )
                )
                (end-of-line)
                (delete-next-character)
            )
        )
        (setq ~TM-end-blank (= (preceding-char) ' '))
        (test-for-begin-paragraph) ; begin-paragrph = 1 if at begin of paraph
        (new-compact-white-space)
        (if (justify-line)
            (progn
                (fill-line)
                (end-of-line)
                (next-line)
                (if
                    (&
                        (! (= (current-column) left-margin))
                        ~TM-end-blank
                    )
                    (insert-string " ")
                )
            )
            (if ~TM-end-blank (insert-string " "))
        )
        (end-of-line)
        (setq ~TM-end-blank 0)
        (novalue)
    )

    (fill-and-justify-paragraph %tm-dot
        (message "Justifying...") (sit-for 0)
        (setq %tm-dot (+ (dot)))        ; store current location
        (save-excursion
            (if (error-occurred (re-search-reverse "\n[\n\^k\^l \t]*\n"))
                (progn
                    (beginning-of-file)
                    (if (looking-at "")  (re-search-forward ""))
                )
                (re-search-forward "")
            )
            (if (looking-at "[\^k\^l \t]*\n") (re-search-forward ""))
            (set-mark)

            ; store relative location relative to start of paragraph
            (setq %tm-dot (- %tm-dot (dot)))

            (tm-forward-paragraph)
            (backward-character)
            (while (looking-at "[\t ]*\n")
;           (while (! (looking-at "\\w"))
                (backward-character)
            )
            (forward-character)
            (if prefix-argument-provided
                (error-occurred (execute-mlisp-line "(spell-region)"))
            )
            (~TM-justify-region)
            (message "Justifying... Done.")
        )
        ; go to original character. If space or tab, the original
        ; dot was probably a space and was deleted.
        (goto-character (+ (dot) %tm-dot -1))
        (if (looking-at "[ \t]") (re-search-forward "[ \t]*"))
    )

    (fill-and-justify-region
        ~TM-end-of-region count
        (if (= (dot) (mark))
            (error-message "No region selected")
        )
        (if prefix-argument-provided
            (error-occurred (execute-mlisp-line "(spell-region)"))
        )
        (message "Justifying region...") (sit-for 0)
        (save-excursion
            (save-restriction
                (narrow-region)
                (if (> (dot) (mark)) (exchange-dot-and-mark))
                (tm-backward-paragraph)
                (setq ~TM-end-of-region 0)
                (while (! ~TM-end-of-region)
                    (set-mark)
                    (tm-forward-paragraph)
                    (if (eobp) (setq ~TM-end-of-region 1))
                    (error-occurred (backward-character))
                    (while (! (looking-at "\\w"))
                        (progn
                            (if (error-occurred (backward-character))
                                (error-message "Region does not contain a paragraph")
                            )
                        )
                    )
                    (forward-character)
                    (~TM-justify-region)
                    (tm-forward-paragraph)
                )
                (message "Justifying region... Done.")
            )
        )
    )

    (~TM-justify-region
        ~TM-right-column ~TM-end-of-paragraph ~TM-last-line-length
        old-rm old-lm old-io old-justify
        (setq old-rm right-margin)
        (setq old-lm left-margin)
        (setq old-io indent-offset)
        (setq old-justify buffer-justify-flag)
        (copy-region-to-buffer "tm-work-buffer")
        (save-excursion
            (temp-use-buffer "tm-work-buffer")
            (setq current-buffer-journalled 0)
            (setq current-buffer-checkpointable 0)
            (setq right-margin old-rm)
            (setq left-margin  old-lm)
            (setq indent-offset old-io)
            (setq buffer-justify-flag old-justify)
            (setq ~TM-end-blank 1)
            (setq ~TM-right-column (- right-margin left-margin))

            ; goto original character. Used error-occured because
            ; this variable in NOT used in fill-and-justify region
            (error-occurred (goto-character (+  %tm-dot 1)))

            (save-excursion
                (beginning-of-file)
                (set-mark)
                (end-of-file)
                (exchange-dot-and-mark)
                (setq ~TM-end-of-paragraph 0)
                (while (! ~TM-end-of-paragraph)
                    (test-for-begin-paragraph)
                    (new-compact-white-space)
                    (setq ~TM-last-line-length (current-column))
                    (while
                        (&
                            (<= (current-column) ~TM-right-column)
                            (! ~TM-end-of-paragraph)
                        )
                        (delete-next-character)
                        (insert-string " ")
                        (new-compact-white-space)
                        (if (= (current-column) ~TM-last-line-length)
                            (setq ~TM-end-of-paragraph 1)
                            (setq ~TM-last-line-length (current-column))
                        )
                    )
                    (test-for-begin-paragraph)
                    (justify-line)
                    (if (! ~TM-end-of-paragraph)
                        (progn
                            (fill-line)
                            (next-line)
                        )
                        (if
                            (& (eobp) (= (current-column) left-margin))
                            (progn
                                (set-mark)
                                (previous-line)
                                (end-of-line)
                                (erase-region)
                            )
                        )
                    )
                )
            )
            ; now store the new dot of the original character
            ; this variable in NOT used in fill-and-justify region
            (error-occurred (setq %tm-dot (+ (dot))))
        )

        (erase-region)
        (yank-buffer "tm-work-buffer")
    )

)
;
;   initialisation for the package
;
    (use-syntax-table "Text-mode")
    (modify-syntax-table "word" "!-~")
;    (declare-global extra-space-chars)
;    (setq extra-space-chars "[.!?]")
    (declare-buffer-specific buffer-justify-flag
                             buffer-display-margins
                             ~TM-end-blank
                             begin-paragraph
                             autowrap-mode-flag
                             indent-offset)
    (setq-default buffer-justify-flag 1)
    (setq-default buffer-display-margins 1)
    (setq auto-fill-hook "test-auto-fill")
    (execute-mlisp-file "paragraph")
    (save-excursion
        (temp-use-buffer "keymap-hack")
        (define-keymap "Text-mode-map")
        (define-keymap "Text-mode-ESC-map")
        (use-local-map "Text-mode-map")
        (execute-mlisp-file "textmode.key")
        (kill-buffer "keymap-hack")
    )
    (error-occurred
        (TM-user-global-initialization)
    )


(defun
    (tm-forward-paragraph
        (beginning-of-line)
        (if (looking-at "[ \t\^l\n]*\n")
            (if (error-occurred (re-search-forward "[ \t\^l\n]*\n"))
                (end-of-file)
            )
            (if (error-occurred (re-search-forward "\n[ \t\^l\n]*\n"))
                (end-of-file)
            )
        )
    )
    (tm-backward-paragraph
        (if (bolp) (previous-line))
        (beginning-of-line)
        (if (looking-at "[ \t\^l]*\n")
            ; looking at white space.  Find a non-white-space character
            (if (error-occurred (re-search-reverse "[^\n\t\^l ]"))
                (beginning-of-file)
            )
        )
        ; find the beginning of white space
        (if (error-occurred (re-search-reverse "\n[\t\^l ]*\n"))
            (beginning-of-file)
        )
        ; goto the a non-white-space character
        (if (! (error-occurred (re-search-forward "[^\n\t\^l ]")))
            (beginning-of-line)
        )
    )

    (tm-forward-word
        (if (error-occurred (re-search-forward "[^\t \n][^\t \n]*"))
;       (if (error-occurred (re-search-forward "[!-~][!-~]*"))
            (end-of-file)
        )
    )
)

)
