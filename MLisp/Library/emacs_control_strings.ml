; 
;   emacs_control_strings.ml
; 

; 
;   convert ESC-[ etc into eight bit form.
;   CSI, SS3, DCS, ???
; 
(setq control-string-convert-to-8-bit "[OP")
; 
;   convert SS3, DCS and ??? into CSI
; 
(setq control-string-convert-to-CSI "\217\220\232")
; 
; A control string consists of
; CSI <par>* <int>* <fin>
; where <par> is the parameter-characters and parameter-separators
;	<int> is the intermediate-characters
;	<fin> is the final-characters
; 
; The settings below conform to ISO control string standards as used
; in DEC VTxxx terminals.
; 
(setq control-string-final-characters
    "@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~")
(setq control-string-intermediate-characters " !\"#$&'()*+,-./")
(setq control-string-parameter-characters "0123456789:<=>?")
(setq control-string-parameter-separators ";")
; 
; Convert CSI <n> ~ into two characters <\200> <n+' '>
; 
(setq control-string-convert-F-keys 1)
; 
; Convert CSI ... &w from the mouse
; 
(setq control-string-convert-mouse 1)
; 
;   Set the following true to enable the above processing.
; 
;   The logical name emacs$control_string_processing is looked
;   to provide a way to turn off control-string-processing.
; 
(progn value
    (if
	(error-occurred
	    (setq value (+ (getenv "EMACS$CONTROL_STRING_PROCESSING")))
	)
	(setq control-string-processing 1)
	(setq control-string-processing value)
    )
)
