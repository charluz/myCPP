##############################################################################
# * Copyright 2007-2009,2011,2012 Sony Corporation.
# * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Sony Corporation.
# * No part of this file may be copied, modified, sold, and distributed in any
# * form or by any means without prior explicit permission in writing from
# * Sony Corporation.
##############################################################################


#-----------------------------------------------------------
# TOOLCHAIN 
#-----------------------------------------------------------
include toolchain.mk
include toolchain_opencv.mk

#-----------------------------------------------------------
# The following path will be defined in toolchain.mk
# SoC_SDK_ROOT		-- AmbaSDK
# SDK_AMBA_ROOT		-- AmbaSDK/ambarella
#-----------------------------------------------------------

pxROOT				= .
pxAmbaLinuxSDK 		= $(SoC_SDK_ROOT)/ambarella
pxAmbaOut			= $(pxAmbaLinuxSDK)/out
pxElektraFakeroot	= $(pxAmbaOut)/s5lm_elektra_v/fakeroot



#-----------------------------------------------------------
# INSTALL directory
#-----------------------------------------------------------
#INSTALL_DIR = $(pxElektraFakeroot)/usr/local/bin
#INSTALL_DIR_LIB = ../lib


#-----------------------------------------------------------
# Select TARGET (either)
#-----------------------------------------------------------
#TESTAPP_EXE := testcv
TARGET_EXE := smaple_ycap
TARGET_EXE_PMS := 755

ROOTFS_EXE_DIR := $(pxElektraFakeroot)/usr/local/bin

#-----------------------------------------------------------
# Header and Library paths
#-----------------------------------------------------------
include_dirs	:= .
include_dirs	+= $(TOOLCHAIN_INCLUDE)
include_dirs	+= $(SDK_AMBA_ROOT)/include 

library_dirs := .
library_dirs += $(TOOLCHAIN_LIB)

library_files :=
library_files += $(TOOLCHAIN_LDLIBRARY)

#-----------------------------------------------------------
# Project files
#-----------------------------------------------------------
#include $(pxSCRIPTS)/flist_sdk.inc
#include $(pxSCRIPTS)/flist_app.inc
#include $(pxSCRIPTS)/flist_lib.inc
#include $(pxSCRIPTS)/flist_iqc.inc


##############################################################
# DIR and Name for FW Release
#
CC_SRCS	= sample_ycap.cpp
CC_SRCS += sample_osd.cpp

#CC_SRCS += $(SDK_SRCS)
#CC_SRCS += $(APP_SRCS)
#CC_SRCS += $(LIB_SRCS)
#CC_SRCS += $(IQC_SRCS)


##########################################
sources = $(CC_SRCS)
#objects := $(notdir $(patsubst %.c,%.o,$(sources)))
objects += $(notdir $(patsubst %.cpp,%.o,$(sources)))
dependencies := $(patsubst %.c,%.d,$(sources))

CFLAGS += -Wall
CFLAGS += $(addprefix -I ,$(include_dirs))
CPPFLAGS += $(addprefix -I ,$(include_dirs))

#-----------------------------------------------------------
## Additional FLASG
#-----------------------------------------------------------
CFLAGS		+=
CPPFLAGS	+=
LDFLAGS		+= $(TOOLCHAIN_LDFLAGS)
ARFLAGS		+=


LDLIBS		+= $(addprefix -L,$(library_dirs))

LDLIBRARY	+= -lpthread -lstdc++
LDLIBRARY	+= -lm
LDLIBRARY	+= $(addprefix -l,$(library_files))

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
#ISP_CFLAGS	+= _D_Bypass_ManualAE_IQ_	<-- Comment out for Formal Release


######################################################################
# Specific Compiler/Linker/Archive Settings
#

sz_build_time = $(shell printf "Build   : %4d-%2d-%2d %2d:%2d:%2d" $(szMK_YEAR) $(szMK_MONTH) $(szMK_DAY) $(szMK_HH) $(szMK_MM) $(szMK_SS) )

############################################################
## Object Build
############################################################
%.o: %.c 
	@$(CC) $(CFLAGS) -MM -MT $@ -o $(@:.o=.d) $<
	@$(CC) $(CFLAGS) -c $<

%.o: %.cpp
	@$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

############################################################
## all
############################################################
all:
	$(MAKE) -s fw


$(TARGET_EXE) : $(objects) $(make_rules)


############################################################
## ISP Daemon 
############################################################
fw: 
	find . -type f -name "*.d" | xargs rm -f
	echo '>>>>>>'
	$(MAKE) -s $(TARGET_EXE)
	echo '++++++'
	@$(CC) $(LDFLAGS) $(LDLIBS) $(objects) $(LDLIBRARY) -o $(TARGET_EXE)
	$(PRINT_GOAL_OK)
	$(PRINT_NEW_LINE)



build_info:	
	@echo "*************************************************"
	@echo "* $(sz_fw_ver) "
	@echo "* $(sz_build_time) "
	@echo "*************************************************"


debugmkfile:
	@echo ">>> " $(objects) " <<<"
	@echo ">>> " $(LDFLAGS) " <<<"
	@echo ">>> " $(LDLIBRARY) " <<<"
	@echo "+++ " $(library_dirs) " +++"
	@echo "+++ " $(include_dirs) " +++"
		
############################################################
## cp_nfs
############################################################
cp_nfs:
	@mkdir -p $(NFS_DIR)
#	@########## Copy PT-MCU files ####################
#	@cp -f $(pxPTMCU_UPGRADE)/sim3c166.bin $(pxPTMCU_UPGRADE)/sim3c166.txt $(NFS_DIR)
#	@########## Copy ISP-FW files ####################
#	@cp -f isp isp.sh $(NFS_DIR)
#	@cp -f res/I80_PROF.BIN res/I80_CALIB.BIN res/isp_conf.ini $(NFS_DIR)
#	@########## Copy ISP-UTIL files ####################
#	@cp -f $(pxUTIL)/isp-ctrl/isp-ctrl $(NFS_DIR)
#	@cp -f $(pxUTIL)/isp-ctrl/ispctrl_alias.sh $(NFS_DIR)
#	@cp -f $(pxUTIL)/run-hb/*.scr  $(NFS_DIR)
#	@cp -f $(pxUTIL)/run-hb/hlink-bench  $(NFS_DIR)
#	@cp -f $(pxUTIL)/zlink-ctrl/zlink-ctrl  $(NFS_DIR)
#	@cp -f $(pxUTIL)/ptlink-ctrl/ptlink-ctrl  $(NFS_DIR)
#	@########## Copy ISP-DRV files ####################
#	@cp -f $(pxISPDRV)/zlink/zlink.ko $(NFS_DIR)
#	@cp -f $(pxISPDRV)/ptlink/ptlink.ko $(NFS_DIR)
#	@########## Copy IQ 3DLut BIN files #################
#	@mkdir -p $(NFS_DIR)/3dlut
#	@cp -f $(pxSDK)/3dlut/s2/* $(NFS_DIR)/3dlut
#	@########## Copy IQ ADJ BIN files ###################
#	@mkdir -p $(NFS_DIR)/adj
#	@cp -f $(pxBIN)/IQ-Adj.BIN/* $(NFS_DIR)/adj




############################################################
## install
############################################################

install : $(TARGET_EXE)
	@mkdir -p $(ROOTFS_EXE_DIR)
	@mkdir -p $(ROOTFS_EXE_DIR)/haarcascades
	$(RM) $(ROOTFS_EXE_DIR)/$(TARGET_EXE)
	$(RM) $(ROOTFS_EXE_DIR)/haarcascades/*
	$(COPY) -f $(TARGET_EXE) XD2.jpg $(ROOTFS_EXE_DIR)
#	$(COPY) -f ./haarcascades/haarcascade_frontalface_default.xml $(ROOTFS_EXE_DIR)/haarcascades
	$(COPY) -f ./haarcascades/*.xml $(ROOTFS_EXE_DIR)/haarcascades
	$(CHMOD) 775 $(ROOTFS_EXE_DIR)/$(TARGET_EXE)
	$(CHMOD) 775 -R $(ROOTFS_EXE_DIR)/haarcascades
	$(PRINT_INSTALL_OK)
	$(PRINT_NEW_LINE)




############################################################
## clean
############################################################

clean:
	find . -type f -name "*.o" | xargs rm -f
	find . -type f -name "*.d" | xargs rm -f
	@$(RM) $(TARGET_EXE)
#	$(RM) $(TARGET_PATH) $(objects)
#	@find . -type f -name "*.BIN" | xargs rm -f
#	@find . -type f -name "tmp_raw.dat" | xargs rm -f
#	@$(MAKE) -C $(pxUTIL)/run-hb clean
#	@$(MAKE) -C $(pxUTIL)/isp-ctrl clean
#	@rm -fr ./download/*
	

############################################################
## depend
############################################################
depend : $(dependencies)



############################################################
## .PHONY
############################################################
.PHONY: debugmkfile fw debug depend clean all build_info install

sinclude $(objects:.o=.d)

