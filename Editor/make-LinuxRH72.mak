#
#	Make file for Linux BEmacs editor component
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

MOTIF=/opt/motif21
MOTIFINC=$(MOTIF)/include
MOTIFLIB=$(MOTIF)/lib

cc_flags = -g -D_DEBUG -DOLD_STDCXX -D_BSD_SOURCE -D_XOPEN_SOURCE -D_POSIX_C_SOURCE=199506L -IInclude/Unix -IInclude/Common -I/usr/X11R6/include -I$(MOTIFINC) -c
cpp = $(CXX) -ansi -pedantic -Wall -frtti -fexceptions -D__std_cpp__ -D__has_bool__
cc = $(CC) -ansi -pedantic -Wall
cppfilt=c++filt
ldstatic = $(cpp) -g -L/usr/X11R6/lib -L$(MOTIFLIB)
lddynamic = $(cpp) -g -L/usr/X11R6/lib -L$(MOTIFLIB)
edit_obj=linux/obj/
edit_exe=linux/exe/
bemacs_libs=-lXm -lXp -lXt -lXmu -lSM -lICE -lXext -lX11

os_specific_obj_files = \
 $(edit_obj)ptyopen_bsd.o \
 $(edit_obj)linux_ld_fix.o 

#ld_bintools=$(ldstatic) -Wl,-Bstatic
ld_bintools=$(ldstatic)

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
