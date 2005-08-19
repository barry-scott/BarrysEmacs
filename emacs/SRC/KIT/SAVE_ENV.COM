$	!
$	!	emacs_save_environment.com
$	!
$	!	create the users environment file
$	!
$	define	= "define"
$	emacs	= "emacs"
$	define/process/user	emacs$memory	nl:
$
$	emacs/nolog/nosub/norestore
(execute-mlisp-line (concat "(" enter-emacs-hook ")"))
(save-environment "emacs$environment:")
$
