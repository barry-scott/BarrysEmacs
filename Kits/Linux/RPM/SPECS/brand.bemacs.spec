Name:		bemacs
Version:	%(major)s.%(minor)s.%(patch)s
Release:	1
Summary:	Barry's Emacs %(version)s text editor for X11 and ANSI terminals
License:	Copyright Barry A. Scott (c) 1990-%(year)s
Group:		Applications/Editors
Packager:	Barry A. Scott <barry@barrys-emacs.org>
%%description
Barry's Emacs %(version)s text editor for X11 and ANSI terminals.

Copyright Barry A. Scott (c) 1990-%(year)s

mailto:barry@barrys-emacs.org
http://www.barrys-emacs.org

%%prep
%%build
%%install
%%post
ln -fs /usr/local/bemacs/bemacs /usr/local/bin/bemacs
ln -fs /usr/local/bemacs/emacs-xfree86.xkeys /usr/local/bemacs/emacs.xkeys
%%postun
rm -f /usr/local/bin/bemacs
rm -f /usr/local/bemacs/emacs.xkeys
rmdir --ignore-fail-on-non-empty /usr/local/bemacs
%%files
%%defattr (-,root,root)
%%attr(555,root,root) /usr/local/bemacs/bemacs
%%attr(555,root,root) /usr/local/bemacs/bemacs_server
%%attr(555,root,root) /usr/local/bemacs/dbadd
%%attr(555,root,root) /usr/local/bemacs/dbcreate
%%attr(555,root,root) /usr/local/bemacs/dbdel
%%attr(555,root,root) /usr/local/bemacs/dblist
%%attr(555,root,root) /usr/local/bemacs/dbprint
%%attr(555,root,root) /usr/local/bemacs/mll-2-db
%%attr(444,root,root) /usr/local/bemacs/emacs-excursions.xkeys
%%attr(444,root,root) /usr/local/bemacs/emacs-hp-new.xkeys
%%attr(444,root,root) /usr/local/bemacs/emacs-hp-old.xkeys
%%attr(444,root,root) /usr/local/bemacs/emacs-lk201.xkeys
%%attr(444,root,root) /usr/local/bemacs/emacs-xfree86.xkeys
%%attr(444,root,root) /usr/local/bemacs/emacs_profile.ml
%%attr(444,root,root) /usr/local/bemacs/emacs_qinfo_c.dat
%%attr(444,root,root) /usr/local/bemacs/emacs_qinfo_c.dir
%%attr(444,root,root) /usr/local/bemacs/emacs_qinfo_c.pag
%%attr(444,root,root) /usr/local/bemacs/emacsdesc.dat
%%attr(444,root,root) /usr/local/bemacs/emacsdesc.dir
%%attr(444,root,root) /usr/local/bemacs/emacsdesc.pag
%%attr(444,root,root) /usr/local/bemacs/emacsinit.ml
%%attr(444,root,root) /usr/local/bemacs/emacslang.dat
%%attr(444,root,root) /usr/local/bemacs/emacslang.dir
%%attr(444,root,root) /usr/local/bemacs/emacslang.pag
%%attr(444,root,root) /usr/local/bemacs/emacslib.dat
%%attr(444,root,root) /usr/local/bemacs/emacslib.dir
%%attr(444,root,root) /usr/local/bemacs/emacslib.pag
%%attr(444,root,root) /usr/local/bemacs/toolbar_build_compile.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_build_next_error.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_build_prev_error.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_case_capitalise.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_case_invert.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_case_lower.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_case_upper.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_edit_copy.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_edit_cut.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_edit_delete.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_edit_paste.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_file_open.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_file_print.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_file_save.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_file_save_all.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_macro_play.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_macro_record.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_macro_stop.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_region_indent.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_region_undent.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_search_find.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_search_fold_case.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_tools_grep.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_view_white_space.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_view_wrap_long.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_cascade.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_del_other.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_del_this.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_split_horiz.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_split_vert.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_tile_horiz.xpm
%%attr(444,root,root) /usr/local/bemacs/toolbar_window_tile_vert.xpm
%%attr(444,root,root) /usr/local/bemacs/stop.xpm
%%attr(444,root,root) /usr/local/bemacs/synchronize.xpm
%%attr(444,root,root) /usr/local/bemacs/open-curly-brace.xpm
%%attr(444,root,root) /usr/local/bemacs/close-curly-brace.xpm
%%attr(444,root,root) /usr/local/bemacs/justify.xpm

%%doc readme.txt
