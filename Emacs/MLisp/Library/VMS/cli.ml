(progn
(external-function "cli_define_table" "emacs$share:emacs_cli_shr" "emacs$cli_define_table")
(external-function "cli_list_tables" "emacs$share:emacs_cli_shr" "emacs$cli_list_tables")
(external-function "cli$dcl_parse" "emacs$share:emacs_cli_shr" "emacs$cli$dcl_parse")
(external-function "cli$present" "emacs$share:emacs_cli_shr" "emacs$cli$present")
(external-function "cli$get_value" "emacs$share:emacs_cli_shr" "emacs$cli$get_value")

(~sys_literal_setup_equal "cli$_"
    "absent"     229872           
    "comma"      261433           
    "concat"     261417           
    "defaulted"  261409           
    "locneg"     229936           
    "locpres"    261425           
    "negated"    229880           
    "nocomd"     229552           
    "present"    261401           
)

(defun define-cli-table
    (
	~table (get-tty-string ": cli-define-table ")
	~file (get-tty-string ": cli-define-table (file) ")
    )
    (cli_define_table ~table ~file)
    
)
(defun
    (list-cli-tables
	(save-excursion
	    (pop-to-buffer "CLI table list")
	    (erase-buffer)
	    (cli_list_tables)
	)
    )
)
)
