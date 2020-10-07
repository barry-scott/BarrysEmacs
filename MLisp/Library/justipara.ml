;
; Macro to extract a "paragraph" from the current buffer at dot and re-justify
; it with ragged or smooth right margin.  The effects of the macro are
; reversible.
;
; Jeffrey Arno Barry, PONE::JBARRY, 231-6756, MRO1-2/E68, 9-Feb-83.
; PONE::DV$PONE___USR1:[JBARRY]JUSTIPARA.ML
;
; The biggest problem faced by this macro is determining the bounds of the
; current paragraph.  This macro treats each paragraph as a collection of
; lines, the contents of which determine the bounds.  A line has the following
; form:
;
; <header><words and stuff><linemark>
;
; The <linemark> can be beginning-or-end-of-buffer, lf, cr-lf, vertical tab, or
; form feed.  Linemarks other than lf or cr-lf ALWAYS mark the beginning or
; end of a paragraph.  Other control characters are ignored except as noted.
;
; Type The <header> has one of these forms:  Example of usage:
; ---- ------------------------------------  --------------------------------
;  1   <any printing character><white space> "C Fortran comments"
;  2   <punctuation character>               "!Bliss comments"
;  3   <white space>                         "    Text with some left margin"
;  4   <nil>                                 "Text with no left margin"
;
; The header of this paragraph, for example, is of type 1 and is "; ".  When
; you start the macro it looks at the start of the CURRENT line to determine
; what the header should be, so it's important to have the EMACS dot in the
; right place.  Exception: if the current line is indented 4 or more columns
; than the next line AND the headers are of the "same" type (1/2 with matching
; guard character or 3/4), then the header is taken from the next line, the
; current line is considered to be the start of the paragraph, and the extra
; indent will be reflected in the re-justified paragraph.  The header will be
; included in each line of the re-justified paragraph.
;
; The beginning/end-of-paragraph tests are made AFTER the macro has bypassed
; the header at the beginning of the line.  There needn't be the same header
; on each line of the paragraph.  This macro makes the assumption that you've
; been messing up the paragraph, so it allows lines which have "less" header
; than the model header.  Again, make sure that you start dot on line which
; has the header you want.
;
; The end of the paragraph is bounded (after header removal) by:
; (1) A blank line or line with less than 3 characters.
; (2) A line containing a tab or 4 or more adjacent spaces.
; (3) A line starting with <optional white space><trailing punctuation>, which
;     includes !.?,;:%>)]} when dealing with header types 3 & 4.
;
; The beginning of the paragraph is bounded (after header removal) by:
; (a) Numbers 1 & 3 above.
; (b) Number 2 above EXCEPT if the tab or 4 or more spaces is at the beginning
;     of the line, in which case the line marks the beginning of the paragraph
;     and the extra indent and line are included in the paragraph.
;
; If started on a blank or short line the macro will move to the next line
; and look for the paragraph there.  If it finds another blank line it punts.
; This feature, combined with dot being left at the end of a processed
; paragraph, means that you can process adjacent paragraphs with no extra
; keystrokes.
;
; If the EMACS prefix-argument is provided then it is used as the right
; margin, otherwise the EMACS variable right-margin is used.  If the result is
; less than 24, 24 is used.  The left margin is determined by the header.  If
; you supply a negative prefix-argument, then you get a smooth right margin at
; the expense of eye-jarring gaps (notice my bias).  The macro uses the same
; definition of right margin as EMACS does, ie if you specify a right margin
; of 80, you get at most 79 columns on the line.
;
; Now, if  you were  so foolish  as to  specify a<linemark>
; smooth  right  margin, here  is  what you  get.<linemark>
; Lines       which       only       hold       a<linemark>
; Sort-of-Very-Very-Very-Very-LONG-WORD<linemark>
; Very-Very-Very-Very-Very-Very-Very-Very-Very-Very-LONG-WORD<linemark>
; single  word are  left alone.   Tries  hard  to<linemark>
; spread  fill  spaces around  evenly.   Look  at<linemark>
; this  paragraph  as  an  example  of  a  filled<linemark>
; paragraph  with a  right margin  of 50  (ie  49<linemark>
; printing columns).<linemark>
;
; The characters  .!?   have 2  spaces put  after them,  and :;, get 1 trailing
; space.  If <white space>.?!,:; is seen, then  the white space is deleted.  If
; .?!,:;<not white space> is  seen, the character is ignored.   Multiple spaces
; are reduced to single spaces.  All  other printing characters have no special
; effect.    (This paragraph  is  a more  reasonable  example of  smooth  right
; margin.)
;
; When the macro terminates, dot and mark surround the new paragraph and the
; old paragraph is in the buffer "Old Paragraph", so you can quickly delete
; the new paragraph and get back the old one if you don't like the results.
;

(progn  ;first progn

; First, some warm-up globals and macros...
(declare-global ~jp-guard)

(defun (~jp-get-header gc (progn        ; extract header, assume bol
    (if (looking-at "[!-~][ \t]")       ; hdr form <print char><white>?
        (progn
            (setq gc (following-char))  ; guard char val to return
            (forward-character)         ; skip guard char
            (if (error-occurred (re-search-forward "[^ \t]"))
                (end-of-line)           ; not there.. hmm
                (backward-character))   ; leave dot at first non ws
            gc)                         ; return guard char
        (if (looking-at "[!-/:-@[-`{-~]"); hdr form <punc char>?
            (progn                      ; yes
                (forward-character)     ; skip guard char
                (preceding-char))       ; return guard char
            (progn                      ; else <whitespace> or <nil> header
                (if (error-occurred (re-search-forward "[^ \t]")); find non ws
                    (end-of-line)       ; not there.. hmm
                    (backward-character)); leave dot at first non ws
                -1)))                   ; return no guard char
)))

; Bypass header according ot ~jp-guard.  Skip ws unless guard char is
; missing.  Return true if line is boundary.
(defun (~jp-bypass-header               ; bypass header, assume bol
    (if (> ~jp-guard 0)         ; is there supposed to be a guard?
        (if (= ~jp-guard (following-char)); yes.. does it match?
            (progn                      ; yes
                (forward-character)     ; skip it and ws if any
                (if (error-occurred (re-search-forward "[^ \t]"))
                    1                   ; term if not non-blank
                    (progn
                        (backward-character)
                        (| (~jp-qt)             ; check short/no line
                           (looking-at ".*\t\\|.*    "))))); & tab/4sp embed
            (| (~jp-qt) ;guard missing.. dangerous.. chk short/no line
               (looking-at ".*\t\\|.*    "); & tab/4sp embed
               (looking-at "[ \t]*[]!.?,;:%>)}]"))); & trailing punc & ret test
        (if (error-occurred (re-search-forward "[^ \t]"));no guard.. skip ws
            1                           ; term if not non-blank
            (progn                      ; else do no guard tests
                (backward-character)
                (| (~jp-qt)             ; do short/no line test
                   (looking-at ".*\t\\|.*    "); & tab/4sp embed test
                   (looking-at "[]!.?,;:%>)}]"))))); & trailing punc test
))

; Quicky macro to perform test 1: empty or short line from dot.
(defun (~jp-qt
    (if (eolp)                          ; empty line test
        1
        (if (looking-at "[ \t][ \t]*$") ; blank line test
            1
            (progn                      ; short line test
                (set-mark)
                (end-of-line)
                (exchange-dot-and-mark)
                (< (- (mark) (dot)) 3))))
))

; OK, now for the real thing...
(defun (justipara rtmrg smooth x s grd ind y col indent bopara eopara (progn

; Ini rtmrg and the smooth flag from right-margin and prefix-argument.
    (setq smooth 0)                     ; assume ragged right
    (if prefix-argument-provided
        (if (< prefix-argument 0)       ; have pref arg
            (progn
                (setq smooth 1)         ; whoops.. smooth right
                (setq rtmrg (- 0 prefix-argument)))
            (setq rtmrg prefix-argument))
        (setq rtmrg right-margin))      ; else no pref arg, use right-margin
    (if (< rtmrg 24) (setq rtmrg 24))   ; limit narrowness

; If looking at short or blank line, go to next line so macro can quickly
; process adjacent paragraphs.
    (if (error-occurred (re-search-reverse "\012\\|\013\\|\014")); fancy bol
        (beginning-of-file)
        (forward-character))
    (if (~jp-qt)                        ; short or blank line?
        (next-line))                    ; yes, skip it
    (setq x (dot))                      ; mark start
    (if (|  (= (following-char) 11)
            (= (following-char) 12)
            (~jp-qt))           ; another blank/short line or ff or vt?
        (error-message "More than 1 blank line before paragraph."))

; Determine what the header is from the current and next lines.
    (setq indent -1)
    (setq bopara -1)
    (setq eopara -1)
    (setq ~jp-guard (~jp-get-header)); get guard char of 1st lin
    (setq col (current-column))         ; and its indent too
    (if (! (| (error-occurred
                  (re-search-forward "\012\\|\013\\|\014")); fancy nxt lin
              (= (preceding-char) 11)
              (= (preceding-char) 12)   ; anything at all wrong with next line?
              (~jp-bypass-header)))     ; no.. see if header of next lin ok
        (if (>= (- col (current-column)) 4); ok.. chk relative indents
            (progn                      ; oh goody.. use header from next lin
                (setq indent col)       ; set 1st lin indent
                (setq col (current-column)); set general indent
                (setq bopara x))))      ; mark beginning of para found
    (goto-character x)                  ; back to start point

; Now loop backwards until we find the beginning of the paragraph.
    (setq x 200)                        ; line limit
    (setq y (dot))                      ; mark initial line start
    (while (< bopara 0)                 ; loop til we find bo para
        (progn bol
            (if (<= (setq x (- x 1)) 0) (error-message "Paragraph >200 lines"))
            (if (|  (bobp)              ; if bo buf bound
                    (= (preceding-char) 11); or vt bound
                    (= (preceding-char) 12)); or ff bound
                (setq bopara (dot))     ; then have bo para.. exit loop
                (progn                  ; else gotta do more checks
                    (backward-character); to beg of prev lin
                    (if (error-occurred (re-search-reverse
                                   "\012\\|\013\\|\014")); fancy bol
                        (beginning-of-file)
                        (forward-character))
                    (setq bol (dot))            ; mark line st
                    (if (~jp-bypass-header)     ; bypass header & chk term
                        (progn                  ; got term string in line
                            (goto-character y)  ; nxt lin is 1st lin
                            (setq bopara (dot)))
                        (progn ; is there a suff larg ind to qualify as para?
                            (setq y bol); sav lin st in case go round again
                            (if (>= (- (current-column) col) 4)
                            (progn              ; yes.. have para begin
                                (setq indent (current-column)); save indent
                                (setq bopara bol)); mark start
                            (goto-character bol)); else no begin.. go bol
                    ))  ;end of line test if false progn
                ))  ;end of if bound false progn
    ))   ;end of backward search
    (goto-character bopara)             ; go to bo para
    (message "Beginning of paragraph") (sit-for 5)

; Now loop forwards until we find the end of the paragraph.  While we are
; moving along, transfer the lines minus headers to the "New Paragraph" buffer.
    (set-mark) (copy-region-to-buffer "New Paragraph"); clr new para
    (~jp-bypass-header)         ; bypass h
    (setq x 200)                        ; line limit
    (while (< eopara 0)                 ; loop til we find eo para
        (progn
            (if (<= (setq x (- x 1)) 0) (error-message "Paragraph >200 lines"))
            (set-mark)
            (if (error-occurred (re-search-forward
                                   "\012\\|\013\\|\014")); fancy next-line
                (end-of-file)
                (if (!= (preceding-char) 10)
                    (backward-character)))
            (setq y (dot))              ; mark beg of lin
            (append-region-to-buffer "New Paragraph"); cop prev lin to n p
            (if (|  (eobp)              ; if eo buf bound
                    (= (following-char) 11); or vt bound
                    (= (following-char) 12)); or ff bound
                (setq eopara (dot))     ; then have eo para.. exit loop
                (if (|  (~jp-bypass-header); bypass h and chk
                        (>= (- (current-column) col) 4)); chk new para too
                    (setq eopara y)))   ; term found.. mark eo para
    ))   ;end of forward search
    (goto-character eopara)     ; go to end of para
    (message "End of paragraph.") (sit-for 5)

; OK. Now that all the text is in "New Paragraph", go on over there and
; rejustify it.  This is simple compared to finding the darn paragraph.
    (setq s (current-buffer-name))      ; so can get bak
    (temp-use-buffer "New Paragraph")   ; go
    (beginning-of-file)

    (set-mark)                          ; created std header in prefix-string
    (insert-string "\012")
    (if (>= ~jp-guard 0) (insert-character ~jp-guard))
    (to-col col)
    (setq prefix-string (region-to-string))
    (delete-to-killbuffer)

    (error-occurred (replace-string "\015" " ")); CRs ==> spaces
    (error-occurred (replace-string "\012" " ")); LFs ==> spaces
    (error-occurred (replace-string "\011" " ")); tabs (if any) ==> spaces
    (while (! (error-occurred (search-forward "  "))); zap "  "
        (progn (delete-previous-character) (backward-character)))

    (end-of-file)                       ; " . " ==> ". " and for !?,;: also
    (insert-string " ")
    (beginning-of-file)
    (while (! (error-occurred (re-search-forward
                " \\. \\| ! \\| ? \\| , \\| ; \\| : ")))
        (progn (backward-character) (backward-character)
               (delete-previous-character)))

    (end-of-file) (delete-white-space)  ; ". " ==> ".  " and for !? also
    (beginning-of-file) (delete-white-space)
    (while (! (error-occurred (re-search-forward "\\. \\|! \\|? ")))
        (insert-string " "))            ; ! and ? get extra trailing space

    (end-of-file) (insert-string " ")
    (beginning-of-file)                 ; now insert the 1st line's indent
    (insert-string " \012")
    (if (>= ~jp-guard 0) (insert-character ~jp-guard))
    (if (>= indent 0)
        (to-col indent)
        (to-col col))
    (setq ind (dot))                    ; mark 1st back search lim

    (setq x 4000)                       ; word limit
    (while (! (error-occurred (search-forward " "))) (progn; main justify loop
        (if (<= (setq x (- x 1)) 0)     ; watch limit
            (error-message "More than 4000 words in paragraph"))
        (if (> (current-column) rtmrg)  ; prev "word" exceed margin?
            (progn                      ; yes
                (backward-character)    ; leave this sp alone
                (search-reverse " ")    ; find prev sp or lin
                (forward-character)
                (if (<= (dot) ind)      ; did we hit limit [ie the word
                    (search-forward " ")); is too long for the line]? go eo wd
                (if (! (eobp))          ; if not at eo buf, break the line
                    (progn
                        (delete-previous-character)
                        (if (= (preceding-char) 32); zap "  " after .!?
                            (delete-previous-character))
                        (insert-string prefix-string)
                        (setq ind (dot))        ; set new back search lim
                        (if (= (following-char) 32)
                            (delete-next-character))))))
    ))  ;eo main jus lp

    (beginning-of-file) (delete-next-character); delete guard characters
    (delete-next-character)
    (end-of-file) (delete-previous-character)
    (message "Paragraph justified.") (sit-for 0)

; Ok, now do the filling if requested.
    (if smooth (progn
        (beginning-of-file)
        (setq x 200)                    ; lin lim
        (setq ind "                                "); create long blank string
        (setq ind (concat ind ind ind ind ind ind ind ind))))
    (while smooth (progn                ; lp til done
        (if (<= (setq x (- x 1)) 0)
            (error-message "More than 200 lines in paragraph"))

        (beginning-of-line)             ; find lh end of line after header
        (if (= ~jp-guard (following-char)); do IFF first char matches guard
            (forward-character))        ; bypass guard char
        (if (error-occurred (re-search-forward "[^ \t]")); find non ws
            (end-of-line)               ; not there.. hmm
            (backward-character))       ; leave dot at first non ws
        (set-mark)                      ; mark it

        (end-of-line)
        (if (eobp) (progn (setq smooth 0) (set-mark))); watch for last line
        (setq y (- rtmrg (current-column))); number of spaces needed to fill
        (if (> y 0) (progn nhol         ; do IFF filling to be done
            (setq nhol 0)               ; lp to count holes on line
            (while (& (! (error-occurred (search-reverse " ")))
                      (> (dot) (mark)))
                (setq nhol (+ nhol 1)))
            (goto-character (mark))
            (if (= nhol 0) (progn       ; use eo lin if no holes
                              (end-of-line) (set-mark)))
            (goto-character (mark))     ; go to proper start point on lin
            (if (eolp)                  ; filling at end of line?
                (novalue)               ; yes.. don't bother
                (progn phf rems mod cnt ; not eol.. calc how to spread damage
                    (setq phf (substr ind 1 (/ y nhol))); create per-hole str
                    (setq rems (% y nhol))      ; rem sp after per-hole fills
                    (setq cnt 0)                ; lp cnt
                    (if (<= (* rems 2) nhol)    ; neater to spread sp or dels?
                        (progn                  ; spread spaces
                            (if (= rems 0)
                                (setq mod 100000)
                                (setq mod (/ nhol rems)))
                            (while (< cnt nhol) (progn
                                (search-forward " ")
                                (insert-string phf)
                                (if (& (> rems 0)
                                       (= 0 (% (+ cnt (/ mod 2)) mod)))
                                    (progn
                                        (setq rems (- rems 1))
                                        (insert-character ' ')))
                                (setq cnt (+ cnt 1)))))
                        (progn                  ; spread deletes
                            (setq phf (concat phf " "))
                            (setq rems (- nhol rems))
                            (setq mod (/ nhol rems))
                            (while (< cnt nhol) (progn
                                (search-forward " ")
                                (insert-string phf)
                                (if (& (> rems 0)
                                       (= 0 (% (+ cnt (/ mod 2)) mod)))
                                    (progn
                                        (setq rems (- rems 1))
                                        (delete-previous-character)))
                                (setq cnt (+ cnt 1)))))
                    )  ;end of if spread sp or dels
            ))  ;end of if eolp false progn
        )) ;end of if y>0
        (next-line)
    ))  ;end of while

; Back to old buffer.  Out with the old and in with the new.
    (use-old-buffer s)
    (goto-character bopara)     ; delete (saved) old para
    (set-mark)
    (goto-character eopara)
    (setq x (preceding-char))           ; extra lf flag
    (delete-region-to-buffer "Old Paragraph")

    (yank-buffer "New Paragraph")
    (if (= x 10) (insert-string "\012")); insert lf if old para had one
    (sit-for 0) (message "Done.  ^W<esc>^YOld Paragraph<cr> to undo it.")
)))

; end of first progn
)
