#
#	makefile for bemacs
#	starts in .\_build
#
#	Can build Win32, Linux and FreeBSD kits
#

build:
	$(MAKE) -f $(CFG2).mak CFG=$(CFG) TARGET_OS=$(CFG2) build


# SourceSafe database & project
SSDB=	    Emacs
SSPROJ=	    EmacsV7

merge-all:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ) /merge=inter

merge-editor:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ)/editor /merge=inter

merge-mlisp:
	ridgeuser /database=$(SSDB) /project=$(SSPROJ)/mlisp /merge=inter

get: get-Release

get-Release:
	for %i in (..) do ridgebuild /fetch=depend /work=%~fi /project=$$/Latest/$(SSPROJ) /database=$(SSDB) /cfg=Release+Win32 /latest

get-Debug:
	for %i in (..) do ridgebuild /fetch=depend /work=%~fi /project=$$/Latest/$(SSPROJ) /database=$(SSDB) /cfg=Debug+Win32 /latest

