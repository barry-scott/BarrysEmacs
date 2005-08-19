; 
; Barry A. Scott	18-Jun-1983
; Updated to use *defun syntax 13-Nov-1993 Barry A. Scott
; 
; The line-number-offset is used to make goto-line work
; in source files where you know only list line numbers.
; 
; The adjust-line-numbers function will set up this
; variable give the desired line number for the current
; line.
; 
; The page-number-offset is used to make goto-page work
; in source files where you know only list page numbers.
; 
; The adjust-page-numbers function will set up this
; variable give the desired page number for the current
; page.
; 
(declare-buffer-specific line-number-offset page-number-offset)
(setq-default line-number-offset 0)
(setq-default page-number-offset 0)

(defun
    adjust-line-number
    (~adjust-by (get-tty-string ": adjust-line-number (new value) "))
    ~line-no
    
    (setq line-number-offset 0)
    (setq ~line-no (current-line-number))
    
    (setq line-number-offset
	(- ~adjust-by ~line-no)
    )
    (novalue)
)

(defun
    
    ;***************************************************************
    ; goto-line
    ; - moves cursor to beginning of indicated line.
    ; - line number is taken from prefix if provided,
    ;   is prompted for otherwise.
    ; - first line of file is line 1
    goto-line
    (line
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (+ (get-tty-string ": goto-line "))
	)
    )
    (setq line (- line 1 line-number-offset))
    (beginning-of-file)
    (if (> line 0)
	(next-line line)
    )
    (beginning-of-line)
    (novalue)
)

(defun
    ;***************************************************************
    ; goto-percent
    ; - moves cursor past indicated percentage of the buffer.
    ; - percentage is taken from prefix if provided,
    ;   is prompted for otherwise.
    ; - (goto-percent n) goes to the character closest to the
    ;   beginning of the buffer that is reported as n% in the
    ;   status line.  This is 2 characters further into the
    ;   buffer than you'd expect.  As a result, (goto-percent 0)
    ;   goes to character 1 in the file, since I didn't feel like
    ;   fixing that special case.
    ; - (goto-percent 100) goes to the end of the buffer.
    ; - remember that the position of the first character
    ;   in the buffer is 1.
    goto-percent
    (percent
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (+ (get-tty-string ": goto-percent "))
	)
    )
    (goto-character (+ (/ (* (buffer-size) percent) 100) 2))
    (novalue)
    
)    
(defun
    (current-page-number
	~page-number
	(setq ~page-number 1)
	(looking-at "\^l")  	; prime search code
	(save-excursion
	    (while
		(! (error-occurred (re-search-reverse "")))
		(setq ~page-number (+ ~page-number 1))
	    )
	)
	~page-number
    )
)
(defun
    goto-page
    (~page-number
	(if prefix-argument-provided
	    (+ prefix-argument)
	    (- (arg 1 ": goto-page ") 1 page-number-offset)
	)
    )
    (beginning-of-file)
    (looking-at "\^l")
    (error-occurred
	(while ~page-number
	    (re-search-forward "")
	    (setq ~page-number (- ~page-number 1))
	)
    )
    (next-line)
    (beginning-of-line)
    (novalue)
)
