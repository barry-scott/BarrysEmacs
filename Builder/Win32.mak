#
#	makefile for bemacs
#	starts in .\Builder
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
build: brand kitdir editor mlisp describe quick_info language inno_kit

brand:
	python brand_version.py version_info.txt ..

kitdir:
	if not exist $(KITROOT) mkdir $(KITROOT)
	if not exist $(KITROOT)\library mkdir $(KITROOT)\Library
	if not exist $(KITROOT)\documentation mkdir $(KITROOT)\Documentation

inno_kit: $(KITROOT)\Output\setup.exe

#
#	Rule to run INNO's IDE to allow debugging of the installation
#
inno_debug:
	"c:\Program Files\Inno Setup 4\Compil32.exe" ..\kit\bemacs.iss

$(KITROOT)\Output\setup.exe: ..\kit\bemacs.iss
	copy ..\kit\msvc$(MSVC_VERSION)_system_files.iss ..\kit\msvc_system_files.iss
	"c:\Program Files\Inno Setup 4\ISCC.exe" ..\kit\bemacs.iss
	..\kit\copy-setup.cmd ..\kit\Output\setup.exe ..\kit
editor: kitdir "$(KITROOT)\_bemacs.pyd"

"$(KITROOT)\_bemacs.pyd":
	@echo Info: Build Editor $(BUILDER_CFG_BUILD_TYPE)
	cd ..\editor
	.\build

!if "$(MSVC_VERSION)" == "71"
	devenv bemacs.sln /build $(BUILDER_CFG_BUILD_TYPE) /project bemacs 
	devenv bemacs.sln /build "Release MinDependency" /project BEmacsClassMoniker
!else if "$(MSVC_VERSION)" == "60"
	msdev bemacs.dsw /make "bemacs - Win32 $(BUILDER_CFG_BUILD_TYPE)"
	msdev bemacs.dsw /make "BEmacsClassMoniker - Win32 Release MinDependency"
!endif
	echo Info: Copying images...
	copy  "..\Editor\win32\obj-pybemacs\_bemacs.pyd" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\dbadd.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\dbcreate.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\dbdel.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\dblist.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\dbprint.exe" "$(KITROOT)" >NUL
	copy  "..\Editor\win32\obj-utils\mll2db.exe" "$(KITROOT)" >NUL

	rem copy  "..\Editor\editor\$(BUILDER_CFG_BUILD_TYPE)\editor.exe" "$(KITROOT)\BEmacsServer.exe" >NUL
	rem copy  "..\Editor\BEmacsComClient\$(BUILDER_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\BEmacs.exe" >NUL
	rem copy  "..\Editor\BEmacsComClient\$(BUILDER_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\BEmacsWait.exe" >NUL
	rem python  "..\Editor\BEmacsComClient\make_bemacs_cmd.py" make "$(KITROOT)\BEmacsWait.exe" main /wait
	rem copy  "..\Editor\BEmacsComClient\$(BUILDER_CFG_BUILD_TYPE)\BEmacsComClient.exe" "$(KITROOT)\vss-view.exe" >NUL
	rem python  "..\Editor\BEmacsComClient\make_bemacs_cmd.py" make "$(KITROOT)\vss-view.exe" main /package=vss-view
	rem copy  "..\BEmacsClassMoniker\ReleaseMinDependency\BEmacsClassMoniker.dll" "$(KITROOT)" >NUL

mlisp:
	echo Info: Copying Mlisp files...
	cd ..\mlisp && python create_library.py common,windows  $(KITROOT)\Library\emacslib $(KITROOT)

describe:
	echo Info: Making describe...
	"$(KITROOT)\dbcreate" "$(KITROOT)\Library\EmacsDesc" -c
	"$(KITROOT)\mll2db" "..\describe\em_desc.mll" "$(KITROOT)\Library\EmacsDesc"

language:
	echo Info: Making language...
	"$(KITROOT)\dbcreate" "$(KITROOT)\Library\EmacsLang" -c
	"$(KITROOT)\mll2db" "..\language\language.mll" "$(KITROOT)\Library\EmacsLang"

quick_info:
	echo Info: Making quick info...
	"$(KITROOT)\dbcreate" "$(KITROOT)\Library\emacs_qinfo_c" -c
	"$(KITROOT)\mll2db" "..\describe\qi_cc.mll" "$(KITROOT)\Library\emacs_qinfo_c"

clean:
	cd ..\editor
	.\build clean
!if "$(MSVC_VERSION)" == "71"
	devenv bemacs.sln /clean $(BUILDER_CFG_BUILD_TYPE) /project bemacs 
	devenv bemacs.sln /clean "Release MinDependency" /project BEmacsClassMoniker
!else if "$(MSVC_VERSION)" == "60"
	msdev bemacs.dsw /make "bemacs - Win32 $(BUILDER_CFG_BUILD_TYPE)" /clean
	msdev bemacs.dsw /make "BEmacsClassMoniker - Win32 Release MinDependency" /clean
!endif
	if exist $(KITROOT) rmdir /s /q  $(KITROOT)
