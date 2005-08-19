(progn
    (declare-global
        cms$_set_library
	cms$_show_class
	cms$_show_element
	cms$_show_group
	cms$_show_reservation
	cms$_fetch
	cms$_reserve
	cms$_unreserve
	cms$_annotate
	cms$_create_class
	cms$_create_group
	cms$_delete_class
	cms$_delete_element
	cms$_delete_group
	cms$_insert_element
	cms$_insert_generation
	cms$_insert_group
	cms$_replace
	cms$_create_element
	cms$_copy_element
	cms$_modify_class
	cms$_modify_element
	cms$_modify_group
	cms$_modify_library
	cms$_remove_element
	cms$_remove_generation
	cms$_remove_group
	cms$_remark
	~cms-library
	cms-element
	cms-group-flag
	cms-generation
	cms-user
	cms-nohistory
	cms-nonotes
	cms-concurrent
	cms-reservation
	cms-reference
	cms-keep
    )
    (declare-buffer-specific
        cms-remark
    )
    (setq-default cms-remark "")
    (setq cms-element "")
    (setq cms-group-flag "1")
    (setq cms-generation "")
    (setq cms-user "")
    (setq cms-nohistory "1")
    (setq cms-nonotes "1")
    (setq cms-concurrent "1")
    (setq cms-reservation "")
    (setq cms-reference "1")
    (setq cms-keep "1")
    (setq cms$_set_library 0)
    (setq cms$_show_class 1)
    (setq cms$_show_element 2)
    (setq cms$_show_group 3)
    (setq cms$_show_reservation 4)
    (setq cms$_fetch 5)
    (setq cms$_reserve 6)
    (setq cms$_unreserve 7)
    (setq cms$_annotate 8)
    (setq cms$_create_class 9)
    (setq cms$_create_group 10)
    (setq cms$_delete_class 11)
    (setq cms$_delete_element 12)
    (setq cms$_delete_group 13)
    (setq cms$_insert_element 14)
    (setq cms$_insert_generation 15)
    (setq cms$_insert_group 16)
    (setq cms$_replace 17)
    (setq cms$_create_element 18)
    (setq cms$_copy_element 19)
    (setq cms$_modify_class 20)
    (setq cms$_modify_element 21)
    (setq cms$_modify_group 22)
    (setq cms$_modify_library 23)
    (setq cms$_remove_element 24)
    (setq cms$_remove_generation 25)
    (setq cms$_remove_group 26)
    (setq cms$_remark 27)
    (setq ~cms-library "")
    (external-function "emacs$cms" "emacs$share:emacs_cms_shr")
    (defun
	(cms-set-library
	    ~cms-lib
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if ~cms-flag
		(setq ~cms-lib (arg 1 ": cms-set-library "))
	        (setq ~cms-lib (getenv "CMS$LIB"))
	    )
	    (emacs$cms cms$_set_library ~cms-lib)
	    (setq ~cms-lib (expand-file-name ~cms-lib))
	    (setq ~cms-library (substr ~cms-lib 1 (- (length ~cms-lib) 2)))
	    (if (interactive)
		(message "Library set to " ~cms-library)
	    )
	    (novalue)
	)
	(cms-show-library
	    (if (length ~cms-library)
	        (message "Your CMS library is " ~cms-library)
		(error-message "You have not defined a CMS library")
	    )
	    (novalue)
	)
        (cms-show-elements
	    ~cms-buffer
	    ~cms-elements
	    ~cms-group_flag
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (if ~cms-flag
		(if (! (form 1 19 "Show Elements" "~cms-show-elements"
		    "Elements" cms-element
		    "Include groups flag" cms-group-flag
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-elements (if (interactive) "" (arg 1)))
		    (setq ~cms-group_flag (if (interactive) 1 (arg 2)))
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_element ~cms-elements ~cms-group_flag)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified elements exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(~cms-show-elements ~cms-group
	    (setq cms-element (setq ~cms-elements (arg 1)))
	    (if (length (setq ~cms-group (arg 2)))
		(setq cms-group-flag (setq ~cms-group_flag ~cms-group))
	    )
	)
	(cms-show-class
	    ~cms-buffer
	    ~cms-classes
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (setq ~cms-classes
	        (if ~cms-flag
		    (arg 1 ": cms-show-class (class name) ")
		    ""
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_class ~cms-classes)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified classes exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(cms-show-group
	    ~cms-buffer
	    ~cms-groups
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (setq ~cms-groups
	        (if ~cms-flag
		    (arg 1 ": cms-show-group (group name) ")
		    ""
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_group ~cms-groups)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified groups exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(cms-show-reservation
	    ~cms-buffer
	    ~cms-elements
	    ~cms-generations
	    ~cms-user
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-buffer current-buffer-name)
	    (if ~cms-flag
		(if (! (form 1 14 "Show Reservations" "~cms-show-reservations"
		    "Elements" cms-element
		    "Generation" cms-generation
		    "User name" cms-user
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-elements (if (interactive) "" (arg 1)))
		    (setq ~cms-generations (if (interactive) "" (arg 2)))
		    (setq ~cms-user (if (interactive) "" (arg 3)))
		)
	    )
	    (error-occurred (delete-buffer "CMS list"))
	    (emacs$cms cms$_show_reservation ~cms-elements ~cms-generations ~cms-user)
	    (if (error-occurred (save-window-excursion (use-old-buffer "CMS list")))
	        (error-message "None of the specified reservations exist in " ~cms-library)
	    )
	    (pop-to-buffer "CMS list")
	    (pop-to-buffer ~cms-buffer)
	    (novalue)
	)
	(~cms-show-reservations
	    (setq cms-element (setq ~cms-elements (arg 1)))
	    (setq cms-generation (setq ~cms-generations (arg 2)))
	    (setq cms-user (setq ~cms-user (arg 3)))
	)
	(cms-fetch
	    ~cms-element
	    ~cms-generation
	    ~cms-nohistory
	    ~cms-nonotes
	    ~cms-buffer
	    ~cms-count
	    ~cms-gen_resp
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 15 "Fetch" "~cms-fetch"
		    "Element" ""
		    "Generation" ""
		    "No history flag" cms-nohistory
		    "No notes flag" cms-nonotes
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-fetch (element name) "))
		    (setq ~cms-generation (if (interactive) "" (arg 2)))
		    (setq ~cms-nohistory (if (interactive) 0 (arg 3)))
		    (setq ~cms-nonotes (if (interactive) 0 (arg 4)))
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "CMS fetch")
		(erase-buffer)
	        (emacs$cms cms$_fetch ~cms-element ~cms-generation
	            ~cms-nohistory ~cms-nonotes ~cms-gen_resp)
	    )
	    (if ask-about-buffer-names
	        (if (error-occurred (save-window-excursion (use-old-buffer ~cms-element)))
		(setq ~cms-buffer ~cms-element)
	        (progn
		    (setq ~cms-buffer
			(get-tty-string
		            (concat
				"Buffer name "
				~cms-element
				"is in use, type a new name or <CR> to clobber: "
			    )
			)
		    )
		    (if (= (length ~cms-buffer) 0)
		        (setq ~cms-buffer ~cms-element)
		    )
		    (error-occurred (delete-buffer ~cms-buffer))
		)
		)
		(progn
		    (setq ~cms-buffer ~cms-element)
		    (setq ~cms-count 1)
		    (save-window-excursion
			(while (! (error-occurred (use-old-buffer ~cms-buffer)))
			    (setq ~cms-count (+ ~cms-count 1))
			    (setq ~cms-buffer (concat ~cms-element "<" ~cms-count ">"))
			)
		    )
		)
	    )
	    (pop-to-buffer "CMS fetch")
	    (setq current-buffer-name ~cms-buffer)
	    (error-occurred (setq current-buffer-file-name ~cms-element))
	    (apply-auto-execute ~cms-element)
	    (setq buffer-is-modified 0)
	    (message "Generation " ~cms-gen_resp " of element " ~cms-element " fetched")
	    (novalue)
	)
	(~cms-fetch
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-generation (setq ~cms-generation (arg 2)))
	    (setq cms-nohistory (setq ~cms-nohistory (arg 3)))
	    (setq cms-nonotes (setq ~cms-nonotes (arg 4)))
	)
	(cms-reserve
	    ~cms-element
	    ~cms-generation
	    ~cms-remark
	    ~cms-nohistory
	    ~cms-nonotes
	    ~cms-concurrent
	    ~cms-buffer
	    ~cms-count
	    ~cms-gen_resp
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 16 "Reserve" "~cms-reserve"
		    "Element" ""
		    "Remark" cms-remark
		    "Generation" ""
		    "No history flag" cms-nohistory
		    "No notes flag" cms-nonotes
		    "Concurrency flag" cms-concurrent
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-reserve (element name) "))
		    (setq ~cms-remark (arg 2 ": cms-reserve (remark) "))
		    (setq ~cms-generation (if (interactive) "" (arg 3)))
		    (setq ~cms-nohistory (if (interactive) 0 (arg 4)))
		    (setq ~cms-nonotes (if (interactive) 0 (arg 5)))
		    (setq ~cms-concurrent (if (interactive) 1 (arg 5)))
		)
	    )
	    (save-window-excursion
		(switch-to-buffer "CMS fetch")
		(erase-buffer)
		(emacs$cms cms$_reserve ~cms-element ~cms-generation ~cms-remark
	            ~cms-nohistory ~cms-nonotes ~cms-concurrent ~cms-gen_resp)
	    )
	    (if ask-about-buffer-names
	        (if (error-occurred (save-window-excursion (use-old-buffer ~cms-element)))
		(setq ~cms-buffer ~cms-element)
	        (progn
		    (setq ~cms-buffer
			(get-tty-string
		            (concat
				"Buffer name "
				~cms-element
				"is in use, type a new name or <CR> to clobber: "
			    )
			)
		    )
		    (if (= (length ~cms-buffer) 0)
		        (setq ~cms-buffer ~cms-element)
		    )
		    (error-occurred (delete-buffer ~cms-buffer))
		)
		)
		(progn
		    (setq ~cms-buffer ~cms-element)
		    (setq ~cms-count 1)
		    (save-window-excursion
			(while (! (error-occurred (use-old-buffer ~cms-buffer)))
			    (setq ~cms-count (+ ~cms-count 1))
			    (setq ~cms-buffer (concat ~cms-element "<" ~cms-count ">"))
			)
		    )
		)
	    )
	    (pop-to-buffer "CMS fetch")
	    (setq current-buffer-name ~cms-buffer)
	    (error-occurred (setq current-buffer-file-name ~cms-element))
	    (apply-auto-execute ~cms-element)
	    (message "Generation " ~cms-gen_resp " of element " ~cms-element " reserved")
	    (novalue)
	)
	(~cms-reserve
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq cms-generation (setq ~cms-generation (arg 3)))
	    (setq cms-nohistory (setq ~cms-nohistory (arg 4)))
	    (setq cms-nonotes (setq ~cms-nonotes (arg 5)))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	)
	(cms-unreserve
	    ~cms-element
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element (arg 1 ": cms-unreserve (element name) "))
	    (setq ~cms-remark
		(if ~cms-flag
		    ""
		    (arg 2 ": cms-unreserve (remark) ")
		)
	    )
	    (emacs$cms cms$_unreserve ~cms-element ~cms-remark)
	    (novalue)
	)
        (cms-create-class
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-create-class (class name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-create-class (remark) ")
	      )
	    )
	(emacs$cms cms$_create_class ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-create-group
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-create-group (group name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-create-group (remark) ")
	      )
	    )
	(emacs$cms cms$_create_group ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-class
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-class (class name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-class (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_class ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-element
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-element (element name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-element (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_element ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-delete-group
	    ~cms-item
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-item (arg 1 ": cms-delete-group (group name) "))
	    (setq ~cms-remark
	      (if ~cms-flag
	        ""
		(arg 2 ": cms-delete-group (remark) ")
	      )
	    )
	(emacs$cms cms$_delete_group ~cms-item ~cms-remark)
	(novalue)
	)
        (cms-insert-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Insert Element" "~cms-item-target"
		    "Element" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-element (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_insert_element ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
	(~cms-item-target
	    (setq ~cms-item (arg 1))
	    (setq ~cms-target (arg 2))
	    (setq cms-remark (setq ~cms-remark (arg 3)))
	)
        (cms-insert-group
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 9 "Insert Group" "~cms-item-target"
		    "Sub-group" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-group (sub-group name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-group (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_insert_group ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
        (cms-insert-generation
	    ~cms-item
	    ~cms-target
	    ~cms-gen
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 10 "Insert Generation" "~cms-insert-generation"
		    "Element" ""
		    "Class" ""
		    "Generation" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-insert-generation (element name) "))
		    (setq ~cms-target (arg 2 ": cms-insert-generation (class name) "))
		    (setq ~cms-gen (if (interactive) "" (arg 3)))
		    (setq ~cms-remark (if (interactive) "" (arg 4)))
		)
	    )
	    (emacs$cms cms$_insert_generation ~cms-item ~cms-target ~cms-remark ~cms-gen)
	    (novalue)
	)
	(~cms-insert-generation
	    (setq ~cms-item (arg 1))
	    (setq ~cms-target (arg 2))
	    (setq ~cms-gen (arg 3))
	    (setq cms-remark (setq ~cms-remark (arg 4)))
	)
	(cms-replace
	    ~cms-element
	    ~cms-remark
	    ~cms-variant
	    ~cms-reserve
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element
		(if (length current-buffer-file-name)
		    current-buffer-name
		    cms-element
		)
	    )
	    (if ~cms-flag
		(if (! (form 2 16 "Replace" "~cms-replace"
		    "Element" ~cms-element
		    "Remark" cms-remark
		    "Variant" ""
		    "Reservation flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-replace (element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 2)))
		    (setq ~cms-variant (if (interactive) "" (arg 3)))
		    (setq ~cms-reserve (if (interactive) "" (arg 4)))
		)
	    )
	    (emacs$cms cms$_replace ~cms-element ~cms-remark ~cms-variant ~cms-reserve)
	    (setq buffer-is-modified 0)
	    (novalue)
	)
	(~cms-replace
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-variant (arg 3))
	    (setq ~cms-reserve (arg 4))
	)
	(cms-create-element
	    ~cms-element
	    ~cms-remark
	    ~cms-history
	    ~cms-keep
	    ~cms-reserve
	    ~cms-concurrent
	    ~cms-reference
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (setq ~cms-element
		(if (length current-buffer-file-name)
		    current-buffer-name
		    cms-element
		)
	    )
	    (if ~cms-flag
		(if (! (form 2 16 "Create Element" "~cms-create-element"
		    "Element" ~cms-element
		    "Remark" cms-remark
		    "History" ""
		    "Keep flag" cms-keep
		    "Reservation flag" cms-reservation
		    "Concurrency flag" cms-concurrent
		    "Reference flag" cms-reference
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-element (arg 1 ": cms-create-element (element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 2)))
		    (setq ~cms-history (if (interactive) "" (arg 3)))
		    (setq ~cms-keep (if (interactive) "1" (arg 4)))
		    (setq ~cms-reserve (if (interactive) "0" (arg 5)))
		    (setq ~cms-concurrent (if (interactive) "1" (arg 6)))
		    (setq ~cms-reference (if (interactive) "1" (arg 7)))
		)
	    )
	    (emacs$cms cms$_create_element ~cms-element ~cms-remark ~cms-history
	        "" ~cms-keep ~cms-reserve ~cms-concurrent ~cms-reference)
	    (setq buffer-is-modified 0)
	    (novalue)
	)
	(~cms-create-element
	    (setq cms-element (setq ~cms-element (arg 1)))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-history (arg 3))
	    (setq cms-keep (setq ~cms-keep (arg 4)))
	    (setq cms-reservation (setq ~cms-reserve (arg 5)))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	    (setq cms-reference (setq ~cms-reference (arg 7)))
	)
        (cms-copy-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 11 "Copy Element" "~cms-item-target"
		    "Element" ""
		    "New element" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-copy-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-copy-element (new element name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_copy_element ~cms-item ~cms-target ~cms-remark)
	    (novalue)
	)
        (cms-modify-class
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-read-only
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 16 "Modify Class" "~cms-modify"
		    "Class" ""
		    "Remark" cms-remark
		    "New class" ""
		    "New class remark" ""
		    "Read only flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_class ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark ~cms-read-only)
	    (novalue)
	)
	(~cms-modify
	    (setq ~cms-item (arg 1))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-new-item (arg 3))
	    (setq ~cms-new-remark (arg 4))
	    (setq ~cms-read-only (arg 5))
	)
        (cms-modify-group
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-read-only
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 16 "Modify Group" "~cms-modify"
		    "Group" ""
		    "Remark" cms-remark
		    "New group" ""
		    "New group remark" ""
		    "Read only flag" ""
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_group ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark ~cms-read-only)
	    (novalue)
	)
        (cms-modify-element
	    ~cms-item
	    ~cms-remark
	    ~cms-new-item
	    ~cms-new-remark
	    ~cms-history
	    ~cms-concurrent
	    ~cms-reference
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if (interactive)
		(if (! (form 1 18 "Modify Element" "~cms-modify"
		    "Element" ""
		    "Remark" cms-remark
		    "New element" ""
		    "New element remark" ""
		    "History" ""
		    "Concurrency flag" cms-concurrent
		    "Reference flag" cms-reference
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1))
		    (setq ~cms-remark (arg 2))
		    (setq ~cms-new-item (arg 3))
		    (setq ~cms-new-remark (arg 4))
		    (setq ~cms-read-only (arg 5))
		)
	    )
	    (emacs$cms cms$_modify_element ~cms-item ~cms-remark ~cms-new-item ~cms-new-remark
	        ~cms-history "" ~cms-concurrent ~cms-reference)
	    (novalue)
	)
	(~cms-modify
	    (setq ~cms-item (arg 1))
	    (setq cms-remark (setq ~cms-remark (arg 2)))
	    (setq ~cms-new-item (arg 3))
	    (setq ~cms-new-remark (arg 4))
	    (setq ~cms-history (arg 5))
	    (setq cms-concurrent (setq ~cms-concurrent (arg 6)))
	    (setq cms-reference (setq ~cms-reference (arg 7)))
	)
        (cms-remove-element
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Element" "~cms-item-target"
		    "Element" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-element (element name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-element (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_element ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remove-generation
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Generation" "~cms-item-target"
		    "Element" ""
		    "Class" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-generation (element name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-generation (class name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_generation ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remove-group
	    ~cms-item
	    ~cms-target
	    ~cms-remark
	    ~cms-flag
	    (setq ~cms-flag prefix-argument-provided)
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (if ~cms-flag
		(if (! (form 1 7 "Remove Group" "~cms-item-target"
		    "Sub-group" ""
		    "Group" ""
		    "Remark" cms-remark
		)) (error-message "Aborted."))
		(progn
		    (setq ~cms-item (arg 1 ": cms-remove-group (sub-group name) "))
		    (setq ~cms-target (arg 2 ": cms-remove-group (group name) "))
		    (setq ~cms-remark (if (interactive) "" (arg 3)))
		)
	    )
	    (emacs$cms cms$_remove_group ~cms-item ~cms-target ~cms-remark 1)
	    (novalue)
	)
        (cms-remark
	    (if (= (length ~cms-library) 0) (cms-set-library))
	    (emacs$cms cms$_remark (arg 1 ": cms-remark "))
	    (novalue)
	)
    )
    (error-occurred (execute-mlisp-file "cms.key"))
    (if (! (is-bound form-depth))
	(error-occurred (execute-mlisp-file "form"))
    )
)
