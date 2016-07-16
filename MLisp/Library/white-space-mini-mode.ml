(declare-global
    check-for-white-space-problems)

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
