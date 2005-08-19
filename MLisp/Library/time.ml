(progn
    (declare-global time-interval &time-running)
    (if (= time-interval 0) (setq time-interval 1))
    (defun
	(%time %time-is
	    (setq %time-is (substr (current-time) 12 8))
	    (setq mode-string %time-is)
	    (schedule-procedure "%time" time-interval)
	)
	(time
	    (if &time-running
		(schedule-procedure "%time" 0)
		(schedule-procedure "%time" time-interval)
	    )
	    (setq &time-running (!= &time-running 0))
	    (setq &time-running (! &time-running))
	    (novalue)
	)
    )
)
