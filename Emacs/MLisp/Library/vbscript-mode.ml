; 
;	vbscript-mode.ml
; 
;	contributed by Jack Horsfield
; 
(defun
    (vbscript-mode
	(setq mode-string "vbscript")
	(use-syntax-table "vbscript")
	(use-local-map "vbscript-map")
	(use-abbrev-table "vbscript")
	(novalue)
    )
)

(defun
    (vbscript-view-buffer
	(if buffer-is-modified
	    (write-current-file)
	)
	(WIN-vbscript-command current-buffer-file-name)
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
(defun
    ~vbscript-setup()
    (save-window-excursion
	(temp-use-buffer "~mode-hack")    
	(use-syntax-table "vbscript")
	(modify-syntax-table "word" "-_")
	
	(modify-syntax-table "comment" "'--" "\n")
	(modify-syntax-table "string" "\"")	
	(~mode-modify-syntax-table "keyword-1,case-fold"
	    "And"
	    "As"
	    "Call"
	    "Case"
	    "Class"
	    "Const"
	    "Dim"
	    "Do"
	    "Each"
	    "Else"
	    "End"
	    "Empty"
	    "Eqv"
	    "Erase"
	    "Error"
	    "Execute"
	    "Exit"
	    "Explicit"
	    "For"
	    "Function"
	    "Get"
	    "Goto"
	    "If"
	    "Imp"
	    "Is"
	    "Let"
	    "Loop"
	    "Mod"
	    "Next"
	    "Not"
	    "Nothing"
	    "Null"
	    "On"
	    "Option"
	    "Or"
	    "Private"
	    "Property"
	    "Public"
	    "Randomize"
	    "ReDim"
	    "Rem"
	    "Resume"
	    "Select"
	    "Set"
	    "Stop"
	    "Sub"
	    "Then"
	    "To"
	    "Type"
	    "Until"
	    "Wend"
	    "While"
	    "With"
	)
	(~mode-modify-syntax-table "keyword-2,case-fold"
	    "Abs"
	    "Array"
	    "Asc"
	    "Atn"
	    "CBool"
	    "CByte"
	    "CCur"
	    "CDate"
	    "CDbl"
	    "Chr"
	    "CInt"
	    "CLng"
	    "Cos"
	    "CreateObject"
	    "CSng"
	    "CStr"
	    "Date"
	    "DateAddFunction"
	    "DateDiff"
	    "DatePart"
	    "DateSerial"
	    "DateValue"
	    "Day"
	    "Eval"
	    "Exp"
	    "Filter"
	    "Fix"
	    "FormatCurrency"
	    "FormatDateTime"
	    "FormatNumber"
	    "FormatPercent"
	    "GetObject"
	    "GetRef"
	    "Hex"
	    "Hour"
	    "InputBox"
	    "InStr"
	    "InStrRev"
	    "Int"
	    "IsArray"
	    "IsDate"
	    "IsEmpty"
	    "IsNull"
	    "IsNumeric"
	    "IsObject"
	    "Join"
	    "LBound"
	    "LCase"
	    "Left"
	    "Len"
	    "LoadPicture"
	    "Log"
	    "LTrim"
	    "Mid"
	    "Minute"
	    "Month"
	    "MonthName"
	    "MsgBox"
	    "Now"
	    "Oct"
	    "Replace"
	    "RGB"
	    "Right"
	    "Rnd"
	    "Round"
	    "RTrim"
	    "ScriptEngine"
	    "ScriptEngineBuildVersion"
	    "ScriptEngineMajorVersion"
	    "ScriptEngineMinorVersion"
	    "Second"
	    "Sgn"
	    "Sin"
	    "Space"
	    "Split"
	    "Sqr"
	    "StrComp"
	    "String"
	    "StrReverse"
	    "Tan"
	    "Time"
	    "Timer"
	    "TimeSerial"
	    "TimeValue"
	    "Trim"
	    "TypeName"
	    "UBound"
	    "UCase"
	    "VarType"
	    "Weekday"
	    "WeekdayName"
	    "Year"
	    
	)
	
	(~mode-modify-syntax-table "keyword-3,case-fold"
	    "Description"
	    "FirstIndex"
	    "Global"
	    "HelpContext"
	    "HelpFile"
	    "IgnoreCase"
	    "Length"
	    "Number"
	    "Pattern"
	    "Source"
	    "Value"
	)
	(define-keymap "vbscript-map")
	(use-local-map "vbscript-map")
	
	(execute-mlisp-file "vbscript-mode.key")
	
	(kill-buffer "~mode-hack")
	(novalue)
    )
)
(~vbscript-setup)
