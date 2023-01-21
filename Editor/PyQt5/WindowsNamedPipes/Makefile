#
#	Makefile
#
CCC=cl /nologo
CC=cl /nologo


# /Zi - debug info
# /MT - link with LIBCMT.LIB
# /EHsc - c++ exceptions
# /GS - enable security checks
# /W3 - warning levevl 3
# /Zc:wchar_t - wchar_t is builtin
# /Od - disable optimization
# /sdl - security checks
# /D "NDEBUG" - 
# /D "_CONSOLE" - 
# /D "_UNICODE" - 
# /D "UNICODE" - 
# /WX- - [no] treat warnings as errors
# /Zc:forScope - enforce Standard C++ for scoping rules
# /RTC1 - Enable fast checks (stack and uninit locals)
# /Gd - _cdecl calling convention

COMMON_FLAGS=/Zi /MT /EHsc /GS /W3 /Gd /Zc:wchar_t /Od /sdl /Zc:inline /WX- /Zc:forScope /RTC1 /Iobj
COMMON_DEFINES=/D "NDEBUG" /D "_UNICODE" /D "UNICODE" /D "WIN32"
CLI_FLAGS=$(COMMON_FLAGS) $(COMMON_DEFINES) /D "_CONSOLE" $(EXTRA_FLAGS)
GUI_FLAGS=$(COMMON_FLAGS) $(COMMON_DEFINES) /D "_WINDOWS" $(EXTRA_FLAGS)

LDEXE=$(CCC) /Zi /MT /EHsc
CLI_LIBS=User32.lib
GUI_LIBS=User32.lib

all: obj obj\named-pipe-client.exe obj\named-pipe-server.exe

obj :
	mkdir obj

#
#	Client App
#
obj\named-pipe-client.exe : obj\named-pipe-client.obj
	@echo Link obj\named-pipe-client.exe
	$(LDEXE) $(CLI_FLAGS) /Feobj\named-pipe-client.exe /Fdobj\named-pipe-client.pdb obj\named-pipe-client.obj $(CLI_LIBS)

obj\named-pipe-client.obj : named-pipe-client.cpp
	@echo Compile: named-pipe-client.cpp into obj\named-pipe-client.obj
	$(CCC) $(CLI_FLAGS) /c /Foobj\named-pipe-client.obj /Fdobj\named-pipe-client.pdb named-pipe-client.cpp

clean::
	if exist obj\named-pipe-client.* del obj\named-pipe-client.*

#
#	Server App
#
obj\named-pipe-server.exe : obj\named-pipe-server.obj
	@echo Link obj\named-pipe-server.exe
	$(LDEXE) $(CLI_FLAGS) /Feobj\named-pipe-server.exe /Fdobj\named-pipe-server.pdb obj\named-pipe-server.obj $(CLI_LIBS)

obj\named-pipe-server.obj : named-pipe-server.cpp
	@echo Compile: named-pipe-server.cpp into obj\named-pipe-server.obj
	$(CCC) $(CLI_FLAGS) /c /Foobj\named-pipe-server.obj /Fdobj\named-pipe-server.pdb named-pipe-server.cpp

clean::
	if exist obj\named-pipe-server.* del obj\named-pipe-server.*
