;
;       Convert a number in the following format
;               0xhhhhh         - hex
;               0oooooo         - octal
;               ddddddd         - decimal
;       into hex octal and decimal and display in the message line
;
(progn
(defun
    (change-to-hex ~number
        (setq ~number (string-to-binary (region-to-string)))
        (erase-region)
        (insert-string (concat "0x" (hex-to-string ~number)))
        (novalue)
    )
)

(defun
    (change-to-octal ~number
        (setq ~number (string-to-binary (region-to-string)))
        (erase-region)
        (insert-string (concat "0x" (hex-to-string ~number)))
        (novalue)
    )
)

(defun
    (change-to-decimal ~number
        (setq ~number (string-to-binary (region-to-string)))
        (erase-region)
        (insert-string (+ ~number))
        (novalue)
    )
)

(defun
    string-to-binary (~number)

    ~length

    (setq ~length (length ~number))
    (if (c= (string-to-char ~number) '0')
        ;then
        (progn
            (setq ~length (- ~length 1))
            (setq ~number (substr ~number 2 ~length))
            (if (c= (string-to-char ~number) 'x')
                ;then
                (progn  ; hex convertion
                    (setq ~length (- ~length 1))
                    (setq ~number (substr ~number 2 ~length))
                    (setq ~number (string-to-hex ~number))
                )
                ;else
                (setq ~number (string-to-octal ~number))
            )
            ;else
            (setq ~number (+ ~number))  ; convert ~number in decimal
        )
    )
    ~number
)
(defun
    (number ~number ~length ~original
        (setq ~original (arg 1 "Number to convert: "))
        (setq ~number ~original)
        (setq ~length (length ~number))

        (if (c= (string-to-char ~number) '0')
            ;then
            (progn
                (setq ~length (- ~length 1))
                (setq ~number (substr ~number 2 ~length))
                (if (c= (string-to-char ~number) 'x')
                    ;then
                    (progn      ; hex convertion
                        (setq ~length (- ~length 1))
                        (setq ~number (substr ~number 2 ~length))
                        (setq ~number (string-to-hex ~number))
                    )
                    ;else
                    (setq ~number (string-to-octal ~number))
                )
                ;else
                (setq ~number (+ 0 ~number))    ; convert ~number to decimal
            )
        )
        (message (concat "Converted " ~original " to: 0x" (hex-to-string ~number)
                     " 0" (octal-to-string ~number)
                     " " ~number)
        )
        (novalue)
    )
)
;
;       Convert a string into a number using hex
;
(defun
    (string-to-hex
        ~number ~digit ~string
        (setq ~string (arg 1 ": string-to-hex "))
        (setq ~number 0)
        (while (> (length ~string) 0)
            (progn
                (setq ~digit (string-to-char ~string))
;(message (concat "~string = " ~string " ~digit = " (char-to-string ~digit))) (sit-for 10)
                (if
                    (c= ~digit '0')  (setq ~digit 0)
                    (c= ~digit '1')  (setq ~digit 1)
                    (c= ~digit '2')  (setq ~digit 2)
                    (c= ~digit '3')  (setq ~digit 3)
                    (c= ~digit '4')  (setq ~digit 4)
                    (c= ~digit '5')  (setq ~digit 5)
                    (c= ~digit '6')  (setq ~digit 6)
                    (c= ~digit '7')  (setq ~digit 7)
                    (c= ~digit '8')  (setq ~digit 8)
                    (c= ~digit '9')  (setq ~digit 9)
                    (c= ~digit 'a')  (setq ~digit 10)
                    (c= ~digit 'b')  (setq ~digit 11)
                    (c= ~digit 'c')  (setq ~digit 12)
                    (c= ~digit 'd')  (setq ~digit 13)
                    (c= ~digit 'e')  (setq ~digit 14)
                    (c= ~digit 'f')  (setq ~digit 15)
                    (error-message (concat "bad digit " (char-to-string ~digit) " in string-to-hex " ~string))
                )
;(message (concat "~number = " ~number " ~digit = " ~digit)) (sit-for 10)
                (setq ~number (+ (* ~number 16) ~digit))
                (setq ~string (substr ~string 2 (- (length ~string) 1)))
            )
        )
        ~number
    )
)
;
;       Convert a string into a number using octal
;
(defun
    (string-to-octal
        ~number ~digit ~string
        (setq ~string (arg 1 ": string-to-octal "))
        (setq ~number 0)
        (while (> (length ~string) 0)
            (progn
                (setq ~digit (string-to-char ~string))
                ;(message (concat "~string = " ~string " ~digit = " (char-to-string ~digit))) (sit-for 10)
                (if
                    (c= ~digit '0')  (setq ~digit 0)
                    (c= ~digit '1')  (setq ~digit 1)
                    (c= ~digit '2')  (setq ~digit 2)
                    (c= ~digit '3')  (setq ~digit 3)
                    (c= ~digit '4')  (setq ~digit 4)
                    (c= ~digit '5')  (setq ~digit 5)
                    (c= ~digit '6')  (setq ~digit 6)
                    (c= ~digit '7')  (setq ~digit 7)
                    (error-message (concat "bad digit " (char-to-string ~digit) " in string-to-octal " ~string))
                )
                ;(message (concat "~number = " ~number " ~digit = " ~digit)) (sit-for 10)
                (setq ~number (+ (* ~number 8) ~digit))
                (setq ~string (substr ~string 2 (- (length ~string) 1)))
            )
        )
        ~number
    )
)
;
;       Convert a number into a string of hex digits
;
(defun
    (hex-to-string
        ~number ~string
        (setq ~number (arg 1 ": hex-to-string "))
        (setq ~string (substr "0123456789abcdef" (+ (% ~number 16) 1) 1))
        (setq ~number (/ ~number 16))
        (while (> ~number 0)
            (progn
                (setq ~string (concat (substr "0123456789abcdef" (+ (% ~number 16) 1) 1) ~string))
                (setq ~number (/ ~number 16))
            )
        )
        ~string
    )
)
;
;       Convert a number into a string of octal digits
;
(defun
    (octal-to-string
        ~number ~string
        (setq ~number (arg 1 ": octal-to-string "))
        (setq ~string (substr "01234567" (+ (% ~number 8) 1) 1))
        (setq ~number (/ ~number 8))
        (while (> ~number 0)
            (progn
                (setq ~string (concat (substr "01234567" (+ (% ~number 8) 1) 1) ~string))
                (setq ~number (/ ~number 8))
            )
        )
        ~string
    )
)
)
