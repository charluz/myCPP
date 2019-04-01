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
TARGET_EXE	:= nbcam.exe

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


CC_SRCS = nbcam_main.cpp
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

#--- Release Version 
CFLAGS += -DR_VER_AA=$(FWVER_AA)
CFLAGS += -DR_VER_B=$(FWVER_B)
CFLAGS += -DR_VER_C=$(FWVER_C)
CFLAGS += -DR_VER_XX=$(FWVER_XX)
CFLAGS += -DR_VER_YY=$(FWVER_YY)
CFLAGS += -DR_VER_ZZ=$(FWVER_ZZ)


CFLAGS += -DH_VER_C=$(HWVER_C)
CFLAGS += -DH_VER_XX=$(HWVER_XX)
CFLAGS += -DH_VER_YY=$(HWVER_YY)

CFLAGS += -DR_VER_RC=$(RC_VER)


#--- Code/Date Version 
szMK_YEAR=$(shell echo `date +'%-Y'`)
szMK_MONTH=$(shell echo `date +'%-m'`)
szMK_DAY=$(shell echo `date +'%-d'`)
szMK_HH=$(shell echo `date +'%-H'`)
szMK_MM=$(shell echo `date +'%-M'`)
szMK_SS=$(shell echo `date +'%-S'`)

CFLAGS	+= -DMK_YEAR=$(szMK_YEAR)
CFLAGS	+= -DMK_MONTH=$(szMK_MONTH)
CFLAGS	+= -DMK_DAY=$(szMK_DAY)
CFLAGS	+= -DMK_HH=$(szMK_HH)
CFLAGS	+= -DMK_MM=$(szMK_MM)
CFLAGS	+= -DMK_SS=$(szMK_SS)
CFLAGS	+= -DFW_VER_MAJOR=$(B_VER_MAJOR)
CFLAGS	+= -DFW_VER_MINOR=$(B_VER_MINOR)


ifeq ($(RC_VER), 0)
sz_fw_ver = $(shell printf     "Release : MCU_PROXY_DAEMON-V%1d.%02d.%02d FORMAL" $(FWVER_C) $(FWVER_XX) $(FWVER_YY) )
else
sz_fw_ver = $(shell printf     "Release : MCU_PROXY_DAEMON-V%1d.%02d.%02d RC%d" $(FWVER_C) $(FWVER_XX) $(FWVER_YY) $(RC_VER) )
endif

sz_build_time = $(shell printf "Build   : %4d-%2d-%2d %2d:%2d:%2d" $(szMK_YEAR) $(szMK_MONTH) $(szMK_DAY) $(szMK_HH) $(szMK_MM) $(szMK_SS) )

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
	#@echo $(LDFLAGS)
	#@echo $(LDLIBS)
	#@echo $(TOOLCHAIN_CFLAGS)




############################################################
## ISP Daemon 
############################################################
fw: 
	$(MAKE) -s $(TARGET_EXE)
	@$(GCPP) -o $(TARGET_EXE) $(LDFLAGS) $(objects) $(LDLIBS) 
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

