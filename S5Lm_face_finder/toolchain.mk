#**********************************************************************
#
# Author : Charles Wu
#**********************************************************************
MY_HOME			= /home/charles
SoC_SDK_ROOT	= $(MY_HOME)/workbench/amba/esmt_linux_sdk
SDK_AMBA_ROOT	= $(SoC_SDK_ROOT)/ambarella
TOPDIR 			= $(shell pwd)/..


#*************  TOOLCHAIN  header   *************
TOOLCHAIN_INCLUDE	:= $(CPP_INCLUDE_DIR)
TOOLCHAIN_INCLUDE	+= $(CPP_INCLUDE_DIR_TOOLCHAIN)

# for I2C-dev
TOOLCHAIN_INCLUDE	+= $(SDK_AMBA_ROOT)/unit_test/linux/i2ctools/include


#*************  TOOLCHAIN  lib   *************
TOOLCHAIN_LIB =
TOOLCHAIN_LIB += $(SYS_LIB_DIR_AARCH64)
TOOLCHAIN_LIB += $(GLIBC_LOCALE_PATH)
TOOLCHAIN_LIB += $(GLIBC_GCONV_PATH_AARCH64)

TOOLCHAIN_LDLIBRARY	=

TOOLCHAIN_LDFLAGS	= 

#CROSS ?= none
export CROSS := aarch64-linux-gnu-

# C -compiler name, can be replaced by another compiler(replace gcc)
export CC = $(CROSS)gcc
export STRIP = $(CROSS)strip
export LD = $(CROSS)ld
export AR = $(CROSS)ar
export RANLIB = $(CROSS)ranlib


# MACRO for cleaning object -files
export RM := rm -rf
export COPY := cp
export CHMOD := chmod

#--- SHELL settings
ECHO := @echo
PRINT_NEW_LINE = @$(ECHO) ""
PRINT_GOAL_OK = @$(ECHO) "*****  built successfully! ******"
PRINT_INSTALL_OK = @$(ECHO) "*****  install successfully! ******"

