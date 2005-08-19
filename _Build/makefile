#
#	makefile for bemacs
#	starts in .\_build
#
#	Can build Win32, Linux and FreeBSD kits
#

build:
	$(MAKE) -f $(RB_CFG_PLATFORM).mak build

clean:
	$(MAKE) -f $(RB_CFG_PLATFORM).mak clean


# SourceSafe database & project
SSDB=	    Emacs
SSPROJ=	    V7.1/Emacs

merge-all:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ) /merge=inter

merge-editor:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ)/editor /merge=inter

merge-mlisp:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ)/mlisp /merge=inter

get: get-Release

get-Release:
	for %i in (..) do ridgebuild /fetch=depend /work=%~fi /project=$$/Latest/$(SSPROJ) /database=$(SSDB) /cfg=BUILD_TYPE=Release+PLATFORM=Win32 /latest

get-Debug:
	for %i in (..) do ridgebuild /fetch=depend /work=%~fi /project=$$/Latest/$(SSPROJ) /database=$(SSDB) /cfg=BUILD_TYPE=Debug+PLATFORM=Win32 /latest

