(if (& (= terminal-is-terminal 3) (= operating-system-name "unix"))
    (progn
	(execute-mlisp-file "emacs_motif.key")
	(if (file-exists "sys$login:emacs_motif_settings.ml")
	    (execute-mlisp-file "sys$login:emacs_motif_settings.ml")
	    (error-occurred (execute-mlisp-file "emacs_motif_settings"))
	)
    )
)
(if (= operating-system-name "Windows")
    (progn
	(execute-mlisp-file "emacs_windows.ml")
	(if (file-exists "sys$login:emacs_windows_settings.ml")
	    (execute-mlisp-file "sys$login:emacs_windows_settings.ml")
	    (error-occurred (execute-mlisp-file "emacs_windows_settings"))
	)
    )
)

