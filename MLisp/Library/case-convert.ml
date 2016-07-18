;
; case-convert.ml
;
; Copyright 1998 (c) BArry A. SCott
;
; Define case manipulation functions
;
(defun
    (case-lower
        (if (error-occurred (mark))
            (case-word-lower)
            (case-region-lower)
        )
    )
)
(defun
    (case-upper
        (if (error-occurred (mark))
            (case-word-upper)
            (case-region-upper)
        )
    )
)
(defun
    (case-capitalize
        (if (error-occurred (mark))
            (case-word-capitalize)
            (case-region-capitalize)
        )
    )
)
(defun
    (case-invert
        (if (error-occurred (mark))
            (case-word-invert)
            (case-region-invert)
        )
    )
)
