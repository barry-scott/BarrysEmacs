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
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
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
# ADD CPP /nologo /MD /W4 /GX /Zi /O2 /Ob2 /I "..\..\Editor\Include\Common" /I "..\..\Editor\Include\Windows" /I "c:\python25\include" /I "..\..\Python\pycxx-5.4.0" /D "NDEBUG" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python25.lib Delayimp.lib /nologo /subsystem:windows /incremental:yes /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Release\_Lib" /libpath:"..\Python_CXX_Lib\Release" /libpath:"c:\python25\libs" /verbose:lib /delayload:python25.dll
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
# ADD CPP /nologo /MDd /W4 /GX /Zi /Od /I "..\..\Editor\Include\Common" /I "..\..\Editor\Include\Windows" /I "c:\python25\include" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /D "_AFXDLL" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo /o"Debug\editor.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python25_d.lib Delayimp.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Debug\_Lib" /libpath:"..\Python_CXX_Lib\Debug" /verbose:lib /delayload:python25_d.dll
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
# ADD CPP /nologo /MDd /W4 /Gm /GX /Zi /O2 /Ob2 /I "..\..\Editor\Include\Common" /I "..\..\Editor\Include\Windows" /D "_AFXDLL" /D "_WINDOWS" /D "WIN32" /D "_NT" /D "_MBCS" /D "__has_bool__" /FR /Yu"emacs.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo /o"Debug\editor.bsc"
# ADD BSC32 /nologo /o"Debug\editor.bsc"
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 wsock32.lib largeint.lib winmm.lib Python_CXX.lib python25_d.lib Delayimp.lib /nologo /subsystem:windows /profile /debug /machine:I386 /nodefaultlib:"LIBC.lib" /nodefaultlib:"libcd.lib" /libpath:"..\..\Debug\_Lib" /libpath:"..\Python_CXX_Lib\Debug" /verbose:lib /delayload:python25_d.dll

!ENDIF 

# Begin Target

# Name "editor - Win32 Release"
# Name "editor - Win32 Debug"
# Name "editor - Win32 Profile"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=..\Source\Common\abbrev.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\abspath.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\arith.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\buf_man.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\caseconv.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\columns.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\commandline.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\dbman.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\display.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\display_insert_delete.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\display_line.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\doprint.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Common\em_stat.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Common\em_time.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacs.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacs_bemacs_python_module.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Release\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacs_init.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacs_proc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacs_python_interface.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Release\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD CPP /I "..\..\PyCXX" /I "..\..\Debug\_Include" /D "PY_WIN32_DELAYLOAD_PYTHON_DLL"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Common\emacsrtl.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emarray.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emstring.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Common\emstrtab.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\errlog.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\ext_func.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\file_name_compare.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Common\fileio.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\fio.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\function.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\getdb.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\getdirectory.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\getfile.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\glob_var.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\gui_input_mode.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\ICommandLineImpl.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD CPP /Yu

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Common\journal.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\key_names.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\keyboard.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\lispfunc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\macros.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\mem_man.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\metacomm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\minibuf.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\mlispars.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\mlispexp.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\mlisproc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\mlprintf.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\ndbm.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\nt_async.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\nt_comm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\nt_pipe.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\nt_proc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\options.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\queue.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\save_env.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search_extended_algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search_extended_parser.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search_interface.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search_simple_algorithm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\search_simple_engine.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\simpcomm.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\stdafx.cpp
# ADD CPP /Yc"emacs.h"
# End Source File
# Begin Source File

SOURCE=..\Source\Common\string_map.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\subproc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\syntax.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\term.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\timer.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\undo.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\variable.cpp
# ADD CPP /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Common\varthunk.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_com_support.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_doc.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_emacs.cpp

!IF  "$(CFG)" == "editor - Win32 Release"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# SUBTRACT CPP /YX /Yc /Yu

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_emacs.odl

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD MTL /tlb "..\Include\Windows\ExternalInclude\BarrysEmacs.tlb" /h "..\Include\Windows\ExternalInclude\BarrysEmacs.h" /iid "..\Include\Windows\ExternalInclude\BarrysEmacsUUID.c"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD MTL /tlb "..\Include\Windows\ExternalInclude\BarrysEmacs.tlb" /h "..\Include\Windows\ExternalInclude\BarrysEmacs.h" /iid "..\Include\Windows\ExternalInclude\BarrysEmacsUUID.c"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_emacs.rc

!IF  "$(CFG)" == "editor - Win32 Release"

# ADD BASE RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Release" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"
# ADD RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Release" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"

!ELSEIF  "$(CFG)" == "editor - Win32 Debug"

# ADD BASE RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Debug" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"
# ADD RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Debug" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"

!ELSEIF  "$(CFG)" == "editor - Win32 Profile"

# ADD BASE RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Profile" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"
# ADD RSC /l 0x809 /i "\wc\Emacs\Editor\Source\Windows" /i ".\Profile" /i ".." /i "\Latest\EmacsV7\Editor" /i "\Stable\EmacsV7\Editor" /i "..\..\Editor\Include\Common" /i "..\..\Editor\Include\Windows"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_ext_func.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_file.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_find.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_list_ctrl_ex.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_main_frame.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_menu.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_opt.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_registry.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_rtl.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_select_buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_toolbar.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_ui.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\win_view.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\windman.cpp
# End Source File
# Begin Source File

SOURCE=..\Source\Common\window.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=..\Include\Common\abbrev.h
# End Source File
# Begin Source File

SOURCE="..\..\PyCXX\CXX\Array.hxx"
# End Source File
# Begin Source File

SOURCE=..\Include\Common\buffer.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\commandline.h
# End Source File
# Begin Source File

SOURCE="..\..\PyCXX\CXX\Config.hxx"
# End Source File
# Begin Source File

SOURCE=..\Include\Common\display.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_gen.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_mac.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_rtn.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_stat.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_time.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_user.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_util.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\em_var.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emacs.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emacs_proc.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emacs_python_interface.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emacsutl.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emexcept.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emobject.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emstring.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\emstrtab.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\errlog.h
# End Source File
# Begin Source File

SOURCE="..\..\PyCXX\CXX\Exception.hxx"
# End Source File
# Begin Source File

SOURCE="..\..\PyCXX\CXX\Extensions.hxx"
# End Source File
# Begin Source File

SOURCE=..\Include\Common\fileio.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\fileserv.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\getdb.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\getdirectory.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\getfile.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\ICommandLineImpl.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\journal.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\key_names.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\keyboard.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\mem_man.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\mlisp.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\mlispexp.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\ndbm.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\nt_async.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\nt_comm.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\nt_pipe.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\nt_proc.h
# End Source File
# Begin Source File

SOURCE="..\..\PyCXX\CXX\Objects.hxx"
# End Source File
# Begin Source File

SOURCE=..\Include\Common\os.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\os_win.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\queue.h
# End Source File
# Begin Source File

SOURCE=..\Include\Windows\resource.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\search.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\syntax.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\undo.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_doc.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_emacs.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_find.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_incl.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_main_frame.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_opt.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_registry.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_select_buffer.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_toolbar.h
# End Source File
# Begin Source File

SOURCE=..\Include\windows\win_view.h
# End Source File
# Begin Source File

SOURCE=..\Include\Common\window.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp00006.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp00007.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\bmp16175.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\BMP24826.BMP
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\BMP28043.BMP
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\idr_main.ico
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar1.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar2.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_build.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_case.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_edit.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_file.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_macro.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_region.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_search.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_tools.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_view.bmp
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\toolbar_window.bmp
# End Source File
# Begin Source File

SOURCE=..\Include\Common\tt.h
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\win_doc.ico
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\win_emacs.ico
# End Source File
# Begin Source File

SOURCE=..\Source\Windows\Resources\win_emacs.rc2
# End Source File
# End Group
# Begin Source File

SOURCE=..\Source\Windows\Resources\win_emacs.rgs
# End Source File
# End Target
# End Project
