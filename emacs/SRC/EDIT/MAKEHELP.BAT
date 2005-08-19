@echo off
REM -- First make map file from App Studio generated resource.h
echo // MAKEHELP.BAT generated Help Map file.  Used by emwinnt.HPJ. >hlp\emwinnt.hm
echo. >>hlp\emwinnt.hm
echo // Commands (ID_* and IDM_*) >>hlp\emwinnt.hm
makehm ID_,HID_,0x10000 IDM_,HIDM_,0x10000 resource.h >>hlp\emwinnt.hm
echo. >>hlp\emwinnt.hm
echo // Prompts (IDP_*) >>hlp\emwinnt.hm
makehm IDP_,HIDP_,0x30000 resource.h >>hlp\emwinnt.hm
echo. >>hlp\emwinnt.hm
echo // Resources (IDR_*) >>hlp\emwinnt.hm
makehm IDR_,HIDR_,0x20000 resource.h >>hlp\emwinnt.hm
echo. >>hlp\emwinnt.hm
echo // Dialogs (IDD_*) >>hlp\emwinnt.hm
makehm IDD_,HIDD_,0x20000 resource.h >>hlp\emwinnt.hm
echo. >>hlp\emwinnt.hm
echo // Frame Controls (IDW_*) >>hlp\emwinnt.hm
makehm IDW_,HIDW_,0x50000 resource.h >>hlp\emwinnt.hm
REM -- Make help for Project emwinnt
call hc31 emwinnt.hpj
echo.
