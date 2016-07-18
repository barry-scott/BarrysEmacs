;
; show-system.ml
;
(progn

(execute-mlisp-file "jpidef")
(execute-mlisp-file "syidef")

(declare-global ~show-system-process-states)
(setq ~show-system-process-states (array 1 14))

(setq-array ~show-system-process-states sch$c_colpg "colpg")
(setq-array ~show-system-process-states sch$c_mwait "mwait")
(setq-array ~show-system-process-states sch$c_cef "cef")
(setq-array ~show-system-process-states sch$c_pfw "pfw")
(setq-array ~show-system-process-states sch$c_lef "lef")
(setq-array ~show-system-process-states sch$c_lefo "lefo")
(setq-array ~show-system-process-states sch$c_hib "hib")
(setq-array ~show-system-process-states sch$c_hibo "hibo")
(setq-array ~show-system-process-states sch$c_susp "susp")
(setq-array ~show-system-process-states sch$c_suspo "suspo")
(setq-array ~show-system-process-states sch$c_fpg "fpg")
(setq-array ~show-system-process-states sch$c_com "com")
(setq-array ~show-system-process-states sch$c_como "como")
(setq-array ~show-system-process-states sch$c_cur "cur")

(defun
    (show-system
        ~nodename ~version ~boottime

        ~ctx
        ~pid ~prcnam ~state ~pri ~bufio ~dirio ~cputim ~faults ~ppg ~gpg
        ~parent-pid ~mode

        (save-excursion
            (setq ~ctx -1)
            (pop-to-buffer "Show System")
            (erase-buffer)
            (sys$getsyi ""
                syi$_nodename   ~nodename
                syi$_version    ~version
                syi$_boottime   ~boottime
            )
            (insert-string
                (concat
                    (sys$fao "VAX/VMS !4AS on node !6AS !%D  Up since: !17AS\n"
                        "ssns" ~version ~nodename 0 ~boottime
                    )
                    "  Pid    Process Name    State  Pri      I/O       CPU       Page flts Ph.Mem\n"
                )
            )
            (while
                (& 1 (sys$getjpi "i" ~ctx
                         jpi$_pid       ~pid
                         jpi$_prcnam    ~prcnam
                         jpi$_state     ~state
                         jpi$_pri       ~pri
                         jpi$_bufio     ~bufio
                         jpi$_dirio     ~dirio
                         jpi$_cputim    ~cputim
                         jpi$_pageflts  ~faults
                         jpi$_ppgcnt    ~ppg
                         jpi$_gpgcnt    ~gpg
                         jpi$_mode      ~mode
                         jpi$_owner     ~parent-pid
                     )
                )

                (progn
                    (insert-string
                        (sys$fao
                            "!XL !16AS!7AS!3SL   !6SL !15AS   !7SL  !5SL  !1AS\n"
                            "nssnnsnns"
                            ~pid ~prcnam
                            (fetch-array ~show-system-process-states ~state)
                            ~pri (+ ~bufio ~dirio)
                            (~sys-time ~cputim)
                            ~faults (+ ~ppg ~gpg)
                            (if ~parent-pid
                                "S"
                                (if (= ~mode jpi$k_network)
                                    "N"
                                    (= ~mode jpi$k_batch)
                                    "B"
                                    ""
                                )
                            )
                        )
                    )
                    (sit-for 0)
                )
            )
            (beginning-of-file)
        )
        (novalue)
    )
)
(defun ~sys-time (~time)
    ~hours ~minutes ~seconds ~frac-secs

    (setq ~frac-secs (% ~time 100)) (setq ~time (/ ~time 100))
    (setq ~seconds (% ~time 60)) (setq ~time (/ ~time 60))
    (setq ~minutes (% ~time 60)) (setq ~time (/ ~time 60))
    (setq ~hours (% ~time 24)) (setq ~time (/ ~time 24))
    (sys$fao "!3SL !2ZL:!2ZL:!2ZL.!2ZL" "nnnnn"
        ~time ~hours ~minutes ~seconds ~frac-secs
    )
)
)
