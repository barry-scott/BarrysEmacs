; 
; emacs_windows.ml - windows user interface functions
; 
; Barry A. Scott  (c) 1993-1999
; 
(setq UI-filter-file-list 
    (concat
	"All Files (*.*)\n*.*\n"
	"MLisp source code\n*.ml;*.mlp,*.key\n"
	"Text Files\n*.txt\n"
	"C/C++ Source code\n*.c;*.h;*.hh;*.hpp;*.hxx;*.cpp;*.cxx;*.rc;*.def\n"
	"HTML Sources\n*.html;*.htm;*.css\n"
	"Ini Files\n*.ini;*.sys;*.bat;*.btm\n";
    )
)

(defun
    (WIN-save-file-as
	~old-current-directory
	(setq ~old-current-directory (current-directory))

	(setq UI-save-as-file-name current-buffer-file-name)

	(if
	    (UI-file-save-as)
	    (progn
		(write-named-file UI-save-as-file-name)
	    )
	)
	(change-directory ~old-current-directory)
    )
)

(defun
    (WIN-open-file
	~old-current-directory
	(setq ~old-current-directory (current-directory))

	(if
	    (UI-file-open)
	    (progn
		(visit-file UI-open-file-name)
		(setq read-only-buffer UI-open-file-readonly)
	    )
	)
	(change-directory ~old-current-directory)
    )
)

(defun
    (WIN-open-recent-file
	(visit-file UI-open-file-name)
    )
)

(defun
    (WIN-open-selection-file ~file
	(setq ~file (region-to-string))
	(if (> (length ~file) 1024)
	    (error-message "The region is too big to be a reasonable filename")
	)
	(visit-file ~file)
    )
)

(defun
    (WIN-save-file
	(write-current-file)
    )
)

(defun
    (WIN-insert-file
	~old-current-directory
	(setq ~old-current-directory (current-directory))

	(if
	    (UI-file-open)
	    (progn
		(insert-file UI-open-file-name)
	    )
	)
	(change-directory ~old-current-directory)
    )
)

(defun
    (WIN-save-environment
	(message "Saving Environment...") (sit-for 0)
	(save-environment "")
	(message "Environment saved")
    )
)

(defun
    (WIN-revert-to-saved
	(if (!= current-buffer-type "file")
	    (error-message "This is not a file buffer")
	)
	(if buffer-is-modified
	    (if
		(= (get-tty-string "Buffer is modified. Revert to saved (ny)[n]: ") "y")
		(read-file current-buffer-file-name)
	    )
	    (read-file current-buffer-file-name)
	    
	)
    )
)
(defun
    (WIN-save-all-files
	(write-modified-files)
    )
)
(defun
    (WIN-edit-cut
	(UI-edit-copy)
	(erase-region)
	(unset-mark)
    )
)

(defun
    (WIN-edit-copy
	(UI-edit-copy)
	(unset-mark)
    )
)

(defun
    (WIN-edit-paste
	(UI-edit-paste)
    )
)

(defun
    (WIN-edit-select-all
	(beginning-of-file)
	(set-mark 1)
	(end-of-file)
    )
)

(defun
    (WIN-find-string
	(setq UI-search-string search-string)
	(UI-find)                            
	(novalue)
    )
)

(defun
    (WIN-find-and-replace
	(setq UI-search-string search-string)
	(UI-find-and-replace)
	(novalue)
    )
)

;
;	drag and drop support function.
;	Do a visit-file unless the user is in the minibuffer
;
(defun UI-drop-file(~file)
    (if (= current-buffer-name "Minibuf")
	(insert-string ~file)
	(visit-file ~file)
    )
)

;
;	DDE support for file opening.
;	This requires that Emacs is registered with windows
;
(defun UI-DDE-command (command)
    ~cmd ~arg1
    (save-excursion
	(temp-use-buffer "~DDE-hack")
	(erase-buffer)
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(insert-string command)
	(beginning-of-file)
	(if (looking-at "[[]*open(\"\\(.*\\)\")")
	    (progn
		(setq ~cmd "visit-file")
		(region-around-match 1)
		(setq ~arg1 (region-to-string))
	    )
	    (message "Unknown DDE command " command)
	)
    )
    (if (= ~cmd "visit-file")
	(visit-file ~arg1)
    )
)

(defun
    WIN-help-command(~file (get-tty-file "Windows Help: "))
    (UI-windows-help ~file "contents" "")
)

(defun
    WIN-HTML-command(~file (get-tty-file "HTML file: "))
    (setq ~file (expand-file-name ~file))
    (UI-windows-shell-execute "open" ~file "" "." "normal")
)
(defun
    (WIN-delete-buffer
	(delete-buffer (current-buffer-name))
    )
)
(defun
    (WIN-execute-line
	~command
	(beginning-of-line)
	(set-mark)
	(end-of-line)
	(setq ~command (region-to-string))
	(erase-region)
	(if (eolp)
	    (delete-next-character)
	)
	(set-mark)
	(filter-region ~command)
    )
)

(defun
    (UI-switch-buffer
	(switch-to-buffer (UI-select-buffer "Emacs Switch to Buffer"))
    )
)

(defun
    (UI-pop-to-buffer
	(pop-to-buffer (UI-select-buffer "Emacs Pop to Buffer"))
    )
)


; 
; load up the key bindings for the menu system
; 
(execute-mlisp-file "emacs_windows.key")
