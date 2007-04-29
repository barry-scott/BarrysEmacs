#
#	Make file for Darwin BEmacs editor component
#

##################################################################################
#                                                                                #
#                                                                                #
#          Settings                                                              #
#                                                                                #
#                                                                                #
##################################################################################
MOTIF_TOP=$(BUILDER_TOP_DIR)/Imports/openmotif-2.2.3-universal

CC=/usr/bin/gcc -pipe
CXX=/usr/bin/g++ -pipe
cc_flags = -g -D_DEBUG -IInclude/Unix -IInclude/Common -I/usr/X11R6/include -I$(MOTIF_TOP)/include -c
cpp = $(CXX) -ansi -pedantic -Wall -Wno-long-long -frtti -D__std_cpp__ -D__has_bool__
cc = $(CC) -ansi -pedantic -Wall=pthread
cppfilt=c++filt
ldstatic = $(cpp) -g -static -L/usr/X11R6/lib -L$(MOTIF_TOP)/lib
lddynamic = $(cpp) -g -dynamic -L/usr/X11R6/lib -L$(MOTIF_TOP)/lib
edit_obj=darwin/obj/
edit_exe=darwin/exe/

ld_bintools=$(cpp)
bemacs_libs=-lX11 -lXm -lXt -lXmu -lSM -lICE -lXext -lXp -lX11

os_specific_obj_files = \
 $(edit_obj)ptyopen_bsd.o

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
