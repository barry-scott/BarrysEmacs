;							-*-mlisp-*-
;
; Page oriented commands using virtual buffer bounds
; See end of this file for default
; key bindings.  Defining a function called ~page-load-hook supresses
; these defaults.
; 
; The string value of global variable <FF> defines a page boundary.
; Default is a line beginning with ^L (ascii 12.).  Beginning and
; and of buffer always act like page boundaries.
;
; A. Witkin 6/81
; Modified 2/82 by D. Oran to use multi-autoload (see misc.ml)
(progn

(declare-global <FF>)			       ; It works to redefine this.
(setq <FF> "^\^l")

(defun

; next pagemark, or eob.  Returns 1 if search succeeds, else 0.
(next-ff
  (if (error-occurred (re-search-forward <FF>))
      (progn (end-of-file) 0) 1))


; previous pagemark, or bob. Returns 1 if search succeeds, else 0.
(previous-ff
  (if (error-occurred (re-search-reverse <FF>))
      (progn (beginning-of-file) 0) 1))


;put mark just AFTER previous pagemark, dot just BEFORE next.  i.e. page
;marks aren't part of the region.
(mark-page
  (if (previous-ff) (re-search-forward "")) (set-mark)
  (if (next-ff) (re-search-reverse "")))


; place virtual bounds around page, excluding pagemarks, minimizing
; redisplay.
(narrow-bounds-to-page
  dot tdot zdot
  (error-occurred (widen-bounds))
  (setq dot (dot))
  (setq tdot (save-excursion (beginning-of-window)(dot)))
  (mark-page)
  (setq zdot (- (~mark) 1))
  (narrow-bounds-to-region)
  (goto-character (- tdot zdot))
  (line-to-top-of-window)
  (goto-character (- dot zdot)))
  
;jump to next page, narrow bounds
(Next-page
  (error-occurred (~widen-bounds))
  (next-ff)
  (narrow-bounds-to-page)
  (beginning-of-file))

; Insert a page mark at dot, and narrow bounds to the second
; of the new pages thus created.
(Cut-page
  (widen-bounds)
  (insert-string <FF>)
  (narrow-bounds-to-page))

; Join current page with next one, leave bounds narrowed to the combined
; page
(Join-page
  (end-of-file)
  (widen-bounds)
  (if (next-ff)
      (progn (set-mark)(search-reverse "")
	     (delete-to-killbuffer)))
  (narrow-bounds-to-page))

;jump to previous page, narrow bounds
(Previous-page
  (error-occurred (~widen-bounds))
  (previous-ff)
  (narrow-bounds-to-page)
  (beginning-of-file))

;prompts for page # (1-indexed), and goes there with narrowed bounds.
(goto-page
  page# i
  (setq i 1)
  (setq page# (get-tty-string "Page #: "))
  (error-occurred (~widen-bounds))
  (beginning-of-file)
  (while (< i page#)
	 (next-ff)
	 (setq i (+ 1 i)))
  (narrow-bounds-to-page)
  (beginning-of-file))


; place a listing of page number and 1st non-blank line in buffer Help.
(~generate-page-directory
  dir i dot mark prev-ff-dot ff-dot nb-dot was-narrow dotn markn tdotn
  (setq dotn (dot))			       ; save these guys
  (setq markn (~mark))			       ; to preserve window state.
  (setq tdotn (save-excursion (beginning-of-window) (dot)))
  (setq was-narrow (! (error-occurred (~widen-bounds))))
  (setq dir "")				       ; this string holds directory
  (save-excursion
    (temp-use-buffer "Help")
    (erase-buffer)
    (insert-string "Page	First non-blank line")(newline)
    (insert-string "----	--------------------")(newline)(newline))
  (setq dot (dot))(setq mark (~mark))
  (setq i 1)				       ; page # count
  (beginning-of-file)
  (while (>= ff-dot 0)
	 (setq prev-ff-dot ff-dot)
	 (setq ff-dot (save-excursion (if (next-ff) (dot) -1)))
	 (setq nb-dot (save-excursion (forward-word) (dot)))
	 (if (| (< ff-dot 0)(> ff-dot nb-dot))
	     (progn (goto-character nb-dot)
		    (beginning-of-line)
		    (if (< (dot) prev-ff-dot)
			(goto-character prev-ff-dot))
		    (set-mark)
		    (end-of-line)
		    (setq dir 
			  (concat i 
				  "	"
				  (region-to-string))))
		    (setq dir (concat dir i (char-to-string 10))))
	 (save-excursion
	   (temp-use-buffer "Help")
	   (end-of-file)
	   (insert-string dir)(newline))
	 (setq i (+ 1 i))
	 (if (> ff-dot 0)(goto-character ff-dot)))
  (goto-character mark)(set-mark)
  (goto-character dot)
  (if was-narrow
      (progn
	(narrow-bounds-to-region)
	(goto-character markn)(set-mark)
	(goto-character tdotn)(line-to-top-of-window)
	(goto-character dotn))))

; view page directory using *more*
(view-page-directory
  wll
  (~generate-page-directory)
  (split-current-window)
  (switch-to-buffer "Help")
  (setq wll wrap-long-lines)(setq wrap-long-lines 0)
  (beginning-of-file)
  (*more*)
  (setq wrap-long-lines wll)
  (delete-window)
  (novalue))

; insert page directory listing at dot.
(insert-page-directory
  d
  (setq d (dot))
  (~generate-page-directory)
  (yank-buffer "Help")
  (goto-character d)
  (novalue))

(~mark m (if (error-occurred (setq m (mark))) 0 m)); error-proof (mark)

)

; these are the default key bindings.  If you want to supress them,
; define a function called ~page-load-hook before loading page.
(if (error-occurred (~page-load-hook))
    (progn
      ; view-page-directory and insert-page directory
      ; are not bound by default.
      (bind-to-key "goto-page" "\^x")
      (bind-to-key "Next-page" "\^x]")
      (bind-to-key "Previous-page" "\^x[")
      (bind-to-key "Cut-page" "\^xc")
      (bind-to-key "Join-page" "\^xj")
      (bind-to-key "widen-bounds" "\^xw")
      (bind-to-key "narrow-bounds-to-page" "\^x\^l"))
      (bind-to-key "narrow-bounds-to-region" "\^xq")))

)
