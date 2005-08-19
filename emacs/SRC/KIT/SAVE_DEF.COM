$	!
$	!	emacs_save_default_environment.com
$	!
$	define	= "define"
$	emacs	= "emacs"
$	define/process/user	emacs$memory	nl:
$
$	emacs/nosub/norestore/nolog
(save-environment "emacs$library:emacsinit")
$
