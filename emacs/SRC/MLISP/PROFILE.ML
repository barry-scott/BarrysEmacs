;
; EMACSPROF.ML for EMACS V6.0
;
; This file sets up the default EMACS environment for user who work in an
; uncustomized environment.
; 
; The default key bindings are loaded from EMACS_DEFAULT_KEYBINDINGS and the
; default autoloads are loaded from EMACS_DEFAULT_AUTOLOADS. Both of these
; files are loaded after the MLisp-library database search list is set up so
; users can have their own key bindings
; 
(message "Emacs is initialising...") (sit-for 0)

(declare-global library-access vms-mail-hook)
(declare-buffer-specific logical-tab-size)

;
; Fill in the MLisp-library database
;
(if
    (error-occurred
	(setq
	    library-access
	    (getenv "EMACS$LIBRARY_ACCESS")
	)
    )
    (setq library-access 0)
)
(error-occurred
    (extend-database-search-list 
	"MLisp-library"
	"emacs$library:emacslib"
	(if (= operating-system-name "VMS") library_access 3)
    )
)
(error-occurred
    (extend-database-search-list
	"describe"
	"emacs$library:emacsdesc"
	(if (= operating-system-name "VMS") library_access 3)
    )
)
;
; Load up the EMACS default keys and autoloads
;
(execute-mlisp-file "emacs_control_strings")
(execute-mlisp-file "emacs_default_autoloads")
(execute-mlisp-file "emacs_default_keybindings")

;
; Define the enter-emacs function which loads in a user's
; initialization file
;
(defun
    (enter-emacs-function
	(error-occurred
	    (setq
		library-access
		(getenv "EMACS$LIBRARY_ACCESS")
	    )
	)
	(error-occurred
	    (extend-database-search-list
		"MLisp-library"
		"sys$login:emacslib"
		(if (= operating-system-name "VMS") library_access 3)
	    )
	)
	(error-occurred
	    (extend-database-search-list
		"describe"
		"sys$login:emacsdesc"
		(if (= operating-system-name "VMS") library_access 3)
	    )
	)
	(if (file-exists "sys$login:emacsinit.ml")
	    (execute-mlisp-file "sys$login:emacsinit.ml")
	    (if (error-occurred (execute-mlisp-file "emacsinit"))
		(if (= error-message "Cannot read emacsinit")
		    (execute-mlisp-file "emacs$library:emacsinit.ml")
		    (error-message error-message)
		)
	    )
	)
	(if (= enter-emacs-hook "enter-emacs-function")
	    (setq enter-emacs-hook "")
	    (execute-mlisp-line (concat "(" enter-emacs-hook ")"))
	)
	(message "")
	(novalue)
    )
)

;
; initialize some variables
;

(setq vms-mail-hook "text-mode")
(setq-default logical-tab-size 8)
(setq enter-emacs-hook "enter-emacs-function")
(novalue)
