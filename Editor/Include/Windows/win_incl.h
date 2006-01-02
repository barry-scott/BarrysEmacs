//
//    win_incl.h
//        Copyright 1993 Barry A. Scott
//

// disable level 4 warnings that Microsoft code raises
#pragma warning( disable : 4214 4201)
#ifndef STRICT
# define STRICT
#endif
# include <windows.h>
# include <windowsx.h>
# include <commdlg.h>
// back to full level 4
#pragma warning( default : 4214 4201)
