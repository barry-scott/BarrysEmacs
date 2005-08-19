#
#	Make file for OpenBSD BEmacs editor component
#

##################################################################################
#                                                                                #
#                                                                                #
#          Settings                                                              #
#                                                                                #
#                                                                                #
##################################################################################
CC=/usr/bin/gcc -pipe
CXX=/usr/bin/g++ -pipe
cc_flags = -g -D_DEBUG -DOLD_STDCXX -IInclude/Unix -IInclude/Common -I/usr/X11R6/include -I/usr/local/include -c
cpp = $(CXX) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__
cc = $(CC) -ansi -pedantic -Wall=pthread
cppfilt=c++filt
ldstatic = $(cpp) -g -static -L/usr/X11R6/lib -L/usr/local/lib
lddynamic = $(cpp) -g -dynamic -L/usr/X11R6/lib -L/usr/local/lib
edit_obj=openbsd/obj/
edit_exe=openbsd/exe/

bemacs_libs=-lXm -lXp -lXt -lXmu -lSM -lICE -lXext -lX11

ld_bintools=$(cpp)

os_specific_obj_files = \
 $(edit_obj)ptyopen_bsd.o

#--------------------------------------------------------------------------------
#
#	Target used by developer
#
#--------------------------------------------------------------------------------
all: static

#--------------------------------------------------------------------------------
#
# target used by ridgebuild
#
#--------------------------------------------------------------------------------
install: install_static_images install_bitmaps install_xkeys

include make-common.mak
