;
; motif-def.ml
;
; constants used in programming the motif interface functions
;
(if (! (is-bound sys_literal_loaded))
    (execute-mlisp-file "sys_literals")
)
(~sys_literal_setup_incr "" 0 1
    "XmNO_ORIENTATION"
    "XmVERTICAL"
    "XmHORIZONTAL"
)
(~sys_literal_setup_incr "" 0 1
    "XmNO_PACKING"
    "XmPACK_TIGHT"
    "XmPACK_COLUMN"
    "XmPACK_NONE"
)
(~sys_literal_setup_incr "" 0 1
    "XmNO_LINE"
    "XmSINGLE_LINE"
    "XmDOUBLE_LINE"
    "XmSINGLE_DASHED_LINE"
    "XmDOUBLE_DASHED_LINE"
    "XmSHADOW_ETCHED_IN"
    "XmSHADOW_ETCHED_OUT"
    "XmSHADOW_ETCHED_IN_DASH"
    "XmSHADOW_ETCHED_OUT_DASH"
)
(~sys_literal_setup_incr "" 0 1
    "XmDIALOG_TEMPLATE"
    "XmDIALOG_ERROR"
    "XmDIALOG_INFORMATION"
    "XmDIALOG_MESSAGE"
    "XmDIALOG_QUESTION"
    "XmDIALOG_WARNING"
    "XmDIALOG_WORKING"
)
