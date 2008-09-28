#
#	Make file for Darwin wxWidgets BEmacs editor component
#

##################################################################################
#                                                                                #
#                                                                                #
#          Settings                                                              #
#                                                                                #
#                                                                                #
##################################################################################
WX_CONFIG=$(HOME)/wxWidgetsDev/wxWidgets-2.8.7/osx-build/wx-config

CC=/usr/bin/gcc -pipe
CXX=/usr/bin/g++ -pipe
cc_flags = -g -D_DEBUG \
 -isysroot /Developer/SDKs/MacOSX10.4u.sdk \
 -I/Developer/SDKs/MacOSX10.4u.sdk/Developer/Headers/FlatCarbon \
 -IInclude/wxWidgets \
 -IInclude/Unix \
 -IInclude/Common \
 `$(WX_CONFIG) --cflags` \
 -c
cpp = $(CXX) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__ -DUSING_CARBON
cc = $(CC) -ansi -pedantic -Wall=pthread
cppfilt=c++filt
ldstatic = $(cpp) -g -static
lddynamic = $(cpp) -g -dynamic -Wl,-syslibroot,/Developer/SDKs/MacOSX10.4u.sdk
edit_obj=darwin/obj/
edit_exe=darwin/exe/

ld_bintools=$(cpp)
bemacs_libs=`$(WX_CONFIG) --libs` 

os_specific_obj_files = \
 $(edit_obj)ptyopen_bsd.o \
 $(edit_obj)emacs_wxwidgets.o

os_specific_client_obj_files =

os_specific_client_ld_options = \
 -framework Carbon

#--------------------------------------------------------------------------------
#
#	Target used by developer
#
#--------------------------------------------------------------------------------
all: dynamic

#--------------------------------------------------------------------------------
#
# target used by ridgebuild
#
#--------------------------------------------------------------------------------
install: install_dynamic_images install_bitmaps install_xkeys

include make-common.mak
