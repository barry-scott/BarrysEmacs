(defun
    (man-word-in-buffer
        ~word
        ~prefix

        (setq ~prefix prefix-argument-provided)
        (save-excursion
            (if (! (eobp))
                (forward-character))
            (backward-word)
            (set-mark)
            (forward-word)
            (setq ~word (region-to-string))
            (if (ere-looking-at "\\(([1-9][a-z]*)\\)")
                (progn
                    (region-around-match 1)
                    (setq ~word (concat (region-to-string) " " ~word))
                )
            )
        )
        (if ~prefix
             (setq ~word (concat (get-tty-string (concat ": man " ~word " section ")) " " ~word))
        )
        (man ~word)
    )
)
(defun
    man (~word (get-tty-string ": man ([section] topic) "))
    ~overstrike-length
    ~text

    (pop-to-buffer "man")
    (setq mode-string (concat "man " ~word))
    (use-syntax-table "man")
    (use-local-map "man-map")

    (erase-buffer)
    (set-mark)
    (message "man " ~word " fetching text...")(sit-for 0)
    (filter-region (concat "MANWIDTH=80 MAN_KEEP_FORMATTING=1 man " ~word))
    (unset-mark)
    (end-of-file)
    (line-to-top-of-window)
    (message "man " ~word " cleaning up text...")(sit-for 0)
    (beginning-of-file)

    ; Either there are ANSI escapes or back-space overstrikes


    ;
    ; replace ansi escape
    ; 
    (beginning-of-file)
    ; this is a ref to a foot note - remove
    (error-occurred
        (replace-string "\e[24m\e[1m" ""))
    (error-occurred
        (ere-replace-string "\e]8;;[-#a-zA-Z0-9:/.]*\e\\\\" ""))
    (error-occurred
        (replace-string "\e[4m\e[22m" "\e[22m\e[4m"))
    (error-occurred
        (ere-replace-string "( +)\e\\[22m" "\e[22m\\1"))
    (error-occurred
        (while
            1
            (ere-search-forward "\e\\[1m([^\e]*)\e\\[(0|22)m")
            (region-around-match 1)
            (setq ~text (region-to-string))
            (region-around-match 0)
            (erase-region)
            (insert-string ~text)
            (apply-colour-to-region (dot) (- (dot) (length ~text)) 6)
        )
    )
    (beginning-of-file)
    (error-occurred
        (while
            1
            (ere-search-forward "\e\\[4m([^\e]*)\e\\[(0|24)m")
            (region-around-match 1)
            (setq ~text (region-to-string))
            (region-around-match 0)
            (erase-region)
            (insert-string ~text)
            (apply-colour-to-region (dot) (- (dot) (length ~text)) 3)
        )
    )
    (error-occurred
        (beginning-of-file)
        (ere-replace-string "\e\\[\\d+m" "")
    )
    (unset-mark)

    ;
    ; calibrate the overstrike amount
    ;
    (setq ~overstrike-length 2)
    (error-occurred
        pattern
        (re-search-forward "[^_]\^h")
        (setq pattern
            (concat
                (char-to-string (following-char))
                "^\h"
                (char-to-string (following-char))
            )
        )
        (setq ~overstrike-length 2)
        (forward-character)
        (while (looking-at pattern)
            (setq ~overstrike-length (+ ~overstrike-length 6))
            (forward-character 3)
        )
    )
    (beginning-of-file)
    (while (! (error-occurred (search-forward "\^h")))
        (backward-character 2)
        (if
            (if
                (= (following-char) '_')
                (! (looking-at "_\^h_"))
                0
            )
            ; its underlined text
            (progn
                (delete-next-character 2)
                (apply-colour-to-region (dot) (+ (dot) 1) 3)
            )
            (looking-at "O\^h+")
            ; its a bullet mark
            (progn
                (delete-next-character 3)
                (insert-string "o")
                (apply-colour-to-region (dot) (- (dot) 1) 6)
            )
            ; its assumed to be bold text
            (progn
                (delete-next-character ~overstrike-length)
                (apply-colour-to-region (dot) (+ (dot) 1) 6)
            )
        )
    )
    (beginning-of-file)
    (message "")
)

; Set up global stuff.
(save-excursion
    (temp-use-buffer "~man")
    (use-syntax-table "man")
    (modify-syntax-table "word" ".")
    (modify-syntax-table "word" ":")
    (modify-syntax-table "word" "-")
    (modify-syntax-table "word" "_")

    (define-keymap "man-map")
    (define-keymap "man-ESC-map")
    (define-keymap "man-^X-map")
    (use-local-map "man-map")
    (local-bind-to-key "man-ESC-map" "\e")
    (local-bind-to-key "man-^X-map" "\^x")

    (error-occurred
        (execute-mlisp-file "unix-man.key"))
)
(delete-buffer "~man")
(novalue)
