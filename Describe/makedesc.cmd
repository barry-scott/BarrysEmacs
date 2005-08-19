cd ..\tmp
echo ;empty >emacsinit.ml
copy ..\mlisp\emacs_profile.ml .
set emacs_path="emacs_library:"
set emacs_library=.
\work\emacsv7\editor\editor\Release\editor /restore=nothing /package=..\describe\em_desc
cd ..\describe
