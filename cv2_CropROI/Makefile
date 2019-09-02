##############################################################################
# * Copyright 2007-2009,2011,2012 Sony Corporation.
# * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
# * No part of this file may be copied, modified, sold, and distributed in any
# * form or by any means without prior explicit permission in writing from
# * Sony Corporation.
##############################################################################
pxROOT			= .

#-----------------------------------------------------------
# TOOLCHAIN
#-----------------------------------------------------------
include toolchain_mingw32.mk

#-----------------------------------------------------------
# INSTALL directory
#-----------------------------------------------------------
INSTALL_DIR = .


#-----------------------------------------------------------
# Select TARGET (either)
#-----------------------------------------------------------
TESTAPP_EXE	:=
TARGET_EXE	:= pic_crop.exe

#-----------------------------------------------------------
# Header paths
#-----------------------------------------------------------
include_dirs	:= .


#-----------------------------------------------------------
# Project files
#-----------------------------------------------------------
#include $(pxSCRIPTS)/flist_sdk.inc
#include $(pxSCRIPTS)/flist_app.inc
#include $(pxSCRIPTS)/flist_lib.inc
#include $(pxSCRIPTS)/flist_iqc.inc


CC_SRCS = pic_crop.cpp
#CC_SRCS += $(SDK_SRCS)
#CC_SRCS += $(APP_SRCS)
#CC_SRCS += $(LIB_SRCS)
#CC_SRCS += $(IQC_SRCS)


source_dirs := $(dir $(CC_SRCS))
sources		:= $(CC_SRCS)
objects_cpp := $(notdir $(patsubst %.cpp,%.o,$(sources)))
objects_c	:= $(notdir $(patsubst %.c,%.o,$(sources)))
objects		:= $(objects_cpp) $(objcts_c)

dependencies := $(patsubst %.c,%.d,$(sources))
dependencies := $(patsubst %.cpp,%.d,$(sources))

#-----------------------------------------------------------
## CFLAGS & LDFLAGS
#-----------------------------------------------------------
CFLAGS		= -O2 -Wall
CFLAGS		+= $(TOOLCHAIN_CFLAGS)

CPPFLAGS	= -O2 -Wall
CPPFLAGS	+= $(TOOLCHAIN_CFLAGS)

LDFLAGS		= $(TOOLCHAIN_LDFLAGS)
LDLIBS		= $(TOOLCHAIN_LDLIBS)

vpath %.h $(include_dirs)
vpath %.hpp $(include_dirs)
vpath %.c $(source_dirs) $(include_dirs)
vpath %.cpp $(source_dirs) $(include_dirs)

#-----------------------------------------------------------
## Additional FLASG
#-----------------------------------------------------------
CFLAGS +=
CPPFLAGS +=
LDFLAGS +=
ARFLAGS +=



ifeq "$(PRODUCT)" "DEBUG"
	CFLAGS +=  -g -DDEBUG
endif



############################################################
## Project / Debug Configuration
############################################################



######################################################################
# the following specify the build options for TEST / Verification
# # All options should be commented out for FORMAL Release !!!!!!
# ######################################################################



######################################################################
# the following specify the debug flag while compiling the whole project
# ######################################################################



############################################################
#-----------------------------------------------------------------------
# Specific Compiler/Linker/Archive Settings
#

############################################################
## Object Build
############################################################
%.o: %.c
	@echo $(CC) Compiling $< ...
	@$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	@echo $(GCPP) compiling $< ...
	$(GCPP) $(CFLAGS) $(CPPFLAGS) -c $<


############################################################
## all
############################################################
all:
#	$(MAKE) -s build_info
	$(MAKE) -s fw
#	$(MAKE) -s testapp
#	$(MAKE) -s build_info


$(TARGET_EXE) : $(objects)
#	echo $(objects)


#	@echo $(LDFLAGS)
#	@echo $(LDLIBS)
#	@echo $(TOOLCHAIN_CFLAGS)




############################################################
## ISP Daemon
############################################################
fw:
	$(MAKE) -s $(TARGET_EXE)
	$(GCPP) -o $(TARGET_EXE) $(LDFLAGS) $(objects) $(LDLIBS)
	$(PRINT_GOAL_OK)
	$(PRINT_NEW_LINE)


build_info:
	@echo "*************************************************"
	@echo "* $(sz_fw_ver) "
	@echo "* $(sz_build_time) "
	@echo "*************************************************"



############################################################
## clean
############################################################

clean :
	@rm -f $(TARGET_EXE) $(TESTAPP_EXE)
	@rm -f *.o *.d


############################################################
## depend
############################################################
depend : $(dependencies)



############################################################
## .PHONY
############################################################
.PHONY: all fw debug depend clean build_info install

sinclude $(objects:.o=.d)
