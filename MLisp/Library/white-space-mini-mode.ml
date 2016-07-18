(declare-global
    check-for-white-space-problems
    check-for-use-of-tabs-problems)
(setq check-for-white-space-problems 0)
(setq check-for-use-of-tabs-problems 0)

(defun
    (add-check-for-white-space-problems-to-syntax-table
        (if check-for-white-space-problems
            (progn
                ; tab after space
                (modify-syntax-table "problem,ere" "  *\t+")
                ; trailing white space
                (modify-syntax-table "problem,ere" " [ \t]*$")
                (modify-syntax-table "problem,ere" "\t[ \t]*$")
            )
        )
    )
)

(defun
    (add-check-for-use-of-tabs-problems-syntax-table
        (if check-for-use-of-tabs-problems
            (progn
                ; tab
                (modify-syntax-table "problem,ere" "\t\t*")
            )
        )
    )
)
