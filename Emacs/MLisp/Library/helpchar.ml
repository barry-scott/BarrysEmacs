(define-keymap "~helpchar-prefix")
(define-keymap "~helpchar-view-prefix")
(defun
    (~helpchar-prompt
	(delete-other-windows)
	(split-current-window)
	(pop-to-buffer "Minibuf")
	(previous-window)
	(switch-to-buffer "Help prompt")
	(while (! (error-occurred (shrink-window))) 0)
	(setq current-buffer-checkpointable 0)
	(setq current-buffer-journalled 0)
	(setq mode-line-format "")
	(erase-buffer)
	(insert-string (arg 1))
	(previous-window)
    )
)
(defun
    (~helpchar-more
	(if (save-window-excursion
		(end-of-file)
		(dot-is-visible)
	    )
	    "- End -"
	    "More..."
	)
    )
)
(defun
    (~helpchar-next-page
	(next-page)
	(setq mode-string (~helpchar-more))
    )
)
(defun
    (~helpchar-previous-page
	(previous-page)
	(setq mode-string (~helpchar-more))
    )
)
(defun
    (~helpchar-beginning
	(beginning-of-file)
	(setq mode-string (~helpchar-more))
    )
)
(defun
    (~helpchar-end
	(end-of-file)
	(setq mode-string (~helpchar-more))
    )
)
(defun
    (help
	(help-key)
    )
)
(defun
    (help-key
	(message "Help category (? for help):")
	(~helpchar-prefix)
    )
)
(save-excursion
    ~helpchar-variable
    (temp-use-buffer "Help h window")
    (use-local-map "~helpchar-view-prefix")
    (setq current-buffer-checkpointable 0)
    (setq current-buffer-journalled 0)
    (setq mode-line-format "")
    (temp-use-buffer "Help window")
    (use-local-map "~helpchar-view-prefix")
    (setq current-buffer-checkpointable 0)
    (setq current-buffer-journalled 0)
    (setq mode-line-format "                                                                  %10m")
    (setq ~helpchar-variable 0)
    (while (< ~helpchar-variable 128)
	(local-bind-to-key "illegal-operation" ~helpchar-variable)
	(setq ~helpchar-variable (+ ~helpchar-variable 1))
    )
    (local-bind-to-key "~helpchar-next-page" " ")
    (local-bind-to-key "~helpchar-next-page" "\(next screen)")
    (local-bind-to-key "~helpchar-previous-page" "\^H")
    (local-bind-to-key "~helpchar-previous-page" "\(prev screen)")
    (local-bind-to-key "exit-emacs" "\^C")
    (local-bind-to-key "exit-emacs" "\(f10)")
    (local-bind-to-key "exit-emacs" "q")
    (local-bind-to-key "exit-emacs" "^]")
    (autoload "~helpchar-view-help" "helpchar_view_help")
    (local-bind-to-key "~helpchar-view-help" "?")
    (local-bind-to-key "~helpchar-view-help" "\^_")
    (local-bind-to-key "~helpchar-view-help" "\(help)")
    (local-bind-to-key "~helpchar-beginning" "b")
    (local-bind-to-key "~helpchar-beginning" "\(up)")
    (local-bind-to-key "~helpchar-end" "e")
    (local-bind-to-key "~helpchar-end" "\(down)")
    (use-local-map "~helpchar-prefix")
    (autoload "~helpchar-help" "helpchar_help")
    (local-bind-to-key "~helpchar-help" "?")
    (local-bind-to-key "~helpchar-help" "\^_")
    (local-bind-to-key "~helpchar-help" "\(help)")
    (autoload "~helpchar-describe" "helpchar_describe")
    (local-bind-to-key "~helpchar-describe" "d")
    (autoload "~helpchar-describe-bindings" "helpchar_describe_bindings")
    (local-bind-to-key "~helpchar-describe-bindings" "b")
    (autoload "~helpchar-variable" "helpchar_variable")
    (autoload "~helpchar-compose" "helpchar_compose")
    (local-bind-to-key "~helpchar-compose" "c")
    (local-bind-to-key "~helpchar-variable" "v")
    (autoload "~helpchar-apropos" "helpchar_apropos")
    (local-bind-to-key "~helpchar-apropos" "a")

    (autoload "~helpchar-describe-key" "helpchar_describe_key")
    (local-bind-to-key "~helpchar-describe-key" "k")
    (autoload "~helpchar-where-is" "helpchar_where_is")
    (local-bind-to-key "~helpchar-where-is" "w")


; 
;   Comment out info, vms help and news as they are not used
; 

;    (autoload "~helpchar-info" "helpchar_info")
;    (local-bind-to-key "~helpchar-info" "i")
;    (autoload "~helpchar-news" "helpchar_news")
;    (local-bind-to-key "~helpchar-news" "n")
;    (if (! (is-bound ~info-is-init))
;	(autoload "info" "info")
;    )
;    (autoload "~helpchar-help-i" "helpchar_help_i")
;    (local-bind-to-key "~helpchar-help-i" "h")

)
