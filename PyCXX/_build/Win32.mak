#
#	makefile for bemacs
#	starts in .\_build
#
#	Can build win32, linux and freebsd kits
#

#################################################################################
#
#
#	Win32 build
#
#
#################################################################################

# build windows kit
build:
	@echo Info: Build Editor $(CFG)
	cd ..\CXX && msdev example_py21.dsp /MAKE "example - Win32 $(CFG)"
