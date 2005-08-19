# Microsoft Developer Studio Project File - Name="editor" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=EDITOR - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "editor.mak" CFG="EDITOR - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "editor - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "editor - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "editor - Win32 Profile" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/EmacsV7/Editor", ZWAAAAAA"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "editor - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /Ob2 /I "..\..\Editor" /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python22.lib Delayimp.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Release\_Lib" /libpath:"..\Python_CXX_Lib\Release" /verbose:lib /delayload:python22.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir "."
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /I "..\..\Editor" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /D "_AFXDLL" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug\editor.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python22_d.lib Delayimp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Debug\_Lib" /libpath:"..\Python_CXX_Lib\Debug" /verbose:lib /delayload:python22_d.dll
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Profile"
# PROP BASE Intermediate_Dir ".\Profile"
# PROP BASE Target_Dir "."
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Profile"
# PROP Intermediate_Dir ".\Profile"
# PROP Target_Dir "."
# ADD BASE CPP /nologo /MDd /W4 /Gm /GX /Zi /Od /I "..\\" /D "_DEBUG" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_AFXDLL" /D "_MBCS" /FR /Yu"emacs.h" /c
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /O2 /Ob2 /I "..\..\Editor" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug\editor.bsc"
# ADD BSC32 /nologo /o"Debug\editor.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python22_d.lib Delayimp.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Debug\_Lib" /libpath:"..\Python_CXX_Lib\Debug" /verbose:lib /delayload:python22_d.dll

!ENDIF 

# Begin Target

# Name "editor - Win32 Release"
# Name "editor - Win32 Debug"
# Name "editor - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\abbrev.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD CPP /Zi

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\abspath.cpp
# End Source File
# Begin Source File

SOURCE=..\arith.cpp
# End Source File
# Begin Source File

SOURCE=..\buf_man.cpp
# End Source File
# Begin Source File

SOURCE=..\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\caseconv.cpp
# End Source File
# Begin Source File

SOURCE=..\columns.cpp
# End Source File
# Begin Source File

SOURCE=..\commandline.cpp
# End Source File
# Begin Source File

SOURCE=..\dbman.cpp
# End Source File
# Begin Source File

SOURCE=..\display.cpp
# End Source File
# Begin Source File

SOURCE=..\display_insert_delete.cpp
# End Source File
# Begin Source File

SOURCE=..\display_line.cpp
# End Source File
# Begin Source File

SOURCE=..\doprint.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\em_stat.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\em_time.cpp
# End Source File
# Begin Source File

SOURCE=..\emacs.cpp
# End Source File
# Begin Source File

SOURCE=..\emacs_bemacs_python_module.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Release\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\emacs_init.cpp
# End Source File
# Begin Source File

SOURCE=..\emacs_proc.cpp
# End Source File
# Begin Source File

SOURCE=..\emacs_python_interface.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Release\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD CPP /I "..\..\Python\CXX-Bindings\cxx" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\emacsrtl.cpp
# End Source File
# Begin Source File

SOURCE=..\emarray.cpp
# End Source File
# Begin Source File

SOURCE=..\emstring.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\emstrtab.cpp
# End Source File
# Begin Source File

SOURCE=..\errlog.cpp
# End Source File
# Begin Source File

SOURCE=..\ext_func.cpp
# End Source File
# Begin Source File

SOURCE=..\file_name_compare.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\fio.cpp
# End Source File
# Begin Source File

SOURCE=..\function.cpp
# End Source File
# Begin Source File

SOURCE=..\getdb.cpp
# End Source File
# Begin Source File

SOURCE=..\getdirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\getfile.cpp
# End Source File
# Begin Source File

SOURCE=..\glob_var.cpp
# End Source File
# Begin Source File

SOURCE=..\gui_input_mode.cpp
# End Source File
# Begin Source File

SOURCE=..\ICommandLineImpl.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\journal.cpp
# End Source File
# Begin Source File

SOURCE=..\key_names.cpp
# End Source File
# Begin Source File

SOURCE=..\keyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\lispfunc.cpp
# End Source File
# Begin Source File

SOURCE=..\macros.cpp
# End Source File
# Begin Source File

SOURCE=..\mem_man.cpp
# End Source File
# Begin Source File

SOURCE=..\metacomm.cpp
# End Source File
# Begin Source File

SOURCE=..\minibuf.cpp
# End Source File
# Begin Source File

SOURCE=..\mlispars.cpp
# End Source File
# Begin Source File

SOURCE=..\mlispexp.cpp
# End Source File
# Begin Source File

SOURCE=..\mlisproc.cpp
# End Source File
# Begin Source File

SOURCE=..\mlprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\ndbm.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\nt_async.cpp
# End Source File
# Begin Source File

SOURCE=..\nt_comm.cpp
# End Source File
# Begin Source File

SOURCE=..\nt_pipe.cpp
# End Source File
# Begin Source File

SOURCE=..\nt_proc.cpp
# End Source File
# Begin Source File

SOURCE=..\options.cpp
# End Source File
# Begin Source File

SOURCE=..\queue.cpp
# End Source File
# Begin Source File

SOURCE=..\save_env.cpp
# End Source File
# Begin Source File

SOURCE=..\search.cpp
# End Source File
# Begin Source File

SOURCE=..\search_extended_algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\search_extended_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\search_interface.cpp
# End Source File
# Begin Source File

SOURCE=..\search_simple_algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\search_simple_engine.cpp
# End Source File
# Begin Source File

SOURCE=..\simpcomm.cpp
# End Source File
# Begin Source File

SOURCE=..\stdafx.cpp
# ADD CPP /Yc"emacs.h"
# End Source File
# Begin Source File

SOURCE=..\string_map.cpp
# End Source File
# Begin Source File

SOURCE=..\subproc.cpp
# End Source File
# Begin Source File

SOURCE=..\syntax.cpp
# End Source File
# Begin Source File

SOURCE=..\term.cpp
# End Source File
# Begin Source File

SOURCE=..\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\undo.cpp
# End Source File
# Begin Source File

SOURCE=..\variable.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\varthunk.cpp
# End Source File
# Begin Source File

SOURCE=..\win_com_support.cpp
# End Source File
# Begin Source File

SOURCE=..\win_doc.cpp
# End Source File
# Begin Source File

SOURCE=..\win_emacs.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\win_emacs.odl

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD MTL /tlb "..\ExternalInclude\BarrysEmacs.tlb" /h "..\ExternalInclude\BarrysEmacs.h" /iid "..\ExternalInclude\BarrysEmacsUUID.c"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD MTL /tlb "..\ExternalInclude\BarrysEmacs.tlb" /h "..\ExternalInclude\BarrysEmacs.h" /iid "..\ExternalInclude\BarrysEmacsUUID.c"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\win_emacs.rc

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD BASE RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Release" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"
# ADD RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Release" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD BASE RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Debug" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"
# ADD RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Debug" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD BASE RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Profile" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"
# ADD RSC /l 0x809 /i "\Latest\V7.1\Emacs\Editor" /i ".\Profile" /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i ".." /i "..\..\Editor"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\win_ext_func.cpp
# End Source File
# Begin Source File

SOURCE=..\win_file.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\win_find.cpp
# End Source File
# Begin Source File

SOURCE=..\win_list_ctrl_ex.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\win_main_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\win_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\win_opt.cpp
# End Source File
# Begin Source File

SOURCE=..\win_registry.cpp
# End Source File
# Begin Source File

SOURCE=..\win_rtl.cpp
# End Source File
# Begin Source File

SOURCE=..\win_select_buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\win_toolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\win_ui.cpp
# End Source File
# Begin Source File

SOURCE=..\win_view.cpp
# End Source File
# Begin Source File

SOURCE=..\windman.cpp
# End Source File
# Begin Source File

SOURCE=..\window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\abbrev.h
# End Source File
# Begin Source File

SOURCE="..\..\Python\CXX-Bindings\CXX\CXX\Array.hxx"
# End Source File
# Begin Source File

SOURCE=..\buffer.h
# End Source File
# Begin Source File

SOURCE=..\commandline.h
# End Source File
# Begin Source File

SOURCE="..\..\Python\CXX-Bindings\CXX\CXX\Config.hxx"
# End Source File
# Begin Source File

SOURCE=..\display.h
# End Source File
# Begin Source File

SOURCE=..\em_gen.h
# End Source File
# Begin Source File

SOURCE=..\em_mac.h
# End Source File
# Begin Source File

SOURCE=..\em_rtn.h
# End Source File
# Begin Source File

SOURCE=..\em_stat.h
# End Source File
# Begin Source File

SOURCE=..\em_time.h
# End Source File
# Begin Source File

SOURCE=..\em_user.h
# End Source File
# Begin Source File

SOURCE=..\em_util.h
# End Source File
# Begin Source File

SOURCE=..\em_var.h
# End Source File
# Begin Source File

SOURCE=..\emacs.h
# End Source File
# Begin Source File

SOURCE=..\emacs_proc.h
# End Source File
# Begin Source File

SOURCE=..\emacs_python_interface.h
# End Source File
# Begin Source File

SOURCE=..\emacsutl.h
# End Source File
# Begin Source File

SOURCE=..\emexcept.h
# End Source File
# Begin Source File

SOURCE=..\emobject.h
# End Source File
# Begin Source File

SOURCE=..\emstring.h
# End Source File
# Begin Source File

SOURCE=..\emstrtab.h
# End Source File
# Begin Source File

SOURCE=..\errlog.h
# End Source File
# Begin Source File

SOURCE="..\..\Python\CXX-Bindings\CXX\CXX\Exception.hxx"
# End Source File
# Begin Source File

SOURCE="..\..\Python\CXX-Bindings\CXX\CXX\Extensions.hxx"
# End Source File
# Begin Source File

SOURCE=..\fileio.h
# End Source File
# Begin Source File

SOURCE=..\fileserv.h
# End Source File
# Begin Source File

SOURCE=..\getdb.h
# End Source File
# Begin Source File

SOURCE=..\getdirectory.h
# End Source File
# Begin Source File

SOURCE=..\getfile.h
# End Source File
# Begin Source File

SOURCE=..\ICommandLineImpl.h
# End Source File
# Begin Source File

SOURCE=..\journal.h
# End Source File
# Begin Source File

SOURCE=..\key_names.h
# End Source File
# Begin Source File

SOURCE=..\keyboard.h
# End Source File
# Begin Source File

SOURCE=..\mem_man.h
# End Source File
# Begin Source File

SOURCE=..\mlisp.h
# End Source File
# Begin Source File

SOURCE=..\mlispexp.h
# End Source File
# Begin Source File

SOURCE=..\ndbm.h
# End Source File
# Begin Source File

SOURCE=..\nt_async.h
# End Source File
# Begin Source File

SOURCE=..\nt_comm.h
# End Source File
# Begin Source File

SOURCE=..\nt_pipe.h
# End Source File
# Begin Source File

SOURCE=..\nt_proc.h
# End Source File
# Begin Source File

SOURCE="..\..\Python\CXX-Bindings\CXX\CXX\Objects.hxx"
# End Source File
# Begin Source File

SOURCE=..\os.h
# End Source File
# Begin Source File

SOURCE=..\os_win.h
# End Source File
# Begin Source File

SOURCE=..\queue.h
# End Source File
# Begin Source File

SOURCE=..\resource.h
# End Source File
# Begin Source File

SOURCE=..\search.h
# End Source File
# Begin Source File

SOURCE=..\syntax.h
# End Source File
# Begin Source File

SOURCE=..\undo.h
# End Source File
# Begin Source File

SOURCE=..\win_doc.h
# End Source File
# Begin Source File

SOURCE=..\win_emacs.h
# End Source File
# Begin Source File

SOURCE=..\win_find.h
# End Source File
# Begin Source File

SOURCE=..\win_incl.h
# End Source File
# Begin Source File

SOURCE=..\win_main_frame.h
# End Source File
# Begin Source File

SOURCE=..\win_opt.h
# End Source File
# Begin Source File

SOURCE=..\win_registry.h
# End Source File
# Begin Source File

SOURCE=..\win_select_buffer.h
# End Source File
# Begin Source File

SOURCE=..\win_toolbar.h
# End Source File
# Begin Source File

SOURCE=..\win_view.h
# End Source File
# Begin Source File

SOURCE=..\window.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=..\res\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=..\res\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=..\res\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=..\res\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\BMP16175.BMP
# End Source File
# Begin Source File

SOURCE=..\res\bmp16175.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\BMP24826.BMP
# End Source File
# Begin Source File

SOURCE=..\res\bmp24826.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\BMP28043.BMP
# End Source File
# Begin Source File

SOURCE=..\res\bmp28043.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\editor\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\toolbar_build.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_build.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_CASE.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_case.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_EDIT.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_edit.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_FILE.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_file.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_MACRO.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_macro.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_REGION.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_region.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\toolbar_search.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_search.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\toolbar_tools.bmp
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_tools.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_VIEW.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_view.bmp
# End Source File
# Begin Source File

SOURCE=..\..\..\EDITOR\RES\TOOLBAR_WINDOW.BMP
# End Source File
# Begin Source File

SOURCE=..\res\toolbar_window.bmp
# End Source File
# Begin Source File

SOURCE=..\tt.h
# End Source File
# Begin Source File

SOURCE=..\..\..\editor\res\win_doc.ico
# End Source File
# Begin Source File

SOURCE=..\res\win_doc.ico
# End Source File
# Begin Source File

SOURCE=..\..\..\editor\res\win_emacs.ico
# End Source File
# Begin Source File

SOURCE=..\res\win_emacs.ico
# End Source File
# Begin Source File

SOURCE=..\res\win_emacs.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=..\win_emacs.rgs
# End Source File
# End Target
# End Project
