;
; process.ml
;
;   Copyright (c) Barry A. Scott 1994-2010
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
    (= operating-system-name "Linux")
    (execute-mlisp-file "unix-process.ml")
    (= operating-system-name "MacOSX")
    (execute-mlisp-file "unix-process.ml")
    (error-message "process.ml does not support operating system " operating-system-name)
)
