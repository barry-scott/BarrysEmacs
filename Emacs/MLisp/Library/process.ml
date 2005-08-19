; 
; process.ml	- Barry A. Scott 31-Aug-1994
; 
;   This module supports Unix processes, VMS and NT processes
;   The code is OS dependant - however the VMS and NT code is
;   resonable common.
; 
(if
    (= operating-system-name "VMS")
    (execute-mlisp-file "vms-process.ml")
    (= operating-system-name "Windows")
    (execute-mlisp-file "windows-process.ml")
    (= operating-system-name "unix")
    (execute-mlisp-file "unix-process.ml")
    (error-message "process.ml does not support operating system " operating-system-name)
)
