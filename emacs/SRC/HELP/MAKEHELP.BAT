@echo on
cd \work\emacs\winnt.rel
echo ;empty >emacsini.ml
echo emacsini.ml emacsinit.ml >filename.dat
if "%1" == "" emwinnt /restore=nothing /package=..\src\describe\em_desc
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by emwinnt.HPJ. >emwinnt.hm
echo. >>emwinnt.hm
echo // Commands (ID_* and IDM_*) >>emwinnt.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 ..\src\edit\resource.h >>emwinnt.hm
echo. >>emwinnt.hm
echo // Prompts (IDP_*) >>emwinnt.hm
makehm IDP_,HIDP_,0x30000 ..\src\edit\resource.h >>emwinnt.hm
echo. >>emwinnt.hm
echo // Resources (IDR_*) >>emwinnt.hm
makehm IDR_,HIDR_,0x20000 ..\src\edit\resource.h >>emwinnt.hm
echo. >>emwinnt.hm
echo // Dialogs (IDD_*) >>emwinnt.hm
makehm IDD_,HIDD_,0x20000 ..\src\edit\resource.h >>emwinnt.hm
echo. >>emwinnt.hm
echo // Frame Controls (IDW_*) >>emwinnt.hm
makehm IDW_,HIDW_,0x50000 ..\src\edit\resource.h >>emwinnt.hm
REM -- Make help for Project emwinnt
call hc31 ..\src\help\emwinnt.hpj
echo.
