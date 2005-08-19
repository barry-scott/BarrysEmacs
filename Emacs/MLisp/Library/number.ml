;
;	Convert a number in the following format
;		0xhhhhh		- hex
;		0oooooo		- octal
;		ddddddd		- decimal
;	into hex octal and decimal and display in the message line
;
(progn
(defun
    (change-to-hex num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (concat "0x" (hex-to-string num)))
	(novalue)
    )
)

(defun
    (change-to-octal num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (concat "0x" (hex-to-string num)))
	(novalue)
    )
)

(defun
    (change-to-decimal num
	(setq num (string-to-binary (region-to-string)))
	(erase-region)
	(insert-string (+ num))
	(novalue)
    )
)

(defun
    string-to-binary (num)
    
    l
    
    (setq l (length num))
    (if (c= (string-to-char num) '0')
	;then
	(progn
	    (setq l (- l 1))
	    (setq num (substr num 2 l))
	    (if (c= (string-to-char num) 'x')
		;then
		(progn	; hex convertion
		    (setq l (- l 1))
		    (setq num (substr num 2 l))
		    (setq num (string-to-hex num))
		)
		;else
		(setq num (string-to-octal num))
	    )
	    ;else
	    (setq num (+ num))	; convert num in decimal
	)
    )
    num
)
(defun
    (number num l original hex oct
	(setq original (arg 1 "Number to convert: "))
	(setq num original)
	(setq l (length num))
	
	(if (c= (string-to-char num) '0')
	    ;then
	    (progn
		(setq l (- l 1))
		(setq num (substr num 2 l))
		(if (c= (string-to-char num) 'x')
		    ;then
		    (progn	; hex convertion
			(setq l (- l 1))
			(setq num (substr num 2 l))
			(setq num (string-to-hex num))
		    )
		    ;else
		    (setq num (string-to-octal num))
		)
		;else
		(setq num (+ 0 num))	; convert num to decimal
	    )
	)
	(message (concat "Converted " original " to: 0x" (hex-to-string num)
		     " 0" (octal-to-string num)
		     " " num)
	)
	(novalue)
    )
)
;
;	Convert a string into a number using hex
;
(defun
    (string-to-hex
	n			; number
	d			; digit
	s			; string
	(setq s (arg 1 ": string-to-hex "))
	(setq n 0)
	(while (> (length s) 0)
	    (progn
		(setq d (string-to-char s))
;(message (concat "s = " s " d = " (char-to-string d))) (sit-for 10)
		(if
		    (c= d '0')	(setq d 0)
		    (c= d '1')	(setq d 1)
		    (c= d '2')	(setq d 2)
		    (c= d '3')	(setq d 3)
		    (c= d '4')	(setq d 4)
		    (c= d '5')	(setq d 5)
		    (c= d '6')	(setq d 6)
		    (c= d '7')	(setq d 7)
		    (c= d '8')	(setq d 8)
		    (c= d '9')	(setq d 9)
		    (c= d 'a')	(setq d 10)
		    (c= d 'b')	(setq d 11)
		    (c= d 'c')	(setq d 12)
		    (c= d 'd')	(setq d 13)
		    (c= d 'e')	(setq d 14)
		    (c= d 'f')	(setq d 15)
		    (error-message (concat "bad digit " (char-to-string d) " in string-to-hex " s))
		)
;(message (concat "n = " n " d = " d)) (sit-for 10)
		(setq n (+ (* n 16) d))
		(setq s (substr s 2 (- (length s) 1)))
	    )
	)
	n
    )
)
;
;	Convert a string into a number using octal
;
(defun
    (string-to-octal
	n			; number
	d			; digit
	s			; string
	(setq s (arg 1 ": string-to-octal "))
	(setq n 0)
	(while (> (length s) 0)
	    (progn
		(setq d (string-to-char s))
		;(message (concat "s = " s " d = " (char-to-string d))) (sit-for 10)
		(if
		    (c= d '0')	(setq d 0)
		    (c= d '1')	(setq d 1)
		    (c= d '2')	(setq d 2)
		    (c= d '3')	(setq d 3)
		    (c= d '4')	(setq d 4)
		    (c= d '5')	(setq d 5)
		    (c= d '6')	(setq d 6)
		    (c= d '7')	(setq d 7)
		    (error-message (concat "bad digit " (char-to-string d) " in string-to-octal " s))
		)
		;(message (concat "n = " n " d = " d)) (sit-for 10)
		(setq n (+ (* n 8) d))
		(setq s (substr s 2 (- (length s) 1)))
	    )
	)
	n
    )
)
;
;	Convert a number into a string of hex digits
;
(defun
    (hex-to-string
	n
	s
	(setq n (arg 1 ": hex-to-string "))
	(setq s (substr "0123456789abcdef" (+ (% n 16) 1) 1))
	(setq n (/ n 16))
	(while (> n 0)
	    (progn
		(setq s (concat (substr "0123456789abcdef" (+ (% n 16) 1) 1) s))
		(setq n (/ n 16))
	    )
	)
	s
    )
)
;
;	Convert a number into a string of octal digits
;
(defun
    (octal-to-string
	n
	s
	(setq n (arg 1 ": octal-to-string "))
	(setq s (substr "01234567" (+ (% n 8) 1) 1))
	(setq n (/ n 8))
	(while (> n 0)
	    (progn
		(setq s (concat (substr "01234567" (+ (% n 8) 1) 1) s))
		(setq n (/ n 8))
	    )
	)
	s
    )
)
)
