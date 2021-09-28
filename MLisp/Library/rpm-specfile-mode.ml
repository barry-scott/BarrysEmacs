;
; RPM-specfile mode
;
(defun
    (RPM-specfile-mode
        (setq mode-string "RPM-specfile")
        (use-syntax-table "RPM-specfile")
        (use-local-map "RPM-specfile-map")
        (use-abbrev-table "RPM-specfile")
        ; defaults for grep
        (error-occurred (setq grep-default-files "*.spec"))
        (novalue)
    )
)

(defun
    (~mode-modify-syntax-table
        ~type
        ~arg
        (setq ~type (arg 1))
        (setq ~arg 2)
        (while (<= ~arg (nargs))
            (modify-syntax-table ~type (arg ~arg))
            (setq ~arg (+ ~arg 1))
        )
    )
)

; see http://ftp.rpm.org/max-rpm/s1-rpm-inside-macros.html for spec file
; details
(defun
    (~RPM-specfile-mode-setup-syntax-table
        (modify-syntax-table "paren" "(" ")")
        (modify-syntax-table "comment" "#" "\n")
        (modify-syntax-table "string,paired" "%{" "}")

        (~mode-modify-syntax-table "keyword-1,case-fold"
            "Name:"
            "version:"
            "Release:"
            "%description"
            "Summary:"
            "License:"
            "Distribution:"
            "Icon:"
            "URL:"
            "Vendor"
            "Group:"
            "Packager:"
            "Provides:"
            "Obsoletes:"
            "Requires:"
            "BuildRequires:"
            "Conflicts:"
            "Epoch:"
            "AutoReqProv:"
            "AutoReq:"
            "AutoProv:"
            "ExcludeArch:"
            "ExclusiveArch:"
            "ExcludeOS:"
            "ExclusiveOS:"
            "Prefix:"
            "BuildRoot:"
            "BuildArch:"
            "NoSource:"
            "NoPatch:"
            "%prep"
            "%build"
            "%install"
            "%check"
            "%clean"
            "%pre"
            "%post"
            "%preun"
            "%postun"
            "%verifyscript"
            "%files"
            "%package"
            "%changelog"
        )
        (~mode-modify-syntax-table "keyword-1,case-fold,ere"
            "Source\\d*:"
            "Patch\\d*:"
        )
        (~mode-modify-syntax-table "keyword-2"
            "%setup"
            "%doc"
            "%config"
            "%attr"
            "%defattr"
            "%define"
            "%ghost"
            "%verify"
            "%docdir"
            "%dir"
            "%if"
            "%ifarch"
            "%ifnarch"
            "%ifos"
            "%ifnos"
            "%else"
            "%endif"
        )
        (~mode-modify-syntax-table "keyword-2,ere"
            "%patch\\d*"
        )
    )
)

(save-window-excursion
    (temp-use-buffer "keymap-hack")
    (define-keymap "RPM-specfile-map")
    (define-keymap "RPM-specfile-ESC-map")
    (use-local-map "RPM-specfile-map")
    (local-bind-to-key "RPM-specfile-ESC-map" "\e")
    (error-occurred (execute-mlisp-file "RPM-specfile-mode.key"))

    (use-syntax-table "RPM-specfile")

    (if (is-bound check-for-white-space-problems)
        (if check-for-white-space-problems
            (add-check-for-white-space-problems-to-syntax-table)
        )
    )
    (if (is-bound check-for-use-of-tabs-problems)
        (if check-for-use-of-tabs-problems
            (add-check-for-use-of-tabs-problems-syntax-table)
        )
    )
    (~RPM-specfile-mode-setup-syntax-table)
    (delete-buffer "keymap-hack")
    (novalue)
)
