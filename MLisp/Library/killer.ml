;       killer.ml       An Emacs kill ring package

;       Requires Emacs V4.1 or later

;       Written  11-Oct-1984 by J.A.Lomicka (REGINA::LOMICKAJ)
;       Inspired by a request from monsur::gaede
;       Last edit 23-July-1986 by J.A.Lomicka

;       This package provides the ability to keep a history of killed
;       objects, and to provide the facility of defining a killed object
;       as any combination of successive kill commands.  The highlighting
;       of the select region is controlled as well, turning highlighting on
;       when mark is set, and off when the region is killed.  These kill-ring
;       commands should be usable within keyboard macros.

;       To load this package, (load "killer") in your emacsinit.ml (either
;       the default, or a modified version).  Be sure killer is in your
;       emacs search path.  If you wish a ring of some other size than 10,
;       you may modify kr-max-buf in KILLER.KEY and set it to some other number
;       before loading killer.

;       If you wish to change the bindings, you should edit killer.KEY only.

;       It works in the following way:

;               set-mark (with highlighting)    M-.
;               toggle-highlighting on or off   ^X-.

;       Killing functions:      Default binding:

;               forward-word    M-d
;               backward-word   M-delete
;               kill-region     ^W
;               forward-line    ^K
;               backward-line   M-k

;       These functions all kill some text, and put the killed text into a
;       new kill buffer.  When any of these is followed by any other of
;       these, or with kill forward-char (^D) or kill backward-char (delete),
;       with no intervening keystrokes, the additional killed text is appended
;       to the correct end of the same buffer.  Both positive and negative
;       prefix arguments may be used.

;       Yank function:

;               yank-killbuffer ^Y
;               yank-more       M-Y (only following ^Y)

;       This function yanks the current kill-buffer, leaving the region
;       around the yanked data.  Successive M-Y following ^Y with no
;       intervening keystrokes will replace the yanked data with the
;       contents of other kill buffers.  If you yanked in the wrong place,
;       ^W or (erase-region) will happily get rid of it.  Prefix argument
;       on yank-killbuffer yanks repeatedly, region around the whole set.
;       Prefix argument on yank-more is equivalent to multiple yank-mores,
;       but is faster.

(progn (message "loading killer.ml")(sit-for 0)
;
;       If the .KEY file is not already loaded, load it now.

(if (! (is-bound kr-max-buf)) (load "killer.key"))

(defun kr-toggle-highlight()
  (setq highlight-region (! highlight-region))
)

(defun kr-set-mark()
  (setq highlight-region 1)
  (set-mark)
)

(defun kr-setup()       ; Turn on kill ring feature
        key
  (declare-global
    kr-current-buf      ; Used to keep track of our place in the ring
    kr-current-thing    ; Used to check for submode of killing mode
  )

;       Create the kill buffers

  (setq kr-current-buf 0)
  (save-excursion
    (while (<= kr-current-buf kr-max-buf)
      (temp-use-buffer (concat "-kr" kr-current-buf))
      (setq current-buffer-checkpointable 0)
      (setq current-buffer-journalled 0)
      (setq kr-current-buf (+ 1 kr-current-buf))
    )
  )
  (setq kr-current-buf 0)

;       Set up kill mode bindings

  (define-keymap "kr-inner-keymap")
  (use-global-map "kr-inner-keymap")
  (kr-more-bindings)
  (use-global-map "default-global-keymap")
)

(defun kr-enter()       ; Enter killing mode
    kr-imitate-pa;      Prefix argument for subkills
    kr-imitate-pap;     Prefix argument provided for subkills
  (kr-prefix-off)
  (while (process-key (kr-inner-keymap))
    (if kr-imitate-pap (return-prefix-argument kr-imitate-pa))
  )
  (setq highlight-region 0)
  (push-back-character last-keys-struck)
  (if kr-imitate-pap (return-prefix-argument kr-imitate-pa))
)

(defun kr-arg-prefix()
  (setq kr-imitate-pa 4);       Initiate null argument string
  (setq kr-imitate-pap 2);      Set argument provided
  (kr-prefix-on)
)

(defun kr-meta-digit()
  (setq kr-imitate-pa (char-to-string (last-key-struck)))
  (setq kr-imitate-pap 1);      Set argument provided
  (kr-prefix-on)
)

(defun kr-meta-minus()
  (setq kr-imitate-pa "-")
  (setq kr-imitate-pap 1);      Set argument provided
  (kr-prefix-on)
)

(defun kr-digit();      Concat another digit
  (if (= kr-imitate-pap 2)
    (progn; Null prefix argument, override default of 4
      (setq kr-imitate-pap 1)
      (setq kr-imitate-pa "")
    )
  )
  (setq kr-imitate-pa (concat kr-imitate-pa (char-to-string (last-key-struck))))
)

(defun kr-minus();      Negate
  (if (= kr-imitate-pap 2)
    (progn
      (setq kr-imitate-pa "-")
      (setq kr-imitate-pap 1)
    )
  ; else this is an illegal minus
    (progn
      (use-global-map "kr-inner-keymap")
      (remove-binding "-")
      (use-global-map "default-global-keymap")
      (push-back-character "-")
    )
  )
)

(defun kr-prefix-on()
  (use-global-map "kr-inner-keymap")
  (bind-to-key "kr-digit" "0")
  (bind-to-key "kr-digit" "1")
  (bind-to-key "kr-digit" "2")
  (bind-to-key "kr-digit" "3")
  (bind-to-key "kr-digit" "4")
  (bind-to-key "kr-digit" "5")
  (bind-to-key "kr-digit" "6")
  (bind-to-key "kr-digit" "7")
  (bind-to-key "kr-digit" "8")
  (bind-to-key "kr-digit" "9")
  (bind-to-key "kr-minus" "-")
  (use-global-map "default-global-keymap")
)

(defun kr-prefix-off()
  (setq kr-imitate-pa 1)
  (setq kr-imitate-pap 0)
  (use-global-map "kr-inner-keymap")
  (if (= (global-binding-of "0") "kr-digit")
    (progn
      (remove-binding "0")
      (remove-binding "1")
      (remove-binding "2")
      (remove-binding "3")
      (remove-binding "4")
      (remove-binding "5")
      (remove-binding "6")
      (remove-binding "7")
      (remove-binding "8")
      (remove-binding "9")
    )
  )
  (if (= (global-binding-of "0") "kr-minus")
    (remove-binding "-")
  )
  (use-global-map "default-global-keymap")
)

(defun kr-kill-thing( kr-pp kr-pa thing) ; Internal use only, enter killing mode
;       The text in "thing" should be MLisp code that will mark out
;       a region that should start the next kill buffer.
  (setq highlight-region 0)
  (setq kr-current-thing thing)
  (setq kr-current-buf
    (if (>= kr-current-buf kr-max-buf) 0 (+ kr-current-buf 1))
  )
  (save-excursion
    (undo-boundary)
    (execute-mlisp-line (concat "(progn " thing ")"))
    (copy-region-to-buffer (concat "-kr" kr-current-buf))
    (erase-region)
  )
  (kr-enter)
)

(defun kr-more-thing( kr-pp kr-pa thing) ; Interal use only, continue killing
;       The text in "thing" should be MLisp code that will mark out
;       a region that should be appended to the current kill buffer.
;       kr-pp is prefix-argument-provided.
;       kr-pa is prefix-argument.  May be -1 with kr-pp false.
  (kr-prefix-off)
  (if (!= "yanking" kr-current-thing)   ; Check submode
    (save-excursion
      (undo-boundary)
      (execute-mlisp-line (concat "(progn " thing ")"))
      (if (>= kr-pa 0)
        (append-region-to-buffer (concat "-kr" kr-current-buf))
        (prepend-region-to-buffer (concat "-kr" kr-current-buf))
      )
      (erase-region)
    )
  ;else change to new killing submode
    (kr-kill-thing kr-pp kr-pa thing)
  )
)

(defun kr-yank()        ; Start yanking
  (setq highlight-region 1)
  (setq kr-current-thing "yanking")
  (undo-boundary)
  (set-mark)
  (prefix-argument-loop (yank-buffer (concat "-kr" kr-current-buf)))
  (kr-enter)
)

(defun kr-more-yank()   ; Continue yanking
  (kr-prefix-off)
  (if (= "yanking" kr-current-thing)    ; Check submode
    (progn      ; Yank next thing
      (undo-boundary)
      (erase-region)
      (setq kr-current-buf
        (prefix-argument-loop
          (if (<= kr-current-buf 0) kr-max-buf (- kr-current-buf 1))
        )
      )
      (error-occurred (yank-buffer (concat "-kr" kr-current-buf)))
    )
  ; else we start yanking
    (kr-yank)
  )
)

(defun kr-line( pp pa)
  (set-mark)
  (if pp
    (progn
      (if (!= pa 0) (beginning-of-line))
      (if (< pa 0) (setq pa (+ pa 1)))
      (provide-prefix-argument pa (next-line))
    )
  ; else no prefix argument, treat terminator seperately
    (if (>= pa 0)
      (if (eolp) (forward-character)(end-of-line))
      (if (bolp) (backward-character)(beginning-of-line))
    )
  )
)

(defun kr-word( pp pa)
  (set-mark)
  (if (>= pa 0)
    (provide-prefix-argument pa (forward-word))

    (provide-prefix-argument (- pa) (backward-word))  )
)

(defun kr-char( pp pa)
  (set-mark)
  (provide-prefix-argument pa (forward-character))
)

(defun kr-kill-forward-word()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-more-forward-word()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-word()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-more-backward-word()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-word kr-pp kr-pa)"
  )
)

(defun kr-kill-region() (kr-kill-thing 1 1 ""))
(defun kr-more-region() (kr-more-thing 1 1 ""))

(defun kr-kill-forward-character()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-more-forward-character()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-character()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-more-backward-character()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-char kr-pp kr-pa)"
  )
)

(defun kr-kill-forward-line()
  (kr-kill-thing prefix-argument-provided prefix-argument
    "(kr-line kr-pp kr-pa)"
  )
)

(defun kr-more-forward-line()
  (kr-more-thing prefix-argument-provided prefix-argument
    "(kr-line kr-pp kr-pa)"
  )
)

(defun kr-kill-backward-line()
  (kr-kill-thing prefix-argument-provided (- prefix-argument)
    "(kr-line kr-pp kr-pa)"
  )
)
(defun kr-more-backward-line()
  (kr-more-thing prefix-argument-provided (- prefix-argument)
    "(kr-line kr-pp kr-pa)"
   )
)

(defun kr-copy-region()
  (setq kr-current-buf
    (if (>= kr-current-buf kr-max-buf) 0 (+ kr-current-buf 1))
  )
  (setq highlight-region 0)
  (copy-region-to-buffer (concat "-kr" kr-current-buf))
  (message "Region copied.")
)

(kr-setup)
(message "loaded  killer.ml")
)
