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
            (if (ere-looking-at "\\(([1-9][a-z]?)\\)")
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
    man (~word (get-tty-string ": man "))
    ~overstrike-length
    ~text

    (pop-to-buffer "man")
    (erase-buffer)
    (set-mark)
    (message "man " ~word " fetching text...")(sit-for 0)
    (filter-region (concat "MAN_KEEP_FORMATTING=1 man " ~word))
    (unset-mark)
    (message "man " ~word " cleaning up text...")(sit-for 0)
    (beginning-of-file)

    ; Either there are ANSI escapes of back-space overstrikes


    ;
    ; replace ansi escape
    ; 
    (beginning-of-file)
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
            (setq ~overstrike-length (+ ~overstrike-length 3))
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
                (apply-colour-to-region (dot) (+ (dot) 1) 1)
            )
            (looking-at "O\^h+")
            ; its a bullet mark
            (progn
                (delete-next-character 3)
                (insert-string "o")
                (apply-colour-to-region (dot) (- (dot) 1) 2)
            )
            ; its assumed to be bold text
            (progn
                (delete-next-character ~overstrike-length)
                (apply-colour-to-region (dot) (+ (dot) 1) 2)
            )
        )
    )
    (beginning-of-file)
)
