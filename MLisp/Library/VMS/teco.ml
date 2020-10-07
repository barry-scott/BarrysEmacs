; TECO.ML       Emacs routines to execute TECO commands on a buffer
; * Requires that the DCL command MUNG be defined as: *
;$      mung :==  $sys$system:teco mung
;
;       Usage is (teco <teco command>), user is prompted if not provided.
;       The command string may contain "~" instead of <esc>.  (Good for
;       PRO keyboards.)  There is no (yet) to enter "~".
;
;       Current position is left where TECO put it.
;
(defun (teco-command tecostring
  (setq tecostring (arg 1 "Enter TECO command:"))
  (save-excursion
    (temp-use-buffer "Teco")
    (erase-buffer)
    (insert-string
      (concat "ERSYS$INPUT:EWSYS$OUTPUT:P" tecostring "i
emacsteco dot
EX"))
    (beginning-of-file)
    (error-occurred (replace-string "~" ""))
    (write-named-file "sys$login:emacsteco.tmp")
  )
  (save-excursion
    (set-mark)
    (end-of-file)
    (filter-region "MUNG sys$login:emacsteco.tmp")
  )
  (search-forward "
emacsteco dot
")
  (provide-prefix-argument 15 (delete-previous-character))
))
