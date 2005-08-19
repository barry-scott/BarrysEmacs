#
#	makefile for bemacs
#	starts in .\_build
#

#################################################################################
#
#
#	Win32 build
#
#
#################################################################################

KITROOT=..\kit\win32

#
build: kitdir editor mlisp describe quick_info language inno_kit

kitdir:
	if not exist $(KITROOT) mkdir  $(KITROOT)

inno_kit: $(KITROOT)\Output\setup.exe

#
#	Rule to run INNO's IDE to allow debugging of the installation
#
inno_debug:
	"c:\Program Files\My Inno Setup Extensions 3\Compil32.exe" ..\kit\bemacs.iss

$(KITROOT)\Output\setup.exe: ..\kit\bemacs.iss
	"c:\Program Files\My Inno Setup Extensions 3\ISCC.exe" ..\kit\bemacs.iss

editor: kitdir "$(KITROOT)\BEmacsServer.exe"

"$(KITROOT)\BEmacsServer.exe":
	@echo Info: Build Editor $(RB_CFG_BUILD_TYPE)
	cd ..\editor
	msdev bemacs.dsp /MAKE "bemacs - Win32 $(RB_CFG_BUILD_TYPE)"

	msdev bemacs.dsp /MAKE "BEmacsClassMoniker - Win32 Release MinDependency"

	echo Info: Copying images...
	copy  "..\Editor\editor\$(RB_CFG_BUILD_TYPE)\editor.exe" "$(KITROOT)\BEmacsServer.exe" >NUL
	copy  "..\Editor\BEmacsComClient\$(RB_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\BEmacs.exe" >NUL
	copy  "..\Editor\BEmacsComClient\$(RB_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\BEmacsWait.exe" >NUL
	python  "..\Editor\BEmacsComClient\make_bemacs_cmd.py" make "$(KITROOT)\BEmacsWait.exe" main /wait
	copy  "..\Editor\BEmacsComClient\$(RB_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\vss-view.exe" >NUL
	python  "..\Editor\BEmacsComClient\make_bemacs_cmd.py" make "$(KITROOT)\vss-view.exe" main /package=vss-view
	copy  "..\BEmacsClassMoniker\ReleaseMinDependency\BEmacsClassMoniker.dll" "$(KITROOT)" >NUL
	copy  "..\Editor\dbcreate\$(RB_CFG_BUILD_TYPE)\dbcreate.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\dbadd\$(RB_CFG_BUILD_TYPE)\dbadd.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\dbdel\$(RB_CFG_BUILD_TYPE)\dbdel.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\dblist\$(RB_CFG_BUILD_TYPE)\dblist.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\dbprint\$(RB_CFG_BUILD_TYPE)\dbprint.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\mll2db\$(RB_CFG_BUILD_TYPE)\mll-2-db.exe" "$(KITROOT)" >NUL

mlisp:
	echo Info: Copying Mlisp files...
	cd ..\mlisp && python create_library.py common,windows  $(KITROOT)\emacslib $(KITROOT)

describe:
	echo Info: Making describe...
	"$(KITROOT)\dbcreate" "$(KITROOT)\EmacsDesc" -c
	"$(KITROOT)\mll-2-db" "..\describe\em_desc.mll" "$(KITROOT)\EmacsDesc"

language:
	echo Info: Making language...
	"$(KITROOT)\dbcreate" "$(KITROOT)\EmacsLang" -c
	"$(KITROOT)\mll-2-db" "..\language\language.mll" "$(KITROOT)\EmacsLang"

quick_info:
	echo Info: Making quick info...
	"$(KITROOT)\dbcreate" "$(KITROOT)\emacs_qinfo_c" -c
	"$(KITROOT)\mll-2-db" "..\describe\qi_cc.mll" "$(KITROOT)\emacs_qinfo_c"
