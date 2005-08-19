; 
; test the search functions
; 
(defun
    (unittest-search
	(switch-to-buffer "search-test-data")
	(erase-buffer)
	(electric-c-mode)
	(insert-string "Line 1 has some simple text to search for.\n")
	(insert-string "line 2 is some more text.\n")
	(insert-string "line 3 has re chars like + and * in it to confuse things.\n")
	(insert-string "line 4 is for re common     word    stuff.\n")

	(unittest-common-searching "search")
	(unittest-common-searching "re-search")
	(unittest-common-searching "ere-search")

	(unittest-plus-star-searching "search" "+" "*")
	(unittest-plus-star-searching "re-search" "+" "\\*")
	(unittest-plus-star-searching "ere-search" "\\+" "\\*")

	(unittest-common-re-searching "re-search")
	(unittest-common-re-searching "ere-search")
    )
)

(defun
    unittest-common-searching (~type)
    
    (search-test (concat "cs 1 " ~type) (concat ~type "-forward") 0 1 "Line" 1 5)
    (search-test (concat "cs 2 " ~type) (concat ~type "-forward") 1 1 "Line" 1 5)
    (search-test (concat "cs 3 " ~type) (concat ~type "-forward") 0 1 "line" 1 48)
    (search-test (concat "cs 4 " ~type) (concat ~type "-forward") 1 1 "line" 1 5)
    (search-test (concat "cs 5 " ~type) (concat ~type "-forward") 1 1 "simple" 1 23)
    (search-test (concat "cs 6 " ~type) (concat ~type "-forward") 1 1 "fred" 0 0)
    (search-test (concat "cs 7 " ~type) (concat ~type "-reverse") 1 70 "fred" 0 0)
    (search-test (concat "cs 8 " ~type) (concat ~type "-reverse") 1 70 "simple" 1 17)
    (search-test (concat "cs 9 " ~type) (concat ~type "-reverse") 0 70 "Line" 1 1)
    (search-test (concat "cs 10 " ~type) (concat ~type "-reverse") 1 70 "Line" 1 44)
    (search-test (concat "cs 11 " ~type) (concat ~type "-reverse") 0 70 "line" 1 44)
    (search-test (concat "cs 12 " ~type) (concat ~type "-reverse") 1 70 "line" 1 44)

)

(defun
    unittest-plus-star-searching(~type ~plus ~star)
    (search-test (concat "ps 1 " ~type) (concat ~type "-forward") 0 1 ~plus 1 96)
    (search-test (concat "ps 2 " ~type) (concat ~type "-forward") 1 1 ~plus 1 96)
    (search-test (concat "ps 3 " ~type) (concat ~type "-forward") 0 1 ~star 1 102)
    (search-test (concat "ps 4 " ~type) (concat ~type "-forward") 1 1 ~star 1 102)
    
    (search-test (concat "ps 5 " ~type) (concat ~type "-reverse") 0 128 ~plus 1 95)
    (search-test (concat "ps 6 " ~type) (concat ~type "-reverse") 1 128 ~plus 1 95)
    (search-test (concat "ps 7 " ~type) (concat ~type "-reverse") 0 128 ~star 1 101)
    (search-test (concat "ps 8 " ~type) (concat ~type "-reverse") 1 128 ~star 1 101)
)

(defun
    unittest-common-re-searching(~type)
    ;  move to word
    (search-test (concat "cr 1 " ~type) (concat ~type "-forward") 0 154 "\\w" 1 157)
    (search-test (concat "cr 2 " ~type) (concat ~type "-forward") 1 154 "\\w" 1 157)
    ; move to the boundary from inside non-word
    (search-test (concat "cr 3 " ~type) (concat ~type "-forward") 0 154 "\\b" 1 156)
    (search-test (concat "cr 4 " ~type) (concat ~type "-forward") 1 154 "\\b" 1 156)
    ; already at the boundary
    (search-test (concat "cr 5 " ~type) (concat ~type "-forward") 0 156 "\\b" 1 156)
    (search-test (concat "cr 6 " ~type) (concat ~type "-forward") 1 156 "\\b" 1 156)
    ; already at the boundary
    (search-test (concat "cr 7 " ~type) (concat ~type "-forward") 0 160 "\\b" 1 160)
    (search-test (concat "cr 8 " ~type) (concat ~type "-forward") 1 160 "\\b" 1 160)
    ; move to the boundary from inside word
    (search-test (concat "cr 9 " ~type) (concat ~type "-forward") 0 158 "\\b" 1 160)
    (search-test (concat "cr 10 " ~type) (concat ~type "-forward") 1 158 "\\b" 1 160)


    ;  move to word
    (search-test (concat "cr 11 " ~type) (concat ~type "-reverse") 0 162 "\\w" 1 159)
    (search-test (concat "cr 12 " ~type) (concat ~type "-reverse") 1 162 "\\w" 1 159)
    ; move to the boundary from inside non-word
    (search-test (concat "cr 13 " ~type) (concat ~type "-reverse") 0 162 "\\b" 1 160)
    (search-test (concat "cr 14 " ~type) (concat ~type "-reverse") 1 162 "\\b" 1 160)
    ; already at the boundary
    (search-test (concat "cr 15 " ~type) (concat ~type "-reverse") 0 156 "\\b" 1 151)
    (search-test (concat "cr 16 " ~type) (concat ~type "-reverse") 1 156 "\\b" 1 151)
    ; already at the boundary
    (search-test (concat "cr 17 " ~type) (concat ~type "-reverse") 0 160 "\\b" 1 156)
    (search-test (concat "cr 18 " ~type) (concat ~type "-reverse") 1 160 "\\b" 1 156)
    ; move to the boundary from inside word
    (search-test (concat "cr 19 " ~type) (concat ~type "-reverse") 0 158 "\\b" 1 156)
    (search-test (concat "cr 20 " ~type) (concat ~type "-reverse") 1 158 "\\b" 1 156)

    ; test 
    (search-test (concat "cr 21 " ~type) (concat ~type "-forward") 1 1 "re.*o" 1 114)
    (search-test (concat "cr 22 " ~type) (concat ~type "-reverse") 1 171 "re.*o" 1 142)

)

(defun 
    search-test( ~test-name ~search-command ~case-fold ~start-position ~search-string ~must-succeed ~end-position)
    (goto-character ~start-position)
    (setq case-fold-search ~case-fold)
    (if
	(error-occurred
	    (execute-mlisp-line (concat "(" ~search-command " ~search-string)"))
	)
	(if ~must-succeed
	    (log-error ~test-name (concat "search test failed - error when must succeed  -" error-message))
	    (log-info ~test-name)
	)
	(if (! ~must-succeed)
	    (log-error ~test-name "search test failed - no error when must not succeed")
	    (if (= ~end-position (dot))
		(log-info ~test-name)
		(log-error ~test-name (concat "end position error expected " ~end-position " actual " (+ (dot))))
	    )
	)
    )
)
